# WSJT-X interoperability

The hardware-free adapter core imports WSJT-X 2.6.x and 2.7.x FreqCal
observations from `fmt.all` and imports or exports frequency-calibration
settings supplied as text by a caller. Other versions are rejected because
their sign and format mapping has not been verified.

WSJT-X interoperability is deliberately lossy: its intercept-and-slope values
cannot carry device identity, validated frequency range, environmental limits,
uncertainty, expiration, provenance, or signatures. The native SDR Calibration
Profile remains authoritative.

Settings import produces an informational projection, never a
qualification-capable profile. Settings export requires one valid linear native
segment and reports the identity, domain, validity, uncertainty, provenance,
assurance, and integrity information omitted from the projection. `fmt.all`
import preserves every source line; malformed lines require manual review and
are not silently discarded. WSJT-X rejection markings are preserved but do not
replace this application's observation-acceptance policy.

Automatic discovery or modification of an installed WSJT-X settings file is
not implemented. A later application-service slice must identify the intended
instance, ensure it is safe to modify, back it up, replace it atomically, read
it back, and restore the original on verification failure.
