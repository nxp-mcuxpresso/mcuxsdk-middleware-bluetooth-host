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
#define gCsAntennaSize_c        (1U)

/* ToF(Pkt) data size - NADM 8 bits + RSSI 8 bits + Pkt Quality 4bits + TS_DIFF 20 bits + Packet_Antenna 8 bits */
#define gCsTofTsSize_c          (6U)
/* Tone IQ data size - PCT 3 bytes, Tone_Quality_Indicator 1 byte */
#define gCsMciqSize_c           (4U)

#define gIQSampleSize_c         (12U)
#define gTimeStampDiffSize_c    (20U)

/* Size of Tone_PCT field for mode 2 data */
#define gTone_PCTSize_c         (3U)

/* Size of Packet_PCT field for mode 1 and 3 data */
#define gPacket_PCTSize_c       (4U)

/* Every step in the ranging-data body carries a 1-byte step-mode prefix. */
#define gRangingStepModeHdrSize_c (1U)

/* Worst-case tone data per step: one IQ PCT plus one quality byte for every
   antenna path, with one extra path slot in the switching sequence. */
#define gRangingToneBlockSize_c ((gMaxNumAntennaPaths_c + 1U) * (gTone_PCTSize_c + 1U))

/* Per-step ranging-data body size by CS mode (worst case, all fields kept).
   Mode 0: quality + rssi + antenna + measured frequency offset.
   Mode 1: quality + nadm + rssi + ToA/ToD + antenna + PCT1 + PCT2.
   Mode 2: antenna permutation index + tone block.
   Mode 3: mode-1 fields + antenna permutation index + tone block. */
#define gRangingStepSizeMode0_c (gRangingStepModeHdrSize_c + 5U)
#define gRangingStepSizeMode1_c (gRangingStepModeHdrSize_c + 12U)
#define gRangingStepSizeMode2_c (gRangingStepModeHdrSize_c + 1U + gRangingToneBlockSize_c)
#define gRangingStepSizeMode3_c (gRangingStepModeHdrSize_c + 13U + gRangingToneBlockSize_c)

/* Fixed framing overhead: procedure and segment headers, plus per-subevent
   header. Generous upper bound for both RAS and BTCS output framings. */
#define gRangingDataProcOverhead_c   (32U)
#define gRangingDataSubevtOverhead_c (16U)

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
    int32_t sde_dist_array[gMaxNumAntennaPaths_c]; /*!< Distance per antenna path, fixed-point Q16.15*/
    int16_t sde_dqi_array[gMaxNumAntennaPaths_c];  /*!< DQI per antenna path, fixed-point Q15 */
    uint16_t nb_valid[gMaxNumAntennaPaths_c];  /*!< Number of valid frequencies per antenna path */
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
    uint16_t subevt_interval;
    void *csAlgoBuf;
    uint8_t channelMap[gCsMaxStepsPerProcedure_c];
    uint8_t modeMap[gCsMaxStepsPerProcedure_c];
    uint8_t *modeMapRemote;
    uint8_t subevtStopIdxLocal[gMaxNumCsSubevents_c];
    uint8_t subevtStopIdxRemote[gMaxNumCsSubevents_c];
    uint8_t subevtConnEvent[gMaxNumCsSubevents_c]; /* Delta regarding ACL counter of first subevent */
    int8_t subevtRefPowerLevelInit[gMaxNumCsSubevents_c]; /* Reference power level per subevent */
    int8_t subevtRefPowerLevelRefl[gMaxNumCsSubevents_c]; /* Reference power level per subevent */
    uint8_t subevtDoneStatusLocal[gMaxNumCsSubevents_c]; /* Status for each subevent - local data */
    uint8_t subevtDoneStatusRemote[gMaxNumCsSubevents_c]; /* Status for each subevent - remote data */
    uint8_t subevtAbortReasonLocal[gMaxNumCsSubevents_c]; /* Abort reason for each subevent - local data */
    uint8_t subevtAbortReasonRemote[gMaxNumCsSubevents_c]; /* Abort reason for each subevent - remote data */
    mode0_data_t *mode0DataLocal;  /* Mode0 step data - local */
    mode0_data_t *mode0DataRemote; /* Mode0 step data - remote */
    uint8_t mode0_total_local;     /* Total mode0 steps collected - local */
    uint8_t mode0_total_remote;    /* Total mode0 steps collected - remote */
} cs_data_t;

