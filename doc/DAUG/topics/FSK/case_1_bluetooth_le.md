# Case 1 \(Bluetooth LE\)

1.  Open a serial port terminal and connect them to the two boards, in the same manner described in in [Testing devices](../testing_devices.md). The start screen after the board is reset is presented in [Figure 1](#fig_c1d_pwl_ldc).



2.  The application starts as a GAP central. To switch the role to a GAP peripheral, press the **ROLESW** button. Depending on the role, when pressing the **SCANSW** button, the application starts either scanning or advertising.
3.  As soon as the **CONNLED** turns solid on both devices, the user can start writing in one of the consoles. The text appears on the other terminal.
4.  After creating a connection, the role \(central or peripheral\) is displayed on the console. The role switch can be pressed again before creating a new connection. An output example can be observed in [Figure 2](#002).

    ![](../images/Fig53.png "Tera Term – received text on Wireless UART (Bluetooth LE)")


**Parent topic:**[Usage](../../topics/FSK/usage.md)

