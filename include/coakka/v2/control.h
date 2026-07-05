#ifndef COAKKA_V2_CONTROL_H
#define COAKKA_V2_CONTROL_H

#include <stddef.h>
#include <stdint.h>

#include "coakka/v2/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Endpoint metadata inside a runtime route snapshot.
 *
 * For endpoints marked COAKKA_V2_ENDPOINT_FLAG_LOCAL, host/port are diagnostic
 * metadata only; the runtime does not bind or listen on that socket.
 */
typedef struct coakka_v2_endpoint_t {
    const char *host;
    uint16_t port;
    uint32_t weight;
    uint32_t flags;
} coakka_v2_endpoint_t;

/** Route resolution policy used when a target has one or more endpoints. */
typedef enum coakka_v2_route_resolution_strategy_t {
    COAKKA_V2_ROUTE_STRATEGY_UNSPECIFIED = 0,
    COAKKA_V2_ROUTE_STRATEGY_SINGLE_OWNER = 1,
    COAKKA_V2_ROUTE_STRATEGY_WEIGHTED_ROUND_ROBIN = 2,
    COAKKA_V2_ROUTE_STRATEGY_RENDEZVOUS_HASH = 3
} coakka_v2_route_resolution_strategy_t;

enum {
    /** No route-level flags. */
    COAKKA_V2_ROUTE_FLAG_NONE = 0u,
    /** Prefer local endpoints when the route also has remote candidates. */
    COAKKA_V2_ROUTE_FLAG_PREFER_LOCAL = 1u << 0
};

enum {
    /** No endpoint flags. */
    COAKKA_V2_ENDPOINT_FLAG_NONE = 0u,
    /** Endpoint is owned by this runtime process. */
    COAKKA_V2_ENDPOINT_FLAG_LOCAL = 1u << 0,
    /** Endpoint is present for diagnostics but unavailable for routing. */
    COAKKA_V2_ENDPOINT_FLAG_UNAVAILABLE = 1u << 1
};

/**
 * Target-to-endpoints route-table entry.
 *
 * The caller owns every string and endpoint array for the duration of the
 * apply call. The runtime copies the snapshot before returning.
 */
typedef struct coakka_v2_route_t {
    const char *target;
    coakka_v2_route_resolution_strategy_t strategy;
    const char *route_key_hint;
    uint32_t flags;
    const coakka_v2_endpoint_t *endpoints;
    size_t endpoint_count;
} coakka_v2_route_t;

/**
 * Complete route snapshot applied under one monotonically increasing generation.
 *
 * Generation 0 is invalid. Newer snapshots replace older route state; stale
 * generations are rejected to keep host control-plane updates explicit.
 */
typedef struct coakka_v2_control_snapshot_t {
    uint64_t generation;
    const coakka_v2_route_t *routes;
    size_t route_count;
} coakka_v2_control_snapshot_t;

/** Applies a route snapshot with the runtime's current overload policy. */
coakka_v2_status_t coakka_v2_runtime_apply_control_snapshot(
    coakka_v2_runtime_t *rt,
    const coakka_v2_control_snapshot_t *snapshot
);

/** Applies a route snapshot and overload policy as one control-plane update. */
coakka_v2_status_t coakka_v2_runtime_apply_control_snapshot_with_policy(
    coakka_v2_runtime_t *rt,
    const coakka_v2_control_snapshot_t *snapshot,
    const coakka_v2_overload_policy_t *policy
);

#ifdef __cplusplus
}
#endif

#endif
