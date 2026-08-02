# Root-Level Native Runtime Test Surface

Date: 2026-08-02

The auditable native C11 consumer harness is now visible at
[`runtime-test/`](../../runtime-test/README.md) in `coakka-publish` and at the
same root-level path in `coakka-samples`.

The harness uses only the public CoAkka Runtime C ABI. It covers finite
request/reply behavior, bounded-admission rejection and deadletters, all four
connection strategies, structured atomic rejection, lifecycle immutability,
and machine-readable result contracts. It does not include private core source
or claim that its local throughput is a production benchmark.

Linux ARM64 CI builds the public harness against the exact released 1.4.0
archive with strict warnings, runs Clang static analysis, and executes both the
workload and four-mode connection-strategy contracts with AddressSanitizer and
UndefinedBehaviorSanitizer. Windows and macOS remain first-class portability
and correctness targets; Windows sanitizer evidence is not implied.

Historical prebuilt runners and their release metadata remain unchanged under
[`runtime/evidence/native/releases/`](../../runtime/evidence/native/releases/).
The former source path contains only a compatibility redirect, so there is one
current public source projection and one SHA-256 manifest to audit.
