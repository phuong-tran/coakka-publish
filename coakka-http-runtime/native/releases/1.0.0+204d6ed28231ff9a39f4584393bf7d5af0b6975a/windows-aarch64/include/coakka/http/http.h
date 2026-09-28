#ifndef COAKKA_HTTP_HTTP_H
#define COAKKA_HTTP_HTTP_H

#include <stddef.h>
#include <stdint.h>

#define COAKKA_HTTP_ABI_VERSION UINT32_C(8)
#define COAKKA_HTTP_RESULT_DETAIL_CAPACITY UINT32_C(192)
#define COAKKA_HTTP_TERMINAL_DETAIL_CAPACITY UINT32_C(1024)
#define COAKKA_HTTP_TIMEOUT_FOREVER UINT64_MAX

#if defined(_WIN32)
#if defined(COAKKA_HTTP_STATIC)
#define COAKKA_HTTP_PUBLIC_API
#elif defined(COAKKA_HTTP_BUILDING)
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

/* Stable numeric outcome returned by every fallible operation. */
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
#define COAKKA_HTTP_RESULT_NOT_FOUND UINT32_C(12)
#define COAKKA_HTTP_RESULT_UNAVAILABLE UINT32_C(13)

/*
 * Operation outcome. Initialize caller-allocated instances before passing
 * them to an initializer. detail is always NUL-terminated when populated.
 * actual and limit describe a rejected bound when the outcome provides one.
 */
typedef struct coakka_http_result {
  uint32_t struct_size;
  coakka_http_result_code_t code;
  uint64_t actual;
  uint64_t limit;
  char detail[COAKKA_HTTP_RESULT_DETAIL_CAPACITY];
} coakka_http_result_t;

/*
 * Non-owning byte view. A null data pointer is valid only when size is zero.
 * The operation receiving a view defines how long its bytes must remain alive.
 */
typedef struct coakka_http_bytes {
  const uint8_t *data;
  uint64_t size;
} coakka_http_bytes_t;

/* Non-owning name and value views for one HTTP header. */
typedef struct coakka_http_header {
  coakka_http_bytes_t name;
  coakka_http_bytes_t value;
} coakka_http_header_t;

typedef uint32_t coakka_http_listener_protocol_t;
#define COAKKA_HTTP_PROTOCOL_HTTP_1_1 UINT32_C(1)
#define COAKKA_HTTP_PROTOCOL_HTTP_2 UINT32_C(2)
#define COAKKA_HTTP_PROTOCOL_HTTP_3 UINT32_C(3)

typedef uint32_t coakka_http_transport_security_t;
#define COAKKA_HTTP_SECURITY_PLAINTEXT UINT32_C(1)
#define COAKKA_HTTP_SECURITY_TLS UINT32_C(2)
#define COAKKA_HTTP_SECURITY_MUTUAL_TLS UINT32_C(3)

typedef uint32_t coakka_http_body_delivery_t;
#define COAKKA_HTTP_BODY_INLINE UINT32_C(1)
#define COAKKA_HTTP_BODY_STREAM UINT32_C(2)

/* Opaque service owner created and destroyed through this API. */
typedef struct coakka_http_server coakka_http_server_t;
/* Opaque request borrowed only for the duration of its handler invocation. */
typedef struct coakka_http_request coakka_http_request_t;
/* Opaque single-writer response stream returned by one request callback. */
typedef struct coakka_http_response_stream coakka_http_response_stream_t;
typedef struct coakka_http_sse_event coakka_http_sse_event_t;
/* Declared here so service construction can borrow one monitor declaration. */
typedef struct coakka_http_monitor_options coakka_http_monitor_options_t;
typedef struct coakka_http_static_mount coakka_http_static_mount_t;
typedef struct coakka_http_file_authority coakka_http_file_authority_t;
typedef struct coakka_http_file_response coakka_http_file_response_t;
typedef struct coakka_http_outbound_options coakka_http_outbound_options_t;
typedef struct coakka_http_outbound_target coakka_http_outbound_target_t;

typedef struct coakka_http_outbound_trust {
  uint32_t struct_size;
  uint32_t reserved0;
  uint64_t generation;
  coakka_http_bytes_t ca_pem;
} coakka_http_outbound_trust_t;

typedef struct coakka_http_outbound_identity {
  uint32_t struct_size;
  uint32_t reserved0;
  uint64_t generation;
  coakka_http_bytes_t certificate_chain_pem;
  coakka_http_bytes_t private_key_pem;
} coakka_http_outbound_identity_t;

/* Construction hard ceilings for the high-level convenience server. */
#define COAKKA_HTTP_SERVER_MAX_STATIC_MOUNTS UINT32_C(16)
#define COAKKA_HTTP_SERVER_MAX_FILE_AUTHORITIES UINT32_C(16)
#define COAKKA_HTTP_SERVER_MAX_OUTBOUND_TARGETS UINT32_C(32)
#define COAKKA_HTTP_SERVER_MAX_OUTBOUND_TRUSTS UINT32_C(16)
#define COAKKA_HTTP_SERVER_MAX_OUTBOUND_IDENTITIES UINT32_C(16)
#define COAKKA_HTTP_SERVER_MAX_ROUTE_GENERATION_ROUTES UINT32_C(16384)

/*
 * Handles one admitted request. The callback may run concurrently with other
 * handlers. context is the exact pointer registered on the matching route.
 * The callback must select one buffered response or response-stream head before
 * returning and must not retain request or any view obtained from it. A
 * successfully started response-stream owner may outlive the callback.
 */
typedef void (*coakka_http_handler_fn)(void *context,
                                       coakka_http_request_t *request);

/* One route declaration copied by coakka_http_server_create. */
typedef struct coakka_http_route {
  uint32_t struct_size;
  coakka_http_body_delivery_t body_delivery;
  uint64_t route_id;
  coakka_http_bytes_t method;
  coakka_http_bytes_t path;
  void *context;
  coakka_http_handler_fn handler;
} coakka_http_route_t;

/*
 * Construction-time service options. Start with
 * coakka_http_server_options_init, then override required fields. Every count,
 * byte limit, queue capacity and timeout is a finite resource ceiling.
 */
typedef struct coakka_http_server_options {
  uint32_t struct_size;
  uint16_t port;
  uint16_t reserved0;
  coakka_http_bytes_t bind_address;
  /*
   * Complete single-listener declaration. All text is copied by create; Core
   * opens credential files and owns every resulting protocol/TLS resource.
   */
  coakka_http_listener_protocol_t protocol;
  coakka_http_transport_security_t security;
  uint64_t credential_generation;
  coakka_http_bytes_t credential_id;
  coakka_http_bytes_t certificate_chain_file;
  coakka_http_bytes_t private_key_file;
  coakka_http_bytes_t trust_roots_file;
  uint32_t worker_count;
  uint32_t max_connections;
  uint32_t max_active_requests;
  uint32_t request_queue_capacity;
  uint32_t response_queue_capacity;
  uint64_t max_request_body_bytes;
  uint64_t max_response_body_bytes;
  uint32_t request_timeout_ms;
  uint32_t response_write_timeout_ms;
  uint32_t shutdown_timeout_ms;
  /*
   * Total lifetime binding identities, including startup, staged, draining and
   * retired handlers. Binding IDs are single-assignment and never reused.
   */
  uint32_t max_handler_bindings;
  uint32_t reserved1;
  /* Borrowed only for create; NULL keeps monitoring disabled and unreserved. */
  const coakka_http_monitor_options_t *monitor;
  /*
   * Both arrays and all nested bytes are borrowed only for create and copied.
   * Counts may not exceed the COAKKA_HTTP_SERVER_MAX_* hard ceilings.
   */
  const coakka_http_static_mount_t *static_mounts;
  uint32_t static_mount_count;
  uint32_t reserved2;
  const coakka_http_file_authority_t *file_authorities;
  uint32_t file_authority_count;
  uint32_t reserved3;
  /*
   * Optional bounded outbound topology. All declarations and nested bytes are
   * copied during create. Targets enable the logical outbound lane; options
   * may be NULL to select Core's conservative defaults.
   */
  const coakka_http_outbound_options_t *outbound;
  const coakka_http_outbound_target_t *outbound_targets;
  uint32_t outbound_target_count;
  uint32_t reserved4;
  const coakka_http_outbound_trust_t *outbound_trusts;
  uint32_t outbound_trust_count;
  uint32_t reserved5;
  const coakka_http_outbound_identity_t *outbound_identities;
  uint32_t outbound_identity_count;
  uint32_t reserved6;
} coakka_http_server_options_t;

/*
 * Buffered terminal response. coakka_http_request_respond copies the header
 * array, header bytes and body bytes before a successful call returns.
 */
typedef struct coakka_http_response {
  uint32_t struct_size;
  uint32_t status_code;
  const coakka_http_header_t *headers;
  uint32_t header_count;
  uint32_t reserved0;
  coakka_http_bytes_t body;
} coakka_http_response_t;

typedef uint32_t coakka_http_request_body_event_kind_t;
#define COAKKA_HTTP_REQUEST_BODY_DATA UINT32_C(1)
#define COAKKA_HTTP_REQUEST_BODY_TRAILERS UINT32_C(2)
#define COAKKA_HTTP_REQUEST_BODY_END UINT32_C(3)
#define COAKKA_HTTP_REQUEST_BODY_CANCELLED UINT32_C(4)

/*
 * One callback-scoped incremental request-body event. DATA and trailer views
 * remain borrowed until the same request's next read or callback return.
 * private_owner is opaque and must be preserved for trailer accessors.
 */
typedef struct coakka_http_request_body_event {
  uint32_t struct_size;
  coakka_http_request_body_event_kind_t kind;
  uint64_t sequence;
  coakka_http_bytes_t data;
  const void *private_owner;
} coakka_http_request_body_event_t;

/* Returns the ABI revision implemented by the loaded shared library. */
COAKKA_HTTP_PUBLIC_API uint32_t coakka_http_abi_version(void);

/* Resets result to a successful empty outcome and records its struct size. */
COAKKA_HTTP_PUBLIC_API void
coakka_http_result_init(coakka_http_result_t *result);

/* Selects conservative bounded defaults and records the options struct size. */
COAKKA_HTTP_PUBLIC_API void
coakka_http_server_options_init(coakka_http_server_options_t *options);

/* Resets a route declaration and records its struct size. */
COAKKA_HTTP_PUBLIC_API void coakka_http_route_init(coakka_http_route_t *route);

