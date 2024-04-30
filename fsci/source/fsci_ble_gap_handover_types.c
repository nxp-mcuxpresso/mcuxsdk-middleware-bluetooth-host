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
#include "ble_general.h"
#include "gap_handover_types.h"

#if gFsciIncluded_c && gFsciBleGapHandoverLayerEnabled_d

/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  Places Anchor Monitor event parameters into a buffer.
*
* \param[in]       pHandoverAnchorMonitorEventParams  Pointer to the params structure.
* \param[in,out]   ppBuffer                           Double pointer to the buffer.
*
********************************************************************************** */
static void fsciBleGapHandoverGetBufferFromAnchorMonitorEventParams
(
    handoverAnchorMonitorEvent_t    *pHandoverAnchorMonitorEventParams,
    uint8_t                         **ppBuffer
);

/*! *********************************************************************************
* \brief  Places Anchor Monitor Packet event parameters into a buffer.
*
* \param[in]       pHandoverAnchorMonitorPacketEventParams  Pointer to the params structure.
* \param[in,out]   ppBuffer                           Double pointer to the buffer.
*
********************************************************************************** */
static void fsciBleGapHandoverGetBufferFromAnchorMonitorPacketEventParams
(
    handoverAnchorMonitorPacketEvent_t  *pHandoverAnchorMonitorPacketEventParams,
    uint8_t                             **ppBuffer
);

/*! *********************************************************************************
* \brief  Places Anchor Monitor Packet Continue event parameters into a buffer.
*
* \param[in]       pHandoverAnchorMonitorPacketContinueEventParams  Pointer to the params structure.
* \param[in,out]   ppBuffer                           Double pointer to the buffer.
*
********************************************************************************** */
static void fsciBleGapHandoverGetBufferFromAnchorMonitorPacketContinueEventParams
(
    handoverAnchorMonitorPacketContinueEvent_t  *pHandoverAnchorMonitorPacketContinueEventParams,
    uint8_t                             **ppBuffer
);

/*! *********************************************************************************
* \brief  Places Time Sync event parameters into a buffer.
*
* \param[in]       pHandoverTimeSyncEventParams  Pointer to the params structure.
* \param[in,out]   ppBuffer                      Double pointer to the buffer.
*
********************************************************************************** */
static void fsciBleGapHandoverGetBufferFromTimeSyncEventParams
(
    handoverTimeSyncEvent_t    *pHandoverTimeSyncEventParams,
    uint8_t                    **ppBuffer
);

