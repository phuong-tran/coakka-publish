#!/usr/bin/env python3
"""Verify the native-only CoAkka HTTP Runtime release candidate."""

from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import sys


SOURCE_REVISION = "b986565104b6cc9ca4750374ce965ddd2a482ba5"
CONNECTOR_REVISION = "70e7d160282b7547146b63c27bc08baa174561b4"
SAMPLES_REVISION = "b43ca59257664a493368577443625f47aab2d809"
DOCUMENTATION_REVISION = "cb671a643b0d3baf62f4e2b0b69922854a60cc64"
RELEASE_ID = f"1.0.0+{SOURCE_REVISION}"
TARGETS = (
    "macos-aarch64",
    "linux-aarch64",
    "linux-x86_64",
    "windows-aarch64",
    "windows-x86_64",
)
# These identities come from matching-host package gates, not from the
# release manifest being checked. Add the Linux ARM64 value only after its
# refreshed package passes on the physical Raspberry Pi 5.
QUALIFIED_BINARY_SHA256 = {
    "macos-aarch64": "51b392029a37aa4a079e289e121a636dfe8b258f64822d7e69429cf9f11a2c4b",
    "linux-x86_64": "19d55ca26a07e7a47ac066726417e804ee0a74943eda0f560f9ef43402b70cc0",
    "windows-aarch64": "e295de7b89e61fd1f6f12124f39bec2566cfc10b9a017645fd9fe37fb6e35ca4",
    "windows-x86_64": "f62a7d45096eebc28e280849282226df422c362faf9851a075b3bfedca45353e",
}
QUALIFIED_HEADER_SHA256 = "bebde4948d58598e4b2b6491ff6fa9b1c078e80ad5050021fe39ac58d3f1ae2d"
# Pin this only after the cooled physical-board campaign is reviewed.
QUALIFIED_BENCHMARK_SHA256: str | None = None
LEGAL_FILES = ("LICENSE", "NATIVE-LICENSE.md", "NOTICE", "PACKAGE-LICENSE.md")
CMAKE_FILES = (
    "CoAkkaHttpHostConfig.cmake",
    "CoAkkaHttpHostConfigVersion.cmake",
    "CoAkkaHttpHostTargets-release.cmake",
    "CoAkkaHttpHostTargets.cmake",
)
UNIX_LIBRARIES = {
    "macos-aarch64": (
        "lib/libcoakka_http_host.dylib",
        "lib/libcoakka_http_host.1.dylib",
        "lib/libcoakka_http_host.1.0.0.dylib",
    ),
    "linux-aarch64": (
        "lib/libcoakka_http_host.so",
        "lib/libcoakka_http_host.so.1",
        "lib/libcoakka_http_host.so.1.0.0",
    ),
    "linux-x86_64": (
        "lib/libcoakka_http_host.so",
        "lib/libcoakka_http_host.so.1",
        "lib/libcoakka_http_host.so.1.0.0",
    ),
}
WINDOWS_LIBRARIES = (
    "bin/coakka_http_host.dll",
    "lib/coakka_http_host.lib",
)
PUBLIC_DOC_FORBIDDEN = re.compile(
    r"black[- ]?box|callback-free|canonical|conformance|internal execution|"
    r"libuv|usockets|uwebsockets|protobuf|ctypes|node-api|jni|private header|"
    r"wire schema|dependency[ ._-]?closure|coakka-http-runtime-core|"
    r"http-runtime-core|\bcore\b|\babi\b",
    re.IGNORECASE,
)
PACKAGE_FORBIDDEN = re.compile(
    rb"coakka.?http.?core|coakka.?core|runtime.?core|core.?configuration|"
    rb"canonical|black.?box|protobuf|libuv|usockets?|uwebsockets?|boost|absl|"
    rb"openssl|boringssl|nghttp[23]|ngtcp2|curl|c-?ares|liburing|sfparse|"
    rb"utf8.?range|zlib|google(::|/)|coakka\.http\.v[0-9]+\.|"
    rb"coakka-(build|source)|native.?poller|runtime\.h|/Users/|"
    rb"C:\\Work\\|/home/phuong/|coakka-http-host-inlined",
    re.IGNORECASE,
)


def fail(message: str) -> None:
    raise SystemExit(f"[http-runtime-release] {message}")


def digest(path: Path) -> str:
    with path.open("rb") as source:
        return hashlib.file_digest(source, "sha256").hexdigest()


def read_json(path: Path) -> dict[str, object]:
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        fail(f"cannot read JSON {path}: {error}")
    if not isinstance(value, dict):
        fail(f"JSON root must be an object: {path}")
    return value


def require_equal(actual: object, expected: object, context: str) -> None:
    if actual != expected:
        fail(f"{context}: expected {expected!r}, found {actual!r}")


