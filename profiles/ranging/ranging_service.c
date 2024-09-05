/*! *********************************************************************************
* \addtogroup Ranging Service
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022-2024 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "math.h"

#include "ble_general.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_timer_manager.h"
#include "gatt_db_app_interface.h"
#include "gatt_server_interface.h"
#include "gap_interface.h"
#include "att_types.h"
#include "hci_interface.h"
#include "ranging_interface.h"

/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

/*! If the Antenna Path Filter is defined as 0bxxxx1111, then report all 4 antenna paths */
#define gAntennaPathFilterAllowAll_c    15U

/*! Antenna Path 1 */
#define gAP1_c    0b0000001
/*! Antenna Path 2 */
#define gAP2_c    0b0000010
/*! Antenna Path 3 */
#define gAP3_c    0b0000100
/*! Antenna Path 4 */
#define gAP4_c    0b0001000

/*! Index for Mode0 data in filter values array */
#define gMode0Idx_c    0U
/*! Index for Mode1 data in filter values array */
#define gMode1Idx_c    1U
/*! Index for Mode2 data in filter values array*/
#define gMode2Idx_c    2U
/*! Index for Mode3 data in filter values array */
#define gMode3Idx_c    3U

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/* Lookup table for RAS-CP command size*/
uint8_t maRasCPCommandSizes[5U] = {3U, 3U, 5U, 1U, 3U};

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
/*! Subscribed Client */
static deviceId_t mRas_ClientDeviceId[gAppMaxConnections_c];

/*! Offset in the CS Subevent Result buffer of the data to be notified */
static uint32_t gRasSubeventStepIndex = 0U;

/*! Service configuration received from application */
static rasConfig_t *mpRasServiceConfig = NULL;

/*! ATT MTU value to be used for Ranging Data notifications */
static uint16_t  gAttMtu[gAppMaxConnections_c];

/*! Status of data transfer */
static rasServerTransferStatus_t mDataTransferState[gAppMaxConnections_c];

/*! Filter values received through the RAS Filter CP command. */
static uint16_t maModeFilters[4U * gAppMaxConnections_c];

/*! Segment information for Retrieve Lost Ranging Data Segments command */
static segmentData_t maSegmentData[gRASMaxNoOfSegments_c];

/*! For each connected peer:
    idx 0 = start segment index (updated after each regment retransmission)
    idx 1 = end segment index (not updated - value received from peer)
    idx 2 = start segment index (not updated - value received from peer) */
static uint8_t maLostSegmRecvIdx[3U * gAppMaxConnections_c] = {0U};

/*! On-Demand Data behavior: BIT0 = 0 notifications BIT0 = 1 indications
    Control Point behavior: BIT1 = 0 notifications BIT1 = 1 indications
    Data Ready behavior: BIT2 = 0 notifications BIT2 = 1 indications
    Data Overwritten behavior: BIT3 = 0 notifications BIT3 = 1 indications
    Real-Time Data behavior: BIT4 = 0 notifications BIT4 = 1 indications */
static uint8_t mNotifIndEnabled[gAppMaxConnections_c] = {0U};

/*! TRUE if peer has enabled real time data transfer */
static bool_t mbServRealTimeTransfer[gAppMaxConnections_c];

/* Antenna permutation order */
const uint8_t maAntPermNAp[24][4] = {
    {0,1,2,3}, /* A1,A2,A3,A4 */
    {1,0,2,3}, /* A2,A1,A3,A4 */
    {0,2,1,3}, /* A1,A3,A2,A4 */
    {2,0,1,3}, /* A3,A1,A2,A4 */
    {2,1,0,3}, /* A3,A2,A1,A4 */
    {1,2,0,3}, /* A2,A3,A1,A4 */
    {0,1,3,2}, /* A1,A2,A4,A3 */
    {1,0,3,2}, /* A2,A1,A4,A3 */
    {0,3,1,2}, /* A1,A4,A2,A3 */
    {3,0,1,2}, /* A4,A1,A2,A3 */
    {3,1,0,2}, /* A4,A2,A1,A3 */
    {1,3,0,2}, /* A2,A4,A1,A3 */
    {0,3,2,1}, /* A1,A4,A3,A2 */
    {3,0,2,1}, /* A4,A1,A3,A2 */
    {0,2,3,1}, /* A1,A3,A4,A2 */
    {2,0,3,1}, /* A3,A1,A4,A2 */
    {2,3,0,1}, /* A3,A4,A1,A2 */
    {3,2,0,1}, /* A4,A3,A1,A2 */
    {3,1,2,0}, /* A4,A2,A3,A1 */
    {1,3,2,0}, /* A2,A4,A3,A1 */
    {3,2,1,0}, /* A4,A3,A2,A1 */
    {2,3,1,0}, /* A3,A4,A2,A1 */
    {2,1,3,0}, /* A3,A2,A4,A1 */
    {1,2,3,0}, /* A2,A3,A4,A1 */
};

/* Mechanism for Ranging Profile-defined RRSP timer */
static TIMER_MANAGER_HANDLE_DEFINE(mRrspTimerId);
static deviceId_t mDeviceId = gInvalidDeviceId_c;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
static bleResult_t sendCommandResponse
(
    deviceId_t deviceId,
    uint8_t* pNotifPayload,
    uint32_t valueLength
);

static void antennaPathFilterStepData
(
    deviceId_t deviceId,
    uint8_t*   pEventData,
    uint8_t*   pNotifData,
    uint16_t   maxLength,
    uint16_t*  pOutLen
);

static bleResult_t handleGetRangingDataSegmNotif
(
    deviceId_t deviceId,
    uint8_t    startAbsSegment,
    uint8_t    endAbsSegment
);

static void packSubeventHeader
(
    uint8_t* pRangingDataBody
);

static bool_t checkStartEndSegm
(
    deviceId_t deviceId,
    uint16_t startSegm,
    uint16_t endSegm
);

static void checkRasStatus
(
    deviceId_t deviceId,
    rasControlPointRspCodeValues_tag rasStatus
);

static bleResult_t handleGetRangingDataCmd
(
    deviceId_t deviceId,
    uint16_t   cmdLen,
    rasControlPointReq_t *pRasCtrlPointCmd
);

static bleResult_t handleAckRangingDataCmd
(
    deviceId_t deviceId,
    uint16_t   cmdLen,
    rasControlPointReq_t *pRasCtrlPointCmd
);

static bleResult_t handleAbortCmd
(
    deviceId_t deviceId,
    uint16_t   cmdLen,
    rasControlPointReq_t *pRasCtrlPointCmd
);

static bleResult_t handleRetrLostSegmentsCmd
(
    deviceId_t deviceId,
    uint16_t   cmdLen,
    rasControlPointReq_t *pRasCtrlPointCmd
);

static bleResult_t handleSetFilterCmd
(
    deviceId_t deviceId,
    uint16_t   cmdLen,
    rasControlPointReq_t *pRasCtrlPointCmd
);

static void RrspTimerCallback
(
    void *param
);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*!**********************************************************************************
* \brief        Starts Ranging Service functionality
*
* \param[in]    pServiceConfig  Pointer to the server configuration information.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_Start
(
    rasConfig_t *pServiceConfig
)
{
    mpRasServiceConfig = pServiceConfig;
    FLib_MemSet(maModeFilters, 0xFFU, 4U * gAppMaxConnections_c * sizeof(uint16_t));
    for (uint8_t idx = 0; idx < gAppMaxConnections_c; idx++)
    {
        mRas_ClientDeviceId[idx] = gInvalidDeviceId_c;
    }
    (void)TM_Open(mRrspTimerId);
    return gBleSuccess_c;
}

/*!**********************************************************************************
* \brief        Stops Ranging Service functionality
*
* \param[in]    pServiceConfig  Pointer to the server configuration information.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_Stop
(
    rasConfig_t *pServiceConfig
)
{
    mpRasServiceConfig = NULL;
    FLib_MemSet(maModeFilters, 0xFFU, 4U * gAppMaxConnections_c * sizeof(uint16_t));

    for (uint8_t idx = 0; idx < gAppMaxConnections_c; idx++)
    {
        mRas_ClientDeviceId[idx] = gInvalidDeviceId_c;
        gAttMtu[idx] = gAttDefaultMtu_c;
        mNotifIndEnabled[idx] = 0U;
        mbServRealTimeTransfer[idx] = FALSE;
        maLostSegmRecvIdx[idx * 3U] = 0U;
        maLostSegmRecvIdx[idx * 3U + 1U] = 0U;
        maLostSegmRecvIdx[idx * 3U + 2U] = 0U;
    }
    return gBleSuccess_c;
}

/*!**********************************************************************************
* \brief        Subscribes a GATT client to the Ranging service
*
* \param[in]    clientDeviceId   Client Id in Device DB.
* \param[in]    realTime         TRUE if peer has enabled real-time data transfer, FALSE for on-demand.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_Subscribe
(
    deviceId_t clientDeviceId,
    bool_t     realTime
)
{
    for (uint8_t idx = 0; idx < gAppMaxConnections_c; idx++)
    {
        if (mRas_ClientDeviceId[idx] == gInvalidDeviceId_c)
        {
            mRas_ClientDeviceId[idx] = clientDeviceId;
            mbServRealTimeTransfer[idx] = realTime;
            break;
        }
    }
    return gBleSuccess_c;
}

/*!**********************************************************************************
* \brief        Unsubscribes a GATT client from the Ranging service
*
* \param[in]    clientDeviceId  Identifier of the peer to be unsubscribed
* \param[in]    disconnect      TRUE if peer has disconnected, FALSE otherwise
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_Unsubscribe
(
    deviceId_t clientDeviceId,
    bool_t disconnect
)
{
    for (uint8_t idx = 0; idx < gAppMaxConnections_c; idx++)
    {
        if (mRas_ClientDeviceId[idx] == clientDeviceId)
        {
            mNotifIndEnabled[idx] = 0U;
            mRas_ClientDeviceId[idx] = gInvalidDeviceId_c;

            if (disconnect == TRUE)
            {
                FLib_MemSet(&maModeFilters[clientDeviceId * 4U], 0xFFU, 4U * sizeof(uint16_t));
                gAttMtu[idx] = gAttDefaultMtu_c;
                mRas_ClientDeviceId[idx] = gInvalidDeviceId_c;
                maLostSegmRecvIdx[idx * 3U] = 0U;
                maLostSegmRecvIdx[idx * 3U + 1U] = 0U;
                maLostSegmRecvIdx[idx * 3U + 2U] = 0U;
            }
            break;
        }
    }

    if (mpRasServiceConfig->pRangingDataBody != NULL)
    {
        (void)MEM_BufferFree(mpRasServiceConfig->pRangingDataBody);
        mpRasServiceConfig->pRangingDataBody = NULL;
    }

    return gBleSuccess_c;
}

/*!**********************************************************************************
* \brief        Set the ATT MTU value to be used by RAS for Ranging Data notifications
*
* \param[in]    deviceId    Peer identifier
* \param[in]    attMtu      ATT MTU value
*
* \return       none
************************************************************************************/
void Ras_SetMtuValue
(
    deviceId_t deviceId,
    uint16_t   attMtu
)
{
    gAttMtu[deviceId] = attMtu;
}

