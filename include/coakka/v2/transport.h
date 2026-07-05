#ifndef COAKKA_V2_TRANSPORT_H
#define COAKKA_V2_TRANSPORT_H

#include <stddef.h>
#include <stdint.h>

#include "coakka/v2/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct coakka_v2_frame_reader_t coakka_v2_frame_reader_t;

/**
 * Creates a bounded framed-pipe reader.
 *
 * The reader does not own fd. The host remains responsible for closing the fd
 * when the lane is no longer needed.
 */
coakka_v2_frame_reader_t *coakka_v2_frame_reader_create(int fd, size_t max_frame_size);

/** Destroys a frame reader without closing the fd passed at creation time. */
void coakka_v2_frame_reader_destroy(coakka_v2_frame_reader_t *reader);

/**
 * Attempts to read one complete frame without blocking the runtime.
 *
 * Returns COAKKA_V2_ERR_WOULD_BLOCK when no full frame is available. On
 * COAKKA_V2_OK, out_buf is caller-owned and must be released with
 * coakka_v2_frame_release().
 */
coakka_v2_status_t coakka_v2_frame_read_try(coakka_v2_frame_reader_t *reader,
                                            uint8_t **out_buf,
                                            size_t *out_len);

/** Writes one length-prefixed frame to fd. The payload span is not retained. */
coakka_v2_status_t coakka_v2_frame_write(int fd,
                                         const uint8_t *payload,
                                         size_t payload_len);

/** Releases a buffer returned by coakka_v2_frame_read_try(). */
void coakka_v2_frame_release(uint8_t *buf);

/**
 * Consumes monitor doorbell bytes and returns the number of signals drained.
 *
 * monitor_read_fd is a wakeup signal. It does not carry serialized monitor
 * event records; refresh stats/health through direct runtime APIs after drain.
 */
coakka_v2_status_t coakka_v2_monitor_consume(int fd, uint64_t *out_signal_count);

#ifdef __cplusplus
}
#endif

#endif
