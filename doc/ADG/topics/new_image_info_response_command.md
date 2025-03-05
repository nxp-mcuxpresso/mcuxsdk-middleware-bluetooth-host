# New image info response command

This command is sent by the OTAP Server to the OTAP Client as a response to a New Image Information Request Command.

| CmdId | Name                    | Dir  | Parameters         | Param Size (Bytes) | Description                                                | Total Size (CmdId<br>+Payload) |
| ----- | ----------------------- | ---- | ------------------ | ------------------ | ---------------------------------------------------------- | ------------------------------ |
| 0x03  | New Image Info Response | S->C | ImageId            | 2                  | Image Id. Value 0xFFFF is reserved as “no image available” | 15                             |
| 0x03  | New Image Info Response | S->C | ImageVersion       | 8                  | Image file version                                         | 15                             |
| 0x03  | New Image Info Response | S->C | 4 Image file size. | 4                  | Image file size                                            | 15                             |

The *ImageId* parameter with a value of 0xFFFF is reserved for the situation where no upgrade image is available for the requesting device.

**Parent topic:**[Bluetooth Low Energy OTAP commands](../topics/bluetooth_low_energy_otap_commands.md)

