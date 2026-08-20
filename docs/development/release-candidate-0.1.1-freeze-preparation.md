# Release candidate 0.1.1 freeze preparation

## Status

**Prepared for an exact source freeze; this document does not name an unknown
future commit or populate unknown artifact hashes.**

After this record and its attributable corrections pass validation, the clean
commit containing them is pushed to `origin/main`. That exact synchronized
commit becomes the frozen `0.1.1` source candidate. No repository change is
permitted after designation: a discovered source or documentation defect
invalidates the candidate and requires a new commit, revision, and artifact set.

The freeze is not a tag, release, Store action, or publication authorization.

## Bound product contract

- Application version: `0.1.1` from the CMake project version.
- Native profile: schema major 1, current schema `1.0.0`.
- Recorded and live requests: exact schema `1.0.0`; the initial packages expose
  recorded-input operation only.
- Diagnostic capture: format version 1, retained as a separately built source
  capability rather than an initial package feature.
- RC-WF: passed for source candidate `b269f9c`; the frozen package candidates
  must preserve that production workflow and fail-closed claim boundary.
- Device claims: none proposed. RC-01 and positive RC-03 cells are optional and
  do not block this candidate.
- Claim language: profiles are evidence-bounded. `traceable`, `certified`, or a
  fixed accuracy claim applies only to a specific result whose evidence meets
  its defined contract.

## Required artifact set

| Artifact | Exact boundary | Required candidate evidence |
| --- | --- | --- |
| Source | Archive of the frozen source revision | Revision, clean/synchronized state, filename, bytes, SHA-256, license |
| macOS | Apple Silicon DMG for macOS 14 or later; recorded-input GUI and CLI | Developer ID identity result, notarization acceptance, staple, Gatekeeper, payload/runtime inventory, Qt corresponding source, filename, bytes, SHA-256 |
| Windows | Windows 11 x64 Store MSIX; recorded-input GUI and CLI | Unsigned pre-submission MSIX identity, version `0.1.1.0`, payload/logo/runtime/license audit, WACK and development-lifecycle results where authorized, filename, bytes, SHA-256; Microsoft signature remains a later Store gate |
| Raspberry Pi | Raspberry Pi OS 13 ARM64 DEB for Raspberry Pi 4; recorded-input CLI only | Native build/tests, payload/resource/runtime/license audit, filename, bytes, SHA-256 |
| Release support | Notices, inventories, corresponding-source files, checksums, release documents, and evidence ledger | Exact file identities and privacy-reviewed public/private disposition |

Historical artifacts and hashes cannot satisfy these rows.

## Construction commands and hosts

All checkouts must equal the frozen revision and be clean before construction.

macOS uses the local Apple Silicon host and the existing configured identity,
notary profile, Qt binaries, and hash-pinned corresponding-source inputs:

```text
cmake --preset macos-release
cmake --build --preset macos-release
ctest --preset macos-release
cmake --build --preset macos-release --target macos-dmg
```

The DMG target is fail closed and performs signing, application and DMG
notarization, stapling, Gatekeeper, payload, runtime, license, and hash checks.
If existing signing or notarization authority is unavailable, construction
stops rather than producing a substitute release artifact.

Windows uses the configured Windows build host and the existing Store-MSIX
contract:

```text
cmake --preset windows-store-release
cmake --build --preset windows-store-release
ctest --preset windows-store-release
cmake --build --preset windows-store-release --target windows-store-msix
```

The pre-submission MSIX is not Store-certified or Microsoft-signed. WACK and a
temporary development-signed same-host lifecycle are candidate qualifications,
not construction or Store-signing substitutes. No package is uploaded or
submitted by this freeze.

Raspberry Pi uses the configured `wspr4` Raspberry Pi OS 13 ARM64 build host:

```text
cmake --preset raspberry-pi-cli-release
cmake --build --preset raspberry-pi-cli-release
ctest --preset raspberry-pi-cli-release
cmake --build --preset raspberry-pi-cli-release --target raspberry-pi-deb
```

The package contains no GUI, Qt, SoapySDR, capture CLI, vendor module, or live
device access.

## Package and evidence rules

