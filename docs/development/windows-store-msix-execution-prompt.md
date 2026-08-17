# Windows Store MSIX execution prompt

## Objective

Create a separate Windows x64 Store-MSIX path that can receive Microsoft's
Store-delivered signature after certification, without changing or replacing
the independently distributed MSI.

## Verified context and scope

- Microsoft Store MSIX submissions are re-signed by Microsoft after
  certification; Store MSI/EXE submissions must already be publisher-signed.
- The package contains only the recorded-input Qt GUI and production CLI.
- The GUI runs as a medium-integrity full-trust desktop application. The CLI is
  exposed as an app execution alias.
- No SDR, stream, device, network, calibration, RF, publication, or submission
  action belongs to this implementation slice.

## Requirements

1. Preserve the MSI target and both of its signing modes unchanged.
2. Source-bind the exact Partner Center package name, publisher, publisher
   display name, and reserved product name as non-secret identity values that
   cannot be changed accidentally at configure time.
3. Fail on blank, guessed, example, or placeholder identity; dirty or
   upstream-unsynchronized source; version drift; output reuse; unsafe output;
   missing tools, Qt runtime, plug-ins, license material, or corresponding
   source; and unexpected executable or DLL payloads.
4. Bind project version `x.y.z` to MSIX version `x.y.z.0`, x64 architecture,
   product-facing names, icons, GUI identity, CLI alias, runtime inventory, and
   exact payload hashes.
5. Declare only `runFullTrust`. Prohibit elevation, broad file-system access,
   services, drivers, custom install actions, and unreviewed capabilities.
6. Keep Qt dynamically deployed and retain exact LGPL source disposition and
   replacement instructions.
7. Record pre-submission signing and certification truthfully. Do not claim a
   Microsoft signature until the exact Store-delivered package is independently
   verified.

## Validation and evidence

Run deterministic source-contract tests, the Windows configure/build/CTest and
package-audit baseline, MakeAppx manifest/schema validation, package creation
and unpacked inventory checks, Qt/license checks, CLI startup through the alias,
and `git diff --check`. On a separately authorized suitable Windows host, retain
exact tool/OS versions, identity, source revision, package filename/size/hash,
manifest/capabilities, inventories, installation, first launch, relaunch, CLI,
update, uninstall, cleanup, icon review, and signature observations.

After certification, obtain the exact Store-delivered artifact where Microsoft
permits, then independently verify identity, version, publisher, signature
chain and validity, payload, launch, Store update/uninstall behavior, and the
absence of unexpected capabilities or files.

## Adversarial review and exit

Challenge MSI/MSIX signing conflation, Store versus standalone trust, premature
certification/publication claims, self-signing claims, CLI loss, identity or
version drift, elevation/per-machine assumptions, runtime or LGPL omissions,
secrets, SmartScreen overclaiming, and lifecycle evidence transfer. Correct all
actionable findings. Completion requires a reviewed implementation and tests;
the Windows public-trust release gate remains open until the exact certified
Store-delivered package signature and lifecycle evidence are retained.
