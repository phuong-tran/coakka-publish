#if defined(__cplusplus)
extern "C" {
#endif
#include "test_support.h"
#if defined(__cplusplus)
}
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

static int check_limits(const test_service_t *owner, uint32_t connections,
                        uint32_t active) {
  coakka_http_host_limits_t limits;
  coakka_http_host_limits_init(&limits);
  if (coakka_http_host_service_limits(owner->service, &limits) !=
      COAKKA_HTTP_HOST_OK) {
    return 0;
  }
  if (limits.max_connections != connections || limits.max_active_requests != active) {
    fprintf(stderr, "Reported connections=%" PRIu32 " active=%" PRIu32
            ", expected connections=%" PRIu32 " active=%" PRIu32 "\n",
            limits.max_connections, limits.max_active_requests, connections, active);
    return 0;
  }
  return 1;
}

static int same_exchange(coakka_http_host_exchange_t first,
                         coakka_http_host_exchange_t second) {
  return first.slot == second.slot && first.generation == second.generation;
}

int main(int argc, char **argv) {
  if (argc != 8 || coakka_http_host_abi_version() != 10U || !test_platform_open()) {
    return EXIT_FAILURE;
  }
  const unsigned long port = strtoul(argv[1], NULL, 10);
  const uint32_t requested = (uint32_t)strtoul(argv[2], NULL, 10);
  const uint32_t expected = (uint32_t)strtoul(argv[3], NULL, 10);
  const uint32_t active = (uint32_t)strtoul(argv[4], NULL, 10);
  const uint8_t uring = (uint8_t)(strcmp(argv[7], "uring") == 0);
  coakka_http_host_configuration_t configuration;
  coakka_http_host_configuration_init(&configuration);
  configuration.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_3;
  configuration.security = COAKKA_HTTP_HOST_SECURITY_TLS;
  configuration.bind_address = test_text("127.0.0.1");
  configuration.port = (uint16_t)port;
  configuration.credential_id = test_text("capacity");
  configuration.credential_generation = 1U;
  configuration.certificate_chain_file = test_text(argv[5]);
  configuration.private_key_file = test_text(argv[6]);
  configuration.use_io_uring = uring;
  configuration.limits.max_connections = requested;
  configuration.limits.max_active_requests = active;
  configuration.limits.handler_timeout_ms = 10000U;
  coakka_http_host_route_t route;
  coakka_http_host_route_init(&route);
  route.route_id = route.handler_binding_id = 1U;
  route.method = test_text("GET");
  route.encoded_path_pattern = test_text("/capacity");
  test_service_t owner;
  test_service_init(&owner);
  coakka_http_host_issue_t issue;
  coakka_http_host_issue_init(&issue);
  coakka_http_host_service_info_t info;
  coakka_http_host_service_info_init(&info);
  uint32_t failed = COAKKA_HTTP_HOST_FAILED_INDEX_NONE;
  uint16_t bound_port = 0U;
  char command[32];
  int ok = 1;
  int stopped = 0;

  TEST_CHECK(port > 0UL && port <= 65535UL);
  TEST_CHECK(coakka_http_host_service_create(&configuration, &route, 1U,
                                            &owner.service, &failed) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(check_limits(&owner, expected, active));
  TEST_CHECK(coakka_http_host_service_start(owner.service, &issue) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(coakka_http_host_service_port(owner.service, &bound_port) ==
             COAKKA_HTTP_HOST_OK && bound_port == (uint16_t)port);
  TEST_CHECK(check_limits(&owner, expected, active));
  TEST_CHECK(coakka_http_host_service_info(owner.service, &info) ==
             COAKKA_HTTP_HOST_OK && info.io_uring_effective == uring &&
             info.abi_version == 10U);
  printf("READY %u connections=%" PRIu32 " active=%" PRIu32 "\n",
         (unsigned)bound_port, expected, active);
  (void)fflush(stdout);

  while (fgets(command, (int)sizeof(command), stdin) != NULL) {
    if (strcmp(command, "take\n") == 0) {
      TEST_CHECK(!owner.lease_live && test_next_request(&owner));
      TEST_CHECK(coakka_http_host_request_route_id(&owner.event) == 1U);
      puts("ADMITTED");
    } else if (strcmp(command, "reply\n") == 0) {
      TEST_CHECK(owner.lease_live);
      const coakka_http_host_exchange_t exchange = owner.event.exchange;
      TEST_CHECK(test_respond(&owner, 200U, "accepted"));
      TEST_CHECK(test_release(&owner));
      TEST_CHECK(test_take(&owner, 3000U) == COAKKA_HTTP_HOST_OK &&
                 owner.event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
                 same_exchange(exchange, owner.event.exchange));
      TEST_CHECK(test_release(&owner));
      puts("TERMINAL");
    } else if (strcmp(command, "stop\n") == 0) {
      TEST_CHECK(!owner.lease_live);
      TEST_CHECK(coakka_http_host_service_stop(owner.service) ==
                 COAKKA_HTTP_HOST_OK);
      TEST_CHECK(check_limits(&owner, expected, active));
      TEST_CHECK(test_service_close(&owner));
      TEST_CHECK(test_service_open(&owner, &configuration, &route, 1U));
      TEST_CHECK(coakka_http_host_service_stop(owner.service) ==
                 COAKKA_HTTP_HOST_OK && test_service_close(&owner));
      stopped = 1;
      puts("STOPPED AND REOPENED");
      (void)fflush(stdout);
      break;
    } else {
      TEST_CHECK(0);
    }
    (void)fflush(stdout);
  }
  TEST_CHECK(stopped);

cleanup:
  return test_finish("h3-connection-capacity", ok, test_service_close(&owner));
}
