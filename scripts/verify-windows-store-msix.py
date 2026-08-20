#!/usr/bin/env python3
"""Verify the fail-closed Windows Store MSIX source contract."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import zipfile
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
    build_script = read(root, "packaging/windows/build-store-msix.ps1")
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
    owner_decisions = read(root, "docs/development/windows-store-owner-decisions.md")
    listing_prompt = read(
        root, "docs/development/windows-store-listing-text-execution-prompt.md"
    )
    screenshot_prompt = read(
        root, "docs/development/windows-store-screenshot-upload-execution-prompt.md"
    )
    package_upload_prompt = read(
        root, "docs/development/windows-store-package-upload-execution-prompt.md"
    )
    partner_center_inspection = read(
        root,
        "evidence/windows-store/2026-08-17-partner-center-readonly/README.md",
    )
    fixture_generator = read(root, "scripts/prepare-store-certification-fixture.py")
    lifecycle = read(root, "packaging/windows/qualify-store-msix.ps1")
    screenshot_dir = root / "evidence/windows-store/2026-08-17-0.1.1-screenshots"
    screenshot_manifest_path = (
        root / "evidence/windows-store/2026-08-17-0.1.1-screenshots/manifest.json"
    )
    require(screenshot_manifest_path.is_file(), "Store screenshot manifest is missing")
    screenshot_manifest = json.loads(
        screenshot_manifest_path.read_text(encoding="utf-8-sig")
    )
    fixture_archive = (
        root / "evidence/windows-store/2026-08-17-0.1.1-certification-fixture"
        / "SDRCalibration-0.1.1-Store-Certification-Fixture.zip"
    )
    require(fixture_archive.is_file(), "Store certification fixture attachment is missing")
    require(
        hashlib.sha256(fixture_archive.read_bytes()).hexdigest()
        == "f11547cdbbedba715074d55413026a86ac5209597aeb42f8712e7527c7b6ff51",
        "Store certification fixture attachment hash mismatch",
    )
    expected_fixture_members = {
        "README.txt", "SHA256SUMS", "first.cf32", "manifest.json",
        "request.json", "second.cf32", "trust.json",
    }
    with zipfile.ZipFile(fixture_archive) as fixture_zip:
        require(set(fixture_zip.namelist()) == expected_fixture_members,
                "Store certification fixture attachment member drift")
        fixture_metadata = json.loads(fixture_zip.read("manifest.json"))
        fixture_request = json.loads(fixture_zip.read("request.json"))
        require(fixture_metadata["certification_ready"] is True,
                "Store certification fixture is not certification-ready")
        require(fixture_metadata["classification"] ==
                "synthetic test fixture; no device or accuracy claim",
                "Store certification fixture classification drift")
        require(fixture_request["software_version"] == "0.1.1",
                "Store certification fixture version drift")
        expected_payload_hashes = {
            "first.cf32": "8e02d0128ec060dbcf344be3f3821fb5045327ee9b927d18d00de5104f59a066",
            "second.cf32": "f830dbb28cba5829c91dfa0599f293a04c29eacdbef35eabd80e262d04cec076",
            "request.json": "edc391ee01bcbcdba15241244fb8502397957ec021650742a5fa9ad8c090bf95",
            "trust.json": "85d6dc934d9a6744f6f176a0626dc3aebe793bf152e19c1482dafc7c92b7e386",
        }
        for name, expected_hash in expected_payload_hashes.items():
            require(hashlib.sha256(fixture_zip.read(name)).hexdigest() == expected_hash,
                    f"Store certification fixture payload hash mismatch: {name}")

    require(screenshot_manifest["candidate_version"] == "0.1.1",
            "Store screenshot version binding drift")
    require(screenshot_manifest["candidate_revision"] ==
            "957fbeb204177c9ba2a1582e936476244b201b9d",
            "Store screenshot revision binding drift")
    require(screenshot_manifest["msix_sha256"] ==
            "1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1",
            "Store screenshot MSIX binding drift")
    require(len(screenshot_manifest["screenshots"]) == 4,
            "Store screenshot evidence must contain exactly four images")
    for record in screenshot_manifest["screenshots"]:
        image_path = screenshot_dir / record["file"]
        require(image_path.is_file(), f"Store screenshot is missing: {record['file']}")
        require(hashlib.sha256(image_path.read_bytes()).hexdigest() == record["sha256"],
                f"Store screenshot hash mismatch: {record['file']}")
        require(record["width"] >= 1366 and record["height"] >= 768,
                f"Store screenshot dimensions are too small: {record['file']}")
        require("synthetic inputs and neutral paths only" in record["privacy_review"],
                f"Store screenshot privacy review is missing: {record['file']}")

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
        '<Resource Language="en-us" />',
        "'Assets\\StoreLogo.png'",
        "unpacked manifest publisher display name does not match",
        "unpacked manifest must declare exactly the en-us package resource language",
        "unpacked manifest must contain exactly the expected package and ",
        "Expected = 'Assets\\Square150x150Logo.png'; Size = 150",
        "Expected = 'Assets\\Square44x44Logo.png'; Size = 44",
        "unpacked manifest $($logo.Label) must reference exactly $($logo.Expected)",
        "does not resolve to a PNG image",
        '"$($logo.Size)x$($logo.Size)"',
    ):
        require(marker in script, f"missing fail-closed Store contract: {marker}")
    for marker in (
        "Resolve-VsWhere",
        "Microsoft Visual Studio\\Installer\\vswhere.exe",
        "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
        "Common7\\Tools\\VsDevCmd.bat",
        "'\"' + $devCmd +",
        "-arch=x64 -host_arch=x64",
        "Visual Studio environment setup did not expose cl.exe",
        "Get-FileHash -LiteralPath $binding.Value -Algorithm SHA256",
        "'windows-store-msix'",
    ):
        require(marker in build_script, f"missing Store build environment contract: {marker}")
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
        "refusing lifecycle qualification from a dirty source tree",
        "source revision is not synchronized with its upstream",
        "MSIX SHA-256 does not match the expected artifact",
        "package construction evidence does not match the expected revision and hash",
        "artifact_source_revision = $ArtifactSourceRevision.ToLowerInvariant()",
        "harness_source_revision = $sourceRevision",
        "refusing to reuse an existing lifecycle evidence directory",
        "New-SelfSignedCertificate",
        "KeyExportPolicy NonExportable",
        "MSIX lifecycle qualification must run from an elevated desktop session",
        "MSIX lifecycle qualification must run in the signed-in desktop session",
        "Cert:\\LocalMachine\\TrustedPeople",
        "Add-AppxPackage",
        "signed package does not identify the temporary signing certificate",
        "development signature verification failed",
        "gui_first_launch = 'passed'",
        "microsoft_store_signing_observed = $false",
        "post-lifecycle cleanup audit failed",
    ):
        require(marker in lifecycle, f"Store lifecycle harness drift: {marker}")
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
        "Status: Store listing complete; first package rejected; replacement pending",
        "Four genuine Store screenshot candidates are retained",
        "evidence/windows-store/2026-08-17-0.1.1-screenshots/",
        "SDRCalibration-0.1.1-Store-Certification-Fixture.zip",
        "f11547cdbbedba715074d55413026a86ac5209597aeb42f8712e7527c7b6ff51",
        "No package has been uploaded",
        "`runFullTrust` justification",
        "Publishing is held for manual owner action.",
        "Approved and saved",
    ):
        require(
            marker in submission_readiness,
            f"Store submission readiness drift: {marker}",
        )
    require("TBD-BLOCKING" not in submission_readiness,
            "Store submission readiness still contains a blocking placeholder")
    for marker in (
        "Reconcile and save the already-reviewed English (United States)",
        "Do not open a file picker or upload screenshots",
        "Do not select **Submit for certification**",
        "Adversarial review",
    ):
        require(marker in listing_prompt, f"Store listing prompt drift: {marker}")
    for marker in (
        "Upload only the four retained, privacy-reviewed",
        "Refuse any screenshot whose current hash or dimensions differ",
        "Do not select **Submit for certification**",
        "Adversarial review",
    ):
        require(marker in screenshot_prompt, f"Store screenshot prompt drift: {marker}")
    for marker in (
        "Upload the single owner-approved exact MSIX",
        "Immediately before transfer, recompute the SHA-256",
        "Do not select **Submit for certification**",
        "Adversarial review",
    ):
        require(marker in package_upload_prompt, f"Store package-upload prompt drift: {marker}")
    for marker in (
        "Status: **Owner approved and saved; upload remains separately authorized**",
        "Preparing or committing a proposed value is not approval of that",
        "Manual publication hold; no automatic publication",
        "The publishing default is automatic after certification",
        "All device-family boxes are initially unchecked",
        "https://github.com/lbussy/SDR-Calibration/issues",
        "Approved final decision",
        "automatic future-device-family availability disabled",
        "Record me as the",
        "- Approval date (UTC): **2026-08-17**",
        "- Approving owner: **Lee Bussy**",
        "selection or upload remains a separate authorized slice",
    ):
        require(marker in owner_decisions, f"Store owner-decision gate drift: {marker}")
    require(owner_decisions.count("- [ ]") == 0,
            "Store owner-decision packet must retain no pending attestations")
    require(owner_decisions.count("- [x]") == 8,
            "Store owner-decision packet must retain eight explicit approvals")
    for marker in (
        "product-list status was `Not started`",
        "application overview status was `In draft`",
        "Current packages contained no package entries",
        "No submission was started, package uploaded, field edited",
        "No pricing, availability, properties,",
        "therefore remain unreconciled pending separate",
        "Partner Center created `Submission 1`",
        "Submission options defaulted to publishing as soon as certification passes",
        "No form value was entered or selected, no Save action was used",
        "Owner-authorized saved draft state",
        "Exact pre-upload inspection",
        "SDR-Calibration-Harness-70ff94c",
        "Owner-approved final gate reconciliation",
        "Owner-approved English listing text",
        "Exact-candidate Desktop screenshot upload",
        "First package acceptance result",
        "Partner Center rejected it with acceptance errors",
        "owner subsequently authorized deletion of that single rejected package",
        "Store listings `Complete`, Packages `Incomplete`",
        "no package was selected or uploaded",
        "no screenshot, image, trailer, package, fixture, or other attachment was",
        "Four same-named `0.1.1` MSIX files were found",
    ):
        require(marker in partner_center_inspection, f"Partner Center inspection drift: {marker}")
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
