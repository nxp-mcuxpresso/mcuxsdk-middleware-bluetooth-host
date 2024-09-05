/*! *********************************************************************************
* \addtogroup BLE
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2024 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* DO NOT MODIFY THIS FILE!
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "fsl_component_timer_manager.h"
#include "ble_general.h"
#include "att_types.h"
#include "ble_config.h"
#include "gap_types.h"
#include "ModuleInfo.h"
#include "gatt_types.h"
#include "fwk_seclib.h"
#include "gatt_client_interface.h"

/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/
/* WARNING: Do not change these defines */
#define gAttConnStorageSize_c            (8U)
#define gActiveDevicesStorageSize_c      (96U)
#define gL2caLeCbChannelEntrySize_c      (52U)
#define gL2caLePsmEntrySize_c            (4U)

#if defined(gBLE52_d) && (gBLE52_d == TRUE) && defined(gEATT_d) && (gEATT_d == TRUE)
#define gEattConnStorageSize             (8U)
#define gEattBearerDataStorageSize       (24U)
#endif

/************************************************************************************
*************************************************************************************
* Public function prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations - external references from Host library
*************************************************************************************
************************************************************************************/
extern const uint8_t gcGapMaximumBondedDevices_d;
const uint8_t gcGapMaximumBondedDevices_d = gMaxBondedDevices_c;
extern const uint8_t gcGapMaximumSavedCccds_d;
const uint8_t gcGapMaximumSavedCccds_d = gcGapMaximumSavedCccds_c;
extern bleBondIdentityHeaderBlob_t gaBondIdentityHeaderBlobs[gMaxBondedDevices_c + gMaxNonBondedDevices_c];
bleBondIdentityHeaderBlob_t gaBondIdentityHeaderBlobs[gMaxBondedDevices_c + gMaxNonBondedDevices_c];

extern const uint8_t gcGapMaximumNonBondedDevices_d;
const uint8_t gcGapMaximumNonBondedDevices_d = gMaxNonBondedDevices_c;
extern bleBondDataRam_t *gapNonBondedData[gMaxNonBondedDevices_c];
bleBondDataRam_t *gapNonBondedData[gMaxNonBondedDevices_c] = {0U};

/* Sizes for GATT Caching arrays */
extern const uint8_t gBleMaxActiveConnectionsGattCaching;
const uint8_t gBleMaxActiveConnectionsGattCaching = gBleAppMaxActiveConnectionsGattCaching_c;
extern const uint8_t gcGapMaximumBondedDevicesGattCaching;
const uint8_t gcGapMaximumBondedDevicesGattCaching = gcAppMaximumBondedDevicesGattCaching_c;

/* Size of EATT array */
extern const uint8_t gBleMaxActiveConnectionsEatt;
const uint8_t gBleMaxActiveConnectionsEatt = gBleAppMaxActiveConnectionsEatt_c;

extern const uint8_t gGapControllerResolvingListSize;
const uint8_t gGapControllerResolvingListSize = gMaxResolvingListSize_c;
extern gapIdentityInformation_t gaControllerPrivacyIdentities[gMaxResolvingListSize_c];
gapIdentityInformation_t gaControllerPrivacyIdentities[gMaxResolvingListSize_c];
extern gapCarSupport_t mCAR_Support[gMaxResolvingListSize_c];
gapCarSupport_t mCAR_Support[gMaxResolvingListSize_c];

extern const uint8_t gcGattMaxHandleCountForWriteNotifications_c;
const uint8_t gcGattMaxHandleCountForWriteNotifications_c = gMaxWriteNotificationHandles_c;
extern uint16_t gGattWriteNotificationHandles[gMaxWriteNotificationHandles_c];
uint16_t gGattWriteNotificationHandles[gMaxWriteNotificationHandles_c];
extern const uint8_t gcGattMaxHandleCountForReadNotifications_c;
const uint8_t gcGattMaxHandleCountForReadNotifications_c = gMaxReadNotificationHandles_c;
extern uint16_t gGattReadNotificationHandles[gMaxReadNotificationHandles_c];
uint16_t gGattReadNotificationHandles[gMaxReadNotificationHandles_c];

