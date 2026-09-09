#include <coakka/http/http.h>

#include "test_threads.h"

#include <errno.h>
#include <limits.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET test_socket_t;
#define TEST_INVALID_SOCKET INVALID_SOCKET
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sched.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
typedef int test_socket_t;
#define TEST_INVALID_SOCKET (-1)
#endif

#if defined(MSG_NOSIGNAL)
#define TEST_SEND_FLAGS MSG_NOSIGNAL
#else
#define TEST_SEND_FLAGS 0
#endif

enum {
  TEST_DEFAULT_CLIENTS = 4,
  TEST_DEFAULT_REQUESTS_PER_CLIENT = 32,
  TEST_DEFAULT_SERVER_WORKERS = 4,
  TEST_MAX_CLIENTS = 32,
  TEST_MAX_REQUESTS_PER_CLIENT = 10000,
  TEST_MAX_SERVER_WORKERS = 32,
  TEST_MAX_CYCLES = 1000,
  TEST_MAX_LIFECYCLE_ITERATIONS = 1000,
  TEST_MAX_TIMEOUT_MS = 60000
};

#define TEST_MAX_TOTAL_REQUESTS UINT64_C(1000000)
#define TEST_MAX_TOTAL_LIFECYCLES UINT64_C(10000)

typedef enum test_mode {
  TEST_MODE_SMOKE,
  TEST_MODE_STRESS,
  TEST_MODE_RACE,
  TEST_MODE_SOAK
} test_mode_t;

typedef struct test_config {
  test_mode_t mode;
  uint32_t clients;
  uint32_t requests_per_client;
  uint32_t server_workers;
  uint32_t cycles;
  uint32_t lifecycle_iterations;
  uint32_t timeout_ms;
} test_config_t;

typedef struct test_result {
  uint64_t requests;
  uint64_t stop_attempted;
  uint64_t stop_completed;
  uint64_t stop_closed;
  uint64_t stop_unexpected;
  uint64_t stop_callbacks;
  uint64_t lifecycle_attempted;
  uint64_t lifecycle_completed;
  uint64_t lifecycle_failures;
} test_result_t;

typedef struct server_state {
  atomic_uint calls;
  atomic_uint failures;
} server_state_t;

typedef struct client_state {
  uint16_t port;
  uint32_t requests;
  uint32_t timeout_ms;
  uint32_t failures;
} client_state_t;

typedef struct stop_race_shared {
  coakka_http_server_t *server;
  uint16_t port;
  uint32_t client_count;
  uint32_t timeout_ms;
  uint64_t deadline_ns;
  atomic_int stopping;
  atomic_uint_fast64_t attempted;
  atomic_uint_fast64_t completed;
  atomic_uint_fast64_t closed;
  atomic_uint_fast64_t unexpected;
  coakka_http_result_code_t stop_code;
} stop_race_shared_t;

typedef struct stop_race_context {
  stop_race_shared_t *shared;
  int coordinator;
} stop_race_context_t;

typedef struct lifecycle_shared {
  const test_config_t *config;
  uint64_t deadline_ns;
  atomic_uint_fast64_t attempted;
  atomic_uint_fast64_t completed;
  atomic_uint_fast64_t failures;
} lifecycle_shared_t;

typedef struct lifecycle_context {
  lifecycle_shared_t *shared;
} lifecycle_context_t;

static void usage(const char *program) {
  fprintf(stderr,
          "usage: %s [smoke|stress|race|soak] [--clients 1..32] "
          "[--requests-per-client 1..10000] [--server-workers 1..32] "
          "[--cycles 1..1000] [--lifecycle-iterations 1..1000] "
          "[--timeout-ms 1..60000]\n",
          program);
}

static const char *mode_name(test_mode_t mode) {
  switch (mode) {
  case TEST_MODE_STRESS:
    return "stress";
  case TEST_MODE_RACE:
    return "race";
  case TEST_MODE_SOAK:
    return "soak";
  case TEST_MODE_SMOKE:
  default:
    return "smoke";
  }
}

