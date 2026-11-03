/* Public host-inlined C fixture used in the release confirmation. */
#include <coakka/http/host.h>

#include <inttypes.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static volatile sig_atomic_t stopping = 0;
static const char body[] = "0123456789abcdef0123456789abcdef";

typedef struct probe_counts {
  uint64_t requests;
  uint64_t terminals;
  uint64_t other_events;
  uint64_t late_responses;
} probe_counts_t;

static coakka_http_host_bytes_t bytes(const char *value) {
  coakka_http_host_bytes_t result = {(const uint8_t *)value, strlen(value)};
  return result;
}

static void stop_signal(int signal_number) {
  (void)signal_number;
  stopping = 1;
}

/* The fixture CLI accepts numeric caps from the recorded campaign. The SDK
 * expects the corresponding named notification profiles. */
static int valid_explicit_profile(unsigned long value) {
  switch (value) {
  case COAKKA_HTTP_HOST_NOTIFICATION_SMALL:
  case COAKKA_HTTP_HOST_NOTIFICATION_MEDIUM:
  case COAKKA_HTTP_HOST_NOTIFICATION_LARGE:
  case COAKKA_HTTP_HOST_NOTIFICATION_XLARGE:
  case COAKKA_HTTP_HOST_NOTIFICATION_XXLARGE:
  case COAKKA_HTTP_HOST_NOTIFICATION_ULTRA:
    return 1;
  default:
    return 0;
  }
}

static int serve(coakka_http_host_service_t *service, probe_counts_t *counts) {
  static const coakka_http_host_header_t content_type = {
      {(const uint8_t *)"content-type", 12U},
      {(const uint8_t *)"application/octet-stream", 24U}};
  while (stopping == 0) {
    coakka_http_host_request_event_t event;
    coakka_http_host_status_t status;
    coakka_http_host_request_event_init(&event);
    status = coakka_http_host_take_request(service, 100U, &event);
    if (status == COAKKA_HTTP_HOST_TIMEOUT) {
      continue;
    }
    if (status == COAKKA_HTTP_HOST_CLOSED) {
      break;
    }
    if (status != COAKKA_HTTP_HOST_OK) {
      (void)fprintf(stderr, "probe-failure take=%d\n", status);
      return 0;
    }
    /* A successful take gives us a lease. Release it after handling the event. */
    if (event.kind == COAKKA_HTTP_HOST_REQUEST) {
      ++counts->requests;
      coakka_http_host_response_t response;
      coakka_http_host_response_init(&response);
      response.headers = &content_type;
      response.header_count = 1U;
      response.body = bytes(body);
      const coakka_http_host_status_t submitted =
          coakka_http_host_respond(service, event.exchange, &response);
      /* A duration-limited client may close after checkout but before this
       * response. Core owns that race and returns STALE_EXCHANGE; release the
       * lease and keep serving. Never retry, fabricate a response, or suppress
       * another failure. Measured HTTP success is independently checked. */
      if (submitted == COAKKA_HTTP_HOST_STALE_EXCHANGE) {
        ++counts->late_responses;
      } else if (submitted != COAKKA_HTTP_HOST_OK) {
        (void)fprintf(stderr,
                      "probe-failure respond=%d slot=%" PRIu64 " generation=%" PRIu64
                      "\n",
                      submitted, event.exchange.slot, event.exchange.generation);
        (void)coakka_http_host_release_request(service, &event);
        return 0;
      }
    } else if (event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL) {
      ++counts->terminals;
    } else {
      ++counts->other_events;
    }
    const coakka_http_host_status_t released =
        coakka_http_host_release_request(service, &event);
    if (released != COAKKA_HTTP_HOST_OK) {
      (void)fprintf(stderr, "probe-failure release=%d\n", released);
      return 0;
    }
  }
  return 1;
}

/* Stop closes admission. Drain terminal events already published so the
 * fixture accounts for them before destroying the service. */
static int drain_after_stop(coakka_http_host_service_t *service,
                            probe_counts_t *counts) {
  for (uint32_t index = 0U; index < 1024U; ++index) {
    coakka_http_host_request_event_t event;
    coakka_http_host_request_event_init(&event);
    const coakka_http_host_status_t status =
        coakka_http_host_take_request(service, 0U, &event);
    if (status == COAKKA_HTTP_HOST_CLOSED || status == COAKKA_HTTP_HOST_TIMEOUT) {
      return 1;
    }
    if (status != COAKKA_HTTP_HOST_OK) {
      return 0;
    }
    if (event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL) {
      ++counts->terminals;
    } else {
      ++counts->other_events;
    }
    if (coakka_http_host_release_request(service, &event) != COAKKA_HTTP_HOST_OK) {
      return 0;
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  coakka_http_host_configuration_t configuration;
  coakka_http_host_service_info_t info;
  coakka_http_host_route_t route;
  coakka_http_host_service_t *service = NULL;
  coakka_http_host_issue_t issue;
  uint32_t failed_route = COAKKA_HTTP_HOST_FAILED_INDEX_NONE;
  probe_counts_t counts = {0};
  unsigned long port;
  unsigned long terminal_cap = 0UL;
  unsigned long request_cap = 0UL;
  unsigned long event_loops = 1UL;
  int passed;
  if ((argc < 3 || argc > 6) ||
      (strcmp(argv[2], "full") != 0 && strcmp(argv[2], "compact") != 0)) {
    return EXIT_FAILURE;
  }
  port = strtoul(argv[1], NULL, 10);
  if (port == 0UL || port > 65535UL) {
    return EXIT_FAILURE;
  }
  if (argc >= 4) {
    char *end = NULL;
    terminal_cap = strtoul(argv[3], &end, 10);
    if (end == argv[3] || *end != '\0' || !valid_explicit_profile(terminal_cap)) {
      return EXIT_FAILURE;
    }
  }
  if (argc >= 5) {
    char *end = NULL;
    request_cap = strtoul(argv[4], &end, 10);
    if (end == argv[4] || *end != '\0' || !valid_explicit_profile(request_cap)) {
      return EXIT_FAILURE;
    }
  }

  coakka_http_host_configuration_init(&configuration);
  coakka_http_host_issue_init(&issue);
  configuration.port = (uint16_t)port;
  configuration.bind_address = bytes("127.0.0.1");
  if (argc == 6) {
    char *end = NULL;
    event_loops = strtoul(argv[5], &end, 10);
    if (end == argv[5] || *end != '\0' || event_loops < 1UL || event_loops > 2UL)
      return EXIT_FAILURE;
  }
  const coakka_http_execution_options_t execution = {
      sizeof(execution), COAKKA_HTTP_EXECUTION_EXACT, (uint32_t)event_loops, 0U};
  configuration.execution = event_loops == 0UL ? NULL : &execution;
  configuration.limits.max_connections = 512U;
  configuration.limits.max_active_requests = 256U;
  configuration.limits.request_queue_capacity = 256U;
  configuration.limits.completion_queue_capacity = 256U;
  /* Zero inherits Core's measured default. Optional arguments are exclusively
   * a private tuning probe; no per-request policy lookup occurs. */
  configuration.limits.terminal_notification_profile = (uint32_t)terminal_cap;
  configuration.limits.request_notification_profile = (uint32_t)request_cap;
  /* In the three-argument A/B, only Core's event policy varies. */
  configuration.omit_buffered_success_events =
      strcmp(argv[2], "compact") == 0 ? 1U : 0U;
  coakka_http_host_route_init(&route);
  route.route_id = UINT64_C(1);
  route.handler_binding_id = UINT64_C(1);
  route.method = bytes("GET");
  route.encoded_path_pattern = bytes("/fixed");
  if (coakka_http_host_service_create(&configuration, &route, 1U, &service,
                                      &failed_route) != COAKKA_HTTP_HOST_OK ||
      coakka_http_host_service_start(service, &issue) != COAKKA_HTTP_HOST_OK) {
    coakka_http_host_service_destroy(&service);
    return EXIT_FAILURE;
  }
  (void)memset(&info, 0, sizeof(info));
  info.struct_size = sizeof(info);
  if (coakka_http_host_service_info(service, &info) != COAKKA_HTTP_HOST_OK ||
      !info.execution.observed ||
      info.execution.configured_event_loops != (uint32_t)event_loops ||
      info.execution.active_event_loops != (uint32_t)event_loops ||
      (terminal_cap != 0UL &&
       info.terminal_notification_batch_size != (uint32_t)terminal_cap) ||
      (request_cap != 0UL &&
       info.request_notification_batch_size != (uint32_t)request_cap)) {
    (void)coakka_http_host_service_stop(service);
    coakka_http_host_service_destroy(&service);
    return EXIT_FAILURE;
  }
  (void)fprintf(stderr, "host-batch-config request=%u terminal=%u\n",
                info.request_notification_batch_size,
                info.terminal_notification_batch_size);
  (void)fprintf(stderr, "host-execution configured=%u active=%u\n",
                info.execution.configured_event_loops,
                info.execution.active_event_loops);

  (void)signal(SIGINT, stop_signal);
  (void)signal(SIGTERM, stop_signal);
  passed = serve(service, &counts);
  (void)coakka_http_host_service_begin_drain(service);
  const coakka_http_host_status_t stopped = coakka_http_host_service_stop(service);
  if (stopped != COAKKA_HTTP_HOST_OK) {
    (void)fprintf(stderr, "probe-failure stop=%d\n", stopped);
    passed = 0;
  }
  if (!drain_after_stop(service, &counts)) {
    (void)fprintf(stderr, "probe-failure drain=1\n");
    passed = 0;
  }
  (void)fprintf(stderr,
                "host-event-accounting requests=%" PRIu64 " terminals=%" PRIu64
                " other=%" PRIu64 "\n",
                counts.requests, counts.terminals, counts.other_events);
  (void)fprintf(stderr, "host-late-responses count=%" PRIu64 "\n",
                counts.late_responses);
  if (strcmp(argv[2], "full") == 0 && counts.terminals < counts.requests) {
    passed = 0;
  }
  coakka_http_host_service_destroy(&service);
  return passed != 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
