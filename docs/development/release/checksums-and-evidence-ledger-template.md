# Candidate checksums and evidence-ledger template

Status: Template only — every candidate value is blocking

## Candidate binding

- Version: `TBD-BLOCKING`
- Source revision: `TBD-BLOCKING`
- Tree cleanliness and upstream synchronization evidence: `TBD-BLOCKING`
- Freeze decision/date: `TBD-BLOCKING`
- Build start/end UTC: `TBD-BLOCKING`

## Release-artifact checksums

Use lowercase SHA-256 and exact filenames. Historical package hashes must not be
substituted for candidate values.

```text
TBD-BLOCKING  source archive exact filename
TBD-BLOCKING  macOS ARM64 DMG exact filename
TBD-BLOCKING  Windows x64 Store MSIX exact identity/version
TBD-BLOCKING  Raspberry Pi ARM64 DEB exact filename
TBD-BLOCKING  each corresponding-source archive exact filename
```

Optional detached `SHA256SUMS` signature: `Not used` or `TBD-BLOCKING` with
algorithm, key identity, trust policy, signature filename/SHA-256, verification
instructions, and independent result. No current decision requires this
signature, and no production checksum-signing policy is established.

## Required artifact ledger

| Artifact | Required identity and disposition | Candidate value | Gate/evidence | Status |
| --- | --- | --- | --- | --- |
| Source archive | Exact frozen revision and MIT project source | `TBD-BLOCKING` | Independent archive/hash review | `TBD-BLOCKING` |
| macOS ARM64 DMG | Developer ID signed, notarized, stapled; recorded-input GUI/CLI | `TBD-BLOCKING` | Package/license plus clean-host and upgrade/rollback evidence | `TBD-BLOCKING` |
| Windows x64 Store MSIX | Microsoft-certified and Store-signed; recorded-input GUI/CLI | `TBD-BLOCKING` | Package/license/certification plus Store delivery, clean-host, migration, and applicable update evidence | `TBD-BLOCKING` |
| Raspberry Pi ARM64 DEB | Raspberry Pi OS 13 CLI-only; no GUI/Qt/SoapySDR | `TBD-BLOCKING` | Package/resource plus clean-host and upgrade/removal evidence | `TBD-BLOCKING` |
| Project notices | MIT license and exact third-party notices | `TBD-BLOCKING` | Payload and legal-disposition review | `TBD-BLOCKING` |
| Dependency inventories | Exact runtime/SBOM per binary payload | `TBD-BLOCKING` | Exact-payload audit | `TBD-BLOCKING` |
| Corresponding source | Hash-pinned source required by conveyed dependencies | `TBD-BLOCKING` | Replacement/disposition audit | `TBD-BLOCKING` |
| Release documentation | Notes, limitations, compatibility, upgrade/rollback, security | `TBD-BLOCKING` | Cross-document adversarial review | `TBD-BLOCKING` |
| Evidence package manifest | Exact candidate files, evidence cells, privacy, review, authorization | `TBD-BLOCKING` | Structural/filesystem/checksum reconciliation | `TBD-BLOCKING` |

## Qualification evidence ledger

| Gate | Exact combination | Evidence location and digest | Independent reviewer | Status |
| --- | --- | --- | --- | --- |
| Hardware-free baseline and focused tests | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| macOS package/signing/license | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| macOS clean install | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| macOS upgrade | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| macOS rollback and cleanup | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Windows MSIX package/Store certification/license | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Windows clean install | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Windows Store update or verified no-predecessor N/A | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Windows MSI migration/coexistence and cleanup | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Raspberry Pi package/resource/license | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Raspberry Pi clean install | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Raspberry Pi upgrade | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Raspberry Pi rollback/removal and cleanup | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Physical end-to-end calibration | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Final platform visual review | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Notices/inventory/corresponding-source/privacy | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |

Build, package, signing, capture, synthetic workflow, clean-host, and physical
calibration evidence are separate cells. One passing row cannot satisfy another.

Allowed required-cell states are `TBD-BLOCKING`, `Missing`, `Blocked`, `Failed`,
`Stale`, `Unreviewed`, and `Passed`. Only `Passed` satisfies a required cell.

## Public/private evidence disposition

| Evidence class | Public package treatment | Candidate disposition |
| --- | --- | --- |
| Release artifacts, notices, inventories, corresponding source, release documents | Include exact approved files | `TBD-BLOCKING` |
| Public evidence summaries and ledger | Include privacy-reviewed relative index and digests | `TBD-BLOCKING` |
| Full qualification logs and host inventories | Retain outside public root; index exact locator/digest/scope | `TBD-BLOCKING` |
| Raw IQ and expanded diagnostics | Retain privately when required; never ship by default | `TBD-BLOCKING` |
| Credentials, signing keys, tokens, private reports | Exclude; record only non-sensitive verification result | `TBD-BLOCKING` |

Privacy review, excluded-material inventory, reviewer, and UTC date:
`TBD-BLOCKING`

## Published-download verification

- Prepublication state: `Pending publication` is allowed only after every other
  required gate is `Passed` and publication authorization is recorded.
- Publication destination and immutable identifiers: `TBD-BLOCKING`
- Downloaded artifact location/date: `TBD-BLOCKING`
- Independently recomputed hashes: `TBD-BLOCKING`
- Signature/notarization/Store-certification verification: `TBD-BLOCKING`
- Payload/version agreement with approved candidate: `TBD-BLOCKING`
- Independent post-publication reviewer: `TBD-BLOCKING`
- Published verification result: `TBD-BLOCKING`

Local assembly cannot mark this section passed. The package may reach
`Ready-to-publish` with this section explicitly `Pending publication`; it
cannot reach `Published-verified` until the published files are downloaded
independently and reconciled with the approved candidate. A mismatch requires
withdrawal or correction, not retroactive approval.
