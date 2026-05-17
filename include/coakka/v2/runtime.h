#ifndef COAKKA_V2_RUNTIME_H
#define COAKKA_V2_RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum coakka_v2_status_t {
    COAKKA_V2_OK = 0,
    COAKKA_V2_ERR_INVALID_ARG = -1,
    COAKKA_V2_ERR_NOMEM = -2,
    COAKKA_V2_ERR_BAD_STATE = -3,
    COAKKA_V2_ERR_STALE_GENERATION = -4,
    COAKKA_V2_ERR_IO = -5,
    COAKKA_V2_ERR_WOULD_BLOCK = -6,
    COAKKA_V2_ERR_CLOSED = -7
} coakka_v2_status_t;

typedef enum coakka_v2_runtime_state_t {
    COAKKA_V2_STATE_CREATED = 0,
    COAKKA_V2_STATE_STARTED = 1,
    COAKKA_V2_STATE_STOPPED = 2
} coakka_v2_runtime_state_t;

enum {
    COAKKA_V2_HEALTH_PROCESS_ALIVE = 1u << 0,
    COAKKA_V2_HEALTH_RUNTIME_STARTED = 1u << 1,
    COAKKA_V2_HEALTH_CONTROL_SNAPSHOT_PRESENT = 1u << 2,
    COAKKA_V2_HEALTH_DATAPLANE_READY = 1u << 3,
    COAKKA_V2_HEALTH_TRANSPORT_PROBE_ONLY = 1u << 4,
    COAKKA_V2_HEALTH_REMOTE_OUTBOUND_SATURATED = 1u << 5,
    COAKKA_V2_HEALTH_DRAINED_ROUTE_PRESENT = 1u << 6
};

typedef struct coakka_v2_runtime_config_t {
    const char *system_name;
    const char *node_id;
    int strict_no_drop;
    int queue_capacity;
} coakka_v2_runtime_config_t;

typedef enum coakka_v2_overload_mode_t {
    COAKKA_V2_OVERLOAD_MODE_REJECT = 0,
    COAKKA_V2_OVERLOAD_MODE_DROP_EXPIRED_FIRST = 1,
    COAKKA_V2_OVERLOAD_MODE_DROP_ONE_WAY_FIRST = 2
} coakka_v2_overload_mode_t;

typedef struct coakka_v2_overload_policy_t {
    size_t struct_size;
    size_t remote_outbound_reply_reserve_slots;
    uint32_t ingress_mode;
    uint32_t local_delivery_mode;
    uint32_t remote_outbound_mode;
} coakka_v2_overload_policy_t;

typedef struct coakka_v2_runtime_stats_t {
    size_t struct_size;
    uint64_t applied_generation;
    size_t route_count;
    coakka_v2_runtime_state_t runtime_state;
    size_t ingress_queue_capacity;
    size_t ingress_queue_depth;
    size_t ingress_queue_high_watermark;
    uint64_t queue_rejected_count;
    uint64_t route_miss_count;
    uint64_t deadletter_count;
    uint64_t delivery_failed_count;
    uint64_t remote_transport_failed_count;
    uint64_t transport_submit_attempt_count;
    uint64_t transport_probe_connect_success_count;
    uint64_t transport_probe_connect_failure_count;
    size_t request_max_frame_size;
    size_t local_dispatch_batch_limit;
    size_t delivered_request_outbound_queue_capacity;
    size_t delivered_request_outbound_queue_depth;
    size_t delivered_request_outbound_queue_high_watermark;
    uint64_t delivered_request_outbound_enqueue_block_count;
    size_t response_outbound_queue_capacity;
    size_t response_outbound_queue_depth;
    size_t response_outbound_queue_high_watermark;
    uint64_t response_outbound_enqueue_block_count;
    size_t deadletter_outbound_queue_capacity;
    size_t deadletter_outbound_queue_depth;
    size_t deadletter_outbound_queue_high_watermark;
    uint64_t deadletter_outbound_enqueue_block_count;
    uint64_t remote_reply_timeout_count;
    uint64_t late_remote_reply_drop_count;
    size_t remote_outbound_queue_capacity;
    size_t remote_outbound_queue_depth;
    size_t remote_outbound_queue_high_watermark;
    uint64_t remote_outbound_queue_rejected_count;
    uint64_t remote_outbound_expired_drop_count;
    uint64_t endpoint_unavailable_count;
    uint64_t remote_response_forwarded_count;
    uint64_t remote_deadletter_forwarded_count;
    size_t drained_route_count;
    uint64_t control_rejected_count;
    uint64_t control_invalid_count;
    uint64_t control_stale_generation_count;
    uint64_t control_bad_state_count;
    uint64_t control_io_count;
    size_t remote_outbound_reply_reserve_slots;
    uint64_t remote_outbound_reply_reservation_reject_count;
    uint32_t ingress_overload_mode;
    uint32_t local_delivery_overload_mode;
    uint32_t remote_outbound_overload_mode;
    uint64_t monitor_event_emitted_count;
    uint64_t monitor_event_dropped_count;
    uint64_t monitor_event_emitted_lifetime_count;
    uint64_t monitor_event_dropped_lifetime_count;
    size_t local_work_queue_capacity;
    size_t local_work_queue_depth;
    size_t local_work_queue_high_watermark;
    uint64_t delivered_request_outbound_direct_write_count;
    uint64_t response_outbound_direct_write_count;
    uint64_t deadletter_outbound_direct_write_count;
    uint64_t remote_outbound_one_way_drop_count;
    uint64_t remote_failover_attempt_count;
    uint64_t remote_failover_success_count;
    uint64_t remote_failover_exhausted_count;
} coakka_v2_runtime_stats_t;

