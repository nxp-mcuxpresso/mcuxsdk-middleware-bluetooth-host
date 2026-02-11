/*! *********************************************************************************
* Copyright 2025 - 2026 NXP
*
* \file app_localization_utils.c
*
* This is a source file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************/
#include "EmbeddedTypes.h"
#include "app_localization.h"
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
    if (remainingLen < fetchDataLen)\
    {\
        endVar = TRUE;\
        break;\
    }\
    else\
    {\
        remainingLen -= fetchDataLen;\
        pos += fetchDataLen;\
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
    if (remainingLen < fetchDataLen)\
    {\
        endVar = TRUE;\
        break;\
    }\
    else\
    {\
        x;\
        remainingLen -= fetchDataLen;\
        pos += fetchDataLen;\
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
    uint32_t *pDdataLength, 
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

/************************************************************************************
*************************************************************************************
* Private functions
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
    int totalStepCounter = 0;
#ifdef SKIP_MAIN_MODES_REPET
    uint32_t parsedMainModeNb = CS_MAIN_MODE_REPEAT_MAX;
#endif
    csAppData_t *dstAppBuffer = (csAppData_t *)pLocalData->pData;
    bool_t bIncomplete = FALSE;

    dstAppBuffer->mciq_data.n_ap = pLocalData->numAntennaPaths;

    dstAppBuffer->csData.subevt_nb = 0U;

    for (;pLocalData->crtStep < pLocalData->totalNumSteps; pLocalData->crtStep++)
    {
        bool_t tofPresent = FALSE;
        bool_t mciqPresent = FALSE;
        uint8_t mode = gCsStepMode0_c;
        uint8_t channel = 0U;
        uint8_t stepDataLength = 0U;

        CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete, 
            mode = *pEventData);

        CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete, 
            channel = *pEventData);

        CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete, 
            stepDataLength = *pEventData);

        dstAppBuffer->csData.channelMap[pLocalData->step] = channel;
        dstAppBuffer->csData.modeMap[pLocalData->step] = mode;

        /* Check if there is reported step data - skip to next step if not */
        if (stepDataLength == 0U)
        {
            pLocalData->step++;
            continue;
        }

        if (mode == gCsStepMode0_c)
        {
            CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete,
                dstAppBuffer->csData.mode0Data[pLocalData->crtStep].quality = *pEventData);
            CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete,
                dstAppBuffer->csData.mode0Data[pLocalData->crtStep].rssi = (int8_t)*pEventData);
            CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete,
                dstAppBuffer->csData.mode0Data[pLocalData->crtStep].antenna = *pEventData);
            if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
            {
                CheckSkipBytes(pEventData, dataSize, sizeof(uint16_t), bIncomplete,
                    dstAppBuffer->csData.mode0Data[pLocalData->crtStep].measuredFreqOffset = Utils_ExtractTwoByteValue(pEventData));
            }
            else
            {
                dstAppBuffer->csData.mode0Data[pLocalData->crtStep].measuredFreqOffset = 0U;
            }
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
            case gCsStepMode1_c:
            {
                /* ToF record */
                tofPresent = TRUE;
            }
            break;

            case gCsStepMode2_c:
            {
                /* Tone record */
                mciqPresent = TRUE;
            }
            break;

            case gCsStepMode3_c:
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
            uint32_t ts_diff = 0;
            int16_t ts_diff_hci = 0;
            int16_t temp1 = 0;
            /* ToF record */
            uint32_t quality = 0;

            CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete, 
                quality = (uint32_t)(*pEventData)); /* Packet_AA_Quality */
            
            CheckSkipBytes(pEventData, dataSize, CS_NADM_SIZE, bIncomplete, 
                hciCsStoreBytesInTofBuffer(dstAppBuffer, pEventData, (int)CS_NADM_SIZE); /* Packet_NADM */
            );

            CheckSkipBytes(pEventData, dataSize, CS_RSSI_SIZE, bIncomplete, 
                hciCsStoreBytesInTofBuffer(dstAppBuffer, pEventData, (int)CS_RSSI_SIZE); /* Packet_RSSI */
            );

