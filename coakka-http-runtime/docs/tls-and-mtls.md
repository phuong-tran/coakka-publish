# TLS And mTLS

CoAkka HTTP Runtime can terminate server-authenticated TLS or mutual TLS in
`CoAkka HTTP Runtime`. Every listener declares its protocol, security
mode, stable credential identity, credential generation, certificate chain,
private key, and, for mutual TLS, client trust roots.

## Contents

- [Install The Private Candidate](#install-the-private-candidate)
- [Security Modes](#security-modes)
- [TLS Sample](#tls-sample)
- [mTLS Sample](#mtls-sample)
- [Language Configuration Map](#language-configuration-map)
- [Protocol And io_uring](#protocol-and-io_uring)
- [Credential Operations](#credential-operations)
- [Verification Checklist](#verification-checklist)

## Install The Private Candidate

The current `1.0.0` train remains private. Install the exact staged artifact
from a local release directory; do not substitute a public registry package.

| Language | Private installation shape |
| --- | --- |
| Python | `python -m pip install ./coakka_http-1.0.0-<platform>.whl` |
| Node.js/Bun | `npm install ./coakka-http-1.0.0.tgz` or `bun add ./coakka-http-1.0.0.tgz` |
| JVM | Place `coakka-http-jvm-1.0.0.jar` under `libs/` and use `implementation(files("libs/coakka-http-jvm-1.0.0.jar"))` |
| Go | Extract the source artifact and use a temporary `replace github.com/phuong-tran/coakka-http-runtime-go => /absolute/extracted/path` while the module publication gate is closed |
| C/C++ | Point `CMAKE_PREFIX_PATH` at the extracted target SDK and consume its installed CMake package |

Match the artifact to the operating system and architecture. Package presence
alone is not protocol evidence; check the packaged capability bits and run a
real handshake on the target host.

## Security Modes

| Mode | Server identity | Client identity | Required listener material |
| --- | --- | --- | --- |
| Plaintext | None | None | TLS fields empty, credential generation zero |
| TLS | Certificate verified by client | Not required | Nonzero generation, credential ID, certificate chain, private key |
| mTLS | Certificate verified by client | Certificate verified by server | TLS fields plus trust-roots file |

Transport authentication does not replace application authorization. With
mTLS, the verified peer identity is an input to App Host or addon policy; the runtime
does not decide which customer or business action that peer may access.

## TLS Sample

The complete Python runtime configuration accepts the listener declaration. This
basic configuration uses HTTP/1.1 over TLS. Check that the exact target package
contains the selected TLS provider before starting; select HTTP/2 or HTTP/3
only when its protocol capability is also present:

```python
from coakka_http import (
    Configuration,
    Listener,
    ListenerProtocol,
    TransportSecurity,
)


with Configuration() as configuration:
    configuration.add_listener(
        Listener(
            listener_id=1,
            bind_address="0.0.0.0",
            port=8443,
            protocol=ListenerProtocol.HTTP_1_1,
            security=TransportSecurity.TLS,
            credential_generation=1,
            credential_id="api-server",
            certificate_chain_file="/run/secrets/server-chain.pem",
            private_key_file="/run/secrets/server-key.pem",
        )
    )
    runtime = configuration.create_core()

runtime.start()
```

The listener reports ready only after configuration and bind validation
succeeds. Add routes before `create_core()`, then run the bounded runtime event
reader/respond lifecycle described in the Python guide. Keep the private key
outside the application image when the deployment platform can mount it as a
secret.

## mTLS Sample

mTLS uses the same portable HTTP/1.1 listener and handler. Change the security
mode and add the trust roots used to verify client certificates:

```python
listener = Listener(
    listener_id=1,
    bind_address="0.0.0.0",
    port=8443,
    protocol=ListenerProtocol.HTTP_1_1,
    security=TransportSecurity.MUTUAL_TLS,
    credential_generation=7,
    credential_id="partner-api",
    certificate_chain_file="/run/secrets/server-chain.pem",
    private_key_file="/run/secrets/server-key.pem",
    trust_roots_file="/run/secrets/client-ca.pem",
)
```

A missing client certificate, untrusted chain, name/policy mismatch, malformed
PEM, key mismatch, or unsupported protocol/security combination fails closed.

## Language Configuration Map

| Language | Listener type | Protocol values | Security values |
| --- | --- | --- | --- |
| Java/Kotlin | `Listener` | `ListenerProtocol.HTTP_1_1`, `HTTP_2`, `HTTP_3` | `TransportSecurity.PLAINTEXT`, `TLS`, `MUTUAL_TLS` |
| Python | `Listener` | `ListenerProtocol.HTTP_1_1`, `HTTP_2`, `HTTP_3` | `TransportSecurity.PLAINTEXT`, `TLS`, `MUTUAL_TLS` |
| JavaScript/TypeScript | `createRuntime({ listener: ... })` | `ListenerProtocol.HTTP_1_1`, `HTTP_2`, `HTTP_3` | `TransportSecurity.PLAINTEXT`, `TLS`, `MUTUAL_TLS` |
| Go | `Listener` in `Config.Listeners` | `ProtocolHTTP11`, `ProtocolHTTP2`, `ProtocolHTTP3` | `SecurityPlaintext`, `SecurityTLS`, `SecurityMutualTLS` |
| C/C++ | `coakka_http_listener_t` | Public listener protocol constants | Public transport security constants |

All language packages copy configuration into the runtime. Application handlers
continue to use the request and response values of their language.

## Protocol And io_uring

Select a protocol and backend only after checking the capability bits of the
exact package on the target host. In the current Linux candidate, explicit
`io_uring` is a supported server backend for eligible HTTP/2 and HTTP/3
configurations. It is not a valid label for an HTTP/1.1 listener, and benchmark
reports keep HTTP/1.1 framework comparisons separate from HTTP/2 TLS backend
A/B measurements.

TLS and mTLS are protocol security modes, not benchmark shortcuts. A valid
test verifies the response, peer trust, selected HTTP version, effective
backend, descriptor/resource return, and negative handshake cases.

## Credential Operations

`credential_generation` and `credential_id` make the active identity visible
without exposing secret material. They do not imply that the current candidate
can rotate listener credentials in place.

Listener credential replacement currently requires creating and starting a
new service instance with the newer credential generation, shifting traffic,
then draining the old instance. Live handler swap is a separate capability and
does not replace TLS identity, protocol, bind address, or listener policy.

## Verification Checklist

- Restrict private-key file ownership and permissions.
- Use an explicit certificate chain and trust store; do not rely on an
  undocumented machine-global store.
- Test correct trust, unknown CA, expired/not-yet-valid certificate, hostname,
  missing client certificate, and mismatched private key.
- Confirm the negotiated protocol and exact package capability bits.
- Keep handshake, connection, request, drain, and stop timeouts finite.
- Exclude keys, certificates, cookies, tokens, and peer-certificate bytes from
  logs and monitor detail.
- Rotate by starting the new generation before draining the old service; do
  not overwrite files beneath a running listener and assume they were reloaded.
