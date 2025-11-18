/*! *********************************************************************************
 * \defgroup Digital Key Car CS Anchor
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* \file digital_key_car_anchor_cs.h
*
* Copyright 2022 - 2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef DIGITAL_KEY_CAR_ANCHOR_CS_H
#define DIGITAL_KEY_CAR_ANCHOR_CS_H

#include "fsl_component_timer_manager.h"

#include "gap_handover_types.h"

/* Profile / Services */
#include "digital_key_interface.h"
#include "app_handover.h"
#include "app_a2b.h"
#include "app_scanner.h"
#include "app_advertiser.h"
#include "app_localization.h"
#include "channel_sounding.h"
/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/

/* Profile Parameters */
#define gReducedPowerMinAdvInterval_c   1600 /* 1 s */
#define gReducedPowerMaxAdvInterval_c   4000 /* 2.5 s */

#define gAdvTime_c                      30 /* 30 s*/
#define gGoToSleepAfterDataTime_c       5 /* 5 s*/

#ifndef gAppDeepSleepMode_c
#define gAppDeepSleepMode_c 1
#endif
/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef enum appEvent_tag{
    mAppEvt_ConfigComplete_c,
    mAppEvt_GenericCallback_PeerDisconnected_c,
    mAppEvt_GenericCallback_LePhyEvent_c,
    mAppEvt_GenericCallback_LeScLocalOobData_c,
    mAppEvt_GenericCallback_BondCreatedEvent_c,
    mAppEvt_AdvertisingCallback_ExtAdvertisingStateChanged_c,
    mAppEvt_ConnectionCallback_EvtLeDataLengthChanged_c,
    mAppEvt_L2capPsmDataCallback_c,
    mAppEvt_ScanningCallback_ExtDeviceScanned_c,
    mAppEvt_ScanningCallback_ScanStateChanged_c,
    mAppEvt_Shell_Reset_Command_c,
    mAppEvt_Shell_FactoryReset_Command_c,
    mAppEvt_Shell_ShellStartDiscoveryOP_Command_c,
    mAppEvt_Shell_ShellStartDiscoveryPE_Command_c,
    mAppEvt_Shell_StopDiscovery_Command_c,
    mAppEvt_Shell_Disconnect_Command_c,
    mAppEvt_Shell_TriggerTimeSync_Command_c,
    mAppEvt_Shell_SetBondingData_Command_c,
    mAppEvt_Shell_ListBondedDev_Command_c,
    mAppEvt_Shell_RemoveBondedDev_Command_c,
    mAppEvt_Shell_ListActiveDev_Command_c,
    mAppEvt_Shell_SetCsConfigParams_Command_c,
    mAppEvt_Shell_SetCsProcedureParams_Command_c,
    mAppEvt_Shell_TriggerCsDistanceMeasurement_Command_c,
    mAppEvt_Shell_SetVerbosityLevel_Command_c,
    mAppEvt_Shell_SetAlgorithm_Command_c,
    mAppEvt_Shell_SetNumProcs_Command_c,
    mAppEvt_AdvertisingStartedLegacy_c,
    mAppEvt_AdvertisingStartedExtendedLR_c,
    mAppEvt_AdvertisingStopped_c,
    mAppEvt_BleConfigDone_c,
    mAppEvt_BleScanning_c,
    mAppEvt_BleScanStopped_c,
    mAppEvt_BleConnectingToDevice_c,
    mAppEvt_SPAKERequestSent_c,
    mAppEvt_SPAKEVerifySent_c,
    mAppEvt_LePhyEvent_c,
    mAppEvt_PeerConnected_c,
    mAppEvt_PeerDisconnected_c,
    mAppEvt_EncryptionChanged_c,
    mAppEvt_PairingComplete_c,
    mAppEvt_PairingReqRcv_c,
    mAppEvt_PairingLocalOobData_c,
    mAppEvt_PairingPeerOobDataReq_c,
    mAppEvt_PairingPeerOobDataRcv_c,
    mAppEvt_ServiceDiscoveryComplete_c,
    mAppEvt_ServiceDiscoveryFailed_c,
    mAppEvt_GattProcComplete_c,
    mAppEvt_GattProcError_c,
    mAppEvt_ReadCharacteristicValueComplete_c,
    mAppEvt_WriteCharacteristicDescriptorComplete_c,
    mAppEvt_WriteCharacteristicValueComplete_c,
    mAppEvt_ExchangeMtuComplete_c,
    mAppEvt_PsmChannelCreated_c,
    mAppEvt_OwnerPairingRequestReceived_c,
    mAppEvt_ReceivedSPAKEResponse_c,
    mAppEvt_BlePairingReady_c,
    mAppEvt_FactoryReset_c,
    mAppEvt_Shell_HandoverTimeSync_Command_c,
    mAppEvt_Shell_Handover_Command_c,
    mAppEvt_Shell_HandoverSendL2cap_Command_c,
    mAppEvt_Shell_HandoverStartAnchorMonitor_Command_c,
    mAppEvt_Shell_HandoverStopAnchorMonitor_Command_c,
    mAppEvt_Shell_HandoverStarted_c,
    mAppEvt_Shell_AnchorMonitorEventReceived_c,
    mAppEvt_Shell_PacketMonitorEventReceived_c,
    mAppEvt_Shell_PacketMonitorContinueEventReceived_c,
    mAppEvt_Shell_HandoverCompleteConnected_c,
    mAppEvt_Shell_HandoverCompleteDisconnected_c,
    mAppEvt_Shell_HandoverError_c,
    mAppEvt_Shell_A2BKeyDerivationComplete_c,
    mAppEvt_Shell_A2BLocalIrkSyncComplete_c,
    mAppEvt_Shell_A2BError_c,
    mAppEvt_L2capPsmChannelStatusNotification_c,
    mAppEvt_BtcsRangingProcResCfg_c,
}appEvent_t;

