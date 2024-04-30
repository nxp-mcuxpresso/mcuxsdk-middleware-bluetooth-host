/*! *********************************************************************************
* \addtogroup FSCI_BLE
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2022-2024 NXP
*
*
* \file
*
* This is a source file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "fsci_ble_gap_handover_types.h"
#include "gap_handover_interface.h"
#include "fsci_ble_gap.h"
#include "fsci_ble_gap_handover.h"

#if gFsciBleHost_d
    #include "host_ble.h"
    #include "FsciCommands.h"
#endif /* gFsciBleHost_d */

#if gFsciIncluded_c && gFsciBleGapHandoverLayerEnabled_d

/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

#define fsciBleGapHandoverCallApiFunction(apiFunction)          bleResult_t result = (apiFunction);             \
                                                                fsciBleGapHandoverStatusMonitor(result)

#define fsciBleGapHandoverMonitorOutParams(functionId, ...)     if(gBleSuccess_c == result)                     \
                                                                {                                               \
                                                                    FsciEvtMonitor(functionId, __VA_ARGS__);    \
                                                                }

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/


/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  Handover connection event callback.
*
* \param[in]    deviceId              Connection peer device ID.
* \param[in]    pConnectionEvent      Pointer to connection event data.
*
********************************************************************************** */
static void fsciBleGapHandoverConnectionCallback
(
    deviceId_t                  deviceId,
    gapConnectionEvent_t        *pConnectionEvent
);

/************************************************************************************
*************************************************************************************
* Public functions prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

#if gFsciBleTest_d
    /* Indicates if FSCI for GAP Handover is enabled or not */
    static bool_t bFsciBleGapHandoverEnabled            = FALSE;
#endif /* gFsciBleTest_d */

