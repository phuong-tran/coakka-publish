#include "test_support.h"

#include <stdlib.h>
#include <string.h>

static int matches(coakka_http_host_exchange_t expected,
                   coakka_http_host_exchange_t actual) {
  return expected.slot == actual.slot &&
         expected.generation == actual.generation;
}

static int check_borrowed(const coakka_http_host_request_event_t *event,
                          int trailers) {
  if (!trailers) {
    if (event->kind != COAKKA_HTTP_HOST_REQUEST_DATA ||
        event->data.size == 0U || event->data.size > 900U) return 0;
    for (size_t index = 0; index < event->data.size; ++index) {
      if (event->data.data[index] != (uint8_t)'a') return 0;
    }
    return 1;
  }
  coakka_http_host_header_t header = {0};
  return event->kind == COAKKA_HTTP_HOST_REQUEST_TRAILERS &&
         coakka_http_host_request_stream_trailer_count(event) == 1U &&
         coakka_http_host_request_stream_trailer(event, 0U, &header) != 0U &&
         test_bytes_equal(header.name, "x-receipt") &&
         test_bytes_equal(header.value, "ready");
}

typedef struct upload_events {
  unsigned cancelled;
  unsigned terminal;
  unsigned ended;
} upload_events_t;

static int count_event(test_service_t *owner, coakka_http_host_exchange_t exchange,
                       upload_events_t *events) {
  int recognized = matches(exchange, owner->event.exchange);
  if (recognized) {
    switch (owner->event.kind) {
    case COAKKA_HTTP_HOST_REQUEST_CANCELLED: ++events->cancelled; break;
    case COAKKA_HTTP_HOST_REQUEST_END: ++events->ended; break;
    case COAKKA_HTTP_HOST_REQUEST_TERMINAL: ++events->terminal; break;
    default: recognized = 0; break;
    }
  }
  return test_release(owner) && recognized;
}

