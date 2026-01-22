/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* \file app_preinclude.h
*
* Copyright 2025-2026 NXP
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
#define gAppButtonCnt_c                       1

/* Number of LED required by the application */
#define gAppLedCnt_c                          2

#define gBoardLedBlue_d                       1

#define gBoardLedGreen_d                      1

/* Localization board in use */
#define BOARD_LOCALIZATION_REVISION_SUPPORT   1

/*! Enable Debug Console (PRINTF) */
#define gDebugConsoleEnable_d                 0

/*! *********************************************************************************
 *     App Configuration
 ********************************************************************************** */
/*! Enable/disable use of bonding capability */
#define gAppUseBonding_d                      1

/*! Enable/disable use of pairing procedure */
#define gAppUsePairing_d                      1

/*! Enable/disable use of privacy */
#define gAppUsePrivacy_d                      1

/* Use a Random Static address as identity address, instead of a Public address */
#define gRandomStaticAddress_d                1

/*! Repeated Attempts - Mitigation for pairing attacks */
#define gRepeatedAttempts_d                   0

#define gPasskeyValue_c                       999999

/*! Advertising interval */
#define gcAdvertisingInterval_c               42

/*! Display distance measurement related timing information */
#define gAppCsTimeInfo_d                      0

/* CS HCI data logging support
  *  0 = disabled
  *  1 = export local HCI data only
  *  2 = export local HCI data and remote data received via RAS
  */
#define gAppHciDataLogExport_d          0

/* CS data logging support
   0 = disabled
   1 = export CS config data + ranging results
   2 = export CS config + PBR data + ToF data + ranging results
  */
#define gAppLocDataExport_d             0

/* Baudrate for loc data export */
#if gAppLocDataExport_d > 0
#define BOARD_APP_UART_BAUDRATE         1000000
#endif

#define gAppUseShellInApplication_d           1

#define gAppDisableControllerLowPower_d       0

/*! Enable/Disable PowerDown functionality in Application */
#define gAppLowpowerEnabled_d                 0

/* Disable LEDs when enabling low power */
#if (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))
  #undef gAppLedCnt_c
  #define gAppLedCnt_c                        0
  #undef gBoardLedBlue_d
  #define gBoardLedBlue_d                     0
  #undef gBoardLedGreen_d
  #define gBoardLedGreen_d                    0
#endif

#define gcScanWindow_c                        18U /* 96 ms */
#define gcScanInterval_c                      36U /* 550 ms */
#define gcConnectionInterval_c                24U /* 30 ms, interval = gcConnectionInterval_c * 1.25ms */

/* Enable/Disable the use of RAS to transfer localization data */
#define gAppRasDataTransfer_d                 1

/* Enable/Disable running the selected localization algorithm */
#define gAppRunAlgo_d                         1

/*! *********************************************************************************
 *     Framework Configuration
 ********************************************************************************** */
/* enable NVM to be used as non volatile storage management by the host stack */
#define gAppUseNvm_d                          1

/* erase NVM pages at download( armgcc only) */
#if ((defined gAppUseNvm_d) && (gAppUseNvm_d != 0)) && defined(__GNUC__)
#define gNvmErasePartitionWhenFlashing_c      1U
#endif

#define SHELL_BUFFER_SIZE                     (128U)
#define SHELL_TASK_STACK_SIZE                 (1400U)

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */
#define gAdvSetHandle_c                       0x00U
#define gNoAdvSetHandle_c                     0xFFU

#define gAppMaxConnections_c                  1

#define gcGapMaximumActiveConnections_c       gAppMaxConnections_c

 /* Enable Serial Manager interface */
#if gAppHciDataLogExport_d
#define gAppUseSerialManager_c                2
#else
#define gAppUseSerialManager_c                1
#endif

/* Enable BLE 5.0 */
#define gBLE50_d                              1

/* Enable 5.1 features */
#define gBLE51_d                              1

/* Enable 5.2 features */
#define gBLE52_d                              1

/* Disable GATT caching */
#define gGattCaching_d                        0

/* Disable GATT automatic robust caching */
#define gGattAutomaticRobustCachingSupport_d  0

/* Enable Channel Sounding */
#define gBLE_ChannelSounding_d                1

#define gAppIsPeripheral_d                    0U

#if gAppIsPeripheral_d
#define gCentralInitiatedPairing_d
#endif

/* Channel Sounding role - default initiator */
#define gCsDefaultRole_c                      gCsRoleInitiator_c

/* Enable the RAS role of Ranging Requestor */
#define gRasRREQ_d                            1

/* Enalbe Real-Time Data Transfer */
#define gAppRealTimeDataTransfer_c            0

/* Enable/Disable the saving of RSSI information for mode 1 data.
   Information available in algorithm result structure */
#define gAppParseRssiInfo_d                     0

/* Configure high speed CPU clock (96 MHz) */
#define gAppHighSystemClockFrequency_d        1

#define gHost_TaskStackSize_c                 1800

#define gMainThreadStackSize_c                3360
/*! *********************************************************************************
 *     BLE LL Configuration
 ***********************************************************************************/

/* Configure high speed NBU clock (64 MHz) */
#define gAppHighNBUClockFrequency_d             1

/* Include common configuration file and board configuration file */
#include "app_preinclude_common.h"
#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
