/*! *********************************************************************************
* Copyright 2026 NXP
*
* NXP Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from NXP.
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

/*
   Defer algorithm run
   If set to TRUE, the algorithm will not run automatically.
   Instead it is the application's responsibility to make the call.
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

/* Measurement data buffer size: ToF and IQ data for the max number of steps */
#define gMeasurementDataSizeMax_c           (4U /* HCI data header */ + (6U + 1U + 4U * (1U + gMaxNumAntennaPaths_c)) /* Mode 3 HCI length */)
#define gMeasurementBufferSize_c            (gMeasurementDataSizeMax_c * gMaxNumCsSteps_c)

#endif /* APP_LOCALIZATION_CONFIG_H */