/*! *********************************************************************************
* \addtogroup Digital Key Device CS Application
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file app_digital_key_device_cs.c
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
#include "RNG_Interface.h"
#include "fsl_adapter_reset.h"
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
#include "fsl_shell.h"
#endif
#include "app.h"

/* BLE Host Stack */
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gap_interface.h"
#include "gatt_db_app_interface.h"
#include "l2ca_cb_interface.h"
#include "gatt_db_handles.h"

/* Profile / Services */
#include "digital_key_interface.h"

#include "ble_conn_manager.h"
#include "ble_service_discovery.h"

#include "digital_key_device_cs.h"
#include "app_digital_key_device_cs.h"
#include "shell_digital_key_device_cs.h"

#include "app_localization.h"
#include "app_localization_algo.h"
#include "channel_sounding.h"
#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
#include "btcs_server_interface.h"
#elif defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
#include "btcs_client_interface.h"
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
appPeerInfo_t maPeerInformation[gAppMaxConnections_c];
gattCharacteristic_t maCharacteristics[mcNumCharacteristics_c]; /* Index 0 - Vehicle PSM; Index 1 - Vehicle Antenna ID; Index 2 - Tx Power  */
uint8_t mValVehiclePsm[2];
uint16_t mValVehicleAntennaId;
int8_t mValTxPower;
uint8_t mCurrentCharReadingIndex;

/* Which peer are we doing OOB pairing with? */
deviceId_t mCurrentPeerId = gInvalidDeviceId_c;

uint8_t mVerbosityLevel = 2U; /* default verbosity: allow all prints */

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
#if defined(gAppUseBonding_d) && (gAppUseBonding_d)
static bool_t mRestoringBondedLink = FALSE;
#endif

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
/* LTK */
static uint8_t gaAppSmpLtk[gcSmpMaxLtkSize_c];

/* RAND*/
static uint8_t gaAppSmpRand[gcSmpMaxRandSize_c];

/* IRK */
static uint8_t gaAppSmpIrk[gcSmpIrkSize_c];