static uint64_t monotonic_ns(void) {
#if defined(_WIN32)
  LARGE_INTEGER counter;
  LARGE_INTEGER frequency;
  uint64_t seconds;
  uint64_t remainder;
  if (QueryPerformanceCounter(&counter) == 0 ||
      QueryPerformanceFrequency(&frequency) == 0 || frequency.QuadPart <= 0) {
    return 0U;
  }
  seconds = (uint64_t)(counter.QuadPart / frequency.QuadPart);
  remainder = (uint64_t)(counter.QuadPart % frequency.QuadPart);
  return seconds * UINT64_C(1000000000) +
         (remainder * UINT64_C(1000000000)) / (uint64_t)frequency.QuadPart;
#else
  struct timespec timestamp;
  if (clock_gettime(CLOCK_MONOTONIC, &timestamp) != 0) {
    return 0U;
  }
  return (uint64_t)timestamp.tv_sec * UINT64_C(1000000000) +
         (uint64_t)timestamp.tv_nsec;
#endif
}

static int deadline_expired(uint64_t deadline_ns) {
  const uint64_t now = monotonic_ns();
  return now == 0U || now >= deadline_ns;
}

static void thread_yield(void) {
#if defined(_WIN32)
  if (SwitchToThread() == 0) {
    Sleep(0U);
  }
#else
  (void)sched_yield();
#endif
}

static int parse_u32(const char *text, uint32_t minimum, uint32_t maximum,
                     uint32_t *out) {
  char *end = NULL;
  unsigned long parsed;
  if (text == NULL || text[0] == '\0' || out == NULL) {
    return -1;
  }
  errno = 0;
  parsed = strtoul(text, &end, 10);
  if (errno != 0 || end == text || *end != '\0' || parsed > UINT32_MAX ||
      parsed < (unsigned long)minimum || parsed > (unsigned long)maximum) {
    return -1;
  }
  *out = (uint32_t)parsed;
  return 0;
}

static int parse_config(int argc, char **argv, test_config_t *config) {
  int index;
  if (config == NULL) {
    return -1;
  }
  config->mode = TEST_MODE_SMOKE;
  config->clients = TEST_DEFAULT_CLIENTS;
  config->requests_per_client = TEST_DEFAULT_REQUESTS_PER_CLIENT;
  config->server_workers = TEST_DEFAULT_SERVER_WORKERS;
  config->cycles = 1U;
  config->lifecycle_iterations = 4U;
  config->timeout_ms = 15000U;
  index = 1;
  if (index < argc && argv[index][0] != '-') {
    if (strcmp(argv[index], "smoke") == 0) {
      config->mode = TEST_MODE_SMOKE;
    } else if (strcmp(argv[index], "stress") == 0) {
      config->mode = TEST_MODE_STRESS;
      config->clients = 8U;
      config->requests_per_client = 256U;
      config->cycles = 4U;
    } else if (strcmp(argv[index], "race") == 0) {
      config->mode = TEST_MODE_RACE;
      config->requests_per_client = 64U;
      config->lifecycle_iterations = 8U;
    } else if (strcmp(argv[index], "soak") == 0) {
      config->mode = TEST_MODE_SOAK;
      config->clients = 8U;
      config->requests_per_client = 512U;
      config->cycles = 20U;
      config->timeout_ms = 60000U;
    } else {
      return -1;
    }
    ++index;
  }
  for (; index < argc; index += 2) {
    uint32_t maximum;
    uint32_t *target;
    if (index + 1 >= argc) {
      return -1;
    }
    if (strcmp(argv[index], "--clients") == 0) {
      maximum = TEST_MAX_CLIENTS;
      target = &config->clients;
    } else if (strcmp(argv[index], "--requests-per-client") == 0) {
      maximum = TEST_MAX_REQUESTS_PER_CLIENT;
      target = &config->requests_per_client;
    } else if (strcmp(argv[index], "--server-workers") == 0) {
      maximum = TEST_MAX_SERVER_WORKERS;
      target = &config->server_workers;
    } else if (strcmp(argv[index], "--cycles") == 0) {
      maximum = TEST_MAX_CYCLES;
      target = &config->cycles;
    } else if (strcmp(argv[index], "--lifecycle-iterations") == 0) {
      maximum = TEST_MAX_LIFECYCLE_ITERATIONS;
      target = &config->lifecycle_iterations;
    } else if (strcmp(argv[index], "--timeout-ms") == 0) {
      maximum = TEST_MAX_TIMEOUT_MS;
      target = &config->timeout_ms;
    } else {
      return -1;
    }
    if (parse_u32(argv[index + 1], 1U, maximum, target) != 0) {
      return -1;
    }
  }
  if ((uint64_t)config->clients * config->requests_per_client * config->cycles >
      TEST_MAX_TOTAL_REQUESTS) {
    return -1;
  }
  if ((uint64_t)config->clients * config->lifecycle_iterations >
      TEST_MAX_TOTAL_LIFECYCLES) {
    return -1;
  }
  return 0;
}