typedef struct appEventL2capPsmData_tag
{
    deviceId_t     deviceId;
    uint16_t       lePsm;
    uint16_t       packetLength;
    uint8_t*       pPacket;
}appEventL2capPsmData_t;

typedef struct appEventLeDataLengthChanged_tag
{
    deviceId_t peerDeviceId;
    gapConnLeDataLengthChanged_t pLeDataLengthChangedEvent;
}appEventLeDataLengthChanged_t;

typedef struct appConnectionCallbackEventData_tag
{
    deviceId_t peerDeviceId;
    gapConnectedEvent_t pConnectedEvent;
}appConnectionCallbackEventData_t;

typedef struct appAnchorMonitorStartEvent_tag
{
    deviceId_t                      deviceId;
    bleHandoverAnchorSearchMode_t   monitorMode;
}appAnchorMonitorStartEvent_t;

typedef struct appEventData_tag
{
    appEvent_t   appEvent;              /*!< Event type. */
    deviceId_t   peerDeviceId;          /*!< Event triggered regarding this device. */
    union {
        void*                                       pData;
        deviceId_t                                  peerDeviceId;
        appHandoverError_t                          handoverError;
        appA2BError_t                               a2bError;
        appHandoverAnchorMonitorEvent_t                 anchorMonitorEvent;
        appHandoverAnchorMonitorPacketEvent_t           anchorPacketEvent;
        appHandoverAnchorMonitorPacketContinueEvent_t   anchorPacketContinueEvent;
        bool_t                                      handoverTimeSync;
        uint8_t                                     verbosityLevel;
        uint8_t                                     algorithmSelection;
        appAnchorMonitorStartEvent_t                monitorStart;
        uint16_t                                    numProcedures;
    } eventData;                        /*!< Event data, selected according to event type. */
}appEventData_t;

typedef struct appCustomInfo_tag
{
    uint16_t     psmChannelId;
    /* Add persistent information here */
}appCustomInfo_t;

