/*! *********************************************************************************
* \addtogroup Digital Key Car Anchor CS Application
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file app_digital_key_car_anchor_cs.c
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
#include "fsl_component_led.h"
#include "fsl_component_timer_manager.h"
#include "fsl_os_abstraction.h"
#include "FunctionLib.h"
#include "fsl_component_mem_manager.h"
#include "fsl_adapter_reset.h"
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
#include "fsl_shell.h"
#endif
#if defined(gPlatformUseHwParameter_d) && (gPlatformUseHwParameter_d > 0)
#include "HWParameter.h"
#endif /* defined(gPlatformUseHwParameter_d) && (gPlatformUseHwParameter_d > 0) */
#include "app.h"
#include "fwk_seclib.h"
/* BLE Host Stack */
#include "gatt_server_interface.h"

/* Connection Manager */
#include "ble_conn_manager.h"

#include "board.h"
#include "app_conn.h"
#include "digital_key_car_anchor_cs.h"

#include "shell_digital_key_car_anchor_cs.h"
#include "app_digital_key_car_anchor_cs.h"

#include "gap_handover_types.h"
#include "gap_handover_interface.h"
#include "app_a2a_interface.h"
#include "app_handover.h"
#include "app_a2b.h"
#include "app_localization.h"
#if defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
#include "app_localization_algo.h"
#endif /* defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U) */
#include "pde_rade.h"

#include "controller_api.h"

#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d==1U)
#include "btcs_client_interface.h"
#elif defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
#include "btcs_server_interface.h"
#endif

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define mcEncryptionKeySize_c   16
/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
/* TRUE if the last connect came from a handover procedure */
bool_t mLastConnectFromHandover = FALSE;

/* Save the Id of the device selected for handover */
deviceId_t gHandoverDeviceId = gInvalidDeviceId_c;
#endif

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
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
/* LTK size */
#define gAppLtkSize_c gcSmpMaxBlobSize_c
/* LTK */
static uint8_t gaAppSmpLtk[gcSmpMaxBlobSize_c];
/* IRK */
static uint8_t gaAppSmpIrk[gcSmpMaxBlobSize_c];
#else /* (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U)) */
/* LTK */
static uint8_t gaAppSmpLtk[gcSmpMaxLtkSize_c];
/* LTK size */
#define gAppLtkSize_c mcEncryptionKeySize_c
/* IRK */
static uint8_t gaAppSmpIrk[gcSmpIrkSize_c];
#endif /* (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U)) */
/* RAND*/
static uint8_t gaAppSmpRand[gcSmpMaxRandSize_c];
/* Address */
static uint8_t gaAppAddress[gcBleDeviceAddressSize_c];
static gapSmpKeys_t gAppOutKeys = {
    .cLtkSize = gAppLtkSize_c,
    .aLtk = (void *)gaAppSmpLtk,
    .aIrk = (void *)gaAppSmpIrk,
    .aCsrk = NULL,
    .aRand = (void *)gaAppSmpRand,
    .cRandSize = gcSmpMaxRandSize_c,
    .ediv = 0,
    .addressType = 0,
    .aAddress = gaAppAddress
};
static gapSmpKeyFlags_t gAppOutKeyFlags;
static bool_t gAppOutLeSc;
static bool_t gAppOutAuth;

#if (defined (gAppSecureMode_d) && (gAppSecureMode_d == 0U) || (defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)))
/*
Global used to identify if a bond was added by
shell command or connection with the device
mBondAddedFromShell = FALSE bond created by connection
mBondAddedFromShell = TRUE  bond added by shell command
*/
static bool_t mBondAddedFromShell = FALSE;
#endif
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

static uint8_t mVerbosityLevel = 2U; /* default: all prints enabled */

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
static SERIAL_MANAGER_WRITE_HANDLE_DEFINE(gDataExportSerialWriteHandle);
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
#if defined(gAppSecureMode_d) && (gAppSecureMode_d == 0U)
static bleResult_t SetBondingData
(
    uint8_t nvmIndex,
    bleAddressType_t addressType,
    uint8_t* ltk,
    uint8_t* irk,
    uint8_t* address
);
#endif /* defined(gAppSecureMode_d) && (gAppSecureMode_d == 0U) */
/* CCC Time Sync */
static bleResult_t CCC_TriggerTimeSync(deviceId_t deviceId);
static void AppPrintLePhyEvent(gapPhyEvent_t* pPhyEvent);
static void PrintLePhyEvent(gapPhyEvent_t* pPhyEvent);
static void App_HandleBondShellCmds(void *pData);
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
static void A2A_HandleGenericCallbackBondCreatedEvent(appEventData_t *pEventData);
#else
static void App_HandleGenericCallbackBondCreatedEvent(appEventData_t *pEventData);
#endif /* (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U)) */
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */
static void App_HandleL2capPsmDataCallback(appEventData_t *pEventData);

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
button_status_t BleApp_HandleKeys0(void *buttonHandle, button_callback_message_t *message, void *callbackParam);
#endif /*gAppButtonCnt_c > 0*/

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1))
button_status_t BleApp_HandleKeys1(void *buttonHandle, button_callback_message_t *message, void *callbackParam);
#endif /*gAppButtonCnt_c > 1*/

#if defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1)
static void A2A_ProcessCommand(void *pMsg);
#endif /* defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1) */

#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
static void A2A_CheckLocalIrk(void);
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
static void BleApp_HandoverEventHandler(appHandoverEvent_t eventType, void *pData);
#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1) */
static void BleApp_CsEventHandler(deviceId_t deviceId, void *pData, appCsEventType_t eventType);
#if defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
static void BleApp_PrintMeasurementResults(deviceId_t deviceId, localizationAlgoResult_t *pResult);
#endif /* defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U) */

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
static void BleApp_SetCsConfigParams(appEventData_t* pEventData);
static void BleApp_SetCsProcParams(appEventData_t* pEventData);
static void BleApp_TriggerCsDistanceMeasurement(deviceId_t deviceId);
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
static void BleApp_OP_StartCaller(appCallbackParam_t param);
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0)) */
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1))
static void BleApp_PE_StartCaller(appCallbackParam_t param);
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1)) */
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
static void BleApp_HandoverCommHandler(uint8_t opGroup, uint8_t cmdId, uint16_t len, uint8_t *pData);
#endif

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
static void App_ExportHciDataLog(void *pData);
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 1)
static void App_ExportRemoteDataLog(void *pData);
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 1) */

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
        maPeerInformation[mPeerId].isLinkEncrypted = FALSE;
        maPeerInformation[mPeerId].appState = mAppIdle_c;
        FLib_MemSet(&maPeerInformation[mPeerId].oobData, 0x00, sizeof(gapLeScOobData_t));
        FLib_MemSet(&maPeerInformation[mPeerId].peerOobData, 0x00, sizeof(gapLeScOobData_t));
    }

    LedStartFlashingAllLeds();

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[0], BleApp_HandleKeys0, NULL);
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0)) */
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1))
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[1], BleApp_HandleKeys1, NULL);
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1)) */

    /* Add/modify init code starting from here */

    /* Register the function handler for the user interface events  */
    BleApp_RegisterUserInterfaceEventHandler(APP_UserInterfaceEventHandler);

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
    /* Register the function handler for the shell commands events  */
    AppShell_RegisterCmdHandler(App_HandleShellCmds);
#endif

    /* Register the function handler for the Bluetooth application events  */
    BleApp_RegisterEventHandler(APP_BleEventHandler);
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
    /* Initialize Handover. */
    (void)AppHandover_Init(BleApp_HandoverEventHandler, BleApp_ConnectionCallback, BleApp_HandoverCommHandler);
#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1) */

    /* Set generic callback */
    BluetoothLEHost_SetGenericCallback(BleApp_GenericCallback);

    /* Initialize Bluetooth Host Stack */
    BluetoothLEHost_Init(BluetoothLEHost_Initialized);

#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
    /* Check if local IRK is set. Otherwise postpone BLE Host initialization. */
    A2A_CheckLocalIrk();
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */

    /* UI */
    shellPrompt.constPrompt = "Anchor>";
    AppShellInit(shellPrompt.prompt);
#if defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1)
    (void)A2A_Init(gSerMgrIf2, A2A_ProcessCommand);
#endif /* defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1) */

    /* Register CS callback and initialize localization */
#if defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
    (void)AppLocalization_Init(gCsDefaultRole_c, BleApp_CsEventHandler, BleApp_PrintMeasurementResults);
#else /* defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U) */
    (void)AppLocalization_Init(gCsDefaultRole_c, BleApp_CsEventHandler, NULL);
