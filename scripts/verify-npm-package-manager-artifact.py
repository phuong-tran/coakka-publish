#!/usr/bin/env python3

import argparse
import json
import posixpath
import re
import sys
import tarfile
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]

PACKAGE_PLATFORMS = (
    "linux-aarch64",
    "linux-x86_64",
    "macos-aarch64",
    "windows-aarch64",
    "windows-x86_64",
)

CURRENT_CANDIDATES = (
    (
        "runtime-node",
        "runtime",
        "node",
        "coakka-v2-connector-node",
        "2.3.0+a83ab412",
        "package-manager/npm/candidates/3a84c7b/runtime/node/coakka-v2-connector-node-2.3.0.tgz",
        None,
    ),
    (
        "runtime-bun",
        "runtime",
        "bun",
        "coakka-v2-connector-bun",
        "2.3.0+a83ab412",
        "package-manager/npm/candidates/3a84c7b/runtime/bun/coakka-v2-connector-bun-2.3.0.tgz",
        None,
    ),
    (
        "runtime-electron",
        "runtime",
        "electron",
        "coakka-v2-connector-electron",
        "2.3.0+a83ab412",
        "package-manager/npm/candidates/3a84c7b/runtime/electron/coakka-v2-connector-electron-2.3.0.tgz",
        "coakka-v2-connector-node",
    ),
    (
        "logger-node",
        "logger",
        "node",
        "coakka-logger-node",
        "1.2.1+f50756ebff0d",
        "package-manager/npm/candidates/b052a3f/logger/node/coakka-logger-node-1.2.6.tgz",
        None,
    ),
    (
        "logger-bun",
        "logger",
        "bun",
        "coakka-logger-bun",
        "1.2.1+f50756ebff0d",
        "package-manager/npm/candidates/b052a3f/logger/bun/coakka-logger-bun-1.2.6.tgz",
        None,
    ),
    (
        "logger-electron",
        "logger",
        "electron",
        "coakka-logger-electron",
        "1.2.1+f50756ebff0d",
        "package-manager/npm/candidates/b052a3f/logger/electron/coakka-logger-electron-1.2.6.tgz",
        "coakka-logger-node",
    ),
)

FORBIDDEN_DEPENDENCY_PATTERNS = tuple(
    re.compile(pattern, re.IGNORECASE)
    for pattern in (
        r"(^|[-_@/])protobuf",
        r"(^|[-_@/])libuv($|[-_@/])",
        r"(^|[-_@/])uv($|[-_@/])",
        r"(^|[-_@/])caf($|[-_@/])",
        r"(^|[-_@/])koffi($|[-_@/])",
        r"(^|[-_@/])ffi($|[-_@/])",
        r"(^|[-_@/])node-gyp($|[-_@/])",
        r"(^|[-_@/])cmake-js($|[-_@/])",
        r"(^|[-_@/])prebuild-install($|[-_@/])",
    )
)

FORBIDDEN_MEMBER_COMPONENTS = {
    ".agents",
    ".git",
    ".github",
    "__tests__",
    "node_modules",
    "samples",
    "scripts",
    "src",
    "test",
    "tests",
}

FORBIDDEN_MEMBER_SUBSTRINGS = (
    "workingDir",
    "source-checkout",
    "source-owner",
    "build-root",
)

FORBIDDEN_METADATA_SUBSTRINGS = (
    "coakkaJVMConnector",
    "github.com/phuong-tran/coakkaJVMConnector",
)

TEXT_MEMBER_SUFFIXES = (
    ".cjs",
    ".d.ts",
    ".js",
    ".json",
    ".md",
    ".mjs",
    ".ts",
    ".txt",
)

FORBIDDEN_TEXT_PATTERNS = tuple(
    re.compile(pattern, re.IGNORECASE)
    for pattern in (
        r"protobufjs",
        r"wire-codec",
        r"proto/",
        r"coakkaJVMConnector",
        r"workingDir",
    )
)

INTERNAL_TRANSPORT_CODEC_PATTERNS = tuple(
    re.compile(pattern, re.IGNORECASE)
    for pattern in (
        r"\bWIRE_VARINT\b",
        r"\bencodeVarint\b",
    )
)

