# Candidate release notes template

Status: Template only — not a release designation

## Candidate identity

- Version: `TBD-BLOCKING`
- Source revision: `TBD-BLOCKING`
- Freeze decision/date: `TBD-BLOCKING`
- Release date: `TBD-BLOCKING`
- Compatibility statement revision: `TBD-BLOCKING`
- Evidence-ledger revision: `TBD-BLOCKING`

## Scope

SDR Calibration produces authoritative native frequency-calibration profiles
from bounded recorded observations. Initial binary packages are intended to be
recorded-input-only and do not bundle SoapySDR, vendor modules,
`sdrcal-capture`, or live-device calibration support.

The intended artifact set is:

- macOS 14.0-or-later Apple Silicon DMG with GUI and CLI;
- Windows 11 x64 public-trust, timestamped MSI with GUI and CLI;
- Raspberry Pi OS 13 ARM64 Raspberry Pi 4 CLI-only DEB;
- exact source archive, checksums, notices, inventories, corresponding-source
  materials, compatibility/upgrade guidance, and evidence ledger.

Actual included artifacts and hashes: `TBD-BLOCKING`

## What changed

- User-visible changes since the selected prior baseline: `TBD-BLOCKING`
- Native profile/schema changes: `TBD-BLOCKING`
- Packaging or dependency changes: `TBD-BLOCKING`
- Security-relevant changes: `TBD-BLOCKING`
- Deferred work: `TBD-BLOCKING`

## Compatibility

The native `sdr-calibration-profile` schema-major-1 semantics are the durable
compatibility boundary. Schema `1.0.0` rejects unknown top-level members;
unknown required capabilities and unsupported schema majors fail closed.
Strict run-request schemas and pre-1.0 package/UI behavior have narrower
exact-version promises. WSJT-X output is a lossy adapter and never replaces the
native profile.

Candidate-specific schema and migration assessment: `TBD-BLOCKING`

## Installation and upgrade

- Clean-install qualification by artifact: `TBD-BLOCKING`
- Prior-version upgrade qualification: `TBD-BLOCKING`
- Rollback qualification and preserved-state result: `TBD-BLOCKING`
- Platform instructions: `TBD-BLOCKING`

Do not replace these fields with same-host package smoke results.

## Qualification summary

- Hardware-free tests and package gates: `TBD-BLOCKING`
- Platform-native clean-host lifecycles: `TBD-BLOCKING`
- Physical end-to-end calibration combinations: `TBD-BLOCKING`
- Human visual review: `TBD-BLOCKING`
- Independent published-download verification: `TBD-BLOCKING`

## Known limitations and security

- Final known-limitations document: `TBD-BLOCKING`
- Supported-version security statement: `TBD-BLOCKING`
- Private vulnerability-reporting channel: `TBD-BLOCKING`

## Downloads and verification

- Release location: `TBD-BLOCKING`
- Checksums file: `TBD-BLOCKING`
- Signature verification instructions: `TBD-BLOCKING`
- Corresponding-source locations and hashes: `TBD-BLOCKING`
- Evidence-package manifest and SHA-256: `TBD-BLOCKING`
- Public evidence index and privacy-review result: `TBD-BLOCKING`

## Authorization

- Required-gate completeness review: `TBD-BLOCKING`
- Independent reviewer and UTC date: `TBD-BLOCKING`
- Publication authorizer and UTC decision: `TBD-BLOCKING`
- Post-publication download verification: `Pending publication`

No tag, upload, publication, or release is authorized while any required field
in this document or the prepublication evidence ledger remains blocking.
