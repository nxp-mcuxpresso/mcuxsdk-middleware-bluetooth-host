/*! *********************************************************************************
 * \defgroup app app
 * @{
 ********************************************************************************** */
/*
 * Copyright 2025 NXP
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_PREINCLUDE_H_
#define _APP_PREINCLUDE_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/*! *********************************************************************************
 *     Board Configuration
 ********************************************************************************** */
/*! Enable Debug Console (PRINTF) */
#define gDebugConsoleEnable_d 1

/*! Enable Time Service */
#define gAppUseTimeService_d 1

/*! *********************************************************************************
 *     App Configuration
 ********************************************************************************** */

/* Enable Serial Manager interface */
#define gAppUseSerialManager_c  1

/*! Enable/disable use of bonding capability */
#define gAppUseBonding_d 1

/*! Enable/disable use of pairing procedure */
#define gAppUsePairing_d 1

/*! Enable/disable use of privacy */
#define gAppUsePrivacy_d 1

/*! Set Passkey value */
#define gPasskeyValue_c 999999

/*! Repeated Attempts - Mitigation for pairing attacks */
#define gRepeatedAttempts_d 0

/*! NVM TABLE is stored in RAM */
#define gNvTableKeptInRam_d TRUE

/*! We want printing in the panic function. */
#define PANIC_ENABLE_LOG 1

/*! The RSSI threshold used for proximity pairing. */
#define PROXIMITY_RSSI_THRESHOLD -42

/*! When set, the central will search for a device with the name defined by PEER_NAME.
 * When found, it will connect to it, irrespective of the RSSI. When not set,
 * it will scan for the all devices with names starting with "REF_APP_". Only when
 * the RSSI is stronger than PROXIMITY_RSSI_THRESHOLD, it'll connect to it */
/* #define PEER_NAME "REF_APP_F921" */

/*! *********************************************************************************
 *     Framework Configuration
 ********************************************************************************** */
/* enable NVM to be used as non volatile storage management by the host stack */
#define gAppUseNvm_d 1

/*! The minimum heap size needed:
    5 blocks of 56
    3 blocks of 104
    1 block of 248
    1 block of 312
    1 block of 392
*/
#define MinimalHeapSize_c 5200

/*! *********************************************************************************
 *   NVM Module Configuration - gAppUseNvm_d shall be defined above as 1 or 0
 ********************************************************************************** */

#if gAppUseNvm_d
/* configure NVM module */
#define gNvStorageIncluded_d (1)
/*! Set to 1 to enable fragmentation */
#define gNvFragmentation_Enabled_d (1)
/*! Set to 1 to enable flex NVM */
#define gNvUseFlexNVM_d (0)
#if gNvUseFlexNVM_d
#define gUnmirroredFeatureSet_d (0)
#else
/*! Set to 1 to unmirror feature set */
#define gUnmirroredFeatureSet_d (1)
#endif /* gNvUseFlexNVM_d */
#if gNvFragmentation_Enabled_d
/*! Buffer size large enough to accommodate the maximum number of CCCDs for every device. */
#define gNvRecordsCopiedBufferSize_c (gMaxBondedDevices_c * 16)
#endif /* gNvFragmentation_Enabled_d */
#endif /* gAppUseNvm_d */

/*! *********************************************************************************
 * 	RTOS Configuration
 ********************************************************************************** */
/* Defines the RTOS used */

/*! Defines controller task stack size */
#define gControllerTaskStackSize_c 4000

/*! Defines controller high priority task stack size */
#define gControllerHighPriorityTaskStackSize_c 2000

/*! Defines total heap size used by the OS - 11k */
#define gTotalHeapSize_c 0xc400

/*! *********************************************************************************
 *     BLE Stack Configuration
 ********************************************************************************** */

/*! Defines the max count of services. */
#define gMaxServicesCount_d 6
/*! Defines the max count of services characters. */
#define gMaxServiceCharCount_d 6

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

/* Number of bonded devices supported by the application. */
#ifndef gMaxBondedDevices_c
/* Make sure that (gMaxBondedDevices_c * gBleBondDataSize_c) fits into the Flash area
 * reserved by the application for bond information. */
#define gMaxBondedDevices_c gAppMaxConnections_c
#endif /* gMaxBondedDevices_c */

