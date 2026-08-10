#ifndef COAKKA_V2_FILE_LANE_H
#define COAKKA_V2_FILE_LANE_H

#include <stddef.h>
#include <stdint.h>

#include "coakka/v2/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Opaque lane owned by the caller from create through stop and destroy. */
typedef struct coakka_v2_file_lane_t coakka_v2_file_lane_t;

/** Stable limits, capability flags, digest size, and progress scale. */
enum {
  COAKKA_V2_FILE_LANE_ENABLE_SENDER = 1u << 0,
  COAKKA_V2_FILE_LANE_ENABLE_RECEIVER = 1u << 1,
  COAKKA_V2_FILE_LANE_SHA256_BYTES = 32u,
  COAKKA_V2_FILE_LANE_TRANSFER_ID_MAX_BYTES = 64u,
  COAKKA_V2_FILE_LANE_TOKEN_MAX_BYTES = 128u,
  COAKKA_V2_FILE_LANE_DETAIL_MAX_BYTES = 160u,
  COAKKA_V2_FILE_LANE_PROGRESS_COMPLETE = 100000u,
  COAKKA_V2_FILE_LANE_MAX_WORKERS = 4u
};

/** Transport protection selected once when the lane is created. */
typedef enum coakka_v2_file_lane_security_mode_t {
  COAKKA_V2_FILE_LANE_SECURITY_DIRECT = COAKKA_V2_TCP_SECURITY_PLAINTEXT,
  COAKKA_V2_FILE_LANE_SECURITY_TLS = COAKKA_V2_TCP_SECURITY_TLS,
  COAKKA_V2_FILE_LANE_SECURITY_MUTUAL_TLS = COAKKA_V2_TCP_SECURITY_MUTUAL_TLS
} coakka_v2_file_lane_security_mode_t;

/** Sender or receiver side of one retained transfer record. */
typedef enum coakka_v2_file_transfer_direction_t {
  COAKKA_V2_FILE_TRANSFER_DIRECTION_SEND = 1u,
  COAKKA_V2_FILE_TRANSFER_DIRECTION_RECEIVE = 2u
} coakka_v2_file_transfer_direction_t;

/** Observable lifecycle state for one transfer side. */
typedef enum coakka_v2_file_transfer_state_t {
  COAKKA_V2_FILE_TRANSFER_STATE_PREPARED = 1u,
  COAKKA_V2_FILE_TRANSFER_STATE_QUEUED = 2u,
  COAKKA_V2_FILE_TRANSFER_STATE_CONNECTING = 3u,
  COAKKA_V2_FILE_TRANSFER_STATE_TRANSFERRING = 4u,
  COAKKA_V2_FILE_TRANSFER_STATE_VERIFYING = 5u,
  COAKKA_V2_FILE_TRANSFER_STATE_COMPLETED = 6u,
  COAKKA_V2_FILE_TRANSFER_STATE_PAUSED = 7u,
  COAKKA_V2_FILE_TRANSFER_STATE_REJECTED = 8u,
  COAKKA_V2_FILE_TRANSFER_STATE_FAILED = 9u,
  COAKKA_V2_FILE_TRANSFER_STATE_CANCELED = 10u
} coakka_v2_file_transfer_state_t;

/** Stable terminal result reported independently by each transfer side. */
typedef enum coakka_v2_file_transfer_result_t {
  COAKKA_V2_FILE_TRANSFER_RESULT_NONE = 0u,
  COAKKA_V2_FILE_TRANSFER_RESULT_OK = 1u,
  COAKKA_V2_FILE_TRANSFER_RESULT_NOT_PREPARED = 2u,
  COAKKA_V2_FILE_TRANSFER_RESULT_TOKEN_MISMATCH = 3u,
  COAKKA_V2_FILE_TRANSFER_RESULT_METADATA_MISMATCH = 4u,
  COAKKA_V2_FILE_TRANSFER_RESULT_SIZE_LIMIT = 5u,
  COAKKA_V2_FILE_TRANSFER_RESULT_STORAGE_IO = 6u,
  COAKKA_V2_FILE_TRANSFER_RESULT_INTEGRITY_MISMATCH = 7u,
  COAKKA_V2_FILE_TRANSFER_RESULT_NETWORK_IO = 8u,
  COAKKA_V2_FILE_TRANSFER_RESULT_TIMEOUT = 9u,
  COAKKA_V2_FILE_TRANSFER_RESULT_QUEUE_FULL = 10u,
  COAKKA_V2_FILE_TRANSFER_RESULT_PROTOCOL_ERROR = 11u,
  COAKKA_V2_FILE_TRANSFER_RESULT_SOURCE_CHANGED = 12u,
  COAKKA_V2_FILE_TRANSFER_RESULT_INTERNAL_ERROR = 13u,
  COAKKA_V2_FILE_TRANSFER_RESULT_CANCELED_BY_HOST = 14u,
  COAKKA_V2_FILE_TRANSFER_RESULT_TLS_CONFIG_INVALID = 15u,
  COAKKA_V2_FILE_TRANSFER_RESULT_TLS_HANDSHAKE_FAILED = 16u,
  COAKKA_V2_FILE_TRANSFER_RESULT_PEER_CERT_UNTRUSTED = 17u,
  COAKKA_V2_FILE_TRANSFER_RESULT_PEER_CERT_EXPIRED = 18u,
  COAKKA_V2_FILE_TRANSFER_RESULT_PEER_IDENTITY_MISMATCH = 19u,
  COAKKA_V2_FILE_TRANSFER_RESULT_CLIENT_CERT_REQUIRED = 20u
} coakka_v2_file_transfer_result_t;

