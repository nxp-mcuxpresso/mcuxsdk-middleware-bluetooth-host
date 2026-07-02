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
 *************************************************************************************/
#include "EmbeddedTypes.h"
#include "app_localization_utils.h"
#if defined(gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1U)
#include "ranging_interface.h"
#include "ranging_client_interface.h"
#endif /* defined(gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1U) */
#include "fsl_component_mem_manager.h"
#include "fsl_component_panic.h"
#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
#include "btcs_client_interface.h"
#endif /* defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U) */
/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\def           ANT_PERM_INDEX_LIST_LEN
*
*\brief         The maximum size of the antenna permutation index array. This list is 
*               used to keep local CS data antenna permutation indexes, which later 
*               will be used by the BTCS remote data unpacking.
*               The list is circular, handled with a producer-consumer algorithm.
*               The list must have room for at least one set of local antenna
*               permutation indexes (around 80 bytes).
*               In case multiple procedures are possible at once, the list must be
*               adjusted, but not with a factor of the number of procedures.
*               Procedures are composed from 2 parts:
*               - local CS data handling (which accumulates indexes)
*               - remote CS data handling (which consumes indexes)
*
********************************************************************************** */
#ifndef ANT_PERM_INDEX_LIST_LEN
#define ANT_PERM_INDEX_LIST_LEN 200u
#endif

/*! *********************************************************************************
*\def           CheckSkipBytesDoNothing(pos, remainingLen, fetchDataLen, endVar)
*
*\brief         Check if fetchDataLen fits inside remainingLen. This macro is designed 
*               to be used in a loop.
*               If this is true advance pos pointer with fetchDataLen and decrease 
*               remainingLen with the same value.
*               In case the result is false set endVar to TRUE and call break (to exit 
*               the loop).
*
*\param         pos             Pointer to start of data.
*\param         remainingLen    Remaining length to parse.
*\param         fetchDataLen    Data length that needs to be parsed.
*\param         endVar          Variable to set to FALSE in case of incomplete data.
*
*\retval        None
********************************************************************************** */
#define CheckSkipBytesDoNothing(pos, remainingLen, fetchDataLen, endVar)\
{\
    if ((remainingLen) < (fetchDataLen))\
    {\
        (endVar) = TRUE;\
        break;\
    }\
    else\
    {\
        (remainingLen) -= (fetchDataLen);\
        (pos) += (fetchDataLen);\
    }\
}

/*! *********************************************************************************
*\def           CheckSkipBytes(pos, remainingLen, fetchDataLen, endVar, x)
*
*\brief         Check if fetchDataLen fits inside remainingLen. This macro is designed 
*               to be used in a loop.
*               If this is true execute the code defined with x, advance pos pointer 
*               with fetchDataLen and decrease remainingLen with the same value.
*               In case the result is false set endVar to TRUE and call break (to exit 
*               the loop).
*
*\param         pos             Pointer to start of data.
*\param         remainingLen    Remaining length to parse.
*\param         fetchDataLen    Data length that needs to be parsed.
*\param         endVar          Variable to set to FALSE in case of incomplete data.
*\param         x               Code to be executed in case of success.
*
*\retval        None
********************************************************************************** */
#define CheckSkipBytes(pos, remainingLen, fetchDataLen, endVar, x)\
{\
    if ((remainingLen) < (fetchDataLen))\
    {\
        (endVar) = TRUE;\
        break;\
    }\
    else\
    {\
        {x}\
        (remainingLen) -= (fetchDataLen);\
        (pos) += (fetchDataLen);\
    }\
}

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
/* Antenna permutation order */
const uint8_t gaAntPermNAp[24][gMaxNumAntennaPaths_c] = {
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

#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1)
static uint8_t aPermIndexList[ANT_PERM_INDEX_LIST_LEN] = {};
static uint16_t mProducer = 0U;
static uint16_t mConsumer = 0U;
static uint16_t mCount = 0U;

static void AddItem(uint8_t item);
static bool_t GetItem(uint8_t *pItem);
#endif /* !defined (gAppRasDataTransfer_d) || (gAppRasDataTransfer_d == 0) */

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/

static void hciCsStoreBytesInTofBuffer
(
    csAppData_t *appData,
    uint8_t *source,
    int nbBytes
);

static void hciCsStoreBytesInIqBuffer
(
    csAppData_t *appData,
    uint8_t *source,
    int nbBytes
);

#if defined(gRasRREQ_d) && (gRasRREQ_d == 1U)
static bool_t HandleRemoteModes
(
    uint8_t mode,
    uint16_t filter,
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    csAppData_t *pDstAppBuffer,
    rasMeasurementData_t *pRemoteData
);
#endif /* #if defined(gRasRREQ_d) && (gRasRREQ_d == 1U) */
#if defined(gRasRREQ_d) && (gRasRREQ_d == 1U) || \
    defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
static bool_t ParseMode0
(
    uint16_t filter, 
    uint8_t **ppEventData, 
    uint32_t *pDataLength,
    csAppData_t *pDstAppBuffer,
    rasMeasurementData_t *pRemoteData
);

static bool_t ParseMode1
(
    uint16_t filter, 
    uint8_t **ppEventData, 
    uint32_t *pDataLength, 
    csAppData_t *pDstAppBuffer, 
    rasMeasurementData_t *pRemoteData
);

static bool_t ParseMode2
(
    uint16_t filter, 
    uint8_t **ppEventData, 
    uint32_t *pDataLength, 
    csAppData_t *pDstAppBuffer, 
    rasMeasurementData_t *pRemoteData
);

static bool_t ParseMode3
(
    uint16_t filter, 
    uint8_t **ppEventData, 
    uint32_t *pDataLength, 
    csAppData_t *pDstAppBuffer, 
    rasMeasurementData_t *pRemoteData
);
#endif /* defined(gRasRREQ_d) && (gRasRREQ_d == 1U) && \
    defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U) */
static void CheckAllLocalDataParsed
(
    rasMeasurementData_t *pLocalData,
    csAppData_t *pDstAppBuffer
);
static bool_t HandleLocalTofPresent
(
    uint8_t **ppEventData,
    uint32_t *pDataSize,
    csAppData_t *pDstAppBuffer
);
static bool_t HandleLocalMciqPresent
(
    uint8_t **ppEventData,
    uint32_t *pDataSize,
    csAppData_t *pDstAppBuffer
);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn         void AppLocalizationAlgo_UncompressResponse(uint8_t *pEventData,
*            uint32_t dataSize, rasMeasurementData_t *pLocalData);
*
*\brief      Uncompress OTA data (HCI-like format) for a given device.
*
*\param[in]  pEventData         Pointer to the received data.
*\param[in]  dataSize           Size of the received data.
*\param[out] pLocalData         Pointer to the output rasMeasurementData_t structure
*
*\retval     none
********************************************************************************** */
void AppLocalizationAlgo_UncompressResponse
(
    uint8_t *pEventData,
    uint32_t dataSize,
    rasMeasurementData_t *pLocalData
)
{
#ifdef SKIP_MAIN_MODES_REPET
    uint32_t parsedMainModeNb = CS_MAIN_MODE_REPEAT_MAX;
#endif
    csAppData_t *pDstAppBuffer = (csAppData_t*)(void*)pLocalData->pData;
    bool_t bIncomplete = FALSE;

    pDstAppBuffer->mciq_data.n_ap = pLocalData->numAntennaPaths;

    pDstAppBuffer->csData.subevt_nb = 0U;

    for (;pLocalData->crtStep < pLocalData->totalNumSteps; pLocalData->crtStep++)
    {
        bool_t tofPresent = FALSE;
        bool_t mciqPresent = FALSE;
        uint8_t mode = (uint8_t)gCsStepMode0_c;
        uint8_t channel = 0U;
        uint8_t stepDataLength = 0U;

        CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete, 
            mode = *pEventData;
        );

        CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete, 
            channel = *pEventData;
        );

        CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete, 
            stepDataLength = *pEventData;
        );

        pDstAppBuffer->csData.channelMap[pLocalData->step] = channel;
        pDstAppBuffer->csData.modeMap[pLocalData->step] = mode;

        /* Check if there is reported step data - skip to next step if not */
        if (stepDataLength == 0U)
        {
            pLocalData->step++;
            continue;
        }

        if (mode == (uint8_t)gCsStepMode0_c)
        {
            assert(pDstAppBuffer->mode0BufferOffset < (gMaxNumCsStepsMode0_c * gMaxNumCsSubevents_c));
            CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete,
                pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].quality = *pEventData;
            );
            CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete,
                pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].rssi = (int8_t)*pEventData;
            );
            CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete,
                pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].antenna = *pEventData;
            );
            if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
            {
                CheckSkipBytes(pEventData, dataSize, sizeof(uint16_t), bIncomplete,
                    pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].measuredFreqOffset = Utils_ExtractTwoByteValue(pEventData);
                );
            }
            else
            {
                pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].measuredFreqOffset = 0U;
            }
            pDstAppBuffer->mode0BufferOffset++;
            pLocalData->step++;
            continue;
        }

#ifdef SKIP_MAIN_MODES_REPET
        /* Skip main mode repetitions */
        if ((mode == 2) && (parsedMainModeNb < meas_params.cfg.main_mode_repeat)) {
            CheckSkipBytes(pEventData, dataSize, stepDataLength, bIncomplete,
                parsedMainModeNb++;
            );
            continue;
        }
#endif

        switch(mode)
        {
            case (uint8_t)gCsStepMode1_c:
            {
                /* ToF record */
                tofPresent = TRUE;
            }
            break;

            case (uint8_t)gCsStepMode2_c:
            {
                /* Tone record */
                mciqPresent = TRUE;
            }
            break;

            case (uint8_t)gCsStepMode3_c:
            {
                /* ToF+Tone record */
                tofPresent = TRUE;
                mciqPresent = TRUE;
            }
            break;

            default:
            {
                /* mode not yet implemented, skip data */
                CheckSkipBytesDoNothing(pEventData, dataSize, stepDataLength, bIncomplete);
            }
            break;
        }

        if (tofPresent)
        {
            bIncomplete = HandleLocalTofPresent(&pEventData, &dataSize, pDstAppBuffer);
            if (bIncomplete == TRUE)
            {
                break;
            }
        }

        if (mciqPresent)
        {
            bIncomplete = HandleLocalMciqPresent(&pEventData, &dataSize, pDstAppBuffer);
            if (bIncomplete == TRUE)
            {
                break;
            }
        }

        pLocalData->step ++;
    }

    CheckAllLocalDataParsed(pLocalData, pDstAppBuffer);
}

void* AppLocalizationAlgo_AllocData(void)
{
    void *pResult = MEM_BufferAlloc(sizeof(csAppData_t));
    
    if (pResult != NULL)
    {
        FLib_MemSet(pResult, 0, sizeof(csAppData_t));
    }
    return pResult;
}

