# 0019 — Initial platform target boundary

Status: Superseded in part by [0020](0020-remove-ubuntu-target.md)

Date: 2026-08-15

## Decision

The initial platform targets are:

- macOS 14.0 or later on Apple Silicon only, with the GUI and CLI;
- Windows 11 x64, with the GUI and CLI;
- Ubuntu 24.04 LTS x86_64, with the GUI and CLI; and
- Raspberry Pi OS 13 ARM64 on Raspberry Pi 4, with the recorded-input CLI only.

The following are explicitly outside the initial supported matrix: Intel macOS,
Windows ARM64, Windows 10 unless deliberately retained by a later decision,
generic Linux distributions, Ubuntu ARM64 desktop, 32-bit Raspberry Pi OS,
other Raspberry Pi models until separately qualified, and SoapySDR or other
live-device operation on Raspberry Pi.

This decision supersedes the broad platform-target statements in
[0001](0001-application-platform.md) and
[0012](0012-platform-and-dependency-baselines.md). Their language, toolkit,
dependency, and evidence-boundary decisions remain in force.

## Consequences

A target identifies intended product scope; it does not establish that a build,
package, clean-host lifecycle, device, or end-to-end calibration has been
qualified. The [platform and device matrix](../platform-and-device-matrix.md)
remains authoritative for positive evidence claims.

Builds or tests on other operating systems or architectures may provide useful
portability evidence but do not add them to the initial supported matrix.
Windows Server CI evidence does not qualify Windows 11. Raspberry Pi artifacts
must remain GUI-, Qt-, SoapySDR-, and live-device-free unless a later accepted
decision deliberately changes that boundary and its qualification is retained.
