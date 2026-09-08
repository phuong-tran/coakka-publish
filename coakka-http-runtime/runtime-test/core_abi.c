#include <coakka/http/core.h>

#include "test_support.h"
#include "test_threads.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*coakka_http_test_case_fn)(coakka_http_test_state_t *state);

typedef struct coakka_http_test_case {
  const char *name;
  coakka_http_test_case_fn run;
} coakka_http_test_case_t;

static coakka_http_route_t route(uint64_t id, const char *method,
                                 const char *pattern) {
  const coakka_http_route_t value = {
      id,
      (const uint8_t *)method,
      strlen(method),
      (const uint8_t *)pattern,
      strlen(pattern),
  };
  return value;
}

static void abi_identity(coakka_http_test_state_t *state) {
  static const struct {
    coakka_http_core_status_t status;
    const char *name;
  } expected[] = {
      {COAKKA_HTTP_CORE_OK, "ok"},
      {COAKKA_HTTP_CORE_INVALID_ARGUMENT, "invalid_argument"},
      {COAKKA_HTTP_CORE_INVALID_METHOD, "invalid_method"},
      {COAKKA_HTTP_CORE_INVALID_PATTERN, "invalid_pattern"},
      {COAKKA_HTTP_CORE_CAPACITY_EXHAUSTED, "capacity_exhausted"},
      {COAKKA_HTTP_CORE_AMBIGUOUS, "ambiguous"},
      {COAKKA_HTTP_CORE_OUT_OF_MEMORY, "out_of_memory"},
      {COAKKA_HTTP_CORE_DUPLICATE_ROUTE_ID, "duplicate_route_id"},
      {COAKKA_HTTP_CORE_UNKNOWN, "unknown"},
  };
  size_t index;

  COAKKA_HTTP_TEST_CHECK(state, coakka_http_core_abi_version() ==
                                    COAKKA_HTTP_CORE_ABI_VERSION);
  for (index = 0U; index < sizeof(expected) / sizeof(expected[0]); ++index) {
    const char *name = coakka_http_core_status_name(expected[index].status);
    COAKKA_HTTP_TEST_CHECK(state, name != NULL);
    if (name != NULL) {
      COAKKA_HTTP_TEST_CHECK(state, strcmp(name, expected[index].name) == 0);
    }
  }
  COAKKA_HTTP_TEST_CHECK(
      state, strcmp(coakka_http_core_status_name(INT32_C(-1)), "unknown") == 0);
  COAKKA_HTTP_TEST_CHECK(
      state, strcmp(coakka_http_core_status_name(INT32_C(6)), "unknown") == 0);
  COAKKA_HTTP_TEST_CHECK(
      state,
      strcmp(coakka_http_core_status_name(INT32_C(999)), "unknown") == 0);
}

static void valid_routes_and_borrowing(coakka_http_test_state_t *state) {
  uint8_t method[] = "POST";
  uint8_t pattern[] = "/items/{item_id}";
  const uint8_t method_before[] = "POST";
  const uint8_t pattern_before[] = "/items/{item_id}";
  coakka_http_route_t routes[] = {
      route(UINT64_C(1), "GET", "/"),
      {UINT64_C(2), method, sizeof(method) - 1U, pattern, sizeof(pattern) - 1U},
      route(UINT64_C(3), "M-SEARCH", "/encoded/a%2Fb"),
  };
  uint32_t failed_index = 0U;

  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_core_validate_routes(routes, 3U, &failed_index) ==
                 COAKKA_HTTP_CORE_OK);
  COAKKA_HTTP_TEST_CHECK(state,
                         failed_index == COAKKA_HTTP_CORE_FAILED_INDEX_NONE);
  COAKKA_HTTP_TEST_CHECK(state,
                         memcmp(method, method_before, sizeof(method)) == 0);
  COAKKA_HTTP_TEST_CHECK(state,
                         memcmp(pattern, pattern_before, sizeof(pattern)) == 0);
  COAKKA_HTTP_TEST_CHECK(state, routes[1].route_id == UINT64_C(2));
  COAKKA_HTTP_TEST_CHECK(state, routes[1].method == method);
  COAKKA_HTTP_TEST_CHECK(state, routes[1].method_size == sizeof(method) - 1U);
  COAKKA_HTTP_TEST_CHECK(state, routes[1].encoded_path_pattern == pattern);
  COAKKA_HTTP_TEST_CHECK(state, routes[1].encoded_path_pattern_size ==
                                    sizeof(pattern) - 1U);

  /* The prior call retained no borrowed storage; new bytes define a new call.
   */
  method[0] = (uint8_t)'P';
  method[1] = (uint8_t)'U';
  method[2] = (uint8_t)'T';
  routes[1].method_size = 3U;
  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_core_validate_routes(routes, 3U, &failed_index) ==
                 COAKKA_HTTP_CORE_OK);
}

