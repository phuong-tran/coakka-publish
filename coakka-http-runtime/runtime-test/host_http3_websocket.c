/* Build against the installed host SDK; the peer drives real loopback QUIC. */
#include <coakka/http/host.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_support.h"
#include "test_threads.h"

#define CHECK(value)                                                      \
  do {                                                                    \
    if (!(value)) {                                                       \
      fprintf(stderr, "HTTP/3 WebSocket fixture line %d: %s\n", __LINE__, \
              #value);                                                    \
      exit(EXIT_FAILURE);                                                 \
    }                                                                     \
  } while (0)

static coakka_http_host_bytes_t bytes(const char* text) {
  const coakka_http_host_bytes_t view = {(const uint8_t*)text, strlen(text)};
  return view;
}

static int equals(coakka_http_host_bytes_t view, const char* text) {
  return view.size == strlen(text) && memcmp(view.data, text, view.size) == 0;
}

static int same_session(coakka_http_host_websocket_t a,
                        coakka_http_host_websocket_t b) {
  return a.slot == b.slot && a.generation == b.generation;
}

static void marker(const char* text) {
  puts(text);
  fflush(stdout);
}

static void barrier(const char* expected) {
  char input[32];
  CHECK(fgets(input, sizeof(input), stdin) != NULL);
  CHECK(strcmp(input, expected) == 0);
}

static coakka_http_host_request_event_t take_request(
    coakka_http_host_service_t* service, const char* target) {
  for (unsigned attempt = 0; attempt < 16; ++attempt) {
    coakka_http_host_request_event_t event;
    coakka_http_host_request_event_init(&event);
    CHECK(coakka_http_host_take_request(service, 4000U, &event) ==
          COAKKA_HTTP_HOST_OK);
    if (event.kind == COAKKA_HTTP_HOST_REQUEST) {
      coakka_http_host_request_view_t request;
      coakka_http_host_request_view_init(&request);
      CHECK(coakka_http_host_request_view(&event, &request) ==
            COAKKA_HTTP_HOST_OK);
      CHECK(equals(request.method, "GET") && equals(request.target, target));
      return event;
    }
    CHECK(coakka_http_host_release_request(service, &event) ==
          COAKKA_HTTP_HOST_OK);
  }
  fputs("Request event budget exhausted\n", stderr);
  exit(EXIT_FAILURE);
}

static coakka_http_host_websocket_event_t websocket_event(
    coakka_http_host_service_t* service,
    coakka_http_host_websocket_event_kind_t kind) {
  for (unsigned attempt = 0; attempt < 16; ++attempt) {
    coakka_http_host_websocket_event_t event;
    coakka_http_host_websocket_event_init(&event);
    CHECK(coakka_http_host_take_websocket(service, 4000U, &event) ==
          COAKKA_HTTP_HOST_OK);
    if (event.kind == kind) {
      return event;
    }
    CHECK(event.kind == COAKKA_HTTP_HOST_WEBSOCKET_WRITABLE);
    CHECK(coakka_http_host_release_websocket(service, &event) ==
          COAKKA_HTTP_HOST_OK);
  }
  fputs("WebSocket event budget exhausted\n", stderr);
  exit(EXIT_FAILURE);
}

static coakka_http_host_websocket_t accept_ws(
    coakka_http_host_service_t* service, const char* path) {
  coakka_http_host_request_event_t request = take_request(service, path);
  const coakka_http_host_exchange_t exchange = request.exchange;
  CHECK(coakka_http_host_release_request(service, &request) ==
        COAKKA_HTTP_HOST_OK);
  CHECK(coakka_http_host_accept_websocket(service, exchange, bytes("echo")) ==
        COAKKA_HTTP_HOST_OK);
  coakka_http_host_websocket_event_t event =
      websocket_event(service, COAKKA_HTTP_HOST_WEBSOCKET_OPEN);
  CHECK(event.origin_exchange.slot == exchange.slot);
  CHECK(event.origin_exchange.generation == exchange.generation);
  CHECK(equals(event.data, "echo"));
  const coakka_http_host_websocket_t session = event.session;
  CHECK(coakka_http_host_release_websocket(service, &event) ==
        COAKKA_HTTP_HOST_OK);
  return session;
}

static void ordinary_response(coakka_http_host_service_t* service) {
  coakka_http_host_request_event_t request = take_request(service, "/plain");
  coakka_http_host_response_t response;
  coakka_http_host_response_init(&response);
  response.status_code = 200U;
  response.body = bytes("ordinary");
  CHECK(coakka_http_host_respond(service, request.exchange, &response) ==
        COAKKA_HTTP_HOST_OK);
  CHECK(coakka_http_host_release_request(service, &request) ==
        COAKKA_HTTP_HOST_OK);
}

static void expect_abnormal_close(coakka_http_host_service_t* service,
                                  coakka_http_host_websocket_t session) {
  coakka_http_host_websocket_event_t closed =
      websocket_event(service, COAKKA_HTTP_HOST_WEBSOCKET_CLOSE);
  CHECK(same_session(closed.session, session) && closed.close_code == 1006U);
  CHECK(coakka_http_host_send_websocket(service, session,
                                        COAKKA_HTTP_HOST_WEBSOCKET_TEXT,
                                        bytes("stale")) != COAKKA_HTTP_HOST_OK);
  CHECK(coakka_http_host_release_websocket(service, &closed) ==
        COAKKA_HTTP_HOST_OK);
}

static void check_live_sibling(coakka_http_host_service_t* service,
                               coakka_http_host_websocket_t b) {
  coakka_http_host_websocket_event_t live =
      websocket_event(service, COAKKA_HTTP_HOST_WEBSOCKET_TEXT);
  CHECK(same_session(live.session, b) && equals(live.data, "still-live"));
  CHECK(coakka_http_host_send_websocket(service, b,
                                        COAKKA_HTTP_HOST_WEBSOCKET_TEXT,
                                        live.data) == COAKKA_HTTP_HOST_OK);
  CHECK(coakka_http_host_release_websocket(service, &live) ==
        COAKKA_HTTP_HOST_OK);

  ordinary_response(service);
  coakka_http_host_websocket_event_t closed =
      websocket_event(service, COAKKA_HTTP_HOST_WEBSOCKET_CLOSE);
  CHECK(same_session(closed.session, b) && closed.close_code == 1000U);
  CHECK(coakka_http_host_release_websocket(service, &closed) ==
        COAKKA_HTTP_HOST_OK);
  // App CLOSE means the response was committed. Let the peer observe it on
  // the wire before stopping the whole listener.
  barrier("WIRE_CLOSED\n");
}

static void check_reset(coakka_http_host_service_t* service, int pending_write) {
  const coakka_http_host_websocket_t a = accept_ws(service, "/a");
  const coakka_http_host_websocket_t b = accept_ws(service, "/b");
  if (pending_write) {
    // A's peer allows only 4 KiB of stream credit. The wire driver verifies
    // partial delivery at that limit before sending the reset.
    static const uint8_t payload[16U * 1024U] = {0};
    const coakka_http_host_bytes_t data = {payload, sizeof(payload)};
    CHECK(coakka_http_host_send_websocket(service, a,
                                          COAKKA_HTTP_HOST_WEBSOCKET_BINARY,
                                          data) == COAKKA_HTTP_HOST_OK);
    marker("WRITE_ACCEPTED");
    expect_abnormal_close(service, a);
  } else {
    coakka_http_host_websocket_event_t held =
        websocket_event(service, COAKKA_HTTP_HOST_WEBSOCKET_TEXT);
    CHECK(same_session(held.session, a) && equals(held.data, "held"));
    marker("HELD");
    // CLOSE retires the session. DATA bytes remain borrowed until release.
    expect_abnormal_close(service, a);
    CHECK(equals(held.data, "held"));
    CHECK(coakka_http_host_release_websocket(service, &held) ==
          COAKKA_HTTP_HOST_OK);
  }
  marker("A_CLOSED");
  check_live_sibling(service, b);
}

static void check_pre_accept_reset(coakka_http_host_service_t* service) {
  const coakka_http_host_websocket_t b = accept_ws(service, "/b");
  coakka_http_host_request_event_t event = take_request(service, "/a");
  const coakka_http_host_exchange_t a = event.exchange;
  CHECK(coakka_http_host_release_request(service, &event) ==
        COAKKA_HTTP_HOST_OK);

  // Routing has delivered A to the application. Keep accept unsubmitted
  // until the application observes the terminal for the cancelled request.
  marker("PRE_ACCEPT");
  int cancelled = 0;
  for (;;) {
    coakka_http_host_request_event_init(&event);
    CHECK(coakka_http_host_take_request(service, 4000U, &event) ==
          COAKKA_HTTP_HOST_OK);
    CHECK(event.exchange.slot == a.slot && event.exchange.generation == a.generation);
    const coakka_http_host_request_event_kind_t kind = event.kind;
    CHECK(coakka_http_host_release_request(service, &event) ==
          COAKKA_HTTP_HOST_OK);
    if (kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL)
      break;
    CHECK(kind == COAKKA_HTTP_HOST_REQUEST_CANCELLED && !cancelled);
    cancelled = 1;
  }
  CHECK(coakka_http_host_accept_websocket(service, a, bytes("echo")) ==
        COAKKA_HTTP_HOST_STALE_EXCHANGE);
  marker("A_CLOSED");
  // A must not produce OPEN or CLOSE. The next WebSocket event, apart
  // from a writable notification, must be B's text on the same connection.
  check_live_sibling(service, b);
}

static void check_drain(coakka_http_host_service_t* service, const char* mode) {
  const coakka_http_host_websocket_t session = accept_ws(service, "/a");
  coakka_http_host_websocket_event_t held =
      websocket_event(service, COAKKA_HTTP_HOST_WEBSOCKET_TEXT);
  CHECK(same_session(held.session, session) && equals(held.data, "held"));
  CHECK(coakka_http_host_service_begin_drain(service) == COAKKA_HTTP_HOST_OK);
  coakka_http_host_drain_state_t state = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
  CHECK(coakka_http_host_service_wait_drained(service, 1U, &state) ==
        COAKKA_HTTP_HOST_TIMEOUT);
  CHECK(state == COAKKA_HTTP_HOST_DRAIN_PENDING);
  marker("DRAIN_PENDING");
  if (strcmp(mode, "aborted") == 0) {
    barrier("STOP\n");
    CHECK(coakka_http_host_service_stop(service) == COAKKA_HTTP_HOST_OK);
    CHECK(coakka_http_host_service_wait_drained(service, 1000U, &state) ==
          COAKKA_HTTP_HOST_CLOSED);
    CHECK(state == COAKKA_HTTP_HOST_DRAIN_ABORTED);
  } else if (strcmp(mode, "expired") == 0) {
    CHECK(coakka_http_host_service_wait_drained(service, 5000U, &state) ==
          COAKKA_HTTP_HOST_TIMEOUT);
    CHECK(state == COAKKA_HTTP_HOST_DRAIN_EXPIRED);
  } else {
    CHECK(coakka_http_host_service_wait_drained(service, 5000U, &state) ==
          COAKKA_HTTP_HOST_OK);
    CHECK(state == COAKKA_HTTP_HOST_DRAIN_COMPLETE);
  }
  coakka_http_host_websocket_event_t closed =
      websocket_event(service, COAKKA_HTTP_HOST_WEBSOCKET_CLOSE);
  CHECK(same_session(closed.session, session));
  CHECK(closed.close_code == (strcmp(mode, "complete") == 0 ? 1000U : 1006U));
  CHECK(coakka_http_host_release_websocket(service, &closed) ==
        COAKKA_HTTP_HOST_OK);
  CHECK(equals(held.data, "held"));
  CHECK(coakka_http_host_service_destroy(&service) != COAKKA_HTTP_HOST_OK &&
        service != NULL);
  CHECK(coakka_http_host_release_websocket(service, &held) ==
        COAKKA_HTTP_HOST_OK);
}

struct waiter {
  coakka_http_host_service_t* service;
  coakka_http_host_status_t result;
  coakka_http_host_websocket_event_t event;
};

struct shutdown_worker {
  struct waiter* waiter;
  int stop;
};

static int run_shutdown_worker(void* opaque) {
  const struct shutdown_worker* worker = opaque;
  struct waiter* waiter = worker->waiter;
  if (worker->stop != 0) {
    marker("HELD");
    barrier("STOP\n");
    CHECK(coakka_http_host_service_stop(waiter->service) ==
          COAKKA_HTTP_HOST_OK);
    return 0;
  }
  coakka_http_host_websocket_event_init(&waiter->event);
  waiter->result =
      coakka_http_host_take_websocket(waiter->service, 4000U, &waiter->event);
  return 0;
}

static void check_shutdown(coakka_http_host_service_t* service) {
  const coakka_http_host_websocket_t session = accept_ws(service, "/a");
  coakka_http_host_websocket_event_t held =
      websocket_event(service, COAKKA_HTTP_HOST_WEBSOCKET_TEXT);
  CHECK(same_session(held.session, session) && equals(held.data, "held"));
  struct waiter waiter = {.service = service};
  // The service and its retained lease outlive both workers. Only the reader
  // writes its result, and the controller inspects it after both joins.
  struct shutdown_worker reader = {&waiter, 0};
  struct shutdown_worker controller = {&waiter, 1};
  void* workers[] = {&reader, &controller};
  CHECK(coakka_http_test_run_threads(run_shutdown_worker, workers, 2U) == 0);
  if (waiter.result == COAKKA_HTTP_HOST_OK) {
    CHECK(waiter.event.kind == COAKKA_HTTP_HOST_WEBSOCKET_CLOSE);
    CHECK(same_session(waiter.event.session, session));
    CHECK(waiter.event.close_code == 1006U);
    CHECK(coakka_http_host_release_websocket(service, &waiter.event) ==
          COAKKA_HTTP_HOST_OK);
  } else {
    CHECK(waiter.result == COAKKA_HTTP_HOST_CLOSED);
    coakka_http_host_websocket_event_t closed =
        websocket_event(service, COAKKA_HTTP_HOST_WEBSOCKET_CLOSE);
    CHECK(closed.close_code == 1006U);
    CHECK(coakka_http_host_release_websocket(service, &closed) ==
          COAKKA_HTTP_HOST_OK);
  }
  CHECK(coakka_http_host_service_stop(service) == COAKKA_HTTP_HOST_OK);
  CHECK(equals(held.data, "held"));
  CHECK(coakka_http_host_service_destroy(&service) != COAKKA_HTTP_HOST_OK &&
        service != NULL);
  CHECK(coakka_http_host_release_websocket(service, &held) ==
        COAKKA_HTTP_HOST_OK);
}

int main(int argc, char** argv) {
  CHECK(argc == 4 || (argc == 5 && strcmp(argv[4], "--io-uring") == 0));
  CHECK(test_platform_open());
  CHECK(coakka_http_host_abi_version() == 10U);
  const int ordinary_only = strcmp(argv[3], "ordinary") == 0;
  const int ordinary =
      ordinary_only || strcmp(argv[3], "ordinary-with-ws") == 0;
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  config.bind_address = bytes("127.0.0.1");
  config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_3;
  config.use_io_uring = argc == 5 ? 1U : 0U;
  config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
  config.certificate_chain_file = bytes(argv[1]);
  config.private_key_file = bytes(argv[2]);
  config.credential_id = bytes("http3-websocket-test");
  config.credential_generation = 1U;
  config.limits.drain_timeout_ms = 1500U;
  const char* paths[] = {"/a", "/b", "/plain"};
  coakka_http_host_route_t routes[3];
  for (unsigned index = 0; index < 3; ++index) {
    coakka_http_host_route_init(&routes[index]);
    routes[index].route_id = index + 1U;
    routes[index].handler_binding_id = index + 100U;
    routes[index].method = bytes("GET");
    routes[index].encoded_path_pattern = bytes(paths[index]);
    routes[index].flags =
        !ordinary_only && index < 2U ? COAKKA_HTTP_HOST_ROUTE_WEBSOCKET : 0U;
  }
  coakka_http_host_service_t* service = NULL;
  coakka_http_host_issue_t issue;
  coakka_http_host_issue_init(&issue);
  uint32_t failed = COAKKA_HTTP_HOST_FAILED_INDEX_NONE;
  if (!ordinary_only) {
    coakka_http_host_configuration_t mutual = config;
    mutual.security = COAKKA_HTTP_HOST_SECURITY_MUTUAL_TLS;
    mutual.trust_roots_file = config.certificate_chain_file;
    CHECK(coakka_http_host_service_create(&mutual, routes, 3U, &service,
                                          &failed) ==
          COAKKA_HTTP_HOST_INVALID_ARGUMENT);
    CHECK(service == NULL);
  }
  const coakka_http_host_status_t created =
      coakka_http_host_service_create(&config, routes, 3U, &service, &failed);
  if (created != COAKKA_HTTP_HOST_OK) {
    fprintf(stderr, "service_create status=%d failed_route=%u\n", (int)created,
            (unsigned)failed);
  }
  CHECK(created == COAKKA_HTTP_HOST_OK);
  CHECK(coakka_http_host_service_start(service, &issue) == COAKKA_HTTP_HOST_OK);
  coakka_http_host_service_info_t backend;
  coakka_http_host_service_info_init(&backend);
  CHECK(coakka_http_host_service_info(service, &backend) ==
        COAKKA_HTTP_HOST_OK);
  printf(
      "IO_URING requested=%u compiled=%u probed=%u supported=%u "
      "effective=%u fallback=%u probe_error=%d\n",
      (unsigned)backend.io_uring_requested, (unsigned)backend.io_uring_compiled,
      (unsigned)backend.io_uring_probed, (unsigned)backend.io_uring_supported,
      (unsigned)backend.io_uring_effective,
      (unsigned)backend.io_fallback_reason, (int)backend.io_uring_probe_error);
  CHECK(backend.io_uring_requested == config.use_io_uring);
  CHECK(backend.io_uring_effective == config.use_io_uring);
  CHECK(backend.io_fallback_reason == COAKKA_HTTP_HOST_IO_FALLBACK_NONE);
  if (config.use_io_uring != 0U) {
    CHECK(backend.io_uring_compiled != 0U && backend.io_uring_probed != 0U &&
          backend.io_uring_supported != 0U);
  }

  if (!ordinary_only) {
    coakka_http_host_tls_reload_request_t reload;
    coakka_http_host_tls_reload_request_init(&reload);
    reload.security = config.security;
    reload.expected_generation = config.credential_generation;
    reload.credential_generation = config.credential_generation + 1U;
    reload.credential_id = config.credential_id;
    reload.certificate_chain_file = config.certificate_chain_file;
    reload.private_key_file = config.private_key_file;
    coakka_http_host_tls_reload_outcome_t outcome;
    coakka_http_host_tls_reload_outcome_init(&outcome);
    CHECK(coakka_http_host_tls_reload(service, &reload, &outcome) ==
          COAKKA_HTTP_HOST_UNSUPPORTED);
    coakka_http_host_tls_credentials_t credentials;
    coakka_http_host_tls_credentials_init(&credentials);
    CHECK(coakka_http_host_tls_credentials(service, &credentials) ==
          COAKKA_HTTP_HOST_OK);
    CHECK(credentials.generation == config.credential_generation &&
          credentials.preparing == 0U);
  }
  CHECK(coakka_http_host_service_port(service, &config.port) ==
            COAKKA_HTTP_HOST_OK &&
        config.port != 0U);
  printf("PORT=%u\n", (unsigned)config.port);
  fflush(stdout);
  if (ordinary) {
    for (unsigned index = 0; index < 8U; ++index) {
      ordinary_response(service);
    }
    barrier("WIRE_CLOSED\n");
  } else if (strncmp(argv[3], "reset-pre-accept",
                     sizeof("reset-pre-accept") - 1U) == 0) {
    check_pre_accept_reset(service);
  } else if (strncmp(argv[3], "reset", 5U) == 0) {
    const int pending_write =
        strncmp(argv[3], "reset-pending-write",
                sizeof("reset-pending-write") - 1U) == 0;
    check_reset(service, pending_write);
  } else if (strcmp(argv[3], "shutdown") == 0) {
    check_shutdown(service);
  } else {
    check_drain(service, argv[3]);
  }
  CHECK(coakka_http_host_service_stop(service) == COAKKA_HTTP_HOST_OK);
  CHECK(coakka_http_host_service_destroy(&service) == COAKKA_HTTP_HOST_OK);
  CHECK(service == NULL);
  CHECK(coakka_http_host_service_create(&config, routes, 3U, &service,
                                        &failed) == COAKKA_HTTP_HOST_OK);
  CHECK(coakka_http_host_service_start(service, &issue) == COAKKA_HTTP_HOST_OK);
  CHECK(coakka_http_host_service_stop(service) == COAKKA_HTTP_HOST_OK);
  CHECK(coakka_http_host_service_destroy(&service) == COAKKA_HTTP_HOST_OK);
  CHECK(test_platform_close());
  marker("PASS PORT_REUSED");
  return EXIT_SUCCESS;
}