#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
            if ((int8_t)(*pEventData) != gRssiNotAvailable_c)
            {
                /* Count RSSI if available */
                dstAppBuffer->aRssiValue[dstAppBuffer->rssiStepNo] = (int8_t)(*pEventData);
                dstAppBuffer->rssiStepNo++;
            }
#endif /* gAppParseRssiInfo_d */

            CheckSkipBytes(pEventData, dataSize, sizeof(uint16_t), bIncomplete, 
                FLib_MemCpy(&ts_diff_hci, pEventData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
                /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
                temp1 = ts_diff_hci/2;
                ts_diff = (uint32_t)(temp1); /* HCI reports half ns, application expects ns in Tof Buffer */
                ts_diff &= 0x00FFFFU;
                ts_diff |= (quality&0x0FU)<<gTimeStampDiffSize_c;
                hciCsStoreBytesInTofBuffer(dstAppBuffer, (uint8_t *)&ts_diff, (int)CS_TS_SIZE);
            );

            CheckSkipBytesDoNothing(pEventData, dataSize, 1U, bIncomplete); /* Packet_Antenna, ignored */
            
            dstAppBuffer->tof_data.nbSteps ++;
        }

        if (mciqPresent)
        {
            /* Tone record */
            uint8_t antPermIndex = 0U;
            const uint8_t *antIndex_p = NULL;
            int32_t iq_dec[ISP_MAX_NO_ANTENNAS];
            uint8_t tqi[ISP_MAX_NO_ANTENNAS];

            CheckSkipBytes(pEventData, dataSize, 1U, bIncomplete,
                antPermIndex = *pEventData; /* Antenna_Permutation_Index */
            );

#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1)
            AddItem(antPermIndex);
#endif

            antIndex_p = &maAntPermNAp[antPermIndex][0];

            /* Num_Antenna_Path + 1 are reported by the firmware, but discard last one */
            /* Re-order per antenna path index */
            for (uint8_t idx = 0U; idx < dstAppBuffer->mciq_data.n_ap; idx++)
            {
                int antIdx = (int)antIndex_p[idx];

                uint32_t temp1 = (((uint32_t)pEventData[2])<<BIT4) | (((uint32_t)pEventData[1])<<BIT3)
                                 | ((uint32_t)pEventData[0]);
                int32_t iq = (int32_t)temp1;

                CheckSkipBytes(pEventData, dataSize, gTone_PCTSize_c, bIncomplete,
                    /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                    uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                                      ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                    iq_dec[antIdx] = (int32_t)temp2;
                );

                CheckSkipBytes(pEventData, dataSize, sizeof(uint8_t), bIncomplete,
                    tqi[antIdx] = *pEventData;
                );
            }

            if (bIncomplete == FALSE)
            {
                for (uint8_t idx = 0U; idx < dstAppBuffer->mciq_data.n_ap; idx++)
                {
                    hciCsStoreBytesInIqBuffer(dstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
                    hciCsStoreBytesInIqBuffer(dstAppBuffer, &tqi[idx], 1);
                }

                /* Skip last IQ data (n_ap+1) */
                CheckSkipBytesDoNothing(pEventData, dataSize, gTone_PCTSize_c + sizeof(uint8_t), bIncomplete);

                dstAppBuffer->mciq_data.nbSteps ++;
            }
        }

        pLocalData->step ++;
    }

    /* All data was parsed */
    if (pLocalData->crtStep >= pLocalData->totalNumSteps)
    {
        /* Populate additional fields in dstAppBuffer */

        /* Total number of steps */
        dstAppBuffer->csData.step_nb = (uint16_t)pLocalData->step;

        /* Start ACL count */
        dstAppBuffer->csData.startAclCnt =
                pLocalData->aSubEventData[dstAppBuffer->csData.subevt_nb].subevtHeader.startACLConnEvent;

        /* For every subevent */
        for (uint8_t index = 0U; index <= pLocalData->subeventIndex; index++)
        {
            /* The stop index is the total number of previous steps plus the current subevent's steps */
            dstAppBuffer->csData.subevtStopIdxLocal[index] =
                (uint8_t)totalStepCounter + pLocalData->aSubEventData[index].subevtHeader.numStepsReported;

            /* Delta regarding ACL counter of first subevent */
            dstAppBuffer->csData.subevtConnEvent[index] =
                (uint8_t)(pLocalData->aSubEventData[index].subevtHeader.startACLConnEvent - dstAppBuffer->csData.startAclCnt);

            /* Count handled steps */
            totalStepCounter += (int)pLocalData->aSubEventData[index].subevtHeader.numStepsReported;

            /* Save the reference power level in subevtRefPowerLevelInit - will be switched to the proper role by the caller */
            dstAppBuffer->csData.subevtRefPowerLevelInit[index] = pLocalData->aSubEventData[index].subevtHeader.referencePowerLevel;
        }

        /* Total number of subevents */
        dstAppBuffer->csData.subevt_nb =  pLocalData->subeventIndex + 1U;
    }
}

