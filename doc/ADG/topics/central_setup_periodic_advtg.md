# Central Setup

1.  Start scanning using `Gap_StartScanning`. Wait for `gPeriodicDeviceScannedV2_c` events in the scanning callback.

2.  Synchronize with a periodic advertiser by calling `Gap_PeriodicAdvCreateSync`. Wait for the `gPeriodicAdvSyncEstablished_c` event in the scanning callback. When PAwR is involved, this event includes additional information such as number of subevents, subevent interval, response slot delay and spacing,

3.  Synchronize to a PAwR subevent by calling `Gap_SetPeriodicSyncSubevent`. This API instructs the Controller to sync with a subset of the subevents within a PAwR train identified by syncHandle \(obtained after synchronizing with the PAwR train in the previous step\).

    ```
    bleResult_t Gap_SetPeriodicSyncSubevent ( uint16_t syncHandle, const gapPeriodicSyncSubeventParameters_t* pParams );
    ```

    Wait for the `gPeriodicSyncSubeventComplete_c` event.

4.  Use `Gap_SetPeriodicAdvResponseData` to set data in the AD format which would be sent as a Periodic Advertising Response to the broadcaster.

    ```
    bleResult_t Gap_SetPeriodicAdvResponseData ( uint16_t syncHandle, const gapPeriodicAdvertisingResponseData_t* pData );
    ```

5.  Optionally, the periodic advertiser may initiate a connection. If no connection callback was set on the scanner via APIs such as `Gap_Connect` or `Gap_StartAdvertising/Gap_StartExtAdvertising`, one must be explicitly set. This is achieved by calling `BluetoothLEHost_SetConnectionCallback` \(defined in `app_conn.h`\), which in turn calls `Gap_SetConnectionCallback`.

    ```
    void Gap_SetConnectionCallback ( gapConnectionCallback_t pfConnectionCallback );
    ```


**Parent topic:**[Periodic Advertising with Responses \(PAwR\)](../topics/periodic_advertizing_with_responses.md)

