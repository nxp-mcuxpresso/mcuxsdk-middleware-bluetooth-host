# Connection Handover API availability

### Gap_HandoverGetDataSize
```
bleResult_t Gap_HandoverGetDataSize(deviceId_t deviceId, uint32_t *pDataSize);
```
**Description**: Returns the size in octets of the Handover Data for the given peer device id.

**Parameters**:
- `deviceId`: Peer device identifier.
- `pDataSize`: Pointer to the size in octets of the Handover Data.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverGetData
```
bleResult_t Gap_HandoverGetData(deviceId_t deviceId, uint8_t *pData);
```
**Description**: Copies the data used by the Handover feature for the given peer device id.

**Parameters**:
- `deviceId`: Peer device identifier.
- `pData`: Pointer to memory location where the Handover data is to be copied.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverSetData
```
bleResult_t Gap_HandoverSetData(uint8_t *pData);
```
**Description**: Sets the data used by the Handover feature.

**Parameters**:
- `pData`: Pointer to memory location where the Handover data is found.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverSetLlPendingData
```
bleResult_t Gap_HandoverSetLlPendingData(uint16_t connectionHandle, uint8_t *pTxData);
```
**Description**: Sets the LL data pending on the target device to be transmitted after handover connect.

**Parameters**:
- `connectionHandle`: Handover connection identifier.
- `pTxData`: Pointer to memory location where the pending data is found.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverFreeData
```
bleResult_t Gap_HandoverFreeData(void);
```
**Description**: Frees reference to handover data provided through `Gap_HandoverSetData()`.

**Returns**: `gBleSuccess_c` or `gBleInvalidState_c` if handover data reference is not set.

### Gap_HandoverGetTime
```
bleResult_t Gap_HandoverGetTime(void);
```
**Description**: Gets timing information from Link Layer to be used in Handover process.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverSuspendTransmit
```
bleResult_t Gap_HandoverSuspendTransmit(deviceId_t deviceId, bleHandoverSuspendTransmitMode_t mode, uint16_t eventCounter, uint8_t noOfConnIntervals);
```
**Description**: Suspends TX for the given connection.

**Parameters**:
- `deviceId`: Peer device identifier.
- `mode`: Mode.
- `eventCounter`: Event counter.
- `noOfConnIntervals`: Number of connection intervals during which TX is suspended. Use 0 for manual resume.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverResumeTransmit
```
bleResult_t Gap_HandoverResumeTransmit(deviceId_t deviceId);
```
**Description**: Resumes TX for the given connection.

**Parameters**:
- `deviceId`: Peer device identifier.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverAnchorNotification
```
bleResult_t Gap_HandoverAnchorNotification(deviceId_t deviceId, bleHandoverAnchorNotificationEnable_t enable, uint8_t noOfReports);
```
**Description**: Enables or disables anchor notifications for the given peer.

**Parameters**:
- `deviceId`: Peer device identifier.
- `enable`: Enables or disables notifications.
- `noOfReports`: Number of reports to receive (0 for manual stop).

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverAnchorSearchStart
```
bleResult_t Gap_HandoverAnchorSearchStart(gapHandoverAnchorSearchStartParams_t *pSearchParams);
```
**Description**: Starts anchor search with the given parameters.

