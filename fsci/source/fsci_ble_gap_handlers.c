/*! *********************************************************************************
* \addtogroup FSCI_BLE
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022 - 2024 NXP
*
*
* \file
*
* This is a source file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "fsci_ble_gap.h"
#include "fsci_ble_gap_types.h"
#include "fsci_ble_gap_handlers.h"
#include "controller_api.h"
#include "fwk_seclib.h"
#if gFsciIncluded_c && gFsciBleGapLayerEnabled_d

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
#if gFsciBleBBox_d || gFsciBleTest_d
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
static bool_t      mGenerateDKeyIRK = TRUE;
#endif  /* (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U)) */
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
#define fsciBleGapCallApiFunction(apiFunction)          bleResult_t result = (apiFunction); \
                                                        fsciBleGapStatusMonitor(result)
                                                          
#define fsciBleGapMonitorOutParams(functionId, ...)     if(gBleSuccess_c == result)                  \
                                                        {                                            \
                                                            FsciEvtMonitor(functionId, __VA_ARGS__); \
                                                        }    
/*! Handler functions for fsciBleGapGetConnectionEventFromBuffer */
static void GetConnEvtConnectedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetPairingParamsFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtPeripheralSecurityRequestFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtAuthenticationRejectedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtPasskeyDisplayFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtKeyExchangeRequestFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtKeysReceivedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtLongTermKeyRequestFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtEncryptionChangedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtPairingCompleteFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtDisconnectedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtRssiReadFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtTxPowerLevelReadFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtPowerReadFailureFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtParameterUpdateRequestFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtParameterUpdateCompleteFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtLeDataLengthChangedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtLeScDisplayNumericValueFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtLeScKeypressNotificationFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtChannelMapReadFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtChannelMapReadFailureFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtChannelMapReadFailureFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

#if defined(gBLE51_d) && (gBLE51_d == 1U)
static void GetConnEvtIqReportReceivedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtCteRequestFailedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);
#endif /* gBLE51_d */

#if defined(gBLE52_d) && (gBLE52_d == 1U)
static void GetConnEvtPathLossThresholdFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtTransmitPowerReportingFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetConnEvtEnhancedReadTransmitPowerLevelFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);
#endif /* gBLE52_d */

/*! Handler functions for fsciBleGapGetBufferFromConnectionEvent */
static void GetBufferFromConnEvtConnected
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromPairingParams
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtPeripheralSecurityRequest
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtAuthenticationRejected
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtPasskeyDisplay
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtKeyExchangeRequest
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtKeysReceived
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtLongTermKeyRequest
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtEncryptionChanged
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtPairingComplete
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtDisconnected
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtRssiRead
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtTxPowerLevelRead
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtPowerReadFailure
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtParameterUpdateRequest
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtParameterUpdateComplete
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtLeSetDataLengthFailure
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtLeDataLengthChanged
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtLeScDisplayNumericValue
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtLeScKeypressNotification
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtChannelMapRead
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtChannelMapReadFailure
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

#if defined(gBLE51_d) && (gBLE51_d == 1U)
static void GetBufferFromConnEvtIqReportReceived
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtCteRequestFailed
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);
#endif /* gBLE51_d */

#if defined(gBLE52_d) && (gBLE52_d == 1U)
static void GetBufferFromConnEvtPathLossThreshold
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtTransmitPowerReporting
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtEnhancedReadTransmitPowerLevel
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

#if defined(gEATT_d) && (gEATT_d == TRUE)
static void GetBufferFromConnEvtEattConnectionRequest
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtEattConnectionComplete
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtEattChannelReconfigureResponse
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

static void GetBufferFromConnEvtEattBearerStatusNotification
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);
#endif /* gEATT_d */
#endif /* gBLE52_d */

/*! Handler functions for fsciBleGapGetConnectionEventBufferSize */
static uint32_t GetConnEvtConnectedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetPairingParamsBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtPeripheralSecurityRequestBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtAuthenticationRejectedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtPasskeyDisplayBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtKeyExchangeRequestBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtKeysReceivedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtLongTermKeyRequestBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtEncryptionChangedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtPairingCompleteBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtDisconnectedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtRssiReadBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtTxPowerLevelReadBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtPowerReadFailureBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtParameterUpdateRequestBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtParameterUpdateCompleteBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtLeSetDataLengthFailureBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtLeDataLengthChangedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtLeScDisplayNumericValueBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtLeScKeypressNotificationBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtChannelMapReadBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtChannelMapReadFailureBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

#if defined(gBLE51_d) && (gBLE51_d == 1U)
static uint32_t GetConnEvtIqReportReceivedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtCteRequestFailedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);
#endif /* gBLE51_d */

#if defined(gBLE52_d) && (gBLE52_d == 1U)
static uint32_t GetConnEvtPathLossThresholdBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtTransmitPowerReportingBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtEnhancedReadTransmitPowerLevelBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);
#if defined(gEATT_d) && (gEATT_d == TRUE)
static uint32_t GetConnEvtEattConnectionRequestBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtEattConnectionCompleteBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtEattChannelReconfigureResponseBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);

static uint32_t GetConnEvtEattBearerStatusNotificationBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
);
#endif /* gEATT_d */
#endif /* gBLE52_d */

/*! Handler functions for fsciBleGapGetGenericEventFromBuffer */
static void GetInitializationCompleteEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetInternalErrorEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetAdvertisingSetupFailedEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetFilterAcceptListSizeReadEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetRandomAddressReadyEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetPublicAddressReadEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetAdvTxPowerLevelFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetPrivateResolvableAddressVerifiedFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetRandomAddressSetFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetLeScLocalOobDataEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetHostPrivacyStateChangedEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetControllerPrivacyStateChangedEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetTxPowerLevelSetCompleteEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetLePhyEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetControllerNotificationEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBondCreatedEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetTxEntryAvailableFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

#if defined(gBLE51_d) && (gBLE51_d == 1U)
static void GetConnectionlessIqSamplingStateChangedFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetAntennaInformationReadFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);
#endif /* gBLE51_d */

/*! Handler functions for fsciBleGapGetBufferFromGenericEvent */
static void GetBufferFromInitializationCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromInternalErrorEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromAdvertisingSetupFailedEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromFilterAcceptListSizeReadEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromRandomAddressReadyEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromPublicAddressReadEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromAdvTxPowerLevel
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromPrivateResolvableAddressVerified
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromRandomAddressSet
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromLeScLocalOobDataEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromHostPrivacyStateChangedEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromControllerPrivacyStateChangedEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromTxPowerLevelSetCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromLePhyEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromControllerNotificationEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromBondCreatedEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromGetConnParamsCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromTxEntryAvailable
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromControllerLocalRPAReadEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

#if defined(gBLE51_d) && (gBLE51_d == 1U)
static void GetBufferFromConnectionlessIqSamplingStateChanged
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromAntennaInformationRead
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromPeriodicAdvRecvEnableCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromPeriodicAdvSyncTransferComplete
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromPeriodicAdvSetInfoTransferCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromSetPeriodicAdvSyncTransferParamsCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromSetDefaultPeriodicAdvSyncTransferParamsCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromPerAdvSyncTransferReceived
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

static void GetBufferFromLeGenerateDhKeyCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);
#endif /* gBLE51_d */

static void GetBufferFromRemoteVersionInformationReadEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

/*! Handler functions for fsciBleGapGetGenericEventBufferSize */
static uint32_t GetInitializationCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetInternalErrorEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetAdvertisingSetupFailedEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetFilterAcceptListSizeReadEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetRandomAddressReadyEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetPublicAddressReadEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetAdvTxPowerLevelBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetPrivateResolvableAddressVerifiedBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetRandomAddressSetBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetLeScLocalOobDataEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetHostPrivacyStateChangedEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetControllerPrivacyStateChangedEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetTxPowerLevelSetCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetLePhyEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetControllerNotificationEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetBondCreatedEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetGetConnParamsCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetTxEntryAvailableBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetControllerLocalRPAReadEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

#if defined(gBLE51_d) && (gBLE51_d == 1U)
static uint32_t GetConnectionlessIqSamplingStateChangedBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetAntennaInformationReadBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetPeriodicAdvRecvEnableCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetPeriodicAdvSyncTransferCompleteBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetPeriodicAdvSetInfoTransferCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetSetPeriodicAdvSyncTransferParamsCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetSetDefaultPeriodicAdvSyncTransferParamsCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetPerAdvSyncTransferReceivedBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

static uint32_t GetLeGenerateDhKeyCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);
#endif /* gBLE51_d */

static uint32_t GetRemoteVersionInformationReadEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);


#if gFsciBleBBox_d || gFsciBleTest_d
void HandleGapCmdRegisterDeviceSecurityRequirementsOpCode(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetAdvertisingParametersOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#if defined(gBLE50_d) && (gBLE50_d == 1U)
void HandleGapCmdSetExtAdvertisingParametersOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetExtAdvertisingDataOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetPeriodicAdvDataOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#endif
#if defined(gBLE51_d) && (gBLE51_d == 1U)
void HandleGapCmdSetConnectionlessCteTransmitParametersOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEnableConnectionlessCteTransmitOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEnableConnectionlessIqSamplingOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetConnectionCteReceiveParametersOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetConnectionCteTransmitParametersOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEnableConnectionCteRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEnableConnectionCteResponseOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdReadAntennaInformationOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdGenerateDhKeyV2OpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdModifySleepClockAccuracy
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetPeriodicAdvRecvEnableOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetPeriodicAdvRecvDisableOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdPeriodicAdvSyncTransferOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdPeriodicAdvSetInfoTransferOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetPeriodicAdvSyncTransferParamsOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetDefaultPeriodicAdvSyncTransferParamsOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#endif /* gBLE51_d */
#if defined(gBLE52_d) && (gBLE52_d == 1U)
void HandleGapCmdEnhancedReadTransmitPowerLevelOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdReadRemoteTransmitPowerLevelOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetPathLossReportingParamsOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEnablePathLossReportingOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEnableTransmitPowerReportingOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#endif /* gBLE52_d */
void HandleGapCmdSetAdvertisingDataOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#if defined(gBLE50_d) && (gBLE50_d == 1U)
void HandleGapCmdUpdatePeriodicAdvListOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdPeriodicAdvTerminateSyncOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleCtrlCmdConfigureAdvCodingSchemeOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleCmdDisablePrivacyPerAdvSetOpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
#endif
void HandleCtrlCmdSetConnNotificationModeOpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
void HandleGapCmdStopAdvertisingOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#if defined(gBLE50_d) && (gBLE50_d == 1U)
void HandleGapCmdStopExtAdvertisingOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdRemoveAdvSetOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdStartPeriodicAdvOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdStopPeriodicAdvOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdPeriodicAdvCreateSyncOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#endif
void HandleGapCmdAuthorizeOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSaveCccdOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdCheckNotificationStatusOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdCheckIndicationStatusOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdPairOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEncryptLinkOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdAcceptPairingRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdRejectPairingOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEnterPasskeyOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdProvideOobOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdRejectPasskeyRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSendSmpKeysOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdRejectKeyExchangeRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdProvideLongTermKeyOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdDenyLongTermKeyOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdLoadEncryptionInformationOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);                        
void HandleGapCmdSetLocalPasskeyOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdStopScanningOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdDisconnectOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSaveCustomPeerInformationOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdLoadCustomPeerInformationOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdCheckIfBondedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdCheckNvmIndexOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdAdvIndexChangeOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdGetHostVersionOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdGetReadRemoteVersionInfoOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdGetConnParamsOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdGetDeviceIdFromConnHandleOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdGetConnectionHandleFromDeviceIdOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdReadFilterAcceptListSizeOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdClearFilterAcceptListOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdAddDeviceToFilterAcceptListOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdRemoveDeviceFromFilterAcceptListOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdReadPublicDeviceAddressOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdReadControllerLocalRPAOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdCreateRandomDeviceAddressOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

void HandleGapCmdSaveDeviceNameOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdGetBondedDevicesCountOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdGetBondedDeviceNameOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdRemoveBondOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdRemoveAllBondsOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdReadRadioPowerLevelOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdVerifyPrivateResolvableAddressOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdUpdateConnectionParametersOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

void HandleGapCmdEnableUpdateConnectionParametersOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdUpdateLeDataLengthOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEnableHostPrivacyOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEnableControllerPrivacyOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetPrivacyModeOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdLeScRegeneratePublicKeyOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdLeScValidateNumericValueOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdLeScGetLocalOobDataOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdLeScSetPeerOobDataOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdLeScSendKeypressNotificationPrivacyOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdGetBondedDevicesIdentityInformationOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetTxPowerLevelOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdLeReadPhyOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdLeSetPhyOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdControllerNotificationOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdLoadKeysOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSaveKeysOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetChannelMapOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdReadChannelMapOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#if defined(gBLE50_d) && (gBLE50_d == 1U)
void HandleGapCmdSetPeriodicAdvParametersOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#endif
#if (defined(CPU_MKW37A512VFT4) || defined(CPU_MKW37Z512VFT4) || defined(CPU_MKW38A512VFT4) || \
        defined(CPU_MKW38Z512VFT4) || defined(CPU_MKW39A512VFT4) || defined(CPU_MKW39Z512VFT4) || \
        defined(CPU_MKW35A512VFP4) || defined(CPU_MKW35Z512VHT4) || defined(CPU_MKW36A512VFP4) || \
        defined(CPU_MKW36A512VHT4) || defined(CPU_MKW36Z512VFP4) || defined(CPU_MKW36Z512VHT4))
void HandleCtrlCmdSetScanDupFiltModeOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#endif
void HandleGapCmdEattConnectionRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEattConnectionAcceptOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEattReconfigureRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapCmdEattSendCreditsRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

void HandleGapCmdEattDisconnectRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
void HandleGapStatusOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtCheckNotificationStatusOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtCheckIndicationStatusOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGetBondedDevicesIdentityInformationOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtLoadEncryptionInformationOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtLoadCustomPeerInformationOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtCheckIfBondedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtCheckNvmIndexOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGetBondedDevicesCountOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGetBondedDeviceNameOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventInitializationCompleteOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventInternalErrorOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventAdvertisingSetupFailedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventAdvertisingParametersSetupCompleteOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventAdvertisingDataSetupCompleteOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventFilterAcceptListSizeReadOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventDeviceAddedToFilterAcceptListOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventDeviceRemovedFromFilterAcceptListOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventFilterAcceptListClearedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventRandomAddressReadyOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventCreateConnectionCanceledOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventPublicAddressReadOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventAdvTxPowerLevelReadOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventPrivateResolvableAddressVerifiedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventRandomAddressSetOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventLeScPublicKeyRegeneratedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventLeScLocalOobDataOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventHostPrivacyStateChangedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventControllerPrivacyStateChangedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventTxPowerLevelSetCompleteOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventLePhyEventOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventControllerNotificationOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventBondCreatedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventChannelMapSetOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#if defined(gBLE50_d) && (gBLE50_d == 1U)
void HandleGapEvtGenericEventExtAdvertisingDataSetupCompleteOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtGenericEventPeriodicAdvCreateSyncCancelledOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#endif
void HandleGapEvtAdvertisingEventAdvertisingStateChangedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtAdvertisingEventAdvertisingCommandFailedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#if defined(gBLE50_d) && (gBLE50_d == 1U)
void HandleGapEvtAdvertisingEventAdvertisingSetTerminatedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#endif
void HandleGapEvtScanningEventScanStateChangedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtScanningEventScanCommandFailedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtScanningEventDeviceScannedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#if defined(gBLE50_d) && (gBLE50_d == 1U)
void HandleGapEvtScanningEventExtDeviceScannedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtScanningEventPeriodicDeviceScannedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#endif
void HandleGapEvtConnectionEventConnectedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventPairingRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventPeripheralSecurityRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventPairingResponseOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventAuthenticationRejectedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventPasskeyRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventOobRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventPasskeyDisplayOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventKeyExchangeRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventKeysReceivedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventLongTermKeyRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventEncryptionChangedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventPairingCompleteOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventDisconnectedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventRssiReadOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventTxPowerLevelReadOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventPowerReadFailureOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventParameterUpdateRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventParameterUpdateCompleteOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

void HandleGapEvtConnectionEventSetDataLenghtFailureOpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);

void HandleGapEvtConnectionEventLeDataLengthChangedOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventLeScOobDataRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventLeScDisplayNumericValueOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventLeScKeypressNotificationOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventChannelMapReadOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
void HandleGapEvtConnectionEventChannelMapReadFailureOpCode    
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#endif /* gFsciBleHost_d */
void HandleGapCmdLeSetSchedulerPriority
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

void HandleGapCmdLeSetHostFeature
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/*! Array of handler functions used by fsciBleGapGetConnectionEventFromBuffer */
const pfGapGetConnEventFromBufferHandler_t maGapGetConnEventFromBufferHandlers[] = 
{
    GetConnEvtConnectedFromBuffer,                      /* 0x00U, gConnEvtConnected_c */
    GetPairingParamsFromBuffer,                         /* 0x01U, gConnEvtPairingRequest_c */
    GetConnEvtPeripheralSecurityRequestFromBuffer,      /* 0x02U, gConnEvtPeripheralSecurityRequest_c */
    GetPairingParamsFromBuffer,                         /* 0x03U, gConnEvtPairingResponse_c */
    GetConnEvtAuthenticationRejectedFromBuffer,         /* 0x04U, gConnEvtAuthenticationRejected_c */
    NULL,                                               /* reserved: 0x05U */
    NULL,                                               /* reserved: 0x06U */
    GetConnEvtPasskeyDisplayFromBuffer,                 /* 0x07U, gConnEvtPasskeyDisplay_c */
    GetConnEvtKeyExchangeRequestFromBuffer,             /* 0x08U, gConnEvtKeyExchangeRequest_c */
    GetConnEvtKeysReceivedFromBuffer,                   /* 0x09U, gConnEvtKeysReceived_c */
    GetConnEvtLongTermKeyRequestFromBuffer,             /* 0x0AU, gConnEvtLongTermKeyRequest_c */
    GetConnEvtEncryptionChangedFromBuffer,              /* 0x0BU, gConnEvtEncryptionChanged_c */
    GetConnEvtPairingCompleteFromBuffer,                /* 0x0CU, gConnEvtPairingComplete_c */
    GetConnEvtDisconnectedFromBuffer,                   /* 0x0DU, gConnEvtDisconnected_c */
    GetConnEvtRssiReadFromBuffer,                       /* 0x0EU, gConnEvtRssiRead_c */
    GetConnEvtTxPowerLevelReadFromBuffer,               /* 0x0FU, gConnEvtTxPowerLevelRead_c */
    GetConnEvtPowerReadFailureFromBuffer,               /* 0x10U, gConnEvtPowerReadFailure_c */
    GetConnEvtParameterUpdateRequestFromBuffer,         /* 0x11U, gConnEvtParameterUpdateRequest_c */
    GetConnEvtParameterUpdateCompleteFromBuffer,        /* 0x12U, gConnEvtParameterUpdateComplete_c */
    GetConnEvtLeDataLengthChangedFromBuffer,            /* 0x13U, gConnEvtLeDataLengthChanged_c */
    NULL,                                               /* reserved: 0x14U */
    GetConnEvtLeScDisplayNumericValueFromBuffer,        /* 0x15U, gConnEvtLeScDisplayNumericValue_c */
    GetConnEvtLeScKeypressNotificationFromBuffer,       /* 0x16U, gConnEvtLeScKeypressNotification_c */
    GetConnEvtChannelMapReadFromBuffer,                 /* 0x17U, gConnEvtChannelMapRead_c */
    GetConnEvtChannelMapReadFailureFromBuffer,          /* 0x18U, gConnEvtChannelMapReadFailure_c */
    NULL,                                               /* reserved: 0x19U */
    NULL,                                               /* reserved: 0x1AU */
    NULL,                                               /* reserved: 0x1BU */
#if defined(gBLE51_d) && (gBLE51_d == 1U)
    GetConnEvtIqReportReceivedFromBuffer,               /* 0x1CU, gConnEvtIqReportReceived_c */
    GetConnEvtCteRequestFailedFromBuffer,               /* 0x1DU, gConnEvtCteRequestFailed_c */
#else /* gBLE51_d */
    NULL,                                               /* reserved: 0x1CU */
    NULL,                                               /* reserved: 0x1DU */
#endif /* gBLE51_d */
    NULL,                                               /* reserved: 0x1EU */
    NULL,                                               /* reserved: 0x1FU */
    NULL,                                               /* reserved: 0x20U */
    NULL,                                               /* reserved: 0x21U */
#if defined(gBLE52_d) && (gBLE52_d == 1U)
    GetConnEvtPathLossThresholdFromBuffer,              /* 0x22U, gConnEvtPathLossThreshold_c */
    GetConnEvtTransmitPowerReportingFromBuffer,         /* 0x23U, gConnEvtTransmitPowerReporting_c */
    GetConnEvtEnhancedReadTransmitPowerLevelFromBuffer, /* 0x24U, gConnEvtEnhancedReadTransmitPowerLevel_c */
    NULL,                                               /* reserved: 0x25U */
    NULL,                                               /* reserved: 0x26U */
    NULL,                                               /* reserved: 0x27U */
    NULL,                                               /* reserved: 0x28U */
    NULL,                                               /* reserved: 0x29U */
    NULL,                                               /* reserved: 0x2AU */
    NULL,                                               /* reserved: 0x2BU */
#else /* gBLE52_d */
    NULL,                                               /* reserved: 0x22U */
    NULL,                                               /* reserved: 0x23U */
    NULL,                                               /* reserved: 0x24U */
    NULL,                                               /* reserved: 0x25U */
    NULL,                                               /* reserved: 0x26U */
    NULL,                                               /* reserved: 0x27U */
    NULL,                                               /* reserved: 0x28U */
    NULL,                                               /* reserved: 0x29U */
    NULL,                                               /* reserved: 0x2AU */
    NULL,                                               /* reserved: 0x2BU */
#endif /* gBLE52_d */
};

/*! Array of handler functions used by fsciBleGapGetBufferFromConnectionEvent */
const pfGapGetBufferFromConnEventHandler_t maGapGetBufferFromConnEventHandlers[] =
{
    GetBufferFromConnEvtConnected,                      /* 0x00U, gConnEvtConnected_c */
    GetBufferFromPairingParams,                         /* 0x01U, gConnEvtPairingRequest_c */
    GetBufferFromConnEvtPeripheralSecurityRequest,      /* 0x02U, gConnEvtPeripheralSecurityRequest_c */
    GetBufferFromPairingParams,                         /* 0x03U, gConnEvtPairingResponse_c */
    GetBufferFromConnEvtAuthenticationRejected,         /* 0x04U, gConnEvtAuthenticationRejected_c */
    NULL,                                               /* reserved: 0x05U */
    NULL,                                               /* reserved: 0x06U */
    GetBufferFromConnEvtPasskeyDisplay,                 /* 0x07U, gConnEvtPasskeyDisplay_c */
    GetBufferFromConnEvtKeyExchangeRequest,             /* 0x08U, gConnEvtKeyExchangeRequest_c */
    GetBufferFromConnEvtKeysReceived,                   /* 0x09U, gConnEvtKeysReceived_c */
    GetBufferFromConnEvtLongTermKeyRequest,             /* 0x0AU, gConnEvtLongTermKeyRequest_c */
    GetBufferFromConnEvtEncryptionChanged,              /* 0x0BU, gConnEvtEncryptionChanged_c */
    GetBufferFromConnEvtPairingComplete,                /* 0x0CU, gConnEvtPairingComplete_c */
    GetBufferFromConnEvtDisconnected,                   /* 0x0DU, gConnEvtDisconnected_c */
    GetBufferFromConnEvtRssiRead,                       /* 0x0EU, gConnEvtRssiRead_c */
    GetBufferFromConnEvtTxPowerLevelRead,               /* 0x0FU, gConnEvtTxPowerLevelRead_c */
    GetBufferFromConnEvtPowerReadFailure,               /* 0x10U, gConnEvtPowerReadFailure_c */
    GetBufferFromConnEvtParameterUpdateRequest,         /* 0x11U, gConnEvtParameterUpdateRequest_c */
    GetBufferFromConnEvtParameterUpdateComplete,        /* 0x12U, gConnEvtParameterUpdateComplete_c */
    GetBufferFromConnEvtLeDataLengthChanged,            /* 0x13U, gConnEvtLeDataLengthChanged_c */
    NULL,                                               /* reserved: 0x14U */
    GetBufferFromConnEvtLeScDisplayNumericValue,        /* 0x15U, gConnEvtLeScDisplayNumericValue_c */
    GetBufferFromConnEvtLeScKeypressNotification,       /* 0x16U, gConnEvtLeScKeypressNotification_c */
    GetBufferFromConnEvtChannelMapRead,                 /* 0x17U, gConnEvtChannelMapRead_c */
    GetBufferFromConnEvtChannelMapReadFailure,          /* 0x18U, gConnEvtChannelMapReadFailure_c */
    NULL,                                               /* reserved: 0x19U */
    NULL,                                               /* reserved: 0x1AU */
    NULL,                                               /* reserved: 0x1BU */
#if defined(gBLE51_d) && (gBLE51_d == 1U)
    GetBufferFromConnEvtIqReportReceived,               /* 0x1CU, gConnEvtIqReportReceived_c */
    GetBufferFromConnEvtCteRequestFailed,               /* 0x1DU, gConnEvtCteRequestFailed_c */
#else /* gBLE51_d */
    NULL,                                               /* reserved: 0x1CU */
    NULL,                                               /* reserved: 0x1DU */
#endif /* gBLE51_d */
    NULL,                                               /* reserved: 0x1EU */
    NULL,                                               /* reserved: 0x1FU */
    NULL,                                               /* reserved: 0x20U */
    NULL,                                               /* reserved: 0x21U */
#if defined(gBLE52_d) && (gBLE52_d == 1U)
    GetBufferFromConnEvtPathLossThreshold,              /* 0x22U, gConnEvtPathLossThreshold_c */
    GetBufferFromConnEvtTransmitPowerReporting,         /* 0x23U, gConnEvtTransmitPowerReporting_c */
    GetBufferFromConnEvtEnhancedReadTransmitPowerLevel, /* 0x24U, gConnEvtEnhancedReadTransmitPowerLevel_c */
    NULL,                                               /* reserved: 0x25U */
    NULL,                                               /* reserved: 0x26U */
    NULL,                                               /* reserved: 0x27U */
#if defined(gEATT_d) && (gEATT_d == TRUE)
    GetBufferFromConnEvtEattConnectionRequest,          /* 0x28U, gConnEvtEattConnectionRequest_c */
    GetBufferFromConnEvtEattConnectionComplete,         /* 0x29U, gConnEvtEattConnectionComplete_c */
    GetBufferFromConnEvtEattChannelReconfigureResponse, /* 0x2AU, gConnEvtEattChannelReconfigureResponse_c */
    GetBufferFromConnEvtEattBearerStatusNotification,   /* 0x2BU, gConnEvtEattBearerStatusNotification_c */
#else /* gEATT_d */
    NULL,                                               /* reserved: 0x28U */
    NULL,                                               /* reserved: 0x29U */
    NULL,                                               /* reserved: 0x2AU */
    NULL,                                               /* reserved: 0x2BU */
#endif /* gEATT_d */
#else /* gBLE52_d */
    NULL,                                               /* reserved: 0x22U */
    NULL,                                               /* reserved: 0x23U */
    NULL,                                               /* reserved: 0x24U */
    NULL,                                               /* reserved: 0x25U */
    NULL,                                               /* reserved: 0x26U */
    NULL,                                               /* reserved: 0x27U */
    NULL,                                               /* reserved: 0x28U */
    NULL,                                               /* reserved: 0x29U */
    NULL,                                               /* reserved: 0x2AU */
    NULL,                                               /* reserved: 0x2BU */
#endif /* gBLE52_d */
    NULL,                                               /* 0x2CU gConnEvtHandoverConnected_c */
    NULL,                                               /* 0x2DU gHandoverDisconnected_c */
    GetBufferFromConnEvtLeSetDataLengthFailure,         /* 0x2EU, gConnEvtLeSetDataLengthFailure_c */
};

