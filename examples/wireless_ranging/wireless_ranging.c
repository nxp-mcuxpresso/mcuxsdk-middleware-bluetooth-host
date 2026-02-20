/*! *********************************************************************************
 * \addtogroup Wireless Ranging Application
 * @{
 ********************************************************************************** */
/*
 * Copyright 2020-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include <stdio.h>

/* Framework / Drivers */
#include "EmbeddedTypes.h"
#include "RNG_Interface.h"
#include "fsl_port.h"
#include "fsl_component_button.h"
#include "fsl_component_led.h"
#include "fsl_component_timer_manager.h"
#include "fsl_component_serial_manager.h"
#include "fsl_component_mem_manager.h"
#include "fsl_adapter_reset.h"
#include "app.h"
#include "board_comp.h"
#include "FunctionLib.h"
#include "board.h"
#include "sensors.h"
#include "fwk_platform_ble.h"
#include "fwk_platform_lcl.h"

/* BLE Host Stack */
#include "gatt_interface.h"
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gatt_database.h"
#include "gap_interface.h"
#include "gatt_db_app_interface.h"

#if !defined(MULTICORE_APPLICATION_CORE) || (!MULTICORE_APPLICATION_CORE)
#include "gatt_db_handles.h"
#else
#define UUID128(name, ...)\
    extern uint8_t name[16];
#include "gatt_uuid128.h"
#undef UUID128
#endif

/* Wrappers */
#include "ble_conn_manager.h"
#include "ble_service_discovery.h"

#include "board.h"
#include "pin_mux.h"
#include "app_conn.h"
#include "app_scanner.h"
#include "app_advertiser.h"
#include "app_version.h"
#include "wireless_ranging.h"
#include "wireless_ranging_service.h"
#include "wireless_ranging_cli.h"
#include "isp_debug_helpers.h"
#include "isp_ranging_engine.h"
#include "hci_transport.h"

#ifdef LCE_KW47_MCXW72
#include "app_lce_init.h"
#endif /* LCE_KW47_MCXW72 */

#include "pde_rade.h"

/************************************************************************************
 *************************************************************************************
 * Private macros
 *************************************************************************************
 ************************************************************************************/
#define mAppMtu_c                       gAttMaxWriteDataSize_d(gAttMaxMtu_c) /* Local Buffer Size */

#define Serial_Print(a,b,c)             SerialManager_WriteBlocking((serial_write_handle_t)g_ispCliWriteHandle, \
                                                                    (uint8_t *)b, strlen(b))
#define Serial_PrintDec(a,b)            SerialManager_WriteBlocking((serial_write_handle_t)g_ispCliWriteHandle, \
                                                                    FORMAT_Dec2Str(b), strlen((char const*)FORMAT_Dec2Str(b)))
#define WR_MIN_SUPERVISION_TIMEOUT      (1000U)  /*[10ms] = 10s*/

/************************************************************************************
 *************************************************************************************
 * Private type definitions
 *************************************************************************************
 ************************************************************************************/
/*! Application Event Types */
typedef enum appEvent_tag
{
    mAppEvt_PeerConnected_c,
    mAppEvt_PeerDisconnected_c,
    mAppEvt_PairingComplete_c,
    mAppEvt_ServiceDiscoveryComplete_c,
    mAppEvt_ServiceDiscoveryNotFound_c,
    mAppEvt_ServiceDiscoveryFailed_c,
    mAppEvt_GattProcComplete_c,
    mAppEvt_GattProcError_c,
    mAppEvt_Timeout_c
} appEvent_t;

/*! Application States */
typedef enum appState_tag
{
    mAppIdle_c,
    mAppExchangeMtu_c,
    mAppServiceDisc_c,
    mAppServiceDiscRetry_c,
    mAppRunning_c
} appState_t;

/*! Wireless Ranging Service Client - Configuration */
typedef struct wrConfig_tag
{
    uint16_t    hService;
    uint16_t    hCtrlStream;
} wrSvcConfig_t;

/*! Peer information - application level */
typedef struct appPeerInfo_tag
{
    deviceId_t         deviceId;
    bool_t             isBonded;
    wrSvcConfig_t      clientInfo;
    appState_t         appState;
    gapRole_t          gapRole;
    bleDeviceAddress_t address;
} appPeerInfo_t;

/************************************************************************************
 *************************************************************************************
 * Public memory declarations
 *************************************************************************************
 ************************************************************************************/
uint16_t mAppMtu = mAppMtu_c;
const uint8_t addressAllZeros[gcBleDeviceAddressSize_c] = {0,0,0,0,0,0};

/************************************************************************************
 *************************************************************************************
 * Private memory declarations
 *************************************************************************************
 ************************************************************************************/
/* Scanning parameters */
static appScanningParams_t mAppScanParams = {
    &gScanParams,
    gGapDuplicateFilteringEnable_c,
    gGapScanContinuously_d,
    gGapScanPeriodicDisabled_d
};
/* Advertising parameters */
static appAdvertisingParams_t mAppAdvParams = {
    &gAdvParams,
    &gAppAdvertisingData,
    &gAppScanRspData
};
/* Peer information needed by the application */
static appPeerInfo_t maPeerInformation[gLclMaxConnections_c];
bleInfo_t bleInfo;

/*
 * Each Wireless Ranging device acts both as GATT client and GATT server.
 * The Server offers a control stream used to send/receive control packets (writable attribute).
 * Packet content is opaque to GATT profile.
 */
static uint16_t mCharMonitoredHandles[1] = { (uint16_t)value_wr_ctrl_stream };

/* Timer to update information */
static TIMER_MANAGER_HANDLE_DEFINE(mBleInfoTimerId);

#if (defined(gAppLedCnt_c) && (gAppLedCnt_c > 0))
/* LED configurations */
static led_flash_config_t gLedFlashSlow = {
    .times     = LED_FLASH_CYCLE_FOREVER,
    .period    = 2000,
    .flashType = kLED_FlashOneColor,
    .duty      = 50
};
static led_flash_config_t gLedFlashFast = {
    .times     = LED_FLASH_CYCLE_FOREVER,
    .period    = 200,
    .flashType = kLED_FlashOneColor,
    .duty      = 50
};
#endif

/************************************************************************************
 *************************************************************************************
 * Private functions prototypes
 *************************************************************************************
 ************************************************************************************/
/* GAP callbacks */
static void BleApp_AdvertisingCallback
(
    gapAdvertisingEvent_t *pAdvertisingEvent
);
static void BleApp_ScanningCallback
(
    gapScanningEvent_t *pScanningEvent
);
static void BleApp_ConnectionCallback
(
    deviceId_t peerDeviceId,
    gapConnectionEvent_t *pConnectionEvent
);

/* GATT callbacks */
static void BleApp_GattServerCallback
(
    deviceId_t deviceId,
    gattServerEvent_t *pServerEvent
);
static void BleApp_GattClientCallback
(
    deviceId_t              serverDeviceId,
    gattProcedureType_t     procedureType,
    gattProcedureResult_t   procedureResult,
    bleResult_t             error
);
static void BleApp_ServiceDiscoveryCallback
(
    deviceId_t      peerDeviceId,
    servDiscEvent_t *pEvent
);

