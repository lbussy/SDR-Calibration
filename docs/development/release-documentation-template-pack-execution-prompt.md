# Release-documentation template-pack execution prompt

## Objective

Create a hardware-free template pack for the initial release notes, known
limitations, upgrade/rollback guidance, vulnerability reporting, checksums, and
evidence ledger. Pre-populate only stable, verified project facts and use
conspicuous blocking placeholders for candidate-specific facts that do not yet
exist.

This slice prepares reviewable release inputs. It does not freeze or designate
a candidate, construct artifacts, or publish a release.

## Verified starting point

- `main` is at `236b2c4` after decision 0022 accepted the initial compatibility
  and release-artifact boundary.
- The intended binary set is a recorded-input-only macOS ARM64 DMG, public-trust
  Windows 11 x64 MSI, and Raspberry Pi OS 13 ARM64 CLI DEB, accompanied by
  source, checksums, notices, inventories, corresponding-source material, and
  exact evidence.
- Existing macOS, Windows, and Raspberry Pi evidence applies to historical exact
  artifacts and cannot be copied into a future candidate ledger as though it
  qualified new hashes.
- Physical end-to-end calibration, fresh-host lifecycles, prior-version
  upgrade/rollback, Windows public trust, candidate binding, and final ledger
  population remain open.
- User-owned macOS icon-composer work is present and must remain outside this
  slice's staged changes.

## Required execution

1. Create a navigable release-template section under `docs/development`.
2. Provide separate templates for release notes, known limitations,
   upgrade/rollback, vulnerability reporting, and checksums/evidence ledger.
3. Mark every candidate-specific version, revision, artifact name, hash, host,
   result, date, URL, signature, and contact channel as `TBD-BLOCKING` until
   exact retained evidence exists.
4. Pre-populate stable boundaries from accepted decisions: authoritative native
   profile, schema-major-1 compatibility, recorded-input-only binary scope,
   supported target intent, unsupported Ubuntu path, lossy WSJT-X adapters, and
   withheld install/upgrade/device/accuracy claims.
5. Make the ledger distinguish required release artifacts from qualification
   evidence and include an independent-download verification stage.
6. Update readiness/status/navigation to say templates exist while final
   candidate population and acceptance remain open.

## Constraints and non-goals

- Do not create final release notes, a changelog, a release version, a frozen
  revision, artifact hashes, signatures, evidence results, or publication URLs.
- Do not reuse a historical artifact hash as a future candidate placeholder.
- Do not build, package, sign, notarize, install, access external services,
  change `SECURITY.md`, or select a vulnerability-reporting destination.
- Do not access hardware or perform RF, clean-host, upgrade, or rollback work.
- Do not modify or stage icon assets, generators, packaging, application source,
  or concurrent user documentation changes outside this template pack.

## Validation and adversarial review

Run Sphinx with warnings as errors and `git diff --check`. Review for accidental
release designation, stale-hash promotion, missing required artifact rows,
ambiguous placeholders, public self-signed Windows claims, absent corresponding-
source/license gates, unsupported Ubuntu inclusion, live-device binary claims,
unsafe public vulnerability-reporting instructions, missing rollback evidence,
and staging of unrelated work. Correct every actionable finding and repeat the
checks.

## Exit criteria

The template pack is navigable, every missing candidate fact fails visibly
closed as `TBD-BLOCKING`, stable facts agree with decision 0022, readiness
documentation distinguishes template completion from RC-08 completion,
validation passes, and only attributable documentation is staged for commit and
push.
