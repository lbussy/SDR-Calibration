# 0016 — Windows MSI deployment and signing boundary

Status: Accepted

Date: 2026-08-15

## Decision

Package the recorded-input CLI and Qt Widgets application as a per-machine
Windows x64 MSI. Deploy the configured dynamic Qt runtime with `windeployqt`,
preserve already-valid timestamped vendor signatures, sign every remaining
deployed executable and DLL plus the final MSI with Authenticode SHA-256, and
verify every signature after MSI administrative extraction. The current
project signing mode uses a locally trusted, self-signed development
certificate without a timestamp. Retain a separate `PUBLIC_TRUST` mode that
requires an RFC 3161 timestamp for a future CA-backed certificate. Retain the
exact source revision, Windows build,
architecture, CMake and Qt versions, payload hashes, MSI hash, and verification
output.

Keep SoapySDR and vendor modules outside this package slice. Reject dirty source
inputs, missing tools or signing inputs, a timestamp supplied in self-signed
mode, a missing or non-HTTPS timestamp service in public-trust mode, unsigned
payload code, unsafe output paths, and incomplete administrative extraction.
The certificate remains in the Windows certificate store and is selected only
by thumbprint; private-key material is never accepted by the build or retained
as evidence.

Create the development certificate with the project helper. It creates a
non-exportable private key in the current user's personal store and trusts only
the corresponding public certificate in that user's Trusted People store. It
does not misclassify the leaf certificate as a root CA. Removal requires the
exact thumbprint and refuses a certificate with an unexpected subject.

## Consequences

A passing self-signed run can establish Authenticode and extracted-payload
integrity on a machine where that exact certificate is explicitly trusted. It
does not establish public Windows trust, SmartScreen reputation, validity after
certificate expiration, clean-host installation, complete
binary-license disposition, general Windows support, device support, or
calibration accuracy. The later license gate must audit the exact deployed Qt
closure and preserve users' shared-library replacement rights before release.
