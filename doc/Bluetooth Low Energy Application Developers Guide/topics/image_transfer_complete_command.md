# Image transfer complete command

This command is sent by the OTAP Client to the OTAP Server when an image file has been completely transferred and its integrity has been checked.

|CmdId|Name|Dir|Parameters|Param Size

 \(Bytes\)

|Description|Total Size \(CmdId+Payload\)|
|-----|----|---|----------|------------------------|-----------|----------------------------|
|0x06|Image Transfer Complete|C-\>S|ImageId|2|Image Id|4|
|Status|1|Status of the image transfer. 0x00 - Success|

The *ImageId* parameter contains the ID of the image file that was transferred.

The *Status* parameter is 0x00 \(Success\) if image integrity and possibly other checks have been successfully made after the image is transferred and another value if integrity or other kind of errors have occurred.

If the status is 0x00 the OTAP Client can trigger the Bootloader to start flashing the new image. The image flashing should take about 15 seconds for a 160 KB flash memory.

**Parent topic:**[Bluetooth Low Energy OTAP commands](../topics/bluetooth_low_energy_otap_commands.md)

