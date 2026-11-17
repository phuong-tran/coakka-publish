#include "test_support.h"
#include "test_threads.h"

#include <inttypes.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#endif

enum { probe_budget_ms = 2000, stop_race_rounds = 16 };

static int observe(coakka_http_host_service_t *service, const char *phase,
                   uint8_t expected_open, uint64_t *last_sequence) {
  coakka_http_host_health_t health;
  coakka_http_host_health_init(&health);
  const coakka_http_host_status_t result =
      coakka_http_host_probe_liveness(service, probe_budget_ms, &health);
  printf("%s %s sequence=%" PRIu64 " admission=%u active=%u\n", phase,
         coakka_http_host_status_name(result), health.acknowledged_probe_sequence,
         (unsigned)health.admission_open, (unsigned)health.activity.active_requests);
  fflush(stdout);
  if (result != COAKKA_HTTP_HOST_OK ||
      health.acknowledged_probe_sequence <= *last_sequence ||
      health.admission_open != expected_open || health.ready != expected_open ||
      (expected_open == 0U &&
       (health.lifecycle != COAKKA_HTTP_HOST_LIFECYCLE_DRAINING ||
        health.activity.active_requests == 0U))) {
    return 0;
  }
  *last_sequence = health.acknowledged_probe_sequence;
  return 1;
}

typedef struct stop_call {
  coakka_http_host_service_t *service;
  int should_stop;
} stop_call_t;

static int stop_or_probe(void *context) {
  const stop_call_t *call = (const stop_call_t *)context;
  if (call->should_stop) {
    return coakka_http_host_service_stop(call->service) == COAKKA_HTTP_HOST_OK ? 0 : -1;
  }
  coakka_http_host_health_t health;
  coakka_http_host_health_init(&health);
  const coakka_http_host_status_t status =
      coakka_http_host_probe_liveness(call->service, probe_budget_ms, &health);
  /* A racing probe may be acknowledged before stop wins, or closed by stop.
   * A probe entering after composition shutdown gets INVALID_ARGUMENT from
   * the public projection of its invalid lifecycle state. */
  if (status == COAKKA_HTTP_HOST_CLOSED ||
      status == COAKKA_HTTP_HOST_INVALID_ARGUMENT ||
      (status == COAKKA_HTTP_HOST_OK && health.acknowledged_probe_sequence != 0U)) {
    return 0;
  }
  fprintf(stderr, "stop race probe: %s\n", coakka_http_host_status_name(status));
  return -1;
}

static int stopped(coakka_http_host_service_t *service) {
  coakka_http_host_health_t health;
  coakka_http_host_health_init(&health);
  const coakka_http_host_status_t status =
      coakka_http_host_probe_liveness(service, probe_budget_ms, &health);
  if (status != COAKKA_HTTP_HOST_INVALID_ARGUMENT) {
    fprintf(stderr, "post-stop probe: %s\n", coakka_http_host_status_name(status));
  }
  return status == COAKKA_HTTP_HOST_INVALID_ARGUMENT &&
         health.lifecycle == COAKKA_HTTP_HOST_LIFECYCLE_STOPPED &&
         health.ready == 0U && health.admission_open == 0U;
}

