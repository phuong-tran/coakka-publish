#ifndef COAKKA_HTTP_HOST_H
#define COAKKA_HTTP_HOST_H

#include <stddef.h>
#include <stdint.h>

#define COAKKA_HTTP_HOST_ABI_VERSION UINT32_C(4)
#define COAKKA_HTTP_HOST_ISSUE_DETAIL_CAPACITY UINT32_C(192)

#define COAKKA_HTTP_HOST_MAX_ROUTES UINT32_C(1024)
#define COAKKA_HTTP_HOST_MAX_TOTAL_SEGMENTS UINT32_C(16384)
#define COAKKA_HTTP_HOST_MAX_ROUTE_BYTES UINT32_C(1048576)
#define COAKKA_HTTP_HOST_MAX_METHOD_BYTES UINT32_C(32)
#define COAKKA_HTTP_HOST_MAX_PATH_BYTES UINT32_C(8192)
#define COAKKA_HTTP_HOST_MAX_CAPTURES_PER_ROUTE UINT32_C(32)
#define COAKKA_HTTP_HOST_MAX_STATIC_MOUNTS UINT32_C(32)
#define COAKKA_HTTP_HOST_MAX_FILE_AUTHORITIES UINT32_C(32)
#define COAKKA_HTTP_HOST_MAX_OUTBOUND_TARGETS UINT32_C(64)
#define COAKKA_HTTP_HOST_MAX_OUTBOUND_ENDPOINTS_PER_TARGET UINT32_C(64)
#define COAKKA_HTTP_HOST_MAX_OUTBOUND_TRUSTS UINT32_C(16)
#define COAKKA_HTTP_HOST_MAX_OUTBOUND_IDENTITIES UINT32_C(16)
#define COAKKA_HTTP_HOST_MAX_ACTIVATION_HISTORY UINT32_C(4096)
#define COAKKA_HTTP_HOST_MAX_MONITOR_EVENTS UINT32_C(65536)
#define COAKKA_HTTP_HOST_MAX_MONITOR_READ UINT32_C(64)
#define COAKKA_HTTP_HOST_MAX_MONITOR_FAILURE_COUNTS UINT32_C(48)
#define COAKKA_HTTP_HOST_MONITOR_FIXED_LATENCY_BUCKETS UINT32_C(16)
#define COAKKA_HTTP_HOST_MAX_LIVENESS_TIMEOUT_MS UINT64_C(60000)
#define COAKKA_HTTP_HOST_FAILED_INDEX_NONE UINT32_MAX

#if defined(_WIN32)
#if defined(COAKKA_HTTP_HOST_BUILDING)
#define COAKKA_HTTP_HOST_API __declspec(dllexport)
#else
#define COAKKA_HTTP_HOST_API __declspec(dllimport)
#endif
#else
#define COAKKA_HTTP_HOST_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t coakka_http_host_status_t;
#define COAKKA_HTTP_HOST_OK INT32_C(0)
#define COAKKA_HTTP_HOST_INVALID_ARGUMENT INT32_C(1)
#define COAKKA_HTTP_HOST_INVALID_METHOD INT32_C(2)
#define COAKKA_HTTP_HOST_INVALID_PATTERN INT32_C(3)
#define COAKKA_HTTP_HOST_CAPACITY_EXHAUSTED INT32_C(4)
#define COAKKA_HTTP_HOST_AMBIGUOUS INT32_C(5)
#define COAKKA_HTTP_HOST_OUT_OF_MEMORY INT32_C(6)
#define COAKKA_HTTP_HOST_DUPLICATE_ROUTE_ID INT32_C(7)
#define COAKKA_HTTP_HOST_DUPLICATE_BINDING_ID INT32_C(8)
#define COAKKA_HTTP_HOST_UNSUPPORTED INT32_C(9)
#define COAKKA_HTTP_HOST_CLOSED INT32_C(10)
#define COAKKA_HTTP_HOST_TIMEOUT INT32_C(11)
#define COAKKA_HTTP_HOST_NOT_FOUND INT32_C(12)
#define COAKKA_HTTP_HOST_IO_ERROR INT32_C(13)
#define COAKKA_HTTP_HOST_UNKNOWN INT32_C(127)

typedef uint32_t coakka_http_host_issue_reason_t;
#define COAKKA_HTTP_HOST_ISSUE_NONE UINT32_C(0)
#define COAKKA_HTTP_HOST_ISSUE_INVALID_ARGUMENT UINT32_C(1)
#define COAKKA_HTTP_HOST_ISSUE_INVALID_STATE UINT32_C(2)
#define COAKKA_HTTP_HOST_ISSUE_CAPACITY UINT32_C(3)
#define COAKKA_HTTP_HOST_ISSUE_TIMEOUT UINT32_C(4)
#define COAKKA_HTTP_HOST_ISSUE_CLOSED UINT32_C(5)
#define COAKKA_HTTP_HOST_ISSUE_OUT_OF_MEMORY UINT32_C(6)
#define COAKKA_HTTP_HOST_ISSUE_UNSUPPORTED UINT32_C(7)
#define COAKKA_HTTP_HOST_ISSUE_LISTENER_START UINT32_C(8)
#define COAKKA_HTTP_HOST_ISSUE_TRANSPORT_SECURITY UINT32_C(9)
#define COAKKA_HTTP_HOST_ISSUE_IO_BACKEND UINT32_C(10)
#define COAKKA_HTTP_HOST_ISSUE_INTERNAL UINT32_C(11)

/*
 * One bounded causal operation failure. Connectors copy this value into their
 * language-native error type and preserve unknown reason values. detail is
 * operator-facing and must not be returned to an HTTP client automatically.
 */
typedef struct coakka_http_host_issue {
  uint32_t struct_size;
  coakka_http_host_status_t status;
  coakka_http_host_issue_reason_t reason;
  int32_t system_code;
  uint32_t item_index;
  uint32_t reserved0;
  uint64_t actual;
  uint64_t limit;
  char detail[COAKKA_HTTP_HOST_ISSUE_DETAIL_CAPACITY];
} coakka_http_host_issue_t;

typedef struct coakka_http_host_bytes {
  const uint8_t *data;
  size_t size;
} coakka_http_host_bytes_t;