/* Resets a buffered response to status 200 and records its struct size. */
COAKKA_HTTP_PUBLIC_API void
coakka_http_response_init(coakka_http_response_t *response);

/* Returns a process-lifetime static name for a result code. */
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

/*
 * Binds the listener and starts request admission. The server must be in its
 * created state. A failure leaves it valid for destroy but not for reuse.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_server_start(coakka_http_server_t *server);

/*
 * Prepares one nonzero, previously unused binding identity. The server adopts
 * its private callback wrapper on success; a non-NULL context remains
 * caller-owned and must stay valid until server_destroy returns. A prepared
 * binding is not dispatchable until a successful generation-checked rebind
 * publishes it.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_prepare_handler(
    coakka_http_server_t *server, uint64_t binding_id, void *context,
    coakka_http_handler_fn handler);

/*
 * Writes the bound port for a started server. This reports the selected port
 * when creation requested port zero. out_port is unchanged on failure.
 */
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

/* Returns the stable route identifier copied from the matched declaration. */
COAKKA_HTTP_PUBLIC_API uint64_t
coakka_http_request_route_id(const coakka_http_request_t *request);

/* Returns the borrowed request method view, valid only inside the handler. */
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_request_method(const coakka_http_request_t *request);

/* Returns the borrowed request scheme view, valid only inside the handler. */
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_request_scheme(const coakka_http_request_t *request);

/* Returns the borrowed authority view, valid only inside the handler. */
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_request_authority(const coakka_http_request_t *request);

/* Returns the borrowed encoded target view, valid only inside the handler. */
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_request_target(const coakka_http_request_t *request);

/* Returns the borrowed buffered body view, valid only inside the handler. */
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_request_body(const coakka_http_request_t *request);

/* Reports INLINE or STREAM for the matched construction-time route. */
COAKKA_HTTP_PUBLIC_API coakka_http_body_delivery_t
coakka_http_request_body_delivery(const coakka_http_request_t *request);

/* Resets one caller-owned request-body event and records its struct size. */
COAKKA_HTTP_PUBLIC_API void
coakka_http_request_body_event_init(coakka_http_request_body_event_t *event);

/*
 * Reads the next DATA, TRAILERS, END, or CANCELLED event for a STREAM request.
 * Exactly the executing handler thread may call this operation. Zero probes
 * once, finite waits use monotonic time, and server stop wakes a forever wait.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_request_body_read(
    coakka_http_request_t *request, uint64_t timeout_ms,
    coakka_http_request_body_event_t *out_event);

COAKKA_HTTP_PUBLIC_API uint32_t coakka_http_request_body_event_trailer_count(
    const coakka_http_request_body_event_t *event);
COAKKA_HTTP_PUBLIC_API uint8_t coakka_http_request_body_event_trailer(
    const coakka_http_request_body_event_t *event, uint32_t index,
    coakka_http_header_t *out_trailer);

/* Returns the number of headers available through request_header. */
COAKKA_HTTP_PUBLIC_API uint32_t
coakka_http_request_header_count(const coakka_http_request_t *request);

/*
 * Writes the borrowed header at index and returns one. An invalid request,
 * output pointer or index returns zero and leaves no view safe to consume.
 */
COAKKA_HTTP_PUBLIC_API uint8_t
coakka_http_request_header(const coakka_http_request_t *request, uint32_t index,
                           coakka_http_header_t *out_header);

/* Returns the number of validated buffered request trailers. */
COAKKA_HTTP_PUBLIC_API uint32_t
coakka_http_request_trailer_count(const coakka_http_request_t *request);

/*
 * Writes the borrowed trailer at index and returns one. Trailer views follow
 * the same callback-only lifetime as request headers.
 */
COAKKA_HTTP_PUBLIC_API uint8_t
coakka_http_request_trailer(const coakka_http_request_t *request,
                            uint32_t index, coakka_http_header_t *out_trailer);

/* Returns one after cancellation is observed, otherwise zero. */
COAKKA_HTTP_PUBLIC_API uint8_t
coakka_http_request_cancelled(const coakka_http_request_t *request);

/*
 * Exactly one terminal operation is allowed per request. Response header and
 * body bytes are copied before a successful call returns.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_request_respond(
    coakka_http_request_t *request, const coakka_http_response_t *response);
/* Selects one authority-confined file as this request's first completion. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_request_respond_file(coakka_http_request_t *request,
                                 const coakka_http_file_response_t *response);

/*
 * Selects a response-stream head as the request's first completion and returns
 * one caller-owned, single-writer stream. head.body must be empty. All head
 * bytes are copied synchronously. The stream becomes writable only after the
 * handler returns and Core admits the head.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_request_start_response_stream(
    coakka_http_request_t *request, const coakka_http_response_t *head,
    coakka_http_response_stream_t **out_stream);

/*
 * Selects Core's fixed 200 text/event-stream response profile. Extra headers
 * are copied synchronously. The returned writer follows the same single-owner,
 * writable-credit and terminal rules as an ordinary response stream.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_request_start_sse(
    coakka_http_request_t *request, const coakka_http_header_t *headers,
    uint32_t header_count, coakka_http_response_stream_t **out_stream);

/*
 * Waits for initial writability or recovery after a queue-full result. An
 * accepted write may be followed immediately by another write; wait again only
 * after queue pressure. Zero probes once; finite timeouts are monotonic;
 * COAKKA_HTTP_TIMEOUT_FOREVER is interrupted by server stop. Calls for one
 * stream, including finish and release, must not overlap.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_response_stream_wait_writable(coakka_http_response_stream_t *stream,
                                          uint64_t timeout_ms);

/* Copies one chunk under the configured Core stream bounds. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_response_stream_write(
    coakka_http_response_stream_t *stream, coakka_http_bytes_t data);

/* Encodes and copies one typed event on a writer opened by start_sse. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_response_stream_write_sse(coakka_http_response_stream_t *stream,
                                      const coakka_http_sse_event_t *event);

/* Selects a WebSocket upgrade and copies the optional selected subprotocol. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_request_accept_websocket(coakka_http_request_t *request,
                                     coakka_http_bytes_t selected_subprotocol);

/*
 * Copies optional final trailers. Success consumes the stream, frees its local
 * owner and nulls *stream. A failure retains the owner for an exact retry or
 * explicit release.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_response_stream_finish(
    coakka_http_response_stream_t **stream,
    const coakka_http_header_t *trailers, uint32_t trailer_count);

/*
 * Releases only the local writer owner and nulls *stream. Releasing an
 * unfinished stream does not fabricate a final frame; Core's request deadline
 * or server stop performs terminal cleanup. Release must not overlap another
 * call on the same stream. Every writer call must quiesce and every local owner
 * must be released before server_destroy.
 */
COAKKA_HTTP_PUBLIC_API void
coakka_http_response_stream_release(coakka_http_response_stream_t **stream);

/*
 * The callback-free connector bridge below is the complete language-connector
 * surface. Configuration is built through copied ordinary HTTP values; the
 * private runtime ABI and its serialization schema never cross this header.
 */

typedef uint64_t coakka_http_capabilities_t;
/*
 * Feature bits describe the loaded target image, not a cross-platform promise.
 * Connectors must test a bit before admitting its optional configuration or
 * operation; an exported function may remain present to keep one stable ABI.
 */
#define COAKKA_HTTP_CAPABILITY_INBOUND UINT64_C(1)
#define COAKKA_HTTP_CAPABILITY_TLS UINT64_C(2)
#define COAKKA_HTTP_CAPABILITY_MUTUAL_TLS UINT64_C(4)
#define COAKKA_HTTP_CAPABILITY_GZIP UINT64_C(8)
#define COAKKA_HTTP_CAPABILITY_REQUEST_STREAM UINT64_C(16)
#define COAKKA_HTTP_CAPABILITY_RESPONSE_STREAM UINT64_C(32)
#define COAKKA_HTTP_CAPABILITY_RESPONSE_TRAILERS UINT64_C(64)
#define COAKKA_HTTP_CAPABILITY_RESPONSE_WRITE_TIMEOUT UINT64_C(128)
#define COAKKA_HTTP_CAPABILITY_SERVER_SENT_EVENTS UINT64_C(256)
#define COAKKA_HTTP_CAPABILITY_WEBSOCKET UINT64_C(512)
#define COAKKA_HTTP_CAPABILITY_HTTP2 UINT64_C(1024)
#define COAKKA_HTTP_CAPABILITY_HTTP3 UINT64_C(2048)
#define COAKKA_HTTP_CAPABILITY_STATIC_FILES UINT64_C(4096)
#define COAKKA_HTTP_CAPABILITY_APPLICATION_FILES UINT64_C(8192)
#define COAKKA_HTTP_CAPABILITY_OUTBOUND UINT64_C(16384)
#define COAKKA_HTTP_CAPABILITY_ROUTE_REBIND UINT64_C(32768)
#define COAKKA_HTTP_CAPABILITY_OUTBOUND_LOGICAL_TARGET UINT64_C(65536)
#define COAKKA_HTTP_CAPABILITY_STATIC_FRONTEND UINT64_C(131072)
#define COAKKA_HTTP_CAPABILITY_HEALTH UINT64_C(262144)
#define COAKKA_HTTP_CAPABILITY_MONITOR UINT64_C(524288)
#define COAKKA_HTTP_CAPABILITY_ROUTE_MANIFEST_PUBLICATION UINT64_C(1048576)
#define COAKKA_HTTP_CAPABILITY_ALL UINT64_C(2097151)

typedef uint32_t coakka_http_provider_t;
#define COAKKA_HTTP_PROVIDER_NONE UINT32_C(0)
#define COAKKA_HTTP_PROVIDER_HOST UINT32_C(1)
#define COAKKA_HTTP_PROVIDER_RUNTIME UINT32_C(2)

typedef struct coakka_http_service_plan_request {
  uint32_t struct_size;
  uint32_t reserved;
  coakka_http_capabilities_t required_capabilities;
  coakka_http_capabilities_t host_capabilities;
  coakka_http_capabilities_t runtime_capabilities;
} coakka_http_service_plan_request_t;

typedef struct coakka_http_service_plan {
  uint32_t struct_size;
  coakka_http_provider_t provider;
  coakka_http_capabilities_t required_capabilities;
  coakka_http_capabilities_t host_capabilities;
  coakka_http_capabilities_t runtime_capabilities;
  coakka_http_capabilities_t missing_host_capabilities;
  coakka_http_capabilities_t missing_runtime_capabilities;
} coakka_http_service_plan_t;

