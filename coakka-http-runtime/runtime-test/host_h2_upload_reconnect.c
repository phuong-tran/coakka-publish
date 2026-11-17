#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#if defined(__cplusplus)
extern "C" {
#endif
#include "test_support.h"
#if defined(__cplusplus)
}
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { upload_size = 140, interrupted_prefix = 32, exchange_count = 3 };

typedef struct observed_exchange {
  coakka_http_host_exchange_t token;
  unsigned terminals;
} observed_exchange_t;

static void marker(const char *message) {
  puts(message);
  fflush(stdout);
}

static void pause_after_empty_poll(void) {
#if defined(_WIN32)
  Sleep(1);
#else
  const struct timespec pause = {0, 1000000L};
  (void)nanosleep(&pause, NULL);
#endif
}

static int matches(coakka_http_host_exchange_t left,
                   coakka_http_host_exchange_t right) {
  return left.slot == right.slot && left.generation == right.generation;
}

static int exercise(test_service_t *owner, int timed) {
  int ok = 1;
  observed_exchange_t seen[exchange_count];
  memset(seen, 0, sizeof(seen));
  uint8_t body[upload_size];
  size_t body_size = 0U;
  unsigned admitted = 0U;
  unsigned terminals = 0U;
  unsigned end_events = 0U;
  unsigned cancelled_events = 0U;
  uint64_t deadline = 0U;
  uint64_t wait = 0U;
  TEST_CHECK(test_now_ms(&deadline));
  deadline += TEST_IO_TIMEOUT_MS;

  while (terminals < exchange_count || cancelled_events == 0U) {
    uint64_t now = 0U;
    TEST_CHECK(test_now_ms(&now) && now < deadline);
    const coakka_http_host_status_t status = test_take(owner, wait);
    if (status == COAKKA_HTTP_HOST_TIMEOUT) {
      if (timed) {
        wait = 1U;
      } else {
        pause_after_empty_poll();
      }
      continue;
    }
    wait = 0U;
    TEST_CHECK(status == COAKKA_HTTP_HOST_OK);

    const coakka_http_host_exchange_t token = owner->event.exchange;
    const coakka_http_host_request_event_kind_t kind = owner->event.kind;
    fprintf(stderr, "EVENT kind=%u slot=%llu generation=%llu len=%llu\n",
            (unsigned)kind, (unsigned long long)token.slot,
            (unsigned long long)token.generation,
            (unsigned long long)owner->event.data.size);
    if (kind == COAKKA_HTTP_HOST_REQUEST) {
      coakka_http_host_request_view_t view;
      coakka_http_host_request_view_init(&view);
      TEST_CHECK(admitted < exchange_count &&
                 coakka_http_host_request_view(&owner->event, &view) == COAKKA_HTTP_HOST_OK &&
                 test_bytes_equal(view.target, admitted == 1U ? "/sibling" : "/upload"));
      seen[admitted++].token = token;
      if (admitted == 2U) {
        TEST_CHECK(test_respond(owner, 200U, "ready") && test_release(owner));
      } else {
        body_size = 0U;
        TEST_CHECK(test_release(owner));
        marker(admitted == 1U ? "FIRST-HEAD" : "FRESH-HEAD");
      }
    } else if (kind == COAKKA_HTTP_HOST_REQUEST_DATA) {
      TEST_CHECK(admitted != 0U && admitted != 2U && matches(seen[admitted - 1U].token, token));
      TEST_CHECK(owner->event.data.size != 0U &&
                 owner->event.data.size <= sizeof(body) - body_size);
      memcpy(body + body_size, owner->event.data.data, (size_t)owner->event.data.size);
      body_size += (size_t)owner->event.data.size;
      TEST_CHECK(test_release(owner));
      if (admitted == 1U) {
        TEST_CHECK(body_size == interrupted_prefix);
        marker("FIRST-DATA-RETURNED");
      }
    } else if (kind == COAKKA_HTTP_HOST_REQUEST_END) {
      TEST_CHECK(admitted == exchange_count && matches(seen[2].token, token) &&
                 body_size == sizeof(body) && ++end_events == 1U && test_release(owner));
      coakka_http_host_response_t response;
      coakka_http_host_response_init(&response);
      response.status_code = 200U;
      response.body.data = body;
      response.body.size = body_size;
      TEST_CHECK(coakka_http_host_respond(owner->service, token, &response) == COAKKA_HTTP_HOST_OK);
    } else if (kind == COAKKA_HTTP_HOST_REQUEST_CANCELLED) {
      TEST_CHECK(matches(seen[0].token, token) && ++cancelled_events == 1U && test_release(owner));
    } else {
      TEST_CHECK(kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL);
      unsigned index = 0U;
      while (index < admitted && !matches(seen[index].token, token))
        ++index;
      TEST_CHECK(index < admitted && ++seen[index].terminals == 1U && test_release(owner));
      ++terminals;
      if (index == 0U) {
        coakka_http_host_response_t stale;
        coakka_http_host_response_init(&stale);
        stale.status_code = 200U;
        TEST_CHECK(coakka_http_host_respond(owner->service, token, &stale) ==
                   COAKKA_HTTP_HOST_STALE_EXCHANGE);
        marker("FIRST-TERMINAL");
      }
    }
  }
  TEST_CHECK(end_events == 1U && test_take(owner, 0U) == COAKKA_HTTP_HOST_TIMEOUT);
cleanup:
  return ok;
}

int main(int argc, char **argv) {
  if (argc != 5 || coakka_http_host_abi_version() != 10U ||
      (strcmp(argv[1], "poll") != 0 && strcmp(argv[1], "timed") != 0) ||
      (strcmp(argv[4], "default") != 0 && strcmp(argv[4], "uring") != 0) ||
      !test_platform_open())
    return EXIT_FAILURE;

  const int timed = strcmp(argv[1], "timed") == 0;
  const int uring = strcmp(argv[4], "uring") == 0;
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_2;
  config.bind_address = test_text("127.0.0.1");
  config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
  config.credential_generation = 1U;
  config.credential_id = test_text("h2-upload-reconnect");
  config.certificate_chain_file = test_text(argv[2]);
  config.private_key_file = test_text(argv[3]);
  config.use_io_uring = (uint8_t)uring;
  coakka_http_host_route_t routes[2];
  for (unsigned index = 0; index < 2U; ++index) {
    coakka_http_host_route_init(&routes[index]);
    routes[index].route_id = routes[index].handler_binding_id = index + 1U;
  }
  routes[0].method = test_text("GET");
  routes[0].encoded_path_pattern = test_text("/sibling");
  routes[1].method = test_text("POST");
  routes[1].encoded_path_pattern = test_text("/upload");
  routes[1].flags = COAKKA_HTTP_HOST_ROUTE_STREAMING_REQUEST;

  int ok = 1;
  test_service_t owner;
  test_service_init(&owner);
  coakka_http_host_service_info_t info;
  coakka_http_host_service_info_init(&info);
  uint16_t port = 0U;
  coakka_http_host_drain_state_t drain = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
  TEST_CHECK(test_service_open(&owner, &config, routes, 2U));
  TEST_CHECK(coakka_http_host_service_info(owner.service, &info) == COAKKA_HTTP_HOST_OK &&
             info.abi_version == 10U && info.io_uring_effective == (uint8_t)uring);
  TEST_CHECK(coakka_http_host_service_port(owner.service, &port) == COAKKA_HTTP_HOST_OK && port != 0U);
  printf("READY %u\n", (unsigned)port);
  fflush(stdout);
  TEST_CHECK(exercise(&owner, timed));
  TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) == COAKKA_HTTP_HOST_OK &&
             coakka_http_host_service_wait_drained(owner.service, 3000U, &drain) == COAKKA_HTTP_HOST_OK &&
             drain == COAKKA_HTTP_HOST_DRAIN_COMPLETE &&
             coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK &&
             test_service_close(&owner));
  config.port = port;
  TEST_CHECK(test_service_open(&owner, &config, routes, 2U) && test_service_close(&owner));
  marker("CLOSED-AND-REOPENED");
cleanup:
  if (!test_service_close(&owner) || !test_platform_close())
    ok = 0;
  return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
