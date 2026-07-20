/*! *********************************************************************************
* \addtogroup Localization Reader application
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file loc_reader.c
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
#include "fsl_component_led.h"
#include "fsl_adapter_reset.h"

#include "app.h"
#include "board.h"
#include "fwk_platform_ble.h"
#include "RNG_Interface.h"
#include "fwk_platform_lcl.h"

/* BLE Host Stack */
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gap_interface.h"
#include "gatt_db_app_interface.h"
#include "gatt_db_handles.h"
#include "ble_conn_manager.h"
#include "ble_service_discovery.h"
#include "app_conn.h"
#include "loc_reader.h"
#include "shell_loc_reader.h"
#include "app_localization.h"
#include "pts_test.h"
#include "app_localization_algo.h"
#include "app_scanner.h"
#include "ranging_client_interface.h"
#include "ranging_interface.h"
#include "ncp_app.h"
#include "NVM_Interface.h"

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
uint16_t gFilterShellVal = (uint16_t)gNoFilter_c;
bool_t filterTestSend = FALSE;

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
    mAppEvt_ExchangeMtuComplete_c,
    mAppEvt_ServiceDiscoveryComplete_c,
    mAppEvt_ServiceDiscoveryFailed_c,
    mAppEvt_WriteCharacteristicDescriptorComplete_c,
    mAppEvt_ReadCharacteristicValueComplete_c,
    mAppEvt_WriteCharacteristicValueComplete_c,
    mAppEvt_GattProcError_c,
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
    bool_t                      isSubscribed;
    appState_t                  appState;
    rasStaticConfig_t           rasConfigInfo;
    gapDisconnectionReason_t    disconReason;
}appPeerInfo_t;
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static appPeerInfo_t maPeerInformation[gAppMaxConnections_c];

#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d == 1U)
static bool_t mAdvOn = FALSE;
#endif

/* Buffer used for Characteristic related procedures */
static gattAttribute_t      *mpCharProcBuffer = NULL;
static uint16_t mProcedureCount = 0x0U;
static gattCharacteristic_t  mpRasCharacteristic;

#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d != 1U)
static appScanningParams_t mAppScanParams = {
    &gScanParams,
    gGapDuplicateFilteringDisable_c,
    gGapScanContinuously_d,
    gGapScanPeriodicDisabled_d
};
static bool_t   mScanningOn = FALSE;
static bool_t   mFoundDeviceToConnect = FALSE;
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
static bool_t   mbDeviceToConnectHasTAK = FALSE;
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */

static void BleApp_ScanningCallback(gapScanningEvent_t* pScanningEvent);
static bool_t CheckScanEventExtended(gapExtScannedDevice_t* pData);
static bool_t CheckScanEventLegacy(gapScannedDevice_t* pData);
#endif

static bool_t mRestoringBondedLink = FALSE;
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
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
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
static void BleApp_StateMachineHandler(deviceId_t peerDeviceId, appEvent_t event);
static void BleApp_GenericCallback(gapGenericEvent_t* pGenericEvent);
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
#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d == 1U)
static void BleApp_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent);
#endif
void BleApp_ConnectionCallback (deviceId_t peerDeviceId,
                                gapConnectionEvent_t* pConnectionEvent);
static void BleApp_GattClientCallback(deviceId_t serverDeviceId,
                                      gattProcedureType_t procedureType,
                                      gattProcedureResult_t procedureResult,
                                      bleResult_t error);
static void BleApp_GattNotificationCallback(deviceId_t serverDeviceId,
                                            uint16_t characteristicValueHandle,
                                            uint8_t* aValue,
                                            uint16_t valueLength);
static void BleApp_GattIndicationCallback(deviceId_t serverDeviceId,
                                          uint16_t characteristicValueHandle,
                                          uint8_t* aValue,
                                          uint16_t valueLength);
static void BleApp_StoreServiceHandles(deviceId_t peerDeviceId,
                                       gattService_t *pService);
static void BleApp_ServiceDiscoveryCallback(deviceId_t peerDeviceId,
                                            servDiscEvent_t* pEvent);

static void BleApp_CsEventHandler(deviceId_t deviceId, void *pData, appCsEventType_t eventType);
static void BleApp_UpdateLocalizationConfig(deviceId_t peerDeviceId, uint16_t connInterval);

static bleResult_t BleApp_ConfigureRasServer
(
    deviceId_t peerDeviceId,
    uint16_t value,
    uint16_t handle
);

static void BleApp_HandleRasSubscription
(
    deviceId_t peerDeviceId
);

#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
static void BleApp_SwitchRealTimeDataState(
    deviceId_t deviceId,
    uint16_t value,
    uint8_t *testStep
);
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief    Initializes application specific functionality before the BLE stack init.
*
********************************************************************************** */
void BluetoothLEHost_AppInit(void)
{
    union Prompt_tag
    {
        const char * constPrompt;
        char * prompt;
    } shellPrompt;

    uint8_t mPeerId = 0;

    /* Initialize table with peer devices information  */
    for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
    {
        maPeerInformation[mPeerId].deviceId = gInvalidDeviceId_c;
        maPeerInformation[mPeerId].appState = mAppIdle_c;
    }
    /* UI */
    LedStartFlashingAllLeds();
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[0],
                                 BleApp_HandleKeys0, NULL);
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0)) */
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1))
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[1],
                                 BleApp_HandleKeys1, NULL);
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1)) */

    /* Register FSIC module to send IQ samples to core 0 */
    (void)App_NcpAppFsciInit();

    /* Set generic callback */
    BluetoothLEHost_SetGenericCallback(BleApp_GenericCallback);
    /* Initialize Bluetooth Host Stack */
    BluetoothLEHost_Init(BluetoothLEHost_Initialized);
    /* UI */
    shellPrompt.constPrompt = "Reader>";
    AppShellInit(shellPrompt.prompt);

     /* Register CS callback and initialize localization */
    (void)AppLocalization_Init(gCsDefaultRole_c,
                               BleApp_CsEventHandler,
                               NULL);
}