typedef struct coakka_http_configuration coakka_http_configuration_t;
typedef struct coakka_http_core coakka_http_core_t;

typedef struct coakka_http_exchange_id {
  uint64_t slot;
  uint64_t generation;
} coakka_http_exchange_id_t;

/* Returns the generation-checked exchange identity for this callback turn. */
COAKKA_HTTP_PUBLIC_API coakka_http_exchange_id_t
coakka_http_request_exchange(const coakka_http_request_t *request);

typedef struct coakka_http_websocket_id {
  uint64_t slot;
  uint64_t generation;
} coakka_http_websocket_id_t;

typedef struct coakka_http_outbound_id {
  uint64_t slot;
  uint64_t generation;
} coakka_http_outbound_id_t;

typedef struct coakka_http_listener {
  uint32_t struct_size;
  coakka_http_listener_protocol_t protocol;
  uint64_t listener_id;
  coakka_http_bytes_t bind_address;
  uint16_t port;
  uint16_t reserved0;
  coakka_http_transport_security_t security;
  uint32_t reserved1;
  uint64_t credential_generation;
  coakka_http_bytes_t credential_id;
  coakka_http_bytes_t certificate_chain_file;
  coakka_http_bytes_t private_key_file;
  coakka_http_bytes_t trust_roots_file;
} coakka_http_listener_t;

/*
 * Construction-fixed request-body admission policy. Accepted content classes
 * control Core's Content-Type and byte-limit checks; they do not request a
 * parsed form or multipart object. Admitted bytes remain available through
 * the raw inline body view or the ordered request-stream event lane.
 */
typedef struct coakka_http_body_policy {
  uint32_t struct_size;
  uint8_t enabled;
  uint8_t accept_absent;
  uint8_t accept_other;
  uint8_t accept_form_urlencoded;
  uint8_t accept_multipart_form_data;
  uint8_t reserved0[3];
  uint64_t max_body_bytes;
} coakka_http_body_policy_t;

typedef struct coakka_http_core_route {
  uint32_t struct_size;
  coakka_http_body_delivery_t body_delivery;
  uint64_t route_id;
  uint64_t request_capture_profile_id;
  uint64_t handler_binding_id;
  coakka_http_bytes_t method;
  coakka_http_bytes_t path;
  coakka_http_body_policy_t body_policy;
} coakka_http_core_route_t;

/* Zero-valued fields select the Core default; explicit positive values override
 * it. */
typedef struct coakka_http_core_limits {
  uint32_t struct_size;
  uint32_t event_loop_threads;
  uint32_t max_listeners;
  uint32_t max_connections;
  uint32_t max_active_exchanges;
  uint32_t request_queue_depth;
  uint32_t completion_queue_depth;
  uint32_t completion_batch_size;
  uint32_t max_header_count;
  uint32_t max_header_bytes;
  uint32_t max_routes;
  uint32_t max_route_segments;
  uint32_t max_captures_per_route;
  uint32_t request_stream_chunk_slots;
  uint32_t request_stream_max_retained_items;
  uint32_t response_stream_chunk_slots;
  uint32_t response_stream_max_retained_items;
  uint32_t max_static_mounts;
  uint32_t max_total_static_files;
  uint32_t max_file_authorities;
  uint32_t route_rebind_command_capacity;
  uint32_t route_rebind_history_capacity;
  uint32_t route_rebind_max_commands_per_turn;
  uint32_t terminal_observation_queue_depth;
  uint32_t deadline_tick_ms;
  uint32_t connection_timeout_batch_size;
  uint64_t max_request_body_bytes;
  uint64_t max_response_body_bytes;
  uint64_t max_serialized_request_bytes;
  uint64_t max_serialized_response_bytes;
  uint64_t max_total_request_body_bytes;
  uint64_t max_total_response_body_bytes;
  uint64_t max_total_serialized_request_bytes;
  uint64_t max_total_serialized_response_bytes;
  uint64_t keep_alive_timeout_ms;
  uint64_t header_timeout_ms;
  uint64_t body_timeout_ms;
  uint64_t app_host_timeout_ms;
  uint64_t idle_timeout_ms;
  uint64_t drain_timeout_ms;
  uint64_t stop_timeout_ms;
  uint64_t response_write_timeout_ms;
  uint64_t request_stream_max_chunk_bytes;
  uint64_t request_stream_max_trailer_bytes;
  uint64_t request_stream_max_retained_bytes;
  uint64_t request_stream_max_total_retained_bytes;
  uint64_t response_stream_max_chunk_bytes;
  uint64_t response_stream_max_trailer_bytes;
  uint64_t response_stream_max_retained_bytes;
  uint64_t response_stream_max_total_retained_bytes;
  uint64_t max_route_manifest_bytes;
  uint64_t max_total_route_metadata_bytes;
  uint64_t max_serialized_terminal_observation_bytes;
  uint64_t max_total_serialized_terminal_observation_bytes;
} coakka_http_core_limits_t;

typedef struct coakka_http_static_mount {
  uint32_t struct_size;
  uint32_t reserved0;
  uint64_t mount_id;
  coakka_http_bytes_t url_prefix;
  coakka_http_bytes_t root_path;
  coakka_http_bytes_t index_file;
  coakka_http_bytes_t cache_control;
  coakka_http_bytes_t spa_fallback_file;
  uint8_t include_dotfiles;
  uint8_t has_index_file;
  uint8_t has_cache_control;
  uint8_t has_spa_fallback_file;
} coakka_http_static_mount_t;

typedef struct coakka_http_file_authority {
  uint32_t struct_size;
  uint32_t max_active_files;
  uint64_t authority_id;
  coakka_http_bytes_t root_path;
  uint64_t max_file_bytes;
} coakka_http_file_authority_t;

typedef uint32_t coakka_http_compression_mode_t;
#define COAKKA_HTTP_COMPRESSION_DISABLED UINT32_C(1)
#define COAKKA_HTTP_COMPRESSION_GZIP UINT32_C(2)

typedef struct coakka_http_compression {
  uint32_t struct_size;
  coakka_http_compression_mode_t mode;
  uint64_t minimum_body_bytes;
  uint64_t maximum_body_bytes;
  uint64_t maximum_encoded_bytes;
  uint64_t workspace_bytes;
  int32_t gzip_level;
  uint32_t reserved0;
} coakka_http_compression_t;

typedef uint32_t coakka_http_io_backend_t;
#define COAKKA_HTTP_IO_UNSPECIFIED UINT32_C(0)
#define COAKKA_HTTP_IO_PLATFORM_DEFAULT UINT32_C(1)
#define COAKKA_HTTP_IO_URING UINT32_C(2)

#define COAKKA_HTTP_RUNTIME_INFO_VERSION UINT32_C(1)

typedef uint32_t coakka_http_io_backend_fallback_reason_t;
#define COAKKA_HTTP_IO_BACKEND_FALLBACK_NONE UINT32_C(0)
#define COAKKA_HTTP_IO_BACKEND_FALLBACK_CONFIGURATION UINT32_C(1)
#define COAKKA_HTTP_IO_BACKEND_FALLBACK_UNAVAILABLE UINT32_C(2)

/*
 * Pointer-free facts owned by the loaded Core image and current host. The
 * io_uring probe is explicit and pull-only. It creates no retained state and
 * reports false when the provider was not compiled or the host rejects the
 * exact poller contract Core requires. The global query leaves requested and
 * effective I/O backends COAKKA_HTTP_IO_UNSPECIFIED.
 */
typedef struct coakka_http_runtime_info {
  uint32_t struct_size;
  uint32_t info_version;
  uint32_t abi_version;
  coakka_http_io_backend_t requested_io_backend;
  coakka_http_io_backend_t effective_io_backend;
  coakka_http_io_backend_fallback_reason_t fallback_reason;
  coakka_http_capabilities_t features;
  uint8_t io_uring_compiled;
  uint8_t io_uring_supported;
  uint8_t core_started;
  uint8_t reserved0[5];
  int32_t io_uring_probe_error;
  uint32_t reserved1;
} coakka_http_runtime_info_t;

typedef uint32_t coakka_http_wait_mode_t;
#define COAKKA_HTTP_WAIT_BLOCKING UINT32_C(1)
#define COAKKA_HTTP_WAIT_ADAPTIVE UINT32_C(2)
#define COAKKA_HTTP_WAIT_BUSY_POLL UINT32_C(3)

typedef struct coakka_http_wait_policy {
  uint32_t struct_size;
  coakka_http_wait_mode_t mode;
  uint32_t spin_iterations;
  uint32_t yield_iterations;
} coakka_http_wait_policy_t;

/* Zero-valued outbound limit fields select Core's conservative defaults. */
typedef struct coakka_http_outbound_lane_limits {
  uint32_t struct_size;
  uint32_t call_capacity;
  uint32_t target_profile_capacity;
  uint32_t work_batch;
  uint32_t max_method_bytes;
  uint32_t max_origin_target_bytes;
  uint32_t max_profile_component_bytes;
  uint64_t max_total_profile_bytes;
  uint32_t max_header_count;
  uint32_t max_header_name_bytes;
  uint32_t max_header_value_bytes;
  uint32_t max_header_bytes;
  uint64_t max_request_body_bytes;
  uint64_t max_total_request_bytes;
  uint64_t max_response_head_bytes;
  uint64_t max_response_body_bytes;
  uint64_t max_total_response_bytes;
  uint32_t deadline_tick_ms;
  uint32_t stop_timeout_ms;
} coakka_http_outbound_lane_limits_t;

typedef struct coakka_http_outbound_transport_limits {
  uint32_t struct_size;
  uint32_t active_call_capacity;
  uint32_t max_response_header_count;
  uint32_t max_response_header_name_bytes;
  uint32_t max_response_header_value_bytes;
  uint64_t max_response_head_bytes;
  uint64_t max_response_body_bytes;
  uint64_t max_total_buffer_bytes;
  uint32_t max_serialized_url_bytes;
  uint32_t max_resolve_entry_bytes;
} coakka_http_outbound_transport_limits_t;

typedef struct coakka_http_outbound_resolver {
  uint32_t struct_size;
  uint32_t max_resolved_addresses;
  coakka_http_bytes_t server_endpoint;
  uint32_t socket_capacity;
  uint32_t timeout_ms;
  uint32_t tries;
} coakka_http_outbound_resolver_t;

typedef struct coakka_http_outbound_pool_limits {
  uint32_t struct_size;
  uint32_t key_capacity;
  uint32_t max_connections_per_key;
  uint32_t max_waiting_per_key;
  uint32_t max_idle_seconds;
  uint32_t max_lifetime_seconds;
} coakka_http_outbound_pool_limits_t;

