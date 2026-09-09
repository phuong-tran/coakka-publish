#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
release_root="${repo_root}/coakka-http-runtime"
native_release="1.0.0+df1a9e76c3d928e3eebdb82fdee9f8a2ef30b431"
connector_release="1.0.0+3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0-7e24ee5"

fail() {
  echo "[http-runtime-release] $*" >&2
  exit 1
}

verify_single_release() {
  local lane="$1"
  local expected="$2"
  local releases="${release_root}/${lane}/releases"
  local actual=()

  [[ -d "${releases}/${expected}" ]] ||
    fail "missing ${lane} release ${expected}"
  while IFS= read -r directory; do
    actual+=("$(basename "${directory}")")
  done < <(find "${releases}" -mindepth 1 -maxdepth 1 -type d | sort)

  if [[ "${#actual[@]}" -ne 1 || "${actual[0]}" != "${expected}" ]]; then
    fail "${lane} release directory set is not exact: ${actual[*]:-empty}"
  fi

  (cd "${releases}/${expected}" && shasum -a 256 -c SHA256SUMS >/dev/null)
}

verify_single_release native "${native_release}"
for lane in jvm python javascript go; do
  verify_single_release "${lane}" "${connector_release}"
done

python3 - "${release_root}" "${native_release}" "${connector_release}" <<'PY'
import hashlib
import json
import os
import sys
import tarfile
import xml.etree.ElementTree as ET
import zipfile

release_root, native_release, connector_release = sys.argv[1:]
repo_root = os.path.dirname(release_root)
version = "1.0.0"
native_source = "df1a9e76c3d928e3eebdb82fdee9f8a2ef30b431"
core_source = "3434adaeb48e25df32a4c6e9d2ddeb1c16c5b8a0"
connector_source = "7e24ee5773bdc5b5564b352fad8270e6e14b9aa5"


def fail(message: str) -> None:
    print(f"[http-runtime-release] {message}", file=sys.stderr)
    raise SystemExit(1)


def read_manifest(lane: str, release: str) -> tuple[str, dict]:
    path = os.path.join(release_root, lane, "releases", release, "manifest.json")
    with open(path, "r", encoding="utf-8") as handle:
        manifest = json.load(handle)
    if manifest.get("version") != version:
        fail(f"{lane} package version is not {version}")
    if manifest.get("release_directory") != release:
        fail(f"{lane} manifest release directory does not match its path")
    if manifest.get("status") != "released" or manifest.get("distribution") != "github":
        fail(f"{lane} distribution status is not the GitHub release contract")
    if manifest.get("registry_mirrors") != []:
        fail(f"{lane} unexpectedly declares a registry mirror")
    return os.path.dirname(path), manifest


def verify_file(root: str, item: dict) -> None:
    relative = item["file"]
    path = os.path.join(root, relative)
    if not os.path.isfile(path):
        fail(f"missing artifact {relative}")
    size = os.path.getsize(path)
    if "size" in item and item["size"] != size:
        fail(f"size mismatch for {relative}")
    with open(path, "rb") as handle:
        digest = hashlib.sha256(handle.read()).hexdigest()
    if item["sha256"] != digest:
        fail(f"SHA-256 mismatch for {relative}")


native_root, native = read_manifest("native", native_release)
if native.get("implementation_source_commit") != native_source:
    fail("native source identity does not match the release suffix")
if native_release != f"{version}+{native_source}":
    fail("native release suffix is malformed")
for target in native.get("targets", []):
    verify_file(native_root, {"file": target["library"], "sha256": target["sha256"]})
    if "import_library" in target:
        verify_file(
            native_root,
            {
                "file": target["import_library"],
                "sha256": target["import_library_sha256"],
            },
        )

if connector_release != f"{version}+{core_source}-{connector_source[:7]}":
    fail("connector release suffix is malformed")

for lane in ("jvm", "python", "javascript", "go"):
    root, manifest = read_manifest(lane, connector_release)
    recorded_source = manifest.get("connector_source_commit", manifest.get("source_commit"))
    if manifest.get("core_source_commit") != core_source or recorded_source != connector_source:
        fail(f"{lane} source identity does not match the release suffix")
    if lane == "jvm":
        items = manifest.get("targets", [])
    elif lane == "python":
        items = manifest.get("artifacts", [])
    else:
        items = [manifest.get("artifact", {})]
    if not items:
        fail(f"{lane} manifest has no artifacts")
    for item in items:
        verify_file(root, item)

go_root, go_manifest = read_manifest("go", connector_release)
go_module_root = os.path.join(release_root, "go")
go_module = "github.com/phuong-tran/coakka-publish/coakka-http-runtime/go"
if go_manifest.get("module") != go_module:
    fail("Go module path differs from the public GitHub path")
if go_manifest.get("module_tag") != "coakka-http-runtime/go/v1.0.0":
    fail("Go module tag differs from the nested semantic tag")
with open(os.path.join(go_module_root, "go.mod"), "r", encoding="utf-8") as handle:
    if handle.readline().strip() != f"module {go_module}":
        fail("Go module metadata differs from the release manifest")

go_archive = os.path.join(go_root, go_manifest["artifact"]["file"])
with tarfile.open(go_archive, "r:gz") as archive:
    prefix = "coakka-http-go-1.0.0/"
    members = [member for member in archive.getmembers() if member.isfile()]
    if any(member.issym() or member.islnk() for member in archive.getmembers()):
        fail("Go release archive contains a link")
    projected = {
        member.name[len(prefix):]: member
        for member in members
        if member.name.startswith(prefix) and member.name != f"{prefix}README.md"
    }
    expected = {
        name
        for name in os.listdir(go_module_root)
        if name.endswith((".go", ".syso")) or name in {"go.mod", "go.sum"}
    }
    if set(projected) != expected:
        fail("Go module source set differs from the release archive")
    for name, member in projected.items():
        archived = archive.extractfile(member)
        if archived is None:
            fail(f"cannot read Go archive member {name}")
        with open(os.path.join(go_module_root, name), "rb") as handle:
            if archived.read() != handle.read():
                fail(f"Go module file differs from the release archive: {name}")

jvm_root, jvm = read_manifest("jvm", connector_release)
maven = jvm.get("maven_repository", {})
expected_maven = {
    "repository_path": "maven",
    "group": "coakka.http",
    "artifact": "coakka-http-jvm",
    "version": version,
    "assembly_source_commit": "5afd91ed5ffb5abd73212b6d76dbb306d3ce2519",
}
for key, expected in expected_maven.items():
    if maven.get(key) != expected:
        fail(f"JVM Maven {key} differs from the release contract")
verify_file(repo_root, maven.get("jar", {}))

maven_artifact_root = os.path.join(repo_root, "maven", "coakka", "http", "coakka-http-jvm")
maven_version_root = os.path.join(maven_artifact_root, version)
jar_path = os.path.join(maven_version_root, f"coakka-http-jvm-{version}.jar")
pom_path = os.path.join(maven_version_root, f"coakka-http-jvm-{version}.pom")
module_path = os.path.join(maven_version_root, f"coakka-http-jvm-{version}.module")
metadata_path = os.path.join(maven_artifact_root, "maven-metadata.xml")
primary_maven_files = (jar_path, pom_path, module_path, metadata_path)
sidecar_algorithms = {
    "md5": "md5",
    "sha1": "sha1",
    "sha256": "sha256",
    "sha512": "sha512",
}
expected_maven_files = set(primary_maven_files)
for path in primary_maven_files:
    for suffix in sidecar_algorithms:
        expected_maven_files.add(f"{path}.{suffix}")
actual_maven_files = {
    os.path.join(directory, name)
    for directory, _, names in os.walk(maven_artifact_root)
    for name in names
}
if actual_maven_files != expected_maven_files:
    fail("JVM Maven repository file set differs from the release contract")
for path in primary_maven_files:
    if not os.path.isfile(path):
        fail(f"missing JVM Maven file {os.path.relpath(path, repo_root)}")
    with open(path, "rb") as handle:
        content = handle.read()
    for suffix, algorithm in sidecar_algorithms.items():
        with open(f"{path}.{suffix}", "r", encoding="ascii") as handle:
            expected = handle.read().strip()
        if hashlib.new(algorithm, content).hexdigest() != expected:
            fail(f"JVM Maven {suffix} mismatch for {os.path.basename(path)}")


def local_name(tag: str) -> str:
    return tag.rsplit("}", 1)[-1]


def child_text(node: ET.Element, name: str) -> str:
    for child in node:
        if local_name(child.tag) == name:
            return (child.text or "").strip()
    return ""


pom = ET.parse(pom_path).getroot()
if (
    child_text(pom, "groupId"),
    child_text(pom, "artifactId"),
    child_text(pom, "version"),
) != ("coakka.http", "coakka-http-jvm", version):
    fail("JVM Maven POM coordinate differs")
expected_dependencies = {
    ("org.jetbrains.kotlin", "kotlin-stdlib", "2.3.10", "compile"),
    ("io.netty", "netty-codec-http", "4.1.137.Final", "runtime"),
    ("io.netty", "netty-codec-http2", "4.1.137.Final", "runtime"),
    ("io.netty", "netty-handler", "4.1.137.Final", "runtime"),
}
actual_dependencies = set()
for dependency in pom.iter():
    if local_name(dependency.tag) != "dependency":
        continue
    values = {
        local_name(child.tag): (child.text or "").strip()
        for child in dependency
    }
    actual_dependencies.add(
        (
            values.get("groupId", ""),
            values.get("artifactId", ""),
            values.get("version", ""),
            values.get("scope", ""),
        ),
    )
if actual_dependencies != expected_dependencies:
    fail("JVM Maven POM dependency set differs")

with open(module_path, "r", encoding="utf-8") as handle:
    module = json.load(handle)
component = module.get("component", {})
if (component.get("group"), component.get("module"), component.get("version")) != (
    "coakka.http",
    "coakka-http-jvm",
    version,
):
    fail("JVM Gradle module coordinate differs")
variants = module.get("variants", [])
if [variant.get("name") for variant in variants] != ["apiElements", "runtimeElements"]:
    fail("JVM Gradle module variants differ")
with open(jar_path, "rb") as handle:
    jar_content = handle.read()
expected_module_file = {
    "name": os.path.basename(jar_path),
    "url": os.path.basename(jar_path),
    "size": len(jar_content),
    "sha512": hashlib.sha512(jar_content).hexdigest(),
    "sha256": hashlib.sha256(jar_content).hexdigest(),
    "sha1": hashlib.sha1(jar_content).hexdigest(),
    "md5": hashlib.md5(jar_content).hexdigest(),
}
for variant in variants:
    if variant.get("files") != [expected_module_file]:
        fail(f"JVM Gradle module artifact differs for {variant.get('name')}")

platforms = (
    "macos-aarch64",
    "linux-aarch64",
    "linux-x86_64",
    "windows-aarch64",
    "windows-x86_64",
)
with zipfile.ZipFile(jar_path) as archive:
    names = {entry.filename for entry in archive.infolist() if not entry.is_dir()}
    native_names = {name for name in names if name.startswith("native/")}
    if len(native_names) != len(platforms) * 2:
        fail("JVM Maven JAR native resource count differs")
    for platform in platforms:
        properties_path = f"META-INF/coakka-http/native/{platform}/native.properties"
        if properties_path not in names:
            fail(f"JVM Maven JAR is missing metadata for {platform}")
        properties = {}
        for line in archive.read(properties_path).decode("ascii").splitlines():
            if line and not line.startswith("#"):
                key, separator, value = line.partition("=")
                if not separator or key in properties:
                    fail(f"JVM Maven JAR metadata is malformed for {platform}")
                properties[key] = value
        if properties.get("format") != "1" or properties.get("platform") != platform:
            fail(f"JVM Maven JAR metadata identity differs for {platform}")
        for role in ("runtime", "bridge"):
            resource = properties.get(f"{role}.resource", "")
            if not resource.startswith(f"native/{platform}/") or resource not in native_names:
                fail(f"JVM Maven JAR {role} resource differs for {platform}")
            payload = archive.read(resource)
            if properties.get(f"{role}.size") != str(len(payload)):
                fail(f"JVM Maven JAR {role} size differs for {platform}")
            if properties.get(f"{role}.sha256") != hashlib.sha256(payload).hexdigest():
                fail(f"JVM Maven JAR {role} checksum differs for {platform}")

print("[http-runtime-release] ok: version=1.0.0 lanes=5 immutable-identities=2")
PY
