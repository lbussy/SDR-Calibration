# Airspy HF+ end-to-end qualification readiness

## Result

**Status: Source blocker resolved with hardware-free tests; current physical
readiness remains unverified and execution was not started.**

The hardware-free Stage A review of the
[execution prompt](airspyhf-end-to-end-qualification-prompt.md) found that the
exact retained Airspy HF+ Discovery capture does not provide all identity and
configuration readbacks required by the production live workflow. A requested
receive-only enumeration was also rejected at the authorization boundary, so
no current hardware state was inspected.

## Blocking evidence

The retained Qualification Gate 1 manifest for serial
`2f52ff5de72635ba` records:

- `hardware_info` containing only the serial, without manufacturer or model;
- an empty clock-source value;
- no effective frequency-correction value.

`SoapyWorkflowBoundary` currently requires a manufacturer, model, hardware
serial, nonempty clock source, positive effective sample rate, and finite
effective frequency correction before it can construct the exact device and
configuration identity used by calibration. It then requires that identity and
configuration to match the request. Inventing defaults or weakening this check
would violate the fail-closed contract and is not part of qualification.

The production workflow also requires at least two independent accepted
observations spanning different frequencies. [NIST's WWV station
page](https://www.nist.gov/pml/time-and-frequency-division/time-distribution/radio-station-wwv)
identifies continuous 10 MHz and 15 MHz broadcasts, while [NIST SP
250-67](https://doi.org/10.6028/NIST.SP.250-67) identifies the carriers as
standard frequencies suitable for calibration within receiving-equipment and
propagation limitations. This establishes a candidate reference plan, not that
either carrier is currently receivable or suitable at this antenna.
WWV and WWVH share nominal frequencies, and WWV's 10 MHz and 15 MHz transmitters
derive from a common station clock. Station attribution, propagation effects,
and scientifically honest independence/correlation evidence therefore remain
unresolved later gates even after device readback is addressed.

## Source resolution

The subsequent hardware-free normalization slice added explicit provenance-
bearing rules rather than assumed readbacks. Exact `AirspyHF` driver/hardware
keys may supply the Airspy HF+ family identity policy; an empty current clock is
normalized only when Soapy reports no selectable sources; and effective
correction becomes zero only when Soapy explicitly reports correction as
unsupported. Unknown identities, selectable-but-unreported clocks, and
supported-but-unreadable correction still fail closed. Fake-API tests cover the
positive and negative boundaries.

## Required next decision

The operator must explicitly authorize the exact serial-bound enumeration and
bounded receive operations listed in the execution prompt. Current physical
metadata must match the normalized request before streaming proceeds. Until the
physical gates pass, the device matrix remains capture-qualified only and end-
to-end calibration remains not qualified.
