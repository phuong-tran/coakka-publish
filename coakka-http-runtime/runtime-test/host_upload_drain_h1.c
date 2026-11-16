#include "test_support.h"

#include <stdlib.h>
#include <string.h>

typedef enum upload_case {
  UPLOAD_DATA_HELD,
  UPLOAD_DATA_RETURNED,
  UPLOAD_TRAILERS_HELD
} upload_case_t;

static int checked_payload(const coakka_http_host_request_event_t *event,
                           upload_case_t scenario) {
  if (scenario == UPLOAD_TRAILERS_HELD) {
    coakka_http_host_header_t header = {0};
    return event->kind == COAKKA_HTTP_HOST_REQUEST_TRAILERS &&
           coakka_http_host_request_stream_trailer_count(event) == 1U &&
           coakka_http_host_request_stream_trailer(event, 0U, &header) != 0U &&
           test_bytes_equal(header.name, "x-receipt") &&
           test_bytes_equal(header.value, "ready");
  }
  if (event->kind != COAKKA_HTTP_HOST_REQUEST_DATA || event->data.size == 0U ||
      event->data.size > 4096U) return 0;
  for (size_t index = 0; index < event->data.size; ++index) {
    if (event->data.data[index] != (uint8_t)'a') return 0;
  }
  return 1;
}

static int same_exchange(const coakka_http_host_request_event_t *event,
                         coakka_http_host_exchange_t exchange) {
  return event->exchange.slot == exchange.slot &&
         event->exchange.generation == exchange.generation;
}

