package coakkahttp

import (
	"errors"
	"fmt"
	"net/http"
	"net/url"
	"strings"
	"sync"
	"sync/atomic"
)

type Handler func(*Request) (Response, error)
type WebSocketHandler func(*WebSocketSession, *Request) error

type routeDeclaration struct {
	id       uint64
	method   string
	pattern  string
	handler  Handler
	upgraded WebSocketHandler
}

type routeSegment struct {
	literal string
	capture string
}

type compiledRoute struct {
	declaration routeDeclaration
	segments    []routeSegment
	revision    uint64
}

type routeTable struct {
	generation uint64
	routes     []compiledRoute
}

type routeOwner struct {
	mu    sync.Mutex
	table atomic.Pointer[routeTable]
}

type RouteBinding struct {
	RouteID    uint64
	Method     string
	Pattern    string
	Revision   uint64
	Generation uint64
}

func compileRoutes(declarations []routeDeclaration) (*routeTable, error) {
	routes := make([]compiledRoute, len(declarations))
	for index, declaration := range declarations {
		segments, err := compilePattern(declaration.pattern)
		if err != nil {
			return nil, err
		}
		routes[index] = compiledRoute{declaration: declaration, segments: segments, revision: 1}
	}
	return &routeTable{generation: 1, routes: routes}, nil
}

func compilePattern(pattern string) ([]routeSegment, error) {
	if pattern == "" || pattern[0] != '/' || strings.Contains(pattern, "//") {
		return nil, errors.New("coakka http route pattern is invalid")
	}
	if pattern == "/" {
		return nil, nil
	}
	parts := strings.Split(pattern[1:], "/")
	segments := make([]routeSegment, len(parts))
	for index, part := range parts {
		if part == "" || part == "." || part == ".." {
			return nil, errors.New("coakka http route pattern is invalid")
		}
		if strings.HasPrefix(part, "{") && strings.HasSuffix(part, "}") {
			name := part[1 : len(part)-1]
			if name == "" || strings.ContainsAny(name, "{}") {
				return nil, errors.New("coakka http route capture is invalid")
			}
			segments[index].capture = name
			continue
		}
		decoded, err := url.PathUnescape(part)
		if err != nil || strings.ContainsAny(decoded, "/{}") {
			return nil, errors.New("coakka http route literal is invalid")
		}
		segments[index].literal = decoded
	}
	return segments, nil
}

func (owner *routeOwner) bindings() []RouteBinding {
	table := owner.table.Load()
	if table == nil {
		return nil
	}
	bindings := make([]RouteBinding, len(table.routes))
	for index, route := range table.routes {
		bindings[index] = RouteBinding{
			RouteID: route.declaration.id, Method: route.declaration.method,
			Pattern: route.declaration.pattern, Revision: route.revision,
			Generation: table.generation,
		}
	}
	return bindings
}

func (owner *routeOwner) rebind(routeID, expectedRevision uint64, handler Handler) (RouteBinding, error) {
	if handler == nil {
		return RouteBinding{}, errors.New("coakka http handler is nil")
	}
	owner.mu.Lock()
	defer owner.mu.Unlock()
	current := owner.table.Load()
	if current == nil {
		return RouteBinding{}, errors.New("coakka http routes are unavailable")
	}
	next := &routeTable{generation: current.generation + 1, routes: append([]compiledRoute(nil), current.routes...)}
	for index := range next.routes {
		if next.routes[index].declaration.id != routeID {
			continue
		}
		if next.routes[index].declaration.upgraded != nil {
			return RouteBinding{}, errors.New("coakka http upgraded route needs RebindWebSocket")
		}
		if next.routes[index].revision != expectedRevision {
			return RouteBinding{}, errors.New("coakka http route revision conflict")
		}
		next.routes[index].declaration.handler = handler
		next.routes[index].revision++
		owner.table.Store(next)
		return RouteBinding{
			RouteID: routeID, Method: next.routes[index].declaration.method,
			Pattern:  next.routes[index].declaration.pattern,
			Revision: next.routes[index].revision, Generation: next.generation,
		}, nil
	}
	return RouteBinding{}, errors.New("coakka http route does not exist")
}

func (owner *routeOwner) rebindWebSocket(routeID, expectedRevision uint64, handler WebSocketHandler) (RouteBinding, error) {
	if handler == nil {
		return RouteBinding{}, errors.New("coakka http WebSocket handler is nil")
	}
	owner.mu.Lock()
	defer owner.mu.Unlock()
	current := owner.table.Load()
	if current == nil {
		return RouteBinding{}, errors.New("coakka http routes are unavailable")
	}
	next := &routeTable{generation: current.generation + 1, routes: append([]compiledRoute(nil), current.routes...)}
	for index := range next.routes {
		if next.routes[index].declaration.id != routeID {
			continue
		}
		if next.routes[index].declaration.upgraded == nil {
			return RouteBinding{}, errors.New("coakka http route is not upgraded")
		}
		if next.routes[index].revision != expectedRevision {
			return RouteBinding{}, errors.New("coakka http route revision conflict")
		}
		next.routes[index].declaration.upgraded = handler
		next.routes[index].revision++
		owner.table.Store(next)
		return RouteBinding{
			RouteID: routeID, Method: next.routes[index].declaration.method,
			Pattern:  next.routes[index].declaration.pattern,
			Revision: next.routes[index].revision, Generation: next.generation,
		}, nil
	}
	return RouteBinding{}, errors.New("coakka http route does not exist")
}

func matchRoute(table *routeTable, request *http.Request) (*compiledRoute, map[string]string) {
	if table == nil {
		return nil, nil
	}
	escaped := request.URL.EscapedPath()
	if escaped == "" || escaped[0] != '/' || strings.Contains(escaped, "//") {
		return nil, nil
	}
	var parts []string
	if escaped != "/" {
		parts = strings.Split(escaped[1:], "/")
	}
	for index := range table.routes {
		route := &table.routes[index]
		if route.declaration.method != request.Method || len(route.segments) != len(parts) {
			continue
		}
		captures := make(map[string]string)
		matched := true
		for segmentIndex, segment := range route.segments {
			decoded, err := url.PathUnescape(parts[segmentIndex])
			if err != nil || decoded == "." || decoded == ".." || strings.Contains(decoded, "/") {
				matched = false
				break
			}
			if segment.capture != "" {
				captures[segment.capture] = decoded
			} else if segment.literal != decoded {
				matched = false
				break
			}
		}
		if matched {
			return route, captures
		}
	}
	return nil, nil
}

func supportedMethod(method string) bool {
	switch method {
	case http.MethodGet, http.MethodPost, http.MethodPut, http.MethodPatch, http.MethodDelete:
		return true
	default:
		return false
	}
}

func normalizeMethod(method string) (string, error) {
	method = strings.ToUpper(strings.TrimSpace(method))
	if !supportedMethod(method) {
		return "", fmt.Errorf("coakka http method %q is unsupported", method)
	}
	return method, nil
}
