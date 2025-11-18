/*
 * @Copyright (c) 2019, IMEC 
 */
/*
 * Copyright 2020-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Prevent double inclusion */
#ifndef _ISP_INTERFACE_H_
#define _ISP_INTERFACE_H_

/*!
 * @addtogroup genfsk_isp GENFSK Narrowband Localization
 * @{
 */

/*! @file
 * Types and macros to interface with localization API.
 */

/* === Includes ============================================================= */
#include <stdbool.h>
#include "fsl_device_registers.h"
#include "gap_types.h"


/* === Macros =============================================================== */
#if (defined(KW37A4_SERIES) || defined(KW37Z4_SERIES) || defined(KW38A4_SERIES) || defined(KW38Z4_SERIES) || defined(KW39A4_SERIES) || defined(K32W232H_SERIES) || defined(KW45B41Z83_SERIES))
#else
#ifndef   __ALIGNED
#define __ALIGNED(x)                           __attribute__((aligned(x)))
#endif
#endif
#define _BV(a) (1<<a)

/**
 *The maximum number of connections
 */
#if defined(KW45B41Z82_SERIES) || defined(KW45B41Z83_SERIES) || defined(K32W1480_SERIES) || defined(CPU_KW45B41Z83AFPA_NBU) || defined(MCXW716A_SERIES) || defined(MCXW716C_SERIES) || defined(KW47B42ZB7_cm33_core0_SERIES) || defined(MCXW727C_cm33_core0_SERIES)
    /* Ensure this is equal to gAppMaxConnections_c on app side (to be revisited with CS) */
    #define gLclMaxConnections_c 8
    #if !defined(CPU_KW45B41Z83AFPA_NBU) // Check consistency bw CM33 and NBU
        #if (gLclMaxConnections_c != gAppMaxConnections_c)
        #error "Defined values should be identical for gLclMaxConnections_c and gAppMaxConnections_c"
        #endif
    #endif
#else
    #define gLclMaxConnections_c gAppMaxConnections_c
#endif

/**
 * The maximum number of antenna pairs used.
 */
#define ISP_MAX_NO_ANTENNAS             (4U)

/*! Types of embedded algorithms that can be configured - flags */
#define eMciqAlgoEmbedCDE  BIT0
#define eMciqAlgoEmbedRADE BIT2

/*! Default values for zero distance calibration in meters, fixed-point Q10 */
#define ZERO_DIST_CAL_MCIQ  (0)
#define ZERO_DIST_CAL_TOF   (0)

/*! @name Debug_flags
 *  Flags used to configure some debug during a measurement.
 * @{
 */
#define ISP_DBG_IQ_DETAILS          (_BV(0))
#define ISP_DBG_IQ_AVG_OFF          (_BV(1))
#define ISP_DBG_FLG_DBG_INFO        (_BV(2))
#define ISP_DBG_CFO_COMP_DISABLE    (_BV(3))
/*! @} */

/* XCVR characteristics */
#define XCVR_F_RANGE 84
#define XCVR_CHAN_MAX                   (78U)
#define XCVR_CHAN_MIN                   (0)
#define XCVR_TX_PWR_MAX                 (gAppMaxTxPowerDbm_c)
#define XCVR_TX_PWR_MIN                 (-12)

/* CS */
#define CS_STEP_NB_MAX_PER_SE (160U)
#define CS_STEP_NB_MAX_PER_PROC (256U)
#define CS_STEP_NB_MODE0_MAX 3
#define CS_MAIN_MODE_REPEAT_MAX (3U)
#define CS_CH_MAP_LEN  10
#define CS_SUBEVT_NB_MAX 32U

#define BLE_ADV_AA 0x8E89BED6
#define BLE_DTM_AA 0x71764129

/* === Types ================================================================ */

