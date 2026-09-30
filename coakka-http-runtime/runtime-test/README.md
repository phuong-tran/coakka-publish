# CoAkka HTTP Runtime Installed-Package Verification

Release verification consumes the installed target package through
`<coakka/http/host.h>` and `CoAkka::HttpHost`. Tests and their private sources
are release evidence; they are not installed in the consumer package.

## Required gates

For every target, the release process verifies:

- the package contains only the focused public header, shared-library files,
  CMake metadata, and legally required license material;
- architecture, source identity, artifact hashes, and target metadata agree;
- the shared library exports exactly the approved host API and has no
  unapproved dynamic dependencies;
- private symbols, build paths, debug data, and internal vocabulary are absent;
- independent C11 and C++20 consumers configure and link from the installed
  prefix;
- the service starts, serves real loopback traffic, reports operational truth,
  drains, and stops on the matching host; and
- language connectors consume that same installed library without another HTTP
  data path or operating-system capability probe.

Static analysis and matching-host stress evidence remain in the private source
repositories. Absence of a required tool or target is recorded as blocked, not
as a passing result. These gates establish packaging and behavior; they are not
throughput benchmark evidence.
