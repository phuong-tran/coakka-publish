#include "test_support.h"
#include "test_tls_material.h"

_Static_assert(COAKKA_HTTP_HOST_ABI_VERSION == 10U,
               "V1 must remain an additive ABI 10 interface");

static const char certificate_path[] = "v1-fixture-certificate.pem";
static const char private_key_path[] = "v1-fixture-key.pem";

static int
exercise_listener(const coakka_http_host_multi_listener_api_v1_t *api,
                  test_service_t *owner, const char *address, uint8_t family) {
  const uint32_t all_protocols = COAKKA_HTTP_HOST_MULTI_LISTENER_HTTP_1_1 |
                                 COAKKA_HTTP_HOST_MULTI_LISTENER_HTTP_2 |
                                 COAKKA_HTTP_HOST_MULTI_LISTENER_HTTP_3;
  const uint32_t both_transports =
      COAKKA_HTTP_HOST_MULTI_LISTENER_TCP | COAKKA_HTTP_HOST_MULTI_LISTENER_UDP;
  coakka_http_host_configuration_t config;
  coakka_http_host_route_t route;
  int ok = 1;

  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text(address);
  config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
  config.credential_generation = 1U;
  config.credential_id = test_text("v1-fixture");
  config.certificate_chain_file = test_text(certificate_path);
  config.private_key_file = test_text(private_key_path);
  coakka_http_host_route_init(&route);
  route.route_id = 1U;
  route.handler_binding_id = 1U;
  route.method = test_text("GET");
  route.encoded_path_pattern = test_text("/lifecycle");

  for (unsigned cycle = 0U; cycle < 3U; ++cycle) {
    uint32_t failed_index = COAKKA_HTTP_HOST_FAILED_INDEX_NONE;
    coakka_http_host_multi_listener_info_v1_t info = {0};
    coakka_http_host_issue_t issue;
    TEST_CHECK(api->create(&config, &route, 1U, &owner->service,
                           &failed_index) == COAKKA_HTTP_HOST_OK);
    info.struct_size = sizeof(info) - 1U;
    info.port = UINT16_MAX;
    TEST_CHECK(api->inspect(owner->service, &info) ==
               COAKKA_HTTP_HOST_INVALID_ARGUMENT);
    TEST_CHECK(info.port == UINT16_MAX);
    info.struct_size = sizeof(info);
    TEST_CHECK(api->inspect(owner->service, &info) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(info.lifecycle == COAKKA_HTTP_HOST_MULTI_LISTENER_CREATED &&
               info.accepted_protocols == all_protocols &&
               info.admitting_protocols == 0U && info.bound_transports == 0U &&
               info.port == 0U);

    coakka_http_host_issue_init(&issue);
    TEST_CHECK(coakka_http_host_service_start(owner->service, &issue) ==
               COAKKA_HTTP_HOST_OK);
    TEST_CHECK(api->inspect(owner->service, &info) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(info.lifecycle == COAKKA_HTTP_HOST_MULTI_LISTENER_RUNNING &&
               info.admitting_protocols == all_protocols &&
               info.bound_transports == both_transports && info.port != 0U &&
               info.address_family == family &&
               info.security == COAKKA_HTTP_HOST_SECURITY_TLS);
    // Start on an ephemeral port; later generations must reclaim that exact
    // pair.
    if (cycle == 0U)
      config.port = info.port;
    TEST_CHECK(info.port == config.port);

    if (cycle == 0U) {
      coakka_http_host_drain_state_t drained =
          COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
      TEST_CHECK(coakka_http_host_service_begin_drain(owner->service) ==
                 COAKKA_HTTP_HOST_OK);
      TEST_CHECK(coakka_http_host_service_wait_drained(
                     owner->service, 5000U, &drained) == COAKKA_HTTP_HOST_OK);
      TEST_CHECK(drained == COAKKA_HTTP_HOST_DRAIN_COMPLETE);
    }
    if (cycle != 2U) {
      TEST_CHECK(coakka_http_host_service_stop(owner->service) ==
                 COAKKA_HTTP_HOST_OK);
      TEST_CHECK(coakka_http_host_service_stop(owner->service) ==
                 COAKKA_HTTP_HOST_OK);
      TEST_CHECK(api->inspect(owner->service, &info) == COAKKA_HTTP_HOST_OK);
      TEST_CHECK(info.lifecycle == COAKKA_HTTP_HOST_MULTI_LISTENER_STOPPED &&
                 info.admitting_protocols == 0U &&
                 info.bound_transports == 0U && info.port == 0U);
    }
    // The last cycle exercises destroy while running; the caller owns no lease.
    TEST_CHECK(test_service_close(owner));
  }

cleanup:
  return ok;
}

int main(void) {
  coakka_http_host_multi_listener_api_v1_t api = {0};
  test_service_t owner;
  int certificate_created = 0;
  int key_created = 0;
  int ok = 1;
  int cleanup_ok = 1;
  if (!test_platform_open())
    return 1;
  test_service_init(&owner);

  TEST_CHECK(coakka_http_host_abi_version() == 10U);
  api.struct_size = sizeof(api) - 1U;
  api.supported_profiles = UINT32_MAX;
  TEST_CHECK(coakka_http_host_multi_listener_get_api_v1(&api) ==
             COAKKA_HTTP_HOST_INVALID_ARGUMENT);
  TEST_CHECK(api.supported_profiles == UINT32_MAX);
  api.struct_size = sizeof(api);
  TEST_CHECK(coakka_http_host_multi_listener_get_api_v1(&api) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(api.supported_profiles ==
                 COAKKA_HTTP_HOST_MULTI_LISTENER_HTTPS_ALL_THREE &&
             api.create != NULL && api.inspect != NULL);
  TEST_CHECK(test_write_material(certificate_path, test_tls_certificate_a,
                                 &certificate_created));
  TEST_CHECK(
      test_write_material(private_key_path, test_tls_key_a, &key_created));
  TEST_CHECK(exercise_listener(&api, &owner, "127.0.0.1", 4U));
  TEST_CHECK(exercise_listener(&api, &owner, "::1", 6U));

cleanup:
  // Do not restore the controller's placement if Core still retains a handle.
  if (!test_service_close(&owner)) {
    (void)fprintf(stderr, "V1 service teardown retained an owner\n");
    return 1;
  }
  if (certificate_created && remove(certificate_path) != 0)
    cleanup_ok = 0;
  if (key_created && remove(private_key_path) != 0)
    cleanup_ok = 0;
  return test_finish("multi_listener_v1_lifecycle", ok, cleanup_ok);
}
