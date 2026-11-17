#include "test_support.h"

#include <stdlib.h>
#include <string.h>

static int same(coakka_http_host_exchange_t a, coakka_http_host_exchange_t b) {
  return a.slot == b.slot && a.generation == b.generation;
}

static void mark(const char *text) {
  puts(text);
  fflush(stdout);
}

static int command(const char *expected) {
  char line[32];
  if (fgets(line, sizeof(line), stdin) == NULL) return 0;
  line[strcspn(line, "\r\n")] = '\0';
  return strcmp(line, expected) == 0;
}

static int take_head(test_service_t *owner, const char *path,
                     coakka_http_host_exchange_t *exchange) {
  coakka_http_host_request_view_t view;
  coakka_http_host_request_view_init(&view);
  if (test_take(owner, 4000U) != COAKKA_HTTP_HOST_OK ||
      owner->event.kind != COAKKA_HTTP_HOST_REQUEST ||
      coakka_http_host_request_view(&owner->event, &view) != COAKKA_HTTP_HOST_OK ||
      !test_bytes_equal(view.target, path)) return 0;
  *exchange = owner->event.exchange;
  return 1;
}

static int late_response(test_service_t *owner, coakka_http_host_exchange_t exchange) {
  coakka_http_host_response_t response;
  coakka_http_host_response_init(&response);
  response.status_code = 200U;
  return coakka_http_host_respond(owner->service, exchange, &response) ==
         COAKKA_HTTP_HOST_STALE_EXCHANGE;
}

static int observe(test_service_t *owner, coakka_http_host_exchange_t first,
                   coakka_http_host_exchange_t sibling, int reset) {
  int ok = 1;
  unsigned terminal = 0U, cancelled = 0U, sibling_terminal = 0U;
  uint64_t end = 0U;
  TEST_CHECK(test_now_ms(&end));
  end += 5000U;
  while (terminal + cancelled + sibling_terminal < (reset ? 3U : 2U)) {
    uint64_t now = 0U;
    TEST_CHECK(test_now_ms(&now) && now < end);
    TEST_CHECK(test_take(owner, end - now) == COAKKA_HTTP_HOST_OK);
    if (same(owner->event.exchange, first)) {
      switch (owner->event.kind) {
      case COAKKA_HTTP_HOST_REQUEST_TERMINAL: TEST_CHECK(++terminal == 1U); break;
      case COAKKA_HTTP_HOST_REQUEST_CANCELLED: TEST_CHECK(reset && ++cancelled == 1U); break;
      case COAKKA_HTTP_HOST_RESPONSE_WRITABLE: TEST_CHECK(!reset); break;
      default: TEST_CHECK(0);
      }
    } else {
      TEST_CHECK(same(owner->event.exchange, sibling) &&
                 owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
                 ++sibling_terminal == 1U);
    }
    TEST_CHECK(test_release(owner));
  }
  TEST_CHECK(test_take(owner, 50U) == COAKKA_HTTP_HOST_TIMEOUT);
cleanup:
  return ok;
}

static int timeout_reason(test_service_t *owner, coakka_http_host_exchange_t exchange) {
  coakka_http_host_monitor_event_t events[16] = {0};
  coakka_http_host_monitor_page_t page;
  coakka_http_host_monitor_page_init(&page);
  for (unsigned i = 0U; i < 16U; ++i) {
    coakka_http_host_monitor_event_init(&events[i]);
  }
  if (coakka_http_host_monitor_read(owner->service, 0U, 16U, events, 16U, &page) !=
          COAKKA_HTTP_HOST_OK || page.remaining_events != 0U) return 0;
  unsigned timed_out = 0U;
  /* ABI 10 projects the published failure reason as a numeric value. */
  const uint32_t response_write_timeout = 603U;
  for (uint32_t i = 0U; i < page.count; ++i) {
    if (!same(events[i].exchange, exchange)) continue;
    if (events[i].kind == COAKKA_HTTP_HOST_EVENT_EXCHANGE_TIMED_OUT &&
        events[i].failure_reason == response_write_timeout) ++timed_out;
  }
  fprintf(stderr, "RESPONSE_WRITE_TIMEOUT events: %u\n", timed_out);
  return timed_out == 1U;
}

