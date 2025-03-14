# Case 1 \(Bluetooth LE\)

1.  Open a serial port terminal and connect to the two boards, in the same manner described in [Testing devices](testing_devices.md). The figure below displays the start screen after the board is reset.

    **Tera Term – Hybrid Wireless UART (Bluetooth LE) - Generic FSK start screen**
    ![Tera Term – Hybrid Wireless UART (Bluetooth LE) - Generic FSK start screen](../images/Fig52.png "Tera Term – Hybrid Wireless UART (Bluetooth LE) - Generic FSK start screen")

2.  The application starts as a GAP central. To switch the role to a GAP peripheral, press the **ROLESW** button. Depending on the role, when pressing the **SCANSW** button, the application starts either scanning or advertising.
3.  As soon as the **CONNLED** turns solid on both devices, the user can start writing in one of the consoles. The text appears on the other terminal.
4.  After creating a connection, the role \(central or peripheral\) is displayed on the console. The role switch can be pressed again before creating a new connection. An output example can be observed in the figure below.

    **Tera Term – received text on Wireless UART (Bluetooth LE)**
    ![Tera Term – received text on Wireless UART (Bluetooth LE)](../images/Fig53.png "Tera Term – received text on Wireless UART (Bluetooth LE)")


**Parent topic:**[Usage](../topics/usage.md)