def relative_inventory(root: Path) -> set[str]:
    return {
        path.relative_to(root).as_posix()
        for path in root.rglob("*")
        if path.is_file() or path.is_symlink()
    }


def expected_target_inventory(target: str) -> set[str]:
    result = {"include/coakka/http/host.h"}
    result.update(f"lib/cmake/CoAkkaHttpHost/{name}" for name in CMAKE_FILES)
    result.update(
        f"share/licenses/coakka-http-runtime/{name}" for name in LEGAL_FILES
    )
    if target in UNIX_LIBRARIES:
        result.update(UNIX_LIBRARIES[target])
    else:
        result.update(WINDOWS_LIBRARIES)
    return result


def verify_safe_links(root: Path) -> None:
    resolved_root = root.resolve()
    for path in root.rglob("*"):
        if not path.is_symlink():
            continue
        target = os.readlink(path)
        if os.path.isabs(target):
            fail(f"absolute symlink is forbidden: {path} -> {target}")
        try:
            path.resolve(strict=True).relative_to(resolved_root)
        except (FileNotFoundError, ValueError):
            fail(f"unsafe or broken symlink: {path} -> {target}")


def verify_checksums(release: Path) -> None:
    checksum_path = release / "SHA256SUMS"
    recorded: dict[str, str] = {}
    for line in checksum_path.read_text(encoding="utf-8").splitlines():
        match = re.fullmatch(r"([0-9a-f]{64})  (\./[^\r\n]+)", line)
        if match is None or match.group(2) in recorded:
            fail(f"malformed or duplicate SHA256SUMS entry: {line!r}")
        pure = PurePosixPath(match.group(2)[2:])
        if pure.is_absolute() or ".." in pure.parts:
            fail(f"unsafe checksum path: {match.group(2)}")
        recorded[match.group(2)] = match.group(1)

    expected = {
        f"./{name}"
        for name in relative_inventory(release)
        if name != "SHA256SUMS"
    }
    require_equal(set(recorded), expected, "checksum inventory")
    for relative, expected_hash in recorded.items():
        require_equal(digest(release / relative[2:]), expected_hash, relative)


def verify_public_docs(product: Path) -> None:
    for path in product.rglob("*.md"):
        match = PUBLIC_DOC_FORBIDDEN.search(path.read_text(encoding="utf-8"))
        if match is not None:
            fail(f"public documentation exposes private vocabulary: {path}: {match.group(0)}")


def verify_no_dependency_inventory(product: Path) -> None:
    for path in product.rglob("*"):
        lowered = path.name.lower().replace("_", "-")
        if "third-party" in lowered or "thirdparty" in lowered or "spdx" in lowered:
            fail(f"dependency inventory is forbidden in the release surface: {path}")


def verify_target(
    release: Path,
    target: str,
    target_manifest: object,
    expected_header_hash: str | None,
) -> str:
    if not isinstance(target_manifest, dict):
        fail(f"target manifest must be an object: {target}")
    target_root = release / target
    require_equal(
        relative_inventory(target_root),
        expected_target_inventory(target),
        f"{target} installed inventory",
    )
    verify_safe_links(target_root)

    header = target_root / "include/coakka/http/host.h"
    header_hash = digest(header)
    require_equal(header_hash, QUALIFIED_HEADER_SHA256, f"{target} qualified public header")
    if expected_header_hash is not None:
        require_equal(header_hash, expected_header_hash, f"{target} public header")
    if b"COAKKA_HTTP_HOST_ABI_VERSION UINT32_C(4)" not in header.read_bytes():
        fail(f"{target} public header does not declare host interface version 4")

    cmake_root = target_root / "lib/cmake/CoAkkaHttpHost"
    require_equal(
        sorted(path.name for path in cmake_root.glob("*.cmake")),
        list(CMAKE_FILES),
        f"{target} CMake metadata",
    )

    binary_relative = target_manifest.get("binary")
    if not isinstance(binary_relative, str):
        fail(f"{target} binary path is missing")
    expected_binary = {
        "macos-aarch64": "macos-aarch64/lib/libcoakka_http_host.1.0.0.dylib",
        "linux-aarch64": "linux-aarch64/lib/libcoakka_http_host.so.1.0.0",
        "linux-x86_64": "linux-x86_64/lib/libcoakka_http_host.so.1.0.0",
        "windows-aarch64": "windows-aarch64/bin/coakka_http_host.dll",
        "windows-x86_64": "windows-x86_64/bin/coakka_http_host.dll",
    }[target]
    require_equal(binary_relative, expected_binary, f"{target} primary binary path")
    binary = release / binary_relative
    binary_hash = digest(binary)
    require_equal(binary_hash, QUALIFIED_BINARY_SHA256[target], f"{target} qualified binary")
    require_equal(binary_hash, target_manifest.get("sha256"), f"{target} manifest binary hash")
    require_equal(binary.stat().st_size, target_manifest.get("size"), f"{target} binary size")
    require_equal(target_manifest.get("installed_entries"), len(expected_target_inventory(target)), f"{target} entry count")
    require_equal(target_manifest.get("export_count"), 102, f"{target} export count")
    for gate in ("package_gate", "matching_host_gate", "connector_gate"):
        require_equal(target_manifest.get(gate), "pass", f"{target} {gate}")

    for relative in ("include/coakka/http/host.h", *(
        f"lib/cmake/CoAkkaHttpHost/{name}" for name in CMAKE_FILES
    )):
        path = target_root / relative
        match = PACKAGE_FORBIDDEN.search(path.read_bytes())
        if match is not None:
            fail(f"{target} package metadata exposes private vocabulary: {relative}")
    match = PACKAGE_FORBIDDEN.search(binary.read_bytes())
    if match is not None:
        fail(f"{target} binary exposes private vocabulary")

    for name in LEGAL_FILES:
        release_legal = release / name
        target_legal = target_root / "share/licenses/coakka-http-runtime" / name
        require_equal(digest(target_legal), digest(release_legal), f"{target} legal file {name}")
    return header_hash


