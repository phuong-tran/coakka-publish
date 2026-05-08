#ifndef COAKKA_V2_TRANSPORT_H
#define COAKKA_V2_TRANSPORT_H

#include <stddef.h>
#include <stdint.h>

#include "coakka/v2/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct coakka_v2_frame_reader_t coakka_v2_frame_reader_t;

coakka_v2_frame_reader_t *coakka_v2_frame_reader_create(int fd, size_t max_frame_size);

void coakka_v2_frame_reader_destroy(coakka_v2_frame_reader_t *reader);

coakka_v2_status_t coakka_v2_frame_read_try(coakka_v2_frame_reader_t *reader,
                                            uint8_t **out_buf,
                                            size_t *out_len);

coakka_v2_status_t coakka_v2_frame_write(int fd,
                                         const uint8_t *payload,
                                         size_t payload_len);

void coakka_v2_frame_release(uint8_t *buf);

coakka_v2_status_t coakka_v2_monitor_consume(int fd, uint64_t *out_signal_count);

#ifdef __cplusplus
}
#endif

#endif
