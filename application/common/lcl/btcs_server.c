/*! *********************************************************************************
* Copyright 2025 - 2026 NXP
*
* NXP Proprietary
*
* This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or
* by downloading, installing, activating and/or otherwise using the software, you are
* agreeing that you have read, and that you agree to comply with and are bound by,
* such license terms. If you do not agree to be bound by the applicable license terms,
* then you may not retain, install, activate or otherwise use the software.
********************************************************************************** */
/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
#include "EmbeddedTypes.h"
#include "fsl_component_mem_manager.h"
#include "fwk_platform.h"

#include "channel_sounding.h"
#include "btcs_server_interface.h"
#include "app_localization_utils.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
/* Maximum size of the payload to be transmitted through L2CAP */
#define gMaxPayloadSize_c       246U

/* Maximum number of segments */
#define gBTCSMaxNoOfSegments_c  15U

/* Step mode array packing: bits per nibble */
#define gStepModeMask_c         0x03U  /* Bits 0-1 or 4-5: Mode */
#define gStepStatusBit_c        0x08U  /* Bit 3 or 7: Status (1=aborted) */
#define gStepModeOddPad_c       0xF0U  /* Padding for odd number of steps */

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef PACKED_STRUCT btcsRangingData_tag
{
    rasMeasurementData_t* pCfg;         /* configuration information + raw data */
    uint8_t*              pRangingData; /* Parsed ranging data body */
} btcsRangingData_t;

/* Segment indexation information */
typedef PACKED_STRUCT btcsSegmIdx_tag
{
    uint16_t aSegmIdx[gBTCSMaxNoOfSegments_c]; /* Index at which the segment begins */
    uint16_t numStepsIdx;                      /* Index at which the last number of steps is stored */
    uint16_t numStepsHdrIdx;                   /* Index at which the total number of steps for the current subevent is stored */
    uint16_t procDoneStatusIdx;                /* Index at which the procedure done and subevent done status field is stored for the current subevent */
    uint16_t modeArrayStartIdx;                /* Index at which the current subevent mode array begins */
    uint8_t  modeArraySteps;                   /* Number of steps currently packed in mode array */
    uint8_t  crtIdx;                           /* Current number of segments */
} btcsSegmIdx_t;

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
/* Ranging data for each peer */
static btcsRangingData_t mpRangingData[gAppMaxConnections_c] = {NULL};

static btcsSegmIdx_t* mpSegmIdx[gAppMaxConnections_c];

static bool_t mbpTransferInProgress[gAppMaxConnections_c] = {FALSE};

/* Antenna permutation order */
static const uint8_t maAntPermNAp[24][4] = {
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

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static uint16_t parseSubeventData
(
    deviceId_t deviceId,
    uint8_t    subEvtIdx,
    uint16_t   maxDataLen,
    uint8_t*   pSrcBuff,
    uint8_t*   pOutNumSteps
);

static void packSubEvtHeader
(
    deviceId_t deviceId,
    uint16_t*  pOutMaxDataSize
);

static uint16_t computeStepOutputSize
(
    uint8_t mode,
    uint8_t stepDataLen,
    uint8_t numAntennaPaths
);

static uint8_t* writeStepData
(
    deviceId_t deviceId,
    uint8_t    mode,
    uint8_t    stepDataLength,
    uint8_t*   pStepDataAux,
    uint8_t    numAntennaPaths
);

static void buildProcHeader
(
    deviceId_t deviceId,
    uint8_t*   pEventData
);

static void buildSubEvtCont
(
    deviceId_t deviceId,
    uint8_t*   pEventData
);

static void buildSubEvtNew
(
    deviceId_t deviceId,
    uint8_t*   pEventData
);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*!**********************************************************************************
* \brief        Initialize internal BTCS structures
*
* \param[in]    none
*
* \return       none.
************************************************************************************/
void BtcsServer_Init(void)
{
    for (uint8_t i = 0U; i < (uint8_t)gAppMaxConnections_c; i++)
    {
        mpRangingData[i].pRangingData = NULL;
        mpRangingData[i].pCfg = NULL;
        mpSegmIdx[i] = NULL;
        mbpTransferInProgress[i] = FALSE;
    }
}

/*!**********************************************************************************
* \brief        Set the received configuration information for the given peer
*
* \param[in]    deviceId    Peer device ID.
* \param[in]    pCfg        Pointer to the local result data
*
* \return       none.
************************************************************************************/
void BtcsServer_SetServerCfg
(
    deviceId_t            deviceId,
    rasMeasurementData_t* pCfg
)
{
    mpRangingData[deviceId].pCfg = pCfg;
}

/*!**********************************************************************************
* \brief        Build the ranging data information for the given peer
*
* \param[in]    deviceId    Peer device ID.
* \param[in]    pEventData  Pointer to Channel Sounding measurement data.
* \param[in]    numSteps    Number of steps included in the subevent
* \param[in]    fragmType   CSProcedureData Message Fragment type
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t BtcsServer_BuildRangingData
(
    deviceId_t             deviceId,
    uint8_t                *pEventData,
    uint8_t                numSteps,
    btcsProcDataMsgFragm_t fragmType
)
{
    bleResult_t result = gBleSuccess_c;
    (void)numSteps; /* Used by caller context; steps are counted internally by parseSubeventData */

    if (mpRangingData[deviceId].pCfg == NULL)
    {
        result = gBleInvalidParameter_c;
    }

    if (result == gBleSuccess_c)
    {
        if (mpRangingData[deviceId].pRangingData == NULL)
        {
            /* Allocate ranging data */
            mpRangingData[deviceId].pRangingData = MEM_BufferAlloc(gMaxCsSubeventDataSize_c);
            if (mpRangingData[deviceId].pRangingData == NULL)
            {
                result = gBleOutOfMemory_c;
            }
            else
            {
                FLib_MemSet(mpRangingData[deviceId].pRangingData, 0U, gMaxCsSubeventDataSize_c);
            }
        }
    }

    if (result == gBleSuccess_c)
    {
        if (mpSegmIdx[deviceId] == NULL)
        {
            /* Allocate segment information */
            mpSegmIdx[deviceId] = MEM_BufferAlloc(sizeof(btcsSegmIdx_t));
            if (mpSegmIdx[deviceId] == NULL)
            {
                result = gBleOutOfMemory_c;
            }
            else
            {
                FLib_MemSet(mpSegmIdx[deviceId], 0U, sizeof(btcsSegmIdx_t));
            }
        }
    }

    if (result == gBleSuccess_c)
    {
        switch (fragmType)
        {
            case gCsProcHeader_c:
            {
                buildProcHeader(deviceId, pEventData);
            }
            break;

            case gCsSubEvtContHeader_c:
            {
                buildSubEvtCont(deviceId, pEventData);
            }
            break;

            case gCsSubEvtHeader_c:
            {
                buildSubEvtNew(deviceId, pEventData);
            }
            break;

            default:
            {
                /* do nothing */
            }
            break;
        }
    }

    return result;
}

