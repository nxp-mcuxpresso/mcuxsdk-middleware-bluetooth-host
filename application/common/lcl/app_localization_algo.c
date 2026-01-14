/*! *********************************************************************************
* Copyright 2023 - 2026 NXP
*
* \file app_localization_algo.c
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
#include "app_localization_algo.h"
#include "fsl_component_mem_manager.h"
#include "dm_phasebased.h"
#include "pde_rade.h"
#include "channel_sounding.h"
#include "app_localization_data_export.h"

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d==1U)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
#include "ranging_client_interface.h"
#endif /* defined (gRasRREQ_d) && (gRasRREQ_d == 1U) */
#elif defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
#include "btcs_client_interface.h"
#elif defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
#include "btcs_server_interface.h"
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */

#ifdef LCE_KW47_MCXW72
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)
#include "PWR_Interface.h"
#endif /* gAppLowpowerEnabled_d */
#endif /* LCE_KW47_MCXW72 */
/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define  CS_GET_RTT_TS_DIFF(report_p, ts_diff, aa_quality) \
    { \
        ts_diff = ((uint16_t)((report_p)[1]) << 8) + (report_p)[0]; \
        aa_quality = ((report_p)[2] >> 4) & 0xFU; \
    }

#define ISP_MCIQ_SIGN_EXTEND_12_16(x)     ((x) | ((((x) & 0x800U) != 0U) ? 0xF000U : 0x0U))
#define FREQMASK_SET(pFreqMask, i) (pFreqMask[(i) >> 5U] |= ((uint32_t)1U << ((i) & 0x1FU)))

#define gIQSampleSize_c         12U
#define gTimeStampDiffSize_c    20U
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
#ifdef LCE_KW47_MCXW72
/* ID of LCE exclusive heap */
extern uint8_t g_ceHeap_id;
#else
static uint8_t g_ceHeap_id = 0U;
#endif
/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static void AppLocalizationAlgo_UncompressResponse
(
    void *srcResultBuffer,
    csAppData_t *dstAppBuffer
);

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

static void isp_mciq_ranging_compute
(
    isp_meas_response_t *meas_response,
    mciq_result_t *mciq_result,
    engine_config_t *engine_config
);

static void isp_tof_ranging_compute
(
    isp_meas_response_t *meas_response,
    tof_result_t *tof_result
);

static void isp_mciq_measurement_unpack_iqs(cs_data_t *cs_data,
                                            mciq_data_t *data1, mciq_data_t *data2,
                                            int16_t *pIQout1, int16_t *pIQout2,
                                            int16_t *pMeasIQout1, int16_t *pMeasIQout2,
                                            uint32_t freqMask[], uint32_t tqi1Mask[],
                                            uint32_t tqi2Mask[], uint32_t nbValid[]);

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
static void AppLocalizationAlgo_UncompressRemoteResponse
(
    void *pSrcResultBuffer,
    csAppData_t *pDstAppBuffer
);
#endif /* defined (gRasRREQ_d) && (gRasRREQ_d == 1U) */
#else
static void AppLocalizationAlgo_UncompressRemoteResponseL2CAP
(
    void*          pSrcResultBuffer,
    void*          pSrcLocalBuffer,
    csAppData_t* pDstAppBuffer
);
#endif

static uint8_t AppLocalizationAlgo_ComputeTsw(deviceId_t deviceId);
static uint8_t AppLocalizationAlgo_CountLeadingZeroes(uint16_t decimalPart);
/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
/* Buffer to hold local and peer measurement data */
csAppData_t gLocalAppDataBuffer;
csAppData_t gRemoteAppDataBuffer;
csAppData_t *localAppDataBuffer = &gLocalAppDataBuffer;
csAppData_t *remoteAppDataBuffer = &gRemoteAppDataBuffer;

#if (defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)) || \
    (defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1))
static const uint32_t mPrecisionScaler = 100U; /* Scaler used for setting the display precision of the measurement results. 10^(number of decimals needed) */
#endif

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn         void AppLocalizationAlgo_ResetPeer(deviceId_t deviceId);
*
*\brief      Algo-related cleanup on peer disconnection.
*
*\param[in]  deviceId         DeviceId of the peer.
*
*\retval     none
********************************************************************************** */
void AppLocalizationAlgo_ResetPeer(deviceId_t deviceId)
{
    if (mRangeSettings[deviceId].csAlgoBuf != NULL)
    {
        rade_deinit(&mRangeSettings[deviceId].csAlgoBuf);
    }
}

/*! *********************************************************************************
 *\fn           void AppLocalizationAlgo_RunMeasurement(deviceId_t deviceId,
 *                                  rasMeasurementData_t *pLocalData,
 *                                  rasMeasurementData_t *pPeerData,
 *                                  uint8_t role,
 *                                  localizationAlgoResult_t *pResult);
 *
 *\brief        Run given algorithm for given data.
 *
 *\param[in]    deviceId            DeviceId of the peer (used to retrieve timing info).
 *\param[in]    pLocalData          Pointer to local data.
 *\param[in]    pPeerData           Pointer to peer data.
 *\param[in]    role                CS role of the local device (initiator or reflector).
 *\param[out]   pResult             Pointer to location for result information.
 *
 *\retval       none
 ********************************************************************************** */
void AppLocalizationAlgo_RunMeasurement
(
    deviceId_t deviceId,
    rasMeasurementData_t *pLocalData,
    rasMeasurementData_t *pPeerData,
    uint8_t role,
    localizationAlgoResult_t *pResult
)
{
    static isp_meas_response_t response;

    csAppData_t *csDataBuffer0, *csDataBuffer1;
#if (defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)) || \
    (defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1))
    uint32_t tempDistance;
    uint32_t tempDqi;

    /* MISRA Rule 10.3 - The value of an expression shall not be assigned to an object with a narrower essential type or of a different essential type category */
    union
    {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
    }var4 = {0};
#endif

#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1)
    pResult->rssiInfo.rssiLocalNo = 0U;
    pResult->rssiInfo.rssiRemoteNo = 0U;
    FLib_MemSet(pResult->rssiInfo.aRssiLocal, gRssiNotAvailable_c, APP_LOCALIZATION_MAX_STEPS);
    FLib_MemSet(pResult->rssiInfo.aRssiRemote, gRssiNotAvailable_c, APP_LOCALIZATION_MAX_STEPS);
