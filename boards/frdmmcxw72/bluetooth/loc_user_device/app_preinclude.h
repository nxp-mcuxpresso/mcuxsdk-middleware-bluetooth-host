/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*!
 * Copyright 2023-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
#define gAppButtonCnt_c                 2

/* Number of LED required by the application */
#define gAppLedCnt_c                    2

/*! Enable Debug Console (PRINTF) */
#define gDebugConsoleEnable_d           0

/*! *********************************************************************************
 *     App Configuration
 ********************************************************************************** */
/*! Enable/disable use of bonding capability */
#define gAppUseBonding_d                1

/*! Enable/disable use of pairing procedure */
#define gAppUsePairing_d                1

/*! Enable/disable use of privacy */
#define gAppUsePrivacy_d                1

/*! Set maximum arguments in command */
#define SHELL_MAX_ARGS                  20U

/* Use a Random Static address as identity address, instead of a Public address */
#define gRandomStaticAddress_d          1

/*! Repeated Attempts - Mitigation for pairing attacks */
#define gRepeatedAttempts_d             0

#define gPasskeyValue_c                 999999

#define gMaxServicesCount_d             6

#define gAppMaxConnections_c            1U

/* Must open an L2CAP channel for each CCC peer */
#define gL2caMaxLeCbChannels_c          gAppMaxConnections_c

#define gAppDisableControllerLowPower_d 0

#define gcScanWindow_c                  18U /* 96 ms */
#define gcScanInterval_c                36U /* 550 ms */
#define gcConnectionInterval_c          24U /* 30 ms, interval = gcConnectionInterval_c * 1.25ms */

/*! Display distance measurement related timing information */
#define gAppCsTimeInfo_d                0

/* CS HCI data logging support
  *  0 = disabled
  *  1 = export local HCI data only
  *  2 = export local HCI data and remote data received via RAS
  */
#define gAppHciDataLogExport_d          0

#define gAppUseShellInApplication_d     1

/*! Enable/Disable PowerDown functionality in Application */
#define gAppLowpowerEnabled_d           0

/* Disable LEDs when enabling low power */
#if (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))
  #undef gAppLedCnt_c
  #define gAppLedCnt_c                  0
#endif

/* Enable/Disable the use of RAS to transfer localization data */
#define gAppRasDataTransfer_d           1

/*! Transient application key demo */
#define gAppUseTAK_d                    0
#define gAppTAKAdvID_c                  "TAK_ID"

/*! *********************************************************************************
 *     Framework Configuration
 ********************************************************************************** */
/* enable NVM to be used as non volatile storage management by the host stack */
#define gAppUseNvm_d                     1

/* erase NVM pages at download( armgcc only) */
#if ((defined gAppUseNvm_d) && (gAppUseNvm_d != 0)) && defined(__GNUC__)
#define gNvmErasePartitionWhenFlashing_c 1U
#endif

/*! The minimum heap size needed (measured with MEM_STATISTICS) */
#define MinimalHeapSize_c               20000

#define SHELL_BUFFER_SIZE               (128U)
#define SHELL_TASK_STACK_SIZE           (1400U)

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */
 /* Enable Serial Manager interface */
#if gAppHciDataLogExport_d
#define gAppUseSerialManager_c                  2
#else
#define gAppUseSerialManager_c                  1
#endif

/* Enable BLE 5.0 */
#define gBLE50_d                                1

/* Enable 5.1 features */
#define gBLE51_d                                1

/* Enable 5.2 features */
#define gBLE52_d                                1

/* Enable EATT */
#define gEATT_d                                 1

/* Disable GATT caching */
#define gGattCaching_d                          0

/* Disable GATT automatic robust caching */
#define gGattAutomaticRobustCachingSupport_d    0

/* Enable Channel Sounding */
#define gBLE_ChannelSounding_d                  1

#define gAppIsPeripheral_d                      1U

#if gAppIsPeripheral_d
#define gCentralInitiatedPairing_d
#endif

/* Channel Sounding role - default reflector */
#define gCsDefaultRole_c                        gCsRoleReflector_c

/* Enable the RAS role of Ranging Responder */
#define gRasRRSP_d                              1

/* Configure high speed CPU clock (96 MHz) */
#define gAppHighSystemClockFrequency_d          1

#define gHost_TaskStackSize_c                   1800

#define gAdvSetHandle_c                         0x00U
#define gNoAdvSetHandle_c                       0xFF

/*! Set the Tx power in dBm */
#define mAdvertisingDefaultTxPower_c    0

/*! Advertising interval */
#define gcAdvertisingInterval_c         42

/*! *********************************************************************************
 *  Auto Configuration
 ********************************************************************************** */

/*! *********************************************************************************
 *     BLE LL Configuration
 ***********************************************************************************/

/* Configure high speed NBU clock (64 MHz) */
/* Set this define if limitations in channel sounding with multiple connections are observed */
#define gAppHighNBUClockFrequency_d             0

/* Include common configuration file and board configuration file */
#include "app_preinclude_common.h"
#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
