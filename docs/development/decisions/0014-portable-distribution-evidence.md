# 0014 — Portable distribution and qualification evidence

Status: Accepted

Date: 2026-08-15

## Decision

Use CMake install rules and CPack portable archives as the first packaging
foundation. Generate a configured SPDX 2.3 inventory, ship project license and
notice material, and audit staged content before treating an archive as a
candidate artifact. Native installers, runtime-library deployment, clean-host
execution, platform signing/notarization, and binary-license disposition remain
separate fail-closed gates.

Track build coverage, packaging, signing, capture qualification, and end-to-end
calibration qualification independently. A claim exists only when the matrix
links retained evidence for the exact source, platform, architecture,
dependencies, device, and test conditions.

## Consequences

The initial archive is useful for build and content validation but is not a
release-ready installer. CI configuration is intent until a run passes and is
retained. No device inherits end-to-end qualification from capture evidence or
from another platform/device row.
