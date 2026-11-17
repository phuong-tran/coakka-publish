#include "test_support.h"
#include "test_tls_material.h"

#include <string.h>

static const char *const paths[] = {
    "a-cert.pem", "a-key.pem", "a-root.pem",
    "b-cert.pem", "b-key.pem", "b-root.pem"};
static const char *const contents[] = {
    test_tls_certificate_a, test_tls_key_a, test_tls_root_a,
    test_tls_certificate_b, test_tls_key_b, test_tls_root_b};

static int acknowledge(const char *message) {
  return puts(message) >= 0 && fflush(stdout) == 0;
}

static int rotate(test_service_t *server, coakka_http_host_security_t security,
                  const char *command) {
  coakka_http_host_tls_reload_request_t request;
  coakka_http_host_tls_reload_outcome_t outcome;
  const int graceful = strcmp(command, "keep\n") == 0;
  const int drain = strcmp(command, "drain\n") == 0;
  const int stale = strcmp(command, "stale\n") == 0;
  const int bad_trust = strcmp(command, "bad-trust\n") == 0;
  const uint64_t old_generation = drain ? 2U : 1U;
  const uint64_t next_generation = stale ? old_generation : old_generation + 1U;
  const char *const id = drain ? "identity-a-drain" : "identity-b";
  const unsigned offset = drain ? 0U : 3U;
  int ok = 1;

  coakka_http_host_tls_reload_request_init(&request);
  coakka_http_host_tls_reload_outcome_init(&outcome);
  request.security = security;
  request.mode = drain ? COAKKA_HTTP_HOST_TLS_RELOAD_DRAIN_EXISTING
                       : COAKKA_HTTP_HOST_TLS_RELOAD_GRACEFUL;
  request.expected_generation = old_generation;
  request.credential_generation = next_generation;
  request.credential_id = test_text(id);
  request.certificate_chain_file = test_text(paths[offset]);
  request.private_key_file = test_text(
      (graceful || drain || stale || bad_trust) ? paths[offset + 1U] : paths[1]);
  if (security == COAKKA_HTTP_HOST_SECURITY_MUTUAL_TLS) {
    request.trust_roots_file =
        test_text(bad_trust ? "missing-trust.pem" : paths[offset + 2U]);
  }
  TEST_CHECK(coakka_http_host_tls_reload(server->service, &request, &outcome) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(outcome.effective.security == security);
  if (graceful || drain) {
    TEST_CHECK(outcome.reason == COAKKA_HTTP_HOST_TLS_RELOAD_APPLIED);
    TEST_CHECK(outcome.changed == 1U);
    TEST_CHECK(outcome.effective.generation == next_generation);
    TEST_CHECK(outcome.effective.drain_before_generation ==
               (drain ? next_generation : 0U));
    TEST_CHECK(strcmp(outcome.effective.credential_id, id) == 0);
  } else {
    TEST_CHECK(outcome.reason == (stale ? COAKKA_HTTP_HOST_TLS_RELOAD_STALE_GENERATION
                                        : COAKKA_HTTP_HOST_TLS_RELOAD_PREPARATION_REJECTED));
    TEST_CHECK(outcome.changed == 0U);
    TEST_CHECK(outcome.effective.generation == old_generation);
    TEST_CHECK(outcome.effective.drain_before_generation == 0U);
    TEST_CHECK(strcmp(outcome.effective.credential_id, "identity-a") == 0);
  }
cleanup:
  return ok;
}

/* A valid earlier request may still have a completion queued. Only a new
 * request event shows that the untrusted connection reached the app. */
static int no_request_admitted(test_service_t *server) {
  for (unsigned index = 0U; index < 8U; ++index) {
    const coakka_http_host_status_t status = test_take(server, 25U);
    if (status == COAKKA_HTTP_HOST_TIMEOUT) return 1;
    if (status != COAKKA_HTTP_HOST_OK) return 0;
    const int terminal =
        server->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL;
    if (!test_release(server) || !terminal) return 0;
  }
  return 0;
}

int main(int argc, char **argv) {
  int created[sizeof(paths) / sizeof(paths[0])] = {0};
  test_service_t server;
  coakka_http_host_configuration_t config;
  coakka_http_host_route_t route;
  char command[40];
  int ok = 1;
  int cleanup_ok = 1;
  uint16_t port = 0U;
  if (argc != 3 ||
      (strcmp(argv[1], "1.1") != 0 && strcmp(argv[1], "2") != 0 &&
       strcmp(argv[1], "3") != 0) ||
      (strcmp(argv[2], "tls") != 0 && strcmp(argv[2], "mtls") != 0) ||
      !test_platform_open()) return 1;
  test_service_init(&server);
  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text("127.0.0.1");
  config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_1_1;
  if (strcmp(argv[1], "2") == 0) config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_2;
  if (strcmp(argv[1], "3") == 0) config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_3;
  config.security = strcmp(argv[2], "mtls") == 0
                        ? COAKKA_HTTP_HOST_SECURITY_MUTUAL_TLS
                        : COAKKA_HTTP_HOST_SECURITY_TLS;
  config.credential_generation = 1U;
  config.credential_id = test_text("identity-a");
  config.certificate_chain_file = test_text(paths[0]);
  config.private_key_file = test_text(paths[1]);
  if (config.security == COAKKA_HTTP_HOST_SECURITY_MUTUAL_TLS)
    config.trust_roots_file = test_text(paths[2]);
  for (size_t index = 0U; index < sizeof(created) / sizeof(created[0]); ++index)
    TEST_CHECK(test_write_material(paths[index], contents[index], &created[index]));
  coakka_http_host_route_init(&route);
  route.route_id = route.handler_binding_id = 1U;
  route.method = test_text("GET");
  route.encoded_path_pattern = test_text("/tls");
  TEST_CHECK(test_service_open(&server, &config, &route, 1U));
  TEST_CHECK(coakka_http_host_service_port(server.service, &port) ==
             COAKKA_HTTP_HOST_OK && port != 0U);
  TEST_CHECK(printf("%u\n", (unsigned)port) > 0 && fflush(stdout) == 0);
  while (fgets(command, sizeof(command), stdin) != NULL) {
    if (strcmp(command, "hold\n") == 0) {
      TEST_CHECK(test_next_request(&server));
      TEST_CHECK(test_bytes_equal(coakka_http_host_request_target(&server.event),
                                  "/tls"));
    } else if (strcmp(command, "respond\n") == 0) {
      TEST_CHECK(test_respond(&server, 200U, "rotation-ok"));
      TEST_CHECK(test_release(&server));
    } else if (strcmp(command, "no-request\n") == 0) {
      TEST_CHECK(!server.lease_live);
      TEST_CHECK(no_request_admitted(&server));
    } else if (strcmp(command, "finish\n") == 0) {
      TEST_CHECK(!server.lease_live);
      TEST_CHECK(coakka_http_host_service_stop(server.service) ==
                 COAKKA_HTTP_HOST_OK);
      TEST_CHECK(acknowledge("ok"));
      goto cleanup;
    } else {
      TEST_CHECK(strcmp(command, "keep\n") == 0 ||
                 strcmp(command, "drain\n") == 0 ||
                 strcmp(command, "stale\n") == 0 ||
                 strcmp(command, "bad-key\n") == 0 ||
                 strcmp(command, "bad-trust\n") == 0);
      TEST_CHECK(rotate(&server, config.security, command));
    }
    TEST_CHECK(acknowledge("ok"));
  }
  ok = 0;
cleanup:
  if (!test_service_close(&server)) cleanup_ok = 0;
  for (size_t index = 0U; index < sizeof(created) / sizeof(created[0]); ++index)
    if (created[index] && remove(paths[index]) != 0) cleanup_ok = 0;
  return test_finish("standalone_tls_reload_wire", ok, cleanup_ok);
}
