#include "coakka/logger/core.h"
#include "coakka/logger/utils.h"

#include <stdio.h>
#include <string.h>

static int require_ok(coakka_logger_status_t status, const char* step) {
  if (status == COAKKA_LOGGER_STATUS_OK) {
    return 0;
  }
  fprintf(stderr, "%s failed: %s\n", step, coakka_logger_status_name(status));
  return 1;
}

int main(void) {
  coakka_logger_core_config_t config = coakka_logger_core_default_config();
  config.system_name = "logger-c-basic";
  config.queue_capacity = 8;
  config.category_capacity = 64;
  config.message_capacity = 128;

  coakka_logger_core_handle_t* handle = NULL;
  if (require_ok(coakka_logger_core_create(&config, &handle), "create")) {
    return 1;
  }
  if (require_ok(coakka_logger_core_start(handle), "start")) {
    coakka_logger_core_destroy(handle);
    return 1;
  }

  uint64_t sequence = 0;
  if (require_ok(coakka_logger_core_log_info(handle, "startup", "logger core booted", &sequence),
                 "log_info")) {
    coakka_logger_core_destroy(handle);
    return 1;
  }

  char category[64];
  char message[128];
  memset(category, 0, sizeof(category));
  memset(message, 0, sizeof(message));

  coakka_logger_core_record_buffer_t record;
  memset(&record, 0, sizeof(record));
  record.struct_size = sizeof(record);
  record.category = category;
  record.category_capacity = sizeof(category);
  record.message = message;
  record.message_capacity = sizeof(message);

  if (require_ok(coakka_logger_core_read_next(handle, 100, &record), "read_next")) {
    coakka_logger_core_destroy(handle);
    return 1;
  }

  printf("sequence=%llu level=%s category=%.*s message=%.*s\n",
         (unsigned long long)record.sequence,
         coakka_logger_level_name(record.level),
         (int)record.category_length,
         record.category,
         (int)record.message_length,
         record.message);

  if (require_ok(coakka_logger_core_stop(handle), "stop")) {
    coakka_logger_core_destroy(handle);
    return 1;
  }
  coakka_logger_core_destroy(handle);
  return 0;
}
