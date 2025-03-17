# User interface

After flashing the board, the device enters Idle mode with all LEDs flashing. To start advertising, press the **ADVSW** button. In GAP Discoverable mode, **CONNLED** is flashing. When the central node connects to the peripheral, **CONNLED** turns solid. To disconnect the node, hold the **ADVSW** pressed for 2-3 seconds. The node then re-enters GAP Discoverable Mode.

The table below describes the hardware references.

| Platform                                 | ADVSW | CONNLED  |
| ---------------------------------------- | ----- | -------  |
| KW45B41Z-EVK / K32W148-EVK / MCX-W71-EVK | SW2   | LED2     |
| FRDM-MCXW71                              | SW2   | Blue LED |
| KW47-EVK / MCX-W72-EVK                   | SW2   | LED2     |
| FRDM-MCXW72                              | SW4   | Blue LED |

**Parent topic:**[HID Device \(Mouse\)](../topics/hid_device_mouse.md)