PUBLIC_TRANSPORT_HELPER_PATTERN = re.compile(
    r"\b(?:decodeEnvelope|decodeDeadletter|encodeEnvelope|encodeControlEnvelope)\b",
    re.IGNORECASE,
)

FORBIDDEN_PUBLIC_CODEC_MEMBERS = (
    "protobuf.js",
    "protobuf.d.ts",
    "wire-codec.js",
    "wire-codec.d.ts",
)

PUBLIC_REPOSITORY_URL = "git+https://github.com/phuong-tran/coakka-publish.git"
PUBLIC_BUGS_URL = "https://github.com/phuong-tran/coakka-samples/issues"

INSTALL_LIFECYCLE_SCRIPTS = (
    "preinstall",
    "install",
    "postinstall",
)

LICENSE_VALUE = "SEE LICENSE IN LICENSE.md"


class VerificationError(Exception):
    pass


def fail(message: str) -> None:
    raise VerificationError(message)


def npm_spec_is_registry_spec(spec: str) -> bool:
    if re.fullmatch(r"\d+\.\d+\.\d+(?:[-+][0-9A-Za-z.-]+)?", spec):
        return True
    if re.fullmatch(r"[~^]?\d+\.\d+\.\d+(?:[-+][0-9A-Za-z.-]+)?", spec):
        return True
    return False


def read_package_json(archive: tarfile.TarFile) -> dict:
    try:
        member = archive.extractfile("package/package.json")
    except KeyError:
        fail("archive is missing package/package.json")
    if member is None:
        fail("archive package/package.json is not a regular file")
    try:
        return json.loads(member.read().decode("utf-8"))
    except json.JSONDecodeError as exc:
        fail(f"package/package.json is not valid JSON: {exc}")


def member_names(archive: tarfile.TarFile) -> list[str]:
    return [member.name for member in archive.getmembers()]


def is_text_member(name: str) -> bool:
    normalized = posixpath.normpath(name).lower()
    return normalized.endswith(TEXT_MEMBER_SUFFIXES)


def verify_member_boundary(names: list[str]) -> None:
    for name in names:
        normalized = posixpath.normpath(name)
        if normalized.startswith("../") or normalized == ".." or normalized.startswith("/"):
            fail(f"unsafe archive member path: {name}")
        if normalized != "package" and not normalized.startswith("package/"):
            fail(f"npm artifact member must live under package/: {name}")
        components = set(normalized.split("/"))
        blocked = sorted(components & FORBIDDEN_MEMBER_COMPONENTS)
        if blocked:
            fail(f"workspace/test/build path leaked into npm artifact: {name}")
        if normalized.endswith(".proto"):
            fail(f"protobuf schema leaked into npm package-manager artifact: {name}")
        if posixpath.basename(normalized).lower() in FORBIDDEN_PUBLIC_CODEC_MEMBERS:
            fail(f"public transport codec member leaked into npm artifact: {name}")
        for marker in FORBIDDEN_MEMBER_SUBSTRINGS:
            if marker in normalized:
                fail(f"unpublished source identity leaked into npm artifact: {name}")


def is_internal_transport_codec_member(name: str, product: str, role: str) -> bool:
    if product != "runtime":
        return False
    normalized = posixpath.normpath(name)
    expected = {
        "node": {
            "package/dist/internal-transport-codec.js",
            "package/dist/internal-transport-codec.d.ts",
        },
        "bun": {
            "package/dist/core/internal-transport-codec.js",
            "package/dist/core/internal-transport-codec.d.ts",
        },
    }
    return normalized in expected.get(role, set())


def is_public_entrypoint_member(name: str, role: str) -> bool:
    normalized = posixpath.normpath(name)
    expected = {
        "node": {"package/dist/index.js", "package/dist/index.d.ts"},
        "bun": {
            "package/dist/index.js",
            "package/dist/index.d.ts",
            "package/dist/core/index.js",
            "package/dist/core/index.d.ts",
        },
        "electron": {"package/dist/index.js", "package/dist/index.d.ts"},
    }
    return normalized in expected.get(role, set())