typedef struct coakka_http_outbound_tls_limits {
  uint32_t struct_size;
  uint32_t trust_capacity;
  uint32_t client_identity_capacity;
  uint64_t max_trust_bundle_bytes;
  uint64_t max_client_certificate_bytes;
  uint64_t max_client_private_key_bytes;
  uint64_t max_total_bytes;
} coakka_http_outbound_tls_limits_t;

typedef struct coakka_http_outbound_registry_limits {
  uint32_t struct_size;
  uint32_t target_capacity;
  uint32_t endpoint_capacity;
  uint32_t max_endpoints_per_target;
  uint32_t max_target_name_bytes;
  uint32_t max_node_id_bytes;
  uint64_t max_total_bytes;
} coakka_http_outbound_registry_limits_t;

/*
 * A present outbound configuration enables Core's native client. Resolver and
 * connection-pool resources remain disabled unless their flags are set.
 */
struct coakka_http_outbound_options {
  uint32_t struct_size;
  uint8_t resolver_enabled;
  uint8_t pool_enabled;
  uint8_t reserved0[2];
  coakka_http_outbound_lane_limits_t lane;
  coakka_http_outbound_transport_limits_t transport;
  coakka_http_outbound_resolver_t resolver;
  coakka_http_outbound_pool_limits_t pool;
  coakka_http_outbound_tls_limits_t tls;
  coakka_http_outbound_registry_limits_t registry;
};

/* Opt-in authenticated target-runtime inspection served entirely by Core. */
typedef struct coakka_http_inspection_options {
  uint32_t struct_size;
  uint8_t allow_plaintext_loopback;
  uint8_t reserved0[3];
  uint64_t runtime_instance_id;
  uint64_t runtime_generation;
  coakka_http_bytes_t bearer_file;
  uint32_t max_bearer_token_bytes;
  uint32_t call_capacity;
  uint32_t max_entries_per_turn;
  uint32_t snapshot_deadline_ms;
  uint64_t max_bytes_per_turn;
  uint64_t max_response_bytes;
  uint64_t max_total_response_bytes;
} coakka_http_inspection_options_t;

typedef struct coakka_http_outbound_endpoint {
  uint32_t struct_size;
  uint32_t weight;
  coakka_http_bytes_t node_id;
  coakka_http_bytes_t connect_host;
  uint16_t connect_port;
  uint16_t reserved0;
  coakka_http_bytes_t http_authority;
  coakka_http_transport_security_t security;
  uint32_t available;
  coakka_http_bytes_t tls_peer_identity;
  uint32_t proxy_mode;
  uint32_t protocol_policy;
  uint32_t connection_strategy;
  uint32_t reserved1;
  coakka_http_bytes_t proxy_identity;
  uint64_t tls_trust_generation;
  uint64_t tls_client_identity_generation;
  uint64_t connection_strategy_generation;
  uint64_t local_network_policy_id;
} coakka_http_outbound_endpoint_t;

typedef uint32_t coakka_http_outbound_strategy_t;
#define COAKKA_HTTP_OUTBOUND_SINGLE_OWNER UINT32_C(1)
#define COAKKA_HTTP_OUTBOUND_WEIGHTED_ROUND_ROBIN UINT32_C(2)

struct coakka_http_outbound_target {
  uint32_t struct_size;
  coakka_http_outbound_strategy_t strategy;
  coakka_http_bytes_t name;
  uint64_t generation;
  const coakka_http_outbound_endpoint_t *endpoints;
  uint32_t endpoint_count;
  uint32_t reserved0;
};

typedef uint32_t coakka_http_monitor_collection_t;
#define COAKKA_HTTP_MONITOR_DISABLED UINT32_C(0)
#define COAKKA_HTTP_MONITOR_AGGREGATES UINT32_C(1)
#define COAKKA_HTTP_MONITOR_AGGREGATES_AND_EVENTS UINT32_C(2)

struct coakka_http_monitor_options {
  uint32_t struct_size;
  coakka_http_monitor_collection_t collection;
  uint32_t event_capacity;
  uint32_t max_events_per_read;
  uint32_t detail_bytes_per_event;
  uint32_t latency_bucket_count;
  uint64_t aggregate_categories;
  uint64_t event_categories;
  uint8_t signal_reserved;
  uint8_t runtime_safe_detail;
  uint8_t fixed_latency_buckets;
  uint8_t reserved0;
};

typedef uint32_t coakka_http_event_kind_t;
#define COAKKA_HTTP_EVENT_REQUEST UINT32_C(1)
#define COAKKA_HTTP_EVENT_REQUEST_DATA UINT32_C(2)
#define COAKKA_HTTP_EVENT_REQUEST_TRAILERS UINT32_C(3)
#define COAKKA_HTTP_EVENT_REQUEST_END UINT32_C(4)
#define COAKKA_HTTP_EVENT_REQUEST_CANCELLED UINT32_C(5)
#define COAKKA_HTTP_EVENT_RESPONSE_WRITABLE UINT32_C(6)
#define COAKKA_HTTP_EVENT_TERMINAL UINT32_C(7)

/*
 * One Core-owned event lease. data has event-specific meaning and remains
 * borrowed until release. Callers must preserve every private_* field.
 */
typedef struct coakka_http_event {
  uint32_t struct_size;
  coakka_http_event_kind_t kind;
  coakka_http_exchange_id_t exchange;
  uint64_t sequence;
  uint64_t handler_binding_id;
  const uint8_t *data;
  uint64_t data_size;
  uint64_t private_lease;
  const void *private_owner;
  const void *private_state;
  uint32_t private_layout_version;
  uint32_t reserved0;
} coakka_http_event_t;

typedef uint32_t coakka_http_exchange_outcome_t;
#define COAKKA_HTTP_EXCHANGE_COMPLETED UINT32_C(1)
#define COAKKA_HTTP_EXCHANGE_TIMED_OUT UINT32_C(2)
#define COAKKA_HTTP_EXCHANGE_DISCONNECTED UINT32_C(3)
#define COAKKA_HTTP_EXCHANGE_CANCELLED UINT32_C(4)
#define COAKKA_HTTP_EXCHANGE_FAILED UINT32_C(5)
#define COAKKA_HTTP_EXCHANGE_STOPPED UINT32_C(6)

/*
 * Public-safe terminal projection copied out of Core's private observation.
 * detail_size is the original byte count; stored_detail_size is the copied
 * prefix and detail_truncated reports a larger original value.
 */
typedef struct coakka_http_terminal_summary {
  uint32_t struct_size;
  coakka_http_exchange_outcome_t outcome;
  uint32_t response_status;
  uint32_t failure_domain;
  uint32_t failure_phase;
  uint32_t failure_reason;
  uint32_t retry_disposition;
  uint32_t http_status_hint;
  uint8_t has_response;
  uint8_t has_failure;
  uint8_t has_http_status_hint;
  uint8_t detail_truncated;
  uint8_t has_admitted_time;
  uint8_t has_dispatched_time;
  uint8_t has_response_accepted_time;
  uint8_t has_completed_time;
  uint64_t admitted_monotonic_ns;
  uint64_t dispatched_monotonic_ns;
  uint64_t response_accepted_monotonic_ns;
  uint64_t completed_monotonic_ns;
  uint64_t detail_size;
  uint32_t stored_detail_size;
  uint32_t reserved0;
  char detail[COAKKA_HTTP_TERMINAL_DETAIL_CAPACITY];
} coakka_http_terminal_summary_t;

typedef struct coakka_http_path_parameter {
  coakka_http_bytes_t name;
  coakka_http_bytes_t encoded_value;
} coakka_http_path_parameter_t;

typedef struct coakka_http_query_parameter {
  coakka_http_bytes_t encoded_key;
  coakka_http_bytes_t encoded_value;
  uint8_t has_value;
  uint8_t reserved0[7];
} coakka_http_query_parameter_t;

typedef struct coakka_http_file_response {
  uint32_t struct_size;
  uint32_t reserved0;
  coakka_http_response_t head;
  uint64_t authority_id;
  coakka_http_bytes_t encoded_path;
} coakka_http_file_response_t;

struct coakka_http_sse_event {
  uint32_t struct_size;
  uint32_t reserved0;
  coakka_http_bytes_t data;
  coakka_http_bytes_t event_type;
  coakka_http_bytes_t id;
  uint64_t retry_ms;
  uint8_t has_event_type;
  uint8_t has_id;
  uint8_t has_retry;
  uint8_t reserved1[5];
};

typedef struct coakka_http_failure {
  uint32_t struct_size;
  uint32_t reason;
  coakka_http_bytes_t detail;
} coakka_http_failure_t;

typedef uint32_t coakka_http_socket_event_kind_t;
#define COAKKA_HTTP_WEBSOCKET_OPEN UINT32_C(1)
#define COAKKA_HTTP_WEBSOCKET_TEXT UINT32_C(2)
#define COAKKA_HTTP_WEBSOCKET_BINARY UINT32_C(3)
#define COAKKA_HTTP_WEBSOCKET_PING UINT32_C(4)
#define COAKKA_HTTP_WEBSOCKET_PONG UINT32_C(5)
#define COAKKA_HTTP_WEBSOCKET_WRITABLE UINT32_C(6)
#define COAKKA_HTTP_WEBSOCKET_CLOSE UINT32_C(7)
#define COAKKA_HTTP_WEBSOCKET_ACCEPT_FAILED UINT32_C(8)

typedef struct coakka_http_socket_event {
  uint32_t struct_size;
  coakka_http_socket_event_kind_t kind;
  coakka_http_websocket_id_t session;
  coakka_http_exchange_id_t origin_exchange;
  uint32_t close_code;
  uint32_t reserved0;
  const uint8_t *data;
  uint64_t data_size;
  uint64_t private_lease;
  const void *private_owner;
} coakka_http_socket_event_t;

typedef struct coakka_http_client_request {
  uint32_t struct_size;
  uint32_t timeout_ms;
  coakka_http_bytes_t logical_target;
  coakka_http_bytes_t method;
  coakka_http_bytes_t target;
  const coakka_http_header_t *headers;
  uint32_t header_count;
  uint32_t reserved0;
  coakka_http_bytes_t body;
} coakka_http_client_request_t;

#ifndef COAKKA_HTTP_OUTBOUND_TERMINAL_TYPES_DEFINED
#define COAKKA_HTTP_OUTBOUND_TERMINAL_TYPES_DEFINED

