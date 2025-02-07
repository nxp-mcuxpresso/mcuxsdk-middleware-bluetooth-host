# Central Setup

The application may decide to listen to periodic advertising by going through the following states:

1.  \[Optional\] Add a known periodic advertiser to the periodic advertiser list held in the Controller by calling:

    ```
    **bleResult\_t Gap\_UpdatePeriodicAdvList**
    (
        gapPeriodicAdvListOperation_t operation,
        bleAddressType_t               addrType,
        uint8_t*                       pAddr,
        uint8_t                        SID
    );
    ```

    Wait for the *gPeriodicAdvListUpdateComplete\_c* event in the Generic Callback.

2.  Synchronize with a periodic advertiser by calling:

    ```
    bleResult_t **Gap\_PeriodicAdvCreateSync**
    (
        gapPeriodicAdvSyncReq_t*   pReq,
    );
    ```

    *pReq* parameter *filterPolicy*can be set to *gUseCommandParameters\_c* to synchronize with the given peer, or to *gUsePeriodicAdvList\_c* to start synchronizing with all the devices in the previously populated periodic advertiser list.

    Wait for the *gPeriodicAdvSyncEstablished\_c* event and check the status. If scanning is not enabled at the time this command is sent, synchronization occurs after scanning is started. Synchronization remains pending until *gPeriodicAdvSyncEstablished\_c*event is received. If synchronization was successful, the *syncHandle* is returned in this event.

3.  Terminate the synchronization with the periodic advertiser by calling:

    ```
    bleResult_t **Gap\_PeriodicAdvTerminateSync**
    (
        uint16_t syncHandle
    );
    ```

    To cancel a pending synchronization, the application should call *Gap\_PeriodicAdvTerminateSync* with *syncHandle* set to the reserved value *gBlePeriodicAdvOngoingSyncCancelHandle* and wait for *gPeriodicAdvCreateSyncCancelled\_c* event.

    Otherwise, to terminate an already established sync with an advertiser, use the *syncHandle* value from the *gPeriodicAdvSyncEstablished\_c* event and wait for a *gPeriodicAdvSyncTerminated\_c* event.


**Parent topic:**[Periodic Advertising](../topics/periodic_advertising.md)

