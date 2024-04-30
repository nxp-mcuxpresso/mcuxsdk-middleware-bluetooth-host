/*! *********************************************************************************
* \addtogroup FSCI_BLE
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2024 NXP
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
#include "gap_interface.h"
#include "controller_api.h"

#include "fsl_component_timer_manager.h"
#include "fsl_adapter_reset.h"
#include "fsci_ble_gap_handlers.h"
#include "fsci_ble_gap2_handlers.h"
#if gFsciBleHost_d
    #include "host_ble.h"
    #include "FsciCommands.h"
#endif /* gFsciBleHost_d */

#if gFsciBleGapHandoverLayerEnabled_d
#include "fsci_ble_gap_handover_types.h"
#include "fsci_ble_gap_handover.h"
#endif /* gFsciBleGapHandoverLayerEnabled_d */

#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
#include "SecLib.h"
#endif
#if defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U) && (defined(CPU_KW45B41Z83AFTA) || defined(CPU_K32W1480VFTA))
#include "fwk_platform.h"
#endif /* defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U) && (defined(CPU_KW45B41Z83AFTA) || defined(CPU_K32W1480VFTA)) */
#if gFsciIncluded_c && gFsciBleGapLayerEnabled_d


/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

#if gFsciBleHost_d
    /* Macro used for saving the out parameters pointers of the GAP functions */
    #define fsciBleGapSaveOutParams(pFirstParam, pSecondParam)      \
            fsciBleGapOutParams.pParam1 = (uint8_t*)pFirstParam;    \
            fsciBleGapOutParams.pParam2 = (uint8_t*)pSecondParam

    /* Macro used for restoring the out parameters pointers of the GAP functions */
    #define fsciBleGapRestoreOutParams()    \
            &fsciBleGapOutParams

    /* Macro used for setting the out parameters pointers of the GAP
    functions to NULL */
    #define fsciBleGapCleanOutParams()              \
            fsciBleGapOutParams.pParam1 = NULL;     \
            fsciBleGapOutParams.pParam2 = NULL;
#endif /* gFsciBleHost_d */

#define fsciBleGapCallApiFunction(apiFunction)          bleResult_t result = (apiFunction); \
                                                        fsciBleGapStatusMonitor(result)
#define fsciBleGapMonitorOutParams(functionId, ...)     if(gBleSuccess_c == result)                  \
                                                        {                                            \
                                                            FsciEvtMonitor(functionId, __VA_ARGS__); \
                                                        }
#define SizeOfArray(a) (sizeof(a)/sizeof(a[0]))

#define gLastGapGenericEventOpCode_c 0x47
/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/*! Structure that keeps callbacks registered by application or FSCI in GAP. */
typedef struct fsciBleGapCallbacks_tag
{
    hciHostToControllerInterface_t  hostToControllerInterface;  /* LE Controller uplink interface function
                                                                pointer. */
    gapGenericCallback_t            genericCallback;            /* Callback used to propagate GAP controller
                                                                events to the application. */
    gapAdvertisingCallback_t        advertisingCallback;        /* Callback used by the application to
                                                                receive advertising events. */
    gapScanningCallback_t           scanningCallback;           /* Callback used by the application to
                                                                receive scanning events. */
    gapConnectionCallback_t         connectionCallback;         /* Callback used by the application to
                                                                receive connection events. */
}fsciBleGapCallbacks_t;

#if gFsciBleHost_d
    /* Structure used for keeping the out parameters pointers of the GAP
     functions */
    typedef struct fsciBleGapOutParams_tag
    {
        uint8_t*    pParam1;
        uint8_t*    pParam2;
    }fsciBleGapOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    Gap_CheckNotificationStatus function */
    typedef struct fsciBleGapCheckNotificationStatusOutParams_tag
    {
        bool_t* pIsActive;
    }fsciBleGapCheckNotificationStatusOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    Gap_CheckIndicationStatus function */
    typedef struct fsciBleGapCheckIndicationStatusOutParams_tag
    {
        bool_t* pIsActive;
    }fsciBleGapCheckIndicationStatusOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    Gap_GetBondedDevicesIdentityInformation function */
    typedef struct fsciBleGapGetBondedDevicesIdentityInformationOutParams_tag
    {
        gapIdentityInformation_t* pIdentityAddresses;
        uint8_t*                  pActualCount;
    }fsciBleGapGetBondedDevicesIdentityInformationParams_t;

    /* Structure used for keeping the out parameters pointers of the
    Gap_LoadEncryptionInformation function */
    typedef struct fsciBleGapLoadEncryptionInformationOutParams_tag
    {
        uint8_t*    pLtk;
        uint8_t*    pLtkSize;
    }fsciBleGapLoadEncryptionInformationOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    Gap_LoadCustomPeerInformation function */
    typedef struct fsciBleGapLoadCustomPeerInformationOutParams_tag
    {
        uint8_t*   pInfo;
    }fsciBleGapLoadCustomPeerInformationOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    Gap_CheckIfBonded function */
    typedef struct fsciBleGapCheckIfBondedOutParams_tag
    {
        bool_t* pIsBonded;
    }fsciBleGapCheckIfBondedOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    Gap_CheckNvmIndex function */
    typedef struct fsciBleGapCheckNvmIndexOutParams_tag
    {
        bool_t* pIsFree;
    }fsciBleGapCheckNvmIndexOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    Gap_GetBondedDevicesCount function */
    typedef struct fsciBleGapGetBondedDevicesCountOutParams_tag
    {
        uint8_t*    pBondedDevicesCount;
    }fsciBleGapGetBondedDevicesCountOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    Gap_GetBondedDeviceName function */
    typedef struct fsciBleGapGetBondedDeviceNameOutParams_tag
    {
        uchar_t*    pName;
    }fsciBleGapGetBondedDeviceNameOutParams_t;
#endif /* gFsciBleHost_d */

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

static void fsciBleGapGenericCallback(gapGenericEvent_t* pGenericEvent);
static bleResult_t fsciBleHciHostToControllerInterface(hciPacketType_t packetType, void* pPacket, uint16_t packetSize);
static bleResult_t fsciWritePublicDeviceAddress(bleDeviceAddress_t bdAddress);
static void WritePublicDeviceAddress_TmrCb (void *param);
#if defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U) && (defined(CPU_KW45B41Z83AFTA) || defined(CPU_K32W1480VFTA))
static void fsciPlatformErrorCallback(uint32_t id, int32_t error_status);
#endif /* defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U) && (defined(CPU_KW45B41Z83AFTA) || defined(CPU_K32W1480VFTA)) */

/************************************************************************************
*************************************************************************************
* Public functions prototypes
*************************************************************************************
************************************************************************************/
void fsciBleGapAdvertisingCallback(gapAdvertisingEvent_t* pAdvertisingEvent);
void fsciBleGapScanningCallback(gapScanningEvent_t* pScanningEvent);
void fsciBleGapConnectionCallback(deviceId_t deviceId, gapConnectionEvent_t* pConnectionEvent);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

static const int16_t maConnectionEventToOpcode[]=
{
    (int16_t)gBleGapEvtConnectionEventConnectedOpCode_c,                         /* = 0x00U, gConnEvtConnected_c */
    (int16_t)gBleGapEvtConnectionEventPairingRequestOpCode_c,                    /* = 0x01U, gConnEvtPairingRequest_c */
    (int16_t)gBleGapEvtConnectionEventPeripheralSecurityRequestOpCode_c,         /* = 0x02U, gConnEvtPeripheralSecurityRequest_c */
    (int16_t)gBleGapEvtConnectionEventPairingResponseOpCode_c,                   /* = 0x03U, gConnEvtPairingResponse_c */
    (int16_t)gBleGapEvtConnectionEventAuthenticationRejectedOpCode_c,            /* = 0x04U, gConnEvtAuthenticationRejected_c  */
    (int16_t)gBleGapEvtConnectionEventPasskeyRequestOpCode_c,                    /* = 0x05U, gConnEvtPasskeyRequest_c */
    (int16_t)gBleGapEvtConnectionEventOobRequestOpCode_c,                        /* = 0x06U, gConnEvtOobRequest_c */
    (int16_t)gBleGapEvtConnectionEventPasskeyDisplayOpCode_c,                    /* = 0x07U, gConnEvtPasskeyDisplay_c */
    (int16_t)gBleGapEvtConnectionEventKeyExchangeRequestOpCode_c,                /* = 0x08U, gConnEvtKeyExchangeRequest_c */  
    (int16_t)gBleGapEvtConnectionEventKeysReceivedOpCode_c,                      /* = 0x09U, gConnEvtKeysReceived_c */
    (int16_t)gBleGapEvtConnectionEventLongTermKeyRequestOpCode_c,                /* = 0x0AU, gConnEvtLongTermKeyRequest_c */
    (int16_t)gBleGapEvtConnectionEventEncryptionChangedOpCode_c,                 /* = 0x0BU, gConnEvtEncryptionChanged_c */
    (int16_t)gBleGapEvtConnectionEventPairingCompleteOpCode_c,                   /* = 0x0CU, gConnEvtPairingComplete_c */
    (int16_t)gBleGapEvtConnectionEventDisconnectedOpCode_c,                      /* = 0x0DU, gConnEvtDisconnected_c */
    (int16_t)gBleGapEvtConnectionEventRssiReadOpCode_c,                          /* = 0x0EU, gConnEvtRssiRead_c */
    (int16_t)gBleGapEvtConnectionEventTxPowerLevelReadOpCode_c,                  /* = 0x0FU, gConnEvtTxPowerLevelRead_c */
    (int16_t)gBleGapEvtConnectionEventPowerReadFailureOpCode_c,                  /* = 0x10U, gConnEvtPowerReadFailure_c */
    (int16_t)gBleGapEvtConnectionEventParameterUpdateRequestOpCode_c,            /* = 0x11U, gConnEvtParameterUpdateRequest_c */
    (int16_t)gBleGapEvtConnectionEventParameterUpdateCompleteOpCode_c,           /* = 0x12U, gConnEvtParameterUpdateComplete_c */
    (int16_t)gBleGapEvtConnectionEventLeDataLengthChangedOpCode_c,               /* = 0x13U, gConnEvtLeDataLengthChanged_c */
    (int16_t)gBleGapEvtConnectionEventLeScOobDataRequestOpCode_c,                /* = 0x14U, gConnEvtLeScOobDataRequest_c */
    (int16_t)gBleGapEvtConnectionEventLeScDisplayNumericValueOpCode_c,           /* = 0x15U, gConnEvtLeScDisplayNumericValue_c */
    (int16_t)gBleGapEvtConnectionEventLeScKeypressNotificationOpCode_c,          /* = 0x16U, gConnEvtLeScKeypressNotification_c */
    (int16_t)gBleGapEvtConnectionEventChannelMapReadOpCode_c,                    /* = 0x17U, gConnEvtChannelMapRead_c */
    (int16_t)gBleGapEvtConnectionEventChannelMapReadFailureOpCode_c,             /* = 0x18U, gConnEvtChannelMapReadFailure_c */
    #if defined(gBLE50_d) && (gBLE50_d == 1U)
    (int16_t)gBleGapEvtConnectionEventChannelSelectionAlgorithm2OpCode_c,        /* = 0x19U, gConnEvtChanSelectionAlgorithm2_c */
    #else
    -1,                                                                          /* reserved: 0x19U */
    #endif
    (int16_t)gBleGapEvtPairingEventNoLtkOpCode_c,                                /* = 0x1AU, gConnEvtPairingNoLtk_c */
    (int16_t)gBleGapEvtPairingAlreadyStartedOpCode_c,                            /* = 0x1BU, gConnEvtPairingAlreadyStarted_c */
    #if defined(gBLE51_d) && (gBLE51_d == 1U)
    (int16_t)gBleGapEvtConnectionEventIqReportReceivedOpCode_c,                  /* = 0x1CU, gConnEvtIqReportReceived_c */
    (int16_t)gBleGapEvtConnectionEventCteRequestFailedOpCode_c,                  /* = 0x1DU, gConnEvtCteRequestFailed_c */
    (int16_t)gBleGapEvtConnectionEventCteReceiveParamsSetupCompleteOpCode_c,     /* = 0x1EU, gConnEvtCteReceiveParamsSetupComplete_c */
    (int16_t)gBleGapEvtConnectionEventCteTransmitParamsSetupCompleteOpCode_c,    /* = 0x1FU, gConnEvtCteTransmitParamsSetupComplete_c */
    (int16_t)gBleGapEvtConnectionEventCteReqStateChangedOpCode_c,                /* = 0x20U, gConnEvtCteReqStateChanged_c */
    (int16_t)gBleGapEvtConnectionEventCteRspStateChangedOpCode_c,                /* = 0x21U, gConnEvtCteRspStateChanged_c */
    #else
    -1,                                                                          /* reserved: 0x1CU */
    -1,                                                                          /* reserved: 0x1DU */
    -1,                                                                          /* reserved: 0x1EU */
    -1,                                                                          /* reserved: 0x1FU */
    -1,                                                                          /* reserved: 0x20U */
    -1,                                                                          /* reserved: 0x21U */
    #endif
    #if defined(gBLE52_d) && (gBLE52_d == 1U)
    (int16_t)gBleGapEvtConnectionEventPathLossThreshold_c,                       /* = 0x22U, gConnEvtPathLossThreshold_c */
    (int16_t)gBleGapEvtConnectionEventTransmitPowerReporting_c,                  /* = 0x23U, gConnEvtTransmitPowerReporting_c */
    (int16_t)gBleGapEvtConnectionEventEnhancedReadTransmitPowerLevel_c,          /* = 0x24U, gConnEvtEnhancedReadTransmitPowerLevel_c */
    (int16_t)gBleGapEvtConnectionEventPathLossReportingParamsSetupComplete_c,    /* = 0x25U, gConnEvtPathLossReportingParamsSetupComplete_c */
    (int16_t)gBleGapEvtConnectionEventPathLossReportingStateChanged_c,           /* = 0x26U, gConnEvtPathLossReportingStateChanged_c */
    (int16_t)gBleGapEvtConnectionEventTransmitPowerReportingStateChanged_c,      /* = 0x27U, gConnEvtTransmitPowerReportingStateChanged_c */
    #if defined(gEATT_d) && (gEATT_d == TRUE)
    (int16_t)gBleGapEvtConnectionEventEattConnectionRequest_c,                   /* = 0x28U, gConnEvtEattConnectionRequest_c */
    (int16_t)gBleGapEvtConnectionEventEattConnectionComplete_c,                  /* = 0x29U, gConnEvtEattConnectionComplete_c */
    (int16_t)gBleGapEvtConnectionEventEattReconfigureResponse_c,                 /* = 0x2AU, gConnEvtEattChannelReconfigureResponse_c */
    (int16_t)gBleGapEvtConnectionEventEattBearerStatusNotification_c,            /* = 0x2BU, gConnEvtEattBearerStatusNotification_c */
    #else
    -1,                                                                          /* reserved: 0x28U */
    -1,                                                                          /* reserved: 0x29U */
    -1,                                                                          /* reserved: 0x2AU */
    -1,                                                                          /* reserved: 0x2BU */
    #endif
    #else
    -1,                                                                          /* reserved: 0x22U */
    -1,                                                                          /* reserved: 0x23U */
    -1,                                                                          /* reserved: 0x24U */
    -1,                                                                          /* reserved: 0x25U */
    -1,                                                                          /* reserved: 0x26U */
    -1,                                                                          /* reserved: 0x27U */
    #endif
    -1,                                                                          /* = 0x2CU, gConnEvtHandoverConnected_c */
    -1,                                                                          /* = 0x2DU, gHandoverDisconnected_c */
    (int16_t)gBleGapEvtConnectionEventLeSetDataLengthFailedOpCode_c,             /* = 0x2EU, gConnEvtLeDataLengthChanged_c */
};