static void invalid_arguments(coakka_http_test_state_t *state) {
  const uint8_t get[] = "GET";
  const uint8_t path[] = "/items";
  coakka_http_route_t value = {UINT64_C(1), get, 3U, path, 6U};
  uint32_t failed_index = 0U;

  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_core_validate_routes(NULL, 1U, &failed_index) ==
                 COAKKA_HTTP_CORE_INVALID_ARGUMENT);
  COAKKA_HTTP_TEST_CHECK(state,
                         failed_index == COAKKA_HTTP_CORE_FAILED_INDEX_NONE);
  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_core_validate_routes(&value, 0U, &failed_index) ==
                 COAKKA_HTTP_CORE_INVALID_ARGUMENT);
  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_core_validate_routes(
                 &value, COAKKA_HTTP_CORE_MAX_ROUTES + 1U, &failed_index) ==
                 COAKKA_HTTP_CORE_INVALID_ARGUMENT);

  value.route_id = UINT64_C(0);
  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_core_validate_routes(&value, 1U, &failed_index) ==
                 COAKKA_HTTP_CORE_INVALID_ARGUMENT);
  COAKKA_HTTP_TEST_CHECK(state, failed_index == 0U);
  value.route_id = UINT64_C(1);
  value.method = NULL;
  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_core_validate_routes(&value, 1U, &failed_index) ==
                 COAKKA_HTTP_CORE_INVALID_METHOD);
  COAKKA_HTTP_TEST_CHECK(state, failed_index == 0U);
  value.method = get;
  /* Oversized lengths must be rejected before reading the short backing array.
   */
  value.method_size = SIZE_MAX;
  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_core_validate_routes(&value, 1U, &failed_index) ==
                 COAKKA_HTTP_CORE_INVALID_METHOD);
  COAKKA_HTTP_TEST_CHECK(state, failed_index == 0U);
  value.method = get;
  value.method_size = 3U;
  value.encoded_path_pattern = NULL;
  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_core_validate_routes(&value, 1U, &failed_index) ==
                 COAKKA_HTTP_CORE_INVALID_PATTERN);
  COAKKA_HTTP_TEST_CHECK(state, failed_index == 0U);
  value.encoded_path_pattern = path;
  /* The same fail-closed rule applies independently to the path extent. */
  value.encoded_path_pattern_size = SIZE_MAX;
  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_core_validate_routes(&value, 1U, &failed_index) ==
                 COAKKA_HTTP_CORE_CAPACITY_EXHAUSTED);
  COAKKA_HTTP_TEST_CHECK(state, failed_index == 0U);
  COAKKA_HTTP_TEST_CHECK(state,
                         coakka_http_core_validate_routes(&value, 1U, NULL) ==
                             COAKKA_HTTP_CORE_CAPACITY_EXHAUSTED);
}

static void invalid_methods_and_patterns(coakka_http_test_state_t *state) {
  static const char *const invalid_methods[] = {"", "G ET", "GET\n", "G@T"};
  static const char *const invalid_patterns[] = {
      "items",        "/items?mode=test",  "/items#part",
      "/items/%ZZ",   "/items/{bad name}", "/items/{id}/{id}",
      "/items/{open",
  };
  size_t index;
  uint32_t failed_index;

  for (index = 0U; index < sizeof(invalid_methods) / sizeof(invalid_methods[0]);
       ++index) {
    const coakka_http_route_t value =
        route(UINT64_C(1), invalid_methods[index], "/items");
    COAKKA_HTTP_TEST_CHECK(
        state, coakka_http_core_validate_routes(&value, 1U, &failed_index) ==
                   COAKKA_HTTP_CORE_INVALID_METHOD);
    COAKKA_HTTP_TEST_CHECK(state, failed_index == 0U);
  }
  for (index = 0U;
       index < sizeof(invalid_patterns) / sizeof(invalid_patterns[0]);
       ++index) {
    const coakka_http_route_t value =
        route(UINT64_C(1), "GET", invalid_patterns[index]);
    COAKKA_HTTP_TEST_CHECK(
        state, coakka_http_core_validate_routes(&value, 1U, &failed_index) ==
                   COAKKA_HTTP_CORE_INVALID_PATTERN);
    COAKKA_HTTP_TEST_CHECK(state, failed_index == 0U);
  }
}

