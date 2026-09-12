#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
product_root="${repo_root}/coakka-http-runtime"
native_id="1.0.0+d8deb6b821cdd90b69fa7d8e1c85629aac164315"
connector_id="${native_id}-26a28a1"
native_release="${product_root}/native/releases/${native_id}"

fail() {
  echo "[http-runtime-release] $*" >&2
  exit 1
}

require_command() {
  command -v "$1" >/dev/null 2>&1 || fail "required command is unavailable: $1"
}

require_command cmake
require_command ctest
require_command file
require_command python3
require_command shasum

[[ -d "${product_root}" ]] || fail "product directory is missing"
[[ ! -e "${repo_root}/maven/coakka/http/coakka-http-jvm" ]] ||
  fail "withdrawn JVM coordinate is still present"

# Validate manifests, checksums, license closure, archive hygiene, and that every
# connector embeds the exact Core bytes promoted by the native lane.
python3 - "${product_root}" "${native_id}" "${connector_id}" <<'PY'
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil
import subprocess
import sys
import tarfile
import zipfile


product = Path(sys.argv[1]).resolve()
native_id = sys.argv[2]
connector_id = sys.argv[3]
core_commit = "d8deb6b821cdd90b69fa7d8e1c85629aac164315"
connector_commit = "26a28a1c4af3eb65334993532b4eaa082fc75438"
targets = (
    "macos-aarch64",
    "linux-aarch64",
    "linux-x86_64",
    "windows-aarch64",
    "windows-x86_64",
)

release_dirs = {
    "native": product / "native" / "releases" / native_id,
    "go": product / "go" / "releases" / connector_id,
    "jvm": product / "jvm" / "releases" / connector_id,
    "python": product / "python" / "releases" / connector_id,
    "javascript": product / "javascript" / "releases" / connector_id,
}

core_files = {
    "macos-aarch64": release_dirs["native"] / "macos-aarch64/lib/libcoakka_http_runtime.1.0.0.dylib",
    "linux-aarch64": release_dirs["native"] / "linux-aarch64/lib/libcoakka_http_runtime.so.1.0.0",
    "linux-x86_64": release_dirs["native"] / "linux-x86_64/lib/libcoakka_http_runtime.so.1.0.0",
    "windows-aarch64": release_dirs["native"] / "windows-aarch64/bin/coakka_http_runtime.dll",
    "windows-x86_64": release_dirs["native"] / "windows-x86_64/bin/coakka_http_runtime.dll",
}
core_hashes = {
    "macos-aarch64": "ee6e602692f71905b76f90205ad020409d667d429cd31bba29d62f8d4c60a009",
    "linux-aarch64": "ece8a618e763c10e23edea90bdd5fe968bf448fd5186b8c05d47f025f76b912b",
    "linux-x86_64": "eaa502ed5ccd0398ffdd463a6930f38def4796e8cd87b81c9cb8c3d847c959e6",
    "windows-aarch64": "1378347cb9ff27fee165e0711980ce2d8695f0c92a7ec61479788fedbc3ad043",
    "windows-x86_64": "1bf2220891e35893b8d47725345ba3bc4740d532eac288b022f2d0f52cdc6e69",
}
artifact_hashes = {
    "go": (
        "coakka-http-go-1.0.0.tar.gz",
        "974c28962ede1a7ecd7ddd77a06dcab9ea38989fcd4c9eb0a4c0163cf926fa0f",
    ),
    "jvm": (
        "coakka-http-jvm-1.0.0.jar",
        "9ffb0c79271e935081d237816a7703cf4deb8a558bd1ecf56615475e082992c7",
    ),
    "javascript": (
        "coakka-http-1.0.0.tgz",
        "650c26c3cb49ce1de626032e58dd8e308c956921e43c7b7f5d139de915329fd0",
    ),
}
wheel_hashes = {
    "coakka_http-1.0.0-py3-none-macosx_11_0_arm64.whl": (
        "macos-aarch64",
        "0bde567fee863e9b15037625e580c3e6cbdd39525e97242bc237504a99130695",
    ),
    "coakka_http-1.0.0-py3-none-manylinux_2_28_aarch64.whl": (
        "linux-aarch64",
        "8bc4d8e12e86b852884ab660f587bca925a8775cea9356e12d23912527b709f9",
    ),
    "coakka_http-1.0.0-py3-none-manylinux_2_28_x86_64.whl": (
        "linux-x86_64",
        "c952f131b118d86fe7b82c225db0c6c8a26c2fcf14acc538104e7440a523c682",
    ),
    "coakka_http-1.0.0-py3-none-win_arm64.whl": (
        "windows-aarch64",
        "25f7ecf8f17456192b74b3102cea52007263e8f280911f0e085a396bf2c86e12",
    ),
    "coakka_http-1.0.0-py3-none-win_amd64.whl": (
        "windows-x86_64",
        "7d79823fdaf8ae114375d3fb2c0ced0fb4140caa72f42a84fe726e086af2bc5f",
    ),
}


def fail(message: str) -> None:
    raise SystemExit(f"[http-runtime-release] {message}")


def digest_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def digest_file(path: Path) -> str:
    with path.open("rb") as source:
        return hashlib.file_digest(source, "sha256").hexdigest()


def require_equal(actual: object, expected: object, context: str) -> None:
    if actual != expected:
        fail(f"{context}: expected {expected!r}, found {actual!r}")


def read_json(path: Path) -> dict[str, object]:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        fail(f"cannot read JSON {path}: {error}")


def check_archive_names(names: list[str], context: str) -> None:
    if len(names) != len(set(names)):
        fail(f"{context} contains duplicate archive paths")
    for name in names:
        path = PurePosixPath(name)
        if path.is_absolute() or ".." in path.parts:
            fail(f"{context} has unsafe path: {name}")
        lowered = name.lower()
        if "/.git/" in f"/{lowered}/" or lowered.endswith((".proto", ".syso")):
            fail(f"{context} leaks forbidden build/source material: {name}")


for lane, release in release_dirs.items():
    parent = release.parent
    actual = sorted(path.name for path in parent.iterdir() if path.is_dir())
    require_equal(actual, [release.name], f"{lane} immutable release inventory")
    required = {
        "README.md",
        "CONSUMING.md",
        "RELEASE.md",
        "manifest.json",
        "LICENSE",
        "NATIVE-LICENSE.md",
        "PACKAGE-LICENSE.md",
        "NOTICE",
        "THIRD-PARTY-NOTICES.md",
        "SHA256SUMS",
        "third-party-licenses",
    }
    missing = sorted(name for name in required if not (release / name).exists())
    if missing:
        fail(f"{lane} release is missing: {', '.join(missing)}")

    manifest = read_json(release / "manifest.json")
    require_equal(manifest.get("schema_version"), 1, f"{lane} manifest schema")
    require_equal(manifest.get("product_lane"), "coakka-http-runtime", f"{lane} product")
    require_equal(manifest.get("language_lane"), lane, f"{lane} language lane")
    require_equal(manifest.get("version"), "1.0.0", f"{lane} version")
    require_equal(manifest.get("release_directory"), release.name, f"{lane} release directory")
    require_equal(manifest.get("core_source_git_commit"), core_commit, f"{lane} Core commit")
    if lane != "native":
        require_equal(
            manifest.get("connector_source_git_commit"),
            connector_commit,
            f"{lane} connector commit",
        )

    checksum_lines = (release / "SHA256SUMS").read_text(encoding="utf-8").splitlines()
    recorded: dict[str, str] = {}
    for line in checksum_lines:
        match = re.fullmatch(r"([0-9a-f]{64})  (\./.+)", line)
        if not match or match.group(2) in recorded:
            fail(f"{lane} has malformed or duplicate SHA256SUMS entry: {line!r}")
        recorded[match.group(2)] = match.group(1)
    inventory = {
        "./" + path.relative_to(release).as_posix()
        for path in release.rglob("*")
        if (path.is_file() or path.is_symlink()) and path.name != "SHA256SUMS"
    }
    require_equal(set(recorded), inventory, f"{lane} checksum inventory")
    for relative, expected in recorded.items():
        path = release / relative[2:]
        if path.is_symlink():
            target = os.readlink(path)
            if os.path.isabs(target) or not path.resolve().is_relative_to(release):
                fail(f"{lane} has unsafe symlink: {relative} -> {target}")
        require_equal(digest_file(path), expected, f"{lane} checksum {relative}")