void* AppLocalizationAlgo_AllocData()
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
*\fn        void AppLocalizationAlgo_UncompressRemoteResponse(uint8_t* pEventData,
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
    uint8_t* pEventData,
    uint32_t dataLength,
    rasMeasurementData_t *pRemoteData,
    bool_t lastSegment
)
{
    uint16_t totalStepCounter = 0U;
    csAppData_t *pDstAppBuffer = (csAppData_t*)pRemoteData->pData;

    uint8_t mode;
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
            MEM_BufferFree(pRemoteData->pRemaining);
            pRemoteData->pRemaining = NULL;
            pRemoteData->remainingLen = 0;
        }
        else
        {
            panic(0, 0, 0, 0);
        }
    }

    /* Loop through all of the received bytes */
    while ((dataLength > 0) && (bIncomplete == FALSE))
    {
        uint16_t filter = 0U;

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
                        RasClient_ParseReceivedSubeventHeader(pRemoteData->deviceId, pEventData));
            } while(0);
            
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
            do
            {
                CheckSkipBytes(pEventData, dataLength, sizeof(uint8_t), bIncomplete, 
                    mode = *pEventData);
            } while(0);
            
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

                    switch(mode)
                    {
                        case gCsStepMode0_c:
                        {
                            bIncomplete = ParseMode0(filter, &pEventData, &dataLength, pDstAppBuffer, pRemoteData);
                        }
                        break;

                        case gCsStepMode1_c:
                        {
                            bIncomplete = ParseMode1(filter, &pEventData, &dataLength, pDstAppBuffer, pRemoteData);
                        }
                        break;

                        case gCsStepMode2_c:
                        {
                            bIncomplete = ParseMode2(filter, &pEventData, &dataLength, pDstAppBuffer, pRemoteData);
                        }
                        break;

                        case gCsStepMode3_c:
                        {
                            bIncomplete = ParseMode3(filter, &pEventData, &dataLength, pDstAppBuffer, pRemoteData);
                        }
                        break;

                        default:
                        {
                            /* mode not yet implemented, skip data */
                        }
                        break;
                    }
                }
            }
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
        MEM_BufferFree(pTemp);
    }

    /* Populate additional fields in pDstAppBuffer */
    if (lastSegment == TRUE)
    {
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
    csAppData_t *pDstAppBuffer = (csAppData_t*)pRemoteData->pData;
    
    uint8_t mode = 0U;
    bool_t bIncomplete = FALSE;
    uint8_t *pLastOk = NULL;
    uint8_t *pTemp = NULL;
    uint8_t parsedSteps = 0;

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
            MEM_BufferFree(pRemoteData->pRemaining);
            pRemoteData->pRemaining = NULL;
            pRemoteData->remainingLen = 0;
        }
        else
        {
            panic(0, 0,0, 0);
        }
    }
    
    /* Loop through all of the received bytes */
    while ((dataLength > 0) && (bIncomplete == FALSE) && (parsedSteps < maxSteps))
    {
        /* Keep a pointer to the end of the last completed parse */
        pLastOk = pEventData;
        pRemoteData->remainingLen = dataLength;

        CheckSkipBytes(pEventData, dataLength, sizeof(uint8_t), bIncomplete, 
            mode = *pEventData);

        /* Make sure the mode is valid and that local and remote mode match */
        assert(mode <= gCsStepMode3_c);

        pDstAppBuffer->csData.modeMap[pRemoteData->step] = mode;

        switch(mode)
        {
            case (uint8_t)gCsStepMode0_c:
            {
                ParseMode0(0, &pEventData, &dataLength, pDstAppBuffer, pRemoteData);
            }
            break;

            case (uint8_t)gCsStepMode1_c:
            {
                ParseMode1(0, &pEventData, &dataLength, pDstAppBuffer, pRemoteData);
            }
            break;

            case (uint8_t)gCsStepMode2_c:
            {
                ParseMode2(0, &pEventData, &dataLength, pDstAppBuffer, pRemoteData);
            }
            break;

            case (uint8_t)gCsStepMode3_c:
            {
                ParseMode3(0, &pEventData, &dataLength, pDstAppBuffer, pRemoteData);
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
        MEM_BufferFree(pTemp);
    }
    
    return dataLength;
}
#endif /* defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1) */

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
    FLib_MemCpy(appData->mciqBuffer + appData->mciqBufferOffset, source, (uint32_t)nbBytes);
    appData->mciqBufferOffset += (uint16_t)nbBytes;
}