int main(int argc, char **argv) {
  if (argc != 6 || (strcmp(argv[2], "plain") != 0 &&
                    strcmp(argv[2], "tls") != 0) ||
      coakka_http_host_abi_version() != 10U || !test_platform_open())
    return EXIT_FAILURE;
  upload_case_t scenario;
  if (strcmp(argv[1], "held") == 0) scenario = UPLOAD_DATA_HELD;
  else if (strcmp(argv[1], "returned") == 0) scenario = UPLOAD_DATA_RETURNED;
  else if (strcmp(argv[1], "trailers") == 0) scenario = UPLOAD_TRAILERS_HELD;
  else return EXIT_FAILURE;

  char *port_end = NULL;
  const unsigned long requested_port = strtoul(argv[5], &port_end, 10);
  if (port_end == argv[5] || *port_end != '\0' || requested_port == 0UL ||
      requested_port > 65535UL)
    return EXIT_FAILURE;

  int ok = 1;
  uint16_t port = 0U;
  test_service_t owner;
  test_service_init(&owner);
  coakka_http_host_request_event_t held;
  coakka_http_host_request_event_init(&held);
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text("127.0.0.1");
  config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_1_1;
  config.port = (uint16_t)requested_port;
  if (strcmp(argv[2], "tls") == 0) {
    config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
    config.credential_id = test_text("upload-stdio-fixture");
    config.credential_generation = 1U;
    config.certificate_chain_file = test_text(argv[3]);
    config.private_key_file = test_text(argv[4]);
  }
  config.limits.drain_timeout_ms = 300U;
  config.limits.stop_timeout_ms = 300U;
  config.limits.request_stream_max_retained_items = 1U;

  coakka_http_host_route_t routes[2];
  coakka_http_host_route_init(&routes[0]);
  routes[0].route_id = routes[0].handler_binding_id = 1U;
  routes[0].method = test_text("GET");
  routes[0].encoded_path_pattern = test_text("/ready");
  coakka_http_host_route_init(&routes[1]);
  routes[1].route_id = routes[1].handler_binding_id = 2U;
  routes[1].method = test_text("POST");
  routes[1].encoded_path_pattern = test_text("/upload");
  routes[1].flags = COAKKA_HTTP_HOST_ROUTE_STREAMING_REQUEST;

  TEST_CHECK(test_service_open(&owner, &config, routes, 2U));
  TEST_CHECK(coakka_http_host_service_port(owner.service, &port) ==
             COAKKA_HTTP_HOST_OK && port == config.port);
  printf("PORT=%u\n", (unsigned)port);
  fflush(stdout);

  TEST_CHECK(test_next_request(&owner));
  TEST_CHECK(coakka_http_host_request_route_id(&owner.event) == 1U);
  const coakka_http_host_exchange_t ready_exchange = owner.event.exchange;
  TEST_CHECK(test_respond(&owner, 200U, "ready"));
  TEST_CHECK(test_release(&owner));
  TEST_CHECK(test_take(&owner, 3000U) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
             same_exchange(&owner.event, ready_exchange));
  TEST_CHECK(test_release(&owner));
  puts("READY");
  fflush(stdout);

  TEST_CHECK(test_next_request(&owner));
  TEST_CHECK(coakka_http_host_request_route_id(&owner.event) == 2U);
  const coakka_http_host_exchange_t exchange = owner.event.exchange;
  TEST_CHECK(test_release(&owner));

  TEST_CHECK(test_take(&owner, 4000U) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(same_exchange(&owner.event, exchange));
  TEST_CHECK(checked_payload(&owner.event, UPLOAD_DATA_HELD));
  if (scenario == UPLOAD_TRAILERS_HELD) {
    TEST_CHECK(test_release(&owner));
    TEST_CHECK(test_take(&owner, 4000U) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(same_exchange(&owner.event, exchange) &&
               checked_payload(&owner.event, scenario));
  }
  if (scenario == UPLOAD_DATA_RETURNED) {
    TEST_CHECK(test_release(&owner));
  } else {
    // Move the descriptor without returning it. The same ABI reader will
    // release it once the controller has observed the expired drain.
    held = owner.event;
    coakka_http_host_request_event_init(&owner.event);
    owner.lease_live = 0;
  }

  TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) == COAKKA_HTTP_HOST_OK);
  coakka_http_host_drain_state_t outcome = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
  const coakka_http_host_status_t waited =
      coakka_http_host_service_wait_drained(owner.service, 4000U, &outcome);
  TEST_CHECK(waited == COAKKA_HTTP_HOST_TIMEOUT &&
             outcome == COAKKA_HTTP_HOST_DRAIN_EXPIRED);
  puts("EXPIRED");
  fflush(stdout);
  char command[16];
  TEST_CHECK(fgets(command, sizeof(command), stdin) != NULL &&
             strcmp(command, "release\n") == 0);

  if (held.kind != 0U) {
    TEST_CHECK(checked_payload(&held, scenario));
  }

  // The stream and exchange terminals use separate public event lanes.
  // Require one terminal from each lane for the same upload.
  const uint32_t stream_terminal = scenario == UPLOAD_TRAILERS_HELD
      ? COAKKA_HTTP_HOST_REQUEST_END : COAKKA_HTTP_HOST_REQUEST_CANCELLED;
  unsigned stream_count = 0U;
  unsigned exchange_count = 0U;
  for (unsigned index = 0U; index < 2U; ++index) {
    TEST_CHECK(test_take(&owner, 3000U) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(same_exchange(&owner.event, exchange));
    if (owner.event.kind == stream_terminal) ++stream_count;
    else if (owner.event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL) ++exchange_count;
    else {
      (void)fprintf(stderr, "unexpected upload event: %u\n",
                    (unsigned)owner.event.kind);
      ok = 0;
      goto cleanup;
    }
    TEST_CHECK(test_release(&owner));
  }
  TEST_CHECK(stream_count == 1U && exchange_count == 1U);

  if (held.kind != 0U) {
    coakka_http_host_service_t *const retained = owner.service;
    const coakka_http_host_status_t stop_refused =
        coakka_http_host_service_stop(owner.service);
    TEST_CHECK(stop_refused == COAKKA_HTTP_HOST_TIMEOUT ||
               stop_refused == COAKKA_HTTP_HOST_INVALID_ARGUMENT);
    const coakka_http_host_status_t refused =
        coakka_http_host_service_destroy(&owner.service);
    TEST_CHECK((refused == COAKKA_HTTP_HOST_TIMEOUT ||
                refused == COAKKA_HTTP_HOST_INVALID_ARGUMENT) &&
               owner.service == retained && checked_payload(&held, scenario));
    TEST_CHECK(coakka_http_host_release_request(owner.service, &held) ==
               COAKKA_HTTP_HOST_OK && held.kind == 0U);
  }
  TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);
  outcome = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
  TEST_CHECK(coakka_http_host_service_wait_drained(owner.service, 0U, &outcome) ==
                 COAKKA_HTTP_HOST_TIMEOUT &&
             outcome == COAKKA_HTTP_HOST_DRAIN_EXPIRED);
  TEST_CHECK(test_take(&owner, 0U) == COAKKA_HTTP_HOST_CLOSED);
  TEST_CHECK(test_service_close(&owner));

  puts("STOPPED");
  fflush(stdout);
  TEST_CHECK(fgets(command, sizeof(command), stdin) != NULL &&
             strcmp(command, "reopen\n") == 0);
  TEST_CHECK(test_service_open(&owner, &config, routes, 2U));
  uint16_t reopened = 0U;
  TEST_CHECK(coakka_http_host_service_port(owner.service, &reopened) ==
                 COAKKA_HTTP_HOST_OK && reopened == port);
  printf("PASS %s %s reopened=%u\n", argv[1], argv[2], (unsigned)reopened);

cleanup:
  if (held.kind != 0U) {
    if (coakka_http_host_release_request(owner.service, &held) != COAKKA_HTTP_HOST_OK)
      ok = 0;
  }
  return test_finish("http1_upload_drain", ok, test_service_close(&owner));
}