static const int16_t maGenericEventToOpcode[]=
{
  (int16_t)gBleGapEvtGenericEventInitializationCompleteOpCode_c,                           /* = 0x00U, gInitializationComplete_c */
  (int16_t)gBleGapEvtGenericEventInternalErrorOpCode_c,                                    /* = 0x01U, gInternalError_c */
  (int16_t)gBleGapEvtGenericEventAdvertisingSetupFailedOpCode_c,                           /* = 0x02U, gAdvertisingSetupFailed_c */
  (int16_t)gBleGapEvtGenericEventAdvertisingParametersSetupCompleteOpCode_c,               /* = 0x03U, gAdvertisingParametersSetupComplete_c */
  (int16_t)gBleGapEvtGenericEventAdvertisingDataSetupCompleteOpCode_c,                     /* = 0x04U, gAdvertisingDataSetupComplete_c */
  (int16_t)gBleGapEvtGenericEventFilterAcceptListSizeReadOpCode_c,                         /* = 0x05U, gFilterAcceptListSizeRead_c */
  (int16_t)gBleGapEvtGenericEventDeviceAddedToFilterAcceptListOpCode_c,                    /* = 0x06U, gDeviceAddedToFilterAcceptList_c */
  (int16_t)gBleGapEvtGenericEventDeviceRemovedFromFilterAcceptListOpCode_c,                /* = 0x07U, gDeviceRemovedFromFilterAcceptList_c */
  (int16_t)gBleGapEvtGenericEventFilterAcceptListClearedOpCode_c,                          /* = 0x08U, gFilterAcceptListCleared_c */
  (int16_t)gBleGapEvtGenericEventRandomAddressReadyOpCode_c,                               /* = 0x09U, gRandomAddressReady_c */
  (int16_t)gBleGapEvtGenericEventCreateConnectionCanceledOpCode_c,                         /* = 0x0AU, gCreateConnectionCanceled_c */
  (int16_t)gBleGapEvtGenericEventPublicAddressReadOpCode_c,                                /* = 0x0BU, gPublicAddressRead_c */
  (int16_t)gBleGapEvtGenericEventAdvTxPowerLevelReadOpCode_c,                              /* = 0x0CU, gAdvTxPowerLevelRead_c */
  (int16_t)gBleGapEvtGenericEventPrivateResolvableAddressVerifiedOpCode_c,                 /* = 0x0DU, gPrivateResolvableAddressVerified_c */
  (int16_t)gBleGapEvtGenericEventRandomAddressSetOpCode_c,                                 /* = 0x0EU, gRandomAddressSet_c */
  (int16_t)gBleGapEvtGenericEventLeScPublicKeyRegeneratedOpCode_c,                         /* = 0x0FU, gLeScPublicKeyRegenerated_c */
  (int16_t)gBleGapEvtGenericEventLeScLocalOobDataOpCode_c,                                 /* = 0x10U, gLeScLocalOobData_c */
  (int16_t)gBleGapEvtGenericEventHostPrivacyStateChangedOpCode_c,                          /* = 0x11U, gHostPrivacyStateChanged_c */
  (int16_t)gBleGapEvtGenericEventControllerPrivacyStateChangedOpCode_c,                    /* = 0x12U, gControllerPrivacyStateChanged_c */
  -1,                                                                                      /* reserved: 0x13U */
  (int16_t)gBleGapEvtGenericEventTxPowerLevelSetCompleteOpCode_c,                          /* = 0x14U, gTxPowerLevelSetComplete_c */
  (int16_t)gBleGapEvtGenericEventLePhyEventOpCode_c,                                       /* = 0x15U, gLePhyEvent_c */
  (int16_t)gBleGapEvtGenericEventControllerNotificationOpCode_c,                           /* = 0x16U, gControllerNotificationEvent_c */
  (int16_t)gBleGapEvtGenericEventGetConnParamsComplete_c,                                  /* = 0x17U, gGetConnParamsComplete_c */
  (int16_t)gBleGapEvtGenericEventBondCreatedOpCode_c,                                      /* = 0x18U, gBondCreatedEvent_c */
  (int16_t)gBleGapEvtGenericEventChannelMapSetOpCode_c,                                    /* = 0x19U, gChannelMapSet_c */
  #if defined(gBLE50_d) && (gBLE50_d == 1U)    
  (int16_t)gBleGapEvtGenericEventExtAdvertisingParametersSetupCompleteOpCode_c,            /* = 0x1AU, gExtAdvertisingParametersSetupComplete_c */
  (int16_t)gBleGapEvtGenericEventExtAdvertisingDataSetupCompleteOpCode_c,                  /* = 0x1BU, gExtAdvertisingDataSetupComplete_c */
  (int16_t)gBleGapEvtAdvertisingEventExtAdvertisingSetRemovedOpCode_c,                     /* = 0x1CU, gExtAdvertisingSetRemoveComplete_c */
  (int16_t)gBleGapEvtGenericEventPeriodicAdvParamSetupCompleteOpCode_c,                    /* = 0x1DU, gPeriodicAdvParamSetupComplete_c */
  (int16_t)gBleGapEvtGenericEventPeriodicAdvDataSetupCompleteOpCode_c,                     /* = 0x1EU, gPeriodicAdvDataSetupComplete_c */
  (int16_t)gBleGapEvtGenericEventPeriodicAdvertisingStateChangedOpCode_c,                  /* = 0x1FU, gPeriodicAdvertisingStateChanged_c */
  (int16_t)gBleGapEvtGenericEventPeriodicAdvListUpdateCompleteOpCode_c,                    /* = 0x20U, gPeriodicAdvListUpdateComplete_c */
  (int16_t)gBleGapEvtGenericEventPeriodicAdvCreateSyncCancelledOpCode_c,                   /* = 0x21U, gPeriodicAdvCreateSyncCancelled_c */
  #else        
  -1,                                                                                      /* reserved: 0x1AU */
  -1,                                                                                      /* reserved: 0x1BU */
  -1,                                                                                      /* reserved: 0x1CU */
  -1,                                                                                      /* reserved: 0x1DU */
  -1,                                                                                      /* reserved: 0x1EU */
  -1,                                                                                      /* reserved: 0x1FU */
  -1,                                                                                      /* reserved: 0x20U */
  -1,                                                                                      /* reserved: 0x21U */
  #endif        
  (int16_t)gBleGapEvtGenericEventTxEntryAvailableOpCode_c,                                 /* = 0x22U, gTxEntryAvailable_c */
  (int16_t)gBleGapEvtGenericEventControllerLocalRPAReadOpCode_c,                           /* = 0x23U, gControllerLocalRPARead_c */

  #if defined(gBLE51_d) && (gBLE51_d == 1U)
  (int16_t)gBleGapEvtGenericEventConnectionlessCteTransmitParamsSetupCompleteOpCode_c,     /* = 0x24U, gConnectionlessCteTransmitParamsSetupComplete_c */
  (int16_t)gBleGapEvtGenericEventConnectionlessCteTransmitStateChangedOpCode_c,            /* = 0x25U, gConnectionlessCteTransmitStateChanged_c */
  (int16_t)gBleGapEvtGenericEventConnectionlessIqSamplingStateChangedOpCode_c,             /* = 0x26U, gConnectionlessIqSamplingStateChanged_c */
  (int16_t)gBleGapEvtGenericEventAntennaInformationReadOpCode_c,                           /* = 0x27U, gAntennaInformationRead_c */
  -1,                                                                                      /* reserved: 0x28U */
  (int16_t)gBleGapEvtGenericEventPeriodicAdvRecvEnableCompleteOpCode_c,                    /* = 0x29U, gPeriodicAdvRecvEnableComplete_c */
  (int16_t)gBleGapEvtGenericEventPeriodicAdvSyncTransferCompleteOpCode_c ,                 /* = 0x2AU, gPeriodicAdvSyncTransferComplete_c */
  (int16_t)gBleGapEvtGenericEventPeriodicAdvSetInfoTransferCompleteOpCode_c,               /* = 0x2BU, gPeriodicAdvSetInfoTransferComplete_c */
  (int16_t)gBleGapEvtGenericEventSetPeriodicAdvSyncTransferParamsCompleteOpCode_c,         /* = 0x2CU, gSetPeriodicAdvSyncTransferParamsComplete_c */
  -1,                                                                                      /* reserved: 0x2DU */
  (int16_t)gBleGapEvtScanningEventPeriodicAdvSyncTransferReceivedOpCode_c,                 /* = 0x2EU, gPeriodicAdvSyncTransferSucceeded_c */
  (int16_t)gBleGapEvtScanningEventPeriodicAdvSyncTransferReceivedOpCode_c,                 /* = 0x2FU, gPeriodicAdvSyncTransferFailed_c */
  #else
  -1,                                                                                      /* reserved: 0x24U */
  -1,                                                                                      /* reserved: 0x25U */
  -1,                                                                                      /* reserved: 0x26U */
  -1,                                                                                      /* reserved: 0x27U */
  -1,                                                                                      /* reserved: 0x28U */
  -1,                                                                                      /* reserved: 0x29U */
  -1,                                                                                      /* reserved: 0x2AU */
  -1,                                                                                      /* reserved: 0x2BU */
  -1,                                                                                      /* reserved: 0x2CU */
  -1,                                                                                      /* reserved: 0x2DU */
  -1,                                                                                      /* reserved: 0x2EU */
  -1,                                                                                      /* reserved: 0x2FU */
  #endif
  (int16_t)gBleGapEvtGenericEventLeGenerateDhKeyComplete_c,                                /* = 0x30U, gConnEvtLeGenerateDhKeyComplete_c */
  -1,                                                                                      /* reserved: 0x31U */
  -1,                                                                                      /* reserved: 0x32U */
  -1,                                                                                      /* reserved: 0x33U */
  -1,                                                                                      /* reserved: 0x34U */
  -1,                                                                                      /* reserved: 0x35U */
  -1,                                                                                      /* reserved: 0x36U */
  -1,                                                                                      /* reserved: 0x37U */
  -1,                                                                                      /* reserved: 0x38U */
  -1,                                                                                      /* reserved: 0x39U */
  -1,                                                                                      /* reserved: 0x3AU */
  -1,                                                                                      /* reserved: 0x3BU */
  -1,                                                                                      /* reserved: 0x3CU */
  -1,                                                                                      /* reserved: 0x3DU */
  -1,                                                                                      /* reserved: 0x3EU */
  -1,                                                                                      /* reserved: 0x3FU */
  (int16_t)gBleGapEvtGenericEventReadRemoteVersionInfoComplete_c,                          /* = 0x40U, gRemoteVersionInformationRead_c */
  -1,                                                                                      /* reserved: 0x41U */
  -1,                                                                                      /* reserved: 0x42U */
  -1,                                                                                      /* reserved: 0x43U */
  -1,                                                                                      /* reserved: 0x44U */
  -1,                                                                                      /* reserved: 0x45U */
  -1,                                                                                      /* reserved: 0x46U */
  -1,                                                                                      /* reserved: 0x47U */
  -1,                                                                                      /* reserved: 0x48U */
  -1,                                                                                      /* reserved: 0x49U */
};

