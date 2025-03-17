# Customization

Use the steps below to change the default settings of this demo.

For Bluetooth LE, the default advertising config \(`gAdvParams`\) parameter is found in the `app_config.c` file. Also, the scanning parameters \(gScanParams\) can be found in this file.

**Note:** The Generic FSK protocol is active during the inactive periods of the Bluetooth LE protocol. The demo is currently configured to have the scan window equal to the scan interval to make the user aware of this, but this can be changed.

For Generic FSK, the following defines of interest can be found in `genfsk_app.h`, described below:

| Name                       | Description |
| -------------------------- |------------ |
| `gGenFSK_NetworkAddress_c` | This is the network address used for the Generic FSK, in the transmitter payload. It is implicitly set to the 0x8E89BED6, but this can be reconfigured. Ensure that it is also changed on the receiver in the `hybrid_gfsk.c` controller file. |
| `gGenFSK_H0Value_c`        | H0 value is used in the header. |
| `gGenFSK_Identifier_c`     | This is the identifier used by the transmitter to be filtered at the receiver. The current implementation filters the Generic FSK packets received, based on this define. |
| `gGenFskApp_TxInterval_c`  | This is the interval the transmitter will repeat the transmission of a packet. It is set in milliseconds. |

**Files of interest**

The demo can be found in the `w_uart_genfsk` from the available examples.

The demo is based on the basic Wireless UART with the addition of some Generic FSK files required for working in dual-mode, described below.

| File name       | Description |
| --------------- | ----------- |
| `genfsk_app.c`  | Application common module. Handles the HCI commands and events for the Generic FSK. Sends the events to the application. |
| `genfsk_app.h`  | Application common module. Exposes public functions. |
| `hybrid_gfsk.c` | Controller common module. Handles initialization of Generic FSK. |
| `hybrid_gfsk.h` | Controller common module. Exposes public functions. |

**Parent topic:**[Hybrid \(Dual-mode\) Bluetooth Low Energy and Generic FSK](../topics/hybrid_dual-mode_bluetooth_low_energy_and_generic_.md)