int main(int argc, char **argv) {
  if (argc != 6 || !test_platform_open()) {
    return 2;
  }
  int ok = 1;
  test_service_t owner;
  test_service_init(&owner);
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text("127.0.0.1");
  config.limits.drain_timeout_ms = 8000U;
  config.limits.handler_timeout_ms = 12000U;
  if (strcmp(argv[1], "h2") == 0) {
    config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_2;
  } else if (strcmp(argv[1], "h3") == 0) {
    config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_3;
  } else {
    TEST_CHECK(strcmp(argv[1], "h1") == 0 || strcmp(argv[1], "h1-tls") == 0);
    config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_1_1;
  }
  if (strcmp(argv[1], "h1") != 0) {
    config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
    config.credential_generation = 1U;
    config.credential_id = test_text("owner-liveness");
    config.certificate_chain_file = test_text(argv[4]);
    config.private_key_file = test_text(argv[5]);
  }
  if (strcmp(argv[2], "uring") == 0) {
    config.use_io_uring = 1U;
  } else {
    TEST_CHECK(strcmp(argv[2], "default") == 0);
  }
  if (strcmp(argv[3], "monitor") == 0) {
    config.monitor.initial_collection = COAKKA_HTTP_HOST_MONITOR_AGGREGATES;
    config.monitor.categories = COAKKA_HTTP_HOST_MONITOR_EXCHANGE;
  } else {
    TEST_CHECK(strcmp(argv[3], "disabled") == 0);
  }

  coakka_http_host_route_t route;
  coakka_http_host_route_init(&route);
  route.route_id = route.handler_binding_id = 1U;
  route.method = test_text("GET");
  route.encoded_path_pattern = test_text("/held");
  TEST_CHECK(coakka_http_host_abi_version() == 10U);
#if defined(_WIN32)
  wchar_t resolved[MAX_PATH];
  wchar_t expected[MAX_PATH];
  HMODULE module = GetModuleHandleW(L"coakka_http_host.dll");
  TEST_CHECK(module != NULL);
  const DWORD loaded = GetModuleFileNameW(module, resolved, MAX_PATH);
  const DWORD requested = GetEnvironmentVariableW(L"COAKKA_EXPECT_LOADED_DLL",
                                                   expected, MAX_PATH);
  TEST_CHECK(loaded != 0U && loaded < MAX_PATH);
  if (requested != 0U) {
    TEST_CHECK(requested < MAX_PATH && _wcsicmp(resolved, expected) == 0);
  }
  fprintf(stderr, "Loaded module: %ls ABI=10\n", resolved);
#endif

  TEST_CHECK(test_service_open(&owner, &config, &route, 1U));
  coakka_http_host_service_info_t info;
  coakka_http_host_service_info_init(&info);
  TEST_CHECK(coakka_http_host_service_info(owner.service, &info) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(info.abi_version == 10U && info.io_uring_effective == config.use_io_uring);
  TEST_CHECK(coakka_http_host_monitor_enabled(owner.service) ==
             (config.monitor.initial_collection != COAKKA_HTTP_HOST_MONITOR_DISABLED));
  uint16_t port = 0U;
  TEST_CHECK(coakka_http_host_service_port(owner.service, &port) == COAKKA_HTTP_HOST_OK);
  uint64_t last_sequence = 0U;
  TEST_CHECK(observe(owner.service, "IDLE", 1U, &last_sequence));
  TEST_CHECK(observe(owner.service, "IDLE", 1U, &last_sequence));
  printf("READY %" PRIu16 " ABI=%u io_uring=%u\n", port,
         (unsigned)info.abi_version, (unsigned)info.io_uring_effective);
  fflush(stdout);
  TEST_CHECK(test_next_request(&owner));
  TEST_CHECK(test_bytes_equal(coakka_http_host_request_target(&owner.event), "/held"));
  TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) == COAKKA_HTTP_HOST_OK);
  coakka_http_host_drain_state_t drain = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
  TEST_CHECK(coakka_http_host_service_wait_drained(owner.service, 0U, &drain) ==
             COAKKA_HTTP_HOST_TIMEOUT && drain == COAKKA_HTTP_HOST_DRAIN_PENDING);
  TEST_CHECK(observe(owner.service, "DRAIN", 0U, &last_sequence));
  TEST_CHECK(test_respond(&owner, 200U, "held-response"));
  TEST_CHECK(test_release(&owner));
  TEST_CHECK(test_take(&owner, 5000U) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(owner.event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL);
  TEST_CHECK(test_release(&owner));
  TEST_CHECK(coakka_http_host_service_wait_drained(owner.service, 10000U, &drain) ==
             COAKKA_HTTP_HOST_OK && drain == COAKKA_HTTP_HOST_DRAIN_COMPLETE);
  TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(stopped(owner.service));
  TEST_CHECK(test_service_close(&owner));

  if (config.protocol == COAKKA_HTTP_HOST_PROTOCOL_HTTP_1_1) {
    /* The control may actively close its TCP connection during drain and
     * reserve that local address in TIME_WAIT. Test stop/rebind on a fresh
     * connection-free listener for this unchanged HTTP/1.1 profile. */
    TEST_CHECK(test_service_open(&owner, &config, &route, 1U));
    TEST_CHECK(coakka_http_host_service_port(owner.service, &port) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(stopped(owner.service));
    TEST_CHECK(test_service_close(&owner));
  }
  config.port = port;
  for (unsigned round = 0U; round < stop_race_rounds; ++round) {
    TEST_CHECK(test_service_open(&owner, &config, &route, 1U));
    uint16_t rebound = 0U;
    TEST_CHECK(coakka_http_host_service_port(owner.service, &rebound) ==
               COAKKA_HTTP_HOST_OK && rebound == port);
    stop_call_t probe = {owner.service, 0};
    stop_call_t stop = {owner.service, 1};
    void *workers[] = {&probe, &stop};
    TEST_CHECK(coakka_http_test_run_threads(stop_or_probe, workers, 2U) == 0);
    TEST_CHECK(stopped(owner.service));
    TEST_CHECK(test_service_close(&owner));
  }
  printf("PASS %s %s %s closed/rebound=%u\n", argv[1], argv[2], argv[3],
         stop_race_rounds);
cleanup:
  return test_finish("host_liveness_lifecycle", ok, test_service_close(&owner));
}
