/*
 * @Copyright (c) 2019, IMEC
 */
/*
 * Copyright 2020-2026 NXP
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

/* Prevent double inclusion */
#ifndef _ISP_MEASUREMENT_H_
#define _ISP_MEASUREMENT_H_

/*!
 * @addtogroup genfsk_isp GENFSK Narrowband Localization
 * @{
 */

/*! @file
 * Measurement API.
 */

/* === Includes ============================================================= */
#include "EmbeddedTypes.h"
#include "isp_interface.h"

/* === Macros =============================================================== */

/* Debug helper to tag subevents boundaries in aggregated result buffer */
//#define ENABLE_SANITY_SUBEVT_SEPARATOR

#ifdef ENABLE_SANITY_SUBEVT_SEPARATOR
#define SANITY_SUBEVT_SEPARATOR_SZ (4U)
#else
#define SANITY_SUBEVT_SEPARATOR_SZ (0U)
#endif

#define gHciCsMaxStepsPerProcedure_c    (256U)
#define gHciCsMaxStepsPerSubevent_c     (160U)
#define gHciCsMaxAntenna_c     (4U)
#define gHciCsChannelMapSz_c   (79U)
#define gHciCsMaxNumPhy_c      (2U)

#define WR_HCI_CONFIG_ID (0x01U) /* Config Id used by wireless ranging app */

#define gCsSubeventMax_c (16U)
#define gCsChannelsNb_c (79U)
/* Mode0 data size - RSSI 8 bits + CFO 16 bits */
#define gCsMode0Sz_c (3U)
/* ToF(Pkt) data size - RSSI 8 bits + Pkt Quality 4bits + TS_DIFF 20 bits + Pkt_NADM 8 bits */
#define gCsTofTsSz_c (5U)
/* Tone IQ data size - PCT 3 bytes, Tone_Quality_Indicator 1 byte */
#define gCsMciqSz_c  (4U)
/* IQ samples per step, per antenna */
#define gCsIQNbPerStep_Avg     (4U)   /* 4 IQ when averaging enabled, see CS averaging config */
/* Raw buffer size: able to hold hciLeCsEventResultEvent_t message header + ToF and IQ data for max number of subevents and max number of steps per procedure */
#define gCsDataSizeMax_c  (4U /* HCI data header */ + (6U + 1U + 4U * (1U + ISP_MAX_NO_ANTENNAS)) /* Mode 3 HCI length */)
#define gCsRawBufferSz_c  (((hciLeCsEventResultEventSize_c+SANITY_SUBEVT_SEPARATOR_SZ)*CS_SUBEVT_NB_MAX) + (gCsDataSizeMax_c * CS_STEP_NB_MAX_PER_PROC))
#define gCsDebugRawBufferSz_c  (1024U)

/* Size of an IQ record */
#define IQ_SIZE 3
/* Size of a Tone Quality Indicator */
#define TQI_SIZE 1
/* Size of channel encoding */
#define CHANNEL_SIZE 1

#define CS_TS_SIZE (3U)
#define CS_RSSI_SIZE (1U)
#define CS_NADM_SIZE (1U)
/* ts_diff assumed to be in16_t: HCI TS encoded on 16 bits (2 bytes). aa_quality assumed to be uint32_t. */
#define  CS_GET_RTT_TS_DIFF(report_p, ts_diff, aa_quality) \
    { \
        ts_diff = ((report_p)[1] << 8) + (report_p)[0]; \
        aa_quality = ((report_p)[2] >> 4) & 0xF; \
    }


/* === Types ================================================================ */

/* contains derived values (from isp_parameters and others) */
typedef struct measurement_parameters_tag {
    isp_configuration_range_t cfg;
    bool is_valid;
    uint8_t *resultBuffer;
} measurement_parameters_t;

/* Buffer storing last captured IQ and corresponding params */
typedef struct debug_data_tag {
    uint16_t step_nb;
    uint16_t n_pairs;
    uint16_t samplesPerStep;
    uint16_t samplesPerStepMode0;
    uint16_t iqBufLength;
    uint8_t *iq;
    uint16_t samples_nb[gHciCsMaxStepsPerProcedure_c];
} debug_data_t;

typedef PACKED_STRUCT event_internal_tag {
    uint32_t   flags;              /*!< Error flags @see CS_flags */
    int32_t    sync_cfo;           /*!< CFO [Hz] measured during during alignment phase*/
    uint8_t    sync_step_id;       /*!< Step number of mode 0 retained for the synch phase */
    int8_t     sync_rssi;          /*!< RSSI [dB] obtained during alignment phase */
    uint8_t    sync_rxgain;        /*!< RX gain (AGC index) set during alignment phase */
    uint8_t    xtal_trim;          /*!< Xtal trim value used after alignement phase */
    uint16_t   agc_delay;          /*!< AGC group delay corresponding to selected RX gain */
    int8_t     temperature;        /*!< Temperature of device during measurement */
    uint8_t    num_time_adj;       /*!< Number of time-grid adjustement in event */
} event_internal_t;

typedef PACKED_STRUCT rtt_internal_tag
{
    uint16_t t1;
    uint16_t t2;
    uint32_t hartt_stat;
    uint32_t stat0;
} rtt_internal_t;

typedef PACKED_STRUCT mciq_internal_tag
{
    uint8_t ctune;
} mciq_internal_t;

/* Data collected via HciLeCsEventResultDebugEvent,
 * must match HCI message syntax.
 */
typedef struct event_internal_data_tag {
    uint16_t rtt_internal_nb;
    uint16_t mciq_internal_nb;
    event_internal_t *event_internal;
    rtt_internal_t *rtt_internal;
    mciq_internal_t *mciq_internal;
} event_internal_data_t;

/* Mode0 data */
typedef struct cs_mode0_data_tag {
    uint8_t *mode0_buffer; /* Array of Mode0 data: RSSI 8 bits + Pkt Quality 4bits + CFO 20 bits */
} cs_mode0_data_t;

/* Buffer storing generic CS data (steps, etc ...) that are common to both devices (and only collected on local device) */
typedef struct cs_data_tag {
    uint16_t step_nb;
    uint16_t startAclCnt;
    uint8_t status;
    uint8_t mode0_nb;
    uint8_t subevt_nb;
    uint8_t channelMap[gHciCsMaxStepsPerProcedure_c];
    uint8_t modeMap[gHciCsMaxStepsPerProcedure_c];
    uint8_t modeMapRemote[gHciCsMaxStepsPerProcedure_c];
    uint8_t subevtStopIdxLocal[gCsSubeventMax_c];   /* stepId of the beginning of the next subevent (out of range for last subevent) - local */
    uint8_t subevtStopIdxRemote[gCsSubeventMax_c];  /* stepId of the beginning of the next subevent (out of range for last subevent) - remote */
    uint8_t subevtConnEvent[gHciCsMaxStepsPerProcedure_c]; /* Delta regarding ACL counter of first subevent */
    int8_t subevtRefPowerLevelInit[gCsSubeventMax_c]; /* Reference power level per subevent */
    int8_t subevtRefPowerLevelRefl[gCsSubeventMax_c]; /* Reference power level per subevent */
    uint8_t subevtDoneStatusLocal[gCsSubeventMax_c]; /* Status for each subevent - local data */
    uint8_t subevtDoneStatusRemote[gCsSubeventMax_c]; /* Status for each subevent - remote data */
} cs_data_t;

typedef struct  {
    uint16_t nbSteps;      /*!< nb of MCIQ steps */
    uint8_t n_ap;          /*!< nb of antenna paths */
    uint8_t *iq;           /*! Array of IQ samples, one IQ per frequency */
    uint8_t *channels;     /*! channels per step */
} mciq_data_t;

/*! ToF raw -frequency data.
 */
typedef struct  {
    uint16_t nbSteps;      /*!< nb of Tof steps */
    uint8_t *ts;           /*!< Array of timestamps delta (RX-TX on reflector, Tx-RX on initiator), OR'd with TSflags */
} tof_data_t;

typedef struct isp_meas_response_tag {
    measurement_parameters_t *mparams;
    mciq_data_t mciq_data[2]; /* 0=initiator, 1=reflector */
    tof_data_t tof_data[2];   /* 0=initiator, 1=reflector */
    cs_mode0_data_t mode0_data[2]; /* 0=initiator, 1=reflector */
    debug_data_t *debug_data[2]; /* 0=initiator, 1=reflector */
    event_internal_data_t *internal_data[2];
    cs_data_t *cs_data;
} isp_meas_response_t;

/* === Externals ========================================== */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

/*! @} */

#endif /* _ISP_MEASUREMENT_H_ */

/* EOF */
