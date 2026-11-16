#include "test_support.h"

#include <string.h>

static int event_matches(const test_service_t *owner,
                         coakka_http_host_exchange_t exchange) {
  return owner->event.exchange.slot == exchange.slot &&
         owner->event.exchange.generation == exchange.generation;
}

int main(void) {
  static const char partial_body[] = "part";
  static const char complete_body[] = "done";
  static const char partial[] =
      "POST /upload HTTP/1.1\r\nHost: localhost\r\n"
      "Content-Length: 32\r\nConnection: close\r\n\r\npart";
  static const char complete[] =
      "POST /upload HTTP/1.1\r\nHost: localhost\r\n"
      "Content-Length: 4\r\nConnection: close\r\n\r\ndone";
  test_service_t owner;
  test_socket_t peer = TEST_INVALID_SOCKET;
  coakka_http_host_configuration_t config;
  coakka_http_host_route_t route;
  coakka_http_host_exchange_t cancelled;
  coakka_http_host_request_view_t view;
  coakka_http_host_response_t response;
  coakka_http_host_drain_state_t drain_state = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
  uint16_t port;
  uint8_t is_cancelled = 0U;
  char wire[TEST_WIRE_CAPACITY];
  size_t size;
  int ok = 1;
  int cleanup_ok = 1;

  if (!test_platform_open()) return 1;
  test_service_init(&owner);
  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text("127.0.0.1");
  config.limits.max_active_requests = 1U;
  config.limits.handler_timeout_ms = 30000U;
  config.limits.idle_timeout_ms = 30000U;
  coakka_http_host_route_init(&route);
  route.route_id = route.handler_binding_id = 1U;
  route.method = test_text("POST");
  route.encoded_path_pattern = test_text("/upload");
  route.flags = COAKKA_HTTP_HOST_ROUTE_STREAMING_REQUEST;
  TEST_CHECK(test_service_open(&owner, &config, &route, 1U));
  TEST_CHECK(coakka_http_host_service_port(owner.service, &port) ==
             COAKKA_HTTP_HOST_OK);

  peer = test_connect(port);
  TEST_CHECK(peer != TEST_INVALID_SOCKET);
  TEST_CHECK(test_send(peer, partial, sizeof(partial) - 1U));
  TEST_CHECK(test_take(&owner, TEST_IO_TIMEOUT_MS) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST);
  cancelled = owner.event.exchange;
  coakka_http_host_request_view_init(&view);
  TEST_CHECK(coakka_http_host_request_view(&owner.event, &view) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(view.body_delivery == COAKKA_HTTP_HOST_REQUEST_BODY_STREAM);
  TEST_CHECK(test_release(&owner));

  /* Wait for actual body delivery before closing the peer. TCP may split the
   * four bytes across events, so the fixture checks the whole prefix. */
  size_t received = 0U;
  while (received < sizeof(partial_body) - 1U) {
    TEST_CHECK(test_take(&owner, TEST_IO_TIMEOUT_MS) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST_DATA &&
               event_matches(&owner, cancelled));
    TEST_CHECK(owner.event.data.size != 0U &&
               owner.event.data.size <= sizeof(partial_body) - 1U - received);
    TEST_CHECK(memcmp(owner.event.data.data, partial_body + received,
                      owner.event.data.size) == 0);
    received += owner.event.data.size;
    TEST_CHECK(test_release(&owner));
  }
  TEST_CHECK(test_socket_close(&peer));
  /* Body cancellation and exchange terminal are distinct observations.
   * The public reader can take either ready lane first. */
  int saw_cancel = 0;
  int saw_terminal = 0;
  for (unsigned i = 0U; i < 2U; ++i) {
    TEST_CHECK(test_take(&owner, TEST_IO_TIMEOUT_MS) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(event_matches(&owner, cancelled));
    if (owner.event.kind == COAKKA_HTTP_HOST_REQUEST_CANCELLED) {
      TEST_CHECK(!saw_cancel);
      saw_cancel = 1;
    } else {
      TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
                 !saw_terminal);
      saw_terminal = 1;
    }
    TEST_CHECK(test_release(&owner));
  }
  TEST_CHECK(saw_cancel && saw_terminal);
  /* Reaping runs on the loop. Both snapshots are valid after terminal. */
  const coakka_http_host_status_t snapshot = coakka_http_host_request_cancelled(
      owner.service, cancelled, &is_cancelled);
  TEST_CHECK((snapshot == COAKKA_HTTP_HOST_OK && is_cancelled == 1U) ||
             (snapshot == COAKKA_HTTP_HOST_STALE_EXCHANGE && is_cancelled == 0U));
  coakka_http_host_response_init(&response);
  TEST_CHECK(coakka_http_host_respond(owner.service, cancelled, &response) ==
             COAKKA_HTTP_HOST_STALE_EXCHANGE);

  /* A successful upload on the same one-slot service proves admission
   * capacity returned. Any extra CANCEL, END or terminal fails event order. */
  peer = test_connect(port);
  TEST_CHECK(peer != TEST_INVALID_SOCKET);
  TEST_CHECK(test_send(peer, complete, sizeof(complete) - 1U));
  TEST_CHECK(test_take(&owner, TEST_IO_TIMEOUT_MS) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST);
  const coakka_http_host_exchange_t next = owner.event.exchange;
  TEST_CHECK(!event_matches(&owner, cancelled));
  TEST_CHECK(test_release(&owner));
  received = 0U;
  for (;;) {
    TEST_CHECK(test_take(&owner, TEST_IO_TIMEOUT_MS) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(event_matches(&owner, next));
    if (owner.event.kind == COAKKA_HTTP_HOST_REQUEST_END) {
      TEST_CHECK(received == sizeof(complete_body) - 1U);
      TEST_CHECK(test_release(&owner));
      break;
    }
    TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST_DATA);
    TEST_CHECK(owner.event.data.size != 0U &&
               owner.event.data.size <= sizeof(complete_body) - 1U - received);
    TEST_CHECK(memcmp(owner.event.data.data, complete_body + received,
                      owner.event.data.size) == 0);
    received += owner.event.data.size;
    TEST_CHECK(test_release(&owner));
  }
  response.status_code = 201U;
  response.body = test_text("uploaded");
  TEST_CHECK(coakka_http_host_respond(owner.service, next, &response) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(test_receive(peer, wire, sizeof(wire), &size));
  TEST_CHECK(test_wire_response(wire, size, 201U, "uploaded"));
  TEST_CHECK(test_socket_close(&peer));
  TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(coakka_http_host_service_wait_drained(
                 owner.service, TEST_IO_TIMEOUT_MS, &drain_state) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(drain_state == COAKKA_HTTP_HOST_DRAIN_COMPLETE);
  TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(test_take(&owner, 0U) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
             event_matches(&owner, next));
  TEST_CHECK(test_release(&owner));
  TEST_CHECK(test_take(&owner, 0U) == COAKKA_HTTP_HOST_CLOSED);

cleanup:
  if (!test_socket_close(&peer)) cleanup_ok = 0;
  if (!test_service_close(&owner)) cleanup_ok = 0;
  return test_finish("partial_upload_cancel_and_reuse", ok, cleanup_ok);
}
