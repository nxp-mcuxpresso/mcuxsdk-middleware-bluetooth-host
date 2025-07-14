/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*!
 * Copyright 2019-2025 NXP
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
#define gRandomStaticAddress_d 1

#define gPasskeyValue_c                999999

/*! Set the Tx power in dBm */
#define mAdvertisingDefaultTxPower_c    0

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

/*! The minimum heap size needed (measured with MEM_STATISTICS) */
#define MinimalHeapSize_c               11500

/*! Enable/Disable PowerDown functionality in Application - In case of troubles with lowpower,
      turn it off for debug purpose to ensure your application is fine without lowpower  */
#define gAppLowpowerEnabled_d           1
        
/* Disable LEDs when enabling low power */
#if (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))
  #undef gAppLedCnt_c
  #define gAppLedCnt_c                0
#endif        

/*! Enable XCVR calibration storage in Flash */
#define gControllerPreserveXcvrDacTrimValue_d     1

/*! Enable the SWD pins to be managed into low-power */
#define gBoard_ManageSwdPinsInLowPower_d    0

/*! *********************************************************************************
 *     RTOS Configuration
 ********************************************************************************** */
#if defined(SDK_OS_FREE_RTOS)
/* The following stack sizes have been chosen based on a worst case scenario. 
 * For different compilers and optimization levels they can be reduced. */

#define gHost_TaskStackSize_c           1764

#define BUTTON_TASK_STACK_SIZE          600

#define SERIAL_MANAGER_TASK_STACK_SIZE  232

#define gMainThreadStackSize_c          864

#define TM_TASK_STACK_SIZE              352

/* The size used for the Idle task, in dwords. */
#define configMINIMAL_STACK_SIZE        156

/* Use Fro32k instead of Osc32k */
#define gBoardUseFro32k_d                 1
#endif

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */

/* Enable Serial Manager interface */
#define gAppUseSerialManager_c            1

/* Time between the beginning of two consecutive advertising PDU's */
#define mcAdvertisingPacketInterval_c     0x02    /* 1.25 msec */

/* Offset to the first instant register. */
#define mcOffsetToFirstInstant_c          0x00      /* 625usec */

/*! *********************************************************************************
 *     BLE LL Configuration
 ***********************************************************************************/
/* Include common configuration file and board configuration file */
#include "app_preinclude_common.h"
#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
