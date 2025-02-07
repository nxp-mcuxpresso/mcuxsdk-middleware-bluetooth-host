# Server callback

The first GATT Server call is the installation of the Server Callback, which has the following prototype:

```
**typedef ****void** (* gattServerCallback_t )
(
    deviceId_t             deviceId,    /*!< Device ID identifying the active connection. */
    gattServerEvent_t *    pServerEvent /*!< Server event. */
);
```

For EATT, the following signature should be used:

```
typedef void (*gattServerEnhancedCallback_t) ( deviceId_t deviceId, bearerId_t bearerId, gattServerEvent_t* pServerEvent );
```

The callback can be installed with:

```
bleResult_t **GattServer\_RegisterCallback**
(
    gattServerCallback_t callback
);
```

The EATT server callback should be installed using the following API:

```
bleResult_t **GattServer\_RegisterEnhancedCallback**
(
     gattServerEnhancedCallback_t callback
);
```

The first member of the *gattServerEvent\_t* structure is the *eventType*, an enumeration type with the following possible values:

-   *gEvtMtuChanged\_c:* Signals that the Client-initiated MTU Exchange Procedure has completed successfully and the *ATT\_MTU* has been increased. The event data contains the new value of the *ATT\_MTU*. Is it possible that the application flow depends on the value of the *ATT\_MTU*, for example, there may be specific optimizations for different *ATT\_MTU* ranges. This event is not triggered if the *ATT\_MTU* was not changed during the procedure.
-   *gEvtHandleValueConfirmation\_c:* A Confirmation was received from the Client after an Indication was sent by the Server.
-   *gEvtAttributeWritten\_c, gEvtAttributeWrittenWithoutResponse\_c:* See [Attribute write notifications](attribute_write_notifications.md#).
-   *gEvtCharacteristicCccdWritten\_c:* The Client has written a CCCD. The application should save the CCCD value for bonded devices with *Gap\_SaveCccd*.
-   *gEvtError\_c:* An error occurred during a Server-initiated procedure.
-   *gEvtLongCharacteristicWritten\_c*: A long characteristic was written.

-   *gEvtInvalidPduReceived\_c*: An invalid PDU was received from Client. Application decides if disconnection is required.

-   *gEvtAttributeRead\_c*: An attribute registered with `GattServer_RegisterHandlesForReadNotifications` is being read.


**Parent topic:**[Server APIs](../topics/server_apis.md)