typedef struct coakka_v2_host_handles_t {
    size_t struct_size;
    uint32_t flags;
    int request_write_fd;
    int response_read_fd;
    int deadletter_read_fd;
    int control_write_fd;
    int monitor_read_fd;
    int delivered_request_read_fd;
} coakka_v2_host_handles_t;

enum {
    COAKKA_V2_HOST_HANDLES_FLAG_ENABLE_MONITOR = 1u << 0,
    COAKKA_V2_HOST_HANDLES_FLAG_SEPARATE_DELIVERED_REQUEST_LANE = 1u << 1
};

/*
 * Legacy typed monitor names are reserved for source compatibility only.
 *
 * The phase-2 monitor lane is a doorbell: runtime rings monitor_read_fd, the
 * host calls coakka_v2_monitor_consume(...), then refreshes stats/health through
 * direct ABI calls. No public API currently emits coakka_v2_monitor_event_t
 * frames, and connectors must not treat these enum values as an event-stream
 * wire contract.
 */
typedef enum coakka_v2_monitor_event_kind_t {
    COAKKA_V2_MONITOR_EVENT_KIND_STARTED = 1,
    COAKKA_V2_MONITOR_EVENT_KIND_STOPPED = 2,
    COAKKA_V2_MONITOR_EVENT_KIND_CONTROL_APPLIED = 3,
    COAKKA_V2_MONITOR_EVENT_KIND_CONTROL_REJECTED = 4,
    COAKKA_V2_MONITOR_EVENT_KIND_QUEUE_REJECTED = 5,
    COAKKA_V2_MONITOR_EVENT_KIND_ENDPOINT_UNAVAILABLE = 6,
    COAKKA_V2_MONITOR_EVENT_KIND_REMOTE_RESPONSE_FORWARDED = 7,
    COAKKA_V2_MONITOR_EVENT_KIND_REMOTE_DEADLETTER_FORWARDED = 8,
    COAKKA_V2_MONITOR_EVENT_KIND_REMOTE_REPLY_TIMEOUT = 9,
    COAKKA_V2_MONITOR_EVENT_KIND_LATE_REMOTE_REPLY_DROPPED = 10
} coakka_v2_monitor_event_kind_t;

typedef enum coakka_v2_monitor_queue_scope_t {
    COAKKA_V2_MONITOR_QUEUE_SCOPE_INGRESS = 1,
    COAKKA_V2_MONITOR_QUEUE_SCOPE_LOCAL_DELIVERY = 2,
    COAKKA_V2_MONITOR_QUEUE_SCOPE_REMOTE_OUTBOUND = 3
} coakka_v2_monitor_queue_scope_t;

/*
 * Reserved legacy event payload shape. Kept in the public header so older
 * source that referenced it still compiles, but monitor_read_fd carries only a
 * doorbell signal, not serialized coakka_v2_monitor_event_t records.
 */
typedef struct coakka_v2_monitor_event_t {
    size_t struct_size;
    uint32_t kind;
    uint32_t flags;
    uint64_t monotonic_time_ns;
    uint64_t generation;
    int64_t code;
    uint64_t value;
} coakka_v2_monitor_event_t;