static coakka_http_bytes_t bytes(const char *value) {
  coakka_http_bytes_t result;
  result.data = (const uint8_t *)value;
  result.size = value == NULL ? 0U : (uint64_t)strlen(value);
  return result;
}

static int bytes_equal(coakka_http_bytes_t value, const char *expected) {
  const size_t size = strlen(expected);
  return value.size == (uint64_t)size &&
         (size == 0U || memcmp(value.data, expected, size) == 0);
}

static void handle_request(void *opaque, coakka_http_request_t *request) {
  server_state_t *state = (server_state_t *)opaque;
  coakka_http_response_t response;
  coakka_http_result_t submitted;

  (void)atomic_fetch_add_explicit(&state->calls, 1U, memory_order_relaxed);
  if (coakka_http_request_route_id(request) != UINT64_C(73) ||
      !bytes_equal(coakka_http_request_method(request), "GET") ||
      !bytes_equal(coakka_http_request_target(request), "/parallel") ||
      coakka_http_request_cancelled(request) != 0U) {
    (void)atomic_fetch_add_explicit(&state->failures, 1U, memory_order_relaxed);
  }
  coakka_http_response_init(&response);
  response.body = bytes("coakka-parallel-response");
  submitted = coakka_http_request_respond(request, &response);
  if (submitted.code != COAKKA_HTTP_RESULT_OK) {
    (void)atomic_fetch_add_explicit(&state->failures, 1U, memory_order_relaxed);
  }
}

static void close_socket(test_socket_t socket_value) {
#if defined(_WIN32)
  (void)closesocket(socket_value);
#else
  (void)close(socket_value);
#endif
}

static test_socket_t connect_loopback(uint16_t port, uint32_t timeout_ms) {
  struct sockaddr_in address;
  test_socket_t socket_value = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#if defined(_WIN32)
  const DWORD socket_timeout_ms = (DWORD)timeout_ms;
#else
  struct timeval timeout;
  timeout.tv_sec = (time_t)(timeout_ms / 1000U);
  timeout.tv_usec = (suseconds_t)((timeout_ms % 1000U) * 1000U);
#endif

  if (socket_value == TEST_INVALID_SOCKET) {
    return TEST_INVALID_SOCKET;
  }
#if defined(SO_NOSIGPIPE)
  {
    const int enabled = 1;
    (void)setsockopt(socket_value, SOL_SOCKET, SO_NOSIGPIPE, &enabled,
                     (socklen_t)sizeof(enabled));
  }
#endif
#if defined(_WIN32)
  (void)setsockopt(socket_value, SOL_SOCKET, SO_RCVTIMEO,
                   (const char *)&socket_timeout_ms,
                   (int)sizeof(socket_timeout_ms));
  (void)setsockopt(socket_value, SOL_SOCKET, SO_SNDTIMEO,
                   (const char *)&socket_timeout_ms,
                   (int)sizeof(socket_timeout_ms));
#else
  (void)setsockopt(socket_value, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                   (socklen_t)sizeof(timeout));
  (void)setsockopt(socket_value, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                   (socklen_t)sizeof(timeout));
#endif
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (connect(socket_value, (const struct sockaddr *)&address,
              (socklen_t)sizeof(address)) != 0) {
    close_socket(socket_value);
    return TEST_INVALID_SOCKET;
  }
  return socket_value;
}

