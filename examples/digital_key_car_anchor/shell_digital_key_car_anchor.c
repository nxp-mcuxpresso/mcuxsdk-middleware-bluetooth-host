/*! *********************************************************************************
* \addtogroup Digital Key Car Anchor Application
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file shell_digital_key_car_anchor.c
*
* Copyright 2021-2026 NXP
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
#include "fsl_adapter_reset.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_timer_manager.h"
#include "fsl_format.h"
#include "app.h"

/* BLE Host Stack */
#include "gap_interface.h"

#include "app_conn.h"
#include "digital_key_car_anchor.h"
#include "shell_digital_key_car_anchor.h"

/* Intrusion Detection System */
#if defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U)
#include "ids_test.h"
#endif /* defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U) */

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
* Private functions prototypes
*************************************************************************************
************************************************************************************/
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
SHELL_HANDLE_DEFINE(g_shellHandle);
/* Shell */
static shell_status_t ShellReset_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellFactoryReset_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellStartDiscovery_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellStopDiscovery_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellDisconnect_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellTriggerTimeSync_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSetBondingData_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellListBondedDev_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellRemoveBondedDev_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellListActiveDev_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
#if defined(gHandoverDemo_d) && (gHandoverDemo_d == 1)
static shell_status_t ShellHandoverSendL2cap_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellHandoverAnchorMonitor_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellHandoverPacketMonitor_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellHandoverDevId_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
#endif /* gHandoverDemo_d */

/* Intrusion Detection System*/
#if defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U)
static shell_status_t ShellIdsTest_ParseEventBit(char * argv[], uint8_t* pEventBit, uint32_t* pEventBitMask);
static void ShellIdsTest_DisplayBit6Note(uint32_t eventBitMask);
static shell_status_t ShellIdsTest_ProcessConnectionCategory(uint8_t eventBit, uint32_t eventBitMask, deviceId_t* pDeviceId);
static shell_status_t ShellIdsTest_ValidateDeviceId(int32_t argc, char * argv[], deviceId_t* pDeviceId);
static shell_status_t ShellIdsTest_FindConnectedDevice(deviceId_t* pDeviceId);
static shell_status_t ShellIdsTest_ProcessPairingEvent(uint8_t eventBit, uint32_t eventBitMask, deviceId_t deviceId);
static shell_status_t ShellIdsTest_ProcessConnectionEvent(uint8_t eventBit, uint32_t eventBitMask, deviceId_t deviceId);
static shell_status_t ShellIdsTest_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
#endif /* defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U) */

static uint8_t BleApp_ParseHexValue(char* pInput);
static uint32_t BleApp_AsciiToHex(char *pString, uint32_t strLen);
#if defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U)
static uint32_t BleApp_AsciiToDec(char *pString, uint32_t strLen);
#endif /* defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U) */
static void ShellResetTimeoutTimerCallback(void* pParam);
#endif /* gAppUseShellInApplication_d */
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)

/* shell callback */
static pfShellCallback_t mpfShellEventHandler = NULL;

static shell_command_t mResetCmd =
{
    .pcCommand = "reset",
    .pcHelpString = "\r\n\"reset\": Reset MCU.\r\n",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellReset_Command,
};

static shell_command_t mFactoryResetCmd =
{
    .pcCommand = "factoryreset",
    .pcHelpString = "\r\n\"factoryreset\": Factory Reset.\r\n",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellFactoryReset_Command,
};

static shell_command_t mSdCmd =
{
    .pcCommand = "sd",
    .pcHelpString = "\r\n\"sd\": Start Discovery for Owner Pairing or Passive Entry.\r\n",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellStartDiscovery_Command,
};

static shell_command_t mSpdCmd =
{
    .pcCommand = "spd",
    .pcHelpString = "\r\n\"spd\": Stop Discovery.\r\n",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellStopDiscovery_Command,
};

static shell_command_t mDcntCmd =
{
    .pcCommand = "dcnt",
    .pcHelpString = "\r\n\"dcnt\": Disconnect all peers.\r\n",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellDisconnect_Command,
};

static shell_command_t mTriggerTimeSyncCmd =
{
    .pcCommand = "ts",
    .pcHelpString = "\r\n\"ts\": Trigger a Time Sync from Device.\r\n",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellTriggerTimeSync_Command,
};

static shell_command_t mSetBondingDataCmd =
{
    .pcCommand = "setbd",
    .pcHelpString = "\r\n\"setbd\": Set bonding data.\r\n",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellSetBondingData_Command,
};

