# 0025 — Store MSIX is the primary Windows release artifact

Status: Accepted

Date: 2026-08-17

Supersedes in part: [0022](0022-initial-compatibility-and-release-artifacts.md)
and [0024](0024-windows-store-msix-distribution.md)

## Decision

The required Windows 11 x64 artifact for the initial release is a Microsoft
Store-delivered MSIX containing the recorded-input GUI and CLI. The exact
candidate must pass package, runtime, license, Store certification, Microsoft
signature, Store delivery, clean-install, launch, CLI-alias, removal, and
cleanup gates before the Windows release cell can pass.

The WiX MSI remains implemented but is not a required release artifact. A
locally self-signed MSI may be built and used for bounded development or test
work when its temporary trust and limitations are explicit. It must not be
published as a release download, described as publicly trusted, or substituted
for Store-MSIX qualification. Public-trust MSI signing and timestamping are no
longer release gates.

MSI and MSIX use independent package identities and servicing models. An MSI
installation is not a prior Store version and cannot qualify Store update or
rollback. Before release, test and document coexistence, user-state
preservation, CLI resolution, Start-menu identity, and the supported manual
migration procedure from a test MSI to the Store MSIX. The release must not
claim automatic MSI removal, in-place conversion, or MSI rollback.

For the first Store release, a prior Store version does not exist. The Windows
prior-version Store-update cell is therefore `Not applicable — no published
Store predecessor`, with that fact independently verified. Future releases
must qualify Store-managed update from the previous supported Store version.
Rollback remains Store-controlled and is not promised unless a future accepted
decision and retained evidence establish it.

## Consequences

Partner Center reservation and local development-signed lifecycle evidence do
not satisfy the release gate. The exact frozen candidate must be uploaded,
certified, signed by Microsoft, installed from the Store on a clean supported
host, exercised, removed, and reconciled with its retained evidence. Store
submission, certification, and publication remain separately authorized
external actions.

Release notes, dependency inventories, corresponding-source materials, and the
evidence ledger bind the pre-submission candidate hash to the certified Store
identity/version and observed Microsoft signature. `SHA256SUMS` covers files
actually shipped outside the Store; it does not invent a downloadable Store
binary. The Store listing and privacy/capability declarations are release inputs and must agree
with the recorded-input-only product boundary and sole `runFullTrust`
capability.