static int exchange(uint16_t port, uint32_t timeout_ms) {
  static const char request[] = "GET /parallel HTTP/1.1\r\n"
                                "Host: 127.0.0.1\r\n"
                                "Connection: close\r\n\r\n";
  test_socket_t socket_value = connect_loopback(port, timeout_ms);
  char response[2048];
  size_t sent = 0U;
  size_t used = 0U;

  if (socket_value == TEST_INVALID_SOCKET) {
    return -1;
  }
  while (sent < sizeof(request) - 1U) {
#if defined(_WIN32)
    const int count = send(socket_value, request + sent,
                           (int)(sizeof(request) - 1U - sent), TEST_SEND_FLAGS);
#else
    const ssize_t count = send(socket_value, request + sent,
                               sizeof(request) - 1U - sent, TEST_SEND_FLAGS);
#endif
    if (count <= 0) {
      close_socket(socket_value);
      return -1;
    }
    sent += (size_t)count;
  }
  while (used + 1U < sizeof(response)) {
#if defined(_WIN32)
    const int count = recv(socket_value, response + used,
                           (int)(sizeof(response) - used - 1U), 0);
#else
    const ssize_t count =
        recv(socket_value, response + used, sizeof(response) - used - 1U, 0);
#endif
    if (count <= 0) {
      break;
    }
    used += (size_t)count;
  }
  close_socket(socket_value);
  response[used] = '\0';
  return strstr(response, "HTTP/1.1 200") != NULL &&
                 strstr(response, "coakka-parallel-response") != NULL
             ? 0
             : -1;
}

static int client_worker(void *opaque) {
  client_state_t *state = (client_state_t *)opaque;
  uint32_t index;
  for (index = 0U; index < state->requests; ++index) {
    if (exchange(state->port, state->timeout_ms) != 0) {
      state->failures += 1U;
    }
  }
  return state->failures == 0U ? 0 : -1;
}

static int stop_race_worker(void *opaque) {
  stop_race_context_t *context = (stop_race_context_t *)opaque;
  stop_race_shared_t *shared = context->shared;

  if (context->coordinator != 0) {
    const uint64_t threshold = (uint64_t)shared->client_count * UINT64_C(8);
    while (atomic_load_explicit(&shared->attempted, memory_order_relaxed) <
           threshold) {
      if (deadline_expired(shared->deadline_ns)) {
        (void)atomic_fetch_add_explicit(&shared->unexpected, 1U,
                                        memory_order_relaxed);
        return -1;
      }
      thread_yield();
    }

    /* Publish shutdown intent before closing admission. Client-side I/O
     * failures are expected only after observing this release. */
    atomic_store_explicit(&shared->stopping, 1, memory_order_release);
    shared->stop_code = coakka_http_server_stop(shared->server).code;
    return shared->stop_code == COAKKA_HTTP_RESULT_OK ? 0 : -1;
  }

  while (!deadline_expired(shared->deadline_ns)) {
    const int exchanged = exchange(shared->port, shared->timeout_ms);
    (void)atomic_fetch_add_explicit(&shared->attempted, 1U,
                                    memory_order_relaxed);
    if (exchanged == 0) {
      (void)atomic_fetch_add_explicit(&shared->completed, 1U,
                                      memory_order_relaxed);
      thread_yield();
      continue;
    }
    if (atomic_load_explicit(&shared->stopping, memory_order_acquire) != 0) {
      (void)atomic_fetch_add_explicit(&shared->closed, 1U,
                                      memory_order_relaxed);
      return 0;
    }
    (void)atomic_fetch_add_explicit(&shared->unexpected, 1U,
                                    memory_order_relaxed);
    return -1;
  }
  (void)atomic_fetch_add_explicit(&shared->unexpected, 1U,
                                  memory_order_relaxed);
  return -1;
}

static void unused_request_handler(void *context,
                                   coakka_http_request_t *request) {
  (void)context;
  (void)request;
}

