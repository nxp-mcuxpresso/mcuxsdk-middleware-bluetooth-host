# Declaring custom 128-bit UUIDs

All Custom 128-bit UUIDs are declared in the required file *gatt\_uuid128.h*.

Each line in this file contains a single UUID declaration. The declaration uses the following macro:

-   *UUID128 \(name, byte1, byte2, …, byte16\)*

The*name* parameter is the user-friendly handle that references this UUID in the *gatt\_db.h*file.

The 16 bytes are written in the *LSB-first* order each one using the 0xZZ format.

**Note:** On some occasions, it is desired to reuse an 128-bit UUID declared in *gatt\_uuid128.h*. The 16 byte array is available through its friendly name and be accessed by including *gatt\_db\_handles.h* in the application. It is strongly advised to use it only in read-only operations. For example:

```

(gatt_uuid128.h)
UID128(uuid_service_wireless_uart, 0xE0, 0x1C, 0x4B, 0x5E, 0x1E, 0xEB, 0xA1, 0x5C, 0xEE, 0xF4, 0x5E, 0xBA, 0x00, 0x01, 0xFF, 0x01)
(app.c)
#include "gatt_db_handles.h"
........
/* Start Service Discovery*/
BleServDisc_FindService(peerDeviceId, gBleUuidType128_c, (bleUuid_t*) &uuid_service_wireless_uart);
```

**Parent topic:**[Creating static GATT database](../topics/creating_static_gatt_database.md)

