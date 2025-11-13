# Exporting HCI data

The *digital_key_car_anchor_cs* and the *digital_key_device_cs* applications can be used for exporting HCI data. These applications allow exporting the raw HCI data of all Subevent Result and Subevent Result Continue HCI events for each CS procedure. This is achieved by enabling the `gAppHciDataLogExport_d` macro in the application project’s *app_preinclude.h* header.

This feature uses the second serial interface and therefore, Connection Handover must be disabled.

The UART wires should be connected to the board's J1-1 and J1-2 pins, in the same way as described in [Running the Connection Handover scenario](running_the_connection_handover_scenario.md). At the other end, use a UART-to-USB adapter to connect to the PC. The exported data includes the local HCI LE CS Subevent Result and LE CS Subevent Result Continue events. On the PC, custom scripts can be used to process the data. These scripts are out of the scope of this note.

The format of the data is the standard HCI data format. Refer to the Core specification definitions for more details (sections 7.7.65.44 LE CS Subevent Result event and 7.7.65.45 LE CS Subevent Result Continue event):

-   OCF (1 octet)
-   OGF (1 octet)
-   Parameter total Length (1 octet)
-   Subevent\_Code (1 octet)
-   Event parameters (variable)

**Parent topic:**[Localization scenarios](../topics/localization_scenarios.md)

