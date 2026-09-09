package coakkahttp

import (
	"context"
	"crypto/tls"
	"errors"
	"fmt"
	"net"
	"net/http"
	"net/url"
	"strconv"
	"strings"
	"sync"
	"sync/atomic"
	"time"
)

type Protocol string

const (
	HTTP11 Protocol = "http/1.1"
	HTTP2  Protocol = "h2"
)

type TLSConfiguration struct {
	CertificateFile string
	PrivateKeyFile  string
}

type HandlerError struct {
	Status int
	Err    error
}

func (failure *HandlerError) Error() string {
	if failure == nil || failure.Err == nil {
		return "coakka http handler failed"
	}
	return failure.Err.Error()
}

func (failure *HandlerError) Unwrap() error { return failure.Err }

type Builder struct {
	host       string
	port       uint16
	bounds     Bounds
	routes     []routeDeclaration
	static     *StaticContent
	tls        *TLSConfiguration
	protocols  []Protocol
	client     *Client
	readHeader time.Duration
	idle       time.Duration
	started    bool
	err        error
}

func NewBuilder() *Builder {
	return &Builder{
		host: "127.0.0.1", bounds: DefaultBounds(), protocols: []Protocol{HTTP11},
		client: NewClient(ClientOptions{}), readHeader: 5 * time.Second,
		idle: 30 * time.Second,
	}
}

func (builder *Builder) Listen(host string, port uint16) *Builder {
	if builder == nil {
		return nil
	}
	if strings.TrimSpace(host) == "" {
		builder.err = errors.New("coakka http listener host is empty")
		return builder
	}
	builder.host, builder.port = host, port
	return builder
}

func (builder *Builder) Bounds(bounds Bounds) *Builder {
	if builder == nil {
		return nil
	}
	if err := bounds.validate(); err != nil {
		builder.err = err
		return builder
	}
	builder.bounds = bounds
	return builder
}

func (builder *Builder) Protocols(protocols ...Protocol) *Builder {
	if builder == nil {
		return nil
	}
	seen := make(map[Protocol]struct{}, len(protocols))
	for _, protocol := range protocols {
		if protocol != HTTP11 && protocol != HTTP2 {
			builder.err = errors.New("coakka http protocol is unsupported")
			return builder
		}
		seen[protocol] = struct{}{}
	}
	if len(seen) == 0 {
		builder.err = errors.New("coakka http protocol set is empty")
		return builder
	}
	builder.protocols = builder.protocols[:0]
	for _, protocol := range []Protocol{HTTP2, HTTP11} {
		if _, ok := seen[protocol]; ok {
			builder.protocols = append(builder.protocols, protocol)
		}
	}
	return builder
}

func (builder *Builder) TLS(configuration TLSConfiguration) *Builder {
	if builder == nil {
		return nil
	}
	if configuration.CertificateFile == "" || configuration.PrivateKeyFile == "" {
		builder.err = errors.New("coakka http TLS configuration is incomplete")
		return builder
	}
	copy := configuration
	builder.tls = &copy
	return builder
}

func (builder *Builder) Client(client *Client) *Builder {
	if builder == nil {
		return nil
	}
	if client == nil {
		builder.err = errors.New("coakka http client is nil")
		return builder
	}
	builder.client = client
	return builder
}

func (builder *Builder) Static(content StaticContent) *Builder {
	if builder == nil {
		return nil
	}
	if err := content.validate(); err != nil {
		builder.err = err
		return builder
	}
	copy := content
	builder.static = &copy
	return builder
}

func (builder *Builder) Handle(method, pattern string, handler Handler) *Builder {
	if builder == nil {
		return nil
	}
	if builder.started {
		builder.err = errors.New("coakka http routes are frozen")
		return builder
	}
	if handler == nil {
		builder.err = errors.New("coakka http handler is nil")
		return builder
	}
	method, err := normalizeMethod(method)
	if err != nil {
		builder.err = err
		return builder
	}
	if len(builder.routes) >= maximumRoutes {
		builder.err = errors.New("coakka http route capacity exhausted")
		return builder
	}
	builder.routes = append(builder.routes, routeDeclaration{
		id: uint64(len(builder.routes) + 1), method: method, pattern: pattern, handler: handler,
	})
	return builder
}

