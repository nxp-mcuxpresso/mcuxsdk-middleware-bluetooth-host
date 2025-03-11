# Image block request command

This command is sent by the OTAP Client to the OTAP Server to request a part of the upgrade image after it has determined the OTAP Server has an upgrade image available.

When an OTAP Server Receives this command it should stop any image file chunk transfer sequences in progress.

|CmdId|Name|Dir|Parameters|Param Size

 \(Bytes\)

|Description|Total Size \(CmdId+Payload\)|
|-----|----|---|----------|------------------------|-----------|----------------------------|
|0x04|Image Block Request|C-\>S|ImageId|2|Image Id|16|
|StartPosition|4|Start position of the image block to be transferred.|
|BlockSize|4|Requested total block size in bytes.|
|ChunkSize|2|Should be optimized to the TransferChannel type. The maximum number of chunks per block is 256. Value is in bytes.|
|TransferMethod|1|0x00 - ATT

 0x01 – L2CAP PSM Credit based channel

|
|L2capChannelOrPsm|2|0x0004 - ATT

 Other values – PSM for credit based channels

|

The *ImageId* parameter contains the ID of the upgrade image.

The *StartPosition* parameter specifies the location in the image upgrade file at which the requested block starts.

The *BlockSize* and *ChunkSize* parameters specify the size in bytes of the block to be transferred and the size of the chunks into which a block is separated. The *ChunkSize* value must be chosen in such a way that the total number of chunks can be represented by the *SeqNumber* parameter of the Image Chunk Command. At the moment this parameter is 1 byte in size so there are a maximum of 256 chunks per block. The chunk sequence number goes from 0 to 255 \(0x00 to 0xFF\). If this condition is not met or the requested block is not entirely into the image file bounds an error is sent to the OTAP Client when the OTAP Server receives this misconfigured Image Block Request Command.

The maximum value of the *ChunkSize* parameter depends on the maximum ATT\_MTU and L2CAP\_MTU supported by the Bluetooth LE stack version and implementation.

The *TransferMethod* parameter is used to select the transfer method which can be ATT or L2CAP PSM CoC. The *L2capChannelOrPsm* parameter must contain the value 0x0004 for the ATT transfer method and another value representing the chosen PSM for the L2CAP PSM transfer method. The default PSM for the Bluetooth LE OTAP demo applications is 0x004F for both the OTAP Server and the OTAP Client although the specification allows different values at the 2 ends of the L2CAP PSM connection. The PSM must be in the range reserved by the Bluetooth specification which is 0x0040 to 0x007F.

The optimal value of the *ChunkSize* parameter depends on the chosen transfer method and the Link Layer payload size. Ideally it must be chosen in such a way that full packets are sent for every chunk in the block.

The default Link Layer payload is 27 bytes form which we subtract 4 for the L2CAP layer and 3 for the ATT layer \(1 for the ATT Cmd Opcode and 2 for the Handle\) leaving us with a 20 byte OTAP protocol payload. From these 20 bytes we subtract 1 for the OTAP CmdId and 1 for the chunk sequence number leaving us with an optimum chunk size of 18 for the ATT transfer method – which is the default in the demo applications. For the L2CAP PSM transfer method the chosen default chunk size is 111. This was chosen so as a chunk fits exactly 5 link layer packets. The default L2CAP payload of 23 \(27 - 4\) multiplied by 5 gives us 115 from which we subtract 2 bytes for the SDU Length \(which is only sent in the first packet\), 1 byte for the OTAP CmdId and 1 byte for the chunk sequence number which leaves exactly 111 bytes for the actual payload.

If the Link layer supports Long Frames feature then the chunk size should be set according to the negotiated ATT MTU for the ATT transfer method. From the negotiated ATT MTU \(*att\_mtu*\) subtract 3 bytes for the ATT layer \(1 for the ATT Cmd Opcode and 2 for the Handle\) then subtract 2 bytes for the OTAP protocol \(1 for the CmdId and 1 for the chunk sequence number\) to determine the optimum chunk size \(*optimum\_att\_chunk\_size = att\_mtu – 3 – 2\)*. For the L2CAP PSM transfer method the chunk size can be set based on the maximum L2CAP SDU size \(*max\_l2cap\_sdu\_size*\) from which 4 bytes should be subtracted, 2 for the SDU Length and 2 for the OTAP protocol \(*optimum\_l2cap\_chunk\_size = max\_l2cap\_sdu\_size – 3 – 2*\). In some particular cases reducing the L2CAP chunk size could lead to better performance. If the L2CAP chunk size needs to be reduced it should be reduced so it fits exactly a number of link layer packets. An example of how to compute an optimal reduced L2CAP chunk size is given in the previous paragraph.

**Parent topic:**[Bluetooth Low Energy OTAP commands](../topics/bluetooth_low_energy_otap_commands.md)