static uint8_t  maHandoverData[gHandoverMaxPayloadLen_c];
static uint8_t  maHandoverCsLlContext[gHandoverMaxPayloadLen_c];
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  Calls the Gap Handover function associated with the operation code received
*         over UART. The Gap Handover function parameters are extracted from the
*         received FSCI payload.
*
* \param[in]    pData               Packet (containing FSCI header and FSCI
*                                   payload) received over UART.
* \param[in]    param               Pointer given when this function is
*                                   registered in FSCI.
* \param[in]    fsciInterfaceId     FSCI interface on which the packet was
*                                   received.
*
********************************************************************************** */
void fsciBleGapHandoverHandler(void* pData, void* param, uint32_t fsciInterfaceId)
{
    clientPacket_t *pClientPacket   = (clientPacket_t*)pData;
    uint8_t        *pBuffer         = &pClientPacket->structured.payload[0];

#if gFsciBleTest_d
    /* Verify if the command is Mode Select */
    if(gBleGapHandoverModeSelectOpCode_c == (fsciBleGapHandoverOpCode_t)pClientPacket->structured.header.opCode)
    {
        /* Get the new FSCI for GAP Handover status (enabled or disabled) */
        fsciBleGetBoolValueFromBuffer(bFsciBleGapHandoverEnabled, pBuffer);

        /* Return status */
        fsciBleGapHandoverStatusMonitor(gBleSuccess_c);
    }
    else
    {
        /* Verify if FSCI is enabled for this layer */
        if(TRUE == bFsciBleGapHandoverEnabled)
        {
#endif /* gFsciBleTest_d */
            /* Select the GAP Handover function to be called (using the FSCI opcode) */
            switch(pClientPacket->structured.header.opCode)
            {
#if gFsciBleBBox_d || gFsciBleTest_d
                case (uint8_t)gBleGapHandoverCmdGetDataSizeOpCode_c:
                    {
                        deviceId_t  deviceId;
                        uint32_t    outDataSize;

                        /* Get deviceId from buffer */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

                        fsciBleGapHandoverCallApiFunction(Gap_HandoverGetDataSize(deviceId, &outDataSize));
                        fsciBleGapHandoverMonitorOutParams(GetDataSize, &outDataSize);
                    }
                    break;
                    
                case (uint8_t)gBleGapHandoverCmdGetDataOpCode_c:
                    {
                        deviceId_t      deviceId;

                        /* Get deviceId from buffer */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        
                        fsciBleGapHandoverCallApiFunction(Gap_HandoverGetData(deviceId, (uint32_t *)maHandoverData));
                    }
                    break;
                    
                case gBleGapHandoverCmdSetDataOpCode_c:
                    {
                        /* Get Handover Data from buffer */
                        fsciBleGetArrayFromBuffer(maHandoverData, pBuffer, gHandoverMaxPayloadLen_c);

                        fsciBleGapHandoverCallApiFunction(Gap_HandoverSetData((uint32_t *)maHandoverData));
                    }
                    break;

                case gBleGapHandoverCmdGetCsLlContextOpCode_c:
                    {
                        deviceId_t      deviceId;

                        /* Get deviceId from buffer */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

                        fsciBleGapHandoverCallApiFunction(Gap_HandoverGetCsLlContext(deviceId));
                    }
                    break;

                case gBleGapHandoverCmdSetCsLlContextOpCode_c:
                    {
                        deviceId_t      deviceId;
                        uint16_t        mask;
                        uint8_t         contextLength;

                        /* Get deviceId from buffer */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        fsciBleGetUint16ValueFromBuffer(mask, pBuffer);
                        fsciBleGetUint8ValueFromBuffer(contextLength, pBuffer);
                        fsciBleGetArrayFromBuffer(maHandoverCsLlContext, pBuffer, contextLength);

                        fsciBleGapHandoverCallApiFunction(Gap_HandoverSetCsLlContext(deviceId, mask, contextLength, maHandoverCsLlContext));
                    }
                    break;
                    
                case gBleGapHandoverCmdGetTimeOpCode_c:
                    {
                        fsciBleGapHandoverCallApiFunction(Gap_HandoverGetTime());
                    }
                    break;
                    
                case gBleGapHandoverCmdSuspendTransmitOpCode_c:
                    {
                        deviceId_t                                  deviceId;
                        bleHandoverSuspendTransmitMode_t            mode;
                        uint16_t                                    eventCounter;
                        uint8_t                                     noOfConnIntervals;
                        
                        /* Extract fields from buffer */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        fsciBleGetEnumValueFromBuffer(mode, pBuffer, bleHandoverSuspendTransmitMode_t);
                        fsciBleGetUint16ValueFromBuffer(eventCounter, pBuffer);
                        fsciBleGetUint8ValueFromBuffer(noOfConnIntervals, pBuffer);
                        
                        fsciBleGapHandoverCallApiFunction(Gap_HandoverSuspendTransmit(deviceId,
                                                                                      mode,
                                                                                      eventCounter,
                                                                                      noOfConnIntervals));
                    }
                    break;
                    
                case gBleGapHandoverCmdResumeTransmitOpCode_c:
                    {
                        deviceId_t deviceId;
                        
                        /* Extract deviceId from buffer */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        
                        fsciBleGapHandoverCallApiFunction(Gap_HandoverResumeTransmit(deviceId));
                    }
                    break;
                    
                case gBleGapHandoverCmdAnchorNotificationOpCode_c:
                    {
                        deviceId_t                                  deviceId;
                        bleHandoverAnchorNotificationEnable_t       enable;
                        uint8_t                                     noOfReports;
                        
                        /* Extract fields from buffer */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        fsciBleGetEnumValueFromBuffer(enable, pBuffer, bleHandoverAnchorNotificationEnable_t);
                        fsciBleGetUint8ValueFromBuffer(noOfReports, pBuffer);
                        
                        fsciBleGapHandoverCallApiFunction(Gap_HandoverAnchorNotification(deviceId, enable, noOfReports));
                    }
                    break;
                    
                case gBleGapHandoverCmdAnchorSearchStartOpCode_c:
                    {
                        gapHandoverAnchorSearchStartParams_t anchorSearchStartParams;
                        
                        fsciBleGapHandoverGetAnchorSearchStartParametersFromBuffer(&anchorSearchStartParams, &pBuffer);

                        fsciBleGapHandoverCallApiFunction(Gap_HandoverAnchorSearchStart(&anchorSearchStartParams));
                    }
                    break;

                case gBleGapHandoverCmdAnchorSearchStopOpCode_c:
                    {
                        uint16_t connHandle = 0U;

                        fsciBleGetUint16ValueFromBuffer(connHandle, pBuffer);

                        fsciBleGapHandoverCallApiFunction(Gap_HandoverAnchorSearchStop(connHandle));
                    }
                    break;
                    
                case gBleGapHandoverCmdConnectOpCode_c:
                    {
                        uint16_t connHandle;
                        uint8_t  anchorNotification;
                        
                        /* Extract fields from buffer */
                        fsciBleGetUint16ValueFromBuffer(connHandle, pBuffer);
                        fsciBleGetUint8ValueFromBuffer(anchorNotification, pBuffer);
                        
                        fsciBleGapHandoverCallApiFunction(Gap_HandoverConnect(connHandle, fsciBleGapHandoverConnectionCallback, anchorNotification));
                    }
                    break;
                    
                case gBleGapHandoverCmdDisconnectOpCode_c:
                    {
                        deviceId_t deviceId;
                        
                        /* Extract deviceId from buffer */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        
                        fsciBleGapHandoverCallApiFunction(Gap_HandoverDisconnect(deviceId));
                    }
                    break;
                    
                case gBleGapHandoverCmdInitOpCode_c:
                    {
                        fsciBleGapHandoverCallApiFunction(Gap_HandoverInit());
                    }
                    break;

                case gBleGapHandoverCmdTimeSyncTransmitOpCode_c:
                    {
                        gapHandoverTimeSyncTransmitParams_t tsTransmitParams;

                        fsciBleGapHandoverGetTimeSyncTransmitParametersFromBuffer(&tsTransmitParams, &pBuffer);

                        fsciBleGapHandoverCallApiFunction(Gap_HandoverTimeSyncTransmit(&tsTransmitParams));
                    }
                    break;

                case gBleGapHandoverCmdTimeSyncReceiveOpCode_c:
                    {
                        gapHandoverTimeSyncReceiveParams_t tsReceiveParams;

                        fsciBleGapHandoverGetTimeSyncReceiveParametersFromBuffer(&tsReceiveParams, &pBuffer);

                        fsciBleGapHandoverCallApiFunction(Gap_HandoverTimeSyncReceive(&tsReceiveParams));
                    }
                    break;
                    
                case gBleGapHandoverUpdateConnParamsOpCode_c:
                    {
                        gapHandoverUpdateConnParams_t connParams = {0U};
                        
                        /* Extract fields from buffer */
                        fsciBleGetUint16ValueFromBuffer(connParams.connHandle, pBuffer);
                        fsciBleGetUint8ValueFromBuffer(connParams.status, pBuffer);
                        fsciBleGetUint8ValueFromBuffer(connParams.txRxPhy, pBuffer);
                        fsciBleGetUint8ValueFromBuffer(connParams.seqNum, pBuffer);
                        fsciBleGetArrayFromBuffer(connParams.aChannelMap, pBuffer, gcBleChannelMapSize_c);
                        fsciBleGetUint16ValueFromBuffer(connParams.eventCounter, pBuffer);
                        
                        fsciBleGapHandoverCallApiFunction(Gap_HandoverUpdateConnParams(&connParams));
                    }
                    break;
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

                default:
                    {
                        /* Unknown FSCI opcode */
                        fsciBleError(gFsciUnknownOpcode_c, fsciInterfaceId);
                    }
                    break;
            }
#if gFsciBleTest_d
        }
        else
        {
            /* FSCI SAP disabled */
            fsciBleError(gFsciSAPDisabled_c, fsciInterfaceId);
        }
    }
#endif /* gFsciBleTest_d */

    (void)MEM_BufferFree(pData);
}

#if gFsciBleBBox_d || gFsciBleTest_d
/*! *********************************************************************************
* \brief  Creates the FSCI packet which contains the status of the last executed
*         Gap Handover command and sends it over UART.
*
* \param[in]    result      Status of the last executed Gap Handover command.
*
********************************************************************************** */
void fsciBleGapHandoverStatusMonitor(bleResult_t result)
{
#if gFsciBleTest_d
    /* If GAP Handover is disabled the status must be not monitored */
    if(FALSE == bFsciBleGapHandoverEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Send status over UART */
    fsciBleStatusMonitor(gFsciBleGapHandoverOpcodeGroup_c, (uint8_t)gBleGapHandoverStatusOpCode_c, result);
}

/*! *********************************************************************************
* \brief  Gap_HandoverGetDataSize command out parameters monitoring function.
*
* \param[in]    pDataSize    Pointer to the Handover Data size.
*
********************************************************************************** */
void fsciBleGapHandoverGetDataSizeEvtMonitor
(
    const uint32_t   *pDataSize
)
{
    clientPacketStructured_t *pClientPacket;
    uint8_t                  *pBuffer;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapHandoverEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapHandoverAllocFsciPacket((uint8_t)gBleGapHandoverEvtGetDataSizeOpCode_c, sizeof(uint32_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromUint32Value(*pDataSize, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  Generic event monitor for Handover.
*
* \param[in]    pGenericEvent Pointer to the generic event data.
*
********************************************************************************** */
void fsciBleGapHandoverGenericEvtMonitor(gapGenericEvent_t* pGenericEvent)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    fsciBleGapHandoverOpCode_t  opCode;
    bool_t                      earlyReturn = FALSE;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if ((FALSE == bFsciBleGapHandoverEnabled) &&
        (gInitializationComplete_c != pGenericEvent->eventType))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Get FSCI opCode */
    switch(pGenericEvent->eventType)
    {
        case gHandoverGetComplete_c:
            {
                opCode = gBleGapHandoverEvtGetCompleteOpCode_c;
            }
            break;

        case gHandoverSetComplete_c:
            {
                opCode = gBleGapHandoverEvtSetCompleteOpCode_c;
            }
            break;

        case gHandoverGetCsLlContextComplete_c:
            {
                opCode = gBleGapHandoverEvtGetCsLlContextCompleteOpCode_c;
            }
            break;

        case gHandoverSetCsLlContextComplete_c:
            {
                opCode = gBleGapHandoverEvtSetCsLlContextCompleteOpCode_c;
            }
            break;

        case gHandoverGetTime_c:
            {
                opCode = gBleGapHandoverEvtGetTimeOpCode_c;
            }
            break;

        case gHandoverSuspendTransmitComplete_c:
            {
                opCode = gBleGapHandoverEvtSuspendTransmitCompleteOpCode_c;
            }
            break;

        case gHandoverResumeTransmitComplete_c:
            {
                opCode = gBleGapHandoverEvtResumeTransmitCompleteOpCode_c;
            }
            break;

        case gHandoverAnchorNotificationStateChanged_c:
            {
                opCode = gBleGapHandoverEvtAnchorNotificationStateChangedOpCode_c;
            }
            break;

        case gHandoverAnchorSearchStarted_c:
            {
                opCode = gBleGapHandoverEvtAnchorSearchStartedOpCode_c;
            }
            break;

        case gHandoverAnchorSearchStopped_c:
            {
                opCode = gBleGapHandoverEvtAnchorSearchStoppedOpCode_c;
            }
            break;

        case gHandoverAnchorMonitorEvent_c:
            {
                opCode = gBleGapHandoverEvtAnchorMonitorOpCode_c;
            }
            break;

        case gHandoverAnchorMonitorPacketEvent_c:
            {
                opCode = gBleGapHandoverEvtAnchorMonitorPacketOpCode_c;
            }
            break;

        case gHandoverAnchorMonitorPacketContinueEvent_c:
            {
                opCode = gBleGapHandoverEvtAnchorMonitorPacketContinueOpCode_c;
            }
            break;

        case gHandoverTimeSyncTransmitStateChanged_c:
            {
                opCode = gBleGapHandoverTimeSyncTransmitStateChangedOpCode_c;
            }
            break;

        case gHandoverTimeSyncReceiveComplete_c:
            {
                opCode = gBleGapHandoverTimeSyncReceiveCompleteOpCode_c;
            }
            break;

        case gHandoverTimeSyncEvent_c:
            {
                opCode = gBleGapHandoverEvtTimeSyncEventOpCode_c;
            }
            break;

        case gHandoverConnParamUpdateEvent_c:
            {
                opCode = gBleGapHandoverEvtConnParamUpdateOpCode_c;
            }
            break;

        case gHandoverUpdateConnParamsComplete_c:
            {
                opCode = gBleGapHandoverEvtUpdateConnParamsCompleteOpCode_c;
            }
            break;

        default:
            {
                /* Unknown event type */
                fsciBleError(gFsciError_c, fsciBleInterfaceId);
                earlyReturn = TRUE;
                break;
            }
    }

    if(!earlyReturn)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleGapHandoverAllocFsciPacket((uint8_t)opCode,
                                                  fsciBleGapHandoverGetGenericEventBufferSize(pGenericEvent));

        if(NULL == pClientPacket)
        {
            return;
        }

        pBuffer = &pClientPacket->payload[0];

        /* Set event parameters in the buffer */
        fsciBleGapHandoverGetBufferFromGenericEvent(pGenericEvent, &pBuffer);

        /* Transmit the packet over UART */
        fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
    }
}

/*! *********************************************************************************
* \brief  Handover connection event monitoring function.
*
* \param[in]    deviceId           Connection peer ID.
* \param[in]    pConnectionEvent   Pointer to the connection event data.
*
********************************************************************************** */
void fsciBleGapHandoverConnectionEvtMonitor(deviceId_t deviceId, gapConnectionEvent_t* pConnectionEvent)
{
    clientPacketStructured_t    *pClientPacket;
    uint8_t                     *pBuffer;
    fsciBleGapHandoverOpCode_t  opCode;
    bool_t                      earlyReturn = FALSE;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapHandoverEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Get FSCI opCode */
    switch(pConnectionEvent->eventType)
    {
        case gConnEvtHandoverConnected_c:
            {
                opCode = gBleGapHandoverEvtConnectedOpCode_c;
            }
            break;
            
        case gHandoverDisconnected_c:
            {
                opCode = gBleGapHandoverEvtDisconnectedOpCode_c;
            }
            break;
      
        default:
            {
                /* Unknown event type */
                earlyReturn = TRUE;
            }
            break;
    }

    if(!earlyReturn)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleGapHandoverAllocFsciPacket((uint8_t)opCode,
                                                  fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                  fsciBleGapHandoverGetConnectionEventBufferSize(pConnectionEvent));

        if(NULL == pClientPacket)
        {
            return;
        }

        pBuffer = &pClientPacket->payload[0];

        /* Set event parameters in the buffer */
        fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
        fsciBleGapHandoverGetBufferFromConnectionEvent(pConnectionEvent, &pBuffer);

        /* Transmit the packet over UART */
        fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
    }
}
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  Handover connection event callback.
*
* \param[in]    deviceId              Connection peer device ID.
* \param[in]    pConnectionEvent      Pointer to connection event data.
*
********************************************************************************** */
static void fsciBleGapHandoverConnectionCallback(deviceId_t deviceId, gapConnectionEvent_t* pConnectionEvent)
{
    /* Must also call the regular Conn Evt monitor in case it was never set */
    fsciBleGapConnectionEvtMonitor(deviceId, pConnectionEvent);
    fsciBleGapHandoverConnectionEvtMonitor(deviceId, pConnectionEvent);
}

#endif /* gFsciIncluded_c && gFsciBleGapHandoverLayerEnabled_d */

/*! *********************************************************************************
* @}
********************************************************************************** */