typedef struct isp_meas_response_tag {
    mciq_data_t mciq_data[2]; /* 0=initiator, 1=reflector */
    tof_data_t tof_data[2];   /* 0=initiator, 1=reflector */
    cs_data_t *cs_data;
} isp_meas_response_t;

typedef struct csAppData_tag {
    tof_data_t tof_data;
    mciq_data_t mciq_data;
    cs_data_t csData;

    /* runtime */
    uint16_t tofBufferOffset;
    uint16_t mciqBufferOffset;
    uint16_t mode0BufferOffset;

    /* Buffers. pTofBuffer and pMciqBuffer are allocated together with this struct
       in a single block; their byte sizes depend on ch_map_repeat and the CS
       mode. pTofBuffer is NULL when neither main nor sub mode is 1 or 3. */
    uint8_t *pTofBuffer;
    uint8_t *pMciqBuffer;
    uint16_t tofBufferSize;
    uint16_t mciqBufferSize;
    uint32_t allocatedDataSize; /* Total size of this block plus its ToF/IQ buffers */

    mode0_data_t mode0Buffer[gMaxNumCsStepsMode0_c * gMaxNumCsSubevents_c];

#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1)
    uint8_t rssiStepNo;
    int8_t aRssiValue[gCsMaxStepsPerProcedure_c];
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

/*! *********************************************************************************
*\fn         void* AppLocalizationAlgo_AllocData(deviceId_t deviceId);
*
*\brief      Allocate a csAppData_t together with its ToF/IQ buffers in one block.
*            Buffer sizes are derived from mRangeSettings[deviceId]; pTofBuffer is
*            only allocated when the main or sub mode is 1 or 3.
*
*\param[in]  deviceId            Peer identifier used to size the buffers.
*
*\retval     void*               Pointer to the allocated csAppData_t, or NULL.
********************************************************************************** */
void* AppLocalizationAlgo_AllocData(deviceId_t deviceId);

/*! *********************************************************************************
*\fn         uint16_t AppLocalizationAlgo_GetStepCount(deviceId_t deviceId);
*
*\brief      Compute the worst-case CS step count for a peer from mRangeSettings,
*            capped at gCsMaxStepsPerProcedure_c.
*
*\param[in]  deviceId            Peer identifier used for the channel map settings.
*
*\retval     uint16_t            Estimated number of CS steps in one procedure.
********************************************************************************** */
uint16_t AppLocalizationAlgo_GetStepCount(deviceId_t deviceId);

/*! *********************************************************************************
*\fn         uint16_t AppLocalizationAlgo_GetRangingDataSize(deviceId_t deviceId);
*
*\brief      Compute the ranging-data body buffer size for a peer from
*            mRangeSettings. Per-step size uses the larger of the configured main
*            and sub mode; total adds fixed procedure and per-subevent framing.
*
*\param[in]  deviceId            Peer identifier used for the CS configuration.
*
*\retval     uint16_t            Ranging-data body size in bytes.
********************************************************************************** */
uint16_t AppLocalizationAlgo_GetRangingDataSize(deviceId_t deviceId);

/*! *********************************************************************************
*\fn         void AppLocalizationAlgo_ClearData(void *pData);
*
*\brief      Reset a csAppData_t for a new procedure while preserving the embedded
*            buffer pointers and their sizes, and re-zeroing the buffer contents.
*
*\param[in]  pData               Pointer to the csAppData_t to clear.
*
*\retval     none
********************************************************************************** */
void AppLocalizationAlgo_ClearData(void *pData);

#if (defined (gRasRREQ_d) && (gRasRREQ_d == 1U))
/*! *********************************************************************************
*\fn         bool_t AppLocalizationAlgo_UncompressRemoteResponse(uint8_t* pEventData,
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
*\retval     TRUE                Remote ranging data was corrupted (invalid CS step mode);
*                                the in-progress procedure state has been dropped and the
*                                caller must reset the peer and skip running the algorithm.
*\retval     FALSE               Data parsed successfully (or segment still incomplete).
********************************************************************************** */
bool_t AppLocalizationAlgo_UncompressRemoteResponse
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
*\param[out] pbInvalidMode      Set to TRUE on invalid CS step mode (drop procedure)
 ********************************************************************************** */
uint32_t AppLocalizationAlgo_UncompressRemoteResponseL2CAP
(
    uint8_t *pEventData,
    uint32_t dataLength,
    rasMeasurementData_t *pRemoteData,
    uint8_t maxSteps,
    bool_t *pbInvalidMode
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
