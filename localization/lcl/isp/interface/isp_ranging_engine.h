/*
 * @Copyright (c) 2019, IMEC 
 */
/*
 * Copyright 2020-2021, 2023-2026 NXP
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

#ifndef _ISP_RANGING_H_
#define _ISP_RANGING_H_

#include "isp_configuration.h"
#include "isp_measurement.h"

/* === Macros =============================================================== */

/* === Types ================================================================ */

typedef struct mciq_result_tag {
    uint16_t nb_valid_freq;     /*!< Number of frequencies for which IQ samples are not saturated */
    int32_t cde_fp;             /*!< CDE distance estimation, fixed-point Q2.10 */
    int16_t cde_dqi;            /*!< CDE distance quality indicator, fixed-point Q2.14 */
    int32_t cde_dist_array[ISP_MAX_NO_ANTENNAS]; /*!< Distance per antenna path, fixed-point Q2.10 */
    int16_t cde_dqi_array[ISP_MAX_NO_ANTENNAS];  /*!< DQI per antenna path, fixed-point Q2.14 */
    uint16_t cde_nb_valid[ISP_MAX_NO_ANTENNAS];  /*!< Number of valid frequencies per antenna path */
    float rade_dist;           /*!< RADE raw distance estimation */ 
    float rade_dist_trk;       /*!< RADE tracking distance estimation */
    float rade_dqi;            /*!< RADE distance distance quality indicator */
    uint8_t rade_error_flag;   /*!< RADE error flag */
} mciq_result_t;

/*! Contains measurement derived values (from isp_parameters and others) */
typedef struct tof_result_tag {
    uint8_t dm_sr;  /*!< Succes rate of ToF packet exchanges [%] */
    int32_t dm_ad;  /*!< Estimated average distance [m], fixed-point s15.16 */
} tof_result_t;

typedef struct engine_response_tag {
    bool_t is_valid; 
    mciq_result_t mciq_result;
    tof_result_t tof_result;
} engine_response_t;

typedef struct engine_config_tag {
    uint8_t  n_ap;
    uint8_t  mciq_algo_flags;
    uint16_t cde_threshold;
    uint16_t cde_div_threshold;
} engine_config_t;

/* === Externals ============================================================ */
extern void *csAlgoBuf;
/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

void isp_mciq_ranging_compute(isp_meas_response_t *meas_response, mciq_result_t *mciq_result, engine_config_t *engine_config);

void isp_tof_ranging_compute(isp_meas_response_t *meas_response, tof_result_t *tof_result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _ISP_RANGING_H_ */