#endif /* gAppParseRssiInfo_d */

    if (pResult != NULL)
    {
        FLib_MemSet(&response, 0, sizeof(isp_meas_response_t));
        response.cs_data = &localAppDataBuffer->csData;

        /* Reset procedure buffers for a new measurement */
        FLib_MemSet(&gLocalAppDataBuffer, 0, sizeof(csAppData_t));
        FLib_MemSet(&gRemoteAppDataBuffer, 0, sizeof(csAppData_t));

        /* Populate timing information to be used by algorithm */
        response.cs_data->t_fcs             = mRangeSettings[deviceId].t_fcs;
        response.cs_data->t_ip1             = mRangeSettings[deviceId].t_ip1;
        response.cs_data->t_ip2             = mRangeSettings[deviceId].t_ip2;
        response.cs_data->t_pm              = mRangeSettings[deviceId].t_pm;
        response.cs_data->main_mode_type    = mRangeSettings[deviceId].main_mode_type;
        response.cs_data->sub_mode_type     = mRangeSettings[deviceId].sub_mode_type;
        response.cs_data->main_mode_repeat  = mRangeSettings[deviceId].main_mode_repeat;
        response.cs_data->sub_mode_type     = mRangeSettings[deviceId].sub_mode_type;
        response.cs_data->mode0_nb          = mRangeSettings[deviceId].mode0_nb;
        response.cs_data->rtt_type          = mRangeSettings[deviceId].rtt_type;
        response.cs_data->phy               = mRangeSettings[deviceId].phy;
        response.cs_data->t_sw              = AppLocalizationAlgo_ComputeTsw(deviceId);
        response.cs_data->conn_interval     = mRangeSettings[deviceId].connInterval;
        response.cs_data->csAlgoBuf         = &mRangeSettings[deviceId].csAlgoBuf;

        /* Uncompress local data */
        for (uint8_t idx = 0U; idx <= pLocalData->subeventIndex; idx++)
        {
            response.cs_data->subevtDoneStatusLocal[idx] = pLocalData->aSubEventData[idx].subevtHeader.subeventDoneStatus;
        }
        AppLocalizationAlgo_UncompressResponse(pLocalData, localAppDataBuffer);

        /* Uncompress remote data */
        for (uint8_t idx = 0U; idx <= pPeerData->subeventIndex; idx++)
        {
            response.cs_data->subevtDoneStatusRemote[idx] = pPeerData->aSubEventData[idx].subevtHeader.subeventDoneStatus;
        }
#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
        AppLocalizationAlgo_UncompressRemoteResponse(pPeerData, remoteAppDataBuffer);
#endif /* defined (gRasRREQ_d) && (gRasRREQ_d == 1U) */
#else
        AppLocalizationAlgo_UncompressRemoteResponseL2CAP(pPeerData, pLocalData, remoteAppDataBuffer);
#endif
        FLib_MemCpy(remoteAppDataBuffer->csData.channelMap,
                    localAppDataBuffer->csData.channelMap,
                    APP_LOCALIZATION_MAX_STEPS);
        FLib_MemCpy(remoteAppDataBuffer->csData.modeMap,
                    localAppDataBuffer->csData.modeMap,
                    APP_LOCALIZATION_MAX_STEPS);

        /* Reorder data so that index 0 represents initiator, index 1 represents reflector whatever the device role is */
        if (role == gCsRoleInitiator_c)
        {
            csDataBuffer0 = localAppDataBuffer;
            csDataBuffer1 = remoteAppDataBuffer;
            FLib_MemCpy(response.cs_data->subevtRefPowerLevelInit, localAppDataBuffer->csData.subevtRefPowerLevelInit, gCsSubeventMax_c);
            FLib_MemCpy(response.cs_data->subevtRefPowerLevelRefl, remoteAppDataBuffer->csData.subevtRefPowerLevelInit, gCsSubeventMax_c);
        }
        else
        {
            csDataBuffer1 = localAppDataBuffer;
            csDataBuffer0 = remoteAppDataBuffer;
            FLib_MemCpy(response.cs_data->subevtRefPowerLevelRefl, localAppDataBuffer->csData.subevtRefPowerLevelInit, gCsSubeventMax_c);
            FLib_MemCpy(response.cs_data->subevtRefPowerLevelInit, remoteAppDataBuffer->csData.subevtRefPowerLevelInit, gCsSubeventMax_c);
        }

        if (localAppDataBuffer->mciq_data.nbSteps != 0U)
        {
            /* MCIQ */
            response.mciq_data[0] = csDataBuffer0->mciq_data;
            response.mciq_data[0].iq = (uint8_t *)(csDataBuffer0->mciqBuffer);
            response.mciq_data[1] = csDataBuffer1->mciq_data;
            response.mciq_data[1].iq = (uint8_t *)(csDataBuffer1->mciqBuffer);
        }
        if (localAppDataBuffer->tof_data.nbSteps != 0U)
        {
            /* ToF */
            response.tof_data[0] = csDataBuffer0->tof_data;
            response.tof_data[0].ts = csDataBuffer0->tofBuffer;
            response.tof_data[1] = csDataBuffer1->tof_data;
            response.tof_data[1].ts = csDataBuffer1->tofBuffer;
        }

        /* Invoke ranging engine on client */
        engine_response_t engine_response = {0};

        engine_config_t engine_config;
        engine_config.mciq_algo_flags = pResult->algorithm;
        engine_config.n_ap = response.mciq_data->n_ap;
        engine_config.cde_threshold = DM_CDE_THRESHOLD_DEFAULT;
        engine_config.cde_div_threshold = DM_CDE_THRESHOLD_DIVERSITY_DEFAULT;

        /* Compute PBR distance */
        isp_mciq_ranging_compute(&response, &engine_response.mciq_result, &engine_config);

        /* Compute RTT distance */
        isp_tof_ranging_compute(&response, &engine_response.tof_result);
        pResult->rttResult.dm_sr = engine_response.tof_result.dm_sr;
        pResult->rttResult.dm_ad = engine_response.tof_result.dm_ad;

#if defined(gAppLocDataExport_c) && (gAppLocDataExport_c > 0)
        engine_response.is_valid = TRUE;
        app_print_cs_data(&response, &engine_response, &mRangeSettings[deviceId]);
#endif 

#if defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)
        if ((pResult->algorithm & eMciqAlgoEmbedCDE) != 0U)
        {
            float var2 = 0.f, var3 = 0.f, dist1 = 0.f, f1 = 0.f;
            uint32_t div1 = 0U;

            /* MISRA Rule 10.3 - The value of an expression shall not be assigned to an object with a narrower essential type or of a different essential type category */
            union
            {
                int16_t i16;
                int32_t i32;
                uint32_t u32;
                uint64_t u64;
            }var1 = {0};

            union
            {
                int16_t i16;
                int32_t i32;
                uint64_t u64;
            }res = {0};

            if (engine_response.mciq_result.cde_fp < 0)
            {
                /* Get 2's complement (absolute value of number) */
                var1.i32 = engine_response.mciq_result.cde_fp;
                res.u64 = 0xFFFFFFFFUL ^ var1.u64;
                res.u64 += 1U;
                engine_response.mciq_result.cde_fp = res.i32;
            }
            var1.u64 = 0U;
            res.u64 = 0U;
            if (engine_response.mciq_result.cde_dqi < 0)
            {
                /* Get 2's complement (absolute value of number) */
                var1.i16 = engine_response.mciq_result.cde_dqi;
                res.u64 = 0xFFFFFFFFUL ^ var1.u64;
                res.u64 += 1U;
                engine_response.mciq_result.cde_dqi = res.i16;
            }
            var2 = (float)(engine_response.mciq_result.cde_fp);
            pResult->resultCDE.distanceInMeters = var2 / 1024.0f;
            /* Convert decimal number to percentage. */
            var3 = (float)(engine_response.mciq_result.cde_dqi);
            pResult->resultCDE.dqiPercentage = var3 / 16384.0f * 100.0f;

            /* Convert float number to int, keeping just mDecimalPrecision decimals. */
            dist1 = pResult->resultCDE.distanceInMeters * (float)mPrecisionScaler;
            tempDistance = (uint32_t)(dist1);
            /* Get the integer part of the distance in meters. */
            var4.u32 = (tempDistance / mPrecisionScaler);
            pResult->resultCDE.distanceIntegerPart = var4.u8;
            /* Get the decimal part of the distance in meters. */
            var4.u32 = (tempDistance - (tempDistance / mPrecisionScaler) * mPrecisionScaler);
            pResult->resultCDE.distanceDecimalPart = var4.u16;
            /* Count leading zeroes for decimal part for display purposes. */
            pResult->resultCDE.leadingZeroesDecimalPart = AppLocalizationAlgo_CountLeadingZeroes(pResult->resultCDE.distanceDecimalPart);

            /* Get integer and decimal parts, keeping just log10(mDecimalPrecision) decimals.
            Division by 100 is needed because 2 decimals where already included in the conversion to percentage. */
            div1 = mPrecisionScaler / 100U;
            f1 = (pResult->resultCDE.dqiPercentage * (float)(div1));
            tempDqi = (uint32_t)f1;
            /* Get the integer part of the DQI in percentage. */
            var4.u32 = (tempDqi / (mPrecisionScaler / 100U));
            pResult->resultCDE.dqiIntegerPart = var4.u16;
            /* Get the decimal part of the DQI in percentage. */
            var4.u32 = (tempDqi - ((tempDqi / (mPrecisionScaler / 100U)) * (mPrecisionScaler / 100U)));
            pResult->resultCDE.dqiDecimalPart = var4.u16;
        }
#endif /* gAppUseCDEAlgorithm_d */

#if defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)
        if ((pResult->algorithm & eMciqAlgoEmbedRADE) != 0U)
        {
            float f2 = 0.f, f3 = 0.f;
            uint32_t div2 = 0U;

            mciq_result_t* result = &engine_response.mciq_result;
            pResult->radeError = result->rade_error_flag;
            pResult->resultRADE.distanceInMeters = result->rade_dist;
            pResult->resultRADE.dqiPercentage = result->rade_dqi * 100.f; /* Convert decimal number to percentage. */

            /* Convert float number to int, keeping just mDecimalPrecision decimals. */
            f2 = pResult->resultRADE.distanceInMeters * (float)(mPrecisionScaler);
            tempDistance = (uint32_t)(f2);
            /* Get the integer part of the distance in meters. */
            var4.u32 = (tempDistance / mPrecisionScaler);
            pResult->resultRADE.distanceIntegerPart = var4.u8;
            /* Get the decimal part of the distance in meters. */
            var4.u32 = (tempDistance - (tempDistance / mPrecisionScaler) * mPrecisionScaler);
            pResult->resultRADE.distanceDecimalPart = var4.u16;
            /* Count leading zeroes for decimal part for display purposes. */
            pResult->resultRADE.leadingZeroesDecimalPart = AppLocalizationAlgo_CountLeadingZeroes(pResult->resultRADE.distanceDecimalPart);

            /* Get integer and decimal parts, keeping just log10(mDecimalPrecision) decimals.
            Division by 100 is needed because 2 decimals where already included in the conversion to percentage. */
            div2 = mPrecisionScaler / 100U;
            f3 = pResult->resultRADE.dqiPercentage * (float)(div2);
            tempDqi = (uint32_t)(f3);
            /* Get the integer part of the DQI in percentage. */
            var4.u32 = (tempDqi / (mPrecisionScaler / 100U));
            pResult->resultRADE.dqiIntegerPart = var4.u16;
            /* Get the decimal part of the DQI in percentage. */
            var4.u32 = (tempDqi - ((tempDqi / (mPrecisionScaler / 100U)) * (mPrecisionScaler / 100U)));
            pResult->resultRADE.dqiDecimalPart = var4.u16;
        }
#endif /* gAppUseRADEAlgorithm_d */

#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1)
    /* Copy local RSSI info */
    pResult->rssiInfo.rssiLocalNo = localAppDataBuffer->rssiStepNo;
    FLib_MemCpy(pResult->rssiInfo.aRssiLocal, localAppDataBuffer->aRssiValue, APP_LOCALIZATION_MAX_STEPS);
    /* Copy remote RSSI info */
    pResult->rssiInfo.rssiRemoteNo = remoteAppDataBuffer->rssiStepNo;
    FLib_MemCpy(pResult->rssiInfo.aRssiRemote, remoteAppDataBuffer->aRssiValue, APP_LOCALIZATION_MAX_STEPS);
#endif /* gAppParseRssiInfo_d */
    }
    return;
}
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
 *\fn           void AppLocalizationAlgo_UncompressResponse(void *srcResultBuffer,
 *                                  csAppData_t *dstAppBuffer);
 *
 * \brief       Uncompress OTA data (HCI-like format) for a given device
 *
 * \param[in]   srcResultBuffer     Pointer to uncompressed data
 * \param[in]   dstAppBuffer        Pointer to destination buffer for uncompressed data
 *
 *\retval       none
 ********************************************************************************** */