typedef uint32_t coakka_http_outbound_terminal_reason_t;
#define COAKKA_HTTP_OUTBOUND_TERMINAL_RESPONSE UINT32_C(1)
#define COAKKA_HTTP_OUTBOUND_TERMINAL_PROVIDER_FAILURE UINT32_C(2)
#define COAKKA_HTTP_OUTBOUND_TERMINAL_PROVIDER_REJECTED UINT32_C(3)
#define COAKKA_HTTP_OUTBOUND_TERMINAL_CANCELLED UINT32_C(4)
#define COAKKA_HTTP_OUTBOUND_TERMINAL_DEADLINE_EXCEEDED UINT32_C(5)
#define COAKKA_HTTP_OUTBOUND_TERMINAL_STOPPED UINT32_C(6)
#define COAKKA_HTTP_OUTBOUND_TERMINAL_RESPONSE_HEAD_LIMIT UINT32_C(7)
#define COAKKA_HTTP_OUTBOUND_TERMINAL_RESPONSE_BODY_LIMIT UINT32_C(8)
#define COAKKA_HTTP_OUTBOUND_TERMINAL_RESPONSE_AGGREGATE_LIMIT UINT32_C(9)
#define COAKKA_HTTP_OUTBOUND_TERMINAL_OUT_OF_MEMORY UINT32_C(10)
#define COAKKA_HTTP_OUTBOUND_TERMINAL_CLOCK_FAILURE UINT32_C(11)
#define COAKKA_HTTP_OUTBOUND_TERMINAL_INVALID_PROVIDER_RESULT UINT32_C(12)

typedef uint32_t coakka_http_outbound_terminal_phase_t;
#define COAKKA_HTTP_OUTBOUND_PHASE_ADMISSION UINT32_C(1)
#define COAKKA_HTTP_OUTBOUND_PHASE_PROVIDER_ADOPTION UINT32_C(2)
#define COAKKA_HTTP_OUTBOUND_PHASE_TRANSPORT UINT32_C(3)
#define COAKKA_HTTP_OUTBOUND_PHASE_RESPONSE UINT32_C(4)
#define COAKKA_HTTP_OUTBOUND_PHASE_CANCELLATION UINT32_C(5)
#define COAKKA_HTTP_OUTBOUND_PHASE_SHUTDOWN UINT32_C(6)

typedef uint32_t coakka_http_outbound_retry_disposition_t;
#define COAKKA_HTTP_OUTBOUND_RETRY_NEVER UINT32_C(1)
#define COAKKA_HTTP_OUTBOUND_RETRY_APP_HOST_MAY_RETRY_IF_SAFE UINT32_C(2)
#define COAKKA_HTTP_OUTBOUND_RETRY_DELIVERY_UNCERTAIN UINT32_C(3)

typedef uint32_t coakka_http_outbound_delivery_certainty_t;
#define COAKKA_HTTP_OUTBOUND_DELIVERY_NO_REQUEST_BYTES_SENT UINT32_C(1)
#define COAKKA_HTTP_OUTBOUND_DELIVERY_REQUEST_UNCERTAIN UINT32_C(2)
#define COAKKA_HTTP_OUTBOUND_DELIVERY_RESPONSE_HEAD_OBSERVED UINT32_C(3)
#define COAKKA_HTTP_OUTBOUND_DELIVERY_COMPLETE_RESPONSE_ADMITTED UINT32_C(4)

#endif

typedef struct coakka_http_client_terminal {
  uint32_t struct_size;
  coakka_http_outbound_terminal_reason_t reason;
  coakka_http_outbound_id_t call;
  coakka_http_bytes_t logical_target;
  uint64_t target_generation;
  coakka_http_bytes_t selected_node_id;
  coakka_http_outbound_terminal_phase_t phase;
  coakka_http_outbound_retry_disposition_t retry;
  coakka_http_outbound_delivery_certainty_t certainty;
  uint16_t response_status;
  uint16_t reserved0;
  int32_t provider_code;
  uint32_t response_header_count;
  coakka_http_bytes_t response_body;
  coakka_http_bytes_t diagnostic;
  uint64_t private_lease;
  const void *private_owner;
} coakka_http_client_terminal_t;

/*
 * Passive health observes current ownership, not a completion barrier.
 * server_active_exchanges includes terminal exchanges still held by a request
 * lease or awaiting event-loop reaping. Releasing a terminal observation or
 * request lease does not guarantee that this gauge is already zero. A liveness
 * probe proves loop progress, but its acknowledgment may precede reaping.
 */
typedef struct coakka_http_health {
  uint32_t struct_size;
  uint32_t lifecycle;
  uint64_t snapshot_sequence;
  uint64_t change_sequence;
  uint64_t lifecycle_generation;
  uint64_t observed_monotonic_ns;
  uint64_t progress_sequence;
  uint64_t progress_monotonic_ns;
  uint64_t acknowledged_probe_sequence;
  uint64_t acknowledged_probe_monotonic_ns;
  uint32_t configured_components;
  uint32_t running_components;
  uint32_t failed_components;
  uint32_t startup;
  uint32_t liveness;
  uint32_t readiness;
  uint8_t admission_open;
  uint8_t partial;
  uint8_t reserved0[6];
  uint32_t server_active_exchanges;
  uint32_t client_active_calls;
  uint32_t client_ready_terminals;
  uint32_t client_outstanding_terminal_leases;
  uint64_t client_retained_request_bytes;
  uint64_t client_retained_target_profile_bytes;
  uint64_t client_retained_response_bytes;
  uint64_t client_admitted_calls;
  uint64_t client_terminal_calls;
  uint64_t client_stale_provider_terminals;
} coakka_http_health_t;

/*
 * Runtime monitoring values contain only bounded, pointer-free operational
 * truth. They never contain request, response, credential or application
 * payload bytes and remain valid after their producing call returns.
 */
typedef uint32_t coakka_http_monitor_notification_t;
#define COAKKA_HTTP_MONITOR_NOTIFY_POLL UINT32_C(0)
#define COAKKA_HTTP_MONITOR_NOTIFY_SIGNAL UINT32_C(1)

typedef uint32_t coakka_http_monitor_latency_t;
#define COAKKA_HTTP_MONITOR_LATENCY_NONE UINT32_C(0)
#define COAKKA_HTTP_MONITOR_LATENCY_FIXED UINT32_C(1)

typedef uint32_t coakka_http_monitor_detail_t;
#define COAKKA_HTTP_MONITOR_DETAIL_NONE UINT32_C(0)
#define COAKKA_HTTP_MONITOR_DETAIL_RUNTIME_SAFE UINT32_C(1)

#define COAKKA_HTTP_MONITOR_CATEGORY_BIT_LIFECYCLE (UINT64_C(1) << 0)
#define COAKKA_HTTP_MONITOR_CATEGORY_BIT_CONFIG (UINT64_C(1) << 1)
#define COAKKA_HTTP_MONITOR_CATEGORY_BIT_CONNECTION (UINT64_C(1) << 2)
#define COAKKA_HTTP_MONITOR_CATEGORY_BIT_TRAFFIC (UINT64_C(1) << 3)
#define COAKKA_HTTP_MONITOR_CATEGORY_BIT_HTTP (UINT64_C(1) << 4)
#define COAKKA_HTTP_MONITOR_CATEGORY_BIT_EXCHANGE (UINT64_C(1) << 5)
#define COAKKA_HTTP_MONITOR_CATEGORY_BIT_QUEUE (UINT64_C(1) << 6)
#define COAKKA_HTTP_MONITOR_CATEGORY_BIT_FAILURE (UINT64_C(1) << 7)
#define COAKKA_HTTP_MONITOR_CATEGORY_BIT_TLS (UINT64_C(1) << 8)
#define COAKKA_HTTP_MONITOR_CATEGORY_BIT_CONNECTOR (UINT64_C(1) << 9)
#define COAKKA_HTTP_MONITOR_CATEGORY_BIT_RESPONSE (UINT64_C(1) << 10)
#define COAKKA_HTTP_MONITOR_CATEGORY_BIT_LATENCY (UINT64_C(1) << 11)

#define COAKKA_HTTP_MONITOR_MAX_FAILURE_COUNTS UINT32_C(48)
#define COAKKA_HTTP_MONITOR_FIXED_LATENCY_BUCKETS UINT32_C(16)
#define COAKKA_HTTP_MONITOR_MAX_EVENT_DETAIL_BYTES UINT32_C(128)
#define COAKKA_HTTP_MONITOR_MAX_EVENTS_PER_READ UINT32_C(64)

typedef struct coakka_http_monitor_policy_view {
  uint32_t struct_size;
  coakka_http_monitor_collection_t collection;
  coakka_http_monitor_notification_t notification;
  coakka_http_monitor_latency_t latency;
  coakka_http_monitor_detail_t detail;
  uint32_t active_event_capacity;
  uint32_t active_detail_bytes;
  uint32_t reserved0;
  uint64_t aggregate_categories;
  uint64_t event_categories;
} coakka_http_monitor_policy_view_t;

typedef struct coakka_http_monitor_config_view {
  uint32_t struct_size;
  uint32_t provenance;
  uint64_t generation;
  uint64_t collection_epoch;
  uint64_t change_sequence;
  uint64_t epoch_started_monotonic_ns;
  uint64_t policy_applied_monotonic_ns;
  uint64_t supported_categories;
  uint32_t reserved_event_capacity;
  uint32_t max_events_per_read;
  uint32_t reserved_detail_bytes_per_event;
  uint32_t reserved_latency_bucket_count;
  uint8_t signal_reserved;
  uint8_t reserved0[7];
  coakka_http_monitor_policy_view_t policy;
} coakka_http_monitor_config_view_t;

typedef uint32_t coakka_http_monitor_apply_reason_view_t;
#define COAKKA_HTTP_MONITOR_APPLY_REASON_APPLIED UINT32_C(0)
#define COAKKA_HTTP_MONITOR_APPLY_REASON_INVALID_ARGUMENT UINT32_C(1)
#define COAKKA_HTTP_MONITOR_APPLY_REASON_INVALID_ENUM UINT32_C(2)
#define COAKKA_HTTP_MONITOR_APPLY_REASON_UNSUPPORTED_CATEGORY UINT32_C(3)
#define COAKKA_HTTP_MONITOR_APPLY_REASON_INVALID_CATEGORY_RELATIONSHIP         \
  UINT32_C(4)
#define COAKKA_HTTP_MONITOR_APPLY_REASON_RESOURCE_RESERVATION_EXCEEDED         \
  UINT32_C(5)
