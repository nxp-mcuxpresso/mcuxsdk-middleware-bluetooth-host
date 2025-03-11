# Characteristic read multiple procedure

The API for this procedure is shown here:

```
bleResult_t GattClient_ReadMultipleCharacteristicValues
(
    deviceId_t                 deviceId,
    uint8_t                    cNumCharacteristics,
    gattCharacteristic_t *     aIoCharacteristics
);
```

This procedure also allows an optimization for a specific situation, which occurs when multiple Characteristics, whose values are of known, fixed-length, can be all read in one single ATT transaction \(usually one single over-the-air packet\).

The application must know the value handle and value length of each Characteristic. It must also write the *value.handle*and *value.maxValueLength* with the aforementioned values, respectively, and then link the *value.paValue* field with an allocated array of size *maxValueLength*.

The following example involves reading three characteristics in a single packet.

```
#define mcNumCharacteristics_c 3
#define mcChar1Length_c 4
#define mcChar2Length_c 5
#define mcChar3Length_c 6
static uint8_t aValue1[mcChar1Length_c];
static uint8_t aValue2[mcChar2Length_c];
static uint8_t aValue3[mcChar3Length_c];
static gattCharacteristic_t myChars[mcNumCharacteristics_c];
myChars[0]. value . handle = 0x0015;
myChars[1]. value . handle = 0x0025;
myChars[2]. value . handle = 0x0035;
myChars[0]. value . maxValueLength = mcChar1Length_c;
myChars[1]. value . maxValueLength = mcChar2Length_c;
myChars[2]. value . maxValueLength = mcChar3Length_c;
myChars[0]. value . paValue = aValue1;
myChars[1]. value . paValue = aValue2;
myChars[2]. value . paValue = aValue3;
bleResult_t result = GattClient_ReadMultipleCharacteristicValues
(
    deviceId,
    mcNumCharacteristics_c,
    myChars
);
if (gBleSuccess_c != result)
{
    /* Handle error */
}
```

When the Client Procedure Callback is triggered, if no error occurs, each Characteristic’s value length should be equal to the requested lengths.

```
void gattClientProcedureCallback
(
    deviceId_t             deviceId,
    gattProcedureType_t p  rocedureType,
    gattProcedureResult_t  procedureResult,
    bleResult_t            error
)
{
    switch (procedureType)
    {
        /* ... */
       case gGattProcReadMultipleCharacteristicValues_c:
            if (gGattProcSuccess_c == procedureResult)
            {
                for ( uint8_t i = 0; i < mcNumCharacteristics_c; i++)
                {
                    /* Read value length */
                    PRINT(myChars[i]. value . valueLength );
                    /* Read data */
                    for ( uint8_t j = 0; j < myChars[i]. value . valueLength ; j++)
                    {
                        PRINT(myChars[i]. value . paValue [j]);
                    }
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

If the server does not know the length of the characteristic values, then the Read Multiple Variable Characteristic Values procedure can be used. This sub-procedure is used to read multiple characteristic values of variable length from a server when the client knows the characteristic value handles. The response returns the characteristic values and their corresponding lengths in the Length Value Tuple List parameter.

```
bleResult_t GattClient_ReadMultipleVariableCharacteristicValues
(
  deviceId_t             deviceId,
  uint8_t                cNumCharacteristics,
  gattCharacteristic_t*  pIoCharacteristics
);
```

The following example involves reading three characteristics of variable length in a single packet.

```
#define mcNumCharacteristics_c 3
#define mcCharLengthMax_c 10
static uint8_t aValue1[mcCharLengthMax _c];
static uint8_t aValue2[mcCharLengthMax _c];
static uint8_t aValue3[mcCharLengthMax _c];
static gattCharacteristic_t myChars[mcNumCharacteristics_c];
myChars[0].value .handle = 0x0015;
myChars[1].value .handle = 0x0025;
myChars[2].value .handle = 0x0035;
myChars[0].value .paValue = aValue1;
myChars[1].value .paValue = aValue2;
myChars[2].value .paValue = aValue3;
bleResult_t result = GattClient_ReadMultipleVariableCharacteristicValues
(
    deviceId,
    mcNumCharacteristics_c,
    pIoCharacteristics
);
if (gBleSuccess_c != result)
{
    /* Handle error */
}
```

The result of this procedure is sent to the application via the GATT procedure callback. The response includes the characteristic value together with a handle, length pair corresponding to each characteristic.

```
static void BleApp_GattClientCallback
(
    deviceId_t              serverDeviceId,
    gattProcedureType_t     procedureType,
    gattProcedureResult_t   procedureResult,
    bleResult_t             error
)
{
    switch (procedureResult)
    {
        /* ... */
        case gGattProcReadMultipleVarLengthCharValues_c:
            if (gGattProcSuccess_c == procedureResult)
            {
                for (uint8_t i = 0; i < mcNumCharacteristics_c; i++)
                {
                    /* Print characteristic handle and length */
                    PRINT(myChars[i].value.handle);
                    PRINT(myChars[i].value.valueLength);
                    for (uint8_t j = 0; j < myChars[i].value.maxValueLength; j++)
                    {
                        /* Print characteristic value */
                        PRINT(myChars[i].value.paValue[j]);
                    }
                }
            }
            else
            {
                /* Handle error */
            }
            break;
}
```

**Parent topic:**[Reading and writing characteristics](../topics/reading_and_writing_characteristics.md)

