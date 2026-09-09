package coakkahttp

import (
	"bytes"
	"context"
	"crypto/tls"
	"errors"
	"fmt"
	"io"
	"net/http"
	"time"
)

type ClientOptions struct {
	Timeout              time.Duration
	MaxResponseBodyBytes int64
	TLSConfig            *tls.Config
}

type ClientRequest struct {
	Method  string
	URL     string
	Headers http.Header
	Body    io.Reader
}

type ClientResponse struct {
	Status  int
	Headers http.Header
	Body    []byte
}

type Client struct {
	http    *http.Client
	maximum int64
}

func NewClient(options ClientOptions) *Client {
	timeout := options.Timeout
	if timeout <= 0 {
		timeout = 30 * time.Second
	}
	maximum := options.MaxResponseBodyBytes
	if maximum <= 0 || maximum > maximumResponseBodyBytes {
		maximum = 8 << 20
	}
	transport := http.DefaultTransport.(*http.Transport).Clone()
	if options.TLSConfig != nil {
		transport.TLSClientConfig = options.TLSConfig.Clone()
	}
	return &Client{http: &http.Client{Timeout: timeout, Transport: transport}, maximum: maximum}
}

func (client *Client) Execute(ctx context.Context, request ClientRequest) (*ClientResponse, error) {
	if client == nil || client.http == nil {
		return nil, errors.New("coakka http client is nil")
	}
	if ctx == nil || request.URL == "" {
		return nil, errors.New("coakka http client request is invalid")
	}
	if request.Method == "" {
		request.Method = http.MethodGet
	}
	method, err := normalizeMethod(request.Method)
	if err != nil {
		return nil, err
	}
	outbound, err := http.NewRequestWithContext(ctx, method, request.URL, request.Body)
	if err != nil {
		return nil, fmt.Errorf("coakka http client request: %w", err)
	}
	outbound.Header = request.Headers.Clone()
	response, err := client.http.Do(outbound)
	if err != nil {
		return nil, fmt.Errorf("coakka http client execute: %w", err)
	}
	defer response.Body.Close()
	body, err := readBounded(response.Body, client.maximum)
	if err != nil {
		return nil, fmt.Errorf("coakka http client response: %w", err)
	}
	return &ClientResponse{Status: response.StatusCode, Headers: response.Header.Clone(), Body: bytes.Clone(body)}, nil
}

func (client *Client) CloseIdleConnections() {
	if client != nil && client.http != nil {
		client.http.CloseIdleConnections()
	}
}
