#!/usr/bin/env python3
"""Check fail-closed native release-record decisions without release artifacts."""

from __future__ import annotations

import importlib.util
from pathlib import Path
import tempfile
import unittest


SCRIPT = Path(__file__).with_name("verify-http-runtime-release.py")
SPEC = importlib.util.spec_from_file_location("http_runtime_release_verifier", SCRIPT)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("cannot load the HTTP runtime release verifier")
VERIFIER = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(VERIFIER)


class ReleaseRecordTest(unittest.TestCase):
    """A draft record must not satisfy the final release decision gate."""

    def test_pending_record_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory(prefix="coakka-http-release-test-") as path:
            release = Path(path)
            (release / "RELEASE.md").write_text("# Release\n\nStatus: pending\n")
            with self.assertRaisesRegex(SystemExit, "release record status"):
                VERIFIER.verify_release_record(release)

    def test_final_status_cannot_leave_pending_body(self) -> None:
        with tempfile.TemporaryDirectory(prefix="coakka-http-release-test-") as path:
            release = Path(path)
            (release / "RELEASE.md").write_text(
                "# Release\n\nStatus: ready-to-release\n\n"
                "Linux ARM64 is pending. Do not distribute.\n"
            )
            with self.assertRaisesRegex(SystemExit, "draft content"):
                VERIFIER.verify_release_record(release)

    def test_unique_final_status_is_required(self) -> None:
        with tempfile.TemporaryDirectory(prefix="coakka-http-release-test-") as path:
            release = Path(path)
            record = release / "RELEASE.md"
            record.write_text("# Release\n\nStatus: ready-to-release\n")
            VERIFIER.verify_release_record(release)
            record.write_text(
                "# Release\n\nStatus: ready-to-release\nStatus: pending\n"
            )
            with self.assertRaisesRegex(SystemExit, "release record status"):
                VERIFIER.verify_release_record(release)


if __name__ == "__main__":
    unittest.main()
