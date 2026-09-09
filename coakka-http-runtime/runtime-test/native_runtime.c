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
  uint32_t request_headers;
  uint32_t limit_rejections;
  uint32_t invalid_response_rejections;
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

static int ascii_name_equal(coakka_http_bytes_t value, const char *expected) {
  const size_t expected_size = strlen(expected);
  size_t index;
  if (value.size != (uint64_t)expected_size) {
    return 0;
  }
  for (index = 0U; index < expected_size; ++index) {
    uint8_t actual = value.data[index];
    uint8_t wanted = (uint8_t)expected[index];
    if (actual >= (uint8_t)'A' && actual <= (uint8_t)'Z') {
      actual = (uint8_t)(actual + ((uint8_t)'a' - (uint8_t)'A'));
    }
    if (wanted >= (uint8_t)'A' && wanted <= (uint8_t)'Z') {
      wanted = (uint8_t)(wanted + ((uint8_t)'a' - (uint8_t)'A'));
    }
    if (actual != wanted) {
      return 0;
    }
  }
  return 1;
}

static int header_equal(coakka_http_header_t header, const char *name,
                        const char *value) {
  return ascii_name_equal(header.name, name) &&
         bytes_equal(header.value, value);
}

static void expect_invalid_response(test_state_t *state,
                                    coakka_http_request_t *request,
                                    const coakka_http_response_t *response) {
  const coakka_http_result_t result =
      coakka_http_request_respond(request, response);
  if (result.code == COAKKA_HTTP_RESULT_INVALID_ARGUMENT) {
    state->invalid_response_rejections += 1U;
  } else {
    state->failures += 1U;
  }
}

