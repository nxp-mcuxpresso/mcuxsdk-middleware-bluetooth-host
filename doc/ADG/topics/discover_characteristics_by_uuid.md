# Discover characteristics by UUID

This procedure is useful when the Client intends to discover a specific Characteristic in a specific Service. The API allows for multiple Characteristics of the same type to be discovered, but most often it is used when a single Characteristic of the given type is expected to be found.

Continuing the example from [Discover primary services by UUID](discover_primary_services_by_uuid.md#), assume the Client wants to discover the Heart Rate Control Point Characteristic inside the Heart Rate Service, as shown in the following code.

```
gattService_t * pService = &heartRateService;
**static **gattCharacteristic_t hrcpCharacteristic;
**static **uint8_t mcHrcpChar;
bleResult_t result = GattClient_DiscoverCharacteristicOfServiceByUuid
(
    deviceId,
    *gBleUuidType16\_c*,
    gBleSig_HrControlPoint_d,
    pService,
    &hrcpCharacteristic,
    1,
    &mcHrcpChar
);
```

This API can be used as in the previous examples, following a Service Discovery procedure. However, the user may want to perform a Characteristic search with UUID over the entire database, skipping the Service Discovery entirely. To do so, a dummy service structure must be defined and its range must be set to maximum, as shown in the following example:

```
gatt Service_t dummyService;
dummyService. startHandle = 0x0001;
dummyService. endHandle = 0xFFFF;
**static **gattCharacteristic_t hrcpCharacteristic;
**static **uint8_t mcHrcpChar;
bleResult_t result = **GattClient\_DiscoverCharacteristicOfServiceByUuid**
(
    deviceId,
    *gBleUuidType16\_c*,
    gBleSig_HrControlPoint_d,
    &dummyService,
    &hrcpCharacteristic,
    1,
    &mcHrcpChar
);
```

In either case, the value of the *mcHrcpChar* variable should be checked in the procedure callback.

```
**void ****gattClientProcedureCallback**
(
    deviceId_t             deviceId,
    gattProcedureType_t    procedureType,
    gattProcedureResult_t  procedureResult,
    bleResult_t            error
)
{
    **switch** (procedureType)
    {
        /* ... */
        **case***gGattProcDiscoverCharacteristicByUuid\_c*:
            **if** (*gGattProcSuccess\_c* == procedureResult)
            {
                **if** (1 == mcHrcpChar)
                {
                    /* HRCP found, read discovered data */
                    PRINT(hrcpCharacteristic. properties );
                    PRINT(hrcpCharacteristic. value . handle );
                }
                **else**
                {
                    /* HRCP not found! */
                }
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

**Parent topic:**[Service and characteristic discovery](../topics/service_and_characteristic_discovery.md)

