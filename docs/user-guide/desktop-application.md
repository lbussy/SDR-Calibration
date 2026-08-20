# Qt desktop application

The `sdrcal-gui` application provides a Qt 6 Widgets interface to the same
recorded- and live-input calibration and atomic publication service used by the
`sdrcal calibrate` command. It does not discover files or trust material,
modify an installed WSJT-X instance, or establish calibration accuracy. A live
request can enumerate, configure, and receive from its explicitly selected SDR
only after review and operator confirmation.

The initial desktop targets are macOS 14.0 or later on Apple Silicon and Windows
11 x64. Raspberry Pi OS is not a desktop target; its initial scope is the
recorded-input CLI only. Ubuntu GUI implementation remains in the repository
but is unsupported and currently unvalidated. The exact indexed macOS and
Windows package hashes have owner-confirmed clean-host lifecycle passes.

## macOS package status

The current macOS DMG is a Developer ID-signed and Apple-notarized package
containing the recorded-input application, production CLI, notices, and
machine-readable dependency inventory. The DMG provides an Applications alias
for copying `SDR Calibration.app`; command-line installation remains an
operator-managed workflow.

The package definitions now present the product as `SDR Calibration` in macOS,
Windows, and Linux launchers, shortcuts, and installed-application listings
while retaining `sdrcal-gui` as an internal implementation name. The installed
macOS bundle is `SDR Calibration.app`. Every GUI package definition includes
the same approved, project-owned application icon in its native platform
formats. The exact current DMG passed a reversible same-host install, first
launch, normal relaunch, CLI help, removal, and restoration lifecycle. The
exact indexed hash also has an owner-confirmed clean-host lifecycle pass.
Prior-version upgrade qualification remains open; existing packages without
the final name and icon remain historical package evidence only.

Phase 14.4 requires each new DMG to carry the complete hash-pinned Qt source,
the exact deployed runtime inventory, license disposition, and Qt replacement
instructions. Notarization and license disposition alone do not establish
clean-host installation, upgrade/uninstall behavior, live SDR access, device
support, or calibration accuracy.

## Windows package status

The primary Windows release path is the Microsoft Store MSIX governed by
decision 0025. Its package definition presents `SDR Calibration` as the installed
product and Start menu shortcut, uses the project icon for both identities, and
launches the internal `sdrcal-gui.exe` executable while exposing `sdrcal.exe`
through an execution alias. The first native MSIX passed package/runtime/license
audit and a same-host development-signed lifecycle. Store upload,
certification, Microsoft signing, delivery, and clean-host qualification remain
open.

The WiX MSI remains available only for bounded testing with an explicitly
trusted self-signed development certificate. Its retained same-host and
owner-confirmed clean-host evidence does not qualify the Store MSIX and it is
not a required release download.

## Ubuntu package status

Ubuntu is not a supported product target and no current Ubuntu validation is
planned. The retained implementation and DEB tooling may be used for future
portability work, but their presence does not establish current build, package,
installation, device, or calibration support.

Phase 14.3 provides a production gate for an Ubuntu 24.04 x86_64 DEB containing
the recorded-input GUI and CLI. It uses dynamically linked Qt libraries from
Ubuntu system packages and deliberately excludes SoapySDR and vendor modules.
The package is not a live-device build.

The desktop entry presents `SDR Calibration`, launches the internal
`sdrcal-gui` executable without a terminal, and resolves `sdr-calibration`
through the installed hicolor icon set. Exact Ubuntu package execution and
clean-host qualification remain separate gates.

The DEB carries the exact dependency clauses, installed package versions, and
Debian copyright dispositions used at construction, plus Qt replacement
instructions. An extracted-payload audit does not establish clean-host
installation, upgrade/removal behavior, repository compatibility, device
support, or calibration accuracy.

## Recorded- and live-input workflow

Select three explicit paths:

1. A versioned recorded- or live-calibration request JSON file.
2. The independent local registry-signature pin file.
3. A new output directory. Existing output destinations are refused.

**Review Request** identifies the explicit schema, displays bounded structured
request metadata, and shows the exact request in a read-only view. An
unrecognized schema is refused. This is operator review; the production request
parser remains authoritative. **Start Calibration** re-reviews a missing or
changed request before execution. A live request then presents a warning that
continuing may enumerate, configure, and receive from the selected SDR;
declining performs no production run. Accepted work runs on a worker while
progress is displayed. **Cancel** requests cooperative cancellation and waits
for the workflow to reach a safe stopping point.

Soapy-enabled builds inject the same production live boundary used by the CLI.
Soapy-disabled builds retain recorded mode and reject live requests before
output staging. Construction and request review do not access hardware. Actual
live execution requires a separately authorized device-, reference-, settings-,
duration-, abort-, cleanup-, and evidence-bound plan.

For a conducted RF reference, the operator must use a compatible 50-ohm path
and enough attenuation to keep the source safely below the exact SDR's
published input limit with a conservative margin. Independent RF power
measurement is optional unless the available specifications cannot establish
safety or overload is suspected. RF power is a safety and signal-quality
condition; it is not part of the frequency-calibration uncertainty.

Success loads the fixed published artifacts into read-only tabs:
`profile.json`, `evidence.json`, `summary.json`, and optional `wsjtx.ini`.
**Open Result** can inspect an existing result directory without activating,
editing, signing, replacing, revoking, or deleting its profile. Review files
are limited to 8 MiB each and symbolic-link files are refused.

The trust-pin mechanism is local and bounded, not a production trust store.
Generated profiles remain unsigned until production key management exists.
Any WSJT-X projection is explicitly lossy and does not replace the native SDR
Calibration Profile.