static int lifecycle_worker(void *opaque) {
  lifecycle_context_t *context = (lifecycle_context_t *)opaque;
  lifecycle_shared_t *shared = context->shared;
  uint32_t iteration;

  for (iteration = 0U; iteration < shared->config->lifecycle_iterations;
       ++iteration) {
    coakka_http_server_options_t options;
    coakka_http_route_t route;
    coakka_http_server_t *server = NULL;
    uint16_t port = 0U;
    int failed = 0;

    (void)atomic_fetch_add_explicit(&shared->attempted, 1U,
                                    memory_order_relaxed);
    coakka_http_server_options_init(&options);
    options.port = 0U;
    options.worker_count = 1U;
    options.max_connections = 4U;
    options.max_active_requests = 2U;
    options.request_queue_capacity = 2U;
    options.response_queue_capacity = 2U;
    options.max_request_body_bytes = 1024U;
    options.max_response_body_bytes = 1024U;
    options.request_timeout_ms = shared->config->timeout_ms;
    options.shutdown_timeout_ms = shared->config->timeout_ms;
    coakka_http_route_init(&route);
    route.route_id = UINT64_C(91);
    route.method = bytes("GET");
    route.path = bytes("/lifecycle");
    route.handler = unused_request_handler;

    if (coakka_http_server_create(&options, &route, 1U, &server).code !=
            COAKKA_HTTP_RESULT_OK ||
        server == NULL) {
      failed = 1;
    }
    if (failed == 0 &&
        coakka_http_server_start(server).code != COAKKA_HTTP_RESULT_OK) {
      failed = 1;
    }
    if (failed == 0 &&
        (coakka_http_server_port(server, &port).code != COAKKA_HTTP_RESULT_OK ||
         port == 0U)) {
      failed = 1;
    }
    if (failed == 0 &&
        coakka_http_server_stop(server).code != COAKKA_HTTP_RESULT_OK) {
      failed = 1;
    }
    if (failed == 0 &&
        coakka_http_server_stop(server).code != COAKKA_HTTP_RESULT_OK) {
      failed = 1;
    }
    /* Destroy is the terminal owner handoff even after a preceding operation
     * fails. The test never reuses a handle whose cleanup did not complete. */
    if (server != NULL) {
      (void)coakka_http_server_stop(server);
      if (coakka_http_server_destroy(&server).code != COAKKA_HTTP_RESULT_OK) {
        failed = 1;
      }
    }
    if (server != NULL) {
      failed = 1;
    }
    if (failed != 0 || deadline_expired(shared->deadline_ns)) {
      (void)atomic_fetch_add_explicit(&shared->failures, 1U,
                                      memory_order_relaxed);
      return -1;
    }
    (void)atomic_fetch_add_explicit(&shared->completed, 1U,
                                    memory_order_relaxed);
  }
  return 0;
}