static shell_command_t mListBondedDevCmd =
{
    .pcCommand = "listbd",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellListBondedDev_Command,
    .pcHelpString = "\r\n\"listbd\": List bonded devices.\r\n",
};

static shell_command_t mListActiveDevCmd =
{
    .pcCommand = "listad",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellListActiveDev_Command,
    .pcHelpString = "\r\n\"listad\": List active device IDs.\r\n",
};


static shell_command_t mRemoveBondedDevCmd =
{
    .pcCommand = "removebd",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellRemoveBondedDev_Command,
    .pcHelpString = "\r\n\"removebd\": Remove bonded devices.\r\n",
};

#if defined(gHandoverDemo_d) && (gHandoverDemo_d == 1)
static shell_command_t mHandoverSendL2capCmd =
{
    .pcCommand = "send",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellHandoverSendL2cap_Command,
    .pcHelpString = "\r\n\"send\": Send a message over the L2CAP Credit Based channel.\r\n",
};

static shell_command_t mHandoverAnchorMonitorCmd =
{
    .pcCommand = "monitor",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellHandoverAnchorMonitor_Command,
    .pcHelpString = "\r\n\"monitor\": Start or stop SN/NESN anchor monitoring.\r\n",
};

static shell_command_t mHandoverPacketMonitorCmd =
{
    .pcCommand = "packetmon",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellHandoverPacketMonitor_Command,
    .pcHelpString = "\r\n\"packetmon\": Start or stop packet monitoring.\r\n",
};

static shell_command_t mHandoverDevIdCmd =
{
    .pcCommand = "handover",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellHandoverDevId_Command,
    .pcHelpString = "\r\n\"handover\": Start handover for specific device id.\r\n",
};
#endif /* gHandoverDemo_d */

#if defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U)
static shell_command_t mIdsTestCmd =
{
    .pcCommand = "idstest",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellIdsTest_Command,
    .pcHelpString = "\r\n\"idstest\": Trigger IDS security event for testing.\r\n"
                    "Usage: idstest <event_bit> [device_id]\r\n",
};
#endif /* defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U) */

#endif /* gAppUseShellInApplication_d */

static TIMER_MANAGER_HANDLE_DEFINE(mResetTmrId);

/************************************************************************************
*************************************************************************************
* External declarations
*************************************************************************************
************************************************************************************/
#if defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U)
/* External declaration for Host stack global - used for IDS test mode validation */
extern bool_t gAutoRejectLtkRequestForUnbondedDevices;
#endif /* defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U) */

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  Initializes the SHELL module .
*
* \param[in]  prompt the string which will be used for command prompt
*
* \remarks
*
********************************************************************************** */
void AppShellInit(char* prompt)
{
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
    shell_status_t status = kStatus_SHELL_Error;
    
    /* Avoid compiler warning in release mode. */
    (void)status;
    status = SHELL_Init((shell_handle_t)g_shellHandle, (serial_handle_t)gSerMgrIf, prompt);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mResetCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mFactoryResetCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSdCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSpdCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mDcntCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mTriggerTimeSyncCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSetBondingDataCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mListBondedDevCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mRemoveBondedDevCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mListActiveDevCmd);
    assert(kStatus_SHELL_Success == status);

#if defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U)
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mIdsTestCmd);
    assert(kStatus_SHELL_Success == status);
#endif /* defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U) */

#if defined(gHandoverDemo_d) && (gHandoverDemo_d == 1)
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mHandoverSendL2capCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mHandoverAnchorMonitorCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mHandoverPacketMonitorCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mHandoverDevIdCmd);
    assert(kStatus_SHELL_Success == status);
#endif /* gHandoverDemo_d */
#endif
}

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
/*! *********************************************************************************
 * \brief        Register function to handle commands from shell
 *
 * \param[in]    pCallback       event handler
 ********************************************************************************** */
void AppShell_RegisterCmdHandler(pfBleCallback_t pfShellEventHandler)
{
    mpfShellEventHandler = pfShellEventHandler;
}

/*! *********************************************************************************
* \brief        Prints string of hex values
*
* \param[in]    pHex    pointer to hex value.
* \param[in]    len     hex value length.
********************************************************************************** */
void BleApp_PrintHex(uint8_t *pHex, uint8_t len)
{
    for (uint32_t i = 0; i<len; i++)
    {
        (void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, (char const*)FORMAT_Hex2Ascii(pHex[i]));
    }
}

