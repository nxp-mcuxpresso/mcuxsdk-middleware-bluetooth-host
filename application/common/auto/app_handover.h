/*! ********************************************************************************************************************
 * \addtogroup AUTO
 * @{
********************************************************************************************************************* */
/*! ********************************************************************************************************************
* Copyright 2022-2026 NXP
*
*
* \file app_handover.h
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************************************************* */

#ifndef APP_HANDOVER_H
#define APP_HANDOVER_H

/***********************************************************************************************************************
************************************************************************************************************************
* Include
************************************************************************************************************************
***********************************************************************************************************************/
#include "EmbeddedTypes.h"
#include "ble_general.h"
#include "gap_handover_interface.h"

/***********************************************************************************************************************
************************************************************************************************************************
* Public macros
************************************************************************************************************************
***********************************************************************************************************************/
/*! Default advertising channel used for the connection handover time synchronization procedure */
#define gHandoverDefaultAdvChan_c       37U
/*! Default Tx power level for the time synchronization transmit procedure */
#define gHandoverTimeSyncDefaultPowerLvl_c      31U
/*! Default Tx interval for the time synchronization transmit procedure */
#define gHandoverTimeSyncDefaultTxInterval_c    10U
/*! Default PHY for the time synchronization transmit procedure */
#define gHandoverTimeSyncDefaultPhy_c           LL_PHY_1M
/*! Default address used for the connection handover time synchronization procedure */
#define HANDOVER_HTS_ADDR    {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC}
/*! Default value for anchor search packet filtering. Send one application event
out of gHandoverMonitorPacketNumberFilter_c events */
#ifndef gHandoverMonitorPacketNumberFilter_c
#define gHandoverMonitorPacketNumberFilter_c        30U
#endif /* gHandoverMonitorPacketNumberFilter_c */

/*! Default value for the LE_Handover_Anchor_Monitor_Event report latency.
0: send all HCI events. */
#ifndef gHandoverAnchorMonitorReportLatency_c
#define gHandoverAnchorMonitorReportLatency_c       gHandoverAnchorMonitorReportLatencySendAll_c
#endif /* gHandoverAnchorMonitorReportLatency_c */

/*! Default value for the connection events RX operation latency.
0: monitor all the connection events for the radio RX. */
#ifndef gHandoverAnchorMonitorConnEvtRxLatency_c
#define gHandoverAnchorMonitorConnEvtRxLatency_c    0U
#endif /* gHandoverAnchorMonitorConnEvtRxLatency_c */

/*! Threshold for the invalid number of anchor monitor events before connection
handover is aborted. Anchor search for connection handover is performed while the
TX is suspended, because of this the threshold value should be chosen carefully
to avoid the peer device being disconnected in case anchor search fails
constantly. */
#ifndef gHandoverAnchorSearchThreshold_c
#define gHandoverAnchorSearchThreshold_c                0U
#endif /* gHandoverAnchorSearchThreshold_c */

/* Default value for the ucNbReports parameter used for Anchor Search */
#ifndef gHandoverAnchorSearchIntervals_c
#define gHandoverAnchorSearchIntervals_c                0U
#endif /* gHandoverAnchorSearchIntervals_c */

#define gHandoverCommandsOpGroup_c                      0xDD

/* Handover commands identifier */
#define gHandoverDataCommandOpCode_c                    0x00    /* S1 -> S2 - Send Handover Data (Host and LL context) */
#define gHandoverAnchorStartSearchCommandOpCode_c       0x01    /* S1 -> S2 - Tell S2 to start the anchor search process */
#define gHandoverInformConnectCommandOpCode_c           0x02    /* S2 -> S1 - Inform S1 that S2 is connected and S1 should disconnect */
#define gHandoverStartTimeSyncCommandOpCode_c           0x03    /* S1 -> S2 - Tell S2 to start receiving for time sync procedure (S1 will advertise) */
#define gHandoverStopTimeSyncCommandOpCode_c            0x04    /* S2 -> S1 - Tell S1 to stop advertising for time sync procedure */
#define gHandoverInformFailureCommandOpCode_c           0x05    /* S2 -> S1 - Inform S1 that S2's anchor search was unsuccessful and S1 should resume transmitting */
#define gHandoverAnchorMonitorCommandOpCode_c           0x06    /* Report anchor monitor information to remote anchor */
#define gHandoverStopAnchorMonitorCommandOpCode_c       0x07    /* Stop anchor monitoring on remote anchor */
#define gHandoverPacketMonitorCommandOpCode_c           0x08    /* Report anchor monitor information to remote anchor */
#define gHandoverPacketContinueMonitorCommandOpCode_c   0x09    /* Report anchor monitor information to remote anchor */
#define gHandoverSetSkdCommandOpCode_c                  0x0A    /* S1 -> S2 - Transfer SKD */
#define gHandoverCsLlContextCommandOpCode_c             0x0B    /* S1 -> S2 - Send CS LL context data */
#define gHandoverAnchMonStartedCommandOpCode_c          0x0D    /* S2 -> S1 - Inform S1 that Anchor/Packet monitoring has started for the included connection handle */
#define gHandoverAnchMonStoppedCommandOpCode_c          0x0E    /* S2 -> S1 - Inform S1 that Anchor/Packet monitoring has been stopped for the included connection handle */
#define gHandoverLlPendingDataCommandOpCode_c           0x0F    /* S1 -> S2 - Inform S2 of the pending LL data */
#define gHandoverLConnectionUpdateParamsCommandOpCode_c 0x10    /* S1 -> S2 - Inform S2 of new connection parameters */
#define gHandoverTimeSyncStartedCommandOpCode_c         0x11    /* S2 -> S1 - Tell S1 to start advertising for time sync procedure */
#define gHandoverCsMonitorChannelMapUpdateCommandOpCode_c 0x12  /* S1 -> S2 - Forward CS monitor channel map update to the other anchor */

/* Handover commands length */
/* Anchor start search command payload. Last byte carries the silent monitoring flag. */
#define gHandoverAnchorStartSearchCommandLen_c          44U
#define gHandoverSetSkdCommandLen_c                     17U
/* CS LL context command payload layout (CS sniffing enabled):
     [0..1] monitorConnHandle | [2..3] responseMask | [4] contextLength | [5..] llContext
   The 2-byte monitored connection handle identifies the sniffed connection the
   context applies to (gInvalidConnectionHandle_c means connection handover, not
   CS sniffing). When CS sniffing is disabled the handle prefix is omitted and the
   payload is: [0..1] responseMask | [2] contextLength | [3..] llContext. */
#define gHandoverCsLlContextCommandLen_c                261U /* temporary value */

#define gHandoverAnchorMonitorLen_c                     20U
#define gHandoverPacketMonitorMaxLen_c                  274U
#define gHandoverPacketContinueMonitorMaxLen_c          259U
#define gHandoverAnchMonStartedCommandLen_c             2U
#define gHandoverAnchMonStopCommandLen_c                2U
#define gHandoverAnchMonStoppedCommandLen_c             2U
#define gHandoverConnectionUpdateProcedureCommandLen_c  15U
/*! Length of the target connection handle (2) appended to the Handover Data
   command. It is gInvalidConnectionHandle_c on the regular (non-fast) handover
   path and the monitored connection handle on the fast handover path. */
#define gHandoverDataCmdConnHandleLen_c                  2U
/* CS monitor channel map update command payload layout:
     [0..1] monitorConnHandle | [2..11] channelMap (10 octets) | [12..13] instant */
#define gHandoverCsMonitorChannelMapUpdateCommandLen_c  (2U + gCsChannelMapSize_c + 2U)

/***********************************************************************************************************************
************************************************************************************************************************
* Public type definitions
************************************************************************************************************************
***********************************************************************************************************************/

typedef struct appHandoverAnchorMonitorEvent_tag
{
    deviceId_t                      deviceId;           /*!< Peer device identifier */
    handoverAnchorMonitorEvent_t    anchorMntEvt;       /*!< Anchor monitor event data */
    int8_t                          rssiActiveAverage;  /*!< Average Active RSSI for gHandoverMonitorPacketNumberFilter_c events */
    int8_t                          rssiRemoteAverage;  /*!< Average Remote RSSI for gHandoverMonitorPacketNumberFilter_c events */
} appHandoverAnchorMonitorEvent_t;

