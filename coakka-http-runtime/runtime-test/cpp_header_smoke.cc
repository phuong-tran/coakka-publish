#include <coakka/http/http.h>

#include <cstdint>
#include <cstdlib>
#include <type_traits>

static_assert(std::is_standard_layout_v<coakka_http_result_t>);
static_assert(std::is_standard_layout_v<coakka_http_bytes_t>);
static_assert(std::is_standard_layout_v<coakka_http_header_t>);
static_assert(std::is_standard_layout_v<coakka_http_route_t>);
static_assert(std::is_standard_layout_v<coakka_http_server_options_t>);
static_assert(std::is_standard_layout_v<coakka_http_response_t>);
static_assert(
    std::is_same_v<decltype(coakka_http_abi_version()), std::uint32_t>);

int main() {
  coakka_http_result_t result{};
  coakka_http_server_options_t options{};
  coakka_http_route_t route{};
  coakka_http_response_t response{};
  coakka_http_result_init(&result);
  coakka_http_server_options_init(&options);
  coakka_http_route_init(&route);
  coakka_http_response_init(&response);
  return result.struct_size == sizeof(result) &&
                 options.struct_size == sizeof(options) &&
                 route.struct_size == sizeof(route) &&
                 response.struct_size == sizeof(response) &&
                 response.status_code == 200U
             ? EXIT_SUCCESS
             : EXIT_FAILURE;
}
