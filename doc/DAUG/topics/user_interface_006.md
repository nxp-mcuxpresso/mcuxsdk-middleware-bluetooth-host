# User interface

After flashing the board, the peripheral is in idle mode \(all LEDs flashing\). To start advertising, press the ADVSW button. When in GAP Discoverable Mode, **CONNLED** is flashing. When a central node connects to the peripheral, **CONNLED** turns solid. The node then waits for an EATT connection request and for the client to configure indications for the two services. The service information is sent over enhanced bearers only. The values indicated are cycled between 0 and 10. To disconnect the node, hold the ADVSW button for 2-3 seconds. The node then re-enters GAP Discoverable Mode and starts advertising.

[Table 1](#TABLE_EB97D10D-29E0-4C92-83F0-AE817B479449) details below the hardware references.

|**Platform**

|**ADVSW**

|**CONNLED**

|
|--------------|-----------|-------------|
|KW45B41Z-EVK / K32W148-EVK

|SW2|LED2

|
|FRDM-MCXW71|SW2|Blue LED|
|KW47-EVK / MCXW72-EVK|SW2|LED2|
|FRDM-MCXW72|SW4|Blue LED|

**Parent topic:**[EATT Peripheral](../topics/eatt_peripheral.md)

