#if defined(__cplusplus)
extern "C" {
#endif
#include "test_support.h"
#if defined(__cplusplus)
}
#endif

#include <stdlib.h>
#include <string.h>

enum { REQUEST_COUNT = 4 };

typedef struct admission_request {
  coakka_http_host_exchange_t exchange;
  coakka_http_host_request_event_t held;
  int admitted;
  int lease_live;
  int terminal;
} admission_request_t;

static int same_exchange(coakka_http_host_exchange_t first,
                         coakka_http_host_exchange_t second) {
  return first.slot == second.slot && first.generation == second.generation;
}

static int collect(test_service_t *owner, admission_request_t *requests,
                   unsigned expected, int retain, unsigned *observed) {
  if (test_take(owner, 3000U) != COAKKA_HTTP_HOST_OK) return 0;
  if (owner->event.kind == COAKKA_HTTP_HOST_REQUEST) {
    const uint64_t route = coakka_http_host_request_route_id(&owner->event);
    if (route == 0U || route > REQUEST_COUNT ||
        (observed == NULL && route != expected) ||
        requests[route - 1U].admitted) return 0;
    admission_request_t *request = &requests[route - 1U];
    request->exchange = owner->event.exchange;
    request->admitted = 1;
    if (observed != NULL) *observed = (unsigned)route;
    if (retain) {
      request->held = owner->event;
      request->lease_live = 1;
      owner->lease_live = 0;
      return 1;
    }
  } else if (owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL) {
    int found = 0;
    for (unsigned index = 0; index < REQUEST_COUNT; ++index) {
      admission_request_t *request = &requests[index];
      if (!request->admitted ||
          !same_exchange(request->exchange, owner->event.exchange)) continue;
      if (request->terminal) return 0;
      request->terminal = 1;
      found = 1;
      break;
    }
    if (!found) return 0;
  } else {
    return 0;
  }
  return test_release(owner);
}

static int respond(test_service_t *owner, coakka_http_host_exchange_t exchange,
                   const char *body) {
  coakka_http_host_response_t response;
  coakka_http_host_response_init(&response);
  response.status_code = 200U;
  response.body = test_text(body);
  return coakka_http_host_respond(owner->service, exchange, &response) ==
         COAKKA_HTTP_HOST_OK;
}

