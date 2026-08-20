#!/usr/bin/env python3
"""Verify cross-platform product identity and committed icon evidence."""

from __future__ import annotations

import argparse
import hashlib
import json
import struct
import xml.etree.ElementTree as ET
import zlib
from pathlib import Path


PRODUCT_NAME = "SDR Calibration"
LINUX_SIZES = (16, 24, 32, 48, 64, 128, 256, 512, 1024)
ICO_SIZES = (16, 24, 32, 48, 64, 128, 256)


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def read_text(root: Path, relative: str) -> str:
    path = root / relative
    require(path.is_file(), f"required branding contract file is missing: {relative}")
    return path.read_text(encoding="utf-8")


def verify_manifest(root: Path) -> None:
    manifest_path = root / "assets/icons/icon-manifest.json"
    require(manifest_path.is_file(), "icon hash manifest is missing")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    require(manifest.get("schema_version") == 1, "unexpected icon manifest schema")
    require(manifest.get("product") == PRODUCT_NAME, "icon manifest product mismatch")
    require(manifest.get("asset_license") == "MIT", "icon manifest license mismatch")
    provenance = manifest.get("provenance", {})
    require(provenance.get("project_owner") == "Lee Bussy", "project owner is not recorded")
    require(provenance.get("approval_date") == "2026-08-15", "approval date is not recorded")
    require(
        provenance.get("third_party_artwork_or_fonts") is False,
        "third-party artwork/font disposition is not fail-closed",
    )
    conversion = manifest.get("conversion", {})
    require(conversion.get("generator") == "scripts/generate-icons.py", "generator mismatch")
    require(conversion.get("python_version"), "Python conversion version is missing")
    require(conversion.get("pillow_version"), "Pillow conversion version is missing")

    expected = {
        "assets/icons/source/sdr-calibration-master-1024.png",
        "assets/icons/source/sdr-calibration-small-master-1024.png",
        "assets/icons/macos/SDRCalibration-macOS26-1024.png",
        "assets/icons/macos/SDRCalibration.icon/icon.json",
        "assets/icons/macos/SDRCalibration.icon/Assets/SDRCalibration-macOS26-1024.png",
        "scripts/generate-icons.py",
        "assets/icons/macos/SDRCalibration.icns",
        "assets/icons/windows/SDRCalibration.ico",
        *(f"assets/icons/linux/sdr-calibration-{size}.png" for size in LINUX_SIZES),
    }
    files = manifest.get("files", {})
    require(set(files) == expected, "icon manifest file set is incomplete or unexpected")
    for relative, record in files.items():
        path = root / relative
        require(path.is_file(), f"manifested icon file is missing: {relative}")
        require(
            sha256(path) == record.get("sha256"),
            f"icon hash manifest drift: {relative}",
        )
    modern = manifest.get("macos_modern", {})
    require(
        modern.get("document") == "assets/icons/macos/SDRCalibration.icon",
        "Icon Composer source document is not recorded",
    )
    require(modern.get("compiler") == "Xcode actool", "modern icon compiler mismatch")
    require(
        modern.get("minimum_deployment_target") == "14.0",
        "modern icon deployment target mismatch",
    )


def verify_png(path: Path, expected_size: int) -> None:
    data = path.read_bytes()
    require(data.startswith(b"\x89PNG\r\n\x1a\n"), f"not a PNG: {path}")
    require(len(data) >= 24 and data[12:16] == b"IHDR", f"PNG lacks IHDR: {path}")
    width, height = struct.unpack(">II", data[16:24])
    require(
        (width, height) == (expected_size, expected_size),
        f"PNG dimensions are {width}x{height}, expected {expected_size}: {path}",
    )