static const int16_t maGenericEvent2ToOpcode[]= {
#if defined(gBLE60_DecisionBasedAdvertisingFilteringSupport_d) && (gBLE60_DecisionBasedAdvertisingFilteringSupport_d == TRUE)
    (int16_t)gBleGapEvtGenericEventSetExtAdvertisingDecisionDataSetupComplete_c,             /* = 0x42U, gExtAdvertisingDecisionDataSetupComplete_c */
    (int16_t)gBleGapEvtGenericEventSetDecisionInstructionsSetupComplete_c,                   /* = 0x43U, gDecisionInstructionsSetupComplete_c */
#else /* gBLE60_DecisionBasedAdvertisingFilteringSupport_d */
    -1,
    -1,
#endif /* gBLE60_DecisionBasedAdvertisingFilteringSupport_d */
};


/* GAP callbacks structure initialized with FSCI empty static functions */
static fsciBleGapCallbacks_t fsciBleGapCallbacks =
{
    fsciBleHciHostToControllerInterface,
    fsciBleGapGenericCallback,
    fsciBleGapAdvertisingCallback,
    fsciBleGapScanningCallback,
    fsciBleGapConnectionCallback
};

#if gFsciBleBBox_d || gFsciBleTest_d
    static gapAutoConnectParams_t* pAutoConnectParams = NULL;
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleTest_d
    /* Indicates if FSCI for GAP is enabled or not */
    static bool_t bFsciBleGapEnabled            = FALSE;

    /* Indicates if the command was initiated by FSCI (to be not monitored) */
    static bool_t bFsciBleGapCmdInitiatedByFsci = FALSE;
#endif /* gFsciBleTest_d */

#if gFsciBleHost_d
    /* Keeps out parameters pointers for Host - BBox functionality */
    static fsciBleGapOutParams_t fsciBleGapOutParams = {NULL, NULL};
#endif /* gFsciBleHost_d */

#if gFsciBleTest_d
    static bool_t                   mDefaultGapPairingActive = FALSE;
    static gapPairingParameters_t   mDefaultGapPairingParameters;
#endif

/* The timer is used for WritePublicDeviceAddress command */
static TIMER_MANAGER_HANDLE_DEFINE(gAppTimerId);


/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

#if gFsciBleBBox_d || gFsciBleTest_d

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
void HandleGapCmdBleHostInitializeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCallApiFunction(Ble_HostInitialize(fsciBleGapCallbacks.genericCallback, fsciBleGapCallbacks.hostToControllerInterface));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdStartAdvertisingOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdStartAdvertisingOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdStartAdvertisingOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGapCallApiFunction(Gap_StartAdvertising(fsciBleGapCallbacks.advertisingCallback, fsciBleGapCallbacks.connectionCallback));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSendPeripheralSecurityRequestOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSendPeripheralSecurityRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSendPeripheralSecurityRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t              deviceId = gInvalidDeviceId_c;

    /* Get command parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

#if gFsciBleTest_d
    /* Just in test mode, use the previously set default pairing parameters */
    if (mDefaultGapPairingActive)
    {
        fsciBleGapCallApiFunction(Gap_SendPeripheralSecurityRequest(deviceId, &mDefaultGapPairingParameters));
    }
    else
    {
        fsciBleGapStatusMonitor(gBleInvalidState_c);
    }
#else
    gapPairingParameters_t  pairingParameters;
    fsciBleGapGetPairingParametersFromBuffer(&pairingParameters, &pBuffer);
    fsciBleGapCallApiFunction(Gap_SendPeripheralSecurityRequest(deviceId, &pairingParameters));
#endif
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdStartScanningOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdStartScanningOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdStartScanningOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bool_t                      bScanningParametersIncluded = FALSE;
    gapFilterDuplicates_t       enableFilterDuplicates = gGapDuplicateFilteringEnable_c;
    uint16_t                    duration = 0U, period = 0U;
    gapScanningParameters_t     scanningParameters;
    gapScanningParameters_t*    pScanningParameters = NULL;

    /* Get from buffer the boolean value which indicates if scanning
    parameters are included in the request */
    fsciBleGetBoolValueFromBuffer(bScanningParametersIncluded, pBuffer);

    /* Verify if scanning parameters are included in the request or not */
    if(TRUE == bScanningParametersIncluded)
    {
        /* Get the scanning parameters from buffer */
        fsciBleGapGetScanningParametersFromBuffer(&scanningParameters, &pBuffer);
        pScanningParameters = &scanningParameters;
    }

    /* Get from buffer the enum value which enables or disables
        duplicate advertising reports filtering */
    fsciBleGetEnumValueFromBuffer(enableFilterDuplicates, pBuffer, gapFilterDuplicates_t);

    /* Get the scanning PHYs */
    fsciBleGetEnumValueFromBuffer(scanningParameters.scanningPHYs, pBuffer, gapLePhyFlags_t);

    /* Get from buffer  the scan duration and period */
    fsciBleGetUint16ValueFromBuffer(duration, pBuffer);
    fsciBleGetUint16ValueFromBuffer(period, pBuffer);

    fsciBleGapCallApiFunction(Gap_StartScanning(pScanningParameters,fsciBleGapCallbacks.scanningCallback, enableFilterDuplicates,
                                                duration, period));
}
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdConnectOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdConnectOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdConnectOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapConnectionRequestParameters_t connectionRequestParameters = {0};

    /* Get connection request parameters from buffer */
    fsciBleGapGetConnectionRequestParametersFromBuffer(&connectionRequestParameters, &pBuffer);

    fsciBleGapCallApiFunction(Gap_Connect(&connectionRequestParameters, fsciBleGapCallbacks.connectionCallback));
}


