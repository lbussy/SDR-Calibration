# Candidate upgrade and rollback template

Status: Template only — no upgrade or rollback is qualified

## Bound candidates

- Prior installed version and source revision: `TBD-BLOCKING`
- Prior artifact filename and SHA-256: `TBD-BLOCKING`
- New version and source revision: `TBD-BLOCKING`
- New artifact filename and SHA-256: `TBD-BLOCKING`
- Platform, architecture, OS build, and clean-host identity: `TBD-BLOCKING`

Maintenance installation of the same candidate is not prior-version upgrade
evidence. Same-host replacement is not a fresh-host lifecycle result.

## State inventory and backup

- Application files expected before upgrade: `TBD-BLOCKING`
- User preferences and locations: `TBD-BLOCKING`
- User profiles/evidence and locations: `TBD-BLOCKING`
- Backup procedure and independently verified digest/index: `TBD-BLOCKING`
- Processes/services that must be stopped: `TBD-BLOCKING`

The procedure must not delete or silently migrate user profiles, evidence,
trust pins, or unrelated application state.

## Upgrade procedure and acceptance

1. Verify both artifact hashes and signatures: `TBD-BLOCKING`.
2. Record the pre-upgrade inventory and application startup result:
   `TBD-BLOCKING`.
3. Apply only the documented platform-native upgrade: `TBD-BLOCKING`.
4. Verify application/CLI identity, preserved state, schema handling, and normal
   startup: `TBD-BLOCKING`.
5. Verify no unexpected files, processes, services, permissions, or prompts:
   `TBD-BLOCKING`.

Pass/fail criteria and retained log location: `TBD-BLOCKING`

## Rollback procedure and acceptance

1. Stop the new version safely: `TBD-BLOCKING`.
2. Apply the reviewed removal or rollback path without deleting user data:
   `TBD-BLOCKING`.
3. Restore the exact prior artifact and only the state explicitly required:
   `TBD-BLOCKING`.
4. Verify prior-version startup, hashes, preserved data, and residue:
   `TBD-BLOCKING`.

Rollback pass/fail criteria and retained evidence: `TBD-BLOCKING`

## Results

- Upgrade result: `TBD-BLOCKING`
- Rollback result: `TBD-BLOCKING`
- Preserved-state result: `TBD-BLOCKING`
- Cleanup/final host state: `TBD-BLOCKING`
- Reviewer and review date: `TBD-BLOCKING`