typedef struct appHandoverAnchorMonitorPacketEvent_tag
{
    deviceId_t                          deviceId;       /*!< Peer device identifier */
    handoverAnchorMonitorPacketEvent_t  pktMntEvt;      /*!< Packet monitor event data */
} appHandoverAnchorMonitorPacketEvent_t;

typedef struct appHandoverAnchorMonitorPacketContinueEvent_tag
{
    deviceId_t                                  deviceId;       /*!< Peer device identifier */
    handoverAnchorMonitorPacketContinueEvent_t  pktMntCntEvt;   /*!< Packet monitor continue event data */
} appHandoverAnchorMonitorPacketContinueEvent_t;

typedef enum appHandoverEvent_tag {
    mAppHandover_TimeSyncStarted_c,
    mAppHandover_ConnectComplete_c,
    /* Raised on S1 when S2 reports it is connected. S1 sends application data on this event. */
    mAppHandover_PeerConnectComplete_c,
    mAppHandover_Disconnected_c,
    mAppHandover_Error_c,
    mAppHandoverAnchorMonitor_c,
    mAppHandoverConnParamUpdate_c,
    mAppHandoverPacketMonitor_c,
    mAppHandoverPacketContinueMonitor_c,
    mAppHandoverAnchorMonitorStarted_c,
    mAppHandoverAnchorMonitorStopped_c,
} appHandoverEvent_t;

typedef enum appHandoverError_tag {
    mAppHandover_NoActiveConnection_c,
    mAppHandover_TimeSyncTx_c,
    mAppHandover_TimeSyncRx_c,
    mAppHandover_AnchorSearchStartFailed_c,
    mAppHandover_PeerBondingDataInvalid_c,
    mAppHandover_UnexpectedError_c,
    mAppHandover_AnchorSearchFailedToSync_c,
    mAppHandover_OutOfMemory_c,
    mAppHandover_ConnParamsUpdateFail_c,
    mAppHandover_MonitorNotActive_c,
    mAppHandover_CsContextSetFailed_c,
} appHandoverError_t;

typedef void (*appHandoverEventCb_t)(appHandoverEvent_t eventType, void *pData);
typedef void (*appHandoverA2AInterfaceCb_t)(uint8_t opGroup, uint8_t cmdId, uint16_t len, uint8_t *pData);

/***********************************************************************************************************************
************************************************************************************************************************
* Public memory declarations
************************************************************************************************************************
***********************************************************************************************************************/

/***********************************************************************************************************************
************************************************************************************************************************
* Public prototypes
************************************************************************************************************************
***********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! ********************************************************************************************************************
*\fn           bleResult_t AppHandover_Init(appHandoverEventCb_t pfAppEventCb,
*                                           gapConnectionCallback_t pfConnectionCallback
*                                           appHandoverA2AInterfaceCb_t pfAppA2AInterfaceCb)
*\brief        Performs initialization of the handover application common module.
*
*\param  [in]  pfAppEventCb             Application callback functions for handover events.
*\param  [in]  pfConnectionCallback     Connection callback for connection events.
*\param  [in]  pfAppA2AInterfaceCb      Communication callback for handover data.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_Init
(
    appHandoverEventCb_t pfAppEventCb,
    gapConnectionCallback_t pfConnectionCallback,
    appHandoverA2AInterfaceCb_t pfAppA2AInterfaceCb
);

/*! ********************************************************************************************************************
*\fn           bleResult_t AppHandover_TimeSyncTransmit(bleHandoverTimeSyncEnable_t enable)
*\brief        Enables/disable transmission of time synchronization information.
*
*\param  [in]  enable   Enable/disabled.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_TimeSyncTransmit
(
    bleHandoverTimeSyncEnable_t enable
);

/*! ********************************************************************************************************************
*\fn           bleResult_t AppHandover_TimeSyncReceive(bleHandoverTimeSyncEnable_t enable)
*\brief        Enables/disable reception of time synchronization information.
*
*\param  [in]  enable   Enable/disabled.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_TimeSyncReceive
(
    bleHandoverTimeSyncEnable_t enable
);

/*! ********************************************************************************************************************
*\fn           void AppHandover_StartTimeSync(bool_t bTimeSyncForHandover)
*\brief        Trigger handover time synchronization.
*
*\param  [in]  bTimeSyncForHandover TRUE if handover is following, FALSE if RSSI sniffing is following.
*
*\return       None
********************************************************************************************************************* */
void AppHandover_StartTimeSync(bool_t bTimeSyncForHandover);

