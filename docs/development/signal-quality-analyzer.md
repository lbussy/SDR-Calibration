# Signal-quality analyzer

## Scope and status

`signal-quality-v1` is the hardware-free analyzer used to derive the sample-
dependent metrics required by observation acceptance. It consumes one exact
bounded complex-sample buffer, its effective sample rate, and a successful
carrier estimate bound to the same sample count.

The analyzer does not access an SDR, infer stream loss, resolve references,
accept or reject an observation, calculate uncertainty, fit a calibration
model, or qualify hardware. Missing-sample and discontinuity counts remain
stream/session evidence supplied by the acquisition boundary.

## Version 1 method

All samples must be finite. The carrier estimate must contain finite midpoint
frequency, drift, and reference time and must identify the exact input sample
count. The reference time must equal the exact buffer midpoint, and successful
estimator power and coherence diagnostics must be finite and valid.

For sample time $t$ and estimator reference time $t_m$, the analyzer removes
the fitted phase:

```text
2*pi * (f_mid*t + 0.5*drift*(t - t_m)^2).
```

The complex mean of the model-removed samples is the fitted carrier amplitude.
Carrier power is its squared magnitude. Residual power is the mean squared
magnitude after subtracting that complex mean. SNR is
`10*log10(carrier_power/residual_power)`. An exact-zero or extremely small
residual uses `carrier_power * 1e-15`, so the result remains finite and capped
at 150 dB. This floor is numerical policy, not a noise measurement.

A sample is counted as normalized clipping when the absolute I or Q component
is at least `0.999`. This detects contact with the declared normalized CF32
rails. It does not prove that a device ADC clipped and does not replace a later
device-specific rule when normalized full scale is not established.

Frequency instability divides the full input into non-overlapping 1,024-sample
windows. At least two complete windows are required. Each window uses the
existing versioned carrier estimator. The metric is the maximum absolute
difference between a window's midpoint estimate and the global linear model at
that absolute time. Incomplete trailing samples do not form an instability
window but remain part of SNR, clipping, and residual-spectrum analysis.

Interference analysis takes the largest centered power-of-two span no greater
than 65,536 samples, applies a symmetric Hann window, and performs a
deterministic radix-2 complex FFT on the model-removed residual. The DC bin and
two bins on either side are excluded. Every remaining bin is normalized by the
Hann-window sum; the largest squared coherent amplitude is divided by fitted-
carrier power and reported as `10*log10(interference/carrier)`. The same
`carrier_power * 1e-15` floor keeps an absent component finite at -150 dB.

Changing a version-one parameter requires a different non-empty algorithm
version, including any nested window-estimator change. Inputs are capped at
16,777,216 complex samples before the analyzer allocates its model-removed
working buffer. FFT samples and instability-window sizes are separately bounded
before allocation.

## Limitations and evidence boundary

SNR is coherent fitted-carrier power relative to all residual power, including
noise, modulation, model error, and interference. The interference metric is a
bin-centered coherent-component estimate; Hann leakage, close-in components
inside the guard, and components between bins affect it. Frequency instability
depends on the estimator and window duration. These deterministic metrics are
acceptance inputs, not measurement uncertainty, probability, confidence, or
calibration accuracy. Version one does not assign uncertainty to these
diagnostics; physical-input behavior and acceptance-threshold margins remain
separate qualification work.

Analyzer SNR is evaluated over the complete supplied buffer. Slowly varying
frequency behavior outside the fitted linear-drift model can accumulate phase
over a long buffer and reduce coherent carrier power even when shorter windows
have high SNR and the estimator's adjacent-increment coherence is near one.
Short-window success does not override a whole-buffer SNR failure; callers must
apply the acceptance policy to the exact observation duration they intend to
accept.

Golden fixtures cover clean and drifting tones, seeded noise, normalized
clipping, a known interfering tone, non-linear frequency variation, invalid
options, estimate/sample mismatch, non-finite samples, and insufficient
windows. They validate the algorithm only and create no device or calibration
qualification.
