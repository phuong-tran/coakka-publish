#include <coakka/http/host.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(call)                                                            \
  do {                                                                         \
    const int result = (call);                                                 \
    fprintf(stderr, "%s = %d\n", #call, result);                               \
    if (result != COAKKA_HTTP_HOST_OK)                                         \
      return EXIT_FAILURE;                                                     \
  } while (0)

static coakka_http_host_bytes_t bytes(const char *text) {
  return (coakka_http_host_bytes_t){(const uint8_t *)text, strlen(text)};
}

int main(int argc, char **argv) {
  if (argc != 5 ||
      (strcmp(argv[1], "stream") != 0 && strcmp(argv[1], "sse") != 0) ||
      (strcmp(argv[2], "2") != 0 && strcmp(argv[2], "3") != 0) ||
      coakka_http_host_abi_version() != 10u)
    return EXIT_FAILURE;
  const int sse = strcmp(argv[1], "sse") == 0;
  coakka_http_host_configuration_t config;
  coakka_http_host_configuration_init(&config);
  config.bind_address = bytes("127.0.0.1");
  config.protocol = strcmp(argv[2], "2") == 0
                        ? COAKKA_HTTP_HOST_PROTOCOL_HTTP_2
                        : COAKKA_HTTP_HOST_PROTOCOL_HTTP_3;
  config.security = COAKKA_HTTP_HOST_SECURITY_TLS;
  config.credential_id = bytes("native-control");
  config.credential_generation = 1;
  config.certificate_chain_file = bytes(argv[3]);
  config.private_key_file = bytes(argv[4]);
  coakka_http_host_route_t route;
  coakka_http_host_route_init(&route);
  route.route_id = 1;
  route.handler_binding_id = 1;
  route.method = bytes("GET");
  route.encoded_path_pattern = bytes("/stream");
  coakka_http_host_service_t *owner = NULL;
  uint32_t failed = UINT32_MAX;
  CHECK(coakka_http_host_service_create(&config, &route, 1, &owner, &failed));
  coakka_http_host_issue_t issue;
  coakka_http_host_issue_init(&issue);
  CHECK(coakka_http_host_service_start(owner, &issue));
  uint16_t port = 0;
  CHECK(coakka_http_host_service_port(owner, &port));
  printf("READY %u\n", (unsigned)port);
  fflush(stdout);
  coakka_http_host_request_event_t event;
  coakka_http_host_request_event_init(&event);
  CHECK(coakka_http_host_take_request(owner, 5000, &event));
  if (event.kind != COAKKA_HTTP_HOST_REQUEST)
    return EXIT_FAILURE;
  coakka_http_host_exchange_t exchange = event.exchange;
  CHECK(coakka_http_host_release_request(owner, &event));

  coakka_http_host_response_t head;
  coakka_http_host_response_init(&head);
  head.status_code = 202;
  coakka_http_host_header_t header = {bytes("x-stream"), bytes("yes")};
  head.headers = &header;
  head.header_count = 1;
  if (sse) {
    CHECK(coakka_http_host_start_sse(owner, exchange, &header, 1));
  } else {
    CHECK(coakka_http_host_start_response_stream(owner, exchange, &head));
  }
  CHECK(coakka_http_host_take_request(owner, 5000, &event));
  if (event.kind != COAKKA_HTTP_HOST_RESPONSE_WRITABLE ||
      event.exchange.slot != exchange.slot ||
      event.exchange.generation != exchange.generation)
    return EXIT_FAILURE;
  CHECK(coakka_http_host_release_request(owner, &event));

  /* Separate calls cross the provider's first acknowledged-chunk boundary. */
  if (sse) {
    const char *const parts[] = {"first", "second\nline", "third"};
    for (size_t index = 0; index < sizeof(parts) / sizeof(parts[0]); ++index) {
      coakka_http_host_sse_event_t frame;
      coakka_http_host_sse_event_init(&frame);
      frame.data = bytes(parts[index]);
      if (index == 1u) {
        frame.event_type = bytes("progress");
        frame.has_event_type = 1;
        frame.id = bytes("2");
        frame.has_id = 1;
        frame.retry_ms = 1250;
        frame.has_retry = 1;
      }
      CHECK(coakka_http_host_write_sse(owner, exchange, &frame));
    }
    CHECK(coakka_http_host_finish_response_stream(owner, exchange, NULL, 0));
  } else {
    CHECK(
        coakka_http_host_write_response_stream(owner, exchange, bytes("one-")));
    CHECK(
        coakka_http_host_write_response_stream(owner, exchange, bytes("two-")));
    CHECK(coakka_http_host_write_response_stream(owner, exchange,
                                                 bytes("three")));
    coakka_http_host_header_t trailer = {bytes("x-finished"), bytes("yes")};
    CHECK(
        coakka_http_host_finish_response_stream(owner, exchange, &trailer, 1));
  }
  int terminal = 0;
  for (unsigned attempt = 0; attempt < 8u; ++attempt) {
    CHECK(coakka_http_host_take_request(owner, 5000, &event));
    terminal = event.kind == COAKKA_HTTP_HOST_REQUEST_TERMINAL &&
               event.exchange.slot == exchange.slot &&
               event.exchange.generation == exchange.generation;
    CHECK(coakka_http_host_release_request(owner, &event));
    if (terminal)
      break;
  }
  if (!terminal)
    return EXIT_FAILURE;
  puts("SENT");
  fflush(stdout);
  char input[16];
  if (fgets(input, sizeof(input), stdin) == NULL ||
      strcmp(input, "stop\n") != 0)
    return EXIT_FAILURE;
  CHECK(coakka_http_host_service_begin_drain(owner));
  uint32_t state = 0;
  CHECK(coakka_http_host_service_wait_drained(owner, 5000, &state));
  if (state != COAKKA_HTTP_HOST_DRAIN_COMPLETE)
    return EXIT_FAILURE;
  CHECK(coakka_http_host_service_stop(owner));
  CHECK(coakka_http_host_service_destroy(&owner));
  if (owner != NULL)
    return EXIT_FAILURE;
  puts("STOPPED");
  return EXIT_SUCCESS;
}