static int upload_reset(test_service_t *owner, int hold_data) {
  int ok = 1;
  coakka_http_host_exchange_t upload, sibling;
  coakka_http_host_request_event_t held;
  coakka_http_host_request_event_init(&held);
  TEST_CHECK(take_head(owner, "/upload", &upload) && test_release(owner));
  mark("UPLOAD");
  TEST_CHECK(take_head(owner, "/sibling", &sibling) && test_release(owner));
  mark("SIBLING");
  if (hold_data) {
    TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
               same(owner->event.exchange, upload) &&
               owner->event.kind == COAKKA_HTTP_HOST_REQUEST_DATA &&
               test_bytes_equal(owner->event.data, "held"));
    held = owner->event;
    coakka_http_host_request_event_init(&owner->event);
    owner->lease_live = 0;
  }
  mark("RESET");
  TEST_CHECK(command("reset-sent"));
  coakka_http_host_response_t response;
  coakka_http_host_response_init(&response);
  response.status_code = 200U;
  response.body = test_text("sibling-ok");
  TEST_CHECK(coakka_http_host_respond(owner->service, sibling, &response) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(observe(owner, upload, sibling, 1));
  TEST_CHECK(late_response(owner, upload));
  if (hold_data) {
    TEST_CHECK(test_bytes_equal(held.data, "held"));
    TEST_CHECK(coakka_http_host_release_request(owner->service, &held) == COAKKA_HTTP_HOST_OK);
    coakka_http_host_request_event_init(&held);
  }
  mark("RECOVERED");
  TEST_CHECK(take_head(owner, "/sibling", &sibling) &&
             test_respond(owner, 200U, "next-ok") && test_release(owner));
  TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
             same(owner->event.exchange, sibling) &&
             owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
             test_release(owner));
cleanup:
  if (held.kind != 0U) {
    (void)coakka_http_host_release_request(owner->service, &held);
  }
  return ok;
}

static int upload_reset_followup(test_service_t *owner, int shared) {
  int ok = 1;
  coakka_http_host_exchange_t upload = {0}, sibling, next;
  if (!shared) {
    TEST_CHECK(take_head(owner, "/upload", &upload) && test_release(owner));
    mark("UPLOAD");
  }
  TEST_CHECK(take_head(owner, "/sibling", &sibling) && test_release(owner));
  mark("SIBLING");
  if (!shared) {
    TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
               same(owner->event.exchange, upload) &&
               owner->event.kind == COAKKA_HTTP_HOST_REQUEST_DATA &&
               test_bytes_equal(owner->event.data, "held") && test_release(owner));
  }
  mark("RESET");
  TEST_CHECK(command("reset-sent"));
  // The incomplete buffered upload on the shared listener never dispatches.
  unsigned a_terminal = shared ? 1U : 0U, b_terminal = 0U;
  for (unsigned step = 0U; !shared; ++step) {
    TEST_CHECK(step < 2U && test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
               same(upload, owner->event.exchange));
    const uint32_t event = owner->event.kind;
    TEST_CHECK(test_release(owner));
    if (event == COAKKA_HTTP_HOST_REQUEST_CANCELLED) break;
    TEST_CHECK(event == COAKKA_HTTP_HOST_REQUEST_TERMINAL && ++a_terminal == 1U);
  }
  coakka_http_host_response_t response;
  coakka_http_host_response_init(&response);
  response.status_code = 200U;
  response.body = test_text("sibling-ok");
  TEST_CHECK(coakka_http_host_respond(owner->service, sibling, &response) == COAKKA_HTTP_HOST_OK);
  /* C is sent as soon as the peer sees B's reply, before the consumer
     takes any remaining terminal events. Only the test reader pauses. */
  mark("RECOVERED");
  TEST_CHECK(command("early-sent"));
  for (unsigned step = 0U; ; ++step) {
    TEST_CHECK(step < 3U && test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK);
    if (owner->event.kind == COAKKA_HTTP_HOST_REQUEST) {
      coakka_http_host_request_view_t view;
      coakka_http_host_request_view_init(&view);
      TEST_CHECK(coakka_http_host_request_view(&owner->event, &view) == COAKKA_HTTP_HOST_OK &&
                 test_bytes_equal(view.target, "/sibling"));
      next = owner->event.exchange;
      mark("FOLLOW-UP");
      TEST_CHECK(command("respond"));
      TEST_CHECK(test_respond(owner, 200U, "next-ok") && test_release(owner));
      break;
    }
    TEST_CHECK(owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL);
    if (same(upload, owner->event.exchange)) TEST_CHECK(++a_terminal == 1U);
    else TEST_CHECK(same(sibling, owner->event.exchange) && ++b_terminal == 1U);
    TEST_CHECK(test_release(owner));
  }
  for (unsigned c_terminal = 0U; a_terminal + b_terminal + c_terminal < 3U; ) {
    TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
               owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL);
    if (same(upload, owner->event.exchange)) TEST_CHECK(++a_terminal == 1U);
    else if (same(sibling, owner->event.exchange)) TEST_CHECK(++b_terminal == 1U);
    else TEST_CHECK(same(next, owner->event.exchange) && ++c_terminal == 1U);
    TEST_CHECK(test_release(owner));
  }
  TEST_CHECK(shared || late_response(owner, upload));
  TEST_CHECK(test_take(owner, 50U) == COAKKA_HTTP_HOST_TIMEOUT);
  if (shared) {
    mark("TCP_READY");
    for (unsigned i = 0U; i < 2U; ++i) {
      TEST_CHECK(take_head(owner, "/sibling", &sibling) &&
                 test_respond(owner, 200U, "tcp-ok") && test_release(owner));
      TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
                 owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
                 same(owner->event.exchange, sibling) && test_release(owner));
    }
  }
  mark("DRAIN_READY");
  TEST_CHECK(command("drain"));
  TEST_CHECK(coakka_http_host_service_begin_drain(owner->service) == COAKKA_HTTP_HOST_OK);
  coakka_http_host_drain_state_t state = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
  TEST_CHECK(coakka_http_host_service_wait_drained(owner->service, 5000U, &state) ==
                 COAKKA_HTTP_HOST_OK &&
             state == COAKKA_HTTP_HOST_DRAIN_COMPLETE);
  mark("DRAINED");
cleanup:
  return ok;
}

static int open_listener(test_service_t *owner,
                         const coakka_http_host_configuration_t *config,
                         const coakka_http_host_route_t *routes, int shared) {
  if (!shared) return test_service_open(owner, config, routes, 2U);
  coakka_http_host_multi_listener_api_v1_t api = {0};
  api.struct_size = sizeof(api);
  uint32_t failed_index = COAKKA_HTTP_HOST_FAILED_INDEX_NONE;
  coakka_http_host_issue_t issue;
  coakka_http_host_issue_init(&issue);
  return coakka_http_host_multi_listener_get_api_v1(&api) == COAKKA_HTTP_HOST_OK &&
         api.create(config, routes, 2U, &owner->service, &failed_index) == COAKKA_HTTP_HOST_OK &&
         coakka_http_host_service_start(owner->service, &issue) == COAKKA_HTTP_HOST_OK;
}

static const uint8_t multipart[] =
    "--local-form\r\nContent-Disposition: form-data; name=\"file\"; filename=\"a.bin\"\r\n"
    "Content-Type: application/octet-stream\r\n\r\na\0b\r\n--local-form--\r\n";

