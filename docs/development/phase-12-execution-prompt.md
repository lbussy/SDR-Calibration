# Phase 12 execution prompt: production CLI

## Objective

Deliver a stable, automation-safe C++20 command-line application over the
Phase 11 shared calibration workflow. The initial production path consumes an
explicit JSON run request and bounded, previously captured CF32LE observations,
then publishes a validated native profile and reviewable evidence artifacts as
one fail-closed output transaction. It must remain useful without Qt, network
access, installed WSJT-X discovery, or physical SDR access.

## Governing contracts

Follow the application architecture, native profile contract, reference and
evidence contracts, failure-state model, ADRs, prior phase prompts, roadmap,
and project instructions. The native profile remains authoritative. Preserve
requested and effective device configuration, indicated and true frequency,
measurement uncertainty, evidence assurance, and reliability quotient as
distinct concepts.

## Required implementation

1. Add a production `sdrcal` executable and reusable CLI library that call the
   existing `CalibrationWorkflow`; do not duplicate scientific or artifact
   policy in the frontend.
2. Define a versioned JSON run-request contract for recorded CF32LE input.
   Require explicit run/profile identity, timestamps, one exact device identity
   and effective configuration, independently identified observations,
   authenticated reference registry, uncertainty budget, assurance components,
   acceptance/estimator policy, bounded evidence policy, and output directory.
3. Treat every input path as an explicit operator selection. Do not discover
   files, devices, registries, credentials, or installed applications. Reject
   unknown schema versions, unknown members, duplicate semantic identifiers,
   non-finite numbers, malformed types, unsafe paths, unbounded files, and
   inconsistent sample counts before running scientific stages.
4. Implement a recorded-observation device boundary that reads headerless
   CF32LE exactly, verifies a supplied SHA-256 digest, enforces byte/sample
   limits, rejects truncated or trailing partial samples, and reports a safe
   final state. It must never enumerate, construct, tune, or stream an SDR.
5. Require registry authentication through an explicitly configured verifier.
   Until a production trust-store/key-provider exists, accept only a bounded
   separately selected local pinned-signature mapping and clearly retain that
   limitation. A run request must never declare its own trust anchors or bypass
   the Phase 11 signature requirement.
6. Provide stable commands and arguments: `calibrate --request FILE
   --trust-file FILE --output-dir DIR`, plus side-effect-free `--help` and `--version`. Reject
   ambiguous command combinations and existing output destinations.
7. Publish the native profile, evidence manifest, reduced workflow summary,
   and optional WSJT-X projection through a staging directory. Sync completed
   files and atomically rename the directory only after every requested
   artifact succeeds. On failure or cancellation, preserve the first cause,
   remove staging output, and never replace an existing result.
8. Emit human-readable progress and diagnostics to stderr. Emit exactly one
   canonical JSON terminal result to stdout so automation never needs to parse
   prose. Do not include secrets, raw IQ, registry signature material, or
   unbounded source content in diagnostics.
9. Define stable exit categories for success, usage/request errors, input or
   authentication failure, workflow/scientific failure, output-publication
   failure, and cancellation. SIGINT must request cooperative cancellation;
   cancellation is not reported as a generic failure.
10. Record requested/effective configuration and input provenance in reduced
    evidence. Artifact paths in terminal output must be relative to the final
    output directory. Existing destinations are never overwritten.
11. Keep the existing diagnostic `sdrcal-capture` tool separate. Do not
    silently change its arguments, output, exit statuses, or qualification
    claims.
12. Add deterministic hardware-free tests for complete parsing, unknown and
    malformed input, digest/size failures, workflow success/failure,
    cancellation, stable JSON/exit mapping, stdout/stderr separation, staging
    cleanup, existing-destination refusal, requested artifact publication, and
    repeatability.
13. Update the roadmap, status, testing, application architecture, source
    layout, user documentation, and toctrees. Mark only recorded-input CLI
    behavior implemented; real-device production integration, trust-store/key
    management, safe installed-WSJT-X mutation, GUI, packaging, and calibration
    qualification remain separate gates.

## Non-goals

- No SDR enumeration, construction, tuning, streaming, GPIO, or RF activity.
- No network registry transport, remote services, telemetry, or credential
  storage.
- No production Ed25519 trust store or signing key provider.
- No installed WSJT-X discovery or settings replacement.
- No GUI, packaging, installer, platform-support, supported-device, calibration
  accuracy, or release claim.
- No new scientific model or duplicated estimator, acceptance, uncertainty,
  assurance, profile, reference, evidence, or interoperability policy.

## Validation and exit gate

Run the default configure/build/CTest presets, a SoapySDR-disabled configure,
build, and CTest run, Sphinx with warnings as errors, formatting/diff checks,
and an adversarial review against this prompt and governing contracts. Inject
every material finding below, correct it, and repeat review and validation until
no blocker remains. Phase 12 is complete only when the production CLI is
deterministic, automation-safe, fail-closed, and uses the shared workflow for
recorded observations. Do not claim live-device calibration, production trust
management, hardware qualification, calibration accuracy, portability,
packaging, or release readiness.

## Adversarial findings injected into the exit gate

The first review required these additional invariants:

- Registry trust pins come from a separately selected trust file. Accepting a
  trust anchor declared by the request it authenticates would provide no trust
  boundary.
- Every observation supplies requested and effective indicated center
  frequencies separately. Recorded-input provenance must not manufacture
  effective readback by copying the request.
- Signed counts and size limits are validated before unsigned conversion, and
  each CF32LE input retains a hard 2 GiB ceiling.

The second review additionally required:

- Estimator and observation-acceptance policy values and exact sample counts
  are explicit versioned request inputs; the production frontend must not hide
  scientific defaults or infer sample counts from a potentially inconsistent
  file.
- Cancellation is rechecked before artifact writing and before the final
  publication rename.
- An explicitly requested WSJT-X projection is required output at this layer;
  the shared coordinator's optional-export warning cannot become silent CLI
  success.
- The finalized evidence manifest records completed atomic publication, its new
  digest is rebound into the native profile, and profile integrity is refreshed
  before synchronized staging and directory publication.

The third review required correlated uncertainty inputs to remain available
through the CLI instead of narrowing the shared application-service contract.
After adding strict correlation parsing and negative-index rejection, the final
review found no remaining material blocker within the recorded-input scope.