/*!**********************************************************************************
* \brief        Send the ranging data information to the given peer
*
* \param[in]    deviceId    Peer device ID.
* \param[in]    channelId   L2CAP channel Id to be used
* \param[in]    btcsMsgId   BTCS Ranging Service Payload Type
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t BtcsServer_SendData
(
    deviceId_t   deviceId,
    uint16_t     channelId,
    btcsMsgId_t  btcsMsgId
)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t dataLen = 0U;
    uint8_t *pData = NULL;
    uint8_t crtSegmIdx = mpSegmIdx[deviceId]->crtIdx;

    pData = MEM_BufferAlloc(gBleMaxTxOctets_c);

    if (pData == NULL)
    {
        result = gBleOutOfMemory_c;
    }

    /* Starting to transfer the results from a BTCS procedure */
    if ((btcsMsgId == gRangingProcResStart_c) &&
        (result == gBleSuccess_c))
    {
        FLib_MemSet(pData, 0U, gBleMaxTxOctets_c);
        mbpTransferInProgress[deviceId] = TRUE;

        /* Build data packet */
        dataLen = mpSegmIdx[deviceId]->aSegmIdx[crtSegmIdx+1U];
        FLib_MemCpy(pData, mpRangingData[deviceId].pRangingData, dataLen);
        mpRangingData[deviceId].pCfg->dataParsedLen += dataLen;
    }

    if ((btcsMsgId == gRangingProcResCont_c) &&
        (result == gBleSuccess_c))
    {
        FLib_MemSet(pData, 0U, gBleMaxTxOctets_c);

        /* Build data packet */
        if (crtSegmIdx == 0U)
        {
            /* Should not get here */
            result = gBleInvalidState_c;
        }
        else
        {
            /* Build data packet */
            dataLen = (mpSegmIdx[deviceId]->aSegmIdx[crtSegmIdx+1U] - mpSegmIdx[deviceId]->aSegmIdx[crtSegmIdx]);
            FLib_MemCpy(pData,
                        mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->dataParsedLen,
                        dataLen);
            mpRangingData[deviceId].pCfg->dataParsedLen += dataLen;
        }
    }

    if (result == gBleSuccess_c)
    {
        /* Send packet */
        result = DK_SendMessage(deviceId, channelId,
                                gDKMessageTypeBTCSRangingServiceMessage_c,
                                (uint8_t)btcsMsgId, dataLen, pData);

        mpSegmIdx[deviceId]->crtIdx++;
    }

    if (mpRangingData[deviceId].pCfg->dataParsedLen == mpRangingData[deviceId].pCfg->totalSentRcvDataIndex)
    {
        /* Transfer finished. Clear local data */
        BtcsServer_ResetPeer(deviceId, FALSE);
    }

    if (pData != NULL)
    {
        (void)MEM_BufferFree(pData);
    }

    return result;
}

/*!**********************************************************************************
* \brief        Check if a BTCS data transfer is in progress with the given peer
*
* \param[in]    deviceId    Peer device ID.
*
* \return       TRUE if transfer is in progress, FALSE otehrwise
************************************************************************************/
bool_t BtcsServer_CheckTransferInProgress
(
    deviceId_t deviceId
)
{
    return mbpTransferInProgress[deviceId];
}

/*!**********************************************************************************
* \brief        Clear internal BTCS Server data for the given peer
*
* \param[in]    deviceId        Peer device ID.
* \param[in]    disconnected    TRUE if the peer has disconnected, FALSE otherwise
*
* \return       none
************************************************************************************/
void BtcsServer_ResetPeer
(
    deviceId_t deviceId,
    bool_t     disconnected
)
{
    /* Clean up ranging data */
    if (mpRangingData[deviceId].pRangingData != NULL)
    {
        FLib_MemSet(mpRangingData[deviceId].pRangingData, 0U, gMaxCsSubeventDataSize_c);
    }

    /* Clean up segment information */
    if (mpSegmIdx[deviceId] != NULL)
    {
        FLib_MemSet(mpSegmIdx[deviceId], 0U, sizeof(btcsSegmIdx_t));
    }

    /* Mark that no transfer is in progress */
    mbpTransferInProgress[deviceId] = FALSE;

    if (disconnected == TRUE)
    {
        /* Free allocated buffer */
        (void)MEM_BufferFree(mpRangingData[deviceId].pRangingData);
        mpRangingData[deviceId].pRangingData = NULL;
        /* Reset config pointer */
        mpRangingData[deviceId].pCfg = NULL;
    }
}

/************************************************************************************
*************************************************************************************
* Private Functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\brief      Helper function to pack a CSSubEventHeader message fragment
*
*\param[in]  deviceId          Peer identifier
*\param[out]  pOutMaxDataSize  Updated data size decreased with the size of the subevent header
*
*\retval     none
********************************************************************************** */
static void packSubEvtHeader
(
    deviceId_t deviceId,
    uint16_t*  pOutMaxDataSize
)
{
    uint8_t subEvtIdx = mpRangingData[deviceId].pCfg->subeventIndex;

    union
    {
        uint16_t u16;
        uint32_t u32;
    }dataLen = {0U};

    if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
    {
        /* Subevent header information for intitiator */
        gCsSubEvtHeaderInitData_t subEvtHeader;

        subEvtHeader.startACLConnEvt = mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.startACLConnEvent;
        subEvtHeader.procEvtDoneStatus = ((mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.subeventDoneStatus & 0x0FU) << 4U) |
          (mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.procedureDoneStatus & 0x0FU);
        subEvtHeader.abortReason = mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.abortReason;
        subEvtHeader.freqCompensation = mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.frequencyCompensation;
        subEvtHeader.PBRFormat = gPBRFormatIQ_c;
        subEvtHeader.referencePowerLevel = mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.referencePowerLevel;
        subEvtHeader.totalSubEvtSteps = 0U; /* Will need to be updated with the final step count for the subevent */
        subEvtHeader.numStepsReported = 0U; /* Step count in this segment, not for the whole subevent */

        /* Copy subevent header information and update parsed length */
        FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                    &subEvtHeader,
                    sizeof(gCsSubEvtHeaderInitData_t));

        /* Save the position of the totalSubEvtSteps field for later updates */
        dataLen.u32 = (2U*sizeof(uint16_t)) + (4U*sizeof(uint8_t));
        mpSegmIdx[deviceId]->numStepsHdrIdx = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex + dataLen.u16;
        mpSegmIdx[deviceId]->numStepsIdx = mpSegmIdx[deviceId]->numStepsHdrIdx + 1U;
        /* Save index for the procedure done status field for late updates */
        dataLen.u32 = sizeof(uint16_t);
        mpSegmIdx[deviceId]->procDoneStatusIdx = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex + dataLen.u16;

        /* Update counters */
        dataLen.u32 = sizeof(gCsSubEvtHeaderInitData_t);
        mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += dataLen.u16;
        *pOutMaxDataSize -= dataLen.u16;
    }
    else
    {
        /* Subevent header information for reflector */
        gCsSubEvtHeaderReflData_t subEvtHeader;

        subEvtHeader.startACLConnEvt = mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.startACLConnEvent;
        subEvtHeader.procEvtDoneStatus = ((mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.subeventDoneStatus & 0x0FU) << 4U) |
          (mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.procedureDoneStatus & 0x0FU);
        subEvtHeader.abortReason = mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.abortReason;
        subEvtHeader.PBRFormat = gPBRFormatIQ_c;
        subEvtHeader.referencePowerLevel = mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.referencePowerLevel;
        subEvtHeader.totalSubEvtSteps = 0U; /* Will need to be updated with the final step count for the subevent */
        subEvtHeader.numStepsReported = 0U; /* Step count in this segment, not for the whole subevent */

        /* Copy subevent header information and update parsed length */
        FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                    &subEvtHeader,
                    sizeof(gCsSubEvtHeaderReflData_t));

        /* Save the position of the totalSubEvtSteps field for later updates */
        dataLen.u32 = sizeof(uint16_t) + (4U*sizeof(uint8_t));
        mpSegmIdx[deviceId]->numStepsHdrIdx = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex + dataLen.u16;

        /* Save index for the procedure done status field for late updates */
        mpSegmIdx[deviceId]->numStepsIdx = mpSegmIdx[deviceId]->numStepsHdrIdx + 1U;
        /* Update procedure and subevent status */
        dataLen.u32 = sizeof(uint16_t);
        mpSegmIdx[deviceId]->procDoneStatusIdx = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex + dataLen.u16;

        /* Update counters */
        dataLen.u32 = sizeof(gCsSubEvtHeaderReflData_t);
        mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += dataLen.u16;
        *pOutMaxDataSize -= dataLen.u16;
    }
}

/*! *********************************************************************************
*\brief      Compute the output data size for a single step based on its mode.
*
*\param[in]  mode             Step mode (0-3)
*\param[in]  stepDataLen      Length of step data from HCI (0 = aborted)
*\param[in]  numAntennaPaths  Number of antenna paths in the configuration
*
*\retval     Output size in bytes (0 if aborted)
********************************************************************************** */
static uint16_t computeStepOutputSize
(
    uint8_t mode,
    uint8_t stepDataLen,
    uint8_t numAntennaPaths
)
{
    uint16_t stepOutputSize = 0U;

    if (stepDataLen > 0U)
    {
        switch (mode)
        {
            case (uint8_t)gCsStepMode0_c:
            {
                stepOutputSize = (uint16_t)stepDataLen;
            }
            break;
            case (uint8_t)gCsStepMode1_c:
            {
                stepOutputSize = gMode1DataSize_c;
            }
            break;
            case (uint8_t)gCsStepMode2_c:
            {
                /* Quality byte + PCT data for each antenna path */
                stepOutputSize = 1U + ((uint16_t)numAntennaPaths * gTone_PCTSize_c);
            }
            break;
            case (uint8_t)gCsStepMode3_c:
            {
                /* Mode-1 data + quality byte + PCT data for each antenna path */
                stepOutputSize = gMode1DataSize_c + 1U + ((uint16_t)numAntennaPaths * gTone_PCTSize_c);
            }
            break;
            default:
            {
                /* Nothing to do */
            }
            break;
        }
    }

    return stepOutputSize;
}

/*! *********************************************************************************
*\brief      Write mode-specific step measurement data to the output buffer.
*
*\param[in]  deviceId          Peer identifier
*\param[in]  mode              Step mode (0-3)
*\param[in]  stepDataLength    Length of step data from HCI source
*\param[in]  pStepDataAux      Pointer to current position in HCI source data
*\param[in]  numAntennaPaths   Number of antenna paths in the configuration
*
*\retval     Updated pointer past the consumed source data
********************************************************************************** */
static uint8_t* writeStepData
(
    deviceId_t deviceId,
    uint8_t    mode,
    uint8_t    stepDataLength,
    uint8_t*   pStepDataAux,
    uint8_t    numAntennaPaths
)
{
    uint8_t antPermIndex = 0U;
    const uint8_t *antIndex_p = NULL;
    uint8_t quality = 0U;
    uint16_t qualityIdx = 0U;

    switch (mode)
    {
        case (uint8_t)gCsStepMode0_c:
        {
            /* Mode-0: copy raw step data (variable length from source) */
            FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                        pStepDataAux, gMode0DataSize_c);
            mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += gMode0DataSize_c;
            pStepDataAux = &pStepDataAux[gMode0DataSize_c];
            if (stepDataLength > 3U)
            {
                FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                            pStepDataAux,
                            sizeof(uint16_t));
                mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += (uint16_t)sizeof(uint16_t);
                pStepDataAux = &pStepDataAux[sizeof(uint16_t)];
            }
        }
        break;

        case (uint8_t)gCsStepMode1_c:
        {
            /* Mode-1: copy fixed-size round-trip measurement data */
            FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                        pStepDataAux,
                        gMode1DataSize_c);
            mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += gMode1DataSize_c;
            pStepDataAux = &pStepDataAux[gMode1DataSize_c];
        }
        break;

        case (uint8_t)gCsStepMode2_c:
        {
            /*
             * Mode-2: tone phase/quality data.
             * Output format: quality[1] + PCT[3] * numAntennaPaths
             * The antenna permutation index maps HCI antenna order
             * to the quality bitmask positions.
             */
            antPermIndex = *pStepDataAux++;
            assert(antPermIndex < 25U);
            antIndex_p = &maAntPermNAp[antPermIndex][0];
            quality = 0U;
            qualityIdx = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
            mpRangingData[deviceId].pCfg->totalSentRcvDataIndex++;
            for (uint8_t idx = 0U; idx < numAntennaPaths; idx++)
            {
                FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                            pStepDataAux,
                            gTone_PCTSize_c);
                mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += gTone_PCTSize_c;
                pStepDataAux += gTone_PCTSize_c;
                quality |= (*pStepDataAux) << (2U * antIndex_p[idx]);
                pStepDataAux++;
            }
            pStepDataAux = &pStepDataAux[gTone_PCTSize_c + 1U];
            mpRangingData[deviceId].pRangingData[qualityIdx] = quality;
        }
        break;

        case (uint8_t)gCsStepMode3_c:
        {
            /* Mode-3: combination of Mode-1 data followed by Mode-2 tone data */
            FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                        pStepDataAux, gMode1DataSize_c);
            mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += gMode1DataSize_c;
            pStepDataAux = &pStepDataAux[gMode1DataSize_c];
            antPermIndex = *pStepDataAux++;
            assert(antPermIndex < 25U);
            antIndex_p = &maAntPermNAp[antPermIndex][0];
            quality = 0U;
            qualityIdx = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
            mpRangingData[deviceId].pCfg->totalSentRcvDataIndex++;
            for (uint8_t idx = 0U; idx < numAntennaPaths; idx++)
            {
                FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                            pStepDataAux, gTone_PCTSize_c);
                mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += gTone_PCTSize_c;
                pStepDataAux += gTone_PCTSize_c;
                quality |= (*pStepDataAux) << (2U * antIndex_p[idx]);
                pStepDataAux++;
            }
            pStepDataAux = &pStepDataAux[gTone_PCTSize_c + 1U];
            mpRangingData[deviceId].pRangingData[qualityIdx] = quality;
        }
        break;

        default:
        {
            assert(FALSE);
        }
        break;
    }

    return pStepDataAux;
}

/*! *********************************************************************************
*\brief      Build the first segment of a procedure: procedure header + subevent header + step data.
*
*\param[in]  deviceId    Peer identifier
*\param[in]  pEventData  Pointer to Channel Sounding measurement data
*
*\retval     none
********************************************************************************** */
static void buildProcHeader
(
    deviceId_t deviceId,
    uint8_t*   pEventData
)
{
    uint8_t numStepsParsed = 0U;
    uint16_t maxDataSize = gMaxPayloadSize_c;
    uint8_t segmIdx = mpSegmIdx[deviceId]->crtIdx;
    uint8_t subEvtIdx = 0U;
    union
    {
        uint16_t u16;
        uint32_t u32;
    }dataLen = {0U};

    gCsProcHeaderData_t procHeader;

    /* First subevent of the procedure - fill our procedure header and subeven header */
    mpRangingData[deviceId].pCfg->totalSentRcvDataIndex = 0U;
    mpRangingData[deviceId].pCfg->dataParsedLen = 0U;
    mpSegmIdx[deviceId]->aSegmIdx[segmIdx] = 0U;
    mpSegmIdx[deviceId]->numStepsIdx = 0U;
    mpSegmIdx[deviceId]->numStepsHdrIdx = 0U;
    subEvtIdx = mpRangingData[deviceId].pCfg->subeventIndex;

    /* The least significant byte of the CS procedure counter */
    procHeader.seqNo = (uint8_t)(mpRangingData[deviceId].pCfg->procedureCounter & 0xFFU);
    procHeader.configId = mpRangingData[deviceId].pCfg->configId;
    procHeader.moduleId = 0xFEU; /* Module ID not supplied (data is from vehicle) */

    /* Copy procedure header information and update parsed length */
    FLib_MemCpy(mpRangingData[deviceId].pRangingData, &procHeader, sizeof(gCsProcHeaderData_t));
    dataLen.u32 = sizeof(gCsProcHeaderData_t);
    mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += dataLen.u16;
    maxDataSize -= dataLen.u16;

    /* Pack subevent header information */
    packSubEvtHeader(deviceId, &maxDataSize);

    /* Reset mode array state for fresh subevent */
    mpSegmIdx[deviceId]->modeArraySteps = 0U;

    /* Pack subevent data */
    (void)parseSubeventData(deviceId, subEvtIdx, maxDataSize, pEventData, &numStepsParsed);

    /* Update total number of steps in the header */
    mpRangingData[deviceId].pRangingData[mpSegmIdx[deviceId]->numStepsHdrIdx] += numStepsParsed;
    /* Update number of steps for the current segment */
    mpRangingData[deviceId].pRangingData[mpSegmIdx[deviceId]->numStepsIdx] += numStepsParsed;

    /* Move on to the next segment */
    if (mpRangingData[deviceId].pCfg->totalSentRcvDataIndex >= gMaxPayloadSize_c)
    {
        mpSegmIdx[deviceId]->crtIdx++;
        mpSegmIdx[deviceId]->aSegmIdx[mpSegmIdx[deviceId]->crtIdx] = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
    }
}

/*! *********************************************************************************
*\brief      Continue packing data from a previous subevent into the current or next segment.
*
*\param[in]  deviceId    Peer identifier
*\param[in]  pEventData  Pointer to Channel Sounding measurement data
*
*\retval     none
********************************************************************************** */
static void buildSubEvtCont
(
    deviceId_t deviceId,
    uint8_t*   pEventData
)
{
    uint8_t numStepsParsed = 0U;
    uint16_t maxDataSize = gMaxPayloadSize_c;
    uint16_t parsedLen = 0U;
    uint8_t segmIdx = mpSegmIdx[deviceId]->crtIdx;
    uint8_t subEvtIdx = mpRangingData[deviceId].pCfg->subeventIndex;
    union
    {
        uint16_t u16;
        uint32_t u32;
    }dataLen = {0U};

    gCsProcContHeaderData_t   procContHeader;
    gCsSubEvtContHeaderData_t subEvtContHeader;

    /* Check if there is room left to add at least the smallest complete step in the current payload size */
    uint16_t currentLen = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex - mpSegmIdx[deviceId]->aSegmIdx[segmIdx];
    if ((currentLen + gMode0DataSize_c) < gMaxPayloadSize_c)
    {
        numStepsParsed = 0U;
        parsedLen = parseSubeventData(deviceId, subEvtIdx, (gMaxPayloadSize_c - currentLen), pEventData,
                                      &numStepsParsed);

        /* Update number of steps in the header */
        mpRangingData[deviceId].pRangingData[mpSegmIdx[deviceId]->numStepsHdrIdx] += numStepsParsed;
        /* Update number of steps for the current segment */
        mpRangingData[deviceId].pRangingData[mpSegmIdx[deviceId]->numStepsIdx] += numStepsParsed;

        /* Update subevent done status */
        mpRangingData[deviceId].pRangingData[mpSegmIdx[deviceId]->procDoneStatusIdx] =
          ((mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.subeventDoneStatus & 0x0FU) << 4U) |
            (mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.procedureDoneStatus & 0x0FU);
    }

    /* Move on to the next segment if there is more data to parse */
    if ((mpRangingData[deviceId].pCfg->dataIndex != mpRangingData[deviceId].pCfg->dataParsedLen) &&
        ((mpRangingData[deviceId].pCfg->dataParsedLen != 0U)))
    {
        mpSegmIdx[deviceId]->crtIdx++;
        segmIdx++;

        /* Mark the index of the beginning of the segment */
        mpSegmIdx[deviceId]->aSegmIdx[segmIdx] = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;

        /* Copy procedure header information and update parsed length */
        subEvtIdx = mpRangingData[deviceId].pCfg->subeventIndex;
        procContHeader.seqNo = (uint8_t)(mpRangingData[deviceId].pCfg->procedureCounter & 0xFFU);
        FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                                &procContHeader,
                                sizeof(gCsProcContHeaderData_t));
        dataLen.u32 = sizeof(gCsProcContHeaderData_t);
        mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += dataLen.u16;
        maxDataSize -= dataLen.u16;

        /* Copy subevent header information */
        subEvtContHeader.startACLConnEvt = mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.startACLConnEvent;
        subEvtContHeader.numStepsReported = 0U;
        FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                                &subEvtContHeader,
                                sizeof(gCsSubEvtContHeaderData_t));
        dataLen.u32 = sizeof(gCsSubEvtContHeaderData_t);
        mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += dataLen.u16;
        maxDataSize -= dataLen.u16;
        /* Save index for the num steps reported to be updated later */
        mpSegmIdx[deviceId]->numStepsIdx = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex - 1U;

        /* Reset mode array state for new segment */
        mpSegmIdx[deviceId]->modeArraySteps = 0U;

        numStepsParsed = 0U;
        (void)parseSubeventData(deviceId, subEvtIdx, maxDataSize, pEventData + parsedLen, &numStepsParsed);

        /* Update number of steps in subevent header */
        mpRangingData[deviceId].pRangingData[mpSegmIdx[deviceId]->numStepsHdrIdx] += numStepsParsed;
        /* Update number of steps for the current segment */
        mpRangingData[deviceId].pRangingData[mpSegmIdx[deviceId]->numStepsIdx] = numStepsParsed;

        /* Update subevent done status */
        mpRangingData[deviceId].pRangingData[mpSegmIdx[deviceId]->procDoneStatusIdx] =
          ((mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.subeventDoneStatus & 0x0FU) << 4U) |
            (mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.procedureDoneStatus & 0x0FU);

        /* Move on to the next segment if we have filled the payload */
        if ((mpRangingData[deviceId].pCfg->totalSentRcvDataIndex -
                 mpSegmIdx[deviceId]->aSegmIdx[segmIdx]) >= gMaxPayloadSize_c)
        {
            mpSegmIdx[deviceId]->crtIdx++;
            mpSegmIdx[deviceId]->aSegmIdx[mpSegmIdx[deviceId]->crtIdx] = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
        }
    }
}

