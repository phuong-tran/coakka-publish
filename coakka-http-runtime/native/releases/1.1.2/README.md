# CoAkka HTTP Runtime Native SDK 1.1.2

Choose the archive for your operating system and architecture. The five
archives contain the native library and public C headers used by C and C++
consumers. WSL2 uses the Linux x86-64 archive.

- [Linux x86-64 (including WSL2)](coakka-http-host-1.1.2-candidate-linux-x86_64.tar.gz?raw=true)
- [Linux ARM64](coakka-http-host-1.1.2-candidate-linux-aarch64.tar.gz?raw=true)
- [macOS ARM64](coakka-http-host-1.1.2-candidate-macos-aarch64.tar.gz?raw=true)
- [Windows x86-64](coakka-http-host-1.1.2-candidate-windows-x86_64.tar.gz?raw=true)
- [Windows ARM64](coakka-http-host-1.1.2-candidate-windows-aarch64.tar.gz?raw=true)

Verify the archive against [SHA256SUMS](SHA256SUMS), then extract it into
your SDK directory and set `COAKKA_HTTP_HOST_PREFIX` to the extracted SDK
prefix. Use the [installed SDK conformance harness](../../../runtime-test/README.md)
to check the installation.