/* Address */
static uint8_t gaAppAddress[gcBleDeviceAddressSize_c];
static gapSmpKeys_t gAppOutKeys = {
    .cLtkSize = mcEncryptionKeySize_c,
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
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

static uint8_t maOutCharReadBuffer[mCharReadBufferLength_c];
static uint16_t mOutCharReadByteCount;

/* Own address used during discovery. Included in First Approach Response. */
static uint8_t gaAppOwnDiscAddress[gcBleDeviceAddressSize_c];

/* Time Sync UWB Device Time. Demo value. */
static uint64_t mTsUwbDeviceTime = 0U;
/*
Global used to identify if bond whas added by
shell command or connection with the device
mBondAddedFromShell = FALSE bond created by connection
mBondAddedFromShell = TRUE  bond added by shell command
*/
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
static bool_t mBondAddedFromShell = FALSE;
#endif
/* Get a simulated UWB clock. */
static uint64_t GetUwbClock(void);

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
static void BleApp_Disconnect(void);
static void BleApp_SetBondingData(appEventData_t *pEventData);
static void BleApp_RemoveBondingData(appEventData_t *pEventData);
static void AppPrintLePhyEvent(gapPhyEvent_t* pPhyEvent);
static void PrintLePhyEvent(gapPhyEvent_t* pPhyEvent);
static void BleApp_ListBondingData(void);
static bleResult_t SetBondingData(uint8_t nvmIndex, bleAddressType_t addressType,
                                  uint8_t* ltk, uint8_t* irk, uint8_t* address);
static void BleApp_SetCsConfigParams(appEventData_t* pEventData);
static void BleApp_SetCsProcParams(appEventData_t* pEventData);
static void BleApp_TriggerCsDistanceMeasurement(deviceId_t deviceId);
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

/* CCC Time Sync */
static bleResult_t CCC_SendTimeSync(deviceId_t deviceId,
                                    uint64_t *pDevEvtCnt,
                                    uint64_t *pUwbDevTime,
                                    uint8_t success);

/* CCC SubEvents */
static bleResult_t CCC_SendSubEvent(deviceId_t deviceId,
                                    dkSubEventCategory_t category,
                                    dkSubEventCommandCompleteType_t type);

/* CCC Phase 2 */
static bleResult_t CCCPhase2_SendSPAKEResponse(deviceId_t deviceId, uint8_t *pData, uint16_t dataLen);
static bleResult_t CCCPhase2_SendSPAKEVerify(deviceId_t deviceId, uint8_t *pData, uint16_t dataLen);

static bleResult_t CCC_FirstApproachReq
(
    deviceId_t deviceId,
    uint8_t* pBdAddr,
    gapLeScOobData_t* pOobData
);

static void App_HandleKeys(appEventData_t *pEventData);
static void App_HandleGattClientCallback(appEventData_t *pEventData);
static void App_HandleGenericCallback(appEventData_t *pEventData);
static void App_HandleConnectionCallback(appEventData_t *pEventData);
static void App_HandleL2capPsmDataCallback(appEventData_t *pEventData);
static void App_HandleL2capPsmControlCallback(appEventData_t *pEventData);

static void BleApp_HandleIdleState(deviceId_t peerDeviceId, appEvent_t event);
static void BleApp_HandleServiceDiscState(deviceId_t peerDeviceId, appEvent_t event);
static void BleApp_HandlePairState(deviceId_t peerDeviceId, appEvent_t event);
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief        State machine handler of the Digital Key Device application.
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
            BleApp_HandleIdleState(peerDeviceId, event);
        }
        break;

        case mAppExchangeMtu_c:
        {
            if (event == mAppEvt_GattProcComplete_c)
            {
                bleUuid_t dkServiceUuid;

                /* Moving to Service Discovery State */
                maPeerInformation[peerDeviceId].appState = mAppServiceDisc_c;

                /* Start Service Discovery */
                dkServiceUuid.uuid16 = gBleSig_CCC_DK_UUID_d;
                (void)BleServDisc_FindService(peerDeviceId, gBleUuidType16_c, &dkServiceUuid);
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
            BleApp_HandleServiceDiscState(peerDeviceId, event);
        }
        break;

        case mAppCCCPhase2WaitingForRequest_c:
        {
            if (event == mAppEvt_SentSPAKEResponse_c)
            {
               maPeerInformation[peerDeviceId].appState = mAppCCCPhase2WaitingForVerify_c;
            }
        }
        break;

        case mAppCCCPhase2WaitingForVerify_c:
        {
            if (event == mAppEvt_ReceivedSPAKEVerify_c)
            {
                maPeerInformation[peerDeviceId].appState = mAppCCCWaitingForPairingReady_c;
            }
        }
        break;

        case mAppCCCWaitingForPairingReady_c:
        {
            if (event == mAppEvt_ReceivedPairingReady_c)
            {
                mCurrentPeerId = peerDeviceId;
                (void)Gap_LeScGetLocalOobData();
                maPeerInformation[peerDeviceId].appState = mAppPair_c;
            }
        }
        break;

        case mAppPair_c:
        {
            BleApp_HandlePairState(peerDeviceId, event);
        }
        break;

        case mAppLocalizationSetup_c:
        {
#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
            if ((event == mAppEvt_GattServerCallback_CCCDWrittenComplete_c) ||
                (event == mAppEvt_BtcsRangingProcResCfg_c))
#elif defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
            if (event == mAppEvt_GattProcComplete_c)
            {
                /* Send BTCS Ranging Procedure Results Config message */
                bool_t bEnableProcResTransfer = TRUE;
                bleResult_t result = gBleSuccess_c;

                /* Receiver should send procedure results */
                result = SendBTCSRangingProcResCfg(peerDeviceId, bEnableProcResTransfer);

                if (result != gBleSuccess_c)
                {
                    shell_write("BTCS Ranging Procedure Results Config failed!\r\n");
                }
                else
#endif
            {
                /* Start CS localization */
                if (mVerbosityLevel == 2U)
                {
                    shell_write("Starting localization Config !\r\n");
                }
                maPeerInformation[peerDeviceId].appState = mAppLocalization_c;

                if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
                {
                    bleResult_t status = gBleSuccess_c;
                    status = AppLocalization_Config(peerDeviceId);

                    if (status != gBleSuccess_c)
                    {
                        shell_write("Localization configuration failed !\r\n");
                    }
                }
            }
#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
            }
#endif /* defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U) */
        }
        break;

        case mAppRunning_c:
        case mAppLocalization_c:
        {
            if (event == mAppEvt_PeerDisconnected_c)
            {
                AppLocalization_ResetPeer(peerDeviceId, TRUE, maPeerInformation[peerDeviceId].nvmIndex);
                shell_cmd_finished();
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
        maPeerInformation[peerDeviceId].customInfo.hPsmChannelChar = gGattDbInvalidHandle_d;
    }
}

/*!*************************************************************************************************
 \fn     uint8_t APP_BleEventHandler(void *pData)
 \brief  This function is used to handle events from BLE

 \param  [in]   pData - pointer to data;
 ***************************************************************************************************/
void APP_BleEventHandler(void *pData)
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
            (void)BleApp_FactoryReset();
        }
        break;

        case mAppEvt_Shell_ShellStartDiscovery_Command_c:
        {
            BleApp_Start();
        }
        break;

        case mAppEvt_Shell_StopDiscovery_Command_c:
        {
            (void)Gap_StopScanning();
        }
        break;

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
        case mAppEvt_Shell_SetBondingData_Command_c:
        {
            BleApp_SetBondingData(pEventData);
        }
        break;

        case mAppEvt_Shell_ListBondedDev_Command_c:
        {
            BleApp_ListBondingData();
        }
        break;

        case mAppEvt_Shell_RemoveBondedDev_Command_c:
        {
            BleApp_RemoveBondingData(pEventData);
        }
        break;

        case mAppEvt_Shell_Disconnect_Command_c:
        {
            BleApp_Disconnect();
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

        case mAppEvt_Shell_SetVerbosityLevel_Command_c:
        {
            /* Set verbosity level for CS procedures */
            mVerbosityLevel = pEventData->eventData.verbosityLevel;
        }
        break;

        case mAppEvt_Shell_SetNumProcs_Command_c:
        {
            AppLocalization_SetNumberOfProcedures(pEventData->peerDeviceId, pEventData->eventData.numProcedures);
        }
        break;
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

        case mAppEvt_KBD_EventPressPB1_c:
        case mAppEvt_KBD_EventLongPB1_c:
        case mAppEvt_KBD_EventVeryLongPB1_c:
        {
            App_HandleKeys(pEventData);
            break;
        }

        case mAppEvt_GattClientCallback_GattProcError_c:
        case mAppEvt_GattClientCallback_GattProcReadCharacteristicValue_c:
        case mAppEvt_GattClientCallback_GattProcReadUsingCharacteristicUuid_c:
        case mAppEvt_GattClientCallback_GattProcComplete_c:
        {
            App_HandleGattClientCallback(pEventData);
            break;
        }

        case mAppEvt_GattServerCallback_CCCDWrittenComplete_c:
        {
            BleApp_StateMachineHandler(pEventData->peerDeviceId, mAppEvt_GattServerCallback_CCCDWrittenComplete_c);
            break;
        }

        case mAppEvt_ServiceDiscoveryCallback_DiscoveryFinishedWithSuccess_c:
        {
            BleApp_StateMachineHandler(pEventData->peerDeviceId, mAppEvt_ServiceDiscoveryComplete_c);
            break;
        }

        case mAppEvt_ServiceDiscoveryCallback_DiscoveryFinishedFailed_c:
        {
            BleApp_StateMachineHandler(pEventData->peerDeviceId, mAppEvt_ServiceDiscoveryFailed_c);
            break;
        }

        case mAppEvt_GenericCallback_PeerDisconnected_c:
        {
            /* Factory reset may trigger internal error in some scenarios. */
            BleApp_StateMachineHandler( mCurrentPeerId, mAppEvt_PeerDisconnected_c );
            break;
        }

        case mAppEvt_GenericCallback_LePhyEvent_c:
        case mAppEvt_GenericCallback_LeScLocalOobData_c:
        case mAppEvt_GenericCallback_RandomAddressReady_c:
        case mAppEvt_GenericCallback_CtrlNotifEvent_c:
        case mAppEvt_GenericCallback_BondCreatedEvent_c:
        {
            App_HandleGenericCallback(pEventData);
            break;
        }

        case mAppEvt_ConnectionCallback_ConnEvtConnected_c:
        case mAppEvt_ConnectionCallback_ConnEvtDisconnected_c:
        case mAppEvt_ConnectionCallback_ConnEvtLeScOobDataRequest_c:
        case mAppEvt_ConnectionCallback_ConnEvtPairingComplete_c:
        case mAppEvt_ConnectionCallback_ConnEvtEncryptionChanged_c:
        case mAppEvt_ConnectionCallback_ConnEvtAuthenticationRejected_c:
        {
            App_HandleConnectionCallback(pEventData);
            break;
        }

        case mAppEvt_L2capPsmDataCallback_c:
        {
            App_HandleL2capPsmDataCallback(pEventData);
            break;
        }

        case mAppEvt_L2capPsmControlCallback_LePsmConnectionComplete_c:
        case mAppEvt_L2capPsmControlCallback_LePsmDisconnectNotification_c:
        case mAppEvt_L2capPsmControlCallback_NoPeerCredits_c:
#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
        case mAppEvt_L2capPsmChannelStatusNotification_c:
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */
        {
            App_HandleL2capPsmControlCallback(pEventData);
            break;
        }

        default:
        {
            ; /* No action required */
        }
        break;
    }

    (void)MEM_BufferFree(pData);
    pData = NULL;

}

