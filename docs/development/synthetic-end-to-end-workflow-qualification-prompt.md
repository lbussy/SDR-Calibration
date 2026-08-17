# Synthetic end-to-end workflow qualification prompt

## Objective

Qualify the production calibration workflow's cross-component behavior with a
deterministic, hardware-free fixture. Verify that accepted synthetic
observations produce a semantically valid and integrity-protected native
profile, correctly linked evidence, bounded in-domain evaluation, and
reviewable failure behavior without making a physical frequency-accuracy claim.

## Verified starting point

- The Phase 11 application workflow already composes authenticated reference
  resolution, injected acquisition, observation acceptance, linear fitting,
  uncertainty, reliability quotient, evidence generation, native profile
  creation, cancellation, and optional WSJT-X projection.
- Existing unit coverage proves stage-level success, repeatability, and several
  failures, but it does not verify the successful output through the production
  profile evaluator or explicitly check the profile-to-evidence digest link.
- The native SDR Calibration Profile remains authoritative. Synthetic fixtures
  cannot qualify an SDR, reference path, signal, calibration accuracy, or
  distribution artifact.

## Required work

1. Extend the deterministic application-workflow fixture through the production
   profile validation, integrity-verification, and evaluation APIs.
2. Verify every expected successful workflow stage is recorded in order and
   the workflow completes without a failed stage.
3. Verify that profile provenance binds the exact SHA-256 of the generated
   evidence manifest and that the evidence manifest binds the workflow summary.
4. Verify the fitted linear model and expanded uncertainty are finite and the
   reliability quotient remains governed by the versioned assurance policy.
5. Evaluate the generated profile at an in-domain frequency, temperature,
   warm-up, exact device, and exact effective configuration. Check the sign
   convention by independently reconstructing estimated true frequency.
6. Verify representative consumer-side failures: tampered profile integrity
   and out-of-domain evaluation must fail closed.
7. Retain existing rejection, cancellation, reduced-assurance, and deterministic
   repeatability coverage. Document the exact scope of this synthetic gate.

## Non-goals and safety boundary

- Do not enumerate, open, configure, or stream from an SDR.
- Do not read retained RF captures or connect a calibration reference.
- Do not add device, driver, packaging, network, trust-store, signing-provider,
  profile-activation, or WSJT-X file-replacement behavior.
- Do not weaken estimator, observation-acceptance, integrity, uncertainty,
  assurance, identity, configuration, validity, or evaluation gates.
- Do not describe synthetic numeric agreement as physical calibration accuracy.

## Validation and adversarial review

Run the focused application-workflow test, default configure/build/CTest, an
isolated Soapy-disabled configure/build/CTest, Sphinx with warnings as errors,
the configured formatter, and `git diff --check`.

Review for tautological digest checks, skipped workflow stages, sign-convention
errors, evaluation outside the fitted domain, identity/configuration bypass,
integrity bypass, uncertainty or reliability conflation, dependence on local
time or random state, hardware access, and qualification overclaim. Correct
all actionable findings and repeat affected validation.

## Exit criteria

One deterministic hardware-free fixture proves the production workflow result
can be validated, integrity-checked, evidence-linked, and evaluated in domain;
tampering and extrapolation fail closed; existing failure coverage remains
green; and the result is explicitly limited to software workflow behavior.
