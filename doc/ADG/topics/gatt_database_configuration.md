# GATT database configuration

The GATT database always resides on the same processor as the entire Bluetooth Low Energy Host Stack, so the attributes must be added by the host application using the serial communication interface.

To create a GATT database remotely, *GATTDBDynamic* commands must be used. The GATTDBDynamic API is provided to the user that performs all the required memory allocations and sends the FSCI commands to the blackbox. The result of the operation is returned, including optionally the service, characteristic, and '*`cccd`*' handles returned by the blackbox.

Current supported API for adding services is the following:

```
bleResult_t **GattDbDynamic\_AddGattService** (gattServiceHandles_t* pOutServiceHandles);
bleResult_t **GattDbDynamic\_AddGapService** (gapServiceHandles_t* pOutServiceHandles);
bleResult_t **GattDbDynamic\_AddIpssService** (ipssServiceHandles_t* pOutServiceHandles);
bleResult_t **GattDbDynamic\_AddHeartRateService** (heartRateServiceHandles_t* pOutServiceHandles);
bleResult_t **GattDbDynamic\_AddBatteryService** (batteryServiceHandles_t* pOutServiceHandles);
bleResult_t **GattDbDynamic\_AddDeviceInformationService** (deviceInfoServiceHandles_t* pOutServiceHandles);
```

The service handles are optional.

Also, a generic function is provided, so that the user can add any generic service to the database:

```
bleResult_t **GattDbDynamic\_AddServiceInDatabase** (serviceInfo_t* pServiceInfo);
```

Usually, a Bluetooth Low Energy Application is ported from a single chip solution, where the Bluetooth Low Energy Application and the Bluetooth Low Energy stack reside on the same processor and the GATT database is populated statically. The user should remove all the attribute handles from any structure and replace them with *gGattDbInvalidHandle\_d*. The attribute handles should be populated after the services are added dynamically to the database with the handles returned by the previous API.

**Parent topic:**[Creating a Bluetooth LE application when the Host Stack runs on another processor](../topics/creating_a_bluetooth_low_energy_application_when_t.md)

