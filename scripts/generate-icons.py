#!/usr/bin/env python3
"""Generate platform icon assets from the committed SDR Calibration masters."""

from __future__ import annotations

import io
import hashlib
import json
import platform
import struct
from collections import deque
from pathlib import Path

try:
    from PIL import Image, __version__ as PILLOW_VERSION
except ImportError as error:
    raise SystemExit(
        "Pillow is required; install it for this Python interpreter and rerun the script"
    ) from error


ROOT = Path(__file__).resolve().parents[1]
FULL_MASTER = ROOT / "assets/icons/source/sdr-calibration-master-1024.png"
SMALL_MASTER = ROOT / "assets/icons/source/sdr-calibration-small-master-1024.png"

# The committed masters are opaque 1254px renders on a nearly black matte. Only
# matte-colored pixels connected to an image edge are eligible for removal.
SOURCE_SIZE = (1254, 1254)
MATTE_BLACK_FLOOR = 4
MATTE_EDGE_LIMIT = 96
EXPECTED_EXTERIOR_FRACTIONS = {
    FULL_MASTER: (0.15, 0.25),
    SMALL_MASTER: (0.30, 0.45),
}
EXPECTED_SOLID_FRAME_SAMPLES = {
    FULL_MASTER: ((627, 60), (60, 627), (627, 1180), (1200, 627)),
    SMALL_MASTER: ((627, 110), (120, 627), (627, 1110), (1130, 627)),
}

LINUX_SIZES = (16, 24, 32, 48, 64, 128, 256, 512, 1024)
ICO_SIZES = (16, 24, 32, 48, 64, 128, 256)
ICNS_PNG_ENTRIES = (
    (b"ic11", 32, "small"),  # 16x16@2x
    (b"ic12", 64, "small"),  # 32x32@2x
    (b"ic07", 128, "full"),
    (b"ic13", 256, "full"),  # 128x128@2x
    (b"ic08", 256, "full"),
    (b"ic14", 512, "full"),  # 256x256@2x
    (b"ic09", 512, "full"),
    (b"ic10", 1024, "full"),  # 512x512@2x
)

GENERATED_ASSETS = (
    *(Path("assets/icons/linux") / f"sdr-calibration-{size}.png" for size in LINUX_SIZES),
    Path("assets/icons/macos/SDRCalibration.icns"),
    Path("assets/icons/windows/SDRCalibration.ico"),
)


def load_master(path: Path) -> Image.Image:
    image = Image.open(path)
    image.load()
    if image.mode != "RGB" or image.size != SOURCE_SIZE:
        raise SystemExit(
            f"source master contract changed: {path} is {image.mode} {image.size}, "
            f"expected RGB {SOURCE_SIZE}"
        )
    return remove_exterior_matte(image, path)


def remove_exterior_matte(image: Image.Image, path: Path) -> Image.Image:
    """Recover alpha only from the black matte connected to the image edges."""
    width, height = image.size
    source = image.load()
    visited = bytearray(width * height)
    exterior: deque[tuple[int, int]] = deque()

    def eligible(x: int, y: int) -> bool:
        return max(source[x, y]) <= MATTE_EDGE_LIMIT

    def enqueue(x: int, y: int) -> None:
        index = y * width + x
        if not visited[index] and eligible(x, y):
            visited[index] = 1
            exterior.append((x, y))

    for x in range(width):
        enqueue(x, 0)
        enqueue(x, height - 1)
    for y in range(height):
        enqueue(0, y)
        enqueue(width - 1, y)

    while exterior:
        x, y = exterior.popleft()
        if x > 0:
            enqueue(x - 1, y)
        if x + 1 < width:
            enqueue(x + 1, y)
        if y > 0:
            enqueue(x, y - 1)
        if y + 1 < height:
            enqueue(x, y + 1)
        if x > 0 and y > 0:
            enqueue(x - 1, y - 1)
        if x + 1 < width and y > 0:
            enqueue(x + 1, y - 1)
        if x > 0 and y + 1 < height:
            enqueue(x - 1, y + 1)
        if x + 1 < width and y + 1 < height:
            enqueue(x + 1, y + 1)

    exterior_count = sum(visited)
    lower, upper = EXPECTED_EXTERIOR_FRACTIONS[path]
    exterior_fraction = exterior_count / (width * height)
    if not lower <= exterior_fraction <= upper:
        raise SystemExit(
            f"exterior mask coverage changed for {path}: "
            f"{exterior_fraction:.3%} is outside {lower:.0%}..{upper:.0%}"
        )

    result = image.convert("RGBA")
    output = result.load()
    for y in range(height):
        for x in range(width):
            if not visited[y * width + x]:
                continue
            red, green, blue = source[x, y]
            value = max(red, green, blue)
            if value <= MATTE_BLACK_FLOOR:
                output[x, y] = (0, 0, 0, 0)
                continue
            alpha = round(
                255
                * (value - MATTE_BLACK_FLOOR)
                / (MATTE_EDGE_LIMIT - MATTE_BLACK_FLOOR)
            )
            # The source edge was composited against black. Un-premultiplying
            # prevents that old matte from becoming a dark fringe on light UI.
            output[x, y] = (
                min(255, round(red * 255 / alpha)),
                min(255, round(green * 255 / alpha)),
                min(255, round(blue * 255 / alpha)),
                alpha,
            )

    corners = (
        (0, 0), (width - 1, 0), (0, height - 1), (width - 1, height - 1)
    )
    if any(result.getpixel(corner)[3] != 0 for corner in corners):
        raise SystemExit(f"exterior mask did not clear every corner: {path}")
    for sample in EXPECTED_SOLID_FRAME_SAMPLES[path]:
        red, green, blue, alpha = result.getpixel(sample)
        if alpha != 255 or blue < 128 or blue <= max(red, green):
            raise SystemExit(
                f"exterior mask damaged the blue frame at {sample}: {path}"
            )
    return result


