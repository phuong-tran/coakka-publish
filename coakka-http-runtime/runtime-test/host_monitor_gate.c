#include "test_support.h"

#include <string.h>

static int traffic(test_service_t *owner, uint16_t port, int expires) {
  int ok = 1;
  char response[TEST_WIRE_CAPACITY];
  size_t size = 0U;
  test_socket_t peer = test_connect(port);
  const char request[] = "GET /monitor HTTP/1.1\r\nHost: localhost\r\n"
                         "Connection: close\r\n\r\n";
  TEST_CHECK(peer != TEST_INVALID_SOCKET);
  TEST_CHECK(test_send(peer, request, sizeof(request) - 1U));
  TEST_CHECK(test_next_request(owner));
  if (!expires) {
    TEST_CHECK(test_respond(owner, 200U, "observed"));
  }
  TEST_CHECK(test_release(owner));
  TEST_CHECK(test_receive(peer, response, sizeof(response), &size));
  if (expires) {
    const char prefix[] = "HTTP/1.1 504";
    TEST_CHECK(size >= sizeof(prefix) - 1U);
    for (size_t i = 0; i < sizeof(prefix) - 1U; ++i) {
      TEST_CHECK(response[i] == prefix[i]);
    }
  } else {
    TEST_CHECK(test_wire_response(response, size, 200U, "observed"));
  }
  TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL);
  TEST_CHECK(test_release(owner));
cleanup:
  return test_socket_close(&peer) && test_release(owner) && ok;
}

