# User interface

After flashing two boards with the OTAP Server and OTAP Client applications respectively, the devices are in Idle mode \(all LEDs flashing\). To start advertising, press the **ADVSW** button on the OTAP Client. To start scanning, press the **SCANSW** button on the OTAP Server. After the two devices connect and start exchanging commands. **CONNLED** becomes solid on the OTAP Server and on the OTAP Client.

Start the OTAP Server PC application after the embedded applications are flashed to the boards. The application creates an OTAP image file using the provided executable `.srec` or `.bin` file. It then connects to the embedded OTAP Server via the configured serial interface and waits for commands. The application shows details about the image file creation and allows the OTAP upgrade image file header to be configured. The log view of the application displays the interactions between the OTAP Client and the OTAP Server.

See [Table 1](#GUID-8EA23B19-A7DC-4A3A-B948-679ED79AA2E0) for the hardware references.

| Platform                                 | ADVSW | SCANSW | CONNLED  |
| ---------------------------------------- | ----- | ------ | -------- |
| KW45B41Z-EVK / K32W148-EVK / MCX-W71-EVK | SW2   | SW2    | LED2     |
| FRDM-MCXW71                              | SW2   | SW2    | Blue LED |
| KW47-EVK / MCX-W72-EVK                   | SW2   | SW2    | LED2     |
| FRDM-MCXW72                              | SW4   | SW4    | Blue LED |

**Parent topic:**[Over the Air Programming \(OTAP\)](../topics/over_the_air_programming_otap.md)

