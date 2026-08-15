# End-to-end calibration qualification gate

No device/platform combination currently passes this gate. Qualification Gate 1
proves only bounded capture for its exact recorded combination.

An end-to-end record must identify one immutable source revision, installed
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

The test fails closed if any identity, configuration, reference authority,
validity, integrity, provenance, uncertainty component, stage artifact, or
cleanup result is absent or conflicted. A successful estimate, profile file,
unit test, capture-only run, or self-consistency check cannot substitute for the
complete gate.
