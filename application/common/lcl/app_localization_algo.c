/*! *********************************************************************************
* Copyright 2023 - 2026 NXP
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
#include <mathfp.h>
#include "app_localization_algo.h"
#include "fsl_component_mem_manager.h"
#include "dm_phasebased.h"
#include "pde_rade.h"
#include "channel_sounding.h"
#include "app_localization_data_export.h"
#include "fsl_component_panic.h"
   
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
        uint16_t ts_diff_tmp = ((uint16_t)(report_p)[1] << 8U) | (uint16_t)(report_p)[0]; \
        ts_diff = (int16_t)ts_diff_tmp; \
        aa_quality = (uint8_t)(((report_p)[2] >> 4U) & 0xFU); \
    }

#define ISP_MCIQ_SIGN_EXTEND_12_16(x)     ((x) | ((((x) & 0x800U) != 0U) ? 0xF000U : 0x0U))
#define FREQMASK_SET(pFreqMask, i) (pFreqMask[(i) >> 5U] |= ((uint32_t)1U << ((i) & 0x1FU)))

#define gPhaseCorrectionFactor_c      (65536) /* (2 * 32768) */
#define gPhaseNormalizedThreshold_c   (32767)
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
                                            int16_t *pIQout1, int16_t *pIQout2, int16_t *pPhases,
                                            uint32_t freqMask[], uint32_t tqi1Mask[],
                                            uint32_t tqi2Mask[], uint16_t nbValid[]);
static uint8_t AppLocalizationAlgo_ComputeTsw(deviceId_t deviceId);
static uint8_t AppLocalizationAlgo_CountLeadingZeroes(uint16_t decimalPart);
static int16_t AppLocalizationAlgo_CombinePhases(int16_t iSample1, int16_t qSample1, int16_t iSample2, int16_t qSample2);
static int32_t AppLocalizationAlgo_GetAbsoluteValue(int32_t value);
static void AppLocalizationAlgo_ConvertFloatToIntDec(float value, uint8_t *pIntPart, uint16_t *pDecPart, uint8_t *pLeadingZeroes);
static void AppLocalizationAlgo_ConvertDqiToIntDec(float dqiPercentage, uint16_t *pIntPart, uint16_t *pDecPart);
static void AppLocalizationAlgo_ProcessAlgoResult(int32_t distFp, int16_t dqiFp, float distScaler, float dqiScaler, localizationAlgoRun_t *pResult);
/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
#if (defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)) || \
    (defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)) || \
    (defined(gAppSlopeBasedCalibrationSupport_d) && (gAppSlopeBasedCalibrationSupport_d == 1))    
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

#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1)
    pResult->rssiInfo.rssiLocalNo = 0U;
    pResult->rssiInfo.rssiRemoteNo = 0U;
    FLib_MemSet(pResult->rssiInfo.aRssiLocal, gRssiNotAvailable_c, gMaxNumCsSteps_c);
    FLib_MemSet(pResult->rssiInfo.aRssiRemote, gRssiNotAvailable_c, gMaxNumCsSteps_c);
