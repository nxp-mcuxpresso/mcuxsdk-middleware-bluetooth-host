# New image notification command

This command can be sent by an OTAP Server to an OTAP Client, usually immediately after the first connection, to notify the OTAP Client of the available images on the OTAP Server.
| CmdId | Name                | Dir   | Parameters    | Param Size (Bytes)  | Description                                                | Total Size (CmdId+Payload) |
| ----- | ----------------------- | ---- | ------------- | ------------------ | ---------------------------------------------------------- | -------------------------- |
| 0x03  | New Image Info Response |   S->C    | ImageId        | 2                  | Image Id. Value 0xFFFF is reserved as “no image available” | 15                         |
| 0x03  | New Image Info Response | S->C  | ImageVersion  | 8                  | Image file version.                                        | 15                         |
| 0x03  | New Image Info Response | S->C | ImageFileSize | 4                  | Image file size.                                           | 15                         |

The *ImageId* parameter should not be '0x0000', which is the reserved value for the current running image or 0xFFFF, which is the reserved value for “no image available”.

**Parent topic:**[Bluetooth Low Energy OTAP commands](../topics/bluetooth_low_energy_otap_commands.md)