for target, path in core_files.items():
    require_equal(digest_file(path), core_hashes[target], f"{target} Core SHA-256")
    header = release_dirs["native"] / target / "include/coakka/http/http.h"
    require_equal(
        digest_file(header),
        "ad95d1369b9851eb3a5299a9b7c108e3a3628b87347c3915233976fbda456a6b",
        f"{target} public header",
    )
    cmake_dir = release_dirs["native"] / target / "lib/cmake/CoAkkaHttp"
    require_equal(
        sorted(path.name for path in cmake_dir.glob("*.cmake")),
        [
            "CoAkkaHttpConfig.cmake",
            "CoAkkaHttpConfigVersion.cmake",
            "CoAkkaHttpTargets-release.cmake",
            "CoAkkaHttpTargets.cmake",
        ],
        f"{target} CMake metadata",
    )

require_equal(
    digest_file(release_dirs["native"] / "windows-aarch64/lib/coakka_http_runtime.lib"),
    "a9c60ac5a4de014d4c08789b28f5443b540bcdbaa58f2c2d77fa6fcfa5fa047d",
    "Windows ARM64 import library",
)
require_equal(
    digest_file(release_dirs["native"] / "windows-x86_64/lib/coakka_http_runtime.lib"),
    "900ead4802551e821eb7d6417803b13634bba58c669965709583e2ecf3d0e675",
    "Windows x86-64 import library",
)

for lane, (name, expected) in artifact_hashes.items():
    require_equal(digest_file(release_dirs[lane] / name), expected, f"{lane} primary artifact")
for name, (_, expected) in wheel_hashes.items():
    require_equal(digest_file(release_dirs["python"] / name), expected, f"Python wheel {name}")

# Exact legal copies make each independently downloaded lane redistributable
# without relying on files elsewhere in this repository.
license_names = (
    "LICENSE",
    "NATIVE-LICENSE.md",
    "PACKAGE-LICENSE.md",
    "NOTICE",
    "THIRD-PARTY-NOTICES.md",
)
reference_release = release_dirs["native"]
reference_third_party = {
    path.relative_to(reference_release).as_posix(): digest_file(path)
    for path in (reference_release / "third-party-licenses").rglob("*")
    if path.is_file()
}
require_equal(len(reference_third_party), 11, "third-party license count")
for lane, release in release_dirs.items():
    for name in license_names:
        require_equal(
            digest_file(release / name),
            digest_file(reference_release / name),
            f"{lane} {name}",
        )
    actual = {
        path.relative_to(release).as_posix(): digest_file(path)
        for path in (release / "third-party-licenses").rglob("*")
        if path.is_file()
    }
    require_equal(actual, reference_third_party, f"{lane} third-party license tree")

# Go archive: one source module, no stale linker blobs, and five exact Core images.
go_archive = release_dirs["go"] / artifact_hashes["go"][0]
go_core_paths = {
    "macos-aarch64": "coakka-http-go-1.0.0/native/darwin-arm64/libcoakka_http_runtime.1.0.0.dylib",
    "linux-aarch64": "coakka-http-go-1.0.0/native/linux-arm64/libcoakka_http_runtime.so.1.0.0",
    "linux-x86_64": "coakka-http-go-1.0.0/native/linux-amd64/libcoakka_http_runtime.so.1.0.0",
    "windows-aarch64": "coakka-http-go-1.0.0/native/windows-arm64/coakka_http_runtime.dll",
    "windows-x86_64": "coakka-http-go-1.0.0/native/windows-amd64/coakka_http_runtime.dll",
}
with tarfile.open(go_archive, "r:gz") as archive:
    names = archive.getnames()
    check_archive_names(names, "Go archive")
    if any(not (member.isfile() or member.isdir()) for member in archive.getmembers()):
        fail("Go archive contains a non-file/non-directory member")
    go_mod = archive.extractfile("coakka-http-go-1.0.0/go.mod")
    if go_mod is None or not go_mod.read().startswith(
        b"module github.com/phuong-tran/coakka-http-runtime-go\n"
    ):
        fail("Go archive has the wrong module identity")
    for target, name in go_core_paths.items():
        member = archive.extractfile(name)
        if member is None:
            fail(f"Go archive is missing {target} Core")
        require_equal(digest_bytes(member.read()), core_hashes[target], f"Go {target} Core")

