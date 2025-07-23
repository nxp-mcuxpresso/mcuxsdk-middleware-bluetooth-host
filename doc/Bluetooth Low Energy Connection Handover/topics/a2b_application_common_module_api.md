# A2B application common module APIs

### A2B_Init
```
bleResult_t A2B_Init(appA2BEventCb_t pfAppEventCb, appA2BA2ACommInterfaceCb_t pfAppA2AInterfaceCb);
```
**Description**: Trigger the A2B feature initialization by generating the public key used for the Edgelock to Edgelock key derivation.

**Parameters**:
- `pfAppEventCb`: Application callback functions for A2B events.
- `pfAppA2AInterfaceCb`: Communication callback for A2B data.

**Returns**: `bleResult_t` - Result of the operation.

### A2B_ProcessA2ACommand
```
void A2B_ProcessA2ACommand(uint8_t cmdId, uint32_t cmdLen, uint8_t *pCmdData);
```
**Description**: Process A2B commands received through the A2A interface.

**Parameters**:
- `cmdId`: Command identifier.
- `cmdLen`: Command length.
- `pCmdData`: Command data.

**Returns**: None

### A2B_FreeE2EKey
```
bleResult_t A2B_FreeE2EKey(void);
```
**Description**: Free the EdgeLock to EdgeLock key.

**Parameters**: None

**Returns**: `bleResult_t` - Result of the operation.
