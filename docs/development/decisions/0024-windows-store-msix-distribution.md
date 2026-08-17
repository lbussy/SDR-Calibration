# 0024 — Windows Store MSIX distribution boundary

Status: Accepted

Date: 2026-08-17

## Decision

Add a Windows 11 x64 MSIX as a distinct Microsoft Store distribution artifact
for the recorded-input Qt GUI and production CLI. The existing per-machine MSI
remains the independently distributed Windows installer governed by decision
0016. It retains its `SELF_SIGNED` and `PUBLIC_TRUST` modes and remains required
by decision 0022 unless a later accepted decision changes that artifact set.

The Store package uses the exact product and publisher identity assigned in
Partner Center. Those non-secret values are explicit build inputs and are not
guessed or represented by committed placeholders. The GUI is the packaged
desktop application and the CLI is exposed as the `sdrcal.exe` app execution
alias. Neither executable requests elevation. The package declares only the
`runFullTrust` restricted capability required for a medium-integrity packaged
desktop application; any additional capability reopens this decision.

Microsoft Store certification supplies the trusted signature for the package
delivered through the Store. That signature does not sign, timestamp, establish
public trust for, or create SmartScreen reputation for an independently
downloaded MSI, EXE, or MSIX. Pre-submission packages remain explicitly
unsigned or locally development-signed and must not be described as certified
or publicly trusted.

## Consequences

MSIX installs and updates by package identity rather than the MSI product and
upgrade codes. Its payload is protected under `WindowsApps`, Store updates are
package-version-driven and atomic, and uninstall follows packaged-app state
rules. This is not the MSI's per-machine Program Files installation, HKLM
registration, administrative maintenance flow, or independently managed
update behavior. MSI and MSIX lifecycle evidence is therefore non-transferable.

The packaged CLI is invoked by its execution alias rather than by a stable
Program Files path. User-selected input and output paths remain supported at
normal user integrity; the package must not depend on writing its installation
directory. Qt remains dynamically deployed. Exact runtime inventory, LGPL
notices, corresponding source, and library-replacement guidance remain release
gates. Store policy or package signing must not be used to weaken those rights.

Store suitability is limited to the recorded-input GUI and CLI. SoapySDR,
vendor modules, services, drivers, elevation, per-machine state, custom install
actions, live-device operation, and RF qualification remain outside this
artifact. Store registration, name reservation, package upload, certification,
and publication are separate external states and evidence gates.
