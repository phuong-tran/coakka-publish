#include "test_support.h"
#include "test_tls_material.h"

#include <string.h>

/* The Python driver owns a unique working directory, its clients and their
 * deadline. This process owns the Core handle and reads all request leases. */
int main(int argc, char **argv) {
  const char certificate[] = "certificate.pem";
  const char key[] = "key.pem";
  const char root[] = "root.pem";
  const uint32_t all = COAKKA_HTTP_HOST_MULTI_LISTENER_HTTP_1_1 |
                       COAKKA_HTTP_HOST_MULTI_LISTENER_HTTP_2 |
                       COAKKA_HTTP_HOST_MULTI_LISTENER_HTTP_3;
  coakka_http_host_configuration_t config;
  coakka_http_host_route_t route;
  test_service_t owner;
  uint16_t port = 0U;
  int certificate_created = 0;
  int key_created = 0;
  int root_created = 0;
  int ok = 1;
  int cleanup_ok = 1;
  int mixed;
  if (argc != 2) return 1;
  coakka_http_host_configuration_init(&config);
  mixed = strcmp(argv[1], "mixed") == 0;
  if (strcmp(argv[1], "1.1") == 0)
    config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_1_1;
  else if (strcmp(argv[1], "2") == 0)
    config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_2;
  else if (strcmp(argv[1], "3") == 0)
    config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_3;
  else if (!mixed)
    return 1;

  if (!test_platform_open()) return 1;
  test_service_init(&owner);
  TEST_CHECK(coakka_http_host_abi_version() == 10U);
  TEST_CHECK(test_write_material(certificate, test_tls_certificate_a,
                                 &certificate_created));
  TEST_CHECK(test_write_material(key, test_tls_key_a, &key_created));
  TEST_CHECK(test_write_material(root, test_tls_root_a, &root_created));
  config.bind_address = test_text("127.0.0.1");
  config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
  config.credential_generation = 1U;
  config.credential_id = test_text("protocol-test");
  config.certificate_chain_file = test_text(certificate);
  config.private_key_file = test_text(key);
  coakka_http_host_route_init(&route);
  route.route_id = 1U;
  route.handler_binding_id = 1U;
  route.method = test_text("GET");
  route.encoded_path_pattern = test_text("/protocol");

  if (mixed) {
    coakka_http_host_multi_listener_api_v1_t api = {0};
    coakka_http_host_multi_listener_info_v1_t info = {0};
    coakka_http_host_issue_t issue;
    uint32_t failed = COAKKA_HTTP_HOST_FAILED_INDEX_NONE;
    api.struct_size = sizeof(api);
    info.struct_size = sizeof(info);
    coakka_http_host_issue_init(&issue);
    TEST_CHECK(coakka_http_host_multi_listener_get_api_v1(&api) ==
               COAKKA_HTTP_HOST_OK);
    TEST_CHECK((api.supported_profiles &
                COAKKA_HTTP_HOST_MULTI_LISTENER_HTTPS_ALL_THREE) != 0U &&
               api.create != NULL && api.inspect != NULL);
    TEST_CHECK(api.create(&config, &route, 1U, &owner.service, &failed) ==
               COAKKA_HTTP_HOST_OK);
    TEST_CHECK(coakka_http_host_service_start(owner.service, &issue) ==
               COAKKA_HTTP_HOST_OK);
    TEST_CHECK(api.inspect(owner.service, &info) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(info.lifecycle == COAKKA_HTTP_HOST_MULTI_LISTENER_RUNNING &&
               info.admitting_protocols == all && info.port != 0U);
    port = info.port;
  } else {
    TEST_CHECK(test_service_open(&owner, &config, &route, 1U));
    TEST_CHECK(coakka_http_host_service_port(owner.service, &port) ==
               COAKKA_HTTP_HOST_OK && port != 0U);
  }
  /* V1 publishes its confirmed startup admission set. ABI 10 standalone
   * startup does not expose a comparable per-listener protocol snapshot. */
  (void)printf("{\"port\":%u,\"admitting_protocols\":%s}\n", (unsigned)port,
               mixed ? "[\"HTTP/1.1\",\"HTTP/2\",\"HTTP/3\"]" : "null");
  TEST_CHECK(fflush(stdout) == 0);

  for (unsigned i = 0U; i < (mixed ? 3U : 1U); ++i) {
    TEST_CHECK(test_next_request(&owner));
    TEST_CHECK(coakka_http_host_request_route_id(&owner.event) == route.route_id &&
               owner.event.handler_binding_id == route.handler_binding_id);
    TEST_CHECK(test_respond(&owner, 200U, "protocol-ok"));
    TEST_CHECK(test_release(&owner));
  }

  /* Wait for the driver to verify the wire replies before closing the sockets.
   * The driver reaps this process on failure and CTest bounds the whole test. */
  TEST_CHECK(getchar() == '\n');

cleanup:
  if (!test_service_close(&owner)) return 1;
  if (certificate_created && remove(certificate) != 0) cleanup_ok = 0;
  if (key_created && remove(key) != 0) cleanup_ok = 0;
  if (root_created && remove(root) != 0) cleanup_ok = 0;
  return test_finish("listener_protocols", ok, cleanup_ok);
}
