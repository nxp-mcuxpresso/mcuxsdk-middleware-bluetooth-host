/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*
* Copyright 2025-2026 NXP
*
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _APP_PREINCLUDE_H_
#define _APP_PREINCLUDE_H_

/* Customize application */
#define INCLUDE_EMBEDDED_COMPUTATION_SUPPORT

/*! *********************************************************************************
 * 	Drivers Configuration
 ********************************************************************************** */
 /* Defines the number of available tsi electrodes for this application */

/* Number of Button required by the application */
#define gAppButtonCnt_c                                 1

/* Number of LED required by the application */
#define gAppLedCnt_c                                    2

#define gBoardLedBlue_d                                 1

#define gBoardLedGreen_d                                1

#define BOARD_LOCALIZATION_REVISION_SUPPORT             1

#ifdef SDK_OS_FREE_RTOS
/* No delay for serial non-blocking writes when using FreeRTOS */
#define SERIAL_MANAGER_WRITE_TIME_DELAY_DEFAULT_VALUE   0
/* CLI should execute on the Serial Manager Task */
#define SERIAL_MANAGER_TASK_HANDLE_RX_AVAILABLE_NOTIFY  1
#endif

/*! *********************************************************************************
 *     App Configuration
 ********************************************************************************** */
/*! Maximum number of connections supported for this application */
#define gAppMaxConnections_c                            8

/*! Enable/disable use of bonding capability */
#define gAppUseBonding_d                                0

/*! Enable/disable use of pairing procedure */
#define gAppUsePairing_d                                0

/*! Enable/disable use of privacy */
#define gAppUsePrivacy_d                                0

/*! Should be set to 0 to disable service discovery (IOP) */
/*! As a consequence, proprietary OTA data exchange will not be performed */
#define gAppUseServiceDiscovery_d                       1

#define gPasskeyValue_c                                 999999

/*! Enable/disable the use of debug data
    Disable when running SRDE with antenna diversity to ensure enough heap size*/
#define gAppEnableDebugData                             1

#define gAppDisableControllerLowPower_d                 0

/*! *********************************************************************************
 * 	Framework Configuration
 ********************************************************************************** */
/* enable NVM to be used as non volatile storage management by the host stack */
#define gAppUseNvm_d                                    1

/* erase NVM pages at download( armgcc only) */
#if ((defined gAppUseNvm_d) && (gAppUseNvm_d != 0)) && defined(__GNUC__)
#define gNvmErasePartitionWhenFlashing_c                1U
#endif

/* serial Manager */
#define gAppUseSerialManager_c                          1
#define gSerialMgrRxBufSize_c                           160

/* Increase main thread stack size for SRDE algorithm run in antenna diversity setup. */
#ifdef SDK_OS_FREE_RTOS
#define gMainThreadStackSize_c                          4500
#endif

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */
#define gMaxServicesCount_d                             6
#define gMaxServiceCharCount_d                          6

/* Enable 5.0 optional features */
#define gBLE50_d                                        1

/* Enable 5.1 optional features */
#define gBLE51_d                                        1

/* Enable 5.2 optional features */
#define gBLE52_d                                        1

/* Enable the use of the Channel Sounding library */
#define gBLE_ChannelSounding_d                          1

/* Configure high speed CPU clock (96 MHz) */
#define gAppHighSystemClockFrequency_d                  1

/*If enable the gUseHciTransportDownward_d macro, Two serial interfaces are required.
 *One serial port(LPUART0) prints the log and the other serial port(LPUART1) sends HCI packets */
#if (defined(gUseHciTransportDownward_d) &&(gUseHciTransportDownward_d))
#define gSerialManagerMaxInterfaces_c                   2
#else
#define gSerialManagerMaxInterfaces_c                   1
#endif

#define gHost_TaskStackSize_c                           1800

/*! *********************************************************************************
 *     BLE LL Configuration
 ***********************************************************************************/

/* Configure high speed NBU clock (64 MHz) */
#define gAppHighNBUClockFrequency_d                      1

/* Include common configuration file and board configuration file */
#include "app_preinclude_common.h"

#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