static void AppLocalizationAlgo_UncompressResponse
(
    void *srcResultBuffer,
    csAppData_t *dstAppBuffer
)
{
    int totalStepCounter = 0;
    int numSteps;
    int step;
#ifdef SKIP_MAIN_MODES_REPET
    uint32_t parsedMainModeNb = CS_MAIN_MODE_REPEAT_MAX;
#endif
    uint8_t *eventData, *stepData;
    rasMeasurementData_t *pLocalData = (rasMeasurementData_t*)srcResultBuffer;

    dstAppBuffer->mciq_data.n_ap = pLocalData->numAntennaPaths;
    eventData = pLocalData->pData;
    numSteps = (int)(pLocalData->totalNumSteps);

    step = 0;
    dstAppBuffer->csData.subevt_nb = 0;

    for (int i = 0; i < numSteps; i ++)
    {
        bool_t tofPresent = FALSE;
        bool_t mciqPresent = FALSE;
        uint8_t mode = *eventData++;
        uint8_t channel = *eventData++;
        uint8_t stepDataLength = *eventData++;
        stepData = eventData;

        dstAppBuffer->csData.channelMap[step] = channel;
        dstAppBuffer->csData.modeMap[step] = mode;

        /* Check if there is reported step data - skip to next step if not */
        if (stepDataLength == 0U)
        {
            step ++;
            continue;
        }

        if (mode == 0U)
        {
            stepData += stepDataLength;
            eventData = stepData;
            step ++;
            continue;
        }

#ifdef SKIP_MAIN_MODES_REPET
        /* Skip main mode repetitions */
        if ((mode == 2) && (parsedMainModeNb < meas_params.cfg.main_mode_repeat)) {
            stepData += stepDataLength;
            eventData = stepData;
            parsedMainModeNb++;
            continue;
        }
#endif

        switch(mode)
        {
            case 1:
            {
                /* ToF record */
                tofPresent = TRUE;
            }
            break;

            case 2:
            {
                /* Tone record */
                mciqPresent = TRUE;
            }
            break;

            case 3:
            {
                /* ToF+Tone record */
                tofPresent = TRUE;
                mciqPresent = TRUE;
            }
            break;

            default:
            {
                /* mode not yet implemented, skip data */
                stepData += stepDataLength;
            }
            break;
        }

        if (tofPresent)
        {
            uint32_t ts_diff = 0;
            int16_t ts_diff_hci = 0;
            int16_t temp1 = 0;
            /* ToF record */
            uint32_t quality = (uint32_t)(*stepData++); /* Packet_AA_Quality */
            hciCsStoreBytesInTofBuffer(dstAppBuffer, stepData, (int)CS_NADM_SIZE); /* Packet_NADM */
            stepData += CS_NADM_SIZE; /* Packet_NADM */
            hciCsStoreBytesInTofBuffer(dstAppBuffer, stepData, (int)CS_RSSI_SIZE); /* Packet_RSSI */

#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
            if ((int8_t)(*stepData) != gRssiNotAvailable_c)
            {
                /* Count RSSI if available */
                dstAppBuffer->aRssiValue[dstAppBuffer->rssiStepNo] = (int8_t)(*stepData);
                dstAppBuffer->rssiStepNo++;
            }
#endif /* gAppParseRssiInfo_d */

            stepData ++;
            FLib_MemCpy(&ts_diff_hci, stepData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
            stepData += sizeof(uint16_t);
            stepData ++; /* Packet_Antenna, ignored */

            /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
            temp1 = ts_diff_hci/2;
            ts_diff = (uint32_t)(temp1); /* HCI reports half ns, application expects ns in Tof Buffer */
            ts_diff &= 0x00FFFFU;
            ts_diff |= (quality&0x0FU)<<gTimeStampDiffSize_c;
            hciCsStoreBytesInTofBuffer(dstAppBuffer, (uint8_t *)&ts_diff, (int)CS_TS_SIZE);
            dstAppBuffer->tof_data.nbSteps ++;
        }

        if (mciqPresent)
        {
            /* Tone record */
            uint8_t antPermIndex = *stepData++; /* Antenna_Permutation_Index */
            const uint8_t *antIndex_p = &maAntPermNAp[antPermIndex][0];
            int32_t iq_dec[ISP_MAX_NO_ANTENNAS];
            uint8_t tqi[ISP_MAX_NO_ANTENNAS];

            /* Num_Antenna_Path + 1 are reported by the firmware, but discard last one */
            /* Re-order per antenna path index */
            for (uint8_t idx = 0U; idx < dstAppBuffer->mciq_data.n_ap; idx++)
            {
                int antIdx = (int)antIndex_p[idx];
                uint32_t temp1 = (((uint32_t)stepData[2])<<BIT4) | (((uint32_t)stepData[1])<<BIT3)
                                 | ((uint32_t)stepData[0]);
                int32_t iq = (int32_t)temp1;

                /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                                  ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                iq_dec[antIdx] = (int32_t)temp2;
                stepData += gTone_PCTSize_c;
                tqi[antIdx] = *stepData;
                stepData += sizeof(uint8_t);
            }

            for (uint8_t idx = 0U; idx < dstAppBuffer->mciq_data.n_ap; idx++)
            {
                hciCsStoreBytesInIqBuffer(dstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
                hciCsStoreBytesInIqBuffer(dstAppBuffer, &tqi[idx], 1);
            }
            /* Skip last IQ data (n_ap+1) */
            stepData += gTone_PCTSize_c + sizeof(uint8_t);
            dstAppBuffer->mciq_data.nbSteps ++;
        }
        eventData = stepData;
        step ++;
    }

    /* Populate additional fields in dstAppBuffer */

    /* Total number of steps */
    dstAppBuffer->csData.step_nb = (uint16_t)step;

    /* Start ACL count */
    dstAppBuffer->csData.startAclCnt =
            pLocalData->aSubEventData[dstAppBuffer->csData.subevt_nb].subevtHeader.startACLConnEvent;

    /* For every subevent */
    for (uint8_t index = 0; index <= pLocalData->subeventIndex; index++)
    {
        /* The stop index is the total number of previous steps plus the current subevent's steps */
        dstAppBuffer->csData.subevtStopIdx[index] =
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

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
/*! *********************************************************************************
 *\fn           void AppLocalizationAlgo_UncompressRemoteResponse(void *pSrcResultBuffer,
 *                                  csAppData_t *pDstAppBuffer);
 *
 * \brief       Uncompress OTA data (HCI-like format) for a given device
 *
 * \param[in]   pSrcResultBuffer    Pointer to uncompressed data
 * \param[in]   pDstAppBuffer       Pointer to destination buffer for uncompressed data
 *
 *\retval       none
 ********************************************************************************** */
static void AppLocalizationAlgo_UncompressRemoteResponse
(
    void *pSrcResultBuffer,
    csAppData_t *pDstAppBuffer
)
{
    uint16_t totalStepCounter = 0U;
    int32_t iq_dec[ISP_MAX_NO_ANTENNAS] = {0};
    uint8_t tqi[ISP_MAX_NO_ANTENNAS] = {0u};
    rasMeasurementData_t *pRemoteData = (rasMeasurementData_t*)pSrcResultBuffer;
    pDstAppBuffer->mciq_data.n_ap = pRemoteData->numAntennaPaths;
    pDstAppBuffer->csData.subevt_nb = 0;
    uint8_t* pEventData = pRemoteData->pData;

    uint8_t step = 0;
    uint16_t crtDataLen = 0U;
    uint16_t filter;
    uint32_t ts_diff = 0;
    int16_t ts_diff_hci = 0;
    uint8_t antPermIndex;
    uint8_t const* pAntIndex;
    uint8_t antIdx;
    uint32_t quality;
    uint8_t mode;

    /* Skip procedure header */
    pEventData += sizeof(rasRangingDataHeader_t);
    crtDataLen += (uint16_t)sizeof(rasRangingDataHeader_t);
    /* Skip first subevent counter */
    pEventData += sizeof(rasSubeventDataHeader_t);
    crtDataLen += (uint16_t)sizeof(rasSubeventDataHeader_t);

    while (crtDataLen < pRemoteData->totalSentRcvDataIndex)
    {
        mode = *pEventData++;
        crtDataLen++;

        if ((mode & BIT7) != 0U)
        {
            /* Step aborted, assume length zero */
        }
        else
        {
            assert(mode <= 3);

            pDstAppBuffer->csData.modeMap[step] = mode;
            /* Get filter for the current mode */
            filter = RasClient_GetModeFilter(pRemoteData->deviceId, mode);
            quality = 0;

            switch(mode)
            {
                case 0:
                {
                    if ((filter & BIT2) != 0U)
                    {
                        /* Data includes Packet Quality*/
                        pEventData++;
                        crtDataLen++;
                    }

                    if ((filter & BIT3) != 0U)
                    {
                        /* Data includes Packet RSSI */
                        pEventData++;
                        crtDataLen++;
                    }

                    if ((filter & BIT4) != 0U)
                    {
                        /* Data includes Packet Antenna */
                        pEventData++;
                        crtDataLen++;
                    }

                    if ((mGlobalRangeSettings.role == gCsRoleReflector_c)
                        && ((filter & BIT5) != 0U))
                    {
                        /* Data includes Measured_Freq_Offset information */
                        pEventData += sizeof(uint16_t);
                        crtDataLen += (uint16_t)sizeof(uint16_t);
                    }
                }
                break;

                case 1:
                {
                    /* ToF record */
                    step ++;
                    pDstAppBuffer->tof_data.nbSteps ++;

                    if ((filter & BIT2) != 0U)
                    {
                        /* Data includes Packet Quality*/
                        quality = (uint32_t)(*pEventData++);
                        crtDataLen++;
                    }

                    if ((filter & BIT3) != 0U)
                    {
                        /* Data includes Packet NADM */
                        hciCsStoreBytesInTofBuffer(pDstAppBuffer, pEventData, (int)CS_NADM_SIZE); /* Packet_NADM */
                        pEventData++;
                        crtDataLen++;
                    }

                    if ((filter & BIT4) != 0U)
                    {
                        /* Data includes Packet RSSI */
                        hciCsStoreBytesInTofBuffer(pDstAppBuffer, pEventData, (int)CS_RSSI_SIZE);
    #if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
                        if ((int8_t)(*pEventData) != gRssiNotAvailable_c)
                        {
                            /* Count RSSI if available */
                            pDstAppBuffer->aRssiValue[pDstAppBuffer->rssiStepNo] = (int8_t)(*pEventData);
                            pDstAppBuffer->rssiStepNo++;
                        }
    #endif /* gAppParseRssiInfo_d */
                        pEventData++;
                        crtDataLen++;
                    }
                    else
                    {
                        uint8_t rssi = 0U;
                        hciCsStoreBytesInTofBuffer(pDstAppBuffer, &rssi, (int)CS_RSSI_SIZE);
                    }

                    if ((filter & BIT5) != 0U)
                    {
                        int16_t temp1 = 0;

                        /* Data includes ToA_ToD_Initiator/ToD_ToA_Reflector information */
                        FLib_MemCpy(&ts_diff_hci, pEventData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
                        pEventData += sizeof(uint16_t);
                        crtDataLen += (uint16_t)sizeof(uint16_t);

                        /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
                        temp1 = ts_diff_hci/2;
                        ts_diff = (uint32_t)(temp1); /* HCI reports half ns, application expects ns in Tof Buffer */
                        ts_diff &= 0x00FFFFU;
                        ts_diff |= (quality & 0x0FU) << gTimeStampDiffSize_c;
                        hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)CS_TS_SIZE);
                    }

                    if ((filter & BIT6) != 0U)
                    {
                        /* Data includes Packet Antenna */
                        pEventData++;
                        crtDataLen++;
                    }

                    /* Check if data includes Packet_PCT1, Packet_PCT2 information (3 octets each) */
                    if (AppLocalization_GetRttSoundingSupport() == TRUE)
                    {
                        if ((filter & BIT7) != 0U)
                        {
                            /* Data includes Packet_PCT1 */
                            pEventData += gPacket_PCTSize_c;
                            crtDataLen += gPacket_PCTSize_c;
                        }

                        if ((filter & BIT8) != 0U)
                        {
                            /* Data includes Packet_PCT2 */
                            pEventData += gPacket_PCTSize_c;
                            crtDataLen += gPacket_PCTSize_c;
                        }
                    }
                }
                break;

                case 2:
                {
                    /* Tone record */
                    step++;
                    antPermIndex = 0;

                    if ((filter & BIT2) != 0U)
                    {
                        /* Data includes Antenna Permutation Index */
                        antPermIndex = *pEventData++; /* Antenna_Permutation_Index */
                        crtDataLen++;
                    }
                    pAntIndex = &maAntPermNAp[antPermIndex][0];
                    antIdx = 0U;
                    FLib_MemSet(iq_dec, 0, ISP_MAX_NO_ANTENNAS * sizeof(int32_t));
                    FLib_MemSet(tqi, 0U, ISP_MAX_NO_ANTENNAS);

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
                                pEventData += gTone_PCTSize_c + sizeof(uint8_t);
                                crtDataLen += gTone_PCTSize_c + sizeof(uint8_t);
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
                                uint32_t temp1 = ((uint32_t)pEventData[2])<<BIT4 | ((uint32_t)pEventData[1])<<BIT3
                                      | ((uint32_t)pEventData[0]);
                                int32_t iq = (int32_t)temp1;

                                /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                                uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                                                  ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                                iq_dec[antIdx] = (int32_t)temp2;
                                pEventData += gTone_PCTSize_c;
                                crtDataLen += gTone_PCTSize_c;
                            }

                            if ((filter & BIT4) != 0U)
                            {
                                /* Data includes Tone_Quality_Indication */
                                tqi[antIdx] = *pEventData;
                                pEventData += sizeof(uint8_t);
                                crtDataLen++;
                            }
                        }
                    }

                    for (uint8_t idx = 0U; idx < pDstAppBuffer->mciq_data.n_ap; idx++)
                    {
                        hciCsStoreBytesInIqBuffer(pDstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
                        hciCsStoreBytesInIqBuffer(pDstAppBuffer, &tqi[idx], 1);
                    }

                    pDstAppBuffer->mciq_data.nbSteps ++;
                }
                break;

                case 3:
                {
                    int16_t temp = 0;

                    /* ToF+Tone record */
                    step ++;

                    if ((filter & BIT2) != 0U)
                    {
                        /* Data includes Packet Quality*/
                        quality = (uint32_t)(*pEventData++);
                        crtDataLen++;
                    }

                    if ((filter & BIT3) != 0U)
                    {
                        /* Data includes Packet NADM */
                        pEventData++;
                        crtDataLen++;
                    }

                    if ((filter & BIT4) != 0U)
                    {
                        /* Data includes Packet RSSI */
                        hciCsStoreBytesInTofBuffer(pDstAppBuffer, pEventData, (int)CS_RSSI_SIZE);
    #if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
                        if ((int8_t)(*pEventData) != gRssiNotAvailable_c)
                        {
                            /* Count RSSI if available */
                            pDstAppBuffer->aRssiValue[pDstAppBuffer->rssiStepNo] = (int8_t)(*pEventData);
                            pDstAppBuffer->rssiStepNo++;
                        }
    #endif /* gAppParseRssiInfo_d */
                        pEventData++;
                        crtDataLen++;
                    }

                    if ((filter & BIT5) != 0U)
                    {
                        /* Data includes ToA_ToD_Initiator/ToD_ToA_Reflector information */
                        FLib_MemCpy(&ts_diff_hci, pEventData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
                        pEventData += sizeof(uint16_t);
                        crtDataLen += (uint16_t)sizeof(uint16_t);

                        /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
                        if ((filter & BIT2) != 0U)
                        {
                            temp = ts_diff_hci/2;
                            ts_diff = (uint32_t)(temp); /* HCI reports half ns, application expects ns in Tof Buffer */
                            ts_diff &= 0x00FFFFU;
                            ts_diff |= (quality & 0x0FU) << gTimeStampDiffSize_c;
                            hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)CS_TS_SIZE);
                        }
                    }

                    if ((filter & BIT6) != 0U)
                    {
                        /* Data includes Packet Antenna */
                        pEventData++;
                        crtDataLen++;
                    }

                    /* Check if data includes Packet_PCT1, Packet_PCT2 information (3 octets each) */
                    if (AppLocalization_GetRttSoundingSupport() == TRUE)
                    {
                        if ((filter & BIT7) != 0U)
                        {
                            /* Data includes Packet_PCT1 */
                            pEventData += gPacket_PCTSize_c;
                            crtDataLen += gPacket_PCTSize_c;
                        }

                        if ((filter & BIT8) != 0U)
                        {
                            /* Data includes Packet_PCT2 */
                            pEventData += gPacket_PCTSize_c;
                            crtDataLen += gPacket_PCTSize_c;
                        }
                    }

                    antPermIndex = 0;

                    if ((filter & BIT9) != 0U)
                    {
                        /* Data includes Antenna Permutation Index */
                        antPermIndex = *pEventData++; /* Antenna_Permutation_Index */
                        crtDataLen++;
                    }
                    pAntIndex = &maAntPermNAp[antPermIndex][0];
                    antIdx = 0U;
                    FLib_MemSet(iq_dec, 0, ISP_MAX_NO_ANTENNAS * sizeof(int32_t));
                    FLib_MemSet(tqi, 0U, ISP_MAX_NO_ANTENNAS);

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
                                pEventData += gTone_PCTSize_c + sizeof(uint8_t);
                                crtDataLen += gTone_PCTSize_c + sizeof(uint8_t);
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
                                uint32_t temp1 = ((uint32_t)pEventData[2])<<BIT4 | ((uint32_t)pEventData[1])<<BIT3
                                      | ((uint32_t)pEventData[0]);
                                int32_t iq = (int32_t)temp1;

                                /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                                uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                                                  ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                                iq_dec[antIdx] = (int32_t)temp2;
                                pEventData += gTone_PCTSize_c;
                                crtDataLen += gTone_PCTSize_c;
                            }

                            if ((filter & BIT11) != 0U)
                            {
                                /* Data includes Tone_Quality_Indication */
                                tqi[antIdx] = *pEventData;
                                pEventData += sizeof(uint8_t);
                                crtDataLen++;
                            }
                        }
                    }

                    for (uint8_t idx = 0U; idx < pDstAppBuffer->mciq_data.n_ap; idx++)
                    {
                        hciCsStoreBytesInIqBuffer(pDstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
                        hciCsStoreBytesInIqBuffer(pDstAppBuffer, &tqi[idx], 1);
                    }

                    pDstAppBuffer->mciq_data.nbSteps ++;
                }
                break;

                default:
                {
                    /* mode not yet implemented, skip data */
                }
                break;
            }
        }

        /* Count step for the current subevent whether it was aborted or not */
        pRemoteData->crtNumSteps++;

        if (pRemoteData->crtNumSteps ==
            pRemoteData->aSubEventData[pRemoteData->subeventIndex].subevtHeader.numStepsReported)
        {
            /* move on to the next subevent */
            pRemoteData->subeventIndex++;
            pRemoteData->crtNumSteps = 0U;

            /* Move on to the next subevent */
            if (crtDataLen < pRemoteData->totalSentRcvDataIndex)
            {
                RasClient_ParseReceivedSubeventHeader(pRemoteData->deviceId, pEventData);
                pEventData += ((uint16_t)sizeof(rasSubeventDataHeader_t));
                crtDataLen += ((uint16_t)sizeof(rasSubeventDataHeader_t));
            }
        }
    }

    /* Populate additional fields in pDstAppBuffer */

    /* Total number of steps */
    pDstAppBuffer->csData.step_nb = (uint16_t)step;

    /* Start ACL count */
    pDstAppBuffer->csData.startAclCnt =
            pRemoteData->aSubEventData[pDstAppBuffer->csData.subevt_nb].subevtHeader.startACLConnEvent;

    /* For every subevent */
    for (uint8_t index = 0; index <= pRemoteData->subeventIndex; index++)
    {
        /* The stop index is the total number of previous steps */
        pDstAppBuffer->csData.subevtStopIdx[index] =
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
    pDstAppBuffer->csData.subevt_nb =  pRemoteData->subeventIndex + 1U;
}
#endif /* defined (gRasRREQ_d) && (gRasRREQ_d == 1U) */
#else
/*! *********************************************************************************
 * \brief        Uncompress BTCS Ranging Data for a given device
 *
 * \param[in]    pSrcResultBuffer    Pointer to uncompressed data
 * \param[in]    pDstAppBuffer       Pointer to destination buffer for uncompressed data
 ********************************************************************************** */
