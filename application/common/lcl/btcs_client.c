/*! *********************************************************************************
* Copyright 2025 - 2026 NXP
*
* \file btcs_server.c
*
* This is a source file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
#include "EmbeddedTypes.h"
#include "fsl_component_mem_manager.h"
#include "fwk_platform.h"
#include "ble_utils.h"

#include "channel_sounding.h"
#include "btcs_client_interface.h"
#include "app_localization.h"
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1U)
#include "fsl_component_timer_manager.h"
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1U) */

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
/* Maximum size of the payload to be transmitted through L2CAP */
#define gMaxPayloadSize_c       246U

/* Maximum number of segments */
#define gBTCSMaxNoOfSegments_c  15U

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef struct btcsClientSubEvtInfo_tag
{
    uint8_t parsedStepsCrtSubEvt; /* Number of steps we have received for the current subevent */
    uint8_t crtSubEvtIdx;         /* Index of the current subevent */
} btcsClientSubEvtInfo_t;

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
/* Remote measurement data  */
static rasMeasurementData_t mPeerResultData[gAppMaxConnections_c];

/* Subevent index and number of steps information for each peer */
static btcsClientSubEvtInfo_t mSubEvtInfo[gAppMaxConnections_c];

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
/* Handler function for the BTCS Ranging Procedure Results Start message */
static bleResult_t handleRangingProcResStart
(
    deviceId_t deviceId,
    uint16_t   packetLen,
    uint8_t*   pMsgData
);

/* Handler function for the BTCS Ranging Procedure Results Continue message */
static bleResult_t handleRangingProcResCont
(
    deviceId_t deviceId,
    uint16_t   packetLen,
    uint8_t*   pMsgData
);

/* Helper function to save the received data and count the number of steps */
static void parseRecvNoOfSteps
(
    deviceId_t deviceId,
    uint16_t   packetLen,
    uint8_t*   pMsgData
);

/* Helper function to parse a CSSubEventHeader message fragment */
static void parseSubEvtHeader
(
    deviceId_t deviceId,
    uint8_t*   pMsgData,
    uint8_t*   pOutParsedLen
);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn            void BtcsClient_Init(void)
*
*\brief         Initialize the internal BTCS client structures
*
*\param[in]     none
*
*\retval        none
********************************************************************************** */
void BtcsClient_Init(void)
{
    for (uint8_t idx = 0U; idx < (uint8_t)gAppMaxConnections_c; idx++)
    {
        mSubEvtInfo[idx].crtSubEvtIdx = 0U;
        mSubEvtInfo[idx].parsedStepsCrtSubEvt = 0U;
    }
}

/*! *********************************************************************************
*\fn            void BtcsClient_ResetPeer(deviceId_t deviceId, bool_t disconnected)
*
*\brief         Clears the internal BTCS client data
*
*\param[in]     deviceId         Peer identifier
*\param[in]     disconnected     TRUE if peer has disconnected, FALSE otherwise
*
*\retval        none
********************************************************************************** */
void BtcsClient_ResetPeer
(
    deviceId_t deviceId,
    bool_t     disconnected
)
{
    /* Clean up ranging data */
    if (mPeerResultData[deviceId].pData != NULL)
    {
        FLib_MemSet(mPeerResultData[deviceId].pData, 0U, gRasCsSubeventDataSize_c);
    }
    mSubEvtInfo[deviceId].crtSubEvtIdx = 0U;
    mSubEvtInfo[deviceId].parsedStepsCrtSubEvt = 0U;

    if (disconnected == TRUE)
    {
        /* Free allocated buffer */
        (void)MEM_BufferFree(mPeerResultData[deviceId].pData);
        mPeerResultData[deviceId].pData = NULL;
    }
}

