# Implemented profiles and services

The Temperature Sensor application implements a GATT server, a custom profile and the following services.

-   Temperature Service \(UUID: 01ff0200-ba5e-f4ee-5ca1-eb1e5e4b1ce0\)
-   Battery Service v1.0
-   Device Information Service v1.1

The application behaves as a GAP peripheral node. It enters GAP General Discoverable Mode and waits for a GAP central node to connect and configure notifications for the temperature value.

The Temperature service is a custom service that implements the Temperature characteristic \(UUID: 0x2A6E\) with a Characteristic Presentation Format descriptor \(UUID: 0x2904\), both defined by the Bluetooth SIG.

The Temperature Collector application implements a GATT client or server for the following profile and services.

-   Temperature Service \(UUID: 01ff0200-ba5e-f4ee-5ca1-eb1e5e4b1ce0\)
-   Battery Service v1.0
-   Device Information Service v1.1

The application behaves as a GAP central node. It enters GAP Limited Discovery Procedure and searches for sensor devices to pair with. After pairing with the peripheral, it configures notifications and displays temperature values on a terminal connected to the UART port.

Both application uses pairing with bonding by default. When connected with the Low-Power Temperature Sensor application, the collector sends the 999999 passcode to the host stack by default.

**Parent topic:**[Low-power temperature sensor and collector](../topics/low-power_temperature_sensor_and_collector.md)

