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
    def test_output_rejects_every_repository_product(self) -> None:
        with tempfile.TemporaryDirectory() as raw_tmp:
            repository = Path(raw_tmp) / "publish"
            for product in ("coakka-runtime", "coakka-http-runtime", "coakka-logger", "scripts"):
                with self.subTest(product=product), self.assertRaisesRegex(
                    PACKAGER.PackageError, "publication repository"
                ):
                    PACKAGER.admitted_output_root(repository / product / "output", repository, [])
            with self.assertRaisesRegex(PACKAGER.PackageError, "publication repository"):
                PACKAGER.admitted_output_root(repository.parent, repository, [])

    def test_output_rejects_source_and_input_overlap(self) -> None:
        with tempfile.TemporaryDirectory() as raw_tmp:
            root = Path(raw_tmp)
            for source in (root / "core", root / "platform"):
                for output in (source, source / "output", root):
                    with self.subTest(source=source, output=output), self.assertRaises(
                        PACKAGER.PackageError
                    ):
                        PACKAGER.admitted_output_root(output, root / "publish", [source])

    def test_symlinked_output_cannot_escape_repository_check(self) -> None:
        with tempfile.TemporaryDirectory() as raw_tmp:
            root = Path(raw_tmp)
            repository = root / "publish"
            product = repository / "coakka-http-runtime"
            product.mkdir(parents=True)
            alias = root / "external-name"
            alias.symlink_to(product, target_is_directory=True)
            with self.assertRaisesRegex(PACKAGER.PackageError, "publication repository"):
                PACKAGER.admitted_output_root(alias / "output", repository, [])

    def test_output_accepts_owned_warehouse_or_external_stage(self) -> None:
        with tempfile.TemporaryDirectory() as raw_tmp:
            root = Path(raw_tmp)
            repository = root / "publish"
            sources = [root / "core", root / "platform"]
            self.assertEqual(PACKAGER.admitted_output_root(None, repository, sources), repository)
            self.assertEqual(PACKAGER.admitted_output_root(repository, repository, sources), repository)
            self.assertEqual(PACKAGER.admitted_output_root(root / "stage", repository, sources), root / "stage")

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