/*! *********************************************************************************
* \brief  Places Connection parameters update event parameters into a buffer.
*
* \param[in]       ConnParamUpdateEvtParams  Pointer to the params structure.
* \param[in,out]   ppBuffer                             Double pointer to the buffer.
*
********************************************************************************** */
static void fsciBleGapHandoverGetBufferFromConnParamUpdateEventParams
(
    handoverConnParamUpdateEvent_t  *ConnParamUpdateEvtParams,
    uint8_t                         **ppBuffer
);
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  Retrieves Anchor Search Start parameters from a buffer.
*
* \param[out]      pParams  Pointer to the parameters structure.
* \param[in,out]   ppBuffer Double pointer to the buffer.
*
********************************************************************************** */
void fsciBleGapHandoverGetAnchorSearchStartParametersFromBuffer
(
    gapHandoverAnchorSearchStartParams_t        *pParams,
    uint8_t                                     **ppBuffer
)
{
    union
    {
        uint8_t uMode;
        bleHandoverAnchorSearchMode_t mode;
    } temp;
    
    fsciBleGetUint32ValueFromBuffer(pParams->startTime625, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pParams->startTimeOffset, *ppBuffer);
    fsciBleGetUint32ValueFromBuffer(pParams->lastRxInstant, *ppBuffer);
    fsciBleGetArrayFromBuffer(pParams->accessAddress, *ppBuffer, 4 * sizeof(uint8_t));
    fsciBleGetArrayFromBuffer(pParams->crcInit, *ppBuffer, 3 * sizeof(uint8_t));
    fsciBleGetArrayFromBuffer(pParams->channelMap, *ppBuffer, gcBleChannelMapSize_c *
                              sizeof(uint8_t));
    fsciBleGetUint16ValueFromBuffer(pParams->connInterval, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pParams->latency, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pParams->supervisionTimeout, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pParams->eventCounter, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->centralSca, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->role, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->centralPhy, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->seqNum, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->hopAlgo2, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->unmappedChannelIndex, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->hopIncrement, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->ucNbReports, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pParams->uiEventCounterAdvance, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->timeout, *ppBuffer);
    fsciBleGetUint32ValueFromBuffer(pParams->timingDiffSlot, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pParams->timingDiffOffset, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(temp.uMode, *ppBuffer);
    pParams->mode = temp.mode;
}

/*! *********************************************************************************
* \brief  Retrieves Time Sync Transmit parameters from a buffer.
*
* \param[out]      pParams  Pointer to the parameters structure.
* \param[in,out]   ppBuffer Double pointer to the buffer.
*
********************************************************************************** */
void fsciBleGapHandoverGetTimeSyncTransmitParametersFromBuffer
(
    gapHandoverTimeSyncTransmitParams_t        *pParams,
    uint8_t                                    **ppBuffer
)
{
    fsciBleGetEnumValueFromBuffer(pParams->enable, *ppBuffer, bleHandoverTimeSyncEnable_t);
    fsciBleGetUint8ValueFromBuffer(pParams->advChannel, *ppBuffer);
    fsciBleGetArrayFromBuffer(pParams->deviceAddress, *ppBuffer, gcBleDeviceAddressSize_c * sizeof(uint8_t));
    fsciBleGetUint8ValueFromBuffer(pParams->phys, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->txPowerLevel, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->txIntervalSlots625, *ppBuffer);
}

/*! *********************************************************************************
* \brief  Retrieves Time Sync Receive parameters from a buffer.
*
* \param[out]      pParams  Pointer to the parameters structure.
* \param[in,out]   ppBuffer Double pointer to the buffer.
*
********************************************************************************** */
void fsciBleGapHandoverGetTimeSyncReceiveParametersFromBuffer
(
    gapHandoverTimeSyncReceiveParams_t        *pParams,
    uint8_t                                   **ppBuffer
)
{
    fsciBleGetEnumValueFromBuffer(pParams->enable, *ppBuffer, bleHandoverTimeSyncEnable_t);
    fsciBleGetUint8ValueFromBuffer(pParams->scanChannel, *ppBuffer);
    fsciBleGetArrayFromBuffer(pParams->deviceAddress, *ppBuffer, gcBleDeviceAddressSize_c * sizeof(uint8_t));
    fsciBleGetUint8ValueFromBuffer(pParams->phys, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pParams->stopWhenFound, *ppBuffer, bleHandoverTimeSyncStopWhenFound_t);
}

/*! *********************************************************************************
* \brief  Retrieves size of a given generic event.
*
* \param[in]    pGenericEvent Pointer to the generic event data.
*
********************************************************************************** */
uint32_t fsciBleGapHandoverGetGenericEventBufferSize(gapGenericEvent_t* pGenericEvent)
{
    /* Get the constant size for the needed buffer */
    uint32_t bufferSize = 0;

    /* Get the variable size for the needed buffer */
    switch(pGenericEvent->eventType)
    {
        case gHandoverGetComplete_c:    /* Fall Through */
        case gHandoverSetComplete_c:
            {
                bufferSize += sizeof(bleResult_t) +
                              gHandoverMaxPayloadLen_c;
            }
            break;

        case gHandoverGetCsLlContextComplete_c:
            {
                bufferSize += sizeof(bleResult_t) + sizeof(uint16_t) +
                              sizeof(uint8_t) + gHandoverMaxPayloadLen_c;
            }
            break;

        case gHandoverGetTime_c:
            {
                bufferSize += sizeof(bleResult_t) +
                              sizeof(uint32_t) +
                              sizeof(uint16_t);
            }
            break;

        case gHandoverSuspendTransmitComplete_c:        /* Fall Through */
        case gHandoverResumeTransmitComplete_c:         /* Fall Through */
        case gHandoverAnchorNotificationStateChanged_c:
            {
                bufferSize += sizeof(uint16_t);
            }
            break;

        case gHandoverAnchorSearchStarted_c:
            {
                bufferSize += sizeof(bleResult_t) +
                              sizeof(uint16_t);
            }
            break;

        case gHandoverAnchorSearchStopped_c:
            {
                bufferSize += sizeof(bleResult_t) +
                              sizeof(uint16_t);
            }
            break;

        case gHandoverAnchorMonitorEvent_c:
            {
                bufferSize += 3 * sizeof(uint16_t) +
                              sizeof (uint32_t) +
                              8 * sizeof(uint8_t);
            }
            break;

        case gHandoverAnchorMonitorPacketEvent_c:
            {
                bufferSize += 3 * sizeof(uint16_t) +
                              sizeof (uint32_t) +
                              8 * sizeof(uint8_t) +
                              pGenericEvent->eventData.handoverAnchorMonitorPacket.pduSize;
            }
            break;

        case gHandoverAnchorMonitorPacketContinueEvent_c:
            {
                bufferSize += sizeof(uint16_t) +
                              2 * sizeof(uint8_t) + 
                              pGenericEvent->eventData.handoverAnchorMonitorPacketContinue.pduSize;
            }
            break;

        case gHandoverTimeSyncEvent_c:
            {
                bufferSize += 2 * sizeof(uint32_t) +
                              2 * sizeof(uint16_t) +
                              sizeof(uint8_t);
            }
            break;

        case gHandoverConnParamUpdateEvent_c:
            {
                bufferSize += 16 * sizeof(uint8_t) +
                              6 * sizeof(uint16_t) +
                              3 * sizeof(uint32_t);
            }
            break;

        case gHandoverUpdateConnParamsComplete_c:
            {
                bufferSize += sizeof(bleResult_t) +
                              sizeof(uint16_t);
            }
            break;

        default:
            ; /* For MISRA compliance */
            break;
    }

    /* Return the size needed for the buffer */
    return bufferSize;
}

/*! *********************************************************************************
* \brief  Places generic event data into a buffer.
*
* \param[in]    pGenericEvent  Pointer to the generic event data.
* \param[out]   ppBuffer       Double pointer to the buffer.
*
********************************************************************************** */
void fsciBleGapHandoverGetBufferFromGenericEvent(gapGenericEvent_t* pGenericEvent, uint8_t** ppBuffer)
{
    /* Write gapGenericEvent_t fields in buffer (without eventType) */
    switch(pGenericEvent->eventType)
    {
        case gHandoverGetComplete_c:
            {
                fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.handoverGetData.status, *ppBuffer, bleResult_t);
                fsciBleGetBufferFromArray(pGenericEvent->eventData.handoverGetData.pData, *ppBuffer, gHandoverMaxPayloadLen_c);
            }
            break;

        case gHandoverSetComplete_c:
            {
                fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.handoverSetData.status, *ppBuffer, bleResult_t);
                fsciBleGetBufferFromArray(pGenericEvent->eventData.handoverSetData.pData, *ppBuffer, gHandoverMaxPayloadLen_c);
            }
            break;

        case gHandoverGetCsLlContextComplete_c:
            {
                fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.handoverGetCsLlContext.status, *ppBuffer, bleResult_t);
                fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.handoverGetCsLlContext.responseMask, *ppBuffer);
                fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.handoverGetCsLlContext.llContextLength, *ppBuffer);
                fsciBleGetBufferFromArray(pGenericEvent->eventData.handoverGetCsLlContext.llContext, *ppBuffer, pGenericEvent->eventData.handoverGetCsLlContext.llContextLength);
            }
            break;

        case gHandoverGetTime_c:
            {
                fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.handoverGetTime.status, *ppBuffer, bleResult_t);
                fsciBleGetBufferFromUint32Value(pGenericEvent->eventData.handoverGetTime.slot, *ppBuffer);
                fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.handoverGetTime.us_offset, *ppBuffer);
            }
            break;

        case gHandoverSuspendTransmitComplete_c:
            {
                fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.handoverSuspendTransmitComplete.connectionHandle, *ppBuffer);
            }
            break;

        case gHandoverResumeTransmitComplete_c:
            {
                fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.handoverResumeTransmitComplete.connectionHandle, *ppBuffer);
            }
            break;

        case gHandoverAnchorNotificationStateChanged_c:
            {
                fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.handoverAnchorNotificationStateChanged.connectionHandle, *ppBuffer);
            }
            break;

        case gHandoverAnchorSearchStarted_c:
            {
                fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.handoverAnchorSearchStart.status, *ppBuffer, bleResult_t);
                fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.handoverAnchorSearchStart.connectionHandle, *ppBuffer);
            }
            break;

        case gHandoverAnchorSearchStopped_c:
            {
                fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.handoverAnchorSearchStop.status, *ppBuffer, bleResult_t);
                fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.handoverAnchorSearchStop.connectionHandle, *ppBuffer);
            }
            break;

        case gHandoverAnchorMonitorEvent_c:
            {
                fsciBleGapHandoverGetBufferFromAnchorMonitorEventParams(&pGenericEvent->eventData.handoverAnchorMonitor, ppBuffer);
            }
            break;

        case gHandoverAnchorMonitorPacketEvent_c:
            {
                fsciBleGapHandoverGetBufferFromAnchorMonitorPacketEventParams(&pGenericEvent->eventData.handoverAnchorMonitorPacket, ppBuffer);
                /* Free PDU memory */
                (void)MEM_BufferFree(pGenericEvent->eventData.handoverAnchorMonitorPacket.pPdu);
            }
            break;

        case gHandoverAnchorMonitorPacketContinueEvent_c:
            {
                fsciBleGapHandoverGetBufferFromAnchorMonitorPacketContinueEventParams(&pGenericEvent->eventData.handoverAnchorMonitorPacketContinue, ppBuffer);
                /* Free PDU memory */
                (void)MEM_BufferFree(pGenericEvent->eventData.handoverAnchorMonitorPacketContinue.pPdu);
            }
            break;

        case gHandoverTimeSyncEvent_c:
            {
                fsciBleGapHandoverGetBufferFromTimeSyncEventParams(&pGenericEvent->eventData.handoverTimeSync, ppBuffer);
            }
            break;

        case gHandoverConnParamUpdateEvent_c:
            {
                fsciBleGapHandoverGetBufferFromConnParamUpdateEventParams(&pGenericEvent->eventData.handoverConnParamUpdate, ppBuffer);
            }
            break;

        case gHandoverUpdateConnParamsComplete_c:
            {
                fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.handoverUpdateConnParams.status, *ppBuffer, bleResult_t);
                fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.handoverUpdateConnParams.connectionHandle, *ppBuffer);
            }
            break;

        default:
            ; /* For MISRA compliance */
            break;
    }
}