typedef uint64_t coakka_http_host_capabilities_t;
#define COAKKA_HTTP_HOST_CAPABILITY_INBOUND UINT64_C(1)
#define COAKKA_HTTP_HOST_CAPABILITY_TLS UINT64_C(2)
#define COAKKA_HTTP_HOST_CAPABILITY_MUTUAL_TLS UINT64_C(4)
#define COAKKA_HTTP_HOST_CAPABILITY_GZIP UINT64_C(8)
#define COAKKA_HTTP_HOST_CAPABILITY_REQUEST_STREAM UINT64_C(16)
#define COAKKA_HTTP_HOST_CAPABILITY_RESPONSE_STREAM UINT64_C(32)
#define COAKKA_HTTP_HOST_CAPABILITY_RESPONSE_TRAILERS UINT64_C(64)
#define COAKKA_HTTP_HOST_CAPABILITY_RESPONSE_WRITE_TIMEOUT UINT64_C(128)
#define COAKKA_HTTP_HOST_CAPABILITY_SERVER_SENT_EVENTS UINT64_C(256)
#define COAKKA_HTTP_HOST_CAPABILITY_WEBSOCKET UINT64_C(512)
#define COAKKA_HTTP_HOST_CAPABILITY_HTTP2 UINT64_C(1024)
#define COAKKA_HTTP_HOST_CAPABILITY_HTTP3 UINT64_C(2048)
#define COAKKA_HTTP_HOST_CAPABILITY_STATIC_FILES UINT64_C(4096)
#define COAKKA_HTTP_HOST_CAPABILITY_APPLICATION_FILES UINT64_C(8192)
#define COAKKA_HTTP_HOST_CAPABILITY_OUTBOUND UINT64_C(16384)
#define COAKKA_HTTP_HOST_CAPABILITY_ROUTE_REBIND UINT64_C(32768)
#define COAKKA_HTTP_HOST_CAPABILITY_OUTBOUND_LOGICAL_TARGET UINT64_C(65536)
#define COAKKA_HTTP_HOST_CAPABILITY_STATIC_FRONTEND UINT64_C(131072)
#define COAKKA_HTTP_HOST_CAPABILITY_HEALTH UINT64_C(262144)
#define COAKKA_HTTP_HOST_CAPABILITY_MONITOR UINT64_C(524288)
#define COAKKA_HTTP_HOST_CAPABILITY_ROUTE_PUBLICATION UINT64_C(1048576)
#define COAKKA_HTTP_HOST_CAPABILITY_ALL UINT64_C(2097151)

/*
 * Connectors declare adapter implementation facts only. This call validates
 * the complete service declaration and its capability dependencies; it does
 * not inspect the operating system or select a second request data plane.
 */
typedef struct coakka_http_host_service_plan_request {
  uint32_t struct_size;
  uint32_t reserved;
  coakka_http_host_capabilities_t required_capabilities;
  coakka_http_host_capabilities_t adapter_capabilities;
} coakka_http_host_service_plan_request_t;

typedef struct coakka_http_host_service_plan {
  uint32_t struct_size;
  uint8_t supported;
  uint8_t reserved0[3];
  coakka_http_host_capabilities_t required_capabilities;
  coakka_http_host_capabilities_t adapter_capabilities;
  coakka_http_host_capabilities_t missing_capabilities;
} coakka_http_host_service_plan_t;

typedef uint32_t coakka_http_host_route_flags_t;
#define COAKKA_HTTP_HOST_ROUTE_WEBSOCKET UINT32_C(1)
#define COAKKA_HTTP_HOST_ROUTE_STREAMING_REQUEST UINT32_C(2)

/* Optional raw request-body admission policy; parsing remains connector-owned.
 */
typedef struct coakka_http_host_body_policy {
  uint32_t struct_size;
  uint8_t enabled;
  uint8_t accept_absent;
  uint8_t accept_other;
  uint8_t accept_form_urlencoded;
  uint8_t accept_multipart_form_data;
  uint8_t reserved0[3];
  uint64_t max_body_bytes;
} coakka_http_host_body_policy_t;

typedef struct coakka_http_host_route {
  uint32_t struct_size;
  coakka_http_host_route_flags_t flags;
  uint64_t route_id;
  uint64_t handler_binding_id;
  coakka_http_host_bytes_t method;
  coakka_http_host_bytes_t encoded_path_pattern;
  coakka_http_host_body_policy_t body_policy;
} coakka_http_host_route_t;

typedef struct coakka_http_host_header {
  coakka_http_host_bytes_t name;
  coakka_http_host_bytes_t value;
} coakka_http_host_header_t;

typedef uint32_t coakka_http_host_protocol_t;
#define COAKKA_HTTP_HOST_PROTOCOL_HTTP_1_1 UINT32_C(1)
#define COAKKA_HTTP_HOST_PROTOCOL_HTTP_2 UINT32_C(2)
#define COAKKA_HTTP_HOST_PROTOCOL_HTTP_3 UINT32_C(3)

typedef uint32_t coakka_http_host_security_t;
#define COAKKA_HTTP_HOST_SECURITY_PLAINTEXT UINT32_C(1)
#define COAKKA_HTTP_HOST_SECURITY_TLS UINT32_C(2)
#define COAKKA_HTTP_HOST_SECURITY_MUTUAL_TLS UINT32_C(3)

typedef uint32_t coakka_http_host_static_mount_flags_t;
#define COAKKA_HTTP_HOST_STATIC_SPA_FALLBACK UINT32_C(1)
#define COAKKA_HTTP_HOST_STATIC_INCLUDE_DOTFILES UINT32_C(2)

/* Immutable static-file snapshot declaration built before listener startup. */
typedef struct coakka_http_host_static_mount {
  uint32_t struct_size;
  coakka_http_host_static_mount_flags_t flags;
  coakka_http_host_bytes_t url_prefix;
  coakka_http_host_bytes_t root_path;
  coakka_http_host_bytes_t index_file;
  coakka_http_host_bytes_t spa_fallback_file;
  coakka_http_host_bytes_t cache_control;
} coakka_http_host_static_mount_t;

typedef uint32_t coakka_http_host_monitor_category_t;
#define COAKKA_HTTP_HOST_MONITOR_LIFECYCLE UINT32_C(1)
#define COAKKA_HTTP_HOST_MONITOR_EXCHANGE UINT32_C(2)
#define COAKKA_HTTP_HOST_MONITOR_CONTROL UINT32_C(4)
#define COAKKA_HTTP_HOST_MONITOR_LATENCY UINT32_C(8)
#define COAKKA_HTTP_HOST_MONITOR_ALL UINT32_C(15)

typedef uint32_t coakka_http_host_monitor_collection_t;
#define COAKKA_HTTP_HOST_MONITOR_DISABLED UINT32_C(0)
#define COAKKA_HTTP_HOST_MONITOR_AGGREGATES UINT32_C(1)
#define COAKKA_HTTP_HOST_MONITOR_AGGREGATES_AND_EVENTS UINT32_C(2)

typedef uint32_t coakka_http_host_monitor_notification_t;
#define COAKKA_HTTP_HOST_MONITOR_POLL UINT32_C(0)
#define COAKKA_HTTP_HOST_MONITOR_SIGNAL UINT32_C(1)

typedef uint32_t coakka_http_host_monitor_latency_t;
#define COAKKA_HTTP_HOST_MONITOR_LATENCY_NONE UINT32_C(0)
#define COAKKA_HTTP_HOST_MONITOR_LATENCY_FIXED UINT32_C(1)