extern const uint8_t gcGapMaxServiceSpecificSecurityRequirements_c;
const uint8_t gcGapMaxServiceSpecificSecurityRequirements_c = gGapMaxServiceSpecificSecurityRequirements_c;
extern gapServiceSecurityRequirements_t gaServiceSecurityRequirements[gGapMaxServiceSpecificSecurityRequirements_c];
gapServiceSecurityRequirements_t gaServiceSecurityRequirements[gGapMaxServiceSpecificSecurityRequirements_c];

extern const uint16_t gcGattServerMtu_c;
const uint16_t gcGattServerMtu_c = gAttMaxMtu_c;

/* Queued Writes server globals, supporting gAppMaxConnections_c clients */
extern const uint8_t gcGattDbMaxPrepareWriteOperationsInQueue_c;
const uint8_t gcGattDbMaxPrepareWriteOperationsInQueue_c = gPrepareWriteQueueSize_c;
extern const uint8_t gcGattDbMaxPrepareWriteClients_c;
const uint8_t gcGattDbMaxPrepareWriteClients_c = gAppMaxConnections_c;
extern uint8_t gaGattDbPrepareWriteQueueIndexes[gAppMaxConnections_c];
uint8_t gaGattDbPrepareWriteQueueIndexes[gAppMaxConnections_c];
extern attPrepareQueueItem *gPrepareWriteQueues[gAppMaxConnections_c][gPrepareWriteQueueSize_c];
attPrepareQueueItem *gPrepareWriteQueues[gAppMaxConnections_c][gPrepareWriteQueueSize_c];

extern const uint16_t gGapDefaultTxOctets;
const uint16_t gGapDefaultTxOctets = gBleDefaultTxOctets_c;
extern const uint16_t gGapDefaultTxTime;
const uint16_t gGapDefaultTxTime = gBleDefaultTxTime_c;

extern const uint16_t gGapHostPrivacyTimeout;
const uint16_t gGapHostPrivacyTimeout = gBleHostPrivacyTimeout_c;
extern const uint16_t gGapControllerPrivacyTimeout;
const uint16_t gGapControllerPrivacyTimeout = gBleControllerPrivacyTimeout_c;
extern const bool_t gGapLeScOobHasMitmProtection;
const bool_t gGapLeScOobHasMitmProtection = gBleLeScOobHasMitmProtection_c;
extern const uint8_t gGapSimultaneousEAChainedReports;
const uint8_t gGapSimultaneousEAChainedReports = gGapSimultaneousEAChainedReports_c;

/* The following definitions are required by the VERSION_TAGS. DO NOT MODIFY or REMOVE */
extern const moduleInfo_t BLE_HOST_VERSION;

#if defined ( __IAR_SYSTEMS_ICC__ )
extern uint8_t ble_dummy;
#pragma required=BLE_HOST_VERSION /* force the linker to keep the symbol in the current compilation unit */
uint8_t ble_dummy; /* symbol suppressed by the linker as it is unused in the compilation unit, but necessary because
                             to avoid warnings related to #pragma required */
#elif defined(__GNUC__)
static const moduleInfo_t *const dummy __attribute__((__used__)) = &BLE_HOST_VERSION;
#endif /* __IAR_SYSTEMS_ICC__ */

/* BLE Host connection storage */
const uint8_t gBleMaxActiveConnections = gAppMaxConnections_c;
extern uint32_t gAttConnStorage[(gAttConnStorageSize_c * gAppMaxConnections_c + 3) / 4];
uint32_t gAttConnStorage[(gAttConnStorageSize_c * gAppMaxConnections_c + 3) / 4];
extern uint32_t gActiveDevicesStorage[(gActiveDevicesStorageSize_c * gAppMaxConnections_c + 3) / 4];
uint32_t gActiveDevicesStorage[(gActiveDevicesStorageSize_c * gAppMaxConnections_c + 3) / 4];
const uint16_t gcConnectionEventMinDefault_c = gGapConnEventLengthMin_d; /*! Time = N * 0.625 ms */
const uint16_t gcConnectionEventMaxDefault_c = gGapConnEventLengthMax_d; /*! Time = N * 0.625 ms */
/* LE credit-based channels storage */
extern const uint8_t gcL2caMaxLePsmSupported;
const uint8_t gcL2caMaxLePsmSupported = gL2caMaxLePsmSupported_c;
extern const uint8_t gcL2caMaxLeCbChannels;
const uint8_t gcL2caMaxLeCbChannels = gL2caMaxLeCbChannels_c;
extern uint32_t gL2caPsmStorage[(gL2caLePsmEntrySize_c * gL2caMaxLePsmSupported_c + 3) / 4];
uint32_t gL2caPsmStorage[(gL2caLePsmEntrySize_c * gL2caMaxLePsmSupported_c + 3) / 4];
extern uint32_t gL2caCbChannelStorage[(gL2caLeCbChannelEntrySize_c * gL2caMaxLeCbChannels_c + 3) / 4];
uint32_t gL2caCbChannelStorage[(gL2caLeCbChannelEntrySize_c * gL2caMaxLeCbChannels_c + 3) / 4];
extern const uint16_t gcL2caLowPeerCreditsThreshold;
const uint16_t gcL2caLowPeerCreditsThreshold = gL2caLowPeerCreditsThreshold_c;

/* EATT connection storage */
const uint8_t gBleEattMaxConnectionChannels = gAppEattMaxNoOfBearers_c;
const uint16_t gBleEattPsmMtu = gAppEattDefaultMtu_c;

#if defined(gBLE52_d) && (gBLE52_d == TRUE) && defined (gEATT_d) && (gEATT_d == TRUE)
extern uint32_t gEattConnStorage[(gEattConnStorageSize * gBleAppMaxActiveConnectionsEatt_c + 3) / 4];
uint32_t gEattConnStorage[(gEattConnStorageSize * gBleAppMaxActiveConnectionsEatt_c + 3) / 4];
extern uint32_t gEattBearerData[(gEattBearerDataStorageSize * gAppEattMaxNoOfBearers_c * gBleAppMaxActiveConnectionsEatt_c + 3) / 4];
uint32_t gEattBearerData[(gEattBearerDataStorageSize * gAppEattMaxNoOfBearers_c * gBleAppMaxActiveConnectionsEatt_c + 3) / 4];
#else
extern uint32_t gEattConnStorage[1];
uint32_t gEattConnStorage[1];
extern uint32_t gEattBearerData[1];
uint32_t gEattBearerData[1];
#endif /* gBLE52_d && gEATT_d */

/* Private structure sizes used for run-time checks */
extern const uint16_t gAttConnEntrySize;
extern const uint16_t gActiveDevicesEntrySize;
extern const uint8_t gL2caLePsmEntrySize;
extern const uint8_t gL2caLeCbChannelEntrySize;

extern const uint8_t gMaxL2caQueueSize;
const uint8_t gMaxL2caQueueSize = gMaxL2caQueueSize_c;
const uint8_t gMaxAdvReportQueueSize = gMaxAdvReportQueueSize_c;

/* Timer interval after which the incomplete accumulated EA report is freed */
extern const uint32_t gFreeEAReportMs;
const uint32_t gFreeEAReportMs = gBleHostFreeEAReportTimeoutMs_c;

/* Service changed indication buffer */
extern gattHandleRange_t gServiceChangedIndicationStorage[gMaxBondedDevices_c];
gattHandleRange_t gServiceChangedIndicationStorage[gMaxBondedDevices_c];


/* server values for its own service changed characteristic and CCCD handles */
uint16_t mServerServiceChangedCharHandle;
uint16_t mServerServiceChangedCCCDHandle;

/* Indicate to the GATT Client if it should trigger a database update procedure upon receiving
   a database out of sync error response from a server */
extern bool_t mbGattUseUpdateDatabaseCopyProc;
bool_t mbGattUseUpdateDatabaseCopyProc = gGattUseUpdateDatabaseCopyProc_c;

#if defined(gBLE51_d) && (gBLE51_d == 1U) && defined(gGattCaching_d) && (gGattCaching_d == 1U)
/* client saved values for service changed characteristic and CCCD handles for each possible server */
uint16_t mActiveServiceChangedCharHandle[gBleAppMaxActiveConnectionsGattCaching_c] = {gGattDbInvalidHandle_d};
extern uint16_t mServiceChangedCharHandle[gcAppMaximumBondedDevicesGattCaching_c];
uint16_t mServiceChangedCharHandle[gcAppMaximumBondedDevicesGattCaching_c] = {gGattDbInvalidHandle_d};
uint16_t mActiveServiceChangedCCCDHandle[gBleAppMaxActiveConnectionsGattCaching_c] = {gGattDbInvalidHandle_d};