static void AppLocalizationAlgo_UncompressRemoteResponseL2CAP
(
    void*          pSrcResultBuffer,
    void*          pSrcLocalBuffer,
    csAppData_t* pDstAppBuffer
)
{
    uint16_t totalStepCounter = 0U;
    int32_t iq_dec[ISP_MAX_NO_ANTENNAS] = {0};
    uint8_t tqi[ISP_MAX_NO_ANTENNAS] = {0u};
    rasMeasurementData_t *pRemoteData = (rasMeasurementData_t*)pSrcResultBuffer;
    rasMeasurementData_t *pLocalData = (rasMeasurementData_t*)pSrcLocalBuffer;
    pDstAppBuffer->mciq_data.n_ap = pRemoteData->numAntennaPaths;
    pDstAppBuffer->csData.subevt_nb = 0U;
    uint8_t* pEventData = pRemoteData->pData;
    uint8_t* pEvtDataLocal = pLocalData->pData;

    uint8_t step = 0U;
    uint16_t crtDataLen = 0U;
    uint32_t ts_diff = 0U;
    int16_t ts_diff_hci = 0U;
    uint8_t antPermIndex = 0U;
    uint8_t const* pAntIndex;
    uint8_t antIdx = 0U;
    uint8_t mode = 0U;

    while (crtDataLen < pRemoteData->totalSentRcvDataIndex)
    {
        mode = *pEventData++;
        crtDataLen++;
        /* Skip local mode */
        pEvtDataLocal++;

        /* Make sure the mode is valid and that local and remote mode match */
        assert(mode <= gCsStepMode3_c);

        pDstAppBuffer->csData.modeMap[step] = mode;

        /* Skip step channel and length from local buffer */
        pEvtDataLocal = &pEvtDataLocal[2U];

        switch(mode)
        {
            case (uint8_t)gCsStepMode0_c:
            {
                pEventData = &pEventData[gMode0DataSize_c];
                crtDataLen += gMode0DataSize_c;
                pEvtDataLocal = &pEvtDataLocal[gMode0DataSize_c];

                if (mGlobalRangeSettings.role == gCsRoleReflector_c)
                {
                    /* Data includes Measured_Freq_Offset information */
                    pEventData += sizeof(uint16_t);
                    crtDataLen += (uint16_t)sizeof(uint16_t);
                }
                else
                {
                    pEvtDataLocal = &pEvtDataLocal[sizeof(uint16_t)];
                }
            }
            break;

            case (uint8_t)gCsStepMode1_c:
            {
                /* ToF record */
                int16_t temp1 = 0U;
                uint32_t quality = 0U;

                step++;
                pDstAppBuffer->tof_data.nbSteps ++;

                /* Data includes Packet Quality*/
                quality = (uint32_t)(*pEventData++);
                crtDataLen++;

                /* Data includes Packet NADM */
                hciCsStoreBytesInTofBuffer(pDstAppBuffer, pEventData, (int)CS_NADM_SIZE); /* Packet_NADM */
                pEventData++;
                crtDataLen++;

                /* Data includes Packet RSSI */
                hciCsStoreBytesInTofBuffer(pDstAppBuffer, pEventData, (int)CS_RSSI_SIZE);
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
                if ((int8_t)(*pEventData) != gRssiNotAvailable_c)
                {
                    /* Count RSSI if available */
                    pDstAppBuffer->aRssiValue[pDstAppBuffer->rssiStepNo] = (int8_t)(*pEventData);
                    pDstAppBuffer->rssiStepNo++;
                }
#endif /* gAppParseRssiInfo_d */
                pEventData++;
                crtDataLen++;

                /* Data includes ToA_ToD_Initiator/ToD_ToA_Reflector information */
                FLib_MemCpy(&ts_diff_hci, pEventData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
                pEventData += sizeof(uint16_t);
                crtDataLen += (uint16_t)sizeof(uint16_t);

                /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
                temp1 = ts_diff_hci/2;
                ts_diff = (uint32_t)(temp1); /* HCI reports half ns, application expects ns in Tof Buffer */
                ts_diff &= 0x00FFFFU;
                ts_diff |= (quality & 0x0FU) << gTimeStampDiffSize_c;
                hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)CS_TS_SIZE);

                /* Data includes Packet Antenna */
                pEventData++;
                crtDataLen++;

                /* Move on to the next step */
                pEvtDataLocal = &pEvtDataLocal[gMode1DataSize_c];
            }
            break;

            case (uint8_t)gCsStepMode2_c:
            {
                /* Tone record */
                uint8_t quality = 0U;
                step++;
                antPermIndex = 0U;

                /* Get local Antenna Permutation Index */
                antPermIndex = *pEvtDataLocal++;
                pAntIndex = &maAntPermNAp[antPermIndex][0];
                antIdx = 0U;
                FLib_MemSet(iq_dec, 0, ISP_MAX_NO_ANTENNAS * sizeof(int32_t));
                FLib_MemSet(tqi, 0U, ISP_MAX_NO_ANTENNAS);

                /* Extract quality - 2 bits per antenna path, up to 4 antenna paths, ordered*/
                quality = *pEventData++;

                /* Num_Antenna_Paths are reported by the remote peer in BTCS data format */
                /* Re-order per antenna path index */
                for (uint8_t idx = 0U; idx < pRemoteData->numAntennaPaths; idx++)
                {
                    antIdx = pAntIndex[idx];

                    /* Data includes Tone_PCT information */
                    uint32_t temp1 = ((uint32_t)pEventData[2])<<BIT4 | ((uint32_t)pEventData[1])<<BIT3
                          | ((uint32_t)pEventData[0]);
                    int32_t iq = (int32_t)temp1;

                    /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                    uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                                      ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                    iq_dec[antIdx] = (int32_t)temp2;
                    pEventData += gTone_PCTSize_c;
                    crtDataLen += gTone_PCTSize_c;
                    pEvtDataLocal += (gTone_PCTSize_c+1U); /* also account for quality field */

                    /* Extract quality information */
                    tqi[antIdx] = (uint8_t)(quality << (2u*antIdx));
                    tqi[antIdx] &= 0x03U; /* keep only the first 2 bits */
                }

                for (uint8_t idx = 0U; idx < pDstAppBuffer->mciq_data.n_ap; idx++)
                {
                    hciCsStoreBytesInIqBuffer(pDstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
                    hciCsStoreBytesInIqBuffer(pDstAppBuffer, &tqi[idx], 1);
                }

                /* Skip extension tone in local data */
                pEvtDataLocal += (gTone_PCTSize_c+1U);

                pDstAppBuffer->mciq_data.nbSteps ++;
            }
            break;

            case (uint8_t)gCsStepMode3_c:
            {
                int16_t temp = 0;
                uint32_t quality = 0U;
                uint8_t pctQuality = 0u;

                /* ToF+Tone record */
                step ++;

                /* Data includes Packet Quality*/
                quality = (uint32_t)(*pEventData++);
                crtDataLen++;

                /* Data includes Packet NADM */
                pEventData++;
                crtDataLen++;

                /* Data includes Packet RSSI */
                hciCsStoreBytesInTofBuffer(pDstAppBuffer, pEventData, (int)CS_RSSI_SIZE);
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1U)
                if ((int8_t)(*pEventData) != gRssiNotAvailable_c)
                {
                    /* Count RSSI if available */
                    pDstAppBuffer->aRssiValue[pDstAppBuffer->rssiStepNo] = (int8_t)(*pEventData);
                    pDstAppBuffer->rssiStepNo++;
                }
#endif /* gAppParseRssiInfo_d */
                pEventData++;
                crtDataLen++;

                /* Data includes ToA_ToD_Initiator/ToD_ToA_Reflector information */
                FLib_MemCpy(&ts_diff_hci, pEventData, sizeof(uint16_t)); /* Time Diff signed Q16, 2 bytes */
                pEventData += sizeof(uint16_t);
                crtDataLen += (uint16_t)sizeof(uint16_t);

                /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
                temp = ts_diff_hci/2;
                ts_diff = (uint32_t)(temp); /* HCI reports half ns, application expects ns in Tof Buffer */
                ts_diff &= 0x00FFFFU;
                ts_diff |= (quality & 0x0FU) << gTimeStampDiffSize_c;
                hciCsStoreBytesInTofBuffer(pDstAppBuffer, (uint8_t *)&ts_diff, (int)CS_TS_SIZE);

                /* Data includes Packet Antenna */
                pEventData++;
                crtDataLen++;

                antPermIndex = 0U;

                /* Get local Antenna Permutation Index */
                antPermIndex = *pEvtDataLocal++;
                pAntIndex = &maAntPermNAp[antPermIndex][0];
                antIdx = 0U;
                FLib_MemSet(iq_dec, 0, ISP_MAX_NO_ANTENNAS * sizeof(int32_t));
                FLib_MemSet(tqi, 0U, ISP_MAX_NO_ANTENNAS);

                /* Extract quality - 2 bits per antenna path, up to 4 antenna paths, ordered*/
                pctQuality = *pEventData++;

                /* Num_Antenna_Path + 1 are reported by the firmware, but discard last one */
                /* Re-order per antenna path index */
                for (uint8_t idx = 0U; idx < pRemoteData->numAntennaPaths; idx++)
                {
                    antIdx = pAntIndex[idx];

                    /* Data includes Tone_PCT information */
                    uint32_t temp1 = ((uint32_t)pEventData[2])<<BIT4 | ((uint32_t)pEventData[1])<<BIT3
                          | ((uint32_t)pEventData[0]);
                    int32_t iq = (int32_t)temp1;

                    /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                    uint32_t temp2 = (((uint32_t)(((uint32_t)iq)&0xFFFU) << gIQSampleSize_c) |
                                      ((uint32_t)(((uint32_t)iq)>>gIQSampleSize_c)&0xFFFU));
                    iq_dec[antIdx] = (int32_t)temp2;
                    pEventData += gTone_PCTSize_c;
                    crtDataLen += gTone_PCTSize_c;

                    /* Extract quality information */
                    tqi[antIdx] = (uint8_t)(pctQuality << (2u*antIdx));
                    tqi[antIdx] &= 0x03U; /* keep only the first 2 bits */
                }

                for (uint8_t idx = 0U; idx < pDstAppBuffer->mciq_data.n_ap; idx++)
                {
                    hciCsStoreBytesInIqBuffer(pDstAppBuffer, (uint8_t *)&iq_dec[idx], 3);
                    hciCsStoreBytesInIqBuffer(pDstAppBuffer, &tqi[idx], 1);
                }

                /* Skip extension tone in local data */
                pEvtDataLocal += (gTone_PCTSize_c+1U);

                pDstAppBuffer->mciq_data.nbSteps ++;
            }
            break;

            default:
            {
                /* mode not yet implemented, skip data */
            }
            break;
        }

        /* Increase parsed number of steps for the current subevent */
        pRemoteData->crtNumSteps++;

        if (pRemoteData->crtNumSteps ==
            pRemoteData->aSubEventData[pRemoteData->subeventIndex].subevtHeader.numStepsReported)
        {
            /* Move on to the next subevent */
            pRemoteData->subeventIndex++;
            pRemoteData->crtNumSteps = 0U;
        }
    }

    /* Populate additional fields in pDstAppBuffer */

    /* Total number of steps */
    pDstAppBuffer->csData.step_nb = (uint16_t)step;

    /* Start ACL count */
    pDstAppBuffer->csData.startAclCnt =
            pRemoteData->aSubEventData[pDstAppBuffer->csData.subevt_nb].subevtHeader.startACLConnEvent;

    /* For every subevent */
    for (uint8_t index = 0U; index <= pRemoteData->subeventIndex; index++)
    {
        /* The stop index is the total number of previous steps */
        pDstAppBuffer->csData.subevtStopIdx[index] =
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
    pDstAppBuffer->csData.subevt_nb =  pRemoteData->subeventIndex + 1U;
}
#endif

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