/*!**********************************************************************************
* \brief        Send a Data Ready indication or notification to the peer after all
*               procedure data has been received from the local controller.
*
* \param[in]    clientdeviceId              Identifier of the peer
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_SendDataReady
(
    deviceId_t   clientDeviceId
)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t valueLength = (uint16_t)sizeof(rasDataReadyIndication_t);

    rasDataReadyIndication_t* indicationData = MEM_BufferAlloc(valueLength);

    if (indicationData != NULL)
    {
        indicationData->procedureIndex = mpRasServiceConfig->pData->procedureCounter;

        if ((clientDeviceId != gInvalidDeviceId_c) && (mbServRealTimeTransfer[clientDeviceId] == FALSE))
        {
            if ((mNotifIndEnabled[clientDeviceId] & BIT2) != 0U)
            {
                result = GattServer_SendInstantValueIndication(clientDeviceId,
                                                               mpRasServiceConfig->dataReadyHandle,
                                                               valueLength,
                                                               (uint8_t*)indicationData);
            }
            else
            {
                result = GattServer_SendInstantValueNotification(clientDeviceId,
                                                                 mpRasServiceConfig->dataReadyHandle,
                                                                 valueLength,
                                                                 (uint8_t*)indicationData);
            }
        }
        else
        {
            result = gBleInvalidParameter_c;
        }

        (void)MEM_BufferFree(indicationData);

    }
    else
    {
        result = gBleOutOfMemory_c;
    }

    return result;
}

/*!**********************************************************************************
* \brief        Send a Data Overwritten indication or notification to the peer if new.
*               procedure data was received before the peer sent the Ack command.
*
* \param[in]    clientDeviceId              Identifier of the peer
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_SendDataOverwritten
(
    deviceId_t  clientDeviceId
)
{
    bleResult_t result = gBleInvalidParameter_c;
    uint8_t payload[2];

    if ((clientDeviceId != gInvalidDeviceId_c) && (mbServRealTimeTransfer[clientDeviceId] == FALSE))
    {
        Utils_PackTwoByteValue(mpRasServiceConfig->pData->procedureCounter, payload);

        if ((mNotifIndEnabled[clientDeviceId] & BIT3) != 0U)
        {
            result = GattServer_SendInstantValueIndication(clientDeviceId,
                                                           mpRasServiceConfig->dataOverwrittenHandle,
                                                           (uint16_t)sizeof(uint16_t),
                                                           payload);
        }
        else
        {
            result = GattServer_SendInstantValueNotification(clientDeviceId,
                                                             mpRasServiceConfig->dataOverwrittenHandle,
                                                             (uint16_t)sizeof(uint16_t),
                                                             payload);
        }
    }
    else
    {
        result = gBleInvalidParameter_c;
    }

    if (result == gBleSuccess_c)
    {
        mDataTransferState[clientDeviceId] = (uint8_t)noTransferInProgress_c;
    }

    return result;
}

/*!**********************************************************************************
* \brief        Handles write operations on the Control point.
*
* \param[in]    deviceId      Identifier of the peer
* \param[in]    pEvent        Pointer to the GATT Server event
*
* \retval       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_ControlPointHandler
(
    deviceId_t                          deviceId,
    gattServerAttributeWrittenEvent_t*  pEvent
)
{
    rasControlPointReq_t rasCtrlPointCmd;
    rasControlPointRspCodeValues_tag rasStatus = gRasSuccess_c;
    bleResult_t result = gBleSuccess_c;

    /* The RRSP shall send a response to each command written to the RAS Control Point.
       The response takes the form of an indication of the RAS Control Point characteristic */
    FLib_MemCpy(&rasCtrlPointCmd, pEvent->aValue, pEvent->cValueLength);

    switch (rasCtrlPointCmd.cmdOpCode)
    {
        case ((uint8_t)getRangingDataOpCode_c):
        {
            result = handleGetRangingDataCmd(deviceId, pEvent->cValueLength, &rasCtrlPointCmd);
        }
        break;

        case ((uint8_t)ackRangingDataOpCode_c):
        {
            result = handleAckRangingDataCmd(deviceId, pEvent->cValueLength, &rasCtrlPointCmd);
        }
        break;

        case ((uint8_t)abortOperationOpCode_c):
        {
            result = handleAbortCmd(deviceId, pEvent->cValueLength, &rasCtrlPointCmd);
        }
        break;

        case ((uint8_t)retrLostRangingDataOpCode_c):
        {
            result = handleRetrLostSegmentsCmd(deviceId, pEvent->cValueLength, &rasCtrlPointCmd);
        }
        break;

        case ((uint8_t)setFilterOpCode_c):
        {
            result = handleSetFilterCmd(deviceId, pEvent->cValueLength, &rasCtrlPointCmd);
        }
        break;

        default:
        {
            /* Unrecognized comman */
            rasStatus = gRasOpCodeNotSupportedError_c;
        }
        break;
    }

    checkRasStatus(deviceId, rasStatus);

    return result;
}

/*!**********************************************************************************
* \brief        Set the RAS data pointer to the local measurement data
*
* \param[in]    pData    Pointer to the local measurement data
*
* \return       none
************************************************************************************/
void Ras_SetDataPointer
(
    rasMeasurementData_t* pData
)
{
    if (mpRasServiceConfig != NULL)
    {
        mpRasServiceConfig->pData = pData;
    }
}

/*!**********************************************************************************
* \brief        Check if data transfer is currently in progress for the given peer
*
* \param[in]    deviceId        Peer identifier
*
* \return       TRUE if transfer is ongoing, FALSE otherwise
************************************************************************************/
bool_t Ras_CheckTransferInProgress
(
    deviceId_t deviceId
)
{
    return ((mDataTransferState[deviceId] == (uint8_t)noTransferInProgress_c)? FALSE:TRUE);
}

/*!**********************************************************************************
* \brief        Set peer preference for transmitting the ranging data through
*               notifications or indications.
*
* \param[in]    deviceId        Peer identifier
* \param[in]    notifInds       On-Demand Data behavior: BIT0 = 0 notifications BIT0 = 1 indications
*                               Control Point behavior: BIT1 = 0 notifications BIT1 = 1 indications
*                               Data Ready behavior: BIT2 = 0 notifications BIT2 = 1 indications
*                               Data Overwritten behavior: BIT3 = 0 notifications BIT3 = 1 indications
*                               Real-Time Data behavior: BIT4 = 0 notifications BIT4 = 1 indications
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_SetDataSendPreference
(
    deviceId_t deviceId,
    uint8_t notifInds
)
{
    bleResult_t result = gBleSuccess_c;

    if (((notifInds & BIT5) != 0U)
        || ((notifInds & BIT6) != 0U) || ((notifInds & BIT7) != 0U))
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        mNotifIndEnabled[deviceId] |= notifInds;
    }

    return result;
}

/*!**********************************************************************************
* \brief        Get peer preference for transmitting the ranging data through
*               notifications or indications.
*
* \param[in]    deviceId        Peer identifier
*
* \return       On-Demand Data behavior: BIT0 = 0 notifications BIT0 = 1 indications
*               Control Point behavior: BIT1 = 0 notifications BIT1 = 1 indications
*               Data Ready behavior: BIT2 = 0 notifications BIT2 = 1 indications
*               Data Overwritten behavior: BIT3 = 0 notifications BIT3 = 1 indications
*               Real-Time Data behavior: BIT4 = 0 notifications BIT4 = 1 indications
*
************************************************************************************/
uint8_t Ras_GetDataSendPreference
(
    deviceId_t deviceId
)
{
    return mNotifIndEnabled[deviceId];
}

/*!**********************************************************************************
* \brief        Check if segment retransmission is currently in progress
*
* \param[in]    deviceId        Peer identifier
*
* \return       TRUE if transfer is ongoing, FALSE otherwise
************************************************************************************/
bool_t Ras_CheckSegmentTransmInProgress
(
    deviceId_t deviceId
)
{
    return ((mDataTransferState[deviceId] == (uint8_t)segmRetransmInProgress_c)? TRUE:FALSE);
}

