#include <coakka/http/http.h>

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

typedef struct test_state {
  uint32_t calls;
  uint32_t failures;
  uint32_t limit_rejections;
} test_state_t;

#define CHECK(expression)                                                      \
  do {                                                                         \
    if (!(expression)) {                                                       \
      fprintf(stderr, "check failed at line %d: %s\n", __LINE__, #expression); \
      return EXIT_FAILURE;                                                     \
    }                                                                          \
  } while (0)

static coakka_http_bytes_t bytes(const char *value) {
  coakka_http_bytes_t result;
  result.data = (const uint8_t *)value;
  result.size = value == NULL ? 0U : (uint64_t)strlen(value);
  return result;
}

static int bytes_equal(coakka_http_bytes_t value, const char *expected) {
  const size_t expected_size = strlen(expected);
  return value.size == (uint64_t)expected_size &&
         (expected_size == 0U ||
          memcmp(value.data, expected, expected_size) == 0);
}

static void handle_request(void *opaque, coakka_http_request_t *request) {
  static const uint8_t oversized_body[1025] = {0U};
  test_state_t *state = (test_state_t *)opaque;
  coakka_http_response_t response;
  coakka_http_header_t header;
  coakka_http_result_t submitted;
  const uint64_t route_id = coakka_http_request_route_id(request);

  state->calls += 1U;
  if (!bytes_equal(coakka_http_request_method(request), "POST") ||
      coakka_http_request_cancelled(request) != 0U) {
    state->failures += 1U;
  }

  if (route_id == UINT64_C(42)) {
    if (!bytes_equal(coakka_http_request_target(request), "/limit")) {
      state->failures += 1U;
    }
    coakka_http_response_init(&response);
    response.body.data = oversized_body;
    response.body.size = sizeof(oversized_body);
    submitted = coakka_http_request_respond(request, &response);
    if (submitted.code != COAKKA_HTTP_RESULT_LIMIT_EXCEEDED) {
      state->failures += 1U;
    } else {
      state->limit_rejections += 1U;
    }
    response.body = bytes("limit-recovered");
    submitted = coakka_http_request_respond(request, &response);
    if (submitted.code != COAKKA_HTTP_RESULT_OK) {
      state->failures += 1U;
    }
    return;
  }

  if (route_id != UINT64_C(41) ||
      !bytes_equal(coakka_http_request_target(request), "/echo?source=abi") ||
      !bytes_equal(coakka_http_request_body(request), "request-body")) {
    state->failures += 1U;
  }

  header.name = bytes("content-type");
  header.value = bytes("text/plain");
  coakka_http_response_init(&response);
  response.status_code = 201U;
  response.headers = &header;
  response.header_count = 1U;
  response.body = bytes("coakka-native-ready");
  submitted = coakka_http_request_respond(request, &response);
  if (submitted.code != COAKKA_HTTP_RESULT_OK) {
    state->failures += 1U;
  }
  submitted = coakka_http_request_respond(request, &response);
  if (submitted.code != COAKKA_HTTP_RESULT_INVALID_STATE) {
    state->failures += 1U;
  }
}

static void close_socket(test_socket_t socket_value) {
#if defined(_WIN32)
  (void)closesocket(socket_value);
#else
  (void)close(socket_value);
#endif
}

static int socket_runtime_start(void) {
#if defined(_WIN32)
  WSADATA data;
  return WSAStartup(MAKEWORD(2, 2), &data) == 0;
#else
  return 1;
#endif
}

static void socket_runtime_stop(void) {
#if defined(_WIN32)
  (void)WSACleanup();
#endif
}

static test_socket_t connect_loopback(uint16_t port) {
  test_socket_t socket_value;
  struct sockaddr_in address;
#if defined(_WIN32)
  DWORD timeout_ms = 5000U;
#else
  struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
#endif

  socket_value = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

static int send_all(test_socket_t socket_value, const char *data, size_t size) {
  size_t offset = 0U;
  while (offset < size) {
#if defined(_WIN32)
    const int sent = send(socket_value, data + offset, (int)(size - offset), 0);
#else
    const ssize_t sent = send(socket_value, data + offset, size - offset, 0);
#endif
    if (sent <= 0) {
      return 0;
    }
    offset += (size_t)sent;
  }
  return 1;
}

static int read_response(test_socket_t socket_value, char *output,
                         size_t capacity, const char *expected_body) {
  size_t used = 0U;
  while (used + 1U < capacity) {
#if defined(_WIN32)
    const int received =
        recv(socket_value, output + used, (int)(capacity - used - 1U), 0);
#else
    const ssize_t received =
        recv(socket_value, output + used, capacity - used - 1U, 0);
#endif
    if (received <= 0) {
      break;
    }
    used += (size_t)received;
    output[used] = '\0';
    if (strstr(output, expected_body) != NULL) {
      return 1;
    }
  }
  output[used] = '\0';
  return 0;
}

int main(void) {
  static const char request_data[] = "POST /echo?source=abi HTTP/1.1\r\n"
                                     "Host: 127.0.0.1\r\n"
                                     "Content-Type: text/plain\r\n"
                                     "Content-Length: 12\r\n"
                                     "Connection: close\r\n\r\n"
                                     "request-body";
  static const char limit_request[] = "POST /limit HTTP/1.1\r\n"
                                      "Host: 127.0.0.1\r\n"
                                      "Content-Length: 0\r\n"
                                      "Connection: close\r\n\r\n";
  coakka_http_server_options_t options;
  coakka_http_route_t routes[2];
  coakka_http_server_t *server = NULL;
  coakka_http_result_t operation;
  test_state_t state = {0U, 0U, 0U};
  uint16_t port = 0U;
  test_socket_t client;
  char response_data[4096];

  CHECK(coakka_http_abi_version() == COAKKA_HTTP_ABI_VERSION);
  CHECK(strcmp(coakka_http_result_code_name(COAKKA_HTTP_RESULT_OK), "ok") == 0);
  CHECK(socket_runtime_start());

  coakka_http_server_options_init(&options);
  options.port = 0U;
  options.max_connections = 8U;
  options.max_active_requests = 4U;
  options.request_queue_capacity = 4U;
  options.response_queue_capacity = 4U;
  options.max_request_body_bytes = 1024U;
  options.max_response_body_bytes = 1024U;
  options.request_timeout_ms = 2000U;
  options.shutdown_timeout_ms = 2000U;

  coakka_http_route_init(&routes[0]);
  routes[0].route_id = UINT64_C(41);
  routes[0].method = bytes("POST");
  routes[0].path = bytes("/echo");
  routes[0].context = &state;
  routes[0].handler = handle_request;
  coakka_http_route_init(&routes[1]);
  routes[1].route_id = UINT64_C(42);
  routes[1].method = bytes("POST");
  routes[1].path = bytes("/limit");
  routes[1].context = &state;
  routes[1].handler = handle_request;

  operation = coakka_http_server_create(&options, routes, 2U, &server);
  if (operation.code != COAKKA_HTTP_RESULT_OK) {
    fprintf(stderr, "create failed: code=%s actual=%llu limit=%llu detail=%s\n",
            coakka_http_result_code_name(operation.code),
            (unsigned long long)operation.actual,
            (unsigned long long)operation.limit, operation.detail);
  }
  CHECK(operation.code == COAKKA_HTTP_RESULT_OK);
  CHECK(server != NULL);
  CHECK(coakka_http_server_start(server).code == COAKKA_HTTP_RESULT_OK);
  CHECK(coakka_http_server_port(server, &port).code == COAKKA_HTTP_RESULT_OK);
  CHECK(port != 0U);

  client = connect_loopback(port);
  CHECK(client != TEST_INVALID_SOCKET);
  CHECK(send_all(client, request_data, sizeof(request_data) - 1U));
  CHECK(read_response(client, response_data, sizeof(response_data),
                      "coakka-native-ready"));
  close_socket(client);
  CHECK(strstr(response_data, "HTTP/1.1 201") != NULL);
  CHECK(strstr(response_data, "content-type: text/plain") != NULL);
  client = connect_loopback(port);
  CHECK(client != TEST_INVALID_SOCKET);
  CHECK(send_all(client, limit_request, sizeof(limit_request) - 1U));
  CHECK(read_response(client, response_data, sizeof(response_data),
                      "limit-recovered"));
  close_socket(client);
  CHECK(strstr(response_data, "HTTP/1.1 200") != NULL);
  CHECK(state.calls == 2U);
  CHECK(state.limit_rejections == 1U);
  CHECK(state.failures == 0U);

  CHECK(coakka_http_server_stop(server).code == COAKKA_HTTP_RESULT_OK);
  CHECK(coakka_http_server_stop(server).code == COAKKA_HTTP_RESULT_OK);
  CHECK(coakka_http_server_destroy(&server).code == COAKKA_HTTP_RESULT_OK);
  CHECK(server == NULL);
  socket_runtime_stop();

  printf("coakka_http_runtime_c_test=pass requests=%u limit_rejections=%u "
         "abi=%u\n",
         state.calls, state.limit_rejections, coakka_http_abi_version());
  return EXIT_SUCCESS;
}
