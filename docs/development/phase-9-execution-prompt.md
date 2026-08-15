# Phase 9 execution prompt: references and evidence

## Objective

Implement the roadmap's Phase 9 reference-registry and evidence-bundle core as
deterministic C++20 libraries. Keep the work hardware-free and network-free.

## Governing contracts

Follow the reference-registry contract, ADRs 0006, 0010, and 0011, the
failure-state model, the native-profile boundary, and the project instructions.
The native profile remains authoritative. The reliability quotient remains an
assurance index rather than probability or uncertainty.

## Required implementation

1. Add a `sdrcal_reference` library with domain types for registry identity,
   version, validity, reference class, frequency and uncertainty, operating
   status, evidence sources, conditions, limitations, provenance, and integrity.
2. Validate every required field and fail closed on invalid identity,
   timestamps, frequency, uncertainty, class-to-ceiling mapping, evidence,
   provenance, digest, or required signature.
3. Provide deterministic canonical JSON serialization, SHA-256 integrity, and
   injected signature verification. Do not add a cryptographic dependency or a
   production trust store in this phase.
4. Activate registries atomically while preserving the previous known-good
   artifact on every validation, write, sync, or replacement failure.
5. Keep operator-owned local overlays separate. Resolve global and local
   records deterministically, reject duplicate IDs inside either source, make
   local shadowing visible, and never allow a global update to erase overlays.
6. Make offline selection explicit: a valid cached registry remains usable;
   expired evidence cannot be qualification-capable; missing, conflicted,
   expired, or unsuitable references fail visibly.
7. Add a `sdrcal_evidence` library that builds bounded, reviewable evidence
   bundle manifests. Record UTC and monotonic timing, source identity, byte and
   time bounds, truncation, partial failures, and atomic-write status.
8. Reject credentials, tokens, private keys, secrets, and unsafe paths from
   shareable metadata. Raw IQ must be explicit, time-limited, byte-limited, and
   separately identified; expanded diagnostics remain opt-in.
9. Add deterministic unit tests for success, boundaries, tampering, expiry,
   signature requirements, atomic preservation, overlay conflicts/shadowing,
   offline use, privacy rejection, and raw-IQ bounds.
10. Update roadmap/status/documentation to state exactly what Phase 9 does and
    does not establish.

## Non-goals

- No network client, TLS stack, downloader, registry service, telemetry, GUI,
  production CLI, production Ed25519 provider, or trust-store integration.
- No SDR enumeration, construction, streaming, RF testing, or hardware claims.
- No profile/application-service integration and no WSJT-X adapter work.
- No archive/container format or raw-IQ retention by default.

## Validation and exit gate

Run the default configure, build, and CTest presets; a SoapySDR-disabled build
and tests; Sphinx with warnings as errors; formatting/diff checks; and an
adversarial review against the prompt and governing contracts. Correct every
material finding and repeat review and validation until no blocker remains.
Phase 9 may be marked implemented and hardware-free validated only when this
gate passes. Do not claim network-update, production-signing, device, RF, or
end-to-end calibration qualification.

## Adversarial findings injected into the exit gate

The first review required these additional invariants:

- Registry activation requires an authenticated signature by default; callers
  must opt out explicitly only for a separately controlled local artifact.
- Atomic activation durability-syncs the temporary artifact before replacement.
- Registry and evidence expiry are mandatory when validity is claimed; missing
  expiry cannot remain qualification-capable.
- Shareable metadata rejects traversal and absolute-path values when a field is
  path- or location-like, in addition to rejecting secret-bearing fields.
- Atomic replacement preserves its cross-platform contract when the destination
  already exists, and serialized byte counts reject values outside signed JSON
  integer range.
