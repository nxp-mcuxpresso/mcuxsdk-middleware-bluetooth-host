/*! *********************************************************************************
* \addtogroup Localization User Device application
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file loc_user_device.c
*
* Copyright 2023-2026 NXP
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
#include "fsl_format.h"
#include "fsl_debug_console.h"
#include "app.h"
#include "board.h"
#include "fwk_platform_ble.h"
#include "NVM_Interface.h"
#include "fsl_shell.h"

/* BLE Host Stack */
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gap_interface.h"
#include "gatt_db_app_interface.h"
#include "gatt_db_handles.h"

/* Profile / Services */
#include "ranging_interface.h"

#include "ble_conn_manager.h"
#include "ble_service_discovery.h"
#include "app_conn.h"
#include "app_scanner.h"
#include "loc_user_device.h"
#include "shell_loc_user_device.h"
#include "app_localization.h"
#include "pts_test.h"
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
typedef enum appEvent_tag
{
    mAppEvt_PeerConnected_c,
    mAppEvt_EncryptionChanged_c,
    mAppEvt_PairingComplete_c,
    mAppEvt_GattProcComplete_c,
    mAppEvt_GattProcError_c,
    mAppEvt_GattServerCallback_CCCDWrittenComplete_c,
    mAppEvt_PeerDisconnected_c
} appEvent_t;

typedef enum appState_tag
{
    mAppIdle_c,
    mAppExchangeMtu_c,
    mAppPairing,
    mAppEncryptLink_c,
    mAppLocalizationSetup_c,
    mAppRunning_c
} appState_t;

typedef struct appPeerInfo_tag
{
    deviceId_t                  deviceId;
    bool_t                      isBonded;
    uint8_t                     nvmIndex;
    appState_t                  appState;
    gapDisconnectionReason_t    disconReason;
} appPeerInfo_t;

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static appPeerInfo_t maPeerInformation[gAppMaxConnections_c];

#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d != 1U)
static appScanningParams_t mAppScanParams = {
    &gScanParams,
    gGapDuplicateFilteringEnable_c,
    gGapScanContinuously_d,
    gGapScanPeriodicDisabled_d
};
static bool_t   mScanningOn = FALSE;
static bool_t   mFoundDeviceToConnect = FALSE;
#endif /* gAppIsPeripheral_d */

static uint16_t mCharMonitoredHandles[4] = { (uint16_t)value_ras_ctrl_point, (uint16_t)cccd_ras_stored_data,
                                             (uint16_t)cccd_ras_real_time_data, (uint16_t)cccd_ras_ctrl_point };
/* Number of the current procedure */
static uint16_t mProcedureCount = 0U;
#if (!defined(gAppUseTAK_d)) || ((defined(gAppUseTAK_d) && gAppUseTAK_d == 0))
static bool_t mRestoringBondedLink = FALSE;
#endif /* defined(gAppUseTAK_d) && gAppUseTAK_d */

#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d == 1U)
static bool_t mAdvOn = FALSE;
#endif

/* LTK */
static uint8_t gaAppSmpLtk[gcSmpMaxLtkSize_c];

/* RAND*/
static uint8_t gaAppSmpRand[gcSmpMaxRandSize_c];

/* IRK */
static uint8_t gaAppSmpIrk[gcSmpIrkSize_c];
static uint8_t gaAppAddress[gcBleDeviceAddressSize_c];
static gapSmpKeys_t gAppOutKeys = {
    .cLtkSize = mcEncryptionKeySize_c,
    .aLtk = (void *)gaAppSmpLtk,
    .aIrk = (void *)gaAppSmpIrk,
    .aCsrk = NULL,
    .aRand = (void *)gaAppSmpRand,
    .cRandSize = gcSmpMaxRandSize_c,
    .ediv = 0U,
    .addressType = 0U,
    .aAddress = gaAppAddress
};
static gapSmpKeyFlags_t gAppOutKeyFlags;
static bool_t gAppOutLeSc;
static bool_t gAppOutAuth;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
/* Local RAS service configuration */
rasStaticConfig_t mRasServiceConfig;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
static button_status_t BleApp_HandleKeys0(void *pButtonHandle,
                                          button_callback_message_t *pMessage,
                                          void *pCallbackParam);
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0)) */
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1))
static button_status_t BleApp_HandleKeys1(void *pButtonHandle,
                                          button_callback_message_t *pMessage,
                                          void *pCallbackParam);
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1)) */
static void BluetoothLEHost_Initialized(void);
static void BleApp_GenericCallback (gapGenericEvent_t* pGenericEvent);
static void BleApp_StateMachineHandler(deviceId_t peerDeviceId, appEvent_t event);
static void BleApp_GattClientCallback(deviceId_t serverDeviceId,
                                      gattProcedureType_t procedureType,
                                      gattProcedureResult_t procedureResult,
                                      bleResult_t error);
static void BleApp_GattServerCallback(deviceId_t deviceId,
                                      gattServerEvent_t *pServerEvent);
static void BleApp_ConnectionCallback(deviceId_t peerDeviceId,
                                      gapConnectionEvent_t* pConnectionEvent);

#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d != 1U)
static void BleApp_ScanningCallback(gapScanningEvent_t* pScanningEvent);
static bool_t CheckScanEventExtended(gapExtScannedDevice_t* pData);
static bool_t CheckScanEventLegacy(gapScannedDevice_t* pData);
#endif /* gAppIsPeripheral_d */

static void BleApp_CsEventHandler(deviceId_t deviceId, void *pData, appCsEventType_t eventType);
static void BleApp_UpdateLocalizationConfig(deviceId_t peerDeviceId, uint16_t connInterval);
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
static void BleApp_PrintMeasurementResults(deviceId_t deviceId, localizationAlgoResult_t *pResult);
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d == 1U)
static void BleApp_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent);
#endif

static void BleApp_ResumeSendNotifis(void *param);

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
    }

    LedStartFlashingAllLeds();
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[0], BleApp_HandleKeys0, NULL);

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1))
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[1], BleApp_HandleKeys1, NULL);
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1)) */
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0)) */

    /* Initialize Bluetooth Host Stack */
    BluetoothLEHost_SetGenericCallback(BleApp_GenericCallback);
    BluetoothLEHost_Init(BluetoothLEHost_Initialized);

    /* UI */
    shellPrompt.constPrompt = "Device>";
    AppShellInit(shellPrompt.prompt);

    /* Temporarily use minimum interval to speed up connection */
    gConnReqParams.connIntervalMin = gGapConnIntervalMin_d;
    gConnReqParams.connIntervalMax = gGapConnIntervalMin_d;

    /* Register CS callback and initialize localization */
    (void)AppLocalization_Init(gCsDefaultRole_c, BleApp_CsEventHandler, NULL);
}