def verify_text_content(archive: tarfile.TarFile, product: str, role: str) -> None:
    for member in archive.getmembers():
        if not member.isfile() or not is_text_member(member.name):
            continue
        file_obj = archive.extractfile(member)
        if file_obj is None:
            continue
        try:
            text = file_obj.read().decode("utf-8")
        except UnicodeDecodeError:
            continue
        for pattern in FORBIDDEN_TEXT_PATTERNS:
            if pattern.search(text):
                fail(f"JavaScript-visible wire/protobuf or private marker leaked into npm artifact: {member.name}")
        if is_internal_transport_codec_member(member.name, product, role):
            continue
        if is_public_entrypoint_member(member.name, role) and PUBLIC_TRANSPORT_HELPER_PATTERN.search(text):
            fail(f"transport framing helper leaked from public package entrypoint: {member.name}")
        for pattern in INTERNAL_TRANSPORT_CODEC_PATTERNS:
            if pattern.search(text):
                fail(f"public transport framing implementation leaked into npm artifact: {member.name}")


def native_library_base(product: str) -> str:
    if product == "runtime":
        return "libcoakka_runtime_v2"
    if product == "logger":
        return "libcoakka_logger_core"
    fail(f"unsupported product lane: {product}")


def native_library_extension(platform: str) -> str:
    extension = ".dll" if platform.startswith("windows-") else ".dylib" if platform.startswith("macos-") else ".so"
    return extension


def native_library_names(product: str, platform: str, expected_native_generation: str) -> tuple[str, str]:
    base = native_library_base(product)
    extension = native_library_extension(platform)
    return f"{base}{extension}", f"{base}-{expected_native_generation}{extension}"


def native_members_by_platform(
    names: list[str],
    product: str,
    expected_native_generation: str,
) -> dict[str, list[str]]:
    by_platform = {platform: [] for platform in PACKAGE_PLATFORMS}
    for name in names:
        normalized = posixpath.normpath(name)
        for platform in PACKAGE_PLATFORMS:
            if f"/{platform}/" not in normalized:
                continue
            generic_name, versioned_name = native_library_names(product, platform, expected_native_generation)
            if posixpath.basename(normalized) in {generic_name, versioned_name}:
                by_platform[platform].append(normalized)
    return by_platform


def verify_native_shape(
    names: list[str],
    product: str,
    role: str,
    expected_native_generation: str,
    expected_internal_dependency: str | None,
    expected_platforms: tuple[str, ...],
) -> None:
    if not expected_platforms:
        fail("expected platform matrix must not be empty")
    if len(set(expected_platforms)) != len(expected_platforms):
        fail("expected platform matrix must not contain duplicates")
    unknown_platforms = sorted(set(expected_platforms) - set(PACKAGE_PLATFORMS))
    if unknown_platforms:
        fail(f"expected platform matrix contains unsupported values: {', '.join(unknown_platforms)}")

    by_platform = native_members_by_platform(names, product, expected_native_generation)
    present = {platform for platform, matches in by_platform.items() if matches}
    expected = set(expected_platforms)

    if role in {"node", "bun"}:
        errors = []
        missing = sorted(expected - present)
        unexpected = sorted(present - expected)
        if missing:
            errors.append(f"{role} package is missing bundled {product} native libraries for: {', '.join(missing)}")
        if unexpected:
            errors.append(f"{role} package includes native libraries outside the release matrix: {', '.join(unexpected)}")
        for platform in expected_platforms:
            matches = by_platform[platform]
            if len(matches) != 1:
                rendered = ", ".join(matches)
                errors.append(f"{role} package must include exactly one {product} native for {platform}: {rendered}")
                continue
            generic_name, versioned_name = native_library_names(product, platform, expected_native_generation)
            basename = posixpath.basename(matches[0])
            if basename == generic_name:
                errors.append(
                    f"{role} package must not ship unversioned native alias for {platform}: {matches[0]}"
                )
            elif basename != versioned_name:
                errors.append(
                    f"{role} package native for {platform} must be {versioned_name}, found {basename}"
                )
        if errors:
            fail("; ".join(errors))
        return

    if role == "electron":
        if present:
            fail("Electron package-manager artifact must delegate natives through its first-party Node package")
        if not expected_internal_dependency:
            fail("Electron package verification requires an expected first-party dependency")
        return

    fail(f"unsupported npm role: {role}")


