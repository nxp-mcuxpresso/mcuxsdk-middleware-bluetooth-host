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
#define gAppButtonCnt_c                 0

/* Number of LED required by the application */
#define gAppLedCnt_c                    0

#define gFsciRxTimeout_c                0

/* Disable sensors as not used and ADC used by sensors increased the wakeup time
 and increase the chance having missed received bytes on FSCI message on UART */
#define gAppUseSensors_d                0

 /*! *********************************************************************************
 *   Xcvr Configuration
 ********************************************************************************** */
#define gAppMaxTxPowerDbm_c             10

/*! *********************************************************************************
 *     App Configuration
 ********************************************************************************** */
#define gAppExtAdvEnable_d              1

/*! Number of connections supported by the application */
#define gAppMaxConnections_c            8

#define gFsciBleTest_d                  1

#define gcGapMaximumActiveConnections_c 8

#define gFsciBleEnabledLayersMask_d     0x0964

/* Enable or disable Handover feature */
#define gHandoverSupportEnabled_d       0

/* Enable Channel Sounding feature */
#define gBLE_ChannelSounding_d          0

#define gBLE54_d                                            TRUE
#define gBLE54_AdvertisingCodingSelectionSupport_d          FALSE
#define gBLE60_d                                            TRUE
#define gBLE60_DecisionBasedAdvertisingFilteringSupport_d   FALSE
#define gPlatResetMethod_c gUseResetByNvicReset_c
/*! *********************************************************************************
 *     Framework Configuration
 ********************************************************************************** */
/* enable NVM to be used as non volatile storage management by the host stack */
#define gAppUseNvm_d                     1

/*enable FSCI Monitoring */
#define gNvmEnableFSCIMonitoring_c       0
#define gNvmEnableFSCIRequests_c         0

/* erase NVM pages at download( armgcc only) */
#if ((defined gAppUseNvm_d) && (gAppUseNvm_d != 0)) && defined(__GNUC__)
#define gNvmErasePartitionWhenFlashing_c 1U
#endif

#if ((defined gAppUseNvm_d) && (gAppUseNvm_d != 0))
#define gAppUseNvmOnFsciBridge_d        1
#endif

/* Enable Serial Manager interface */
#define gAppUseSerialManager_c          1
#define SERIAL_MANAGER_NON_BLOCKING_DUAL_MODE   0

/*! The minimum heap size needed:
    6 blocks of 32
    3 blocks of 64
    3 blocks of 128
    2 blocks of 256
    4 blocks of 512
    8 blocks of 1024
*/
#define MinimalHeapSize_c               11520

/* Enable/Disable FSCI */
#define gFsciIncluded_c                 1

/* Defines FSCI length - set this to FALSE is FSCI length has 1 byte */
#define gFsciLenHas2Bytes_c             1

/* Defines FSCI maximum payload length */
#define gFsciMaxPayloadLen_c            1660

/* Defines FSCI maximum number of operation groups */
#define gFsciMaxOpGroups_c              12

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
/*! Lowpower Constraint setting for various BLE states (Advertising, Scanning, connected mode)
    The value shall map with the type defintion PWR_LowpowerMode_t in PWR_Interface.h
      0 : no LowPower, WFI only
      1 : Reserved
      2 : Deep Sleep
      3 : Power Down
    Note that if a Ble State is configured to Power Down mode, please make sure
       gLowpowerPowerDownEnable_d variable is set to 1 in Linker Script
    The PowerDown mode will allow lowest power consumption but the wakeup time is longer
       and the first 16K in SRAM is reserved to ROM code (this section will be corrupted on
       each power down wakeup so only temporary data could be stored there.)     */
#define gAppLowPowerModeConstraints_c   2

#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
#define gSecLibAllowLtkFromBlob_c       1
#endif
/*! *********************************************************************************
 *     RTOS Configuration
 ********************************************************************************** */
#if defined(SDK_OS_FREE_RTOS)
/* Fsci Task Stack size */
#if defined(DEBUG)
#define gFsciTaskStackSize_c            4600
#else
#define gFsciTaskStackSize_c            2600
#endif

#define gHost_TaskStackSize_c           2000

#define configTIMER_QUEUE_LENGTH        30
#endif

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */

/* Enable/Disable Dynamic GattDb functionality */
#define gGattDbDynamic_d                0

/* Size of prepare write queue. */
#define gPrepareWriteQueueSize_c        6

/* Number of bonded devices supported by the application */
#define gMaxBondedDevices_c             24

/* Enable 5.3 optional features */
#define gBLE53_d                        1

/*
    How to handle L2CAP data and events on the EATT PSM
    TRUE - forward to EATT layer callbacks (default behavior)
    FALSE - forward to application callbacks (testing purposes)
*/
#define gSendEattPsmToEattLayer_c        TRUE

#define gGapSimultaneousEAChainedReports_c  (2U)

/*If enable the gUseHciTransportDownward_d macro, Two serial interfaces are required.
 *One serial port(LPUART0) prints the log and the other serial port(LPUART1) sends HCI packets */
#if (defined(gUseHciTransportDownward_d) &&(gUseHciTransportDownward_d))
#define gHcitInterfaceType_d            gSerialMgrLpuart_c
#define gHcitInterfaceNumber_d          BOARD_DEBUG_UART_INSTANCE
#define gHcitInterfaceSpeed_d           BOARD_DEBUG_UART_BAUDRATE
#endif

/*! *********************************************************************************
 *     BLE LL Configuration
 ***********************************************************************************/
/*  ble_ll_config.h file lists the parameters with their default values. User can override
 *    the parameter here by defining the parameter to a user defined value. */
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
