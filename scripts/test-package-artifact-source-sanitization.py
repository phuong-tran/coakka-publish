#!/usr/bin/env python3

"""Focused tests for native input sanitization in addon packaging."""

from __future__ import annotations

import importlib.util
import tempfile
import unittest
from pathlib import Path
from unittest import mock


SCRIPT = Path(__file__).with_name("package-artifact-source-release.py")
SPEC = importlib.util.spec_from_file_location("artifact_source_packager", SCRIPT)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"could not load packager: {SCRIPT}")
PACKAGER = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(PACKAGER)


class NativeInputSanitizationTests(unittest.TestCase):
    def test_regular_binary_with_private_path_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as raw_tmp:
            root = Path(raw_tmp)
            source = root / "source.so"
            destination = root / "package" / "library.so"
            private_path = b"/".join(
                (b"", b"Users", b"builder", b"workspace", b"pkg", b"source.cc")
            )
            source.write_bytes(b"prefix " + private_path + b" suffix")

            with self.assertRaisesRegex(PACKAGER.PackageError, "private build path"):
                PACKAGER.copy_public_binary(source, destination)

    def test_clean_regular_binary_is_copied_unchanged(self) -> None:
        with tempfile.TemporaryDirectory() as raw_tmp:
            root = Path(raw_tmp)
            source = root / "source.so"
            destination = root / "package" / "library.so"
            source.write_bytes(b"clean fixture")

            PACKAGER.copy_public_binary(source, destination)

            self.assertEqual(destination.read_bytes(), b"clean fixture")

    def test_macho_is_stripped_before_private_path_scan(self) -> None:
        with tempfile.TemporaryDirectory() as raw_tmp:
            root = Path(raw_tmp)
            source = root / "source.dylib"
            destination = root / "package" / "library.dylib"
            magic = bytes.fromhex("cffaedfe")
            private_path = b"/".join(
                (
                    b"",
                    b"Users",
                    b"builder",
                    b"workspace",
                    b"pkg",
                    b"private-object.o",
                )
            )
            source.write_bytes(magic + b" " + private_path)

            def fake_strip(command: list[str], **_: object) -> None:
                self.assertEqual(command[:3], ["/usr/bin/xcrun", "strip", "-S"])
                Path(command[3]).write_bytes(magic + b" sanitized")

            with mock.patch.object(PACKAGER.shutil, "which", return_value="/usr/bin/xcrun"), mock.patch.object(
                PACKAGER.subprocess, "run", side_effect=fake_strip
            ) as run:
                PACKAGER.copy_public_binary(source, destination)

            run.assert_called_once()
            self.assertEqual(destination.read_bytes(), magic + b" sanitized")


if __name__ == "__main__":
    unittest.main()
