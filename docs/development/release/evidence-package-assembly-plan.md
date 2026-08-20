# Release evidence package assembly plan

Status: Prepared — no candidate is bound and no package is assembled

## Purpose

This plan defines the reviewable package that accompanies one exact frozen
release candidate. It distinguishes public release materials from retained
qualification evidence and prevents historical or partial results from being
promoted into a release claim.

The package reaches `Ready-to-publish` only when every prepublication field is
populated from retained evidence, every digest is independently recomputed,
every prepublication gate passes, privacy review passes, and publication
authorization is explicit. `TBD-BLOCKING`, `Missing`, `Failed`, `Blocked`,
`Stale`, or `Unreviewed` anywhere in the required prepublication matrix prevents
that state. It reaches `Published-verified` only after an independent download
of every published file is reconciled with the approved package.

## Candidate root and layout

Use a new directory named with the exact candidate version and abbreviated
source revision. Refuse an existing non-empty destination.

```text
SDR-Calibration-<version>-<revision>/
├── README.md
├── manifest.json
├── SHA256SUMS
├── artifacts/
│   ├── source/
│   ├── macos-arm64/
│   ├── windows-x64/
│   └── raspberry-pi-arm64/
├── corresponding-source/
├── notices/
├── inventories/
├── release-docs/
└── evidence-index/
```

The public root contains release artifacts and public, privacy-reviewed index
documents. Full qualification logs, raw IQ, host inventories, private security
reports, signing credentials, keys, tokens, and sensitive device details remain
outside the public root. `evidence-index/` contains relative references,
digests, scope, result, and reviewer identity for retained evidence; it does not
silently copy private evidence into the release.

## Required public artifacts

| Class | Required content | Completion rule |
| --- | --- | --- |
| Source | Archive made from the exact frozen revision | Archive contents and independently computed digest match the revision |
| macOS | Apple Silicon DMG with recorded-input GUI/CLI | Exact package, signing/notarization, payload, license, lifecycle, and candidate evidence pass |
| Windows | Microsoft Store Windows 11 x64 MSIX with recorded-input GUI/CLI | Exact certification, Microsoft signature, Store delivery, payload, license, lifecycle, migration, and candidate evidence pass |
| Raspberry Pi | Raspberry Pi OS 13 ARM64 CLI-only DEB | Exact payload, resource, license, lifecycle, and candidate evidence pass |
| Checksums | `SHA256SUMS` over every shipped file other than itself | Sorted relative paths, lowercase SHA-256, no duplicate or escaping path |
| Notices | MIT license and exact third-party notices | Contents reconcile with all conveyed files |
| Inventories | Per-artifact runtime/payload inventory and SBOM | Exact artifact digest and dependency disposition are bound |
| Corresponding source | Hash-pinned source required by conveyed dependencies | Completeness and replacement instructions independently reviewed |
| Release documents | Notes, limitations, compatibility, lifecycle, security, verification, and ledger | Cross-document identities and claims agree |

An optional detached signature for `SHA256SUMS` may be included only after an
accepted signing policy identifies the algorithm, signing key, public trust
material, verification procedure, signature filename/hash, and independent
verification. Its absence is not currently a failure because decision 0022
requires SHA-256 checksums but does not establish release-manifest signing.

## Manifest contract

`manifest.json` is a UTF-8 JSON document with no duplicate keys. Before final
assembly, freeze its schema or exact structural validator. It must contain:

- manifest type and version;
- candidate version, full source revision, tree state, freeze decision, and UTC
  assembly interval;
- one record per public file with relative path, byte size, lowercase SHA-256,
  media type, artifact class, platform/architecture where applicable, source or
  supplier, license/disposition reference, and inclusion reason;
- one record per required qualification cell with exact combination, result,
  retained evidence locator, evidence digest, reviewer, and review date;
- release-document identities and digests;
- privacy-review result and excluded private-material summary;
- independent verification result; and
- final authorization state and authorizer.

Paths must be normalized relative paths using `/`, with no absolute paths,
drive letters, `..`, empty components, control characters, or duplicate
case-folded destinations. A manifest may describe itself by a separately
defined canonical-payload digest only after that rule is frozen; otherwise it
must not claim a self-hash.

