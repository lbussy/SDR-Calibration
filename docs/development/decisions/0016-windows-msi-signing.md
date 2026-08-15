# 0016 — Windows MSI deployment and signing boundary

Status: Accepted

Date: 2026-08-15

## Decision

Package the recorded-input CLI and Qt Widgets application as a per-machine
Windows x64 MSI. Deploy the configured dynamic Qt runtime with `windeployqt`,
preserve already-valid timestamped vendor signatures, sign every remaining
deployed executable and DLL plus the final MSI with Authenticode SHA-256 and an
RFC 3161 timestamp, and verify every signature after MSI
administrative extraction. Retain the exact source revision, Windows build,
architecture, CMake and Qt versions, payload hashes, MSI hash, and verification
output.

Keep SoapySDR and vendor modules outside this package slice. Reject dirty source
inputs, missing tools or signing inputs, non-HTTPS timestamp services, unsigned
payload code, unsafe output paths, and incomplete administrative extraction.
The certificate remains in the Windows certificate store and is selected only
by thumbprint; private-key material is never accepted by the build or retained
as evidence.

## Consequences

A passing run can establish Authenticode and extracted-payload integrity for
one exact MSI hash. It does not establish clean-host installation, complete
binary-license disposition, general Windows support, device support, or
calibration accuracy. The later license gate must audit the exact deployed Qt
closure and preserve users' shared-library replacement rights before release.