static int run_stop_race(const test_config_t *config, test_result_t *evidence) {
  coakka_http_server_options_t options;
  coakka_http_route_t route;
  coakka_http_server_t *server = NULL;
  coakka_http_result_t operation;
  server_state_t state;
  stop_race_shared_t shared;
  stop_race_context_t contexts[TEST_MAX_CLIENTS + 1U];
  void *worker_contexts[TEST_MAX_CLIENTS + 1U];
  uint16_t port = 0U;
  size_t index;
  int thread_result;

  atomic_init(&state.calls, 0U);
  atomic_init(&state.failures, 0U);
  coakka_http_server_options_init(&options);
  options.port = 0U;
  options.worker_count = config->server_workers;
  options.max_connections = config->clients * 2U;
  options.max_active_requests = config->clients * 2U;
  options.request_queue_capacity = config->clients * 2U;
  options.response_queue_capacity = config->clients * 2U;
  options.max_request_body_bytes = 1024U;
  options.max_response_body_bytes = 1024U;
  options.request_timeout_ms = config->timeout_ms;
  options.shutdown_timeout_ms = config->timeout_ms;
  coakka_http_route_init(&route);
  route.route_id = UINT64_C(73);
  route.method = bytes("GET");
  route.path = bytes("/parallel");
  route.context = &state;
  route.handler = handle_request;

  operation = coakka_http_server_create(&options, &route, 1U, &server);
  if (operation.code == COAKKA_HTTP_RESULT_OK && server != NULL) {
    operation = coakka_http_server_start(server);
  }
  if (operation.code == COAKKA_HTTP_RESULT_OK && server != NULL) {
    operation = coakka_http_server_port(server, &port);
  }
  if (operation.code != COAKKA_HTTP_RESULT_OK || server == NULL || port == 0U) {
    fprintf(stderr, "race server setup failed: %s port=%u\n",
            coakka_http_result_code_name(operation.code), (unsigned int)port);
    if (server != NULL) {
      (void)coakka_http_server_stop(server);
      (void)coakka_http_server_destroy(&server);
    }
    return -1;
  }

  memset(&shared, 0, sizeof(shared));
  shared.server = server;
  shared.port = port;
  shared.client_count = config->clients;
  shared.timeout_ms = config->timeout_ms;
  shared.deadline_ns =
      monotonic_ns() + (uint64_t)config->timeout_ms * UINT64_C(1000000);
  shared.stop_code = COAKKA_HTTP_RESULT_INTERNAL;
  atomic_init(&shared.stopping, 0);
  atomic_init(&shared.attempted, 0U);
  atomic_init(&shared.completed, 0U);
  atomic_init(&shared.closed, 0U);
  atomic_init(&shared.unexpected, 0U);
  for (index = 0U; index < (size_t)config->clients; ++index) {
    contexts[index].shared = &shared;
    contexts[index].coordinator = 0;
    worker_contexts[index] = &contexts[index];
  }
  contexts[config->clients].shared = &shared;
  contexts[config->clients].coordinator = 1;
  worker_contexts[config->clients] = &contexts[config->clients];

  thread_result = coakka_http_test_run_threads(
      stop_race_worker, worker_contexts, (size_t)config->clients + 1U);

  /* No server owner is destroyed until every raw client and the stop caller
   * have returned. This keeps the public lifecycle non-overlapping. */
  operation = coakka_http_server_stop(server);
  {
    const coakka_http_result_t destroyed = coakka_http_server_destroy(&server);
    if (operation.code == COAKKA_HTTP_RESULT_OK) {
      operation = destroyed;
    }
  }
  evidence->stop_attempted =
      atomic_load_explicit(&shared.attempted, memory_order_relaxed);
  evidence->stop_completed =
      atomic_load_explicit(&shared.completed, memory_order_relaxed);
  evidence->stop_closed =
      atomic_load_explicit(&shared.closed, memory_order_relaxed);
  evidence->stop_unexpected =
      atomic_load_explicit(&shared.unexpected, memory_order_relaxed);
  evidence->stop_callbacks =
      atomic_load_explicit(&state.calls, memory_order_relaxed);

  if (thread_result != 0 || shared.stop_code != COAKKA_HTTP_RESULT_OK ||
      operation.code != COAKKA_HTTP_RESULT_OK || server != NULL ||
      evidence->stop_closed != config->clients ||
      evidence->stop_attempted !=
          evidence->stop_completed + evidence->stop_closed ||
      evidence->stop_callbacks < evidence->stop_completed ||
      evidence->stop_callbacks > evidence->stop_attempted ||
      evidence->stop_unexpected != 0U ||
      atomic_load_explicit(&state.failures, memory_order_relaxed) != 0U) {
    fprintf(stderr,
            "stop race failed: threads=%d stop=%s cleanup=%s "
            "attempted=%llu completed=%llu closed=%llu unexpected=%llu "
            "callbacks=%llu\n",
            thread_result, coakka_http_result_code_name(shared.stop_code),
            coakka_http_result_code_name(operation.code),
            (unsigned long long)evidence->stop_attempted,
            (unsigned long long)evidence->stop_completed,
            (unsigned long long)evidence->stop_closed,
            (unsigned long long)evidence->stop_unexpected,
            (unsigned long long)evidence->stop_callbacks);
    return -1;
  }
  return 0;
}

