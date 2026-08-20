# Windows Store replacement-build execution prompt

## Objective

Build and qualify a replacement `0.1.1` Windows Store MSIX from the clean,
upstream-synchronized revision containing the language and logo-audit fixes.
Produce exact artifact and lifecycle evidence suitable for a new owner-binding
decision. Do not upload or alter Partner Center.

## Verified context

- Partner Center rejected the first `0.1.1` upload for unresolved logo paths,
  blank publisher display name, and missing/default package language.
- The rejected draft package was deleted with owner authorization, leaving zero
  package entries and the approved Desktop-only device-family state unchanged.
- Package construction now declares exactly `en-us`, verifies the exact
  publisher display name, parses all three logo declarations from the unpacked
  manifest, resolves their exact payload paths, and verifies PNG type and
  50x50, 150x150, and 44x44 dimensions.
- The Windows packaging and qualification harnesses discover Qt runtime and
  Windows SDK tools without relying on the SSH process `PATH`.

## Scope

1. Confirm local and Windows source trees are clean and synchronized with
   `origin/main` at the same full revision.
2. Use a fresh Windows build/output location; do not reuse or overwrite the
   rejected candidate or prior qualification evidence.
3. Configure and build `windows-store-release` with the already approved Qt
   corresponding-source archive bindings.
4. Run the complete hardware-free CTest suite and package audit.
5. Construct the unsigned Store MSIX and retain construction evidence.
6. Independently inspect the unpacked manifest values, all three declared logo
   paths and dimensions, package version, architecture, byte size, and SHA-256.
7. Run the same-host development-signed install, CLI alias, GUI first-launch,
   GUI relaunch, uninstall, and temporary-certificate cleanup lifecycle from
   the signed-in elevated Windows desktop session.
8. Record the exact source revision and resulting artifact identity without
   treating development signing as Store signing.

## Constraints and non-goals

- Remain hardware-free and network-free except for Git synchronization and the
  existing SSH control channel. Do not access an SDR or start a sample stream.
- Preserve prior candidates and evidence. Do not delete or overwrite them.
- Do not alter version or Store identity merely to obtain a passing result.
- Do not select or upload a package, save Partner Center package state, submit
  for certification, or enable publication.
- Do not claim that local construction or lifecycle qualification proves
  Partner Center acceptance, Store signing, certification, or publication.
- Do not commit generated binaries, private keys, certificates, credentials,
  tokens, or machine-specific secrets.

## Validation and evidence

- Record Windows `git status`, full `HEAD`, and `origin/main` revisions.
- Capture configure, build, CTest, package-audit, MakeAppx pack/unpack, payload,
  manifest, logo, and hash evidence under a new isolated output directory.
- Require all hardware-free tests and package construction checks to pass.
- Require lifecycle qualification to pass every checkpoint and clean up the
  installed package, GUI process, and temporary certificate.
- Copy only durable text/JSON evidence needed for repository review; reference
  the full Windows artifact path rather than committing the MSIX.
- Run the macOS source-contract verifier, matching CTest, Sphinx warnings-as-
  errors build, and `git diff --check` for any repository evidence changes.

## Adversarial review

Attempt to disprove candidate binding by checking for dirty or divergent source,
stale output reuse, mismatched revision/hash/version/architecture, manifest
values not matching Partner Center identity, logo declarations not resolving to
the inspected images, incomplete lifecycle cleanup, and prose that conflates
local qualification with Store acceptance. Correct all actionable findings and
repeat affected checks until clean.

## Exit criteria

A fresh replacement MSIX is bound to one clean pushed revision and one exact
SHA-256; all construction and same-host lifecycle gates pass; cleanup passes;
the repository records a truthful owner-decision packet without package bytes
or secrets; Partner Center remains unchanged; and attributable repository
changes are committed and pushed.
