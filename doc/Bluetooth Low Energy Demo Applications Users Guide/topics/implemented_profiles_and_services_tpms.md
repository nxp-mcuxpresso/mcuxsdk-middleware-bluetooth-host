# Implemented profiles and services

The TPMS Sensor application implements a GATT server, the TPMS profile and the following services.

-   Tire Pressure Monitoring System Service (TPMS)
-   Battery Service v1.0
-   Device Information Service v1.1

The application behaves as a GAP peripheral node. It enters GAP Limited Discoverable Mode to perform either connectable or non-connectable advertising. When performing connectable advertising, it waits for a Primary Monitor to connect and configure it, as defined by the TPMS Profile. When performing non-connectable advertising, it broadcasts packets containing the tire pressure and temperature values.

The TPMS Service is defined by the Bluetooth SIG.

The TPMS Monitor application implements a GATT client or server for the following profile and services.

-   Tire Pressure Monitoring System Service (TPMS)
-   Battery Service v1.0
-   Device Information Service v1.1

The application behaves as a GAP central node. It enters GAP Limited Discovery Procedure and searches for connectable or non-connectable advertising packets from TPMS Sensors. When a TPMS Sensor is found, the Monitor can connect to it, pair, bond, and read/write TPMS characteristics as defined by the TPMS Profile. The Monitor can also receive non-connectable broadcast packets containing tire pressure and temperature values from sensors.

**Parent topic:**[TPMS Sensor and Monitor](../topics/tpms_sensor_and_monitor.md)

