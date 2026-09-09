package coakkahttp

import (
	"bytes"
	"context"
	"crypto/rand"
	"crypto/rsa"
	"crypto/tls"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/json"
	"encoding/pem"
	"errors"
	"fmt"
	"io"
	"math/big"
	"mime/multipart"
	"net"
	"net/http"
	"os"
	"path/filepath"
	"sort"
	"strings"
	"testing"
	"time"

	"github.com/coder/websocket"
)

func TestHTTPFeatureSurface(t *testing.T) {
	staticRoot := t.TempDir()
	mustWrite(t, filepath.Join(staticRoot, "index.html"), "<h1>CoAkka</h1>")
	mustWrite(t, filepath.Join(staticRoot, "asset.txt"), "0123456789")

	events := make(chan ServerEvent, 2)
	events <- ServerEvent{Data: "first", Event: "item", ID: "7"}
	events <- ServerEvent{Data: "second", RetryMillis: 1000}
	close(events)

	var service *Service
	builder := NewBuilder().Listen("127.0.0.1", 0).
		Get("/values/{id}", func(request *Request) (Response, error) {
			return mustJSON(http.StatusOK, map[string]string{
				"id": request.Params["id"], "query": request.URL.Query().Get("q"),
				"header": request.Headers.Get("X-Test"),
			}), nil
		}).
		Post("/json", func(request *Request) (Response, error) {
			var value map[string]bool
			if err := request.JSON(&value); err != nil {
				return Response{}, err
			}
			return mustJSON(http.StatusCreated, value), nil
		}).
		Post("/form", func(request *Request) (Response, error) {
			fields, err := request.Form()
			if err != nil {
				return Response{}, err
			}
			sort.Slice(fields, func(left, right int) bool { return fields[left].Value < fields[right].Value })
			return mustJSON(http.StatusOK, fields), nil
		}).
		Post("/multipart", func(request *Request) (Response, error) {
			parts, err := request.Multipart()
			if err != nil {
				return Response{}, err
			}
			return mustJSON(http.StatusOK, parts), nil
		}).
		Post("/upload", func(request *Request) (Response, error) {
			stream, err := request.Stream()
			if err != nil {
				return Response{}, err
			}
			return Stream(http.StatusOK, stream).WithHeader("X-Stream", "request"), nil
		}).
		Get("/stream", func(*Request) (Response, error) {
			return Stream(http.StatusOK, strings.NewReader("one-two")).WithHeader("X-Stream", "response"), nil
		}).
		Get("/events", func(*Request) (Response, error) { return Events(events), nil }).
		Get("/outbound", func(request *Request) (Response, error) {
			result, err := service.Client().Execute(request.Context(), ClientRequest{
				URL: service.URL() + "/values/out?q=client", Headers: http.Header{"X-Test": {"outbound"}},
			})
			if err != nil {
				return Response{}, err
			}
			return Bytes(result.Status, result.Body).WithHeader("Content-Type", result.Headers.Get("Content-Type")), nil
		}).
		Get("/rebind", func(*Request) (Response, error) { return Text(http.StatusOK, "before"), nil }).
		WebSocket("/socket/{room}", echoWebSocket).
		Static(StaticContent{Root: staticRoot, SPAFallback: true})

	var err error
	service, err = builder.Start()
	if err != nil {
		t.Fatal(err)
	}
	t.Cleanup(func() { closeService(t, service) })

	assertJSON(t, get(t, service.URL()+"/values/42?q=value", http.Header{"X-Test": {"header"}}), map[string]string{
		"id": "42", "query": "value", "header": "header",
	})
	created := do(t, http.MethodPost, service.URL()+"/json", strings.NewReader(`{"ok":true}`), http.Header{"Content-Type": {"application/json"}})
	if created.StatusCode != http.StatusCreated {
		t.Fatalf("JSON status = %d", created.StatusCode)
	}
	assertJSON(t, created, map[string]bool{"ok": true})

	form := do(t, http.MethodPost, service.URL()+"/form", strings.NewReader("a=1&a=2"), http.Header{"Content-Type": {"application/x-www-form-urlencoded"}})
	var fields []FormField
	decodeResponse(t, form, &fields)
	if len(fields) != 2 || fields[0].Value != "1" || fields[1].Value != "2" {
		t.Fatalf("form = %#v", fields)
	}

	var multipartBody bytes.Buffer
	multipartWriter := multipart.NewWriter(&multipartBody)
	if err := multipartWriter.WriteField("field", "value"); err != nil {
		t.Fatal(err)
	}
	file, err := multipartWriter.CreateFormFile("upload", "note.txt")
	if err != nil {
		t.Fatal(err)
	}
	_, _ = io.WriteString(file, "file-body")
	_ = multipartWriter.Close()
	partsResponse := do(t, http.MethodPost, service.URL()+"/multipart", &multipartBody, http.Header{"Content-Type": {multipartWriter.FormDataContentType()}})
	var parts []MultipartPart
	decodeResponse(t, partsResponse, &parts)
	if len(parts) != 2 || parts[0].Name != "field" || parts[1].Filename != "note.txt" || string(parts[1].Data) != "file-body" {
		t.Fatalf("multipart = %#v", parts)
	}

	upload := do(t, http.MethodPost, service.URL()+"/upload", strings.NewReader("streamed"), nil)
	assertText(t, upload, http.StatusOK, "streamed")
	streamed := get(t, service.URL()+"/stream", nil)
	if streamed.Header.Get("X-Stream") != "response" {
		t.Fatalf("stream header = %q", streamed.Header.Get("X-Stream"))
	}
	assertText(t, streamed, http.StatusOK, "one-two")
	eventResponse := get(t, service.URL()+"/events", nil)
	eventText := readText(t, eventResponse)
	if !strings.Contains(eventText, "event: item\nid: 7\ndata: first") || !strings.Contains(eventText, "retry: 1000\ndata: second") {
		t.Fatalf("events = %q", eventText)
	}
	assertJSON(t, get(t, service.URL()+"/outbound", nil), map[string]string{
		"id": "out", "query": "client", "header": "outbound",
	})

	rangeResponse := get(t, service.URL()+"/asset.txt", http.Header{"Range": {"bytes=2-5"}})
	assertText(t, rangeResponse, http.StatusPartialContent, "2345")
	initial := get(t, service.URL()+"/asset.txt", nil)
	etag := initial.Header.Get("ETag")
	_ = readText(t, initial)
	if etag == "" {
		t.Fatal("static ETag is empty")
	}
	notModified := get(t, service.URL()+"/asset.txt", http.Header{"If-None-Match": {etag}})
	assertText(t, notModified, http.StatusNotModified, "")
	spa := get(t, service.URL()+"/application/route", http.Header{"Accept": {"text/html"}})
	assertText(t, spa, http.StatusOK, "<h1>CoAkka</h1>")

	binding := findBinding(t, service.Routes(), "/rebind")
	rebound, err := service.Rebind(binding.RouteID, binding.Revision, func(*Request) (Response, error) {
		return Text(http.StatusOK, "after"), nil
	})
	if err != nil || rebound.Revision != 2 || rebound.Generation != 2 {
		t.Fatalf("Rebind() = %#v, %v", rebound, err)
	}
	assertText(t, get(t, service.URL()+"/rebind", nil), http.StatusOK, "after")
	if _, err := service.Rebind(binding.RouteID, binding.Revision, func(*Request) (Response, error) { return Empty(204), nil }); err == nil {
		t.Fatal("stale route rebind succeeded")
	}

	webSocketRoundTrip(t, strings.Replace(service.URL(), "http://", "ws://", 1)+"/socket/room")
	waitFor(t, func() bool { return service.Snapshot().WebSocketSessions == 0 })
	if !service.Health().Ready || service.Inspect().RouteCount != 10 || service.Snapshot().Completed < 11 {
		t.Fatalf("inspection = %#v, snapshot = %#v", service.Inspect(), service.Snapshot())
	}
}