#define COAKKA_HTTP_MONITOR_APPLY_REASON_SIGNAL_NOT_RESERVED UINT32_C(6)
#define COAKKA_HTTP_MONITOR_APPLY_REASON_GENERATION_CONFLICT UINT32_C(7)
#define COAKKA_HTTP_MONITOR_APPLY_REASON_INVALID_LIFECYCLE UINT32_C(8)
#define COAKKA_HTTP_MONITOR_APPLY_REASON_SEQUENCE_EXHAUSTED UINT32_C(9)
#define COAKKA_HTTP_MONITOR_APPLY_REASON_INTERNAL UINT32_C(10)

typedef struct coakka_http_monitor_apply_outcome {
  uint32_t struct_size;
  coakka_http_monitor_apply_reason_view_t reason;
  uint64_t actual;
  uint64_t limit;
  uint8_t changed;
  uint8_t reserved0[7];
  coakka_http_monitor_config_view_t effective;
} coakka_http_monitor_apply_outcome_t;

typedef struct coakka_http_monitor_failure_count_view {
  uint32_t reason;
  uint32_t reserved0;
  uint64_t count;
} coakka_http_monitor_failure_count_view_t;

typedef struct coakka_http_monitor_latency_bucket_view {
  uint64_t upper_bound_ns;
  uint64_t count;
} coakka_http_monitor_latency_bucket_view_t;

typedef struct coakka_http_monitor_snapshot_view {
  uint32_t struct_size;
  uint32_t failure_count;
  uint32_t latency_bucket_count;
  uint8_t collecting;
  uint8_t stale;
  uint8_t partial;
  uint8_t reserved0;
  uint64_t snapshot_sequence;
  uint64_t observed_monotonic_ns;
  uint64_t collected_change_sequence;
  uint64_t inbound_admitted_requests;
  uint64_t inbound_request_bytes;
  uint64_t exchange_completed;
  uint64_t exchange_failed;
  uint64_t exchange_timed_out;
  uint64_t exchange_disconnected;
  uint64_t exchange_cancelled;
  uint64_t response_status_family[5];
  uint64_t retained_events;
  uint64_t event_overwrite_count;
  uint64_t event_drop_count;
  uint64_t signal_notify_count;
  uint64_t signal_coalesced_count;
  uint64_t signal_failure_count;
  coakka_http_monitor_config_view_t config;
  coakka_http_health_t health;
  coakka_http_monitor_failure_count_view_t
      failure_counts[COAKKA_HTTP_MONITOR_MAX_FAILURE_COUNTS];
  coakka_http_monitor_latency_bucket_view_t
      latency_buckets[COAKKA_HTTP_MONITOR_FIXED_LATENCY_BUCKETS];
} coakka_http_monitor_snapshot_view_t;

typedef uint32_t coakka_http_monitor_event_kind_view_t;
#define COAKKA_HTTP_MONITOR_EVENT_KIND_LIFECYCLE_CHANGED UINT32_C(1)
#define COAKKA_HTTP_MONITOR_EVENT_KIND_POLICY_APPLIED UINT32_C(2)
#define COAKKA_HTTP_MONITOR_EVENT_KIND_POLICY_REJECTED UINT32_C(3)
#define COAKKA_HTTP_MONITOR_EVENT_KIND_EXCHANGE_COMPLETED UINT32_C(4)
#define COAKKA_HTTP_MONITOR_EVENT_KIND_EXCHANGE_FAILED UINT32_C(5)
#define COAKKA_HTTP_MONITOR_EVENT_KIND_EXCHANGE_TIMED_OUT UINT32_C(6)
#define COAKKA_HTTP_MONITOR_EVENT_KIND_EXCHANGE_DISCONNECTED UINT32_C(7)
#define COAKKA_HTTP_MONITOR_EVENT_KIND_EXCHANGE_CANCELLED UINT32_C(8)

typedef struct coakka_http_monitor_event_view {
  uint32_t struct_size;
  coakka_http_monitor_event_kind_view_t kind;
  uint64_t sequence;
  uint64_t change_sequence;
  uint64_t config_generation;
  uint64_t collection_epoch;
  uint64_t observed_monotonic_ns;
  uint64_t category;
  uint32_t failure_reason;
  uint32_t queue_scope;
  uint32_t queue_depth;
  uint32_t queue_capacity;
  coakka_http_exchange_id_t exchange;
  uint32_t detail_size;
  uint32_t reserved0;
  char detail[COAKKA_HTTP_MONITOR_MAX_EVENT_DETAIL_BYTES];
} coakka_http_monitor_event_view_t;

typedef struct coakka_http_monitor_event_page_view {
  uint32_t struct_size;
  uint32_t count;
  uint64_t oldest_available_sequence;
  uint64_t latest_sequence;
  uint64_t missed_events;
  uint64_t remaining_events;
} coakka_http_monitor_event_page_view_t;

#ifndef COAKKA_HTTP_ROUTE_REBIND_CODES_DEFINED
#define COAKKA_HTTP_ROUTE_REBIND_CODES_DEFINED
typedef uint32_t coakka_http_route_rebind_code_t;
#define COAKKA_HTTP_ROUTE_REBIND_APPLIED UINT32_C(0)
#define COAKKA_HTTP_ROUTE_REBIND_INVALID_ARGUMENT UINT32_C(1)
#define COAKKA_HTTP_ROUTE_REBIND_CLOSED UINT32_C(2)
#define COAKKA_HTTP_ROUTE_REBIND_QUEUE_FULL UINT32_C(3)
#define COAKKA_HTTP_ROUTE_REBIND_HISTORY_FULL UINT32_C(4)
#define COAKKA_HTTP_ROUTE_REBIND_ACTIVATION_CONFLICT UINT32_C(5)
#define COAKKA_HTTP_ROUTE_REBIND_ACTIVATION_EXPIRED UINT32_C(6)
#define COAKKA_HTTP_ROUTE_REBIND_ACTIVATION_OUT_OF_ORDER UINT32_C(7)
#define COAKKA_HTTP_ROUTE_REBIND_TIMEOUT UINT32_C(8)
#define COAKKA_HTTP_ROUTE_REBIND_WAKEUP_FAILED UINT32_C(9)
#define COAKKA_HTTP_ROUTE_REBIND_ROUTE_NOT_FOUND UINT32_C(10)
#define COAKKA_HTTP_ROUTE_REBIND_ROUTE_GENERATION_MISMATCH UINT32_C(11)
#define COAKKA_HTTP_ROUTE_REBIND_BINDING_REVISION_MISMATCH UINT32_C(12)
#define COAKKA_HTTP_ROUTE_REBIND_REVISION_EXHAUSTED UINT32_C(13)
#define COAKKA_HTTP_ROUTE_REBIND_SEQUENCE_EXHAUSTED UINT32_C(14)
#define COAKKA_HTTP_ROUTE_REBIND_INTERNAL UINT32_C(15)
#endif

typedef struct coakka_http_route_rebind {
  uint32_t struct_size;
  uint32_t reserved0;
  uint64_t activation_id;
  uint64_t expected_route_generation;
  uint64_t route_id;
  uint64_t expected_binding_revision;
  uint64_t new_handler_binding_id;
} coakka_http_route_rebind_t;

typedef struct coakka_http_route_rebind_outcome {
  uint32_t struct_size;
  coakka_http_route_rebind_code_t code;
  uint64_t activation_id;
  uint64_t operation_digest;
  uint64_t route_generation;
  uint64_t route_id;
  uint64_t previous_handler_binding_id;
  uint64_t previous_binding_revision;
  uint64_t effective_handler_binding_id;
  uint64_t effective_binding_revision;
  uint64_t binding_change_sequence;
  uint32_t retained_previous_exchanges;
  uint32_t previous_effective_route_references;
  uint8_t changed;
  uint8_t replayed;
  uint8_t reserved0[2];
} coakka_http_route_rebind_outcome_t;

#ifndef COAKKA_HTTP_ROUTE_MANIFEST_PUBLICATION_CODES_DEFINED
#define COAKKA_HTTP_ROUTE_MANIFEST_PUBLICATION_CODES_DEFINED
typedef uint32_t coakka_http_route_manifest_publication_code_t;
#define COAKKA_HTTP_ROUTE_MANIFEST_APPLIED UINT32_C(0)
#define COAKKA_HTTP_ROUTE_MANIFEST_INVALID_ARGUMENT UINT32_C(1)
#define COAKKA_HTTP_ROUTE_MANIFEST_CLOSED UINT32_C(2)
#define COAKKA_HTTP_ROUTE_MANIFEST_QUEUE_FULL UINT32_C(3)
#define COAKKA_HTTP_ROUTE_MANIFEST_HISTORY_FULL UINT32_C(4)
#define COAKKA_HTTP_ROUTE_MANIFEST_ACTIVATION_CONFLICT UINT32_C(5)
#define COAKKA_HTTP_ROUTE_MANIFEST_ACTIVATION_EXPIRED UINT32_C(6)
#define COAKKA_HTTP_ROUTE_MANIFEST_ACTIVATION_OUT_OF_ORDER UINT32_C(7)
#define COAKKA_HTTP_ROUTE_MANIFEST_TIMEOUT UINT32_C(8)
#define COAKKA_HTTP_ROUTE_MANIFEST_WAKEUP_FAILED UINT32_C(9)
#define COAKKA_HTTP_ROUTE_MANIFEST_GENERATION_MISMATCH UINT32_C(10)
#define COAKKA_HTTP_ROUTE_MANIFEST_GENERATION_EXHAUSTED UINT32_C(11)
#define COAKKA_HTTP_ROUTE_MANIFEST_REJECTED UINT32_C(12)
#define COAKKA_HTTP_ROUTE_MANIFEST_OUT_OF_MEMORY UINT32_C(13)
#define COAKKA_HTTP_ROUTE_MANIFEST_INTERNAL UINT32_C(14)
#endif

/*
 * One complete structural generation. Route text and the route array are
 * borrowed only for the synchronous call and copied before it returns.
 * Handler bindings must have been prepared on the same service first.
 */
typedef struct coakka_http_route_publication {
  uint32_t struct_size;
  uint32_t route_count;
  uint64_t activation_id;
  uint64_t expected_route_generation;
  uint64_t expected_metadata_generation;
  uint64_t expected_binding_change_sequence;
  const coakka_http_core_route_t *routes;
} coakka_http_route_publication_t;