# JVM archive: exact Core/JNI pairs and integrity manifests for every target.
jvm_archive = release_dirs["jvm"] / artifact_hashes["jvm"][0]
jvm_core_paths = {
    target: f"native/{target}/"
    + (
        "libcoakka_http_runtime.1.0.0.dylib"
        if target == "macos-aarch64"
        else "libcoakka_http_runtime.so.1.0.0"
        if target.startswith("linux-")
        else "coakka_http_runtime.dll"
    )
    for target in targets
}
jvm_bridge_paths = {
    "macos-aarch64": (
        "native/macos-aarch64/libcoakka_http_jvm.dylib",
        "82c8cebc1d92e43b3d0f83265e0f7b0d1294a6c3591fe5c45d7cab8b924a0cbf",
    ),
    "linux-aarch64": (
        "native/linux-aarch64/libcoakka_http_jvm.so",
        "d92ac75770c45ce719b8bfee44b52612ad866d64fd147218d48b380d4ddead79",
    ),
    "linux-x86_64": (
        "native/linux-x86_64/libcoakka_http_jvm.so",
        "d5b7699c284e44ee7d61452e72bd28425688a13b125139ca130a2bf036f8c984",
    ),
    "windows-aarch64": (
        "native/windows-aarch64/coakka_http_jvm.dll",
        "f955b1cdd54eca4a7f2585b47096a75451b3e6eb1a5f9baff5c5efa27f75e559",
    ),
    "windows-x86_64": (
        "native/windows-x86_64/coakka_http_jvm.dll",
        "3306ac7328a6c689772b3d1a8300d64761289b1ee565aab452634eeea902267b",
    ),
}
with zipfile.ZipFile(jvm_archive) as archive:
    names = archive.namelist()
    check_archive_names(names, "JVM JAR")
    for target in targets:
        require_equal(
            digest_bytes(archive.read(jvm_core_paths[target])),
            core_hashes[target],
            f"JVM {target} Core",
        )
        bridge_path, bridge_hash = jvm_bridge_paths[target]
        require_equal(
            digest_bytes(archive.read(bridge_path)), bridge_hash, f"JVM {target} bridge"
        )
        properties = archive.read(
            f"META-INF/coakka-http/native/{target}/native.properties"
        ).decode("ascii")
        if (
            f"core.sha256={core_hashes[target]}" not in properties
            or f"bridge.sha256={bridge_hash}" not in properties
        ):
            fail(f"JVM {target} integrity manifest does not match embedded bytes")

# Python wheels contain one and only one target Core plus an integrity manifest.
python_native_names: set[str] = set()
for wheel_name, (target, expected_hash) in wheel_hashes.items():
    wheel = release_dirs["python"] / wheel_name
    library = (
        "libcoakka_http_runtime.1.0.0.dylib"
        if target == "macos-aarch64"
        else "libcoakka_http_runtime.so.1.0.0"
        if target.startswith("linux-")
        else "coakka_http_runtime.dll"
    )
    resource = f"coakka_http/_native/{target}/{library}"
    with zipfile.ZipFile(wheel) as archive:
        names = archive.namelist()
        check_archive_names(names, f"Python wheel {wheel_name}")
        native_libraries = [
            name for name in names if name.endswith((".dylib", ".so.1.0.0", ".dll"))
        ]
        require_equal(native_libraries, [resource], f"Python {target} native inventory")
        require_equal(
            digest_bytes(archive.read(resource)), core_hashes[target], f"Python {target} Core"
        )
        manifest = json.loads(
            archive.read(f"coakka_http/_native/{target}/native.json")
        )
        require_equal(
            manifest["sha256"], core_hashes[target], f"Python {target} native manifest"
        )
        python_native_names.add(resource)
    require_equal(digest_file(wheel), expected_hash, f"Python {target} wheel")
require_equal(len(python_native_names), 5, "Python target inventory")

