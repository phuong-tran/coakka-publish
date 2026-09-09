// Package coakkahttp provides a bounded HTTP server and client API backed by
// CoAkka HTTP Core route admission.
package coakkahttp

import (
	"bytes"
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"mime"
	"mime/multipart"
	"net/http"
	"net/url"
	"sync"
)

const (
	maximumRoutes            = 1024
	maximumRequestBodyBytes  = 1 << 30
	maximumRequestTargetSize = 64 << 10
	maximumHeaderBytes       = 16 << 20
	maximumResponseBodyBytes = 1 << 30
	maximumStreamChunkBytes  = 4 << 20
	maximumWebSocketBytes    = 64 << 20
)

var ErrBodyLimit = errors.New("coakka http body limit exceeded")

// Bounds fixes resource ceilings before a service starts.
type Bounds struct {
	MaxActiveHandlers     int
	MaxConnections        int
	MaxRequestBodyBytes   int64
	MaxRequestTargetBytes int
	MaxHeaderBytes        int
	MaxResponseBodyBytes  int64
	MaxStreamChunkBytes   int
	MaxMultipartParts     int
	MaxWebSocketBytes     int64
	MaxDiagnostics        int
}

func DefaultBounds() Bounds {
	return Bounds{
		MaxActiveHandlers:     256,
		MaxConnections:        512,
		MaxRequestBodyBytes:   1 << 20,
		MaxRequestTargetBytes: 8 << 10,
		MaxHeaderBytes:        1 << 20,
		MaxResponseBodyBytes:  8 << 20,
		MaxStreamChunkBytes:   256 << 10,
		MaxMultipartParts:     128,
		MaxWebSocketBytes:     1 << 20,
		MaxDiagnostics:        64,
	}
}

func (bounds Bounds) validate() error {
	if bounds.MaxActiveHandlers <= 0 || bounds.MaxActiveHandlers > 16_384 ||
		bounds.MaxConnections <= 0 || bounds.MaxConnections > 65_536 ||
		bounds.MaxRequestBodyBytes <= 0 || bounds.MaxRequestBodyBytes > maximumRequestBodyBytes ||
		bounds.MaxRequestTargetBytes <= 0 || bounds.MaxRequestTargetBytes > maximumRequestTargetSize ||
		bounds.MaxHeaderBytes <= 0 || bounds.MaxHeaderBytes > maximumHeaderBytes ||
		bounds.MaxResponseBodyBytes <= 0 || bounds.MaxResponseBodyBytes > maximumResponseBodyBytes ||
		bounds.MaxStreamChunkBytes <= 0 || bounds.MaxStreamChunkBytes > maximumStreamChunkBytes ||
		bounds.MaxMultipartParts <= 0 || bounds.MaxMultipartParts > 4096 ||
		bounds.MaxWebSocketBytes <= 0 || bounds.MaxWebSocketBytes > maximumWebSocketBytes ||
		bounds.MaxDiagnostics <= 0 || bounds.MaxDiagnostics > 4096 {
		return errors.New("coakka http bounds are invalid")
	}
	return nil
}

type FormField struct {
	Name  string
	Value string
}

type MultipartPart struct {
	Name        string
	Value       string
	Filename    string
	ContentType string
	Data        []byte
}

// Request owns a single-use body. Buffered accessors cache one bounded copy;
// Stream transfers body reading to the caller and cannot be mixed with them.
type Request struct {
	Method     string
	URL        *url.URL
	Headers    http.Header
	Params     map[string]string
	RemoteAddr string

	context  context.Context
	body     io.ReadCloser
	maximum  int64
	maxParts int
	mu       sync.Mutex
	buffer   []byte
	err      error
	read     bool
	stream   bool
}

func (request *Request) Context() context.Context {
	if request == nil || request.context == nil {
		return context.Background()
	}
	return request.context
}

func (request *Request) Stream() (io.ReadCloser, error) {
	if request == nil {
		return nil, errors.New("coakka http request is nil")
	}
	request.mu.Lock()
	defer request.mu.Unlock()
	if request.read || request.stream {
		return nil, errors.New("coakka http request body was already consumed")
	}
	request.stream = true
	return &boundedBody{source: request.body, remaining: request.maximum}, nil
}

func (request *Request) Bytes() ([]byte, error) {
	if request == nil {
		return nil, errors.New("coakka http request is nil")
	}
	request.mu.Lock()
	defer request.mu.Unlock()
	if request.stream {
		return nil, errors.New("coakka http request body stream is already owned")
	}
	if !request.read {
		request.buffer, request.err = readBounded(request.body, request.maximum)
		request.read = true
	}
	return append([]byte(nil), request.buffer...), request.err
}

func (request *Request) Text() (string, error) {
	bytes, err := request.Bytes()
	return string(bytes), err
}

func (request *Request) JSON(value any) error {
	payload, err := request.Bytes()
	if err != nil {
		return err
	}
	decoder := json.NewDecoder(bytes.NewReader(payload))
	decoder.DisallowUnknownFields()
	if err := decoder.Decode(value); err != nil {
		return fmt.Errorf("coakka http request JSON: %w", err)
	}
	if decoder.Decode(&struct{}{}) != io.EOF {
		return errors.New("coakka http request JSON has trailing data")
	}
	return nil
}

func (request *Request) Form() ([]FormField, error) {
	mediaType, _, err := mime.ParseMediaType(request.Headers.Get("Content-Type"))
	if err != nil || mediaType != "application/x-www-form-urlencoded" {
		return nil, errors.New("coakka http request is not form data")
	}
	payload, err := request.Bytes()
	if err != nil {
		return nil, err
	}
	values, err := url.ParseQuery(string(payload))
	if err != nil {
		return nil, fmt.Errorf("coakka http form: %w", err)
	}
	fields := make([]FormField, 0, len(values))
	for name, entries := range values {
		for _, value := range entries {
			fields = append(fields, FormField{Name: name, Value: value})
		}
	}
	return fields, nil
}

func (request *Request) Multipart() ([]MultipartPart, error) {
	mediaType, parameters, err := mime.ParseMediaType(request.Headers.Get("Content-Type"))
	if err != nil || mediaType != "multipart/form-data" || parameters["boundary"] == "" {
		return nil, errors.New("coakka http request is not multipart data")
	}
	payload, err := request.Bytes()
	if err != nil {
		return nil, err
	}
	reader := multipart.NewReader(bytes.NewReader(payload), parameters["boundary"])
	parts := make([]MultipartPart, 0)
	for {
		part, nextErr := reader.NextPart()
		if errors.Is(nextErr, io.EOF) {
			return parts, nil
		}
		if nextErr != nil {
			return nil, fmt.Errorf("coakka http multipart: %w", nextErr)
		}
		if len(parts) >= request.maxParts {
			_ = part.Close()
			return nil, errors.New("coakka http multipart part limit exceeded")
		}
		data, readErr := readBounded(part, request.maximum)
		_ = part.Close()
		if readErr != nil {
			return nil, readErr
		}
		parts = append(parts, MultipartPart{
			Name: part.FormName(), Value: string(data), Filename: part.FileName(),
			ContentType: part.Header.Get("Content-Type"), Data: data,
		})
	}
}

func readBounded(reader io.Reader, maximum int64) ([]byte, error) {
	if reader == nil {
		return nil, nil
	}
	bytes, err := io.ReadAll(io.LimitReader(reader, maximum+1))
	if err != nil {
		return nil, err
	}
	if int64(len(bytes)) > maximum {
		return nil, ErrBodyLimit
	}
	return bytes, nil
}

type boundedBody struct {
	source    io.ReadCloser
	remaining int64
	exceeded  bool
}

func (body *boundedBody) Read(target []byte) (int, error) {
	if body.exceeded {
		return 0, ErrBodyLimit
	}
	if body.remaining == 0 {
		var one [1]byte
		count, err := body.source.Read(one[:])
		if count != 0 {
			body.exceeded = true
			return 0, ErrBodyLimit
		}
		return 0, err
	}
	if int64(len(target)) > body.remaining {
		target = target[:body.remaining]
	}
	count, err := body.source.Read(target)
	body.remaining -= int64(count)
	return count, err
}

func (body *boundedBody) Close() error { return body.source.Close() }