def verify_scripts(package: dict) -> None:
    scripts = package.get("scripts") or {}
    for script_name in INSTALL_LIFECYCLE_SCRIPTS:
        if script_name in scripts:
            fail(f"package-manager artifact must not run npm lifecycle script: {script_name}")


def verify_license(package: dict, names: list[str]) -> None:
    if package.get("license") != LICENSE_VALUE:
        fail(f"package license must be {LICENSE_VALUE!r}")
    if "package/LICENSE.md" not in names:
        fail("package-manager artifact must include package/LICENSE.md")


def dependency_bucket(package: dict, bucket: str) -> dict:
    deps = package.get(bucket) or {}
    if not isinstance(deps, dict):
        fail(f"package {bucket} must be an object when present")
    return deps


def verify_dependency_name(name: str) -> None:
    for pattern in FORBIDDEN_DEPENDENCY_PATTERNS:
        if pattern.search(name):
            fail(f"forbidden native or implementation dependency in npm metadata: {name}")


def verify_dependency_spec(name: str, spec: str, expected_internal_dependency: str | None) -> None:
    if not isinstance(spec, str):
        fail(f"dependency spec for {name} must be a string")
    lowered = spec.lower()
    if (
        "://" in lowered
        or lowered.startswith("file:")
        or lowered.startswith("link:")
        or lowered.startswith("workspace:")
        or lowered.startswith("git+")
        or lowered.endswith(".tgz")
    ):
        fail(f"npm package-manager dependency must resolve through registry metadata: {name}={spec}")
    if name == expected_internal_dependency and not npm_spec_is_registry_spec(spec):
        fail(f"first-party dependency must use an npm registry version spec: {name}={spec}")


def verify_dependencies(package: dict, expected_internal_dependency: str | None) -> None:
    errors = []
    runtime_deps = dependency_bucket(package, "dependencies")
    optional_deps = dependency_bucket(package, "optionalDependencies")
    peer_deps = dependency_bucket(package, "peerDependencies")

    if expected_internal_dependency:
        if set(runtime_deps) != {expected_internal_dependency}:
            rendered = ", ".join(sorted(runtime_deps)) or "<none>"
            errors.append(f"expected only first-party dependency {expected_internal_dependency}, found: {rendered}")
        elif expected_internal_dependency in runtime_deps:
            try:
                verify_dependency_spec(
                    expected_internal_dependency,
                    runtime_deps[expected_internal_dependency],
                    expected_internal_dependency,
                )
            except VerificationError as exc:
                errors.append(str(exc))
    else:
        for name, spec in runtime_deps.items():
            for check, check_args in (
                (verify_dependency_name, (name,)),
                (verify_dependency_spec, (name, spec, expected_internal_dependency)),
            ):
                try:
                    check(*check_args)
                except VerificationError as exc:
                    errors.append(str(exc))

    for bucket_name, deps in (("optionalDependencies", optional_deps), ("peerDependencies", peer_deps)):
        if deps:
            rendered = ", ".join(sorted(deps))
            errors.append(f"package-manager artifact must not rely on {bucket_name}: {rendered}")
    if errors:
        fail("; ".join(errors))


def verify_public_metadata(package: dict) -> None:
    errors = []
    encoded = json.dumps(package, sort_keys=True)
    for marker in FORBIDDEN_METADATA_SUBSTRINGS:
        if marker in encoded:
            errors.append(f"private source repo marker leaked into npm metadata: {marker}")

    repository = package.get("repository")
    if not isinstance(repository, dict):
        errors.append("package-manager artifact must declare public repository metadata")
    elif repository.get("url") != PUBLIC_REPOSITORY_URL:
        errors.append(f"package repository.url must be {PUBLIC_REPOSITORY_URL!r}")

    bugs = package.get("bugs")
    if not isinstance(bugs, dict) or bugs.get("url") != PUBLIC_BUGS_URL:
        errors.append(f"package bugs.url must be {PUBLIC_BUGS_URL!r}")

    if errors:
        fail("; ".join(errors))


def exported_targets(value: object) -> list[str]:
    if isinstance(value, str):
        return [value]
    if isinstance(value, dict):
        targets = []
        for nested in value.values():
            targets.extend(exported_targets(nested))
        return targets
    if isinstance(value, list):
        targets = []
        for nested in value:
            targets.extend(exported_targets(nested))
        return targets
    return []


