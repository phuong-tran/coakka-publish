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
        "1.3.1+bda2ef5",
        "package-manager/npm/candidates/f3e25ec/runtime/node/coakka-v2-connector-node-1.3.4.tgz",
        None,
    ),
    (
        "runtime-bun",
        "runtime",
        "bun",
        "coakka-v2-connector-bun",
        "1.3.1+bda2ef5",
        "package-manager/npm/candidates/f3e25ec/runtime/bun/coakka-v2-connector-bun-1.3.4.tgz",
        None,
    ),
    (
        "runtime-electron",
        "runtime",
        "electron",
        "coakka-v2-connector-electron",
        "1.3.1+bda2ef5",
        "package-manager/npm/candidates/f3e25ec/runtime/electron/coakka-v2-connector-electron-1.3.4.tgz",
        "coakka-v2-connector-node",
    ),
    (
        "logger-node",
        "logger",
        "node",
        "coakka-logger-node",
        "1.2.1+f50756ebff0d",
        "package-manager/npm/candidates/f3e25ec/logger/node/coakka-logger-node-1.2.3.tgz",
        None,
    ),
    (
        "logger-bun",
        "logger",
        "bun",
        "coakka-logger-bun",
        "1.2.1+f50756ebff0d",
        "package-manager/npm/candidates/f3e25ec/logger/bun/coakka-logger-bun-1.2.3.tgz",
        None,
    ),
    (
        "logger-electron",
        "logger",
        "electron",
        "coakka-logger-electron",
        "1.2.1+f50756ebff0d",
        "package-manager/npm/candidates/f3e25ec/logger/electron/coakka-logger-electron-1.2.3.tgz",
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
        for marker in FORBIDDEN_MEMBER_SUBSTRINGS:
            if marker in normalized:
                fail(f"unpublished source identity leaked into npm artifact: {name}")


def native_library_name(product: str, platform: str) -> str:
    extension = ".dll" if platform.startswith("windows-") else ".dylib" if platform.startswith("macos-") else ".so"
    if product == "runtime":
        return f"libcoakka_runtime_v2{extension}"
    if product == "logger":
        return f"libcoakka_logger_core{extension}"
    fail(f"unsupported product lane: {product}")


def native_members_by_platform(names: list[str], product: str) -> dict[str, list[str]]:
    by_platform = {platform: [] for platform in PACKAGE_PLATFORMS}
    for name in names:
        normalized = posixpath.normpath(name)
        for platform in PACKAGE_PLATFORMS:
            if f"/{platform}/" not in normalized:
                continue
            if posixpath.basename(normalized) == native_library_name(product, platform):
                by_platform[platform].append(normalized)
    return by_platform


def verify_native_shape(names: list[str], product: str, role: str, expected_internal_dependency: str | None) -> None:
    by_platform = native_members_by_platform(names, product)
    present = {platform for platform, matches in by_platform.items() if matches}

    if role in {"node", "bun"}:
        missing = sorted(set(PACKAGE_PLATFORMS) - present)
        if missing:
            fail(f"{role} package is missing bundled {product} native libraries for: {', '.join(missing)}")
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


def verify_package_json(package: dict, package_name: str, role: str, expected_internal_dependency: str | None) -> None:
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
    if role == "electron" and "exports" not in package:
        errors.append("Electron package-manager artifact must declare explicit exports")
    for check, check_args in (
        (verify_scripts, (package,)),
        (verify_dependencies, (package, expected_internal_dependency)),
    ):
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
) -> None:
    if not artifact.is_file():
        fail(f"artifact does not exist: {artifact}")
    with tarfile.open(artifact, "r:gz") as archive:
        names = member_names(archive)
        package = read_package_json(archive)

    errors = []
    for check, check_args in (
        (verify_member_boundary, (names,)),
        (verify_package_json, (package, package_name, role, expected_internal_dependency)),
        (verify_license, (package, names)),
        (verify_native_shape, (names, product, role, expected_internal_dependency)),
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
        "--candidate-manifest",
        type=Path,
        help="verify all package-manager candidate artifacts listed in a manifest.json file",
    )
    parser.add_argument(
        "--current-candidates",
        action="store_true",
        help="audit the current public Node/Bun/Electron runtime and logger npm candidates",
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


def verify_candidate_manifest(manifest_path: Path) -> int:
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
        return verify_candidate_manifest(args.candidate_manifest)

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
        )
    except VerificationError as exc:
        print(f"[npm-package-manager] blocked: {exc}", file=sys.stderr)
        return 1
    print("[npm-package-manager] ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
