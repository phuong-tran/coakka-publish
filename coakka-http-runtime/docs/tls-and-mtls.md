# TLS And mTLS

CoAkka HTTP Runtime can terminate server-authenticated TLS or mutual TLS in
`CoAkka HTTP Runtime`. Every listener declares its protocol, security
mode, stable credential identity, credential generation, certificate chain,
private key, and, for mutual TLS, client trust roots.

## Contents

- [Install The Repository Package](#install-the-repository-package)
- [Security Modes](#security-modes)
- [TLS Sample](#tls-sample)
- [mTLS Sample](#mtls-sample)
- [Language Configuration Map](#language-configuration-map)
- [Protocol And io_uring](#protocol-and-io_uring)
- [Credential Operations](#credential-operations)
- [What can be hot reloaded](#what-can-be-hot-reloaded)
- [Rotate certificates with a replacement instance](#rotate-certificates-with-a-replacement-instance)
- [Verification Checklist](#verification-checklist)

## Install The Repository Package

Install the exact `1.0.0` archive for your platform from the repository.
Do not substitute a similarly named registry package.

| Language | Installation guide |
| --- | --- |
| Python | [Extracted module and matching library](../python/README.md); no wheel is published in this train |
| Node.js/Bun | [Shared JavaScript archive](../javascript/README.md) |
| JVM | [JAR and matching native libraries](../jvm/README.md) |
| Go | [Module and native bundle](../go/README.md) |
| C/C++ | [Installed CMake package](../native/README.md) |

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

`credential_generation` and `credential_id` identify the selected credential
set without exposing secret material. Current deployments rotate listener
credentials through a replacement service. Live certificate reload is planned
for a future version; see the [roadmap](../roadmap.md#certificate-lifecycle).

Listener credential replacement currently requires creating and starting a
new service instance with the newer credential generation, shifting traffic,
then draining the old instance. Live handler swap is a separate capability and
does not replace TLS identity, protocol, bind address, or listener policy.

`credential_id` names the logical identity; `credential_generation` identifies
the deployment's selected revision of its material. Neither is a file watcher.
Incrementing the number, overwriting a PEM file, replacing a symlink or swapping
a route handler does not instruct an existing listener to reload its identity.
Do not mistake a configured version for proof of a successful handshake.

## What can be hot reloaded

Use the live-update APIs for routes and monitor policy today. Certificate
reload is on the roadmap; until that API ships, use the replacement procedure
below for certificate and trust-root changes.

| Change | Current packaged path | Important bound |
| --- | --- | --- |
| Replace a route handler | Prepare a binding, then generation/revision-checked apply | Captured work retains the old binding until it finishes. |
| Replace the route table | Publish a complete accepted structural generation | Rejected candidates leave the prior effective table intact. |
| Change monitor collection policy | Expected-generation policy apply | Cannot enlarge startup reservations; unsupported intent is refused. |
| Replace listener certificate/key or client trust roots | Start a replacement service, validate it, shift traffic, drain the old service | Live certificate reload is planned for a future version. |
| Change listener address or protocol | Construct a replacement service | A handler change does not rebind a listener. |

These are independent contracts, not one universal reload switch. For why
compare-and-apply versions matter, see [generations](glossary.md#why-generations-are-needed).

## Rotate certificates with a replacement instance

1. Prepare a distinct credential set with restricted access. Keep the chain,
   matching private key and intended trust roots together; do not mutate files
   beneath the live instance. Check validity, names, chain and key matching.
2. Construct the replacement using its new credential generation and file
   paths. Use another endpoint or machine while the old listener remains live;
   do not assume two services can bind the same address and port.
3. Before shifting traffic, perform a real handshake with the intended peer
   trust and name, followed by a request. For mTLS, also verify that a client
   without a trusted identity is rejected. A successful startup alone is not
   end-to-end rotation evidence.
4. Admit the replacement to ingress, stop selecting the old instance for new
   traffic, then invoke the old service's graceful close contract. Established
   sessions do not move to the replacement automatically.
5. Observe drain/close outcomes before removing old process resources. Retain
   rollback material only under the deployment's security and retention policy;
   never roll back to an expired, compromised or otherwise revoked identity.

If a replacement fails validation, leave it out of ingress. Do not change the
old instance's effective state just to force a rollout through. When only one
address/process slot is available, a stop-and-start rotation can create a
service interruption; zero downtime is not implied by this recipe.

The Kotlin builder shape below is drawn from the packaged
[security sample](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-http-runtime/kotlin/src/main/kotlin/sample/Security.kt).
It creates a **new** service; it does not mutate a running one. Provide the
credential files first and put the returned service under the application's
checked close/finally owner, as in the complete sample.

```kotlin
import coakka.http.Handler
import coakka.http.Listener
import coakka.http.ListenerProtocol
import coakka.http.Responses
import coakka.http.ServiceBuilder
import coakka.http.TransportSecurity

val replacement = ServiceBuilder()
    .listener(Listener(
        bindAddress = "127.0.0.1",
        port = 8444,
        protocol = ListenerProtocol.HTTP_1_1,
        security = TransportSecurity.TLS,
        credentialId = "api-server",
        credentialGeneration = 8L,
        certificateChainFile = "/run/secrets/api-v8/server-chain.pem",
        privateKeyFile = "/run/secrets/api-v8/server-key.pem",
    ))
    .get("/secure", Handler { Responses.text("ready") })
    .start()
// Deployment owner verifies the TLS handshake before changing ingress.
// It must close replacement on refusal or on its eventual retirement.
```

For an mTLS CA transition, plan client and server trust changes together.
An overlap window may be necessary to keep old and new valid peers connected,
but it also broadens accepted trust during that window; make its duration and
removal explicit. Updating trust in a replacement does not retroactively
re-authenticate existing connections. A compromised credential may require
terminating old sessions rather than giving them a normal drain interval.

If TLS terminates at a reverse proxy instead, certificate rotation belongs to
that proxy; it does not demonstrate CoAkka listener hot reload. Document the
backend transport/trust boundary separately. See
[deployment without Kubernetes](deployment-without-kubernetes.md) for ingress
and drain responsibilities. This is an operational recipe, not a claim that
an automated rotation controller or multi-node rotation test ships in the SDK.

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
