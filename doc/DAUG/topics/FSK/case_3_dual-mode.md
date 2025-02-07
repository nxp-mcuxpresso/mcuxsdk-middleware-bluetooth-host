# Case 3 \(Dual-mode\)

The application is ready to combine both scenarios. It can either:

-   Establish a Bluetooth LE connection and perform Generic FSK activity.
-   During Generic FSK activity, the Bluetooth LE connection can be established and the Controller pauses receiving or announcing the discarded Generic FSK transmissions. This activity is resumed after the Bluetooth LE activity is finished.

    To run the Dual-mode scenario, follow the steps below:


1.  Establish a Bluetooth LE connection as described in [Case 1 \(Bluetooth LE\)](case_1_bluetooth_le.md).
2.  Start Generic FSK activity as described in [Case 2 \(Generic FSK\)](case_2_generic_fsk.md), independent of the Bluetooth LE roles chosen in Case 1.
3.  Start typing in either of the terminals. As seen in [Figure 1](#fig_k14_2fl_ldc), the Bluetooth LE activity is prioritized. In this step, characters are printed in the peer’s terminal and Generic FSK continues in the available slots, not used by the Bluetooth LE link.

    ![](../images/Fig56.png "Teraterm – Mixed Wireless UART (Bluetooth LE) and Generic FSK
                                    activity")


**Parent topic:**[Usage](../../topics/FSK/usage.md)

