#!/usr/bin/env python3
"""Admit the two qualified standalone HTTP Inspect archives without extraction.

Independent whole-archive pins bind prior matching-host/hardening evidence.
Inventory validation is bounded and never executes a supplied archive.
"""
import argparse
import hashlib
from pathlib import Path
import tarfile

CANDIDATE = "2026-10-08"
QUALIFIED = {
    "linux-aarch64": "4db4ead159e50ea1eb2556051c9547fedcc91edc995d98b02eb337a72adf4847",
    "macos-aarch64": "e497ef4fc0cfbc7fa0c321d36c30d18bd872e28e984aad5011599e4098300141",
}
FILES = {"bin/coakka-http-runtime-inspect"} | {
    "share/licenses/coakka-http-runtime-inspect/" + name
    for name in ("LICENSE", "NOTICE", "NATIVE-LICENSE.md", "PACKAGE-LICENSE.md")
}


def require(condition, message):
    if not condition:
        raise ValueError(message)


def verify_members(archive):
    """Five ordinary files only; bounded headers and payloads, no links/devices."""
    seen = set()
    for entry in archive:
        require(entry.name in FILES and entry.name not in seen,
                "unexpected or duplicate Inspect member")
        require(entry.isfile() and 0 < entry.size <= 32 * 1024 * 1024,
                "invalid Inspect member type or size")
        require(entry.mode == (0o755 if entry.name.startswith("bin/") else 0o644),
                "unexpected Inspect member permissions")
        seen.add(entry.name)
    require(seen == FILES, "incomplete Inspect payload")


def verify_candidate(product):
    """Reject incomplete, replaced or redirected candidates before admission."""
    root = product / "inspect/candidates" / CANDIDATE
    for entry in (root, *root.parents):
        require(not entry.is_symlink(), "linked Inspect candidate path")
    pins = {f"coakka-http-runtime-inspect-1.0.0-{target}.tar.gz": sha
            for target, sha in QUALIFIED.items()}
    require({entry.name for entry in root.iterdir()} == set(pins) | {"SHA256SUMS"},
            "unexpected Inspect candidate inventory")
    ledger = root / "SHA256SUMS"
    require(ledger.is_file() and not ledger.is_symlink() and ledger.stat().st_size < 1024,
            "invalid Inspect checksum ledger")
    require(ledger.read_text() == "".join(f"{sha}  {name}\n" for name, sha in sorted(pins.items())),
            "Inspect checksum ledger differs")
    for name, sha in pins.items():
        path = root / name
        require(path.is_file() and not path.is_symlink() and path.stat().st_size <= 16 * 1024 * 1024,
                "invalid Inspect archive")
        digest = hashlib.sha256()
        with path.open("rb") as stream:
            for block in iter(lambda: stream.read(1024 * 1024), b""):
                digest.update(block)
        require(digest.hexdigest() == sha, "unqualified Inspect archive")
        with tarfile.open(path, "r:gz") as archive:
            verify_members(archive)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.parse_args()
    try:
        verify_candidate(Path(__file__).resolve().parent.parent / "coakka-http-runtime")
    except (OSError, ValueError, tarfile.TarError) as error:
        parser.exit(1, f"[http-inspect-candidate] {error}\n")
    print("[http-inspect-candidate] verified 2 exact archives; not publication approval")


if __name__ == "__main__":
    main()
