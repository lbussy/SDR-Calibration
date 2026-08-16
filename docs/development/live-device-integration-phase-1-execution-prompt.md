# Live-device integration Phase 1 execution prompt

## Objective

Freeze the production live-device acquisition and observation-diagnostics
contract before connecting SoapySDR to the shared calibration workflow or the
`sdrcal calibrate` command. Define the smallest implementation sequence that
can preserve exact device identity, requested and effective configuration,
bounded acquisition, independently reviewable quality metrics, cancellation,
cleanup, and evidence without trusting operator-supplied acceptance results.

## Verified starting point

- The receive-only SoapySDR adapter and bounded capture recorder are
  implemented and hardware-free tested through injected fake APIs.
- The shared calibration workflow accepts an injected `DeviceWorkflowBoundary`
  and fails closed when identity, configuration, reference, acceptance, or
  cleanup requirements are not established.
- The production CLI currently implements only digest-verified recorded CF32LE
  observations. Its request supplies observation-quality metrics whose
  derivation remains outside the application.
- Observation acceptance consumes, but deliberately does not derive, SNR,
  clipping, missing-sample, discontinuity, instability, interference, effective-
  configuration, and reference-condition diagnostics.

## Required work

1. Add a durable live-device calibration contract under `docs/development/`.
2. Define the boundary among SoapySDR preparation/readback, bounded in-memory
   acquisition, signal-quality analysis, reference-condition evidence, the
   shared workflow, and artifact publication.
3. Define provenance for every acceptance input. Classify each input as derived
   from samples, derived from stream/session evidence, resolved from authenticated
   reference evidence, or supplied as an explicit operator assertion that cannot
   independently satisfy acceptance.
4. Require exact, stable device selection. Enumeration order is not durable
   identity, and ambiguous or changed identity/configuration must fail before
   model fitting.
5. Define resource bounds, cancellation checkpoints, per-observation cleanup,
   terminal device-state rules, and publication behavior. A failed or cancelled
   observation must not silently become model input.
6. Define a hardware-free implementation sequence with injected fake Soapy and
   deterministic synthetic-signal fixtures. Preserve a separately authorized
   real-device gate.
7. Update status, roadmap, and Sphinx navigation so implemented behavior,
   frozen contracts, planned code, and qualification remain distinct.

## Constraints and non-goals

- Documentation and contract work only. Do not modify production C++ or schemas.
- Do not enumerate, open, configure, or stream from an SDR or USB device.
- Do not add GUI controls, network access, trust-store integration, profile
  activation, clean-host work, packaging work, or WSJT-X file mutation.
- Do not accept operator-entered SNR, interference, instability, clipping,
  missing-sample, or discontinuity values as if the application derived them.
- Do not claim live-device integration, supported hardware, calibration
  accuracy, end-to-end qualification, or release readiness.

## Validation and adversarial review

Build the Sphinx documentation with warnings as errors, run `git diff --check`,
and review the complete diff. Search for contradictions involving `live-device`,
`recorded-input`, `planned`, `implemented`, `qualified`, and `operator`.

Treat every actionable finding as a blocker. Correct it and repeat the affected
checks and adversarial review until no actionable finding remains.

## Exit criteria

The contract identifies every required acceptance input and its provenance,
defines fail-closed lifecycle and evidence behavior, supplies a gated
hardware-free implementation sequence, and leaves production live mode and
hardware qualification explicitly unimplemented.
