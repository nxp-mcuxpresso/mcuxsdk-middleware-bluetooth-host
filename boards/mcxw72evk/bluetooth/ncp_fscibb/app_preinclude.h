/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*!
 * Copyright 2019-2025 NXP
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

/*! *********************************************************************************
 * 	App Configuration
 ********************************************************************************** */
/* Use an optimized version of the application, for Matter commissioning */
/* Ensure that the Matter flavor of the Host library is used */
#ifndef gMatterConfig_d
    #define gMatterConfig_d                 0
#endif /* gMatterConfig_d */

/*! Maximum number of connections supported for this application */
#define gAppMaxConnections_c            8U

/* The maximum number of BLE connection supported by platform */
#define MAX_PLATFORM_SUPPORTED_CONNECTIONS     (24U)

#define gNcpApplication_d               1

/* Number of devices identified by address to keep track of for Repeated Attempts */
#ifndef gRepeatedAttemptsNoOfDevices_c
    #define gRepeatedAttemptsNoOfDevices_c  (4U)
#endif

/* Minimum timeout after a pairing failure before the same peer can re-attempt it */
#ifndef gRepeatedAttemptsTimeoutMin_c
    #define gRepeatedAttemptsTimeoutMin_c   (10U) /* seconds */
#endif

/* Maximum timeout after a pairing failure before the same peer can re-attempt it */
#ifndef gRepeatedAttemptsTimeoutMax_c
    #define gRepeatedAttemptsTimeoutMax_c   (640U) /* seconds */
#endif

#define gMainThreadStackSize_c      2600

#define gAppTaskWaitTimeout_ms_c       osaWaitForever_c

/*! *********************************************************************************
 *     Framework Configuration
 ********************************************************************************** */
 /* enable NVM to be used as non volatile storage management by the host stack */
#define gAppUseNvmNcp_d                 1

#define gFSCI_ResetCpu_c -              0

#define gFsciOverRpmsg_c                1

#define gPlatformUseLptmr_d             1

/*! *********************************************************************************
 *     Component Configuration
 ***********************************************************************************/
#ifndef gMemManagerLightExtendHeapAreaUsage
#define gMemManagerLightExtendHeapAreaUsage   1
#endif
/*! *********************************************************************************
 *     HKB Definition
 ********************************************************************************** */
#define SERIAL_MANAGER_NON_BLOCKING_DUAL_MODE   0

/*! The minimum heap size needed (measured with MEM_STATISTICS) */
#define MinimalHeapSize_c               17000

/* Enable/Disable FSCI */
#define gFsciIncluded_c                 1

/* Enable/Disable FSCI Low Power Commands*/
#define gFSCI_IncludeLpmCommands_c      0

/* Defines FSCI length - set this to FALSE is FSCI length has 1 byte */
#define gFsciLenHas2Bytes_c             1

/* Defines FSCI maximum payload length */
#define gFsciMaxPayloadLen_c            1660

/* Enable/Disable Ack transmission */
#define gFsciTxAck_c                    0

/* Enable/Disable Ack reception */
#define gFsciRxAck_c                    0

/* Enable FSCI Rx restart with timeout */
#define gFsciRxTimeout_c                1
#define mFsciRxTimeoutUsePolling_c      1

/* Use Misra Compliant version of FSCI module */
#define gFsciUseDedicatedTask_c         1

/* FSCI task size */
#if defined(DEBUG)
#define gFsciTaskStackSize_c            4600
#else
#define gFsciTaskStackSize_c            2600
#endif

/*! Enable/Disable PowerDown functionality in Application - In case of troubles with lowpower,
      turn it off for debug purpose to ensure your application is fine without lowpower  */
#define gAppLowpowerEnabled_d           0

/*! Enable/Disable to enable LPUART0 as wake up source by default
 *  This will prevent WAKE domain to go in retention and keep FRO6M running
 *  so the power consumption will increase during low power period.
 *  This assumes LPUART0 is used with Serial Manager (applicative serial interface).
 *  No effect if low power is disabled. */
#define gAppLpuart0WakeUpSourceEnable_d 0
          
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
          
/* Disable LEDs when enabling low power */
#if (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))
  #undef gAppLedCnt_c
  #define gAppLedCnt_c                0
#endif

/*! Enable the SWD pins to be managed into low-power */
#define gBoard_ManageSwdPinsInLowPower_d    0

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */

/* Enable Serial Manager interface */
#define gAppUseSerialManager_c              0

#define gUseHciTransport_d                  0

/* Enable/Disable Dynamic GattDb functionality */
#define gGattDbDynamic_d                    1

/* Enable FSCI BLE blackbox functionality */
#define gFsciBleBBox_d                      1

/* Indicates the Host layers which are enabled
 * for FSCI communication*/
#if (defined(gMatterConfig_d) && (gMatterConfig_d > 0))
/* L2CAP Credit-Based is not needed by Matter - disable it */
#define gFsciBleEnabledLayersMask_d         0x0160
#else
#define gFsciBleEnabledLayersMask_d         0x0964
#endif

/* Enable 5.3 optional features */
#define gBLE53_d                            1

