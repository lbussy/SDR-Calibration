# Phase 14.2 execution prompt

## Objective

Implement the Windows x64 MSI production gate from the committed Phase 14.1
state. Produce a dynamically linked Qt package whose source identity, deployed
runtime, Authenticode signatures and timestamps, MSI payload, and artifact hash
are independently reviewable.

## Required implementation

1. Add a dedicated Windows release configuration for the recorded-input CLI
   and GUI with SoapySDR and vendor modules disabled.
2. Install into an isolated tree and deploy the configured dynamic Qt runtime
   with `windeployqt` while preserving the required runtime layout.
3. Reject dirty source, missing WiX v4, SignTool, Qt deployment tooling,
   certificate thumbprint or HTTPS timestamp service, and unsafe output paths.
4. Preserve deployed executables and DLLs that already have valid timestamped
   vendor signatures; sign and timestamp every unsigned remainder before
   building a per-machine x64 MSI, then sign and timestamp the MSI itself.
5. Administratively extract the MSI and fail on missing required content,
   invalid payload signatures, or CLI startup failure.
6. Retain the exact source revision, OS version/build, architecture, CMake and
   Qt versions, artifact and payload hashes, signature verification, and
   explicit open-gate dispositions without retaining credentials.
7. Update build, testing, licensing, roadmap, status, matrix, user/developer
   guidance, and the decision log without claiming clean-host installation or
   binary-license disposition.

## Non-goals and safety boundary

- Do not access SDR hardware, enumerate devices, start streams, or perform RF
  or calibration qualification.
- Do not bundle SoapySDR or vendor modules.
- Do not implement Ubuntu DEB, Phase 14.4 license disposition, clean-host
  qualification, release publication, or application-store submission.
- Do not treat administrative extraction or Authenticode as clean installation,
  LGPL compliance, device support, or calibration evidence.
- Never accept, copy, export, query, or retain certificate private-key material.

## Validation and adversarial exit gate

Run baseline build/tests/docs, Windows release build/tests/staged-package audit,
PowerShell parsing, WiX build, deployment, signature and timestamp checks, MSI
administrative extraction, payload/startup audit, evidence inspection, and
`git diff --check`. Review the complete diff and artifact adversarially for
credential leakage, unsigned code, flattened or incomplete runtime layout,
unsafe replacement, mutable inputs, false claims, missing failure checks, and
unsafe hardware behavior. Inject every finding below, correct it, and repeat.

Phase 14.2 implementation is complete when hardware-free local checks pass and
the Windows-only production gate is ready to fail closed. Artifact qualification
requires a passing Windows run retained against one exact signed MSI hash;
Phase 14 remains open.

## Injected adversarial findings

Findings discovered during execution are recorded here with their corrective
disposition before the final repeated validation. An empty final list means the
last adversarial pass found no open issue, not that later Phase 14 gates passed.

1. **Timestamp verification was implicit:** the initial signature checks did
   not request SignTool's missing-timestamp warning. Corrected by applying `/tw`
   to staged payload, MSI, and extracted-payload verification and treating its
   warning exit as failure.
2. **Existing vendor signatures would be overwritten:** the initial loop
   re-signed every deployed DLL, destroying valid Microsoft or Qt provenance.
   Corrected by retaining payload signatures that already pass policy and
   timestamp verification, signing only the remainder, and recording one
   path-qualified verification inventory without filename collisions.
3. **Harvested payload feature association was implicit:** the initial WiX
   authoring placed harvested files below the install directory while declaring
   a separate empty feature. Corrected by placing `Files` explicitly inside the
   MSI's required feature and binding it to `INSTALLFOLDER`.
