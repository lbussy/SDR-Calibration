# Prior-version baseline retention execution prompt

## Objective

Preserve the existing exact `0.1.0` macOS, Windows, and Raspberry Pi installable
artifacts as content-addressed, read-only prior-version baselines before any
future candidate version increment. Verify every source artifact against its
retained evidence, copy without rebuilding or modifying it, independently
verify the retained copy, and record its location and limitations.

This slice creates upgrade/rollback test inputs. It does not qualify an upgrade,
designate a release, increment a version, or publish an artifact.

## Verified starting point

- The macOS DMG exists locally at the exact hash recorded for source `5eb3670`.
- The Windows MSI exists on `ecm-mule` at the exact hash recorded for source
  `15ed17b` and uses the retained locally trusted self-signed development mode.
- The Raspberry Pi ARM64 DEB exists on `wspr4` at the exact hash recorded for
  source `5f50d31`.
- All three artifacts report application/package version `0.1.0`. They become
  prior-version inputs only for a future candidate with a strictly higher
  version.
- The repository is clean and synchronized at `05b3a51`. No artifact belongs in
  Git, and no release publication is authorized.

## Required execution

1. Recompute each source artifact's SHA-256 and stop on any evidence mismatch.
2. Create one explicit host-local retention root per platform outside its build
   tree, organized by version and full SHA-256.
3. Refuse to overwrite an existing retained filename. If it already exists,
   accept it only after an exact hash match.
4. Copy the original artifact byte-for-byte, make the retained file read-only,
   and recompute its SHA-256 from the retention location.
5. Record exact filename, version, source revision, SHA-256, signing/trust state,
   host, source path, retained path, verification result, and limitations in a
   committed baseline manifest.
6. Update the readiness ledger: baseline selection is complete, while fresh-host
   upgrade/rollback execution and selection of a higher-version candidate remain
   open.

## Exact baselines and retention roots

- macOS ARM64 DMG, SHA-256
  `94a831d4549b92edd85222c55e0cd64395dbe8acfd1f4bd5c57351f15bf80ad4`,
  retained below `/Users/lbussy/SDR-Calibration-Release-Baselines/0.1.0/`.
- Windows x64 MSI, SHA-256
  `7be98c92de35bb31024152161f9b3bb4e76cbf6a3d2728001060f3d338d81973`,
  retained below
  `C:\Users\lee\SDR-Calibration-Release-Baselines\0.1.0\` on `ecm-mule`.
- Raspberry Pi ARM64 DEB, SHA-256
  `d3de3d6397aaec7c3959e8854ac1871a787e4850706ae7736761a3fa887605ef`,
  retained below `/home/pi/SDR-Calibration-Release-Baselines/0.1.0/` on
  `wspr4`.

Each artifact is stored in a child directory named by its full SHA-256.

## Constraints and non-goals

- Do not rebuild, re-sign, rename internally, mutate metadata, or reconstruct a
  missing artifact from source.
- Do not overwrite, delete, or move the original package or any unrelated file.
- Do not increment `PROJECT_VERSION`, freeze a new candidate, install a package,
  run maintenance/upgrade/rollback, or alter user application state.
- Do not publish, upload to GitHub, create a release/tag, or treat host-local
  retention as off-host backup or public availability.
- Do not access SDR hardware, references, antennas, or RF paths.

## Validation and adversarial review

Verify source and destination hashes independently, file read-only state, exact
content-addressed paths, source/evidence revision agreement, absence of staging
or package modifications, and `git diff --check`. Run Sphinx with warnings as
errors. Review for same-version maintenance conflated with upgrade, self-signed
Windows trust promoted to public trust, stale hash/path transcription, build-
tree-only retention, accidental binary staging, overwrite behavior, and any
claim of upgrade qualification. Correct every actionable finding and repeat
affected checks.

## Exit criteria

All three exact `0.1.0` artifacts have verified read-only content-addressed
copies outside their build trees; the committed manifest makes their identity,
location, trust state, and host-local retention limitation reviewable; the
future candidate remains unversioned and unfrozen; upgrade/rollback remains an
open fresh-host gate; and only attributable documentation is committed and
pushed.
