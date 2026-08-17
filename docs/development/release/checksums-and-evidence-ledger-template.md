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
TBD-BLOCKING  Windows x64 MSI exact filename
TBD-BLOCKING  Raspberry Pi ARM64 DEB exact filename
TBD-BLOCKING  detached signature for the checksum manifest exact filename
TBD-BLOCKING  each corresponding-source archive exact filename
```

## Required artifact ledger

| Artifact | Required identity and disposition | Candidate value | Gate/evidence | Status |
| --- | --- | --- | --- | --- |
| Source archive | Exact frozen revision and MIT project source | `TBD-BLOCKING` | Independent archive/hash review | `TBD-BLOCKING` |
| macOS ARM64 DMG | Developer ID signed, notarized, stapled; recorded-input GUI/CLI | `TBD-BLOCKING` | Package/license plus clean-host and upgrade/rollback evidence | `TBD-BLOCKING` |
| Windows x64 MSI | Public-trust signed and RFC 3161 timestamped; recorded-input GUI/CLI | `TBD-BLOCKING` | Package/license/trust plus clean-host and upgrade/rollback evidence | `TBD-BLOCKING` |
| Raspberry Pi ARM64 DEB | Raspberry Pi OS 13 CLI-only; no GUI/Qt/SoapySDR | `TBD-BLOCKING` | Package/resource plus clean-host and upgrade/removal evidence | `TBD-BLOCKING` |
| Project notices | MIT license and exact third-party notices | `TBD-BLOCKING` | Payload and legal-disposition review | `TBD-BLOCKING` |
| Dependency inventories | Exact runtime/SBOM per binary payload | `TBD-BLOCKING` | Exact-payload audit | `TBD-BLOCKING` |
| Corresponding source | Hash-pinned source required by conveyed dependencies | `TBD-BLOCKING` | Replacement/disposition audit | `TBD-BLOCKING` |
| Release documentation | Notes, limitations, compatibility, upgrade/rollback, security | `TBD-BLOCKING` | Cross-document adversarial review | `TBD-BLOCKING` |

## Qualification evidence ledger

| Gate | Exact combination | Evidence location and digest | Independent reviewer | Status |
| --- | --- | --- | --- | --- |
| Hardware-free baseline and focused tests | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| macOS package/signing/license | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| macOS clean install/upgrade/rollback | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Windows package/public trust/license | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Windows clean install/upgrade/rollback | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Raspberry Pi package/resource/lifecycle | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Physical end-to-end calibration | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |
| Final platform visual review | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` | `TBD-BLOCKING` |

Build, package, signing, capture, synthetic workflow, clean-host, and physical
calibration evidence are separate cells. One passing row cannot satisfy another.

## Published-download verification

- Publication destination and immutable identifiers: `TBD-BLOCKING`
- Downloaded artifact location/date: `TBD-BLOCKING`
- Independently recomputed hashes: `TBD-BLOCKING`
- Signature/notarization/timestamp verification: `TBD-BLOCKING`
- Payload/version agreement with approved candidate: `TBD-BLOCKING`
- Final reviewer and release authorization: `TBD-BLOCKING`