#define gGapSimultaneousEAChainedReports_c  (2U)

/* Defines number of timers needed by the protocol stack */
#define gTmrStackTimers_c (3U + (gAppMaxConnections_c * 2U) + gL2caMaxLeCbChannels_c + gGapSimultaneousEAChainedReports_c)

/* Specify if the Bluetooth address is set using vendor specific command or using Controller API */
#define gBleSetMacAddrFromVendorCommand_d   (1)

/* The stack size of the Host task needs to be rise when using mbedTLS as mbedTLS structures are bigger */
#define gHost_TaskStackSize_c       1650

#define gL2caMaxLeCbChannels_c         (2U)

     /*! Size of bond data structures for a bonded device  */
#define gBleBondIdentityHeaderSize_c     (56U)

#define gGattUseUpdateDatabaseCopyProc_c FALSE

/*! Automatically enable Bluetooth optional features. The _OPT version of the Bluetooth
 * LE Host library enables all the features below. _OPT version of the library
 * must be used for linking */
#if (defined gBLE53_d) && (gBLE53_d == 1)
/*! Enable Bluetooth 5.0 optional features at application level (if present)
 * Extended advertising
 * Advertising sets
 * Periodic advertising
 * Set terminated event
 * Channel Selection algorithm
 * Read Maximum advertising data length
 * Scan timeout event
 * Scan request event
 * LDM timer */
#define gBLE50_d        1

/*! Enable Bluetooth 5.1 optional features at application level (if present)
 * Set Host Channel Classification
 * Periodic advertisement sync transfer
 * Gatt caching
 * Connection and connectionless CTE
 * Modify Sleep Clock Accuracy
 * Periodic advertising receive
 * Generate DH Key v2 */
#if (defined(gMatterConfig_d) && (gMatterConfig_d > 0))
#define gBLE51_d        0
#define gGattCaching_d  0
#else
#define gBLE51_d        1
#define gGattCaching_d  1
#endif

/*! Enable Bluetooth 5.2 optional features at application level (if present)
 * ATT multiple handle value notification
 * ATT multiple variable request/response
 * GATT client enhanced procedures
 * Enhanced L2CAP
 * EATT */
#if (defined(gMatterConfig_d) && (gMatterConfig_d > 0))
#define gBLE52_d        0
#define gEATT_d         0
#else
#define gBLE52_d        1
#define gEATT_d         1
#endif
#endif

/* Number of bonded devices supported by the application. */
#ifndef gMaxBondedDevices_c
    /* Make sure that (gMaxBondedDevices_c * gBleBondDataSize_c) fits into the Flash area
     * reserved by the application for bond information. */
    #define gMaxBondedDevices_c         gAppMaxConnections_c
#endif /* gMaxBondedDevices_c */

#if defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 0)
#ifndef gMaxResolvingListSize_c
    /* If privacy disabled, set to minimum size in order to save RAM */
    #define gMaxResolvingListSize_c     1
#endif /* gMaxResolvingListSize_c */
#else
#ifndef gMaxResolvingListSize_c
    /* LL supported maximum size */
    #define gMaxResolvingListSize_c     36
#endif /* gMaxResolvingListSize_c */
#endif

#if defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U)  \
    && defined(gAppUsePairing_d) && (gAppUsePairing_d == 0U)
    #error "Enable pairing to make use of bonding"
#endif

/* Enable/Disable database hash computation */
#if defined(gBLE51_d) && (gBLE51_d == 1U)
/* Default enabled for dynamic databases and disabled for static ones */
#if defined(gGattDbDynamic_d) && (gGattDbDynamic_d == 1U)
    #define gGattDbComputeHash_d        (1U)
#else
#ifndef gGattDbComputeHash_d
    #define gGattDbComputeHash_d        (0U)
#endif /* #ifndef gGattDbComputeHash_d */
#endif /* defined(gGattDbDynamic_d) && (gGattDbDynamic_d == 1U) */
#endif /* defined(gBLE51_d) && (gBLE51_d == 1U) */

#ifndef gGattUseUpdateDatabaseCopyProc_c
#define gGattUseUpdateDatabaseCopyProc_c FALSE
#endif

/* Enable/Disable application secure mode */
#ifndef gAppSecureMode_d
#define gAppSecureMode_d                 (0U)
#endif

#if (gAppSecureMode_d == 1U)
#define gSecLibSssUseEncryptedKeys_d     (1U)
#define gHostSecureMode_d                (1U)
#else
#define gHostSecureMode_d                (0U)
#endif

/* Max Services - update according to the usecase */
#define gMaxServicesCount_d             6U

/* Max Characteristics - update according to the usecase */
#define gMaxServiceCharCount_d          10U

/* Max Descriptors - update according to the usecase */
#define gMaxCharDescriptorsCount_d      4U

/*! *********************************************************************************
 *     BLE LL Configuration
 ***********************************************************************************/
/*  ble_ll_config.h file lists the parameters with their default values. User can override
 *    the parameter here by defining the parameter to a user defined value. */
#define gAppExtAdvEnable_d                      1
#define gLlScanPeriodicAdvertiserListSize_c     8

#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