/*! *********************************************************************************
* \brief    Starts the BLE application.
*
********************************************************************************** */
void BleApp_Start(void)
{
#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d == 1U)
    /* Device is not already advertising */
    if (!mAdvOn)
    {
        /* UI update */
        Led1On();

        /* Set advertising parameters, advertising to start on gAdvertisingParametersSetupComplete_c */
        (void)BluetoothLEHost_StartExtAdvertising(&gAppAdvParams,
                                                  BleApp_AdvertisingCallback,
                                                  BleApp_ConnectionCallback);
    }
#else
    if (!mScanningOn)
    {
        /* Start scanning */
        (void)BluetoothLEHost_StartScanning(&mAppScanParams, BleApp_ScanningCallback);
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
            if (maPeerInformation[deviceId].isSubscribed == TRUE)
            {
                /* Reset data before starting a new procedure */
                AppLocalization_ResetPeer(deviceId, FALSE, gInvalidNvmIndex_c);

                result = AppLocalization_SetProcedureParameters(deviceId);

                if (result == gBleOverflow_c)
                {
                    shell_write("Maximum concurrent CS procedures reached!\r\n");
                }
            }
            else
            {
                shell_write("\r\n[");
                shell_writeDec(deviceId);
                shell_write("] Cannot start CS procedure, not subscribed to RAS service\r\n");
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
                shell_write("] Skipping, localization procedure in progress\r\n");
            }
            else
            {
                if (maPeerInformation[i].isSubscribed == TRUE)
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

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
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
*
********************************************************************************** */
void BleApp_RunPtsTest(void *pParam)
{
    const deviceId_t deviceId = 0U;
    char *pArg = (char*)pParam;

    if (strcmp(pArg, "RAP/REQ/RRD/BV-01-C") == 0)
    {
        static uint8_t testStep = 0U;

        switch(testStep)
        {
            case 0U:
            {
                AppLocalization_RunPtsTest(deviceId, 101U, 0U);
                testStep++;
                break;
            }
            case 1U:
            case 3U:
            {
                /* Disable Real-Time data transfer */
                BleApp_SwitchRealTimeDataState(deviceId, gCccdEmpty_c, &testStep);
                break;
            }
            case 2U:
            {
                /* Enable Real-Time data transfer */
                BleApp_SwitchRealTimeDataState(deviceId, gCccdIndication_c, &testStep);
                break;
            }
            case 4U:
            {
                testStep = 0U;
                break;
            }

            default:
            {
                ; /* No action required */
            }
            break;
        }
    }
    else if (strcmp(pArg, "RAP/REQ/RRD/BV-02-C") == 0)
    {
        static uint8_t testStep = 0U;

        switch(testStep)
        {
            case 0U: /* Fall-through */
            case 4U: /* Fall-through */
            case 8U: /* Fall-through */
            case 12U:
            {
                /* Disable Real-Time data transfer */
                BleApp_SwitchRealTimeDataState(deviceId, gCccdEmpty_c, &testStep);
                break;
            }
            case 1U:
            case 9U:
            {
                /* Enable Real-Time data transfer notification */
                BleApp_SwitchRealTimeDataState(deviceId, gCccdNotification_c, &testStep);
                break;
            }
            /* Round 1 cli commands:
             * filter 0 0x0028 0
             * filter 0 0x0028 1
             * filter 0 0x0031 0
             * filter 0 0x0031 1
             * filter 0 0x004E 0
             * filter 0 0x004E 1
             * filter 0 0x15A3 0
             * filter 0 0x15A3 1
             * 
             * Round 2 cli commands:
             * filter 0 0x0020 0
             * filter 0 0x0020 1
             * filter 0 0x0021 0
             * filter 0 0x0021 1
             * filter 0 0x000A 0
             * filter 0 0x000A 1
             * filter 0 0x0423 0
             * filter 0 0x0423 1
             */
            case 5U:
            case 13U:
            {
                /* Enable Real-Time data transfer indication */
                BleApp_SwitchRealTimeDataState(deviceId, gCccdIndication_c, &testStep);
                break;
            }
            case 2U: /* Fall-through */
            case 3U: /* Fall-through */
            case 6U: /* Fall-through */
            case 7U: /* Fall-through */
            case 10U: /* Fall-through */
            case 11U: /* Fall-through */
            case 14U: /* Fall-through */
            case 15U:
            {
                AppLocalization_RunPtsTest(deviceId, 101U, 0U);
                testStep++;
                break;
            }
            case 16U:
            {
                testStep = 0U;
                break;
            }

            default:
            {
                ; /* No action required */
            }
            break;
        }
    }
    else if (strcmp(pArg, "RAP/REQ/RRD/BI-01-C") == 0 ||
             strcmp(pArg, "RAP/REQ/RRD/BI-02-C") == 0)
    {
        AppLocalization_RunPtsTest(deviceId, 1U, 0U);
    }
    else if (strcmp(pArg, "RAP/REQ/ORD/BV-01-C") == 0 ||
             strcmp(pArg, "RAP/REQ/ORD/BV-03-C") == 0)
    {
        AppLocalization_RunPtsTest(deviceId, 101U, 0U);
    }
    else if (strcmp(pArg, "RAP/REQ/ORD/BV-02-C") == 0)
    {
        static uint8_t testStep = 0U;
        bleResult_t result = gBleSuccess_c;
        uint16_t handle = gGattDbInvalidHandle_d;

        switch(testStep)
        {
            case 0U: /* Fall-through */
            case 3U: /* Fall-through */
            case 6U: /* Fall-through */
            case 9U:
            {
                /* Disable On-Demand data transfer */
                handle = (uint16_t)(maPeerInformation[deviceId].rasConfigInfo.onDemandDataHandle + 1U);

                if (mpCharProcBuffer == NULL)
                {
                    mpCharProcBuffer = MEM_BufferAlloc(sizeof(gattAttribute_t) + gAttDefaultMtu_c);
                }

                if (mpCharProcBuffer != NULL)
                {
                    result = BleApp_ConfigureRasServer(deviceId, gCccdEmpty_c, handle);
                }

                if (result == gBleSuccess_c)
                {
                    testStep++;
                }
                break;
            }
            /* Round 1 cli commands:
             * filter 0 0x0028 0
             * filter 0 0x0028 1
             * filter 0 0x0031 0
             * filter 0 0x0031 1
             * filter 0 0x004E 0
             * filter 0 0x004E 1
             * filter 0 0x15A3 0
             * filter 0 0x15A3 1
             * 
             * Round 2 cli commands:
             * filter 0 0x0020 0
             * filter 0 0x0020 1
             * filter 0 0x0021 0
             * filter 0 0x0021 1
             * filter 0 0x000A 0
             * filter 0 0x000A 1
             * filter 0 0x0423 0
             * filter 0 0x0423 1
             */
            case 1U: /* Fall-through */
            case 2U: /* Fall-through */
            case 4U: /* Fall-through */
            case 5U: /* Fall-through */
            case 7U: /* Fall-through */
            case 8U: /* Fall-through */
            case 10U: /* Fall-through */
            case 11U:
            {
                AppLocalization_RunPtsTest(deviceId, 101U, 0U);
                testStep++;
                break;
            }
            case 12U:
            {
                testStep = 0U;
                break;
            }

            default:
            {
                ; /* No action required */
            }
            break;
        }
    }
    else if (strcmp(pArg, "RAP/REQ/ORD/BV-04-C") == 0)
    {
        static uint8_t testStep = 0U;

        switch(testStep)
        {
            case 0U:
            {
                AppLocalization_RunPtsTest(deviceId, 103U, 0U);
                testStep++;
                break;
            }
            case 1U:
            {
                AppLocalization_RunPtsTest(deviceId, 102U, 0U);
                testStep++;
                break;
            }
            case 2U:
            {
                testStep = 0U;
                break;
            }

            default:
            {
                ; /* No action required */
            }
            break;
        }
    }
    else if (strcmp(pArg, "RAP/REQ/ORD/BV-05-C") == 0)
    {
        uint16_t handle = gGattDbInvalidHandle_d;

        /* Enable Data Ready optional notifications */
        handle = (uint16_t)(maPeerInformation[deviceId].rasConfigInfo.dataReadyHandle + 1U);

        if (mpCharProcBuffer == NULL)
        {
            mpCharProcBuffer = MEM_BufferAlloc(sizeof(gattAttribute_t) + gAttDefaultMtu_c);
        }

        if (mpCharProcBuffer != NULL)
        {
            (void)BleApp_ConfigureRasServer(deviceId, gCccdNotification_c, handle);
        }
    }
    else if (strcmp(pArg, "RAP/REQ/ORD/BV-06-C") == 0)
    {
        static uint8_t testStep = 0U;
        bleResult_t result = gBleSuccess_c;
        uint16_t handle = gGattDbInvalidHandle_d;

        switch(testStep)
        {
            case 0U:
            {
                /* Enable Data Ready optional notifications */
                handle = (uint16_t)(maPeerInformation[deviceId].rasConfigInfo.dataReadyHandle + 1U);

                if (mpCharProcBuffer == NULL)
                {
                    mpCharProcBuffer = MEM_BufferAlloc(sizeof(gattAttribute_t) + gAttDefaultMtu_c);
                }

                if (mpCharProcBuffer != NULL)
                {
                    result = BleApp_ConfigureRasServer(deviceId, gCccdNotification_c, handle);
                }

                if (result == gBleSuccess_c)
                {
                    testStep++;
                }
                break;
            }
            case 1U:
            {
                /* Read Data Ready characteristic */
                mpRasCharacteristic.value.handle = maPeerInformation[deviceId].rasConfigInfo.dataReadyHandle;
                mpRasCharacteristic.value.uuidType = gBleUuidType16_c;
                mpRasCharacteristic.value.uuid.uuid16 = gBleSig_RasProcDataReady_d;
                mpRasCharacteristic.value.paValue = MEM_BufferAlloc(sizeof(uint32_t));
                if (mpRasCharacteristic.value.paValue != NULL)
                {
                    (void)GattClient_ReadCharacteristicValue(deviceId,
                                                            &mpRasCharacteristic,
                                                            (uint16_t)(sizeof(uint32_t)));
                }
                testStep++;
                break;
            }
            case 2U:
            {
                testStep = 0U;
                break;
            }

            default:
            {
                ; /* No action required */
            }
            break;
        }
    }
    else if (strcmp(pArg, "RAP/REQ/ORD/BV-08-C") == 0)
    {
        uint16_t handle = gGattDbInvalidHandle_d;

        /* Enable Data Overwritten optional notifications */
        handle = (uint16_t)(maPeerInformation[deviceId].rasConfigInfo.dataOverwrittenHandle + 1U);

        if (mpCharProcBuffer == NULL)
        {
            mpCharProcBuffer = MEM_BufferAlloc(sizeof(gattAttribute_t) + gAttDefaultMtu_c);
        }

        if (mpCharProcBuffer != NULL)
        {
            (void)BleApp_ConfigureRasServer(deviceId, gCccdNotification_c, handle);
        }
    }
    else if (strcmp(pArg, "RAP/REQ/ORD/BV-09-C") == 0)
    {
        static uint8_t testStep = 0U;
        bleResult_t result = gBleSuccess_c;
        uint16_t handle = gGattDbInvalidHandle_d;

        switch(testStep)
        {
            case 0U:
            {
                /* Enable Data Overwritten optional notifications */
                handle = (uint16_t)(maPeerInformation[deviceId].rasConfigInfo.dataOverwrittenHandle + 1U);

                if (mpCharProcBuffer == NULL)
                {
                    mpCharProcBuffer = MEM_BufferAlloc(sizeof(gattAttribute_t) + gAttDefaultMtu_c);
                }

                if (mpCharProcBuffer != NULL)
                {
                    result = BleApp_ConfigureRasServer(deviceId, gCccdNotification_c, handle);
                }

                if (result == gBleSuccess_c)
                {
                    testStep++;
                }
                break;
            }
            case 1U:
            {
                /* Read Data Overwritten characteristic */
                mpRasCharacteristic.value.handle = maPeerInformation[deviceId].rasConfigInfo.dataOverwrittenHandle;
                mpRasCharacteristic.value.uuidType = gBleUuidType16_c;
                mpRasCharacteristic.value.uuid.uuid16 = gBleSig_RasprocDataOverwritten_d;
                mpRasCharacteristic.value.paValue = MEM_BufferAlloc(sizeof(uint32_t));
                if (mpRasCharacteristic.value.paValue != NULL)
                {
                    (void)GattClient_ReadCharacteristicValue(deviceId,
                                                            &mpRasCharacteristic,
                                                            (uint16_t)(sizeof(uint32_t)));
                }
                testStep++;
                break;
            }
            case 2U:
            {
                testStep = 0U;
                break;
            }

            default:
            {
                ; /* No action required */
            }
            break;
        }
    }
    else if (strcmp(pArg, "RAP/REQ/ORD/BI-03-C") == 0)
    {
        AppLocalization_RunPtsTest(deviceId, 101U, 0U);
    }
    else
    {
        ; /* MISRA */
    }

    (void)MEM_BufferFree(pParam);
}
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

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
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
                    if (mbDeviceToConnectHasTAK == TRUE)
                    {
                        takEntry_t *pTakEntry = NULL;

                        maPeerInformation[peerDeviceId].appState = mAppEncryptLink_c;

                        pTakEntry = BleConnManager_GetTak(peerDeviceId, FALSE);
                        if (pTakEntry != NULL)
                        {
                            (void)Gap_EncryptLinkTak(peerDeviceId, pTakEntry->aTak);
                            
                            /* Clear Transient Key after usage */
                            FLib_MemSet(pTakEntry->aTak, 0, sizeof(pTakEntry->aTak));
                            
                            AppLocalization_SetTakEnable(peerDeviceId);
                        }
                    }
                    else
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */
                    {
                        maPeerInformation[peerDeviceId].appState = mAppPairing;
#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d != 1U)
#if gAppUsePairing_d
                        (void)Gap_Pair(peerDeviceId, &gPairingParameters);
#endif
#endif
                    }
                }
            }
        }
        break;

        case mAppPairing:
        {
            if (event == mAppEvt_PairingComplete_c)
            {
                shell_write("Pairing complete\n\r");
                /* Moving to Exchange MTU State */
                (void)GattClient_ExchangeMtu(peerDeviceId, gAttMaxMtu_c);
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
                (void)GattClient_ExchangeMtu(peerDeviceId, gAttMaxMtu_c);
                maPeerInformation[peerDeviceId].appState = mAppExchangeMtu_c;
            }
        }
        break;

        case mAppExchangeMtu_c:
        {
            if (event == mAppEvt_ExchangeMtuComplete_c)
            {
                /* Start service discovery for Ranging Service */
                bleUuid_t rasUuid;
                /* Moving to Localization Setup State*/
                shell_write("MTU Exchange complete\n\r");
                maPeerInformation[peerDeviceId].appState = mAppLocalizationSetup_c;
                rasUuid.uuid16 = gBleSig_RangingService_d;
                (void)BleServDisc_FindService(peerDeviceId, gBleUuidType16_c, &rasUuid);
            }
            else
            {
                if (event == mAppEvt_GattProcError_c)
                {
                    (void)Gap_Disconnect(peerDeviceId);
                }
            }
        }
        break;

        case mAppLocalizationSetup_c:
        {
            if (event == mAppEvt_ServiceDiscoveryComplete_c)
            {
                /* Read RAS features characteristic */
                mpRasCharacteristic.value.handle = maPeerInformation[peerDeviceId].rasConfigInfo.featuresHandle;
                mpRasCharacteristic.value.uuidType = gBleUuidType16_c;
                mpRasCharacteristic.value.uuid.uuid16 = gBleSig_RasFeature_d;
                mpRasCharacteristic.value.paValue = MEM_BufferAlloc(sizeof(uint32_t));
                if (mpRasCharacteristic.value.paValue != NULL)
                {
                    (void)GattClient_ReadCharacteristicValue(peerDeviceId,
                                                            &mpRasCharacteristic,
                                                            (uint16_t)(sizeof(uint32_t)));
                }
            }

            /* Wrote Filter command */
            else if (event == mAppEvt_WriteCharacteristicValueComplete_c)
            {
                if( mpCharProcBuffer == NULL )
                {
                    mpCharProcBuffer = MEM_BufferAlloc(sizeof(gattAttribute_t) + gAttDefaultMtu_c);
                }

                if( mpCharProcBuffer != NULL )
                {
                    BleApp_HandleRasSubscription(peerDeviceId);
                }
            }
            else if (event == mAppEvt_ReadCharacteristicValueComplete_c)
            {
                if (mpRasCharacteristic.value.handle == maPeerInformation[peerDeviceId].rasConfigInfo.featuresHandle)
                {
                    uint32_t rasFeatures = Utils_ExtractFourByteValue(mpRasCharacteristic.value.paValue);
                    RasClient_SetRasSupportedFeatures(peerDeviceId, rasFeatures);

                    if( mpCharProcBuffer == NULL )
                    {
                        mpCharProcBuffer = MEM_BufferAlloc(sizeof(gattAttribute_t) + gAttDefaultMtu_c);
                    }

                    if( mpCharProcBuffer != NULL )
                    {
                        BleApp_HandleRasSubscription(peerDeviceId);
                    }
                }
            }
            else if (event == mAppEvt_WriteCharacteristicDescriptorComplete_c)
            {
                if( mpCharProcBuffer == NULL )
                {
                    mpCharProcBuffer = MEM_BufferAlloc(sizeof(gattAttribute_t) + gAttDefaultMtu_c);
                }

                if( mpCharProcBuffer != NULL )
                {
                    BleApp_HandleRasSubscription(peerDeviceId);
                }
            }
            else if (event == mAppEvt_ServiceDiscoveryFailed_c)
            {
                (void)Gap_Disconnect(peerDeviceId);
            }
            else
            {
                /* For MISRA compliance */
            }
        }
        break;

        case mAppRunning_c:
        {
            if (event == mAppEvt_WriteCharacteristicDescriptorComplete_c)
            {
                shell_write("Channel Sounding configuration complete\n\r");
                (void)MEM_BufferFree(mpCharProcBuffer);
                mpCharProcBuffer = NULL;
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
    }
    /* Handle disconnect event in all application states. */
    if (event == mAppEvt_PeerDisconnected_c)
    {
        shell_write("Disconnected with reason ");
        shell_writeDec((uint32_t)maPeerInformation[peerDeviceId].disconReason);
        shell_write("\n\r");
        shell_cmd_finished();
        gFilterShellVal = (uint16_t)gNoFilter_c;
        maPeerInformation[peerDeviceId].deviceId = gInvalidDeviceId_c;
        maPeerInformation[peerDeviceId].appState = mAppIdle_c;
        maPeerInformation[peerDeviceId].isSubscribed = FALSE;
        AppLocalization_ResetPeer(peerDeviceId, TRUE, maPeerInformation[peerDeviceId].nvmIndex);
        AppLocalizationAlgo_ResetPeer(peerDeviceId);
    }
}

