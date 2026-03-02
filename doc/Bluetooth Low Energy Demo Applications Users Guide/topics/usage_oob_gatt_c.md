# Out of band GATT Client

The application supports setting the required GATT client handles, thus avoiding service discovery after each connection.
The method for obtaining the service handles is out of the scope of this example.
To activate this feature, user must set gDbOobPopulated_c to 1.
The required handles are the Wireless UART Service handle (expressed as gDbOobServiceHandle_c) and the UART stream service handle (expressed as gDbOobUartStreamHandle_c). When gDbOobPopulated_c is set, these two macros must be also set with corect values reflecting Server's handles.

**Parent topic:**[Wireless UART](../topics/wireless_uart.md)

