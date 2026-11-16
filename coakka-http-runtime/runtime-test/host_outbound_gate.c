#include "test_support.h"

#include <stdlib.h>
#include <string.h>

static int port_number(const char *value, uint16_t *port) {
  char *end = NULL;
  const unsigned long parsed = strtoul(value, &end, 10);
  if (value[0] == '\0' || *end != '\0' || parsed == 0UL || parsed > UINT16_MAX)
    return 0;
  *port = (uint16_t)parsed;
  return 1;
}

static int load_pem(const char *path, uint8_t *data, size_t cap,
                    size_t *count) {
  FILE *file = NULL;
#if defined(_WIN32)
  if (fopen_s(&file, path, "rb") != 0) return 0;
#else
  file = fopen(path, "rb");
#endif
  if (file == NULL)
    return 0;
  *count = fread(data, 1U, cap, file);
  const int ok = *count != 0U && feof(file) && !ferror(file);
  return fclose(file) == 0 && ok;
}

static int check_call(test_service_t *owner, const char *name, const char *path,
                      coakka_http_host_outbound_reason_t reason,
                      uint32_t timeout, char *first_port,
                      size_t first_port_size) {
  int ok = 1;
  int lease = 0;
  coakka_http_host_outbound_call_t id;
  coakka_http_host_outbound_request_t request;
  coakka_http_host_outbound_event_t event;
  coakka_http_host_outbound_request_init(&request);
  coakka_http_host_outbound_event_init(&event);
  request.logical_target = test_text(name);
  request.method = test_text("POST");
  request.target = test_text(path);
  request.body = test_text("outbound-payload");
  request.timeout_ms = timeout;
  coakka_http_host_header_t header = {test_text("x-fixture"),
                                      test_text("native")};
  request.headers = &header;
  request.header_count = 1U;
  TEST_CHECK(coakka_http_host_submit_outbound(owner->service, &request, &id) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(coakka_http_host_take_outbound(owner->service, 5000U, &event) ==
             COAKKA_HTTP_HOST_OK);
  lease = 1;
  printf("OUTBOUND %s %s reason=%u phase=%u code=%d\n", name, path,
         (unsigned)event.reason, (unsigned)event.phase,
         (int)event.provider_code);
  TEST_CHECK(event.call.slot == id.slot &&
             event.call.generation == id.generation);
  TEST_CHECK(event.reason == reason &&
             test_bytes_equal(event.logical_target, name) &&
             event.target_generation == 7U &&
             test_bytes_equal(event.selected_node_id, name));
  if (reason == COAKKA_HTTP_HOST_OUTBOUND_RESPONSE) {
    TEST_CHECK(event.certainty == COAKKA_HTTP_HOST_OUTBOUND_COMPLETE_RESPONSE &&
               event.response_status == 201U &&
               test_bytes_equal(event.response_body, "outbound-payload"));
    int port_seen = 0;
    for (uint32_t i = 0U; i < event.response_header_count; ++i) {
      coakka_http_host_header_t field;
      TEST_CHECK(
          coakka_http_host_outbound_header(owner->service, &event, i, &field));
      if (test_bytes_equal(field.name, "x-peer-port")) {
        TEST_CHECK(!port_seen && field.value.size > 0U &&
                   field.value.size < first_port_size);
        port_seen = 1;
        if (*first_port == '\0') {
          memcpy(first_port, field.value.data, field.value.size);
          first_port[field.value.size] = '\0';
        } else {
          /* The installed host ABI selects PER_EXCHANGE. A persistent
           peer makes accidental reuse observable on the second call. */
          TEST_CHECK(!test_bytes_equal(field.value, first_port));
        }
      }
    }
    TEST_CHECK(port_seen);
  } else if (reason == COAKKA_HTTP_HOST_OUTBOUND_PROVIDER_FAILURE) {
    TEST_CHECK(event.phase == COAKKA_HTTP_HOST_OUTBOUND_PHASE_TRANSPORT &&
               event.certainty ==
                   COAKKA_HTTP_HOST_OUTBOUND_NO_REQUEST_BYTES_SENT);
  }
  TEST_CHECK(coakka_http_host_release_outbound(owner->service, &event) ==
             COAKKA_HTTP_HOST_OK);
  coakka_http_host_outbound_event_init(&event);
  const coakka_http_host_status_t next =
      coakka_http_host_take_outbound(owner->service, 0U, &event);
  lease = next == COAKKA_HTTP_HOST_OK;
  TEST_CHECK(next == COAKKA_HTTP_HOST_TIMEOUT);
cleanup:
  if (lease && coakka_http_host_release_outbound(owner->service, &event) !=
                   COAKKA_HTTP_HOST_OK)
    ok = 0;
  return ok;
}

static int check_response_boundary(test_service_t *owner, const char *mode) {
  int ok = 1, lease = 0;
  coakka_http_host_outbound_reason_t expected;
  const char *path;
  const size_t body_limit = 256U * 1024U;
  if (strcmp(mode, "short") == 0) {
    path = "/short";
    expected = COAKKA_HTTP_HOST_OUTBOUND_PROVIDER_FAILURE;
  } else if (strcmp(mode, "large-head") == 0) {
    path = "/large-head";
    expected = COAKKA_HTTP_HOST_OUTBOUND_RESPONSE_HEAD_LIMIT;
  } else if (strcmp(mode, "large-body") == 0) {
    path = "/large-body";
    expected = COAKKA_HTTP_HOST_OUTBOUND_RESPONSE_BODY_LIMIT;
  } else if (strcmp(mode, "large-declared-body") == 0) {
    path = "/large-declared-body";
    expected = COAKKA_HTTP_HOST_OUTBOUND_RESPONSE_BODY_LIMIT;
  } else if (strcmp(mode, "body-boundary") == 0) {
    path = "/body-boundary";
    expected = COAKKA_HTTP_HOST_OUTBOUND_RESPONSE;
  } else {
    return 0;
  }
  coakka_http_host_outbound_request_t request;
  coakka_http_host_outbound_request_init(&request);
  request.logical_target = test_text("plain");
  request.method = test_text("GET");
  request.target = test_text(path);
  request.timeout_ms = 5000U;
  coakka_http_host_outbound_call_t call;
  TEST_CHECK(coakka_http_host_submit_outbound(owner->service, &request, &call) ==
             COAKKA_HTTP_HOST_OK);
  coakka_http_host_outbound_event_t result;
  coakka_http_host_outbound_event_init(&result);
  TEST_CHECK(coakka_http_host_take_outbound(owner->service, 5000U, &result) ==
             COAKKA_HTTP_HOST_OK);
  lease = 1;
  printf("OUTBOUND %s reason=%u expected=%u provider=%d phase=%u certainty=%u retry=%u\n", path,
         (unsigned)result.reason, (unsigned)expected,
         (int)result.provider_code, (unsigned)result.phase,
         (unsigned)result.certainty, (unsigned)result.retry);
  TEST_CHECK(result.call.slot == call.slot && result.call.generation == call.generation);
  const coakka_http_host_outbound_reason_t actual = result.reason;
  if (expected == COAKKA_HTTP_HOST_OUTBOUND_RESPONSE) {
    TEST_CHECK(actual == expected && result.response_status == 200U &&
               result.response_body.size == body_limit &&
               result.certainty == COAKKA_HTTP_HOST_OUTBOUND_COMPLETE_RESPONSE);
    for (size_t i = 0U; i < body_limit; ++i)
      TEST_CHECK(result.response_body.data[i] == (uint8_t)'x');
  } else {
    TEST_CHECK(result.response_status == 0U && result.response_body.size == 0U &&
               result.response_header_count == 0U);
    // Refusing an oversized field precedes receipt of the complete head.
    const coakka_http_host_outbound_certainty_t certainty =
        expected == COAKKA_HTTP_HOST_OUTBOUND_RESPONSE_HEAD_LIMIT
            ? COAKKA_HTTP_HOST_OUTBOUND_REQUEST_UNCERTAIN
            : COAKKA_HTTP_HOST_OUTBOUND_RESPONSE_HEAD_OBSERVED;
    TEST_CHECK(result.certainty == certainty &&
               result.retry == COAKKA_HTTP_HOST_OUTBOUND_RETRY_DELIVERY_UNCERTAIN);
  }
  const coakka_http_host_outbound_phase_t phase = result.phase;
  TEST_CHECK(coakka_http_host_release_outbound(owner->service, &result) == COAKKA_HTTP_HOST_OK);
  lease = 0;
  char peer_port[8] = {0};
  TEST_CHECK(check_call(owner, "plain", "/echo", COAKKA_HTTP_HOST_OUTBOUND_RESPONSE,
                        3000U, peer_port, sizeof(peer_port)));
  TEST_CHECK(actual == expected);
  TEST_CHECK(phase == (expected == COAKKA_HTTP_HOST_OUTBOUND_PROVIDER_FAILURE
                           ? COAKKA_HTTP_HOST_OUTBOUND_PHASE_TRANSPORT
                           : COAKKA_HTTP_HOST_OUTBOUND_PHASE_RESPONSE));
cleanup:
  if (lease && coakka_http_host_release_outbound(owner->service, &result) !=
                   COAKKA_HTTP_HOST_OK)
    ok = 0;
  return ok;
}

int main(int argc, char **argv) {
  uint16_t plain = 0U, secure = 0U;
  uint8_t certificate[8192];
  size_t size = 0U;
  if ((argc != 4 && argc != 5) || !port_number(argv[1], &plain) ||
      !port_number(argv[2], &secure) ||
      !load_pem(argv[3], certificate, sizeof(certificate), &size) ||
      !test_platform_open())
    return EXIT_FAILURE;
  int ok = 1;
  test_service_t owner;
  test_service_init(&owner);
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text("127.0.0.1");
  coakka_http_host_outbound_trust_t trust;
  coakka_http_host_outbound_trust_init(&trust);
  trust.generation = 1U;
  trust.ca_pem = (coakka_http_host_bytes_t){certificate, size};
  config.outbound_trusts = &trust;
  config.outbound_trust_count = 1U;
  const char *const names[] = {"plain", "secure", "wrong-name"};
  coakka_http_host_outbound_endpoint_t endpoints[3];
  coakka_http_host_outbound_target_t targets[3];
  for (uint32_t i = 0; i < 3U; ++i) {
    coakka_http_host_outbound_endpoint_init(&endpoints[i]);
    endpoints[i].node_id = test_text(names[i]);
    endpoints[i].connect_host = test_text("127.0.0.1");
    endpoints[i].connect_port = i == 0U ? plain : secure;
    endpoints[i].http_authority = test_text("localhost");
    endpoints[i].available = 1U;
    if (i != 0U) {
      endpoints[i].security = COAKKA_HTTP_HOST_SECURITY_TLS;
      endpoints[i].tls_peer_identity =
          test_text(i == 1U ? "localhost" : "wrong.test");
      endpoints[i].tls_trust_generation = trust.generation;
    }
    coakka_http_host_outbound_target_init(&targets[i]);
    targets[i].name = test_text(names[i]);
    targets[i].generation = 7U;
    targets[i].endpoints = &endpoints[i];
    targets[i].endpoint_count = 1U;
  }
  config.outbound_targets = targets;
  config.outbound_target_count = 3U;
  TEST_CHECK(coakka_http_host_abi_version() == 10U);
  TEST_CHECK(test_service_open(&owner, &config, NULL, 0U));
  if (argc == 5) {
    TEST_CHECK(check_response_boundary(&owner, argv[4]));
    goto cleanup;
  }
  coakka_http_host_outbound_request_t invalid;
  coakka_http_host_outbound_request_init(&invalid);
  invalid.logical_target = test_text("missing");
  invalid.method = test_text("GET");
  invalid.target = test_text("/echo");
  invalid.timeout_ms = 1000U;
  coakka_http_host_outbound_call_t sentinel = {123U, 456U};
  const coakka_http_host_status_t missing =
      coakka_http_host_submit_outbound(owner.service, &invalid, &sentinel);
  printf("OUTBOUND unknown target status=%s\n",
         coakka_http_host_status_name(missing));
  TEST_CHECK(missing == COAKKA_HTTP_HOST_NOT_FOUND);
  TEST_CHECK(sentinel.slot == 123U && sentinel.generation == 456U);

  char peer_port[8] = {0};
  TEST_CHECK(check_call(&owner, "plain", "/echo",
                        COAKKA_HTTP_HOST_OUTBOUND_RESPONSE, 3000U, peer_port,
                        sizeof(peer_port)));
  TEST_CHECK(check_call(&owner, "plain", "/echo",
                        COAKKA_HTTP_HOST_OUTBOUND_RESPONSE, 3000U, peer_port,
                        sizeof(peer_port)));
  peer_port[0] = '\0';
  TEST_CHECK(check_call(&owner, "secure", "/echo",
                        COAKKA_HTTP_HOST_OUTBOUND_RESPONSE, 3000U, peer_port,
                        sizeof(peer_port)));
  TEST_CHECK(check_call(&owner, "wrong-name", "/echo",
                        COAKKA_HTTP_HOST_OUTBOUND_PROVIDER_FAILURE, 3000U,
                        peer_port, sizeof(peer_port)));
  TEST_CHECK(check_call(&owner, "plain", "/stall",
                        COAKKA_HTTP_HOST_OUTBOUND_DEADLINE_EXCEEDED, 500U,
                        peer_port, sizeof(peer_port)));
  peer_port[0] = '\0';
  TEST_CHECK(check_call(&owner, "plain", "/echo",
                        COAKKA_HTTP_HOST_OUTBOUND_RESPONSE, 3000U, peer_port,
                        sizeof(peer_port)));
cleanup:
  return test_finish("outbound_transport_contract", ok,
                     test_service_close(&owner));
}
