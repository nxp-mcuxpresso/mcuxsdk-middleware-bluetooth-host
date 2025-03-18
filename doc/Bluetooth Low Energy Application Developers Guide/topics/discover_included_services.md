# Discover included services

[Discover all primary services](discover_all_primary_services.md#) shows how to discover Primary Services. However, a Server may also contain Secondary Services, which are not meant to be used standalone and are usually included in the Primary Services. The inclusion means that all the Secondary Service’s Characteristics may be used by the profile that requires the Primary Service.

Therefore, after a Primary Service has been discovered, the following procedure may be used to discover services \(usually Secondary Services\) included in it:

```
bleResult_t GattClient_FindIncludedServices
(
    deviceId_t         deviceId,
    gattService_t *    pIoService,
    uint8_t            maxServiceCount
);
```

The service structure that *pIoService* points to must have the *aIncludedServices* field linked to an allocated array of services, of size *maxServiceCount*, chosen according to the expected number of included services to be found. This is the application’s choice, usually following profile specifications.

Also, the service’s range must be set \(the *startHandle* and *endHandle* fields\), which may have already been done by the previous Service Discovery procedure \(as described in [Discover all primary services](discover_all_primary_services.md#) and [Discover primary services by UUID](discover_primary_services_by_uuid.md#)\).

The number of discovered included services is written by the GATT module in the *cNumIncludedServices* field of the structure from *pIoService*. Obviously, a maximum of *maxServiceCount* included services is discovered.

The following example assumes the Heart Rate Service was discovered using the code provided in [Discover primary services by UUID](discover_primary_services_by_uuid.md#).

```
/* Finding services included in the Heart Rate Primary Service */
gattService_t * pPrimaryService = &heartRateService;
#define mxMaxIncludedServices_c 3
static gattService_t includedServices[mxMaxIncludedServices_c];
/* Linking the array */
pPrimaryService-> aIncludedServices = includedServices;
bleResult_t result = GattClient_FindIncludedServices
(
    deviceId,
    pPrimaryService,
    mxMaxIncludedServices_c
);
if (gBleSuccess_c != result)
{
    /* Treat error */
}
```

When the Client Procedure Callback is triggered, if any included services are found, the application can read their handle range and their UUIDs.

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
        case gGattProcFindIncludedServices_c:
            if (gGattProcSuccess_c == procedureResult)
            {
                /* Read included services data */
                PRINT( pPrimaryService-> cNumIncludedServices );
                for (int j = 0; j < pPrimaryService-> cNumIncludedServices ; j++)
                {
                    PRINT( pPrimaryService-> aIncludedServices [j]. startHandle );
                    PRINT( pPrimaryService-> aIncludedServices [j]. endHandle );
                    PRINT( pPrimaryService-> aIncludedServices [j]. uuidType );
                    PRINT( pPrimaryService-> aIncludedServices [j]. uuid );
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

