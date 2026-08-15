# 0015 — macOS DMG deployment and signing boundary

Status: Accepted

Date: 2026-08-15

## Decision

Package the macOS recorded-input CLI and Qt Widgets application in a Developer
ID-signed and Apple-notarized HFS+ DMG. Deploy Qt as dynamic frameworks using the
`macdeployqt` from the configured Qt installation. Require hardened runtime and
secure timestamps, an accepted notarization response, a stapled ticket, direct
Gatekeeper assessment, and retained hash and verification evidence.

Set the package deployment target to macOS 14.0 for the current Qt 6.11.1
package input. AppleClang's standard library establishes 13.3 as the project-
code floor for floating-point `std::to_chars`, but the configured Qt frameworks
were built for macOS 14.0. The higher linked-input floor governs this exact
package; neither value is a broader operating-system support claim.

Keep SoapySDR and vendor modules out of this package slice. Refuse dirty source
inputs and absolute non-system runtime dependencies. Resolve signing and notary
credentials through normal Apple tools; never place credentials in CMake cache,
command output, evidence, or repository files.

## Consequences

The resulting evidence can establish signing and notarization for one exact DMG
hash. It does not establish clean-host installation, binary-license disposition,
general macOS support, device support, or calibration accuracy. The later
license gate must audit the deployed Qt closure and supply notices, license
texts, source disposition, and replacement instructions before publication.
