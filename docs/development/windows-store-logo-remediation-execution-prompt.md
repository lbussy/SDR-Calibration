# Windows Store logo-remediation hardening execution prompt

## Objective

Close the source-audit gap exposed by Partner Center's rejection of the first
`0.1.1` MSIX. Make package construction fail closed unless all three logo
references in the unpacked manifest resolve to the intended PNG payload files
with the required dimensions. State clearly that local validation cannot prove
Partner Center acceptance.

## Verified context

- The rejected package reported unresolved package, 150-pixel, and 44-pixel
  logo references, a blank publisher display name, and no supported/default
  language.
- The rejected package evidence already recorded logo files in its payload.
- The current source declares `en-us` and checks three hard-coded unpacked asset
  paths, but it does not derive those paths from the unpacked manifest or open
  the referenced files as images.
- The rejected draft package has been deleted with owner authorization. The
  submission currently has no package entry.

## Scope

1. Parse the package `Properties/Logo` and the one application's
   `uap:VisualElements` logo attributes from the unpacked manifest.
2. Require the exact intended package-relative paths.
3. Resolve each declared path inside the unpacked payload.
4. Require decodable PNG images at exactly 50x50, 150x150, and 44x44 pixels.
5. Extend the source-contract verifier so removal of any audit fails tests.
6. Correct readiness and status prose to distinguish prevention from proof.

## Constraints and non-goals

- Preserve the existing source artwork and image-generation behavior.
- Do not add a dependency or Qt module.
- Do not build or qualify a replacement MSIX in this slice.
- Do not select, upload, save, certify, or publish anything in Partner Center.
- Do not claim that the prior Partner Center errors are resolved. Only
  acceptance of a newly built and fully qualified replacement can establish
  that result.

## Validation and evidence

- Run `python3 scripts/verify-windows-store-msix.py --source-dir . --project-version 0.1.1`.
- Run `ctest --preset default --output-on-failure -R windows_store_msix_contract_tests`.
- Build the documentation with warnings treated as errors.
- Run `git diff --check` and review the complete diff.
- Report checks unavailable on macOS rather than claiming they passed.

## Adversarial review

Confirm that the audit reads the unpacked manifest rather than trusting the
source template, covers exactly all three required declarations, verifies file
type and dimensions after resolving each declaration, disposes image handles,
and contains no statement equating local checks with Store acceptance. Correct
all actionable findings and repeat the applicable checks.

## Exit criteria

The source contract fails closed for a missing, renamed, misreferenced,
non-PNG, or incorrectly sized declared logo; documentation truthfully retains
Partner Center acceptance as an open gate; the narrow validation suite passes;
and only attributable files are committed and pushed when authorized.