/*! *********************************************************************************
*\brief      Start a new subevent within the current procedure.
*
*\param[in]  deviceId    Peer identifier
*\param[in]  pEventData  Pointer to Channel Sounding measurement data
*
*\retval     none
********************************************************************************** */
static void buildSubEvtNew
(
    deviceId_t deviceId,
    uint8_t*   pEventData
)
{
    uint8_t numStepsParsed = 0U;
    uint16_t maxDataSize = gMaxPayloadSize_c;
    uint8_t segmIdx = mpSegmIdx[deviceId]->crtIdx;
    uint8_t subEvtIdx = mpRangingData[deviceId].pCfg->subeventIndex;
    bool_t bBuiltHeader = FALSE;
    uint16_t parsedLen = 0U;
    union
    {
        uint16_t u16;
        uint32_t u32;
    }dataLen = {0U};

    gCsProcContHeaderData_t procContHeader;
    gCsSubEvtContHeaderData_t subEvtContHeader;

    /* Check if there is room left to add more data in the current payload size */
    uint16_t currentLen = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex - mpSegmIdx[deviceId]->aSegmIdx[segmIdx];
    uint16_t remaininLen = (gMaxPayloadSize_c - currentLen);
    if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
    {
        /* Subevent header information for intitiator */
        if ((remaininLen >= sizeof(gCsSubEvtHeaderInitData_t)))
        {
            /* pack subevent continue header */
            packSubEvtHeader(deviceId, &remaininLen);
            bBuiltHeader = TRUE;
        }
        else
        {
            /* Move on to the next segment */
            mpSegmIdx[deviceId]->crtIdx++;
            segmIdx++;
        }
    }
    else
    {
        /* Subevent header information for reflector */
        if ((remaininLen >= sizeof(gCsSubEvtHeaderReflData_t)))
        {
            /* Pack subevent continue header */
            packSubEvtHeader(deviceId, &remaininLen);
            bBuiltHeader = TRUE;
        }
        else
        {
            /* move on to the next segment */
            mpSegmIdx[deviceId]->crtIdx++;
            segmIdx++;
        }
    }

    /* Check if there is room left to include additional data in the current segment */
    if ((remaininLen > 0U) && (bBuiltHeader == TRUE))
    {
        /* Reset mode array state for new subevent */
        mpSegmIdx[deviceId]->modeArraySteps = 0U;
        numStepsParsed = 0U;
        parsedLen = parseSubeventData(deviceId, subEvtIdx, remaininLen, pEventData, &numStepsParsed);

        /* Update number of steps in the header */
        mpRangingData[deviceId].pRangingData[mpSegmIdx[deviceId]->numStepsHdrIdx] += numStepsParsed;
        /* Update number of steps for the current segment */
        mpRangingData[deviceId].pRangingData[mpSegmIdx[deviceId]->numStepsIdx] += numStepsParsed;

        /* Move on to the next segment */
        if ((mpRangingData[deviceId].pCfg->dataParsedLen != 0U) &&
            (mpRangingData[deviceId].pCfg->dataIndex != mpRangingData[deviceId].pCfg->dataParsedLen))
        {
            mpSegmIdx[deviceId]->crtIdx++;
            segmIdx++;
        }
    }
    else if ((remaininLen == 0U) && (bBuiltHeader == TRUE))
    {
        /* Subevent header consumed all remaining space in the current segment.
         * Advance to the next segment so that the "Mark the index" block below
         * writes to the correct (new) segment slot instead of overwriting the
         * current segment's start index. */
        mpSegmIdx[deviceId]->crtIdx++;
        segmIdx++;
    }
    else
    {
        /* For MISRA compliance */
    }

    /* Mark the index of the beginning of the segment */
    if ((mpRangingData[deviceId].pCfg->dataParsedLen != 0U) &&
        (mpRangingData[deviceId].pCfg->dataIndex != mpRangingData[deviceId].pCfg->dataParsedLen))
    {
        mpSegmIdx[deviceId]->aSegmIdx[segmIdx] = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
        /* Fill the procedure continue header */
        subEvtIdx = mpRangingData[deviceId].pCfg->subeventIndex;
        procContHeader.seqNo = (uint8_t)(mpRangingData[deviceId].pCfg->procedureCounter & 0xFFU);
        FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                    &procContHeader,
                    sizeof(gCsProcContHeaderData_t));
        dataLen.u32 = sizeof(gCsProcContHeaderData_t);
        mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += dataLen.u16;
        maxDataSize -= dataLen.u16;

        if (bBuiltHeader == FALSE)
        {
            /* Pack subevent continue header */
            packSubEvtHeader(deviceId, &maxDataSize);
        }
        else
        {
            /* Pack subevent continue header */
            subEvtContHeader.startACLConnEvt = mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.startACLConnEvent;
            subEvtContHeader.numStepsReported =0U;
            FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                        &subEvtContHeader,
                        sizeof(gCsSubEvtContHeaderData_t));
            dataLen.u32 = sizeof(gCsSubEvtContHeaderData_t);
            mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += dataLen.u16;
            maxDataSize -= dataLen.u16;
            /* Save index for the num steps reported to be updated later */
            mpSegmIdx[deviceId]->numStepsIdx = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex - 1U;
        }

        /* Reset mode array state for new segment */
        mpSegmIdx[deviceId]->modeArraySteps = 0U;

        (void)parseSubeventData(deviceId, subEvtIdx, maxDataSize, pEventData + parsedLen, &numStepsParsed);

        /* Update number of steps for this segment */
        mpRangingData[deviceId].pRangingData[mpSegmIdx[deviceId]->numStepsIdx] = numStepsParsed;
        /* Update number of steps in subevent header */
        mpRangingData[deviceId].pRangingData[mpSegmIdx[deviceId]->numStepsHdrIdx] += numStepsParsed;
    }

    /* Move on to the next segment */
    if (((mpRangingData[deviceId].pCfg->totalSentRcvDataIndex -
         mpSegmIdx[deviceId]->aSegmIdx[segmIdx]) >= gMaxPayloadSize_c) &&
        (mpRangingData[deviceId].pCfg->dataParsedLen > 0U))
    {
        mpSegmIdx[deviceId]->crtIdx++;
        mpSegmIdx[deviceId]->aSegmIdx[mpSegmIdx[deviceId]->crtIdx] = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
    }
}