static int multipart_reset(test_service_t *owner) {
  int ok = 1;
  coakka_http_host_exchange_t upload;
  coakka_http_host_request_event_t held;
  coakka_http_host_request_event_init(&held);
  TEST_CHECK(take_head(owner, "/upload", &upload));
  int header_seen = 0;
  for (uint32_t i = 0U; i < coakka_http_host_request_header_count(&owner->event); ++i) {
    coakka_http_host_header_t header;
    TEST_CHECK(coakka_http_host_request_header(&owner->event, i, &header));
    if (test_bytes_equal(header.name, "content-type")) {
      TEST_CHECK(test_bytes_equal(header.value, "multipart/form-data; boundary=local-form"));
      header_seen = 1;
    }
  }
  TEST_CHECK(header_seen && test_release(owner));
  mark("UPLOAD");
  TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
             same(owner->event.exchange, upload) &&
             owner->event.kind == COAKKA_HTTP_HOST_REQUEST_DATA);
  TEST_CHECK(owner->event.data.size == 31U &&
             memcmp(owner->event.data.data, multipart, 31U) == 0);
  held = owner->event;
  coakka_http_host_request_event_init(&owner->event);
  owner->lease_live = 0;
  mark("RESET");

  unsigned cancelled = 0U, terminal = 0U;
  for (unsigned i = 0U; i < 2U; ++i) {
    TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
               same(upload, owner->event.exchange));
    if (owner->event.kind == COAKKA_HTTP_HOST_REQUEST_CANCELLED) {
      ++cancelled;
    } else {
      TEST_CHECK(owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL);
      ++terminal;
    }
    TEST_CHECK(test_release(owner));
  }
  TEST_CHECK(cancelled == 1U && terminal == 1U && late_response(owner, upload));
  TEST_CHECK(held.data.size == 31U && memcmp(held.data.data, multipart, 31U) == 0);
  TEST_CHECK(coakka_http_host_release_request(owner->service, &held) == COAKKA_HTTP_HOST_OK);
  coakka_http_host_request_event_init(&held);
  mark("RECOVERED");

  TEST_CHECK(take_head(owner, "/upload", &upload) && test_release(owner));
  size_t offset = 0U;
  for (;;) {
    TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
               same(upload, owner->event.exchange));
    if (owner->event.kind == COAKKA_HTTP_HOST_REQUEST_END) {
      TEST_CHECK(test_release(owner));
      break;
    }
    TEST_CHECK(owner->event.kind == COAKKA_HTTP_HOST_REQUEST_DATA &&
               owner->event.data.size > 0U &&
               owner->event.data.size <= sizeof(multipart) - 1U - offset);
    TEST_CHECK(memcmp(owner->event.data.data, multipart + offset, owner->event.data.size) == 0);
    offset += owner->event.data.size;
    TEST_CHECK(test_release(owner));
  }
  TEST_CHECK(offset == sizeof(multipart) - 1U);
  coakka_http_host_response_t response;
  coakka_http_host_response_init(&response);
  response.status_code = 200U;
  response.body = test_text("upload-recovered");
  TEST_CHECK(coakka_http_host_respond(owner->service, upload, &response) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
             owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
             same(owner->event.exchange, upload) && test_release(owner));
  TEST_CHECK(test_take(owner, 25U) == COAKKA_HTTP_HOST_TIMEOUT);
cleanup:
  if (held.kind != 0U) {
    (void)coakka_http_host_release_request(owner->service, &held);
  }
  return ok;
}

static int file_drain(test_service_t *owner) {
  int ok = 1;
  coakka_http_host_exchange_t file;
  coakka_http_host_drain_state_t state = COAKKA_HTTP_HOST_DRAIN_NOT_STARTED;
  TEST_CHECK(take_head(owner, "/download", &file) && test_release(owner));
  coakka_http_host_file_response_t response;
  coakka_http_host_file_response_init(&response);
  response.head.status_code = 200U;
  response.authority_id = 1U;
  response.encoded_path = test_text("/asset.bin");
  TEST_CHECK(coakka_http_host_respond_file(owner->service, file, &response) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(command("drain"));
  TEST_CHECK(coakka_http_host_service_begin_drain(owner->service) == COAKKA_HTTP_HOST_OK);
  TEST_CHECK(coakka_http_host_service_wait_drained(owner->service, 50U, &state) ==
                 COAKKA_HTTP_HOST_TIMEOUT && state == COAKKA_HTTP_HOST_DRAIN_PENDING);
  mark("PENDING");
  TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
             owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
             same(owner->event.exchange, file) && test_release(owner));
  TEST_CHECK(coakka_http_host_service_wait_drained(owner->service, 4000U, &state) ==
                 COAKKA_HTTP_HOST_OK && state == COAKKA_HTTP_HOST_DRAIN_COMPLETE);
  const coakka_http_host_status_t next = test_take(owner, 0U);
  TEST_CHECK(next == COAKKA_HTTP_HOST_TIMEOUT || next == COAKKA_HTTP_HOST_CLOSED);
  mark("COMPLETE");
cleanup:
  return ok;
}

