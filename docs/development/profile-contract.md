# SDR Calibration Profile Contract

Status: Draft 0.1

Schema: `sdr-calibration-profile` 1.0.0

The reference producer is a multi-platform C++ application using SoapySDR for
device discovery, configuration, and sample acquisition. The profile contract
itself is implementation-language and SDR-vendor neutral.

## 1. Purpose and scope

An SDR Calibration Profile is an immutable, machine-readable statement of how
one identified SDR configuration's indicated frequencies relate to estimated
true frequencies under stated conditions.

It allows a consumer to determine whether the profile applies, convert an
indicated frequency to an estimated true frequency, calculate uncertainty, and
reject use outside the validated frequency, time, temperature, or configuration
domain.

The profile does not calibrate a transmitter, guarantee indefinite accuracy,
assert that a reference station is currently receivable, or authorize
extrapolation outside its validated domain. `target_frequency_hz` is not a
profile field; it belongs to the record of a particular application of the
profile.

## 2. Artifact separation

The system distinguishes:

1. **SDR Calibration Profile** — the semi-durable model for an identified SDR.
2. **Calibration Application Record** — the record of evaluating that model at
   an indicated or target frequency.
3. **External adapter artifact** — an imported or exported representation such
   as WSJT-X settings or `fmt.all` observations.

External artifacts do not replace the native profile.

## 3. Device identity and configuration

A profile identifies the physical SDR using a hardware serial, driver serial,
operator-assigned identifier, or `insufficient`. A profile whose identity is
`insufficient` is informational only.

The profile records all binding operating settings that may affect frequency
interpretation, including clock source, sample rate, bandwidth, driver-applied
frequency correction, tuner path, and relevant software or firmware versions.
A consumer must reject qualification use when the connected device or a binding
setting does not match.

## 4. Frequency-error convention

The contract defines:

```text
indicated_error_hz = indicated_frequency_hz - true_frequency_hz

estimated_true_frequency_hz
    = indicated_frequency_hz - indicated_error_hz
```

A positive indicated error means the SDR reports a frequency higher than the
estimated true frequency. Producers and consumers must use this sign convention.

## 5. Frequency model

A profile contains one or more non-overlapping frequency segments. Version 1
supports `constant_ppm` and `linear` models.

For a linear model:

```text
indicated_error_hz(f)
    = intercept_error_hz
    + slope_ppm * (f - reference_frequency_hz) / 1,000,000
```

The evaluation frequency must fall within the selected segment. Consumers must
reject unsupported model types and must not silently approximate them.

## 6. Validity and uncertainty

Every profile states its calibration time, expiration or defensible
recalibration rule, warm-up requirement, and validated temperature domain.

Every usable segment has an uncertainty model. The profile states whether the
reported value is standard or expanded uncertainty, and an expanded uncertainty
states its coverage factor. Consumers must not report more precision than the
uncertainty supports.

Qualification use fails closed when the profile is expired, the target is
outside the frequency or temperature domain, warm-up is insufficient, identity
or configuration does not match, integrity fails, or application uncertainty
requirements are not met.

## 6.1 Reference assurance and reliability quotient

The producer may use authority-confirmed, derived traceable, locally
characterized, ad-hoc, or unknown references. Locally generated and ad-hoc
references are valid inputs when their identity, evidence, conditions, and
limitations are recorded; they are not promoted to authority-confirmed status.

Every profile records a versioned, integer reliability quotient from 0 through
100. It is an assurance index, not a probability, confidence level, or
substitute for measurement uncertainty. The quotient is the minimum of the
applicable assurance-component scores, so weak reference evidence cannot be
hidden by stronger device, observation, integrity, or model evidence.

The initial reference-class ceilings are:

| Reference class | Maximum score |
| --- | ---: |
| `authority_confirmed` | 100 |
| `derived_traceable` | 90 |
| `locally_characterized` | 75 |
| `ad_hoc` | 50 |
| `unknown` | 0 |

The JSON Schema constrains each class to its assigned ceiling. The semantic
validator additionally verifies that the quotient equals the minimum applicable
component score and does not exceed the reference ceiling.

A profile is qualification-capable only when its quotient meets the consumer's
required threshold and all other validity and uncertainty checks pass. The
default threshold is 90; consumers may require a higher value. Scoring policy
changes require a new policy version and regression fixtures.

## 7. Provenance, integrity, and immutability

Profiles retain the calibration run, producing software, complete reference
set, evidence-bundle digest, and observation identifiers. When a signed global
registry contributed records, its version and digest are retained. Local
overlays remain separately identifiable and cannot be represented as part of
the signed global registry.

A published profile is immutable. Recalibration or correction creates a new
profile, optionally naming the profile it supersedes.

