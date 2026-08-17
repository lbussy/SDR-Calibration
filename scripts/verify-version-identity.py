#!/usr/bin/env python3
"""Fail closed when production version identity drifts from CMake."""

from __future__ import annotations

import argparse
import re
import subprocess
from pathlib import Path


SEMVER = re.compile(r"(?<![0-9])([0-9]+\.[0-9]+\.[0-9]+)(?![0-9])")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def read(root: Path, relative: str) -> str:
    path = root / relative
    require(path.is_file(), f"required version contract file is missing: {relative}")
    return path.read_text(encoding="utf-8")


def verify_source(root: Path, expected: str) -> None:
    cmake = read(root, "CMakeLists.txt")
    match = re.search(
        r"project\(\s*SDRCalibration\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)",
        cmake,
        re.MULTILINE,
    )
    require(match is not None, "CMake project version is missing or malformed")
    require(match.group(1) == expected, "configured and source project versions differ")

    version_header = read(root, "src/Version.h.in")
    require(
        'inline constexpr std::string_view kVersion = "@PROJECT_VERSION@";' in version_header,
        "configured C++ version binding is missing",
    )

    entry_points = (
        "src/cli/product_main.cpp",
        "src/cli/main.cpp",
        "src/gui/main.cpp",
    )
    for relative in entry_points:
        content = read(root, relative)
        require('#include "sdrcal/Version.h"' in content, f"version header missing: {relative}")
        require("sdrcal::kVersion" in content, f"configured version unused: {relative}")
        require(not SEMVER.search(content), f"hard-coded semantic version in {relative}")

    source_cmake = read(root, "src/CMakeLists.txt")
    for marker in (
        '"${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in"',
        '"${PROJECT_BINARY_DIR}/generated/sdrcal/Version.h"',
        "add_library(sdrcal_version INTERFACE)",
    ):
        require(marker in source_cmake, f"version interface contract missing: {marker}")

    for relative, targets in (
        ("src/cli/CMakeLists.txt", ("sdrcal", "sdrcal-capture")),
        ("src/gui/CMakeLists.txt", ("sdrcal-gui",)),
    ):
        content = read(root, relative)
        for target in targets:
            pattern = rf"target_link_libraries\(\s*{re.escape(target)}\s+PRIVATE[^\)]*sdrcal_version"
            require(re.search(pattern, content), f"{target} is not bound to sdrcal_version")

    gui_cmake = read(root, "src/gui/CMakeLists.txt")
    for marker in (
        'MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"',
        'MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"',
    ):
        require(marker in gui_cmake, f"macOS version binding missing: {marker}")

    plist = read(root, "packaging/macos/Info.plist.in")
    for marker in (
        "${MACOSX_BUNDLE_SHORT_VERSION_STRING}",
        "${MACOSX_BUNDLE_BUNDLE_VERSION}",
    ):
        require(marker in plist, f"macOS plist version binding missing: {marker}")

    require(
        cmake.count('--version "${PROJECT_VERSION}"') == 3,
        "macOS/Ubuntu/Raspberry Pi package version bindings drifted",
    )
    require('-Version "${PROJECT_VERSION}"' in cmake, "Windows MSI version binding missing")
    require(
        cmake.count('-Version "${PROJECT_VERSION}"') == 2,
        "Windows MSI/MSIX package version bindings drifted",
    )
    require(
        'set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")' in cmake,
        "CPack version binding missing",
    )
    require(
        "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-" in cmake,
        "CPack filename version binding missing",
    )

    spdx = read(root, "packaging/sdrcal.spdx.json.in")
    require(spdx.count("@PROJECT_VERSION@") >= 3, "SPDX project version bindings are incomplete")
    windows = read(root, "packaging/windows/package-msi.ps1")
    require('Version="$Version"' in windows, "WiX package version parameter is missing")
    store = read(root, "packaging/windows/package-store-msix.ps1")
    require('Version="${Version}.0"' in store, "MSIX manifest version parameter is missing")
    for relative in (
        "packaging/macos/package-dmg.sh",
        "packaging/ubuntu/package-deb.sh",
        "packaging/raspberry-pi/package-deb.sh",
    ):
        require("--version" in read(root, relative), f"package version argument missing: {relative}")


def verify_runtime(executable: Path, expected: str) -> None:
    result = subprocess.run(
        [str(executable), "--version"],
        check=False,
        capture_output=True,
        text=True,
        timeout=10,
    )
    require(result.returncode == 0, f"sdrcal --version failed: {result.returncode}")
    require(not result.stderr, f"sdrcal --version wrote diagnostics: {result.stderr!r}")
    require(result.stdout == f"sdrcal {expected}\n", "sdrcal --version identity mismatch")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-dir", required=True, type=Path)
    parser.add_argument("--project-version", required=True)
    parser.add_argument("--sdrcal-executable", type=Path)
    args = parser.parse_args()

    verify_source(args.source_dir.resolve(), args.project_version)
    if args.sdrcal_executable is not None:
        verify_runtime(args.sdrcal_executable.resolve(), args.project_version)
    print(f"Version identity contracts passed for {args.project_version}.")


if __name__ == "__main__":
    main()