static int writer_timeout(test_service_t *owner) {
  int ok = 1;
  coakka_http_host_exchange_t blocked, sibling;
  TEST_CHECK(take_head(owner, "/stream", &blocked) && test_release(owner));
  coakka_http_host_response_t head;
  coakka_http_host_response_init(&head);
  head.status_code = 200U;
  TEST_CHECK(coakka_http_host_start_response_stream(owner->service, blocked, &head) ==
             COAKKA_HTTP_HOST_OK);
  TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
             same(owner->event.exchange, blocked) &&
             owner->event.kind == COAKKA_HTTP_HOST_RESPONSE_WRITABLE &&
             test_release(owner));
  TEST_CHECK(command("write"));
  static uint8_t data[4096];
  memset(data, 'w', sizeof(data));
  const coakka_http_host_bytes_t payload = {data, sizeof(data)};
  TEST_CHECK(coakka_http_host_write_response_stream(owner->service, blocked, payload) ==
             COAKKA_HTTP_HOST_OK);
  mark("WRITING");
  TEST_CHECK(take_head(owner, "/sibling", &sibling) &&
             test_respond(owner, 200U, "sibling-ok") && test_release(owner));
  TEST_CHECK(observe(owner, blocked, sibling, 0));
  TEST_CHECK(timeout_reason(owner, blocked));
  TEST_CHECK(coakka_http_host_write_response_stream(owner->service, blocked, payload) ==
                 COAKKA_HTTP_HOST_STALE_EXCHANGE &&
             coakka_http_host_finish_response_stream(owner->service, blocked, NULL, 0U) ==
                 COAKKA_HTTP_HOST_STALE_EXCHANGE);
  mark("WRITE_TIMEOUT");
  TEST_CHECK(take_head(owner, "/sibling", &sibling) &&
             test_respond(owner, 200U, "next-ok") && test_release(owner));
  TEST_CHECK(test_take(owner, 4000U) == COAKKA_HTTP_HOST_OK &&
             same(owner->event.exchange, sibling) &&
             owner->event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
             test_release(owner));
cleanup:
  return ok;
}