def verify_modern_macos_artwork(root: Path) -> None:
    generated = root / "assets/icons/macos/SDRCalibration-macOS26-1024.png"
    embedded = (
        root
        / "assets/icons/macos/SDRCalibration.icon/Assets/SDRCalibration-macOS26-1024.png"
    )
    verify_png(generated, 1024)
    verify_png(embedded, 1024)
    require(generated.read_bytes() == embedded.read_bytes(), "Icon Composer artwork drift")

    data = generated.read_bytes()
    require(data[24] == 8 and data[25] == 2, "modern icon must be opaque 8-bit RGB")
    compressed = bytearray()
    offset = 8
    while offset < len(data):
        length = struct.unpack(">I", data[offset : offset + 4])[0]
        kind = data[offset + 4 : offset + 8]
        if kind == b"IDAT":
            compressed.extend(data[offset + 8 : offset + 8 + length])
        offset += length + 12
    raw = zlib.decompress(compressed)
    stride = 1024 * 3
    previous = bytearray(stride)
    rows = []
    cursor = 0
    for _ in range(1024):
        filter_kind = raw[cursor]
        cursor += 1
        scanline = bytearray(raw[cursor : cursor + stride])
        cursor += stride
        reconstructed = bytearray(stride)
        for index, value in enumerate(scanline):
            left = reconstructed[index - 3] if index >= 3 else 0
            above = previous[index]
            upper_left = previous[index - 3] if index >= 3 else 0
            if filter_kind == 0:
                predictor = 0
            elif filter_kind == 1:
                predictor = left
            elif filter_kind == 2:
                predictor = above
            elif filter_kind == 3:
                predictor = (left + above) // 2
            elif filter_kind == 4:
                estimate = left + above - upper_left
                distances = (
                    abs(estimate - left),
                    abs(estimate - above),
                    abs(estimate - upper_left),
                )
                predictor = (left, above, upper_left)[distances.index(min(distances))]
            else:
                raise SystemExit(f"unsupported PNG filter {filter_kind}: {generated}")
            reconstructed[index] = (value + predictor) & 0xFF
        rows.append(reconstructed)
        previous = reconstructed

    for x, y in ((0, 0), (1023, 0), (0, 1023), (1023, 1023)):
        red, green, blue = rows[y][x * 3 : x * 3 + 3]
        require(
            blue > red and blue >= green and blue - red >= 60,
            f"modern icon corner is not blue-dominant: {(x, y)} {(red, green, blue)}",
        )
    left, top, right, bottom = (70, 70, 954, 954)
    radius = 145
    background_top = (74, 207, 252)
    background_bottom = background_top
    for y, row in enumerate(rows):
        mix = y / 1023
        expected_background = tuple(
            round(upper * (1.0 - mix) + lower * mix)
            for upper, lower in zip(background_top, background_bottom, strict=True)
        )
        for x in range(1024):
            nearest_x = min(max(x, left + radius), right - radius)
            nearest_y = min(max(y, top + radius), bottom - radius)
            inside_face = (
                left <= x <= right
                and top <= y <= bottom
                and (x - nearest_x) ** 2 + (y - nearest_y) ** 2 <= radius**2
            )
            if not inside_face:
                actual = tuple(row[x * 3 : x * 3 + 3])
                require(
                    actual == expected_background,
                    f"legacy enclosure escaped the modern face at {(x, y)}: {actual}",
                )


def verify_native_icons(root: Path) -> None:
    verify_modern_macos_artwork(root)
    for size in LINUX_SIZES:
        verify_png(root / f"assets/icons/linux/sdr-calibration-{size}.png", size)

    ico = (root / "assets/icons/windows/SDRCalibration.ico").read_bytes()
    require(len(ico) >= 6, "Windows icon is truncated")
    reserved, kind, count = struct.unpack("<HHH", ico[:6])
    require((reserved, kind, count) == (0, 1, len(ICO_SIZES)), "Windows icon header mismatch")
    sizes = []
    for index in range(count):
        offset = 6 + index * 16
        require(len(ico) >= offset + 16, "Windows icon directory is truncated")
        width, height = ico[offset], ico[offset + 1]
        sizes.append((256 if width == 0 else width, 256 if height == 0 else height))
    require(sizes == [(size, size) for size in ICO_SIZES], "Windows icon sizes mismatch")

    icns = (root / "assets/icons/macos/SDRCalibration.icns").read_bytes()
    require(len(icns) >= 8 and icns[:4] == b"icns", "macOS icon header mismatch")
    require(struct.unpack(">I", icns[4:8])[0] == len(icns), "macOS icon length mismatch")
    required_chunks = {b"is32", b"s8mk", b"il32", b"l8mk", b"ic11", b"ic12", b"ic07", b"ic13", b"ic08", b"ic14", b"ic09", b"ic10"}
    found = set()
    offset = 8
    while offset < len(icns):
        require(offset + 8 <= len(icns), "macOS icon chunk header is truncated")
        kind = icns[offset : offset + 4]
        length = struct.unpack(">I", icns[offset + 4 : offset + 8])[0]
        require(length >= 8 and offset + length <= len(icns), "macOS icon chunk is invalid")
        found.add(kind)
        offset += length
    require(offset == len(icns) and required_chunks <= found, "macOS representations are incomplete")


def verify_linux(root: Path) -> None:
    desktop = read_text(root, "packaging/ubuntu/sdrcal.desktop")
    entries = {}
    for line in desktop.splitlines():
        if line and not line.startswith("[") and "=" in line:
            key, value = line.split("=", 1)
            entries[key] = value
    expected = {
        "Type": "Application",
        "Name": PRODUCT_NAME,
        "Exec": "sdrcal-gui",
        "Icon": "sdr-calibration",
        "Terminal": "false",
    }
    for key, value in expected.items():
        require(entries.get(key) == value, f"Linux desktop entry mismatch: {key}")
    cmake = read_text(root, "CMakeLists.txt")
    require(
        '"${PROJECT_SOURCE_DIR}/assets/icons/linux/sdr-calibration-${icon_size}.png"'
        in cmake,
        "Linux icon install source pattern is missing",
    )
    require(
        '"${CMAKE_INSTALL_DATADIR}/icons/hicolor/${icon_size}x${icon_size}/apps"'
        in cmake,
        "Linux hicolor install destination pattern is missing",
    )
    for size in LINUX_SIZES:
        require(
            (root / f"assets/icons/linux/sdr-calibration-{size}.png").is_file(),
            f"Linux icon install source is missing: {size}",
        )
    require('RENAME "sdr-calibration.png"' in cmake, "Linux icon-theme name mismatch")


def verify_macos(root: Path) -> None:
    gui_cmake = read_text(root, "src/gui/CMakeLists.txt")
    require('DESTINATION "SDR Calibration.app"' in gui_cmake, "macOS install bundle name mismatch")
    require("TARGET_BUNDLE_DIR:sdrcal-gui" in gui_cmake, "macOS build target identity drift")
    plist = read_text(root, "packaging/macos/Info.plist.in")
    for key, value in (
        ("CFBundleExecutable", "${MACOSX_BUNDLE_EXECUTABLE_NAME}"),
        ("CFBundleName", "${MACOSX_BUNDLE_BUNDLE_NAME}"),
        ("CFBundleIconFile", "${MACOSX_BUNDLE_ICON_FILE}"),
        ("CFBundleIconName", "SDRCalibration"),
    ):
        require(f"<key>{key}</key>" in plist and f"<string>{value}</string>" in plist, f"macOS plist binding mismatch: {key}")

    package_files = (
        "packaging/audit-package.cmake",
        "packaging/macos/package-dmg.sh",
        "packaging/licenses/qt-library-replacement.md",
    )
    for relative in package_files:
        content = read_text(root, relative)
        old_path_count = content.count("sdrcal-gui.app")
        if relative == "packaging/audit-package.cmake":
            require(
                old_path_count == 1
                and 'if(EXISTS "${SDRCAL_STAGE_DIR}/sdrcal-gui.app")' in content,
                "macOS audit does not contain exactly one fail-closed stale-name check",
            )
        else:
            require(old_path_count == 0, f"stale macOS bundle path: {relative}")
        require("SDR Calibration.app" in content, f"new macOS bundle path missing: {relative}")
    dmg = read_text(root, "packaging/macos/package-dmg.sh")
    require('gui_executable="$app/Contents/MacOS/sdrcal-gui"' in dmg, "internal macOS executable drift")
    require(
        "configured Qt package is outside the physical Qt prefix" in dmg,
        "mixed-prefix Qt package preflight is missing",
    )
    for marker in (
        "assets/icons/macos/SDRCalibration.icon",
        "--app-icon SDRCalibration",
        "--standalone-icon-behavior all",
        "Assets.car",
    ):
        require(marker in gui_cmake, f"modern macOS icon build contract missing: {marker}")
    audit = read_text(root, "packaging/audit-package.cmake")
    require(
        "scripts/verify-macos-icon-bundle.py" in audit,
        "staged actool-generated ICNS validation is missing",
    )


