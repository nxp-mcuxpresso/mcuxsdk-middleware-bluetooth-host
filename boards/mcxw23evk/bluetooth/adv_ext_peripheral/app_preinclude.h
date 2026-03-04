/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*
 * Copyright 2021 - 2026 NXP
 *
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _APP_PREINCLUDE_H_
#define _APP_PREINCLUDE_H_

/*!
 *  Application specific configuration file only
 *  Board Specific Configuration shall be added to board.h file directly such as :
 *  - Number of buttons on the board,
 *  - Number of LEDs,
 *  - etc...
 */
/*! *********************************************************************************
 *     Board Configuration
 ********************************************************************************** */
/* Number of Buttons required by the application */
#define gAppButtonCnt_c                         2

/* Number of LEDs required by the application */
#define gAppLedCnt_c                            1

/*! Enable Debug Console (PRINTF) */
#define gDebugConsoleEnable_d                   0

/*! *********************************************************************************
 *     App Configuration
 ********************************************************************************** */
/* Enable 5.0 optional features */
#define gBLE50_d                                1
     
/*! Enable/disable printing debug information*/
#define mAE_PeripheralDebug_c                   0
     
/*define the max number of connections this device is able to handle*/
#define gAppMaxConnections_c                    (2U)
     
/*! Enable/disable use of bonding capability */
#define gAppUseBonding_d                        1

/*! Enable/disable use of pairing procedure */
#define gAppUsePairing_d                        1

/*! Enable/disable use of privacy */
#define gAppUsePrivacy_d                        0

/* Use a Random Static address as identity address, instead of a Public address */
#define gRandomStaticAddress_d                  1

#define gPasskeyValue_c                         999999

/*! Set the Tx power in dBm */
#define mAdvertisingDefaultTxPower_c            0

/*! specifies whether the phy update procedure is going to be initiated in connection or not */
#define gConnInitiatePhyUpdateRequest_c         (0U)

/*! Repeated Attempts - Mitigation for pairing attacks */
#define gRepeatedAttempts_d                     0

/*! *********************************************************************************
 *     Framework Configuration
 ********************************************************************************** */
/* enable NVM to be used as non volatile storage management by the host stack */
#define gAppUseNvm_d                     1

/* erase NVM pages at download( armgcc only) */
#if ((defined gAppUseNvm_d) && (gAppUseNvm_d != 0)) && defined(__GNUC__)
#define gNvmErasePartitionWhenFlashing_c 1U
#endif

/*! The minimum heap size needed  */
#define MinimalHeapSize_c                       10500

/*! Enable/Disable PowerDown functionality in Application - In case of troubles with lowpower,
      turn it off for debug purpose to ensure your application is fine without lowpower  */
#define gAppLowpowerEnabled_d           0

/* Disable LEDs when enabling low power */
#if (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))
  #undef gAppLedCnt_c
  #define gAppLedCnt_c                0
#endif

/*! *********************************************************************************
 *     RTOS Configuration
 ********************************************************************************** */
#if defined(SDK_OS_FREE_RTOS)

#define BUTTON_TASK_STACK_SIZE          352

#define SERIAL_MANAGER_TASK_STACK_SIZE  288

#define gMainThreadStackSize_c          1120

#define TM_TASK_STACK_SIZE              392

/* The size used for the Idle task, in dwords. */
#define configMINIMAL_STACK_SIZE        158

#endif

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */
#define gMaxServicesCount_d                     6
#define gMaxServiceCharCount_d                  6

/* Enable Serial Manager interface */
#define gAppUseSerialManager_c                  1

/* See documentation on how to enable DBAF */
#define gBLE60_DecisionBasedAdvertisingFilteringSupport_d 0
#define gAppPAWRSupport_d                                 0
#define gAppEADSupport_d                                  0

/*! *********************************************************************************
 *   Xcvr Configuration
 ********************************************************************************** */

/* Define the max tx power setting in dBm. Allowed values 0, 2 or 6 */
#if !defined(gAppMaxTxPowerDbm_c)
#define gAppMaxTxPowerDbm_c 0
#endif /* !defined(gAppMaxTxPowerDbm_c) */

#if gAppMaxTxPowerDbm_c > 6
#error "gAppMaxTxPowerDbm_c 6dBm is the maximum supported"
#endif

/*! *********************************************************************************
 *  Auto Configuration
 ********************************************************************************** */

/*! *********************************************************************************
 *     BLE LL Configuration
 ***********************************************************************************/
/* Include common configuration file and board configuration file */
#include "app_preinclude_common.h"
#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
