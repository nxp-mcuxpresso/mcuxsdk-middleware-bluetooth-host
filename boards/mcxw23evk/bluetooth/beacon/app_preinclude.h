/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
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
#define gAppLedCnt_c                    1U

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

/*! *********************************************************************************
 *     RTOS Configuration
 ********************************************************************************** */
#if defined(SDK_OS_FREE_RTOS)
/* The following stack sizes have been chosen based on a worst case scenario.
 * For different compilers and optimization levels they can be reduced. */

#define BUTTON_TASK_STACK_SIZE          700

#define SERIAL_MANAGER_TASK_STACK_SIZE  360

#define gMainThreadStackSize_c          1876

#define TM_TASK_STACK_SIZE              440

/* The size used for the Idle task, in dwords. */
#define configMINIMAL_STACK_SIZE        140

#endif
/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */

/* Enable 5.3 features, _OPT bluetooth library must be linked */
#define gBLE53_d                        1

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
/*  ble_ll_config.h file lists the parameters with their default values. User can override
 *    the parameter here by defining the parameter to a user defined value. */
#define gAppExtAdvEnable_d              1
#define gLlMaxUsedAdvSet_c              2
#define gLlUsePeriodicAdvertising_d     0

/*
 * Specific configuration of LL pools by block size and number of blocks for this application.
 * Optimized using the MEM_OPTIMIZE_BUFFER_POOL feature in MemManager,
 * we find that the most optimized combination for LL buffers.
 *
 * If LlPoolsDetails_c is not defined, default LL buffer configuration in app_preinclude_common.h
 * will be applied.
 */

/* Include common configuration file and board configuration file */
#include "app_preinclude_common.h"
#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