/*! Array of handler functions used by fsciBleGapGetConnectionEventBufferSize */
const pfGapGetConnEventBufferSizeHandler_t maGapGetConnEventBufferSizeHandlers[] = 
{
    GetConnEvtConnectedBufferSize,                      /* 0x00U, gConnEvtConnected_c */
    GetPairingParamsBufferSize,                         /* 0x01U, gConnEvtPairingRequest_c */
    GetConnEvtPeripheralSecurityRequestBufferSize,      /* 0x02U, gConnEvtPeripheralSecurityRequest_c */
    GetPairingParamsBufferSize,                         /* 0x03U, gConnEvtPairingResponse_c */
    GetConnEvtAuthenticationRejectedBufferSize,         /* 0x04U, gConnEvtAuthenticationRejected_c */
    NULL,                                               /* reserved: 0x05U */
    NULL,                                               /* reserved: 0x06U */
    GetConnEvtPasskeyDisplayBufferSize,                 /* 0x07U, gConnEvtPasskeyDisplay_c */
    GetConnEvtKeyExchangeRequestBufferSize,             /* 0x08U, gConnEvtKeyExchangeRequest_c */
    GetConnEvtKeysReceivedBufferSize,                   /* 0x09U, gConnEvtKeysReceived_c */
    GetConnEvtLongTermKeyRequestBufferSize,             /* 0x0AU, gConnEvtLongTermKeyRequest_c */
    GetConnEvtEncryptionChangedBufferSize,              /* 0x0BU, gConnEvtEncryptionChanged_c */
    GetConnEvtPairingCompleteBufferSize,                /* 0x0CU, gConnEvtPairingComplete_c */
    GetConnEvtDisconnectedBufferSize,                   /* 0x0DU, gConnEvtDisconnected_c */
    GetConnEvtRssiReadBufferSize,                       /* 0x0EU, gConnEvtRssiRead_c */
    GetConnEvtTxPowerLevelReadBufferSize,               /* 0x0FU, gConnEvtTxPowerLevelRead_c */
    GetConnEvtPowerReadFailureBufferSize,               /* 0x10U, gConnEvtPowerReadFailure_c */
    GetConnEvtParameterUpdateRequestBufferSize,         /* 0x11U, gConnEvtParameterUpdateRequest_c */
    GetConnEvtParameterUpdateCompleteBufferSize,        /* 0x12U, gConnEvtParameterUpdateComplete_c */
    GetConnEvtLeDataLengthChangedBufferSize,            /* 0x13U, gConnEvtLeDataLengthChanged_c */
    NULL,                                               /* reserved: 0x14U */
    GetConnEvtLeScDisplayNumericValueBufferSize,        /* 0x15U, gConnEvtLeScDisplayNumericValue_c */
    GetConnEvtLeScKeypressNotificationBufferSize,       /* 0x16U, gConnEvtLeScKeypressNotification_c */
    GetConnEvtChannelMapReadBufferSize,                 /* 0x17U, gConnEvtChannelMapRead_c */
    GetConnEvtChannelMapReadFailureBufferSize,          /* 0x18U, gConnEvtChannelMapReadFailure_c */
    NULL,                                               /* reserved: 0x19U */
    NULL,                                               /* reserved: 0x1AU */
    NULL,                                               /* reserved: 0x1BU */
#if defined(gBLE51_d) && (gBLE51_d == 1U)
    GetConnEvtIqReportReceivedBufferSize,               /* 0x1CU, gConnEvtIqReportReceived_c */
    GetConnEvtCteRequestFailedBufferSize,               /* 0x1DU, gConnEvtCteRequestFailed_c */
#else /* gBLE51_d */
    NULL,                                               /* reserved: 0x1CU */
    NULL,                                               /* reserved: 0x1DU */
#endif /* gBLE51_d */
    NULL,                                               /* reserved: 0x1EU */
    NULL,                                               /* reserved: 0x1FU */
    NULL,                                               /* reserved: 0x20U */
    NULL,                                               /* reserved: 0x21U */
#if defined(gBLE52_d) && (gBLE52_d == 1U)
    GetConnEvtPathLossThresholdBufferSize,              /* 0x22U, gConnEvtPathLossThreshold_c */
    GetConnEvtTransmitPowerReportingBufferSize,         /* 0x23U, gConnEvtTransmitPowerReporting_c */
    GetConnEvtEnhancedReadTransmitPowerLevelBufferSize, /* 0x24U, gConnEvtEnhancedReadTransmitPowerLevel_c */
    NULL,                                               /* reserved: 0x25U */
    NULL,                                               /* reserved: 0x26U */
    NULL,                                               /* reserved: 0x27U */
#if defined(gEATT_d) && (gEATT_d == TRUE)
    GetConnEvtEattConnectionRequestBufferSize,          /* 0x28U, gConnEvtEattConnectionRequest_c */
    GetConnEvtEattConnectionCompleteBufferSize,         /* 0x29U, gConnEvtEattConnectionComplete_c */
    GetConnEvtEattChannelReconfigureResponseBufferSize, /* 0x2AU, gConnEvtEattChannelReconfigureResponse_c */
    GetConnEvtEattBearerStatusNotificationBufferSize,   /* 0x2BU, gConnEvtEattBearerStatusNotification_c */
#else /* gEATT_d */
    NULL,                                               /* reserved: 0x28U */
    NULL,                                               /* reserved: 0x29U */
    NULL,                                               /* reserved: 0x2AU */
    NULL,                                               /* reserved: 0x2BU */
#endif /* gEATT_d */
#else /* gBLE52_d */
    NULL,                                               /* reserved: 0x22U */
    NULL,                                               /* reserved: 0x23U */
    NULL,                                               /* reserved: 0x24U */
    NULL,                                               /* reserved: 0x25U */
    NULL,                                               /* reserved: 0x26U */
    NULL,                                               /* reserved: 0x27U */
    NULL,                                               /* reserved: 0x28U */
    NULL,                                               /* reserved: 0x29U */
    NULL,                                               /* reserved: 0x2AU */
    NULL,                                               /* reserved: 0x2BU */
#endif /* gBLE52_d */
    NULL,                                               /* 0x2CU gConnEvtHandoverConnected_c */
    NULL,                                               /* 0x2DU gHandoverDisconnected_c */
    GetConnEvtLeSetDataLengthFailureBufferSize,         /* 0x2EU, gConnEvtLeSetDataLengthFailure_c */
};

/*! Array of handler functions used by fsciBleGapGetGenericEventFromBuffer */
const pfGapGetGenericEventFromBufferHandler_t maGapGetGenericEventFromBufferHandlers[] =
{
    GetInitializationCompleteEventFromBuffer,                   /* 0x00U, gInitializationComplete_c */
    GetInternalErrorEventFromBuffer,                            /* 0x01U, gInternalError_c */
    GetAdvertisingSetupFailedEventFromBuffer,                   /* 0x02U, gAdvertisingSetupFailed_c */
    NULL,                                                       /* reserved: 0x03U */
    NULL,                                                       /* reserved: 0x04U */
    GetFilterAcceptListSizeReadEventFromBuffer,                 /* 0x05U, gFilterAcceptListSizeRead_c */
    NULL,                                                       /* reserved: 0x06U  */
    NULL,                                                       /* reserved: 0x07U  */
    NULL,                                                       /* reserved: 0x08U  */
    GetRandomAddressReadyEventFromBuffer,                       /* 0x09U, gRandomAddressReady_c */
    NULL,                                                       /* reserved: 0x0AU */
    GetPublicAddressReadEventFromBuffer,                        /* 0x0BU, gPublicAddressRead_c */
    GetAdvTxPowerLevelFromBuffer,                               /* 0x0CU, gAdvTxPowerLevelRead_c */
    GetPrivateResolvableAddressVerifiedFromBuffer,              /* 0x0DU, gPrivateResolvableAddressVerified_c */
    GetRandomAddressSetFromBuffer,                              /* 0x0EU, gRandomAddressSet_c */
    NULL,                                                       /* reserved: 0x0FU */
    GetLeScLocalOobDataEventFromBuffer,                         /* 0x10U, gLeScLocalOobData_c */
    GetHostPrivacyStateChangedEventFromBuffer,                  /* 0x11U, gHostPrivacyStateChanged_c */
    GetControllerPrivacyStateChangedEventFromBuffer,            /* 0x12U, gControllerPrivacyStateChanged_c */
    NULL,                                                       /* reserved: 0x13U */
    GetTxPowerLevelSetCompleteEventFromBuffer,                  /* 0x14U, gTxPowerLevelSetComplete_c */
    GetLePhyEventFromBuffer,                                    /* 0x15U, gLePhyEvent_c */
    GetControllerNotificationEventFromBuffer,                   /* 0x16U, gControllerNotificationEvent_c */
    NULL,                                                       /* reserved: 0x17U */
    GetBondCreatedEventFromBuffer,                              /* 0x18U, gBondCreatedEvent_c */
    NULL,                                                       /* reserved: 0x19U */
#if defined(gBLE50_d) && (gBLE50_d == 1U)
    GetAdvTxPowerLevelFromBuffer,                               /* 0x1AU, gExtAdvertisingParametersSetupComplete_c */
#else /* gBLE50_d */
    NULL,                                                       /* reserved: 0x1AU */
#endif /* gBLE50_d */
    NULL,                                                       /* reserved: 0x1BU */
    NULL,                                                       /* reserved: 0x1CU */
    NULL,                                                       /* reserved: 0x1DU */
    NULL,                                                       /* reserved: 0x1EU */
    NULL,                                                       /* reserved: 0x1FU */
    NULL,                                                       /* reserved: 0x20U */
    NULL,                                                       /* reserved: 0x21U */
    GetTxEntryAvailableFromBuffer,                              /* 0x22U, gTxEntryAvailable_c */
    NULL,                                                       /* reserved: 0x23U */
    NULL,                                                       /* reserved: 0x24U */
    NULL,                                                       /* reserved: 0x25U */
#if defined(gBLE51_d) && (gBLE51_d == 1U)
    GetConnectionlessIqSamplingStateChangedFromBuffer,          /* 0x26U, gConnectionlessIqSamplingStateChanged_c */
    GetAntennaInformationReadFromBuffer,                        /* 0x27U, gAntennaInformationRead_c */
#else /* gBLE51_d */
    NULL,                                                       /* reserved: 0x26U */
    NULL,                                                       /* reserved: 0x27U */
#endif /*  gBLE51_d */
};

/*! Array of handler functions used by fsciBleGapGetBufferFromGenericEvent */
const pfGapGetBufferFromGenericEventHandler_t maGapGetBufferFromGenericEventHandlers[] =
{
    GetBufferFromInitializationCompleteEvent,                                   /* 0x00U, gInitializationComplete_c */
    GetBufferFromInternalErrorEvent,                                            /* 0x01U, gInternalError_c */
    GetBufferFromAdvertisingSetupFailedEvent,                                   /* 0x02U, gAdvertisingSetupFailed_c */
    NULL,                                                                       /* reserved: 0x03U */
    NULL,                                                                       /* reserved: 0x04U */
    GetBufferFromFilterAcceptListSizeReadEvent,                                 /* 0x05U, gFilterAcceptListSizeRead_c */
    NULL,                                                                       /* reserved: 0x06U  */
    NULL,                                                                       /* reserved: 0x07U  */
    NULL,                                                                       /* reserved: 0x08U  */
    GetBufferFromRandomAddressReadyEvent,                                       /* 0x09U, gRandomAddressReady_c */
    NULL,                                                                       /* reserved: 0x0AU */
    GetBufferFromPublicAddressReadEvent,                                        /* 0x0BU, gPublicAddressRead_c */
    GetBufferFromAdvTxPowerLevel,                                               /* 0x0CU, gAdvTxPowerLevelRead_c */
    GetBufferFromPrivateResolvableAddressVerified,                              /* 0x0DU, gPrivateResolvableAddressVerified_c */
    GetBufferFromRandomAddressSet,                                              /* 0x0EU, gRandomAddressSet_c */
    NULL,                                                                       /* reserved: 0x0FU */
    GetBufferFromLeScLocalOobDataEvent,                                         /* 0x10U, gLeScLocalOobData_c */
    GetBufferFromHostPrivacyStateChangedEvent,                                  /* 0x11U, gHostPrivacyStateChanged_c */
    GetBufferFromControllerPrivacyStateChangedEvent,                            /* 0x12U, gControllerPrivacyStateChanged_c */
    NULL,                                                                       /* reserved: 0x13U */
    GetBufferFromTxPowerLevelSetCompleteEvent,                                  /* 0x14U, gTxPowerLevelSetComplete_c */
    GetBufferFromLePhyEvent,                                                    /* 0x15U, gLePhyEvent_c */
    GetBufferFromControllerNotificationEvent,                                   /* 0x16U, gControllerNotificationEvent_c */
    GetBufferFromGetConnParamsCompleteEvent,                                    /* 0x17U, gGetConnParamsComplete_c */
    GetBufferFromBondCreatedEvent,                                              /* 0x18U, gBondCreatedEvent_c */
    NULL,                                                                       /* reserved: 0x19U */
#if defined(gBLE50_d) && (gBLE50_d == 1U)
    GetBufferFromAdvTxPowerLevel,                                               /* 0x1AU, gExtAdvertisingParametersSetupComplete_c */
#else /* gBLE50_d */
    NULL,                                                                       /* reserved: 0x1AU */
#endif /* gBLE50_d */
    NULL,                                                                       /* reserved: 0x1BU */
    NULL,                                                                       /* reserved: 0x1CU */
    NULL,                                                                       /* reserved: 0x1DU */
    NULL,                                                                       /* reserved: 0x1EU */
    NULL,                                                                       /* reserved: 0x1FU */
    NULL,                                                                       /* reserved: 0x20U */
    NULL,                                                                       /* reserved: 0x21U */
    GetBufferFromTxEntryAvailable,                                              /* 0x22U, gTxEntryAvailable_c */
    GetBufferFromControllerLocalRPAReadEvent,                                   /* 0x23U, gControllerLocalRPARead_c */
    NULL,                                                                       /* reserved: 0x24U */
    NULL,                                                                       /* reserved: 0x25U */
#if defined(gBLE51_d) && (gBLE51_d == 1U)
    GetBufferFromConnectionlessIqSamplingStateChanged,                          /* 0x26U, gConnectionlessIqSamplingStateChanged_c */
    GetBufferFromAntennaInformationRead,                                        /* 0x27U, gAntennaInformationRead_c */
    NULL,                                                                       /* reserved: 0x28U */
    GetBufferFromPeriodicAdvRecvEnableCompleteEvent,                            /* 0x29U, gPeriodicAdvRecvEnableComplete_c */
    GetBufferFromPeriodicAdvSyncTransferComplete,                               /* 0x2AU, gPeriodicAdvSyncTransferComplete_c */
    GetBufferFromPeriodicAdvSetInfoTransferCompleteEvent,                       /* 0x2BU, gPeriodicAdvSetInfoTransferComplete_c */
    GetBufferFromSetPeriodicAdvSyncTransferParamsCompleteEvent,                 /* 0x2CU, gSetPeriodicAdvSyncTransferParamsComplete_c */
    GetBufferFromSetDefaultPeriodicAdvSyncTransferParamsCompleteEvent,          /* 0x2DU, gSetDefaultPeriodicAdvSyncTransferParamsComplete_c */
    GetBufferFromPerAdvSyncTransferReceived,                                    /* 0x2EU, gPeriodicAdvSyncTransferSucceeded_c */
    GetBufferFromPerAdvSyncTransferReceived,                                    /* 0x2FU, gPeriodicAdvSyncTransferFailed_c */
    GetBufferFromLeGenerateDhKeyCompleteEvent,                                  /* 0x30U, gConnEvtLeGenerateDhKeyComplete_c */
#else /* gBLE51_d */
    NULL,                                                                       /* reserved: 0x26U */
    NULL,                                                                       /* reserved: 0x27U */
    NULL,                                                                       /* reserved: 0x28U */
    NULL,                                                                       /* reserved: 0x29U */
    NULL,                                                                       /* reserved: 0x2AU */
    NULL,                                                                       /* reserved: 0x2BU */
    NULL,                                                                       /* reserved: 0x2CU */
    NULL,                                                                       /* reserved: 0x2DU */
    NULL,                                                                       /* resreved: 0x2EU */
    NULL,                                                                       /* reserved: 0x2FU */
    NULL,                                                                       /* reserved: 0x30U */
#endif /* gBLE51_d */
    NULL,                                                                       /* reserved: 0x31U */
    NULL,                                                                       /* reserved: 0x32U */
    NULL,                                                                       /* reserved: 0x33U */
    NULL,                                                                       /* reserved: 0x34U */
    NULL,                                                                       /* reserved: 0x35U */
    NULL,                                                                       /* reserved: 0x36U */
    NULL,                                                                       /* reserved: 0x37U */
    NULL,                                                                       /* reserved: 0x38U */
    NULL,                                                                       /* resreved: 0x39U */
    NULL,                                                                       /* reserved: 0x3AU */
    NULL,                                                                       /* reserved: 0x3BU */
    NULL,                                                                       /* reserved: 0x3CU */
    GetBufferFromRemoteVersionInformationReadEvent                              /* 0x3DU, gRemoteVersionInformationRead_c */
};

/*! Array of handler functions used by fsciBleGapGetGenericEventBufferSize */
const pfGapGetGenericEventBufferSizeHandler_t maGapGetGenericEventBufferSizeHandlers[] =
{
    GetInitializationCompleteEventBufferSize,                                   /* 0x00U, gInitializationComplete_c */
    GetInternalErrorEventBufferSize,                                            /* 0x01U, gInternalError_c */
    GetAdvertisingSetupFailedEventBufferSize,                                   /* 0x02U, gAdvertisingSetupFailed_c */
    NULL,                                                                       /* reserved: 0x03U */
    NULL,                                                                       /* reserved: 0x04U */
    GetFilterAcceptListSizeReadEventBufferSize,                                 /* 0x05U, gFilterAcceptListSizeRead_c */
    NULL,                                                                       /* reserved: 0x06U  */
    NULL,                                                                       /* reserved: 0x07U  */
    NULL,                                                                       /* reserved: 0x08U  */
    GetRandomAddressReadyEventBufferSize,                                       /* 0x09U, gRandomAddressReady_c */
    NULL,                                                                       /* reserved: 0x0AU */
    GetPublicAddressReadEventBufferSize,                                        /* 0x0BU, gPublicAddressRead_c */
    GetAdvTxPowerLevelBufferSize,                                               /* 0x0CU, gAdvTxPowerLevelRead_c */
    GetPrivateResolvableAddressVerifiedBufferSize,                              /* 0x0DU, gPrivateResolvableAddressVerified_c */
    GetRandomAddressSetBufferSize,                                              /* 0x0EU, gRandomAddressSet_c */
    NULL,                                                                       /* reserved: 0x0FU */
    GetLeScLocalOobDataEventBufferSize,                                         /* 0x10U, gLeScLocalOobData_c */
    GetHostPrivacyStateChangedEventBufferSize,                                  /* 0x11U, gHostPrivacyStateChanged_c */
    GetControllerPrivacyStateChangedEventBufferSize,                            /* 0x12U, gControllerPrivacyStateChanged_c */
    NULL,                                                                       /* reserved: 0x13U */
    GetTxPowerLevelSetCompleteEventBufferSize,                                  /* 0x14U, gTxPowerLevelSetComplete_c */
    GetLePhyEventBufferSize,                                                    /* 0x15U, gLePhyEvent_c */
    GetControllerNotificationEventBufferSize,                                   /* 0x16U, gControllerNotificationEvent_c */
    GetGetConnParamsCompleteEventBufferSize,                                    /* 0x17U, gGetConnParamsComplete_c */
    GetBondCreatedEventBufferSize,                                              /* 0x18U, gBondCreatedEvent_c */
    NULL,                                                                       /* reserved: 0x19U */
#if defined(gBLE50_d) && (gBLE50_d == 1U)
    GetAdvTxPowerLevelBufferSize,                                               /* 0x1AU, gExtAdvertisingParametersSetupComplete_c */
#else /* gBLE50_d */
    NULL,                                                                       /* reserved: 0x1AU */
#endif /* gBLE50_d */
    NULL,                                                                       /* reserved: 0x1BU */
    NULL,                                                                       /* reserved: 0x1CU */
    NULL,                                                                       /* reserved: 0x1DU */
    NULL,                                                                       /* reserved: 0x1EU */
    NULL,                                                                       /* reserved: 0x1FU */
    NULL,                                                                       /* reserved: 0x20U */
    NULL,                                                                       /* reserved: 0x21U */
    GetTxEntryAvailableBufferSize,                                              /* 0x22U, gTxEntryAvailable_c */
    GetControllerLocalRPAReadEventBufferSize,                                   /* 0x23U, gControllerLocalRPARead_c */
    NULL,                                                                       /* reserved: 0x24U */
    NULL,                                                                       /* reserved: 0x25U */
#if defined(gBLE51_d) && (gBLE51_d == 1U)
    GetConnectionlessIqSamplingStateChangedBufferSize,                          /* 0x26U, gConnectionlessIqSamplingStateChanged_c */
    GetAntennaInformationReadBufferSize,                                        /* 0x27U, gAntennaInformationRead_c */
    NULL,                                                                       /* reserved: 0x28U */
    GetPeriodicAdvRecvEnableCompleteEventBufferSize,                            /* 0x29U, gPeriodicAdvRecvEnableComplete_c */
    GetPeriodicAdvSyncTransferCompleteBufferSize,                               /* 0x2AU, gPeriodicAdvSyncTransferComplete_c */
    GetPeriodicAdvSetInfoTransferCompleteEventBufferSize,                       /* 0x2BU, gPeriodicAdvSetInfoTransferComplete_c */
    GetSetPeriodicAdvSyncTransferParamsCompleteEventBufferSize,                 /* 0x2CU, gSetPeriodicAdvSyncTransferParamsComplete_c */
    GetSetDefaultPeriodicAdvSyncTransferParamsCompleteEventBufferSize,          /* 0x2DU, gSetDefaultPeriodicAdvSyncTransferParamsComplete_c */
    GetPerAdvSyncTransferReceivedBufferSize,                                    /* 0x2EU, gPeriodicAdvSyncTransferSucceeded_c */
    GetPerAdvSyncTransferReceivedBufferSize,                                    /* 0x2FU, gPeriodicAdvSyncTransferFailed_c */
    GetLeGenerateDhKeyCompleteEventBufferSize,                                  /* 0x30U, gConnEvtLeGenerateDhKeyComplete_c */
#else /* gBLE51_d */
    NULL,                                                                       /* reserved: 0x26U */
    NULL,                                                                       /* reserved: 0x27U */
    NULL,                                                                       /* reserved: 0x28U */
    NULL,                                                                       /* reserved: 0x29U */
    NULL,                                                                       /* reserved: 0x2AU */
    NULL,                                                                       /* reserved: 0x2BU */
    NULL,                                                                       /* reserved: 0x2CU */
    NULL,                                                                       /* reserved: 0x2DU */
    NULL,                                                                       /* resreved: 0x2EU */
    NULL,                                                                       /* reserved: 0x2FU */
    NULL,                                                                       /* reserved: 0x30U */
#endif /* gBLE51_d */
    NULL,                                                                       /* reserved: 0x31U */
    NULL,                                                                       /* reserved: 0x32U */
    NULL,                                                                       /* reserved: 0x33U */
    NULL,                                                                       /* reserved: 0x34U */
    NULL,                                                                       /* reserved: 0x35U */
    NULL,                                                                       /* reserved: 0x36U */
    NULL,                                                                       /* reserved: 0x37U */
    NULL,                                                                       /* reserved: 0x38U */
    NULL,                                                                       /* resreved: 0x39U */
    NULL,                                                                       /* reserved: 0x3AU */
    NULL,                                                                       /* reserved: 0x3BU */
    NULL,                                                                       /* reserved: 0x3CU */
    GetRemoteVersionInformationReadEventBufferSize,                             /* 0x3DU, gRemoteVersionInformationRead_c */
    NULL                                                                        /* reserved: 0x3EU */
};