int main(int argc, char **argv) {
  if ((argc != 6 && argc != 7) || coakka_http_host_abi_version() != 10U)
    return EXIT_FAILURE;
  const int before = strcmp(argv[1], "reset-before-data") == 0;
  const int held = strcmp(argv[1], "reset-held-data") == 0;
  const int shared = strcmp(argv[1], "reset-data-followup-shared") == 0;
  const int follow = shared || strcmp(argv[1], "reset-data-followup") == 0;
  const int file = strcmp(argv[1], "file-drain") == 0;
  const int timeout = strcmp(argv[1], "write-timeout") == 0;
  const int h3 = strcmp(argv[2], "3") == 0;
  const int h1 = strcmp(argv[2], "1.1") == 0;
  const int multipart_case = strcmp(argv[1], "multipart-reset") == 0;
  unsigned long fixed_port = 0UL;
  if (h1) {
    if (argc != 7) return EXIT_FAILURE;
    char *end = NULL;
    fixed_port = strtoul(argv[6], &end, 10);
    if (*end != '\0' || fixed_port == 0UL || fixed_port > UINT16_MAX)
      return EXIT_FAILURE;
  } else if (argc != 6) {
    return EXIT_FAILURE;
  }
  if ((!before && !held && !follow && !file && !timeout && !multipart_case) ||
      (follow && !h3) ||
      (!h3 && !(h1 && multipart_case) && strcmp(argv[2], "2") != 0) || !test_platform_open()) return EXIT_FAILURE;

  int ok = 1;
  test_service_t owner;
  test_service_init(&owner);
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  config.bind_address = test_text("127.0.0.1");
  if (h1) {
    config.port = (uint16_t)fixed_port;
    config.protocol = COAKKA_HTTP_HOST_PROTOCOL_HTTP_1_1;
  } else {
    config.protocol = shared ? COAKKA_HTTP_HOST_PROTOCOL_HTTP_1_1
                             : (h3 ? COAKKA_HTTP_HOST_PROTOCOL_HTTP_3
                                   : COAKKA_HTTP_HOST_PROTOCOL_HTTP_2);
    config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
    config.credential_id = test_text("lifecycle-interactions");
    config.credential_generation = 1U;
    config.certificate_chain_file = test_text(argv[3]);
    config.private_key_file = test_text(argv[4]);
  }
  config.limits.drain_timeout_ms = 4000U;
  if (follow) {
    config.limits.max_connections = 64U;
    config.limits.max_active_requests = shared ? 48U : 16U;
    config.limits.request_queue_capacity = config.limits.max_active_requests;
    config.limits.handler_timeout_ms = 7000U;
  }
  config.limits.response_write_timeout_ms = timeout ? 900U : 0U;
  if (timeout) {
    config.monitor.event_capacity = 16U;
    config.monitor.max_events_per_read = 16U;
    config.monitor.categories = COAKKA_HTTP_HOST_MONITOR_EXCHANGE;
    config.monitor.initial_collection = COAKKA_HTTP_HOST_MONITOR_AGGREGATES_AND_EVENTS;
  }

  coakka_http_host_file_authority_t authority;
  coakka_http_host_file_authority_init(&authority);
  if (file) {
    authority.authority_id = 1U;
    authority.root_path = test_text(argv[5]);
    authority.max_active_files = 1U;
    authority.max_file_bytes = 65536U;
    config.file_authorities = &authority;
    config.file_authority_count = 1U;
  }

  coakka_http_host_route_t routes[2];
  for (uint32_t i = 0U; i < 2U; ++i) {
    coakka_http_host_route_init(&routes[i]);
    routes[i].route_id = routes[i].handler_binding_id = i + 1U;
    routes[i].method = test_text("GET");
  }
  routes[0].encoded_path_pattern = test_text("/sibling");
  routes[1].encoded_path_pattern = test_text(file ? "/download" : (timeout ? "/stream" : "/upload"));
  if (before || held || follow || multipart_case) {
    routes[1].method = test_text("POST");
    if (!shared) routes[1].flags = COAKKA_HTTP_HOST_ROUTE_STREAMING_REQUEST;
  }

  TEST_CHECK(open_listener(&owner, &config, routes, shared));
  uint16_t port = 0U;
  TEST_CHECK(coakka_http_host_service_port(owner.service, &port) == COAKKA_HTTP_HOST_OK);
  printf("READY %u\n", (unsigned)port);
  fflush(stdout);
  if (file) {
    TEST_CHECK(file_drain(&owner));
  } else if (multipart_case) {
    TEST_CHECK(multipart_reset(&owner));
  } else if (timeout) {
    TEST_CHECK(writer_timeout(&owner));
  } else if (follow) {
    TEST_CHECK(upload_reset_followup(&owner, shared));
  } else {
    TEST_CHECK(upload_reset(&owner, held));
  }
  TEST_CHECK(command("finish"));
  if (follow) {
    TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);
    TEST_CHECK(coakka_http_host_service_stop(owner.service) == COAKKA_HTTP_HOST_OK);
  }
  TEST_CHECK(test_service_close(&owner));
  config.port = port;
  TEST_CHECK(open_listener(&owner, &config, routes, shared));
  uint16_t reopened = 0U;
  TEST_CHECK(coakka_http_host_service_port(owner.service, &reopened) == COAKKA_HTTP_HOST_OK &&
             reopened == port);
  printf("REBOUND %u\n", (unsigned)reopened);
cleanup:
  return test_finish(argv[1], ok, test_service_close(&owner));
}
