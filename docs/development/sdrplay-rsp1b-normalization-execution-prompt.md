# SDRplay RSP1B normalization execution prompt

## Objective

Resolve the hardware-free production identity blocker exposed by the retained
SoapySDRPlay3 RSP1B capture without inventing configuration readbacks or
weakening exact identity/configuration matching. Preserve all requested and
effective settings and add only a narrow, provenance-bearing identity policy
supported by exact Soapy driver and hardware keys.

## Verified starting point

- The retained RSP1B capture identifies driver key `SDRplay`, hardware key
  `RSP1B`, hardware serial `2404058C60`, hardware version 6, and SDRplay API
  3.15, but its hardware-info map does not contain generic manufacturer or model
  fields.
- Its unrequested effective configuration was 200,000 Hz bandwidth, 30 dB
  aggregate gain, automatic gain enabled, driver-default clock, and 0 ppm
  correction. These are evidence from the exact capture, not universal RSP1B
  defaults.
- The generic Soapy boundary already records effective bandwidth, aggregate
  gain, automatic-gain state, clock provenance, and correction provenance. The
  production live boundary correctly rejects the sparse identity.

## Required work

1. Add an exact RSP1B identity policy that applies only when the driver key is
   `SDRplay` and the hardware key is `RSP1B`.
2. Under that exact policy, normalize an absent manufacturer to `SDRplay` and
   an absent model to `RSP1B`. Preserve nonempty generic hardware-info values
   rather than replacing them.
3. Bind use of the policy into the native device configuration with a versioned
   provenance value.
4. Preserve the exact serial from the deterministic selection path. Do not
   infer identity from a label, enumeration index, API version, or hardware
   version.
5. Keep partial key matches, other SDRplay products, missing serials, ambiguous
   clock state, supported-but-unreadable correction, and effective-setting
   mismatches fail closed.
6. Add deterministic fake-API tests for the positive policy and negative key
   boundaries. Document the implemented policy and its physical-qualification
   limits.

## Non-goals and safety boundary

- Do not enumerate, open, configure, or stream from a real SDR.
- Do not add named-gain, antenna-selection, sample-rate-list, bandwidth-list,
  clock-selection, or vendor-API controls.
- Do not treat the retained automatic gain, 30 dB gain, 200 kHz bandwidth, or
  0 ppm correction as defaults for another run.
- Do not change estimator or acceptance thresholds, reinterpret the rejected
  antenna sample, or claim WWV suitability or calibration qualification.
- Do not add dependencies or perform packaging, installation, or external
  service work.

## Validation and adversarial review

Run the focused Soapy fake-API tests, default configure/build/CTest, an isolated
Soapy-disabled configure/build/CTest, Sphinx with warnings as errors, formatting
checks when the configured formatter is available, and `git diff --check`.

Review for fabricated identity, overbroad SDRplay matching, serial or label
inference, effective-setting defaults, AGC/manual-gain confusion, ambiguous
clock or correction handling, omitted provenance, unknown-driver fallback,
device access, and qualification overclaim. Correct every actionable finding
and repeat affected checks.

## Exit criteria

Exact sparse `SDRplay`/`RSP1B` metadata can form a versioned,
provenance-bearing production identity in fake tests; nonmatching and ambiguous
cases still fail closed; effective configuration remains readback-derived; no
hardware is accessed; and physical signal and calibration qualification remain
separate gates.