/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetScanModeOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetScanModeOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetScanModeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapScanMode_t scanMode = gDefaultScan_c;

    /* Get scanMode parameter from buffer */
    fsciBleGetEnumValueFromBuffer(scanMode, pBuffer, gapScanMode_t);

    /* Free the oldest pAutoConnectParams */
    (void)fsciBleGapFreeAutoConnectParams(pAutoConnectParams);
    pAutoConnectParams = NULL;

    if(gAutoConnect_c == scanMode)
    {
        /* Allocate buffer for pAutoConnectParams */
        pAutoConnectParams = fsciBleGapAllocAutoConnectParamsForBuffer(pBuffer);

        if(NULL == pAutoConnectParams)
        {
            /* No memory => the GAP command can not be executed */
            fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
        }
        else
        {
            /* Get pAutoConnectParams from buffer */
            fsciBleGapGetAutoConnectParamsFromBuffer(pAutoConnectParams, &pBuffer);

            fsciBleGapCallApiFunction(Gap_SetScanMode(scanMode, pAutoConnectParams, fsciBleGapConnectionCallback));
        }
    }
    else
    {
        fsciBleGapCallApiFunction(Gap_SetScanMode(scanMode, NULL, NULL));
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetDefaultPairingParameters(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetDefaultPairingParameters opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetDefaultPairingParameters(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bool_t                  bPairingParametersIncluded = FALSE;
    gapPairingParameters_t  pairingParameters = {0};
    gapPairingParameters_t* pPairingParameters = NULL;

    /* Get bPairingParametersIncluded from buffer */
    fsciBleGetBoolValueFromBuffer(bPairingParametersIncluded, pBuffer);

    if(TRUE == bPairingParametersIncluded)
    {
        /* Get pPairingParameters from buffer */
        fsciBleGapGetPairingParametersFromBuffer(&pairingParameters, &pBuffer);
        pPairingParameters = &pairingParameters;
    }

#if gFsciBleTest_d
    /* Just in test mode, save pairing parameters to be used on Gap_SendPeripheralSecurityRequest */
    mDefaultGapPairingActive = bPairingParametersIncluded;

    if (mDefaultGapPairingActive)
    {
        mDefaultGapPairingParameters = *pPairingParameters;
    }
#endif

    fsciBleGapCallApiFunction(Gap_SetDefaultPairingParameters(pPairingParameters));

}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetRandomAddressOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetRandomAddressOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdSetRandomAddressOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleDeviceAddress_t address = {0};

    /* Get address from buffer */
    fsciBleGetAddressFromBuffer(address, pBuffer);

    fsciBleGapCallApiFunction(Gap_SetRandomAddress(address));
}

/*! *********************************************************************************
*\private
*\fn           void HandleCtrlWritePublicDeviceAddressOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleCtrlWritePublicDeviceAddressOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleCtrlWritePublicDeviceAddressOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleDeviceAddress_t deviceAddress = {0};
    bool_t  reset = FALSE;
    bleResult_t status = gBleSuccess_c;

    fsciBleGetBoolValueFromBuffer(reset, pBuffer);

    /* Get Device Address from buffer */
    fsciBleGetArrayFromBuffer(deviceAddress, pBuffer, ((uint32_t)gcBleDeviceAddressSize_c));

    status = fsciWritePublicDeviceAddress(deviceAddress);
    if(status == gBleSuccess_c)
    {
        if(reset == TRUE)
        {
            /* Allocate application timer and reset MCU on callback */
            timer_status_t statusTimer = TM_Open((timer_handle_t)gAppTimerId);
            if (statusTimer == kStatus_TimerSuccess)
            {
                /* reset MCU after 1 seconds */
                (void)TM_InstallCallback((timer_handle_t)gAppTimerId, WritePublicDeviceAddress_TmrCb, NULL);
                (void)TM_Start((timer_handle_t)gAppTimerId, (uint8_t)kTimerModeSingleShot, TmSecondsToMilliseconds(1U));
            }
            else
            {
                /* No timer available reset MCU */
                HAL_ResetMCU();
            }
        }
    }

    /* Send status over UART */
    fsciBleStatusMonitor(gFsciBleGapOpcodeGroup_c, (uint8_t)gBleGapStatusOpCode_c, status);
}

#if defined(gBLE50_d) && (gBLE50_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdStartExtAdvertisingOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdStartExtAdvertisingOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapCmdStartExtAdvertisingOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t  handle = 0U;
    uint16_t duration = 0U;
    uint8_t maxExtAdvEvents = 0U;

    /* Get start parameters from buffer */
    fsciBleGetUint8ValueFromBuffer(handle, pBuffer);
    fsciBleGetUint16ValueFromBuffer(duration, pBuffer);
    fsciBleGetUint8ValueFromBuffer(maxExtAdvEvents, pBuffer);
    fsciBleGapCallApiFunction(Gap_StartExtAdvertising(fsciBleGapCallbacks.advertisingCallback,
                                                        fsciBleGapCallbacks.connectionCallback,
                                                        handle,
                                                        duration,
                                                        maxExtAdvEvents));
}


#endif

#if defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U) && (defined(CPU_KW45B41Z83AFTA) || defined(CPU_K32W1480VFTA))
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdPlatformRegisterErrorCallback(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdPlatformRegisterErrorCallbackOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdPlatformRegisterErrorCallback
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
)
{
    bool_t enable = FALSE;
    PLATFORM_ErrorCallback_t pfPlatformErrorCallback = NULL;
    
    fsciBleGetBoolValueFromBuffer(enable, pBuffer);
    
    if (enable == TRUE)
    {
        pfPlatformErrorCallback = fsciPlatformErrorCallback;
    }
    
    PLATFORM_RegisterErrorCallback(pfPlatformErrorCallback);
    fsciBleGapStatusMonitor(gBleSuccess_c);
}
#endif /* defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U) && (defined(CPU_KW45B41Z83AFTA) || defined(CPU_K32W1480VFTA)) */

#endif

void fsciBleSetHciHostToControllerInterface(hciHostToControllerInterface_t hostToControllerInterface)
{
    /* Set HCI host to controller interface to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGapCallbacks.hostToControllerInterface = (NULL != hostToControllerInterface) ?
                                                    hostToControllerInterface :
                                                    fsciBleHciHostToControllerInterface;
}


void fsciBleSetGapGenericCallback(gapGenericCallback_t genericCallback)
{
    /* Set GAP controller callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGapCallbacks.genericCallback  = (NULL != genericCallback) ?
                                           genericCallback :
                                           fsciBleGapGenericCallback;
}


void fsciBleSetGapAdvertisingCallback(gapAdvertisingCallback_t advertisingCallback)
{
    /* Set GAP advertising callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGapCallbacks.advertisingCallback = (NULL != advertisingCallback) ?
                                              advertisingCallback :
                                              fsciBleGapAdvertisingCallback;
}


void fsciBleSetGapConnectionCallback(gapConnectionCallback_t connectionCallback)
{
    /* Set GAP connection callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGapCallbacks.connectionCallback  = (NULL != connectionCallback) ?
                                              connectionCallback :
                                              fsciBleGapConnectionCallback;
}


void fsciBleSetGapScanningCallback(gapScanningCallback_t scanningCallback)
{
    /* Set GAP scanning callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGapCallbacks.scanningCallback  = (NULL != scanningCallback) ?
                                            scanningCallback :
                                            fsciBleGapScanningCallback;
}

void fsciBleGapHandler(void* pData, void* param, uint32_t fsciInterfaceId)
{
    clientPacket_t* pClientPacket   = (clientPacket_t*)pData;
    uint8_t*        pBuffer         = &pClientPacket->structured.payload[0];
    bool_t          opCodeHandled   = FALSE;
#if gFsciBleTest_d
    /* Mark this command as initiated by FSCI */
    bFsciBleGapCmdInitiatedByFsci = TRUE;

    /* Verify if the command is Mode Select */
    if(gBleGapModeSelectOpCode_c == (fsciBleGapOpCode_t)pClientPacket->structured.header.opCode)
    {
        /* Get the new FSCI for GAP status (enabled or disabled) */
        fsciBleGetBoolValueFromBuffer(bFsciBleGapEnabled, pBuffer);
        /* Enable/Disable GAP2 */
        fsciBleGap2Enable(bFsciBleGapEnabled);
        /* Return status */
        fsciBleGapStatusMonitor(gBleSuccess_c);
    }
    else
    {
        /* Verify if FSCI is enabled for this layer */
        if(TRUE == bFsciBleGapEnabled)
        {
#endif /* gFsciBleTest_d */

#if gFsciBleBBox_d || gFsciBleTest_d
            if (pClientPacket->structured.header.opCode < maGapCmdOpCodeHandlersArraySize)
            {
                if (maGapCmdOpCodeHandlers[pClientPacket->structured.header.opCode] != NULL)
                {
                    /* Select the GAP function to be called (using the FSCI opcode) */
                    maGapCmdOpCodeHandlers[pClientPacket->structured.header.opCode](pBuffer, fsciInterfaceId);
                    opCodeHandled = TRUE;
                }
            }
#endif

#if gFsciBleHost_d
            if ((pClientPacket->structured.header.opCode >= gBleGapStatusOpCode_c) &&
               (pClientPacket->structured.header.opCode < maGapEvtOpCodeHandlersArraySize) &&
                 (opCodeHandled == FALSE))
            {
                if (maGapEvtOpCodeHandlers[pClientPacket->structured.header.opCode - gBleGapStatusOpCode_c] != NULL)
                {
                    maGapEvtOpCodeHandlers[pClientPacket->structured.header.opCode - gBleGapStatusOpCode_c](pBuffer, fsciInterfaceId);
                    opCodeHandled = TRUE;
                }
            }
#endif /* gFsciBleHost_d */
            if (opCodeHandled == FALSE)
            {
                /* Unknown FSCI opcode */
                fsciBleError(gFsciUnknownOpcode_c, fsciInterfaceId);
            }                
            
#if gFsciBleTest_d
        }
        else
        {
            /* FSCI SAP disabled */
            fsciBleError(gFsciSAPDisabled_c, fsciInterfaceId);
        }
    }

    /* Mark the next command as not initiated by FSCI */
    bFsciBleGapCmdInitiatedByFsci = FALSE;
#endif /* gFsciBleTest_d */

    (void)MEM_BufferFree(pData);
}

#if gFsciBleBBox_d || gFsciBleTest_d

void fsciBleGapStatusMonitor(bleResult_t result)
{
#if gFsciBleTest_d
    /* If GAP is disabled the status must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Send status over UART */
    fsciBleStatusMonitor(gFsciBleGapOpcodeGroup_c, (uint8_t)gBleGapStatusOpCode_c, result);
}

#endif /* gFsciBleBBox_d || gFsciBleTest_d */


#if gFsciBleHost_d || gFsciBleTest_d

void fsciBleGapNoParamCmdMonitor(fsciBleGapOpCode_t opCode)
{
#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

     /* Call the generic FSCI BLE monitor for commands or events that have no parameters */
    fsciBleNoParamCmdOrEvtMonitor((uint8_t)gFsciBleGapOpcodeGroup_c, (uint8_t)opCode);
}


void fsciBleGapUint8ParamCmdMonitor(fsciBleGapOpCode_t opCode, uint8_t param)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)opCode, sizeof(uint8_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(param, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapDeviceIdParamCmdMonitor(fsciBleGapOpCode_t opCode, deviceId_t deviceId)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)opCode,
                                              fsciBleGetDeviceIdBufferSize(&deviceId));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapCheckNotificationsAndIndicationsCmdMonitor(fsciBleGapOpCode_t opCode, deviceId_t deviceId, uint16_t handle, bool_t* pOutIsActive)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)opCode,
                                              fsciBleGetDeviceIdBufferSize(&deviceId) +
                                              sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(handle, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGapSaveOutParams(pOutIsActive, NULL);
#endif /* gFsciBleHost_d */
}


