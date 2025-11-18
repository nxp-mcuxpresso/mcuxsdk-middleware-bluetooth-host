/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file ranging_client.c
*
* This is a source file for the ranging client code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1U)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
#include "ranging_client_interface.h"
#include "fsl_component_timer_manager.h"
#include "fsl_component_mem_manager.h"
#include "channel_sounding.h"
#include "gatt_client_interface.h"

/************************************************************************************
*************************************************************************************
* Private macros
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
/* Remote measurement data  */
static rasMeasurementData_t mPeerResultData[gAppMaxConnections_c];
/* Tracker for the expected current segment number in RAS transfer */
static uint8_t segmentCounter = gRasSegmentCounterMinValue_c;
/* Tracker for whether the current RAS transfer was dropped and save indexes of lost segments */
static rasTransferStatus_t mRasTransferInfo[gAppMaxConnections_c];
/* Characteristic to be used for sending RAS commands */
static gattCharacteristic_t maRasCharacteristic;
/* Optional RAS features supported by the peer */
static uint32_t maRASFeatures[gAppMaxConnections_c] = {0U};
/* Values for the RAS filters for each CS mode */
static rasFilter_t maRasClientFilter[4U * gAppMaxConnections_c];
/* Handle of the RAS Control Point Characteristic */
static uint16_t mRasControlPointHandle[gAppMaxConnections_c];
/* Handle of the RAS Real Time Characteristic */
static uint16_t mRasRealTimeHandle[gAppMaxConnections_c];
/* RREQ - Real-Time data transfer enabled
   RRSP - Real-Time indication in progress */
static bool_t mbRealTimeTransfer[gAppMaxConnections_c] = {FALSE};
/* Application callback */
static pfAppCsCallback_t mpfAppCallback = NULL;
/* Tracker if Retrieve Lost Ranging Data Segments was requested */
static bool_t mbRetrLostRangingDataOngoing[gAppMaxConnections_c] = {FALSE};

/* Mechanism to handle RAP-defined timeouts on the RREQ */
static rreqTimeoutData_t mRreqTimeoutData;
static TIMER_MANAGER_HANDLE_DEFINE(mRreqTimerId);

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
/* RAP timer callback */
static void RreqTimerCallback
(
    void *param
);

/* Helper function to parse and save buffered data in case of lost segments */
static void parseBufferedNotifs
(
    deviceId_t deviceId
);

/* Helper function to send the Retrieve Lost Ranging Data Segments command */
static void handleRetrLostRangingData
(
    deviceId_t deviceId
);

/* Helper function to check if the last segment was received */
static bool_t checkForLastSegment
(
    deviceId_t deviceId
);

/* Helper function to check if a given segment was already received */
static bool_t checkIfSegmWasReceived
(
    deviceId_t deviceId,
    uint8_t segmHeader
);

/* Process indications received for the RAS control point characteristic
 * with the opcode completeProcDataRspOpCode_c */
static bleResult_t RasClient_CPRspCompleteProcData
(
    deviceId_t  deviceId
);

/* Process indications received for the RAS control point characteristic
 * with the opcode completeLostDataSegmentResponseOpCode_c */
static bleResult_t RasClient_CPRspCompleteLostDataSegment
(
    deviceId_t  deviceId
);

/* Process indications received for the RAS control point characteristic
 * with the opcode responseOpCode_c */
static bleResult_t RasClient_CPRspResponse
(
    deviceId_t  deviceId,
    rasControlPointRsp_t* pRasIndication
);

/* Store missing measurement data from a response to a Get Record Segments request */
static bleResult_t RasClient_ProcessGetRecordSegmentsResponse
(
    deviceId_t deviceId,
    uint8_t*   pValue,
    uint16_t   valueLength
);

/* Unsubscribe from receiving real-time data */
static bleResult_t RasClient_UnsubscribeRealTime
(
    deviceId_t deviceId
);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public Functions
*************************************************************************************

************************************************************************************/
/*! *********************************************************************************
*\fn          void RasClient_Init(pfAppCsCallback_t pAppCallback)
*
*\brief       Initialize internal RAS client structures.
*
*\param[in]   pAppCallback   Application callback
*
*\retval      none
********************************************************************************** */
void RasClient_Init
(
    pfAppCsCallback_t pAppCallback
)
{
    mpfAppCallback = pAppCallback;

    /* Reset measurement data */
    for (uint8_t index = 0U; index < (uint8_t)gAppMaxConnections_c; index++)
    {
        FLib_MemSet(mRasTransferInfo[index].lostSegm, 0U, gRASMaxNoOfSegments_c);
        FLib_MemSet(mRasTransferInfo[index].recvSegm, 0U, gRASMaxNoOfSegments_c);
        FLib_MemSet(mRasTransferInfo[index].recvIntermSegm, 0U, gRASMaxNoOfSegments_c);
        FLib_MemSet(mRasTransferInfo[index].recvIntermSegmLen, 0U, gRASMaxNoOfSegments_c * sizeof(uint16_t));
        for (uint8_t filterIdx = 0; filterIdx < 4U; filterIdx++)
        {
            maRasClientFilter[index * 4U + filterIdx].filterVal = gNoFilter_c;
            maRasClientFilter[index * 4U + filterIdx].filterSet = FALSE;
        }
        mRasTransferInfo[index].currentIdxLostSegm = 0U;
        mRasTransferInfo[index].currentIdxRecvSegm = 0U;
        mRasTransferInfo[index].currentIdxRecvIntermSegm = 0U;
        mRasTransferInfo[index].crtTempDataIdx  = 0U;
        mRasTransferInfo[index].lastDataIdx = 0U;
        mRasTransferInfo[index].crtIdxRecvLost = 0U;
        mRasTransferInfo[index].expectingSegments = FALSE;
        mPeerResultData[index].pData = NULL;
        maRASFeatures[index] = 0U;
        mbRealTimeTransfer[index] = FALSE;
        mbRetrLostRangingDataOngoing[index] = FALSE;
    }
}

