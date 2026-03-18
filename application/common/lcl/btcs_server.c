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
    uint8_t subEvtIdx = 0U;
    uint16_t maxDataSize = gMaxPayloadSize_c;
    uint8_t numStepsParsed = 0U;
    union
    {
        uint16_t u16;
        uint32_t u32;
    }dataLen = {0U};

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
        /* Current segment index */
        uint8_t segmIdx = mpSegmIdx[deviceId]->crtIdx;
        /* Number of parsed steps */
        numStepsParsed = 0U;

        switch (fragmType)
        {
            case gCsProcHeader_c:
            {
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
                }
            }
            break;

            case gCsSubEvtContHeader_c:
            {
                uint16_t parsedLen = 0U;

                /* Continuedata from a previous subevent */
                gCsProcContHeaderData_t   procContHeader;
                gCsSubEvtContHeaderData_t subEvtContHeader;
                subEvtIdx = mpRangingData[deviceId].pCfg->subeventIndex;

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
                    }
                }
            }
            break;

            case gCsSubEvtHeader_c:
            {
                /* New subevent in the current procedure */
                gCsProcContHeaderData_t procContHeader;
                gCsSubEvtContHeaderData_t subEvtContHeader;
                bool_t bBuiltHeader = FALSE;
                uint16_t parsedLen = 0U;
                
                subEvtIdx = mpRangingData[deviceId].pCfg->subeventIndex;

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
                }
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
*\brief      Helper function to pack a CSSubEventData message fragment
*
*\param[in]   deviceId          Peer identifier
*\param[in]   subEvtIdx         Current subevent index
*\param[in]   maxDataLen        Maximum data size to be packed
*\param[out]  pSrcBuff          Pointer to data source
*\param[out]  pOutNumSteps      Number of packed steps
*
*\retval     none
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
    uint8_t* pStepDataAux = pSrcBuff;
    uint16_t dataStartIdx = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
    uint8_t mode = 0U;
    uint8_t stepDataLength = 0U;
    uint8_t antPermIndex = 0U;
    const uint8_t *antIndex_p = NULL;
    uint8_t quality = 0U;
    uint16_t qualityIdx = 0U;
    *pOutNumSteps = 0U;
    bool_t bEarlyExit = FALSE;
    uint16_t dataCopiedLen = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex - dataStartIdx;

    union
    {
        uint8_t stepLen8;
        uint16_t stepLen16;
        uint32_t stepLen32;
    } stepLen = {0U};
    uint16_t parsedLen = mpRangingData[deviceId].pCfg->dataParsedLen;

    while (dataCopiedLen < maxDataLen)
    {
        if  (mpRangingData[deviceId].pCfg->dataParsedLen >= mpRangingData[deviceId].pCfg->dataIndex)
        {
            /* we reached the end of the data */
            break;
        }

        stepLen.stepLen32 = 0U;
        /* Step mode value */
        mode = *pStepDataAux++;
        /* Skip step channel */
        pStepDataAux++;
        /* Step data length and step data */
        stepDataLength = *pStepDataAux++;

        assert(mode <= gCsStepMode3_c);

        /* Copy step mode information to notification buffer */
        mpRangingData[deviceId].pRangingData[mpRangingData[deviceId].pCfg->totalSentRcvDataIndex] = mode;
        mpRangingData[deviceId].pCfg->totalSentRcvDataIndex++;

        switch (mode)
        {
            case (uint8_t)gCsStepMode0_c:
            {
                if ((dataCopiedLen + stepDataLength + 1U) > maxDataLen)
                {
                    /* There is no more room for this step - break loop */
                    mpRangingData[deviceId].pCfg->totalSentRcvDataIndex--;
                    bEarlyExit = TRUE;
                }
                else
                {
                    /* Mode 0 data size: Packet Quality + RSSI + Antenna */
                    FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                                pStepDataAux,
                                gMode0DataSize_c);
                    mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += gMode0DataSize_c;
                    pStepDataAux = &pStepDataAux[gMode0DataSize_c];

                    if (stepDataLength > 3U)
                    {
                        /* Local device is initiator - copy Frequency Compensation information */
                        FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                                pStepDataAux,
                                sizeof(uint16_t));
                        stepLen.stepLen32 = sizeof(uint16_t);
                        mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += stepLen.stepLen16;
                        pStepDataAux = &pStepDataAux[sizeof(uint16_t)];
                    }
                }
            }
            break;

            case (uint8_t)gCsStepMode1_c:
            {
                if ((dataCopiedLen + gMode1DataSize_c + 1U) > maxDataLen)
                {
                    /* There is no more room for this step - break loop */
                    mpRangingData[deviceId].pCfg->totalSentRcvDataIndex--;
                    bEarlyExit = TRUE;
                }
                else
                {
                    /* Mode 1 data : Packet Quality, NADM, RSSI, Time Diff, Packet Antenna */
                    FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                                pStepDataAux,
                                gMode1DataSize_c);
                    mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += gMode1DataSize_c;
                    pStepDataAux = &pStepDataAux[gMode1DataSize_c];
                }
            }
            break;

            case (uint8_t)gCsStepMode2_c:
            {
                /* Mode 2 data: 1 byte quality + 3 bytes per antenna path for Tone_PCT */
                if ((dataCopiedLen + ((uint16_t)mpRangingData[deviceId].pCfg->numAntennaPaths * gTone_PCTSize_c) + 2U) > maxDataLen)
                {
                    /* There is no more room for this step - break loop */
                    mpRangingData[deviceId].pCfg->totalSentRcvDataIndex--;
                    bEarlyExit = TRUE;
                }
                else
                {
                    /* Get antenna permutation index */
                    antPermIndex = *pStepDataAux++;
                    assert(antPermIndex < 25);
                    antIndex_p = &maAntPermNAp[antPermIndex][0];

                    /* Compressed tone quality (2 bits per antenna path) */
                    quality = 0U;
                    qualityIdx = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
                    /* Leave space to fill later the compressed tone quality value */
                    mpRangingData[deviceId].pCfg->totalSentRcvDataIndex++;

                    /* Mode 2 data: Tone Quality Indicator[k], Tone PCT[k] */
                    for (uint8_t idx = 0U; idx < mpRangingData[deviceId].pCfg->numAntennaPaths; idx++)
                    {
                        FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                                    pStepDataAux,
                                    gTone_PCTSize_c);
                        mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += gTone_PCTSize_c;
                        pStepDataAux += gTone_PCTSize_c;

                        /* Build quality value */
                        quality |= (*pStepDataAux) << (2U * antIndex_p[idx]);
                        pStepDataAux++;
                }

                    /* Skip extension tone */
                    pStepDataAux = &pStepDataAux[gTone_PCTSize_c+1U];

                    /* Copy quality information */
                    mpRangingData[deviceId].pRangingData[qualityIdx] = quality;
                }
            }
            break;

            case (uint8_t)gCsStepMode3_c:
            {
                if ((dataCopiedLen + ((uint16_t)mpRangingData[deviceId].pCfg->numAntennaPaths*gTone_PCTSize_c) +
                     2U + gMode1DataSize_c)> maxDataLen)
                {
                    /* There is no more room for this step - break loop */
                    mpRangingData[deviceId].pCfg->totalSentRcvDataIndex--;
                    bEarlyExit = TRUE;
                }
                else
                {
                    /* Packet Quality, NADM, RSSI, Time Diff, Packet Antenna */
                    FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                                pStepDataAux,
                                gMode1DataSize_c);
                    mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += gMode1DataSize_c;
                    pStepDataAux = &pStepDataAux[gMode1DataSize_c];

                    antPermIndex = *pStepDataAux++;
                    assert(antPermIndex < 25);
                    antIndex_p = &maAntPermNAp[antPermIndex][0];

                    /* Compressed tone quality (2 bits per antenna path) */
                    quality = 0U;
                    qualityIdx = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
                    /* Leave space to fill later the compressed tone quality value */
                    mpRangingData[deviceId].pCfg->totalSentRcvDataIndex++;

                    /* Tone Quality Indicator[k], Tone PCT[k] */
                    for (uint8_t idx = 0U; idx < mpRangingData[deviceId].pCfg->numAntennaPaths; idx++)
                    {
                        FLib_MemCpy(mpRangingData[deviceId].pRangingData + mpRangingData[deviceId].pCfg->totalSentRcvDataIndex,
                                    pStepDataAux,
                                    gTone_PCTSize_c);
                        mpRangingData[deviceId].pCfg->totalSentRcvDataIndex += gTone_PCTSize_c;
                        pStepDataAux += gTone_PCTSize_c;

                        /* Build quality value */
                        quality |= (*pStepDataAux) << (2U * antIndex_p[idx]);
                        pStepDataAux++;
                    }

                    /* Skip extension tone */
                    pStepDataAux = &pStepDataAux[gTone_PCTSize_c+1U];

                    /* Copy quality information */
                    mpRangingData[deviceId].pRangingData[qualityIdx] = quality;
                }
            }
            break;

            default:
            {
                /* Should not get here */
                assert(FALSE);
            }
            break;
        }

        if (bEarlyExit == TRUE)
        {
            /* There wasn't enaough space left for this subevent - break loop */
            break;
        }

        /* Increment number of parsed steps */
        (*pOutNumSteps)++;

        stepLen.stepLen8 = stepDataLength;
        /* Account for the data we parsed so far: step mode, channel, length and data */
        mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].currentDataSize += stepLen.stepLen16 + 3U;
        mpRangingData[deviceId].pCfg->dataParsedLen += stepLen.stepLen16 + 3U;

        /* Updated how much data we copied so far */
        dataCopiedLen = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex - dataStartIdx;
    }

    /* Reset dataParsedLen to be used for sending the L2CAP message fragments */
    if ((mpRangingData[deviceId].pCfg->aSubEventData[subEvtIdx].subevtHeader.procedureDoneStatus == (uint8_t)gCsCompleteResults_c)
        && (mpRangingData[deviceId].pCfg->dataIndex == mpRangingData[deviceId].pCfg->dataParsedLen))
    {
        mpRangingData[deviceId].pCfg->dataParsedLen = 0U;
        /* Mark end of data */
        mpSegmIdx[deviceId]->crtIdx++;
        mpSegmIdx[deviceId]->aSegmIdx[mpSegmIdx[deviceId]->crtIdx] = mpRangingData[deviceId].pCfg->totalSentRcvDataIndex;
        /* Reset segment index */
        mpSegmIdx[deviceId]->crtIdx = 0U;
    }
    
    return (mpRangingData[deviceId].pCfg->dataParsedLen - parsedLen);
}
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */