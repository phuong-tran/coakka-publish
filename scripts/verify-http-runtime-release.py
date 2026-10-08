#!/usr/bin/env python3
"""Admit exact HTTP native candidate archives without implying release approval.

Pins are independent of the candidate's manifest. No archive is extracted and
no private source is required. Candidate admission is not branch-merge or
publication authorization; the owning coordinated audit records those gates.
"""

import argparse
import hashlib
import json
import posixpath
from pathlib import Path, PurePosixPath
import re
import subprocess
import sys
import tarfile


CANDIDATE = "2026-10-08-r3"
# Admitted archive identities from the completed five-target package gates.
QUALIFIED = {
    "linux-aarch64": "4225febd7681d550b3f85feeb0254ed72d6332fc37079210a8f3c940ed4c148a",
    "linux-x86_64": "7c0b63bcca23d984ea630eb2c8ee6561b7f7ceea7850eb2f810b1edbfc817634",
    "macos-aarch64": "a3260760511e18334c1e680ff46b45b043acd305243cd98463be1c9aaa74356b",
    "windows-aarch64": "470030b59e5a2dd674749e6973ccf6dd98d15972157a6dd75edf79bc6d71ca10",
    "windows-x86_64": "cede881af8fc24c45fe58409a17417abcdda78bf8e4b88182093f9bb38e9e914"
}


def digest(path):
    with path.open("rb") as stream:
        result = hashlib.sha256()
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            result.update(chunk)
        return result.hexdigest()


def require(condition, message):
    if not condition:
        raise ValueError(message)


def archive_name(target):
    return f"coakka-http-native-1.0.0-candidate-{target}.tar.gz"


def layout(target):
    """Exact installed SDK inventory; Unix links are checked separately."""
    files = {f"include/coakka/http/{name}.h"
             for name in ("http", "execution", "request_control")}
    files.update(f"lib/cmake/CoAkkaHttp/CoAkkaHttp{name}.cmake" for name in
                 ("Config", "ConfigVersion", "Targets-release", "Targets"))
    files.update(f"share/licenses/coakka-http-runtime/{name}" for name in
                 ("LICENSE", "NATIVE-LICENSE.md", "PACKAGE-LICENSE.md", "NOTICE"))
    files.add("share/coakka-http-runtime/coakka-http-runtime.artifact.json")
    links = {}
    if target.startswith("windows-"):
        binary = "bin/coakka_http_runtime.dll"
        files.add("lib/coakka_http_runtime.lib")
    elif target.startswith("macos-"):
        binary = "lib/libcoakka_http_runtime.1.0.0.dylib"
        links = {"lib/libcoakka_http_runtime.dylib": "libcoakka_http_runtime.1.dylib",
                 "lib/libcoakka_http_runtime.1.dylib": "libcoakka_http_runtime.1.0.0.dylib"}
    else:
        binary = "lib/libcoakka_http_runtime.so.1.0.0"
        links = {"lib/libcoakka_http_runtime.so": "libcoakka_http_runtime.so.1",
                 "lib/libcoakka_http_runtime.so.1": "libcoakka_http_runtime.so.1.0.0"}
    return files | {binary}, links, binary


def verify_members(archive, target):
    """Reject duplicates, extras and non-regular entries before reading payloads."""
    members = archive.getmembers()
    files, links, binary = layout(target)
    names = [entry.name for entry in members]
    require(len(names) == len(set(names)), "duplicate archive member")
    require(set(names) == files | set(links), "unexpected installed inventory")
    for entry in members:
        path = PurePosixPath(entry.name)
        require(not path.is_absolute() and ".." not in path.parts, "unsafe archive path")
        if entry.name in links:
            require(entry.issym() and entry.linkname == links[entry.name], "unsafe library link")
        else:
            require(entry.isfile() and 0 <= entry.size <= 32 * 1024 * 1024,
                    "unsupported archive entry or size")
    metadata = json.load(archive.extractfile(
        "share/coakka-http-runtime/coakka-http-runtime.artifact.json"))
    require(isinstance(metadata, dict), "artifact identity must be an object")
    require(metadata.get("schema") == "coakka.http.artifact-metadata.v1"
            and metadata.get("product") == "coakka-http-runtime"
            and metadata.get("target") == target, "artifact identity mismatch")
    data = archive.extractfile(binary).read()
    require(metadata.get("file") == PurePosixPath(binary).name
            and type(metadata.get("size")) is int and metadata["size"] == len(data)
            and metadata.get("sha256") == hashlib.sha256(data).hexdigest(),
            "artifact binary mismatch")
    header = archive.extractfile("include/coakka/http/http.h").read()
    require(b"COAKKA_HTTP_ABI_VERSION UINT32_C(12)" in header, "wrong application header")


def verify_candidate(product):
    root = product / "native/candidates" / CANDIDATE
    require(not (product / "native/releases").exists(), "obsolete native assembly remains")
    for path in product.rglob("*"):
        name = path.name.lower().replace("_", "-")
        require("third-party" not in name and "thirdparty" not in name,
                "dependency inventory is not a distributable artifact")
    for path in (product / "native/candidate.json", root / "SHA256SUMS"):
        for entry in (path, *path.parents):
            require(not entry.is_symlink(), "linked candidate path")
            if entry == product:
                break
    manifest = json.loads((product / "native/candidate.json").read_text())
    require(manifest == {"schema": "coakka.http.native-candidate.v1",
                         "candidate": CANDIDATE, "status": "candidate",
                         "archives": {archive_name(t): h for t, h in QUALIFIED.items()}},
            "candidate manifest does not match admitted archives")
    expected = {archive_name(t) for t in QUALIFIED} | {"SHA256SUMS"}
    require(root.is_dir() and not root.is_symlink(), "missing or linked candidate")
    require({p.name for p in root.iterdir()} == expected, "candidate file inventory")
    sums = "".join(f"{QUALIFIED[t]}  {archive_name(t)}\n" for t in sorted(QUALIFIED))
    require((root / "SHA256SUMS").read_text() == sums, "checksum ledger mismatch")
    for target, sha in QUALIFIED.items():
        path = root / archive_name(target)
        require(path.is_file() and not path.is_symlink(), "archive must be a regular file")
        require(digest(path) == sha, f"unqualified archive: {target}")
        with tarfile.open(path, "r:gz") as archive:
            verify_members(archive, target)
            for name in ("LICENSE", "NOTICE", "NATIVE-LICENSE.md", "PACKAGE-LICENSE.md"):
                # Exact archive hashes above preserve producer bytes. Windows
                # NOTICE uses CRLF; the repository copy uses LF. Compare text
                # with only that line-ending distinction, never strip clauses.
                packaged = archive.extractfile(f"share/licenses/coakka-http-runtime/{name}").read()
                require(packaged.replace(b"\r\n", b"\n")
                        == (product / name).read_bytes().replace(b"\r\n", b"\n"),
                        f"product legal material differs: {name}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--candidate", action="store_true",
                        help="verify native candidate only, not release readiness")
    parser.add_argument("--all-candidates", action="store_true",
                        help="verify native, connector and Inspect candidates, not publication approval")
    args = parser.parse_args()
    product = Path(__file__).resolve().parent.parent / "coakka-http-runtime"
    try:
        verify_candidate(product)
        if args.all_candidates:
            pins = json.loads(Path(__file__).with_name("http-runtime-connector-candidates.json").read_text())
            require(set(pins) == {"go", "jvm", "javascript", "python"}, "connector candidate lane inventory")
            for lane, record in pins.items():
                verify_connector_candidate(product, lane, record)
            subprocess.run([sys.executable, str(Path(__file__).with_name(
                "verify-http-inspect-candidate.py"))], check=True)
        if not (args.candidate or args.all_candidates):
            raise ValueError("publication is separate; use --all-candidates for archive admission")
    except (OSError, ValueError, tarfile.TarError, subprocess.CalledProcessError) as error:
        parser.exit(1, f"[http-runtime-release] {error}\n")
    count = 27 if args.all_candidates else 5
    print(f"[http-runtime-release] candidate admission verified: {count} exact archives; NOT release approval")


