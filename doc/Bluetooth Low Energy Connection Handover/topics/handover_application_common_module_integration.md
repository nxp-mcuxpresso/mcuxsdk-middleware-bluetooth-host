# Integration

All the APIs referenced in this section are available in the Connection Handover application common module.

Connection handover requires a communication interface with the other devices involved in the connection handover process.

Perform the following steps to integrate Connection Handover in an application:

1. Add `app_handover.c` and `app_handover.h` from `middleware\wireless\bluetooth\application\common\auto\` in the application project.
2. Initialize the Connection Handover application common module by calling `AppHandover_Init()`.
3. Call `AppHandover_ProcessA2ACommand()` to handle connection handover commands received.
4. Call `AppHandover_GenericCallback()` in the application generic events callback to handle the following events:
    - `gHandoverAnchorSearchStarted_c`
    - `gHandoverAnchorSearchStopped_c`
    - `gHandoverAnchorMonitorEvent_c`
    - `gHandoverSuspendTransmitComplete_c`
    - `gHandoverGetComplete_c`
    - `gGetConnParamsComplete_c`
    - `gHandoverTimeSyncEvent_c`
    - `gHandoverTimeSyncTransmitStateChanged_c`
    - `gHandoverTimeSyncReceiveComplete_c`
    - `gLlSkdReportEvent_c`
    - `gInternalError_c`
    - `gHandoverAnchorMonitorPacketEvent_c`
    - `gHandoverAnchorMonitorPacketContinueEvent_c`
    - `gHandoverFreeComplete_c`
    - `gHandoverLlPendingData_c`
    - `gHandoverConnectionUpdateProcedureEvent_c`
    - `gHandoverApplyConnectionUpdateProcedureComplete_c`
5. Call `AppHandover_ConnectionCallback` in the application connection events callback to handle the following events:
    - `gConnEvtHandoverConnected_c`
    - `gHandoverDisconnected_c`
6. To trigger connection handover:
   - **Standard flow** (Time Synchronization required): call `AppHandover_SetPeerDevice()` followed by `AppHandover_StartTimeSync(TRUE)`.
   - **Fast flow** (Anchor/Packet Monitoring already active on Target): call `AppHandover_StartConnectionHandover(deviceId)`. This skips Time Synchronization and Anchor Search entirely.
