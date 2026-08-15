# Qt desktop application

The Phase 13 `sdrcal-gui` application provides a Qt 6 Widgets interface to the
same recorded-input calibration and atomic publication service used by the
`sdrcal calibrate` command. It does not enumerate or access an SDR, discover
files or trust material, modify an installed WSJT-X instance, or establish
calibration accuracy.

## macOS package status

The Phase 14.1 macOS DMG is a Developer ID-signed and Apple-notarized package
containing the recorded-input application, production CLI, notices, and
machine-readable dependency inventory. The DMG provides an Applications alias
for copying `sdrcal-gui.app`; command-line installation remains an operator-
managed workflow until clean-host packaging qualification is complete.

The current platform-facing identity is provisional. Phase 14.5 will present
the product as `SDR Calibration` in macOS, Windows, and Linux launchers,
shortcuts, and installed-application listings while retaining `sdrcal-gui` as
an internal implementation name. The macOS bundle will be
`SDR Calibration.app`. Every GUI package must include the same approved,
project-owned application icon in its native platform formats before clean-host
qualification; existing packages without the final name and icon remain
historical package evidence only.

Phase 14.4 requires each new DMG to carry the complete hash-pinned Qt source,
the exact deployed runtime inventory, license disposition, and Qt replacement
instructions. Notarization and license disposition still do not establish
clean-host installation, upgrade/uninstall behavior, live SDR access, device
support, or calibration accuracy.

## Ubuntu package status

Phase 14.3 provides a production gate for an Ubuntu 24.04 x86_64 DEB containing
the recorded-input GUI and CLI. It uses dynamically linked Qt libraries from
Ubuntu system packages and deliberately excludes SoapySDR and vendor modules.
The package is not a live-device build.

The DEB carries the exact dependency clauses, installed package versions, and
Debian copyright dispositions used at construction, plus Qt replacement
instructions. An extracted-payload audit does not establish clean-host
installation, upgrade/removal behavior, repository compatibility, device
support, or calibration accuracy.

## Recorded-input workflow

Select three explicit paths:

1. A versioned recorded-calibration request JSON file.
2. The independent local registry-signature pin file.
3. A new output directory. Existing output destinations are refused.

**Review Request** displays bounded structured request metadata and the exact
request in a read-only view. This is operator review; the production request
parser remains authoritative. **Start Calibration** runs the production
service on a worker while progress is displayed. **Cancel** requests
cooperative cancellation and waits for the workflow to reach a safe stopping
point.

Success loads the fixed published artifacts into read-only tabs:
`profile.json`, `evidence.json`, `summary.json`, and optional `wsjtx.ini`.
**Open Result** can inspect an existing result directory without activating,
editing, signing, replacing, revoking, or deleting its profile. Review files
are limited to 8 MiB each and symbolic-link files are refused.

The trust-pin mechanism is local and bounded, not a production trust store.
Generated profiles remain unsigned until production key management exists.
Any WSJT-X projection is explicitly lossy and does not replace the native SDR
Calibration Profile.