typedef enum appState_tag{
    mAppIdle_c,
    mAppExchangeMtu_c,
    mAppServiceDisc_c,
    mAppCCCWaitingForOwnerPairingRequest_c,
    mAppCCCPhase2WaitingForResponse_c, /* certificate exchange */
    mAppCCCPhase2WaitingForVerify_c,
    mAppCCCReadyForPairing_c,
    mAppPair_c,
    mAppLocalizationSetup_c,
    mAppLocalization_c,
    mAppRunning_c
}appState_t;

typedef struct appPeerInfo_tag
{
    deviceId_t                  deviceId;
    gapRole_t                   gapRole;
    appCustomInfo_t             customInfo;
    bool_t                      isBonded;
    uint8_t                     nvmIndex;
    bool_t                      isLinkEncrypted;
    bool_t                      csSecurityEnabled;
    bool_t                      csCapabWritten;
    appState_t                  appState;
    gapLeScOobData_t            oobData;
    gapLeScOobData_t            peerOobData;
    gapDisconnectionReason_t    disconReason;
}appPeerInfo_t;

/* APP -  pointer to function for BLE events*/
typedef void (*pfBleCallback_t)(void* pData);
/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */
extern gapAdvertisingData_t             gAppAdvertisingData;
extern gapAdvertisingData_t             gAppAdvertisingDataEmpty;
extern gapScanResponseData_t            gAppScanRspData;
extern gapExtAdvertisingParameters_t  gExtAdvParams;
extern gapExtAdvertisingParameters_t  gLegacyAdvParams;
extern appExtAdvertisingParams_t        gAppAdvParams;
/* This global will be TRUE if the user adds or removes a bond */
extern bool_t                           gPrivacyStateChangedByUser;

/* Time Sync UWB Device Time. Demo value. */
extern uint64_t                         mTsUwbDeviceTime;
/* Table with peer devices information */
extern appPeerInfo_t maPeerInformation[gAppMaxConnections_c];

#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
/* TRUE if the last connect came from a handover procedure */
extern bool_t mLastConnectFromHandover;
/* Last deviceId connected to via handover */
extern deviceId_t gLastHandedOverPeerId;
#endif

extern uint16_t gFilterShellVal;
extern bool_t   gFilterTestSend;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
void BleApp_RegisterEventHandler(pfBleCallback_t pfBleEventHandler);
void BleApp_RegisterUserInterfaceEventHandler(pfBleCallback_t pfUserInterfaceEventHandler);
void BleApp_Start(void);
void BleApp_GenericCallback (gapGenericEvent_t* pGenericEvent);
void BleApp_ConnectionCallback (deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent);
void App_HandleLeScLocalOobDataCallback(appEventData_t *pEventData);
void BluetoothLEHost_Initialized(void);
void BleApp_FactoryReset(void);
void BleApp_OP_Start(void);
void BleApp_PE_Start(void);
void BleApp_StopDiscovery(void);
void BleApp_Disconnect(void);
deviceId_t BleApp_SelectDeviceIdForHandover(void);
bool_t BleApp_CheckActiveConnections(void);

void BleApp_StateMachineHandler(deviceId_t peerDeviceId, appEvent_t event);
/* CCC Phase 2 */
bleResult_t CCCPhase2_SendSPAKERequest(deviceId_t deviceId, uint8_t *pData, uint16_t dataLen);
bleResult_t CCCPhase2_SendSPAKEVerify(deviceId_t deviceId, uint8_t *pData, uint16_t dataLen);
/* CCC SubEvents */
bleResult_t CCC_SendSubEvent(deviceId_t deviceId, dkSubEventCategory_t category, dkSubEventCommandCompleteType_t type);
/* CCC Pairing */
bleResult_t CCC_FirstApproachRsp(deviceId_t deviceId, uint8_t* pBdAddr, gapLeScOobData_t* pOobData);
#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
/* Send BTCS Ranging Procedure Results Config message */
bleResult_t SendBTCSRangingProcResCfg
(
    deviceId_t deviceId,
    bool_t     bEnableProcResTransfer
);
#endif /* defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U) */

#ifdef __cplusplus
}
#endif


#endif /* DIGITAL_KEY_CAR_ANCHOR_CS_H */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