/*! *********************************************************************************
* \brief  This is the callback for displaying distance measurement results
********************************************************************************** */
void BleApp_PrintMeasurementResults(deviceId_t deviceId, localizationAlgoResult_t *pResult)
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
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief        Handler of the mAppIdle_c state for BleApp_StateMachineHandler.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
************************************************************************************/
static void BleApp_HandleIdleState(deviceId_t peerDeviceId, appEvent_t event)
{
    if (event == mAppEvt_PeerConnected_c)
    {
        if (maPeerInformation[peerDeviceId].customInfo.hPsmChannelChar == gGattDbInvalidHandle_d)
        {
            /* Moving to Exchange MTU State */
            maPeerInformation[peerDeviceId].appState = mAppExchangeMtu_c;
            (void)GattClient_ExchangeMtu(peerDeviceId, gAttMaxMtu_c);
        }
    }
    else if ( event == mAppEvt_EncryptionChanged_c )
    {
        bleUuid_t psmCharUuid;
        gattHandleRange_t handleRange;

        /* Moving to Service Discovery State*/
        maPeerInformation[peerDeviceId].appState = mAppServiceDisc_c;

        /* Read SPSM from vehicle. */
        FLib_MemCpy(psmCharUuid.uuid128, uuid_char_vehicle_psm_vdbt_version, gcBleLongUuidSize_c);
        handleRange.startHandle = 0x0001U;
        handleRange.endHandle = 0xFFFFU;
        mCurrentCharReadingIndex = mcCharVehiclePsmDkVersionIndex_c;
        (void)GattClient_ReadUsingCharacteristicUuid(peerDeviceId,
                                                    gBleUuidType128_c,
                                                    &psmCharUuid,
                                                    &handleRange,
                                                    maOutCharReadBuffer,
                                                    mCharReadBufferLength_c,
                                                    &mOutCharReadByteCount);
    }
    else if ( event == mAppEvt_AuthenticationRejected_c )
    {
        /* Something went wrong - peer has likely lost the bond.
           Must pair again, move to Exchange MTU */
        maPeerInformation[peerDeviceId].appState = mAppExchangeMtu_c;
        maPeerInformation[peerDeviceId].isBonded = FALSE;
        (void)GattClient_ExchangeMtu(peerDeviceId, gAttMaxMtu_c);
    }
    else
    {
        /* For MISRA compliance */
    }
}

/*! *********************************************************************************
* \brief        Handler of the mAppServiceDisc_c state for BleApp_StateMachineHandler.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
************************************************************************************/
static void BleApp_HandleServiceDiscState(deviceId_t peerDeviceId, appEvent_t event)
{
    bleResult_t status = gBleUnexpectedError_c;

    if (event == mAppEvt_ReadCharacteristicValueComplete_c)
    {
        (void)L2ca_ConnectLePsm((uint16_t)Utils_BeExtractTwoByteValue(maCharacteristics[mcCharVehiclePsmIndex_c].value.paValue),
                                peerDeviceId, mAppLeCbInitialCredits_c);
    }

    if (event == mAppEvt_PsmChannelCreated_c)
    {
        if (maPeerInformation[peerDeviceId].isBonded)
        {
            uint64_t devEvtCnt = 0U;
            /* Send Time Sync */
            (void)CCC_SendTimeSync(peerDeviceId, &devEvtCnt, &mTsUwbDeviceTime, 1U);

            shell_cmd_finished();

            /* Localizaion setup - Exchange MTU */
            shell_write("MTU Exchange\r\n");
            maPeerInformation[peerDeviceId].appState = mAppLocalizationSetup_c;
            (void)GattClient_ExchangeMtu(peerDeviceId, gAttMaxMtu_c);
        }
        else
        {
            /* Send request_owner_pairing */
            shell_write("\r\nSending Command Complete SubEvent: Request_owner_pairing\r\n");
            status =  CCC_SendSubEvent(peerDeviceId, gCommandComplete_c, gRequestOwnerPairing_c);
            if (status == gBleSuccess_c)
            {
                maPeerInformation[peerDeviceId].appState = mAppCCCPhase2WaitingForRequest_c;
            }
        }
    }
}

/*! *********************************************************************************
* \brief        Handler of the mAppPair state for BleApp_StateMachineHandler.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    event               Event type.
************************************************************************************/
static void BleApp_HandlePairState(deviceId_t peerDeviceId, appEvent_t event)
{
    if ( event == mAppEvt_PairingLocalOobData_c )
    {
        shell_write("\r\nSending First_Approach_RQ\r\n");
        (void)CCC_FirstApproachReq(peerDeviceId, gaAppOwnDiscAddress, &maPeerInformation[peerDeviceId].oobData);
    }
    else if ( event == mAppEvt_PairingPeerOobDataRcv_c )
    {
        shell_write("\r\nReceived First_Approach_RS.\r\n");
        shell_write("\r\nPairing...\r\n");
        (void)Gap_Pair(peerDeviceId, &gPairingParameters);
    }
    else if ( event == mAppEvt_PairingPeerOobDataReq_c )
    {
        (void)Gap_LeScSetPeerOobData(peerDeviceId, &maPeerInformation[peerDeviceId].peerOobData);
    }
    else if ( event == mAppEvt_PairingComplete_c )
    {
        uint64_t devEvtCnt = 0U;
        FLib_MemSet(&maPeerInformation[peerDeviceId].oobData, 0x00, sizeof(gapLeScOobData_t));
        FLib_MemSet(&maPeerInformation[peerDeviceId].peerOobData, 0x00, sizeof(gapLeScOobData_t));
        maPeerInformation[peerDeviceId].appState = mAppRunning_c;
        shell_write("\r\nPairing successful.\r\n");
        shell_cmd_finished();
        /* Send Time Sync */
        (void)CCC_SendTimeSync(peerDeviceId, &devEvtCnt, &mTsUwbDeviceTime, 1U);
#if defined(gAppUseBonding_d) && (gAppUseBonding_d)
        /* Write data in NVM */
        (void)Gap_SaveCustomPeerInformation(peerDeviceId,
                                            (void *)&maPeerInformation[peerDeviceId].customInfo, 0,
                                            (uint16_t)sizeof(appCustomInfo_t));
#endif
    }
    else
    {
        /* For MISRA compliance */
    }
}