def main() -> None:
    repo_root = Path(__file__).resolve().parent.parent
    product = repo_root / "coakka-http-runtime"
    release = product / "native/releases" / RELEASE_ID
    if not product.is_dir() or not release.is_dir():
        fail(f"expected native candidate is missing: {release}")

    release_dirs = sorted(path.name for path in release.parent.iterdir() if path.is_dir())
    require_equal(release_dirs, [RELEASE_ID], "native release inventory")
    for language in ("go", "jvm", "python", "javascript"):
        if (product / language / "releases").exists():
            fail(f"registry-oriented {language} packages are outside this release step")

    verify_no_dependency_inventory(product)
    verify_public_docs(product)
    verify_safe_links(release)

    required_release_files = {
        "README.md",
        "RELEASE.md",
        "manifest.json",
        "SHA256SUMS",
        *LEGAL_FILES,
    }
    missing = sorted(name for name in required_release_files if not (release / name).is_file())
    if missing:
        fail(f"native release is missing: {', '.join(missing)}")

    manifest = read_json(release / "manifest.json")
    require_equal(manifest.get("schema"), "coakka.http.native-release.v1", "manifest schema")
    require_equal(manifest.get("product"), "coakka-http-runtime", "manifest product")
    require_equal(manifest.get("version"), "1.0.0", "manifest version")
    require_equal(manifest.get("status"), "ready-to-release", "manifest status")
    require_equal(manifest.get("source_revision"), SOURCE_REVISION, "source revision")
    require_equal(
        manifest.get("connector_revision"), CONNECTOR_REVISION, "connector revision"
    )
    require_equal(manifest.get("samples_revision"), SAMPLES_REVISION, "samples revision")
    require_equal(
        manifest.get("documentation_revision"),
        DOCUMENTATION_REVISION,
        "documentation revision",
    )
    require_equal(manifest.get("release_directory"), RELEASE_ID, "release directory")
    targets = manifest.get("targets")
    if not isinstance(targets, dict):
        fail("manifest targets must be an object")
    require_equal(tuple(targets), TARGETS, "manifest target order")
    require_equal(set(QUALIFIED_BINARY_SHA256), set(TARGETS), "qualified target inventory")

    benchmark = manifest.get("benchmark")
    if not isinstance(benchmark, dict):
        fail("benchmark evidence is missing")
    require_equal(benchmark.get("status"), "accepted", "benchmark status")
    benchmark_path = benchmark.get("document")
    if benchmark_path != "docs/benchmark-results-rpi5.md":
        fail("benchmark document path is invalid")
    if QUALIFIED_BENCHMARK_SHA256 is None:
        fail("physical-board benchmark has not been qualified")
    benchmark_document = product / benchmark_path
    benchmark_hash = digest(benchmark_document)
    require_equal(benchmark_hash, QUALIFIED_BENCHMARK_SHA256, "qualified benchmark document")
    require_equal(benchmark_hash, benchmark.get("sha256"), "manifest benchmark document")

    expected_header_hash: str | None = None
    for target in TARGETS:
        expected_header_hash = verify_target(
            release, target, targets[target], expected_header_hash
        )

    for name in LEGAL_FILES:
        require_equal(digest(product / name), digest(release / name), f"release legal file {name}")

    verify_checksums(release)
    print(
        "[http-runtime-release] pass "
        f"release={RELEASE_ID} targets={len(TARGETS)} exports=102 benchmark=accepted"
    )


if __name__ == "__main__":
    main()