def resized_png(image: Image.Image, size: int) -> bytes:
    resized = image.resize((size, size), Image.Resampling.LANCZOS)
    clear_detached_alpha(resized)
    output = io.BytesIO()
    resized.save(output, format="PNG", optimize=True)
    return output.getvalue()


def clear_detached_alpha(image: Image.Image) -> None:
    """Remove only resampling remnants disconnected from the icon body."""
    width, height = image.size
    alpha = image.getchannel("A")
    alpha_bytes = alpha.tobytes()
    center = (height // 2) * width + width // 2
    if alpha_bytes[center] == 0:
        raise SystemExit(
            f"resized icon center unexpectedly became transparent: {image.size}"
        )

    connected = bytearray(width * height)
    connected[center] = 1
    pending = deque([center])
    while pending:
        index = pending.popleft()
        y, x = divmod(index, width)
        for near_y in range(max(0, y - 1), min(height, y + 2)):
            for near_x in range(max(0, x - 1), min(width, x + 2)):
                near = near_y * width + near_x
                if alpha_bytes[near] and not connected[near]:
                    connected[near] = 1
                    pending.append(near)

    pixels = image.load()
    for index, value in enumerate(alpha_bytes):
        if value and not connected[index]:
            y, x = divmod(index, width)
            pixels[x, y] = (0, 0, 0, 0)


def master_for_size(
    size: int, full_master: Image.Image, small_master: Image.Image
) -> Image.Image:
    return small_master if size <= 64 else full_master


def write_png_set(full_master: Image.Image, small_master: Image.Image) -> None:
    output_dir = ROOT / "assets/icons/linux"
    output_dir.mkdir(parents=True, exist_ok=True)
    for size in LINUX_SIZES:
        output = output_dir / f"sdr-calibration-{size}.png"
        output.write_bytes(
            resized_png(master_for_size(size, full_master, small_master), size)
        )


def write_ico(full_master: Image.Image, small_master: Image.Image) -> None:
    images = [
        (size, resized_png(master_for_size(size, full_master, small_master), size))
        for size in ICO_SIZES
    ]
    header_size = 6 + 16 * len(images)
    offset = header_size
    directory = []
    payload = []
    for size, png in images:
        encoded_size = 0 if size == 256 else size
        directory.append(
            struct.pack(
                "<BBBBHHII",
                encoded_size,
                encoded_size,
                0,
                0,
                1,
                32,
                len(png),
                offset,
            )
        )
        payload.append(png)
        offset += len(png)

    output = ROOT / "assets/icons/windows/SDRCalibration.ico"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(
        struct.pack("<HHH", 0, 1, len(images)) + b"".join(directory + payload)
    )


def write_icns(full_master: Image.Image, small_master: Image.Image) -> None:
    chunks = []
    for rgb_type, mask_type, size in (
        (b"is32", b"s8mk", 16),
        (b"il32", b"l8mk", 32),
    ):
        icon = Image.open(io.BytesIO(resized_png(small_master, size))).convert("RGBA")
        chunks.append(icns_chunk(rgb_type, icon.convert("RGB").tobytes()))
        chunks.append(icns_chunk(mask_type, icon.getchannel("A").tobytes()))
    for icon_type, size, source in ICNS_PNG_ENTRIES:
        master = small_master if source == "small" else full_master
        chunks.append(icns_chunk(icon_type, resized_png(master, size)))

    body = b"".join(chunks)
    output = ROOT / "assets/icons/macos/SDRCalibration.icns"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(b"icns" + struct.pack(">I", len(body) + 8) + body)


def icns_chunk(icon_type: bytes, payload: bytes) -> bytes:
    return icon_type + struct.pack(">I", len(payload) + 8) + payload


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def write_manifest() -> None:
    tracked_files = (
        Path("assets/icons/source/sdr-calibration-master-1024.png"),
        Path("assets/icons/source/sdr-calibration-small-master-1024.png"),
        Path("scripts/generate-icons.py"),
        *GENERATED_ASSETS,
    )
    manifest = {
        "schema_version": 1,
        "product": "SDR Calibration",
        "asset_license": "MIT",
        "provenance": {
            "project_owner": "Lee Bussy",
            "source": "Original project artwork supplied and approved by the project owner",
            "approval_date": "2026-08-15",
            "third_party_artwork_or_fonts": False,
        },
        "conversion": {
            "generator": "scripts/generate-icons.py",
            "python_version": platform.python_version(),
            "pillow_version": PILLOW_VERSION,
        },
        "files": {
            path.as_posix(): {"sha256": sha256(ROOT / path)}
            for path in tracked_files
        },
    }
    output = ROOT / "assets/icons/icon-manifest.json"
    output.write_text(
        json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )


def main() -> None:
    full_master = load_master(FULL_MASTER)
    small_master = load_master(SMALL_MASTER)
    write_png_set(full_master, small_master)
    write_ico(full_master, small_master)
    write_icns(full_master, small_master)
    write_manifest()


if __name__ == "__main__":
    main()