/*! *********************************************************************************
* \brief        Handles L2capPsmControl events.
*
* \param[in]    pEventData    pointer to appEventData_t.
********************************************************************************** */
static void App_HandleL2capPsmControlCallback(appEventData_t *pEventData)
{
    switch(pEventData->appEvent)
    {
        case mAppEvt_L2capPsmControlCallback_LePsmConnectionComplete_c:
        {
            l2caLeCbConnectionComplete_t *pConnComplete = pEventData->eventData.pData;

            if (pConnComplete->result == gSuccessful_c)
            {
                /* Handle Conn Complete */
                shell_write("\r\nL2CAP PSM Connection Complete.\r\n");

                maPeerInformation[pConnComplete->deviceId].customInfo.psmChannelId = pConnComplete->cId;
#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
                AppLocalization_SetPsmChannelId(pConnComplete->deviceId, pConnComplete->cId);
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */
                /* Move to Time Sync */
                BleApp_StateMachineHandler(maPeerInformation[pConnComplete->deviceId].deviceId, mAppEvt_PsmChannelCreated_c);
            }
            break;
        }

        case mAppEvt_L2capPsmControlCallback_LePsmDisconnectNotification_c:
        {
            shell_write("\r\nL2CAP PSM disconnected. Reconnecting...\r\n");
            (void)L2ca_ConnectLePsm((uint16_t)Utils_BeExtractTwoByteValue(maCharacteristics[mcCharVehiclePsmIndex_c].value.paValue),
                                        pEventData->peerDeviceId, mAppLeCbInitialCredits_c);
            break;
        }

        case mAppEvt_L2capPsmControlCallback_NoPeerCredits_c:
        {
            l2caLeCbNoPeerCredits_t *pCbNoPeerCredits = pEventData->eventData.pData;

            (void)L2ca_SendLeCredit (pCbNoPeerCredits->deviceId,
                                     pCbNoPeerCredits->cId,
                                     mAppLeCbInitialCredits_c);
            break;
        }

#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
        case mAppEvt_L2capPsmChannelStatusNotification_c:
        {
            l2caLeCbChannelStatusNotification_t* pCBChStatusNotif = pEventData->eventData.pData;

            if (pCBChStatusNotif->status == gL2ca_ChannelStatusChannelIdle_c)
            {
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
                /* Monitor the transfer transitions from in progress to finished */
                static bool_t bTemp = FALSE;
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

                if (BtcsServer_CheckTransferInProgress(pCBChStatusNotif->deviceId) == TRUE)
                {
                    (void)BtcsServer_SendData(pCBChStatusNotif->deviceId, pCBChStatusNotif->cId, gRangingProcResCont_c);
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
                    bTemp = TRUE;
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */
                }
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
                else if (bTemp == TRUE)
                {
                    localizationAlgoResult_t algoResult = {0};

                    gCsTimeInfo.transferEnd = TM_GetTimestamp();
                    bTemp = FALSE;

                    algoResult.algorithm = 0U;
                    algoResult.csConfigDuration = gCsTimeInfo.csConfigEndTs - gCsTimeInfo.csConfigStartTs;
                    algoResult.csProcedureDuration = gCsTimeInfo.csDistMeasDuration;
                    algoResult.transferDuration = gCsTimeInfo.transferEnd - gCsTimeInfo.transferStart;
                    gCsTimeInfo.csDistMeasDuration = 0;
                    gCsTimeInfo.lastAclConnEvtCnt = 0;
                    gCsTimeInfo.noOfRcvSubEvsInConnInt = 0;
                    gCsTimeInfo.csConfigEndTs = 0UL;
                    gCsTimeInfo.csConfigStartTs = 0UL;

                    BleApp_PrintMeasurementResults(pCBChStatusNotif->deviceId,  &algoResult);
                }
                else
                {
                    /* For MISRA compliance */
                }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */
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
}

/*! *********************************************************************************
* \brief        Handles keyboard events.
*
* \param[in]    pEventData    pointer to appEventData_t.
********************************************************************************** */
static void App_HandleKeys(appEventData_t *pEventData)
{
    switch(pEventData->appEvent)
    {
        case mAppEvt_KBD_EventPressPB1_c:
        {
            BleApp_Start();
            break;
        }

        case mAppEvt_KBD_EventLongPB1_c:
        {
            uint8_t mPeerId = 0;
            for (mPeerId = 0; mPeerId < (uint8_t)gAppMaxConnections_c; mPeerId++)
            {
                if (maPeerInformation[mPeerId].deviceId != gInvalidDeviceId_c)
                {
                    (void)Gap_Disconnect(maPeerInformation[mPeerId].deviceId);
                }
            }
            break;
        }


        case mAppEvt_KBD_EventVeryLongPB1_c:
        {
            (void)BleApp_FactoryReset();
            break;
        }

        default:
        {
            ; /* No action required */
        }
        break;
    }
}

/*! *********************************************************************************
* \brief        Handles GattClientCallback events.
*
* \param[in]    pEventData    pointer to appEventData_t.
********************************************************************************** */
static void App_HandleGattClientCallback(appEventData_t *pEventData)
{
    switch(pEventData->appEvent)
    {

        case mAppEvt_GattClientCallback_GattProcError_c:
        {
            BleApp_StateMachineHandler(pEventData->peerDeviceId, mAppEvt_GattProcError_c);
            break;
        }

        case mAppEvt_GattClientCallback_GattProcReadCharacteristicValue_c:
        {
            if (mCurrentCharReadingIndex == mcCharVehiclePsmIndex_c)
            {
                /* The SPSM value shall use big-endian byte order so reverse it when received */
                maPeerInformation[pEventData->peerDeviceId].customInfo.lePsmValue = (uint16_t)Utils_BeExtractTwoByteValue(mValVehiclePsm);
                /* Register DK L2CAP PSM */
                (void)L2ca_RegisterLePsm(maPeerInformation[pEventData->peerDeviceId].customInfo.lePsmValue, gDKMessageMaxLength_c);
                BleApp_StateMachineHandler(pEventData->peerDeviceId, mAppEvt_ReadCharacteristicValueComplete_c);

                /* Read next char */
                mCurrentCharReadingIndex = mcCharVehicleAntennaIdIndex_c;
                (void)GattClient_ReadCharacteristicValue(pEventData->peerDeviceId, &maCharacteristics[mcCharVehicleAntennaIdIndex_c], mcCharVehicleAntennaIdLength_c);
            }
            else if (mCurrentCharReadingIndex == mcCharVehicleAntennaIdIndex_c)
            {
                /* Read next char */
                mCurrentCharReadingIndex = mcCharTxPowerLevelIndex_c;
                (void)GattClient_ReadCharacteristicValue(pEventData->peerDeviceId, &maCharacteristics[mcCharTxPowerLevelIndex_c], mcCharTxPowerLevelLength_c);
            }
            else
            {
                /* All chars read - reset index */
                mCurrentCharReadingIndex = mcCharVehiclePsmIndex_c;
            }
            break;
        }

        case mAppEvt_GattClientCallback_GattProcReadUsingCharacteristicUuid_c:
        {
            gattHandleRange_t handleRange;
            bleUuid_t charUuid;
            handleRange.startHandle = 0x0001U;
            handleRange.endHandle = 0xFFFFU;

            if ((mCurrentCharReadingIndex == mcCharVehiclePsmIndex_c) || (mCurrentCharReadingIndex == mcCharVehiclePsmDkVersionIndex_c))
            {
                maCharacteristics[mcCharVehiclePsmIndex_c].value.paValue = mValVehiclePsm;
                /* length 1 octet, handle 2 octets, value(psm) 2 octets */
                maCharacteristics[mcCharVehiclePsmIndex_c].value.handle = (((uint16_t)maOutCharReadBuffer[2]) << 8) | (uint16_t)maOutCharReadBuffer[1];
                maCharacteristics[mcCharVehiclePsmIndex_c].value.paValue[0] = maOutCharReadBuffer[3];
                maCharacteristics[mcCharVehiclePsmIndex_c].value.paValue[1] = maOutCharReadBuffer[4];
                maPeerInformation[pEventData->peerDeviceId].customInfo.lePsmValue = (uint16_t)Utils_BeExtractTwoByteValue(mValVehiclePsm);
                /* Register DK L2CAP PSM */
                (void)L2ca_RegisterLePsm(maPeerInformation[pEventData->peerDeviceId].customInfo.lePsmValue, gDKMessageMaxLength_c);
                BleApp_StateMachineHandler(pEventData->peerDeviceId, mAppEvt_ReadCharacteristicValueComplete_c);

                if (mCurrentCharReadingIndex == mcCharVehiclePsmDkVersionIndex_c)
                {
                    /* Last byte of the value contains the Supported Features - BIT7 indicates BTCS support */
                    maPeerInformation[pEventData->peerDeviceId].btcsSupported = ((maOutCharReadBuffer[mOutCharReadByteCount - 1U] & 0x80U) != 0x0U);
                }

                /* Read next char if present */
                if (maPeerInformation[pEventData->peerDeviceId].customInfo.hAntennaIdChar != 0x0U)
                {
                    mCurrentCharReadingIndex = mcCharVehicleAntennaIdIndex_c;
                    FLib_MemCpy(charUuid.uuid128, uuid_char_antenna_id, gcBleLongUuidSize_c);

                    (void)GattClient_ReadUsingCharacteristicUuid(pEventData->peerDeviceId,
                                                                 gBleUuidType128_c,
                                                                 &charUuid,
                                                                 &handleRange,
                                                                 maOutCharReadBuffer,
                                                                 mCharReadBufferLength_c,
                                                                 &mOutCharReadByteCount);
                }
                else
                {
                    /* Read next char if present */
                    if (maPeerInformation[pEventData->peerDeviceId].customInfo.hTxPowerChar != 0x0U)
                    {
                        mCurrentCharReadingIndex = mcCharTxPowerLevelIndex_c;
                        charUuid.uuid16 = (uint16_t)gBleSig_TxPower_d;

                        (void)GattClient_ReadUsingCharacteristicUuid(pEventData->peerDeviceId,
                                                                     gBleUuidType16_c,
                                                                     &charUuid,
                                                                     &handleRange,
                                                                     maOutCharReadBuffer,
                                                                     mCharReadBufferLength_c,
                                                                     &mOutCharReadByteCount);
                    }
                }
            }
            else if (mCurrentCharReadingIndex == mcCharVehicleAntennaIdIndex_c)
            {
                maCharacteristics[mcCharVehicleAntennaIdIndex_c].value.paValue = (uint8_t *)&mValVehicleAntennaId;
                /* length 1 octet, handle 2 octets, value 2 octets */
                maCharacteristics[mcCharVehicleAntennaIdIndex_c].value.handle = (((uint16_t)maOutCharReadBuffer[2]) << 8) | (uint16_t)maOutCharReadBuffer[1];
                maCharacteristics[mcCharVehicleAntennaIdIndex_c].value.paValue[0] = maOutCharReadBuffer[3];
                maCharacteristics[mcCharVehicleAntennaIdIndex_c].value.paValue[1] = maOutCharReadBuffer[4];

                /* Read next char if present */
                if (maPeerInformation[pEventData->peerDeviceId].customInfo.hTxPowerChar != 0x0U)
                {
                    mCurrentCharReadingIndex = mcCharTxPowerLevelIndex_c;
                    charUuid.uuid16 = (uint16_t)gBleSig_TxPower_d;

                    (void)GattClient_ReadUsingCharacteristicUuid(pEventData->peerDeviceId,
                                                                 gBleUuidType16_c,
                                                                 &charUuid,
                                                                 &handleRange,
                                                                 maOutCharReadBuffer,
                                                                 mCharReadBufferLength_c,
                                                                 &mOutCharReadByteCount);
                }
            }
            else
            {
                maCharacteristics[mcCharTxPowerLevelIndex_c].value.paValue = (uint8_t *)&mValTxPower;
                /* length 1 octet, handle 2 octets, value 1 octet */
                maCharacteristics[mcCharTxPowerLevelIndex_c].value.handle = (((uint16_t)maOutCharReadBuffer[2]) << 8) | (uint16_t)maOutCharReadBuffer[1];
                maCharacteristics[mcCharTxPowerLevelIndex_c].value.paValue[0] = maOutCharReadBuffer[3];

                /* All chars read - reset index */
                mCurrentCharReadingIndex = mcCharVehiclePsmIndex_c;
            }
            break;
        }

        case mAppEvt_GattClientCallback_GattProcComplete_c:
        {
            BleApp_StateMachineHandler(pEventData->peerDeviceId, mAppEvt_GattProcComplete_c);
            break;
        }

        default:
        {
            ; /* No action required */
        }
        break;
    }
}

/*! *********************************************************************************
* \brief        Handles GenericCallback events.
*
* \param[in]    pEventData    pointer to appEventData_t.
********************************************************************************** */
static void App_HandleGenericCallback(appEventData_t *pEventData)
{
    switch(pEventData->appEvent)
    {

        case mAppEvt_GenericCallback_PeerDisconnected_c:
        {
            /* Factory reset may trigger internal error in some scenarios. */
            BleApp_StateMachineHandler( mCurrentPeerId, mAppEvt_PeerDisconnected_c );
            break;
        }

        case mAppEvt_GenericCallback_LePhyEvent_c:
        {
            gapPhyEvent_t *pPhyEvent = (gapPhyEvent_t *)pEventData->eventData.pData;

            if (pPhyEvent->phyEventType == gPhyRead_c )
            {
                appLocalization_rangeCfg_t locConfig;

                /* Read current CS config */
                (void)AppLocalization_ReadConfig(pPhyEvent->deviceId, &locConfig);

                /* Set the CS PHY according to the connection PHY */
                locConfig.phy = pPhyEvent->rxPhy;

                /* Update CS config with the PHY */
                (void)AppLocalization_WriteConfig(pPhyEvent->deviceId, &locConfig);
            }

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
            if (pPhyEvent->phyEventType == gPhyUpdateComplete_c )
            {
                AppPrintLePhyEvent(pPhyEvent);
            }
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */
            pPhyEvent = NULL;
            break;
        }

        case mAppEvt_GenericCallback_LeScLocalOobData_c:
        {
            if (mCurrentPeerId != gInvalidDeviceId_c)
            {
                FLib_MemCpy(&maPeerInformation[mCurrentPeerId].oobData, pEventData->eventData.pData, sizeof(gapLeScOobData_t));
                BleApp_StateMachineHandler(mCurrentPeerId, mAppEvt_PairingLocalOobData_c);
            }
            break;
        }

        case mAppEvt_GenericCallback_RandomAddressReady_c:
        {
            FLib_MemCpy(gaAppOwnDiscAddress, pEventData->eventData.pData, gcBleDeviceAddressSize_c);
            break;
        }

        case mAppEvt_GenericCallback_CtrlNotifEvent_c:
        {
            bleNotificationEvent_t *pCtrlNotifEvent = (bleNotificationEvent_t *)pEventData->eventData.pData;

            if ((pCtrlNotifEvent->eventType & ((uint16_t)gNotifConnCreated_c | (uint16_t)gNotifPhyUpdateInd_c)) > 0U )
            {
                /* Compute event timestamp. */
                uint64_t bleTime = TM_GetTimestamp();
                /* Get current timestamp */
                mTsUwbDeviceTime = GetUwbClock();
                /* Subtract event delay */

                if (bleTime >= pCtrlNotifEvent->timestamp)
                {
                    mTsUwbDeviceTime = mTsUwbDeviceTime - (bleTime - (uint64_t)pCtrlNotifEvent->timestamp);
                }
                else
                {
                    mTsUwbDeviceTime = mTsUwbDeviceTime - ((0x00000000FFFFFFFFU - (uint64_t)pCtrlNotifEvent->timestamp) + bleTime);
                }

                if ((pCtrlNotifEvent->eventType & (uint16_t)gNotifPhyUpdateInd_c) > 0U)
                {
                    /* Send Time Sync. */
                    uint64_t devEvtCnt = 0xFFFFFFFFFFFFFFFFU;

                    if (maPeerInformation[pCtrlNotifEvent->deviceId].appState == mAppRunning_c)
                    {
                        (void)CCC_SendTimeSync(pCtrlNotifEvent->deviceId, &devEvtCnt, &mTsUwbDeviceTime, 1U);
                    }
                }
            }
            break;
        }
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
        case mAppEvt_GenericCallback_BondCreatedEvent_c:
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
            break;
        }
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

        default:
        {
            ; /* No action required */
        }
        break;
    }
}

/*! *********************************************************************************
* \brief        Handles ConnectionCallback events.
*
* \param[in]    pEventData    pointer to appEventData_t.
********************************************************************************** */
static void App_HandleConnectionCallback(appEventData_t *pEventData)
{
    switch(pEventData->appEvent)
    {
        case mAppEvt_ConnectionCallback_ConnEvtConnected_c:
        {
            appConnectionCallbackEventData_t *pConnectedEventData = (appConnectionCallbackEventData_t *)pEventData->eventData.pData;

            /* Check MISRA directive 4.14 */
            if(pConnectedEventData->peerDeviceId >= gAppMaxConnections_c)
            {
                 /* peerDeviceId bigger then gAppMaxConnections_c */
                 panic(0, (uint32_t)App_HandleConnectionCallback, 0, 0);
            }

            /* Save address used during discovery if controller privacy was used. */
            if (pConnectedEventData->eventData.pConnectedEvent.localRpaUsed)
            {
                FLib_MemCpy(gaAppOwnDiscAddress, pConnectedEventData->eventData.pConnectedEvent.localRpa, gcBleDeviceAddressSize_c);
            }

            /* Update UI */
            LedStopFlashingAllLeds();
            Led1On();

            shell_write("Connected!\r\n");

            maPeerInformation[pConnectedEventData->peerDeviceId].deviceId = pConnectedEventData->peerDeviceId;
            maPeerInformation[pConnectedEventData->peerDeviceId].isBonded = FALSE;
            maPeerInformation[pConnectedEventData->peerDeviceId].nvmIndex = gInvalidNvmIndex_c;

            appLocalization_rangeCfg_t locConfig;
            /* Read current CS config and update procedure repeat interval */
            (void)AppLocalization_ReadConfig(pConnectedEventData->peerDeviceId, &locConfig);

            /* Estimated algo durations (4 AP, 80 channels) for procedure repeat frequency calculation */
            uint32_t algoDurationMs = 0U;
#if defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)
            algoDurationMs += 45U;
#endif /* gAppUseRADEAlgorithm_d */
#if defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)
            algoDurationMs += 25U;
#endif /* gAppUseCDEAlgorithm_d */

            uint32_t procInterval = (uint32_t)(CS_PROC_DURATION_MS_MAX + POSTPROC_VERB_DURATION_MS_MIN + APPLICATION_OFFSET_DURATION_MS + algoDurationMs);
#if defined (BOARD_LOCALIZATION_REVISION_SUPPORT) && (BOARD_LOCALIZATION_REVISION_SUPPORT == 1U)
            procInterval +=  LOC_BOARD_PROC_REPEAT_DELAY;
#endif
            /* Convert ms to connection intervals */
            uint32_t connInterval = (uint32_t)(pConnectedEventData->eventData.pConnectedEvent.connParameters.connInterval);
            if (connInterval >= gGapConnIntervalMin_d && connInterval <= gGapConnIntervalMax_d)
            {
                procInterval = 1U + (procInterval * 1000U)/(connInterval * 1250U);
                locConfig.minPeriodBetweenProcedures = (uint16_t)procInterval;
                locConfig.maxPeriodBetweenProcedures = (uint16_t)procInterval;
            }

            (void)AppLocalization_WriteConfig(pConnectedEventData->peerDeviceId, &locConfig);
            AppLocalization_SetConnectionInterval(pConnectedEventData->peerDeviceId, pConnectedEventData->eventData.pConnectedEvent.connParameters.connInterval);

            /* Read the PHY on which the connection was establihed */
            (void)Gap_LeReadPhy(pConnectedEventData->peerDeviceId);

#if defined(gAppUseBonding_d) && (gAppUseBonding_d)
            (void)Gap_CheckIfBonded(pConnectedEventData->peerDeviceId, &maPeerInformation[pConnectedEventData->peerDeviceId].isBonded, &maPeerInformation[pConnectedEventData->peerDeviceId].nvmIndex);

            if ((maPeerInformation[pConnectedEventData->peerDeviceId].isBonded) &&
                (gBleSuccess_c == Gap_LoadCustomPeerInformation(pConnectedEventData->peerDeviceId,
                    (void*)&maPeerInformation[pConnectedEventData->peerDeviceId].customInfo, 0, (uint16_t)sizeof (appCustomInfo_t))))
            {
                mRestoringBondedLink = TRUE;
                /* Restored custom connection information. Encrypt link */
                (void)Gap_EncryptLink(pConnectedEventData->peerDeviceId);
            }
#endif
            BleApp_StateMachineHandler(pConnectedEventData->peerDeviceId, mAppEvt_PeerConnected_c);
            break;
        }

        case mAppEvt_ConnectionCallback_ConnEvtDisconnected_c:
        {
            /* Reset Service Discovery to be sure*/
            BleServDisc_Stop(pEventData->peerDeviceId);

            shell_write("Disconnected with reason ");
            shell_writeDec((uint32_t)maPeerInformation[pEventData->peerDeviceId].disconReason);
            shell_write("!\r\n");

            LedStopFlashingAllLeds();
            LedStartFlashingAllLeds();

            BleApp_StateMachineHandler(pEventData->peerDeviceId, mAppEvt_PeerDisconnected_c);
            break;
        }

        case mAppEvt_ConnectionCallback_ConnEvtLeScOobDataRequest_c:
        {
            BleApp_StateMachineHandler(pEventData->peerDeviceId, mAppEvt_PairingPeerOobDataReq_c);
            break;
        }

        case mAppEvt_ConnectionCallback_ConnEvtPairingComplete_c:
        {
            BleApp_StateMachineHandler(pEventData->peerDeviceId, mAppEvt_PairingComplete_c);
            break;
        }

        case mAppEvt_ConnectionCallback_ConnEvtEncryptionChanged_c:
        {
            if( mRestoringBondedLink )
            {
                mRestoringBondedLink = FALSE;
                BleApp_StateMachineHandler(pEventData->peerDeviceId, mAppEvt_EncryptionChanged_c);
            }
            break;
        }

        case mAppEvt_ConnectionCallback_ConnEvtAuthenticationRejected_c:
        {
            BleApp_StateMachineHandler(pEventData->peerDeviceId, mAppEvt_AuthenticationRejected_c);
            break;
        }

        case mAppEvt_ConnectionCallback_ConnEvtParameterUpdateComplete_c:
        {
            /* Update connection interval when a Parameter Update procedure completes */
            appConnectionCallbackEventData_t *pConnParamUpdateCompleteEvent = (appConnectionCallbackEventData_t *)pEventData->eventData.pData;
            AppLocalization_SetConnectionInterval(pConnParamUpdateCompleteEvent->peerDeviceId, pConnParamUpdateCompleteEvent->eventData.pConnParamUpdateCompleteEvent.connInterval); 
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
        dkMessageType_t protocol = (dkMessageType_t)pPacket[0];
        uint8_t msgId = pPacket[1];
        uint16_t length = 0;
        FLib_MemCpyReverseOrder(&length, &pPacket[2], gLengthFieldSize_c);

        switch (protocol)
        {
            case gDKMessageTypeFrameworkMessage_c:
            {
                if (msgId == (uint8_t)gDkApduRQ_c)
                {
                    if (maPeerInformation[deviceId].appState == mAppCCCPhase2WaitingForRequest_c)
                    {
                        shell_write("\r\nSPAKE Request received.\r\n");
                        bleResult_t result = CCCPhase2_SendSPAKEResponse(deviceId, &pPacket[4], length);
                        if (result == gBleSuccess_c)
                        {
                            BleApp_StateMachineHandler(deviceId, mAppEvt_SentSPAKEResponse_c);
                        }
                    }
                    else if (maPeerInformation[deviceId].appState == mAppCCCPhase2WaitingForVerify_c)
                    {
                        shell_write("\r\nSPAKE Verify received.\r\n");
                        bleResult_t result = CCCPhase2_SendSPAKEVerify(deviceId, &pPacket[4], length);
                        if (result == gBleSuccess_c)
                        {
                            BleApp_StateMachineHandler(deviceId, mAppEvt_ReceivedSPAKEVerify_c);
                        }
                    }
                    else
                    {
                        /* For MISRA compliance */
                    }
                }
            }
            break;

            case gDKMessageTypeSupplementaryServiceMessage_c:
            {
                if ( (msgId == (uint8_t)gFirstApproachRQ_c) || (msgId == (uint8_t)gFirstApproachRS_c ) )
                {
                    if ( packetLength == (gMessageHeaderSize_c + gPayloadHeaderSize_c + gLengthFieldSize_c + gFirstApproachReqRspPayloadLength) )
                    {
                        uint8_t *pData = &pPacket[gMessageHeaderSize_c + gPayloadHeaderSize_c + gLengthFieldSize_c];

                        /* BD address not used. */
                        pData += gcBleDeviceAddressSize_c;
                        /* Confirm Value */
                        FLib_MemCpy(&maPeerInformation[deviceId].peerOobData.confirmValue, pData, gSmpLeScRandomConfirmValueSize_c);
                        pData += gSmpLeScRandomConfirmValueSize_c;
                        /* Random Value */
                        FLib_MemCpy(&maPeerInformation[deviceId].peerOobData.randomValue, pData, gSmpLeScRandomValueSize_c);

                        /* Send event to the application state machine. */
                        BleApp_StateMachineHandler(deviceId, mAppEvt_PairingPeerOobDataRcv_c);
                    }
                    else
                    {
                        shell_write("\r\nERROR: Invalid length for FirstApproach message.\r\n");
                    }
                }
            }
            break;

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
                            switch (type)
                            {
                                case gBlePairingReady_c:
                                {
                                    shell_write("\r\nReceived Command Complete SubEvent: BLE_pairing_ready\r\n");
                                    BleApp_StateMachineHandler(deviceId, mAppEvt_ReceivedPairingReady_c);
                                }
                                break;

                                default:
                                {
                                    ; /* For MISRA compliance */
                                }
                                break;
                            }
                        }
                    }
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
                /* Print message */
                shell_write("\r\nReceived L2CAP data: ");
                shell_writeN((char const*)pPacket, packetLength);
                SHELL_NEWLINE();
            }
            break;
        }
    }
}

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
/*! *********************************************************************************
* \brief    Set bonding data.
*
********************************************************************************** */
static void BleApp_SetBondingData(appEventData_t *pEventData)
{
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
}

