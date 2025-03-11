# Attribute write notifications

When the GATT Client reads and writes values from/into the Server’s GATT Database, it uses ATT Requests.

The GATT Server module implementation manages these requests and, according to the database security settings and the Client’s security status \(authenticated, authorized, and so on\), automatically sends the ATT Responses without notifying the application.

There are however some situations where the application needs to be informed of ATT packet exchanges. For example, a lot of standard profiles define, for certain Services, some, so-called, Control-Point Characteristics. These are Characteristics whose values are only of immediate significance to the application. Writing these Characteristics usually triggers specific actions.

For example, consider a fictitious Smart Lamp. It has Bluetooth Low Energy connectivity in the Peripheral role and it contains a small GATT Database with a Lamp Service \(among other Services\). The Lamp Service contains two Characteristics: the Lamp State Characteristic \(LSC\) and the Lamp Action Characteristic \(LAC\).

LSC is a “normal” Characteristic with Read and Write properties. Its value is either 0, lamp off, or 1, lamp on\). Writing the value sets the lamp in the desired state. Reading it provides its current state, which is only useful when passing the information remotely.

The LAC has only one property, which is Write Without Response. The user can use the Write Without Response procedure to write only the value 0x01 \(all other values are invalid\). Whenever the user writes 0x01 in LAC, the lamp switches its state.

The LAC is a good example of a Control-Point Characteristic for these reasons:

-   Writing a certain value \(in this case 0x01\) triggers an action on the lamp.
-   The value the user writes has immediate significance only \(“0x01 switches the lamp”\) and is never used again in the future. For this reason, it does not need to be stored in the database.

Obviously, whenever a Control-Point Characteristic is written, the application must be notified to trigger some application-specific action.

The GATT Server allows the application to register a set of attribute handles as “write-notifiable”, in other words, the application wants to receive an event each time any of these attributes is written by the peer Client.

All Control-Point Characteristics in the GATT Database must have their Value handle registered. In fact, the application may register any other handle for write notifications for its own purposes with the following API:

```
bleResult_t GattServer\_RegisterHandlesForWriteNotifications
(
    uint8_t         handleCount,
    const uint16_t *      aAttributeHandles
);
```

The *handleCount* is the size of the *aAttributeHandles* array and it cannot exceed *gcGattMaxHandleCountForWriteNotifications\_c*.

After an attribute handle has been registered with this function, whenever the Client attempts to write its value, the GATT Server Callback is triggered with one of the following event types:

-   *gEvtAttributeWritten\_c* is triggered when the attribute is written with a Write procedure \(ATT Write Request\). In this instance, the application has to decide whether the written value is valid and whether it must be written in the database, and, if so, the application must write the value with the *GattDb\_WriteAttribute*, see [GATT database application interface](gatt_database_application_interface.md). At this point, the GATT Server module does not automatically send the ATT Write Response over the air. Instead, it waits for the application to call this function:

```
bleResult_t GattServer_SendAttributeWrittenStatus
(
    deviceId_t     deviceId,
    uint16_t       attributeHandle,
    uint8_t        status
);
```

This API also has an enhanced counterpart, which adds the *bearerId* parameter.

The value of the *status* parameter is interpreted as an ATT Error Code. It must be equal to the *gAttErrCodeNoError\_c* \(0x00\) if the value is valid and it is successfully processed by the application. Otherwise, it must be equal to a profile-specific error code \(in interval 0xE0-0xFF\) or an application-specific error code \(in interval 0x80-0x9F\).

-   *gEvtAttributeWrittenWithoutResponse\_c* is triggered when the attribute is written with a Write Without Response procedure \(ATT Write Command\). Because this procedure expects no response, the application may process it and, if necessary, write it in the database. Regardless of whether the value is valid or not, no response is needed from the application.
-   *gEvtLongCharacteristicWritten\_c* is triggered when the Client has completed writing a Long Characteristic value; the event data includes the handle of the Characteristic Value attribute and a pointer to its value in the database.

Attributes can also be registered for read notifications using the followng API:

```
bleResult_t GattServer_RegisterHandlesForReadNotifications
(
 uint8_t handleCount,
 const uint16_t* aAttributeHandles
);

```

To unregister one or more handles from the list for either write or read, the following APIs can be used:

```

bleResult_t GattServer_UnregisterHandlesForWriteNotifications
(
 uint8_t handleCount,
 const uint16_t* aAttributeHandles
);

bleResult_t GattServer_UnregisterHandlesForReadNotifications
(
 uint8_t handleCount,
 const uint16_t* aAttributeHandles
);
```

**Parent topic:**[Server APIs](../topics/server_apis.md)

