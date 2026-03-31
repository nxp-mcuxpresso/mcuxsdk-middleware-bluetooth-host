/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* \file app_preinclude.h
*
* Copyright 2022-2026 NXP
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

/*! Repeated Attempts - Mitigation for pairing attacks */
#define gRepeatedAttempts_d             0

#define gPasskeyValue_c                 999999

/* Use a Random Static address as identity address, instead of a Public address */
#define gRandomStaticAddress_d          1

/* Do not send any Peripheral Security Requests */
#define gCentralInitiatedPairing_d

/* Use an application defined IRK (APP_SMP_IRK) instead of generating one */
#define gUseCustomIRK_d

#define APP_BD_ADDR     {0xC5, 0xBC, 0x70, 0x37, 0x60, 0xC4}
#define APP_SMP_IRK     {0x0A, 0x2D, 0xF4, 0x65, 0xE3, 0xBD, 0x7B, 0x49, 0x1E, 0xB4, 0xC0, 0x95, 0x95, 0x13, 0x46, 0x73}

/*! BLE CCC Digital Key UUID */
#define gBleSig_CCC_DK_UUID_d                    0xFFF5U

#define gAppUseShellInApplication_d     1

/*! Display distance measurement related timing information */
#define gAppCsTimeInfo_d                0

#define gAppLowpowerEnabled_d           0

#define gAppDisableControllerLowPower_d 0

/* Disable LEDs when enabling low power */
#if (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))
  #undef gAppLedCnt_c
  #define gAppLedCnt_c                    0
  #define gAppLpuart0WakeUpSourceEnable_d 1
#endif

#define gBleLeScOobHasMitmProtection_c    1

/* Enable/Disable the BTCS Server for L2CAP transfer functionality */
#define gAppBtcsServer_d                  0

/* Enable/Disable the BTCS Client for L2CAP transfer functionality */
#define gAppBtcsClient_d                  1

/* Enable/Disable running the selected localization algorithm */
#define gAppRunAlgo_d                   1

#if defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1) && \
    ((!defined(gAppBtcsClient_d)) || (defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 0)))
#error "Must be BTCS Client to run the algorithm"
#endif

/* Enable/Disable the saving of RSSI information for mode 1 data.
   Information available in algorithm result structure */
#define gAppParseRssiInfo_d               0

#define gAppMaxConnections_c              2

/* Maximum number of concurrent Channel Sounding procedures across all connections */
#define gChannelSoundingMaxConcurrentProcedures_c  2U

/* Validate Channel Sounding configuration */
#if (gChannelSoundingMaxConcurrentProcedures_c > gAppMaxConnections_c)
#error "gChannelSoundingMaxConcurrentProcedures_c cannot exceed gAppMaxConnections_c "
#endif

/*! *********************************************************************************
 *     CCC Configuration
 ********************************************************************************** */
/* Scan for non-CCC key fobs */
#define gAppScanNonCCC_d                        0

/* Enable the optional LE Coded PHY advertising */
#define gAppLeCodedAdvEnable_d                  1

#define gcAdvertisingIntervalCCC_1M_c           (68U)   /* 42.5 ms */
#define gcAdvertisingIntervalCCC_CodedPhy_c     (135U)  /* 84.3 ms */

#define gDK_DefaultVehiclePsm_c                 (0x0081U) /* Range 0x0080-0x00FF */
#define gDKMessageMaxLength_c                   (255U)
#define mAppLeCbInitialCredits_c                (32768U)
#define gCCCL2caTimeout_c                       (5U)    /* Seconds */
#define gDummyPayload_c       {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC}

#define gDummyPayloadLength_c 16

#define gHandoverIncluded_d     1
#define gAppSecureMode_d        0
#define gA2BEnabled_d           0
#define gA2BInitiator_d         0

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

#if (gHandoverIncluded_d == 1) || (gA2BEnabled_d == 1)
#define gA2ASerialInterface_d   1
#endif

#if ((gHandoverIncluded_d == 1) || (gA2BEnabled_d == 1)) && (gAppHciDataLogExport_d == 1)
#error "Distance measurement data export not available if connection handover or A2B is enabled"
#endif

#if (gA2BEnabled_d == 1) && (gAppSecureMode_d == 0)
#error "The A2B feature is available only in Secure Mode"
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

#define SHELL_BUFFER_SIZE               (128U)
#define SHELL_TASK_STACK_SIZE           (1200U)

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */
#define gLegacyAdvSetHandle_c                   0x00U
#define gExtendedAdvSetHandle_c                 0x01U
#define gNoAdvSetHandle_c                       0xFFU

/* Must open an L2CAP channel for each CCC peer */
#define gL2caMaxLeCbChannels_c                  gAppMaxConnections_c

 /* Enable Serial Manager interface */
#if gA2ASerialInterface_d || gAppHciDataLogExport_d
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

/* Channel Sounding role - default initiator */
#define gCsDefaultRole_c                        gCsRoleInitiator_c

/* Configure high speed CPU clock (96 MHz) */
#define gAppHighSystemClockFrequency_d          1

#define gHost_TaskStackSize_c                   1800

#define gMainThreadStackSize_c                  3000
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