def verify_windows(root: Path) -> None:
    script = read_text(root, "packaging/windows/package-msi.ps1")
    gui_cmake = read_text(root, "src/gui/CMakeLists.txt")
    resource = read_text(root, "packaging/windows/sdrcal.rc.in")
    require(
        "assets/icons/windows/SDRCalibration.ico" in gui_cmake
        and 'target_sources(sdrcal-gui PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/sdrcal.rc")'
        in gui_cmake,
        "Windows icon resource is not bound to the GUI target",
    )
    require(
        resource.strip() == 'IDI_ICON1 ICON "@SDRCAL_WINDOWS_ICON_FILE@"',
        "Windows resource icon binding mismatch",
    )
    required = (
        '<Package Name="SDR Calibration"',
        '<Directory Id="INSTALLFOLDER" Name="SDR Calibration" />',
        '<Feature Id="Main" Title="SDR Calibration"',
        'Id="SDRCalibrationStartMenuShortcutLink"',
        'Name="SDR Calibration"',
        'Target="[INSTALLFOLDER]bin\\sdrcal-gui.exe"',
        'Icon="SDRCalibration.ico"',
        '<RemoveFolder Id="RemoveSDRCalibrationProgramMenuFolder" On="uninstall" />',
        '<RegistryValue Root="HKLM" Key="Software\\SDR Calibration"',
        'KeyPath="yes"',
        '<Icon Id="SDRCalibration.ico" SourceFile="$wixIcon" />',
        '<Property Id="ARPPRODUCTICON" Value="SDRCalibration.ico" />',
        '<ComponentGroupRef Id="PayloadComponents" />',
        '<ComponentRef Id="SDRCalibrationStartMenuShortcut" />',
    )
    for marker in required:
        require(marker in script, f"Windows installed-app/shortcut contract missing: {marker}")

    heredoc_start = script.find('@"\n<Wix ')
    heredoc_end = script.find('\n"@ | Set-Content', heredoc_start)
    require(heredoc_start >= 0 and heredoc_end > heredoc_start, "WiX XML template is missing")
    wix_xml = script[heredoc_start + 3 : heredoc_end]
    wix_xml = (
        wix_xml.replace("$Version", "0.1.0")
        .replace("$wixIcon", r"C:\\SDRCalibration.ico")
        .replace(
            "$wixPayloadComponents",
            '      <Component Id="PayloadComponent_test" Guid="*">'
            '<File Id="PayloadFile_test" Source="C:\\stage\\LICENSE" />'
            '</Component>',
        )
    )
    try:
        wix_root = ET.fromstring(wix_xml)
    except ET.ParseError as error:
        raise SystemExit(f"generated WiX XML is not well formed: {error}") from error
    namespace = {"w": "http://wixtoolset.org/schemas/v4/wxs"}
    shortcut = wix_root.find(".//w:Shortcut", namespace)
    require(shortcut is not None, "generated WiX XML lacks the Start menu shortcut")
    require(
        shortcut.attrib.get("Name") == PRODUCT_NAME
        and shortcut.attrib.get("Target") == r"[INSTALLFOLDER]bin\sdrcal-gui.exe"
        and shortcut.attrib.get("Icon") == "SDRCalibration.ico",
        "generated WiX shortcut identity mismatch",
    )


def verify_application_identity(root: Path) -> None:
    main = read_text(root, "src/gui/main.cpp")
    window = read_text(root, "src/gui/MainWindow.cpp")
    require('setApplicationName("SDR Calibration")' in main, "Qt application name mismatch")
    require('setWindowTitle(tr("SDR Calibration' in window, "Qt window title mismatch")


def verify_claim_language(root: Path) -> None:
    package_metadata = (
        "CMakeLists.txt",
        "packaging/raspberry-pi/package-deb.sh",
        "packaging/ubuntu/package-deb.sh",
        "packaging/ubuntu/sdrcal.desktop",
        "packaging/windows/package-msi.ps1",
        "packaging/windows/package-store-msix.ps1",
    )
    for relative in package_metadata:
        content = read_text(root, relative)
        lowered = content.lower()
        require(
            "evidence-bounded" in lowered,
            f"package metadata lacks evidence-bounded claim language: {relative}",
        )
        require(
            "traceable per-device" not in lowered
            and "create traceable" not in lowered,
            f"package metadata contains an unconditional traceability claim: {relative}",
        )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-dir", required=True, type=Path)
    args = parser.parse_args()
    root = args.source_dir.resolve()
    verify_manifest(root)
    verify_native_icons(root)
    verify_application_identity(root)
    verify_macos(root)
    verify_windows(root)
    verify_linux(root)
    verify_claim_language(root)
    print("Cross-platform branding and icon contracts passed.")


if __name__ == "__main__":
    main()
