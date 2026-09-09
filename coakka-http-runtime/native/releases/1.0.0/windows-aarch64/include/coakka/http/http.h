#ifndef COAKKA_HTTP_HTTP_H
#define COAKKA_HTTP_HTTP_H

#include <stddef.h>
#include <stdint.h>

#define COAKKA_HTTP_ABI_VERSION UINT32_C(1)
#define COAKKA_HTTP_RESULT_DETAIL_CAPACITY UINT32_C(192)

#if defined(_WIN32)
#if defined(COAKKA_HTTP_BUILDING)
#define COAKKA_HTTP_PUBLIC_API __declspec(dllexport)
#else
#define COAKKA_HTTP_PUBLIC_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define COAKKA_HTTP_PUBLIC_API __attribute__((visibility("default")))
#else
#define COAKKA_HTTP_PUBLIC_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t coakka_http_result_code_t;

#define COAKKA_HTTP_RESULT_OK UINT32_C(0)
#define COAKKA_HTTP_RESULT_INVALID_ARGUMENT UINT32_C(1)
#define COAKKA_HTTP_RESULT_INVALID_STATE UINT32_C(2)
#define COAKKA_HTTP_RESULT_LIMIT_EXCEEDED UINT32_C(3)
#define COAKKA_HTTP_RESULT_QUEUE_FULL UINT32_C(4)
#define COAKKA_HTTP_RESULT_TIMEOUT UINT32_C(5)
#define COAKKA_HTTP_RESULT_CLOSED UINT32_C(6)
#define COAKKA_HTTP_RESULT_CANCELLED UINT32_C(7)
#define COAKKA_HTTP_RESULT_OUT_OF_MEMORY UINT32_C(8)
#define COAKKA_HTTP_RESULT_UNSUPPORTED UINT32_C(9)
#define COAKKA_HTTP_RESULT_SYSTEM_ERROR UINT32_C(10)
#define COAKKA_HTTP_RESULT_INTERNAL UINT32_C(11)

typedef struct coakka_http_result {
  uint32_t struct_size;
  coakka_http_result_code_t code;
  uint64_t actual;
  uint64_t limit;
  char detail[COAKKA_HTTP_RESULT_DETAIL_CAPACITY];
} coakka_http_result_t;

typedef struct coakka_http_bytes {
  const uint8_t *data;
  uint64_t size;
} coakka_http_bytes_t;

typedef struct coakka_http_header {
  coakka_http_bytes_t name;
  coakka_http_bytes_t value;
} coakka_http_header_t;

typedef struct coakka_http_server coakka_http_server_t;
typedef struct coakka_http_request coakka_http_request_t;

typedef void (*coakka_http_handler_fn)(void *context,
                                       coakka_http_request_t *request);

typedef struct coakka_http_route {
  uint32_t struct_size;
  uint32_t reserved0;
  uint64_t route_id;
  coakka_http_bytes_t method;
  coakka_http_bytes_t path;
  void *context;
  coakka_http_handler_fn handler;
} coakka_http_route_t;

typedef struct coakka_http_server_options {
  uint32_t struct_size;
  uint16_t port;
  uint16_t reserved0;
  coakka_http_bytes_t bind_address;
  uint32_t worker_count;
  uint32_t max_connections;
  uint32_t max_active_requests;
  uint32_t request_queue_capacity;
  uint32_t response_queue_capacity;
  uint64_t max_request_body_bytes;
  uint64_t max_response_body_bytes;
  uint32_t request_timeout_ms;
  uint32_t shutdown_timeout_ms;
} coakka_http_server_options_t;

typedef struct coakka_http_response {
  uint32_t struct_size;
  uint32_t status_code;
  const coakka_http_header_t *headers;
  uint32_t header_count;
  uint32_t reserved0;
  coakka_http_bytes_t body;
} coakka_http_response_t;

/*
 * Initializers select conservative bounded defaults. Callers may override
 * fields after initialization and must preserve struct_size.
 */
COAKKA_HTTP_PUBLIC_API uint32_t coakka_http_abi_version(void);
COAKKA_HTTP_PUBLIC_API void
coakka_http_result_init(coakka_http_result_t *result);
COAKKA_HTTP_PUBLIC_API void
coakka_http_server_options_init(coakka_http_server_options_t *options);
COAKKA_HTTP_PUBLIC_API void coakka_http_route_init(coakka_http_route_t *route);
COAKKA_HTTP_PUBLIC_API void
coakka_http_response_init(coakka_http_response_t *response);
COAKKA_HTTP_PUBLIC_API const char *
coakka_http_result_code_name(coakka_http_result_code_t code);

/*
 * Create copies all option and route text before returning. Handler contexts
 * remain caller-owned and must stay valid until server_destroy returns. Create
 * performs no listener bind and leaves *out_server null on failure.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_create(
    const coakka_http_server_options_t *options,
    const coakka_http_route_t *routes, uint32_t route_count,
    coakka_http_server_t **out_server);

COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_server_start(coakka_http_server_t *server);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_server_port(const coakka_http_server_t *server, uint16_t *out_port);

/*
 * Lifecycle operations for one server must not overlap. Request callbacks may
 * run concurrently according to worker_count. Stop closes admission and waits
 * for bounded shutdown. It is idempotent.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_server_stop(coakka_http_server_t *server);

/* Destroy stops when necessary, releases all native state, and nulls *server.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_server_destroy(coakka_http_server_t **server);

/* Request handles and every returned byte view are valid only in the handler.
 */
COAKKA_HTTP_PUBLIC_API uint64_t
coakka_http_request_route_id(const coakka_http_request_t *request);
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_request_method(const coakka_http_request_t *request);
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_request_scheme(const coakka_http_request_t *request);
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_request_authority(const coakka_http_request_t *request);
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_request_target(const coakka_http_request_t *request);
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_request_body(const coakka_http_request_t *request);
COAKKA_HTTP_PUBLIC_API uint32_t
coakka_http_request_header_count(const coakka_http_request_t *request);
COAKKA_HTTP_PUBLIC_API uint8_t
coakka_http_request_header(const coakka_http_request_t *request, uint32_t index,
                           coakka_http_header_t *out_header);
COAKKA_HTTP_PUBLIC_API uint8_t
coakka_http_request_cancelled(const coakka_http_request_t *request);

/*
 * Exactly one terminal operation is allowed per request. Response header and
 * body bytes are copied before a successful call returns.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_request_respond(
    coakka_http_request_t *request, const coakka_http_response_t *response);

#ifdef __cplusplus
}
#endif

#endif