#if defined (gRasRREQ_d) && (gRasRREQ_d == 1)
/*! *********************************************************************************
*\brief     Merge previously buffered remote event data with the new chunk.
*
*\param[in,out] ppEventData   Pointer to the current event data pointer.
*\param[in,out] pDataLength   Pointer to the current data length.
*\param[out]    ppTemp        Pointer to the intermediate buffer pointer (allocated here).
*\param[in,out] pRemoteData   Pointer to the remote measurement data structure.
*
*\retval    none
********************************************************************************** */
static void MergeRemainingRemoteData
(
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    uint8_t **ppTemp,
    rasMeasurementData_t *pRemoteData
)
{
    /* Check if we have uncompressed event data left from a previous event */
    if (pRemoteData->pRemaining != NULL)
    {
        uint8_t *pTemp = MEM_BufferAlloc(*pDataLength + pRemoteData->remainingLen);
        if (pTemp != NULL)
        {
            /* Merge previous event data with this new event data */
            FLib_MemCpy(pTemp, pRemoteData->pRemaining, pRemoteData->remainingLen);
            FLib_MemCpy(pTemp + pRemoteData->remainingLen, *ppEventData, *pDataLength);
            *ppEventData = pTemp;
            *pDataLength += pRemoteData->remainingLen;
            *ppTemp = pTemp;

            /* Clear previous event data */
            (void)MEM_BufferFree(pRemoteData->pRemaining);
            pRemoteData->pRemaining = NULL;
            pRemoteData->remainingLen = 0U;
        }
        else
        {
            panic(0, 0, 0, 0);
        }
    }
}

/*! *********************************************************************************
*\brief     Proprietary IPT solution: filter out mode-2 and mode-3 PCT data.
*
*\param[in] mode      Current CS step mode.
*\param[in] filter    Filter value to be adjusted.
*
*\retval    uint16_t  The adjusted filter value.
********************************************************************************** */
static uint16_t FilterInlinePctData(uint8_t mode, uint16_t filter)
{
    if (mode == 2U)
    {
        filter &= ~((uint16_t)1U << 2U);
        filter &= ~((uint16_t)1U << 3U);
        filter &= ~((uint16_t)1U << 4U);
    }
    else if (mode == 3U)
    {
        filter &= ~((uint16_t)1U << 9U);
        filter &= ~((uint16_t)1U << 10U);
        filter &= ~((uint16_t)1U << 11U);
    }
    else
    {
        /* MISRA */
    }

    return filter;
}

/*! *********************************************************************************
*\brief     Parse the mode byte and the mode data for a single remote CS step.
*
*\param[in,out] ppEventData   Pointer to the current event data pointer.
*\param[in,out] pDataLength   Pointer to the current data length.
*\param[in,out] pDstAppBuffer Pointer to the destination application buffer.
*\param[in,out] pRemoteData   Pointer to the remote measurement data structure.
*
*\retval    bool_t  TRUE if the step data is incomplete, FALSE otherwise.
********************************************************************************** */
static bool_t ParseRemoteStepMode
(
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    csAppData_t *pDstAppBuffer,
    rasMeasurementData_t *pRemoteData
)
{
    bool_t bIncomplete = FALSE;
    uint8_t mode = 0U;
    uint16_t filter = 0U;
    uint8_t *pEventData = *ppEventData;
    uint32_t dataLength = *pDataLength;

    do
    {
        CheckSkipBytes(pEventData, dataLength, sizeof(uint8_t), bIncomplete,
            mode = *pEventData;
        );
    } while(FALSE);

    if (bIncomplete == FALSE)
    {
        if ((mode & BIT7) != 0U)
        {
            /* Step aborted, assume length zero */
        }
        else
        {
            assert(mode <= 3U);

            pDstAppBuffer->csData.modeMap[pRemoteData->step] = mode;
            /* Get filter for the current mode */
            filter = RasClient_GetModeFilter(pRemoteData->deviceId, mode);
            /* Proprietary IPT solution: filter out mode-2 and mode-3 PCT data */
            if (mRangeSettings[pRemoteData->deviceId].inlinePctEnabled == TRUE)
            {
                filter = FilterInlinePctData(mode, filter);
            }
            /* Unpack mode data */
            bIncomplete = HandleRemoteModes(mode, filter, &pEventData,
                                            &dataLength, pDstAppBuffer,
                                            pRemoteData);
        }
    }

    *ppEventData = pEventData;
    *pDataLength = dataLength;

    return bIncomplete;
}

/*! *********************************************************************************
*\brief     Populate the additional destination buffer fields once the last
*           segment has been received.
*
*\param[in,out] pRemoteData   Pointer to the remote measurement data structure.
*\param[in,out] pDstAppBuffer Pointer to the destination application buffer.
*
*\retval    none
********************************************************************************** */
static void PopulateRemoteLastSegmentInfo
(
    rasMeasurementData_t *pRemoteData,
    csAppData_t *pDstAppBuffer
)
{
    uint16_t totalStepCounter = 0U;

    /* Total number of steps */
    pDstAppBuffer->csData.step_nb = (uint16_t)pRemoteData->step;

    /* Start ACL count */
    pDstAppBuffer->csData.startAclCnt =
            pRemoteData->aSubEventData[pDstAppBuffer->csData.subevt_nb].subevtHeader.startACLConnEvent;

    /* For every subevent */
    for (uint8_t index = 0U; index <= pRemoteData->subeventIndex; index++)
    {
        /* The stop index is the total number of previous steps */
        pDstAppBuffer->csData.subevtStopIdxRemote[index] =
            (uint8_t)totalStepCounter + pRemoteData->aSubEventData[index].subevtHeader.numStepsReported;

        /* Delta regarding ACL counter of first subevent */
        pDstAppBuffer->csData.subevtConnEvent[index] =
            (uint8_t)(pRemoteData->aSubEventData[index].subevtHeader.startACLConnEvent - pDstAppBuffer->csData.startAclCnt);

        /* Save the reference power level in subevtRefPowerLevelInit - will be switched to the proper role by the caller */
        pDstAppBuffer->csData.subevtRefPowerLevelInit[index] = pRemoteData->aSubEventData[index].subevtHeader.referencePowerLevel;

        /* Count handled steps */
        totalStepCounter += pRemoteData->aSubEventData[index].subevtHeader.numStepsReported;
    }

    /* Total number of subevents */
    pDstAppBuffer->csData.subevt_nb = pRemoteData->subeventIndex + 1U;
}

/*! *********************************************************************************
*\fn        void AppLocalizationAlgo_UncompressRemoteResponse(uint8_t *pEventData,
*           uint32_t dataLength, rasMeasurementData_t *pRemoteData, bool_t lastSegment);
*
*\brief     Uncompress CS data, received from the peer, on-the-fly.
*
*\param[in] pData               Pointer to the received chhunk of data.
*\param[in] dataLength          Size of the received data chunk.
*\param[in] pRemoteData         Pointer to rasMeasurementData_t structure containing 
*                               the unpacked data information.
*\param[in] lastSegment         Is this the last segment or not.
*
*\retval    none
********************************************************************************** */
void AppLocalizationAlgo_UncompressRemoteResponse
(
    uint8_t *pEventData,
    uint32_t dataLength,
    rasMeasurementData_t *pRemoteData,
    bool_t lastSegment
)
{
    csAppData_t *pDstAppBuffer = (csAppData_t*)(void*)pRemoteData->pData;

    bool_t bIncomplete = FALSE;
    uint8_t *pLastOk = NULL;
    uint8_t *pTemp = NULL;
    uint8_t crtNumSteps = 0U;
    uint8_t totalNumSteps = 0U;

    /* Make sure the number of antenna paths is set at the begining of the uncompress procedure */
    if (pDstAppBuffer->mciq_data.n_ap == 0U)
    {
        pDstAppBuffer->mciq_data.n_ap = pRemoteData->numAntennaPaths;
    }

    /* Merge any uncompressed event data left from a previous event with this chunk */
    MergeRemainingRemoteData(&pEventData, &dataLength, &pTemp, pRemoteData);

    /* Loop through all of the received bytes */
    while ((dataLength > 0U) && (bIncomplete == FALSE))
    {
        /* Keep a pointer to the end of the last completed parse */
        pLastOk = pEventData;
        pRemoteData->remainingLen = dataLength;
        
        /* Check if we have a new subevent */
        if (pRemoteData->crtNumSteps ==
            pRemoteData->aSubEventData[pRemoteData->subeventIndex].subevtHeader.numStepsReported)
        {
            crtNumSteps = pRemoteData->crtNumSteps;
            totalNumSteps = pRemoteData->totalNumSteps;

            /* move on to the next subevent */
            pRemoteData->subeventIndex++;
            pRemoteData->crtNumSteps = 0U;

            do
            {
                CheckSkipBytes(pEventData, dataLength, sizeof(rasSubeventDataHeader_t), bIncomplete, 
                        RasClient_ParseReceivedSubeventHeader(pRemoteData->deviceId, pEventData);
                );
            } while(FALSE);
            
            /* In case the subevent has no data, continue the loop.
               We can have another subevent header or no more data.
               In case the subevent is split between 2 ATT frames bIncomplete is TRUE */
            if ((totalNumSteps == pRemoteData->totalNumSteps) && (bIncomplete == FALSE))
            {
                continue;
            }
        }

        if (bIncomplete == FALSE)
        {
            bIncomplete = ParseRemoteStepMode(&pEventData, &dataLength, pDstAppBuffer, pRemoteData);
        }

        /* Data is complete */
        if (bIncomplete == FALSE)
        {
            /* Increase parsed number of steps for the current subevent */
            pRemoteData->crtNumSteps++;
        }
        /* Incomplete data.
           In case we stopped in the middle of parsing -> enqueue this buffer
           and exit the procedure to wait for the next chunk and try again with
           this buffer concatenated with the following one */
        else
        {
            /* Reverse new subevent header global data update */
            if (pRemoteData->crtNumSteps == 0U)
            {
                pRemoteData->crtNumSteps = crtNumSteps;
                pRemoteData->totalNumSteps = totalNumSteps;
                pRemoteData->subeventIndex--;
            }
            
            pRemoteData->pRemaining = MEM_BufferAlloc(pRemoteData->remainingLen);
            if (pRemoteData->pRemaining != NULL)
            {
                FLib_MemCpy(pRemoteData->pRemaining, pLastOk, pRemoteData->remainingLen);
            }
            break;
        }
    }
    
    /* Clear memory if we have intermediate buffer */
    if (pTemp != NULL)
    {
        pEventData = NULL;
        (void)MEM_BufferFree(pTemp);
    }

    /* Populate additional fields in pDstAppBuffer */
    if (lastSegment == TRUE)
    {
        PopulateRemoteLastSegmentInfo(pRemoteData, pDstAppBuffer);
    }
}
#endif /* defined (gRasRREQ_d) && (gRasRREQ_d == 1) */

#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1)
/*! *********************************************************************************
 * \brief        Uncompress BTCS Ranging Data for a given device
 *
*\param[in] pEventData          Pointer to the received chhunk of data.
*\param[in] dataLength          Size of the received data chunk.
*\param[in] pRemoteData         Pointer to rasMeasurementData_t structure containing 
*                               the unpacked data information.
*\param[in] maxSteps            Maximum number of steps to unpack 
 ********************************************************************************** */
uint32_t AppLocalizationAlgo_UncompressRemoteResponseL2CAP
(
    uint8_t*   pEventData,
    uint32_t   dataLength,
    rasMeasurementData_t *pRemoteData,
    uint8_t maxSteps
)
{
    csAppData_t *pDstAppBuffer = (csAppData_t*)(void*)pRemoteData->pData;
    
    uint8_t mode = 0U;
    bool_t bIncomplete = FALSE;
    uint8_t *pLastOk = NULL;
    uint8_t *pTemp = NULL;
    uint8_t parsedSteps = 0U;

    /* Make sure the number of antenna paths is set at the begining of the uncompress procedure */
    if (pDstAppBuffer->mciq_data.n_ap == 0U)
    {
        pDstAppBuffer->mciq_data.n_ap = pRemoteData->numAntennaPaths;
    }

    /* Check if we have uncompressed event data left from a previous event */
    if (pRemoteData->pRemaining != NULL)
    {
        pTemp = MEM_BufferAlloc(dataLength + pRemoteData->remainingLen);
        if (pTemp != NULL)
        {
            /* Merge previous event data with this new event data */
            FLib_MemCpy(pTemp, pRemoteData->pRemaining, pRemoteData->remainingLen);
            FLib_MemCpy(pTemp + pRemoteData->remainingLen, pEventData, dataLength);
            pEventData = pTemp;
            dataLength += pRemoteData->remainingLen;

            /* Clear previous event data */
            (void)MEM_BufferFree(pRemoteData->pRemaining);
            pRemoteData->pRemaining = NULL;
            pRemoteData->remainingLen = 0U;
        }
        else
        {
            panic(0, 0,0, 0);
        }
    }
    
    /* Loop through all of the received bytes */
    while ((dataLength > 0U) && (bIncomplete == FALSE) && (parsedSteps < maxSteps))
    {
        /* Keep a pointer to the end of the last completed parse */
        pLastOk = pEventData;
        pRemoteData->remainingLen = dataLength;

        /* Read mode from the pre-populated modeMap array (filled by parseStepModeArray) */
        mode = pDstAppBuffer->csData.modeMap[pRemoteData->step];

        /* Make sure the mode is valid */
        assert(mode <= (uint8_t)gCsStepMode3_c);

        switch(mode)
        {
            case (uint8_t)gCsStepMode0_c:
            {
                bIncomplete = ParseMode0(0, &pEventData, &dataLength, pDstAppBuffer, pRemoteData);
            }
            break;

            case (uint8_t)gCsStepMode1_c:
            {
                bIncomplete = ParseMode1(0, &pEventData, &dataLength, pDstAppBuffer, pRemoteData);
            }
            break;

            case (uint8_t)gCsStepMode2_c:
            {
                bIncomplete = ParseMode2(0, &pEventData, &dataLength, pDstAppBuffer, pRemoteData);
            }
            break;

            case (uint8_t)gCsStepMode3_c:
            {
                bIncomplete = ParseMode3(0, &pEventData, &dataLength, pDstAppBuffer, pRemoteData);
            }
            break;

            default:
            {
                /* mode not yet implemented, skip data */
            }
            break;
        }

        /* Data is complete */
        if (bIncomplete == FALSE)
        {
            /* Increase parsed number of steps */
            parsedSteps++;
            pRemoteData->crtNumSteps++;
        }
        /* Incomplete data.
           In case we stopped in the middle of parsing -> enqueue this buffer
           and exit the procedure to wait for the next chunk and try again with
           this buffer concatenated with the following one */
        else
        {
            pRemoteData->pRemaining = MEM_BufferAlloc(pRemoteData->remainingLen);
            if (pRemoteData->pRemaining != NULL)
            {
                FLib_MemCpy(pRemoteData->pRemaining, pLastOk, pRemoteData->remainingLen);
            }
            break;
        }
    }
    
    /* Clear memory if we have intermediate buffer */
    if (pTemp != NULL)
    {
        pEventData = NULL;
        (void)MEM_BufferFree(pTemp);
    }
    
    return dataLength;
}
#endif /* defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1) */

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
 *\fn           static bool_t HandleLocalTofPresent(uint8_t **ppEventData,
 *                                                   uint32_t *pDataSize,
 *                                                   csAppData_t *pDstAppBuffer)
 *
 * \brief       Parse and store Time-of-Flight (ToF) record data from local CS event.
 *              Extracts quality, NADM, RSSI, and timestamp difference information
 *              from the event data and stores it in the ToF buffer.
 *
 * \param[in,out] ppEventData       Pointer to pointer of event data. Updated to point
 *                                  after parsed ToF data.
 * \param[in,out] pDataSize         Pointer to remaining data size. Updated after parsing.
 * \param[out]    pDstAppBuffer     Pointer to destination application buffer for storing
 *                                  parsed ToF data
 *
 *\retval       bool_t              TRUE if data is incomplete, FALSE if parsing succeeded
 ********************************************************************************** */
static bool_t HandleLocalTofPresent
(
    uint8_t **ppEventData,
    uint32_t *pDataSize,
    csAppData_t *pDstAppBuffer
)
{
    uint32_t ts_diff = 0U;
    int16_t ts_diff_hci = 0;
    int16_t temp1 = 0;
    /* ToF record */
    uint32_t quality = 0U;
    bool_t bIncomplete = FALSE;
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
    int8_t rssiValue = (int8_t)gRssiNotAvailable_c;
#endif /* gAppParseRssiInfo_d */

    do
    {
        CheckSkipBytes(*ppEventData, *pDataSize, sizeof(uint8_t), bIncomplete, 
            quality = (uint32_t)(**ppEventData); /* Packet_AA_Quality */
        );
        
        CheckSkipBytes(*ppEventData, *pDataSize, gCsNadmSize_c, bIncomplete, 
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, *ppEventData, (int)gCsNadmSize_c); /* Packet_NADM */
        );

#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
        CheckSkipBytes(*ppEventData, *pDataSize, gCsRssiSize_c, bIncomplete, 
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, *ppEventData, (int)gCsRssiSize_c); /* Packet_RSSI */
            rssiValue = (int8_t)(**ppEventData); /* capture before pointer advances */
        );

        if (rssiValue != (int8_t)gRssiNotAvailable_c)
        {
            /* Count RSSI if available */
            pDstAppBuffer->aRssiValue[pDstAppBuffer->rssiStepNo] = rssiValue;
            pDstAppBuffer->rssiStepNo++;
        }
#else
        CheckSkipBytes(*ppEventData, *pDataSize, gCsRssiSize_c, bIncomplete, 
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, *ppEventData, (int)gCsRssiSize_c); /* Packet_RSSI */
        );
#endif /* gAppParseRssiInfo_d */

        CheckSkipBytes(*ppEventData, *pDataSize, sizeof(uint16_t), bIncomplete, 
            FLib_MemCpy(&ts_diff_hci, *ppEventData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
            /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
            temp1 = ts_diff_hci/2;
            ts_diff = (uint32_t)(temp1); /* HCI reports half ns, application expects ns in Tof Buffer */
            ts_diff &= 0x00FFFFU;
            ts_diff |= (quality&0x0FU)<<gTimeStampDiffSize_c;
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)gCsTsSize_c);
        );

        /* Store Packet_Antenna in tof buffer after TS */
        CheckSkipBytes(*ppEventData, *pDataSize, gCsAntennaSize_c, bIncomplete,
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, *ppEventData, (int)gCsAntennaSize_c); /* Packet_Antenna */
        );
        
        pDstAppBuffer->tof_data.nbSteps++;
    } while(FALSE);

    return bIncomplete;
}

/*! *********************************************************************************
 *\fn           static bool_t HandleLocalMciqPresent(uint8_t **ppEventData,
 *                                                    uint32_t *pDataSize,
 *                                                    csAppData_t *pDstAppBuffer)
 *
 * \brief       Parse and store Mode Change IQ (MCIQ) tone record data from local CS event.
 *              Extracts antenna permutation index, IQ samples, and tone quality
 *              indicators for all antenna paths and stores them in the IQ buffer.
 *              The IQ samples are reordered according to the antenna permutation index.
 *
 * \param[in,out] ppEventData       Pointer to pointer of event data. Updated to point
 *                                  after parsed MCIQ data.
 * \param[in,out] pDataSize         Pointer to remaining data size. Updated after parsing.
 * \param[out]    pDstAppBuffer     Pointer to destination application buffer for storing
 *                                  parsed IQ data
 *
 *\retval       bool_t              TRUE if data is incomplete, FALSE if parsing succeeded
 ********************************************************************************** */
static bool_t HandleLocalMciqPresent
(
    uint8_t **ppEventData,
    uint32_t *pDataSize,
    csAppData_t *pDstAppBuffer
)
{
    /* Tone record */
    uint8_t antPermIndex = 0U;
    const uint8_t *antIndex_p = NULL;
    int32_t iq_dec[gMaxNumAntennaPaths_c] = {};
    uint8_t tqi[gMaxNumAntennaPaths_c] = {};
    bool_t bIncomplete = FALSE;

    do
    {
        CheckSkipBytes(*ppEventData, *pDataSize, 1U, bIncomplete,
            antPermIndex = **ppEventData; /* Antenna_Permutation_Index */
        );

#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1)
        AddItem(antPermIndex);
#endif

        antIndex_p = &gaAntPermNAp[antPermIndex][0];
        
        if (pDstAppBuffer->mciq_data.n_ap > gMaxNumAntennaPaths_c)
        {
            bIncomplete = TRUE;
            break;
        }

        /* Num_Antenna_Path + 1 are reported by the firmware, but discard last one */
        /* Re-order per antenna path index */
        for (uint8_t idx = 0U; idx < pDstAppBuffer->mciq_data.n_ap; idx++)
        {
            int antIdx = (int)antIndex_p[idx];

            uint32_t temp1 = (((uint32_t)(*ppEventData)[2])<<BIT4) | (((uint32_t)(*ppEventData)[1])<<BIT3)
                             | ((uint32_t)(*ppEventData)[0]);
            int32_t iq = (int32_t)temp1;

            CheckSkipBytes(*ppEventData, *pDataSize, gTone_PCTSize_c, bIncomplete,
                /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                                  ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                iq_dec[antIdx] = (int32_t)temp2;
            );

            CheckSkipBytes(*ppEventData, *pDataSize, sizeof(uint8_t), bIncomplete,
                tqi[antIdx] = **ppEventData;
            );
        }

        if (bIncomplete == FALSE)
        {
            for (uint8_t idx = 0U; idx < pDstAppBuffer->mciq_data.n_ap; idx++)
            {
                hciCsStoreBytesInIqBuffer(pDstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
                hciCsStoreBytesInIqBuffer(pDstAppBuffer, &tqi[idx], 1);
            }

            /* Skip last IQ data (n_ap+1) */
            CheckSkipBytesDoNothing(*ppEventData, *pDataSize, gTone_PCTSize_c + sizeof(uint8_t), bIncomplete);

            pDstAppBuffer->mciq_data.nbSteps ++;
        }
    } while(FALSE);
    
    return bIncomplete;
}

/*! *********************************************************************************
 *\fn           static void CheckAllLocalDataParsed(rasMeasurementData_t *pLocalData,
 *                                                   csAppData_t *pDstAppBuffer)
 *
 * \brief       Check if all local CS data has been parsed and populate additional
 *              fields in the application buffer. This function is called after
 *              parsing local measurement data to finalize the data structure with
 *              subevent information, step counts, and ACL connection event details.
 *
 * \param[in]   pLocalData          Pointer to local measurement data structure
 *                                  containing parsing state and subevent information
 * \param[out]  pDstAppBuffer       Pointer to destination application buffer where
 *                                  final data will be stored
 *
 *\retval       none
 ********************************************************************************** */
static void CheckAllLocalDataParsed
(
    rasMeasurementData_t *pLocalData,
    csAppData_t *pDstAppBuffer
)
{
    int totalStepCounter = 0;

    /* All data was parsed */
    if ((pLocalData->crtStep >= pLocalData->totalNumSteps) && (pLocalData->subeventIndex < gMaxNumCsSubevents_c))
    {
        /* Populate additional fields in pDstAppBuffer */

        /* Total number of steps */
        pDstAppBuffer->csData.step_nb = (uint16_t)pLocalData->step;

        /* Start ACL count */
        pDstAppBuffer->csData.startAclCnt =
                pLocalData->aSubEventData[pDstAppBuffer->csData.subevt_nb].subevtHeader.startACLConnEvent;

        /* For every subevent */
        for (uint8_t index = 0U; index <= pLocalData->subeventIndex; index++)
        {
            /* The stop index is the total number of previous steps plus the current subevent's steps */
            pDstAppBuffer->csData.subevtStopIdxLocal[index] =
                (uint8_t)totalStepCounter + pLocalData->aSubEventData[index].subevtHeader.numStepsReported;

            /* Delta regarding ACL counter of first subevent */
            pDstAppBuffer->csData.subevtConnEvent[index] =
                (uint8_t)(pLocalData->aSubEventData[index].subevtHeader.startACLConnEvent - pDstAppBuffer->csData.startAclCnt);

            /* Count handled steps */
            totalStepCounter += (int)pLocalData->aSubEventData[index].subevtHeader.numStepsReported;

            /* Save the reference power level in subevtRefPowerLevelInit - will be switched to the proper role by the caller */
            pDstAppBuffer->csData.subevtRefPowerLevelInit[index] = pLocalData->aSubEventData[index].subevtHeader.referencePowerLevel;
        }

        /* Total number of subevents */
        pDstAppBuffer->csData.subevt_nb =  pLocalData->subeventIndex + 1U;
    }
}

/*! *********************************************************************************
 *\fn           void hciCsStoreBytesInTofBuffer(csAppData_t *appData,
 *                                  uint8_t *source,
 *                                  int nbBytes);
 *
 * \brief       Store data to send in application buffer.
 *
 * \param[in]   appData             Pointer to application data
 * \param[in]   source              Pointer to source of data
 * \param[in]   nbBytes             Number of bytes to be copied
 *
 *\retval       none
 ********************************************************************************** */
static void hciCsStoreBytesInTofBuffer
(
    csAppData_t *appData,
    uint8_t *source,
    int nbBytes
)
{
    assert((appData->tofBufferOffset + (uint16_t)nbBytes) <= sizeof(appData->tofBuffer));
    FLib_MemCpy(appData->tofBuffer + appData->tofBufferOffset, source, (uint32_t)nbBytes);
    appData->tofBufferOffset += (uint16_t)nbBytes;
}

/*! *********************************************************************************
 *\fn           void hciCsStoreBytesInIqBuffer(csAppData_t *appData,
 *                                  uint8_t *source,
 *                                  int nbBytes);
 *
 * \brief       Store IQ information in application buffer.
 *
 * \param[in]   appData             Pointer to application data
 * \param[in]   source              Pointer to source of IQ data
 * \param[in]   nbBytes             Number of bytes to be copied
 *
 *\retval       none
 ********************************************************************************** */
static void hciCsStoreBytesInIqBuffer
(
    csAppData_t *appData,
    uint8_t *source,
    int nbBytes
)
{
    assert((appData->mciqBufferOffset + (uint16_t)nbBytes) <= sizeof(appData->mciqBuffer));
    FLib_MemCpy(appData->mciqBuffer + appData->mciqBufferOffset, source, (uint32_t)nbBytes);
    appData->mciqBufferOffset += (uint16_t)nbBytes;
}

#if defined (gRasRREQ_d) && (gRasRREQ_d == 1)
/*! *********************************************************************************
 *\fn           static bool_t HandleRemoteModes(uint8_t mode,
 *                                       uint16_t filter, 
 *                                       uint8_t **ppEventData, 
 *                                       uint32_t *pDataLength,
 *                                       csAppData_t *pDstAppBuffer,
 *                                       rasMeasurementData_t *pRemoteData)  
 *
 * \brief       Parse CS Mode 0 data from received event data.
 *              Mode 0 contains only metadata without ToF or tone information.
 *
 * \param[in]     mode              Mode number
 * \param[in]     filter            Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data. Updated to point
 *                                  after parsed data.
 * \param[in,out] pDataLength       Pointer to remaining data length. Updated after parsing.
 * \param[out]    pDstAppBuffer     Pointer to destination application buffer for storing
 *                                  parsed metadata
 * \param[in,out] pRemoteData       Pointer to measurement data structure containing
 *                                  parsing state information
 *
 *\retval       bool_t              TRUE if data is incomplete, FALSE if parsing succeeded
 ********************************************************************************** */
static bool_t HandleRemoteModes
(
    uint8_t mode,
    uint16_t filter,
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    csAppData_t *pDstAppBuffer,
    rasMeasurementData_t *pRemoteData
)
{
    bool_t bIncomplete = FALSE;
    switch(mode)
    {
        case (uint8_t)gCsStepMode0_c:
        {
            bIncomplete = ParseMode0(filter, ppEventData, pDataLength, pDstAppBuffer, pRemoteData);
        }
        break;

        case (uint8_t)gCsStepMode1_c:
        {
            bIncomplete = ParseMode1(filter, ppEventData, pDataLength, pDstAppBuffer, pRemoteData);
        }
        break;

        case (uint8_t)gCsStepMode2_c:
        {
            bIncomplete = ParseMode2(filter, ppEventData, pDataLength, pDstAppBuffer, pRemoteData);
        }
        break;

        case (uint8_t)gCsStepMode3_c:
        {
            bIncomplete = ParseMode3(filter, ppEventData, pDataLength, pDstAppBuffer, pRemoteData);
        }
        break;

        default:
        {
            /* mode not yet implemented, skip data */
        }
        break;
    }
    
    return bIncomplete;
}

/*! *********************************************************************************
 *\fn           static bool_t ParseMode0(uint16_t filter, 
 *                                       uint8_t **ppEventData, 
 *                                       uint32_t *pDataLength,
 *                                       csAppData_t *pDstAppBuffer,
 *                                       rasMeasurementData_t *pRemoteData)  
 *
 * \brief       Parse CS Mode 0 data from received event data.
 *              Mode 0 contains only metadata without ToF or tone information.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data. Updated to point
 *                                  after parsed data.
 * \param[in,out] pDataLength       Pointer to remaining data length. Updated after parsing.
 * \param[out]  pDstAppBuffer       Pointer to destination application buffer for storing
 *                                  parsed metadata
 * \param[in,out] pRemoteData       Pointer to measurement data structure containing
 *                                  parsing state information
 *
 *\retval       bool_t              TRUE if data is incomplete, FALSE if parsing succeeded
 ********************************************************************************** */
static bool_t ParseMode0
(
    uint16_t filter, 
    uint8_t **ppEventData, 
    uint32_t *pDataLength,
    csAppData_t *pDstAppBuffer,
    rasMeasurementData_t *pRemoteData
)
{
    bool_t bIncomplete = FALSE;

    assert(pDstAppBuffer->mode0BufferOffset < (gMaxNumCsStepsMode0_c * gMaxNumCsSubevents_c));

    do
    {
        if ((filter & BIT2) != 0U)
        {
            /* Data includes Packet Quality*/
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
                pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].quality = **ppEventData;
            );
        }

        if ((filter & BIT3) != 0U)
        {
            /* Data includes Packet RSSI */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
                pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].rssi = (int8_t)**ppEventData;
            );
        }

        if ((filter & BIT4) != 0U)
        {
            /* Data includes Packet Antenna */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
                pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].antenna = **ppEventData;
            );
        }

        if ((mGlobalRangeSettings.role == gCsRoleReflector_c)
            && ((filter & BIT5) != 0U))
        {
            /* Data includes Measured_Freq_Offset information */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint16_t), bIncomplete,
                pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].measuredFreqOffset = Utils_ExtractTwoByteValue(*ppEventData);
            );
        }
        else
        {
            pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].measuredFreqOffset = 0U;
        }
    } while(FALSE);

    /* if parsing is complete, count the step */
    if (bIncomplete == FALSE)
    {
        pDstAppBuffer->mode0BufferOffset++;
        pRemoteData->step++;
    }

    return bIncomplete;
}

/*! *********************************************************************************
 *\fn           static bool_t ParseMode1(uint16_t filter, 
 *                                       uint8_t **ppEventData, 
 *                                       uint32_t *pDataLength, 
 *                                       csAppData_t *pDstAppBuffer, 
 *                                       rasMeasurementData_t *pRemoteData)
 *
 * \brief       Parse CS Mode 1 data from received event data.
 *              Mode 1 contains Time-of-Flight (ToF) record information.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data. Updated to point
 *                                  after parsed data.
 * \param[in,out] pDataLength       Pointer to remaining data length. Updated after parsing.
 * \param[out]  pDstAppBuffer       Pointer to destination application buffer for storing
 *                                  parsed ToF data
 * \param[in,out] pRemoteData       Pointer to measurement data structure containing
 *                                  parsing state information
 *
 *\retval       bool_t              TRUE if data is incomplete, FALSE if parsing succeeded
 ********************************************************************************** */
