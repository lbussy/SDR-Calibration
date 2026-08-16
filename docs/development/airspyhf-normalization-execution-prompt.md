# Airspy HF+ normalization execution prompt

## Objective

Resolve the hardware-free readiness blocker for the exact SoapyAirspyHF family
without inventing readbacks or weakening production identity/configuration
matching. Add explicit, reviewable normalization only where the Soapy API or a
narrow driver capability policy establishes the meaning of an omitted value.

## Verified starting point

- The retained Airspy HF+ capture identifies driver key and hardware key
  `AirspyHF`, hardware serial `2f52ff5de72635ba`, RX antenna, and verified sample
  rate/frequency, but its generic hardware-info map omits manufacturer/model,
  `getClockSource()` is empty, and frequency correction is unsupported.
- The native profile requires a nonempty manufacturer/model binding, clock
  source, and finite effective driver correction.
- The production live boundary currently rejects the sparse metadata before
  analysis, as required by its fail-closed contract.

## Required work

1. Extend the injected Soapy API with selectable-clock-source discovery so an
   empty current source can be distinguished from an ambiguous selectable
   source.
2. Normalize an empty current source to the explicit token
   `soapy-driver-default` only when the driver reports no selectable clock
   sources. Record that provenance in the profile binding extension.
3. Record effective frequency correction as zero only when Soapy explicitly
   reports that frequency correction is unsupported. Do not convert a supported
   but missing/failed readback to zero. Record the unsupported-zero provenance.
4. Add the narrow `AirspyHF` identity policy: when both driver and hardware keys
   are exactly `AirspyHF`, missing hardware-info fields normalize to manufacturer
   `Airspy` and model `Airspy HF+ family`, with an explicit policy-version binding.
   Preserve the exact serial and do not infer it from a label.
5. Keep unknown sparse drivers, selectable-but-unreported clocks, supported but
   unreadable correction, serial absence, or policy mismatch fail closed.
6. Add deterministic fake-API tests for the positive policy and every negative
   boundary. Update capture/live contracts and the Airspy readiness record.

## Non-goals and safety boundary

- Do not enumerate, open, configure, or stream from a real SDR.
- Do not claim the normalized family name proves a retail submodel.
- Do not add operator overrides, label parsing, generic manufacturer inference,
  schema relaxation, acceptance-threshold changes, or physical qualification.

## Validation and adversarial review

Run the focused Soapy fake-API tests, default configure/build/CTest, isolated
Soapy-disabled configure/build/CTest, Sphinx with warnings as errors, formatting
checks, and `git diff --check`.

Review for fabricated identity, unsupported-to-zero confusion, ambiguous clock
selection, serial inference, provenance omission, request/effective mismatch,
capture-manifest drift, unknown-driver fallback, device access, and qualification
overclaim. Correct all actionable findings and repeat affected checks.

## Exit criteria

Sparse AirspyHF metadata can form an exact, provenance-bearing production
identity/configuration through fake tests; unknown or ambiguous cases still fail
closed; no hardware is accessed; and the exact physical qualification remains a
separately authorized gate.