/*! ********************************************************************************************************************
*\fn           bleResult_t AppHandover_StartConnectionHandover(deviceId_t deviceId)
*\brief        Trigger connection handover without requiring time synchronization.
*              Anchor monitoring must already be in progress for the specified connection.
*
*\param  [in]  deviceId     Peer device identifier of the connection to be handed over.
*
*\return       bleResult_t  Status of the operation.
*                           gBleSuccess_c - Handover initiated successfully.
*                           gBleInvalidState_c - Handover already in progress or module not idle.
*                           gBleInvalidParameter_c - Invalid parameters.
*                           gBleUnavailable_c - Anchor monitoring not active for specified connection.
********************************************************************************************************************* */
bleResult_t AppHandover_StartConnectionHandover
(
    deviceId_t deviceId
);

/*! ********************************************************************************************************************
*\fn           void AppHandover_TimeSyncTransmitSetParams(gapHandoverTimeSyncTransmitParams_t *pParams)
*\brief        Set parameters for the transmission of time synchronization information.
*
*\param  [in]  pParams      Tx params.
*
*\return       None
********************************************************************************************************************* */
void AppHandover_TimeSyncTransmitSetParams
(
    gapHandoverTimeSyncTransmitParams_t *pParams
);

/*! ********************************************************************************************************************
*\fn           void AppHandover_TimeSyncReceiveSetParams(gapHandoverTimeSyncReceiveParams_t *pParams)
*\brief        Set parameters for the reception of time synchronization information.
*
*\param  [in]  pParams      Rx params.
*
*\return       None
********************************************************************************************************************* */
void AppHandover_TimeSyncReceiveSetParams
(
    gapHandoverTimeSyncReceiveParams_t *pParams
);

/*! ********************************************************************************************************************
*\fn           void AppHandover_SetPeerDevice(deviceId_t deviceId)
*\brief        Set the peer device for connection handover.
*
*\param  [in]  deviceId     Peer devive identifier for connection handover.
*
*\return       None
********************************************************************************************************************* */
void AppHandover_SetPeerDevice
(
    deviceId_t deviceId
);

/*! ********************************************************************************************************************
*\fn           void AppHandover_ProcessA2ACommand(uint8_t cmdId, uint32_t cmdLen, uint8_t *pCmdData)
*\brief        Process handover commads.
*
*\param  [in]  cmdId        Handover commmand identifier.
*\param  [in]  cmdLen       Handover commmand length.
*\param  [in]  pCmdData     Handover commmand data.
*
*\return       None
********************************************************************************************************************* */
void AppHandover_ProcessA2ACommand
(
    uint8_t     cmdId,
    uint32_t    cmdLen,
    uint8_t     *pCmdData
);

/*! ********************************************************************************************************************
*\fn           bleResult_t AppHandover_SetPeerSkd(uint8_t *pSkd, uint8_t nvmIndex)
*\brief        Set the peer LL SKD. Required for connection handover with advanced secure mode.
*
*\param  [in]  nvmIndex     Peer device NVM index.
*\param  [in]  pSkd         LL Session Key Diversifier.
*
*\return       bleResult_t  Status of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_SetPeerSkd
(
    uint8_t nvmIndex,
    uint8_t *pSkd
);

/*! ********************************************************************************************************************
*\fn            bleResult_t AppHandover_GetPeerSkd(uint8_t *pOutSkd)
*\brief         Get the peer LL SKD for a peer device based on the NVM index. Required for connection handover with
*               advanced secure mode.
*
*\param  [in]   nvmIndex    NVM index for the peer device..
*\param  [in]   pOutSkd     Pointer to memory location where the LL Session Key Diversifier is to be copied.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_GetPeerSkd
(
    uint8_t nvmIndex,
    uint8_t *pOutSkd
);

/*! ********************************************************************************************************************
*\fn           void AppHandover_GenericCallback(gapGenericEvent_t* pGenericEvent)
*\brief        Handler of Handover related generic events.
*
*\param  [in]  pGenericEvent    Pointer to generic event.
*
*\return       None.
********************************************************************************************************************* */
void AppHandover_GenericCallback
(
    gapGenericEvent_t* pGenericEvent
);