typedef struct coakka_v2_runtime_health_t {
    size_t struct_size;
    coakka_v2_runtime_state_t runtime_state;
    uint32_t flags;
    uint64_t applied_generation;
} coakka_v2_runtime_health_t;

enum {
    COAKKA_V2_RUNTIME_FEATURE_REQUEST_PIPE = 1u << 0,
    COAKKA_V2_RUNTIME_FEATURE_CONTROL_PIPE = 1u << 1,
    COAKKA_V2_RUNTIME_FEATURE_MONITOR = 1u << 2,
    COAKKA_V2_RUNTIME_FEATURE_NATIVE_SUBMIT = 1u << 3,
    COAKKA_V2_RUNTIME_FEATURE_CONTROL_JSON = 1u << 4,
    COAKKA_V2_RUNTIME_FEATURE_REMOTE_TRANSPORT = 1u << 5,
    COAKKA_V2_RUNTIME_FEATURE_JEMALLOC = 1u << 6,
    COAKKA_V2_RUNTIME_FEATURE_DELIVERED_REQUEST_PIPE = 1u << 7
};

typedef struct coakka_v2_runtime_info_t {
    size_t struct_size;
    uint32_t abi_version;
    uint32_t feature_flags;
    const char *runtime_version;
    const char *git_commit;
    const char *transport_provider;
    const char *allocator_provider;
    const char *docs_hint;
    const char *transport_profile;
    uint32_t transport_profile_version;
} coakka_v2_runtime_info_t;

typedef struct coakka_v2_runtime_config_view_t {
    size_t struct_size;
    const char *system_name;
    const char *node_id;
    int strict_no_drop;
    int queue_capacity;
    size_t request_max_frame_size;
    size_t local_dispatch_batch_limit;
    coakka_v2_runtime_state_t runtime_state;
    uint32_t snapshot_present;
    uint64_t applied_generation;
    size_t route_count;
    const char *transport_bind_host;
    uint16_t transport_bind_port;
    uint32_t configured_ingress_overload_mode;
    uint32_t configured_local_delivery_overload_mode;
    uint32_t configured_remote_outbound_overload_mode;
    size_t configured_remote_outbound_reply_reserve_slots;
    uint32_t effective_ingress_overload_mode;
    uint32_t effective_local_delivery_overload_mode;
    uint32_t effective_remote_outbound_overload_mode;
    size_t effective_remote_outbound_reply_reserve_slots;
} coakka_v2_runtime_config_view_t;

typedef struct coakka_v2_runtime_t coakka_v2_runtime_t;

uint32_t coakka_v2_runtime_get_abi_version(void);

coakka_v2_status_t coakka_v2_runtime_get_info(coakka_v2_runtime_info_t *out_info);

coakka_v2_status_t coakka_v2_runtime_get_config(coakka_v2_runtime_t *rt,
                                                coakka_v2_runtime_config_view_t *out_config);

coakka_v2_runtime_t *coakka_v2_runtime_create(const coakka_v2_runtime_config_t *cfg);

void coakka_v2_runtime_destroy(coakka_v2_runtime_t *rt);

coakka_v2_status_t coakka_v2_runtime_get_host_handles(coakka_v2_runtime_t *rt,
                                                      coakka_v2_host_handles_t *out_handles);

coakka_v2_status_t coakka_v2_runtime_start(coakka_v2_runtime_t *rt);

coakka_v2_status_t coakka_v2_runtime_stop(coakka_v2_runtime_t *rt);

coakka_v2_status_t coakka_v2_runtime_apply_overload_policy(
    coakka_v2_runtime_t *rt,
    const coakka_v2_overload_policy_t *policy
);

coakka_v2_status_t coakka_v2_runtime_get_stats(coakka_v2_runtime_t *rt,
                                               coakka_v2_runtime_stats_t *out_stats);

coakka_v2_status_t coakka_v2_runtime_get_health(coakka_v2_runtime_t *rt,
                                                coakka_v2_runtime_health_t *out_health);

coakka_v2_status_t coakka_v2_runtime_submit_envelope(coakka_v2_runtime_t *rt,
                                                     const uint8_t *buf,
                                                     size_t len);

coakka_v2_status_t coakka_v2_runtime_apply_control_envelope(coakka_v2_runtime_t *rt,
                                                            const uint8_t *buf,
                                                            size_t len);

#ifdef __cplusplus
}
#endif

#endif
