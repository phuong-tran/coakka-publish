#!/usr/bin/env python3
"""Adversarial candidate admission fixtures; no compiler or extraction needed."""

import hashlib
import importlib.util
import io
import json
from pathlib import Path
import sys
import tarfile
import tempfile
import unittest
from unittest.mock import patch

SCRIPT = Path(__file__).with_name("verify-http-runtime-release.py")
SPEC = importlib.util.spec_from_file_location("release_verifier", SCRIPT)
V = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(V)


def fixture(target, mutate=None):
    """Tiny deterministic SDK shape; only the candidate gate pins real bytes."""
    files, links, binary = V.layout(target)
    entries = []
    for name in sorted(files | set(links)):
        entry = tarfile.TarInfo(name)
        data = b"fixture"
        if name in links:
            entry.type, entry.linkname, data = tarfile.SYMTYPE, links[name], b""
        elif name.endswith(".artifact.json"):
            data = json.dumps({"schema": "coakka.http.artifact-metadata.v1",
                               "product": "coakka-http-runtime", "target": target,
                               "file": Path(binary).name, "size": 7,
                               "sha256": hashlib.sha256(b"fixture").hexdigest()}).encode()
        elif name.endswith("/http.h"):
            data = b"#define COAKKA_HTTP_ABI_VERSION UINT32_C(12)\n"
        entry.size = len(data)
        entries.append((entry, data))
    if mutate:
        mutate(entries)
    stream = io.BytesIO()
    with tarfile.open(fileobj=stream, mode="w") as archive:
        for entry, data in entries:
            archive.addfile(entry, io.BytesIO(data))
    stream.seek(0)
    return tarfile.open(fileobj=stream, mode="r:")


class CandidateTest(unittest.TestCase):
    def test_obsolete_native_abi_is_rejected(self):
        def mutate(rows):
            for index, (entry, data) in enumerate(rows):
                if entry.name.endswith('/http.h'):
                    data = data.replace(b'UINT32_C(12)', b'UINT32_C(11)')
                    entry.size = len(data)
                    rows[index] = entry, data
        with fixture('linux-aarch64', mutate) as archive:
            with self.assertRaisesRegex(ValueError, 'header'):
                V.verify_members(archive, 'linux-aarch64')

    def test_all_target_layouts(self):
        for target in V.QUALIFIED:
            with self.subTest(target=target), fixture(target) as archive:
                V.verify_members(archive, target)

    def test_duplicate(self):
        with fixture("linux-aarch64", lambda rows: rows.append(rows[0])) as archive:
            with self.assertRaisesRegex(ValueError, "duplicate"):
                V.verify_members(archive, "linux-aarch64")

    def test_missing_and_extra_and_traversal(self):
        mutations = [lambda rows: rows.pop(),
                     lambda rows: setattr(rows[0][0], "name", "private.txt"),
                     lambda rows: setattr(rows[0][0], "name", "../outside")]
        for mutate in mutations:
            with fixture("linux-aarch64", mutate) as archive:
                with self.assertRaisesRegex(ValueError, "inventory"):
                    V.verify_members(archive, "linux-aarch64")

    def test_symlink_cannot_escape(self):
        def mutate(rows):
            next(entry for entry, _ in rows if entry.issym()).linkname = "/outside"
        with fixture("linux-aarch64", mutate) as archive:
            with self.assertRaisesRegex(ValueError, "link"):
                V.verify_members(archive, "linux-aarch64")

    def test_hardlink_not_admitted(self):
        def mutate(rows):
            rows[0][0].type, rows[0][0].linkname = tarfile.LNKTYPE, "other"
            rows[0][0].size = 0
            rows[0] = (rows[0][0], b"")
        with fixture("windows-aarch64", mutate) as archive:
            with self.assertRaisesRegex(ValueError, "entry"):
                V.verify_members(archive, "windows-aarch64")

    def test_wrong_platform_identity(self):
        def mutate(rows):
            for index, (entry, data) in enumerate(rows):
                if entry.name.endswith(".artifact.json"):
                    data = data.replace(b"linux-aarch64", b"linux-x86_64")
                    entry.size = len(data)
                    rows[index] = entry, data
        with fixture("linux-aarch64", mutate) as archive:
            with self.assertRaisesRegex(ValueError, "identity"):
                V.verify_members(archive, "linux-aarch64")

    def test_final_release_stays_closed(self):
        with patch.object(V, "verify_candidate"), patch.object(sys, "argv", [str(SCRIPT)]):
            with self.assertRaises(SystemExit) as error:
                V.main()
            self.assertEqual(error.exception.code, 1)

    def test_manifest_cannot_promote_itself(self):
        with tempfile.TemporaryDirectory(prefix="http-admission-") as directory:
            product = Path(directory).resolve()
            (product / "native").mkdir()
            (product / "native/candidate.json").write_text(json.dumps({
                "schema": "coakka.http.native-candidate.v1", "candidate": V.CANDIDATE,
                "status": "ready-to-release", "archives": {}}))
            with self.assertRaisesRegex(ValueError, "manifest"):
                V.verify_candidate(product)

    def test_real_candidate(self):
        V.verify_candidate(SCRIPT.parent.parent / "coakka-http-runtime")

    def test_real_connector_candidates(self):
        pins = json.loads(SCRIPT.with_name("http-runtime-connector-candidates.json").read_text())
        for lane, record in pins.items():
            with self.subTest(lane=lane):
                V.verify_connector_candidate(SCRIPT.parent.parent / "coakka-http-runtime", lane, record)

    def test_connector_path_traversal(self):
        with fixture("linux-aarch64") as archive:
            with self.assertRaisesRegex(ValueError, "unsafe connector archive path"):
                V.verify_connector_payload(archive, "package")


if __name__ == "__main__":
    unittest.main()