def verify_exports(package: dict, require_explicit_exports: bool) -> None:
    exports = package.get("exports")
    if exports is None:
        if require_explicit_exports:
            fail("package-manager artifact must declare explicit package exports")
        return
    if not isinstance(exports, dict) or "." not in exports:
        fail("package exports must include the package root entrypoint")

    blocked_markers = ("internal-transport-codec", "protobuf", "wire-codec")
    errors = []
    for key, value in exports.items():
        if any(marker in key.casefold() for marker in blocked_markers):
            errors.append(f"package exports expose internal transport path: {key}")
        for target in exported_targets(value):
            if any(marker in target.casefold() for marker in blocked_markers):
                errors.append(f"package exports target internal transport path: {target}")
    if errors:
        fail("; ".join(errors))


def verify_package_json(
    package: dict,
    package_name: str,
    role: str,
    expected_internal_dependency: str | None,
    require_public_metadata: bool,
) -> None:
    errors = []
    if package.get("name") != package_name:
        errors.append(f"expected package name {package_name!r}, found {package.get('name')!r}")
    if package.get("private") is True:
        errors.append("package-manager artifact must not set private=true")
    if package.get("type") != "module":
        errors.append("package-manager artifact must declare type=module")
    if not package.get("main"):
        errors.append("package-manager artifact must declare main")
    if not package.get("types"):
        errors.append("package-manager artifact must declare types")
    checks = [
        (verify_scripts, (package,)),
        (verify_dependencies, (package, expected_internal_dependency)),
        (verify_exports, (package, require_public_metadata or role == "electron")),
    ]
    if require_public_metadata:
        checks.append((verify_public_metadata, (package,)))
    for check, check_args in checks:
        try:
            check(*check_args)
        except VerificationError as exc:
            errors.append(str(exc))
    if errors:
        fail("; ".join(errors))


def verify_artifact(
    artifact: Path,
    product: str,
    role: str,
    package_name: str,
    expected_native_generation: str,
    expected_internal_dependency: str | None,
    require_public_metadata: bool = False,
    expected_platforms: tuple[str, ...] = PACKAGE_PLATFORMS,
) -> None:
    if not artifact.is_file():
        fail(f"artifact does not exist: {artifact}")
    with tarfile.open(artifact, "r:gz") as archive:
        names = member_names(archive)
        package = read_package_json(archive)
        try:
            verify_text_content(archive, product, role)
        except VerificationError as exc:
            text_content_error = str(exc)
        else:
            text_content_error = ""

    errors = []
    if text_content_error:
        errors.append(text_content_error)
    for check, check_args in (
        (verify_member_boundary, (names,)),
        (verify_package_json, (package, package_name, role, expected_internal_dependency, require_public_metadata)),
        (verify_license, (package, names)),
        (
            verify_native_shape,
            (
                names,
                product,
                role,
                expected_native_generation,
                expected_internal_dependency,
                expected_platforms,
            ),
        ),
    ):
        try:
            check(*check_args)
        except VerificationError as exc:
            errors.append(str(exc))

    manifest = package.get("coakka") or {}
    if manifest:
        native_generation = manifest.get("native_generation")
        if native_generation and native_generation != expected_native_generation:
            errors.append(
                f"package coakka.native_generation is {native_generation!r}, "
                f"expected {expected_native_generation!r}"
            )
    if errors:
        fail("; ".join(errors))


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Verify that an npm-shaped CoAkka artifact is ready for package-manager distribution."
    )
    parser.add_argument("--artifact", type=Path)
    parser.add_argument("--product", choices=("runtime", "logger"))
    parser.add_argument("--role", choices=("node", "bun", "electron"))
    parser.add_argument("--package-name")
    parser.add_argument("--expected-native-generation")
    parser.add_argument("--expected-internal-dependency")
    parser.add_argument(
        "--expected-platform",
        action="append",
        choices=PACKAGE_PLATFORMS,
        dest="expected_platforms",
        help="expected packaged native platform; repeat to declare the exact release matrix",
    )
    parser.add_argument(
        "--candidate-manifest",
        type=Path,
        help="verify all package-manager candidate artifacts listed in a manifest.json file",
    )
    parser.add_argument(
        "--current-candidates",
        action="store_true",
        help="audit the current public Node/Bun/Electron runtime and logger npm candidates",
    )
    parser.add_argument(
        "--require-public-metadata",
        action="store_true",
        help="require public repository and issue metadata; use for new publish candidates",
    )
    return parser.parse_args()


