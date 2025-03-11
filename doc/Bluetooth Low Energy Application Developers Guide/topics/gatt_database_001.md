# GATT database

The *gatt\_db* contains a set of header files grouped in the *macros* subfolder. These macros are used for static code generation for the GATT Database by expanding the contents of the *gatt\_db.h* file in different ways. [Creating GATT database](creating_gatt_database.md#) explains how to write the *gatt\_db.h* file using user-friendly macros that define the GATT database.

At application compile time, the *gatt\_database.c* file is populated with enumerations, structures, and initialization code used to allocate and properly populate the GATT database. In this way, the *gattDatabase*array and the *gGattDbAttributeCount\_c* variable \(see [GATT database](gatt_database.md#)\) are created and properly initialized.

**Note:** Do not modify any of the files contained in the *gatt\_db* folder and its subfolder.

To complete the GATT database initialization, this demo application includes the required `gatt_db.h` and `gatt_uuid128.h` files in its specific application folder, along with other profile-specific configuration and code files.

**Parent topic:**[Application Structure](../topics/application_structure.md)