/*! *********************************************************************************
* \brief  Retrieves size of a given connection event.
*
* \param[in]    pConnectionEvent Pointer to the connection event data.
*
********************************************************************************** */
uint32_t fsciBleGapHandoverGetConnectionEventBufferSize(gapConnectionEvent_t* pConnectionEvent)
{
    /* Get the constant size for the needed buffer */
    uint32_t bufferSize = 0;

    /* Get the variable size for the needed buffer */
    switch(pConnectionEvent->eventType)
    {
        case gConnEvtHandoverConnected_c:
            {
                bufferSize += sizeof(bleAddressType_t) +
                              gcBleDeviceAddressSize_c +
                              sizeof(bleLlConnectionRole_t);
            }
            break;
            
        case gHandoverDisconnected_c:
            {
                bufferSize += sizeof(bleResult_t);
            }
            break;

        default:
            ; /* For MISRA compliance */
            break;
    }

    /* Return the size needed for the buffer */
    return bufferSize;
}

/*! *********************************************************************************
* \brief  Places connection event data into a buffer.
*
* \param[in]    pConnectionEvent  Pointer to the connection event data.
* \param[out]   ppBuffer          Double pointer to the buffer.
*
********************************************************************************** */
void fsciBleGapHandoverGetBufferFromConnectionEvent(gapConnectionEvent_t* pConnectionEvent, uint8_t** ppBuffer)
{
    /* Write gapConnectionEvent_t fields in buffer (without eventType) */
    switch(pConnectionEvent->eventType)
    {
        case gConnEvtHandoverConnected_c:
            {
                fsciBleGetBufferFromEnumValue(pConnectionEvent->eventData.handoverConnectedEvent.peerAddressType, *ppBuffer, bleAddressType_t);
                fsciBleGetBufferFromAddress(pConnectionEvent->eventData.handoverConnectedEvent.peerAddress, *ppBuffer);
                fsciBleGetBufferFromEnumValue(pConnectionEvent->eventData.handoverConnectedEvent.connectionRole, *ppBuffer, bleLlConnectionRole_t);
            }
            break;
            
        case gHandoverDisconnected_c:
            {
                fsciBleGetBufferFromEnumValue(pConnectionEvent->eventData.handoverDisconnectedEvent.status, *ppBuffer, bleResult_t);
            }
            break;

        default:
            ; /* For MISRA compliance */
            break;
    }
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  Places Anchor Monitor event parameters into a buffer.
*
* \param[in]       pHandoverAnchorMonitorEventParams  Pointer to the params structure.
* \param[in,out]   ppBuffer                           Double pointer to the buffer.
*
********************************************************************************** */
static void fsciBleGapHandoverGetBufferFromAnchorMonitorEventParams
(
    handoverAnchorMonitorEvent_t    *pHandoverAnchorMonitorEventParams,
    uint8_t                         **ppBuffer
)
{
    /* Write handoverAnchorMonitorEvent_t structure fields in buffer */
    fsciBleGetBufferFromUint16Value(pHandoverAnchorMonitorEventParams->connectionHandle, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pHandoverAnchorMonitorEventParams->connEvent, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorEventParams->rssiRemote, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorEventParams->lqiRemote, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorEventParams->statusRemote, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorEventParams->rssiActive, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorEventParams->lqiActive, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorEventParams->statusActive, *ppBuffer);
    fsciBleGetBufferFromUint32Value(pHandoverAnchorMonitorEventParams->anchorClock625Us, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pHandoverAnchorMonitorEventParams->anchorDelay, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorEventParams->chIdx, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorEventParams->ucNbReports, *ppBuffer);
}

/*! *********************************************************************************
* \brief  Places Anchor Monitor Packet event parameters into a buffer.
*
* \param[in]       pHandoverAnchorMonitorPacketEventParams  Pointer to the params structure.
* \param[in,out]   ppBuffer                           Double pointer to the buffer.
*
********************************************************************************** */
static void fsciBleGapHandoverGetBufferFromAnchorMonitorPacketEventParams
(
    handoverAnchorMonitorPacketEvent_t  *pHandoverAnchorMonitorPacketEventParams,
    uint8_t                             **ppBuffer
)
{
    /* Write handoverAnchorMonitorPacketEvent_t structure fields in buffer */
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorPacketEventParams->packetCounter, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pHandoverAnchorMonitorPacketEventParams->connectionHandle, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorPacketEventParams->statusPacket, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorPacketEventParams->phy, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorPacketEventParams->chIdx, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorPacketEventParams->rssiPacket, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorPacketEventParams->lqiPacket, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pHandoverAnchorMonitorPacketEventParams->connEvent, *ppBuffer);
    fsciBleGetBufferFromUint32Value(pHandoverAnchorMonitorPacketEventParams->anchorClock625Us, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pHandoverAnchorMonitorPacketEventParams->anchorDelay, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorPacketEventParams->ucNbConnIntervals, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorPacketEventParams->pduSize, *ppBuffer);
    FLib_MemCpy(*ppBuffer, pHandoverAnchorMonitorPacketEventParams->pPdu, pHandoverAnchorMonitorPacketEventParams->pduSize);
}