## Evidence-cell matrix

Record these as separate cells for every exact final candidate:

1. frozen source and version consistency;
2. hardware-free build and focused tests per platform;
3. macOS payload/license/signing/notarization;
4. macOS clean install;
5. macOS upgrade;
6. macOS rollback and cleanup;
7. Windows MSIX payload/license/Store certification/Microsoft signature;
8. Windows clean install;
9. Windows Store update or independently verified no-predecessor N/A;
10. Windows MSI migration/coexistence and cleanup;
11. Raspberry Pi payload/resource/license;
12. Raspberry Pi clean install;
13. Raspberry Pi upgrade;
14. Raspberry Pi rollback/removal and cleanup;
15. RC-WF final package binding for the user-directed calibration workflow;
16. optional physical end-to-end calibration for every device claim actually
    proposed by the candidate, or an independently reviewed no-claim
    disposition;
17. application/icon visual review for packaged GUI platforms;
18. compatibility and schema review;
19. notices, inventory, corresponding-source, and privacy review;
20. independent local package verification before publication; and
21. independent published-download verification after publication.

Evidence is candidate-specific unless its governing contract explicitly makes
it durable. Clean-host or visual evidence for a historical hash cannot qualify
a rebuilt final package.

## Assembly sequence

1. Confirm the candidate is frozen, clean, synchronized, version-consistent,
   and approved for candidate construction.
2. Create the new empty root and record the assembly environment without
   secrets or unnecessary host identifiers.
3. Copy only exact approved artifacts; independently compute size and SHA-256
   after copying. Never rebuild or repair an artifact during assembly.
4. Add notices, inventories, corresponding source, and replacement guidance;
   reconcile them against the exact binary payloads.
5. Populate release documents from evidence. Do not remove a limitation or
   blocking field merely because work is planned.
6. Populate the evidence index with exact retained locators and digests. Keep
   private evidence outside the public root.
7. Generate sorted `SHA256SUMS`, validate every entry, and independently
   recompute it from a separate traversal.
8. Populate and structurally validate `manifest.json`; cross-check it against
   the filesystem, checksums, ledger, and release documents.
9. Perform privacy and credential scanning plus manual review.
10. Have an independent reviewer recompute all public file hashes, sample or
    fully verify retained evidence as required, and reconcile all claims.
11. Mark the package `Ready-to-publish` only after every required
    prepublication cell passes and publication authorization is recorded.
12. After publication, independently download and reconcile every published
    file before marking `Published-verified`. A mismatch requires immediate
    withdrawal or correction under an explicit incident record; never approve
    the differing files retroactively.

## Validation checklist

- Root is new, exact-candidate-named, and contains only allowed paths.
- No symlink, special file, absolute path, build-tree path, credential, private
  key, token, personal information, or unnecessary device identifier exists.
- Every public file appears exactly once in the manifest and `SHA256SUMS` as
  applicable; every recorded size and digest matches.
- Artifact version, revision, platform, architecture, filename, signing state,
  package inventory, and documentation agree.
- Corresponding source matches every conveyed LGPL-covered Qt component and
  includes replacement instructions.
- The Windows MSIX has the exact certified Store identity/version and Microsoft
  signature; local development signatures and self-signed MSI evidence are not
  substituted.
- Each clean-install, upgrade, rollback, cleanup, physical calibration, and
  visual-review cell is distinct and bound to the exact artifact.
- Release notes and limitations do not imply live-device binaries, unsupported
  platforms, automatic profile activation, WSJT-X mutation, or calibration
  accuracy beyond retained evidence.
- Published-download verification is `Pending publication` at the
  `Ready-to-publish` gate. It cannot be pre-completed from local assembly and
  must pass before the release is marked `Published-verified`.

## Failure and change control

Any artifact byte change, rebuilt package, signing/certification change,
candidate revision change, manifest/checksum correction, evidence reversal, or
required-document change invalidates affected digests and reopens every
dependent review. Never patch an assembled package in place and retain its
prior approval. Create a new candidate root or new assembly attempt with clear
supersession records.
