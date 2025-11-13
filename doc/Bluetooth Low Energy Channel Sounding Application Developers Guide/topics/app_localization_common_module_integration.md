# Integration

All the APIs referenced in this section are available in the `app_localization` application common module.

Perform the following steps for integration into the application. Unless specified, each step applies to both the CS Initiator and the CS Reflector:

1. Add the following components to the application project via Kconfig:
   - `CONFIG_MCUX_COMPONENT_middleware.wireless.lcl=y`
   - `CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_CS=y`
   - `CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_CS_component_lib=y`
2. Begin the initialization of the common module by calling `AppLocalization_Init()`. One of the parameters of this API is the application callback where events will be received from the app_localization module.
3. Complete the initialization of the common module by calling `AppLocalization_HostInitHandler()` after the Host stack has been initialized.
4. The CS Config depends on the Bluetooth LE connection interval. Upon connection with the peer, the CS Config is retrieved by calling `AppLocalization_ReadConfig()` and updated by calling `AppLocalization_WriteConfig()`.
5. If the application is the Ranging Requester, that is, it will run the distance measurement algorithm, the connection interval must also be saved by calling the function `AppLocalization_SetConnectionInterval()`. This is required by the algorithm.
6. Once the Ranging Requester has discovered and subscribed to the Ranging Service on the Ranging Responder (as detailed in the section below), the CS procedure can begin. The first step is to call `AppLocalization_Config()`.
7. Upon receiving the `gConfigComplete_c` or the `gLocalConfigWritten_c` events, only the device who has the GAP Central role must call `AppLocalization_SecurityEnable()`.
8. Upon receiving the `gCsSecurityEnabled_c` event, only one device must call `AppLocalization_SetProcedureParameters()`.
9.  Upon receiving the `gSetProcParamsComplete_c` event, the device from step 8 must call `AppLocalization_StartMeasurement()`.
10. From this point onwards, the application will receive events informing it of the status of the distance measurement procedure, such as:
    - `gDistanceMeastStarted_c` - the CS Procedure has started
    - `gLocalMeasurementComplete_c` - the CS Procedure is complete and local data is available
    - various error events
11. Upon peer disconnection, cleanup should be performed by calling `AppLocalization_ResetPeer()`.