/*! *********************************************************************************
 *\fn           void isp_mciq_ranging_compute(isp_meas_response_t *meas_response,
 *                                  mciq_result_t *mciq_result,
 *                                  engine_config_t *engine_config);
 *
 * \brief       Run distance estimation algorithm(s) and store computation results in mciq_result
 *
 * \param[in]   meas_response       Pointer to measurement response data
 * \param[in]   mciq_result         Pointer to mciq result data
 * \param[in]   engine_config       Pointer to ranging engine configuration
 *
 *\retval       none
 ********************************************************************************** */
static void isp_mciq_ranging_compute
(
    isp_meas_response_t *meas_response,
    mciq_result_t *mciq_result,
    engine_config_t *engine_config
)
{
    mciq_data_t *init_data = &meas_response->mciq_data[0];
    mciq_data_t *refl_data = &meas_response->mciq_data[1];
    int16_t *iq1, *iq2;
    int16_t *meas_iq1, *meas_iq2;
#if defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)
    uint32_t m, index = 0;
#endif

    uint32_t nb_valid[ISP_MAX_NO_ANTENNAS] = {0};
    /* mask of CS channels that have been actually used for RTP measurements */
    uint32_t freqMask[(XCVR_F_RANGE/32)+1];
    /* mask of CS channels that have been actually used for RTP measurements and for which TQI was GOOD */
    uint32_t tqi1Mask[(XCVR_F_RANGE/32)+1];
    uint32_t tqi2Mask[(XCVR_F_RANGE/32)+1];

    /* Allocate a single buffer to store combined phases and IQs for all devices. freed in isp_measurement_stop */
    /* For each device : 2 IQ buff (each 2*uint16_t*channels*n_ap) */
    uint32_t item_size = (2U * sizeof(int16_t) * gCsChannelsNb_c * (uint32_t)engine_config->n_ap);
    uint8_t *buf_start = (uint8_t *)MEM_BufferAlloc(2U*item_size);

    if (buf_start == NULL)
    {
        return;
    }

    FLib_MemSet(buf_start, 0U, 2U * item_size);
    iq1 = (int16_t *)(void *)buf_start;
    iq2 = (int16_t *)(void *)(buf_start + item_size);

    /* Allocate a buffer to keep the measured IQs from a CS pocedure */
    union {
        uint32_t u32;
        uint16_t u16;
    } meas_iq_item_size = {0U};
    meas_iq_item_size.u16  = 2U * sizeof(int16_t) * meas_response->cs_data->step_nb * engine_config->n_ap;
    uint8_t *meas_buf_start = (uint8_t *)MEM_BufferAlloc(2U * meas_iq_item_size.u32);

    if (meas_buf_start == NULL)
    {
        (void)MEM_BufferFree(buf_start);
        return;
    }

    FLib_MemSet(meas_buf_start, 0U, 2U * meas_iq_item_size.u32);
    meas_iq1 = (int16_t *)(void *)meas_buf_start;
    meas_iq2 = (int16_t *)(void *)(meas_buf_start + meas_iq_item_size.u32);

    mciq_result->cde_dqi = 0;
    mciq_result->cde_fp = 0;

    /* Init frequency mask */
    for (uint8_t i= 0; i < (XCVR_F_RANGE/32U + 1U); i++)
    {
        freqMask[i] = 0;
        tqi1Mask[i] = 0;
        tqi2Mask[i] = 0;
    }

    /* Extract IQs */
    isp_mciq_measurement_unpack_iqs(meas_response->cs_data,
                                                   init_data,
                                                   refl_data,
                                                   iq1,
                                                   iq2,
                                                   meas_iq1,
                                                   meas_iq2,
                                                   freqMask,
                                                   tqi1Mask,
                                                   tqi2Mask,
                                                   nb_valid);

    /* CDE is enabled */
#if defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)
    if((engine_config->mciq_algo_flags & eMciqAlgoEmbedCDE) != 0U)
    {
        dm_cde_estimate_t estimate = {0};
        uint32_t cde_threshold = engine_config->n_ap == 1U ? engine_config->cde_threshold : engine_config->cde_div_threshold;
        int32_t d_min = 1024 * Q10_SCALING_FACTOR;   /* in Q21.10 format */
        bool success[ISP_MAX_NO_ANTENNAS];

        for(m = 0; m < engine_config->n_ap; m++)
        {

            success[m] = 0;

            /* Run the algorithm */
            success[m] = dm_cde_distance_estimation(iq1 + (2U*m*gCsChannelsNb_c),
                                                    iq2 + (2U*m*gCsChannelsNb_c),
                                                    NULL,
                                                    freqMask,
                                                    gCsChannelsNb_c,
                                                    DM_FREQUENCY_RASTER,
                                                    0,
                                                    cde_threshold,
                                                    &estimate);

            if (success[m])
            {
                /* Apply zero-distance compensation for CDE distance */
                mciq_result->cde_dist_array[m] = estimate.distance_estimate; /*- isp_compute_calibration(eMeasurementOptionMCIQ, init_bid, refl_bid);*/
                mciq_result->cde_dqi_array[m] = estimate.dqi;
                mciq_result->cde_nb_valid[m] = (uint16_t)nb_valid[m];
            }
            else
            {
                mciq_result->cde_dist_array[m] = 0;
                mciq_result->cde_dqi_array[m] = 0;
                mciq_result->cde_nb_valid[m] = 0;
            }
        } /* end for m */

        for(m = 0; m < engine_config->n_ap; m++)
        {
            if (success[m])
            {
                if (mciq_result->cde_dist_array[m] <= d_min)
                {
                    /* choose the antenna path with smallest distance estimate */
                    d_min = mciq_result->cde_dist_array[m];
                    index = m;
                }
            }
        }

        mciq_result->cde_fp = mciq_result->cde_dist_array[index];
        mciq_result->cde_dqi = mciq_result->cde_dqi_array[index]; /* Final DQI */
        mciq_result->nb_valid_freq = mciq_result->cde_nb_valid[index];
    }
#endif /* gAppUseCDEAlgorithm_d */
    (void)MEM_BufferFree(buf_start);

#if defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)
    if((engine_config->mciq_algo_flags & eMciqAlgoEmbedRADE) != 0U)
    {
        /* Run RADE algorithm */
        rade_result_type_t radeStatus;
        mciq_result->rade_error_flag = 0U;
        uint32_t tqiMask[(XCVR_F_RANGE/32)+1];
        for (uint32_t i = 0u; i < (XCVR_F_RANGE/32U)+1U; i++)
        {
            tqiMask[i] = tqi1Mask[i] & tqi2Mask[i];
        }
        uint8_t *subevtDoneStatus_init = mGlobalRangeSettings.role == gCsRoleInitiator_c ? meas_response->cs_data->subevtDoneStatusLocal : meas_response->cs_data->subevtDoneStatusRemote;
        uint8_t *subevtDoneStatus_refl = mGlobalRangeSettings.role == gCsRoleInitiator_c ? meas_response->cs_data->subevtDoneStatusRemote : meas_response->cs_data->subevtDoneStatusLocal;
        float_rade_t radeResReserved;
        rade_cs_para_t radeCsPara;
        rade_result_t radeResult;
        rade_data_t radeData;
        radeCsPara.step_nb                = meas_response->cs_data->step_nb;
        radeCsPara.startAclCnt            = meas_response->cs_data->startAclCnt;
        radeCsPara.mode0_nb               = meas_response->cs_data->mode0_nb;
        radeCsPara.subevt_nb              = meas_response->cs_data->subevt_nb;
        radeCsPara.t_fcs                  = meas_response->cs_data->t_fcs;
        radeCsPara.t_ip1                  = meas_response->cs_data->t_ip1;
        radeCsPara.t_ip2                  = meas_response->cs_data->t_ip2;
        radeCsPara.t_pm                   = meas_response->cs_data->t_pm;
        radeCsPara.t_sw                   = meas_response->cs_data->t_sw;
        radeCsPara.channelMap             = meas_response->cs_data->channelMap;
        radeCsPara.modeMap                = meas_response->cs_data->modeMap;
        radeCsPara.subevtStopIdx          = meas_response->cs_data->subevtStopIdx;
        radeCsPara.subevtConnEvent        = meas_response->cs_data->subevtConnEvent;
        radeCsPara.main_mode_repeat       = meas_response->cs_data->main_mode_repeat;
        radeCsPara.rtt_type               = meas_response->cs_data->rtt_type;
        radeCsPara.rtt_phy                = meas_response->cs_data->phy;
        radeCsPara.main_mode_type         = meas_response->cs_data->main_mode_type;
        radeCsPara.sub_mode_type          = meas_response->cs_data->sub_mode_type;
        radeCsPara.connInterval           = meas_response->cs_data->conn_interval;
        radeCsPara.refPowerLevel_init     = meas_response->cs_data->subevtRefPowerLevelInit;
        radeCsPara.refPowerLevel_refl     = meas_response->cs_data->subevtRefPowerLevelRefl;
        radeCsPara.subevtDoneStatus_init  = subevtDoneStatus_init;
        radeCsPara.subevtDoneStatus_refl  = subevtDoneStatus_refl;
        radeResult.rng_est = &mciq_result->rade_dist;
        radeResult.rng_est_qi = &mciq_result->rade_dqi;
        radeResult.reserved = &radeResReserved;
        radeResult.rng_trk = &mciq_result->rade_dist_trk;
        radeData.pct_i = meas_iq1;
        radeData.pct_r = meas_iq2;
        radeData.tqi_mask = tqiMask;
        radeData.chan_mask = freqMask;
        radeData.n_ap = engine_config->n_ap;
        rade_para_t radePara = {.radeMode = kRadeNormal, .distBias = 0.0f, .ceHeap_id = g_ceHeap_id};
#ifdef LCE_KW47_MCXW72
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)
        /* LCE is not able to compute while CORE0 is in deep sleep */
        (void)PWR_SetLowPowerModeConstraint(PWR_WFI);
#endif /* defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0) */
#endif /* LCE_KW47_MCXW72 */
        radeStatus = pde_rade(&radeData, meas_response->cs_data->csAlgoBuf, &radeCsPara, &radeResult, &radePara);
#ifdef LCE_KW47_MCXW72
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)
        (void)PWR_ReleaseLowPowerModeConstraint(PWR_WFI);