int main(int argc, char **argv) {
  if (argc != 7 || coakka_http_host_abi_version() != 10U || !test_platform_open())
    return EXIT_FAILURE;
  const unsigned long port = strtoul(argv[1], NULL, 10);
  const unsigned long active = strtoul(argv[2], NULL, 10);
  if (port == 0UL || port > 65535UL || (active != 1UL && active != 2UL))
    return EXIT_FAILURE;
  const int tls = strcmp(argv[3], "tls") == 0;
  const int uring = strcmp(argv[6], "uring") == 0;
  const char *const paths[REQUEST_COUNT] = {"/a", "/b", "/c", "/filler"};
  const char *const bodies[REQUEST_COUNT] = {"first", "second", "fresh", "filler"};
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text("127.0.0.1");
  config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_2;
  config.port = (uint16_t)port;
  config.use_io_uring = (uint8_t)uring;
  config.security = tls ? COAKKA_HTTP_HOST_SECURITY_TLS : COAKKA_HTTP_HOST_SECURITY_PLAINTEXT;
  if (tls) {
    config.credential_id = test_text("h2-admission-reclaim");
    config.credential_generation = 1U;
    config.certificate_chain_file = test_text(argv[4]);
    config.private_key_file = test_text(argv[5]);
  }
  config.limits.max_connections = 8U;
  config.limits.max_active_requests = (uint32_t)active;
  config.limits.request_queue_capacity = 1U;
  config.limits.completion_queue_capacity = 1U;
  config.limits.handler_timeout_ms = 5000U;
  config.limits.drain_timeout_ms = 900U;
  config.limits.stop_timeout_ms = 1000U;
  coakka_http_host_route_t routes[REQUEST_COUNT];
  for (unsigned index = 0; index < REQUEST_COUNT; ++index) {
    coakka_http_host_route_init(&routes[index]);
    routes[index].route_id = routes[index].handler_binding_id = index + 1U;
    routes[index].method = test_text("GET");
    routes[index].encoded_path_pattern = test_text(paths[index]);
  }
  admission_request_t requests[REQUEST_COUNT];
  memset(requests, 0, sizeof(requests));
  test_service_t owner;
  test_service_init(&owner);
  int ok = 1;
  int stopped = 0;
  char command[40];
  coakka_http_host_service_info_t info;
  coakka_http_host_service_info_init(&info);
  coakka_http_host_limits_t limits;
  coakka_http_host_limits_init(&limits);
  if (!tls) {
    uint32_t failed = COAKKA_HTTP_HOST_FAILED_INDEX_NONE;
    coakka_http_host_status_t status =
        coakka_http_host_service_create(&config, routes, REQUEST_COUNT, &owner.service, &failed);
    if (status == COAKKA_HTTP_HOST_OK) {
      coakka_http_host_issue_t issue;
      coakka_http_host_issue_init(&issue);
      status = coakka_http_host_service_start(owner.service, &issue);
    }
    TEST_CHECK(status == COAKKA_HTTP_HOST_INVALID_ARGUMENT);
    TEST_CHECK(test_service_close(&owner));
    puts("PLAINTEXT REFUSED");
    goto cleanup;
  }
  TEST_CHECK(test_service_open(&owner, &config, routes, REQUEST_COUNT));
  TEST_CHECK(coakka_http_host_service_info(owner.service, &info) == COAKKA_HTTP_HOST_OK &&
             info.abi_version == 10U && info.io_uring_effective == (uint8_t)uring);
  TEST_CHECK(coakka_http_host_service_limits(owner.service, &limits) == COAKKA_HTTP_HOST_OK &&
             limits.max_active_requests == (uint32_t)active &&
             limits.request_queue_capacity == 1U && limits.completion_queue_capacity == 1U);
  printf("READY %u backend=%s\n", (unsigned)config.port, uring ? "uring" : "default");
  (void)fflush(stdout);

  while (fgets(command, (int)sizeof(command), stdin) != NULL) {
    if (strcmp(command, "quiet\n") == 0) {
      TEST_CHECK(test_take(&owner, 120U) == COAKKA_HTTP_HOST_TIMEOUT);
    } else if (strcmp(command, "next\n") == 0) {
      unsigned observed = 0U;
      for (unsigned attempt = 0; observed == 0U && attempt < REQUEST_COUNT; ++attempt)
        TEST_CHECK(collect(&owner, requests, 0U, 0, &observed));
      TEST_CHECK(observed == 2U || observed == 3U);
      printf("ADMITTED %u\n", observed);
      (void)fflush(stdout);
      continue;
    } else if (strcmp(command, "begin\n") == 0) {
      TEST_CHECK(coakka_http_host_service_begin_drain(owner.service) == COAKKA_HTTP_HOST_OK);
    } else if (strcmp(command, "drained\n") == 0) {
      coakka_http_host_drain_state_t state = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
      TEST_CHECK(coakka_http_host_service_wait_drained(owner.service, 2500U, &state) ==
                 COAKKA_HTTP_HOST_OK && state == COAKKA_HTTP_HOST_DRAIN_COMPLETE);
    } else if (strcmp(command, "stop\n") == 0) {
      TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);
      TEST_CHECK(test_service_close(&owner));
      TEST_CHECK(test_service_open(&owner, &config, routes, REQUEST_COUNT));
      TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);
      TEST_CHECK(test_service_close(&owner));
      stopped = 1;
      puts("STOPPED AND REOPENED");
      (void)fflush(stdout);
      break;
    } else {
      const char *argument = strchr(command, ' ');
      TEST_CHECK(argument != NULL && argument[1] >= '1' && argument[1] <= '4');
      const unsigned id = (unsigned)(argument[1] - '0');
      admission_request_t *request = &requests[id - 1U];
      if (strncmp(command, "take ", 5U) == 0 ||
          strncmp(command, "retain ", 7U) == 0) {
        const int retain = command[0] == 'r';
        for (unsigned attempt = 0; !request->admitted && attempt < REQUEST_COUNT; ++attempt)
          TEST_CHECK(collect(&owner, requests, id, retain, NULL));
        TEST_CHECK(request->admitted && request->lease_live == retain);
      } else if (strncmp(command, "terminal ", 9U) == 0) {
        TEST_CHECK(request->admitted);
        for (unsigned attempt = 0; !request->terminal && attempt < REQUEST_COUNT; ++attempt)
          TEST_CHECK(collect(&owner, requests, 0U, 0, NULL));
        TEST_CHECK(request->terminal);
      } else if (strncmp(command, "answer ", 7U) == 0) {
        TEST_CHECK(request->admitted && !request->terminal &&
                   respond(&owner, request->exchange, bodies[id - 1U]));
      } else if (strncmp(command, "return ", 7U) == 0) {
        TEST_CHECK(request->lease_live &&
                   coakka_http_host_request_route_id(&request->held) == id);
        TEST_CHECK(coakka_http_host_release_request(owner.service, &request->held) ==
                   COAKKA_HTTP_HOST_OK);
        request->lease_live = 0;
      } else {
        TEST_CHECK(0);
      }
    }
    puts("OK");
    (void)fflush(stdout);
  }
  TEST_CHECK(stopped);

cleanup:
  for (unsigned index = 0; index < REQUEST_COUNT; ++index) {
    if (requests[index].lease_live &&
        coakka_http_host_release_request(owner.service, &requests[index].held) !=
            COAKKA_HTTP_HOST_OK) ok = 0;
  }
  return test_finish("h2-admission-reclaim", ok, test_service_close(&owner));
}
