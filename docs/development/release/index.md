# Release documentation templates

These files are working templates for a future exact release candidate. They
are not release notes, do not identify a candidate, and do not authorize
publication. Every `TBD-BLOCKING` field must be replaced by retained evidence or
an accepted decision before release approval.

Assembly structure, public/private evidence separation, manifest content,
checksums, completion states, and review order are defined by the
[release evidence package assembly plan](evidence-package-assembly-plan.md).

```{toctree}
:maxdepth: 1

release-notes-template
known-limitations-template
upgrade-and-rollback-template
vulnerability-reporting-template
checksums-and-evidence-ledger-template
evidence-package-assembly-plan
```

The templates implement the artifact and compatibility boundary in
[decision 0022](../decisions/0022-initial-compatibility-and-release-artifacts.md)
and the outstanding gates in the
[release-candidate readiness audit](../release-candidate-readiness.md).

## Preparation and completion rules

- Copy each template into a new exact-candidate working directory; do not edit
  these source templates into apparent release records.
- Use only `TBD-BLOCKING`, `Missing`, `Blocked`, `Failed`, `Stale`, `Unreviewed`,
  or `Passed` for required-gate disposition. Only `Passed` satisfies a gate.
- Historical evidence may be linked as context but cannot populate a final
  candidate cell unless the governing contract explicitly makes it durable.
- Keep retained private evidence outside the public package and index it by
  scope, locator, digest, result, and reviewer.
- Any required prepublication blocking value prevents `Ready-to-publish`, tag,
  publication, or publication authorization. `Published-verified` additionally
  requires independent download reconciliation after publication.
