# 0022 — Initial compatibility and release artifacts

Status: Accepted

Date: 2026-08-17

## Decision

### Initial artifact set

The intended initial release artifact set is:

- a source archive for the exact released revision;
- a Developer ID-signed and notarized macOS DMG for macOS 14.0 or later on
  Apple Silicon, containing the recorded-input GUI and CLI;
- a publicly trusted and timestamped Windows MSI for Windows 11 x64, containing
  the recorded-input GUI and CLI;
- a Raspberry Pi OS 13 ARM64 DEB for Raspberry Pi 4, containing the recorded-
  input CLI only;
- SHA-256 checksums, project and third-party notices, exact dependency/runtime
  inventories, and the corresponding-source materials required for each binary
  payload; and
- release notes, known limitations, compatibility and upgrade/rollback
  statements, vulnerability-reporting guidance, and an evidence ledger bound
  to the exact artifacts.

The Windows artifact remains required by the current target decision, but the
existing locally trusted self-signed MSI cannot satisfy this release set. The
Raspberry Pi artifact remains required by its accepted CLI-only target boundary.
Decision 0024 adds a distinct Store MSIX path; it does not replace this
independently distributed MSI or transfer a Store-provided signature to it.

Ubuntu packages, generic portable binary archives, debug artifacts, raw-IQ
qualification evidence, locally self-signed Windows installers, and historical
packages are not initial release artifacts. Evidence may identify them without
shipping them as product payloads.

The initial binary packages are recorded-input-only and do not bundle
SoapySDR, vendor modules, `sdrcal-capture`, or live-device calibration support.
Live acquisition remains a separately built and qualified source capability.
No initial binary artifact creates a supported-device claim.

Generated `profile.json` is user output, not a shipped release artifact.
`evidence.json`, `summary.json`, optional `wsjtx.ini`, and diagnostic
`.cf32`/`.capture.json` pairs are likewise runtime or qualification outputs,
not files distributed as part of the release artifact set.

### Compatibility promise

Application version `0.1.0` is pre-1.0. Package layout, command-line requests,
and UI workflow may change in later minor releases when documented. Application
versioning does not silently change the meaning of an existing native profile.

The native `sdr-calibration-profile` is the authoritative semi-durable
interchange artifact. Schema major version 1 preserves the frequency-error
sign convention, device/configuration binding, bounded-domain evaluation,
uncertainty, provenance, assurance, integrity, and fail-closed semantics.
Backward-compatible additions require a higher minor version and explicit
capability handling. Unknown required capabilities are rejected. Schema
`1.0.0` rejects unknown top-level members. A breaking semantic change requires
schema major version 2 and is rejected by major-version-1 consumers unless a
future deliberate migration is implemented.

Recorded and live calibration requests are strict execution inputs, not semi-
durable profiles. The initial clients accept their exact `1.0.0` schemas and
reject unknown fields or versions. No cross-version request migration is
promised for the initial release.

The `sdrcal_capture` format-version-1 manifest and its named headerless CF32LE
file form one inseparable diagnostic pair. They are not a general recording
container or calibration profile. Any incompatible manifest or sample-layout
change requires a new format version; consumers must not guess missing sample
metadata from the raw file alone.

Published result directories have fixed required names for the current
workflow: `profile.json`, `evidence.json`, and `summary.json`, plus optional
`wsjtx.ini`. Only the native profile receives the durable schema-major promise.
Evidence and summary files remain versioned, run-bound records and must be read
according to their declared format rather than assumed compatible by filename.

WSJT-X artifacts remain explicitly lossy adapters. Compatibility is limited to
the tested WSJT-X 2.6.x–2.7.x mapping and never replaces the native profile.

No backward-compatible installer upgrade, state migration, or rollback promise
is made until exact prior-version lifecycle qualification passes on each shipped
platform. A clean installation promise is also withheld until the corresponding
fresh-host gate passes.

Unsigned profiles remain permitted by [decision 0009](0009-canonicalization-signing-and-revocation.md)
when all scientific gates pass and the missing signature is explicit. This
decision does not add production key management or promote an unsigned profile
to verified-signature status.

## Consequences

The release candidate cannot be frozen until final artifacts can match this set
or a later accepted decision changes the target boundary. Windows public-trust
signing and fresh-host lifecycle evidence, Raspberry Pi clean-host lifecycle
evidence, physical calibration evidence, and final ledger assembly remain open.

The repository may continue to implement and test live-device capabilities,
portable packages, and adapters, but their existence does not add them to the
initial distributed binary scope. Changes to the native profile's version-one
semantics or to the intended platform artifact set require explicit review and,
when durable, a superseding decision.