/*! *********************************************************************************
* \brief    Starts the BLE application.
*
********************************************************************************** */
void BleApp_Start(void)
{
#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d != 1U)
    if (!mScanningOn)
    {
        /* Start scanning */
        (void)BluetoothLEHost_StartScanning(&mAppScanParams, BleApp_ScanningCallback);
    }
#else
      /* Set advertising parameters, advertising to start on gAdvertisingParametersSetupComplete_c */
      if (!mAdvOn)
      {

          (void)BluetoothLEHost_StartExtAdvertising(&gAppAdvParams,
                                                    BleApp_AdvertisingCallback,
                                                    BleApp_ConnectionCallback);
      }
#endif
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
* \brief        Trigger CS distance measurement.
*
********************************************************************************** */
bleResult_t BleApp_TriggerCsDistanceMeasurement(deviceId_t deviceId)
{
    bleResult_t result = gBleSuccess_c;

    if (deviceId != gInvalidDeviceId_c)
    {
        /* Check if localization state allows starting a new procedure */
        appLocalization_State_t locState = AppLocalization_GetLocState(deviceId);

        if (locState != gAppLclIdle_c)
        {
            shell_write("\r\n[");
            shell_writeDec(deviceId);
            shell_write("] Cannot start CS procedure, localization procedure in progress \r\n");
            result = gBleInvalidState_c;
        }
        else
        {
            if (Ras_CheckIfSubscribed(deviceId) == TRUE)
            {
                /* Reset data before starting a new procedure */
                AppLocalization_ResetPeer(deviceId, FALSE, gInvalidNvmIndex_c);

                result = AppLocalization_SetProcedureParameters(deviceId);
            }
            else
            {
                shell_write("\r\n[");
                shell_writeDec(deviceId);
                shell_write("] Cannot start CS procedure, RAS client not subscribed\r\n");
                result = gBleInvalidParameter_c;
            }
        }
    }
    else
    {
        for (uint8_t i = 0; i < (uint8_t)gAppMaxConnections_c; i++)
        {
            /* Check if localization state allows starting a new procedure */
            appLocalization_State_t locState = AppLocalization_GetLocState(i);

            if (locState != gAppLclIdle_c)
            {
                shell_write("\r\n[");
                shell_writeDec(i);
                shell_write("] Skipping, localization procedure in progress \r\n");
            }
            else
            {
                if (Ras_CheckIfSubscribed(i) == TRUE)
                {
                    AppLocalization_ResetPeer(i, FALSE, gInvalidNvmIndex_c);
                    result = AppLocalization_SetProcedureParameters(i);

                    if (result == gBleOverflow_c)
                    {
                        shell_write("Maximum concurrent CS procedures reached!\r\n");
                    }
                }
            }
        }
    }

    return result;
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
* \brief        Lists bonding data for all peers.
*
********************************************************************************** */
void BleApp_ListBondingData(void)
{
    gapIdentityInformation_t aIdentity[gMaxBondedDevices_c];
    uint8_t nrBondedDevices = 0;
    uint8_t foundBondedDevices = 0;
    bleResult_t result = Gap_GetBondedDevicesIdentityInformation(aIdentity, gMaxBondedDevices_c, &nrBondedDevices);
    if (gBleSuccess_c == result && nrBondedDevices > 0U)
    {
        for (uint8_t i = 0; i < (uint8_t)gMaxBondedDevices_c; i++)
        {
            result = Gap_LoadKeys((uint8_t)i, &gAppOutKeys, &gAppOutKeyFlags, &gAppOutLeSc, &gAppOutAuth);
            if (gBleSuccess_c == result)
            {
                /* address type, address, ltk, irk */
                shell_write("\r\nNVMIndex: ");
                shell_writeHex((uint8_t*)&i, 1);
                shell_write(" ");
                shell_write(" BondingData: ");
                shell_writeHex((uint8_t*)&gAppOutKeys.addressType, 1);
                shell_write(" ");
                shell_writeHex((uint8_t*)gAppOutKeys.aAddress, 6);
                shell_write(" ");
                shell_writeHex((uint8_t*)gAppOutKeys.aLtk, 16);
                shell_write(" ");
                shell_writeHex((uint8_t*)gAppOutKeys.aIrk, 16);
                foundBondedDevices++;
            }
            if(foundBondedDevices == nrBondedDevices)
            {
                shell_write("\r\n");
                shell_cmd_finished();
                break;
            }
        }
    }
}

#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
/*! *********************************************************************************
* \brief        Run commands to test PTS.
*               Thin wrapper over the shared RAS Server engine located in
*               application/common/lcl/pts_test.c. The RAS Server role does not
*               require any application-specific callbacks.
*
* \param[in]    pParam      Heap-allocated C-string with the PTS test-case ID.
********************************************************************************** */
void BleApp_RunPtsTest(void *pParam)
{
    static const ptsTestCallbacks_t mLocUserDevPtsCbs = {NULL, NULL, NULL};

    PtsTest_RegisterCallbacks(&mLocUserDevPtsCbs);
    PtsTest_RunRasServer(pParam);
}
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
 * \brief        Handle the main application state machine
 *
 * \param[in]    peerDeviceId       The remote device ID.
 * \param[in]    event              The application event.
 ********************************************************************************** */
static void BleApp_StateMachineHandler
(
    deviceId_t peerDeviceId,
    appEvent_t event
)
{
    if (maPeerInformation[peerDeviceId].deviceId != gInvalidDeviceId_c)
    {
        switch (maPeerInformation[peerDeviceId].appState)
        {
        case mAppIdle_c:
        {
            if (event == mAppEvt_PeerConnected_c)
            {
                shell_write("Connected\r\n");
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
                maPeerInformation[peerDeviceId].appState = mAppEncryptLink_c;
#else
                if (maPeerInformation[peerDeviceId].isBonded == TRUE)
                {
                    maPeerInformation[peerDeviceId].appState = mAppEncryptLink_c;
                    mRestoringBondedLink = TRUE;
#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d != 1U)
                    /* Restored custom connection information. Encrypt link */
                    (void)Gap_EncryptLink(peerDeviceId);
#endif
                }
                else
                {
                    maPeerInformation[peerDeviceId].appState = mAppPairing;
#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d != 1U)
                (void)Gap_Pair(peerDeviceId, &gPairingParameters);
#endif
                }
#endif /* #if (defined(gAppUseTAK_d) && gAppUseTAK_d) */
            }
        }
        break;

        case mAppPairing:
        {
            if (event == mAppEvt_PairingComplete_c)
            {
                shell_write("Pairing complete\n\r");
                /* Moving to Exchange MTU State */
                maPeerInformation[peerDeviceId].appState = mAppExchangeMtu_c;
            }
        }
        break;

        case mAppEncryptLink_c:
        {
            if (event == mAppEvt_EncryptionChanged_c)
            {
                shell_write("Link encrypted\n\r");
                /* Moving to Exchange MTU State */
                maPeerInformation[peerDeviceId].appState = mAppExchangeMtu_c;
            }
        }
        break;

        case mAppExchangeMtu_c:
        {
        }
        break;

        case mAppLocalizationSetup_c:
        {
            /* RAS client subscribed */
            if (event == mAppEvt_GattServerCallback_CCCDWrittenComplete_c)
            {
                /* Start CS localization */
                shell_write("Starting channel sounding configuration\r\n");
                maPeerInformation[peerDeviceId].appState = mAppRunning_c;

                if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
                {
                    bleResult_t status = gBleSuccess_c;
                    status = AppLocalization_Config(peerDeviceId);

                    if (status != gBleSuccess_c)
                    {
                        shell_write("Localization configuration failed\r\n");
                    }
                }
            }
        }
        break;

        case mAppRunning_c:
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
    /* Handle disconnect event in all application states. */
    if (event == mAppEvt_PeerDisconnected_c)
    {
        shell_write("Disconnected with reason ");
        shell_writeDec((uint32_t)maPeerInformation[peerDeviceId].disconReason);
        shell_write("\n\r");
        shell_cmd_finished();
        maPeerInformation[peerDeviceId].appState = mAppIdle_c;
        maPeerInformation[peerDeviceId].deviceId = gInvalidDeviceId_c;
        AppLocalization_ResetPeer(peerDeviceId, TRUE, maPeerInformation[peerDeviceId].nvmIndex);
    }
}

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
/*! *********************************************************************************
* \brief        Handler for the first key.
*
* \param[in]    pButtonHandle       Pointer to the button handle.
* \param[in]    pMessage            Pointer to the message.
* \param[in]    pCallbackParam      Pointer to the callback parameters.
********************************************************************************** */
static button_status_t BleApp_HandleKeys0
(
    void *pButtonHandle,
    button_callback_message_t *pMessage,
    void *pCallbackParam
)
{
    uint8_t mPeerId = 0;

    switch (pMessage->event)
    {
        case kBUTTON_EventOneClick:
        case kBUTTON_EventShortPress:
        {

            LedStopFlashingAllLeds();
            Led1Flashing();
            BleApp_Start();
            break;
        }

        case kBUTTON_EventLongPress:
        {
            for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
            {
                if (maPeerInformation[mPeerId].deviceId != gInvalidDeviceId_c)
                {
                    (void)Gap_Disconnect(maPeerInformation[mPeerId].deviceId);
                }
            }

            break;
        }

        default:
        {
            ; /* No action required */
            break;
        }
    }
    return kStatus_BUTTON_Success;
}

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1))
/*! *********************************************************************************
* \brief        Handler for the second key.
*
* \param[in]    pButtonHandle       Pointer to the button handle.
* \param[in]    pMessage            Pointer to the message.
* \param[in]    pCallbackParam      Pointer to the callback parameters.
********************************************************************************** */
static button_status_t BleApp_HandleKeys1
(
    void *pButtonHandle,
    button_callback_message_t *pMessage,
    void *pCallbackParam
)
{
    switch (pMessage->event)
    {
        case kBUTTON_EventOneClick:
        case kBUTTON_EventShortPress:
        {
            uint8_t mPeerId = 0U;

            for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
            {
                if (maPeerInformation[mPeerId].deviceId != gInvalidDeviceId_c)
                {
                    (void)BleApp_TriggerCsDistanceMeasurement(maPeerInformation[mPeerId].deviceId);
                    break;
                }
            }

            if (mPeerId == (uint8_t)gAppMaxConnections_c)
            {
                shell_write("No peer device available\r\n");
            }
            break;
        }

        case kBUTTON_EventLongPress:
            /* No action required */
            break;

        default:
        {
            /* No action required */
            break;
        }
    }

    return kStatus_BUTTON_Success;
}
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1)) */
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0)) */