typedef struct coakka_http_host_monitor_options {
  uint32_t struct_size;
  uint32_t event_capacity;
  uint32_t max_events_per_read;
  coakka_http_host_monitor_category_t categories;
  coakka_http_host_monitor_collection_t initial_collection;
  uint8_t signal_reserved;
  uint8_t fixed_latency_buckets;
  uint8_t reserved0[2];
} coakka_http_host_monitor_options_t;

typedef struct coakka_http_host_limits {
  uint32_t struct_size;
  uint32_t max_routes;
  uint32_t max_total_segments;
  uint32_t max_route_bytes;
  uint32_t max_captures_per_route;
  uint32_t activation_history_capacity;
  /* Remaining zero-valued fields select bounded library defaults. */
  uint32_t event_loop_threads;
  uint32_t max_connections;
  uint32_t max_active_requests;
  uint32_t request_queue_capacity;
  uint32_t completion_queue_capacity;
  uint32_t completion_batch_size;
  uint32_t max_header_count;
  uint32_t max_header_bytes;
  uint32_t request_stream_chunk_slots;
  uint32_t request_stream_max_retained_items;
  uint32_t response_stream_chunk_slots;
  uint32_t response_stream_max_retained_items;
  uint32_t route_control_capacity;
  uint32_t route_control_history_capacity;
  uint32_t route_control_max_commands_per_turn;
  uint32_t terminal_queue_capacity;
  uint32_t deadline_tick_ms;
  uint32_t connection_timeout_batch_size;
  uint64_t max_request_body_bytes;
  uint64_t max_response_body_bytes;
  uint64_t keep_alive_timeout_ms;
  uint64_t header_timeout_ms;
  uint64_t body_timeout_ms;
  uint64_t handler_timeout_ms;
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
} coakka_http_host_limits_t;

typedef struct coakka_http_host_compression {
  uint32_t struct_size;
  uint8_t enabled;
  uint8_t reserved0[3];
  /* Zero-valued limits and level select the library's bounded defaults. */
  uint64_t minimum_body_bytes;
  uint64_t maximum_body_bytes;
  uint64_t maximum_encoded_bytes;
  uint64_t workspace_bytes;
  int32_t gzip_level;
  uint32_t reserved1;
} coakka_http_host_compression_t;

typedef struct coakka_http_host_file_authority
    coakka_http_host_file_authority_t;
typedef struct coakka_http_host_outbound_target
    coakka_http_host_outbound_target_t;
typedef struct coakka_http_host_outbound_trust
    coakka_http_host_outbound_trust_t;
typedef struct coakka_http_host_outbound_identity
    coakka_http_host_outbound_identity_t;

typedef struct coakka_http_host_configuration {
  uint32_t struct_size;
  uint16_t port;
  uint16_t reserved0;
  coakka_http_host_protocol_t protocol;
  coakka_http_host_security_t security;
  coakka_http_host_bytes_t bind_address;
  uint64_t credential_generation;
  coakka_http_host_bytes_t credential_id;
  coakka_http_host_bytes_t certificate_chain_file;
  coakka_http_host_bytes_t private_key_file;
  coakka_http_host_bytes_t trust_roots_file;
  /* Opt-in only. Probing and the Linux platform fallback stay library-owned. */
  uint8_t use_io_uring;
  uint8_t reserved1[7];
  uint32_t static_mount_count;
  coakka_http_host_limits_t limits;
  coakka_http_host_monitor_options_t monitor;
  const coakka_http_host_static_mount_t *static_mounts;
  uint32_t max_total_static_assets;
  uint32_t file_authority_count;
  const coakka_http_host_file_authority_t *file_authorities;
  uint32_t outbound_target_count;
  const coakka_http_host_outbound_target_t *outbound_targets;
  uint32_t outbound_trust_count;
  const coakka_http_host_outbound_trust_t *outbound_trusts;
  uint32_t outbound_identity_count;
  const coakka_http_host_outbound_identity_t *outbound_identities;
  coakka_http_host_compression_t compression;
  uint32_t reserved2;
} coakka_http_host_configuration_t;

typedef struct coakka_http_host_service coakka_http_host_service_t;

typedef uint32_t coakka_http_host_io_fallback_reason_t;
#define COAKKA_HTTP_HOST_IO_FALLBACK_NONE UINT32_C(0)
#define COAKKA_HTTP_HOST_IO_FALLBACK_CONFIGURATION UINT32_C(1)
#define COAKKA_HTTP_HOST_IO_FALLBACK_UNAVAILABLE UINT32_C(2)

/*
 * Pointer-free native transport facts. requested/effective are reported
 * separately so an opted-in connector can observe a transparent fallback.
 * The probed field describes an actual eligible engine attempt; reading this
 * snapshot never performs an additional platform probe.
 */
typedef struct coakka_http_host_service_info {
  uint32_t struct_size;
  uint32_t abi_version;
  uint8_t io_uring_requested;
  uint8_t io_uring_effective;
  uint8_t io_uring_compiled;
  uint8_t io_uring_probed;
  uint8_t io_uring_supported;
  uint8_t service_started;
  uint8_t reserved0[2];
  coakka_http_host_io_fallback_reason_t io_fallback_reason;
  int32_t io_uring_probe_error;
  uint32_t reserved1;
} coakka_http_host_service_info_t;

typedef struct coakka_http_host_exchange {
  uint64_t slot;
  uint64_t generation;
} coakka_http_host_exchange_t;

typedef uint32_t coakka_http_host_request_body_delivery_t;
#define COAKKA_HTTP_HOST_REQUEST_BODY_INLINE UINT32_C(1)
#define COAKKA_HTTP_HOST_REQUEST_BODY_STREAM UINT32_C(2)

typedef uint32_t coakka_http_host_request_event_kind_t;
#define COAKKA_HTTP_HOST_REQUEST UINT32_C(1)
#define COAKKA_HTTP_HOST_REQUEST_DATA UINT32_C(2)
#define COAKKA_HTTP_HOST_REQUEST_TRAILERS UINT32_C(3)
#define COAKKA_HTTP_HOST_REQUEST_END UINT32_C(4)
#define COAKKA_HTTP_HOST_REQUEST_CANCELLED UINT32_C(5)
#define COAKKA_HTTP_HOST_RESPONSE_WRITABLE UINT32_C(6)
#define COAKKA_HTTP_HOST_REQUEST_TERMINAL UINT32_C(7)

/*
 * One native-owned event lease. Byte views remain valid until release. The
 * reserved storage is library-owned and connectors must preserve it exactly.
 */
typedef struct coakka_http_host_request_event {
  uint32_t struct_size;
  coakka_http_host_request_event_kind_t kind;
  coakka_http_host_exchange_t exchange;
  uint64_t sequence;
  uint64_t handler_binding_id;
  coakka_http_host_bytes_t data;
  uint64_t reserved_storage[16];
} coakka_http_host_request_event_t;

typedef struct coakka_http_host_path_parameter {
  coakka_http_host_bytes_t name;
  coakka_http_host_bytes_t encoded_value;
} coakka_http_host_path_parameter_t;

typedef struct coakka_http_host_query_parameter {
  coakka_http_host_bytes_t encoded_key;
  coakka_http_host_bytes_t encoded_value;
  uint8_t has_value;
  uint8_t reserved0[7];
} coakka_http_host_query_parameter_t;

typedef struct coakka_http_host_response {
  uint32_t struct_size;
  uint32_t status_code;
  const coakka_http_host_header_t *headers;
  uint32_t header_count;
  uint32_t reserved0;
  coakka_http_host_bytes_t body;
} coakka_http_host_response_t;

typedef struct coakka_http_host_file_authority {
  uint32_t struct_size;
  uint32_t max_active_files;
  uint64_t authority_id;
  coakka_http_host_bytes_t root_path;
  uint64_t max_file_bytes;
} coakka_http_host_file_authority_t;

typedef struct coakka_http_host_file_response {
  uint32_t struct_size;
  uint32_t reserved0;
  coakka_http_host_response_t head;
  uint64_t authority_id;
  coakka_http_host_bytes_t encoded_path;
} coakka_http_host_file_response_t;

typedef struct coakka_http_host_sse_event {
  uint32_t struct_size;
  uint32_t reserved0;
  coakka_http_host_bytes_t data;
  coakka_http_host_bytes_t event_type;
  coakka_http_host_bytes_t id;
  uint64_t retry_ms;
  uint8_t has_event_type;
  uint8_t has_id;
  uint8_t has_retry;
  uint8_t reserved1[5];
} coakka_http_host_sse_event_t;

typedef struct coakka_http_host_websocket {
  uint64_t slot;
  uint64_t generation;
} coakka_http_host_websocket_t;

typedef uint32_t coakka_http_host_websocket_event_kind_t;
#define COAKKA_HTTP_HOST_WEBSOCKET_OPEN UINT32_C(1)
#define COAKKA_HTTP_HOST_WEBSOCKET_TEXT UINT32_C(2)
#define COAKKA_HTTP_HOST_WEBSOCKET_BINARY UINT32_C(3)
#define COAKKA_HTTP_HOST_WEBSOCKET_PING UINT32_C(4)
#define COAKKA_HTTP_HOST_WEBSOCKET_PONG UINT32_C(5)
#define COAKKA_HTTP_HOST_WEBSOCKET_WRITABLE UINT32_C(6)
#define COAKKA_HTTP_HOST_WEBSOCKET_CLOSE UINT32_C(7)
#define COAKKA_HTTP_HOST_WEBSOCKET_ACCEPT_FAILED UINT32_C(8)

/* One native-owned WebSocket event lease; preserve reserved_storage. */
typedef struct coakka_http_host_websocket_event {
  uint32_t struct_size;
  coakka_http_host_websocket_event_kind_t kind;
  coakka_http_host_websocket_t session;
  coakka_http_host_exchange_t origin_exchange;
  uint32_t close_code;
  uint32_t reserved0;
  coakka_http_host_bytes_t data;
  uint64_t reserved_storage[12];
} coakka_http_host_websocket_event_t;

typedef struct coakka_http_host_outbound_endpoint {
  uint32_t struct_size;
  uint32_t weight;
  coakka_http_host_bytes_t node_id;
  coakka_http_host_bytes_t connect_host;
  uint16_t connect_port;
  uint16_t reserved0;
  coakka_http_host_bytes_t http_authority;
  coakka_http_host_security_t security;
  uint32_t available;
  coakka_http_host_bytes_t tls_peer_identity;
  uint64_t tls_trust_generation;
  uint64_t tls_client_identity_generation;
} coakka_http_host_outbound_endpoint_t;

typedef uint32_t coakka_http_host_outbound_strategy_t;
#define COAKKA_HTTP_HOST_OUTBOUND_SINGLE_OWNER UINT32_C(1)
#define COAKKA_HTTP_HOST_OUTBOUND_WEIGHTED_ROUND_ROBIN UINT32_C(2)

struct coakka_http_host_outbound_target {
  uint32_t struct_size;
  coakka_http_host_outbound_strategy_t strategy;
  coakka_http_host_bytes_t name;
  uint64_t generation;
  const coakka_http_host_outbound_endpoint_t *endpoints;
  uint32_t endpoint_count;
  uint32_t reserved0;
};

/* Immutable PEM material copied during service creation. */
struct coakka_http_host_outbound_trust {
  uint32_t struct_size;
  uint32_t reserved0;
  uint64_t generation;
  coakka_http_host_bytes_t ca_pem;
};

struct coakka_http_host_outbound_identity {
  uint32_t struct_size;
  uint32_t reserved0;
  uint64_t generation;
  coakka_http_host_bytes_t certificate_chain_pem;
  coakka_http_host_bytes_t private_key_pem;
};

typedef struct coakka_http_host_outbound_call {
  uint64_t slot;
  uint64_t generation;
} coakka_http_host_outbound_call_t;

typedef struct coakka_http_host_outbound_request {
  uint32_t struct_size;
  uint32_t timeout_ms;
  coakka_http_host_bytes_t logical_target;
  coakka_http_host_bytes_t method;
  coakka_http_host_bytes_t target;
  const coakka_http_host_header_t *headers;
  uint32_t header_count;
  uint32_t reserved0;
  coakka_http_host_bytes_t body;
} coakka_http_host_outbound_request_t;

typedef uint32_t coakka_http_host_outbound_reason_t;
#define COAKKA_HTTP_HOST_OUTBOUND_RESPONSE UINT32_C(1)
#define COAKKA_HTTP_HOST_OUTBOUND_PROVIDER_FAILURE UINT32_C(2)
#define COAKKA_HTTP_HOST_OUTBOUND_PROVIDER_REJECTED UINT32_C(3)
#define COAKKA_HTTP_HOST_OUTBOUND_CANCELLED UINT32_C(4)
#define COAKKA_HTTP_HOST_OUTBOUND_DEADLINE_EXCEEDED UINT32_C(5)
#define COAKKA_HTTP_HOST_OUTBOUND_STOPPED UINT32_C(6)
#define COAKKA_HTTP_HOST_OUTBOUND_RESPONSE_HEAD_LIMIT UINT32_C(7)
#define COAKKA_HTTP_HOST_OUTBOUND_RESPONSE_BODY_LIMIT UINT32_C(8)
#define COAKKA_HTTP_HOST_OUTBOUND_RESPONSE_AGGREGATE_LIMIT UINT32_C(9)
#define COAKKA_HTTP_HOST_OUTBOUND_OUT_OF_MEMORY UINT32_C(10)
#define COAKKA_HTTP_HOST_OUTBOUND_CLOCK_FAILURE UINT32_C(11)
#define COAKKA_HTTP_HOST_OUTBOUND_INVALID_PROVIDER_RESULT UINT32_C(12)

typedef uint32_t coakka_http_host_outbound_phase_t;
#define COAKKA_HTTP_HOST_OUTBOUND_PHASE_ADMISSION UINT32_C(1)
#define COAKKA_HTTP_HOST_OUTBOUND_PHASE_PROVIDER_ADOPTION UINT32_C(2)
#define COAKKA_HTTP_HOST_OUTBOUND_PHASE_TRANSPORT UINT32_C(3)
#define COAKKA_HTTP_HOST_OUTBOUND_PHASE_RESPONSE UINT32_C(4)
#define COAKKA_HTTP_HOST_OUTBOUND_PHASE_CANCELLATION UINT32_C(5)
#define COAKKA_HTTP_HOST_OUTBOUND_PHASE_SHUTDOWN UINT32_C(6)

typedef uint32_t coakka_http_host_outbound_retry_t;
#define COAKKA_HTTP_HOST_OUTBOUND_RETRY_NEVER UINT32_C(1)
#define COAKKA_HTTP_HOST_OUTBOUND_RETRY_IF_SAFE UINT32_C(2)
#define COAKKA_HTTP_HOST_OUTBOUND_RETRY_DELIVERY_UNCERTAIN UINT32_C(3)

typedef uint32_t coakka_http_host_outbound_certainty_t;
#define COAKKA_HTTP_HOST_OUTBOUND_NO_REQUEST_BYTES_SENT UINT32_C(1)
#define COAKKA_HTTP_HOST_OUTBOUND_REQUEST_UNCERTAIN UINT32_C(2)
#define COAKKA_HTTP_HOST_OUTBOUND_RESPONSE_HEAD_OBSERVED UINT32_C(3)
#define COAKKA_HTTP_HOST_OUTBOUND_COMPLETE_RESPONSE UINT32_C(4)

typedef struct coakka_http_host_outbound_event {
  uint32_t struct_size;
  coakka_http_host_outbound_reason_t reason;
  coakka_http_host_outbound_call_t call;
  coakka_http_host_bytes_t logical_target;
  uint64_t target_generation;
  coakka_http_host_bytes_t selected_node_id;
  coakka_http_host_outbound_phase_t phase;
  coakka_http_host_outbound_retry_t retry;
  coakka_http_host_outbound_certainty_t certainty;
  uint16_t response_status;
  uint16_t reserved0;
  int32_t provider_code;
  uint32_t response_header_count;
  coakka_http_host_bytes_t response_body;
  coakka_http_host_bytes_t diagnostic;
  uint64_t reserved_storage[20];
} coakka_http_host_outbound_event_t;

typedef struct coakka_http_host_failure {
  uint32_t struct_size;
  uint32_t reason;
  coakka_http_host_bytes_t detail;
} coakka_http_host_failure_t;

typedef uint32_t coakka_http_host_lifecycle_t;
#define COAKKA_HTTP_HOST_LIFECYCLE_CREATED UINT32_C(1)
#define COAKKA_HTTP_HOST_LIFECYCLE_RUNNING UINT32_C(2)
#define COAKKA_HTTP_HOST_LIFECYCLE_DRAINING UINT32_C(3)
#define COAKKA_HTTP_HOST_LIFECYCLE_STOPPED UINT32_C(4)

/* Host-owned gauges are copied only when a snapshot is requested. */
typedef struct coakka_http_host_activity {
  uint32_t struct_size;
  uint32_t active_requests;
  uint32_t pending_requests;
  uint32_t active_connections;
  uint32_t websocket_sessions;
  uint32_t outbound_pending;
  uint32_t reserved0;
} coakka_http_host_activity_t;

typedef struct coakka_http_host_health {
  uint32_t struct_size;
  coakka_http_host_lifecycle_t lifecycle;
  uint64_t snapshot_sequence;
  uint64_t observed_monotonic_ns;
  uint64_t progress_sequence;
  uint64_t progress_monotonic_ns;
  uint64_t acknowledged_probe_sequence;
  uint64_t acknowledged_probe_monotonic_ns;
  uint64_t route_generation;
  uint64_t binding_change_sequence;
  uint8_t ready;
  uint8_t admission_open;
  uint8_t reserved0[6];
  coakka_http_host_activity_t activity;
} coakka_http_host_health_t;

typedef struct coakka_http_host_route_identity {
  uint32_t struct_size;
  uint32_t route_count;
  uint64_t route_generation;
  uint64_t binding_change_sequence;
} coakka_http_host_route_identity_t;

typedef struct coakka_http_host_route_state {
  uint64_t route_id;
  uint64_t handler_binding_id;
  uint64_t binding_revision;
} coakka_http_host_route_state_t;

typedef uint32_t coakka_http_host_control_code_t;
#define COAKKA_HTTP_HOST_CONTROL_APPLIED UINT32_C(0)
#define COAKKA_HTTP_HOST_CONTROL_INVALID_ARGUMENT UINT32_C(1)
#define COAKKA_HTTP_HOST_CONTROL_CLOSED UINT32_C(2)
#define COAKKA_HTTP_HOST_CONTROL_ACTIVATION_CONFLICT UINT32_C(3)
#define COAKKA_HTTP_HOST_CONTROL_ACTIVATION_EXPIRED UINT32_C(4)
#define COAKKA_HTTP_HOST_CONTROL_ACTIVATION_OUT_OF_ORDER UINT32_C(5)
#define COAKKA_HTTP_HOST_CONTROL_ROUTE_NOT_FOUND UINT32_C(6)
#define COAKKA_HTTP_HOST_CONTROL_GENERATION_MISMATCH UINT32_C(7)
#define COAKKA_HTTP_HOST_CONTROL_REVISION_MISMATCH UINT32_C(8)
#define COAKKA_HTTP_HOST_CONTROL_SEQUENCE_EXHAUSTED UINT32_C(9)
#define COAKKA_HTTP_HOST_CONTROL_REJECTED UINT32_C(10)
#define COAKKA_HTTP_HOST_CONTROL_OUT_OF_MEMORY UINT32_C(11)
#define COAKKA_HTTP_HOST_CONTROL_INTERNAL UINT32_C(12)

typedef struct coakka_http_host_rebind_request {
  uint32_t struct_size;
  uint32_t reserved;
  uint64_t activation_id;
  uint64_t expected_route_generation;
  uint64_t route_id;
  uint64_t expected_binding_revision;
  uint64_t new_handler_binding_id;
} coakka_http_host_rebind_request_t;

typedef struct coakka_http_host_rebind_outcome {
  uint32_t struct_size;
  coakka_http_host_control_code_t code;
  uint64_t activation_id;
  uint64_t operation_digest;
  uint64_t route_generation;
  uint64_t binding_change_sequence;
  uint64_t route_id;
  uint64_t previous_handler_binding_id;
  uint64_t previous_binding_revision;
  uint64_t effective_handler_binding_id;
  uint64_t effective_binding_revision;
  uint8_t changed;
  uint8_t replayed;
  uint8_t reserved0[6];
} coakka_http_host_rebind_outcome_t;

typedef struct coakka_http_host_publication_request {
  uint32_t struct_size;
  uint32_t route_count;
  uint64_t activation_id;
  uint64_t expected_route_generation;
  uint64_t expected_binding_change_sequence;
  const coakka_http_host_route_t *routes;
} coakka_http_host_publication_request_t;

