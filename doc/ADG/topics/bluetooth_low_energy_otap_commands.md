# Bluetooth Low Energy OTAP commands

The Bluetooth LE OTAP Commands general format is shown below. A command consists of two parts, a Command ID, and a Command Payload as shown in the table below.

|Field Name|CmdId        |  CmdPayload|
|----------|------|----------|
|Size \(Bytes\)  |     1  |variable|

Commands are sent over the transport medium starting with the Command ID and continuing with the Command Payload.

All multibyte command parameters in the Command Payload are sent in a least significant octet first order \(little endian\).

A summary of the commands supported by the Bluetooth LE OTAP Protocol is shown in the table below. Each of the commands is then detailed in its own section.

|CmdId|Command Name|
|-----|------------|
|0x01|New Image Notification|
|0x02|New Image Info Request|
|0x03|New Image Info Response|
|0x04|Image Block Request|
|0x05|Image Chunk|
|0x06|Image Transfer Complete|
|0x07|Error Notification|
|0x08|Stop Image Transfer|


```{include} ../topics/new_image_notification_command.md
:heading-offset: 3
```

```{include} ../topics/new_image_info_request_command.md
:heading-offset: 3
```

```{include} ../topics/new_image_info_response_command.md
:heading-offset: 3
```

```{include} ../topics/image_block_request_command.md
:heading-offset: 3
```

```{include} ../topics/image_chunk_command.md
:heading-offset: 3
```

```{include} ../topics/image_transfer_complete_command.md
:heading-offset: 3
```

```{include} ../topics/error_notification_command.md
:heading-offset: 3
```

```{include} ../topics/stop_image_transfer_command.md
:heading-offset: 3
```

**Parent topic:**[Bluetooth LE OTAP protocol](../topics/bluetooth_le_otap_protocol.md)