#if gFsciBleBBox_d || gFsciBleTest_d
const pfGapOpCodeHandler_t maGapCmdOpCodeHandlers[]=
{
    NULL,                                                                             /* reserved: 0x00 */
    HandleGapCmdBleHostInitializeOpCode,                                              /* = 0x01, gBleGapCmdBleHostInitializeOpCode_c */
    HandleGapCmdRegisterDeviceSecurityRequirementsOpCode,                             /* = 0x02, gBleGapCmdRegisterDeviceSecurityRequirementsOpCode_c */
    HandleGapCmdSetAdvertisingParametersOpCode,                                       /* = 0x03, gBleGapCmdSetAdvertisingParametersOpCode_c */
    HandleGapCmdSetAdvertisingDataOpCode,                                             /* = 0x04, gBleGapCmdSetAdvertisingDataOpCode_c */
    HandleGapCmdStartAdvertisingOpCode,                                               /* = 0x05, gBleGapCmdStartAdvertisingOpCode_c */
    HandleGapCmdStopAdvertisingOpCode,                                                /* = 0x06, gBleGapCmdStopAdvertisingOpCode_c */
    HandleGapCmdAuthorizeOpCode,                                                      /* = 0x07, gBleGapCmdAuthorizeOpCode_c */
    HandleGapCmdSaveCccdOpCode,                                                       /* = 0x08, gBleGapCmdSaveCccdOpCode_c */
    HandleGapCmdCheckNotificationStatusOpCode,                                        /* = 0x09, gBleGapCmdCheckNotificationStatusOpCode_c */
    HandleGapCmdCheckIndicationStatusOpCode,                                          /* = 0x0A, gBleGapCmdCheckIndicationStatusOpCode_c */
    NULL,                                                                             /* reserved: 0x0B */
    HandleGapCmdPairOpCode,                                                           /* = 0x0C, gBleGapCmdPairOpCode_c */
    HandleGapCmdSendPeripheralSecurityRequestOpCode,                                  /* = 0x0D, gBleGapCmdSendPeripheralSecurityRequestOpCode_c */
    HandleGapCmdEncryptLinkOpCode,                                                    /* = 0x0E, gBleGapCmdEncryptLinkOpCode_c */
    HandleGapCmdAcceptPairingRequestOpCode,                                           /* = 0x0F, gBleGapCmdAcceptPairingRequestOpCode_c */
    HandleGapCmdRejectPairingOpCode,                                                  /* = 0x10, gBleGapCmdRejectPairingOpCode_c */
    HandleGapCmdEnterPasskeyOpCode,                                                   /* = 0x11, gBleGapCmdEnterPasskeyOpCode_c */
    HandleGapCmdProvideOobOpCode,                                                     /* = 0x12, gBleGapCmdProvideOobOpCode_c */
    HandleGapCmdRejectPasskeyRequestOpCode,                                           /* = 0x13, gBleGapCmdRejectPasskeyRequestOpCode_c */
    HandleGapCmdSendSmpKeysOpCode,                                                    /* = 0x14, gBleGapCmdSendSmpKeysOpCode_c */
    HandleGapCmdRejectKeyExchangeRequestOpCode,                                       /* = 0x15, gBleGapCmdRejectKeyExchangeRequestOpCode_c */
    HandleGapCmdProvideLongTermKeyOpCode,                                             /* = 0x16, gBleGapCmdProvideLongTermKeyOpCode_c */
    HandleGapCmdDenyLongTermKeyOpCode,                                                /* = 0x17, gBleGapCmdDenyLongTermKeyOpCode_c */
    HandleGapCmdLoadEncryptionInformationOpCode,                                      /* = 0x18, gBleGapCmdLoadEncryptionInformationOpCode_c */
    HandleGapCmdSetLocalPasskeyOpCode,                                                /* = 0x19, gBleGapCmdSetLocalPasskeyOpCode_c */
    HandleGapCmdStartScanningOpCode,                                                  /* = 0x1A, gBleGapCmdStartScanningOpCode_c */
    HandleGapCmdStopScanningOpCode,                                                   /* = 0x1B, gBleGapCmdStopScanningOpCode_c */
    HandleGapCmdConnectOpCode,                                                        /* = 0x1C, gBleGapCmdConnectOpCode_c */
    HandleGapCmdDisconnectOpCode,                                                     /* = 0x1D, gBleGapCmdDisconnectOpCode_c */
    HandleGapCmdSaveCustomPeerInformationOpCode,                                      /* = 0x1E, gBleGapCmdSaveCustomPeerInformationOpCode_c */
    HandleGapCmdLoadCustomPeerInformationOpCode,                                      /* = 0x1F, gBleGapCmdLoadCustomPeerInformationOpCode_c */
    HandleGapCmdCheckIfBondedOpCode,                                                  /* = 0x20, gBleGapCmdCheckIfBondedOpCode_c */
    HandleGapCmdReadFilterAcceptListSizeOpCode,                                       /* = 0x21, gBleGapCmdReadFilterAcceptListSizeOpCode_c */
    HandleGapCmdClearFilterAcceptListOpCode,                                          /* = 0x22, gBleGapCmdClearFilterAcceptListOpCode_c */
    HandleGapCmdAddDeviceToFilterAcceptListOpCode,                                    /* = 0x23, gBleGapCmdAddDeviceToFilterAcceptListOpCode_c */
    HandleGapCmdRemoveDeviceFromFilterAcceptListOpCode,                               /* = 0x24, gBleGapCmdRemoveDeviceFromFilterAcceptListOpCode_c */
    HandleGapCmdReadPublicDeviceAddressOpCode,                                        /* = 0x25, gBleGapCmdReadPublicDeviceAddressOpCode_c */
    HandleGapCmdCreateRandomDeviceAddressOpCode,                                      /* = 0x26, gBleGapCmdCreateRandomDeviceAddressOpCode_c */
    HandleGapCmdSaveDeviceNameOpCode,                                                 /* = 0x27, gBleGapCmdSaveDeviceNameOpCode_c */
    HandleGapCmdGetBondedDevicesCountOpCode,                                          /* = 0x28, gBleGapCmdGetBondedDevicesCountOpCode_c */
    HandleGapCmdGetBondedDeviceNameOpCode,                                            /* = 0x29, gBleGapCmdGetBondedDeviceNameOpCode_c */
    HandleGapCmdRemoveBondOpCode,                                                     /* = 0x2A, gBleGapCmdRemoveBondOpCode_c */
    HandleGapCmdRemoveAllBondsOpCode,                                                 /* = 0x2B, gBleGapCmdRemoveAllBondsOpCode_c */
    HandleGapCmdReadRadioPowerLevelOpCode,                                            /* = 0x2C, gBleGapCmdReadRadioPowerLevelOpCode_c */
    HandleGapCmdVerifyPrivateResolvableAddressOpCode,                                 /* = 0x2D, gBleGapCmdVerifyPrivateResolvableAddressOpCode_c */
    HandleGapCmdSetRandomAddressOpCode,                                               /* = 0x2E, gBleGapCmdSetRandomAddressOpCode_c */
    HandleGapCmdSetScanModeOpCode,                                                    /* = 0x2F, gBleGapCmdSetScanModeOpCode_c */
    HandleGapCmdSetDefaultPairingParameters,                                          /* = 0x30, gBleGapCmdSetDefaultPairingParameters_c */
    HandleGapCmdUpdateConnectionParametersOpCode,                                     /* = 0x31, gBleGapCmdUpdateConnectionParametersOpCode_c */
    HandleGapCmdEnableUpdateConnectionParametersOpCode,                               /* = 0x32, gBleGapCmdEnableUpdateConnectionParametersOpCode_c */
    HandleGapCmdUpdateLeDataLengthOpCode,                                             /* = 0x33, gBleGapCmdUpdateLeDataLengthOpCode_c */
    NULL,                                                                             /* reserved: 0x34 */
    HandleGapCmdEnableHostPrivacyOpCode,                                              /* = 0x35, gBleGapCmdEnableHostPrivacyOpCode_c */
    HandleGapCmdEnableControllerPrivacyOpCode,                                        /* = 0x36, gBleGapCmdEnableControllerPrivacyOpCode_c */
    HandleGapCmdLeScRegeneratePublicKeyOpCode,                                        /* = 0x37, gBleGapCmdLeScRegeneratePublicKeyOpCode_c */
    HandleGapCmdLeScValidateNumericValueOpCode,                                       /* = 0x38, gBleGapCmdLeScValidateNumericValueOpCode_c */
    HandleGapCmdLeScGetLocalOobDataOpCode,                                            /* = 0x39, gBleGapCmdLeScGetLocalOobDataOpCode_c */
    HandleGapCmdLeScSetPeerOobDataOpCode,                                             /* = 0x3A, gBleGapCmdLeScSetPeerOobDataOpCode_c */
    HandleGapCmdLeScSendKeypressNotificationPrivacyOpCode,                            /* = 0x3B, gBleGapCmdLeScSendKeypressNotificationPrivacyOpCode_c */
    HandleGapCmdGetBondedDevicesIdentityInformationOpCode,                            /* = 0x3C, gBleGapCmdGetBondedDevicesIdentityInformationOpCode_c */
    HandleGapCmdSetTxPowerLevelOpCode,                                                /* = 0x3D, gBleGapCmdSetTxPowerLevelOpCode_c */
    HandleGapCmdLeReadPhyOpCode,                                                      /* = 0x3E, gBleGapCmdLeReadPhyOpCode_c */
    HandleGapCmdLeSetPhyOpCode,                                                       /* = 0x3F, gBleGapCmdLeSetPhyOpCode_c */
    HandleGapCmdControllerNotificationOpCode,                                         /* = 0x40, gBleGapCmdControllerNotificationOpCode_c */
    HandleGapCmdLoadKeysOpCode,                                                       /* = 0x41, gBleGapCmdLoadKeysOpCode_c */
    HandleGapCmdSaveKeysOpCode,                                                       /* = 0x42, gBleGapCmdSaveKeysOpCode_c */
    HandleGapCmdSetChannelMapOpCode,                                                  /* = 0x43, gBleGapCmdSetChannelMapOpCode_c */
    HandleGapCmdReadChannelMapOpCode,                                                 /* = 0x44, gBleGapCmdReadChannelMapOpCode_c */
    HandleGapCmdSetPrivacyModeOpCode,                                                 /* = 0x45, gBleGapCmdSetPrivacyModeOpCode_c */
#if (defined(CPU_MKW37A512VFT4) || defined(CPU_MKW37Z512VFT4) || defined(CPU_MKW38A512VFT4) || \
        defined(CPU_MKW38Z512VFT4) || defined(CPU_MKW39A512VFT4) || defined(CPU_MKW39Z512VFT4) || \
        defined(CPU_MKW35A512VFP4) || defined(CPU_MKW35Z512VHT4) || defined(CPU_MKW36A512VFP4) || \
        defined(CPU_MKW36A512VHT4) || defined(CPU_MKW36Z512VFP4) || defined(CPU_MKW36Z512VHT4))
    HandleCtrlCmdSetScanDupFiltModeOpCode,                                            /* = 0x46, gBleCtrlCmdSetScanDupFiltModeOpCode_c */
#else
    NULL,                                                                             /* reserved: 0x46 */
#endif
    HandleGapCmdReadControllerLocalRPAOpCode,                                         /* = 0x47, gBleGapCmdReadControllerLocalRPAOpCode_c */
    HandleCtrlWritePublicDeviceAddressOpCode,                                         /* = 0x48, gBleCtrlWritePublicDeviceAddressOpCode_c */
    HandleGapCmdCheckNvmIndexOpCode,                                                  /* = 0x49, gBleGapCmdCheckNvmIndexOpCode_c */
    HandleGapCmdGetDeviceIdFromConnHandleOpCode,                                      /* = 0x4A, gBleGapCmdGetDeviceIdFromConnHandleOpCode_c */
    HandleGapCmdGetConnectionHandleFromDeviceIdOpCode,                                /* = 0x4B, gBleGapCmdGetConnectionHandleFromDeviceIdOpCode_c */
    HandleGapCmdAdvIndexChangeOpCode,                                                 /* = 0x4C, gBleGapCmdAdvIndexChangeOpCode_c */
    HandleGapCmdGetHostVersionOpCode,                                                 /* = 0x4D, gBleGapCmdGetHostVersionOpCode_c */
    HandleGapCmdGetReadRemoteVersionInfoOpCode,                                       /* = 0x4E, gBleGapCmdGetReadRemoteVersionInfoOpCode_c */
    HandleGapCmdGetConnParamsOpCode,                                                  /* = 0x4F, gBleGapCmdGetConnParamsOpCode_c */
#if defined(gBLE50_d) && (gBLE50_d == 1U)
    HandleGapCmdSetExtAdvertisingParametersOpCode,                                    /* = 0x50 gBleGapCmdSetExtAdvertisingParametersOpCode_c */
    HandleGapCmdStartExtAdvertisingOpCode,                                            /* = 0x51, gBleGapCmdStartExtAdvertisingOpCode_c */
    HandleGapCmdRemoveAdvSetOpCode,                                                   /* = 0x52, gBleGapCmdRemoveAdvSetOpCode_c */
    HandleGapCmdStopExtAdvertisingOpCode,                                             /* = 0x53, gBleGapCmdStopExtAdvertisingOpCode_c */
    HandleGapCmdUpdatePeriodicAdvListOpCode,                                          /* = 0x54, gBleGapCmdUpdatePeriodicAdvListOpCode_c */
    HandleGapCmdSetPeriodicAdvParametersOpCode,                                       /* = 0x55, gBleGapCmdSetPeriodicAdvParametersOpCode_c */
    HandleGapCmdStartPeriodicAdvOpCode,                                               /* = 0x56, gBleGapCmdStartPeriodicAdvOpCode_c */
    HandleGapCmdStopPeriodicAdvOpCode,                                                /* = 0x57, gBleGapCmdStopPeriodicAdvOpCode_c */
    HandleGapCmdSetExtAdvertisingDataOpCode,                                          /* = 0x58, gBleGapCmdSetExtAdvertisingDataOpCode_c */
    HandleGapCmdSetPeriodicAdvDataOpCode,                                             /* = 0x59, gBleGapCmdSetPeriodicAdvDataOpCode_c */
    HandleGapCmdPeriodicAdvCreateSyncOpCode,                                          /* = 0x5A, gBleGapCmdPeriodicAdvCreateSyncOpCode_c */
    HandleGapCmdPeriodicAdvTerminateSyncOpCode,                                       /* = 0x5B, gBleGapCmdPeriodicAdvTerminateSyncOpCode_c */
    HandleCtrlCmdConfigureAdvCodingSchemeOpCode,                                      /* = 0x5C, gBleCtrlCmdConfigureAdvCodingSchemeOpCode_c */
#else /* gBLE50_d */  
    NULL,                                                                             /* reserved: 0x50 */
    NULL,                                                                             /* reserved: 0x51 */
    NULL,                                                                             /* reserved: 0x52 */
    NULL,                                                                             /* reserved: 0x53 */
    NULL,                                                                             /* reserved: 0x54 */
    NULL,                                                                             /* reserved: 0x55 */
    NULL,                                                                             /* reserved: 0x56 */
    NULL,                                                                             /* reserved: 0x57 */
    NULL,                                                                             /* reserved: 0x58 */
    NULL,                                                                             /* reserved: 0x59 */
    NULL,                                                                             /* reserved: 0x5A */
    NULL,                                                                             /* reserved: 0x5B */
    NULL,                                                                             /* reserved: 0x5C */
#endif /* gBLE50_d */
#if defined(gBLE51_d) && (gBLE51_d == 1U)    
    HandleGapCmdSetConnectionlessCteTransmitParametersOpCode,                         /* = 0x5D, gBleGapCmdSetConnectionlessCteTransmitParametersOpCode_c */
    HandleGapCmdEnableConnectionlessCteTransmitOpCode,                                /* = 0x5E, gBleGapCmdEnableConnectionlessCteTransmitOpCode_c */
    HandleGapCmdEnableConnectionlessIqSamplingOpCode,                                 /* = 0x5F, gBleGapCmdEnableConnectionlessIqSamplingOpCode_c */
    HandleGapCmdSetConnectionCteReceiveParametersOpCode,                              /* = 0x60, gBleGapCmdSetConnectionCteReceiveParametersOpCode_c */
    HandleGapCmdSetConnectionCteTransmitParametersOpCode,                             /* = 0x61, gBleGapCmdSetConnectionCteTransmitParametersOpCode_c */
    HandleGapCmdEnableConnectionCteRequestOpCode,                                     /* = 0x62, gBleGapCmdEnableConnectionCteRequestOpCode_c */
    HandleGapCmdEnableConnectionCteResponseOpCode,                                    /* = 0x63, gBleGapCmdEnableConnectionCteResponseOpCode_c */
    HandleGapCmdReadAntennaInformationOpCode,                                         /* = 0x64, gBleGapCmdReadAntennaInformationOpCode_c */
    HandleGapCmdGenerateDhKeyV2OpCode,                                                /* = 0x65, gBleGapCmdGenerateDhKeyV2OpCode_c */
    HandleGapCmdModifySleepClockAccuracy,                                             /* = 0x66, gBleGapCmdModifySleepClockAccuracy_c */
    HandleGapCmdSetPeriodicAdvRecvEnableOpCode,                                       /* = 0x67, gBleGapCmdSetPeriodicAdvRecvEnableOpCode_c */
    HandleGapCmdSetPeriodicAdvRecvDisableOpCode,                                      /* = 0x68, gBleGapCmdSetPeriodicAdvRecvDisableOpCode_c */
    HandleGapCmdPeriodicAdvSyncTransferOpCode,                                        /* = 0x69, gBleGapCmdPeriodicAdvSyncTransferOpCode_c */
    HandleGapCmdPeriodicAdvSetInfoTransferOpCode,                                     /* = 0x6A, gBleGapCmdPeriodicAdvSetInfoTransferOpCode_c */
    HandleGapCmdSetPeriodicAdvSyncTransferParamsOpCode,                               /* = 0x6B, gBleGapCmdSetPeriodicAdvSyncTransferParamsOpCode_c */
    HandleGapCmdSetDefaultPeriodicAdvSyncTransferParamsOpCode,                        /* = 0x6C, gBleGapCmdSetDefaultPeriodicAdvSyncTransferParamsOpCode_c */
#else /* gBLE51_d */   
    NULL,                                                                             /* reserved: 0x5D */
    NULL,                                                                             /* reserved: 0x5E */
    NULL,                                                                             /* reserved: 0x5F */
    NULL,                                                                             /* reserved: 0x60 */
    NULL,                                                                             /* reserved: 0x61 */
    NULL,                                                                             /* reserved: 0x62 */
    NULL,                                                                             /* reserved: 0x63 */
    NULL,                                                                             /* reserved: 0x64 */
    NULL,                                                                             /* reserved: 0x65 */
    NULL,                                                                             /* reserved: 0x66 */
    NULL,                                                                             /* reserved: 0x67 */
    NULL,                                                                             /* reserved: 0x68 */
    NULL,                                                                             /* reserved: 0x69 */
    NULL,                                                                             /* reserved: 0x6A */
    NULL,                                                                             /* reserved: 0x6B */
    NULL,                                                                             /* reserved: 0x6C */
#endif /* gBLE51_d */ 
#if defined(gBLE52_d) && (gBLE52_d == 1U)    
    HandleGapCmdEnhancedReadTransmitPowerLevelOpCode,                                 /* = 0x6D, gBleGapCmdEnhancedReadTransmitPowerLevelOpCode_c */
    HandleGapCmdReadRemoteTransmitPowerLevelOpCode,                                   /* = 0x6E, gBleGapCmdReadRemoteTransmitPowerLevelOpCode_c */
    HandleGapCmdSetPathLossReportingParamsOpCode,                                     /* = 0x6F, gBleGapCmdSetPathLossReportingParamsOpCode_c */
    HandleGapCmdEnablePathLossReportingOpCode,                                        /* = 0x70, gBleGapCmdEnablePathLossReportingOpCode_c */
    HandleGapCmdEnableTransmitPowerReportingOpCode,                                   /* = 0x71, gBleGapCmdEnableTransmitPowerReportingOpCode_c */
#else /* gBLE52_d */ 
    NULL,                                                                             /* reserved: 0x6D */
    NULL,                                                                             /* reserved: 0x6E */
    NULL,                                                                             /* reserved: 0x6F */
    NULL,                                                                             /* reserved: 0x70 */
    NULL,                                                                             /* reserved: 0x71 */
#endif /* gBLE52_d */ 
    HandleGapCmdEattConnectionRequestOpCode,                                          /* = 0x72, gBleGapCmdEattConnectionRequestOpCode_c */
    HandleGapCmdEattConnectionAcceptOpCode,                                           /* = 0x73, gBleGapCmdEattConnectionAcceptOpCode_c */
    HandleGapCmdEattReconfigureRequestOpCode,                                         /* = 0x74, gBleGapCmdEattReconfigureRequestOpCode_c */
    HandleGapCmdEattSendCreditsRequestOpCode,                                         /* = 0x75, gBleGapCmdEattSendCreditsRequestOpCode_c */ 
    HandleGapCmdEattDisconnectRequestOpCode,                                          /* = 0x76, gBleGapCmdEattDisconnectRequestOpCode_c */ 
    HandleCtrlCmdSetConnNotificationModeOpCode,                                       /* = 0x77, gBleCtrlCmdSetConnNotificationModeOpCode_c*/

#if defined(gBLE50_d) && (gBLE50_d == 1U)
    HandleCmdDisablePrivacyPerAdvSetOpCode,                                           /* = 0x78, gBleCmdDisablePrivacyPerAdvSetOpCode_c */
#else /* gBLE50_d */
    NULL,                                                                             /* reserved: 0x78 */
#endif
    HandleGapCmdLeSetSchedulerPriority,                                               /* = 0x79, gBleGapCmdLeSetSchedulerPriority_c */
    NULL,                                                                             /* reserved: 0x7A - gBleCtrlCmdSetMDBitModeOpCode_c on KW38 */
    HandleGapCmdLeSetHostFeature,                                                     /* = 0x7B, gBleGapCmdLeSetHostFeature_c */
#if defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U) && (defined(CPU_KW45B41Z83AFTA) || defined(CPU_K32W1480VFTA))
    HandleGapCmdPlatformRegisterErrorCallback,                                        /* = 0x7C, gBleGapCmdPlatformRegisterErrorCallbackOpCode_c*/
#else
    NULL,                                                                              /* reserved: 0x7C */
#endif /* defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U) && (defined(CPU_KW45B41Z83AFTA) || defined(CPU_K32W1480VFTA)) */
    NULL,                                                                             /* reserved: 0x7D */
    NULL,                                                                             /* reserved: 0x7E */
};
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
const pfGapOpCodeHandler_t maGapEvtOpCodeHandlers[]=
{
    HandleGapStatusOpCode,                                                            /* = 0x80, gBleGapStatusOpCode_c */
    NULL,                                                                             /* reserved: 0x81 */
    HandleGapEvtCheckNotificationStatusOpCode,                                        /* = gBleG gBleGapEvtCheckNotificationStatusOpCode_c */
    HandleGapEvtCheckIndicationStatusOpCode,                                          /* = 0x82, gBleGapEvtCheckIndicationStatusOpCode_c */
    NULL,                                                                             /* reserved: 0x83 */
    HandleGapEvtLoadEncryptionInformationOpCode,                                      /* = 0x84, gBleGapEvtLoadEncryptionInformationOpCode_c */
    HandleGapEvtLoadCustomPeerInformationOpCode,                                      /* = 0x85, gBleGapEvtLoadCustomPeerInformationOpCode_c */
    HandleGapEvtCheckIfBondedOpCode,                                                  /* = 0x86, gBleGapEvtCheckIfBondedOpCode_c */
    HandleGapEvtGetBondedDevicesCountOpCode,                                          /* = 0x87, gBleGapEvtGetBondedDevicesCountOpCode_c */
    HandleGapEvtGetBondedDeviceNameOpCode,                                            /* = 0x88, gBleGapEvtGetBondedDeviceNameOpCode_c */
    HandleGapEvtGenericEventInitializationCompleteOpCode,                             /* = 0x89, gBleGapEvtGenericEventInitializationCompleteOpCode_c */
    HandleGapEvtGenericEventInternalErrorOpCode,                                      /* = 0x8A, gBleGapEvtGenericEventInternalErrorOpCode_c */
    HandleGapEvtGenericEventAdvertisingSetupFailedOpCode,                             /* = 0x8B, gBleGapEvtGenericEventAdvertisingSetupFailedOpCode_c */
    HandleGapEvtGenericEventAdvertisingParametersSetupCompleteOpCode,                 /* = 0x8C, gBleGapEvtGenericEventAdvertisingParametersSetupCompleteOpCode_c */
    HandleGapEvtGenericEventAdvertisingDataSetupCompleteOpCode,                       /* = 0x8D, gBleGapEvtGenericEventAdvertisingDataSetupCompleteOpCode_c */
    HandleGapEvtGenericEventFilterAcceptListSizeReadOpCode,                           /* = 0x8E, gBleGapEvtGenericEventFilterAcceptListSizeReadOpCode_c */
    HandleGapEvtGenericEventDeviceAddedToFilterAcceptListOpCode,                      /* = 0x8F, gBleGapEvtGenericEventDeviceAddedToFilterAcceptListOpCode_c */
    HandleGapEvtGenericEventDeviceRemovedFromFilterAcceptListOpCode,                  /* = 0x90, gBleGapEvtGenericEventDeviceRemovedFromFilterAcceptListOpCode_c */
    HandleGapEvtGenericEventFilterAcceptListClearedOpCode,                            /* = 0x91, gBleGapEvtGenericEventFilterAcceptListClearedOpCode_c */
    HandleGapEvtGenericEventRandomAddressReadyOpCode,                                 /* = 0x92, gBleGapEvtGenericEventRandomAddressReadyOpCode_c */
    HandleGapEvtGenericEventCreateConnectionCanceledOpCode,                           /* = 0x93, gBleGapEvtGenericEventCreateConnectionCanceledOpCode_c */
    HandleGapEvtGenericEventPublicAddressReadOpCode,                                  /* = 0x94, gBleGapEvtGenericEventPublicAddressReadOpCode_c */
    HandleGapEvtGenericEventAdvTxPowerLevelReadOpCode,                                /* = 0x95, gBleGapEvtGenericEventAdvTxPowerLevelReadOpCode_c */
    HandleGapEvtGenericEventPrivateResolvableAddressVerifiedOpCode,                   /* = 0x96, gBleGapEvtGenericEventPrivateResolvableAddressVerifiedOpCode_c */
    HandleGapEvtGenericEventRandomAddressSetOpCode,                                   /* = 0x97, gBleGapEvtGenericEventRandomAddressSetOpCode_c */
    HandleGapEvtAdvertisingEventAdvertisingStateChangedOpCode,                        /* = 0x98, gBleGapEvtAdvertisingEventAdvertisingStateChangedOpCode_c */
    HandleGapEvtAdvertisingEventAdvertisingCommandFailedOpCode,                       /* = 0x99, gBleGapEvtAdvertisingEventAdvertisingCommandFailedOpCode_c */
    HandleGapEvtScanningEventScanStateChangedOpCode,                                  /* = 0x9A, gBleGapEvtScanningEventScanStateChangedOpCode_c */
    HandleGapEvtScanningEventScanCommandFailedOpCode,                                 /* = 0x9B, gBleGapEvtScanningEventScanCommandFailedOpCode_c */
    HandleGapEvtScanningEventDeviceScannedOpCode,                                     /* = 0x9C, gBleGapEvtScanningEventDeviceScannedOpCode_c */
    HandleGapEvtConnectionEventConnectedOpCode,                                       /* = 0x9D, gBleGapEvtConnectionEventConnectedOpCode_c */
    HandleGapEvtConnectionEventPairingRequestOpCode,                                  /* = 0x9E, gBleGapEvtConnectionEventPairingRequestOpCode_c */
    HandleGapEvtConnectionEventPeripheralSecurityRequestOpCode,                       /* = 0x9F, gBleGapEvtConnectionEventPeripheralSecurityRequestOpCode_c */
    HandleGapEvtConnectionEventPairingResponseOpCode,                                 /* = 0xA0, gBleGapEvtConnectionEventPairingResponseOpCode_c */
    HandleGapEvtConnectionEventAuthenticationRejectedOpCode,                          /* = 0xA1, gBleGapEvtConnectionEventAuthenticationRejectedOpCode_c */
    HandleGapEvtConnectionEventPasskeyRequestOpCode,                                  /* = 0xA2, gBleGapEvtConnectionEventPasskeyRequestOpCode_c */
    HandleGapEvtConnectionEventOobRequestOpCode,                                      /* = 0xA3, gBleGapEvtConnectionEventOobRequestOpCode_c */
    HandleGapEvtConnectionEventPasskeyDisplayOpCode,                                  /* = 0xA4, gBleGapEvtConnectionEventPasskeyDisplayOpCode_c */
    HandleGapEvtConnectionEventKeyExchangeRequestOpCode,                              /* = 0xA5, gBleGapEvtConnectionEventKeyExchangeRequestOpCode_c */
    HandleGapEvtConnectionEventKeysReceivedOpCode,                                    /* = 0xA6, gBleGapEvtConnectionEventKeysReceivedOpCode_c */
    HandleGapEvtConnectionEventLongTermKeyRequestOpCode,                              /* = 0xA7, gBleGapEvtConnectionEventLongTermKeyRequestOpCode_c */
    HandleGapEvtConnectionEventEncryptionChangedOpCode,                               /* = 0xA8, gBleGapEvtConnectionEventEncryptionChangedOpCode_c */
    HandleGapEvtConnectionEventPairingCompleteOpCode,                                 /* = 0xA9, gBleGapEvtConnectionEventPairingCompleteOpCode_c */
    HandleGapEvtConnectionEventDisconnectedOpCode,                                    /* = 0xAA, gBleGapEvtConnectionEventDisconnectedOpCode_c */
    HandleGapEvtConnectionEventRssiReadOpCode,                                        /* = 0xAB, gBleGapEvtConnectionEventRssiReadOpCode_c */
    HandleGapEvtConnectionEventTxPowerLevelReadOpCode,                                /* = 0xAC, gBleGapEvtConnectionEventTxPowerLevelReadOpCode_c */
    HandleGapEvtConnectionEventPowerReadFailureOpCode,                                /* = 0xAD, gBleGapEvtConnectionEventPowerReadFailureOpCode_c */
    HandleGapEvtConnectionEventParameterUpdateRequestOpCode,                          /* = 0xAE, gBleGapEvtConnectionEventParameterUpdateRequestOpCode_c */
    HandleGapEvtConnectionEventParameterUpdateCompleteOpCode,                         /* = 0xAF, gBleGapEvtConnectionEventParameterUpdateCompleteOpCode_c */
    HandleGapEvtConnectionEventLeDataLengthChangedOpCode,                             /* = 0xB0, gBleGapEvtConnectionEventLeDataLengthChangedOpCode_c */
    HandleGapEvtConnectionEventLeScOobDataRequestOpCode,                              /* = 0xB1, gBleGapEvtConnectionEventLeScOobDataRequestOpCode_c */
    HandleGapEvtConnectionEventLeScDisplayNumericValueOpCode,                         /* = 0xB2, gBleGapEvtConnectionEventLeScDisplayNumericValueOpCode_c */
    HandleGapEvtConnectionEventLeScKeypressNotificationOpCode,                        /* = 0xB3, gBleGapEvtConnectionEventLeScKeypressNotificationOpCode_c */
    HandleGapEvtConnectionEventSetDataLenghtFailureOpCode,                            /* = 0xB4, gBleGapEvtConnectionEventLeSetDataLengthFailedOpCode_c */
    HandleGapEvtGenericEventLeScPublicKeyRegeneratedOpCode,                           /* = 0xB5, gBleGapEvtGenericEventLeScPublicKeyRegeneratedOpCode_c */
    HandleGapEvtGenericEventLeScLocalOobDataOpCode,                                   /* = 0xB6, gBleGapEvtGenericEventLeScLocalOobDataOpCode_c */
    HandleGapEvtGenericEventHostPrivacyStateChangedOpCode,                            /* = 0xB7, gBleGapEvtGenericEventHostPrivacyStateChangedOpCode_c */
    HandleGapEvtGenericEventControllerPrivacyStateChangedOpCode,                      /* = 0xB8, gBleGapEvtGenericEventControllerPrivacyStateChangedOpCode_c */
    HandleGapEvtGenericEventTxPowerLevelSetCompleteOpCode,                            /* = 0xB9, gBleGapEvtGenericEventTxPowerLevelSetCompleteOpCode_c */
    HandleGapEvtGenericEventLePhyEventOpCode,                                         /* = 0xBA, gBleGapEvtGenericEventLePhyEventOpCode_c */
    NULL,                                                                             /* reserved: 0xBB */
    HandleGapEvtGenericEventControllerNotificationOpCode,                             /* = 0xBC, gBleGapEvtGenericEventControllerNotificationOpCode_c */
    HandleGapEvtGenericEventBondCreatedOpCode,                                        /* = 0xBD, gBleGapEvtGenericEventBondCreatedOpCode_c */
    HandleGapEvtGenericEventChannelMapSetOpCode,                                      /* = 0xBE, gBleGapEvtGenericEventChannelMapSetOpCode_c */
    HandleGapEvtConnectionEventChannelMapReadOpCode,                                  /* = 0xBF, gBleGapEvtConnectionEventChannelMapReadOpCode_c */
    HandleGapEvtConnectionEventChannelMapReadFailureOpCode,                           /* = 0xC0, gBleGapEvtConnectionEventChannelMapReadFailureOpCode_c */
    NULL,                                                                             /* reserved: 0xC1 */
#if defined(gBLE50_d) && (gBLE50_d == 1U)    
    HandleGapEvtGenericEventExtAdvertisingDataSetupCompleteOpCode,                    /* = 0xC2, gBleGapEvtGenericEventExtAdvertisingDataSetupCompleteOpCode_c */
    NULL,                                                                             /* reserved: 0xC3 */
    NULL,                                                                             /* reserved: 0xC4 */
    NULL,                                                                             /* reserved: 0xC5 */
    NULL,                                                                             /* reserved: 0xC6 */
    HandleGapEvtAdvertisingEventAdvertisingSetTerminatedOpCode,                       /* = 0xC7, gBleGapEvtAdvertisingEventAdvertisingSetTerminatedOpCode_c */
    NULL,                                                                             /* reserved: 0xC8 */
    NULL,                                                                             /* reserved: 0xC9 */
    NULL,                                                                             /* reserved: 0xCA */
    HandleGapEvtScanningEventExtDeviceScannedOpCode,                                  /* = 0xCB, gBleGapEvtScanningEventExtDeviceScannedOpCode_c */
    NULL,                                                                             /* reserved: 0xCC */
    NULL,                                                                             /* reserved: 0xCD */
    NULL,                                                                             /* reserved: 0xCE */
    HandleGapEvtScanningEventPeriodicDeviceScannedOpCode,                             /* = 0xCF, gBleGapEvtScanningEventPeriodicDeviceScannedOpCode_c */
    HandleGapEvtGenericEventPeriodicAdvCreateSyncCancelledOpCode,                     /* = 0xD0, gBleGapEvtGenericEventPeriodicAdvCreateSyncCancelledOpCode_c */
#else /*gBLE50_d == 1*/
    NULL,                                                                             /* reserved: 0xC2 */
    NULL,                                                                             /* reserved: 0xC3 */
    NULL,                                                                             /* reserved: 0xC4 */
    NULL,                                                                             /* reserved: 0xC5 */
    NULL,                                                                             /* reserved: 0xC6 */
    NULL,                                                                             /* reserved: 0xC7 */
    NULL,                                                                             /* reserved: 0xC8 */
    NULL,                                                                             /* reserved: 0xC9 */
    NULL,                                                                             /* reserved: 0xCA */
    NULL,                                                                             /* reserved: 0xCB */
    NULL,                                                                             /* reserved: 0xCC */
    NULL,                                                                             /* reserved: 0xCD */
    NULL,                                                                             /* reserved: 0xCE */
    NULL,                                                                             /* reserved: 0xCF */
    NULL,                                                                             /* reserved: 0xD0 */
#endif /*gBLE50_d == 1*/
    NULL,                                                                             /* reserved: 0xD1 */
    NULL,                                                                             /* reserved: 0xD2 */
    NULL,                                                                             /* reserved: 0xD3 */
    HandleGapEvtCheckNvmIndexOpCode,                                                  /* = 0xD4, gBleGapEvtCheckNvmIndexOpCode_c */
    NULL,                                                                             /* reserved: 0xD5 */
    HandleGapEvtGetConnectionHandleFromDeviceIdOpCode,                                /* = 0xD6, gBleGapEvtGetConnectionHandleFromDeviceIdOpCode_c */
    NULL,                                                                             /* reserved: 0xD7 */
    NULL,                                                                             /* reserved: 0xD8 */
    NULL,                                                                             /* reserved: 0xD9 */
    NULL,                                                                             /* reserved: 0xDA */
    NULL,                                                                             /* reserved: 0xDB */
    NULL,                                                                             /* reserved: 0xDC */
    NULL,                                                                             /* reserved: 0xDD */
    NULL,                                                                             /* reserved: 0xDE */
    NULL,                                                                             /* reserved: 0xDF */
    NULL,                                                                             /* reserved: 0xE0 */
    NULL,                                                                             /* reserved: 0xE1 */
    NULL,                                                                             /* reserved: 0xE2 */
    NULL,                                                                             /* reserved: 0xE3 */
    NULL,                                                                             /* reserved: 0xE4 */
    NULL,                                                                             /* reserved: 0xE5 */
    NULL,                                                                             /* reserved: 0xE6 */
    NULL,                                                                             /* reserved: 0xE7 */
    NULL,                                                                             /* reserved: 0xE8 */
    NULL,                                                                             /* reserved: 0xE9 */
    NULL,                                                                             /* reserved: 0xEA */
    NULL,                                                                             /* reserved: 0xEB */
    NULL,                                                                             /* reserved: 0xEC */
    NULL,                                                                             /* reserved: 0xED */
    NULL,                                                                             /* reserved: 0xEE */
    NULL,                                                                             /* reserved: 0xEF */
    NULL,                                                                             /* reserved: 0xF0 */
    NULL,                                                                             /* reserved: 0xF1 */
    NULL,                                                                             /* reserved: 0xF2 */
    NULL,                                                                             /* reserved: 0xF3 */
    NULL,                                                                             /* reserved: 0xF4 */
    NULL,                                                                             /* reserved: 0xF5 */
    NULL,                                                                             /* reserved: 0xF6 */
    NULL                                                                              /* reserved: 0xF7 */
}
#endif /* gFsciBleHost_d */


/*! Size of maGapGetConnEventFromBufferHandlers array */
const uint32_t mGapGetConnEventFromBufferHandlersArraySize =
    (sizeof(maGapGetConnEventFromBufferHandlers)/sizeof(maGapGetConnEventFromBufferHandlers[0]));

/*! Size of maGapGetBufferFromConnEventHandlers array */
const uint32_t mGapGetBufferFromConnEventHandlersArraySize =
    (sizeof(maGapGetBufferFromConnEventHandlers)/sizeof(maGapGetBufferFromConnEventHandlers[0]));

/*! Size of maGapGetConnEventBufferSizeHandlers array */
const uint32_t mGapGetConnEventBufferSizeHandlersArraySize =
    (sizeof(maGapGetConnEventBufferSizeHandlers)/sizeof(maGapGetConnEventBufferSizeHandlers[0]));

/*! Size of maGapGetGenericEventFromBufferHandlers array */
const uint32_t mGapGetGenericEventFromBufferHandlersArraySize =
    (sizeof(maGapGetGenericEventFromBufferHandlers)/sizeof(maGapGetGenericEventFromBufferHandlers[0]));

/*! Size of maGapGetBufferFromGenericEventHandlers array */
const uint32_t mGapGetBufferFromGenericEventHandlersArraySize =
    (sizeof(maGapGetBufferFromGenericEventHandlers)/sizeof(maGapGetBufferFromGenericEventHandlers[0]));

/*! Size of maGapGetGenericEventBufferSizeHandlers array */
const uint32_t mGapGetGenericEventBufferSizeHandlersArraySize =
    (sizeof(maGapGetGenericEventBufferSizeHandlers)/sizeof(maGapGetGenericEventBufferSizeHandlers[0]));

/*! Size of maGapCmdOpCodeHandlers array */
const uint32_t maGapCmdOpCodeHandlersArraySize =
    (sizeof(maGapCmdOpCodeHandlers)/sizeof(maGapCmdOpCodeHandlers[0]));

#if gFsciBleHost_d
/*! Size of maGapEvtOpCodeHandlers array */
const uint32_t maGapEvtOpCodeHandlersArraySize =
    (sizeof(maGapEvtOpCodeHandlers)/sizeof(maGapEvtOpCodeHandlers[0]));
