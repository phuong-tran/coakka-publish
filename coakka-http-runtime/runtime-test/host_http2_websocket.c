#include "test_support.h"
#include "test_tls_material.h"

#include <stdlib.h>
#include <string.h>

#define READ_TIMEOUT UINT64_C(5000)

static int write_line(const char *value) {
  return puts(value) >= 0 && fflush(stdout) == 0;
}

static int same_session(coakka_http_host_websocket_t left,
                        coakka_http_host_websocket_t right) {
  return left.slot == right.slot && left.generation == right.generation;
}

int main(int argc, char **argv) {
  static const char root[] = "http2-ws-root.pem";
  static const char cert[] = "http2-ws-cert.pem";
  static const char key[] = "http2-ws-key.pem";
  int root_created = 0, cert_created = 0, key_created = 0;
  int ok = 1;
  test_service_t owner;
  coakka_http_host_configuration_t config;
  coakka_http_host_route_t routes[2];
  coakka_http_host_websocket_t sessions[16] = {{0}};
  coakka_http_host_exchange_t pending = {0};
  coakka_http_host_websocket_event_t retained;
  int retain = 0;
  uint16_t port = 0;
  char line[96];
  char command[24];
  unsigned argument = 0;
  unsigned code = 0;
  int platform_open = test_platform_open();

  test_service_init(&owner);
  coakka_http_host_websocket_event_init(&retained);
  TEST_CHECK(platform_open);
  TEST_CHECK(test_write_material(root, test_tls_root_a, &root_created));
  TEST_CHECK(test_write_material(cert, test_tls_certificate_a, &cert_created));
  TEST_CHECK(test_write_material(key, test_tls_key_a, &key_created));
  coakka_http_host_configuration_init(&config);
  config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_2;
  config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
  config.bind_address = test_text("127.0.0.1");
  config.credential_id = test_text("test-websocket");
  config.credential_generation = 1U;
  config.certificate_chain_file = test_text(cert);
  config.private_key_file = test_text(key);
  config.limits.drain_timeout_ms =
      argc > 1 && strcmp(argv[1], "graceful") == 0 ? 3000U : 1200U;
  for (unsigned i = 0; i < 2; ++i) {
    coakka_http_host_route_init(&routes[i]);
    routes[i].route_id = i + 1U;
    routes[i].handler_binding_id = i + 1U;
    routes[i].method = test_text("GET");
    routes[i].encoded_path_pattern = test_text(i == 0U ? "/ws" : "/check");
  }
  const int enabled = argc == 1 || strcmp(argv[1], "off") != 0;
  if (enabled) {
    coakka_http_host_configuration_t unsupported = config;
    unsupported.reserve_websocket = 1U;
    unsupported.security = COAKKA_HTTP_HOST_SECURITY_MUTUAL_TLS;
    unsupported.trust_roots_file = test_text(root);
    uint32_t failed_index = COAKKA_HTTP_HOST_FAILED_INDEX_NONE;
    TEST_CHECK(coakka_http_host_service_create(&unsupported, routes, 2U,
                                               &owner.service, &failed_index) ==
               COAKKA_HTTP_HOST_INVALID_ARGUMENT);
    TEST_CHECK(owner.service == NULL);
  }
  config.reserve_websocket = enabled ? 1U : 0U;
  TEST_CHECK(test_service_open(&owner, &config, routes, 2U));
  routes[0].flags = enabled ? COAKKA_HTTP_HOST_ROUTE_WEBSOCKET : 0U;
  TEST_CHECK(coakka_http_host_service_port(owner.service, &port) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(port != 0U);
  TEST_CHECK(printf("%u %s\n", (unsigned)port, root) > 0 &&
             fflush(stdout) == 0);

  while (fgets(line, sizeof(line), stdin) != NULL) {
    const int fields =
#if defined(_WIN32)
        sscanf_s(line, "%23s %u %u", command, (unsigned)sizeof(command),
                 &argument, &code);
#else
        sscanf(line, "%23s %u %u", command, &argument, &code);
#endif
    TEST_CHECK(fields > 0);
    if (strcmp(command, "accept") == 0) {
      TEST_CHECK(fields == 2 && argument < 16U);
      TEST_CHECK(test_next_request(&owner));
      TEST_CHECK(owner.event.handler_binding_id == 1U);
      TEST_CHECK(test_bytes_equal(coakka_http_host_request_method(&owner.event),
                                  "GET"));
      TEST_CHECK(coakka_http_host_accept_websocket(
                     owner.service, owner.event.exchange, test_text("chat")) ==
                 COAKKA_HTTP_HOST_OK);
      TEST_CHECK(test_release(&owner));
      coakka_http_host_websocket_event_t event;
      coakka_http_host_websocket_event_init(&event);
      TEST_CHECK(coakka_http_host_take_websocket(owner.service, READ_TIMEOUT,
                                                 &event) ==
                 COAKKA_HTTP_HOST_OK);
      sessions[argument] = event.session;
      const int opened = event.kind == COAKKA_HTTP_HOST_WEBSOCKET_OPEN;
      TEST_CHECK(coakka_http_host_release_websocket(owner.service, &event) ==
                 COAKKA_HTTP_HOST_OK);
      TEST_CHECK(opened);
    } else if (strcmp(command, "capture") == 0) {
      TEST_CHECK(test_next_request(&owner));
      TEST_CHECK(owner.event.handler_binding_id == 1U);
      pending = owner.event.exchange;
      TEST_CHECK(test_release(&owner));
    } else if (strcmp(command, "cancelled") == 0) {
      do {
        TEST_CHECK(test_take(&owner, READ_TIMEOUT) == COAKKA_HTTP_HOST_OK);
        TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL);
        const int matched =
            owner.event.exchange.slot == pending.slot &&
            owner.event.exchange.generation == pending.generation;
        TEST_CHECK(test_release(&owner));
        if (matched)
          break;
      } while (1);
      TEST_CHECK(coakka_http_host_accept_websocket(owner.service, pending,
                                                   test_text("chat")) ==
                 COAKKA_HTTP_HOST_STALE_EXCHANGE);
    } else if (strcmp(command, "echo") == 0 || strcmp(command, "closed") == 0 ||
               strcmp(command, "binary") == 0 || strcmp(command, "ping") == 0 ||
               strcmp(command, "hold") == 0) {
      TEST_CHECK(fields >= 2 && argument < 16U && !retain);
      do {
        coakka_http_host_websocket_event_init(&retained);
        TEST_CHECK(coakka_http_host_take_websocket(owner.service, READ_TIMEOUT,
                                                   &retained) ==
                   COAKKA_HTTP_HOST_OK);
        retain = 1;
        if (retained.kind != COAKKA_HTTP_HOST_WEBSOCKET_WRITABLE)
          break;
        TEST_CHECK(coakka_http_host_release_websocket(
                       owner.service, &retained) == COAKKA_HTTP_HOST_OK);
        retain = 0;
      } while (1);
      TEST_CHECK(same_session(retained.session, sessions[argument]));
      if (strcmp(command, "closed") == 0) {
        TEST_CHECK(fields == 3 &&
                   retained.kind == COAKKA_HTTP_HOST_WEBSOCKET_CLOSE &&
                   retained.close_code == code);
      } else if (strcmp(command, "binary") == 0 ||
                 strcmp(command, "ping") == 0) {
        const int binary = strcmp(command, "binary") == 0;
        TEST_CHECK(retained.kind == (binary ? COAKKA_HTTP_HOST_WEBSOCKET_BINARY
                                            : COAKKA_HTTP_HOST_WEBSOCKET_PING));
        const coakka_http_host_bytes_t expected =
            test_text(binary ? "bytes" : "pulse");
        TEST_CHECK(retained.data.size == expected.size &&
                   memcmp(retained.data.data, expected.data, expected.size) ==
                       0);
        TEST_CHECK(coakka_http_host_send_websocket(
                       owner.service, retained.session,
                       binary ? COAKKA_HTTP_HOST_WEBSOCKET_BINARY
                              : COAKKA_HTTP_HOST_WEBSOCKET_PONG,
                       expected) == COAKKA_HTTP_HOST_OK);
      } else {
        TEST_CHECK(retained.kind == COAKKA_HTTP_HOST_WEBSOCKET_TEXT &&
                   test_bytes_equal(retained.data, "hello"));
        if (strcmp(command, "echo") == 0) {
          TEST_CHECK(coakka_http_host_send_websocket(
                         owner.service, retained.session,
                         COAKKA_HTTP_HOST_WEBSOCKET_TEXT,
                         test_text("world")) == COAKKA_HTTP_HOST_OK);
        }
      }
      if (strcmp(command, "hold") != 0) {
        TEST_CHECK(coakka_http_host_release_websocket(
                       owner.service, &retained) == COAKKA_HTTP_HOST_OK);
        retain = 0;
      }
    } else if (strcmp(command, "reset-held") == 0) {
      TEST_CHECK(fields == 2 && argument < 16U);
      TEST_CHECK(retain && test_bytes_equal(retained.data, "hello"));
      coakka_http_host_websocket_event_t close;
      coakka_http_host_websocket_event_init(&close);
      TEST_CHECK(coakka_http_host_take_websocket(owner.service, READ_TIMEOUT,
                                                 &close) ==
                 COAKKA_HTTP_HOST_OK);
      const int closed = close.kind == COAKKA_HTTP_HOST_WEBSOCKET_CLOSE &&
                         close.close_code == 1006U &&
                         same_session(close.session, sessions[argument]);
      TEST_CHECK(coakka_http_host_release_websocket(owner.service, &close) ==
                 COAKKA_HTTP_HOST_OK);
      TEST_CHECK(closed);
      TEST_CHECK(retain && test_bytes_equal(retained.data, "hello"));
      TEST_CHECK(coakka_http_host_release_websocket(owner.service, &retained) ==
                 COAKKA_HTTP_HOST_OK);
      retain = 0;
    } else if (strcmp(command, "empty") == 0) {
      coakka_http_host_websocket_event_init(&retained);
      TEST_CHECK(!retain && coakka_http_host_take_websocket(owner.service, 0U,
                                                            &retained) ==
                                COAKKA_HTTP_HOST_TIMEOUT);
    } else if (strcmp(command, "publish") == 0) {
      coakka_http_host_route_identity_t identity;
      coakka_http_host_route_state_t states[2];
      coakka_http_host_publication_request_t request;
      coakka_http_host_publication_outcome_t outcome;
      coakka_http_host_route_identity_init(&identity);
      TEST_CHECK(coakka_http_host_route_snapshot(owner.service, states, 2U,
                                                 &identity) ==
                 COAKKA_HTTP_HOST_OK);
      coakka_http_host_publication_request_init(&request);
      coakka_http_host_publication_outcome_init(&outcome);
      request.activation_id = 23U;
      request.expected_route_generation = identity.route_generation;
      request.expected_metadata_generation = identity.metadata_generation;
      request.expected_binding_change_sequence =
          identity.binding_change_sequence;
      request.routes = routes;
      request.route_count = 2U;
      TEST_CHECK(coakka_http_host_publish_routes(
                     owner.service, &request, &outcome) == COAKKA_HTTP_HOST_OK);
      TEST_CHECK(outcome.code == COAKKA_HTTP_HOST_CONTROL_APPLIED);
    } else if (strcmp(command, "check-refusal") == 0) {
      coakka_http_host_tls_reload_request_t reload;
      coakka_http_host_tls_reload_outcome_t outcome;
      coakka_http_host_tls_reload_request_init(&reload);
      coakka_http_host_tls_reload_outcome_init(&outcome);
      reload.expected_generation = config.credential_generation;
      reload.security = config.security;
      reload.credential_id = config.credential_id;
      reload.certificate_chain_file = config.certificate_chain_file;
      reload.private_key_file = config.private_key_file;
      TEST_CHECK(
          coakka_http_host_tls_reload(owner.service, &reload, &outcome) ==
          COAKKA_HTTP_HOST_UNSUPPORTED);
    } else if (strcmp(command, "hold-request") == 0) {
      TEST_CHECK(test_next_request(&owner));
      TEST_CHECK(owner.event.handler_binding_id == 2U);
    } else if (strcmp(command, "respond") == 0 ||
               strcmp(command, "respond-held") == 0) {
      if (strcmp(command, "respond") == 0) {
        TEST_CHECK(test_next_request(&owner));
      }
      TEST_CHECK(owner.lease_live && owner.event.handler_binding_id == 2U);
      TEST_CHECK(test_respond(&owner, 200U, "sibling"));
      TEST_CHECK(test_release(&owner));
    } else if (strcmp(command, "drain") == 0) {
      TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) ==
                 COAKKA_HTTP_HOST_OK);
    } else if (strcmp(command, "wait") == 0) {
      coakka_http_host_drain_state_t state = 0U;
      TEST_CHECK(fields == 2);
      const coakka_http_host_status_t status =
          coakka_http_host_service_wait_drained(owner.service, READ_TIMEOUT,
                                                &state);
      TEST_CHECK(state == argument);
      TEST_CHECK(status == (state == COAKKA_HTTP_HOST_DRAIN_COMPLETE
                                ? COAKKA_HTTP_HOST_OK
                            : state == COAKKA_HTTP_HOST_DRAIN_ABORTED
                                ? COAKKA_HTTP_HOST_CLOSED
                                : COAKKA_HTTP_HOST_TIMEOUT));
    } else if (strcmp(command, "stop") == 0) {
      TEST_CHECK(coakka_http_host_service_stop(owner.service) ==
                 COAKKA_HTTP_HOST_OK);
      TEST_CHECK(coakka_http_host_service_stop(owner.service) ==
                 COAKKA_HTTP_HOST_OK);
    } else if (strcmp(command, "release") == 0) {
      TEST_CHECK(retain);
      TEST_CHECK(coakka_http_host_service_destroy(&owner.service) !=
                 COAKKA_HTTP_HOST_OK);
      TEST_CHECK(owner.service != NULL &&
                 test_bytes_equal(retained.data, "hello"));
      TEST_CHECK(coakka_http_host_release_websocket(owner.service, &retained) ==
                 COAKKA_HTTP_HOST_OK);
      retain = 0;
    } else if (strcmp(command, "close") == 0) {
      TEST_CHECK(fields == 2 && argument < 16U);
      TEST_CHECK(coakka_http_host_close_websocket(
                     owner.service, sessions[argument], 1000U, test_text("")) ==
                 COAKKA_HTTP_HOST_OK);
    } else if (strcmp(command, "finish") == 0) {
      TEST_CHECK(test_service_close(&owner));
      config.port = port;
      TEST_CHECK(test_service_open(&owner, &config, routes, 2U));
      TEST_CHECK(test_service_close(&owner));
      TEST_CHECK(write_line("ok"));
      goto cleanup;
    } else {
      TEST_CHECK(0);
    }
    TEST_CHECK(write_line("ok"));
  }
  TEST_CHECK(0);

cleanup:
  if (retain && owner.service != NULL)
    ok &= coakka_http_host_release_websocket(owner.service, &retained) ==
          COAKKA_HTTP_HOST_OK;
  ok &= test_service_close(&owner);
  if (root_created)
    ok &= remove(root) == 0;
  if (cert_created)
    ok &= remove(cert) == 0;
  if (key_created)
    ok &= remove(key) == 0;
  if (platform_open)
    ok &= test_platform_close();
  return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