- Record source revision, tree state, UTC interval, OS, architecture, compiler,
  CMake, Qt where applicable, packaging tools, and dependency versions.
- Record each artifact's exact filename, byte size, SHA-256, internal version,
  architecture, identity, signing state, payload inventory, and license
  disposition.
- Keep generated packages and full host logs outside Git. Commit no binary,
  credentials, private keys, tokens, private host inventory, or machine-specific
  secret.
- Treat package evidence as artifact-specific. Prior clean-host, signing,
  notarization, WACK, or lifecycle results do not transfer to rebuilt bytes.
- Keep RC-WF source validation, package binding, optional hardware claims,
  Store certification, lifecycle qualification, and release authorization as
  separate evidence cells.
- Preserve the public/private evidence boundary and exclude raw IQ by default.

## Candidate invalidation

The candidate is invalid if any repository content changes after designation,
an artifact is not built from the frozen revision, version/schema/package
identity disagrees, a required audit fails, a historical artifact is
substituted, signing state is overstated, or private information enters the
public evidence set. Invalidation requires a new frozen revision and rebuilt
artifact set; no old hash carries forward.

## Pre-freeze adversarial review

The review found and closed three repository findings before designation:

1. CPack, Windows MSI/MSIX, Raspberry Pi, and historical Ubuntu package metadata
   made unconditional `traceable` claims. Those descriptions now say
   `evidence-bounded` without changing conditional reference-class semantics.
2. The package claim language had no deterministic regression guard. The
   cross-platform branding verifier now requires `evidence-bounded` in every
   package metadata source and rejects the unconditional phrases.
3. Decision 0022, the release ledger, known-limitations template, release-notes
   template, roadmap, and readiness conclusion still treated a positive
   physical-device calibration cell as a release or freeze blocker. They now
   require RC-WF package binding and retain physical cells only for device
   claims actually proposed by a candidate.

Package-host preflight also found a stale macOS cache path for the Qt source
archive. An existing 50,648,500-byte retained archive was found with the exact
expected SHA-256
`d9594a31228aa23ad6b531719a29b45f0f3989fe6c136d45767ea179f233c1ac`;
the candidate configure must bind that readable file explicitly. The Windows
and Raspberry Pi hosts were reachable, and their existing checkouts were clean
but behind the candidate branch. They must be synchronized only after the
frozen revision is pushed.

No unresolved repository finding remains at designation time.

## First-candidate invalidation

Revision `4ab5c4acea33aaab63593c28986959dea1b96cde` was designated and then
invalidated during macOS construction. The preset had auto-selected QtBase
from one Homebrew prefix and QtSvg from another. `macdeployqt` could not resolve
the QtSvg framework, and the partial staged closure also contained a runtime
without an exact Qt disposition. The target stopped before any notarization
submission; no artifact from that attempt is reusable.

The package path now rejects a QtSvg package outside the prefix that supplies
`macdeployqt`, before staging, signing, or notarization. The replacement
candidate must be configured explicitly against the retained unified official
Qt 6.11.1 installation and both hash-pinned qtbase and qtsvg source archives.
This is a preflight correction, not a relaxation of the runtime or license
audit.

Revision `50ad4d36b421dadee4990edfbf1433e9177ae303` was then invalidated by a
second preflight stop. Explicitly binding `Qt6_DIR` to the unified installation
caused CMake to retain the valid value with cache type `UNINITIALIZED`; the
package script accepted only cache type `PATH` and therefore failed to locate
`macdeployqt`. The parser now accepts any CMake cache type while retaining the
exact required Qt directory suffix and all physical-prefix checks. This attempt
also stopped before staging, signing, or notarization and produced no reusable
artifact.

## Known post-freeze gates

Freezing source and constructing candidate artifacts do not complete:

- reconciliation of the corrected evidence-bounded Partner Center listing;
- Store upload, certification, Microsoft signing, delivery, or clean-host use;
- applicable macOS and Raspberry Pi upgrade/rollback qualification;
- Windows MSI coexistence and manual migration qualification;
- RC-08 final evidence assembly and independent review;
- final release audit, tagging, publication, or downloaded-artifact verification.

No SDR or RF hardware is part of the freeze process.