def verify_connector_payload(archive, prefix):
    """Check safe archive members and each inner checksum without extraction.

    Admission first checks a separately pinned complete archive digest. This
    second check catches malformed producer layouts; it never accepts arbitrary
    files just because an archive carries its own checksum ledger.
    """
    entries = archive.getmembers()
    names = [entry.name.rstrip("/") for entry in entries]
    require(len(names) == len(set(names)) and len(entries) <= 256, "duplicate or excessive connector entries")
    files = {}
    links = {}
    for entry in entries:
        name = entry.name.rstrip("/")
        path = PurePosixPath(name)
        require(not path.is_absolute() and ".." not in path.parts
                and "\\" not in name and (name == prefix or name.startswith(prefix + "/")),
                "unsafe connector archive path")
        require(entry.isdir() or entry.isfile() or entry.issym(), "unsupported connector archive entry")
        require(0 <= entry.size <= 32 * 1024 * 1024, "oversized connector entry")
        if entry.issym():
            destination = posixpath.normpath(posixpath.join(posixpath.dirname(name), entry.linkname))
            require(not entry.linkname.startswith("/") and destination.startswith(prefix + "/"),
                    "escaping connector link")
            links[name] = destination
        elif entry.isfile():
            files[name] = entry
    for destination in links.values():
        seen = set()
        while destination in links:
            require(destination not in seen, "cyclic connector link")
            seen.add(destination)
            destination = links[destination]
        require(destination in files, "broken connector link")
    ledger = prefix + "/SHA256SUMS"
    require(ledger in files, "missing inner checksum ledger")
    recorded = {}
    for line in archive.extractfile(files[ledger]).read().decode("utf-8").splitlines():
        match = re.fullmatch(r"([0-9a-f]{64})  (.+)", line)
        require(match is not None, "malformed inner checksum")
        relative = match[2].removeprefix("./")
        name = prefix + "/" + relative
        require(name not in recorded and name in files, "duplicate or unknown inner checksum")
        recorded[name] = match[1]
    require(set(recorded) == set(files) - {ledger}, "inner checksum inventory")
    for name, sha in recorded.items():
        require(hashlib.sha256(archive.extractfile(files[name]).read()).hexdigest() == sha,
                f"connector payload differs: {name}")


def verify_connector_candidate(product, lane, record):
    """Admit precisely the five independently qualified archives for one lane."""
    require(set(record["sha256"]) == set(QUALIFIED), "connector platform inventory")
    root = product / lane / "candidates" / record["date"]
    files = {f"coakka-http-{lane}-1.0.0-candidate-{target}.{record['extension']}": sha
             for target, sha in record["sha256"].items()}
    for path in (root, *root.parents):
        require(not path.is_symlink(), "linked connector directory")
        if path == product:
            break
    require({path.name for path in root.iterdir()} == set(files) | {"SHA256SUMS"},
            "connector candidate file inventory")
    require(not (root / "SHA256SUMS").is_symlink(), "linked connector checksum ledger")
    expected = "".join(f"{sha}  {name}\n" for name, sha in sorted(files.items()))
    require((root / "SHA256SUMS").read_text() == expected, "connector checksum ledger differs")
    for name, sha in files.items():
        path = root / name
        require(path.is_file() and not path.is_symlink() and digest(path) == sha,
                f"unqualified connector archive: {name}")
        prefix = "package" if lane == "javascript" else name.removesuffix(".tar.gz")
        with tarfile.open(path, "r:gz") as archive:
            verify_connector_payload(archive, prefix)


if __name__ == "__main__":
    main()
