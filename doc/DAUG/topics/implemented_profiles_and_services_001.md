# Implemented profiles and services

The EATT Central application implements a GATT server and the following profiles and services:

-   Generic Attribute Profile

The application behaves as a GAP central node. It searches for an EATT peripheral to connect to. After connecting, it performs service discovery, initiates an EATT connection and configures indications on the peripheral for services A and B. The Central reports the received service data and the steps taken during the setup on a terminal connected to an UART port.

**Parent topic:**[EATT Central](../topics/eatt_central.md)

