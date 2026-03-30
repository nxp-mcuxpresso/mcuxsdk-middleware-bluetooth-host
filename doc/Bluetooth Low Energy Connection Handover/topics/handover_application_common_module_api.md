# Connection Handover application common module APIs

### AppHandover_Init
```
bleResult_t AppHandover_Init(appHandoverEventCb_t pfAppEventCb, gapConnectionCallback_t pfConnectionCallback, appHandoverA2AInterfaceCb_t pfAppA2AInterfaceCb);
```
**Description**: Initializes the handover application common module.

**Parameters**:
- `pfAppEventCb`: Application callback for handover events.
- `pfConnectionCallback`: Connection callback for connection events.
- `pfAppA2AInterfaceCb`: Communication callback for handover data.

**Returns**: `bleResult_t`

### AppHandover_TimeSyncTransmit
```
bleResult_t AppHandover_TimeSyncTransmit(bleHandoverTimeSyncEnable_t enable);
```
**Description**: Enables/disables transmission of time synchronization information.

**Parameters**:
- `enable`: Enables or disables the transmission of time synchronization parameters.

**Returns**: `bleResult_t`

### AppHandover_TimeSyncReceive
```
bleResult_t AppHandover_TimeSyncReceive(bleHandoverTimeSyncEnable_t enable);
```
**Description**: Enables/disables reception of time synchronization information.

**Parameters**:
- `enable`: Enables or disables the reception of time synchronization parameters.

**Returns**: `bleResult_t`

### AppHandover_StartTimeSync
```
void AppHandover_StartTimeSync(bool_t bTimeSyncForHandover);
```
**Description**: Triggers handover time synchronization.

**Parameters**:
- `bTimeSyncForHandover`: TRUE if handover is following, FALSE if RSSI sniffing is following.

**Returns**: None

### AppHandover_TimeSyncTransmitSetParams
```
void AppHandover_TimeSyncTransmitSetParams(gapHandoverTimeSyncTransmitParams_t *pParams);
```
**Description**: Sets parameters for the transmission of time synchronization information.

**Parameters**:
- `pParams`: Tx params.

**Returns**: None

### AppHandover_TimeSyncReceiveSetParams
```
void AppHandover_TimeSyncReceiveSetParams(gapHandoverTimeSyncReceiveParams_t *pParams);
```
**Description**: Sets parameters for the reception of time synchronization information.

**Parameters**:
- `pParams`: Rx params.

**Returns**: None

### AppHandover_SetPeerDevice
```
void AppHandover_SetPeerDevice(deviceId_t deviceId);
```
**Description**: Sets the peer device for connection handover.

**Parameters**:
- `deviceId`: Peer device identifier.

**Returns**: None

### AppHandover_ProcessA2ACommand
```
void AppHandover_ProcessA2ACommand(uint8_t cmdId, uint32_t cmdLen, uint8_t *pCmdData);
```
**Description**: Processes handover commands.

**Parameters**:
- `cmdId`: Command identifier.
- `cmdLen`: Command length.
- `pCmdData`: Command data.

**Returns**: None

### AppHandover_SetPeerSkd
```
bleResult_t AppHandover_SetPeerSkd(uint8_t *pSkd, uint8_t nvmIndex);
```
**Description**: Sets the peer LL SKD for connection handover with advanced secure mode.

**Parameters**:
- `nvmIndex`: Peer device NVM index.
- `pSkd`: