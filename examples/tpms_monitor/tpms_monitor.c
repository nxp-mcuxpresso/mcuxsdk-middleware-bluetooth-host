/*! *********************************************************************************
* \addtogroup TPMS Monitor
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2026 NXP
*
*
* \file
*
* This file is the source file for the TPMS Monitor application
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
#include "RNG_Interface.h"
#include "fsl_component_button.h"
#include "fsl_component_led.h"
#include "fsl_component_panic.h"
#include "fsl_component_timer_manager.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_serial_manager.h"
#include "fsl_format.h"
#include "fsl_adapter_reset.h"
#include "FunctionLib.h"
#include "NVM_Interface.h"

/* BLE Host Stack */
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gap_interface.h"

/* Wrappers */
#include "ble_conn_manager.h"
#include "ble_service_discovery.h"

#include "app_conn.h"
#include "app_scanner.h"
#include "board.h"
#include "app.h"
#include "tpms_monitor.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define LedTurnOffAllLeds() LedStopFlashingAllLeds()

#define mcNumChars_c     3U
/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

typedef enum appEvent_tag{
    mAppEvt_PeerConnected_c,
    mAppEvt_PairingComplete_c,
    mAppEvt_LinkEncrypted_c,
    mAppEvt_ServiceDiscoveryComplete_c,
    mAppEvt_ServiceDiscoveryFailed_c,
    mAppEvt_GattProcComplete_c,
    mAppEvt_GattProcError_c
}appEvent_t;

typedef enum appState_tag{
    mAppIdle_c,
    mAppExchangeMtu_c,
    mAppServiceDisc_c,
    mAppEstablishSecurity_c,
    mAppRunning_c,
}appState_t;

/*! TPMS Monitor - Configuration */
typedef struct tpmsMonitorConfig_tag
{
    uint16_t    hService;
    uint16_t    hTirePressure;
    uint16_t    hTirePressureCccd;    
    uint16_t    hTireTemperature;
    uint16_t    hTireTemperatureCccd;
    uint16_t    hTireAcceleration;
    uint16_t    hTpmsProperties;
    uint16_t    hTpmsMonitoringDutyCycle;
    uint16_t    hTpmsPosition;
    uint16_t    hTpmsSigningKey;
    uint16_t    hTpmsSigningKeyCccd;
} tpmsMonitorConfig_t;

typedef struct appCustomInfo_tag
{
    bool_t usesHmac;
    uint32_t lastSeqNum;
    uint8_t tpmsSigningKey[16];
}appCustomInfo_t;

typedef struct appPeerInfo_tag
{
    deviceId_t      deviceId;
    tpmsMonitorConfig_t tpmsMonitorConfig;
    appCustomInfo_t customInfo;
    bool_t          isBonded;
    appState_t      appState;
}appPeerInfo_t;
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
/* Set to FALSE to only process non-connectable advertising from bonded peers */
static bool_t bProcessAllScanReports = TRUE;

/* Table with peer devices information */
static appPeerInfo_t maPeerInformation[gAppMaxConnections_c];

static bool_t mRestoringBondedLink = FALSE;
static bool_t mAuthRejected = FALSE;

static bool_t   mScanningOn = FALSE;
static bool_t   mFoundDeviceToConnect = FALSE;

/* Buffer used for Characteristic related procedures */
static gattAttribute_t       mpCharProcBuffer;
static gattCharacteristic_t  mpCharacteristic;

/* Timers */
static TIMER_MANAGER_HANDLE_DEFINE(mAppTimerId);

static serial_handle_t gAppSerMgrIf;
static SERIAL_MANAGER_WRITE_HANDLE_DEFINE(s_writeHandle);

static appScanningParams_t mAppScanParams = {
    &gScanParams,
    gGapDuplicateFilteringDisable_c,
    gGapScanContinuously_d,
    gGapScanPeriodicDisabled_d
};

static gattCharacteristic_t maChars[mcNumChars_c];
static uint8_t maPressureValue[3];
static uint8_t maTemperatureValue[2];
static uint8_t maAccelerationValue[2];

/* Generic buffers used for GATT characteristic reads/writes */
static uint8_t *maCharValue[gAppMaxConnections_c];

static uint16_t mLastWrittenHandle = gGattDbInvalidHandle_d;
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

static void BleApp_GattClientCallback
(
    deviceId_t              serverDeviceId,
    gattProcedureType_t     procedureType,
    gattProcedureResult_t   procedureResult,
    bleResult_t             error
);

static void BleApp_GattIndicationCallback
(
    deviceId_t          serverDeviceId,
    uint16_t characteristicValueHandle,
    uint8_t* aValue,
    uint16_t valueLength
);

static void BleApp_GattNotificationCallback
(
    deviceId_t          serverDeviceId,
    uint16_t characteristicValueHandle,
    uint8_t* aValue,
    uint16_t valueLength
);


static void BleApp_ServiceDiscoveryCallback
(
    deviceId_t peerDeviceId,
    servDiscEvent_t* pEvent
);

static void BleApp_StateMachineHandlerRunning
(
    deviceId_t peerDeviceId,
    appEvent_t event
);

static void BleApp_StateMachineHandleEstablishSecurity
(
    deviceId_t peerDeviceId,
    appEvent_t event
);

static void BleApp_StateMachineHandlerServiceDisc
(
    deviceId_t peerDeviceId,
    appEvent_t event
);

static void BleApp_StateMachineHandlerExchangeMtu
(
    deviceId_t peerDeviceId,
    appEvent_t event
);

static void BleApp_StateMachineHandlerIdle
(
    deviceId_t peerDeviceId,
    appEvent_t event
);

static void BleApp_StateMachineHandler
(
    deviceId_t peerDeviceId,
    appEvent_t event
);

static bool_t CheckScanEvent(gapScannedDevice_t* pData);
static void ProcessNonConnScanEvent(gapScannedDevice_t* pData);
static void ParseIndicatorsAndOptionalFields(uint8_t *pData, uint8_t optionalFieldsLen);

static void BleApp_StoreServiceHandles
(
    deviceId_t      peerDeviceId,
    gattService_t   *pService
);

static void BleApp_ReadPressureTemperatureAcceleration(deviceId_t peerDeviceId);

static void AppPrintString(const char* pBuff);
static void AppPrintDec(uint16_t dec);

static void BluetoothLEHost_Initialized(void);
static void BleApp_SerialInit(void);
static void BluetoothLEHost_GenericCallback (gapGenericEvent_t* pGenericEvent);

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
button_status_t BleApp_HandleKeys0(void *pButtonHandle, button_callback_message_t *pMessage, void *pCallbackParam);
button_status_t BleApp_HandleKeys1(void *pButtonHandle, button_callback_message_t *pMessage, void *pCallbackParam);
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0)) */
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
    BleApp_SerialInit();
    LedStartFlashingAllLeds();
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[0], BleApp_HandleKeys0, NULL);
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[1], BleApp_HandleKeys1, NULL);
#endif
    /* Initialize Bluetooth Host Stack */
    BluetoothLEHost_SetGenericCallback(BluetoothLEHost_GenericCallback);
    BluetoothLEHost_Init(BluetoothLEHost_Initialized);

    /* Initialize table with peer devices information  */
    FLib_MemSet(maPeerInformation, 0U, sizeof(appPeerInfo_t) * gAppMaxConnections_c);

    for (uint8_t peerId = 0U; peerId < (uint8_t)gAppMaxConnections_c; peerId++)
    {
        maPeerInformation[peerId].deviceId = gInvalidDeviceId_c;
    }
}

/*! *********************************************************************************
* \brief    Starts the BLE application.
*
********************************************************************************** */
void BleApp_Start(void)
{
    if (!mScanningOn)
    {
        if (gcBondedDevices == gAppMaxTpmsSensors_c)
        {
            mAppScanParams.pHostScanParams->filterPolicy = gScanWithFilterAcceptList_c;
        }
        else
        {
            mAppScanParams.pHostScanParams->filterPolicy = gScanAll_c;
        }

        /* Start scanning */
        (void)BluetoothLEHost_StartScanning(&mAppScanParams, BleApp_ScanningCallback);
    }
}

/*! *********************************************************************************
* \brief        Handler for the first key.
*
* \param[in]    pButtonHandle       Pointer to the button handle.
* \param[in]    pMessage            Pointer to the message.
* \param[in]    pCallbackParam      Pointer to the callback parameters.
********************************************************************************** */
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
button_status_t BleApp_HandleKeys0(void *pButtonHandle, button_callback_message_t *pMessage, void *pCallbackParam)
{
    switch (pMessage->event)
    {

    case kBUTTON_EventOneClick:
    case kBUTTON_EventShortPress:
        {
            BleApp_Start();
        }
        break;
    /* Disconnect on long button press */
    case kBUTTON_EventLongPress:
        {
            uint8_t mPeerId = 0;

            /* Disconnect all peer devices */
            for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
            {
                if (maPeerInformation[mPeerId].deviceId != gInvalidDeviceId_c)
                {
                    AppPrintString("\r\nDisconnecting...\r\n");
                    (void)Gap_Disconnect(maPeerInformation[mPeerId].deviceId);
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

/*! *********************************************************************************
* \brief        Handler for the second key.
*
* \param[in]    pButtonHandle       Pointer to the button handle.
* \param[in]    pMessage            Pointer to the message.
* \param[in]    pCallbackParam      Pointer to the callback parameters.
********************************************************************************** */
button_status_t BleApp_HandleKeys1(void *pButtonHandle, button_callback_message_t *pMessage, void *pCallbackParam)
{
    (void)pButtonHandle;
    (void)pCallbackParam;
    switch (pMessage->event)
    {
        /* Factory reset on long button press */
        case kBUTTON_EventLongPress:
        {
            /* Erase NVM Datasets */
            NVM_Status_t status = NvFormat();
            if (status != gNVM_OK_c)
            {
                /* NvFormat exited with an error status */ 
                panic(0, (uint32_t)BleApp_HandleKeys1, 0, 0);
            }

            /* Reset MCU */
            HAL_ResetMCU();
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
#endif /*gAppButtonCnt_c > 0 */
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief        Handles BLE generic callback.
*
* \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
********************************************************************************** */
static void BluetoothLEHost_GenericCallback (gapGenericEvent_t* pGenericEvent)
{
    /* Call BLE Conn Manager */
    BleConnManager_GenericEvent(pGenericEvent);
}

/*! *********************************************************************************
* \brief        Configures BLE Stack after initialization. Usually used for
*               configuring advertising, scanning, filter accept list, services, et al.
*
********************************************************************************** */
static void BluetoothLEHost_Initialized(void)
{
    /* Configure as GAP Central */
    BleConnManager_GapCommonConfig();

    /* Register for callbacks*/
    (void)App_RegisterGattClientProcedureCallback(BleApp_GattClientCallback);
    (void)App_RegisterGattClientIndicationCallback(BleApp_GattIndicationCallback);
    (void)App_RegisterGattClientNotificationCallback(BleApp_GattNotificationCallback);
    BleServDisc_RegisterCallback(BleApp_ServiceDiscoveryCallback);

    /* Initialize private variables */
    mScanningOn = FALSE;
    mFoundDeviceToConnect = FALSE;

    /* Allocate scan timeout timer */
    (void)TM_Open(mAppTimerId);

    AppPrintString("\r\nTPMS Monitor -> Press SCANSW to scan/connect to a TPMS Sensor.\r\n");


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
            /* Check if the scanned device implements the TPMS Service */
            if( FALSE == mFoundDeviceToConnect )
            {
                mFoundDeviceToConnect = CheckScanEvent(&pScanningEvent->eventData.scannedDevice);

                /* Found TPMS Service */
                if (mFoundDeviceToConnect)
                {
                    /* The advertising is connectable */
                    if (pScanningEvent->eventData.scannedDevice.advEventType == gBleAdvRepAdvInd_c)
                    {
                        /* Set connection parameters and stop scanning. Connect on gScanStateChanged_c. */
                        gConnReqParams.peerAddressType = pScanningEvent->eventData.scannedDevice.addressType;
                        FLib_MemCpy(gConnReqParams.peerAddress,
                                    pScanningEvent->eventData.scannedDevice.aAddress,
                                    sizeof(bleDeviceAddress_t));

                        (void)Gap_StopScanning();
                        gConnReqParams.usePeerIdentityAddress = pScanningEvent->eventData.scannedDevice.advertisingAddressResolved;
                    }
                    /* Non-connectable advertising */
                    else
                    {
                        /* Reset mFoundDeviceToConnect to keep checking scan events */
                        if ((pScanningEvent->eventData.scannedDevice.advertisingAddressResolved == TRUE) || (bProcessAllScanReports == TRUE))
                        {
                            ProcessNonConnScanEvent(&pScanningEvent->eventData.scannedDevice);
                        }
                    }
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

                AppPrintString("Scanning...\r\n");
                LedStopFlashingAllLeds();
                Led1Flashing();
            }
            /* Node is not scanning */
            else
            {
                (void)TM_Stop((timer_handle_t)mAppTimerId);

                /* Connect with the previously scanned peer device */
                if (mFoundDeviceToConnect)
                {

                    AppPrintString("Scan stopped.\r\n");
                    AppPrintString("Connecting...\r\n");
                    (void)BluetoothLEHost_Connect(&gConnReqParams, BleApp_ConnectionCallback);
                }
                else
                {
                    AppPrintString("Scan stopped.\r\n");
                    LedStopFlashingAllLeds();
                    LedStartFlashingAllLeds();
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
    if (pConnectionEvent->eventType == gConnEvtPeripheralSecurityRequest_c)
    {
        /* If the sensor requets pairing with bonding act as Primary Monitor, otherwise as Secondary Monitor */
        gPairingParameters.withBonding = pConnectionEvent->eventData.peripheralSecurityRequestEvent.bondAfterPairing;
    }

    /* Connection Manager to handle Host Stack interactions */
    BleConnManager_GapCentralEvent(peerDeviceId, pConnectionEvent);

    switch (pConnectionEvent->eventType)
    {
        case gConnEvtConnected_c:
        {
            /* Update UI */
            LedStopFlashingAllLeds();
            Led1On();

            AppPrintString("Connected!\r\n");

            maPeerInformation[peerDeviceId].deviceId = peerDeviceId;
            maPeerInformation[peerDeviceId].isBonded = FALSE;
            maCharValue[peerDeviceId] = MEM_BufferAlloc(16U); /* to be freed on disconnect */
            if (maCharValue[peerDeviceId] == NULL)
            {
                /* No memory for GATT operations */
                (void)Gap_Disconnect(peerDeviceId);
            }

            BleApp_StateMachineHandler(maPeerInformation[peerDeviceId].deviceId, mAppEvt_PeerConnected_c);
        }
        break;

        case gConnEvtDisconnected_c:
        {
            maPeerInformation[peerDeviceId].deviceId = gInvalidDeviceId_c;
            maPeerInformation[peerDeviceId].appState = mAppIdle_c;
            (void)MEM_BufferFree(maCharValue[peerDeviceId]);
            maCharValue[peerDeviceId] = NULL;

            /* Reset Service Discovery to be sure*/
            BleServDisc_Stop(peerDeviceId);

            AppPrintString("Disconnected with reason ");
            AppPrintDec((uint16_t)pConnectionEvent->eventData.disconnectedEvent.reason);
            AppPrintString("!\r\n");

            LedTurnOffAllLeds();
            LedStartFlashingAllLeds();
        }
        break;

        case gConnEvtPairingComplete_c:
        {
            /* Notify state machine handler on pairing complete */
            if (pConnectionEvent->eventData.pairingCompleteEvent.pairingSuccessful)
            {
                maPeerInformation[peerDeviceId].isBonded = TRUE;
                mAuthRejected = FALSE;

                BleApp_StateMachineHandler(peerDeviceId, mAppEvt_PairingComplete_c);
                AppPrintString("\r\n-->  GAP Event: Device Paired.\r\n");
            }
            else
            {
                 maPeerInformation[peerDeviceId].isBonded = FALSE;
                 AppPrintString("\r\n-->  GAP Event: Pairing Unsuccessful.\r\n");
            }
        }
        break;

        case gConnEvtEncryptionChanged_c:
        {
            if( pConnectionEvent->eventData.encryptionChangedEvent.newEncryptionState )
            {
                if ( (TRUE == mRestoringBondedLink) &&
                     (FALSE == mAuthRejected) )
                {
                    maPeerInformation[peerDeviceId].isBonded = TRUE;
                    BleApp_StateMachineHandler(maPeerInformation[peerDeviceId].deviceId, mAppEvt_LinkEncrypted_c);
                }
            }
        }
        break;

        case gConnEvtAuthenticationRejected_c:
        {
            mAuthRejected = TRUE;
            /* Start Pairing Procedure */
            (void)Gap_Pair(peerDeviceId, &gPairingParameters);
        }
        break;

    default:
        ; /* No action required */
        break;
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
* \brief        Stores handles for the specified service.
*
* \param[in]    pService    Pointer to gattService_t.
********************************************************************************** */
static void BleApp_StoreServiceHandles
(
    deviceId_t      peerDeviceId,
    gattService_t   *pService
)
{
    uint8_t i,j;

    if ((pService->uuidType == gBleUuidType16_c) &&
        (pService->uuid.uuid16 == gBleSig_TirePressureMonitoringService_d))
    {
        /* Found TPMS Service */
       maPeerInformation[peerDeviceId].tpmsMonitorConfig.hService = pService->startHandle;

        for (i = 0; i < pService->cNumCharacteristics; i++)
        {
            if (pService->aCharacteristics[i].value.uuidType == gBleUuidType16_c)
            {
               switch (pService->aCharacteristics[i].value.uuid.uuid16)
               {
                   case gBleSig_TirePressure_d:
                   {
                        /* Found Tire Pressure Char */
                       maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTirePressure = pService->aCharacteristics[i].value.handle;

                        for (j = 0; j < pService->aCharacteristics[i].cNumDescriptors; j++)
                        {
                            if (pService->aCharacteristics[i].aDescriptors[j].uuidType == gBleUuidType16_c)
                            {
                                if (pService->aCharacteristics[i].aDescriptors[j].uuid.uuid16 == gBleSig_CCCD_d)
                                {
                                    /* Found Tire Pressure Char CCCD */
                                   maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTirePressureCccd = pService->aCharacteristics[i].aDescriptors[j].handle;
                                }
                            }
                        }
                   }
                   break;

                   case gBleSig_TireTemperature_d:
                   {
                        /* Found Tire Temperature Char */
                       maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTireTemperature = pService->aCharacteristics[i].value.handle;

                        for (j = 0; j < pService->aCharacteristics[i].cNumDescriptors; j++)
                        {
                            if (pService->aCharacteristics[i].aDescriptors[j].uuidType == gBleUuidType16_c)
                            {
                                if (pService->aCharacteristics[i].aDescriptors[j].uuid.uuid16 == gBleSig_CCCD_d)
                                {
                                    /* Found Tire Temperature Char CCCD */
                                   maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTireTemperatureCccd = pService->aCharacteristics[i].aDescriptors[j].handle;
                                }
                            }
                        }
                   }
                   break;

                   case gBleSig_TireAcceleration_d:
                   {
                        /* Found Tire Acceleration Char */
                       maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTireAcceleration = pService->aCharacteristics[i].value.handle;
                   }
                   break;

                   case gBleSig_TpmsProperties_d:
                   {
                        /* Found TPMS Properties Char */
                       maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTpmsProperties = pService->aCharacteristics[i].value.handle;
                   }
                   break;

                   case gBleSig_TpmsMonitoringModeDutyCycle_d:
                   {
                       /* Found TPMS Monitoring Duty Cycle Char */
                       maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTpmsMonitoringDutyCycle = pService->aCharacteristics[i].value.handle;
                   }
                   break;

                   case gBleSig_TpmsPosition_d:
                   {
                        /* Found TPMS Position Char */
                       maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTpmsPosition = pService->aCharacteristics[i].value.handle;
                   }
                   break;

                   case gBleSig_TpmsSigningKey_d:
                   {
                        /* Found TPMS Signing Key Char */
                       maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTpmsSigningKey = pService->aCharacteristics[i].value.handle;

                        for (j = 0; j < pService->aCharacteristics[i].cNumDescriptors; j++)
                        {
                            if (pService->aCharacteristics[i].aDescriptors[j].uuidType == gBleUuidType16_c)
                            {
                                if (pService->aCharacteristics[i].aDescriptors[j].uuid.uuid16 == gBleSig_CCCD_d)
                                {
                                    /* Found TPMS Signing Key Char CCCD */
                                   maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTpmsSigningKeyCccd = pService->aCharacteristics[i].aDescriptors[j].handle;
                                }
                            }
                        }
                   }
                   break;

                   default:
                   {
                       ; /* Nothing to do */
                   }
                   break;
                }
            }
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
static void BleApp_GattClientCallback(
    deviceId_t              serverDeviceId,
    gattProcedureType_t     procedureType,
    gattProcedureResult_t   procedureResult,
    bleResult_t             error
)
{
    uint16_t value = gCccdIndication_c;

    /* Do not process GATT Server messages unless a trusted relationship was established */
    if ((maPeerInformation[serverDeviceId].isBonded) || (maPeerInformation[serverDeviceId].appState != mAppRunning_c))
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
                switch(procedureType)
                {
                    case gGattProcReadCharacteristicValue_c:
                    {
                        if (mpCharacteristic.value.uuid.uuid16 == gBleSig_TpmsProperties_d)
                        {
                            /* Retain information about signing algorithm used by sensor - HMAC or CMAC */
                            maPeerInformation[serverDeviceId].customInfo.usesHmac = ((mpCharacteristic.value.paValue[0] & BIT1) != 0U) ? TRUE : FALSE;

                            AppPrintString("TPMS Properties read from Sensor - signing uses ");
                            if (maPeerInformation[serverDeviceId].customInfo.usesHmac == TRUE)
                            {
                                AppPrintString("HMAC.\r\n");
                            }
                            else
                            {
                                AppPrintString("CMAC.\r\n");
                            }

                            /* Read TPMS Signing Key */
                            mpCharacteristic.value.handle = maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTpmsSigningKey;
                            mpCharacteristic.value.uuidType = gBleUuidType16_c;
                            mpCharacteristic.value.uuid.uuid16 = gBleSig_TpmsSigningKey_d;
                            mpCharacteristic.value.paValue = maCharValue[serverDeviceId];
                            
                            FLib_MemSet(mpCharacteristic.value.paValue, 0U, 16U);
                            (void)GattClient_ReadCharacteristicValue(serverDeviceId,
                                                                    &mpCharacteristic,
                                                                    16U);
                        }
                        else if (mpCharacteristic.value.uuid.uuid16 == gBleSig_TpmsSigningKey_d)
                        {
                            AppPrintString("TPMS Signing Key read from Sensor.\r\n");

                            /* Save TPMS Signing Key */
                            FLib_MemCpy(maPeerInformation[serverDeviceId].customInfo.tpmsSigningKey, mpCharacteristic.value.paValue, 16U);
                            /* Also in NVM */
                            (void)Gap_SaveCustomPeerInformation(maPeerInformation[serverDeviceId].deviceId,
                                                                (void *)&maPeerInformation[serverDeviceId].customInfo, 0U,
                                                                (uint16_t)sizeof(appCustomInfo_t));
                            if (maPeerInformation[serverDeviceId].isBonded == TRUE)
                            {
                                /* Act as Primary Monitor */
                                if (maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTpmsMonitoringDutyCycle != gGattDbInvalidHandle_d)
                                {
                                    /* Write Duty Cycle */
                                    mpCharacteristic.value.handle = maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTpmsMonitoringDutyCycle;
                                    mpCharacteristic.value.uuidType = gBleUuidType16_c;
                                    mpCharacteristic.value.uuid.uuid16 = gBleSig_TpmsMonitoringModeDutyCycle_d;
                                    mpCharacteristic.value.paValue = maCharValue[serverDeviceId];

                                    FLib_MemSet(mpCharacteristic.value.paValue, 0U, 16U);
                                    mpCharacteristic.value.paValue[0] = 200U;  /* Advertising state duration (increments of 10ms) */
                                    mpCharacteristic.value.paValue[1] = 10U;   /* Standby state duration in Parked state (seconds) - byte1 */
                                    mpCharacteristic.value.paValue[2] = 0U;    /* Standby state duration in Parked state (seconds) - byte2 */
                                    mpCharacteristic.value.paValue[3] = 10U;   /* Standby state duration in Early Driving state (seconds) - byte1 */
                                    mpCharacteristic.value.paValue[4] = 0U;    /* Standby state duration in Early Driving state (seconds) - byte2 */
                                    mpCharacteristic.value.paValue[5] = 10U;   /* Standby state duration in Driving state (seconds) - byte2 */
                                    mpCharacteristic.value.paValue[6] = 0U;    /* Standby state duration in Driving state (seconds) - byte2 */
                                    mpCharacteristic.value.paValue[7] = 10U;   /* Standby state duration in Early Parked state (seconds) - byte2 */
                                    mpCharacteristic.value.paValue[8] = 0U;    /* Standby state duration in Early Parked state (seconds) - byte2 */
                                    mpCharacteristic.value.paValue[9] = 1U;    /* Early Driving state duration (minutes) */
                                    mpCharacteristic.value.paValue[10] = 1U;   /* Early Driving Primary Monitor Pairing state duration (minutes) */
                                    mpCharacteristic.value.paValue[11] = 1U;   /* Early Driving Parked state duration (minutes) */
                                    mpCharacteristic.value.paValue[12] = 30U;  /*! Service State duration (seconds) */
                                    (void)GattClient_WriteCharacteristicValue(serverDeviceId, &mpCharacteristic, 13U, mpCharacteristic.value.paValue, FALSE, FALSE, FALSE, NULL);
                                }
                                else
                                {
                                    /* Enable indications for TPMS Signing Key changes */
                                    value = gCccdIndication_c;
                                    mpCharProcBuffer.handle = maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTpmsSigningKeyCccd;
                                    mpCharProcBuffer.uuidType = gBleUuidType16_c;
                                    mpCharProcBuffer.uuid.uuid16 = gBleSig_CCCD_d;
                                    mpCharProcBuffer.valueLength = 1U;
                                    mLastWrittenHandle = maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTpmsSigningKeyCccd;
                                    (void)GattClient_WriteCharacteristicDescriptor(serverDeviceId,
                                                                                   &mpCharProcBuffer,
                                                                                   (uint16_t)sizeof(value),
                                                                                   (void*)&value); 
                                }
                            }
                            else
                            {
                                /* Act as Secondary Monitor. Read values */
                                BleApp_ReadPressureTemperatureAcceleration(serverDeviceId);
                            }
                        }
                        else
                        {
                            /* MISRA */
                        }
                    }
                    break;

                    case gGattProcWriteCharacteristicValue_c:
                    {
                        if (mpCharacteristic.value.uuid.uuid16 == gBleSig_TpmsMonitoringModeDutyCycle_d)
                        {
                            AppPrintString("TPMS Duty Cycle updated.\r\n");
                            /* Enable indications for TPMS Signing Key changes */
                            value = gCccdIndication_c;
                            mpCharProcBuffer.handle = maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTpmsSigningKeyCccd;
                            mpCharProcBuffer.uuidType = gBleUuidType16_c;
                            mpCharProcBuffer.uuid.uuid16 = gBleSig_CCCD_d;
                            mpCharProcBuffer.valueLength = 1U;
                            mLastWrittenHandle = maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTpmsSigningKeyCccd;
                            (void)GattClient_WriteCharacteristicDescriptor(serverDeviceId,
                                                                           &mpCharProcBuffer,
                                                                           (uint16_t)sizeof(value),
                                                                           (void*)&value);
                        }
                    }
                    break;

                    case gGattProcWriteCharacteristicDescriptor_c:
                    {
                        if (mLastWrittenHandle == maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTpmsSigningKeyCccd)
                        {
                            AppPrintString("TPMS Signing Key indications enabled.\r\n");
                            /* Enable notifications for Tire Pressure changes */
                            value = gCccdNotification_c;
                            mpCharProcBuffer.handle = maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTirePressureCccd;
                            mpCharProcBuffer.uuidType = gBleUuidType16_c;
                            mpCharProcBuffer.uuid.uuid16 = gBleSig_CCCD_d;
                            mpCharProcBuffer.valueLength = 1U;
                            mLastWrittenHandle = maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTirePressureCccd;
                            (void)GattClient_WriteCharacteristicDescriptor(serverDeviceId,
                                                                           &mpCharProcBuffer,
                                                                           (uint16_t)sizeof(value),
                                                                           (void*)&value);
                        }
                        else if (mLastWrittenHandle == maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTirePressureCccd)
                        {
                            AppPrintString("Tire Pressure notifications enabled.\r\n");
                            /* Enable notifications for Tire Temperature changes */
                            value = gCccdNotification_c;
                            if (maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTireTemperatureCccd != gGattDbInvalidHandle_d)
                            {
                                mpCharProcBuffer.handle = maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTireTemperatureCccd;
                                mpCharProcBuffer.uuidType = gBleUuidType16_c;
                                mpCharProcBuffer.uuid.uuid16 = gBleSig_CCCD_d;
                                mpCharProcBuffer.valueLength = 1U;
                                mLastWrittenHandle = maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTireTemperatureCccd;
                                (void)GattClient_WriteCharacteristicDescriptor(serverDeviceId,
                                                                               &mpCharProcBuffer,
                                                                               (uint16_t)sizeof(value),
                                                                               (void*)&value);
                            }
                            else
                            {
                                /* If the optional Temperature characteristic is not supported, move to reading values */
                                BleApp_ReadPressureTemperatureAcceleration(serverDeviceId);
                            }
                        }
                        else if (mLastWrittenHandle == maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTireTemperatureCccd)
                        {
                            AppPrintString("Tire Temperature notifications enabled.\r\n");
                            /* Finished enabling notifications/indications, move to reading values */
                            BleApp_ReadPressureTemperatureAcceleration(serverDeviceId);
                        }
                        else
                        {
                            /* MISRA */
                        }
                    }
                    break;

                    case gGattProcReadMultipleCharacteristicValues_c:
                    {
                        AppPrintString("Pressure read: ");
                        AppPrintDec(Utils_ExtractTwoByteValue(maPressureValue));
                        AppPrintString(" Accuracy: ");
                        AppPrintDec(maPressureValue[2]);
                        AppPrintString("\r\nTemperature read: ");
                        AppPrintDec(maTemperatureValue[0]);
                        AppPrintString(" Accuracy: ");
                        AppPrintDec(maPressureValue[2]);
                        AppPrintString("\r\nAcceleration read: ");
                        AppPrintDec(Utils_ExtractTwoByteValue(maAccelerationValue));
                    }
                    break;

                    default:
                    {
                        ; /* No action required */

                    }
                    break;
                }

                BleApp_StateMachineHandler(serverDeviceId, mAppEvt_GattProcComplete_c);
            }
        }

        /* Signal Service Discovery Module */
        BleServDisc_SignalGattClientEvent(serverDeviceId, procedureType, procedureResult, error);
    }
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
    deviceId_t          serverDeviceId,
    uint16_t characteristicValueHandle,
    uint8_t* aValue,
    uint16_t valueLength
)
{
    /* Do not process GATT Server notifications unless a trusted relationship was established */
    if (maPeerInformation[serverDeviceId].isBonded == TRUE)
    {
        if (characteristicValueHandle == maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTirePressure)
        {
            uint16_t pressure = Utils_ExtractTwoByteValue(aValue);
            uint8_t accuracy = aValue[2];
            AppPrintString("\r\nReceived Tire Pressure notification: ");
            AppPrintDec((uint16_t)pressure);
            AppPrintString(" Accuracy: ");
            AppPrintDec((uint16_t)accuracy);
        }
        if (characteristicValueHandle == maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTireTemperature)
        {
            uint8_t temperature = aValue[0];
            uint8_t accuracy = aValue[1];
            AppPrintString("\r\nReceived Tire Temperature notification: ");
            AppPrintDec((uint16_t)temperature);
            AppPrintString(" Accuracy: ");
            AppPrintDec((uint16_t)accuracy);
        }
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
    AppPrintString("\r\nReceived changed signed key indication.");
    
    /* Do not process GATT Server notifications unless a trusted relationship was established */
    if (maPeerInformation[serverDeviceId].isBonded == TRUE)
    {
        if (characteristicValueHandle == maPeerInformation[serverDeviceId].tpmsMonitorConfig.hTpmsSigningKey)
        {
            /* Save TPMS Signing Key */
            FLib_MemCpy(maPeerInformation[serverDeviceId].customInfo.tpmsSigningKey, aValue, 16U);
            /* Also in NVM */
            (void)Gap_SaveCustomPeerInformation(maPeerInformation[serverDeviceId].deviceId,
                                                (void *)&maPeerInformation[serverDeviceId].customInfo, 0U,
                                                (uint16_t)sizeof(appCustomInfo_t));
        }
    }
}

/*! *********************************************************************************
* \brief        Detect whether the provided data is found in the advertising data.
*
* \param[in]    pElement                    Pointer a to AD structure.
* \param[in]    pData                       Data to look for.
* \param[in]    iDataLen                    Size of data to look for.
*
* \return       TRUE if data matches, FALSE if not
********************************************************************************** */
static bool_t MatchDataInAdvElementList(gapAdStructure_t *pElement, void *pData, uint8_t iDataLen)
{
    uint32_t i;
    bool_t status = FALSE;

    for (i = 0; i < (uint32_t)pElement->length - 1UL; i += iDataLen)
    {
        /* Compare input data with advertising data. */
        if (FLib_MemCmp(pData, &pElement->aData[i], iDataLen))
        {
            status = TRUE;
            break;
        }
    }
    return status;
}

/*! *********************************************************************************
* \brief        Process non-connectable scan events from a bonded TPMS sensor.
*
* \param[in]    pData                   Pointer to gapScannedDevice_t.
*
* \return       None
********************************************************************************** */
static void ProcessNonConnScanEvent(gapScannedDevice_t* pData)
{
    gapAdStructure_t adElement;
    uint8_t tpmsSigningKey[16] = {0U};
    uint8_t keyReversed[16] = {0U};
    bool_t bFound = FALSE;
    uint32_t index = 0;
    uint8_t optionalFieldsLen = 0U;

    while (index < pData->dataLength)
    {
        adElement.length = pData->data[index];
        adElement.adType = (gapAdType_t)pData->data[index + 1U];
        adElement.aData = &pData->data[index + 2U];

        if(adElement.adType == gAdServiceData16bit_c)
        {
            /* Check optional fields */
            if (adElement.length > 16U)
            {
                optionalFieldsLen = adElement.length - 16U;
            }

            /* Parse indicators and optional fields */
            ParseIndicatorsAndOptionalFields(adElement.aData, optionalFieldsLen);

            /* Verify MAC */
            uint8_t hmac_output[32] = {0};
            uint8_t computedMac[4] = {0};
            for (uint8_t i = 0; i < (uint8_t)gMaxBondedDevices_c; i++)
            {
                bool_t bUsesHmac = FALSE;

                (void)Gap_LoadCustomBondedDeviceInformation(i, &bUsesHmac, 0U, 1U);
                (void)Gap_LoadCustomBondedDeviceInformation(i, tpmsSigningKey, sizeof(uint32_t) + sizeof(uint32_t), 16U); /* account for padding */
                FLib_MemCpyReverseOrder(keyReversed, tpmsSigningKey, 16);
                if (bUsesHmac == TRUE)
                {
                    HMAC_SHA256(keyReversed, 16U, (const uint8_t*)adElement.aData, (11U + (uint32_t)optionalFieldsLen), hmac_output);
                }
                else
                {
                    AES_128_CMAC((const uint8_t*)adElement.aData, (11U + (uint32_t)optionalFieldsLen), keyReversed, hmac_output);
                }

                FLib_MemCpyReverseOrder(computedMac, &hmac_output, 4U);

                if (FLib_MemCmp(&adElement.aData[11U + optionalFieldsLen], computedMac, 4U) == TRUE)
                {
                    uint16_t pressure = Utils_ExtractTwoByteValue((uint8_t*)&adElement.aData[2]);
                    uint8_t accuracy = adElement.aData[4];
                    AppPrintString("MAC check OK. Tire Pressure: ");
                    AppPrintDec(pressure);
                    AppPrintString(" Accuracy: ");
                    AppPrintDec(accuracy);
                    AppPrintString("\r\n");
                    bFound = TRUE;
                    break;
                }
            }

            if (bFound == FALSE)
            {
                AppPrintString("\nMAC check failed.\r\n");
            }
        }
        else
        {
            AppPrintString("Other AD\r\n");
        }
        index += (uint32_t)adElement.length + sizeof(uint8_t);
    }

    /* Reset flag to keep checking non-connectable advertising reports */
    mFoundDeviceToConnect = FALSE;
}

/*! *********************************************************************************
* \brief        Process scanning events to search for the TPM Service.
*               This function is called from the scanning callback.
*
* \param[in]    pData                   Pointer to gapScannedDevice_t.
*
* \return       TRUE if the scanned device implements the TPM Service,
                FALSE otherwise
********************************************************************************** */
static bool_t CheckScanEvent(gapScannedDevice_t* pData)
{
    uint32_t index = 0;
    char name[11];
    uint8_t nameLength = 0;
    bool_t foundMatch = FALSE;

    while (index < pData->dataLength)
    {
        gapAdStructure_t adElement;

        adElement.length = pData->data[index];
        adElement.adType = (gapAdType_t)pData->data[index + 1U];
        adElement.aData = &pData->data[index + 2U];

         /* Search for TPMS Service */
        if (adElement.adType == gAdServiceData16bit_c)
        {
            uint16_t uuid = gBleSig_TirePressureMonitoringService_d;
            foundMatch = MatchDataInAdvElementList(&adElement, &uuid, (uint8_t)sizeof(uint16_t));
        }

        if ((adElement.adType == gAdShortenedLocalName_c) ||
          (adElement.adType == gAdCompleteLocalName_c))
        {
            nameLength = MIN(adElement.length, 10U);
            FLib_MemCpy(name, adElement.aData, nameLength);
        }

        /* Move on to the next AD element type */
        index += (uint32_t)adElement.length + sizeof(uint8_t);
    }

    if (foundMatch && (nameLength > 0U))
    {
        /* Update UI */
        AppPrintString("Found device: \r\n");
        name[nameLength-1U] = '\0';
        AppPrintString((const char*)name);
        AppPrintString("\r\n");
    }
    return foundMatch;
}

/*! *********************************************************************************
* \brief        State machine handler of the TPMS Monitor application for mAppRunning_c state.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
********************************************************************************** */
static void BleApp_StateMachineHandlerRunning(deviceId_t peerDeviceId, appEvent_t event)
{

}
/*! *********************************************************************************
* \brief        State machine handler of the TPMS Monitor application for mAppEstablishSecurity_c state.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
********************************************************************************** */
static void BleApp_StateMachineHandleEstablishSecurity(deviceId_t peerDeviceId, appEvent_t event)
{
    if (event == mAppEvt_PairingComplete_c || event == mAppEvt_LinkEncrypted_c)
    {
        maPeerInformation[peerDeviceId].appState = mAppRunning_c;
  
        /* Read TPMS Properties */
        mpCharacteristic.value.handle =maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTpmsProperties;
        mpCharacteristic.value.uuidType = gBleUuidType16_c;
        mpCharacteristic.value.uuid.uuid16 = gBleSig_TpmsProperties_d;
        mpCharacteristic.value.paValue = maCharValue[peerDeviceId];

        FLib_MemSet(mpCharacteristic.value.paValue, 0U, 16U);
        (void)GattClient_ReadCharacteristicValue(peerDeviceId,
                                                &mpCharacteristic,
                                                16U);
    }
}
/*! *********************************************************************************
* \brief        State machine handler of the TPMS Monitor application for mAppServiceDisc_c state.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
********************************************************************************** */
static void BleApp_StateMachineHandlerServiceDisc(deviceId_t peerDeviceId, appEvent_t event)
{
    if (event == mAppEvt_ServiceDiscoveryComplete_c)
    {
        bleResult_t result;
        bool_t isBonded = FALSE;

        (void)Gap_CheckIfBonded(peerDeviceId, &isBonded, NULL);
        result = Gap_LoadCustomPeerInformation(peerDeviceId,
                (void*) &maPeerInformation[peerDeviceId].customInfo, 0U, (uint16_t)sizeof(appCustomInfo_t));

        if (isBonded && (result == gBleSuccess_c)) 
        {
            mRestoringBondedLink = TRUE;
            /* Restored custom connection information. Encrypt link */
            (void)Gap_EncryptLink(peerDeviceId);
        }
        else
        {
            mRestoringBondedLink = FALSE;
            mAuthRejected = FALSE;
            (void)Gap_Pair(peerDeviceId, &gPairingParameters);
        }
        maPeerInformation[peerDeviceId].appState = mAppEstablishSecurity_c;
    }
    else
    {
        if (event == mAppEvt_ServiceDiscoveryFailed_c)
        {
            (void)Gap_Disconnect(peerDeviceId);
        }
    }
}
/*! *********************************************************************************
* \brief        State machine handler of the TPMS Monitor application for mAppExchangeMtu_c state.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
********************************************************************************** */
static void BleApp_StateMachineHandlerExchangeMtu(deviceId_t peerDeviceId, appEvent_t event)
{
    if (event == mAppEvt_GattProcComplete_c)
    {
        /* Moving to Service Discovery State*/
        maPeerInformation[peerDeviceId].appState = mAppServiceDisc_c;

        /* Start Service Discovery*/
        (void)BleServDisc_Start(peerDeviceId);
    }
    else
    {
        if (event == mAppEvt_GattProcError_c)
        {
            (void)Gap_Disconnect(peerDeviceId);
        }
    }
}
/*! *********************************************************************************
* \brief        State machine handler of the TPMS Monitor application for mAppIdle_c state.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
********************************************************************************** */
static void BleApp_StateMachineHandlerIdle(deviceId_t peerDeviceId, appEvent_t event)
{
    if (event == mAppEvt_PeerConnected_c)
    {
        /* Moving to Exchange MTU State */
        maPeerInformation[peerDeviceId].appState = mAppExchangeMtu_c;
        (void)GattClient_ExchangeMtu(peerDeviceId, gAttMaxMtu_c);
    }
}

/*! *********************************************************************************
* \brief        State machine handler of the TPMS Monitor application.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
********************************************************************************** */
static void BleApp_StateMachineHandler(deviceId_t peerDeviceId, appEvent_t event)
{
    switch (maPeerInformation[peerDeviceId].appState)
    {
    case mAppIdle_c:
        BleApp_StateMachineHandlerIdle(peerDeviceId, event);
        break;

    case mAppExchangeMtu_c:
        BleApp_StateMachineHandlerExchangeMtu(peerDeviceId, event);
        break;

    case mAppServiceDisc_c:
        BleApp_StateMachineHandlerServiceDisc(peerDeviceId, event);
        break;

    case mAppEstablishSecurity_c:
        BleApp_StateMachineHandleEstablishSecurity(peerDeviceId, event);
        break;

    case mAppRunning_c:
        BleApp_StateMachineHandlerRunning(peerDeviceId, event);
        break;

    default:
        {
            ; /* No action required */
        }
        break;
    }
}

/*! *********************************************************************************
* \brief        Prints a string.
*
********************************************************************************** */
static void AppPrintString( const char* pBuff)
{
    uint32_t buffLength = strlen(pBuff);
    union{
        const char* pcBuff;
        uint8_t* pBuff;
    }buff;
    buff.pcBuff = pBuff;
    (void)SerialManager_WriteBlocking((serial_write_handle_t)s_writeHandle, buff.pBuff, buffLength );
}

/*! *********************************************************************************
* \brief        Prints a number in decimal.
*
********************************************************************************** */
static void AppPrintDec(uint16_t dec)
{
    uint8_t *pDec;
    pDec = FORMAT_Dec2Str(dec);
    (void)SerialManager_WriteBlocking((serial_write_handle_t)s_writeHandle, pDec, strlen((char const *)pDec));
}

/*! *********************************************************************************
* \brief        Initializes serial interface.
*
********************************************************************************** */
static void BleApp_SerialInit(void)
{
    serial_manager_status_t status;

    /* UI */
    gAppSerMgrIf = (serial_handle_t)&gSerMgrIf[0];

    /*open wireless uart write handle*/
    status = SerialManager_OpenWriteHandle((serial_handle_t)gAppSerMgrIf, (serial_write_handle_t)s_writeHandle);
    assert(kStatus_SerialManager_Success == status);
    (void)status;
}

/*! *********************************************************************************
* \brief        Reads values from connected TPMS Sensor.
*
********************************************************************************** */
static void BleApp_ReadPressureTemperatureAcceleration(deviceId_t peerDeviceId)
{
    maChars[0].value.handle = maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTirePressure;
    maChars[1].value.handle = maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTireTemperature;
    maChars[2].value.handle = maPeerInformation[peerDeviceId].tpmsMonitorConfig.hTireAcceleration;
    maChars[0].value.maxValueLength = 3;
    maChars[1].value.maxValueLength = 2;
    maChars[2].value.maxValueLength = 2;
    maChars[0].value.paValue = maPressureValue;
    maChars[1].value.paValue = maTemperatureValue;
    maChars[2].value.paValue = maAccelerationValue;

    (void)GattClient_ReadMultipleCharacteristicValues(maPeerInformation[peerDeviceId].deviceId,
                                                      mcNumChars_c,
                                                      maChars);
}

/*! *********************************************************************************
* \brief        Parse service data fields from TPMS Sensor.
*
********************************************************************************** */
static void ParseIndicatorsAndOptionalFields(uint8_t *pData, uint8_t optionalFieldsLen)
{
    uint8_t indicators, indicatorsExt1, indicatorsExt2 = 0U;
    uint8_t pDataIndex = 6U; /* index of mandatory Indicators field */
    uint8_t optLength, optType;

    indicators = pData[pDataIndex];

    AppPrintString("Indicators: ");
    AppPrintDec(indicators);
    AppPrintString("\r\n");

    if ((indicators & BIT7) != 0U)
    {
        pDataIndex++;
        optionalFieldsLen--;
        indicatorsExt1 = pData[pDataIndex];
        AppPrintString("Indicators_EXT1: ");
        AppPrintDec(indicatorsExt1);
        AppPrintString("\r\n");
        if ((indicatorsExt1 & BIT7) != 0U)
        {
            pDataIndex++;
            optionalFieldsLen--;
            indicatorsExt2 = pData[pDataIndex];
            AppPrintString("Indicators_EXT2: ");
            AppPrintDec(indicatorsExt2);
            AppPrintString("\r\n");
        }
    }

    uint8_t i = pDataIndex + 1U;
    while (i < (pDataIndex + optionalFieldsLen))
    {
        optType = pData[i] & 0x0FU;
        optLength = (pData[i] & 0xF0U) >> 4U;
        switch (optType)
        {
            case 1:
            {
                AppPrintString("\r\nOption Type: Temperature Accuracy // Option Length: ");
            }
            break;

            case 2:
            {
                AppPrintString("\r\nOption Type: Position // Option Length: ");
            }
            break;

            case 3:
            {
                AppPrintString("\r\nOption Type: Angle Data // Option Length: ");
            }
            break;

            case 4:
            {
                AppPrintString("\r\nOption Type: Acceleration Data // Option Length: ");
            }
            break;

            default:
            {
                AppPrintString("\r\nOption Type: RFU // Option Length: ");
            }
            break;
        }

        AppPrintDec(optLength);
        AppPrintString("\r\nOption Value: ");
        for(uint8_t j = 0; j < optLength; j++)
        {
            AppPrintDec(pData[i+1U+j]);
        }
        AppPrintString("\r\n");
        i += (1U + optLength); 
    }
}
/*! *********************************************************************************
* @}
********************************************************************************** */