/*!**********************************************************************************
* \brief        Sends indications with the missing segments requested by the peer
*
* \param[in]    deviceId          Identifier of the peer
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_HandleGetRangingDataSegmInd
(
    deviceId_t deviceId
)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t     crtSegmIdx = 0U;
    rasControlPointRsp_t    rasCPResponse;
    union
    {
        uint32_t dataLen32;
        uint16_t dataLen16;
    } dataLen = {0U};

    dataLen.dataLen16 = gAttMaxNotifIndDataSize_d(gAttMtu[deviceId]);
    uint8_t *pNotificationData = MEM_BufferAlloc(dataLen.dataLen32);

    if (Ras_CheckIfSubscribed(deviceId) == FALSE)
    {
        /* Client has unsubscribed */
        result = gBleInvalidState_c;
    }
    else if ((maLostSegmRecvIdx[deviceId*3U] == 0U) ||
             (maLostSegmRecvIdx[deviceId*3U] > maLostSegmRecvIdx[(deviceId*3U) + 1U]))
    {
        /* Restart Timer to wait for response */
        (void)TM_InstallCallback((timer_handle_t)mRrspTimerId, RrspTimerCallback, &mDeviceId);
        (void)TM_Start((timer_handle_t)mRrspTimerId,(uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSingleShot | (uint8_t)kTimerModeSetSecondTimer,
                       gRsspTimeoutCompleteSeconds_c);

        /* Fill start segment and end segment value with the indices of the segments sent */
        rasCPResponse.rspOpCode = ((uint8_t)completeLostDataSegmentResponseOpCode_c);
        rasCPResponse.cmdParameters.complLostRsp.procCounter = mpRasServiceConfig->pData->procedureCounter;

        if (maLostSegmRecvIdx[deviceId*3U + 1U] != 0xFFU)
        {
            rasCPResponse.cmdParameters.complLostRsp.startAbsSegment = maLostSegmRecvIdx[deviceId*3U];
            rasCPResponse.cmdParameters.complLostRsp.endAbsSegment = maLostSegmRecvIdx[deviceId*3U + 1U];
        }
        else
        {
            rasCPResponse.cmdParameters.complLostRsp.startAbsSegment = maLostSegmRecvIdx[deviceId*3U + 2U];
            for (uint8_t idx = 0; idx < gRASMaxNoOfSegments_c; idx++)
            {
                if ((maSegmentData[idx].segmentIdx & (uint8_t)gRasNotifLastSegment_c) != 0U)
                {
                    rasCPResponse.cmdParameters.complLostRsp.endAbsSegment = maSegmentData[idx].segmentIdx;
                    break;
                }
            }
        }
        result = sendCommandResponse(deviceId, (uint8_t*)&rasCPResponse, sizeof(rasCPResponse));

        /* Transfer completed. Send response to Get Report Records procedure */
        mDataTransferState[deviceId] = (uint8_t)waitAckRangingDataRetransm_c;
        maLostSegmRecvIdx[deviceId*3U] = 0U;
        maLostSegmRecvIdx[deviceId*3U + 1U] = 0U;
    }
    else if (pNotificationData != NULL)
    {
        for (uint8_t idx = 0; idx < gRASMaxNoOfSegments_c; idx++)
        {
            if (maSegmentData[idx].segmentIdx == maLostSegmRecvIdx[deviceId * 3U])
            {
                crtSegmIdx = idx;
                break;
            }
        }

        assert(crtSegmIdx < gRASMaxNoOfSegments_c);
        segmentData_t *pData = &maSegmentData[crtSegmIdx];

        /* Set segment header */
        *pNotificationData = pData->segmentIdx;

        /* Pack notification data */
        FLib_MemCpy((pNotificationData + sizeof(uint8_t)),
                    mpRasServiceConfig->pRangingDataBody + pData->dataIdxStart,
                    (uint32_t)pData->dataSize);

        /* Send instant indication instead of writing the data in the database */
        result = GattServer_SendInstantValueIndication(deviceId,
                                                       mpRasServiceConfig->onDemandDataHandle,
                                                       (uint16_t)(pData->dataSize + sizeof(uint8_t)),
                                                       pNotificationData);
        /* Move on to the next segment to be sent */
        maLostSegmRecvIdx[deviceId*3U] = maSegmentData[crtSegmIdx + 1U].segmentIdx;
    }
    else
    {
        result = gBleOutOfMemory_c;

        /* Send error response */
        rasCPResponse.rspOpCode = ((uint8_t)responseOpCode_c);
        rasCPResponse.cmdParameters.rspPayload.rspValue = ((uint8_t)gRasProcedureNotCompletedError_c);
        (void)sendCommandResponse(deviceId, (uint8_t*)&rasCPResponse, sizeof(rasCPResponse));
    }

    if (pNotificationData != NULL)
    {
        (void)MEM_BufferFree(pNotificationData);
    }

    return result;
}

/*!**********************************************************************************
* \brief        Clear data before starting a new transfer
*
* \param[in]    deviceId         Identifier of the peer
*
* \return       gBleSuccess_c or error
************************************************************************************/
void Ras_ClearDataForTransfer
(
    deviceId_t deviceId
)
{
    gRasSubeventStepIndex = 0U;
    mDataTransferState[deviceId] = (uint8_t)sendingRangingData_c;
    FLib_MemSet(maSegmentData, 0U, gRASMaxNoOfSegments_c * sizeof(segmentData_t));
    mpRasServiceConfig->pData->totalSentRcvDataIndex = 0U;
}

/*!**********************************************************************************
* \brief        Sends indications with the ranging data
*
* \param[in]    deviceId            Identifier of the peer
* \param[in]    hValueRasNotif      Handle of the Subevent Ranging Data Characteristic
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_SendRangingDataIndication
(
    deviceId_t  deviceId,
    uint16_t    hValueRasNotif
)
{
    uint16_t             availableLen = gAttMaxNotifIndDataSize_d(gAttMtu[deviceId]);
    uint16_t             notifDataLen = 0U;
    bleResult_t          result = gBleSuccess_c;
    uint8_t              segmentationHeader = 0U;
    uint16_t             skippedDataSize = 0U;
    uint16_t             procCounter = mpRasServiceConfig->pData->procedureCounter;
    uint8_t*             pNotificationData = MEM_BufferAlloc((uint32_t)availableLen);
    rasControlPointRsp_t rasCPResponse;
    union
    {
        uint8_t  dataLen8;
        uint16_t dataLen16;
    } dataLen = {0U};

    static uint8_t       segmDataIdx = 0U;
    static uint8_t       segmentCounter = gRasSegmentCounterMinValue_c;

    if (Ras_CheckIfSubscribed(deviceId) == FALSE)
    {
        /* Client has unsubscribed */
        result = gBleInvalidState_c;
    }
    else if (mpRasServiceConfig->pData->totalSentRcvDataIndex >= mpRasServiceConfig->pData->dataParsedLen)
    {
        /* Transfer completed. Send response to Get Report Records procedure */
        result = gBleSuccess_c;
        segmDataIdx = 0U;

        if ((mbServRealTimeTransfer[deviceId] == FALSE) &&
            (mDataTransferState[deviceId] == (uint8_t)sendingRangingData_c))
        {
            mDataTransferState[deviceId] = (uint8_t)waitingAckRangingData_c;
            rasCPResponse.rspOpCode = ((uint8_t)completeProcDataRspOpCode_c);
            rasCPResponse.cmdParameters.procCounter = procCounter;
            result = sendCommandResponse(deviceId, (uint8_t*)&rasCPResponse, sizeof(rasCPResponse));
            mDeviceId = deviceId;
            (void)TM_InstallCallback((timer_handle_t)mRrspTimerId, RrspTimerCallback, &mDeviceId);
            (void)TM_Start((timer_handle_t)mRrspTimerId,
                           (uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSingleShot | (uint8_t)kTimerModeSetSecondTimer,
                           gRsspTimeoutCompleteSeconds_c);
        }
    }
    else if (pNotificationData != NULL)
    {
        FLib_MemSet(pNotificationData, 0U, ((uint32_t)availableLen));

        notifDataLen += ((uint16_t)sizeof(uint8_t));
        availableLen -= ((uint16_t)sizeof(uint8_t));
        segmentationHeader = (uint8_t)(segmentCounter << 2U);

        if (mpRasServiceConfig->pData->totalSentRcvDataIndex == 0U)
        {
            /* First indication */
            gRasSubeventStepIndex = 0U;
            mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].currentDataSize = 0U;
            segmentationHeader |= (uint8_t)gRasNotifFirstSegment_c;
        }

        if (mbServRealTimeTransfer[deviceId] == FALSE)
        {
            maSegmentData[segmDataIdx].dataIdxStart = mpRasServiceConfig->pData->totalSentRcvDataIndex;
        }

        /* Set segment header */
        *pNotificationData = segmentationHeader;

        /* Compute how much data there is left in this subevent */
        uint16_t dataLeft = mpRasServiceConfig->pData->dataParsedLen - mpRasServiceConfig->pData->totalSentRcvDataIndex;

        if (dataLeft < availableLen)
        {
            availableLen = dataLeft;
        }

        /* Pack indication data */
        FLib_MemCpy((pNotificationData + notifDataLen),
                    (mpRasServiceConfig->pRangingDataBody + mpRasServiceConfig->pData->totalSentRcvDataIndex),
                    availableLen);

        mpRasServiceConfig->pData->totalSentRcvDataIndex += availableLen;
        notifDataLen += availableLen;

        /* We do not include step channel and step length in the ranging data body */
        dataLen.dataLen8 = mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].subevtHeader.numStepsReported;
        skippedDataSize = 2U*dataLen.dataLen16;

        if ((mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].dataSize - skippedDataSize +
             (uint16_t)mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].dataIdx) <= mpRasServiceConfig->pData->totalSentRcvDataIndex)
        {
            /* If there are multiple subevents, move on to the next one */
            if (mpRasServiceConfig->pData->subeventIndex > 0U)
            {
                gRasSubeventStepIndex++;
            }
        }

        if ((mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].subevtHeader.procedureDoneStatus == 0U) &&
             (mpRasServiceConfig->pData->totalSentRcvDataIndex == mpRasServiceConfig->pData->dataParsedLen))
        {
            /* Last notification */
            segmentationHeader |= (uint8_t)gRasNotifLastSegment_c;
            gRasSubeventStepIndex = 0U;
            FLib_MemCpy(pNotificationData, &segmentationHeader, sizeof(uint8_t));
        }

        result = GattServer_SendInstantValueIndication(deviceId,
                                                       hValueRasNotif,
                                                       notifDataLen,
                                                       pNotificationData);

        /* Update counters */
        if (mbServRealTimeTransfer[deviceId] == FALSE)
        {
            maSegmentData[segmDataIdx].segmentIdx = segmentationHeader;
            maSegmentData[segmDataIdx].dataSize = notifDataLen;
            segmDataIdx++;
        }

        if ((segmentCounter == (uint8_t)gRasSegmentCounterMaxValue_c) ||
            ((segmentationHeader & (uint8_t)gRasNotifLastSegment_c) != 0U))
        {
            segmentCounter = gRasSegmentCounterMinValue_c;
        }
        else
        {
            /* Update counters */
            segmentCounter++;
        }
    }
    else
    {
        result = gBleOutOfMemory_c;

        /* Send error response */
        if (mbServRealTimeTransfer[deviceId] == FALSE)
        {
            rasCPResponse.rspOpCode = ((uint8_t)responseOpCode_c);
            rasCPResponse.cmdParameters.rspPayload.rspValue = ((uint8_t)gRasProcedureNotCompletedError_c);
            (void)sendCommandResponse(deviceId, (uint8_t*)&rasCPResponse, sizeof(rasCPResponse));
        }
    }

    if (pNotificationData != NULL)
    {
        (void)MEM_BufferFree(pNotificationData);
        pNotificationData = NULL;
    }

    return result;
}