#if defined (gRasRREQ_d) && (gRasRREQ_d == 1)
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
    do
    {
        if ((filter & BIT2) != 0U)
        {
            /* Data includes Packet Quality*/
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
                pDstAppBuffer->csData.mode0Data[pRemoteData->crtNumSteps].quality = **ppEventData);
        }

        if ((filter & BIT3) != 0U)
        {
            /* Data includes Packet RSSI */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
                pDstAppBuffer->csData.mode0Data[pRemoteData->crtNumSteps].rssi = (int8_t)**ppEventData);
        }

        if ((filter & BIT4) != 0U)
        {
            /* Data includes Packet Antenna */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
                pDstAppBuffer->csData.mode0Data[pRemoteData->crtNumSteps].antenna = **ppEventData);
        }

        if ((mGlobalRangeSettings.role == gCsRoleReflector_c)
            && ((filter & BIT5) != 0U))
        {
            /* Data includes Measured_Freq_Offset information */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint16_t), bIncomplete,
                pDstAppBuffer->csData.mode0Data[pRemoteData->crtNumSteps].measuredFreqOffset = Utils_ExtractTwoByteValue(*ppEventData));
        }
        else
        {
            pDstAppBuffer->csData.mode0Data[pRemoteData->crtNumSteps].measuredFreqOffset = 0U;
        }
    } while(FALSE);

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
    bool_t bIncomplete = FALSE;
    
    uint32_t quality = 0U;
    uint8_t aNadm[CS_NADM_SIZE] = {};
    uint8_t aRssi[CS_RSSI_SIZE] = {};
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
                quality = (uint32_t)**ppEventData);
        }

        if ((filter & BIT3) != 0U)
        {
            /* Data includes Packet NADM */
            CheckSkipBytes(*ppEventData, *pDataLength, CS_NADM_SIZE, bIncomplete,
                FLib_MemCpy(aNadm, *ppEventData, CS_NADM_SIZE)); /* Packet_NADM */
        }

        if ((filter & BIT4) != 0U)
        {
            /* Data includes Packet RSSI */
            CheckSkipBytes(*ppEventData, *pDataLength, CS_RSSI_SIZE, bIncomplete,
                FLib_MemCpy(aRssi, *ppEventData, CS_RSSI_SIZE)
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
                rssiValue = (int8_t)(**ppEventData); /* do we need also dataLength--? */
#endif /* gAppParseRssiInfo_d */
            );
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
            CheckSkipBytesDoNothing(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete);
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
        pDstAppBuffer->tof_data.nbSteps ++;

        if ((filter & BIT3) != 0U)
        {
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, aNadm, (int)CS_NADM_SIZE);
        }
        if ((filter & BIT4) != 0U)
        {
            /* Data includes Packet RSSI */
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, aRssi, (int)CS_RSSI_SIZE);
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
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, &rssi, (int)CS_RSSI_SIZE);
        }
        if ((filter & BIT5) != 0U)
        {
            int16_t temp1 = 0;
            uint32_t ts_diff = 0;
            
            /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
            temp1 = ts_diff_hci/2;
            ts_diff = (uint32_t)(temp1); /* HCI reports half ns, application expects ns in Tof Buffer */
            ts_diff &= 0x00FFFFU;
            ts_diff |= (quality & 0x0FU) << gTimeStampDiffSize_c;
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)CS_TS_SIZE);
        }
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
    int32_t iq_dec[ISP_MAX_NO_ANTENNAS] = {};
    uint8_t tqi[ISP_MAX_NO_ANTENNAS] = {};
    
    /* First parse all bytes, based on filter bits, and store data in temporary variables */
    do
    {
        if ((filter & BIT2) != 0U)
        {
            /* Data includes Antenna Permutation Index */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
                antPermIndex = **ppEventData; /* Antenna_Permutation_Index */
            );
        }
        pAntIndex = &maAntPermNAp[antPermIndex][0];

        /* Num_Antenna_Path + 1 are reported by the firmware, but discard last one */
        /* Re-order per antenna path index */
        for (uint8_t idx = 0U; idx <= pRemoteData->numAntennaPaths; idx++)
        {
            if (idx == pRemoteData->numAntennaPaths)
            {
                /* Skip last IQ data (n_ap+1) */
                if (((antIdx == 0U) && ((filter & BIT5) != 0U)) ||
                ((antIdx == 1U) && ((filter & BIT6) != 0U)) ||
                ((antIdx == 2U) && ((filter & BIT7) != 0U)) ||
                ((antIdx == 3U) && ((filter & BIT8) != 0U)))
                {
                    CheckSkipBytesDoNothing(*ppEventData, *pDataLength, gTone_PCTSize_c + sizeof(uint8_t), bIncomplete);
                }
                break;
            }
            antIdx = pAntIndex[idx];

            /* Check if the corresponding Antenna Path is enabled */
            if (((antIdx == 0U) && ((filter & BIT5) != 0U)) ||
                ((antIdx == 1U) && ((filter & BIT6) != 0U)) ||
                ((antIdx == 2U) && ((filter & BIT7) != 0U)) ||
                ((antIdx == 3U) && ((filter & BIT8) != 0U)))
            {
                if ((filter & BIT3) != 0U)
                {
                    /* Data includes Tone_PCT information */
                    CheckSkipBytes(*ppEventData, *pDataLength, gTone_PCTSize_c, bIncomplete,
                        uint32_t temp1 = ((uint32_t)(*ppEventData)[2])<<BIT4 | ((uint32_t)(*ppEventData)[1])<<BIT3
                              | ((uint32_t)(*ppEventData)[0]);
                        int32_t iq = (int32_t)temp1;
                        
                        /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                        uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                            ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                        iq_dec[antIdx] = (int32_t)temp2;
                    );
                }

                if ((filter & BIT4) != 0U)
                {
                    /* Data includes Tone_Quality_Indication */
                    CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
                        tqi[antIdx] = **ppEventData);
                }
            }
        }
    } while(FALSE);
    
    /* In case no error occured, store temporary variables into the actual output */
    if (bIncomplete ==  FALSE)
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
    int32_t iq_dec[ISP_MAX_NO_ANTENNAS] = {};
    uint8_t tqi[ISP_MAX_NO_ANTENNAS] = {};
    uint32_t quality = 0U;
    uint8_t aRssi[CS_RSSI_SIZE] = {};
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
    int8_t rssiValue = 0;
