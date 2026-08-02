# Native C++ Transport Configuration API

## Scope And Status

This document defines the native C++ connector surface for runtime TCP
connection strategy, TLS/mTLS, and capability discovery. The C++ SDK delegates
all validation, entitlement, lifecycle, and atomic-publication semantics to the
public C ABI. It does not invent a second configuration model.

The surface follows the released public connector ABI. `PER_EXCHANGE +
PLAINTEXT` remains the default when `StartSpec` leaves both transport fields
unset.

## Public Types

### `TcpConnectionStrategySpec`

| Field | Default | Meaning |
| --- | --- | --- |
| `mode` | `PER_EXCHANGE` | Per-instance connection strategy selected before startup |
| `max_connections` | unset | Bounded-pool tuning; unset uses the profile default |
| `max_requests_per_connection` | unset | Bounded-pool retirement count; unset uses the profile default |
| `idle_timeout_ms` | unset | Bounded-pool idle retirement; unset uses the profile default |

Numeric tuning fields are valid only for `BOUNDED_POOL` and only when
`RuntimeCapabilitiesView::supports(COAKKA_V2_CAPABILITY_TCP_POOL_TUNING)` is
true. Unset is distinct from explicitly setting zero.

`mode` is stored as `uint32_t`, matching the C ABI configuration boundary.
Named `COAKKA_V2_TCP_CONNECTION_*` constants remain the normal input; an
unknown numeric value is rejected by runtime validation with a structured
result instead of becoming an invalid C++ enum value.

### `TcpSecuritySpec`

| Field | Default | Meaning |
| --- | --- | --- |
| `mode` | `PLAINTEXT` | `PLAINTEXT`, `TLS`, or `MUTUAL_TLS` |
| `reload_mode` | `GRACEFUL` | Existing-session policy for a successful newer generation |
| `credential_generation` | `0` | Must be nonzero for TLS/mTLS and strictly increase on reload |
| `credential_id` | empty | Non-secret operator label, at most 127 bytes |
| `ca_certificate_file` | empty | PEM trust roots |
| `identity_certificate_file` | empty | PEM identity chain |
| `private_key_file` | empty | PEM private key matching the identity chain |

`mode` and `reload_mode` are `uint32_t` C ABI values. Use the named
`COAKKA_V2_TCP_SECURITY_*` and `COAKKA_V2_TLS_RELOAD_*` constants; unknown
values are passed to runtime validation and rejected atomically.

The native connector currently exposes the implemented file-backed credential
source. `MEMORY` and `PROVIDER` remain C ABI vocabulary but are not presented as
usable C++ options until the runtime implements them.

The connector owns its `std::string` copies. The runtime borrows each path only
for the synchronous apply, reads and validates the files, and publishes a
private immutable credential context. Results never expose secret bytes or
source paths.

### Result And View Types

- `TcpConnectionConfigView` is a value copy of the effective strategy and its
  explicit/default provenance.
- `TcpConnectionApplyResult` contains `status`, `changed`, stable `reason`,
  runtime state, field-level validation, and `active_config` after the attempt.
- `TcpSecurityInfoView` is copy-safe non-secret security and certificate
  identity metadata.
- `TcpSecurityApplyResult` contains the same attempt metadata plus
  `active_security`, including after rejection.
- `RuntimeCapabilitiesView` carries compiled, entitled, and effective
  capability masks. Effective capability truth, not edition name alone, decides
  whether an option may be presented to users.

`TcpConnectionApplyResult::applied()` and
`TcpSecurityApplyResult::applied()` return true only when `status` is
`COAKKA_V2_OK`. `changed == false` can still be a successful idempotent apply.

## Startup Contract

`RuntimeHandle(const StartSpec&)` and `ConnectorOrchestrator(const StartSpec&)`
execute this order:

1. create the runtime;
2. apply `tcp_connection_strategy`, when present;
3. apply `tcp_security`, when present;
4. export host handles;
5. atomically apply the initial route snapshot;
6. start the runtime.

Connection strategy is therefore startup-configured. A startup connection
rejection throws `TcpConnectionApplyError`; a startup security rejection throws
`TcpSecurityApplyError`. Both exceptions use connector category
`CONFIGURATION` and retain the complete structured result. The partially
constructed runtime is stopped/destroyed and no host handle escapes.

After successful construction,
`startupTcpConnectionApplyResult()` and
`startupTcpSecurityApplyResult()` return value copies of the requested startup
attempts. They return `std::nullopt` when that option was omitted.

## Function Reference

### `RuntimeCapabilitiesView::supports(uint64_t capabilities) const noexcept`

- Purpose: test whether every requested bit is effective.
- Parameter: one bit or an OR-combination of stable capability bits; zero is
  always supported.
- Ownership/lifetime: value-only, no borrowed storage.
- Thread safety/blocking: const, non-blocking, no shared state.
- Lifecycle: independent of runtime instances.
- Result/error: boolean; does not throw.
- Edition availability: all editions. The returned effective mask expresses
  actual availability.

### `RuntimeHandle::readRuntimeCapabilities()`

- Purpose: read edition, license state, and capability truth before creating a
  runtime.
- Parameters/defaults: none.
- Ownership/lifetime: returns a complete value copy.
- Thread safety/blocking: global immutable metadata read; non-blocking.
- Lifecycle: no instance required.
- Result/error: returns `RuntimeCapabilitiesView`; throws `StatusError` only if
  the C ABI metadata read fails.
- Edition availability: all editions.

`ConnectorOrchestrator::readRuntimeCapabilities()` is an identical static
forwarder with the same contract.

### `RuntimeHandle::applyTcpConnectionStrategy(const TcpConnectionStrategySpec&)`

- Purpose: submit one atomic connection-policy apply and expose the exact C ABI
  result.
- Parameter/defaults: `spec`; unset pool tuning uses runtime profile defaults.
- Ownership/lifetime: the connector builds a temporary C options block; the
  runtime copies effective values before return.
- Thread safety: connection/security apply calls on one handle are serialized
  by the connector. The caller must not race them with `stop`, destruction, or
  move-assignment of the same handle.
- Blocking: synchronous; validation is local and adapter configuration may
  allocate bounded resources. It performs no business exchange.
- Lifecycle/atomicity: accepted only in `CREATED`. `RuntimeHandle` starts
  eagerly, so normal post-construction calls return `BAD_STATE` with
  `RUNTIME_NOT_CONFIGURABLE`; `active_config` remains unchanged. Use
  `StartSpec::tcp_connection_strategy` for a real selection.
- Result/error: returns `TcpConnectionApplyResult` for success and rejection;
  it does not throw for a runtime rejection.
- Edition availability: `PER_EXCHANGE` everywhere;
  `BOUNDED_POOL`, advanced modes, and tuning follow effective capability bits.

`ConnectorOrchestrator::applyTcpConnectionStrategy(...)` forwards to its owned
handle with the same contract.

### `RuntimeHandle::readTcpConnectionConfig() const`

- Purpose: read the effective strategy, defaults revision, and provenance.
- Parameters/defaults: none.
- Ownership/lifetime: returns a complete value copy.
- Thread safety/blocking: the runtime takes its instance configuration lock;
  the call is synchronous and short.
- Lifecycle: valid from successful construction until `stop`; core introspection
  also remains valid in `STOPPED` while the handle exists.
- Result/error: returns `TcpConnectionConfigView`; throws `StatusError` on C ABI
  read failure.
- Edition availability: all editions.

`ConnectorOrchestrator::readTcpConnectionConfig()` is an identical forwarder.

### `RuntimeHandle::applyTcpSecurity(const TcpSecuritySpec&)`

- Purpose: atomically apply initial security or load a strictly newer
  credential generation for the active TLS/mTLS mode.
- Parameter/defaults: `spec`; `GRACEFUL` is the default reload policy.
- Ownership/lifetime: path strings are borrowed only during the call. The
  returned `active_security` is a value copy with no secret paths or bytes.
- Thread safety: serialized with connection/security applies on the same
  handle. Do not race it with lifecycle destruction, `stop`, or move-assignment.
- Blocking: synchronous file I/O, PEM parsing, key/certificate matching, and
  immutable TLS context construction may block. Run rotation from a control
  thread, not a latency-sensitive business handler.
- Lifecycle/atomicity: initial mode is applied in `CREATED`; after `STARTED`,
  only a newer generation of the same TLS/mTLS mode is accepted. Mode change
  requires a new runtime and application-owned cutover. Rejection never
  publishes partial credentials and returns the previous `active_security`.
- Result/error: returns `TcpSecurityApplyResult` for success and rejection; it
  does not throw for a runtime rejection.
- Edition availability: plaintext everywhere. TLS, mTLS, and live reload follow
  `TCP_TLS`, `TCP_MUTUAL_TLS`, and `TLS_CREDENTIAL_RELOAD` effective bits.

`ConnectorOrchestrator::applyTcpSecurity(...)` forwards to its owned handle
with the same contract.

### `RuntimeHandle::readTcpSecurityInfo() const`

- Purpose: read active non-secret mode, generation, verification flags,
  certificate bounds, ID, and SHA-256 fingerprint.
- Parameters/defaults: none.
- Ownership/lifetime: all strings are copied; no runtime pointer escapes.
- Thread safety/blocking: the runtime takes its instance configuration lock;
  the call performs no credential file I/O.
- Lifecycle: readable while the runtime handle exists, including after a failed
  reload and after `stop`.
- Result/error: returns `TcpSecurityInfoView`; throws `StatusError` on C ABI read
  failure.
- Edition availability: all editions; unsupported builds report plaintext.