#endif /* defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0) */
#endif /* LCE_KW47_MCXW72 */
        if (kRadeSuccess != radeStatus)
        {
            mciq_result->rade_error_flag = (uint8_t)radeStatus;
            mciq_result->rade_dist = null_RNG;
            mciq_result->rade_dqi = 0.0f;
        }
    }
#endif /* gAppUseRADEAlgorithm_d */

    (void)MEM_BufferFree(meas_buf_start);
}

/*! *********************************************************************************
 *\fn           void isp_tof_ranging_compute(isp_meas_response_t *meas_response,
 *                                  tof_result_t *tof_result);
 *
 * \brief       Store computation result in tof_derived
 *
 * \param[in]   meas_response       Pointer to measurement response data
 * \param[in]   tof_result          Pointer to tof result data
 *
 *\retval       none
 ********************************************************************************** */
static void isp_tof_ranging_compute
(
    isp_meas_response_t *meas_response,
    tof_result_t *tof_result
)
{
    uint8_t *init_ts = meas_response->tof_data[0].ts;
    uint8_t *refl_ts = meas_response->tof_data[1].ts;
    uint32_t nb_steps = meas_response->tof_data[0].nbSteps;
    int32_t sum_rtt_ns = 0;
    uint16_t nb_steps_valid = 0U;

    for (uint32_t tone = 0; tone < nb_steps; tone++)
    {
        uint16_t init_ns = 0U;
        uint16_t refl_ns = 0U;
        uint8_t init_aa_quality = 0U;
        uint8_t refl_aa_quality = 0U;

        init_ts += CS_NADM_SIZE + CS_RSSI_SIZE;
        refl_ts += CS_NADM_SIZE + CS_RSSI_SIZE;
        CS_GET_RTT_TS_DIFF(init_ts, init_ns, init_aa_quality);
        CS_GET_RTT_TS_DIFF(refl_ts, refl_ns, refl_aa_quality);

        /* Check that both quality indicator are valid */
        if ((init_aa_quality | refl_aa_quality) == 0U)
        {
            sum_rtt_ns += (int32_t)init_ns - (int32_t)refl_ns;
            nb_steps_valid++;
        }
        init_ts += CS_TS_SIZE;
        refl_ts += CS_TS_SIZE;
    }

    tof_result->dm_sr = 0;
    if (nb_steps_valid != 0U) {
        /*  average distance expected in fixed-point s15.16 (meters) */
        tof_result->dm_ad = (int32_t)(sum_rtt_ns/(int16_t)nb_steps_valid);
        tof_result->dm_ad = (int32_t)(tof_result->dm_ad * 9830); /* RTT/2 * 0.3m/ns * 2^16 (0.3*2^16/2 is 9830.4) */
        tof_result->dm_sr = (uint8_t)(((uint32_t)nb_steps_valid * 100U)/nb_steps);
    }
}