static void conflicts_fail_closed(coakka_http_test_state_t *state) {
  const coakka_http_route_t ambiguous[] = {
      route(UINT64_C(1), "GET", "/items/{id}"),
      route(UINT64_C(2), "GET", "/items/{name}"),
  };
  const coakka_http_route_t duplicate_id[] = {
      route(UINT64_C(7), "GET", "/first"),
      route(UINT64_C(7), "POST", "/second"),
  };
  uint32_t failed_index = 0U;

  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_core_validate_routes(ambiguous, 2U, &failed_index) ==
                 COAKKA_HTTP_CORE_AMBIGUOUS);
  COAKKA_HTTP_TEST_CHECK(state, failed_index == 1U);
  COAKKA_HTTP_TEST_CHECK(state, coakka_http_core_validate_routes(
                                    duplicate_id, 2U, &failed_index) ==
                                    COAKKA_HTTP_CORE_DUPLICATE_ROUTE_ID);
  COAKKA_HTTP_TEST_CHECK(state, failed_index == 1U);

  /* A failed call owns no persistent route state. */
  COAKKA_HTTP_TEST_CHECK(state, coakka_http_core_validate_routes(
                                    duplicate_id, 1U, &failed_index) ==
                                    COAKKA_HTTP_CORE_OK);
}

static int build_segment_routes(coakka_http_route_t *routes, char *paths,
                                uint32_t segments_per_route) {
  uint32_t route_index;
  for (route_index = 0U; route_index < COAKKA_HTTP_CORE_MAX_ROUTES;
       ++route_index) {
    char *cursor = paths + (size_t)route_index * 160U;
    int written = snprintf(cursor, 160U, "/r%04u", route_index);
    uint32_t segment;
    if (written < 0 || written >= 160) {
      return -1;
    }
    for (segment = 1U; segment < segments_per_route; ++segment) {
      const size_t used = strlen(cursor);
      if (used + 2U >= 160U) {
        return -1;
      }
      cursor[used] = '/';
      cursor[used + 1U] = 's';
      cursor[used + 2U] = '\0';
    }
    routes[route_index] =
        route((uint64_t)route_index + UINT64_C(1), "GET", cursor);
  }
  return 0;
}

static void route_and_segment_bounds(coakka_http_test_state_t *state) {
  coakka_http_route_t *routes = (coakka_http_route_t *)calloc(
      COAKKA_HTTP_CORE_MAX_ROUTES, sizeof(*routes));
  char *paths =
      (char *)calloc((size_t)COAKKA_HTTP_CORE_MAX_ROUTES, (size_t)160U);
  uint32_t failed_index = 0U;

  COAKKA_HTTP_TEST_CHECK(state, routes != NULL);
  COAKKA_HTTP_TEST_CHECK(state, paths != NULL);
  if (routes == NULL || paths == NULL) {
    free(paths);
    free(routes);
    return;
  }
  COAKKA_HTTP_TEST_CHECK(state, build_segment_routes(routes, paths, 16U) == 0);
  COAKKA_HTTP_TEST_CHECK(state, coakka_http_core_validate_routes(
                                    routes, COAKKA_HTTP_CORE_MAX_ROUTES,
                                    &failed_index) == COAKKA_HTTP_CORE_OK);
  COAKKA_HTTP_TEST_CHECK(state,
                         failed_index == COAKKA_HTTP_CORE_FAILED_INDEX_NONE);

  COAKKA_HTTP_TEST_CHECK(state, build_segment_routes(routes, paths, 17U) == 0);
  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_core_validate_routes(
                 routes, COAKKA_HTTP_CORE_MAX_ROUTES, &failed_index) ==
                 COAKKA_HTTP_CORE_CAPACITY_EXHAUSTED);
  /* 963 complete routes consume 16,371 segments; index 963 crosses 16,384. */
  COAKKA_HTTP_TEST_CHECK(state, failed_index == 963U);
  free(paths);
  free(routes);
}