/**
 * File-lane transport security. The lane copies every path during create.
 * DIRECT selects the platform kernel file-send path and requires a protected
 * network. TLS authenticates the receiver; MUTUAL_TLS authenticates both
 * peers. Sender-only TLS lanes need only ca_certificate_file. TLS receiver-only
 * lanes need only an identity certificate and key. Mutual-TLS peers need both
 * a CA and an identity certificate/key.
 */
typedef struct coakka_v2_file_lane_security_config_t {
  size_t struct_size;
  uint32_t mode;
  uint32_t reserved;
  uint64_t credential_generation;
  const char *credential_id;
  const char *ca_certificate_file;
  const char *identity_certificate_file;
  const char *private_key_file;
} coakka_v2_file_lane_security_config_t;

/**
 * Independent bulk-file lane configuration.
 *
 * The lane copies bind_host during create. queue_capacity is clamped to a
 * conservative implementation bound. A receiver with bind_port=0 binds an
 * ephemeral port which can be read after start with
 * coakka_v2_file_lane_get_bound_port(). Zero worker counts select one worker.
 * Progress updates are coalesced by bytes or monotonic time, whichever occurs
 * first; zero values select conservative defaults.
 *
 * A null security pointer selects DIRECT. The pointer and all referenced text
 * are copied during create. DIRECT must be deployed inside an authenticated
 * and encrypted network boundary.
 */
typedef struct coakka_v2_file_lane_config_t {
  size_t struct_size;
  uint32_t flags;
  const char *bind_host;
  uint16_t bind_port;
  size_t queue_capacity;
  uint64_t max_file_size;
  uint32_t io_timeout_ms;
  uint64_t checkpoint_bytes;
  uint64_t progress_bytes;
  uint32_t progress_interval_ms;
  uint32_t sender_worker_count;
  uint32_t receiver_worker_count;
  const coakka_v2_file_lane_security_config_t *security;
} coakka_v2_file_lane_config_t;

/**
 * Receiver-side authorization prepared by the host after its control-plane
 * policy accepts a file offer. The lane copies every string and digest.
 * destination_path is local-only and is never sent over the wire.
 */
typedef struct coakka_v2_file_receive_spec_t {
  size_t struct_size;
  const char *transfer_id;
  const char *authorization_token;
  const char *destination_path;
  uint64_t expected_size;
  uint8_t expected_sha256[COAKKA_V2_FILE_LANE_SHA256_BYTES];
} coakka_v2_file_receive_spec_t;

/**
 * Sender-side file job. The lane copies all strings and opens source_path
 * before returning. expected_sha256 is checked against that retained source
 * handle before network delivery. The host must not mutate the opened file in
 * place during the transfer.
 */
typedef struct coakka_v2_file_send_spec_t {
  size_t struct_size;
  const char *transfer_id;
  const char *authorization_token;
  const char *remote_host;
  uint16_t remote_port;
  const char *source_path;
  uint64_t expected_size;
  uint8_t expected_sha256[COAKKA_V2_FILE_LANE_SHA256_BYTES];
  uint32_t timeout_ms;
} coakka_v2_file_send_spec_t;

