# End-to-end calibration evidence contracts

The mandatory product gate validates the user-directed calibration and
evidence-qualification workflow; it does not certify the project owner's
hardware. Optional positive physical device/platform cells remain indexed
separately under decisions 0027 and 0028. Qualification Gate 1 proves bounded
capture only for its exact recorded combination.

Release-enabling workflow validation may compose bounded physical acquisition
and truthful physical acceptance/rejection evidence with deterministic
synthetic or reviewed recorded inputs that exercise the remaining production
stages. Each handoff must retain exact identity, hashes, provenance, conditions,
and limitations. A rejected physical input can prove correct workflow behavior
but cannot be used as an accepted fitting input.

An optional positive physical-cell record must identify one immutable source revision, installed
package and hash, clean test host, exact SDR/driver identity, effective settings,
authoritative reference and traceability, environmental conditions, operator,
UTC interval, abort method, and cleanup result. It must retain independently
reviewable evidence for every stage:

1. bounded acquisition with requested/effective settings and integrity hashes;
2. carrier estimation and diagnostics;
3. observation acceptance inputs, policy version, and reasons;
4. model fitting inputs, residuals, validity domain, and independence evidence;
5. uncertainty components, correlations, coverage factor, and combined result;
6. native profile generation, canonical bytes, integrity, provenance, and the
   honest production-signature disposition;
7. reliability quotient components and applicable reference-class ceiling;
8. bounded profile evaluation at an in-domain target against the authoritative
   reference, including error and uncertainty acceptance criteria; and
9. artifact publication, independent hash verification, final safe state, and
   any deviation or unresolved limitation.

The physical cell fails closed if any identity, configuration, reference authority,
validity, integrity, provenance, uncertainty component, stage artifact, or
cleanup result is absent or conflicted. A successful estimate, profile file,
unit test, capture-only run, or self-consistency check cannot substitute for the
complete gate.

For the mandatory release gate, the candidate must show that the same production
services enforce these stages and claim boundaries for user-supplied evidence.
No project-owner SDR, RF path, or supported-device matrix must become a positive
cell unless the release proposes that separate claim.