/*! *********************************************************************************
* \brief  Places Anchor Monitor Packet Continue event parameters into a buffer.
*
* \param[in]       pHandoverAnchorMonitorPacketContinueEventParams  Pointer to the params structure.
* \param[in,out]   ppBuffer                           Double pointer to the buffer.
*
********************************************************************************** */
static void fsciBleGapHandoverGetBufferFromAnchorMonitorPacketContinueEventParams
(
    handoverAnchorMonitorPacketContinueEvent_t  *pHandoverAnchorMonitorPacketContinueEventParams,
    uint8_t                             **ppBuffer
)
{
    /* Write handoverAnchorMonitorPacketContinueEvent_t structure fields in buffer */
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorPacketContinueEventParams->packetCounter, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pHandoverAnchorMonitorPacketContinueEventParams->connectionHandle, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverAnchorMonitorPacketContinueEventParams->pduSize, *ppBuffer);
    FLib_MemCpy(*ppBuffer, pHandoverAnchorMonitorPacketContinueEventParams->pPdu, pHandoverAnchorMonitorPacketContinueEventParams->pduSize);
}
/*! *********************************************************************************
* \brief  Places Time Sync event parameters into a buffer.
*
* \param[in]       pHandoverTimeSyncEventParams  Pointer to the params structure.
* \param[in,out]   ppBuffer                      Double pointer to the buffer.
*
********************************************************************************** */
static void fsciBleGapHandoverGetBufferFromTimeSyncEventParams
(
    handoverTimeSyncEvent_t    *pHandoverTimeSyncEventParams,
    uint8_t                    **ppBuffer
)
{
    /* Write handoverTimeSyncEvent_t structure fields in buffer */
    fsciBleGetBufferFromUint32Value(pHandoverTimeSyncEventParams->txClkSlot, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pHandoverTimeSyncEventParams->txUs, *ppBuffer);
    fsciBleGetBufferFromUint32Value(pHandoverTimeSyncEventParams->rxClkSlot, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pHandoverTimeSyncEventParams->rxUs, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHandoverTimeSyncEventParams->rssi, *ppBuffer);
}