#endif
    int16_t ts_diff_hci = 0;

    /* First parse all bytes, based on filter bits, and store data in temporary variables */
    do
    {
        /* ToF+Tone record */
        if ((filter & BIT2) != 0U)
        {
            /* Data includes Packet Quality*/
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
                quality = (uint32_t)(**ppEventData));
        }

        if ((filter & BIT3) != 0U)
        {
            /* Data includes Packet NADM */
            CheckSkipBytesDoNothing(*ppEventData, *pDataLength, 1U, bIncomplete);
        }
        
        if ((filter & BIT4) != 0U)
        {
            /* Data includes Packet RSSI */
            CheckSkipBytes(*ppEventData, *pDataLength, (int)CS_RSSI_SIZE, bIncomplete,
                FLib_MemCpy(aRssi, *ppEventData, CS_RSSI_SIZE)
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
                rssiVal = (int8_t)(**ppEventData);
#endif /* gAppParseRssiInfo_d */
            );
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
            CheckSkipBytesDoNothing(*ppEventData, *pDataLength, 1U, bIncomplete);
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

        antPermIndex = 0;

        if ((filter & BIT9) != 0U)
        {
            /* Data includes Antenna Permutation Index */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
                antPermIndex = **ppEventData; /* Antenna_Permutation_Index */
            );
        }
        pAntIndex = &maAntPermNAp[antPermIndex][0];

        /* Num_Antenna_Path + 1 are reported by the firmware, but discard last one */
        /* Re-order per antenna path index */
        for (uint8_t idx = 0U; idx <= pRemoteData->numAntennaPaths; idx++)
        {
            if (idx == pRemoteData->numAntennaPaths)
            {
                /* Skip last IQ data (n_ap+1) */
                if (((antIdx == 0U) && ((filter & BIT12) != 0U)) ||
                  ((antIdx == 1U) && ((filter & BIT13) != 0U)) ||
                  ((antIdx == 2U) && ((filter & BIT14) != 0U)) ||
                  ((antIdx == 3U) && ((filter & BIT15) != 0U)))
                {
                    CheckSkipBytesDoNothing(*ppEventData, *pDataLength, gTone_PCTSize_c + sizeof(uint8_t), bIncomplete);
                }
                break;
            }
            antIdx = pAntIndex[idx];

            /* Check if the corresponding Antenna Path is enabled */
            if (((antIdx == 0U) && ((filter & BIT12) != 0U)) ||
                ((antIdx == 1U) && ((filter & BIT13) != 0U)) ||
                ((antIdx == 2U) && ((filter & BIT14) != 0U)) ||
                ((antIdx == 3U) && ((filter & BIT15) != 0U)))
            {
                if ((filter & BIT10) != 0U)
                {
                    /* Data includes Tone_PCT information */
                    CheckSkipBytes(*ppEventData, *pDataLength, gTone_PCTSize_c, bIncomplete,
                        uint32_t temp1 = ((uint32_t)(*ppEventData)[2])<<BIT4 | ((uint32_t)(*ppEventData)[1])<<BIT3 | ((uint32_t)(*ppEventData)[0]);
                        int32_t iq = (int32_t)temp1;

                        /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                        uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                                          ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                        iq_dec[antIdx] = (int32_t)temp2;
                     );
                }

                if ((filter & BIT11) != 0U)
                {
                    /* Data includes Tone_Quality_Indication */
                    CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
                        tqi[antIdx] = **ppEventData);
                }
            }
        }
    } while (FALSE);
    
    /* In case no error occured, store temporary variables into the actual output */
    if (bIncomplete ==  FALSE)
    {
        /* ToF+Tone record */
        pRemoteData->step++;

        if ((filter & BIT4) != 0U)
        {
            /* Data includes Packet RSSI */
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, aRssi, (int)CS_RSSI_SIZE);
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
            if (rssiValue != gRssiNotAvailable_c)
            {
                /* Count RSSI if available */
                pDstAppBuffer->aRssiValue[pDstAppBuffer->rssiStepNo] = rssiValue;
                pDstAppBuffer->rssiStepNo++;
            }
#endif /* gAppParseRssiInfo_d */
        }

        if ((filter & BIT5) != 0U)
        {
            int16_t temp1 = 0;
            uint32_t ts_diff = 0;

            /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
            temp1 = ts_diff_hci/2;
            ts_diff = (uint32_t)(temp1); /* HCI reports half ns, application expects ns in Tof Buffer */
            ts_diff &= 0x00FFFFU;
            ts_diff |= (quality & 0x0FU) << gTimeStampDiffSize_c;
            hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)CS_TS_SIZE);
        }
        
        for (uint8_t idx = 0U; idx < pDstAppBuffer->mciq_data.n_ap; idx++)
        {
            hciCsStoreBytesInIqBuffer(pDstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
            hciCsStoreBytesInIqBuffer(pDstAppBuffer, &tqi[idx], 1);
        }

        pDstAppBuffer->mciq_data.nbSteps++;
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
    do
    {
        /* Data includes Packet Quality */
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
            pDstAppBuffer->csData.mode0Data[pRemoteData->crtNumSteps].quality = **ppEventData);

        /* Data includes Packet RSSI */
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
            pDstAppBuffer->csData.mode0Data[pRemoteData->crtNumSteps].rssi = (int8_t)**ppEventData);

        /* Data includes Packet Antenna */
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
            pDstAppBuffer->csData.mode0Data[pRemoteData->crtNumSteps].antenna = **ppEventData);
        
        if (mGlobalRangeSettings.role == gCsRoleReflector_c)
        {
            /* Data includes Measured_Freq_Offset information */
            CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint16_t), bIncomplete,
                pDstAppBuffer->csData.mode0Data[pRemoteData->crtNumSteps].measuredFreqOffset = Utils_ExtractTwoByteValue(*ppEventData));
        }
        else
        {
            pDstAppBuffer->csData.mode0Data[pRemoteData->crtNumSteps].measuredFreqOffset = 0U;
        }
    } while(FALSE);
    
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
    uint8_t aNadm[CS_NADM_SIZE] = {};
    uint8_t aRssi[CS_RSSI_SIZE] = {};
    int16_t ts_diff_hci = 0;
    
    bool_t bIncomplete = FALSE;
    
    /* First parse all bytes, based on filter bits, and store data in temporary variables */
    do
    {        
        /* Data includes Packet Quality*/
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete, 
            quality = (uint32_t)**ppEventData);

        /* Data includes Packet NADM */
        CheckSkipBytes(*ppEventData, *pDataLength, CS_NADM_SIZE, bIncomplete,
            FLib_MemCpy(aNadm, *ppEventData, CS_NADM_SIZE)); /* Packet_NADM */

        /* Data includes Packet RSSI */
        CheckSkipBytes(*ppEventData, *pDataLength, CS_RSSI_SIZE, bIncomplete,
            FLib_MemCpy(aRssi, *ppEventData, CS_RSSI_SIZE)
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
            rssiValue = (int8_t)(**ppEventData);
#endif /* gAppParseRssiInfo_d */
        );

        /* Data includes ToA_ToD_Initiator/ToD_ToA_Reflector information */
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint16_t), bIncomplete,
            FLib_MemCpy(&ts_diff_hci, *ppEventData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
        );

        /* Data includes Packet Antenna */
        CheckSkipBytesDoNothing(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete);
    } while(FALSE);

    /* In case no error occured, store temporary variables into the actual output */
    if (bIncomplete == FALSE)
    {
        /* ToF record */
        pRemoteData->step++;
        pDstAppBuffer->tof_data.nbSteps ++;

        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aNadm, (int)CS_NADM_SIZE);
        
        /* Data includes Packet RSSI */
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aRssi, (int)CS_RSSI_SIZE);
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
        if (rssiValue != gRssiNotAvailable_c)
        {
            /* Count RSSI if available */
            pDstAppBuffer->aRssiValue[pDstAppBuffer->rssiStepNo] = rssiValue;
            pDstAppBuffer->rssiStepNo++; /* TBD: this must be reverted in case of a failure */
        }