static bool_t ParseMode1
(
    uint16_t filter, 
    uint8_t **ppEventData, 
    uint32_t *pDataLength, 
    csAppData_t *pDstAppBuffer, 
    rasMeasurementData_t *pRemoteData
)
{
    bool_t bIncomplete = FALSE;
    
    uint32_t quality = 0U;
    uint8_t aNadm[gCsNadmSize_c] = {};
    uint8_t aRssi[gCsRssiSize_c] = {};
    uint8_t aAntenna[gCsAntennaSize_c] = {};
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
    int8_t rssiValue = 0;
#endif
    int16_t ts_diff_hci = 0;
    
    /* First parse all bytes, based on filter bits, and store data in temporary variables */
    do
    {
        if ((filter & BIT2) != 0U)
        {
            /* Data includes Packet Quality*/
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete, 
                quality = (uint32_t)**ppEventData;
            );
        }

        if ((filter & BIT3) != 0U)
        {
            /* Data includes Packet NADM */
            CheckSkipBytes(*ppEventData, *pDataLength, gCsNadmSize_c, bIncomplete,
                FLib_MemCpy(aNadm, *ppEventData, gCsNadmSize_c); /* Packet_NADM */
            );
        }

        if ((filter & BIT4) != 0U)
        {
            /* Data includes Packet RSSI */
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
            CheckSkipBytes(*ppEventData, *pDataLength, gCsRssiSize_c, bIncomplete,
                FLib_MemCpy(aRssi, *ppEventData, gCsRssiSize_c);
                rssiValue = (int8_t)(**ppEventData);
            );
#else
            CheckSkipBytes(*ppEventData, *pDataLength, gCsRssiSize_c, bIncomplete,
                FLib_MemCpy(aRssi, *ppEventData, gCsRssiSize_c);
            );
#endif /* gAppParseRssiInfo_d */
        }

        if ((filter & BIT5) != 0U)
        {
            /* Data includes ToA_ToD_Initiator/ToD_ToA_Reflector information */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint16_t), bIncomplete,
                FLib_MemCpy(&ts_diff_hci, *ppEventData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
            );
        }

        if ((filter & BIT6) != 0U)
        {
            /* Data includes Packet Antenna */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
                FLib_MemCpy(aAntenna, *ppEventData, gCsAntennaSize_c);
            );
        }

        /* Check if data includes Packet_PCT1, Packet_PCT2 information (3 octets each) */
        if (AppLocalization_GetRttSoundingSupport() == TRUE)
        {
            if ((filter & BIT7) != 0U)
            {
                /* Data includes Packet_PCT1 */
                CheckSkipBytesDoNothing(*ppEventData, *pDataLength, gPacket_PCTSize_c, bIncomplete);
            }

            if ((filter & BIT8) != 0U)
            {
                /* Data includes Packet_PCT2 */
                CheckSkipBytesDoNothing(*ppEventData, *pDataLength, gPacket_PCTSize_c, bIncomplete);
            }
        }
    } while(FALSE);

    /* In case no error occured, store temporary variables into the actual output */
    if (bIncomplete == FALSE)
    {
        /* ToF record */
        pRemoteData->step++;
        pDstAppBuffer->tof_data.nbSteps++;

        /* aNadm is zero-initialised; if BIT3 is absent the placeholder 0x00 is stored */
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aNadm, (int)gCsNadmSize_c);
        if ((filter & BIT4) != 0U)
        {
            /* Data includes Packet RSSI */
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, aRssi, (int)gCsRssiSize_c);
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
            if (rssiValue != gRssiNotAvailable_c)
            {
                /* Count RSSI if available */
                pDstAppBuffer->aRssiValue[pDstAppBuffer->rssiStepNo] = rssiValue;
                pDstAppBuffer->rssiStepNo++; /* TBD: this must be reverted in case of a failure */
            }
#endif /* gAppParseRssiInfo_d */
        }
        else
        {
            uint8_t rssi = 0U;
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, &rssi, (int)gCsRssiSize_c);
        }
        if ((filter & BIT5) != 0U)
        {
            int16_t temp1 = 0;
            uint32_t ts_diff = 0U;
            
            /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
            temp1 = ts_diff_hci/2;
            ts_diff = (uint32_t)(temp1); /* HCI reports half ns, application expects ns in Tof Buffer */
            ts_diff &= 0x00FFFFU;
            ts_diff |= (quality & 0x0FU) << gTimeStampDiffSize_c;
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)gCsTsSize_c);
        }
        else
        {
            uint32_t ts_diff = 0U;
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)gCsTsSize_c);
        }

        /* Store Packet_Antenna in tof buffer after TS.
         * aAntenna is zero-initialised, so if BIT6 is not set in the filter
         * (field absent from the RAS stream), 0x00 is stored as a placeholder.
         * This keeps the per-step record layout fixed at gCsTofTsSize_c bytes,
         * which is required by isp_tof_ranging_compute and the converter functions. */
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aAntenna, (int)gCsAntennaSize_c);
    }

    return bIncomplete;
}

/*! *******************************************************************************
 * \brief       Check if antenna path is enabled based on filter bits.
 *
 * \param[in]   antIdx              Antenna path index (0-3)
 * \param[in]   filter              Filter bits indicating enabled antenna paths
 *
 * \retval      bool_t              TRUE if antenna path is enabled, FALSE otherwise
 ******************************************************************************* */
static bool_t IsAntennaPathEnabled
(
    uint8_t antIdx,
    uint16_t filter
)
{
    bool_t isEnabled = FALSE;

    if ((antIdx == 0U) && ((filter & BIT5) != 0U))
    {
        isEnabled = TRUE;
    }
    else if ((antIdx == 1U) && ((filter & BIT6) != 0U))
    {
        isEnabled = TRUE;
    }
    else if ((antIdx == 2U) && ((filter & BIT7) != 0U))
    {
        isEnabled = TRUE;
    }
    else if ((antIdx == 3U) && ((filter & BIT8) != 0U))
    {
        isEnabled = TRUE;
    }
    else
    {
        /* Antenna path not enabled */
    }

    return isEnabled;
}

/*! *******************************************************************************
 * \brief       Parse antenna permutation index from Mode 2 event data.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data
 * \param[in,out] pDataLength       Pointer to remaining data length
 * \param[out]  pAntPermIndex       Pointer to store antenna permutation index
 * \param[in,out] pIncomplete       Pointer to incomplete flag
 *
 * \retval      void
 ******************************************************************************* */
static void ParseMode2AntennaPermutation
(
    uint16_t filter,
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    uint8_t *pAntPermIndex,
    bool_t *pIncomplete
)
{
    do
    {
        if ((filter & BIT2) != 0U)
        {
            /* Data includes Antenna Permutation Index */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), *pIncomplete,
                *pAntPermIndex = **ppEventData; /* Antenna_Permutation_Index */
            );
        }
    } while (FALSE);
}

/*! *******************************************************************************
 * \brief       Parse tone PCT and quality data for a single antenna path.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data
 * \param[in,out] pDataLength       Pointer to remaining data length
 * \param[in]   antIdx              Antenna path index
 * \param[out]  pIqDec              Pointer to store decoded IQ sample
 * \param[out]  pTqi                Pointer to store tone quality indicator
 * \param[in,out] pIncomplete       Pointer to incomplete flag
 *
 * \retval      void
 ******************************************************************************* */
static void ParseMode2ToneData
(
    uint16_t filter,
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    uint8_t antIdx,
    int32_t *pIqDec,
    uint8_t *pTqi,
    bool_t *pIncomplete
)
{
    do
    {
        if ((filter & BIT3) != 0U)
        {
            /* Data includes Tone_PCT information */
            CheckSkipBytes(*ppEventData, *pDataLength, gTone_PCTSize_c, *pIncomplete,
                uint32_t temp1 = ((uint32_t)(*ppEventData)[2])<<BIT4 | 
                                ((uint32_t)(*ppEventData)[1])<<BIT3 | 
                                ((uint32_t)(*ppEventData)[0]);
                int32_t iq = (int32_t)temp1;

                /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                                 ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                *pIqDec = (int32_t)temp2;
            );
        }

        if ((filter & BIT4) != 0U)
        {
            /* Data includes Tone_Quality_Indication */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), *pIncomplete,
                *pTqi = **ppEventData;
            );
        }
    } while (FALSE);
}

/*! *******************************************************************************
 * \brief       Parse tone data for all antenna paths in Mode 2.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data
 * \param[in,out] pDataLength       Pointer to remaining data length
 * \param[in]   pAntIndex           Pointer to antenna permutation array
 * \param[in]   numAntennaPaths     Number of antenna paths to process
 * \param[out]  iq_dec              Array to store decoded IQ samples
 * \param[out]  tqi                 Array to store tone quality indicators
 * \param[in,out] pIncomplete       Pointer to incomplete flag
 *
 * \retval      void
 ******************************************************************************* */
static void ParseMode2AllAntennaPaths
(
    uint16_t filter,
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    uint8_t const *pAntIndex,
    uint8_t numAntennaPaths,
    int32_t *iq_dec,
    uint8_t *tqi,
    bool_t *pIncomplete
)
{
    uint8_t antIdx = 0u;

    do
    {
        /* Num_Antenna_Path + 1 are reported by the firmware, but discard last one */
        /* Re-order per antenna path index */
        for (uint8_t idx = 0U; idx <= numAntennaPaths; idx++)
        {
            if (idx == numAntennaPaths)
            {
                /* Skip last IQ data (n_ap+1) */
                if (IsAntennaPathEnabled(antIdx, filter))
                {
                    if ((filter & BIT3) != 0U)
                    {
                        CheckSkipBytesDoNothing(*ppEventData, *pDataLength, gTone_PCTSize_c, *pIncomplete);
                    }
                    if ((filter & BIT4) != 0U)
                    {
                        CheckSkipBytesDoNothing(*ppEventData, *pDataLength, sizeof(uint8_t), *pIncomplete);
                    }
                }
                break;
            }

            antIdx = pAntIndex[idx];

            /* Check if the corresponding Antenna Path is enabled */
            if (IsAntennaPathEnabled(antIdx, filter))
            {
                ParseMode2ToneData(filter, ppEventData, pDataLength, antIdx,
                                  &iq_dec[antIdx], &tqi[antIdx], pIncomplete);

                if (*pIncomplete)
                {
                    break;
                }
            }
        }
    } while (FALSE);
}

/*! *******************************************************************************
 * \fn           static bool_t ParseMode2(uint16_t filter,
 *                                       uint8_t **ppEventData,
 *                                       uint32_t *pDataLength,
 *                                       csAppData_t *pDstAppBuffer,
 *                                       rasMeasurementData_t *pRemoteData)
 *
 * \brief       Parse CS Mode 2 data from received event data.
 *              Mode 2 contains tone record information with IQ samples and quality
 *              indicators for multiple antenna paths.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data. Updated to point
 *                                  after parsed data.
 * \param[in,out] pDataLength       Pointer to remaining data length. Updated after parsing.
 * \param[out]  pDstAppBuffer       Pointer to destination application buffer for storing
 *                                  parsed tone/IQ data
 * \param[in,out] pRemoteData       Pointer to measurement data structure containing
 *                                  parsing state information
 *
 * \retval       bool_t              TRUE if data is incomplete, FALSE if parsing succeeded
 ******************************************************************************* */
static bool_t ParseMode2
(
    uint16_t filter,
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    csAppData_t *pDstAppBuffer,
    rasMeasurementData_t *pRemoteData
)
{
    bool_t bIncomplete = FALSE;
    uint8_t antPermIndex = 0u;
    uint8_t const *pAntIndex = NULL;
    int32_t iq_dec[gMaxNumAntennaPaths_c] = {};
    uint8_t tqi[gMaxNumAntennaPaths_c] = {};

    /* First parse all bytes, based on filter bits, and store data in temporary variables */
    do
    {
        /* Parse antenna permutation index if present */
        ParseMode2AntennaPermutation(filter, ppEventData, pDataLength, 
                                    &antPermIndex, &bIncomplete);

        if (bIncomplete)
        {
            break;
        }

        pAntIndex = &gaAntPermNAp[antPermIndex][0];

        /* Parse tone data for all antenna paths */
        ParseMode2AllAntennaPaths(filter, ppEventData, pDataLength, pAntIndex,
                                 pRemoteData->numAntennaPaths, iq_dec, tqi, &bIncomplete);

    } while (FALSE);

    /* In case no error occurred, store temporary variables into the actual output */
    if (bIncomplete == FALSE)
    {
        pRemoteData->step++;

        for (uint8_t idx = 0U; idx < pDstAppBuffer->mciq_data.n_ap; idx++)
        {
            hciCsStoreBytesInIqBuffer(pDstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
            hciCsStoreBytesInIqBuffer(pDstAppBuffer, &tqi[idx], 1);
        }

        /* Tone record */
        pDstAppBuffer->mciq_data.nbSteps ++;
    }

    return bIncomplete;
}

/*! *******************************************************************************
 * \brief       Check if antenna path is enabled for Mode 3 based on filter bits.
 *
 * \param[in]   antIdx              Antenna path index (0-3)
 * \param[in]   filter              Filter bits indicating enabled antenna paths
 *
 * \retval      bool_t              TRUE if antenna path is enabled, FALSE otherwise
 ******************************************************************************* */
static bool_t IsMode3AntennaPathEnabled
(
    uint8_t antIdx,
    uint16_t filter
)
{
    bool_t isEnabled = FALSE;

    if ((antIdx == 0U) && ((filter & BIT12) != 0U))
    {
        isEnabled = TRUE;
    }
    else if ((antIdx == 1U) && ((filter & BIT13) != 0U))
    {
        isEnabled = TRUE;
    }
    else if ((antIdx == 2U) && ((filter & BIT14) != 0U))
    {
        isEnabled = TRUE;
    }
    else if ((antIdx == 3U) && ((filter & BIT15) != 0U))
    {
        isEnabled = TRUE;
    }
    else
    {
        /* Antenna path not enabled */
    }

    return isEnabled;
}

/*! *******************************************************************************
 * \brief       Parse packet quality, NADM, and RSSI from Mode 3 event.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data
 * \param[in,out] pDataLength       Pointer to remaining data length
 * \param[out]  pQuality            Pointer to store quality value
 * \param[out]  aNadm               Array to store NADM data
 * \param[out]  aRssi               Array to store RSSI data
 * \param[out]  pRssiValue          Pointer to store RSSI value (conditional)
 * \param[in,out] pIncomplete       Pointer to incomplete flag
 *
 * \retval      void
 ******************************************************************************* */
static void ParseMode3PacketMetrics
(
    uint16_t filter,
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    uint32_t *pQuality,
    uint8_t *aNadm,
    uint8_t *aRssi,
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
    int8_t *pRssiValue,
#endif
    bool_t *pIncomplete
)
{
    do
    {
        if ((filter & BIT2) != 0U)
        {
            /* Data includes Packet Quality*/
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), *pIncomplete,
                *pQuality = (uint32_t)(**ppEventData);
            );
        }

        if ((filter & BIT3) != 0U)
        {
            /* Data includes Packet NADM */
            CheckSkipBytes(*ppEventData, *pDataLength, gCsNadmSize_c, *pIncomplete,
                FLib_MemCpy(aNadm, *ppEventData, gCsNadmSize_c);
            );
        }

        if ((filter & BIT4) != 0U)
        {
            /* Data includes Packet RSSI */
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
            CheckSkipBytes(*ppEventData, *pDataLength, gCsRssiSize_c, *pIncomplete,
                FLib_MemCpy(aRssi, *ppEventData, gCsRssiSize_c);
                *pRssiValue = (int8_t)(**ppEventData);
            );
#else
            CheckSkipBytes(*ppEventData, *pDataLength, gCsRssiSize_c, *pIncomplete,
                FLib_MemCpy(aRssi, *ppEventData, gCsRssiSize_c);
            );
#endif /* gAppParseRssiInfo_d */
        }
    } while (FALSE);
}

/*! *******************************************************************************
 * \brief       Parse ToF and antenna data from Mode 3 event.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data
 * \param[in,out] pDataLength       Pointer to remaining data length
 * \param[out]  pTsDiffHci          Pointer to store timestamp difference
 * \param[out]  pAntPermIndex       Pointer to store antenna permutation index
 * \param[out]  pAntenna            Pointer to store Packet_Antenna value
 * \param[in,out] pIncomplete       Pointer to incomplete flag
 *
 * \retval      void
 ******************************************************************************* */
static void ParseMode3ToFAndAntenna
(
    uint16_t filter,
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    int16_t *pTsDiffHci,
    uint8_t *pAntPermIndex,
    uint8_t *pAntenna,
    bool_t *pIncomplete
)
{
    do
    {
        if ((filter & BIT5) != 0U)
        {
            /* Data includes ToA_ToD_Initiator/ToD_ToA_Reflector information */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint16_t), *pIncomplete,
                FLib_MemCpy(pTsDiffHci, *ppEventData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
            );
        }

        if ((filter & BIT6) != 0U)
        {
            /* Data includes Packet Antenna */
            CheckSkipBytes(*ppEventData, *pDataLength, 1U, *pIncomplete,
                *pAntenna = **ppEventData;
            );
        }

        /* Check if data includes Packet_PCT1, Packet_PCT2 information (3 octets each) */
        if (AppLocalization_GetRttSoundingSupport() == TRUE)
        {
            if ((filter & BIT7) != 0U)
            {
                /* Data includes Packet_PCT1 */
                CheckSkipBytesDoNothing(*ppEventData, *pDataLength, gPacket_PCTSize_c, *pIncomplete);
            }
            if ((filter & BIT8) != 0U)
            {
                /* Data includes Packet_PCT2 */
                CheckSkipBytesDoNothing(*ppEventData, *pDataLength, gPacket_PCTSize_c, *pIncomplete);
            }
        }

        *pAntPermIndex = 0;
        if ((filter & BIT9) != 0U)
        {
            /* Data includes Antenna Permutation Index */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), *pIncomplete,
                *pAntPermIndex = **ppEventData; /* Antenna_Permutation_Index */
            );
        }
    } while (FALSE);
}

/*! *******************************************************************************
 * \brief       Parse tone PCT and quality data for a single antenna path in Mode 3.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data
 * \param[in,out] pDataLength       Pointer to remaining data length
 * \param[in]   antIdx              Antenna path index
 * \param[out]  pIqDec              Pointer to store decoded IQ sample
 * \param[out]  pTqi                Pointer to store tone quality indicator
 * \param[in,out] pIncomplete       Pointer to incomplete flag
 *
 * \retval      void
 ******************************************************************************* */
static void ParseMode3ToneData
(
    uint16_t filter,
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    uint8_t antIdx,
    int32_t *pIqDec,
    uint8_t *pTqi,
    bool_t *pIncomplete
)
{
    do
    {
        if ((filter & BIT10) != 0U)
        {
            /* Data includes Tone_PCT information */
            CheckSkipBytes(*ppEventData, *pDataLength, gTone_PCTSize_c, *pIncomplete,
                uint32_t temp1 = ((uint32_t)(*ppEventData)[2])<<BIT4 | 
                                ((uint32_t)(*ppEventData)[1])<<BIT3 | 
                                ((uint32_t)(*ppEventData)[0]);
                int32_t iq = (int32_t)temp1;
                /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                                 ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                *pIqDec = (int32_t)temp2;
            );
        }

        if ((filter & BIT11) != 0U)
        {
            /* Data includes Tone_Quality_Indication */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), *pIncomplete,
                *pTqi = **ppEventData;
            );
        }
    } while (FALSE);
}

/*! *******************************************************************************
 * \brief       Parse tone data for all antenna paths in Mode 3.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data
 * \param[in,out] pDataLength       Pointer to remaining data length
 * \param[in]   pAntIndex           Pointer to antenna permutation array
 * \param[in]   numAntennaPaths     Number of antenna paths to process
 * \param[out]  iq_dec              Array to store decoded IQ samples
 * \param[out]  tqi                 Array to store tone quality indicators
 * \param[in,out] pIncomplete       Pointer to incomplete flag
 *
 * \retval      void
 ******************************************************************************* */
static void ParseMode3AllAntennaPaths
(
    uint16_t filter,
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    uint8_t const *pAntIndex,
    uint8_t numAntennaPaths,
    int32_t *iq_dec,
    uint8_t *tqi,
    bool_t *pIncomplete
)
{
    uint8_t antIdx = 0u;

    do
    {
        /* Num_Antenna_Path + 1 are reported by the firmware, but discard last one */
        /* Re-order per antenna path index */
        for (uint8_t idx = 0U; idx <= numAntennaPaths; idx++)
        {
            if (idx == numAntennaPaths)
            {
                /* Skip last IQ data (n_ap+1) */
                if (IsMode3AntennaPathEnabled(antIdx, filter))
                {
                    if ((filter & BIT10) != 0U)
                    {
                        CheckSkipBytesDoNothing(*ppEventData, *pDataLength, gTone_PCTSize_c, *pIncomplete);
                    }
                    if ((filter & BIT11) != 0U)
                    {
                        CheckSkipBytesDoNothing(*ppEventData, *pDataLength, sizeof(uint8_t), *pIncomplete);
                    }
                }
                break;
            }

            antIdx = pAntIndex[idx];

            /* Check if the corresponding Antenna Path is enabled */
            if (IsMode3AntennaPathEnabled(antIdx, filter))
            {
                ParseMode3ToneData(filter, ppEventData, pDataLength, antIdx,
                                  &iq_dec[antIdx], &tqi[antIdx], pIncomplete);
                
                if (*pIncomplete)
                {
                    break;
                }
            }
        }
    } while (FALSE);
}

/*! *******************************************************************************
 * \fn           static bool_t ParseMode3(uint16_t filter,
 *                                       uint8_t **ppEventData,
 *                                       uint32_t *pDataLength,
 *                                       csAppData_t *pDstAppBuffer,
 *                                       rasMeasurementData_t *pRemoteData)
 *
 * \brief       Parse CS Mode 3 data from received event data.
 *              Mode 3 contains both Time-of-Flight (ToF) and tone record information,
 *              combining data from Mode 1 and Mode 2.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data. Updated to point
 *                                  after parsed data.
 * \param[in,out] pDataLength       Pointer to remaining data length. Updated after parsing.
 * \param[out]  pDstAppBuffer       Pointer to destination application buffer for storing
 *                                  parsed ToF and tone/IQ data
 * \param[in,out] pRemoteData       Pointer to measurement data structure containing
 *                                  parsing state information
 *
 * \retval       bool_t              TRUE if data is incomplete, FALSE if parsing succeeded
 ******************************************************************************* */
static bool_t ParseMode3
(
    uint16_t filter,
    uint8_t **ppEventData,
    uint32_t *pDataLength,
    csAppData_t *pDstAppBuffer,
    rasMeasurementData_t *pRemoteData
)
{
    bool_t bIncomplete = FALSE;
    uint8_t antPermIndex = 0u;
    uint8_t const *pAntIndex;
    int32_t iq_dec[gMaxNumAntennaPaths_c] = {};
    uint8_t tqi[gMaxNumAntennaPaths_c] = {};
    uint32_t quality = 0U;
    uint8_t aRssi[gCsRssiSize_c] = {};
    uint8_t aNadm[gCsNadmSize_c] = {};
    uint8_t aAntenna[gCsAntennaSize_c] = {};
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
    int8_t rssiValue = 0;
#endif
    int16_t ts_diff_hci = 0;

    /* First parse all bytes, based on filter bits, and store data in temporary variables */
    do
    {
        /* ToF+Tone record */
        /* Parse packet quality, NADM, and RSSI */
        ParseMode3PacketMetrics(filter, ppEventData, pDataLength, &quality, aNadm, aRssi,
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
                               &rssiValue,
#endif
                               &bIncomplete);

        if (bIncomplete)
        {
            break;
        }

        /* Parse ToF and antenna information */
        ParseMode3ToFAndAntenna(filter, ppEventData, pDataLength, &ts_diff_hci, 
                               &antPermIndex, aAntenna, &bIncomplete);

        if (bIncomplete)
        {
            break;
        }

        pAntIndex = &gaAntPermNAp[antPermIndex][0];

        /* Parse tone data for all antenna paths */
        ParseMode3AllAntennaPaths(filter, ppEventData, pDataLength, pAntIndex,
                                 pRemoteData->numAntennaPaths, iq_dec, tqi, &bIncomplete);

    } while (FALSE);

    /* In case no error occurred, store temporary variables into the actual output */
    if (bIncomplete == FALSE)
    {
        /* ToF+Tone record */
        pRemoteData->step++;

        /* aNadm is zero-initialised; if BIT3 is absent the placeholder 0x00 is stored */
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aNadm, (int)gCsNadmSize_c);

        if ((filter & BIT4) != 0U)
        {
            /* Data includes Packet RSSI */
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, aRssi, (int)gCsRssiSize_c);
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
            if (rssiValue != gRssiNotAvailable_c)
            {
                /* Count RSSI if available */
                pDstAppBuffer->aRssiValue[pDstAppBuffer->rssiStepNo] = rssiValue;
                pDstAppBuffer->rssiStepNo++;
            }
#endif /* gAppParseRssiInfo_d */
        }
        else
        {
            uint8_t rssi = 0U;
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, &rssi, (int)gCsRssiSize_c);
        }

        if ((filter & BIT5) != 0U)
        {
            int16_t temp1 = 0;
            uint32_t ts_diff = 0U;
            /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
            temp1 = ts_diff_hci/2;
            ts_diff = (uint32_t)(temp1); /* HCI reports half ns, application expects ns in Tof Buffer */
            ts_diff &= 0x00FFFFU;
            ts_diff |= (quality & 0x0FU) << gTimeStampDiffSize_c;
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)gCsTsSize_c);
        }
        else
        {
            uint32_t ts_diff = 0U;
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)gCsTsSize_c);
        }

        /* Store Packet_Antenna in tof buffer after TS.
         * aAntenna is zero-initialised, so if BIT6 is not set in the filter
         * (field absent from the RAS stream), 0x00 is stored as a placeholder.
         * This keeps the per-step record layout fixed at gCsTofTsSize_c bytes. */
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aAntenna, (int)gCsAntennaSize_c);

        for (uint8_t idx = 0U; idx < pDstAppBuffer->mciq_data.n_ap; idx++)
        {
            hciCsStoreBytesInIqBuffer(pDstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
            hciCsStoreBytesInIqBuffer(pDstAppBuffer, &tqi[idx], 1);
        }

        pDstAppBuffer->mciq_data.nbSteps++;
        pDstAppBuffer->tof_data.nbSteps++;
    }

    return bIncomplete;
}
#endif /* defined (gRasRREQ_d) && (gRasRREQ_d == 1) */

#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1)
/*! *********************************************************************************
 *\fn           static bool_t ParseMode0(uint16_t filter, 
 *                                       uint8_t **ppEventData, 
 *                                       uint32_t *pDataLength,
 *                                       csAppData_t *pDstAppBuffer,
 *                                       rasMeasurementData_t *pRemoteData)  
 *
 * \brief       Parse CS Mode 0 data from received event data.
 *              Mode 0 contains only metadata without ToF or tone information.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data. Updated to point
 *                                  after parsed data.
 * \param[in,out] pDataLength       Pointer to remaining data length. Updated after parsing.
 * \param[out]  pDstAppBuffer       Pointer to destination application buffer for storing
 *                                  parsed metadata
 * \param[in,out] pRemoteData       Pointer to measurement data structure containing
 *                                  parsing state information
 * 
 *\retval       bool_t              TRUE if data is incomplete, FALSE if parsing succeeded
 ********************************************************************************** */
static bool_t ParseMode0
(
    uint16_t filter, 
    uint8_t **ppEventData, 
    uint32_t *pDataLength,
    csAppData_t *pDstAppBuffer,
    rasMeasurementData_t *pRemoteData
)
{
    bool_t bIncomplete = FALSE;

    assert(pDstAppBuffer->mode0BufferOffset < (gMaxNumCsStepsMode0_c * gMaxNumCsSubevents_c));

    do
    {
        /* Data includes Packet Quality */
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
            pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].quality = **ppEventData;
        );

        /* Data includes Packet RSSI */
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
            pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].rssi = (int8_t)**ppEventData;
        );

        /* Data includes Packet Antenna */
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
            pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].antenna = **ppEventData;
        );

        if (mGlobalRangeSettings.role == gCsRoleReflector_c)
        {
            /* Data includes Measured_Freq_Offset information */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint16_t), bIncomplete,
                pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].measuredFreqOffset = Utils_ExtractTwoByteValue(*ppEventData);
            );
        }
        else
        {
            pDstAppBuffer->mode0Buffer[pDstAppBuffer->mode0BufferOffset].measuredFreqOffset = 0U;
        }
    } while(FALSE);

    pDstAppBuffer->mode0BufferOffset++;
    pRemoteData->step++;
    
    return bIncomplete;
}

/*! *********************************************************************************
 *\fn           static bool_t ParseMode1(uint16_t filter, 
 *                                       uint8_t **ppEventData, 
 *                                       uint32_t *pDataLength, 
 *                                       csAppData_t *pDstAppBuffer, 
 *                                       rasMeasurementData_t *pRemoteData)
 *
 * \brief       Parse CS Mode 1 data from received event data.
 *              Mode 1 contains Time-of-Flight (ToF) record information.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data. Updated to point
 *                                  after parsed data.
 * \param[in,out] pDataLength       Pointer to remaining data length. Updated after parsing.
 * \param[out]  pDstAppBuffer       Pointer to destination application buffer for storing
 *                                  parsed ToF data
 * \param[in,out] pRemoteData       Pointer to measurement data structure containing
 *                                  parsing state information
 *
 *\retval       bool_t              TRUE if data is incomplete, FALSE if parsing succeeded
 ********************************************************************************** */
static bool_t ParseMode1
(
    uint16_t filter, 
    uint8_t **ppEventData, 
    uint32_t *pDataLength, 
    csAppData_t *pDstAppBuffer, 
    rasMeasurementData_t *pRemoteData
)
{
    uint32_t quality = 0U;
    uint8_t aNadm[gCsNadmSize_c] = {};
    uint8_t aRssi[gCsRssiSize_c] = {};
    uint8_t aAntenna[gCsAntennaSize_c] = {};
    int16_t ts_diff_hci = 0;
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
    int rssiValue = 0;
#endif /* gAppParseRssiInfo_d */
    
    bool_t bIncomplete = FALSE;
    
    /* First parse all bytes, based on filter bits, and store data in temporary variables */
    do
    {
        /* Data includes Packet Quality*/
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete, 
            quality = (uint32_t)**ppEventData;
        );

        /* Data includes Packet NADM */
        CheckSkipBytes(*ppEventData, *pDataLength, gCsNadmSize_c, bIncomplete,
            FLib_MemCpy(aNadm, *ppEventData, gCsNadmSize_c);
        ); /* Packet_NADM */

        /* Data includes Packet RSSI */
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
        CheckSkipBytes(*ppEventData, *pDataLength, gCsRssiSize_c, bIncomplete,
            FLib_MemCpy(aRssi, *ppEventData, gCsRssiSize_c);
            rssiValue = (int8_t)(**ppEventData);
        );
#else
        CheckSkipBytes(*ppEventData, *pDataLength, gCsRssiSize_c, bIncomplete,
            FLib_MemCpy(aRssi, *ppEventData, gCsRssiSize_c);
        );
#endif /* gAppParseRssiInfo_d */

        /* Data includes ToA_ToD_Initiator/ToD_ToA_Reflector information */
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint16_t), bIncomplete,
            FLib_MemCpy(&ts_diff_hci, *ppEventData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
        );

        /* Data includes Packet Antenna */
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
            FLib_MemCpy(aAntenna, *ppEventData, gCsAntennaSize_c);
        );
    } while(FALSE);

    /* In case no error occured, store temporary variables into the actual output */
    if (bIncomplete == FALSE)
    {
        /* ToF record */
        pRemoteData->step++;
        pDstAppBuffer->tof_data.nbSteps++;

        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aNadm, (int)gCsNadmSize_c);
        
        /* Data includes Packet RSSI */
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aRssi, (int)gCsRssiSize_c);
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
        if (rssiValue != gRssiNotAvailable_c)
        {
            /* Count RSSI if available */
            pDstAppBuffer->aRssiValue[pDstAppBuffer->rssiStepNo] = rssiValue;
            pDstAppBuffer->rssiStepNo++; /* TBD: this must be reverted in case of a failure */
        }
#endif /* gAppParseRssiInfo_d */
        
        int16_t temp1 = 0;
        uint32_t ts_diff = 0U;
        
        /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
        temp1 = ts_diff_hci/2;
        ts_diff = (uint32_t)(temp1); /* HCI reports half ns, application expects ns in Tof Buffer */
        ts_diff &= 0x00FFFFU;
        ts_diff |= (quality & 0x0FU) << gTimeStampDiffSize_c;
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)gCsTsSize_c);

        /* Store Packet_Antenna in tof buffer after TS */
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aAntenna, (int)gCsAntennaSize_c);
    }

    return bIncomplete;
}

/*! *********************************************************************************
 *\fn           static bool_t ParseMode2(uint16_t filter, 
 *                                       uint8_t **ppEventData, 
 *                                       uint32_t *pDataLength, 
 *                                       csAppData_t *pDstAppBuffer, 
 *                                       rasMeasurementData_t *pRemoteData)
 *
 * \brief       Parse CS Mode 2 data from received event data.
 *              Mode 2 contains tone record information with IQ samples and quality
 *              indicators for multiple antenna paths.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data. Updated to point
 *                                  after parsed data.
 * \param[in,out] pDataLength       Pointer to remaining data length. Updated after parsing.
 * \param[out]  pDstAppBuffer       Pointer to destination application buffer for storing
 *                                  parsed tone/IQ data
 * \param[in,out] pRemoteData       Pointer to measurement data structure containing
 *                                  parsing state information
 *
 *\retval       bool_t              TRUE if data is incomplete, FALSE if parsing succeeded
 ********************************************************************************** */
static bool_t ParseMode2
(
    uint16_t filter, 
    uint8_t **ppEventData, 
    uint32_t *pDataLength, 
    csAppData_t *pDstAppBuffer, 
    rasMeasurementData_t *pRemoteData
)
{
    bool_t bIncomplete = FALSE;
    
    uint8_t antPermIndex = 0u;
    uint8_t antIdx = 0u;
    uint8_t const *pAntIndex = NULL;
    int32_t iq_dec[gMaxNumAntennaPaths_c] = {};
    uint8_t tqi[gMaxNumAntennaPaths_c] = {};
    uint8_t quality = 0U;
    
    /* First parse all bytes, based on filter bits, and store data in temporary variables */
    do
    {
        /* Data includes Antenna Permutation Index */
        (void)GetItem(&antPermIndex);
        pAntIndex = &gaAntPermNAp[antPermIndex][0];
        
        /* Data includes Packet Quality*/
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete, 
            quality = **ppEventData;
        );

        /* Data includes Tone_PCT information */
        for (uint8_t idx = 0U; idx < pRemoteData->numAntennaPaths; idx++)
        {
            antIdx = pAntIndex[idx];
            
            /* Data includes Tone_PCT information */
            CheckSkipBytes(*ppEventData, *pDataLength, gTone_PCTSize_c, bIncomplete,
                uint32_t temp1 = ((uint32_t)(*ppEventData)[2])<<BIT4 | ((uint32_t)(*ppEventData)[1])<<BIT3
                      | ((uint32_t)(*ppEventData)[0]);
                int32_t iq = (int32_t)temp1;
                
                /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                    ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                iq_dec[antIdx] = (int32_t)temp2;
                
                /* Extract quality information */
                tqi[antIdx] = (uint8_t)(quality << (2u*antIdx));
                tqi[antIdx] &= 0x03U; /* keep only the first 2 bits */
            );
        }
    } while(FALSE);
    
    /* In case no error occured, store temporary variables into the actual output */
    if (bIncomplete == FALSE)
    {
        pRemoteData->step++;

        for (uint8_t idx = 0U; idx < pDstAppBuffer->mciq_data.n_ap; idx++)
        {
            hciCsStoreBytesInIqBuffer(pDstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
            hciCsStoreBytesInIqBuffer(pDstAppBuffer, &tqi[idx], 1);
        }

        /* Tone record */
        pDstAppBuffer->mciq_data.nbSteps ++;
    }

    return bIncomplete;
}

/*! *********************************************************************************
 *\fn           static bool_t ParseMode3(uint16_t filter, 
 *                                       uint8_t **ppEventData, 
 *                                       uint32_t *pDataLength, 
 *                                       csAppData_t *pDstAppBuffer, 
 *                                       rasMeasurementData_t *pRemoteData)
 *
 * \brief       Parse CS Mode 3 data from received event data.
 *              Mode 3 contains both Time-of-Flight (ToF) and tone record information,
 *              combining data from Mode 1 and Mode 2.
 *
 * \param[in]   filter              Filter bits indicating which fields are present
 * \param[in,out] ppEventData       Pointer to pointer of event data. Updated to point
 *                                  after parsed data.
 * \param[in,out] pDataLength       Pointer to remaining data length. Updated after parsing.
 * \param[out]  pDstAppBuffer       Pointer to destination application buffer for storing
 *                                  parsed ToF and tone/IQ data
 * \param[in,out] pRemoteData       Pointer to measurement data structure containing
 *                                  parsing state information
 *
 *\retval       bool_t              TRUE if data is incomplete, FALSE if parsing succeeded
 ********************************************************************************** */
static bool_t ParseMode3
(
    uint16_t filter, 
    uint8_t **ppEventData, 
    uint32_t *pDataLength, 
    csAppData_t *pDstAppBuffer,
    rasMeasurementData_t *pRemoteData
)
{
    bool_t bIncomplete = FALSE;
    
    uint8_t antPermIndex = 0u;
    uint8_t antIdx = 0u;
    uint8_t const *pAntIndex;
    int32_t iq_dec[gMaxNumAntennaPaths_c] = {};
    uint8_t tqi[gMaxNumAntennaPaths_c] = {};
    uint32_t quality = 0u;
    uint8_t pctQuality = 0u;
    uint8_t aRssi[gCsRssiSize_c] = {};
    uint8_t aNadm[gCsNadmSize_c] = {};
    uint8_t aAntenna[gCsAntennaSize_c] = {};
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
    int8_t rssiValue = 0;
#endif
    int16_t ts_diff_hci = 0;
    int16_t temp = 0;
    uint32_t ts_diff = 0U;

    /* First parse all bytes, based on filter bits, and store data in temporary variables */
    do
    {
        /* ToF+Tone record */
        /* Data includes Packet Quality*/
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
            quality = (uint32_t)(**ppEventData);
        );

        /* Data includes Packet NADM */
        CheckSkipBytes(*ppEventData, *pDataLength, gCsNadmSize_c, bIncomplete,
            FLib_MemCpy(aNadm, *ppEventData, gCsNadmSize_c);
        ); /* Packet_NADM */

        /* Data includes Packet RSSI */
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
        CheckSkipBytes(*ppEventData, *pDataLength, gCsRssiSize_c, bIncomplete,
            FLib_MemCpy(aRssi, *ppEventData, gCsRssiSize_c);
            rssiValue = (int8_t)(**ppEventData);
        );
#else
        CheckSkipBytes(*ppEventData, *pDataLength, gCsRssiSize_c, bIncomplete,
            FLib_MemCpy(aRssi, *ppEventData, gCsRssiSize_c);
        );    
#endif /* gAppParseRssiInfo_d */
        

        /* Data includes ToA_ToD_Initiator/ToD_ToA_Reflector information */
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint16_t), bIncomplete,
            FLib_MemCpy(&ts_diff_hci, *ppEventData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
        );

        /* Data includes Packet Antenna */
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
            FLib_MemCpy(aAntenna, *ppEventData, gCsAntennaSize_c);
        );

        /* Data includes Antenna Permutation Index */
        (void)GetItem(&antPermIndex);
        pAntIndex = &gaAntPermNAp[antPermIndex][0];

        /* Extract quality - 2 bits per antenna path, up to 4 antenna paths, ordered*/
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
            pctQuality = (uint8_t)(**(uint8_t**)ppEventData);
        );

        /* Re-order per antenna path index */
        for (uint8_t idx = 0U; idx < pRemoteData->numAntennaPaths; idx++)
        {
            antIdx = pAntIndex[idx];

            /* Data includes Tone_PCT information */
            CheckSkipBytes(*ppEventData, *pDataLength, gTone_PCTSize_c, bIncomplete,
                uint32_t temp1 = ((uint32_t)(*ppEventData)[2])<<BIT4 | ((uint32_t)(*ppEventData)[1])<<BIT3 | ((uint32_t)(*ppEventData)[0]);
                int32_t iq = (int32_t)temp1;

                /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                                  ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                iq_dec[antIdx] = (int32_t)temp2;
                
                /* Extract quality information */
                tqi[antIdx] = (uint8_t)(pctQuality << (2u*antIdx));
                tqi[antIdx] &= 0x03U; /* keep only the first 2 bits */
             );
        }
    } while (FALSE);
    
    /* In case no error occured, store temporary variables into the actual output */
    if (bIncomplete ==  FALSE)
    {
        /* ToF+Tone record */
        pRemoteData->step++;

        /* Data includes Packet NADM */
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aNadm, (int)gCsNadmSize_c);

        /* Data includes Packet RSSI */
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aRssi, (int)gCsRssiSize_c);
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
        if (rssiValue != gRssiNotAvailable_c)
        {
            /* Count RSSI if available */
            pDstAppBuffer->aRssiValue[pDstAppBuffer->rssiStepNo] = rssiValue;
            pDstAppBuffer->rssiStepNo++;
        }
#endif /* gAppParseRssiInfo_d */
        
        /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
        temp = ts_diff_hci/2;
        ts_diff = (uint32_t)(temp); /* HCI reports half ns, application expects ns in Tof Buffer */
        ts_diff &= 0x00FFFFU;
        ts_diff |= (quality & 0x0FU) << gTimeStampDiffSize_c;
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)gCsTsSize_c);

        /* Store Packet_Antenna in tof buffer after TS.
         * aAntenna is zero-initialised, so if BIT6 is not set in the filter
         * (field absent from the RAS stream), 0x00 is stored as a placeholder.
         * This keeps the per-step record layout fixed at gCsTofTsSize_c bytes. */
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aAntenna, (int)gCsAntennaSize_c);

        for (uint8_t idx = 0U; idx < pDstAppBuffer->mciq_data.n_ap; idx++)
        {
            hciCsStoreBytesInIqBuffer(pDstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
            hciCsStoreBytesInIqBuffer(pDstAppBuffer, &tqi[idx], 1);
        }

        pDstAppBuffer->mciq_data.nbSteps++;
        pDstAppBuffer->tof_data.nbSteps++;
    }

    return bIncomplete;
}
#endif /* defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1) */

#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1)
/*! *********************************************************************************
 *\fn           void AddItem(uint8_t item)
 *
 * \brief       Add a new item in the list
 *
 * \param[out]  item        Value to be added
 *
 *\retval       none
 ********************************************************************************** */
static void AddItem(uint8_t item)
{
    /* Check if buffer is full */
    if (mCount >= ANT_PERM_INDEX_LIST_LEN)
    {
        /* In this case the ANT_PERM_INDEX_LIST_LEN must be increased */
        panic(0, 0, 0, 0);
    }

    aPermIndexList[mProducer] = item;
    mProducer = (mProducer + 1U) % ANT_PERM_INDEX_LIST_LEN;
    mCount++;
}

/*! *********************************************************************************
 *\fn           bool_t GetItem(uint8_t *pItem)
 *
 * \brief       Retrieve an element from the list
 *
 * \param[out]  pItem       Pointer to area where to put the item
 *
 *\retval       bool_t      TRUE if the item was fetched, FALSE in case something went wrong
 ********************************************************************************** */
static bool_t GetItem(uint8_t *pItem)
{
    bool_t result = TRUE;

    /* Check if buffer is empty */
    if (mCount == 0U)
    {
        result = FALSE;  /* Buffer empty, nothing to get */
    }
    else
    {    
        *pItem = aPermIndexList[mConsumer];
        mConsumer = (mConsumer + 1U) % ANT_PERM_INDEX_LIST_LEN;
        mCount--;
    }

    return result;
}
#endif /* defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1) */
