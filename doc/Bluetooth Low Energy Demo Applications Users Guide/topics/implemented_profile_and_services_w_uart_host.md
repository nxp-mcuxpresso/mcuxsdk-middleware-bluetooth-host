# Implemented profile and services

The Wireless UART Host application implements both the GATT client and server for the custom Wireless UART profile and services.

-   Wireless UART Service \(UUID: 01ff0100-ba5e-f4ee-5ca1-eb1e5e4b1ce0\)
-   Battery Service v1.0
-   Device Information Service v1.1

The Wireless UART service is a custom service that implements a custom writable ASCII Char characteristic \(UUID: 01ff0101-ba5e-f4ee-5ca1-eb1e5e4b1ce0\) that holds the character written by the peer device.

The application behaves at first as a GAP central node. It enters GAP Limited Discovery Procedure and searches for other Wireless UART devices to connect. To change the device role to GAP peripheral, use the **ROLESW** button. The device enters GAP General Discoverable Mode and waits for a GAP central node to connect.

**Parent topic:**[Wireless UART Host](../topics/wireless_uart_host.md)

