/*! *********************************************************************************
* Copyright 2023-2025 NXP
*
* \file app_localization_algo.h
*
* This is a source file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef APP_LOCALIZATION_ALGO_H
#define APP_LOCALIZATION_ALGO_H
/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "app_localization.h"
#if defined(gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1U)
#include "ranging_interface.h"
#endif /* defined(gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1U) */
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
 /* Maximum allowable distance result in meters
    Higher values produced by the algorithm will be discarded by the application */
#ifndef gMaxDistanceMeters_c
#define gMaxDistanceMeters_c  (120U)
#endif

#define gCsChannelsNb_c       (79U)

#define gCsSubeventMax_c      (16U)

/* Mode0 data size - RSSI 8 bits + Pkt Quality 4bits + CFO 20 bits */
#define gCsMode0Sz_c          (4U)
/* ToF(Pkt) data size - RSSI 8 bits + Pkt Quality 4bits + TS_DIFF 20 bits */
#define gCsTofTsSz_c          (4U)
/* Tone IQ data size - PCT 3 bytes, Tone_Quality_Indicator 1 byte */
#define gCsMciqSz_c           (4U)

#define CS_TS_SIZE            (3U)
#define CS_RSSI_SIZE          (1U)
#define CS_NADM_SIZE          (1U)

/* Size of an IQ record */
#define IQ_SIZE                 (3U)
/* Size of a Tone Quality Indicator */
#define TQI_SIZE                (1U)
/* Size of channel encoding */
#define CHANNEL_SIZE            (1U)

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef struct engine_config_tag {
    uint8_t  n_ap;
    uint8_t  mciq_algo_flags;
    uint16_t cde_threshold;
    uint16_t cde_div_threshold;
} engine_config_t;

typedef struct mciq_result_tag {
    uint16_t nb_valid_freq;     /*!< Number of frequencies for which IQ samples are not saturated */
    int32_t cde_fp;             /*!< CDE distance estimation, fixed-point Q2.10 */
    int16_t cde_dqi;            /*!< CDE distance quality indicator, fixed-point Q2.14 */
    int32_t cde_dist_array[ISP_MAX_NO_ANTENNAS]; /*!< Distance per antenna path, fixed-point Q2.10 */
    int16_t cde_dqi_array[ISP_MAX_NO_ANTENNAS];  /*!< DQI per antenna path, fixed-point Q2.14 */
    uint16_t cde_nb_valid[ISP_MAX_NO_ANTENNAS];  /*!< Number of valid frequencies per antenna path */
    float rade_dist;           /*!< RADE distance estimation */
    float rade_dist_trk;       /*!< RADE tracking distance estimation */
    float rade_dqi;            /*!< RADE distance distance quality indicator */
    uint8_t rade_error_flag;   /*!< RADE error flag */
} mciq_result_t;

typedef struct engine_response_tag {
    bool_t is_valid; 
    mciq_result_t mciq_result;
    tof_result_t tof_result;
} engine_response_t;

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

/* Buffer storing generic CS data (steps, etc ...) that are common to both devices (and only collected on local device) */
typedef struct cs_data_tag {
    uint16_t step_nb;
    uint16_t startAclCnt;
    uint8_t subevt_nb;
    uint8_t main_mode_repeat;
    uint8_t mode0_nb;
    uint8_t rtt_type;
    uint8_t phy;
    uint8_t main_mode_type;
    uint8_t sub_mode_type;
    uint8_t t_fcs;
    uint8_t t_ip1;
    uint8_t t_ip2;
    uint8_t t_pm;
    uint8_t t_sw;
    uint16_t conn_interval;
    void *csAlgoBuf;
    uint8_t channelMap[APP_LOCALIZATION_MAX_STEPS];
    uint8_t modeMap[APP_LOCALIZATION_MAX_STEPS];
    uint8_t subevtStopIdx[gCsSubeventMax_c];
    uint8_t subevtConnEvent[gCsSubeventMax_c]; /* Delta regarding ACL counter of first subevent */
    int8_t subevtRefPowerLevelInit[gCsSubeventMax_c]; /* Reference power level per subevent */
    int8_t subevtRefPowerLevelRefl[gCsSubeventMax_c]; /* Reference power level per subevent */
    uint8_t subevtDoneStatusLocal[gCsSubeventMax_c]; /* Status for each subevent - local data */
    uint8_t subevtDoneStatusRemote[gCsSubeventMax_c]; /* Status for each subevent - remote data */
} cs_data_t;