#endif /* gAppParseRssiInfo_d */

    if (pResult != NULL)
    {
        csAppData_t *pLocalCsAppData = (csAppData_t*)(void*)pLocalData->pData;
        csAppData_t *pRemoteCsAppData = (csAppData_t*)(void*)pPeerData->pData;
        
        FLib_MemSet(&response, 0, sizeof(isp_meas_response_t));
        response.cs_data = &pLocalCsAppData->csData;
        response.cs_data->modeMapRemote = pRemoteCsAppData->csData.modeMap;

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

        /* Extract Subevent Done Status for local data */
        for (uint8_t idx = 0U; idx <= pLocalData->subeventIndex; idx++)
        {
            response.cs_data->subevtDoneStatusLocal[idx] = pLocalData->aSubEventData[idx].subevtHeader.subeventDoneStatus;
        }

        /* Extract Subevent Done Status and Stop Index for remote data */
        for (uint8_t idx = 0U; idx <= pPeerData->subeventIndex; idx++)
        {
            response.cs_data->subevtDoneStatusRemote[idx] = pPeerData->aSubEventData[idx].subevtHeader.subeventDoneStatus;
            response.cs_data->subevtStopIdxRemote[idx] = pRemoteCsAppData->csData.subevtStopIdxRemote[idx];
        }

        /* Reorder data so that index 0 represents initiator, index 1 represents reflector whatever the device role is */
        if (role == gCsRoleInitiator_c)
        {
            csDataBuffer0 = pLocalCsAppData;
            csDataBuffer1 = pRemoteCsAppData;
            FLib_MemCpy(response.cs_data->subevtRefPowerLevelInit, pLocalCsAppData->csData.subevtRefPowerLevelInit, gMaxNumCsSubevents_c);
            FLib_MemCpy(response.cs_data->subevtRefPowerLevelRefl, pRemoteCsAppData->csData.subevtRefPowerLevelInit, gMaxNumCsSubevents_c);
        }
        else
        {
            csDataBuffer1 = pLocalCsAppData;
            csDataBuffer0 = pRemoteCsAppData;
            FLib_MemCpy(response.cs_data->subevtRefPowerLevelRefl, pLocalCsAppData->csData.subevtRefPowerLevelInit, gMaxNumCsSubevents_c);
            FLib_MemCpy(response.cs_data->subevtRefPowerLevelInit, pRemoteCsAppData->csData.subevtRefPowerLevelInit, gMaxNumCsSubevents_c);
        }

        if (pLocalCsAppData->mciq_data.nbSteps != 0U)
        {
            /* MCIQ */
            response.mciq_data[0] = csDataBuffer0->mciq_data;
            response.mciq_data[0].iq = (uint8_t *)(csDataBuffer0->mciqBuffer);
            response.mciq_data[1] = csDataBuffer1->mciq_data;
            response.mciq_data[1].iq = (uint8_t *)(csDataBuffer1->mciqBuffer);
        }
        if (pLocalCsAppData->tof_data.nbSteps != 0U)
        {
            /* ToF */
            response.tof_data[0] = csDataBuffer0->tof_data;
            response.tof_data[0].ts = csDataBuffer0->tofBuffer;
            response.tof_data[1] = csDataBuffer1->tof_data;
            response.tof_data[1].ts = csDataBuffer1->tofBuffer;
        }

        /* Store remote mode0 step data - local already set */
        FLib_MemCpy(&response.cs_data->mode0Data[gMaxNumCsStepsMode0_c],
                    pRemoteCsAppData->csData.mode0Data,
                    gMaxNumCsStepsMode0_c * sizeof(mode0_data_t));

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

#if defined(gAppLocDataExport_d) && (gAppLocDataExport_d > 0)
        if (pLocalData->pData != NULL)
        {
            engine_response.is_valid = TRUE;
            app_print_cs_data(&response, &engine_response, &mRangeSettings[deviceId]);
        }
#endif 

#if defined(gAppSlopeBasedCalibrationSupport_d) && (gAppSlopeBasedCalibrationSupport_d == 1)
        for (uint8_t i = 0U; i < engine_config.n_ap; i++)
        {
            AppLocalizationAlgo_ProcessAlgoResult(
                engine_response.mciq_result.sde_dist_array[i],
                engine_response.mciq_result.sde_dqi_array[i],
                32768.0f,  /* SDE distance scaler */
                32768.0f,  /* SDE DQI scaler */
                &pResult->aResultSlope[i]);
        }
#endif /* gAppSlopeBasedCalibrationSupport_d */

#if defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)
        if ((pResult->algorithm & eMciqAlgoEmbedCDE) != 0U)
        {
            AppLocalizationAlgo_ProcessAlgoResult(
                engine_response.mciq_result.cde_fp,
                engine_response.mciq_result.cde_dqi,
                1024.0f,   /* CDE distance scaler */
                16384.0f,  /* CDE DQI scaler */
                &pResult->resultCDE
            );
        }
#endif /* gAppUseCDEAlgorithm_d */

#if defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)
        if ((pResult->algorithm & eMciqAlgoEmbedRADE) != 0U)
        {
            mciq_result_t* result = &engine_response.mciq_result;
            pResult->radeError = result->rade_error_flag;
            pResult->resultRADE.distanceInMeters = result->rade_dist;
            pResult->resultRADE.dqiPercentage = result->rade_dqi * 100.f; /* Convert decimal number to percentage. */

            AppLocalizationAlgo_ConvertFloatToIntDec(
                pResult->resultRADE.distanceInMeters,
                &pResult->resultRADE.distanceIntegerPart,
                &pResult->resultRADE.distanceDecimalPart,
                &pResult->resultRADE.leadingZeroesDecimalPart
            );
            
            AppLocalizationAlgo_ConvertDqiToIntDec(
                pResult->resultRADE.dqiPercentage,
                &pResult->resultRADE.dqiIntegerPart,
                &pResult->resultRADE.dqiDecimalPart
            );
        }
#endif /* gAppUseRADEAlgorithm_d */

#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1)
        /* Copy local RSSI info */
        pResult->rssiInfo.rssiLocalNo = pLocalCsAppData->rssiStepNo;
        FLib_MemCpy(pResult->rssiInfo.aRssiLocal, pLocalCsAppData->aRssiValue, gMaxNumCsSteps_c);
        /* Copy remote RSSI info */
        pResult->rssiInfo.rssiRemoteNo = pRemoteCsAppData->rssiStepNo;
        FLib_MemCpy(pResult->rssiInfo.aRssiRemote, pRemoteCsAppData->aRssiValue, gMaxNumCsSteps_c);
#endif /* gAppParseRssiInfo_d */

        /* Clear local data and peer data */
#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
        RasClient_ResetPeer(deviceId, FALSE);
#endif
        AppLocalization_FreeLocalData(deviceId);
        pLocalData->pData = NULL;
    }
    return;
}
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

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
    int16_t *pPhases = NULL;

    uint16_t nb_valid[gMaxNumAntennaPaths_c] = {0};
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

    mciq_result->cde_dqi = 0;
    mciq_result->cde_fp = 0;

    /* Init frequency mask */
    for (uint8_t i= 0; i < (XCVR_F_RANGE/32U + 1U); i++)
    {
        freqMask[i] = 0;
        tqi1Mask[i] = 0;
        tqi2Mask[i] = 0;
    }