/* client state information for bonded and active clients */
extern gattCachingClientState_c gGattClientState[gcAppMaximumBondedDevicesGattCaching_c];
gattCachingClientState_c gGattClientState[gcAppMaximumBondedDevicesGattCaching_c];
extern gattCachingClientState_c gGattActiveClientState[gBleAppMaxActiveConnectionsGattCaching_c * (1U + gAppEattMaxNoOfBearers_c)];
gattCachingClientState_c gGattActiveClientState[gBleAppMaxActiveConnectionsGattCaching_c * (1U + gAppEattMaxNoOfBearers_c)];

/* Database hash values - the client needs a hash value for each possible peer */
extern uint8_t mGattActiveServerDatabaseHash[gGattDatabaseHashSize_c * gBleAppMaxActiveConnectionsGattCaching_c];
uint8_t mGattActiveServerDatabaseHash[gGattDatabaseHashSize_c * gBleAppMaxActiveConnectionsGattCaching_c] = {0};

extern uint8_t* gpGattActiveServerDatabaseHash[gBleAppMaxActiveConnectionsGattCaching_c];
uint8_t* gpGattActiveServerDatabaseHash[gBleAppMaxActiveConnectionsGattCaching_c] = {NULL};

extern uint8_t mGattServerDatabaseHash[gGattDatabaseHashSize_c * gcAppMaximumBondedDevicesGattCaching_c];
uint8_t mGattServerDatabaseHash[gGattDatabaseHashSize_c * gcAppMaximumBondedDevicesGattCaching_c] = {0};

/* client supported features handles for active gatt servers */
uint16_t gGattActiveClientSupportedFeaturesHandles[gBleAppMaxActiveConnectionsGattCaching_c] = {gGattDbInvalidHandle_d};

/* client supported features information for bonded gatt clients */
extern uint8_t gGattClientSupportedFeatures[gcAppMaximumBondedDevicesGattCaching_c];
uint8_t gGattClientSupportedFeatures[gcAppMaximumBondedDevicesGattCaching_c] = {0U};
#else
/* client saved values for service changed characteristic and CCCD handles for each possible server */
extern uint16_t mActiveServiceChangedCharHandle[1];
uint16_t mActiveServiceChangedCharHandle[1];
extern uint16_t mServiceChangedCharHandle[1];
uint16_t mServiceChangedCharHandle[1];
extern uint16_t mActiveServiceChangedCCCDHandle[1];
uint16_t mActiveServiceChangedCCCDHandle[1];

/* client state information for bonded and active clients */
extern gattCachingClientState_c gGattClientState[1];
gattCachingClientState_c gGattClientState[1];
extern gattCachingClientState_c gGattActiveClientState[1];
gattCachingClientState_c gGattActiveClientState[1];

/* Database hash values - the client needs a hash value for each possible peer */
extern uint8_t mGattActiveServerDatabaseHash[1];
uint8_t mGattActiveServerDatabaseHash[1];
extern uint8_t* gpGattActiveServerDatabaseHash[1];
uint8_t* gpGattActiveServerDatabaseHash[1];
extern uint8_t mGattServerDatabaseHash[1];
uint8_t mGattServerDatabaseHash[1];

/* client supported features handles for active gatt servers */
extern uint16_t gGattActiveClientSupportedFeaturesHandles[1];
uint16_t gGattActiveClientSupportedFeaturesHandles[1];

/* client supported features information for bonded gatt clients */
extern uint8_t gGattClientSupportedFeatures[1];
uint8_t gGattClientSupportedFeatures[1];
#endif /* gBLE51_d && gGattCaching_d */

extern procStatus_t gProcedureStatus[gAppMaxConnections_c * (1U + gAppEattMaxNoOfBearers_c)];
procStatus_t gProcedureStatus[gAppMaxConnections_c * (1U + gAppEattMaxNoOfBearers_c)];
procDataStruct_t* pProcedureData[gAppMaxConnections_c * (1U + gAppEattMaxNoOfBearers_c)];
extern bool_t gIndicationPendingData[gAppMaxConnections_c * (1U + gAppEattMaxNoOfBearers_c)];
bool_t gIndicationPendingData[gAppMaxConnections_c * (1U + gAppEattMaxNoOfBearers_c)] = {FALSE};

