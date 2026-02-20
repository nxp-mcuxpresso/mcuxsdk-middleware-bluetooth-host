/*! ********************************************************************************************************************
 * \addtogroup AUTO
 * @{
 ********************************************************************************************************************* */
/*! ********************************************************************************************************************
* Copyright 2022-2026 NXP
*
*
* \file app_handover.c
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************************************************* */

/*! ********************************************************************************************************************
************************************************************************************************************************
* Include
************************************************************************************************************************
********************************************************************************************************************* */
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
#include "app_handover.h"
#include "gap_interface.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_messaging.h"
#include "hci_types.h"
/***********************************************************************************************************************
************************************************************************************************************************
* Private macros
************************************************************************************************************************
***********************************************************************************************************************/
#define gWaitingForConnectionHandle_c (0xFFFEU)
#define gMonitorConnectionHandlePending (0xFFFEU)

/***********************************************************************************************************************
************************************************************************************************************************
* Private type definitions
************************************************************************************************************************
***********************************************************************************************************************/
typedef struct appHandoverPeerDeviceData_tag
{
    uint16_t    connectionHandle;
    uint8_t     nvmIndex;
    uint8_t     aPeerSkd[gSkdSize_c];
} appHandoverPeerDeviceData_t;

typedef enum
{
    gIdle_c                         = 0x00,
    gContextTx_c                    = 0x01,
    gContextRx_c                    = 0x02,
    gAnchorSearch_c                 = 0x03,
    gAnchorMonitorRemoteStarting_c  = 0x04,
    gAnchorMonitorLocal_c           = 0x05,
} appHandoverState_t;

typedef enum
{
    gTimeSyncIdle_c                 = 0x00,
    gTimeSyncRx_c                   = 0x01,
    gTimeSyncTx_c                   = 0x02,
} appTimeSyncState_t;

typedef struct appMonitoringState_tag
{
    uint16_t                        monitorConnHandle;
    bleHandoverAnchorSearchMode_t   monitorMode;
} appMonitoringState_t;

typedef struct appMonitorFilter_tag
{
    uint16_t connHandle;
    uint32_t eventCount;
    int32_t rssiRemoteSum;
    int32_t rssiActiveSum;
} appMonitorFilter_t;

/***********************************************************************************************************************
************************************************************************************************************************
* Private memory declarations
************************************************************************************************************************
***********************************************************************************************************************/
static uint32_t mSlotLocal;
static uint16_t mOffsetLocal;
static uint32_t mSlotRemote;
static uint16_t mOffsetRemote;

static uint32_t   mHandoverDataSize;
static uint32_t   *mpHandoverData;

static bool_t gHandoverDeviceFound = FALSE;
static bool_t gTimeSyncForHandover = FALSE;

static appHandoverEventCb_t mpfAppEventCb = NULL;
static gapConnectionCallback_t mpfAppConnCb = NULL;
static appHandoverA2AInterfaceCb_t mpfAppA2AInterfaceCb = NULL;

static gapHandoverTimeSyncTransmitParams_t mHandoverTimeSyncTransmitParams = {
    .enable = gTimeSyncEnable_c,
    .advChannel = gHandoverDefaultAdvChan_c,
    .deviceAddress = HANDOVER_HTS_ADDR,
    .txPowerLevel = gHandoverTimeSyncDefaultPowerLvl_c,
    .txIntervalSlots625 = gHandoverTimeSyncDefaultTxInterval_c,
    .phys = gHandoverTimeSyncDefaultPhy_c
};

static gapHandoverTimeSyncReceiveParams_t mHandoverTimeSyncReceiveParams = {
    .enable = gTimeSyncEnable_c,
    .scanChannel = gHandoverDefaultAdvChan_c,
    .deviceAddress = HANDOVER_HTS_ADDR,
    .stopWhenFound = gTimeSyncStopWhenFound_c,
    .phys = gHandoverTimeSyncDefaultPhy_c
};

static deviceId_t mHandoverCentralDeviceId = gInvalidDeviceId_c;
static uint16_t mHandoverConnHandle = gInvalidConnectionHandle_c;
static appHandoverState_t mAppHandoverState = gIdle_c;
static appTimeSyncState_t mAppTimeSyncState = gTimeSyncIdle_c;
static bool_t mContinuousAnchorMonitoring = FALSE;
static uint8_t mHandoverAnchorSearchThreshold = gHandoverAnchorSearchThreshold_c;

#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
static appHandoverPeerDeviceData_t maPeerDeviceInfo[gMaxBondedDevices_c];
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
static appMonitoringState_t maAppMonitorData[gAppMaxConnections_c];
static appMonitorFilter_t maAppMonitorFilter[gAppMaxConnections_c];

static uint16_t gSizeOfDataTxInOldestPacket;
static messaging_t mSrcLlPendingDataQueue;
/***********************************************************************************************************************
************************************************************************************************************************
* Public memory declarations
************************************************************************************************************************
***********************************************************************************************************************/

/***********************************************************************************************************************
************************************************************************************************************************
* Private functions prototypes
************************************************************************************************************************
***********************************************************************************************************************/
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
static uint8_t getNvmIndexFromConnHandle(uint16_t connectionHandle);
static bleResult_t setNvmIndexForAnchSearchStart(uint8_t nvmIndex);
static bleResult_t setConnHandleForAnchSearchStart(uint16_t connectionHandle);
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
static void notifyRemoteDevice(uint8_t cmdId, uint16_t len, uint8_t *pData);
static deviceId_t getMonitoredDeviceId(uint16_t monitorConnectionHandle);
static uint16_t getMonitoredConnHandle(deviceId_t deviceId);
static bool_t checkMonitorFilterCounter(uint16_t connHandle, int8_t rssiRemote, int8_t rssiActive);
static void addMonitorFilter(uint16_t connHandle);
static void removeMonitorFilter(uint16_t connHandle);
static int8_t getMonitorFilterAverageActiveRssi(uint16_t connHandle);
static int8_t getMonitorFilterAverageRemoteRssi(uint16_t connHandle);
static bleResult_t anchorMonitorStop(uint16_t connHandle);
static void anchorMonitorRemoteStop(uint16_t connHandle);
static bleResult_t HandleTimeSyncReceiveComplete(gapGenericEvent_t *pGenericEvent, appHandoverError_t *pError);
static void HandleHandoverTimeSyncTransmitStateChanged();
static bleResult_t HandleTimeSyncEvent(gapGenericEvent_t *pGenericEvent);
static bleResult_t HandleAnchorSearchStarted(gapGenericEvent_t* pGenericEvent, appHandoverError_t *pError);
static void HandleAnchorSearchStopped(gapGenericEvent_t *pGenericEvent);
static void HandleAnchorMonitorPacketContinueEvent(gapGenericEvent_t *pGenericEvent);
static bleResult_t HandleSuspendTransmitComplete(gapGenericEvent_t *pGenericEvent, appHandoverError_t *pError);
#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1)
static bleResult_t HandleGetCsLlContextComplete(gapGenericEvent_t *pGenericEvent);
#endif /* defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1) */
static bleResult_t HandleAnchorMonitorEvent(gapGenericEvent_t* pGenericEvent, appHandoverError_t *pError);
static bleResult_t HandleGetConnParamsComplete(gapGenericEvent_t *pGenericEvent, appHandoverError_t *pError);
static void HandleHandoverConnParamUpdateEvent(gapGenericEvent_t *pGenericEvent);
static bleResult_t HandleConnectionUpdateProcedureEvent(gapGenericEvent_t *pGenericEvent);
static bleResult_t HandleInternalError(gapGenericEvent_t *pGenericEvent, appHandoverError_t *pError);
static void HandleLlPendingData(gapGenericEvent_t *pGenericEvent);
static bleResult_t HandleGetComplete(gapGenericEvent_t* pGenericEvent);
static void HandleFreeComplete();
static void HandleAnchorMonitorPacketEvent(gapGenericEvent_t *pGenericEvent);
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
static bleResult_t HandleSkdReportEvent(gapGenericEvent_t *pGenericEvent);
#endif
static void HandleError(bleResult_t result, appHandoverError_t error);
static bleResult_t HandleStopTimeSyncCommand(appHandoverError_t *pError);
#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1)
static bleResult_t HandleCsLlContextCommand(uint8_t *pCmdData, appHandoverError_t *pError);
static bleResult_t HandleCsContextCompletedCommand(appHandoverError_t *pError);
#endif /* defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1) */
static bleResult_t HandleLlPendingDataCommand(uint8_t *pCmdData, appHandoverError_t *pError);
static bleResult_t HandleDataCommand(uint32_t cmdLen, uint8_t *pCmdData, appHandoverError_t *pError);
static bleResult_t HandleAnchorStartSearchCommand(uint8_t *pCmdData, appHandoverError_t *pError);
static bleResult_t HandleAnchMonStartedCommand(uint8_t *pCmdData, appHandoverError_t *pError);
static bleResult_t HandleAnchorMonitorCommand(uint8_t *pCmdData, appHandoverError_t *pError);
static bleResult_t HandlePacketMonitorCommand(uint8_t *pCmdData, appHandoverError_t *pError);
static bleResult_t HandlePacketContinueMonitorCommand(uint8_t *pCmdData, appHandoverError_t *pError);
static bleResult_t HandleStopAnchorMonitorCommand(uint8_t *pCmdData, appHandoverError_t *pError);
static bleResult_t HandleAnchMonStoppedCommand(uint8_t *pCmdData, appHandoverError_t *pError);
static bleResult_t HandleInformConnectCommand(appHandoverError_t *pError);
static bleResult_t HandleInformFailureCommand(appHandoverError_t *pError);
static bleResult_t HandleStartTimeSyncCommand(uint8_t *pCmdData, appHandoverError_t *pError);
static bleResult_t HandleSetSkdCommand(uint8_t *pCmdData, appHandoverError_t *pError);
static bleResult_t HandleConnectionUpdateParamsCommand(uint8_t *pCmdData, appHandoverError_t *pError);

/***********************************************************************************************************************
************************************************************************************************************************
* Public functions
************************************************************************************************************************
***********************************************************************************************************************/

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
)
{
    bleResult_t result = gBleSuccess_c;
    
    if ((pfAppEventCb == NULL) ||
        (pfConnectionCallback == NULL) ||
        (pfAppA2AInterfaceCb == NULL))
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        mpfAppEventCb = pfAppEventCb;
        mpfAppConnCb = pfConnectionCallback;
        mpfAppA2AInterfaceCb = pfAppA2AInterfaceCb;
        
        result = Gap_HandoverInit();
    }
    
    for (uint32_t i = 0U; i < (uint32_t)gAppMaxConnections_c; i++)
    {
        maAppMonitorData[i].monitorConnHandle = gInvalidConnectionHandle_c;
        maAppMonitorData[i].monitorMode = gSuspendTxMode_c;
        maAppMonitorFilter[i].connHandle = gInvalidConnectionHandle_c;
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
        maPeerDeviceInfo[i].connectionHandle = gInvalidConnectionHandle_c;
        maPeerDeviceInfo[i].nvmIndex = gInvalidNvmIndex_c;
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
    }
    
    MSG_QueueInit(&mSrcLlPendingDataQueue);

    return result;
}

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
)
{
    bleResult_t result = gBleSuccess_c;
    
    gHandoverDeviceFound = FALSE;
    mHandoverTimeSyncTransmitParams.enable = enable;
    result = Gap_HandoverTimeSyncTransmit(&mHandoverTimeSyncTransmitParams);
    
    return result;
}

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
)
{
    bleResult_t result = gBleSuccess_c;
    
    gHandoverDeviceFound = FALSE;
    mHandoverTimeSyncReceiveParams.enable = enable;
    result = Gap_HandoverTimeSyncReceive(&mHandoverTimeSyncReceiveParams);
    
    return result;
}

/*! ********************************************************************************************************************
*\fn           bleResult_t AppHandover_StartTimeSync(bool_t bTimeSyncForHandover)
*\brief        Trigger handover time synchronization.
*
*\param  [in]  bTimeSyncForHandover TRUE if handover is following, FALSE is RSSI sniffing is following.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_StartTimeSync(bool_t bTimeSyncForHandover)
{
    bleResult_t result = gBleSuccess_c;

    gTimeSyncForHandover = bTimeSyncForHandover;

    /* Start scanning for handover time sync */
    result = AppHandover_TimeSyncReceive(gTimeSyncEnable_c);
    
    if (result == gBleSuccess_c)
    {
        /* Tell other anchor to start advertising for time sync */
        notifyRemoteDevice(gHandoverStartTimeSyncCommandOpCode_c, 1U, (uint8_t*)&bTimeSyncForHandover);
    }
    
    return result;
}

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
)
{
    FLib_MemCpy(&mHandoverTimeSyncTransmitParams, pParams, sizeof(gapHandoverTimeSyncTransmitParams_t));
}

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
)
{
    FLib_MemCpy(&mHandoverTimeSyncReceiveParams, pParams, sizeof(gapHandoverTimeSyncReceiveParams_t));
}

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
)
{
    mHandoverCentralDeviceId = deviceId;
}

/*! ********************************************************************************************************************
*\fn           void AppHandover_ProcessA2ACommand(uint8_t cmdId, uint32_t cmdLen, uint8_t *pCmdData)
*\brief        Process handover commands.
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
)
{
    bleResult_t result = gBleSuccess_c;
    appHandoverError_t error = mAppHandover_UnexpectedError_c;
    
    switch (cmdId)
    {
        case gHandoverStopTimeSyncCommandOpCode_c:
        {
            result = HandleStopTimeSyncCommand(&error);
        }
        break;

#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1)
        case gHandoverCsLlContextCommandOpCode_c:
        {
            result = HandleCsLlContextCommand(pCmdData, &error);
        }
        break;
        
        case gHandoverCsContextCompletedCommandOpCode_c:
        {
            result = HandleCsContextCompletedCommand(&error);
        }
        break;
#endif
        case gHandoverLlPendingDataCommandOpCode_c:
        {
            result = HandleLlPendingDataCommand(pCmdData, &error);
        }
        break;

        case gHandoverDataCommandOpCode_c:
        {
            result = HandleDataCommand(cmdLen, pCmdData, &error);
        }
        break;

        case gHandoverAnchorStartSearchCommandOpCode_c:
        {
            result = HandleAnchorStartSearchCommand(pCmdData, &error);
        }
        break;
        
        case gHandoverAnchMonStartedCommandOpCode_c:
        {
            result = HandleAnchMonStartedCommand(pCmdData, &error);
        }
        break;
    
        case gHandoverAnchorMonitorCommandOpCode_c:
        {
            result = HandleAnchorMonitorCommand(pCmdData, &error);
        }
        break;
    
        case gHandoverPacketMonitorCommandOpCode_c:
        {
            result = HandlePacketMonitorCommand(pCmdData, &error);
        }
        break;
    
        case gHandoverPacketContinueMonitorCommandOpCode_c:
        {
            result = HandlePacketContinueMonitorCommand(pCmdData, &error);
        }
        break;
        
        case gHandoverStopAnchorMonitorCommandOpCode_c:
        {
            result = HandleStopAnchorMonitorCommand(pCmdData, &error);
        }
        break;
        
        case gHandoverAnchMonStoppedCommandOpCode_c:
        {
            result = HandleAnchMonStoppedCommand(pCmdData, &error);
        }
        break;
        
        case gHandoverInformConnectCommandOpCode_c:
        {
            result = HandleInformConnectCommand(&error);
        }
        break;

        case gHandoverInformFailureCommandOpCode_c:
        {
            result = HandleInformFailureCommand(&error);
        }
        break;

        case gHandoverStartTimeSyncCommandOpCode_c:
        {
            result = HandleStartTimeSyncCommand(pCmdData, &error);
        }
        break;
        
        case gHandoverSetSkdCommandOpCode_c:
        {
            result = HandleSetSkdCommand(pCmdData, &error);
        }
        break;
        
        case gHandoverLConnectionUpdateParamsCommandOpCode_c:
        {
            result = HandleConnectionUpdateParamsCommand(pCmdData, &error);
        }
        break;

        default:
        {
            /* */
        }
        break;
    }
    
    if (result != gBleSuccess_c)
    {
        AppHandover_Abort(TRUE, error);
    }
}

/*! ********************************************************************************************************************
*\fn           bleResult_t AppHandover_SetPeerSkd(uint8_t nvmIndex, uint8_t *pSkd)
*\brief        Set the peer LL SKD. Required for connection handover with advanced secure mode.
*
*\param  [in]  nvmIndex     Peer device NVM index.
*\param  [in]  pSkd         LL Session Key Diversifier.
*
*\return       bleResult_t  Status of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_SetPeerSkd(uint8_t nvmIndex, uint8_t *pSkd)
{
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
    bleResult_t result = gBleSuccess_c;
    
    if ((pSkd == NULL) || (nvmIndex >= (uint8_t)gMaxBondedDevices_c))
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        maPeerDeviceInfo[nvmIndex].nvmIndex = nvmIndex;
        FLib_MemCpy(maPeerDeviceInfo[nvmIndex].aPeerSkd, pSkd, gSkdSize_c);
    }
    
#else
    bleResult_t result = gBleUnavailable_c;
#endif
    
    return result;
}

/*! ********************************************************************************************************************
*\fn           bleResult_t AppHandover_GetPeerSkd(uint8_t nvmIndex, uint8_t *pOutSkd)
*\brief         Get the peer LL SKD for a peer device based on the NVM index. Required for connection handover with
*               advanced secure mode.
*
*\param  [in]   nvmIndex    NVM index for the peer device..
*\param  [in]   pOutSkd     Pointer to memory location where the LL Session Key Diversifier is to be copied.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_GetPeerSkd(uint8_t nvmIndex, uint8_t *pOutSkd)
{
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
    bleResult_t result = gBleSuccess_c;
    
    if ((pOutSkd == NULL) || (nvmIndex >= (uint8_t)gMaxBondedDevices_c))
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        FLib_MemCpy(pOutSkd, maPeerDeviceInfo[nvmIndex].aPeerSkd, gSkdSize_c);
    }
#else
    bleResult_t result = gBleUnavailable_c;
#endif
    return result;
}

/*! ********************************************************************************************************************
*\fn           void AppHandover_GenericCallback(gapGenericEvent_t* pGenericEvent)
*\brief        Handler of Handover related generic events.
*
*\param  [in]  pGenericEvent    Pointer to generic event.
*
*\return       None.
********************************************************************************************************************* */
void AppHandover_GenericCallback(gapGenericEvent_t* pGenericEvent)
{
    bleResult_t result = gBleSuccess_c;
    appHandoverError_t error = mAppHandover_UnexpectedError_c;
    
    switch (pGenericEvent->eventType)
    {
        case gHandoverTimeSyncReceiveComplete_c:
        {
            HandleTimeSyncReceiveComplete(pGenericEvent, &error);
        }
        break;
        
        case gHandoverTimeSyncTransmitStateChanged_c:
        {
            HandleHandoverTimeSyncTransmitStateChanged();
        }
        break;
        
        case gHandoverTimeSyncEvent_c:
        {
            result = HandleTimeSyncEvent(pGenericEvent);
        }
        break;

        case gHandoverAnchorSearchStarted_c:
        {
            result = HandleAnchorSearchStarted(pGenericEvent, &error);
        }
        break;
        
        case gHandoverAnchorSearchStopped_c:
        {
            HandleAnchorSearchStopped(pGenericEvent);
        }
        break;
        
        case gHandoverAnchorMonitorEvent_c:
        {
            result = HandleAnchorMonitorEvent(pGenericEvent, &error);
        }
        break;
        
        case gHandoverAnchorMonitorPacketEvent_c:
        {
            HandleAnchorMonitorPacketEvent(pGenericEvent);
        }
        break;
        
        case gHandoverAnchorMonitorPacketContinueEvent_c:
        {
            HandleAnchorMonitorPacketContinueEvent(pGenericEvent);
        }
        break;

        case gHandoverSuspendTransmitComplete_c:
        {
            result = HandleSuspendTransmitComplete(pGenericEvent, &error);
        }
        break;

#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1)
        case gHandoverGetCsLlContextComplete_c:
        {
            result = HandleGetCsLlContextComplete(pGenericEvent);
        }
        break;
        
        case gHandoverSetCsLlContextComplete_c:
        {
            /* LL CS context set - inform peer to disconnect */
            notifyRemoteDevice(gHandoverCsContextCompletedCommandOpCode_c, 0U, NULL);
        }
        break;
#endif
        case gHandoverLlPendingData_c:
        {
            HandleLlPendingData(pGenericEvent);
        }
        break;

        case gHandoverGetComplete_c:
        {
            result = HandleGetComplete(pGenericEvent);
        }
        break;

        case gHandoverFreeComplete_c:
        {
            HandleFreeComplete();
        }
        break;

        case gGetConnParamsComplete_c:
        {
            result = HandleGetConnParamsComplete(pGenericEvent, &error);
        }
        break;

        case gHandoverConnParamUpdateEvent_c:
        {
            HandleHandoverConnParamUpdateEvent(pGenericEvent);
        }
        break;

        case gHandoverConnectionUpdateProcedureEvent_c:
        {
            result = HandleConnectionUpdateProcedureEvent(pGenericEvent);
        }
        break;

        case gHandoverApplyConnectionUpdateProcedureComplete_c:
        {
            mHandoverConnHandle = gInvalidConnectionHandle_c;
        }
        break;

        case gInternalError_c:
        {
            result = HandleInternalError(pGenericEvent, &error);
        }
        break;
        
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
        case gLlSkdReportEvent_c:
        {
            HandleSkdReportEvent(pGenericEvent);
        }
        break;
#endif

        default:
        {
            ; /* No action required */
        }
        break;
    }
    
    HandleError(result, error);
}

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
)
{
    switch (pConnectionEvent->eventType)
    {
        case gConnEvtHandoverConnected_c:
        {
            bleResult_t result =  gBleSuccess_c;
            mAppHandoverState = gIdle_c;
            
            result = Gap_HandoverFreeData();
            
            if (result != gBleSuccess_c)
            {
                /* Handover data not set*/
                (void)MEM_BufferFree(mpHandoverData);
                mpHandoverData = NULL;
                mHandoverDataSize = 0;
            }
            
            if (mpfAppEventCb != NULL)
            {
                mpfAppEventCb(mAppHandover_ConnectComplete_c, &peerDeviceId);
            }
            
            AppHandover_SetPeerDevice(peerDeviceId);
            notifyRemoteDevice(gHandoverInformConnectCommandOpCode_c, 0U, NULL);
        }
        break;

        case gHandoverDisconnected_c:
        {
            if (mpfAppEventCb != NULL)
            {
                mpfAppEventCb(mAppHandover_Disconnected_c, &peerDeviceId);
            }
        }
        break;
        
        default:
        {
            ;/* No action required. */
        }
        break;
    }
}

/*! ********************************************************************************************************************
*\fn           void AppHandover_Abort(void)
*\brief        Abort current connection handover process.
*
*\param  [in]  notifyPeerAnch   TRUE, notify peer device of handover failure, FALSE otherwise.
*\param  [in]  error            Handover error status
*
*\return       None.
********************************************************************************************************************* */
void AppHandover_Abort(bool_t notifyPeerAnch, appHandoverError_t error)
{
    bleResult_t result = gBleSuccess_c;
    
    switch (mAppTimeSyncState)
    {
        case gTimeSyncRx_c:
        {
            /* Stop scanning for handover time sync */
            (void)AppHandover_TimeSyncReceive(gTimeSyncDisable_c);
        }
        break;

        case gTimeSyncTx_c:
        {
            /* Time Sync Tx active, disable it. */
            (void)AppHandover_TimeSyncTransmit(gTimeSyncDisable_c);
        }
        break;

        default:
        {
            ; /* No action required. */
        }
        break;
    }
    
    switch (mAppHandoverState)
    {
        case gContextTx_c:
        {
            (void)Gap_HandoverResumeTransmit(mHandoverCentralDeviceId);
        }
        break;
        
        case gAnchorSearch_c:
        {
            (void)Gap_HandoverAnchorSearchStop(mHandoverConnHandle);
            mHandoverConnHandle = gInvalidConnectionHandle_c;
        }
        break;
        
        case gAnchorMonitorLocal_c:
        {
            /* Stop anchor search */
            (void)Gap_HandoverAnchorSearchStop(mHandoverConnHandle);
            mHandoverConnHandle = gInvalidConnectionHandle_c;
        }
        break;

        default:
        {
            ; /* No action required. */
        }
        break;
    }

    result = Gap_HandoverFreeData();

    if (result != gBleSuccess_c)
    {
        /* Handover data not set*/
        (void)MEM_BufferFree(mpHandoverData);
        mpHandoverData = NULL;
        mHandoverDataSize = 0;
    }
    mHandoverCentralDeviceId = gInvalidDeviceId_c;

    mAppHandoverState = gIdle_c;
    
    while(MSG_QueueGetHead(&mSrcLlPendingDataQueue) != NULL)
    {
        uint8_t *pTxData = MSG_QueueRemoveHead(&mSrcLlPendingDataQueue);
        MSG_Free(pTxData);
    }
    
    if (notifyPeerAnch)
    {
        notifyRemoteDevice(gHandoverInformFailureCommandOpCode_c, 0U, NULL);
    }
    
    /* Notify application. */
    if (mpfAppEventCb != NULL)
    {
        mpfAppEventCb(mAppHandover_Error_c, &error);
    }
}

/*! ********************************************************************************************************************
*\fn            bleResult_t AppHandover_AnchorMonitorStart(deviceId_t deviceId)
*\brief         Trigger Anchor Monitor on remote anchor
*
*\param  [in]   deviceId     peer device id of the connection to be monitored.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_AnchorMonitorStart(deviceId_t deviceId)
{
    bleResult_t status = gBleSuccess_c;
    
    if (deviceId == gInvalidDeviceId_c)
    {
        status = gBleInvalidParameter_c;
    }
    else if (mAppHandoverState != gIdle_c)
    {
        status = gBleInvalidState_c;
    }
    else
    {
        mAppHandoverState = gAnchorMonitorRemoteStarting_c;
        status = Gap_GetConnParamsMonitoring(deviceId, 3U);
    }
    
    return status;
}

/*! ********************************************************************************************************************
*\fn            bleResult_t AppHandover_AnchorMonitorStop(deviceId_t deviceId)
*\brief         Stop Anchor Monitor
*
*\param  [in]   deviceId    Peer device identifier
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
bleResult_t AppHandover_AnchorMonitorStop(deviceId_t deviceId)
{
    bleResult_t status = gBleSuccess_c;
    uint16_t monitoredConnHandle = getMonitoredConnHandle(deviceId);
    
    if (monitoredConnHandle != gInvalidConnectionHandle_c)
    {
        anchorMonitorRemoteStop(monitoredConnHandle);
    }
    else
    {
        status = gBleInvalidParameter_c;
    }
    
    return status;
}

/*! ********************************************************************************************************************
*\fn            bleResult_t AppHandover_SetMonitorMode(deviceId_t deviceId, bleHandoverAnchorSearchMode_t mode)
*\brief         Set Anchor Monitor mode
*
*\param  [in]   deviceId    Connection to be monitored.
*\param  [in]   mode        Anchor monitor mode
*
*\return        bleResult_t Status of the operation
********************************************************************************************************************* */
bleResult_t AppHandover_SetMonitorMode(deviceId_t deviceId, bleHandoverAnchorSearchMode_t mode)
{
    bleResult_t result = gBleSuccess_c;
    
    if ((deviceId < (uint8_t)gAppMaxConnections_c) &&
        ((mode == gRssiSniffingMode_c) || (mode == gPacketMode_c)))
    {
        if (maAppMonitorData[deviceId].monitorMode == gSuspendTxMode_c)
        {
            maAppMonitorData[deviceId].monitorMode = mode;
        }
        else
        {
            result = gBleInvalidState_c;
        }
    }
    else
    {
        result = gBleInvalidParameter_c;
    }
    
    return result;
}

/***********************************************************************************************************************
************************************************************************************************************************
* Private functions
************************************************************************************************************************
***********************************************************************************************************************/
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
/*! ********************************************************************************************************************
*\fn           static uint8_t getNvmIndexFromConnHandle(uint16_t connectionHandle)
*\brief        Get the nvm index for a connection handle.
*
*\param  [in]  deviceId     Device id of the peer device for which the nvm index is to be retrieved.
*
*\return       None.
********************************************************************************************************************* */
static uint8_t getNvmIndexFromConnHandle(uint16_t connectionHandle)
{
    uint8_t nvmIndex = gInvalidNvmIndex_c;
    
    for (uint32_t i = 0U; i < (uint32_t)gMaxBondedDevices_c; i++)
    {
        if (maPeerDeviceInfo[i].connectionHandle == connectionHandle)
        {
            nvmIndex = maPeerDeviceInfo[i].nvmIndex;
            break;
        }
    }
    
    return nvmIndex;
}

/*! ********************************************************************************************************************
*\fn           static bleResult_t setNvmIndexForAnchSearchStart(uint8_t nvmIndex)
*\brief        Set the nvm index for the current connection handover process.
*
*\param  [in]  nvmIndex     Nvm index of the peer device for which the nvm index is to be retrieved.
*
*\return       bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t setNvmIndexForAnchSearchStart(uint8_t nvmIndex)
{
    bleResult_t result = gBleUnexpectedError_c;
    
    if (nvmIndex < (uint8_t)gMaxBondedDevices_c)
    {
        maPeerDeviceInfo[nvmIndex].connectionHandle = gWaitingForConnectionHandle_c;
        result = gBleSuccess_c;
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn           static bleResult_t setConnHandleForAnchSearchStart(uint16_t connectionHandle)
*\brief        Set the nvm index for the current connection handover process.
*
*\param  [in]  connectionHandle     Connection handle of the anchor search operation.
*
*\return       bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t setConnHandleForAnchSearchStart(uint16_t connectionHandle)
{
    bleResult_t result = gBleUnexpectedError_c;
    
    for (uint32_t i = 0U; i < (uint32_t)gMaxBondedDevices_c; i++)
    {
        if (maPeerDeviceInfo[i].connectionHandle == gWaitingForConnectionHandle_c)
        {
            maPeerDeviceInfo[i].connectionHandle = connectionHandle;
            result = gBleSuccess_c;
            break;
        }
    }
    
    return result;
}
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */

/*! ********************************************************************************************************************
*\fn           static void notifyRemoteDevice(uint8_t cmdId, uint8_t len, uint8_t *pData)
*\brief        Send command to the remote device involved in the connection handover process.
*
*\param  [in]  cmdId    Handover command identifier.
*\param  [in]  len      Data payload length.
*\param  [in]  pData    Data
*
*\return       None
********************************************************************************************************************* */
static void notifyRemoteDevice(uint8_t cmdId, uint16_t len, uint8_t *pData)
{
    if (mpfAppA2AInterfaceCb != NULL)
    {
        mpfAppA2AInterfaceCb(gHandoverCommandsOpGroup_c, cmdId, len, pData);
    }
}

/*! ********************************************************************************************************************
*\fn           static deviceId_t getMonitoredDeviceId(uint16_t peerConnectionHandle)
*\brief        Get device id from the connection handle of the device performing the monitoring.
*
*\param  [in]  monitorConnectionHandle  The connection handle reported by the device performing the monitoring.
*
*\return       deviceId_t   Local connection identifier
********************************************************************************************************************* */
static deviceId_t getMonitoredDeviceId(uint16_t monitorConnectionHandle)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    
    for (uint8_t i = 0U; i < (uint8_t)gAppMaxConnections_c; i++)
    {
        if (maAppMonitorData[i].monitorConnHandle == monitorConnectionHandle)
        {
            deviceId = i;
            break;
        }
    }
    
    return deviceId;
}

/*! ********************************************************************************************************************
*\fn            static uint16_t getMonitoredConnHandle(deviceId_t deviceId)
*\brief         Set Anchor Monitor mode
*
*\param  [in]   deviceId    Connection identifier.
*
*\return        uint16_t    Monitored connection handle
********************************************************************************************************************* */
static uint16_t getMonitoredConnHandle(deviceId_t deviceId)
{
    uint16_t connectionHandle = gInvalidConnectionHandle_c;
    
    if (deviceId < (uint8_t)gAppMaxConnections_c)
    {
        connectionHandle = maAppMonitorData[deviceId].monitorConnHandle;
    }
    
    return connectionHandle;
}

/*! ********************************************************************************************************************
*\fn            static bool_t checkMonitorFilterCounter(uint16_t connHandle)
*\brief         Get the number of Anchor/Packet monitoring events received
*
*\param  [in]   connHandle  Connection identifier.
*\param  [in]   rssiRemote  RSSI of the packet from the remote device.
*\param  [in]   rssiActive  RSSI of the packet from the active device.
*
*\return        bool_t      TRUE if event should be processed, FALSE otherwise.
********************************************************************************************************************* */
static bool_t checkMonitorFilterCounter(uint16_t connHandle, int8_t rssiRemote, int8_t rssiActive)
{
    bool_t result = FALSE;
    
    for (uint8_t i = 0U; i < (uint8_t)gAppMaxConnections_c; i++)
    {
        if (maAppMonitorFilter[i].connHandle == connHandle)
        {
            maAppMonitorFilter[i].rssiActiveSum += rssiActive;
            maAppMonitorFilter[i].rssiRemoteSum += rssiRemote;

            if ((maAppMonitorFilter[i].eventCount != 0U) &&
                ((maAppMonitorFilter[i].eventCount % gHandoverMonitorPacketNumberFilter_c) == 0U))
            {
                result = TRUE;
            }
            
            maAppMonitorFilter[i].eventCount++;
            break;
        }
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static void addMonitorFilter(uint16_t connHandle)
*\brief         Add anchor/packet monitoring filter
*
*\param  [in]   connHandle  Connection identifier.
*
*\return        none
********************************************************************************************************************* */
static void addMonitorFilter(uint16_t connHandle)
{
    uint32_t firstFreeIdx = gAppMaxConnections_c;
    
    for (uint8_t i = 0U; i < (uint8_t)gAppMaxConnections_c; i++)
    {
        if (maAppMonitorFilter[i].connHandle == connHandle)
        {
            /* Already exists, reset to invalid to prevent adding duplicate */
            firstFreeIdx = gAppMaxConnections_c;
            break;
        }
        else if (maAppMonitorFilter[i].connHandle == gInvalidConnectionHandle_c)
        {
            /* Store first free index only if not already found */
            if (firstFreeIdx == (uint32_t)gAppMaxConnections_c)
            {
                firstFreeIdx = i;
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    
    if (firstFreeIdx < (uint32_t)gAppMaxConnections_c)
    {
        maAppMonitorFilter[firstFreeIdx].connHandle = connHandle;
        maAppMonitorFilter[firstFreeIdx].eventCount = 0U;
        maAppMonitorFilter[firstFreeIdx].rssiActiveSum = 0;
        maAppMonitorFilter[firstFreeIdx].rssiRemoteSum = 0;
    }
    
    return;
}

/*! ********************************************************************************************************************
*\fn            static void removeMonitorFilter(uint16_t connHandle)
*\brief         Remove anchor/packet monitoring filter
*
*\param  [in]   connHandle  Connection identifier.
*
*\return        none
********************************************************************************************************************* */
static void removeMonitorFilter(uint16_t connHandle)
{
    for (uint8_t i = 0U; i < (uint8_t)gAppMaxConnections_c; i++)
    {
        if (maAppMonitorFilter[i].connHandle == connHandle)
        {
            maAppMonitorFilter[i].connHandle = gInvalidConnectionHandle_c;
            break;
        }
    }
    
    return;
}

/*! ********************************************************************************************************************
*\fn            static void getMonitorFilterAverageActiveRssi(uint16_t connHandle)
*\brief         Return and reset the active average RSSI of received events
*
*\param  [in]   connHandle  Connection identifier.
*
*\return        Active average RSSI value
********************************************************************************************************************* */
static int8_t getMonitorFilterAverageActiveRssi(uint16_t connHandle)
{
    int averageRssi = 0;

    for (uint8_t i = 0U; i < (uint8_t)gAppMaxConnections_c; i++)
    {
        if (maAppMonitorFilter[i].connHandle == connHandle)
        {
            averageRssi = (int)((int)maAppMonitorFilter[i].rssiActiveSum / (int)gHandoverMonitorPacketNumberFilter_c);
            maAppMonitorFilter[i].rssiActiveSum = 0;
            break;
        }
    }

    return (int8_t)averageRssi;
}

/*! ********************************************************************************************************************
*\fn            static void getMonitorFilterAverageRemoteRssi(uint16_t connHandle)
*\brief         Return and reset the remote average RSSI of received events
*
*\param  [in]   connHandle  Connection identifier.
*
*\return        Active average RSSI value
********************************************************************************************************************* */
static int8_t getMonitorFilterAverageRemoteRssi(uint16_t connHandle)
{
    int averageRssi = 0;

    for (uint8_t i = 0U; i < (uint8_t)gAppMaxConnections_c; i++)
    {
        if (maAppMonitorFilter[i].connHandle == connHandle)
        {
            averageRssi = (int8_t)(maAppMonitorFilter[i].rssiRemoteSum / (int32_t)gHandoverMonitorPacketNumberFilter_c);
            maAppMonitorFilter[i].rssiRemoteSum = 0;
            break;
        }
    }

    return (int8_t)averageRssi;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t anchorMonitorStop(uint16_t connHandle)
*\brief         Stop Anchor Monitor
*
*\param  [in]   connHandle  Monitored connection identifier
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t anchorMonitorStop(uint16_t connHandle)
{
    bleResult_t status = gBleSuccess_c;
    
    /* Stop anchor search */
    uint8_t buf[gHandoverAnchMonStopCommandLen_c] = {0U};
    Utils_PackTwoByteValue(connHandle, &buf[0]);
    status = Gap_HandoverAnchorSearchStop(connHandle);
    mHandoverConnHandle = gInvalidConnectionHandle_c;
    /* Notify remote anchor that anchor monitoring was stopped */
    notifyRemoteDevice(gHandoverAnchMonStoppedCommandOpCode_c, gHandoverAnchMonStoppedCommandLen_c, buf);
    
    return status;
}

/*! ********************************************************************************************************************
*\fn            static void anchorMonitorRemoteStop(uint16_t connHandle)
*\brief         Stop Anchor Monitor on remote anchor
*
*\param  [in]   connHandle  Monitored connection identifier
*
*\return        none
********************************************************************************************************************* */
static void anchorMonitorRemoteStop(uint16_t connHandle)
{
    /* Stop anchor search on remote anchor */
    uint8_t buf[gHandoverAnchMonStopCommandLen_c] = {0U};
    
    Utils_PackTwoByteValue(connHandle, &buf[0]);
    notifyRemoteDevice(gHandoverStopAnchorMonitorCommandOpCode_c, gHandoverAnchMonStopCommandLen_c, buf);
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleTimeSyncReceiveComplete(gapGenericEvent_t *pGenericEvent, appHandoverError_t *pError)
*\brief         Handle time sync receive complete event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*\param  [out]  pError         Pointer to error status.
*
*\return        bleResult_t    Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleTimeSyncReceiveComplete
(
    gapGenericEvent_t *pGenericEvent,
    appHandoverError_t *pError
)
{
    bleResult_t result = gBleSuccess_c;

    /* Regular flow */
    if (mAppTimeSyncState == gTimeSyncIdle_c)
    {
        mAppTimeSyncState = gTimeSyncRx_c;
    }
    /* Handover was aborted while time sync was in progress */
    else if (mAppTimeSyncState == gTimeSyncRx_c)
    {
        mAppTimeSyncState = gTimeSyncIdle_c;
    }
    else
    {
        /* Should not get here */
        result = gBleUnexpectedError_c;
        *pError = mAppHandover_UnexpectedError_c;
    }
    return result;
}

/*! ********************************************************************************************************************
*\fn            static void HandleHandoverTimeSyncTransmitStateChanged(void)
*\brief         Handle handover time sync transmit state changed event
*
*\return        None
********************************************************************************************************************* */
static void HandleHandoverTimeSyncTransmitStateChanged()
{
    if (mAppTimeSyncState == gTimeSyncIdle_c)
    {
        mAppTimeSyncState = gTimeSyncTx_c;
    }
    else
    {
        mAppTimeSyncState = gTimeSyncIdle_c;
    }
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleTimeSyncEvent(gapGenericEvent_t *pGenericEvent)
*\brief         Handle time sync event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*
*\return        bleResult_t    Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleTimeSyncEvent(gapGenericEvent_t *pGenericEvent)
{
    bleResult_t result = gBleSuccess_c;
    
    if (mAppTimeSyncState == gTimeSyncRx_c)
    {
        mSlotLocal = pGenericEvent->eventData.handoverTimeSync.rxClkSlot;
        mOffsetLocal = pGenericEvent->eventData.handoverTimeSync.rxUs;
        mSlotRemote = pGenericEvent->eventData.handoverTimeSync.txClkSlot;
        mOffsetRemote = pGenericEvent->eventData.handoverTimeSync.txUs;
        
        /* Notify the other device to stop handover Time Sync. */
        notifyRemoteDevice(gHandoverStopTimeSyncCommandOpCode_c, 0U, NULL);
        
        if (gTimeSyncForHandover == TRUE)
        {
            /* Suspend Tx before retrieving the Handover Data. */
            result = Gap_HandoverSuspendTransmit(mHandoverCentralDeviceId, gSafeStopLlTx_c, 0U, 0U);
        }
        else
        {
            (void)AppHandover_AnchorMonitorStart(mHandoverCentralDeviceId);
        }

        mAppTimeSyncState = gTimeSyncIdle_c;
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleAnchorSearchStarted(gapGenericEvent_t* pGenericEvent, appHandoverError_t *pError)
*\brief         Handle anchor search started event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*\param  [out]  pError         Pointer to error status.
*
*\return        bleResult_t    Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleAnchorSearchStarted
(
    gapGenericEvent_t* pGenericEvent,
    appHandoverError_t *pError
)
{
    bleResult_t result = gBleSuccess_c;
    
    if (mAppHandoverState == gContextRx_c)
    {
        mAppHandoverState = gAnchorSearch_c;
    }
    
    if(pGenericEvent->eventData.handoverAnchorSearchStart.status != gBleSuccess_c)
    {
        *pError = mAppHandover_AnchorSearchStartFailed_c;
        result = gBleUnexpectedError_c;
    }
    else
    {
        mHandoverConnHandle = pGenericEvent->eventData.handoverAnchorSearchStart.connectionHandle;
        
        if (mAppHandoverState == gAnchorMonitorLocal_c)
        {
            uint8_t buf[gHandoverAnchMonStartedCommandLen_c] = {0U};
            
            Utils_PackTwoByteValue(pGenericEvent->eventData.handoverAnchorSearchStart.connectionHandle, &buf[0]);
            /* Send data to remote anchor */
            notifyRemoteDevice(gHandoverAnchMonStartedCommandOpCode_c, gHandoverAnchMonStartedCommandLen_c, buf);
            mAppHandoverState = gIdle_c;
            addMonitorFilter(pGenericEvent->eventData.handoverAnchorSearchStart.connectionHandle);
        }
        
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
        result = setConnHandleForAnchSearchStart(mHandoverConnHandle);
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static void HandleAnchorSearchStopped(gapGenericEvent_t *pGenericEvent)
*\brief         Handle anchor search stopped event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*
*\return        None
********************************************************************************************************************* */
static void HandleAnchorSearchStopped(gapGenericEvent_t *pGenericEvent)
{
    if (mAppHandoverState == gIdle_c)
    {
        removeMonitorFilter(pGenericEvent->eventData.handoverAnchorSearchStop.connectionHandle);
    }
}

/*! ********************************************************************************************************************
*\fn            static void HandleAnchorMonitorPacketContinueEvent(gapGenericEvent_t *pGenericEvent)
*\brief         Handle anchor monitor packet continue event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*
*\return        None
********************************************************************************************************************* */
static void HandleAnchorMonitorPacketContinueEvent
(
    gapGenericEvent_t *pGenericEvent
)
{
    handoverAnchorMonitorPacketContinueEvent_t *pAnchMntPktCntEvt = &pGenericEvent->eventData.handoverAnchorMonitorPacketContinue;
    uint8_t buf[gHandoverPacketMonitorMaxLen_c] = {0U};
    
    buf[0] = pAnchMntPktCntEvt->packetCounter;
    Utils_PackTwoByteValue(pAnchMntPktCntEvt->connectionHandle, &buf[1]);
    /* PDU length */
    buf[3] = pAnchMntPktCntEvt->pduSize;
    
    if (pAnchMntPktCntEvt->pPdu != NULL)
    {
        FLib_MemCpy(&buf[4], pAnchMntPktCntEvt->pPdu, pAnchMntPktCntEvt->pduSize);
        (void)MEM_BufferFree(pAnchMntPktCntEvt->pPdu);
    }
    else
    {
        /* Set length to 0 */
        buf[3] = 0U;
    }
    /* Send data to remote anchor */
    notifyRemoteDevice(gHandoverPacketContinueMonitorCommandOpCode_c, (4U + (uint16_t)(buf[3])), buf);
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleSuspendTransmitComplete(gapGenericEvent_t *pGenericEvent, appHandoverError_t *pError)
*\brief         Handle suspend transmit complete event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*\param  [out]  pError         Pointer to error status.
*
*\return        bleResult_t    Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleSuspendTransmitComplete
(
    gapGenericEvent_t *pGenericEvent,
    appHandoverError_t *pError
)
{
    bleResult_t result = gBleSuccess_c;
    
    mAppHandoverState = gContextTx_c;
    gSizeOfDataTxInOldestPacket = pGenericEvent->eventData.handoverSuspendTransmitComplete.sizeOfDataTxInOldestPacket;
    
    result = Gap_HandoverGetDataSize(mHandoverCentralDeviceId, &mHandoverDataSize);
    
    if (result == gBleSuccess_c)
    {
        mpHandoverData = MEM_BufferAlloc(mHandoverDataSize);
        
        if (mpHandoverData != NULL)
        {
            result = Gap_HandoverGetData(mHandoverCentralDeviceId, mpHandoverData);
        }
        else
        {
            result = gBleOutOfMemory_c;
            *pError = mAppHandover_OutOfMemory_c;
        }
    }
    
    return result;
}

#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1)
/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleGetCsLlContextComplete(gapGenericEvent_t *pGenericEvent)
*\brief         Handle get CS LL context complete event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*
*\return        bleResult_t    Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleGetCsLlContextComplete
(
    gapGenericEvent_t *pGenericEvent
)
{
    bleResult_t result = gBleSuccess_c;
    if(pGenericEvent->eventData.handoverGetCsLlContext.status == gBleSuccess_c)
    {
        uint8_t buf[gHandoverCsLlContextCommandLen_c] = {0U};

        FLib_MemCpy(&buf[0], &pGenericEvent->eventData.handoverGetCsLlContext.responseMask, 2U);
        buf[2] = pGenericEvent->eventData.handoverGetCsLlContext.llContextLength;
        FLib_MemCpy(&buf[3], pGenericEvent->eventData.handoverGetCsLlContext.llContext, pGenericEvent->eventData.handoverGetCsLlContext.llContextLength);
        notifyRemoteDevice(gHandoverCsLlContextCommandOpCode_c, (uint16_t)3U + pGenericEvent->eventData.handoverGetCsLlContext.llContextLength, buf);
    }
    else
    {
        result = gBleUnexpectedError_c;
    }
    return result;
}
#endif /* defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1) */

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleAnchorMonitorEvent(gapGenericEvent_t* pGenericEvent, appHandoverError_t *pError)
*\brief         Handle anchor monitor event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*\param  [out]  pError         Pointer to error status.
*
*\return        bleResult_t    Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleAnchorMonitorEvent
(
    gapGenericEvent_t* pGenericEvent,
    appHandoverError_t *pError
)
{
    bleResult_t result = gBleSuccess_c;
    *pError = mAppHandover_UnexpectedError_c;

    do
    {
        if (mAppHandoverState == gAnchorSearch_c)
        {
            if ((pGenericEvent->eventData.handoverAnchorMonitor.statusRemote & gHandoverAnchorMonitorStatusRssi_c) == 0U)
            {
                if ((pGenericEvent->eventData.handoverAnchorMonitor.ucNbReports != 1U) &&
                    (mHandoverAnchorSearchThreshold > 0U))
                {
                    mHandoverAnchorSearchThreshold--;
                    break;
                }

                *pError = mAppHandover_AnchorSearchFailedToSync_c;
                result = gBleUnexpectedError_c;
            }
            else
            {
                if (gHandoverDeviceFound == FALSE)
                {
                    gHandoverDeviceFound = TRUE;
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
                    uint8_t nvmIndex = getNvmIndexFromConnHandle(pGenericEvent->eventData.handoverAnchorMonitor.connectionHandle);
                    
                    if (nvmIndex != gInvalidNvmIndex_c)
                    {
                        uint8_t aPeerSkd[gSkdSize_c] = {0U};
                        
                        (void)AppHandover_GetPeerSkd(nvmIndex, aPeerSkd);
                        /* Set peer SKD to regenerate LL Session Key. */
                        result = Gap_HandoverSetSkd(nvmIndex, aPeerSkd);
                    }
                    else
                    {
                        result = gBleUnexpectedError_c;
                    }
                    
                    if (result == gBleSuccess_c)
                    {
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
                        /* Send pending LL data packets to be sent after handover connect.
                        In case of error free the mSrcLlPendingDataQueue queue */
                        while(MSG_QueueGetHead(&mSrcLlPendingDataQueue) != NULL)
                        {
                            uint8_t *pTxData = MSG_QueueRemoveHead(&mSrcLlPendingDataQueue);
                            
                            if (result == gBleSuccess_c)
                            {
                                result = Gap_HandoverSetLlPendingData(pGenericEvent->eventData.handoverAnchorMonitor.connectionHandle, pTxData);
                            }
                            
                            MSG_Free(pTxData);
                        }
                        if (result == gBleSuccess_c)
                        {
                            /* LL Search successful, perform connect */
                            result = Gap_HandoverConnect(pGenericEvent->eventData.handoverAnchorMonitor.connectionHandle,
                                                         mpfAppConnCb, 0U);
                        }
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
                    }
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
                
                }
            }
        }
        else if (mAppHandoverState == gIdle_c)
        {
            static uint32_t eventCount = 0U;
            handoverAnchorMonitorEvent_t *pAnchMntEvt = &pGenericEvent->eventData.handoverAnchorMonitor;
            
            if (checkMonitorFilterCounter(pAnchMntEvt->connectionHandle, pAnchMntEvt->rssiRemote, pAnchMntEvt->rssiActive))
            {
                uint8_t buf[gHandoverAnchorMonitorLen_c] = {0U};
                Utils_PackTwoByteValue(pAnchMntEvt->connectionHandle, &buf[0]);
                Utils_PackTwoByteValue(pAnchMntEvt->connEvent, &buf[2]);
                buf[4] = (uint8_t)(pAnchMntEvt->rssiRemote);
                buf[5] = pAnchMntEvt->lqiRemote;
                buf[6] = pAnchMntEvt->statusRemote;
                buf[7] = (uint8_t)(pAnchMntEvt->rssiActive);
                buf[8] = pAnchMntEvt->lqiActive;
                buf[9] = pAnchMntEvt->statusActive;
                Utils_PackFourByteValue(pAnchMntEvt->anchorClock625Us, &buf[10]);
                Utils_PackTwoByteValue(pAnchMntEvt->anchorDelay, &buf[14]);
                buf[16] = pAnchMntEvt->chIdx;
                buf[17] = pAnchMntEvt->ucNbReports;
                buf[18] = (uint8_t)getMonitorFilterAverageActiveRssi(pAnchMntEvt->connectionHandle);
                buf[19] = (uint8_t)getMonitorFilterAverageRemoteRssi(pAnchMntEvt->connectionHandle);
                /* Send data to remote anchor */
                notifyRemoteDevice(gHandoverAnchorMonitorCommandOpCode_c, gHandoverAnchorMonitorLen_c, buf);
            }
            
            if ((pAnchMntEvt->ucNbReports == 1U) && (mContinuousAnchorMonitoring == FALSE))
            {
                /* Anchor monitor complete */
                (void)anchorMonitorStop(pAnchMntEvt->connectionHandle);
            }
            
            eventCount++;
        }
        else
        {
            /* MISRA compliance */
        }
    } while(FALSE);
    
    return result;
}


/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleGetConnParamsComplete(gapGenericEvent_t *pGenericEvent, appHandoverError_t *pError)
*\brief         Handle get connection parameters complete event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*\param  [out]  pError         Pointer to error status.
*
*\return        bleResult_t    Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleGetConnParamsComplete
(
    gapGenericEvent_t *pGenericEvent,
    appHandoverError_t *pError
)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t nvmIndex = gInvalidNvmIndex_c;
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
    bool_t isBonded = FALSE;
    
    if (mAppHandoverState != gAnchorMonitorRemoteStarting_c)
    {
        result = Gap_CheckIfBonded(mHandoverCentralDeviceId, &isBonded, &nvmIndex);
        
        if ((result != gBleSuccess_c) || (isBonded == FALSE) || (nvmIndex == gInvalidNvmIndex_c))
        {
            result = gBleUnexpectedError_c;
            *pError = mAppHandover_PeerBondingDataInvalid_c;
        }
    }
#endif /* (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U)) */
    if (result == gBleSuccess_c)
    {
        deviceId_t deviceId = gInvalidDeviceId_c;
        /* Send command to start Anchor Search */
        uint8_t buf[gHandoverAnchorStartSearchCommandLen_c] = {0U};
        uint32_t timingDiffSlot = mSlotRemote - mSlotLocal;
        uint16_t timingDiffOffset = mOffsetRemote - mOffsetLocal;
        uint16_t eventCounterAdvance = 0U;
        
        buf[0] = nvmIndex;
        FLib_MemCpy(&buf[1], &pGenericEvent->eventData.getConnParams.ulTxAccCode, 4U);
        FLib_MemCpy(&buf[5], pGenericEvent->eventData.getConnParams.aCrcInitVal, 3U);
        FLib_MemCpy(&buf[8], &pGenericEvent->eventData.getConnParams.uiConnInterval, 2U);
        FLib_MemCpy(&buf[10], &pGenericEvent->eventData.getConnParams.uiSuperTO, 2U);
        FLib_MemCpy(&buf[12], &pGenericEvent->eventData.getConnParams.uiConnLatency, 2U);
        FLib_MemCpy(&buf[14], pGenericEvent->eventData.getConnParams.aChMapBm, 5U);
        buf[19] = pGenericEvent->eventData.getConnParams.ucChannelSelection;
        buf[20] = pGenericEvent->eventData.getConnParams.ucHop;
        buf[21] = pGenericEvent->eventData.getConnParams.ucUnMapChIdx;
        buf[22] = pGenericEvent->eventData.getConnParams.ucCentralSCA;
        buf[23] = pGenericEvent->eventData.getConnParams.ucRole;
        buf[24] = pGenericEvent->eventData.getConnParams.aucRemoteMasRxPHY;
        buf[25] = pGenericEvent->eventData.getConnParams.seqNum;
        FLib_MemCpy(&buf[26], &pGenericEvent->eventData.getConnParams.uiConnEvent, 2U);
        FLib_MemCpy(&buf[28], &pGenericEvent->eventData.getConnParams.ulAnchorClk, 4U);
        FLib_MemCpy(&buf[32], &pGenericEvent->eventData.getConnParams.uiAnchorDelay, 2U);
        FLib_MemCpy(&buf[34], &pGenericEvent->eventData.getConnParams.ulRxInstant, 4U);
        FLib_MemCpy(&buf[38], &eventCounterAdvance, 2U);
        FLib_MemCpy(&buf[40], &timingDiffSlot, 4U);
        FLib_MemCpy(&buf[44], &timingDiffOffset, 2U);
        buf[46] = 0U;
        buf[47] = 0U;
        buf[48] = (uint8_t)gSuspendTxMode_c;

        if (mAppHandoverState == gAnchorMonitorRemoteStarting_c)
        {
            result = Gap_GetDeviceIdFromConnHandle(pGenericEvent->eventData.getConnParams.connectionHandle, &deviceId);
            
            if (result == gBleSuccess_c)
            {
                /* Overwrite search mode */
                buf[48] = (uint8_t)(maAppMonitorData[deviceId].monitorMode);
                maAppMonitorData[deviceId].monitorConnHandle = gMonitorConnectionHandlePending;
            }
            mAppHandoverState = gIdle_c;
        }
        else
        {
            /* Default value for the Anchor Search ucNbReports parameter */
            buf[47] = gHandoverAnchorSearchIntervals_c;
        }
        
        notifyRemoteDevice(gHandoverAnchorStartSearchCommandOpCode_c, gHandoverAnchorStartSearchCommandLen_c, buf);
    }
    
    return result;
}


/*! ********************************************************************************************************************
*\fn            static void HandleHandoverConnParamUpdateEvent(gapGenericEvent_t *pGenericEvent)
*\brief         Handle handover connection parameter update event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*
*\return        None
********************************************************************************************************************* */
static void HandleHandoverConnParamUpdateEvent(gapGenericEvent_t *pGenericEvent)
{
    if (mpfAppEventCb != NULL)
    {
        mpfAppEventCb(mAppHandoverConnParamUpdate_c, &pGenericEvent->eventData.handoverConnParamUpdate);
    }
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleConnectionUpdateProcedureEvent(gapGenericEvent_t *pGenericEvent)
*\brief         Handle connection update procedure event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*
*\return        bleResult_t    Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleConnectionUpdateProcedureEvent(gapGenericEvent_t *pGenericEvent)
{
    handoverConnectionUpdateProcedureEvent_t *pConnParams = &pGenericEvent->eventData.handoverConnectionUpdateProcedure;
    uint16_t monitoredConnHandle = gInvalidConnectionHandle_c;
    deviceId_t deviceId = gInvalidDeviceId_c;
    bleResult_t status = gBleSuccess_c;
    /* Notify peer anchor only if anchor monitoring is in progress for this connection */
    status = Gap_GetDeviceIdFromConnHandle(pConnParams->connectionHandle, &deviceId);
    
    if ((status == gBleSuccess_c) && (deviceId != gInvalidDeviceId_c))
    {
        monitoredConnHandle = getMonitoredConnHandle(deviceId);
        
        if (monitoredConnHandle != gInvalidConnectionHandle_c)
        {
            /* Anchor Monitoring is started for this connection */
            uint8_t buf[gHandoverConnectionUpdateProcedureCommandLen_c] = {0U};
            Utils_PackTwoByteValue(monitoredConnHandle, &buf[0]);
            buf[2] = pConnParams->winSize;
            Utils_PackTwoByteValue(pConnParams->winOffset, &buf[3]);
            Utils_PackTwoByteValue(pConnParams->interval, &buf[5]);
            Utils_PackTwoByteValue(pConnParams->latency, &buf[7]);
            Utils_PackTwoByteValue(pConnParams->timeout, &buf[9]);
            Utils_PackTwoByteValue(pConnParams->instant, &buf[11]);
            Utils_PackTwoByteValue(pConnParams->currentEventCounter, &buf[13]);
            
            notifyRemoteDevice(gHandoverLConnectionUpdateParamsCommandOpCode_c, gHandoverConnectionUpdateProcedureCommandLen_c, buf);
        }
    }
    
    return status;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleInternalError(gapGenericEvent_t *pGenericEvent, appHandoverError_t *pError)
*\brief         Handle internal error event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*\param  [out]  pError         Pointer to error status.
*
*\return        bleResult_t    Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleInternalError
(
    gapGenericEvent_t *pGenericEvent,
    appHandoverError_t *pError
)
{
    bleResult_t result = gBleSuccess_c;
    switch (pGenericEvent->eventData.internalError.errorSource)
    {
        case gHandover_c:
        case gHandoverGetLlContext_c:
        case gGetConnParams_c:
        case gHandoverConnect_c:
        case gHandoverDisconnect_c:
        case gHandoverSetSkd_c:
        case gHandoverSetCsLlContext_c:
        case gHandoverSuspendTransmitLlProcInProgress_c:
        case gHandoverSuspendTransmitHciTx_c:
        case gHandoverResumeTransmitHciTx_c:
        case gHandoverAnchorNotifHciTx_c:
        case gHandoverAnchorSearchStartHciTx_c:
        case gHandoverAnchorSearchStopHciTx_c:
        case gHandoverTimeSyncTxHciTx_c:
        case gHandoverTimeSyncRxHciTx_c:
        case gHandoverSuspendTransmitLl_c:
        case gHandoverGetTimeLl_c:
        case gHandoverResumeTransmitLl_c:
        case gHandoverAnchorNotifLl_c:
        case gHandoverAnchorSearchStartLl_c:
        case gHandoverAnchorSearchStopLl_c:
        case gHandoverTimeSyncTxLl_c:
        case gHandoverTimeSyncRxLl_c:
        {
            result = gBleUnexpectedError_c;
        }
        break;
        case gHandoverApplyConnectionUpdateProcedure_c:
        {
            /* Abort anchor monitoring */
            result = pGenericEvent->eventData.internalError.errorCode;
            mAppHandoverState = gAnchorMonitorLocal_c;
            *pError = mAppHandover_ConnParamsUpdateFail_c;
        }
        break;
#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1)
        case gHandoverGetCsLlContext_c:
        {
            /* CS configuration not available, assume it is not supported.
            At this point the connection handover is complete. */
            result = Gap_HandoverDisconnect(mHandoverCentralDeviceId);
            mAppHandoverState = gIdle_c;
            mHandoverCentralDeviceId = gInvalidDeviceId_c;
        }
        break;
#endif /* defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1) */

        default:
        {
            ;/* Ignore error sources not related to connection handover*/
            
        }
        break;
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static void HandleLlPendingData(gapGenericEvent_t *pGenericEvent)
*\brief         Handle LL pending data event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*
*\return        None
********************************************************************************************************************* */
static void HandleLlPendingData(gapGenericEvent_t *pGenericEvent)
{
    handoverLlPendingDataIndication_t *pTxPacket = &pGenericEvent->eventData.handoverLlPendingDataIndication;
            
    if (mAppHandoverState == gContextTx_c)
    {
        if (gSizeOfDataTxInOldestPacket > 0U)
        {
            /* Remove data already transmitted */
            if (gSizeOfDataTxInOldestPacket < (pTxPacket->dataSize - gHciAclDataPacketHeaderLength_c))
            {
                uint16_t newPacketDataLen = pTxPacket->dataSize - gHciAclDataPacketHeaderLength_c - gSizeOfDataTxInOldestPacket;
                FLib_MemInPlaceCpy(&pTxPacket->pData[gHciAclDataPacketHeaderLength_c],
                                   &pTxPacket->pData[gHciAclDataPacketHeaderLength_c + gSizeOfDataTxInOldestPacket],
                                   newPacketDataLen);
                pTxPacket->dataSize -= gSizeOfDataTxInOldestPacket;
                /* Update header length */
                Utils_PackTwoByteValue(newPacketDataLen, &pTxPacket->pData[2]);
                /* Set packet boundary flag */
                pTxPacket->pData[1] |= 0x10U;
            }
            
            gSizeOfDataTxInOldestPacket = 0U;
        }
        /* Notify application of pending LL ACL data */
        notifyRemoteDevice(gHandoverLlPendingDataCommandOpCode_c, pTxPacket->dataSize, pTxPacket->pData);
    }
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleGetComplete(gapGenericEvent_t* pGenericEvent)
*\brief         Handle get complete event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*
*\return        bleResult_t    Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleGetComplete(gapGenericEvent_t* pGenericEvent)
{
    bleResult_t result = gBleSuccess_c;
    if(pGenericEvent->eventData.handoverGetData.status == gBleSuccess_c)
    {
        notifyRemoteDevice(gHandoverDataCommandOpCode_c, (uint16_t)mHandoverDataSize, (uint8_t *)mpHandoverData);
        (void)MEM_BufferFree(mpHandoverData);
        mpHandoverData = NULL;
        mHandoverDataSize = 0;
        
        result = Gap_GetConnParamsMonitoring(mHandoverCentralDeviceId, 0U);
    }
    else
    {
        result = gBleUnexpectedError_c;
    }
    return result;
}

/*! ********************************************************************************************************************
*\fn            static void HandleFreeComplete(void)
*\brief         Handle free complete event
*
*\return        None
********************************************************************************************************************* */
static void HandleFreeComplete()
{
    (void)MEM_BufferFree(mpHandoverData);
    mpHandoverData = NULL;
    mHandoverDataSize = 0;
}

/*! ********************************************************************************************************************
*\fn            static void HandleAnchorMonitorPacketEvent(gapGenericEvent_t *pGenericEvent)
*\brief         Handle anchor monitor packet event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*
*\return        None
********************************************************************************************************************* */
static void HandleAnchorMonitorPacketEvent(gapGenericEvent_t *pGenericEvent)
{
    handoverAnchorMonitorPacketEvent_t *pAnchMntPktEvt = &pGenericEvent->eventData.handoverAnchorMonitorPacket;
    
    if (checkMonitorFilterCounter(pAnchMntPktEvt->connectionHandle, 0, 0))
    {
        uint8_t buf[gHandoverPacketMonitorMaxLen_c] = {0U};
        buf[0] = pAnchMntPktEvt->packetCounter;
        Utils_PackTwoByteValue(pAnchMntPktEvt->connectionHandle, &buf[1]);
        buf[3] = pAnchMntPktEvt->statusPacket;
        buf[4] = pAnchMntPktEvt->phy;
        buf[5] = pAnchMntPktEvt->chIdx;
        buf[6] = (uint8_t)(pAnchMntPktEvt->rssiPacket);
        buf[7] = pAnchMntPktEvt->lqiPacket;
        Utils_PackTwoByteValue(pAnchMntPktEvt->connEvent, &buf[8]);
        Utils_PackTwoByteValue(pAnchMntPktEvt->anchorClock625Us, &buf[10]);
        Utils_PackTwoByteValue(pAnchMntPktEvt->anchorDelay, &buf[14]);
        buf[16] = pAnchMntPktEvt->ucNbConnIntervals;
        /* PDU length */
        buf[17] = pAnchMntPktEvt->pduSize;
        
        if (pAnchMntPktEvt->pPdu != NULL)
        {
            FLib_MemCpy(&buf[18], pAnchMntPktEvt->pPdu, pAnchMntPktEvt->pduSize);
        }
        else
        {
            /* Set length to 0 */
            buf[17] = 0U;
        }
        /* Send data to remote anchor */
        notifyRemoteDevice(gHandoverPacketMonitorCommandOpCode_c, (18U + (uint16_t)(buf[17])), buf);
    }
    
    (void)MEM_BufferFree(pAnchMntPktEvt->pPdu);
    
    if ((pAnchMntPktEvt->ucNbConnIntervals == 1U) && (mContinuousAnchorMonitoring == FALSE))
    {
        /* Anchor monitor complete */
        (void)anchorMonitorStop(pAnchMntPktEvt->connectionHandle);
    }
}

#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleSkdReportEvent(gapGenericEvent_t *pGenericEvent)
*\brief         Handle SKD report event
*
*\param  [in]   pGenericEvent  Pointer to generic event.
*
*\return        bleResult_t    Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleSkdReportEvent(gapGenericEvent_t *pGenericEvent)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t nvmIndex = gInvalidNvmIndex_c;
    bool_t isBonded = FALSE;
    
    result = Gap_CheckIfBonded(pGenericEvent->eventData.gapSkdReport.deviceId, &isBonded, &nvmIndex);
    
    if (result == gBleSuccess_c)
    {
        result = AppHandover_SetPeerSkd(nvmIndex, pGenericEvent->eventData.gapSkdReport.aSKD);
        
        if (isBonded == TRUE)
        {
            uint8_t aSetSkdCmd[gHandoverSetSkdCommandLen_c] = {0};
            /* If the device is bonded send the new SKD value to the other device involved in connection handover. */
            aSetSkdCmd[0] = nvmIndex;
            FLib_MemCpy(&aSetSkdCmd[1], pGenericEvent->eventData.gapSkdReport.aSKD, 16U);
            notifyRemoteDevice(gHandoverSetSkdCommandOpCode_c, gHandoverSetSkdCommandLen_c, aSetSkdCmd);
        }
    }
    
    return result;
}
#endif

/*! ********************************************************************************************************************
*\fn            static void HandleError(bleResult_t result, appHandoverError_t error)
*\brief         Handle error
*
*\param  [in]   result  Result of the operation.
*\param  [in]   error   Error status.
*
*\return        None
********************************************************************************************************************* */
static void HandleError(bleResult_t result, appHandoverError_t error)
{
    if (result != gBleSuccess_c)
    {
        AppHandover_Abort(TRUE, error);
    }
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleStopTimeSyncCommand(appHandoverError_t *pError)
*\brief         Handle stop time sync command
*
*\param  [out]  pError  Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleStopTimeSyncCommand(appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    
    mAppHandoverState = gContextRx_c;
    result = AppHandover_TimeSyncTransmit(gTimeSyncDisable_c);
    
    return result;
}

#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1)
/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleCsLlContextCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle CS LL context command
*
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleCsLlContextCommand(uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t mask;
    uint8_t length;

    mask = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    length = *pCmdData;
    pCmdData++;
    result = Gap_HandoverSetCsLlContext(mHandoverCentralDeviceId,
                                        mask, length, pCmdData);
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleCsContextCompletedCommand(appHandoverError_t *pError)
*\brief         Handle CS context completed command
*
*\param  [out]  pError  Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleCsContextCompletedCommand(appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    
    /* Connection was handed over to the other anchor */
    result = Gap_HandoverDisconnect(mHandoverCentralDeviceId);
    mAppHandoverState = gIdle_c;
    mHandoverCentralDeviceId = gInvalidDeviceId_c;
    
    return result;
}
#endif

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleLlPendingDataCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle LL pending data command
*
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleLlPendingDataCommand(uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t *pMsg = NULL;
    uint16_t msgSize = Utils_ExtractTwoByteValue(&pCmdData[2]) + gHciAclDataPacketHeaderLength_c;
    
    pMsg = MSG_Alloc(msgSize);
    
    if (pMsg != NULL)
    {
        FLib_MemCpy(pMsg, pCmdData, msgSize);
        (void)MSG_QueueAddTail(&mSrcLlPendingDataQueue, pMsg);
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleDataCommand(uint32_t cmdLen, uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle data command
*
*\param  [in]   cmdLen    Command data length.
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleDataCommand(uint32_t cmdLen, uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    
    if (mpHandoverData != NULL)
    {
        result = gBleInvalidState_c;
    }
    else
    {
        mpHandoverData = MEM_BufferAlloc(cmdLen);
        
        if (mpHandoverData != NULL)
        {
            FLib_MemCpy(mpHandoverData, pCmdData, cmdLen);
            /* Set the data */
            result = Gap_HandoverSetData(mpHandoverData);
        }
        else
        {
            result = gBleOutOfMemory_c;
            *pError = mAppHandover_OutOfMemory_c;
        }
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleAnchorStartSearchCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle anchor start search command
*
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleAnchorStartSearchCommand(uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    gapHandoverAnchorSearchStartParams_t searchParams;
    union {
        uint8_t                       mode8;
        bleHandoverAnchorSearchMode_t mode;
    } temp = {0U};
        
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
    uint8_t peerNvmIndex = gInvalidNvmIndex_c;

    peerNvmIndex = *pCmdData;
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
    pCmdData++;
    FLib_MemCpy(searchParams.accessAddress, pCmdData, 4U);
    pCmdData = &pCmdData[4];
    FLib_MemCpy(searchParams.crcInit, pCmdData, 3U);
    pCmdData = &pCmdData[3];
    searchParams.connInterval = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    searchParams.supervisionTimeout = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    searchParams.latency = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    FLib_MemCpy(searchParams.channelMap, pCmdData, 5U);
    pCmdData = &pCmdData[5];
    searchParams.hopAlgo2 = *pCmdData;
    pCmdData++;
    searchParams.hopIncrement = *pCmdData;
    pCmdData++;
    searchParams.unmappedChannelIndex = *pCmdData;
    pCmdData++;
    searchParams.centralSca = *pCmdData;
    pCmdData++;
    searchParams.role = *pCmdData;
    pCmdData++;
    searchParams.centralPhy = *pCmdData;
    pCmdData++;
    searchParams.seqNum = *pCmdData;
    pCmdData++;
    searchParams.eventCounter = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    searchParams.startTime625 = Utils_ExtractFourByteValue(pCmdData);
    pCmdData = &pCmdData[4];
    searchParams.startTimeOffset = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    searchParams.lastRxInstant = Utils_ExtractFourByteValue(pCmdData);
    pCmdData = &pCmdData[4];
    searchParams.uiEventCounterAdvance = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    searchParams.timingDiffSlot = Utils_ExtractFourByteValue(pCmdData);
    pCmdData = &pCmdData[4];
    searchParams.timingDiffOffset = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    searchParams.timeout = *pCmdData;
    pCmdData++;
    searchParams.ucNbReports = *pCmdData;
    pCmdData++;
    temp.mode8 = *pCmdData;
    searchParams.mode = temp.mode;
    
    /* Do not start anchor monitoring if connection handover is in progress. */
    if ((searchParams.mode == gRssiSniffingMode_c) || (searchParams.mode == gPacketMode_c))
    {
        if (mAppHandoverState == gContextRx_c)
        {
            mAppHandoverState = gAnchorMonitorLocal_c;
            if (searchParams.ucNbReports == 0U)
            {
                mContinuousAnchorMonitoring = TRUE;
            }
            else
            {
                mContinuousAnchorMonitoring = FALSE;
            }
            
            result = Gap_HandoverAnchorSearchStart(&searchParams);
        }
        else
        {
            result = gBleInvalidParameter_c;
        }
    }
    else
    {
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
        result = setNvmIndexForAnchSearchStart(peerNvmIndex);
        
        if (result == gBleSuccess_c)
        {
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
            mHandoverAnchorSearchThreshold = gHandoverAnchorSearchThreshold_c;
            result = Gap_HandoverAnchorSearchStart(&searchParams);
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
        }
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleAnchMonStartedCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle anchor monitor started command
*
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleAnchMonStartedCommand(uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    
    for (uint8_t i = 0U; i < (uint8_t)gAppMaxConnections_c; i++)
    {
        if (maAppMonitorData[i].monitorConnHandle == gMonitorConnectionHandlePending)
        {
            maAppMonitorData[i].monitorConnHandle = Utils_ExtractTwoByteValue(pCmdData);
        }
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleAnchorMonitorCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle anchor monitor command
*
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleAnchorMonitorCommand(uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    appHandoverAnchorMonitorEvent_t appAnchMntEvt = {0U};
    
    appAnchMntEvt.anchorMntEvt.connectionHandle = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    appAnchMntEvt.anchorMntEvt.connEvent = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    appAnchMntEvt.anchorMntEvt.rssiRemote = (int8_t)*pCmdData++;
    appAnchMntEvt.anchorMntEvt.lqiRemote = *pCmdData++;
    appAnchMntEvt.anchorMntEvt.statusRemote = *pCmdData++;
    appAnchMntEvt.anchorMntEvt.rssiActive = (int8_t)*pCmdData++;
    appAnchMntEvt.anchorMntEvt.lqiActive = *pCmdData++;
    appAnchMntEvt.anchorMntEvt.statusActive = *pCmdData++;
    appAnchMntEvt.anchorMntEvt.anchorClock625Us = Utils_BeExtractFourByteValue(pCmdData);
    pCmdData = &pCmdData[4];
    appAnchMntEvt.anchorMntEvt.anchorDelay = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    appAnchMntEvt.anchorMntEvt.chIdx = *pCmdData++;
    appAnchMntEvt.anchorMntEvt.ucNbReports = *pCmdData++;
    appAnchMntEvt.deviceId = getMonitoredDeviceId(appAnchMntEvt.anchorMntEvt.connectionHandle);
    appAnchMntEvt.rssiActiveAverage = (int8_t)*pCmdData++;
    appAnchMntEvt.rssiRemoteAverage = (int8_t)*pCmdData++;
    
    if (mpfAppEventCb != NULL)
    {
        mpfAppEventCb(mAppHandoverAnchorMonitor_c, &appAnchMntEvt);
    }
    
    if (appAnchMntEvt.anchorMntEvt.ucNbReports == 1U)
    {
        /* Anchor monitoring complete */
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandlePacketMonitorCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle packet monitor command
*
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandlePacketMonitorCommand(uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    appHandoverAnchorMonitorPacketEvent_t  appPktMntEvt = {0U};
    
    appPktMntEvt.pktMntEvt.packetCounter = *pCmdData;
    pCmdData = &pCmdData[1];
    appPktMntEvt.pktMntEvt.connectionHandle = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    appPktMntEvt.pktMntEvt.statusPacket = *pCmdData;
    pCmdData = &pCmdData[1];
    appPktMntEvt.pktMntEvt.phy = *pCmdData;
    pCmdData = &pCmdData[1];
    appPktMntEvt.pktMntEvt.chIdx = *pCmdData;
    pCmdData = &pCmdData[1];
    appPktMntEvt.pktMntEvt.rssiPacket = (int8_t)*pCmdData;
    pCmdData = &pCmdData[1];
    appPktMntEvt.pktMntEvt.lqiPacket = *pCmdData;
    pCmdData = &pCmdData[1];
    appPktMntEvt.pktMntEvt.connEvent = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    appPktMntEvt.pktMntEvt.anchorClock625Us = Utils_BeExtractFourByteValue(pCmdData);
    pCmdData = &pCmdData[4];
    appPktMntEvt.pktMntEvt.anchorDelay = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    appPktMntEvt.pktMntEvt.ucNbConnIntervals = *pCmdData;
    pCmdData = &pCmdData[1];
    appPktMntEvt.pktMntEvt.pduSize = *pCmdData;
    pCmdData = &pCmdData[1];
    appPktMntEvt.deviceId = getMonitoredDeviceId(appPktMntEvt.pktMntEvt.connectionHandle);
    
    if (appPktMntEvt.pktMntEvt.pduSize != 0U)
    {
        appPktMntEvt.pktMntEvt.pPdu = MEM_BufferAlloc(appPktMntEvt.pktMntEvt.pduSize);
    }
    
    if (appPktMntEvt.pktMntEvt.pPdu != NULL)
    {
        FLib_MemCpy(appPktMntEvt.pktMntEvt.pPdu, pCmdData, appPktMntEvt.pktMntEvt.pduSize);
    }
    
    if (mpfAppEventCb != NULL)
    {
        mpfAppEventCb(mAppHandoverPacketMonitor_c, &appPktMntEvt);
    }
    
    if (appPktMntEvt.pktMntEvt.ucNbConnIntervals == 1U)
    {
        /* Anchor monitoring complete */
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandlePacketContinueMonitorCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle packet continue monitor command
*
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandlePacketContinueMonitorCommand(uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    appHandoverAnchorMonitorPacketContinueEvent_t appPacketContinueMntEvt = {0U};
    
    appPacketContinueMntEvt.pktMntCntEvt.packetCounter = *pCmdData;
    pCmdData = &pCmdData[1];
    appPacketContinueMntEvt.pktMntCntEvt.connectionHandle = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    appPacketContinueMntEvt.pktMntCntEvt.pduSize = *pCmdData;
    pCmdData = &pCmdData[1];
    appPacketContinueMntEvt.deviceId = getMonitoredDeviceId(appPacketContinueMntEvt.pktMntCntEvt.connectionHandle);
    
    if (appPacketContinueMntEvt.pktMntCntEvt.pduSize != 0U)
    {
        appPacketContinueMntEvt.pktMntCntEvt.pPdu = MEM_BufferAlloc(appPacketContinueMntEvt.pktMntCntEvt.pduSize);
    }
    
    if (appPacketContinueMntEvt.pktMntCntEvt.pPdu != NULL)
    {
        FLib_MemCpy(appPacketContinueMntEvt.pktMntCntEvt.pPdu, pCmdData, appPacketContinueMntEvt.pktMntCntEvt.pduSize);
    }
    
    if (mpfAppEventCb != NULL)
    {
        mpfAppEventCb(mAppHandoverPacketContinueMonitor_c, &appPacketContinueMntEvt);
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleStopAnchorMonitorCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle stop anchor monitor command
*
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleStopAnchorMonitorCommand(uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t connectionHandle = Utils_ExtractTwoByteValue(pCmdData);
    result = anchorMonitorStop(connectionHandle);
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleAnchMonStoppedCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle anchor monitor stopped command
*
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleAnchMonStoppedCommand(uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    bool_t anchMonInProgress = FALSE;
    uint16_t connectionHandle = Utils_ExtractTwoByteValue(pCmdData);
    
    for (uint8_t i = 0U; i < (uint8_t)gAppMaxConnections_c; i++)
    {
        if (maAppMonitorData[i].monitorConnHandle == connectionHandle)
        {
            maAppMonitorData[i].monitorMode = gSuspendTxMode_c;
            maAppMonitorData[i].monitorConnHandle = gInvalidConnectionHandle_c;
        }
        else if (maAppMonitorData[i].monitorConnHandle != gInvalidConnectionHandle_c)
        {
            anchMonInProgress = TRUE;
        }
        else
        {
            continue;
        }
    }
        
    if (anchMonInProgress == FALSE)
    {
        mAppHandoverState = gIdle_c;
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleInformConnectCommand(appHandoverError_t *pError)
*\brief         Handle inform connect command
*
*\param  [out]  pError  Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleInformConnectCommand(appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    
#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1)
    result = Gap_HandoverGetCsLlContext(mHandoverCentralDeviceId);
#else
    /* Connection was handed over to the other anchor */
    result = Gap_HandoverDisconnect(mHandoverCentralDeviceId);
    mAppHandoverState = gIdle_c;
    mHandoverCentralDeviceId = gInvalidDeviceId_c;
#endif
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleInformFailureCommand(appHandoverError_t *pError)
*\brief         Handle inform failure command
*
*\param  [out]  pError  Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleInformFailureCommand(appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    
    /* Error reported by the other device, abort connection handover. */
    AppHandover_Abort(FALSE, mAppHandover_UnexpectedError_c);
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleStartTimeSyncCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle start time sync command
*
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleStartTimeSyncCommand(uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    bool_t bTimeSyncForHandover = (bool_t)*pCmdData;
    
    /* Request to start time sync received, start handover time sync. */
    result = AppHandover_TimeSyncTransmit(gTimeSyncEnable_c);
    
    if (result != gBleSuccess_c)
    {
        *pError = mAppHandover_TimeSyncTx_c;
    }
    
    if ((mpfAppEventCb != NULL) && (result == gBleSuccess_c))
    {
        mpfAppEventCb(mAppHandover_TimeSyncStarted_c, &bTimeSyncForHandover);
    }
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleSetSkdCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle set SKD command
*
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleSetSkdCommand(uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    
    /* A new peer SKD value has been generated. */
    uint8_t nvmIndex = *pCmdData;
    pCmdData++;
    result = AppHandover_SetPeerSkd(nvmIndex, pCmdData);
    
    return result;
}

/*! ********************************************************************************************************************
*\fn            static bleResult_t HandleConnectionUpdateParamsCommand(uint8_t *pCmdData, appHandoverError_t *pError)
*\brief         Handle connection update params command
*
*\param  [in]   pCmdData  Pointer to command data.
*\param  [out]  pError    Pointer to error status.
*
*\return        bleResult_t  Status of the operation.
********************************************************************************************************************* */
static bleResult_t HandleConnectionUpdateParamsCommand(uint8_t *pCmdData, appHandoverError_t *pError)
{
    bleResult_t result = gBleSuccess_c;
    gapHandoverApplyConnectionUpdateProcedure_t connParams = {0U};
    
    connParams.connHandle = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    connParams.winSize = *pCmdData;
    pCmdData = &pCmdData[1];
    connParams.winOffset = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    connParams.interval = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    connParams.latency = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    connParams.timeout = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    connParams.instant = Utils_ExtractTwoByteValue(pCmdData);
    pCmdData = &pCmdData[2];
    connParams.currentEventCounter = Utils_ExtractTwoByteValue(pCmdData);
    /* Save connection handle for error handling */
    mHandoverConnHandle = connParams.connHandle;
    
    result = Gap_HandoverApplyConnectionUpdateProcedure(&connParams);
    
    if (result != gBleSuccess_c)
    {
        /* Abort anchor monitoring */
        mAppHandoverState = gAnchorMonitorLocal_c;
        *pError = mAppHandover_ConnParamsUpdateFail_c;
    }
    
    return result;
}

#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1) */