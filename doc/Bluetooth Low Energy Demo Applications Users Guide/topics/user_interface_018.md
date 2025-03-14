# User interface

After flashing the board, the device is in idle mode \(all LEDs flashing\). To start scanning, press the **SCANSW** button. When in GAP Limited Discovery Procedure, **CONNLED** is flashing. When the central node connects to the peripheral, **CONNLED** turns solid. To disconnect the node, hold the **SCANSW** button pressed for 2-3 seconds. The node then re-enters GAP Limited Discovery Procedure.

See [User interface](user_interface_018.md) below for hardware references.

| Platform                                 | SCANSW | CONNLED  |
| ---------------------------------------- | ------ | -------- |
| KW45B41Z-EVK / K32W148-EVK / MCX-W71-EVK | SW2    | LED2     |
| FRDM-MCXW71                              | SW2    | Blue LED |
| KW47-EVK / MCX-W72-EVK                   | SW2    | LED2     |
| FRDM-MCXW72                              | SW4    | Blue LED |

**Parent topic:**[HID Host](../topics/hid_host.md)