At minimum, RFC 8785 canonical profile payload bytes have a SHA-256 digest. The
payload for hashing and signing excludes the top-level `integrity` member,
preventing a self-referential digest. A detached Ed25519 signature over the same
payload is supported and may be required by a consumer policy. An invalid
signature is a hard failure; a missing optional signature is explicit and may
lower the integrity assurance component.

## 8. Consumer results

A consumer evaluates an `indicated_frequency_hz` and may additionally supply a
`target_frequency_hz`. The result records the profile and segment identifiers,
evaluated indicated error, estimated true frequency, expanded uncertainty, and
target offset when a target was supplied.

Supported result statuses include:

- `qualification_capable`
- `calibrated`
- `informational_only`
- `profile_expired`
- `identity_mismatch`
- `configuration_mismatch`
- `outside_frequency_domain`
- `outside_temperature_domain`
- `insufficient_warmup`
- `unsupported_schema`
- `unsupported_model`
- `integrity_failure`
- `assurance_requirement_not_met`
- `uncertainty_requirement_not_met`

## 9. Adapter registry

The profile schema includes `interoperability.adapters`, a registry of external
formats associated with the profile. Version 1 recognizes:

- `wsjt-x-settings`
- `wsjt-x-fmt-all`

Every adapter entry declares its direction, source or destination application
version, creation time, artifact digest, and whether the conversion is lossy.
Import and export details are retained in `metadata` without changing the native
frequency model.

Schema 1.0 rejects unknown adapter identifiers. A later backward-compatible
schema revision may add identifiers, but a consumer must not claim to interpret
an adapter unless it implements that adapter's contract.

## 10. WSJT-X settings import

The adapter may import these WSJT-X values:

```ini
[Configuration]
CalibrationIntercept=-0.12
CalibrationSlopePPM=0.84
```

WSJT-X describes its model as:

```text
dial_error_hz = intercept_hz + slope_ppm * frequency_mhz
```

An import records the WSJT-X version, source digest, configuration or rig name,
import time, and parsed values. Because the settings file does not establish
physical device identity, frequency domain, validity, temperature conditions,
uncertainty, or traceability, importing it alone produces an
`informational_only` profile.

The importer must not infer calibration time from file modification time or
infer uncertainty from decimal precision.

## 11. WSJT-X `fmt.all` import

The adapter may import FreqCal observations from `fmt.all`. It preserves every
original line, parsed fields, rejection markers, source digest, WSJT-X version,
and parser version. Unparseable lines are retained and reported as
`manual_review_required`; they are never silently discarded.

WSJT-X rejection markings are evidence, but the calibration producer applies
its own documented acceptance and outlier rules.

## 12. WSJT-X settings export

Export is permitted only when the chosen native profile segment can be
represented by WSJT-X's single linear intercept-and-slope model. Export must be
rejected when it would require multiple segments, a nonlinear model,
interpolation, or extrapolation.

Every export is marked `lossy: true` and records omitted capabilities, including
device identity, validity conditions, frequency domain, uncertainty,
provenance, and signature information. A successful export means only that
WSJT-X received a compatible numerical projection.

## 13. WSJT-X sign compatibility

Each supported WSJT-X version range requires a tested mapping. A fixture with
known positive and negative errors must demonstrate that WSJT-X and the native
contract correct in the same direction. An unknown or untested WSJT-X version
must be rejected for qualification use or require explicit operator review.

## 14. Safe WSJT-X modification

The exporter must identify the intended WSJT-X instance, require WSJT-X to be
closed unless a supported live interface exists, preserve unrelated settings,
create a recoverable backup, modify only the calibration keys, replace the file
atomically, and read back the result. Failed verification restores the original.

Default settings locations are:

```text
Windows: %LOCALAPPDATA%\WSJT-X\WSJT-X.ini
Linux:   ~/.config/WSJT-X.ini
macOS:   ~/Library/Preferences/WSJT-X.ini
```

Named rig instances can use different paths. Discovery must present the chosen
instance to the operator rather than assuming the first match is correct.

## 15. Round-trip behavior

Importing a WSJT-X projection previously exported by this application should
recover the same intercept and slope within documented numerical precision.
Round-trip conversion cannot recover information omitted by the lossy export;
the original profile remains authoritative.

## 16. Versioning

The schema follows semantic versioning. Consumers reject unsupported major
versions. Unknown optional fields may be preserved and ignored. Unknown required
capabilities cause rejection.

## 17. Fundamental interpretation rule

A profile supports this claim:

> Under the documented conditions, this identified SDR's indicated frequency
> can be converted to an estimated true frequency using the stated model and
> uncertainty.

It does not claim that every measurement made by the SDR is correct. Consumers
remain responsible for signal identification, estimator suitability,
interference rejection, propagation effects, target behavior, and fitness of
the resulting uncertainty for their purpose.
