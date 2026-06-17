/*! *********************************************************************************
* Copyright 2026 NXP
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
#ifndef APP_LOCALIZATION_CONFIG_H
#define APP_LOCALIZATION_CONFIG_H
/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
/* Maximum data size for a CS subevent */
#ifndef gMaxCsSubeventDataSize_c
#define gMaxCsSubeventDataSize_c            (2300U)
#endif

 /* Rough estimation of worst case CS procedure duration for WR
 (depends on subevent fragmentation, n_ap ..., assumes 4 AP, 80 channels in a single subevent) */
#define gMaxCsProcDurationMs_c              (40U)

/* Estimated print durations for procedure repeat frequency calculation */
#define gPostProcVerbDurationMs_c           (20U)

/* Application duration offset (varies by scenario) */
#define gAppOffsetDurationMs_c              (500U)

#if defined (BOARD_LOCALIZATION_REVISION_SUPPORT) && (BOARD_LOCALIZATION_REVISION_SUPPORT == 1U)
#define gLocBoardDelayMs_c                  (40U)
#endif

/* Procedure Repeat: Max Number of Procedures */
#ifndef gCsProcRepeatMaxNumProcedures_c
#define gCsProcRepeatMaxNumProcedures_c     (5U)
#endif

/* Enable/Disable the linking of the CDE algorithm */
#ifndef gAppUseCDEAlgorithm_d
#define gAppUseCDEAlgorithm_d               (1U)
#endif

/* Enable/Disable the linking of the RADE algorithm */
#ifndef gAppUseRADEAlgorithm_d
#define gAppUseRADEAlgorithm_d              (1U)
#endif

/* Enable/Disable the linking of the slope-based distance estimation for calibration
   This feature is experimental */
#ifndef gAppSlopeBasedCalibrationSupport_d
#define gAppSlopeBasedCalibrationSupport_d  (0U)
#endif

/*
   Defer algorithm run
   If set to TRUE, the algorithm will not run automatically.
   Instead it is the application's responsibility to:
      1. Trigger the algorithm run via AppLocalization_RunAlgorithm()
      2. Call RasClient_ResetPeer()/BtcsClient_ResetPeer() to cleanup peer data
      3. Call AppLocalization_FreeLocalData() to cleanup local data
*/
#ifndef gAppDeferAlgoRun_d
#define gAppDeferAlgoRun_d                  FALSE
#endif

/* Maximum number of CS steps in a procedure */
#ifndef gMaxNumCsSteps_c
#define gMaxNumCsSteps_c                    (160U)
#endif

/* Maximum number of mode-0 CS steps in a subevent */
#ifndef gMaxNumCsStepsMode0_c
#define gMaxNumCsStepsMode0_c               (3U)
#endif

/* Maximum number of CS subevents in a procedure */
#ifndef gMaxNumCsSubevents_c
#define gMaxNumCsSubevents_c                (10U)
#endif

/* The maximum number of antenna paths used. */
#define gMaxNumAntennaPaths_c               (4U)

/* Number of antennas and antenna paths used at initialization.
   On KW47 / MCXW72 with Inline PCT Transfer (IPT) enabled, only the 1x1
   antenna configuration is supported, so a single antenna and a single
   antenna path are used. Otherwise 4 antenna paths are always used, and the
   number of antennas depends on the board: 2 antennas when the localization
   revision board is supported, 1 antenna otherwise. */
#if (defined(KW47_core0_SERIES) || defined(MCXW72_core0_SERIES)) && \
    defined(gAppUseInlinePctTransfer_d) && (gAppUseInlinePctTransfer_d == 1U)
#define gNumAntennaPaths_c                  (1U)
#define gNumAntennas_c                      (1U)
#else
#define gNumAntennaPaths_c                  (4U)
#if defined(BOARD_LOCALIZATION_REVISION_SUPPORT) && (BOARD_LOCALIZATION_REVISION_SUPPORT == 1U)
#define gNumAntennas_c                      (2U)
#else
#define gNumAntennas_c                      (1U)
#endif
#endif

/* Measurement data buffer size: ToF and IQ data for the max number of steps */
#define gMeasurementDataSizeMax_c           (4U /* HCI data header */ + (6U + 1U + 4U * (1U + gMaxNumAntennaPaths_c)) /* Mode 3 HCI length */)
#define gMeasurementBufferSize_c            (gMeasurementDataSizeMax_c * gMaxNumCsSteps_c)

/* Maximum concurrent CS procedures */
#ifndef gChannelSoundingMaxConcurrentProcedures_c
#define gChannelSoundingMaxConcurrentProcedures_c  (1U)
#endif

/* Adaptive CS procedure interval based on RSSI.
   When enabled (and the procedure auto-restart loop is active), the remote RSSI
   average of each procedure iteration is used to adjust the CS procedure
   interval for the next iteration:
     - good RSSI -> decrease interval (faster distance report rate)
     - bad RSSI  -> increase interval (more time for the RAS transfer to finish)
   This feature requires gAppParseRssiInfo_d to be enabled (the remote RSSI
   samples are only available in the algorithm result when RSSI parsing is on). */
#ifndef gAppAdaptiveProcInterval_d
#define gAppAdaptiveProcInterval_d          (0U)
#endif

#if defined(gAppAdaptiveProcInterval_d) && (gAppAdaptiveProcInterval_d == 1U)
#if !defined(gAppParseRssiInfo_d) || (gAppParseRssiInfo_d != 1U)
#error "gAppAdaptiveProcInterval_d requires gAppParseRssiInfo_d to be enabled"
#endif

/* Remote RSSI average (signed dBm) at or above which the link is considered good
   and the CS procedure interval can be decreased. */
#ifndef gAdaptiveRssiGoodThreshold_d
#define gAdaptiveRssiGoodThreshold_d        (-60)
#endif

/* Remote RSSI average (signed dBm) at or below which the link is considered bad
   and the CS procedure interval should be increased. */
#ifndef gAdaptiveRssiBadThreshold_d
#define gAdaptiveRssiBadThreshold_d         (-80)
#endif

/* Substitute remote RSSI average (signed dBm) assigned to a CS procedure that did
   not complete during a loop iteration. Procedures that fail to produce a valid
   RSSI are treated as a very bad link so that an unstable connection increases
   the interval. Kept below gAdaptiveRssiBadThreshold_d on purpose. */
#ifndef gAdaptiveRssiNotCompleted_d
#define gAdaptiveRssiNotCompleted_d         (-100)
#endif

#if (gAdaptiveRssiBadThreshold_d >= gAdaptiveRssiGoodThreshold_d)
#error "gAdaptiveRssiBadThreshold_d must be less than gAdaptiveRssiGoodThreshold_d"
#endif

/* Step (in connection interval units) by which the CS procedure interval is
   adjusted on each iteration. */
#ifndef gAdaptiveProcIntervalStep_d
#define gAdaptiveProcIntervalStep_d         (1U)
#endif

/* Lower bound (in connection interval units) for the adaptive procedure interval. */
#ifndef gAdaptiveProcIntervalMin_d
#define gAdaptiveProcIntervalMin_d          (10U)
#endif

/* Upper bound (in connection interval units) for the adaptive procedure interval. */
#ifndef gAdaptiveProcIntervalMax_d
#define gAdaptiveProcIntervalMax_d          (100U)
#endif
#endif /* gAppAdaptiveProcInterval_d */

#endif /* APP_LOCALIZATION_CONFIG_H */