/*! *********************************************************************************
* \brief        Configures RAS server by writing to characteristic descriptor.
*
* \param[in]    peerDeviceId    Peer device ID.
* \param[in]    value           Value to write to descriptor.
* \param[in]    handle          Handle of the descriptor to write to.
*
* \return       bleResult_t     Result of the configuration operation.
********************************************************************************** */
static bleResult_t BleApp_ConfigureRasServer
(
    deviceId_t peerDeviceId,
    uint16_t value,
    uint16_t handle
)
{
    bleResult_t result = gBleSuccess_c;

    mpCharProcBuffer->handle = handle;
    mpCharProcBuffer->uuidType = gBleUuidType16_c;
    mpCharProcBuffer->uuid.uuid16 = gBleSig_CCCD_d;
    mpCharProcBuffer->valueLength = 1U;

    result = GattClient_WriteCharacteristicDescriptor(peerDeviceId,
                                                      mpCharProcBuffer,
                                                      (uint16_t)sizeof(value),
                                                      (void*)&value);

    return result;
}

/*! *********************************************************************************
* \brief        Handles RAS service subscription setup and configuration.
*
* \param[in]    peerDeviceId    Peer device ID.
********************************************************************************** */
static void BleApp_HandleRasSubscription
(
    deviceId_t peerDeviceId
)
{
    static uint16_t lastWrittenHandle = gGattDbInvalidHandle_d;
    uint16_t value = gRasUseNotifOrInd_c;
    bleResult_t result = gBleSuccess_c;
    bool_t filteringDone = TRUE;

    if (lastWrittenHandle == gGattDbInvalidHandle_d)
    {
        if (gFilterShellVal != (uint16_t)gNoFilter_c)
        {
            /* Set the filter and proceed as normal afterwards */
            result = RasClient_RasSetFilter(peerDeviceId, gFilterShellVal, TRUE, &filteringDone);
        }

        if ((result == gBleSuccess_c) && (filteringDone == TRUE))
        {
            gFilterShellVal = (uint16_t)gNoFilter_c;

            /* Write the RAS-CP */
            value = gCccdIndication_c;
            lastWrittenHandle = (uint16_t)(maPeerInformation[peerDeviceId].rasConfigInfo.controlPointHandle + 1U);
            result = BleApp_ConfigureRasServer(peerDeviceId, value, lastWrittenHandle);
        }
    }
#if defined(gAppRealTimeDataTransfer_d) && (gAppRealTimeDataTransfer_d == 1U)
    else if (lastWrittenHandle == (maPeerInformation[peerDeviceId].rasConfigInfo.controlPointHandle + 1U))
    {
        /* Enable Real-Time data transfer */
        lastWrittenHandle = (uint16_t)(maPeerInformation[peerDeviceId].rasConfigInfo.realTimeDataHandle + 1U);
        RasClient_SetRealTimePreference(peerDeviceId, TRUE);
        result = BleApp_ConfigureRasServer(peerDeviceId, value, lastWrittenHandle);
    }
#else
    else if (lastWrittenHandle == (maPeerInformation[peerDeviceId].rasConfigInfo.controlPointHandle + 1U))
    {
        value = gRasUseNotifOrInd_c;
        lastWrittenHandle = (uint16_t)(maPeerInformation[peerDeviceId].rasConfigInfo.dataReadyHandle + 1U);
        result = BleApp_ConfigureRasServer(peerDeviceId, value, lastWrittenHandle);
    }
    else if (lastWrittenHandle == (maPeerInformation[peerDeviceId].rasConfigInfo.dataReadyHandle + 1U))
    {
        value = gRasUseNotifOrInd_c;
        lastWrittenHandle = (uint16_t)(maPeerInformation[peerDeviceId].rasConfigInfo.dataOverwrittenHandle + 1U);
        result = BleApp_ConfigureRasServer(peerDeviceId, value, lastWrittenHandle);
    }
    else if (lastWrittenHandle == (maPeerInformation[peerDeviceId].rasConfigInfo.dataOverwrittenHandle + 1U))
    {
        lastWrittenHandle = (uint16_t)(maPeerInformation[peerDeviceId].rasConfigInfo.onDemandDataHandle + 1U);
        result = BleApp_ConfigureRasServer(peerDeviceId, value, lastWrittenHandle);
    }
#endif
    else
    {
        /* MISRA compliance */
    }

    if ((result == gBleSuccess_c) && ((lastWrittenHandle ==
        (maPeerInformation[peerDeviceId].rasConfigInfo.onDemandDataHandle + 1U)) ||
        (lastWrittenHandle == (maPeerInformation[peerDeviceId].rasConfigInfo.realTimeDataHandle + 1U))))
    {
         /* Begin localization procedure */
        maPeerInformation[peerDeviceId].appState = mAppRunning_c;
        maPeerInformation[peerDeviceId].isSubscribed = TRUE;
        lastWrittenHandle = gGattDbInvalidHandle_d;

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

    if (pGenericEvent->eventType == gLePhyEvent_c)
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
}

/*! *********************************************************************************
 * \brief        Configures BLE Stack after initialization
 *
 ********************************************************************************** */
static void BluetoothLEHost_Initialized(void)
{
    bleResult_t status = gBleSuccess_c;
    /* Common GAP configuration */
    BleConnManager_GapCommonConfig();

    status = App_RegisterGattClientProcedureCallback(BleApp_GattClientCallback);

    if (status == gBleSuccess_c)
    {
        status = App_RegisterGattClientNotificationCallback(BleApp_GattNotificationCallback);
    }

    if (status == gBleSuccess_c)
    {
        status = App_RegisterGattClientIndicationCallback(BleApp_GattIndicationCallback);
    }

    if (status == gBleSuccess_c)
    {
        BleServDisc_RegisterCallback(BleApp_ServiceDiscoveryCallback);

        status = AppLocalization_HostInitHandler();
    }

    if (status == gBleSuccess_c)
    {
        shell_write("\r\nLocalization Reader");
    }
    else
    {
        shell_write("\r\nInit error");
    }

    shell_cmd_finished();
}

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
            mAdvOn = !mAdvOn;
            if (mAdvOn == TRUE)
            {
                shell_write("Advertising started\r\n");

                 /* UI */
                 LedStopFlashingAllLeds();
                 Led1Flashing();
            }
            else
            {
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
#endif

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
    AppLocalization_ComputeSubeventLength(connInterval, &locConfig.minSubeventLen, &locConfig.maxSubeventLen);

    (void)AppLocalization_WriteConfig(peerDeviceId, &locConfig);
    AppLocalization_SetConnectionInterval(peerDeviceId, connInterval);
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
    AppLocalization_TimeInfoSetConnInterval(connInterval);
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */
}

/*! *********************************************************************************
* \brief        Handles BLE Connection callback from host stack.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    pConnectionEvent    Pointer to gapConnectionEvent_t.
********************************************************************************** */
void BleApp_ConnectionCallback (deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent)
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

            /* Advertising stops when connected */
#if defined(gAppIsPeripheral_d) && (gAppIsPeripheral_d == 1U)
            mAdvOn = FALSE;
#endif

            maPeerInformation[peerDeviceId].deviceId = peerDeviceId;
            maPeerInformation[peerDeviceId].isBonded = FALSE;
            maPeerInformation[peerDeviceId].nvmIndex = gInvalidNvmIndex_c;

            (void)Gap_CheckIfBonded(peerDeviceId, &maPeerInformation[peerDeviceId].isBonded, &maPeerInformation[peerDeviceId].nvmIndex);
            BleApp_StateMachineHandler(peerDeviceId, mAppEvt_PeerConnected_c);

            /* UI */
            LedStopFlashingAllLeds();

            /* UI */
            Led1On();

            /* Compute the CS config that depends on the connection interval */
            BleApp_UpdateLocalizationConfig(peerDeviceId, connInterval);

            /* Read PHY on which connection was established */
            (void)Gap_LeReadPhy(peerDeviceId);
        }
        break;

        case gConnEvtDisconnected_c:
        {
            maPeerInformation[peerDeviceId].disconReason = pConnectionEvent->eventData.disconnectedEvent.reason;

            BleApp_StateMachineHandler(peerDeviceId, mAppEvt_PeerDisconnected_c);

            LedStartFlashingAllLeds();
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

        case gConnEvtEncryptionChanged_c:
        {
            if( pConnectionEvent->eventData.encryptionChangedEvent.newEncryptionState )
            {
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
                if (mbDeviceToConnectHasTAK == TRUE)
                {
                    mbDeviceToConnectHasTAK = FALSE;
                    BleApp_StateMachineHandler(peerDeviceId, mAppEvt_EncryptionChanged_c);
                }
                else
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */
                if( mRestoringBondedLink )
                {
                    mRestoringBondedLink = FALSE;
                    BleApp_StateMachineHandler(peerDeviceId, mAppEvt_EncryptionChanged_c);
                }
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
        {
            ; /* No action required */
        }
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
#if gAppUsePairing_d
        attErrorCode_t attError = (attErrorCode_t)(uint8_t)(error);

        if (attError == gAttErrCodeInsufficientEncryption_c     ||
            attError == gAttErrCodeInsufficientAuthorization_c  ||
            attError == gAttErrCodeInsufficientAuthentication_c)
        {
            /* Start Pairing Procedure */
            (void)Gap_Pair(serverDeviceId, &gPairingParameters);
        }
#endif
        BleApp_StateMachineHandler(serverDeviceId, mAppEvt_GattProcError_c);
    }
    else
    {
        if (procedureResult == gGattProcSuccess_c)
        {
            switch(procedureType)
            {
                case gGattProcWriteCharacteristicDescriptor_c:
                {
                    BleApp_StateMachineHandler(serverDeviceId, mAppEvt_WriteCharacteristicDescriptorComplete_c);
                }
                break;

                case gGattProcReadCharacteristicValue_c:
                case gGattProcReadUsingCharacteristicUuid_c:
                {
                    BleApp_StateMachineHandler(serverDeviceId, mAppEvt_ReadCharacteristicValueComplete_c);
                }
                break;

                case gGattProcWriteCharacteristicValue_c:
                {
                    BleApp_StateMachineHandler(serverDeviceId, mAppEvt_WriteCharacteristicValueComplete_c);
                }
                break;

                case gGattProcExchangeMtu_c:
                {
                   BleApp_StateMachineHandler(serverDeviceId, mAppEvt_ExchangeMtuComplete_c);
                }
                break;

                default:
                {
                    ; /* No action required */
                }
                break;
            }
        }
    }

    /* Signal Service Discovery Module */
    BleServDisc_SignalGattClientEvent(serverDeviceId, procedureType, procedureResult, error);
}

/*! *********************************************************************************
* \brief        Handles GATT client notification callback from host stack.
*
* \param[in]    serverDeviceId              GATT Server device ID.
* \param[in]    characteristicValueHandle   Handle.
* \param[in]    aValue                      Pointer to value.
* \param[in]    valueLength                 Value length.
********************************************************************************** */
static void BleApp_GattNotificationCallback
(
    deviceId_t  serverDeviceId,
    uint16_t    characteristicValueHandle,
    uint8_t*    aValue,
    uint16_t    valueLength
)
{
    if ((characteristicValueHandle ==  maPeerInformation[serverDeviceId].rasConfigInfo.onDemandDataHandle) ||
        (characteristicValueHandle ==  maPeerInformation[serverDeviceId].rasConfigInfo.realTimeDataHandle))
    {
        /* Received localization data from peer */
        (void)RasClient_StorePeerMeasurementData(serverDeviceId, aValue, valueLength);
    }

    if (characteristicValueHandle == maPeerInformation[serverDeviceId].rasConfigInfo.controlPointHandle)
    {
        /* Received a command response from peer */
        (void)RasClient_ProcessRasCPRsp(serverDeviceId, aValue, valueLength);
    }

    if (characteristicValueHandle == maPeerInformation[serverDeviceId].rasConfigInfo.dataReadyHandle)
    {
        /* Received an indication from peer for the RAS control point characteristic */
        (void)RasClient_ProcessRasDataReadyIndications(serverDeviceId, aValue, valueLength);
    }

    if (characteristicValueHandle == maPeerInformation[serverDeviceId].rasConfigInfo.dataOverwrittenHandle)
    {
        /* Received an indication from peer for the RAS Data Overwritten characteristic */
        (void)RasClient_ProcessRasDataOverwrittenIndications(serverDeviceId, aValue, valueLength);
    }
}

/*! *********************************************************************************
* \brief        Handles GATT client indication callback from host stack.
*
* \param[in]    serverDeviceId              GATT Server device ID.
* \param[in]    characteristicValueHandle   Handle.
* \param[in]    aValue                      Pointer to value.
* \param[in]    valueLength                 Value length.
********************************************************************************** */
static void BleApp_GattIndicationCallback
(
    deviceId_t  serverDeviceId,
    uint16_t    characteristicValueHandle,
    uint8_t*    aValue,
    uint16_t    valueLength
)
{
    if (characteristicValueHandle == maPeerInformation[serverDeviceId].rasConfigInfo.controlPointHandle)
    {
        /* Received an indication from peer for the RAS control point characteristic */
        (void)RasClient_ProcessRasCPRsp(serverDeviceId, aValue, valueLength);
    }

    if (characteristicValueHandle == maPeerInformation[serverDeviceId].rasConfigInfo.dataReadyHandle)
    {
        /* Received an indication from peer for the RAS control point characteristic */
        (void)RasClient_ProcessRasDataReadyIndications(serverDeviceId, aValue, valueLength);
    }

    if (characteristicValueHandle == maPeerInformation[serverDeviceId].rasConfigInfo.dataOverwrittenHandle)
    {
        /* Received an indication from peer for the RAS Data Overwritten characteristic */
        (void)RasClient_ProcessRasDataOverwrittenIndications(serverDeviceId, aValue, valueLength);
    }

    if ((characteristicValueHandle ==  maPeerInformation[serverDeviceId].rasConfigInfo.onDemandDataHandle) ||
        (characteristicValueHandle ==  maPeerInformation[serverDeviceId].rasConfigInfo.realTimeDataHandle))
    {
        /* Received localization data from peer */
        (void)RasClient_StorePeerMeasurementData(serverDeviceId, aValue, valueLength);
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
                BleApp_StateMachineHandler(peerDeviceId, mAppEvt_ServiceDiscoveryComplete_c);
            }
            else
            {
                BleApp_StateMachineHandler(peerDeviceId, mAppEvt_ServiceDiscoveryFailed_c);
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
 * \brief        Stores handles used by the application.
 *
 * \param[in]    pService    Pointer to gattService_t.
 ********************************************************************************** */
static void BleApp_StoreServiceHandles
(
    deviceId_t peerDeviceId,
    gattService_t *pService
)
{
   if ((pService->uuidType == gBleUuidType16_c) &&
       (pService->uuid.uuid16 == gBleSig_RangingService_d))
   {
        /* Found Wireless Ranging Service */
        maPeerInformation[peerDeviceId].rasConfigInfo.serviceHandle = pService->startHandle;

        for (uint8_t i = 0; i < pService->cNumCharacteristics; i++)
        {
            if ((pService->aCharacteristics[i].value.uuidType == gBleUuidType16_c) &&
                (pService->aCharacteristics[i].value.uuid.uuid16 == gBleSig_RasControlPoint_d))
            {
                /* Found RAS Control Point Characteristic */
                maPeerInformation[peerDeviceId].rasConfigInfo.controlPointHandle =
                pService->aCharacteristics[i].value.handle;
                /* Register value with localization component */
                RasClient_SetRasControlPointHandle(peerDeviceId,
                                                         pService->aCharacteristics[i].value.handle);
            }

            if ((pService->aCharacteristics[i].value.uuidType == gBleUuidType16_c) &&
                (pService->aCharacteristics[i].value.uuid.uuid16 == gBleSig_RasProcDataReady_d))
            {
                /* Found RAS Ranging Data Ready Characteristic */
                maPeerInformation[peerDeviceId].rasConfigInfo.dataReadyHandle =
                pService->aCharacteristics[i].value.handle;
            }

            if ((pService->aCharacteristics[i].value.uuidType == gBleUuidType16_c) &&
                (pService->aCharacteristics[i].value.uuid.uuid16 == gBleSig_RasOnDemandProcData_d))
            {
                /* Found RAS Stored Ranging Data Characteristic */
                maPeerInformation[peerDeviceId].rasConfigInfo.onDemandDataHandle =
                pService->aCharacteristics[i].value.handle;
            }

            if ((pService->aCharacteristics[i].value.uuidType == gBleUuidType16_c) &&
                (pService->aCharacteristics[i].value.uuid.uuid16 == gBleSig_RasRealTimeProcData_d))
            {
                /* Found RAS Stored Ranging Data Characteristic */
                maPeerInformation[peerDeviceId].rasConfigInfo.realTimeDataHandle =
                pService->aCharacteristics[i].value.handle;
                RasClient_SetRasRealTimeHandle(peerDeviceId,
                                                     pService->aCharacteristics[i].value.handle);
            }

            if ((pService->aCharacteristics[i].value.uuidType == gBleUuidType16_c) &&
                (pService->aCharacteristics[i].value.uuid.uuid16 == gBleSig_RasprocDataOverwritten_d))
            {
                /* Found RAS Procedure Data Overwritten Characteristic */
                maPeerInformation[peerDeviceId].rasConfigInfo.dataOverwrittenHandle =
                pService->aCharacteristics[i].value.handle;
            }

            if ((pService->aCharacteristics[i].value.uuidType == gBleUuidType16_c) &&
                (pService->aCharacteristics[i].value.uuid.uuid16 == gBleSig_RasFeature_d))
            {
                /* Found RAS Features Characteristic */
                maPeerInformation[peerDeviceId].rasConfigInfo.featuresHandle =
                pService->aCharacteristics[i].value.handle;
            }
        }
   }
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
                shell_writeDec((uint32_t)pEvent->eventData.csCommandError.errorSource); /* value in commandErrorSource_t enum */
                shell_write(", status: 0x ");
                shell_writeHexLe((uint8_t*)&pEvent->eventData.csCommandError.status, 2); /* value in bleResult_t enum */
                shell_write("\r\n");
            }
        }
        break;

        case gCsStatusEvent_c:
        {
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
                    shell_write("\r\nSet Procedure parameters failed.\r\n");
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
            shell_write("\r\nDistance measurement complete. Local data available.\r\n");
        }
        break;

        case gDataOverwritten_c:
        {
            shell_write("Received Data Overwritten Indication! Clearing local data....\r\n");
        }
        break;

        case gErrRasOpCodeNotSupported_c:
        {
            shell_write("\r\nRAS error op code not supported!\r\n");
        }
        break;

        case gErrRasInvalidParameter_c:
        {
            shell_write("\r\nRAS error invalid parameter!\r\n");
        }
        break;

        case gErrRasParameterNotSupported_c:
        {
            shell_write("\r\nRAS error parameter not supported!\r\n");
        }
        break;

        case gErrRasAbortUnsuccessful_c:
        {
            shell_write("\r\nRAS error abort unsuccessul!\r\n");
        }
        break;

        case gErrRasProcedureNotCompleted_c:
        {
            shell_write("\r\nRAS error procedure not completed!\r\n");
        }
        break;

        case gErrRasServerBusy_c:
        {
            shell_write("\r\nRAS error server busy!\r\n");
        }
        break;

        case gErrRasNoRecordsFound_c:
        {
            shell_write("\r\nRAS error no records found!\r\n");
        }
        break;

        case gErrorEvent_c:
        {
            if (pData != NULL)
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

                    case gAppLclUnexpectedSDS_c:
                    {
                        shell_write("Received an unexpected Set Default Settings Event!\r\n");
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

                    case gAppLclInvalidDeviceId_c:
                    {
                        shell_write("Received an invalid device Id!\r\n");
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

                    case gAppLclAlgoNotRun_c:
                    {
                        shell_write("\r\nAlgorithm did not run, procedure likely failed on peer.\r\n");
                    }
                    break;

                    case gAppLclStartMeasurementFail_c:
                    {
                        shell_write("Start measurement failed!\r\n");
                    }
                    break;

                    case gAppLclProcStatusFailed_c:
                    {
                        shell_write("Procedure done status error received!\r\n");
                    }
                    break;

                    case gAppLclProcedureAborted_c:
                    {
                        shell_write("All subsequent CS procedures aborted!\r\n");
                    }
                    break;

                    case gAppLclRasTransferFailed_c:
                    {
                        shell_write("RAS - Received an error response from RAS server!\r\n");
                    }
                    break;

                    case gAppLclInvalidProcCounter_c:
                    {
                        shell_write("RAS - Received an invalid procedure index!\r\n");
                    }
                    break;

                    case gAppLclInvalidProcIndex_c:
                    {
                        shell_write("RAS - Received a data ready indication for a procedure index different from the local one!\r\n");
                    }
                    break;

                    case gAppLclInvalidSegmentCounter_c:
                    {
                        shell_write("RAS - Received an invalid segment counter in data notification!\r\n");
                    }
                    break;

                    case gAppLclSubeventStatusFailed_c:
                    {
                        shell_write("Subevent status failed!\r\n");
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

                    case gAppLclAlgoNotRunNoDataReady_c:
                    {
                        shell_write("Algorithm did not run - No Data Ready from peer!\r\n");
                    }
                    break;

                    case gAppLclAlgoNotRunNoRangingData_c:
                    {
                        shell_write("Algorithm did not run - Ranging Data not complete!\r\n");
                    }
                    break;

                    case gAppLclAlgoNotRunNoRealTimeData_c:
                    {
                        shell_write("Algorithm did not run - Real Time Ranging Data not complete!\r\n");
                        maPeerInformation[deviceId].isSubscribed = FALSE;
                    }
                    break;

                    case gAppLclMaxProceduresReached_c:
                    {
                        shell_write("Maximum concurrent CS procedures reached!\r\n");
                    }
                    break;

                    default:
                    {
                        shell_write("Unknown error!\r\n");
                    }
                    break;
                }
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
                    shell_write("Unknown! Abort reason: 0x");
                    shell_writeHex(&abortReason, (uint8_t)sizeof(uint8_t));
                    shell_write("\r\n");
                }
                break;
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
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
    bool_t bHasTAK = FALSE;
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */
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
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
        else if (foundMatch == TRUE && adElement.adType == gAdManufacturerSpecificData_c)
        {
            if (FLib_MemCmp(gAppTAKAdvID_c, adElement.aData, adElement.length))
            {
                bHasTAK = TRUE;
            }
        }
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */
        /* Move on to the next AD element type */
        index += (uint32_t)adElement.length + sizeof(uint8_t);
    }

    if (foundMatch)
    {
        /* Update UI */
        shell_write("Legacy ADV: ");
        shell_writeHexLe(pData->aAddress, gcBleDeviceAddressSize_c);
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
        if (bHasTAK == TRUE)
        {
            shell_write(" (TAK)");
            mbDeviceToConnectHasTAK = TRUE;
        }
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */
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
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
    bool_t bHasTAK = FALSE;
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */
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
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
        else if (foundMatch == TRUE && adElement.adType == gAdManufacturerSpecificData_c)
        {
            if (FLib_MemCmp(gAppTAKAdvID_c, adElement.aData, adElement.length))
            {
                bHasTAK = TRUE;
            }
        }
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */

        /* Move on to the next AD element type */
        index += (uint32_t)adElement.length + sizeof(uint8_t);
    }

    if (foundMatch)
    {
        /* Update UI */
        shell_write("Extended LR ADV: ");
        shell_writeHexLe(pData->aAddress, gcBleDeviceAddressSize_c);
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
        if (bHasTAK == TRUE)
        {
            shell_write(" (TAK)");
            mbDeviceToConnectHasTAK = TRUE;
        }
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */
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
#endif /* !gAppIsPeripheral_d */

#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
/*! *********************************************************************************
* \brief        Run commands to test PTS.
*
********************************************************************************** */
static void BleApp_SwitchRealTimeDataState(deviceId_t deviceId, uint16_t value, uint8_t *testStep)
{
    /* Real-Time data transfer handle */
    uint16_t handle = (uint16_t)(maPeerInformation[deviceId].rasConfigInfo.realTimeDataHandle + 1U);
    bleResult_t result = gBleSuccess_c;

    /* Set local Real Time Preference state */
    if (value == gCccdEmpty_c)
    {
        RasClient_SetRealTimePreference(deviceId, FALSE);
    }
    else
    {
        RasClient_SetRealTimePreference(deviceId, TRUE);
    }

    if (mpCharProcBuffer == NULL)
    {
        mpCharProcBuffer = MEM_BufferAlloc(sizeof(gattAttribute_t) + gAttDefaultMtu_c);
    }

    /* Set the CCCD on the remote */
    if (mpCharProcBuffer != NULL)
    {
        result = BleApp_ConfigureRasServer(deviceId, value, handle);
    }
    else
    {
        result = gBleOutOfMemory_c;
    }

    if (result == gBleSuccess_c)
    {
        (*testStep)++;
    }
}
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */
