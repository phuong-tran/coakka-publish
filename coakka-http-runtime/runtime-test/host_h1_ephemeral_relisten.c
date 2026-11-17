#if defined(__cplusplus)
extern "C" {
#endif
#include "test_support.h"
#if defined(__cplusplus)
}
#endif
#include "test_tls_material.h"

#include <stdlib.h>
#include <string.h>

static int serve_one(test_service_t *owner) {
  return test_next_request(owner) &&
         test_bytes_equal(coakka_http_host_request_target(&owner->event), "/relisten") &&
         test_respond(owner, 200U, "relisten-ok") && test_release(owner);
}

static int occupied_port_is_refused(
    const coakka_http_host_configuration_t *config,
    const coakka_http_host_route_t *route) {
  test_service_t contender;
  coakka_http_host_issue_t issue;
  uint32_t failed = COAKKA_HTTP_HOST_FAILED_INDEX_NONE;
  test_service_init(&contender);
  coakka_http_host_issue_init(&issue);
  if (coakka_http_host_service_create(config, route, 1U, &contender.service,
                                     &failed) != COAKKA_HTTP_HOST_OK) {
    return 0;
  }
  const coakka_http_host_status_t status =
      coakka_http_host_service_start(contender.service, &issue);
  const int closed = test_service_close(&contender);
  return status == COAKKA_HTTP_HOST_IO_ERROR &&
         issue.reason == COAKKA_HTTP_HOST_ISSUE_LISTENER_START && closed;
}

int main(int argc, char **argv) {
  coakka_http_host_configuration_t config;
  coakka_http_host_route_t route;
  test_service_t owner;
  const char certificate[] = "certificate.pem";
  const char key[] = "key.pem";
  const char root[] = "root.pem";
  int certificate_created = 0;
  int key_created = 0;
  int root_created = 0;
  uint16_t actual_port = 0U;
  uint16_t reopened_port = 0U;
  int ok = 1;
  int cleanup_ok = 1;
  if (argc != 5 ||
      (strcmp(argv[1], "127.0.0.1") != 0 && strcmp(argv[1], "::1") != 0) ||
      (strcmp(argv[2], "plain") != 0 && strcmp(argv[2], "tls") != 0 &&
       strcmp(argv[2], "mtls") != 0) ||
      (strcmp(argv[4], "request") != 0 && strcmp(argv[4], "idle") != 0)) {
    return 1;
  }
  char *end = NULL;
  const unsigned long initial_port = strtoul(argv[3], &end, 10);
  if (end == argv[3] || *end != '\0' || initial_port > UINT16_MAX ||
      !test_platform_open()) {
    return 1;
  }
  const int tls = strcmp(argv[2], "plain") != 0;
  const int mtls = strcmp(argv[2], "mtls") == 0;
  const int traffic = strcmp(argv[4], "request") == 0;
  test_service_init(&owner);
  TEST_CHECK(coakka_http_host_abi_version() == 10U);
  coakka_http_host_configuration_init(&config);
  config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_1_1;
  config.bind_address = test_text(argv[1]);
  config.port = (uint16_t)initial_port;
  if (tls) {
    TEST_CHECK(test_write_material(certificate, test_tls_certificate_a,
                                   &certificate_created));
    TEST_CHECK(test_write_material(key, test_tls_key_a, &key_created));
    TEST_CHECK(test_write_material(root, test_tls_root_a, &root_created));
    config.security = mtls ? COAKKA_HTTP_HOST_SECURITY_MUTUAL_TLS
                           : COAKKA_HTTP_HOST_SECURITY_TLS;
    config.credential_generation = 1U;
    config.credential_id = test_text("relisten-test");
    config.certificate_chain_file = test_text(certificate);
    config.private_key_file = test_text(key);
    if (mtls) config.trust_roots_file = test_text(root);
  }
  coakka_http_host_route_init(&route);
  route.route_id = route.handler_binding_id = 1U;
  route.method = test_text("GET");
  route.encoded_path_pattern = test_text("/relisten");
  TEST_CHECK(test_service_open(&owner, &config, &route, 1U));
  TEST_CHECK(coakka_http_host_service_port(owner.service, &actual_port) ==
             COAKKA_HTTP_HOST_OK && actual_port != 0U);
  TEST_CHECK(initial_port == 0U || actual_port == initial_port);
  config.port = actual_port;
  TEST_CHECK(occupied_port_is_refused(&config, &route));
  TEST_CHECK(printf("READY %u\n", (unsigned)actual_port) > 0 && fflush(stdout) == 0);
  if (traffic) TEST_CHECK(serve_one(&owner));
  /* The driver waits for EOF from the peer before asking Core to destroy
   * the owner. This keeps the active close on the server side. */
  TEST_CHECK(getchar() == '\n');
  TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(test_service_close(&owner));
  TEST_CHECK(puts("CLOSED") >= 0 && fflush(stdout) == 0);
  TEST_CHECK(getchar() == '\n');
  TEST_CHECK(test_service_open(&owner, &config, &route, 1U));
  TEST_CHECK(coakka_http_host_service_port(owner.service, &reopened_port) ==
             COAKKA_HTTP_HOST_OK && reopened_port == actual_port);
  TEST_CHECK(printf("REOPENED %u\n", (unsigned)reopened_port) > 0 && fflush(stdout) == 0);
  TEST_CHECK(serve_one(&owner));
  TEST_CHECK(getchar() == '\n');
  TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);

cleanup:
  if (!test_service_close(&owner)) return 1;
  if (certificate_created && remove(certificate) != 0) cleanup_ok = 0;
  if (key_created && remove(key) != 0) cleanup_ok = 0;
  if (root_created && remove(root) != 0) cleanup_ok = 0;
  return test_finish("standalone_h1_ephemeral_relisten", ok, cleanup_ok);
}