def verify_current_candidates() -> int:
    failures = 0
    for label, product, role, package_name, native_generation, relative_path, dependency in CURRENT_CANDIDATES:
        artifact = REPO_ROOT / relative_path
        try:
            verify_artifact(artifact, product, role, package_name, native_generation, dependency)
        except VerificationError as exc:
            failures += 1
            print(f"[npm-package-manager] {label}: blocked: {exc}", file=sys.stderr)
        else:
            print(f"[npm-package-manager] {label}: ok")
    return 1 if failures else 0


def require_manifest_string(entry: dict, key: str, label: str) -> str:
    value = entry.get(key)
    if not isinstance(value, str) or not value:
        fail(f"{label} manifest entry must include string field {key!r}")
    return value


def manifest_expected_platforms(entry: dict, label: str) -> tuple[str, ...]:
    value = entry.get("expected_platforms")
    if value is None:
        return PACKAGE_PLATFORMS
    if not isinstance(value, list) or not value or not all(isinstance(item, str) and item for item in value):
        fail(f"{label} manifest entry field 'expected_platforms' must be a non-empty string list")
    return tuple(value)


def verify_candidate_manifest(manifest_path: Path, require_public_metadata: bool) -> int:
    if not manifest_path.is_file():
        print(f"[npm-package-manager] candidate manifest does not exist: {manifest_path}", file=sys.stderr)
        return 1
    try:
        with manifest_path.open("r", encoding="utf-8") as fh:
            manifest = json.load(fh)
    except json.JSONDecodeError as exc:
        print(f"[npm-package-manager] candidate manifest is not valid JSON: {exc}", file=sys.stderr)
        return 1

    packages = manifest.get("packages")
    if not isinstance(packages, list) or not packages:
        print("[npm-package-manager] candidate manifest must include a non-empty packages list", file=sys.stderr)
        return 1

    failures = 0
    base = manifest_path.parent
    for entry in packages:
        if not isinstance(entry, dict):
            print("[npm-package-manager] candidate manifest package entry must be an object", file=sys.stderr)
            failures += 1
            continue
        label = str(entry.get("label") or "<unknown>")
        try:
            artifact = base / require_manifest_string(entry, "relative_path", label)
            verify_artifact(
                artifact,
                require_manifest_string(entry, "product", label),
                require_manifest_string(entry, "role", label),
                require_manifest_string(entry, "package_name", label),
                require_manifest_string(entry, "expected_native_generation", label),
                entry.get("expected_internal_dependency"),
                require_public_metadata,
                expected_platforms=manifest_expected_platforms(entry, label),
            )
        except VerificationError as exc:
            failures += 1
            print(f"[npm-package-manager] {label}: blocked: {exc}", file=sys.stderr)
        else:
            print(f"[npm-package-manager] {label}: ok")
    return 1 if failures else 0


def main() -> int:
    args = parse_args()
    if args.current_candidates:
        return verify_current_candidates()
    if args.candidate_manifest:
        return verify_candidate_manifest(args.candidate_manifest, args.require_public_metadata)

    required = (
        args.artifact,
        args.product,
        args.role,
        args.package_name,
        args.expected_native_generation,
    )
    if any(value is None for value in required):
        print(
            "verify-npm-package-manager-artifact.py requires either --current-candidates "
            "or --artifact, --product, --role, --package-name, and --expected-native-generation",
            file=sys.stderr,
        )
        return 2

    try:
        verify_artifact(
            args.artifact,
            args.product,
            args.role,
            args.package_name,
            args.expected_native_generation,
            args.expected_internal_dependency,
            args.require_public_metadata,
            expected_platforms=tuple(args.expected_platforms or PACKAGE_PLATFORMS),
        )
    except VerificationError as exc:
        print(f"[npm-package-manager] blocked: {exc}", file=sys.stderr)
        return 1
    print("[npm-package-manager] ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
