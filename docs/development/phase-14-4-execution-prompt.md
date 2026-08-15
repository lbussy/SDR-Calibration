# Phase 14.4 execution prompt

## Objective

Close the distribution-license engineering gate for the exact macOS, Windows,
and Ubuntu payloads without treating legal disposition as clean installation,
platform support, device support, or release publication.

## Required implementation

1. Audit the actual deployed-code inventory rather than the configured link
   graph or an assumed Qt module list.
2. For macOS and Windows, require the complete qtbase source archive matching
   the deployed Qt version and a caller-pinned SHA-256; reject a mismatched,
   incomplete, or differently versioned archive.
3. Carry that source archive, its license and REUSE metadata, a payload
   inventory, an exact machine-readable disposition, and shared-library
   replacement instructions with each binary package.
4. Record that Qt is unmodified, dynamically linked under LGPL-3.0-only, and
   that SoapySDR and vendor modules are absent. Reject evidence to the contrary.
5. For Ubuntu, prove that no third-party shared library is conveyed, retain the
   exact generated dependency clauses, resolve each clause to an installed
   package/version, and fail when its Debian copyright record is unavailable.
6. Update package manifests and documentation so the exact-payload gate can be
   passed while clean-host, platform, device, calibration, and publication gates
   remain separate.

## Non-goals and safety boundary

- Do not download source during packaging, access SDR hardware, enumerate
  devices, stream samples, transmit RF, publish a release, or perform legal
  review.
- Do not accept a source URL or written promise in place of the complete,
  locally supplied and hash-verified source archive for bundled Qt.
- Do not claim that source availability, extraction, signing, or startup on a
  build host establishes clean installation or general platform support.

## Validation and adversarial exit gate

Run baseline build/tests/docs, script syntax and parser checks, synthetic
positive and fail-closed disposition checks, each available platform package
gate, package-content inspection, and `git diff --check`. Review the complete
diff adversarially for guessed dependencies, source/version mismatch, missing
license metadata, incomplete runtime inventory, nonreplaceable libraries,
misclassified system dependencies, false claims, and unsafe behavior. Inject
and close every finding, then repeat.

Phase 14.4 implementation is complete when the gates fail closed and the exact
payload disposition travels with each package. A macOS or Windows artifact is
not disposition-qualified until its own retained run supplies the pinned source
archive and passing manifest. The retained Ubuntu run must likewise be repeated
at the Phase 14.4 revision. Clean-host installation remains the next Phase 14
gate.

## Injected adversarial findings

1. **A source URL is not corresponding source:** the initial design could have
   recorded a mutable retrieval location without conveying source. Corrected by
   requiring and packaging the complete, SHA-256-pinned qtbase source archive.
2. **Repository notices cannot describe an exact payload:** the initial notice
   named permitted modules but did not bind them to deployed files. Corrected
   by hashing and packaging the actual runtime inventory and generating a
   payload-local manifest.
3. **Ubuntu non-bundling did not identify dependency evidence:** merely stating
   that Qt comes from the system left the exact package disposition implicit.
   Corrected by resolving every generated dependency clause to an installed
   package/version and requiring its Debian copyright record.
4. **Platform signatures could be mistaken for a replacement prohibition:**
   replacement guidance did not explain the effect of modifying signed files.
   Corrected with platform-specific extraction/replacement and local re-signing
   guidance that separates original signature validity from user modification.
5. **Windows deployment could silently add a separately licensed runtime:**
   `windeployqt --compiler-runtime` could copy Microsoft compiler libraries, and
   software OpenGL or another non-Qt DLL could enter without corresponding
   disposition. Corrected by excluding compiler-runtime/software-OpenGL
   deployment and rejecting every DLL outside the exact Qt naming family.
6. **The macOS load-command parser also consumed evidence headers:** the first
   dependency audit parsed `FILE` and SHA-256 lines as library paths. Corrected
   by accepting only indented `otool -L` dependency records.
7. **Pre-signing hashes did not describe the shipped code:** the first macOS
   and Windows inventories were generated before project binaries received
   their final signatures. Corrected by signing first and hashing the exact
   post-signing deployed code consumed by installer construction.
8. **Non-Qt libraries were rejected only on Windows:** macOS and Ubuntu could
   still acquire an unexpected bundled shared library without reopening its
   disposition. Corrected by allowlisting project/Qt Mach-O paths on macOS and
   rejecting every conveyed shared-object file in the Ubuntu stage.
9. **The first real macOS run exposed incomplete module source and invalid
   nested signing:** the official SDK deployed QtSvg plug-ins while the gate
   accepted only qtbase source, and `macdeployqt` signing left a modified
   framework signature. Corrected by requiring hash-pinned qtsvg source when
   QtSvg is present, providing the official plug-in framework compatibility
   path, disabling deployment-time signing, and signing plug-ins, frameworks,
   executables, and the application explicitly from the inside out.
10. **Static trust checks did not prove that the application could load Qt:**
    the first notarized DMG passed signature, stapling, and Gatekeeper
    assessment while its main executable had no `LC_RPATH`; launch therefore
    failed before application code ran. Corrected by installing and verifying
    `@executable_path/../Frameworks` before signing the application and by
    requiring a bounded launch of the mounted application to remain alive.