static char *capture_pattern(uint32_t count) {
  size_t capacity = (size_t)count * 12U + 2U;
  char *pattern = (char *)calloc(capacity, 1U);
  uint32_t index;
  if (pattern == NULL) {
    return NULL;
  }
  for (index = 0U; index < count; ++index) {
    const size_t used = strlen(pattern);
    const int written =
        snprintf(pattern + used, capacity - used, "/{p%u}", index);
    if (written < 0 || (size_t)written >= capacity - used) {
      free(pattern);
      return NULL;
    }
  }
  return pattern;
}

static void capture_bound(coakka_http_test_state_t *state) {
  char *at_limit = capture_pattern(COAKKA_HTTP_CORE_MAX_CAPTURES_PER_ROUTE);
  char *over_limit =
      capture_pattern(COAKKA_HTTP_CORE_MAX_CAPTURES_PER_ROUTE + 1U);
  uint32_t failed_index = 0U;

  COAKKA_HTTP_TEST_CHECK(state, at_limit != NULL);
  COAKKA_HTTP_TEST_CHECK(state, over_limit != NULL);
  if (at_limit != NULL) {
    const coakka_http_route_t value = route(UINT64_C(1), "GET", at_limit);
    COAKKA_HTTP_TEST_CHECK(
        state, coakka_http_core_validate_routes(&value, 1U, &failed_index) ==
                   COAKKA_HTTP_CORE_OK);
  }
  if (over_limit != NULL) {
    const coakka_http_route_t value = route(UINT64_C(1), "GET", over_limit);
    COAKKA_HTTP_TEST_CHECK(
        state, coakka_http_core_validate_routes(&value, 1U, &failed_index) ==
                   COAKKA_HTTP_CORE_CAPACITY_EXHAUSTED);
    COAKKA_HTTP_TEST_CHECK(state, failed_index == 0U);
  }
  free(over_limit);
  free(at_limit);
}

static void route_declaration_byte_bound(coakka_http_test_state_t *state) {
  /*
   * Each single-segment route consumes method bytes, encoded pattern bytes,
   * and decoded segment bytes. These lengths place the final route exactly at
   * the public declaration budget without relying on private state.
   */
  enum { route_count = 64, max_path_bytes = 8192, final_path_bytes = 8128 };
  coakka_http_route_t *routes =
      (coakka_http_route_t *)calloc(route_count, sizeof(*routes));
  char *paths = (char *)calloc(route_count, max_path_bytes + 1U);
  uint32_t index;
  uint32_t failed_index = 0U;

  COAKKA_HTTP_TEST_CHECK(state, routes != NULL);
  COAKKA_HTTP_TEST_CHECK(state, paths != NULL);
  if (routes == NULL || paths == NULL) {
    free(paths);
    free(routes);
    return;
  }
  for (index = 0U; index < route_count; ++index) {
    const size_t path_bytes =
        index + 1U == route_count ? final_path_bytes : max_path_bytes;
    char *path = paths + (size_t)index * (max_path_bytes + 1U);
    const int prefix = snprintf(path, max_path_bytes + 1U, "/r%02u-", index);
    COAKKA_HTTP_TEST_CHECK(state, prefix == 5);
    if (prefix != 5) {
      free(paths);
      free(routes);
      return;
    }
    memset(path + prefix, 'a', path_bytes - (size_t)prefix);
    path[path_bytes] = '\0';
    routes[index] = route((uint64_t)index + UINT64_C(1), "GET", path);
  }
  COAKKA_HTTP_TEST_CHECK(state, coakka_http_core_validate_routes(
                                    routes, route_count, &failed_index) ==
                                    COAKKA_HTTP_CORE_OK);
  COAKKA_HTTP_TEST_CHECK(state,
                         failed_index == COAKKA_HTTP_CORE_FAILED_INDEX_NONE);

  paths[(size_t)(route_count - 1) * (max_path_bytes + 1U) + final_path_bytes] =
      'a';
  paths[(size_t)(route_count - 1) * (max_path_bytes + 1U) + final_path_bytes +
        1U] = '\0';
  routes[route_count - 1].encoded_path_pattern_size = final_path_bytes + 1U;
  COAKKA_HTTP_TEST_CHECK(state, coakka_http_core_validate_routes(
                                    routes, route_count, &failed_index) ==
                                    COAKKA_HTTP_CORE_CAPACITY_EXHAUSTED);
  COAKKA_HTTP_TEST_CHECK(state, failed_index == 63U);
  free(paths);
  free(routes);
}

