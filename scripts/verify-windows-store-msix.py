#!/usr/bin/env python3
"""Verify the fail-closed Windows Store MSIX source contract."""

from __future__ import annotations

import argparse
import re
from pathlib import Path


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def read(root: Path, relative: str) -> str:
    path = root / relative
    require(path.is_file(), f"required Store MSIX contract file is missing: {relative}")
    return path.read_text(encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-dir", required=True, type=Path)
    parser.add_argument("--project-version", required=True)
    args = parser.parse_args()
    root = args.source_dir.resolve()
    cmake = read(root, "CMakeLists.txt")
    script = read(root, "packaging/windows/package-store-msix.ps1")

    identities = {
        "SDRCAL_STORE_PACKAGE_NAME": "LeeBussy.SDRCalibration",
        "SDRCAL_STORE_PUBLISHER": "CN=66465467-9B9D-4BDE-9CC9-BE392698D910",
        "SDRCAL_STORE_PUBLISHER_DISPLAY_NAME": "Lee Bussy",
        "SDRCAL_STORE_PRODUCT_NAME": "SDR Calibration",
    }
    for variable, value in identities.items():
        require(f'set({variable} "{value}")' in cmake, f"Store identity drift: {variable}")
        require(
            f'set({variable} "{value}" CACHE' not in cmake,
            f"Store identity must not be configure-overridable: {variable}",
        )
    for marker in (
        "add_custom_target(windows-store-msix",
        '-Version "${PROJECT_VERSION}"',
        "package-store-msix.ps1",
    ):
        require(marker in cmake, f"missing Store target contract: {marker}")
    for marker in (
        "refusing to package a dirty source tree",
        "source revision is not synchronized with its upstream",
        "refusing to reuse an existing Store package output directory",
        "must be an exact non-placeholder Partner Center value",
        'Version="${Version}.0"',
        'ProcessorArchitecture="x64"',
        'EntryPoint="Windows.FullTrustApplication"',
        'Alias="sdrcal.exe"',
        '<rescap:Capability Name="runFullTrust" />',
        "Microsoft Store signing not observed",
        "certification_state = 'not submitted'",
        "Qt license disposition failed",
        "unexpected executable or DLL in Store payload",
        "Resize-Png",
    ):
        require(marker in script, f"missing fail-closed Store contract: {marker}")
    for prohibited in (
        'Name="allowElevation"',
        'Name="broadFileSystemAccess"',
        "CertificateThumbprint",
        "Sign-File",
    ):
        require(prohibited not in script, f"prohibited Store MSIX contract present: {prohibited}")
    require(
        re.fullmatch(r"[0-9]+\.[0-9]+\.[0-9]+", args.project_version) is not None,
        "configured project version is malformed",
    )
    print(f"Windows Store MSIX source contracts passed for {args.project_version}.")


if __name__ == "__main__":
    main()
