#ifndef COAKKA_HTTP_RUNTIME_TEST_THREADS_H
#define COAKKA_HTTP_RUNTIME_TEST_THREADS_H

#include <stddef.h>

typedef int (*coakka_http_test_thread_fn)(void *context);

/*
 * Starts every worker and holds it at one shared gate until all workers have
 * reached that gate. Each context is exclusively owned by its matching worker
 * until this function returns.
 */
int coakka_http_test_run_threads(coakka_http_test_thread_fn worker,
                                 void **contexts, size_t count);

#endif
