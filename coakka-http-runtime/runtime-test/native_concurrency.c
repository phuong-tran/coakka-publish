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
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET test_socket_t;
#define TEST_INVALID_SOCKET INVALID_SOCKET
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
typedef int test_socket_t;
#define TEST_INVALID_SOCKET (-1)
#endif

enum {
  TEST_DEFAULT_CLIENTS = 4,
  TEST_DEFAULT_REQUESTS_PER_CLIENT = 32,
  TEST_DEFAULT_SERVER_WORKERS = 4,
  TEST_MAX_CLIENTS = 32,
  TEST_MAX_REQUESTS_PER_CLIENT = 10000,
  TEST_MAX_SERVER_WORKERS = 32,
  TEST_MAX_CYCLES = 1000
};

#define TEST_MAX_TOTAL_REQUESTS UINT64_C(1000000)

typedef struct test_config {
  uint32_t clients;
  uint32_t requests_per_client;
  uint32_t server_workers;
  uint32_t cycles;
} test_config_t;

typedef struct server_state {
  atomic_uint calls;
  atomic_uint failures;
} server_state_t;

typedef struct client_state {
  uint16_t port;
  uint32_t requests;
  uint32_t failures;
} client_state_t;

static void usage(const char *program) {
  fprintf(stderr,
          "usage: %s [--clients 1..32] [--requests-per-client 1..10000] "
          "[--server-workers 1..32] [--cycles 1..1000]\n",
          program);
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
  config->clients = TEST_DEFAULT_CLIENTS;
  config->requests_per_client = TEST_DEFAULT_REQUESTS_PER_CLIENT;
  config->server_workers = TEST_DEFAULT_SERVER_WORKERS;
  config->cycles = 1U;
  for (index = 1; index < argc; index += 2) {
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

static test_socket_t connect_loopback(uint16_t port) {
  struct sockaddr_in address;
  test_socket_t socket_value = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#if defined(_WIN32)
  DWORD timeout_ms = 5000U;
#else
  struct timeval timeout = {5, 0};
#endif

  if (socket_value == TEST_INVALID_SOCKET) {
    return TEST_INVALID_SOCKET;
  }
#if defined(_WIN32)
  (void)setsockopt(socket_value, SOL_SOCKET, SO_RCVTIMEO,
                   (const char *)&timeout_ms, (int)sizeof(timeout_ms));
#else
  (void)setsockopt(socket_value, SOL_SOCKET, SO_RCVTIMEO, &timeout,
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

static int exchange(uint16_t port) {
  static const char request[] = "GET /parallel HTTP/1.1\r\n"
                                "Host: 127.0.0.1\r\n"
                                "Connection: close\r\n\r\n";
  test_socket_t socket_value = connect_loopback(port);
  char response[2048];
  size_t sent = 0U;
  size_t used = 0U;

  if (socket_value == TEST_INVALID_SOCKET) {
    return -1;
  }
  while (sent < sizeof(request) - 1U) {
#if defined(_WIN32)
    const int count = send(socket_value, request + sent,
                           (int)(sizeof(request) - 1U - sent), 0);
#else
    const ssize_t count =
        send(socket_value, request + sent, sizeof(request) - 1U - sent, 0);
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
    if (exchange(state->port) != 0) {
      state->failures += 1U;
    }
  }
  return state->failures == 0U ? 0 : -1;
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
  options.request_timeout_ms = 5000U;
  options.shutdown_timeout_ms = 5000U;

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
  uint64_t completed_requests = 0U;
  uint32_t cycle;
#if defined(_WIN32)
  WSADATA socket_data;
#endif

  if (parse_config(argc, argv, &config) != 0) {
    usage(argv[0]);
    return 64;
  }
#if defined(_WIN32)
  if (WSAStartup(MAKEWORD(2, 2), &socket_data) != 0) {
    return EXIT_FAILURE;
  }
#endif
  for (cycle = 1U; cycle <= config.cycles; ++cycle) {
    if (run_cycle(&config, cycle, &completed_requests) != 0) {
#if defined(_WIN32)
      (void)WSACleanup();
#endif
      return EXIT_FAILURE;
    }
  }
#if defined(_WIN32)
  (void)WSACleanup();
#endif
  printf("{\"schema\":\"coakka.http.native-concurrency.v1\","
         "\"clients\":%u,\"server_workers\":%u,\"cycles\":%u,"
         "\"requests\":%llu,\"status\":\"pass\"}\n",
         config.clients, config.server_workers, config.cycles,
         (unsigned long long)completed_requests);
  return EXIT_SUCCESS;
}