void fsciBleGapAddressParamsCmdMonitor(fsciBleGapOpCode_t opCode, bleAddressType_t addressType, const bleDeviceAddress_t address)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)opCode,
                                              sizeof(bleAddressType_t) +
                                              gcBleDeviceAddressSize_c);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromEnumValue(addressType, pBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(address, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapHostInitializeCmdMonitor(gapGenericCallback_t genericCallback, hciHostToControllerInterface_t  hostToControllerInterface)
{
#if gFsciBleHost_d
    /* Save generic callback */
    fsciBleGapCallbacks.genericCallback = genericCallback;

    /* Send FSCI CPU reset command to BlackBox */
    fsciBleNoParamCmdOrEvtMonitor(gFSCI_ReqOpcodeGroup_c, mFsciMsgResetCPUReq_c);
#else
    fsciBleGapNoParamCmdMonitor(gBleGapCmdBleHostInitializeOpCode_c);
#endif /* gFsciBleHost_d */
}


void fsciBleGapRegisterDeviceSecurityRequirementsCmdMonitor(const gapDeviceSecurityRequirements_t* pSecurity)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    bool_t                      bDeviceSecurityRequirementsIncluded = ((NULL != pSecurity) ? TRUE : FALSE);

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdRegisterDeviceSecurityRequirementsOpCode_c,
                                              sizeof(bool_t) +
                                              ((TRUE == bDeviceSecurityRequirementsIncluded) ?
                                               fsciBleGapGetDeviceSecurityRequirementsBufferSize(pSecurity) :
                                               0U));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromBoolValue(bDeviceSecurityRequirementsIncluded, pBuffer);
    if(TRUE == bDeviceSecurityRequirementsIncluded)
    {
        fsciBleGapGetBufferFromDeviceSecurityRequirements(pSecurity, &pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapSetAdvertisingParametersCmdMonitor(const gapAdvertisingParameters_t* pAdvertisingParameters)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetAdvertisingParametersOpCode_c,
                                              fsciBleGapGetAdvertisingParametersBufferSize(pAdvertisingParameters));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGapGetBuffFromAdvParameters(pAdvertisingParameters, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


#if defined(gBLE50_d) && (gBLE50_d == 1U)
void fsciBleGapSetExtAdvertisingParametersCmdMonitor(gapExtAdvertisingParameters_t* pAdvertisingParameters)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetExtAdvertisingParametersOpCode_c,
                                              fsciBleGapGetExtAdvertisingParametersBufferSize(pAdvertisingParameters));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGapGetBufferFromExtAdvertisingParameters(pAdvertisingParameters, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}
#endif /* gBLE50_d */


void fsciBleGapSetAdvertisingDataCmdMonitor(gapAdvertisingData_t* pAdvertisingData, gapScanResponseData_t* pScanResponseData)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    bool_t                      bAdvertisingDataIncluded = FALSE;
    bool_t                      bScanResponseDataIncluded = FALSE;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    if( NULL != pAdvertisingData )
    {
        if( pAdvertisingData->cNumAdStructures )
        {
            bAdvertisingDataIncluded = TRUE;
        }
    }

    if( NULL != pScanResponseData )
    {
        if( pScanResponseData->cNumAdStructures )
        {
            bScanResponseDataIncluded = TRUE;
        }
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetAdvertisingDataOpCode_c,
                                              sizeof(bool_t) +
                                              ((TRUE == bAdvertisingDataIncluded) ?
                                               fsciBleGapGetAdvertisingDataBufferSize(pAdvertisingData) :
                                               0U) +
                                              sizeof(bool_t) +
                                              ((TRUE == bScanResponseDataIncluded) ?
                                              fsciBleGapGetScanResponseDataBufferSize(pScanResponseData) :
                                              0U));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromBoolValue(bAdvertisingDataIncluded, pBuffer);
    if(TRUE == bAdvertisingDataIncluded)
    {
        fsciBleGapGetBufferFromAdvertisingData(pAdvertisingData, &pBuffer);
    }

    fsciBleGetBufferFromBoolValue(bScanResponseDataIncluded, pBuffer);
    if(TRUE == bScanResponseDataIncluded)
    {
        fsciBleGapGetBufferFromScanResponseData(pScanResponseData, &pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

#if defined(gBLE50_d) && (gBLE50_d == 1U)
void fsciBleGapSetExtAdvertisingDataCmdMonitor(uint8_t handle, gapAdvertisingData_t* pAdvertisingData, gapScanResponseData_t* pScanResponseData)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    bool_t                      bAdvertisingDataIncluded = FALSE;
    bool_t                      bScanResponseDataIncluded = FALSE;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    if( NULL != pAdvertisingData )
    {
        if( pAdvertisingData->cNumAdStructures )
        {
            bAdvertisingDataIncluded = TRUE;
        }
    }

    if( NULL != pScanResponseData )
    {
        if( pScanResponseData->cNumAdStructures )
        {
            bScanResponseDataIncluded = TRUE;
        }
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetAdvertisingDataOpCode_c,
                                              sizeof(uint8_t) + sizeof(bool_t) +
                                              ((TRUE == bAdvertisingDataIncluded) ?
                                               fsciBleGapGetAdvertisingDataBufferSize(pAdvertisingData) :
                                               0U) +
                                              sizeof(bool_t) +
                                              ((TRUE == bScanResponseDataIncluded) ?
                                               fsciBleGapGetScanResponseDataBufferSize(pScanResponseData) :
                                               0U));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(handle, pBuffer);
    fsciBleGetBufferFromBoolValue(bAdvertisingDataIncluded, pBuffer);
    if(TRUE == bAdvertisingDataIncluded)
    {
        fsciBleGapGetBufferFromAdvertisingData(pAdvertisingData, &pBuffer);
    }

    fsciBleGetBufferFromBoolValue(bScanResponseDataIncluded, pBuffer);
    if(TRUE == bScanResponseDataIncluded)
    {
        fsciBleGapGetBufferFromScanResponseData(pScanResponseData, &pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}
#endif

void fsciBleGapStartAdvertisingCmdMonitor(gapAdvertisingCallback_t advertisingCallback, gapConnectionCallback_t connectionCallback)
{
#if gFsciBleHost_d
    fsciBleGapCallbacks.advertisingCallback = advertisingCallback;
    fsciBleGapCallbacks.connectionCallback  = connectionCallback;
#endif /* gFsciBleHost_d */

    fsciBleGapNoParamCmdMonitor(gBleGapCmdStartAdvertisingOpCode_c);
}

#if defined(gBLE50_d) && (gBLE50_d == 1U)
void fsciBleGapStartExtAdvertisingCmdMonitor(gapAdvertisingCallback_t advertisingCallback, gapConnectionCallback_t connectionCallback, uint8_t handle, uint16_t duration, uint8_t maxExtAdvEvents)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

#if gFsciBleHost_d
    fsciBleGapCallbacks.advertisingCallback = advertisingCallback;
    fsciBleGapCallbacks.connectionCallback  = connectionCallback;
#endif /* gFsciBleHost_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdStartExtAdvertisingOpCode_c,
                                              sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint8_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(handle, pBuffer);
    fsciBleGetBufferFromUint16Value(duration, pBuffer);
    fsciBleGetBufferFromUint8Value(maxExtAdvEvents, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}
#endif

void fsciBleGapAuthorizeCmdMonitor(deviceId_t deviceId, uint16_t handle, gattDbAccessType_t access)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdAuthorizeOpCode_c, fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                                           sizeof(uint16_t) +
                                                                           sizeof(gattDbAccessType_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(handle, pBuffer);
    fsciBleGetBufferFromEnumValue(access, pBuffer, gattDbAccessType_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapSaveCccdCmdMonitor(deviceId_t deviceId, uint16_t handle, gattCccdFlags_t cccd)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSaveCccdOpCode_c, fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                                          sizeof(uint16_t) +
                                                                          sizeof(gattCccdFlags_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(handle, pBuffer);
    fsciBleGetBufferFromEnumValue(cccd, pBuffer, gattCccdFlags_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapGetBondedDevicesIdentityInformationCmdMonitor(const gapIdentityInformation_t* aOutIdentityAddresses, uint8_t maxDevices, const uint8_t* pOutActualCount)
{
    fsciBleGapUint8ParamCmdMonitor(gBleGapCmdGetBondedDevicesIdentityInformationOpCode_c, maxDevices);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGapSaveOutParams(aOutIdentityAddresses, pOutActualCount);
#endif /* gFsciBleHost_d */
}

void fsciBleGapPairCmdMonitor(deviceId_t deviceId, const gapPairingParameters_t* pPairingParameters)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdPairOpCode_c, fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                                      fsciBleGapGetPairingParametersBufferSize(pPairingParameters));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGapGetBufferFromPairingParameters(pPairingParameters, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapSendPeripheralSecurityRequestCmdMonitor(deviceId_t deviceId, const gapPairingParameters_t* pPairingParameters)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSendPeripheralSecurityRequestOpCode_c, fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                                                          fsciBleGapGetPairingParametersBufferSize(pPairingParameters));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGapGetBufferFromPairingParameters(pPairingParameters, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapAcceptPairingRequestCmdMonitor(deviceId_t deviceId, const gapPairingParameters_t* pPairingParameters)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdAcceptPairingRequestOpCode_c,
                                              fsciBleGetDeviceIdBufferSize(&deviceId) +
                                              fsciBleGapGetPairingParametersBufferSize(pPairingParameters));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGapGetBufferFromPairingParameters(pPairingParameters, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapRejectPairingCmdMonitor(deviceId_t deviceId, gapAuthenticationRejectReason_t reason)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdRejectPairingOpCode_c,
                                              fsciBleGetDeviceIdBufferSize(&deviceId) +
                                              sizeof(gapAuthenticationRejectReason_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromEnumValue(reason, pBuffer, gapAuthenticationRejectReason_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapEnterPasskeyCmdMonitor(deviceId_t deviceId, uint32_t passkey)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdEnterPasskeyOpCode_c,
                                              fsciBleGetDeviceIdBufferSize(&deviceId) +
                                              sizeof(uint32_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint32Value(passkey, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapProvideOobCmdMonitor(deviceId_t deviceId, const uint8_t* aOob)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
   /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdProvideOobOpCode_c,
                                              fsciBleGetDeviceIdBufferSize(&deviceId) +
                                              gcSmpOobSize_c);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromOob(aOob, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapSendSmpKeysCmdMonitor(deviceId_t deviceId, const gapSmpKeys_t* pKeys)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSendSmpKeysOpCode_c,
                                              fsciBleGetDeviceIdBufferSize(&deviceId) +
                                              fsciBleGapGetSmpKeysBufferSize(pKeys));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGapGetBufferFromSmpKeys(pKeys, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapProvideLongTermKeyCmdMonitor(deviceId_t deviceId, const uint8_t* aLtk, uint8_t ltkSize)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
   /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdProvideLongTermKeyOpCode_c,
                                              fsciBleGetDeviceIdBufferSize(&deviceId) +
                                              sizeof(uint8_t) + ltkSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint8Value(ltkSize, pBuffer);
    fsciBleGetBufferFromArray(aLtk, pBuffer, ltkSize);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapLoadEncryptionInformationCmdMonitor(deviceId_t deviceId, const uint8_t* aOutLtk, const uint8_t* pOutLtkSize)
{
    /* Monitor deviceId parameter */
    fsciBleGapDeviceIdParamCmdMonitor(gBleGapCmdLoadEncryptionInformationOpCode_c, deviceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGapSaveOutParams(aOutLtk, pOutLtkSize);
#endif /* gFsciBleHost_d */
}


void fsciBleGapSetLocalPasskeyCmdMonitor(uint32_t passkey)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetLocalPasskeyOpCode_c,
                                              sizeof(uint32_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint32Value(passkey, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapStartScanningCmdMonitor(const gapScanningParameters_t* pScanningParameters,
                                       gapScanningCallback_t scanningCallback,
                                       gapFilterDuplicates_t enableFilterDuplicates,
                                       uint16_t duration,
                                       uint16_t period
)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    bool_t                      bScanningParametersIncluded = ((NULL != pScanningParameters) ? TRUE : FALSE);

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

#if gFsciBleHost_d
    fsciBleGapCallbacks.scanningCallback = scanningCallback;
#endif /* gFsciBleHost_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdStartScanningOpCode_c,
                                              sizeof(bool_t) +
                                              ((TRUE == bScanningParametersIncluded) ?
                                               fsciBleGapGetScanningParametersBufferSize(pScanningParameters) :
                                               0U) + sizeof(gapFilterDuplicates_t) + sizeof(uint16_t) + sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromBoolValue(bScanningParametersIncluded, pBuffer);
    if(TRUE == bScanningParametersIncluded)
    {
        fsciBleGapGetBuffFromScanParameters(pScanningParameters, &pBuffer);
    }
    /* Set the enable filter duplicates field */
    fsciBleGetBufferFromEnumValue(enableFilterDuplicates, pBuffer, gapFilterDuplicates_t);

    /* Set the scanning PHYs */
    fsciBleGetBufferFromEnumValue(pScanningParameters->scanningPHYs, pBuffer, gapLePhyFlags_t);

    /* Set scan duration and period */
    fsciBleGetBufferFromUint16Value(duration, pBuffer);
    fsciBleGetBufferFromUint16Value(period, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapConnectCmdMonitor(const gapConnectionRequestParameters_t* pParameters, gapConnectionCallback_t connCallback)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

#if gFsciBleHost_d
    fsciBleGapCallbacks.connectionCallback = connCallback;
#endif /* gFsciBleHost_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdConnectOpCode_c,
                                              fsciBleGapGetConnectionRequestParametersBufferSize(pParameters));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGapGetBuffFromConnReqParameters(pParameters, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapSaveCustomPeerInformationCmdMonitor(deviceId_t deviceId, const uint8_t* aInfo, uint16_t offset, uint16_t infoSize)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSaveCustomPeerInformationOpCode_c,
                                              fsciBleGetDeviceIdBufferSize(&deviceId) +
                                              sizeof(uint16_t) + sizeof(uint16_t) + infoSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(offset, pBuffer);
    fsciBleGetBufferFromUint16Value(infoSize, pBuffer);
    fsciBleGetBufferFromArray(aInfo, pBuffer, infoSize);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapLoadCustomPeerInformationCmdMonitor(deviceId_t deviceId, const uint8_t* aOutInfo, uint16_t offset, uint16_t infoSize)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdLoadCustomPeerInformationOpCode_c,
                                              fsciBleGetDeviceIdBufferSize(&deviceId) +
                                              sizeof(uint16_t) + sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(offset, pBuffer);
    fsciBleGetBufferFromUint16Value(infoSize, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGapSaveOutParams(aOutInfo, NULL);
#endif /* gFsciBleHost_d */
}


void fsciBleGapCheckIfBondedCmdMonitor(deviceId_t deviceId, bool_t* pOutIsBonded)
{
    /* Monitor deviceId parameter */
    fsciBleGapDeviceIdParamCmdMonitor(gBleGapCmdCheckIfBondedOpCode_c, deviceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGapSaveOutParams(pOutIsBonded, NULL);
#endif /* gFsciBleHost_d */
}

void fsciBleGapCheckNvmIndexCmdMonitor(uint8_t nvmIndex, bool_t* pOutIsFree)
{
    /* Monitor nvmIndex parameter */
    fsciBleGapUint8ParamCmdMonitor(gBleGapCmdCheckNvmIndexOpCode_c, nvmIndex);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGapSaveOutParams(pOutIsFree, NULL);
#endif /* gFsciBleHost_d */
}

/*! *********************************************************************************
* \brief  Gap_GetConnectionHandleFromDeviceId command monitoring macro.
*
* \param[in]    deviceId    The connected peer to read the channel map.
*
********************************************************************************** */
void fsciBleGapGetConnectionHandleFromDeviceIdCmdMonitor
(
    deviceId_t deviceId
)
{
    uint8_t*                    pBuffer = NULL;
    clientPacketStructured_t*   pClientPacket = NULL;

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket(gBleGapCmdGetConnectionHandleFromDeviceIdOpCode_c,
                                              sizeof(uint8_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  Gap_GetDeviceIdFromConnHandle command monitoring macro.
*
* \param[in]    connHandle  Corresponding connection handle.
*
********************************************************************************** */
void fsciBleGapGetDeviceIdFromConnHandleCmdMonitor
(
    uint16_t    connHandle
)
{
    uint8_t*                    pBuffer = NULL;
    clientPacketStructured_t*   pClientPacket = NULL;

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket(gBleGapCmdGetDeviceIdFromConnHandleOpCode_c,
                                              sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(connHandle, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapCreateRandomDeviceAddressCmdMonitor(const uint8_t* aIrk, const uint8_t* aRandomPart)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    bool_t                      bIrkIncluded        = ((NULL != aIrk) ? TRUE : FALSE);
    bool_t                      bRandomPartIncluded = ((NULL != aRandomPart) ? TRUE : FALSE);

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdCreateRandomDeviceAddressOpCode_c,
                                              sizeof(bool_t) +
                                              ((TRUE == bIrkIncluded) ?
                                               (gcSmpIrkSize_c +
                                                sizeof(bool_t) +
                                                ((TRUE == bRandomPartIncluded) ? 3U : 0U))
                                               : 0U));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromBoolValue(bIrkIncluded, pBuffer);
    if(TRUE == bIrkIncluded)
    {
        fsciBleGetBufferFromIrk(aIrk, pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapSaveDeviceNameCmdMonitor(deviceId_t deviceId, const uchar_t* aName, uint8_t cNameSize)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSaveDeviceNameOpCode_c,
                                              fsciBleGetDeviceIdBufferSize(&deviceId) +
                                              sizeof(uint8_t) + cNameSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint8Value(cNameSize, pBuffer);
    fsciBleGetBufferFromArray(aName, pBuffer, cNameSize);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapGetBondedDevicesCountCmdMonitor(const uint8_t* pOutBondedDevicesCount)
{
    fsciBleGapNoParamCmdMonitor(gBleGapCmdGetBondedDevicesCountOpCode_c);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGapSaveOutParams(pOutBondedDevicesCount, NULL);
#endif /* gFsciBleHost_d */
}


void fsciBleGapGetBondedDeviceNameCmdMonitor(uint8_t nvmIndex, uchar_t* aOutName, uint8_t maxNameSize)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdGetBondedDeviceNameOpCode_c,
                                              sizeof(uint8_t) + sizeof(uint8_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(nvmIndex, pBuffer);
    fsciBleGetBufferFromUint8Value(maxNameSize, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGapSaveOutParams(aOutName, NULL);
#endif /* gFsciBleHost_d */
}


void fsciBleGapReadRadioPowerLevelCmdMonitor(gapRadioPowerLevelReadType_t txReadType, deviceId_t deviceId)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdReadRadioPowerLevelOpCode_c,
                                              sizeof(gapRadioPowerLevelReadType_t) + fsciBleGetDeviceIdBufferSize(&deviceId));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromEnumValue(txReadType, pBuffer, gapRadioPowerLevelReadType_t);
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapVerifyPrivateResolvableAddressCmdMonitor(uint8_t nvmIndex, const bleDeviceAddress_t aAddress)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdVerifyPrivateResolvableAddressOpCode_c,
                                              sizeof(uint8_t) + gcBleDeviceAddressSize_c);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(nvmIndex, pBuffer);
    fsciBleGetBufferFromAddress(aAddress, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapSetRandomAddressCmdMonitor(const bleDeviceAddress_t aAddress)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetRandomAddressOpCode_c,
                                              gcBleDeviceAddressSize_c);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromAddress(aAddress, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapSetScanModeCmdMonitor(gapScanMode_t scanMode, gapAutoConnectParams_t* pAutoConnectParams)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetScanModeOpCode_c,
                                              sizeof(gapScanMode_t) +
                                              ((gAutoConnect_c == scanMode) ?
                                                fsciBleGapGetAutoConnectParamsBufferSize(pAutoConnectParams) : 0U));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromEnumValue(scanMode, pBuffer, gapScanMode_t);
    fsciBleGapGetBufferFromAutoConnectParams(pAutoConnectParams, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapSetDefaultPairingParametersCmdMonitor(const gapPairingParameters_t* pPairingParameters)
{
    bool_t                      bPairingParametersIncluded = (NULL == pPairingParameters) ? FALSE : TRUE;
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetDefaultPairingParameters_c,
                                              sizeof(bool_t) +
                                              ((TRUE == bPairingParametersIncluded) ?
                                               fsciBleGapGetPairingParametersBufferSize(pPairingParameters) : 0U));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromBoolValue(bPairingParametersIncluded, pBuffer);

    if(TRUE == bPairingParametersIncluded)
    {
        fsciBleGapGetBufferFromPairingParameters(pPairingParameters, &pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapUpdateConnectionParametersCmdMonitor(deviceId_t deviceId, uint16_t intervalMin, uint16_t intervalMax, uint16_t peripheralLatency, uint16_t timeoutMultiplier, uint16_t minCeLength, uint16_t maxCeLength)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If Gap is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdUpdateConnectionParametersOpCode_c,
                                                fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                sizeof(uint16_t) + sizeof(uint16_t) +
                                                sizeof(uint16_t) + sizeof(uint16_t) +
                                                sizeof(uint16_t) + sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(intervalMin, pBuffer);
    fsciBleGetBufferFromUint16Value(intervalMax, pBuffer);
    fsciBleGetBufferFromUint16Value(peripheralLatency, pBuffer);
    fsciBleGetBufferFromUint16Value(timeoutMultiplier, pBuffer);
    fsciBleGetBufferFromUint16Value(minCeLength, pBuffer);
    fsciBleGetBufferFromUint16Value(maxCeLength, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapEnableUpdateConnectionParametersCmdMonitor(deviceId_t deviceId, bool_t enable)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If Gap is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdEnableUpdateConnectionParametersOpCode_c,
                                                fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(bool_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromBoolValue(enable, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapUpdateLeDataLengthCmdMonitor(deviceId_t deviceId, uint16_t txOctets, uint16_t txTime)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdUpdateLeDataLengthOpCode_c,
                                              2U * sizeof(uint16_t) + fsciBleGetDeviceIdBufferSize(&deviceId));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(txOctets, pBuffer);
    fsciBleGetBufferFromUint16Value(txTime, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapEnableHostPrivacyCmdMonitor(bool_t enable, const uint8_t* aIrk)
{
    clientPacketStructured_t* pClientPacket = NULL;
    uint8_t*                  pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdEnableHostPrivacyOpCode_c,
                                              sizeof(bool_t) +
                                              ((TRUE == enable) ? gcSmpIrkSize_c : 0U));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromBoolValue(enable, pBuffer);
    if(TRUE == enable)
    {
        fsciBleGetBufferFromIrk(aIrk, pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapEnableControllerPrivacyCmdMonitor(bool_t enable, const uint8_t* aOwnIrk, uint8_t peerIdCount, const gapIdentityInformation_t* aPeerIdentities)
{
    clientPacketStructured_t* pClientPacket = NULL;
    uint8_t*                  pBuffer = NULL;
    uint32_t                  i = 0U;
    uint8_t                   size = sizeof(bool_t);

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    if( TRUE == enable )
    {
        size += gcSmpIrkSize_c + peerIdCount * (sizeof(bleAddressType_t) +
                                                gcBleDeviceAddressSize_c +
                                                gcSmpIrkSize_c +
                                                sizeof(blePrivacyMode_t));
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdEnableControllerPrivacyOpCode_c, size);
    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromBoolValue(enable, pBuffer);
    if(TRUE == enable)
    {
        fsciBleGetBufferFromIrk(aOwnIrk, pBuffer);
        fsciBleGetBufferFromUint8Value(peerIdCount, pBuffer);

        for( i=0; i<peerIdCount; i++ )
        {
            fsciBleGapGetBufferFromIdentityInformation(&aPeerIdentities[i], &pBuffer);
        }
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapSetPrivacyModeCmdMonitor(uint8_t nvmIndex, blePrivacyMode_t privacyMode)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetPrivacyModeOpCode_c,
                                              sizeof(uint8_t) + sizeof(blePrivacyMode_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(nvmIndex, pBuffer);
    fsciBleGetBufferFromEnumValue(privacyMode, pBuffer, blePrivacyMode_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapLeScValidateNumericValueCmdMonitor(deviceId_t deviceId, bool_t valid)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdLeScValidateNumericValueOpCode_c,
                                              sizeof(bool_t) + fsciBleGetDeviceIdBufferSize(&deviceId));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromBoolValue(valid, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapLeScSetPeerOobDataCmdMonitor(deviceId_t deviceId, const gapLeScOobData_t* pPeerOobData)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdLeScSetPeerOobDataOpCode_c,
                                              gSmpLeScRandomValueSize_c +
                                              gSmpLeScRandomConfirmValueSize_c +
                                              fsciBleGetDeviceIdBufferSize(&deviceId));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromArray(pPeerOobData->randomValue, pBuffer, ((uint32_t)gSmpLeScRandomValueSize_c));
    fsciBleGetBufferFromArray(pPeerOobData->confirmValue, pBuffer, ((uint32_t)gSmpLeScRandomConfirmValueSize_c));

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapLeScSendKeypressNotificationCmdMonitor(deviceId_t deviceId, gapKeypressNotification_t keypressNotification)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdLeScSendKeypressNotificationPrivacyOpCode_c,
                                              sizeof(gapKeypressNotification_t) + fsciBleGetDeviceIdBufferSize(&deviceId));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromEnumValue(keypressNotification, pBuffer, gapKeypressNotification_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapSetTxPowerLevelCmdMonitor(uint8_t powerLevel, bleTransmitPowerChannelType_t channelType)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetTxPowerLevelOpCode_c,
                                              sizeof(uint8_t) + sizeof(bleTransmitPowerChannelType_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(powerLevel, pBuffer);
    fsciBleGetBufferFromEnumValue(channelType, pBuffer, bleTransmitPowerChannelType_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapLeReadPhyCmdMonitor(deviceId_t deviceId)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdLeReadPhyOpCode_c,
                                              fsciBleGetDeviceIdBufferSize(&deviceId));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapLeSetPhyCmdMonitor(bool_t defaultMode, deviceId_t deviceId, uint8_t allPhys,
                                  uint8_t txPhys, uint8_t rxPhys, uint16_t phyOptions)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdLeSetPhyOpCode_c,
                                              sizeof(bool_t) + fsciBleGetDeviceIdBufferSize(&deviceId) +
                                              3U * sizeof(uint8_t) + sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromBoolValue(defaultMode, pBuffer);
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint8Value(allPhys, pBuffer);
    fsciBleGetBufferFromUint8Value(txPhys, pBuffer);
    fsciBleGetBufferFromUint8Value(rxPhys, pBuffer);
    fsciBleGetBufferFromUint16Value(phyOptions, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapControllerEnhancedNotificationCmdMonitor(uint16_t eventType, deviceId_t deviceId)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdControllerNotificationOpCode_c,
                                              sizeof(uint16_t) + fsciBleGetDeviceIdBufferSize(&deviceId));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(eventType, pBuffer);
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapLoadKeysCmdMonitor
(
    uint8_t           nvmIndex,
    gapSmpKeys_t*     pOutKeys,
    gapSmpKeyFlags_t* pOutKeyFlags,
    bool_t*           pOutLeSc,
    bool_t*           pOutAuth
)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdLoadKeysOpCode_c,
                                              sizeof(uint8_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(nvmIndex, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapSaveKeysCmdMonitor(uint8_t nvmIndex, gapSmpKeys_t* pKeys, bool_t leSc, bool_t auth)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSaveKeysOpCode_c,
                                              sizeof(uint8_t) + 2U * sizeof(bool_t) +
                                              fsciBleGapGetSmpKeysBufferSize(pKeys));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(nvmIndex, pBuffer);
    fsciBleGapGetBufferFromSmpKeys(pKeys, &pBuffer);
    fsciBleGetBufferFromBoolValue(leSc, pBuffer);
    fsciBleGetBufferFromBoolValue(auth, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapSetChannelMapCmdMonitor
(
    bleChannelMap_t channelMap
)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetChannelMapOpCode_c,
                                              gcBleChannelMapSize_c);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromArray(channelMap, pBuffer, ((uint32_t)gcBleChannelMapSize_c));

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

#if defined(gBLE50_d) && (gBLE50_d == 1U)
void fsciBleGapSetPeriodicAdvParametersCmdMonitor(gapPeriodicAdvParameters_t* pAdvertisingParameters)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetPeriodicAdvParametersOpCode_c,
                                              sizeof(uint8_t) + sizeof(bool_t) +  2U * sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGapGetBufferFromPerAdvParameters(pAdvertisingParameters, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapSetPeriodicAdvertisingDataCmdMonitor(uint8_t handle, gapAdvertisingData_t* pAdvertisingData)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdSetPeriodicAdvDataOpCode_c,
                                              sizeof(uint8_t) +
                                              fsciBleGapGetAdvertisingDataBufferSize(pAdvertisingData));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(handle, pBuffer);
    fsciBleGapGetBufferFromAdvertisingData(pAdvertisingData, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}
#endif /* gBLE50_d */

#endif /* gFsciBleHost_d || gFsciBleTest_d */


#if gFsciBleBBox_d || gFsciBleTest_d

void fsciBleGapBoolParamEvtMonitor(fsciBleGapOpCode_t opCode, bool_t bParam)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)opCode, sizeof(bool_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromBoolValue(bParam, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapArrayAndSizeParamEvtMonitor(fsciBleGapOpCode_t opCode, const uint8_t* aArray, uint8_t arraySize)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)opCode, (sizeof(uint8_t) +
                                                      ((NULL == aArray) ? 0U : (uint16_t)arraySize)));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromUint8Value(arraySize, pBuffer);

    if(NULL != aArray)
    {
        fsciBleGetBufferFromArray(aArray, pBuffer, arraySize);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapGetBondedDevIdentityInfoEvtMonitor(const gapIdentityInformation_t* aOutIdentityAddresses, const uint8_t* pOutActualCount)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    uint32_t                    iCount = 0U;

 #if gFsciBleTest_d
   /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapEvtGetBondedDevicesIdentityInformationOpCode_c,
                                              sizeof(uint8_t) +
                                              (uint32_t)(*pOutActualCount) * (sizeof(bleAddressType_t) +
                                                                             gcBleDeviceAddressSize_c +
                                                                             gcSmpIrkSize_c +
                                                                             sizeof(blePrivacyMode_t)));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromUint8Value(*pOutActualCount, pBuffer);

    for(iCount = 0; iCount < *pOutActualCount; iCount++)
    {
        fsciBleGapGetBufferFromIdentityInformation(&aOutIdentityAddresses[iCount], &pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapLoadCustomPeerInfoEvtMonitor(const uint8_t* aOutInfo, uint16_t infoSize)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapEvtLoadCustomPeerInformationOpCode_c,
                                              sizeof(uint16_t) + infoSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromUint16Value(infoSize, pBuffer);
    fsciBleGetBufferFromArray(aOutInfo, pBuffer, infoSize);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGapCheckIfBondedEvtMonitor
(
    bool_t*     pOutIsBonded,
    uint8_t*    pOutNvmIndex
)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapEvtCheckIfBondedOpCode_c, sizeof(bool_t) + sizeof(uint8_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromBoolValue(*pOutIsBonded, pBuffer);
    fsciBleGetBufferFromUint8Value(*pOutNvmIndex, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapCheckNvmIndexEvtMonitor
(
    bool_t*     pOutIsFree
)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapEvtCheckNvmIndexOpCode_c, sizeof(bool_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromBoolValue(*pOutIsFree, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapGetHostVersionEvtMonitor
(
    gapHostVersion_t *pOutHostVersion
)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapEvtGetHostVersionOpCode_c, 
                                              sizeof(gapHostVersion_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGapGetBufferFromHostVersion(pOutHostVersion, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapGetDeviceIdFromConnHandleEvtMonitor
(
    deviceId_t    deviceId
)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapEvtGetDeviceIdFromConnHandleOpCode_c, sizeof(deviceId_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapGetConnectionHandleFromDeviceIdEvtMonitor
(
    uint16_t    connHandle
)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapEvtGetConnectionHandleFromDeviceIdOpCode_c, sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromUint16Value(connHandle, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapLoadKeysEvtMonitor
(
    gapSmpKeys_t     *pOutKeys,
    gapSmpKeyFlags_t *pOutKeyFlags,
    bool_t           *pOutLeSc,
    bool_t           *pOutAuth
)
{
    clientPacketStructured_t   *pClientPacket = NULL;
    uint8_t                    *pBuffer = NULL;

#if gFsciBleTest_d

    /* If GAP is disabled the event must be not monitored */
    if (FALSE == bFsciBleGapEnabled)
    {
        return;
    }

#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapEvtLoadKeysOpCode_c,
                                              fsciBleGapGetSmpKeysBufferSize(pOutKeys) +
                                              sizeof(gapSmpKeyFlags_t) + 2U * sizeof(bool_t));

    if (NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGapGetBufferFromSmpKeys(pOutKeys, &pBuffer);
    fsciBleGetBufferFromEnumValue(*pOutKeyFlags, pBuffer, gapSmpKeyFlags_t);
    fsciBleGetBufferFromBoolValue(*pOutLeSc, pBuffer);
    fsciBleGetBufferFromBoolValue(*pOutAuth, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapGenericEvtMonitor(gapGenericEvent_t* pGenericEvent)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    int16_t                     tempOpCode = 0;
    fsciBleGapOpCode_t          opCode = gBleGapModeSelectOpCode_c;
    opGroup_t                   opGroup = gFsciBleGapOpcodeGroup_c;
    bool_t earlyReturn = FALSE;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if ((FALSE == bFsciBleGapEnabled) &&
        (gInitializationComplete_c != pGenericEvent->eventType))
    {
        return;
    }
#endif /* gFsciBleTest_d */
    
    /* Get FSCI opCode */
    tempOpCode = maGenericEventToOpcode[pGenericEvent->eventType];
    
    if ((tempOpCode == -1) && (pGenericEvent->eventType > gLastGapGenericEventOpCode_c))
    {
        /* Use GAP2 opgroup */
        opGroup = gFsciBleGap2OpcodeGroup_c;
        tempOpCode = maGenericEvent2ToOpcode[pGenericEvent->eventType - (gLastGapGenericEventOpCode_c + 1U)];
    }
    
    if(tempOpCode == -1)
    {
#if gFsciBleGapHandoverLayerEnabled_d
        fsciBleGapHandoverGenericEvtMonitor(pGenericEvent);
#else /* gFsciBleGapHandoverLayerEnabled_d */
        /* Unknown event type */
        fsciBleError(gFsciError_c, fsciBleInterfaceId);
#endif /* gFsciBleGapHandoverLayerEnabled_d */
        earlyReturn = TRUE;
    }
    else
    {
        opCode = (fsciBleGapOpCode_t)tempOpCode;
    }

    if(!earlyReturn)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket =  fsciBleAllocFsciPacket(opGroup, (uint8_t)opCode, fsciBleGapGetGenericEventBufferSize(pGenericEvent));

        if(NULL == pClientPacket)
        {
            return;
        }

        pBuffer = &pClientPacket->payload[0];

        /* Set event parameters in the buffer */
        fsciBleGapGetBufferFromGenericEvent(pGenericEvent, &pBuffer);

        /* Transmit the packet over UART */
        fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
    }
}


void fsciBleGapAdvertisingEvtMonitor(gapAdvertisingEvent_t* pAdvertisingEvent)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    fsciBleGapOpCode_t          opCode = gBleGapModeSelectOpCode_c;
    bool_t earlyReturn = FALSE;

#if gFsciBleTest_d
   /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Get FSCI opCode */
    switch(pAdvertisingEvent->eventType)
    {
        case gAdvertisingStateChanged_c:
            {
                opCode = gBleGapEvtAdvertisingEventAdvertisingStateChangedOpCode_c;
            }
            break;

        case gAdvertisingCommandFailed_c:
            {
                opCode = gBleGapEvtAdvertisingEventAdvertisingCommandFailedOpCode_c;
            }
            break;
#if defined(gBLE50_d) && (gBLE50_d == 1U)
        case gExtAdvertisingStateChanged_c:
            {
                opCode = gBleGapEvtAdvertisingEventExtAdvertisingStateChangedOpCode_c;
            }
            break;

        case gAdvertisingSetTerminated_c:
            {
                opCode = gBleGapEvtAdvertisingEventAdvertisingSetTerminatedOpCode_c;
            }
            break;

        case gExtScanNotification_c:
            {
                opCode = gBleGapEvtAdvertisingEventExtScanReqReceivedOpCode_c;
            }
            break;
#endif
        default:
            {
                /* Unknown event type */
                fsciBleError(gFsciError_c, fsciBleInterfaceId);
                earlyReturn = TRUE;
                break;
            }
    }

    if (!earlyReturn)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)opCode,
                                                  fsciBleGapGetAdvertisingEventBufferSize(pAdvertisingEvent));

        if(NULL == pClientPacket)
        {
            return;
        }

        pBuffer = &pClientPacket->payload[0];

        /* Set event parameters in the buffer */
        fsciBleGapGetBuffFromAdvEvent(pAdvertisingEvent, &pBuffer);

        /* Transmit the packet over UART */
        fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
    }
}


void fsciBleGapScanningEvtMonitor(gapScanningEvent_t* pScanningEvent)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    fsciBleGapOpCode_t          opCode = gBleGapModeSelectOpCode_c;
    bool_t earlyReturn = FALSE;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Get FSCI opCode */
    switch(pScanningEvent->eventType)
    {
        case gScanStateChanged_c:
            {
                opCode = gBleGapEvtScanningEventScanStateChangedOpCode_c;
            }
            break;

        case gScanCommandFailed_c:
            {
                opCode = gBleGapEvtScanningEventScanCommandFailedOpCode_c;
            }
            break;

        case gDeviceScanned_c:
            {
                opCode = gBleGapEvtScanningEventDeviceScannedOpCode_c;
            }
            break;

#if defined(gBLE50_d) && (gBLE50_d == 1U)
        case gExtDeviceScanned_c:
            {
                opCode = gBleGapEvtScanningEventExtDeviceScannedOpCode_c;
            }
            break;

        case gPeriodicDeviceScanned_c:
            {
                opCode = gBleGapEvtScanningEventPeriodicDeviceScannedOpCode_c;
            }
            break;

        case gPeriodicAdvSyncEstablished_c:
            {
                opCode = gBleGapEvtScanningEventPeriodicAdvSyncEstablishedOpCode_c;
            }
            break;

        case gPeriodicAdvSyncTerminated_c:
            {
                opCode = gBleGapEvtScanningEventPeriodicAdvSyncTerminatedOpCode_c;
            }
            break;

        case gPeriodicAdvSyncLost_c:
            {
                opCode = gBleGapEvtScanningEventPeriodicAdvSyncLostOpCode_c;
            }
            break;
#endif
#if defined(gBLE51_d) && (gBLE51_d == 1U)
        case gConnectionlessIqReportReceived_c:
            {
                opCode = gBleGapEvtScanningEventConnectionlessIqReportReceivedOpCode_c;
            }
            break;
#endif
        default:
            {
                /* Unknown event type */
                fsciBleError(gFsciError_c, fsciBleInterfaceId);
                earlyReturn = TRUE;
                break;
            }
    }

    if (!earlyReturn)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)opCode,
                                                  fsciBleGapGetScanningEventBufferSize(pScanningEvent));

        if(NULL == pClientPacket)
        {
            return;
        }

        pBuffer = &pClientPacket->payload[0];

        /* Set event parameters in the buffer */
        fsciBleGapGetBufferFromScanningEvent(pScanningEvent, &pBuffer);

        /* Transmit the packet over UART */
        fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
    }
}

void fsciBleGapConnectionEvtMonitor(deviceId_t deviceId, gapConnectionEvent_t* pConnectionEvent)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    int16_t                     tempOpCode = 0;
    fsciBleGapOpCode_t          opCode = gBleGapModeSelectOpCode_c;
    bool_t earlyReturn = FALSE;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if(FALSE == bFsciBleGapEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Get FSCI opCode */
    tempOpCode = maConnectionEventToOpcode[pConnectionEvent->eventType];
    if(tempOpCode == -1)
    {
        /* Unknown event type */
        earlyReturn= TRUE;
    }
    else
    {  
        opCode = (fsciBleGapOpCode_t)tempOpCode;
    }

    if(!earlyReturn)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)opCode,
                                                  fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                  fsciBleGapGetConnectionEventBufferSize(pConnectionEvent));

        if(NULL == pClientPacket)
        {
            return;
        }

        pBuffer = &pClientPacket->payload[0];

        /* Set event parameters in the buffer */
        fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
        fsciBleGapGetBufferFromConnectionEvent(pConnectionEvent, &pBuffer);

        /* Transmit the packet over UART */
        fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
    }
}

#endif /* gFsciBleBBox_d || gFsciBleTest_d */

void fsciBleGapAdvertisingCallback(gapAdvertisingEvent_t* pAdvertisingEvent)
{
    fsciBleGapAdvertisingEvtMonitor(pAdvertisingEvent);
}


void fsciBleGapScanningCallback(gapScanningEvent_t* pScanningEvent)
{
    fsciBleGapScanningEvtMonitor(pScanningEvent);
}


void fsciBleGapConnectionCallback(deviceId_t deviceId, gapConnectionEvent_t* pConnectionEvent)
{
    fsciBleGapConnectionEvtMonitor(deviceId, pConnectionEvent);
#if defined(gFsciBleGapHandoverLayerEnabled_d) && (gFsciBleGapHandoverLayerEnabled_d == 1U)
    fsciBleGapHandoverConnectionEvtMonitor(deviceId, pConnectionEvent);
#endif /* defined(gHandoverSupportEnabled_d) && (gHandoverSupportEnabled_d == 1U) */
}

#if defined(gBLE50_d) && (gBLE50_d == 1U)
#if gFsciBleHost_d || gFsciBleTest_d
void fsciBleGapUpdatePeriodicAdvListCmdMonitor(gapPeriodicAdvListOperation_t operation, bleAddressType_t addrType, uint8_t* pAddr, uint8_t SID)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGapEnabled) ||
       (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    if (operation != gRemoveAllDevices_c)
    {
        pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdUpdatePeriodicAdvListOpCode_c,
                                                     sizeof(gapPeriodicAdvListOperation_t) + sizeof(bleAddressType_t) + gcBleDeviceAddressSize_c + sizeof(uint8_t));
    }
    else
    {
        pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdUpdatePeriodicAdvListOpCode_c, sizeof(gapPeriodicAdvListOperation_t));
    }

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromEnumValue(operation, pBuffer, gapPeriodicAdvListOperation_t);
    /* the clear operation does not require parameters */
    if (operation != gRemoveAllDevices_c)
    {
        fsciBleGetBufferFromEnumValue(addrType, pBuffer, bleAddressType_t);
        fsciBleGetBufferFromAddress(pAddr, pBuffer);
        fsciBleGetBufferFromUint8Value(SID, pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGapPeriodicAdvCreateSyncCmdMonitor
(
    gapPeriodicAdvSyncReq_t * pReq
)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
        /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
        if((FALSE == bFsciBleGapEnabled) ||
           (TRUE == bFsciBleGapCmdInitiatedByFsci))
        {
            return;
        }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapCmdPeriodicAdvCreateSyncOpCode_c,
                                              fsciBleGapGetPeriodicAdvSyncReqParametersBufferSize(pReq));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGapGetBufferFromPerAdvSyncReq(pReq, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}
#endif /* gFsciBleHost_d || gFsciBleTest_d */
#endif /* gBLE50_d */

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

static void fsciBleGapGenericCallback(gapGenericEvent_t* pGenericEvent)
{
    fsciBleGapGenericEvtMonitor(pGenericEvent);
}


static bleResult_t fsciBleHciHostToControllerInterface(hciPacketType_t packetType, void* pPacket, uint16_t packetSize)
{
    return gBleSuccess_c;
}

/*! *********************************************************************************
* \brief        Configures Bluetooth Address
*
********************************************************************************** */
static bleResult_t fsciWritePublicDeviceAddress(bleDeviceAddress_t bdAddress)
{
    bleResult_t retStatus = gBleSuccess_c;
#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
#else
    hardwareParameters_t *pHWParams==NULL;
    uint8_t HWParam_status = 0U;
    HWParam_status = NV_ReadHWParameters(&pHWParams);
    if(HWParam_status != 0U)
    {
        retStatus = gBleOsError_c;
    }
    else
    {
        if (!FLib_MemCmp(bdAddress, pHWParams->bluetooth_address, gcBleDeviceAddressSize_c))
        {
            FLib_MemCpy(pHWParams->bluetooth_address, bdAddress, gcBleDeviceAddressSize_c);
            if(NV_WriteHWParameters() != 0U)
            {
                retStatus = gBleOsError_c;
            }
        }
    }
#endif /* defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0) */
    return retStatus;
}

/*! *********************************************************************************
 * \brief        Handles timer callback.
 *
 * \param[in]    void
 *
 * \return       none
 ********************************************************************************** */
static void WritePublicDeviceAddress_TmrCb (void *param)
{
    (void)TM_Stop((timer_handle_t)gAppTimerId);
    /* Reset the MCU */
    HAL_ResetMCU();
}

#if defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U) && (defined(CPU_KW45B41Z83AFTA) || defined(CPU_K32W1480VFTA))
/*! *********************************************************************************
 * \brief       Error callback used to handle error at platform level.
 *
 * \param[in]   id of the caller of the callback.
 * \param[in]   error_status given by the caller of the callback.
 *
 * \return      none
 ********************************************************************************** */
static void fsciPlatformErrorCallback(uint32_t id, int32_t error_status)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if ((FALSE == bFsciBleGapEnabled) ||
        (TRUE == bFsciBleGapCmdInitiatedByFsci))
    {
        return;
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGapAllocFsciPacket((uint8_t)gBleGapEvtPlatformError_c,
                                              (sizeof(id) + sizeof(error_status)));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value((uint8_t)id, pBuffer);
    fsciBleGetBufferFromUint32Value(error_status, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}
#endif /* defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U) && (defined(CPU_KW45B41Z83AFTA) || defined(CPU_K32W1480VFTA)) */

#endif /* gFsciIncluded_c && gFsciBleGapLayerEnabled_d */

/*! *********************************************************************************
* @}
********************************************************************************** */
