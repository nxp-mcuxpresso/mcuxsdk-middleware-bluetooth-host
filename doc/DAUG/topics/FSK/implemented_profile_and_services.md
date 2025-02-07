# Implemented profile and services

The Hybrid \(Dual-Mode\) Bluetooth Low Energy and Generic FSK application implements the GATT client and server for the custom Wireless UART profile and services. It also acts as a Generic FSK transmitter/receptor, repeating a custom packet, at a fixed periodic interval. It uses a predefined identifier, isolated to the address used in the Bluetooth LE protocol of the demo.

-   Wireless UART Service \(UUID: 01ff0100-ba5e-f4ee-5ca1-eb1e5e4b1ce0\)
-   Battery Service v1.0
-   Device Information Service v1.1

The Wireless UART service is a custom service that implements a custom writable ASCII Char characteristic \(UUID: 01ff0101-ba5e-f4ee-5ca1-eb1e5e4b1ce0\) that holds the character written by the peer device.

The application is ready to start either Bluetooth LE scanning for Wireless UART Service, Bluetooth LE advertising Wireless UART Service, Generic FSK periodic transmit of a custom packet or Generic FSK receive, in the available slots not used by the Bluetooth LE protocol.

**Parent topic:**[Hybrid \(Dual-mode\) Bluetooth Low Energy and Generic FSK](../../topics/FSK/hybrid_dual-mode_bluetooth_low_energy_and_generic_.md)

