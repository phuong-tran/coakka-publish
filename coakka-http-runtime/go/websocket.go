package coakkahttp

import (
	"context"
	"errors"
	"net/http"
	"sync"

	"github.com/coder/websocket"
)

type MessageType uint8

const (
	TextMessage MessageType = iota + 1
	BinaryMessage
)

type WebSocketSession struct {
	connection *websocket.Conn
	maximum    int64
	writeMu    sync.Mutex
	closeOnce  sync.Once
}

func (session *WebSocketSession) Read(ctx context.Context) (MessageType, []byte, error) {
	if session == nil || session.connection == nil || ctx == nil {
		return 0, nil, errors.New("coakka http WebSocket read is invalid")
	}
	kind, bytes, err := session.connection.Read(ctx)
	if err != nil {
		return 0, nil, err
	}
	if int64(len(bytes)) > session.maximum {
		_ = session.Close(websocket.StatusMessageTooBig, "message too large")
		return 0, nil, errors.New("coakka http WebSocket message limit exceeded")
	}
	if kind == websocket.MessageText {
		return TextMessage, append([]byte(nil), bytes...), nil
	}
	return BinaryMessage, append([]byte(nil), bytes...), nil
}

func (session *WebSocketSession) Send(ctx context.Context, kind MessageType, bytes []byte) error {
	if session == nil || session.connection == nil || ctx == nil {
		return errors.New("coakka http WebSocket send is invalid")
	}
	if int64(len(bytes)) > session.maximum {
		return errors.New("coakka http WebSocket message limit exceeded")
	}
	nativeKind := websocket.MessageBinary
	if kind == TextMessage {
		nativeKind = websocket.MessageText
	} else if kind != BinaryMessage {
		return errors.New("coakka http WebSocket message type is invalid")
	}
	session.writeMu.Lock()
	defer session.writeMu.Unlock()
	return session.connection.Write(ctx, nativeKind, bytes)
}

func (session *WebSocketSession) SendText(ctx context.Context, text string) error {
	return session.Send(ctx, TextMessage, []byte(text))
}

func (session *WebSocketSession) Close(status websocket.StatusCode, reason string) error {
	if session == nil || session.connection == nil {
		return nil
	}
	var err error
	session.closeOnce.Do(func() { err = session.connection.Close(status, reason) })
	return err
}

func (session *WebSocketSession) closeNow() {
	if session == nil || session.connection == nil {
		return
	}
	session.closeOnce.Do(func() { _ = session.connection.CloseNow() })
}

func (service *Service) serveWebSocket(
	writer http.ResponseWriter,
	incoming *http.Request,
	request *Request,
	handler WebSocketHandler,
) {
	connection, err := websocket.Accept(writer, incoming, nil)
	if err != nil {
		service.report(err)
		return
	}
	connection.SetReadLimit(service.bounds.MaxWebSocketBytes)
	session := &WebSocketSession{connection: connection, maximum: service.bounds.MaxWebSocketBytes}
	service.webSocketMu.Lock()
	service.webSockets[session] = struct{}{}
	service.webSocketCount.Add(1)
	service.webSocketMu.Unlock()
	defer func() {
		session.closeNow()
		service.webSocketMu.Lock()
		delete(service.webSockets, session)
		service.webSocketCount.Add(-1)
		service.webSocketMu.Unlock()
	}()
	if err := handler(session, request); err != nil && websocket.CloseStatus(err) == -1 {
		service.report(err)
	}
}

func (service *Service) closeWebSockets() {
	service.webSocketMu.Lock()
	sessions := make([]*WebSocketSession, 0, len(service.webSockets))
	for session := range service.webSockets {
		sessions = append(sessions, session)
	}
	service.webSocketMu.Unlock()
	for _, session := range sessions {
		session.closeNow()
	}
}
