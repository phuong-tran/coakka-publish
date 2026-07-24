# 2026-07-25 Runtime Stop Backpressure Hotfix 8ff6f32

This hotfix refreshes every public runtime lane over native runtime generation
`1.3.1+0da8c2d9`.

The native fix closes runtime-owned terminal write lanes before joining
outbound writer threads during `runtime_stop()`. This prevents shutdown from
blocking when a terminal lane is full and no host reader is draining it.

| Surface | Public generation |
| --- | --- |
| Native C ABI | `1.3.1+0da8c2d9` |
| JVM runtime | `1.3.1-g0da8c2d9-8ff6f32` |
| Spring Boot adapter | `1.3.1-g0da8c2d9-8ff6f32` |
| Quarkus adapter | `1.3.1-g0da8c2d9-8ff6f32` |
| Python runtime | `1.3.1+0da8c2d9-8ff6f32` |
| Node.js runtime | `1.3.6` |
| Bun runtime | `1.3.6` |
| Electron runtime | `1.3.6` |
| Go runtime | `1.3.1+0da8c2d9-8ff6f32` |
| C# runtime | `1.3.1+0da8c2d9-8ff6f32` |
| Rust runtime | `1.3.1+0da8c2d9-8ff6f32` |
| Mojo runtime | `1.3.1+0da8c2d9-8ff6f32` |
| Zig runtime | `1.3.1+0da8c2d9-8ff6f32` |
| Tauri runtime | `1.3.1+0da8c2d9-8ff6f32` |

Verification gates:

```sh
scripts/verify-public-surface.sh
scripts/test-npm-package-manager-candidates.sh package-manager/npm/candidates/8ff6f32/manifest.json
scripts/publish-npm-package-manager-candidates.sh \
  --manifest package-manager/npm/candidates/8ff6f32/manifest.json \
  --dry-run
```

The npm candidate set is staged under
`package-manager/npm/candidates/8ff6f32/` and must be published with current
npm 2FA before the npm registry surface is considered live.
