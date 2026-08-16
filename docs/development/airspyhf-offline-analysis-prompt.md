# Airspy HF+ 10 MHz offline-analysis execution prompt

## Objective

Apply the immutable production carrier estimator and `signal-quality-v1`
analyzer directly to the retained private 10 MHz diagnostic CF32LE artifact.
Reconcile their result with the exploratory spectrum without accessing hardware
or changing scientific policy.

## Exact input and bounds

- Input: `/private/tmp/sdrcal-airspy-10mhz.OLD12o/capture.cf32`
- Required SHA-256:
  `c98a446ed4f6e45456a231ede56ed55daf6fdababdf1308f30f1b03f0617bac0`
- Format: headerless interleaved CF32LE
- Samples: exactly 960,000
- Sample rate: exactly 192,000 complex samples/s
- Algorithms: default `CarrierEstimatorOptions` and immutable
  `SignalQualityOptions` version `signal-quality-v1`

## Required work

1. Reverify file size and digest before analysis.
2. Build a temporary, non-installed diagnostic harness outside the repository
   that only reads CF32LE and calls the project core APIs. Do not add a product
   CLI or alter production algorithms for this evidence pass.
3. Record estimator status, reason, frequency, drift, coherence, residual phase,
   mean power, and exact sample count.
4. If estimation succeeds, record analyzer status, SNR, clipping, instability,
   interference-to-carrier ratio, carrier/residual power, window count, and FFT
   size.
5. Compare the production output with the earlier block-averaged FFT. Distinguish
   algorithm rejection, coherent-but-wrong-signal capture, and suitable intended
   carrier evidence.
6. Preserve the unchanged acceptance policy. Stop with `Not suitable` if the
   production metrics do not establish the intended near-zero WWV carrier.

## Safety and non-goals

- Do not enumerate, construct, configure, or stream from an SDR.
- Do not capture again, analyze 15 MHz, run production calibration, publish raw
  IQ, install software, or use network services.
- Do not tune estimator/analyzer/acceptance thresholds or claim station identity
  from spectral shape alone.

## Validation and exit

The temporary harness must compile against the current built core, consume the
exact hashed file once, and emit finite bounded results or an explicit typed
failure. Review the output for sample/digest mismatch, API-policy drift,
misidentified carrier, unsupported inference, and qualification overclaim.

The exit is a durable evidence update stating whether production analysis
accepted the sample mathematically and whether that result establishes the
intended WWV reference. These are separate conclusions.
