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
#define gAppButtonCnt_c                 1

/* Number of LED required by the application */
#define gAppLedCnt_c                    2

/*! Enable Debug Console (PRINTF) */
#define gDebugConsoleEnable_d           0

#define configUSE_IDLE_HOOK             1

#define gFsciOverRpmsgBridge_c          1

#define gFsciOverRpmsg_c                1

/*! *********************************************************************************
 *     App Configuration
 ********************************************************************************** */
#define gAppDisableControllerLowPower_d 0

/*! Enable/Disable PowerDown functionality in Application */
#define gAppLowpowerEnabled_d           0

/*! Set maximum arguments in command */
#define SHELL_MAX_ARGS                  20U

/* Disable LEDs when enabling low power */
#if (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))
  #undef gAppLedCnt_c
  #define gAppLedCnt_c                  0
#endif

/*! Set local RAS role as requester */
#define gRasRREQ_d                      1

/* Enable/Disable FSCI */
#define gFsciIncluded_c                 1

#define gAppMaxConnections_c            2

/* Maximum number of concurrent Channel Sounding procedures across all connections */
#define gChannelSoundingMaxConcurrentProcedures_c  2U

/* Validate Channel Sounding configuration */
#if (gChannelSoundingMaxConcurrentProcedures_c > gAppMaxConnections_c)
#error "gChannelSoundingMaxConcurrentProcedures_c cannot exceed gAppMaxConnections_c "
#endif

/*! *********************************************************************************
 *     Framework Configuration
 ********************************************************************************** */
/* enable NVM to be used as non volatile storage management by the host stack */
#define gAppUseNvm_d                    1

#define SHELL_BUFFER_SIZE               (128U)
#define SHELL_TASK_STACK_SIZE           (1400U)

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */
#define gAppUseSerialManager_c                  1

/* Enable BLE 5.0 */
#define gBLE50_d                                1

/* Enable 5.1 features */
#define gBLE51_d                                1

/* Enable 5.2 features */
#define gBLE52_d                                1

/* Disable GATT caching */
#define gGattCaching_d                          0

/* Disable GATT automatic robust caching */
#define gGattAutomaticRobustCachingSupport_d    0

/* Enable Channel Sounding */
#define gBLE_ChannelSounding_d                  1

/* Configure high speed CPU clock (96 MHz) */
#define gAppHighSystemClockFrequency_d          1

#define gHost_TaskStackSize_c                   1800

#define gMainThreadStackSize_c                  3800

#define gFsciTaskStackSize_c                    3800

#define gAppRasDataTransfer_d                   1

#define gAppRunAlgo_d                           1

/* Enable/Disable the saving of RSSI information for mode 1 data.
   Information available in algorithm result structure */
#define gAppParseRssiInfo_d                     0

/* Enable/Disable adaptive CS procedure interval based on the RSSI average.
   EXPERIMENTAL: this feature is provided for evaluation only and is disabled
   by default.
   When the procedure auto-restart loop is active, the RSSI average of each
   iteration is used to adjust the CS procedure interval for the next iteration:
     - good RSSI -> decrease interval (faster distance report rate)
     - bad RSSI  -> increase interval (more time for the RAS transfer to finish)
   The auto-restart loop runs on the NCP core (ncp_loc_reader) while the RSSI
   samples are produced on the host core (loc_reader_host), so the host forwards
   the per-procedure RSSI average to the NCP core over FSCI.
   Requires gAppParseRssiInfo_d to be enabled. */
#define gAppAdaptiveProcInterval_d              0

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
