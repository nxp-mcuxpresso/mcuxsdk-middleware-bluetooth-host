# A2A application common module APIs

### A2A_Init
```
bleResult_t A2A_Init(serial_handle_t pSerialHandle, appA2ADataIndicationCb_t pfDataIndCb);
```
**Description**: Performs initialization of the Anchor to Anchor serial communication.

**Parameters**:
- `pSerialHandle`: Serial interface handle to be used for communication.
- `pfConnectionCallback`: Application callback for received packets.

**Returns**: `bleResult_t`

### A2A_SendCommand
```
void A2A_SendCommand(uint8_t opGroup, uint8_t opCode, uint8_t *pPayload, uint16_t len);
```
**Description**: Send packet over the Anchor to Anchor interface.

**Parameters**:
- `opGroup`: Group identifier.
- `opCode`: Command identifier.
- `pPayload`: Pointer to command data.
- `len`: Command length.

**Returns**: None
