# Static GATT database definition examples

The GAP Service must be present on any GATT Database. It has the Service UUID equal to 0x1800, *<<GAP Service\>\>*, and it contains three read-only Characteristics, no authentication required: *Device Name, Appearance,*and *Peripheral Preferred Connection Parameters*. These also have well defined UUIDs in the SIG documents.

Most of the demos also include the optional GATT Security Levels characteristic, which defines the highest security requirements of the GATT server when operating in a LE connection.

The definition for this Service is shown here:

```
**PRIMARY\_SERVICE\(service\_gap, 0x1800\) **
    **CHARACTERISTIC\(char\_device\_name, 0x2A00, \(gGattCharPropRead\_c\) \)** 
            **VALUE\(value\_device\_name, 0x2A00, \(gGattAttPermAccessReadable\_c\), ** **6, “Sensor”\)** 
    **CHARACTERISTIC\(char\_appearance, 0x2A01, \(gGattCharPropRead\_c\) \)** 
            **VALUE\(value\_appearance, 0x2A01, \(gGattAttPermAccessReadable\_c\), 2, 0xC2, 0x03\)** 
    **CHARACTERISTIC\(char\_ppcp, 0x2A04, \(gGattCharPropRead\_c\) \)** 
            **VALUE\(value\_ppcp, 0x2A04, \(gGattAttPermAccessReadable\_c\), ****8, 0x0A, 0x00, 0x10, 0x00, 0x64, 0x00, 0xE2, 0x04\)**
    **CHARACTERISTIC\(char\_security\_levels, gBleSig\_GattSecurityLevels\_d, \(gGattCharPropRead\_c\) \)
            VALUE\(value\_security\_levels, gBleSig\_GattSecurityLevels\_d, \(gPermissionFlagReadable\_c\), 2, 0x01, 0x01\)**
```

Another often encountered Service is the Scan Parameters Service:

```
**PRIMARY\_SERVICE\(service\_scan\_parameters, 0x1813\)** 
    **CHARACTERISTIC\(char\_scan\_interval\_window, 0x2A4F, \(gGattCharPropWriteWithoutRsp\_c\) \)** 
        **VALUE\(value\_scan\_interval\_window, 0x2A4F, \(gGattAttPermAccessWritable\), ** **4, 0x00, 0x00, 0x00, 0x00\)** 
    **CHARACTERISTIC\(char\_scan\_refresh, 0x2A31, \(gGattCharPropRead\_c \| gGattCharPropNotify\_c\) \)** 
        **VALUE\(value\_scan\_refresh, 0x2A31, \(gGattAttPermAccessReadable\_c\), 1, 0x00\)** **CCCD\(cccd\_scan\_refresh\)**
```

**Note:** All “user-friendly” names given in declarations are statically defined as *enum* members, numerically equal to the *attribute handle* of the declaration. This means that one of those names can be used in code wherever an attribute handle is required as a parameter of a function if *gatt\_db\_handles.h* is included in the application source file. For example, to write the value of the Scan Refresh Characteristic from the application-level code, use these instructions:

```
#include "gatt_db_handles.h"
...
uint8_t scan_refresh_value = 0x12;
*GattDb\_WriteAttribute\(char\_scan\_refresh, 1, &scan\_refresh\_value\);*
```

For static database declarations, the 'attribute handle' is equal to the line number in the `gatt_fb.h` file, where the attribute is defined.

**Parent topic:**[Creating static GATT database](../topics/creating_static_gatt_database.md)

