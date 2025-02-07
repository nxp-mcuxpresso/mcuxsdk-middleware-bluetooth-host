# Discover all characteristics of a service

The main API for Characteristic Discovery has the following prototype:

```
bleResult_t **GattClient\_DiscoverAllCharacteristicsOfService**
(
    deviceId_t         deviceId,
    gattService_t *    pIoService,
    uint8_t            maxCharacteristicCount
);
```

All required information is contained in the service structure pointed to by *pIoService*, most importantly being the service range \(*startHandle* and *endHandle*\) which is usually already filled out by a Service Discovery procedure. If not, they need to be written manually.

Also, the service structure’s *aCharacteristics* field must be linked to an allocated characteristic array.

The following example discovers all Characteristics contained in the Heart Rate Service discovered in Section [Discover primary services by UUID](discover_primary_services_by_uuid.md#).

```
gattService_t* pService = &heartRateService
**\#define** mcMaxCharacteristics_c 10
**static **gattCharacteristic_t hrsCharacteristics[mcMaxCharacteristics_c];
pService->aCharacteristics = hrsCharacteristics;
bleResult_t result = GattClient_DiscoverAllCharacteristicsOfService
(
    deviceId,
    pService,
    mcMaxCharacteristics_c
);
```

The Client Procedure Callback is triggered when the procedure completes.

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
      **case***gGattProcDiscoverAllCharacteristics\_c*:
          **if** (*gGattProcSuccess\_c* == procedureResult)
          {
            /* Read number of discovered Characteristics */
              PRINT(pService-> cNumCharacteristics );
            /* Read discovered Characteristics data */
              **for** ( uint8_t j = 0; j < pService-> cNumCharacteristics ; j++)
                {
            /* Characteristic UUID is found inside the value field
               to avoid duplication */
                    PRINT(pService-> aCharacteristics [j]. value . uuidType );
                    PRINT(pService-> aCharacteristics [j]. value . uuid );
            /* Characteristic Properties indicating the supported operations:
                     * - Read
                     * - Write
                     * - Write Without Response
                     * - Notify
                     * - Indicate
            */
                    PRINT(pService-> aCharacteristics [j]. properties );
           /* Characteristic Value Handle is used to identify the 
              Characteristic in future operations */
                    PRINT(pService-> aCharacteristics [j]. value . handle );
                }
            }
            **else**
            {
                /* Handle error */
                PRINT( error );
            }
            **break**;
        /* ... */
    }
}
```

**Parent topic:**[Service and characteristic discovery](../topics/service_and_characteristic_discovery.md)

