/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*
 * Copyright 2020 - 2025 NXP
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
 *  - Number of button on the board,
 *  - Number of LEDs,
 *  - etc...
 */
/*! *********************************************************************************
 *     Board Configuration
 ********************************************************************************** */
/* Number of Button required by the application */
#define gAppButtonCnt_c                 2U

/* Number of LED required by the application */
#define gAppLedCnt_c                    2U

/*! Enable Debug Console (PRINTF) */
#define gDebugConsoleEnable_d           0

/*! *********************************************************************************
 *     App Configuration
 ********************************************************************************** */
/*! Maximum number of connections supported for this application */
#define gAppMaxConnections_c            8U

/*! Enable/disable use of bonding capability */
#define gAppUseBonding_d                0

/*! Enable/disable use of pairing procedure */
#define gAppUsePairing_d                0

/*! Enable/disable use of privacy */
#define gAppUsePrivacy_d                0

#define gPasskeyValue_c                 999999

/* Enable/Disable FSCI */
#define gFsciIncluded_c                 1

/* Use Misra Compliant version of FSCI module */
#define gFsciUseDedicatedTask_c         1

/* Defines FSCI length - set this to FALSE is FSCI length has 1 byte */
#define gFsciLenHas2Bytes_c             1

/* Defines FSCI maximum payload length */
#define gFsciMaxPayloadLen_c            1660

/* Enable FSCI Rx restart with timeout */
#define gFsciRxTimeout_c                1
#define mFsciRxTimeoutUsePolling_c      1

#define gFsciBleBBox_d                  1

#define gFsciBleEnabledLayersMask_d     0x0164

#define gFsciOverRpmsgBridge_c          1

/* Enable Serial Manager interface */
#if !defined(gFsciOverRpmsgBridge_c) || (gFsciOverRpmsgBridge_c == 0)
#define gAppUseSerialManager_c          2
#else
#define gAppUseSerialManager_c          1
#endif /* !defined(gFsciOverRpmsgBridge_c) || (gFsciOverRpmsgBridge_c == 0) */

/*! Enable/Disable PowerDown functionality in Application - In case of troubles with lowpower,
      turn it off for debug purpose to ensure your application is fine without lowpower  */
#define gAppLowpowerEnabled_d           0

#if (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))

/*! Enable/Disable to enable LPUART0 as wake up source by default
 *  This will prevent WAKE domain to go in retention and keep FRO6M running
 *  so the power consumption will increase during low power period.
 *  This assumes LPUART0 is used with Serial Manager (applicative serial interface).
 *  No effect if low power is disabled. */
#define gAppLpuart0WakeUpSourceEnable_d 1

/* JP16, JP17, JP23 and JP24 must be mounted on the UART0 position(2-3) */
#define DEFAULT_APP_UART                0

/* Disable LEDs when enabling low power */
#undef gAppLedCnt_c
#define gAppLedCnt_c                    0

#else /* (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)) */

#define gAppDisableControllerLowPower_d 1

#endif /* (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)) */
/*! *********************************************************************************
 *     Framework Configuration
 ********************************************************************************** */
/* enable NVM to be used as non volatile storage management by the host stack */
#define gAppUseNvm_d                    0

/* erase NVM pages at download( armgcc only) */
#if ((defined gAppUseNvm_d) && (gAppUseNvm_d != 0)) && defined(__GNUC__)
#define gNvmErasePartitionWhenFlashing_c 1U
#endif

/*! Repeated Attempts - Mitigation for pairing attacks */
#define gRepeatedAttempts_d             0

/*! The minimum heap size needed (measured with MEM_STATISTICS) */
#define MinimalHeapSize_c               13000

#define gPlatResetMethod_c gUseResetByNvicReset_c

/*! *********************************************************************************
 *     RTOS Configuration
 ********************************************************************************** */
#if defined(SDK_OS_FREE_RTOS)
/* The following stack sizes have been chosen based on a worst case scenario. 
 * For different compilers and optimization levels they can be reduced. */

#define gHost_TaskStackSize_c           2000

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
/* Max Services - update according to the usecase */
#define gMaxServicesCount_d             6U

/* Max Characteristics - update according to the usecase */
#define gMaxServiceCharCount_d          6U

/* Max Descriptors - update according to the usecase */
#define gMaxCharDescriptorsCount_d      4U

/* Enable/Disable Dynamic GattDb functionality */
#define gGattDbDynamic_d                        1

/*! *********************************************************************************
 *     BLE LL Configuration
 ***********************************************************************************/
/*  ble_ll_config.h file lists the parameters with their default values. User can override
 *    the parameter here by defining the parameter to a user defined value. */

#define gAppExtAdvEnable_d                   1
#define gLlScanPeriodicAdvertiserListSize_c (8U)
/* disable autonomous feature exchange */
#define gL1AutonomousFeatureExchange_d 0

/*
 * Specific configuration of LL pools by block size and number of blocks for this application.
 * Optimized using the MEM_OPTIMIZE_BUFFER_POOL feature in MemManager,
 * we find that the most optimized combination for LL buffers.
 *
 * If LlPoolsDetails_c is not defined, default LL buffer configuration in app_preinclude_common.h
 * will be applied.
 */
/* Define the max tx power setting in dBm. Allowed values 0, 7 or 10 */
#define gAppMaxTxPowerDbm_c     10

/* Include common configuration file and board configuration file */
#include "app_preinclude_common.h"
#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