#endif /* defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U) */

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
    /* Open write handle */
    (void)SerialManager_OpenWriteHandle(gSerMgrIf2, (serial_write_handle_t)gDataExportSerialWriteHandle);
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */
}

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
/*! *********************************************************************************
* \brief        Handles keyboard events.
*
* \param[in]    buttonHandle    button handle
* \param[in]    message         Button press event
* \param[in]    callbackParam   parameter
********************************************************************************** */
button_status_t BleApp_HandleKeys0(void *buttonHandle, button_callback_message_t *message,void *callbackParam)
{
    switch (message->event)
    {
        case kBUTTON_EventOneClick:
        case kBUTTON_EventShortPress:
        {
            (void)App_PostCallbackMessage(BleApp_OP_StartCaller, NULL);
        }
        break;

        /* Disconnect on long button press */
        case kBUTTON_EventLongPress:
        {
            uint8_t mPeerId = 0;
            for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
            {
                if (maPeerInformation[mPeerId].deviceId != gInvalidDeviceId_c)
                {
                    (void)Gap_Disconnect(maPeerInformation[mPeerId].deviceId);
                }
            }
        }
        break;

        /* Very Long Press not available - use Double Click */
        case kBUTTON_EventDoubleClick:
        {
            /* Factory reset*/
            BleApp_FactoryReset();
        }
        break;

        default:
        {
            ; /* No action required */
        }
        break;
    }

    return kStatus_BUTTON_Success;
}
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0)) */

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1))
/*! *********************************************************************************
* \brief        Handles keyboard events.
*
* \param[in]    buttonHandle    button handle
* \param[in]    message         Button press event
* \param[in]    callbackParam   parameter
********************************************************************************** */
button_status_t BleApp_HandleKeys1(void *buttonHandle, button_callback_message_t *message,void *callbackParam)
{
    switch (message->event)
    {
        case kBUTTON_EventOneClick:
        case kBUTTON_EventShortPress:
        {
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
            bleResult_t status = gBleSuccess_c;

            gHandoverDeviceId = BleApp_SelectDeviceIdForHandover();

            if (gHandoverDeviceId == gInvalidDeviceId_c)
            {
                shell_write("\r\n Handover device id error.\r\n");
                status = gBleInvalidState_c;
            }
            else
            {
                appLocalization_State_t locState = AppLocalization_GetLocState(gHandoverDeviceId);
                if (locState == gAppLclIdle_c)
                {
                    AppHandover_SetPeerDevice(gHandoverDeviceId);
                    status = AppHandover_StartTimeSync(TRUE);
                    shell_write("\r\nHandover started.\r\n");
                }
                else
                {
                    shell_write("\r\nCannot start handover while a CS procedure is in progress!\r\n");
                    shell_cmd_finished();
                }
            }

            if (status != gBleSuccess_c)
            {
                shell_write("\r\nHandover time synchronization error.\r\n");
                shell_cmd_finished();
            }
#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1) */
        }
        break;

        case kBUTTON_EventLongPress:
        {
            (void)App_PostCallbackMessage(BleApp_PE_StartCaller, NULL);
        }
        break;

        default:
        {
            ; /* No action required */
        }
        break;
    }

    return kStatus_BUTTON_Success;
}
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1)) */

/*!*************************************************************************************************
* \fn     uint8_t APP_UserInterfaceEventHandler(uint8_t *pData)
* \brief  This function is used to handle User Interface events from BLE
*         To register another function use BleApp_RegisterUserInterfaceEventHandler
*
* \param  [in]   pData - pointer to appEventData_t data;
 ***************************************************************************************************/
void APP_UserInterfaceEventHandler(void *pData)
{
    /* Here custom code can be added to handle user interface update based on application events */
    appEventData_t *pEventData = (appEventData_t *)pData;
    switch(pEventData->appEvent)
    {
        case mAppEvt_PeerConnected_c:
        {
            shell_write("\r\nConnected!\r\n");
        }
        break;

        case mAppEvt_PsmChannelCreated_c:
        {
            shell_write("\r\nL2CAP PSM Connection Complete.\r\n");
        }
        break;

        case mAppEvt_PairingPeerOobDataRcv_c:
        {
            shell_write("\r\nReceived First_Approach_RQ.\r\n");
        }
        break;

        case mAppEvt_PeerDisconnected_c:
        {
            shell_write("Disconnected with reason ");
            shell_writeDec((uint32_t)maPeerInformation[pEventData->eventData.peerDeviceId].disconReason);
            shell_write("!\r\n");
            shell_cmd_finished();
        }
        break;

        case mAppEvt_PairingLocalOobData_c:
        {
            shell_write("\r\nSending First_Approach_RS\r\n");
        }
        break;

        case mAppEvt_PairingComplete_c:
        {
            shell_write("\r\nPairing successful.\r\n");
            shell_cmd_finished();
        }
        break;

        case mAppEvt_PairingReqRcv_c:
        {
            shell_write("\r\nPairing...\r\n");
        }
        break;

        case mAppEvt_SPAKERequestSent_c:
        {
            shell_write("\r\nSPAKE Request sent.\r\n");
        }
        break;

        case mAppEvt_SPAKEVerifySent_c:
        {
            shell_write("\r\nSPAKE Verify sent.\r\n");
        }
        break;

        case mAppEvt_AdvertisingStartedLegacy_c:
        {
            shell_write("Advertising started - Legacy.\r\n");
        }
        break;
#if defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1)
        case mAppEvt_AdvertisingStartedExtendedLR_c:
        {
            shell_write("Advertising started - Extended LR.\r\n");
        }
        break;
#endif /* defined(gAppLeCodedAdvEnable_d) && (gAppLeCodedAdvEnable_d == 1) */

        case mAppEvt_AdvertisingStopped_c:
        {
            shell_write("Advertising stopped - All PHYs.\r\n");
        }
        break;

        case mAppEvt_BleConfigDone_c:
        {
            shell_write("\r\nDigital Key Car Anchor.\r\n");
            shell_cmd_finished();
        }
        break;

        case mAppEvt_BleScanning_c:
        {
            shell_write("Scanning...\r\n");
        }
        break;

        case mAppEvt_BleScanStopped_c:
        {
            shell_write("Scan stopped.\r\n");
        }
        break;

        case mAppEvt_BleConnectingToDevice_c:
        {
            shell_write("Connecting...\r\n");
        }
        break;

        case mAppEvt_LePhyEvent_c:
        {
            gapPhyEvent_t *pPhyEvent = (gapPhyEvent_t *)pEventData->eventData.pData;

            if (pPhyEvent->phyEventType == gPhyRead_c )
            {
                appLocalization_rangeCfg_t locConfig;

                /* Read current CS config */
                (void)AppLocalization_ReadConfig(pPhyEvent->deviceId, &locConfig);

                /* Set the PHY according to the connection PHY */
                if (pPhyEvent->rxPhy == gLePhyCoded_c)
                {
                    /* This event does not differentiate between coding schemes,
                       but Channel Sounding does - application uses S2 */
                    locConfig.phy = gPowerControlLePhyCodedS2_c;
                }
                else
                {
                    locConfig.phy = pPhyEvent->rxPhy;
                }

                /* Update CS config with the PHY */
                (void)AppLocalization_WriteConfig(pPhyEvent->deviceId, &locConfig);
            }

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
            if (pPhyEvent->phyEventType == gPhyUpdateComplete_c )
            {
                AppPrintLePhyEvent((gapPhyEvent_t *)pEventData->eventData.pData);
            }
#endif
        }
        break;
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
        case mAppEvt_Shell_HandoverError_c:
        {
            switch (pEventData->eventData.handoverError)
            {
                case mAppHandover_NoActiveConnection_c:
                {
                    shell_write("\r\nNo active connection to transfer.\r\n");
                }
                break;

                case mAppHandover_TimeSyncTx_c:
                {
                    shell_write("\r\nTime synchronization transmit error.\r\n");
                }
                break;

                case mAppHandover_AnchorSearchStartFailed_c:
                {
                    shell_write("\r\nAnchor search start failed.\r\n");
                }
                break;

                case mAppHandover_PeerBondingDataInvalid_c:
                {
                    shell_write("\r\nInvalid bonding data.\r\n");
                }
                break;

                case mAppHandover_UnexpectedError_c:
                {
                    shell_write("\r\nUnexpected handover error.\r\n");
                }
                break;

                case mAppHandover_AnchorSearchFailedToSync_c:
                {
                    shell_write("\r\nHandover failed - Anchor search unsuccessful.\r\n");
                }
                break;

                case mAppHandover_OutOfMemory_c:
                {
                    shell_write("\r\nOut of memory error.\r\n");
                }
                break;

                case mAppHandover_ConnParamsUpdateFail_c:
                {
                    shell_write("\r\nConnection parameters update failed.\r\n");
                }
                break;

                default:
                {
                    shell_write("\r\nHandover error ");
                    shell_writeDec((uint32_t)(pEventData->eventData.handoverError));
                    shell_write(".\r\n");
                    shell_cmd_finished();
                }
                break;
            }

            shell_cmd_finished();
        }
        break;

        case mAppEvt_Shell_HandoverCompleteConnected_c:
        {
            shell_write("\r\nHandover complete, connected.\r\n");
            shell_cmd_finished();
        }
        break;

        case mAppEvt_Shell_HandoverCompleteDisconnected_c:
        {
            shell_write("\r\nHandover complete, disconnected.\r\n");
            shell_cmd_finished();
        }
        break;

        case mAppEvt_Shell_HandoverStarted_c:
        {
            if (pEventData->eventData.handoverTimeSync == TRUE)
            {
                shell_write("\r\nHandover started.\r\n");
            }
            else
            {
                shell_write("\r\nAnchor Monitor started.\r\n");
            }
            shell_cmd_finished();
        }
        break;

        case mAppEvt_Shell_AnchorMonitorEventReceived_c:
        {
            shell_write("\r\nRSSI event received for device id: ");
            shell_writeDec(pEventData->eventData.anchorMonitorEvent.deviceId);
            shell_write("\r\n");
        }
        break;

        case mAppEvt_Shell_PacketMonitorEventReceived_c:
        {
            shell_write("\r\nPacket monitor event received for device id ");
            shell_writeDec(pEventData->eventData.anchorPacketEvent.deviceId);
            shell_write(", ");

            /* Status bit1: packet transmitter central (1) or peripheral (0) */
            if ((pEventData->eventData.anchorPacketEvent.pktMntEvt.statusPacket & BIT1) != 0U)
            {
                shell_write("from central");
            }
            else
            {
                shell_write("from peripheral");
            }

            shell_write(" with RSSI: ");
            if(((uint8_t)pEventData->eventData.anchorPacketEvent.pktMntEvt.rssiPacket >> 7) != 0U)
            {
                shell_write("-");

                uint8_t aux = ~((uint8_t)pEventData->eventData.anchorPacketEvent.pktMntEvt.rssiPacket - 1U);
                pEventData->eventData.anchorPacketEvent.pktMntEvt.rssiPacket = aux;
            }

            shell_writeDec(pEventData->eventData.anchorPacketEvent.pktMntEvt.rssiPacket);
            shell_write("\r\n");
            /* Free pdu memory */
            (void)MEM_BufferFree(pEventData->eventData.anchorPacketEvent.pktMntEvt.pPdu);
        }
        break;

        case mAppEvt_Shell_PacketMonitorContinueEventReceived_c:
        {
            shell_write("\r\nPacket continue monitor event received for device id ");
            shell_writeDec(pEventData->eventData.anchorPacketContinueEvent.deviceId);
            shell_write("\r\n");
            /* Free pdu memory */
            (void)MEM_BufferFree(pEventData->eventData.anchorPacketContinueEvent.pktMntCntEvt.pPdu);
        }
        break;
#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1) */
#if defined(gA2BEnabled_d) && (gA2BEnabled_d == 1)
        case mAppEvt_Shell_A2BKeyDerivationComplete_c:
        {
            shell_write("\r\nE2E key derivation successful.\r\n");
            shell_cmd_finished();
        }
        break;

        case mAppEvt_Shell_A2BLocalIrkSyncComplete_c:
        {
            shell_write("\r\nE2E local IRK sync successful.\r\n");
            shell_cmd_finished();
        }
        break;

        case mAppEvt_Shell_A2BError_c:
        {
            switch (pEventData->eventData.a2bError)
            {
                case mAppA2B_E2EKeyDerivationFailiure_c:
                {
                    shell_write("\r\nE2E key derivation failed.\r\n");
                    shell_cmd_finished();
                }
                break;

                case mAppA2B_E2ELocalIrkSyncFailiure_c:
                {
                    shell_write("\r\nE2E local IRK sync failed.\r\n");
                    shell_cmd_finished();
                }
                break;

                default:
                {
                    shell_write("\r\nA2B error.\r\n");
                    shell_cmd_finished();
                }
                break;
            }
        }
        break;
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d == 1) */
        default:
        {
            ; /* No action required */
        }
        break;
    }

    (void)MEM_BufferFree(pData);
    pData = NULL;
}

