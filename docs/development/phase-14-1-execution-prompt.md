# Phase 14.1 execution prompt

## Objective

Implement the macOS DMG production gate from the committed Phase 14 portability
foundation. Produce a dynamically linked Qt application package whose identity,
runtime closure, signing, notarization, stapling, Gatekeeper disposition, source
revision, and artifact hash are independently reviewable.

## Required implementation

1. Give the application a stable bundle identifier and version metadata.
2. Add a dedicated macOS release configuration that builds the recorded-input
   production CLI and GUI without enabling or packaging SoapySDR or vendor
   modules.
3. Install into an isolated staging tree and deploy the configured dynamic Qt
   runtime with `macdeployqt`.
4. Reject a dirty source tree, missing Developer ID identity or notarization
   profile, missing tools, and non-system runtime dependencies that retain
   absolute build, Homebrew, or other host paths.
5. Sign the application, deployed Qt closure, command-line executable, and DMG
   with Developer ID, hardened runtime where applicable, and secure timestamps.
6. Submit with `notarytool`, require an `Accepted` result, staple and validate
   the ticket, and require Gatekeeper assessment of the DMG and mounted app.
7. Retain the exact source revision, platform, architecture, Qt version, DMG
   SHA-256, notarization result, signing details, and verification output without
   copying credentials into the build tree or evidence.
8. Update build, testing, licensing, roadmap, status, matrix, user, and decision
   documentation without claiming clean-host installation or binary-license
   disposition.

## Non-goals and safety boundary

- Do not access SDR hardware, enumerate devices, start streams, or perform RF or
  calibration qualification.
- Do not bundle SoapySDR, vendor modules, or hardware runtime libraries.
- Do not implement Windows MSI, Ubuntu DEB, the Phase 14.4 license-disposition
  gate, clean-host qualification, or release publication.
- Do not treat notarization as clean-install, LGPL compliance, device support,
  or calibration evidence.
- Keep credentials in normal Apple tooling and do not query Keychain directly.

## Validation and adversarial exit gate

Run the baseline build/tests/docs, the macOS release build/tests/staged-package
audit, script syntax, bundle-metadata inspection, runtime-closure audit, signing,
notarization, stapling, Gatekeeper checks, evidence inspection, and
`git diff --check`. Review the complete diff and artifact adversarially for
unsigned nested code, host-path leakage, credential leakage, ambiguous identity,
mutable or dirty inputs, false qualification claims, missing failure checks, and
unsafe hardware behavior. Inject every finding below, correct it, and repeat.

Phase 14.1 is complete only when a DMG at the committed revision passes every
check and retained evidence identifies its exact hash. Phase 14 remains open.

## Injected adversarial findings

Findings discovered during execution are recorded here with their corrective
disposition before the final repeated validation. An empty final list means the
last adversarial pass found no open issue, not that later Phase 14 gates passed.

1. **Unsafe output deletion boundary:** the initial script accepted an arbitrary
   output path before recursive replacement. Corrected by requiring the output
   directory to be a strict child of the configured build directory.
2. **Incomplete runtime inspection:** executable permission bits alone are not a
   sufficient Mach-O classifier. Corrected by identifying every staged Mach-O,
   retaining its hash and load commands, and auditing that complete inventory.
3. **Application ticket not stapled:** notarizing only the containing DMG did not
   retain a stapled application ticket. Corrected with a separate application
   submission, acceptance check, staple, and validation before DMG assembly.
4. **Insufficient environment evidence:** the initial manifest omitted exact OS
   build and toolchain inputs. Corrected by recording macOS version/build, CMake,
   Xcode, architecture, Qt version, runtime closure, and artifact hash.
5. **Incorrect deployment floor:** project code compiled for 13.3, but the
   configured Qt 6.11.1 frameworks require 14.0 and produced linker warnings.
   Corrected by raising this package preset to 14.0 and documenting the exact-
   input basis without making a general support claim.
