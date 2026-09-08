#include "test_support.h"

#include <stdio.h>

void coakka_http_test_fail(coakka_http_test_state_t *state, const char *file,
                           int line, const char *expression) {
  state->failures += UINT64_C(1);
  (void)fprintf(stderr, "%s:%d: case=%s check failed: %s\n", file, line,
                state->case_name, expression);
}
