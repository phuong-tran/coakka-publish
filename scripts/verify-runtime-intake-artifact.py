#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
import tarfile
from pathlib import Path, PurePosixPath
from zipfile import ZipFile


SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_SCANNER = SCRIPT_DIR / "scan-public-surface.sh"
FORBIDDEN_COMPONENTS = frozenset(
    {
        ".gradle",
        ".idea",
        "__pycache__",
        "build",
        "demo",
        "demos",
        "node_modules",
        "sample",
        "samples",
        "test",
        "tests",
    }
)
FORBIDDEN_NATIVE_NAME_FRAGMENTS = ("libcoakka_v2_transport_proto",)
RUNTIME_NATIVE_PREFIX = "libcoakka_runtime_v2"


def fail(message: str) -> None:
    raise SystemExit(f"[runtime-intake] {message}")


def normalized(entry: str) -> str:
    return entry.replace("\\", "/").lstrip("/")


def path_components(entry: str) -> list[str]:
    return [part for part in normalized(entry).split("/") if part and part != "."]


def report_bad_entries(reason: str, entries: list[str]) -> None:
    shown = "\n".join(f"- {entry}" for entry in sorted(entries)[:60])
    extra = "" if len(entries) <= 60 else f"\n... {len(entries) - 60} more"
    fail(f"{reason}\n{shown}{extra}")


def require_no_forbidden_components(entries: list[str]) -> None:
    bad = [
        entry
        for entry in entries
        if any(component in FORBIDDEN_COMPONENTS for component in path_components(entry))
    ]
    if bad:
        report_bad_entries("workspace/demo/test path leaked into package", bad)


def is_native_runtime_entry(entry: str) -> bool:
    name = PurePosixPath(normalized(entry)).name
    return name in {"libcoakka_runtime_v2.so", "libcoakka_runtime_v2.dylib"} or (
        name.startswith(f"{RUNTIME_NATIVE_PREFIX}-")
        and (name.endswith(".so") or name.endswith(".dylib"))
    )


def require_native_entries_clean(entries: list[str], expected_native_version: str, require_native: bool) -> None:
    allowed_names = {
        f"{RUNTIME_NATIVE_PREFIX}.dylib",
        f"{RUNTIME_NATIVE_PREFIX}.so",
        f"{RUNTIME_NATIVE_PREFIX}-{expected_native_version}.dylib",
        f"{RUNTIME_NATIVE_PREFIX}-{expected_native_version}.so",
    }
    bad: list[str] = []
    native_count = 0
    for entry in entries:
        name = PurePosixPath(normalized(entry)).name
        if any(fragment in name for fragment in FORBIDDEN_NATIVE_NAME_FRAGMENTS):
            bad.append(entry)
            continue
        if is_native_runtime_entry(entry):
            native_count += 1
            if name not in allowed_names:
                bad.append(entry)
    if bad:
        report_bad_entries("stale or forbidden native library entry", bad)
    if require_native and native_count == 0:
        fail("package does not include any runtime native library entries")


def parse_assignment(text: str, pattern: str, label: str) -> str:
    match = re.search(pattern, text, re.MULTILINE)
    if match is None:
        fail(f"could not read {label}")
    return match.group(1)


def read_tar_member_text(archive: tarfile.TarFile, suffix: str) -> str:
    matches = [member for member in archive.getmembers() if member.name.endswith(suffix) and member.isfile()]
    if len(matches) != 1:
        fail(f"expected exactly one tar member ending with {suffix!r}, found {len(matches)}")
    extracted = archive.extractfile(matches[0])
    if extracted is None:
        fail(f"could not read {matches[0].name}")
    return extracted.read().decode("utf-8")


def jvm_native_version(artifact: Path) -> tuple[str, list[str]]:
    with ZipFile(artifact) as archive:
        entries = archive.namelist()
        try:
            manifest_text = archive.read("META-INF/MANIFEST.MF").decode("utf-8")
        except KeyError as exc:
            fail(f"{artifact.name} is missing META-INF/MANIFEST.MF")
            raise AssertionError from exc
    return (
        parse_assignment(manifest_text, r"^Coakka-V2-Native-Package-Version:\s*(\S+)\s*$", "JVM native package version"),
        entries,
    )


