#!/usr/bin/env python3

"""Verify one immutable CoAkka native runtime-addon release directory."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
import tarfile
from pathlib import Path, PurePosixPath
from typing import Any


PLATFORMS = [
    "linux-aarch64",
    "linux-x86_64",
    "macos-aarch64",
    "windows-aarch64",
    "windows-x86_64",
]
PLATFORM_SUFFIX = {
    "linux-aarch64": ".so",
    "linux-x86_64": ".so",
    "macos-aarch64": ".dylib",
    "windows-aarch64": ".dll",
    "windows-x86_64": ".dll",
}
MAX_METADATA_BYTES = 1024 * 1024
MAX_ARCHIVE_BYTES = 512 * 1024 * 1024
MAX_ARCHIVE_MEMBERS = 512
MAX_ARCHIVE_EXPANDED_BYTES = 2 * 1024 * 1024 * 1024
REQUIRED_MANIFEST_FIELDS = {
    "schemaVersion",
    "name",
    "addonId",
    "version",
    "release",
    "sourceSnapshot",
    "archive",
    "sha256",
    "runtimeCompatibility",
    "ownedStaticDependencies",
    "userInstalledNativeDependencies",
    "cmakeConfig",
    "headers",
    "exportedSymbols",
    "platforms",
}
SFTP_EXPORTS = [
    "coakka_sftp_publisher_cancel",
    "coakka_sftp_publisher_create",
    "coakka_sftp_publisher_dependency_version",
    "coakka_sftp_publisher_destroy",
    "coakka_sftp_publisher_forget",
    "coakka_sftp_publisher_get",
    "coakka_sftp_publisher_get_target",
    "coakka_sftp_publisher_start",
    "coakka_sftp_publisher_stop",
    "coakka_sftp_publisher_submit",
    "coakka_sftp_publisher_wait",
]


class VerificationError(RuntimeError):
    pass


def fail(message: str) -> None:
    raise VerificationError(message)


def load_json(path: Path) -> dict[str, Any]:
    try:
        if path.stat().st_size > MAX_METADATA_BYTES:
            fail(f"JSON metadata is too large: {path}")
        value = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        fail(f"could not read JSON {path}: {error}")
    if not isinstance(value, dict):
        fail(f"JSON root must be an object: {path}")
    return value


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def require_string(value: Any, label: str, pattern: str | None = None) -> str:
    if not isinstance(value, str) or not value:
        fail(f"{label} must be a non-empty string")
    if pattern is not None and re.fullmatch(pattern, value) is None:
        fail(f"{label} has invalid format: {value}")
    return value


def require_exact_keys(value: dict[str, Any], expected: set[str], label: str) -> None:
    actual = set(value)
    if actual != expected:
        fail(
            f"{label} fields differ: missing={sorted(expected - actual)} "
            f"extra={sorted(actual - expected)}"
        )


def require_unique_strings(value: Any, label: str, pattern: str) -> list[str]:
    if not isinstance(value, list) or not value:
        fail(f"{label} must be a non-empty array")
    result = [require_string(item, f"{label} entry", pattern) for item in value]
    if len(result) != len(set(result)):
        fail(f"{label} contains duplicates")
    return result


def verify_checksums(release_dir: Path, required_names: set[str]) -> None:
    sums_path = release_dir / "SHA256SUMS"
    if not sums_path.is_file():
        fail("release is missing SHA256SUMS")
    if sums_path.stat().st_size > MAX_METADATA_BYTES:
        fail("SHA256SUMS is too large")
    seen: set[str] = set()
    for line_number, line in enumerate(
        sums_path.read_text(encoding="utf-8").splitlines(), start=1
    ):
        if not line:
            continue
        match = re.fullmatch(r"([0-9a-f]{64})  ([A-Za-z0-9._+-]+)", line)
        if match is None:
            fail(f"invalid SHA256SUMS row {line_number}")
        expected, name = match.groups()
        if name in seen:
            fail(f"duplicate SHA256SUMS entry: {name}")
        path = release_dir / name
        if not path.is_file() or path.parent != release_dir:
            fail(f"SHA256SUMS references missing or unsafe file: {name}")
        if sha256(path) != expected:
            fail(f"SHA256SUMS mismatch: {name}")
        seen.add(name)
    if not required_names.issubset(seen):
        fail(f"SHA256SUMS is missing entries: {sorted(required_names - seen)}")
    expected_files = required_names | {"SHA256SUMS"}
    release_entries = list(release_dir.iterdir())
    if any(path.is_symlink() or not path.is_file() for path in release_entries):
        fail("release directory may contain regular files only")
    actual_files = {path.name for path in release_entries}
    if actual_files != expected_files:
        fail(
            f"release directory files differ: missing={sorted(expected_files - actual_files)} "
            f"extra={sorted(actual_files - expected_files)}"
        )


def archive_members(archive: Path) -> tuple[str, dict[str, tarfile.TarInfo], tarfile.TarFile]:
    if archive.stat().st_size > MAX_ARCHIVE_BYTES:
        fail("addon archive exceeds the 512 MiB intake limit")
    try:
        bundle = tarfile.open(archive, mode="r:gz")
    except (OSError, tarfile.TarError) as error:
        fail(f"could not open addon archive: {error}")
    members: dict[str, tarfile.TarInfo] = {}
    roots: set[str] = set()
    archive_entries = bundle.getmembers()
    if len(archive_entries) > MAX_ARCHIVE_MEMBERS:
        bundle.close()
        fail("archive contains too many members")
    if sum(member.size for member in archive_entries) > MAX_ARCHIVE_EXPANDED_BYTES:
        bundle.close()
        fail("archive expanded size exceeds the 2 GiB intake limit")
    for member in archive_entries:
        path = PurePosixPath(member.name)
        if path.is_absolute() or ".." in path.parts or not path.parts:
            bundle.close()
            fail(f"archive contains unsafe path: {member.name}")
        if member.issym() or member.islnk() or not (member.isfile() or member.isdir()):
            bundle.close()
            fail(f"archive contains unsupported member type: {member.name}")
        roots.add(path.parts[0])
        normalized_name = member.name.rstrip("/")
        if normalized_name in members:
            bundle.close()
            fail(f"archive contains duplicate member: {member.name}")
        members[normalized_name] = member
    if len(roots) != 1:
        bundle.close()
        fail("archive must contain exactly one package root")
    return next(iter(roots)), members, bundle


def read_archive_json(
    bundle: tarfile.TarFile, members: dict[str, tarfile.TarInfo], name: str
) -> dict[str, Any]:
    member = members.get(name)
    if member is None or not member.isfile():
        fail(f"archive is missing JSON file: {name}")
    if member.size > MAX_METADATA_BYTES:
        fail(f"archive JSON metadata is too large: {name}")
    extracted = bundle.extractfile(member)
    if extracted is None:
        fail(f"could not read archive member: {name}")
    try:
        value = json.loads(extracted.read().decode("utf-8"))
    except (UnicodeDecodeError, json.JSONDecodeError) as error:
        fail(f"invalid archive JSON {name}: {error}")
    if not isinstance(value, dict):
        fail(f"archive JSON root must be an object: {name}")
    return value


def verify_archive(
    archive: Path,
    slug: str,
    version: str,
    addon_id: str,
    cmake_config: str,
    headers: list[str],
    platforms: list[dict[str, Any]],
) -> None:
    root, members, bundle = archive_members(archive)
    expected_root = f"coakka-runtime-addon-{slug}-native-{version}"
    try:
        if root != expected_root:
            fail(f"archive package root must be {expected_root}, found {root}")
        required = {
            f"{root}/README.md",
            f"{root}/CONSUMING.md",
            f"{root}/LICENSE.md",
            f"{root}/THIRD_PARTY_NOTICES.md",
            f"{root}/{cmake_config}",
            f"{root}/share/coakka/runtime-addons/{slug}/addon.manifest.json",
        }
        required.update(f"{root}/{header}" for header in headers)
        required.update(f"{root}/{platform['library']}" for platform in platforms)
        missing = sorted(name for name in required if name not in members)
        if missing:
            fail(f"archive is missing required members: {missing}")

        embedded_forbidden = sorted(
            name
            for name in members
            if re.fullmatch(
                r"(?:libcoakka_runtime_v2|libssh2|libcrypto|libssl)"
                r"(?:[.]so(?:[.][0-9]+)*|[.]dylib|[.]dll)",
                PurePosixPath(name).name,
            )
        )
        if embedded_forbidden:
            fail(f"archive embeds forbidden sidecar libraries: {embedded_forbidden}")

        addon_manifest_name = (
            f"{root}/share/coakka/runtime-addons/{slug}/addon.manifest.json"
        )
        addon_manifest = read_archive_json(bundle, members, addon_manifest_name)
        if addon_manifest.get("manifest_version") != 1:
            fail("installed addon manifest_version must be 1")
        if addon_manifest.get("addon_id") != addon_id:
            fail("installed addon manifest addon_id does not match release manifest")
        if addon_manifest.get("addon_version") != version:
            fail("installed addon manifest addon_version does not match release version")
        if addon_manifest.get("runtime_family") != "coakka-runtime-v2":
            fail("installed addon manifest runtime_family must be coakka-runtime-v2")
        if slug == "artifact-publisher-sftp":
            if addon_manifest.get("addon_kind") != "bridge":
                fail("installed SFTP addon kind must be bridge")
            if addon_manifest.get("required_runtime_features") != ["file_lane"]:
                fail("installed SFTP addon manifest must require file_lane")
            if (
                addon_manifest.get("entrypoint_symbol")
                != "coakka_sftp_publisher_create"
            ):
                fail("installed SFTP addon entrypoint differs from the C ABI")
    finally:
        bundle.close()


def verify_release(release_dir: Path, expected_addon: str | None) -> None:
    release_dir = release_dir.resolve()
    if not release_dir.is_dir():
        fail(f"release directory does not exist: {release_dir}")
    if release_dir.parent.name != "releases" or release_dir.parent.parent.name != "native":
        fail("release path must end with native/releases/<release>")
    slug = release_dir.parent.parent.parent.name
    if expected_addon is not None and slug != expected_addon:
        fail(f"release addon mismatch: expected={expected_addon} actual={slug}")

    manifest_path = release_dir / "manifest.json"
    readme_path = release_dir / "README.md"
    if not readme_path.is_file():
        fail("release is missing README.md")
    manifest = load_json(manifest_path)
    require_exact_keys(manifest, REQUIRED_MANIFEST_FIELDS, "manifest")
    if manifest["schemaVersion"] != 1:
        fail("schemaVersion must be 1")

    version = require_string(manifest["version"], "version", r"[0-9]+\.[0-9]+\.[0-9]+")
    snapshot = require_string(manifest["sourceSnapshot"], "sourceSnapshot", r"[0-9a-f]{7,40}")
    release = require_string(
        manifest["release"], "release", r"[0-9]+\.[0-9]+\.[0-9]+\+[0-9a-f]{7,40}"
    )
    if release != f"{version}+{snapshot}" or release_dir.name != release:
        fail("release directory, version, and sourceSnapshot are inconsistent")

    name = require_string(manifest["name"], "name", r"coakka-runtime-addon-[a-z0-9-]+-native")
    if name != f"coakka-runtime-addon-{slug}-native":
        fail("manifest name does not match addon directory")
    addon_id = require_string(manifest["addonId"], "addonId", r"coakka\.[a-z0-9.]+")
    archive_name = require_string(
        manifest["archive"], "archive", r"[A-Za-z0-9._+-]+\.tar\.gz"
    )
    expected_archive = f"coakka-runtime-addon-{slug}-native-{version}.tar.gz"
    if archive_name != expected_archive:
        fail(f"archive name must be {expected_archive}")
    archive = release_dir / archive_name
    if not archive.is_file():
        fail(f"release archive is missing: {archive_name}")
    archive_sha = require_string(manifest["sha256"], "sha256", r"[0-9a-f]{64}")
    if sha256(archive) != archive_sha:
        fail("release manifest archive sha256 mismatch")

    compatibility = manifest["runtimeCompatibility"]
    if not isinstance(compatibility, dict):
        fail("runtimeCompatibility must be an object")
    require_exact_keys(
        compatibility,
        {"abiMajor", "minimumNativeVersion", "requiredFeatures"},
        "runtimeCompatibility",
    )
    if compatibility["abiMajor"] != 2:
        fail("runtimeCompatibility.abiMajor must be 2")
    minimum_native_version = require_string(
        compatibility["minimumNativeVersion"],
        "runtimeCompatibility.minimumNativeVersion",
        r"[0-9]+\.[0-9]+\.[0-9]+",
    )
    features = require_unique_strings(
        compatibility["requiredFeatures"],
        "runtimeCompatibility.requiredFeatures",
        r"[a-z][a-z0-9_]*",
    )

    dependencies = manifest["ownedStaticDependencies"]
    if not isinstance(dependencies, list) or not dependencies:
        fail("ownedStaticDependencies must be a non-empty array")
    dependency_names: list[str] = []
    dependency_versions: dict[str, str] = {}
    for index, dependency in enumerate(dependencies):
        if not isinstance(dependency, dict):
            fail(f"ownedStaticDependencies[{index}] must be an object")
        require_exact_keys(dependency, {"name", "version"}, "static dependency")
        dependency_name = require_string(dependency["name"], "dependency name")
        dependency_names.append(dependency_name)
        dependency_version = require_string(
            dependency["version"], "dependency version"
        )
        if dependency_version in {"pending", "target-package-defined", "unknown"}:
            fail("promoted static dependency versions must be exact")
        dependency_versions[dependency_name] = dependency_version
    if len(dependency_names) != len(set(dependency_names)):
        fail("ownedStaticDependencies contains duplicate names")
    if manifest["userInstalledNativeDependencies"] is not False:
        fail("userInstalledNativeDependencies must be false")
    cmake_config = require_string(
        manifest["cmakeConfig"],
        "cmakeConfig",
        r"cmake/CoAkkaRuntimeAddon[A-Za-z0-9]+Config[.]cmake",
    )

    headers = require_unique_strings(
        manifest["headers"], "headers", r"include/coakka/addons/[a-z0-9_]+\.h"
    )
    exports = require_unique_strings(
        manifest["exportedSymbols"], "exportedSymbols", r"coakka_[a-z0-9_]+"
    )

    platforms_value = manifest["platforms"]
    if not isinstance(platforms_value, list) or not platforms_value:
        fail("platforms must be a non-empty array")
    platforms: list[dict[str, Any]] = []
    platform_ids: list[str] = []
    for index, platform in enumerate(platforms_value):
        if not isinstance(platform, dict):
            fail(f"platforms[{index}] must be an object")
        require_exact_keys(
            platform,
            {"id", "library", "matchingHostRuntimeTest", "dynamicDependencyAudit"},
            "platform",
        )
        platform_id = require_string(platform["id"], "platform id")
        if platform_id not in PLATFORMS:
            fail(f"unsupported platform id: {platform_id}")
        suffix = PLATFORM_SUFFIX[platform_id]
        expected_library = (
            f"native/{platform_id}/libcoakka_addon_"
            f"{slug.replace('-', '_')}{suffix}"
        )
        if platform["library"] != expected_library:
            fail(f"platform library must be {expected_library}")
        if platform["matchingHostRuntimeTest"] != "passed":
            fail(f"matching-host runtime test has not passed: {platform_id}")
        if platform["dynamicDependencyAudit"] != "passed":
            fail(f"dynamic dependency audit has not passed: {platform_id}")
        platform_ids.append(platform_id)
        platforms.append(platform)
    expected_order = [item for item in PLATFORMS if item in platform_ids]
    if platform_ids != expected_order or len(platform_ids) != len(set(platform_ids)):
        fail("platforms must be unique and in canonical order")

    if slug == "artifact-publisher-sftp":
        if addon_id != "coakka.artifact.publisher.sftp":
            fail("SFTP addonId must be coakka.artifact.publisher.sftp")
        if minimum_native_version != "2.3.0":
            fail("SFTP addon minimum native runtime must be 2.3.0")
        if any(platform_id.startswith("windows-") for platform_id in platform_ids):
            fail("SFTP Windows release remains blocked by the staging safety gate")
        if features != ["file_lane"]:
            fail("SFTP addon must require exactly file_lane")
        if "libssh2" not in dependency_names:
            fail("SFTP addon must record libssh2 as an owned static dependency")
        if dependency_versions["libssh2"] != "1.11.1":
            fail("SFTP addon release requires the reviewed libssh2 1.11.1")
        if (
            cmake_config
            != "cmake/CoAkkaRuntimeAddonArtifactPublisherSftpConfig.cmake"
        ):
            fail("SFTP addon CMake package path differs from the public contract")
        if headers != ["include/coakka/addons/artifact_publisher_sftp.h"]:
            fail("SFTP addon header list differs from the public C ABI")
        if exports != SFTP_EXPORTS:
            fail("SFTP addon export list differs from the reviewed C ABI")

    verify_archive(
        archive, slug, version, addon_id, cmake_config, headers, platforms
    )
    verify_checksums(
        release_dir, {archive_name, "manifest.json", "README.md"}
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--release-dir", required=True, type=Path)
    parser.add_argument("--expected-addon")
    args = parser.parse_args()
    try:
        verify_release(args.release_dir, args.expected_addon)
    except VerificationError as error:
        print(f"[runtime-addon-intake] {error}", file=sys.stderr)
        return 1
    print(f"[runtime-addon-intake] ok: {args.release_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