#if defined(gAppSlopeBasedCalibrationSupport_d) && (gAppSlopeBasedCalibrationSupport_d == 1)
    pPhases = MEM_BufferAlloc(meas_response->cs_data->step_nb * sizeof(int16_t) * (uint32_t)engine_config->n_ap);
#endif /* gAppSlopeBasedCalibrationSupport_d */

    /* Extract IQs */
    isp_mciq_measurement_unpack_iqs(meas_response->cs_data,
                                                   init_data,
                                                   refl_data,
                                                   iq1,
                                                   iq2,
                                                   pPhases,
                                                   freqMask,
                                                   tqi1Mask,
                                                   tqi2Mask,
                                                   nb_valid);

    /* SDE is enabled */
#if defined(gAppSlopeBasedCalibrationSupport_d) && (gAppSlopeBasedCalibrationSupport_d == 1)
    if (pPhases != NULL)
    {
        dm_slope_out_t aSlopeResult[gMaxNumAntennaPaths_c] = {};
        int16_t *pIndex = pPhases;
        dm_status_t slopeStatus;

        for (uint8_t m = 0U; m < engine_config->n_ap; m++)
        {
            slopeStatus = dm_slope_based_fp(nb_valid[m],
                                             pIndex,
                                             freqMask,
                                             DM_FREQUENCY_RASTER,
                                             &aSlopeResult[m],
                                             NULL);
            if (slopeStatus == DM_STATUS_OK)
            {
                mciq_result->sde_dist_array[m] = aSlopeResult[m].distance;
                mciq_result->sde_dqi_array[m] = aSlopeResult[m].dqi;
                mciq_result->nb_valid[m] = nb_valid[m];
            }
            else
            {
                mciq_result->sde_dist_array[m] = 0;
                mciq_result->sde_dqi_array[m] = 0;
                mciq_result->nb_valid[m] = 0;           
            }
            pIndex += nb_valid[m];
        }
    } 
#endif /* gAppSlopeBasedCalibrationSupport_d */
    (void)MEM_BufferFree(pPhases);

    /* CDE is enabled */
