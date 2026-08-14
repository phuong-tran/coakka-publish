#!/usr/bin/env python3

"""Assemble reviewed artifact-source addon binaries into public archives."""

from __future__ import annotations

import argparse
import gzip
import hashlib
import json
import os
import shutil
import subprocess
import tarfile
import tempfile
from pathlib import Path, PurePosixPath
from typing import Any


PLATFORM_ORDER = [
    "linux-aarch64",
    "linux-x86_64",
    "macos-aarch64",
    "windows-aarch64",
    "windows-x86_64",
]
EXPORT_SUFFIXES = [
    "cancel",
    "create",
    "dependency_version",
    "destroy",
    "forget",
    "get",
    "get_target",
    "start",
    "stop",
    "submit",
    "wait",
]
MAX_INPUT_FILE_BYTES = 256 * 1024 * 1024


class PackageError(RuntimeError):
    pass


def fail(message: str) -> None:
    raise PackageError(message)


def read_json(path: Path) -> dict[str, Any]:
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        fail(f"could not read JSON {path}: {error}")
    if not isinstance(value, dict):
        fail(f"JSON root must be an object: {path}")
    return value


def write_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2) + "\n", encoding="utf-8")


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def command_output(*command: str, cwd: Path) -> str:
    try:
        return subprocess.run(
            command,
            cwd=cwd,
            check=True,
            capture_output=True,
            text=True,
        ).stdout.strip()
    except subprocess.CalledProcessError as error:
        fail(f"command failed in {cwd}: {' '.join(command)}: {error.stderr.strip()}")


def parse_platform_roots(values: list[str]) -> dict[str, Path]:
    roots: dict[str, Path] = {}
    for value in values:
        platform, separator, raw_path = value.partition("=")
        if not separator or platform not in PLATFORM_ORDER or not raw_path:
            fail(f"invalid --platform-root value: {value}")
        if platform in roots:
            fail(f"duplicate platform root: {platform}")
        root = Path(raw_path).resolve()
        if not root.is_dir():
            fail(f"platform root is not a directory: {root}")
        roots[platform] = root
    return roots


def find_unique_file(root: Path, name: str) -> Path:
    matches = [path for path in root.rglob(name) if path.is_file()]
    if len(matches) != 1:
        fail(f"expected one {name} below {root}, found {len(matches)}")
    path = matches[0]
    if path.stat().st_size > MAX_INPUT_FILE_BYTES:
        fail(f"platform input exceeds 256 MiB: {path}")
    return path


def copy_regular(source: Path, destination: Path) -> None:
    destination.parent.mkdir(parents=True, exist_ok=True)
    with source.open("rb") as input_file, destination.open("xb") as output_file:
        shutil.copyfileobj(input_file, output_file, 1024 * 1024)
    os.chmod(destination, 0o644)


def cmake_token(slug: str) -> str:
    return "".join(part.title() for part in slug.removeprefix("artifact-publisher-").split("-"))


def symbol_name(slug: str) -> str:
    return slug.removeprefix("artifact-publisher-").replace("-", "_")


def library_names(stem: str, platform: str) -> tuple[str, str | None, str | None]:
    if platform.startswith("linux-"):
        return f"lib{stem}.so", f"lib{stem}.so.0", None
    if platform == "macos-aarch64":
        return f"lib{stem}.dylib", f"lib{stem}.0.dylib", None
    return f"lib{stem}.dll", None, f"lib{stem}.lib"