/*!**********************************************************************************
* \brief        Sends notifications with the ranging data body
*
* \param[in]    deviceId            Identifier of the peer
* \param[in]    hValueRasNotif      Handle of the Subevent Ranging Data Characteristic
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_SendRangingDataNotifs
(
    deviceId_t  deviceId,
    uint16_t    hValueRasNotif
)
{
    uint16_t    availableLen = 0U;
    uint16_t    notifDataLen = 0U;
    static uint8_t segmentCounter = gRasSegmentCounterMinValue_c;
    uint8_t     segmentationHeader = 0U;
    bleResult_t result = gBleSuccess_c;
    uint16_t    res = gAttMaxNotifIndDataSize_d(gAttMtu[deviceId]);
    uint8_t*    pNotificationData = MEM_BufferAlloc((uint32_t)res);
    uint8_t     segmDataIdx = 0U;
    uint16_t    skippedDataSize = 0U;
    union
    {
        uint8_t  dataLen8;
        uint16_t dataLen16;
    } dataLen = {0U};

    if (Ras_CheckIfSubscribed(deviceId) == FALSE)
    {
        /* Client has unsubscribed */
        result = gBleInvalidState_c;
    }
    else if (pNotificationData != NULL)
    {
        while (mpRasServiceConfig->pData->totalSentRcvDataIndex < mpRasServiceConfig->pData->dataParsedLen)
        {
            FLib_MemSet(pNotificationData, 0U, ((uint32_t)res));

            notifDataLen = ((uint16_t)sizeof(uint8_t));
            availableLen = gAttMaxNotifIndDataSize_d(gAttMtu[deviceId]) - notifDataLen;
            segmentationHeader = (uint8_t)(segmentCounter << 2U);

            if (mpRasServiceConfig->pData->totalSentRcvDataIndex == 0U)
            {
                /* First notification */
                gRasSubeventStepIndex = 0U;
                mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].currentDataSize = 0U;
                segmentCounter = gRasSegmentCounterMinValue_c;
                segmentationHeader = (uint8_t)(segmentCounter << 2U);
                segmentationHeader |= (uint8_t)gRasNotifFirstSegment_c;
            }

            /* Set segment header */
            *pNotificationData = segmentationHeader;

            if (mbServRealTimeTransfer[deviceId] == FALSE)
            {
                maSegmentData[segmDataIdx].dataIdxStart = mpRasServiceConfig->pData->totalSentRcvDataIndex;
            }

            /* Compute how much data there is left in this subevent */
            uint16_t dataLeft = mpRasServiceConfig->pData->dataParsedLen - mpRasServiceConfig->pData->totalSentRcvDataIndex;

            if (dataLeft < availableLen)
            {
                availableLen = dataLeft;
            }

            /* Pack notification data */
            FLib_MemCpy((pNotificationData + notifDataLen),
                        (mpRasServiceConfig->pRangingDataBody + mpRasServiceConfig->pData->totalSentRcvDataIndex),
                        availableLen);
            mpRasServiceConfig->pData->totalSentRcvDataIndex += availableLen;
            notifDataLen += availableLen;

            /* We do not include step channel and step length in the ranging data body */
            dataLen.dataLen8 = mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].subevtHeader.numStepsReported;
            skippedDataSize = 2U*dataLen.dataLen16;

            if ((mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].dataSize - skippedDataSize +
                 (uint16_t)mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].dataIdx) <= mpRasServiceConfig->pData->totalSentRcvDataIndex)
            {
                /* If there are multiple subevents, move on to the next one */
                if (mpRasServiceConfig->pData->subeventIndex > 0U)
                {
                    gRasSubeventStepIndex++;
                }
            }

            /* Check if we reached the last notification */
            if ((mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].subevtHeader.procedureDoneStatus == 0U) &&
                (mpRasServiceConfig->pData->totalSentRcvDataIndex == mpRasServiceConfig->pData->dataParsedLen))
            {
                /* Last notification */
                segmentationHeader |= (uint8_t)gRasNotifLastSegment_c;
                gRasSubeventStepIndex = 0U;
                FLib_MemCpy(pNotificationData, &segmentationHeader, sizeof(uint8_t));
            }

            result = GattServer_SendInstantValueNotification(deviceId,
                                                             hValueRasNotif,
                                                             notifDataLen,
                                                             pNotificationData);

            if (mbServRealTimeTransfer[deviceId] == FALSE)
            {
                maSegmentData[segmDataIdx].segmentIdx = segmentationHeader;
                maSegmentData[segmDataIdx].dataSize = notifDataLen - 1U;
                segmDataIdx++;
            }

            if ((segmentCounter == (uint8_t)gRasSegmentCounterMaxValue_c) ||
                ((segmentationHeader & (uint8_t)gRasNotifLastSegment_c) != 0U))
            {
                segmentCounter = gRasSegmentCounterMinValue_c;
            }
            else
            {
                /* Update counters */
                segmentCounter++;
            }
        }
        (void)MEM_BufferFree(pNotificationData);
    }
    else
    {
        result = gBleOutOfMemory_c;
    }

    return result;
}

/*!**********************************************************************************
* \brief        Checks if peer wants to receive real time data
*
* \param[in]    deviceId            Identifier of the peer
*
* \return       TRUE if peer has configured real time data, false otherwise
************************************************************************************/
bool_t Ras_CheckRealTimeData
(
    deviceId_t  deviceId
)
{
    return mbServRealTimeTransfer[deviceId];
}

/*!**********************************************************************************
* \brief        Build the ranging data body from local HCI data
*
* \param[in]    deviceId         Identifier of the peer
* \param[in]    firstSubevt      TRUE if the ranging data body should include the procedure
*                                header, FALSE otherwise
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_BuildRangingDataBody
(
  deviceId_t deviceId,
  bool_t     firstSubevt
)
{
    uint16_t rangingDataLen = 0U;
    static uint16_t currentDataIdx = 0U;
    uint16_t availableLen = 0U;
    bleResult_t result = gBleSuccess_c;
    union
    {
        uint8_t  dataLen8;
        uint32_t dataLen32;
        uint16_t dataLen16;
    } dataLen = {0U};

    if (mpRasServiceConfig->pData == NULL)
    {
        result = gBleInvalidParameter_c;
    }

    if (result == gBleSuccess_c)
    {
        rangingDataLen = gRasCsSubeventDataSize_c;

        if (mbServRealTimeTransfer[deviceId] == FALSE)
        {
            if (mpRasServiceConfig->pRangingDataBody != NULL)
            {
                (void)MEM_BufferFree(mpRasServiceConfig->pRangingDataBody);
                mpRasServiceConfig->pRangingDataBody = NULL;
            }
            /* Compute total maximum length for the Ranging Data Body */
            dataLen.dataLen8 = mpRasServiceConfig->pData->subeventIndex + 1U;
            dataLen.dataLen32 *= sizeof(rasSubeventDataHeader_t);
            dataLen.dataLen32 += mpRasServiceConfig->pData->dataIndex + sizeof(rasRangingDataHeader_t);
            rangingDataLen = dataLen.dataLen16;
        }

        if (mpRasServiceConfig->pRangingDataBody == NULL)
        {
            mpRasServiceConfig->pRangingDataBody = MEM_BufferAlloc(rangingDataLen);
            if (mpRasServiceConfig->pRangingDataBody == NULL)
            {
                result = gBleOutOfMemory_c;
            }
        }
    }

    if (result == gBleSuccess_c)
    {
        /* pack procedure header */
        rasRangingDataHeader_t procHeader;
        availableLen = rangingDataLen;
        uint16_t dataIdxInit = mpRasServiceConfig->pData->totalSentRcvDataIndex;

        if (firstSubevt == TRUE)
        {
            currentDataIdx = 0U;
            gRasSubeventStepIndex = 0U;
            dataLen.dataLen32 = 0U;
            uint8_t devIdIdx = (deviceId * gAppMaxConnections_c) + gMode2Idx_c;

            /* Set first 16 bits = Procedure Counter (the lower 12 bits) + Configuration ID */
            procHeader.procCounterConfigId = ((mpRasServiceConfig->pData->procedureCounter << 4U) & 0xF0U);
            dataLen.dataLen8 = mpRasServiceConfig->pData->configId & 0x0FU;
            procHeader.procCounterConfigId |= dataLen.dataLen16;
            /* Set selected Tx power */
            procHeader.selectedTxPower = ((uint8_t)mpRasServiceConfig->pData->selectedTxPower);

            /* Set Antenna Paths Mask */
            procHeader.antennaPathsMask = 0U;

            if ((maModeFilters[devIdIdx] & BIT3) != 0U)
            {
                /* Antenna path 1 enabled */
                procHeader.antennaPathsMask |= BIT0;
            }
            if ((maModeFilters[devIdIdx] & BIT4) != 0U)
            {
                /* Antenna path 1 enabled */
                procHeader.antennaPathsMask |= BIT1;
            }
            if ((maModeFilters[devIdIdx] & BIT5) != 0U)
            {
                /* Antenna path 1 enabled */
                procHeader.antennaPathsMask |= BIT2;
            }
            if ((maModeFilters[devIdIdx] & BIT6) != 0U)
            {
                /* Antenna path 1 enabled */
                procHeader.antennaPathsMask |= BIT3;
            }

            FLib_MemCpy(mpRasServiceConfig->pRangingDataBody + currentDataIdx, &procHeader, sizeof(rasRangingDataHeader_t));
            currentDataIdx += ((uint16_t)sizeof(rasRangingDataHeader_t));
            availableLen -= ((uint16_t)sizeof(rasRangingDataHeader_t));
        }

        /* Pack subevent header */
        uint16_t crtSubevtDataIdx = (uint16_t)mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].dataIdx;
        packSubeventHeader(mpRasServiceConfig->pRangingDataBody + currentDataIdx);
        currentDataIdx += ((uint16_t)sizeof(rasSubeventDataHeader_t));
        availableLen -= ((uint16_t)sizeof(rasSubeventDataHeader_t));

        /* Pack rangin data body */
        mpRasServiceConfig->pData->totalSentRcvDataIndex = crtSubevtDataIdx;
        antennaPathFilterStepData(deviceId, mpRasServiceConfig->pData->pData + crtSubevtDataIdx,
                                  mpRasServiceConfig->pRangingDataBody, availableLen, &currentDataIdx);

        /* Reset data legth counters */
        mpRasServiceConfig->pData->totalSentRcvDataIndex = dataIdxInit;
        mpRasServiceConfig->pData->dataParsedLen = currentDataIdx;
    }

    return result;
}

