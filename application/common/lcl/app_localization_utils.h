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
#ifndef APP_LOCALIZATION_UTILS_H
#define APP_LOCALIZATION_UTILS_H
/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "app_localization.h"
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
/* Sizes of fields in mode-1 CS steps */
#define gCsTsSize_c             (3U)
#define gCsRssiSize_c           (1U)
#define gCsNadmSize_c           (1U)

/* ToF(Pkt) data size - RSSI 8 bits + Pkt Quality 4bits + TS_DIFF 20 bits */
#define gCsTofTsSize_c          (4U)
/* Tone IQ data size - PCT 3 bytes, Tone_Quality_Indicator 1 byte */
#define gCsMciqSize_c           (4U)

#define gIQSampleSize_c         (12U)
#define gTimeStampDiffSize_c    (20U)

/* Size of Tone_PCT field for mode 2 data */
#define gTone_PCTSize_c         (3U)

/* Size of Packet_PCT field for mode 1 and 3 data */
#define gPacket_PCTSize_c       (4U)
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
    int32_t cde_dist_array[gMaxNumAntennaPaths_c]; /*!< Distance per antenna path, fixed-point Q2.10 */
    int16_t cde_dqi_array[gMaxNumAntennaPaths_c];  /*!< DQI per antenna path, fixed-point Q2.14 */
    uint16_t cde_nb_valid[gMaxNumAntennaPaths_c];  /*!< Number of valid frequencies per antenna path */
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

/*! Mode0 step data.
 */
typedef struct  {
    uint8_t quality;             /*!< Packet Quality */
    int8_t  rssi;                /*!< Packet RSSI */
    uint8_t antenna;             /*!< Packet Antenna */
    uint16_t measuredFreqOffset; /*!< Measured Frequency Offset (initiator only) */
} mode0_data_t;

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
    uint8_t channelMap[gMaxNumCsSteps_c];
    uint8_t modeMap[gMaxNumCsSteps_c];
    uint8_t *modeMapRemote;
    uint8_t subevtStopIdxLocal[gMaxNumCsSubevents_c];
    uint8_t subevtStopIdxRemote[gMaxNumCsSubevents_c];
    uint8_t subevtConnEvent[gMaxNumCsSubevents_c]; /* Delta regarding ACL counter of first subevent */
    int8_t subevtRefPowerLevelInit[gMaxNumCsSubevents_c]; /* Reference power level per subevent */
    int8_t subevtRefPowerLevelRefl[gMaxNumCsSubevents_c]; /* Reference power level per subevent */
    uint8_t subevtDoneStatusLocal[gMaxNumCsSubevents_c]; /* Status for each subevent - local data */
    uint8_t subevtDoneStatusRemote[gMaxNumCsSubevents_c]; /* Status for each subevent - remote data */
    mode0_data_t mode0Data[2U * gMaxNumCsStepsMode0_c]; /* Mode0 step data - local and remote */
} cs_data_t;

/* Buffer storing last captured IQ and corresponding params */
typedef struct debug_data_tag {
    uint16_t step_nb;
    uint16_t n_pairs;
    uint16_t samplesPerStep;
    uint16_t samplesPerStepMode0;
    uint16_t iqBufLength;
    uint8_t *iq;
    uint16_t samples_nb[gMaxNumCsSteps_c];
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
    uint8_t tofBuffer[gCsTofTsSize_c * gMaxNumCsSteps_c];
    uint8_t mciqBuffer[gCsMciqSize_c * gMaxNumAntennaPaths_c * gMaxNumCsSteps_c];

#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1)
    uint8_t rssiStepNo;
    int8_t aRssiValue[gMaxNumCsSteps_c];
#endif /* gAppParseRssiInfo_d */
} csAppData_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
extern const uint8_t gaAntPermNAp[24][4];
/************************************************************************************
 *************************************************************************************
 *  Public prototypes
 *************************************************************************************
 ************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void* AppLocalizationAlgo_AllocData(void);

#if (defined (gRasRREQ_d) && (gRasRREQ_d == 1U))
/*! *********************************************************************************
*\fn         void AppLocalizationAlgo_UncompressRemoteResponse(uint8_t* pEventData,
*            uint32_t dataLength, rasMeasurementData_t *pRemoteData, bool_t lastSegment);
*
*\brief      Uncompress CS data, received from the peer, on-the-fly.
*
*\param[in]  pData               Pointer to the received chhunk of data.
*\param[in]  dataLength          Size of the received data chunk.
*\param[out] pRemoteData         Pointer to rasMeasurementData_t structure containing 
*                                the unpacked data information.
*\param[in]  lastSegment         Is this the last segment or not.
*
*\retval     none
********************************************************************************** */
void AppLocalizationAlgo_UncompressRemoteResponse
(
    uint8_t *pEventData,
    uint32_t dataLength,
    rasMeasurementData_t *pRemoteData,
    bool_t lastSegment
);
#elif (defined (gAppBtcsClient_d) && (gAppBtcsClient_d == 1U))
/*! *********************************************************************************
*\fn        uint32_t AppLocalizationAlgo_UncompressRemoteResponseL2CAP(uint8_t *pEventData,
            uint32_t dataLength, rasMeasurementData_t *pRemoteData, uint8_t maxSteps);
*
*\brief     Uncompress BTCS Ranging Data for a given device
*
*\param[in] pEventData          Pointer to the received chhunk of data.
*\param[in] dataLength          Size of the received data chunk.
*\param[in] pRemoteData         Pointer to rasMeasurementData_t structure containing 
*                               the unpacked data information.
*\param[in] maxSteps            Maximum number of steps to unpack 
 ********************************************************************************** */
uint32_t AppLocalizationAlgo_UncompressRemoteResponseL2CAP
(
    uint8_t *pEventData,
    uint32_t dataLength,
    rasMeasurementData_t *pRemoteData,
    uint8_t maxSteps
);
#endif

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
);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* APP_LOCALIZATION_UTILS_H */