typedef struct concurrent_context {
  uint32_t iterations;
  uint32_t failures;
} concurrent_context_t;

static int concurrent_worker(void *opaque) {
  concurrent_context_t *context = (concurrent_context_t *)opaque;
  const coakka_http_route_t routes[] = {
      route(UINT64_C(1), "GET", "/health"),
      route(UINT64_C(2), "POST", "/items/{id}"),
  };
  uint32_t iteration;

  for (iteration = 0U; iteration < context->iterations; ++iteration) {
    uint32_t failed_index = 0U;
    if (coakka_http_core_validate_routes(routes, 2U, &failed_index) !=
            COAKKA_HTTP_CORE_OK ||
        failed_index != COAKKA_HTTP_CORE_FAILED_INDEX_NONE) {
      context->failures += 1U;
    }
  }
  return context->failures == 0U ? 0 : -1;
}

static void concurrent_independent_calls(coakka_http_test_state_t *state) {
  enum { worker_count = 4, iterations = 32 };
  concurrent_context_t contexts[worker_count];
  void *context_pointers[worker_count];
  size_t index;

  memset(contexts, 0, sizeof(contexts));
  for (index = 0U; index < worker_count; ++index) {
    contexts[index].iterations = iterations;
    context_pointers[index] = &contexts[index];
  }
  COAKKA_HTTP_TEST_CHECK(
      state, coakka_http_test_run_threads(concurrent_worker, context_pointers,
                                          worker_count) == 0);
  for (index = 0U; index < worker_count; ++index) {
    COAKKA_HTTP_TEST_CHECK(state, contexts[index].failures == 0U);
  }
}

int main(void) {
  static const coakka_http_test_case_t cases[] = {
      {"abi_identity", abi_identity},
      {"valid_routes_and_borrowing", valid_routes_and_borrowing},
      {"invalid_arguments", invalid_arguments},
      {"invalid_methods_and_patterns", invalid_methods_and_patterns},
      {"conflicts_fail_closed", conflicts_fail_closed},
      {"route_and_segment_bounds", route_and_segment_bounds},
      {"capture_bound", capture_bound},
      {"route_declaration_byte_bound", route_declaration_byte_bound},
      {"concurrent_independent_calls", concurrent_independent_calls},
  };
  uint64_t total_checks = UINT64_C(0);
  uint64_t total_failures = UINT64_C(0);
  size_t index;

  (void)printf("{\"schema\":\"coakka.http.runtime-test.v1\","
               "\"abiVersion\":%u,\"cases\":[",
               coakka_http_core_abi_version());
  for (index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index) {
    coakka_http_test_state_t state = {cases[index].name, UINT64_C(0),
                                      UINT64_C(0)};
    cases[index].run(&state);
    total_checks += state.checks;
    total_failures += state.failures;
    (void)printf("%s{\"name\":\"%s\",\"checks\":%llu,\"status\":\"%s\"}",
                 index == 0U ? "" : ",", state.case_name,
                 (unsigned long long)state.checks,
                 state.failures == 0U ? "pass" : "fail");
  }
  (void)printf(
      "],\"summary\":{\"cases\":%llu,\"checks\":%llu,\"failures\":%llu,"
      "\"status\":\"%s\"}}\n",
      (unsigned long long)(sizeof(cases) / sizeof(cases[0])),
      (unsigned long long)total_checks, (unsigned long long)total_failures,
      total_failures == 0U ? "pass" : "fail");
  return total_failures == 0U ? EXIT_SUCCESS : EXIT_FAILURE;
}
