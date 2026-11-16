#include "test_support.h"

#include <stdlib.h>
#include <string.h>

static int take_file(test_service_t *owner, coakka_http_host_exchange_t *out) {
  int ok = 1;
  coakka_http_host_request_view_t request;
  TEST_CHECK(test_next_request(owner));
  coakka_http_host_request_view_init(&request);
  TEST_CHECK(coakka_http_host_request_view(&owner->event, &request) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(test_bytes_equal(request.target, "/download"));
  *out = owner->event.exchange;
cleanup:
  return test_release(owner) && ok;
}

/* The file reply supplies an authority and a path, never file bytes. */
static int reply_file(test_service_t *owner, coakka_http_host_exchange_t exchange) {
  int ok = 1;
  coakka_http_host_file_response_t response;
  coakka_http_host_header_t header = {test_text("cache-control"),
                                      test_text("private, max-age=60")};
  coakka_http_host_file_response_init(&response);
  response.head.status_code = 200U;
  response.head.headers = &header;
  response.head.header_count = 1U;
  response.authority_id = 82U;
  response.encoded_path = test_text("/asset.txt");
  TEST_CHECK(coakka_http_host_respond_file(owner->service, exchange,
                                           &response) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(test_take(owner, 5000U) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL);
  TEST_CHECK(owner->event.exchange.slot == exchange.slot &&
             owner->event.exchange.generation == exchange.generation);
cleanup:
  return test_release(owner) && ok;
}

int main(int argc, char **argv) {
  const int controlled = argc == 6 && strcmp(argv[1], "3") == 0;
  const int sibling = controlled && strcmp(argv[5], "--sibling") == 0;
  const int one_file = controlled && strcmp(argv[5], "--one-file") == 0;
  if ((argc != 5 && !sibling && !one_file) ||
      coakka_http_host_abi_version() != 10U)
    return EXIT_FAILURE;
  test_service_t owner;
  test_service_init(&owner);
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text("127.0.0.1");
  if (strcmp(argv[1], "1.1") == 0) {
    config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_1_1;
  } else if (strcmp(argv[1], "2") == 0 || strcmp(argv[1], "3") == 0) {
    config.protocol = strcmp(argv[1], "2") == 0
                          ? COAKKA_HTTP_HOST_PROTOCOL_HTTP_2
                          : COAKKA_HTTP_HOST_PROTOCOL_HTTP_3;
    config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
    config.credential_id = test_text("files-wire-fixture");
    config.credential_generation = 1U;
    config.certificate_chain_file = test_text(argv[2]);
    config.private_key_file = test_text(argv[3]);
  } else {
    return EXIT_FAILURE;
  }

  coakka_http_host_static_mount_t mount;
  coakka_http_host_static_mount_init(&mount);
  mount.flags = COAKKA_HTTP_HOST_STATIC_SPA_FALLBACK;
  mount.url_prefix = test_text("/assets");
  mount.root_path = test_text(argv[4]);
  mount.index_file = test_text("index.html");
  mount.spa_fallback_file = mount.index_file;
  config.static_mounts = &mount;
  config.static_mount_count = 1U;

  coakka_http_host_file_authority_t authority;
  coakka_http_host_file_authority_init(&authority);
  authority.authority_id = 82U;
  authority.root_path = test_text(argv[4]);
  authority.max_active_files = 1U;
  authority.max_file_bytes = 65536U;
  config.file_authorities = &authority;
  config.file_authority_count = 1U;

  coakka_http_host_route_t routes[2];
  for (uint32_t i = 0; i < 2U; ++i) {
    coakka_http_host_route_init(&routes[i]);
    routes[i].route_id = i + 1U;
    routes[i].handler_binding_id = i + 1U;
    routes[i].method = test_text(i == 0U ? "GET" : "HEAD");
    routes[i].encoded_path_pattern = test_text("/download");
  }

  int ok = 1;
  uint16_t port = 0U;
  char command[16];
  uint32_t drained = 0U;
  uint64_t drain_start = 0U;
  uint64_t drain_end = 0U;
  if (!test_platform_open())
    return EXIT_FAILURE;
  TEST_CHECK(test_service_open(&owner, &config, routes, 2U));
  TEST_CHECK(coakka_http_host_service_port(owner.service, &port) ==
             COAKKA_HTTP_HOST_OK);
  (void)printf("READY %u\n", (unsigned)port);
  (void)fflush(stdout);
  TEST_CHECK(fgets(command, sizeof(command), stdin) != NULL &&
             strcmp(command, "check\n") == 0);
  TEST_CHECK(test_take(&owner, 0U) == COAKKA_HTTP_HOST_TIMEOUT);
  (void)puts("STATIC");
  (void)fflush(stdout);
  const unsigned file_count = controlled ? 1U : 4U;
  coakka_http_host_exchange_t held = {0};
  for (unsigned index = 0U; index < file_count; ++index) {
    coakka_http_host_exchange_t exchange;
    TEST_CHECK(fgets(command, sizeof(command), stdin) != NULL &&
               strcmp(command, "file\n") == 0);
    TEST_CHECK(take_file(&owner, &exchange));
    if (sibling) {
      (void)puts("ADMITTED");
      (void)fflush(stdout);
      TEST_CHECK(take_file(&owner, &held));
    }
    TEST_CHECK(reply_file(&owner, exchange));
    (void)puts("SENT");
    (void)fflush(stdout);
  }
  TEST_CHECK(fgets(command, sizeof(command), stdin) != NULL &&
             strcmp(command, "stop\n") == 0);
  TEST_CHECK(test_now_ms(&drain_start));
  TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) ==
             COAKKA_HTTP_HOST_OK);
  if (controlled) {
    (void)puts("DRAINING");
    (void)fflush(stdout);
  }
  if (sibling) {
    TEST_CHECK(coakka_http_host_service_wait_drained(owner.service, 650U,
                                                    &drained) ==
                   COAKKA_HTTP_HOST_TIMEOUT &&
               drained == COAKKA_HTTP_HOST_DRAIN_PENDING);
    (void)puts("PENDING");
    (void)fflush(stdout);
    TEST_CHECK(reply_file(&owner, held));
    (void)puts("SIBLING");
    (void)fflush(stdout);
  }
  /* Some HTTP/3 clients exit after reading the file, with the final
   * transport ACK still outstanding. The Core outcome must be terminal. */
  const coakka_http_host_status_t drain_status =
      coakka_http_host_service_wait_drained(owner.service, 5000U, &drained);
  TEST_CHECK(test_now_ms(&drain_end));
  const int complete = drain_status == COAKKA_HTTP_HOST_OK &&
                       drained == COAKKA_HTTP_HOST_DRAIN_COMPLETE;
  const int expired = drain_status == COAKKA_HTTP_HOST_TIMEOUT &&
                      drained == COAKKA_HTTP_HOST_DRAIN_EXPIRED;
  const char *outcome = complete ? "COMPLETE" : (expired ? "EXPIRED" : "UNEXPECTED");
  (void)printf("DRAIN %s %llu ms\n", outcome,
               (unsigned long long)(drain_end - drain_start));
  (void)fflush(stdout);
  TEST_CHECK(complete ||
             (config.protocol == COAKKA_HTTP_HOST_PROTOCOL_HTTP_3 && expired));
  const coakka_http_host_status_t next = test_take(&owner, 0U);
  TEST_CHECK(next == COAKKA_HTTP_HOST_TIMEOUT || next == COAKKA_HTTP_HOST_CLOSED);
  TEST_CHECK(coakka_http_host_service_stop(owner.service) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(test_service_close(&owner));
  if (config.protocol == COAKKA_HTTP_HOST_PROTOCOL_HTTP_3) {
    config.port = port;
    TEST_CHECK(test_service_open(&owner, &config, routes, 2U));
    (void)printf("REBOUND %u\n", (unsigned)port);
    (void)fflush(stdout);
  }
cleanup:
  return test_finish("standalone-files", ok, test_service_close(&owner));
}