extern const bool_t gAdvSetsUseSameRandomAddr;
const bool_t gAdvSetsUseSameRandomAddr = gBleHostExtAdvUseSameRandomAddr_c;

extern const bool_t gAutoRejectLtkRequestForUnbondedDevices;
const bool_t gAutoRejectLtkRequestForUnbondedDevices = gBleHostAutoRejectLtkRequestForUnbondedDevices_c;

#if defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U)
extern const bool_t gSendEattPsmToEattLayer;
const bool_t gSendEattPsmToEattLayer = gSendEattPsmToEattLayer_c;
#endif

/*! How to handle ECDH keys generation
 * TRUE - use S200 blobs for LeSC, IRK and CSRK
 * FALSE - use default framework */
extern const bool_t gUseS200;
#if defined(gAppSecureMode_d) && (gAppSecureMode_d == 1U)
const bool_t gUseS200 = gAppSecureMode_d;
secLibFunctions_t gSecLibFunctions = {ECDH_P256_FreeDhKeyDataSecure, 
                                     SecLib_DeriveBluetoothSKDSecure, 
                                     SecLib_ObfuscateKeySecure, 
                                     SecLib_DeobfuscateKeySecure, 
                                     ECDH_P256_ComputeA2BKeySecure, 
                                     ECDH_P256_FreeE2EKeyDataSecure, 
                                     SecLib_ExportA2BBlobSecure, 
                                     SecLib_ImportA2BBlobSecure, 
                                     SecLib_GenerateBluetoothF5KeysSecure, 
                                     SecLib_VerifyBluetoothAhSecure, 
                                     SecLib_GenerateBluetoothEIRKBlobSecure};
#else
const bool_t gUseS200 = 0U;
secLibFunctions_t gSecLibFunctions = {NULL, 
                                     NULL, 
                                     NULL, 
                                     NULL, 
                                     NULL, 
                                     NULL,
                                     NULL, 
                                     NULL, 
                                     SecLib_GenerateBluetoothF5Keys, 
                                     SecLib_VerifyBluetoothAh,
                                     NULL};
#endif

/*! Enables/Disables Hci Command Flow Control if the host lib supports this feature. 
 *  The host lib must be compiled with gHciCommandFlowControlSupport_d set TRUE
 *  to support it.*/
#if defined(gAppHciCommandFlowControl_d) &&  (gAppHciCommandFlowControl_d == 1U)
const bool_t gUseHciCommandFlowControl = 1U;
#else
const bool_t gUseHciCommandFlowControl = 0U;
#endif

/*! Allows an advertising set to advertise with a public address (if configured as such
 *  via the Gap_SetExtAdvertisingParameters API) even if Host/Controller Privacy is enabled.
 *  Set gDisablePrivacyPerAdvSet[adv_handle] to TRUE to ignore Privacy for the set identified by adv_handle,
 *  FALSE for default behavior.
 *  WARNING: this feature is not compliant with the Bluetooth LE Core Spec. Use at own discretion.
 */
#if defined(gBLE50_d) && (gBLE50_d == 1U)
extern bool_t gDisablePrivacyPerAdvSet[gMaxAdvSets_c];
bool_t gDisablePrivacyPerAdvSet[gMaxAdvSets_c] = {FALSE};
#endif
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
bool_t Ble_CheckMemoryStorage(void)
{
    bool_t status = TRUE;

    if ((uint32_t)gAttConnEntrySize * (uint32_t)gAppMaxConnections_c > sizeof(gAttConnStorage))
    {
        status = FALSE;
    }

    if ((uint32_t)gActiveDevicesEntrySize * (uint32_t)gAppMaxConnections_c > sizeof(gActiveDevicesStorage))
    {
        status = FALSE;
    }

    if (gL2caLeCbChannelEntrySize > gL2caLeCbChannelEntrySize_c)
    {
        status = FALSE;
    }

    if (gL2caLePsmEntrySize > gL2caLePsmEntrySize_c)
    {
        status = FALSE;
    }

    return status;
}

/*! *********************************************************************************
* @}
********************************************************************************** */
