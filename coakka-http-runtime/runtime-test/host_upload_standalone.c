#include "test_support.h"

#include <stdlib.h>
#include <string.h>

static int same_exchange(const test_service_t *owner,
                         coakka_http_host_exchange_t exchange) {
  return owner->event.exchange.slot == exchange.slot &&
         owner->event.exchange.generation == exchange.generation;
}

static int await_cancelled_upload(test_service_t *owner,
                                  coakka_http_host_exchange_t exchange,
                                  size_t received, size_t payload_size) {
  int saw_cancel = 0;
  int saw_terminal = 0;
  uint64_t deadline = 0U;
  if (!test_now_ms(&deadline) || deadline > UINT64_MAX - TEST_IO_TIMEOUT_MS) {
    return 0;
  }
  deadline += TEST_IO_TIMEOUT_MS;
  while (!saw_cancel || !saw_terminal) {
    uint64_t now = 0U;
    if (!test_now_ms(&now) || now >= deadline ||
        test_take(owner, deadline - now) != COAKKA_HTTP_HOST_OK ||
        !same_exchange(owner, exchange)) return 0;
    if (owner->event.kind == COAKKA_HTTP_HOST_REQUEST_CANCELLED && !saw_cancel) {
      saw_cancel = 1;
    } else if (owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
               !saw_terminal) {
      saw_terminal = 1;
    } else if (owner->event.kind == COAKKA_HTTP_HOST_REQUEST_DATA && !saw_cancel &&
               owner->event.data.size != 0U &&
               owner->event.data.size < payload_size - received) {
      // With UDP, the listener can deliver bytes already in flight before its
      // idle timer observes that the client went away. They must be a prefix.
      for (size_t i = 0U; i < owner->event.data.size; ++i) {
        if (owner->event.data.data[i] != (uint8_t)((received + i) % 251U)) {
          return 0;
        }
      }
      received += owner->event.data.size;
    } else {
      return 0;
    }
    if (!test_release(owner)) return 0;
  }
  coakka_http_host_response_t response;
  coakka_http_host_response_init(&response);
  response.status_code = 201U;
  return saw_cancel && saw_terminal &&
         coakka_http_host_respond(owner->service, exchange, &response) ==
             COAKKA_HTTP_HOST_STALE_EXCHANGE;
}

