# macOS ARM64 signed DMG same-host evidence

This record covers the hardware-free macOS package run on the development Mac
on 2026-08-17. It qualifies one exact signed and notarized DMG on the host that
built it. It is not clean-host installation or prior-version upgrade evidence.

## Exact inputs

- Source: `5eb3670b08b8aee2c4d915e5553b140394dc0d01`, clean and synchronized
  `main` checkout
- Host: macOS 26.5.2, build 25F84, Apple Silicon ARM64
- Apple clang 21.0.0; CMake/CTest 4.4.2; macOS SDK 26.5
- Official Qt 6.11.1 dynamic frameworks; deployment target macOS 14.0
- SoapySDR disabled; no SDR enumeration, streaming, calibration, or RF activity
- Developer ID Application signing with hardened runtime and secure timestamp
- Application and DMG notarized through the existing `sdrcal-notary` profile
- DMG: `SDRCalibration-0.1.0-macOS.dmg`
- DMG SHA-256:
  `94a831d4549b92edd85222c55e0cd64395dbe8acfd1f4bd5c57351f15bf80ad4`

## Automated package results

- Release configuration and build passed.
- All 16 hardware-free CTest tests passed.
- The staged package audit and exact-payload license gate passed.
- Independent SHA-256 calculation matched the package evidence manifest.
- Application and DMG signature verification passed. Notarization was accepted,
  staples validated, and Gatekeeper accepted the DMG and mounted application.
- Runtime-closure, bundle metadata, product-name, executable, and icon-resource
  checks passed.
- The packaging target's mounted-app smoke remained running for three seconds
  without hardware access.

The detailed generated logs remain under
`build/macos-official-release/macos-package/evidence` and are intentionally not
committed because they are reproducible build output.

## Same-host lifecycle results

- A pre-existing `/Applications/SDR Calibration.app` was moved to an isolated
  backup path before the qualified copy was installed. It was not overwritten.
- The application copied from the exact mounted DMG passed strict code-signing
  verification and Gatekeeper assessment from `/Applications`.
- The installed executable SHA-256 matched the mounted executable:
  `3669e780f6ae2bdb39bef5adf7e4c12c095ccc0e6ed60cca8640764b7ac27b1a`.
- First launch displayed `SDR Calibration — Recorded Input`, the recorded-input
  scope notice, expected input selectors, review/start controls, status area,
  and result tabs. No security, permission, or unexpected warning prompt
  appeared. No calibration action was initiated.
- Quit and normal relaunch succeeded with the same expected window and controls.
- The DMG's separately packaged `bin/sdrcal --help` path returned the documented
  calibrate usage without mutation.
- The qualified installed copy was removed, the pre-existing application was
  restored to its original path, and its executable hash matched the value
  recorded before isolation. The temporary copy was deleted, the exact DMG was
  detached, and no SDR Calibration process remained.

## Qualification boundary

This result qualifies only the recorded source, artifact hash, host, and
same-host lifecycle. It does not establish clean-host installation,
prior-version upgrade, Intel macOS support, public artifact publication, SDR
device support, reference suitability, or calibration accuracy.