int main(int argc, char **argv) {
  const int h2 = argc == 7 && strcmp(argv[6], "h2") == 0;
  if ((argc != 6 && !h2) ||
      (strcmp(argv[1], "expiry") != 0 && strcmp(argv[1], "abort") != 0 &&
       (!h2 || strcmp(argv[1], "returned") != 0)) ||
      (strcmp(argv[2], "data") != 0 && strcmp(argv[2], "trailers") != 0) ||
      (strcmp(argv[5], "auto") != 0 && strcmp(argv[5], "io-uring") != 0) ||
      !test_platform_open()) {
    return EXIT_FAILURE;
  }
  const int returned = strcmp(argv[1], "returned") == 0;
  const int expiry = returned || strcmp(argv[1], "expiry") == 0;
  const int trailers = strcmp(argv[2], "trailers") == 0;
  const int uring = strcmp(argv[5], "io-uring") == 0;
  const uint32_t payload_kind = trailers ? COAKKA_HTTP_HOST_REQUEST_TRAILERS
                                         : COAKKA_HTTP_HOST_REQUEST_DATA;
  int ok = 1;
  upload_events_t events = {0};
  uint16_t port = 0U;
  uint64_t end = 0U;
  test_service_t owner;
  test_service_init(&owner);
  coakka_http_host_request_event_t held;
  coakka_http_host_request_event_init(&held);
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text("127.0.0.1");
  config.protocol = h2 ? COAKKA_HTTP_HOST_PROTOCOL_HTTP_2
                       : COAKKA_HTTP_HOST_PROTOCOL_HTTP_3;
  config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
  config.credential_id = test_text("upload-drain");
  config.credential_generation = 1U;
  config.certificate_chain_file = test_text(argv[3]);
  config.private_key_file = test_text(argv[4]);
  config.limits.drain_timeout_ms = 600U;
  config.use_io_uring = uring ? 1U : 0U;
  coakka_http_host_route_t route;
  coakka_http_host_route_init(&route);
  route.route_id = route.handler_binding_id = 1U;
  route.method = test_text("POST");
  route.encoded_path_pattern = test_text("/upload");
  route.flags = COAKKA_HTTP_HOST_ROUTE_STREAMING_REQUEST;
  TEST_CHECK(coakka_http_host_abi_version() == 10U);
  TEST_CHECK(test_service_open(&owner, &config, &route, 1U));
  coakka_http_host_service_info_t info;
  coakka_http_host_service_info_init(&info);
  TEST_CHECK(coakka_http_host_service_info(owner.service, &info) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(!uring || info.io_uring_effective != 0U);
  TEST_CHECK(coakka_http_host_service_port(owner.service, &port) == COAKKA_HTTP_HOST_OK);
  printf("PORT=%u\n", (unsigned)port);
  fflush(stdout);

  TEST_CHECK(test_take(&owner, 4000U) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST);
  const coakka_http_host_exchange_t exchange = owner.event.exchange;
  TEST_CHECK(test_release(&owner));
  puts("HEAD");
  fflush(stdout);
  TEST_CHECK(test_now_ms(&end));
  end += 4000U;
  for (;;) {
    uint64_t now = 0U;
    TEST_CHECK(test_now_ms(&now) && now < end);
    TEST_CHECK(test_take(&owner, end - now) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(matches(exchange, owner.event.exchange));
    if (owner.event.kind == payload_kind) {
      held = owner.event;
      coakka_http_host_request_event_init(&owner.event);
      owner.lease_live = 0;
      break;
    }
    TEST_CHECK(trailers && owner.event.kind == COAKKA_HTTP_HOST_REQUEST_DATA);
    TEST_CHECK(test_release(&owner));
    puts("DATA");
    fflush(stdout);
  }
  TEST_CHECK(check_borrowed(&held, trailers));
  const void *const data_address = held.data.data;
  const size_t data_size = held.data.size;
  if (returned) {
    TEST_CHECK(coakka_http_host_release_request(owner.service, &held) ==
               COAKKA_HTTP_HOST_OK);
  }
  if (expiry) {
    TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) == COAKKA_HTTP_HOST_OK);
  }
  coakka_http_host_drain_state_t state = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
  const coakka_http_host_status_t expected_wait =
      expiry ? COAKKA_HTTP_HOST_TIMEOUT : COAKKA_HTTP_HOST_CLOSED;
  const coakka_http_host_drain_state_t expected_state =
      expiry ? COAKKA_HTTP_HOST_DRAIN_EXPIRED : COAKKA_HTTP_HOST_DRAIN_ABORTED;
  if (expiry) {
    TEST_CHECK(test_now_ms(&end));
    end += 4000U;
    coakka_http_host_status_t wait;
    for (;;) {
      wait = coakka_http_host_service_wait_drained(owner.service, 0U, &state);
      if (state != COAKKA_HTTP_HOST_DRAIN_PENDING) break;
      uint64_t now = 0U;
      TEST_CHECK(test_now_ms(&now));
      if (now >= end) break;
      const coakka_http_host_status_t event = test_take(&owner, 40U);
      if (event == COAKKA_HTTP_HOST_CLOSED) {
        wait = coakka_http_host_service_wait_drained(owner.service, end - now,
                                                     &state);
        break;
      }
      if (event == COAKKA_HTTP_HOST_OK) {
        TEST_CHECK(count_event(&owner, exchange, &events));
      } else {
        TEST_CHECK(event == COAKKA_HTTP_HOST_TIMEOUT);
      }
    }
    printf("DRAIN wait=%d state=%d\n", (int)wait, (int)state);
    TEST_CHECK(wait == expected_wait && state == expected_state);
  }
  if (!returned) {
    TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_INVALID_ARGUMENT);
    TEST_CHECK(coakka_http_host_service_wait_drained(owner.service, 0U, &state) ==
               expected_wait && state == expected_state);
    coakka_http_host_service_t *const retained_owner = owner.service;
    TEST_CHECK(coakka_http_host_service_destroy(&owner.service) ==
               COAKKA_HTTP_HOST_INVALID_ARGUMENT && owner.service == retained_owner);
    TEST_CHECK(held.data.data == data_address && held.data.size == data_size);
    TEST_CHECK(check_borrowed(&held, trailers));
    TEST_CHECK(coakka_http_host_release_request(owner.service, &held) == COAKKA_HTTP_HOST_OK);
  }
  coakka_http_host_status_t taken;
  while ((taken = test_take(&owner, 0U)) == COAKKA_HTTP_HOST_OK) {
    TEST_CHECK(count_event(&owner, exchange, &events));
  }
  printf("EVENTS terminal=%u cancelled=%u end=%u take=%d\n",
         events.terminal, events.cancelled, events.ended, (int)taken);
  TEST_CHECK(taken == COAKKA_HTTP_HOST_TIMEOUT ||
             (returned && taken == COAKKA_HTTP_HOST_CLOSED));
  TEST_CHECK(events.terminal == 1U);
  TEST_CHECK(trailers ? (events.ended == 1U && events.cancelled == 0U)
                      : (events.ended == 0U && events.cancelled == 1U));
  TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(coakka_http_host_service_wait_drained(owner.service, 0U, &state) ==
             expected_wait && state == expected_state);
  TEST_CHECK(test_take(&owner, 0U) == COAKKA_HTTP_HOST_CLOSED);
  TEST_CHECK(coakka_http_host_service_destroy(&owner.service) ==
             COAKKA_HTTP_HOST_OK && owner.service == NULL);
  config.port = port;
  TEST_CHECK(test_service_open(&owner, &config, &route, 1U));
  uint16_t reopened_port = 0U;
  TEST_CHECK(coakka_http_host_service_port(owner.service, &reopened_port) ==
             COAKKA_HTTP_HOST_OK && reopened_port == port);
  printf("PASS %s %s %s same-port=%u\n", argv[1], argv[2], argv[5],
         (unsigned)port);

cleanup:
  if (held.kind != 0U) {
    (void)coakka_http_host_release_request(owner.service, &held);
  }
  return test_finish("upload_drain", ok, test_service_close(&owner));
}
