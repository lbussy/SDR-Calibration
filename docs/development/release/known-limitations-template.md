# Candidate known-limitations template

Status: Template only — candidate-specific review required

Retain, remove, or refine each item only against the exact frozen candidate.
Any removal requires evidence or an accepted decision.

## Product and artifact boundaries

- Initial binary packages are recorded-input-only. They do not bundle SoapySDR,
  vendor modules, capture tooling, or live-device calibration support.
- The native SDR Calibration Profile is authoritative. `wsjtx.ini` and
  `fmt.all` are lossy adapters and cannot replace it.
- Generated profiles are unsigned until production signing/key management is
  implemented. Missing signature state must remain explicit and affects
  assurance; an invalid signature fails closed.
- Registry trust uses an independent local pin file, not a production network
  trust store or key-management service.
- The application does not activate, replace, revoke, or manage installed
  profiles and does not discover or mutate installed WSJT-X settings.

## Compatibility boundaries

- Application version 0.x is pre-1.0. Strict recorded/live requests currently
  require exact schema `1.0.0`; no request migration is promised.
- Only native profile schema-major-1 semantics carry the durable compatibility
  promise. Unknown required capabilities and other schema majors are rejected.
- Diagnostic CF32LE requires its matching format-version-1 capture manifest;
  the raw file alone is not self-describing.
- WSJT-X mapping is limited to tested 2.6.x–2.7.x behavior.

## Platform and qualification boundaries

- macOS exact-candidate clean-host, upgrade, rollback, and cleanup results:
  `TBD-BLOCKING`
- Windows public-trust and exact-candidate clean-host, upgrade, rollback, and
  cleanup results: `TBD-BLOCKING`
- Raspberry Pi exact-candidate clean-host, upgrade, rollback/removal, and
  cleanup results: `TBD-BLOCKING`
- Physical end-to-end calibration and supported-device claims: `TBD-BLOCKING`
- Reference suitability and accuracy claims: `TBD-BLOCKING`
- Final platform visual/scaling review: `TBD-BLOCKING`

Ubuntu remains an unsupported, currently unvalidated portability path and is
not an initial release artifact. Intel macOS, Windows ARM64, unapproved Windows
versions, generic Linux, 32-bit Raspberry Pi OS, other Pi models, and Raspberry
Pi live-device operation are outside the initial target boundary.

## Candidate-specific limitations

- Performance/resource limits: `TBD-BLOCKING`
- Accessibility limitations: `TBD-BLOCKING`
- Data migration or retained-state limitations: `TBD-BLOCKING`
- Known defects with issue links and workarounds: `TBD-BLOCKING`
- Security limitations or mitigations: `TBD-BLOCKING`
- Public/private evidence exclusions and operator impact: `TBD-BLOCKING`
