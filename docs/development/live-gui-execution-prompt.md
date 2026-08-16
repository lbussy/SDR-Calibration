# Production live-GUI execution prompt

## Objective

Expose the explicit production live-calibration request through the Qt desktop
application using the same application service and injected Soapy boundary as
the CLI. Keep request review, execution, cancellation, cleanup, publication,
and result review aligned while normal validation remains hardware-free.

## Verified starting point

- The production CLI accepts distinct recorded and live request schemas with no
  fallback.
- The injected Soapy workflow boundary owns bounded acquisition, derived
  analyzer metrics, stream evidence, identity/configuration readback, and safe
  cleanup.
- The desktop application currently invokes the production service without a
  live-boundary factory and describes itself as recorded-only.
- Physical end-to-end calibration is not qualified.

## Required work

1. Generalize the GUI workflow adapter to forward an injected live-boundary
   factory to the existing production command service. Preserve recorded mode.
2. In Soapy-enabled builds, construct the same production live boundary used by
   the CLI. In Soapy-disabled builds, retain recorded operation and the existing
   fail-closed live rejection.
3. Make request review clearly identify recorded versus live schema. Reject an
   unrecognized schema from GUI review without treating review as authoritative
   execution validation.
4. Require a reviewed, unchanged live request before Start can proceed, and
   display an explicit confirmation that starting live mode can enumerate,
   configure, and receive from the selected SDR. Declining must cause no device
   access and no output staging.
5. Keep background execution, cooperative cancellation, safe-close waiting,
   canonical result reporting, atomic publication, and fixed read-only artifact
   review unchanged.
6. Add deterministic GUI workflow and offscreen widget tests for mode labeling,
   review gating, side-effect-free construction/review, and factory forwarding
   boundaries. Do not access hardware.
7. Update user and developer documentation. Keep device compatibility,
   reference/signal performance, calibration accuracy, packaging, and physical
   qualification explicitly open.

## Non-goals and safety boundary

- Do not enumerate, open, configure, or stream from a real SDR in this slice.
- Do not add discovery-driven device controls, trust/key management, profile
  activation, raw-IQ publication, WSJT-X mutation, packaging, or scientific
  algorithms.
- Do not claim GUI wiring or fake-boundary tests qualify a physical device.

## Validation and adversarial review

Run focused GUI workflow and offscreen widget tests, the default
configure/build/CTest baseline, an isolated Soapy-disabled
configure/build/CTest run, Sphinx with warnings as errors, formatter checks,
and `git diff --check`.

Review the complete diff for construction-time enumeration, review-time device
access, mode fallback, stale-review execution, bypassed live confirmation,
missing factory propagation, disabled-build regression, unsafe cancellation or
close behavior, publication drift, misleading UX, and qualification overclaim.
Correct every actionable finding and repeat affected checks.

## Exit criteria

The Qt application can explicitly review and run recorded or live production
requests through the shared service, live execution is deliberately confirmed,
Soapy-disabled builds fail closed without losing recorded mode, all automated
validation is hardware-free, and physical execution remains a separately
authorized qualification gate.