/*!**********************************************************************************
* \brief        Check if the peer has subscribed to either real-time or on-demand data.
*
* \param[in]    deviceId         Identifier of the peer
*
* \return       TRUE if the peer has subscribed, FALSE otherwise
************************************************************************************/
bool_t Ras_CheckIfSubscribed
(
    deviceId_t deviceId
)
{
    bool_t result = FALSE;

    for (uint8_t idx = 0; idx < gAppMaxConnections_c; idx++)
    {
        if (mRas_ClientDeviceId[idx] == deviceId)
        {
            result = TRUE;
            break;
        }
    }

    return result;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*!**********************************************************************************
* \brief         Copies the current subevent header in the provided ranging data body
*
* \param[in\out] pRangingDataBody   Pointer to the ranging data body
*
* \return       none
************************************************************************************/
static void packSubeventHeader
(
    uint8_t* pRangingDataBody
)
{
    rasSubeventDataHeader_t subeventHeader;
    csRasSubeventHeader_t *pSubevtHeader = &mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].subevtHeader;

    subeventHeader.startACLConnEvent = pSubevtHeader->startACLConnEvent;
    subeventHeader.frequencyCompensation = pSubevtHeader->frequencyCompensation;
    subeventHeader.procAndSubeventDoneStatus = ((pSubevtHeader->procedureDoneStatus << 4U) & 0xF0U) | (pSubevtHeader->subeventDoneStatus & 0x0FU);
    subeventHeader.procAndSubeventAbortReason = pSubevtHeader->abortReason;
    subeventHeader.referencePowerLevel = pSubevtHeader->referencePowerLevel;
    subeventHeader.numStepsReported = pSubevtHeader->numStepsReported;

    FLib_MemCpy(pRangingDataBody, &subeventHeader, sizeof(subeventHeader));
}

/*!**********************************************************************************
* \brief        Filter notification data according to the received Antenna Path Filter
*
* \param[in]    deviceId      Peer identifier
* \param[in]    pEventData    Pointer to procedure data to be filtered
* \param[in]    pNotifData    Pointer to notification data where to store fitlered data
* \param[in]    maxLength     Maximum length of data to be copied
* \param[in]    pOutLen       Length of the stored data
*
* \return       none
************************************************************************************/
static void antennaPathFilterStepData
(
    deviceId_t deviceId,
    uint8_t*   pEventData,
    uint8_t*   pNotifData,
    uint16_t   maxLength,
    uint16_t*  pOutLen
)
{
    /* Save data pointer to event data */
    uint8_t* pStepDataAux = pEventData;
    /* Data structure: step mode, step channel, step data length, step data */
    uint8_t mode = 0U;
    uint8_t stepDataLength = 0U;
    uint8_t antPermIndex = 0U;
    uint8_t antIdx = 0U;
    uint8_t devIdIdx = deviceId * gAppMaxConnections_c;
    const uint8_t *antIndex_p = NULL;
    union
    {
        uint8_t stepLen8;
        uint16_t stepLen16;
    } stepLen = {0U};

    while (*pOutLen < maxLength)
    {
        if (mpRasServiceConfig->pData->totalSentRcvDataIndex >= mpRasServiceConfig->pData->dataIndex)
        {
            /* we reached the end of the data */
            break;
        }

        /* Step mode value */
        mode = *pStepDataAux++;
        /* Skip step channel */
        pStepDataAux++;
        /* Step data length and step data */
        stepDataLength = *pStepDataAux++;

        assert(mode < 4U);

        if ((mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].subevtHeader.subeventDoneStatus != 0U) &&
            (mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].subevtHeader.subeventDoneStatus != 1U))
        {
            /* Set BIT7 to 1 to mark step as aborted */
            mode |= BIT7;
        }
        /* Copy step mode information to notification buffer */
        *(pNotifData + (*pOutLen)) = mode;
        (*pOutLen)++;

        devIdIdx = deviceId * gAppMaxConnections_c;

        switch (mode)
        {
            /* Mode 0 data: Packet AA Quality, RSSI, Packet Antenna, Measured frequency offset */
            case 0U:
            {
                devIdIdx += gMode0Idx_c;

                if ((maModeFilters[devIdIdx] & BIT0) != 0U)
                {
                    /* Copy packet quality information */
                    *(pNotifData + (*pOutLen)) = *pStepDataAux;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint8_t)];

                if ((maModeFilters[devIdIdx] & BIT1) != 0U)
                {
                    /* Copy packet RSSI information */
                    *(pNotifData + (*pOutLen)) = *pStepDataAux;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint8_t)];

                if ((maModeFilters[devIdIdx] & BIT2) != 0U)
                {
                    /* Copy packet antenna information */
                    *(pNotifData + (*pOutLen)) = *pStepDataAux;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint8_t)];

                if (stepDataLength > 3U)
                {
                    if ((maModeFilters[devIdIdx] & BIT3) != 0U)
                    {
                        /* Copy measured frequency offset information */
                        FLib_MemCpy(pNotifData + (*pOutLen), pStepDataAux, sizeof(uint16_t));
                        (*pOutLen) += (uint16_t)sizeof(uint16_t);
                    }
                    pStepDataAux = &pStepDataAux[sizeof(uint16_t)];
                }
            }
            break;

            /* Mode 1 data: Packet AA Quality, NADM, RSSI, ToA ToD Initiator/Reflector, Packet Antenna,
             *             (Packet PCT1 and Packet PCT2 if RTT type contains a sounding sequence) */
            case 1U:
            {
                devIdIdx += gMode1Idx_c;

                if ((maModeFilters[devIdIdx] & BIT0) != 0U)
                {
                    /* Copy packet quality information */
                    *(pNotifData + (*pOutLen)) = *pStepDataAux;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);

                }
                pStepDataAux = &pStepDataAux[sizeof(uint8_t)];

                if ((maModeFilters[devIdIdx] & BIT1) != 0U)
                {
                    /* Copy packet NADM information */
                    *(pNotifData + (*pOutLen)) = *pStepDataAux;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint8_t)];

                if ((maModeFilters[devIdIdx] & BIT2) != 0U)
                {
                    /* Copy packet RSSI information */
                    *(pNotifData + (*pOutLen)) = *pStepDataAux;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint8_t)];

                if ((maModeFilters[devIdIdx] & BIT3) != 0U)
                {
                    /* Copy packet ToA ToD information */
                    FLib_MemCpy(pNotifData + (*pOutLen), pStepDataAux, sizeof(uint16_t));
                    (*pOutLen) += (uint16_t)sizeof(uint16_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint16_t)];

                if ((maModeFilters[devIdIdx] & BIT4) != 0U)
                {
                    /* Copy packet antenna information */
                    *(pNotifData + (*pOutLen)) = *pStepDataAux;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint8_t)];

                if (stepDataLength > 6U)
                {
                    if ((maModeFilters[devIdIdx] & BIT5) != 0U)
                    {
                        /* Copy packet PCT1 information */
                        FLib_MemCpy(pNotifData + (*pOutLen), pStepDataAux, gPacket_PCTSize_c);
                        (*pOutLen) += (uint16_t)gPacket_PCTSize_c;
                    }
                    pStepDataAux = &pStepDataAux[gPacket_PCTSize_c];

                    if ((maModeFilters[devIdIdx] & BIT6) != 0U)
                    {
                        /* Copy packet PCT2 information */
                        FLib_MemCpy(pNotifData + (*pOutLen), pStepDataAux, gPacket_PCTSize_c);
                        (*pOutLen) += (uint16_t)gPacket_PCTSize_c;
                    }
                    pStepDataAux = &pStepDataAux[gPacket_PCTSize_c];
                }
            }
            break;

            /* Mode 2 data: Antenna Permutation Index, Tone PCT[k], Tone Quality Indicator[k] */
            case 2U:
            {
                devIdIdx += gMode2Idx_c;

                antPermIndex = *pStepDataAux++;
                assert(antPermIndex < 25);
                antIndex_p = &maAntPermNAp[antPermIndex][0];

                if ((maModeFilters[devIdIdx] & BIT0) != 0U)
                {
                    /* Copy Antenna Permutation Index information */
                    *(pNotifData + (*pOutLen)) = antPermIndex;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);
                }

                for (uint8_t idx = 0U; idx <= mpRasServiceConfig->pData->numAntennaPaths; idx++)
                {
                    if (idx < mpRasServiceConfig->pData->numAntennaPaths)
                    {
                        antIdx = antIndex_p[idx];
                    }
                    else
                    {
                        /* extension slot - repeat last antenna path in the switching sequence */
                        antIdx = antIndex_p[mpRasServiceConfig->pData->numAntennaPaths - 1U];
                    }

                    /* Check if the corresponding Antenna Path is enabled */
                    if (((antIdx == 0U) && ((maModeFilters[devIdIdx] & BIT3) != 0U)) ||
                        ((antIdx == 1U) && ((maModeFilters[devIdIdx] & BIT4) != 0U)) ||
                        ((antIdx == 2U) && ((maModeFilters[devIdIdx] & BIT5) != 0U)) ||
                        ((antIdx == 3U) && ((maModeFilters[devIdIdx] & BIT6) != 0U)))
                    {
                        if ((maModeFilters[devIdIdx] & BIT1) != 0U)
                        {
                            /* Copy Tone_PCT information in IQ format */
                            FLib_MemCpy(pNotifData + (*pOutLen), pStepDataAux, gTone_PCTSize_c);

                            (*pOutLen) += (uint16_t)gTone_PCTSize_c;
                        }
                        pStepDataAux = &pStepDataAux[gTone_PCTSize_c];

                        if ((maModeFilters[devIdIdx] & BIT2) != 0U)
                        {
                            /* Copy Tone_PCT information */
                            *(pNotifData + (*pOutLen)) = *pStepDataAux;
                            (*pOutLen) += (uint16_t)sizeof(uint8_t);
                        }
                        pStepDataAux = &pStepDataAux[sizeof(uint8_t)];
                    }
                    else
                    {
                        pStepDataAux = &pStepDataAux[sizeof(uint8_t) + gTone_PCTSize_c];
                    }
                }
            }
            break;

            /* Mode 3 data: AA Quality, NADM, RSSI, ToA ToD Initiator/Reflector, Packet Antenna,
             *              (Packet PCT1 and Packet PCT2 if RTT type contains a sounding sequence),
             *              Antenna Permutation Index, Tone PCT[k], Tone Quality Indicator[k]
             * */
            case 3U:
            {
                devIdIdx += gMode3Idx_c;

                if ((maModeFilters[devIdIdx] & BIT0) != 0U)
                {
                    /* Copy AA Quality information */
                    *(pNotifData + (*pOutLen)) = *pStepDataAux;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint8_t)];

                if ((maModeFilters[devIdIdx] & BIT1) != 0U)
                {
                    /* Copy NADM information */
                    *(pNotifData + (*pOutLen)) = *pStepDataAux;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint8_t)];

                if ((maModeFilters[devIdIdx] & BIT2) != 0U)
                {
                    /* Copy RSSI information */
                    *(pNotifData + (*pOutLen)) = *pStepDataAux;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint8_t)];

                if ((maModeFilters[devIdIdx] & BIT3) != 0U)
                {
                    /* Copy packet ToA ToD information */
                     FLib_MemCpy(pNotifData + (*pOutLen), pStepDataAux, sizeof(uint16_t));
                    (*pOutLen) += (uint16_t)sizeof(uint16_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint16_t)];

                if ((maModeFilters[devIdIdx] & BIT4) != 0U)
                {
                    /* Copy Antenna information */
                    *(pNotifData + (*pOutLen)) = *pStepDataAux;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint8_t)];

                if (stepDataLength > (7U + ((1U + mpRasServiceConfig->pData->numAntennaPaths) * 4U)))
                {
                    if ((maModeFilters[devIdIdx] & BIT5) != 0U)
                    {
                        /* Copy packet PCT1 information */
                        FLib_MemCpy(pNotifData + (*pOutLen), pStepDataAux, gPacket_PCTSize_c);
                        (*pOutLen) += (uint16_t)gPacket_PCTSize_c;
                    }
                    pStepDataAux = &pStepDataAux[gPacket_PCTSize_c];

                    if ((maModeFilters[devIdIdx] & BIT6) != 0U)
                    {
                        /* Copy packet PCT2 information */
                        FLib_MemCpy(pNotifData + (*pOutLen), pStepDataAux, gPacket_PCTSize_c);
                        (*pOutLen) += (uint16_t)gPacket_PCTSize_c;
                    }
                    pStepDataAux = &pStepDataAux[gPacket_PCTSize_c];
                }

                antPermIndex = *pStepDataAux;
                antIndex_p = &maAntPermNAp[antPermIndex][0];

                if ((maModeFilters[devIdIdx] & BIT7) != 0U)
                {
                    /* Copy Antenna Permutation Index information */
                    *(pNotifData + (*pOutLen)) = *pStepDataAux;
                    (*pOutLen) += (uint16_t)sizeof(uint8_t);
                }
                pStepDataAux = &pStepDataAux[sizeof(uint8_t)];

                for (uint8_t idx = 0U; idx <= mpRasServiceConfig->pData->numAntennaPaths; idx++)
                {
                    antIdx = antIndex_p[idx];

                    /* Check if the corresponding Antenna Path is enabled */
                    if (((antIdx == 0U) && ((maModeFilters[devIdIdx] & BIT10) != 0U)) ||
                        ((antIdx == 1U) && ((maModeFilters[devIdIdx] & BIT11) != 0U)) ||
                        ((antIdx == 2U) && ((maModeFilters[devIdIdx] & BIT12) != 0U)) ||
                        ((antIdx == 3U) && ((maModeFilters[devIdIdx] & BIT13) != 0U)))
                    {
                        if ((maModeFilters[devIdIdx] & BIT8) != 0U)
                        {
                            /* Copy Tone_PCT information in IQ format */
                            FLib_MemCpy(pNotifData + (*pOutLen), pStepDataAux, gTone_PCTSize_c);

                            (*pOutLen) += (uint16_t)gTone_PCTSize_c;
                        }
                        pStepDataAux = &pStepDataAux[gTone_PCTSize_c];

                        if ((maModeFilters[devIdIdx] & BIT9) != 0U)
                        {
                            /* Copy Tone_PCT information */
                            *(pNotifData + (*pOutLen)) = *pStepDataAux;
                            (*pOutLen) += (uint16_t)sizeof(uint8_t);
                        }
                        pStepDataAux = &pStepDataAux[sizeof(uint8_t)];
                    }
                }
            }
            break;

            default:
            {
                ; /* should not get here - break the loop */
            }
            break;
        }

        /* Account for the data we parsed so far: step mode, channel, length and data */
        stepLen.stepLen8 = stepDataLength;
        mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].currentDataSize += stepLen.stepLen16 + 3U;
        mpRasServiceConfig->pData->totalSentRcvDataIndex += stepLen.stepLen16 + 3U;

        /* Chek if we reached the end of the subevent */
        if (mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].currentDataSize ==
            mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].dataSize)
        {
            gRasSubeventStepIndex++;

            /* Copy the next subevent header */
            if (((uint8_t)gRasSubeventStepIndex < mpRasServiceConfig->pData->subeventIndex+1U) &&
                (mpRasServiceConfig->pData->aSubEventData[gRasSubeventStepIndex].subevtHeader.procedureDoneStatus == 0U) )
            {
                packSubeventHeader(pNotifData + (*pOutLen));
                (*pOutLen) += ((uint16_t)sizeof(rasSubeventDataHeader_t));
            }
        }
    }
}

