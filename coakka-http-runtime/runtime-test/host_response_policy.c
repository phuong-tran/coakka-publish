#include "test_support.h"

#include <stdlib.h>
#include <string.h>

enum {
  gzip_body_bytes = 768,
  pressure_data_bytes = 6000,
  pressure_chunk_bytes = 8192,
  pressure_retained_bytes = 32768,
  max_pressure_writes = 512
};

static int next_command(const char *name) {
  char input[32];
  return fgets(input, sizeof(input), stdin) != NULL &&
         strncmp(input, name, strlen(name)) == 0 &&
         strcmp(input + strlen(name), "\n") == 0;
}

static void mark(const char *message) {
  puts(message);
  fflush(stdout);
}

static int same_exchange(coakka_http_host_exchange_t a,
                         coakka_http_host_exchange_t b) {
  return a.slot == b.slot && a.generation == b.generation;
}

static int next_event(test_service_t *owner, uint32_t kind,
                      coakka_http_host_exchange_t exchange,
                      coakka_http_host_exchange_t slow) {
  for (unsigned attempt = 0; attempt < 32U; ++attempt) {
    if (test_take(owner, 5000U) != COAKKA_HTTP_HOST_OK)
      return 0;
    const uint32_t observed = owner->event.kind;
    const coakka_http_host_exchange_t token = owner->event.exchange;
    if (!test_release(owner))
      return 0;
    if (observed == kind && same_exchange(token, exchange))
      return 1;
    /* The slow stream can publish writable while its initial prefix drains. */
    if (observed != COAKKA_HTTP_HOST_RESPONSE_WRITABLE ||
        (!same_exchange(token, slow) && !same_exchange(token, exchange))) {
      fprintf(stderr, "Unexpected event kind %u for slot %u\n",
              (unsigned)observed, (unsigned)token.slot);
      return 0;
    }
  }
  return 0;
}

static int take_path(test_service_t *owner, const char *path,
                     coakka_http_host_exchange_t slow,
                     coakka_http_host_exchange_t *exchange) {
  for (unsigned attempt = 0; attempt < 32U; ++attempt) {
    if (test_take(owner, 5000U) != COAKKA_HTTP_HOST_OK)
      return 0;
    const uint32_t kind = owner->event.kind;
    const int matches =
        kind == COAKKA_HTTP_HOST_REQUEST &&
        test_bytes_equal(coakka_http_host_request_target(&owner->event), path);
    const coakka_http_host_exchange_t token = owner->event.exchange;
    if (!test_release(owner))
      return 0;
    if (matches) {
      *exchange = token;
      return 1;
    }
    if (kind != COAKKA_HTTP_HOST_RESPONSE_WRITABLE ||
        !same_exchange(token, slow))
      return 0;
  }
  return 0;
}

static coakka_http_host_status_t write_event(test_service_t *owner,
                                            coakka_http_host_exchange_t exchange,
                                            const char *data) {
  coakka_http_host_sse_event_t event;
  coakka_http_host_sse_event_init(&event);
  event.data = test_text(data);
  return coakka_http_host_write_sse(owner->service, exchange, &event);
}

static int fill_slow(test_service_t *owner, coakka_http_host_exchange_t slow,
                     const char *data, int require_accepted_write) {
  for (unsigned count = 0; count < max_pressure_writes; ++count) {
    const coakka_http_host_status_t result = write_event(owner, slow, data);
    if (result == COAKKA_HTTP_HOST_CAPACITY_EXHAUSTED) {
      printf("PRESSURE writes=%u limit=%u\n", count, pressure_retained_bytes);
      fflush(stdout);
      return !require_accepted_write || count != 0U;
    }
    if (result != COAKKA_HTTP_HOST_OK) {
      fprintf(stderr, "Slow SSE writer result %d\n", result);
      return 0;
    }
  }
  return 0;
}

static int serve_ordinary(test_service_t *owner,
                          coakka_http_host_exchange_t slow) {
  coakka_http_host_exchange_t ordinary = {0};
  coakka_http_host_response_t response;
  coakka_http_host_response_init(&response);
  response.body = test_text("ordinary-ok");
  return take_path(owner, "/ordinary", slow, &ordinary) &&
         coakka_http_host_respond(owner->service, ordinary, &response) ==
             COAKKA_HTTP_HOST_OK &&
         next_event(owner, COAKKA_HTTP_HOST_REQUEST_TERMINAL, ordinary, slow);
}

