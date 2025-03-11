# Initialization and release

Before anything can be added to the database, it must be initialized with an empty collection of attributes.

The *GattDbDynamic\_Init\(\)* API is automatically called by the *GattDb\_Init\(\)* implementation provided in the *gatt\_database.c* source file. Application-specific code does not need to call this API again, unless at some point it destroys the database with *GattDb\_ReleaseDatabase\(\)*.

**Parent topic:**[Creating a GATT database dynamically](../topics/creating_a_gatt_database_dynamically.md)

