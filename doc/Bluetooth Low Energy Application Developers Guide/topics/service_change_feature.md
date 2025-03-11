# Service change feature

The **service changed** feature applies to GATT servers and supports the service changed characteristic, dynamic databases, and handle value indications. The GATT clients that require to be notified for structural modifications on the database should write the CCCD of the Service Changed Characteristic on the server. The value of the Service Changed characteristic is represented by 2 handle values for the handle range affected by the modifications.

The changes that trigger a server database modification are represented by the following API calls:

-   *GattDbDynamic\_AddPrimaryServiceDeclaration*
-   *GattDbDynamic\_AddSecondaryServiceDeclaration*
-   *GattDbDynamic\_AddIncludeDeclaration*
-   *GattDbDynamic\_AddCharacteristicDeclarationAndValue*
-   *GattDbDynamic\_AddCharDescriptor*
-   *GattDbDynamic\_AddCccd*
-   *GattDbDynamic\_RemoveService*
-   *GattDbDynamic\_RemoveCharacteristic*

Those GATT server API calls update two internal handles to memorize the minimum and maximum range affected by the change.

After the GATT server database update is done, the application must call the `GattDbDynamic_EndDatabaseUpdate()` API. After this, a Service Changed indication is internally sent to each connected peer that has enabled these indications. The indication contains the handle range affected by the change.

. For bonded devices with whom the server is not currently in an active connection, the changes are buffered on the server and the peers are notified upon reconnection.

**Parent topic:**[Gatt caching](../topics/gatt_caching.md)

