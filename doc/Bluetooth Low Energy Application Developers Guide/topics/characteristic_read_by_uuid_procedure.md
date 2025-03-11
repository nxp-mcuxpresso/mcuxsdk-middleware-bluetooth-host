# Characteristic read by UUID procedure

This API for this procedure is shown here:

```
bleResult_t GattClient_ReadUsingCharacteristicUuid
(
    deviceId_t               deviceId,
    bleUuidType_t            uuidType,
    const bleUuid_t*         pUuid,
    const gattHandleRange_t* pHandleRange,
    uint8_t*                 aOutBuffer,
    uint16_t                 maxReadBytes,
    uint16_t*                pOutActualReadBytes
);
```

This provides support for an important optimization, which involves reading a Characteristic Value without performing any Service or Characteristic Discovery.

For example, the following is the process to write an application that connects to any Server and wants to read the device name.

The device name is contained in the Device Name Characteristic from the GAP Service. Therefore, the necessary steps involve discovering all primary services, identifying the GAP Service by its UUID, discovering all Characteristics of the GAP Service and identifying the Device Name Characteristic \(alternatively, discovering Characteristic by UUID inside GAP Service\), and, finally, reading the device name by using the Characteristic Read Procedure.

Instead, the Characteristic Read by UUID Procedure allows reading a Characteristic with a specified UUID, assuming one exists on the Server, without knowing the Characteristic Value Handle.

The described example is implemented as follows:

```
#define mcMaxValueLength_c
/* First byte is for handle-value pair length. Next 2 bytes are the handle */
static uint8_t aValue[1 + 2 + mcMaxValueLength_c];
static uint16_t deviceNameLength;
bleUuid_t uuid = {
        .uuid16 = gBleSig_GapDeviceName_d
};
bleResult_t result = GattClient_ReadUsingCharacteristicUuid
(
    deviceId,
    gBleUuidType16_c,
    &uuid,
    &pHandleRange,
    aValue,
    1 + 2 + mcMaxValueLength_c,
    deviceNameLength
);
if (gBleSuccess_c != result)
{
    /* Handle error */
}
```

The Client Procedure Callback is triggered when the reading is complete. Because only one air packet is exchanged during this procedure, it can only be used as a quick reading of Characteristic Values with length no greater than *ATT\_MTU – 1*.

```
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
         case gGattProcReadUsingCharacteristicUuid\_c*:
              if (gGattProcSuccess_c == procedureResult)
            {
               /* Read handle-value pair length */
                PRINT(aValue[0]);
                deviceNameLength -= 1;
              /* Read characteristic value handle */
                PRINT(aValue[1] | (aValue[2] << 8));
                deviceNameLength -= 2;
                /* Read value length */
                PRINT(deviceNameLength);
                /* Read data */
                for ( uint8_t j = 0; j < deviceNameLength; j++)
                {
                    PRINT(aValue[3 + j]);
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

