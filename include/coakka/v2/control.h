#ifndef COAKKA_V2_CONTROL_H
#define COAKKA_V2_CONTROL_H

#include <stddef.h>
#include <stdint.h>

#include "coakka/v2/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct coakka_v2_endpoint_t {
    const char *host;
    uint16_t port;
    uint32_t weight;
    uint32_t flags;
} coakka_v2_endpoint_t;

typedef enum coakka_v2_route_resolution_strategy_t {
    COAKKA_V2_ROUTE_STRATEGY_UNSPECIFIED = 0,
    COAKKA_V2_ROUTE_STRATEGY_SINGLE_OWNER = 1,
    COAKKA_V2_ROUTE_STRATEGY_WEIGHTED_ROUND_ROBIN = 2,
    COAKKA_V2_ROUTE_STRATEGY_RENDEZVOUS_HASH = 3
} coakka_v2_route_resolution_strategy_t;

enum {
    COAKKA_V2_ROUTE_FLAG_NONE = 0u,
    COAKKA_V2_ROUTE_FLAG_PREFER_LOCAL = 1u << 0
};

enum {
    COAKKA_V2_ENDPOINT_FLAG_NONE = 0u,
    COAKKA_V2_ENDPOINT_FLAG_LOCAL = 1u << 0,
    COAKKA_V2_ENDPOINT_FLAG_UNAVAILABLE = 1u << 1
};

typedef struct coakka_v2_route_t {
    const char *target;
    coakka_v2_route_resolution_strategy_t strategy;
    const char *route_key_hint;
    uint32_t flags;
    const coakka_v2_endpoint_t *endpoints;
    size_t endpoint_count;
} coakka_v2_route_t;

typedef struct coakka_v2_control_snapshot_t {
    uint64_t generation;
    const coakka_v2_route_t *routes;
    size_t route_count;
} coakka_v2_control_snapshot_t;

coakka_v2_status_t coakka_v2_runtime_apply_control_snapshot(
    coakka_v2_runtime_t *rt,
    const coakka_v2_control_snapshot_t *snapshot
);

coakka_v2_status_t coakka_v2_runtime_apply_control_snapshot_with_policy(
    coakka_v2_runtime_t *rt,
    const coakka_v2_control_snapshot_t *snapshot,
    const coakka_v2_overload_policy_t *policy
);

#ifdef __cplusplus
}
#endif

#endif