/*!**********************************************************************************
* \brief        Sends a command response to the RREQ
*
* \param[in]    deviceId       Identifier of the peer
* \param[in]    pNotifPayload  Pointer to response data
* \param[in]    valueLength    Length of response
*
* \return       gBleSuccess_c or error
************************************************************************************/
static bleResult_t sendCommandResponse
(
    deviceId_t deviceId,
    uint8_t* pNotifPayload,
    uint32_t valueLength
)
{
    /* Send notification with command result to the peer */
    if ((mNotifIndEnabled[deviceId] & BIT1) != 0U)
    {
        return GattServer_SendInstantValueIndication(deviceId,
                                                     mpRasServiceConfig->controlPointHandle,
                                                     ((uint16_t)valueLength),
                                                     pNotifPayload);
    }
    else
    {
        return GattServer_SendInstantValueNotification(deviceId,
                                                       mpRasServiceConfig->controlPointHandle,
                                                       ((uint16_t)valueLength),
                                                       pNotifPayload);
    }
}

/*!**********************************************************************************
* \brief        Helper function to send the segments requested through the
*               Retrieve Lost Ranging Data Segments procedure
*
* \param[in]    deviceId         Peer Identifier
* \param[in]    startAbsSegment  Index of the first segment
* \param[in]    endAbsSegment    Index of the last segment
*
* \return       gBleSuccess_c or error
************************************************************************************/
static bleResult_t handleGetRangingDataSegmNotif
(
    deviceId_t deviceId,
    uint8_t    startAbsSegment,
    uint8_t    endAbsSegment
)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t     crtSegmIdx = 0U;
    uint16_t    res = gAttMaxNotifIndDataSize_d(gAttMtu[deviceId]);
    uint8_t*    pNotificationData = MEM_BufferAlloc((uint32_t)res);
    union
    {
        uint32_t dataLen32;
        uint16_t dataLen16;
    } dataLen = {0U};

    if (Ras_CheckIfSubscribed(deviceId) == FALSE)
    {
        /* Client has unsubscribed */
        result = gBleInvalidState_c;
    }
    else if (pNotificationData != NULL)
    {
        while ((result == gBleSuccess_c) && (startAbsSegment <= endAbsSegment))
        {
            for (uint8_t idx = 0U; idx < gRASMaxNoOfSegments_c; idx++)
            {
                if (maSegmentData[idx].segmentIdx == startAbsSegment)
                {
                    crtSegmIdx = idx;

                    if ((idx + 1U) < gRASMaxNoOfSegments_c)
                    {
                        if ( maSegmentData[idx + 1U].segmentIdx != 0U)
                        {
                            startAbsSegment = maSegmentData[idx + 1U].segmentIdx;
                        }
                        else
                        {
                            /* It's the last segment, we must exit the loop */
                            endAbsSegment = startAbsSegment - 1U;
                            maLostSegmRecvIdx[deviceId*3U + 1U] = startAbsSegment;
                        }
                    }
                    break;
                }
            }
            assert (crtSegmIdx < gRASMaxNoOfSegments_c);
            segmentData_t *pData = &maSegmentData[crtSegmIdx];

            /* Set segment header */
            *pNotificationData = pData->segmentIdx;

            /* Pack notification data */
            FLib_MemCpy((pNotificationData + sizeof(uint8_t)),
                        mpRasServiceConfig->pRangingDataBody + pData->dataIdxStart,
                        pData->dataSize);

            dataLen.dataLen32 = pData->dataSize + sizeof(uint8_t);
            /* Send instant notification instead of writing the data in the database */
            result = GattServer_SendInstantValueNotification(deviceId,
                                                             mpRasServiceConfig->onDemandDataHandle,
                                                             dataLen.dataLen16,
                                                             pNotificationData);
        }
    }
    else
    {
        result = gBleOutOfMemory_c;
    }

    return result;
}