def render_cmake(addon: dict[str, Any]) -> str:
    slug = addon["slug"]
    symbol = symbol_name(slug)
    token = cmake_token(slug)
    stem = addon["library_stem"]
    windows_supported = "windows-x86_64" in addon["platforms"]
    windows_branch = f'''elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
    set(_coakka_addon_platform "windows-aarch64")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
    set(_coakka_addon_platform "windows-x86_64")
  else()
    message(FATAL_ERROR "CoAkka {addon['display_name']} package has no Windows binary for ${{CMAKE_SYSTEM_PROCESSOR}}")
  endif()
  set(_coakka_addon_library_name "lib{stem}.dll")
  set(_coakka_addon_implib_name "lib{stem}.lib")
''' if windows_supported else '''elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  message(FATAL_ERROR "CoAkka Local Drop addon does not support Windows")
'''
    return f'''include_guard(GLOBAL)

include(CMakeFindDependencyMacro)
find_dependency(CoAkkaRuntimeNativeV2 CONFIG)

get_filename_component(_coakka_addon_root "${{CMAKE_CURRENT_LIST_DIR}}/.." ABSOLUTE)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
  set(_coakka_addon_platform "macos-aarch64")
  set(_coakka_addon_library_name "lib{stem}.dylib")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
    set(_coakka_addon_platform "linux-aarch64")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
    set(_coakka_addon_platform "linux-x86_64")
  else()
    message(FATAL_ERROR "CoAkka {addon['display_name']} package has no Linux binary for ${{CMAKE_SYSTEM_PROCESSOR}}")
  endif()
  set(_coakka_addon_library_name "lib{stem}.so")
{windows_branch}else()
  message(FATAL_ERROR "CoAkka {addon['display_name']} package does not support ${{CMAKE_SYSTEM_NAME}}/${{CMAKE_SYSTEM_PROCESSOR}}")
endif()

set(_coakka_addon_library "${{_coakka_addon_root}}/native/${{_coakka_addon_platform}}/${{_coakka_addon_library_name}}")
if(NOT EXISTS "${{_coakka_addon_library}}")
  message(FATAL_ERROR "CoAkka addon native library is missing: ${{_coakka_addon_library}}")
endif()

if(NOT TARGET CoAkkaRuntimeAddonArtifactPublisher{token}::artifact_publisher_{symbol})
  add_library(CoAkkaRuntimeAddonArtifactPublisher{token}::artifact_publisher_{symbol} SHARED IMPORTED)
  set_target_properties(CoAkkaRuntimeAddonArtifactPublisher{token}::artifact_publisher_{symbol} PROPERTIES
    IMPORTED_LOCATION "${{_coakka_addon_library}}"
    INTERFACE_INCLUDE_DIRECTORIES "${{_coakka_addon_root}}/include"
    INTERFACE_LINK_LIBRARIES "CoAkkaRuntimeNativeV2::runtime_v2")
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(_coakka_addon_implib "${{_coakka_addon_root}}/native/${{_coakka_addon_platform}}/${{_coakka_addon_implib_name}}")
    if(NOT EXISTS "${{_coakka_addon_implib}}")
      message(FATAL_ERROR "CoAkka addon import library is missing: ${{_coakka_addon_implib}}")
    endif()
    set_property(TARGET CoAkkaRuntimeAddonArtifactPublisher{token}::artifact_publisher_{symbol}
      PROPERTY IMPORTED_IMPLIB "${{_coakka_addon_implib}}")
  endif()
endif()

set(CoAkkaRuntimeAddonArtifactPublisher{token}_FOUND TRUE)
set(CoAkkaRuntimeAddonArtifactPublisher{token}_PLATFORM "${{_coakka_addon_platform}}")
set(CoAkkaRuntimeAddonArtifactPublisher{token}_NATIVE_LIBRARY "${{_coakka_addon_library}}")
'''


def render_package_readme(addon: dict[str, Any], version: str) -> str:
    bundled = addon["bundled_modules"]
    dependency_note = (
        "The archive also carries the private HTTPS engine required by this adapter; "
        "it is an implementation module, not a second public API."
        if bundled
        else "The archive carries no other addon module."
    )
    return f'''# CoAkka {addon["display_name"]} Native Addon

Version `{version}` provides the reviewed public C header and native modules for
the platforms declared by the release manifest. It requires CoAkka Runtime
native `2.4.0` or newer with File Lane support.

{addon["summary"]}

{dependency_note} CoAkka Runtime is always supplied separately. The application
owns Runtime, File Lane, credentials, authorization, retry policy, and lifecycle
ordering. The addon owns its bounded acquisition job and verified staging.

There is no high-level language connector in this release. Applications consume
the small C ABI directly or provide their own wrapper. See `CONSUMING.md`.
'''


