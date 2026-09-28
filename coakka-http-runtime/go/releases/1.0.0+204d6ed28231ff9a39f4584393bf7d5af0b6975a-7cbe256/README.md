# CoAkka HTTP For Go 1.0.0

This immutable source archive embeds the exact runtime image for all five release
targets. The selected payload is extracted to an owner-only process directory,
verified by SHA-256, and retained for process lifetime.

The archive is self-contained for `go test`, including its TLS test fixtures.
Use `Builder`/`Service` for host-inline handlers and `Config`/advanced service for
the complete event, streaming, WebSocket, outbound, route-control, health and
monitor surfaces.

