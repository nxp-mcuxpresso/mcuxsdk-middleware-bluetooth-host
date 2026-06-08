/*! *********************************************************************************
* \addtogroup TPMS Sensor
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2026 NXP
*
*
* \file
*
* This file is the source file for the TPMS Sensor application
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
#include "fsl_component_timer_manager.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_panic.h"
#include "fsl_component_serial_manager.h"
#include "fsl_format.h"
#include "fsl_adapter_reset.h"
#include "FunctionLib.h"
#if defined(gAppUseSensors_d) && (gAppUseSensors_d > 0U)
#include "sensors.h"
#endif /* defined(gAppUseSensors_d) && (gAppUseSensors_d > 0U) */
#include "NVM_Interface.h"

/* BLE Host Stack */
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gap_interface.h"
#include "gatt_db_handles.h"

/* Profile / Services */
#include "battery_interface.h"
#include "device_info_interface.h"
#include "tpm_interface.h"

/* Connection Manager */
#include "ble_conn_manager.h"

#include "board.h"
#include "app.h"
#include "app_conn.h"
#include "app_advertiser.h"
#include "tpms_sensor.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
/* NVM dataset identifier for the application-owned TPMS Signing Key */
#define nvmId_TpmsSigningKeyId_c        0x4101

/* NVM dataset identifier for the application-owned TPMS advertising sequence number */
#define nvmId_TpmsSeqNumId_c            0x4102

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef enum advMode_tag{
    appAdvIdle,
    appAdvMonitorMode,
    appAdvConnectedMode
} advMode_t;

typedef struct advState_tag{
    bool_t      advOn;
    advMode_t   advMode;
}advState_t;

typedef enum appBtnAction_tag {
    appBtnAction_MonitorModeToggle_c,
    appBtnAction_TirePressureNotify_c,
    appBtnAction_ConnectedModeToggle_c,
    appBtnAction_UpdateSigningKey_c,
    appBtnAction_FactoryReset_c
} appBtnAction_t;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
/* Gatt and Att callbacks */
static void BleApp_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent);
static void BleApp_ConnectionCallback (deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent);
static void BleApp_GattServerCallback (deviceId_t deviceId, gattServerEvent_t* pServerEvent);

/* Timer Callbacks */
static void AdvertisingTimerCallback(void *pParam);

static void BleApp_Advertise(void);
static void AppPrintString( const char* pBuff);
static void AppPrintDec(uint32_t dec);

static void BluetoothLEHost_Initialized(void);
static void BleApp_SerialInit(void);
static void BluetoothLEHost_GenericCallback (gapGenericEvent_t* pGenericEvent);
static void BleApp_HandleMonitorModeToggle(void *pData);
static void BleApp_HandleTirePressureNotify(void *pData);
static void BleApp_HandleConnectedModeToggle(void *pData);
static void BleApp_HandleUpdateSigningKey(void *pData);
static void BleApp_HandleFactoryReset(void *pData);
static void BleApp_PostButtonAction(appBtnAction_t action);
static void BleApp_Start(void);
static void BleApp_ReadSensorData(tpmsSensorReadData_t *pSensorReadData);
static void BleApp_SaveSigningKey(const uint8_t *pKey, uint16_t keyLength);
static void BleApp_SaveSeqNum(uint32_t seqNum);
button_status_t BleApp_HandleKeys0(void *pButtonHandle, button_callback_message_t *pMessage, void *pCallbackParam);
button_status_t BleApp_HandleKeys1(void *pButtonHandle, button_callback_message_t *pMessage, void *pCallbackParam);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
/* Adv State */
static advState_t   mAdvState;
static bool_t       mConnected;

/* Service Data*/
static bool_t           basValidClientList[gAppMaxConnections_c] = {FALSE};
static basConfig_t      basServiceConfig = {(uint16_t)service_battery, 0, basValidClientList, gAppMaxConnections_c};
static disConfig_t      disServiceConfig = {(uint16_t)service_device_info};
static bool_t           tpmValidClientList[gAppMaxConnections_c] = {FALSE};
static tpmConfig_t      tpmServiceConfig = {(uint16_t)service_tpms, tpmValidClientList, gAppMaxConnections_c, BleApp_SaveSigningKey, BleApp_SaveSeqNum};

/* Application-owned NVM storage for the TPMS Signing Key */
static uint8_t          maSigningKey[gTpmsSigningKeyLength_c] = {0U};
NVM_RegisterDataSet(maSigningKey,
                    1U,
                    (uint16_t)sizeof(maSigningKey),
                    nvmId_TpmsSigningKeyId_c,
                    (uint16_t)gNVM_MirroredInRam_c);

/* Application-owned NVM storage for the TPMS advertising sequence number */
static uint32_t         mSeqNum = 0U;
NVM_RegisterDataSet(&mSeqNum,
                    1U,
                    (uint16_t)sizeof(mSeqNum),
                    nvmId_TpmsSeqNumId_c,
                    (uint16_t)gNVM_MirroredInRam_c);


/* Application specific data*/
static TIMER_MANAGER_HANDLE_DEFINE(appTimerId);

static serial_handle_t gAppSerMgrIf;
static SERIAL_MANAGER_WRITE_HANDLE_DEFINE(s_writeHandle);

static appAdvertisingParams_t mAppAdvParams = {
    &gAdvParams,
    &gAppAdvertisingData,
    &gAppScanRspData
};

static uint16_t mCharWriteMonitoredHandles[2] = { (uint16_t)value_tpms_monitoring_duty_cycle,
                                                  (uint16_t)value_tpms_position };
static uint16_t mCharReadMonitoredHandles[4] = { (uint16_t)value_tire_pressure, 
                                                 (uint16_t)value_tire_temperature, 
                                                 (uint16_t)value_tire_acceleration, 
                                                 (uint16_t)value_tpms_signing_key };

static uint8_t mState = (uint8_t)gParkedState_c;

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
    /* Initialize application support for drivers */
    BleApp_SerialInit();
    LedStartFlashingAllLeds();
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[0], BleApp_HandleKeys0, NULL);
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[1], BleApp_HandleKeys1, NULL);

    /* Initialize Bluetooth Host Stack */
    BluetoothLEHost_SetGenericCallback(BluetoothLEHost_GenericCallback);
    BluetoothLEHost_Init(BluetoothLEHost_Initialized);
}

/*! *********************************************************************************
* \brief        Handler for the first key.
*
* \param[in]    pButtonHandle       Pointer to the button handle.
* \param[in]    pMessage            Pointer to the message.
* \param[in]    pCallbackParam      Pointer to the callback parameters.
********************************************************************************** */
button_status_t BleApp_HandleKeys0(void *pButtonHandle, button_callback_message_t *pMessage, void *pCallbackParam)
{
    (void)pButtonHandle;
    (void)pCallbackParam;
    switch (pMessage->event)
    {
        case kBUTTON_EventOneClick:
        case kBUTTON_EventShortPress:
        {
            BleApp_PostButtonAction(appBtnAction_MonitorModeToggle_c);
        }
        break;
        
        /* Trigger notification for Tire Pressure (and Tire Temperature) */
        case kBUTTON_EventLongPress:
        {
            BleApp_PostButtonAction(appBtnAction_TirePressureNotify_c);
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
        case kBUTTON_EventOneClick:
        case kBUTTON_EventShortPress:
        {
            BleApp_PostButtonAction(mConnected ? 
                                    appBtnAction_UpdateSigningKey_c : 
                                    appBtnAction_ConnectedModeToggle_c);   
        }
        break;

        /* Factory reset on long button press */
        case kBUTTON_EventLongPress:
        {
            BleApp_PostButtonAction(appBtnAction_FactoryReset_c);
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
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief    Starts the BLE application.
*
********************************************************************************** */
static void BleApp_Start(void)
{
    Led1On();

    if (mAdvState.advMode != appAdvIdle)
    {
        /* Advertising to be started on the application task */
       (void)App_PostCallbackMessage(APP_AdvertiseCommandHandler, NULL);
    }
    else
    {
        /* Stop advertising */
        (void)TM_Stop(appTimerId);

        if (mAdvState.advOn == TRUE)
        {
            (void)Gap_StopAdvertising();
        }
    }
}

/*!*************************************************************************************************
 \fn     uint8_t APP_AdvertiseCommandHandler(void *pData)
 \brief  This function is used to handle the start advertising command.

 \param  [in]   pData - pointer to data;
 ***************************************************************************************************/
void APP_AdvertiseCommandHandler(void *pData)
{
    BleApp_Advertise();
}

/*! *********************************************************************************
* \brief        Configures BLE Stack after initialization. Usually used for
*               configuring advertising, scanning, filter accept list, services, et al.
*
********************************************************************************** */
static void BluetoothLEHost_Initialized(void)
{
    /* Common GAP configuration */
    BleConnManager_GapCommonConfig();

    /* Register for callbacks*/
    (void)App_RegisterGattServerCallback(BleApp_GattServerCallback);
    (void)GattServer_RegisterHandlesForWriteNotifications(NumberOfElements(mCharWriteMonitoredHandles), mCharWriteMonitoredHandles);
    (void)GattServer_RegisterHandlesForReadNotifications(NumberOfElements(mCharReadMonitoredHandles), mCharReadMonitoredHandles);
    
    mAdvState.advOn = FALSE;

#if (defined(gAppUseSensors_d) && (gAppUseSensors_d == 1))
    basServiceConfig.batteryLevel = SENSORS_GetBatteryLevel();
#endif
    (void)Bas_Start(&basServiceConfig);
    (void)Dis_Start(&disServiceConfig);
    Tpms_Start(&tpmServiceConfig);

    /* Restore a previously persisted Signing Key from NVM */
    if (gNVM_OK_c == NvRestoreDataSet((void*)maSigningKey, TRUE))
    {
        (void)Tpms_SetSigningKey(&tpmServiceConfig, maSigningKey, gTpmsSigningKeyLength_c);
    }

    /* Restore a previously persisted advertising sequence number from NVM.
       The sequence number is only saved once every gTpmsSeqNumSaveInterval_c
       updates, so advance the restored value by that amount to ensure a value
       is never reused with the same signing key after a reset. */
    if (gNVM_OK_c == NvRestoreDataSet((void*)&mSeqNum, TRUE))
    {
        Tpms_SetSeqNum(mSeqNum + gTpmsSeqNumSaveInterval_c);
    }

    /* Allocate application timer */
    (void)TM_Open(appTimerId);

    AppPrintString("\r\nTPMS sensor -> Press switch to start advertising.\r\n");
}

/*! *********************************************************************************
* \brief        Configures GAP Advertise parameters. Advertise will start after
*               the parameters are set.
*
********************************************************************************** */
static void BleApp_Advertise(void)
{
    tpmsSensorReadData_t sensorReadData = {};

    if ((gcBondedDevices == 0U) || (mAdvState.advMode == appAdvConnectedMode))
    {
        /* Connectable advertising to establish bond with Primary Monitor or as configured */
        mAppAdvParams.pGapAdvParams->advertisingType = gAdvConnectableUndirected_c;
        mAppAdvParams.pGapAdvParams->minInterval = 0x800; /* recommended spec value: 1.28s */
        mAppAdvParams.pGapAdvParams->maxInterval = 0x800; /* recommended spec value: 1.28s */
    }
    else
    {
        mAppAdvParams.pGapAdvParams->advertisingType = gAdvNonConnectable_c;
        mAppAdvParams.pGapAdvParams->minInterval = 32; /* recommended spec value: 20ms */
        mAppAdvParams.pGapAdvParams->maxInterval = 32; /* recommended spec value: 20ms */
    }

    BleApp_ReadSensorData(&sensorReadData);
    Tpms_UpdateAdvData(mAppAdvParams.pGapAdvData->aAdStructures[1].aData, &sensorReadData);

    /* Start advertising */
    (void)BluetoothLEHost_StartAdvertising(&mAppAdvParams, BleApp_AdvertisingCallback, BleApp_ConnectionCallback);
}

/*! *********************************************************************************
* \brief        Handles BLE generic callback.
*
* \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
********************************************************************************** */
static void BluetoothLEHost_GenericCallback (gapGenericEvent_t* pGenericEvent)
{
    /* Call BLE Conn Manager */
    BleConnManager_GenericEvent(pGenericEvent);

    switch (pGenericEvent->eventType)
    {
        case gAdvertisingSetupFailed_c:
        {
            panic(0,0,0,0);
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
* \brief        Handles BLE Advertising callback from host stack.
*
* \param[in]    pAdvertisingEvent    Pointer to gapAdvertisingEvent_t.
********************************************************************************** */
static void BleApp_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent)
{
    switch (pAdvertisingEvent->eventType)
    {
        case gAdvertisingStateChanged_c:
        {
            mAdvState.advOn = !mAdvState.advOn;

            if(!mAdvState.advOn)
            {
                AppPrintString("Advertising stopped.\r\n");

                if (mAdvState.advMode == appAdvMonitorMode)
                {
                    /* Start standby timer */
                    (void)TM_InstallCallback((timer_handle_t)appTimerId, AdvertisingTimerCallback, NULL);
                    (void)TM_Start((timer_handle_t)appTimerId, (uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSetSecondTimer, Tpms_GetStandbyDuration(mState));
                }

                /* UI */
                LedStopFlashingAllLeds();
                LedStartFlashingAllLeds();
            }
            else
            {
                AppPrintString("Advertising started.\r\n");

                if (mAdvState.advMode == appAdvMonitorMode)
                {
                    /* Start advertising timer */
                    (void)TM_InstallCallback((timer_handle_t)appTimerId, AdvertisingTimerCallback, NULL);
                    (void)TM_Start((timer_handle_t)appTimerId, (uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSetMicrosTimer, (uint32_t)Tpms_GetAdvDuration() * 10000U);
                }

                /* UI */
                LedStopFlashingAllLeds();
                Led1Flashing();
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
    if (pConnectionEvent->eventType == gConnEvtConnected_c)
    {
        if (gcBondedDevices == 1U)
        {
            /* We are bonded with a Primary Monitor, all subsequent pairings
            with Secondary Monitors will be performed without bonding. */
            gPairingParameters.withBonding = FALSE;
        }
    }

    /* Connection Manager to handle Host Stack interactions */
    BleConnManager_GapPeripheralEvent(peerDeviceId, pConnectionEvent);

    switch (pConnectionEvent->eventType)
    {
        case gConnEvtConnected_c:
        {
            /* Advertising stops when connected */
            mAdvState.advOn = FALSE;
            mAdvState.advMode = appAdvIdle;
            (void)TM_Stop((timer_handle_t)appTimerId);
            
            mConnected = TRUE;

            /* Subscribe client*/
            (void)Bas_Subscribe(&basServiceConfig, peerDeviceId);
            (void)Tpms_Subscribe(&tpmServiceConfig, peerDeviceId);

            AppPrintString("Connected!\r\n");
            LedStopFlashingAllLeds();

            /* UI */
            Led1On();
        }
        break;

        case gConnEvtDisconnected_c:
        {
            mConnected = FALSE;
            
            /* Unsubscribe client */
            (void)Bas_Unsubscribe(&basServiceConfig, peerDeviceId);
            (void)Tpms_Unsubscribe(&tpmServiceConfig, peerDeviceId);

            AppPrintString("Disconnected with reason ");
            AppPrintDec((uint32_t)pConnectionEvent->eventData.disconnectedEvent.reason);
            AppPrintString("!\r\n");
            /* Link loss reconnection procedure */
            if (pConnectionEvent->eventData.disconnectedEvent.reason == gHciConnectionTimeout_c)
            {
                mAdvState.advMode = appAdvConnectedMode;
                BleApp_Advertise();
            }
        }
        break;

        case gConnEvtPairingComplete_c:
        {
            /* Notify state machine handler on pairing complete */
            if (pConnectionEvent->eventData.pairingCompleteEvent.pairingSuccessful)
            {
                AppPrintString("Device paired.\r\n");
            }
            else
            {
                 AppPrintString("Pairing unsuccessful.\r\n");
            }
        }
        break;

        case gBondCreatedEvent_c:
        {
            AppPrintString("Bond created.\r\n");
            /* Signing Key must change when a new Primary Monitor is established */
            Tpms_UpdateSigningKey(&tpmServiceConfig);
        }
        break;

        case gConnEvtEncryptionChanged_c:   /* Fall-through */
        {
            AppPrintString("Encryption completed.\r\n");
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
* \brief        Handles GATT server callback from host stack.
*
* \param[in]    deviceId        Peer device ID.
* \param[in]    pServerEvent    Pointer to gattServerEvent_t.
********************************************************************************** */
static void BleApp_GattServerCallback (deviceId_t deviceId, gattServerEvent_t* pServerEvent)
{
    switch (pServerEvent->eventType)
    {
        case gEvtAttributeWritten_c:
        {
            Tpms_HandleAttributeWritten(deviceId, pServerEvent);
        }
        break;

        case gEvtAttributeRead_c:
        {
            Tpms_HandleAttributeRead(deviceId, pServerEvent);
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
* \brief        Stops advertising when the application timeout has expired.
*
* \param[in]    pParam        Callback parameters.
********************************************************************************** */
static void AdvertisingTimerCallback(void* pParam)
{
    /* Stop advertising */
    if (mAdvState.advOn)
    {
        (void)Gap_StopAdvertising();
    }
    else
    {
        (void)App_PostCallbackMessage(APP_AdvertiseCommandHandler, NULL);
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
static void AppPrintDec(uint32_t dec)
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
* \brief        Retrieves data from sensors (tire pressure, tire temperature).
*
********************************************************************************** */
static void BleApp_ReadSensorData(tpmsSensorReadData_t *pSensorReadData)
{
    /* 
       Application can use this function to read the actual data from sensors
       Dummy values used for the demo
    */
    pSensorReadData->tirePressure = 456U;
    pSensorReadData->tirePressureAccuracy = 2U;
    pSensorReadData->tireTemperature = 25U;
    pSensorReadData->tireTemperatureAccuracy = 1U;
}

/*! *********************************************************************************
* \brief        Posts a button action to be executed on the application task.
*
* \param[in]    action    The button action to execute.
********************************************************************************** */
static void BleApp_PostButtonAction(appBtnAction_t action)
{
    switch (action)
    {
        case appBtnAction_MonitorModeToggle_c:
        {
            (void)App_PostCallbackMessage(BleApp_HandleMonitorModeToggle, NULL);
        }
        break;

        case appBtnAction_TirePressureNotify_c:
        {
            (void)App_PostCallbackMessage(BleApp_HandleTirePressureNotify, NULL);
        }
        break;

        case appBtnAction_ConnectedModeToggle_c:
        {
            (void)App_PostCallbackMessage(BleApp_HandleConnectedModeToggle, NULL);
        }
        break;

        case appBtnAction_UpdateSigningKey_c:
        {
            (void)App_PostCallbackMessage(BleApp_HandleUpdateSigningKey, NULL);
        }
        break;

        case appBtnAction_FactoryReset_c:
        {
            (void)App_PostCallbackMessage(BleApp_HandleFactoryReset, NULL);
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
* \brief        Handles monitor mode toggle action on the application task.
*
* \param[in]    pData    Pointer to data (unused).
********************************************************************************** */
static void BleApp_HandleMonitorModeToggle(void *pData)
{
    (void)pData;
    
    if (mAdvState.advMode == appAdvIdle)
    {
        /* Start advertising in monitoring mode */
        mAdvState.advMode = appAdvMonitorMode;
        AppPrintString("Configured advertising for Monitoring Mode.\r\n");
        BleApp_Start();
    }
    else if (mAdvState.advMode == appAdvMonitorMode)
    {
        /* Stop advertising */
        mAdvState.advMode = appAdvIdle;
        AppPrintString("Stop advertising.\r\n");
        BleApp_Start();
    }
    else
    {
        ; /* Do nothing */
    }
}

/*! *********************************************************************************
* \brief        Handles tire pressure notify action on the application task.
*
* \param[in]    pData    Pointer to data (unused).
********************************************************************************** */
static void BleApp_HandleTirePressureNotify(void *pData)
{
    (void)pData;
    
    tpmsSensorReadData_t sensorReadData = {};
    BleApp_ReadSensorData(&sensorReadData);
    Tpms_NotifyTirePressure(&tpmServiceConfig, &sensorReadData);
}

/*! *********************************************************************************
* \brief        Handles connected mode toggle action on the application task.
*
* \param[in]    pData    Pointer to data (unused).
********************************************************************************** */
static void BleApp_HandleConnectedModeToggle(void *pData)
{
    (void)pData;
    
    if (mAdvState.advMode == appAdvIdle)
    {
        /* Start advertising in connected mode */
        mAdvState.advMode = appAdvConnectedMode;
        AppPrintString("Configured advertising for Connected Mode.\r\n");
        BleApp_Start();
    }
    else if (mAdvState.advMode == appAdvConnectedMode)
    {
        /* Stop advertising */
        mAdvState.advMode = appAdvIdle;
        AppPrintString("Stop advertising.\r\n");
        BleApp_Start();
    }
    else
    {
        /* MISRA */
    }
}

/*! *********************************************************************************
* \brief        Handles update signing key action on the application task.
*
* \param[in]    pData    Pointer to data (unused).
********************************************************************************** */
static void BleApp_HandleUpdateSigningKey(void *pData)
{
    (void)pData;
    
    AppPrintString("Updating signing key.\r\n");
    Tpms_UpdateSigningKey(&tpmServiceConfig);
}

/*! *********************************************************************************
* \brief        Persists the TPMS Signing Key into NVM. Registered with the TPM
*               service as the signingKeyChangedCb and invoked whenever the key
*               changes (button press, new bond, sequence number rollover, or a
*               write from the Primary Monitor).
*
* \param[in]    pKey        Pointer to the new Signing Key bytes.
* \param[in]    keyLength   Length of the key in bytes.
********************************************************************************** */
static void BleApp_SaveSigningKey(const uint8_t *pKey, uint16_t keyLength)
{
    if ((pKey != NULL) && (keyLength == gTpmsSigningKeyLength_c))
    {
        FLib_MemCpy(maSigningKey, pKey, gTpmsSigningKeyLength_c);
        (void)NvSaveOnIdle((void*)maSigningKey, FALSE);
    }
}

/*! *********************************************************************************
* \brief        Persists the TPMS advertising sequence number into NVM. Registered
*               with the TPM service as the seqNumSaveCb and invoked periodically
*               (once every gTpmsSeqNumSaveInterval_c updates) rather than on every
*               update, to limit NVM wear.
*
* \param[in]    seqNum    The current sequence-number value to persist.
********************************************************************************** */
static void BleApp_SaveSeqNum(uint32_t seqNum)
{
    mSeqNum = seqNum;
    (void)NvSaveOnIdle((void*)&mSeqNum, FALSE);
}

/*! *********************************************************************************
* \brief        Handles factory reset action on the application task.
*
* \param[in]    pData    Pointer to data (unused).
********************************************************************************** */
static void BleApp_HandleFactoryReset(void *pData)
{
    (void)pData;
    
    /* Erase NVM Datasets */
    NVM_Status_t status = NvFormat();
    if (status != gNVM_OK_c)
    {
        /* NvFormat exited with an error status */ 
        panic(0, (uint32_t)BleApp_HandleFactoryReset, 0, 0);
    }

    /* Reset MCU */
    HAL_ResetMCU();
}
/*! *********************************************************************************
* @}
********************************************************************************** */