/*!**********************************************************************************
* \brief        Ranging Profile Timer Callback
*
* \param[in]    param         Peer Identifier
*
* \return       none
************************************************************************************/
static void RrspTimerCallback
(
    void *param
)
{
    deviceId_t deviceId = *(deviceId_t*)param;

     /* Reset the result events buffer and start accepting data from a new CS procedure */
    FLib_MemSet(maSegmentData, 0U, gRASMaxNoOfSegments_c * sizeof(segmentData_t));

    mDataTransferState[deviceId] = (uint8_t)noTransferInProgress_c;
    gRasSubeventStepIndex = 0U;

    FLib_MemSet(mpRasServiceConfig->pData, 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
    FLib_MemSet(mpRasServiceConfig->pData->pData, 0U, gRasCsSubeventDataSize_c);
    (void)MEM_BufferFree(mpRasServiceConfig->pRangingDataBody);
    mpRasServiceConfig->pRangingDataBody = NULL;

}

/*!**********************************************************************************
* \brief        Checks the validity of the start and end segment values received as
*               parameters for the Retrieve Lost Ranging Data Segments command
*
* \param[in]    deviceId    Peer identifier
* \param[in]    startSegm   Start segment parameter value
* \param[in]    endSegm     End segment parameter value
*
* \return       TRUE if the received values are valid, FALSE otherwise
************************************************************************************/
static bool_t checkStartEndSegm
(
    deviceId_t deviceId,
    uint16_t startSegm,
    uint16_t endSegm
)
{
    bool_t result = FALSE;
    bool_t foundStart = FALSE;

    for (uint8_t idx = 0; idx < gRASMaxNoOfSegments_c; idx++)
    {
        if (maSegmentData[idx].segmentIdx == startSegm)
        {
            foundStart = TRUE;
        }

        if (foundStart == TRUE)
        {
            if ((maSegmentData[idx].segmentIdx == endSegm) ||(endSegm == 0xFFU))
            {
                result = TRUE;
            }
        }
    }

    return result;
}

/*!**********************************************************************************
* \brief        Checks if a RAS error occured and sends a response with the
*               provided error code.
*
* \param[in]    deviceId    Peer identifier
* \param[in]    rasStatus   Ras status value
*
* \return       none
************************************************************************************/
static void checkRasStatus
(
    deviceId_t deviceId,
    rasControlPointRspCodeValues_tag rasStatus
)
{
    rasControlPointRsp_t rasCPResponse;

    if (rasStatus != gRasSuccess_c)
    {
        /* An error occured in the handling of a command - reset the ranging data body */
        if (mpRasServiceConfig->pRangingDataBody != NULL)
        {
            (void)MEM_BufferFree(mpRasServiceConfig->pRangingDataBody);
            mpRasServiceConfig->pRangingDataBody = NULL;
        }

        /* Send response with the received error code */
        rasCPResponse.cmdParameters.rspPayload.rspValue = ((uint8_t)rasStatus);
        rasCPResponse.rspOpCode = ((uint8_t)responseOpCode_c);
        (void)sendCommandResponse(deviceId, (uint8_t*)&rasCPResponse, sizeof(rasCPResponse));
    }
}

/*!**********************************************************************************
* \brief        Helper function to handle the Get Ranging Data command
*
* \param[in]    deviceId            Peer Identifier
* \param[in]    cmdLen              Command length
* \param[in]    pRasCtrlPointCmd    Pointer to the RAS CP command
*
* \return       gBleSuccess_c or error
************************************************************************************/
static bleResult_t handleGetRangingDataCmd
(
    deviceId_t deviceId,
    uint16_t   cmdLen,
    rasControlPointReq_t *pRasCtrlPointCmd
)
{
    rasControlPointRsp_t rasCPResponse;
    rasControlPointRspCodeValues_tag rasStatus = gRasSuccess_c;
    bleResult_t result = gBleSuccess_c;

    if ((mpRasServiceConfig->pData == NULL) || (mpRasServiceConfig->pData->pData == NULL) ||
        (pRasCtrlPointCmd->cmdParameters.procCounter != mpRasServiceConfig->pData->procedureCounter))
    {
        /* Check that the received procedure counter matches the local one */
        rasStatus = gRasNoRecordsFoundError_c;
        result = gBleInvalidParameter_c;
    }
    else if (cmdLen != maRasCPCommandSizes[getRangingDataOpCode_c])
    {
        /* Check command length */
        rasStatus = gRasInvalidParameterError_c;
        result = gBleInvalidParameter_c;
    }
    else
    {
        /* Received a Get Ranging Data Command */
        Ras_ClearDataForTransfer(deviceId);

        if ((mNotifIndEnabled[deviceId] & BIT0) == 0U)
        {
            /* Send Ranging data through notifications */
            result = Ras_SendRangingDataNotifs(deviceId, mpRasServiceConfig->onDemandDataHandle);

            mDataTransferState[deviceId] = (uint8_t)waitingAckRangingData_c;
            if (result != gBleSuccess_c)
            {
                /* Send error response */
                rasCPResponse.rspOpCode = ((uint8_t)responseOpCode_c);
                rasCPResponse.cmdParameters.rspPayload.rspValue = ((uint8_t)gRasProcedureNotCompletedError_c);
                result = sendCommandResponse(deviceId, (uint8_t*)&rasCPResponse, sizeof(rasCPResponse));
            }
            else
            {
                /* Send response to Get Report Records procedure */
                rasCPResponse.rspOpCode = ((uint8_t)completeProcDataRspOpCode_c);
                rasCPResponse.cmdParameters.procCounter = ((uint8_t)pRasCtrlPointCmd->cmdParameters.procCounter);
                result = sendCommandResponse(deviceId, (uint8_t*)&rasCPResponse, sizeof(rasCPResponse));
                mDeviceId = deviceId;
                (void)TM_InstallCallback((timer_handle_t)mRrspTimerId, RrspTimerCallback, &mDeviceId);
                (void)TM_Start((timer_handle_t)mRrspTimerId,
                               (uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSingleShot | (uint8_t)kTimerModeSetSecondTimer,
                               gRsspTimeoutCompleteSeconds_c);
            }
        }
        else
        {
            /* Send Ranging data through indications */
            mpRasServiceConfig->pData->totalSentRcvDataIndex = 0U;
            result = Ras_SendRangingDataIndication(deviceId,
                                                   mpRasServiceConfig->onDemandDataHandle);
        }
    }

    checkRasStatus(deviceId, rasStatus);

    return result;
}

/*!**********************************************************************************
* \brief        Helper function to handle the Ack Ranging Data command
*
* \param[in]    deviceId            Peer Identifier
* \param[in]    cmdLen              Command length
* \param[in]    pRasCtrlPointCmd    Pointer to the RAS CP command
*
* \return       gBleSuccess_c or error
************************************************************************************/
static bleResult_t handleAckRangingDataCmd
(
    deviceId_t deviceId,
    uint16_t   cmdLen,
    rasControlPointReq_t *pRasCtrlPointCmd
)
{
    rasControlPointRsp_t rasCPResponse;
    rasControlPointRspCodeValues_tag rasStatus = gRasSuccess_c;
    bleResult_t result = gBleSuccess_c;

    if ((mpRasServiceConfig->pData == NULL) || (mpRasServiceConfig->pData->pData == NULL))
    {
        rasStatus = gRasNoRecordsFoundError_c;
        result = gBleInvalidParameter_c;
    }
    else if (cmdLen != maRasCPCommandSizes[ackRangingDataOpCode_c])
    {
        /* Check command length */
        rasStatus = gRasInvalidParameterError_c;
        result = gBleInvalidParameter_c;
    }
    else
    {
        (void)TM_Stop((timer_handle_t)mRrspTimerId);

        /* Reset the result events buffer and start accepting data from a new CS procedure */
        FLib_MemSet(maSegmentData, 0U, gRASMaxNoOfSegments_c * sizeof(segmentData_t));

        mDataTransferState[deviceId] = (uint8_t)noTransferInProgress_c;
        gRasSubeventStepIndex = 0U;
        /* Check that the received procedure counter matches the local one */
        if (pRasCtrlPointCmd->cmdParameters.procCounter == mpRasServiceConfig->pData->procedureCounter)
        {
            FLib_MemSet(mpRasServiceConfig->pData, 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
            FLib_MemSet(mpRasServiceConfig->pData->pData, 0U, gRasCsSubeventDataSize_c);
            (void)MEM_BufferFree(mpRasServiceConfig->pRangingDataBody);
            mpRasServiceConfig->pRangingDataBody = NULL;

            /* Send response to the ACK Stored Records Command */
            rasCPResponse.rspOpCode = ((uint8_t)responseOpCode_c);
            rasCPResponse.cmdParameters.rspPayload.rspValue = ((uint8_t)gRasSuccess_c);
            result = sendCommandResponse(deviceId, (uint8_t*)&rasCPResponse, sizeof(rasCPResponse));
        }
        else
        {
            rasStatus = gRasNoRecordsFoundError_c;
            result = gBleInvalidParameter_c;
        }
    }

    checkRasStatus(deviceId, rasStatus);

    return result;
}

/*!**********************************************************************************
* \brief        Helper function to handle the Abort command
*
* \param[in]    deviceId            Peer Identifier
* \param[in]    cmdLen              Command length
* \param[in]    pRasCtrlPointCmd    Pointer to the RAS CP command
*
* \return       gBleSuccess_c or error
************************************************************************************/
static bleResult_t handleAbortCmd
(
    deviceId_t deviceId,
    uint16_t   cmdLen,
    rasControlPointReq_t *pRasCtrlPointCmd
)
{
    rasControlPointRsp_t rasCPResponse;
    rasControlPointRspCodeValues_tag rasStatus = gRasSuccess_c;
    bleResult_t result = gBleSuccess_c;

    if (cmdLen != maRasCPCommandSizes[abortOperationOpCode_c])
    {
        /* Check command length */
        rasStatus = gRasInvalidParameterError_c;
        result = gBleInvalidParameter_c;
    }
    else
    {
        (void)TM_Stop((timer_handle_t)mRrspTimerId);

        /* Reset the result events buffer */
        FLib_MemSet(maSegmentData, 0U, gRASMaxNoOfSegments_c * sizeof(segmentData_t));
        FLib_MemSet(mpRasServiceConfig->pData, 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
        FLib_MemSet(mpRasServiceConfig->pData->pData, 0U, gRasCsSubeventDataSize_c);
        if (mpRasServiceConfig->pRangingDataBody != NULL)
        {
            (void)MEM_BufferFree(mpRasServiceConfig->pRangingDataBody);
            mpRasServiceConfig->pRangingDataBody = NULL;
        }

        mDataTransferState[deviceId] = (uint8_t)noTransferInProgress_c;
        gRasSubeventStepIndex = 0U;

        /* Send response to Data Abort Command */
        rasCPResponse.rspOpCode = ((uint8_t)responseOpCode_c);
        rasCPResponse.cmdParameters.rspPayload.rspValue = ((uint8_t)gRasSuccess_c);
        result = sendCommandResponse(deviceId, (uint8_t*)&rasCPResponse, sizeof(rasCPResponse));
    }

    checkRasStatus(deviceId, rasStatus);

    return result;
}

/*!**********************************************************************************
* \brief        Helper function to handle the Retrieve Lost Ranging Data Segments command
*
* \param[in]    deviceId            Peer Identifier
* \param[in]    cmdLen              Command length
* \param[in]    pRasCtrlPointCmd    Pointer to the RAS CP command
*
* \return       gBleSuccess_c or error
************************************************************************************/
static bleResult_t handleRetrLostSegmentsCmd
(
    deviceId_t deviceId,
    uint16_t   cmdLen,
    rasControlPointReq_t *pRasCtrlPointCmd
)
{
    rasControlPointRsp_t rasCPResponse;
    rasControlPointRspCodeValues_tag rasStatus = gRasSuccess_c;
    bleResult_t result = gBleSuccess_c;

    if ((mpRasServiceConfig->pData == NULL) ||
        (pRasCtrlPointCmd->cmdParameters.retrLostDataParams.procCounter!= mpRasServiceConfig->pData->procedureCounter))
    {
        /* Check that the received procedure counter matches the local one */
        rasStatus = gRasNoRecordsFoundError_c;
        result = gBleInvalidParameter_c;
    }
    else if (cmdLen != maRasCPCommandSizes[retrLostRangingDataOpCode_c])
    {
        /* Check command length */
        rasStatus = gRasInvalidParameterError_c;
        result = gBleInvalidParameter_c;
    }
    else if ((mDataTransferState[deviceId] != (uint8_t)waitingAckRangingData_c) &&
             (mDataTransferState[deviceId] != (uint8_t)waitAckRangingDataRetransm_c))
    {
        /* Check state - this command is only valid if we sent all segments and are waiting
           for the Ack Ranging Data command */
        if (mDataTransferState[deviceId] == (uint8_t)sendingRangingData_c)
        {
            rasStatus = gRasServerBusyError_c;
            result = gBleInvalidParameter_c;
        }
    }
    else
    {
        /* Check start and end segment parameters */
        bool_t validParams = checkStartEndSegm(deviceId,
                                               pRasCtrlPointCmd->cmdParameters.retrLostDataParams.startAbsSegment,
                                               pRasCtrlPointCmd->cmdParameters.retrLostDataParams.endAbsSegment);

        if (validParams)
        {
            /* Valid request - stop response timer */
            (void)TM_Stop((timer_handle_t)mRrspTimerId);

            mDataTransferState[deviceId] = (uint8_t)segmRetransmInProgress_c;
            maLostSegmRecvIdx[deviceId*3U] = pRasCtrlPointCmd->cmdParameters.retrLostDataParams.startAbsSegment;
            maLostSegmRecvIdx[deviceId*3U + 2U] = pRasCtrlPointCmd->cmdParameters.retrLostDataParams.startAbsSegment;
            maLostSegmRecvIdx[deviceId*3U + 1U] = pRasCtrlPointCmd->cmdParameters.retrLostDataParams.endAbsSegment;
            if ((mNotifIndEnabled[deviceId] & BIT0) == 0U)
            {
                result = handleGetRangingDataSegmNotif(deviceId,
                                                       pRasCtrlPointCmd->cmdParameters.retrLostDataParams.startAbsSegment,
                                                       pRasCtrlPointCmd->cmdParameters.retrLostDataParams.endAbsSegment);
                if (result == gBleSuccess_c)
                {
                    /* Send response to the Retrieve Lost Ranging Data Segments procedure */
                    mDataTransferState[deviceId] = (uint8_t)waitAckRangingDataRetransm_c;
                    rasCPResponse.rspOpCode = ((uint8_t)completeLostDataSegmentResponseOpCode_c);

                    rasCPResponse.cmdParameters.complLostRsp.procCounter = pRasCtrlPointCmd->cmdParameters.retrLostDataParams.procCounter;
                    rasCPResponse.cmdParameters.complLostRsp.startAbsSegment = pRasCtrlPointCmd->cmdParameters.retrLostDataParams.startAbsSegment;
                    /* Fill start segment and end segmet value with the indeces of the segments sent */
                    if (pRasCtrlPointCmd->cmdParameters.retrLostDataParams.endAbsSegment == 0xFFU)
                    {
                        rasCPResponse.cmdParameters.complLostRsp.endAbsSegment = maLostSegmRecvIdx[deviceId*3U + 1U];
                    }
                    else
                    {
                        rasCPResponse.cmdParameters.complLostRsp.endAbsSegment = pRasCtrlPointCmd->cmdParameters.retrLostDataParams.endAbsSegment;
                    }

                    result = sendCommandResponse(deviceId, (uint8_t*)&rasCPResponse, sizeof(rasCPResponse));

                    /* Restart Timer to wait for response */
                    (void)TM_InstallCallback((timer_handle_t)mRrspTimerId, RrspTimerCallback, &mDeviceId);
                    (void)TM_Start((timer_handle_t)mRrspTimerId,
                                   (uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSingleShot | (uint8_t)kTimerModeSetSecondTimer,
                                   gRsspTimeoutCompleteSeconds_c);
                }
                else
                {
                    rasStatus = gRasProcedureNotCompletedError_c;
                }
            }
            else
            {
                result = Ras_HandleGetRangingDataSegmInd(deviceId);
            }
        }
        else
        {
            rasStatus = gRasNoRecordsFoundError_c;
            result = gBleInvalidParameter_c;
        }
    }

    checkRasStatus(deviceId, rasStatus);

    return result;
}

/*!**********************************************************************************
* \brief        Helper function to handle the Set Filter command
*
* \param[in]    deviceId            Peer Identifier
* \param[in]    cmdLen              Command length
* \param[in]    pRasCtrlPointCmd    Pointer to the RAS CP command
*
* \return       gBleSuccess_c or error
************************************************************************************/
static bleResult_t handleSetFilterCmd
(
    deviceId_t deviceId,
    uint16_t   cmdLen,
    rasControlPointReq_t *pRasCtrlPointCmd
)
{
    rasControlPointRsp_t rasCPResponse;
    rasControlPointRspCodeValues_tag rasStatus = gRasSuccess_c;
    bleResult_t result = gBleSuccess_c;

    if (cmdLen != maRasCPCommandSizes[setFilterOpCode_c])
    {
        /* Not allowed to set filter after enabling notifications/indications */
        rasStatus = gRasInvalidParameterError_c;
        result = gBleInvalidParameter_c;
    }
    else if(Ras_CheckIfSubscribed(deviceId) == FALSE)
    {
        /* Check the Mode bits and save the filter value */
        uint16_t filterVal = pRasCtrlPointCmd->cmdParameters.filterValue;

        if (((filterVal & BIT14) == 0U)
            && ((filterVal & BIT15) == 0U))
        {
            /* Mode 0 filter */
            maModeFilters[deviceId * gAppMaxConnections_c + gMode0Idx_c] = filterVal;
        }
        else if (((filterVal & BIT14) != 0U)
            && ((filterVal & BIT15) == 0U))
        {
            /* Mode 1 filter */
            maModeFilters[deviceId * gAppMaxConnections_c + gMode1Idx_c] = filterVal;
        }
        else if (((filterVal & BIT14) == 0U)
                 && ((filterVal & BIT15) != 0U))
        {
            /* Mode 2 filter */
            maModeFilters[deviceId * gAppMaxConnections_c + gMode2Idx_c] = filterVal;
        }
        else if (((filterVal & BIT14) != 0U)
                 && ((filterVal & BIT15) != 0U))
        {
            /* Mode 3 filter */
            maModeFilters[deviceId * gAppMaxConnections_c + gMode3Idx_c] = filterVal;
        }
        else
        {
            /* Invalid mode */
            rasStatus = gRasInvalidParameterError_c;
            result = gBleInvalidParameter_c;
        }
    }
    else
    {
        rasStatus = gRasInvalidParameterError_c;
        result = gBleInvalidState_c;
    }

    if (result == gBleSuccess_c)
    {
        /* Send response to the Filter Command */
        rasCPResponse.rspOpCode = (uint8_t)responseOpCode_c;
        rasCPResponse.cmdParameters.rspPayload.rspValue = (uint8_t)gRasSuccess_c;
        result = sendCommandResponse(deviceId, (uint8_t*)&rasCPResponse, sizeof(rasCPResponse));
    }

    checkRasStatus(deviceId, rasStatus);

    return result;
}

/*! *********************************************************************************
* @}
********************************************************************************** */
