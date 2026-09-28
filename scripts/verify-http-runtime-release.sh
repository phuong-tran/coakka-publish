#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
product_root="${repo_root}/coakka-http-runtime"
native_id="1.0.0+204d6ed28231ff9a39f4584393bf7d5af0b6975a"
connector_id="${native_id}-7cbe256"
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
core_commit = "204d6ed28231ff9a39f4584393bf7d5af0b6975a"
connector_commit = "7cbe25630e73750f13e477999b6fca2e3ced1541"
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
    "macos-aarch64": "027bcabeb06bd7b2810b33a015853c205600466f998816860ebc08115ed4331e",
    "linux-aarch64": "22a7bfe646699c5770ca1b83407efa65df4222bbd758dd9bbe00a462108a35f2",
    "linux-x86_64": "0f5208841f331c62dc10cb55e776a161093ead8e01aaa7a3ef0c353bf3a77f79",
    "windows-aarch64": "9bc1f29ee84baa23e713258da2f922f9a7334bfd1f2a049901841078288b6990",
    "windows-x86_64": "94d9ef20d45eadba479dfecf50cdcb773b1aa764524c7d901299b0d9aabba0bc",
}
artifact_hashes = {
    "go": (
        "coakka-http-go-1.0.0.tar.gz",
        "5e965659bfd995d0a2dbe43d0a51cec15483217d880bfb8ece47bd5cb1742228",
    ),
    "jvm": (
        "coakka-http-jvm-1.0.0.jar",
        "1963e289662032b2c305f890e04af9480fc0a4b52fd71eea483d40ddddec7a59",
    ),
    "javascript": (
        "coakka-http-1.0.0.tgz",
        "a05e02ab79f28a91f93ef05329c07ba1535f6113f1ceccd83cc008bf6dc44345",
    ),
}
wheel_hashes = {
    "coakka_http-1.0.0-py3-none-macosx_11_0_arm64.whl": (
        "macos-aarch64",
        "42b6f13d628d40ceef5bbc906a702be3d3e833ab8500c773ccec8274405bb397",
    ),
    "coakka_http-1.0.0-py3-none-manylinux_2_28_aarch64.whl": (
        "linux-aarch64",
        "b3f54d6ca372e09d767ebfb823737caad3ce77ed8093d80af5c9cdf44836abe5",
    ),
    "coakka_http-1.0.0-py3-none-manylinux_2_28_x86_64.whl": (
        "linux-x86_64",
        "b2c491c8a9205ac1b00cfec94e73264d613606b7e8ca3b1dcfb7d700609337d3",
    ),
    "coakka_http-1.0.0-py3-none-win_arm64.whl": (
        "windows-aarch64",
        "5e52a3a1bbaf626e8f4da403c10b6efb0c5bcd30d50d0a8f4dfa5b9a0621dfc6",
    ),
    "coakka_http-1.0.0-py3-none-win_amd64.whl": (
        "windows-x86_64",
        "61af98ad2cad61f5f5baad008c5fbc43f4593709de0596e55134e0c2d36c6b4e",
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
            manifest.get("status"),
            "application-candidate",
            f"{lane} application publication status",
        )
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
        "c9072f66c466a56d44784cf3f3c1054f796ebf44bc431d06ece9fed7358a9bd2",
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

    metadata_root = release_dirs["native"] / target / "share/coakka-http-runtime"
    target_legal_root = (
        release_dirs["native"] / target / "share/licenses/coakka-http-runtime"
    )
    release_metadata = read_json(
        metadata_root / "coakka-http-runtime.release.json"
    )
    require_equal(
        release_metadata.get("schema"),
        "coakka.http.release-metadata.v1",
        f"{target} release metadata schema",
    )
    require_equal(release_metadata.get("product"), "coakka-http-runtime", f"{target} product")
    require_equal(release_metadata.get("version"), "1.0.0", f"{target} version")
    require_equal(
        release_metadata.get("source_revision"), core_commit, f"{target} source revision"
    )
    require_equal(release_metadata.get("target"), target, f"{target} metadata target")
    created = release_metadata.get("created")
    if not isinstance(created, str) or not re.fullmatch(
        r"\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z", created
    ):
        fail(f"{target} release metadata has an invalid creation time")

    legal = release_metadata.get("legal")
    if not isinstance(legal, list):
        fail(f"{target} release metadata has no legal inventory")
    actual_legal = {
        entry.get("file"): entry.get("sha256")
        for entry in legal
        if isinstance(entry, dict)
    }
    expected_legal = {
        name: digest_file(target_legal_root / name)
        for name in ("LICENSE", "NATIVE-LICENSE.md", "NOTICE", "PACKAGE-LICENSE.md")
    }
    require_equal(actual_legal, expected_legal, f"{target} installed legal metadata")

    components = release_metadata.get("components")
    if not isinstance(components, list):
        fail(f"{target} release metadata has no component inventory")
    expected_component_ids = {
        "abseil", "boost", "c-ares", "curl", "libuv", "nghttp2", "nghttp3",
        "ngtcp2", "openssl", "protobuf", "sfparse", "usockets", "utf8-range",
        "uwebsockets",
    }
    actual_component_ids = {
        component.get("id") for component in components if isinstance(component, dict)
    }
    require_equal(actual_component_ids, expected_component_ids, f"{target} component inventory")
    require_equal(len(components), 14, f"{target} component count")
    for component in components:
        if not isinstance(component, dict):
            fail(f"{target} component metadata contains a non-object")
        license_path = component.get("license_path")
        if not isinstance(license_path, str):
            fail(f"{target} component has no license path")
        installed_license = target_legal_root / license_path
        if not installed_license.is_file():
            fail(f"{target} component license is missing: {license_path}")
        require_equal(
            digest_file(installed_license),
            component.get("license_sha256"),
            f"{target} component license {license_path}",
        )

    artifact_metadata = read_json(
        metadata_root / "coakka-http-runtime.artifact.json"
    )
    require_equal(
        artifact_metadata.get("schema"),
        "coakka.http.artifact-metadata.v1",
        f"{target} artifact metadata schema",
    )
    require_equal(artifact_metadata.get("product"), "coakka-http-runtime", f"{target} artifact product")
    require_equal(
        artifact_metadata.get("source_revision"), core_commit, f"{target} artifact source"
    )
    require_equal(artifact_metadata.get("target"), target, f"{target} artifact target")
    require_equal(artifact_metadata.get("file"), path.name, f"{target} artifact filename")
    require_equal(artifact_metadata.get("size"), path.stat().st_size, f"{target} artifact size")
    require_equal(artifact_metadata.get("sha256"), core_hashes[target], f"{target} artifact hash")

    spdx = read_json(metadata_root / "coakka-http-runtime.spdx.json")
    packages = spdx.get("packages")
    relationships = spdx.get("relationships")
    expected_spdx_dependencies = 15 if target.startswith("linux-") else 14
    require_equal(spdx.get("name"), f"coakka-http-runtime-1.0.0-{target}", f"{target} SPDX name")
    require_equal(
        len(packages) if isinstance(packages, list) else -1,
        expected_spdx_dependencies + 1,
        f"{target} SPDX packages",
    )
    require_equal(
        len(relationships) if isinstance(relationships, list) else -1,
        expected_spdx_dependencies,
        f"{target} SPDX relationships",
    )

require_equal(
    digest_file(release_dirs["native"] / "windows-aarch64/lib/coakka_http_runtime.lib"),
    "cecba5aa55d437cc642de59005f7f19c996feacd4eff9878c09240041a9f6f07",
    "Windows ARM64 import library",
)
require_equal(
    digest_file(release_dirs["native"] / "windows-x86_64/lib/coakka_http_runtime.lib"),
    "7165a3dbe414f891b9815a83626ea397b8bfdb9cb216be02e1645e86a1507816",
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
require_equal(len(reference_third_party), 14, "third-party license count")
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
    for fixture in ("ca.pem", "server.pem", "server.key", "client.pem", "client.key"):
        fixture_path = f"coakka-http-go-1.0.0/test-fixtures/tls/{fixture}"
        if fixture_path not in names:
            fail(f"Go archive is missing its self-contained TLS fixture: {fixture}")
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
        "c3cfb30eddff96ad69d1ec4aaee164b5da4186669f0d9b712e2839c446f7606e",
    ),
    "linux-aarch64": (
        "native/linux-aarch64/libcoakka_http_jvm.so",
        "d1531dc116cdc2df372948f180b937fc4349498a70c60839ab049a44d2f1d175",
    ),
    "linux-x86_64": (
        "native/linux-x86_64/libcoakka_http_jvm.so",
        "5a130dc9512e583c1439a4fef984f262a8f2d0575311b2b6c3a9f69a88f1a38e",
    ),
    "windows-aarch64": (
        "native/windows-aarch64/coakka_http_jvm.dll",
        "1fbae8b2aeb6c935ffab530dd66f37ebb18221a88f602bdcf5ec243d8d312d15",
    ),
    "windows-x86_64": (
        "native/windows-x86_64/coakka_http_jvm.dll",
        "a043f38c38ae1c7a24816f2181f06ae4b737cabf4c07f8ac8bb4375c4a3aff2d",
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
        "604329d3c3819d3423c19bfe36c8f5024db4fc27a792be78a09dcdfd1a435e26",
    ),
    "linux-aarch64": (
        "linux-arm64",
        "libcoakka_http_runtime.so.1",
        "13d5d12ffd70f74b58beb6574dea1ea1563f04d501b358b664e5b6fe2a19bf86",
    ),
    "linux-x86_64": (
        "linux-x64",
        "libcoakka_http_runtime.so.1",
        "c517b2ee5f656ed7172f63e5b847e332ce31bcb2099ea4b1dc08cb288c3abc12",
    ),
    "windows-aarch64": (
        "win32-arm64",
        "coakka_http_runtime.dll",
        "fe11ad0c990a491cbb58c5988d8b016173461caed7424d55fbb20dd38051073d",
    ),
    "windows-x86_64": (
        "win32-x64",
        "coakka_http_runtime.dll",
        "9aa8aa9c6f170329aed7fe2a859a6fe80a2ef97e93e51d929f3a4a77b42f464f",
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
def find_llvm_tool(name: str) -> str | None:
    unversioned = shutil.which(name)
    if unversioned:
        return unversioned
    homebrew = Path("/opt/homebrew/opt/llvm/bin") / name
    if homebrew.is_file():
        return str(homebrew)
    for major in range(24, 10, -1):
        versioned = shutil.which(f"{name}-{major}")
        if versioned:
            return versioned
    return None


llvm_nm = find_llvm_tool("llvm-nm")
llvm_readobj = find_llvm_tool("llvm-readobj")
if llvm_nm is None or llvm_readobj is None:
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
    "bde4759dfc4b5c0399d608e300038c4a5f01e9b1edbf813390ee7ad8695d9a39",
    "ABI export allow-list",
)
expected_exports = {
    line.removeprefix("_")
    for line in allowlist_path.read_text(encoding="ascii").splitlines()
    if line
}
require_equal(len(expected_exports), 176, "ABI export allow-list count")

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

mach_output = run(
    llvm_readobj,
    "--needed-libs",
    str(core_files["macos-aarch64"]),
)
mach_block = mach_output.split("NeededLibraries [", 1)[1].split("]", 1)[0]
mach_dependencies = set(mach_block.split())
require_equal(
    mach_dependencies,
    {
        "@rpath/libcoakka_http_runtime.1.dylib",
        "/System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation",
        "/usr/lib/libSystem.B.dylib",
        "/usr/lib/libc++.1.dylib",
        "/usr/lib/libresolv.9.dylib",
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
        "advapi32.dll",
        "api-ms-win-core-synch-l1-2-0.dll",
        "crypt32.dll",
        "dbghelp.dll",
        "iphlpapi.dll",
        "kernel32.dll",
        "ntdll.dll",
        "user32.dll",
        "ws2_32.dll",
    },
    "windows-x86_64": {
        "advapi32.dll",
        "crypt32.dll",
        "dbghelp.dll",
        "iphlpapi.dll",
        "kernel32.dll",
        "ntdll.dll",
        "user32.dll",
        "ws2_32.dll",
    },
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
