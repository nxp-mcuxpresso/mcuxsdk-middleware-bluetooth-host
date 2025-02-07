# Adding characteristics and descriptors

The APIs for adding Characteristics and Descriptors are enumerated below:

-   *GattDbDynamic\_AddCharacteristicDeclarationAndValue*
    -   The Characteristic UUID, properties, access permissions, and initial value are specified as parameters.

-   *GattDbDynamic\_AddCharacteristicDeclarationWithUniqueValue*
    -   Multiple calls to this API allocate a unique 512-byte value buffer as an optimization for application that deal with large value buffers that do not always need to be stored separately.

-   *GattDbDynamic\_AddCharacteristicDescriptor*
    -   The Descriptor UUID, access permissions and initial value are specified as parameters.

-   *GattDbDynamic\_AddCccd*
    -   Shortcut for a CCCD.


Characteristics and descriptors are automatically added at the end of the database. Thus, a service declaration should be followed by all desired characteristic and descriptor definitions before adding a new service to the database.

**Parent topic:**[Creating a GATT database dynamically](../topics/creating_a_gatt_database_dynamically.md)

