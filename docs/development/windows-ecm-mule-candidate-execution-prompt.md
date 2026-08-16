# Windows `ecm-mule` candidate execution prompt

## Objective

Prepare the Windows 11 x64 test host `ecm-mule` for the exact Phase 14.2 and
Phase 14.5 candidate path, then run the narrowest hardware-free source validation
that the available inputs permit. Preserve the accepted signed-MSI boundary and
stop fail-closed before packaging if the required signing certificate is absent.
Use the current locally trusted self-signed development mode without making a
public-trust or release-signing claim.

## Required execution

1. Connect through the existing key-authenticated SSH alias and confirm the host
   name, Windows edition/build, x64 architecture, available storage, and absence
   of a pending reboot.
2. Inventory Visual Studio C++ components, CMake, Git, Ninja, WiX v4, the Windows
   SDK SignTool, Qt 6.11.1 MSVC 2022 x64, `windeployqt`, and code-signing
   certificates without exposing credentials or private-key material.
3. Install only missing user- or host-level build dependencies required by the
   committed Windows release preset. Use versioned, attributable upstream
   packages; record exact versions and paths. Do not install SDR drivers,
   SoapySDR, or vendor modules.
4. Create a fresh project checkout from the canonical GitHub repository at the
   exact authorized `main` revision. Do not reuse or alter another project
   checkout. Require a clean tree synchronized with `origin/main` before and
   after validation.
5. Configure and build the Windows release preset with SoapySDR disabled, run
   its hardware-free CTest suite, and run the staged package audit and branding
   contract checks. Retain console output and an environment summary outside
   the Git worktree.
6. Create the project development certificate with the committed helper when no
   suitable project certificate exists. Require its non-exportable private key
   in the current user's personal store and its exact public certificate in the
   current user's Trusted People store. Record only its subject, thumbprint,
   validity, and store disposition. Never transfer, export, query, or retain
   private-key material.
7. Run the signed MSI target only when WiX v4, SignTool, `windeployqt`, complete
   corresponding Qt source archives, and the locally trusted certificate
   selected by thumbprint are all present. Do not provide a timestamp URL in
   self-signed mode.
8. If a signing input is absent, report the exact open gate and stop. Do not
   generate or describe an unsigned MSI as a candidate, and do not describe the
   self-signed candidate as publicly trusted or release-signed.

## Safety and non-goals

- Do not enumerate, open, configure, or stream from SDR hardware.
- Do not perform RF, device, frequency-accuracy, or calibration qualification.
- Do not publish a release or alter GitHub settings, secrets, or workflows.
- Do not weaken Authenticode, mode-specific signing, exact-payload
  license-disposition, or evidence requirements to obtain a nominally passing
  artifact.
- Host preparation and hardware-free tests do not establish package installation
  or Windows support. Only an exact mode-labelled signed MSI can enter the
  lifecycle gate.

## Exit gate

Success for this bounded preparation slice requires an identified Windows 11
x64 host; a clean exact-revision checkout; recorded compiler, CMake, Qt, Ninja,
WiX, SDK, and signing-input disposition; and passing configure, build, CTest,
staged-package, and branding checks. If the signing certificate is unavailable,
the signed MSI, installation, launch, upgrade, uninstall, and clean-state gates
remain explicitly open.
