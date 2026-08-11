# Release Checklist

This checklist is for the `coakka-jvm-native-runtime-v2` lane only.

## Preconditions

- `coakkaCore/v2` builds on the current host
- the staged native set for the platforms you plan to publish exists, or can
  be fetched from the relevant Linux VMs
- local JDK 17 is available
- the staged native package is runtime 2.4.0 and exports capability,
  structured connection/security apply, and complete File Lane and Stream Lane
  symbols

Repo-local staged native layout:

```text
v2/staging/native/<native-package-version>/<platform>/
```

Fetch the staged Linux natives if needed:

```sh
./gradlew :v2:jvm:fetchLinuxAarch64RuntimeNative
./gradlew :v2:jvm:fetchLinuxX8664RuntimeNative
```

Sync the current verified host/Linux natives into repo-local versioned staging:

```sh
./gradlew :v2:jvm:stageRuntimeNativeRepoStaging
```

## Verify The Runtime Module

```sh
./gradlew :v2:jvm:test
./gradlew :v2:jvm:smokePackagedRuntimeJvmJar
```

Expected result:

- tests pass, including File Lane and Stream Lane against the staged native for
  the current host
- packaged-jar smoke prints `runtime_smoke ok ...`
- packaged-jar smoke exercises capability discovery, explicit per-exchange
  startup, and explicit plaintext startup so a stale embedded native fails the
  gate before consumer publication

For a host-only package dry-run before cross-platform staging is complete:

```sh
./gradlew :v2:jvm:verifyRuntimeJvmJarContents \
  -PcoakkaV2HostRuntimeLib=/abs/path/to/current/libcoakka_runtime_v2.dylib
```

This override is an input to the dry-run package only. It does not replace or
promote repo staging.

## Publish To Maven Local

```sh
./gradlew :v2:jvm:publishToMavenLocal
```

Expected local artifacts:

```text
~/.m2/repository/coakka/v2/coakka-jvm-native-runtime-v2/0.1.0/
```

## Verify External Consumer Shape

```sh
./gradlew -p v2/jvm/consumer-smoke run
```

Expected result:

- consumer run prints `consumer_runtime_smoke ok ...`

This step matters because it proves the artifact works through `mavenLocal()`
instead of only through the in-repo project classes.

## Build Dist Output

```sh
./gradlew :v2:jvm:distRuntimeJvm
```

Expected dist folder:

```text
v2/jvm/build/dist/coakka-jvm-native-runtime-v2/
```

Expected files:

- `coakka-jvm-native-runtime-v2-<jvm-artifact-version>.jar`
- `coakka-jvm-native-runtime-v2-<jvm-artifact-version>-sources.jar`
- `SHA256SUMS`
- `README.md`
- `CONSUMING.md`

## Platform Notes

Current release-shaped native coverage:

- `macos-aarch64`
- `linux-aarch64`
- `linux-x86_64`
- `windows-aarch64`
- `windows-x86_64`

Current expectations:

- staged directories should carry the native package version
- staged filenames should stay generic inside that versioned directory
- macOS host build should package `libcoakka_runtime_v2-<version>.dylib`
- Linux host build should package `libcoakka_runtime_v2-<version>.so`
- the jar should embed one runtime library per staged platform
- jar manifest should record the bundled native core version and native package
  version

If either host starts emitting the wrong suffix, fix that in `coakkaCore/v2`
before publishing a new artifact.