#endif /* gFsciBleHost_d */
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtConnectedFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the connectedEvent data fields from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtConnectedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetConnectedEventFromBuffer(&pConnectionEvent->eventData.connectedEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetPairingParamsFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the pairingEvent data fields from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetPairingParamsFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetPairingParametersFromBuffer(&pConnectionEvent->eventData.pairingEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtPeripheralSecurityRequestFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the peripheralSecurityRequestEvent data fields from the
*              provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtPeripheralSecurityRequestFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetPeripheralSecurityRequestParametersFromBuffer(&pConnectionEvent->eventData.peripheralSecurityRequestEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtAuthenticationRejectedFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the authenticationRejectedEvent data fields from the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtAuthenticationRejectedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetAuthenticationRejectedEventFromBuffer(&pConnectionEvent->eventData.authenticationRejectedEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtPasskeyDisplayFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Extracts the passkeyForDisplay value from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtPasskeyDisplayFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetUint32ValueFromBuffer(pConnectionEvent->eventData.passkeyForDisplay, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtKeyExchangeRequestFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the keyExchangeRequestEvent data fields from the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtKeyExchangeRequestFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetKeyExchangeRequestEventFromBuffer(&pConnectionEvent->eventData.keyExchangeRequestEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtKeysReceivedFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the keysReceivedEvent data fields from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtKeysReceivedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetKeysReceivedEventFromBuffer(&pConnectionEvent->eventData.keysReceivedEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtLongTermKeyRequestFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the longTermKeyRequestEvent data fields from the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtLongTermKeyRequestFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetLongTermKeyRequestEventFromBuffer(&pConnectionEvent->eventData.longTermKeyRequestEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtEncryptionChangedFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the encryptionChangedEvent data fields from the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtEncryptionChangedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetEncryptionChangedEventFromBuffer(&pConnectionEvent->eventData.encryptionChangedEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtPairingCompleteFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the pairingCompleteEvent data fields from the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtPairingCompleteFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetPairComplEventFromBuffer(&pConnectionEvent->eventData.pairingCompleteEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtDisconnectedFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the disconnectedEvent data fields from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtDisconnectedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetDisconnectedEventFromBuffer(&pConnectionEvent->eventData.disconnectedEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtRssiReadFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Extracts the rssi_dBm value from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtRssiReadFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetUint8ValueFromBufferSigned(pConnectionEvent->eventData.rssi_dBm, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtTxPowerLevelReadFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Extracts the txPowerLevel_dBm value from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtTxPowerLevelReadFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetUint8ValueFromBufferSigned(pConnectionEvent->eventData.txPowerLevel_dBm, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtPowerReadFailureFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Extracts the failReason value from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtPowerReadFailureFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetEnumValueFromBuffer(pConnectionEvent->eventData.failReason, *ppBuffer, bleResult_t);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtParameterUpdateRequestFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the connectionUpdateRequest data fields from the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtParameterUpdateRequestFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetConnParamUpdateReqFromBuffer(&pConnectionEvent->eventData.connectionUpdateRequest, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtParameterUpdateCompleteFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the connectionUpdateComplete data fields from the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtParameterUpdateCompleteFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetConnParameterUpdateCompleteFromBuffer(&pConnectionEvent->eventData.connectionUpdateComplete, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtLeDataLengthChangedFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the leDataLengthChanged data fields from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtLeDataLengthChangedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetConnLeDataLengthChangedFromBuffer(&pConnectionEvent->eventData.leDataLengthChanged, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtLeScDisplayNumericValueFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Extracts the numericValueForDisplay value from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtLeScDisplayNumericValueFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetUint32ValueFromBuffer(pConnectionEvent->eventData.numericValueForDisplay, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtLeScKeypressNotificationFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Extracts the incomingKeypressNotification value from the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtLeScKeypressNotificationFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetEnumValueFromBuffer(pConnectionEvent->eventData.incomingKeypressNotification, *ppBuffer, gapKeypressNotification_t);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtChannelMapReadFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the channelMap array from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the array
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtChannelMapReadFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetArrayFromBuffer(pConnectionEvent->eventData.channelMap, *ppBuffer, gcBleChannelMapSize_c);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtChannelMapReadFailureFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Extracts the failReason value from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtChannelMapReadFailureFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetEnumValueFromBuffer(pConnectionEvent->eventData.failReason, *ppBuffer, bleResult_t);
}

#if defined(gBLE51_d) && (gBLE51_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void GetConnEvtIqReportReceivedFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the connIqReport data fields from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtIqReportReceivedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetConnIqReportReceivedFromBuffer(&pConnectionEvent->eventData.connIqReport, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtCteRequestFailedFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Extracts the cteRequestFailedEvent value from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtCteRequestFailedFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetEnumValueFromBuffer(pConnectionEvent->eventData.cteRequestFailedEvent, *ppBuffer, bleResult_t);
}
#endif /* gBLE51_d */

#if defined(gBLE52_d) && (gBLE52_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void GetConnEvtPathLossThresholdFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the pathLossThreshold data fields from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtPathLossThresholdFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetPathLossThresholdFromBuffer(&pConnectionEvent->eventData.pathLossThreshold, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtTransmitPowerReportingFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the transmitPowerReporting data fields from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtTransmitPowerReportingFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetTransmitPowerReportingFromBuffer(&pConnectionEvent->eventData.transmitPowerReporting, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetConnEvtEnhancedReadTransmitPowerLevelFromBuffer(
*                                       gapConnectionEvent_t *pConnectionEvent,
*                                       uint8_t              **ppBuffer)
*\brief        Fills the transmitPowerInfo data fields from the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event to fill.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnEvtEnhancedReadTransmitPowerLevelFromBuffer
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetTransmitPowerInfoFromBuffer(&pConnectionEvent->eventData.transmitPowerInfo, ppBuffer);
}
#endif /* gBLE52_d */

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtConnectedBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the gConnEvtConnected_c
*              event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtConnectedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    uint32_t bufferSize = 0U;
    
    bufferSize += fsciBleGapGetConnectedEventBufferSize(&pConnectionEvent->eventData.connectedEvent);
    if( FALSE == pConnectionEvent->eventData.connectedEvent.peerRpaResolved )
    {
        bufferSize -= gcBleDeviceAddressSize_c;
    }
    if( FALSE == pConnectionEvent->eventData.connectedEvent.localRpaUsed )
    {
        bufferSize -= gcBleDeviceAddressSize_c;
    }
    
    return bufferSize;
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetPairingParamsBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtPairingRequest_c and the gConnEvtPairingResponse_c events.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetPairingParamsBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetPairingParametersBufferSize(&pConnectionEvent->eventData.pairingEvent);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtPeripheralSecurityRequestBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtPeripheralSecurityRequest_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtPeripheralSecurityRequestBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetPeripheralSecurityRequestParametersBufferSize(&pConnectionEvent->eventData.peripheralSecurityRequestEvent);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtAuthenticationRejectedBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtAuthenticationRejected_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtAuthenticationRejectedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetAuthenticationRejectedEventBufferSize(&pConnectionEvent->eventData.authenticationRejectedEvent);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtPasskeyDisplayBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtPasskeyDisplay_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtPasskeyDisplayBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return sizeof(uint32_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtKeyExchangeRequestBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtKeyExchangeRequest_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtKeyExchangeRequestBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetKeyExchangeRequestEventBufferSize(&pConnectionEvent->eventData.keyExchangeRequestEvent);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtKeysReceivedBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtKeysReceived_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtKeysReceivedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetKeysReceivedEventBufferSize(&pConnectionEvent->eventData.keysReceivedEvent);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtLongTermKeyRequestBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtLongTermKeyRequest_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtLongTermKeyRequestBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetLongTermKeyRequestEventBufferSize(&pConnectionEvent->eventData.longTermKeyRequestEvent);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtEncryptionChangedBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtEncryptionChanged_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtEncryptionChangedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetEncryptionChangedEventBufferSize(&pConnectionEvent->eventData.encryptionChangedEvent);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtPairingCompleteBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtPairingComplete_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtPairingCompleteBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetPairingCompleteEventBufferSize(&pConnectionEvent->eventData.pairingCompleteEvent);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtDisconnectedBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtDisconnected_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtDisconnectedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetDisconnectedEventBufferSize(&pConnectionEvent->eventData.disconnectedEvent);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtRssiReadBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the gConnEvtRssiRead_c
*              event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtRssiReadBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return sizeof(int8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtTxPowerLevelReadBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtTxPowerLevelRead_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtTxPowerLevelReadBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return sizeof(int8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtPowerReadFailureBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtPowerReadFailure_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtPowerReadFailureBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return sizeof(bleResult_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtParameterUpdateRequestBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtParameterUpdateRequest_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtParameterUpdateRequestBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetConnParameterUpdateRequestBufferSize(&pConnectionEvent->eventData.connectionUpdateRequest);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtParameterUpdateCompleteBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtParameterUpdateComplete_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtParameterUpdateCompleteBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetConnParameterUpdateCompleteBufferSize(&pConnectionEvent->eventData.connectionUpdateComplete);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtLeSetDataLengthFailureBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtLeSetDataLengthFailure_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtLeSetDataLengthFailureBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetConnLeSetDataLengthFailureBufferSize(&pConnectionEvent->eventData.failReason);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtLeDataLengthChangedBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtLeDataLengthChanged_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtLeDataLengthChangedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetConnLeDataLengthChangedBufferSize(&pConnectionEvent->eventData.leDataLengthChanged);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtLeScDisplayNumericValueBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtLeScDisplayNumericValue_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtLeScDisplayNumericValueBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetConnLeScDisplayNumericValueBufferSize(&pConnectionEvent->eventData.numericValueForDisplay);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtLeScKeypressNotificationBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtLeScKeypressNotification_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtLeScKeypressNotificationBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetConnLeScKeypressNotificationBufferSize(&pConnectionEvent->eventData.incomingKeypressNotification);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtChannelMapReadBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtChannelMapRead_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtChannelMapReadBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return gcBleChannelMapSize_c;
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtChannelMapReadFailureBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtChannelMapReadFailure_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtChannelMapReadFailureBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return sizeof(bleResult_t);
}

#if defined(gBLE51_d) && (gBLE51_d == 1U)
/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtIqReportReceivedBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtIqReportReceived_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtIqReportReceivedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetConnectionIqReportReceivedBufferSize(&pConnectionEvent->eventData.connIqReport);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtCteRequestFailedBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtCteRequestFailed_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtCteRequestFailedBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return sizeof(bleResult_t);
}
#endif /* gBLE51_d */

#if defined(gBLE52_d) && (gBLE52_d == 1U)
/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtPathLossThresholdBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtPathLossThreshold_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtPathLossThresholdBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetPathLossThresholdBufferSize(&pConnectionEvent->eventData.pathLossThreshold);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtTransmitPowerReportingBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtTransmitPowerReporting_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtTransmitPowerReportingBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetTransmitPowerReportingBufferSize(&pConnectionEvent->eventData.transmitPowerReporting);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtEnhancedReadTransmitPowerLevelBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtEnhancedReadTransmitPowerLevel_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtEnhancedReadTransmitPowerLevelBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetTransmitPowerInfoBufferSize(&pConnectionEvent->eventData.transmitPowerInfo);
}
#if defined(gEATT_d) && (gEATT_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtEattConnectionRequestBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtEattConnectionRequest_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtEattConnectionRequestBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetEattConnectionRequestBufferSize(&pConnectionEvent->eventData.eattConnectionRequest);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtEattConnectionCompleteBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtEattConnectionComplete_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtEattConnectionCompleteBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetEattConnectionCompleteBufferSize(&pConnectionEvent->eventData.eattConnectionComplete);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtEattChannelReconfigureResponseBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtEattChannelReconfigureResponse_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtEattChannelReconfigureResponseBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetEattReconfigureResponseBufferSize(&pConnectionEvent->eventData.eattReconfigureResponse);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnEvtEattBearerStatusNotificationBufferSize(
*                                       gapConnectionEvent_t *pConnectionEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtEattBearerStatusNotification_c event.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnEvtEattBearerStatusNotificationBufferSize
(
    gapConnectionEvent_t *pConnectionEvent
)
{
    return fsciBleGapGetEattBearerStatusNotificationBufferSize(&pConnectionEvent->eventData.eattBearerStatusNotification);
}
#endif /* gEATT_d */
#endif /* gBLE52_d */

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtConnected(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the connectedEvent data fields in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtConnected
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromConnectedEvent(&pConnectionEvent->eventData.connectedEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromPairingParams(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the pairingEvent data fields in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromPairingParams
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromPairingParameters(&pConnectionEvent->eventData.pairingEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtPeripheralSecurityRequest(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the peripheralSecurityRequestEvent data fields in the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtPeripheralSecurityRequest
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromPeripheralSecurityRequestParameters(&pConnectionEvent->eventData.peripheralSecurityRequestEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtAuthenticationRejected(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the authenticationRejectedEvent data fields in the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtAuthenticationRejected
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromAuthenticationRejectedEvent(&pConnectionEvent->eventData.authenticationRejectedEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtPasskeyDisplay(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the passkey value in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtPasskeyDisplay
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetBufferFromUint32Value(pConnectionEvent->eventData.passkeyForDisplay, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtKeyExchangeRequest(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the keyExchangeRequestEvent data fields in the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtKeyExchangeRequest
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromKeyExchangeRequestEvent(&pConnectionEvent->eventData.keyExchangeRequestEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtKeysReceived(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the keysReceivedEvent data fields in the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtKeysReceived
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromKeysReceivedEvent(&pConnectionEvent->eventData.keysReceivedEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtLongTermKeyRequest(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the longTermKeyRequestEvent data fields in the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtLongTermKeyRequest
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromLongTermKeyRequestEvent(&pConnectionEvent->eventData.longTermKeyRequestEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtEncryptionChanged(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the encryptionChangedEvent data fields in the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtEncryptionChanged
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromEncryptionChangedEvent(&pConnectionEvent->eventData.encryptionChangedEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtPairingComplete(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the pairingCompleteEvent data fields in the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtPairingComplete
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromPairingCompleteEvent(&pConnectionEvent->eventData.pairingCompleteEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtDisconnected(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the disconnectedEvent data fields in the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtDisconnected
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromDisconnectedEvent(&pConnectionEvent->eventData.disconnectedEvent, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtRssiRead(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the RSSI value in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtRssiRead
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetBufferFromUint8ValueSigned(pConnectionEvent->eventData.rssi_dBm, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtTxPowerLevelRead(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the Tx power level value in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtTxPowerLevelRead
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetBufferFromUint8ValueSigned(pConnectionEvent->eventData.txPowerLevel_dBm, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtPowerReadFailure(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the failReason value in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtPowerReadFailure
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetBufferFromEnumValue(pConnectionEvent->eventData.failReason, *ppBuffer, bleResult_t);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtParameterUpdateRequest(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the connectionUpdateRequest data fields in the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtParameterUpdateRequest
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
   fsciBleGapGetBufferFromConnParameterUpdateRequest(&pConnectionEvent->eventData.connectionUpdateRequest, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtParameterUpdateComplete(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the connectionUpdateComplete data fields in the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtParameterUpdateComplete
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBuffFromConnParameterUpdateComplete(&pConnectionEvent->eventData.connectionUpdateComplete, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtLeSetDataLengthFailure(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the leDataLengthChanged data fields in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtLeSetDataLengthFailure
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromConnLeSetDataLengthChangedFailure(pConnectionEvent->eventData.failReason, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtLeDataLengthChanged(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the leDataLengthChanged data fields in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtLeDataLengthChanged
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromConnLeDataLengthChanged(&pConnectionEvent->eventData.leDataLengthChanged, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtLeScDisplayNumericValue(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the numeric value in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtLeScDisplayNumericValue
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetBufferFromUint32Value(pConnectionEvent->eventData.numericValueForDisplay, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtLeScKeypressNotification(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the keypress notification value in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtLeScKeypressNotification
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetBufferFromEnumValue(pConnectionEvent->eventData.incomingKeypressNotification, *ppBuffer, gapKeypressNotification_t);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtChannelMapRead(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the channel map array in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the array
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtChannelMapRead
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetBufferFromArray(pConnectionEvent->eventData.channelMap, *ppBuffer, ((uint32_t)gcBleChannelMapSize_c));
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtChannelMapReadFailure(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the failReason value in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtChannelMapReadFailure
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
   fsciBleGetBufferFromEnumValue(pConnectionEvent->eventData.failReason, *ppBuffer, bleResult_t);
}

#if defined(gBLE51_d) && (gBLE51_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtIqReportReceived(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the connIqReport data fields in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtIqReportReceived
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromConnIqReportReceived(&pConnectionEvent->eventData.connIqReport, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtCteRequestFailed(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the status vaue in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtCteRequestFailed
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGetBufferFromEnumValue(pConnectionEvent->eventData.cteRequestFailedEvent.status, *ppBuffer, bleResult_t);
}
#endif /* gBLE51_d */

#if defined(gBLE52_d) && (gBLE52_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtPathLossThreshold(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the pathLossThreshold data fields in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtPathLossThreshold
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromPathLossThreshold(&pConnectionEvent->eventData.pathLossThreshold, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtTransmitPowerReporting(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the transmitPowerReporting data fields in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtTransmitPowerReporting
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromTransmitPowerReporting(&pConnectionEvent->eventData.transmitPowerReporting, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtEnhancedReadTransmitPowerLevel(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the transmitPowerInfo data fields in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtEnhancedReadTransmitPowerLevel
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromTransmitPowerInfo(&pConnectionEvent->eventData.transmitPowerInfo, ppBuffer);
}
#if defined(gEATT_d) && (gEATT_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtEattConnectionRequest(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the eattConnectionRequest data fields in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtEattConnectionRequest
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromEattConnectionRequest(&pConnectionEvent->eventData.eattConnectionRequest, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtEattConnectionComplete(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the eattConnectionComplete data fields in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtEattConnectionComplete
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromEattConnectionComplete(&pConnectionEvent->eventData.eattConnectionComplete, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtEattChannelReconfigureResponse(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the eattReconfigureResponse data fields in the provided buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtEattChannelReconfigureResponse
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromEattReconfigureResponse(&pConnectionEvent->eventData.eattReconfigureResponse, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnEvtEattBearerStatusNotification(
*                                           gapConnectionEvent_t *pConnectionEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the eattBearerStatusNotification data fields in the provided
*              buffer.
*
*\param  [in]  pConnectionEvent    Pointer to the connection event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnEvtEattBearerStatusNotification
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
)
{
    fsciBleGapGetBufferFromEattBearerStatusNotification(&pConnectionEvent->eventData.eattBearerStatusNotification, ppBuffer);
}
#endif /* gEATT_d */
#endif /* gBLE52_d */

/*! *********************************************************************************
*\private
*\fn           void GetInitializationCompleteEventFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the initCompleteData data fields from the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetInitializationCompleteEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetUint64ValueFromBuffer(pGenericEvent->eventData.initCompleteData.supportedFeatures, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pGenericEvent->eventData.initCompleteData.maxAdvDataSize, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.initCompleteData.numOfSupportedAdvSets, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.initCompleteData.periodicAdvListSize, *ppBuffer);  
}

/*! *********************************************************************************
*\private
*\fn           void GetInternalErrorEventFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the internalError data fields from the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetInternalErrorEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGapGetInternalErrorFromBuffer(&pGenericEvent->eventData.internalError, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetAdvertisingSetupFailedEventFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the setupFailError value from the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetAdvertisingSetupFailedEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetEnumValueFromBuffer(pGenericEvent->eventData.setupFailError, *ppBuffer, bleResult_t);
}

/*! *********************************************************************************
*\private
*\fn           void GetFilterAcceptListSizeReadEventFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the filterAcceptListSize value from the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetFilterAcceptListSizeReadEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.filterAcceptListSize, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetRandomAddressReadyEventFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the addrReady data fields from the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetRandomAddressReadyEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetAddressFromBuffer(pGenericEvent->eventData.addrReady.aAddress, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.addrReady.advHandle, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetPublicAddressReadEventFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the aAddress value from the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetPublicAddressReadEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetAddressFromBuffer(pGenericEvent->eventData.aAddress, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetAdvTxPowerLevelFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the advTxPowerLevel value from the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetAdvTxPowerLevelFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetUint8ValueFromBufferSigned(pGenericEvent->eventData.advTxPowerLevel_dBm, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetPrivateResolvableAddressVerifiedFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the verified value from the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetPrivateResolvableAddressVerifiedFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBoolValueFromBuffer(pGenericEvent->eventData.verified, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetRandomAddressSetFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the advHandle value from the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetRandomAddressSetFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.advHandle, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetLeScLocalOobDataEventFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the localOobData data fields from the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetLeScLocalOobDataEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetArrayFromBuffer(pGenericEvent->eventData.localOobData.randomValue, *ppBuffer, ((uint32_t)gSmpLeScRandomValueSize_c));
    fsciBleGetArrayFromBuffer(pGenericEvent->eventData.localOobData.confirmValue, *ppBuffer, ((uint32_t)gSmpLeScRandomConfirmValueSize_c));
}

/*! *********************************************************************************
*\private
*\fn           void GetHostPrivacyStateChangedEventFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the newHostPrivacyState value from the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetHostPrivacyStateChangedEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBoolValueFromBuffer(pGenericEvent->eventData.newHostPrivacyState, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetControllerPrivacyStateChangedEventFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the newControllerPrivacyState value from the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetControllerPrivacyStateChangedEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBoolValueFromBuffer(pGenericEvent->eventData.newControllerPrivacyState, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetTxPowerLevelSetCompleteEventFromBuffer(
*                                            gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the txPowerLevelSetStatus value from the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetTxPowerLevelSetCompleteEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetEnumValueFromBuffer(pGenericEvent->eventData.txPowerLevelSetStatus, *ppBuffer, bleResult_t);
}

/*! *********************************************************************************
*\private
*\fn           void GetLePhyEventFromBuffer(gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the phyEvent data fields from the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetLePhyEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetEnumValueFromBuffer(pGenericEvent->eventData.phyEvent.phyEventType, *ppBuffer, gapPhyEventType_t);
    if( gPhySetDefaultComplete_c != pGenericEvent->eventData.phyEvent.phyEventType )
    {
        fsciBleGetDeviceIdFromBuffer(&pGenericEvent->eventData.phyEvent.deviceId, ppBuffer);
        fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.phyEvent.txPhy, *ppBuffer);
        fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.phyEvent.rxPhy, *ppBuffer);
    }
}

/*! *********************************************************************************
*\private
*\fn           void GetControllerNotificationEventFromBuffer(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the notifEvent data fields from the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetControllerNotificationEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetEnumValueFromBuffer(pGenericEvent->eventData.notifEvent.eventType, *ppBuffer, bleNotificationEventType_t);
    fsciBleGetDeviceIdFromBuffer(&pGenericEvent->eventData.notifEvent.deviceId, ppBuffer);
    fsciBleGetUint8ValueFromBufferSigned(pGenericEvent->eventData.notifEvent.rssi, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.notifEvent.channel, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pGenericEvent->eventData.notifEvent.ce_counter, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pGenericEvent->eventData.notifEvent.status, *ppBuffer, bleResult_t);
    fsciBleGetUint32ValueFromBuffer(pGenericEvent->eventData.notifEvent.timestamp, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.notifEvent.adv_handle, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBondCreatedEventFromBuffer(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the bondCreatedEvent data fields from the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetBondCreatedEventFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.bondCreatedEvent.nvmIndex, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pGenericEvent->eventData.bondCreatedEvent.addressType, *ppBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(pGenericEvent->eventData.bondCreatedEvent.address, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetTxEntryAvailableFromBuffer(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the deviceId value for the gTxEntryAvailable_c generic
*              event from the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetTxEntryAvailableFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.deviceId, *ppBuffer);
}

#if defined(gBLE51_d) && (gBLE51_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void GetConnectionlessIqSamplingStateChangedFromBuffer(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the syncHandle value from the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetConnectionlessIqSamplingStateChangedFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetUint16ValueFromBuffer(pGenericEvent->eventData.syncHandle, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetAntennaInformationReadFromBuffer(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Extracts the antennaInformation data fields from the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event that should
*                                  be filled.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be extracted from.
*
*\retval       void.
********************************************************************************** */
static void GetAntennaInformationReadFromBuffer
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetEnumValueFromBuffer(pGenericEvent->eventData.antennaInformation.supportedSwitchingSamplingRates, *ppBuffer, bleSupportedSwitchingSamplingRates_t);
    fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.antennaInformation.numAntennae, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.antennaInformation.maxSwitchingPatternLength, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pGenericEvent->eventData.antennaInformation.maxCteLength, *ppBuffer);
}
#endif /* gBLE51_d */

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromInitializationCompleteEvent(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the initCompleteData data fields in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromInitializationCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint64Value(pGenericEvent->eventData.initCompleteData.supportedFeatures, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.initCompleteData.maxAdvDataSize, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.initCompleteData.numOfSupportedAdvSets, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.initCompleteData.periodicAdvListSize, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromInternalErrorEvent(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the internalError data fields in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromInternalErrorEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGapGetBufferFromInternalError(&pGenericEvent->eventData.internalError, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromAdvertisingSetupFailedEvent(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the setupFailError value in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromAdvertisingSetupFailedEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.setupFailError, *ppBuffer, bleResult_t);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromFilterAcceptListSizeReadEvent(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the filterAcceptListSize value in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromFilterAcceptListSizeReadEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.filterAcceptListSize, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromRandomAddressReadyEvent(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the addrReady data fields in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromRandomAddressReadyEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromAddress(pGenericEvent->eventData.addrReady.aAddress, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.addrReady.advHandle, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromPublicAddressReadEvent(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the aAddress value in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromPublicAddressReadEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromAddress(pGenericEvent->eventData.aAddress, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromAdvTxPowerLevel(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the advTxPowerLevel value in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromAdvTxPowerLevel
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint8ValueSigned(pGenericEvent->eventData.advTxPowerLevel_dBm, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromPrivateResolvableAddressVerified(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the verified value in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromPrivateResolvableAddressVerified
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromBoolValue(pGenericEvent->eventData.verified, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromRandomAddressSet(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the advHandle value in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromRandomAddressSet
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.advHandle, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromLeScLocalOobDataEvent(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the localOobData data fields in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromLeScLocalOobDataEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromArray(pGenericEvent->eventData.localOobData.randomValue, *ppBuffer, ((uint32_t)gSmpLeScRandomValueSize_c));
    fsciBleGetBufferFromArray(pGenericEvent->eventData.localOobData.confirmValue, *ppBuffer, ((uint32_t)gSmpLeScRandomConfirmValueSize_c));
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromHostPrivacyStateChangedEvent(
*                                           gapGenericEvent_t   *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the newHostPrivacyState value in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromHostPrivacyStateChangedEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromBoolValue(pGenericEvent->eventData.newHostPrivacyState, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromControllerPrivacyStateChangedEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the newControllerPrivacyState value in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromControllerPrivacyStateChangedEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromBoolValue(pGenericEvent->eventData.newControllerPrivacyState, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromTxPowerLevelSetCompleteEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the txPowerLevelSetStatus value in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromTxPowerLevelSetCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.txPowerLevelSetStatus, *ppBuffer, bleResult_t);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromLePhyEvent(gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the phyEvent data fields in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromLePhyEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.phyEvent.phyEventType, *ppBuffer, gapPhyEventType_t);
    if( gPhySetDefaultComplete_c != pGenericEvent->eventData.phyEvent.phyEventType )
    {
        fsciBleGetBufferFromDeviceId(&pGenericEvent->eventData.phyEvent.deviceId, ppBuffer);
        fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.phyEvent.txPhy, *ppBuffer);
        fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.phyEvent.rxPhy, *ppBuffer);
    }
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromControllerNotificationEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the notifEvent data fields in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromControllerNotificationEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.notifEvent.eventType, *ppBuffer, bleNotificationEventType_t);
    fsciBleGetBufferFromDeviceId(&pGenericEvent->eventData.notifEvent.deviceId, ppBuffer);
    fsciBleGetBufferFromUint8ValueSigned(pGenericEvent->eventData.notifEvent.rssi, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.notifEvent.channel, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.notifEvent.ce_counter, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.notifEvent.status, *ppBuffer, bleResult_t);
    fsciBleGetBufferFromUint32Value(pGenericEvent->eventData.notifEvent.timestamp, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.notifEvent.adv_handle, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromBondCreatedEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the bondCreatedEvent data fields in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromBondCreatedEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.bondCreatedEvent.nvmIndex, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.bondCreatedEvent.addressType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(pGenericEvent->eventData.bondCreatedEvent.address, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromGetConnParamsCompleteEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the getConnParams data fields in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromGetConnParamsCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGapGetBufferFromGetConnParamsCompleteEvent(&pGenericEvent->eventData.getConnParams, ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromTxEntryAvailable(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the deviceId value from the gTxEntryAvailable_c event in
*              the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromTxEntryAvailable
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.deviceId, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromControllerLocalRPAReadEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the aControllerLocalRPA value in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromControllerLocalRPAReadEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromAddress(pGenericEvent->eventData.aControllerLocalRPA, *ppBuffer);
}

#if defined(gBLE51_d) && (gBLE51_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void GetBufferFromConnectionlessIqSamplingStateChanged(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the syncHandle value in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromConnectionlessIqSamplingStateChanged
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.syncHandle, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromAntennaInformationRead(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the antennaInformation data fields in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromAntennaInformationRead
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.antennaInformation.supportedSwitchingSamplingRates, *ppBuffer, bleSupportedSwitchingSamplingRates_t);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.antennaInformation.numAntennae, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.antennaInformation.maxSwitchingPatternLength, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.antennaInformation.maxCteLength, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromPeriodicAdvRecvEnableCompleteEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the perAdvSyncTransferEnable value in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromPeriodicAdvRecvEnableCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint8Value((uint8_t)pGenericEvent->eventData.perAdvSyncTransferEnable, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromPeriodicAdvSyncTransferComplete(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the perAdvSyncTransfer data fields in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromPeriodicAdvSyncTransferComplete
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint8Value((uint8_t)pGenericEvent->eventData.perAdvSyncTransfer.status, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.perAdvSyncTransfer.deviceId, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromPeriodicAdvSetInfoTransferCompleteEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the perAdvSetInfoTransfer data fields in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromPeriodicAdvSetInfoTransferCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint8Value((uint8_t)pGenericEvent->eventData.perAdvSetInfoTransfer.status, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.perAdvSetInfoTransfer.deviceId, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromSetPeriodicAdvSyncTransferParamsCompleteEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the perAdvSetDefaultPerAdvSyncTransferParams value
*              in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromSetPeriodicAdvSyncTransferParamsCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint8Value((uint8_t)pGenericEvent->eventData.perAdvSetDefaultPerAdvSyncTransferParams, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromSetDefaultPeriodicAdvSyncTransferParamsCompleteEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the perAdvSetDefaultPerAdvSyncTransferParams value
*              in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the value
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromSetDefaultPeriodicAdvSyncTransferParamsCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint8Value((uint8_t)pGenericEvent->eventData.perAdvSetDefaultPerAdvSyncTransferParams, *ppBuffer);
}
                                   
/*! *********************************************************************************
*\private
*\fn           void GetBufferFromPerAdvSyncTransferReceived(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the perAdvSyncTransferReceived data fields in the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromPerAdvSyncTransferReceived
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint8Value((uint8_t)pGenericEvent->eventData.perAdvSyncTransferReceived.status, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.perAdvSyncTransferReceived.deviceId, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.perAdvSyncTransferReceived.serviceData, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.perAdvSyncTransferReceived.syncHandle, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.perAdvSyncTransferReceived.advSID, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.perAdvSyncTransferReceived.advAddressType, *ppBuffer);
    fsciBleGetBufferFromAddress(pGenericEvent->eventData.perAdvSyncTransferReceived.advAddress, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.perAdvSyncTransferReceived.advPhy, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.perAdvSyncTransferReceived.periodicAdvInt, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.perAdvSyncTransferReceived.advClockAccuracy, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromLeGenerateDhKeyCompleteEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the aDHKey array in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the array
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromLeGenerateDhKeyCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromArray(pGenericEvent->eventData.leGenerateDhKeyCompleteEvent.aDHKey, *ppBuffer, ((uint32_t)gDHKeySize_c));
}
#endif /* gBLE51_d */

/*! *********************************************************************************
*\private
*\fn           void GetBufferFromRemoteVersionInformationReadEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the gapRemoteVersionInfoRead data fields in the provided buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
static void GetBufferFromRemoteVersionInformationReadEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromEnumValue(pGenericEvent->eventData.gapRemoteVersionInfoRead.status, *ppBuffer, bleResult_t);
    fsciBleGetBufferFromDeviceId(&pGenericEvent->eventData.gapRemoteVersionInfoRead.deviceId, ppBuffer);
    fsciBleGetBufferFromUint8Value(pGenericEvent->eventData.gapRemoteVersionInfoRead.version, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.gapRemoteVersionInfoRead.manufacturerName, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pGenericEvent->eventData.gapRemoteVersionInfoRead.subversion, *ppBuffer);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetInitializationCompleteEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gInitializationComplete_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetInitializationCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint64_t) + sizeof(uint16_t) + 2U * sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetInternalErrorEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the gInternalError_c
*              event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetInternalErrorEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return fsciBleGapGetInternalErrorBufferSize(&pGenericEvent->internalError);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetAdvertisingSetupFailedEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gAdvertisingSetupFailed_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetAdvertisingSetupFailedEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(bleResult_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetFilterAcceptListSizeReadEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gFilterAcceptListSizeRead_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetFilterAcceptListSizeReadEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetRandomAddressReadyEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the gRandomAddressReady_c
*              event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetRandomAddressReadyEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return gcBleDeviceAddressSize_c + sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetPublicAddressReadEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the gPublicAddressRead_c
*              event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetPublicAddressReadEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return gcBleDeviceAddressSize_c;
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetAdvTxPowerLevelBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the gAdvTxPowerLevelRead_c
*              and the gExtAdvertisingParametersSetupComplete_c events.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetAdvTxPowerLevelBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(int8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetPrivateResolvableAddressVerifiedBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gPrivateResolvableAddressVerified_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetPrivateResolvableAddressVerifiedBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(bool_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetRandomAddressSetBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the gRandomAddressSet_c
*              event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetRandomAddressSetBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetLeScLocalOobDataEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the gLeScLocalOobData_c
*              event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetLeScLocalOobDataEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return gSmpLeScRandomValueSize_c + gSmpLeScRandomConfirmValueSize_c;
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetHostPrivacyStateChangedEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gHostPrivacyStateChanged_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetHostPrivacyStateChangedEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(bool_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetControllerPrivacyStateChangedEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gControllerPrivacyStateChanged_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetControllerPrivacyStateChangedEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(bool_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetTxPowerLevelSetCompleteEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gTxPowerLevelSetComplete_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetTxPowerLevelSetCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(bleResult_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetLePhyEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the gLePhyEvent_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetLePhyEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(gapPhyEventType_t) + 3U * sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetControllerNotificationEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gControllerNotificationEvent_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetControllerNotificationEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint16_t) + sizeof(deviceId_t) + sizeof(int8_t) +
           sizeof(uint8_t) + sizeof(uint16_t) + sizeof(bleResult_t) +
           sizeof(uint32_t) + sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetBondCreatedEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the gBondCreatedEvent_c
*              event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetBondCreatedEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint8_t) + sizeof(bleAddressType_t) + gcBleDeviceAddressSize_c;
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetGetConnParamsCompleteEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gGetConnParamsComplete_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetGetConnParamsCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return 6U * sizeof(uint16_t) + 3U * sizeof(uint32_t) + 15U * sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetTxEntryAvailableBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gTxEntryAvailable_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetTxEntryAvailableBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetControllerLocalRPAReadEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gControllerLocalRPARead_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetControllerLocalRPAReadEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return gcBleDeviceAddressSize_c;
}

#if defined(gBLE51_d) && (gBLE51_d == 1U)
/*! *********************************************************************************
*\private
*\fn           uint32_t GetConnectionlessIqSamplingStateChangedBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnectionlessIqSamplingStateChanged_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetConnectionlessIqSamplingStateChangedBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint16_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetAntennaInformationReadBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gAntennaInformationRead_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetAntennaInformationReadBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(bleSupportedSwitchingSamplingRates_t) + sizeof(uint8_t)
                              + sizeof(uint8_t) + sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetPeriodicAdvRecvEnableCompleteEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gPeriodicAdvRecvEnableComplete_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetPeriodicAdvRecvEnableCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetPeriodicAdvSyncTransferCompleteBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gPeriodicAdvSyncTransferComplete_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetPeriodicAdvSyncTransferCompleteBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint8_t) + sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetPeriodicAdvSetInfoTransferCompleteEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gPeriodicAdvSetInfoTransferComplete_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetPeriodicAdvSetInfoTransferCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint8_t) + sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetSetPeriodicAdvSyncTransferParamsCompleteEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gSetPeriodicAdvSyncTransferParamsComplete_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetSetPeriodicAdvSyncTransferParamsCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint8_t) + sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetSetDefaultPeriodicAdvSyncTransferParamsCompleteEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gSetDefaultPeriodicAdvSyncTransferParamsComplete_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetSetDefaultPeriodicAdvSyncTransferParamsCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetPerAdvSyncTransferReceivedBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gPeriodicAdvSyncTransferSucceeded_c and the
*              gPeriodicAdvSyncTransferFailed_c events.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetPerAdvSyncTransferReceivedBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint16_t) +
           sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t) +
           gcBleDeviceAddressSize_c + sizeof(uint8_t) +
           sizeof(uint16_t) + sizeof(uint8_t);
}

/*! *********************************************************************************
*\private
*\fn           uint32_t GetLeGenerateDhKeyCompleteEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gConnEvtLeGenerateDhKeyComplete_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetLeGenerateDhKeyCompleteEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return gDHKeySize_c;
}
#endif /* gBLE51_d */

/*! *********************************************************************************
*\private
*\fn           uint32_t GetRemoteVersionInformationReadEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gRemoteVersionInformationRead_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
static uint32_t GetRemoteVersionInformationReadEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(bleResult_t) + sizeof(deviceId_t) +
           sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t);
}

#endif /* gFsciIncluded_c && gFsciBleGapLayerEnabled_d */

#if gFsciBleBBox_d || gFsciBleTest_d
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdRegisterDeviceSecurityRequirementsOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdRegisterDeviceSecurityRequirementsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdRegisterDeviceSecurityRequirementsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bool_t                              bDeviceSecurityRequirementsIncluded = FALSE;
    gapDeviceSecurityRequirements_t*    pDeviceSecurityRequirements = NULL;
    bleResult_t                         status                      = gBleSuccess_c;

    /* Get from buffer the boolean value which indicates if device security
    requirements are included in the request */
    fsciBleGetBoolValueFromBuffer(bDeviceSecurityRequirementsIncluded, pBuffer);

    /* Verify if device security requirements are included in the request or not */
    if(TRUE == bDeviceSecurityRequirementsIncluded)
    {
        /* Allocate buffer for the device security requirements */
        pDeviceSecurityRequirements = fsciBleGapAllocDeviceSecurityRequirementsForBuffer(pBuffer);

        if(NULL == pDeviceSecurityRequirements)
        {
            /* No memory */
            status = gBleOverflow_c;
        }
        else
        {
            /* Get the device security requirements from buffer */
            fsciBleGapGetDeviceSecurityRequirementsFromBuffer(pDeviceSecurityRequirements, &pBuffer);
        }
    }

    if(gBleSuccess_c == status)
    {
        fsciBleGapCallApiFunction(Gap_RegisterDeviceSecurityRequirements(pDeviceSecurityRequirements));

        /* Free the buffer allocated for device security requirements */
        (void)fsciBleGapFreeDeviceSecurityRequirements(pDeviceSecurityRequirements);
    }
    else
    {
        /* The GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetAdvertisingParametersOpCode(
*                                                  uint8_t *pBuffer,
*                                                  uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetAdvertisingParametersOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdSetAdvertisingParametersOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapAdvertisingParameters_t advertisingParameters = {0};

    /* Get advertising parameters from buffer */
    fsciBleGapGetAdvertisingParametersFromBuffer(&advertisingParameters, &pBuffer);

    fsciBleGapCallApiFunction(Gap_SetAdvertisingParameters(&advertisingParameters));
}

#if defined(gBLE50_d) && (gBLE50_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetExtAdvertisingParametersOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetExtAdvertisingParametersOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdSetExtAdvertisingParametersOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapExtAdvertisingParameters_t advertisingParameters = {0};

    /* Get advertising parameters from buffer */
    fsciBleGapGetExtAdvertisingParametersFromBuffer(&advertisingParameters, &pBuffer);

    fsciBleGapCallApiFunction(Gap_SetExtAdvertisingParameters(&advertisingParameters));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetExtAdvertisingDataOpCode(uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetExtAdvertisingDataOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdSetExtAdvertisingDataOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t                 handle = 0x0U;
    bool_t                  bAdvertisingDataIncluded = FALSE;
    gapAdvertisingData_t*   pAdvertisingData    = NULL;
    bool_t                  bScanResponseDataIncluded = FALSE;
    gapScanResponseData_t*  pScanResponseData   = NULL;
    bleResult_t             status              = gBleSuccess_c;

    fsciBleGetUint8ValueFromBuffer(handle, pBuffer);

    /* Get from buffer the boolean value which indicates if advertising
    data is included in the request */
    fsciBleGetBoolValueFromBuffer(bAdvertisingDataIncluded, pBuffer);

    /* Verify if advertising data is included in the request or not */
    if(TRUE == bAdvertisingDataIncluded)
    {
        /* Allocate buffer for the advertising data */
        pAdvertisingData = fsciBleGapAllocAdvertisingDataForBuffer(pBuffer);

        if(NULL == pAdvertisingData)
        {
            /* No memory */
            status = gBleOverflow_c;
        }
        else
        {
            /* Get the advertising data from buffer */
            fsciBleGapGetAdvertisingDataFromBuffer(pAdvertisingData, &pBuffer);
        }
    }

    if(gBleSuccess_c == status)
    {
        /* Get from buffer the boolean value which indicates if scan response
        data is included in the request */
        fsciBleGetBoolValueFromBuffer(bScanResponseDataIncluded, pBuffer);

        /* Verify if scan response data is included in the request or not */
        if(TRUE == bScanResponseDataIncluded)
        {
            /* Allocate buffer for the scan response data */
            pScanResponseData = fsciBleGapAllocScanResponseDataForBuffer(pBuffer);

            if(NULL == pScanResponseData)
            {
                /* No memory */
                status = gBleOverflow_c;
                /* Free the buffer allocated for advertising data */
                (void)fsciBleGapFreeAdvertisingData(pAdvertisingData);
            }
            else
            {
                /* Get the scan response data from buffer */
                fsciBleGapGetScanResponseDataFromBuffer(pScanResponseData, &pBuffer);
            }
        }
    }

    if(gBleSuccess_c == status)
    {
        fsciBleGapCallApiFunction(Gap_SetExtAdvertisingData(handle, pAdvertisingData, pScanResponseData));

        /* Free the buffers allocated for advertising and scan response data */
        (void)fsciBleGapFreeAdvertisingData(pAdvertisingData);
        (void)fsciBleGapFreeScanResponseData(pScanResponseData);
    }
    else
    {
        /* The GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetPeriodicAdvDataOpCode(uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetPeriodicAdvDataOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdSetPeriodicAdvDataOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t                 handle = 0x0U;
    bool_t                  bUpdateDID = FALSE;
    bleResult_t             status = gBleSuccess_c;
    gapAdvertisingData_t*   pAdvertisingData = NULL;

    fsciBleGetUint8ValueFromBuffer(handle, pBuffer);
    fsciBleGetBoolValueFromBuffer(bUpdateDID, pBuffer);

    /* Allocate buffer for the advertising data */
    pAdvertisingData = fsciBleGapAllocAdvertisingDataForBuffer(pBuffer);

    if(NULL == pAdvertisingData)
    {
        /* No memory */
        status = gBleOverflow_c;
    }
    else
    {
        /* Get the advertising data from buffer */
        fsciBleGapGetAdvertisingDataFromBuffer(pAdvertisingData, &pBuffer);
    }

    if (gBleSuccess_c == status)
    {
        fsciBleGapCallApiFunction(Gap_SetPeriodicAdvertisingData(handle, pAdvertisingData, bUpdateDID));

        /* Free the buffers allocated for advertising and scan response data */
        (void)fsciBleGapFreeAdvertisingData(pAdvertisingData);
    }
    else
    {
        /* The GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}

#endif
#if defined(gBLE51_d) && (gBLE51_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetConnectionlessCteTransmitParametersOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetConnectionlessCteTransmitParametersOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdSetConnectionlessCteTransmitParametersOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapConnectionlessCteTransmitParams_t *pTransmitParams = NULL;
    uint8_t handle = 0U;
    uint8_t cteLength = 0U;
    bleCteType_t cteType = (uint8_t)gCteTypeAoA_c;
    uint8_t cteCount = 0U;
    uint8_t switchingPatternLength = 0U;

    /* Extract fields from buffer */
    fsciBleGetUint8ValueFromBuffer(handle, pBuffer);
    fsciBleGetUint8ValueFromBuffer(cteLength, pBuffer);
    fsciBleGetEnumValueFromBuffer(cteType, pBuffer, bleCteType_t);
    fsciBleGetUint8ValueFromBuffer(cteCount, pBuffer);
    fsciBleGetUint8ValueFromBuffer(switchingPatternLength, pBuffer);

    /* Allocate buffer for params struct */
    pTransmitParams = MEM_BufferAlloc(sizeof(gapConnectionlessCteTransmitParams_t) + switchingPatternLength);

    if(NULL == pTransmitParams)
    {
        /* No memory => The GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        pTransmitParams->handle = handle;
        pTransmitParams->cteLength = cteLength;
        pTransmitParams->cteCount = cteCount;
        pTransmitParams->switchingPatternLength = switchingPatternLength;

        /* Get array of antenna IDs from buffer */
        fsciBleGetArrayFromBuffer(pTransmitParams->aAntennaIds, pBuffer, switchingPatternLength);

        fsciBleGapCallApiFunction(Gap_SetConnectionlessCteTransmitParameters(pTransmitParams));

        /* Free the buffer allocated */
        (void)MEM_BufferFree(pTransmitParams);
    }
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEnableConnectionlessCteTransmitOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEnableConnectionlessCteTransmitOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */    
void HandleGapCmdEnableConnectionlessCteTransmitOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t handle = 0x0U;
    bleCteTransmitEnable_t enable = gCteTransmitDisable_c;

    fsciBleGetUint8ValueFromBuffer(handle, pBuffer);
    fsciBleGetEnumValueFromBuffer(enable, pBuffer, bleCteTransmitEnable_t);

    fsciBleGapCallApiFunction(Gap_EnableConnectionlessCteTransmit(handle, enable));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEnableConnectionlessIqSamplingOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEnableConnectionlessIqSamplingOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */    
void HandleGapCmdEnableConnectionlessIqSamplingOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapConnectionlessIqSamplingParams_t *pSamplingParams = NULL;
    uint16_t syncHandle = 0x0U;
    bleIqSamplingEnable_t iqSamplingEnable = gIqSamplingEnable_c;
    bleSlotDurations_t slotDurations = (uint8_t)gSlotDurations1us_c;
    uint8_t maxSampledCtes = 0U;
    uint8_t switchingPatternLength = 0U;

    /* Extract fields from buffer */
    fsciBleGetUint16ValueFromBuffer(syncHandle, pBuffer);
    fsciBleGetEnumValueFromBuffer(iqSamplingEnable, pBuffer, bleIqSamplingEnable_t);
    fsciBleGetEnumValueFromBuffer(slotDurations, pBuffer, bleSlotDurations_t);
    fsciBleGetUint8ValueFromBuffer(maxSampledCtes, pBuffer);
    fsciBleGetUint8ValueFromBuffer(switchingPatternLength, pBuffer);

    /* Allocate buffer for params struct */
    if ((gGapMinSwitchingPatternLength_c <= switchingPatternLength) && (switchingPatternLength <= gGapMaxSwitchingPatternLength_c))
    {
        pSamplingParams = MEM_BufferAlloc(sizeof(gapConnectionlessIqSamplingParams_t) + switchingPatternLength);
    }

    if(NULL == pSamplingParams)
    {
        /* No memory => The GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        pSamplingParams->iqSamplingEnable = iqSamplingEnable;
        pSamplingParams->slotDurations = slotDurations;
        pSamplingParams->maxSampledCtes = maxSampledCtes;
        pSamplingParams->switchingPatternLength = switchingPatternLength;

        /* Get array of antenna IDs from buffer */
        fsciBleGetArrayFromBuffer(pSamplingParams->aAntennaIds, pBuffer, switchingPatternLength);

        fsciBleGapCallApiFunction(Gap_EnableConnectionlessIqSampling(syncHandle, pSamplingParams));

        /* Free the buffer allocated */
        (void)MEM_BufferFree(pSamplingParams);
    }
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetConnectionCteReceiveParametersOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetConnectionCteReceiveParametersOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */    
void HandleGapCmdSetConnectionCteReceiveParametersOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    gapConnectionCteReceiveParams_t *pReceiveParams = NULL;
    bleIqSamplingEnable_t iqSamplingEnable = gIqSamplingEnable_c;
    bleSlotDurations_t slotDurations = (uint8_t)gSlotDurations1us_c;
    uint8_t switchingPatternLength = 0U;

    /* Extract fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(iqSamplingEnable, pBuffer, bleIqSamplingEnable_t);
    fsciBleGetEnumValueFromBuffer(slotDurations, pBuffer, bleSlotDurations_t);
    fsciBleGetUint8ValueFromBuffer(switchingPatternLength, pBuffer);

    /* Allocate buffer for params struct */
    pReceiveParams = MEM_BufferAlloc(sizeof(gapConnectionCteReceiveParams_t) + switchingPatternLength);

    if(NULL == pReceiveParams)
    {
        /* No memory => The GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        pReceiveParams->iqSamplingEnable = iqSamplingEnable;
        pReceiveParams->slotDurations = slotDurations;
        pReceiveParams->switchingPatternLength = switchingPatternLength;

        /* Get array of antenna IDs from buffer */
        fsciBleGetArrayFromBuffer(pReceiveParams->aAntennaIds, pBuffer, switchingPatternLength);

        fsciBleGapCallApiFunction(Gap_SetConnectionCteReceiveParameters(deviceId, pReceiveParams));

        /* Free the buffer allocated */
        (void)MEM_BufferFree(pReceiveParams);
    }

}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetConnectionCteTransmitParametersOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetConnectionCteTransmitParametersOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */    
void HandleGapCmdSetConnectionCteTransmitParametersOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    gapConnectionCteTransmitParams_t *pTransmitParams = NULL;
    bleCteAllowedTypesMap_t cteTypes = {0};
    uint8_t switchingPatternLength = 0U;

    /* Extract fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(cteTypes, pBuffer, bleCteAllowedTypesMap_t);
    fsciBleGetUint8ValueFromBuffer(switchingPatternLength, pBuffer);

    /* Allocate buffer for params struct */
    pTransmitParams = MEM_BufferAlloc(sizeof(gapConnectionCteTransmitParams_t) + switchingPatternLength);

    if(NULL == pTransmitParams)
    {
        /* No memory => The GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        pTransmitParams->cteTypes = cteTypes;
        pTransmitParams->switchingPatternLength = switchingPatternLength;

        /* Get array of antenna IDs from buffer */
        fsciBleGetArrayFromBuffer(pTransmitParams->aAntennaIds, pBuffer, switchingPatternLength);

        fsciBleGapCallApiFunction(Gap_SetConnectionCteTransmitParameters(deviceId, pTransmitParams));

        /* Free the buffer allocated */
        (void)MEM_BufferFree(pTransmitParams);
    }
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEnableConnectionCteRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEnableConnectionCteRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */    
void HandleGapCmdEnableConnectionCteRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    gapConnectionCteReqEnableParams_t reqEnableParams;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGapGetConnectionCteReqParametersFromBuffer(&reqEnableParams, &pBuffer);

    fsciBleGapCallApiFunction(Gap_EnableConnectionCteRequest(deviceId, &reqEnableParams));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEnableConnectionCteResponseOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEnableConnectionCteResponseOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */    
void HandleGapCmdEnableConnectionCteResponseOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    bleCteRspEnable_t enable = gCteRspDisable_c;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(enable, pBuffer, bleCteRspEnable_t);

    fsciBleGapCallApiFunction(Gap_EnableConnectionCteResponse(deviceId, enable));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdReadAntennaInformationOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdReadAntennaInformationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */    
void HandleGapCmdReadAntennaInformationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCallApiFunction(Gap_ReadAntennaInformation());
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdGenerateDhKeyV2OpCode(uint8_t *pBuffer,
*                                                     uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdGenerateDhKeyV2OpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */    
void HandleGapCmdGenerateDhKeyV2OpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    ecdhPublicKey_t remoteP256PublicKey;
    gapPrivateKeyType_t keyType = gUseGeneratedKey_c;

    fsciBleGetArrayFromBuffer(&remoteP256PublicKey, pBuffer, sizeof(ecdhPublicKey_t));
    fsciBleGetEnumValueFromBuffer(keyType, pBuffer, gapPrivateKeyType_t);
    fsciBleGapCallApiFunction(Gap_GenerateDhKeyV2(&remoteP256PublicKey, keyType));

}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdModifySleepClockAccuracy(uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdModifySleepClockAccuracy opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdModifySleepClockAccuracy(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapSleepClockAccuracy_t action = gSwitchToMoreAccurateClock_c;

    fsciBleGetEnumValueFromBuffer(action, pBuffer, gapSleepClockAccuracy_t);
    fsciBleGapCallApiFunction(Gap_ModifySleepClockAccuracy(action));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetPeriodicAdvRecvEnableOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetPeriodicAdvRecvEnableOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetPeriodicAdvRecvEnableOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t syncHandle = 0x0U;
    bool_t   bEnableDuplicateFiltering = FALSE;
    fsciBleGetUint16ValueFromBuffer(syncHandle, pBuffer);
    fsciBleGetBoolValueFromBuffer(bEnableDuplicateFiltering, pBuffer);
    fsciBleGapCallApiFunction(Gap_PeriodicAdvReceiveEnable(syncHandle, bEnableDuplicateFiltering));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetPeriodicAdvRecvDisableOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetPeriodicAdvRecvDisableOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetPeriodicAdvRecvDisableOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t syncHandle = 0x0U;
    fsciBleGetUint16ValueFromBuffer(syncHandle, pBuffer);
    fsciBleGapCallApiFunction(Gap_PeriodicAdvReceiveDisable(syncHandle));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdPeriodicAdvSyncTransferOpCode
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdPeriodicAdvSyncTransferOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdPeriodicAdvSyncTransferOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapPeriodicAdvSyncTransfer_t params = {0};
    fsciBleGapGetPeriodicAdvSncTransferParametersFromBuffer(&params, &pBuffer);
    fsciBleGapCallApiFunction(Gap_PeriodicAdvSyncTransfer(&params));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdPeriodicAdvSetInfoTransferOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdPeriodicAdvSetInfoTransferOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdPeriodicAdvSetInfoTransferOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapPeriodicAdvSetInfoTransfer_t params = {0};
    fsciBleGapGetPeriodicAdvSetInfoTransferFromBuffer(&params, &pBuffer);
    fsciBleGapCallApiFunction(Gap_PeriodicAdvSetInfoTransfer(&params));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetPeriodicAdvSyncTransferParamsOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetPeriodicAdvSyncTransferParamsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetPeriodicAdvSyncTransferParamsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapSetPeriodicAdvSyncTransferParams_t params = {0};
    fsciBleGapSetPeriodicAdvSyncTransferParamsFromBuffer(&params, &pBuffer);
    fsciBleGapCallApiFunction(Gap_SetPeriodicAdvSyncTransferParams(&params));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetDefaultPeriodicAdvSyncTransferParamsOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetDefaultPeriodicAdvSyncTransferParamsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetDefaultPeriodicAdvSyncTransferParamsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapSetPeriodicAdvSyncTransferParams_t params = {0};
    fsciBleGapSetPeriodicAdvSyncTransferParamsFromBuffer(&params, &pBuffer);
    fsciBleGapCallApiFunction(Gap_SetDefaultPeriodicAdvSyncTransferParams(&params));
}
                
#endif /* gBLE51_d */
#if defined(gBLE52_d) && (gBLE52_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEnhancedReadTransmitPowerLevelOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEnhancedReadTransmitPowerLevelOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEnhancedReadTransmitPowerLevelOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    blePowerControlPhyType_t phy;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(phy, pBuffer, blePowerControlPhyType_t);

    fsciBleGapCallApiFunction(Gap_EnhancedReadTransmitPowerLevel(deviceId, phy));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdReadRemoteTransmitPowerLevelOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdReadRemoteTransmitPowerLevelOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdReadRemoteTransmitPowerLevelOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    blePowerControlPhyType_t phy;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(phy, pBuffer, blePowerControlPhyType_t);

    fsciBleGapCallApiFunction(Gap_ReadRemoteTransmitPowerLevel(deviceId, phy));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetPathLossReportingParamsOpCode
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetPathLossReportingParamsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetPathLossReportingParamsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    gapPathLossReportingParams_t pathLossReportingParams = {0};

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGapGetPathLossReportingParametersFromBuffer(&pathLossReportingParams, &pBuffer);

    fsciBleGapCallApiFunction(Gap_SetPathLossReportingParameters(deviceId, &pathLossReportingParams));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEnablePathLossReportingOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEnablePathLossReportingOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEnablePathLossReportingOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    blePathLossReportingEnable_t enable = gPathLossReportingDisable_c;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(enable, pBuffer, blePathLossReportingEnable_t);

    fsciBleGapCallApiFunction(Gap_EnablePathLossReporting(deviceId, enable));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEnableTransmitPowerReportingOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEnableTransmitPowerReportingOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEnableTransmitPowerReportingOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    bleTxPowerReportingEnable_t localEnable = gTxPowerReportingDisable_c;
    bleTxPowerReportingEnable_t remoteEnable = gTxPowerReportingDisable_c;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(localEnable, pBuffer, bleTxPowerReportingEnable_t);
    fsciBleGetEnumValueFromBuffer(remoteEnable, pBuffer, bleTxPowerReportingEnable_t);

    fsciBleGapCallApiFunction(Gap_EnableTransmitPowerReporting(deviceId, localEnable, remoteEnable));
}
                
#endif /* gBLE52_d */
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetAdvertisingDataOpCode(uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetAdvertisingDataOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetAdvertisingDataOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bool_t                  bAdvertisingDataIncluded = FALSE;
    gapAdvertisingData_t*   pAdvertisingData    = NULL;
    bool_t                  bScanResponseDataIncluded;
    gapScanResponseData_t*  pScanResponseData   = NULL;
    bleResult_t             status              = gBleSuccess_c;

    /* Get from buffer the boolean value which indicates if advertising
    data is included in the request */
    fsciBleGetBoolValueFromBuffer(bAdvertisingDataIncluded, pBuffer);

    /* Verify if advertising data is included in the request or not */
    if(TRUE == bAdvertisingDataIncluded)
    {
        /* Allocate buffer for the advertising data */
        pAdvertisingData = fsciBleGapAllocAdvertisingDataForBuffer(pBuffer);

        if(NULL == pAdvertisingData)
        {
            /* No memory */
            status = gBleOverflow_c;
        }
        else
        {
            /* Get the advertising data from buffer */
            fsciBleGapGetAdvertisingDataFromBuffer(pAdvertisingData, &pBuffer);
        }
    }

    if(gBleSuccess_c == status)
    {
        /* Get from buffer the boolean value which indicates if scan response
        data is included in the request */
        fsciBleGetBoolValueFromBuffer(bScanResponseDataIncluded, pBuffer);

        /* Verify if scan response data is included in the request or not */
        if(TRUE == bScanResponseDataIncluded)
        {
            /* Allocate buffer for the scan response data */
            pScanResponseData = fsciBleGapAllocScanResponseDataForBuffer(pBuffer);

            if(NULL == pScanResponseData)
            {
                /* No memory */
                status = gBleOverflow_c;
                /* Free the buffer allocated for advertising data */
                (void)fsciBleGapFreeAdvertisingData(pAdvertisingData);
            }
            else
            {
                /* Get the scan response data from buffer */
                fsciBleGapGetScanResponseDataFromBuffer(pScanResponseData, &pBuffer);
            }
        }
    }

    if(gBleSuccess_c == status)
    {
        fsciBleGapCallApiFunction(Gap_SetAdvertisingData(pAdvertisingData, pScanResponseData));

        /* Free the buffers allocated for advertising and scan response data */
        (void)fsciBleGapFreeAdvertisingData(pAdvertisingData);
        (void)fsciBleGapFreeScanResponseData(pScanResponseData);
    }
    else
    {
        /* The GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}

#if defined(gBLE50_d) && (gBLE50_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdUpdatePeriodicAdvListOpCode(uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdUpdatePeriodicAdvListOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdUpdatePeriodicAdvListOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapPeriodicAdvListOperation_t operation = gAddDevice_c;
    bleAddressType_t addrType;
    bleDeviceAddress_t addr;
    uint8_t SID = 0U;

    fsciBleGetEnumValueFromBuffer(operation, pBuffer, gapPeriodicAdvListOperation_t);
    fsciBleGetEnumValueFromBuffer(addrType, pBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(addr, pBuffer);
    fsciBleGetUint8ValueFromBuffer(SID, pBuffer);
    fsciBleGapCallApiFunction(Gap_UpdatePeriodicAdvList(operation, addrType, addr, SID));

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdPeriodicAdvTerminateSyncOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdPeriodicAdvTerminateSyncOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdPeriodicAdvTerminateSyncOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t  syncHandle = 0x0U;

    /* Get start parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(syncHandle, pBuffer);
    fsciBleGapCallApiFunction(Gap_PeriodicAdvTerminateSync(syncHandle));
}

/*! *********************************************************************************
*\private
*\fn           void HandleCtrlCmdConfigureAdvCodingSchemeOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleCtrlCmdConfigureAdvCodingSchemeOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleCtrlCmdConfigureAdvCodingSchemeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    advCodingScheme_t coding_scheme = gAdv_CodingScheme_S8_S8_c;
    uint8_t handle = 0x00U;

    fsciBleGetEnumValueFromBuffer(coding_scheme, pBuffer, advCodingScheme_t);
    fsciBleGetUint8ValueFromBuffer(handle, pBuffer);

    /* Need to cast returned type osa_status_t to bleResults_t */
    fsciBleGapCallApiFunction((bleResult_t)Controller_ConfigureAdvCodingScheme(coding_scheme, handle));
}

/*! *********************************************************************************
*\private
*\fn           void HandleCmdDisablePrivacyPerAdvSetOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gCmdDisablePrivacyPerAdvSet opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleCmdDisablePrivacyPerAdvSetOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t adv_handle = 0x0U;
    bool_t  bBypassPrivacySetting = FALSE;
    bleResult_t result = gBleSuccess_c;

    fsciBleGetUint8ValueFromBuffer(adv_handle, pBuffer);
    fsciBleGetBoolValueFromBuffer(bBypassPrivacySetting, pBuffer);

    if (adv_handle < gMaxAdvSets_c)
    {
        gDisablePrivacyPerAdvSet[adv_handle] = bBypassPrivacySetting;
        result = gBleSuccess_c;
    }
    else
    {
        result = gBleInvalidParameter_c;
    }

    fsciBleGapStatusMonitor(result);
}
#endif

/*! *********************************************************************************
*\private
*\fn           void HandleCtrlCmdSetConnNotificationModeOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleCtrlCmdSetConnNotificationModeOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleCtrlCmdSetConnNotificationModeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint32_t mode = 0U;

    fsciBleGetUint8ValueFromBuffer(mode, pBuffer);

    /* Need to cast returned type osa_status_t to bleResults_t */
    fsciBleGapCallApiFunction((bleResult_t)Controller_SetConnNotificationMode(mode));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdStopAdvertisingOpCode(uint8_t *pBuffer,
*                                                     uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdStopAdvertisingOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdStopAdvertisingOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCallApiFunction(Gap_StopAdvertising());
}


#if defined(gBLE50_d) && (gBLE50_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdStopExtAdvertisingOpCode(uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdStopExtAdvertisingOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdStopExtAdvertisingOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t handle = 0x0U;

    /* Get stop handle from buffer */
    fsciBleGetUint8ValueFromBuffer(handle, pBuffer);
    fsciBleGapCallApiFunction(Gap_StopExtAdvertising(handle));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdRemoveAdvSetOpCode(uint8_t *pBuffer,
*                                                  uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdRemoveAdvSetOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdRemoveAdvSetOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t handle = 0x0U;
    fsciBleGetUint8ValueFromBuffer(handle, pBuffer);
    fsciBleGapCallApiFunction(Gap_RemoveAdvSet(handle));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdStartPeriodicAdvOpCode(uint8_t *pBuffer,
*                                                      uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdStartPeriodicAdvOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdStartPeriodicAdvOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t handle = 0x0U;
    bool_t bIncludeADI = FALSE;

    /* Get handle from buffer */
    fsciBleGetUint8ValueFromBuffer(handle, pBuffer);
    fsciBleGetBoolValueFromBuffer(bIncludeADI, pBuffer);
    fsciBleGapCallApiFunction(Gap_StartPeriodicAdvertising(handle, bIncludeADI));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdStopPeriodicAdvOpCode(uint8_t *pBuffer,
*                                                     uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdStopPeriodicAdvOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdStopPeriodicAdvOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t handle = 0x0U;

    /* Get handle from buffer */
    fsciBleGetUint8ValueFromBuffer(handle, pBuffer);
    fsciBleGapCallApiFunction(Gap_StopPeriodicAdvertising(handle));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdPeriodicAdvCreateSyncOpCode(uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdPeriodicAdvCreateSyncOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdPeriodicAdvCreateSyncOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapPeriodicAdvSyncReq_t pReq = {0};
    fsciBleGapGetPeriodicAdvSyncReqFromBuffer(&pReq, &pBuffer);
    fsciBleGapCallApiFunction(Gap_PeriodicAdvCreateSync(&pReq));
}

#endif
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdAuthorizeOpCode(uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdAuthorizeOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdAuthorizeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t          deviceId = gInvalidDeviceId_c;
    uint16_t            handle = 0x0U;
    gattDbAccessType_t  access = gAccessRead_c;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);
    fsciBleGetEnumValueFromBuffer(access, pBuffer, gattDbAccessType_t);

    fsciBleGapCallApiFunction(Gap_Authorize(deviceId, handle, access));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSaveCccdOpCode(uint8_t *pBuffer,
*                                              uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSaveCccdOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSaveCccdOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t      deviceId = gInvalidDeviceId_c;
    uint16_t        handle = 0x0U;
    gattCccdFlags_t cccd;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);
    fsciBleGetEnumValueFromBuffer(cccd, pBuffer, gattCccdFlags_t);

    fsciBleGapCallApiFunction(Gap_SaveCccd(deviceId, handle, cccd));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdCheckNotificationStatusOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdCheckNotificationStatusOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdCheckNotificationStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint16_t    handle = 0x0U;
    bool_t      bOutIsActive = FALSE;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);

    fsciBleGapCallApiFunction(Gap_CheckNotificationStatus(deviceId, handle, &bOutIsActive));
    fsciBleGapMonitorOutParams(CheckNotificationStatus, &bOutIsActive);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdCheckIndicationStatusOpCode(uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdCheckIndicationStatusOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdCheckIndicationStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint16_t    handle = 0x0U;
    bool_t      bOutIsActive = FALSE;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);

    fsciBleGapCallApiFunction(Gap_CheckIndicationStatus(deviceId, handle, &bOutIsActive));
    fsciBleGapMonitorOutParams(CheckIndicationStatus, &bOutIsActive);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdPairOpCode(uint8_t *pBuffer,
*                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdPairOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdPairOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t              deviceId = gInvalidDeviceId_c;
    gapPairingParameters_t  pairingParameters = {0};

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGapGetPairingParametersFromBuffer(&pairingParameters, &pBuffer);

    fsciBleGapCallApiFunction(Gap_Pair(deviceId, &pairingParameters));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEncryptLinkOpCode(uint8_t *pBuffer,
*                                                 uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEncryptLinkOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEncryptLinkOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;

    /* Get deviceId parameter from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    fsciBleGapCallApiFunction(Gap_EncryptLink(deviceId));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdAcceptPairingRequestOpCode(uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdAcceptPairingRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdAcceptPairingRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t              deviceId = gInvalidDeviceId_c;
    gapPairingParameters_t  pairingParameters = {0};

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGapGetPairingParametersFromBuffer(&pairingParameters, &pBuffer);

    fsciBleGapCallApiFunction(Gap_AcceptPairingRequest(deviceId, &pairingParameters));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdRejectPairingOpCode(uint8_t *pBuffer,
*                                                   uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdRejectPairingOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdRejectPairingOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t                      deviceId = gInvalidDeviceId_c;
    gapAuthenticationRejectReason_t reason;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(reason, pBuffer, gapAuthenticationRejectReason_t);

    fsciBleGapCallApiFunction(Gap_RejectPairing(deviceId, reason));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEnterPasskeyOpCode(uint8_t *pBuffer,
*                                                  uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEnterPasskeyOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEnterPasskeyOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint32_t    passkey = 0U;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint32ValueFromBuffer(passkey, pBuffer);

    fsciBleGapCallApiFunction(Gap_EnterPasskey(deviceId, passkey));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdProvideOobOpCode(uint8_t *pBuffer,
*                                                uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdProvideOobOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdProvideOobOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint8_t     oob[gcSmpOobSize_c] = {0};

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetOobFromBuffer(oob, pBuffer);

    fsciBleGapCallApiFunction(Gap_ProvideOob(deviceId, oob));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdRejectPasskeyRequestOpCode(uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdRejectPasskeyRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdRejectPasskeyRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;

    /* Get deviceId from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    fsciBleGapCallApiFunction(Gap_RejectPasskeyRequest(deviceId));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSendSmpKeysOpCode(uint8_t *pBuffer,
*                                                 uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSendSmpKeysOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSendSmpKeysOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t      deviceId = gInvalidDeviceId_c;
    gapSmpKeys_t*   pSmpKeys = NULL;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    /* Allocate buffer for SMP keys */
    pSmpKeys = fsciBleGapAllocSmpKeysForBuffer(pBuffer);

    if(NULL == pSmpKeys)
    {
        /* No memory => the GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get SMP keys from buffer */
        fsciBleGapGetSmpKeysFromBuffer(pSmpKeys, &pBuffer);

        fsciBleGapCallApiFunction(Gap_SendSmpKeys(deviceId, pSmpKeys));

        /* Free the buffer allocated for SMP keys */
        (void)fsciBleGapFreeSmpKeys(pSmpKeys);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdRejectKeyExchangeRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdRejectKeyExchangeRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdRejectKeyExchangeRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;

    /* Get deviceId from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    fsciBleGapCallApiFunction(Gap_RejectKeyExchangeRequest(deviceId));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdProvideLongTermKeyOpCode(uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdProvideLongTermKeyOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdProvideLongTermKeyOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint8_t     ltkSize = 0U;
    uint8_t*    pLtk = NULL;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(ltkSize, pBuffer);

    /* Allocate buffer for LTK (consider that ltkSize is
    bigger than 0) */
    pLtk = MEM_BufferAlloc(ltkSize);

    if(NULL == pLtk)
    {
        /* No memory => The GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get LTK from buffer */
        fsciBleGetArrayFromBuffer(pLtk, pBuffer, ltkSize);

        fsciBleGapCallApiFunction(Gap_ProvideLongTermKey(deviceId, pLtk, ltkSize));

        /* Free the buffer allocated for LTK */
        (void)MEM_BufferFree(pLtk);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdDenyLongTermKeyOpCode(uint8_t *pBuffer,
*                                                     uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdDenyLongTermKeyOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdDenyLongTermKeyOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;

    /* Get deviceId from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    fsciBleGapCallApiFunction(Gap_DenyLongTermKey(deviceId));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLoadEncryptionInformationOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdLoadEncryptionInformationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdLoadEncryptionInformationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint8_t*    pOutLtk = NULL;
    uint8_t     outLtkSize = 0U;

    /* Get deviceId from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    /* Allocate buffer for LTK (maximum LTK size) */
    pOutLtk = MEM_BufferAlloc(gcSmpMaxLtkSize_c);

    if(NULL == pOutLtk)
    {
        /* No memory => the GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))                        
        uint8_t ltkBlob[gcSmpMaxBlobSize_c];
        fsciBleGapCallApiFunction(Gap_LoadEncryptionInformation(deviceId, ltkBlob, &outLtkSize));  
        if(result == gBleSuccess_c)
        {
            if (gSecLibFunctions.pfSecLib_DeobfuscateKey != NULL)
            {
                (void)gSecLibFunctions.pfSecLib_DeobfuscateKey(ltkBlob, pOutLtk);
            }
            outLtkSize = gcSmpMaxLtkSize_c;
        }
        
#else          
        fsciBleGapCallApiFunction(Gap_LoadEncryptionInformation(deviceId, pOutLtk, &outLtkSize));
#endif /* (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U)) */                            
        
        fsciBleGapMonitorOutParams(LoadEncryptionInformation, pOutLtk, &outLtkSize);

        /* Free the buffer allocated for LTK */
        (void)MEM_BufferFree(pOutLtk);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetLocalPasskeyOpCode(uint8_t *pBuffer,
*                                                     uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetLocalPasskeyOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetLocalPasskeyOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint32_t passkey = 0U;

    /* Get passkey from buffer */
    fsciBleGetUint32ValueFromBuffer(passkey, pBuffer);

    fsciBleGapCallApiFunction(Gap_SetLocalPasskey(passkey));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdStopScanningOpCode(uint8_t *pBuffer,
*                                                  uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdStopScanningOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdStopScanningOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCallApiFunction(Gap_StopScanning());
}


/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdDisconnectOpCode(uint8_t *pBuffer,
*                                                uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdDisconnectOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdDisconnectOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;

    /* Get deviceId from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    fsciBleGapCallApiFunction(Gap_Disconnect(deviceId));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSaveCustomPeerInformationOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSaveCustomPeerInformationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSaveCustomPeerInformationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint16_t    offset = 0U;
    uint16_t    infoSize = 0U;
    uint8_t*    pInfo = NULL;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(offset, pBuffer);
    fsciBleGetUint16ValueFromBuffer(infoSize, pBuffer);

    /* Allocate buffer for info (consider that infoSize is
    bigger than 0) */
    if (infoSize <= gcReservedFlashSizeForCustomInformation_c)
    {
        pInfo = MEM_BufferAlloc(infoSize);
    }

    if(NULL == pInfo)
    {
        /* No memory => the GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get info from buffer */
        fsciBleGetArrayFromBuffer(pInfo, pBuffer, infoSize);

        fsciBleGapCallApiFunction(Gap_SaveCustomPeerInformation(deviceId, pInfo, offset, infoSize));

        /* Free the buffer allocated for info */
        (void)MEM_BufferFree(pInfo);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLoadCustomPeerInformationOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdLoadCustomPeerInformationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdLoadCustomPeerInformationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint16_t    offset = 0U;
    uint16_t    infoSize = 0U;
    uint8_t*    pOutInfo = NULL;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(offset, pBuffer);
    fsciBleGetUint16ValueFromBuffer(infoSize, pBuffer);

    /* Allocate buffer for info (consider that infoSize is
    bigger than 0) */
    if (infoSize <= gcReservedFlashSizeForCustomInformation_c)
    {
        pOutInfo = MEM_BufferAlloc(infoSize);
    }

    if(NULL == pOutInfo)
    {
        /* No memory => the GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        fsciBleGapCallApiFunction(Gap_LoadCustomPeerInformation(deviceId, pOutInfo, offset, infoSize));
        fsciBleGapMonitorOutParams(LoadCustomPeerInfo, pOutInfo, infoSize);

        /* Free the buffer allocated for info */
        (void)MEM_BufferFree(pOutInfo);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdCheckIfBondedOpCode(uint8_t *pBuffer,
*                                                   uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdCheckIfBondedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdCheckIfBondedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    bool_t      bOutIsBonded = FALSE;
    uint8_t     outNvmIndex = 0U;

    /* Get deviceId from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    fsciBleGapCallApiFunction(Gap_CheckIfBonded(deviceId, &bOutIsBonded, &outNvmIndex));
    fsciBleGapMonitorOutParams(CheckIfBonded, &bOutIsBonded, &outNvmIndex);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdCheckNvmIndexOpCode(uint8_t *pBuffer,
*                                                   uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdCheckNvmIndexOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdCheckNvmIndexOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t  nvmIndex = 0U;
    bool_t   bOutIsFree = FALSE;

    /* Get nvmIndex from buffer */
    fsciBleGetUint8ValueFromBuffer(nvmIndex, pBuffer);

    fsciBleGapCallApiFunction(Gap_CheckNvmIndex(nvmIndex, &bOutIsFree));
    fsciBleGapMonitorOutParams(CheckNvmIndex, &bOutIsFree);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdAdvIndexChangeOpCode(uint8_t *pBuffer,
*                                                    uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdAdvIndexChangeOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdAdvIndexChangeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleAdvIndexType_t   advIndexType = gAdvIndexAscend_c;
    uint8_t             aUserDefinedChannels[3] = {0};

    /* Get advertising index type from buffer */
    fsciBleGetEnumValueFromBuffer(advIndexType, pBuffer, bleAdvIndexType_t);

    if(advIndexType == gAdvIndexUser_c)
    {
        /* Get User Defined Channels from buffer */
        fsciBleGetArrayFromBuffer(aUserDefinedChannels, pBuffer, sizeof(aUserDefinedChannels));
    }

    fsciBleGapCallApiFunction(Gap_BleAdvIndexChange(advIndexType, aUserDefinedChannels));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdGetHostVersionOpCode(uint8_t *pBuffer,
*                                                   uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdGetHostVersionOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdGetHostVersionOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapHostVersion_t hostVersion = {0};

    fsciBleGapCallApiFunction(Gap_GetHostVersion(&hostVersion));
    fsciBleGapMonitorOutParams(GetHostVersion, &hostVersion);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdGetReadRemoteVersionInfoOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdGetReadRemoteVersionInfoOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdGetReadRemoteVersionInfoOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;

    /* Get deviceId from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    
    fsciBleGapCallApiFunction(Gap_ReadRemoteVersionInformation(deviceId));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdGetConnParamsOpCode(uint8_t *pBuffer,
*                                                   uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdGetConnParamsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdGetConnParamsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    uint8_t eventReportMode = 0U;

    /* Get deviceId from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(eventReportMode, pBuffer);
    
    fsciBleGapCallApiFunction(Gap_GetConnParamsMonitoring(deviceId, eventReportMode));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdGetDeviceIdFromConnHandleOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdGetDeviceIdFromConnHandleOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdGetDeviceIdFromConnHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t connHandle = 0x0U;
    deviceId_t deviceId = gInvalidDeviceId_c;

    /* Get connection handle from buffer */
    fsciBleGetUint16ValueFromBuffer(connHandle, pBuffer);

    fsciBleGapCallApiFunction(Gap_GetDeviceIdFromConnHandle(connHandle, &deviceId));
    fsciBleGapMonitorOutParams(GetDeviceIdFromConnHandle, deviceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdGetConnectionHandleFromDeviceIdOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdGetConnectionHandleFromDeviceIdOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdGetConnectionHandleFromDeviceIdOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    uint16_t connHandle = 0x0U;

    /* Get device id from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    fsciBleGapCallApiFunction(Gap_GetConnectionHandleFromDeviceId(deviceId, &connHandle));
    fsciBleGapMonitorOutParams(GetConnectionHandleFromDeviceId, connHandle);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdReadFilterAcceptListSizeOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdReadFilterAcceptListSizeOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdReadFilterAcceptListSizeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCallApiFunction(Gap_ReadFilterAcceptListSize());
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdClearFilterAcceptListOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdClearFilterAcceptListOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdClearFilterAcceptListOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCallApiFunction(Gap_ClearFilterAcceptList());
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdAddDeviceToFilterAcceptListOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdAddDeviceToFilterAcceptListOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdAddDeviceToFilterAcceptListOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleAddressType_t    addressType;
    bleDeviceAddress_t  address;

    /* Get command parameters from buffer */
    fsciBleGetEnumValueFromBuffer(addressType, pBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(address, pBuffer);

    fsciBleGapCallApiFunction(Gap_AddDeviceToFilterAcceptList(addressType, address));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdRemoveDeviceFromFilterAcceptListOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdRemoveDeviceFromFilterAcceptListOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdRemoveDeviceFromFilterAcceptListOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleAddressType_t    addressType;
    bleDeviceAddress_t  address;

    /* Get command parameters from buffer */
    fsciBleGetEnumValueFromBuffer(addressType, pBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(address, pBuffer);

    fsciBleGapCallApiFunction(Gap_RemoveDeviceFromFilterAcceptList(addressType, address));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdReadPublicDeviceAddressOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdReadPublicDeviceAddressOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdReadPublicDeviceAddressOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCallApiFunction(Gap_ReadPublicDeviceAddress());
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdReadControllerLocalRPAOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdReadControllerLocalRPAOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdReadControllerLocalRPAOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleIdentityAddress_t bleIdentityAddress = {0};
    /* Get command parameters from buffer */
    fsciBleGetEnumValueFromBuffer(bleIdentityAddress.idAddressType, pBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(bleIdentityAddress.idAddress, pBuffer);
    fsciBleGapCallApiFunction(Gap_ReadControllerLocalRPA(&bleIdentityAddress));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdCreateRandomDeviceAddressOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdCreateRandomDeviceAddressOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdCreateRandomDeviceAddressOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bool_t      bIrkIncluded = FALSE;
    uint8_t     irk[gcSmpIrkSize_c] = {0};
    uint8_t*    pIrk        = NULL;
    bool_t      bRandomPartIncluded = FALSE;
    uint8_t     randomPart[3] = {0};
    uint8_t*    pRandomPart = NULL;
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
    uint8_t     irkBlob[gcSmpMaxIrkBlobSize_c] = {0};                        
#endif                        
    /* Get from buffer the boolean value which indicates if IRK
    is included in the request */
    fsciBleGetBoolValueFromBuffer(bIrkIncluded, pBuffer);

    /* Verify if IRK is included in the request or not */
    if(TRUE == bIrkIncluded)
    {
        /* Get IRK from buffer */
        fsciBleGetIrkFromBuffer(irk, pBuffer);
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
        if (gSecLibFunctions.pfSecLib_ObfuscateKey != NULL)
        {
            (void)gSecLibFunctions.pfSecLib_ObfuscateKey(irk, irkBlob, 1U);
        }
        pIrk = irkBlob;
#else                            
        pIrk = irk;
#endif
        /* Get from buffer the boolean value which indicates if random
        part is included in the request */
        fsciBleGetBoolValueFromBuffer(bRandomPartIncluded, pBuffer);

        /* Verify if random part is included in the request or not */
        if(TRUE == bRandomPartIncluded)
        {
            /* Get random part (3 bytes) from buffer */
            fsciBleGetArrayFromBuffer(randomPart, pBuffer, 3U);
            pRandomPart = randomPart;
        }
    }

    fsciBleGapCallApiFunction(Gap_CreateRandomDeviceAddress(pIrk, pRandomPart));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSaveDeviceNameOpCode(uint8_t *pBuffer,
*                                                    uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSaveDeviceNameOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSaveDeviceNameOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint8_t     nameSize = 0U;
    uchar_t*    pName = NULL;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(nameSize, pBuffer);

    /* Allocate buffer for name (consider that nameSize
    is bigger than 0) */
    pName = MEM_BufferAlloc(nameSize);

    if(NULL == pName)
    {
        /* No memory => the GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get name from buffer */
        fsciBleGetArrayFromBuffer(pName, pBuffer, nameSize);

        fsciBleGapCallApiFunction(Gap_SaveDeviceName(deviceId, pName, nameSize));

        /* Free buffer allocated for name */
        (void)MEM_BufferFree(pName);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdGetBondedDevicesCountOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdGetBondedDevicesCountOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdGetBondedDevicesCountOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t outBondedDevicesCount = 0U;

    fsciBleGapCallApiFunction(Gap_GetBondedDevicesCount(&outBondedDevicesCount));
    fsciBleGapMonitorOutParams(GetBondedDevicesCount, &outBondedDevicesCount);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdGetBondedDeviceNameOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdGetBondedDeviceNameOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdGetBondedDeviceNameOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t     nvmIndex = 0U;
    uint8_t     maxNameSize = 0U;
    uchar_t*    pOutName = NULL;

    /* Get command parameters from buffer */
    fsciBleGetUint8ValueFromBuffer(nvmIndex, pBuffer);
    fsciBleGetUint8ValueFromBuffer(maxNameSize, pBuffer);

    /* Allocate buffer for name (consider that nameSize
    is bigger than 0) */
    pOutName = MEM_BufferAlloc(maxNameSize);

    if(NULL == pOutName)
    {
        /* No memory => the GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        fsciBleGapCallApiFunction(Gap_GetBondedDeviceName(nvmIndex, pOutName, maxNameSize));
        fsciBleGapMonitorOutParams(GetBondedDeviceName, pOutName);

        /* Free buffer allocated for name */
        (void)MEM_BufferFree(pOutName);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdRemoveBondOpCode(uint8_t *pBuffer,
*                                                uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdRemoveBondOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdRemoveBondOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t nvmIndex = 0U;

    /* Get nvmIndex from buffer */
    fsciBleGetUint8ValueFromBuffer(nvmIndex, pBuffer);

    fsciBleGapCallApiFunction(Gap_RemoveBond(nvmIndex));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdRemoveAllBondsOpCode(uint8_t *pBuffer,
*                                                    uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdRemoveAllBondsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdRemoveAllBondsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCallApiFunction(Gap_RemoveAllBonds());
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdReadRadioPowerLevelOpCode(uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdReadRadioPowerLevelOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdReadRadioPowerLevelOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapRadioPowerLevelReadType_t    txReadType = gTxPowerCurrentLevelInConnection_c;
    deviceId_t                      deviceId = gInvalidDeviceId_c;

    /* Get command parameters from buffer */
    fsciBleGetEnumValueFromBuffer(txReadType, pBuffer, gapRadioPowerLevelReadType_t);
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    fsciBleGapCallApiFunction(Gap_ReadRadioPowerLevel(txReadType, deviceId));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdVerifyPrivateResolvableAddressOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdVerifyPrivateResolvableAddressOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdVerifyPrivateResolvableAddressOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t             nvmIndex = 0U;
    bleDeviceAddress_t  address;

    /* Get command parameters from buffer */
    fsciBleGetUint8ValueFromBuffer(nvmIndex, pBuffer);
    fsciBleGetAddressFromBuffer(address, pBuffer);

    fsciBleGapCallApiFunction(Gap_VerifyPrivateResolvableAddress(nvmIndex, address));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdUpdateConnectionParametersOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdUpdateConnectionParametersOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdUpdateConnectionParametersOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint16_t    intervalMin = 0U;
    uint16_t    intervalMax = 0U;
    uint16_t    peripheralLatency = 0U;
    uint16_t    timeoutMultiplier = 0U;
    uint16_t    minCeLength = 0U;
    uint16_t    maxCeLength = 0U;

    /* Get command parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(intervalMin, pBuffer);
    fsciBleGetUint16ValueFromBuffer(intervalMax, pBuffer);
    fsciBleGetUint16ValueFromBuffer(peripheralLatency, pBuffer);
    fsciBleGetUint16ValueFromBuffer(timeoutMultiplier, pBuffer);
    fsciBleGetUint16ValueFromBuffer(minCeLength, pBuffer);
    fsciBleGetUint16ValueFromBuffer(maxCeLength, pBuffer);

    fsciBleGapCallApiFunction(Gap_UpdateConnectionParameters(deviceId, intervalMin, intervalMax, peripheralLatency, timeoutMultiplier, minCeLength, maxCeLength));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEnableUpdateConnectionParametersOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEnableUpdateConnectionParametersOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEnableUpdateConnectionParametersOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    bool_t      bEnable = FALSE;

    /* Get command parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetBoolValueFromBuffer(bEnable, pBuffer);

    fsciBleGapCallApiFunction(Gap_EnableUpdateConnectionParameters(deviceId, bEnable));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdUpdateLeDataLengthOpCode(uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdUpdateLeDataLengthOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdUpdateLeDataLengthOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    txOctets = 0U;
    uint16_t    txTime = 0U;
    deviceId_t  deviceId = gInvalidDeviceId_c;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(txOctets, pBuffer);
    fsciBleGetUint16ValueFromBuffer(txTime, pBuffer);

    fsciBleGapCallApiFunction(Gap_UpdateLeDataLength(deviceId, txOctets, txTime));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEnableHostPrivacyOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEnableHostPrivacyOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEnableHostPrivacyOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t* pIrk = NULL;
    uint8_t  irk[gcSmpIrkSize_c] = {0};
    bool_t   enable = FALSE;
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
    uint8_t     irkBlob[gcSmpMaxIrkBlobSize_c] = {0};                        
#endif
    /* Get command parameters from buffer */
    fsciBleGetBoolValueFromBuffer(enable, pBuffer);

    /* Verify if IRK is included in the request or not */
    if( TRUE == enable )
    {
        /* Get IRK from buffer */
        fsciBleGetIrkFromBuffer(irk, pBuffer);
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
        if (gSecLibFunctions.pfSecLib_ObfuscateKey != NULL)
        {
            (void)gSecLibFunctions.pfSecLib_ObfuscateKey(irk, irkBlob, 1U);
        }
        pIrk = irkBlob;                            
#else                            
        pIrk = irk;
#endif                                                        
    }

    fsciBleGapCallApiFunction(Gap_EnableHostPrivacy(enable, pIrk));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEnableControllerPrivacyOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEnableControllerPrivacyOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEnableControllerPrivacyOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapIdentityInformation_t* pPeerIdentities = NULL;
    gapIdentityInformation_t* pBufferPeerIdentities = NULL;
    uint32_t                  i = 0U;
    uint8_t                   ownIrk[gcSmpIrkSize_c] = {0};
    bool_t                    enable = FALSE;
    uint8_t                   peerIdCount = 0U;
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
    uint8_t     irkBlob[gcSmpMaxIrkBlobSize_c] = {0};                        
#endif
    /* Get command parameters from buffer */
    fsciBleGetBoolValueFromBuffer(enable, pBuffer);

    /* Verify if parameters are included in the request or not */
    if( TRUE == enable )
    {
        /* Get IRK from buffer */
        fsciBleGetIrkFromBuffer(ownIrk, pBuffer);
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
        if (gSecLibFunctions.pfSecLib_ObfuscateKey != NULL)
        {
            (void)gSecLibFunctions.pfSecLib_ObfuscateKey(ownIrk, irkBlob, 1U);
        }
        if (gSecLibFunctions.pfSecLib_GenerateBluetoothEIRKBlob != NULL)
        {
            (void)gSecLibFunctions.pfSecLib_GenerateBluetoothEIRKBlob(irkBlob, TRUE, mGenerateDKeyIRK, ownIrk);
        }
        mGenerateDKeyIRK = FALSE;
#endif
        fsciBleGetUint8ValueFromBuffer(peerIdCount, pBuffer);

        if( peerIdCount != 0U)
        {
            /* Allocate memory for gapIdentityInformation_t */
            pPeerIdentities = (gapIdentityInformation_t*)MEM_BufferAlloc((uint32_t)peerIdCount * sizeof(gapIdentityInformation_t));

            if( NULL == pPeerIdentities )
            {
                fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
            }
            else
            {
                pBufferPeerIdentities = pPeerIdentities;

                for( i=0; i<peerIdCount; i++ )
                {
                    fsciBleGapGetIdentityInformationFromBuffer(pBufferPeerIdentities, &pBuffer);
                    pBufferPeerIdentities++;
                }

                fsciBleGapCallApiFunction(Gap_EnableControllerPrivacy(enable, ownIrk, peerIdCount, pPeerIdentities));

                /* Free gapIdentityInformation_t allocated memory */
                (void)MEM_BufferFree(pPeerIdentities);
            }
        }
        else
        {
            fsciBleGapCallApiFunction(Gap_EnableControllerPrivacy(TRUE, NULL, 0, NULL));
        }
    }
    else
    {
        fsciBleGapCallApiFunction(Gap_EnableControllerPrivacy(FALSE, NULL, 0, NULL));
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetPrivacyModeOpCode(uint8_t *pBuffer,
*                                                    uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetPrivacyModeOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetPrivacyModeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t             nvmIndex = 0U;
    blePrivacyMode_t    privacyMode;

    /* Get command parameters from buffer */
    fsciBleGetUint8ValueFromBuffer(nvmIndex, pBuffer);
    fsciBleGetEnumValueFromBuffer(privacyMode, pBuffer, blePrivacyMode_t);
    fsciBleGapCallApiFunction(Gap_SetPrivacyMode(nvmIndex, privacyMode));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLeScRegeneratePublicKeyOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdLeScRegeneratePublicKeyOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdLeScRegeneratePublicKeyOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCallApiFunction(Gap_LeScRegeneratePublicKey());
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLeScValidateNumericValueOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdLeScValidateNumericValueOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdLeScValidateNumericValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    bool_t     valid = FALSE;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetBoolValueFromBuffer(valid, pBuffer);

    fsciBleGapCallApiFunction(Gap_LeScValidateNumericValue(deviceId, valid));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLeScGetLocalOobDataOpCode(uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdLeScGetLocalOobDataOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdLeScGetLocalOobDataOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCallApiFunction(Gap_LeScGetLocalOobData());
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLeScSetPeerOobDataOpCode(uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdLeScSetPeerOobDataOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdLeScSetPeerOobDataOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapLeScOobData_t leScOObData = {0};
    deviceId_t       deviceId = gInvalidDeviceId_c;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetArrayFromBuffer(leScOObData.randomValue, pBuffer, ((uint32_t)gSmpLeScRandomValueSize_c));
    fsciBleGetArrayFromBuffer(leScOObData.confirmValue, pBuffer, ((uint32_t)gSmpLeScRandomConfirmValueSize_c));

    fsciBleGapCallApiFunction(Gap_LeScSetPeerOobData(deviceId, &leScOObData));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLeScSendKeypressNotificationPrivacyOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdLeScSendKeypressNotificationPrivacyOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdLeScSendKeypressNotificationPrivacyOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t                deviceId = gInvalidDeviceId_c;
    gapKeypressNotification_t keypressNotification;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(keypressNotification, pBuffer, gapKeypressNotification_t);

    fsciBleGapCallApiFunction(Gap_LeScSendKeypressNotification(deviceId, keypressNotification));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdGetBondedDevicesIdentityInformationOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdGetBondedDevicesIdentityInformationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdGetBondedDevicesIdentityInformationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapIdentityInformation_t* pOutIdentityAddresses = NULL;
    uint8_t                   maxDevices = 0U;
    uint8_t                   outActualCount = 0U;

    /* Get command parameters from buffer */
    fsciBleGetUint8ValueFromBuffer(maxDevices, pBuffer);

    /* Allocate buffer for the device addresses array (consider that
    maxDevices is bigger than 0) */
    pOutIdentityAddresses = (gapIdentityInformation_t*)MEM_BufferAlloc((uint32_t)maxDevices * sizeof(gapIdentityInformation_t));

    if(NULL == pOutIdentityAddresses)
    {
        /* No memory => the GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        fsciBleGapCallApiFunction(Gap_GetBondedDevicesIdentityInformation(pOutIdentityAddresses, maxDevices, &outActualCount));
        fsciBleGapMonitorOutParams(GetBondedDevIdentityInfo, pOutIdentityAddresses, &outActualCount);

        /* Free the buffer allocated for the device addresses array */
        (void)MEM_BufferFree(pOutIdentityAddresses);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetTxPowerLevelOpCode(uint8_t *pBuffer,
*                                                     uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetTxPowerLevelOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetTxPowerLevelOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t                       powerLevel = 0U;
    bleTransmitPowerChannelType_t channelType = gTxPowerAdvChannel_c;

    fsciBleGetUint8ValueFromBuffer(powerLevel, pBuffer);
    fsciBleGetEnumValueFromBuffer(channelType, pBuffer, bleTransmitPowerChannelType_t);

    fsciBleGapCallApiFunction(Gap_SetTxPowerLevel(powerLevel, channelType));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLeReadPhyOpCode(uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdLeReadPhyOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdLeReadPhyOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    fsciBleGapCallApiFunction(Gap_LeReadPhy(deviceId));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLeSetPhyOpCode(uint8_t *pBuffer,
*                                              uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdLeSetPhyOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdLeSetPhyOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bool_t      defaultMode = FALSE;
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint8_t     allPhys = 0U;
    uint8_t     txPhys = 0U;
    uint8_t     rxPhys = 0U;
    uint16_t    phyOptions = 0U;

    /* Get command parameters from buffer */
    fsciBleGetBoolValueFromBuffer(defaultMode, pBuffer);
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(allPhys, pBuffer);
    fsciBleGetUint8ValueFromBuffer(txPhys, pBuffer);
    fsciBleGetUint8ValueFromBuffer(rxPhys, pBuffer);
    fsciBleGetUint16ValueFromBuffer(phyOptions, pBuffer);

    fsciBleGapCallApiFunction(Gap_LeSetPhy(defaultMode, deviceId, allPhys, txPhys, rxPhys, phyOptions));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdControllerNotificationOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdControllerNotificationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdControllerNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t   eventType = 0U;
    deviceId_t deviceId = gInvalidDeviceId_c;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(eventType, pBuffer);
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    fsciBleGapCallApiFunction(Gap_ControllerEnhancedNotification(eventType, deviceId));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLoadKeysOpCode(uint8_t *pBuffer,
*                                              uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdLoadKeysOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdLoadKeysOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t       nvmIndex = 0U;
    gapSmpKeys_t *pKeys = NULL;
    gapSmpKeyFlags_t keyFlags = 0U;
    bool_t        leSc = FALSE;
    bool_t        auth = FALSE;

    /* Get command parameters from buffer */
    fsciBleGetUint8ValueFromBuffer(nvmIndex, pBuffer);

    /* Allocate buffer for SMP keys */
    pKeys = fsciBleGapAllocSmpKeys();

    if (NULL == pKeys)
    {
        /* No memory => the GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciBleInterfaceId);
    }
    else
    {
        fsciBleGapCallApiFunction(Gap_LoadKeys(nvmIndex, pKeys, &keyFlags, &leSc, &auth));

        /* Set keys to NULL if not used. */
        if ((keyFlags & gLtk_c) == 0U)  { pKeys->aLtk = NULL; }
        if ((keyFlags & gIrk_c) == 0U)  { pKeys->aIrk = NULL; }
        if ((keyFlags & gCsrk_c) == 0U) { pKeys->aCsrk = NULL; }
        
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
        uint8_t tempKey[gcSmpMaxLtkSize_c];
        if((result == gBleSuccess_c) && (pKeys->aLtk != NULL)) 
        {
            if (gSecLibFunctions.pfSecLib_DeobfuscateKey != NULL)
            {
                (void)gSecLibFunctions.pfSecLib_DeobfuscateKey(pKeys->aLtk, tempKey);
            }
            pKeys->aLtk = tempKey;
            pKeys->cLtkSize = gcSmpMaxLtkSize_c;                                
        }                           
#endif   
        fsciBleGapMonitorOutParams(LoadKeys, pKeys, &keyFlags, &leSc, &auth);

        /* Free the buffer allocated for SMP keys */
        (void)fsciBleGapFreeSmpKeys(pKeys);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSaveKeysOpCode(uint8_t *pBuffer,
*                                              uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSaveKeysOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSaveKeysOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t       nvmIndex = 0U;
    gapSmpKeys_t *pKeys = NULL;
    bool_t        leSc = FALSE;
    bool_t        auth = FALSE;

    /* Get command parameters from buffer */
    fsciBleGetUint8ValueFromBuffer(nvmIndex, pBuffer);

    /* Allocate buffer for SMP keys */
    pKeys = fsciBleGapAllocSmpKeysForBuffer(pBuffer);

    if (NULL == pKeys)
    {
        /* No memory => the GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciBleInterfaceId);
    }
    else
    {
        /* Get SMP keys from buffer */
        fsciBleGapGetSmpKeysFromBuffer(pKeys, &pBuffer);

        fsciBleGetBoolValueFromBuffer(leSc, pBuffer);
        fsciBleGetBoolValueFromBuffer(auth, pBuffer);

        fsciBleGapCallApiFunction(Gap_SaveKeys(nvmIndex, pKeys, leSc, auth));

        /* Free the buffer allocated for SMP keys */
        (void)fsciBleGapFreeSmpKeys(pKeys);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetChannelMapOpCode(uint8_t *pBuffer,
*                                                   uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetChannelMapOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetChannelMapOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleChannelMap_t channelMap;

    /* Get command parameters from buffer */
    fsciBleGetArrayFromBuffer(channelMap, pBuffer, ((uint32_t)gcBleChannelMapSize_c));

    fsciBleGapCallApiFunction(Gap_SetChannelMap(channelMap));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdReadChannelMapOpCode(uint8_t *pBuffer,
*                                                    uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdReadChannelMapOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdReadChannelMapOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    fsciBleGapCallApiFunction(Gap_ReadChannelMap(deviceId));
}


#if defined(gBLE50_d) && (gBLE50_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetPeriodicAdvParametersOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetPeriodicAdvParametersOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetPeriodicAdvParametersOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapPeriodicAdvParameters_t advertisingParameters = {0};

    /* Get advertising parameters from buffer */
    fsciBleGapGetPeriodicAdvParametersFromBuffer(&advertisingParameters, &pBuffer);

    fsciBleGapCallApiFunction(Gap_SetPeriodicAdvParameters(&advertisingParameters));
}

#endif
#if (defined(CPU_MKW37A512VFT4) || defined(CPU_MKW37Z512VFT4) || defined(CPU_MKW38A512VFT4) || \
defined(CPU_MKW38Z512VFT4) || defined(CPU_MKW39A512VFT4) || defined(CPU_MKW39Z512VFT4) || \
defined(CPU_MKW35A512VFP4) || defined(CPU_MKW35Z512VHT4) || defined(CPU_MKW36A512VFP4) || \
defined(CPU_MKW36A512VHT4) || defined(CPU_MKW36Z512VFP4) || defined(CPU_MKW36Z512VHT4))
/*! *********************************************************************************
*\private
*\fn           void HandleCtrlCmdSetScanDupFiltModeOpCode(uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the gBleCtrlCmdSetScanDupFiltModeOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleCtrlCmdSetScanDupFiltModeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t mode = 0U;

    fsciBleGetUint8ValueFromBufferSigned(mode, pBuffer);

    fsciBleGapCallApiFunction(Controller_SetScanDupFiltMode((uint8_t)mode));
}

#endif

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEattConnectionRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEattConnectionRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEattConnectionRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    uint16_t mtu = 0U;
    uint8_t cBearers = 0U;
    uint16_t initialCredits = 0U;
    bool_t bAutoCreditsMgmt = FALSE;

    fsciBleGetUint8ValueFromBuffer(deviceId, pBuffer);
    fsciBleGetUint16ValueFromBuffer(mtu, pBuffer);
    fsciBleGetUint8ValueFromBuffer(cBearers, pBuffer);
    fsciBleGetUint16ValueFromBuffer(initialCredits, pBuffer);
    fsciBleGetBoolValueFromBuffer(bAutoCreditsMgmt, pBuffer);

    fsciBleGapCallApiFunction(Gap_EattConnectionRequest(deviceId, mtu, cBearers, initialCredits, bAutoCreditsMgmt));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEattConnectionAcceptOpCode(uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEattConnectionAcceptOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEattConnectionAcceptOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    bool_t accept = FALSE;
    uint16_t mtu = 0U;
    uint16_t initialCredits = 0U;
    bool_t bAutoCreditsMgmt = FALSE;

    fsciBleGetUint8ValueFromBuffer(deviceId, pBuffer);
    fsciBleGetBoolValueFromBuffer(accept, pBuffer);
    fsciBleGetUint16ValueFromBuffer(mtu, pBuffer);
    fsciBleGetUint16ValueFromBuffer(initialCredits, pBuffer);
    fsciBleGetBoolValueFromBuffer(bAutoCreditsMgmt, pBuffer);

    fsciBleGapCallApiFunction(Gap_EattConnectionAccept(deviceId, accept, mtu, initialCredits, bAutoCreditsMgmt));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEattReconfigureRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEattReconfigureRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEattReconfigureRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    uint16_t mtu = 0U;
    uint16_t mps = 0U;
    uint8_t cBearers = 0U;
    bearerId_t aBearers[gGapEattMaxBearers] = {0};

    fsciBleGetUint8ValueFromBuffer(deviceId, pBuffer);
    fsciBleGetUint16ValueFromBuffer(mtu, pBuffer);
    fsciBleGetUint16ValueFromBuffer(mps, pBuffer);
    fsciBleGetUint8ValueFromBuffer(cBearers, pBuffer);
    fsciBleGetArrayFromBuffer(aBearers, pBuffer, cBearers);

    fsciBleGapCallApiFunction(Gap_EattReconfigureRequest(deviceId, mtu, mps, cBearers, aBearers));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEattSendCreditsRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEattSendCreditsRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEattSendCreditsRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    bearerId_t bearerId;
    uint16_t credits = 0U;

    fsciBleGetUint8ValueFromBuffer(deviceId, pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetUint16ValueFromBuffer(credits, pBuffer);

    fsciBleGapCallApiFunction(Gap_EattSendCredits(deviceId, bearerId, credits));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEattDisconnectRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEattDisconnectRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdEattDisconnectRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    bearerId_t bearerId;

    fsciBleGetUint8ValueFromBuffer(deviceId, pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);

    fsciBleGapCallApiFunction(Gap_EattDisconnect(deviceId, bearerId));
}

#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
/*! *********************************************************************************
*\private
*\fn           void HandleGapStatusOpCode(uint8_t *pBuffer,
*                                         uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGapStatusOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleResult_t status = gBleSuccess_c;

    /* Get status from buffer */
    fsciBleGetEnumValueFromBuffer(status, pBuffer, bleResult_t);

    if(gBleSuccess_c != status)
    {
        /* Clean out parameters pointers kept in FSCI */
        fsciBleGapCleanOutParams();
    }
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtCheckNotificationStatusOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtCheckNotificationStatusOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtCheckNotificationStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCheckNotificationStatusOutParams_t* pOutParams = (fsciBleGapCheckNotificationStatusOutParams_t*)fsciBleGapRestoreOutParams();

    if(NULL != pOutParams->pIsActive)
    {
        /* Get out parameter of the Gap_CheckNotificationStatus function from buffer */
        fsciBleGetBoolValueFromBuffer(*pOutParams->pIsActive, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGapCleanOutParams();

        /* Signal out parameters ready */
        Ble_OutParamsReady();
    }
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtCheckIndicationStatusOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtCheckIndicationStatusOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtCheckIndicationStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCheckIndicationStatusOutParams_t* pOutParams = (fsciBleGapCheckIndicationStatusOutParams_t*)fsciBleGapRestoreOutParams();

    if(NULL != pOutParams->pIsActive)
    {
        /* Get out parameter of the Gap_CheckIndicationStatus function from buffer */
        fsciBleGetBoolValueFromBuffer(*pOutParams->pIsActive, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGapCleanOutParams();

        /* Signal out parameters ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGetBondedDevicesIdentityInformationOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGetBondedDevicesIdentityInformationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGetBondedDevicesIdentityInformationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapGetBondedDevicesIdentityInformationParams_t*  pOutParams;
    uint32_t                                                iCount;

    pOutParams = (fsciBleGapGetBondedDevicesIdentityInformationParams_t*)fsciBleGapRestoreOutParams();

    if(NULL != pOutParams->pActualCount)
    {
        /* Get number of device addresses from buffer */
        fsciBleGetUint8ValueFromBuffer(*pOutParams->pActualCount, pBuffer);

        if(NULL != pOutParams->pIdentityAddresses)
        {
            /* Get device addresses from buffer */
            for(iCount = 0; iCount < *pOutParams->pActualCount; iCount++)
            {
                fsciBleGapGetIdentityInformationFromBuffer(&pOutParams->pIdentityAddresses[iCount], &pBuffer);
            }
        }

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGapCleanOutParams();

        /* Signal out parameters ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtLoadEncryptionInformationOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtLoadEncryptionInformationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtLoadEncryptionInformationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapLoadEncryptionInformationOutParams_t* pOutParams = (fsciBleGapLoadEncryptionInformationOutParams_t*)fsciBleGapRestoreOutParams();

    /* Get out parameters of the Gap_LoadEncryptionInformation function from buffer */

    if(NULL != pOutParams->pLtkSize)
    {
        fsciBleGetUint8ValueFromBuffer(*pOutParams->pLtkSize, pBuffer);
    }

    if(NULL != pOutParams->pLtk)
    {
        fsciBleGetArrayFromBuffer(pOutParams->pLtk, pBuffer, *pOutParams->pLtkSize);
    }

    if((NULL != pOutParams->pLtkSize) ||
        (NULL != pOutParams->pLtk))
    {
        /* Clean out parameters pointers kept in FSCI */
        fsciBleGapCleanOutParams();

        /* Signal out parameters ready */
        Ble_OutParamsReady();
    }
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtLoadCustomPeerInformationOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtLoadCustomPeerInformationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtLoadCustomPeerInformationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapLoadCustomPeerInformationOutParams_t* pOutParams = (fsciBleGapLoadCustomPeerInformationOutParams_t*)fsciBleGapRestoreOutParams();
    uint16_t                                        infoSize;

    if(NULL != pOutParams->pInfo)
    {
        /* Get info size from buffer */
        fsciBleGetUint16ValueFromBuffer(infoSize, pBuffer);

        /* Get info value from buffer */
        fsciBleGetArrayFromBuffer(pOutParams->pInfo, pBuffer, infoSize);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGapCleanOutParams();

        /* Signal out parameters ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtCheckIfBondedOpCode(uint8_t *pBuffer,
*                                                   uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtCheckIfBondedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtCheckIfBondedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCheckIfBondedOutParams_t* pOutParams = (fsciBleGapCheckIfBondedOutParams_t*)fsciBleGapRestoreOutParams();

    if(NULL != pOutParams->pIsBonded)
    {
        /* Get out parameter of the Gap_CheckIfBonded function from buffer */
        fsciBleGetBoolValueFromBuffer(*pOutParams->pIsBonded, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGapCleanOutParams();

        /* Signal out parameters ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtCheckNvmIndexOpCode(uint8_t *pBuffer,
*                                                   uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtCheckNvmIndexOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtCheckNvmIndexOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCheckNvmIndexOutParams_t* pOutParams = (fsciBleGapCheckNvmIndexOutParams_t*)fsciBleGapRestoreOutParams();

    if(NULL != pOutParams->pIsFree)
    {
        /* Get out parameter of the Gap_CheckNvmIndex function from buffer */
        fsciBleGetBoolValueFromBuffer(*pOutParams->pIsFree, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGapCleanOutParams();

        /* Signal out parameters ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGetBondedDevicesCountOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGetBondedDevicesCountOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGetBondedDevicesCountOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapGetBondedDevicesCountOutParams_t* pOutParams = (fsciBleGapGetBondedDevicesCountOutParams_t*)fsciBleGapRestoreOutParams();

    if(NULL != pOutParams->pBondedDevicesCount)
    {
        /* Get out parameter of the Gap_GetBondedDevicesCount function from buffer */
        fsciBleGetUint8ValueFromBuffer(*pOutParams->pBondedDevicesCount, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGapCleanOutParams();

        /* Signal out parameters ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGetBondedDeviceNameOpCode(uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGetBondedDeviceNameOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGetBondedDeviceNameOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapGetBondedDeviceNameOutParams_t*   pOutParams = (fsciBleGapGetBondedDeviceNameOutParams_t*)fsciBleGapRestoreOutParams();
    uint16_t                                    nameSize = 0U;

    if(NULL != pOutParams->pName)
    {
        /* Get out parameters of the Gap_GetBondedDeviceName function from buffer */
        fsciBleGetUint16ValueFromBuffer(nameSize, pBuffer);
        fsciBleGetArrayFromBuffer(pOutParams->pName, pBuffer, nameSize);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGapCleanOutParams();

        /* Signal out parameters ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventInitializationCompleteOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventInitializationCompleteOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventInitializationCompleteOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gInitializationComplete_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventInternalErrorOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventInternalErrorOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventInternalErrorOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gInternalError_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventAdvertisingSetupFailedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventAdvertisingSetupFailedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventAdvertisingSetupFailedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gAdvertisingSetupFailed_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventAdvertisingParametersSetupCompleteOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventAdvertisingParametersSetupCompleteOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventAdvertisingParametersSetupCompleteOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gAdvertisingParametersSetupComplete_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventAdvertisingDataSetupCompleteOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventAdvertisingDataSetupCompleteOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventAdvertisingDataSetupCompleteOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gAdvertisingDataSetupComplete_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventFilterAcceptListSizeReadOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventFilterAcceptListSizeReadOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventFilterAcceptListSizeReadOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gFilterAcceptListSizeRead_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventDeviceAddedToFilterAcceptListOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventDeviceAddedToFilterAcceptListOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventDeviceAddedToFilterAcceptListOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gDeviceAddedToFilterAcceptList_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventDeviceRemovedFromFilterAcceptListOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventDeviceRemovedFromFilterAcceptListOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventDeviceRemovedFromFilterAcceptListOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gDeviceRemovedFromFilterAcceptList_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventFilterAcceptListClearedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventFilterAcceptListClearedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventFilterAcceptListClearedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gFilterAcceptListCleared_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventRandomAddressReadyOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventRandomAddressReadyOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventRandomAddressReadyOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gRandomAddressReady_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventCreateConnectionCanceledOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventCreateConnectionCanceledOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventCreateConnectionCanceledOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gCreateConnectionCanceled_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventPublicAddressReadOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventPublicAddressReadOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventPublicAddressReadOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gPublicAddressRead_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventAdvTxPowerLevelReadOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventAdvTxPowerLevelReadOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventAdvTxPowerLevelReadOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gAdvTxPowerLevelRead_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void void HandleGapEvtGenericEventPrivateResolvableAddressVerifiedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*  
*\brief        Handler for the gBleGapEvtGenericEventPrivateResolvableAddressVerifiedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventPrivateResolvableAddressVerifiedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gPrivateResolvableAddressVerified_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventRandomAddressSetOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventRandomAddressSetOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventRandomAddressSetOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gRandomAddressSet_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventLeScPublicKeyRegeneratedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventLeScPublicKeyRegeneratedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventLeScPublicKeyRegeneratedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gLeScPublicKeyRegenerated_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventLeScLocalOobDataOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventLeScLocalOobDataOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventLeScLocalOobDataOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gLeScLocalOobData_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventHostPrivacyStateChangedOpCode
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventHostPrivacyStateChangedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventHostPrivacyStateChangedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gHostPrivacyStateChanged_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventControllerPrivacyStateChangedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventControllerPrivacyStateChangedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventControllerPrivacyStateChangedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gControllerPrivacyStateChanged_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventTxPowerLevelSetCompleteOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventTxPowerLevelSetCompleteOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventTxPowerLevelSetCompleteOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gTxPowerLevelSetComplete_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventLePhyEventOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventLePhyEventOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventLePhyEventOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gLePhyEvent_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventControllerNotificationOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventControllerNotificationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventControllerNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gControllerNotificationEvent_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventBondCreatedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventBondCreatedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventBondCreatedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gBondCreatedEvent_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventChannelMapSetOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventChannelMapSetOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventChannelMapSetOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gChannelMapSet_c, pBuffer, fsciInterfaceId);

}

#if defined(gBLE50_d) && (gBLE50_d == 1U)

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventExtAdvertisingDataSetupCompleteOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventExtAdvertisingDataSetupCompleteOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventExtAdvertisingDataSetupCompleteOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gExtAdvertisingDataSetupComplete_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtAdvertisingEventExtAdvertisingSetRemovedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtAdvertisingEventExtAdvertisingSetRemovedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtAdvertisingEventExtAdvertisingSetRemovedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gExtAdvertisingSetRemoveComplete_c, pBuffer, fsciInterfaceId);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventPeriodicAdvCreateSyncCancelledOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventPeriodicAdvCreateSyncCancelledOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventPeriodicAdvCreateSyncCancelledOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gPeriodicAdvCreateSyncCancelled_c, pBuffer, fsciInterfaceId);

}

#endif

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtGenericEventTxEntryAvailableOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtGenericEventTxEntryAvailableOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtGenericEventTxEntryAvailableOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapGenericEvent(gTxEntryAvailable_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdBleHostInitializeOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdBleHostInitializeOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapGenericEvent(gapGenericEventType_t  eventType, uint8_t *pBuffer, uint32_t fsciInterfaceId){

    gapGenericEvent_t genericEvent = {0};
    genericEvent.eventType = eventType;

    fsciBleGapGetGenericEventFromBuffer(&genericEvent, &pBuffer);
    fsciBleGapCallbacks.genericCallback(&genericEvent);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtAdvertisingEventAdvertisingStateChangedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtAdvertisingEventAdvertisingStateChangedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtAdvertisingEventAdvertisingStateChangedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapAdvertisingEvent(gAdvertisingStateChanged_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtAdvertisingEventAdvertisingCommandFailedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtAdvertisingEventAdvertisingCommandFailedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtAdvertisingEventAdvertisingCommandFailedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapAdvertisingEvent(gAdvertisingCommandFailed_c, pBuffer, fsciInterfaceId);
}
   
#if defined(gBLE50_d) && (gBLE50_d == 1U)

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtAdvertisingEventAdvertisingSetTerminatedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtAdvertisingEventAdvertisingSetTerminatedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtAdvertisingEventAdvertisingSetTerminatedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapAdvertisingEvent(gAdvertisingSetTerminated_c, pBuffer, fsciInterfaceId);
}
#endif

/*! *********************************************************************************
*\private
*\fn           void HandleGapAdvertisingEvent(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for GAP advertising events.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapAdvertisingEvent(gapAdvertisingEvent_t eventType, uint8_t *pBuffer, uint32_t fsciInterfaceId){

    gapAdvertisingEvent_t advertisingEvent = {0};
    advertisingEvent.eventType = eventType;

    fsciBleGapGetAdvertisingEventFromBuffer(&advertisingEvent, &pBuffer);
    fsciBleGapCallbacks.advertisingCallback(&advertisingEvent);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtScanningEventScanStateChangedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtScanningEventScanStateChangedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtScanningEventScanStateChangedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapScanningEvent(gScanStateChanged_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtScanningEventScanCommandFailedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtScanningEventScanCommandFailedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtScanningEventScanCommandFailedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapScanningEvent(gScanCommandFailed_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtScanningEventDeviceScannedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtScanningEventDeviceScannedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtScanningEventDeviceScannedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapScanningEvent(gDeviceScanned_c, pBuffer, fsciInterfaceId);
}
   
#if defined(gBLE50_d) && (gBLE50_d == 1U)

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtScanningEventExtDeviceScannedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtScanningEventExtDeviceScannedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtScanningEventExtDeviceScannedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapScanningEvent(gExtDeviceScanned_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtScanningEventPeriodicDeviceScannedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtScanningEventPeriodicDeviceScannedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtScanningEventPeriodicDeviceScannedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapScanningEvent(gPeriodicDeviceScanned_c, pBuffer, fsciInterfaceId);
}
#endif

/*! *********************************************************************************
*\private
*\fn           void HandleGapScanningEvent(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for GAP scanning events.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapScanningEvent(gapScanningEventType_t eventType, uint8_t *pBuffer, uint32_t fsciInterfaceId){
    gapScanningEvent_t*     pScanningEvent = NULL;
    gapScanningEventType_t  eventType = gScanStateChanged_c;
    pScanningEvent = fsciBleGapAllocScanningEventForBuffer(eventType, pBuffer);

    if(NULL == pScanningEvent)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        fsciBleGapGetScanningEventFromBuffer(pScanningEvent, &pBuffer);

        fsciBleGapCallbacks.scanningCallback(pScanningEvent);

        fsciBleGapFreeScanningEvent(pScanningEvent);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventConnectedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventConnectedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventConnectedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtConnected_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventPairingRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventPairingRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventPairingRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtPairingRequest_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventPeripheralSecurityRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventPeripheralSecurityRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventPeripheralSecurityRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtPeripheralSecurityRequest_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventPairingResponseOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventPairingResponseOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventPairingResponseOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtPairingResponse_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventAuthenticationRejectedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventAuthenticationRejectedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventAuthenticationRejectedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtAuthenticationRejected_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventPasskeyRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventPasskeyRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventPasskeyRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtPasskeyRequest_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventOobRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventOobRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventOobRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtOobRequest_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventPasskeyDisplayOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventPasskeyDisplayOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventPasskeyDisplayOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtPasskeyDisplay_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventKeyExchangeRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventKeyExchangeRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventKeyExchangeRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtKeyExchangeRequest_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventKeysReceivedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventKeysReceivedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventKeysReceivedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtKeysReceived_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventLongTermKeyRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventLongTermKeyRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventLongTermKeyRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtLongTermKeyRequest_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventEncryptionChangedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventEncryptionChangedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventEncryptionChangedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtEncryptionChanged_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventPairingCompleteOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventPairingCompleteOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventPairingCompleteOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtPairingComplete_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventDisconnectedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventDisconnectedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventDisconnectedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtDisconnected_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventRssiReadOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventRssiReadOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventRssiReadOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtRssiRead_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventTxPowerLevelReadOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventTxPowerLevelReadOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventTxPowerLevelReadOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtTxPowerLevelRead_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventPowerReadFailureOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventPowerReadFailureOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventPowerReadFailureOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtPowerReadFailure_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventParameterUpdateRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventParameterUpdateRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventParameterUpdateRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtParameterUpdateRequest_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventParameterUpdateCompleteOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventParameterUpdateCompleteOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventParameterUpdateCompleteOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtParameterUpdateComplete_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventSetDataLenghtFailureOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gConnEvtLeSetDataLengthFailure_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapEvtConnectionEventSetDataLenghtFailureOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtLeSetDataLengthFailure_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventLeDataLengthChangedOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventLeDataLengthChangedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventLeDataLengthChangedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtLeDataLengthChanged_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventLeScOobDataRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventLeScOobDataRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventLeScOobDataRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtLeScOobDataRequest_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventLeScDisplayNumericValueOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventLeScDisplayNumericValueOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventLeScDisplayNumericValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtLeScDisplayNumericValue_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventLeScKeypressNotificationOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventLeScKeypressNotificationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventLeScKeypressNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtLeScKeypressNotification_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventChannelMapReadOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventChannelMapReadOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventChannelMapReadOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtChannelMapRead_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtConnectionEventChannelMapReadFailureOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtConnectionEventChannelMapReadFailureOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtConnectionEventChannelMapReadFailureOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    HandleGapConnectionEvent(gConnEvtChannelMapReadFailure_c, pBuffer, fsciInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapConnectionEvent(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for GAP connection events.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapConnectionEvent(gapConnectionEventType_t eventType, uint8_t *pBuffer, uint32_t fsciInterfaceId){
{
    gapConnectionEvent_t*       gConnectionEvent;
    gapConnectionEventType_t    eventType;
    deviceId_t deviceId;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    gConnectionEvent = fsciBleGapAllocConnectionEventForBuffer(eventType, pBuffer);

    if(NULL == gConnectionEvent)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        fsciBleGapGetConnectionEventFromBuffer(gConnectionEvent, &pBuffer);

        fsciBleGapCallbacks.connectionCallback(deviceId, gConnectionEvent);

        fsciBleGapFreeConnectionEvent(gConnectionEvent);
    }
}
#endif /* gFsciBleHost_d */

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLeSetSchedulerPriority(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdLeSetSchedulerPriority_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdLeSetSchedulerPriority(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t priorityHandle = 0x0U;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(priorityHandle, pBuffer);

    fsciBleGapCallApiFunction(Gap_LeSetSchedulerPriority(priorityHandle));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLeSetHostFeature(uint8_t *pBuffer,
*                                                uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdLeSetHostFeature_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdLeSetHostFeature
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
)
{
    uint8_t bitNumber = 0U;
    bool_t  enable = FALSE;

    /* Get command parameters from buffer */
    fsciBleGetUint8ValueFromBuffer(bitNumber, pBuffer);
    fsciBleGetBoolValueFromBuffer(enable, pBuffer);

    fsciBleGapCallApiFunction(Gap_LeSetHostFeature(bitNumber, enable));
}

/*! *********************************************************************************
* @}
********************************************************************************** */
