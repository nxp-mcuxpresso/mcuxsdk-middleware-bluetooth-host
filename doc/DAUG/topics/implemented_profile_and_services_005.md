# Implemented profile and services

The adv\_ext\_peripheral application implements a GATT server, a custom profile and the following services.

-   Temperature Service \(UUID: 01ff0200-ba5e-f4ee-5ca1-eb1e5e4b1ce0\)
-   Battery Service v1.0
-   Device Information Service v1.1

The application behaves as a GAP peripheral node. It enters GAP General Discoverable Mode and waits for a GAP central node to connect and configure notifications for the temperature value.

The Temperature service is a custom service that implements the Temperature characteristic \(UUID: 0x2A6E\) with a Characteristic Presentation Format descriptor \(UUID: 0x2904\), both defined by the Bluetooth SIG.

The adv\_ext\_central application implements a GATT client or server for the following profile and services.

-   Temperature Service \(UUID: 01ff0200-ba5e-f4ee-5ca1-eb1e5e4b1ce0\)
-   Battery Service v1.0
-   Device Information Service v1.1

The application behaves as a GAP central node. It enters GAP Limited Discovery Procedure and searches for peripherals devices to pair with. After pairing with the peripheral, it configures notifications and displays temperature values on a terminal connected to the UART port.

Both applications use pairing with bonding by default. When connected with the Low-Power Extended Advertising Peripheral application, the Extended Advertising Central application sends the 999999 passcode to the host stack by default.

**Parent topic:**[Low-power extended advertising Peripheral and Central](../topics/low-power_extended_advertising_peripheral_and_exte.md)