/* Buffer storing last captured IQ and corresponding params */
typedef struct debug_data_tag {
    uint16_t step_nb;
    uint16_t n_pairs;
    uint16_t samplesPerStep;
    uint16_t samplesPerStepMode0;
    uint16_t iqBufLength;
    uint8_t *iq;
    uint16_t samples_nb[APP_LOCALIZATION_MAX_STEPS];
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

typedef struct isp_meas_response_tag {
    mciq_data_t mciq_data[2]; /* 0=initiator, 1=reflector */
    tof_data_t tof_data[2];   /* 0=initiator, 1=reflector */
    cs_data_t *cs_data;
} isp_meas_response_t;

typedef struct csAppData_tag {
    bool_t locked; /* locked for read access */
    uint32_t csStepsReceived;
    uint16_t dbgFlags;
    tof_data_t tof_data;
    mciq_data_t mciq_data;
    cs_data_t csData;

    /* runtime */
    uint16_t tofBufferOffset;
    uint16_t mciqBufferOffset;

    /* Buffers */
    uint8_t tofBuffer[gCsTofTsSz_c * APP_LOCALIZATION_MAX_STEPS];
    uint8_t mciqBuffer[gCsMciqSz_c * ISP_MAX_NO_ANTENNAS * APP_LOCALIZATION_MAX_STEPS];

#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1)
    uint8_t rssiStepNo;
    int8_t aRssiValue[APP_LOCALIZATION_MAX_STEPS];
#endif /* gAppParseRssiInfo_d */
} csAppData_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
/* Buffer to hold local and peer measurement data */
extern csAppData_t gLocalAppDataBuffer;
extern csAppData_t gRemoteAppDataBuffer;
extern csAppData_t *localAppDataBuffer;
extern csAppData_t *remoteAppDataBuffer;
/************************************************************************************
 *************************************************************************************
 *  Public prototypes
 *************************************************************************************
 ************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\fn         void AppLocalizationAlgo_RunMeasurement(deviceId_t deviceId,
*                                    rasMeasurementData_t *pLocalData,
*                                    rasMeasurementData_t *pPeerData,
*                                    uint8_t role,
*                                    localizationAlgoResult_t *pResult);
*
*\brief      Run given algorithm for given data.
*
*\param[in]  deviceId         DeviceId of the peer (used to retrieve timing info).
*\param[in]  pLocalData       Pointer to local data.
*\param[in]  pPeerData        Pointer to peer data.
*\param[in]  role             CS role of the local device (initiator or reflector).
*\param[out] pResult          Pointer to location for result information.
*
*\retval     none
********************************************************************************** */
void AppLocalizationAlgo_RunMeasurement(deviceId_t deviceId,
                                        rasMeasurementData_t *pLocalData,
                                        rasMeasurementData_t *pPeerData,
                                        uint8_t role,
                                        localizationAlgoResult_t *pResult);

/*! *********************************************************************************
*\fn         void AppLocalizationAlgo_ResetPeer(deviceId_t deviceId);
*
*\brief      Algo-related cleanup on peer disconnection.
*
*\param[in]  deviceId         DeviceId of the peer.
*
*\retval     none
********************************************************************************** */
void AppLocalizationAlgo_ResetPeer(deviceId_t deviceId);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* APP_LOCALIZATION_ALGO_H */