/*!*************************************************************************************************
 *\fn     uint8_t APP_BleEventHandler(uint8_t *pData)
 *\brief  This function is used to handle events from BLE
 *        To register another function use BleApp_RegisterEventHandler
 *
 *\param  [in]   pData - pointer to appEventData_t data;
 ***************************************************************************************************/
void APP_BleEventHandler(void *pData)
{
    appEventData_t *pEventData = (appEventData_t *)pData;

    switch(pEventData->appEvent)
    {
        case mAppEvt_GenericCallback_LeScLocalOobData_c:
        {
            /* Code for CCC demo application */
            (void)App_HandleLeScLocalOobDataCallback(pEventData);
        }
        break;

        case mAppEvt_GenericCallback_BondCreatedEvent_c:
        {
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
            A2A_HandleGenericCallbackBondCreatedEvent(pEventData);
#else
            App_HandleGenericCallbackBondCreatedEvent(pEventData);
#endif /* (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U)) */
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */
        }
        break;

        case mAppEvt_L2capPsmDataCallback_c:
        {
            App_HandleL2capPsmDataCallback(pEventData);
        }
        break;

        case mAppEvt_FactoryReset_c:
        {
            BleApp_FactoryReset();
        }
        break;

#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
        case mAppEvt_L2capPsmChannelStatusNotification_c:
        {
            l2caLeCbChannelStatusNotification_t* pCBChStatusNotif = pEventData->eventData.pData;

            if ((pCBChStatusNotif->status == gL2ca_ChannelStatusChannelIdle_c) &&
                (BtcsServer_CheckTransferInProgress(pCBChStatusNotif->deviceId) == TRUE))
            {
                (void)BtcsServer_SendData(pCBChStatusNotif->deviceId, pCBChStatusNotif->cId, gRangingProcResCont_c);
            }
        }
        break;
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */

        default:
        {
            ; /* No action required */
        }
        break;
    }

    (void)MEM_BufferFree(pData);
    pData = NULL;
}

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
/*! *********************************************************************************
* \brief        Handles Shell Commands events.
*
* \param[in]    pEventData    pointer to appEventData_t.
********************************************************************************** */
void App_HandleShellCmds(void *pData)
{
    appEventData_t *pEventData = (appEventData_t *)pData;

    switch(pEventData->appEvent)
    {
        case mAppEvt_Shell_Reset_Command_c:
        {
            HAL_ResetMCU();
        }
        break;

        case mAppEvt_Shell_FactoryReset_Command_c:
        {
            /* Factory Reset for the CCC demo application*/
            BleApp_FactoryReset();
        }
        break;

        case mAppEvt_Shell_ShellStartDiscoveryOP_Command_c:
        {
            /* start owner pairing scenario */
            BleApp_OP_Start();
        }
        break;

        case mAppEvt_Shell_ShellStartDiscoveryPE_Command_c:
        {
            /* start passive entry scenario */
            BleApp_PE_Start();
        }
        break;

        case mAppEvt_Shell_StopDiscovery_Command_c:
        {
            /* stop discovery */
            BleApp_StopDiscovery();
        }
        break;

        case mAppEvt_Shell_Disconnect_Command_c:
        {
            BleApp_Disconnect();
        }
        break;

        case mAppEvt_Shell_TriggerTimeSync_Command_c:
        {
            (void)CCC_TriggerTimeSync(pEventData->peerDeviceId);
        }
        break;

        case mAppEvt_Shell_SetBondingData_Command_c:
        case mAppEvt_Shell_ListBondedDev_Command_c:
        case mAppEvt_Shell_RemoveBondedDev_Command_c:
        {
            App_HandleBondShellCmds(pData);
        }
        break;
        
        case mAppEvt_Shell_ListActiveDev_Command_c:
        {
            bool_t found = FALSE;
            bool_t  peerBonded = FALSE;
            uint8_t peerNvmIndex = 0U;
            bleResult_t result = gBleUnavailable_c;
            gapSmpKeys_t outKeys = {};
            gapSmpKeyFlags_t outKeyFlags = 0U;
            bool_t outLeSc = FALSE;
            bool_t outAuth = FALSE;
            uint8_t aBleDeviceAddress[gcBleDeviceAddressSize_c] = {};
            outKeys.aAddress = aBleDeviceAddress;

            shell_write("\r\nDevId      AddrType    Address\r\n");
            for (uint8_t i = 0U; i < (uint8_t)gAppMaxConnections_c; i++)
            {
                if (maPeerInformation[i].deviceId == gInvalidDeviceId_c)
                {
                    continue;
                }

                result = Gap_CheckIfBonded(maPeerInformation[i].deviceId , &peerBonded, &peerNvmIndex);

                if ((result == gBleSuccess_c) && (peerBonded == TRUE))
                {
                    result = Gap_LoadKeys(peerNvmIndex, &outKeys, &outKeyFlags, &outLeSc, &outAuth);
                }
                else
                {
                    result = gBleUnavailable_c;
                }

                if (result == gBleSuccess_c)
                {
                    shell_writeHex((uint8_t*)&maPeerInformation[i].deviceId, (uint8_t)sizeof(uint8_t));
                    shell_write("         ");
                    shell_writeHex((uint8_t*)&outKeys.addressType, (uint8_t)sizeof(uint8_t));
                    shell_write("          ");
                    shell_writeHex((uint8_t*)&outKeys.aAddress, gcBleDeviceAddressSize_c);
                    shell_write("\r\n");
                    found = TRUE;
                }
            }

            if(found == FALSE)
            {
                shell_write(" No active devices ");
            }

            shell_cmd_finished();
        }
        break;

        case mAppEvt_Shell_HandoverSendL2cap_Command_c:
        {
            deviceId_t handoverDeviceId = gInvalidDeviceId_c;

            handoverDeviceId = BleApp_SelectDeviceIdForHandover();

            if (handoverDeviceId != gInvalidDeviceId_c)
            {
                char msg[] = L2CAP_SAMPLE_MESSAGE;
                (void)L2ca_SendLeCbData(handoverDeviceId, maPeerInformation[handoverDeviceId].customInfo.psmChannelId,
                                        (const uint8_t *)msg, (uint16_t)FLib_StrLen(L2CAP_SAMPLE_MESSAGE));
            }
        }
        break;

#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
        case mAppEvt_Shell_HandoverStartAnchorMonitor_Command_c:
        {
            if (pEventData->eventData.monitorStart.deviceId != gInvalidDeviceId_c)
            {
                bleResult_t result = gBleSuccess_c;
                result = AppHandover_SetMonitorMode(pEventData->eventData.monitorStart.deviceId, pEventData->eventData.monitorStart.monitorMode);

                if (result == gBleSuccess_c)
                {
                    AppHandover_SetPeerDevice(pEventData->eventData.monitorStart.deviceId);
                    result = AppHandover_StartTimeSync(FALSE);
                }

                if (result != gBleSuccess_c)
                {
                    shell_write("\r\nAnchor monitor start failed");
                }
            }
        }
        break;

        case mAppEvt_Shell_HandoverStopAnchorMonitor_Command_c:
        {
            bleResult_t result = gBleInvalidParameter_c;

            result = AppHandover_AnchorMonitorStop(pEventData->peerDeviceId);

            if (result != gBleSuccess_c)
            {
                shell_write("\r\nAnchor monitor stop failed");
            }
        }
        break;

        case mAppEvt_Shell_Handover_Command_c:
        {
            bleResult_t result = gBleInvalidParameter_c;
            deviceId_t handoverDeviceId = pEventData->eventData.peerDeviceId;
            shell_write("\r\nHandover started.\r\n");

            if (maPeerInformation[handoverDeviceId].deviceId == gInvalidDeviceId_c)
            {
                shell_write("\r\n Handover device id error.\r\n");
                result = gBleInvalidState_c;
            }
            else
            {
                gHandoverDeviceId = handoverDeviceId;
                AppHandover_SetPeerDevice(handoverDeviceId);
                result = AppHandover_StartTimeSync(TRUE);
            }
            
            if (result != gBleSuccess_c)
            {
                shell_write("\r\nHandover time synchronization error.\r\n");
                shell_cmd_finished();
            }
        }
        break;
#endif /* gHandoverIncluded_d */
        case mAppEvt_Shell_SetVerbosityLevel_Command_c:
        {
            /* Set verbosity level for CS procedures */
            mVerbosityLevel = pEventData->eventData.verbosityLevel;
        }
        break;

        case mAppEvt_Shell_SetCsConfigParams_Command_c:
        {
            BleApp_SetCsConfigParams(pEventData);
        }
        break;

        case mAppEvt_Shell_SetCsProcedureParams_Command_c:
        {
            BleApp_SetCsProcParams(pEventData);
        }
        break;

        case mAppEvt_Shell_TriggerCsDistanceMeasurement_Command_c:
        {
            BleApp_TriggerCsDistanceMeasurement(pEventData->peerDeviceId);
        }
        break;
#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
        case mAppEvt_Shell_SetAlgorithm_Command_c:
        {
            AppLocalization_SetAlgorithm(pEventData->eventData.algorithmSelection);
        }
        break;
#endif /* defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1U) */
        case mAppEvt_Shell_SetNumProcs_Command_c:
        {
            AppLocalization_SetNumberOfProcedures(pEventData->peerDeviceId, pEventData->eventData.numProcedures);
        }
        break;

        default:
        {
            ; /* No action required */
        }
        break;
    }

    (void)MEM_BufferFree(pData);
    pData = NULL;
}

#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
/*! *********************************************************************************
* \brief        Handler function for APP Handover events.
*
********************************************************************************** */
void BleApp_A2BEventHandler(appA2BEvent_t eventType, void *pData)
{

    switch (eventType)
    {
        case mAppA2B_E2EKeyDerivationComplete_c:
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));

            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_A2BKeyDerivationComplete_c;
                pEventData->eventData.pData = NULL;
                if (gBleSuccess_c != App_PostCallbackMessage(APP_UserInterfaceEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pEventData);
                }
            }
        }
        break;

        case mAppA2B_E2ELocalIrkSyncComplete_c:
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));

            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_A2BLocalIrkSyncComplete_c;
                pEventData->eventData.pData = NULL;
                if (gBleSuccess_c != App_PostCallbackMessage(APP_UserInterfaceEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pEventData);
                }
            }
        }
        break;

        case mAppA2B_Error_c:
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));

            if(pEventData != NULL)
            {
                appA2BError_t error = *(appA2BError_t *)pData;
                pEventData->appEvent = mAppEvt_Shell_A2BError_c;
                pEventData->eventData.a2bError = error;
                if (gBleSuccess_c != App_PostCallbackMessage(APP_UserInterfaceEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pEventData);
                }
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
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
/*! *********************************************************************************
* \brief        Set CS Create Config default parameters.
*
********************************************************************************** */
static void BleApp_SetCsConfigParams(appEventData_t* pEventData)
{
    appLocalization_rangeCfg_t csConfigParams;
    appCsConfigParams_t *pAppCsConfigParams = pEventData->eventData.pData;
    deviceId_t deviceId = pEventData->peerDeviceId;

    /* Read current configuration. */
    (void)AppLocalization_ReadConfig(deviceId, &csConfigParams);

    /* Update configuration with the new values. */
    csConfigParams.main_mode_type = pAppCsConfigParams->mainModeType;
    csConfigParams.sub_mode_type = pAppCsConfigParams->subModeType;
    csConfigParams.main_mode_min = pAppCsConfigParams->mainModeMinSteps;
    csConfigParams.main_mode_max = pAppCsConfigParams->mainModeMaxSteps;
    csConfigParams.main_mode_repeat = pAppCsConfigParams->mainModeRepetition;
    csConfigParams.mode0_nb = pAppCsConfigParams->mode0Steps;
    mGlobalRangeSettings.role = pAppCsConfigParams->role;
    csConfigParams.rtt_type = pAppCsConfigParams->RTTType;
    FLib_MemCpy(csConfigParams.ch_map, pAppCsConfigParams->channelMap, APP_LOCALIZATION_CH_MAP_LEN);
    csConfigParams.ch_map_repeat = pAppCsConfigParams->channelMapRepetition;
    csConfigParams.channelSelectionType = pAppCsConfigParams->channelSelectionType;

    (void)AppLocalization_WriteConfig(deviceId, &csConfigParams);
};

/*! *********************************************************************************
* \brief        Set CS Procedure default parameters.
*
********************************************************************************** */
static void BleApp_SetCsProcParams(appEventData_t* pEventData)
{
    appLocalization_rangeCfg_t csConfigParams;
    appCsProcedureParams_t *pAppCsProcParams = pEventData->eventData.pData;
    deviceId_t deviceId = pEventData->peerDeviceId;

    /* Read current configuration. */
    (void)AppLocalization_ReadConfig(deviceId, &csConfigParams);

    /* Update configuration with the new values. */
    csConfigParams.maxProcedureDuration = pAppCsProcParams->maxProcedureDuration;
    csConfigParams.minPeriodBetweenProcedures = pAppCsProcParams->minPeriodBetweenProcedures;
    csConfigParams.maxPeriodBetweenProcedures = pAppCsProcParams->maxPeriodBetweenProcedures;
    csConfigParams.maxNumProcedures = pAppCsProcParams->maxNumProcedures;
    csConfigParams.minSubeventLen = pAppCsProcParams->minSubeventLen;
    csConfigParams.maxSubeventLen = pAppCsProcParams->maxSubeventLen;
    csConfigParams.ant_cfg_index = pAppCsProcParams->antCfgIndex;

    (void)AppLocalization_WriteConfig(deviceId, &csConfigParams);
}

/*! *********************************************************************************
* \brief        Trigger CS distance measurement.
*
********************************************************************************** */
static void BleApp_TriggerCsDistanceMeasurement(deviceId_t deviceId)
{
    bleResult_t result = gBleSuccess_c;

    /* Reset data before starting a new procedure */
    AppLocalization_ResetPeer(deviceId, FALSE, gInvalidNvmIndex_c);

    result = AppLocalization_SetProcedureParameters(deviceId);

    if (result != gBleSuccess_c)
    {
        shell_write("\r\nCS distance measurement failed.\r\n");
    }
}

/*! *********************************************************************************
* \brief        Handles mAppEvt_Shell_SetBondingData_Command_c,
*               mAppEvt_Shell_ListBondedDev_Command_c,
*               mAppEvt_Shell_RemoveBondedDev_Command_c events.
*
* \param[in]    pEventData    pointer to appEventData_t.
********************************************************************************** */
static void App_HandleBondShellCmds(void *pData)
{
    appEventData_t *pEventData = (appEventData_t *)pData;

    switch(pEventData->appEvent)
    {
        case mAppEvt_Shell_SetBondingData_Command_c:
        {
#if defined (gAppSecureMode_d) && (gAppSecureMode_d == 1U)
            shell_write("\r\nsetbd command not available in Secure Mode.\r\n");
            shell_cmd_finished();
#else
            /* Set address type, LTK, IRK and pear device address  */
            appBondingData_t *pAppBondingData = (appBondingData_t *)pEventData->eventData.pData;

            bleResult_t status = gBleSuccess_c;
            status = SetBondingData(pAppBondingData->nvmIndex, pAppBondingData->addrType, pAppBondingData->aLtk,
                                    pAppBondingData->aIrk, pAppBondingData->deviceAddr);
            if ( status != gBleSuccess_c )
            {
                shell_write("\r\nsetbd failed with status: ");
                shell_writeHex((uint8_t*)&status, 2);
                shell_write("\r\n");
                shell_cmd_finished();
            }
            else
            {
#if defined(gAppUseBonding_d) && (gAppUseBonding_d == 1)
                (void)Gap_AddDeviceToFilterAcceptList(pAppBondingData->addrType, pAppBondingData->deviceAddr);
#endif
                mBondAddedFromShell = TRUE;
            }
#endif /* defined (gAppSecureMode_d) && (gAppSecureMode_d == 0U) */
        }
        break;

        case mAppEvt_Shell_ListBondedDev_Command_c:
        {
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
            gapIdentityInformation_t aIdentity[gMaxBondedDevices_c];
            uint8_t nrBondedDevices = 0;
            uint8_t foundBondedDevices = 0;
            bleResult_t result = Gap_GetBondedDevicesIdentityInformation(aIdentity, gMaxBondedDevices_c, &nrBondedDevices);
            if (gBleSuccess_c == result && nrBondedDevices > 0U)
            {
                for (int8_t i = 0; i < gMaxBondedDevices_c; i++)
                {
                    result = Gap_LoadKeys((uint8_t)i, &gAppOutKeys, &gAppOutKeyFlags, &gAppOutLeSc, &gAppOutAuth);
                    if (gBleSuccess_c == result && nrBondedDevices > 0U)
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
#endif /* gAppUseBonding_d */
        }
        break;

        case mAppEvt_Shell_RemoveBondedDev_Command_c:
        {
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
            bleResult_t result = gGapSuccess_c;
            result = Gap_LoadKeys(pEventData->peerDeviceId, &gAppOutKeys, &gAppOutKeyFlags, &gAppOutLeSc, &gAppOutAuth);
            if(result == gBleSuccess_c)
            {
                if(gBleSuccess_c == Gap_RemoveDeviceFromFilterAcceptList(gAppOutKeys.addressType, gAppOutKeys.aAddress))
                {
                    /* Remove bond based on nvm index stored in eventData.peerDeviceId */
                    if ((Gap_RemoveBond(pEventData->peerDeviceId) == gBleSuccess_c))
                    {
                        gcBondedDevices--;
                        gPrivacyStateChangedByUser = TRUE;
                        (void)BleConnManager_DisablePrivacy();
                        shell_write("\r\nBond removed!\r\n");
                        shell_cmd_finished();
                    }
                    else
                    {
                        shell_write("\r\nOperation failed!\r\n");
                        shell_cmd_finished();
                    }
                }
                else
                {
                     shell_write("\r\nOperation failed!\r\n");
                     shell_cmd_finished();
                }
            }
            else
            {
                shell_write("\r\nRemoved bond failed because unable to load the keys from the bond.\r\n");
                shell_cmd_finished();
            }
#endif /* gAppUseBonding_d */
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
* \brief        Prints phy event.
*
********************************************************************************** */
static void AppPrintLePhyEvent(gapPhyEvent_t* pPhyEvent)
{
    PrintLePhyEvent(pPhyEvent);
}

/*! *********************************************************************************
* \brief        Prints phy event.
*
********************************************************************************** */
static void PrintLePhyEvent(gapPhyEvent_t* pPhyEvent)
{
    /* String dictionary corresponding to gapLePhyMode_t */
    static const char* mLePhyModeStrings[] =
    {
        "Invalid\r\n",
        "1M\r\n",
        "2M\r\n",
        "Coded\r\n",
    };
    uint8_t txPhy = ((gapLePhyMode_tag)(pPhyEvent->txPhy) <= gLePhyCoded_c) ? pPhyEvent->txPhy : 0U;
    uint8_t rxPhy = ((gapLePhyMode_tag)(pPhyEvent->rxPhy) <= gLePhyCoded_c) ? pPhyEvent->rxPhy : 0U;
    shell_write("Phy Update Complete.\r\n");
    shell_write("TxPhy ");
    shell_write(mLePhyModeStrings[txPhy]);
    shell_write("RxPhy ");
    shell_write(mLePhyModeStrings[rxPhy]);
    shell_cmd_finished();
}
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

/*! *********************************************************************************
* \brief        Handles L2capPsmDataCallback events.
*
* \param[in]    pEventData    pointer to appEventData_t.
********************************************************************************** */
static void App_HandleL2capPsmDataCallback(appEventData_t *pEventData)
{

    appEventL2capPsmData_t *l2capDataEvent = (appEventL2capPsmData_t *)pEventData->eventData.pData;
    deviceId_t deviceId = l2capDataEvent->deviceId;
    uint16_t packetLength = l2capDataEvent->packetLength;
    uint8_t* pPacket = l2capDataEvent->pPacket;

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
                                shell_write("\r\nReceived Command Complete SubEvent: Request_owner_pairing.\r\n");
                                BleApp_StateMachineHandler(deviceId, mAppEvt_OwnerPairingRequestReceived_c);
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
                        uint8_t *pData = &pPacket[gMessageHeaderSize_c + gPayloadHeaderSize_c + gLengthFieldSize_c];

                        /* BD address not used. */
                        pData = &pData[gcBleDeviceAddressSize_c];
                        /* Confirm Value */
                        FLib_MemCpy(&maPeerInformation[deviceId].peerOobData.confirmValue, pData, gSmpLeScRandomConfirmValueSize_c);
                        pData = &pData[gSmpLeScRandomConfirmValueSize_c];
                        /* Random Value */
                        FLib_MemCpy(&maPeerInformation[deviceId].peerOobData.randomValue, pData, gSmpLeScRandomValueSize_c);

                        /* Send event to the application state machine. */
                        BleApp_StateMachineHandler(deviceId, mAppEvt_PairingPeerOobDataRcv_c);
                    }
                    else
                    {
                        shell_write("\r\nERROR: Invalid length for FirstApproachRQ.\r\n");
                    }
                }
                else if (msgId == (uint8_t)gTimeSync_c)
                {
                    shell_write("\r\nTime Sync received.\r\n");
                    shell_cmd_finished();
                    /* Handle Time Sync data */
                    (void)mTsUwbDeviceTime;
                }
                else
                {
                    /* For MISRA compliance */
                }
            }
            break;

            case gDKMessageTypeFrameworkMessage_c:
            {
                switch (maPeerInformation[deviceId].appState)
                {
                    case mAppCCCPhase2WaitingForResponse_c:
                    {
                         /* Process the data in pPacket here */
                         shell_write("\r\nSPAKE Response received.\r\n");
                         BleApp_StateMachineHandler(deviceId, mAppEvt_ReceivedSPAKEResponse_c);
                    }
                    break;

                    case mAppCCCPhase2WaitingForVerify_c:
                    {
                        /* Process the data in pPacket here */
                        shell_write("\r\nSPAKE Verify received.\r\n");
                        shell_write("\r\nSending Command Complete SubEvent: BLE_pairing_ready\r\n");
                        (void)CCC_SendSubEvent(deviceId, gCommandComplete_c, gBlePairingReady_c);
                        BleApp_StateMachineHandler(deviceId, mAppEvt_BlePairingReady_c);
                    }
                    break;

                    default:
                    {
                        ; /* For MISRA compliance */
                    }
                    break;
                }
            }
            break;

#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
            case gDKMessageTypeBTCSRangingServiceMessage_c:
            {
                bleResult_t result = BtcsClient_HandleRangingServiceMsg(deviceId, pPacket);
                if (result != gBleSuccess_c)
                {
                    shell_write("\r\nReceiving BTCS data fail! Error: ");
                    shell_writeDec((uint32_t)result);
                    SHELL_NEWLINE();
                }
            }
            break;
#endif

#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
            case gDKMessageTypeBTCSRangingServiceMessage_c:
            {
                if (msgId == (uint8_t)gRangingProcResCfg_c)
                {
                    uint8_t *pData = &pPacket[gMessageHeaderSize_c + gPayloadHeaderSize_c + gLengthFieldSize_c];
                    bool_t bEnableProcResTransfer = (bool_t)(*pData);

                    if (bEnableProcResTransfer == TRUE)
                    {
                        shell_write("\r\nReceived BTCS Ranging Procedure Results Config\r\n");
                        BleApp_StateMachineHandler(deviceId, mAppEvt_BtcsRangingProcResCfg_c);
                    }
                }
            }
            break;
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */

            default:
            {
                ; /* For MISRA compliance */
            }
            break;
        }
    }
}

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
/*! *********************************************************************************
* \brief        Handles GenericCallback events.
*
* \param[in]    pEventData    pointer to appEventData_t.
********************************************************************************** */
static void A2A_HandleGenericCallbackBondCreatedEvent(appEventData_t *pEventData)
{
    bleBondCreatedEvent_t *pBondEventData = (bleBondCreatedEvent_t *)pEventData->eventData.pData;
    bleResult_t status = gBleSuccess_c;
#if (defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U))
    secResultType_t secStatus = gSecSuccess_c;
#endif /* (defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)) */

    status = Gap_LoadKeys(pBondEventData->nvmIndex,
                          &gAppOutKeys, &gAppOutKeyFlags, &gAppOutLeSc,
                          &gAppOutAuth);

    if ( status == gBleSuccess_c)
    {
        /* address type, address, ltk, irk */
        shell_write("\r\nBondingData: ");
        shell_writeHex((uint8_t*)&gAppOutKeys.addressType, 1);
        shell_write(" ");
        shell_writeHex(gAppOutKeys.aAddress, gcBleDeviceAddressSize_c);
        shell_write(" ");
        shell_writeHex((uint8_t*)gAppOutKeys.aLtk, gcSmpMaxBlobSize_c);
        shell_write(" ");
        shell_writeHex((uint8_t*)gAppOutKeys.aIrk, gcSmpIrkSize_c);
        shell_write("\r\n");
#if (defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U))
        if (mBondAddedFromShell == FALSE)
        {
            /* Bonding data containing E2E blobs received from the other Anchor. */
            /* get LTK E2E blob */
            uint8_t aTempKeyData[gcSmpMaxBlobSize_c];
            FLib_MemCpy(aTempKeyData, gAppOutKeys.aLtk, gcSmpMaxBlobSize_c);
            if (gSecLibFunctions.pfSecLib_ExportA2BBlob != NULL)
            {
                secStatus = gSecLibFunctions.pfSecLib_ExportA2BBlob(aTempKeyData, gSecLtkElkeBlob_c, gAppOutKeys.aLtk);
            }
            else
            {
                secStatus = gSecError_c;
            }

            if (gSecSuccess_c == secStatus)
            {
                /* get IRK E2E blob */
                FLib_MemCpy(aTempKeyData, gAppOutKeys.aIrk, gcSmpIrkSize_c);
                if (gSecLibFunctions.pfSecLib_ExportA2BBlob != NULL)
                {
                    secStatus = gSecLibFunctions.pfSecLib_ExportA2BBlob(aTempKeyData, gSecPlainText_c, gAppOutKeys.aIrk);
                }
            }
        }
#endif /* (defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)) */
    }

#if (defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U))
    if (gSecSuccess_c == secStatus)
    {
        if (mBondAddedFromShell == TRUE)
        {
            shell_cmd_finished();
            mBondAddedFromShell = FALSE;
            gPrivacyStateChangedByUser = TRUE;
            (void)BleConnManager_DisablePrivacy();
        }
        else
        {
            uint8_t buf[gHandoverSetBdCommandLen_c] = {0};
            buf[0] = pBondEventData->nvmIndex;
            buf[1] = gAppOutKeys.addressType;
            FLib_MemCpy(&buf[2], gAppOutKeys.aAddress, gcBleDeviceAddressSize_c);
            FLib_MemCpy(&buf[8], gAppOutKeys.aLtk, gcSmpMaxBlobSize_c);
            FLib_MemCpy(&buf[48], gAppOutKeys.aIrk, gcSmpMaxIrkBlobSize_c);
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
            (void)AppHandover_GetPeerSkd(pBondEventData->nvmIndex, &buf[88]);
#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1) */
            A2A_SendSetBondingDataCommand(buf, gHandoverSetBdCommandLen_c);
        }
    }
    else
    {
        shell_write("\r\nE2E BondingData load failed ");
    }
#endif /* (defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)) */
}
#else
/*! *********************************************************************************
* \brief        Handles GenericCallback events.
*
* \param[in]    pEventData    pointer to appEventData_t.
********************************************************************************** */
static void App_HandleGenericCallbackBondCreatedEvent(appEventData_t *pEventData)
{
    bleBondCreatedEvent_t *pBondEventData = (bleBondCreatedEvent_t *)pEventData->eventData.pData;
    bleResult_t status;

    status = Gap_LoadKeys(pBondEventData->nvmIndex,
                          &gAppOutKeys, &gAppOutKeyFlags, &gAppOutLeSc,
                          &gAppOutAuth);

    if ( status == gBleSuccess_c)
    {
        /* address type, address, ltk, irk */
        shell_write("\r\nBondingData: ");
        shell_writeHex((uint8_t*)&gAppOutKeys.addressType, 1);
        shell_write(" ");
        shell_writeHex(gAppOutKeys.aAddress, 6);
        shell_write(" ");
        shell_writeHex((uint8_t*)gAppOutKeys.aLtk, 16);
        shell_write(" ");
        shell_writeHex((uint8_t*)gAppOutKeys.aIrk, 16);
        shell_write("\r\n");
    }

    if (mBondAddedFromShell == TRUE)
    {
        shell_cmd_finished();
        mBondAddedFromShell = FALSE;
        gPrivacyStateChangedByUser = TRUE;
        (void)BleConnManager_DisablePrivacy();
    }
#if defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1)
    else
    {
        uint8_t buf[gHandoverSetBdCommandLen_c] = {0};
        buf[0] = pBondEventData->nvmIndex;
        buf[1] = gAppOutKeys.addressType;
        FLib_MemCpy(&buf[2], gAppOutKeys.aAddress, gcBleDeviceAddressSize_c);
        FLib_MemCpy(&buf[8], gAppOutKeys.aLtk, gcSmpMaxLtkSize_c);
        FLib_MemCpy(&buf[24], gAppOutKeys.aIrk, gcSmpIrkSize_c);
        A2A_SendSetBondingDataCommand(buf, gHandoverSetBdCommandLen_c);
    }
#endif /* defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1) */
}
#endif /* (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U)) */

#if defined(gAppSecureMode_d) && (gAppSecureMode_d == 0U)
/*! *********************************************************************************
* \brief    Set Bonding Data on the BLE application.
*
********************************************************************************** */
static bleResult_t SetBondingData(uint8_t nvmIndex, bleAddressType_t addressType,
                                  uint8_t* ltk, uint8_t* irk, uint8_t* address)
{
    bleResult_t status;

    gAppOutKeys.addressType = addressType;
    FLib_MemCpy(gAppOutKeys.aAddress, address, gcBleDeviceAddressSize_c);
    FLib_MemCpy(gAppOutKeys.aLtk, ltk, gcSmpMaxLtkSize_c);
    FLib_MemCpy(gAppOutKeys.aIrk, irk, gcSmpIrkSize_c);

    status = Gap_SaveKeys(nvmIndex, &gAppOutKeys, TRUE, TRUE);

    return status;
}
#endif /* defined(gAppSecureMode_d) && (gAppSecureMode_d == 0U) */

/*! *********************************************************************************
 * \brief        Trigger TimeSync from device - send LE Data Length Changed message
 *
 ********************************************************************************** */
static bleResult_t CCC_TriggerTimeSync(deviceId_t deviceId)
{
    bleResult_t result = gBleSuccess_c;

    if (deviceId < (uint8_t)gAppMaxConnections_c)
    {
        result = Gap_LeSetPhy(FALSE, deviceId, 0, gConnDefaultTxPhySettings_c, gConnDefaultRxPhySettings_c, 0);
    }
    else
    {
        result = gBleInvalidParameter_c;
    }

    return result;
}
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

#if defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1)
/*! *********************************************************************************
* \brief        Processes received Handover commands.
*
********************************************************************************** */
static void A2A_ProcessCommand(void *pMsg)
{
    clientPacketStructured_t* pPacket = (clientPacketStructured_t*)pMsg;

    switch(pPacket->header.opGroup)
    {
        case gHandoverCommandsOpGroup_c:
        {
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
            AppHandover_ProcessA2ACommand(pPacket->header.opCode,
                                        pPacket->header.len,
                                        pPacket->payload);
#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1) */
        }
        break;

        case gA2ACommandsOpGroup_c:
        {
            switch(pPacket->header.opCode)
            {
                case gSetBdCommandOpCode_c:
                {
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
                    uint8_t *pIndex = pPacket->payload;
                    uint8_t nvmIndex = gInvalidNvmIndex_c;
                    secResultType_t secStatus = gSecError_c;
                    bleResult_t status = gBleUnexpectedError_c;

                    nvmIndex = *pIndex;
                    pIndex++;
                    gAppOutKeys.addressType = *pIndex;
                    pIndex++;
                    FLib_MemCpy(gAppOutKeys.aAddress, pIndex, gcBleDeviceAddressSize_c);
                    pIndex = &pIndex[gcBleDeviceAddressSize_c];

                    if (gSecLibFunctions.pfSecLib_ImportA2BBlob != NULL)
                    {
                        secStatus = gSecLibFunctions.pfSecLib_ImportA2BBlob(pIndex, gSecLtkElkeBlob_c, gAppOutKeys.aLtk);
                    }

                    if (gSecSuccess_c == secStatus)
                    {
                        pIndex = &pIndex[gcSmpMaxIrkBlobSize_c];
                        if (gSecLibFunctions.pfSecLib_ImportA2BBlob != NULL)
                        {
                            secStatus = gSecLibFunctions.pfSecLib_ImportA2BBlob(pIndex, gSecPlainText_c, gAppOutKeys.aIrk);
                        }
                    }

                    if (gSecSuccess_c == secStatus)
                    {
                        pIndex = &pIndex[gcSmpMaxIrkBlobSize_c];
                        mBondAddedFromShell = TRUE;

                        status = Gap_SaveKeys(nvmIndex, &gAppOutKeys, TRUE, TRUE);
                    }
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
                    (void)AppHandover_SetPeerSkd(nvmIndex, pIndex);

                    shell_write("\r\nReceived peer SKD: ");
                    shell_writeHex(pIndex, gSkdSize_c);
                    shell_write("\r\n");
#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1) */
                    /* If Handover is not enabled the EdgeLock to EdgeLock key is no longer needed. */
                    if (gBleSuccess_c == status)
                    {
                        (void)A2B_FreeE2EKey();
                    }
                    if ((gSecSuccess_c != secStatus) || (gBleSuccess_c != status))
                    {
                        shell_write("\r\nA2A SetBD fail.\r\n");
                    }
#else
                    uint8_t *pIndex = pPacket->payload;
                    uint8_t nvmIndex = gInvalidNvmIndex_c;

                    nvmIndex = *pIndex;
                    pIndex++;
                    gAppOutKeys.addressType = *pIndex;
                    pIndex++;
                    FLib_MemCpy(gAppOutKeys.aAddress, pIndex, gcBleDeviceAddressSize_c);
                    pIndex = &pIndex[gcBleDeviceAddressSize_c];
                    FLib_MemCpy(gAppOutKeys.aLtk, pIndex, gcSmpMaxLtkSize_c);
                    pIndex = &pIndex[gcSmpMaxLtkSize_c];
                    FLib_MemCpy(gAppOutKeys.aIrk, pIndex, gcSmpIrkSize_c);
                    /* Save to first index in handover demo */
                    mBondAddedFromShell = TRUE;
                    (void)Gap_SaveKeys(nvmIndex, &gAppOutKeys, TRUE, TRUE);
#endif /* defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U) */
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */
                }
                break;

#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
                case gHandoverApplicationDataCommandOpCode_c:
                {
                    if (gLastHandedOverPeerId != gInvalidDeviceId_c)
                    {
                        union {
                            uint8_t                 u8LocState;
                            appLocalization_State_t locState;
                        } temp = {0U}; /* MISRA rule 11.3 */
                        void *pAux;

                        uint8_t *pData = pPacket->payload;
                        csReadRemoteSupportedCapabilitiesCompleteEvent_t *remoteCapabilities;

                        /* Extract default settings. Do not overwrite the csAlgoBuf pointer */
                        FLib_MemCpy(&mRangeSettings[gLastHandedOverPeerId],
                                    pData,
                                    sizeof(appLocalization_rangeCfg_t) - sizeof(void *));
                        pData = &pData[sizeof(appLocalization_rangeCfg_t)];

                        /* Extract application state information */
                        temp.u8LocState = *pData;
                        AppLocalization_SetLocState(gLastHandedOverPeerId, temp.locState);
                        pData = &pData[sizeof(uint8_t)];

                        /* Extract cached remote peer capabilities */
                        remoteCapabilities = MEM_BufferAlloc(sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t));

                        if (remoteCapabilities != NULL)
                        {
                            uint8_t nvmIndex = gInvalidNvmIndex_c;
                            bool_t isBonded = FALSE;

                            FLib_MemCpy(remoteCapabilities,
                                        pData,
                                        sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t));
                            /* Update device id */
                            remoteCapabilities->deviceId = gLastHandedOverPeerId;

                            (void)Gap_CheckIfBonded(gLastHandedOverPeerId, &isBonded, &nvmIndex);
                            if (nvmIndex != gInvalidNvmIndex_c)
                            {
                                maPeerInformation[gLastHandedOverPeerId].nvmIndex = nvmIndex;
                                AppLocalization_SetRemoteCachedSupportedCapabilities(nvmIndex, remoteCapabilities);
                            }
                            (void)MEM_BufferFree(remoteCapabilities);
                        }

                        pData = &pData[sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t)];

                        /* Extract handle information */
                        pAux = (void*)pData;
                        mGlobalRangeSettings.role = *(uint8_t*)pAux;
                        pData = &pData[sizeof(uint8_t)];
                        maPeerInformation[gLastHandedOverPeerId].csSecurityEnabled = (bool_t)*pData;
                        pData = &pData[sizeof(uint8_t)];
                    }
                }
                break;
#endif /* gHandoverIncluded_d */

                default:
                {
                    ; /* No action required */
                }
                break;
            }
        }
        break;

        case gA2BCommandsOpGroup_c:
        {
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
            A2B_ProcessA2ACommand(pPacket->header.opCode,
                                pPacket->header.len,
                                pPacket->payload);
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
        }
        break;

        default:
        {
            ; /* No action required */
        }
        break;
    }

    (void)MEM_BufferFree(pMsg);
    return;
}
#endif /* defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1) */

#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
/*! *********************************************************************************
* \brief        Check if the local IRK is set.
*
********************************************************************************** */
static void A2A_CheckLocalIrk(void)
{
    bleResult_t status = gBleSuccess_c;
    bleLocalKeysBlob_t localIrk = {0};

    status = App_NvmReadLocalIRK(&localIrk);

    if ((gBleSuccess_c == status) && (localIrk.keyGenerated == TRUE))
    {
        gA2ALocalIrkSet = TRUE;
    }
    else
    {
        gA2ALocalIrkSet = FALSE;
    }
}
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
/*! *********************************************************************************
* \brief        Handler function for APP Handover events.
*
********************************************************************************** */
static void BleApp_HandoverEventHandler(appHandoverEvent_t eventType, void *pData)
{
    switch (eventType)
    {
        case mAppHandover_ConnectComplete_c:
        {
            deviceId_t peerDeviceId = *(deviceId_t *)pData;
            LedStopFlashingAllLeds();
            Led1On();
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));

            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_HandoverCompleteConnected_c;
                pEventData->eventData.pData = NULL;
                if (gBleSuccess_c != App_PostCallbackMessage(APP_UserInterfaceEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pEventData);
                }
            }

            /* Save peer device ID */
            maPeerInformation[peerDeviceId].deviceId = peerDeviceId;
            mLastConnectFromHandover = TRUE;
        }
        break;

        case mAppHandover_Disconnected_c:
        {
            deviceId_t peerDeviceId = *(deviceId_t *)pData;
            uint8_t peerId = 0U;

            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));

            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_HandoverCompleteDisconnected_c;
                pEventData->eventData.pData = NULL;
                if (gBleSuccess_c != App_PostCallbackMessage(APP_UserInterfaceEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pEventData);
                }
            }

            /* Reset localization state */
            AppLocalization_ResetPeer(peerDeviceId, TRUE, maPeerInformation[peerDeviceId].nvmIndex);
