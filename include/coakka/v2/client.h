#ifndef COAKKA_V2_CLIENT_H
#define COAKKA_V2_CLIENT_H

#include <stddef.h>
#include <stdint.h>

#include "coakka/v2/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct coakka_v2_ask_client_t coakka_v2_ask_client_t;
typedef struct coakka_v2_ask_ticket_t coakka_v2_ask_ticket_t;

typedef enum coakka_v2_client_result_kind_t {
    COAKKA_V2_CLIENT_RESULT_NONE = 0,
    COAKKA_V2_CLIENT_RESULT_RESPONSE = 1,
    COAKKA_V2_CLIENT_RESULT_DEADLETTER = 2
} coakka_v2_client_result_kind_t;

typedef enum coakka_v2_client_delivery_hint_t {
    COAKKA_V2_CLIENT_DELIVERY_HINT_UNSPECIFIED = 0,
    COAKKA_V2_CLIENT_DELIVERY_HINT_ROUTER_DEFAULT = 1,
    COAKKA_V2_CLIENT_DELIVERY_HINT_PREFER_LOCAL = 2,
    COAKKA_V2_CLIENT_DELIVERY_HINT_REQUIRE_LOCAL = 3,
    COAKKA_V2_CLIENT_DELIVERY_HINT_REQUIRE_REMOTE = 4
} coakka_v2_client_delivery_hint_t;

typedef struct coakka_v2_client_raw_request_spec_t {
    size_t struct_size;
    const char *message_id;
    const char *source;
    const char *target;
    const char *reply_to;
    const uint8_t *payload;
    size_t payload_len;
    uint32_t timeout_ms;
    uint32_t delivery_hint;
    uint32_t one_way;
} coakka_v2_client_raw_request_spec_t;

typedef struct coakka_v2_client_raw_reply_spec_t {
    size_t struct_size;
    const uint8_t *request_buf;
    size_t request_len;
    const char *source;
    const uint8_t *payload;
    size_t payload_len;
} coakka_v2_client_raw_reply_spec_t;

coakka_v2_ask_client_t *coakka_v2_ask_client_create(
    coakka_v2_runtime_t *rt,
    const coakka_v2_host_handles_t *handles
);

void coakka_v2_ask_client_destroy(coakka_v2_ask_client_t *client);

coakka_v2_status_t coakka_v2_ask_client_begin(
    coakka_v2_ask_client_t *client,
    const uint8_t *request_buf,
    size_t request_len,
    coakka_v2_ask_ticket_t **out_ticket
);

coakka_v2_status_t coakka_v2_ask_ticket_await(
    coakka_v2_ask_ticket_t *ticket,
    uint32_t timeout_ms,
    uint32_t *out_result_kind,
    uint8_t **out_buf,
    size_t *out_len
);

coakka_v2_status_t coakka_v2_ask_ticket_poll(
    coakka_v2_ask_ticket_t *ticket,
    uint32_t *out_result_kind,
    uint8_t **out_buf,
    size_t *out_len
);

void coakka_v2_ask_ticket_cancel(coakka_v2_ask_ticket_t *ticket);

const char *coakka_v2_ask_ticket_message_id(const coakka_v2_ask_ticket_t *ticket);

void coakka_v2_ask_ticket_destroy(coakka_v2_ask_ticket_t *ticket);

coakka_v2_status_t coakka_v2_client_build_raw_request(
    const coakka_v2_client_raw_request_spec_t *spec,
    uint8_t **out_buf,
    size_t *out_len
);

coakka_v2_status_t coakka_v2_client_build_raw_reply(
    const coakka_v2_client_raw_reply_spec_t *spec,
    uint8_t **out_buf,
    size_t *out_len
);

void coakka_v2_client_bytes_release(uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif
