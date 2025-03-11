# Stop image transfer command

This command is sent from the OTAP Client to the OTAP Server whenever the former wants to stop the transfer of an image block which is currently in progress, or from OTAP Server to the OTAP Client when the image transfer is stopped from application \(Test Tool\).

|CmdId|Name|Dir|Parameters|Param Size

 \(Bytes\)

|Description|Total Size \(CmdId+Payload\)|
|-----|----|---|----------|------------------------|-----------|----------------------------|
|0x08|Stop Image Transfer|C-\>S|ImageId|2|Image Id|3|

The *ImageId* parameter contains the ID of the image being transferred.

**Parent topic:**[Bluetooth Low Energy OTAP commands](../topics/bluetooth_low_energy_otap_commands.md)

