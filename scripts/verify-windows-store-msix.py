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
    executable_manifest = read(root, "packaging/windows/sdrcal.exe.manifest")
    gui_cmake = read(root, "src/gui/CMakeLists.txt")
    qt_test_wrapper = read(root, "tests/RunQtTest.cmake")
    unit_cmake = read(root, "tests/unit/CMakeLists.txt")
    policy = read(
        root,
        "docs/development/decisions/0025-store-msix-primary-windows-artifact.md",
    )
    release_ledger = read(
        root,
        "docs/development/release/checksums-and-evidence-ledger-template.md",
    )
    submission_readiness = read(
        root,
        "docs/development/windows-store-submission-readiness.md",
    )
    fixture_generator = read(root, "scripts/prepare-store-certification-fixture.py")

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
        '<uap5:ExecutionAlias Alias="sdrcal.exe" />',
        '<rescap:Capability Name="runFullTrust" />',
        "Microsoft Store signing not observed",
        "certification_state = 'not submitted'",
        "Qt license disposition failed",
        "'-DSDRCAL_PLATFORM=Windows'",
        "unexpected executable or DLL in Store payload",
        "Resize-Png",
        "Resolve-MakeAppx",
        "KitsRoot10",
        "x64\\makeappx.exe",
        "PATH and installed x64 Windows SDK checked",
        "(Join-Path $assets 'StoreLogo.png') 50",
        r"<Logo>Assets\StoreLogo.png</Logo>",
        "'StoreLogo.png', 'Square150x150Logo.png'",
    ):
        require(marker in script, f"missing fail-closed Store contract: {marker}")
    for marker in ("true/pm", "PerMonitorV2, PerMonitor"):
        require(marker in executable_manifest, f"missing Windows DPI contract: {marker}")
    for marker in (
        'target_link_options(sdrcal-gui PRIVATE',
        '"/MANIFEST:EMBED"',
        '"/MANIFESTINPUT:${PROJECT_SOURCE_DIR}/packaging/windows/sdrcal.exe.manifest"',
    ):
        require(marker in gui_cmake, f"Windows executable manifest merge is missing: {marker}")
    for marker in (
        "SDRCAL_QT_RUNTIME_DIR",
        'set(ENV{PATH} "${SDRCAL_QT_RUNTIME_DIR};$ENV{PATH}")',
        'set(ENV{QT_QPA_PLATFORM} "offscreen")',
    ):
        require(marker in qt_test_wrapper, f"missing Qt test runtime contract: {marker}")
    for marker in (
        "-DSDRCAL_QT_RUNTIME_DIR=${SDRCAL_CONFIGURED_QT_RUNTIME_DIR}",
        'tests/RunQtTest.cmake',
    ):
        require(marker in unit_cmake, f"missing Qt test harness binding: {marker}")
    for prohibited in (
        'Name="allowElevation"',
        'Name="broadFileSystemAccess"',
        "CertificateThumbprint",
        "Sign-File",
    ):
        require(prohibited not in script, f"prohibited Store MSIX contract present: {prohibited}")
    for marker in (
        "The required Windows 11 x64 artifact for the initial release is a Microsoft",
        "The WiX MSI remains implemented but is not a required release artifact.",
        "locally self-signed MSI may be built and used for bounded development or test",
    ):
        require(marker in policy, f"Windows artifact policy drift: {marker}")
    require(
        "| Windows x64 Store MSIX | Microsoft-certified and Store-signed;" in release_ledger,
        "required release ledger no longer binds the Store MSIX",
    )
    require(
        "| Windows x64 MSI |" not in release_ledger,
        "required release ledger silently restored the MSI",
    )
    for marker in (
        "Partner Center remains draft",
        "No genuine Store screenshot is currently retained.",
        "No package has been uploaded",
        "`runFullTrust` justification",
        "Publishing is held for manual owner action.",
    ):
        require(
            marker in submission_readiness,
            f"Store submission readiness drift: {marker}",
        )
    for marker in (
        "refusing certification fixture preparation from a dirty tree",
        "source revision is not synchronized with its upstream",
        "refusing to reuse an existing fixture output directory",
        "fixture output directory must be outside the source tree",
        "sdrcal runtime version does not match candidate version",
        '"certification_ready": not dirty',
        "synthetic test fixture; no device or accuracy claim",
    ):
        require(marker in fixture_generator, f"Store certification fixture drift: {marker}")
    require(
        re.fullmatch(r"[0-9]+\.[0-9]+\.[0-9]+", args.project_version) is not None,
        "configured project version is malformed",
    )
    print(f"Windows Store MSIX source contracts passed for {args.project_version}.")


if __name__ == "__main__":
    main()