static int run_lifecycle_race(const test_config_t *config,
                              test_result_t *evidence) {
  lifecycle_shared_t shared;
  lifecycle_context_t contexts[TEST_MAX_CLIENTS];
  void *worker_contexts[TEST_MAX_CLIENTS];
  size_t index;
  int thread_result;

  memset(&shared, 0, sizeof(shared));
  shared.config = config;
  shared.deadline_ns =
      monotonic_ns() + (uint64_t)config->timeout_ms * UINT64_C(1000000);
  atomic_init(&shared.attempted, 0U);
  atomic_init(&shared.completed, 0U);
  atomic_init(&shared.failures, 0U);
  for (index = 0U; index < (size_t)config->clients; ++index) {
    contexts[index].shared = &shared;
    worker_contexts[index] = &contexts[index];
  }

  /* The test thread gate releases independent server owners together. No
   * server handle, route storage, or handler context crosses workers. */
  thread_result = coakka_http_test_run_threads(
      lifecycle_worker, worker_contexts, config->clients);
  evidence->lifecycle_attempted =
      atomic_load_explicit(&shared.attempted, memory_order_relaxed);
  evidence->lifecycle_completed =
      atomic_load_explicit(&shared.completed, memory_order_relaxed);
  evidence->lifecycle_failures =
      atomic_load_explicit(&shared.failures, memory_order_relaxed);
  if (thread_result != 0 || evidence->lifecycle_failures != 0U ||
      evidence->lifecycle_attempted !=
          (uint64_t)config->clients * config->lifecycle_iterations ||
      evidence->lifecycle_completed !=
          (uint64_t)config->clients * config->lifecycle_iterations) {
    fprintf(stderr,
            "lifecycle race failed: threads=%d attempted=%llu "
            "completed=%llu failures=%llu\n",
            thread_result, (unsigned long long)evidence->lifecycle_attempted,
            (unsigned long long)evidence->lifecycle_completed,
            (unsigned long long)evidence->lifecycle_failures);
    return -1;
  }
  return 0;
}

static int run_cycle(const test_config_t *config, uint32_t cycle,
                     uint64_t *completed_requests) {
  coakka_http_server_options_t options;
  coakka_http_route_t route;
  coakka_http_server_t *server = NULL;
  coakka_http_result_t result;
  server_state_t state;
  client_state_t clients[TEST_MAX_CLIENTS];
  void *contexts[TEST_MAX_CLIENTS];
  uint16_t port = 0U;
  size_t index;
  int thread_result;
  const uint32_t expected_requests =
      config->clients * config->requests_per_client;
  const uint32_t connection_capacity = config->clients * 2U;

  atomic_init(&state.calls, 0U);
  atomic_init(&state.failures, 0U);
  coakka_http_server_options_init(&options);
  options.worker_count = config->server_workers;
  options.max_connections = connection_capacity;
  options.max_active_requests = connection_capacity;
  options.request_queue_capacity = connection_capacity;
  options.response_queue_capacity = connection_capacity;
  options.max_request_body_bytes = 1024U;
  options.max_response_body_bytes = 1024U;
  options.request_timeout_ms = config->timeout_ms;
  options.shutdown_timeout_ms = config->timeout_ms;

  coakka_http_route_init(&route);
  route.route_id = UINT64_C(73);
  route.method = bytes("GET");
  route.path = bytes("/parallel");
  route.context = &state;
  route.handler = handle_request;

  result = coakka_http_server_create(&options, &route, 1U, &server);
  if (result.code != COAKKA_HTTP_RESULT_OK || server == NULL) {
    fprintf(
        stderr, "cycle %u create failed: %s detail=%s actual=%llu limit=%llu\n",
        cycle, coakka_http_result_code_name(result.code), result.detail,
        (unsigned long long)result.actual, (unsigned long long)result.limit);
    (void)coakka_http_server_destroy(&server);
    return -1;
  }
  result = coakka_http_server_start(server);
  if (result.code != COAKKA_HTTP_RESULT_OK) {
    fprintf(
        stderr, "cycle %u start failed: %s detail=%s actual=%llu limit=%llu\n",
        cycle, coakka_http_result_code_name(result.code), result.detail,
        (unsigned long long)result.actual, (unsigned long long)result.limit);
    (void)coakka_http_server_destroy(&server);
    return -1;
  }
  result = coakka_http_server_port(server, &port);
  if (result.code != COAKKA_HTTP_RESULT_OK || port == 0U) {
    fprintf(stderr, "cycle %u port failed: %s port=%u\n", cycle,
            coakka_http_result_code_name(result.code), (unsigned int)port);
    (void)coakka_http_server_destroy(&server);
    return -1;
  }

  for (index = 0U; index < (size_t)config->clients; ++index) {
    clients[index].port = port;
    clients[index].requests = config->requests_per_client;
    clients[index].timeout_ms = config->timeout_ms;
    clients[index].failures = 0U;
    contexts[index] = &clients[index];
  }
  thread_result =
      coakka_http_test_run_threads(client_worker, contexts, config->clients);
  result = coakka_http_server_destroy(&server);

  if (thread_result != 0 || result.code != COAKKA_HTTP_RESULT_OK ||
      server != NULL ||
      atomic_load_explicit(&state.calls, memory_order_relaxed) !=
          expected_requests ||
      atomic_load_explicit(&state.failures, memory_order_relaxed) != 0U) {
    fprintf(stderr,
            "cycle %u failed: threads=%d destroy=%s callbacks=%u/%u "
            "handler_failures=%u\n",
            cycle, thread_result, coakka_http_result_code_name(result.code),
            atomic_load_explicit(&state.calls, memory_order_relaxed),
            expected_requests,
            atomic_load_explicit(&state.failures, memory_order_relaxed));
    for (index = 0U; index < (size_t)config->clients; ++index) {
      if (clients[index].failures != 0U) {
        fprintf(stderr, "client %u failures=%u\n", (unsigned int)index,
                clients[index].failures);
      }
    }
    return -1;
  }
  *completed_requests += expected_requests;
  return 0;
}

