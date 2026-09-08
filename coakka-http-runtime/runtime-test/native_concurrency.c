#include <coakka/http/http.h>

#include "test_threads.h"

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

enum { TEST_WORKERS = 4, TEST_REQUESTS_PER_WORKER = 32 };

typedef struct server_state {
  atomic_uint calls;
  atomic_uint failures;
} server_state_t;

typedef struct client_state {
  uint16_t port;
  uint32_t requests;
  uint32_t failures;
} client_state_t;

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

int main(void) {
  coakka_http_server_options_t options;
  coakka_http_route_t route;
  coakka_http_server_t *server = NULL;
  coakka_http_result_t result;
  server_state_t state;
  client_state_t clients[TEST_WORKERS];
  void *contexts[TEST_WORKERS];
  uint16_t port = 0U;
  size_t index;
  int thread_result;
#if defined(_WIN32)
  WSADATA socket_data;
  if (WSAStartup(MAKEWORD(2, 2), &socket_data) != 0) {
    return EXIT_FAILURE;
  }
#endif

  atomic_init(&state.calls, 0U);
  atomic_init(&state.failures, 0U);
  coakka_http_server_options_init(&options);
  options.worker_count = TEST_WORKERS;
  options.max_connections = 64U;
  options.max_active_requests = 64U;
  options.request_queue_capacity = 64U;
  options.response_queue_capacity = 64U;
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
    fprintf(stderr, "create failed: %s detail=%s actual=%llu limit=%llu\n",
            coakka_http_result_code_name(result.code), result.detail,
            (unsigned long long)result.actual,
            (unsigned long long)result.limit);
    (void)coakka_http_server_destroy(&server);
#if defined(_WIN32)
    (void)WSACleanup();
#endif
    return EXIT_FAILURE;
  }
  result = coakka_http_server_start(server);
  if (result.code != COAKKA_HTTP_RESULT_OK) {
    fprintf(stderr, "start failed: %s detail=%s actual=%llu limit=%llu\n",
            coakka_http_result_code_name(result.code), result.detail,
            (unsigned long long)result.actual,
            (unsigned long long)result.limit);
    (void)coakka_http_server_destroy(&server);
#if defined(_WIN32)
    (void)WSACleanup();
#endif
    return EXIT_FAILURE;
  }
  result = coakka_http_server_port(server, &port);
  if (result.code != COAKKA_HTTP_RESULT_OK || port == 0U) {
    fprintf(stderr, "port failed: %s port=%u\n",
            coakka_http_result_code_name(result.code), (unsigned int)port);
    (void)coakka_http_server_destroy(&server);
#if defined(_WIN32)
    (void)WSACleanup();
#endif
    return EXIT_FAILURE;
  }

  for (index = 0U; index < TEST_WORKERS; ++index) {
    clients[index].port = port;
    clients[index].requests = TEST_REQUESTS_PER_WORKER;
    clients[index].failures = 0U;
    contexts[index] = &clients[index];
  }
  thread_result =
      coakka_http_test_run_threads(client_worker, contexts, TEST_WORKERS);
  result = coakka_http_server_destroy(&server);
#if defined(_WIN32)
  (void)WSACleanup();
#endif

  if (thread_result != 0 || result.code != COAKKA_HTTP_RESULT_OK ||
      server != NULL ||
      atomic_load_explicit(&state.calls, memory_order_relaxed) !=
          TEST_WORKERS * TEST_REQUESTS_PER_WORKER ||
      atomic_load_explicit(&state.failures, memory_order_relaxed) != 0U) {
    fprintf(stderr,
            "concurrency failed: threads=%d destroy=%s callbacks=%u "
            "handler_failures=%u clients=%u,%u,%u,%u\n",
            thread_result, coakka_http_result_code_name(result.code),
            atomic_load_explicit(&state.calls, memory_order_relaxed),
            atomic_load_explicit(&state.failures, memory_order_relaxed),
            clients[0].failures, clients[1].failures, clients[2].failures,
            clients[3].failures);
    return EXIT_FAILURE;
  }
  printf("{\"schema\":\"coakka.http.native-concurrency.v1\","
         "\"workers\":%u,\"requests\":%u,\"status\":\"pass\"}\n",
         (unsigned int)TEST_WORKERS,
         (unsigned int)(TEST_WORKERS * TEST_REQUESTS_PER_WORKER));
  return EXIT_SUCCESS;
}