/*! *********************************************************************************
*\brief      Helper function to pack a CSSubEventData message fragment.
*            Per BTCS spec, the output format is:
*              [StepModeArray (ceil(totalSteps/2) bytes)] [StepData for non-aborted steps]
*            The step mode array packs 2 steps per byte:
*              Lower nibble: bits[0:1]=mode, bit[2]=RFU, bit[3]=status(1=aborted)
*              Upper nibble: bits[4:5]=mode, bit[6]=RFU, bit[7]=status
*            If odd number of steps, upper nibble of last byte = 0xF.
*            Supports extending an existing mode array when called for continuation
*            data of the same subevent (modeArraySteps > 0).
*
*\param[in]   deviceId          Peer identifier
*\param[in]   subEvtIdx         Current subevent index
*\param[in]   maxDataLen        Maximum data size to be packed
*\param[in]   pSrcBuff          Pointer to data source (HCI format: mode+channel+len+data)
*\param[out]  pOutNumSteps      Number of packed steps
*
*\retval     Number of bytes consumed from source
********************************************************************************** */
static uint16_t parseSubeventData
(
    deviceId_t deviceId,
    uint8_t    subEvtIdx,
    uint16_t   maxDataLen,
    uint8_t*   pSrcBuff,
    uint8_t*   pOutNumSteps
)
{
    bool_t bEarlyReturn = FALSE;
    uint16_t retBytes = 0U;
    uint8_t* pStepDataAux = pSrcBuff;
    uint8_t numAntennaPaths = mpRangingData[deviceId].pCfg->numAntennaPaths;
    union {
        uint8_t stepLen8;
        uint16_t stepLen16;
        uint32_t stepLen32;
    } stepLen = {0U};
    /* Save current dataParsedLen to compute how many source bytes were consumed */
    uint16_t parsedLen = mpRangingData[deviceId].pCfg->dataParsedLen;

    *pOutNumSteps = 0U;

    /*
     * Retrieve the current mode array state from the segment context.
     * If this is a continuation call for the same subevent, existingSteps > 0
     * and we will extend the existing mode array rather than creating a new one.
     */
    uint8_t existingSteps = mpSegmIdx[deviceId]->modeArraySteps;
    uint16_t modeArrayStartIdx = mpSegmIdx[deviceId]->modeArrayStartIdx;
    uint8_t oldModeArrayLen = (existingSteps + 1U) / 2U;

    if (existingSteps == 0U)
    {
        /* First call for this subevent/segment: record where the mode array begins */
        modeArrayStartIdx = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
        mpSegmIdx[deviceId]->modeArrayStartIdx = modeArrayStartIdx;
        oldModeArrayLen = 0U;
    }

    /*
     * ===== PASS 1: PRE-SCAN =====
     * Walk through the source buffer (HCI format: mode[1] + channel[1] + len[1] + data[len])
     * without writing anything, to determine how many new steps will fit in the
     * remaining L2CAP payload space. This accounts for:
     *   - Mode array growth (ceil(totalSteps/2) bytes)
     *   - Existing step data already written after the mode array
     *   - Output size of each new step (only non-aborted steps produce data)
     */
    uint8_t numNewStepsFit = 0U;
    uint16_t totalNewStepDataSize = 0U;
    uint8_t* pScanPtr = pSrcBuff;
    uint16_t srcBytesForFitSteps = 0U;
    /* Size of step data already written after the current mode array */
    uint16_t existingStepDataSize = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex -
                                    (modeArrayStartIdx + oldModeArrayLen);

    while (TRUE)
    {
        /* Stop if all source data has been accounted for */
        if (((uint32_t)mpRangingData[deviceId].pCfg->dataParsedLen + (uint32_t)srcBytesForFitSteps) >=
            mpRangingData[deviceId].pCfg->dataIndex)
        {
            break;
        }

        /* Peek at step header: mode and data length (without consuming) */
        uint8_t scanMode = pScanPtr[0];
        uint8_t scanStepDataLen = pScanPtr[2];

        /* Compute the output size for this step's data (0 if aborted, i.e. len==0) */
        uint16_t stepOutputSize = computeStepOutputSize(scanMode, scanStepDataLen, numAntennaPaths);

        /*
         * Check if adding this step would exceed the available space.
         * The total space consumed from modeArrayStartIdx is:
         *   new_mode_array_len + existing_step_data + new_step_data_so_far + this_step_data
         */
        uint8_t totalStepsIfIncluded = existingSteps + numNewStepsFit + 1U;
        uint8_t newModeArrayLenCalc = (totalStepsIfIncluded + 1U) / 2U;
        uint16_t totalFromModeArrayStart = (uint16_t)newModeArrayLenCalc + existingStepDataSize +
                                           totalNewStepDataSize + stepOutputSize;

        /* Determine available space from mode array start to end of L2CAP payload */
        uint16_t availableSpace;
        if (existingSteps == 0U)
        {
            /* Fresh mode array: use the remaining maxDataLen passed by caller */
            availableSpace = maxDataLen;
        }
        else
        {
            /* Extending: compute from segment start to max payload */
            uint16_t segStart = mpSegmIdx[deviceId]->aSegmIdx[mpSegmIdx[deviceId]->crtIdx];
            availableSpace = gMaxPayloadSize_c - (modeArrayStartIdx - segStart);
        }

        if (totalFromModeArrayStart > availableSpace)
        {
            /* This step would overflow the payload; stop here */
            break;
        }

        /* Step fits - account for it */
        numNewStepsFit++;
        totalNewStepDataSize += stepOutputSize;
        srcBytesForFitSteps += 3U + (uint16_t)scanStepDataLen;
        pScanPtr += 3U + (uint16_t)scanStepDataLen;
    }

    if (numNewStepsFit == 0U)
    {
        /* If this is the last subevent and it's empty */
        if ((mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.procedureDoneStatus == (uint8_t)gCsCompleteResults_c)
            && (mpRangingData[deviceId].pCfg->dataIndex == mpRangingData[deviceId].pCfg->dataParsedLen))
        {
            mpRangingData[deviceId].pCfg->dataParsedLen = 0U;
            mpSegmIdx[deviceId]->crtIdx++;
            mpSegmIdx[deviceId]->aSegmIdx[mpSegmIdx[deviceId]->crtIdx] = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
            mpSegmIdx[deviceId]->crtIdx = 0U;
        }
        /* No steps fit in the remaining space */
        bEarlyReturn = TRUE;
    }

    if (bEarlyReturn == FALSE)
    {
        /*
         * ===== MODE ARRAY MANAGEMENT =====
         * The mode array packs 2 steps per byte: ceil(totalSteps/2) bytes total.
         * When extending an existing mode array, we may need to grow it by
         * inserting bytes between the mode array and the step data that follows.
         */
        uint8_t totalSteps = existingSteps + numNewStepsFit;
        uint8_t newModeArrayLen = (totalSteps + 1U) / 2U;
        uint8_t modeArrayGrowth = newModeArrayLen - oldModeArrayLen;

        if ((modeArrayGrowth > 0U) && (existingStepDataSize > 0U))
        {
            /*
             * Mode array must grow AND there is existing step data after it.
             * Shift existing step data forward by modeArrayGrowth bytes to make room.
             * Use backward copy since source and destination regions overlap (dst > src).
             */
            uint16_t stepDataStart = modeArrayStartIdx + oldModeArrayLen;
            for (uint16_t i = existingStepDataSize; i > 0U; i--)
            {
                mpRangingData[deviceId].pRangingData[stepDataStart + modeArrayGrowth + i - 1U] =
                    mpRangingData[deviceId].pRangingData[stepDataStart + i - 1U];
            }
            mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += modeArrayGrowth;
        }
        else if (existingSteps == 0U)
        {
            /* Brand new mode array: zero-initialize the reserved space */
            FLib_MemSet(mpRangingData[deviceId].pRangingData + modeArrayStartIdx, 0U, newModeArrayLen);
            mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += newModeArrayLen;
        }
        else
        {
            /* Mode array grows but no existing step data to shift (rare edge case) */
            mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += modeArrayGrowth;
        }

        /*
         * If extending an existing mode array, clean up the previous state:
         * - Remove the 0xF0 odd padding from the last byte (we may write into that nibble now)
         * - Zero-initialize any newly added mode array bytes
         */
        if (existingSteps > 0U)
        {
            if ((existingSteps & 1U) != 0U)
            {
                /* Previous total was odd: upper nibble of last byte had 0xF padding; clear it */
                uint8_t lastOldByteIdx = (existingSteps - 1U) / 2U;
                mpRangingData[deviceId].pRangingData[modeArrayStartIdx + lastOldByteIdx] &= 0x0FU;
            }
            if (modeArrayGrowth > 0U)
            {
                /* Zero the newly added mode array bytes so OR operations below work correctly */
                FLib_MemSet(mpRangingData[deviceId].pRangingData + modeArrayStartIdx + oldModeArrayLen,
                            0U, modeArrayGrowth);
            }
        }

        /*
         * ===== PASS 2: WRITE =====
         * Now consume the source buffer and produce the output:
         *   1. Pack each step's mode+status into the mode array (2 steps per byte)
         *   2. Write step measurement data ONLY for non-aborted steps
         *
         * Source format per step: mode[1] + channel[1] + dataLen[1] + data[dataLen]
         * If dataLen==0, the step is aborted (status bit=1, no output data).
         */
        pStepDataAux = pSrcBuff;
        for (uint8_t newStepIdx = 0U; newStepIdx < numNewStepsFit; newStepIdx++)
        {
            /* Extract step header from HCI source data */
            uint8_t mode = *pStepDataAux++;
            pStepDataAux++; /* skip channel index (not used in BTCS output) */
            uint8_t stepDataLength = *pStepDataAux++;
            assert(mode <= (uint8_t)gCsStepMode3_c);

            /*
             * Pack mode + status into the mode array nibble.
             * globalStepIdx determines which byte and which nibble (lower or upper).
             * Even index -> lower nibble (bits 0-3), Odd index -> upper nibble (bits 4-7).
             */
            uint8_t globalStepIdx = existingSteps + newStepIdx;
            uint8_t byteIdx = globalStepIdx / 2U;
            bool_t aborted = (stepDataLength == 0U) ? TRUE : FALSE;
            uint8_t nibble = (mode & gStepModeMask_c);
            if (aborted == TRUE) { nibble |= gStepStatusBit_c; }

            if ((globalStepIdx & 1U) == 0U)
            {
                /* Even step index: lower nibble (bits 0-3) */
                mpRangingData[deviceId].pRangingData[modeArrayStartIdx + byteIdx] |= nibble;
            }
            else
            {
                /* Odd step index: upper nibble (bits 4-7) */
                mpRangingData[deviceId].pRangingData[modeArrayStartIdx + byteIdx] |= (uint8_t)(nibble << 4U);
            }

            /*
             * Write step measurement data to the output buffer.
             * Only non-aborted steps produce output; aborted steps just advance the source pointer.
             */
            if (aborted == FALSE)
            {
                pStepDataAux = writeStepData(deviceId, mode, stepDataLength, pStepDataAux, numAntennaPaths);
            }
            else
            {
                /* Aborted step: skip source data (if any), no output data is written */
                pStepDataAux += stepDataLength;
            }

            /* Update step counters and track how much source data has been consumed */
            (*pOutNumSteps)++;
            stepLen.stepLen8 = stepDataLength;
            mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].currentDataSize += stepLen.stepLen16 + 3U;
            mpRangingData[deviceId].pCfg->dataParsedLen += stepLen.stepLen16 + 3U;
        }

        /*
         * Apply odd padding: if the total number of steps is odd, the upper nibble
         * of the last mode array byte is unused - set it to 0xF per BTCS spec.
         */
        if ((totalSteps & 1U) != 0U)
        {
            uint8_t lastByteIdx = (totalSteps - 1U) / 2U;
            mpRangingData[deviceId].pRangingData[modeArrayStartIdx + lastByteIdx] |= gStepModeOddPad_c;
        }

        /* Persist the updated mode array step count for potential future extension calls */
        mpSegmIdx[deviceId]->modeArraySteps = totalSteps;

        /*
         * When the procedure is complete (all source data consumed and procedure done),
         * finalize the segment table and reset dataParsedLen for the L2CAP send phase.
         */
        if ((mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.procedureDoneStatus == (uint8_t)gCsCompleteResults_c)
            && (mpRangingData[deviceId].pCfg->dataIndex == mpRangingData[deviceId].pCfg->dataParsedLen))
        {
            mpRangingData[deviceId].pCfg->dataParsedLen = 0U;
            mpSegmIdx[deviceId]->crtIdx++;
            mpSegmIdx[deviceId]->aSegmIdx[mpSegmIdx[deviceId]->crtIdx] = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
            mpSegmIdx[deviceId]->crtIdx = 0U;
        }
        retBytes = mpRangingData[deviceId].pCfg->dataParsedLen - parsedLen;
    }

    /* Return the number of source bytes consumed in this call */
    return retBytes;
}
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */
