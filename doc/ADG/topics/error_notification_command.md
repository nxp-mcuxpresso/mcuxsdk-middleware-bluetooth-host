# Error notification command

This command can be sent by both the OTAP Server and the OTAP Client when an error of any kind occurs. When an OTAP Server Receives this command it should stop any image file chunk transfer sequences in progress.

|**CmdId**|**Name**|**Dir**|**Parameters**|**Param Size \(Bytes\)**|**Description**|**Total Size \(CmdId+Payload\)**|
|---------|--------|-------|--------------|------------------------|---------------|--------------------------------|
|0x07|Error Notification|Bidir|CmdId|1|Id of the command which generated the error.|3|
|ErrorStatus|1|Error Status: Examples: out of image bounds, chunk too small, chunk too large, image verification failure, bad command format, image not available, unknown command

|

The *CmdId* parameter contains the ID of the command which caused the error \(if applicable\).

The *ErrorStatus* parameter contains the source of the error. All error statuses are defined in the *otapStatus\_t* enumerated type in the *otap\_interface.h* file.

**Parent topic:**[Bluetooth Low Energy OTAP commands](../topics/bluetooth_low_energy_otap_commands.md)