/**
 * Caller-owned point-in-time transfer projection.
 *
 * progress_milli ranges from 0 through 100000 (100.000%). A 100% byte count
 * is not a success receipt; state must be COMPLETED. update_sequence is
 * monotonic for the lane lifetime and can be passed to wait_transfer().
 */
typedef struct coakka_v2_file_transfer_snapshot_t {
  size_t struct_size;
  uint32_t direction;
  uint32_t state;
  uint32_t result;
  uint64_t expected_size;
  uint64_t transferred_bytes;
  uint64_t committed_offset;
  uint32_t progress_milli;
  uint32_t cancel_requested;
  uint64_t update_sequence;
  uint64_t submitted_mono_ns;
  uint64_t started_mono_ns;
  uint64_t updated_mono_ns;
  uint64_t terminal_mono_ns;
  char detail[COAKKA_V2_FILE_LANE_DETAIL_MAX_BYTES];
} coakka_v2_file_transfer_snapshot_t;

/**
 * Bounded operational counters for one lane instance. Failed totals include
 * rejected and failed attempts, but not host-canceled attempts.
 */
typedef struct coakka_v2_file_lane_stats_t {
  size_t struct_size;
  size_t queue_capacity;
  size_t queued_sends;
  size_t prepared_receives;
  size_t active_sends;
  size_t active_receives;
  size_t retained_records;
  uint64_t submitted_sends;
  uint64_t prepared_receive_count;
  uint64_t completed_sends;
  uint64_t completed_receives;
  uint64_t failed_sends;
  uint64_t failed_receives;
  uint64_t canceled_transfers;
  uint64_t completed_send_bytes;
  uint64_t completed_receive_bytes;
} coakka_v2_file_lane_stats_t;

/**
 * Creates a stopped lane, or returns NULL on invalid config or allocation
 * failure. Prefer create_ex when the exact status is required. The caller owns
 * the returned lane and must destroy it after stop.
 *
 * @param config Borrowed configuration copied before this call returns.
 * @return Owned stopped lane, or NULL on invalid input or allocation failure.
 */
coakka_v2_file_lane_t *
coakka_v2_file_lane_create(const coakka_v2_file_lane_config_t *config);

/**
 * Status-returning factory. On success, caller owns *out_lane.
 *
 * @param config Borrowed configuration copied before this call returns.
 * @param out_lane Receives the owned stopped lane on success or NULL on failure.
 * @return Stable runtime status.
 */
coakka_v2_status_t
coakka_v2_file_lane_create_ex(const coakka_v2_file_lane_config_t *config,
                              coakka_v2_file_lane_t **out_lane);

/**
 * Releases a stopped lane. Passing NULL is allowed. The caller must prevent
 * concurrent API calls and call stop before destroying a started lane.
 *
 * @param lane Owned stopped lane or NULL.
 */
void coakka_v2_file_lane_destroy(coakka_v2_file_lane_t *lane);

/**
 * Starts enabled workers and the receiver listener.
 *
 * @param lane Owned stopped lane.
 * @return COAKKA_V2_OK on transition to started, otherwise a stable status.
 */
coakka_v2_status_t coakka_v2_file_lane_start(coakka_v2_file_lane_t *lane);

/**
 * Requests stop, wakes transfer waits, joins workers, and rejects new work.
 * Repeated calls are safe.
 *
 * @param lane Owned lane to stop.
 * @return COAKKA_V2_OK for a valid lane, including repeated stop calls.
 */
coakka_v2_status_t coakka_v2_file_lane_stop(coakka_v2_file_lane_t *lane);

/**
 * Reads the active receiver port after start; output is unchanged on error.
 *
 * @param lane Borrowed started receiver-capable lane.
 * @param out_port Receives the bound host-order port on success.
 * @return Stable runtime status.
 */
coakka_v2_status_t
coakka_v2_file_lane_get_bound_port(coakka_v2_file_lane_t *lane,
                                   uint16_t *out_port);

/**
 * Registers one receiver authorization and destination. Admission is bounded;
 * the spec is copied and no file bytes are transferred by this call.
 *
 * @param lane Borrowed started receiver-capable lane.
 * @param spec Borrowed authorization copied before this call returns.
 * @return COAKKA_V2_OK on admission or a stable rejection status.
 */
coakka_v2_status_t
coakka_v2_file_lane_prepare_receive(coakka_v2_file_lane_t *lane,
                                    const coakka_v2_file_receive_spec_t *spec);

