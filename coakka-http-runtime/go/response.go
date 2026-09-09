package coakkahttp

import (
	"bufio"
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"net/http"
	"strconv"
	"strings"
)

type responseKind uint8

const (
	responseBuffered responseKind = iota
	responseStream
	responseEvents
)

// Response is the single handler result type for buffered, streamed and
// server-event responses.
type Response struct {
	Status  int
	Headers http.Header
	Body    []byte

	kind   responseKind
	stream io.Reader
	events <-chan ServerEvent
}

type ServerEvent struct {
	Data        string
	Event       string
	ID          string
	RetryMillis int
}

func Empty(status int) Response {
	return Response{Status: normalizedStatus(status), Headers: make(http.Header)}
}

func Text(status int, value string) Response {
	response := Bytes(status, []byte(value))
	response.Headers.Set("Content-Type", "text/plain; charset=utf-8")
	return response
}

func JSON(status int, value any) (Response, error) {
	bytes, err := json.Marshal(value)
	if err != nil {
		return Response{}, fmt.Errorf("coakka http response JSON: %w", err)
	}
	response := Bytes(status, bytes)
	response.Headers.Set("Content-Type", "application/json")
	return response, nil
}

func Bytes(status int, value []byte) Response {
	return Response{
		Status: normalizedStatus(status), Headers: make(http.Header),
		Body: append([]byte(nil), value...),
	}
}

func Stream(status int, reader io.Reader) Response {
	return Response{
		Status: normalizedStatus(status), Headers: make(http.Header),
		kind: responseStream, stream: reader,
	}
}

func Events(events <-chan ServerEvent) Response {
	response := Response{
		Status: http.StatusOK, Headers: make(http.Header),
		kind: responseEvents, events: events,
	}
	response.Headers.Set("Content-Type", "text/event-stream")
	response.Headers.Set("Cache-Control", "no-cache")
	return response
}

func normalizedStatus(status int) int {
	if status >= 100 && status <= 999 {
		return status
	}
	return http.StatusOK
}

func (response Response) WithHeader(name, value string) Response {
	if response.Headers == nil {
		response.Headers = make(http.Header)
	} else {
		response.Headers = response.Headers.Clone()
	}
	response.Headers.Set(name, value)
	return response
}

func writeResponse(ctx context.Context, writer http.ResponseWriter, response Response, bounds Bounds) error {
	if response.Status < 100 || response.Status > 999 {
		return errors.New("coakka http response status is invalid")
	}
	if err := copyResponseHeaders(writer.Header(), response.Headers, bounds.MaxHeaderBytes); err != nil {
		return err
	}
	switch response.kind {
	case responseBuffered:
		if int64(len(response.Body)) > bounds.MaxResponseBodyBytes {
			return errors.New("coakka http response body limit exceeded")
		}
		writer.WriteHeader(response.Status)
		_, err := writer.Write(response.Body)
		return err
	case responseStream:
		if response.stream == nil {
			return errors.New("coakka http response stream is nil")
		}
		writer.WriteHeader(response.Status)
		return copyStream(ctx, writer, response.stream, bounds)
	case responseEvents:
		if response.events == nil {
			return errors.New("coakka http server event stream is nil")
		}
		writer.WriteHeader(response.Status)
		return writeEvents(ctx, writer, response.events, bounds)
	default:
		return errors.New("coakka http response kind is invalid")
	}
}

func copyResponseHeaders(destination, source http.Header, maximum int) error {
	bytes := 0
	for name, values := range source {
		if !validResponseHeader(name) {
			return fmt.Errorf("coakka http response header %q is not allowed", name)
		}
		for _, value := range values {
			bytes += len(name) + len(value)
			if bytes > maximum {
				return errors.New("coakka http response header limit exceeded")
			}
			destination.Add(name, value)
		}
	}
	return nil
}

func validResponseHeader(name string) bool {
	switch strings.ToLower(name) {
	case "connection", "content-length", "keep-alive", "proxy-connection", "transfer-encoding", "upgrade":
		return false
	default:
		return validHeaderName(name)
	}
}

func validHeaderName(name string) bool {
	if name == "" {
		return false
	}
	for index := range len(name) {
		character := name[index]
		if (character >= 'a' && character <= 'z') ||
			(character >= 'A' && character <= 'Z') ||
			(character >= '0' && character <= '9') ||
			strings.ContainsRune("!#$%&'*+-.^_`|~", rune(character)) {
			continue
		}
		return false
	}
	return true
}

func copyStream(ctx context.Context, writer http.ResponseWriter, source io.Reader, bounds Bounds) error {
	if closer, ok := source.(io.Closer); ok {
		defer closer.Close()
	}
	buffer := make([]byte, bounds.MaxStreamChunkBytes)
	var total int64
	for {
		if err := ctx.Err(); err != nil {
			return err
		}
		count, err := source.Read(buffer)
		if count > 0 {
			total += int64(count)
			if total > bounds.MaxResponseBodyBytes {
				return errors.New("coakka http response stream limit exceeded")
			}
			if _, writeErr := writer.Write(buffer[:count]); writeErr != nil {
				return writeErr
			}
			if flusher, ok := writer.(http.Flusher); ok {
				flusher.Flush()
			}
		}
		if errors.Is(err, io.EOF) {
			return nil
		}
		if err != nil {
			return err
		}
		if count == 0 {
			return errors.New("coakka http response stream made no progress")
		}
	}
}

func writeEvents(ctx context.Context, writer http.ResponseWriter, events <-chan ServerEvent, bounds Bounds) error {
	buffered := bufio.NewWriterSize(writer, bounds.MaxStreamChunkBytes)
	var total int64
	for {
		select {
		case <-ctx.Done():
			return ctx.Err()
		case event, ok := <-events:
			if !ok {
				return buffered.Flush()
			}
			encoded, err := encodeEvent(event, bounds.MaxStreamChunkBytes)
			if err != nil {
				return err
			}
			total += int64(len(encoded))
			if total > bounds.MaxResponseBodyBytes {
				return errors.New("coakka http server event stream limit exceeded")
			}
			if _, err := buffered.WriteString(encoded); err != nil {
				return err
			}
			if err := buffered.Flush(); err != nil {
				return err
			}
			if flusher, ok := writer.(http.Flusher); ok {
				flusher.Flush()
			}
		}
	}
}

func encodeEvent(event ServerEvent, maximum int) (string, error) {
	if strings.ContainsAny(event.Event, "\r\n") || strings.ContainsAny(event.ID, "\r\n") || event.RetryMillis < 0 {
		return "", errors.New("coakka http server event is invalid")
	}
	var builder strings.Builder
	if event.Event != "" {
		builder.WriteString("event: ")
		builder.WriteString(event.Event)
		builder.WriteByte('\n')
	}
	if event.ID != "" {
		builder.WriteString("id: ")
		builder.WriteString(event.ID)
		builder.WriteByte('\n')
	}
	if event.RetryMillis != 0 {
		builder.WriteString("retry: ")
		builder.WriteString(strconv.Itoa(event.RetryMillis))
		builder.WriteByte('\n')
	}
	for _, line := range strings.Split(strings.ReplaceAll(event.Data, "\r\n", "\n"), "\n") {
		builder.WriteString("data: ")
		builder.WriteString(line)
		builder.WriteByte('\n')
	}
	if event.Data == "" {
		builder.WriteString("data: \n")
	}
	builder.WriteByte('\n')
	if builder.Len() > maximum {
		return "", errors.New("coakka http server event limit exceeded")
	}
	return builder.String(), nil
}