/*! *********************************************************************************
*\fn            void BtcsClient_HandleRangingServiceMsg(deviceId_t deviceId,
*                                                       uint8_t* pMsgData)
*
*\brief         Handles BTCS messages reveived from a peer.
*
*\param[in]     deviceId         Peer identifier
*\param[in]     pMsgData         Pointer to the BTCS message
*
*\retval        gBleSuccess_c or error
********************************************************************************** */
bleResult_t BtcsClient_HandleRangingServiceMsg
(
    deviceId_t deviceId,
    uint8_t*   pMsgData
)
{
    bleResult_t result = gBleSuccess_c;

    uint8_t *pData = &pMsgData[gMessageHeaderSize_c];
    /* Extract message type */
    btcsMsgId_t msgId = (btcsMsgId_t)(*pData);
    pData = &pData[gPayloadHeaderSize_c];
    /* Extract message length */
    uint16_t packetLen = Utils_BeExtractTwoByteValue(pData);
    pData = &pData[gLengthFieldSize_c];

    if (msgId == gRangingProcResStart_c)
    {
        /* Received a BTCS_Ranging_Procedure_Results_Start command */
        result = handleRangingProcResStart(deviceId, packetLen, pData);
    }

    if (msgId == gRangingProcResCont_c)
    {
        /* Received a BTCS_Ranging_Procedure_Results_Continue command */
        result = handleRangingProcResCont(deviceId, packetLen, pData);
    }

    return result;
}

/*! *********************************************************************************
*\fn            void BtcsClient_GetPeerRangingData(deviceId_t deviceId)
*
*\brief         Returns a pointer to the BTCS data of the client.
*
*\param[in]     deviceId         Peer identifier
*
*\retval        Pointer to the BTCS data of the client
********************************************************************************** */
rasMeasurementData_t* BtcsClient_GetPeerRangingData
(
    deviceId_t deviceId
)
{
    return &mPeerResultData[deviceId];
}

/*! *********************************************************************************
*\fn            uint16_t BtcsClient_GetPeerRangingDataSize(deviceId_t deviceId);
*
*\brief         Get the size of the peer ranging data
*
*\param[in]     deviceId        Peer identifier
*
*\retval        Peer ranging data size
********************************************************************************** */
uint16_t BtcsClient_GetPeerRangingDataSize
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
*\fn         static void parseSubEvtHeader(deviceId_t deviceId,
*                                             uint16_t packetLen, uint8_t*   pMsgData)
*
*\brief      Helper function to parse a CSSubEventHeader message fragment
*
*\param[in]  deviceId         Peer identifier
*\param[in]  pMsgData         Pointer to message data
*\param[out] pOutParsedLen    Size of the parsed data
*
*\retval     none
********************************************************************************** */
static void parseSubEvtHeader
(
    deviceId_t deviceId,
    uint8_t*   pMsgData,
    uint8_t*   pOutParsedLen
)
{
    uint8_t *pData = pMsgData;
    uint8_t subEvtIdx = mPeerResultData[deviceId].subeventIndex;

    union
    {
        uint8_t u8;
        uint32_t u32;
    }dataLen = {0U};

    /* Extract subevent header information */
    if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
    {
        /* Peer is reflector */
        gCsSubEvtHeaderReflData_t subEvtHeader;
        FLib_MemCpy(&subEvtHeader, pData, sizeof(gCsSubEvtHeaderReflData_t));
        pData = &pData[sizeof(gCsSubEvtHeaderReflData_t)];
        dataLen.u32 = sizeof(gCsSubEvtHeaderReflData_t);
        *pOutParsedLen = dataLen.u8;

        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.numStepsReported = subEvtHeader.totalSubEvtSteps;
        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.startACLConnEvent = subEvtHeader.startACLConnEvt;
        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.abortReason = subEvtHeader.abortReason;
        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.referencePowerLevel = subEvtHeader.referencePowerLevel;
        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.procedureDoneStatus = subEvtHeader.procEvtDoneStatus & 0x0FU;
        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.subeventDoneStatus = subEvtHeader.procEvtDoneStatus & 0xF0U;
    }
    else
    {
        /* Peer is initiator */
        gCsSubEvtHeaderInitData_t subEvtHeader;
        FLib_MemCpy(&subEvtHeader, pData, sizeof(gCsSubEvtHeaderInitData_t));
        pData = &pData[sizeof(gCsSubEvtHeaderInitData_t)];
        dataLen.u32 = sizeof(gCsSubEvtHeaderInitData_t);
        *pOutParsedLen = dataLen.u8;

        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.numStepsReported = subEvtHeader.totalSubEvtSteps;
        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.startACLConnEvent = subEvtHeader.startACLConnEvt;
        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.frequencyCompensation = subEvtHeader.freqCompensation;
        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.abortReason = subEvtHeader.abortReason;
        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.referencePowerLevel = subEvtHeader.referencePowerLevel;
        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.procedureDoneStatus = subEvtHeader.procEvtDoneStatus & 0x0FU;
        mPeerResultData[deviceId].aSubEventData[subEvtIdx].subevtHeader.subeventDoneStatus = subEvtHeader.procEvtDoneStatus & 0xF0U;
    }
}

