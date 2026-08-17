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
| Existing MSI | Publisher signs PE files and MSI; `PUBLIC_TRUST` requires a purchased or managed publicly trusted certificate plus RFC 3161 timestamp | Per-machine Program Files/MSI maintenance; publisher controls updates | Required independent Windows installer |
| Store MSI/EXE | Same publisher-signing requirement; Microsoft does not re-sign the installer | Store launches the publisher-hosted installer; existing-user updates remain publisher-managed | No no-cost signing advantage |
| Store MSIX | Microsoft re-signs the package after certification for Store delivery | Identity-based packaged install; Store-hosted atomic updates and clean package removal | Separate no-certificate-cost Store route |

The Store signature applies only to the certified Store-delivered MSIX. It does
not transfer to the MSI or a directly downloaded MSIX.

## Build inputs and command

Reserve the product first and copy only these non-secret identity values from
Partner Center into the configure command. Do not guess them or commit account
credentials, tokens, certificates, or private keys.

```powershell
cmake --preset windows-store-release `
  -DSDRCAL_STORE_PACKAGE_NAME='<Package/Identity/Name>' `
  -DSDRCAL_STORE_PUBLISHER='<Package/Identity/Publisher>' `
  -DSDRCAL_STORE_PUBLISHER_DISPLAY_NAME='<publisher display name>' `
  -DSDRCAL_STORE_PRODUCT_NAME='<reserved product name>' `
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

The initial Store listing must accurately describe the recorded-input boundary,
provide required descriptions and imagery, and supply a privacy-policy URL if
the app accesses personal information or policy/law otherwise requires it.
Declaring `runFullTrust` requires a truthful capability explanation during
submission. No additional capability is approved by this implementation.