/*! Size of bond data structures for a bonded device  */
#define gBleBondIdentityHeaderSize_c (56U)

/* Setting list size parameter value if privacy is not used. */
#if (gAppUsePrivacy_d == 0)
#ifndef gMaxResolvingListSize_c
/*! Set minimal value in gMaxResolvingListSize_c */
#define gMaxResolvingListSize_c 16
#endif /* gMaxResolvingListSize_c */
#endif /* gAppUsePrivacy_d */

#if (gAppUseBonding_d) && (!gAppUsePairing_d)
#error "Enable pairing to make use of bonding"
#endif /* (gAppUseBonding_d) && (!gAppUsePairing_d) */

/* Enable/Disable database hash computation */
#if defined(gBLE51_d) && (gBLE51_d == 1U)
/* Default enabled for dynamic databases and disabled for static ones */
#if defined(gGattDbDynamic_d) && (gGattDbDynamic_d == 1U)
#define gGattDbComputeHash_d (1U)
#else
#ifndef gGattDbComputeHash_d
#define gGattDbComputeHash_d (0U)
#endif /* #ifndef gGattDbComputeHash_d */
#endif /* gGattDbDynamic_d && gGattDbDynamic_d == 1U) */
#endif /* defined(gBLE51_d) && (gBLE51_d == 1U) */

#ifndef gGattUseUpdateDatabaseCopyProc_c
/*! Set gGattUseUpdateDatabaseCopyProc_c to False if not defined. */
#define gGattUseUpdateDatabaseCopyProc_c FALSE
#endif /* gGattUseUpdateDatabaseCopyProc_c */

/*! *********************************************************************************
 *   Auto Configuration
 ********************************************************************************** */

#if (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d == 1)) || (defined(gAppUseNvm_d) && (gAppUseNvm_d == 1))
/* Enable FreeRtos Idle Hook for NVM saving in Idle and CM33 lowpower */
#define configUSE_IDLE_HOOK 1
#endif /* gAppLowpowerEnabled_d && gAppLowpowerEnabled_d || gAppUseNvm_d && gAppUseNvm_d */

/*! *********************************************************************************
 *     Component Configuration
 ***********************************************************************************/
#if defined(gDebugConsoleEnable_d)
#define SERIAL_MANAGER_NON_BLOCKING_DUAL_MODE gDebugConsoleEnable_d
#endif /* gDebugConsoleEnable_d */

/*! Set light memory manager. */
#define gMemManagerLight 1

/*! *********************************************************************************
 *   Host stack configuration
 ********************************************************************************** */

/*! The MemManager requires to know the number of connection for LL buffer sizing */
#define gAppMaxConnections_c (1U)

/*! Number of credit-based channels supported */
#define gL2caMaxLeCbChannels_c (2U)

/*! Simultaneous EA chained reports.
 * This value must be overwritten by applications that enable BLE 5.0 Observer features */
#define gGapSimultaneousEAChainedReports_c (0U)

/*! Defines number of timers needed by the protocol stack */
#define gTmrStackTimers_c \
    (3U + (gAppMaxConnections_c * 2U) + gL2caMaxLeCbChannels_c + gGapSimultaneousEAChainedReports_c)

/*! *********************************************************************************
 *   LlMem - gLlMemPoolId_c shall be defined above as 1
 ********************************************************************************** */
/*! *********************************************************************************
gLlUsePeriodicAdvertising_d:
---------------------------
If set to 1, periodic advertising from AE is enabled. If 0, periodic advertising is disabled.
gAppExtAdvEnable_d:
------------------
If set to 1, advertising extensions (extended + periodic) are enabled. If set to 0, AEs are disabled.
 ********************************************************************************** */
#ifndef gLlMemPoolId_c
/* If define is not set by application, use a common pool for app/host and LL. */
#define gLlMemPoolId_c 0
#else /* gLlMemPoolId_c */
/* Application set the flag, make sure it is valid. */
#if (gLlMemPoolId_c > 1)
#error Please select pool 0 or pool 1
#endif /* (gLlMemPoolId_c > 1) */
#endif /* gLlMemPoolId_c */

#if defined(gLlUsePeriodicAdvertising_d)
/* check compile switch incompatibilities */
#if defined(gAppExtAdvEnable_d)
#if ((gAppExtAdvEnable_d == 0) && (gLlUsePeriodicAdvertising_d == 1))
#error Compile switch incompatibility! gLlUsePeriodicAdvertising_d=1 shall not be used with gAppExtAdvEnable_d=0
#endif /* ((gAppExtAdvEnable_d == 0) && (gLlUsePeriodicAdvertising_d == 1)) */
#else
/* Periodic advertising support needs extended advertising support. */
#if (gLlUsePeriodicAdvertising_d == 1)
#define gAppExtAdvEnable_d 1
#endif /* (gLlUsePeriodicAdvertising_d == 1) */
#endif /*defined(gAppExtAdvEnable_d)*/
#endif /* (defined(gLlUsePeriodicAdvertising_d)) */

#if (defined(gLlScanPeriodicAdvertiserListSize_c) && !defined(gLlScanAdvertiserListSize_c))
#if (gLlScanPeriodicAdvertiserListSize_c != 0)
#define gLlScanAdvertiserListSize_c (26 - gLlScanPeriodicAdvertiserListSize_c)
#endif /* (gLlScanPeriodicAdvertiserListSize_c != 0) */
#endif /* (defined(gLlScanPeriodicAdvertiserListSize_c) && !defined(gLlScanAdvertiserListSize_c)) */

#if (!defined(gAppExtAdvEnable_d))
/*! Set gAppExtAdvEnable_d to 0 if not defined. */
#define gAppExtAdvEnable_d 0
#endif /* (!defined(gAppExtAdvEnable_d)) */

/* Defines L1Mem pools by block size and number of blocks. Must be aligned to 4 bytes.*/
#if (gAppExtAdvEnable_d == 0)
/*! Large size events (<= 72 bytes).*/
#define gLlBufferNbrLargeSizeEvent_c (4) /* BT_FW_LE_EVENT_TYPE1_BUFFERS */
/*! Medium size events (<= 32 bytes).*/
#define gLlBufferNbrMediumSizeEvent_c (4) /* BT_FW_LE_EVENT_TYPE2_BUFFERS */
/*! Small size events (<= 12 bytes).*/
#define gLlBufferNbrSmallSizeEvent_c (6) /* BT_FW_LE_EVENT_TYPE3_BUFFERS */
/*! Generic events (<= 72 bytes).*/
#define gLlBufferGenericSizeEvent_c (4) /* BT_FW_LE_EVENT_TYPE4_BUFFERS */
/*! Large events (80 bytes).*/
#define gLlCmdBuffer80Bytes_c (1)

/*If extended advertising is not set, use legacy settings for advertising*/
#ifndef gLlMaxUsedAdvSet_c
/*! Set gLlMaxUsedAdvSet_c */
#define gLlMaxUsedAdvSet_c 1
#endif /* gLlMaxUsedAdvSet_c */
#ifndef gLlMaxExtAdvDataLength_c
/*! Set gLlMaxExtAdvDataLength_c to 31 */
#define gLlMaxExtAdvDataLength_c 31
#endif /* gLlMaxExtAdvDataLength_c */
#ifndef gLlUsePeriodicAdvertising_d
/*! Unset gLlUsePeriodicAdvertising_d. */
#define gLlUsePeriodicAdvertising_d 0
#endif /* gLlUsePeriodicAdvertising_d */

#if (gLlMemPoolId_c == 1)
#ifndef LlPoolsDetails_c
#define LlPoolsDetails_c                                                                                               \
    _block_set_(32, (gLlBufferNbrSmallSizeEvent_c + gLlBufferNbrMediumSizeEvent_c + ((3 + 4) * gAppMaxConnections_c)), \
                1) _eol_ _block_set_(64, ((2 * gAppMaxConnections_c)), 1)                                              \
        _eol_ _block_set_(80, (gLlBufferNbrLargeSizeEvent_c + gLlCmdBuffer80Bytes_c + gLlBufferGenericSizeEvent_c), 1) \
            _eol_ _block_set_(268, (gLlBufferNbrTxAclPkts + gLlBufferNbrRxAclPkts), 1) _eol_
