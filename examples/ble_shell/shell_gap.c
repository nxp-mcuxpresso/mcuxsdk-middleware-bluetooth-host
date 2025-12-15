/*! *********************************************************************************
 * \addtogroup SHELL GAP
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2025 NXP
*
*
* \file
*
* This file is the source file for the GAP Shell module
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "EmbeddedTypes.h"

/* Framework / Drivers */
#include "fsl_component_timer_manager.h"
#include "fsl_shell.h"
#include "fsl_component_panic.h"
#include "fsl_component_mem_manager.h"
#include "FunctionLib.h"
#include "fsl_os_abstraction.h"
#include "board.h"

/* BLE Host Stack */
#include "gatt_interface.h"
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gatt_db_app_interface.h"
#include "gap_interface.h"
#include "l2ca_interface.h"

#include "shell_gattdb.h"
#include "shell_gap.h"
#include "ble_shell.h"
#include "ble_conn_manager.h"

#include "app_conn.h"
#include "app_scanner.h"
#include "app_advertiser.h"
#include "app.h"

#include <string.h>
#include <stdlib.h>
/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

#define mShellGapCmdsCount_c NumberOfElements(mGapShellCmds)

#define mShellGapMaxScannedDevicesCount_c   20U
#define mShellGapMaxDeviceNameLength_c      20U

#define mShellErrorStatus                   "\r\n-->  An error has occurred!"
#define mShellGapNoConnRssiTimeout_c        500U

/* Minimum and maximum Tx power values
* NOTE1! BLE5.1 defines this range to -127..20
* NOTE2! See platform datasheet for tx_power_dBm lookup table. For MKW37 datasheet section 4.3, table 6 */
#define mShellMinTxPower_c                  (-127)
#define mShellMaxTxPower_c                  (127)

#if defined(BLE_SHELL_AE_SUPPORT) && (BLE_SHELL_AE_SUPPORT)
#define mShellGapDefaultAdvHandle_c         (1U)
#endif

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
/* Value used to set every response slot start for the advertiser */
#define mShellGapPerResponseSlotStart_c     (0U)
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef struct gapCmds_tag
{
    char*       name;
    shell_status_t      (*cmd)(uint8_t argc, char * argv[]);
}gapCmds_t;

typedef struct gapScannedDevices_tag
{
    bleAddressType_t    addrType;
    bleDeviceAddress_t  aAddress;
    uchar_t             name[mShellGapMaxDeviceNameLength_c];
}gapScannedDevices_t;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

/* Shell GAP Events Callback */
void ShellGap_AdvertisingCallback
(
    gapAdvertisingEvent_t* pAdvertisingEvent
);

void ShellGap_ScanningCallback
(
    gapScanningEvent_t* pScanningEvent
);

/* Shell API Functions */
static shell_status_t ShellGap_DeviceAddress(uint8_t argc, char **argv);
static shell_status_t ShellGap_DeviceName(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_StartAdvertising(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_StopAdvertising(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_SetAdvertisingParameters(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_ChangeAdvertisingData(uint8_t argc, char * argv[]);

static shell_status_t ShellGap_StartScanning(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_StopScanning(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_SetScanParameters(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_ChangeScanData(uint8_t argc, char * argv[]);

static shell_status_t ShellGap_Connect(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_SetConnectionParameters(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_Disconnect(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_UpdateConnection(uint8_t argc, char * argv[]);

static shell_status_t ShellGap_Pair(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_PairCfg(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_EnterPasskey(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_Bonds(uint8_t argc, char * argv[]);

static shell_status_t ShellGap_SetPhy(uint8_t argc, char * argv[]);
static void ShellGap_PrintPhy(uint8_t physBitmask);
static shell_status_t ShellGap_SetTxPower(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_RSSIMonitor(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_RSSIMonitorStop(uint8_t argc, char * argv[]);

static void ShellGap_ReadRssiTmrCb (void *param);
static void ShellGap_ReadRssi (void *param);

#if defined(BLE_SHELL_AE_SUPPORT) && (BLE_SHELL_AE_SUPPORT)
static shell_status_t ShellGap_StartExtAdvertising(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_StopExtAdvertising(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_SetExtAdvertisingParameters(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_ChangeExtAdvertisingData(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_ChangeExtScanData(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_PeriodicSyncCreate(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_PeriodicSyncStop(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_ChangePeriodicAdvData(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_SetPeriodicAdvParameters(uint8_t argc, char * argv[]);
#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
static shell_status_t ShellGap_SetPeriodicAdvSubeventData(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_SetPeriodicAdvResponseData(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_SetPeriodicSyncSubevent(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_ConnectFromPawr(uint8_t argc, char * argv[]);
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */
static shell_status_t ShellGap_StopPeriodicAdv(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_StartPeriodicAdv(uint8_t argc, char * argv[]);
static void ShellGap_SetDefaultExtData(gapAdvertisingData_t* pExtAdvData);
#endif /* BLE_SHELL_AE_SUPPORT */

#if defined(BLE_SHELL_DBAF_SUPPORT) && (BLE_SHELL_DBAF_SUPPORT)
static shell_status_t ShellGap_SetDecisionInstructions(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_AddDecisionInstruction(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_DeleteDecisionInstructions(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_SetExtAdvDecisionData(uint8_t argc, char * argv[]);
#endif /* BLE_SHELL_DBAF_SUPPORT */

static void ShellGap_HandleConnectedEvt(deviceId_t peerDeviceId, gapConnectedEvent_t *pEvent);
static void ShellGap_SendSmpKeys(deviceId_t peerDeviceId, gapSmpKeyFlags_t *pRequestedKeys);
static void ShellGap_HandleLongTermKeyRequestEvt(deviceId_t peerDeviceId, gapLongTermKeyRequestEvent_t *pEvent);
static void ShellGap_HandlePairingCompleteEvt(deviceId_t peerDeviceId, gapPairingCompleteEvent_t *pEvent);
static void ShellGap_HandleEncryptionChangedEvt(gapEncryptionChangedEvent_t *pEvent);
static void ShellGap_HandleParameterUpdateCompleteEvt(gapConnParamsUpdateComplete_t *pEvent);
static void ShellGap_HandleRssiReadEvt(int8_t rssi_dBm);
static void ShellGap_HandleTxPowerLevelSetCompleteEvt(gapGenericEvent_t* pGenericEvent);

#if defined(BLE_SHELL_AE_SUPPORT) && (BLE_SHELL_AE_SUPPORT)
static void ShellGap_PrintExtAdvParametersCfg(void);
static void ShellGap_ConfigureExtAdvInterval(uint8_t argc, char * argv[], uint32_t argIdx);
static void ShellGap_ConfigureExtAdvPhy(uint8_t argc, char * argv[], uint32_t argIdx);
#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
static bool_t ShellGap_ErasePeriodicSubeventData(void);
static bool_t ShellGap_ErasePeriodicResponseData(uint8_t idx);
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */
#endif /* BLE_SHELL_AE_SUPPORT */

#if defined(BLE_SHELL_MONADV_SUPPORT) && (BLE_SHELL_MONADV_SUPPORT)
static shell_status_t ShellGap_AddDeviceToMonAdvList(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_RemoveDeviceFromMonAdvList(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_ClearMonAdvList(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_EnableMonAdv(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_ReadMonAdvListSize(uint8_t argc, char * argv[]);
#endif /* BLE_SHELL_MONADV_SUPPORT */
#if BLE_SHELL_CONN_SBR_SUPPORT
static shell_status_t ShellGap_ConfigConnSubrateParams(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_SetDefaultConnSbrParams(uint8_t argc, char * argv[]);
static shell_status_t ShellGap_ConnSbrReq(uint8_t argc, char * argv[]);
static void ShellGap_HandleSubrateChangeEvt(deviceId_t peerDeviceId, gapSubrateChangeEvent_t *pEvent);
#endif /* BLE_SHELL_CONN_SBR_SUPPORT */
static void ShellGap_GenericCallbackAdv (gapGenericEvent_t* pGenericEvent);
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static const gapCmds_t mGapShellCmds[] =
{
    {"address",     ShellGap_DeviceAddress},
    {"devicename",  ShellGap_DeviceName},

    {"advstart",    ShellGap_StartAdvertising},
    {"advstop",     ShellGap_StopAdvertising},
    {"advcfg",      ShellGap_SetAdvertisingParameters},
    {"advdata",     ShellGap_ChangeAdvertisingData},

    {"scanstart",   ShellGap_StartScanning},
    {"scanstop",    ShellGap_StopScanning},
    {"scancfg",     ShellGap_SetScanParameters},
    {"scandata",    ShellGap_ChangeScanData},

    {"connect",     ShellGap_Connect},
    {"connectcfg",  ShellGap_SetConnectionParameters},
    {"disconnect",  ShellGap_Disconnect},
    {"connupdate",  ShellGap_UpdateConnection},

    {"pair",        ShellGap_Pair},
    {"paircfg",     ShellGap_PairCfg},
    {"enterpin",    ShellGap_EnterPasskey},
    {"bonds",       ShellGap_Bonds},

    {"phy",         ShellGap_SetPhy},
    {"txpower",     ShellGap_SetTxPower},
    {"rssimonitor", ShellGap_RSSIMonitor},
    {"rssistop",    ShellGap_RSSIMonitorStop},

#if defined(BLE_SHELL_AE_SUPPORT) && (BLE_SHELL_AE_SUPPORT)
    {"extadvstart", ShellGap_StartExtAdvertising},
    {"extadvstop",  ShellGap_StopExtAdvertising},
    {"extadvcfg",   ShellGap_SetExtAdvertisingParameters},
    {"extadvdata",  ShellGap_ChangeExtAdvertisingData},
    {"extscandata", ShellGap_ChangeExtScanData},
    {"periodicstart", ShellGap_StartPeriodicAdv},
    {"periodicstop",  ShellGap_StopPeriodicAdv},
    {"periodiccfg",   ShellGap_SetPeriodicAdvParameters},
    {"periodicdata",  ShellGap_ChangePeriodicAdvData},
#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
    {"periodicsubeventdata",   ShellGap_SetPeriodicAdvSubeventData},
    {"periodicresponsedata",   ShellGap_SetPeriodicAdvResponseData},
    {"periodicsyncsubevent",   ShellGap_SetPeriodicSyncSubevent},
    {"connectpawr", ShellGap_ConnectFromPawr},
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */
    {"periodicsync",  ShellGap_PeriodicSyncCreate},
    {"periodicsyncstop",  ShellGap_PeriodicSyncStop},
#endif /* BLE_SHELL_AE_SUPPORT */

#if defined(BLE_SHELL_DBAF_SUPPORT) && (BLE_SHELL_DBAF_SUPPORT)
    {"setdecinstr",     ShellGap_SetDecisionInstructions},
    {"adddecinstr",     ShellGap_AddDecisionInstruction},
    {"deldecinstr",     ShellGap_DeleteDecisionInstructions},
    {"extadvdecdata",   ShellGap_SetExtAdvDecisionData},
#endif /* BLE_SHELL_DBAF_SUPPORT */
#if defined(BLE_SHELL_MONADV_SUPPORT) && (BLE_SHELL_MONADV_SUPPORT)
    {"monadvadd",       ShellGap_AddDeviceToMonAdvList},
    {"monadvrem",       ShellGap_RemoveDeviceFromMonAdvList},
    {"monadvclear",     ShellGap_ClearMonAdvList},
    {"monadven",        ShellGap_EnableMonAdv},
    {"monadvsize",      ShellGap_ReadMonAdvListSize},
#endif /* BLE_SHELL_MONADV_SUPPORT */
#if BLE_SHELL_CONN_SBR_SUPPORT
    {"sbrcfg",          ShellGap_ConfigConnSubrateParams},
    {"setdefsbrparam",  ShellGap_SetDefaultConnSbrParams},
    {"connsbrreq",      ShellGap_ConnSbrReq},
#endif /* BLE_SHELL_CONN_SBR_SUPPORT */
};

static bool_t mIsBonded = FALSE;
static bool_t mScanningOn = FALSE;

static bool_t mIsCentral = FALSE;

static gapScannedDevices_t     mScannedDevices[mShellGapMaxScannedDevicesCount_c];
static uint8_t                 mScannedDevicesCount;
static uint8_t                 mConnectToDeviceId = mShellGapMaxScannedDevicesCount_c;

static gapConnectionParameters_t mConnectionParams;

static appScanningParams_t mAppScanParams = {
    &gAppScanParams,
    gGapDuplicateFilteringEnable_c,
    gGapScanContinuously_d,
    gGapScanPeriodicDisabled_d
};

static appAdvertisingParams_t mAppAdvParams = {
    &gAdvParams,
    &gAppAdvertisingData,
    &gAppScanRspData
};

/* Pointer to ADV data */
static uint8_t  *mpAdvData = NULL;
static uint8_t  *mpScanRspData = NULL;

/* Pointer to EXT_ADV data */
static uint8_t  *mpExtAdvData = NULL;

/* Pointer to EXT_SCAN data */
static uint8_t  *mpExtScanData = NULL;

/* Pointer to periodic advertising data */
static uint8_t  *mpPeriodicData = NULL;

#if defined(BLE_SHELL_AE_SUPPORT) && (BLE_SHELL_AE_SUPPORT)
static uint8_t mDataFlags = 0x06U;
static uint8_t mDefaultAdStructure[] = "\r\nEXT_ADV_DEMO\r\n";

/* Default extended advertising structures */
static gapAdStructure_t mExtDefaultAdStruct1 =
{

        .length = 2U,
        .adType = gAdFlags_c,
        .aData = &mDataFlags,
};
static gapAdStructure_t mExtDefaultAdStruct2 =
{
    .length = (uint8_t)sizeof(mDefaultAdStructure) + 1U,
    .adType = gAdCompleteLocalName_c,
    .aData = mDefaultAdStructure,
};

static bool_t bDefaultExtData = FALSE;

/* Default Periodic advertiser */
static gapPeriodicAdvSyncReq_t mPeriodicSyncPeer = {.options.filterPolicy = (gapCreateSyncReqFilterPolicy_t)gUseCommandParameters_c,
                                                    .SID = 1U,
                                                    .peerAddressType = gBleAddrTypePublic_c,
                                                    .peerAddress = {0, 0, 0, 0, 0, 0},
                                                    .skipCount = 0,
                                                    .timeout = gGapScanIntervalMax_d};

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
/* Periodic Sync Subevent Request Parameters */
static gapPeriodicSyncSubeventParameters_t *mpPeriodicSubeventSync = NULL;
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */

static const char* mShellEADataPattern = SHELL_EXT_ADV_DATA_PATTERN;

static appExtAdvertisingParams_t mAppExtAdvParams = {
    &gExtAdvParams,
    &gAppExtAdvertisingData,
    &gAppExtScanRspData,
#if (defined(BLE_SHELL_DBAF_SUPPORT) && BLE_SHELL_DBAF_SUPPORT)
    &gAppExtAdvDecisionData,
#endif
    mShellGapDefaultAdvHandle_c,
    gBleExtAdvNoDuration_c,
    gBleExtAdvNoMaxEvents_c
};
#endif /* BLE_SHELL_AE_SUPPORT */

/* Gap Event string header */
static const char mGapEventHeader[] = "\r\n-->  GAP Event: ";

/* Include the ">" character and the NULL terminator. */
static char     mDeviceName[mMaxCharValueLength_d + 2]="";

/* Default scanning parameters */
static uint16_t mScanDuration = gGapScanContinuously_d;
static uint16_t mScanPeriod = gGapScanPeriodicDisabled_d;

/* String dictionary corresponding to gapLePhyMode_t */
static const char* mShellPhy[] =
{
    "Invalid",
    "1M ",
    "2M ",
    "Coded",
};

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
/* String dictionary corresponding to gapLePhyMode_t */
static const char* mShellAdvPhyOpt[] =
{
    "Le Coding No Preference",
    "Le Coding S2",
    "Le Coding S8",
    "Le Coding S2 Required",
    "Le Coding S8 Required",
};

/* Handle value used when periodic sync is established */
static uint16_t mPeriodicSyncHandle = 0U;
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */

/* Timer used by rssimonitor */
static TIMER_MANAGER_HANDLE_DEFINE(mBleShellGapTimerId);

/* RSSI monitor demo */
static bool_t mRSSIMonitor = FALSE;
static bleDeviceAddress_t mMonitorPeer;

/* If the advertising callback is not set, no event is received for advertising commands */
static bool_t mAdvertisingCbSet = FALSE;

#if defined(BLE_SHELL_MONADV_SUPPORT) && (BLE_SHELL_MONADV_SUPPORT)
/* The Enable parameter may be set to 0x01 at any time as a method of restarting monitoring. */
/* The success event for this command does not indicate the new state. */
static bool_t mLastMonAdvState = FALSE;
static bool_t mNewMonAdvState = FALSE;
#endif /* defined(BLE_SHELL_MONADV_SUPPORT) && (BLE_SHELL_MONADV_SUPPORT) */

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
 * \brief        Entry point for "gap" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
shell_status_t ShellGap_Command(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    shell_status_t result = kStatus_SHELL_Error;
    uint32_t i = 0U;

    /* For "gap "command, return command usage */
    if (argc >= 2)
    {
        /* Handle the GAP command in the appropriate function */
        for (i = 0U; i < mShellGapCmdsCount_c; i++)
        {
            if(0 == strcmp((char*)argv[1], mGapShellCmds[i].name) )
            {
                result = mGapShellCmds[i].cmd((uint8_t)argc-2U, (char **)(&argv[2]));
                break;
            }
        }

        if (i >= mShellGapCmdsCount_c)
        {
            (void)SHELL_PrintfSynchronization((shell_handle_t)shellHandle, "\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        }
    }
    else
    {
        (void)SHELL_PrintfSynchronization((shell_handle_t)shellHandle, "\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
    }

    return result;
}
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
 * \brief        Handles "gap address" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_DeviceAddress(uint8_t argc, char **argv)
{
    shell_status_t result = kStatus_SHELL_Error;

    if (argc == 0U)
    {
        /* If no address value was given, print the current Public (or Random Static) address */
#if defined(gRandomStaticAddress_d) && (gRandomStaticAddress_d > 0)
        shell_write(mGapEventHeader);
        shell_write("Random Static Address:");

        for(uint32_t i = sizeof(bleDeviceAddress_t); i > 0U; i-- )
        {
            shell_writeHex(gaBleDeviceAddress[i - 1U]);
        }
        SHELL_NEWLINE();
#else
        (void)Gap_ReadPublicDeviceAddress();
#endif
        result = kStatus_SHELL_Success;
    }
#if defined(BLE_SHELL_AE_SUPPORT) && (BLE_SHELL_AE_SUPPORT)
    else if (argc == 1U)
    {
        /* Set Random address */
        /* Check that address's length is valid */
        if (gcBleDeviceAddressSize_c == BleApp_ParseHexValue(argv[0]))
        {
            FLib_MemCpyReverseOrder(&gExtAdvParams.ownRandomAddr, argv[0], sizeof(bleDeviceAddress_t));

            (void)Gap_SetRandomAddress(gExtAdvParams.ownRandomAddr);

            /* Set advertising address type to random */
            gExtAdvParams.ownAddressType = gBleAddrTypeRandom_c;
            result = kStatus_SHELL_Success;
        }
    }
    else
    {
        /* If more than one argument was passed, set the address of the peer device */
        for (uint32_t i = 2U; i < argc; i += 2U)
        {
            if (0 == strcmp((char*)argv[i], "-peer"))
            {
                /* Check that address's length is valid */
                if (gcBleDeviceAddressSize_c == BleApp_ParseHexValue(argv[i+1U]))
                {
                    FLib_MemCpy(&gExtAdvParams.peerAddress, argv[0], sizeof(bleDeviceAddress_t));
                }
            }
            /* Set the address type of the peer */
            if (0 == strcmp((char*)argv[i], "-type"))
            {
                gExtAdvParams.peerAddressType = (bleAddressType_t)BleApp_atoi(argv[i+1U]);
            }
        }
        result = kStatus_SHELL_Success;
    }
#endif /* BLE_SHELL_AE_SUPPORT */

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gatt devicename" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_DeviceName(uint8_t argc, char * argv[])
{
    shell_status_t returnStatus = kStatus_SHELL_Success;
    uint16_t valueHandle = 0, serviceHandle = 0, length;
    bleResult_t result;
    bleUuid_t   uuid;


    uuid.uuid16 = gBleSig_GenericAccessProfile_d;
    (void)GattDb_FindServiceHandle(1, gBleUuidType16_c, &uuid, &serviceHandle);

    uuid.uuid16 = gBleSig_GapDeviceName_d;
    (void)GattDb_FindCharValueHandleInService(serviceHandle, gBleUuidType16_c, &uuid, &valueHandle);

    /* If no other argument, print the device name*/
    if (argc == 0U)
    {
        result =  GattDb_ReadAttribute(valueHandle, mMaxCharValueLength_d, (uint8_t*)mDeviceName, &length);

        if (result != gGattDbSuccess_c)
        {
            /* An error occurred when reading the device name*/
            shell_write("\r\n-->  GATTDB Event: Procedure Error ");
            SHELL_NEWLINE();
        }
        else
        {
            /* Print GAP device name */
            shell_write("\r\n-->  GATTDB Event: Attribute Read ");
            shell_write("\r\n     Value: ");
            shell_write(mDeviceName);
            SHELL_NEWLINE();
        }
    }
    /* Set the device name to the given value in argv[1] */
    else
    {
        length = (uint16_t)strlen(argv[0]);

        /* Check if length is valid */
        if (length > mMaxCharValueLength_d)
        {
            shell_write("\r\n-->  Variable length exceeds maximum!");
            returnStatus = kStatus_SHELL_Error;
        }
        else
        {
            /* Write the new GAP device name */
            result =  GattDb_WriteAttribute(valueHandle, length, (uint8_t*)argv[0]);

            if (result != gGattDbSuccess_c)
            {
                /* GATTDB Error */
                shell_write("\r\n-->  GATTDB Event: Procedure Error ");
                SHELL_NEWLINE();
            }
            else
            {
                shell_write("\r\n-->  GATTDB Event: Attribute Written ");
                SHELL_NEWLINE();
                /* Attribute was written successfully, therefore change prompt to reflect the new name */
                FLib_MemCpy(mDeviceName, argv[0], length);
                mDeviceName[length] = '>';
                mDeviceName[length+1U] = '\0';
                SHELL_ChangePrompt((shell_handle_t)g_shellHandle, mDeviceName);
            }
        }
    }

    return returnStatus;
}

/*! *********************************************************************************
 * \brief        Handles "gap advstart" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_StartAdvertising(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    /* Do not accept command if other arguments were passed */
    if (argc != 0U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else
    {
        /* Central device is the scanner */
        mIsCentral = FALSE;
        (void)BluetoothLEHost_StartAdvertising(&mAppAdvParams, ShellGap_AdvertisingCallback, ShellGap_ConnectionCallback);
        mAdvertisingCbSet = TRUE;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap advstop" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_StopAdvertising(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    /* Do not accept command if other arguments were passed */
    if (argc != 0U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else
    {
        (void)Gap_StopAdvertising();
        if (mAdvertisingCbSet == FALSE)
        {
            /* Advertising was never started. No event from controller will be received */
            shell_write(mShellErrorStatus);
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap advcfg" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetAdvertisingParameters(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;
    bool_t bValidCmd = FALSE;

    if (argc == 0U)
    {
        /* If no parameters were given, print the current configuration */
        uint16_t maxInterval = gAdvParams.maxInterval * 625U / 1000U;
        shell_write("\r\n-->  Advertising Parameters:");
        shell_write("\r\n    -->  Advertising Interval: ");
        shell_writeDec(maxInterval);
        shell_write(" ms");
        /* Print advertising type */
        shell_write("\r\n    -->  Advertising Type: ");

        switch(gAdvParams.advertisingType)
        {
            case gAdvConnectableUndirected_c:
            {
                shell_write("ADV_IND");
            }
            break;
            case gAdvDirectedHighDutyCycle_c:
            {
                shell_write("ADV_IND");
            }
            break;
            case gAdvNonConnectable_c:
            {
                shell_write("ADV_NONCON_IND");
            }
            break;
            case gAdvScannable_c:
            {
                shell_write("ADV_SCAN_IND");
            }
            break;
            case gAdvDirectedLowDutyCycle_c:
            {
                shell_write("ADV_DIRECT");
            }
            break;
            default:
            {
                ; /* Should not get here */
            }
            break;
        }
        SHELL_NEWLINE();
    }
    else
    {
        /* Search through the arguments for keywords */
        for (uint32_t i = 0U; i < argc; i += 2U)
        {
            if(0 == strcmp((char*)argv[i], "-interval") && ((i+1U) < argc))
            {
                uint16_t interval = (uint16_t)BleApp_atoi(argv[i+1U])*1000U/625U;
                /* Set min and max interval to the same value */
                gAdvParams.maxInterval = interval;
                gAdvParams.minInterval = interval;
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-type") && ((i+1U) < argc))
            {
                uint8_t advType = (uint8_t)BleApp_atoi(argv[i+1U]);
                gAdvParams.advertisingType = (bleAdvertisingType_t)advType;
                bValidCmd = TRUE;
            }
        }

        if (bValidCmd)
        {
            /* Set the new advertising parameter set if at least one parameter was set */
            (void)Gap_SetAdvertisingParameters(&gAdvParams);
            result = kStatus_SHELL_Success;
        }
        else
        {
            shell_write("\r\nNot a valid command.  Enter \"help\" to view a list of available commands.\r\n\r\n");
            result = kStatus_SHELL_Error;
        }
    }
    return result;
}

/*! *********************************************************************************
 * \brief        Adds advertising data to the already existing one.
 *
 * \param[in]    pAdvData       Pointer to the storage of advertising data
 * \param[in]    type           The type of the advertising structure
 * \param[in]    pData          Pointer to data to be added
 * \param[in]    fIsScanRsp     TRUE - if the same data is also used in scan response
 *
 * \return       bool_t         TRUE - if data was successfully added, FALSE - otherwise
 ********************************************************************************** */
static bool_t ShellGap_AppendAdvData
(
    gapAdvertisingData_t *pAdvData,
    gapAdType_t type,
    char * pData,
    bool_t fIsScanRsp,
    bool_t bIsExtAdvData,
    bool_t bIsPeriodicAdvData
)
{
    uint8_t advIdx = 0;
    uint16_t advCursor = 0U;
    uint8_t length;
    uint8_t *pAdvArrayData;

    if ((fIsScanRsp == TRUE) && (bIsExtAdvData == TRUE))
    {
        pAdvArrayData = mpExtScanData;
    }
    else if (fIsScanRsp == TRUE)
    {
        pAdvArrayData = mpScanRspData;
    }
    else if (bIsExtAdvData == TRUE)
    {
        pAdvArrayData = mpExtAdvData;
    }
    else if (bIsPeriodicAdvData == TRUE)
    {
        pAdvArrayData = mpPeriodicData;
    }
    else
    {
        pAdvArrayData = mpAdvData;
    }

    if (pAdvArrayData == NULL)
    {
        /* No existing ADV data. Allocate memory */
        pAdvArrayData = MEM_BufferAlloc(gcGapMaxAdvertisingDataLength_c);

        if  (NULL != pAdvArrayData)
        {
            if ((fIsScanRsp == TRUE) && (bIsExtAdvData == TRUE))
            {
                mpExtScanData = pAdvArrayData;
            }
            else if (fIsScanRsp == TRUE)
            {
                mpScanRspData = pAdvArrayData;
            }
            else if (bIsExtAdvData == TRUE)
            {
                mpExtAdvData = pAdvArrayData;
            }
            else if (bIsPeriodicAdvData == TRUE)
            {
                mpPeriodicData = pAdvArrayData;
            }
            else
            {
                mpAdvData = pAdvArrayData;
            }
        }
        else
        {
            return FALSE;
        }

    }

    for (advIdx=0; advIdx<pAdvData->cNumAdStructures; advIdx++)
    {
        if (pAdvData->aAdStructures[advIdx].adType == type)
        {
            /* The same ad structure type already exists */
            return TRUE;
        }

        advCursor += pAdvData->aAdStructures[advIdx].length - 1U;
    }

    /* Get length of the string */
    length = (uint8_t)strlen(pData);

    /* Copy ADV payload*/
    pAdvData->aAdStructures[advIdx].aData = pAdvArrayData + advCursor;

    switch(type)
    {
        /* Process string payload type */
        case gAdShortenedLocalName_c:
        case gAdCompleteLocalName_c:
        {
            /* Check if we got space. Take into account length and type octets */
            if ((advCursor + length + 2U * (pAdvData->cNumAdStructures)) > gcGapMaxAdvertisingDataLength_c )
            {
                /* No more free space */
                return FALSE;
            }

            FLib_MemCpy(pAdvArrayData + advCursor, pData, length);
        }
        break;

        default:
        {
            /* Check if we got space. Take into account length and type octets */
            uint8_t availableSize = gcGapMaxAdvertisingDataLength_c - (advCursor + 2U * (pAdvData->cNumAdStructures));

            /* Halve length because string size is twice the hex array */
            length = ((length % 2U) != 0U) ? (uint8_t)((length/2U) + 1U) : (uint8_t)(length/2U);

            if (length > availableSize )
            {
                /* No more free space */
                return FALSE;
            }

            /* Length will be updated to the actual size parsed by BleApp_ParseHexValue */
            length = BleApp_ParseHexValue(pData);
            if (length == 0U)
            {
                return FALSE;
            }
            FLib_MemCpy(pAdvArrayData + advCursor, pData, length);
        }
        break;
    }

    pAdvData->aAdStructures[advIdx].adType = type;
    pAdvData->aAdStructures[advIdx].length = length + 1U;

    pAdvData->cNumAdStructures += 1U;

    return TRUE;
}

/*! *********************************************************************************
 * \brief        Handles "gap advdata" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_ChangeAdvertisingData(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;

    switch(argc)
    {
        case 0:
        {
            /* No argument was passed */
            shell_write("\r\r\n\n-->  Advertising Data:");
            result = kStatus_SHELL_Success;
        }
        break;

        case 1:
        {
            /* Clear the existing data and free the memory buffer that kept it */
            if(0 == strcmp((char*)argv[0], "-erase"))
            {
                gAppAdvertisingData.cNumAdStructures = 0;
                if (mpAdvData != NULL)
                {
                    (void)MEM_BufferFree(mpAdvData);
                    mpAdvData = NULL;
                }
                shell_write("\r\r\n\n-->  Advertising Data Erased.\r\n");
                result = kStatus_SHELL_Success;
            }
        }
        break;

        default:
        {
            /* Add new advertising data */
            gapAdType_t advType = (gapAdType_t)BleApp_atoi(argv[0]);
            if (FALSE == ShellGap_AppendAdvData(&gAppAdvertisingData, advType, argv[1], FALSE, FALSE, FALSE))
            {
                /* An error occurred when copying the data */
                shell_write("\r\r\n\n-->  Failed to set Advertising Data!");
                result = kStatus_SHELL_Success;
            }
            else
            {
                result = kStatus_SHELL_Success;
            }
        }
        break;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap scanstart" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_StartScanning(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;
    gapFilterDuplicates_t enableFilterDuplicates = gGapDuplicateFilteringDisable_c;

    /* Do not accept command if other arguments were passed */
    if ((argc != 0U) && (0 == strcmp((char*)argv[0], "filter")))
    {
        enableFilterDuplicates = gGapDuplicateFilteringEnable_c;
    }

    mIsCentral = TRUE;
    mAppScanParams.enableDuplicateFiltering = enableFilterDuplicates;
    mAppScanParams.duration = mScanDuration;
    mAppScanParams.period = mScanPeriod;

    if(gBleSuccess_c != BluetoothLEHost_StartScanning(&mAppScanParams, ShellGap_ScanningCallback))
    {
        shell_write(mShellErrorStatus);
        result = kStatus_SHELL_Success;
    }

    return result;
}
/*! *********************************************************************************
 * \brief        Handles "gap scanstop" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_StopScanning(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;
    /* Do not accept command if other arguments were passed */
    if (argc != 0U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else
    {
        if (mScanningOn)
        {
            (void)Gap_StopScanning();
        }
        else
        {
            shell_write("\r\n-->  Scanning is already stopped!");
            result = kStatus_SHELL_Success;
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap scancfg" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetScanParameters(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;
    bool_t bValidCmd = FALSE;

    if (argc == 0U)
    {
        /* If no argument was passed, print the current scanning parameters */
        shell_write("\r\n-->  Scan Parameters:");
        shell_write("\r\n    -->  Scan Interval: ");
        /* Convert the value to milliseconds */
        shell_writeDec((uint32_t)gAppScanParams.interval * 625U / 1000U);
        shell_write(" ms");
        shell_write("\r\n    -->  Scan Window: ");
        /* Convert the value to milliseconds */
        shell_writeDec((uint32_t)gAppScanParams.window * 625U / 1000U);
        shell_write(" ms");
        shell_write("\r\n    -->  Scan Type: ");
        (gAppScanParams.type == gScanTypeActive_c) ? shell_write("ACTIVE") : shell_write("PASSIVE");
        shell_write("\r\n    -->  Scanning PHYs: ");
        ShellGap_PrintPhy(gAppScanParams.scanningPHYs);
        shell_write("\r\n    -->  Scan Duration: ");
        shell_writeDec((uint32_t)mScanDuration);
        shell_write(" ms");
        shell_write("\r\n    -->  Scan Period: ");
        shell_writeDec((uint32_t)mScanPeriod);
        shell_write(" ms");
#if defined(BLE_SHELL_DBAF_SUPPORT) && (BLE_SHELL_DBAF_SUPPORT)
        shell_write("\r\n    -->  Scan Filter Policy: ");
        shell_writeDec((uint8_t)gAppScanParams.filterPolicy);
#endif /* BLE_SHELL_DBAF_SUPPORT */
        SHELL_NEWLINE();
    }
    else
    {
        /* Store the given parameters in the local scanning data structure */
        for (uint32_t i = 0U; i < argc; i += 2U)
        {
            /* Search for keywords */
            if(0 == strcmp((char*)argv[i], "-interval") && ((i+1U) < argc))
            {
                /* Set the same value for both min and max interval */
                uint16_t interval = (uint16_t)BleApp_atoi(argv[i+1U])*1000U/625U;
                gAppScanParams.interval = interval;
                gConnReqParams.scanInterval = interval;
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-window") && ((i+1U) < argc))
            {
                /* Convert the value from milliseconds */
                uint16_t window = (uint16_t)BleApp_atoi(argv[i+1U])*1000U/625U;
                gAppScanParams.window = window;
                gConnReqParams.scanWindow = window;
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-type") && ((i+1U) < argc))
            {
                uint8_t scanType = (uint8_t)BleApp_atoi(argv[i+1U]);
                gAppScanParams.type = (bleScanType_t)scanType;
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-phy") && ((i+1U) < argc))
            {
                uint8_t scanningPhy = (uint8_t)BleApp_atoi(argv[i+1U]);
                gAppScanParams.scanningPHYs = scanningPhy;
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-duration") && ((i+1U) < argc))
            {
                mScanDuration = (uint16_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-period") && ((i+1U) < argc))
            {
                mScanPeriod = (uint16_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }

#if defined(BLE_SHELL_DBAF_SUPPORT) && (BLE_SHELL_DBAF_SUPPORT)
            if(0 == strcmp((char*)argv[i], "-filter") && ((i+1U) < argc))
            {
                gAppScanParams.filterPolicy = (bleScanningFilterPolicy_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }
#endif /* BLE_SHELL_DBAF_SUPPORT */
        }

        /* A valid command should set at least one scanning parameter */
        if (!bValidCmd)
        {
            shell_write("\r\nNot a valid command.  Enter \"help\" to view a list of available commands.\r\n\r\n");
            result = kStatus_SHELL_Error;
        }
    }
    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap scandata" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_ChangeScanData(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;
    switch(argc)
    {
        case 0:
        {
            /* No argument was passed */
            shell_write("\r\r\n\n-->  Scan Response Data:");
            result = kStatus_SHELL_Success;
        }
        break;

        case 1:
        {
            /* Clear the existing data and free the memory buffer that kept it */
            if(0 == strcmp((char*)argv[0], "-erase"))
            {
                gAppScanRspData.cNumAdStructures = 0;
                if (mpAdvData != NULL)
                {
                    (void)MEM_BufferFree(mpAdvData);
                }
                shell_write("\r\r\n\n-->  Scan Response Erased.");
                result = kStatus_SHELL_Success;
            }
        }
        break;

        default:
        {
            /* Add new scanning data */
            gapAdType_t advType = (gapAdType_t)BleApp_atoi(argv[0]);
            if (FALSE == ShellGap_AppendAdvData(&gAppAdvertisingData, advType, argv[1], TRUE, FALSE, FALSE))
            {
                /* An error occurred when copying the data */
                shell_write("\r\r\n\n-->  Failed to set Scanning Data!");
                result = kStatus_SHELL_Success;
            }
            else
            {
                (void)Gap_SetAdvertisingData(NULL, &gAppScanRspData);
                result = kStatus_SHELL_Success;
            }
        }
        break;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap connect" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_Connect(uint8_t argc, char * argv[])
{
    uint8_t deviceId;
    shell_status_t result = kStatus_SHELL_Error;

    /* Must contain the peer device ID */
    if (argc != 1U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else
    {
        deviceId = (uint8_t)BleApp_atoi(argv[0]);

        if (deviceId > mScannedDevicesCount)
        {
            shell_write("\r\n-->  The Id of the device cannot be found! Please enter a valid Id!\r\n\r\n");
            result = kStatus_SHELL_Error;
        }
        else
        {
            mConnectToDeviceId =  deviceId;
            /* The device initiating the connection is the Central */
            mIsCentral = TRUE;

            gConnReqParams.scanInterval = gAppScanParams.interval;
            gConnReqParams.scanWindow = gAppScanParams.window;
            gConnReqParams.peerAddressType = mScannedDevices[deviceId].addrType;
            FLib_MemCpy(gConnReqParams.peerAddress,
                        mScannedDevices[deviceId].aAddress,
                        sizeof(bleDeviceAddress_t));

            /* Stop scanning if it's on*/
            if (mScanningOn)
            {
                (void)Gap_StopScanning();
                result = kStatus_SHELL_Success;
            }

            /* Initiates connection to the scanned device */
            if (gBleSuccess_c != BluetoothLEHost_Connect(&gConnReqParams, ShellGap_ConnectionCallback))
            {
                shell_write(mShellErrorStatus);
                result = kStatus_SHELL_Success;
            }
        }
    }
    return result;
}

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
/*! *********************************************************************************
 * \brief        Handles "gap connectpawr" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_ConnectFromPawr(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;
    gapConnectionFromPawrParameters_t pawrConnReqParams = {0};
    bleAddressType_t addrType = 0xFF;
    bleDeviceAddress_t addr = {0};
    uint8_t advHandle = mAppExtAdvParams.handle;
    uint8_t subevent = 0xFF;
    /* Use this bitmask to indicate if required parameters are not introduced */
    uint8_t paramsValidBitmask = 0U;

    /* For no argument, print current configuration */
    if (argc >= 6U)
    {
        /* Search for keywords */
        for (uint32_t i = 0U; i < argc; i += 2U)
        {
            if (0 == strcmp((char*)argv[i], "-advhandle") && ((i + 1U) < argc))
            {
                /* Optional argument, set implicitly from default */
                advHandle = (bleAddressType_t)BleApp_atoi(argv[i + 1U]);
            }

            if (0 == strcmp((char*)argv[i], "-subevent") && ((i + 1U) < argc))
            {
                subevent = (bleAddressType_t)BleApp_atoi(argv[i + 1U]);
                /* Mandatory argument */
                paramsValidBitmask |= 1U << 0U;
            }

            if (0 == strcmp((char*)argv[i], "-peer") && ((i + 1U) < argc))
            {
                /* Check that address's length is valid */
                if (gcBleDeviceAddressSize_c == BleApp_ParseHexValue(argv[i + 1U]))
                {
                    FLib_MemCpyReverseOrder(&addr, argv[i + 1U], sizeof(bleDeviceAddress_t));
                }
                /* Mandatory argument */
                paramsValidBitmask |= 1U << 1U;
            }

            if (0 == strcmp((char*)argv[i], "-peeraddrtype") && ((i + 1U) < argc))
            {
                addrType = (bleAddressType_t)BleApp_atoi(argv[i + 1U]);
                /* Mandatory argument */
                paramsValidBitmask |= 1U << 2U;
            }
        }

        /* Only allow the command to the Host if all required parameters are given */
        if (paramsValidBitmask == 0x07U)
        {
            gConnReqParams.scanInterval = gAppScanParams.interval;
            gConnReqParams.scanWindow = gAppScanParams.window;
            gConnReqParams.peerAddressType = addrType;
            FLib_MemCpy(gConnReqParams.peerAddress,
                        addr,
                        sizeof(bleDeviceAddress_t));

            FLib_MemCpy(&pawrConnReqParams,
                        &gConnReqParams,
                        sizeof(gConnReqParams));

            pawrConnReqParams.advHandle = advHandle;
            pawrConnReqParams.subevent = subevent;

            /* Initiates connection to the scanned device */
            if (gBleSuccess_c != BluetoothLEHost_ConnectFromPawr(&pawrConnReqParams, ShellGap_ConnectionCallback))
            {
                shell_write(mShellErrorStatus);
                result = kStatus_SHELL_Success;
            }
        }
        else
        {
            result = kStatus_SHELL_Error;
        }
    }
    else
    {
        result = kStatus_SHELL_Error;
    }

    if (result == kStatus_SHELL_Error)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
    }

    return result;
}
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */

/*! *********************************************************************************
 * \brief        Handles "gap connectcfg" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetConnectionParameters(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;
    bool_t bValidCmd = FALSE;

    if (argc == 0U)
    {
        bValidCmd = TRUE;
    }

    /* Search through the arguments for keywords */
    for (uint32_t i = 0U; i < argc; i += 2U)
    {
        if(0 == strcmp((char*)argv[i], "-interval") && ((i+1U) < argc))
        {
            /* Set the min and max interval to the same value */
            uint16_t interval = (uint16_t)BleApp_atoi(argv[i+1U])*8U/10U;
            gConnReqParams.connIntervalMin = interval;
            gConnReqParams.connIntervalMax = interval;
            bValidCmd = TRUE;
        }

        if(0 == strcmp((char*)argv[i], "-latency") && ((i+1U) < argc))
        {
            /* The maximum number of consecutive connection events the Peripheral is allowed to ignore. Default: 0 */
            uint16_t latency = (uint16_t)BleApp_atoi(argv[i+1U]);
            gConnReqParams.connLatency = latency;
            bValidCmd = TRUE;
        }

        if(0 == strcmp((char*)argv[i], "-timeout") && ((i+1U) < argc))
        {
            /* The maximum time interval between consecutive over-the-air packets; Default: 32s */
            uint16_t timeout = (uint16_t)BleApp_atoi(argv[i+1U])/10U;
            gConnReqParams.supervisionTimeout = timeout;
            bValidCmd = TRUE;
        }

        if(0 == strcmp((char*)argv[i], "-phy") && ((i+1U) < argc))
        {
            uint8_t connectingPhy = (uint8_t)BleApp_atoi(argv[i+1U]);
            gConnReqParams.initiatingPHYs = connectingPhy;
            bValidCmd = TRUE;
        }
#if defined(BLE_SHELL_DBAF_SUPPORT) && (BLE_SHELL_DBAF_SUPPORT)
        if(0 == strcmp((char*)argv[i], "-filter") && ((i+1U) < argc))
        {
            uint8_t filterPolicy = (uint8_t)BleApp_atoi(argv[i+1U]);
            gConnReqParams.filterPolicy = filterPolicy;
            bValidCmd = TRUE;
        }
#endif /* BLE_SHELL_DBAF_SUPPORT */
    }

    /* Print the connection parameters */
    if (bValidCmd)
    {
        shell_write("\r\n-->  Connection Parameters:");
        shell_write("\r\n    -->  Connection Interval: ");
        /* Convert the value to milliseconds */
        shell_writeDec((uint32_t)gConnReqParams.connIntervalMax * 10U / 8U);
        shell_write(" ms");

        shell_write("\r\n    -->  Connection Latency: ");
        shell_writeDec(gConnReqParams.connLatency);

        shell_write("\r\n    -->  Supervision Timeout: ");
        /* Convert the value to milliseconds */
        shell_writeDec((uint32_t)gConnReqParams.supervisionTimeout * 10U);
        shell_write(" ms");

        shell_write("\r\n    -->  Connecting PHYs: ");
        ShellGap_PrintPhy(gConnReqParams.initiatingPHYs);

#if defined(BLE_SHELL_DBAF_SUPPORT) && (BLE_SHELL_DBAF_SUPPORT)
        shell_write("\r\n    -->  Connection Filter Policy: ");
        shell_writeDec((uint8_t)gConnReqParams.filterPolicy);
#endif /* BLE_SHELL_DBAF_SUPPORT */

        SHELL_NEWLINE();
        result = kStatus_SHELL_Success;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap disconnect" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_Disconnect(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;
    deviceId_t peerId = gInvalidDeviceId_c;

    if (argc != 1U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else
    {
        peerId = (deviceId_t)BleApp_atoi(argv[0]);
    }

    if ((peerId < gAppMaxConnections_c) && (IS_CONNECTED(peerId) != 0U))
    {
        (void)Gap_Disconnect(peerId);
    }
    else
    {
        shell_write("Invalid peer ID!");
        SHELL_NEWLINE();
        result = kStatus_SHELL_Success;
    }
    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap connupdate" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_UpdateConnection(uint8_t argc, char * argv[])
{
    uint16_t minInterval, maxInterval, latency, timeout;
    deviceId_t peerId;
    shell_status_t result = kStatus_SHELL_Success;

    /* Check number of arguments */
    if (argc != 5U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }

    peerId = (deviceId_t)BleApp_atoi(argv[0]);

    /* Convert from milliseconds */
    minInterval = (uint16_t)BleApp_atoi(argv[1])*8U/10U;
    maxInterval = (uint16_t)BleApp_atoi(argv[2])*8U/10U;
    latency = (uint16_t)BleApp_atoi(argv[3]);
    timeout = (uint16_t)BleApp_atoi(argv[4])/10U;

    if ((peerId < gAppMaxConnections_c) && (IS_CONNECTED(peerId) != 0U))
    {
        (void)Gap_UpdateConnectionParameters(peerId, minInterval, maxInterval, latency, timeout,
                                    gGapConnEventLengthMin_d, gGapConnEventLengthMax_d);
    }
    else
    {
        shell_write("Invalid peer ID!");
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap pair" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_Pair(uint8_t argc, char * argv[])
{
    deviceId_t peerId;
    shell_status_t result = kStatus_SHELL_Success;

    if (argc != 1U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else
    {
        peerId = (deviceId_t)BleApp_atoi(argv[0]);

        /* Device must be connected before pairing */
        if (IS_CONNECTED(peerId) == 0U)
        {
            shell_write("\r\n-->  Please connect the node first...");
            result = kStatus_SHELL_Error;
        }
        else
        {
            shell_write("\r\n-->  Pairing...\r\n");

            if (mIsCentral)
            {
                (void)Gap_Pair(peerId, &gPairingParameters);
            }
            else
            {
                /* Inform the peer Central about the local security requirements */
                (void)Gap_SendPeripheralSecurityRequest(peerId, &gPairingParameters);
            }
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap paircfg" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_PairCfg(uint8_t argc, char * argv[])
{
    bool_t bValidCmd = FALSE;

    /* If no arguments were passed, print the current configuration */
    if (argc == 0U)
    {
        bValidCmd = TRUE;
    }

    /* Search through the arguments for keywords */
    for (uint32_t i=0U; i<argc; i+=2U)
    {
        /* Store the given parameters in the local data structure */
        if(0 == strcmp((char*)argv[i], "-usebonding") && ((i+1U) < argc))
        {
            uint8_t usebonding = (uint8_t)BleApp_atoi(argv[i+1U]);
            gPairingParameters.withBonding = (usebonding != 0U) ? TRUE:FALSE;
            bValidCmd = TRUE;
        }

        if(0 == strcmp((char*)argv[i], "-seclevel") && ((i+1U) < argc))
        {
            /* Holds both security level and security mode */
            uint8_t level = (uint8_t)BleApp_AsciiToHex(argv[i+1U], FLib_StrLen(argv[i+1U]));
            uint8_t secLevel = level & 0x0FU;
            uint8_t secMode = level & 0xF0U;
            if ((gapSecurityLevel_t)(secLevel) <= gSecurityLevel_WithMitmProtection_c &&
                (gapSecurityMode_t)(secMode) <= gSecurityMode_2_c)
            {
                gPairingParameters.securityModeAndLevel = (gapSecurityModeAndLevel_t)level;
                bValidCmd = TRUE;
            }
        }

        if(0 == strcmp((char*)argv[i], "-keyflags") && ((i+1U) < argc))
        {
            /* SMP keys to be distributed by the Central/Peripheral */
            uint8_t flags = (uint8_t)BleApp_AsciiToHex(argv[i+1U], FLib_StrLen(argv[i+1U]));
            gPairingParameters.centralKeys = (gapSmpKeyFlags_t)(flags & 0x07U);
            gPairingParameters.peripheralKeys = (gapSmpKeyFlags_t)(flags & 0x07U);
            bValidCmd = TRUE;
        }
    }

    /* Print the pairing configuration */
    if (bValidCmd)
    {
        shell_write("\r\n-->  Pairing Configuration:");
        shell_write("\r\n    -->  Use Bonding: ");
        shell_writeBool(gPairingParameters.withBonding);

        shell_write("\r\n    -->  SecurityLevel: ");
        shell_writeHex(gPairingParameters.securityModeAndLevel);

        shell_write("\r\n    -->  Flags: ");
        shell_writeHex(gPairingParameters.securityModeAndLevel);

        SHELL_NEWLINE();
        return kStatus_SHELL_Success;
    }
    else
    {
        return kStatus_SHELL_Error;
    }
}

/*! *********************************************************************************
 * \brief        Handles "gap enterpin" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_EnterPasskey(uint8_t argc, char * argv[])
{
    uint32_t passKey;
    deviceId_t peerId;

    if (argc != 2U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        return kStatus_SHELL_Error;
    }

    peerId = (deviceId_t)BleApp_atoi(argv[0]);

    /* Read PIN */
    passKey = (uint32_t)BleApp_atoi(argv[1]);

    if (IS_CONNECTED(peerId) != 0U)
    {
        (void)Gap_EnterPasskey(peerId, passKey);
    }
    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
 * \brief        Handles "gap bonds" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_Bonds(uint8_t argc, char * argv[])
{
    uint8_t i = 0, count = 0;
    shell_status_t result = kStatus_SHELL_Success;

    switch(argc)
    {
        /* If no other argument is passed, print the current list of bonded peers */
        case 0:
        {
            /* Get number of bonded devices */
            (void)Gap_GetBondedDevicesCount(&count);

            if (count == 0U)
            {
                shell_write("\r\n-->  No bonds found on the device! \r\n");
                result = kStatus_SHELL_Success;
            }
            else
            {
                void* pBuffer = NULL;

                /* Allocate buffer for name */
                pBuffer = MEM_BufferAlloc(mShellGapMaxDeviceNameLength_c);

                if (pBuffer == NULL)
                {
                    shell_write("\r\nMemory allocation error!\r\n\r\n");
                    result = kStatus_SHELL_Error;
                }
                else
                {
                    /* Print the names of bonded devices */
                    for (i=0; i<count;i++)
                    {
                        shell_write("\r\n-->  ");
                        shell_writeDec(i);
                        shell_write(". ");
                        (void)Gap_GetBondedDeviceName(i, pBuffer, mShellGapMaxDeviceNameLength_c);

                        if (strlen(pBuffer) > 0U)
                        {
                            shell_write(pBuffer);
                        }
                        else
                        {
                            shell_write("[No saved name]");
                        }
                    }
                    SHELL_NEWLINE();
                    /* Free memory */
                    (void)MEM_BufferFree(pBuffer);
                    result = kStatus_SHELL_Success;
                }
            }
        }
        break;

        case 1:
        {
            /* "-erase" removes all bonds */
            if(0 == strcmp((char*)argv[0], "-erase"))
            {
                /* Command is valid only when device is disconnected */
                if (gActiveConnections != 0U)
                {
                    shell_write("\r\n-->  Please disconnect the node first...");
                    result = kStatus_SHELL_Error;
                }

                if (result != kStatus_SHELL_Error)
                {
                    if (Gap_RemoveAllBonds() == gBleSuccess_c)
                    {

#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
                        gcBondedDevices = 0U;
#endif /* (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U)) */
                        shell_write("\r\n-->  Bonds removed!\r\n");
                        result = kStatus_SHELL_Success;
                    }
                    else
                    {
                        shell_write("\r\n-->  Operation failed!\r\n");
                        result = kStatus_SHELL_Error;
                    }
                }
            }
        }
        break;

        case 2:
        {
            /* Remove specific bond */
            if(0 == strcmp((char*)argv[0], "-remove"))
            {
                uint8_t index = (uint8_t)BleApp_atoi(argv[1]);

                if (gActiveConnections != 0U)
                {
                    shell_write("\r\n-->  Please disconnect the node first...\r\n");
                    result = kStatus_SHELL_Error;
                }

                if (result != kStatus_SHELL_Error)
                {
                    /* Get number of bonded devices */
                    (void)Gap_GetBondedDevicesCount(&count);

                    /* Validate bond index and remove it */
                    if ((count > 0U) && (index < count) &&
                        (Gap_RemoveBond(index) == gBleSuccess_c))
                    {
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
                        gcBondedDevices--;
#endif /* (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U)) */
                        shell_write("\r\n-->  Bond removed!\r\n");
                        result = kStatus_SHELL_Success;
                    }
                    else
                    {
                        shell_write("\r\n-->  Operation failed!\r\n");
                        result = kStatus_SHELL_Error;
                    }
                }
            }
        }
        break;

        /* If any other number of arguments, return usage */
        default:
            result = kStatus_SHELL_Error;
            break;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Parses gapScannedDevice_t data structure and prints the device's
 *               name and RSSI
 *
 * \param[in]    pData           Pointer to gapScannedDevice_t data structure
 *
 ********************************************************************************** */
static void ShellGap_ParseScannedDevice(gapScannedDevice_t* pData)
{
    uint32_t index = 0;
    uint8_t nameLength;
    uint8_t aux;

    /* Parse data structure and print name, address and RSSI */
    while (index < pData->dataLength)
    {
        gapAdStructure_t adElement;

        adElement.length = pData->data[index];
        adElement.adType = (gapAdType_t)pData->data[index + 1U];
        adElement.aData = &pData->data[index + 2U];

        /* Check if device advertises its local name */
        if ((adElement.adType == gAdShortenedLocalName_c) ||
          (adElement.adType == gAdCompleteLocalName_c))
        {
            nameLength = (uint8_t)MIN(adElement.length-1U, mShellGapMaxDeviceNameLength_c);
            FLib_MemCpy(mScannedDevices[mScannedDevicesCount].name, adElement.aData, nameLength);

            shell_write((char*)mScannedDevices[mScannedDevicesCount].name);
            shell_write(" ");
        }

        /* Move on to the next AD element type */
        index += (uint32_t)adElement.length + (uint8_t)sizeof(uint8_t);
    }

    for(uint32_t i = sizeof(bleDeviceAddress_t); i > 0U; i-- )
    {
        shell_writeHex(pData->aAddress[i - 1U]);
    }
    shell_write(" ");

    /* Temporary store scanned data to use for connection */
    mScannedDevices[mScannedDevicesCount].addrType = pData->addressType;
    FLib_MemCpy(mScannedDevices[mScannedDevicesCount].aAddress,
                pData->aAddress,
                sizeof(bleDeviceAddress_t));

    if(((uint8_t)pData->rssi >> 7) != 0U)
    {
        /* Negative Value */
        (void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, "-");
        aux = ~((uint8_t) pData->rssi - 1U);
        pData->rssi = (int8_t)aux;
    }
    shell_writeDec((uint32_t)pData->rssi);
    shell_write(" dBm");

    /* Keep track of the number of scanned devices */
    mScannedDevicesCount++;
}
#if defined(BLE_SHELL_AE_SUPPORT) && (BLE_SHELL_AE_SUPPORT)
/*! *********************************************************************************
 * \brief        Handles "gap extscandata" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_ChangeExtScanData(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;
    gapAdType_t advType;

    /* If no arguments were passed, set the default large scan response data */
    if (argc != 0U)
    {
        if(0 == strcmp((char*)argv[0], "-erase"))
        {
            /* Clean the existing scan response data */
            gAppExtScanRspData.cNumAdStructures = 0U;
            if (NULL != mpExtScanData)
            {
                (void)MEM_BufferFree(mpExtScanData);
                mpExtScanData = NULL;
            }
            shell_write("\r\r\n\n-->  Extended Scan Response Erased.");
            result = kStatus_SHELL_Success;
        }
        else
        {
            if(argc == 2U)
            {
                /* Get type and payload */
                advType = (gapAdType_t)BleApp_atoi(argv[0]);
                /* Append it to current data */
                (void)ShellGap_AppendAdvData(&gAppExtScanRspData, advType, argv[1], TRUE, TRUE, FALSE);
                result = kStatus_SHELL_Success;
            }
            else
            {
                shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
                result = kStatus_SHELL_Error;
            }
        }
    }
    else
    {
        /* Set large scan response data */
        ShellGap_SetDefaultExtData(&gAppExtScanRspData);
        bDefaultExtData = TRUE;
        result = kStatus_SHELL_Success;
    }

    if (result == kStatus_SHELL_Success)
    {
        /* Set data in GAP */
        (void)Gap_SetExtAdvertisingData(gExtAdvParams.handle, NULL, &gAppExtScanRspData);
        /* Free the default extended scan data if added */
        if (bDefaultExtData == TRUE)
        {
            /* The first 2 AD structures are statically allocated */
            for (uint32_t iCount = 2U; iCount < gAppExtScanRspData.cNumAdStructures; iCount++)
            {
                (void)MEM_BufferFree(gAppExtScanRspData.aAdStructures[iCount].aData);
            }
        }

        result = kStatus_SHELL_Success;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Parses gapExtScannedDevice_t data structure and prints the device's
 *               name and RSSI
 *
 * \param[in]    pData           Pointer to gapExtScannedDevice_t data structure
 *
 ********************************************************************************** */
static void ShellGap_ParseExtScannedDevice(gapExtScannedDevice_t* pData)
{
    uint32_t index = 0U;
    uint8_t nameLength;
    uint8_t aux;

    /* Print address and RSSI of the scanned device */
    for(uint32_t i = sizeof(bleDeviceAddress_t); i > 0U; i-- )
    {
        shell_writeHex(pData->aAddress[i - 1U]);
    }

    shell_write(" ");

    if(((uint8_t)pData->rssi >> 7) != 0U)
    {
        /* Negative Value */
        (void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, "-");
        aux = ~((uint8_t)pData->rssi - 1U);
        pData->rssi = (int8_t)aux;
    }
    shell_writeDec((uint32_t)pData->rssi);
    shell_write(" dBm");
    shell_write("\r\n Advertising Extended Data:\r\n");

    /* Print extended advertising data */
    while (index < pData->dataLength)
    {
        gapAdStructure_t adElement;

        adElement.length = pData->pData[index];
        adElement.adType = (gapAdType_t)pData->pData[index + 1U];
        adElement.aData = &pData->pData[index + 2U];

        if ((adElement.adType == gAdShortenedLocalName_c) ||
          (adElement.adType == gAdCompleteLocalName_c)||
          (adElement.adType == gAdManufacturerSpecificData_c))
        {
            nameLength = MIN(adElement.length-1U, mShellGapMaxDeviceNameLength_c);
            FLib_MemCpy(mScannedDevices[mScannedDevicesCount].name, &adElement.aData, nameLength);

            shell_writeN((char*)adElement.aData, (uint32_t)adElement.length - 1U);
        }

        /* Move on to the next AD element type */
        index += (uint32_t)adElement.length + sizeof(uint8_t);
    }


    /* Temporary store scanned data to use for connection */
    mScannedDevices[mScannedDevicesCount].addrType = pData->addressType;
    FLib_MemCpy(mScannedDevices[mScannedDevicesCount].aAddress,
                pData->aAddress,
                sizeof(bleDeviceAddress_t));


    /* Increment total number of scanned devices */
    mScannedDevicesCount++;
}

/*! *********************************************************************************
 * \brief        Handles "gap extadvstart" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_StartExtAdvertising(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    gUseShellThrGenericCb = FALSE;
    mIsCentral = FALSE;

    mAppExtAdvParams.handle = gExtAdvParams.handle;
    if (gBleSuccess_c != BluetoothLEHost_StartExtAdvertising(&mAppExtAdvParams,
                                                             ShellGap_AdvertisingCallback,
                                                             ShellGap_ConnectionCallback))
    {
        shell_write(mShellErrorStatus);
        result = kStatus_SHELL_Success;
    }
    mAdvertisingCbSet = TRUE;

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap extadvstop" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_StopExtAdvertising(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    if ((gBleSuccess_c != Gap_StopExtAdvertising(gExtAdvParams.handle))
        || (mAdvertisingCbSet == FALSE))
    {
        shell_write(mShellErrorStatus);
        result = kStatus_SHELL_Success;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap extadvdata" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_ChangeExtAdvertisingData(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    if (argc > 0U)
    {
        /* "gap extadvdata -erase" -- free the added ext adv data, but keep the default ext adv structures */
        if (0 == strcmp((char*)argv[0], "-erase"))
        {
            gAppExtAdvertisingData.cNumAdStructures = 0;
            if (bDefaultExtData == TRUE)
            {
                bDefaultExtData = FALSE;
                
                /* Free the default extended data if added */
                /* The first 2 AD structures are statically allocated */
                for (uint32_t iCount = 2U; iCount < gAppExtAdvertisingData.cNumAdStructures; iCount++)
                {
                    (void)MEM_BufferFree(gAppExtAdvertisingData.aAdStructures[iCount].aData);
                }
            }
            else
            {
                if (mpExtAdvData != NULL)
                {
                    (void)MEM_BufferFree(mpExtAdvData);
                    mpExtAdvData = NULL;
                }
            }
            shell_write("\r\r\n\n-->  Extended Advertising Data Erased.");
        }
        /* "gap extadvdata [type] [payload]" */
        else
        {
            if (argc == 2U)
            {
                /* Delete the default ext adv data, if is set */
                if (bDefaultExtData == TRUE)
                {
                    gAppExtAdvertisingData.cNumAdStructures = 0;
                    bDefaultExtData = FALSE;
                }
                gapAdType_t advType = (gapAdType_t)BleApp_atoi(argv[0]);
                (void)ShellGap_AppendAdvData(&gAppExtAdvertisingData, (gapAdType_t)advType, argv[1], FALSE, TRUE, FALSE);
                result = kStatus_SHELL_Success;
            }
            else
            {
                shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
                result = kStatus_SHELL_Error;
            }
        }
    }
    /* If no argument was passed add the default large extended advertising data */
    else
    {
        ShellGap_SetDefaultExtData(&gAppExtAdvertisingData);
        bDefaultExtData = TRUE;
        result = kStatus_SHELL_Success;
    }

    if (result != kStatus_SHELL_Success)
    {
        gUseShellThrGenericCb = FALSE;
         result = kStatus_SHELL_Success;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap extadvcfg" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetExtAdvertisingParameters(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    /* For no argument, print current configuration */
    if (argc == 0U)
    {
        ShellGap_PrintExtAdvParametersCfg();
    }
    else
    {
        /* Search for keywords */
        for (uint32_t i = 0U; i < argc; i += 2U)
        {
            ShellGap_ConfigureExtAdvInterval(argc, argv, i);

            if(0 == strcmp((char*)argv[i], "-type") && ((i+1U) < argc))
            {
                /* connectable and scannable --> not permitted */
                /* connectable and chained adv data --> not permitted */
                gExtAdvParams.extAdvProperties = (bleAdvRequestProperties_t)BleApp_atoi(argv[i+1U]);
            }

            ShellGap_ConfigureExtAdvPhy(argc, argv, i);

            if(0 == strcmp((char*)argv[i], "-tx") && ((i+1U) < argc))
            {
                gExtAdvParams.txPower = (int8_t)BleApp_atoi(argv[i+1U]);
            }

            if(0 == strcmp((char*)argv[i], "-sn") && ((i+1U) < argc))
            {
                gExtAdvParams.enableScanReqNotification = (bool_t)BleApp_atoi(argv[i+1U]);;
            }

            if(0 == strcmp((char*)argv[i], "-peer") && ((i+1U) < argc))
            {
                /* Check that address's length is valid */
                if (gcBleDeviceAddressSize_c == BleApp_ParseHexValue(argv[i+1U]))
                {
                    FLib_MemCpy(&gExtAdvParams.peerAddress, argv[i+1U], sizeof(bleDeviceAddress_t));
                }
            }

            if(0 == strcmp((char*)argv[i], "-peeraddrtype") && ((i+1U) < argc))
            {
                gExtAdvParams.peerAddressType = (bleAddressType_t)BleApp_atoi(argv[i+1U]);
            }

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
            if (0 == strcmp((char*)argv[i], "-pphyopt") && ((i + 1U) < argc))
            {
                gExtAdvParams.primaryAdvPhyOptions = (gapLePhyOptionsFlags_t)BleApp_atoi(argv[i + 1U]);
            }

            if (0 == strcmp((char*)argv[i], "-sphyopt") && ((i + 1U) < argc))
            {
                gExtAdvParams.secondaryAdvPhyOptions = (gapLePhyOptionsFlags_t)BleApp_atoi(argv[i + 1U]);
            }
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */
        }

        gUseShellThrGenericCb = FALSE;
        result = kStatus_SHELL_Success;

    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap periodicdata" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_ChangePeriodicAdvData(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;

    /* For no argument, return usage */
    if (argc > 0U)
    {
        if(0 == strcmp((char*)argv[0], "-erase"))
        {
            gAppExtAdvertisingData.cNumAdStructures = 0U;
            if (NULL != mpPeriodicData)
            {
                (void)MEM_BufferFree(mpPeriodicData);
                mpPeriodicData = NULL;
            }
            /* Clean periodic advertising data */
            shell_write("\r\n\r\n-->  Periodic Advertising Data Erased.");
            result = kStatus_SHELL_Success;
        }
        else if (argc == 2U)
        {
            /* Get advertising type */
            gapAdType_t advType = (gapAdType_t)BleApp_atoi(argv[0]);
            /* Append data to the periodic advertising structure */
            (void)ShellGap_AppendAdvData(&gAppPeriodicAdvData, (gapAdType_t)advType, argv[1], FALSE, FALSE, TRUE);
            /* Set data in GAP */
            (void)Gap_SetPeriodicAdvertisingData(gExtAdvParams.handle, &gAppPeriodicAdvData, FALSE);
            result = kStatus_SHELL_Success;
        }
        else
        {
            shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
            result = kStatus_SHELL_Error;
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap periodicsync" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_PeriodicSyncCreate(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;

    for (uint32_t i = 0U; i < argc; i += 2U)
    {
        if(0 == strcmp((char*)argv[i], "-peer") && ((i+1U) < argc))
        {
            /* Check that address's length is valid */
            if (gcBleDeviceAddressSize_c == BleApp_ParseHexValue(argv[i+1U]))
            {
                FLib_MemCpyReverseOrder(&mPeriodicSyncPeer.peerAddress, argv[i+1U], sizeof(bleDeviceAddress_t));
                result = kStatus_SHELL_Success;
            }
        }

        if(0 == strcmp((char*)argv[i], "-type") && ((i+1U) < argc))
        {
            mPeriodicSyncPeer.peerAddressType = (bleAddressType_t)BleApp_atoi(argv[i+1U]);
            result = kStatus_SHELL_Success;
        }

        if(0 == strcmp((char*)argv[i], "-sid") && ((i+1U) < argc))
        {
            mPeriodicSyncPeer.SID = (uint8_t)BleApp_atoi(argv[i+1U]);
            result = kStatus_SHELL_Success;
        }
    }

    if (result == kStatus_SHELL_Success)
    {
        if (gBleSuccess_c != Gap_PeriodicAdvCreateSync(&mPeriodicSyncPeer))
        {
            shell_write(mShellErrorStatus);
        }
    }

    return result;
}

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
/*! *********************************************************************************
 * \brief        Handles "gap periodicsyncsubevent" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetPeriodicSyncSubevent(uint8_t argc, char * argv[])
{
    static uint16_t     perAdvProperties = 0U;
    static uint8_t      numSubevents = 1U;
    static uint8_t      aSubevents[SHELL_PER_ADV_MAX_NUM_SUBEVENTS] = {0};
    shell_status_t      result = kStatus_SHELL_Success;

    /* first gather shell data, struct is allocated dynamically as it depends on numSubevents*/
    for (uint32_t i = 0U; i < argc; i += 2U)
    {
        if ((0 == strcmp((char*)argv[i], "-peradvproperties")) && ((i + 1U) < argc) &&
           (result == kStatus_SHELL_Success))
        {
            perAdvProperties = (uint16_t)BleApp_atoi(argv[i + 1U]);
        }

        if ((0 == strcmp((char*)argv[i], "-numsubevents")) && ((i + 1U) < argc) &&
           (result == kStatus_SHELL_Success))
        {
            if (SHELL_PER_ADV_MAX_NUM_SUBEVENTS >= (uint8_t)BleApp_atoi(argv[i + 1U]))
            {
                numSubevents = (uint8_t)BleApp_atoi(argv[i + 1U]);
            }
            else
            {
                result = kStatus_SHELL_Error;
            }
        }

        if ((0 == strcmp((char*)argv[i], "-subevents")) && ((i + 1U) < argc) &&
           (result == kStatus_SHELL_Success))
        {
            for (uint32_t j = 0U; j < numSubevents; j++)
            {
                /* argv[i + 1U] == 0x00,0x01,0x02;
                 * argv[i + 1U] + j * 5 == 0x01 (j == 1U) */
                aSubevents[j] = (uint8_t)BleApp_AsciiToHex(argv[i + 1U] + j * 5U, 4U);
            }
        }
    }

    /* allocate periodic subevent sync structure */
    if (NULL == mpPeriodicSubeventSync)
    {
        mpPeriodicSubeventSync = MEM_BufferAlloc((sizeof(mpPeriodicSubeventSync) * 1U) + (sizeof(uint8_t) * numSubevents));

        if (NULL == mpPeriodicSubeventSync)
        {
            result = kStatus_SHELL_Error;
        }
    }
    else if (mpPeriodicSubeventSync->numSubevents != numSubevents)
    {
        (void)MEM_BufferFree(mpPeriodicSubeventSync);

        mpPeriodicSubeventSync = MEM_BufferAlloc((sizeof(mpPeriodicSubeventSync) * 1U) + (sizeof(uint8_t) * numSubevents));

        if (NULL == mpPeriodicSubeventSync)
        {
            result = kStatus_SHELL_Error;
        }
    }
    else
    {
        /* Comply with MISRA_C_2012_Rule_15.7 */
    }

    if ((mpPeriodicSubeventSync != NULL) && (result == kStatus_SHELL_Success))
    {
        mpPeriodicSubeventSync->perAdvProperties = perAdvProperties;
        mpPeriodicSubeventSync->numSubevents = numSubevents;
        FLib_MemCpy(mpPeriodicSubeventSync->aSubevents, aSubevents, sizeof(uint8_t) * numSubevents);

        if (gBleSuccess_c != Gap_SetPeriodicSyncSubevent(mPeriodicSyncHandle, mpPeriodicSubeventSync))
        {
            shell_write(mShellErrorStatus);
        }
    }

    return result;
}
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */

/*! *********************************************************************************
 * \brief        Handles "gap periodicsyncstop" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_PeriodicSyncStop(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    if (gBleSuccess_c != Gap_PeriodicAdvTerminateSync((uint16_t)mPeriodicSyncPeer.SID))
    {
        shell_write(mShellErrorStatus);
    }
    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap periodiccfg" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetPeriodicAdvParameters(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;

    /* If no arguments were passed, print the current configuration and set it in GAP */
    if (argc == 0U)
    {
        /* print handle */
        shell_write("\r\n-->  Periodic Advertising Parameters:");
        shell_write("\r\n    -->  Periodic Advertising Handle: ");
        shell_writeDec(gPeriodicAdvParams.handle);
        /* print advertising interval */
        shell_write("\r\n    -->  Periodic Advertising Interval: ");
        shell_writeDec((uint32_t)gPeriodicAdvParams.maxInterval * 625U / 1000U);
        shell_write(" ms");

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
        /* Number of subevents. */
        shell_write("\r\n    -->  Number of subevents: ");
        shell_writeDec(gPeriodicAdvParams.numSubevents);

        /* Interval between subevents. */
        shell_write("\r\n    -->  Interval between subevents: ");
        shell_writeDec((uint32_t)gPeriodicAdvParams.subeventInterval * 125U / 100U);
        shell_write(".");
        shell_writeDec((uint32_t)gPeriodicAdvParams.subeventInterval * 125U % 100U);
        shell_write(" ms");

        /* Time between the advertising packet in a subevent and the first response slot. */
        shell_write("\r\n    -->  Time between the advertising packet in a subevent and the first response slot: ");
        shell_writeDec((uint32_t)gPeriodicAdvParams.responseSlotDelay * 125U / 100U);
        shell_write(".");
        shell_writeDec((uint32_t)gPeriodicAdvParams.responseSlotDelay * 125U % 100U);
        shell_write(" ms");

        /* Time between response slots. */
        shell_write("\r\n    -->  Time between response slots: ");
        shell_writeDec((uint32_t)gPeriodicAdvParams.responseSlotSpacing * 125U / 1000U);
        shell_write(".");
        shell_writeDec((uint32_t)gPeriodicAdvParams.responseSlotSpacing * 125U % 1000U);
        shell_write(" ms");

        /* Number of response slots. */
        shell_write("\r\n    -->  Number of response slots: ");
        shell_writeDec(gPeriodicAdvParams.numResponseSlots);
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */

        SHELL_NEWLINE();

        result = kStatus_SHELL_Success;
    }

    /* Search for keywords */
    for (uint32_t i = 0U; i < argc; i += 2U)
    {
        /* Periodic Advertising minimum interval */
        if(0 == strcmp((char*)argv[i], "-mininterval") && ((i+1U) < argc))
        {
            uint16_t interval = (uint16_t)BleApp_atoi(argv[i+1U])*1000U/625U;
            gPeriodicAdvParams.minInterval = interval;
            result = kStatus_SHELL_Success;
        }

        /* Periodic Advertising maximum interval */
        if(0 == strcmp((char*)argv[i], "-maxinterval") && ((i+1U) < argc))
        {
            uint16_t interval = (uint16_t)BleApp_atoi(argv[i+1U])*1000U/625U;
            gPeriodicAdvParams.maxInterval = interval;
            result = kStatus_SHELL_Success;
        }

        /* Periodic Advertising add Tx power in Advertising */
        if(0 == strcmp((char*)argv[i], "-txpower") && ((i+1U) < argc))
        {
            uint8_t txPower = (uint8_t)BleApp_atoi(argv[i+1U]);
            gPeriodicAdvParams.addTxPowerInAdv = (bool_t)txPower;
            result = kStatus_SHELL_Success;
        }

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
        /* Periodic Advertising Number of subevents. */
        if (0 == strcmp((char*)argv[i], "-numsubevents") && ((i + 1U) < argc))
        {
            gPeriodicAdvParams.numSubevents = (uint8_t)BleApp_atoi(argv[i + 1U]);
            result = kStatus_SHELL_Success;
        }

        /* Periodic Advertising Interval between subevents. */
        if (0 == strcmp((char*)argv[i], "-subint") && ((i + 1U) < argc))
        {
            gPeriodicAdvParams.subeventInterval = (uint8_t)BleApp_atoi(argv[i + 1U]);
            result = kStatus_SHELL_Success;
        }

        /* Periodic Advertising Time between the advertising packet in a subevent and the first response slot. */
        if (0 == strcmp((char*)argv[i], "-rspslotdelay") && ((i + 1U) < argc))
        {
            gPeriodicAdvParams.responseSlotDelay = (uint8_t)BleApp_atoi(argv[i + 1U]);
            result = kStatus_SHELL_Success;
        }

        /* Periodic Advertising Time between response slots. */
        if (0 == strcmp((char*)argv[i], "-rspslotspace") && ((i + 1U) < argc))
        {
            gPeriodicAdvParams.responseSlotSpacing = (uint8_t)BleApp_atoi(argv[i + 1U]);
            result = kStatus_SHELL_Success;
        }

        /* Periodic Advertising Number of subevent response slots. */
        if (0 == strcmp((char*)argv[i], "-numrspslot") && ((i + 1U) < argc))
        {
            gPeriodicAdvParams.numResponseSlots = (uint8_t)BleApp_atoi(argv[i + 1U]);
            result = kStatus_SHELL_Success;
        }
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */
    }

    if (result == kStatus_SHELL_Success)
    {
        if (gBleSuccess_c != Gap_SetPeriodicAdvParameters(&gPeriodicAdvParams))
        {
            /* If an error occurred, print error message */
            shell_write(mShellErrorStatus);
        }
        else
        {
            result = kStatus_SHELL_Success;
        }
    }

    return result;
}

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
/*! *********************************************************************************
 * \brief        Clears periodic subevent from the application buffers.
 *
 * \return       bool_t         TRUE - if data was successfully erased, FALSE - otherwise
 ********************************************************************************** */
static bool_t ShellGap_ErasePeriodicSubeventData(void)
{
    uint8_t *pAdvArrayData;
    uint32_t i;

    for (i = 0U; i < gAppPerAdvSubeventData.cNumSubevents; i += 1U)
    {
        if (gAppPerAdvSubeventData.aSubeventDataStructures[i].pAdvertisingData->aAdStructures != NULL)
        {
            /* aData is an array allocated once with the size cNumAdStructures * length of each gapAdStructure_t,
             * aData is only the starting pointer in the array coresponding to aAdStructures[idx] */
            pAdvArrayData = (uint8_t *)gAppPerAdvSubeventData.aSubeventDataStructures[i].pAdvertisingData->aAdStructures[0].aData;
            if (NULL != pAdvArrayData)
            {
                (void)MEM_BufferFree(pAdvArrayData);
                pAdvArrayData = NULL;
            }
            else
            {
                return FALSE;
            }

            if (NULL != gaAdvSubeventDataStruct[i].aAdStructures)
            {
                (void)MEM_BufferFree(gaAdvSubeventDataStruct[i].aAdStructures);
                gaAdvSubeventDataStruct[i].aAdStructures = NULL;
            }
            else
            {
                return FALSE;
            }
        }
    }

    gAppPerAdvSubeventData.cNumSubevents = 0U;

    /* Clean periodic advertising data */
    shell_write("\r\n\r\n-->  Periodic Advertising Subevent Data Erased.\r\n");

    return TRUE;
}

/*! *********************************************************************************
 * \brief        Handles "gap periodicsubeventdata" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetPeriodicAdvSubeventData(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;

    if (argc > 0U)
    {
        if (0 == strcmp((char*)argv[0], "-erase"))
        {
            if (TRUE == ShellGap_ErasePeriodicSubeventData())
            {
                result = kStatus_SHELL_Success;
            }
        }
        else if ((argc % 2U == 0U) && (argc / 2U <= SHELL_EXT_ADV_DATA_MAX_AD_STRUCTURES))
        {
            uint8_t subEvtIdx = gAppPerAdvSubeventData.cNumSubevents;

            if ((subEvtIdx >= SHELL_PER_ADV_MAX_NUM_SUBEVENTS) ||
                (subEvtIdx >= gPeriodicAdvParams.numSubevents))
            {
                /* When the user exceeds the SHELL_PER_ADV_MAX_NUM_SUBEVENTS or
                 * gPeriodicAdvParams.numSubevents, the data is reset to add new data */
                (void)ShellGap_ErasePeriodicSubeventData();
                subEvtIdx = 0U;
            }

            gAppPerAdvSubeventData.cNumSubevents++;

            for (uint32_t i = 0U; i < argc; i += 2U)
            {
                /* Get advertising type */
                gapAdType_t advType = (gapAdType_t)BleApp_atoi(argv[i]);

                /* Append data to the periodic advertising structure */
                 (void)ShellGap_AppendAdvData(&gAppPeriodicAdvData, (gapAdType_t)advType, argv[i + 1U], FALSE, FALSE, TRUE);
            }

            gaAdvSubeventDataStruct[subEvtIdx].cNumAdStructures = gAppPeriodicAdvData.cNumAdStructures;
            gaAdvSubeventDataStruct[subEvtIdx].aAdStructures = (gapAdStructure_t*)MEM_BufferAlloc(gAppPeriodicAdvData.cNumAdStructures *
                                                                                                    sizeof(gapAdStructure_t));
            FLib_MemCpy(gaAdvSubeventDataStruct[subEvtIdx].aAdStructures,
                        gAppPeriodicAdvData.aAdStructures,
                        sizeof(gapAdStructure_t) * gAppPeriodicAdvData.cNumAdStructures);

            /* Set the mpPeriodicData to null so the next command can alloc new memory for different subevent */
            /* mpPeriodicData is the first aAdStructures[0].aData casted to (uint8_t*) */
            gAppPeriodicAdvData.cNumAdStructures = 0U;
            mpPeriodicData = NULL;

            gaSubeventDataStruct[subEvtIdx].subevent = subEvtIdx;
            gaSubeventDataStruct[subEvtIdx].responseSlotStart = mShellGapPerResponseSlotStart_c;
            gaSubeventDataStruct[subEvtIdx].responseSlotCount = gPeriodicAdvParams.numResponseSlots;
            gaSubeventDataStruct[subEvtIdx].pAdvertisingData = &gaAdvSubeventDataStruct[subEvtIdx];

            gAppPerAdvSubeventData.aSubeventDataStructures[subEvtIdx] = gaSubeventDataStruct[subEvtIdx];

            result = kStatus_SHELL_Success;
        }
        else
        {
            shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
            result = kStatus_SHELL_Error;
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Clears periodic response data from the application buffer at idx.
 *
 * \return       bool_t         TRUE - if data was successfully erased, FALSE - otherwise
 ********************************************************************************** */
static bool_t ShellGap_ErasePeriodicResponseData(uint8_t idx)
{
    bool_t status = TRUE;

    if (gaAppPerAdvResponseData[idx].pResponseData != NULL)
    {
        if (gaAppPerAdvResponseData[idx].pResponseData->aAdStructures != NULL)
        {
            if (gaAppPerAdvResponseData[idx].pResponseData->aAdStructures[0].aData != NULL)
            {
                (void)MEM_BufferFree(gaAppPerAdvResponseData[idx].pResponseData->aAdStructures[0].aData);
                gaAppPerAdvResponseData[idx].pResponseData->aAdStructures[0].aData = NULL;
            }
            else
            {
                status = FALSE;
            }

            (void)MEM_BufferFree(gaAppPerAdvResponseData[idx].pResponseData->aAdStructures);
            gaAppPerAdvResponseData[idx].pResponseData->aAdStructures = NULL;
        }
        else
        {
            status = FALSE;
        }

        gaAppPerAdvResponseData[idx].pResponseData->cNumAdStructures = 0U;
    }
    else
    {
        status = FALSE;
    }

    return status;
}

/*! *********************************************************************************
 * \brief        Handles "gap periodicresponsedata" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetPeriodicAdvResponseData(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;
    uint8_t subEvtIdx = 0xFF;

    /* For no argument, return usage */
    if (argc > 0U)
    {
        /* Search for keywords */
        for (uint32_t i = 0U; i < argc; i += 2U)
        {
            /* Used to identify the subevent of the PAwR train */
            if (0 == strcmp((char*)argv[i], "-responsesubevent") && ((i + 1U) < argc))
            {
                subEvtIdx = (uint8_t)BleApp_atoi(argv[i + 1U]);
                gaAppPerAdvResponseData[subEvtIdx].responseSubevent = subEvtIdx;
                result = kStatus_SHELL_Success;
            }
            /* Used to identify the response slot of the PAwR train */
            else if (0 == strcmp((char*)argv[i], "-responseslot") && ((i + 1U) < argc) && subEvtIdx < SHELL_PER_ADV_MAX_NUM_SUBEVENTS)
            {
                gaAppPerAdvResponseData[subEvtIdx].responseSlot = (uint8_t)BleApp_atoi(argv[i + 1U]);
                result = kStatus_SHELL_Success;
            }
            /* Response data */
            else if (0 == strcmp((char*)argv[i], "-data") && ((i + 2U) < argc) && subEvtIdx < SHELL_PER_ADV_MAX_NUM_SUBEVENTS)
            {
                uint32_t j = 0U;

                for (j = i + 1U; j < argc; j += 2U)
                {
                    /* Get advertising type */
                    gapAdType_t advType = (gapAdType_t)BleApp_atoi(argv[j]);

                    /* Append data to the periodic advertising structure */
                     (void)ShellGap_AppendAdvData(&gAppPeriodicAdvData, (gapAdType_t)advType, argv[j + 1U], FALSE, FALSE, TRUE);
                }

                /* If data is present here from other command, free and alloc new size required */
                (void)ShellGap_ErasePeriodicResponseData(subEvtIdx);

                /* alloc once for the lifetime of the app */
                if (gaAppPerAdvResponseData[subEvtIdx].pResponseData == NULL)
                {
                    gaAppPerAdvResponseData[subEvtIdx].pResponseData = (gapAdvertisingData_t*)MEM_BufferAlloc(sizeof(gapAdvertisingData_t));
                }

                if (gaAppPerAdvResponseData[subEvtIdx].pResponseData != NULL)
                {
                    gaAppPerAdvResponseData[subEvtIdx].pResponseData->cNumAdStructures = gAppPeriodicAdvData.cNumAdStructures;
                    gaAppPerAdvResponseData[subEvtIdx].pResponseData->aAdStructures = (gapAdStructure_t*)MEM_BufferAlloc(gAppPeriodicAdvData.cNumAdStructures *
                                                                                                                         sizeof(gapAdStructure_t));
                    if (gaAppPerAdvResponseData[subEvtIdx].pResponseData->aAdStructures != NULL)
                    {
                        FLib_MemCpy(gaAppPerAdvResponseData[subEvtIdx].pResponseData->aAdStructures,
                                    gAppPeriodicAdvData.aAdStructures,
                                    sizeof(gapAdStructure_t) * gAppPeriodicAdvData.cNumAdStructures);
                    }

                    /* Set the mpPeriodicData to null so the next command can alloc new memory for different subevent */
                    /* mpPeriodicData is the first aAdStructures[0].aData casted to (uint8_t*) */
                    gAppPeriodicAdvData.cNumAdStructures = 0U;
                    mpPeriodicData = NULL;
                }

                /* The `-data` parameter must be the last. It contains a volatile number of advertising data structures. */
                break;
            }
            else if (0 == strcmp((char*)argv[0], "-erase"))
            {
                uint8_t j = 0U;

                for (j = 0U; j < SHELL_PER_ADV_MAX_NUM_SUBEVENTS; j++)
                {
                    (void)ShellGap_ErasePeriodicResponseData(j);
                }

                /* Clean periodic advertising data */
                shell_write("\r\n\r\n-->  Periodic Advertising Data Erased.\r\n\r\n");
                result = kStatus_SHELL_Success;
            }
            else
            {
                result = kStatus_SHELL_Error;
            }
        }
    }

    return result;
}
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */

/*! *********************************************************************************
 * \brief        Handles "gap periodicstop" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_StopPeriodicAdv(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    if (gBleSuccess_c != Gap_StopPeriodicAdvertising(gExtAdvParams.handle))
    {
        shell_write(mShellErrorStatus);
    }
    else
    {
        result = kStatus_SHELL_Success;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap periodicstart" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_StartPeriodicAdv(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    mIsCentral = FALSE;

    if (gBleSuccess_c != Gap_StartPeriodicAdvertising(gExtAdvParams.handle, FALSE))
    {
        shell_write(mShellErrorStatus);
    }
    else
    {
        if (mAdvertisingCbSet == FALSE)
        {
            shell_write("\r\n-->  Periodic advertising will start after enabling the advertising set!");
            result = kStatus_SHELL_OpenWriteHandleFailed;
        }
        else
        {
            result = kStatus_SHELL_Success;
        }
    }

    return result;
}
#endif /* BLE_SHELL_AE_SUPPORT */

#if defined(BLE_SHELL_DBAF_SUPPORT) && (BLE_SHELL_DBAF_SUPPORT)
/*! *********************************************************************************
 * \brief        Handles "gap setdecinstr" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetDecisionInstructions(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    /* Do not accept command if other arguments were passed */
    if (argc != 0U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else
    {
        /* Set the current decision instructions */
        if (gBleSuccess_c != Gap_SetDecisionInstructions(gNumDecisionInstructions, gaDecisionInstructions))
        {
            shell_write(mShellErrorStatus);
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap adddecinstr" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_AddDecisionInstruction(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;
    bool_t bValidCmd = FALSE;

    /* Do not accept command if no arguments were passed */
    if (argc == 0U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else if (gNumDecisionInstructions == gMaxNumDecisionInstructions_c)
    {
        shell_write("\r\nDecision Instructions limit reached.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else
    {
        /* Search for keywords */
        for (uint32_t i = 0U; i < argc; i += 2U)
        {
            if(0 == strcmp((char*)argv[i], "-group") && ((i+1U) < argc))
            {
                gaDecisionInstructions[gNumDecisionInstructions].testGroup = (gapDecisionInstructionsTestGroup_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-criteria") && ((i+1U) < argc))
            {
                gaDecisionInstructions[gNumDecisionInstructions].passCriteria = (gapDecisionInstructionsTestPassCriteria_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-field") && ((i+1U) < argc))
            {
                gaDecisionInstructions[gNumDecisionInstructions].relevantField = (gapDecisionInstructionsRelevantField_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }

            /* testParameters */
            if(0 == strcmp((char*)argv[i], "-restagkey") && ((i+1U) < argc))
            {
                uint8_t paramSize = BleApp_ParseHexValue(argv[i+1U]);
                paramSize = paramSize > gcDecisionDataKeySize_c ? gcDecisionDataKeySize_c : paramSize;

                FLib_MemCpy(&gaDecisionInstructions[gNumDecisionInstructions].testParameters.resolvableTagKey,
                            argv[i+1U],
                            (sizeof(uint8_t) * paramSize));
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-arbmask") && ((i+1U) < argc))
            {
                uint8_t paramSize = BleApp_ParseHexValue(argv[i+1U]);
                paramSize = paramSize > gcDecisionInstructionsArbitraryDataMaskSize_c ? gcDecisionInstructionsArbitraryDataMaskSize_c : paramSize;

                FLib_MemCpy(&gaDecisionInstructions[gNumDecisionInstructions].testParameters.arbitraryData.mask,
                            argv[i+1U],
                            (sizeof(uint8_t) * paramSize));
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-arbtarget") && ((i+1U) < argc))
            {
                uint8_t paramSize = BleApp_ParseHexValue(argv[i+1U]);
                paramSize = paramSize > gcDecisionInstructionsArbitraryDataTargetSize_c ? gcDecisionInstructionsArbitraryDataTargetSize_c : paramSize;

                FLib_MemCpy(&gaDecisionInstructions[gNumDecisionInstructions].testParameters.arbitraryData.target,
                            argv[i+1U],
                            (sizeof(uint8_t) * paramSize));
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-rssimin") && ((i+1U) < argc))
            {
                gaDecisionInstructions[gNumDecisionInstructions].testParameters.rssi.min = (int8_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-rssimax") && ((i+1U) < argc))
            {
                gaDecisionInstructions[gNumDecisionInstructions].testParameters.rssi.max = (int8_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-lossmin") && ((i+1U) < argc))
            {
                gaDecisionInstructions[gNumDecisionInstructions].testParameters.pathLoss.min = (uint8_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-lossmax") && ((i+1U) < argc))
            {
                gaDecisionInstructions[gNumDecisionInstructions].testParameters.pathLoss.max = (uint8_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-advacheck") && ((i+1U) < argc))
            {
                gaDecisionInstructions[gNumDecisionInstructions].testParameters.advA.check = (gapDecisionInstructionsAdvAChecks_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-add1type") && ((i+1U) < argc))
            {
                gaDecisionInstructions[gNumDecisionInstructions].testParameters.advA.address1Type = (bleAddressType_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-add1") && ((i+1U) < argc))
            {
                uint8_t paramSize = BleApp_ParseHexValue(argv[i+1U]);
                paramSize = paramSize > gcBleDeviceAddressSize_c ? gcBleDeviceAddressSize_c : paramSize;

                FLib_MemCpy(&gaDecisionInstructions[gNumDecisionInstructions].testParameters.advA.address1,
                            argv[i+1U],
                            (sizeof(uint8_t) * paramSize));
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-add2type") && ((i+1U) < argc))
            {
                gaDecisionInstructions[gNumDecisionInstructions].testParameters.advA.address2Type = (bleAddressType_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-add2") && ((i+1U) < argc))
            {
                uint8_t paramSize = BleApp_ParseHexValue(argv[i+1U]);
                paramSize = paramSize > gcBleDeviceAddressSize_c ? gcBleDeviceAddressSize_c : paramSize;

                FLib_MemCpy(&gaDecisionInstructions[gNumDecisionInstructions].testParameters.advA.address2,
                            argv[i+1U],
                            (sizeof(uint8_t) * paramSize));
                bValidCmd = TRUE;
            }

            if(0 == strcmp((char*)argv[i], "-advmode") && ((i+1U) < argc))
            {
                gaDecisionInstructions[gNumDecisionInstructions].testParameters.advMode = (gapDecisionInstructionsAdvMode_t)BleApp_atoi(argv[i+1U]);
                bValidCmd = TRUE;
            }
        }
        /* A valid command should set at least one scanning parameter */
        if (!bValidCmd)
        {
            shell_write("\r\nNot a valid command.  Enter \"help\" to view a list of available commands.\r\n\r\n");
            result = kStatus_SHELL_Error;
        }
        else
        {
            /* Increment the number of existing decision instructions */
            gNumDecisionInstructions++;
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap deldecinstr" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_DeleteDecisionInstructions(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    /* Do not accept command if other arguments were passed */
    if (argc != 0U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else
    {
        FLib_MemSet(&gaDecisionInstructions, 0U, gNumDecisionInstructions * sizeof(gapDecisionInstructionsData_t));
        gNumDecisionInstructions = 0U;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap extadvdecdata" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetExtAdvDecisionData(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;
    bool_t bValidCmd = FALSE;

    /* Do not accept command if no arguments were passed */
    if (argc == 0U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }

    /* Search for keywords */
    for (uint32_t i = 0U; i < argc; i += 2U)
    {
        if(0 == strcmp((char*)argv[i], "-key") && ((i+1U) < argc))
        {
            uint8_t paramSize = BleApp_ParseHexValue(argv[i+1U]);

            if (paramSize == 1U && BleApp_atoi(argv[i+1U]) == 0U)
            {
                /* Set key parameter to NULL */
                mAppExtAdvParams.pGapDecisionData->pKey = NULL;
            }
            else
            {
                paramSize = paramSize > gcDecisionDataKeySize_c ? gcDecisionDataKeySize_c : paramSize;

                /* Set key parameter to gaDecisionKey */
                mAppExtAdvParams.pGapDecisionData->pKey = gaDecisionKey;
                FLib_MemCpy(&gaDecisionKey, argv[i+1U], (sizeof(uint8_t) * paramSize));
            }

            bValidCmd = TRUE;
        }

        if(0 == strcmp((char*)argv[i], "-prand") && ((i+1U) < argc))
        {
            uint8_t paramSize = BleApp_ParseHexValue(argv[i+1U]);

            if (paramSize == 1U && BleApp_atoi(argv[i+1U]) == 0U)
            {
                /* Set prand parameter to NULL */
                mAppExtAdvParams.pGapDecisionData->pPrand = NULL;
            }
            else
            {
                paramSize = paramSize > gcDecisionDataPrandSize_c ? gcDecisionDataPrandSize_c : paramSize;

                /* Set prand parameter to gaPrand */
                mAppExtAdvParams.pGapDecisionData->pPrand = gaPrand;
                FLib_MemCpy(&gaPrand, argv[i+1U], (sizeof(uint8_t) * paramSize));
            }

            bValidCmd = TRUE;
        }

        if(0 == strcmp((char*)argv[i], "-decdata") && ((i+1U) < argc))
        {
            uint8_t paramSize = BleApp_ParseHexValue(argv[i+1U]);

            if (paramSize == 1U && BleApp_atoi(argv[i+1U]) == 0U)
            {
                /* Set decisionData parameter to NULL */
                mAppExtAdvParams.pGapDecisionData->pDecisionData = NULL;
            }
            else
            {
                paramSize = paramSize > gcDecisionDataMaxSize_c ? gcDecisionDataMaxSize_c : paramSize;

                /* Set decisionData parameter to gaDecisionData */
                mAppExtAdvParams.pGapDecisionData->pDecisionData = gaDecisionData;
                FLib_MemCpy(&gaDecisionData, argv[i+1U], (sizeof(uint8_t) * paramSize));
            }

            bValidCmd = TRUE;
        }

        if(0 == strcmp((char*)argv[i], "-datalen") && ((i+1U) < argc))
        {
            mAppExtAdvParams.pGapDecisionData->dataLength = (uint8_t)BleApp_atoi(argv[i+1U]);
            bValidCmd = TRUE;
        }

        if(0 == strcmp((char*)argv[i], "-restag") && ((i+1U) < argc))
        {
            mAppExtAdvParams.pGapDecisionData->resolvableTagPresent = (bool_t)BleApp_atoi(argv[i+1U]);
            bValidCmd = TRUE;
        }
    }

    /* A valid command should set at least one scanning parameter */
    if (!bValidCmd)
    {
        shell_write("\r\nNot a valid command.  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }

    return result;
}
#endif /* BLE_SHELL_DBAF_SUPPORT */

/*! *********************************************************************************
 * \brief        Handles "gap phy" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetPhy(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;
    uint8_t txPhys = 0xFFU;
    uint8_t rxPhys = 0xFFU;
    uint8_t allPhys = 0U;
    gapLePhyOptionsFlags_t phyOptions = gLeCodingNoPreference_c;
    deviceId_t peerId;

    peerId = (deviceId_t)BleApp_atoi(argv[0]);

    if (IS_CONNECTED(peerId) == 0U)
    {
        /* Command is valid only when in a connection */
        shell_write("\r\n-->  Please connect the node first...");
        result =  kStatus_SHELL_Error;
    }
    else
    {
        /* Process argument's value */
        for (uint32_t i = 1U; i < argc; i += 2U)
        {
            if(0 == strcmp((char*)argv[i], "-rx") && ((i+1U) < argc))
            {
                rxPhys = (uint8_t)BleApp_atoi(argv[i+1U]);
                result = kStatus_SHELL_Success;
            }

            if(0 == strcmp((char*)argv[i], "-tx") && ((i+1U) < argc))
            {
                txPhys = (uint8_t)BleApp_atoi(argv[i+1U]);
                result = kStatus_SHELL_Success;
            }

            if(0 == strcmp((char*)argv[i], "-o") && ((i+1U) < argc))
            {
                phyOptions = (gapLePhyOptionsFlags_t)BleApp_atoi(argv[i+1U]);
                result = kStatus_SHELL_Success;
            }
        }

        /* If no value was set for rx/tx phys, update allPhy with NoPreference */
        if (txPhys == 0xFFU)
        {
            allPhys |= (uint8_t)gLeTxPhyNoPreference_c;
        }
        if (rxPhys == 0xFFU)
        {
            allPhys |= (uint8_t)gLeRxPhyNoPreference_c;
        }

        /* Check that parameters are valid */
        if ((rxPhys > ((uint8_t)gLePhy1MFlag_c | (uint8_t)gLePhy2MFlag_c | (uint8_t)gLePhyCodedFlag_c)) ||
            (txPhys > ((uint8_t)gLePhy1MFlag_c | (uint8_t)gLePhy2MFlag_c | (uint8_t)gLePhyCodedFlag_c)) ||
            (phyOptions > gLeCodingS8_c))
        {
            shell_write(mShellErrorStatus);
            result = kStatus_SHELL_Success;
        }
        else
        {
            /* This PHY preference will be used only for the current connection */
            if (gBleSuccess_c != Gap_LeSetPhy(FALSE, peerId, allPhys, txPhys, rxPhys, (uint16_t)phyOptions))
            {
                shell_write(mShellErrorStatus);
                result = kStatus_SHELL_Success;
            }
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap txpower" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetTxPower(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;
    int32_t powerLevel;
    bleTransmitPowerChannelType_t channelType;

    /* The arguments must be "channelType" and "powerLevel". Otherwise return usage */
    if (argc == 2U)
    {
        if (0 == strcmp((char*)argv[0], "adv"))
        {
            channelType = gTxPowerAdvChannel_c;
            result = kStatus_SHELL_Success;
        }
        else if (0 == strcmp((char*)argv[0], "conn"))
        {
            channelType = gTxPowerConnChannel_c;
            result = kStatus_SHELL_Success;
        }
        else
        {
            shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
            result = kStatus_SHELL_Error;
        }

        if (result == kStatus_SHELL_Success)
        {
            powerLevel = BleApp_atoi(argv[1]);

            if ((powerLevel < mShellMinTxPower_c) || (powerLevel > mShellMaxTxPower_c))
            {
                shell_write(mShellErrorStatus);
                result =  kStatus_SHELL_Success;
            }
            else
            {
                if (gBleSuccess_c != Gap_SetTxPowerLevel((uint8_t)powerLevel, channelType))
                {
                    shell_write(mShellErrorStatus);
                }
                else
                {
                    /* Wait for GAP Event gTxPowerLevelSetComplete_c */
                    result = kStatus_SHELL_Success;
                }
            }
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap rssimonitor" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_RSSIMonitor(uint8_t argc, char * argv[])
{
    shell_status_t result;
    deviceId_t peerId = gInvalidDeviceId_c;

    /* If RSSI monitor is already started return shell prompt and do nothing */
    if (mRSSIMonitor == FALSE)
    {
        /* The argument can be the peer's address or the connected peer ID */
        if (strlen(argv[0]) == gcBleDeviceAddressSize_c*2U)
        {
            (void)BleApp_ParseHexValue(argv[0]);
        }
        else
        {
            peerId = (deviceId_t)BleApp_atoi(argv[0]);
        }
        /* If not connected, monitor the advertising RSSI of a given peer */
        if (peerId == gInvalidDeviceId_c)
        {
            mRSSIMonitor = TRUE;
            FLib_MemCpy(&mMonitorPeer, argv[0], sizeof(bleDeviceAddress_t));
            shell_write("\r\n-->  Reading RSSI on advertising channel:\r\n");
            (void)Gap_StartScanning(&gAppScanParams, ShellGap_ScanningCallback, gGapDuplicateFilteringDisable_c, mScanDuration, mScanPeriod);
            result = kStatus_SHELL_Success;
        }
        else
        {
            shell_write("\r\n-->  Reading RSSI from connected device:\r\n");

            /* If device is in a connection, return the instant value of the RSSI */
            (void)Gap_ReadRssi(peerId);
            result = kStatus_SHELL_Success;

            /* Start monitoring the RSSI of the connection periodically */
            if((argc != 1U) && (0 == strcmp((char*)argv[1], "-c")))
            {
                /* Read continuously the value of the RSSI until stopped by user */
                if (kStatus_TimerSuccess == TM_Open((timer_handle_t)mBleShellGapTimerId))
                {
                    /* The reading interval is a multiple of connection interval */
                    uint32_t timeout = (uint32_t)mConnectionParams.connInterval * 100U/8U;
                    if(timeout == 0U)
                    {
                        timeout = mShellGapNoConnRssiTimeout_c;
                    }
                    mRSSIMonitor = TRUE;
                    (void)TM_InstallCallback((timer_handle_t)mBleShellGapTimerId, ShellGap_ReadRssiTmrCb, (uint8_t*)((uint32_t)peerId));
                    (void)TM_Start((timer_handle_t)mBleShellGapTimerId, (uint8_t)kTimerModeIntervalTimer, timeout);
                }
                else
                {
                    shell_write("\r\n-->  No timer available!\r\n");
                }
            }
        }
    }
    else
    {
        result = kStatus_SHELL_Success;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles timer callback.
 *
 * \param[in]    void
 *
 * \return       none
 ********************************************************************************** */
static void ShellGap_ReadRssiTmrCb (void *param)
{
    /* post message to application task */
    (void)App_PostCallbackMessage(ShellGap_ReadRssi, param);
}

/*! *********************************************************************************
 * \brief        Triggers a new reading of RSSI.
 *
 * \param[in]    void
 *
 * \return       none
 ********************************************************************************** */
static void ShellGap_ReadRssi (void *param)
{
    (void)Gap_ReadRssi(*((deviceId_t*)param));
}

/*! *********************************************************************************
 * \brief        Handles the "gap rssistop" shell interrupt command and stops RSSI monitor.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_RSSIMonitorStop(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;
    
    if (argc != 0U)
    {
      shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
      result = kStatus_SHELL_Error;
    }
    else
    {
      mRSSIMonitor = FALSE;
      if (mScanningOn)
      {
        (void)Gap_StopScanning();
        /* Wait for stop scanning GAP event */
        result = kStatus_SHELL_Success;
      }

      /* Stop timer, if started */
      if (kStatus_TimerSuccess != TM_Close((timer_handle_t)mBleShellGapTimerId))
      {
        shell_write("ERROR: Invalid timer ID\r\n");
      }
    }

    return result;

}
/*! *********************************************************************************
 * \brief        Handles BLE generic callback.
 *
 * \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
 ********************************************************************************** */
void ShellGap_GenericCallback (gapGenericEvent_t* pGenericEvent)
{
    /* Handles generic GAP event */
    bool_t bEventProcessed = TRUE;

    switch(pGenericEvent->eventType)
    {
        case gPublicAddressRead_c:
        {
            /* Confirm for shell command: "gap address"  */
            shell_write(mGapEventHeader);
            shell_write("Public Address Read:");

            for(uint32_t i = sizeof(bleDeviceAddress_t); i > 0U; i-- )
            {
                shell_writeHex(pGenericEvent->eventData.aAddress[i - 1U]);
            }
            break;
        }

        case gRandomAddressSet_c:
        {
            /* Confirm for shell command: "gap address [address]"  */
            shell_write(mGapEventHeader);
            shell_write("Random Address Set!");
            break;
        }
        case gExtAdvertisingDataSetupComplete_c:
        case gAdvertisingDataSetupComplete_c:
        case gExtAdvertisingParametersSetupComplete_c:
        case gAdvertisingParametersSetupComplete_c:
        case gPeriodicAdvParamSetupComplete_c:
        case gPeriodicAdvDataSetupComplete_c:
        case gPeriodicAdvCreateSyncCancelled_c:
        case gAdvertisingSetupFailed_c:
        case gPeriodicAdvListUpdateComplete_c:
        case gPeriodicAdvertisingStateChanged_c:
#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
        case gPeriodicAdvSetSubeventDataComplete_c:
        case gPeriodicAdvSetResponseDataComplete_c:
        case gPeriodicSyncSubeventComplete_c:
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */
        {
            ShellGap_GenericCallbackAdv(pGenericEvent);
            break;
        }

#if defined(BLE_SHELL_DBAF_SUPPORT) && (BLE_SHELL_DBAF_SUPPORT)
        case gDecisionInstructionsSetupComplete_c:
        {
            /* Confirm for shell command: "gap setdecinstr" */
            shell_write(mGapEventHeader);
            shell_write("Decision Instructions Setup Complete.");
            break;
        }

        case gExtAdvertisingDecisionDataSetupComplete_c:
        {
            /* Confirm for shell command: "gap extadvdecdata" */
            shell_write(mGapEventHeader);
            shell_write("Extended Advertising Decision Data Setup Complete.");
            break;
        }
#endif /* BLE_SHELL_DBAF_SUPPORT */

        case gLePhyEvent_c:
        {
            /* Phy Mode of a connection has been updated by the Controller */
            if (pGenericEvent->eventData.phyEvent.phyEventType == gPhyUpdateComplete_c)
            {
                shell_write(mGapEventHeader);
                shell_write("Phy update complete with peer ");
                shell_writeDec(pGenericEvent->eventData.phyEvent.deviceId);
                shell_write("\r\n-->  TxPhy: ");
                shell_write(mShellPhy[pGenericEvent->eventData.phyEvent.txPhy]);
                shell_write("\r\n-->  RxPhy: ");
                shell_write(mShellPhy[pGenericEvent->eventData.phyEvent.rxPhy]);
            }
            break;
        }

        case gTxPowerLevelSetComplete_c:
        {
            /* Event for shell command: "gap txpower" */
            ShellGap_HandleTxPowerLevelSetCompleteEvt(pGenericEvent);
            break;
        }
#if defined(BLE_SHELL_MONADV_SUPPORT) && (BLE_SHELL_MONADV_SUPPORT)
        case gDeviceAddedToMonAdvList_c:
        {
            shell_write(mGapEventHeader);
            shell_write("Device added to Monitored Advertisers List.");
            break;
        }
        case gDeviceRemovedFromMonAdvList_c:
        {
            shell_write(mGapEventHeader);
            shell_write("Device removed from Monitored Advertisers List.");
            break;
        }
        case gMonAdvListCleared_c:
        {
            shell_write(mGapEventHeader);
            shell_write("Monitored Advertisers List cleared.");
            break;
        }
        case gMonAdvEnabled_c:
        {
            shell_write(mGapEventHeader);
            if ((mNewMonAdvState == mLastMonAdvState) && (mNewMonAdvState == TRUE))
            {
                shell_write("Monitoring Advertisers restarted.");
            }
            else if (mNewMonAdvState == TRUE)
            {
                shell_write("Monitoring Advertisers enabled.");
            }
            else
            {
                shell_write("Monitoring Advertisers disabled.");
            }
            mLastMonAdvState = mNewMonAdvState;
            break;
        }
        case gMonAdvListSizeRead_c:
        {
            shell_write(mGapEventHeader);
            shell_write("Monitored Advertisers List size: ");
            shell_writeDec(pGenericEvent->eventData.monAdvListSize);
            break;
        }
#endif /* BLE_SHELL_MONADV_SUPPORT */
#if BLE_SHELL_CONN_SBR_SUPPORT
        case gLeSetDefaultConnectionSubrateParametersSetupComplete_c:
        {
            shell_write(mGapEventHeader);
            shell_write("Default Connection Subrate Parameters Setup Complete.");
            break;
        }
#endif /* BLE_SHELL_CONN_SBR_SUPPORT */
        case gInternalError_c:
        {
            /* Command is not supported */
            if (pGenericEvent->eventData.internalError.errorCode == gHciUnknownHciCommand_c)
            {
                shell_write(mGapEventHeader);
                shell_write("Command not supported!\r\n");
            }
        }
        break;

        default:
            /* Other Generic Event */
            bEventProcessed = FALSE;
        break;
    }

    if (bEventProcessed == TRUE)
    {
        /* Enter new line only if the event was processed */
        SHELL_NEWLINE();
        shell_cmd_finished();
    }
}

/*! *********************************************************************************
 * \brief        Handles BLE Advertising callback from host stack.
 *
 * \param[in]    pAdvertisingEvent    Pointer to gapAdvertisingEvent_t.
 ********************************************************************************** */
void ShellGap_AdvertisingCallback
(
    gapAdvertisingEvent_t* pAdvertisingEvent
)
{
    /* Handles advertising events */
    shell_write("\r\n-->  GAP Event: Advertising ");

    switch (pAdvertisingEvent->eventType)
    {
        case gAdvertisingStateChanged_c:
        case gExtAdvertisingStateChanged_c:
        {
            /* Operation succeeded */
            shell_write("state changed successfully!\r\n");
            break;
        }
        case gAdvertisingCommandFailed_c:
        {
            /* An error occurred */
            shell_write("state could not be changed.\r\n");
            break;
        }

        case gAdvertisingSetTerminated_c:
        {
            shell_write("stopped!\r\n");
            break;
        }

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
        case gPerAdvSubeventDataRequest_c:
        {
            if (gAppPerAdvSubeventData.cNumSubevents != 0U)
            {
                if (gBleSuccess_c != Gap_SetPeriodicAdvSubeventData(gExtAdvParams.handle, &gAppPerAdvSubeventData))
                {
                    shell_write(mShellErrorStatus);
                }
                else
                {
                    shell_write("Request for periodic subevent data - Data Set\r\n");
                }
            }
            else
            {
                shell_write("Request for periodic subevent data - No Data Set\r\n");
            }
            break;
        }

        case gPerAdvResponse_c:
        {
            uint16_t dataLength = pAdvertisingEvent->eventData.perAdvResponse.dataLength;
            uint8_t *pData = (uint8_t*)pAdvertisingEvent->eventData.perAdvResponse.aData;
            uint16_t index = 0U;

            shell_write("periodic advertising response received:\r\n");

            shell_write("\r\n\tAdvertising Handle: ");
            shell_writeDec((uint32_t)pAdvertisingEvent->eventData.perAdvResponse.advHandle);
            shell_write("\r\n\tSubevent: ");
            shell_writeDec((uint32_t)pAdvertisingEvent->eventData.perAdvResponse.subevent);
            shell_write("\r\n\tResponse Slot: ");
            shell_writeDec((uint32_t)pAdvertisingEvent->eventData.perAdvResponse.responseSlot);

            shell_write("\r\n\tAdvertising Data: ");
            /* Print advertising data */
            while (index < dataLength)
            {
                gapAdStructure_t adElement;

                adElement.length = pData[index];
                adElement.adType = (gapAdType_t)pData[index + 1U];
                adElement.aData = &pData[index + 2U];

                if (adElement.adType == gAdManufacturerSpecificData_c)
                {
                    shell_writeN((char const *)adElement.aData, (uint32_t)adElement.length - 1U);
                }
                else
                {
                    for (uint8_t j = 0U; j < adElement.length - 1U; j++)
                    {
                        shell_writeHex(adElement.aData[j]);
                    }
                }

                /* Move on to the next AD element type */
                index += (uint32_t)adElement.length + sizeof(uint8_t);
            }
            break;
        }
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */

    default:
            ; /* Other Advertising Event */
        break;
    }

    shell_cmd_finished();
}

/*! *********************************************************************************
 * \brief        Handles BLE Connection callback from host stack.
 *
 * \param[in]    peerDeviceId        Peer device ID.
 * \param[in]    pConnectionEvent    Pointer to gapConnectionEvent_t.
 ********************************************************************************** */
void ShellGap_ConnectionCallback
(
    deviceId_t peerDeviceId,
    gapConnectionEvent_t* pConnectionEvent
)
{
    switch (pConnectionEvent->eventType)
    {
        case gConnEvtConnected_c:
        {
            ShellGap_HandleConnectedEvt(peerDeviceId, &pConnectionEvent->eventData.connectedEvent);
        }
        break;

        case gConnEvtDisconnected_c:
        {
            /* Reset global deviceId variable */
            CLEAR_CONN(peerDeviceId);
            shell_write("\r\n-->  GAP Event: Disconnected from peer ");
            shell_writeDec(peerDeviceId);
            shell_write(" with reason ");
            shell_writeDec((uint32_t)pConnectionEvent->eventData.disconnectedEvent.reason);
            shell_cmd_finished();
            if (mRSSIMonitor)
            {
                (void)ShellGap_RSSIMonitorStop(0U, NULL);
            }
        }
        break;

        case gConnEvtPairingRequest_c:
        {
            /* Accept the pairing request from a peer and store the pairing parameters */
            (void)Gap_AcceptPairingRequest(peerDeviceId, &gPairingParameters);
        }
        break;

        case gConnEvtKeyExchangeRequest_c:
        {
            ShellGap_SendSmpKeys(peerDeviceId, &pConnectionEvent->eventData.keyExchangeRequestEvent.requestedKeys);
        }
        break;

        case gConnEvtPasskeyRequest_c:
        {
            /* Peer requested a PIN for pairing procedure. Only Central will receive this event */
            shell_write("\r\r\n\n-->  GAP Event: PIN required\r\n");
            shell_cmd_finished();
        }
        break;

        case gConnEvtPasskeyDisplay_c:
        {
            /* Peripheral displays the passcode for the Central's use */
            shell_write("\r\n-->  GAP Event: Passkey is ");
            shell_writeDec(pConnectionEvent->eventData.passkeyForDisplay);
            SHELL_NEWLINE();
            shell_cmd_finished();
        }
        break;

        case gConnEvtLongTermKeyRequest_c:
        {
            ShellGap_HandleLongTermKeyRequestEvt(peerDeviceId, &pConnectionEvent->eventData.longTermKeyRequestEvent);
            break;
        }

        case gConnEvtPairingComplete_c:
        {
            ShellGap_HandlePairingCompleteEvt(peerDeviceId, &pConnectionEvent->eventData.pairingCompleteEvent);
        }
        break;

        case gConnEvtLeScDisplayNumericValue_c:
        {
            (void) pConnectionEvent->eventData.numericValueForDisplay;
            /* Display on a screen for user confirmation then validate/invalidate based on value. */
            (void)Gap_LeScValidateNumericValue(peerDeviceId, TRUE);
        }
        break;

        case gConnEvtEncryptionChanged_c:
        {
            ShellGap_HandleEncryptionChangedEvt(&pConnectionEvent->eventData.encryptionChangedEvent);
        }
        break;

        case gConnEvtParameterUpdateRequest_c:
        {
            (void)Gap_EnableUpdateConnectionParameters(peerDeviceId, TRUE);
        }
        break;

        case gConnEvtParameterUpdateComplete_c:
        {
            ShellGap_HandleParameterUpdateCompleteEvt(&pConnectionEvent->eventData.connectionUpdateComplete);
        }
        break;

        case gConnEvtRssiRead_c:
        {
            ShellGap_HandleRssiReadEvt(pConnectionEvent->eventData.rssi_dBm);
        }
        break;

        case gConnEvtPowerReadFailure_c:
        {
            shell_write("\r\n-->  GAP Event: RSSI could not be read!");
            shell_cmd_finished();
        }
        break;
#if BLE_SHELL_CONN_SBR_SUPPORT
        case gConnEvtLeSubrateChange_c:
        {
            ShellGap_HandleSubrateChangeEvt(peerDeviceId, &pConnectionEvent->eventData.gapSubrateChangeEvent);
        }
        break;
#endif /* BLE_SHELL_CONN_SBR_SUPPORT */
        default:
            ; /* Other Connection Event */
        break;
    }
}

/*! *********************************************************************************
 * \brief        Handles BLE Scanning callback from host stack.
 *
 * \param[in]    pScanningEvent    Pointer to gapScanningEvent_t.
 ********************************************************************************** */
void ShellGap_ScanningCallback (gapScanningEvent_t* pScanningEvent)
{
    uint8_t aux;
	
    /* Handle scanning events */
    switch (pScanningEvent->eventType)
    {
        case gExtDeviceScanned_c:
        case gDeviceScanned_c:
        {
            if (mRSSIMonitor)
            {
                bleDeviceAddress_t peerAddr;
                int8_t rssi;

                if (gExtDeviceScanned_c == pScanningEvent->eventType)
                {
                    FLib_MemCpyReverseOrder(&peerAddr, &pScanningEvent->eventData.extScannedDevice.aAddress, gcBleDeviceAddressSize_c);
                    rssi = pScanningEvent->eventData.extScannedDevice.rssi;
                }
                else
                {
                    FLib_MemCpyReverseOrder(&peerAddr, &pScanningEvent->eventData.scannedDevice.aAddress, gcBleDeviceAddressSize_c);
                    rssi = pScanningEvent->eventData.scannedDevice.rssi;
                }

                /* Check if scanned device is the monitored peer */
                if (FLib_MemCmp(&mMonitorPeer, &peerAddr, sizeof(bleDeviceAddress_t)))
                {
                    shell_write("\r\n RSSI: ");
                    if(((uint8_t)rssi >> 7) != 0U)
                    {
                        /* Negative Value */
                        (void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, "-");
                        aux = ~((uint8_t)rssi - 1U);
                        rssi = (int8_t)aux;
                    }
                    shell_writeDec((uint32_t)rssi);
                    shell_write(" dBm");
                }
            }
            else
            {
                /* Only first mShellGapMaxScannedDevicesCount_c are processed */
                if (mScannedDevicesCount < mShellGapMaxScannedDevicesCount_c)
                {
                    shell_write("\r\n-->  GAP Event: Found device ");
                    shell_writeDec(mScannedDevicesCount);
                    shell_write(" : ");
#if defined(BLE_SHELL_AE_SUPPORT) && (BLE_SHELL_AE_SUPPORT)
                    /* Process advertising structures and print device's name, address and RSSI */
                    if (gExtDeviceScanned_c == pScanningEvent->eventType)
                    {
                        ShellGap_ParseExtScannedDevice(&pScanningEvent->eventData.extScannedDevice);
                    }
                    else
#endif /* BLE_SHELL_AE_SUPPORT */
                    {

                        ShellGap_ParseScannedDevice(&pScanningEvent->eventData.scannedDevice);
                    }
                }
            }
            break;
        }

        case gScanStateChanged_c:
        {
        /* Update scanning state */
            mScanningOn = !mScanningOn;

            shell_write("\r\n->  GAP Event: Scan ");
            if (mScanningOn)
            {
                /* Reset counter and details of scanned devices, when scan is restarted */
                mScannedDevicesCount = 0;
                FLib_MemSet(&mScannedDevices, 0U, mShellGapMaxScannedDevicesCount_c * sizeof(gapScannedDevices_t));
                shell_write("started.\r\n");
            }
            else
            {
                shell_write("stopped.\r\n");
            }
            shell_cmd_finished();
            break;
        }

        case gPeriodicAdvSyncEstablished_c:
            shell_write("\r\n-->  GAP Event: Periodic Advertising Sync Established");
#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
            mPeriodicSyncHandle = pScanningEvent->eventData.syncEstb.syncHandle;
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */
            break;

        case gPeriodicAdvSyncLost_c:
        case gPeriodicAdvSyncTerminated_c:
            shell_write("\r\n-->  GAP Event: Periodic Advertising Sync Lost");
            break;

        case gPeriodicDeviceScanned_c:
            shell_write("\r\n-->  GAP Event: Periodic Device Scanned");
            if (mRSSIMonitor)
            {
                shell_write(" RSSI: ");
                if(((uint8_t)pScanningEvent->eventData.periodicScannedDevice.rssi >> 7) != 0U)
                {
                    /* Negative Value */
                    (void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, "-");
                    aux = ~((uint8_t)pScanningEvent->eventData.periodicScannedDevice.rssi - 1U);
                    pScanningEvent->eventData.periodicScannedDevice.rssi = (int8_t)aux;
                }
                shell_writeDec((uint32_t)pScanningEvent->eventData.periodicScannedDevice.rssi);
                shell_write(" dBm");
            }
            break;

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
        case gPeriodicDeviceScannedV2_c:
        {
            /* Periodic information variables */
            uint16_t eventIdx = pScanningEvent->eventData.periodicScannedDeviceV2.periodicEventCounter;
            uint8_t subeventIdx = pScanningEvent->eventData.periodicScannedDeviceV2.subevent;
            bool_t bResponseSet = FALSE;
            /* Advertising data variables */
            uint16_t dataLength = pScanningEvent->eventData.periodicScannedDeviceV2.dataLength;
            uint8_t *pData = (uint8_t*)pScanningEvent->eventData.periodicScannedDeviceV2.pData;
            uint16_t index = 0U;

            shell_write("\r\n-->  GAP Event: Periodic V2 Device Scanned");

            /* The responder will always set the data for the ongoing event */
            shell_write("\r\n\tEvent Counter: ");
            shell_writeDec((uint32_t)eventIdx);
            gaAppPerAdvResponseData[subeventIdx].requestEvent = eventIdx;

            /* The responder will always set the data for the ongoing subevent */
            shell_write("\r\n\tSubevent Synced: ");
            shell_writeDec((uint32_t)subeventIdx);
            gaAppPerAdvResponseData[subeventIdx].requestSubevent = subeventIdx;

            shell_write("\r\n\tRSSI: ");

            if (((uint8_t)pScanningEvent->eventData.periodicScannedDevice.rssi >> 7) != 0U)
            {
                /* Negative Value */
                shell_write("-");
                aux = ~((uint8_t)pScanningEvent->eventData.periodicScannedDevice.rssi - 1U);
                pScanningEvent->eventData.periodicScannedDevice.rssi = (int8_t)aux;
            }
            shell_writeDec((uint32_t)pScanningEvent->eventData.periodicScannedDevice.rssi);
            shell_write(" dBm");

            shell_write("\r\n\tAdvertising Data: ");

            /* Print advertising data */
            while (index < dataLength)
            {
                gapAdStructure_t adElement;

                adElement.length = pData[index];
                adElement.adType = (gapAdType_t)pData[index + 1U];
                adElement.aData = &pData[index + 2U];

                if (adElement.adType == gAdManufacturerSpecificData_c)
                {
                    shell_writeN((char const *)adElement.aData, (uint32_t)adElement.length - 1U);
                }
                else
                {
                    for (uint8_t j = 0U; j < adElement.length - 1U; j++)
                    {
                        shell_writeHex(adElement.aData[j]);
                    }
                }

                /* Move on to the next AD element type */
                index += (uint32_t)adElement.length + sizeof(uint8_t);
            }

            if (gaAppPerAdvResponseData[subeventIdx].pResponseData != NULL)
            {
                if (gaAppPerAdvResponseData[subeventIdx].pResponseData->cNumAdStructures != 0U)
                {
                    bResponseSet = TRUE;
                }
            }

            if (TRUE == bResponseSet)
            {
                /* Every time scan data from periodic is received, give a response if data is set */
                if (gBleSuccess_c != Gap_SetPeriodicAdvResponseData(mPeriodicSyncHandle, &gaAppPerAdvResponseData[subeventIdx]))
                {
                    shell_write(mShellErrorStatus);
                }
                else
                {
                    shell_write("\r\nResponse Data Set\r\n");
                }
            }

            break;
        }
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */

#if defined(BLE_SHELL_MONADV_SUPPORT) && (BLE_SHELL_MONADV_SUPPORT)
        case gMonAdvReportEventReceived_c:
        {
            shell_write("\r\n-->  GAP Event: Monitored Advertiser Report");
            shell_write("\r\n    Address Type: ");
            shell_writeDec(pScanningEvent->eventData.monAdvReport.peerAddressType);
            shell_write("\r\n    Address: ");
            for(uint32_t i = sizeof(bleDeviceAddress_t); i > 0U; i-- )
            {
                shell_writeHex(pScanningEvent->eventData.monAdvReport.peerAddress[i - 1U]);
            }
            shell_write("\r\n    Condition: ");
            if (pScanningEvent->eventData.monAdvReport.condition == gBleMonAdvConditionRssiLowThreshold_c)
            {
                shell_write("RSSI Low Threshold");
            }
            else if (pScanningEvent->eventData.monAdvReport.condition == gBleMonAdvConditionRssiHighThreshold_c)
            {
                shell_write("RSSI High Threshold");
            }
            else
            {
                shell_write("Unknown");
            }
            shell_write("\r\n");
            break;
        }
#endif /* BLE_SHELL_MONADV_SUPPORT */
        case gScanCommandFailed_c:
        {
            /* Scan could not be started/stopped */
            shell_write("\r\n-->  GAP Event: Scan state could not be changed.");
            shell_cmd_finished();
            break;
        }

        default:
             ; /* Other Scanning Event */
            break;
    }

}

/*! *********************************************************************************
 * \brief        Utility function that prints the LE PHYs parsing a bitmask.
 *
 * \param[in]    physBitmask    PHYs bitmask.
 ********************************************************************************** */
static void ShellGap_PrintPhy(uint8_t physBitmask)
{
    /* gLePhy1MFlag_c */
    if((physBitmask & (uint8_t)gLePhy1MFlag_c) != 0U)
    {
        shell_write(mShellPhy[gLePhy1M_c]);
    }
    /* gLePhy2MFlag_c */
    if((physBitmask & (uint8_t)gLePhy2MFlag_c) != 0U)
    {
        shell_write(mShellPhy[gLePhy2M_c]);
    }
    /* gLePhyCodedFlag_c */
    if((physBitmask & (uint8_t)gLePhyCodedFlag_c) != 0U)
    {
        shell_write(mShellPhy[gLePhyCoded_c]);
    }
}

#if defined(BLE_SHELL_AE_SUPPORT) && (BLE_SHELL_AE_SUPPORT)
/*! *********************************************************************************
 * \brief        Function used to copy the user defined pattern into an AD structure.
 *
 * \param[in]    pStart             Pointer to starting AD structure.
 * \param[in]    sizeLeftToWrite    Size of data to be filled with the pattern
 ********************************************************************************** */
static void ShellGap_CopyGenericExtAdvData
(
    uint8_t* pStart,
    uint32_t sizeLeftToWrite
)
{
    /* Copy the AdvData pattern to buffer */
    uint32_t sizeOfPattern = FLib_StrLen(mShellEADataPattern);
    uint32_t offset = 0U;
    do
    {
        FLib_MemCpy(pStart+offset, mShellEADataPattern, sizeOfPattern);
        sizeLeftToWrite -= sizeOfPattern;
        offset += sizeOfPattern;
    }while (sizeLeftToWrite > sizeOfPattern);
    /* Copy remaining data */
    FLib_MemCpy(pStart+offset, mShellEADataPattern, sizeLeftToWrite);
}
/*! *********************************************************************************
 * \brief        Function used to set large extended advertising data.
 *
 * \param[in]    pExtAdvData    Pointer to large extended advertising data.
 ********************************************************************************** */
static void ShellGap_SetDefaultExtData(gapAdvertisingData_t* pExtAdvData)
{
    uint8_t numOfAdStructures = 2U;
    uint32_t extAdvDataSize = SHELL_EXT_ADV_DATA_SIZE;
    bool_t noMemory = FALSE;

    /* Add Flags adv structure data */
    pExtAdvData->aAdStructures[0] = mExtDefaultAdStruct1;

    /* Add adv strucure data "EXT_ADV_DEMO" */
    pExtAdvData->aAdStructures[1] = mExtDefaultAdStruct2;

    while (extAdvDataSize > gBleMaxADStructureLength_c)
    {
        /*Alloc memory for each AD Structure */
        uint8_t* pADStruct = MEM_BufferAlloc(gBleMaxADStructureLength_c);
        if (pADStruct != NULL)
        {
            ShellGap_CopyGenericExtAdvData(pADStruct, gBleMaxADStructureLength_c);

            /* Add data to AD structure */
            pExtAdvData->aAdStructures[numOfAdStructures].length = gBleMaxADStructureLength_c + 1U;
            pExtAdvData->aAdStructures[numOfAdStructures].adType = gAdCompleteLocalName_c;
            pExtAdvData->aAdStructures[numOfAdStructures].aData = pADStruct;
            numOfAdStructures++;
            extAdvDataSize -= gBleMaxADStructureLength_c;
        }
        else
        {
            /* No memory */
            noMemory = TRUE;
            break;
        }
    }

    /* Add the remaining chunk. If memory allocation failed for previously AD structures, skip this */
    if (noMemory == FALSE)
    {
        uint8_t* pADStruct = MEM_BufferAlloc(extAdvDataSize);
        if (pADStruct != NULL)
        {
            ShellGap_CopyGenericExtAdvData(pADStruct, extAdvDataSize);
            pExtAdvData->aAdStructures[numOfAdStructures].length = (uint8_t)extAdvDataSize + 1U;
            pExtAdvData->aAdStructures[numOfAdStructures].adType = gAdCompleteLocalName_c;
            pExtAdvData->aAdStructures[numOfAdStructures].aData = pADStruct;
            numOfAdStructures++;
        }
    }
    pExtAdvData->cNumAdStructures = numOfAdStructures;
}
#endif /* BLE_SHELL_AE_SUPPORT */

/*! *********************************************************************************
 * \brief        Handles the gConnEvtConnected_c connection event.
 *
 * \param[in]    peerDeviceId   ID of the peer device 
 *
 * \param[in]    pEvent         pointer to the event data   
 *
 * \return       -
 ********************************************************************************** */
static void ShellGap_HandleConnectedEvt(deviceId_t peerDeviceId, gapConnectedEvent_t *pEvent)
{
    /* Store the device ID of the peer connected device */
    SET_NEW_CONN(peerDeviceId);
    shell_write("\r\n-->  GAP Event: Connected to peer ");
    shell_writeDec(peerDeviceId);
    shell_cmd_finished();

    mIsBonded = FALSE;

    (void)Gap_CheckIfBonded(peerDeviceId, &mIsBonded, NULL);

    if ((mIsBonded) &&
    (gBleSuccess_c == Gap_LoadCustomPeerInformation(peerDeviceId, NULL, 0 ,0)))
    {
        /* Restored custom connection information. Encrypt link */
        (void)Gap_EncryptLink(peerDeviceId);
    }

    /* Save connection parameters */
    FLib_MemCpy(&mConnectionParams,
                &pEvent->connParameters,
                sizeof(gapConnectionParameters_t));
}

/*! *********************************************************************************
 * \brief        Handles the gConnEvtKeyExchangeRequest_c connection event.
 *
 * \param[in]    peerDeviceId           ID of the peer device 
 *
 * \param[in]    pRequestedKeys         pointer to the key flags     
 *
 * \return       -
 ********************************************************************************** */
static void ShellGap_SendSmpKeys(deviceId_t peerDeviceId, gapSmpKeyFlags_t *pRequestedKeys)
{
    gapSmpKeys_t sentSmpKeys = gSmpKeys;

    if (0U == ((*pRequestedKeys) & (uint8_t)gLtk_c))
    {
        sentSmpKeys.aLtk = NULL;
        /* When the LTK is NULL EDIV and Rand are not sent and will be ignored. */
    }

    if (0U == ((*pRequestedKeys) & (uint8_t)gIrk_c))
    {
        sentSmpKeys.aIrk = NULL;
        /* When the IRK is NULL the Address and Address Type are not sent and will be ignored. */
    }

    if (0U == ((*pRequestedKeys) & (uint8_t)gCsrk_c))
    {
        sentSmpKeys.aCsrk = NULL;
    }

    (void)Gap_SendSmpKeys(peerDeviceId, &sentSmpKeys);
}

/*! *********************************************************************************
 * \brief        Handles the gConnEvtLongTermKeyRequest_c connection event.
 *
 * \param[in]    peerDeviceId   ID of the peer device 
 *
 * \param[in]    pEvent         pointer to the event data    
 *
 * \return       -
 ********************************************************************************** */
static void ShellGap_HandleLongTermKeyRequestEvt(deviceId_t peerDeviceId, gapLongTermKeyRequestEvent_t *pEvent)
{
    if (pEvent->ediv == gSmpKeys.ediv &&
        pEvent->randSize == gSmpKeys.cRandSize)
    {
        (void)Gap_LoadEncryptionInformation(peerDeviceId, gSmpKeys.aLtk, &gSmpKeys.cLtkSize);
        /* EDIV and RAND both matched */
        (void)Gap_ProvideLongTermKey(peerDeviceId, gSmpKeys.aLtk, gSmpKeys.cLtkSize);
    }
    else
    /* EDIV or RAND size did not match */
    {
        (void)Gap_DenyLongTermKey(peerDeviceId);
    }
}

/*! *********************************************************************************
 * \brief        Handles the gConnEvtPairingComplete_c connection event.
 *
 * \param[in]    peerDeviceId   ID of the peer device 
 *
 * \param[in]    pEvent         pointer to the event data    
 *
 * \return       -
 ********************************************************************************** */
static void ShellGap_HandlePairingCompleteEvt(deviceId_t peerDeviceId, gapPairingCompleteEvent_t *pEvent)
{
    if (pEvent->pairingSuccessful)
    {
        shell_write("\r\n-->  GAP Event: Device Paired.\r\n");

        /* Save Device Name in NVM*/
        (void)Gap_SaveDeviceName(peerDeviceId, mScannedDevices[mConnectToDeviceId].name, (uint8_t)strlen((char*)mScannedDevices[mConnectToDeviceId].name));
    }
    else
    {
        shell_write("\r\n-->  GAP Event: Pairing Unsuccessful.\r\n");
    }
    shell_cmd_finished();
}

/*! *********************************************************************************
 * \brief        Handles the gConnEvtEncryptionChanged_c connection event.
 *
 * \param[in]    pEvent   pointer to the event data    
 *
 * \return       -
 ********************************************************************************** */
static void ShellGap_HandleEncryptionChangedEvt(gapEncryptionChangedEvent_t *pEvent)
{
    /* Link's encryption state has changed */
    if (pEvent->newEncryptionState)
    {
        shell_write("\r\n-->  GAP Event: Link Encrypted.\r\n");
    }
    else
    {
        shell_write("\r\n-->  GAP Event: Link Not Encrypted.\r\n");
    }
    shell_cmd_finished();
}

/*! *********************************************************************************
 * \brief        Handles the gConnEvtParameterUpdateComplete_c connection event.
 *
 * \param[in]    pEvent   pointer to the event data    
 *
 * \return       -
 ********************************************************************************** */
static void ShellGap_HandleParameterUpdateCompleteEvt(gapConnParamsUpdateComplete_t *pEvent)
{
    if (pEvent->status == gBleSuccess_c)
    {
        /* Store the updated connection parameters */
        mConnectionParams.connInterval = pEvent->connInterval;
        mConnectionParams.connLatency = pEvent->connLatency;
        mConnectionParams.supervisionTimeout = pEvent->supervisionTimeout;

        shell_write("\r\n-->  GAP Event: Connection Parameters Changed.");
        shell_write("\r\n   -->Connection Interval: ");
        /* Convert the value to milliseconds */
        shell_writeDec((uint32_t)mConnectionParams.connInterval * 10U/8U);
        shell_write(" ms");
        shell_write("\r\n   -->Connection Latency: ");
        shell_writeDec(mConnectionParams.connLatency);
        shell_write("\r\n   -->Supervision Timeout: ");
        /* Convert the value to milliseconds */
        shell_writeDec((uint32_t)mConnectionParams.supervisionTimeout * 10U);
        shell_write(" ms");
        shell_cmd_finished();
    }
    else
    {
        shell_write("\r\n-->  GAP Event: Connection Parameters Did Not Change.");
    }
}

/*! *********************************************************************************
 * \brief        Handles the gConnEvtRssiRead_c connection event.
 *
 * \param[in]    rssi_dBm   Value of the RSSI in dBm    
 *
 * \return       -
 ********************************************************************************** */
static void ShellGap_HandleRssiReadEvt(int8_t rssi_dBm)
{
    uint8_t aux;
    shell_write("\r\n RSSI: ");
    if(((uint8_t)rssi_dBm >> 7) != 0U)
    {
        /* Negative Value */
        (void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, "-");
        aux = ~((uint8_t)rssi_dBm - 1U);
        rssi_dBm = (int8_t)aux;
    }
    shell_writeDec((uint32_t)rssi_dBm);
    shell_write(" dBm");
    if (mRSSIMonitor == FALSE)
    {
        /* Resume shell only if reading an instant value */
        shell_cmd_finished();
    }
}

/*! *********************************************************************************
 * \brief        Handles the gTxPowerLevelSetComplete_c event.
 *
 * \param[in]    pGenericEvent   pointer to the event data   
 *
 * \return       -
 ********************************************************************************** */
static void ShellGap_HandleTxPowerLevelSetCompleteEvt(gapGenericEvent_t* pGenericEvent){
  
    shell_write(mGapEventHeader);
    if (pGenericEvent->eventData.txPowerLevelSetStatus == gBleSuccess_c)
    {
        shell_write("Success!");
    }
    else
    {
        shell_write("Failed!");
    }

}

#if defined(BLE_SHELL_AE_SUPPORT) && (BLE_SHELL_AE_SUPPORT)
/*! *********************************************************************************
 * \brief        Helper for ShellGap_SetExtAdvertisingParameters. It prints the
 *               current extended advertising configuration.
 *
 * \param[in]    none
 *
 * \return       -
 ********************************************************************************** */
static void ShellGap_PrintExtAdvParametersCfg(void)
{
    /* Advertising maximum interval */
    shell_write("\r\n-->  Extended Advertising Parameters:");
    shell_write("\r\n    -->  Advertising Handle: ");
    shell_writeDec(gExtAdvParams.handle);
    shell_write("\r\n    -->  Advertising SID: ");
    shell_writeDec(gExtAdvParams.SID);
    shell_write("\r\n    -->  Advertising Interval: ");
    /* Convert to ms */
    shell_writeDec(gExtAdvParams.maxInterval * 625U / 1000U);
    shell_write(" ms");
    /* Print Advertising properties corresponding to the bits set in extAdvProperties */
    shell_write("\r\n    -->  Advertising Properties: ");

    if (0U != ((uint16_t)gExtAdvParams.extAdvProperties & (uint16_t)gAdvReqConnectable_c))
    {
        shell_write("connectable ");
    }
    if (0U != ((uint16_t)gExtAdvParams.extAdvProperties & (uint16_t)gAdvReqScannable_c))
    {
        shell_write("scannable ");
    }
    if (0U != ((uint16_t)gExtAdvParams.extAdvProperties & (uint16_t)gAdvReqDirected_c))
    {
        shell_write("directed ");
    }
    if (0U != ((uint16_t)gExtAdvParams.extAdvProperties & (uint16_t)gAdvReqHighDutyCycle_c))
    {
        shell_write("high duty cycle ");
    }
    if (0U != ((uint16_t)gExtAdvParams.extAdvProperties & (uint16_t)gAdvReqLegacy_c))
    {
        shell_write("legacy ");
    }
    if (0U != ((uint16_t)gExtAdvParams.extAdvProperties & (uint16_t)gAdvReqAnonymous_c))
    {
        shell_write("anonymous ");
    }
    if (0U != ((uint16_t)gExtAdvParams.extAdvProperties & (uint16_t)gAdvIncludeTxPower_c))
    {
        shell_write("Tx power included ");
    }

    /* Primary PHY */
    shell_write("\r\n    -->  Primary PHY: ");
    shell_write(mShellPhy[gExtAdvParams.primaryPHY]);
    /* Secondary PHY */
    shell_write("\r\n    -->  Secondary PHY: ");
    shell_write(mShellPhy[gExtAdvParams.secondaryPHY]);

    /* Tx Power */
    shell_write("\r\n    -->  Tx Power: ");
    shell_writeDec((uint32_t)gExtAdvParams.txPower);

    /* Scan Reqest Notification */
    shell_write("\r\n    -->  Scan Request Notification: ");
    shell_writeBool(gExtAdvParams.enableScanReqNotification);

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
    /* Primary Advertising Phy Options */
    shell_write("\r\n    -->  Primary Advertising Phy Options: ");
    shell_write(mShellAdvPhyOpt[gExtAdvParams.primaryAdvPhyOptions]);

    /* Secondary Advertising Phy Options */
    shell_write("\r\n    -->  Secondary Advertising Phy Options: ");
    shell_write(mShellAdvPhyOpt[gExtAdvParams.secondaryAdvPhyOptions]);
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */

    SHELL_NEWLINE();
}

/*! *********************************************************************************
 * \brief        Helper for ShellGap_SetExtAdvertisingParameters. Checks if the
 *               extended advertising parameter provided at a given index in the
 *               user command is related to the advertising interval. If positive,
 *               it sets the advertising interval parameter.
 *
 * \param[in]    argc           shell argument count
 *
 * \param[in]    argv           shell argument value
 *
 * \param[in]    argIdx         index of the shell argument to check
 *
 * \return       -
 ********************************************************************************** */
static void ShellGap_ConfigureExtAdvInterval(uint8_t argc, char * argv[], uint32_t argIdx)
{
    if(0 == strcmp((char*)argv[argIdx], "-min") && ((argIdx+1U) < argc))
    {
        /* Convert from milliseconds to spec defined units */
        gExtAdvParams.minInterval = (uint32_t)BleApp_atoi(argv[argIdx+1U])*1000U/625U;
    }

    if(0 == strcmp((char*)argv[argIdx], "-max") && ((argIdx+1U) < argc))
    {
        /* Convert from milliseconds to spec defined units */
        gExtAdvParams.maxInterval = (uint32_t)BleApp_atoi(argv[argIdx+1U])*1000U/625U;
    }
}

/*! *********************************************************************************
 * \brief        Helper for ShellGap_SetExtAdvertisingParameters. Checks if the
 *               extended advertising parameter provided at a given index in the
 *               user command is PHY related. If positive, it sets the PHY parameter.
 *
 * \param[in]    argc           shell argument count
 *
 * \param[in]    argv           shell argument value
 *
 * \param[in]    argIdx         index of the shell argument to check
 *
 * \return       -
 ********************************************************************************** */
static void ShellGap_ConfigureExtAdvPhy(uint8_t argc, char * argv[], uint32_t argIdx)
{
    if(0 == strcmp((char*)argv[argIdx], "-phy1") && ((argIdx+1U) < argc))
    {
        gExtAdvParams.primaryPHY = (gapLePhyMode_t)BleApp_atoi(argv[argIdx+1U]);
    }

    if(0 == strcmp((char*)argv[argIdx], "-phy2") && ((argIdx+1U) < argc))
    {
        gExtAdvParams.secondaryPHY = (gapLePhyMode_t)BleApp_atoi(argv[argIdx+1U]);
    }
}
#endif /* BLE_SHELL_AE_SUPPORT */

#if defined(BLE_SHELL_MONADV_SUPPORT) && (BLE_SHELL_MONADV_SUPPORT)
/*! *********************************************************************************
 * \brief        Handles "gap monadvadd" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_AddDeviceToMonAdvList(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;
    bleAddressType_t addressType = 0xFFU;
    bleDeviceAddress_t address = {0};
    int8_t rssiLowThreshold = gBleMonAdvRssiThresholdMin_c;
    int8_t rssiHighThreshold = gBleMonAdvRssiThresholdMax_c;
    uint8_t timeout = 0U;
    /* Use this bitmask to indicate if required parameters are not introduced */
    uint8_t paramsValidBitmask = 0U;

    /* For no argument, print current configuration */
    if (argc >= 10U)
    {
        /* Search for keywords */
        for (uint32_t i = 0U; i < argc; i += 2U)
        {
            if (0 == strcmp((char*)argv[i], "-addr") && ((i + 1U) < argc))
            {
                /* Check that address's length is valid */
                if (gcBleDeviceAddressSize_c == BleApp_ParseHexValue(argv[i + 1U]))
                {
                    FLib_MemCpyReverseOrder(&address, argv[i + 1U], sizeof(bleDeviceAddress_t));
                }
                /* Mandatory argument */
                paramsValidBitmask |= 1U << 0U;
            }

            if (0 == strcmp((char*)argv[i], "-type") && ((i + 1U) < argc))
            {
                addressType = (bleAddressType_t)BleApp_atoi(argv[i + 1U]);
                /* Mandatory argument */
                paramsValidBitmask |= 1U << 1U;
            }

            if (0 == strcmp((char*)argv[i], "-rssilow") && ((i + 1U) < argc))
            {
                rssiLowThreshold = (int8_t)BleApp_atoi(argv[i + 1U]);
                /* Mandatory argument */
                paramsValidBitmask |= 1U << 2U;
            }

            if (0 == strcmp((char*)argv[i], "-rssihigh") && ((i + 1U) < argc))
            {
                rssiHighThreshold = (int8_t)BleApp_atoi(argv[i + 1U]);
                /* Mandatory argument */
                paramsValidBitmask |= 1U << 3U;
            }

            if (0 == strcmp((char*)argv[i], "-timeout") && ((i + 1U) < argc))
            {
                timeout = (uint8_t)BleApp_atoi(argv[i + 1U]);
                /* Mandatory argument */
                paramsValidBitmask |= 1U << 4U;
            }
        }

        /* Only allow the command to the Host if all required parameters are given */
        if (paramsValidBitmask == 0x1FU)
        {
            /* Validate parameter ranges */
            if ((rssiLowThreshold >= gBleMonAdvRssiThresholdMin_c) && 
                (rssiLowThreshold <= gBleMonAdvRssiThresholdMax_c) &&
                (rssiHighThreshold >= gBleMonAdvRssiThresholdMin_c) && 
                (rssiHighThreshold <= gBleMonAdvRssiThresholdMax_c) &&
                (rssiLowThreshold <= rssiHighThreshold) &&
                ((addressType == gBleAddrTypePublic_c) || (addressType == gBleAddrTypeRandom_c)))
            {
                /* Add device to monitored advertisers list */
                if (gBleSuccess_c != Gap_AddDeviceToMonAdvList(addressType, address, rssiLowThreshold, rssiHighThreshold, timeout))
                {
                    shell_write(mShellErrorStatus);
                    result = kStatus_SHELL_Success;
                }
                else
                {
                    result = kStatus_SHELL_Success;
                }
            }
            else
            {
                result = kStatus_SHELL_Error;
            }
        }
        else
        {
            result = kStatus_SHELL_Error;
        }
    }
    else
    {
        result = kStatus_SHELL_Error;
    }

    if (result == kStatus_SHELL_Error)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap monadvrem" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_RemoveDeviceFromMonAdvList(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;
    bleAddressType_t addressType = 0xFFU;
    bleDeviceAddress_t address = {0};
    /* Use this bitmask to indicate if required parameters are not introduced */
    uint8_t paramsValidBitmask = 0U;

    /* For no argument, print current configuration */
    if (argc >= 4U)
    {
        /* Search for keywords */
        for (uint32_t i = 0U; i < argc; i += 2U)
        {
            if (0 == strcmp((char*)argv[i], "-addr") && ((i + 1U) < argc))
            {
                /* Check that address's length is valid */
                if (gcBleDeviceAddressSize_c == BleApp_ParseHexValue(argv[i + 1U]))
                {
                    FLib_MemCpyReverseOrder(&address, argv[i + 1U], sizeof(bleDeviceAddress_t));
                }
                /* Mandatory argument */
                paramsValidBitmask |= 1U << 0U;
            }

            if (0 == strcmp((char*)argv[i], "-type") && ((i + 1U) < argc))
            {
                addressType = (bleAddressType_t)BleApp_atoi(argv[i + 1U]);
                /* Mandatory argument */
                paramsValidBitmask |= 1U << 1U;
            }
        }

        /* Only allow the command to the Host if all required parameters are given */
        if (paramsValidBitmask == 0x03U)
        {
            /* Validate address type */
            if ((addressType == gBleAddrTypePublic_c) || (addressType == gBleAddrTypeRandom_c))
            {
                /* Remove device from monitored advertisers list */
                if (gBleSuccess_c != Gap_RemoveDeviceFromMonAdvList(addressType, address))
                {
                    shell_write(mShellErrorStatus);
                    result = kStatus_SHELL_Success;
                }
                else
                {
                    result = kStatus_SHELL_Success;
                }
            }
            else
            {
                result = kStatus_SHELL_Error;
            }
        }
        else
        {
            result = kStatus_SHELL_Error;
        }
    }
    else
    {
        result = kStatus_SHELL_Error;
    }

    if (result == kStatus_SHELL_Error)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap monadvclear" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_ClearMonAdvList(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    /* Do not accept command if other arguments were passed */
    if (argc != 0U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else
    {
        if (gBleSuccess_c != Gap_ClearMonAdvList())
        {
            shell_write(mShellErrorStatus);
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap monadven" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_EnableMonAdv(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;
    bool_t enable = FALSE;
    /* Use this bitmask to indicate if required parameters are not introduced */
    uint8_t paramsValidBitmask = 0U;

    /* For no argument, print current configuration */
    if (argc == 2U)
    {
        /* Search for keywords */
        for (uint32_t i = 0U; i < argc; i += 2U)
        {
            if (0 == strcmp((char*)argv[i], "-enable") && ((i + 1U) < argc))
            {
                enable = (bool_t)BleApp_atoi(argv[i + 1U]);
                /* Mandatory argument */
                paramsValidBitmask |= 1U << 0U;
            }
        }

        /* Only allow the command to the Host if all required parameters are given */
        if (paramsValidBitmask == 0x01U)
        {
            if (gBleSuccess_c != Gap_EnableMonAdv(enable))
            {
                shell_write(mShellErrorStatus);
                result = kStatus_SHELL_Success;
            }
            else
            {
                result = kStatus_SHELL_Success;
                mNewMonAdvState = enable;
            }
        }
        else
        {
            result = kStatus_SHELL_Error;
        }
    }
    else
    {
        result = kStatus_SHELL_Error;
    }

    if (result == kStatus_SHELL_Error)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap monadvsize" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_ReadMonAdvListSize(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    /* Do not accept command if other arguments were passed */
    if (argc != 0U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else
    {
        if (gBleSuccess_c != Gap_ReadMonAdvListSize())
        {
            shell_write(mShellErrorStatus);
        }
    }

    return result;
}
#endif /* BLE_SHELL_MONADV_SUPPORT */

#if BLE_SHELL_CONN_SBR_SUPPORT
/*! *********************************************************************************
 * \brief        Handles "gap sbrcfg" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_ConfigConnSubrateParams(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Error;
    bool_t bValidCmd = FALSE;

    if (argc == 0U)
    {
        bValidCmd = TRUE;
    }

    /* Search through the arguments for keywords */
    for (uint32_t i = 0U; i < argc; i += 2U)
    {
        if (0 == strcmp((char*)argv[i], "-sbrmin") && ((i+1U) < argc))
        {
            /* Set min subrate factor */
            gConnSubrateParams.subrateMin = (uint16_t)BleApp_atoi(argv[i+1U]);
            bValidCmd = TRUE;
        }
        else if (0 == strcmp((char*)argv[i], "-sbrmax") && ((i+1U) < argc))
        {
            /* Set max subrate factor */
            gConnSubrateParams.subrateMax = (uint16_t)BleApp_atoi(argv[i+1U]);
            bValidCmd = TRUE;
        }
        else if (0 == strcmp((char*)argv[i], "-latency") && ((i+1U) < argc))
        {
            /* The maximum number of consecutive subrated connection events the Peripheral is allowed to ignore. Default: 0 */
            gConnSubrateParams.latencyMax = (uint16_t)BleApp_atoi(argv[i+1U]);
            bValidCmd = TRUE;
        }
        else if (0 == strcmp((char*)argv[i], "-contnum") && ((i+1U) < argc))
        {
            /* Minimum number of underlying connection events to remain active after a packet containing
             a Link Layer PDU with a non-zero Length field is sent or received */
            gConnSubrateParams.continuationNumber = (uint16_t)BleApp_atoi(argv[i+1U]);
            bValidCmd = TRUE;
        }
        else if(0 == strcmp((char*)argv[i], "-timeout") && ((i+1U) < argc))
        {
            /* The maximum time interval between consecutive over-the-air packets; Default: 32s */
            gConnSubrateParams.supervisionTimeout = (uint16_t)BleApp_atoi(argv[i+1U])/10U;
             bValidCmd = TRUE;
        }
        else
        {
            /* MISRA rule 15.7 - All if else if constructs shall be terminated with an else statement */
        }
    }

    /* Print the connection subrate parameters */
    if (bValidCmd)
    {
        shell_write("\r\n-->  Connection Subrate Parameters:");
        shell_write("\r\n    -->  subrateMin: ");
        shell_writeDec((uint32_t)gConnSubrateParams.subrateMin);
        shell_write("\r\n    -->  subrateMax: ");
        shell_writeDec((uint32_t)gConnSubrateParams.subrateMax);
        shell_write("\r\n    -->  Connection Latency: ");
        shell_writeDec((uint32_t)gConnSubrateParams.latencyMax);
        shell_write("\r\n    -->  continuationNumber: ");
        shell_writeDec((uint32_t)gConnSubrateParams.continuationNumber);

        shell_write("\r\n    -->  Supervision Timeout: ");
        /* Convert the value to milliseconds */
        shell_writeDec((uint32_t)gConnSubrateParams.supervisionTimeout * 10U);
        shell_write(" ms");
        SHELL_NEWLINE();
        result = kStatus_SHELL_Success;
    }

    return result;
}
/*! *********************************************************************************
 * \brief        Handles "gap setdefsbrparam" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_SetDefaultConnSbrParams(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;

    if (gBleSuccess_c != Gap_SetDefaultConnectionSubrateParameters(&gConnSubrateParams))
    {
        shell_write(mShellErrorStatus);
        result = kStatus_SHELL_Success;
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Handles "gap connsbrreq" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellGap_ConnSbrReq(uint8_t argc, char * argv[])
{
    shell_status_t result = kStatus_SHELL_Success;
    deviceId_t peerId = gInvalidDeviceId_c;

    if (argc != 1U)
    {
        shell_write("\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n");
        result = kStatus_SHELL_Error;
    }
    else
    {
        peerId = (deviceId_t)BleApp_atoi(argv[0]);
    }

    if ((peerId < gAppMaxConnections_c) && (IS_CONNECTED(peerId) != 0U))
    {
        if (gBleSuccess_c != Gap_ConnectionSubrateRequest(peerId, &gConnSubrateParams))
        {
            shell_write(mShellErrorStatus);
         }
    }
    else
    {
        shell_write("Invalid peer ID!");
        SHELL_NEWLINE();
    }
    return result;
}

/*! *********************************************************************************
 * \brief        Handles the gConnEvtLeSubrateChange_c connection event.
 *
 * \param[in]    peerDeviceId   ID of the peer device 
 *
 * \param[in]    pEvent         pointer to the event data   
 *
 * \return       void
 ********************************************************************************** */
static void ShellGap_HandleSubrateChangeEvt(deviceId_t peerDeviceId, gapSubrateChangeEvent_t *pEvent)
{
    /* Print subrate change event parameters */
    shell_write("\r\n-->  GAP Event: Subrate Changed ");
    shell_writeDec(peerDeviceId);
    shell_write("\r\n    -->  subrateFactor: ");
    shell_writeDec((uint32_t)pEvent->subrateFactor);
    shell_write("\r\n    -->  peripheralLatency: ");
    shell_writeDec((uint32_t)pEvent->peripheralLatency);
    shell_write("\r\n    -->  continuationNumber: ");
    shell_writeDec((uint32_t)pEvent->continuationNumber);
    shell_write("\r\n    -->  supervisionTimeout: ");
    /* Convert the value to milliseconds */
    shell_writeDec((uint32_t)pEvent->supervisionTimeout * 10U);
    shell_write(" ms");
    shell_cmd_finished();
}
#endif /* BLE_SHELL_CONN_SBR_SUPPORT */

/*! *********************************************************************************
 * \brief        Handles BLE generic callback related to advertising.
 *
 * \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
 ********************************************************************************** */
static void ShellGap_GenericCallbackAdv (gapGenericEvent_t* pGenericEvent)
{
    /* Handles generic GAP events related to advertising */
    switch(pGenericEvent->eventType)
    {
        case gExtAdvertisingDataSetupComplete_c:
        case gAdvertisingDataSetupComplete_c:
        {
            shell_write(mGapEventHeader);
            /* Confirm for shell command: "gap advdata [data]" or "gap extadvdata [data]" */
            if (gExtAdvertisingDataSetupComplete_c == pGenericEvent->eventType)
            {
                shell_write("Extended ");
            }

            shell_write("Advertising data successfully set.");
            break;
        }

        case gExtAdvertisingParametersSetupComplete_c:
        case gAdvertisingParametersSetupComplete_c:
        {
            shell_write(mGapEventHeader);
            /* Confirm for shell command: "gap advcfg [params]" or "gap extadvcfg [params]" */
            if (gExtAdvertisingParametersSetupComplete_c == pGenericEvent->eventType)
            {
                shell_write("Extended ");
            }
            shell_write("Advertising parameters successfully set.");
            break;
        }

        case gPeriodicAdvParamSetupComplete_c:
        {
            /* Confirm for shell command: "gap periodiccfg [params]"  */
            shell_write(mGapEventHeader);
            shell_write("Periodic Advertising parameters successfully set.");
            break;
        }

        case gPeriodicAdvDataSetupComplete_c:
        {
            /* Confirm for shell command: "gap periodicdata [data]"  */
            shell_write(mGapEventHeader);
            shell_write("Periodic Advertising data successfully set.");
            break;
        }

        case gPeriodicAdvCreateSyncCancelled_c:
        {
            /* Periodic advertising create sync command was successfully cancelled */
            shell_write(mGapEventHeader);
            shell_write("Periodic sync cancelled.");
            break;
        }

        case gAdvertisingSetupFailed_c:
        {
            /* Error for shell command: "gap advcfg" */
            shell_write(mGapEventHeader);
            shell_write("Advertising setup failed.");
            break;
        }

        case gPeriodicAdvListUpdateComplete_c:
        {
            /* Periodic advertiser list has been successfully updated */
            shell_write(mGapEventHeader);
            shell_write("Periodic Advertising list updated.");
            break;
        }

        case gPeriodicAdvertisingStateChanged_c:
        {
            /* Operation succeeded */
            shell_write("Periodic Advertising state changed successfully!\r\n");
            break;
        }

#if (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1)
        case gPeriodicAdvSetSubeventDataComplete_c:
        {
            /* Operation succeeded */
            shell_write("Periodic advertising subevent data has been successfully set.\r\n");
        }
        break;

        case gPeriodicAdvSetResponseDataComplete_c:
        {
            /* Operation succeeded */
            shell_write("Periodic advertising response data has been successfully set.\r\n");
        }
        break;

        case gPeriodicSyncSubeventComplete_c:
        {
            /* Operation succeeded */
            shell_write("Set Sync Subevent command successfully completed.\r\n");
        }
        break;
#endif /* (defined BLE_SHELL_PAWR_SUPPORT) && (BLE_SHELL_PAWR_SUPPORT == 1) */

        default:
            /* Other Generic Event */
        break;
    }
}
/*! *********************************************************************************
 * @}
 ********************************************************************************** */