/*! *********************************************************************************
* \brief    Remove bonding data.
*
********************************************************************************** */
static void BleApp_RemoveBondingData(appEventData_t *pEventData)
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
#endif
}

/*! *********************************************************************************
* \brief    List bonding data.
*
********************************************************************************** */
static void BleApp_ListBondingData(void)
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

/*! *********************************************************************************
* \brief    Disconnect received from the BLE application.
*
********************************************************************************** */
static void BleApp_Disconnect(void)
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
}

/*! *********************************************************************************
* \brief        Set CS Create Config default parameters.
*
********************************************************************************** */
static void BleApp_SetCsConfigParams(appEventData_t* pEventData)
{
    appLocalization_rangeCfg_t csConfigParams;
    appCsConfigParams_t *pAppCsConfigParams = pEventData->eventData.pData;

    /* Read current configuration. */
    (void)AppLocalization_ReadConfig(pEventData->peerDeviceId, &csConfigParams);

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
    csConfigParams.cs_sync_phy = pAppCsConfigParams->csSyncPhy;

    (void)AppLocalization_WriteConfig(pEventData->peerDeviceId, &csConfigParams);
};

/*! *********************************************************************************
* \brief        Set CS Procedure default parameters.
*
********************************************************************************** */
static void BleApp_SetCsProcParams(appEventData_t* pEventData)
{
    appLocalization_rangeCfg_t csConfigParams;
    appCsProcedureParams_t *pAppCsProcParams = pEventData->eventData.pData;

    /* Read current configuration. */
    (void)AppLocalization_ReadConfig(pEventData->peerDeviceId, &csConfigParams);

    /* Update configuration with the new values. */
    csConfigParams.maxProcedureDuration = pAppCsProcParams->maxProcedureDuration;
    csConfigParams.minPeriodBetweenProcedures = pAppCsProcParams->minPeriodBetweenProcedures;
    csConfigParams.maxPeriodBetweenProcedures = pAppCsProcParams->maxPeriodBetweenProcedures;
    csConfigParams.maxNumProcedures = pAppCsProcParams->maxNumProcedures;
    csConfigParams.minSubeventLen = pAppCsProcParams->minSubeventLen;
    csConfigParams.maxSubeventLen = pAppCsProcParams->maxSubeventLen;
    csConfigParams.ant_cfg_index = pAppCsProcParams->antCfgIndex;
    csConfigParams.snr_control_init = pAppCsProcParams->snrControlInit;
    csConfigParams.snr_control_refl = pAppCsProcParams->snrControlRefl;

    (void)AppLocalization_WriteConfig(pEventData->peerDeviceId, &csConfigParams);
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

#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

/*! *********************************************************************************
 * \brief        Owner Pairing Certificate Exchange - step 1, SPAKE2+ Response Command
 *
 ********************************************************************************** */
static bleResult_t CCCPhase2_SendSPAKEResponse(deviceId_t deviceId, uint8_t *pData, uint16_t dataLen)
{
    bleResult_t result = gBleSuccess_c;

    /* Dummy data - replace with calls to get actual data */
    uint16_t payloadLen = gDummyPayloadLength_c;
    uint8_t payload[gDummyPayloadLength_c] = gDummyPayload_c;

    result = DK_SendMessage(deviceId,
                            maPeerInformation[deviceId].customInfo.psmChannelId,
                            gDKMessageTypeFrameworkMessage_c,
                            (uint8_t)gDkApduRS_c,
                            payloadLen,
                            payload);
    shell_write("\r\nSPAKE Response sent.\r\n");

    return result;
}

/*! *********************************************************************************
 * \brief        Owner Pairing Certificate Exchange - step 2, SPAKE2+ Verify Command
 *
 ********************************************************************************** */
static bleResult_t CCCPhase2_SendSPAKEVerify(deviceId_t deviceId, uint8_t *pData, uint16_t dataLen)
{
    bleResult_t result = gBleSuccess_c;

    /* Dummy data - replace with calls to get actual data */
    uint16_t payloadLen = gDummyPayloadLength_c;
    uint8_t payload[gDummyPayloadLength_c] = gDummyPayload_c;

    result = DK_SendMessage(deviceId,
                            maPeerInformation[deviceId].customInfo.psmChannelId,
                            gDKMessageTypeFrameworkMessage_c,
                            (uint8_t)gDkApduRS_c,
                            payloadLen,
                            payload);
    shell_write("\r\nSPAKE Verify sent.\r\n");

    return result;
}

/*! *********************************************************************************
 * \brief        Sends DK SubEvents to Car Anchor.
 *
 ********************************************************************************** */
static bleResult_t CCC_SendSubEvent(deviceId_t deviceId,
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

/*! *********************************************************************************
 * \brief        Device sends Time Sync to Car Anchor.
 *
 ********************************************************************************** */
static bleResult_t CCC_SendTimeSync(deviceId_t deviceId,
                                    uint64_t *pDevEvtCnt,
                                    uint64_t *pUwbDevTime,
                                    uint8_t success)
{
    bleResult_t result = gBleSuccess_c;

    uint8_t payload[gTimeSyncPayloadLength_c] = {0};
    uint8_t *pPtr = payload;

    /* Add DeviceEventCount */
    FLib_MemCpy(pPtr, pDevEvtCnt, sizeof(uint64_t));
    pPtr += sizeof(uint64_t);
    /* Add UWB_Device_Time */
    FLib_MemCpy(pPtr, pUwbDevTime, sizeof(uint64_t));
    pPtr += sizeof(uint64_t);
    /* Skip UWB_Device_Time_Uncertainty */
    pPtr++;
    /* Skip UWB_Clock_Skew_Measurement_available */
    pPtr++;
    /* Skip Device_max_PPM */
    pPtr += sizeof(uint16_t);
    /* Add Success */
    *pPtr = success;
    /* Skip RetryDelay */
    pPtr += sizeof(uint16_t);

    result = DK_SendMessage(deviceId,
                            maPeerInformation[deviceId].customInfo.psmChannelId,
                            gDKMessageTypeSupplementaryServiceMessage_c,
                            (uint8_t)gTimeSync_c,
                            gTimeSyncPayloadLength_c,
                            payload);
    shell_write("\r\nTime Sync sent with UWB Device Time:");
    shell_writeHexLe((uint8_t*)pUwbDevTime, (uint8_t)sizeof(uint64_t));
    shell_write("\r\n");

    return result;
}

/*! *********************************************************************************
 * \brief        First Approach messages enables BLE OOB Secure LE pairing for both owner and friend devices
 *
 ********************************************************************************** */
static bleResult_t CCC_FirstApproachReq(deviceId_t deviceId, uint8_t* pBdAddr, gapLeScOobData_t* pOobData)
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
        pData += gcBleDeviceAddressSize_c;
        FLib_MemCpy(pData, pOobData->confirmValue, gSmpLeScRandomConfirmValueSize_c);
        pData += gSmpLeScRandomConfirmValueSize_c;
        FLib_MemCpy(pData, pOobData->randomValue, gSmpLeScRandomValueSize_c);

        shell_write("\r\nOOB Data: ");
        shell_write("\r\nAddress, Confirm, Random:");
        shell_write(" ");
        shell_writeHex(pBdAddr, gcBleDeviceAddressSize_c);
        shell_write(" ");
        shell_writeHex(pOobData->confirmValue, gSmpLeScRandomConfirmValueSize_c);
        shell_write(" ");
        shell_writeHex(pOobData->randomValue, gSmpLeScRandomConfirmValueSize_c);
        shell_write("\r\n");

        result = DK_SendMessage(deviceId,
                                maPeerInformation[deviceId].customInfo.psmChannelId,
                                gDKMessageTypeSupplementaryServiceMessage_c,
                                (uint8_t)gFirstApproachRQ_c,
                                gFirstApproachReqRspPayloadLength,
                                aPayload);
    }

    return result;
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
#endif
/*! *********************************************************************************
* @}
********************************************************************************** */