/*! *********************************************************************************
 *\fn           void isp_mciq_measurement_unpack_iqs(cs_data_t *cs_data,
 *                                  mciq_data_t *data1,
 *                                  mciq_data_t *data2,
 *                                  int16_t *pIQout1,
 *                                  int16_t *pIQout2,
 *                                  int16_t *pMeasIQout1,
 *                                  int16_t *pMeasIQout2,
 *                                  uint32_t freqMask[],
 *                                  uint32_t tqi1Mask[],
 *                                  uint32_t tqi2Mask[],
 *                                  uint32_t nbValid[]);
 *
 * \brief       Unpack IQs from mciq measurement data
 *
 * \param[in]   cs_data             Pointer to CS data
 * \param[in]   data1               Pointer to initiator data
 * \param[in]   data2               Pointer to reflector data
 * \param[out]  pIQout1             Pointer to IQ1 data
 * \param[out]  pIQout2             Pointer to IQ2 data
 * \param[out]  pMeasIQout1         Pointer to MeasIQ1 data
 * \param[out]  pMeasIQout2         Pointer to MeasIQ2 data
 * \param[out]  freqMask            Mask of CS channels used for RTP measurements
 * \param[out]  tqi1Mask            Mask of CS channels used for RTP measurements; TQI was GOOD
 * \param[out]  tqi2Mask            Mask of CS channels used for RTP measurements; TQI was GOOD
 * \param[out]  nbValid             Number of mode 2 and mode 3 steps that contain valid data
 *
 *\retval       none
 ********************************************************************************** */
static void isp_mciq_measurement_unpack_iqs
(
    cs_data_t *cs_data,
    mciq_data_t *data1,
    mciq_data_t *data2,
    int16_t *pIQout1,
    int16_t *pIQout2,
    int16_t *pMeasIQout1, 
    int16_t *pMeasIQout2,
    uint32_t freqMask[],
    uint32_t tqi1Mask[],
    uint32_t tqi2Mask[],
    uint32_t nbValid[]
)
{
    uint32_t i;
    uint8_t *pIQ;
    uint8_t *pIQin1;
    uint8_t *pIQin2;
    uint32_t channel;
    int16_t iSample1, qSample1;
    int16_t iSample2, qSample2;
    int16_t tqi1, tqi2;
    uint32_t apIdx, apNb;

    /* CS IQs are 2 * 12 bits compressed, I is located in 12bits MSB, all antenna paths packed by step */
    apNb = data1->n_ap;

    for (apIdx = 0; apIdx < apNb; apIdx++)
    {
        pIQin1 = data1->iq;
        pIQin2 = data2->iq;
        nbValid[apIdx] = 0;
        for (i = 0U; i < cs_data->step_nb; i++)
        {
            if (cs_data->modeMap[i] == 2U || cs_data->modeMap[i] == 3U)
            {
                uint16_t qSample1u, qSample2u, iSample1u, iSample2u;
                channel = cs_data->channelMap[i];
                assert(channel < gCsChannelsNb_c);
                pIQ = pIQin1 + (IQ_SIZE+TQI_SIZE)*apIdx;
                qSample1u = (uint16_t)pIQ[0] | ((uint16_t)pIQ[1] & 0xFU) << 8U;
                qSample1u = ISP_MCIQ_SIGN_EXTEND_12_16((uint16_t)qSample1u);
                qSample1 = (int16_t)qSample1u;
                iSample1u = (((uint16_t)pIQ[1] & 0xF0U) >> 4U) | (pIQ[2] << 4U);
                iSample1u = ISP_MCIQ_SIGN_EXTEND_12_16((uint16_t)iSample1u);
                iSample1 = (int16_t)iSample1u;
                pIQ += IQ_SIZE;
                tqi1 = (int16_t)*pIQ;
                pIQ = pIQin2 + (IQ_SIZE+TQI_SIZE)*apIdx;
                qSample2u = (uint16_t)pIQ[0] | ((uint16_t)pIQ[1] & 0xFU) << 8U;
                qSample2u = ISP_MCIQ_SIGN_EXTEND_12_16((uint16_t)qSample2u);
                qSample2 = (int16_t)qSample2u;
                iSample2u = (((uint16_t)pIQ[1] & 0xF0U) >> 4U) | (pIQ[2] << 4U);
                iSample2u = ISP_MCIQ_SIGN_EXTEND_12_16((uint16_t)iSample2u);
                iSample2 = (int16_t)iSample2u;
                pIQ += IQ_SIZE;
                tqi2 = (int16_t)*pIQ;
                
                /* Store unpacked measured IQs */
                pMeasIQout1[i*2U] = iSample1;
                pMeasIQout1[i*2U+1U] = qSample1;
                pMeasIQout2[i*2U] = iSample2;
                pMeasIQout2[i*2U+1U] = qSample2;

                /* Store unpacked IQs */
                pIQout1[channel*2U] = iSample1;
                pIQout1[channel*2U+1U] = qSample1;
                pIQout2[channel*2U] = iSample2;
                pIQout2[channel*2U+1U] = qSample2;

                if (((uint16_t)tqi1 & 0xFU) == 0U)
                {
                    FREQMASK_SET(tqi1Mask, channel);
                }
                if (((uint16_t)tqi2 & 0xFU) == 0U)
                {
                    FREQMASK_SET(tqi2Mask, channel);
                }

                FREQMASK_SET(freqMask, channel); /* channel has been used */
                nbValid[apIdx]++;

                pIQin1 += (IQ_SIZE + TQI_SIZE)*apNb;
                pIQin2 += (IQ_SIZE + TQI_SIZE)*apNb;
            }
        }
        pIQout1 += 2U*gCsChannelsNb_c;
        pIQout2 += 2U*gCsChannelsNb_c;
        pMeasIQout1 += 2U*cs_data->step_nb;
        pMeasIQout2 += 2U*cs_data->step_nb;
    }
}

/*! *********************************************************************************
 *\fn           uint8_t AppLocalizationAlgo_ComputeTsw(deviceId_t deviceId);
 *
 * \brief       Function which computes the T_SW used by a CS procedure.
 *
 * \param[in]   deviceId            Peer identifier
 *
 *\retval       t_sw                Value of T_SW
 ********************************************************************************** */
static uint8_t AppLocalizationAlgo_ComputeTsw
(
    deviceId_t deviceId
)
{
    uint8_t t_sw = 0U;

    switch (mRangeSettings[deviceId].ant_cfg_index)
    {
        /* For N:1 it's the initator's T_SW */
        case 1U:
        case 2U:
        case 3U:
        {
            t_sw = (mGlobalRangeSettings.role == gCsRoleInitiator_c) ? mGlobalRangeSettings.t_sw_local : mRangeSettings[deviceId].t_sw_remote;
        }
        break;

        /* For 1:N it's the reflector's T_SW */
        case 4U:
        case 5U:
        case 6U:
        {
            t_sw = (mGlobalRangeSettings.role == gCsRoleReflector_c) ? mGlobalRangeSettings.t_sw_local : mRangeSettings[deviceId].t_sw_remote;
        }
        break;

        /* For 2:2 it's the larger T_SW value from the initiator and the reflector */
        case 7U:
        {
            t_sw = ( mGlobalRangeSettings.t_sw_local >= mRangeSettings[deviceId].t_sw_remote) ?  mGlobalRangeSettings.t_sw_local : mRangeSettings[deviceId].t_sw_remote;
        }
        break;

        default:
        {
            ; /* leave t_sw with the initialization value of 0 */
        }
        break;
    }

    return t_sw;
}

/*! *********************************************************************************
 *\fn           uint8_t AppLocalizationAlgo_CountLeadingZeroes(uint16_t decimalPart);
 *
 * \brief       Computes leading zeroes of the result decimal part, for display purposes.
 *
 * \param[in]   decimalPart         Decimal part value
 *
 *\retval       leadingZeroes       Number of leading zeroes to be displayed
 ********************************************************************************** */
static uint8_t AppLocalizationAlgo_CountLeadingZeroes
(
    uint16_t decimalPart
)
{
    uint16_t leadingZeroes = 0U;

    while ((decimalPart != 0U) && (decimalPart * 10U < mPrecisionScaler))
    {
        leadingZeroes++;
        decimalPart *= 10U;
    }

    return leadingZeroes;
}
