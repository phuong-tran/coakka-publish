# CoAkka Runtime Test coverage for Core 3.2

This runner is paired with SDK release `3.2+f9e5a632d2d046a5b4e89d5ce0c9042aee5c7b45`.
Its archive receipts record the SDK archive and bundled library checksums,
along with the exact revision of the public test source.

| Check | What it exercises |
| --- | --- |
| Smoke and pressure | Request and reply, bounded admission, and terminal results. |
| Concurrency | Concurrent requests, stop while callers are active, and replacement of route snapshots. |
| Connection configuration | Accepted and rejected connection settings through the public C interface. |
| File | Completion, receipt, and progress for a file transfer. |
| Stream | Frame order, pressure, and cleanup after a transfer. |

These are bounded correctness checks for the bundled Community build.
Run the [source suite](../../../../../coakka-runtime/coakka-runtime-test/README.md)
for additional controls. The runner makes no benchmark or TLS coverage claim.
