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

No separate vector or layered design source is currently retained. Future
visual changes must update the committed masters rather than hand-edit a native
platform output.

## Reproducible generation

Run `scripts/generate-icons.py` from any location with Python and Pillow. The
committed assets were generated with the Python and Pillow versions recorded in
`icon-manifest.json`. The generator produces:

- macOS `macos/SDRCalibration.icns` with standard and Retina representations;
- Windows `windows/SDRCalibration.ico` with 16 through 256 pixel
  representations; and
- Linux hicolor PNGs from 16 through 1024 pixels.

The generator also rewrites `icon-manifest.json`. That manifest records the
conversion tool versions and SHA-256 of both masters, the generator, and every
generated platform asset. `branding_contract_tests` fails if a recorded file is
missing or its hash changes.

The native outputs are generated artifacts. Do not edit them directly. A
generator or Pillow version change requires regeneration, visual review at
small and scaled sizes, a manifest update, and repeat package validation.
