/*! *********************************************************************************
* \addtogroup Digital Key Car Anchor CS
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file digital_key_car_anchor_cs.c
*
* Copyright 2022 - 2026 NXP
*
* NXP Proprietary
*
* This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or
* by downloading, installing, activating and/or otherwise using the software, you are
* agreeing that you have read, and that you agree to comply with and are bound by,
* such license terms. If you do not agree to be bound by the applicable license terms,
* then you may not retain, install, activate or otherwise use the software.
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
/* Framework / Drivers */
#include "EmbeddedTypes.h"
#include "fsl_component_button.h"
#include "fsl_component_timer_manager.h"
#include "fsl_component_panic.h"
#include "FunctionLib.h"
#include "fsl_component_mem_manager.h"
#include "fsl_adapter_reset.h"
#include "fsl_debug_console.h"
#include "app.h"
#include "board.h"
#include "fwk_platform_ble.h"
#include "NVM_Interface.h"
#include "RNG_Interface.h"

/* BLE Host Stack */
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gap_interface.h"
#include "gatt_db_app_interface.h"
#include "gatt_db_handles.h"

/* Profile / Services */
#include "digital_key_interface.h"

#include "ble_conn_manager.h"
#include "ble_service_discovery.h"

#include "app_conn.h"
#include "digital_key_car_anchor_cs.h"
#include "shell_digital_key_car_anchor_cs.h"
#include "app_digital_key_car_anchor_cs.h"

#include "gap_handover_types.h"
#include "gap_handover_interface.h"
#include "app_a2a_interface.h"
#include "app_handover.h"
#include "app_localization.h"
#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
#include "app_localization_algo.h"
#endif /* defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1U) */

#include "channel_sounding.h"

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
/* Time Sync UWB Device Time. Demo value. */
uint64_t mTsUwbDeviceTime = 0U;

/* Table with peer devices information */
appPeerInfo_t maPeerInformation[gAppMaxConnections_c];

/* This global will be TRUE if the user adds or removes a bond */
bool_t gPrivacyStateChangedByUser = FALSE;

#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
/* Save the value of the last deviceId connected to via handover */
deviceId_t gLastHandedOverPeerId = gInvalidDeviceId_c;
#endif

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef struct advState_tag{
    bool_t      advOn;
}advState_t;
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
/* Own address used during discovery. Included in First Approach Response. */
static uint8_t gaAppOwnDiscAddress[gcBleDeviceAddressSize_c];

/* Application timer*/
static TIMER_MANAGER_HANDLE_DEFINE(mL2caTimerId);
static bool_t mL2caTimerValid = FALSE;
#if defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1)
/* Owner Pairing Mode enabled */
static bool_t mOwnerPairingMode = TRUE;
#endif /* defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1) */

/* Current Advertising Set */
static uint8_t gCurrentAdvHandle = gLegacyAdvSetHandle_c;
/* Holds the identification information about the device are we doing OOB pairing with */
static deviceId_t mCurrentPeerId = gInvalidDeviceId_c;
/* Application specific data*/
static deviceId_t mDeviceIdToDisconnect;

/* application callback */
static pfBleCallback_t mpfBleEventHandler = NULL;
static pfBleCallback_t mpfBleUserInterfaceEventHandler = NULL;

/* Buffer used for Characteristic related procedures */
static gattAttribute_t      *mpCharProcBuffer = NULL;
#if defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1)
static bool_t gStopExtAdvSetAfterConnect = FALSE;
#endif /* defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1) */
static bleDeviceAddress_t mRandomStaticAddr = APP_BD_ADDR;
/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
static void BleApp_GattClientCallback
(
    deviceId_t              serverDeviceId,
    gattProcedureType_t     procedureType,
    gattProcedureResult_t   procedureResult,
    bleResult_t             error
);
static void BleApp_GattServerCallback (deviceId_t deviceId, gattServerEvent_t* pServerEvent);

static void L2caTimerCallback(void *param);

/* PSM callbacks */
static void BleApp_L2capPsmDataCallback (deviceId_t deviceId, uint16_t lePsm, uint8_t* pPacket, uint16_t packetLength);
static void BleApp_L2capPsmControlCallback (l2capControlMessage_t* pMessage);

static void BleApp_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent);
static void BleApp_Advertise(void);

static uint8_t BleApp_GetNoOfActiveConnections(void);

static void BleApp_StateMachineHandler_AppPair(deviceId_t peerDeviceId, appEvent_t event);
static void BleApp_StateMachineHandler_AppIdle(deviceId_t peerDeviceId, appEvent_t event);
static void BleApp_StateMachineHandler_AppCCCWaitingForOwnerPairingRequest(deviceId_t peerDeviceId, appEvent_t event);
static void BleApp_StateMachineHandler_AppCCCPhase2WaitingForResponse(deviceId_t peerDeviceId, appEvent_t event);
static void BleApp_StateMachineHandler_LocalizationSetup(deviceId_t peerDeviceId, appEvent_t event);

static void BleApp_GenericCallback_ControllerNotificationEvent(gapGenericEvent_t* pGenericEvent);
static void BleApp_GenericCallback_HandlePrivacyEvents(gapGenericEvent_t* pGenericEvent);
/* Get a simulated UWB clock. */
static uint64_t GetUwbClock(void);

#if defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1) && \
    defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
static void A2A_SendApplicationData(deviceId_t deviceId);
#endif /* defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1) */
#if (defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U))
static void BleApp_A2BCommHandler(uint8_t opGroup, uint8_t cmdId, uint16_t len, uint8_t *pData);
#endif /* (defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)) */

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
 * \brief        Register function to handle events from BLE to APP
 *
 * \param[in]    pfBleEventHandler   event handler
 ********************************************************************************** */
void BleApp_RegisterEventHandler(pfBleCallback_t pfBleEventHandler)
{
    mpfBleEventHandler = pfBleEventHandler;
}

/*! *********************************************************************************
 * \brief        Register function to handle User Interface events from BLE to APP
 *
 * \param[in]    pfBleEventHandler   event handler
 ********************************************************************************** */
void BleApp_RegisterUserInterfaceEventHandler(pfBleCallback_t pfUserInterfaceEventHandler)
{
    mpfBleUserInterfaceEventHandler = pfUserInterfaceEventHandler;
}

/*! *********************************************************************************
* \brief    Starts the BLE application.
*
********************************************************************************** */
void BleApp_Start(void)
{
    Led1On();

    /* Set advertising parameters, advertising to start on gAdvertisingParametersSetupComplete_c */
    gCurrentAdvHandle = gLegacyAdvSetHandle_c;
    BleApp_Advertise();
}

/*! *********************************************************************************
* \brief        Get active peer for handover.
*
********************************************************************************** */
deviceId_t BleApp_SelectDeviceIdForHandover(void)
{
    static uint8_t devIdIdx = 0U;
    deviceId_t deviceId = gInvalidDeviceId_c;

    for (uint32_t i = 0U; i < ((uint32_t)gAppMaxConnections_c); i++)
    {
        devIdIdx++;

        if (devIdIdx == ((uint8_t)gAppMaxConnections_c))
        {
            devIdIdx = 0U;
        }

        if (maPeerInformation[devIdIdx].deviceId != gInvalidDeviceId_c)
        {
            deviceId = maPeerInformation[devIdIdx].deviceId;
            break;
        }
    }

    return deviceId;
}

/*! *********************************************************************************
* \brief        Helper function. Return TRUE if device has active connections.
*
********************************************************************************** */
bool_t BleApp_CheckActiveConnections(void)
{
    uint8_t mPeerId = 0;
    bool_t bActiveConn = FALSE;

    for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
    {
        if (maPeerInformation[mPeerId].deviceId != gInvalidDeviceId_c)
        {
            bActiveConn = TRUE;
            break;
        }
    }

    return bActiveConn;
}

