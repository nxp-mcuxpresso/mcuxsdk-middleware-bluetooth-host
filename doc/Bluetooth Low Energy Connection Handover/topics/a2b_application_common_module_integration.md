# Integration

Perform the following steps to integrate A2B application common module in an application:

1. Add `app_a2b.c` and `app_a2b.h` from `middleware\wireless\bluetooth\application\common\auto\` in the application project.
2. Initialize the A2B application common module by calling `A2B_Init()`.
3. One of the device involved in the Connection Handover process must be configured with the `gA2BInitiator_d` macro set to `1`, the other must be configured with the `gA2BInitiator_d` macro set to `0`. The initiator device triggers the EdgeLock-to-EdgeLock (E2E) key derivation and local IRK synchronization. Therefore, it should be started last.
4. After initialization the E2E key will be derived and the `gSecLibFunctions.pfSecLib_ExportA2BBlob()` and `gSecLibFunctions.pfSecLib_ImportA2BBlob()` functions can be used to export and import encrypted key material between the two devices.