/*! *********************************************************************************
*\fn         static void handleRangingProcResStart(deviceId_t deviceId,
*                                             uint16_t packetLen, uint8_t*   pMsgData)
*
*\brief      Handler function for the BTCS Ranging Procedure Results Start message
*
*\param[in]  deviceId         Peer identifier
*\param[in]  packetLen        Size of the message
*\param[in]  pMsgData         Pointer to message data
*
*\retval     gBleSuccess_c or error
********************************************************************************** */
static bleResult_t handleRangingProcResStart
(
    deviceId_t deviceId,
    uint16_t   packetLen,
    uint8_t*   pMsgData
)
{
    bleResult_t result = gBleSuccess_c;
    gCsProcHeaderData_t procHeader;
    uint8_t* pData = pMsgData;
    uint8_t outParsedLen = 0U;
    uint16_t parsedDataLen = 0U;

    union
    {
        uint16_t u16;
        uint32_t u32;
    }dataLen = {0U};

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1U)
    /* Set transferStart when first L2CAP paket is received */
    if (gCsTimeInfo.transferStart == 0U)
    {
        gCsTimeInfo.transferStart = TM_GetTimestamp();
    }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1U) */

    /* Reset data in preparation for a new procedure */
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
        /* Clean up data */
        FLib_MemSet(&mPeerResultData[deviceId], 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
        FLib_MemSet(mPeerResultData[deviceId].pData, 0U, gRasCsSubeventDataSize_c);
        mSubEvtInfo[deviceId].crtSubEvtIdx = 0U;
        mSubEvtInfo[deviceId].parsedStepsCrtSubEvt = 0U;

        /* Parse procedure header */
        FLib_MemCpy(&procHeader, pMsgData, sizeof(gCsProcHeaderData_t));
        mPeerResultData[deviceId].configId = procHeader.configId;
        mPeerResultData[deviceId].procedureCounter = procHeader.seqNo;
        mPeerResultData[deviceId].numAntennaPaths = AppLocalization_GetNumAntennaPaths(deviceId);
        pData = &pData[sizeof(gCsProcHeaderData_t)];
        dataLen.u32 = sizeof(gCsProcHeaderData_t);
        parsedDataLen = dataLen.u16;

        /* Parse subevent header */
        parseSubEvtHeader(deviceId, pData, &outParsedLen);
        pData = &pData[outParsedLen];
        parsedDataLen += outParsedLen;

        /* Count the received number of steps */
        parseRecvNoOfSteps(deviceId, (packetLen-parsedDataLen), pData);
        mPeerResultData[deviceId].totalSentRcvDataIndex = (packetLen-parsedDataLen);
    }

    return result;
}

