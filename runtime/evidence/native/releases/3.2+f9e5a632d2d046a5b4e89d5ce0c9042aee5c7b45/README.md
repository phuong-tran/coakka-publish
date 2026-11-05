# CoAkka Runtime Test for Core 3.2

Use this runner to exercise CoAkka Runtime through its public C interface.
Choose the archive for your operating system and CPU. The runner includes the
same Community Core 3.2 library as the SDK and needs no compiler.

This runner is paired with SDK release `3.2+f9e5a632d2d046a5b4e89d5ce0c9042aee5c7b45`.
For the source, build instructions and additional checks, see
[CoAkka Runtime Test](../../../../../coakka-runtime/coakka-runtime-test/README.md).

## Run the smoke test

Verify the archive against `SHA256SUMS`, then extract it and open a terminal
in the extracted directory. On Linux, choose your platform directory and run:

```sh
LD_LIBRARY_PATH="$PWD/native/linux-aarch64" \
  ./bin/coakka-runtime-native-evidence smoke --requests 128
```

Use `native/linux-x86_64` on Linux x86-64. On macOS ARM64:

```sh
DYLD_LIBRARY_PATH="$PWD/native/macos-aarch64" \
  ./bin/coakka-runtime-native-evidence smoke --requests 128
```

On Windows PowerShell, run this from the extracted directory:

```powershell
$runtimeDirectory = (Get-ChildItem -Directory .\native).FullName
$previousPath = $env:PATH
try {
    $env:PATH = "$runtimeDirectory;$previousPath"
    & .\bin\coakka-runtime-native-evidence.exe smoke --requests 128
    if ($LASTEXITCODE -ne 0) { throw "CoAkka Runtime smoke test failed" }
} finally {
    $env:PATH = $previousPath
}
```

The smoke test exits with status zero when all admitted requests receive a
reply. Its JSON reports Runtime version `3.2` and ABI `1`. Keep the
`native/` folder with `bin/` when you move the runner.

The other executables in `bin/` cover concurrent requests, route updates,
connection configuration, and the File and Stream interfaces. The smoke
executable also accepts `pressure` to check admission under load.
See the [scope of these checks](SYSTEMS-AUDIT.md) for details.
