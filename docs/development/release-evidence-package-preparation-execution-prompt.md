# Release evidence package preparation execution prompt

## Objective

Prepare the fail-closed release evidence package for a future exact SDR
Calibration release candidate. Define its directory layout, manifest and digest
rules, artifact/evidence mapping, assembly order, validation, independent
review, and publication boundary. Upgrade the existing release templates so
they can be populated without ambiguity once a candidate is frozen.

This is a hardware-free preparation slice. It does not freeze a candidate,
populate candidate hashes, build or sign packages, perform qualification,
publish files, tag a revision, or authorize a release.

## Verified context

- Decision 0022 requires an exact source archive, macOS ARM64 DMG, public-trust
  Windows x64 MSI, Raspberry Pi OS 13 ARM64 CLI DEB, SHA-256 checksums, notices,
  inventories, corresponding source, release documents, and an evidence ledger.
- Clean-host results, current historical package records, and retained `0.1.0`
  upgrade baselines exist, but evidence does not transfer to a rebuilt final
  candidate.
- Upgrade/rollback procedures are prepared; execution requires a consistent
  version greater than `0.1.0`.
- Physical end-to-end calibration, Windows public trust, candidate binding,
  upgrade/rollback execution, and final release authorization remain open.
- No accepted decision requires a detached signature over the checksum file,
  and production release-manifest signing/key management is not implemented.

## Required work

1. Create a durable evidence-package assembly plan with one exact candidate
   root, deterministic relative paths, exact artifact identities, SHA-256
   manifests, evidence digests, status vocabulary, completeness rules, privacy
   review, and independent verification.
2. Separate shipped release artifacts from retained qualification evidence.
   Raw IQ, private diagnostics, host paths, credentials, keys, and unnecessary
   device identifiers must not enter the public package.
3. Define a manifest format that binds candidate revision/version, every public
   artifact, every required evidence cell, release documentation, provenance,
   and review disposition without inventing signatures or qualification.
4. Require all candidate-specific fields to remain `TBD-BLOCKING` until exact
   retained evidence exists. Historical values may be cited only as prior
   evidence and never copied into final candidate cells.
5. Correct the checksum template so a detached checksum signature is optional,
   not an unstated release requirement. If later supplied, require its exact
   algorithm, key identity, trust policy, file hash, and independent verification.
6. Split combined lifecycle rows so clean installation, upgrade, rollback, and
   cleanup cannot conceal a partial result.
7. Add assembly, validation, adversarial review, and final-authorization
   checklists to the release template index and active project status.

## Constraints and non-goals

- Do not change product, schema, package, compatibility, signing, or release
  policy beyond correcting unsupported template implications.
- Do not fill a future candidate version, revision, hash, date, URL, approval,
  security contact, or qualification result.
- Do not create binaries, archives, checksums for nonexistent artifacts,
  signatures, tags, releases, or external uploads.
- Do not touch Windows signing, another host, an SDR, a reference, RF hardware,
  a splitter, or a signal generator.
- Do not commit generated Sphinx HTML or local build output.

## Validation and adversarial review

Run `git diff --check` and Sphinx with warnings as errors. Review every required
artifact in decision 0022 against the package layout and ledger. Challenge the
result for historical hashes promoted to candidate values; public artifacts
mixed with private evidence; absolute host paths; unsigned checksums described
as signed; optional signatures made mandatory; combined rows hiding failures;
missing corresponding source, notices, inventories, privacy review, or
published-download verification; a release authorized with any blocking field;
or evidence digests that do not bind the referenced files. Correct every
actionable finding and repeat affected checks.

## Exit criteria

- The evidence-package plan and improved ledger are complete and navigable.
- Every intended artifact and qualification cell has one explicit disposition.
- Candidate-specific values remain blocking.
- Optional checksum signing is represented truthfully.
- Assembly cannot reach `Ready` with missing, failed, stale, private, or
  unreviewed required evidence.
- Documentation validation succeeds and no external or release action occurs.

## Execution result

Completed on 2026-08-17 as a hardware-free preparation slice. The assembly
plan, release index, checksum/evidence ledger, release notes, known limitations,
navigation, readiness ledger, and status were updated. No candidate value,
artifact, checksum, signature, tag, upload, hardware path, or external service
was created or used.

Adversarial review corrected two actionable inherited defects. First, the
ledger had made a detached checksum signature mandatory even though decision
0022 requires SHA-256 checksums and no production checksum-signing policy
exists; the signature is now optional and fail-closed if later enabled. Second,
published-download verification had been made a prerequisite to publication,
which is impossible. The plan now distinguishes `Ready-to-publish` after all
prepublication gates from `Published-verified` after independent download
reconciliation, with withdrawal/correction required on mismatch. Lifecycle
rows were also split so clean install, upgrade, rollback, and cleanup cannot
hide partial failures. After correction, no actionable finding remained within
this preparation slice.
