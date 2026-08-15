# Phase 11 execution prompt: shared application services

## Objective

Implement the roadmap's Phase 11 common calibration-workflow coordinator as a
deterministic C++20 library. Exercise the complete workflow through injected
fake-device fixtures without Qt, network access, operating-system discovery,
installed WSJT-X access, or physical SDR hardware.

## Governing contracts

Follow the application architecture, profile contract, reference-registry
contract, observation and uncertainty policies, evidence/privacy policy,
failure-state model, ADRs 0002 and 0005 through 0011, prior phase contracts, and
the project instructions. The native profile remains authoritative. Requested
and effective configuration, uncertainty, evidence assurance, and reliability
quotient remain distinct.

## Required implementation

1. Add an `sdrcal_application` library depending on the existing core,
   profile, reference, evidence, and WSJT-X adapter libraries. It must not
   depend on Qt, SoapySDR, platform paths, network access, or a concrete CLI.
2. Define a narrow injected device-workflow boundary for deterministic
   discovery and bounded acquisition. Discovery must select exactly one
   identified device; missing or ambiguous identity fails before acquisition.
3. Carry requested center frequency and effective device identity/configuration
   separately. Every acquisition must confirm safe final device state and the
   same binding identity/configuration before its samples may be used.
4. Resolve every requested reference through the authenticated/cached global
   registry plus separate local overlays. Preserve selected class, effective
   assurance ceiling, shadowing, conditions, limitations, and evidence digest.
5. For each bounded acquisition, estimate the carrier from in-memory complex
   samples, derive the indicated carrier frequency without changing the
   reference truth, construct observation diagnostics, and run the versioned
   acceptance policy. Rejected observations stop the workflow with reviewable
   stage and reason information; they never reach fitting.
6. Fit only the explicitly requested supported local or linear model from
   accepted, independently identified observations. Prohibit extrapolation and
   retain the observation-bounded frequency domain.
7. Calculate the supplied versioned uncertainty budget and reliability
   quotient. Incomplete material uncertainty or assurance below the
   qualification threshold must visibly reduce the resulting profile to
   informational-only rather than being silently promoted.
8. Generate a native profile candidate bound to the selected device,
   configuration, references, observations, validity conditions, uncertainty,
   evidence digest, and assurance components. Validate and refresh integrity;
   no partial or invalid profile may be returned as successful.
9. Generate a bounded evidence manifest and reduced workflow-summary artifact.
   Raw IQ and expanded diagnostics remain opt-in under existing evidence
   limits. Candidate generation must not falsely claim filesystem atomic-write
   completion.
10. Optionally generate a separately identified, explicitly lossy WSJT-X
    settings projection only after the native profile validates. WSJT-X export
    failure must be reported without weakening or replacing the native profile.
11. Treat cancellation as its own terminal status, preserve the first causal
    failure and completed stage trace, and never continue into later scientific
    stages after failure or cancellation.
12. Add deterministic end-to-end tests covering success, exact discovery,
    ambiguity, reference rejection, acquisition failure/unsafe cleanup,
    cancellation, estimator and acceptance rejection, model and uncertainty
    failure, reduced assurance, profile validation, evidence privacy failure,
    optional interoperability failure, stage ordering, and repeatability.
13. Update roadmap, status, testing, architecture, and source-layout
    documentation to distinguish implemented candidate generation from deferred
    persistence, production device integration, CLI/GUI behavior, and physical
    calibration qualification.

## Non-goals

- No real SDR enumeration, construction, tuning, streaming, or RF activity.
- No network registry transport, production signing/trust store, telemetry, or
  remote service.
- No profile/evidence filesystem destination policy, multi-artifact transaction,
  installed WSJT-X mutation, CLI argument surface, or GUI.
- No new estimator, acceptance, fitting, uncertainty, assurance, profile,
  evidence, or interoperability policy that duplicates an existing component.
- No calibration-accuracy, supported-device, platform, or release claim.

## Validation and exit gate

Run the default configure, build, and CTest presets; a SoapySDR-disabled build
and tests; Sphinx with warnings as errors; formatting/diff checks; and an
adversarial review against this prompt and all governing contracts. Correct
every material finding, inject it below, and repeat review and validation until
no blocker remains. Phase 11 may be marked implemented and hardware-free
validated only when the deterministic fake-device workflow completes and every
failure path remains fail-closed. Do not claim production persistence, hardware
integration, device support, RF qualification, or calibration accuracy.

## Adversarial findings injected into the exit gate

The first review required these additional invariants:

- The global registry is signature-verified through an injected verifier before
  reference resolution; digest validity alone is insufficient for this
  workflow.
- Each acquisition returns an effective indicated center frequency separately
  from the request. Estimation uses only that verified effective value.
- Cancellation is checked between downstream scientific and artifact stages,
  not only before discovery and acquisition.
- Evidence candidate identity and creation time are bound by the coordinator to
  the calibration run rather than accepted as divergent caller metadata.
- Local-overlay selection is explicit in profile reference provenance.
- The native profile schema cannot represent the core's zero-span local-constant
  model without inventing a validity range. Phase 11 therefore rejects local
  model workflow requests until a separately reviewed profile representation
  exists.

The second review additionally required:

- Global-registry authentication has no per-request bypass.
- The acquisition boundary separately attests that its effective indicated
  center is verified; a merely finite value is insufficient.
- Estimator sample rate must agree with the effective configuration bound into
  the profile, and discovered device identity/configuration must be structurally
  usable before acquisition.

The third review required the reduced workflow evidence to retain requested and
effective device configuration and center frequency separately; retaining only
the derived indicated carrier frequency is not sufficient provenance.