/*! Verbosity levels for application print and debug @see isp_configuration_local_t::verbosity */
typedef enum isp_verbosity_flags_tag {
    eVerbosityInfo                  = 0x01,     /*!< Information */
    eVerbosityDebug                 = 0x02,     /*!< Debug */
    eVerbosityMeasurementInfo       = 0x04,     /*!< Measurement Information (gain, cfo, RSSI ...) */
    eVerbosityMeasurementData       = 0x08,     /*!< Measurement raw data (IQ or RTT) */
    eVerbosityMeasurementDebug      = 0x10,     /*!< Measurement debug information */
    eVerbosityBoardInfo             = 0x20,     /*!< Information on the boards */
    eVerbosityMeasurementProfiling  = 0x40,     /*!< Measurement profiling information */
} isp_verbosity_flags_t;

/*! Structure holding configuration of a ranging measurement. */
typedef struct isp_configuration_range_tag
{
    uint16_t max_proc_count;    /*!< Number of procedure repeat */
    uint16_t proc_interval;     /*!< Procedure interval (us) */
    uint32_t subevent_len;      /*!< CS subevent length (us) */
    uint8_t role;               /*!< CS role, 0=initiator, 1=reflector */
    uint8_t main_mode_type;     /*!< Main mode of CS steps */
    uint8_t sub_mode_type;      /*!< Sub mode of CS steps */
    uint8_t main_mode_min;      /*!< Minimum number of Main mode steps */
    uint8_t main_mode_max;      /*!< Maximum number of Main mode steps */
    uint8_t main_mode_repeat;   /*!< number of Main mode steps to be inserted after a mode0 */
    uint8_t mode0_nb;           /*!< Number of mode0 steps to be inserted at each event */
    uint8_t rtt_type;           /*!< RTT type */
    uint8_t rtt_phy;            /*!< RTT Phy 0=1Mbps, 1=2Mbps */
    int8_t tx_pwr;              /*!< Transmit power during measurement in dB [-12,4] */
    uint8_t t_fcs;              /*!< CS T_FCS */
    uint8_t t_ip1;              /*!< CS T_IP1 */
    uint8_t t_ip2;              /*!< CS T_IP2 */
    uint8_t t_pm;               /*!< CS T_PM */
    uint8_t t_sw_remote;        /*!< CS T_SW remote */
    uint8_t t_sw_local;         /*!< CS T_SW local */
    uint8_t t_sw;               /*!< CS Actual T_SW used by procedure */
    uint8_t t_pm_tone_ext;      /*!< CS T_PM tone extension */
    bool_t ch_list_auto;        /*!< set to TRUE when ch_list has to be auto-generated */
    bool_t ch_isrand;           /*!< set to TRUE when auto-generated ch_list must be random */
    uint8_t ch_start;           /*!< First channel used for ch_list generation */
    uint8_t ch_stop;            /*!< Last channel used for ch_list generation */
    uint8_t ch_nb;              /*!< Number of channels in the channel list */
    uint8_t ch_list[CS_STEP_NB_MAX_PER_SE]; /*!< List of channels used for CS Test mode */
    uint8_t ch_map[CS_CH_MAP_LEN];   /*!< Bitmask for channels 0-78 */
    uint8_t ch_map_repeat;      /*!< channel map repetition */
    uint8_t ch_sel_algo;        /*!< channel selection algorithm */
    uint8_t ch_sel_shape;       /*!< channel selection shape for algorithm #3c */
    uint8_t ch_sel_jump;        /*!< channel selection jump for algorithm #3c */
    uint8_t debug;              /*!< debug flags, @see Debug_flags */
    uint8_t test_mode;          /*!< Test mode: reflector will wait on first sync packet */
    uint8_t ant_cfg_index;      /*!< Antenna configuration index 0-7 */
    uint8_t ant_perm_index;     /*!< Antenna permutation index 0-23, 0xFF = loop */
    uint8_t ant_CS_SYNC;        /*!< Antenna index for CS_SYNC packets 1-2, 0xFE = round-robin, 0xFF = propietary round-robin */
    uint8_t ant_type;           /*!< Antenna div board type 0:none, 1:X-FR-ANTDIV SMA, 2:X-FR-ANTDIV printed */
    uint8_t initiator_AA[4];    /*!< Initiator AA */
    uint8_t reflector_AA[4];    /*!< Reflector AA */
    uint16_t connInterval;      /*!< Connection interval */
} isp_configuration_range_t;

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

/*! @} */

#endif /* _ISP_INTERFACE_H_ */
/* EOF */