#endif /* gAppParseRssiInfo_d */
        
        int16_t temp1 = 0;
        uint32_t ts_diff = 0;
        
        /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
        temp1 = ts_diff_hci/2;
        ts_diff = (uint32_t)(temp1); /* HCI reports half ns, application expects ns in Tof Buffer */
        ts_diff &= 0x00FFFFU;
        ts_diff |= (quality & 0x0FU) << gTimeStampDiffSize_c;
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)CS_TS_SIZE);
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
    int32_t iq_dec[ISP_MAX_NO_ANTENNAS] = {};
    uint8_t tqi[ISP_MAX_NO_ANTENNAS] = {};
    uint8_t quality = 0U;
    
    /* First parse all bytes, based on filter bits, and store data in temporary variables */
    do
    {
        /* Data includes Antenna Permutation Index */
        GetItem(&antPermIndex);
        pAntIndex = &maAntPermNAp[antPermIndex][0];
        
        /* Data includes Packet Quality*/
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete, 
            quality = (uint32_t)**ppEventData);

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
    int32_t iq_dec[ISP_MAX_NO_ANTENNAS] = {};
    uint8_t tqi[ISP_MAX_NO_ANTENNAS] = {};
    uint32_t quality = 0u;
    uint8_t pctQuality = 0u;
    uint8_t aRssi[CS_RSSI_SIZE] = {};
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
    int8_t rssiValue = 0;
