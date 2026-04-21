/*
 * Copyright 2021-2026 NXP
 *
 * NXP Proprietary
 *
 * This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or
 * by downloading, installing, activating and/or otherwise using the software, you are
 * agreeing that you have read, and that you agree to comply with and are bound by,
 * such license terms. If you do not agree to be bound by the applicable license terms,
 * then you may not retain, install, activate or otherwise use the software.
 */

/* === Includes ============================================================ */
#include <stdint.h>
#include <stdbool.h>

#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
#include "fsl_component_mem_manager.h"
#else
#include "MemManager.h"
#endif /*SDK_COMPONENT_INTEGRATION > 0*/

#include "isp_ranging_engine.h"
#include "isp_measurement.h"
#include "isp_debug_helpers.h"
#include "dm_phasebased.h"
#include "pde_rade.h"

/* === Macros =============================================================== */

#define CDE_ZERO_DQI_RSSI_THRESHOLD (-95) /* dBm */
#define CDE_IQ_SAT_UPPER_THRESHOLD (2000U)
#define CDE_IQ_SIMUL_LO_THRESHOLD (32U)

#define ISP_MCIQ_SIGN_EXTEND_12_16(x)     ((x) | (((x) & 0x800U) ? 0xF000U : 0x0U))

#define ABS(x) (((x) > 0) ? ((uint32_t)(x)) : ((uint32_t) -(x)))

/* assumes pFreqMask is uint32_t [] */
#define FREQMASK_SET(pFreqMask, i) (pFreqMask[(i) >> 5U] |= ((uint32_t)1U << ((i) & 0x1FU)))
#define FREQMASK_CLR(pFreqMask, i) (pFreqMask[(i) >> 5U] &= ~((uint32_t)1U << (i & 0x1FU)))
#define FREQMASK_ISSET(pFreqMask, i) (((pFreqMask[i >> 5U]) & ((uint32_t)1U << (i & 0x1FU))) != 0U)

#include "clock_config.h"

/* === Types =============================================================== */

/* === Globals ============================================================= */
void *csAlgoBuf = NULL;
#ifdef LCE_KW47_MCXW72
extern uint8_t g_ceHeap_id;
#else
static uint8_t g_ceHeap_id = 0U;
#endif

/* === Implementation ====================================================== */

static void isp_mciq_measurement_unpack_iqs(cs_data_t *cs_data,
                                                mciq_data_t *data1, mciq_data_t *data2,
                                                int16_t *pIQout1, int16_t *pIQout2,
                                                uint32_t freqMask[],
                                                uint32_t tqi1Mask[],
                                                uint32_t tqi2Mask[],
                                                uint32_t nbValid[])
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
            if (cs_data->modeMap[i] == 2 || cs_data->modeMap[i] == 3)
            {
                channel = cs_data->channelMap[i];
                assert(channel < gCsChannelsNb_c);

                pIQ = pIQin1 + (IQ_SIZE+TQI_SIZE)*apIdx;
                qSample1 = pIQ[0] | ((pIQ[1] & 0x0F) << 8U);
                qSample1 = ISP_MCIQ_SIGN_EXTEND_12_16(qSample1);
                iSample1 = ((pIQ[1] & 0xF0) >> 4U) | (pIQ[2] << 4U);
                iSample1 = ISP_MCIQ_SIGN_EXTEND_12_16(iSample1);
                pIQ += IQ_SIZE; // TQI located after IQ
                tqi1 = *pIQ;
                pIQ = pIQin2 + (IQ_SIZE+TQI_SIZE)*apIdx;
                qSample2 = pIQ[0] | ((pIQ[1] & 0x0F) << 8U);
                qSample2 = ISP_MCIQ_SIGN_EXTEND_12_16(qSample2);
                iSample2 = ((pIQ[1] & 0xF0) >> 4U) | (pIQ[2] << 4U);
                iSample2 = ISP_MCIQ_SIGN_EXTEND_12_16(iSample2);
                pIQ += IQ_SIZE; // TQI located after IQ
                tqi2 = *pIQ;

                /* Store unpacked IQs */
                pIQout1[channel*2] = iSample1;
                pIQout1[channel*2+1] = qSample1;
                pIQout2[channel*2] = iSample2;
                pIQout2[channel*2+1] = qSample2;

                if ((tqi1 & 0xF) == 0)
                {
                    FREQMASK_SET(tqi1Mask, channel); /* TQI1 is GOOD */
                }
                if ((tqi2 & 0xF) == 0)
                {
                    FREQMASK_SET(tqi2Mask, channel); /* TQI2 is GOOD */
                }

                FREQMASK_SET(freqMask, channel); /* channel has been used */
                nbValid[apIdx]++;

                //DPRINTF_DBG("%d IQ1 %d %d (%d), \tIQ2 %d %d (%d)\n", apIdx, iSample1, qSample1, tqi1, iSample2, qSample2, tqi2);
                pIQin1 += (IQ_SIZE + TQI_SIZE)*apNb;
                pIQin2 += (IQ_SIZE + TQI_SIZE)*apNb;
            }
        }
        pIQout1 += 2*gCsChannelsNb_c;
        pIQout2 += 2*gCsChannelsNb_c;
    }
}