typedef struct coakka_http_route_publication_outcome {
  uint32_t struct_size;
  coakka_http_route_manifest_publication_code_t code;
  uint64_t activation_id;
  uint64_t operation_digest;
  uint64_t previous_route_generation;
  uint64_t previous_metadata_generation;
  uint64_t previous_binding_change_sequence;
  uint64_t effective_route_generation;
  uint64_t effective_metadata_generation;
  uint64_t effective_binding_change_sequence;
  uint64_t actual;
  uint64_t limit;
  uint32_t rejected_route_index;
  uint8_t changed;
  uint8_t replayed;
  uint8_t reserved0[2];
} coakka_http_route_publication_outcome_t;

/*
 * Publishes one prepared convenience-server handler on Core's event-loop
 * owner. TIMEOUT and WAKEUP_FAILED outcomes are delivery-ambiguous; retry the
 * exact request. Typed rejection leaves the prior effective binding unchanged.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_rebind_handler(
    coakka_http_server_t *server, const coakka_http_route_rebind_t *request,
    uint64_t timeout_ms, coakka_http_route_rebind_outcome_t *out_outcome);
/*
 * Publishes one complete route generation on Core's event loop. TIMEOUT and
 * WAKEUP_FAILED are delivery-ambiguous; retry the exact request and route
 * payload while its bounded replay history remains retained. If that retry
 * returns ACTIVATION_EXPIRED, stop and recreate the service before another
 * generation; the prior result cannot be inferred safely. Other typed
 * rejection preserves the prior effective generation.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_publish_routes(
    coakka_http_server_t *server,
    const coakka_http_route_publication_t *request, uint64_t timeout_ms,
    coakka_http_route_publication_outcome_t *out_outcome);

COAKKA_HTTP_PUBLIC_API coakka_http_capabilities_t coakka_http_features(void);
COAKKA_HTTP_PUBLIC_API void coakka_http_service_plan_request_init(
    coakka_http_service_plan_request_t *request);
COAKKA_HTTP_PUBLIC_API void
coakka_http_service_plan_init(coakka_http_service_plan_t *plan);
/*
 * Selects one provider before listener publication. The host mask is an
 * audited connector implementation fact; connectors must not probe the OS to
 * manufacture it. runtime_capabilities normally comes from
 * coakka_http_features(). Valid input always writes a diagnostic plan. An
 * unsupported result selects NONE and startup must fail without downgrade.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_plan_service(const coakka_http_service_plan_request_t *request,
                         coakka_http_service_plan_t *out_plan);
COAKKA_HTTP_PUBLIC_API void
coakka_http_runtime_info_init(coakka_http_runtime_info_t *info);
/*
 * Probes only runtime-owned build/host capability. Unsupported io_uring is a
 * successful snapshot with io_uring_supported zero and a diagnostic error.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_runtime_get_info(coakka_http_runtime_info_t *out_info);
COAKKA_HTTP_PUBLIC_API void
coakka_http_listener_init(coakka_http_listener_t *listener);
COAKKA_HTTP_PUBLIC_API void
coakka_http_body_policy_init(coakka_http_body_policy_t *policy);
COAKKA_HTTP_PUBLIC_API void
coakka_http_core_route_init(coakka_http_core_route_t *route);
COAKKA_HTTP_PUBLIC_API void
coakka_http_core_limits_init(coakka_http_core_limits_t *limits);
COAKKA_HTTP_PUBLIC_API void
coakka_http_static_mount_init(coakka_http_static_mount_t *mount);
COAKKA_HTTP_PUBLIC_API void
coakka_http_file_authority_init(coakka_http_file_authority_t *authority);
COAKKA_HTTP_PUBLIC_API void
coakka_http_compression_init(coakka_http_compression_t *compression);
COAKKA_HTTP_PUBLIC_API void
coakka_http_wait_policy_init(coakka_http_wait_policy_t *policy);
COAKKA_HTTP_PUBLIC_API void
coakka_http_outbound_options_init(coakka_http_outbound_options_t *options);
COAKKA_HTTP_PUBLIC_API void
coakka_http_inspection_options_init(coakka_http_inspection_options_t *options);
COAKKA_HTTP_PUBLIC_API void
coakka_http_outbound_endpoint_init(coakka_http_outbound_endpoint_t *endpoint);
COAKKA_HTTP_PUBLIC_API void
coakka_http_outbound_target_init(coakka_http_outbound_target_t *target);
COAKKA_HTTP_PUBLIC_API void
coakka_http_outbound_trust_init(coakka_http_outbound_trust_t *trust);
COAKKA_HTTP_PUBLIC_API void coakka_http_outbound_identity_init(
    coakka_http_outbound_identity_t *identity);
COAKKA_HTTP_PUBLIC_API void
coakka_http_monitor_options_init(coakka_http_monitor_options_t *monitor);
COAKKA_HTTP_PUBLIC_API void coakka_http_event_init(coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API void
coakka_http_terminal_summary_init(coakka_http_terminal_summary_t *summary);
COAKKA_HTTP_PUBLIC_API void
coakka_http_file_response_init(coakka_http_file_response_t *response);
COAKKA_HTTP_PUBLIC_API void
coakka_http_sse_event_init(coakka_http_sse_event_t *event);
COAKKA_HTTP_PUBLIC_API void
coakka_http_socket_event_init(coakka_http_socket_event_t *event);
COAKKA_HTTP_PUBLIC_API void
coakka_http_failure_init(coakka_http_failure_t *failure);
COAKKA_HTTP_PUBLIC_API void
coakka_http_outbound_request_init(coakka_http_client_request_t *request);
COAKKA_HTTP_PUBLIC_API void
coakka_http_outbound_terminal_init(coakka_http_client_terminal_t *terminal);
COAKKA_HTTP_PUBLIC_API void
coakka_http_health_init(coakka_http_health_t *health);
COAKKA_HTTP_PUBLIC_API void
coakka_http_monitor_policy_view_init(coakka_http_monitor_policy_view_t *policy);
COAKKA_HTTP_PUBLIC_API void
coakka_http_monitor_config_view_init(coakka_http_monitor_config_view_t *config);
COAKKA_HTTP_PUBLIC_API void coakka_http_monitor_apply_outcome_init(
    coakka_http_monitor_apply_outcome_t *outcome);
COAKKA_HTTP_PUBLIC_API void coakka_http_monitor_snapshot_view_init(
    coakka_http_monitor_snapshot_view_t *snapshot);
COAKKA_HTTP_PUBLIC_API void
coakka_http_monitor_event_view_init(coakka_http_monitor_event_view_t *event);
COAKKA_HTTP_PUBLIC_API void coakka_http_monitor_event_page_init(
    coakka_http_monitor_event_page_view_t *page);
COAKKA_HTTP_PUBLIC_API void
coakka_http_route_rebind_init(coakka_http_route_rebind_t *request);
COAKKA_HTTP_PUBLIC_API void coakka_http_route_rebind_outcome_init(
    coakka_http_route_rebind_outcome_t *outcome);
COAKKA_HTTP_PUBLIC_API void
coakka_http_route_publication_init(coakka_http_route_publication_t *request);
COAKKA_HTTP_PUBLIC_API void coakka_http_route_publication_outcome_init(
    coakka_http_route_publication_outcome_t *outcome);

/*
 * Convenience-server observability uses the same pointer-free Core values as
 * the callback-free surface. Runtime info is valid before or after start;
 * health requires a started server. Caller initializes each output first.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_get_runtime_info(
    const coakka_http_server_t *server, coakka_http_runtime_info_t *out_info);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_health(
    const coakka_http_server_t *server, coakka_http_health_t *out_health);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_probe_liveness(
    const coakka_http_server_t *server, uint64_t timeout_ms,
    coakka_http_health_t *out_health);
/*
 * Complete convenience-server monitor plane. Snapshot reads are multi-reader;
 * exactly one wait may block. wait may overlap interrupt or stop: stop closes
 * call admission, wakes the waiter, and retains the server until it returns.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_monitor_config(
    const coakka_http_server_t *server,
    coakka_http_monitor_config_view_t *out_config);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_monitor_snapshot(
    const coakka_http_server_t *server,
    coakka_http_monitor_snapshot_view_t *out_snapshot);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_monitor_apply(
    coakka_http_server_t *server, uint64_t expected_generation,
    const coakka_http_monitor_policy_view_t *policy,
    coakka_http_monitor_apply_outcome_t *out_outcome);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_monitor_read(
    const coakka_http_server_t *server, uint64_t after_sequence,
    uint32_t maximum_events, coakka_http_monitor_event_view_t *events,
    uint32_t event_capacity, coakka_http_monitor_event_page_view_t *out_page);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_monitor_wait(
    const coakka_http_server_t *server, uint64_t timeout_ms);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_server_monitor_interrupt(coakka_http_server_t *server);

/*
 * Exactly one thread may own take_websocket for a server. Successful take
 * returns borrowed data that remains valid until exact release and retains one
 * server call lease. Stop wakes a blocking take, rejects new calls, and waits
 * up to the configured shutdown timeout for exact release; release remains
 * legal while stop is in progress. Send and close copy byte inputs
 * synchronously.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_take_websocket(
    coakka_http_server_t *server, uint64_t timeout_ms,
    coakka_http_socket_event_t *out_event);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_server_release_websocket(coakka_http_server_t *server,
                                     coakka_http_socket_event_t *event);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_send_websocket(
    coakka_http_server_t *server, coakka_http_websocket_id_t session,
    coakka_http_socket_event_kind_t kind, coakka_http_bytes_t data);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_close_websocket(
    coakka_http_server_t *server, coakka_http_websocket_id_t session,
    uint32_t code, coakka_http_bytes_t reason);

/*
 * Logical outbound requests copy every input byte before submit returns.
 * Exactly one thread may take terminals, and at most one terminal lease may
 * remain borrowed. Stop wakes a blocking take and waits for exact release.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_outbound_submit(
    coakka_http_server_t *server, const coakka_http_client_request_t *request,
    coakka_http_outbound_id_t *out_call);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_outbound_cancel(
    coakka_http_server_t *server, coakka_http_outbound_id_t call);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_take_outbound(
    coakka_http_server_t *server, uint64_t timeout_ms,
    coakka_http_client_terminal_t *out_terminal);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_outbound_header(
    coakka_http_server_t *server,
    const coakka_http_client_terminal_t *terminal, uint32_t index,
    coakka_http_header_t *out_header);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_server_release_outbound(
    coakka_http_server_t *server, coakka_http_client_terminal_t *terminal);

/* Configuration is single-owner and mutable until core creation begins. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_create(coakka_http_configuration_t **out_config);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_destroy(coakka_http_configuration_t **config);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_set_limits(coakka_http_configuration_t *config,
                                     const coakka_http_core_limits_t *limits);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_add_listener(coakka_http_configuration_t *config,
                                       const coakka_http_listener_t *listener);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_configuration_add_route(
    coakka_http_configuration_t *config, const coakka_http_core_route_t *route);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_add_static_mount(
    coakka_http_configuration_t *config,
    const coakka_http_static_mount_t *mount);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_add_file_authority(
    coakka_http_configuration_t *config,
    const coakka_http_file_authority_t *authority);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_set_compression(
    coakka_http_configuration_t *config,
    const coakka_http_compression_t *compression);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_set_io_backend(coakka_http_configuration_t *config,
                                         coakka_http_io_backend_t backend);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_set_wait_policy(
    coakka_http_configuration_t *config,
    const coakka_http_wait_policy_t *policy);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_set_monitor(
    coakka_http_configuration_t *config,
    const coakka_http_monitor_options_t *monitor);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_set_inspection(
    coakka_http_configuration_t *config,
    const coakka_http_inspection_options_t *inspection);
/* Replaces all outbound reservations while preserving added
 * credentials/targets. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_set_outbound(
    coakka_http_configuration_t *config,
    const coakka_http_outbound_options_t *options);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_enable_outbound(coakka_http_configuration_t *config);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_add_outbound_target(
    coakka_http_configuration_t *config,
    const coakka_http_outbound_target_t *target);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_add_outbound_trust(
    coakka_http_configuration_t *config, uint64_t generation,
    coakka_http_bytes_t ca_pem);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_configuration_add_outbound_identity(
    coakka_http_configuration_t *config, uint64_t generation,
    coakka_http_bytes_t certificate_chain_pem,
    coakka_http_bytes_t private_key_pem);

/*
 * Core copies the complete configuration; config remains caller-owned.
 * The owner must stop readers/callers and release every lease before destroy.
 * No operation may overlap destroy on the same opaque handle.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_create(
    const coakka_http_configuration_t *config, coakka_http_core_t **out_core);
/*
 * Returns requested backend identity and the effective backend at the instant
 * of the query. Effective selection may change once during start if Core falls
 * back from an unavailable requested provider. The Core remains caller-owned
 * and must not overlap destroy with this query.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_get_runtime_info(
    const coakka_http_core_t *core, coakka_http_runtime_info_t *out_info);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_start(coakka_http_core_t *core);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_bound_port(const coakka_http_core_t *core, uint16_t *out_port);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_drain(coakka_http_core_t *core);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_stop(coakka_http_core_t *core);
/* Wakes the sole event reader without stopping Core or fabricating an event. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_interrupt(coakka_http_core_t *core);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_destroy(coakka_http_core_t **core);

/*
 * Exactly one connector reader owns take_event for a core. After stop, queued
 * terminal observations remain readable and releasable; an empty queue returns
 * COAKKA_HTTP_RESULT_CLOSED. Other event kinds are not produced after stop.
 * Request and terminal observations have independent leases. Releasing a
 * request may enqueue event-loop cleanup; releasing a terminal observation
 * returns only that observation's storage. Neither release is a drain barrier.
 */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_take_event(coakka_http_core_t *core, uint64_t timeout_ms,
                            coakka_http_event_t *out_event);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_release_event(
    coakka_http_core_t *core, coakka_http_event_t *event);

