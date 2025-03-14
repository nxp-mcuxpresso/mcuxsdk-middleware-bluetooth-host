# User interface

After flashing the board, the device is in idle mode \(all LEDs flashing\). To start Bluetooth LE scanning, press the **SCANSW** button. When in GAP Limited Discovery Procedure of GAP General Discoverable Mode, **CONNLED** is flashing. When the node connects to a peer device, **CONNLED** turns solid. To disconnect the node, hold the **SCANSW** button pressed for 2-3 seconds. The node then re-enters GAP Limited Discovery Procedure.

To start Generic FSK receiving, double click the **SCANSW** button and the device starts receiving packets on the same channel as the Bluetooth LE channel 37, with the network address defined in `gGenFSK_NetworkAddress_c`. The receiver only prints the packets that have the predefined identifier at `gGenFSK_Identifier_c`. To stop Generic FSK reception, double click the **ROLESW** button.

On a different device, start Generic FSK transmit by long pressing the **ROLESW** button. To stop the periodic Generic FSK transmit, long press again the **ROLESW** button, if the transmit procedure is ongoing. The long press **ROLESW** acts as a toggle for the transmit.

See the table below for hardware references of the Hybrid (Dual-mode) Bluetooth Low Energy and Generic FSK application.

| Platform                    | SCANSW | CONNLED | ROLESW |
| --------------------------- | ------ | ------- | ------ |
| KW45B41Z-EVK / KW45B41Z-LOC | SW2    | LED2    | SW3    |
| KW47-EVK / KW47-LOC         | SW2    | LED2    | SW3    |



**Parent topic:**[Hybrid \(Dual-mode\) Bluetooth Low Energy and Generic FSK](../topics/hybrid_dual-mode_bluetooth_low_energy_and_generic_.md)

