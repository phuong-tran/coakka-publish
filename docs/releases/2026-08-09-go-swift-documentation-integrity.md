# Go And Swift Documentation Integrity Patches

Published Go module `github.com/phuong-tran/coakka-runtime-go@v1.5.1` and
SwiftPM package `https://github.com/phuong-tran/coakka-runtime-swift.git`, exact
`2.1.1`, on August 9, 2026.

Both patches preserve connector behavior and native generation
`2.1.0+60ddf70d`. They replace a private connector-repository file-lane link
with the canonical public contract:

`https://github.com/phuong-tran/coakka-publish/blob/main/docs/runtime-file-transfer.md`

Go `v1.5.1` resolves through the public Go module proxy at source commit
`4dcad02563251a48e1b0650591e7f256493aac98`. Its tests require the canonical
link and reject private repository names in the packaged README.

Swift `v2.1.1` resolves through its public Git tag at source commit
`45375fbfc540932a9708d520112efe1346c52d07`. Swift package readiness requires
the canonical link and rejects private repository names in exported metadata.

Clean Go and Swift consumers load native runtime `2.1.0+60ddf70d` and complete
request/reply on macOS ARM64. All five bundled native payloads remain unchanged.