# JavaScript tarball is intentionally private until the npm publication gate.
js_archive = release_dirs["javascript"] / artifact_hashes["javascript"][0]
js_platforms = {
    "macos-aarch64": (
        "darwin-arm64",
        "libcoakka_http_runtime.1.dylib",
        "28e50b51ab0020b400f40a746beb76c48cc70bd7847d8f8a698ddd55e55693a9",
    ),
    "linux-aarch64": (
        "linux-arm64",
        "libcoakka_http_runtime.so.1",
        "72a338b737d48a9734e77416568342dadb30505df37050e300be29a9eeabf9be",
    ),
    "linux-x86_64": (
        "linux-x64",
        "libcoakka_http_runtime.so.1",
        "96fde3ac9f9ab13176145fc19bf752100af1083a7ef4b1be3ab4f6ceba6a8faa",
    ),
    "windows-aarch64": (
        "win32-arm64",
        "coakka_http_runtime.dll",
        "be46053d8e42d307dbfb19a5ed003a03737f2b89c1de3ee7db7005365eb53b80",
    ),
    "windows-x86_64": (
        "win32-x64",
        "coakka_http_runtime.dll",
        "47d5becc73581586a68be5a49a8c05a356530626cbbf695909cafb0b51eda9a0",
    ),
}
with tarfile.open(js_archive, "r:gz") as archive:
    names = archive.getnames()
    check_archive_names(names, "JavaScript tarball")
    if any(not (member.isfile() or member.isdir()) for member in archive.getmembers()):
        fail("JavaScript tarball contains a non-file/non-directory member")
    package_json_member = archive.extractfile("package/package.json")
    if package_json_member is None:
        fail("JavaScript tarball is missing package.json")
    package_json = json.load(package_json_member)
    require_equal(
        (package_json.get("name"), package_json.get("version"), package_json.get("private")),
        ("@coakka/http", "1.0.0", True),
        "JavaScript package identity",
    )
    for target, (platform, library, addon_hash) in js_platforms.items():
        prefix = f"package/prebuilds/{platform}/"
        core_member = archive.extractfile(prefix + library)
        addon_member = archive.extractfile(prefix + "coakka_http_javascript.node")
        manifest_member = archive.extractfile(prefix + "native.json")
        if core_member is None or addon_member is None or manifest_member is None:
            fail(f"JavaScript tarball is missing the {target} prebuild")
        require_equal(
            digest_bytes(core_member.read()), core_hashes[target], f"JavaScript {target} Core"
        )
        require_equal(
            digest_bytes(addon_member.read()), addon_hash, f"JavaScript {target} addon"
        )
        manifest = json.load(manifest_member)
        require_equal(
            manifest["core"]["sha256"],
            core_hashes[target],
            f"JavaScript {target} Core manifest",
        )
        require_equal(
            manifest["addon"]["sha256"],
            addon_hash,
            f"JavaScript {target} addon manifest",
        )

# Cross-format symbol, architecture and dependency inspection uses LLVM because
# the verifier runs all five object formats from one host.
llvm_nm = shutil.which("llvm-nm") or "/opt/homebrew/opt/llvm/bin/llvm-nm"
llvm_readobj = shutil.which("llvm-readobj") or "/opt/homebrew/opt/llvm/bin/llvm-readobj"
if not Path(llvm_nm).is_file() or not Path(llvm_readobj).is_file():
    fail("LLVM llvm-nm/llvm-readobj are required for five-format inspection")


def run(*command: str) -> str:
    try:
        return subprocess.run(
            command,
            check=True,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        ).stdout
    except subprocess.CalledProcessError as error:
        fail(f"command failed: {' '.join(command)}\n{error.stderr}")


allowlist_path = release_dirs["native"] / "ABI-EXPORTS.txt"
require_equal(
    digest_file(allowlist_path),
    "379e57f24719225890e99a74851d3884b7e169c3cb94e18581e19756259a66b2",
    "ABI export allow-list",
)
expected_exports = {
    line.removeprefix("_")
    for line in allowlist_path.read_text(encoding="ascii").splitlines()
    if line
}
require_equal(len(expected_exports), 125, "ABI export allow-list count")

mach_symbols = {
    line.split()[-1].removeprefix("_")
    for line in run(
        llvm_nm,
        "--defined-only",
        "--extern-only",
        str(core_files["macos-aarch64"]),
    ).splitlines()
}
require_equal(mach_symbols, expected_exports, "macOS export allow-list")
for target in ("linux-aarch64", "linux-x86_64"):
    symbols = {
        line.split()[-1].split("@", 1)[0]
        for line in run(
            llvm_nm,
            "-D",
            "--defined-only",
            "--extern-only",
            str(core_files[target]),
        ).splitlines()
        if line.split()[-1].startswith("coakka_http_")
    }
    require_equal(symbols, expected_exports, f"{target} export allow-list")
for target in ("windows-aarch64", "windows-x86_64"):
    output = run(llvm_readobj, "--coff-exports", str(core_files[target]))
    symbols = set(
        re.findall(r"^  Name: (coakka_http_[A-Za-z0-9_]+)$", output, re.MULTILINE)
    )
    require_equal(symbols, expected_exports, f"{target} export allow-list")

