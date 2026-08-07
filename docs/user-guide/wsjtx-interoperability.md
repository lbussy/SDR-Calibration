# WSJT-X interoperability

The application is intended to import WSJT-X FreqCal observations from
`fmt.all` and to import or export WSJT-X frequency-calibration settings.

WSJT-X interoperability is deliberately lossy: its intercept-and-slope values
cannot carry device identity, validated frequency range, environmental limits,
uncertainty, expiration, provenance, or signatures. The native SDR Calibration
Profile remains authoritative.