/*! *********************************************************************************
* \brief        Handles BLE generic callback.
*
* \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
********************************************************************************** */
static void BleApp_GenericCallback(gapGenericEvent_t* pGenericEvent)
{
    /* Call BLE Conn Manager */
    BleConnManager_GenericEvent(pGenericEvent);

    switch (pGenericEvent->eventType)
    {
        case gTxEntryAvailable_c:
        {
            deviceId_t deviceId =  pGenericEvent->eventData.deviceId;

            (void)App_PostCallbackMessage(BleApp_ResumeSendNotifis, &maPeerInformation[deviceId].deviceId);
        }
        break;

        case gLePhyEvent_c:
        {
            if ((pGenericEvent->eventData.phyEvent.phyEventType == gPhyRead_c ) ||
                (pGenericEvent->eventData.phyEvent.phyEventType == gPhyUpdateComplete_c))
            {
                appLocalization_rangeCfg_t locConfig;

                /* Read current CS config */
                (void)AppLocalization_ReadConfig(pGenericEvent->eventData.phyEvent.deviceId, &locConfig);

                /* Set the CS PHY according to the connection PHY */
                locConfig.phy = pGenericEvent->eventData.phyEvent.rxPhy;

                /* Update CS config with the PHY */
                (void)AppLocalization_WriteConfig(pGenericEvent->eventData.phyEvent.deviceId, &locConfig);
            }
        }
        break;

        default:
        {
            ; /* For MISRA compliance */
        }
        break;
    }
}

/*! *********************************************************************************
* \brief  This is the callback for Bluetooth LE Host stack initialization.
********************************************************************************** */
static void BluetoothLEHost_Initialized(void)
{
    uint8_t mPeerId = 0;
    bleResult_t status = gBleSuccess_c;

    /* Common GAP configuration */
    BleConnManager_GapCommonConfig();

    /* Register for callbacks*/
    status = App_RegisterGattClientProcedureCallback(BleApp_GattClientCallback);

    if (status == gBleSuccess_c)
    {
        status = App_RegisterGattServerCallback(BleApp_GattServerCallback);
    }

    if (status == gBleSuccess_c)
    {
        status = GattServer_RegisterHandlesForWriteNotifications(NumberOfElements(mCharMonitoredHandles), mCharMonitoredHandles);
    }

    if (status == gBleSuccess_c)
    {
        /* Initialize private variables */
        for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
        {
            maPeerInformation[mPeerId].appState = mAppIdle_c;
            maPeerInformation[mPeerId].deviceId = gInvalidDeviceId_c;
        }

#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d != 1U)
        mScanningOn = FALSE;
        mFoundDeviceToConnect = FALSE;
#endif

        /* Initialize RAS */
        mRasServiceConfig.serviceHandle = (uint16_t)service_ranging;
        mRasServiceConfig.controlPointHandle = (uint16_t)value_ras_ctrl_point;
        mRasServiceConfig.onDemandDataHandle = (uint16_t)value_ras_stored_data;
        mRasServiceConfig.realTimeDataHandle = (uint16_t)value_ras_real_time_data;
        mRasServiceConfig.dataReadyHandle = (uint16_t)value_ras_ranging_data_ready;
        mRasServiceConfig.dataOverwrittenHandle = (uint16_t)value_ras_ranging_data_overwritten;
        mRasServiceConfig.featuresHandle = (uint16_t)value_ras_feature;
        status = Ras_Start(&mRasServiceConfig);
    }

    if (status == gBleSuccess_c)
    {
        /* Continue CS initialization */
        status = AppLocalization_HostInitHandler();
    }

    if (status == gBleSuccess_c)
    {
        shell_write("\r\nLocalization User Device");
    }
    else
    {
        shell_write("Init error\r\n");
    }

    shell_cmd_finished();
}

