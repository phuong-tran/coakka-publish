# JavaScript Release Evidence

- Source: `coakka-http-runtime-connector@26a28a1c4af3eb65334993532b4eaa082fc75438`.
- Native bridges compile warnings-as-errors; ESLint, JSDoc and strict
  TypeScript consumption pass.
- The exact aggregate tarball passes installed public API, low-level Core and
  environment-cleanup tests three times per runtime on macOS Node and Bun.
- The same tarball passes three Node rounds on Raspberry Pi 5, Linux x86-64
  UTM, Windows ARM64 Node and Windows x64 Node. The Linux ARM64 package also
  passes Bun three times; the x86-64 addon has separate clean Bun evidence.
- Every addon has the advertised ISA and imports only its adjacent Core plus
  OS/Node host libraries. The loader verifies addon and Core size plus SHA-256.

No npm coordinate, signing or portable performance claim is included.