/* Request views are valid only for a leased COAKKA_HTTP_EVENT_REQUEST. */
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_event_request_method(const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_event_request_scheme(const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_event_request_authority(const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_event_request_target(const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API coakka_http_bytes_t
coakka_http_event_request_body(const coakka_http_event_t *event);
/*
 * Returns COAKKA_HTTP_BODY_INLINE or COAKKA_HTTP_BODY_STREAM when the request
 * carries a body. Zero is the read-only sentinel for an absent request body;
 * it is not a valid route-configuration delivery mode.
 */
COAKKA_HTTP_PUBLIC_API uint32_t
coakka_http_event_request_body_delivery(const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API uint64_t
coakka_http_event_request_route_id(const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API uint64_t
coakka_http_event_request_route_generation(const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API uint64_t
coakka_http_event_request_binding_revision(const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API uint32_t
coakka_http_event_request_header_count(const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API uint8_t coakka_http_event_request_header(
    const coakka_http_event_t *event, uint32_t index,
    coakka_http_header_t *out_header);
COAKKA_HTTP_PUBLIC_API uint32_t coakka_http_event_request_path_parameter_count(
    const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API uint8_t coakka_http_event_request_path_parameter(
    const coakka_http_event_t *event, uint32_t index,
    coakka_http_path_parameter_t *out_parameter);
COAKKA_HTTP_PUBLIC_API uint32_t coakka_http_event_request_query_parameter_count(
    const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API uint8_t coakka_http_event_request_query_parameter(
    const coakka_http_event_t *event, uint32_t index,
    coakka_http_query_parameter_t *out_parameter);
COAKKA_HTTP_PUBLIC_API uint32_t
coakka_http_event_request_trailer_count(const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API uint8_t coakka_http_event_request_trailer(
    const coakka_http_event_t *event, uint32_t index,
    coakka_http_header_t *out_trailer);
/* Stream-trailer views remain borrowed until the containing event is released.
 */
COAKKA_HTTP_PUBLIC_API uint32_t
coakka_http_event_stream_trailer_count(const coakka_http_event_t *event);
COAKKA_HTTP_PUBLIC_API uint8_t coakka_http_event_stream_trailer(
    const coakka_http_event_t *event, uint32_t index,
    coakka_http_header_t *out_trailer);
/* Copies the decoded terminal summary; it remains valid after event release. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_event_terminal_summary(const coakka_http_event_t *event,
                                   coakka_http_terminal_summary_t *out_summary);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_exchange_cancelled(
    coakka_http_core_t *core, coakka_http_exchange_id_t exchange,
    uint8_t *out_cancelled);

/* Every successful first-completion call copies all borrowed input bytes. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_respond(
    coakka_http_core_t *core, coakka_http_exchange_id_t exchange,
    const coakka_http_response_t *response);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_respond_file(
    coakka_http_core_t *core, coakka_http_exchange_id_t exchange,
    const coakka_http_file_response_t *response);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_fail(
    coakka_http_core_t *core, coakka_http_exchange_id_t exchange,
    const coakka_http_failure_t *failure);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_start_response_stream(coakka_http_core_t *core,
                                       coakka_http_exchange_id_t exchange,
                                       const coakka_http_response_t *head);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_start_sse(
    coakka_http_core_t *core, coakka_http_exchange_id_t exchange,
    const coakka_http_header_t *headers, uint32_t header_count);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_write_response_stream(coakka_http_core_t *core,
                                       coakka_http_exchange_id_t exchange,
                                       coakka_http_bytes_t data);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_write_sse(
    coakka_http_core_t *core, coakka_http_exchange_id_t exchange,
    const coakka_http_sse_event_t *event);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_finish_response_stream(coakka_http_core_t *core,
                                        coakka_http_exchange_id_t exchange,
                                        const coakka_http_header_t *trailers,
                                        uint32_t trailer_count);

COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_accept_websocket(
    coakka_http_core_t *core, coakka_http_exchange_id_t exchange,
    coakka_http_bytes_t selected_subprotocol);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_take_websocket(coakka_http_core_t *core, uint64_t timeout_ms,
                                coakka_http_socket_event_t *out_event);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_release_websocket(
    coakka_http_core_t *core, coakka_http_socket_event_t *event);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_send_websocket(
    coakka_http_core_t *core, coakka_http_websocket_id_t session,
    coakka_http_socket_event_kind_t kind, coakka_http_bytes_t data);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_close_websocket(
    coakka_http_core_t *core, coakka_http_websocket_id_t session, uint32_t code,
    coakka_http_bytes_t reason);

COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_outbound_submit(
    coakka_http_core_t *core, const coakka_http_client_request_t *request,
    coakka_http_outbound_id_t *out_call);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_outbound_cancel(
    coakka_http_core_t *core, coakka_http_outbound_id_t call);
/* Wakes the sole outbound terminal reader without cancelling admitted calls. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_interrupt_outbound(coakka_http_core_t *core);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_take_outbound(coakka_http_core_t *core, uint64_t timeout_ms,
                               coakka_http_client_terminal_t *out_terminal);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_outbound_header(
    coakka_http_core_t *core, const coakka_http_client_terminal_t *terminal,
    uint32_t index, coakka_http_header_t *out_header);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_release_outbound(
    coakka_http_core_t *core, coakka_http_client_terminal_t *terminal);

COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_rebind(
    coakka_http_core_t *core, const coakka_http_route_rebind_t *request,
    uint64_t timeout_ms, coakka_http_route_rebind_outcome_t *out_outcome);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_publish_routes(
    coakka_http_core_t *core,
    const coakka_http_route_publication_t *request, uint64_t timeout_ms,
    coakka_http_route_publication_outcome_t *out_outcome);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_health(
    coakka_http_core_t *core, coakka_http_health_t *out_health);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_probe_liveness(coakka_http_core_t *core, uint64_t timeout_ms,
                                coakka_http_health_t *out_health);

/* Snapshot reads are multi-reader safe and never wake a runtime loop. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_monitor_config(
    coakka_http_core_t *core, coakka_http_monitor_config_view_t *out_config);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_monitor_snapshot(
    coakka_http_core_t *core,
    coakka_http_monitor_snapshot_view_t *out_snapshot);
/* A rejected live policy is an OK call with a typed non-APPLIED outcome. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_monitor_apply(
    coakka_http_core_t *core, uint64_t expected_generation,
    const coakka_http_monitor_policy_view_t *policy,
    coakka_http_monitor_apply_outcome_t *out_outcome);
/* Caller initializes all event slots; Core copies at most maximum_events. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t coakka_http_core_monitor_read(
    coakka_http_core_t *core, uint64_t after_sequence, uint32_t maximum_events,
    coakka_http_monitor_event_view_t *events, uint32_t event_capacity,
    coakka_http_monitor_event_page_view_t *out_page);
/* One blocking monitor waiter is allowed; success means pull newer truth. */
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_monitor_wait(coakka_http_core_t *core, uint64_t timeout_ms);
COAKKA_HTTP_PUBLIC_API coakka_http_result_t
coakka_http_core_monitor_interrupt(coakka_http_core_t *core);

#ifdef __cplusplus
}
#endif

#endif
