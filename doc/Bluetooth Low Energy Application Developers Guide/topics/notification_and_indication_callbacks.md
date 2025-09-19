# Notification and indication callbacks

When the Client receives a notification from the Server, it triggers a callback with the following prototype:

```
typedef void (* gattClientNotificationCallback_t )
(
    deviceId_t     deviceId,
    uint16_t       characteristicValueHandle,
    uint8_t *      aValue,
    uint16_t       valueLength
);
```

The *deviceId* identifies the Server connection \(for multiple connections at the same time\). The *characteristicValueHandle* is the attribute handle of the Characteristic Value declaration in the GATT Database. The Client must have discovered it previously to be able recognize it.

For EATT, the following signature should be used:

```
typedef void (*gattClientEnhancedNotificationCallback_t)
 ( deviceId_t deviceId,
   bearerId_t bearerId,
   uint16_t characteristicValueHandle,
   uint8_t* aValue,
   uint16_t valueLength );
```

The callback must be installed with:

```
bleResult_t GattClient_RegisterNotificationCallback
(
    gattClientNotificationCallback_t callback
);
```

Very similar definitions exist for indications.

The EATT notification callback should be installed using the following API:

```
bleResult_t GattClient_RegisterEnhancedNotificationCallback
(
 gattClientEnhancedNotificationCallback_t callback
)
```

When receiving a notification or indication, the Client uses the *characteristicValueHandle* to identify which Characteristic was notified. The Client must be aware of the possible Characteristic Value handles that can be notified/indicated at any time, because it has previously activated them by writing its CCCD \(see [Reading and writing characteristic descriptors](reading_and_writing_characteristic_descriptors.md#)\).

When the Client receives a multiple value notification from the Server, it triggers a callback with the following prototype:

```
typedef void (*gattClientMultipleValueNotificationCallback_t)
(
    deviceId_t     deviceId,  
    /*!< Device ID identifying the active connection. */   
    uint8_t* aHandleLenValue, 
    /*!< The array of handle, value length, value tuples. */
    uint32_t totalLength      
    /*!< Value array size. */
);
```

The callback must be installed with:

```
bleResult_t GattClient_RegisterMultipleValueNotificationCallback
(
    gattClientMultipleValueNotificationCallback_t callback
);
```

When using EATT, the following callback prototype and registration APIs should be used:

```
typedef void (*gattClientEnhancedMultipleValueNotificationCallback_t)
( deviceId_t deviceId,   
    /*!< Device ID identifying the active connection. */   
  bearerId_t bearerId,    
   /*!< Bearer ID identifing the Enhanced ATT bearer used. */    
  uint8_t* aHandleLenValue,   
   /*!< The array of handle, value length, value tuples. */ 
  uint32_t totalLength     /*!< Value array size. */
 );
```

**Parent topic:**[Installing client callbacks](../topics/installing_client_callbacks.md)

