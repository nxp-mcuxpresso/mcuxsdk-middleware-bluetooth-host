# Characteristic value read procedure

The main API for reading a Characteristic Value is shown here:

```
bleResult_t GattClient\_ReadCharacteristicValue
(
    deviceId_t                 deviceId,
    gattCharacteristic_t *     pIoCharacteristic,
    uint16_t                   maxReadBytes
);
```

This procedure assumes that the application knows the Characteristic Value Handle, usually from a previous Characteristic Discovery procedure. Therefore, the *value.handle* field of the structure pointed by *pIoCharacteristic* must be completed.

Also, the application must allocate a large enough array of bytes where the received value \(from the ATT packet exchange\) is written. The *maxReadBytes* parameter is set to the size of this allocated array.

The GATT Client module takes care of long characteristics, whose values have a greater length than can fit in a single ATT packet, by issuing repeated ATT Read Blob Requests when needed.

The following examples assume that the application knows the Characteristic Value Handle and that the value length is variable, but limited to 50 bytes.

```
gattCharacteristic_t myCharacteristic;
myCharacteristic. value . handle = 0x10AB;
#define mcMaxValueLength_c 50
static uint8_t aValue[mcMaxValueLength_c];
myCharacteristic. value . paValue = aValue;
bleResult_t result = GattClient_ReadCharacteristicValue
(
    deviceId,
    &myCharacteristic,
    mcMaxValueLength_c
);
if (gBleSuccess_c != result)
{
    /* Handle error */
}
```

Regardless of the value length, the Client Procedure Callback is triggered when the reading is complete. The received value length is also filled in the *value* structure.

```
void gattClientProcedureCallback
(
    deviceId_t             deviceId,
    gattProcedureType_t    procedureType,
    gattProcedureResult_t  procedureResult,
    bleResult_t            error
)
{
    switch (procedureType)
    {
        /* ... */
        case*gGattProcReadCharacteristicValue\_c*:
            if (*gGattProcSuccess_c == procedureResult)
            {
                /* Read value length */
                PRINT(myCharacteristic. value . valueLength );
                /* Read data */
                for ( uint16_t j = 0; j < myCharacteristic. value . valueLength ; j++)
                {
                    PRINT(myCharacteristic. value . paValue [j]);
                }
            }
            else
            {
                /* Handle error */
                PRINT(error);
            }
            break;
        /* ... */
    }
}
```

**Parent topic:**[Reading and writing characteristics](../topics/reading_and_writing_characteristics.md)

