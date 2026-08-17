# Candidate upgrade and rollback record

Status: Template only — no upgrade or rollback is qualified

Governing procedure: [upgrade and rollback qualification plan](../upgrade-rollback-qualification-plan.md)

## Bound transition

- Platform, architecture, OS/build, and host identity: `TBD-BLOCKING`
- Maintenance-window owner and UTC interval: `TBD-BLOCKING`
- Prior version and source revision: `TBD-BLOCKING`
- Prior artifact filename, size, SHA-256, and retention path: `TBD-BLOCKING`
- Prior signing/trust state: `TBD-BLOCKING`
- New version and frozen source revision: `TBD-BLOCKING`
- New artifact filename, size, SHA-256, and evidence path: `TBD-BLOCKING`
- New signing/trust state: `TBD-BLOCKING`
- Platform procedure selected: `TBD-BLOCKING`

The prior artifact must be the exact retained `0.1.0` baseline and the new
version must be strictly greater. Same-version maintenance, a clean install,
or replacement with unbound files is not upgrade evidence.

## Entry-gate evidence

- Clean/synchronized frozen candidate revision: `TBD-BLOCKING`
- Version-consistency report across source, executable, bundle/package,
  filename, SBOM, and evidence: `TBD-BLOCKING`
- Old and new package/payload/license gate results: `TBD-BLOCKING`
- Hash and signature verification commands/results: `TBD-BLOCKING`
- Host preflight, ownership, power/storage, reboot, and process state:
  `TBD-BLOCKING`
- Independently verified old-artifact recovery copy: `TBD-BLOCKING`
- Entry-gate disposition and reviewer: `TBD-BLOCKING`

Any unresolved entry item requires `Blocked`; do not touch the installation.

## State inventory and backup

- Pre-upgrade package-manager and installed-payload inventory:
  `TBD-BLOCKING`
- Prior application/CLI identity and startup result: `TBD-BLOCKING`
- Relevant preferences and application-owned state, including an explicit
  `none found` result where applicable: `TBD-BLOCKING`
- Seeded external-state directory and sorted SHA-256 manifest:
  `TBD-BLOCKING`
- Backup scope, procedure, location, and independently verified manifest:
  `TBD-BLOCKING`
- Processes/services/tasks that must be stopped: `TBD-BLOCKING`

User profiles, evidence, requests, trust pins, recorded IQ, and unrelated files
must not be deleted, rewritten, or silently treated as application-managed
state.

## Upgrade execution and acceptance

- Exact documented commands/actions and complete logs: `TBD-BLOCKING`
- Installer/package-manager result and reboot state: `TBD-BLOCKING`
- New registration, payload, and visible-version verification:
  `TBD-BLOCKING`
- GUI/CLI launch and dependency result, as applicable: `TBD-BLOCKING`
- Seeded-state manifest comparison: `TBD-BLOCKING`
- Unexpected prompts, files, permissions, processes, services, tasks,
  registrations, or residue: `TBD-BLOCKING`
- Upgrade result with exact failed criterion if any: `TBD-BLOCKING`

## Rollback execution and acceptance

- Reviewed platform rollback path: `TBD-BLOCKING`
- Exact commands/actions and complete logs: `TBD-BLOCKING`
- New-version removal/registration result: `TBD-BLOCKING`
- Exact old-version restoration and package registration: `TBD-BLOCKING`
- Restored payload hashes, prior identity, and GUI/CLI launch:
  `TBD-BLOCKING`
- Seeded-state and backup comparison: `TBD-BLOCKING`
- Unexpected files, permissions, processes, services, tasks, registrations,
  prompts, or residue: `TBD-BLOCKING`
- Rollback result with exact failed criterion if any: `TBD-BLOCKING`

Removing the new version alone is not rollback. The exact old version must be
restored and reverified unless the governing plan explicitly requires a
different final state.

## Abort, cleanup, and final state

- Abort or failure events and retained partial evidence: `TBD-BLOCKING`
- Final installed version and package-manager registration: `TBD-BLOCKING`
- Final running process/service/task state: `TBD-BLOCKING`
- Final seeded-state integrity: `TBD-BLOCKING`
- Final residue inventory and cleanup result: `TBD-BLOCKING`
- Final host disposition: `TBD-BLOCKING`

If final state or cleanup is uncertain, record `cleanup_failed` and do not
claim qualification.

## Independent review and result

- Independently recomputed old/new artifact hashes: `TBD-BLOCKING`
- Independently recomputed seeded-state and restored-payload hashes:
  `TBD-BLOCKING`
- Package registration/payload reconciliation: `TBD-BLOCKING`
- Findings and corrections: `TBD-BLOCKING`
- Upgrade result: `TBD-BLOCKING`
- Rollback result: `TBD-BLOCKING`
- Overall platform result: `TBD-BLOCKING`
- Reviewer and UTC review date: `TBD-BLOCKING`

Only a platform record with passing upgrade, rollback, cleanup, and independent
review can close that exact candidate/platform cell.