int main(int argc, char **argv) {
  if ((argc != 7 && argc != 8) || strcmp(argv[1], "upload") != 0 ||
      (strcmp(argv[2], "2") != 0 && strcmp(argv[2], "3") != 0) ||
      (argc == 8 && strcmp(argv[7], "hold-first-data") != 0 &&
       strcmp(argv[7], "cancel-held-data") != 0) ||
      coakka_http_host_abi_version() != 10U || !test_platform_open())
    return EXIT_FAILURE;

  test_service_t owner;
  test_service_init(&owner);
  coakka_http_host_configuration_t config;
  coakka_http_host_limits_t effective;
  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text("127.0.0.1");
  config.protocol = strcmp(argv[2], "2") == 0
                        ? COAKKA_HTTP_HOST_PROTOCOL_HTTP_2
                        : COAKKA_HTTP_HOST_PROTOCOL_HTTP_3;
  config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
  config.credential_id = test_text("upload-wire-fixture");
  config.credential_generation = 1U;
  config.certificate_chain_file = test_text(argv[3]);
  config.private_key_file = test_text(argv[4]);
  coakka_http_host_route_t route;
  coakka_http_host_route_init(&route);
  route.route_id = route.handler_binding_id = 1U;
  route.method = test_text("POST");
  route.encoded_path_pattern = test_text("/upload");
  route.flags = COAKKA_HTTP_HOST_ROUTE_STREAMING_REQUEST;
  coakka_http_host_response_t response;
  coakka_http_host_drain_state_t drain_state = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
  coakka_http_host_exchange_t exchange;
  uint16_t port;
  int ok = 1;
  char command[16];
  char *end = NULL;
  const unsigned long requested_size = strtoul(argv[5], &end, 10);
  TEST_CHECK(end != argv[5] && *end == '\0' &&
             requested_size > 0UL && requested_size <= 65536UL);
  const size_t payload_size = (size_t)requested_size;
  const unsigned long reserved_items = strtoul(argv[6], &end, 10);
  TEST_CHECK(end != argv[6] && *end == '\0' && reserved_items <= 128UL);
  config.limits.request_stream_max_retained_items = (uint32_t)reserved_items;
  const int hold_first_data = argc == 8;
  const int cancel_held = argc == 8 && strcmp(argv[7], "cancel-held-data") == 0;
  if (cancel_held) {
    // A killed UDP client cannot close gracefully. Bound the idle path so
    // the transport retires while the application still owns its DATA.
    config.limits.idle_timeout_ms = 1500U;
    config.limits.handler_timeout_ms = 10000U;
    config.limits.max_active_requests = 1U;
  }
  TEST_CHECK(!hold_first_data || reserved_items == 1UL);

  TEST_CHECK(test_service_open(&owner, &config, &route, 1U));
  coakka_http_host_limits_init(&effective);
  TEST_CHECK(coakka_http_host_service_limits(owner.service, &effective) ==
             COAKKA_HTTP_HOST_OK);
  if (reserved_items != 0UL) {
    TEST_CHECK(effective.request_stream_max_retained_items ==
               (uint32_t)reserved_items);
  }
  (void)fprintf(stderr, "upload bytes=%zu, Core retained items=%u\n",
                payload_size, (unsigned)effective.request_stream_max_retained_items);
  TEST_CHECK(coakka_http_host_service_port(owner.service, &port) ==
             COAKKA_HTTP_HOST_OK);
  (void)printf("READY %u\n", (unsigned)port);
  (void)fflush(stdout);
  TEST_CHECK(test_take(&owner, TEST_IO_TIMEOUT_MS) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST);
  TEST_CHECK(coakka_http_host_request_body_delivery(&owner.event) ==
             COAKKA_HTTP_HOST_REQUEST_BODY_STREAM);
  exchange = owner.event.exchange;
  TEST_CHECK(test_release(&owner));

  size_t received = 0U;
  uint64_t now = 0U;
  uint64_t until = 0U;
  int reached_end = 0;
  TEST_CHECK(test_now_ms(&until) && until <= UINT64_MAX - TEST_IO_TIMEOUT_MS);
  until += TEST_IO_TIMEOUT_MS;
  while (!reached_end) {
    TEST_CHECK(test_now_ms(&now) && now < until);
    TEST_CHECK(test_take(&owner, until - now) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(same_exchange(&owner, exchange));
    if (owner.event.kind == COAKKA_HTTP_HOST_REQUEST_END) {
      TEST_CHECK(received == payload_size);
      reached_end = 1;
    } else {
      if (owner.event.kind != COAKKA_HTTP_HOST_REQUEST_DATA) {
        (void)fprintf(stderr, "upload event %u after %zu of %zu bytes\n",
                      (unsigned)owner.event.kind, received, payload_size);
      }
      TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST_DATA);
      TEST_CHECK(owner.event.data.size > 0U &&
                 owner.event.data.size <= payload_size - received);
      if (hold_first_data && received == 0U) {
        TEST_CHECK(owner.event.data.size < payload_size);
        // The listener must finish an admitted upload even after drain fences
        // new work. Begin drain while the first DATA lease is still held.
        if (!cancel_held) {
          TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) ==
                     COAKKA_HTTP_HOST_OK);
        }
        (void)puts("HELD");
        (void)fflush(stdout);
        TEST_CHECK(fgets(command, sizeof(command), stdin) != NULL &&
                   strcmp(command, "release\n") == 0);
      }
      /* Read the borrowed bytes after the hold to verify they stayed valid. */
      for (size_t i = 0U; i < owner.event.data.size; ++i) {
        TEST_CHECK(owner.event.data.data[i] == (uint8_t)((received + i) % 251U));
      }
      received += owner.event.data.size;
    }
    TEST_CHECK(test_release(&owner));
    if (cancel_held) {
      TEST_CHECK(await_cancelled_upload(&owner, exchange, received, payload_size));
      (void)puts("CANCELLED");
      (void)fflush(stdout);
      break;
    }
  }
  if (!cancel_held) {
    coakka_http_host_response_init(&response);
    response.status_code = 201U;
    response.body = test_text("uploaded");
    TEST_CHECK(coakka_http_host_respond(owner.service, exchange, &response) ==
               COAKKA_HTTP_HOST_OK);
    TEST_CHECK(test_take(&owner, TEST_IO_TIMEOUT_MS) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
               same_exchange(&owner, exchange));
    TEST_CHECK(test_release(&owner));
    (void)puts("SENT");
    (void)fflush(stdout);
  }
  TEST_CHECK(fgets(command, sizeof(command), stdin) != NULL &&
             strcmp(command, "stop\n") == 0);
  uint64_t drain_start = 0U;
  uint64_t drain_end = 0U;
  TEST_CHECK(test_now_ms(&drain_start));
  TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) ==
             COAKKA_HTTP_HOST_OK);
  const coakka_http_host_status_t drain_status =
      coakka_http_host_service_wait_drained(owner.service, 5000U, &drain_state);
  TEST_CHECK(test_now_ms(&drain_end));
  const int complete = drain_status == COAKKA_HTTP_HOST_OK &&
                       drain_state == COAKKA_HTTP_HOST_DRAIN_COMPLETE;
  const int expired = drain_status == COAKKA_HTTP_HOST_TIMEOUT &&
                      drain_state == COAKKA_HTTP_HOST_DRAIN_EXPIRED;
  const char *outcome = complete ? "COMPLETE" : (expired ? "EXPIRED" : "UNEXPECTED");
  (void)fprintf(stderr, "upload drain %s %llu ms\n", outcome,
                (unsigned long long)(drain_end - drain_start));
  /* The controller independently validates HTTP 201 and its bytes. A
   * successful upload can leave only the final HTTP/3 reply unacknowledged. */
  TEST_CHECK(complete ||
             (config.protocol == COAKKA_HTTP_HOST_PROTOCOL_HTTP_3 &&
              !cancel_held && expired));
  TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);
  uint32_t stopped_drain = 0U;
  TEST_CHECK(coakka_http_host_service_wait_drained(owner.service, 0U,
                                                    &stopped_drain) ==
                 drain_status && stopped_drain == drain_state);
  TEST_CHECK(test_take(&owner, 0U) == COAKKA_HTTP_HOST_CLOSED);
  TEST_CHECK(test_service_close(&owner));
  (void)puts("STOPPED");
  (void)fflush(stdout);

cleanup:
  return test_finish("standalone_upload", ok, test_service_close(&owner));
}
