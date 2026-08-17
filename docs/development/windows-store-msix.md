# Windows Store MSIX path

## Suitability decision

The recorded-input SDR Calibration GUI and CLI are suitable for a distinct
Store-distributed MSIX. They run as the current user, accept explicit
user-selected files and a new output directory, and do not require elevation,
services, drivers, per-machine registry state, or writes to the installed
payload. The GUI remains the Start application; `sdrcal.exe` is available
through an app execution alias.

This decision does not extend to live SDR support or silently remove behavior
from the MSI. If a future package needs drivers, services, elevation, custom
install actions, or broader capabilities, suitability must be reassessed.

## Distribution paths

| Path | Signature and trust | Install/update model | Current role |
| --- | --- | --- | --- |
| Existing MSI | Locally self-signed for bounded testing with explicit temporary trust | Per-machine Program Files/MSI maintenance | Optional development/testing artifact; not a release download |
| Store MSI/EXE | Same publisher-signing requirement; Microsoft does not re-sign the installer | Store launches the publisher-hosted installer; existing-user updates remain publisher-managed | No no-cost signing advantage |
| Store MSIX | Microsoft re-signs the package after certification for Store delivery | Identity-based packaged install; Store-hosted atomic updates and clean package removal | Primary required Windows release artifact |

The Store signature applies only to the certified Store-delivered MSIX. It does
not transfer to the MSI or a directly downloaded MSIX.

Decision 0025 makes Store MSIX primary. The self-signed MSI remains useful for
bounded test work but cannot satisfy a release gate. MSI and MSIX may coexist;
automatic conversion or removal is not implemented. The supported migration
procedure and its state/identity/CLI behavior must be qualified before release.

## Reserved identity and build command

The Store product was reserved on 2026-08-17 with these public manifest values:

- product name: `SDR Calibration`;
- package identity name: `LeeBussy.SDRCalibration`;
- package identity publisher: `CN=66465467-9B9D-4BDE-9CC9-BE392698D910`;
- publisher display name: `Lee Bussy`; and
- Store ID: `9PK9W5S2D66L`.

These values are source-bound rather than configure-overridable. A different
identity requires a reviewed source change. Account credentials, tokens,
certificates, private keys, contact details, and verification materials remain
outside the repository.

```powershell
cmake --preset windows-store-release `
  -DSDRCAL_QT_SOURCE_ARCHIVE='<matching qtbase source archive>' `
  -DSDRCAL_QT_SOURCE_SHA256='<lowercase SHA-256>'
cmake --build --preset windows-store-release
ctest --preset windows-store-release
cmake --build build/windows-store-release --target package-audit
cmake --build build/windows-store-release --target windows-store-msix
```

The target emits an unsigned pre-submission MSIX and evidence under
`build/windows-store-release/windows-store-package`. It refuses a dirty or
upstream-unsynchronized source tree and refuses to reuse that output path.
Delete or archive a previous output deliberately before a new run.

## Qualification and external state

Local construction and unpacking establish only manifest, payload, runtime,
license, and hash evidence. Installation requires a suitable development trust
arrangement and does not establish Store signing. Partner Center states must be
reported exactly as not created, draft, uploaded, certification pending,
certified, or published. Package upload, certification submission, and public
availability each require their own authorization.

The first native Windows result at revision `99d96a6` passed 19 hardware-free
tests, package audit, MakeAppx creation/unpack validation, and a same-host
development-signed install/CLI alias/GUI relaunch/uninstall lifecycle. The
unsigned candidate SHA-256 is
`de28c7d9c11c1fcf95e70ffc80a5147c6dfd5f26e93c8f6138906a6f6ba95f85`.
See the [native MSIX qualification record](https://github.com/lbussy/SDR-Calibration/blob/main/evidence/windows-store/2026-08-17-native-msix-qualification/README.md).
This result is not Store signing, certification, Store delivery, clean-host
qualification, or update qualification. Partner Center remained a draft and no
package was uploaded or submitted.

The initial Store listing must accurately describe the recorded-input boundary,
provide required descriptions and imagery, and supply a privacy-policy URL if
the app accesses personal information or policy/law otherwise requires it.
Declaring `runFullTrust` requires a truthful capability explanation during
submission. No additional capability is approved by this implementation.
