/*! *********************************************************************************
* \addtogroup Digital Key Device CS
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file digital_key_device_cs.c
*
* Copyright 2022 - 2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
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
#include "fsl_format.h"
#include "fsl_debug_console.h"
#include "app.h"
#include "board.h"
#include "fwk_platform_ble.h"
#include "NVM_Interface.h"

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
#include "fsl_shell.h"
#endif

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
#include "app_scanner.h"
#include "digital_key_device_cs.h"

#include "app_digital_key_device_cs.h"
#include "shell_digital_key_device_cs.h"

#include "app_localization.h"
#include "channel_sounding.h"

/************************************************************************************
*************************************************************************************
* Extern functions
*************************************************************************************
************************************************************************************/
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

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static appScanningParams_t appScanParams = {
    &gScanParams,
    gGapDuplicateFilteringEnable_c,
    gGapScanContinuously_d,
    gGapScanPeriodicDisabled_d
};

static bool_t   mScanningOn = FALSE;
static bool_t   mFoundDeviceToConnect = FALSE;

/* Application callback */
pfBleCallback_t mpfBleEventHandler = NULL;

/* This global will be TRUE if the user adds or removes a bond */
bool_t gPrivacyStateChangedByUser = FALSE;

/* Peer MTU values */
static uint16_t mPeerMtu[gAppMaxConnections_c] = {0U};

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1)
static SERIAL_MANAGER_WRITE_HANDLE_DEFINE(gDataExportSerialWriteHandle);
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1) */

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
/* Host Stack callbacks */
static void BleApp_ScanningCallback
(
    gapScanningEvent_t* pScanningEvent
);

static void BleApp_ConnectionCallback
(
    deviceId_t peerDeviceId,
    gapConnectionEvent_t* pConnectionEvent
);

static void BleApp_ConnectionCallback_SignalSimpleEvents(deviceId_t peerDeviceId, appEvent_t connectionCallbackEvent);

static void BleApp_GattClientCallback
(
    deviceId_t              serverDeviceId,
    gattProcedureType_t     procedureType,
    gattProcedureResult_t   procedureResult,
    bleResult_t             error
);

static void BleApp_ServiceDiscoveryCallback
(
    deviceId_t peerDeviceId,
    servDiscEvent_t* pEvent
);

static void BleApp_CsEventHandler(deviceId_t deviceId, void *pData, appCsEventType_t eventType);

/* PSM callbacks */
static void BleApp_L2capPsmDataCallback (deviceId_t deviceId, uint16_t lePsm, uint8_t* pPacket, uint16_t packetLength);
static void BleApp_L2capPsmControlCallback (l2capControlMessage_t* pMessage);

static bool_t CheckScanEventLegacy(gapScannedDevice_t* pData);
static bool_t CheckScanEventExtended(gapExtScannedDevice_t* pData);

static void BleApp_StoreServiceHandles
(
    deviceId_t      peerDeviceId,
    gattService_t   *pService
);

static void BleApp_GenericCallback_HandlePrivacyEvents(gapGenericEvent_t* pGenericEvent);
static void BluetoothLEHost_Initialized(void);

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
static button_status_t BleApp_HandleKeys0(void *buttonHandle, button_callback_message_t *message,void *callbackParam);
#endif /*gAppButtonCnt_c > 0*/

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1)
static void App_ExportHciDataLog(void *pData);
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1) */

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  This is the initialization function for each application. This function
*         should contain all the initialization code required by the bluetooth demo
********************************************************************************** */
void BluetoothLEHost_AppInit(void)
{
    union Prompt_tag
    {
        const char * constPrompt;
        char * prompt;
    } shellPrompt;

    uint8_t mPeerId = 0;

    for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
    {
        maPeerInformation[mPeerId].deviceId = gInvalidDeviceId_c;
        FLib_MemSet(&maPeerInformation[mPeerId].oobData, 0x00, sizeof(gapLeScOobData_t));
        FLib_MemSet(&maPeerInformation[mPeerId].peerOobData, 0x00, sizeof(gapLeScOobData_t));
        mPeerMtu[mPeerId] = gAttDefaultMtu_c;
    }

    LedStartFlashingAllLeds();

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[0], BleApp_HandleKeys0, NULL);
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0)) */

    BleApp_RegisterEventHandler(APP_BleEventHandler);

    /* Set generic callback */
    BluetoothLEHost_SetGenericCallback(BleApp_GenericCallback);

    /* Initialize Bluetooth Host Stack */
    BluetoothLEHost_Init(BluetoothLEHost_Initialized);

    (void)Gap_ControllerEnhancedNotification(((uint32_t)gNotifConnCreated_c | (uint32_t)gNotifPhyUpdateInd_c), 0U);

    /* UI */
    shellPrompt.constPrompt = "Device>";
    AppShellInit(shellPrompt.prompt);

    /* Register CS callback and initialize localization */
    (void)AppLocalization_Init(gCsDefaultRole_c, BleApp_CsEventHandler, BleApp_PrintMeasurementResults);

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1)
    /* Open write handle */
    (void)SerialManager_OpenWriteHandle(gSerMgrIf2, (serial_write_handle_t)gDataExportSerialWriteHandle);
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1) */
}

/*! *********************************************************************************
 * \brief        Register function to handle events from BLE to APP
 *
 * \param[in]    pCallback       event handler
 ********************************************************************************** */
