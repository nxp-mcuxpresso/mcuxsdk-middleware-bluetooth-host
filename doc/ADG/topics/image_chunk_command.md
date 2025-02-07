# Image chunk command

One or more Image Chunk Commands are sent from the OTAP Server to the OTAP Client after an Image Block Request is received by the former. The image chunks are sent via the ATT Write Without Response mechanism if the ATT transfer method is chosen and directly via L2CAP if the L2CAP PSM CoC transfer method is chosen.

|CmdId|Name|Dir|Parameters|Param Size

 \(Bytes\)

|Description|Total Size \(CmdId+Payload\)|
|-----|----|---|----------|------------------------|-----------|----------------------------|
|0x05|Image Chunk|S-\>C|SeqNumber|1|In the range 0 -\> BlockSize/ChunkSize - calculated by Server, checked by Client.

 The command code is present even when ATT is used.

|3 or more

|
|Data|variable|Actual data|

The *SeqNumber* parameter is the chunk sequence number and it has incremental values from 0 to 255 \(0x00 to 0x FF\) for a maximum of 256 chunks per block.

The *Data* parameter is an array containing the actual image part being transferred starting from the *BlockStartPosition + SeqNumber \***ChunkSize*position in the image file and containing *ChunkSize* or less bytes depending on the position in the block. Only the last chunk in a block can have less than *ChunkSize*bytes in the Image Chunk Command data payload.

**Parent topic:**[Bluetooth Low Energy OTAP commands](../topics/bluetooth_low_energy_otap_commands.md)