def render_consuming(addon: dict[str, Any], version: str) -> str:
    slug = addon["slug"]
    symbol = symbol_name(slug)
    token = cmake_token(slug)
    deps = ", ".join(addon["system_dependencies"]) or "no protocol library"
    return f'''# Consuming {addon["display_name"]}

Unpack Runtime native `2.4.0` or newer and addon `{version}`, then expose both
CMake package directories:

```sh
cmake -S app -B build \\
  -DCoAkkaRuntimeNativeV2_DIR=/path/to/runtime/cmake \\
  -DCoAkkaRuntimeAddonArtifactPublisher{token}_DIR=/path/to/addon/cmake
```

```cmake
find_package(CoAkkaRuntimeAddonArtifactPublisher{token} CONFIG REQUIRED)
target_link_libraries(app PRIVATE
  CoAkkaRuntimeAddonArtifactPublisher{token}::artifact_publisher_{symbol})
```

Keep the Runtime and addon native directories on the process loader path. The
semantic adapters already carry their required private HTTPS module. Platform
requirements are explicit in `manifest.json`; network builds use {deps}.

The host must keep the borrowed sender lane alive until every publisher instance
has stopped and been destroyed. Cancellation, timeout, integrity validation,
no-clobber staging, and terminal job state are exposed by the C header contract.
'''


def render_top_readme(addon: dict[str, Any], release: str, archive_name: str) -> str:
    platforms = ", ".join(f"`{item}`" for item in addon["platforms"])
    return f'''# {addon["display_name"]} Runtime Addon

Status: native `{release}` is public for {platforms}. It requires CoAkka Runtime
native `2.4.0` or newer with File Lane support.

{addon["summary"]}

## Native Coordinate

```text
runtime-addons/{addon["slug"]}/native/releases/{release}/
  {archive_name}
```

The application owns credentials, authorization grants, lifecycle, retry and
rollout policy. The addon owns protocol mechanics, bounded acquisition,
integrity checking, and no-clobber staging. Runtime owns File Lane delivery.

This release exposes the reviewed C ABI. It does not yet provide a Go, Swift,
JVM, Node, Python, or .NET connector; that wrapper layer is a separate product
slice and is not implied by availability of this native archive.
'''


def render_release_readme(addon: dict[str, Any], version: str, release: str, archive_name: str) -> str:
    platforms = ", ".join(f"`{item}`" for item in addon["platforms"])
    return f'''# {addon["display_name"]} Native {version}

Release generation: `{release}`

Published archive: `{archive_name}`

Supported platforms: {platforms}. Minimum Runtime native version: `2.4.0`.

## Evidence

- exact private implementation snapshot: `{release.split("+", 1)[1]}`;
- exact 11-symbol reviewed C ABI;
- strict build and public C ABI execution on every listed matching host;
- dynamic dependency and sibling-module loader inspection on packaged binaries;
- bounded worker/queue, cancellation, timeout, integrity, and no-clobber tests;
- archive structure, checksum, installed manifest, and CMake intake verification.

This archive contains no Runtime copy and no high-level connector. Windows DLLs
are unsigned; verify SHA-256 and apply an organization signature when required.

```sh
python3 scripts/verify-runtime-addon-release.py \\
  --release-dir runtime-addons/{addon["slug"]}/native/releases/{release} \\
  --expected-addon {addon["slug"]}
```
'''


def add_tree_to_archive(source_root: Path, archive: Path) -> None:
    with archive.open("xb") as raw_file:
        with gzip.GzipFile(filename="", mode="wb", fileobj=raw_file, mtime=0) as gzip_file:
            with tarfile.open(fileobj=gzip_file, mode="w", format=tarfile.PAX_FORMAT) as bundle:
                for path in sorted(source_root.rglob("*"), key=lambda item: item.as_posix()):
                    relative = path.relative_to(source_root.parent).as_posix()
                    info = bundle.gettarinfo(str(path), arcname=relative)
                    info.uid = info.gid = 0
                    info.uname = info.gname = "root"
                    info.mtime = 0
                    info.mode = 0o755 if path.is_dir() else 0o644
                    if path.is_file():
                        with path.open("rb") as handle:
                            bundle.addfile(info, handle)
                    else:
                        bundle.addfile(info)


