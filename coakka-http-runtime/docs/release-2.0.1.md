# CoAkka HTTP Runtime 2.0.1

This release pairs the native SDK and all five language packages at version
2.0.1. The native C ABI is 10. Download the [native SDK and platform
checksums](../native/releases/2.0.1/) if you use C, C++ or Go.

[Read how connector releases pin the native SDK](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/docs/versions.md).

Direct outbound requests can connect to a valid IPv4 or IPv6 address without
performing a DNS lookup for the address. The configured TCP destination,
HTTP authority, and TLS peer identity retain their separate roles. The fix
also keeps resolved address overrides scoped to the connection that needs
them.

Choose the guide for your application.

- [Go](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/go/README.md)
- [Node.js and Bun](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/javascript/README.md)
- [Python](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/python/README.md)
- [Java and Kotlin](https://github.com/phuong-tran/coakka-publish/blob/main/coakka-http-runtime/jvm/README.md)

Go applications upgrading from 1.x must change the module path and imports to
`github.com/phuong-tran/coakka-http-runtime-go/v2`. Install SDK 2.0.1 and
rebuild the application. JVM applications should update both the JAR and the
native classifier ZIP. Node.js, Bun, and Python installers supply the matching
native files with their packages.