architectures = {
    "macos-aarch64": ("Mach-O 64-bit", "arm64"),
    "linux-aarch64": ("ELF 64-bit", "ARM aarch64"),
    "linux-x86_64": ("ELF 64-bit", "x86-64"),
    "windows-aarch64": ("PE32+", "Aarch64"),
    "windows-x86_64": ("PE32+", "x86-64"),
}
for target, markers in architectures.items():
    description = run("file", "-b", str(core_files[target]))
    if not all(marker in description for marker in markers):
        fail(f"{target} architecture mismatch: {description.strip()}")

mach_dependencies = {
    line.strip().split()[0]
    for line in run("otool", "-L", str(core_files["macos-aarch64"])).splitlines()[1:]
}
require_equal(
    mach_dependencies,
    {
        "@rpath/libcoakka_http_runtime.1.dylib",
        "/System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation",
        "/usr/lib/libSystem.B.dylib",
        "/usr/lib/libc++.1.dylib",
    },
    "macOS dependency closure",
)

linux_dependencies = {
    "linux-aarch64": {
        "ld-linux-aarch64.so.1",
        "libc.so.6",
        "libdl.so.2",
        "libm.so.6",
        "libpthread.so.0",
        "librt.so.1",
    },
    "linux-x86_64": {
        "ld-linux-x86-64.so.2",
        "libc.so.6",
        "libdl.so.2",
        "libm.so.6",
        "libpthread.so.0",
        "librt.so.1",
    },
}
for target, expected in linux_dependencies.items():
    output = run(llvm_readobj, "--needed-libs", str(core_files[target]))
    block = output.split("NeededLibraries [", 1)[1].split("]", 1)[0]
    require_equal(set(block.split()), expected, f"{target} dependency closure")
    versions = {
        (int(major), int(minor))
        for major, minor in re.findall(
            r"GLIBC_(\d+)\.(\d+)",
            run(llvm_readobj, "--version-info", str(core_files[target])),
        )
    }
    if not versions or max(versions) > (2, 28):
        fail(f"{target} exceeds the GLIBC 2.28 ceiling: {sorted(versions)}")

windows_dependencies = {
    "windows-aarch64": {
        "user32.dll",
        "ws2_32.dll",
        "dbghelp.dll",
        "kernel32.dll",
        "api-ms-win-core-synch-l1-2-0.dll",
    },
    "windows-x86_64": {"user32.dll", "ws2_32.dll", "dbghelp.dll", "kernel32.dll"},
}
for target, expected in windows_dependencies.items():
    output = run(llvm_readobj, "--coff-imports", str(core_files[target]))
    imports = {
        name.lower() for name in re.findall(r"^  Name: (.+)$", output, re.MULTILINE)
    }
    require_equal(imports, expected, f"{target} dependency closure")

print("[http-runtime-release] manifests, bytes, archives, ABI and dependency closure: pass")
PY

(
  cd "${product_root}/runtime-test"
  shasum -a 256 -c SOURCE-MANIFEST.sha256 >/dev/null
)

# Exercise the promoted installed tree through the public CMake target on the
# current matching host. Cross-target evidence is recorded in RELEASE.md files.
case "$(uname -s)-$(uname -m)" in
  Darwin-arm64) host_target="macos-aarch64" ;;
  Linux-aarch64 | Linux-arm64) host_target="linux-aarch64" ;;
  Linux-x86_64) host_target="linux-x86_64" ;;
  *) fail "unsupported verifier host: $(uname -s)-$(uname -m)" ;;
esac

build_dir="$(mktemp -d "${TMPDIR:-/tmp}/coakka-http-release.XXXXXX")"
trap 'rm -rf "${build_dir}"' EXIT
cmake -S "${product_root}/runtime-test" -B "${build_dir}" \
  -DCMAKE_PREFIX_PATH="${native_release}/${host_target}" \
  -DCMAKE_BUILD_TYPE=Release >/dev/null
cmake --build "${build_dir}" --parallel >/dev/null
ctest --test-dir "${build_dir}" --output-on-failure

git -C "${repo_root}" diff --check
git -C "${repo_root}" diff --cached --check
echo "[http-runtime-release] ok: CoAkka HTTP Runtime 1.0.0 is internally consistent"