**Parameters**:
- `pSearchParams`: Pointer to structure containing anchor search parameters. The structure contains:
  - `startTime625`: Slot of the anchor point timing of the connection event.
  - `startTimeOffset`: Slot offset of the anchor point timing of the connection event.
  - `lastRxInstant`: Last successful access address reception instant (unit 625 us).
  - `accessAddress`: Access address (4 bytes).
  - `crcInit`: CRC initialization value (3 bytes).
  - `channelMap`: Channel map (5 bytes, FF FF FF FF 1F if all channels are used).
  - `connInterval`: Connection interval (unit 1.25ms).
  - `latency`: Latency (unit connection interval).
  - `supervisionTimeout`: Supervision timeout (unit 10ms).
  - `eventCounter`: Current connection event counter.
  - `centralSca`: Sleep clock accuracy (0 to 7).
  - `role`: Role (0 for central and 1 for peripheral).
  - `centralPhy`: TX/RX PHY.
  - `seqNum`: Sequence number; bits 1-0: latest received SN and NESN; bits 5-4: latest transmitted SN and NESN.
  - `hopAlgo2`: Hop algorithm (0/1 for hop algorithm #1 or #2).
  - `unmappedChannelIndex`: Unmapped channel index (used only for hop algorithm #1).
  - `hopIncrement`: Hop increment.
  - `ucNbReports`: Number of connection intervals to monitor (0: continuous, 1-255: automatic stop).
  - `uiEventCounterAdvance`: Event counter delay to perform the search (0 to start ASAP).
  - `timeout`: Timeout in connection intervals (0 if no timeout).
  - `timingDiffSlot`: Slot difference of the LL timing.
  - `timingDiffOffset`: Slot offset difference of the LL timing.
  - `mode`: Search mode setting.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverAnchorSearchStop
```
bleResult_t Gap_HandoverAnchorSearchStop(uint16_t connHandle);
```
**Description**: Stops anchor search for the given connection handle.

**Parameters**:
- `connHandle`: Connection handle for which to stop the anchor search.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverTimeSyncTransmit
```
bleResult_t Gap_HandoverTimeSyncTransmit(gapHandoverTimeSyncTransmitParams_t *pTransmitParams);
```
**Description**: Starts time sync advertising with given parameters.

**Parameters**:
- `pTransmitParams`: Pointer to structure containing time sync transmit parameters. The structure contains:
  - `enable`: Enables or disables the transmit operation (0 = disable, 1 = enable).
  - `advChannel`: BLE channel to use for advertising (0 to 39).
  - `deviceAddress`: Packet identifier, can be the BD address or any value (6 bytes).
  - `phys`: PHY setting (0 = 1M, 1 = 2M, 2 = LR S8, 3 = LR S2).
  - `txPowerLevel`: TX Power level (0 to 31).
  - `txIntervalSlots625`: Packet interval in slots (4 to 255).

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverTimeSyncReceive
```
bleResult_t Gap_HandoverTimeSyncReceive(gapHandoverTimeSyncReceiveParams_t *pReceiveParams);
```
**Description**: Starts time sync scanning with the given parameters.

**Parameters**:
- `pReceiveParams`: Pointer to structure containing time sync receive parameters. The structure contains:
  - `enable`: Enables or disables the receive operation (0 = disable, 1 = enable).
  - `scanChannel`: BLE channel to use for scanning (0 to 39).
  - `deviceAddress`: Packet identifier, can be the BD address or any value (6 bytes).
  - `phys`: PHY setting (0 = 1M, 1 = 2M, 2 = LR S8, 3 = LR S2).
  - `stopWhenFound`: One-shot setting (0 = continue scanning, 1 = stop when packet is found).

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverGetConnParams
```
bleResult_t Gap_HandoverGetConnParams(uint16_t connHandle, gapConnectionCallback_t connectionCallback, uint8_t anchorNotification);
```
**Description**: Connects to a peer device initialized through the Connection Handover procedure.

**Parameters**:
- `connHandle`: Handover Connection handle.
- `connectionCallback`: Pointer to the connection event handler function.
- `anchorNotification`: Anchor search setting after handover, 0 - stop anchor notification, 1 - keep anchor notification.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverDisconnect
```
bleResult_t Gap_HandoverDisconnect(deviceId_t deviceId);
```
**Description**: Disconnects a peer device for which the connection was handed over to another device.

**Parameters**:
- `deviceId`: Peer device identifier.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverInit
```
bleResult_t Gap_HandoverInit(void);
```
**Description**: Initializes the connection handover feature. Must be called before `Ble_Initialize()`.

**Returns**: `gBleInvalidState_c` if `Ble_Initialize()` was already called, `gBleSuccess_c` otherwise.

### Gap_HandoverSetSkd
```
bleResult_t Gap_HandoverSetSkd(uint8_t nvmIndex, uint8_t *pSkd);
```
**Description**: Sets the SKD of the connection that is to be handed over. Used only if `gAppSecureMode_d` is enabled in the application. Requires Handover Data to be set.

**Parameters**:
- `nvmIndex`: Bonding Data NVM index.
- `pSkd`: Pointer to 16 octets SKD.

**Returns**: `gBleInvalidState_c` if `Ble_Initialize()` was already called, `gBleSuccess_c` otherwise.

### Gap_HandoverGetCsLlContext
```
bleResult_t Gap_HandoverGetCsLlContext(deviceId_t deviceId);
```
**Description**: Copies the LL CS-related context data for the given peer device id.

**Parameters**:
- `deviceId`: Peer device identifier.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverSetCsLlContext
```
bleResult_t Gap_HandoverSetCsLlContext(deviceId_t deviceId, uint16_t mask, uint8_t contextLength, uint8_t *pContextData);
```
**Description**: Sets the CS-related context data into the LL, for the given peer device id.

**Parameters**:
- `deviceId`: Peer device identifier.
- `mask`: Context data bitmask (used by the LL).
- `contextLength`: Length of the context data.
- `pContextData`: Pointer to context data.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverUpdateConnParams
```
bleResult_t Gap_HandoverUpdateConnParams(gapHandoverUpdateConnParams_t *pConnParams);
```
**Description**: Updates the channel map and/or PHY used in the current anchor monitoring process.

**Parameters**:
- `pConnParams`: Pointer to connection parameters to be updated.

**Returns**: `gBleSuccess_c` or error.

### Gap_HandoverApplyConnectionUpdateProcedure
```
bleResult_t Gap_HandoverApplyConnectionUpdateProcedure(gapHandoverApplyConnectionUpdateProcedure_t *pConnParams);
```
**Description**: Applies the new connection parameters in case of Connection Update Procedure on the active controller.

**Parameters**:
- `pConnParams`: Pointer to connection parameters to be applied.

**Returns**: `gBleSuccess_c` or error.

# Connection Handover Events

The following events can be received through the GAP generic callback during the handover operations:

### gHandoverGetComplete_c (0x31)
**Description**: Event generated when handover data get operation is complete.

**Event Data**: `handoverGetData_t` containing:
- `status`: Command status.
- `pData`: Pointer to connection handover data.

### gHandoverSetComplete_c (0x32)
**Description**: Event generated when handover data set operation is complete.

**Event Data**: `handoverSetData_t` containing:
- `status`: Command status.
- `pData`: Pointer to connection handover data.

### gHandoverGetCsLlContextComplete_c (0x33)
**Description**: Event generated when handover CS LL context data get operation is complete.

**Event Data**: `handoverGetCsLlContext_t` containing:
- `status`: Command status.
- `responseMask`: LL context bitmap indicating the Channel Sounding context.
- `llContextLength`: Context data length.
- `llContext`: LL Context data.

### gHandoverSetCsLlContextComplete_c (0x34)
**Description**: Event generated when handover CS LL context data set operation is complete.

**Event Data**: Status of the operation.

### gHandoverGetTime_c (0x35)
**Description**: Event generated when Handover Get Time command is complete.

**Event Data**: `handoverGetTime_t` containing:
- `status`: Command status.
- `slot`: LL timing slot counter (unit 625us).
- `us_offset`: LL timing slot offset (0 to 624, unit us).

### gHandoverSuspendTransmitComplete_c (0x36)
**Description**: Event generated when Handover Suspend Transmit command is complete.

**Event Data**: `handoverSuspendTransmitCompleteEvent_t` containing:
- `connectionHandle`: Connection identifier.
- `noOfPendingAclPackets`: Number of pending ACL packets.
- `sizeOfPendingAclPackets`: Pending ACL packet data size.
- `sizeOfDataTxInOldestPacket`: Size of data transmitted and acked in the oldest ACL packet.
- `sizeOfDataNAckInOldestPacket`: Size of data transmitted but not acked in the oldest ACL packet.

### gHandoverResumeTransmitComplete_c (0x37)
**Description**: Event generated when Handover Resume Transmit command is complete.

**Event Data**: `handoverResumeTransmitCompleteEvent_t` containing:
- `connectionHandle`: Connection identifier.

### gHandoverAnchorNotificationStateChanged_c (0x38)
**Description**: Event generated when Handover Anchor Notification command is complete.

**Event Data**: `handoverAnchorNotificationStateChanged_t` containing:
- `connectionHandle`: Connection identifier.

### gHandoverAnchorSearchStarted_c (0x39)
**Description**: Event generated when Handover Anchor Search Start command is complete.

**Event Data**: `handoverAnchorSearchStart_t` containing:
- `status`: Command status.
- `connectionHandle`: Connection identifier.

### gHandoverAnchorSearchStopped_c (0x3A)
**Description**: Event generated when Handover Anchor Search Stop command is complete.

**Event Data**: `handoverAnchorSearchStop_t` containing:
- `status`: Command status.
- `connectionHandle`: Connection identifier.

### gHandoverTimeSyncTransmitStateChanged_c (0x3B)
**Description**: Event generated when Handover Time Sync Transmit command is complete.

**Event Data**: Status of the operation.

### gHandoverTimeSyncReceiveComplete_c (0x3C)
**Description**: Event generated when Handover Time Sync Receive command is complete.

**Event Data**: Status of the operation.

### gHandoverAnchorMonitorEvent_c (0x3D)
**Description**: Event received from Controller - Handover Anchor Monitor.

**Event Data**: `handoverAnchorMonitorEvent_t` containing:
- `connectionHandle`: Connection identifier.
- `connEvent`: Current connection event counter.
- `rssiRemote`: RSSI of the packet from the remote device (+127 if not available).
- `lqiRemote`: LQI (Link Quality Indicator) of the packet from the remote device.
- `statusRemote`: Status of the packet from the remote device.
- `rssiActive`: RSSI of the packet from the active device (+127 if not available).
- `lqiActive`: LQI (Link Quality Indicator) of the packet from the active device.
- `statusActive`: Status of the packet from the active device.
- `anchorClock625Us`: Slot of the anchor point timing of the connection event.
- `anchorDelay`: Slot offset of the anchor point timing of the connection event.
- `chIdx`: BLE channel index.
- `ucNbReports`: Number of remaining reports to receive.

### gHandoverTimeSyncEvent_c (0x3E)
**Description**: Event received from Controller - Handover Time Sync.

**Event Data**: `handoverTimeSyncEvent_t` containing:
- `txClkSlot`: Transmitter packet start time in slot (625 us).
- `txUs`: Transmitter packet start time offset inside the slot (0 to 624 us).
- `rxClkSlot`: Receiver packet start time in slot (625 us).
- `rxUs`: Receiver packet start time offset inside the slot (0 to 624 us).
- `rssi`: RSSI value.

### gHandoverConnParamUpdateEvent_c (0x3F)
**Description**: Event received from Controller - Handover Connection Parameters Update.

**Event Data**: `handoverConnParamUpdateEvent_t` containing:
- `status`: Status indicating the event trigger source.
- `connectionHandle`: Connection identifier.
- `ulTxAccCode`: Access address.
- `aCrcInitVal`: CRC initialization value (3 bytes).
- `uiConnInterval`: Connection interval (unit 1.25ms).
- `uiSuperTO`: Supervision timeout (unit 10ms).
- `uiConnLatency`: Latency (unit connection interval).
- `aChMapBm`: Channel map (5 bytes).
- `ucChannelSelection`: Hop algorithm (0/1 for algorithm #1 or #2).
- `ucHop`: Hop increment.
- `ucUnMapChIdx`: Unmapped channel index.
- `ucCentralSca`: Sleep clock accuracy.
- `ucRole`: Role (0 for central and 1 for peripheral).
- `aucRemoteMasRxPHY`: TX/RX PHY.
- `seqNum`: Sequence number.
- `uiConnEvent`: Current connection event counter.
- `ulAnchorClk`: Slot of the anchor point timing.
- `uiAnchorDelay`: Slot offset of the anchor point timing.
- `ulRxInstant`: Last successful access address reception instant.

### gHandoverFreeComplete_c (0x46)
**Description**: Event generated when handover data free operation is complete.

**Event Data**: Status of the operation.

### gHandoverUpdateConnParamsComplete_c (0x47)
**Description**: Event generated when Handover Update Connection Parameters command is complete.

**Event Data**: `handoverUpdateConnParams_t` containing:
- `status`: Command status.
- `connectionHandle`: Connection identifier.

### gHandoverLlPendingData_c (0x4A)
**Description**: Event indicating ACL Data pending to be transmitted by the LL.

**Event Data**: `handoverLlPendingDataIndication_t` containing:
- `dataSize`: Pending HCI ACL data packet size.
- `pData`: Pointer to message containing the HCI ACL data packet (should be freed by the application).

### gHandoverAnchorMonitorPacketEvent_c (0x44)
**Description**: Event received from Controller - Handover Anchor Monitor Packet.

**Event Data**: `handoverAnchorMonitorPacketEvent_t` containing:
- `packetCounter`: Packet counter, incremented for each event.
- `connectionHandle`: Connection identifier.
- `statusPacket`: Status of the packet.
- `phy`: PHY (0/1/2/3 for 1M/2M/LR S8/LR S2).
- `chIdx`: BLE channel index.
- `rssiPacket`: RSSI of the packet (+127 if not available).
- `lqiPacket`: LQI (Link Quality Indicator) of the packet.
- `connEvent`: Current connection event counter.
- `anchorClock625Us`: Slot value of packet start time (in 625us unit).
- `anchorDelay`: Slot offset value of packet start time (in 1us unit).
- `ucNbConnIntervals`: Number of remaining connection intervals to monitor.
- `pduSize`: PDU length.
- `pPdu`: Pointer to PDU data (must be freed by the application).

### gHandoverAnchorMonitorPacketContinueEvent_c (0x45)
**Description**: Event received from Controller - Handover Anchor Monitor Packet Continue.

**Event Data**: `handoverAnchorMonitorPacketContinueEvent_t` containing:
- `packetCounter`: Packet counter, incremented for each event.
- `connectionHandle`: Connection identifier.
- `pduSize`: PDU length.
- `pPdu`: Pointer to PDU data (must be freed by the application).

### gHandoverConnectionUpdateProcedureEvent_c (0x4F)
**Description**: Event reporting new connection parameters indicated during the Connection Update procedure.

**Event Data**: `handoverConnectionUpdateProcedureEvent_t` containing:
- `connectionHandle`: Connection identifier.
- `winSize`: Transmit window size value (in 1.25 ms units).
- `winOffset`: Transmit window offset value (in 1.25 ms units).
- `interval`: Connection interval value (in 1.25 ms units).
- `latency`: Connection peripheral latency value.
- `timeout`: Connection supervision timeout value (in 10 ms units).
- `instant`: Connection event counter value when new parameters will be applied.
- `currentEventCounter`: Current connection event counter.

### gHandoverApplyConnectionUpdateProcedureComplete_c (0x50)
**Description**: Event reporting completion of the Connection Update procedure application.

**Event Data**: `handoverApplyConnectionUpdateProcedure_t` containing:
- `connectionHandle`: Connection identifier.