void BleApp_RegisterEventHandler(pfBleCallback_t pfBleEventHandler)
{
    mpfBleEventHandler = pfBleEventHandler;
}

/*! *********************************************************************************
* \brief    Starts the BLE application.
*
********************************************************************************** */
void BleApp_Start(void)
{
    if (!mScanningOn)
    {
        /* Start scanning */
        (void)BluetoothLEHost_StartScanning(&appScanParams, BleApp_ScanningCallback);
    }
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
    /* Call BLE Conn Manager */
    BleConnManager_GenericEvent(pGenericEvent);

    switch (pGenericEvent->eventType)
    {
        case gLePhyEvent_c:
            {
                if(mpfBleEventHandler != NULL)
                {
                    appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(gapPhyEvent_t));
                    if(pEventData != NULL)
                    {
                        pEventData->appEvent = mAppEvt_GenericCallback_LePhyEvent_c;
                        pEventData->eventData.pData = pEventData + 1;
                        FLib_MemCpy(pEventData->eventData.pData, &pGenericEvent->eventData.phyEvent, sizeof(gapPhyEvent_t));
                        if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                        {
                            (void)MEM_BufferFree(pEventData);
                        }
                    }
                }
            }
            break;

        case gLeScLocalOobData_c:
            {
                if(mpfBleEventHandler != NULL)
                {
                    appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(gapLeScOobData_t));
                    if(pEventData != NULL)
                    {
                        pEventData->appEvent = mAppEvt_GenericCallback_LeScLocalOobData_c;
                        pEventData->eventData.pData = pEventData + 1;
                        FLib_MemCpy(pEventData->eventData.pData, &pGenericEvent->eventData.localOobData, sizeof(gapLeScOobData_t));
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
                if(mpfBleEventHandler != NULL)
                {
                    appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + gcBleDeviceAddressSize_c);
                    if(pEventData != NULL)
                    {
                        pEventData->appEvent = mAppEvt_GenericCallback_RandomAddressReady_c;
                        pEventData->eventData.pData = pEventData + 1;
                        FLib_MemCpy(pEventData->eventData.pData, pGenericEvent->eventData.addrReady.aAddress, gcBleDeviceAddressSize_c);
                        if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                        {
                            (void)MEM_BufferFree(pEventData);
                        }
                    }
                }
            }
            break;

        case gControllerNotificationEvent_c:
        {
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(bleNotificationEvent_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_GenericCallback_CtrlNotifEvent_c;
                    pEventData->eventData.pData = pEventData +1U;
                    FLib_MemCpy(pEventData->eventData.pData, &pGenericEvent->eventData.notifEvent, sizeof(bleNotificationEvent_t));
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
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(bleBondCreatedEvent_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_GenericCallback_BondCreatedEvent_c;
                    pEventData->eventData.pData = pEventData + 1;
                    FLib_MemCpy(pEventData->eventData.pData, &pGenericEvent->eventData.bondCreatedEvent, sizeof(bleBondCreatedEvent_t));
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
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

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/


/*! *********************************************************************************
* \brief        Handles keyboard events.
*
* \param[in]    events    Key event structure.
********************************************************************************** */
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
static button_status_t  BleApp_HandleKeys0(void *buttonHandle, button_callback_message_t *message,void *callbackParam)
{
    switch (message->event)
    {
        case kBUTTON_EventOneClick:
        case kBUTTON_EventShortPress:
        {
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_KBD_EventPressPB1_c;
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
        break;

        /* Disconnect on long button press */
        case kBUTTON_EventLongPress:
        {
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_KBD_EventLongPB1_c;
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
        break;

        /* Very Long Press not available - use Double Click */
        case kBUTTON_EventDoubleClick:
        {
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_KBD_EventVeryLongPB1_c;
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
        break;

        default:
        {
            ; /* No action required */
            break;
        }
    }

    return kStatus_BUTTON_Success;
}
#endif /*gAppButtonCnt_c > 0*/

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
* \brief        Handles BLE Scanning callback from host stack.
*
* \param[in]    pScanningEvent    Pointer to gapScanningEvent_t.
********************************************************************************** */
static void BleApp_ScanningCallback (gapScanningEvent_t* pScanningEvent)
{
    switch (pScanningEvent->eventType)
    {
        case gDeviceScanned_c:
        {
            /* Check if the scanned device implements the DK Service */
            if( FALSE == mFoundDeviceToConnect )
            {
                /* Advertising for Passive Entry has an empty payload - if we have a bond with the device, connect */
                /* Only check the payload if we do not have a bond */
                if (pScanningEvent->eventData.scannedDevice.advertisingAddressResolved == FALSE)
                {
                    mFoundDeviceToConnect = CheckScanEventLegacy(&pScanningEvent->eventData.scannedDevice);
                }

                if (mFoundDeviceToConnect || (pScanningEvent->eventData.scannedDevice.advertisingAddressResolved == TRUE))
                {
                    mFoundDeviceToConnect = TRUE;
                    /* Set connection parameters and stop scanning. Connect on gScanStateChanged_c. */
                    gConnReqParams.peerAddressType = pScanningEvent->eventData.scannedDevice.addressType;
                    FLib_MemCpy(gConnReqParams.peerAddress,
                                pScanningEvent->eventData.scannedDevice.aAddress,
                                sizeof(bleDeviceAddress_t));

                    (void)Gap_StopScanning();
#if defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d)
                    gConnReqParams.usePeerIdentityAddress = pScanningEvent->eventData.scannedDevice.advertisingAddressResolved;
#endif
                }
            }
        }
        break;

        case gExtDeviceScanned_c:
        {
            /* Check if the scanned device implements the DK Service */
            if( FALSE == mFoundDeviceToConnect )
            {
                /* Advertising for Passive Entry has an empty payload - if we have a bond with the device, connect */
                /* Only check the payload if we do not have a bond */
                if (pScanningEvent->eventData.extScannedDevice.advertisingAddressResolved == FALSE)
                {
                    mFoundDeviceToConnect = CheckScanEventExtended(&pScanningEvent->eventData.extScannedDevice);
                }

                if (mFoundDeviceToConnect || (pScanningEvent->eventData.extScannedDevice.advertisingAddressResolved == TRUE))
                {
                    mFoundDeviceToConnect = TRUE;
                    /* Set connection parameters and stop scanning. Connect on gScanStateChanged_c. */
                    gConnReqParams.peerAddressType = pScanningEvent->eventData.extScannedDevice.addressType;
                    FLib_MemCpy(gConnReqParams.peerAddress,
                                pScanningEvent->eventData.extScannedDevice.aAddress,
                                sizeof(bleDeviceAddress_t));

                    (void)Gap_StopScanning();
#if defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d)
                    gConnReqParams.usePeerIdentityAddress = pScanningEvent->eventData.extScannedDevice.advertisingAddressResolved;
#endif
                }
            }
        }
        break;

        case gScanStateChanged_c:
        {
            mScanningOn = !mScanningOn;

            /* Node starts scanning */
            if (mScanningOn)
            {
                mFoundDeviceToConnect = FALSE;

                shell_write("Scanning...\r\n");

                LedStopFlashingAllLeds();
                Led1Flashing();
            }
            /* Node is not scanning */
            else
            {
                shell_write("Scan stopped.\r\n");

                /* Connect with the previously scanned peer device */
                if (mFoundDeviceToConnect)
                {
                    shell_write("Connecting...\r\n");
                    (void)BluetoothLEHost_Connect(&gConnReqParams, BleApp_ConnectionCallback);
                }
                else
                {
                    LedStopFlashingAllLeds();
                    Led1Flashing();
#if (defined(gAppLedCnt_c) && (gAppLedCnt_c > 1U))
                    Led2Flashing();
#endif
                    shell_cmd_finished();
                }
            }
        }
        break;

        case gScanCommandFailed_c:
        {
            ; /* No action required */
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
static void BleApp_ConnectionCallback (deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent)
{
    /* Connection Manager to handle Host Stack interactions */
    BleConnManager_GapCentralEvent(peerDeviceId, pConnectionEvent);

    switch (pConnectionEvent->eventType)
    {
        case gConnEvtConnected_c:
        {
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(appConnectionCallbackEventData_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_ConnectionCallback_ConnEvtConnected_c;
                    pEventData->eventData.pData = pEventData + 1;
                    appConnectionCallbackEventData_t *pConnectionCallbackEventData = pEventData->eventData.pData;
                    pConnectionCallbackEventData->peerDeviceId = peerDeviceId;
                    FLib_MemCpy(&pConnectionCallbackEventData->eventData.pConnectedEvent, &pConnectionEvent->eventData.connectedEvent, sizeof(gapConnectedEvent_t));
                    pConnectionCallbackEventData = NULL;
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
        break;

        case gConnEvtDisconnected_c:
        {
            maPeerInformation[peerDeviceId].disconReason = pConnectionEvent->eventData.disconnectedEvent.reason;
            BleApp_ConnectionCallback_SignalSimpleEvents(peerDeviceId, mAppEvt_ConnectionCallback_ConnEvtDisconnected_c);
        }
        break;

#if gAppUsePairing_d
        case gConnEvtLeScOobDataRequest_c:
        {
            BleApp_ConnectionCallback_SignalSimpleEvents(peerDeviceId, mAppEvt_ConnectionCallback_ConnEvtLeScOobDataRequest_c);
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
                BleApp_ConnectionCallback_SignalSimpleEvents(peerDeviceId, mAppEvt_ConnectionCallback_ConnEvtPairingComplete_c);
            }
        }
        break;

#if defined(gAppUseBonding_d) && (gAppUseBonding_d)
        case gConnEvtEncryptionChanged_c:
        {
            if( pConnectionEvent->eventData.encryptionChangedEvent.newEncryptionState )
            {
                BleApp_ConnectionCallback_SignalSimpleEvents(peerDeviceId, mAppEvt_ConnectionCallback_ConnEvtEncryptionChanged_c);
            }
        }
        break;

        case gConnEvtAuthenticationRejected_c:
        {
            BleApp_ConnectionCallback_SignalSimpleEvents(peerDeviceId, mAppEvt_ConnectionCallback_ConnEvtAuthenticationRejected_c);
        }
        break;
#endif /* gAppUseBonding_d */
#endif /* gAppUsePairing_d */

        case gConnEvtParameterUpdateComplete_c:
        {
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(appConnectionCallbackEventData_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_ConnectionCallback_ConnEvtParameterUpdateComplete_c;
                    pEventData->eventData.pData = pEventData + 1;
                    appConnectionCallbackEventData_t *pConnectionCallbackEventData = pEventData->eventData.pData;
                    pConnectionCallbackEventData->peerDeviceId = peerDeviceId;
                    FLib_MemCpy(&pConnectionCallbackEventData->eventData.pConnParamUpdateCompleteEvent, &pConnectionEvent->eventData.connectionUpdateComplete, sizeof(gapConnParamsUpdateComplete_t));
                    pConnectionCallbackEventData = NULL;
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
        break;

        default:
            ; /* No action required */
            break;
    }
}


/*! *********************************************************************************
* \brief        Handles signaling of simple events (peerDeviceId and appEvent_t
*               connection event) from BLE Connection callback from host stack to the
*               application.
*
* \param[in]    peerDeviceId                     Peer device ID.
* \param[in]    connectionCallbackEvent          Event type.
********************************************************************************** */
static void BleApp_ConnectionCallback_SignalSimpleEvents
(
    deviceId_t peerDeviceId,
    appEvent_t connectionCallbackEvent
)
{
    if(mpfBleEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = connectionCallbackEvent;
            pEventData->peerDeviceId = peerDeviceId;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
            {
                (void)MEM_BufferFree(pEventData);
            }
        }
    }
}

/*! *********************************************************************************
* \brief        Handles discovered services.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    pEvent              Pointer to servDiscEvent_t.
********************************************************************************** */
static void BleApp_ServiceDiscoveryCallback(deviceId_t peerDeviceId, servDiscEvent_t* pEvent)
{
    switch(pEvent->eventType)
    {
        /* Store the discovered handles for later use. */
        case gServiceDiscovered_c:
        {
            BleApp_StoreServiceHandles(peerDeviceId, pEvent->eventData.pService);
        }
        break;

        /* Service discovery has finished, run the state machine. */
        case gDiscoveryFinished_c:
        {
            if (pEvent->eventData.success)
            {
                if(mpfBleEventHandler != NULL)
                {
                    appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                    if(pEventData != NULL)
                    {
                        pEventData->appEvent = mAppEvt_ServiceDiscoveryCallback_DiscoveryFinishedWithSuccess_c;
                        pEventData->peerDeviceId = peerDeviceId;
                        if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                        {
                            (void)MEM_BufferFree(pEventData);
                        }
                    }
                }
            }
            else
            {
                if(mpfBleEventHandler != NULL)
                {
                    appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                    if(pEventData != NULL)
                    {
                        pEventData->appEvent = mAppEvt_ServiceDiscoveryCallback_DiscoveryFinishedFailed_c;
                        pEventData->peerDeviceId = peerDeviceId;
                        if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                        {
                            (void)MEM_BufferFree(pEventData);
                        }
                    }
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
* \brief        Stores handles for the specified service.
*
* \param[in]    peerDeviceId   Peer device ID.
* \param[in]    pService       Pointer to gattService_t.
********************************************************************************** */
static void BleApp_StoreServiceHandles
(
    deviceId_t      peerDeviceId,
    gattService_t   *pService
)
{
    uint8_t i;

    if ((pService->uuidType == gBleUuidType16_c) &&
        (pService->uuid.uuid16 == gBleSig_CCC_DK_UUID_d))
    {
        /* Found DK Service */
        maPeerInformation[peerDeviceId].customInfo.hDkService = pService->startHandle;
        for (i = 0; i < pService->cNumCharacteristics; i++)
        {
            if (pService->aCharacteristics[i].value.uuidType == gBleUuidType128_c)
            {
                if (FLib_MemCmp(pService->aCharacteristics[i].value.uuid.uuid128, uuid_char_vehicle_psm, 16))
                {
                    /* Found Vehicle PSM Char */
                    maPeerInformation[peerDeviceId].customInfo.hPsmChannelChar = pService->aCharacteristics[i].value.handle;

                    maCharacteristics[mcCharVehiclePsmIndex_c].value.handle = maPeerInformation[peerDeviceId].customInfo.hPsmChannelChar;
                    maCharacteristics[mcCharVehiclePsmIndex_c].value.maxValueLength = mcCharVehiclePsmLength_c;
                    maCharacteristics[mcCharVehiclePsmIndex_c].value.paValue = mValVehiclePsm;
                }
                else if (FLib_MemCmp(pService->aCharacteristics[i].value.uuid.uuid128, uuid_char_antenna_id, 16))
                {
                    /* Found Vehicle Antenna Identifier Char */
                    maPeerInformation[peerDeviceId].customInfo.hAntennaIdChar = pService->aCharacteristics[i].value.handle;

                    maCharacteristics[mcCharVehicleAntennaIdIndex_c].value.handle = maPeerInformation[peerDeviceId].customInfo.hAntennaIdChar;
                    maCharacteristics[mcCharVehicleAntennaIdIndex_c].value.maxValueLength = mcCharVehicleAntennaIdLength_c;
                    maCharacteristics[mcCharVehicleAntennaIdIndex_c].value.paValue = (uint8_t *)&mValVehicleAntennaId;
                }
                else
                {
                    /* MISRA */
                }
            }
            else if (pService->aCharacteristics[i].value.uuidType == gBleUuidType16_c)
            {
                if (pService->aCharacteristics[i].value.uuid.uuid16 == (uint16_t)gBleSig_TxPower_d)
                {
                    /* Found Tx Power Char */
                    maPeerInformation[peerDeviceId].customInfo.hTxPowerChar = pService->aCharacteristics[i].value.handle;

                    maCharacteristics[mcCharTxPowerLevelIndex_c].value.handle = maPeerInformation[peerDeviceId].customInfo.hTxPowerChar;
                    maCharacteristics[mcCharTxPowerLevelIndex_c].value.maxValueLength = mcCharTxPowerLevelLength_c;
                    maCharacteristics[mcCharTxPowerLevelIndex_c].value.paValue = (uint8_t *)&mValTxPower;
                }
            }
            else
            {
                /* MISRA */
            }
        }

        mCurrentCharReadingIndex = mcCharVehiclePsmIndex_c;
        (void)GattClient_ReadCharacteristicValue(peerDeviceId, &maCharacteristics[mcCharVehiclePsmIndex_c], mcCharVehiclePsmLength_c);
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
static void BleApp_GattClientCallback(
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

        if(mpfBleEventHandler != NULL)
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_GattClientCallback_GattProcError_c;
                pEventData->peerDeviceId = serverDeviceId;
                if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pEventData);
                }
            }
        }
    }
    else
    {
        if (procedureResult == gGattProcSuccess_c)
        {
            switch(procedureType)
            {
                case gGattProcReadCharacteristicValue_c:
                {
                    if(mpfBleEventHandler != NULL)
                    {
                        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                        if(pEventData != NULL)
                        {
                            pEventData->appEvent = mAppEvt_GattClientCallback_GattProcReadCharacteristicValue_c;
                            pEventData->peerDeviceId = serverDeviceId;
                            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                            {
                                (void)MEM_BufferFree(pEventData);
                            }
                        }
                    }
                }
                break;

                case gGattProcReadUsingCharacteristicUuid_c:
                {
                    if(mpfBleEventHandler != NULL)
                    {
                        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                        if(pEventData != NULL)
                        {
                            pEventData->appEvent = mAppEvt_GattClientCallback_GattProcReadUsingCharacteristicUuid_c;
                            pEventData->peerDeviceId = serverDeviceId;
                            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                            {
                                (void)MEM_BufferFree(pEventData);
                            }
                        }
                    }
                }
                break;

                case gGattProcExchangeMtu_c:
                {
                    /* Get new MTU value */
                    (void)Gatt_GetMtu(serverDeviceId, &mPeerMtu[serverDeviceId]);

                    if(mpfBleEventHandler != NULL)
                    {
                        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                        if(pEventData != NULL)
                        {
                            pEventData->appEvent = mAppEvt_GattClientCallback_GattProcComplete_c;
                            pEventData->peerDeviceId = serverDeviceId;
                            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                            {
                                (void)MEM_BufferFree(pEventData);
                            }
                        }
                    }
                }
                break;

                default:
                {
                    if(mpfBleEventHandler != NULL)
                    {
                        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                        if(pEventData != NULL)
                        {
                            pEventData->appEvent = mAppEvt_GattClientCallback_GattProcComplete_c;
                            pEventData->peerDeviceId = serverDeviceId;
                            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                            {
                                (void)MEM_BufferFree(pEventData);
                            }
                        }
                    }
                }
                break;
            }
        }
    }

    /* Signal Service Discovery Module */
    BleServDisc_SignalGattClientEvent(serverDeviceId, procedureType, procedureResult, error);
}

/*! *********************************************************************************
* \brief        Process scanning events to search for the DK Ranging Service.
*               This function is called from the scanning callback.
*
* \param[in]    pData                   Pointer to gapScannedDevice_t.
*
* \return       TRUE if the scanned device implements the DK Ranging Service,
                FALSE otherwise
********************************************************************************** */
static bool_t CheckScanEventLegacy(gapScannedDevice_t* pData)
{
    uint32_t index = 0;
    bool_t foundMatch = FALSE;
    uint8_t uuid_dk_service[2] = {UuidArray(gBleSig_CCC_DK_UUID_d)};
    while (index < pData->dataLength)
    {
        gapAdStructure_t adElement;

        adElement.length = pData->data[index];
        adElement.adType = (gapAdType_t)pData->data[index + 1U];
        adElement.aData = &pData->data[index + 2U];

         /* Search for DK Ranging Service */
        if ((adElement.adType == gAdIncomplete16bitServiceList_c) ||
          (adElement.adType == gAdComplete16bitServiceList_c))
        {
            foundMatch = BluetoothLEHost_MatchDataInAdvElementList(&adElement, &uuid_dk_service, 2);
        }

        /* Move on to the next AD element type */
        index += (uint32_t)adElement.length + sizeof(uint8_t);
    }

    if (foundMatch)
    {
        /* Update UI */
        shell_write("\r\nLegacy ADV: ");
        shell_writeHexLe(pData->aAddress, gcBleDeviceAddressSize_c);
        shell_write("\r\n");
    }

    return foundMatch;
}

/*! *********************************************************************************
* \brief        Process scanning events to search for the DK Ranging Service.
*               This function is called from the scanning callback.
*
* \param[in]    pData                   Pointer to gapExtScannedDevice_t.
*
* \return       TRUE if the scanned device implements the DK Ranging Service,
                FALSE otherwise
********************************************************************************** */
static bool_t CheckScanEventExtended(gapExtScannedDevice_t* pData)
{
    uint32_t index = 0;
    bool_t foundMatch = FALSE;
    uint8_t uuid_dk_service[2] = {UuidArray(gBleSig_CCC_DK_UUID_d)};
    while (index < pData->dataLength)
    {
        gapAdStructure_t adElement;

        adElement.length = pData->pData[index];
        adElement.adType = (gapAdType_t)pData->pData[index + 1U];
        adElement.aData = &pData->pData[index + 2U];

         /* Search for DK Ranging Service */
        if ((adElement.adType == gAdIncomplete16bitServiceList_c) ||
          (adElement.adType == gAdComplete16bitServiceList_c))
        {
            foundMatch = BluetoothLEHost_MatchDataInAdvElementList(&adElement, &uuid_dk_service, 2);
        }

        /* Move on to the next AD element type */
        index += (uint32_t)adElement.length + sizeof(uint8_t);
    }

    if (foundMatch)
    {
        /* Update UI */
        shell_write("\r\nExtended LR ADV: ");
        shell_writeHexLe(pData->aAddress, gcBleDeviceAddressSize_c);
        shell_write("\r\n");
    }

    return foundMatch;
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
    if(mpfBleEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(appEventL2capPsmData_t) + (uint32_t)packetLength);
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_L2capPsmDataCallback_c;
            pEventData->eventData.pData = pEventData + 1;
            appEventL2capPsmData_t *pL2capPsmDataEvent = pEventData->eventData.pData;
            pL2capPsmDataEvent->deviceId = deviceId;
            pL2capPsmDataEvent->lePsm = lePsm;
            pL2capPsmDataEvent->packetLength = packetLength;
            pL2capPsmDataEvent->pPacket = (uint8_t*)(pL2capPsmDataEvent + 1);
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
        case gL2ca_LePsmConnectRequest_c:
        {
            /* This message is unexpected, the DK Device sends Conn Req and expects a response */
            break;
        }
        case gL2ca_LePsmConnectionComplete_c:
        {
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(l2caLeCbConnectionComplete_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_L2capPsmControlCallback_LePsmConnectionComplete_c;
                    pEventData->eventData.pData = pEventData + 1;
                    FLib_MemCpy(pEventData->eventData.pData, &pMessage->messageData.connectionComplete, sizeof(l2caLeCbConnectionComplete_t));
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
            break;
        }

        case gL2ca_LePsmDisconnectNotification_c:
        {
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_L2capPsmControlCallback_LePsmDisconnectNotification_c;
                    pEventData->peerDeviceId = pMessage->messageData.disconnection.deviceId;
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
            break;
        }

        case gL2ca_NoPeerCredits_c:
        {
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(l2caLeCbNoPeerCredits_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_L2capPsmControlCallback_NoPeerCredits_c;
                    pEventData->eventData.pData = pEventData + 1;
                    FLib_MemCpy(pEventData->eventData.pData, &pMessage->messageData.noPeerCredits, sizeof(l2caLeCbNoPeerCredits_t));
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
            }
            break;
        }

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

        case gL2ca_Error_c:
        {
            /* Handle error */
            break;
        }

        default:
            ; /* For MISRA compliance */
            break;
    }
}

/*! *********************************************************************************
* \brief  This is the callback for Bluetooth LE Host stack initialization.
********************************************************************************** */
static void BluetoothLEHost_Initialized(void)
{
    uint8_t mPeerId = 0U;

    /* Common GAP configuration */
    BleConnManager_GapCommonConfig();

    /* Register for callbacks*/
    (void)App_RegisterGattClientProcedureCallback(BleApp_GattClientCallback);
    BleServDisc_RegisterCallback(BleApp_ServiceDiscoveryCallback);

    /* Initialize private variables */
    for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
    {
        maPeerInformation[mPeerId].appState = mAppIdle_c;
        maPeerInformation[mPeerId].deviceId = gInvalidDeviceId_c;
        maPeerInformation[mPeerId].btcsSupported = FALSE;
    }
    mScanningOn = FALSE;
    mFoundDeviceToConnect = FALSE;

    /* Register stack callbacks */
    (void)App_RegisterLeCbCallbacks(BleApp_L2capPsmDataCallback, BleApp_L2capPsmControlCallback);

    /* Continue CS initialization */
    (void)AppLocalization_HostInitHandler();

    shell_write("\r\nDigital Key Device.\r\n");
    shell_cmd_finished();
}

/*! *********************************************************************************
* \brief  This is the callback for Bluetooth LE CS events
********************************************************************************** */
static void BleApp_CsEventHandler(deviceId_t deviceId, void *pData, appCsEventType_t eventType)
{
    switch (eventType)
    {
        case gCsMetaEvent_c:
        {

        }
        break;

        case gCsCcEvent_c:
        {
            csCommandCompleteEvent_t *pEvent = (csCommandCompleteEvent_t*)pData;
            if (pEvent->eventType == commandError_c)
            {
                shell_write("CS Command Complete error! errorSource: ");
                shell_writeDec(pEvent->eventData.csCommandError.errorSource); /* value in commandErrorSource_t enum */
                shell_write(", status ");
                shell_writeDec(pEvent->eventData.csCommandError.status); /* value in bleResult_t enum */
                SHELL_NEWLINE();
            }
        }
        break;

        case gCsSecurityEnabled_c:
        {
            if (mVerbosityLevel == 2U)
            {
                shell_write("\r\nCS security enabled.\r\n");
            }

            if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
            {
                bleResult_t result = gBleSuccess_c;
                result = AppLocalization_SetProcedureParameters(deviceId);

                if (result != gBleSuccess_c)
                {
                    shell_write("\r\nSet Procedure parameters failed.\r\n");
                }
            }
        }
        break;

        case gConfigComplete_c:
        {
            bleResult_t result = gBleSuccess_c;

            if (mVerbosityLevel == 2U)
            {
                shell_write("\r\nLocalization config complete.\r\n");
            }

            result = AppLocalization_SecurityEnable(deviceId);

            if (result != gBleSuccess_c)
            {
                shell_write("\r\nCS Security Enable failed.\r\n");
            }
        }
        break;

        case gLocalConfigWritten_c:
        {
            if (mVerbosityLevel == 2U)
            {
                shell_write("\r\nLocalization config complete.\r\n");
            }

            bleResult_t result = AppLocalization_SecurityEnable(deviceId);

            if (result != gBleSuccess_c)
            {
                shell_write("\r\nCS Security Enable failed.\r\n");
            }
        }
        break;

        case gSetProcParamsComplete_c:
        {
            bleResult_t result = gBleSuccess_c;
            if (mVerbosityLevel == 2U)
            {
                shell_write("\r\nSet Procedure parameters complete.\r\n");
                shell_write("\r\nStart distance measurement.\r\n");
            }

            result = AppLocalization_StartMeasurement(deviceId);

            if (result != gBleSuccess_c)
            {
                if (mVerbosityLevel == 2U)
                {
                    shell_write("\r\nDistance measurement start failed.\r\n");
                }
            }
        }
        break;

        case gDistanceMeastStarted_c:
        {
            if (mGlobalRangeSettings.role == gCsRoleReflector_c)
            {

                if (mVerbosityLevel == 2U)
                {
                    shell_write("\r\nDistance measurement started.\r\n");
                }
            }
        }
        break;

        case gLocalMeasurementComplete_c:
        {
            uint16_t procCount = AppLocalization_GetProcedureCount(deviceId);

            if ((mVerbosityLevel != 0U) || (procCount == (mRangeSettings[deviceId].maxNumProcedures - 1U)))
            {
                shell_write("\r\nDistance measurement complete. Local data available for procedure index ");
                shell_writeDec(procCount);
                shell_write(".\r\n");
            }
        }
        break;

        case gErrorEvent_c:
        {
            appLocalizationError_t *pError = (appLocalizationError_t*)pData;

            shell_write("Error event for deviceId ");
            shell_writeDec((uint8_t)deviceId);
            shell_write(":\r\n");

            switch (*pError)
            {
                case gAppLclErrorRLSC_c:
                {
                    shell_write("Error occured! Source: csReadLocalSupportedCapabilities!\r\n");
                }
                break;

                case gAppLclUnexpectedCC_c:
                {
                    shell_write("Received an unexpected Config Complete Event!\r\n");
                }
                break;

                case gAppLclUnexpectedRRSCC_c:
                {
                    shell_write("Received an unexpected Read Remote Supported Capabilities Complete Event!\r\n");
                }
                break;

                case gAppLclUnexpectedWCCC_c:
                {
                    shell_write("Received an unexpectedWrite Cached Remote Supported Capabilities Command Complete Event!\r\n");
                }
                break;

                case gAppLclUnexpectedPEC_c:
                {
                    shell_write("Received an unexpected Procedure Enable Complete Event!\r\n");
                }
                break;

                case gAppLclUnexpectedSRE_c:
                {
                    shell_write("Received an unexpected Subevent Result Event!\r\n");
                }
                break;

                case gAppLclUnexpectedSRCE_c:
                {
                    shell_write("Received an unexpected Subevent Result Continue Event!\r\n");
                }
                break;

                case gAppLclErrorRRSCCC_c:
                {
                    shell_write("Error occured! Source: readRemoteSupportedCapabilitiesComplete!\r\n");
                }
                break;

                case gAppLclErrorRRFAETC_c:
                {
                    shell_write("Error occured! Source: readRemoteFAETableComplete!\r\n");
                }
                break;

                case gAppLclErrorSEC_c:
                {
                    shell_write("Error occured! Source: securityEnableComplete!\r\n");
                }
                break;

                case gAppLclErrorCC_c:
                {
                    shell_write("Error occured! Source: configComplete!\r\n");
                }
                break;

                case gAppLclErrorPEC_c:
                {
                    shell_write("Error occured! Source: procedureEnableComplete!\r\n");
                }
                break;

                case gAppLclErrorERE_c:
                {
                    shell_write("Error occured! Source: eventResult!\r\n");
                }
                break;

                case gAppLclErrorERCE_c:
                {
                    shell_write("Error occured! Source: eventResultContinue!\r\n");
                }
                break;

                case gAppLclUnexpectedSPP_c:
                {
                    shell_write("Received an unexpected Set Procedure Parameters Command Status Event!\r\n");
                }
                break;

                case gAppLclInvalidDeviceId_c:
                {
                    shell_write("Received an invalid device Id!\r\n");
                }
                break;

                case gAppLclStartMeasurementFail_c:
                {
                    shell_write("Start measurement failed!\r\n");
                }
                break;

                case gAppLclSDSConfigError_c:
                {
                    shell_write("CS_SetDefaultSettings command failed!\r\n");
                }
                break;

                case gAppLclCCConfigError_c:
                {
                    shell_write("CS_CreateConfig command failed!\r\n");
                }
                break;

                case gAppLclRRSCError_c:
                {
                    shell_write("Error status received! csReadRemoteSupportedCsCapabilities command status event!\r\n");
                }
                break;

                case gAppLclSEError_c:
                {
                    shell_write("Error status received! csSecurityEnable command status event!\r\n");
                }
                break;

                case gAppLclCCError_c:
                {
                    shell_write("Error status received! csCreateConfig command status event!\r\n");
                }
                break;

                case gAppLclProcStatusFailed_c:
                {
                    shell_write("Procedure done status error received!\r\n");
                }
                break;

                case gAppLclProcedureAborted_c:
                {
                    shell_write("All subsequent CS procedures aborted!.\r\n");
                }
                break;

                case gAppLclSubeventStatusFailed_c:
                {
                    shell_write("Subevent status failed!\r\n");
                }
                break;

                case gAppLclProcEndSubeventStatusFailed_c:
                {
                    shell_write("Procedure done, subevent status failed was received.\r\n");
                }
                break;

                case gAppLclNoSubeventMemoryAvailable_c:
                {
                    shell_write("No more memory available for a local subevent!\r\n");
                }
                break;

                case gAppLclErrorProcessingSubevent_c:
                {
                    shell_write("An error occured in the processing of subevent data!\r\n");
                }
                break;

                default:
                {
                    ; /* Do nothing */
                }
                break;
            }
        }
        break;

        case gErrorSubeventAborted_c:
        {
            uint8_t abortReason = *((uint8_t*)pData);

            shell_write("Current CS subevent aborted for deviceId ");
            shell_writeDec((uint8_t)deviceId);
            shell_write("! Abort Reason: ");

            switch (abortReason)
            {
                case (uint8_t)gAppLclNoCsSync_c:
                {
                    shell_write("No CS_SYNC (mode0) received.\r\n");
                }
                break;

                case (uint8_t)gAppLclScheduleConflict_c:
                {
                    shell_write("Scheduling conflicts or limited resources.\r\n");
                }
                break;

                case (uint8_t)gAppLclTimePassed_c:
                {
                    shell_write("Time passed.\r\n");
                }
                break;

                case (uint8_t)gAppLclInvalidArguments_c:
                {
                    shell_write("Invalid arguments.\r\n");
                }
                break;

                case (uint8_t)gAppLclAborted_c:
                {
                    shell_write("Aborted.\r\n");
                }
                break;

                case (uint8_t)gAppLclUnspecifiedReasons_c:
                {
                    shell_write("Unspecified reasons.\r\n");
                }
                break;

                default:
                {
                    shell_write("Unknown!\r\n");
                }
                break;
            }
        }
        break;

        case gErrorProcedureAborted_c:
        {
            uint8_t abortReason = *((uint8_t*)pData);

            shell_write("All subsequent CS procedures aborted for deviceId ");
            shell_writeDec((uint8_t)deviceId);
            shell_write("! Abort Reason: ");

            switch (abortReason)
            {
                case (uint8_t)gAppLclLocalHost_c:
                {
                    shell_write("Abort because of local Host or remote request.\r\n");
                }
                break;

                case (uint8_t)gAppLclRequiredChannelNumber_c:
                {
                    shell_write("Abort because filtered channel map has less than 15 channels.\r\n");
                }
                break;

                case (uint8_t)gAppLclChannelMapInstant_c:
                {
                    shell_write("Abort because the channel map update instant has passed.\r\n");
                }
                break;

                case (uint8_t)gAppLclUnspecifiedReasons_c:
                {
                    shell_write("Abort because of unspecified reasons.\r\n");
                }
                break;

                default:
                {
                    shell_write("Unknown!\r\n");
                }
                break;
            }
        }
        break;

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1)
        case gCsHciDataLogEvent_c:
        {
            csHciDataLogEvent_t *pHciDataLog = (csHciDataLogEvent_t*)pData;

            union
            {
                const uint8_t *p_u8;
                void *v_ptr;
            }temp = {};

            /* Construct full CS HCI data packet */
            uint8_t * pCsHciPacket = MEM_BufferAlloc((uint32_t)pHciDataLog->packetSize + gCsHciDataHdrLength_c);

            if (pCsHciPacket != NULL)
            {
                /* Add header, length and subevent opcode */
                pCsHciPacket[0] = gHciPacketIndicator_c;
                pCsHciPacket[1] = gHciEventCode_c;
                pCsHciPacket[2] = pHciDataLog->packetSize;
                pCsHciPacket[3] = pHciDataLog->opCode;

                /* Add CS data */
                FLib_MemCpy(&(pCsHciPacket[4]), pHciDataLog->pPacket, (uint32_t)pHciDataLog->packetSize - 1U);

                /* Post callback for serial operation to prevent the addition of delays during the procedure */
                if (gBleSuccess_c != App_PostCallbackMessage(App_ExportHciDataLog, (void *)pCsHciPacket))
                {
                    (void)MEM_BufferFree(pCsHciPacket);
                }
            }

            temp.p_u8 = pHciDataLog->pPacket;

            (void)MEM_BufferFree(temp.v_ptr);
        }
        break;
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1) */

        default:
        {
            ; /* Do nothing */
        }
        break;
    }
}

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1)
/*! *********************************************************************************
* \brief        Handler function for exporting HCI data via serial
*
********************************************************************************** */
static void App_ExportHciDataLog(void *pData)
{
    uint8_t *pCsHciPacket = (uint8_t*)pData;

    /* Serial write full packet */
    (void)SerialManager_WriteBlocking(gDataExportSerialWriteHandle, pCsHciPacket, (uint32_t)pCsHciPacket[2] + gCsHciDataHdrLength_c);

    (void)MEM_BufferFree(pCsHciPacket);
}
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1) */
/*! *********************************************************************************
* @}
********************************************************************************** */
