# Application icon provenance

## Ownership and license

The two committed PNG masters are original SDR Calibration project artwork
supplied and approved by project owner Lee Bussy on 2026-08-15. They do not use
third-party artwork or fonts. The masters, generator, and generated platform
assets are project source and are distributed under the repository's MIT
License.

The committed raster masters are the editable source retained by this project:

- `source/sdr-calibration-master-1024.png` for normal and large
  representations.
- `source/sdr-calibration-small-master-1024.png` with adjustments intended to
  preserve legibility at small launcher sizes.

The macOS path also retains the editable Icon Composer document
`macos/SDRCalibration.icon`. Its project-owned image layer is the opaque square
`macos/SDRCalibration-macOS26-1024.png`, generated from the full master over a
blue enclosure derived from the existing frame colors. This gives macOS 26 a
square layer to mask instead of an irregular legacy silhouette that receives a
gray fallback background. The same PNG is embedded in the Icon Composer
document. The generator updates both copies from one encoded byte stream, and
branding validation fails if they differ.

No vector or layered design source for the underlying illustration is retained;
the Icon Composer document wraps one flattened project-owned raster layer.
Future illustration changes must update the committed masters rather than
hand-edit a native platform output.

## Reproducible generation

Run `scripts/generate-icons.py` from any location with Python and Pillow. The
committed assets were generated with the Python and Pillow versions recorded in
`icon-manifest.json`. The generator produces:

- macOS `macos/SDRCalibration.icns` with standard and Retina representations;
- macOS 26 square input `macos/SDRCalibration-macOS26-1024.png`;
- Windows `windows/SDRCalibration.ico` with 16 through 256 pixel
  representations; and
- Linux hicolor PNGs from 16 through 1024 pixels.

The generator also rewrites `icon-manifest.json`. That manifest records the
conversion tool versions and SHA-256 of both masters, the generator, and every
generated platform asset. `branding_contract_tests` fails if a recorded file is
missing or its hash changes.

The CMake macOS GUI build requires Xcode's `actool`. It compiles
`SDRCalibration.icon` for the macOS 14.0 minimum deployment target, places
`Assets.car` in the application resources for macOS 26, and generates
`SDRCalibration.icns` from the same document for earlier supported releases.
That generated pre-26 rendition is authoritative and need not be pixel-identical
to the older project-generated ICNS. Icon Composer 26.6 created the committed
document. Document changes require visual review in Icon Composer and packaged
review on macOS 26 plus a supported pre-26 release.

The native outputs are generated artifacts. Do not edit them directly. A
generator or Pillow version change requires regeneration, visual review at
small and scaled sizes, a manifest update, and repeat package validation.
