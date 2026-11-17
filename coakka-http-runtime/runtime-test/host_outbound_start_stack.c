#define _POSIX_C_SOURCE 200809L
#include "test_support.h"

#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#endif

typedef struct start_case {
  size_t stack_bytes;
  int outbound;
  int passed;
} start_case_t;

static int start_service(start_case_t *test) {
  if (!test_platform_open())
    return EXIT_FAILURE;
  coakka_http_host_configuration_t configuration;
  coakka_http_host_configuration_init(&configuration);
  configuration.bind_address = test_text("127.0.0.1");

  coakka_http_host_outbound_endpoint_t endpoint;
  coakka_http_host_outbound_endpoint_init(&endpoint);
  endpoint.node_id = test_text("loopback-node");
  endpoint.connect_host = test_text("127.0.0.1");
  endpoint.connect_port = 8080U;
  endpoint.http_authority = test_text("localhost");

  coakka_http_host_outbound_target_t target;
  coakka_http_host_outbound_target_init(&target);
  target.name = test_text("loopback");
  target.generation = 1U;
  target.endpoints = &endpoint;
  target.endpoint_count = 1U;
  if (test->outbound) {
    configuration.outbound_targets = &target;
    configuration.outbound_target_count = 1U;
  }

  // Announce the configuration before entering SDK code. The runner can
  // distinguish a fault during create from a returned startup status.
  printf("ABI %u, app thread stack %zu bytes, outbound %d: create\n",
         (unsigned)coakka_http_host_abi_version(), test->stack_bytes,
         test->outbound);
  fflush(stdout);
  int ok = 1;
  test_service_t owner;
  test_service_init(&owner);
  TEST_CHECK(coakka_http_host_abi_version() == 10U);
  TEST_CHECK(test_service_open(&owner, &configuration, NULL, 0U));
  printf("Create and start passed\n");
cleanup:
  return test_finish("outbound_start_stack", ok, test_service_close(&owner));
}

#if defined(_WIN32)
static DWORD WINAPI run_on_app_thread(LPVOID context) {
  start_case_t *test = context;
  test->passed = start_service(test) == EXIT_SUCCESS;
  return 0;
}

static int run_case(start_case_t *test) {
  HANDLE thread = CreateThread(NULL, test->stack_bytes, run_on_app_thread,
                               test, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
  if (thread == NULL)
    return 0;
  const DWORD wait = WaitForSingleObject(thread, INFINITE);
  return CloseHandle(thread) && wait == WAIT_OBJECT_0;
}
#else
static void *run_on_app_thread(void *context) {
  start_case_t *test = context;
  test->passed = start_service(test) == EXIT_SUCCESS;
  return NULL;
}

static int run_case(start_case_t *test) {
  pthread_attr_t attributes;
  if (pthread_attr_init(&attributes) != 0)
    return 0;
  pthread_t thread;
  int status = pthread_attr_setstacksize(&attributes, test->stack_bytes);
  if (status == 0)
    status = pthread_create(&thread, &attributes, run_on_app_thread, test);
  pthread_attr_destroy(&attributes);
  return status == 0 && pthread_join(thread, NULL) == 0;
}
#endif

int main(int argc, char **argv) {
  if (argc != 2)
    return EXIT_FAILURE;
  start_case_t test = {544U * 1024U, 0, 0};
  if (strcmp(argv[1], "outbound") == 0)
    test.outbound = 1;
  else if (strcmp(argv[1], "outbound-large-stack") == 0) {
    test.outbound = 1;
    test.stack_bytes = 1024U * 1024U;
  } else if (strcmp(argv[1], "plain") != 0)
    return EXIT_FAILURE;
  if (!run_case(&test)) {
    fprintf(stderr, "Could not run the bounded app thread\n");
    return EXIT_FAILURE;
  }
  return test.passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