def python_native_version(artifact: Path) -> tuple[str, list[str]]:
    with ZipFile(artifact) as archive:
        entries = archive.namelist()
        try:
            packaging_text = archive.read("coakka_v2_connector/_packaging.py").decode("utf-8")
        except KeyError as exc:
            fail(f"{artifact.name} is missing coakka_v2_connector/_packaging.py")
            raise AssertionError from exc
    return (
        parse_assignment(packaging_text, r"^COAKKA_V2_NATIVE_PACKAGE_VERSION\s*=\s*['\"]([^'\"]+)['\"]", "Python native package version"),
        entries,
    )


def node_native_version(artifact: Path) -> tuple[str, list[str]]:
    with tarfile.open(artifact, "r:gz") as archive:
        entries = [member.name for member in archive.getmembers()]
        packaging_text = read_tar_member_text(archive, "/dist/packaging.js")
    return (
        parse_assignment(packaging_text, r"export const COAKKA_V2_NATIVE_PACKAGE_VERSION = ['\"]([^'\"]+)['\"];", "Node native package version"),
        entries,
    )


def go_native_version(artifact: Path) -> tuple[str, list[str]]:
    with tarfile.open(artifact, "r:gz") as archive:
        entries = [member.name for member in archive.getmembers()]
        packaging_text = read_tar_member_text(archive, "/packaging.go")
    return (
        parse_assignment(packaging_text, r"\bCoakkaV2NativePackageVersion\s*=\s*\"([^\"]+)\"", "Go native package version"),
        entries,
    )


def csharp_native_version(artifact: Path) -> tuple[str, list[str]]:
    with ZipFile(artifact) as archive:
        entries = archive.namelist()
        try:
            metadata = json.loads(archive.read("coakka/metadata/runtime-package.json").decode("utf-8"))
        except KeyError as exc:
            fail(f"{artifact.name} is missing coakka/metadata/runtime-package.json")
            raise AssertionError from exc
    return str(metadata.get("bundled_native_package_version", "")), entries


def rust_native_version(artifact: Path) -> tuple[str, list[str]]:
    with tarfile.open(artifact, "r:gz") as archive:
        entries = [member.name for member in archive.getmembers()]
        metadata = json.loads(read_tar_member_text(archive, "/coakka-runtime-package.json"))
    return str(metadata.get("bundled_native_package_version", "")), entries


def source_connector_native_version(artifact: Path, metadata_name: str) -> tuple[str, list[str]]:
    with tarfile.open(artifact, "r:gz") as archive:
        entries = [member.name for member in archive.getmembers()]
        metadata = json.loads(read_tar_member_text(archive, f"/{metadata_name}"))
    return str(metadata.get("bundled_native_package_version", "")), entries


def mojo_native_version(artifact: Path) -> tuple[str, list[str]]:
    return source_connector_native_version(artifact, "coakka-runtime-mojo-package.json")


def zig_native_version(artifact: Path) -> tuple[str, list[str]]:
    return source_connector_native_version(artifact, "coakka-runtime-zig-package.json")


LANE_READERS = {
    "jvm": jvm_native_version,
    "python": python_native_version,
    "node": node_native_version,
    "go": go_native_version,
    "csharp": csharp_native_version,
    "mojo": mojo_native_version,
    "rust": rust_native_version,
    "zig": zig_native_version,
}


def run_scanner(scanner: Path, artifact: Path) -> None:
    if not scanner.is_file():
        fail(f"scanner does not exist: {scanner}")
    result = subprocess.run([str(scanner), str(artifact)], check=False)
    if result.returncode != 0:
        fail(f"public surface scanner rejected {artifact}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Validate a runtime language artifact before public intake.")
    parser.add_argument("--lane", choices=sorted(LANE_READERS), required=True)
    parser.add_argument("--artifact", required=True, help="Path to the package artifact to validate.")
    parser.add_argument("--expected-native-version", required=True)
    parser.add_argument("--scanner", default=str(DEFAULT_SCANNER))
    parser.add_argument(
        "--allow-no-native",
        action="store_true",
        help="Allow auxiliary artifacts such as JVM sources jars to omit runtime native library entries.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    artifact = Path(args.artifact).resolve()
    scanner = Path(args.scanner).resolve()
    if not artifact.is_file():
        fail(f"artifact does not exist: {artifact}")

    native_version, entries = LANE_READERS[args.lane](artifact)
    if native_version != args.expected_native_version:
        fail(
            f"{artifact.name} bundles native package version {native_version!r}; "
            f"expected {args.expected_native_version!r}"
        )
    require_no_forbidden_components(entries)
    require_native_entries_clean(entries, args.expected_native_version, require_native=not args.allow_no_native)
    run_scanner(scanner, artifact)
    print(f"[runtime-intake] ok lane={args.lane} artifact={artifact.name}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