/*! *********************************************************************************
* \brief        Handles BLE generic callback.
*
* \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
********************************************************************************** */
void BleApp_GenericCallback (gapGenericEvent_t* pGenericEvent)
{
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
    if ((TRUE == gA2ALocalIrkSet) ||
        ((FALSE == gA2ALocalIrkSet) && (pGenericEvent->eventType != gInitializationComplete_c)))
    {
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
    /* Call BLE Conn Manager */
    BleConnManager_GenericEvent(pGenericEvent);
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
    }
    else
    {
        /* Postpone processing of the gInitializationComplete_c generic event until the local IRK is set. */
        if (NULL == gpBleHostInitComplete)
        {
            gpBleHostInitComplete = MEM_BufferAlloc(sizeof(gapGenericEvent_t));
        }

        if (NULL != gpBleHostInitComplete)
        {
            FLib_MemCpy(gpBleHostInitComplete, pGenericEvent, sizeof(gapGenericEvent_t));
        }
        else
        {
            panic(0,0,0,0);
        }
    }
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
    switch (pGenericEvent->eventType)
    {
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
        case gHandoverAnchorSearchStarted_c:
        case gHandoverAnchorSearchStopped_c:
        case gHandoverAnchorMonitorEvent_c:
        case gHandoverSuspendTransmitComplete_c:
        case gHandoverGetComplete_c:
        case gGetConnParamsComplete_c:
        case gHandoverTimeSyncEvent_c:
        case gHandoverTimeSyncTransmitStateChanged_c:
        case gHandoverTimeSyncReceiveComplete_c:
        case gLlSkdReportEvent_c:
        case gInternalError_c:
        case gHandoverAnchorMonitorPacketEvent_c:
        case gHandoverAnchorMonitorPacketContinueEvent_c:
        case gHandoverFreeComplete_c:
        case gHandoverLlPendingData_c:
        case gHandoverConnectionUpdateProcedureEvent_c:
        case gHandoverApplyConnectionUpdateProcedureComplete_c:
        {
            AppHandover_GenericCallback(pGenericEvent);
        }
        break;
#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d == 1)
        case gHandoverGetCsLlContextComplete_c:
        {
            for (uint8_t idx = 0U; idx < ((uint8_t)gAppMaxConnections_c); idx++)
            {
                if (maPeerInformation[idx].deviceId == gHandoverDeviceId)
                {
                    A2A_SendApplicationData(maPeerInformation[idx].deviceId);
                    break;
                }
            }

            AppHandover_GenericCallback(pGenericEvent);
        }
        break;

        case gHandoverSetCsLlContextComplete_c:
        {
            AppHandover_GenericCallback(pGenericEvent);
            /* Set CS configuration locally and start procedure if initiator */
            if (gLastHandedOverPeerId != gInvalidDeviceId_c)
            {
                uint8_t nvmIndex = gInvalidNvmIndex_c;
                bool_t isBonded = FALSE;

                (void)Gap_CheckIfBonded(gLastHandedOverPeerId, &isBonded, &nvmIndex);

                if (nvmIndex != gInvalidNvmIndex_c)
                {
                    (void)AppLocalization_WriteCachedRemoteCapabilities(gLastHandedOverPeerId, nvmIndex);
                }
            }
        }
        break;
#endif /* gBLE_ChannelSounding_d */
#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1) */
        case gLePhyEvent_c:
        {
            /* Inform application about Le Phy user interface event */
            if (mpfBleUserInterfaceEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(gapPhyEvent_t));
                if (pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_LePhyEvent_c;
                    pEventData->eventData.pData = &pEventData[1];
                    FLib_MemCpy(pEventData->eventData.pData, &pGenericEvent->eventData.phyEvent, sizeof(gapPhyEvent_t));
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleUserInterfaceEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
        break;

        case gLeScLocalOobData_c:
        {
            /* Inform application about LE SC Local OOB Data event */
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(gapLeScOobData_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_GenericCallback_LeScLocalOobData_c;
                    pEventData->eventData.pData = &pEventData[1];
                    FLib_MemCpy(pEventData->eventData.pData, &pGenericEvent->eventData.localOobData, sizeof(gapLeScOobData_t));
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
        break;

        case gBondCreatedEvent_c:
        {
            /* Inform application about bond created event */
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(bleBondCreatedEvent_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_GenericCallback_BondCreatedEvent_c;
                    pEventData->eventData.pData = &pEventData[1];
                    FLib_MemCpy(pEventData->eventData.pData, &pGenericEvent->eventData.bondCreatedEvent, sizeof(bleBondCreatedEvent_t));
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
        break;

        case gRandomAddressReady_c:
        {
            if (pGenericEvent->eventData.addrReady.advHandle == gLegacyAdvSetHandle_c)
            {
                FLib_MemCpy(gaAppOwnDiscAddress, pGenericEvent->eventData.addrReady.aAddress, gcBleDeviceAddressSize_c);
            }
        }
        break;

        case gControllerNotificationEvent_c:
        {
            BleApp_GenericCallback_ControllerNotificationEvent(pGenericEvent);
        }
        break;

        case gHostPrivacyStateChanged_c:
        case gControllerPrivacyStateChanged_c:
        {
            BleApp_GenericCallback_HandlePrivacyEvents(pGenericEvent);
        }
        break;

        default:
        {
            ; /* No action required */
        }
        break;
    }
}


/*! *********************************************************************************
* \brief        Handles BLE Connection callback from host stack.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    pConnectionEvent    Pointer to gapConnectionEvent_t.
********************************************************************************** */
void BleApp_ConnectionCallback (deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent)
{
    if (pConnectionEvent->eventType == gConnEvtConnected_c)
    {
        if (pConnectionEvent->eventData.connectedEvent.connectionRole == gBleLlConnectionCentral_c)
        {
            maPeerInformation[peerDeviceId].gapRole = gGapCentral_c;
        }
        else
        {
            maPeerInformation[peerDeviceId].gapRole = gGapPeripheral_c;
        }
    }

    /* Connection Manager to handle Host Stack interactions */
    if (maPeerInformation[peerDeviceId].gapRole == gGapCentral_c)
    {
        BleConnManager_GapCentralEvent(peerDeviceId, pConnectionEvent);
    }
    else
    {
        BleConnManager_GapPeripheralEvent(peerDeviceId, pConnectionEvent);
    }

    switch (pConnectionEvent->eventType)
    {
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
        case gConnEvtHandoverConnected_c:
        {
            gLastHandedOverPeerId = peerDeviceId;
            AppHandover_ConnectionCallback(peerDeviceId, pConnectionEvent);
        }
        break;
        case gHandoverDisconnected_c:
        {
            if (gLastHandedOverPeerId == peerDeviceId)
            {
                gLastHandedOverPeerId = gInvalidDeviceId_c;
            }
            AppHandover_ConnectionCallback(peerDeviceId, pConnectionEvent);
        }
        break;
#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1) */
        case gConnEvtConnected_c:
        {
#if defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1)
            bleResult_t result = gBleSuccess_c;
#endif /* defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1) */
            maPeerInformation[peerDeviceId].isBonded = FALSE;
            maPeerInformation[peerDeviceId].nvmIndex = gInvalidNvmIndex_c;

            (void)Gap_CheckIfBonded(peerDeviceId, &maPeerInformation[peerDeviceId].isBonded, &maPeerInformation[peerDeviceId].nvmIndex);

            /* Save address used during discovery if controller privacy was used. */
            if (pConnectionEvent->eventData.connectedEvent.localRpaUsed)
            {
                FLib_MemCpy(gaAppOwnDiscAddress, pConnectionEvent->eventData.connectedEvent.localRpa, gcBleDeviceAddressSize_c);
            }

#if defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1)
            /* Advertising stops when connected - on all PHYs */
            result = Gap_StopExtAdvertising(0xFF);

            if ((result == gBleInvalidState_c) && (gCurrentAdvHandle == gExtendedAdvSetHandle_c))
            {
                /* Connection was establised before the extended advertising set was started */
                gStopExtAdvSetAfterConnect = TRUE;
            }
            /* Reset gCurrentAdvHandle */
            gCurrentAdvHandle = gNoAdvSetHandle_c;

#endif /* defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1) */

            maPeerInformation[peerDeviceId].deviceId = peerDeviceId;
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
            mLastConnectFromHandover = FALSE;
#endif

            BleApp_StateMachineHandler(maPeerInformation[peerDeviceId].deviceId, mAppEvt_PeerConnected_c);
            /* UI */

            LedStopFlashingAllLeds();

            /* UI */
            Led1On();

            appLocalization_rangeCfg_t locConfig;
            /* Read current CS config and update procedure repeat interval */
            (void)AppLocalization_ReadConfig(peerDeviceId, &locConfig);

            /* Estimated algo durations (4 AP, 80 channels) for procedure repeat frequency calculation */
            uint32_t algoDurationMs = 0U;
#if defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)
            algoDurationMs += 45U;
#endif /* gAppUseRADEAlgorithm_d */
#if defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)
            algoDurationMs += 25U;
#endif /* gAppUseCDEAlgorithm_d */

            uint32_t procInterval = (uint32_t)(gMaxCsProcDurationMs_c + gPostProcVerbDurationMs_c + gAppOffsetDurationMs_c + algoDurationMs);
#if defined (BOARD_LOCALIZATION_REVISION_SUPPORT) && (BOARD_LOCALIZATION_REVISION_SUPPORT == 1U)
            procInterval +=  gLocBoardDelayMs_c;
#endif
            /* Convert ms to connection intervals */
            procInterval = 1U + (procInterval * 1000U)/(((uint32_t)(pConnectionEvent->eventData.connectedEvent.connParameters.connInterval)) * 1250U);
            locConfig.minPeriodBetweenProcedures = (uint16_t)procInterval;
            locConfig.maxPeriodBetweenProcedures = (uint16_t)procInterval;

            (void)AppLocalization_WriteConfig(peerDeviceId, &locConfig); 
            AppLocalization_SetConnectionInterval(peerDeviceId, pConnectionEvent->eventData.connectedEvent.connParameters.connInterval);
            /* Read the PHY on which the connection was establihed */
            (void)Gap_LeReadPhy(peerDeviceId);

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
            AppLocalization_TimeInfoSetConnInterval(pConnectionEvent->eventData.connectedEvent.connParameters.connInterval);
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */
        }
        break;

        case gConnEvtDisconnected_c:
        {
            maPeerInformation[peerDeviceId].disconReason = pConnectionEvent->eventData.disconnectedEvent.reason;

            BleApp_StateMachineHandler(peerDeviceId, mAppEvt_PeerDisconnected_c);
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
            mLastConnectFromHandover = FALSE;
            maPeerInformation[peerDeviceId].csCapabWritten = FALSE;
            maPeerInformation[peerDeviceId].csSecurityEnabled = FALSE;
            if (gLastHandedOverPeerId == peerDeviceId)
            {
                gLastHandedOverPeerId = gInvalidDeviceId_c;
            }
#endif

            LedStartFlashingAllLeds();

        }
        break;

#if gAppUsePairing_d
        case gConnEvtPairingRequest_c:
        {
            BleApp_StateMachineHandler(maPeerInformation[peerDeviceId].deviceId, mAppEvt_PairingReqRcv_c);
        }
        break;

        case gConnEvtLeScOobDataRequest_c:
        {
            BleApp_StateMachineHandler(peerDeviceId, mAppEvt_PairingPeerOobDataReq_c);
        }
        break;

        case gConnEvtPairingComplete_c:
        {
            /* Notify state machine handler on pairing complete */
            if (pConnectionEvent->eventData.pairingCompleteEvent.pairingSuccessful)
            {
                if (pConnectionEvent->eventData.pairingCompleteEvent.pairingCompleteData.withBonding == TRUE)
                {
                    (void)Gap_CheckIfBonded(peerDeviceId, &maPeerInformation[peerDeviceId].isBonded, &maPeerInformation[peerDeviceId].nvmIndex);
                }
                BleApp_StateMachineHandler(maPeerInformation[peerDeviceId].deviceId, mAppEvt_PairingComplete_c);
            }
        }
        break;

#if defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U)
        case gConnEvtEncryptionChanged_c:
        {
            BleApp_StateMachineHandler(peerDeviceId, mAppEvt_EncryptionChanged_c);
        }
        break;
#endif /* gAppUseBonding_d */
#endif /* gAppUsePairing_d */

        case gConnEvtParameterUpdateComplete_c:
        {
            /* Update connection interval when a Parameter Update procedure completes */
            AppLocalization_SetConnectionInterval(peerDeviceId, pConnectionEvent->eventData.connectionUpdateComplete.connInterval);
        }
        break;

        default:
        {
            ; /* No action required */
        }
        break;
    }
}

/*! *********************************************************************************
* \brief        Handles LeScLocalOobDataCallback events.
*
* \param[in]    pEventData    pointer to appEventData_t.
********************************************************************************** */
void App_HandleLeScLocalOobDataCallback(appEventData_t *pEventData)
{
    if (mCurrentPeerId != gInvalidDeviceId_c)
    {
        FLib_MemCpy(&maPeerInformation[mCurrentPeerId].oobData, pEventData->eventData.pData, sizeof(gapLeScOobData_t));
        BleApp_StateMachineHandler(mCurrentPeerId, mAppEvt_PairingLocalOobData_c);
    }
}

/*! *********************************************************************************
* \brief        Handles Shell_Factory Reset Command event.
********************************************************************************** */
void BleApp_FactoryReset(void)
{
    /* Erase NVM Datasets */
    NVM_Status_t status = NvFormat();
    if (status != gNVM_OK_c)
    {
         /* NvFormat exited with an error status */
         panic(0, (uint32_t)BleApp_FactoryReset, 0, 0);
    }

    /* Reset MCU */
    HAL_ResetMCU();
}

/*! *********************************************************************************
* \brief    Starts the Passive Entry Scenario on the BLE application.
*
********************************************************************************** */
void BleApp_PE_Start(void)
{
#if defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1)
    mOwnerPairingMode = FALSE;
#endif /* defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1) */
    gAppAdvParams.pGapAdvData = &gAppAdvertisingDataEmpty;
    BleApp_Start();
}

/*! *********************************************************************************
* \brief    Starts the Owner Pairing Scenario on the BLE application.
*
********************************************************************************** */
void BleApp_OP_Start(void)
{
    FLib_MemSet(gaAppOwnDiscAddress, 0x00, gcBleDeviceAddressSize_c);
#if defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1)
    mOwnerPairingMode = TRUE;
#endif /* defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1) */
    gAppAdvParams.pGapAdvData = &gAppAdvertisingData;
    BleApp_Start();
}

/*! *********************************************************************************
* \brief    Stop Discovery on the BLE application.
*
********************************************************************************** */
void BleApp_StopDiscovery(void)
{
    gCurrentAdvHandle = gNoAdvSetHandle_c;
    (void)Gap_StopExtAdvertising(0xFF);
}

/*! *********************************************************************************
* \brief    Disconnect from each device in the peer table on the BLE application.
*
********************************************************************************** */
void BleApp_Disconnect(void)
{
    uint8_t peerId;

    for (peerId = 0; peerId < (uint8_t)gAppMaxConnections_c; peerId++)
    {
        if (maPeerInformation[peerId].deviceId != gInvalidDeviceId_c)
        {
            (void)Gap_Disconnect(maPeerInformation[peerId].deviceId);
        }
    }
}

/*! *********************************************************************************
* \brief        State machine handler of the Digital Key Car Anchor application.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
********************************************************************************** */
void BleApp_StateMachineHandler(deviceId_t peerDeviceId, appEvent_t event)
{
    switch (maPeerInformation[peerDeviceId].appState)
    {
        case mAppIdle_c:
        {
            BleApp_StateMachineHandler_AppIdle(peerDeviceId, event);
        }
        break;

        case mAppCCCWaitingForOwnerPairingRequest_c:
        {
            BleApp_StateMachineHandler_AppCCCWaitingForOwnerPairingRequest(peerDeviceId, event);
        }
        break;

        case mAppCCCPhase2WaitingForResponse_c:
        {
            BleApp_StateMachineHandler_AppCCCPhase2WaitingForResponse(peerDeviceId, event);
        }
        break;

        case mAppCCCPhase2WaitingForVerify_c:
        {
            if (event == mAppEvt_BlePairingReady_c)
            {
                maPeerInformation[peerDeviceId].appState = mAppCCCReadyForPairing_c;
            }
        }
        break;

        case mAppCCCReadyForPairing_c:
        {
            if ( event == mAppEvt_PairingPeerOobDataRcv_c )
            {
                mCurrentPeerId = peerDeviceId;
                (void)Gap_LeScGetLocalOobData();
                maPeerInformation[peerDeviceId].appState = mAppPair_c;
            }
        }
        break;

        case mAppPair_c:
        {
            BleApp_StateMachineHandler_AppPair(peerDeviceId,event);
        }
        break;

        case mAppServiceDisc_c:
        {
            if (event == mAppEvt_ServiceDiscoveryComplete_c)
            {
                /* Moving to Running State*/
                maPeerInformation[peerDeviceId].appState = mAppRunning_c;
#if gAppUseBonding_d
                /* Write data in NVM */
                (void)Gap_SaveCustomPeerInformation(maPeerInformation[peerDeviceId].deviceId,
                                                    (void *)&maPeerInformation[peerDeviceId].customInfo, 0,
                                                    (uint16_t)sizeof(appCustomInfo_t));
#endif
            }
            else if (event == mAppEvt_ServiceDiscoveryFailed_c)
            {
                (void)Gap_Disconnect(peerDeviceId);
            }
            else
            {
                /* ignore other event types */
            }
        }
        break;

        case mAppLocalizationSetup_c:
        {
            BleApp_StateMachineHandler_LocalizationSetup(peerDeviceId, event);
        }
        break;

        case mAppRunning_c:
        case mAppLocalization_c:
        {
            if (event == mAppEvt_WriteCharacteristicDescriptorComplete_c)
            {
                (void)MEM_BufferFree(mpCharProcBuffer);
                mpCharProcBuffer = NULL;
            }
            else if (event == mAppEvt_PeerDisconnected_c)
            {
                AppLocalization_ResetPeer(peerDeviceId, TRUE, maPeerInformation[peerDeviceId].nvmIndex);
#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
                AppLocalizationAlgo_ResetPeer(peerDeviceId);
#endif /* defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1U) */
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
                gHandoverDeviceId = gInvalidDeviceId_c;
#endif
            }
            else
            {
                /* ignore other event types */
            }
        }
        break;

        default:
        {
            ; /* No action required */
        }
        break;
    }

    /* Handle disconnect event in all application states. */
    if (event == mAppEvt_PeerDisconnected_c)
    {
        maPeerInformation[peerDeviceId].deviceId = gInvalidDeviceId_c;
        maPeerInformation[peerDeviceId].appState = mAppIdle_c;
        maPeerInformation[peerDeviceId].isLinkEncrypted = FALSE;
    }

    /* Inform the user interface handler about events received in the BleApp_StateMachineHandler */
    if(mpfBleUserInterfaceEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = event;
            pEventData->eventData.peerDeviceId = peerDeviceId;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleUserInterfaceEventHandler, pEventData))
            {
                (void)MEM_BufferFree(pEventData);
            }
        }
    }
}

/*! *********************************************************************************
 * \brief        Owner Pairing Certificate Exchange - step 1, SPAKE2+ Request Command
 *
 ********************************************************************************** */
bleResult_t CCCPhase2_SendSPAKERequest(deviceId_t deviceId, uint8_t *pData, uint16_t dataLen)
{
    bleResult_t result = gBleSuccess_c;

    result = DK_SendMessage(deviceId,
                            maPeerInformation[deviceId].customInfo.psmChannelId,
                            gDKMessageTypeFrameworkMessage_c,
                            (uint8_t)gDkApduRQ_c,
                            dataLen,
                            pData);

    /* Inform the user interface handler that SPAKE Request has been sent */
    if(mpfBleUserInterfaceEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_SPAKERequestSent_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleUserInterfaceEventHandler, pEventData))
            {
                (void)MEM_BufferFree(pEventData);
            }
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Owner Pairing Certificate Exchange - step 2, SPAKE2+ Verify Command
 *
 ********************************************************************************** */
bleResult_t CCCPhase2_SendSPAKEVerify(deviceId_t deviceId, uint8_t *pData, uint16_t dataLen)
{
    bleResult_t result = gBleSuccess_c;

    result = DK_SendMessage(deviceId,
                            maPeerInformation[deviceId].customInfo.psmChannelId,
                            gDKMessageTypeFrameworkMessage_c,
                            (uint8_t)gDkApduRQ_c,
                            dataLen,
                            pData);

    /* Inform the user interface handler that SPAKE Verify has been sent */
    if(mpfBleUserInterfaceEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_SPAKEVerifySent_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleUserInterfaceEventHandler, pEventData))
            {
                (void)MEM_BufferFree(pEventData);
            }
        }
    }
    return result;
}