#if defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)
    if((engine_config->mciq_algo_flags & eMciqAlgoEmbedCDE) != 0U)
    {
        dm_cde_estimate_t estimate = {0};
        uint8_t index = 0U;
        uint32_t cde_threshold = engine_config->n_ap == 1U ? engine_config->cde_threshold : engine_config->cde_div_threshold;
        int32_t d_min = 1024 * Q10_SCALING_FACTOR;   /* in Q21.10 format */
        bool success[gMaxNumAntennaPaths_c];

        for(uint8_t m = 0; m < engine_config->n_ap; m++)
        {

            success[m] = false;

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
                mciq_result->nb_valid[m] = (uint16_t)nb_valid[m];
            }
            else
            {
                mciq_result->cde_dist_array[m] = 0;
                mciq_result->cde_dqi_array[m] = 0;
                mciq_result->nb_valid[m] = 0;
            }
        } /* end for m */

        for(uint8_t m = 0; m < engine_config->n_ap; m++)
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
        mciq_result->nb_valid_freq = mciq_result->nb_valid[index];
    }
#endif /* gAppUseCDEAlgorithm_d */
    (void)MEM_BufferFree(buf_start);

#if defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)
    if((engine_config->mciq_algo_flags & eMciqAlgoEmbedRADE) != 0U)
    {
        /* Run RADE algorithm */
        rade_result_type_t radeStatus;
        mciq_result->rade_error_flag = 0U;

        float_rade_t radeResReserved;
        rade_cs_para_t radeCsPara;
        rade_result_t radeResult;
        rade_cs_data_t radeCsData;
        radeCsPara.csRole                   = mGlobalRangeSettings.role == gCsRoleInitiator_c ? 0U : 1U;
        radeCsPara.step_nb                  = meas_response->cs_data->step_nb;
        radeCsPara.startAclCnt              = meas_response->cs_data->startAclCnt;
        radeCsPara.mode0_nb                 = meas_response->cs_data->mode0_nb;
        radeCsPara.subevt_nb                = meas_response->cs_data->subevt_nb;
        radeCsPara.t_fcs                    = meas_response->cs_data->t_fcs;
        radeCsPara.t_ip1                    = meas_response->cs_data->t_ip1;
        radeCsPara.t_ip2                    = meas_response->cs_data->t_ip2;
        radeCsPara.t_pm                     = meas_response->cs_data->t_pm;
        radeCsPara.t_sw                     = meas_response->cs_data->t_sw;
        radeCsPara.channelMap               = meas_response->cs_data->channelMap;
        radeCsPara.modeMap                  = meas_response->cs_data->modeMap;
        radeCsPara.modeMap_remote           = meas_response->cs_data->modeMapRemote;
        radeCsPara.subevtStopIdx_local      = meas_response->cs_data->subevtStopIdxLocal;
        radeCsPara.subevtStopIdx_remote     = meas_response->cs_data->subevtStopIdxRemote;
        radeCsPara.subevtConnEvent          = meas_response->cs_data->subevtConnEvent;
        radeCsPara.main_mode_repeat         = meas_response->cs_data->main_mode_repeat;
        radeCsPara.rtt_type                 = meas_response->cs_data->rtt_type;
        radeCsPara.rtt_phy                  = meas_response->cs_data->phy;
        radeCsPara.main_mode_type           = meas_response->cs_data->main_mode_type;
        radeCsPara.sub_mode_type            = meas_response->cs_data->sub_mode_type;
        radeCsPara.connInterval             = meas_response->cs_data->conn_interval;
        radeCsPara.refPowerLevel_init       = meas_response->cs_data->subevtRefPowerLevelInit;
        radeCsPara.refPowerLevel_refl       = meas_response->cs_data->subevtRefPowerLevelRefl;
        radeCsPara.subevtDoneStatus_local   = meas_response->cs_data->subevtDoneStatusLocal;
        radeCsPara.subevtDoneStatus_remote  = meas_response->cs_data->subevtDoneStatusRemote;
        radeResult.rng_est                  = &mciq_result->rade_dist;
        radeResult.rng_est_qi               = &mciq_result->rade_dqi;
        radeResult.reserved                 = &radeResReserved;
        radeResult.rng_trk                  = &mciq_result->rade_dist_trk;
        radeCsData.iq_i                     = init_data->iq;
        radeCsData.iq_r                     = refl_data->iq;
        radeCsData.n_ap                     = engine_config->n_ap;
        radeCsData.csDataBufFreeCb.pfFreeCb = NULL;
        radeCsData.csDataBufFreeCb.deviceId = 0U;      
        rade_para_t radePara                = {.radeMode = kRadeNormal, .distBias = 0.0f, .ceHeap_id = g_ceHeap_id};
#ifdef LCE_KW47_MCXW72
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)
        /* LCE is not able to compute while CORE0 is in deep sleep */
        (void)PWR_SetLowPowerModeConstraint(PWR_WFI);
