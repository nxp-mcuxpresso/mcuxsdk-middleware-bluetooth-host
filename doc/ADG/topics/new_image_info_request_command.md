# New image info request command

This command can be sent by an OTAP Client to an OTAP Server to inquire about available upgrade images on the OTAP Server.

| CmdId | Name                   | Dir  | Parameters   | Param Size (Bytes) | Description                                                                                                                                                                                                             | Total Size (CmdId<br>+Payload) |
| ----- | ---------------------- | ---- | ------------ | ------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------ |
| 0x02  | New Image Info Request | C->S | CurrImageId  | 2                  | Id of the currently running image. Should be 0x0000.                                                                                                                                                                    | 11                             |
| 0x02  | New Image Info Request | C->S | CurrImageVer | 8                  | Version of the currently running image. A value of all zeroes signals that the client is looking for all images available on an OTAP Server. A value of all zeroes requests information about all images on the server. | 11                             |

- The *CurrImageId* parameter should be set to 0x0000 to signify the current running image.

- The *CurrImageVer* parameter should contain sufficient information about the target device for the OTAP Server to determine if it has an upgrade image available for the requesting OTAP Client.

- A value of all zeroes for the *CurrImageVer* means that an OTAP Client is requesting information about all available images on an OTAP Server and the OTAP Server should send a New Image Info Response for each image.

**Parent topic:**[Bluetooth Low Energy OTAP commands](../topics/bluetooth_low_energy_otap_commands.md)

