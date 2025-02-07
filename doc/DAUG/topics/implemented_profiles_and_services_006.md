# Implemented profiles and services

The HID Device application implements a GATT server and the following profile and services.

-   HID over GATT Profile v1.0
-   Human Interface Device Service v1.0
-   Battery Service v1.0
-   Device Information Service v1.1

The application behaves as a GAP peripheral node. It enters GAP General Discoverable Mode and waits for a GAP central node to connect. Security on the services and bonding is enabled on this device.

When the GATT client configures notification, the application starts sending HID reports every two seconds with the movement of the MOUSE\_STEP. The demo moves the cursor in a square pattern between AXIS\_MIN and AXIS\_MAX. The report contains 3 bytes, one for button status, one for X axis, and one for Y axis. The report descriptor matches the example in chapter E.10 from the USB Device Class Definition for Human Interface Devices \(USB HID Specification\), Version 1.11.

**Parent topic:**[HID Device \(Mouse\)](../topics/hid_device_mouse.md)

