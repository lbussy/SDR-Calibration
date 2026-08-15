# Qt desktop application

The Phase 13 `sdrcal-gui` application provides a Qt 6 Widgets interface to the
same recorded-input calibration and atomic publication service used by the
`sdrcal calibrate` command. It does not enumerate or access an SDR, discover
files or trust material, modify an installed WSJT-X instance, or establish
calibration accuracy.

## Recorded-input workflow

Select three explicit paths:

1. A versioned recorded-calibration request JSON file.
2. The independent local registry-signature pin file.
3. A new output directory. Existing output destinations are refused.

**Review Request** displays bounded structured request metadata and the exact
request in a read-only view. This is operator review; the production request
parser remains authoritative. **Start Calibration** runs the production
service on a worker while progress is displayed. **Cancel** requests
cooperative cancellation and waits for the workflow to reach a safe stopping
point.

Success loads the fixed published artifacts into read-only tabs:
`profile.json`, `evidence.json`, `summary.json`, and optional `wsjtx.ini`.
**Open Result** can inspect an existing result directory without activating,
editing, signing, replacing, revoking, or deleting its profile. Review files
are limited to 8 MiB each and symbolic-link files are refused.

The trust-pin mechanism is local and bounded, not a production trust store.
Generated profiles remain unsigned until production key management exists.
Any WSJT-X projection is explicitly lossy and does not replace the native SDR
Calibration Profile.
