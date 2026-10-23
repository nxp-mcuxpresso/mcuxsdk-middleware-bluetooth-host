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

### AppHandover_StartConnectionHandover
```
bleResult_t AppHandover_StartConnectionHandover(deviceId_t deviceId);
```
**Description**: Triggers connection handover without requiring time synchronization or anchor search. Anchor monitoring must already be in progress for the specified connection on the Target device.

**Parameters**:
- `deviceId`: Peer device identifier of the connection to be handed over.

**Returns**: `bleResult_t`
- `gBleSuccess_c`: Handover initiated successfully.
- `gBleInvalidParameter_c`: Invalid device identifier.
- `gBleInvalidState_c`: Handover already in progress.
- `gBleUnavailable_c`: Anchor monitoring not active for the specified connection.

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
- `pSkd`: LL Session Key Diversifier.

**Returns**: `bleResult_t`

### AppHandover_SetMonitorMode
```
bleResult_t AppHandover_SetMonitorMode(deviceId_t deviceId, bleHandoverAnchorSearchMode_t mode, bool_t silent);
```
**Description**: Sets the Anchor Monitor mode for the given connection. The `silent` parameter was added to support Channel Sounding monitoring, where the monitor must stay active at the Link Layer level while suppressing the application monitoring reports normally forwarded over the anchor-to-anchor (A2A) interface.

**Parameters**:
- `deviceId`: Connection to be monitored.
- `mode`: Anchor monitor mode.
- `silent`: TRUE to keep monitoring active at Link Layer level but suppress application monitoring reports over A2A; FALSE to report normally.

**Returns**: `bleResult_t`

### AppHandover_AnchorMonitorReset
```
void AppHandover_AnchorMonitorReset(deviceId_t deviceId);
```
**Description**: Stops remote anchor monitoring and frees the local monitor slot in a single call. Use on a live-connection disconnect so that a subsequent `AppHandover_SetMonitorMode()` for the same device cannot fail with `gBleInvalidState_c`.

**Parameters**:
- `deviceId`: Peer device identifier.

**Returns**: None

### AppHandover_GetMonitoredConnHandle
```
uint16_t AppHandover_GetMonitoredConnHandle(deviceId_t deviceId);
```
**Description**: Returns the monitored connection handle associated with a device identifier.

**Parameters**:
- `deviceId`: Connection identifier.

**Returns**: `uint16_t`
- Monitored connection handle, or `gInvalidConnectionHandle_c` if the device is not currently monitored.