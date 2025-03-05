# Application removal of bonded devices data

The application can remove a bonded device from NVM. The bonded device cannot be deleted if it is in an active connection. The application can remove one or all bonds by calling the following synchronous GAP APIs:

-   `Gap_RemoveBond(uint8_t nvmIndex)` – `nvmIndex` can be obtained via the `Gap_CheckIfBonded` API.
-   `Gap_RemoveAllBonds()` - no connections should be active otherwise the call fails.

Removing a bonded device does not affect the controller address resolution state nor the contents of either the Controller Filter Accept List or the Controller Resolving List. If Controller Privacy is enabled, it remains so until it is disabled or the device is reset.

In a scenario where the user wants to remove a bonded device and all its effects on device behavior \(Controller Filter Accept List, Controller Resolving List\), the following operations should be executed:

-   `Gap_ClearFilterAcceptList or Gap_RemoveDeviceFromFilterAcceptList`
    -   Clear Controller Filter Accept List or clear a device from Filter Accept List.
-   `Gap_RemoveAllBonds or Gap_RemoveBond`
    -   All bonded devices are removed or one bonded device is removed from NVM.
-   `BleConnManager_DisablePrivacy`
    -   Controller Privacy is disabled, Controller Resolving List is cleared and address resolution is disabled. The device should not be advertising or scanning, otherwise this call fails.
-   `BleConnManager_EnablePrivacy`
    -   Called after the `gControllerPrivacyStateChanged_c` event is received, confirming Controller Privacy has been disabled. If not all bonds have been deleted, Controller Privacy is reenabled. In the absence of bonds, Host Privacy is enabled.

**Parent topic:**[Data management of bonded devices](../topics/data_management_of_bonded_devices.md)