`ConnectorOrchestrator::readTcpSecurityInfo()` is an identical forwarder.

### Startup Result Accessors

`RuntimeHandle::startupTcpConnectionApplyResult() const` and
`RuntimeHandle::startupTcpSecurityApplyResult() const`:

- Purpose: inspect the successful startup apply that construction performed.
- Parameters/defaults: none.
- Ownership/lifetime: return optional value copies.
- Thread safety/blocking: immutable after successful construction; non-blocking.
- Lifecycle: valid for the handle lifetime.
- Result/error: `std::nullopt` when the matching `StartSpec` option was absent;
  do not throw.
- Edition availability: all editions.

### Startup Exceptions

`TcpConnectionApplyError(TcpConnectionApplyResult)` and
`TcpSecurityApplyError(TcpSecurityApplyResult)` are public so exception objects
remain ordinary C++ values, but application code normally catches rather than
constructs them. `result()` returns a const reference valid for the exception
object's lifetime. It does not block or throw.

## Connection Mode Examples

Always query capability truth before presenting advanced choices:

```cpp
using namespace coakka::v2::native_cpp;

const auto caps = ConnectorOrchestrator::readRuntimeCapabilities();

StartSpec spec;
spec.system_name = "orders";
spec.node_id = "orders-a";
spec.routes = load_routes();

TcpConnectionStrategySpec strategy;
strategy.mode = COAKKA_V2_TCP_CONNECTION_PER_EXCHANGE;
spec.tcp_connection_strategy = strategy;
ConnectorOrchestrator per_exchange(spec);
```

Use the same startup field for the other modes:

```cpp
strategy.mode = COAKKA_V2_TCP_CONNECTION_BOUNDED_POOL;
spec.tcp_connection_strategy = strategy;

if (caps.supports(COAKKA_V2_CAPABILITY_TCP_PERSISTENT_SINGLE_FLIGHT)) {
  strategy.mode = COAKKA_V2_TCP_CONNECTION_PERSISTENT_SINGLE_FLIGHT;
  spec.tcp_connection_strategy = strategy;
}

if (caps.supports(COAKKA_V2_CAPABILITY_TCP_MULTIPLEXING)) {
  strategy.mode = COAKKA_V2_TCP_CONNECTION_MULTIPLEXED;
  spec.tcp_connection_strategy = strategy;
}
```

Each snippet represents a separate runtime instance. Do not change `strategy`
after constructing an orchestrator and expect the active instance to change.

Pool tuning is capability-gated:

```cpp
if (caps.supports(COAKKA_V2_CAPABILITY_TCP_POOL_TUNING)) {
  TcpConnectionStrategySpec pool;
  pool.mode = COAKKA_V2_TCP_CONNECTION_BOUNDED_POOL;
  pool.max_connections = 4;
  pool.max_requests_per_connection = 4096;
  pool.idle_timeout_ms = 15000;
  spec.tcp_connection_strategy = pool;
}
```

## TLS Startup And Reload Example

```cpp
using namespace coakka::v2::native_cpp;

TcpSecuritySpec tls;
tls.mode = COAKKA_V2_TCP_SECURITY_TLS;
tls.reload_mode = COAKKA_V2_TLS_RELOAD_GRACEFUL;
tls.credential_generation = 1;
tls.credential_id = "orders-generation-1";
tls.ca_certificate_file = "/run/coakka/ca.pem";
tls.identity_certificate_file = "/run/coakka/identity.pem";
tls.private_key_file = "/run/coakka/identity.key";

StartSpec spec;
spec.system_name = "orders";
spec.node_id = "orders-a";
spec.routes = load_routes();
spec.tcp_security = tls;

ConnectorOrchestrator connector(spec);

TcpSecuritySpec next = tls;
next.credential_generation = 2;
next.credential_id = "orders-generation-2";
next.identity_certificate_file = "/run/coakka/next/identity.pem";
next.private_key_file = "/run/coakka/next/identity.key";

const TcpSecurityApplyResult result = connector.applyTcpSecurity(next);
if (!result.applied()) {
  log_reload_rejection(result.reason,
                       result.active_security.credential_generation);
}
```

Do not delete the old secret material merely because apply returned. Follow the
selected graceful/drain policy and deployment ownership rules in
[Runtime TLS And mTLS](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md).

## Related Contracts

- [Runtime Connection Strategies](https://github.com/phuong-tran/coakka-publish/blob/main/docs/connection-strategies.md)
- [Runtime TLS And mTLS](https://github.com/phuong-tran/coakka-publish/blob/main/docs/tls-and-mtls.md)
- [Runtime Integration Guide](https://github.com/phuong-tran/coakka-publish/blob/main/docs/runtime-integration-guide.md)
- [Troubleshooting](https://github.com/phuong-tran/coakka-publish/blob/main/docs/troubleshooting.md)
