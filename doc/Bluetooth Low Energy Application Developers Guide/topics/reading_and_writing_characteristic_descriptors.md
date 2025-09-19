# Reading and writing characteristic descriptors

Two APIs are provided for these procedures which are very similar to Characteristic Read and Write.

The only difference is that the handle of the attribute to be read/written is provided through a pointer to an *gattAttribute\_t* structure \(same type as the *gattCharacteristic\_t.value* field\).

All of the following APIs have an enhanced counterpart of the form *GattClient\_Enhanced\[procedure\]*. A *bearerId*parameter was added to specify on which bearer the transaction should take place. A value of *0* for the bearer Id identifies the Unenhanced ATT bearer. Values higher than *0* are used to identify the Enhanced ATT bearer used for the ATT procedure.

```
bleResult_t GattClient_ReadCharacteristicDescriptor
(
    deviceId_t             deviceId,
    gattAttribute_t *      pIoDescriptor,
    uint16_t               maxReadBytes
);
```

The *pIoDescriptor-\>handle* is required \(it may have been discovered previously by *GattClient\_DiscoverAllCharacteristicDescriptors*\). The GATT module fills the value that was read in the fields *pIoDescriptor-\>aValue* \(must be linked to an allocated array\) and *pIoDescriptor-\>valueLength* \(size of the array\).

Writing a descriptor is also performed similarly with this function:

```
bleResult_t GattClient_WriteCharacteristicDescriptor
(
    deviceId_t             deviceId,
    gattAttribute_t *      pDescriptor,
    uint16_t               valueLength,
    uint8_t *              aValue
);
```

Only the *pDescriptor-\>handle* must be filled before calling the function.

One of the most frequently written descriptors is the Client Characteristic Configuration Descriptor \(CCCD\). It has a well-defined UUID \(*gBleSig\_CCCD\_d*\) and a 2-byte long value that can be written to enable/disable notifications and/or indications.

In the following example, a Characteristic’s descriptors are discovered and its CCCD written to activate notifications.

```
static gattCharacteristic_t myChar;
myChar.value.handle = 0x00A0; /* Or maybe it was previously discovered? */
#define mcMaxDescriptors_c 5
static gattAttribute_t aDescriptors[mcMaxDescriptors_c];
myChar.aDescriptors = aDescriptors;
/* ... */
{
    bleResult_t result = GattClient_DiscoverAllCharacteristicDescriptors
    (
        deviceId,
        &myChar,
        0xFFFF,
        mcMaxDescriptors_c
    );
    if (gBleSuccess_c != result)
    {
        /* Handle error */
    }
}
/* ... */
void gattClientProcedureCallback
(
    deviceId_t                 deviceId,
    gattProcedureType_t        procedureType,
    gattProcedureResult_t      procedureResult,
    bleResult_t                error
)
{
    switch (procedureType)
    {
      /* ... */
      case gGattProcDiscoverAllCharacteristicDescriptors_c:
         if (gGattProcSuccess_c == procedureResult)
          {
           /* Find CCCD */
            for ( uint8_t j = 0; j < myChar.cNumDescriptors; j++)
               {
                if ((myChar.aDescriptors[j].uuidType == gBleUuidType16_c) && 
                    (gBleSig_CCCD_d == myChar.aDescriptors[j].uuid.uuid16))
                 {
                    uint8_t cccdValue[2];
                    packTwoByteValue(gCccdNotification_c, cccdValue);
                    bleResult_t result = GattClient_WriteCharacteristicDescriptor
                     (
                       deviceId,
                       &myChar. aDescriptors [j],
                       2,
                       (uint8_t*)&cccdValue
                     );
                    if (gBleSuccess_c != result)
                       {
                        /* Handle error */
                       }
                       break;
                     }
                }
            }
            else
            {
                /* Handle error */
                PRINT(error);
            }
            break;
        case gGattProcWriteCharacteristicDescriptor_c:
            if (gGattProcSuccess_c == procedureResult)
            {
                /* Notification successfully activated */
            }
            else
            {
                /* Handle error */
                PRINT(error);
            }
        /* ... */
    }
}
```

**Parent topic:**[Client APIs](../topics/client_apis.md)

