# Integration

Perform the following steps to integrate the Ranging Service on the application that has the role of Ranging Requester (GATT client):

1. Add the following component to the application project via Kconfig:
   - `CONFIG_MCUX_COMPONENT_middleware.wireless.ble_profiles_ranging_client=y`
2. Ensure the `gRasRREQ_d` define is present and set to `1` in the application's `app_preinclude.h` file.
3. After the connection with the peer is encrypted, proceed to discover the RAS and its characteristics through the common Service Discovery module. Register a service discovery callback using `BleServDisc_RegisterCallback` and call `BleServDisc_FindService()` with the `gBleSig_RangingService_d` UUID as parameter. On the `gServiceDiscovered_c` event in the registered callback, store the handles of the RAS and its characteristics and characteristic descriptors for later use. Some handles also need to be registered with the localization component via the following APIs:
   1. `RasClient_SetRasControlPointHandle`
   2. `RasClient_SetRasRealTimeHandle`
   3. `RasClient_SetRasSupportedFeatures`
4. Proceed to use the regular GATT client APIs such as `GattClient_ReadCharacteristicValue()` and `GattClient_WriteCharacteristicDescriptor()` in order to read the peer's supported RAS features and enable notifications/indications on the RAS Control Point CCCD and on either the On-Demand or Real-Time CCCDs. If you choose On-Demand as a data transfer method, you must also enable notifications/indications on the RAS Data Ready and Data Overwritten CCCDs. The client is now considered subscribed to the RAS and the CS Procedure can start by following the steps described in the sections above.
5. Depending on the RAS settings chosen by the client application, communication from the server arrives as either GATT notifications or indications. In the GATT notification or indication callback, the following APIs should be used:
   1. `RasClient_StorePeerMeasurementData`, when the On-Demand or Real-Time handle is notified/indicated.
   2. `RasClient_ProcessRasCPRsp`, when the Control Point handle is notified/indicated.
   3. `RasClient_ProcessRasDataReadyIndications`, when the Data Ready handle is notified/indicated.
   4. `RasClient_ProcessRasDataOverwrittenIndications`, when the Data Overwritten handle is notified/indicated.

Perform the following steps to integrate the Ranging Service on the application that has the role of Ranging Responder (GATT server):

1. Add the following component to the application project via Kconfig:
   - `CONFIG_MCUX_COMPONENT_middleware.wireless.ble_profiles_ranging=y`
2. Ensure the `gRasRRSP_d` and `gAppRasDataTransfer_d` defines are present and set to `1` in the application's `app_preinclude.h` file.
3. Add the Ranging Service and its characteristics to the GATT database in the application project's `gatt_db.h` file as in the excerpt below:
   ```
   PRIMARY_SERVICE(service_ranging, gBleSig_RangingService_d)
    CHARACTERISTIC(char_ras_ctrl_point, gBleSig_RasControlPoint_d, (gGattCharPropWriteWithoutRsp_c | gGattCharPropIndicate_c) )
        VALUE_VARLEN(value_ras_ctrl_point, gBleSig_RasControlPoint_d, (gPermissionFlagWritable_c | gPermissionFlagWriteWithEncryption_c ), 0x05, 0x01, 0x00)
        CCCD(cccd_ras_ctrl_point)
    CHARACTERISTIC(char_ras_stored_data, gBleSig_RasOnDemandProcData_d, (gGattCharPropNotify_c | gGattCharPropIndicate_c) )
        VALUE_VARLEN(value_ras_stored_data, gBleSig_RasOnDemandProcData_d, (gPermissionNone_c), gAttMaxNotifIndDataSize_d(gAttMaxMtu_c), 0x01, 0x00)
        CCCD(cccd_ras_stored_data)
    CHARACTERISTIC(char_ras_real_time_data, gBleSig_RasRealTimeProcData_d, (gGattCharPropNotify_c | gGattCharPropIndicate_c) )
        VALUE_VARLEN(value_ras_real_time_data, gBleSig_RasRealTimeProcData_d, (gPermissionNone_c), gAttMaxNotifIndDataSize_d(gAttMaxMtu_c), 0x01, 0x00)
        CCCD(cccd_ras_real_time_data)
    CHARACTERISTIC(char_ras_ranging_data_ready, gBleSig_RasProcDataReady_d, (gGattCharPropNotify_c | gGattCharPropIndicate_c) )
        VALUE(value_ras_ranging_data_ready, gBleSig_RasProcDataReady_d, (gPermissionNone_c), 0x02, 0x00, 0x00)
        CCCD(cccd_ras_data_ready)
    CHARACTERISTIC(char_ras_ranging_data_overwritten, gBleSig_RasprocDataOverwritten_d, (gGattCharPropNotify_c | gGattCharPropIndicate_c) )
        VALUE(value_ras_ranging_data_overwritten, gBleSig_RasprocDataOverwritten_d, (gPermissionNone_c), 0x02, 0x00, 0x00)
        CCCD(cccd_ras_data_overwritten)
    CHARACTERISTIC(char_ras_feature, gBleSig_RasFeature_d, (gGattCharPropRead_c) )
        VALUE(value_ras_feature, gBleSig_RasFeature_d, (gPermissionFlagReadable_c | gPermissionFlagReadWithEncryption_c), 4, 0x0F, 0x00, 0x00, 0x00)
   ```
4. Initialize RAS by calling `Ras_Start()` with the handles defined in the GATT database.
5. Upon the MTU Exchange procedure's completion, call `Ras_SetMtuValue()` to inform the module.
6. The client's interactions with the server are processed in the GATT server callback. Here the following APIs are used:
   1. `Ras_Subscribe` when the client subscribes to either On-Demand or Real-Time CCCDs.
   2. `Ras_SetDataSendPreference` to save the client's preference for the data transfer (either notifications or indications).
   3. `Ras_ControlPointHandler` when the client writes a command to the RAS Control Point.
   4. `Ras_Unsubscribe` when the client unsubscribes or disconnects.