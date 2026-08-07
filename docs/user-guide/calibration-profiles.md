# Calibration profiles

An SDR Calibration Profile describes how one identified SDR configuration's
indicated frequencies relate to estimated true frequencies under stated
conditions.

The reusable profile contains the device identity, effective SDR configuration,
frequency-error model, validated domain, uncertainty, and provenance. A target
frequency belongs to an application record created when another program uses
the profile; it is not part of the semi-durable device profile.

The normative format is defined by the
[profile contract](../development/profile-contract.md) and the JSON Schema in
the repository's `schemas/` directory.

## Reference reliability

Calibration is not limited to nationally confirmed broadcast references. A
profile may use a locally characterized signal generator, disciplined
oscillator, injected signal, or an ad-hoc received signal when the source and
available evidence are recorded.

The profile reports a reliability quotient from 0 through 100. Less traceable
references impose lower ceilings on that quotient: a locally characterized
reference can score no higher than 75 and an ad-hoc reference no higher than
50 under the initial policy. These profiles remain useful, but a consumer can
distinguish them from profiles based on authority-confirmed or derived
traceable references and reject them for qualification work.

The quotient describes evidence assurance. It is not a probability or a
measurement-uncertainty value. Consumers must evaluate both the reliability
quotient and the stated frequency uncertainty.