/*! *********************************************************************************
 * \brief        First Approach messages enables BLE OOB Secure LE pairing for both owner and friend devices
 *
 ********************************************************************************** */
bleResult_t CCC_FirstApproachRsp(deviceId_t deviceId, uint8_t* pBdAddr, gapLeScOobData_t* pOobData)
{
    bleResult_t result = gBleSuccess_c;

    uint8_t aPayload[gFirstApproachReqRspPayloadLength] = {0};
    uint8_t *pData = aPayload;

    if (FLib_MemCmpToVal(pOobData, 0x00, sizeof(gapLeScOobData_t)))
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        FLib_MemCpy(pData, pBdAddr, gcBleDeviceAddressSize_c);
        pData = &pData[gcBleDeviceAddressSize_c];
        FLib_MemCpy(pData, pOobData->confirmValue, gSmpLeScRandomConfirmValueSize_c);
        pData = &pData[gSmpLeScRandomConfirmValueSize_c];
        FLib_MemCpy(pData, pOobData->randomValue, gSmpLeScRandomValueSize_c);

        result = DK_SendMessage(deviceId,
                                maPeerInformation[deviceId].customInfo.psmChannelId,
                                gDKMessageTypeSupplementaryServiceMessage_c,
                                (uint8_t)gFirstApproachRS_c,
                                gFirstApproachReqRspPayloadLength,
                                aPayload);
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Sends DK SubEvents to Device.
 *
 ********************************************************************************** */
bleResult_t CCC_SendSubEvent(deviceId_t deviceId,
                             dkSubEventCategory_t category,
                             dkSubEventCommandCompleteType_t type)
{
    bleResult_t result = gBleSuccess_c;

    uint8_t payload[gCommandCompleteSubEventPayloadLength_c] = {0}; /* SubEvent Category + SubEvent Type */
    payload[0] = (uint8_t)category;
    payload[1] = (uint8_t)type;

    result = DK_SendMessage(deviceId,
                            maPeerInformation[deviceId].customInfo.psmChannelId,
                            gDKMessageTypeDKEventNotification_c,
                            (uint8_t)gDkEventNotification_c,
                            gCommandCompleteSubEventPayloadLength_c,
                            payload);

    return result;
}

#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
/*! *********************************************************************************
 * \brief        Send BTCS Ranging Procedure Results Config message
 *
 ********************************************************************************** */
bleResult_t SendBTCSRangingProcResCfg
(
    deviceId_t deviceId,
    bool_t     bEnableProcResTransfer
)
{
    bleResult_t result = gBleSuccess_c;

    result = DK_SendMessage(deviceId,
                            maPeerInformation[deviceId].customInfo.psmChannelId,
                            gDKMessageTypeBTCSRangingServiceMessage_c,
                            (uint8_t)gRangingProcResCfg_c,
                            (uint16_t)1U,
                            (uint8_t*)&bEnableProcResTransfer);

    return result;
}
#endif /* defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U) */

/*! *********************************************************************************
 * \brief        Configures BLE Stack after initialization
 *
 ********************************************************************************** */
void BluetoothLEHost_Initialized(void)
{
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
    if (FALSE == gA2ALocalIrkSet)
    {
        /* do nothing */
    }
    else
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
    {
        /* Common GAP configuration */
        BleConnManager_GapCommonConfig();
    }

    /* Configures Bluetooth Identity Address on the anchor from APP_BD_ADDR (Random Static)
       This is specific to the anchor project to ensure all anchors have the same Identity Address  */
    gSmpKeys.aAddress = mRandomStaticAddr;
    gSmpKeys.addressType = gBleAddrTypeRandom_c;

    /* Register for callbacks*/
    (void)App_RegisterGattServerCallback(BleApp_GattServerCallback);
    (void)App_RegisterGattClientProcedureCallback(BleApp_GattClientCallback);

    /* Register DK L2CAP PSM */
    (void)L2ca_RegisterLePsm(gDK_DefaultVehiclePsm_c, gDKMessageMaxLength_c);

    /* Register stack callbacks */
    (void)App_RegisterLeCbCallbacks(BleApp_L2capPsmDataCallback, BleApp_L2capPsmControlCallback);

    if (TM_Open(mL2caTimerId) == kStatus_TimerSuccess)
    {
        mL2caTimerValid = TRUE;
    }

    /* Inform the user interface handler that Bluetooth application configuration done */
    if(mpfBleUserInterfaceEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_BleConfigDone_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleUserInterfaceEventHandler, pEventData))
            {
                (void)MEM_BufferFree(pEventData);
            }
        }
    }

    (void)Gap_ControllerEnhancedNotification(((uint32_t)gNotifConnCreated_c | (uint32_t)gNotifPhyUpdateInd_c), 0U);
#if (defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U))
    (void)A2B_Init(BleApp_A2BEventHandler, BleApp_A2BCommHandler);
#endif /* (defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)) */
    (void)AppLocalization_HostInitHandler();
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief        Handler of gHostPrivacyStateChanged_c and
*               gControllerPrivacyStateChanged_c from BleApp_GenericCallback.
*
* \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
********************************************************************************** */
static void BleApp_GenericCallback_HandlePrivacyEvents(gapGenericEvent_t* pGenericEvent)
{
    switch (pGenericEvent->eventType)
    {
        case gHostPrivacyStateChanged_c:
        {
            if (!pGenericEvent->eventData.newHostPrivacyState)
            {
                if(gPrivacyStateChangedByUser)
                {
                    gPrivacyStateChangedByUser = FALSE;
                    /* Host privacy disabled because a bond was removed
                       or added. Enable privacy. */
                    (void)BleConnManager_EnablePrivacy();
                }
            }
        }
        break;

        case gControllerPrivacyStateChanged_c:
        {
            if (!pGenericEvent->eventData.newControllerPrivacyState)
            {
                if(gPrivacyStateChangedByUser)
                {
                    gPrivacyStateChangedByUser = FALSE;
                    /* Controller privacy disabled because a bond was removed
                       or added. Enable privacy. */
                    (void)BleConnManager_EnablePrivacy();
                }
            }
        }
        break;

        default:
        {
            ; /* No action required */
        }
        break;
    }
}

/*! *********************************************************************************
* \brief        Handler of gControllerNotificationEvent_c from BleApp_GenericCallback.
*
* \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
********************************************************************************** */
static void BleApp_GenericCallback_ControllerNotificationEvent(gapGenericEvent_t* pGenericEvent)
{
    if ((pGenericEvent->eventData.notifEvent.eventType & ((uint16_t)gNotifConnCreated_c | (uint16_t)gNotifPhyUpdateInd_c)) > 0U )
    {
        uint64_t bleTime = TM_GetTimestamp();
        /* Get current timestamp */
        mTsUwbDeviceTime = GetUwbClock();
        /* Subtract event delay */
        if (bleTime >= pGenericEvent->eventData.notifEvent.timestamp)
        {
            mTsUwbDeviceTime = mTsUwbDeviceTime - ((uint64_t)bleTime - (uint64_t)pGenericEvent->eventData.notifEvent.timestamp);
        }
        else
        {
            mTsUwbDeviceTime = mTsUwbDeviceTime - ((0x00000000FFFFFFFFU - (uint64_t)pGenericEvent->eventData.notifEvent.timestamp) + bleTime);
        }
    }
}

/*! *********************************************************************************
* \brief        Handler of the mAppCCCPhase2WaitingForResponse_c state for
*               BleApp_StateMachineHandler.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
************************************************************************************/
static void BleApp_StateMachineHandler_AppCCCPhase2WaitingForResponse(deviceId_t peerDeviceId, appEvent_t event)
{
    if (event == mAppEvt_ReceivedSPAKEResponse_c)
    {
        /* SPAKE2+ Flow: Send Verify Command */
        /* Dummy data - replace with calls to get actual data */
        uint16_t payloadLen = gDummyPayloadLength_c;
        uint8_t payload[gDummyPayloadLength_c] = gDummyPayload_c;
        bleResult_t status = CCCPhase2_SendSPAKEVerify(peerDeviceId, payload, payloadLen);
        if (status == gBleSuccess_c)
        {
            maPeerInformation[peerDeviceId].appState = mAppCCCPhase2WaitingForVerify_c;
        }
    }
    else if (event == mAppEvt_EncryptionChanged_c)
    {
        maPeerInformation[peerDeviceId].isLinkEncrypted = TRUE;
        maPeerInformation[peerDeviceId].appState = mAppRunning_c;
    }
    else
    {
        /* For MISRA compliance */
    }
}

/*! *********************************************************************************
* \brief        Handler of the mAppCCCWaitingForOwnerPairingRequest_c state for
*               BleApp_StateMachineHandler.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
************************************************************************************/
static void BleApp_StateMachineHandler_AppPair(deviceId_t peerDeviceId, appEvent_t event)
{
    if ( event == mAppEvt_PairingLocalOobData_c )
    {
        (void)CCC_FirstApproachRsp(peerDeviceId, gaAppOwnDiscAddress, &maPeerInformation[peerDeviceId].oobData);
    }
    else if ( event == mAppEvt_PairingPeerOobDataRcv_c )
    {
        mCurrentPeerId = peerDeviceId;
        (void)Gap_LeScGetLocalOobData();
    }
    else if (event == mAppEvt_PairingReqRcv_c)
    {
        /* No action required */
    }
    else if ( event == mAppEvt_PairingPeerOobDataReq_c )
    {
        (void)Gap_LeScSetPeerOobData(peerDeviceId, &maPeerInformation[peerDeviceId].peerOobData);
    }
    else if ( event == mAppEvt_PairingComplete_c )
    {
        FLib_MemSet(&maPeerInformation[peerDeviceId].oobData, 0x00, sizeof(gapLeScOobData_t));
        FLib_MemSet(&maPeerInformation[peerDeviceId].peerOobData, 0x00, sizeof(gapLeScOobData_t));
        maPeerInformation[peerDeviceId].appState = mAppRunning_c;
    }
    else
    {
        /* For MISRA compliance */
    }
}

/*! *********************************************************************************
* \brief        Handler of the mAppIdle_c state for BleApp_StateMachineHandler.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
************************************************************************************/
static void BleApp_StateMachineHandler_AppIdle(deviceId_t peerDeviceId, appEvent_t event)
{
    if (event == mAppEvt_PeerConnected_c)
    {
        maPeerInformation[peerDeviceId].appState = mAppCCCWaitingForOwnerPairingRequest_c;
    }
}

/*! *********************************************************************************
* \brief        Handler of the mAppIdle_c state for BleApp_StateMachineHandler.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
************************************************************************************/
static void BleApp_StateMachineHandler_AppCCCWaitingForOwnerPairingRequest
(
    deviceId_t peerDeviceId,
    appEvent_t event
)
{
    if (event == mAppEvt_OwnerPairingRequestReceived_c)
    {
        bleResult_t status;

        /* Move to CCC Phase 2 */
        /* Dummy data - replace with calls to get actual data */
        uint16_t payloadLen = gDummyPayloadLength_c;
        uint8_t payload[gDummyPayloadLength_c] = gDummyPayload_c;
        status = CCCPhase2_SendSPAKERequest(peerDeviceId, payload, payloadLen);

        if (status == gBleSuccess_c)
        {
            maPeerInformation[peerDeviceId].appState = mAppCCCPhase2WaitingForResponse_c;
        }
    }
    else if (event == mAppEvt_EncryptionChanged_c)
    {
        maPeerInformation[peerDeviceId].isLinkEncrypted = TRUE;
        maPeerInformation[peerDeviceId].appState = mAppLocalizationSetup_c;
    }
    else
    {
        /* For MISRA compliance */
    }
}

/*! *********************************************************************************
* \brief        Handler of the mAppLocalizationSetup_c state for BleApp_StateMachineHandler.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
************************************************************************************/
static void BleApp_StateMachineHandler_LocalizationSetup
(
    deviceId_t peerDeviceId,
    appEvent_t event
)
{
    bleResult_t result = gBleSuccess_c;
    bool_t bProceedToLocalization = FALSE;

#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
    if (event == mAppEvt_ExchangeMtuComplete_c)
    {
        /* Send BTCS Ranging Procedure Results Config message */
        bool_t bEnableProcResTransfer = TRUE;

        /* Receiver should send procedure results */
        result = SendBTCSRangingProcResCfg(peerDeviceId, bEnableProcResTransfer);

        if (result != gBleSuccess_c)
        {
            shell_write("BTCS Ranging Procedure Results Config failed!\r\n");
        }
        else
        {
            bProceedToLocalization = TRUE;
        }
    }
#elif defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
    if (event == mAppEvt_PsmChannelCreated_c)
    {
        AppLocalization_SetPsmChannelId(peerDeviceId,
                                        maPeerInformation[peerDeviceId].customInfo.psmChannelId);
    }
    else if (event == mAppEvt_BtcsRangingProcResCfg_c)
    {
        bProceedToLocalization = TRUE;
    }
    else
    {
        /* For MISRA compliance */
    }
#endif

    if (bProceedToLocalization)
    {
        maPeerInformation[peerDeviceId].appState = mAppLocalization_c;

        if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
        {
            result = AppLocalization_Config(peerDeviceId);

            if (result != gBleSuccess_c)
            {
                shell_write("Localization configuration failed !\r\n");
            }
        }
    }
}

