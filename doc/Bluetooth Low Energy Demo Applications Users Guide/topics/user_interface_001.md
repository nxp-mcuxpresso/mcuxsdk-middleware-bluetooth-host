# User interface

After flashing the board, the device is in idle mode \(all LEDs flashing\). To start advertising, press the **ADVSW** button. When in GAP Discoverable Mode, **CONNLED** is flashing. When the ANCS/AMS Server \(Gap Central\) connects to the ANCS/AMS Client \(GAP Peripheral\), **CONNLED** turns solid. To disconnect, hold the **ADVSW** for 2-3 seconds. The ANCS/AMS Client then re-enters the advertising state.

For displaying operating information and ANCS Notifications \(AMS information and commands\), the demo application uses a shell exposed via a serial communication interface.

See [Table 1](#TABLE_ZV4_LDL_FDB) for hardware references.

| Platform                                 | ADVSW | CONNLED  |
| ---------------------------------------- | ----- | -------- |
| KW45B41Z-EVK / K32W148-EVK / MCX-W71-EVK | SW2   | LED2     |
| FRDM-MCXW71                              | SW2   | Blue LED |
| KW47-EVK / MCX-W72-EVK                   | SW2   | LED2     |
| FRDM-MCXW72                              | SW4   | Blue LED |

**Parent topic:**[ANCS/AMS client \(ancs\_c\)](../topics/ancs_client.md)

