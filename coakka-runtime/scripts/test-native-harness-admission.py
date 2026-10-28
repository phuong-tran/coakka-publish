#!/usr/bin/env python3
"""Compile/link SDK-admission fixtures; no runtime implementation is built."""

from __future__ import annotations

from pathlib import Path
import subprocess
import tempfile
import unittest


HARNESS = Path(__file__).resolve().parents[1] / "runtime-test"
DIAGNOSTIC = "rolling harness requires an installed SDK exposing and linking"


class InstalledSdkAdmissionTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory(prefix="coakka-harness-admission-")
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)

    def command(self, *arguments: str) -> subprocess.CompletedProcess[str]:
        return subprocess.run(arguments, capture_output=True, text=True, check=False)

    def sdk(self, name: str, declaration: bool, symbol: bool) -> Path:
        source = self.root / name
        source.mkdir()
        include = source / "include/coakka/v2"
        include.mkdir(parents=True)
        header = "typedef struct { int unused; } coakka_v2_host_handles_t;\n"
        if declaration:
            header += "void coakka_v2_host_handles_close(coakka_v2_host_handles_t *handles);\n"
        (include / "transport.h").write_text(header)
        implementation = (
            '#include "coakka/v2/transport.h"\n'
            "void coakka_v2_host_handles_close(coakka_v2_host_handles_t *handles) { (void)handles; }\n"
            if symbol else "void unrelated_fixture_symbol(void) {}\n"
        )
        (source / "fixture.c").write_text(implementation)
        (source / "Config.cmake").write_text(
            'include("${CMAKE_CURRENT_LIST_DIR}/FixtureTargets.cmake")\n'
            'set(CoAkkaRuntimeNativeV2_PLATFORM "fixture")\n'
            "if(WIN32)\n"
            "  get_target_property(_implib CoAkkaRuntimeNativeV2::runtime_v2 IMPORTED_IMPLIB_RELEASE)\n"
            "  set_target_properties(CoAkkaRuntimeNativeV2::runtime_v2 PROPERTIES IMPORTED_IMPLIB \"${_implib}\")\n"
            "endif()\n"
        )
        (source / "CMakeLists.txt").write_text(
            "cmake_minimum_required(VERSION 3.20)\n"
            "project(InstalledSdkFixture LANGUAGES C)\n"
            "add_library(fixture SHARED fixture.c)\n"
            "set_target_properties(fixture PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF EXPORT_NAME runtime_v2 WINDOWS_EXPORT_ALL_SYMBOLS ON)\n"
            'target_include_directories(fixture PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<INSTALL_INTERFACE:include>")\n'
            "install(TARGETS fixture EXPORT FixtureTargets RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)\n"
            "install(DIRECTORY include/ DESTINATION include)\n"
            "install(EXPORT FixtureTargets NAMESPACE CoAkkaRuntimeNativeV2:: DESTINATION lib/cmake/CoAkkaRuntimeNativeV2)\n"
            "install(FILES Config.cmake DESTINATION lib/cmake/CoAkkaRuntimeNativeV2 RENAME CoAkkaRuntimeNativeV2Config.cmake)\n"
        )
        build = source / "build"
        prefix = source / "installed"
        for arguments in (
            ("cmake", "-S", str(source), "-B", str(build), "-DCMAKE_BUILD_TYPE=Release", f"-DCMAKE_INSTALL_PREFIX={prefix}"),
            ("cmake", "--build", str(build), "--config", "Release", "--parallel", "2"),
            ("cmake", "--install", str(build), "--config", "Release"),
        ):
            result = self.command(*arguments)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        return prefix

    def configure(self, prefix: Path, build: str = "consumer") -> subprocess.CompletedProcess[str]:
        return self.command(
            "cmake", "-S", str(HARNESS), "-B", str(self.root / build),
            f"-DCoAkkaRuntimeNativeV2_DIR={prefix / 'lib/cmake/CoAkkaRuntimeNativeV2'}",
        )

    def test_complete_sdk_is_admitted(self) -> None:
        result = self.configure(self.sdk("complete", True, True))
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_missing_declaration_is_rejected(self) -> None:
        result = self.configure(self.sdk("missing-header-law", False, True))
        self.assertNotEqual(result.returncode, 0)
        self.assertIn(DIAGNOSTIC, result.stdout + result.stderr)

    def test_missing_binary_symbol_is_rejected(self) -> None:
        result = self.configure(self.sdk("missing-symbol", True, False))
        self.assertNotEqual(result.returncode, 0)
        self.assertIn(DIAGNOSTIC, result.stdout + result.stderr)

    def test_cached_success_cannot_admit_a_different_sdk(self) -> None:
        first = self.configure(self.sdk("first", True, True))
        self.assertEqual(first.returncode, 0, first.stdout + first.stderr)
        changed = self.configure(self.sdk("changed", True, False))
        self.assertNotEqual(changed.returncode, 0)
        self.assertIn(DIAGNOSTIC, changed.stdout + changed.stderr)


if __name__ == "__main__":
    unittest.main()
