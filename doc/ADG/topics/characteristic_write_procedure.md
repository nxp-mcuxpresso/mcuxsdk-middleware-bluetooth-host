# Characteristic write procedure

There is a general API that may be used for writing Characteristic Values:

```
bleResult_t **GattClient\_WriteCharacteristicValue**
(
    deviceId_t                         deviceId,
    const gattCharacteristic_t *       pCharacteristic,
    uint16_t                           valueLength,
    const uint8_t *                    aValue,
    bool_t                             withoutResponse,
    bool_t                             signedWrite,
    bool_t                             doReliableLongCharWrites,
    const uint8_t *                    aCsrk
);
```

It has many parameters to support different combinations of Characteristic Write Procedures.

The structure pointed to by the *pCharacteristic* is only used for the *value.handle*field which indicates the Characteristic Value Handle. The value to be written is contained in the *aValue* array of size *valueLength*.

The *withoutResponse* parameter can be set to *TRUE* if the application wishes to perform a Write Without Response Procedure, which translates into an ATT Write Command. If this value is selected, the *signedWrite* parameter indicates whether data should be signed \(Signed Write Procedure over ATT Signed Write Command\), in which case the *aCsrk* parameters must not be NULL and contains the CSRK to sign the data with. Otherwise, both *signedWrite* and *aCsrk* are ignored.

Finally, *doReliableLongCharWrites* should be sent to *TRUE* if the application is writing a long Characteristic Value \(one that requires multiple air packets due to *ATT\_MTU* limitations\) and wants the Server to confirm each part of the attribute that is sent over the air.

To simplify the application code, the following macros are defined:

```
**\#define** GattClient_SimpleCharacteristicWrite(deviceId, pChar, valueLength, aValue) \
    GattClient_WriteCharacteristicValue\
        (deviceId, pChar, valueLength, aValue, FALSE, FALSE, FALSE, NULL)
```

This is the simplest usage for writing a Characteristic. It sends an ATT Write Request if the value length does not exceed the maximum space for an over-the-air packet \(*ATT\_MTU – 3*\). Otherwise, it sends ATT Prepare Write Requests with parts of the attribute, without checking the ATT Prepare Write Response data for consistency, and in the end an ATT Execute Write Request.

```
**\#define** GattClient_CharacteristicWriteWithoutResponse(deviceId, pChar, valueLength, aValue) \
    GattClient_WriteCharacteristicValue\
        (deviceId, pChar, valueLength, aValue, TRUE, FALSE, FALSE, NULL)
```

This usage sends an ATT Write Command. Long Characteristic values are not allowed here and trigger a *gBleInvalidParameter\_c* error.

```
**\#define** GattClient_CharacteristicSignedWrite(deviceId, pChar, valueLength, aValue, aCsrk) \
    GattClient_WriteCharacteristicValue\
        (deviceId, pChar, valueLength, aValue, TRUE, TRUE, FALSE, aCsrk)
```

This usage sends an ATT Signed Write Command. The CSRK used to sign data must be provided.

This is a short example to write a 3-byte long Characteristic Value.

```
gattCharacteristic_t myChar;
myChar. value . handle = 0x00A0; /* Or maybe it was previously discovered? */
**\#define** mcValueLength_c 3
uint8_t aValue[mcValueLength_c] = { 0x01, 0x02, 0x03 };
bleResult_t result = **GattClient\_SimpleCharacteristicWrite**
(
    deviceId,
    &myChar,
    mcValueLength_c,
    aValue
);
**if** (*gBleSuccess\_c* != result)
{
    /* Handle error */
}
```

The Client Procedure Callback is triggered when writing is complete.

```
**void ****gattClientProcedureCallback**
(
    deviceId_t                 deviceId,
    gattProcedureType_t        procedureType,
    gattProcedureResult_t      procedureResult,
    bleResult_t                error
)
{
    **switch** (procedureType)
    {
        /* ... */
        **case***gGattProcWriteCharacteristicValue\_c*:
            **if** (*gGattProcSuccess\_c* == procedureResult)
            {
                /* Continue */
            }
            **else**
            {
                /* Handle error */
                PRINT(error);
            }
            **break**;
        /* ... */
    }
}
```

**Parent topic:**[Reading and writing characteristics](../topics/reading_and_writing_characteristics.md)