typedef struct coakka_http_host_publication_outcome {
  uint32_t struct_size;
  coakka_http_host_control_code_t code;
  uint64_t activation_id;
  uint64_t operation_digest;
  uint64_t previous_route_generation;
  uint64_t previous_binding_change_sequence;
  uint64_t effective_route_generation;
  uint64_t effective_binding_change_sequence;
  uint32_t route_count;
  uint32_t rejected_route_index;
  uint8_t changed;
  uint8_t replayed;
  uint8_t reserved0[6];
} coakka_http_host_publication_outcome_t;

typedef uint32_t coakka_http_host_monitor_event_kind_t;
#define COAKKA_HTTP_HOST_EVENT_LIFECYCLE_CHANGED UINT32_C(1)
#define COAKKA_HTTP_HOST_EVENT_POLICY_APPLIED UINT32_C(2)
#define COAKKA_HTTP_HOST_EVENT_POLICY_REJECTED UINT32_C(3)
#define COAKKA_HTTP_HOST_EVENT_EXCHANGE_COMPLETED UINT32_C(4)
#define COAKKA_HTTP_HOST_EVENT_EXCHANGE_FAILED UINT32_C(5)
#define COAKKA_HTTP_HOST_EVENT_EXCHANGE_TIMED_OUT UINT32_C(6)
#define COAKKA_HTTP_HOST_EVENT_EXCHANGE_DISCONNECTED UINT32_C(7)
#define COAKKA_HTTP_HOST_EVENT_EXCHANGE_CANCELLED UINT32_C(8)

typedef struct coakka_http_host_monitor_event {
  uint32_t struct_size;
  coakka_http_host_monitor_event_kind_t kind;
  uint64_t sequence;
  uint64_t change_sequence;
  uint64_t config_generation;
  uint64_t collection_epoch;
  uint64_t observed_monotonic_ns;
  coakka_http_host_monitor_category_t category;
  uint32_t failure_reason;
  uint32_t queue_scope;
  uint32_t queue_depth;
  uint32_t queue_capacity;
  coakka_http_host_exchange_t exchange;
} coakka_http_host_monitor_event_t;

typedef struct coakka_http_host_monitor_page {
  uint32_t struct_size;
  uint32_t count;
  uint64_t oldest_sequence;
  uint64_t latest_sequence;
  uint64_t overwritten_events;
  uint64_t remaining_events;
} coakka_http_host_monitor_page_t;

typedef struct coakka_http_host_monitor_policy {
  uint32_t struct_size;
  coakka_http_host_monitor_collection_t collection;
  coakka_http_host_monitor_notification_t notification;
  coakka_http_host_monitor_latency_t latency;
  uint32_t active_event_capacity;
  coakka_http_host_monitor_category_t aggregate_categories;
  coakka_http_host_monitor_category_t event_categories;
} coakka_http_host_monitor_policy_t;

typedef struct coakka_http_host_monitor_config {
  uint32_t struct_size;
  uint32_t reserved0;
  uint64_t generation;
  uint64_t collection_epoch;
  uint64_t change_sequence;
  uint64_t epoch_started_monotonic_ns;
  uint64_t policy_applied_monotonic_ns;
  coakka_http_host_monitor_category_t supported_categories;
  uint32_t reserved_event_capacity;
  uint32_t max_events_per_read;
  uint8_t signal_reserved;
  uint8_t fixed_latency_buckets;
  uint8_t reserved1[6];
  coakka_http_host_monitor_policy_t policy;
} coakka_http_host_monitor_config_t;

typedef uint32_t coakka_http_host_monitor_apply_reason_t;
#define COAKKA_HTTP_HOST_MONITOR_APPLIED UINT32_C(0)
#define COAKKA_HTTP_HOST_MONITOR_INVALID_ARGUMENT UINT32_C(1)
#define COAKKA_HTTP_HOST_MONITOR_INVALID_ENUM UINT32_C(2)
#define COAKKA_HTTP_HOST_MONITOR_UNSUPPORTED_CATEGORY UINT32_C(3)
#define COAKKA_HTTP_HOST_MONITOR_INVALID_CATEGORY_RELATIONSHIP UINT32_C(4)
#define COAKKA_HTTP_HOST_MONITOR_RESERVATION_EXCEEDED UINT32_C(5)
#define COAKKA_HTTP_HOST_MONITOR_SIGNAL_NOT_RESERVED UINT32_C(6)
#define COAKKA_HTTP_HOST_MONITOR_GENERATION_CONFLICT UINT32_C(7)
#define COAKKA_HTTP_HOST_MONITOR_INVALID_LIFECYCLE UINT32_C(8)
#define COAKKA_HTTP_HOST_MONITOR_SEQUENCE_EXHAUSTED UINT32_C(9)
#define COAKKA_HTTP_HOST_MONITOR_INTERNAL UINT32_C(10)

typedef struct coakka_http_host_monitor_apply_outcome {
  uint32_t struct_size;
  coakka_http_host_monitor_apply_reason_t reason;
  uint64_t actual;
  uint64_t limit;
  uint8_t changed;
  uint8_t reserved0[7];
  coakka_http_host_monitor_config_t effective;
} coakka_http_host_monitor_apply_outcome_t;

typedef struct coakka_http_host_monitor_failure_count {
  uint32_t reason;
  uint32_t reserved0;
  uint64_t count;
} coakka_http_host_monitor_failure_count_t;

typedef struct coakka_http_host_monitor_latency_bucket {
  uint64_t upper_bound_ns;
  uint64_t count;
} coakka_http_host_monitor_latency_bucket_t;

typedef struct coakka_http_host_monitor_snapshot {
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
  uint64_t admitted_requests;
  uint64_t request_bytes;
  uint64_t exchange_completed;
  uint64_t exchange_failed;
  uint64_t exchange_timed_out;
  uint64_t exchange_disconnected;
  uint64_t exchange_cancelled;
  uint64_t response_status_family[5];
  uint64_t retained_events;
  uint64_t overwritten_events;
  uint64_t dropped_events;
  uint64_t signal_notifications;
  uint64_t coalesced_signals;
  uint64_t signal_failures;
  coakka_http_host_monitor_config_t config;
  coakka_http_host_health_t health;
  coakka_http_host_monitor_failure_count_t
      failure_counts[COAKKA_HTTP_HOST_MAX_MONITOR_FAILURE_COUNTS];
  coakka_http_host_monitor_latency_bucket_t
      latency_buckets[COAKKA_HTTP_HOST_MONITOR_FIXED_LATENCY_BUCKETS];
} coakka_http_host_monitor_snapshot_t;

COAKKA_HTTP_HOST_API void coakka_http_host_service_plan_request_init(
    coakka_http_host_service_plan_request_t *request);