#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d != 1U)
/*! *********************************************************************************
* \brief        Process scanning events to search for the Ranging Service.
*               This function is called from the scanning callback.
*
* \param[in]    pData                   Pointer to gapScannedDevice_t.
*
* \return       TRUE if the scanned device implements the Ranging Service,
                FALSE otherwise
********************************************************************************** */
static bool_t CheckScanEventLegacy(gapScannedDevice_t* pData)
{
    uint32_t index = 0;
    bool_t foundMatch = FALSE;
    while (index < pData->dataLength)
    {
        gapAdStructure_t adElement;

        adElement.length = pData->data[index];
        adElement.adType = (gapAdType_t)pData->data[index + 1U];
        adElement.aData = &pData->data[index + 2U];

         /* Search for Ranging Service */
        if ((adElement.adType == gAdIncomplete16bitServiceList_c) ||
          (adElement.adType == gAdComplete16bitServiceList_c))
        {
            uint16_t uuid = gBleSig_RangingService_d;
            foundMatch = BluetoothLEHost_MatchDataInAdvElementList(&adElement, &uuid, (uint8_t)sizeof(uint16_t));
        }

        /* Move on to the next AD element type */
        index += (uint32_t)adElement.length + sizeof(uint8_t);
    }

    if (foundMatch)
    {
        /* Update UI */
        shell_write("Legacy ADV: ");
        shell_writeHexLe(pData->aAddress, gcBleDeviceAddressSize_c);
        shell_write("\r\n");
    }
    return foundMatch;
}