#endif /* defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0) */
#endif /* LCE_KW47_MCXW72 */
        radeStatus = pde_rade(&radeCsData, meas_response->cs_data->csAlgoBuf, &radeCsPara, &radeResult, &radePara);
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
    uint16_t nb_steps = meas_response->tof_data[0].nbSteps;
    int32_t sum_rtt_ns = 0;
    uint16_t nb_steps_valid = 0U;

    for (uint16_t tone = 0; tone < nb_steps; tone++)
    {
        int16_t init_ns = 0;
        int16_t refl_ns = 0;
        uint8_t init_aa_quality = 0U;
        uint8_t refl_aa_quality = 0U;

        init_ts += gCsNadmSize_c + gCsRssiSize_c;
        refl_ts += gCsNadmSize_c + gCsRssiSize_c;
        CS_GET_RTT_TS_DIFF(init_ts, init_ns, init_aa_quality);
        CS_GET_RTT_TS_DIFF(refl_ts, refl_ns, refl_aa_quality);

        /* Check that both quality indicator are valid */
        if ((init_aa_quality | refl_aa_quality) == 0U)
        {
            sum_rtt_ns += (int32_t)init_ns - (int32_t)refl_ns;
            nb_steps_valid++;
        }
        init_ts += gCsTsSize_c;
        refl_ts += gCsTsSize_c;
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
 *                                  int16_t *pPhases,
 *                                  uint32_t freqMask[],
 *                                  uint32_t tqi1Mask[],
 *                                  uint32_t tqi2Mask[],
 *                                  uint16_t nbValid[]);
 *
 * \brief       Unpack IQs from mciq measurement data
 *
 * \param[in]   cs_data             Pointer to CS data
 * \param[in]   data1               Pointer to initiator data
 * \param[in]   data2               Pointer to reflector data
 * \param[out]  pIQout1             Pointer to IQ1 data
 * \param[out]  pIQout2             Pointer to IQ2 data
 * \param[out]  pPhases             Pointer to combined phase data
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
    int16_t *pPhases,
    uint32_t freqMask[],
    uint32_t tqi1Mask[],
    uint32_t tqi2Mask[],
    uint16_t nbValid[]
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
                pIQ = pIQin1 + (gCsIqSize_c+gCsTqiSize_c)*apIdx;
                qSample1u = (uint16_t)pIQ[0] | ((uint16_t)pIQ[1] & 0xFU) << 8U;
                qSample1u = ISP_MCIQ_SIGN_EXTEND_12_16((uint16_t)qSample1u);
                qSample1 = (int16_t)qSample1u;
                iSample1u = (((uint16_t)pIQ[1] & 0xF0U) >> 4U) | (pIQ[2] << 4U);
                iSample1u = ISP_MCIQ_SIGN_EXTEND_12_16((uint16_t)iSample1u);
                iSample1 = (int16_t)iSample1u;
                pIQ += gCsIqSize_c;
                tqi1 = (int16_t)*pIQ;
                pIQ = pIQin2 + (gCsIqSize_c+gCsTqiSize_c)*apIdx;
                qSample2u = (uint16_t)pIQ[0] | ((uint16_t)pIQ[1] & 0xFU) << 8U;
                qSample2u = ISP_MCIQ_SIGN_EXTEND_12_16((uint16_t)qSample2u);
                qSample2 = (int16_t)qSample2u;
                iSample2u = (((uint16_t)pIQ[1] & 0xF0U) >> 4U) | (pIQ[2] << 4U);
                iSample2u = ISP_MCIQ_SIGN_EXTEND_12_16((uint16_t)iSample2u);
                iSample2 = (int16_t)iSample2u;
                pIQ += gCsIqSize_c;
                tqi2 = (int16_t)*pIQ;

                if (pPhases != NULL)
                {
                    *pPhases++ = AppLocalizationAlgo_CombinePhases(iSample1, qSample1, iSample2, qSample2);
                }

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

                pIQin1 += (gCsIqSize_c + gCsTqiSize_c)*apNb;
                pIQin2 += (gCsIqSize_c + gCsTqiSize_c)*apNb;
            }
        }
        pIQout1 += 2U*gCsChannelsNb_c;
        pIQout2 += 2U*gCsChannelsNb_c;
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
    union 
    {
        uint16_t u16;
        uint8_t u8;
    } tmp;
    uint16_t leadingZeroes = 0U;

    while (((uint32_t)decimalPart != 0U) && ((uint32_t)decimalPart * 10U < mPrecisionScaler))
    {
        leadingZeroes++;
        decimalPart *= 10U;
    }
    tmp.u16 = leadingZeroes;

    return tmp.u8;
}