/*! *********************************************************************************
* \brief        Handles BLE Advertising callback from host stack.
*
* \param[in]    pAdvertisingEvent    Pointer to gapAdvertisingEvent_t.
********************************************************************************** */
static void BleApp_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent)
{
    switch (pAdvertisingEvent->eventType)
    {
        case gExtAdvertisingStateChanged_c:
        {
            appEvent_t appEvent = mAppEvt_AdvertisingStopped_c;
            if (gCurrentAdvHandle == gLegacyAdvSetHandle_c)
            {
                /* Inform the user interface handler that legacy advertising started */
                appEvent = mAppEvt_AdvertisingStartedLegacy_c;
#if defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1)
                if (FALSE == mOwnerPairingMode)
                {
                    gCurrentAdvHandle = gExtendedAdvSetHandle_c;
                    gStopExtAdvSetAfterConnect = FALSE;
                    BleApp_Advertise();
                }
#endif /* defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1) */

                /* UI */
                LedStopFlashingAllLeds();
                Led1Flashing();
            }
#if defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1)
            else if (gCurrentAdvHandle == gExtendedAdvSetHandle_c)
            {
                /* Inform the user interface handler that extended advertising started */
                appEvent = mAppEvt_AdvertisingStartedExtendedLR_c;
            }
            else
            {
                if (gStopExtAdvSetAfterConnect == TRUE)
                {
                    /* Connection established based on the legacy advertising set
                    before the extended advertising set was started. Advertising
                    should be stopped */
                    gStopExtAdvSetAfterConnect = FALSE;
                    (void)Gap_StopExtAdvertising(0xFF);
                }
                else
                {
                    /* Inform the user interface handler that advertising has stopped */
                    appEvent = mAppEvt_AdvertisingStopped_c;

                    if (0U == BleApp_GetNoOfActiveConnections())
                    {
                        /* UI */
                        LedStopFlashingAllLeds();
                        Led1Flashing();
#if (defined(gAppLedCnt_c) && (gAppLedCnt_c > 1U))
                        Led2Flashing();
#endif
                    }
                }
            }
#else /* defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1) */
            else
            {
                /* Inform the user interface handler that advertising has stopped */
                appEvent = mAppEvt_AdvertisingStopped_c;

                if (0U == BleApp_GetNoOfActiveConnections())
                {
                    /* UI */
                    LedStopFlashingAllLeds();
                    Led1Flashing();
#if (defined(gAppLedCnt_c) && (gAppLedCnt_c > 1U))
                    Led2Flashing();
#endif
                }
            }
#endif /* defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1) */

            if(mpfBleUserInterfaceEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = appEvent;
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleUserInterfaceEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
        break;

        case gAdvertisingCommandFailed_c:
        {
            /* Panic UI */
#if (defined(gAppLedCnt_c) && (gAppLedCnt_c > 1U))
            Led2On();
#endif
            panic(0,0,0,0);
        }
        break;

        case gAdvertisingSetTerminated_c:
        {

        }
        break;

        default:
        {
            ; /* No action required */
        }
        break;
    }
}



/*! *********************************************************************************
* \brief        Configures GAP Advertise parameters. Advertise will start after
*               the parameters are set.
*
********************************************************************************** */
static void BleApp_Advertise(void)
{
    /* Set advertising parameters*/
    if (gCurrentAdvHandle == gLegacyAdvSetHandle_c)
    {
        gAppAdvParams.pGapExtAdvParams = &gLegacyAdvParams;
        gAppAdvParams.handle = gLegacyAdvSetHandle_c;
    }
#if defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1)
    else if (gCurrentAdvHandle == gExtendedAdvSetHandle_c)
    {
        gAppAdvParams.pGapExtAdvParams = &gExtAdvParams;
        gAppAdvParams.handle = gExtendedAdvSetHandle_c;
    }
#endif /* defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1) */
    else
    {
        /* For MISRA compliance */
    }

    (void)BluetoothLEHost_StartExtAdvertising(&gAppAdvParams, BleApp_AdvertisingCallback, BleApp_ConnectionCallback);
}
/*! *********************************************************************************
* \brief        Handles GATT server callback from host stack.
*
* \param[in]    deviceId        Peer device ID.
* \param[in]    pServerEvent    Pointer to gattServerEvent_t.
********************************************************************************** */
static void BleApp_GattServerCallback (deviceId_t deviceId, gattServerEvent_t* pServerEvent)
{
    switch (pServerEvent->eventType)
    {
        case gEvtCharacteristicCccdWritten_c:
        {

        }
        break;

        case gEvtMtuChanged_c:
        {
            appEvent_t appEvt = mAppEvt_GattProcComplete_c;
            appEvt = mAppEvt_ExchangeMtuComplete_c;
            BleApp_StateMachineHandler(deviceId, appEvt);
        }
        break;


        default:
        {
            ; /* No action required */
        }
        break;
    }
}

/*! *********************************************************************************
* \brief        Callback for incoming PSM data.
*
* \param[in]    deviceId        The device ID of the connected peer that sent the data
* \param[in]    lePsm           Channel ID
* \param[in]    pPacket         Pointer to incoming data
* \param[in]    packetLength    Length of incoming data
********************************************************************************** */
static void BleApp_L2capPsmDataCallback (deviceId_t     deviceId,
                                         uint16_t       lePsm,
                                         uint8_t*       pPacket,
                                         uint16_t       packetLength)
{
    /* The vehicle shall trigger a disconnect 5 seconds after an unencrypted L2CAP connection establishment
     * if no First_Approach_RQ or Request_owner_pairing Command Complete SubEvent notification has been
     * received during the first approach and owner pairing, respectively.*/
    if(TM_IsTimerActive(mL2caTimerId) == 1U)
    {
        if (packetLength > (gMessageHeaderSize_c + gPayloadHeaderSize_c + gLengthFieldSize_c))
        {
            dkMessageType_t messageType = (dkMessageType_t)pPacket[0];
            uint8_t msgId = pPacket[1];

            switch (messageType)
            {
                case gDKMessageTypeDKEventNotification_c:
                {
                    if ( msgId == (uint8_t)gDkEventNotification_c )
                    {
                        if ( packetLength == (gMessageHeaderSize_c + gPayloadHeaderSize_c + gLengthFieldSize_c + gCommandCompleteSubEventPayloadLength_c) )
                        {
                            dkSubEventCategory_t category = (dkSubEventCategory_t)pPacket[gMessageHeaderSize_c + gPayloadHeaderSize_c + gLengthFieldSize_c];
                            if ( category == gCommandComplete_c)
                            {
                                dkSubEventCommandCompleteType_t type = (dkSubEventCommandCompleteType_t)pPacket[gMessageHeaderSize_c + gPayloadHeaderSize_c + gLengthFieldSize_c + sizeof(category)];
                                if ( type == gRequestOwnerPairing_c )
                                {
                                    (void)TM_Stop((timer_handle_t)mL2caTimerId);
                                }
                            }
                        }
                    }
                }
                break;

                case gDKMessageTypeSupplementaryServiceMessage_c:
                {
                    if ( msgId == (uint8_t)gFirstApproachRQ_c )
                    {
                        if ( packetLength == (gMessageHeaderSize_c + gPayloadHeaderSize_c + gLengthFieldSize_c + gFirstApproachReqRspPayloadLength) )
                        {
                            (void)TM_Stop((timer_handle_t)mL2caTimerId);
                        }
                    }
                }
                break;

                default:
                    ; /* For MISRA compliance */
                    break;
            }
        }
    }

    /* Inform the application events handler that L2capPsmData package received */
    if(mpfBleEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(appEventL2capPsmData_t) + (uint32_t)packetLength);
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_L2capPsmDataCallback_c;
            pEventData->eventData.pData = &pEventData[1];
            appEventL2capPsmData_t *pL2capPsmDataEvent = pEventData->eventData.pData;
            pL2capPsmDataEvent->deviceId = deviceId;
            pL2capPsmDataEvent->lePsm = lePsm;
            pL2capPsmDataEvent->packetLength = packetLength;
            pL2capPsmDataEvent->pPacket = (uint8_t*)(&pL2capPsmDataEvent[1]);
            FLib_MemCpy(pL2capPsmDataEvent->pPacket, pPacket, packetLength);
            pL2capPsmDataEvent = NULL;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
            {
                (void)MEM_BufferFree(pEventData);
            }
        }
    }
}