func (builder *Builder) Get(pattern string, handler Handler) *Builder {
	return builder.Handle(http.MethodGet, pattern, handler)
}

func (builder *Builder) Post(pattern string, handler Handler) *Builder {
	return builder.Handle(http.MethodPost, pattern, handler)
}

func (builder *Builder) Put(pattern string, handler Handler) *Builder {
	return builder.Handle(http.MethodPut, pattern, handler)
}

func (builder *Builder) Patch(pattern string, handler Handler) *Builder {
	return builder.Handle(http.MethodPatch, pattern, handler)
}

func (builder *Builder) Delete(pattern string, handler Handler) *Builder {
	return builder.Handle(http.MethodDelete, pattern, handler)
}

func (builder *Builder) WebSocket(pattern string, handler WebSocketHandler) *Builder {
	if builder == nil {
		return nil
	}
	if builder.started {
		builder.err = errors.New("coakka http routes are frozen")
		return builder
	}
	if handler == nil || len(builder.routes) >= maximumRoutes {
		builder.err = errors.New("coakka http WebSocket declaration is invalid")
		return builder
	}
	builder.routes = append(builder.routes, routeDeclaration{
		id: uint64(len(builder.routes) + 1), method: http.MethodGet,
		pattern: pattern, upgraded: handler,
	})
	return builder
}

func (builder *Builder) Start() (*Service, error) {
	if builder == nil {
		return nil, errors.New("coakka http builder is nil")
	}
	if builder.started {
		return nil, errors.New("coakka http builder can start only once")
	}
	builder.started = true
	if builder.err != nil {
		return nil, builder.err
	}
	if err := builder.bounds.validate(); err != nil {
		return nil, err
	}
	if len(builder.routes) == 0 && builder.static == nil {
		return nil, errors.New("coakka http service requires a route or static content")
	}
	if builder.tls == nil {
		for _, protocol := range builder.protocols {
			if protocol == HTTP2 {
				return nil, errors.New("coakka http HTTP/2 requires TLS")
			}
		}
	}
	if len(builder.routes) > 0 {
		if err := validateRoutes(builder.routes); err != nil {
			return nil, err
		}
	}
	table, err := compileRoutes(builder.routes)
	if err != nil {
		return nil, err
	}
	listener, err := net.Listen("tcp", net.JoinHostPort(builder.host, strconv.Itoa(int(builder.port))))
	if err != nil {
		return nil, fmt.Errorf("coakka http listen: %w", err)
	}
	rootContext, cancel := context.WithCancel(context.Background())
	service := &Service{
		listener: listener, serveDone: make(chan struct{}), errors: make([]error, 0, builder.bounds.MaxDiagnostics),
		bounds: builder.bounds, protocols: append([]Protocol(nil), builder.protocols...),
		static: builder.static, client: builder.client, cancel: cancel,
		secure:     builder.tls != nil,
		webSockets: make(map[*WebSocketSession]struct{}),
	}
	service.routes.table.Store(table)
	service.server = &http.Server{
		Handler: service, ReadHeaderTimeout: builder.readHeader, IdleTimeout: builder.idle,
		MaxHeaderBytes: builder.bounds.MaxHeaderBytes,
		BaseContext:    func(net.Listener) context.Context { return rootContext },
	}
	service.configureProtocols()
	service.phase.Store(phaseRunning)
	limited := newLimitedListener(listener, builder.bounds.MaxConnections, &service.connections, &service.rejected)
	go service.serve(limited, builder.tls)
	return service, nil
}

const (
	phaseRunning uint32 = iota + 1
	phaseDraining
	phaseStopped
)

type Service struct {
	server         *http.Server
	listener       net.Listener
	serveDone      chan struct{}
	bounds         Bounds
	protocols      []Protocol
	static         *StaticContent
	client         *Client
	secure         bool
	cancel         context.CancelFunc
	routes         routeOwner
	phase          atomic.Uint32
	dispatched     atomic.Uint64
	completed      atomic.Uint64
	rejected       atomic.Uint64
	pending        atomic.Int64
	connections    atomic.Int64
	webSocketCount atomic.Int64
	closeMu        sync.Mutex
	errorMu        sync.Mutex
	errors         []error
	webSocketMu    sync.Mutex
	webSockets     map[*WebSocketSession]struct{}
}

type Snapshot struct {
	Status            string
	Dispatched        uint64
	Completed         uint64
	Rejected          uint64
	Pending           int64
	Connections       int64
	WebSocketSessions int64
	RouteGeneration   uint64
	DiagnosticCount   int
}

type Health struct {
	Status string
	Ready  bool
}

type Inspection struct {
	Host       string
	Port       uint16
	Protocols  []Protocol
	RouteCount int
	TLS        bool
}

func (service *Service) ServeHTTP(writer http.ResponseWriter, incoming *http.Request) {
	if service == nil || service.phase.Load() != phaseRunning {
		http.Error(writer, "service unavailable", http.StatusServiceUnavailable)
		return
	}
	if len(incoming.RequestURI) > service.bounds.MaxRequestTargetBytes || headerBytes(incoming.Header) > service.bounds.MaxHeaderBytes {
		service.rejected.Add(1)
		http.Error(writer, "request rejected", http.StatusRequestHeaderFieldsTooLarge)
		return
	}
	if incoming.ContentLength > service.bounds.MaxRequestBodyBytes {
		service.rejected.Add(1)
		http.Error(writer, "request body too large", http.StatusRequestEntityTooLarge)
		return
	}
	table := service.routes.table.Load()
	route, captures := matchRoute(table, incoming)
	if route == nil {
		if service.static != nil && service.static.serve(writer, incoming, service.bounds) {
			return
		}
		http.NotFound(writer, incoming)
		return
	}
	if !service.acquireHandler() {
		service.rejected.Add(1)
		http.Error(writer, "service busy", http.StatusServiceUnavailable)
		return
	}
	service.dispatched.Add(1)
	defer func() {
		service.pending.Add(-1)
		service.completed.Add(1)
	}()
	incoming.Body = http.MaxBytesReader(writer, incoming.Body, service.bounds.MaxRequestBodyBytes)
	request := &Request{
		Method: incoming.Method, URL: cloneURL(incoming.URL), Headers: incoming.Header.Clone(),
		Params: captures, RemoteAddr: incoming.RemoteAddr, context: incoming.Context(),
		body: incoming.Body, maximum: service.bounds.MaxRequestBodyBytes,
		maxParts: service.bounds.MaxMultipartParts,
	}
	if route.declaration.upgraded != nil {
		service.serveWebSocket(writer, incoming, request, route.declaration.upgraded)
		return
	}
	tracked := &trackedWriter{ResponseWriter: writer}
	response, err := invokeHandler(route.declaration.handler, request)
	if err == nil {
		err = writeResponse(incoming.Context(), tracked, response, service.bounds)
	}
	if err != nil {
		service.report(err)
		if !tracked.wrote {
			status := http.StatusInternalServerError
			if errors.Is(err, ErrBodyLimit) {
				status = http.StatusRequestEntityTooLarge
			}
			var handlerFailure *HandlerError
			if errors.As(err, &handlerFailure) && handlerFailure.Status >= 400 && handlerFailure.Status <= 599 {
				status = handlerFailure.Status
			}
			http.Error(writer, http.StatusText(status), status)
		}
	}
}

func invokeHandler(handler Handler, request *Request) (response Response, err error) {
	defer func() {
		if recovered := recover(); recovered != nil {
			err = fmt.Errorf("coakka http handler panic: %v", recovered)
		}
	}()
	return handler(request)
}

func (service *Service) acquireHandler() bool {
	for {
		pending := service.pending.Load()
		if pending >= int64(service.bounds.MaxActiveHandlers) {
			return false
		}
		if service.pending.CompareAndSwap(pending, pending+1) {
			return true
		}
	}
}

func (service *Service) URL() string {
	if service == nil {
		return ""
	}
	scheme := "http"
	if service.secure {
		scheme = "https"
	}
	port, err := service.Port()
	if err != nil {
		return ""
	}
	return fmt.Sprintf("%s://127.0.0.1:%d", scheme, port)
}

func (service *Service) Port() (uint16, error) {
	if service == nil || service.listener == nil {
		return 0, errors.New("coakka http service is nil")
	}
	address, ok := service.listener.Addr().(*net.TCPAddr)
	if !ok || address.Port <= 0 || address.Port > 65_535 {
		return 0, errors.New("coakka http listener address is invalid")
	}
	return uint16(address.Port), nil
}

func (service *Service) Client() *Client {
	if service == nil {
		return nil
	}
	return service.client
}

func (service *Service) Routes() []RouteBinding { return service.routes.bindings() }

func (service *Service) Rebind(routeID, expectedRevision uint64, handler Handler) (RouteBinding, error) {
	if service == nil || service.phase.Load() != phaseRunning {
		return RouteBinding{}, errors.New("coakka http service is not running")
	}
	return service.routes.rebind(routeID, expectedRevision, handler)
}

func (service *Service) RebindWebSocket(routeID, expectedRevision uint64, handler WebSocketHandler) (RouteBinding, error) {
	if service == nil || service.phase.Load() != phaseRunning {
		return RouteBinding{}, errors.New("coakka http service is not running")
	}
	return service.routes.rebindWebSocket(routeID, expectedRevision, handler)
}

func (service *Service) Snapshot() Snapshot {
	if service == nil {
		return Snapshot{Status: "stopped"}
	}
	table := service.routes.table.Load()
	generation := uint64(0)
	if table != nil {
		generation = table.generation
	}
	service.errorMu.Lock()
	diagnostics := len(service.errors)
	service.errorMu.Unlock()
	return Snapshot{
		Status: phaseName(service.phase.Load()), Dispatched: service.dispatched.Load(),
		Completed: service.completed.Load(), Rejected: service.rejected.Load(), Pending: service.pending.Load(),
		Connections: service.connections.Load(), WebSocketSessions: service.webSocketCount.Load(),
		RouteGeneration: generation, DiagnosticCount: diagnostics,
	}
}

func (service *Service) Health() Health {
	status := phaseName(phaseStopped)
	if service != nil {
		status = phaseName(service.phase.Load())
	}
	return Health{Status: status, Ready: status == "running"}
}

func (service *Service) Inspect() Inspection {
	if service == nil {
		return Inspection{}
	}
	port, _ := service.Port()
	return Inspection{
		Host: service.listener.Addr().String(), Port: port,
		Protocols:  append([]Protocol(nil), service.protocols...),
		RouteCount: len(service.Routes()), TLS: service.secure,
	}
}

func (service *Service) PollError() error {
	if service == nil {
		return nil
	}
	service.errorMu.Lock()
	defer service.errorMu.Unlock()
	if len(service.errors) == 0 {
		return nil
	}
	err := service.errors[0]
	copy(service.errors, service.errors[1:])
	service.errors = service.errors[:len(service.errors)-1]
	return err
}

func (service *Service) report(err error) {
	if err == nil {
		return
	}
	service.errorMu.Lock()
	defer service.errorMu.Unlock()
	if len(service.errors) == service.bounds.MaxDiagnostics {
		copy(service.errors, service.errors[1:])
		service.errors[len(service.errors)-1] = err
		return
	}
	service.errors = append(service.errors, err)
}

func (service *Service) Wait(ctx context.Context) error {
	if service == nil || ctx == nil {
		return errors.New("coakka http wait requires a service and context")
	}
	select {
	case <-service.serveDone:
		return nil
	case <-ctx.Done():
		return ctx.Err()
	}
}

func (service *Service) Close(ctx context.Context) error {
	if service == nil {
		return nil
	}
	if ctx == nil {
		return errors.New("coakka http close context is nil")
	}
	service.closeMu.Lock()
	defer service.closeMu.Unlock()
	if service.phase.Load() == phaseStopped {
		return nil
	}
	service.phase.Store(phaseDraining)
	service.cancel()
	service.closeWebSockets()
	service.client.CloseIdleConnections()
	if err := service.server.Shutdown(ctx); err != nil {
		return err
	}
	select {
	case <-service.serveDone:
		service.phase.Store(phaseStopped)
		return nil
	case <-ctx.Done():
		return ctx.Err()
	}
}

func (service *Service) serve(listener net.Listener, configuration *TLSConfiguration) {
	defer func() {
		service.cancel()
		service.closeWebSockets()
		service.client.CloseIdleConnections()
		service.phase.Store(phaseStopped)
		close(service.serveDone)
	}()
	var err error
	if configuration == nil {
		err = service.server.Serve(listener)
	} else {
		err = service.server.ServeTLS(listener, configuration.CertificateFile, configuration.PrivateKeyFile)
	}
	if err != nil && !errors.Is(err, http.ErrServerClosed) {
		service.report(err)
	}
}

func (service *Service) configureProtocols() {
	if service.static != nil {
		copy := *service.static
		service.static = &copy
	}
	if service.protocols == nil {
		service.protocols = []Protocol{HTTP11}
	}
	if service.secure && service.server.TLSConfig == nil {
		service.server.TLSConfig = &tls.Config{MinVersion: tls.VersionTLS12}
	}
	next := make([]string, len(service.protocols))
	for index, protocol := range service.protocols {
		next[index] = string(protocol)
	}
	if service.secure {
		service.server.TLSConfig.NextProtos = next
	}
	if service.secure && len(next) == 1 && next[0] == string(HTTP11) {
		service.server.TLSNextProto = make(map[string]func(*http.Server, *tls.Conn, http.Handler))
	}
}

func phaseName(phase uint32) string {
	switch phase {
	case phaseRunning:
		return "running"
	case phaseDraining:
		return "draining"
	default:
		return "stopped"
	}
}

func headerBytes(headers http.Header) int {
	total := 0
	for name, values := range headers {
		for _, value := range values {
			total += len(name) + len(value)
		}
	}
	return total
}

func cloneURL(source *url.URL) *url.URL {
	if source == nil {
		return nil
	}
	copy := *source
	return &copy
}

type trackedWriter struct {
	http.ResponseWriter
	wrote bool
}

func (writer *trackedWriter) WriteHeader(status int) {
	writer.wrote = true
	writer.ResponseWriter.WriteHeader(status)
}

func (writer *trackedWriter) Write(bytes []byte) (int, error) {
	writer.wrote = true
	return writer.ResponseWriter.Write(bytes)
}

func (writer *trackedWriter) Flush() {
	writer.wrote = true
	if flusher, ok := writer.ResponseWriter.(http.Flusher); ok {
		flusher.Flush()
	}
}

type limitedListener struct {
	net.Listener
	sem         chan struct{}
	connections *atomic.Int64
	rejected    *atomic.Uint64
}

func newLimitedListener(listener net.Listener, maximum int, connections *atomic.Int64, rejected *atomic.Uint64) net.Listener {
	return &limitedListener{Listener: listener, sem: make(chan struct{}, maximum), connections: connections, rejected: rejected}
}

func (listener *limitedListener) Accept() (net.Conn, error) {
	for {
		connection, err := listener.Listener.Accept()
		if err != nil {
			return nil, err
		}
		select {
		case listener.sem <- struct{}{}:
			listener.connections.Add(1)
			return &limitedConnection{Conn: connection, owner: listener}, nil
		default:
			listener.rejected.Add(1)
			_ = connection.Close()
		}
	}
}

type limitedConnection struct {
	net.Conn
	owner *limitedListener
	once  sync.Once
}

func (connection *limitedConnection) Close() error {
	err := connection.Conn.Close()
	connection.once.Do(func() {
		<-connection.owner.sem
		connection.owner.connections.Add(-1)
	})
	return err
}
