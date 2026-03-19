/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*!
 * Copyright 2019-2026 NXP
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
#define gAppLedCnt_c                    2

/*! Enable Debug Console (PRINTF) */
#define gDebugConsoleEnable_d           0

/* Enable Serial Manager interface */
#define gAppUseSerialManager_c          1

/*! *********************************************************************************
 *     App Configuration
 ********************************************************************************** */
/*! Enable/disable use of bonding capability */
#define gAppUseBonding_d   1

/*! Enable/disable use of pairing procedure */
#define gAppUsePairing_d   1

/*! Enable/disable use of privacy */
#define gAppUsePrivacy_d   0

/* Use a Random Static address as identity address, instead of a Public address */
#define gRandomStaticAddress_d 0

#define gPasskeyValue_c                999999

#define gMaxServicesCount_d            6

/*! specifies whether the phy update procedure is going to be initiated in connection or not */
#define gConnInitiatePhyUpdateRequest_c            (1U)

/*! Repeated Attempts - Mitigation for pairing attacks */
#define gRepeatedAttempts_d             0
/*! *********************************************************************************
 *     Framework Configuration
 ********************************************************************************** */
/* enable NVM to be used as non volatile storage management by the host stack */
#define gAppUseNvm_d                     1

/* erase NVM pages at download( armgcc only) */
#if ((defined gAppUseNvm_d) && (gAppUseNvm_d != 0)) && defined(__GNUC__)
#define gNvmErasePartitionWhenFlashing_c 1U
#endif

/*! The minimum heap size needed */
#define MinimalHeapSize_c               12000

/* Disable LEDs when enabling low power */
#if (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))
  #undef gAppLedCnt_c
  #define gAppLedCnt_c                0
#endif

/*! Enable the SWD pins to be managed into low-power */
#define gBoard_ManageSwdPinsInLowPower_d    0

/*! *********************************************************************************
 *     RTOS Configuration
 ********************************************************************************** */
#if defined(SDK_OS_FREE_RTOS)

#define BUTTON_TASK_STACK_SIZE          380

#define SERIAL_MANAGER_TASK_STACK_SIZE  320

#define TM_TASK_STACK_SIZE              360

/* The size used for the Idle task, in dwords. */
#define configMINIMAL_STACK_SIZE        156

#endif /* defined(SDK_OS_FREE_RTOS) */

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
 *     BLE LL Configuration
 ***********************************************************************************/
/* Include common configuration file and board configuration file */
#include "app_preinclude_common.h"
#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