COAKKA_HTTP_HOST_API void
coakka_http_host_issue_init(coakka_http_host_issue_t *issue);
COAKKA_HTTP_HOST_API void
coakka_http_host_service_plan_init(coakka_http_host_service_plan_t *plan);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_plan_service(
    const coakka_http_host_service_plan_request_t *request,
    coakka_http_host_service_plan_t *out_plan);

COAKKA_HTTP_HOST_API void
coakka_http_host_route_init(coakka_http_host_route_t *route);
COAKKA_HTTP_HOST_API void
coakka_http_host_static_mount_init(coakka_http_host_static_mount_t *mount);
COAKKA_HTTP_HOST_API void
coakka_http_host_limits_init(coakka_http_host_limits_t *limits);
COAKKA_HTTP_HOST_API void coakka_http_host_monitor_options_init(
    coakka_http_host_monitor_options_t *options);
COAKKA_HTTP_HOST_API void coakka_http_host_configuration_init(
    coakka_http_host_configuration_t *configuration);
COAKKA_HTTP_HOST_API void
coakka_http_host_service_info_init(coakka_http_host_service_info_t *info);
COAKKA_HTTP_HOST_API void
coakka_http_host_request_event_init(coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API void
coakka_http_host_response_init(coakka_http_host_response_t *response);
COAKKA_HTTP_HOST_API void
coakka_http_host_body_policy_init(coakka_http_host_body_policy_t *policy);
COAKKA_HTTP_HOST_API void coakka_http_host_file_authority_init(
    coakka_http_host_file_authority_t *authority);
COAKKA_HTTP_HOST_API void
coakka_http_host_file_response_init(coakka_http_host_file_response_t *response);
COAKKA_HTTP_HOST_API void
coakka_http_host_compression_init(coakka_http_host_compression_t *compression);
COAKKA_HTTP_HOST_API void
coakka_http_host_sse_event_init(coakka_http_host_sse_event_t *event);
COAKKA_HTTP_HOST_API void coakka_http_host_websocket_event_init(
    coakka_http_host_websocket_event_t *event);
COAKKA_HTTP_HOST_API void coakka_http_host_outbound_endpoint_init(
    coakka_http_host_outbound_endpoint_t *endpoint);
COAKKA_HTTP_HOST_API void coakka_http_host_outbound_target_init(
    coakka_http_host_outbound_target_t *target);
COAKKA_HTTP_HOST_API void
coakka_http_host_outbound_trust_init(coakka_http_host_outbound_trust_t *trust);
COAKKA_HTTP_HOST_API void coakka_http_host_outbound_identity_init(
    coakka_http_host_outbound_identity_t *identity);
COAKKA_HTTP_HOST_API void coakka_http_host_outbound_request_init(
    coakka_http_host_outbound_request_t *request);
COAKKA_HTTP_HOST_API void
coakka_http_host_outbound_event_init(coakka_http_host_outbound_event_t *event);
COAKKA_HTTP_HOST_API void
coakka_http_host_failure_init(coakka_http_host_failure_t *failure);
COAKKA_HTTP_HOST_API void
coakka_http_host_activity_init(coakka_http_host_activity_t *activity);
COAKKA_HTTP_HOST_API void
coakka_http_host_health_init(coakka_http_host_health_t *health);
COAKKA_HTTP_HOST_API void coakka_http_host_route_identity_init(
    coakka_http_host_route_identity_t *identity);
COAKKA_HTTP_HOST_API void coakka_http_host_rebind_request_init(
    coakka_http_host_rebind_request_t *request);
COAKKA_HTTP_HOST_API void coakka_http_host_rebind_outcome_init(
    coakka_http_host_rebind_outcome_t *outcome);
COAKKA_HTTP_HOST_API void coakka_http_host_publication_request_init(
    coakka_http_host_publication_request_t *request);
COAKKA_HTTP_HOST_API void coakka_http_host_publication_outcome_init(
    coakka_http_host_publication_outcome_t *outcome);
COAKKA_HTTP_HOST_API void
coakka_http_host_monitor_event_init(coakka_http_host_monitor_event_t *event);
COAKKA_HTTP_HOST_API void
coakka_http_host_monitor_page_init(coakka_http_host_monitor_page_t *page);
COAKKA_HTTP_HOST_API void
coakka_http_host_monitor_policy_init(coakka_http_host_monitor_policy_t *policy);
COAKKA_HTTP_HOST_API void
coakka_http_host_monitor_config_init(coakka_http_host_monitor_config_t *config);
COAKKA_HTTP_HOST_API void coakka_http_host_monitor_apply_outcome_init(
    coakka_http_host_monitor_apply_outcome_t *outcome);
COAKKA_HTTP_HOST_API void coakka_http_host_monitor_snapshot_init(
    coakka_http_host_monitor_snapshot_t *snapshot);

/*
 * Create copies the complete route declaration. A zero route count requires a
 * null route pointer and represents a valid host with no application routes.
 * No partial owner is returned after validation, capacity or allocation
 * failure.
 */
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_service_create(
    const coakka_http_host_configuration_t *configuration,
    const coakka_http_host_route_t *routes, uint32_t route_count,
    coakka_http_host_service_t **out_service, uint32_t *failed_route_index);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_service_start(
    coakka_http_host_service_t *service, coakka_http_host_issue_t *out_issue);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_service_begin_drain(coakka_http_host_service_t *service);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_service_stop(coakka_http_host_service_t *service);
COAKKA_HTTP_HOST_API void
coakka_http_host_service_destroy(coakka_http_host_service_t **service);

/*
 * Native owns the listener, socket polling and protocol engine. Exactly one
 * connector reader calls take_request; it dispatches by handler_binding_id
 * and releases every successful lease after copying language-owned values.
 * A streamed request's REQUEST event is always delivered before DATA,
 * TRAILERS, END or CANCELLED for the same exchange, even when protocol input
 * makes a body event ready first. Connectors must preserve that event order.
 */
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_service_info(const coakka_http_host_service_t *service,
                              coakka_http_host_service_info_t *out_info);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_service_port(
    const coakka_http_host_service_t *service, uint16_t *out_port);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_take_request(
    coakka_http_host_service_t *service, uint64_t timeout_ms,
    coakka_http_host_request_event_t *out_event);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_release_request(coakka_http_host_service_t *service,
                                 coakka_http_host_request_event_t *event);
/* Wakes the sole request reader without stopping the service. */
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_interrupt_requests(coakka_http_host_service_t *service);

COAKKA_HTTP_HOST_API coakka_http_host_bytes_t
coakka_http_host_request_method(const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API coakka_http_host_bytes_t
coakka_http_host_request_scheme(const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API coakka_http_host_bytes_t
coakka_http_host_request_authority(
    const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API coakka_http_host_bytes_t
coakka_http_host_request_target(const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API coakka_http_host_bytes_t
coakka_http_host_request_body(const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API coakka_http_host_request_body_delivery_t
coakka_http_host_request_body_delivery(
    const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API uint64_t coakka_http_host_request_route_id(
    const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API uint64_t coakka_http_host_request_route_generation(
    const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API uint64_t coakka_http_host_request_binding_revision(
    const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API uint32_t coakka_http_host_request_header_count(
    const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API uint8_t coakka_http_host_request_header(
    const coakka_http_host_request_event_t *event, uint32_t index,
    coakka_http_host_header_t *out_header);
COAKKA_HTTP_HOST_API uint32_t coakka_http_host_request_path_parameter_count(
    const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API uint8_t coakka_http_host_request_path_parameter(
    const coakka_http_host_request_event_t *event, uint32_t index,
    coakka_http_host_path_parameter_t *out_parameter);
COAKKA_HTTP_HOST_API uint32_t coakka_http_host_request_query_parameter_count(
    const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API uint8_t coakka_http_host_request_query_parameter(
    const coakka_http_host_request_event_t *event, uint32_t index,
    coakka_http_host_query_parameter_t *out_parameter);
COAKKA_HTTP_HOST_API uint32_t coakka_http_host_request_trailer_count(
    const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API uint8_t coakka_http_host_request_trailer(
    const coakka_http_host_request_event_t *event, uint32_t index,
    coakka_http_host_header_t *out_trailer);
COAKKA_HTTP_HOST_API uint32_t coakka_http_host_request_stream_trailer_count(
    const coakka_http_host_request_event_t *event);
COAKKA_HTTP_HOST_API uint8_t coakka_http_host_request_stream_trailer(
    const coakka_http_host_request_event_t *event, uint32_t index,
    coakka_http_host_header_t *out_trailer);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_request_cancelled(coakka_http_host_service_t *service,
                                   coakka_http_host_exchange_t exchange,
                                   uint8_t *out_cancelled);

/* Successful completion calls copy all borrowed response/failure bytes. */
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_respond(
    coakka_http_host_service_t *service, coakka_http_host_exchange_t exchange,
    const coakka_http_host_response_t *response);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_fail(
    coakka_http_host_service_t *service, coakka_http_host_exchange_t exchange,
    const coakka_http_host_failure_t *failure);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_respond_file(
    coakka_http_host_service_t *service, coakka_http_host_exchange_t exchange,
    const coakka_http_host_file_response_t *response);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_start_response_stream(coakka_http_host_service_t *service,
                                       coakka_http_host_exchange_t exchange,
                                       const coakka_http_host_response_t *head);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_write_response_stream(coakka_http_host_service_t *service,
                                       coakka_http_host_exchange_t exchange,
                                       coakka_http_host_bytes_t data);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_finish_response_stream(
    coakka_http_host_service_t *service, coakka_http_host_exchange_t exchange,
    const coakka_http_host_header_t *trailers, uint32_t trailer_count);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_start_sse(
    coakka_http_host_service_t *service, coakka_http_host_exchange_t exchange,
    const coakka_http_host_header_t *headers, uint32_t header_count);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_write_sse(
    coakka_http_host_service_t *service, coakka_http_host_exchange_t exchange,
    const coakka_http_host_sse_event_t *event);

COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_accept_websocket(
    coakka_http_host_service_t *service, coakka_http_host_exchange_t exchange,
    coakka_http_host_bytes_t selected_subprotocol);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_take_websocket(
    coakka_http_host_service_t *service, uint64_t timeout_ms,
    coakka_http_host_websocket_event_t *out_event);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_release_websocket(coakka_http_host_service_t *service,
                                   coakka_http_host_websocket_event_t *event);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_send_websocket(
    coakka_http_host_service_t *service, coakka_http_host_websocket_t session,
    coakka_http_host_websocket_event_kind_t kind,
    coakka_http_host_bytes_t data);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_close_websocket(
    coakka_http_host_service_t *service, coakka_http_host_websocket_t session,
    uint32_t code, coakka_http_host_bytes_t reason);

COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_submit_outbound(
    coakka_http_host_service_t *service,
    const coakka_http_host_outbound_request_t *request,
    coakka_http_host_outbound_call_t *out_call);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_cancel_outbound(
    coakka_http_host_service_t *service, coakka_http_host_outbound_call_t call);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_interrupt_outbound(coakka_http_host_service_t *service);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_take_outbound(
    coakka_http_host_service_t *service, uint64_t timeout_ms,
    coakka_http_host_outbound_event_t *out_event);
COAKKA_HTTP_HOST_API uint8_t coakka_http_host_outbound_header(
    coakka_http_host_service_t *service,
    const coakka_http_host_outbound_event_t *event, uint32_t index,
    coakka_http_host_header_t *out_header);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_release_outbound(coakka_http_host_service_t *service,
                                  coakka_http_host_outbound_event_t *event);

COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_service_health(
    coakka_http_host_service_t *service, coakka_http_host_health_t *out_health);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_probe_liveness(
    coakka_http_host_service_t *service, uint64_t timeout_ms,
    coakka_http_host_health_t *out_health);

COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_route_snapshot(
    coakka_http_host_service_t *service, coakka_http_host_route_state_t *routes,
    uint32_t route_capacity, coakka_http_host_route_identity_t *out_identity);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_rebind(coakka_http_host_service_t *service,
                        const coakka_http_host_rebind_request_t *request,
                        coakka_http_host_rebind_outcome_t *out_outcome);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_publish_routes(
    coakka_http_host_service_t *service,
    const coakka_http_host_publication_request_t *request,
    coakka_http_host_publication_outcome_t *out_outcome);

COAKKA_HTTP_HOST_API uint8_t
coakka_http_host_monitor_enabled(coakka_http_host_service_t *service);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_monitor_config(coakka_http_host_service_t *service,
                                coakka_http_host_monitor_config_t *out_config);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_monitor_apply(
    coakka_http_host_service_t *service, uint64_t expected_generation,
    const coakka_http_host_monitor_policy_t *policy,
    coakka_http_host_monitor_apply_outcome_t *out_outcome);
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_monitor_read(
    coakka_http_host_service_t *service, uint64_t after_sequence,
    uint32_t maximum_events, coakka_http_host_monitor_event_t *events,
    uint32_t event_capacity, coakka_http_host_monitor_page_t *out_page);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_monitor_snapshot(
    coakka_http_host_service_t *service,
    coakka_http_host_monitor_snapshot_t *out_snapshot);
/* Exactly one thread may wait. Interrupt is safe from another thread. */
COAKKA_HTTP_HOST_API coakka_http_host_status_t coakka_http_host_monitor_wait(
    coakka_http_host_service_t *service, uint64_t timeout_ms);
COAKKA_HTTP_HOST_API coakka_http_host_status_t
coakka_http_host_monitor_interrupt(coakka_http_host_service_t *service);

COAKKA_HTTP_HOST_API uint32_t coakka_http_host_abi_version(void);
COAKKA_HTTP_HOST_API const char *
coakka_http_host_status_name(coakka_http_host_status_t status);
COAKKA_HTTP_HOST_API const char *
coakka_http_host_control_code_name(coakka_http_host_control_code_t code);

#ifdef __cplusplus
}
#endif

#endif