#endif
    int16_t ts_diff_hci = 0;
    int16_t temp = 0;
    uint32_t ts_diff = 0;

    /* First parse all bytes, based on filter bits, and store data in temporary variables */
    do
    {
        /* ToF+Tone record */
        /* Data includes Packet Quality*/
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
            quality = (uint32_t)(**ppEventData));

        /* Data includes Packet NADM */
        CheckSkipBytesDoNothing(*ppEventData, *pDataLength, 1U, bIncomplete);

        /* Data includes Packet RSSI */
        CheckSkipBytes(*ppEventData, *pDataLength, (int)CS_RSSI_SIZE, bIncomplete,
            FLib_MemCpy(aRssi, *ppEventData, CS_RSSI_SIZE)
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
            rssiVal = (int8_t)(**ppEventData);
#endif /* gAppParseRssiInfo_d */
        );

        /* Data includes ToA_ToD_Initiator/ToD_ToA_Reflector information */
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint16_t), bIncomplete,
            FLib_MemCpy(&ts_diff_hci, *ppEventData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
        );

        /* Skip Antenna used by the sender */
        CheckSkipBytesDoNothing(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete);

        /* Data includes Antenna Permutation Index */
        GetItem(&antPermIndex);
        pAntIndex = &maAntPermNAp[antPermIndex][0];

        /* Extract quality - 2 bits per antenna path, up to 4 antenna paths, ordered*/
        CheckSkipBytes(*ppEventData, *pDataLength, sizeof(uint8_t), bIncomplete,
            pctQuality = (uint32_t)(**ppEventData));

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

        /* Data includes Packet RSSI */
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, aRssi, (int)CS_RSSI_SIZE);
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
        hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)CS_TS_SIZE);

        for (uint8_t idx = 0U; idx < pDstAppBuffer->mciq_data.n_ap; idx++)
        {
            hciCsStoreBytesInIqBuffer(pDstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
            hciCsStoreBytesInIqBuffer(pDstAppBuffer, &tqi[idx], 1);
        }

        pDstAppBuffer->mciq_data.nbSteps++;
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
    if (mCount == 0)
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