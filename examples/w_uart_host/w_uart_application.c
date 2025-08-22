/*! *********************************************************************************
* Copyright 2022-2025 NXP
*
*
* \file hsdk_main.c
*
* This is the main source file for the HSDK module
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
/* Framework / Drivers */
#include "fsl_component_button.h"
#include "fsl_component_timer_manager.h"
#include "fsl_component_panic.h"
#include "fsl_component_mem_manager.h"
#include "fsl_format.h"
#include "app.h"

#include "host_hsdk_interface.h"
#include "host_cmd_ble.h"

/* BLE Host Stack */
#include "gap_types.h"
#include "gatt_interface.h"
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gatt_database.h"
#include "gap_interface.h"
#include "gatt_db_app_interface.h"

/* Profile / Services */
#include "host_battery_interface.h"

/* Application headers */
#include "w_uart_application.h"
#include "host_ble_conn_manager.h"
#include "host_ble_init.h"

#if defined(MCXW727C_cm33_core0_SERIES)
#include "sensors.h"
#endif

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

#define mAppUartBufferSize_c            gAttMaxWriteDataSize_d(gAttMaxMtu_c) /* Local Buffer Size */

#define mAppUartFlushIntervalInMs_c     (7)     /* Flush Timeout in Ms */

#define mBatteryLevelReportInterval_c   (10)    /* Battery level report interval in seconds  */

#define gAllowToBlock_d                 (TRUE)
#define gNoBlock_d                      (FALSE)
#define Serial_Print(a,b)               do{ \
                                            union \
                                            { \
                                                const char *pStr; \
                                                uint8_t *pUint8; \
                                            } temp; \
                                            temp.pStr = (a); \
                                            if ((b) == gAllowToBlock_d) \
                                            { \
                                                (void)SerialManager_WriteBlocking((serial_write_handle_t)s_writeHandle, (uint8_t *)temp.pUint8, strlen(temp.pStr)); \
                                            } \
                                            else \
                                            { \
                                                (void)SerialManager_WriteNonBlocking((serial_write_handle_t)s_writeHandle, (uint8_t *)temp.pUint8, strlen(temp.pStr)); \
                                            } \
                                        } while(0);
#define Serial_PrintDec(a)              (void)SerialManager_WriteBlocking((serial_write_handle_t)s_writeHandle, FORMAT_Dec2Str(a), strlen((char const *)FORMAT_Dec2Str(a)))

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

typedef struct advState_tag
{
    bool_t advOn;
} advState_t;

/************************************************************************************
 *************************************************************************************
 * Private functions prototypes
 *************************************************************************************
 ************************************************************************************/

/*! *********************************************************************************
 * \brief    Starts the BLE application.
 *
 * \param[in]    gapRole            GAP Start Role (Central or Peripheral).
 ********************************************************************************** */
static void BleApp_Start
(
    gapRole_t gapRole
);

#if gWuart_PeripheralRole_c == 1
/*! *********************************************************************************
* \brief        Handles BLE Advertising event state changed from host stack.
*
********************************************************************************** */
static void BleApp_AdvertisingEvtStateChangedHandler
(
    void
);
#endif /* gWuart_PeripheralRole_c */

/*! *********************************************************************************
 * \brief        Function handling the initialization complete event of the Bluetooth LE Host stack.
 *
 ********************************************************************************** */
static void BleApp_GenericEvtInitCompleteHandler
(
    void
);

/*! *********************************************************************************
 * \brief        Timer handler for flushing the UART.
 *
 * \param[in]    pData              Pointer to the parameters.
 ********************************************************************************** */
static void UartStreamFlushTimerCallback
(
    void *pData
);

/*! *********************************************************************************
* \brief        Handles battery measurement timer callback.
*
* \param[in]    pParam        Callback parameters.
********************************************************************************** */
static void BatteryMeasurementTimerCallback
(
    void *pParam
);

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c == 1))
/*! *********************************************************************************
 * \brief        Handles the switch press timer callback.
 *
 * \param[in]    pParam             Callback parameters.
 ********************************************************************************** */
static void SwitchPressTimerCallback
(
    void *pParam
);
#endif

/*! *********************************************************************************
* \brief        Handles UART Receive callback.
*
* \param[in]    pData        Unused pointer to data.
* \param[in]    pMessage     Unused pointer to message.
* \param[in]    status       Unused status.
********************************************************************************** */
static void Uart_RxCallBack
(
    void *pData,
    serial_manager_callback_message_t *pMessage,
    serial_manager_status_t status
);
/*! *********************************************************************************
* \brief        Handles UART Transmit callback.
*
* \param[in]    pBuffer      Pointer to the sent data.
* \param[in]    pMessage     Unused pointer to message.
* \param[in]    status       Unused status.
********************************************************************************** */
static void Uart_TxCallBack
(
    void *pBuffer,
    serial_manager_callback_message_t *pMessage,
    serial_manager_status_t status
);

/*! *********************************************************************************
 * \brief        Get all bytes from the serial interface and send it over GATT.
 *
 * \param[in]    pParam             Callback parameters.
 ********************************************************************************** */
static void BleApp_FlushUartStream
(
    void *pParam
);

/*! *********************************************************************************
 * \brief        Write bytes to serial.
 *
 * \param[in]    peerDeviceId       The remote device ID.
 * \param[in]    pStream            Pointer to the received stream.
 * \param[in]    streamLength       Number of bytes in the strem.
 ********************************************************************************** */
static void BleApp_ReceivedUartStream
(
    deviceId_t peerDeviceId,
    uint8_t *pStream,
    uint16_t streamLength
);

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
);
#endif /*gAppButtonCnt_c > 0*/

#if (gWuart_CentralRole_c == 1) && (gWuart_PeripheralRole_c == 1)
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
);
#endif
#endif

/*! *********************************************************************************
 * \brief        Heleper Function to register removable observer.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void hsdkObserverGATTClientRegisterProcedureCallback(bleEvtContainer_t *pContainer);

/*! *********************************************************************************
 * \brief        Heleper Function to register removable observer.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void hsdkObserverGATTClientRegisterNotificationCallback(bleEvtContainer_t *pContainer);

/*! *********************************************************************************
 * \brief        Heleper Function to register removable observer.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void hsdkObserverGATTClientRegisterIndicationCallback(bleEvtContainer_t *pContainer);

/*! *********************************************************************************
 * \brief        Heleper Function to register removable observer.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void hsdkObserverGATTServerRegisterCallbackRequest(bleEvtContainer_t *pContainer);

#if defined(gUseControllerNotifications_c) && (gUseControllerNotifications_c)
/*! *********************************************************************************
 * \brief        Heleper Function to register removable observer.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void hsdkObserverGAPControllerEnhancedNotificationRequest(bleEvtContainer_t *pContainer);
#endif

/*! *********************************************************************************
 * \brief        Heleper Function to start scanning procedure.
 *
 ********************************************************************************** */
static void GapStartScanningRequest
(
    void
);

/*! *********************************************************************************
 * \brief        Heleper Function to stop scanning procedure.
 *
 ********************************************************************************** */
static void BleApp_GapStopScanningRequest
(
    void *pParam
);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
extern servDiscInfo_t maServDiscInfo[gAppMaxConnections_c];

/* Role of the device */
gapRole_t mGapRole;

/* Counter of the active connections */
static uint8_t mcActiveConnNo;

/* Application specific information */
appPeerInfo_t maPeerInformation[gAppMaxConnections_c];

/* Service Data*/
static bool_t      mBasValidClientList[gAppMaxConnections_c] = {FALSE};
static basConfig_t mBasServiceConfig = {(uint16_t)mBatteryServiceHandle_c, 0, mBasValidClientList, gAppMaxConnections_c};

/* Timers used by the application */
static TIMER_MANAGER_HANDLE_DEFINE(mAppTimerId);
static TIMER_MANAGER_HANDLE_DEFINE(mUartStreamFlushTimerId);
static TIMER_MANAGER_HANDLE_DEFINE(mBatteryMeasurementTimerId);

/* If the board has only one button, multiplex the required functionalities on it using an application timer */
#if (gAppButtonCnt_c == 1)
static TIMER_MANAGER_HANDLE_DEFINE(mSwitchPressTimerId);
#endif

static uint16_t mAppUartBufferSize = mAppUartBufferSize_c;
static volatile bool_t mAppUartNewLine = FALSE;
static volatile bool_t mAppDapaPending = FALSE;

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c == 1))
static uint8_t mSwitchPressCnt = 0;
#endif

/* Adv Parameters */
#if gWuart_PeripheralRole_c == 1
static advState_t mAdvState;
#endif /* gWuart_PeripheralRole_c */

uint8_t uuid_service_wireless_uart[16] = {0xE0, 0x1C, 0x4B, 0x5E, 0x1E, 0xEB, 0xA1, 0x5C, 0xEE, 0xF4, 0x5E, 0xBA, 0x00, 0x01, 0xFF, 0x01};
uint8_t uuid_uart_stream[16] = {0xE0, 0x1C, 0x4B, 0x5E, 0x1E, 0xEB, 0xA1, 0x5C, 0xEE, 0xF4, 0x5E, 0xBA, 0x01, 0x01, 0xFF, 0x01};

static bool_t mScanningOn = FALSE;
static bool_t mInitiatingConnection = FALSE;

/* wireless uart write handle */
static SERIAL_MANAGER_WRITE_HANDLE_DEFINE(s_writeHandle);
/* wireless uart read handle */
static SERIAL_MANAGER_READ_HANDLE_DEFINE(s_readHandle);

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
/* This is the last device id used to indetify the last request GAPCheckIfBonded */
static deviceId_t mLastCheckIfBondedDeviceId = gInvalidDeviceId_c;
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
static bool_t mDataLengthChangeLocallyInitiated = FALSE;
#endif /* defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U) */
#endif /* defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U) */

static bool_t mGattCallbacksInitialized = FALSE;

/* This is the last device id used to indetify the last request GATT Get MTU */
static deviceId_t mLastGetMtuDeviceId = gInvalidDeviceId_c;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
/* Helper functions */
static void BleApp_StoreServiceHandles
(
    deviceId_t peerDeviceId,
    gattService_t *pService
);

static void ScanningTimerCallback
(
    void *pParam
);

static bool_t checkScanEvent
(
    gapScannedDevice_t *pData
);

static bool_t matchDataInAdvElementList
(
    gapAdStructure_t *pElement,
    void             *pData,
    uint8_t          iDataLen
);

#if defined(gUseControllerNotifications_c) && (gUseControllerNotifications_c)
static void BleApp_HandleControllerNotification
(
    bleEvtContainer_t* pMsg
);
#endif
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
 * \brief        Function used to setup the serial interface.
 *
 ********************************************************************************** */
void Shell_Init
(
    void
)
{
    serial_manager_status_t status;
    /*wireless uart serial manager handle*/
    static serial_handle_t appSerMgrIf;

    /* UI */
    appSerMgrIf = (serial_handle_t)&gSerMgrIf[0];

    /*open wireless uart read/write handle*/
    status = SerialManager_OpenWriteHandle((serial_handle_t)appSerMgrIf, (serial_write_handle_t)s_writeHandle);
    assert(kStatus_SerialManager_Success == status);
    (void)status;

    status = SerialManager_OpenReadHandle((serial_handle_t)appSerMgrIf, (serial_read_handle_t)s_readHandle);
    assert(kStatus_SerialManager_Success == status);
    status = SerialManager_InstallRxCallback((serial_read_handle_t)s_readHandle, Uart_RxCallBack, NULL);
    assert(kStatus_SerialManager_Success == status);
}

/*! *********************************************************************************
* \brief        Handle BLE events. Helper function.
*
* \param[in]    pMsg              Pointer to the event revceived.
********************************************************************************** */
static void BleApp_EventCallback3
(
    bleEvtContainer_t *pMsg
)
{
    deviceId_t deviceId = gInvalidDeviceId_c;

    switch (pMsg->id)
    {
        case GATTServerAttributeWrittenWithoutResponseIndication_FSCI_ID:
        {
            /* Check for the wireless uart stream value handle */
            if (pMsg->Data.GATTServerAttributeWrittenWithoutResponseIndication.AttributeWrittenEvent.Handle == mWUartServiceHandle_c + 2U)
            {
                BleApp_ReceivedUartStream(pMsg->Data.GATTServerAttributeWrittenWithoutResponseIndication.DeviceId,
                                          pMsg->Data.GATTServerAttributeWrittenWithoutResponseIndication.AttributeWrittenEvent.Value,
                                          pMsg->Data.GATTServerAttributeWrittenWithoutResponseIndication.AttributeWrittenEvent.ValueLength);
            }
        }
        break;

        case GATTClientProcedureDiscoverAllPrimaryServicesIndication_FSCI_ID:
        case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_FSCI_ID:
        case GATTClientProcedureFindIncludedServicesIndication_FSCI_ID:
        case GATTClientProcedureDiscoverAllCharacteristicsIndication_FSCI_ID:
        case GATTClientProcedureDiscoverCharacteristicByUuidIndication_FSCI_ID:
        case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_FSCI_ID:
        case GATTClientProcedureReadCharacteristicValueIndication_FSCI_ID:
        {
            /* deviceId is at the same position in the union */
            deviceId = pMsg->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication.DeviceId;
            BleApp_StateMachineHandler(deviceId, mAppEvt_GattProcComplete_c);
        }
        break;

        case GATTClientProcedureWriteCharacteristicValueIndication_FSCI_ID:
        {
            /* deviceId is at the same position in the union */
            deviceId = pMsg->Data.GATTClientProcedureWriteCharacteristicValueIndication.DeviceId;

            if ((pMsg->Data.GATTClientProcedureWriteCharacteristicValueIndication.ProcedureResult == 
                 GATTClientProcedureWriteCharacteristicValueIndication_ProcedureResult_gProcedureError_c) &&
                (pMsg->Data.GATTClientProcedureWriteCharacteristicValueIndication.Error == 
                 GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattConnectionSecurityRequirementsNotMet_c)
                )
            {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
                if (maPeerInformation[deviceId].gapRole == gGapCentral_c)
                {
                    /* Start Pairing Procedure in central role
                     * local bond could be lost */
                    GAPPairRequest_t req;
                    req.DeviceId = deviceId;
                    req.PairingParameters.WithBonding = gPairingParameters.withBonding;
                    req.PairingParameters.SecurityModeAndLevel = GAPPairRequest_PairingParameters_SecurityModeAndLevel_gMode1Level3_c;
                    req.PairingParameters.MaxEncryptionKeySize = mcEncryptionKeySize_c;
                    req.PairingParameters.LocalIoCapabilities = GAPPairRequest_PairingParameters_LocalIoCapabilities_gIoKeyboardDisplay_c;
                    req.PairingParameters.OobAvailable = gPairingParameters.oobAvailable;
                    req.PairingParameters.CentralKeys = gPairingParameters.centralKeys;
                    req.PairingParameters.PeripheralKeys = gPairingParameters.peripheralKeys;
                    req.PairingParameters.LeSecureConnectionSupported = gPairingParameters.leSecureConnectionSupported;
                    req.PairingParameters.UseKeypressNotifications = gPairingParameters.useKeypressNotifications;

                    (void)GAPPairRequest(&req, gFsciInterface_c);
                }
                else if (maPeerInformation[deviceId].gapRole == gGapPeripheral_c)
                {
                    /* Send Security Request in peripheral role
                     * peer bond could be lost */
                    GAPSendPeripheralSecurityRequestRequest_t req;
                    req.DeviceId = deviceId;
                    req.PairingParameters.WithBonding = gPairingParameters.withBonding;
                    req.PairingParameters.SecurityModeAndLevel = GAPSendPeripheralSecurityRequestRequest_PairingParameters_SecurityModeAndLevel_gMode1Level3_c;
                    req.PairingParameters.MaxEncryptionKeySize = gPairingParameters.maxEncryptionKeySize;
                    req.PairingParameters.LocalIoCapabilities =
                      GAPSendPeripheralSecurityRequestRequest_PairingParameters_LocalIoCapabilities_gIoDisplayOnly_c;
                    req.PairingParameters.OobAvailable = gPairingParameters.oobAvailable;
                    req.PairingParameters.CentralKeys = gPairingParameters.centralKeys;
                    req.PairingParameters.PeripheralKeys = gPairingParameters.peripheralKeys;
                    req.PairingParameters.LeSecureConnectionSupported = FALSE;
                    req.PairingParameters.UseKeypressNotifications = FALSE;

                    (void)GAPSendPeripheralSecurityRequestRequest(&req, gFsciInterface_c);
                }
                else
                {
                    /* No action required */
                }
#endif /* gAppUsePairing_d */
            }
            else
            {
                BleApp_StateMachineHandler(deviceId, mAppEvt_GattProcError_c);
            }
        }
        break;

        case GATTServerErrorIndication_FSCI_ID:
        {
            BleApp_StateMachineHandler(pMsg->Data.GATTServerErrorIndication.DeviceId, mAppEvt_GattProcError_c);
        }
        break;

        case GAPGenericEventInternalErrorIndication_FSCI_ID:
        {
            if((pMsg->Data.GAPGenericEventInternalErrorIndication.ErrorCode == GAPGenericEventInternalErrorIndication_ErrorCode_gBleOverflow_c) && 
               (pMsg->Data.GAPGenericEventInternalErrorIndication.ErrorSource == GAPGenericEventInternalErrorIndication_ErrorSource_gAddNewConnection_c))
            {
                Serial_Print("Connection failed ", gAllowToBlock_d);
#if gWuart_PeripheralRole_c == 1
                if(mAdvState.advOn)
                {
                    mAdvState.advOn = FALSE;
                    Serial_Print(" as peripheral.\n\r", gAllowToBlock_d);
                }
                else
#endif /* gWuart_PeripheralRole_c == 1 */
                {
#if gWuart_CentralRole_c == 1
                    Serial_Print(" as central.\n\r", gAllowToBlock_d);
#endif /* gWuart_CentralRole_c == 1 */

                }
            }
        }
        break;

#if defined(gUseControllerNotifications_c) && (gUseControllerNotifications_c)
        case GAPControllerNotificationIndication_FSCI_ID:
        {
            BleApp_HandleControllerNotification(pMsg);
        }
        break;
#endif
        case GAPAdvertisingEventCommandFailedIndication_FSCI_ID:
        {
            panic(0, 0, 0, 0);
        }
        break;

        case GAPScanningEventCommandFailedIndication_FSCI_ID:
        {
            panic(0, 0, 0, 0);
        }
        break;

        default:
        {
            ;
        }
        break;
    }
}
/*! *********************************************************************************
* \brief        Handle BLE events. Helper function.
*
* \param[in]    pMsg              Pointer to the event revceived.
********************************************************************************** */
static void BleApp_EventCallback2
(
    bleEvtContainer_t *pMsg
)
{
    uint16_t tempMtu = 0;
    union
    {
        uint8_t     *pUuidArray;
        bleUuid_t   *pUuidObj;
    } temp; /* MISRA rule 11.3 */

    temp.pUuidArray = uuid_service_wireless_uart;

    switch (pMsg->id)
    {
        case GAPConnectionEventDisconnectedIndication_FSCI_ID:
        {
            deviceId_t peerDeviceId = pMsg->Data.GAPConnectionEventDisconnectedIndication.DeviceId;
            Serial_Print("Disconnected from device ", gAllowToBlock_d);
            Serial_PrintDec(peerDeviceId);
            Serial_Print(" with reason ", gAllowToBlock_d);
            Serial_PrintDec((uint32_t)pMsg->Data.GAPConnectionEventDisconnectedIndication.Reason);
            Serial_Print(".\n\r", gAllowToBlock_d);

            maPeerInformation[peerDeviceId].appState = mAppIdle_c;
            maPeerInformation[peerDeviceId].clientInfo.hService = gGattDbInvalidHandleIndex_d;
            maPeerInformation[peerDeviceId].clientInfo.hUartStream = gGattDbInvalidHandleIndex_d;

            /* Unsubscribe client */
            (void)Bas_Unsubscribe(&mBasServiceConfig, peerDeviceId);

            /* Reset Service Discovery to be sure*/
            BleServDisc_Stop(peerDeviceId);

            /* UI */
            LedStartFlashingAllLeds();

            /* mark device id as invalid */
            maPeerInformation[peerDeviceId].deviceId = gInvalidDeviceId_c;
            mcActiveConnNo--;
            if(mcActiveConnNo == 0U)
            {
                (void)TM_Stop((timer_handle_t)mBatteryMeasurementTimerId);
            }
            peerDeviceId = gInvalidDeviceId_c;

            mAppUartNewLine = TRUE;

            if (mGapRole == gGapPeripheral_c)
            {
                BleApp_Start(mGapRole);
            }
        }
        break;

        case GATTClientProcedureExchangeMtuIndication_FSCI_ID:
        {
            BleApp_StateMachineHandler(pMsg->Data.GATTClientProcedureExchangeMtuIndication.DeviceId,
                                       mAppEvt_GattProcComplete_c);

        }
        break;

        case GATTGetMtuIndication_FSCI_ID:
        {
            tempMtu = pMsg->Data.GATTGetMtuIndication.Mtu;

            if (tempMtu >= gAttDefaultMtu_c)
            {
                tempMtu = gAttMaxWriteDataSize_d(tempMtu);

                mAppUartBufferSize = mAppUartBufferSize <= tempMtu ? mAppUartBufferSize : tempMtu;

                /* Moving to Service Discovery State*/
                maPeerInformation[mLastGetMtuDeviceId].appState = mAppServiceDisc_c;

                /* Start Service Discovery*/
                (void)BleServDisc_FindService(mLastGetMtuDeviceId,
                                            gBleUuidType128_c,
                                            temp.pUuidObj);
            }
        }
        break;

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
        case GAPConnectionEventPairingCompleteIndication_FSCI_ID:
        {
            if (pMsg->Data.GAPConnectionEventPairingCompleteIndication.PairingStatus
                == GAPConnectionEventPairingCompleteIndication_PairingStatus_PairingSuccessful)
            {
                if (pMsg->Data.GAPConnectionEventPairingCompleteIndication.PairingData.PairingSuccessful_WithBonding)
                {
                    GAPAddDeviceToFilterAcceptListRequest_t req;
                    req.AddressType =
                        (GAPAddDeviceToFilterAcceptListRequest_AddressType_t)maPeerInformation[pMsg->Data.GAPConnectionEventPairingCompleteIndication.DeviceId].addressType;
                    FLib_MemCpy(req.Address,
                                maPeerInformation[pMsg->Data.GAPConnectionEventPairingCompleteIndication.DeviceId].address,
                                gcBleDeviceAddressSize_c);
                    GAPAddDeviceToFilterAcceptListRequest(&req, gFsciInterface_c);
                }
                BleApp_StateMachineHandler(pMsg->Data.GAPConnectionEventPairingCompleteIndication.DeviceId,
                                           mAppEvt_PairingComplete_c);
            }
        }
        break;

        case GAPConnectionEventAuthenticationRejectedIndication_FSCI_ID:
        {
            deviceId_t peerDeviceId = pMsg->Data.GAPConnectionEventPeripheralSecurityRequestIndication.DeviceId;

            if (maPeerInformation[peerDeviceId].gapRole == gGapCentral_c)
            {
                /* Start Pairing Procedure 
                 * peripheral could have lost the bond */
                GAPPairRequest_t req;
                req.DeviceId = pMsg->Data.GAPConnectionEventPeripheralSecurityRequestIndication.DeviceId;
                req.PairingParameters.WithBonding = gPairingParameters.withBonding;
                req.PairingParameters.SecurityModeAndLevel = GAPPairRequest_PairingParameters_SecurityModeAndLevel_gMode1Level3_c;
                req.PairingParameters.MaxEncryptionKeySize = mcEncryptionKeySize_c;
                req.PairingParameters.LocalIoCapabilities = GAPPairRequest_PairingParameters_LocalIoCapabilities_gIoKeyboardDisplay_c;
                req.PairingParameters.OobAvailable = gPairingParameters.oobAvailable;
                req.PairingParameters.CentralKeys = gPairingParameters.centralKeys;
                req.PairingParameters.PeripheralKeys = gPairingParameters.peripheralKeys;
                req.PairingParameters.LeSecureConnectionSupported = gPairingParameters.leSecureConnectionSupported;
                req.PairingParameters.UseKeypressNotifications = gPairingParameters.useKeypressNotifications;

                (void)GAPPairRequest(&req, gFsciInterface_c);
            }
        }
        break;

        case GAPPairingEventNoLTKIndication_FSCI_ID:
        {
            deviceId_t peerDeviceId = (deviceId_t)pMsg->Data.GAPPairingEventNoLTKIndication.DeviceId;

            if (maPeerInformation[peerDeviceId].gapRole == gGapPeripheral_c)
            {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
                GAPSendPeripheralSecurityRequestRequest_t req;
                req.DeviceId = peerDeviceId;
                req.PairingParameters.WithBonding = gPairingParameters.withBonding;
                req.PairingParameters.SecurityModeAndLevel = GAPSendPeripheralSecurityRequestRequest_PairingParameters_SecurityModeAndLevel_gMode1Level3_c;
                req.PairingParameters.MaxEncryptionKeySize = gPairingParameters.maxEncryptionKeySize;
                req.PairingParameters.LocalIoCapabilities =
                  GAPSendPeripheralSecurityRequestRequest_PairingParameters_LocalIoCapabilities_gIoDisplayOnly_c;
                req.PairingParameters.OobAvailable = gPairingParameters.oobAvailable;
                req.PairingParameters.CentralKeys = gPairingParameters.centralKeys;
                req.PairingParameters.PeripheralKeys = gPairingParameters.peripheralKeys;
                req.PairingParameters.LeSecureConnectionSupported = FALSE;
                req.PairingParameters.UseKeypressNotifications = FALSE;
                GAPSendPeripheralSecurityRequestRequest(&req, gFsciInterface_c);
#endif
            }
        }
        break;

#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
        case GAPCheckIfBondedIndication_FSCI_ID:
        {
            if (pMsg->Data.GAPCheckIfBondedIndication.IsBonded == TRUE)
            {
                /* Restore bonding information */
                maPeerInformation[mLastCheckIfBondedDeviceId].isBonded = TRUE;
                GAPLoadCustomPeerInformationRequest_t req;
                req.DeviceId = mLastCheckIfBondedDeviceId;
                req.Offset = 0U;
                req.InfoSize = sizeof(wucConfig_t);
                (void)GAPLoadCustomPeerInformationRequest(&req, gFsciInterface_c);
            }
            else
            {
                /* If peer was not bonded send peripheral security request */
                maPeerInformation[mLastCheckIfBondedDeviceId].isBonded = FALSE;
                BleApp_StateMachineHandler(mLastCheckIfBondedDeviceId, mAppEvt_PeerConnected_c);
                if (maPeerInformation[mLastCheckIfBondedDeviceId].gapRole == gGapPeripheral_c)
                {
                    GAPSendPeripheralSecurityRequestRequest_t req;
                    req.DeviceId = mLastCheckIfBondedDeviceId;
                    req.PairingParameters.WithBonding = gPairingParameters.withBonding;
                    req.PairingParameters.SecurityModeAndLevel =
                      GAPSendPeripheralSecurityRequestRequest_PairingParameters_SecurityModeAndLevel_gMode1Level3_c;
                    req.PairingParameters.MaxEncryptionKeySize = gPairingParameters.maxEncryptionKeySize;
                    req.PairingParameters.LocalIoCapabilities =
                      GAPSendPeripheralSecurityRequestRequest_PairingParameters_LocalIoCapabilities_gIoDisplayOnly_c;
                    req.PairingParameters.OobAvailable = gPairingParameters.oobAvailable;
                    req.PairingParameters.CentralKeys = gPairingParameters.centralKeys;
                    req.PairingParameters.PeripheralKeys = gPairingParameters.peripheralKeys;
                    req.PairingParameters.LeSecureConnectionSupported = FALSE;
                    req.PairingParameters.UseKeypressNotifications = FALSE;
                    GAPSendPeripheralSecurityRequestRequest(&req, gFsciInterface_c);
                }
            }
        }
        break;

        case GAPLoadCustomPeerInformationIndication_FSCI_ID:
        {
            /* Restored custom connection information. Encrypt link */
            if (pMsg->Data.GAPLoadCustomPeerInformationIndication.Info != NULL)
            {
                FLib_MemCpy((uint8_t *) &maPeerInformation[mLastCheckIfBondedDeviceId].clientInfo,
                            pMsg->Data.GAPLoadCustomPeerInformationIndication.Info,
                            pMsg->Data.GAPLoadCustomPeerInformationIndication.InfoSize);
            }

#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
            if ((maPeerInformation[mLastCheckIfBondedDeviceId].gapRole == gGapCentral_c) && 
                (maPeerInformation[mLastCheckIfBondedDeviceId].isBonded == TRUE))
            {
                /* Encrypt Link */
                GAPEncryptLinkRequest_t req;
                req.DeviceId = mLastCheckIfBondedDeviceId;
                (void)GAPEncryptLinkRequest(&req, gFsciInterface_c);
            }
#endif
        }
        break;

        case GAPConnectionEventEncryptionChangedIndication_FSCI_ID:
        {
            if (pMsg->Data.GAPConnectionEventEncryptionChangedIndication.NewEncryptionState == TRUE)
            {
                BleApp_StateMachineHandler(pMsg->Data.GAPConnectionEventEncryptionChangedIndication.DeviceId, mAppEvt_PeerConnected_c);
            }
        }
        break;
#endif
#endif
        default:
        {
            BleApp_EventCallback3(pMsg);
        }
        break;
    }
}
/*! *********************************************************************************
* \brief        Handle BLE events
*
* \param[in]    pMsg              Pointer to the event revceived.
********************************************************************************** */
void BleApp_EventCallback
(
    bleEvtContainer_t *pMsg
)
{
    deviceId_t deviceId = gInvalidDeviceId_c;

    switch (pMsg->id)
    {
        case GAPGenericEventInitializationCompleteIndication_FSCI_ID:
        {
            BleApp_GenericEvtInitCompleteHandler();
        }
        break;

        case GAPAdvertisingEventStateChangedIndication_FSCI_ID:
        {
            BleApp_AdvertisingEvtStateChangedHandler();
        }
        break;

        case GAPScanningEventStateChangedIndication_FSCI_ID:
        {
            mScanningOn = !mScanningOn;

            /* Node starts scanning */
            if (mScanningOn)
            {
                mAppUartNewLine = TRUE;
                /* Start scanning timer */
                (void)TM_InstallCallback((timer_handle_t)mAppTimerId, ScanningTimerCallback, NULL);
                (void)TM_Start((timer_handle_t)mAppTimerId,
                            (uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSetSecondTimer, gScanningTime_c);
#if (defined(gAppLedCnt_c) && (gAppLedCnt_c == 1))
                LedSetColor(0, kLED_Blue);
#endif  /*gAppLedCnt_c == 1*/
                Led1Flashing();
                Serial_Print("\n\rScanning...\n\r", gAllowToBlock_d);
            }
            else
            {
                if (mInitiatingConnection == TRUE)
                {
                    GAPConnectRequest_t req;
                    req.ScanInterval = gConnReqParams.scanInterval;
                    req.ScanWindow = gConnReqParams.scanWindow;
                    req.FilterPolicy = (GAPConnectRequest_FilterPolicy_t)gConnReqParams.filterPolicy;
                    req.OwnAddressType = (GAPConnectRequest_OwnAddressType_t)gConnReqParams.ownAddressType;
                    req.ConnIntervalMin = gConnReqParams.connIntervalMin;
                    req.ConnIntervalMax = gConnReqParams.connIntervalMax;
                    req.ConnLatency = gConnReqParams.connLatency;
                    req.SupervisionTimeout = gConnReqParams.supervisionTimeout;
                    req.ConnEventLengthMin = gConnReqParams.connEventLengthMin;
                    req.ConnEventLengthMax = gConnReqParams.connEventLengthMax;
                    req.Initiating_PHYs = gConnReqParams.initiatingPHYs;
                    FLib_MemCpy(req.PeerAddress, gConnReqParams.peerAddress, gcBleDeviceAddressSize_c);
                    req.PeerAddressType = (GAPConnectRequest_PeerAddressType_t)gConnReqParams.peerAddressType;
                    req.usePeerIdentityAddress = gConnReqParams.usePeerIdentityAddress;
                    GAPConnectRequest(&req, gFsciInterface_c);
                }

                /* Node is not scanning */
                (void)TM_Stop((timer_handle_t)mAppTimerId);
                LedStartFlashingAllLeds();
                Serial_Print("\n\rStop Scanning...\n\r", gAllowToBlock_d);
            }
        }
        break;

        case GAPScanningEventDeviceScannedIndication_FSCI_ID:
        {
            /* Checks Scan data for a device to connect */
            if (checkScanEvent((gapScannedDevice_t*)(&pMsg->Data.GAPScanningEventDeviceScannedIndication)) &&
                (mcActiveConnNo < gAppMaxConnections_c))
            {
                /* Found device - stop scanning and initiate connection */
                mInitiatingConnection = TRUE;
                /* save peer address information for connection request */
                FLib_MemCpy(gConnReqParams.peerAddress,
                            &pMsg->Data.GAPScanningEventDeviceScannedIndication.Address,
                            gcBleDeviceAddressSize_c);
                gConnReqParams.peerAddressType = (GAPConnectRequest_PeerAddressType_t)pMsg->Data.GAPScanningEventDeviceScannedIndication.AddressType;
#if gAppUsePrivacy_d
                gConnReqParams.usePeerIdentityAddress = pMsg->Data.GAPScanningEventDeviceScannedIndication.advertisingAddressResolved;
#endif
                GAPStopScanningRequest(gFsciInterface_c);
            }
        }
        break;

        case GAPConnectionEventConnectedIndication_FSCI_ID:
        {
            deviceId = pMsg->Data.GAPConnectionEventConnectedIndication.DeviceId;

            mcActiveConnNo++;

            mInitiatingConnection = FALSE;

            maPeerInformation[deviceId].deviceId = pMsg->Data.GAPConnectionEventConnectedIndication.DeviceId;
            maPeerInformation[deviceId].gapRole = (gapRole_t)pMsg->Data.GAPConnectionEventConnectedIndication.connectionRole;
            maPeerInformation[deviceId].addressType = (bleAddressType_t)pMsg->Data.GAPConnectionEventConnectedIndication.PeerAddressType;
            FLib_MemCpy(maPeerInformation[deviceId].address,
                        pMsg->Data.GAPConnectionEventConnectedIndication.PeerAddress,
                        gcBleDeviceAddressSize_c);

            /* Advertising stops when connected */
#if gWuart_PeripheralRole_c == 1
            if(pMsg->Data.GAPConnectionEventConnectedIndication.connectionRole ==
               GAPConnectionEventConnectedIndication_connectionRole_gBleLlConnectionPeripheral_c)
            {
                mAdvState.advOn = FALSE;
            }
#endif

            /* Subscribe client*/
            (void)Bas_Subscribe(&mBasServiceConfig, deviceId);

            /* UI */
            LedStopFlashingAllLeds();
#if (defined(gAppLedCnt_c) && (gAppLedCnt_c == 1))
            LedSetColor(0, kLED_White);
#endif /* gAppLedCnt_c == 1 */
            Led1On();

            if (TM_IsTimerActive((timer_handle_t)mBatteryMeasurementTimerId) == 0U)
            {
                /* Start battery measurements */
                (void)TM_InstallCallback((timer_handle_t)mBatteryMeasurementTimerId, BatteryMeasurementTimerCallback, NULL);
                (void)TM_Start((timer_handle_t)mBatteryMeasurementTimerId,
                            (uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSetSecondTimer, mBatteryLevelReportInterval_c);
            }

            Serial_Print("Connected to device ", gAllowToBlock_d);
            Serial_PrintDec(deviceId);

            if (pMsg->Data.GAPConnectionEventConnectedIndication.connectionRole ==
                GAPConnectionEventConnectedIndication_connectionRole_gBleLlConnectionCentral_c)
            {
                Serial_Print(" as central.\n\r", gAllowToBlock_d);
            }
            else
            {
                Serial_Print(" as peripheral.\n\r", gAllowToBlock_d);
            }

            mAppUartNewLine = TRUE;

            BleServDisc_RegisterCallback(BleApp_ServiceDiscoveryCallback);

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U) && \
     defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
            /* Check if the peer was previously bonded */
            GAPCheckIfBondedRequest_t req;
            req.DeviceId = pMsg->Data.GAPConnectionEventConnectedIndication.DeviceId;
            mLastCheckIfBondedDeviceId = req.DeviceId;
            (void)GAPCheckIfBondedRequest(&req, gFsciInterface_c);
#else /* gAppUsePairing_d && gAppUseBonding_d*/
            /* run the state machine */
            BleApp_StateMachineHandler(deviceId, mAppEvt_PeerConnected_c);
#endif /* gAppUsePairing_d && gAppUseBonding_d*/
        }
        break;

        case GAPConnectionEventLeDataLengthChangedIndication_FSCI_ID:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
            deviceId_t peerDeviceId = pMsg->Data.GAPConnectionEventLeDataLengthChangedIndication.DeviceId;

            if ((maPeerInformation[peerDeviceId].gapRole == gGapPeripheral_c) &&
                (mDataLengthChangeLocallyInitiated == TRUE))
            {
                /* Check if the peer was previously bonded */
                mDataLengthChangeLocallyInitiated = FALSE;
                GAPCheckIfBondedRequest_t req;
                req.DeviceId = pMsg->Data.GAPConnectionEventLeDataLengthChangedIndication.DeviceId;
                (void)GAPCheckIfBondedRequest(&req,  gFsciInterface_c);
            }
#else
            BleApp_StateMachineHandler(pMsg->Data.GAPConnectionEventLeDataLengthChangedIndication.DeviceId, mAppEvt_PeerConnected_c);
#endif /* gAppUseBonding_d* */
#endif /* gAppUsePairing_d */
        }
        break;

        default:
        {
            BleApp_EventCallback2(pMsg);
        }
        break;
    }
}
/*! *********************************************************************************
* \brief        Handle Service Discovery events
*
* \param[in]    peerDeviceId        Remote device ID.
* \param[in]    pEvent              Pointer to the event revceived.
********************************************************************************** */
void BleApp_ServiceDiscoveryCallback
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
                if (FLib_MemCmp((void *)&uuid_service_wireless_uart, (void *)&pEvent->eventData.pService->uuid, sizeof(bleUuid_t)))
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
 * \brief        Handle the main application state machine
 *
 * \param[in]    peerDeviceId       The remote device ID.
 * \param[in]    event              The application event.
 ********************************************************************************** */
void BleApp_StateMachineHandler
(
    deviceId_t peerDeviceId,
    appEvent_t event
)
{
    union
    {
        uint8_t     *pUuidArray;
        bleUuid_t   *pUuidObj;
    } temp; /* MISRA rule 11.3 */

    temp.pUuidArray = uuid_service_wireless_uart;

    /* invalid client information */
    if (maPeerInformation[peerDeviceId].deviceId != gInvalidDeviceId_c)
    {
        switch (maPeerInformation[peerDeviceId].appState)
        {
        case mAppIdle_c:
        {
            if (event == mAppEvt_PeerConnected_c ||
                event == mAppEvt_PairingComplete_c)
            {
                /* Let the central device initiate the Exchange MTU procedure*/
                if (maPeerInformation[peerDeviceId].gapRole == gGapCentral_c)
                {
                    /* Moving to Exchange MTU State */
                    maPeerInformation[peerDeviceId].appState = mAppExchangeMtu_c;
                    GATTClientExchangeMtuRequest_t req;
                    req.DeviceId = peerDeviceId;
                    req.Mtu = gAttMaxMtu_c;
                    GATTClientExchangeMtuRequest(&req, gFsciInterface_c);
                }
                else
                {
                    /* Moving to Service Discovery State*/
                    maPeerInformation[peerDeviceId].appState = mAppServiceDisc_c;

                    /* Start Service Discovery*/
                    (void)BleServDisc_FindService(peerDeviceId,
                                                  gBleUuidType128_c,
                                                  temp.pUuidObj);
                }
            }
        }
        break;

        case mAppExchangeMtu_c:
        {
            if (event == mAppEvt_GattProcComplete_c)
            {
                mLastGetMtuDeviceId = peerDeviceId;

                /* update stream length with minimum of maximum MTU's of connected devices */
                GATTGetMtuRequest_t req;
                req.DeviceId = peerDeviceId;
                GATTGetMtuRequest(&req, gFsciInterface_c);
            }
            else
            {
                if (event == mAppEvt_GattProcError_c)
                {
                    GAPDisconnectRequest_t req;
                    req.DeviceId = peerDeviceId;
                    GAPDisconnectRequest(&req, gFsciInterface_c);
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
                union
                {
                    uint32_t u32;
                    uint16_t u16;
                } tempCast;

                tempCast.u32 = sizeof(wucConfig_t);
                /* Write data in NVM */
                GAPSaveCustomPeerInformationRequest_t req;
                req.DeviceId = maPeerInformation[peerDeviceId].deviceId;
                req.Offset = 0;
                req.InfoSize = tempCast.u16;
                req.Info = (uint8_t *) &maPeerInformation[peerDeviceId].clientInfo;
                GAPSaveCustomPeerInformationRequest(&req, gFsciInterface_c);
#endif
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
                GAPDisconnectRequest_t req;
                req.DeviceId = peerDeviceId;
                GAPDisconnectRequest(&req, gFsciInterface_c);
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
                GAPDisconnectRequest_t req;
                req.DeviceId = peerDeviceId;
                GAPDisconnectRequest(&req, gFsciInterface_c);
            }
            else
            {
                /* ignore other event types */
            }
        }
        break;

        case mAppRunning_c:
          {
#if gAppUseBonding_d
              if (event == mAppEvt_EncryptionComplete_c)
              {
                  union
                  {
                      uint32_t u32;
                      uint16_t u16;
                  } tempCast;

                  tempCast.u32 = sizeof(wucConfig_t);
                  /* Write data in NVM */
                  GAPSaveCustomPeerInformationRequest_t req;
                  req.DeviceId = maPeerInformation[peerDeviceId].deviceId;
                  req.Offset = 0;
                  req.InfoSize = tempCast.u16;
                  req.Info = (uint8_t *) &maPeerInformation[peerDeviceId].clientInfo;
                  GAPSaveCustomPeerInformationRequest(&req, gFsciInterface_c);
              }
#endif
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

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c == 1))
/*! *********************************************************************************
 * \brief        Handles the switch press timer callback.
 *
 * \param[in]    pParam             Callback parameters.
 ********************************************************************************** */
static void SwitchPressTimerCallback
(
    void *pParam
)
{
  if(mSwitchPressCnt >= gSwitchPressThreshold_c)
  {
      /* Switch the current role */
      if (mGapRole == gGapCentral_c)
      {
          Serial_Print("\n\rSwitched role to GAP Peripheral.\n\r", gAllowToBlock_d);
          mAppUartNewLine = TRUE;
          mGapRole = gGapPeripheral_c;
      }
      else
      {
          Serial_Print("\n\rSwitched role to GAP Central.\n\r", gAllowToBlock_d);
          mAppUartNewLine = TRUE;
          mGapRole = gGapCentral_c;
      }
  }
  else
  {
      /* start the application using the selected role */
      LedStopFlashingAllLeds();
      Led1Flashing();
      BleApp_Start(mGapRole);
  }

  /* reset the switch press counter */
  mSwitchPressCnt = 0;
}
#endif

/*! *********************************************************************************
 * \brief        Get all bytes from the serial interface and send it over GATT.
 *
 * \param[in]    pParam             Callback parameters.
 ********************************************************************************** */
static void BleApp_FlushUartStream
(
    void *pParam
)
{
    uint8_t *pMsg = NULL;
    uint32_t bytesRead = 0;
    uint8_t  mPeerId = 0;
    bool_t   mValidDevices = FALSE;

    /* Valid devices are in Running state */
    for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
    {
        if ((gInvalidDeviceId_c != maPeerInformation[mPeerId].deviceId) &&
            (mAppRunning_c == maPeerInformation[mPeerId].appState))
        {
            mValidDevices = TRUE;
            break;
        }
    }

    if (mValidDevices)
    {
        /* Allocate buffer for GATT Write */
        pMsg = MEM_BufferAlloc(mAppUartBufferSize);

        if (pMsg != NULL)
        {
            /* Collect the data from the serial manager buffer */
            if (SerialManager_TryRead((serial_read_handle_t)s_readHandle, pMsg, mAppUartBufferSize, &bytesRead) == kStatus_SerialManager_Success)
            {
                if (bytesRead != 0U)
                {
                    /* Send data over the air */
                    BleApp_SendUartStream(pMsg, (uint8_t)bytesRead);
                }
            }


            /* Free Buffer */
            (void)MEM_BufferFree(pMsg);
        }
    }

    mAppDapaPending = FALSE;
}

/*! *********************************************************************************
 * \brief        Write bytes to serial.
 *
 * \param[in]    peerDeviceId       The remote device ID.
 * \param[in]    pStream            Pointer to the received stream.
 * \param[in]    streamLength       Number of bytes in the strem.
 ********************************************************************************** */
void BleApp_ReceivedUartStream
(
    deviceId_t peerDeviceId,
    uint8_t *pStream,
    uint16_t streamLength
)
{
    static deviceId_t previousDeviceId = gInvalidDeviceId_c;

    char additionalInfoBuff[10] = { '\r', '\n', '[', '0', '0', '-', 'C', ']', ':', ' '};
    uint8_t *pBuffer = NULL;
    uint32_t messageHeaderSize = 0;


    if (mAppUartNewLine || (previousDeviceId != peerDeviceId))
    {
        streamLength += (uint16_t)sizeof(additionalInfoBuff);
    }

    /* Allocate buffer for asynchronous write */
    pBuffer = MEM_BufferAlloc(streamLength + 1U);

    if (pBuffer != NULL)
    {
        /* if this is a message from a previous device, print device ID */
        if (mAppUartNewLine || (previousDeviceId != peerDeviceId))
        {
            messageHeaderSize = sizeof(additionalInfoBuff);

            if (mAppUartNewLine)
            {
                mAppUartNewLine = FALSE;
            }

            /* Print device ID of peer who sent the message */
            additionalInfoBuff[3] = '0' + (peerDeviceId / 10U);
            additionalInfoBuff[4] = '0' + (peerDeviceId % 10U);

            if (gGapCentral_c != maPeerInformation[peerDeviceId].gapRole)
            {
                additionalInfoBuff[6] = 'P';
            }

            FLib_MemCpy(pBuffer, additionalInfoBuff, messageHeaderSize);
        }
        FLib_MemCpy(&pBuffer[messageHeaderSize], pStream, (uint32_t)streamLength - messageHeaderSize);
#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
        serial_manager_status_t status = SerialManager_InstallTxCallback((serial_write_handle_t)s_writeHandle, Uart_TxCallBack, pBuffer);
        (void)status;
        assert(kStatus_SerialManager_Success == status);
        if(SerialManager_WriteNonBlocking((serial_write_handle_t)s_writeHandle, pBuffer, streamLength) != kStatus_SerialManager_Success)
        {
            (void)MEM_BufferFree(pBuffer);
        }
#else
        (void)MEM_BufferFree(pBuffer);
#endif /*SERIAL_MANAGER_NON_BLOCKING_MODE > 0U*/
    }

    /* update the previous device ID */
    previousDeviceId = peerDeviceId;
}

/*! *********************************************************************************
 * \brief        Send the received uart stream over GATT
 *
 * \param[in]    pData              Pointer to the received stream.
 * \param[in]    streamSize         The number of bytes in the stream.
 ********************************************************************************** */
void BleApp_SendUartStream
(
    uint8_t *pRecvStream,
    uint32_t streamSize
)
{
    uint8_t mPeerId = 0U;
    GATTClientWriteCharacteristicValueRequest_t req;

    /* send UART stream to all peers */
    for (mPeerId = 0U; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
    {
        if (gInvalidDeviceId_c != maPeerInformation[mPeerId].deviceId &&
            mAppRunning_c == maPeerInformation[mPeerId].appState)
        {
            FLib_MemSet(&req, 0, sizeof(GATTClientWriteCharacteristicValueRequest_t));
            req.DeviceId = mPeerId;
            req.Characteristic.Properties = gCharPropNone_c;
            req.Characteristic.NbOfDescriptors = 0;
            req.Characteristic.Descriptors = NULL;
            req.ValueLength = streamSize;
            req.Value = (uint8_t*)pRecvStream;
            req.WithoutResponse = TRUE;
            req.SignedWrite = FALSE;
            req.ReliableLongCharWrites = FALSE;
            req.Characteristic.Value.UuidType = Uuid16Bits;
            req.Characteristic.Value.Handle = maPeerInformation[mPeerId].clientInfo.hUartStream;
            GATTClientWriteCharacteristicValueRequest(&req, gFsciInterface_c);
        }
    }
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
 * \brief        Function handling the initialization complete event of the Bluetooth LE Host stack.
 *
 ********************************************************************************** */
static void BleApp_GenericEvtInitCompleteHandler
(
    void
)
{
    uint8_t peerId = 0;

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[0], BleApp_HandleKeys0, NULL);

#if (gWuart_CentralRole_c == 1) && (gWuart_PeripheralRole_c == 1)
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1))
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[1], BleApp_HandleKeys1, NULL);
#endif
#endif
#endif

    mcActiveConnNo = 0U;

    for (peerId = 0; peerId < (uint8_t)gAppMaxConnections_c; peerId++)
    {
        maPeerInformation[peerId].deviceId = gInvalidDeviceId_c;
        maPeerInformation[peerId].appState = mAppIdle_c;
        maPeerInformation[peerId].clientInfo.hService = gGattDbInvalidHandleIndex_d;
        maPeerInformation[peerId].clientInfo.hUartStream = gGattDbInvalidHandleIndex_d;
    }

#if (gWuart_AutoStart_c == 1)
    mGapRole = gWuart_AutoStartGapRole_c;
#else
    /* By default, always start node as GAP central */
#if gWuart_CentralRole_c == 1
    mGapRole = gGapCentral_c;
#else
    mGapRole = gGapPeripheral_c;
#endif /* gWuart_CentralRole_c */
#endif /* gWuart_AutoStart_c */

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c == 1))
    Serial_Print("\n\rWireless UART starting as GAP Central.\n\r", gAllowToBlock_d);
    Serial_Print("\n\rWithin one second, either:\n\r", gAllowToBlock_d);
    Serial_Print(" - double press the switch to change the role or\n\r", gAllowToBlock_d);
    Serial_Print(" - single press to start the application with the selected role.\n\r", gAllowToBlock_d);
#else
    if (mGapRole == gGapCentral_c)
    {
        Serial_Print("\n\rWireless UART starting as GAP Central.\n\r", gAllowToBlock_d);
    }
    else
    {
        Serial_Print("\n\rWireless UART starting as GAP Peripheral.\n\r", gAllowToBlock_d);
    }
#endif
#if gWuart_PeripheralRole_c == 1
    mAdvState.advOn = FALSE;
#endif /* gWuart_PeripheralRole_c */

#if gWuart_CentralRole_c == 1
    mScanningOn = FALSE;
#endif /* gWuart_CentralRole_c */

    /* Start services */
    mBasServiceConfig.batteryLevel = SENSORS_GetBatteryLevel();
    (void)Bas_Start(&mBasServiceConfig);

    /* Allocate application timer */
    (void)TM_Open(mAppTimerId);
    (void)TM_Open(mUartStreamFlushTimerId);
    (void)TM_Open(mBatteryMeasurementTimerId);

#if (gAppButtonCnt_c == 1)
    (void)TM_Open(mSwitchPressTimerId);
#endif

#if !(defined(cPWR_UsePowerDownMode) && (cPWR_UsePowerDownMode))
    LedStartFlashingAllLeds();
#endif
#if (gWuart_AutoStart_c == 1)
    BleApp_Start(mGapRole);
#endif

    bleUuid_t uuid;
    uuid.uuid16 = gBleSig_GenericAccessProfile_d;

    /* Add the Generic Access profile to the BB Database */
    GATTDBDynamicAddPrimaryServiceDeclarationRequest_t req = { 0 };
    req.DesiredHandle = mcGenericAccessProfileHandle_c;
    req.UuidType = Uuid16Bits;
    Utils_PackTwoByteValue(uuid.uuid16, req.Uuid.Uuid16Bits);
    GATTDBDynamicAddPrimaryServiceDeclarationRequest(&req, gFsciInterface_c);
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
#if (1 == gAppButtonCnt_c)
            /* increment the switch press counter */
            mSwitchPressCnt++;

            if(FALSE == TM_IsTimerActive(mSwitchPressTimerId))
            {
                /* Start the switch press timer */
                (void)TM_InstallCallback((timer_handle_t)mSwitchPressTimerId, SwitchPressTimerCallback, NULL);
                (void)TM_Start((timer_handle_t)mAppTimerId, (uint8_t)kTimerModeSingleShot | (uint8_t)kTimerModeLowPowerTimer, gSwitchPressTimeout_c);
            }
#else  /*1 == gAppButtonCnt_c*/

            BleApp_Start(mGapRole);
#endif /*1 == gAppButtonCnt_c*/
            break;
        }

        case kBUTTON_EventLongPress:
        {
            for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
            {
                if (maPeerInformation[mPeerId].deviceId != gInvalidDeviceId_c)
                {
                    GAPDisconnectRequest_t req;
                    req.DeviceId = maPeerInformation[mPeerId].deviceId;

                    GAPDisconnectRequest(&req, gFsciInterface_c);
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
#endif /*gAppButtonCnt_c > 0*/

#if (gWuart_CentralRole_c == 1) && (gWuart_PeripheralRole_c == 1)
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
            /* Switch current role */
            if (mGapRole == gGapCentral_c)
            {
                Serial_Print("\n\rSwitched role to GAP Peripheral.\n\r", gAllowToBlock_d);
                mAppUartNewLine = TRUE;
                mGapRole = gGapPeripheral_c;
            }
            else
            {
                Serial_Print("\n\rSwitched role to GAP Central.\n\r", gAllowToBlock_d);
                mAppUartNewLine = TRUE;
                mGapRole = gGapCentral_c;
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
#endif /* (gWuart_CentralRole_c == 1) && (gWuart_PeripheralRole_c == 1) */
#endif /*gAppButtonCnt_c > 0*/

/*! *********************************************************************************
 * \brief    Starts the BLE application.
 *
 * \param[in]    gapRole            GAP Start Role (Central or Peripheral).
 ********************************************************************************** */
static void BleApp_Start
(
    gapRole_t gapRole
)
{
    LedStopFlashingAllLeds();
    Led1Flashing();
    switch (gapRole)
    {
#if gWuart_CentralRole_c == 1
    case gGapCentral_c:
        {
            if (mScanningOn == FALSE)
            {
                mAppUartNewLine = TRUE;
#if defined(gAppUsePairing_d) && (gAppUsePairing_d == 1)
                gPairingParameters.localIoCapabilities = gIoKeyboardDisplay_c;
#endif /* gAppUsePairing_d */
                /* Register GATT Callbacks and start scanning */
                if (mGattCallbacksInitialized == FALSE)
                {
                    RegisterRemovableObserver(GATTConfirm_FSCI_ID,
                                              hsdkObserverGATTClientRegisterProcedureCallback);
                    GATTClientRegisterProcedureCallbackRequest(gFsciInterface_c);
                }
                else
                {
                    GapStartScanningRequest();
                }
            }
            break;
        }
#endif /* gWuart_CentralRole_c */
#if gWuart_PeripheralRole_c == 1
    case gGapPeripheral_c:
        {
            mAppUartNewLine = TRUE;
#if defined(gAppUsePairing_d) && (gAppUsePairing_d == 1)
            gPairingParameters.localIoCapabilities = gIoDisplayOnly_c;
#endif /* gAppUsePairing_d */

            /* Start ADV only if it's not already started */
            if (!mAdvState.advOn)
            {
                /* Register GATT Callbacks. When done set advertising parameters and data and start Advertising */
                if (mGattCallbacksInitialized == FALSE)
                {
                    RegisterRemovableObserver(GATTConfirm_FSCI_ID,
                                              hsdkObserverGATTClientRegisterProcedureCallback);
                    GATTClientRegisterProcedureCallbackRequest(gFsciInterface_c);
                }
                else
                {
                    GAPSetAdvertisingParametersRequest_t req;
                    /* Copy values from gAdvParams to packed structure req */
                    req.MinInterval = gAdvParams.minInterval;
                    req.MaxInterval = gAdvParams.maxInterval;
                    req.AdvertisingType = (GAPSetAdvertisingParametersRequest_AdvertisingType_t)gAdvParams.advertisingType;
                    req.OwnAddressType = (GAPSetAdvertisingParametersRequest_OwnAddressType_t)gAdvParams.ownAddressType;
                    req.PeerAddressType = (GAPSetAdvertisingParametersRequest_PeerAddressType_t)gAdvParams.peerAddressType;
                    FLib_MemCpy(req.PeerAddress, gAdvParams.peerAddress, sizeof(req.PeerAddress));
                    req.ChannelMap = (uint8_t)gAdvParams.channelMap;
                    req.FilterPolicy = (GAPSetAdvertisingParametersRequest_FilterPolicy_t)gAdvParams.filterPolicy;
                    /* Set advertising parameters, data and start Advertising */
                    GAPSetAdvertisingParametersRequest(&req, gFsciInterface_c);
                }
            }
            break;
        }
#endif /* gWuart_PeripheralRole_c */
    default:
        {
            ; /* No action required */
            break;
        }
    }
}

#if (gWuart_PeripheralRole_c == 1)
/*! *********************************************************************************
* \brief        Handles BLE Advertising event state changed from host stack.
*
********************************************************************************** */
static void BleApp_AdvertisingEvtStateChangedHandler
(
    void
)
{
    mAdvState.advOn = !mAdvState.advOn;
    LedStopFlashingAllLeds();
    if (mAdvState.advOn)
    {
#if (defined(gAppLedCnt_c) && (gAppLedCnt_c ==1))
        LedSetColor(0, kLED_Blue);
#endif /* gAppLedCnt_c == 1 */
        Led1Flashing();
        Serial_Print("\n\rAdvertising...\n\r", gAllowToBlock_d);
    }
    else
    {
#if (defined(gAppLedCnt_c) && (gAppLedCnt_c ==1))
        LedSetColor(0, kLED_White);
#endif /* gAppLedCnt_c == 1 */
        LedStartFlashingAllLeds();
    }
}
#endif

static void BleApp_StoreServiceHandles
(
    deviceId_t peerDeviceId,
    gattService_t *pService
)
{
    /* Found Wireless UART Service */
    maPeerInformation[peerDeviceId].clientInfo.hService = pService->startHandle;

    if (pService->cNumCharacteristics > 0U &&
        pService->aCharacteristics != NULL)
    {
        /* Found Uart Characteristic */
        maPeerInformation[peerDeviceId].clientInfo.hUartStream = pService->aCharacteristics[0].value.handle;
    }
}

#if gWuart_CentralRole_c == 1
/*! *********************************************************************************
 * \brief        Handles scanning timer callback.
 *
 * \param[in]    pParam        Callback parameters.
*
*\retval     void.
 ********************************************************************************** */
static void ScanningTimerCallback
(
    void *pParam
)
{
    /* Stop scanning */
    (void)App_PostCallbackMessage(BleApp_GapStopScanningRequest, NULL);
}
#endif /* gWuart_CentralRole_c == 1 */

/*! *********************************************************************************
* \brief       Checks the advertising data looking for the UUIDs of the desired service.
*
* \param[in]   pData    Pointer to gapScannedDevice_t.
*
*\retval     void.
********************************************************************************** */
static bool_t checkScanEvent
(
    gapScannedDevice_t *pData
)
{
    uint32_t index = 0;
    bool_t foundMatch = FALSE;

    while (index < pData->dataLength)
    {
        gapAdStructure_t adElement;

        adElement.length = pData->data[index];
        adElement.adType = (gapAdType_t) pData->data[index + 1U];
        adElement.aData = &pData->data[index + 2U];

        /* Search for Wireless UART Service */
        if ((adElement.adType == gAdIncomplete128bitServiceList_c)
            || (adElement.adType == gAdComplete128bitServiceList_c))
        {
            foundMatch = matchDataInAdvElementList(
                    &adElement,
                    &uuid_service_wireless_uart,
                    16);
        }

        /* Move on to the next AD element type */
        index += (uint32_t)adElement.length + (uint8_t)sizeof(uint8_t);
    }

    return foundMatch;
}

/*! *********************************************************************************
*\fn    bool_t BluetoothLEHost_MatchDataInAdvElementList(gapAdStructure_t *pElement,
*                                                     void *pData, uint8_t iDataLen)
*
*\brief Search if the contents from pData can be found in an advertising element.
*
*\param[in]  pElement   Pointer to the structure containing the ad structure element.
*\param[in]  pData      Pointer to the data to be searched for.
*\param[in]  iDataLen   The length of the data.

* \retval      TRUE if data was found in this element, FALSE otherwise
********************************************************************************** */
static bool_t matchDataInAdvElementList
(
    gapAdStructure_t *pElement,
    void             *pData,
    uint8_t          iDataLen
)
{
    uint8_t i;
    bool_t status = FALSE;

    if( pElement->length != 0U)
    {
        for (i = 0; i < (pElement->length - 1U); i += iDataLen)
        {
            if (FLib_MemCmp(pData, &pElement->aData[i], iDataLen))
            {
                status = TRUE;
                break;
            }
        }
    }
    return status;
}

/*! *********************************************************************************
 * \brief        Timer handler for flushing the UART.
 *
 * \param[in]    pData              Pointer to the parameters.
 ********************************************************************************** */
static void UartStreamFlushTimerCallback
(
    void *pData
)
{
    if (!mAppDapaPending)
    {
        mAppDapaPending = TRUE;
        (void)App_PostCallbackMessage(BleApp_FlushUartStream, NULL);
    }
}

/*! *********************************************************************************
* \brief        Handles UART Receive callback.
*
* \param[in]    pData        Unused pointer to data.
* \param[in]    pMessage     Unused pointer to message.
* \param[in]    status       Unused status.
********************************************************************************** */
static void Uart_RxCallBack
(
    void *pData,
    serial_manager_callback_message_t *pMessage,
    serial_manager_status_t status
)
{
    uint16_t byteCount = 0;

    if (byteCount < mAppUartBufferSize)
    {
        /* Restart flush timer */
        (void)TM_InstallCallback((timer_handle_t)mUartStreamFlushTimerId, UartStreamFlushTimerCallback, NULL);
        (void)TM_Start((timer_handle_t)mUartStreamFlushTimerId,
                    (uint8_t)kTimerModeLowPowerTimer | (uint8_t)kTimerModeSingleShot, mAppUartFlushIntervalInMs_c);
    }
    else
    {
        /* Post App Msg only one at a time */
        if (!mAppDapaPending)
        {
            mAppDapaPending = TRUE;
            (void)App_PostCallbackMessage(BleApp_FlushUartStream, NULL);
        }
    }
}

/*! *********************************************************************************
* \brief        Handles UART Transmit callback.
*
* \param[in]    pBuffer      Pointer to the sent data.
* \param[in]    pMessage     Unused pointer to message.
* \param[in]    status       Unused status.
********************************************************************************** */
static void Uart_TxCallBack
(
    void *pBuffer,
    serial_manager_callback_message_t *pMessage,
    serial_manager_status_t status
)
{
    (void)MEM_BufferFree(pMessage->buffer);
}

/*! *********************************************************************************
* \brief        Handles battery measurement timer callback.
*
* \param[in]    pParam        Callback parameters.
********************************************************************************** */
static void BatteryMeasurementTimerCallback
(
    void *pParam
)
{
    mBasServiceConfig.batteryLevel = SENSORS_GetBatteryLevel();
    (void)App_PostCallbackMessage(Bas_RecordBatteryMeasurement, &mBasServiceConfig);
}

#if defined(gUseControllerNotifications_c) && (gUseControllerNotifications_c)
/*! *********************************************************************************
 * \brief        Function handling the Bluetooth Controller notification events.
 *
 * \param[in]    pNotificationEvent Pointer to the notification event.
 ********************************************************************************** */
static void BleApp_HandleControllerNotification
(
    bleEvtContainer_t* pMsg
)
{
    switch(pMsg->Data.GAPControllerNotificationIndication.EventType)
    {
        case GAPControllerNotificationIndication_EventType_gNotifConnEventOver_c:
        {
            Serial_Print("CONN Event Over device ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.DeviceId);
            Serial_Print(" on channel ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.Channel);
            Serial_Print(" with RSSI ", gAllowToBlock_d);
            Serial_PrintDec((uint8_t)pMsg->Data.GAPControllerNotificationIndication.RSSI);
            Serial_Print(" and event counter ", gAllowToBlock_d);
            Serial_PrintDec((uint16_t)pMsg->Data.GAPControllerNotificationIndication.ConnEvCounter);
            Serial_Print("\n\r", gAllowToBlock_d);
            break;
        }

        case GAPControllerNotificationIndication_EventType_gNotifConnRxPdu_c:
        {
            Serial_Print("CONN Rx PDU from device ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.DeviceId);
            Serial_Print(" on channel ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.Channel);
            Serial_Print(" with RSSI ", gAllowToBlock_d);
            Serial_PrintDec((uint8_t)pMsg->Data.GAPControllerNotificationIndication.RSSI);
            Serial_Print(" with event counter ", gAllowToBlock_d);
            Serial_PrintDec((uint16_t)pMsg->Data.GAPControllerNotificationIndication.ConnEvCounter);
            Serial_Print(" and timestamp ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.Timestamp);
            Serial_Print("\n\r", gAllowToBlock_d);
            break;
        }

        case GAPControllerNotificationIndication_EventType_gNotifAdvEventOver_c:
        {
            Serial_Print("ADV Event Over.\n\r", gAllowToBlock_d);
            Serial_Print("\n\r", gAllowToBlock_d);
            break;
        }

        case GAPControllerNotificationIndication_EventType_gNotifAdvTx_c:
        {
            Serial_Print("ADV Tx on channel ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.Channel);
            Serial_Print("\n\r", gAllowToBlock_d);
            break;
        }

        case GAPControllerNotificationIndication_EventType_gNotifAdvScanReqRx_c:
        {
            Serial_Print("ADV Rx Scan Req on channel ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.Channel);
            Serial_Print(" with RSSI ", gAllowToBlock_d);
            Serial_PrintDec((uint8_t)pMsg->Data.GAPControllerNotificationIndication.RSSI);
            Serial_Print("\n\r", gAllowToBlock_d);
            break;
        }

        case GAPControllerNotificationIndication_EventType_gNotifAdvConnReqRx_c:
        {
            Serial_Print("ADV Rx Conn Req on channel ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.Channel);
            Serial_Print(" with RSSI ", gAllowToBlock_d);
            Serial_PrintDec((uint8_t)pMsg->Data.GAPControllerNotificationIndication.RSSI);
            Serial_Print("\n\r", gAllowToBlock_d);
            break;
        }

        case GAPControllerNotificationIndication_EventType_gNotifScanEventOver_c:
        {
            Serial_Print("SCAN Event Over on channel ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.Channel);
            Serial_Print("\n\r", gAllowToBlock_d);
            break;
        }

        case GAPControllerNotificationIndication_EventType_gNotifScanAdvPktRx_c:
        {
            Serial_Print("SCAN Rx Adv Pkt on channel ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.Channel);
            Serial_Print(" with RSSI ", gAllowToBlock_d);
            Serial_PrintDec((uint8_t)pMsg->Data.GAPControllerNotificationIndication.RSSI);
            Serial_Print("\n\r", gAllowToBlock_d);
            break;
        }

        case GAPControllerNotificationIndication_EventType_gNotifScanRspRx_c:
        {
            Serial_Print("SCAN Rx Scan Rsp on channel ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.Channel);
            Serial_Print(" with RSSI ", gAllowToBlock_d);
            Serial_PrintDec((uint8_t)pMsg->Data.GAPControllerNotificationIndication.RSSI);
            Serial_Print("\n\r", gAllowToBlock_d);
            break;
        }

        case GAPControllerNotificationIndication_EventType_gNotifScanReqTx_c:
        {
            Serial_Print("SCAN Tx Scan Req on channel ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.Channel);
            Serial_Print("\n\r", gAllowToBlock_d);
            break;
        }

        case GAPControllerNotificationIndication_EventType_gNotifConnCreated_c:
        {
            Serial_Print("CONN Created with device ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.DeviceId);
            Serial_Print(" with timestamp ", gAllowToBlock_d);
            Serial_PrintDec(pMsg->Data.GAPControllerNotificationIndication.Timestamp);
            Serial_Print("\n\r", gAllowToBlock_d);
            break;
        }

        default:
        {
            ; /* No action required */
            break;
        }
    }
}
#endif

/*! *********************************************************************************
 * \brief        Heleper Function to register removable observer.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void hsdkObserverGATTClientRegisterProcedureCallback
(
    bleEvtContainer_t *pContainer
)
{
    RegisterRemovableObserver(GATTConfirm_FSCI_ID,
                              hsdkObserverGATTClientRegisterNotificationCallback);
    GATTClientRegisterNotificationCallbackRequest(gFsciInterface_c);
}

/*! *********************************************************************************
 * \brief        Heleper Function to register removable observer.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void hsdkObserverGATTClientRegisterNotificationCallback
(
    bleEvtContainer_t *pContainer
)
{
     RegisterRemovableObserver(GATTConfirm_FSCI_ID,
                              hsdkObserverGATTClientRegisterIndicationCallback);
     GATTClientRegisterIndicationCallbackRequest(gFsciInterface_c);
}

/*! *********************************************************************************
 * \brief        Heleper Function to register removable observer.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void hsdkObserverGATTClientRegisterIndicationCallback
(
    bleEvtContainer_t *pContainer
)
{
    RegisterRemovableObserver(GATTConfirm_FSCI_ID,
                              hsdkObserverGATTServerRegisterCallbackRequest);
    GATTServerRegisterCallbackRequest(gFsciInterface_c);
}

/*! *********************************************************************************
 * \brief        Heleper Function to register removable observer.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void hsdkObserverGATTServerRegisterCallbackRequest
(
    bleEvtContainer_t *pContainer
)
{
    mGattCallbacksInitialized = TRUE;

    if (mGapRole == gGapPeripheral_c)
    {
#if defined(gUseControllerNotifications_c) && (gUseControllerNotifications_c)
        RegisterRemovableObserver(GATTConfirm_FSCI_ID,
                      hsdkObserverGAPControllerEnhancedNotificationRequest);
        GAPControllerEnhancedNotificationRequest_t req;
        req.DeviceId = 0;
        req.EventType = gNotifAdvEventOver_c | gNotifAdvTx_c | gNotifAdvScanReqRx_c | gNotifAdvConnReqRx_c;
        GAPControllerEnhancedNotificationRequest(&req, gFsciInterface_c);
#else
        GAPSetAdvertisingParametersRequest_t req;
        /* Copy values from gAdvParams to packed structure req */
        req.MinInterval = gAdvParams.minInterval;
        req.MaxInterval = gAdvParams.maxInterval;
        req.AdvertisingType = (GAPSetAdvertisingParametersRequest_AdvertisingType_t)gAdvParams.advertisingType;
        req.OwnAddressType = (GAPSetAdvertisingParametersRequest_OwnAddressType_t)gAdvParams.ownAddressType;
        req.PeerAddressType = (GAPSetAdvertisingParametersRequest_PeerAddressType_t)gAdvParams.peerAddressType;
        FLib_MemCpy(req.PeerAddress, gAdvParams.peerAddress, sizeof(req.PeerAddress));
        req.ChannelMap = (uint8_t)gAdvParams.channelMap;
        req.FilterPolicy = (GAPSetAdvertisingParametersRequest_FilterPolicy_t)gAdvParams.filterPolicy;
        /* Set advertising parameters and data and start Advertising */
        GAPSetAdvertisingParametersRequest(&req, gFsciInterface_c);
#endif
    }
    else
    {
#if defined(gUseControllerNotifications_c) && (gUseControllerNotifications_c)
        RegisterRemovableObserver(GATTConfirm_FSCI_ID,
                      hsdkObserverGAPControllerEnhancedNotificationRequest);
        GAPControllerEnhancedNotificationRequest_t req;
        req.DeviceId = 0;
        req.EventType = gNotifScanEventOver_c | gNotifScanAdvPktRx_c | gNotifScanRspRx_c | gNotifScanReqTx_c;
        GAPControllerEnhancedNotificationRequest(&req, gFsciInterface_c);
#else
        /* Star scanning */
        GapStartScanningRequest();
#endif
    }
}

/*! *********************************************************************************
 * \brief        Heleper Function to register removable observer.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
#if defined(gUseControllerNotifications_c) && (gUseControllerNotifications_c)
static void hsdkObserverGAPControllerEnhancedNotificationRequest
(
    bleEvtContainer_t *pContainer
)
{
    if (mGapRole == gGapPeripheral_c)
    {
        
        GAPSetAdvertisingParametersRequest_t req;
        /* Copy values from gAdvParams to packed structure req */
        req.MinInterval = gAdvParams.minInterval;
        req.MaxInterval = gAdvParams.maxInterval;
        req.AdvertisingType = (GAPSetAdvertisingParametersRequest_AdvertisingType_t)gAdvParams.advertisingType;
        req.OwnAddressType = (GAPSetAdvertisingParametersRequest_OwnAddressType_t)gAdvParams.ownAddressType;
        req.PeerAddressType = (GAPSetAdvertisingParametersRequest_PeerAddressType_t)gAdvParams.peerAddressType;
        FLib_MemCpy(req.PeerAddress, gAdvParams.peerAddress, sizeof(req.PeerAddress));
        req.ChannelMap = (uint8_t)gAdvParams.channelMap;
        req.FilterPolicy = (GAPSetAdvertisingParametersRequest_FilterPolicy_t)gAdvParams.filterPolicy;
        /* Set advertising parameters and data and start Advertising */
        GAPSetAdvertisingParametersRequest(&req, gFsciInterface_c);
    }
    else
    {
        /* Star scanning */
        GapStartScanningRequest();
    }
}
#endif

/*! *********************************************************************************
 * \brief        Heleper Function to start scanning procedure.
 *
 ********************************************************************************** */
static void GapStartScanningRequest
(
    void
)
{
     /* Star scanning */
    GAPStartScanningRequest_t req;
    req.ScanningParametersIncluded = TRUE;
    req.ScanningParameters.Type = (GAPStartScanningRequest_ScanningParameters_Type_t)gScanParams.type;
    req.ScanningParameters.Interval = gScanParams.interval;
    req.ScanningParameters.Window = gScanParams.window;
    req.ScanningParameters.OwnAddressType = (GAPStartScanningRequest_ScanningParameters_OwnAddressType_t)gScanParams.ownAddressType;
    req.ScanningParameters.FilterPolicy = (GAPStartScanningRequest_ScanningParameters_FilterPolicy_t)gScanParams.filterPolicy;
    req.ScanningPHYs = gScanParams.scanningPHYs;
    req.FilterDuplicates = GAPStartScanningRequest_FilterDuplicates_Enable;
    req.Duration = gGapScanContinuously_d;
    req.Period = gGapScanPeriodicDisabled_d;
    GAPStartScanningRequest(&req, gFsciInterface_c);
}

/*! *********************************************************************************
 * \brief        Heleper Function to stop scanning procedure.
 *
 ********************************************************************************** */
static void BleApp_GapStopScanningRequest
(
    void *pParam
)
{
     /* Stop scanning */
    GAPStopScanningRequest(gFsciInterface_c);
}
/*! *********************************************************************************
* @}
********************************************************************************** */