/*! *********************************************************************************
*\fn         static void handleRangingProcResCont(deviceId_t deviceId,
*                                             uint16_t packetLen, uint8_t*   pMsgData)
*
*\brief      Handler function for the BTCS Ranging Procedure Results Continue message
*
*\param[in]  deviceId         Peer identifier
*\param[in]  packetLen        Size of the message
*\param[in]  pMsgData         Pointer to message data
*
*\retval     gBleSuccess_c or error
********************************************************************************** */
static bleResult_t handleRangingProcResCont
(
    deviceId_t deviceId,
    uint16_t   packetLen,
    uint8_t*   pMsgData
)
{
    bleResult_t result = gBleSuccess_c;
    gCsProcContHeaderData_t procHeader;
    uint8_t* pData = pMsgData;
    uint8_t stepIdx = mSubEvtInfo[deviceId].crtSubEvtIdx;
    uint8_t outParsedLen = 0U;
    uint16_t parsedDataLen = 0U;
    gCsSubEvtContHeaderData_t subEvtContHeader;

    union
    {
        uint16_t u16;
        uint32_t u32;
    }dataLen = {0U};

    if (mPeerResultData[deviceId].pData == NULL)
    {
        /* Should not get here */
        result = gBleInvalidState_c;
    }

    if (result == gBleSuccess_c)
    {
        /* Parse procedure header */
        FLib_MemCpy(&procHeader, pData, sizeof(gCsProcContHeaderData_t));
        pData = &pData[sizeof(gCsProcContHeaderData_t)];
        dataLen.u32 = sizeof(gCsProcContHeaderData_t);
        parsedDataLen += dataLen.u16;

        /* Check that we received data for the current procedure */
        if (mPeerResultData[deviceId].procedureCounter != procHeader.seqNo)
        {
            result = gBleInvalidParameter_c;
        }
        else
        {
            if (mSubEvtInfo[deviceId].parsedStepsCrtSubEvt >=
                mPeerResultData[deviceId].aSubEventData[stepIdx].subevtHeader.numStepsReported)
            {
                /* We're starting a new subevet */
                mSubEvtInfo[deviceId].crtSubEvtIdx++;
                mPeerResultData[deviceId].subeventIndex++;
                mSubEvtInfo[deviceId].parsedStepsCrtSubEvt = 0U;

                /* Parse subevent header */
                parseSubEvtHeader(deviceId, pData, &outParsedLen);
                pData = &pData[outParsedLen];
                parsedDataLen += outParsedLen;
            }
            else
            {
                /* We received additional data for the current subevent */
                FLib_MemCpy(&subEvtContHeader, pData, sizeof(gCsSubEvtContHeaderData_t));
                pData = &pData[sizeof(gCsSubEvtContHeaderData_t)];
                dataLen.u32 = sizeof(gCsSubEvtContHeaderData_t);
                parsedDataLen += dataLen.u16;
            }

            /* Count the received number of steps */
            parseRecvNoOfSteps(deviceId,
                               (packetLen-parsedDataLen),
                               pData);

            stepIdx = mPeerResultData[deviceId].subeventIndex;

            /* Check if we reached the end of the transfer */
            if ((mSubEvtInfo[deviceId].parsedStepsCrtSubEvt >=
                 mPeerResultData[deviceId].aSubEventData[stepIdx].subevtHeader.numStepsReported) &&
                (mPeerResultData[deviceId].aSubEventData[stepIdx].subevtHeader.procedureDoneStatus == (uint8_t)gCsCompleteResults_c))
            {
#if defined(gAppDeferAlgoRun_d) && (gAppDeferAlgoRun_d == TRUE)
                result = gBleUnavailable_c;
#else
                /* Call algo */
                AppLocalization_RunAlgorithm(deviceId);
#endif
            }
        }
    }

    return result;
}

/*! *********************************************************************************
*\fn         static void parseRecvNoOfSteps(deviceId_t deviceId,
*                                           uint16_t packetLen, uint8_t* pMsgData)
*
*\brief       Helper function to save the received data and count the number of steps
*
*\param[in]  deviceId         Peer identifier
*\param[in]  deviceId         Peer identifier
*\param[in]  deviceId         Peer identifier
*
*\retval     none
********************************************************************************** */
static void parseRecvNoOfSteps
(
    deviceId_t deviceId,
    uint16_t   packetLen,
    uint8_t*   pMsgData
)
{
    uint8_t* pData = pMsgData;
    uint16_t parsedDataLen = 0U;
    uint8_t mode = 0U;
    uint8_t stepIdx = mSubEvtInfo[deviceId].crtSubEvtIdx;

    union
    {
        uint16_t u16;
        uint32_t u32;
    }dataLen = {0U};

    while (parsedDataLen < packetLen)
    {
        dataLen.u32 = 0U;
        /* Step mode value */
        mode = *pData++;
        parsedDataLen++;

        assert(mode <= gCsStepMode3_c);

        mPeerResultData[deviceId].pData[mPeerResultData[deviceId].totalSentRcvDataIndex] = mode;
        mPeerResultData[deviceId].totalSentRcvDataIndex++;

        switch (mode)
        {
            case (uint8_t)gCsStepMode0_c:
            {
               FLib_MemCpy(mPeerResultData[deviceId].pData + mPeerResultData[deviceId].totalSentRcvDataIndex,
                           pData,
                           gMode0DataSize_c);

                pData = &pData[gMode0DataSize_c];
                parsedDataLen += gMode0DataSize_c;
                mPeerResultData[deviceId].totalSentRcvDataIndex += gMode0DataSize_c;

                if (mGlobalRangeSettings.role == gCsRoleReflector_c)
                {
                    /* Peer is initiator - parse additional mode 0 data */
                    FLib_MemCpy(mPeerResultData[deviceId].pData + mPeerResultData[deviceId].totalSentRcvDataIndex,
                             pData,
                             sizeof(uint16_t));

                    mPeerResultData[deviceId].totalSentRcvDataIndex += sizeof(uint16_t);
                    dataLen.u32 = sizeof(uint16_t);
                    pData = &pData[sizeof(uint16_t)];
                    parsedDataLen += dataLen.u16;
                }
            }
            break;

            case (uint8_t)gCsStepMode1_c:
            {
                FLib_MemCpy(mPeerResultData[deviceId].pData + mPeerResultData[deviceId].totalSentRcvDataIndex,
                             pData,
                             gMode1DataSize_c);

                mPeerResultData[deviceId].totalSentRcvDataIndex += gMode1DataSize_c;
                pData = &pData[gMode1DataSize_c];
                parsedDataLen += gMode1DataSize_c;
            }
            break;

            case (uint8_t)gCsStepMode2_c:
            {
                /* 1 byte quality + 3 bytes per antenna path for Tone_PCT */
                uint8_t mode2DataSize = (mPeerResultData[deviceId].numAntennaPaths*gTone_PCTSize_c) + 1U;

                FLib_MemCpy(mPeerResultData[deviceId].pData + mPeerResultData[deviceId].totalSentRcvDataIndex,
                             pData,
                             mode2DataSize);

                mPeerResultData[deviceId].totalSentRcvDataIndex += mode2DataSize;
                pData = &pData[mode2DataSize];
                parsedDataLen += mode2DataSize;
            }
            break;

            case (uint8_t)gCsStepMode3_c:
            {
                /* 1 byte quality + 3 bytes per antenna path for Tone_PCT */
                uint8_t mode3DataSize = (mPeerResultData[deviceId].numAntennaPaths*gTone_PCTSize_c) + 1U + gMode1DataSize_c;

                FLib_MemCpy(mPeerResultData[deviceId].pData + mPeerResultData[deviceId].totalSentRcvDataIndex,
                             pData,
                             mode3DataSize);

                mPeerResultData[deviceId].totalSentRcvDataIndex += mode3DataSize;

                pData = &pData[mode3DataSize];
                parsedDataLen += mode3DataSize;
            }
            break;

            default:
            {
                /* Should not get here */
                assert(FALSE);
            }
            break;
        }

        mSubEvtInfo[deviceId].parsedStepsCrtSubEvt++;

        /* We reached the end of the current subevent - parse the header for the new one */
        if ((mSubEvtInfo[deviceId].parsedStepsCrtSubEvt ==
            mPeerResultData[deviceId].aSubEventData[stepIdx].subevtHeader.numStepsReported) &&
            (mPeerResultData[deviceId].aSubEventData[stepIdx].subevtHeader.procedureDoneStatus != (uint8_t)gCsCompleteResults_c))
        {
            uint8_t outParsedLen = 0U;
            stepIdx++;
            mSubEvtInfo[deviceId].crtSubEvtIdx++;
            /* Increase subevent index */
            mPeerResultData[deviceId].subeventIndex++;
            /* Parse subevent header */
            parseSubEvtHeader(deviceId, pData, &outParsedLen);
            /* Increase counters */
            pData = &pData[outParsedLen];
            parsedDataLen += outParsedLen;
            mSubEvtInfo[deviceId].parsedStepsCrtSubEvt = 0U;
        }
    }
}

#endif /* gAppBtcsClient_d */
