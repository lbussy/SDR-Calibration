#!/usr/bin/env python3
"""Verify the actool-generated icon resources in a staged macOS app."""

from __future__ import annotations

import argparse
import plistlib
import struct
from pathlib import Path


EXPECTED_PNG_CHUNKS = {
    b"ic11": (32, 32),
    b"ic12": (64, 64),
    b"ic07": (128, 128),
    b"ic13": (256, 256),
    b"ic08": (256, 256),
    b"ic14": (512, 512),
    b"ic09": (512, 512),
    b"ic10": (1024, 1024),
}
EXPECTED_RAW_CHUNKS = {b"ic04": (16, 16), b"ic05": (32, 32)}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def verify_icns(path: Path) -> None:
    data = path.read_bytes()
    require(len(data) >= 8 and data[:4] == b"icns", "generated ICNS header mismatch")
    require(struct.unpack(">I", data[4:8])[0] == len(data), "generated ICNS length mismatch")
    found = set()
    offset = 8
    while offset < len(data):
        require(offset + 8 <= len(data), "generated ICNS chunk header is truncated")
        kind = data[offset : offset + 4]
        length = struct.unpack(">I", data[offset + 4 : offset + 8])[0]
        require(length >= 8 and offset + length <= len(data), "generated ICNS chunk is invalid")
        require(kind not in found, f"generated ICNS repeats chunk {kind!r}")
        found.add(kind)
        payload = data[offset + 8 : offset + length]
        if kind in EXPECTED_PNG_CHUNKS:
            require(payload.startswith(b"\x89PNG\r\n\x1a\n"), f"{kind!r} is not PNG-backed")
            require(len(payload) >= 24 and payload[12:16] == b"IHDR", f"{kind!r} lacks IHDR")
            dimensions = struct.unpack(">II", payload[16:24])
            require(dimensions == EXPECTED_PNG_CHUNKS[kind], f"{kind!r} dimensions mismatch")
        elif kind in EXPECTED_RAW_CHUNKS:
            require(payload.startswith(b"ARGB"), f"{kind!r} lacks its ARGB header")
            width, height = EXPECTED_RAW_CHUNKS[kind]
            require(
                len(payload) > width * height,
                f"{kind!r} is too short for its {width}x{height} representation",
            )
        offset += length
    require(offset == len(data), "generated ICNS has trailing data")
    required = set(EXPECTED_PNG_CHUNKS) | set(EXPECTED_RAW_CHUNKS)
    require(found == required, f"generated ICNS representations mismatch: {sorted(found)}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--app-bundle", required=True, type=Path)
    args = parser.parse_args()
    resources = args.app_bundle / "Contents/Resources"
    verify_icns(resources / "SDRCalibration.icns")
    plist = plistlib.loads((args.app_bundle / "Contents/Info.plist").read_bytes())
    require(plist.get("CFBundleIconFile") == "SDRCalibration.icns", "CFBundleIconFile mismatch")
    require(plist.get("CFBundleIconName") == "SDRCalibration", "CFBundleIconName mismatch")
    require((resources / "Assets.car").stat().st_size > 0, "Assets.car is empty")
    print("Generated macOS Icon Composer bundle resources passed.")


if __name__ == "__main__":
    main()