/*! *********************************************************************************
* \brief        Callback for control messages.
*
* \param[in]    pMessage    Pointer to control message
********************************************************************************** */
static void BleApp_L2capPsmControlCallback(l2capControlMessage_t* pMessage)
{
    switch (pMessage->messageType)
    {
        case gL2ca_HandoverConnectionComplete_c:
        {
            /* Save L2CAP channel ID */
            maPeerInformation[pMessage->messageData.handoverConnectionComplete.deviceId].customInfo.psmChannelId =
                pMessage->messageData.handoverConnectionComplete.cId;
            /* Set the L2CAP channel into the localization module if BTCS server */
#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
            AppLocalization_SetPsmChannelId(pMessage->messageData.handoverConnectionComplete.deviceId,
                                            pMessage->messageData.handoverConnectionComplete.cId);
#endif
        }
        break;

        case gL2ca_LePsmConnectRequest_c:
        {
            deviceId_t deviceId = pMessage->messageData.connectionRequest.deviceId;

            /* For Passive Entry the link must be encrypted before opening the L2CAP CB channel */
            if ((maPeerInformation[deviceId].isBonded == TRUE) && (maPeerInformation[deviceId].isLinkEncrypted == FALSE))
            {
                (void)L2ca_CancelConnection(gDK_DefaultVehiclePsm_c, deviceId, gInsufficientEncryption_c);
            }
            else
            {
                (void)L2ca_ConnectLePsm(gDK_DefaultVehiclePsm_c, deviceId, mAppLeCbInitialCredits_c);
            }
        }
        break;

        case gL2ca_LePsmConnectionComplete_c:
        {
            if (pMessage->messageData.connectionComplete.result == gSuccessful_c)
            {
                /* Handle Conn Complete */
                maPeerInformation[pMessage->messageData.connectionComplete.deviceId].customInfo.psmChannelId = pMessage->messageData.connectionComplete.cId;
                if (maPeerInformation[pMessage->messageData.connectionComplete.deviceId].isLinkEncrypted == FALSE)
                {
                    if (mL2caTimerValid == TRUE)
                    {
                        mDeviceIdToDisconnect = pMessage->messageData.connectionComplete.deviceId;
                        (void)TM_InstallCallback((timer_handle_t)mL2caTimerId, L2caTimerCallback, &mDeviceIdToDisconnect);
                        (void)TM_Start((timer_handle_t)mL2caTimerId, (uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSetSecondTimer, gCCCL2caTimeout_c);
                    }
                }
                BleApp_StateMachineHandler(maPeerInformation[pMessage->messageData.connectionComplete.deviceId].deviceId, mAppEvt_PsmChannelCreated_c);
            }
        }
        break;

        case gL2ca_LePsmDisconnectNotification_c:
        {

        }
        break;

        case gL2ca_NoPeerCredits_c:
        {
            (void)L2ca_SendLeCredit (pMessage->messageData.noPeerCredits.deviceId,
                                    pMessage->messageData.noPeerCredits.cId,
                                    mAppLeCbInitialCredits_c);
        }
        break;

        case gL2ca_Error_c:
        {
            /* Handle error */
        }
        break;

#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
        case gL2ca_ChannelStatusNotification_c:
        {
            if (mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(l2caLeCbChannelStatusNotification_t));
                if (pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_L2capPsmChannelStatusNotification_c;
                    pEventData->eventData.pData = pEventData + 1;
                    FLib_MemCpy(pEventData->eventData.pData,
                                &pMessage->messageData.channelStatusNotification,
                                sizeof(l2caLeCbChannelStatusNotification_t));

                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
            break;
        }
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */

        default:
        {
            ; /* For MISRA compliance */
            break;
        }
    }
}

/*! *********************************************************************************
* \brief        Handles GATT client callback from host stack.
*
* \param[in]    serverDeviceId      GATT Server device ID.
* \param[in]    procedureType       Procedure type.
* \param[in]    procedureResult     Procedure result.
* \param[in]    error               Callback result.
********************************************************************************** */
static void BleApp_GattClientCallback
(
    deviceId_t              serverDeviceId,
    gattProcedureType_t     procedureType,
    gattProcedureResult_t   procedureResult,
    bleResult_t             error
)
{
    if (procedureResult == gGattProcError_c)
    {
        attErrorCode_t attError = (attErrorCode_t)(uint8_t)(error);

        if (attError == gAttErrCodeInsufficientEncryption_c     ||
            attError == gAttErrCodeInsufficientAuthorization_c  ||
            attError == gAttErrCodeInsufficientAuthentication_c)
        {
            /* Start Pairing Procedure */
            (void)Gap_Pair(serverDeviceId, &gPairingParameters);
        }

        BleApp_StateMachineHandler(serverDeviceId, mAppEvt_GattProcError_c);
    }
    else
    {
        if (procedureResult == gGattProcSuccess_c)
        {
            appEvent_t appEvt = mAppEvt_GattProcComplete_c;

            switch(procedureType)
            {

                case gGattProcReadCharacteristicValue_c:
                case gGattProcReadUsingCharacteristicUuid_c:
                {
                    appEvt = mAppEvt_ReadCharacteristicValueComplete_c;
                }
                break;

                case gGattProcWriteCharacteristicDescriptor_c:
                {
                    appEvt = mAppEvt_WriteCharacteristicDescriptorComplete_c;
                }
                break;

                case gGattProcWriteCharacteristicValue_c:
                {
                    appEvt = mAppEvt_WriteCharacteristicValueComplete_c;
                }
                break;

                default:
                {
                    ; /* No action required */
                }
                break;
            }

            BleApp_StateMachineHandler(serverDeviceId, appEvt);
        }
    }

    /* Signal Service Discovery Module */
    BleServDisc_SignalGattClientEvent(serverDeviceId, procedureType, procedureResult, error);
}

/*! *********************************************************************************
 * \brief        Returns number of active connections.
 ********************************************************************************** */
static uint8_t BleApp_GetNoOfActiveConnections(void)
{
    uint8_t activeConnections = 0;
    uint8_t mPeerId = 0;

    for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
    {
        if (maPeerInformation[mPeerId].deviceId != gInvalidDeviceId_c)
        {
            ++activeConnections;
        }
    }

    return activeConnections;
}

/*! *********************************************************************************
 * \brief        Timer Callback
 ********************************************************************************** */
static void L2caTimerCallback(void *param)
{
    deviceId_t deviceId = *(deviceId_t*)param;

    if (maPeerInformation[deviceId].deviceId != gInvalidDeviceId_c)
    {
        (void)Gap_Disconnect(maPeerInformation[deviceId].deviceId);
    }
}

/*! *********************************************************************************
 * \brief        Returns UWB clock.
 ********************************************************************************** */
static uint64_t GetUwbClock(void)
{
    uint32_t randomNo = 0U;
    (void)RNG_GetTrueRandomNumber(&randomNo);
    /* Get a simulated UWB clock */
    return TM_GetTimestamp() + (uint8_t)randomNo;
}

#if defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1) && \
    defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
/*! *********************************************************************************
* \brief        Send application specific data to the peer.
*
********************************************************************************** */
static void A2A_SendApplicationData(deviceId_t deviceId)
{
    /* Data to transfer: mRangeSettings, maAppLclState, mpCachedRemoteCaps,
       role, csSecurityEnabled */
    uint32_t dataSize = sizeof(appLocalization_rangeCfg_t) +
                        sizeof(uint8_t) +
                        sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t) +
                        sizeof(uint8_t) * 2U;

    uint8_t* pAppData = MEM_BufferAlloc(dataSize);

    if (pAppData != NULL)
    {
        union {
            uint8_t                 u8LocState;
            appLocalization_State_t locState;
        } temp = {0U}; /* MISRA rule 11.3 */
        uint8_t* pAuxPtr = pAppData;
        uint8_t nvmIndex = gInvalidNvmIndex_c;
        bool_t isBonded = FALSE;
        csReadRemoteSupportedCapabilitiesCompleteEvent_t *remoteCapabilities = NULL;

        /* Default settings */
        FLib_MemCpy(pAuxPtr,
                    &mRangeSettings[deviceId],
                    sizeof(appLocalization_rangeCfg_t));
        pAuxPtr += sizeof(appLocalization_rangeCfg_t);

        /* Application state */
        temp.locState = AppLocalization_GetLocState(deviceId);
        *pAuxPtr = temp.u8LocState;
        pAuxPtr = &pAuxPtr[1];

        (void)Gap_CheckIfBonded(deviceId, &isBonded, &nvmIndex);
        if (nvmIndex != gInvalidNvmIndex_c)
        {
            remoteCapabilities = AppLocalization_GetRemoteCachedSupportedCapabilities(nvmIndex);
        }

        if (remoteCapabilities != NULL)
        {
            FLib_MemCpy(pAuxPtr,
                        remoteCapabilities,
                        sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t));
        }
        else
        {
            FLib_MemSet(pAuxPtr, 0U, sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t));
        }

        pAuxPtr += sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t);

        /* Copy local Role information */
        Utils_Copy8(pAuxPtr, mGlobalRangeSettings.role);

        /* Copy local Security Enabled status information */
        Utils_Copy8(pAuxPtr, maPeerInformation[deviceId].csSecurityEnabled);

        /* Send data to peer */
        (void)A2A_SendApplicationDataCommand(pAppData, (uint16_t)dataSize);
        (void)MEM_BufferFree(pAppData);
    }
}
#endif

#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
/*! *********************************************************************************
* \brief        Handler function for APP A2B communication interface.
*
********************************************************************************** */
static void BleApp_A2BCommHandler(uint8_t opGroup, uint8_t cmdId, uint16_t len, uint8_t *pData)
{
    A2A_SendCommand(opGroup, cmdId, pData, len);
}
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