/*! ********************************************************************************************************************
*\fn           void AppHandover_ConnectionCallback(deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent)
*\brief        Handler of Handover related connection events.
*
*\param  [in]  pGenericEvent    Pointer to connection event.
*
*\return       None.
********************************************************************************************************************* */
void AppHandover_ConnectionCallback
(
    deviceId_t peerDeviceId,
    gapConnectionEvent_t* pConnectionEvent
);

/*! ********************************************************************************************************************
*\fn           void AppHandover_Abort(void)
*\brief        Abort current connection handover process.
*
*\param  [in]  notifyPeerAnch   TRUE, notify peer device of handover failure, FALSE otherwise.
*\param  [in]  error            Handover error status
*
*\return       None.
********************************************************************************************************************* */
void AppHandover_Abort
(
    bool_t notifyPeerAnch,
    appHandoverError_t error
);

/*! ********************************************************************************************************************
*\fn            bleResult_t AppHandover_AnchorMonitorStart(deviceId_t deviceId)
*\brief         Trigger Anchor Monitor on remote anchor
*
*\param  [in]   deviceId     peer device id of the connection to be monitored.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_AnchorMonitorStart
(
    deviceId_t deviceId
);

/*! ********************************************************************************************************************
*\fn            bleResult_t AppHandover_AnchorMonitorStop(deviceId_t deviceId)
*\brief         Stop Anchor Monitor
*
*\param  [in]   deviceId    Peer device identifier
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_AnchorMonitorStop
(
    deviceId_t deviceId
);

/*! ********************************************************************************************************************
*\fn            void AppHandover_AnchorMonitorReset(deviceId_t deviceId)
*\brief         Stop remote anchor monitoring and free the local monitor slot at once.
*               Use on a live-connection disconnect so a later SetMonitorMode for the
*               same device cannot fail with gBleInvalidState_c.
*
*\param  [in]   deviceId    Peer device identifier.
*
*\return        None
********************************************************************************************************************* */
void AppHandover_AnchorMonitorReset
(
    deviceId_t deviceId
);

/*! ********************************************************************************************************************
*\fn            bleResult_t AppHandover_SetMonitorMode(deviceId_t deviceId, bleHandoverAnchorSearchMode_t mode, bool_t silent)
*\brief         Set Anchor Monitor mode
*
*\param  [in]   deviceId    Connection to be monitored.
*\param  [in]   mode        Anchor monitor mode
*\param  [in]   silent      TRUE to keep monitoring active at LL level but suppress
*                           application monitoring reports over A2A, FALSE otherwise.
*
*\return        bleResult_t Status of the operation
********************************************************************************************************************* */
bleResult_t AppHandover_SetMonitorMode
(
    deviceId_t deviceId,
    bleHandoverAnchorSearchMode_t mode,
    bool_t silent
);

/*! ********************************************************************************************************************
*\fn            uint16_t AppHandover_GetMonitoredConnHandle(deviceId_t deviceId)
*\brief         Get the monitored connection handle for a device.
*
*\param  [in]   deviceId    Connection identifier.
*
*\return        uint16_t    Monitored connection handle, or gInvalidConnectionHandle_c if not monitored.
********************************************************************************************************************* */
uint16_t AppHandover_GetMonitoredConnHandle
(
    deviceId_t deviceId
);

#if defined(gAppCsSniffing_d) && (gAppCsSniffing_d == 1)
/*! ********************************************************************************************************************
*\fn            deviceId_t AppHandover_GetDeviceIdFromMonitoredConnHandle(uint16_t monitorConnHandle)
*\brief         Get the deviceId associated with a monitored connection handle.
*
*\param  [in]   monitorConnHandle   Monitored connection handle on the remote anchor.
*
*\return        deviceId_t          Associated deviceId, or gInvalidDeviceId_c if not found.
********************************************************************************************************************* */
deviceId_t AppHandover_GetDeviceIdFromMonitoredConnHandle
(
    uint16_t monitorConnHandle
);

/*! ********************************************************************************************************************
*\fn            bleResult_t AppHandover_GetAndSetCsLlContext(deviceId_t deviceId, bool_t pending)
*\brief         Mark the pending CS LL context request as being for CS sniffing (TRUE) or
*               connection handover (FALSE), then request the LL CS context from the stack.
*               When set for sniffing, the CS LL context command sent to the peer anchor
*               carries the sniffing flag, so the peer starts CS monitoring once the
*               context is applied.
*
*\param  [in]   deviceId    Peer device identifier to fetch the CS LL context for.
*\param  [in]   pending     TRUE if the pending CS LL context get is for CS sniffing.
*
*\return        bleResult_t Result of the Gap_HandoverGetCsLlContext operation.
********************************************************************************************************************* */
bleResult_t AppHandover_GetAndSetCsLlContext
(
    deviceId_t deviceId,
    bool_t pending
);

/*! ********************************************************************************************************************
*\fn            void AppHandover_NotifyRemoteDevice(uint8_t cmdId, uint16_t len, uint8_t *pData)
*\brief         Send a handover A2A command to the peer anchor through the registered A2A
*               interface callback.
*
*\param  [in]   cmdId   Handover A2A command opcode.
*\param  [in]   len     Command payload length.
*\param  [in]   pData   Pointer to the command payload.
*
*\return        None
********************************************************************************************************************* */
void AppHandover_NotifyRemoteDevice
(
    uint8_t cmdId,
    uint16_t len,
    uint8_t *pData
);

/*! ********************************************************************************************************************
*\fn            void AppHandover_FreeMonitorSlotsForDevice(deviceId_t deviceId)
*\brief         Release every anchor monitor slot held for a device.
*
*\param  [in]   deviceId   Peer device identifier.
*
*\return        None
********************************************************************************************************************* */
void AppHandover_FreeMonitorSlotsForDevice
(
    deviceId_t deviceId
);

/*! ********************************************************************************************************************
*\fn            void AppHandover_FreeInactiveMonitorSlots(void)
*\brief         Release every anchor monitor slot that never reached the active state (starting or
*               stale).
*
*\return        None
********************************************************************************************************************* */
void AppHandover_FreeInactiveMonitorSlots(void);

/*! ********************************************************************************************************************
*\fn            deviceId_t AppHandover_ActivateMonitorSlot(uint16_t connHandle)
*\brief         Bind the pending monitor slot to the supplied remote connection handle and mark it
*               active.
*
*\param  [in]   connHandle   Monitored connection handle on the remote (sniffer) anchor.
*
*\return        deviceId_t   Local device id of the activated slot, or gInvalidDeviceId_c if none pending.
********************************************************************************************************************* */
deviceId_t AppHandover_ActivateMonitorSlot
(
    uint16_t connHandle
);

/*! ********************************************************************************************************************
*\fn            void AppHandover_NotifyAppEvent(appHandoverEvent_t event, void *pParam)
*\brief         Invoke the registered application event callback. Exposed so the CS sniffing add-on
*               can raise application handover events without duplicating the private callback pointer.
*
*\param  [in]   event    Application handover event.
*\param  [in]   pParam   Opaque event parameter forwarded to the callback.
*
*\return        None
********************************************************************************************************************* */
void AppHandover_NotifyAppEvent
(
    appHandoverEvent_t event,
    void *pParam
);

/*! ********************************************************************************************************************
*\fn            bool_t AppHandover_IsConnHandoverInProgress(void)
*\brief         Report whether a real connection handover is in progress on this anchor
*               (mHandoverCtx.centralDeviceId is valid).
*
*\return        bool_t    TRUE if a connection handover is in progress, FALSE otherwise.
********************************************************************************************************************* */
bool_t AppHandover_IsConnHandoverInProgress(void);

#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1)
/*! ********************************************************************************************************************
*\fn            bleResult_t AppHandover_HandleCsLlContextCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Run the connection-handover CS LL context command handling.
*
*\param  [in]   pCmdData  Pointer to the command payload at the develop/main layout start (responseMask).
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_HandleCsLlContextCommand
(
    uint8_t *pCmdData,
    appHandoverError_t *pError
);
#endif /* defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1) */

#endif /* defined(gAppCsSniffing_d) && (gAppCsSniffing_d == 1) */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* APP_HANDOVER_H */