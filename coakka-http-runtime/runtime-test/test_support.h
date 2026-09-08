#ifndef COAKKA_HTTP_RUNTIME_TEST_SUPPORT_H
#define COAKKA_HTTP_RUNTIME_TEST_SUPPORT_H

#include <stddef.h>
#include <stdint.h>

typedef struct coakka_http_test_state {
  const char *case_name;
  uint64_t checks;
  uint64_t failures;
} coakka_http_test_state_t;

void coakka_http_test_fail(coakka_http_test_state_t *state, const char *file,
                           int line, const char *expression);

#define COAKKA_HTTP_TEST_CHECK(state, expression)                              \
  do {                                                                         \
    (state)->checks += UINT64_C(1);                                            \
    if (!(expression)) {                                                       \
      coakka_http_test_fail((state), __FILE__, __LINE__, #expression);         \
    }                                                                          \
  } while (0)

#endif
