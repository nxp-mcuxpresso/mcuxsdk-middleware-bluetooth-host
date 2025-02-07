# Finding attribute handles

Although the application should be fully aware of the contents of the GATT Database, in certain situations it might be useful to perform some dynamic searches of certain attribute handles.

To find the handle value for a Service for which only the UUID is know the following API can be used:

```
bleResult_t GattDb_FindServiceHandle
(
 uint16_t startHandle,
 bleUuidType_t serviceUuidType,
 const bleUuid_t* pServiceUuid,
 uint16_t* pOutServiceHandle
);
```

To find a specific Characteristic Value Handle in a Service whose declaration handle is known, the following API is provided:

```
bleResult_t **GattDb\_FindCharValueHandleInService**
(
    uint16_t               serviceHandle,
    bleUuidType_t          characteristicUuidType,
    const bleUuid_t *      pCharacteristicUuid,
    uint16_t *             pOutCharValueHandle
);
```

If the return value is *gBleSuccess\_c*, the handle is written at *pOutCharValueHandle*. If the *serviceHandle* is invalid or not a valid Service declaration, the *gBleGattDbInvalidHandle\_c* is returned. Otherwise, the search is performed starting with the *serviceHandle+1*. If no Characteristic of the given UUID is found, the function returns the *gBleGattDbCharacteristicNotFound\_c* value.

To find a Characteristic Descriptor of a given type in a Characteristic, when the Characteristic Value Handle is known, the following API is provided:

```
bleResult_t **GattDb\_FindDescriptorHandleForCharValueHandle**
(
    uint16_t           charValueHandle,
    bleUuidType_t      descriptorUuidType,
    const bleUuid_t *  pDescriptorUuid,
    uint16_t *         pOutDescriptorHandle
);
```

Similarly, the function returns *gBleGattDbInvalidHandle\_c* is the handle is invalid. Otherwise, it starts searching from the *charValueHandle+1*. Then, *gBleGattDbDescriptorNotFound\_c* is returned if no Descriptor of the specified type is found. Otherwise, its attribute handle is written at the *pOutDescriptorHandle* and the function returns *gBleSuccess\_c*.

One of the most commonly used Characteristic Descriptors is the Client Configuration Characteristic Descriptor \(CCCD\), which has the UUID equal to *gBleSig\_CCCD\_d*. For this specific type, a special API is used as a shortcut:

```
bleResult_t **GattDb\_FindCccdHandleForCharValueHandle**
(
    uint16_t     charValueHandle,
    uint16_t *   pOutCccdHandle
);
```

**Parent topic:**[GATT database application interface](../topics/gatt_database_application_interface.md)