static int observed_write_timeout(test_service_t *owner,
                                  coakka_http_host_exchange_t slow) {
  coakka_http_host_monitor_event_t events[16];
  coakka_http_host_monitor_page_t page;
  coakka_http_host_monitor_page_init(&page);
  for (unsigned index = 0; index < 16U; ++index)
    coakka_http_host_monitor_event_init(&events[index]);
  if (coakka_http_host_monitor_read(owner->service, 0U, 16U, events, 16U,
                                    &page) != COAKKA_HTTP_HOST_OK ||
      page.remaining_events != 0U)
    return 0;

  unsigned timeouts = 0U;
  /* ABI 10 projects RESPONSE_WRITE_TIMEOUT as failure reason 603. */
  for (uint32_t index = 0U; index < page.count; ++index) {
    if (same_exchange(events[index].exchange, slow) &&
        events[index].kind == COAKKA_HTTP_HOST_EVENT_EXCHANGE_TIMED_OUT &&
        events[index].failure_reason == 603U)
      ++timeouts;
  }
  return timeouts == 1U;
}

static int serve_sse(test_service_t *owner, int expire) {
  int ok = 1;
  coakka_http_host_exchange_t slow = {0}, sibling = {0};
  char payload[pressure_data_bytes + 1];
  memset(payload, 'a', sizeof(payload) - 1U);
  payload[sizeof(payload) - 1U] = '\0';

  TEST_CHECK(take_path(owner, "/stream", slow, &slow));
  TEST_CHECK(coakka_http_host_start_sse(owner->service, slow, NULL, 0U) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(next_event(owner, COAKKA_HTTP_HOST_RESPONSE_WRITABLE, slow, slow));
  TEST_CHECK(write_event(owner, slow, "slow-first") == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(next_command("fill"));
  TEST_CHECK(fill_slow(owner, slow, payload, 1));

  TEST_CHECK(take_path(owner, "/events", slow, &sibling));
  TEST_CHECK(coakka_http_host_start_sse(owner->service, sibling, NULL, 0U) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(next_event(owner, COAKKA_HTTP_HOST_RESPONSE_WRITABLE, sibling, slow));
  TEST_CHECK(write_event(owner, sibling, "sibling-first") == COAKKA_HTTP_HOST_OK);

  TEST_CHECK(serve_ordinary(owner, slow));
  TEST_CHECK(write_event(owner, sibling, "sibling-last") == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(coakka_http_host_finish_response_stream(
                 owner->service, sibling, NULL, 0U) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(next_event(owner, COAKKA_HTTP_HOST_REQUEST_TERMINAL, sibling, slow));

  TEST_CHECK(next_command("bound"));
  TEST_CHECK(fill_slow(owner, slow, payload, 0));
  if (!expire)
    TEST_CHECK(next_command("cancelled"));
  TEST_CHECK(next_event(owner, COAKKA_HTTP_HOST_REQUEST_TERMINAL, slow, slow));
  TEST_CHECK(write_event(owner, slow, "late") == COAKKA_HTTP_HOST_STALE_EXCHANGE);
  TEST_CHECK(coakka_http_host_finish_response_stream(
                 owner->service, slow, NULL, 0U) == COAKKA_HTTP_HOST_STALE_EXCHANGE);
  if (expire) {
    TEST_CHECK(observed_write_timeout(owner, slow));
    mark("WRITE_TIMEOUT");
    TEST_CHECK(serve_ordinary(owner, slow));
  }
  TEST_CHECK(test_take(owner, 0U) == COAKKA_HTTP_HOST_TIMEOUT);
  mark(expire ? "TERMINALS 4" : "TERMINALS 3");
cleanup:
  return ok;
}

static int serve_gzip(test_service_t *owner) {
  int ok = 1;
  char body[gzip_body_bytes + 1];
  const char *const paths[] = {"/gzip", "/gzip", "/gzip", "/small", "/empty"};
  memset(body, 'a', sizeof(body) - 1U);
  body[sizeof(body) - 1U] = '\0';
  for (size_t index = 0; index < sizeof(paths) / sizeof(paths[0]); ++index) {
    coakka_http_host_exchange_t exchange = {0};
    TEST_CHECK(take_path(owner, paths[index], exchange, &exchange));
    coakka_http_host_response_t response;
    coakka_http_host_response_init(&response);
    const coakka_http_host_header_t header = {
        test_text("content-type"), test_text("text/plain")};
    response.headers = &header;
    response.header_count = 1U;
    response.status_code = index == 4U ? 204U : 200U;
    if (index != 4U)
      response.body = test_text(index == 3U ? "small" : body);
    TEST_CHECK(coakka_http_host_respond(owner->service, exchange, &response) ==
               COAKKA_HTTP_HOST_OK);
    TEST_CHECK(next_event(owner, COAKKA_HTTP_HOST_REQUEST_TERMINAL, exchange, exchange));
  }
  TEST_CHECK(test_take(owner, 0U) == COAKKA_HTTP_HOST_TIMEOUT);
  mark("TERMINALS 5");
cleanup:
  return ok;
}

int main(int argc, char **argv) {
  if (argc != 6 || coakka_http_host_abi_version() != 10U)
    return EXIT_FAILURE;
  const int expire = strcmp(argv[1], "write-timeout") == 0;
  const int sse = expire || strcmp(argv[1], "sse-pressure") == 0;
  const int http1 = strcmp(argv[2], "1.1") == 0;
  const int http2 = strcmp(argv[2], "2") == 0;
  const int http3 = strcmp(argv[2], "3") == 0;
  char *end = NULL;
  const unsigned long port_arg = strtoul(argv[5], &end, 10);
  if ((!sse && strcmp(argv[1], "gzip") != 0) ||
      (!http1 && !http2 && !http3) || (expire && !http1) ||
      argv[5] == end || *end != '\0' ||
      port_arg > UINT16_MAX || !test_platform_open())
    return EXIT_FAILURE;

  int ok = 1;
  test_service_t owner;
  test_service_init(&owner);
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text("127.0.0.1");
  config.port = (uint16_t)port_arg;
  config.protocol = http1 ? COAKKA_HTTP_HOST_PROTOCOL_HTTP_1_1
                          : (http2 ? COAKKA_HTTP_HOST_PROTOCOL_HTTP_2
                                   : COAKKA_HTTP_HOST_PROTOCOL_HTTP_3);
  if (!http1) {
    config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
    config.credential_id = test_text("response-policy-fixture");
    config.credential_generation = 1U;
    config.certificate_chain_file = test_text(argv[3]);
    config.private_key_file = test_text(argv[4]);
  }
  if (sse) {
    config.limits.handler_timeout_ms = 15000U;
    config.limits.response_write_timeout_ms = expire ? 1800U : 12000U;
    config.limits.response_stream_max_chunk_bytes = pressure_chunk_bytes;
    config.limits.response_stream_max_retained_bytes = pressure_retained_bytes;
    config.limits.response_stream_max_total_retained_bytes = 4U * pressure_retained_bytes;
  } else {
    config.compression.enabled = 1U;
    config.compression.minimum_body_bytes = 128U;
  }
  if (expire) {
    config.monitor.event_capacity = 16U;
    config.monitor.max_events_per_read = 16U;
    config.monitor.categories = COAKKA_HTTP_HOST_MONITOR_EXCHANGE;
    config.monitor.initial_collection = COAKKA_HTTP_HOST_MONITOR_AGGREGATES_AND_EVENTS;
  }
  const char *const sse_paths[] = {"/stream", "/events", "/ordinary"};
  const char *const gzip_paths[] = {"/gzip", "/small", "/empty"};
  coakka_http_host_route_t routes[3];
  for (uint32_t index = 0U; index < 3U; ++index) {
    coakka_http_host_route_init(&routes[index]);
    routes[index].route_id = index + 1U;
    routes[index].handler_binding_id = index + 1U;
    routes[index].method = test_text("GET");
    routes[index].encoded_path_pattern = test_text(
        sse ? sse_paths[index] : gzip_paths[index]);
  }
  uint16_t port = 0U;
  TEST_CHECK(test_service_open(&owner, &config, routes, 3U));
  TEST_CHECK(coakka_http_host_service_port(owner.service, &port) ==
             COAKKA_HTTP_HOST_OK && port != 0U);
  printf("READY %u ABI 10\n", (unsigned)port);
  fflush(stdout);
  TEST_CHECK(sse ? serve_sse(&owner, expire) : serve_gzip(&owner));
  TEST_CHECK(next_command("close"));
  TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) == COAKKA_HTTP_HOST_OK);
  coakka_http_host_drain_state_t state = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
  TEST_CHECK(coakka_http_host_service_wait_drained(owner.service, 5000U, &state) ==
             COAKKA_HTTP_HOST_OK && state == COAKKA_HTTP_HOST_DRAIN_COMPLETE);
  TEST_CHECK(test_service_close(&owner));

  config.port = port;
  TEST_CHECK(test_service_open(&owner, &config, routes, 3U));
  uint16_t reopened = 0U;
  TEST_CHECK(coakka_http_host_service_port(owner.service, &reopened) ==
             COAKKA_HTTP_HOST_OK && reopened == port);
  mark("REOPENED");
cleanup:
  return test_finish("installed-response-policy", ok, test_service_close(&owner));
}
