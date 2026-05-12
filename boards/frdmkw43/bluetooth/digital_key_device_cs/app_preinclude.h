/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*!
 * Copyright 2020-2026 NXP
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
#define gAppButtonCnt_c                 1

/* Number of LED required by the application */
#define gAppLedCnt_c                    0

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
#define gRandomStaticAddress_d          0

/*! Repeated Attempts - Mitigation for pairing attacks */
#define gRepeatedAttempts_d             0

#define gPasskeyValue_c                 999999

#define gMaxServicesCount_d             6

#define gAppMaxConnections_c            2U

/* Must open an L2CAP channel for each CCC peer */
#define gL2caMaxLeCbChannels_c          gAppMaxConnections_c

/*! BLE CCC Digital Key UUID */
#define gBleSig_CCC_DK_UUID_d           0xFFF5U

#define gAppUseShellInApplication_d     1

#define gAppLowpowerEnabled_d           0

#define gAppDisableControllerLowPower_d     1

/* Disable LEDs when enabling low power */
#if (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))
  #undef gAppLedCnt_c
  #define gAppLedCnt_c                  0
#endif

#define gBleLeScOobHasMitmProtection_c  1

/* Enable/Disable the BTCS Server for L2CAP transfer functionality */
#define gAppBtcsServer_d                1

/* Enable/Disable the BTCS Client for L2CAP transfer functionality */
#define gAppBtcsClient_d                0

/* Enable/Disable running the selected localization algorithm */
#define gAppRunAlgo_d                   0

/* Enable/Disable the saving of RSSI information for mode 1 data.
   Information available in algorithm result structure */
#define gAppParseRssiInfo_d               0

/* Maximum number of concurrent Channel Sounding procedures across all connections */
#define gChannelSoundingMaxConcurrentProcedures_c  2U

/* Validate Channel Sounding configuration */
#if (gChannelSoundingMaxConcurrentProcedures_c > gAppMaxConnections_c)
#error "gChannelSoundingMaxConcurrentProcedures_c cannot exceed gAppMaxConnections_c "
#endif

/* Enable/Disable CS Enhancement: Inline PCT Transfer
   Only the RADE algorithm supports distance measurement with Inline PCT Transfer */
#define gAppUseInlinePctTransfer_d            (0U)

#if gAppUseInlinePctTransfer_d
#define gHostInitEnableExpmFeatures_c         (1U)
#endif
/*! *********************************************************************************
 *     CCC Configuration
 ********************************************************************************** */
#define gcScanWindowCCC_c               154U /* 96 ms */
#define gcScanIntervalCCC_c             880U /* 550 ms */
#define gcConnectionIntervalCCC_c       24U /* interval = gcConnectionIntervalCCC_c * 1.25ms */

#define gDKMessageMaxLength_c           (255U)
#define mAppLeCbInitialCredits_c        (32768U)

#define gDummyPayload_c       {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC}

#define gDummyPayloadLength_c           16

/*! Display distance measurement related timing information */
#define gAppCsTimeInfo_d                0

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
#define MinimalHeapSize_c               22000

#define SHELL_BUFFER_SIZE               (128U)
#define SHELL_TASK_STACK_SIZE           (1200U)

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */
 /* Enable Serial Manager interface */
#define gAppUseSerialManager_c                  1

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

/* Channel Sounding role - default reflector */
#define gCsDefaultRole_c                        gCsRoleReflector_c

/* Configure high speed CPU clock (96 MHz) */
#define gAppHighSystemClockFrequency_d          1

#define gHost_TaskStackSize_c                   2200
#define gMainThreadStackSize_c                  3600

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