/*
 * Run distance estimation algorithm(s) and store computation results in mciq_result
 */
void isp_mciq_ranging_compute(isp_meas_response_t *meas_response, mciq_result_t *mciq_result, engine_config_t *engine_config)
{
    mciq_data_t *init_data = &meas_response->mciq_data[0];
    mciq_data_t *refl_data = &meas_response->mciq_data[1];
    uint32_t m, index = 0;
    int16_t *iq1, *iq2;

    uint32_t nb_valid[ISP_MAX_NO_ANTENNAS];
    /* mask of CS channels that have been actually used for RTP measurements */
    uint32_t freqMask[(XCVR_F_RANGE/32)+1];
    /* mask of CS channels that have been actually used for RTP measurements and for which TQI was GOOD */
    uint32_t tqi1Mask[(XCVR_F_RANGE/32)+1];
    uint32_t tqi2Mask[(XCVR_F_RANGE/32)+1];

    /* Allocate a single buffer to store combined phases and IQs for all devices. freed in isp_measurement_stop */
    /* For each device : 2 IQ buff (each 2*uint16_t*channels*n_ap) */
    uint32_t item_size = 2 * sizeof(int16_t) * gCsChannelsNb_c * engine_config->n_ap;
    uint8_t *buf_start = (uint8_t *)MEM_BufferAlloc(2*item_size);

    if (buf_start == NULL)
        return;

    FLib_MemSet(buf_start, 0U, 2U * item_size);
    iq1 = (int16_t *)buf_start;
    iq2 = (int16_t *)(buf_start + item_size);

    mciq_result->cde_dqi = 0;
    mciq_result->cde_fp = 0;
    mciq_result->rade_error_flag = 0;

    /* Init frequency mask */
    for (int i= 0; i < (XCVR_F_RANGE/32 + 1); i++)
    {
        freqMask[i] = 0;
        tqi1Mask[i] = 0;
        tqi2Mask[i] = 0;
    }

    /* Extract IQs */
    isp_mciq_measurement_unpack_iqs(meas_response->cs_data,
                                                   init_data, refl_data,
                                                   iq1, iq2,
                                                   freqMask,
                                                   tqi1Mask, tqi2Mask,
                                                   nb_valid);

    /* CDE is enabled */
    if(engine_config->mciq_algo_flags & eMciqAlgoEmbedCDE)
    {
        dm_cde_estimate_t estimate;
        uint32_t cde_threshold = engine_config->n_ap == 1 ? engine_config->cde_threshold : engine_config->cde_div_threshold;
        int32_t d_min = 1024 * Q10_SCALING_FACTOR;   /* in Q21.10 format */
        bool success[ISP_MAX_NO_ANTENNAS];

        for(m = 0; m < engine_config->n_ap; m++)
        {

            success[m] = 0;

            /* Run the algorithm */
            success[m] = dm_cde_distance_estimation(iq1 + (2*m*gCsChannelsNb_c),
                                                    iq2 + (2*m*gCsChannelsNb_c),
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
                mciq_result->cde_nb_valid[m] = nb_valid[m];
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
        if (1) // FIXME (init_data->info->rssi > CDE_ZERO_DQI_RSSI_THRESHOLD) && (refl_data->info->rssi > CDE_ZERO_DQI_RSSI_THRESHOLD))
            mciq_result->cde_dqi = mciq_result->cde_dqi_array[index]; /* Final DQI */
        else
            mciq_result->cde_dqi = 0; /* RSSI too low, estimation is unreliable */
        mciq_result->nb_valid_freq = mciq_result->cde_nb_valid[index];
    }
    
    MEM_BufferFree(buf_start);

    if(engine_config->mciq_algo_flags & eMciqAlgoEmbedRADE)
    {
        /* Run RADE algorithm */
        rade_result_type_t radeStatus;
        float_rade_t radeResReserved;
        rade_cs_para_t radeCsPara;
        rade_result_t radeResult;
        rade_cs_data_t radeCsData;

        radeCsPara.csRole               =  meas_response->mparams->cfg.role           ;
        radeCsPara.step_nb              =  meas_response->cs_data->step_nb            ;
        radeCsPara.startAclCnt          =  meas_response->cs_data->startAclCnt        ;
        radeCsPara.subevt_nb            =  meas_response->cs_data->subevt_nb          ;
        radeCsPara.channelMap           =  meas_response->cs_data->channelMap         ;
        radeCsPara.modeMap              =  meas_response->cs_data->modeMap            ;
        radeCsPara.modeMap_remote       =  meas_response->cs_data->modeMapRemote      ;
        radeCsPara.subevtStopIdx_local  =  meas_response->cs_data->subevtStopIdxLocal ;
        radeCsPara.subevtStopIdx_remote =  meas_response->cs_data->subevtStopIdxRemote;
        radeCsPara.subevtConnEvent      =  meas_response->cs_data->subevtConnEvent    ;
        radeCsPara.mode0_nb             =  meas_response->mparams->cfg.mode0_nb       ;
        radeCsPara.main_mode_repeat     =  meas_response->mparams->cfg.main_mode_repeat ;
        radeCsPara.rtt_type             =  meas_response->mparams->cfg.rtt_type         ;
        radeCsPara.rtt_phy              =  meas_response->mparams->cfg.rtt_phy          ;
        radeCsPara.t_fcs                =  meas_response->mparams->cfg.t_fcs            ;
        radeCsPara.t_ip1                =  meas_response->mparams->cfg.t_ip1;           ;
        radeCsPara.t_ip2                =  meas_response->mparams->cfg.t_ip2;           ;
        radeCsPara.t_pm                 =  meas_response->mparams->cfg.t_pm;            ;
        radeCsPara.t_sw                 =  meas_response->mparams->cfg.t_sw;            ;
        radeCsPara.main_mode_type       =  meas_response->mparams->cfg.main_mode_type   ;
        radeCsPara.sub_mode_type        =  meas_response->mparams->cfg.sub_mode_type    ;
        radeCsPara.connInterval         =  meas_response->mparams->cfg.connInterval     ;
        radeCsPara.refPowerLevel_init   =  meas_response->cs_data->subevtRefPowerLevelInit;
        radeCsPara.refPowerLevel_refl   =  meas_response->cs_data->subevtRefPowerLevelRefl;
        radeCsPara.subevtDoneStatus_local = meas_response->cs_data->subevtDoneStatusLocal  ;
        radeCsPara.subevtDoneStatus_remote = meas_response->cs_data->subevtDoneStatusRemote ;
        radeCsPara.pctTransMode         = 0U;
        radeResult.rng_est              =  &mciq_result->rade_dist;
        radeResult.rng_trk              =  &mciq_result->rade_dist_trk;
        radeResult.rng_est_qi           =  &mciq_result->rade_dqi;
        radeResult.reserved             =  &radeResReserved;

        radeCsData.iq_i                     = init_data->iq;
        radeCsData.iq_r                     = refl_data->iq;
        radeCsData.n_ap                     = engine_config->n_ap;
        radeCsData.csDataBufFreeCb.pfFreeCb = NULL;
        radeCsData.csDataBufFreeCb.deviceId = 0U; 

        rade_para_t radePara            =  {.radeMode = kRadeNormal, .distBias = 0.0f, .ceHeap_id = g_ceHeap_id}; 

        radeStatus = pde_rade(&radeCsData, &csAlgoBuf, &radeCsPara, &radeResult, &radePara);
        if (kRadeSuccess != radeStatus)
        {
            mciq_result->rade_error_flag = radeStatus;
            mciq_result->rade_dist = null_RNG;
            mciq_result->rade_dist_trk = null_RNG;
            mciq_result->rade_dqi = 0.0f;
        }
    }

}

/*
 * Store computation result in tof_derived
 */
void isp_tof_ranging_compute(isp_meas_response_t *meas_response, tof_result_t *tof_result)
{
    uint8_t *init_ts = meas_response->tof_data[0].ts;
    uint8_t *refl_ts = meas_response->tof_data[1].ts;
    uint32_t nb_steps = meas_response->tof_data[0].nbSteps;
    int32_t sum_rtt_ns = 0;
    uint8_t nb_steps_valid = 0U;

    for (uint32_t tone = 0; tone < nb_steps; tone++)
    {
        int16_t init_ns;
        int16_t refl_ns;
        uint32_t init_aa_quality;
        uint32_t refl_aa_quality;
        init_ts += CS_NADM_SIZE + CS_RSSI_SIZE;
        refl_ts += CS_NADM_SIZE + CS_RSSI_SIZE;
        CS_GET_RTT_TS_DIFF(init_ts, init_ns, init_aa_quality);
        CS_GET_RTT_TS_DIFF(refl_ts, refl_ns, refl_aa_quality);
        //DPRINTF_DBG("init_ns=%d, refl_ns=%d, init_q=%d, refl_q=%d\n", init_ns, refl_ns, init_aa_quality, refl_aa_quality);
        /* Check that both quality indicator are valid */
        if ((init_aa_quality | refl_aa_quality) == 0)
        {
            sum_rtt_ns += init_ns - refl_ns;
            nb_steps_valid++;
        }
        init_ts += CS_TS_SIZE;
        refl_ts += CS_TS_SIZE;
    }
    tof_result->dm_sr = 0;
    if (nb_steps_valid != 0) {
        /*  average distance expected in fixed-point s15.16 (meters) */
        tof_result->dm_ad = sum_rtt_ns/nb_steps_valid;
        tof_result->dm_ad = tof_result->dm_ad * 9830U; /* RTT/2 * 0.3m/ns * 2^16 (0.3*2^16/2 is 9830.4) */
        tof_result->dm_sr = (nb_steps_valid * 100U)/nb_steps;
    }
}
