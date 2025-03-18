# Defining custom UUIDs

The first step when defining a new service included in a profile is to define the custom 128-bit UUID for the service and the included characteristics. These values are defined in *gatt\_uuid128.h*, which is located in the application folder. For example, the Temperature Profile uses the following UUID for the service:

```
/* Temperature */
UUID128(uuid_service_temperature, 0xfb ,0x34 ,0x9b ,0x5f ,0x80 ,0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x02, 0x00, 0xfe, 0x00, 0x00)
```

The definition of the services and characteristics are made in *gattdb.h*, as explained in [Creating GATT database](creating_gatt_database.md#). For more details on how to structure the database, see [Application Structure](application_structure.md#).

**Parent topic:**[Creating a Custom Profile](../topics/creating_a_custom_profile.md)