/*! *********************************************************************************
 *\fn           int16_t AppLocalizationAlgo_CombinePhases(int16_t iSample1,
 *                               int16_t qSample1, int16_t iSample2, int16_t qSample2);
 *
 * \brief       Combines phases, scales to [-1;1] in Q15 fixed-point format.
 *
 * \param[in]   iSample1         I sample on initiator.
 * \param[in]   qSample1         Q sample on initiator.
 * \param[in]   iSample2         I sample on reflector.
 * \param[in]   qSample2         Q sample on reflector.
 *
 *\retval       phaseConv.i16    Normalized combined phase in Q15 format.
 ********************************************************************************** */
static int16_t AppLocalizationAlgo_CombinePhases(int16_t iSample1, int16_t qSample1, int16_t iSample2, int16_t qSample2)
{
    int32_t phase1, phase2;
    union
    {
        int32_t i32;
        int16_t i16;
    } phaseConv;
    
    /* Compute phase = atan(Q/I) */
    phase1 = atan2fp(qSample1, iSample1);
    /* Normalize phase to Pi in Q3.12 and convert to Q15 */
    phase1 = (phase1 * 32768) / 12867;
    
    phase2 = atan2fp(qSample2, iSample2);
    phase2 = (phase2 * 32768) / 12867;
    
    /* Sum phases */
    phase1 = phase1 + phase2;
    
    /* Phi normalization wrapped in [-1,1] */
    if (phase1 > gPhaseNormalizedThreshold_c)
    {
        phase1 -= gPhaseCorrectionFactor_c;
    }
    else if (phase1 < (-gPhaseNormalizedThreshold_c))
    {
        phase1 += gPhaseCorrectionFactor_c;
    }
    else
    {
        /* MISRA rule 15.7 */
    }

    phaseConv.i32 = phase1;
    return phaseConv.i16;
}

/*! *********************************************************************************
 *\fn           int32_t AppLocalizationAlgo_GetAbsoluteValue(int32_t value);
 *
 * \brief       Get absolute value using 2's complement for negative numbers.
 *
 * \param[in]   value         Input value.
 *
 *\retval       int32_t       Absolute value.
 ********************************************************************************** */
static int32_t AppLocalizationAlgo_GetAbsoluteValue(int32_t value)
{
    if (value < 0)
    {
        union
        {
            uint32_t u32;
            int32_t  i32;
        } conv;

        conv.u32 = (0xFFFFFFFFUL ^ (uint32_t)value) + 1UL;
        value = conv.i32;
    }
    return value;
}

/*! *********************************************************************************
 *\fn           void AppLocalizationAlgo_ConvertFloatToIntDec(float value,
 *                                                            uint8_t *pIntPart,
 *                                                            uint16_t *pDecPart,
 *                                                            uint8_t *pLeadingZeroes);
 *
 * \brief       Convert float value to integer and decimal parts.
 *
 * \param[in]   value            Float value to convert.
 * \param[out]  pIntPart         Pointer to store integer part.
 * \param[out]  pDecPart         Pointer to store decimal part.
 * \param[out]  pLeadingZeroes   Pointer to store leading zeroes count.
 *
 *\retval       none
 ********************************************************************************** */
