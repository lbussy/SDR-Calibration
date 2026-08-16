# Production live-CLI execution prompt

## Objective

Add an explicit live-device request mode to the production `sdrcal calibrate`
command through the existing shared calibration service and injected Soapy
workflow boundary. Preserve recorded-input behavior unchanged, derive all live
observation diagnostics inside the application, and keep normal validation
deterministic and hardware-free.

## Verified starting point

- Recorded CF32LE calibration is the only production CLI mode.
- The injected `SoapyWorkflowBoundary` already composes strict identity and
  configuration readback, bounded in-memory acquisition, carrier estimation,
  signal-quality analysis, stream evidence, and known-safe cleanup.
- Physical-device integration and qualification have not started. Existing
  capture evidence does not transfer to end-to-end live calibration.

## Required work

1. Define `sdrcal-live-calibration-request` version `1.0.0` as a schema-
   discriminated peer of the recorded request. Never infer a mode and never
   fall back between live and recorded input.
2. Preserve common workflow, trust, reference, uncertainty, assurance,
   acceptance, evidence, output-transaction, terminal JSON, and exit contracts.
3. Live observations may supply identity, reference, requested center, and
   bounded acquisition inputs only. Reject caller-supplied effective readback,
   samples, SNR, clipping, missing/discontinuity counts, instability, or
   interference metrics.
4. Parse exact Soapy selection arguments, channel, strict setting policy,
   optional gain, one positive sample-or-duration bound, read timeout, and a
   positive memory ceiling. Reject enumeration indexes as durable selection.
5. Inject live-boundary construction into the reusable CLI service. The real
   executable may construct the production Soapy boundary only for an explicit
   valid live request; tests use fake APIs. A build without Soapy support must
   reject live mode before enumeration or output staging while recorded mode
   remains available.
6. Use authenticated registry content for the reference-condition gate. A
   reference whose signed condition list is exactly the explicit token `none`
   may be marked established with registry provenance. Any other declared
   condition remains unmet in this slice;
   never trust a request-supplied assertion as machine verification.
7. Preserve cooperative cancellation, typed workflow failures, reverse-order
   cleanup, canonical terminal JSON, stable exit categories, staging cleanup,
   and atomic final publication.
8. Add hardware-free tests for schema discrimination, unknown/cross-mode
   fields, invalid bounds and selection, disabled-Soapy rejection, successful
   injected-live-boundary publication, and no fallback. Retain the separate
   fake-Soapy tests for derived metrics, cancellation, stream failure, cleanup
   failure, identity/readback, and reference-condition evidence.
9. Update user and developer documentation while keeping physical-device,
   signal/reference performance, calibration accuracy, supported-device,
   packaging, and release qualification explicitly open.

## Non-goals and safety boundary

- Do not enumerate, open, configure, or stream from a real SDR during this
  slice or its validation.
- Do not add GUI controls, raw-IQ publication, network services, trust-store or
  signing-key management, WSJT-X mutation, packaging work, or new scientific
  algorithms.
- Do not claim that fake-device tests establish physical compatibility or
  calibration qualification.

## Validation and adversarial review

Run focused parser and fake-Soapy tests, the default configure/build/CTest
baseline, an isolated Soapy-disabled configure/build/CTest run, Sphinx with
warnings as errors, formatter checks, and `git diff --check`.

Separately review the complete diff for mode confusion, recorded-mode
regression, real-device access in tests, device construction before bounds,
caller-supplied diagnostics, untrusted reference-condition assertions,
identity/configuration drift, partial samples, cleanup omission, cancellation
after staging, fallback, output overclaim, and exception escape. Correct every
actionable finding and repeat affected checks until none remain.

## Exit criteria

The production CLI has explicit recorded and live request schemas, uses the
shared workflow for both, derives live diagnostics through the injected Soapy
boundary, fails closed without Soapy support or verifiable conditions, retains
recorded behavior, passes all hardware-free checks, and leaves physical-device
execution and qualification as a separately authorized gate.
