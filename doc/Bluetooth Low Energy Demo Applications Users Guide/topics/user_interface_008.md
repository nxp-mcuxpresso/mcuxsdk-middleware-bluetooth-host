# User interface

After flashing the board, the central is in Idle mode \(all LEDs flashing\). To start scanning, press the **SCANSW** button. After connecting to the peripheral, the **CONNLED** turns solid. The data information together with the bearer it was received on is sent over UART. To disconnect the node, hold the **SCANSW** button pressed for 2-3 seconds. The node then restarts scanning.

See the table below for hardware references for EATT Central.

| Platform                                 | SCANSW | CONNLED  |
| ---------------------------------------- | ------ | -------- |
| KW45B41Z-EVK / K32W148-EVK / MCX-W71-EVK | SW2    | LED2     |
| FRDM-MCXW71                              | SW2    | Blue LED |
| KW47-EVK / MCX-W72-EVK                   | SW2    | LED2     |
| FRDM-MCXW72                              | SW4    | Blue LED |

**Parent topic:**[EATT Central](../topics/eatt_central.md)

