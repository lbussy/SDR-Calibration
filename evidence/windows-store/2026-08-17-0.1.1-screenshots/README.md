# Windows Store screenshot evidence

These four Microsoft Store listing candidates were captured on Windows 11 from
the exact staged GUI and CLI payload associated with source revision
`957fbeb204177c9ba2a1582e936476244b201b9d` and Store MSIX SHA-256
`1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`.

The workflow used the retained deterministic synthetic certification fixture.
No SDR hardware, live acquisition, network access, package upload, Partner
Center edit, or submission occurred. Temporary fixture and result directories
were absent after capture, and the GUI and CLI processes were closed.

`manifest.json` records the dimensions, byte counts, SHA-256 values, capture
host, candidate binding, and privacy disposition. Independent visual review
confirmed that the accepted images show only synthetic inputs, neutral public
paths, the packaged CLI version, and the application's explicit limitations.
An earlier composite exposing a personal Downloads path was rejected and is not
retained here.

The capture wrapper redirected PowerShell stdout and stderr to a durable log so
the run could be monitored remotely. That transcript contained only the local
operator path and success message and is intentionally not retained as Store
evidence.
