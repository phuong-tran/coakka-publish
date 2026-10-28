#!/usr/bin/env python3
"""Small hostile archive fixtures; no extraction, execution or retained staging."""
import importlib.util
import hashlib
import io
from pathlib import Path
import tarfile
import tempfile
import unittest
from unittest.mock import patch

SPEC = importlib.util.spec_from_file_location(
    "inspect_candidate", Path(__file__).with_name("verify-http-inspect-candidate.py"))
V = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(V)


def fixture(mutate):
    rows = []
    for name in sorted(V.FILES):
        member = tarfile.TarInfo(name)
        member.size = 1
        member.mode = 0o755 if name.startswith("bin/") else 0o644
        rows.append(member)
    mutate(rows)
    data = io.BytesIO()
    with tarfile.open(fileobj=data, mode="w") as archive:
        for member in rows:
            archive.addfile(member, io.BytesIO(b"x"))
    data.seek(0)
    return tarfile.open(fileobj=data, mode="r:")


class AdmissionTest(unittest.TestCase):
    def test_exact_layout(self):
        with fixture(lambda rows: None) as archive:
            V.verify_members(archive)

    def test_rejections(self):
        cases = {
            "duplicate": lambda rows: rows.append(rows[0]),
            "missing": lambda rows: rows.pop(),
            "extra": lambda rows: setattr(rows[0], "name", "debug.map"),
            "traversal": lambda rows: setattr(rows[0], "name", "../bin/tool"),
            "absolute": lambda rows: setattr(rows[0], "name", "/bin/tool"),
            "symlink": lambda rows: setattr(rows[0], "type", tarfile.SYMTYPE),
            "hardlink": lambda rows: setattr(rows[0], "type", tarfile.LNKTYPE),
            "mode": lambda rows: setattr(rows[0], "mode", 0o4755),
        }
        for name, mutate in cases.items():
            with self.subTest(name=name), fixture(mutate) as archive:
                with self.assertRaises(ValueError):
                    V.verify_members(archive)

    def test_warehouse_controls(self):
        # Tiny archive fixtures keep hostile-input tests independent of large
        # release files. Only the real admission command uses production pins.
        cases = ("valid", "hash", "ledger", "extra", "missing", "linked-file", "linked-root")
        for case in cases:
            with self.subTest(case=case), tempfile.TemporaryDirectory(prefix="inspect-pin-") as tmp:
                product = Path(tmp).resolve()
                root = product / "inspect/candidates" / V.CANDIDATE
                root.mkdir(parents=True)
                name = "coakka-http-runtime-inspect-1.0.0-test.tar.gz"
                path = root / name
                with fixture(lambda rows: None) as archive:
                    payload = archive.fileobj.getvalue()
                import gzip
                payload = gzip.compress(payload, mtime=0)
                path.write_bytes(payload)
                sha = hashlib.sha256(payload).hexdigest()
                (root / "SHA256SUMS").write_text(f"{sha}  {name}\n")
                if case == "hash":
                    path.write_bytes(b"replaced")
                elif case == "ledger":
                    (root / "SHA256SUMS").write_text("changed\n")
                elif case == "extra":
                    (root / "unexpected").write_text("extra")
                elif case == "missing":
                    path.unlink()
                elif case == "linked-file":
                    outside = product / "outside.tar.gz"
                    path.rename(outside)
                    path.symlink_to(outside)
                elif case == "linked-root":
                    outside = product / "outside"
                    root.rename(outside)
                    root.symlink_to(outside, target_is_directory=True)
                with patch.object(V, "QUALIFIED", {"test": sha}):
                    if case == "valid":
                        V.verify_candidate(product)
                    else:
                        with self.assertRaises(ValueError):
                            V.verify_candidate(product)


if __name__ == "__main__":
    unittest.main()
