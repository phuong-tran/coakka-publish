package coakkahttp

/*
#cgo CFLAGS: -std=c11 -Wall -Wextra -Wpedantic -Werror

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct coakka_http_route {
  uint64_t route_id;
  const uint8_t *method;
  size_t method_size;
  const uint8_t *encoded_path_pattern;
  size_t encoded_path_pattern_size;
} coakka_http_route_t;

int32_t coakka_http_core_validate_routes(const coakka_http_route_t *routes,
                                         uint32_t route_count,
                                         uint32_t *failed_index);
uint32_t coakka_http_core_abi_version(void);
const char *coakka_http_core_status_name(int32_t status);
*/
import "C"

import (
	"errors"
	"fmt"
	"math"
	"unsafe"
)

type RouteValidationError struct {
	Status string
	Index  uint32
}

func (failure *RouteValidationError) Error() string {
	if failure.Index == math.MaxUint32 {
		return "coakka http route validation failed: " + failure.Status
	}
	return fmt.Sprintf("coakka http route %d validation failed: %s", failure.Index, failure.Status)
}

func validateRoutes(routes []routeDeclaration) error {
	if len(routes) == 0 || len(routes) > maximumRoutes {
		return errors.New("coakka http requires between 1 and 1024 routes")
	}
	if uint32(C.coakka_http_core_abi_version()) != 1 {
		return errors.New("coakka http Core ABI mismatch")
	}
	bytes := C.size_t(len(routes)) * C.size_t(unsafe.Sizeof(C.coakka_http_route_t{}))
	memory := C.calloc(1, bytes)
	if memory == nil {
		return errors.New("coakka http route projection allocation failed")
	}
	defer C.free(memory)
	projected := unsafe.Slice((*C.coakka_http_route_t)(memory), len(routes))
	allocations := make([]unsafe.Pointer, 0, len(routes)*2)
	defer func() {
		for _, allocation := range allocations {
			C.free(allocation)
		}
	}()
	for index, route := range routes {
		method := C.CBytes([]byte(route.method))
		if method == nil {
			return errors.New("coakka http method projection allocation failed")
		}
		allocations = append(allocations, method)
		pattern := C.CBytes([]byte(route.pattern))
		if pattern == nil {
			return errors.New("coakka http pattern projection allocation failed")
		}
		allocations = append(allocations, pattern)
		projected[index].route_id = C.uint64_t(route.id)
		projected[index].method = (*C.uint8_t)(method)
		projected[index].method_size = C.size_t(len(route.method))
		projected[index].encoded_path_pattern = (*C.uint8_t)(pattern)
		projected[index].encoded_path_pattern_size = C.size_t(len(route.pattern))
	}
	failed := C.uint32_t(math.MaxUint32)
	status := C.coakka_http_core_validate_routes(
		(*C.coakka_http_route_t)(memory), C.uint32_t(len(routes)), &failed,
	)
	if status != 0 {
		return &RouteValidationError{
			Status: C.GoString(C.coakka_http_core_status_name(status)),
			Index:  uint32(failed),
		}
	}
	return nil
}
