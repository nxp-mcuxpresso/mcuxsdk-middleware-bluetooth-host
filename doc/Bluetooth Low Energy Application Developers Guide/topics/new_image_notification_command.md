# New image notification command

This command can be sent by an OTAP Server to an OTAP Client, usually immediately after the first connection, to notify the OTAP Client of the available images on the OTAP Server.

### Table 14: New Image Notification Command Parameters
| CmdId | Name                   | Dir  | Parameters     | Param Size (Bytes) | Description                                                                                  | Total Size (CmdId + Payload) |
|-------|------------------------|------|----------------|--------------------|----------------------------------------------------------------------------------------------|-----------------------------|
| 0x01  | New Image Notification | S->C | ImageId        | 2                  | Short image identifier used for transactions between the OTAP Server and OTAP Client. Should be unique for all images on a server. | 15                          |
|       |                        |      | ImageVersion   | 8                  | Image file version. Contains sufficient information to identify the target hardware, stack version and build version. |                             |
|       |                        |      | ImageFileSize  | 4                  | Image file size in bytes.                                                                    |                             |

The *ImageId* parameter should not be '0x0000', which is the reserved value for the current running image or 0xFFFF, which is the reserved value for “no image available”.

**Parent topic:**[Bluetooth Low Energy OTAP commands](../topics/bluetooth_low_energy_otap_commands.md)