func TestBoundsPressureCancellationAndDiagnostics(t *testing.T) {
	bounds := DefaultBounds()
	bounds.MaxActiveHandlers = 1
	bounds.MaxRequestBodyBytes = 4
	entered := make(chan struct{})
	service, err := NewBuilder().Bounds(bounds).
		Post("/body", func(request *Request) (Response, error) {
			_, err := request.Bytes()
			if err != nil {
				return Response{}, err
			}
			return Empty(http.StatusNoContent), nil
		}).
		Get("/wait", func(request *Request) (Response, error) {
			close(entered)
			<-request.Context().Done()
			return Empty(499), nil
		}).
		Get("/panic", func(*Request) (Response, error) { panic("failure") }).
		Start()
	if err != nil {
		t.Fatal(err)
	}
	t.Cleanup(func() { closeService(t, service) })

	assertText(t, do(t, http.MethodPost, service.URL()+"/body", strings.NewReader("12345"), nil), http.StatusRequestEntityTooLarge, "request body too large\n")
	done := make(chan *http.Response, 1)
	go func() {
		response, _ := http.Get(service.URL() + "/wait")
		done <- response
	}()
	select {
	case <-entered:
	case <-time.After(2 * time.Second):
		t.Fatal("handler did not enter")
	}
	assertText(t, get(t, service.URL()+"/wait", nil), http.StatusServiceUnavailable, "service busy\n")

	ctx, cancel := context.WithTimeout(context.Background(), 2*time.Second)
	if err := service.Close(ctx); err != nil {
		cancel()
		t.Fatal(err)
	}
	cancel()
	response := <-done
	assertText(t, response, 499, "")
	if service.Health().Status != "stopped" || service.Snapshot().Rejected < 2 {
		t.Fatalf("shutdown snapshot = %#v", service.Snapshot())
	}

	panicService, err := NewBuilder().Get("/panic", func(*Request) (Response, error) { panic("failure") }).Start()
	if err != nil {
		t.Fatal(err)
	}
	defer closeService(t, panicService)
	assertText(t, get(t, panicService.URL()+"/panic", nil), http.StatusInternalServerError, "Internal Server Error\n")
	if panicService.PollError() == nil {
		t.Fatal("handler panic was not retained")
	}
}

func TestTLSAndHTTP2(t *testing.T) {
	certificate, privateKey, roots := generateTLSFiles(t)
	service, err := NewBuilder().Protocols(HTTP2, HTTP11).
		TLS(TLSConfiguration{CertificateFile: certificate, PrivateKeyFile: privateKey}).
		Get("/secure", func(*Request) (Response, error) { return Text(http.StatusOK, "secure"), nil }).
		Start()
	if err != nil {
		t.Fatal(err)
	}
	defer closeService(t, service)
	client := &http.Client{Transport: &http.Transport{
		TLSClientConfig:   &tls.Config{RootCAs: roots, MinVersion: tls.VersionTLS12},
		ForceAttemptHTTP2: true,
	}}
	port, _ := service.Port()
	response, err := client.Get(fmt.Sprintf("https://localhost:%d/secure", port))
	if err != nil {
		t.Fatal(err)
	}
	if response.ProtoMajor != 2 {
		response.Body.Close()
		t.Fatalf("protocol = %s", response.Proto)
	}
	assertText(t, response, http.StatusOK, "secure")
}

func generateTLSFiles(t *testing.T) (string, string, *x509.CertPool) {
	t.Helper()
	key, err := rsa.GenerateKey(rand.Reader, 2048)
	if err != nil {
		t.Fatal(err)
	}
	template := &x509.Certificate{
		SerialNumber: big.NewInt(1), Subject: pkix.Name{CommonName: "localhost"},
		NotBefore: time.Now().Add(-time.Minute), NotAfter: time.Now().Add(time.Hour),
		KeyUsage:    x509.KeyUsageDigitalSignature | x509.KeyUsageKeyEncipherment,
		ExtKeyUsage: []x509.ExtKeyUsage{x509.ExtKeyUsageServerAuth},
		DNSNames:    []string{"localhost"}, IPAddresses: []net.IP{net.ParseIP("127.0.0.1")},
	}
	der, err := x509.CreateCertificate(rand.Reader, template, template, &key.PublicKey, key)
	if err != nil {
		t.Fatal(err)
	}
	directory := t.TempDir()
	certificate := filepath.Join(directory, "certificate.pem")
	privateKey := filepath.Join(directory, "private-key.pem")
	mustWriteBytes(t, certificate, pem.EncodeToMemory(&pem.Block{Type: "CERTIFICATE", Bytes: der}))
	mustWriteBytes(t, privateKey, pem.EncodeToMemory(&pem.Block{Type: "RSA PRIVATE KEY", Bytes: x509.MarshalPKCS1PrivateKey(key)}))
	parsed, err := x509.ParseCertificate(der)
	if err != nil {
		t.Fatal(err)
	}
	roots := x509.NewCertPool()
	roots.AddCert(parsed)
	return certificate, privateKey, roots
}

func TestCoreRejectsAmbiguousRoutes(t *testing.T) {
	service, err := NewBuilder().
		Get("/user/{id}", func(*Request) (Response, error) { return Empty(204), nil }).
		Get("/user/{name}", func(*Request) (Response, error) { return Empty(204), nil }).
		Start()
	if service != nil || err == nil {
		t.Fatalf("Start() = %#v, %v", service, err)
	}
	var validation *RouteValidationError
	if !errors.As(err, &validation) || validation.Status != "ambiguous" {
		t.Fatalf("validation = %#v", err)
	}
}

func TestInvalidBoundsFailBeforeListening(t *testing.T) {
	bounds := DefaultBounds()
	bounds.MaxStreamChunkBytes = maximumStreamChunkBytes + 1
	service, err := NewBuilder().Bounds(bounds).
		Get("/", func(*Request) (Response, error) { return Empty(http.StatusNoContent), nil }).
		Start()
	if service != nil || err == nil {
		t.Fatalf("Start() = %#v, %v", service, err)
	}
}

func echoWebSocket(session *WebSocketSession, request *Request) error {
	kind, message, err := session.Read(request.Context())
	if err != nil {
		return err
	}
	return session.Send(request.Context(), kind, []byte(request.Params["room"]+":"+string(message)))
}

func webSocketRoundTrip(t *testing.T, address string) {
	t.Helper()
	ctx, cancel := context.WithTimeout(context.Background(), 3*time.Second)
	defer cancel()
	connection, _, err := websocket.Dial(ctx, address, nil)
	if err != nil {
		t.Fatal(err)
	}
	defer connection.CloseNow()
	if err := connection.Write(ctx, websocket.MessageText, []byte("hello")); err != nil {
		t.Fatal(err)
	}
	_, response, err := connection.Read(ctx)
	if err != nil || string(response) != "room:hello" {
		t.Fatalf("WebSocket response = %q, %v", response, err)
	}
}

func mustJSON(status int, value any) Response {
	response, err := JSON(status, value)
	if err != nil {
		panic(err)
	}
	return response
}

func findBinding(t *testing.T, bindings []RouteBinding, pattern string) RouteBinding {
	t.Helper()
	for _, binding := range bindings {
		if binding.Pattern == pattern {
			return binding
		}
	}
	t.Fatalf("route %q was not found", pattern)
	return RouteBinding{}
}

func get(t *testing.T, address string, headers http.Header) *http.Response {
	return do(t, http.MethodGet, address, nil, headers)
}

func do(t *testing.T, method, address string, body io.Reader, headers http.Header) *http.Response {
	t.Helper()
	request, err := http.NewRequest(method, address, body)
	if err != nil {
		t.Fatal(err)
	}
	request.Header = headers.Clone()
	response, err := http.DefaultClient.Do(request)
	if err != nil {
		t.Fatal(err)
	}
	return response
}

func assertText(t *testing.T, response *http.Response, status int, expected string) {
	t.Helper()
	if response.StatusCode != status {
		response.Body.Close()
		t.Fatalf("status = %d, want %d", response.StatusCode, status)
	}
	if actual := readText(t, response); actual != expected {
		t.Fatalf("body = %q, want %q", actual, expected)
	}
}

func readText(t *testing.T, response *http.Response) string {
	t.Helper()
	defer response.Body.Close()
	bytes, err := io.ReadAll(response.Body)
	if err != nil {
		t.Fatal(err)
	}
	return string(bytes)
}

func assertJSON[T any](t *testing.T, response *http.Response, expected T) {
	t.Helper()
	var actual T
	decodeResponse(t, response, &actual)
	expectedBytes, _ := json.Marshal(expected)
	actualBytes, _ := json.Marshal(actual)
	if !bytes.Equal(actualBytes, expectedBytes) {
		t.Fatalf("JSON = %#v, want %#v", actual, expected)
	}
}

func decodeResponse(t *testing.T, response *http.Response, target any) {
	t.Helper()
	defer response.Body.Close()
	if err := json.NewDecoder(response.Body).Decode(target); err != nil {
		t.Fatal(err)
	}
}

func mustWrite(t *testing.T, path, text string) {
	t.Helper()
	mustWriteBytes(t, path, []byte(text))
}

func mustWriteBytes(t *testing.T, path string, bytes []byte) {
	t.Helper()
	if err := os.WriteFile(path, bytes, 0o600); err != nil {
		t.Fatal(err)
	}
}

func closeService(t *testing.T, service *Service) {
	t.Helper()
	ctx, cancel := context.WithTimeout(context.Background(), 2*time.Second)
	defer cancel()
	if err := service.Close(ctx); err != nil {
		t.Errorf("Close() = %v", err)
	}
}

func waitFor(t *testing.T, condition func() bool) {
	t.Helper()
	deadline := time.Now().Add(2 * time.Second)
	for time.Now().Before(deadline) {
		if condition() {
			return
		}
		time.Sleep(10 * time.Millisecond)
	}
	t.Fatal("condition did not become true")
}