static int page_bounds(coakka_http_host_service_t *service) {
  int ok = 1;
  const uint32_t limit = COAKKA_HTTP_HOST_MAX_MONITOR_READ;
  coakka_http_host_monitor_event_t events[COAKKA_HTTP_HOST_MAX_MONITOR_READ + 1U];
  for (uint32_t i = 0; i <= limit; ++i)
    coakka_http_host_monitor_event_init(&events[i]);
  coakka_http_host_monitor_page_t page;
  coakka_http_host_monitor_page_init(&page);
  page.count = 1U;
  unsigned char previous_events[sizeof(events)];
  unsigned char previous_page[sizeof(page)];
  memcpy(previous_events, events, sizeof(events));
  memcpy(previous_page, &page, sizeof(page));
  TEST_CHECK(coakka_http_host_monitor_read(service, 0U, limit + 1U, events,
                                           limit + 1U, &page) ==
             COAKKA_HTTP_HOST_INVALID_ARGUMENT);
  TEST_CHECK(memcmp(previous_events, events, sizeof(events)) == 0 &&
             memcmp(previous_page, &page, sizeof(page)) == 0);
  TEST_CHECK(coakka_http_host_monitor_read(service, 0U, limit, events, limit,
                                           &page) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(page.count == 0U && page.remaining_events == 0U);
cleanup:
  return ok;
}

int main(void) {
  int ok = 1;
  if (!test_platform_open())
    return 1;
  test_service_t owner;
  test_service_init(&owner);
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  TEST_CHECK(config.monitor.initial_collection ==
             COAKKA_HTTP_HOST_MONITOR_DISABLED);
  config.monitor.initial_collection =
      COAKKA_HTTP_HOST_MONITOR_AGGREGATES_AND_EVENTS;
  config.monitor.categories = COAKKA_HTTP_HOST_MONITOR_EXCHANGE;
  config.monitor.event_capacity = 4U;
  config.monitor.max_events_per_read = 2U;
  config.limits.handler_timeout_ms = 350U;
  coakka_http_host_route_t route;
  coakka_http_host_route_init(&route);
  route.route_id = route.handler_binding_id = 1U;
  route.method = test_text("GET");
  route.encoded_path_pattern = test_text("/monitor");
  TEST_CHECK(coakka_http_host_abi_version() == 10U);
  coakka_http_host_configuration_t bounds_config = config;
  bounds_config.monitor.event_capacity = COAKKA_HTTP_HOST_MAX_MONITOR_READ;
  bounds_config.monitor.max_events_per_read = COAKKA_HTTP_HOST_MAX_MONITOR_READ;
  TEST_CHECK(test_service_open(&owner, &bounds_config, &route, 1U));
  TEST_CHECK(page_bounds(owner.service));
  TEST_CHECK(test_service_close(&owner));
  TEST_CHECK(test_service_open(&owner, &config, &route, 1U));
  TEST_CHECK(coakka_http_host_monitor_enabled(owner.service) == 1U);
  coakka_http_host_health_t health;
  coakka_http_host_health_init(&health);
  TEST_CHECK(coakka_http_host_probe_liveness(owner.service, 1000U, &health) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(health.ready && health.admission_open &&
             health.lifecycle == COAKKA_HTTP_HOST_LIFECYCLE_RUNNING);
  uint16_t port = 0U;
  TEST_CHECK(coakka_http_host_service_port(owner.service, &port) ==
             COAKKA_HTTP_HOST_OK);
  for (unsigned i = 0; i < 7U; ++i) {
    TEST_CHECK(traffic(&owner, port, 0));
  }
  TEST_CHECK(traffic(&owner, port, 1));
  coakka_http_host_monitor_snapshot_t snapshot;
  coakka_http_host_monitor_snapshot_init(&snapshot);
  TEST_CHECK(coakka_http_host_monitor_snapshot(owner.service, &snapshot) ==
             COAKKA_HTTP_HOST_OK);
  printf(
      "MONITOR admitted=%llu completed=%llu timed_out=%llu overwritten=%llu\n",
      (unsigned long long)snapshot.admitted_requests,
      (unsigned long long)snapshot.exchange_completed,
      (unsigned long long)snapshot.exchange_timed_out,
      (unsigned long long)snapshot.overwritten_events);
  TEST_CHECK(
      snapshot.admitted_requests == 8U && snapshot.exchange_completed == 7U &&
      snapshot.exchange_timed_out == 1U && snapshot.overwritten_events > 0U);
  TEST_CHECK(snapshot.config.max_events_per_read == 2U);
  coakka_http_host_monitor_event_t events[2];
  coakka_http_host_monitor_page_t page;
  coakka_http_host_monitor_page_init(&page);
  for (unsigned i = 0U; i < 2U; ++i)
    coakka_http_host_monitor_event_init(&events[i]);
  TEST_CHECK(coakka_http_host_monitor_read(owner.service, 1U, 2U, events, 2U,
                                           &page) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(page.count == 2U && page.remaining_events == 2U &&
             page.overwritten_events > 0U);
  const uint64_t cursor = events[1].sequence;
  TEST_CHECK(events[0].sequence < cursor);
  TEST_CHECK(coakka_http_host_monitor_read(owner.service, cursor, 2U, events,
                                           2U, &page) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(page.count == 2U && page.remaining_events == 0U &&
             events[0].sequence > cursor &&
             events[1].sequence == page.latest_sequence &&
             events[1].kind == COAKKA_HTTP_HOST_EVENT_EXCHANGE_TIMED_OUT);

  coakka_http_host_monitor_policy_t policy;
  coakka_http_host_monitor_policy_init(&policy);
  policy.collection = COAKKA_HTTP_HOST_MONITOR_DISABLED;
  coakka_http_host_monitor_apply_outcome_t outcome;
  coakka_http_host_monitor_apply_outcome_init(&outcome);
  const uint64_t old_generation = snapshot.config.generation;
  TEST_CHECK(coakka_http_host_monitor_apply(owner.service, old_generation,
                                            &policy,
                                            &outcome) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(
      outcome.reason == COAKKA_HTTP_HOST_MONITOR_APPLIED && outcome.changed &&
      outcome.effective.policy.collection == COAKKA_HTTP_HOST_MONITOR_DISABLED);
  const uint64_t new_generation = outcome.effective.generation;
  TEST_CHECK(new_generation > old_generation &&
             !coakka_http_host_monitor_enabled(owner.service));
  TEST_CHECK(coakka_http_host_monitor_apply(owner.service, old_generation,
                                            &policy,
                                            &outcome) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(outcome.reason == COAKKA_HTTP_HOST_MONITOR_GENERATION_CONFLICT &&
             outcome.effective.generation == new_generation);
  TEST_CHECK(traffic(&owner, port, 0));
  TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(coakka_http_host_service_stop(owner.service) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(coakka_http_host_service_health(owner.service, &health) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(health.lifecycle == COAKKA_HTTP_HOST_LIFECYCLE_STOPPED &&
             !health.admission_open);
cleanup:
  return test_finish("monitor_real_traffic", ok, test_service_close(&owner));
}
