#if defined(__linux__)
#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <signal.h>
#endif

#include "test_support.h"

#include <stdlib.h>
#include <string.h>

#if defined(__linux__)
static int app_signal_is_default(void) {
  struct sigaction disposition;
  sigset_t blocked;
  return sigaction(SIGPIPE, NULL, &disposition) == 0 &&
         disposition.sa_handler == SIG_DFL &&
         pthread_sigmask(SIG_SETMASK, NULL, &blocked) == 0 &&
         sigismember(&blocked, SIGPIPE) == 0;
}
#endif

int main(int argc, char **argv) {
  if (argc != 6)
    return EXIT_FAILURE;
  char *end = NULL;
  const unsigned long port_arg = strtoul(argv[5], &end, 10);
  if (*end != '\0' || port_arg == 0UL || port_arg > UINT16_MAX ||
      !test_platform_open())
    return EXIT_FAILURE;
  int ok = 1;
  test_service_t owner;
  test_service_init(&owner);
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text("127.0.0.1");
  config.port = (uint16_t)port_arg;
  if (strcmp(argv[1], "tls") == 0) {
    config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
    config.certificate_chain_file = test_text(argv[2]);
    config.private_key_file = test_text(argv[3]);
    config.credential_id = test_text("file-gate");
    config.credential_generation = 1U;
  } else {
    TEST_CHECK(strcmp(argv[1], "plain") == 0);
  }
  coakka_http_host_static_mount_t mount;
  coakka_http_host_static_mount_init(&mount);
  mount.url_prefix = test_text("/assets");
  mount.root_path = test_text(argv[4]);
  config.static_mounts = &mount;
  config.static_mount_count = 1U;
#if defined(__linux__)
  TEST_CHECK(app_signal_is_default());
#endif
  TEST_CHECK(coakka_http_host_abi_version() == 10U);
  TEST_CHECK(test_service_open(&owner, &config, NULL, 0U));
#if defined(__linux__)
  TEST_CHECK(app_signal_is_default());
#endif
  uint16_t port = 0U;
  TEST_CHECK(coakka_http_host_service_port(owner.service, &port) ==
             COAKKA_HTTP_HOST_OK);
  printf("PORT=%u\n", (unsigned)port);
  fflush(stdout);
  TEST_CHECK(fgetc(stdin) == '\n');
  TEST_CHECK(test_take(&owner, 0U) == COAKKA_HTTP_HOST_TIMEOUT);
  TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) ==
             COAKKA_HTTP_HOST_OK);
  coakka_http_host_drain_state_t drain = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
  TEST_CHECK(coakka_http_host_service_wait_drained(
                 owner.service, 4000U, &drain) == COAKKA_HTTP_HOST_OK &&
             drain == COAKKA_HTTP_HOST_DRAIN_COMPLETE);
  TEST_CHECK(test_service_close(&owner));
  config.port = port;
  TEST_CHECK(test_service_open(&owner, &config, NULL, 0U));
#if defined(__linux__)
  TEST_CHECK(app_signal_is_default());
#endif
cleanup:
  return test_finish("static_file_cancel_recover", ok,
                     test_service_close(&owner));
}