/*! *********************************************************************************
* \brief  Places Connection parameters update event parameters into a buffer.
*
* \param[in]       ConnParamUpdateEvtParams  Pointer to the params structure.
* \param[in,out]   ppBuffer                  Double pointer to the buffer.
*
********************************************************************************** */
static void fsciBleGapHandoverGetBufferFromConnParamUpdateEventParams
(
    handoverConnParamUpdateEvent_t  *ConnParamUpdateEvtParams,
    uint8_t                         **ppBuffer
)
{
    /* Write handoverConnParamUpdateEvent_t structure fields in buffer */
    fsciBleGetBufferFromUint8Value(ConnParamUpdateEvtParams->status, *ppBuffer);
    fsciBleGetBufferFromUint16Value(ConnParamUpdateEvtParams->connectionHandle, *ppBuffer);
    fsciBleGetBufferFromUint32Value(ConnParamUpdateEvtParams->ulTxAccCode, *ppBuffer);
    fsciBleGetBufferFromArray(ConnParamUpdateEvtParams->aCrcInitVal, *ppBuffer, 3U * sizeof(uint8_t));
    fsciBleGetBufferFromUint16Value(ConnParamUpdateEvtParams->uiConnInterval, *ppBuffer);
    fsciBleGetBufferFromUint16Value(ConnParamUpdateEvtParams->uiSuperTO, *ppBuffer);
    fsciBleGetBufferFromUint16Value(ConnParamUpdateEvtParams->uiConnLatency, *ppBuffer);
    fsciBleGetBufferFromArray(ConnParamUpdateEvtParams->aChMapBm, *ppBuffer, 5U * sizeof(uint8_t));
    fsciBleGetBufferFromUint8Value(ConnParamUpdateEvtParams->ucChannelSelection, *ppBuffer);
    fsciBleGetBufferFromUint8Value(ConnParamUpdateEvtParams->ucHop, *ppBuffer);
    fsciBleGetBufferFromUint8Value(ConnParamUpdateEvtParams->ucUnMapChIdx, *ppBuffer);
    fsciBleGetBufferFromUint8Value(ConnParamUpdateEvtParams->ucCentralSCA, *ppBuffer);
    fsciBleGetBufferFromUint8Value(ConnParamUpdateEvtParams->ucRole, *ppBuffer);
    fsciBleGetBufferFromUint8Value(ConnParamUpdateEvtParams->aucRemoteMasRxPHY, *ppBuffer);
    fsciBleGetBufferFromUint8Value(ConnParamUpdateEvtParams->seqNum, *ppBuffer);
    fsciBleGetBufferFromUint16Value(ConnParamUpdateEvtParams->uiConnEvent, *ppBuffer);
    fsciBleGetBufferFromUint32Value(ConnParamUpdateEvtParams->ulAnchorClk, *ppBuffer);
    fsciBleGetBufferFromUint16Value(ConnParamUpdateEvtParams->uiAnchorDelay, *ppBuffer);
    fsciBleGetBufferFromUint32Value(ConnParamUpdateEvtParams->ulRxInstant, *ppBuffer);
}
#endif /* gFsciIncluded_c && gFsciBleGapLayerEnabled_d */

/*! *********************************************************************************
* @}
********************************************************************************** */