/*! *********************************************************************************
*\fn         bleResult_t RasClient_StorePeerMeasurementData(deviceId_t deviceId,
*            uint8_t*   pValue, uint16_t   valueLength)
*
*\brief      Store measurement data received from the given peer.
*
*\param[in]  deviceId         Peer device id.
*\param[in]  pValue           Pointer to value.
*\param[in]  valueLength      Value length.
*
*\retval     gBleSuccess_c        Successful
*\retval     gBleOutOfMemory_c    Could not allocate memory for peer data
*\retval     gBleUnavailable_c    Algorithm not run, app must call it directly
********************************************************************************** */
bleResult_t RasClient_StorePeerMeasurementData
(
    deviceId_t deviceId,
    uint8_t*   pValue,
    uint16_t   valueLength
)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t* pRangingData = pValue;
    uint8_t segmentHeader;
    uint8_t receivedSegmentCounter;
    uint8_t currentIdx;

    /* Restart Ranging Data timer */
    (void)TM_Stop((timer_handle_t)mRreqTimerId);
    (void)TM_Start((timer_handle_t)mRreqTimerId, (uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSingleShot | (uint8_t)kTimerModeSetSecondTimer,
                   gRreqTimeoutDataSeconds_c);

    if (mPeerResultData[deviceId].pData == NULL)
    {
        mPeerResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
        if (mPeerResultData[deviceId].pData == NULL)
        {
            result = gBleOutOfMemory_c;
        }
    }

    if (mRasTransferInfo[deviceId].expectingSegments == TRUE)
    {
        (void)RasClient_ProcessGetRecordSegmentsResponse(deviceId, pValue, valueLength);
    }
    else
    {
        /* Extract Segment Header */
        segmentHeader = *pRangingData++;
        receivedSegmentCounter = segmentHeader;
        receivedSegmentCounter &= (~(uint8_t)gRasNotifFirstSegment_c);
        receivedSegmentCounter &= (~(uint8_t)gRasNotifLastSegment_c);
        receivedSegmentCounter = (uint8_t)(receivedSegmentCounter >> 2U);

       if ((segmentHeader & ((uint8_t)gRasNotifFirstSegment_c)) != 0U)
        {
            segmentCounter = gRasSegmentCounterMinValue_c;

            if (mbRealTimeTransfer[deviceId] == TRUE)
            {
                FLib_MemSet(&mPeerResultData[deviceId], 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
                FLib_MemSet(mPeerResultData[deviceId].pData, 0U, gRasCsSubeventDataSize_c);
                mPeerResultData[deviceId].totalSentRcvDataIndex = 0U;
            }
        }

        if ((receivedSegmentCounter == segmentCounter) || (mbRealTimeTransfer[deviceId] == TRUE))
        {
            /* Copy all received data */
            if (mRasTransferInfo[deviceId].pNotifTempBuffer == NULL)
            {
                FLib_MemCpy(mPeerResultData[deviceId].pData + mPeerResultData[deviceId].totalSentRcvDataIndex,
                            pRangingData,
                            (valueLength - sizeof(uint8_t)));
                mPeerResultData[deviceId].totalSentRcvDataIndex += (uint16_t)(valueLength - sizeof(uint8_t));

                /* Copy segmentation header information for the received segments */
                currentIdx = mRasTransferInfo[deviceId].currentIdxRecvSegm;
                mRasTransferInfo[deviceId].recvSegm[currentIdx] = segmentHeader;
                mRasTransferInfo[deviceId].currentIdxRecvSegm++;

                if ((segmentHeader & ((uint8_t)gRasNotifLastSegment_c)) != 0U)
                {
                    if (mbRealTimeTransfer[deviceId] == TRUE)
                    {
                        (void)TM_Stop((timer_handle_t)mRreqTimerId);
#if defined(gAppDeferAlgoRun_d) && (gAppDeferAlgoRun_d == TRUE)
                        result = gBleUnavailable_c;
#else
                        AppLocalization_RunAlgorithm(deviceId);
#endif
                    }
                }
            }
            else
            {
                /* Some segments were previously lost - copy notification data to temporary buffer */
                FLib_MemCpy(mRasTransferInfo[deviceId].pNotifTempBuffer + mRasTransferInfo[deviceId].crtTempDataIdx,
                            pValue,
                            valueLength);
                mRasTransferInfo[deviceId].crtTempDataIdx += valueLength;
                /* Save segment counter and data length for the received notification */
                currentIdx = mRasTransferInfo[deviceId].currentIdxRecvIntermSegm;
                mRasTransferInfo[deviceId].recvIntermSegm[currentIdx] = segmentHeader;
                mRasTransferInfo[deviceId].recvIntermSegmLen[currentIdx] = valueLength;
                mRasTransferInfo[deviceId].currentIdxRecvIntermSegm++;
            }
        }
        else
        {
            /* Store new notifications in a temporary buffer */
            if (mRasTransferInfo[deviceId].pNotifTempBuffer == NULL)
            {
                mRasTransferInfo[deviceId].pNotifTempBuffer = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
            }

            if (mRasTransferInfo[deviceId].pNotifTempBuffer != NULL)
            {
                mRasTransferInfo[deviceId].lastDataIdx = mPeerResultData[deviceId].totalSentRcvDataIndex;
                /* Compute the segmentation header value for the lost segment */
                uint8_t segmentationHeader = (uint8_t)(segmentCounter << 2U);
                if ((mRasTransferInfo[deviceId].crtTempDataIdx == 0U) &&
                    (mPeerResultData[deviceId].totalSentRcvDataIndex == 0U))
                {
                    /* First notification was lost */
                    segmentationHeader |= (uint8_t)gRasNotifFirstSegment_c;
                }

                for (uint8_t idx = mRasTransferInfo[deviceId].currentIdxLostSegm; idx < gRASMaxNoOfSegments_c; idx++)
                {
                    mRasTransferInfo[deviceId].lostSegm[idx] = segmentationHeader;
                    mRasTransferInfo[deviceId].currentIdxLostSegm++;
                    segmentCounter++;

                    if (segmentCounter == receivedSegmentCounter)
                    {
                        break;
                    }

                    segmentationHeader = (uint8_t)(segmentCounter << 2U);
                }

                /* Copy notification data to temporary buffer */
                FLib_MemCpy(mRasTransferInfo[deviceId].pNotifTempBuffer + mRasTransferInfo[deviceId].crtTempDataIdx,
                            pValue,
                            valueLength);
                mRasTransferInfo[deviceId].crtTempDataIdx += valueLength;
                currentIdx = mRasTransferInfo[deviceId].currentIdxRecvIntermSegm;
                mRasTransferInfo[deviceId].recvIntermSegm[currentIdx] = segmentHeader;
                mRasTransferInfo[deviceId].recvIntermSegmLen[currentIdx] = valueLength;
                mRasTransferInfo[deviceId].currentIdxRecvIntermSegm++;
            }
        }

        segmentCounter++;
        if ((segmentCounter == gRasSegmentCounterMaxValue_c) ||
            ((segmentHeader & ((uint8_t)gRasNotifLastSegment_c)) != 0U))
        {
            segmentCounter = gRasSegmentCounterMinValue_c;
        }
    }

    return result;
}

/*! *********************************************************************************
*\fn           bleResult_t RasClient_ProcessRasDataReadyIndications(deviceId_t deviceId,
*              uint8_t*    pValue, uint16_t    valueLength)
*
*\brief        Process indications received for the RAS Ranging Data Ready characteristic .
*
*\param[in]    deviceId          Peer device id.
*\param[in]    pValue            Pointer to value.
*\param[in]    valueLength       Value length.
*
*\retval       bleResult_t       Result of the operation.
********************************************************************************** */
bleResult_t RasClient_ProcessRasDataReadyIndications
(
    deviceId_t deviceId,
    uint8_t*   pValue,
    uint16_t   valueLength
)
{
    bleResult_t result = gBleSuccess_c;
    appLocalizationError_t error;
    uint16_t procCounter = AppLocalization_GetProcedureCount(deviceId);
    uint8_t subeventIndex = AppLocalization_GetSubeventIdx(deviceId);
    rasDataReadyIndication_t* pRasIndication = (rasDataReadyIndication_t * )(void * )pValue;
    uint16_t procedureCounter = AppLocalization_GetGlobalProcedureCount(deviceId);

    /* Received data ready indication */
    (void)TM_Stop((timer_handle_t)mRreqTimerId);

    /* Reset RAS transfer data in preparation for a new procedure */
    if (mPeerResultData[deviceId].pData == NULL)
    {
        mPeerResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
        if (mPeerResultData[deviceId].pData == NULL)
        {
            result = gBleOutOfMemory_c;
        }
    }

    if (result == gBleSuccess_c)
    {
        FLib_MemSet(&mPeerResultData[deviceId], 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
        FLib_MemSet(mPeerResultData[deviceId].pData, 0U, gRasCsSubeventDataSize_c);
        FLib_MemSet(mRasTransferInfo[deviceId].lostSegm, 0U, gRASMaxNoOfSegments_c);
        FLib_MemSet(mRasTransferInfo[deviceId].recvSegm, 0U, gRASMaxNoOfSegments_c);
        FLib_MemSet(mRasTransferInfo[deviceId].recvIntermSegm, 0U, gRASMaxNoOfSegments_c);
        FLib_MemSet(mRasTransferInfo[deviceId].recvIntermSegmLen, 0U, gRASMaxNoOfSegments_c * sizeof(uint16_t));
        mRasTransferInfo[deviceId].expectingSegments = FALSE;
        mRasTransferInfo[deviceId].currentIdxLostSegm = 0U;
        mRasTransferInfo[deviceId].currentIdxRecvIntermSegm = 0U;
        mRasTransferInfo[deviceId].currentIdxRecvSegm = 0U;

        /* Check if the received procedure index matches the local one */
        if (pRasIndication->procedureIndex != procedureCounter)
        {
            /* Received an unexpected procedure - do not initiate transfer and wait for the next procedure */
            if (procCounter == mRangeSettings[deviceId].maxNumProcedures)
            {
                AppLocalization_SetLocState(deviceId, gAppLclIdle_c);
            }
            else
            {
                AppLocalization_SetLocState(deviceId, gAppLclWaitingForMeasData_c);
            }

            error = gAppLclInvalidProcIndex_c;
            segmentCounter = gRasSegmentCounterMinValue_c;
            result = gBleUnexpectedError_c;

            if (mpfAppCallback != NULL)
            {
                mpfAppCallback(deviceId, (void*)&error, gErrorEvent_c);
            }
        }
        else
        {
            /* Send Get ranging data command if the procedure executed successfully locally */
            if (AppLocalization_GetProcDoneStatus(deviceId, subeventIndex) == (uint8_t)gCsCompleteResults_c)
            {
                segmentCounter = gRasSegmentCounterMinValue_c;
                result = RasClient_SendRasCommand(deviceId, getRangingDataOpCode_c,
                                                  0U, 0U, procedureCounter,
                                                  gAntennaPathFilterAllowAll_c);
            }
        }
    }

    return result;
}

/*! *********************************************************************************
*\fn           bleResult_t RasClient_ProcessRasDataOverwrittenIndications(deviceId_t deviceId,
*              uint8_t*    pValue, uint16_t    valueLength)
*
*\brief        Process indications received for the RAS Ranging Data Overwritten characteristic .
*
*\param[in]    deviceId          Peer device id.
*\param[in]    pValue            Pointer to value.
*\param[in]    valueLength       Value length.
*
*\retval       bleResult_t       Result of the operation.
********************************************************************************** */
bleResult_t RasClient_ProcessRasDataOverwrittenIndications
(
    deviceId_t deviceId,
    uint8_t*   pValue,
    uint16_t   valueLength
)
{
    bleResult_t result = gBleSuccess_c;

    /* Received data overwritten indication - clear current procedure data and wait for a new data ready indication */
    FLib_MemSet(&mPeerResultData[deviceId], 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
    if (mPeerResultData[deviceId].pData != NULL)
    {
        FLib_MemSet(mPeerResultData[deviceId].pData, 0U, gRasCsSubeventDataSize_c);
    }

    if (mRasTransferInfo[deviceId].pNotifTempBuffer != NULL)
    {
        (void)MEM_BufferFree(mRasTransferInfo[deviceId].pNotifTempBuffer);
        mRasTransferInfo[deviceId].pNotifTempBuffer = NULL;
    }

    if (mpfAppCallback != NULL)
    {
        mpfAppCallback(deviceId, NULL, gDataOverwritten_c);
    }

    return result;
}

/*! *********************************************************************************
*\fn         bleResult_t RasClient_ProcessRasCPRsp(deviceId_t deviceId,
*            uint8_t* pValue, uint16_t valueLength)
*
*\brief      Process indications received for the RAS control point characteristic .
*
*\param[in]  deviceId         Peer device id.
*\param[in]  pValue           Pointer to value.
*\param[in]  valueLength      Value length.
*
*\retval     gBleSuccess_c        Successful
*\retval     gBleUnavailable_c    Algorithm not run, app must call it directly
*\retval     bleResult_t          Result of call to RasClient_SendRasCommand
********************************************************************************** */
bleResult_t RasClient_ProcessRasCPRsp
(
    deviceId_t  deviceId,
    uint8_t*    pValue,
    uint16_t    valueLength
)
{
    bleResult_t result = gBleSuccess_c;
    rasControlPointRsp_t* pRasIndication = (rasControlPointRsp_t * )(void * )pValue;

    if (pRasIndication->rspOpCode == ((uint8_t)completeProcDataRspOpCode_c))
    {
        result = RasClient_CPRspCompleteProcData(deviceId);
    }

    if ((pRasIndication->rspOpCode == ((uint8_t)completeLostDataSegmentResponseOpCode_c)) &&
        (mbRetrLostRangingDataOngoing[deviceId] == TRUE))
    {
        mbRetrLostRangingDataOngoing[deviceId] = FALSE;
        result = RasClient_CPRspCompleteLostDataSegment(deviceId);
    }

    if (pRasIndication->rspOpCode == ((uint8_t)responseOpCode_c))
    {
        result = RasClient_CPRspResponse(deviceId, pRasIndication);
    }

    return result;
}

/*! *********************************************************************************
*\fn         bleResult_t RasClient_SendRasCommand(deviceId_t peerDeviceId,
*            rasControlPointOperations_tag rasCmdOpcode, uint8_t startSegm, uint8_t endSegm,
*            uint16_t procCounter, uint16_t filter)
*
*\brief      Handle a RAS Command Response received from the peer.
*
*\param[in]  peerDeviceId       Peer device id.
*\param[in]  rasCmdOpcode       OpCode of the command to be sent
*\param[in]  startSegm          Start segment parameter of Retrieve Lost Ranging Data Segments command
*\param[in]  endSegm            Start segment parameter of Retrieve Lost Ranging Data Segments command
*\param[in]  procCounter        Identifier of the procedure to which the command is reffering
*\param[in]  filter             Filter value to be used for the Filter command
*
*\retval     bleResult_t        Result of the operation.
********************************************************************************** */
bleResult_t RasClient_SendRasCommand
(
    deviceId_t                    peerDeviceId,
    rasControlPointOperations_tag rasCmdOpcode,
    uint8_t                       startSegm,
    uint8_t                       endSegm,
    uint16_t                      procCounter,
    uint16_t                      filter
)
{
    rasControlPointReq_t rasCtrlPointCmd = {0};
    bool_t rasCommandSupported = TRUE;

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
    if (rasCmdOpcode == getRangingDataOpCode_c)
    {
        gCsTimeInfo.transferStart = TM_GetTimestamp();
    }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

    switch ((uint8_t)rasCmdOpcode)
    {
        case (uint8_t)getRangingDataOpCode_c:
        {
            mRreqTimeoutData.deviceId = peerDeviceId;
            mRreqTimeoutData.reason = (uint8_t)rreqWaitingForRangingData_c;
            (void)TM_InstallCallback((timer_handle_t)mRreqTimerId, RreqTimerCallback, &mRreqTimeoutData);
            (void)TM_Start((timer_handle_t)mRreqTimerId, (uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSingleShot | (uint8_t)kTimerModeSetSecondTimer,
                            gRreqTimeoutDataSeconds_c);

        }
        break;

        case (uint8_t)retrLostRangingDataOpCode_c:
        {
            if ((maRASFeatures[peerDeviceId] & BIT1) == 0U)
            {
                rasCommandSupported = FALSE;
            }
            else
            {
                mbRetrLostRangingDataOngoing[peerDeviceId] = TRUE;
            }
        }
        break;

        case (uint8_t)abortOperationOpCode_c:
        {
            if ((maRASFeatures[peerDeviceId] & BIT2) == 0U)
            {
                rasCommandSupported = FALSE;
            }
        }
        break;

        case (uint8_t)setFilterOpCode_c:
        {
            if ((maRASFeatures[peerDeviceId] & BIT3) == 0U)
            {
                rasCommandSupported = FALSE;
            }
        }
        break;

        default:
        {
            /* MISRA compliance */
        }
        break;
    }

    if (rasCommandSupported == TRUE)
    {
        rasCtrlPointCmd.cmdOpCode = ((uint8_t)rasCmdOpcode);
        rasCtrlPointCmd.cmdParameters.procCounter = procCounter;

        if (rasCmdOpcode == retrLostRangingDataOpCode_c)
        {
            rasCtrlPointCmd.cmdParameters.retrLostDataParams.procCounter = procCounter;
            rasCtrlPointCmd.cmdParameters.retrLostDataParams.startAbsSegment = startSegm;
            rasCtrlPointCmd.cmdParameters.retrLostDataParams.endAbsSegment = endSegm;
        }

        if (rasCmdOpcode == setFilterOpCode_c)
        {
            rasCtrlPointCmd.cmdParameters.filterValue = filter;
        }

        maRasCharacteristic.value.handle = mRasControlPointHandle[peerDeviceId];
        maRasCharacteristic.value.uuidType = gBleUuidType16_c;
        maRasCharacteristic.value.uuid.uuid16 = gBleSig_RasControlPoint_d;

        return GattClient_CharacteristicWriteWithoutResponse(peerDeviceId, &maRasCharacteristic,
                                                             (uint16_t)maRasCPCommandSizes[rasCmdOpcode],
                                                             (uint8_t*)&rasCtrlPointCmd);
    }
    else
    {
        /* Peer RAS server does not support the selected command */
        return gBleFeatureNotSupported_c;
    }
}

/*! *********************************************************************************
*\fn            bleResult_t RasClient_UnsubscribeRealTime(deviceId_t deviceId);
*
*\brief         Unsubscribe from receiving real-time data
*
*\param[in]     deviceId         Peer identifier
*
*\retval        gBleSuccess_c or error
********************************************************************************** */
static bleResult_t RasClient_UnsubscribeRealTime
(
    deviceId_t deviceId
)
{
    /* Write value 0 to Real-Time characteristic CCCD to unsubscribe */
    uint16_t value = 0U;

    maRasCharacteristic.value.handle = mRasRealTimeHandle[deviceId] + 1U;
    maRasCharacteristic.value.uuidType = gBleUuidType16_c;
    maRasCharacteristic.value.uuid.uuid16 = gBleSig_CCCD_d;

    return GattClient_SimpleCharacteristicWrite(deviceId, &maRasCharacteristic,
                                                (uint16_t)sizeof(value),
                                                (void*)&value);
}

/*! *********************************************************************************
*\fn         void RasClient_SetRasControlPointHandle(deviceId_t deviceId, uint16_t handle);
*
*\brief      Register the handle value for the RAS Control Point characteristic.
*
*\param[in]  deviceId       Peer Identifier
*\param[in]  handle         Handle value
*
*\retval     none
********************************************************************************** */
void RasClient_SetRasControlPointHandle
(
    deviceId_t deviceId,
    uint16_t   handle
)
{
    mRasControlPointHandle[deviceId] = handle;
}

/*! *********************************************************************************
*\fn         void RasClient_SetRasRealTimeHandle(deviceId_t deviceId, uint16_t handle);
*
*\brief      Register the handle value for the RAS Real-Time Ranging Data characteristic.
*
*\param[in]  deviceId       Peer Identifier
*\param[in]  handle         Handle value
*
*\retval     none
********************************************************************************** */
void RasClient_SetRasRealTimeHandle
(
    deviceId_t deviceId,
    uint16_t   handle
)
{
    mRasRealTimeHandle[deviceId] = handle;
}

/*! *********************************************************************************
*\fn         void RasClient_SetRasSupportedFeatures(deviceId_t deviceId,
*                                                   uint32_t features);
*
*\brief      Register the handle value for the RAS Control Point characteristic.
*
*\param[in]  deviceId         Peer device id.
*\param[in]  features         Optional RAS features supported by peer
*
*\retval     none
********************************************************************************** */
void RasClient_SetRasSupportedFeatures
(
    deviceId_t deviceId,
    uint32_t   features
)
{
    maRASFeatures[deviceId] = features;
}

/*! *********************************************************************************
*\fn            void RasClient_ParseReceivedSubeventHeader(deviceId_t deviceId,
*               uint8_t* pRangingData);
*
*\brief         Parse the subevent header information in the ranging data.
*
*\param[in]     deviceId      Peer identifier
*\param[in]     pRangingData  Pointer to ranging data
*
*\retval        none
********************************************************************************** */
void RasClient_ParseReceivedSubeventHeader
(
    deviceId_t deviceId,
    uint8_t*   pRangingData
)
{
    uint8_t* pAuxData = pRangingData;

    /* Subevent header information */
    uint8_t subeventIdx = mPeerResultData[deviceId].subeventIndex;
    csRasSubeventHeader_t *pSubevtHeader = &mPeerResultData[deviceId].aSubEventData[subeventIdx].subevtHeader;

    pSubevtHeader->startACLConnEvent = Utils_ExtractTwoByteValue(pAuxData);
    pAuxData = &pAuxData[2];
    pSubevtHeader->frequencyCompensation = Utils_ExtractTwoByteValue(pAuxData);
    pAuxData = &pAuxData[2];
    pSubevtHeader->procedureDoneStatus = (*pAuxData) & 0x0F;
    pSubevtHeader->subeventDoneStatus = ((*pAuxData++) & 0xF0) >> 4U;
    pSubevtHeader->abortReason = *pAuxData++;
    pSubevtHeader->referencePowerLevel = (int8_t)(*pAuxData++);
    pSubevtHeader->numStepsReported = *pAuxData++;
    mPeerResultData[deviceId].totalNumSteps += pSubevtHeader->numStepsReported;
}

/*! *********************************************************************************
*\fn            uint16_t RasClient_GetModeFilter(deviceId_t deviceId, uint8_t mode);
*
*\brief         Return the filter set for the giver peer id and step mode.
*
*\param[in]     deviceId      Peer identifier
*\param[in]     mode          Step Mode (0 - 3)
*
*\retval        uint16_t      Filter value
********************************************************************************** */
uint16_t RasClient_GetModeFilter
(
    deviceId_t deviceId,
    uint8_t    mode
)
{
    return (uint16_t)maRasClientFilter[deviceId * 4U + mode].filterVal;
}

/*! *********************************************************************************
*\fn            bleResult_t RasClient_RasSetFilter(deviceId_t deviceId,
*                           uint16_t  filterValue, bool_t sendCommand, bool_t* pOutFilterSetDone);
*
*\brief         Send a Filter command to the peer with the provided filter value.
*
*\param[in]     deviceId             Peer identifier
*\param[in]     filterValue          Filter value to be set
*\param[in]     sendCommand          TRUE if command should be sent to peer, FALSE otherwise.
*\param[out]    pOutFilterSetDone    TRUE if all filters have been set, FALSE otherwise.
*
*\retval        gBleSuccess_c or error
********************************************************************************** */
bleResult_t RasClient_RasSetFilter
(
    deviceId_t deviceId,
    uint16_t  filterValue,
    bool_t    sendCommand,
    bool_t*   pOutFilterSetDone
)
{
    bleResult_t result = gBleSuccess_c;
    static uint8_t filterIdx[gAppMaxConnections_c] = {0x0U};
    union
    {
        uint32_t val32;
        uint16_t val16;
    } filterVal = {0U};

    *pOutFilterSetDone = FALSE;
    filterVal.val16 = filterValue;

    /* Check the Mode bits and save the filter value */
    if (sendCommand == FALSE)
    {
        filterIdx[deviceId] = 0;
        if (((filterValue & BIT0) == 0U)
            && ((filterValue & BIT1) == 0U))
        {
            maRasClientFilter[deviceId * 4U + gMode0Idx_c].filterVal =
                filterVal.val32;
            maRasClientFilter[deviceId * 4U + gMode0Idx_c].filterSet = FALSE;
        }
        else if (((filterValue & BIT0) != 0U)
            && ((filterValue & BIT1) == 0U))
        {
            maRasClientFilter[deviceId * 4U + gMode1Idx_c].filterVal =
                filterVal.val32;
            maRasClientFilter[deviceId * 4U + gMode1Idx_c].filterSet = FALSE;
        }
        else if (((filterValue & BIT0) == 0U)
                 && ((filterValue & BIT1) != 0U))
        {
            maRasClientFilter[deviceId * 4U + gMode2Idx_c].filterVal =
                filterVal.val32;
            maRasClientFilter[deviceId * 4U + gMode2Idx_c].filterSet = FALSE;
        }
        else if (((filterValue & BIT0) != 0U)
                 && ((filterValue & BIT1) != 0U))
        {
            maRasClientFilter[deviceId * 4U + gMode3Idx_c].filterVal =
                filterVal.val32;
            maRasClientFilter[deviceId * 4U + gMode3Idx_c].filterSet = FALSE;
        }
        else
        {
            /* Invalid mode */
            result = gBleInvalidParameter_c;
        }
    }

    if ((result == gBleSuccess_c) && (sendCommand == TRUE))
    {
        /* Check if filters for multiple modes are set */
        for (uint8_t idx = filterIdx[deviceId]; idx <= (gMode3Idx_c+1U); idx++)
        {
            filterIdx[deviceId] = idx;
            if ((maRasClientFilter[deviceId * 4U + idx].filterVal != gNoFilter_c)
                && (maRasClientFilter[deviceId * 4U + idx].filterSet == FALSE))
            {
                /* save filter for next call */
                break;
            }
        }

        /* All filters were set */
        if (filterIdx[deviceId] > gMode3Idx_c)
        {
            filterIdx[deviceId] = 0xFF;
            *pOutFilterSetDone = TRUE;
        }
        else
        {
            filterValue = (uint16_t)maRasClientFilter[deviceId * 4U + filterIdx[deviceId]].filterVal;
            result = RasClient_SendRasCommand(deviceId,
                                                    setFilterOpCode_c,
                                                    0U, 0U, 0U,
                                                    filterValue);
            maRasClientFilter[deviceId * 4U + filterIdx[deviceId]].filterSet = TRUE;
        }
    }

    return result;
}

/*! *********************************************************************************
*\fn            bool_t RasClient_GetRealTimePreference(deviceId_t deviceId);
*
*\brief         Get the current setting for real time data transfer.
*
*\param[in]     deviceId         Peer identifier
*
*\retval        bool_t  TRUE if real-time data transfer is enabled, FALSE otherwise
********************************************************************************** */
bool_t RasClient_GetRealTimePreference
(
    deviceId_t deviceId
)
{
    return mbRealTimeTransfer[deviceId];
}

/*! *********************************************************************************
*\fn            void RasClient_SetRealTimePreference(deviceId_t deviceId);
*
*\brief         Enable if the client application wishes to use real time transfer
*
*\param[in]     deviceId           Peer identifier
*\param[in]     realTimePreference TRUE if real-time transfer is enabled, FALSE otherwise
*
*\retval        none
********************************************************************************** */
void RasClient_SetRealTimePreference
(
    deviceId_t deviceId,
    bool_t     realTimePreference
)
{
    mbRealTimeTransfer[deviceId] = realTimePreference;
}

/*! *********************************************************************************
*\fn         void RasClient_GetRealTimeMode(deviceId_t deviceId);
*
*\brief      Returns TRUE if Real Time RAS transfer is enabled, FALSE otherwise.
*
*\param[in]  deviceId       Peer Identifier
*
*\retval     TRUE if Real Time RAS transfer is enabled, FALSE otherwise.
********************************************************************************** */
bool_t RasClient_GetRealTimeMode
(
    deviceId_t deviceId
)
{
    return mbRealTimeTransfer[deviceId];
}

/*! *********************************************************************************
*\fn         void RasClient_ResetRasTransferInfo(deviceId_t deviceId);
*
*\brief      Resets internal RAS trasnfer information
*
*\param[in]  deviceId       Peer Identifier
*
*\retval     none
********************************************************************************** */
void RasClient_ResetRasTransferInfo
(
    deviceId_t deviceId
)
{
    /* Cleary any temporary data */
    if (mRasTransferInfo[deviceId].pNotifTempBuffer != NULL)
    {
        (void)MEM_BufferFree(mRasTransferInfo[deviceId].pNotifTempBuffer);
        mRasTransferInfo[deviceId].pNotifTempBuffer = NULL;
    }
    /* Clear transfer data */
    FLib_MemSet(&mRasTransferInfo[deviceId], 0U, sizeof(rasTransferStatus_t));
}

/*! *********************************************************************************
*\fn         void RasClient_ResetPeerInfo(deviceId_t deviceId);
*
*\brief      Resets internal RAS peer information
*
*\param[in]  deviceId       Peer Identifier
*
*\retval     none
********************************************************************************** */
void RasClient_ResetPeerInfo
(
    deviceId_t deviceId
)
{
    FLib_MemSet(&mPeerResultData[deviceId], 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
    if (mPeerResultData[deviceId].pData != NULL)
    {
        FLib_MemSet(mPeerResultData[deviceId].pData, 0U, gRasCsSubeventDataSize_c);
    }
}

/*! *********************************************************************************
*\fn            void RasClient_ResetPeer(deviceId_t deviceId, bool_t disconnected)
*
*\brief         Resets RAS client data for the given peer device.
*
*\param  [in]   deviceId        Peer device identifier.
*\param  [in]   disconnected    TRUE if the peer disconnected, FALSE otherwise.
*
*\retval        none.
********************************************************************************** */
void RasClient_ResetPeer
(
    deviceId_t deviceId,
    bool_t disconnected
)
{
    FLib_MemSet(&mPeerResultData[deviceId], 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
    if (mPeerResultData[deviceId].pData != NULL)
    {
        (void)MEM_BufferFree(mPeerResultData[deviceId].pData);
        mPeerResultData[deviceId].pData = NULL;
    }

    /* Reset transfer information */
    RasClient_ResetRasTransferInfo(deviceId);

    if (disconnected == TRUE)
    {
        mbRealTimeTransfer[deviceId] = FALSE;
        mbRetrLostRangingDataOngoing[deviceId] = FALSE;

        maRASFeatures[deviceId] = 0U;
        /* Stop RAP timer */
        (void)TM_Stop((timer_handle_t)mRreqTimerId);

        for (uint8_t filterIdx = 0; filterIdx < 4U; filterIdx++)
        {
            maRasClientFilter[deviceId * 4U + filterIdx].filterVal = gNoFilter_c;
            maRasClientFilter[deviceId * 4U + filterIdx].filterSet = FALSE;
        }
    }
}

/*! *********************************************************************************
*\fn            void RasClient_OpenRapTimer(void)
*
*\brief         Open the RAP for the given peer device.
*
*\param  [in]   none
*
*\retval        none.
********************************************************************************** */
void RasClient_OpenRapTimer(void)
{
    (void)TM_Open(mRreqTimerId);
}

/*! *********************************************************************************
*\fn            void RasClient_SartRapTimer(deviceId_t deviceId,
*                                           rreqTimeoutData_t rreqTimeoutData)
*
*\brief         Start the RAP for the given peer device.
*
*\param  [in]   deviceId           Peer device identifier.
*\param  [in]   rreqTimeoutData    Timer callback information
*
*\retval        none.
********************************************************************************** */
void RasClient_SartRapTimer
(
    deviceId_t        deviceId,
    rreqTimeoutData_t rreqTimeoutData
)
{
    /* Set the new timer callback information */
    FLib_MemCpy(&mRreqTimeoutData, &rreqTimeoutData, sizeof(rreqTimeoutData_t));
    /* Start timer */
    (void)TM_InstallCallback((timer_handle_t)mRreqTimerId, RreqTimerCallback, &mRreqTimeoutData);
    (void)TM_Start((timer_handle_t)mRreqTimerId, (uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSingleShot | (uint8_t)kTimerModeSetSecondTimer,
                   gRreqTimeoutDataReadySeconds_c);
}

/*! *********************************************************************************
*\fn            void RasClient_ParseDataHeader(deviceId_t deviceId);
*
*\brief         Parser for header data.
*
*\param[in]     deviceId        Peer identifier
*
*\retval        None
********************************************************************************** */
void RasClient_ParseDataHeader
(
    deviceId_t deviceId
)
{
    mPeerResultData[deviceId].deviceId = deviceId;
    uint8_t* pRangingData = mPeerResultData[deviceId].pData;
    uint16_t counterCfgId = 0U;

    /* After RAS transfer is completed mPeerResultData will contain the whole ranging body:
       procedure header, subevent header 1, subevent data 1, ... , subevent header n, subevent data n.*/
    counterCfgId = Utils_BeExtractTwoByteValue(pRangingData);
    pRangingData += 2U;
    mPeerResultData[deviceId].procedureCounter = ((counterCfgId & 0xFF00U) >> 8U);
    mPeerResultData[deviceId].configId = (uint8_t)((counterCfgId & 0xF0U) >> 4U);
    mPeerResultData[deviceId].procedureCounter |= ((counterCfgId & 0x0FU) << 8U);

    mPeerResultData[deviceId].selectedTxPower = (int8_t)(*pRangingData);
    pRangingData++;
    /* Skip RFU (Reserved for future use) byte */
    pRangingData++;
    mPeerResultData[deviceId].numAntennaPaths = AppLocalization_GetNumAntennaPaths(deviceId);

    /* Parse first subevent header */
    RasClient_ParseReceivedSubeventHeader(deviceId, pRangingData);
}

/*! *********************************************************************************
*\fn            uint16_t RasClient_GetPeerProcCount(deviceId_t deviceId);
*
*\brief         Return the procedure counter received from the peer
*
*\param[in]     deviceId        Peer identifier
*
*\retval        Peer procedure counter
********************************************************************************** */
uint16_t RasClient_GetPeerProcCount
(
    deviceId_t deviceId
)
{
    return mPeerResultData[deviceId].procedureCounter;
}

/*! *********************************************************************************
*\fn            uint16_t RasClient_ResetPeerProcData(deviceId_t deviceId);
*
*\brief         Reset the peer procedure data
*
*\param[in]     deviceId        Peer identifier
*
*\retval        none
********************************************************************************** */
void RasClient_ResetPeerProcData
(
    deviceId_t deviceId
)
{
    FLib_MemSet(&mPeerResultData[deviceId], 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
    if (mPeerResultData[deviceId].pData != NULL)
    {
        FLib_MemSet(mPeerResultData[deviceId].pData, 0U, gRasCsSubeventDataSize_c);
    }
}

/*! *********************************************************************************
*\fn            rasMeasurementData_t* RasClient_GetPeerRangingData(deviceId_t deviceId);
*
*\brief         Get a pointer to the peer ranging data
*
*\param[in]     deviceId        Peer identifier
*
*\retval        Pointer to peer ranging data
********************************************************************************** */
rasMeasurementData_t* RasClient_GetPeerRangingData
(
    deviceId_t deviceId
)
{
    return &mPeerResultData[deviceId];
}

/*! *********************************************************************************
*\fn            void RasClient_SetPeerRangingData(deviceId_t deviceId);
*
*\brief         Set a pointer to the peer ranging data
*
*\param[in]     deviceId     Peer identifier
*\param[in]     pData        Pointer to the new ranging data
*
*\retval        Pointer to peer ranging data
********************************************************************************** */
void RasClient_SetPeerRangingData
(
    deviceId_t deviceId,
    rasMeasurementData_t* pData
)
{
    /* Copy the received data */
    FLib_MemCpy(&mPeerResultData[deviceId], pData, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
    mPeerResultData[deviceId].pData = pData->pData;
}

/*! *********************************************************************************
*\fn            uint16_t RasClient_GetPeerRangingDataSize(deviceId_t deviceId);
*
*\brief         Get the size of the peer ranging data
*
*\param[in]     deviceId        Peer identifier
*
*\retval        Peer ranging data size
********************************************************************************** */
uint16_t RasClient_GetPeerRangingDataSize
(
    deviceId_t deviceId
)
{
    return mPeerResultData[deviceId].totalSentRcvDataIndex;
}

/************************************************************************************
*************************************************************************************
* Private Functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn         static void parseBufferedNotifs(deviceId_t deviceId)
*
*\brief      Parse the notifications buffered in case of lost ranging segments
*
*\param[in]  deviceId         Peer identifier
*
*\retval     none
********************************************************************************** */
static void parseBufferedNotifs
(
    deviceId_t deviceId
)
{
    uint8_t* pRangingData;
    uint8_t dataLen = 0U;
    uint8_t currentIdx = 0U;

    for (uint8_t recvIdx = 0U; recvIdx < mRasTransferInfo[deviceId].currentIdxRecvIntermSegm; recvIdx++)
    {
        if ((mRasTransferInfo[deviceId].recvIntermSegm[recvIdx] != 0U) &&
            (mRasTransferInfo[deviceId].recvIntermSegm[recvIdx] != 0xFFU))
        {
            dataLen = (uint8_t)(mRasTransferInfo[deviceId].recvIntermSegmLen[recvIdx]);
            pRangingData = mRasTransferInfo[deviceId].pNotifTempBuffer + mRasTransferInfo[deviceId].crtTempDataIdx;
            /* Skip segmentation header */
            pRangingData++;
            /* Copy data from buffered notifications */
            FLib_MemCpy(mPeerResultData[deviceId].pData + mPeerResultData[deviceId].totalSentRcvDataIndex,
                        pRangingData,
                        (dataLen - sizeof(uint8_t)));
            mPeerResultData[deviceId].totalSentRcvDataIndex += (uint16_t)(dataLen - sizeof(uint8_t));
            mRasTransferInfo[deviceId].crtTempDataIdx += dataLen;
            /* mark segment as copied */
            currentIdx = mRasTransferInfo[deviceId].currentIdxRecvSegm;
            mRasTransferInfo[deviceId].recvSegm[currentIdx] = mRasTransferInfo[deviceId].recvIntermSegm[recvIdx];
            mRasTransferInfo[deviceId].currentIdxRecvSegm++;
            mRasTransferInfo[deviceId].recvIntermSegm[recvIdx] = 0xFF;
        }
    }

    /* Data cleanup */
    (void)MEM_BufferFree(mRasTransferInfo[deviceId].pNotifTempBuffer);
    mRasTransferInfo[deviceId].pNotifTempBuffer = NULL;
}

/*! *********************************************************************************
*\fn         static void handleRetrLostRangingData(deviceId_t deviceId)
*
*\brief      Parse the lost segments indices and send the Retrieve Lost Ranging Data
*            Segments command if needed.
*
*\param[in]  deviceId         Peer identifier
*
*\retval     none
********************************************************************************** */
static void handleRetrLostRangingData
(
    deviceId_t deviceId
)
{
    uint8_t startSegm = 0U;
    uint8_t endSegm = 0U;
    uint8_t startCounter;
    uint8_t idx = 0U;

    for (idx = 0U; idx < mRasTransferInfo[deviceId].currentIdxLostSegm; idx++)
    {
        if ((startSegm == 0U) && (mRasTransferInfo[deviceId].lostSegm[idx] != 0U))
        {
            /* set start segment */
            startSegm = mRasTransferInfo[deviceId].lostSegm[idx];
        }

        if (startSegm != 0U)
        {
            if (mRasTransferInfo[deviceId].currentIdxLostSegm == 1U)
            {
                endSegm = startSegm;
                break;
            }

            if (idx < (mRasTransferInfo[deviceId].currentIdxLostSegm -1U))
            {
                endSegm = mRasTransferInfo[deviceId].lostSegm[idx+1U];
            }

            startCounter = (uint8_t)(startSegm >> 2U);

            if ((endSegm >> 2U) != (startCounter + 1U))
            {
                /* Lost segments are not consecutive - send sepparate get commands */
                endSegm = startSegm;
                break;
            }
        }
    }

    if ((startSegm != 0U) && (endSegm != 0U))
    {
        /* Extract Segment Header */
        uint8_t startRollingCounter = (uint8_t)(startSegm >> 2U);
        uint8_t endRollingCounter = (uint8_t)(endSegm >> 2U);

        (void)RasClient_SendRasCommand(deviceId, retrLostRangingDataOpCode_c,
                                       startRollingCounter, endRollingCounter,
                                       AppLocalization_GetGlobalProcedureCount(deviceId),
                                       (uint16_t)gNoFilter_c);
    }
}

/*! *********************************************************************************
*\fn         static void RreqTimerCallback(void *param)
*
*\brief      Ranging Profile Timer Callback
*
*\param[in]  param         Pointer to timeout data (reason and deviceId)
*
*\retval     none
********************************************************************************** */
static void RreqTimerCallback
(
    void *param
)
{
    rreqTimeoutData_t *pTimeoutData = (rreqTimeoutData_t*)param;
    uint16_t procCount = AppLocalization_GetProcedureCount(pTimeoutData->deviceId);
    appLocalizationError_t error;

    /* Update state */
    if (procCount == mRangeSettings[pTimeoutData->deviceId].maxNumProcedures)
    {
        AppLocalization_SetLocState(pTimeoutData->deviceId, gAppLclIdle_c);
    }
    else
    {
        AppLocalization_SetLocState(pTimeoutData->deviceId, gAppLclWaitingForMeasData_c);
    }

    if (mbRealTimeTransfer[pTimeoutData->deviceId] == TRUE)
    {
        error = gAppLclAlgoNotRunNoRealTimeData_c;
    }
    else
    {
        if (pTimeoutData->reason == (uint8_t)rreqWaitingForDataReady_c)
        {
            error = gAppLclAlgoNotRunNoDataReady_c;
        }
        else
        {
            error = gAppLclAlgoNotRunNoRangingData_c;
        }
    }

    if (mpfAppCallback != NULL)
    {
        mpfAppCallback(pTimeoutData->deviceId, (void*)&error, gErrorEvent_c);
    }

    uint16_t maAlgoRunCount = AppLocalization_GetAlgoRunCount(pTimeoutData->deviceId) + 1U;
    AppLocalization_SetAlgoRunCount(pTimeoutData->deviceId, maAlgoRunCount);

    if ((error == gAppLclAlgoNotRunNoRangingData_c) &&
        (mbRealTimeTransfer[pTimeoutData->deviceId] == FALSE))
    {
        (void)RasClient_SendRasCommand(pTimeoutData->deviceId, abortOperationOpCode_c,
                                       0U, 0U, 0U, gAntennaPathFilterAllowAll_c);
    }

    if (mbRealTimeTransfer[pTimeoutData->deviceId] == TRUE)
    {
        (void)RasClient_UnsubscribeRealTime(pTimeoutData->deviceId);
        AppLocalization_ResetPeer(pTimeoutData->deviceId, FALSE, gInvalidNvmIndex_c);
    }
}

/*! *********************************************************************************
*\fn            bool_t checkForLastSegment(deviceId_t deviceId);
*
*\brief         Checks if the received segment is the last segment.
*
*\param[in]     deviceId        Peer identifier
*
*\retval        bool_t  TRUE if the received segment is the last, FALSE otherwise
********************************************************************************** */
static bool_t checkForLastSegment
(
    deviceId_t deviceId
)
{
    bool_t lastSegmPresent = FALSE;

    for (uint8_t idx = 0U; idx < mRasTransferInfo[deviceId].currentIdxRecvSegm; idx++)
    {
        if ((mRasTransferInfo[deviceId].recvSegm[idx] & ((uint8_t)gRasNotifLastSegment_c)) != 0U)
        {
            lastSegmPresent = TRUE;
            break;
        }
    }

    return lastSegmPresent;
}

/*! *********************************************************************************
*\fn            bool_t checkIfSegmWasReceived(deviceId_t deviceId,
*               uint8_t segmHeader);
*
*\brief         Checks if the received segment is the header segment.
*
*\param[in]     deviceId        Peer identifier
*\param[in]     segmHeader      Segment header
*
*\retval        bool_t  TRUE if the received segment is the header, FALSE otherwise
********************************************************************************** */
static bool_t checkIfSegmWasReceived
(
    deviceId_t deviceId,
    uint8_t segmHeader
)
{
    bool_t segmPresent = FALSE;

    for (uint8_t idx = 0U; idx < mRasTransferInfo[deviceId].currentIdxRecvSegm; idx++)
    {
        if (mRasTransferInfo[deviceId].recvSegm[idx] == segmHeader)
        {
            segmPresent = TRUE;
            break;
        }
    }

    return segmPresent;
}

/*! *********************************************************************************
*\fn         bleResult_t RasClient_CPRspCompleteProcData(deviceId_t deviceId)
*
*\brief      Process indications received for the RAS control point characteristic
*            with the opcode completeProcDataRspOpCode_c.
*
*\param[in]  deviceId         Peer device id.
*
*\retval     gBleSuccess_c        Successful
*\retval     gBleUnavailable_c    Algorithm not run, app must call it directly
*\retval     bleResult_t          Result of call to RasClient_SendRasCommand
********************************************************************************** */
static bleResult_t RasClient_CPRspCompleteProcData
(
    deviceId_t  deviceId
)
{
    bleResult_t result = gBleSuccess_c;

    /* Stop RAP timer */
    (void)TM_Stop((timer_handle_t)mRreqTimerId);

    bool_t lastSegmPresent = checkForLastSegment(deviceId);

    /* Check if all segments have received from the RRSP */
    if ((lastSegmPresent == TRUE) && (mRasTransferInfo[deviceId].currentIdxLostSegm == 0U))
    {
#if defined(gAppDeferAlgoRun_d) && (gAppDeferAlgoRun_d == TRUE)
        result = gBleUnavailable_c;
#else
        AppLocalization_RunAlgorithm(deviceId);
#endif
    }
    else
    {
        if (mRasTransferInfo[deviceId].currentIdxLostSegm != 0U)
        {
            /* We have lost segments - send command to retrieve them */
            handleRetrLostRangingData(deviceId);

            /* Reset counter to receive notifications */
            mRasTransferInfo[deviceId].crtTempDataIdx = 0U;
            mRasTransferInfo[deviceId].crtIdxRecvLost = 0U;
            mRasTransferInfo[deviceId].expectingSegments = TRUE;
        }
        else
        {
            /* We lost the last segment - request everything from the last received */
            uint8_t lastIdx = mRasTransferInfo[deviceId].currentIdxRecvSegm - 1U;
            uint8_t startRollingCounter = (uint8_t)((mRasTransferInfo[deviceId].recvSegm[lastIdx]) >> 2U);
            mRasTransferInfo[deviceId].expectingSegments = TRUE;
            mRasTransferInfo[deviceId].crtIdxRecvLost = 0U;

            (void)RasClient_SendRasCommand(deviceId, retrLostRangingDataOpCode_c,
                                           startRollingCounter, 0xFF,
                                           AppLocalization_GetGlobalProcedureCount(deviceId),
                                           (uint16_t)gNoFilter_c);
        }
    }

    return result;
}

/*! *********************************************************************************
*\fn         bleResult_t RasClient_CPRspCompleteLostDataSegment(deviceId_t deviceId)
*
*\brief      Process indications received for the RAS control point characteristic
*            with the opcode completeLostDataSegmentResponseOpCode_c.
*
*\param[in]  deviceId         Peer device id.
*
*\retval     gBleSuccess_c        Successful
*\retval     gBleUnavailable_c    Algorithm not run, app must call it directly
*\retval     bleResult_t          Result of call to RasClient_SendRasCommand
********************************************************************************** */
static bleResult_t RasClient_CPRspCompleteLostDataSegment
(
    deviceId_t  deviceId
)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t idx = 0U;

    (void)TM_Stop((timer_handle_t)mRreqTimerId);

    /* Check if there are more segments to be received */
    do
    {
        if (mRasTransferInfo[deviceId].lostSegm[idx] != 0U)
        {
            break;
        }
        idx++;

    } while (idx < mRasTransferInfo[deviceId].currentIdxLostSegm);

    if (idx >= mRasTransferInfo[deviceId].currentIdxLostSegm)
    {
        /* All segments received - check if last segment was included */
        bool_t lastSegmPresent = checkForLastSegment(deviceId);

        /* Parse buffered data, if it exists */
        mRasTransferInfo[deviceId].expectingSegments = FALSE;
        if (mRasTransferInfo[deviceId].pNotifTempBuffer != NULL)
        {
            parseBufferedNotifs(deviceId);
        }

        if (lastSegmPresent == TRUE)
        {
#if defined(gAppDeferAlgoRun_d) && (gAppDeferAlgoRun_d == TRUE)
            result = gBleUnavailable_c;
#else
            AppLocalization_RunAlgorithm(deviceId);
#endif
        }
        else
        {
            /* We lost the last segment - request everything from the last received */
            uint8_t lastIdx = mRasTransferInfo[deviceId].currentIdxRecvSegm - 1U;
            uint8_t startRollingCounter = (uint8_t)((mRasTransferInfo[deviceId].recvSegm[lastIdx]) >> 2U);

            mRasTransferInfo[deviceId].expectingSegments = TRUE;
            mRasTransferInfo[deviceId].crtIdxRecvLost = 0U;
            (void)RasClient_SendRasCommand(deviceId, retrLostRangingDataOpCode_c,
                                           startRollingCounter, 0xFF,
                                           AppLocalization_GetGlobalProcedureCount(deviceId),
                                           (uint16_t)gNoFilter_c);
        }
    }
    else
    {
        /* We have lost segments - send command to retrieve them */
        handleRetrLostRangingData(deviceId);
    }

    return result;
}

/*! *********************************************************************************
*\fn         bleResult_t RasClient_CPRspResponse(deviceId_t deviceId,
*            rasControlPointRsp_t* pRasIndication)
*
*\brief      Process indications received for the RAS control point characteristic
*            with the opcode responseOpCode_c.
*
*\param[in]  deviceId         Peer device id.
*\param[in]  pRasIndication   RAS Control Point Response payload.
*
*\retval     gBleSuccess_c        Successful
*\retval     bleResult_t          Result of call to RasClient_SendRasCommand
********************************************************************************** */
static bleResult_t RasClient_CPRspResponse
(
    deviceId_t  deviceId,
    rasControlPointRsp_t* pRasIndication
)
{
    bleResult_t result = gBleSuccess_c;

    /* Inform application if an error response was received */
    if ((pRasIndication->cmdParameters.rspPayload.rspValue != ((uint8_t)gRasSuccess_c)) &&
       (pRasIndication->cmdParameters.rspPayload.rspValue != ((uint8_t)gRasSuccessPersisted_c)) &&
       (pRasIndication->cmdParameters.rspPayload.rspValue != ((uint8_t)gRasErrorRfu_c)))
    {
        /* An error occured - free resources on server  */
        union
        {
            appCsEventType_t evtType;
            uint8_t evtTypeu8;
        } revEvt;

        revEvt.evtTypeu8 = (uint8_t)pRasIndication->cmdParameters.rspPayload.rspValue +
                           (uint8_t)(gErrRasOpCodeNotSupported_c)-(uint8_t)(gRasOpCodeNotSupportedError_c);

        /* Ignore RFU error codes */
        if (pRasIndication->cmdParameters.rspPayload.rspValue < ((uint8_t)gRasNoRecordsFoundError_c))
        {
            /* Command failed - clear local data and wait for next procedure */
            AppLocalization_ClearLocalData(deviceId);
            /* Clear peer data */
            FLib_MemSet(&mPeerResultData[deviceId], 0x00, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
            if (mPeerResultData[deviceId].pData != NULL)
            {
                FLib_MemSet(mPeerResultData[deviceId].pData, 0U, gRasCsSubeventDataSize_c);
            }

            if (mRasTransferInfo[deviceId].pNotifTempBuffer != NULL)
            {
                (void)MEM_BufferFree(mRasTransferInfo[deviceId].pNotifTempBuffer);
                mRasTransferInfo[deviceId].pNotifTempBuffer = NULL;
            }

            if (mpfAppCallback != NULL)
            {
                mpfAppCallback(deviceId, NULL, revEvt.evtType);
            }
        }
    }

    return result;
}

/*! *********************************************************************************
*\fn         bleResult_t RasClient_ProcessGetRecordSegmentsResponse(deviceId_t deviceId,
*            uint8_t*   pValue, uint16_t   valueLength)
*
*\brief      Store missing measurement data from a response to a Get Record Segments request.
*
*\param[in]  deviceId         Peer device id.
*\param[in]  pValue           Pointer to value.
*\param[in]  valueLength      Value length.
*
*\retval     bleResult_t      Result of the operation.
********************************************************************************** */
static bleResult_t RasClient_ProcessGetRecordSegmentsResponse
(
    deviceId_t deviceId,
    uint8_t*   pValue,
    uint16_t   valueLength
)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t* pRangingData = pValue;
    uint8_t segmentHeader;
    uint8_t* pData;
    uint8_t crtIdx = mRasTransferInfo[deviceId].crtIdxRecvLost;

    /* Extract Segment Header */
    segmentHeader = *pRangingData++;

    /* First measurement - fill procedure data header */
    if (FALSE == checkIfSegmWasReceived(deviceId, segmentHeader))
    {
        FLib_MemCpy(mPeerResultData[deviceId].pData + mPeerResultData[deviceId].totalSentRcvDataIndex,
                    pRangingData,
                    (valueLength - sizeof(uint8_t)));

        mPeerResultData[deviceId].totalSentRcvDataIndex += (uint16_t)(valueLength - sizeof(uint8_t));

        /* Mark segment as received */
        if (mRasTransferInfo[deviceId].currentIdxLostSegm > 0U)
        {
            mRasTransferInfo[deviceId].lostSegm[crtIdx] = 0x0U;
            crtIdx++;
            mRasTransferInfo[deviceId].crtIdxRecvLost++;
            mRasTransferInfo[deviceId].recvSegm[mRasTransferInfo[deviceId].currentIdxRecvSegm] = segmentHeader;
            mRasTransferInfo[deviceId].currentIdxRecvSegm++;
        }
        else
        {
            /* Copy segmentation header information for the received segments */
            mRasTransferInfo[deviceId].recvSegm[mRasTransferInfo[deviceId].currentIdxRecvSegm] = segmentHeader;
            mRasTransferInfo[deviceId].currentIdxRecvSegm++;
        }
    }

    /* Check if there are received notifications in-between the missing segments */
    if (mRasTransferInfo[deviceId].currentIdxLostSegm > 1U)
    {
        for (uint8_t recvIdx = 0; recvIdx < mRasTransferInfo[deviceId].currentIdxRecvIntermSegm; recvIdx++)
        {
            if (mRasTransferInfo[deviceId].recvIntermSegm[recvIdx] < mRasTransferInfo[deviceId].lostSegm[crtIdx])
            {
                pData = mRasTransferInfo[deviceId].pNotifTempBuffer + mRasTransferInfo[deviceId].crtTempDataIdx;
                /* Skip segment counter */
                pData++;
                /* Parse buffered notification before moving on to the next mising segment */
                FLib_MemCpy(mPeerResultData[deviceId].pData + mPeerResultData[deviceId].totalSentRcvDataIndex,
                            pData,
                            ((uint32_t)(mRasTransferInfo[deviceId].recvIntermSegmLen[recvIdx]) - 1U));
                mPeerResultData[deviceId].totalSentRcvDataIndex += (mRasTransferInfo[deviceId].recvIntermSegmLen[recvIdx] - 1U);

                mRasTransferInfo[deviceId].crtTempDataIdx += mRasTransferInfo[deviceId].recvIntermSegmLen[recvIdx];
                /* mark segment as copied */
                mRasTransferInfo[deviceId].recvIntermSegm[recvIdx] = 0xFF;
            }
            else
            {
                break;
            }
        }
    }

    if (crtIdx == mRasTransferInfo[deviceId].currentIdxLostSegm)
    {
        /* We received the last segment */
        mRasTransferInfo[deviceId].crtIdxRecvLost = 0U;
    }

    return result;
}

#endif /* gRasRREQ_d */
#endif /* gAppRasDataTransfer_d */