static void handle_request(void *opaque, coakka_http_request_t *request) {
  static const uint8_t oversized_body[1025] = {0U};
  test_state_t *state = (test_state_t *)opaque;
  coakka_http_response_t response;
  coakka_http_header_t header;
  coakka_http_result_t submitted;
  const uint64_t route_id = coakka_http_request_route_id(request);
  const uint32_t header_count = coakka_http_request_header_count(request);
  uint32_t index;
  int saw_content_type = 0;
  int saw_test_header = 0;

  state->calls += 1U;
  if (!bytes_equal(coakka_http_request_method(request), "POST") ||
      !bytes_equal(coakka_http_request_scheme(request), "http") ||
      !bytes_equal(coakka_http_request_authority(request), "127.0.0.1") ||
      coakka_http_request_cancelled(request) != 0U) {
    state->failures += 1U;
  }
  for (index = 0U; index < header_count; ++index) {
    if (coakka_http_request_header(request, index, &header) == 0U) {
      state->failures += 1U;
      continue;
    }
    saw_content_type |= header_equal(header, "content-type", "text/plain");
    saw_test_header |= header_equal(header, "x-coakka-test", "request-surface");
  }
  if (header_count < 2U || saw_content_type == 0 || saw_test_header == 0 ||
      coakka_http_request_header(request, header_count, &header) != 0U ||
      coakka_http_request_header(request, 0U, NULL) != 0U) {
    state->failures += 1U;
  }
  state->request_headers += header_count;

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

  {
    uint8_t response_header_name[] = "content-type";
    uint8_t response_header_value[] = "text/plain";
    uint8_t response_body[] = "coakka-native-ready";
    header.name.data = response_header_name;
    header.name.size = sizeof(response_header_name) - 1U;
    header.value.data = response_header_value;
    header.value.size = sizeof(response_header_value) - 1U;

    coakka_http_response_init(&response);
    response.struct_size = 0U;
    expect_invalid_response(state, request, &response);
    coakka_http_response_init(&response);
    response.status_code = 99U;
    expect_invalid_response(state, request, &response);
    response.status_code = 600U;
    expect_invalid_response(state, request, &response);
    coakka_http_response_init(&response);
    response.header_count = 101U;
    expect_invalid_response(state, request, &response);
    response.header_count = 1U;
    expect_invalid_response(state, request, &response);
    coakka_http_response_init(&response);
    response.body.data = NULL;
    response.body.size = 1U;
    expect_invalid_response(state, request, &response);
    coakka_http_response_init(&response);
    response.headers = &header;
    response.header_count = 1U;
    header.name = bytes(NULL);
    expect_invalid_response(state, request, &response);
    header.name.data = NULL;
    header.name.size = 1U;
    expect_invalid_response(state, request, &response);
    header.name.data = response_header_name;
    header.name.size = sizeof(response_header_name) - 1U;
    header.value.data = NULL;
    header.value.size = 1U;
    expect_invalid_response(state, request, &response);

    header.value.data = response_header_value;
    header.value.size = sizeof(response_header_value) - 1U;
    coakka_http_response_init(&response);
    response.status_code = 201U;
    response.headers = &header;
    response.header_count = 1U;
    response.body.data = response_body;
    response.body.size = sizeof(response_body) - 1U;
    submitted = coakka_http_request_respond(request, &response);
    if (submitted.code != COAKKA_HTTP_RESULT_OK) {
      state->failures += 1U;
    }

    /* A successful call owns its copy before returning. */
    memset(response_header_name, 'x', sizeof(response_header_name) - 1U);
    memset(response_header_value, 'x', sizeof(response_header_value) - 1U);
    memset(response_body, 'x', sizeof(response_body) - 1U);
    submitted = coakka_http_request_respond(request, &response);
    if (submitted.code != COAKKA_HTTP_RESULT_INVALID_STATE) {
      state->failures += 1U;
    }
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
                                     "X-CoAkka-Test: request-surface\r\n"
                                     "Content-Length: 12\r\n"
                                     "Connection: close\r\n\r\n"
                                     "request-body";
  static const char limit_request[] = "POST /limit HTTP/1.1\r\n"
                                      "Host: 127.0.0.1\r\n"
                                      "Content-Type: text/plain\r\n"
                                      "X-CoAkka-Test: request-surface\r\n"
                                      "Content-Length: 0\r\n"
                                      "Connection: close\r\n\r\n";
  static const char missing_request[] = "GET /missing HTTP/1.1\r\n"
                                        "Host: 127.0.0.1\r\n"
                                        "Connection: close\r\n\r\n";
  coakka_http_server_options_t options;
  coakka_http_route_t routes[2];
  coakka_http_server_t *server = NULL;
  coakka_http_result_t operation;
  test_state_t state = {0U, 0U, 0U, 0U, 0U};
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
  client = connect_loopback(port);
  CHECK(client != TEST_INVALID_SOCKET);
  CHECK(send_all(client, missing_request, sizeof(missing_request) - 1U));
  CHECK(read_response(client, response_data, sizeof(response_data),
                      "HTTP/1.1 404"));
  close_socket(client);
  if (state.calls != 2U || state.request_headers < 4U ||
      state.limit_rejections != 1U || state.invalid_response_rejections != 9U ||
      state.failures != 0U) {
    fprintf(stderr,
            "request state mismatch: calls=%u headers=%u "
            "limit_rejections=%u invalid_response_rejections=%u "
            "failures=%u\n",
            state.calls, state.request_headers, state.limit_rejections,
            state.invalid_response_rejections, state.failures);
  }
  CHECK(state.calls == 2U);
  CHECK(state.request_headers >= 4U);
  CHECK(state.limit_rejections == 1U);
  CHECK(state.invalid_response_rejections == 9U);
  CHECK(state.failures == 0U);

  CHECK(coakka_http_server_stop(server).code == COAKKA_HTTP_RESULT_OK);
  CHECK(coakka_http_server_stop(server).code == COAKKA_HTTP_RESULT_OK);
  CHECK(coakka_http_server_destroy(&server).code == COAKKA_HTTP_RESULT_OK);
  CHECK(server == NULL);
  socket_runtime_stop();

  printf("{\"schema\":\"coakka.http.native-runtime.v1\","
         "\"requests\":%u,\"request_headers\":%u,"
         "\"limit_rejections\":%u,\"invalid_response_rejections\":%u,"
         "\"abi\":%u,\"status\":\"pass\"}\n",
         state.calls, state.request_headers, state.limit_rejections,
         state.invalid_response_rejections, coakka_http_abi_version());
  return EXIT_SUCCESS;
}
