# Adding services

The APIs that can be used to add Services are self-explanatory:

-   *GattDbDynamic\_AddPrimaryServiceDeclaration*
    -   The Service UUID is specified as parameter.

        Memory requirements: one entry in the attribute buffer and UUID size in value buffer.

-   *GattDbDynamic\_AddSecondaryServiceDeclaration*
    -   The Service UUID is specified as parameter.

        Memory requirements: one entry in the attribute buffer and UUID size in value buffer.

-   *GattDbDynamic\_AddIncludeDeclaration*
    -   The Service UUID and handle range are specified as parameters.

        Memory requirements: one entry in the attribute buffer and 6 bytes in value buffer.


The functions have an optional out parameter *pOutHandle*. If its value is not NULL, the execution writes a 16-bit value in the pointed location representing the attribute handle of the added declaration. The application can use this handle as parameter in few *GattDbApp* APIs or in the Service removal functions.

At least one Service must be added before any Characteristic.

**Parent topic:**[Creating a GATT database dynamically](../topics/creating_a_gatt_database_dynamically.md)