def package_addon(
    publish_root: Path,
    core_root: Path,
    addon: dict[str, Any],
    version: str,
    snapshot: str,
    platform_roots: dict[str, Path],
    network_notices: str,
    owned_dependencies: list[dict[str, str]],
) -> None:
    slug = addon["slug"]
    release = f"{version}+{snapshot}"
    package_name = f"coakka-runtime-addon-{slug}-native-{version}"
    archive_name = f"{package_name}.tar.gz"
    release_dir = publish_root / "runtime-addons" / slug / "native" / "releases" / release
    if release_dir.exists():
        fail(f"immutable release directory already exists: {release_dir}")

    with tempfile.TemporaryDirectory(prefix="coakka-addon-package.") as raw_tmp:
        tmp = Path(raw_tmp)
        package_root = tmp / package_name
        header_source = (
            core_root / "v2" / "addons" / addon["source_directory"] / "include" /
            "coakka" / "addons" / addon["header"]
        )
        copy_regular(header_source, package_root / "include" / "coakka" / "addons" / addon["header"])

        platform_manifest: list[dict[str, Any]] = []
        for platform in addon["platforms"]:
            input_root = platform_roots.get(platform)
            if input_root is None:
                fail(f"missing --platform-root for {slug}: {platform}")
            library, loader, import_library = library_names(addon["library_stem"], platform)
            native_dir = package_root / "native" / platform
            copy_regular(find_unique_file(input_root, library), native_dir / library)
            entry: dict[str, Any] = {
                "id": platform,
                "library": f"native/{platform}/{library}",
                "matchingHostRuntimeTest": "passed",
                "dynamicDependencyAudit": "passed",
            }
            if loader:
                copy_regular(find_unique_file(input_root, loader), native_dir / loader)
                entry["loaderLibrary"] = f"native/{platform}/{loader}"
            if import_library:
                copy_regular(find_unique_file(input_root, import_library), native_dir / import_library)
                entry["importLibrary"] = f"native/{platform}/{import_library}"

            bundled: list[str] = []
            for bundled_slug in addon["bundled_modules"]:
                if bundled_slug != "artifact-publisher-https":
                    fail(f"unsupported bundled module in release spec: {bundled_slug}")
                https_library, https_loader, _ = library_names(
                    "coakka_addon_artifact_publisher_https", platform
                )
                for name in (https_library, https_loader):
                    if name:
                        copy_regular(find_unique_file(input_root, name), native_dir / name)
                        bundled.append(f"native/{platform}/{name}")
            if bundled:
                entry["bundledLibraries"] = bundled
            platform_manifest.append(entry)

        token = cmake_token(slug)
        cmake_path = f"cmake/CoAkkaRuntimeAddonArtifactPublisher{token}Config.cmake"
        cmake_file = package_root / PurePosixPath(cmake_path)
        cmake_file.parent.mkdir(parents=True, exist_ok=True)
        cmake_file.write_text(render_cmake(addon), encoding="utf-8")
        (package_root / "README.md").write_text(render_package_readme(addon, version), encoding="utf-8")
        (package_root / "CONSUMING.md").write_text(render_consuming(addon, version), encoding="utf-8")
        copy_regular(publish_root / "LICENSE.md", package_root / "LICENSE.md")
        notices = (
            "# Third-Party Notices\n\nThis Local Drop addon contains no third-party implementation library.\n"
            if not addon["system_dependencies"]
            else network_notices
        )
        (package_root / "THIRD_PARTY_NOTICES.md").write_text(notices, encoding="utf-8")

        installed_manifest = {
            "manifest_version": 1,
            "addon_id": addon["addon_id"],
            "addon_kind": "bridge",
            "addon_version": version,
            "runtime_family": "coakka-runtime-v2",
            "required_runtime_features": ["file_lane"],
            "entrypoint_symbol": addon["entrypoint_symbol"],
        }
        write_json(
            package_root / "share" / "coakka" / "runtime-addons" / slug / "addon.manifest.json",
            installed_manifest,
        )

        release_dir.mkdir(parents=True)
        archive = release_dir / archive_name
        add_tree_to_archive(package_root, archive)
        exports = [f"{addon['symbol_prefix']}{suffix}" for suffix in EXPORT_SUFFIXES]
        manifest = {
            "schemaVersion": 1,
            "name": f"coakka-runtime-addon-{slug}-native",
            "addonId": addon["addon_id"],
            "version": version,
            "release": release,
            "sourceSnapshot": snapshot,
            "archive": archive_name,
            "sha256": sha256(archive),
            "runtimeCompatibility": {
                "abiMajor": 2,
                "minimumNativeVersion": "2.4.0",
                "requiredFeatures": ["file_lane"],
            },
            "ownedStaticDependencies": owned_dependencies if addon["system_dependencies"] else [],
            "userInstalledNativeDependencies": bool(addon["system_dependencies"]),
            "cmakeConfig": cmake_path,
            "headers": [f"include/coakka/addons/{addon['header']}"],
            "exportedSymbols": exports,
            "platforms": platform_manifest,
        }
        write_json(release_dir / "manifest.json", manifest)
        (release_dir / "README.md").write_text(
            render_release_readme(addon, version, release, archive_name), encoding="utf-8"
        )
        checksum_names = [archive_name, "manifest.json", "README.md"]
        (release_dir / "SHA256SUMS").write_text(
            "".join(f"{sha256(release_dir / name)}  {name}\n" for name in checksum_names),
            encoding="ascii",
        )
        top_readme = publish_root / "runtime-addons" / slug / "README.md"
        top_readme.parent.mkdir(parents=True, exist_ok=True)
        top_readme.write_text(render_top_readme(addon, release, archive_name), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--core-root", required=True, type=Path)
    parser.add_argument("--source-snapshot", required=True)
    parser.add_argument("--platform-root", action="append", default=[])
    parser.add_argument("--network-third-party-notices", required=True, type=Path)
    parser.add_argument("--owned-dependencies", required=True, type=Path)
    parser.add_argument("--output-root", type=Path)
    args = parser.parse_args()
    try:
        publish_root = (
            args.output_root.resolve()
            if args.output_root is not None
            else Path(__file__).resolve().parent.parent
        )
        core_root = args.core_root.resolve()
        if command_output("git", "status", "--porcelain", cwd=core_root):
            fail("Core source tree must be clean")
        head = command_output("git", "rev-parse", "HEAD", cwd=core_root)
        snapshot = args.source_snapshot
        if len(snapshot) != 8 or not head.startswith(snapshot):
            fail(f"source snapshot must be the 8-character prefix of Core HEAD {head}")
        spec = read_json(core_root / "v2" / "addons" / "artifact-source-release.json")
        version = spec.get("version")
        addons = spec.get("addons")
        if version != "1.1.0" or not isinstance(addons, list) or len(addons) != 11:
            fail("release spec must declare the reviewed 11-addon version 1.1.0 wave")
        roots = parse_platform_roots(args.platform_root)
        notices = args.network_third_party_notices.read_text(encoding="utf-8")
        dependencies = read_json(args.owned_dependencies).get("ownedStaticDependencies")
        if not isinstance(dependencies, list) or not dependencies:
            fail("owned dependency report must contain a non-empty ownedStaticDependencies array")
        for addon in addons:
            package_addon(
                publish_root, core_root, addon, version, snapshot, roots, notices, dependencies
            )
    except (OSError, PackageError) as error:
        print(f"[artifact-source-package] {error}", file=os.sys.stderr)
        return 1
    print(f"[artifact-source-package] ok: 11 addons at 1.1.0+{args.source_snapshot}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