#if defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
            AppLocalizationAlgo_ResetPeer(peerDeviceId);
#endif /* defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U) */
            gFilterShellVal = (uint16_t)gNoFilter_c;
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
            gHandoverDeviceId = gInvalidDeviceId_c;
#endif

            /* Mark device ID as invalid */
            maPeerInformation[peerDeviceId].deviceId = gInvalidDeviceId_c;
            maPeerInformation[peerDeviceId].csCapabWritten = FALSE;
            maPeerInformation[peerDeviceId].csSecurityEnabled = FALSE;
            mLastConnectFromHandover = FALSE;
            /* UI */

            /* Check to see if there are other devices connected. */
            for (peerId = 0; peerId < (uint8_t)gAppMaxConnections_c; peerId++)
            {
                if (maPeerInformation[peerId].deviceId != gInvalidDeviceId_c)
                {
                    break;
                }
            }

            /* Update the UI if no other devices are connected. */
            if (peerId == (uint8_t)gAppMaxConnections_c)
            {
                LedStartFlashingAllLeds();
            }
        }
        break;

        case mAppHandover_TimeSyncStarted_c:
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));

            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_HandoverStarted_c;
                pEventData->eventData.handoverTimeSync = *(bool_t *)pData;
                if (gBleSuccess_c != App_PostCallbackMessage(APP_UserInterfaceEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pEventData);
                }
            }
        }
        break;

        case mAppHandoverAnchorMonitor_c:
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));

            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_AnchorMonitorEventReceived_c;
                FLib_MemCpy(&pEventData->eventData.anchorMonitorEvent, pData, sizeof(appHandoverAnchorMonitorEvent_t));
                if (gBleSuccess_c != App_PostCallbackMessage(APP_UserInterfaceEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pEventData);
                }
            }
        }
        break;

        case mAppHandoverPacketMonitor_c:
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
            appHandoverAnchorMonitorPacketEvent_t *pAppPacketMonitorEvent = pData;

            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_PacketMonitorEventReceived_c;
                FLib_MemCpy(&pEventData->eventData.anchorMonitorEvent, pData, sizeof(appHandoverAnchorMonitorPacketEvent_t));
                if (gBleSuccess_c != App_PostCallbackMessage(APP_UserInterfaceEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pAppPacketMonitorEvent->pktMntEvt.pPdu);
                    (void)MEM_BufferFree(pEventData);
                }
            }
            else
            {
                (void)MEM_BufferFree(pAppPacketMonitorEvent->pktMntEvt.pPdu);
            }
        }
        break;

        case mAppHandoverPacketContinueMonitor_c:
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
            appHandoverAnchorMonitorPacketContinueEvent_t *pAppPacketContinueMonitorEvent = pData;

            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_PacketMonitorContinueEventReceived_c;
                FLib_MemCpy(&pEventData->eventData.anchorPacketContinueEvent, pData, sizeof(appHandoverAnchorMonitorPacketContinueEvent_t));
                if (gBleSuccess_c != App_PostCallbackMessage(APP_UserInterfaceEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pAppPacketContinueMonitorEvent->pktMntCntEvt.pPdu);
                    (void)MEM_BufferFree(pEventData);
                }
            }
            else
            {
                (void)MEM_BufferFree(pAppPacketContinueMonitorEvent->pktMntCntEvt.pPdu);
            }
        }
        break;

        case mAppHandover_Error_c:
        {
            appHandoverError_t error = *(appHandoverError_t *)pData;
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));

            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_HandoverError_c;
                pEventData->eventData.handoverError = error;
                if (gBleSuccess_c != App_PostCallbackMessage(APP_UserInterfaceEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pEventData);
                }
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
#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1) */

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

        case gCsStatusEvent_c:
        {
        }
        break;

        case gCsSecurityEnabled_c:
        {
            maPeerInformation[deviceId].csSecurityEnabled = TRUE;
            if (mVerbosityLevel == 2U)
            {
                shell_write("\r\nCS security enabled.\r\n");
            }

            if ((mGlobalRangeSettings.role == gCsRoleInitiator_c) &&
                (maPeerInformation[deviceId].csCapabWritten == TRUE))
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
            maPeerInformation[deviceId].csCapabWritten = TRUE;

            if (mVerbosityLevel == 2U)
            {
                shell_write("\r\nLocalization config complete.\r\n");
            }

            if(maPeerInformation[deviceId].gapRole == gGapCentral_c)
            {
                result = AppLocalization_SecurityEnable(deviceId);

                if (result != gBleSuccess_c)
                {
                    shell_write("\r\nCS Security Enable failed.\r\n");
                }
            }

            if ((mGlobalRangeSettings.role != gCsRoleInitiator_c) &&
                (maPeerInformation[deviceId].csSecurityEnabled == TRUE))
            {
                result = AppLocalization_SetProcedureParameters(deviceId);

                if (result != gBleSuccess_c)
                {
                    shell_write("\r\nSet Procedure parameters failed.\r\n");
                }
            }
        }
        break;

        case gLocalConfigWritten_c:
        {
            maPeerInformation[deviceId].csCapabWritten = TRUE;
            if (mVerbosityLevel == 2U)
            {
                shell_write("\r\nLocalization config complete.\r\n");
            }

#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
            if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
            {
                if ((mLastConnectFromHandover == TRUE) ||
                    (maPeerInformation[deviceId].csSecurityEnabled == TRUE))
                {
                    bleResult_t result = AppLocalization_SetProcedureParameters(deviceId);

                    if (result != gBleSuccess_c)
                    {
                        shell_write("\r\nSet Procedure parameters failed.\r\n");
                    }
                }
            }
#endif
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

            if ((maPeerInformation[deviceId].csCapabWritten == TRUE) &&
                (maPeerInformation[deviceId].csSecurityEnabled == TRUE))
            {
                result = AppLocalization_StartMeasurement(deviceId);

                if (result != gBleSuccess_c)
                {
                    if (mVerbosityLevel == 2U)
                    {
                        shell_write("\r\nDistance measurement start failed.\r\n");
                    }
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
                shell_write("\r\n[");
                shell_writeDec(deviceId);
                shell_write("] Distance measurement complete. Local data available for procedure index ");
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
                    shell_write("\r\nAlgorithm did not run.\r\n");
                }
                break;

                case gAppLclAlgoNotRunNoDataReady_c:
                {
                    shell_write("\r\nAlgorithm did not run, procedure likely failed on peer - no Data Ready received.\r\n");
                }
                break;

                case gAppLclAlgoNotRunNoRangingData_c:
                {
                    shell_write("\r\nAlgorithm did not run - did not receive complete Ranging Data from peer.\r\n");
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

                case gAppLclAlgoNotRunNoRealTimeData_c:
                {
                    shell_write("Algorithm did not run - Real Time Ranging Data not complete!\r\n");
                }
                break;

                default:
                {
                    shell_write("Unknown error!\r\n");
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

            /* MISRA Rule 10.3 - The value of an expression shall not be assigned to an object with a narrower essential type or of a different essential type category */
            union
            {
                uint8_t u8;
                uint32_t u32;
            }temp = {0};

            temp.u32 = ~0x0FU;
            abortReason &= temp.u8;

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

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
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
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 1)
        case gCsRemoteDataLogEvent_c:
        {
            uint16_t dataLen = BtcsClient_GetPeerRangingDataSize(deviceId);

            /* Construct full CS HCI data packet */
            uint8_t * pCsRemoteDataPacket = MEM_BufferAlloc(dataLen + sizeof(uint16_t));

            if (pCsRemoteDataPacket != NULL)
            {
                /* Pack data length */
                Utils_PackTwoByteValue(dataLen, &pCsRemoteDataPacket[0]);
                /* Pack data */
                FLib_MemCpy(&pCsRemoteDataPacket[2], pData, dataLen);

                /* Post callback for serial operation to prevent the addition of delays during the procedure */
                if (gBleSuccess_c != App_PostCallbackMessage(App_ExportRemoteDataLog, (void *)pCsRemoteDataPacket))
                {
                    (void)MEM_BufferFree(pCsRemoteDataPacket);
                }
            }
        }
        break;
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */

        default:
        {
            ; /* No action required */
        }
        break;
    }
}

#if defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
/*! *********************************************************************************
* \brief  This is the callback for displaying distance measurement results
********************************************************************************** */
static void BleApp_PrintMeasurementResults(deviceId_t deviceId, localizationAlgoResult_t *pResult)
{
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
    uint16_t procCount = AppLocalization_GetProcedureCount(deviceId);
    uint16_t qInt =0U;
    uint16_t qFrac = 0U;

    if ((mVerbosityLevel != 0U) || (procCount == mRangeSettings[deviceId].maxNumProcedures))
    {
#if defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)
        if ((pResult->algorithm & eMciqAlgoEmbedRADE) != 0U)
        {
            shell_write("\r\n[");
            shell_writeDec((uint8_t)deviceId);

            if (pResult->radeError != 0U)
            {
                shell_write("] RADE Error: ");
                shell_writeDec(pResult->radeError);
                shell_write("!\r\n");
            }
            else if (pResult->resultRADE.dqiIntegerPart == 0U)
            {
                shell_write("] Low Quality data for RADE! Quality indicator is 0! \n\r");
            }
            else if (pResult->resultRADE.distanceIntegerPart > gMaxDistanceMeters_c)
            {
                shell_write("] Outlier RADE distance result - discarded. Check gMaxDistanceMeters_c value. \n\r");
            }
            else
            {
                shell_write("] Distance (RADE): ");
                /* Display the integer part of the distance in meters. */
                shell_writeDec(pResult->resultRADE.distanceIntegerPart);
                shell_write(".");

                /* Leading zeroes for decimal part */
                for (uint8_t i = 0U; i < pResult->resultRADE.leadingZeroesDecimalPart; i++)
                {
                    shell_write("0");
                }

                /* Display the decimal part of the distance in meters. */
                shell_writeDec(pResult->resultRADE.distanceDecimalPart);
                shell_write(" m   ");

                shell_write("Quality: ");
                shell_writeDec(pResult->resultRADE.dqiIntegerPart);
                shell_write(".");
                shell_writeDec(pResult->resultRADE.dqiDecimalPart);
                shell_write("%%\r\n");
            }
        }
#endif /* gAppUseRADEAlgorithm_d */

#if defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)
        if ((pResult->algorithm & eMciqAlgoEmbedCDE) != 0U)
        {
            shell_write("\r\n[");
            shell_writeDec((uint8_t)deviceId);
            shell_write("] Distance (CDE): ");
            /* Display the integer part of the distance in meters. */
            shell_writeDec(pResult->resultCDE.distanceIntegerPart);
            shell_write(".");

            /* Leading zeroes for decimal part */
            for (uint8_t i = 0U; i < pResult->resultCDE.leadingZeroesDecimalPart; i++)
            {
                shell_write("0");
            }

            /* Display the decimal part of the distance in meters. */
            shell_writeDec(pResult->resultCDE.distanceDecimalPart);
            shell_write(" m   ");

            shell_write("Quality: ");
            shell_writeDec(pResult->resultCDE.dqiIntegerPart);
            shell_write(".");
            shell_writeDec(pResult->resultCDE.dqiDecimalPart);
            shell_write("%%\r\n");
        }
#endif /* gAppUseCDEAlgorithm_d */

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
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
            shell_write("L2CAP transfer: ");
            shell_writeDec(pResult->transferDuration/1000);
            shell_write("ms\r\n");
        }
        if (pResult->algoDuration != 0)
        {
            shell_write("Localization algorithm: ");
            shell_writeDec(pResult->algoDuration/1000);
            shell_write("ms\r\n");
        }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

        /* Print RTT information */
        shell_write("\r\n[");
        shell_writeDec((uint8_t)deviceId);
        shell_write("] RTT Distance: ");

        /* Set negative distance to zero */
        if (pResult->rttResult.dm_ad <= 0)
        {
            qInt = 0U;
            qFrac = 0U;
        }
        else
        {
            /* Round to nearest integer */
            pResult->rttResult.dm_ad += 3277;
            qInt = (uint16_t)((uint32_t)pResult->rttResult.dm_ad >> 16U);
            qFrac = (uint16_t)((((uint32_t)pResult->rttResult.dm_ad & 0x0000FFFFU)*10U) >> 16U);
        }

        /* Print Results */
        shell_writeDec(qInt);
        shell_write(".");
        shell_writeDec(qFrac);
        shell_write(" m Success Rate: ");
        shell_writeDec(pResult->rttResult.dm_sr);
        shell_write("\r\n");

/* Print average RSSI values */
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1)
        shell_write("RSSI information:");
        shell_write("\r\n");
        if (pResult->rssiInfo.rssiLocalNo != 0U)
        {
            int8_t rssiLocalAverage = 0;
            shell_write("Local Average: ");
            for (uint8_t idx = 0U; idx < pResult->rssiInfo.rssiLocalNo; idx++)
            {
                rssiLocalAverage += (pResult->rssiInfo.aRssiLocal[idx]/pResult->rssiInfo.rssiLocalNo);
            }
            if (((uint8_t)rssiLocalAverage >> 7U) != 0U)
            {
                shell_write("-");
                rssiLocalAverage = ~((uint8_t)rssiLocalAverage - 1U);
            }
            shell_writeDec(rssiLocalAverage);
            shell_write("    ");
        }

        if (pResult->rssiInfo.rssiRemoteNo != 0U)
        {
            int8_t rssiRemoteAverage = 0;
            shell_write("Remote Average: ");
            for (uint8_t idx = 0U; idx < pResult->rssiInfo.rssiRemoteNo; idx++)
            {
                rssiRemoteAverage += (pResult->rssiInfo.aRssiRemote[idx]/pResult->rssiInfo.rssiRemoteNo);
            }
            if (((uint8_t)rssiRemoteAverage >> 7U) != 0U)
            {
                shell_write("-");
                rssiRemoteAverage = ~((uint8_t)rssiRemoteAverage - 1U);
            }
            shell_writeDec(rssiRemoteAverage);
            shell_write("\r\n");
        }
#endif /* defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1) */
    }

    if (procCount == mRangeSettings[deviceId].maxNumProcedures)
    {
        shell_cmd_finished();
    }

#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
    if (gHandoverDeviceId != gInvalidDeviceId_c)
    {
        bleResult_t status = gBleSuccess_c;
        appLocalization_State_t locState = AppLocalization_GetLocState(gHandoverDeviceId);

        if (locState == gAppLclIdle_c)
        {
            AppHandover_SetPeerDevice(gHandoverDeviceId);
            status = AppHandover_StartTimeSync(TRUE);
            shell_write("\r\nHandover started.\r\n");
        }

        if (status != gBleSuccess_c)
        {
            shell_write("\r\nHandover time synchronization error.\r\n");
            shell_cmd_finished();
        }
    }
#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1) */
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */
}
#endif /* defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U) */

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
/*! *********************************************************************************
* \brief        Calls BleApp_OP_Start on application task
*
********************************************************************************** */
static void BleApp_OP_StartCaller(appCallbackParam_t param)
{
    (void)param;
    BleApp_OP_Start();
}
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0)) */

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1))
/*! *********************************************************************************
* \brief        Calls BleApp_PE_Start on application task
*
********************************************************************************** */
static void BleApp_PE_StartCaller(appCallbackParam_t param)
{
    (void)param;
    BleApp_PE_Start();
}
#endif /* (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 1)) */

#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
/*! *********************************************************************************
* \brief        Handler function for APP Handover communication interface.
*
********************************************************************************** */
static void BleApp_HandoverCommHandler(uint8_t opGroup, uint8_t cmdId, uint16_t len, uint8_t *pData)
{
    A2A_SendCommand(opGroup, cmdId, pData, len);
}
#endif

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
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
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 1)
/*! *********************************************************************************
* \brief        Handler function for exporting remote transfer data via serial
*
********************************************************************************** */
static void App_ExportRemoteDataLog(void *pData)
{
    uint8_t *pCsRemotePacket = (uint8_t*)pData;

    /* Get data length for the peer */
    uint16_t dataLen = Utils_ExtractTwoByteValue(pCsRemotePacket);

    /* Serial write full packet */
    (void)SerialManager_WriteBlocking(gDataExportSerialWriteHandle, &pCsRemotePacket[2], dataLen);

    (void)MEM_BufferFree(pCsRemotePacket);
}
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 1) */
/*! *********************************************************************************
* @}
********************************************************************************** */
