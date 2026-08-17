# Initial compatibility and artifact-scope execution prompt

## Objective

Decide and document the initial release's intended artifact set and compatibility
promise without freezing a source revision, building a candidate, or publishing
anything. Bind the decision to implemented contracts and current supported-
platform boundaries, then update the readiness ledger truthfully.

## Verified starting point

- The repository begins at `b06a4e8` with a release-candidate readiness audit
  that identifies compatibility and artifact scope as the next hardware-free
  decision.
- User-owned icon-generation changes are present and must remain untouched and
  outside this slice's commit.
- The native SDR Calibration Profile schema is `1.0.0`; the profile engine
  accepts semantic-version major 1, rejects other majors and unknown required
  capabilities, and makes the native profile authoritative over adapters.
- Production run requests currently require exact schema version `1.0.0` and
  reject unknown members.
- Current native packages are recorded-input-only: macOS ARM64 DMG and Windows
  x64 MSI provide GUI and CLI, while Raspberry Pi OS 13 ARM64 provides CLI only.
  Ubuntu is an unsupported portability path.
- Production profile signing, trust-store management, profile activation,
  automatic upgrade migration, and safe installed WSJT-X mutation are not
  implemented.

## Required execution

1. Record a durable decision defining the intended initial binary and source
   artifact set, excluded artifacts, and the relationship between target scope
   and qualification still required.
2. Define compatibility separately for:
   - application/package versions;
   - native profile schema major 1;
   - recorded/live run-request schemas;
   - diagnostic CF32LE capture pairs;
   - published result directories and evidence;
   - WSJT-X adapters; and
   - install, upgrade, and rollback behavior.
3. Preserve the native profile as the authoritative semi-durable artifact.
   Require fail-closed handling of unsupported schema majors, required
   capabilities, integrity failures, and unknown strict request fields.
4. State which compatibility promises are durable and which remain explicitly
   pre-1.0, exact-version, diagnostic, lossy, or unqualified.
5. Reconcile the release-candidate readiness ledger, roadmap/status navigation,
   and operator-facing package documentation with the accepted decision.

## Constraints and non-goals

- Do not change schemas, serialization, CLI behavior, package payloads, version
  numbers, tests, application code, icon assets, or generators.
- Do not freeze a source revision or schema file, select an exact candidate,
  create release notes, tag, package, sign, notarize, upload, or publish.
- Do not access SDR hardware, antennas, references, splitters, network services,
  or external hosts.
- Do not promise backward-compatible package upgrades before clean-host upgrade
  and rollback qualification.
- Do not convert WSJT-X projections, evidence summaries, capture files, or run
  requests into substitutes for the native profile.

## Validation and adversarial review

Run Sphinx with warnings as errors and `git diff --check`. Review for conflict
with decisions 0002, 0008, 0009, 0019, and 0020; mismatch with current parser and
profile-engine behavior; accidental live-device packaging claims; unsupported
Ubuntu inclusion; silent promotion of unsigned profiles; unqualified upgrade
promises; ambiguous artifact inclusion; and any modification or staging of the
pre-existing icon work. Correct every actionable finding and repeat checks.

## Exit criteria

One accepted decision unambiguously defines the intended initial artifact set
and compatibility boundaries, readiness documentation reflects that the policy
decision is complete while candidate binding remains open, validation passes,
and only attributable documentation changes are staged for the separately
authorized commit and push.