/*! *********************************************************************************
* \brief        Process scanning events to search for the Ranging Service.
*               This function is called from the scanning callback.
*
* \param[in]    pData                   Pointer to gapExtScannedDevice_t.
*
* \return       TRUE if the scanned device implements the Ranging Service,
                FALSE otherwise
********************************************************************************** */
static bool_t CheckScanEventExtended(gapExtScannedDevice_t* pData)
{
    uint32_t index = 0;
    bool_t foundMatch = FALSE;
    while (index < pData->dataLength)
    {
        gapAdStructure_t adElement;

        adElement.length = pData->pData[index];
        adElement.adType = (gapAdType_t)pData->pData[index + 1U];
        adElement.aData = &pData->pData[index + 2U];

         /* Search for Ranging Service */
        if ((adElement.adType == gAdIncomplete16bitServiceList_c) ||
          (adElement.adType == gAdComplete16bitServiceList_c))
        {
            uint16_t uuid = gBleSig_RangingService_d;
            foundMatch = BluetoothLEHost_MatchDataInAdvElementList(&adElement, &uuid, (uint8_t)sizeof(uint16_t));
        }

        /* Move on to the next AD element type */
        index += (uint32_t)adElement.length + sizeof(uint8_t);
    }

    if (foundMatch)
    {
        /* Update UI */
        shell_write("Extended LR ADV: ");
        shell_writeHexLe(pData->aAddress, gcBleDeviceAddressSize_c);
        shell_write("\r\n");
    }
    return foundMatch;
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
            /* Check if the scanned device implements the Ranging Service */
            if( FALSE == mFoundDeviceToConnect )
            {
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
            /* Check if the scanned device implements the Ranging Service */
            if( FALSE == mFoundDeviceToConnect )
            {
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

                shell_write("Scanning\r\n");

                LedStopFlashingAllLeds();
                Led1Flashing();
            }
            /* Node is not scanning */
            else
            {
                shell_write("Scan stopped\r\n");

                /* Connect with the previously scanned peer device */
                if (mFoundDeviceToConnect)
                {
                    shell_write("Connecting\r\n");
                    /* Temporarily use minimum interval to speed up connection */
                    gConnReqParams.connIntervalMin = gcConnectionInterval_c;
                    gConnReqParams.connIntervalMax = gcConnectionInterval_c;
                    (void)BluetoothLEHost_Connect(&gConnReqParams, BleApp_ConnectionCallback);
                }
                else
                {
                    LedStopFlashingAllLeds();
                    Led1Flashing();
                    Led2Flashing();
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
#endif /* gAppIsPeripheral_d */

/*! *********************************************************************************
* \brief        Recomputes and stores the CS localization config that depends on
*               the connection interval. Called when a connection is established
*               and whenever the connection parameters are updated.
*
* \param[in]    peerDeviceId    Peer device ID.
* \param[in]    connInterval    Current connection interval.
********************************************************************************** */
static void BleApp_UpdateLocalizationConfig(deviceId_t peerDeviceId, uint16_t connInterval)
{
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
#if defined(gAppSlopeBasedCalibrationSupport_d) && (gAppSlopeBasedCalibrationSupport_d == 1)
    algoDurationMs += 25U;
#endif /* gAppSlopeBasedCalibrationSupport_d */

    uint32_t procInterval = gMaxCsProcDurationMs_c + gPostProcVerbDurationMs_c + gAppOffsetDurationMs_c + algoDurationMs;
#if defined (BOARD_LOCALIZATION_REVISION_SUPPORT) && (BOARD_LOCALIZATION_REVISION_SUPPORT == 1U)
    procInterval +=  gLocBoardDelayMs_c;
#endif

    /* Convert ms to connection intervals */
    procInterval = 1U + (procInterval * 1000U)/(((uint32_t)(connInterval)) * 1250U);
    locConfig.minPeriodBetweenProcedures = (uint16_t)procInterval;
    locConfig.maxPeriodBetweenProcedures = (uint16_t)procInterval;
    AppLocalization_ComputeMaxProcedureDuration(procInterval, connInterval, &locConfig.maxProcedureDuration);

    (void)AppLocalization_WriteConfig(peerDeviceId, &locConfig);
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
#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d != 1U)
    BleConnManager_GapCentralEvent(peerDeviceId, pConnectionEvent);
#else
    BleConnManager_GapPeripheralEvent(peerDeviceId, pConnectionEvent);
#endif

    switch (pConnectionEvent->eventType)
    {
        case gConnEvtConnected_c:
        {
            uint16_t connInterval = pConnectionEvent->eventData.connectedEvent.connParameters.connInterval;
            /* Update UI */
            LedStopFlashingAllLeds();
            Led1On();
            maPeerInformation[peerDeviceId].deviceId = peerDeviceId;
            maPeerInformation[peerDeviceId].isBonded = FALSE;
            maPeerInformation[peerDeviceId].nvmIndex = gInvalidNvmIndex_c;

#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d == 1U)
             mAdvOn = FALSE;
#endif

            /* Compute the CS config that depends on the connection interval */
            BleApp_UpdateLocalizationConfig(peerDeviceId, connInterval);

            (void)Gap_CheckIfBonded(peerDeviceId, &maPeerInformation[peerDeviceId].isBonded, &maPeerInformation[peerDeviceId].nvmIndex);

            /* Read PHY on which connection was established */
            (void)Gap_LeReadPhy(peerDeviceId);
            BleApp_StateMachineHandler(peerDeviceId, mAppEvt_PeerConnected_c);
        }
        break;

        case gConnEvtDisconnected_c:
        {
            (void)Ras_Unsubscribe(peerDeviceId, TRUE);

            maPeerInformation[peerDeviceId].disconReason = pConnectionEvent->eventData.disconnectedEvent.reason;

            BleApp_StateMachineHandler(peerDeviceId, mAppEvt_PeerDisconnected_c);
            /* UI */
            LedStartFlashingAllLeds();
        }
        break;

        case gConnEvtEncryptionChanged_c:
        {
            if( pConnectionEvent->eventData.encryptionChangedEvent.newEncryptionState )
            {
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
                BleApp_StateMachineHandler(peerDeviceId, mAppEvt_EncryptionChanged_c);
#else
                if( mRestoringBondedLink )
                {
                    mRestoringBondedLink = FALSE;
                    BleApp_StateMachineHandler(peerDeviceId, mAppEvt_EncryptionChanged_c);
                }
#endif /* defined(gAppUseTAK_d) && gAppUseTAK_d */
            }
        }
        break;

        case gConnEvtPairingComplete_c:
        {
            if (pConnectionEvent->eventData.pairingCompleteEvent.pairingSuccessful)
            {
                if (pConnectionEvent->eventData.pairingCompleteEvent.pairingCompleteData.withBonding == TRUE)
                {
                    (void)Gap_CheckIfBonded(peerDeviceId, &maPeerInformation[peerDeviceId].isBonded, &maPeerInformation[peerDeviceId].nvmIndex);
                }
                BleApp_StateMachineHandler(peerDeviceId, mAppEvt_PairingComplete_c);
            }
        }
        break;

        case gConnEvtParameterUpdateComplete_c:
        {
            uint16_t connInterval = pConnectionEvent->eventData.connectionUpdateComplete.connInterval;

            /* Recompute the CS config for the new connection interval */
            BleApp_UpdateLocalizationConfig(peerDeviceId, connInterval);
        }
        break;

        default:
            ; /* No action required */
            break;
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
#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d != 1U)
            /* Start Pairing Procedure */
            (void)Gap_Pair(serverDeviceId, &gPairingParameters);
#endif
        }

        BleApp_StateMachineHandler(serverDeviceId, mAppEvt_GattProcError_c);
    }
    else
    {
        if (procedureResult == gGattProcSuccess_c)
        {
            BleApp_StateMachineHandler(serverDeviceId, mAppEvt_GattProcComplete_c);
        }
    }
}

/*! **********************************************************************************
 * \brief        Handles CCCD written event for RAS characteristics.
 *
 * \param[in]    deviceId           Client peer device ID.
 * \param[in]    pServerEvent       Pointer to gattServerEvent_t.
 ***********************************************************************************/
static void BleApp_HandleCharacteristicCccdWritten
(
    deviceId_t deviceId,
    gattServerEvent_t *pServerEvent
)
{
    uint8_t rasPreferenceValue = Ras_GetDataSendPreference(deviceId);

    switch (pServerEvent->eventData.charCccdWrittenEvent.handle)
    {
        case (uint16_t)cccd_ras_data_ready:
        {
            if ((pServerEvent->eventData.charCccdWrittenEvent.newCccd & gCccdIndication_c) != 0U)
            {
                /* Signal preference for indications for Data Ready */
                rasPreferenceValue |= BIT1;
            }
        }
        break;

        case (uint16_t)cccd_ras_data_overwritten:
        {
            if ((pServerEvent->eventData.charCccdWrittenEvent.newCccd & gCccdIndication_c) != 0U)
            {
                /* Signal preference for indications for Data Overwritten */
                rasPreferenceValue |= BIT2;
            }
        }
        break;

        default:
        {
            /* Should not get here */
            rasPreferenceValue = 0xFF;
        }
        break;
    }

    (void)Ras_SetDataSendPreference(deviceId, rasPreferenceValue);
}

/*! **********************************************************************************
 * \brief        Handles RAS control point CCCD write.
 *
 * \param[in]    deviceId           Client peer device ID.
 * \param[in]    pServerEvent       Pointer to gattServerEvent_t.
 ***********************************************************************************/
static void BleApp_HandleRasCtrlPointCccdWrite
(
    deviceId_t deviceId,
    gattServerEvent_t *pServerEvent
)
{
    uint8_t status = (uint8_t)gAttErrCodeNoError_c;
    uint16_t newCccd = Utils_ExtractTwoByteValue(pServerEvent->eventData.attributeWrittenEvent.aValue);

    if (newCccd != gCccdIndication_c)
    {
        status = (uint8_t)gAttErrCodeWriteRequestRejected_c;
    }

    if (pServerEvent->eventType == gEvtAttributeWritten_c)
    {
        (void)GattServer_SendAttributeWrittenStatus(deviceId,
                                                    pServerEvent->eventData.attributeWrittenEvent.handle,
                                                    status);
    }
}

/*! **********************************************************************************
 * \brief        Handles RAS control point value write.
 *
 * \param[in]    deviceId           Client peer device ID.
 * \param[in]    pServerEvent       Pointer to gattServerEvent_t.
 ***********************************************************************************/
static void BleApp_HandleRasCtrlPointValueWrite
(
    deviceId_t deviceId,
    gattServerEvent_t *pServerEvent
)
{
    rasControlPointReq_t *rasCtrlPointCmd = (rasControlPointReq_t*)(void*)pServerEvent->eventData.attributeWrittenEvent.aValue;

    if (pServerEvent->eventType == gEvtAttributeWritten_c)
    {
        (void)GattServer_SendAttributeWrittenStatus(deviceId,
                                              pServerEvent->eventData.attributeWrittenEvent.handle,
                                              (uint8_t)gAttErrCodeNoError_c);
    }

    /* RAS control point characteristic was written */
    bleResult_t result = Ras_ControlPointHandler(deviceId,
                                                 &pServerEvent->eventData.attributeWrittenEvent);

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
    if (rasCtrlPointCmd->cmdOpCode == getRangingDataOpCode_c)
    {
        gCsTimeInfo.transferStart = TM_GetTimestamp();
    }

    if (rasCtrlPointCmd->cmdOpCode == ackRangingDataOpCode_c)
    {
        localizationAlgoResult_t algoResult = {0};

        gCsTimeInfo.transferEnd = TM_GetTimestamp();
        algoResult.algorithm = 0U;
        algoResult.csConfigDuration = gCsTimeInfo.csConfigEndTs - gCsTimeInfo.csConfigStartTs;
        algoResult.csProcedureDuration = gCsTimeInfo.csDistMeasDuration;
        algoResult.transferDuration = gCsTimeInfo.transferEnd - gCsTimeInfo.transferStart;

        BleApp_PrintMeasurementResults(deviceId,  &algoResult);
    }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

    if (result != gBleSuccess_c)
    {
        shell_write("[");
        shell_writeDec(deviceId);
        shell_write("] RAS Error Received! Error code: ");
        shell_writeDec((uint32_t)result);
        shell_write(".\r\n");
    }

    if (rasCtrlPointCmd->cmdOpCode == (uint8_t)ackRangingDataOpCode_c)
    {
        shell_write("RAS transfer completed for procedure index ");
        shell_writeDec((uint32_t)mProcedureCount - 1U); /* It was previously incremented at CS procedure completion */
        shell_write(".\r\n");

        if (mProcedureCount == mRangeSettings[deviceId].maxNumProcedures)
        {
            shell_cmd_finished();
        }
    }
}

/*! **********************************************************************************
 * \brief        Handles RAS real-time data CCCD write.
 *
 * \param[in]    deviceId           Client peer device ID.
 * \param[in]    pServerEvent       Pointer to gattServerEvent_t.
 ***********************************************************************************/
static void BleApp_HandleRasRealTimeDataCccdWrite
(
    deviceId_t deviceId,
    gattServerEvent_t *pServerEvent
)
{
    uint16_t *newCccd = (uint16_t*)(void*)pServerEvent->eventData.attributeWrittenEvent.aValue;
    uint8_t status = (uint8_t)gAttErrCodeNoError_c;
    uint8_t rasPreferenceValue = Ras_GetDataSendPreference(deviceId);

    if (*newCccd == 0U)
    {
        (void)Ras_Unsubscribe(deviceId, FALSE);
        shell_write("Ranging Client Unsubscribed for Real-Time\r\n");
        (void)GattDb_WriteAttribute(pServerEvent->eventData.attributeWrittenEvent.handle,
                                  pServerEvent->eventData.attributeWrittenEvent.cValueLength,
                                  pServerEvent->eventData.attributeWrittenEvent.aValue);
        (void)Gap_SaveCccd(deviceId, pServerEvent->eventData.attributeWrittenEvent.handle, (gattCccdFlags_t)(*newCccd));
    }
    else
    {
        if ((Ras_CheckIfSubscribed(deviceId) == FALSE) || (Ras_CheckRealTimeData(deviceId) == TRUE))
        {
            (void)GattDb_WriteAttribute(pServerEvent->eventData.attributeWrittenEvent.handle,
                                      pServerEvent->eventData.attributeWrittenEvent.cValueLength,
                                      pServerEvent->eventData.attributeWrittenEvent.aValue);
            (void)Gap_SaveCccd(deviceId, pServerEvent->eventData.attributeWrittenEvent.handle, (gattCccdFlags_t)(*newCccd));

            if ((*newCccd) == gCccdIndication_c)
            {
                /* Signal preference for indications for RAS On-Demand Data */
                rasPreferenceValue |= BIT3;
            }
            (void)Ras_SetDataSendPreference(deviceId, rasPreferenceValue);

            if (Ras_CheckIfSubscribed(deviceId) == FALSE)
            {
                (void)Ras_Subscribe(deviceId, TRUE);
            }
            shell_write("Ranging Client Subscribed for Real-Time\r\n");
        }
        else
        {
            status = (uint8_t)gAttErrCodeCccdImproperlyConfigured_c;
        }
    }

    if (pServerEvent->eventType == gEvtAttributeWritten_c)
    {
        (void)GattServer_SendAttributeWrittenStatus(deviceId,
                                                    pServerEvent->eventData.attributeWrittenEvent.handle,
                                                    status);
    }

    BleApp_StateMachineHandler(deviceId, mAppEvt_GattServerCallback_CCCDWrittenComplete_c);
}

/*! **********************************************************************************
 * \brief        Handles RAS stored data CCCD write.
 *
 * \param[in]    deviceId           Client peer device ID.
 * \param[in]    pServerEvent       Pointer to gattServerEvent_t.
 ***********************************************************************************/
static void BleApp_HandleRasStoredDataCccdWrite
(
    deviceId_t deviceId,
    gattServerEvent_t *pServerEvent
)
{
    uint16_t *newCccd = (uint16_t*)(void*)pServerEvent->eventData.attributeWrittenEvent.aValue;
    uint8_t status = (uint8_t)gAttErrCodeNoError_c;
    uint8_t rasPreferenceValue = Ras_GetDataSendPreference(deviceId);

    if (*newCccd == 0U)
    {
        (void)Ras_Unsubscribe(deviceId, FALSE);
        shell_write("Ranging Client Unsubscribed for On-Demand\r\n");
        (void)GattDb_WriteAttribute(pServerEvent->eventData.attributeWrittenEvent.handle,
                                  pServerEvent->eventData.attributeWrittenEvent.cValueLength,
                                  pServerEvent->eventData.attributeWrittenEvent.aValue);
        (void)Gap_SaveCccd(deviceId, pServerEvent->eventData.attributeWrittenEvent.handle, (gattCccdFlags_t)(*newCccd));
    }
    else
    {
        if ((Ras_CheckIfSubscribed(deviceId) == FALSE) || (Ras_CheckRealTimeData(deviceId) == FALSE))
        {
            (void)GattDb_WriteAttribute(pServerEvent->eventData.attributeWrittenEvent.handle,
                                      pServerEvent->eventData.attributeWrittenEvent.cValueLength,
                                      pServerEvent->eventData.attributeWrittenEvent.aValue);
            (void)Gap_SaveCccd(deviceId, pServerEvent->eventData.attributeWrittenEvent.handle, (gattCccdFlags_t)(*newCccd));

            if ((*newCccd) == gCccdIndication_c)
            {
                /* Signal preference for indications for RAS On-Demand Data */
                rasPreferenceValue |= BIT0;
            }
            (void)Ras_SetDataSendPreference(deviceId, rasPreferenceValue);

            if (Ras_CheckIfSubscribed(deviceId) == FALSE)
            {
                (void)Ras_Subscribe(deviceId, FALSE);
            }
            shell_write("Ranging Client Subscribed for On-Demand\r\n");
        }
        else
        {
            status = (uint8_t)gAttErrCodeCccdImproperlyConfigured_c;
        }
    }

    if (pServerEvent->eventType == gEvtAttributeWritten_c)
    {
        (void)GattServer_SendAttributeWrittenStatus(deviceId,
                                                    pServerEvent->eventData.attributeWrittenEvent.handle,
                                                    status);
    }
    
    BleApp_StateMachineHandler(deviceId, mAppEvt_GattServerCallback_CCCDWrittenComplete_c);
}

/*! **********************************************************************************
 * \brief        Handles attribute written events.
 *
 * \param[in]    deviceId           Client peer device ID.
 * \param[in]    pServerEvent       Pointer to gattServerEvent_t.
 ***********************************************************************************/
static void BleApp_HandleAttributeWritten
(
    deviceId_t deviceId,
    gattServerEvent_t *pServerEvent
)
{
    if (pServerEvent->eventData.attributeWrittenEvent.handle == (uint16_t)cccd_ras_ctrl_point)
    {
        BleApp_HandleRasCtrlPointCccdWrite(deviceId, pServerEvent);
    }

    if (pServerEvent->eventData.attributeWrittenEvent.handle == (uint16_t)value_ras_ctrl_point)
    {
        BleApp_HandleRasCtrlPointValueWrite(deviceId, pServerEvent);
    }

    if (pServerEvent->eventData.attributeWrittenEvent.handle == (uint16_t)cccd_ras_real_time_data)
    {
        BleApp_HandleRasRealTimeDataCccdWrite(deviceId, pServerEvent);
    }

    if (pServerEvent->eventData.attributeWrittenEvent.handle == (uint16_t)cccd_ras_stored_data)
    {
        BleApp_HandleRasStoredDataCccdWrite(deviceId, pServerEvent);
    }
}

/*! **********************************************************************************
 * \brief        Handles value confirmation event.
 *
 * \param[in]    deviceId           Client peer device ID.
 ***********************************************************************************/
static void BleApp_HandleValueConfirmation
(
    deviceId_t deviceId
)
{
    /* Confirm indication received */
    Ras_GattValueConfirmationHandler(deviceId);

    if (Ras_CheckTransferInProgress(deviceId))
    {
        if ((Ras_GetDataSendPreference(deviceId) & BIT0) != 0U)
        {
            /* On-Demand data transfer in progress through indications */
            (void)Ras_SendRangingDataIndication(deviceId, (uint16_t)value_ras_stored_data);
        }
    }

    if (Ras_CheckSegmentTransmInProgress(deviceId))
    {
        /* Segment retransmission in progress through indications */
        (void)Ras_HandleGetRangingDataSegmInd(deviceId);
    }

    if ((Ras_CheckRealTimeData(deviceId)) && ((Ras_GetDataSendPreference(deviceId) & BIT3) != 0U))
    {
        /* Real-Time data transfer in progress through indications */
        (void)Ras_SendRangingDataIndication(deviceId, (uint16_t)value_ras_real_time_data);
    }
}

/*! **********************************************************************************
 * \brief        Handles MTU changed event.
 *
 * \param[in]    deviceId           Client peer device ID.
 * \param[in]    pServerEvent       Pointer to gattServerEvent_t.
 ***********************************************************************************/
static void BleApp_HandleMtuChanged
(
    deviceId_t deviceId,
    gattServerEvent_t *pServerEvent
)
{
    /* Get new MTU value to register with RAS */
    shell_write("MTU Exchange complete\n\r");

    Ras_SetMtuValue(deviceId, pServerEvent->eventData.mtuChangedEvent.newMtu);
    /* Moving to Service Discovery State*/
    maPeerInformation[deviceId].appState = mAppLocalizationSetup_c;
}

/*! **********************************************************************************
 * \brief        Handles GATT server callback from host stack.
 *
 * \param[in]    deviceId           Client peer device ID.
 * \param[in]    pServerEvent       Pointer to gattServerEvent_t.
 ***********************************************************************************/
static void BleApp_GattServerCallback
(
    deviceId_t deviceId,
    gattServerEvent_t *pServerEvent
)
{
    switch (pServerEvent->eventType)
    {
        case gEvtCharacteristicCccdWritten_c:
        {
            BleApp_HandleCharacteristicCccdWritten(deviceId, pServerEvent);
        }
        break;

        case gEvtAttributeWrittenWithoutResponse_c:
        case gEvtAttributeWritten_c:
        {
            BleApp_HandleAttributeWritten(deviceId, pServerEvent);
        }
        break;

        case gEvtHandleValueConfirmation_c:
        {
            BleApp_HandleValueConfirmation(deviceId);
        }
        break;

        case gEvtMtuChanged_c:
        {
            BleApp_HandleMtuChanged(deviceId, pServerEvent);
        }
        break;

        case gEvtError_c:
        {
            if (pServerEvent->eventData.procedureError.procedureType == gSendIndication_c)
            {
                Ras_ClearIndicationPendingFlag(deviceId);
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

/*! **********************************************************************************
 * \brief  Handle CS error event messages - Part 1.
 *
 * \param[in]    error          Error type
 *
 * \return       bool           True if handled, false otherwise
 ********************************************************************************** */
static bool BleApp_HandleCsErrorPart1
(
    appLocalizationError_t error
)
{
    bool handled = true;
    
    switch (error)
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

        default:
        {
            handled = false;
        }
        break;
    }

    return handled;
}

/*! **********************************************************************************
 * \brief  Handle CS error event messages - Part 2.
 *
 * \param[in]    error          Error type
 *
 * \return       bool           True if handled, false otherwise
 ********************************************************************************** */
static bool BleApp_HandleCsErrorPart2
(
    appLocalizationError_t error
)
{
    bool handled = true;

    switch (error)
    {
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

        case gAppLclErrorSEC_c:
        {
            shell_write("Error occured! Source: securityEnableComplete!\r\n");
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

        default:
        {
            handled = false;
        }
        break;
    }

    return handled;
}

/*! **********************************************************************************
 * \brief  Handle CS error event messages - Part 3.
 *
 * \param[in]    error          Error type
 ********************************************************************************** */
static void BleApp_HandleCsErrorPart3
(
    appLocalizationError_t error
)
{
    switch (error)
    {
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

        case gAppLclRasTransferFailed_c:
        {
            shell_write("Received an error response from RAS server!.\r\n");
        }
        break;

        case gAppLclProcEndSubeventStatusFailed_c:
        {
            shell_write("Procedure done, subevent status failed was received.\r\n");
        }
        break;

        case gAppLclRasSendIndicationFailed_c:
        {
            shell_write("Error occured! Ras_SendDataReady call failed.\r\n");
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
        ; /* Do nothing */
        break;
    }
}

/*! **********************************************************************************
 * \brief  Handle CS error event messages.
 *
 * \param[in]    deviceId       Device identifier
 * \param[in]    pError         Pointer to error type
 ********************************************************************************** */
static void BleApp_HandleCsErrorEvent
(
    deviceId_t deviceId,
    appLocalizationError_t *pError
)
{
    shell_write("Error event for deviceId ");
    shell_writeDec((uint8_t)deviceId);
    shell_write(":\r\n");
    
    if (!BleApp_HandleCsErrorPart1(*pError))
    {
        if (!BleApp_HandleCsErrorPart2(*pError))
        {
            BleApp_HandleCsErrorPart3(*pError);
        }
    }
}

/*! **********************************************************************************
 * \brief  Handle CS subevent abort event.
 *
 * \param[in]    deviceId       Device identifier
 * \param[in]    abortReason    Abort reason code
 ********************************************************************************** */
static void BleApp_HandleSubeventAborted
(
    deviceId_t deviceId,
    uint8_t abortReason
)
{
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

        case (uint8_t)gAppLclLocalHostSubevent_c:
        {
            shell_write("Abort because of local Host or remote request.\r\n");
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
                    shell_write("Unknown! Abort reason: 0x");
                    shell_writeHex(&abortReason, (uint8_t)sizeof(uint8_t));
                    shell_write("\r\n");
        }
        break;
    }
}

/*! **********************************************************************************
 * \brief  Handle CS procedure abort event.
 *
 * \param[in]    deviceId       Device identifier
 * \param[in]    abortReason    Abort reason code
 ********************************************************************************** */
static void BleApp_HandleProcedureAborted
(
    deviceId_t deviceId,
    uint8_t abortReason
)
{
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
            shell_write("Unknown! Abort reason: 0x");
            shell_writeHex(&abortReason, (uint8_t)sizeof(uint8_t));
            shell_write("\r\n");
        }
        break;
    }
}

/*! **********************************************************************************
 * \brief  This is the callback for Bluetooth LE CS events
 ********************************************************************************** */
static void BleApp_CsEventHandler
(
    deviceId_t deviceId,
    void *pData,
    appCsEventType_t eventType
)
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
                shell_writeDec((uint32_t)pEvent->eventData.csCommandError.errorSource); /* value in commandErrorSource_t enum */
                shell_write(", status: 0x ");
                shell_writeHexLe((uint8_t*)&pEvent->eventData.csCommandError.status, 2); /* value in bleResult_t enum */
                SHELL_NEWLINE();
            }
        }
        break;

        case gCsSecurityEnabled_c:
        {
            shell_write("CS security enabled.\r\n");

            if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
            {
                bleResult_t result = gBleSuccess_c;
                result = AppLocalization_SetProcedureParameters(deviceId);

                if (result != gBleSuccess_c)
                {
                    shell_write("Set Procedure parameters failed.\r\n");
                }
            }
        }
        break;

        case gConfigComplete_c:
        case gLocalConfigWritten_c:
        {
            shell_write("Localization config complete.\r\n");
#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d != 1U)
            bleResult_t result = AppLocalization_SecurityEnable(deviceId);

            if (result != gBleSuccess_c)
            {
                shell_write("CS Security Enable failed.\r\n");
            }
#endif
        }
        break;

        case gSetProcParamsComplete_c:
        {
            bleResult_t result = gBleSuccess_c;

            shell_write("Set Procedure parameters complete.\r\n");
            shell_write("Start distance measurement.\r\n");
            mProcedureCount = 0U;

            result = AppLocalization_StartMeasurement(deviceId);

            if (result != gBleSuccess_c)
            {
                shell_write("Distance measurement start failed.\r\n");
            }
        }
        break;

        case gDistanceMeastStarted_c:
        {
            if (mGlobalRangeSettings.role == gCsRoleReflector_c)
            {
                mProcedureCount = 0U;
                shell_write("Distance measurement started.\r\n");
            }
        }
        break;

        case gLocalMeasurementComplete_c:
        {
            mProcedureCount++;
            shell_write("Distance measurement complete. Local data available\r\n");
        }
        break;

        case gDataOverwritten_c:
        {
            shell_write("\r\nSent Data Overwritten Indication!\r\n");
        }
        break;

        case gErrorEvent_c:
        {
            BleApp_HandleCsErrorEvent(deviceId, (appLocalizationError_t*)pData);
        }
        break;

        case gErrorSubeventAborted_c:
        {
            BleApp_HandleSubeventAborted(deviceId, *((uint8_t*)pData));
        }
        break;

        case gErrorProcedureAborted_c:
        {
            BleApp_HandleProcedureAborted(deviceId, *((uint8_t*)pData));
        }
        break;

        default:
        ; /* Do nothing */
        break;
    }
}

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
/*! *********************************************************************************
* \brief  This is the callback for displaying distance measurement results
********************************************************************************** */
static void BleApp_PrintMeasurementResults(deviceId_t deviceId, localizationAlgoResult_t *pResult)
{
        shell_write("Time information:");
        shell_write("\r\n");
        if (pResult->csConfigDuration != 0)
        {
            shell_write("CS Config: ");
            shell_writeDec(pResult->csConfigDuration/1000);
            shell_write("ms\r\n");
        }
        if (pResult->csProcedureDuration != 0)
        {
            shell_write("CS Procedure: ");
            shell_writeDec(pResult->csProcedureDuration/1000);
            shell_write("ms\r\n");
        }
        if (pResult->transferDuration != 0)
        {
            shell_write("RAS transfer: ");
            shell_writeDec(pResult->transferDuration/1000);
            shell_write("ms\r\n");
        }
        if (pResult->algoDuration != 0)
        {
            shell_write("Localization algorithm: ");
            shell_writeDec(pResult->algoDuration/1000);
            shell_write("ms\r\n");
        }
}
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d == 1U)
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
            if (mAdvOn == FALSE)
            {
                mAdvOn = !mAdvOn;
                shell_write("Advertising started\r\n");

                 /* UI */
                 LedStopFlashingAllLeds();
                 Led1Flashing();
            }
            else
            {
                mAdvOn = !mAdvOn;
                shell_write("Advertising stopped\r\n");

                /* UI */
                LedStopFlashingAllLeds();
                Led1Flashing();
                Led2Flashing();
            }
        }
        break;

        case gAdvertisingCommandFailed_c:
        {
            /* Panic UI */
            Led2On();
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
#endif /* gAppIsPeripheral_d */

/*! *********************************************************************************
* @brief        Resumes  sending BLE notifications after a buffer overflow pause.
*
* @param[in]    param    Pointer to the device ID for which notifications should 
*                        resume.
************************************************************************************/
static void BleApp_ResumeSendNotifis(void *param)
{
    deviceId_t deviceId =  *(deviceId_t *)param;

    if (Ras_GetNotificationsPause(deviceId) == TRUE)
    {
        Ras_SetNotificationsPause(deviceId, FALSE);

        if (Ras_CheckRealTimeData(deviceId))
        {
            (void)Ras_SendRangingDataNotifs(deviceId, mRasServiceConfig.realTimeDataHandle);
        }
        else
        {
            (void)Ras_SendRangingDataNotifs(deviceId, mRasServiceConfig.onDemandDataHandle);
        }
    }
}
/*! *********************************************************************************
* @}
********************************************************************************** */
