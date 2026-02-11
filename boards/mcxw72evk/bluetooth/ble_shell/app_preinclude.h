/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*
 * Copyright 2020 - 2026 NXP
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
#define gAppButtonCnt_c                 0

/* Number of LED required by the application */
#define gAppLedCnt_c                    2U

/*! Enable Debug Console (PRINTF) */
#define gDebugConsoleEnable_d           0

/*! *********************************************************************************
 *     App Configuration
 ********************************************************************************** */
/*! Maximum number of connections supported for this application */
#define gAppMaxConnections_c           8U

/*! Enable/disable use of bonding capability */
#define gAppUseBonding_d               0

/*! Enable/disable use of pairing procedure */
#define gAppUsePairing_d               0

/*! Enable/disable use of privacy */
#define gAppUsePrivacy_d               0

/* Use a Random Static address as identity address, instead of a Public address */
#define gRandomStaticAddress_d         1

#define gPasskeyValue_c                999999

/*! Repeated Attempts - Mitigation for pairing attacks */
#define gRepeatedAttempts_d             0

/* Enable Advertising Extension shell commands */
#define BLE_SHELL_AE_SUPPORT            1

/*! Set maximum arguments in command */
#define SHELL_MAX_ARGS                  20U

#if BLE_SHELL_AE_SUPPORT

#define gGapSimultaneousEAChainedReports_c     2

/* User defined payload pattern and length of extended advertising data */
#define SHELL_EXT_ADV_DATA_PATTERN      "\n\rEXTENDED_ADVERTISING_DATA_LARGE_PAYLOAD"
#define SHELL_EXT_ADV_DATA_SIZE        (500U)
#endif /* BLE_SHELL_AE_SUPPORT */

/* Enable Decision Based Advertising Filtering shell commands */
#define BLE_SHELL_DBAF_SUPPORT            0

#if BLE_SHELL_DBAF_SUPPORT

#define gBLE60_d        1
#define gBLE60_DecisionBasedAdvertisingFilteringSupport_d       1

#define gMaxNumDecisionInstructions_c     8U

#endif /* BLE_SHELL_DBAF_SUPPORT */

/* Enable Periodic Advertising with Responses shell commands */
#define BLE_SHELL_PAWR_SUPPORT          0

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)

#define gBLE54_PawrSupport_d            TRUE

#define SHELL_PER_ADV_MAX_NUM_SUBEVENTS         3U
#define SHELL_EXT_ADV_DATA_MAX_AD_STRUCTURES    5U

#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */

/* Enable Monitoring Advertisers shell commands */
#define BLE_SHELL_MONADV_SUPPORT                0U
/* Enable Connection Subrating shell commands */
#define BLE_SHELL_CONN_SBR_SUPPORT              0U

/* Experimental features require special initialization */
#if ((defined(BLE_SHELL_MONADV_SUPPORT)) && (BLE_SHELL_MONADV_SUPPORT == 1U)) || \
    ((defined(BLE_SHELL_CONN_SBR_SUPPORT)) && (BLE_SHELL_CONN_SBR_SUPPORT == 1U))
#define gHostInitEnableExpmFeatures_c      TRUE
#endif

#if defined(BLE_SHELL_MONADV_SUPPORT) && (BLE_SHELL_MONADV_SUPPORT)
#define gBLE60_d                                1
#define gBLE60_MonitoredAdvertisers_d           TRUE
#endif /* BLE_SHELL_MONADV_SUPPORT */

#if (((defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)) || \
     ((defined BLE_SHELL_DBAF_SUPPORT) && (BLE_SHELL_DBAF_SUPPORT == 1)) || \
     ((defined BLE_SHELL_MONADV_SUPPORT) && (BLE_SHELL_MONADV_SUPPORT == 1)))
/* Increase shell buffer size & task stack size to allow longer commands */
#define SHELL_BUFFER_SIZE                       128U
#define SHELL_TASK_STACK_SIZE                   1200U
#endif /* (((defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)) || \
           ((defined BLE_SHELL_DBAF_SUPPORT) && (BLE_SHELL_DBAF_SUPPORT == 1)) || \
           ((defined BLE_SHELL_MONADV_SUPPORT) && (BLE_SHELL_MONADV_SUPPORT == 1))) */

/*! Transient application key demo */
#define gAppUseTAK_c                            0

/*! Number of TAK keys the application can store at once */
#define BLE_SHELL_MAX_TAK_ENTRIES               1U

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
#define MinimalHeapSize_c               16000

 /*! *********************************************************************************
 *   Xcvr Configuration
 ********************************************************************************** */

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */
#define gMaxServicesCount_d             6
#define gMaxServiceCharCount_d          6

/* Enable Serial Manager interface */
#define gAppUseSerialManager_c          1

/* Enable 5.0 optional features */
#define gBLE50_d                        1

/* Enable 5.1 optional features */
#define gBLE51_d                        1

/* Enable 5.2 optional features */
#define gBLE52_d                        1

/* Enable EATT */
#define gEATT_d                         1

/* Enable Dynamic GATT database */
#define gGattDbDynamic_d                1

/*! *********************************************************************************
 *     BLE LL Configuration
 ***********************************************************************************/

/* Include common configuration file and board configuration file */
#include "app_preinclude_common.h"
#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