#endif /* LlPoolsDetails_c */
#else  /* (gLlMemPoolId_c == 1) */
/*! LlPoolsDetails_c check. */
#ifdef LlPoolsDetails_c
#error Single pool is used, please do not define LlPoolsDetails_c in app_preinclude.h
#else
/*! Define LlPoolsDetails_c. */
#define LlPoolsDetails_c
#endif                                     /* LlPoolsDetails_c */
#endif                                     /* (gLlMemPoolId_c == 1) */
#else                                      /* (gAppExtAdvEnable_d == 0) */
/*Large size events (<= 288 bytes).*/
#define gLlBufferNbrLargeSizeEvent_c  (10) /* BT_FW_LE_EVENT_TYPE1_BUFFERS */
/*Medium size events (<= 128 bytes).*/
#define gLlBufferNbrMediumSizeEvent_c (16) /* BT_FW_LE_EVENT_TYPE2_BUFFERS */
/*Small size events (<= 64 bytes).*/
#define gLlBufferNbrSmallSizeEvent_c  (12) /* BT_FW_LE_EVENT_TYPE3_BUFFERS */
/*Generic events (<= 72 bytes).*/
#define gLlBufferGenericSizeEvent_c   (6)  /* BT_FW_LE_EVENT_TYPE4_BUFFERS */

#define gLlCmdBuffer288Bytes_c (1)

/*
 * Default configuration for LlPoolsDetails_c
 *
 * The 128 bytes buffer pool has been changed to a 80 bytes buffer pool: 128 byte buffer for events whose size is > 64 &
 * < 128 is not a good size because there are three events in this range with size = 72
 * (HCI_READ_LOCAL_SUPPORTED_COMMANDS & HCI_LE_READ_LOCAL_P256_PUBLIC_KEY) and size = 74 (HCI_VENDOR_DTM_RX_PKT_EVENT)
 *         => take 80 bytes buffer size instead
 */
#if (gLlMemPoolId_c == 1)
#ifndef LlPoolsDetails_c
#define LlPoolsDetails_c                                                                      \
    _block_set_(32, ((3 + 4) * gAppMaxConnections_c), 1)                                      \
        _eol_ _block_set_(64, (gLlBufferNbrSmallSizeEvent_c + (2 * gAppMaxConnections_c)), 1) \
            _eol_ _block_set_(80, (gLlBufferGenericSizeEvent_c), 1, 0)                        \
                _eol_ _block_set_(128, (gLlBufferNbrMediumSizeEvent_c), 1)                    \
                    _eol_ _block_set_(268, (gLlBufferNbrTxAclPkts + gLlBufferNbrRxAclPkts), 1) _eol_
_block_set_(288, (gLlBufferNbrLargeSizeEvent_c + gLlCmdBuffer288Bytes_c), 1) _eol_
#endif /* LlPoolsDetails_c */
#else  /* (gLlMemPoolId_c == 1) */
#ifdef LlPoolsDetails_c
#error Single pool is used, please do not define LlPoolsDetails_c in app_preinclude.h
#else
#define LlPoolsDetails_c
#endif /* LlPoolsDetails_c */
#endif /* (gLlMemPoolId_c == 1) */
#endif /* (gAppExtAdvEnable_d == 0) */

/*! *********************************************************************************
 *   Xcvr Configuration
 ********************************************************************************** */

/* Define the max tx power setting in dBm. Allowed values 0, 7 or 10 */
#if !defined(gAppMaxTxPowerDbm_c)
#define gAppMaxTxPowerDbm_c 10
#endif /* !defined(gAppMaxTxPowerDbm_c) */

/*! Set maximum transmit power. */
#define PLATFORM_MAX_TX_POWER_DBM 10U

/*! *********************************************************************************
 *     RTOS Configuration
 ********************************************************************************** */
#if defined(SDK_OS_FREE_RTOS)

#ifndef gMainThreadStackSize_c
#define gMainThreadStackSize_c 2600
#endif /* defined(SDK_OS_FREE_RTOS) */

#ifndef gHost_TaskStackSize_c
#define gHost_TaskStackSize_c 2800
#endif /* gHost_TaskStackSize_c */

#ifndef gAppTaskWaitTimeout_ms_c
#define gAppTaskWaitTimeout_ms_c osaWaitForever_c
#endif /* gAppTaskWaitTimeout_ms_c */

#endif /* SDK_OS_FREE_RTOS */
#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
