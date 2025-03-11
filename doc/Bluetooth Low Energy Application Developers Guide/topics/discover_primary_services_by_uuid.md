# Discover primary services by UUID

To discover only Primary Services of a known type \(Service UUID\), the following API can be used:

```
bleResult_t GattClient_DiscoverPrimaryServicesByUuid
(
    deviceId_t         deviceId,
    bleUuidType_t      uuidType,
    const bleUuid_t *  pUuid,
    gattService_t *    aOutPrimaryServices,
    uint8_t            maxServiceCount,
    uint8_t *          pOutDiscoveredCount
);
```

The procedure is very similar to the one described in [Discover all primary services](discover_all_primary_services.md#). The only difference is this time we are filtering the search according to a Service UUID described by two extra parameters: *pUuid* and *uuidType*.

This procedure is useful when the Client is only interested in a specific type of Services. Usually, it is performed on Servers that are known to contain a certain Service, which is specific to a certain profile. Therefore, most of the times the search is expected to find a single Service of the given type. As a result, only one structure is usually allocated.

For example, when two devices implement the Heart Rate \(HR\) Profile, an HR Collector connects to an HR Sensor and may only be interested in discovering the Heart Rate Service \(HRS\) to work with its Characteristics. The following code example shows how to achieve this. Standard values for Service and Characteristic UUIDs, as defined by the Bluetooth SIG, are located in the *ble\_sig\_defines.h* file.

```
static gattService_t heartRateService;
static uint8_t mcHrs;
bleResult_t result = GattClient_DiscoverPrimaryServicesByUuid
(
    deviceId,
    gBleUuidType16_c,             /* Service UUID type */
    gBleSig_HeartRateService_d,   /* Service UUID */
    &heartRateService,            /* Only one HRS is expected to be found */
    1,
    &mcHrs                       
   /* Will be equal to 1 at the end of the procedure 
    if the HRS is found, 0 otherwise */
                                  
);
if (gBleSuccess_c != result)
{
    /* Treat error */
}
```

In the Client Procedure Callback, the application should check if any Service with the given UUID was found and read its handle range \(also perhaps proceed with Characteristic Discovery within that service range\).

```
void gattClientProcedureCallback
(
    deviceId_t deviceId,
    gattProcedureType_t procedureType,
    gattProcedureResult_t procedureResult,
    bleResult_t error
)
{
    switch (procedureType)
    {
        /* ... */
        case gGattProcDiscoverPrimaryServicesByUuid_c:
            if (gGattProcSuccess_c == procedureResult)
            {
                if (1 == mcHrs)
                {
                    /* HRS found, read the handle range */
                    PRINT( heartRateService. startHandle );
                    PRINT( heartRateService. endHandle );
                }
                else
                {
                    /* HRS not found! */
                }
            }
            else
            {
                /* Handle error */
                PRINT( error );
            }
            break;
        /* ... */
    }
}
```

**Parent topic:**[Service and characteristic discovery](../topics/service_and_characteristic_discovery.md)

