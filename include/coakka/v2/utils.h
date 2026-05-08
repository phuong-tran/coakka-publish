#ifndef COAKKA_V2_UTILS_H
#define COAKKA_V2_UTILS_H

#include <stddef.h>
#include <stdint.h>

#include "coakka/v2/control.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *coakka_v2_status_name(coakka_v2_status_t status);

const char *coakka_v2_runtime_state_name(coakka_v2_runtime_state_t state);

const char *coakka_v2_overload_mode_name(uint32_t mode);

const char *coakka_v2_route_strategy_name(coakka_v2_route_resolution_strategy_t strategy);

/*
 * Legacy monitor enum name helpers. They describe reserved source-compatible
 * names only; monitor_read_fd is a doorbell and does not emit typed event
 * records.
 */
const char *coakka_v2_monitor_event_kind_name(uint32_t kind);

const char *coakka_v2_monitor_queue_scope_name(uint32_t scope);

const char *coakka_v2_runtime_feature_flag_name(uint32_t flag);

const char *coakka_v2_health_flag_name(uint32_t flag);

const char *coakka_v2_route_flag_name(uint32_t flag);

const char *coakka_v2_endpoint_flag_name(uint32_t flag);

size_t coakka_v2_format_runtime_feature_flags(uint32_t flags, char *buf, size_t buf_len);

size_t coakka_v2_format_health_flags(uint32_t flags, char *buf, size_t buf_len);

#ifdef __cplusplus
}
#endif

#endif
