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

/** Result kind returned by a completed ask ticket. */
typedef enum coakka_v2_client_result_kind_t {
    COAKKA_V2_CLIENT_RESULT_NONE = 0,
    COAKKA_V2_CLIENT_RESULT_RESPONSE = 1,
    COAKKA_V2_CLIENT_RESULT_DEADLETTER = 2
} coakka_v2_client_result_kind_t;

/** Delivery preference encoded into a connector-built request envelope. */
typedef enum coakka_v2_client_delivery_hint_t {
    COAKKA_V2_CLIENT_DELIVERY_HINT_UNSPECIFIED = 0,
    COAKKA_V2_CLIENT_DELIVERY_HINT_ROUTER_DEFAULT = 1,
    COAKKA_V2_CLIENT_DELIVERY_HINT_PREFER_LOCAL = 2,
    COAKKA_V2_CLIENT_DELIVERY_HINT_REQUIRE_LOCAL = 3,
    COAKKA_V2_CLIENT_DELIVERY_HINT_REQUIRE_REMOTE = 4
} coakka_v2_client_delivery_hint_t;

/**
 * Raw request envelope builder input.
 *
 * The caller owns every string and payload byte span for the duration of the
 * build call. The returned buffer is owned by the caller and must be released
 * with coakka_v2_client_bytes_release(). Required fields outside struct_size
 * fail closed. Required strings must be non-empty. Optional tail fields outside
 * struct_size use safe defaults. struct_size = 0 means the current full local
 * struct. delivery_hint must be one of coakka_v2_client_delivery_hint_t, and
 * one_way must be 0 or 1. timeout_ms must fit the transport int32 timeout
 * field.
 */
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

/**
 * Raw reply envelope builder input.
 *
 * request_buf/request_len must contain one delivered request frame returned by
 * the runtime. The returned reply buffer is caller-owned and must be released
 * with coakka_v2_client_bytes_release(). Required fields outside struct_size
 * fail closed. Required strings must be non-empty. Optional tail fields outside
 * struct_size use safe defaults. struct_size = 0 means the current full local
 * struct. request_len must fit the wire parser's int length parameter.
 * The request frame must be reply-capable: MESSAGE_KIND_REQUEST,
 * one_way=false, and non-empty message_id/source.
 */
typedef struct coakka_v2_client_raw_reply_spec_t {
    size_t struct_size;
    const uint8_t *request_buf;
    size_t request_len;
    const char *source;
    const uint8_t *payload;
    size_t payload_len;
} coakka_v2_client_raw_reply_spec_t;

/**
 * Creates connector-side ask state over an already-created runtime host.
 *
 * The host handles span must include response_read_fd, deadletter_read_fd, and
 * delivered_request_read_fd; older spans fail closed instead of reading beyond
 * the caller's advertised struct_size. struct_size = 0 means the current full
 * local host-handle struct.
 */
coakka_v2_ask_client_t *coakka_v2_ask_client_create(
    coakka_v2_runtime_t *rt,
    const coakka_v2_host_handles_t *handles
);

/** Destroys the ask client and releases connector-side matching state. */
void coakka_v2_ask_client_destroy(coakka_v2_ask_client_t *client);

/**
 * Submits a request frame and returns a ticket used to await or poll the result.
 *
 * Returns COAKKA_V2_ERR_CLOSED when the ask helper has already observed that
 * the runtime's terminal response/deadletter lanes are no longer usable.
 */
coakka_v2_status_t coakka_v2_ask_client_begin(
    coakka_v2_ask_client_t *client,
    const uint8_t *request_buf,
    size_t request_len,
    coakka_v2_ask_ticket_t **out_ticket
);

/**
 * Blocks up to timeout_ms for a response or matched deadletter.
 *
 * On COAKKA_V2_OK, out_buf is caller-owned and must be released with
 * coakka_v2_client_bytes_release().
 * Returns COAKKA_V2_ERR_WOULD_BLOCK when the caller wait window expires while
 * the ask remains pending. Returns COAKKA_V2_ERR_CLOSED when the runtime
 * closes its terminal response/deadletter lanes before any terminal result is
 * delivered.
 */
coakka_v2_status_t coakka_v2_ask_ticket_await(
    coakka_v2_ask_ticket_t *ticket,
    uint32_t timeout_ms,
    uint32_t *out_result_kind,
    uint8_t **out_buf,
    size_t *out_len
);

/**
 * Polls an ask ticket without blocking.
 *
 * Returns COAKKA_V2_ERR_WOULD_BLOCK when no response/deadletter is available.
 * Returns COAKKA_V2_ERR_CLOSED when the runtime has already closed terminal
 * ask lanes before any matched result arrived.
 * On COAKKA_V2_OK, out_buf is caller-owned and must be released with
 * coakka_v2_client_bytes_release().
 */
coakka_v2_status_t coakka_v2_ask_ticket_poll(
    coakka_v2_ask_ticket_t *ticket,
    uint32_t *out_result_kind,
    uint8_t **out_buf,
    size_t *out_len
);

/** Cancels a pending ask ticket. Safe to call before destroying the ticket. */
void coakka_v2_ask_ticket_cancel(coakka_v2_ask_ticket_t *ticket);

/** Returns the ticket's message id; the pointer is valid until ticket destroy. */
const char *coakka_v2_ask_ticket_message_id(const coakka_v2_ask_ticket_t *ticket);

/** Releases one ask ticket and any remaining connector-side ticket state. */
void coakka_v2_ask_ticket_destroy(coakka_v2_ask_ticket_t *ticket);

/** Builds a raw request frame from a C-friendly spec. */
coakka_v2_status_t coakka_v2_client_build_raw_request(
    const coakka_v2_client_raw_request_spec_t *spec,
    uint8_t **out_buf,
    size_t *out_len
);

/** Builds a raw reply frame from a delivered request frame and response payload. */
coakka_v2_status_t coakka_v2_client_build_raw_reply(
    const coakka_v2_client_raw_reply_spec_t *spec,
    uint8_t **out_buf,
    size_t *out_len
);

/** Releases buffers returned by client build and ask-ticket APIs. */
void coakka_v2_client_bytes_release(uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif
