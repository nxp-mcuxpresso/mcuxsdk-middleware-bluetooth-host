# Bluetooth LE Host Stack libraries and API availability

All the APIs referenced in this document are available in the Central and Peripheral libraries. The support for Bluetooth 5.3 optional features such as Advertising, Advertising Extensions, GATT Caching, and EATT are provided in separate host libraries. They are distributed in a similar process as the legacy ones using GAP/GATT role support, which is described as follows.

For example, below are listed the full-featured libraries with complete support for both Central and Peripheral APIs, at GAP level.

-   *lib\_ble\_OPT\_host\_cm33\_iar.a* \(for IAR projects\)
-   *lib\_ble\_OPT\_host\_cm33\_gcc.a* \(for MCUX projects\)

These libraries include optional features implemented by the Bluetooth LE Host. For applications that need to use only the mandatory 5.3 Bluetooth LE and below features, the *lib\_ble\_host\_cm33\_iar.a* or *lib\_ble\_host\_cm33\_gcc.a* libraries can be used instead.

However, some applications may be targeted to memory-constrained devices and do not need the full support. In the interest of reducing code size and RAM utilization, optimized libraries are provided:

-   *lib\_ble\_host\_peripheral\_cm33\_iar.a/ lib\_ble\_host\_peripheral\_cm33\_gcc.a* and

-   *lib\_ble\_OPT\_host\_peripheral\_cm33\_iar.a / lib\_ble\_OPT\_host\_peripheral\_cm33\_gcc.a*.

    -   Support only APIs for the GAP Peripheral and GAP Broadcaster roles
    -   Support only APIs for the GATT Server role

-   *lib\_ble\_host\_central\_cm33\_iar.a* and *lib\_ble\_OPT\_host\_central\_cm33\_iar.a*

-   *lib\_ble\_host\_central\_cm33\_gcc.a* and *lib\_ble\_OPT\_host\_central\_cm33\_gcc.a*

    -   Support only APIs for the GAP Central and GAP Observer roles
    -   Support only APIs for the GATT Client role

If one attempts to use an API that is not supported \(for instance, calling *Gap\_Connect* with the *lib\_ble\_host\_peripheral\_cm33\_iar.a* and *lib\_ble\_host\_peripheral\_cm33\_gcc.a*\), then the API returns the *gBleFeatureNotSupported\_c* error code.

Similarly, if the API for OPT is used with a host library that does not have support for optional features, then *gBleFeatureNotSupported\_c* is returned. For instance, calling *Gap\_SetExtAdvertising* parameters with the *lib\_ble\_host\_peripheral\_cm33\_iar.a* and *lib\_ble\_host\_peripheral\_cm33\_gcc.a*\) returns the exit code *gBleFeatureNotSupported\_c*.

**Note:** See the *Bluetooth Low Energy Host Stack API Reference Manual* for explicit information regarding API support. Each function documentation contains this information in the Remarks section.

**Parent topic:**[Bluetooth LE Host Stack Initialization and APIs](../topics/bluetooth_le_host_stack_initialization_and_apis.md)

