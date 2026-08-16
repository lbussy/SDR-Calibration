# 0020 — Remove Ubuntu from the supported target list

Status: Accepted

Date: 2026-08-15

## Decision

Remove Ubuntu from the supported product target list. The supported desktop
targets are macOS 14.0 or later on Apple Silicon and Windows 11 x64. Raspberry
Pi OS 13 ARM64 on Raspberry Pi 4 remains a recorded-input CLI-only target.

Retain the existing Ubuntu source, CMake presets, packaging tooling,
documentation records, and historical evidence. They form an
unsupported portability path and do not create a current Ubuntu build,
packaging, installation, device, or calibration-support claim. No current
Ubuntu validation is required or planned. Remove Ubuntu build, release, and DEB
jobs from active CI; CI runner choice for unrelated work is not Ubuntu product
validation.

This decision supersedes only the inclusion of Ubuntu in the target list in
[decision 0019](0019-initial-platform-target-boundary.md). Decision 0019's
remaining platform boundaries and evidence rules remain in force. Decisions
[0017](0017-ubuntu-deb-system-dependencies.md) and
[0018](0018-exact-payload-license-disposition.md) continue to govern any future
use or distribution of the retained Ubuntu packaging path.

## Consequences

Ubuntu code and packaging may remain buildable, but ordinary project validation
does not need to exercise them. Historical Ubuntu evidence remains traceable and
must be labeled as historical rather than used as a current positive platform
claim. Restoring Ubuntu as a supported target requires a later accepted
decision and new retained evidence for the exact candidate being claimed.
