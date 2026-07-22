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
#define gAppButtonCnt_c                 1U

/* Number of LED required by the application */
#define gAppLedCnt_c                    0U

/*! Enable Debug Console (PRINTF) */
#define gDebugConsoleEnable_d           0

/*! *********************************************************************************
 * 	App Configuration
 ********************************************************************************** */
/* Enable Extended Advertising */
#define gBeaconAE_c                     1

#if gBeaconAE_c
/* Use very large extended advertising data */
#define gBeaconLargeExtAdvData_c        0

#endif /* gBeaconAE_c */

/*! Repeated Attempts - Mitigation for pairing attacks */
#define gRepeatedAttempts_d             0

/*! *********************************************************************************
 *     Framework Configuration
 ********************************************************************************** */
/* Do not modify. Not used for this application */
#define gAppUseNvm_d                     1

/* erase NVM pages at download( armgcc only) */
#if ((defined gAppUseNvm_d) && (gAppUseNvm_d != 0)) && defined(__GNUC__)
#define gNvmErasePartitionWhenFlashing_c 1U
#endif

/*! The minimum heap size needed (measured with MEM_STATISTICS) */
#define MinimalHeapSize_c               8500

/*! Enable/Disable PowerDown functionality in Application - In case of troubles with lowpower,
      turn it off for debug purpose to ensure your application is fine without lowpower  */
#define gAppLowpowerEnabled_d           0

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
/* The following stack sizes have been chosen based on a worst case scenario. 
 * For different compilers and optimization levels they can be reduced. */

#define gHost_TaskStackSize_c           1584

#define BUTTON_TASK_STACK_SIZE          356

#define SERIAL_MANAGER_TASK_STACK_SIZE  232

#define gMainThreadStackSize_c          1246

#define TM_TASK_STACK_SIZE              352

/* The size used for the Idle task, in dwords. */
#define configMINIMAL_STACK_SIZE        148

#endif
/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */

/* Enable 5.3 features, _OPT bluetooth library must be linked */
#define gBLE53_d                        1

/*! *********************************************************************************
 *     BLE LL Configuration
 ***********************************************************************************/
/*  ble_ll_config.h file lists the parameters with their default values. User can override
 *    the parameter here by defining the parameter to a user defined value. */
#define gLlUsePeriodicAdvertising_d     0

/* Include common configuration file and board configuration file */
#include "app_preinclude_common.h"
#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
