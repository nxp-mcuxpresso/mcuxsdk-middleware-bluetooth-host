/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* \file app_preinclude.h
*
* Copyright 2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _APP_PREINCLUDE_H_
#define _APP_PREINCLUDE_H_

/*!
 *  Application specific configuration file only
 *  Board Specific Configuration shall be added to board.h file directly such as :
 *  - Number of button on the board,
 *  - Number of LEDs,
 *  - etc...
 */
/*! *********************************************************************************
 *     Board Configuration
 ********************************************************************************** */
/* Number of Button required by the application */
#define gAppButtonCnt_c                 1

/* Number of LED required by the application */
#define gAppLedCnt_c                    2

/*! Enable Debug Console (PRINTF) */
#define gDebugConsoleEnable_d           0

#define gNcpApplication_d               1

#define gFsciOverRpmsg_c                1

#define gFsciIncluded_c                 1

#define gAppUseNvmNcp_d                 1

#define gAppRasDataTransfer_d           1

#define gAppRunAlgo_d                   1

#define gFSCI_ResetCpu_c -              0

/*! *********************************************************************************
 *     App Configuration
 ********************************************************************************** */
/*! Enable/disable use of bonding capability */
#define gAppUseBonding_d                1

/*! Enable/disable use of pairing procedure */
#define gAppUsePairing_d                1

/*! Enable/disable use of privacy */
#define gAppUsePrivacy_d                1

/*! Repeated Attempts - Mitigation for pairing attacks */
#define gRepeatedAttempts_d             0

#define gPasskeyValue_c                 999999

/*! Advertising interval */
#define gcAdvertisingInterval_c         42

/*! Display distance measurement related timing information */
#define gAppCsTimeInfo_d                0

/*! Display RSSI and Tone Quality Indicator information */
#define gAppParseQualityInfo_d          0

#define gcScanWindow_c                  18U /* 96 ms */
#define gcScanInterval_c                36U /* 550 ms */
#define gcConnectionInterval_c          24U /* 30 ms, interval = gcConnectionInterval_c * 1.25ms */


/*! *********************************************************************************
 *     Framework Configuration
 ********************************************************************************** */
/*! The minimum heap size needed (measured with MEM_STATISTICS) */
#define MinimalHeapSize_c                17000U
#define SHELL_BUFFER_SIZE               (128U)
#define SHELL_TASK_STACK_SIZE           (1400U)

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */
#define gAdvSetHandle_c                   0x00U
#define gNoAdvSetHandle_c                       0xFF

#define gAppMaxConnections_c                    1

#define gcGapMaximumActiveConnections_c         gAppMaxConnections_c

#define gAppUseSerialManager_c                  1

/* Enable BLE 5.0 */
#define gBLE50_d                                1

/* Enable 5.1 features */
#define gBLE51_d                                1

/* Enable 5.2 features */
#define gBLE52_d                                1

/* Disable GATT caching */
#define gGattCaching_d                          0

/* Disable GATT automatic robust caching */
#define gGattAutomaticRobustCachingSupport_d    0

/* Enable Channel Sounding */
#define gBLE_ChannelSounding_d                  1

#define gAppIsPeripheral_d                      0U

#if gAppIsPeripheral_d
#define gCentralInitiatedPairing_d
#endif

/* Channel Sounding role - default initiator */
#define gCsDefaultRole_c                        gCsRoleInitiator_c

/* Enable the RAS role of Ranging Requestor */
#define gRasRREQ_d                              1

/* Enalbe Real-Time Data Transfer */
#define gAppRealTimeDataTransfer_c              0

/* Configure high speed CPU clock (96 MHz) */
#define gAppHighSystemClockFrequency_d          1

#define gHost_TaskStackSize_c                   1800

#define gMainThreadStackSize_c                  3800

#define gAppTaskWaitTimeout_ms_c                osaWaitForever_c

#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
#define gSecLibAllowLtkFromBlob_c       1
#endif

#define gPlatformUseLptmr_d             1

/*! *********************************************************************************
 *     BLE LL Configuration
 ***********************************************************************************/

/* Include common configuration file and board configuration file */
#include "app_preinclude_common.h"
#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
