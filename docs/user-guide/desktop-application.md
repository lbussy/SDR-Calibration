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

Notarization establishes Apple's assessment for one exact package hash. It does
not yet establish clean-host installation, upgrade/uninstall behavior,
binary-license disposition, live SDR access, device support, or calibration
accuracy. Do not redistribute a candidate DMG until the later license and
clean-host gates are recorded as passing.

## Ubuntu package status

Phase 14.3 provides a production gate for an Ubuntu 24.04 x86_64 DEB containing
the recorded-input GUI and CLI. It uses dynamically linked Qt libraries from
Ubuntu system packages and deliberately excludes SoapySDR and vendor modules.
The package is not a live-device build.

An extracted-payload audit establishes package structure, dependency metadata,
runtime linkage, and CLI startup for one exact DEB hash on its build host. It
does not establish clean-host installation, upgrade/removal behavior,
repository compatibility, binary-license disposition, device support, or
calibration accuracy. Do not redistribute a candidate DEB until the later
license and clean-host gates are recorded as passing.

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