/* Channel Sounding callbacks */
static bleResult_t BleApp_CsMetaEventCallback
(
    csMetaEvent_t* pPacket
);
static bleResult_t BleApp_CsEventCmdCompleteCallback
(
    csCommandCompleteEvent_t* pPacket
);
static bleResult_t BleApp_CsEventCmdStatusCallback
(
    csCommandStatusEvent_t* pPacket
);

/* Initialization complete callback */
static void BluetoothLEHost_Initialized(void);
/* Generic callback */
static void BluetoothLEHost_GenericCallback(gapGenericEvent_t *pGenericEvent);

/* State machine handler */
static void BleApp_StateMachineHandler
(
    deviceId_t peerDeviceId,
    appEvent_t event
);

/* Helper functions */
static void BleApp_StoreServiceHandles
(
    deviceId_t       peerDeviceId,
    gattService_t   *pService
);
static bool_t BleApp_CheckScanEvent(gapScannedDevice_t *pData);

static void BleInfo_Disconnect(bool_t hardreset);
static bool_t BleInfo_CompareConfiguration(void);
static bool_t BleInfo_CheckAddress (uint8_t* pAddress);
static void BleApp_Reset(void);

/* Timer Callbacks */
static void BleInfo_TimerCallback(void * pParam);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
#if (defined(gAppLedCnt_c) && (gAppLedCnt_c > 0))
/*! *********************************************************************************
 * \brief        Updates the LEDs according to the given state
 *
 * \param[in]    state    New LED state
 ********************************************************************************** */
void BleApp_SetLED(bleLedStates_t state)
{
    /* Re-configure pinmux for LEDs if required (we have a conflict with RF_GPO) */
    if (state != mLedOff)
    {
        BOARD_InitPinLED1();
#if (defined(gAppLedCnt_c) && (gAppLedCnt_c > 1U))
        BOARD_InitPinLED2();
#endif
    }
#if defined(gBoardLedRgbHdl)
    (void)BOARD_RGB_LED_SET_COLOR((led_color_t)kLED_Red);
#endif
    (void)BOARD_RGB_LED_ACTUATE(0);
    (void)BOARD_MONOCHROME_LED_ACTUATE(0);

    switch(state)
    {
    case mLedOff:
        break;

    case mLedConnecting:
        /* fast flash */
        if(bleInfo.role == gGapPeripheral_c)
        {
            (void)BOARD_MONOCHROME_LED_FLASH(&gLedFlashFast);
        }
        else
        {
            (void)BOARD_RGB_LED_FLASH(&gLedFlashFast);
        }
        break;

    case mLedConnected:
        /* no flash */
        if(bleInfo.role == gGapPeripheral_c)
        {
            (void)BOARD_MONOCHROME_LED_ACTUATE(1);
        }
        else
        {
            (void)BOARD_RGB_LED_ACTUATE(1);
        }
        break;

    default:
        /* slow flash */
        (void)BOARD_RGB_LED_ACTUATE(1);
        (void)BOARD_RGB_LED_FLASH(&gLedFlashSlow);
        (void)BOARD_MONOCHROME_LED_ACTUATE(1);
        (void)BOARD_MONOCHROME_LED_FLASH(&gLedFlashSlow);
        break;
    }
}
#endif

/*! *********************************************************************************
* \brief  This is the initialization function for each application. This function
*         should contain all the initialization code required by the bluetooth demo
********************************************************************************** */
void BluetoothLEHost_AppInit(void)
{
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[0], BleApp_HandleKeys0, NULL);

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1))
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[1], BleApp_HandleKeys1, NULL);
#endif
#endif

    /* Initialize Bluetooth Host Stack */
    BluetoothLEHost_SetGenericCallback(BluetoothLEHost_GenericCallback);
    BluetoothLEHost_Init(BluetoothLEHost_Initialized);

    isp_globals_init();
    isp_soc_debug_init();
    isp_cli_powerup();
    wrs_ConnDataInit(CS_TEST_DEVICEID, 0);

#ifdef LCE_KW47_MCXW72
    (void)APP_InitLce();
#endif
}

/*! *********************************************************************************
 * \brief        Send a UART stream to the peer identified by the given deviceId.
 *
 * \param[in]    deviceId       Peer identifier
 * \param[in]    pRecvStream    Data to be sent to the peer
 * \param[in]    streamSize     Data length
 ********************************************************************************** */
bleResult_t BleApp_SendMessage(uint8_t deviceId, uint8_t *pRecvStream, uint16_t streamSize)
{
    bleResult_t  result = gBleInvalidParameter_c;
    gattCharacteristic_t characteristic = {(uint8_t)gGattCharPropNone_c, {0}, 0, 0};

    if(gInvalidDeviceId_c != maPeerInformation[deviceId].deviceId &&
            (mAppRunning_c == maPeerInformation[deviceId].appState))
    {
        characteristic.value.handle = maPeerInformation[deviceId].clientInfo.hCtrlStream;
        result = GattClient_WriteCharacteristicValue(
                    deviceId, &characteristic, streamSize, pRecvStream, TRUE, FALSE, FALSE, NULL);
    }
    return result;
}

/*! *********************************************************************************
 * \brief        Returns the device id of the peer corresponding to the given address
 *
 * \param[in]    address       Peer address
 ********************************************************************************** */
deviceId_t BleApp_address_2_device_id(uint8_t *address)
{
    uint32_t i;
    uint8_t deviceId = gInvalidDeviceId_c;

    for (i = 0; i < bleInfo.max_connections; i++)
    {
        if (FLib_MemCmp(address, (uint8_t*)maPeerInformation[i].address, gcBleDeviceAddressSize_c))
        {
            deviceId = (uint8_t)i;
            break;
        }
    }
    return deviceId;
}

/*! *********************************************************************************
 * \brief        Return number of connections in Running state
 *
 * \param[in]    andActive      TRUE if the connections should be active
 ********************************************************************************** */
uint8_t BleInfo_GetConnections(bool_t andActive)
{
    uint8_t mPeerId;
    uint8_t no = 0;

    if (!BleInfo_CompareConfiguration())
    {
        return no;
    }

    for (mPeerId = 0; mPeerId < (uint8_t)gLclMaxConnections_c; mPeerId++)
    {
        if (maPeerInformation[mPeerId].deviceId == gInvalidDeviceId_c)
        {
            continue;
        }

        if (maPeerInformation[mPeerId].appState != mAppRunning_c && andActive)
        {
            continue;
        }

        no++;;
    }
    return no;
}

/*! *********************************************************************************
 * \brief        Return TRUE if local device is in a connection
 ********************************************************************************** */
bool_t BleInfo_IsConnected(void)
{
    /* Forced update of state */
    union
    {
        void* pV;
        uint32_t number;
    }param = {.number = 1U};
    BleInfo_TimerCallback(param.pV);
    return (bleInfo.state == mConnConnected)? TRUE:FALSE;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief        Configures BLE Stack after initialization. Usually used for
*               configuring advertising, scanning, white list, services, et al.
*
********************************************************************************** */
static void BluetoothLEHost_Initialized(void)
{
    uint8_t mPeerId = 0;

    {
        uint8_t status = PLATFORM_InitLcl();
        assert(status == 0U);
        (void)status;
    }

    /* Vendor-specific config via HCI - must be done after PLATFORM_InitLcl */
    measurement_powerup(gRangeSettings);

    /* Common GAP configuration */
    BleConnManager_GapCommonConfig();

    /* Initialize channel sounding */
    CS_Init();
    (void)CS_RegisterCsMetaEventCallback(BleApp_CsMetaEventCallback);
    (void)CS_RegisterCsEventCallback(BleApp_CsEventCmdCompleteCallback);
    (void)CS_RegisterCmdStatusEventCallback(BleApp_CsEventCmdStatusCallback);

    /* Register for callbacks */
    (void)App_RegisterGattServerCallback(BleApp_GattServerCallback);
    (void)App_RegisterGattClientProcedureCallback(BleApp_GattClientCallback);
    (void)GattServer_RegisterHandlesForWriteNotifications(NumberOfElements(mCharMonitoredHandles), mCharMonitoredHandles);
    BleServDisc_RegisterCallback(BleApp_ServiceDiscoveryCallback);

    for (mPeerId = 0; mPeerId < (uint8_t)gLclMaxConnections_c; mPeerId++)
    {
        maPeerInformation[mPeerId].appState = mAppIdle_c;
        maPeerInformation[mPeerId].deviceId = gInvalidDeviceId_c;
        maPeerInformation[mPeerId].clientInfo.hService = gGattDbInvalidHandleIndex_d;
        maPeerInformation[mPeerId].clientInfo.hCtrlStream = gGattDbInvalidHandleIndex_d;
    }

    /* Allocate application timer */
    (void)TM_Open((timer_handle_t)mBleInfoTimerId);

    bleInfo.advOn = FALSE;
    bleInfo.scnOn = FALSE;
    bleInfo.foundDeviceToConnect = FALSE;
    bleInfo.state = mConnIdle;
    bleInfo.ledstate = mLedIdle;
    bleInfo.retries = 0;

#if (defined(gAppLedCnt_c) && (gAppLedCnt_c > 0))
    BleApp_SetLED(mLedIdle);
#endif /*gAppLedCnt_c > 0*/

    PLATFORM_GetBDAddr(local_bid.uid);

    FLib_MemCpyAligned32bit(&local_bid.ver, &c_app_version, sizeof(c_app_version));

    /* Start bleInfo timer */
    (void)TM_InstallCallback((timer_handle_t)mBleInfoTimerId, BleInfo_TimerCallback, NULL);
    (void)TM_Start((timer_handle_t)mBleInfoTimerId, kTimerModeIntervalTimer | kTimerModeLowPowerTimer, gBleInfoRefreshTime_c /*Pworkaround TMR issue*/);
}

/*! *********************************************************************************
 * \brief        Handles LE Meta Events from the channel sounding lib
 *
 * \param[in]    pPacket    Pointer to csMetaEvent_t.
 ********************************************************************************** */
static bleResult_t BleApp_CsMetaEventCallback
(
    csMetaEvent_t* pPacket
)
{
    bleResult_t result = gBleSuccess_c;
    isp_soc_eventlog(0xAA000000U | (uint32_t)pPacket->eventType);

    switch (pPacket->eventType)
    {
        case csConfigComplete_c:
        {
            processCsConfigCompleteEvent(&pPacket->eventData.csConfigComplete);
        }
        break;

        case csSubeventResultEvent_c:
        {
            DEBUG_PIN0_PLS
            wrs_ProcessResultEvents(pPacket->eventData.csSubeventResultEvent.deviceId,
                                    &pPacket->eventData.csSubeventResultEvent);

            /* Free Event Data */
            (void)MEM_BufferFree(pPacket->eventData.csSubeventResultEvent.pData);
        }
        break;

        case csSubeventResultContinueEvent_c:
        {
            uint8_t *eventData = (uint8_t *)pPacket->eventData.csSubeventResultContinueEvent.pData;

            DEBUG_PIN0_PLS
            processCsEventResultContinueData(&pPacket->eventData.csSubeventResultContinueEvent,
                                               eventData);
            /* Only status 0x01 means continue */
            if (pPacket->eventData.csSubeventResultContinueEvent.procedureDoneStatus != 0x01U)
            {
                DEBUG_PIN0_PLS
                notifyEndOfEventResult(pPacket->eventData.csSubeventResultContinueEvent.deviceId,
                                       pPacket->eventData.csSubeventResultContinueEvent.procedureDoneStatus);
            }
            (void)MEM_BufferFree(pPacket->eventData.csSubeventResultContinueEvent.pData);
        }
        break;

        case csSecurityEnableComplete_c:
        {
            processSecurityEnableCompleteEvent(&pPacket->eventData.csSecurityEnableComplete);
        }
        break;

        case csReadRemoteSupportedCapabilitiesComplete_c:
        {
            processReadRemoteSupportedCapabilitiesCompleteEvent(&pPacket->eventData.csReadRemoteSupportedCapabilitiesComplete);
        }
        break;

        case csTestEnd_c:
        {
            DPRINTF_DBG("CS Test Ended. Status=%d.\n", pPacket->eventData.csTestEndEvent.status);
        }
        break;

#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
        case csEventResultDebug_c:
        {
            DEBUG_PIN0_PLS
            processCsEventResultDebugEvent(&pPacket->eventData.csEventResultDebugEvent);
            (void)MEM_BufferFree(pPacket->eventData.csEventResultDebugEvent.pData);
        }
        break;
#endif

        case csSetProcedureParametersComplete_c:
        {
            ; /* untreated event - ignore */
        }
        break;

        case csProcedureEnableCompleteEvent_c:
        {
            wrs_CSDataInit(pPacket->eventData.csProcedureEnableCompleteEvent.deviceId,
                           &pPacket->eventData.csProcedureEnableCompleteEvent);
        }
        break;

        case csError_c:
        {
            DPRINTF("An Error occurred! error source: [%d], error code: [0x%04X]\n",
                     pPacket->eventData.csError.csErrorSource,
                     pPacket->eventData.csError.status);
        }
        break;

        default:
        {
            DPRINTF("Unknown CS Event: 0x%04X\n", pPacket->eventType);
        }
        break;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles Command Complete Events from the channel sounding lib
 *
 * \param[in]    pPacket    Pointer to csCommandCompleteEvent_t.
 ********************************************************************************** */
static bleResult_t BleApp_CsEventCmdCompleteCallback
(
    csCommandCompleteEvent_t* pPacket
)
{
    bleResult_t result = gBleSuccess_c;

    switch (pPacket->eventType)
    {
        case readLocalSupportedCapabilities_c:
        {
            processReadLocalSupportedCapabilitiesCompleteEvent(pPacket);
        }
        break;

        case setDefaultSettings_c:
        {
            processSetDefaultSettingsCompleteEvent(pPacket);
        }
        break;

        case commandError_c:
        {
            DPRINTF_DBG("A command error occurred! error source: [%d], error code: [0x%04X]\n",
                        pPacket->eventData.csCommandError.errorSource,
                        pPacket->eventData.csCommandError.status);
            wrs_NotifyHCICommandError(pPacket->deviceId, 
                                      (uint32_t)(pPacket->eventData.csCommandError.errorSource), 
                                      (uint32_t)(pPacket->eventData.csCommandError.status));
        }
        break;

        default:
        {
            ; /* No action required */
            break;
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles Command Status Events from the channel sounding lib
 *
 * \param[in]    pPacket    Pointer to csCommandStatusEvent_t.
 ********************************************************************************** */
static bleResult_t BleApp_CsEventCmdStatusCallback
(
    csCommandStatusEvent_t* pPacket
)
{
    if (pPacket->status != gBleSuccess_c)
    {
        DPRINTF_DBG("A command status error was received! code: [%d] event: [%d]\n",
                    pPacket->status,
                    pPacket->eventType);
        wrs_NotifyHCICommandError(gInvalidDeviceId_c, 
                                  (uint32_t)(pPacket->eventType), 
                                  (uint32_t)(pPacket->status));
    }

    return gBleSuccess_c;
}

/*! *********************************************************************************
 * \brief        Handles BLE Scanning callback from host stack.
 *
 * \param[in]    pScanningEvent    Pointer to gapScanningEvent_t.
 ********************************************************************************** */
static void BleApp_ScanningCallback(gapScanningEvent_t *pScanningEvent)
{
    switch (pScanningEvent->eventType)
    {
        case gDeviceScanned_c:
        {
            if (bleInfo.foundDeviceToConnect == FALSE)
            {
                CLI_PRINTF_INFO(">Scanned 0x%02X%02X%02X%02X%02X%02X",
                                pScanningEvent->eventData.scannedDevice.aAddress[5],
                                pScanningEvent->eventData.scannedDevice.aAddress[4],
                                pScanningEvent->eventData.scannedDevice.aAddress[3],
                                pScanningEvent->eventData.scannedDevice.aAddress[2],
                                pScanningEvent->eventData.scannedDevice.aAddress[1],
                                pScanningEvent->eventData.scannedDevice.aAddress[0]);

                /* Check that peer device uuid corresponds to NXP_WR and has expected address (if specified) */
                if ((!BleApp_CheckScanEvent(&pScanningEvent->eventData.scannedDevice) ||
                    ((gCommunicationSettings->address_list_len != 0U) &&
                        !BleInfo_CheckAddress(pScanningEvent->eventData.scannedDevice.aAddress))) ||
                    (bleInfo.max_connections <= BleInfo_GetConnections(FALSE)))
                {
                    CLI_PRINTF_INFO(" -> Invalid\n");
                    break;
                }
                CLI_PRINTF_INFO("\n");

                gConnReqParams.peerAddressType = pScanningEvent->eventData.scannedDevice.addressType;
                FLib_MemCpy(gConnReqParams.peerAddress,
                            pScanningEvent->eventData.scannedDevice.aAddress,
                            sizeof(bleDeviceAddress_t));

#if gAppUsePrivacy_d
                gConnReqParams.usePeerIdentityAddress = pScanningEvent->eventData.scannedDevice.advertisingAddressResolved;
#endif
                (void)Gap_StopScanning();
                bleInfo.foundDeviceToConnect = TRUE;
            }
        }
        break;

        case gScanStateChanged_c:
        {
            bleInfo.scnOn = !bleInfo.scnOn;
            /* check SCAN_STATUS-SCAN_ON status */
            if(bleInfo.scnOn)
            {
                CLI_PRINTF_INFO(">Scanning...\n");
                bleInfo.foundDeviceToConnect = FALSE;
            }
            /* Node is not scanning */
            else
            {
                CLI_PRINTF_INFO("<Scanning stopped...\n");
                if (bleInfo.foundDeviceToConnect == TRUE)
                {
                    (void)BluetoothLEHost_Connect(&gConnReqParams, BleApp_ConnectionCallback);
                    bleInfo.foundDeviceToConnect = FALSE;
                }
            }
        }
        break;

        case gScanCommandFailed_c:
        {
            if(pScanningEvent->eventData.failReason != gHciCommandDisallowed_c)
            {
                BleApp_Reset();
            }
        }
        break;

        default:
        {
            ; /* No action required */
            break;
        }
    }
}

/*! *********************************************************************************
* \brief        Handles BLE Advertising callback from host stack.
*
* \param[in]    pAdvertisingEvent    Pointer to gapAdvertisingEvent_t.
********************************************************************************** */
static void BleApp_AdvertisingCallback(gapAdvertisingEvent_t *pAdvertisingEvent)
{
    switch (pAdvertisingEvent->eventType)
    {
        case gAdvertisingStateChanged_c:
        {
            bleInfo.advOn = !bleInfo.advOn;
            /* check ADV_STATUS-ADV_ON status */
            if(bleInfo.advOn)
            {
                CLI_PRINTF_INFO(">Advertising...\n");
            }
            else
            {
                CLI_PRINTF_INFO("<Advertising stopped...\n");
            }
        }
        break;

        case gAdvertisingCommandFailed_c:
        {
            if(pAdvertisingEvent->eventData.failReason != gHciCommandDisallowed_c)
            {
                BleApp_Reset();
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
* \brief        Handles BLE Connection callback from host stack.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    pConnectionEvent    Pointer to gapConnectionEvent_t.
********************************************************************************** */
static void BleApp_ConnectionCallback(deviceId_t peerDeviceId, gapConnectionEvent_t *pConnectionEvent)
{
    switch (pConnectionEvent->eventType)
    {
        case gConnEvtConnected_c:
        {
            if(peerDeviceId >= (uint8_t)gLclMaxConnections_c)
            {
                (void)Gap_Disconnect(peerDeviceId);
                break;
            }

            /* advertising stops after connect */
            bleInfo.advOn = FALSE;

#if gAppUsePairing_d
#if gAppUseBonding_d

            if (bleInfo.role == gGapCentral_c)
            {
                (void)Gap_CheckIfBonded(peerDeviceId, &maPeerInformation[peerDeviceId].isBonded, NULL);

                if ((maPeerInformation[peerDeviceId].isBonded) &&
                    (gBleSuccess_c == Gap_LoadCustomPeerInformation(peerDeviceId,
                            (void *) &maPeerInformation[peerDeviceId].clientInfo, 0, sizeof(wucConfig_t))))
                {
                    /* Restored custom connection information. Encrypt link */
                    (void)Gap_EncryptLink(peerDeviceId);
                }
            }

#endif /* gAppUseBonding_d*/
#endif /* gAppUsePairing_d */

            uint8_t *ptr = pConnectionEvent->eventData.connectedEvent.peerAddress;
            CLI_PRINTF_INFO(">Connected to device 0x%02X%02X%02X%02X%02X%02X", ptr[5], ptr[4], ptr[3], ptr[2], ptr[1], ptr[0]);

            if( bleInfo.role == gGapCentral_c )
            {
                CLI_PRINTF_INFO(":%d as central\n", peerDeviceId);
                /* Restart the scan for next device */
                (void)BluetoothLEHost_StartScanning(&mAppScanParams, BleApp_ScanningCallback);
            }
            else
            {
                CLI_PRINTF_INFO(":%d as peripheral\n", peerDeviceId);
            }
            FLib_MemCpy(maPeerInformation[peerDeviceId].address, pConnectionEvent->eventData.connectedEvent.peerAddress, gcBleDeviceAddressSize_c);
            maPeerInformation[peerDeviceId].gapRole = bleInfo.role;
            bleInfo.connInterval = pConnectionEvent->eventData.connectedEvent.connParameters.connInterval;

            /* run the state machine */
            BleApp_StateMachineHandler(peerDeviceId, mAppEvt_PeerConnected_c);
        }
        break;

        case gConnEvtDisconnected_c:
        {
            uint8_t *ptr = maPeerInformation[peerDeviceId].address;
            CLI_PRINTF_INFO("<Disconnected from device 0x%02X%02X%02X%02X%02X%02X:%d (%d)\n", ptr[5], ptr[4], ptr[3], ptr[2], ptr[1], ptr[0], peerDeviceId, pConnectionEvent->eventData.disconnectedEvent.reason);


            /* run the state machine */
            BleApp_StateMachineHandler(peerDeviceId, mAppEvt_PeerDisconnected_c);

            /* Reset Service Discovery to be sure*/
            BleServDisc_Stop(peerDeviceId);

            /* recalculate minimum of maximum MTU's of all connected devices */
            mAppMtu                       = mAppMtu_c;

            for (uint8_t mPeerId = 0; mPeerId < (uint8_t)gLclMaxConnections_c; mPeerId++)
            {
                if (gInvalidDeviceId_c != maPeerInformation[mPeerId].deviceId)
                {
                    uint16_t tempMtu = 0U;

                    (void)Gatt_GetMtu(mPeerId, &tempMtu);
                    tempMtu = gAttMaxWriteDataSize_d(tempMtu);

                    if (tempMtu < mAppMtu)
                    {
                        mAppMtu = tempMtu;
                    }
                }
            }
        }
        break;

#if gAppUsePairing_d

        case gConnEvtPairingComplete_c:
        {
            if (pConnectionEvent->eventData.pairingCompleteEvent.pairingSuccessful)
            {
                BleApp_StateMachineHandler(peerDeviceId,
                                           mAppEvt_PairingComplete_c);
            }
        }
        break;
#endif /* gAppUsePairing_d */
         case gConnEvtLeDataLengthChanged_c:
             break;

        default:
        {
            CLI_PRINTF_DBG("Unhandled connection event (%d)\n", pConnectionEvent->eventType);
        }
        break;
    }

    /* Connection Manager to handle Host Stack interactions */
    if(peerDeviceId < (uint8_t)gLclMaxConnections_c)
    {
        switch (maPeerInformation[peerDeviceId].gapRole)
        {
            case gGapCentral_c:
                BleConnManager_GapCentralEvent(peerDeviceId, pConnectionEvent);
                break;

            case gGapPeripheral_c:
                BleConnManager_GapPeripheralEvent(peerDeviceId, pConnectionEvent);
                break;

            default:
                ; /* No action required */
                break;
        }
    }
}

/*! *********************************************************************************
* \brief        Handle Service Discovery events
*
* \param[in]    peerDeviceId        Remote device ID.
* \param[in]    pEvent              Pointer to the event revceived.
********************************************************************************** */
static void BleApp_ServiceDiscoveryCallback
(
    deviceId_t peerDeviceId,
    servDiscEvent_t *pEvent
)
{
    switch (pEvent->eventType)
    {
        case gServiceDiscovered_c:
        {
            if (pEvent->eventData.pService->uuidType == gBleUuidType128_c)
            {
                if (FLib_MemCmp((void *)&uuid_service_wireless_ranging, (void *)&pEvent->eventData.pService->uuid, sizeof(bleUuid_t)))
                {
                    BleApp_StoreServiceHandles(peerDeviceId, pEvent->eventData.pService);
                }
            }
        }
        break;

        case gDiscoveryFinished_c:
        {
            if (pEvent->eventData.success)
            {
                if (gGattDbInvalidHandleIndex_d != maPeerInformation[peerDeviceId].clientInfo.hService)
                {
                    BleApp_StateMachineHandler(peerDeviceId,
                                               mAppEvt_ServiceDiscoveryComplete_c);
                }
                else
                {
                    BleApp_StateMachineHandler(peerDeviceId,
                                               mAppEvt_ServiceDiscoveryNotFound_c);
                }
            }
            else
            {
                BleApp_StateMachineHandler(peerDeviceId,
                                           mAppEvt_ServiceDiscoveryFailed_c);
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
    switch (procedureResult)
    {
        case gGattProcError_c:
            if (gGattPeerDisconnected_c == error)
            {
                /* When gGattPeerDisconnected_c is received, the deviceId has been invalidated */
                BleApp_StateMachineHandler(serverDeviceId, mAppEvt_PeerDisconnected_c);
            }
            else
            {
                BleApp_StateMachineHandler(serverDeviceId, mAppEvt_GattProcError_c);
            }
            break;

        case gGattProcSuccess_c:
            BleApp_StateMachineHandler(serverDeviceId, mAppEvt_GattProcComplete_c);
            break;

        default:
            ; /* No action required */
            break;
    }

    /* Signal Service Discovery Module */
    BleServDisc_SignalGattClientEvent(serverDeviceId, procedureType, procedureResult, error);
}

/*! *********************************************************************************
 * \brief        Handles GATT server callback from host stack.
 *
 * \param[in]    deviceId        Client peer device ID.
 * \param[in]    pServerEvent    Pointer to gattServerEvent_t.
 ********************************************************************************** */
static void BleApp_GattServerCallback(
    deviceId_t deviceId,
    gattServerEvent_t *pServerEvent)
{
    uint16_t tempMtu = 0;

    switch (pServerEvent->eventType)
    {
        case gEvtAttributeWrittenWithoutResponse_c:
        {
            if (pServerEvent->eventData.attributeWrittenEvent.handle == (uint16_t)value_wr_ctrl_stream)
            {
                wrs_msg_received(deviceId, pServerEvent->eventData.attributeWrittenEvent.aValue,
                                 pServerEvent->eventData.attributeWrittenEvent.cValueLength);
            }
            break;
        }

        case gEvtMtuChanged_c:
        {
            /* update stream length with minimum of  new MTU */
            (void)Gatt_GetMtu(deviceId, &tempMtu);
            tempMtu = gAttMaxWriteDataSize_d(tempMtu);

            mAppMtu = mAppMtu <= tempMtu ? mAppMtu : tempMtu;
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
 * \brief        Checks Scan data for a device to connect.
 *
 * \param[in]    pData              Pointer to gapScannedDevice_t.
 ********************************************************************************** */
static bool_t MatchDataInAdvElementList(gapAdStructure_t *pElement,
                                        void *pData,
                                        uint8_t iDataLen)
{
    uint8_t i;
    bool_t status = FALSE;

    for (i = 0; i < (pElement->length - 1U); i += iDataLen)
    {
        if (FLib_MemCmp(pData, &pElement->aData[i], iDataLen))
        {
            status = TRUE;
            break;
        }
    }

    return status;
}

/*! *********************************************************************************
 * \brief        Checks Scan data for a device to connect.
 *
 * \param[in]    pData    Pointer to gapScannedDevice_t.
 ********************************************************************************** */
static bool_t BleApp_CheckScanEvent(gapScannedDevice_t *pData)
{
    uint32_t index = 0;
    bool_t foundMatch = FALSE;

    while (index < pData->dataLength)
    {
        gapAdStructure_t adElement;

        adElement.length = pData->data[index];
        adElement.adType = (gapAdType_t) pData->data[index + 1U];
        adElement.aData = &pData->data[index + 2U];

        /* Search for Wireless Ranging Service */
        if ((adElement.adType == gAdIncomplete128bitServiceList_c)
            || (adElement.adType == gAdComplete128bitServiceList_c))
        {
            foundMatch = MatchDataInAdvElementList(&adElement,
                                                   &uuid_service_wireless_ranging, 16);
        }

        /* Move on to the next AD element type */
        index += (uint32_t)adElement.length + sizeof(uint8_t);
    }

    return foundMatch;
}

/*! *********************************************************************************
 * \brief        Stores handles used by the application.
 *
 * \param[in]    pService    Pointer to gattService_t.
 ********************************************************************************** */
static void BleApp_StoreServiceHandles(deviceId_t peerDeviceId, gattService_t *pService)
{
    /* Found Wireless Ranging Service */
    maPeerInformation[peerDeviceId].clientInfo.hService = pService->startHandle;

    if (pService->cNumCharacteristics > 0U &&
        pService->aCharacteristics != NULL)
    {
        /* Found stream Characteristic */
        maPeerInformation[peerDeviceId].clientInfo.hCtrlStream =
            pService->aCharacteristics[0].value.handle;
    }
}

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
    uint16_t tempMtu = 0;
    bleUuid_t uuid;

    switch(event)
    {
        case mAppEvt_PeerConnected_c:
            maPeerInformation[peerDeviceId].deviceId = peerDeviceId;
            maPeerInformation[peerDeviceId].appState = mAppIdle_c;
            wrs_ConnDataInit(peerDeviceId, bleInfo.connInterval);
             /* Default is Server as soon as a connection is established, may be changed if a range command is issued */
            wrs_SetRole(peerDeviceId, eRoleServer);
            /* Default settings */
            wrs_SetDefaultSettings(peerDeviceId);
            break;
        case mAppEvt_PeerDisconnected_c:
            maPeerInformation[peerDeviceId].deviceId = gInvalidDeviceId_c; /* mark device id as invalid */
            maPeerInformation[peerDeviceId].appState = mAppIdle_c;
            maPeerInformation[peerDeviceId].clientInfo.hService = gGattDbInvalidHandleIndex_d;
            maPeerInformation[peerDeviceId].clientInfo.hCtrlStream = gGattDbInvalidHandleIndex_d;
            if (csAlgoBuf != NULL)
            {
                rade_deinit(&csAlgoBuf);
            }
            wrs_ConnDataRelease(peerDeviceId);
            break;
        default:
            {
                ; /* No action required */
                break;
            }
    }

    /* invalid client information */
    if (gInvalidDeviceId_c == maPeerInformation[peerDeviceId].deviceId)
    {
        return;
    }

    switch (maPeerInformation[peerDeviceId].appState)
    {
        case mAppIdle_c:
        {
            if (event == mAppEvt_PeerConnected_c)
            {
                /* Let the central device initiate the Exchange MTU procedure*/
                if (maPeerInformation[peerDeviceId].gapRole == gGapCentral_c)
                {
                    /* Moving to Exchange MTU State */
                    maPeerInformation[peerDeviceId].appState = mAppExchangeMtu_c;
                    (void)GattClient_ExchangeMtu(peerDeviceId, gAttMaxMtu_c);
                }
                else
                {
#if gAppUseServiceDiscovery_d
                    /* Moving to Service Discovery State*/
                    maPeerInformation[peerDeviceId].appState = mAppServiceDisc_c;

                    FLib_MemCpy(uuid.uuid128, uuid_service_wireless_ranging, sizeof(bleUuid_t));

                    /* Start Service Discovery*/
                    (void)BleServDisc_FindService(peerDeviceId,
                                                  gBleUuidType128_c,
                                                  &uuid);
#else
                     maPeerInformation[peerDeviceId].appState = mAppRunning_c;
#endif /* gAppUseServiceDiscovery_d */
                }
            }
        }
        break;

        case mAppExchangeMtu_c:
        {
            if (event == mAppEvt_GattProcComplete_c)
            {
                /* update stream length with minimum of maximum MTU's of connected devices */
                (void)Gatt_GetMtu(peerDeviceId, &tempMtu);
                tempMtu = gAttMaxWriteDataSize_d(tempMtu);

                mAppMtu = mAppMtu <= tempMtu ? mAppMtu : tempMtu;

#if gAppUseServiceDiscovery_d
                /* Moving to Service Discovery State*/
                maPeerInformation[peerDeviceId].appState = mAppServiceDisc_c;

                FLib_MemCpy(uuid.uuid128, uuid_service_wireless_ranging, sizeof(bleUuid_t));

                /* Start Service Discovery*/
                (void)BleServDisc_FindService(peerDeviceId,
                                              gBleUuidType128_c,
                                              &uuid);
#else
                 maPeerInformation[peerDeviceId].appState = mAppRunning_c;
                 if (maPeerInformation[peerDeviceId].gapRole == gGapCentral_c) {
                     /* Start CS setup procedure from central device */
                     wrs_StartSetup(peerDeviceId);
                 }
#endif /* gAppUseServiceDiscovery_d */
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

        case mAppServiceDisc_c:
        {
            if (event == mAppEvt_ServiceDiscoveryComplete_c)
            {
                /* Moving to Running State*/
                maPeerInformation[peerDeviceId].appState = mAppRunning_c;
#if gAppUseBonding_d
                /* Write data in NVM */
                (void)Gap_SaveCustomPeerInformation(maPeerInformation[peerDeviceId].deviceId,
                                                    (void *) &maPeerInformation[peerDeviceId].clientInfo, 0,
                                                    sizeof(wucConfig_t));
#endif
                 if (maPeerInformation[peerDeviceId].gapRole == gGapCentral_c)
                 {
                     /* Start CS setup procedure from central device */
                     wrs_StartSetup(peerDeviceId);
                 }
            }
            else if (event == mAppEvt_ServiceDiscoveryNotFound_c)
            {
                /* Moving to Service discovery Retry State*/
                maPeerInformation[peerDeviceId].appState = mAppServiceDiscRetry_c;
                /* Restart Service Discovery for all services */
                (void)BleServDisc_Start(peerDeviceId);
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

        case mAppServiceDiscRetry_c:
        {
            if (event == mAppEvt_ServiceDiscoveryComplete_c)
            {
                /* Moving to Running State*/
                maPeerInformation[peerDeviceId].appState = mAppRunning_c;
            }
            else if ((event == mAppEvt_ServiceDiscoveryNotFound_c) ||
                     (event == mAppEvt_ServiceDiscoveryFailed_c))
            {
                (void)Gap_Disconnect(peerDeviceId);
            }
            else
            {
                /* ignore other event types */
            }
        }
        break;

        case mAppRunning_c:
            break;

        default:
        {
            ; /* No action required */
        }
        break;
    }
}

/*
 * Check if it's time to collect temperature (will inform NBU as a consequence)
 */
static void CheckForTemperatureUpdate(void)
{
    static uint32_t tickCounter;
    const uint32_t tickInterval = gTemperaturePollingInterval_c/gBleInfoRefreshTime_c;

    if ((tickCounter % tickInterval) == 0)
    {
      SENSORS_TriggerTemperatureMeasurement();
    }
    else if ((tickCounter % tickInterval) == 1)
    {
      (void) SENSORS_RefreshTemperatureValue();
    }

    tickCounter ++;
}

/*! *********************************************************************************
 * \brief        Update BLE info and LEDs according to connection state
 ********************************************************************************** */
static void BleInfo_TimerCallback(void * pParam)
{
    bleConnStates_t prevState = bleInfo.state;
    if(bleInfo.retries != 0U)
    {
        bleInfo.retries--;
    }
    if(pParam == NULL)
    {
        CheckForTemperatureUpdate();
    }

    switch(bleInfo.state)
    {
    case mConnIdle:
        FLib_MemCpy(bleInfo.address_list, gCommunicationSettings->address_list, sizeof(gCommunicationSettings->address_list));
        /* TODO: multiple connections */
        bleInfo.max_connections = 1;
        bleInfo.role = gCommunicationSettings->role;
        gConnReqParams.connIntervalMin = (uint16_t)(gCommunicationSettings->conn_int);
        gConnReqParams.connIntervalMax = gConnReqParams.connIntervalMin;
        gConnReqParams.supervisionTimeout = (uint16_t)((10U * (gCommunicationSettings->conn_int * 1250U)) / 10000U); /* [10ms] make it 10 times the Conn interval */
        if (gConnReqParams.supervisionTimeout < WR_MIN_SUPERVISION_TIMEOUT) {
            gConnReqParams.supervisionTimeout = WR_MIN_SUPERVISION_TIMEOUT;
        }
        if (gConnReqParams.supervisionTimeout > gcConnectionSupervisionTimeoutMax_c) {
            gConnReqParams.supervisionTimeout = gcConnectionSupervisionTimeoutMax_c;
        }

        /*
         * Scanning/Advertising must be enabled within 500ms after it was requested (depending on intervals?)
         * Connection must be active within 500ms after connection was setup (depending on intervals?)
         */
        switch(bleInfo.role)
        {
        case gGapCentral_c:
        case gGapPeripheral_c:
            bleInfo.state = mConnRequest;
            if(!bleInfo.scnOn && bleInfo.role == gGapCentral_c)
            {
                gPairingParameters.localIoCapabilities = gIoKeyboardDisplay_c;
                (void)BluetoothLEHost_StartScanning(&mAppScanParams, BleApp_ScanningCallback);
            }
            else if(!bleInfo.advOn && bleInfo.role == gGapPeripheral_c)
            {
                gPairingParameters.localIoCapabilities = gIoDisplayOnly_c;
                (void)BluetoothLEHost_StartAdvertising(&mAppAdvParams, BleApp_AdvertisingCallback, BleApp_ConnectionCallback);
            }
            else
            {
                /* No action needed */
            }
            break;
        default:
            {
                ; /* No action required */
                break;
            }
        }
        break;

    case mConnRequest:
        /* scanning/advertising was requested: wait until it is enabled */
        if(bleInfo.retries == 0U)
        {
            CLI_PRINTF_DBG("Scan/Advertising failed\n");
            bleInfo.state = mConnDisconnect;
        }
        else if(BleInfo_GetConnections(TRUE) == bleInfo.max_connections)
        {
            bleInfo.state = mConnConnected;
        }
        else if(BleInfo_GetConnections(FALSE) != 0U)
        {
            bleInfo.state = mConnConnecting;
        }
        else if(bleInfo.advOn || bleInfo.scnOn)
        {
            bleInfo.state = mConnSeek;
        }
        else
        {
            /* No action needed */
        }
        break;

    case mConnSeek:
        /* either we should be advertising/scanning or have a connection */
        /* when the correct advertise message is received we manually instruct the stack
         * to stop-scanning and connect to the peripheral. scanning is stopped before the connection is made,
         * so there is a gap where scanning state is off but the connection process is ongoing */
        if(BleInfo_GetConnections(TRUE) == bleInfo.max_connections)
        {
            bleInfo.state = mConnConnected;
        }
        else if(BleInfo_GetConnections(FALSE) != 0U)
        {
            bleInfo.state = mConnConnecting;
        }
        else if((!BleInfo_CompareConfiguration()) ||
                (bleInfo.advOn && bleInfo.role == gGapCentral_c) ||
                (bleInfo.scnOn && bleInfo.role == gGapPeripheral_c))
        {
            bleInfo.state = mConnDisconnect;
        }
        else
        {
            /* No action needed */
        }
        break;

    case mConnConnecting:
        if(bleInfo.retries == 0U)
        {
            CLI_PRINTF_DBG("Not all peripherals connected in time\n");
            bleInfo.state = mConnDisconnect;
        }
        else if(BleInfo_GetConnections(FALSE) == 0U)
        {
            bleInfo.state = mConnDisconnect;
        }
        else if(BleInfo_GetConnections(TRUE) == bleInfo.max_connections)
        {
            bleInfo.state = mConnConnected;
        }
        else
        {
            /* No action needed */
        }
        break;

    case mConnConnected:
        /* Disconnect all if one connection is lost to prevent unexpected states. */
        if(BleInfo_GetConnections(TRUE) != bleInfo.max_connections)
        {
            bleInfo.state = mConnDisconnect;
        }
        break;

    case mConnDisconnect:
        if(bleInfo.retries == 0U)
        {
            /* try again and clear peer information */
            BleInfo_Disconnect(TRUE);
            bleInfo.state = mConnIdle;
        }
        else if (BleInfo_GetConnections(FALSE) == 0U && !bleInfo.advOn && !bleInfo.scnOn)
        {
            bleInfo.state = mConnIdle;
        }
        else
        {
            /* No action needed */
        }
        break;

    default:
        bleInfo.state = mConnIdle;
        break;
    }

    if (prevState != bleInfo.state)
    {
        CLI_PRINTF_DBG("BleState=%d\n", bleInfo.state);
        bleInfo.retries = (gBleInfoTimeout_c / gBleInfoRefreshTime_c) + 1;
        if(bleInfo.state == mConnConnecting)
        {
            bleInfo.retries *= (10U * (uint16_t)bleInfo.max_connections);
        }

        switch (bleInfo.state)
        {
            case mConnDisconnect:
                BleInfo_Disconnect(FALSE);
                break;

            case mConnConnected:
                if( bleInfo.role == gGapCentral_c )
                {
                    (void)Gap_StopScanning();
                }
                break;

            default:
            {
                ; /* No action required */
                break;
            }
        }
        switch (bleInfo.state)
        {
            case mConnRequest:
            case mConnSeek:
            case mConnConnecting:
                bleInfo.ledstate = mLedConnecting;
                break;
            case mConnConnected:
                bleInfo.ledstate = mLedConnected;
                break;
            default:
                bleInfo.ledstate = mLedIdle;
                break;
        }

#if (defined(gAppLedCnt_c) && (gAppLedCnt_c > 0))
        bleLedStates_t prevledstate = bleInfo.ledstate;
        if ((prevledstate != bleInfo.ledstate) && (gLocalSettings->debug == 0U))
        {
            BleApp_SetLED(bleInfo.ledstate);
        }
#endif /*gAppLedCnt_c > 0*/
    }
}

/*! *********************************************************************************
 * \brief        Compare configurations from bleInfo and gCommunicationSettings
 ********************************************************************************** */
static bool_t BleInfo_CompareConfiguration(void)
{
    if (bleInfo.role != gCommunicationSettings->role)
    {
        return FALSE;
    }

    if (!FLib_MemCmp(bleInfo.address_list,
                     gCommunicationSettings->address_list,
                     sizeof(gCommunicationSettings->address_list)))
    {
        return FALSE;
    }

    return TRUE;
}

/*! *********************************************************************************
 * \brief        Check if the given address belongs to a connected device
 *
 * \param[in]    pAddress    Address to be checked
 ********************************************************************************** */
static bool_t BleInfo_CheckAddress (uint8_t* pAddress)
{
    uint8_t mId = 0;
    if (FLib_MemCmp(bleInfo.address_list[0], addressAllZeros, gcBleDeviceAddressSize_c))
    {
        return TRUE;
    }

    for (mId = 0; mId < bleInfo.max_connections; mId++)
    {
        if (FLib_MemCmp(bleInfo.address_list[mId], pAddress, gcBleDeviceAddressSize_c))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*! *********************************************************************************
 * \brief        Reset CPU
 ********************************************************************************** */
static void BleApp_Reset(void)
{
    HAL_ResetMCU();
}

/*! *********************************************************************************
 * \brief        Close any connection and stops advertising/scanning.
 *
 * \param[in]    hardreset    If TRUE handle disconnect event in the main application
 *                            state machine handler
 ********************************************************************************** */
static void BleInfo_Disconnect(bool_t hardreset)
{
    uint8_t mPeerId;
    /* KW3x SDKs prior 2.2.2 ignored multiple calls to StopScanning/StopAdvertising, newer do not! */
    if (bleInfo.scnOn)
    {
        (void)Gap_StopScanning();
    }

    if(bleInfo.advOn)
    {
        (void)Gap_StopAdvertising();
    }

    for (mPeerId = 0; mPeerId < (uint8_t)gLclMaxConnections_c; mPeerId++)
    {
        if (maPeerInformation[mPeerId].deviceId == gInvalidDeviceId_c)
        {
            continue;
        }
        (void)Gap_Disconnect(mPeerId);
        maPeerInformation[mPeerId].appState = mAppIdle_c;

        if (hardreset)
        {
            BleApp_StateMachineHandler(mPeerId, mAppEvt_PeerDisconnected_c);
        }
    }
}

/*! *********************************************************************************
* \brief        Handles localization callback
*
* \param[in]    pEvent      Event to be treated
********************************************************************************** */
void AppLclCtrlCallback(wrs_app_event_t *pEvent)
{
    switch(pEvent->eventType) {
    case eAppEvtMeasureComplete:
        {
            wrs_AppMeasureCompleteEvent_t *evt_d = (wrs_AppMeasureCompleteEvent_t *)&pEvent->evt_d;

            wrs_RecordTimestamp(&evt_d->serviceInfo, eTimestampStartPostProcessing);
            DEBUG_PIN0_SET
            /* Convert raw measurement data */
            static isp_meas_response_t meas_response;
            measurement_populate_response(&meas_response, evt_d->localEventResultBuffer,
                                          evt_d->remoteEventResultBuffer,
                                          evt_d->localEventDebugBuffer, evt_d->remoteEventDebugBuffer);
            /* Invoke ranging engine on client */
            engine_response_t engine_response = {0};

            /* Do not attempt to run ranging_engine if one of:
                - this device is the server
                - measurement failed (even partially)
                - client-server over the air data exchange didn't complete successfully on time
             */
            if (evt_d->serviceInfo.dataExchangeDone &&
                (wrs_GetRole(pEvent->deviceId) == eRoleClient) &&
                (meas_response.cs_data->status == 0))
            {
                engine_config_t engine_config;
                engine_config.mciq_algo_flags = gLocalSettings->mciq_algo_flags;
                engine_config.n_ap = meas_response.mciq_data->n_ap;
                engine_config.cde_threshold = gLocalSettings->cde_threshold;
                engine_config.cde_div_threshold = gLocalSettings->cde_div_threshold;

                DEBUG_PIN0_PLS
                if (meas_response.mciq_data[0].nbSteps > 0)
                {
                    isp_mciq_ranging_compute(&meas_response, &engine_response.mciq_result, &engine_config);
                }
                DEBUG_PIN0_PLS
                if (meas_response.tof_data[0].nbSteps > 0)
                {
                    isp_tof_ranging_compute(&meas_response, &engine_response.tof_result);
                }
                DEBUG_PIN0_PLS
                engine_response.is_valid = TRUE;
            }
            else
            {
                FLib_MemSet(&engine_response, 0 , sizeof(engine_response));
                engine_response.is_valid = FALSE;
            }
            wrs_RecordTimestamp(&evt_d->serviceInfo, eTimestampStop);
            cli_print_measurement(&meas_response, &engine_response, &evt_d->serviceInfo);
            wrs_ProcTearDown(pEvent->deviceId);
            DEBUG_PIN0_CLR
        }
        break;
    default:
        CLI_PRINTF("Received unknown localisation event: %d\n", pEvent->eventType);
        break;
    }
}

#if (defined(KW37A4_SERIES) || defined(KW37Z4_SERIES) || defined(KW38A4_SERIES) || defined(KW38Z4_SERIES) || defined(KW39A4_SERIES))
/* Override weak function defined in SDK */
void Controller_InterruptHandlerWrapper(void)
{
    isp_controller_ble_event();
    Controller_InterruptHandler();
}
#endif

/*! *********************************************************************************
 * \brief        Function handling the geeneric events from the host stack.
 *
 ********************************************************************************** */
static void BluetoothLEHost_GenericCallback(gapGenericEvent_t *pGenericEvent)
{
    /* Call BLE Conn Manager */
    BleConnManager_GenericEvent(pGenericEvent);
}

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
