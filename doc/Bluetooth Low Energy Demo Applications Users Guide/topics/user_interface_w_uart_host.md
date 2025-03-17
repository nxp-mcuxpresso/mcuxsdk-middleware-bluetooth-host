# User interface

Ensure that both NCP FSCI Blackbox (for the NBU core) and Wireless UART Host (for the Application core) have been flashed to the board. 

After flashing the board, the device is in idle mode \(all LEDs flashing\). To start scanning, press the **SCANSW** button. When in GAP Limited Discovery Procedure of GAP General Discoverable Mode, **CONNLED** is flashing. When the node connects to a peer device, **CONNLED** turns solid. To disconnect the node, hold the **SCANSW** button pressed for 2-3 seconds. The node then re-enters GAP Limited Discovery Procedure.

See the table below for hardware references for the Wireless UART application.

| Platform                   | SCANSW | CONNLED  | ROLESW |
| -------------------------- | ------ | -------- | ------ |
| MCXW72-EVK                 | SW2    | LED2     | SW3    |
| FRDM-MCXW72                | SW4    | Blue LED | SW2    |

**Parent topic:**[Wireless UART Host](../topics/wireless_uart_host.md)