int main(int argc, char **argv) {
  test_config_t config;
  test_result_t evidence;
  uint32_t cycle;
#if defined(_WIN32)
  WSADATA socket_data;
#endif

  if (parse_config(argc, argv, &config) != 0) {
    usage(argv[0]);
    return 64;
  }
  memset(&evidence, 0, sizeof(evidence));
#if defined(_WIN32)
  if (WSAStartup(MAKEWORD(2, 2), &socket_data) != 0) {
    return EXIT_FAILURE;
  }
#endif
  for (cycle = 1U; cycle <= config.cycles; ++cycle) {
    if (run_cycle(&config, cycle, &evidence.requests) != 0) {
#if defined(_WIN32)
      (void)WSACleanup();
#endif
      return EXIT_FAILURE;
    }
  }
  if (config.mode == TEST_MODE_RACE &&
      (run_stop_race(&config, &evidence) != 0 ||
       run_lifecycle_race(&config, &evidence) != 0)) {
#if defined(_WIN32)
    (void)WSACleanup();
#endif
    return EXIT_FAILURE;
  }
#if defined(_WIN32)
  (void)WSACleanup();
#endif
  printf("{\"schema\":\"coakka.http.native-concurrency.v2\","
         "\"mode\":\"%s\",\"clients\":%u,\"server_workers\":%u,"
         "\"cycles\":%u,\"lifecycle_iterations\":%u,"
         "\"requests\":%llu,\"stop_attempted\":%llu,"
         "\"stop_completed\":%llu,\"stop_closed\":%llu,"
         "\"stop_unexpected\":%llu,\"stop_callbacks\":%llu,"
         "\"lifecycle_attempted\":%llu,"
         "\"lifecycle_completed\":%llu,\"lifecycle_failures\":%llu,"
         "\"status\":\"pass\"}\n",
         mode_name(config.mode), config.clients, config.server_workers,
         config.cycles, config.lifecycle_iterations,
         (unsigned long long)evidence.requests,
         (unsigned long long)evidence.stop_attempted,
         (unsigned long long)evidence.stop_completed,
         (unsigned long long)evidence.stop_closed,
         (unsigned long long)evidence.stop_unexpected,
         (unsigned long long)evidence.stop_callbacks,
         (unsigned long long)evidence.lifecycle_attempted,
         (unsigned long long)evidence.lifecycle_completed,
         (unsigned long long)evidence.lifecycle_failures);
  return EXIT_SUCCESS;
}