/**
 * Opens and validates the source synchronously, then queues one bounded send.
 * Queue rejection does not retain the job or its source handle.
 *
 * @param lane Borrowed started sender-capable lane.
 * @param spec Borrowed send job copied before this call returns.
 * @return COAKKA_V2_OK on admission or a stable rejection status.
 */
coakka_v2_status_t
coakka_v2_file_lane_submit_send(coakka_v2_file_lane_t *lane,
                                const coakka_v2_file_send_spec_t *spec);

/**
 * Copies the latest projection for one retained direction/transfer record.
 *
 * @param lane Borrowed started or stopped lane retaining the record.
 * @param transfer_id Borrowed NUL-terminated application transfer ID.
 * @param direction COAKKA_V2_FILE_TRANSFER_DIRECTION_SEND or RECEIVE.
 * @param out_snapshot Caller-owned output initialized with struct_size.
 * @return COAKKA_V2_OK on copy; output is unspecified on failure.
 */
coakka_v2_status_t coakka_v2_file_lane_get_transfer(
    coakka_v2_file_lane_t *lane, const char *transfer_id, uint32_t direction,
    coakka_v2_file_transfer_snapshot_t *out_snapshot);

/**
 * Waits until update_sequence advances beyond after_update_sequence.
 * timeout_ms=0 performs a non-blocking read and returns WOULD_BLOCK when no
 * newer update exists. Stop wakes the wait. On OK, out_snapshot is a complete
 * copied projection; on error its contents are unspecified.
 *
 * @param lane Borrowed lane retaining the record.
 * @param transfer_id Borrowed NUL-terminated application transfer ID.
 * @param direction Sender or receiver record to observe.
 * @param after_update_sequence Last update already processed.
 * @param timeout_ms Bounded wait in milliseconds; zero is non-blocking.
 * @param out_snapshot Caller-owned output initialized with struct_size.
 * @return COAKKA_V2_OK, WOULD_BLOCK, CLOSED, or another stable status.
 */
coakka_v2_status_t coakka_v2_file_lane_wait_transfer(
    coakka_v2_file_lane_t *lane, const char *transfer_id, uint32_t direction,
    uint64_t after_update_sequence, uint32_t timeout_ms,
    coakka_v2_file_transfer_snapshot_t *out_snapshot);

/**
 * Requests cancellation. Queued/prepared work is canceled synchronously;
 * active I/O observes the request asynchronously. Wait for terminal state
 * before forgetting the record.
 *
 * @param lane Borrowed lane retaining the record.
 * @param transfer_id Borrowed NUL-terminated application transfer ID.
 * @param direction Sender or receiver record to cancel.
 * @return Stable runtime status for the cancellation request.
 */
coakka_v2_status_t coakka_v2_file_lane_cancel_transfer(
    coakka_v2_file_lane_t *lane, const char *transfer_id, uint32_t direction);

/**
 * Removes one terminal transfer record and its prepared receive state. Active
 * records cannot be forgotten. A successful call invalidates its wait cursor.
 *
 * @param lane Borrowed lane retaining the terminal record.
 * @param transfer_id Borrowed NUL-terminated application transfer ID.
 * @param direction Sender or receiver record to release.
 * @return COAKKA_V2_OK on release or BAD_STATE for non-terminal work.
 */
coakka_v2_status_t coakka_v2_file_lane_forget_transfer(
    coakka_v2_file_lane_t *lane, const char *transfer_id, uint32_t direction);

/**
 * Reads bounded operational counters without exposing file semantics.
 *
 * @param lane Borrowed lane.
 * @param out_stats Caller-owned output initialized with struct_size.
 * @return COAKKA_V2_OK on copy; output is unspecified on failure.
 */
coakka_v2_status_t
coakka_v2_file_lane_get_stats(coakka_v2_file_lane_t *lane,
                              coakka_v2_file_lane_stats_t *out_stats);

/**
 * Computes the digest and size used by prepare/send contracts. The path is
 * borrowed only for the call. Both outputs are written only on success.
 *
 * @param path Borrowed NUL-terminated local file path.
 * @param out_sha256 Caller-owned 32-byte digest output.
 * @param out_size Caller-owned byte-count output.
 * @return COAKKA_V2_OK on a stable read or a storage/argument status.
 */
coakka_v2_status_t
coakka_v2_file_sha256_path(const char *path,
                           uint8_t out_sha256[COAKKA_V2_FILE_LANE_SHA256_BYTES],
                           uint64_t *out_size);

#ifdef __cplusplus
}
#endif

#endif