/*! *********************************************************************************
* \brief        Prints string of hex values in reversed order
*
* \param[in]    pHex    pointer to hex LE value.
* \param[in]    len     hex value length.
********************************************************************************** */
void BleApp_PrintHexLe(uint8_t *pHex, uint8_t len)
{
    for (uint32_t i = 0; i<len; i++)
    {
        (void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, (char const*)FORMAT_Hex2Ascii(pHex[((uint32_t)len - 1U) - i]));
    }
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief        Reset MCU.
*
* \param[in]    shellHandle     Shell handle
* \param[in]    argc            Number of arguments
* \param[in]    argv            Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellReset_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    shell_status_t status = kStatus_SHELL_Error;
    timer_status_t tmrStatus = kStatus_TimerError;
    
    tmrStatus = TM_Open(mResetTmrId);
    
    if (tmrStatus == kStatus_TimerSuccess)
    {
        /* Start 10ms timer before reset to allow for shell prompt to be 
        sent over the serial interface. */
        (void)TM_InstallCallback((timer_handle_t)mResetTmrId, ShellResetTimeoutTimerCallback, NULL);
        tmrStatus = TM_Start((timer_handle_t)mResetTmrId, (uint8_t)kTimerModeSingleShot, 10U);
        
        if (tmrStatus == kStatus_TimerSuccess)
        {
            status = kStatus_SHELL_Success;
        }
    }
    
    return status;
}

/*! *********************************************************************************
* \brief        Reset timer callback.
                Called on timer task.
*
* \param[in]    pParam              not used
********************************************************************************** */
static void ShellResetTimeoutTimerCallback(void* pParam)
{
    if(mpfShellEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_Reset_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
            {
               (void)MEM_BufferFree(pEventData);
            }
        }
    }
}

/*! *********************************************************************************
* \brief        Factory Reset.
*
* \param[in]    shellHandle    Shell handle
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellFactoryReset_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if(mpfShellEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_FactoryReset_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
            {
               (void)MEM_BufferFree(pEventData);
            }
        }
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Initialize Owner Pairing or Passive Entry.
*
* \param[in]    shellHandle    Shell handle
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellStartDiscovery_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    const char* ownerPairingCmd = "op";
    const char* passiveEntryCmd = "pe";
    if ((uint32_t)argc == 2U)
    {
        if (TRUE == FLib_MemCmp(argv[1], ownerPairingCmd, 2))
        {
            if(mpfShellEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_Shell_ShellStartDiscoveryOP_Command_c;
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
                    {
                       (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
        else if (TRUE == FLib_MemCmp(argv[1], passiveEntryCmd, 2))
        {
            if(mpfShellEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_Shell_ShellStartDiscoveryPE_Command_c;
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
                    {
                       (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
        else
        {
            shell_write("\r\nUsage: \
                        \r\nsd op - Start advertising for Owner Pairing \
                        \r\nsd pe - Start advertising for Passive Entry \
                        \r\n");
        }
    }
    else
    {
            shell_write("\r\nUsage: \
                        \r\nsd op - Start advertising for Owner Pairing \
                        \r\nsd pe - Start advertising for Passive Entry \
                        \r\n");
    }
    return kStatus_SHELL_Success;
}
                
/*! *********************************************************************************
* \brief        Stop discovery, if active.
*
* \param[in]    shellHandle    Shell handle
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellStopDiscovery_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if(mpfShellEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_StopDiscovery_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
            {
               (void)MEM_BufferFree(pEventData);
            }
        }
    }
    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Disconnect device.
*
* \param[in]    shellHandle    Shell handle
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellDisconnect_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if(mpfShellEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_Disconnect_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
            {
               (void)MEM_BufferFree(pEventData);
            }
        }
    }
    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Trigger a Time Sync from Device.
*
* \param[in]    shellHandle    Shell handle
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellTriggerTimeSync_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if ((uint32_t)argc == 2U)
    {
        deviceId_t deviceId = (uint8_t)BleApp_AsciiToHex(argv[1], FLib_StrLen(argv[1]));
        if (deviceId < (uint8_t)gAppMaxConnections_c)
        {
            if(mpfShellEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_Shell_TriggerTimeSync_Command_c;
                    pEventData->eventData.peerDeviceId = deviceId;
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
                    {
                       (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
        else
        {
            shell_write("\r\nUsage: \
                        \r\nts peer_id \
                        \r\n");
        }
    }
    else
    {
        shell_write("\r\nUsage: \
                    \r\nts peer_id \
                    \r\n");
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Save Bonding Data on device.
*
* \param[in]    shellHandle    Shell handle
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellSetBondingData_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if (argc == 6)
    {
        if(mpfShellEventHandler != NULL)
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(appBondingData_t));
            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_SetBondingData_Command_c;
                pEventData->eventData.pData = &pEventData[1];
                appBondingData_t *pAppBondingEventData = pEventData->eventData.pData;
                if ( sizeof(uint8_t) == BleApp_ParseHexValue(argv[1]) )
                {
                    pAppBondingEventData->nvmIndex = (uint8_t)*argv[1];
                }
                if ( sizeof(bleAddressType_t) == BleApp_ParseHexValue(argv[2]) )
                {
                    pAppBondingEventData->addrType = (uint8_t)*argv[2];
                }
                
                if ( gcBleDeviceAddressSize_c ==  BleApp_ParseHexValue(argv[3]) )
                {
                    FLib_MemCpy(pAppBondingEventData->deviceAddr, argv[3], gcBleDeviceAddressSize_c);
                }
                
                if ( gcSmpMaxLtkSize_c == BleApp_ParseHexValue(argv[4]) )
                {
                    FLib_MemCpy(pAppBondingEventData->aLtk, argv[4], gcSmpMaxLtkSize_c);
                }
                
                if ( gcSmpIrkSize_c ==  BleApp_ParseHexValue(argv[5]) )
                {
                    FLib_MemCpy(pAppBondingEventData->aIrk, argv[5], gcSmpIrkSize_c);
                }

                if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
                {
                   (void)MEM_BufferFree(pEventData);
                }
            }
        }
    }
    else
    {
        shell_write("\r\nUsage: \
                    \r\nsetbd nvm_index addr_type peer_device_address ltk irk  \
                    \r\n");
    }
    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        List bonded devices.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellListBondedDev_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if(mpfShellEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_ListBondedDev_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
            {
               (void)MEM_BufferFree(pEventData);
            }
        }
    }
    return kStatus_SHELL_Success;
}
                    
                    
/*! *********************************************************************************
* \brief        List active devices.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellListActiveDev_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if(mpfShellEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_ListActiveDev_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
            {
               (void)MEM_BufferFree(pEventData);
            }
        }
    }
    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        remove bonded devices.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellRemoveBondedDev_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if (argc == 2)
    {
        if(mpfShellEventHandler != NULL)
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_RemoveBondedDev_Command_c;
                if ( sizeof(uint8_t) == BleApp_ParseHexValue(argv[1]) )
                {
                    /* Store nvm index to be removed in eventData.peerDeviceId  */
                    pEventData->eventData.peerDeviceId = (uint8_t)*argv[1];
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
                    {
                       (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
    }
    else
    {
        shell_write("\r\nUsage: \
                    \r\nremovebd nvm_index \
                    \r\n");
    }
    return kStatus_SHELL_Success;
}

#if defined(gHandoverDemo_d) && (gHandoverDemo_d == 1)
/*! *********************************************************************************
* \brief        Send a predefined L2CAP Credit Based message for demo purposes.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellHandoverSendL2cap_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{

    if(mpfShellEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_HandoverSendL2cap_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
            {
               (void)MEM_BufferFree(pEventData);
            }
        }
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Start or stop anchor monitoring via SN/NESN.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellHandoverAnchorMonitor_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    const char* startCmd = "start";
    const char* stopCmd = "stop";

    if ((uint32_t)argc == 3U)
    {
        deviceId_t deviceId = (uint8_t)BleApp_AsciiToHex(argv[1], FLib_StrLen(argv[1]));
        
        if (deviceId < (uint8_t)gAppMaxConnections_c)
        {
            if (TRUE == FLib_MemCmp(argv[2], startCmd, 4))
            {
                if(mpfShellEventHandler != NULL)
                {
                    appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                    if(pEventData != NULL)
                    {
                        pEventData->eventData.monitorStart.monitorMode = gRssiSniffingMode_c;
                        pEventData->appEvent = mAppEvt_Shell_HandoverStartAnchorMonitor_Command_c;
                        pEventData->eventData.monitorStart.deviceId = deviceId;
                        if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
                        {
                            (void)MEM_BufferFree(pEventData);
                        }
                    }
                }
            }
            else if (TRUE == FLib_MemCmp(argv[2], stopCmd, 4))
            {
                if(mpfShellEventHandler != NULL)
                {
                    appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                    if(pEventData != NULL)
                    {
                        pEventData->appEvent = mAppEvt_Shell_HandoverStopAnchorMonitor_Command_c;
                        pEventData->eventData.peerDeviceId = deviceId;
                        if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
                        {
                            (void)MEM_BufferFree(pEventData);
                        }
                    }
                }
            }
            else
            {
                shell_write("\r\nUsage: monitor deviceId start|stop\r\n");
            }
        }
        else
        {
            shell_write("\r\nInvalid deviceId\r\n");
        }
    }
    else
    {
            shell_write("\r\nUsage: monitor deviceId start|stop\r\n");
    }
    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Start or stop packet monitoring.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellHandoverPacketMonitor_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    const char* startCmd = "start";
    const char* stopCmd = "stop";

    if ((uint32_t)argc == 3U)
    {
        deviceId_t deviceId = (uint8_t)BleApp_AsciiToHex(argv[1], FLib_StrLen(argv[1]));
        
        if (deviceId < (uint8_t)gAppMaxConnections_c)
        {
            if (TRUE == FLib_MemCmp(argv[2], startCmd, 4))
            {
                if(mpfShellEventHandler != NULL)
                {
                    appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                    
                    if(pEventData != NULL)
                    {
                        pEventData->appEvent = mAppEvt_Shell_HandoverStartAnchorMonitor_Command_c;
                        pEventData->eventData.monitorStart.deviceId = deviceId;
                        pEventData->eventData.monitorStart.monitorMode = gPacketMode_c;
                        if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
                        {
                            (void)MEM_BufferFree(pEventData);
                        }
                    }
                }
            }
            else if (TRUE == FLib_MemCmp(argv[2], stopCmd, 4))
            {
                if(mpfShellEventHandler != NULL)
                {
                    appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                    
                    if(pEventData != NULL)
                    {
                        pEventData->appEvent = mAppEvt_Shell_HandoverStopAnchorMonitor_Command_c;
                        pEventData->eventData.peerDeviceId = deviceId;
                        if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
                        {
                            (void)MEM_BufferFree(pEventData);
                        }
                    }
                }
            }
            else
            {
                shell_write("\r\nUsage: packetmon deviceId start|stop\r\n");
            }
        }
        else
        {
            shell_write("\r\nInvalid deviceId\r\n");
        }
    }
    else
    {
            shell_write("\r\nUsage: packetmon deviceId start|stop\r\n");
    }
    return kStatus_SHELL_Success;
}
                    
/*! *********************************************************************************
* \brief        Trigger Connection Handover for the specified peer device.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */                 
static shell_status_t ShellHandoverDevId_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if (argc == 2)
    {
        if(mpfShellEventHandler != NULL)
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_Handover_Command_c;
                if ( sizeof(uint8_t) == BleApp_ParseHexValue(argv[1]) )
                {
                    /* Store device id to be used for handover in eventData.peerDeviceId  */
                    pEventData->eventData.peerDeviceId = (uint8_t)*argv[1];
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfShellEventHandler, pEventData))
                    {
                       (void)MEM_BufferFree(pEventData);
                    }
                }
            }
        }
    }
    else
    {
        shell_write("\r\nUsage: \
                    \r\nhandover deviceId \
                    \r\n");
    }
    return kStatus_SHELL_Success;
}
#endif /* gHandoverDemo_d */
/*! *********************************************************************************
 * \brief        Parses a string input interpreting its content as a hex number and
 *               writes the value at the input address.
 *
 * \param[in]    pInput         Pointer to string
 *
 * \return       uint8_t        Returns the size of the resulted uint value/array
 ********************************************************************************** */
static uint8_t BleApp_ParseHexValue(char* pInput)
{
    uint8_t i, length = (uint8_t)strlen(pInput);
    uint32_t value;
    uint8_t result = 0U;

    /* If the hex misses a 0, return error. Process single character */
    if ((length == 1U) || (length % 2U) == 0U)
    {
        if(0 == strncmp(pInput, "0x", 2))
        {
            length -= 2U;

            /* Save as little endian hex value */
            value = BleApp_AsciiToHex(&pInput[2], FLib_StrLen(&pInput[2]));

            FLib_MemCpy(pInput, &value, sizeof(uint32_t));

            result = length/2U;
        }
        else if (length > 1U)
        {
            char octet[2];

            /* Save as big endian hex */
            for(i=0U;i < length / 2U; i++)
            {
                FLib_MemCpy(octet, &pInput[i*2U], 2U);

                pInput[i] = (char)BleApp_AsciiToHex(octet, 2U);
            }
            result = length/2U;
        }
        else
        {
            /* Convert single character from ASCII to hex */
            pInput[0] = (char)BleApp_AsciiToHex(pInput, length);
            result = length;
        }
    }
    return result;
}

/*!*************************************************************************************************
 *  \brief  Converts a string into hex.
 *
 *  \param  [in]    pString     pointer to string
 *  \param  [in]    strLen      string length
 *
 * \return uint32_t value in hex
 **************************************************************************************************/
static uint32_t BleApp_AsciiToHex(char *pString, uint32_t strLen)
{
    uint32_t length = strLen;
    uint32_t retValue = 0U;
    int32_t hexDig = 0;
    bool_t validChar;

    /* Loop until reaching the end of the string or the given length */
    while ((length != 0U) && (pString != NULL))
    {
        hexDig = 0;
        validChar = FALSE;

        /* digit 0-9 */
        if (*pString >= '0' && *pString <= '9')
        {
            hexDig = *pString - '0';
            validChar = TRUE;
        }

        /* character 'a' - 'f' */
        if (*pString >= 'a' && *pString <= 'f')
        {
            hexDig = *pString - 'a' + 10;
            validChar = TRUE;
        }

        /* character 'A' - 'B' */
        if (*pString >= 'A' && *pString <= 'F')
        {
            hexDig = *pString - 'A' + 10;
            validChar = TRUE;
        }

        /* a hex digit is 4 bits */
        if (validChar == TRUE)
        {
            retValue = (uint32_t)((retValue << 4U) ^ (uint32_t)hexDig);
        }

        /* Increment position */
        pString++;
        length--;
    }

    return retValue;
}

#endif

#if defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U)
/*!*************************************************************************************************
 *  \brief  Converts a string into decimal.
 *
 *  \param  [in]    pString     pointer to string
 *  \param  [in]    strLen      string length
 *
 * \return  uint32_t value in dec
 **************************************************************************************************/
static uint32_t BleApp_AsciiToDec(char *pString, uint32_t strLen)
{
    uint32_t length = strLen;
    uint32_t retValue = 0U;

    while ((length != 0U) && (pString != NULL))
    {
        if (*pString >= '0' && *pString <= '9')
        {
            retValue = (retValue * 10U) + (uint32_t)(*pString - '0');
        }
        else
        {
            break;
        }

        pString++;
        length--;
    }

    return retValue;
}

/*! *********************************************************************************
* \brief        Parse and validate event bit parameter.
*
* \param[in]    argv            Pointer to arguments
* \param[out]   pEventBit       Pointer to store event bit
* \param[out]   pEventBitMask   Pointer to store event bit mask
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellIdsTest_ParseEventBit(char * argv[], uint8_t* pEventBit, uint32_t* pEventBitMask)
{
    shell_status_t status = kStatus_SHELL_Success;

    /* Convert ASCII decimal string to event bit number */
    *pEventBit = (uint8_t)BleApp_AsciiToDec(argv[1], FLib_StrLen(argv[1]));

    /* Validate event bit is within valid range (0-31) */
    if (*pEventBit >= 32U)
    {
        /* Event bit exceeds maximum allowed value */
        shell_write("\r\nInvalid event bit (must be 0-31).\r\n");
        status = kStatus_SHELL_Error;
    }

    /* Calculate bit mask from event bit number */
    if (status == kStatus_SHELL_Success)
    {
        /* Convert bit position to bit mask */
        *pEventBitMask = (1UL << *pEventBit);
    }

    return status;
}

/*! *********************************************************************************
* \brief        Display special notes for BIT6 event.
*
* \param[in]    eventBitMask    Event bit mask
*
* \return       void
********************************************************************************** */
static void ShellIdsTest_DisplayBit6Note(uint32_t eventBitMask)
{
    /* Check if this is the Encryption Request from Non-Bonded Device event (BIT6) */
    if (eventBitMask == (uint32_t)gSecEvt_EncReqNonBonded_c)
    {
        shell_write("\r\n[Note: BIT6 can be triggered naturally: bond -> factoryreset on anchor -> reconnect]\r\n");

        if (gAutoRejectLtkRequestForUnbondedDevices == FALSE)
        {
            shell_write("\r\nPlease set gBleHostAutoRejectLtkRequestForUnbondedDevices_c to TRUE\r\n");
        }
    }
}

/*! *********************************************************************************
* \brief        Process connection category events.
*
* \param[in]    eventBit        Event bit number
* \param[in]    eventBitMask    Event bit mask
* \param[in,out] pDeviceId      Pointer to device ID
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellIdsTest_ProcessConnectionCategory(uint8_t eventBit, uint32_t eventBitMask, deviceId_t* pDeviceId)
{
    shell_status_t status = kStatus_SHELL_Success;

    /* Check if device ID was not specified by user */
    if (*pDeviceId == gInvalidDeviceId_c)
    {
        /* Find first connected device automatically */
        status = ShellIdsTest_FindConnectedDevice(pDeviceId);
    }

    /* Trigger the connection event if device is available */
    if (status == kStatus_SHELL_Success)
    {
        /* Execute the IDS test event for the connection category */
        status = ShellIdsTest_ProcessConnectionEvent(eventBit, eventBitMask, *pDeviceId);
    }

    return status;
}

/*! *********************************************************************************
* \brief        Validate and parse device ID from command arguments.
*
* \param[in]    argc            Number of arguments
* \param[in]    argv            Pointer to arguments
* \param[out]   pDeviceId       Pointer to store device ID
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellIdsTest_ValidateDeviceId(int32_t argc, char * argv[], deviceId_t* pDeviceId)
{
    shell_status_t status = kStatus_SHELL_Success;
    uint8_t requestedDevId;
    bool_t found = FALSE;

    /* Check if device ID parameter was provided */
    if (argc < 3)
    {
        /* No device ID specified - will use default or find connected device later */
        status = kStatus_SHELL_Success;
    }
    else
    {
        /* Parse device ID from command line argument */
        requestedDevId = (uint8_t)BleApp_AsciiToDec(argv[2], FLib_StrLen(argv[2]));

        /* Validate device ID is within allowed range */
        if (requestedDevId >= (uint8_t)gAppMaxConnections_c)
        {
            /* Device ID exceeds maximum connections */
            shell_write("\r\nInvalid device_id (must be 0-");
            shell_writeDec((uint32_t)gAppMaxConnections_c - 1U);
            shell_write(").\r\n");
            status = kStatus_SHELL_Error;
        }

        /* Search for the device in the peer information table */
        if (status == kStatus_SHELL_Success)
        {
            /* Iterate through all possible connections */
            for (uint8_t i = 0U; i < (uint8_t)gAppMaxConnections_c; i++)
            {
                /* Check if this slot has a valid device with matching ID */
                if ((maPeerInformation[i].deviceId != gInvalidDeviceId_c) &&
                    (maPeerInformation[i].deviceId == requestedDevId))
                {
                    /* Device found - store the ID */
                    *pDeviceId = requestedDevId;
                    found = TRUE;
                    break;
                }
            }

            /* Check if requested device was found */
            if (!found)
            {
                /* Device ID is valid but device is not connected */
                shell_write("\r\nDevice ");
                shell_writeDec((uint32_t)requestedDevId);
                shell_write(" is not connected.\r\n");
                status = kStatus_SHELL_Error;
            }
        }
    }

    return status;
}

/*! *********************************************************************************
* \brief        Find first connected device.
*
* \param[out]   pDeviceId       Pointer to store device ID
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellIdsTest_FindConnectedDevice(deviceId_t* pDeviceId)
{
    shell_status_t status = kStatus_SHELL_Success;
    bool_t found = FALSE;

    /* Search through all connection slots for an active device */
    for (uint8_t i = 0U; i < (uint8_t)gAppMaxConnections_c; i++)
    {
        /* Check if this slot contains a valid connected device */
        if (maPeerInformation[i].deviceId != gInvalidDeviceId_c)
        {
            /* Found a connected device - use this one */
            *pDeviceId = maPeerInformation[i].deviceId;
            found = TRUE;
            break;
        }
    }

    /* Check if any connected device was found */
    if (!found)
    {
        /* No active connections available */
        shell_write("\r\nNo active connection found.\r\n");
        shell_write("Please connect a device first or specify device_id.\r\n");
        status = kStatus_SHELL_Error;
    }

    return status;
}

/*! *********************************************************************************
* \brief        Handle pairing category IDS events.
*
* \param[in]    eventBit        Event bit number
* \param[in]    eventBitMask    Event bit mask
* \param[in]    deviceId        Device ID
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellIdsTest_ProcessPairingEvent(uint8_t eventBit, uint32_t eventBitMask, deviceId_t deviceId)
{
    shell_status_t status = kStatus_SHELL_Success;
    bleResult_t result;
    deviceId_t targetDeviceId = deviceId;

    /* Display pairing event activation message */
    shell_write("\r\n[IDS TEST] Pairing event BIT");
    shell_writeDec((uint32_t)eventBit);
    shell_write(" (mask 0x");
    shell_writeHex((uint8_t*)&eventBitMask, (uint8_t)sizeof(uint32_t));
    shell_write(") will be triggered on next pairing");

    /* Display target device ID if specified */
    if (targetDeviceId != gInvalidDeviceId_c)
    {
        shell_write(" for device ");
        shell_writeDec((uint32_t)targetDeviceId);
    }

    /* Display instructions for triggering the event */
    shell_write(".\r\n");
    shell_write("[IDS TEST] Hook activated. Now run 'sd op' to start Owner Pairing.\r\n");

    /* Use default device ID if not specified */
    if (targetDeviceId == gInvalidDeviceId_c)
    {
        /* Default to device 0 for pairing events */
        targetDeviceId = 0U;
    }

    /* Activate the pairing hook for the specified event */
    result = IdsTest_ActivatePairingHook(targetDeviceId, eventBitMask);

    /* Check if hook activation was successful */
    if (result != gBleSuccess_c)
    {
        /* Hook activation failed */
        shell_write("\r\nFailed to activate pairing hook.\r\n");
        status = kStatus_SHELL_Error;
    }

    return status;
}

/*! *********************************************************************************
* \brief        Handle connection category IDS events.
*
* \param[in]    eventBit        Event bit number
* \param[in]    eventBitMask    Event bit mask
* \param[in]    deviceId        Device ID
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellIdsTest_ProcessConnectionEvent(uint8_t eventBit, uint32_t eventBitMask, deviceId_t deviceId)
{
    shell_status_t status = kStatus_SHELL_Success;
    bleResult_t result;

    /* Trigger the IDS test event immediately */
    result = IdsTest_TriggerEvent(deviceId, eventBitMask);

    /* Check if event was triggered successfully */
    if (result == gBleSuccess_c)
    {
        /* Display success message with event details */
        shell_write("\r\n[IDS TEST] Event BIT");
        shell_writeDec((uint32_t)eventBit);
        shell_write(" (mask 0x");
        shell_writeHex((uint8_t*)&eventBitMask, (uint8_t)sizeof(uint32_t));
        shell_write(") triggered for device ");
        shell_writeDec((uint32_t)deviceId);
        shell_write(".\r\n");
    }
    else
    {
        /* Display error message with error code */
        shell_write("\r\nFailed to trigger IDS event. Error code: 0x");
        shell_writeHex((uint8_t*)&result, (uint8_t)sizeof(bleResult_t));
        shell_write("\r\n");
        status = kStatus_SHELL_Error;
    }

    return status;
}

/*! *********************************************************************************
* \brief        Trigger IDS test event.
*
* \param[in]    shellHandle     Shell handle
* \param[in]    argc            Number of arguments
* \param[in]    argv            Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellIdsTest_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    shell_status_t status = kStatus_SHELL_Success;
    uint8_t eventBit = 0U;
    uint32_t eventBitMask = 0U;
    deviceId_t deviceId = gInvalidDeviceId_c;
    uint8_t category;

    /* Validate minimum number of arguments */
    if (argc < 2)
    {
        /* Display usage information */
        shell_write("\r\nUsage: idstest <event_bit> [device_id]\r\n");
        status = kStatus_SHELL_Error;
    }

    /* Parse and validate the event bit parameter */
    if (status == kStatus_SHELL_Success)
    {
        /* Convert event bit string to number and calculate bit mask */
        status = ShellIdsTest_ParseEventBit(argv, &eventBit, &eventBitMask);
    }

    /* Validate optional device ID parameter */
    if (status == kStatus_SHELL_Success)
    {
        /* Check if device ID is valid and device is connected */
        status = ShellIdsTest_ValidateDeviceId(argc, argv, &deviceId);
    }

    /* Display special notes for specific events */
    if (status == kStatus_SHELL_Success)
    {
        /* Show BIT6 specific information if applicable */
        ShellIdsTest_DisplayBit6Note(eventBitMask);
    }

    /* Process the event based on its category */
    if (status == kStatus_SHELL_Success)
    {
        /* Determine if this is a pairing or connection event */
        category = IDS_GET_EVENT_CATEGORY(eventBitMask);
        
        /* Handle pairing events differently from connection events */
        if (category == IDS_EVENT_CATEGORY_PAIRING)
        {
            /* Activate hook for pairing events */
            status = ShellIdsTest_ProcessPairingEvent(eventBit, eventBitMask, deviceId);
        }
        else
        {
            /* Trigger connection events immediately */
            status = ShellIdsTest_ProcessConnectionCategory(eventBit, eventBitMask, &deviceId);
        }
    }

    return status;
}

#endif /* defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U) */