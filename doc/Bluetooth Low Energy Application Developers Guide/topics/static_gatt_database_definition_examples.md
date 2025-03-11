# Static GATT database definition examples

The GAP Service must be present on any GATT Database. It has the Service UUID equal to 0x1800, *<<GAP Service\>\>*, and it contains three read-only Characteristics, no authentication required: *Device Name, Appearance,*and *Peripheral Preferred Connection Parameters*. These also have well defined UUIDs in the SIG documents.

Most of the demos also include the optional GATT Security Levels characteristic, which defines the highest security requirements of the GATT server when operating in a LE connection.

The definition for this Service is shown here:

```
PRIMARY_SERVICE(service_gap, 0x1800)
    CHARACTERISTIC(char_device_name, 0x2A00, (gGattCharPropRead_c) )
         VALUE(value_device_name, 0x2A00, (gGattAttPermAccessReadable_c), 6, “Sensor”)
    CHARACTERISTIC(char_appearance, 0x2A01, (gGattCharPropRead_c) )
        VALUE(value_appearance, 0x2A01, (gGattAttPermAccessReadable_c), 2,
         0xC2, 0x03)
    CHARACTERISTIC(char_ppcp, 0x2A04, (gGattCharPropRead_c) )
         VALUE(value_ppcp, 0x2A04, (gGattAttPermAccessReadable_c), 8, 0x0A,
         0x00, 0x10, 0x00, 0x64, 0x00, 0xE2, 0x04)
    CHARACTERISTIC(char_security_levels, gBleSig_GattSecurityLevels_d,
        (gGattCharPropRead_c) )
         VALUE(value_security_levels, gBleSig_GattSecurityLevels_d,
         (gPermissionFlagReadable_c), 2, 0x01, 0x01)
```

Another often encountered Service is the Scan Parameters Service:

```
PRIMARY_SERVICE(service_scan_parameters, 0x1813)
       CHARACTERISTIC(char_scan_interval_window, 0x2A4F,            (gGattCharPropWriteWithoutRsp_c) )
         VALUE(value_scan_interval_window, 0x2A4 (gGattAttPermAccessWritable), 4, 0x00, 0x00, 0x00, 0x00)
        CHARACTERISTIC(char_scan_refresh, 0x2A31, (gGattCharPropRead_c gGattCharPropNotify_c) )
         VALUE(value_scan_refresh, 0x2A31, (gGattAttPermAccessReadable_c), 1, 0x00) CCCD(cccd_scan_refresh)
```

**Note:** All “user-friendly” names given in declarations are statically defined as *enum* members, numerically equal to the *attribute handle* of the declaration. This means that one of those names can be used in code wherever an attribute handle is required as a parameter of a function if *gatt\_db\_handles.h* is included in the application source file. For example, to write the value of the Scan Refresh Characteristic from the application-level code, use these instructions:

```
#include "gatt_db_handles.h"
...
uint8_t scan_refresh_value = 0x12;
GattDb_WriteAttribute(char_scan_refresh, 1, &scan_refresh_value);
```

For static database declarations, the 'attribute handle' is equal to the line number in the `gatt_fb.h` file, where the attribute is defined.

**Parent topic:**[Creating static GATT database](../topics/creating_static_gatt_database.md)

