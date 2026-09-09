#include <coakka/http/http.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t checks = 0U;
static uint64_t failures = 0U;

#define CHECK(expression)                                                      \
  do {                                                                         \
    checks += 1U;                                                              \
    if (!(expression)) {                                                       \
      failures += 1U;                                                          \
      fprintf(stderr, "check failed at line %d: %s\n", __LINE__, #expression); \
    }                                                                          \
  } while (0)

static coakka_http_bytes_t bytes(const char *value) {
  coakka_http_bytes_t result;
  result.data = (const uint8_t *)value;
  result.size = value == NULL ? 0U : (uint64_t)strlen(value);
  return result;
}

static void unused_handler(void *context, coakka_http_request_t *request) {
  (void)context;
  (void)request;
}

static void valid_route(coakka_http_route_t *route) {
  coakka_http_route_init(route);
  route->route_id = 1U;
  route->method = bytes("GET");
  route->path = bytes("/contract");
  route->handler = unused_handler;
}

static void valid_options(coakka_http_server_options_t *options) {
  coakka_http_server_options_init(options);
  options->port = 0U;
  options->max_connections = 8U;
  options->max_active_requests = 4U;
  options->request_queue_capacity = 4U;
  options->response_queue_capacity = 4U;
  options->max_request_body_bytes = 1024U;
  options->max_response_body_bytes = 1024U;
  options->request_timeout_ms = 2000U;
  options->shutdown_timeout_ms = 2000U;
}

int main(void) {
  static const struct {
    coakka_http_result_code_t code;
    const char *name;
  } names[] = {
      {COAKKA_HTTP_RESULT_OK, "ok"},
      {COAKKA_HTTP_RESULT_INVALID_ARGUMENT, "invalid_argument"},
      {COAKKA_HTTP_RESULT_INVALID_STATE, "invalid_state"},
      {COAKKA_HTTP_RESULT_LIMIT_EXCEEDED, "limit_exceeded"},
      {COAKKA_HTTP_RESULT_QUEUE_FULL, "queue_full"},
      {COAKKA_HTTP_RESULT_TIMEOUT, "timeout"},
      {COAKKA_HTTP_RESULT_CLOSED, "closed"},
      {COAKKA_HTTP_RESULT_CANCELLED, "cancelled"},
      {COAKKA_HTTP_RESULT_OUT_OF_MEMORY, "out_of_memory"},
      {COAKKA_HTTP_RESULT_UNSUPPORTED, "unsupported"},
      {COAKKA_HTTP_RESULT_SYSTEM_ERROR, "system_error"},
      {COAKKA_HTTP_RESULT_INTERNAL, "internal"},
  };
  coakka_http_result_t result;
  coakka_http_server_options_t options;
  coakka_http_route_t routes[2];
  coakka_http_response_t response;
  coakka_http_server_t *server = NULL;
  coakka_http_header_t header;
  size_t index;

  CHECK(coakka_http_abi_version() == COAKKA_HTTP_ABI_VERSION);
  for (index = 0U; index < sizeof(names) / sizeof(names[0]); ++index) {
    CHECK(strcmp(coakka_http_result_code_name(names[index].code),
                 names[index].name) == 0);
  }
  CHECK(strcmp(coakka_http_result_code_name(UINT32_C(999)), "unknown") == 0);

  coakka_http_result_init(NULL);
  coakka_http_server_options_init(NULL);
  coakka_http_route_init(NULL);
  coakka_http_response_init(NULL);

  memset(&result, 0xff, sizeof(result));
  coakka_http_result_init(&result);
  CHECK(result.struct_size == sizeof(result));
  CHECK(result.code == COAKKA_HTTP_RESULT_OK);
  CHECK(result.actual == 0U && result.limit == 0U && result.detail[0] == '\0');

  coakka_http_server_options_init(&options);
  CHECK(options.struct_size == sizeof(options));
  CHECK(options.worker_count > 0U && options.max_connections > 0U);
  CHECK(options.max_active_requests > 0U &&
        options.max_active_requests <= options.max_connections);
  CHECK(options.request_queue_capacity > 0U &&
        options.response_queue_capacity > 0U);
  CHECK(options.max_request_body_bytes > 0U &&
        options.max_response_body_bytes > 0U);
  CHECK(options.request_timeout_ms > 0U && options.shutdown_timeout_ms > 0U);

  coakka_http_route_init(&routes[0]);
  CHECK(routes[0].struct_size == sizeof(routes[0]));
  coakka_http_response_init(&response);
  CHECK(response.struct_size == sizeof(response));
  CHECK(response.status_code == 200U);

  CHECK(coakka_http_request_route_id(NULL) == 0U);
  CHECK(coakka_http_request_method(NULL).size == 0U);
  CHECK(coakka_http_request_scheme(NULL).size == 0U);
  CHECK(coakka_http_request_authority(NULL).size == 0U);
  CHECK(coakka_http_request_target(NULL).size == 0U);
  CHECK(coakka_http_request_body(NULL).size == 0U);
  CHECK(coakka_http_request_header_count(NULL) == 0U);
  CHECK(coakka_http_request_header(NULL, 0U, &header) == 0U);
  CHECK(coakka_http_request_cancelled(NULL) != 0U);
  CHECK(coakka_http_request_respond(NULL, &response).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);

  valid_route(&routes[0]);
  CHECK(coakka_http_server_create(NULL, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  CHECK(coakka_http_server_create(&options, NULL, 1U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  CHECK(coakka_http_server_create(&options, routes, 0U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  CHECK(coakka_http_server_create(&options, routes, 1025U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);

  options.struct_size = 0U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  valid_options(&options);
  options.bind_address.data = NULL;
  options.bind_address.size = 1U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  valid_options(&options);

  routes[0].struct_size = 0U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  valid_route(&routes[0]);
  routes[0].route_id = 0U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  valid_route(&routes[0]);
  routes[0].method = bytes(NULL);
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  valid_route(&routes[0]);
  routes[0].method.data = NULL;
  routes[0].method.size = 1U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  valid_route(&routes[0]);
  routes[0].path = bytes(NULL);
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  valid_route(&routes[0]);
  routes[0].handler = NULL;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  valid_route(&routes[0]);

  routes[1] = routes[0];
  result = coakka_http_server_create(&options, routes, 2U, &server);
  CHECK(result.code == COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  CHECK(result.actual == 1U && result.limit == routes[0].route_id);
  CHECK(server == NULL);

  options.worker_count = 0U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_LIMIT_EXCEEDED);
  CHECK(server == NULL);
  valid_options(&options);
  options.max_active_requests = options.max_connections + 1U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_LIMIT_EXCEEDED);
  valid_options(&options);
  options.request_queue_capacity = options.max_active_requests + 1U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_LIMIT_EXCEEDED);
  valid_options(&options);
  options.response_queue_capacity = 0U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_LIMIT_EXCEEDED);
  valid_options(&options);
  options.max_request_body_bytes = 0U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_LIMIT_EXCEEDED);
  valid_options(&options);
  options.max_response_body_bytes = UINT64_MAX;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_LIMIT_EXCEEDED);
  valid_options(&options);
  options.request_timeout_ms = 0U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_LIMIT_EXCEEDED);
  valid_options(&options);
  options.shutdown_timeout_ms = 0U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_LIMIT_EXCEEDED);
  valid_options(&options);

  server = (coakka_http_server_t *)(uintptr_t)1U;
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  server = NULL;

  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_OK);
  CHECK(server != NULL);
  CHECK(coakka_http_server_port(server, &options.port).code ==
        COAKKA_HTTP_RESULT_INVALID_STATE);
  CHECK(coakka_http_server_port(NULL, &options.port).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  CHECK(coakka_http_server_port(server, NULL).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  CHECK(coakka_http_server_start(NULL).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);
  CHECK(coakka_http_server_stop(server).code == COAKKA_HTTP_RESULT_OK);
  CHECK(coakka_http_server_stop(server).code == COAKKA_HTTP_RESULT_OK);
  CHECK(coakka_http_server_start(server).code ==
        COAKKA_HTTP_RESULT_INVALID_STATE);
  CHECK(coakka_http_server_destroy(&server).code == COAKKA_HTTP_RESULT_OK);
  CHECK(server == NULL);
  CHECK(coakka_http_server_destroy(&server).code ==
        COAKKA_HTTP_RESULT_INVALID_ARGUMENT);

  valid_options(&options);
  CHECK(coakka_http_server_create(&options, routes, 1U, &server).code ==
        COAKKA_HTTP_RESULT_OK);
  CHECK(coakka_http_server_start(server).code == COAKKA_HTTP_RESULT_OK);
  CHECK(coakka_http_server_start(server).code ==
        COAKKA_HTTP_RESULT_INVALID_STATE);
  CHECK(coakka_http_server_port(server, &options.port).code ==
        COAKKA_HTTP_RESULT_OK);
  CHECK(options.port != 0U);
  CHECK(coakka_http_server_destroy(&server).code == COAKKA_HTTP_RESULT_OK);
  CHECK(server == NULL);

  printf("{\"schema\":\"coakka.http.native-contract.v1\","
         "\"checks\":%llu,\"failures\":%llu,\"status\":\"%s\"}\n",
         (unsigned long long)checks, (unsigned long long)failures,
         failures == 0U ? "pass" : "fail");
  return failures == 0U ? EXIT_SUCCESS : EXIT_FAILURE;
}