static void AppLocalizationAlgo_ConvertFloatToIntDec
(
    float value,
    uint8_t *pIntPart,
    uint16_t *pDecPart,
    uint8_t *pLeadingZeroes
)
{
    float temp;
    uint32_t scaledValue, integerPart, decimalPart;
    
    temp = value * (float)mPrecisionScaler;
    scaledValue = (uint32_t)temp;
    integerPart = scaledValue / mPrecisionScaler;
    if (integerPart > (uint32_t)UINT8_MAX)
    {
        integerPart = (uint32_t)UINT8_MAX;
    }
    *pIntPart = (uint8_t)integerPart;

    decimalPart = (scaledValue - (integerPart * mPrecisionScaler));
    if (decimalPart > (uint32_t)UINT16_MAX)
    {
        decimalPart = (uint32_t)UINT16_MAX;
    }    
    *pDecPart = (uint16_t)decimalPart;
    *pLeadingZeroes = AppLocalizationAlgo_CountLeadingZeroes(*pDecPart);
}

/*! *********************************************************************************
 *\fn           void AppLocalizationAlgo_ConvertDqiToIntDec(float dqiPercentage,
 *                                                          uint16_t *pIntPart,
 *                                                          uint16_t *pDecPart);
 *
 * \brief       Convert DQI percentage to integer and decimal parts.
 *
 * \param[in]   dqiPercentage    DQI value in percentage.
 * \param[out]  pIntPart         Pointer to store integer part.
 * \param[out]  pDecPart         Pointer to store decimal part.
 *
 *\retval       none
 ********************************************************************************** */
static void AppLocalizationAlgo_ConvertDqiToIntDec
(
    float dqiPercentage,
    uint16_t *pIntPart,
    uint16_t *pDecPart
)
{
    float temp;
    uint32_t integerPart, decimalPart, scaledDqi;
    uint32_t divFactor = mPrecisionScaler / 100U;
    
    temp = dqiPercentage * (float)divFactor;
    scaledDqi = (uint32_t)temp;

    integerPart = scaledDqi / divFactor;
    if (integerPart > (uint32_t)UINT16_MAX)
    {
        integerPart = (uint32_t)UINT16_MAX;
    }
    *pIntPart = (uint16_t)integerPart;

    decimalPart = scaledDqi - (integerPart * divFactor);
    if (decimalPart > (uint32_t)UINT16_MAX)
    {
        decimalPart = (uint32_t)UINT16_MAX;
    }
    *pDecPart = (uint16_t)decimalPart;
}

/*! *********************************************************************************
 *\fn           void AppLocalizationAlgo_ProcessAlgoResult(int32_t distFp,
 *                                                         int16_t dqiFp,
 *                                                         float distScaler,
 *                                                         float dqiScaler,
 *                                                         algoResult_t *pResult);
 *
 * \brief       Process algorithm result and convert to display format.
 *
 * \param[in]   distFp        Distance in fixed-point format.
 * \param[in]   dqiFp         DQI in fixed-point format.
 * \param[in]   distScaler    Scaler for distance conversion.
 * \param[in]   dqiScaler     Scaler for DQI conversion.
 * \param[out]  pResult       Pointer to store converted result.
 *
 *\retval       none
 ********************************************************************************** */
static void AppLocalizationAlgo_ProcessAlgoResult
(
    int32_t distFp,
    int16_t dqiFp,
    float distScaler,
    float dqiScaler,
    localizationAlgoRun_t *pResult
)
{
    int32_t temp;

    /* Get absolute values */
    distFp = AppLocalizationAlgo_GetAbsoluteValue(distFp);
    temp = dqiFp;
    temp = AppLocalizationAlgo_GetAbsoluteValue(temp);
    if (temp > (int32_t)INT16_MAX)
    {
        temp = (int32_t)INT16_MAX;
    }
    dqiFp = (int16_t)temp;
    
    /* Convert distance */
    pResult->distanceInMeters = (float)distFp / distScaler;
    
    /* Convert DQI to percentage */
    pResult->dqiPercentage = ((float)dqiFp / dqiScaler) * 100.0f;
    
    /* Convert distance to integer/decimal parts */
    AppLocalizationAlgo_ConvertFloatToIntDec(
        pResult->distanceInMeters,
        &pResult->distanceIntegerPart,
        &pResult->distanceDecimalPart,
        &pResult->leadingZeroesDecimalPart
    );
    
    /* Convert DQI to integer/decimal parts */
    AppLocalizationAlgo_ConvertDqiToIntDec(
        pResult->dqiPercentage,
        &pResult->dqiIntegerPart,
        &pResult->dqiDecimalPart
    );
}