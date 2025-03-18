# Sending notifications and indications

The APIs provided for these Server-initiated operations are very similar.

All of the following APIs have an enhanced counterpart of the form *GattServer\_Enhanced\[procedure\]*. A *bearerId* parameter was added to specify on which bearer the transaction should take place. A value of *0* for the bearerId identifies the Unenhanced ATT bearer. Values higher than*0* are used to identify the Enhanced ATT bearer used for the ATT procedure.

```
bleResult_t GattServer_SendNotification
(
    deviceId_t     deviceId,
    uint16_t       handle
);
bleResult_t GattServer_SendIndication
(
    deviceId_t     deviceId,
    uint16_t       handle
);
```

Only the attribute handle needs to be provided to these functions. The attribute value is automatically retrieved from the GATT Database.

**Note:** It is the application developer’s responsibility to check if the Client designated by the *deviceId* has previously activated Notifications/Indications by writing the corresponding CCCD value. To do that, the following GAP APIs should be used:

```
bleResult_t Gap_CheckNotificationStatus
(
    deviceId_t     deviceId,
    uint16_t       handle,
    bool_t *       pOutIsActive
);
bleResult_t Gap_CheckIndicationStatus
(
    deviceId_t     deviceId,
    uint16_t       handle,
    bool_t *       pOutIsActive
    );
```

**Note:** It is necessary to use these two functions with the *Gap\_SaveCccd* only for bonded devices, because the data is saved in NVM and reloaded at reconnection. For devices that do not bond, the application may also use its own bookkeeping mechanism.

There is an important difference between sending **Notifications and Indications**:

-   The latter can only be sent one at a time. In addition, the application must wait for the Client Confirmation \(signaled by the *gEvtHandleValueConfirmation\_c* Server event, or by a *gEvtError\_c* event with *gGattClientConfirmationTimeout\_c* error code\) before sending a new Indication. Otherwise, a *gEvtError\_c* event with *gGattIndicationAlreadyInProgress\_c* error code is triggered.
-   The Notifications can be sent consecutively.

**Parent topic:**[Server APIs](../topics/server_apis.md)

