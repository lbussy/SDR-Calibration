# Signal-quality analyzer execution prompt

## Objective

Freeze and implement the versioned, hardware-free signal-quality analyzer
required by the live-device calibration contract. Derive SNR, normalized
clipping, frequency instability, and strongest-interference ratio from one
exact bounded complex-sample buffer and its successful carrier estimate.

## Requirements

1. Add a calibration-core analyzer independent of Qt, SoapySDR, operating-
   system paths, network access, and external formats.
2. Version the algorithm and validate all options, samples, sample rate, and
   carrier-estimate inputs. The supplied estimate must be successful, complete,
   finite, and bound to the exact sample count.
3. Define SNR as coherent fitted-carrier power divided by residual power after
   removing the midpoint-frequency and linear-drift model. Use a documented
   finite deterministic floor for an otherwise zero residual.
4. Count a normalized CF32 sample as clipped when either I or Q reaches the
   configured absolute rail threshold. Keep this rule explicit; do not claim it
   describes an ADC's device-specific clipping behavior.
5. Derive frequency instability as the maximum absolute departure of accepted
   bounded-window carrier estimates from the global linear carrier model.
6. Derive strongest-interference ratio with a bounded power-of-two Hann-window
   FFT of the model-removed residual. Exclude DC/carrier guard bins and report
   the strongest coherent residual component relative to fitted-carrier power.
7. Bound working samples and FFT size before allocation. Reject rather than
   silently weaken invalid inputs or insufficient windows.
8. Add deterministic golden fixtures for clean tone, seeded noise, clipping,
   an interfering tone, linear drift, non-linear instability, invalid options,
   mismatched estimates, and non-finite samples.
9. Document equations, normalization, bounds, limitations, and the distinction
   between derived analyzer metrics and stream-derived missing/discontinuity
   evidence. Update status and roadmap without claiming live integration or
   qualification.

## Non-goals and safety boundary

- Do not access SDR hardware or modify acquisition, CLI, GUI, profiles,
  packaging, trust management, references, or observation-acceptance policy.
- Do not infer missing samples or stream discontinuities from sample values.
- Do not call the metrics measurement uncertainty, confidence, probability,
  calibration accuracy, or hardware qualification.

## Validation and adversarial review

Run formatter checks, analyzer golden tests, the default and SoapySDR-disabled
hardware-free build/CTest suites, Sphinx with warnings as errors, and
`git diff --check`. Adversarially inspect frequency/drift phase removal, FFT
normalization and guard bins, allocation bounds, finite-floor behavior,
clipping semantics, estimate/sample binding, determinism, and documentation
claim boundaries. Correct every actionable finding and repeat affected checks.

## Exit criteria

The analyzer deterministically produces finite reviewable metrics from valid
synthetic inputs, fails closed on invalid or mismatched evidence, remains
hardware-free, and leaves live workflow and physical-device qualification as
separate later gates.
