/*! *********************************************************************************
* \addtogroup Digital Key Device CS Shell
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file shell_digital_key_device_cs.c
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
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
#include "fsl_shell.h"
#endif
#include "fsl_adapter_reset.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_timer_manager.h"

#include "app.h"

/* BLE Host Stack */
#include "gap_interface.h"
#include "app_localization.h"
#include "digital_key_device_cs.h"
#include "shell_digital_key_device_cs.h"
#include "app_conn.h"
#include "channel_sounding.h"

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
* Private memory declarations
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
static shell_status_t ShellSetBondingData_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellListBondedDev_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellRemoveBondedDev_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSetCsConfigParams_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSetCsProcedureParams_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellTriggerDistanceMeasurement_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSetVerbosityLevel_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSetCsRole_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSetNumProcs_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSelectAlgorithm_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);

static bleResult_t ShellSetCsConfigParams_UpdateParam(char *pName, char *pValue, appCsConfigParams_t *pParams);
static void ShellSetCsConfig_PrefillFromCurrent(deviceId_t deviceId, appCsConfigParams_t *pParams);
static bleResult_t ShellSetCsProcedureParams_UpdateParam(char *pName, char *pValue, appCsProcedureParams_t *pParams);
static void ShellSetCsProc_PrefillFromCurrent(deviceId_t deviceId, appCsProcedureParams_t *pParams);

static bleResult_t ShellSetCsProc_HandleSuccess(appEventData_t *pEventData);

static uint8_t BleApp_ParseHexValue(char* pInput);
static uint32_t BleApp_AsciiToHex(char *pString, uint32_t strLen);
static int32_t BleApp_atoi(char *pStr);

static void ShellResetTimeoutTimerCallback(void* pParam);

static shell_command_t mResetCmd =
{
    .pcCommand = "reset",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellReset_Command,
    .pcHelpString = "\r\n\"reset\": Reset MCU.\r\n",
};

static shell_command_t mFactoryResetCmd =
{
    .pcCommand = "factoryreset",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellFactoryReset_Command,
    .pcHelpString = "\r\n\"factoryreset\": Factory Reset.\r\n",
};

static shell_command_t mSdCmd =
{
    .pcCommand = "sd",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellStartDiscovery_Command,
    .pcHelpString = "\r\n\"sd\": Start Discovery for Owner Pairing or Passive Entry.\r\n",
};

static shell_command_t mSpdCmd =
{
    .pcCommand = "spd",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellStopDiscovery_Command,
    .pcHelpString = "\r\n\"spd\": Stop Discovery.\r\n",
};

static shell_command_t mDcntCmd =
{
    .pcCommand = "dcnt",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellDisconnect_Command,
    .pcHelpString = "\r\n\"dcnt\": Disconnect all peers.\r\n",
};

static shell_command_t mSetBondingDataCmd =
{
    .pcCommand = "setbd",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellSetBondingData_Command,
    .pcHelpString = "\r\n\"setbd\": Set bonding data.\r\n",
};

static shell_command_t mListBondedDevCmd =
{
    .pcCommand = "listbd",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellListBondedDev_Command,
    .pcHelpString = "\r\n\"listbd\": List bonded devices.\r\n",
};

static shell_command_t mRemoveBondedDevCmd =
{
    .pcCommand = "removebd",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellRemoveBondedDev_Command,
    .pcHelpString = "\r\n\"removebd\": Remove bonded devices.\r\n",
};

static shell_command_t mSetCsConfigParamsCmd =
{
    .pcCommand = "setcsconfig",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellSetCsConfigParams_Command,
    .pcHelpString = "\r\n\"setcsconfig\": Set default parameters for Channel Sounding Create Config command.\r\n",
};

static shell_command_t mSetCsProcParamsCmd =
{
    .pcCommand = "setcsproc",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellSetCsProcedureParams_Command,
    .pcHelpString = "\r\n\"setcsproc\": Set default parameters for Channel Sounding Set Procedure Parameters command.\r\n",
};

static shell_command_t mTriggerCsDistMeasCmd =
{
    .pcCommand = "tdm",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellTriggerDistanceMeasurement_Command,
    .pcHelpString = "\r\n\"tdm\": Trigger Channel Sounding distance measurements with a given peer.\r\n",
};

static shell_command_t mSetVerbosityCmd =
{
    .pcCommand = "verbosity",
    .pcHelpString = "\r\n\"verbosity\": Set verbosity level during CS procedure.\r\n"
                    "2 = All prints enabled.\r\n"
                    "1 = Only local procedure ended print enabled.\r\n"
                    "0 = No print enabled.\r\n",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellSetVerbosityLevel_Command,
};

static shell_command_t mSetCsRoleCmd =
{
    .pcCommand = "role",
    .pcHelpString = "\r\n\"role\": Set Channel Sounding role [0-1].\r\n"
                    "1 = Reflector\r\n"
                    "0 = Initiator\r\n",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellSetCsRole_Command,
};

static shell_command_t mSetNumProcsCmd =
{
    .pcCommand = "setnumprocs",
    .pcHelpString = "\r\n\"setnumprocs\": Set number of CS procedures.\r\n",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellSetNumProcs_Command,
};

static shell_command_t mSelectAlgoCmd =
{
    .pcCommand = "setalgo",
    .pcHelpString = "\r\n\"setalgo\": Select the algorithm to run at the end of the CS procedure [0-3].\r\n"
                    "BIT0 - CDE; BIT1 - RADE\r\n",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellSelectAlgorithm_Command,
};
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/


/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  Initializes the SHELL module .
*
* \param[in]  prompt the string which will be used for command prompt
********************************************************************************** */
void AppShellInit(char *prompt)
{
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
    /* UI */
    shell_status_t status = kStatus_SHELL_Error;
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
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSetBondingDataCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mListBondedDevCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mRemoveBondedDevCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSetCsConfigParamsCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSetCsProcParamsCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mTriggerCsDistMeasCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSetVerbosityCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSetCsRoleCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSetNumProcsCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSelectAlgoCmd);
    assert(kStatus_SHELL_Success == status);
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */
}

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
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
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
/*! *********************************************************************************
* \brief        Save Bonding Data on device.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellSetBondingData_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if (argc == 6)
    {
        if(mpfBleEventHandler != NULL)
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(appBondingData_t));
            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_SetBondingData_Command_c;
                pEventData->eventData.pData = pEventData + 1;
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

                if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
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
    if(mpfBleEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_ListBondedDev_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
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
        if(mpfBleEventHandler != NULL)
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_RemoveBondedDev_Command_c;
                if ( sizeof(uint8_t) == BleApp_ParseHexValue(argv[1]) )
                {
                    /* Store nvm index to be removed in eventData.peerDeviceId  */
                    pEventData->peerDeviceId = (uint8_t)*argv[1];
                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
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

/*! *********************************************************************************
 * \brief        Reset MCU.
 *
 ********************************************************************************** */
static shell_status_t ShellReset_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    shell_status_t status = kStatus_SHELL_Success;
    static TIMER_MANAGER_HANDLE_DEFINE(resetTmrId);
    timer_status_t tmrStatus;

    tmrStatus = TM_Open(resetTmrId);

    if (tmrStatus != kStatus_TimerSuccess)
    {
        status = kStatus_SHELL_Error;
    }
    else
    {
        /* Start 10ms timer before reset to allow for shell prompt to be
        sent over the serial interface. */
        (void)TM_InstallCallback((timer_handle_t)resetTmrId, ShellResetTimeoutTimerCallback, NULL);
        (void)TM_Start((timer_handle_t)resetTmrId, (uint8_t)kTimerModeSingleShot, 10U);
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
    if(mpfBleEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_Reset_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
            {
                (void)MEM_BufferFree(pEventData);
            }
        }
    }
}

/*! *********************************************************************************
 * \brief        Factory Reset.
 *
 ********************************************************************************** */
static shell_status_t ShellFactoryReset_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if(mpfBleEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_FactoryReset_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
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
 ********************************************************************************** */
static shell_status_t ShellStartDiscovery_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if(mpfBleEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_ShellStartDiscovery_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
            {
                (void)MEM_BufferFree(pEventData);
            }
        }
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
 * \brief        Stop discovery, if active.
 *
 ********************************************************************************** */
static shell_status_t ShellStopDiscovery_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if(mpfBleEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_StopDiscovery_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
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
 ********************************************************************************** */
static shell_status_t ShellDisconnect_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if(mpfBleEventHandler != NULL)
    {
        appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
        if(pEventData != NULL)
        {
            pEventData->appEvent = mAppEvt_Shell_Disconnect_Command_c;
            if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
            {
                (void)MEM_BufferFree(pEventData);
            }
        }
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Validates and applies a single CS config parameter by name/value.
*
* \param[in]    pName               Parameter name
* \param[in]    pValue              Parameter value (as string)
* \param[out]   pParams             Pointer to CS config parameters
*
* \return       bleResult_t         Validation result
********************************************************************************** */
static bleResult_t ShellSetCsConfigParams_UpdateParamGroup1(char *pName, char *pValue, appCsConfigParams_t *pParams, bool_t *pHandled)
{
    bleResult_t status = gBleSuccess_c;
    uint8_t val = (uint8_t)BleApp_atoi(pValue);

    *pHandled = TRUE;

    if (0 == strcmp(pName, "mainmodetype"))
    {
        if ((val == 0U) || (val > 3U))
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->mainModeType = val;
        }
    }
    else if (0 == strcmp(pName, "submodetype"))
    {
        if ((val == 0U) || ((val > 3U) && (val != 255U)))
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->subModeType = val;
        }
    }
    else if (0 == strcmp(pName, "mainmodeminsteps"))
    {
        pParams->mainModeMinSteps = val;
    }
    else if (0 == strcmp(pName, "mainmodemaxsteps"))
    {
        pParams->mainModeMaxSteps = val;
    }
    else if (0 == strcmp(pName, "mainmoderepetition"))
    {
        if (val > 3U)
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->mainModeRepetition = val;
        }
    }
    else if (0 == strcmp(pName, "mode0steps"))
    {
        if ((val == 0U) || (val > 3U))
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->mode0Steps = val;
        }
    }
    else
    {
        *pHandled = FALSE;
    }

    return status;
}

/*! *********************************************************************************
* \brief        Update and validate a single CS config parameter (group 2).
*
* \param[in]    pName          Parameter name
* \param[in]    pValue         Parameter value (as string)
* \param[out]   pParams        Pointer to the CS config parameters to update
* \param[out]   pHandled       Set to TRUE if the parameter name was recognized
*
* \return       bleResult_t    gBleSuccess_c on success, error code otherwise
********************************************************************************** */
static bleResult_t ShellSetCsConfigParams_UpdateParamGroup2(char *pName, char *pValue, appCsConfigParams_t *pParams, bool_t *pHandled)
{
    bleResult_t status = gBleSuccess_c;
    uint8_t val = (uint8_t)BleApp_atoi(pValue);

    *pHandled = TRUE;

    if (0 == strcmp(pName, "role"))
    {
        if (val > 1U)
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->role = val;
        }
    }
    else if (0 == strcmp(pName, "rtttype"))
    {
        if (val > 6U)
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->RTTType = val;
        }
    }
    else if (0 == strcmp(pName, "channelmap"))
    {
        if (gHCICSChannelMapSize == BleApp_ParseHexValue(pValue))
        {
            FLib_MemCpy(pParams->channelMap, pValue, gCsChannelMapLength_c);
        }
        else
        {
            status = gBleInvalidParameter_c;
        }
    }
    else if (0 == strcmp(pName, "channelmaprepetition"))
    {
        if (val == 0U)
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->channelMapRepetition = val;
        }
    }
    else if (0 == strcmp(pName, "channelselectiontype"))
    {
        if (val > 1U)
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->channelSelectionType = val;
        }
    }
    else if (0 == strcmp(pName, "cssyncphy"))
    {
        if ((val < 1U) || (val > 3U))
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->csSyncPhy = val;
        }
    }
    else
    {
        *pHandled = FALSE;
    }

    return status;
}

/*! *********************************************************************************
* \brief        Update and validate a single CS config parameter identified by its
*               name. Delegates to group helpers to keep complexity low.
*
* \param[in]    pName          Parameter name
* \param[in]    pValue         Parameter value (as string)
* \param[out]   pParams        Pointer to the CS config parameters to update
*
* \return       bleResult_t    gBleSuccess_c on success, error code otherwise
********************************************************************************** */
static bleResult_t ShellSetCsConfigParams_UpdateParam(char *pName, char *pValue, appCsConfigParams_t *pParams)
{
    bleResult_t status;
    bool_t handled = FALSE;

    status = ShellSetCsConfigParams_UpdateParamGroup1(pName, pValue, pParams, &handled);

    if (!handled)
    {
        status = ShellSetCsConfigParams_UpdateParamGroup2(pName, pValue, pParams, &handled);
    }

    if (!handled)
    {
        status = gBleInvalidParameter_c;
    }

    return status;
}


/*! *********************************************************************************
* \brief        Pre-populates the CS config parameters from the current configuration
*               so that unspecified parameters retain their current values.
*
* \param[in]    deviceId            Peer device identifier
* \param[out]   pParams             Pointer to CS config parameters
********************************************************************************** */
static void ShellSetCsConfig_PrefillFromCurrent(deviceId_t deviceId, appCsConfigParams_t *pParams)
{
    appLocalization_rangeCfg_t currentCfg;

    (void)AppLocalization_ReadConfig(deviceId, &currentCfg);

    pParams->mainModeType = currentCfg.main_mode_type;
    pParams->subModeType = currentCfg.sub_mode_type;
    pParams->mainModeMinSteps = currentCfg.main_mode_min;
    pParams->mainModeMaxSteps = currentCfg.main_mode_max;
    pParams->mainModeRepetition = currentCfg.main_mode_repeat;
    pParams->mode0Steps = currentCfg.mode0_nb;
    pParams->role = mGlobalRangeSettings.role;
    pParams->RTTType = currentCfg.rtt_type;
    FLib_MemCpy(pParams->channelMap, currentCfg.ch_map, gCsChannelMapLength_c);
    pParams->channelMapRepetition = currentCfg.ch_map_repeat;
    pParams->channelSelectionType = currentCfg.channelSelectionType;
    pParams->csSyncPhy = currentCfg.cs_sync_phy;
}

/*! *********************************************************************************
* \brief        Set CS Create Config parameters. Allows per-parameter updates by
*               name/value pairs. Unspecified parameters retain their current value.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellSetCsConfigParams_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    bleResult_t status = gBleSuccess_c;

    if ((argc >= 4) && (((argc - 2) % 2) == 0))
    {
        deviceId_t deviceId = (uint8_t)BleApp_atoi(argv[1]);

        if (deviceId >= (uint8_t)gAppMaxConnections_c)
        {
            status = gBleInvalidParameter_c;
        }

        if (status == gBleSuccess_c)
        {
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(appCsConfigParams_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_Shell_SetCsConfigParams_Command_c;
                    pEventData->peerDeviceId = deviceId;
                    pEventData->eventData.pData = pEventData + 1;
                    appCsConfigParams_t *pAppCsConfigParams = pEventData->eventData.pData;

                    /* Pre-populate with the current configuration so that only the
                       specified parameters are updated. */
                    ShellSetCsConfig_PrefillFromCurrent(deviceId, pAppCsConfigParams);

                    uint8_t argCount = (uint8_t)argc;
                    for (uint8_t i = 2U; i < argCount; i += 2U)
                    {
                        status = ShellSetCsConfigParams_UpdateParam(argv[i], argv[i + 1U], pAppCsConfigParams);
                        if (status != gBleSuccess_c)
                        {
                            break;
                        }
                    }

                    if (status == gBleSuccess_c)
                    {
                        status = App_PostCallbackMessage(mpfBleEventHandler, pEventData);
                    }

                    if (status != gBleSuccess_c)
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                    else
                    {
                        shell_write("\r\nConfig parameters set successfully.\r\n");
                    }
                }
                else
                {
                    status = gBleOutOfMemory_c;
                }
            }
        }
    }
    else
    {
        status = gBleInvalidParameter_c;
    }

    if (status == gBleOutOfMemory_c)
    {
        shell_write("\r\nOut of memory\r\n");
    }
    if (status == gBleInvalidParameter_c)
    {
        shell_write("\r\nInvalid parameter\r\n");
        shell_write("\r\nUsage: \
                    \r\nsetcsconfig peer_id <paramName> <value> [<paramName> <value> ...] \
                    \r\nparamName: mainmodetype, submodetype, mainmodeminsteps, mainmodemaxsteps, mainmoderepetition, mode0steps, role, rtttype, channelmap, channelmaprepetition, channelselectiontype, cssyncphy \
                    \r\n");
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Check whether a value is a valid SNR control setting.
*
* \param[in]    val                 SNR control value to validate
*
* \return       bool_t              TRUE if the value is a valid SNR control setting
********************************************************************************** */
static bool_t ShellIsValidSnrControl(uint8_t val)
{
    return isValidSnrControl(val);
}

/*! *********************************************************************************
* \brief        Validates and applies a single CS procedure parameter by name/value.
*
* \param[in]    pName               Parameter name
* \param[in]    pValue              Parameter value (as string)
* \param[out]   pParams             Pointer to CS procedure parameters
*
* \return       bleResult_t         Validation result
********************************************************************************** */
static bleResult_t ShellSetCsProcedureParams_UpdateParam(char *pName, char *pValue, appCsProcedureParams_t *pParams)
{
    bleResult_t status = gBleSuccess_c;

    if (0 == strcmp(pName, "maxprocedureduration"))
    {
        uint16_t val = (uint16_t)BleApp_atoi(pValue);

        if (val == 0U)
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->maxProcedureDuration = val;
        }
    }
    else if (0 == strcmp(pName, "minperiodbetweenprocedures"))
    {
        pParams->minPeriodBetweenProcedures = (uint16_t)BleApp_atoi(pValue);
    }
    else if (0 == strcmp(pName, "maxperiodbetweenprocedures"))
    {
        pParams->maxPeriodBetweenProcedures = (uint16_t)BleApp_atoi(pValue);
    }
    else if (0 == strcmp(pName, "maxnumprocedures"))
    {
        pParams->maxNumProcedures = (uint16_t)BleApp_atoi(pValue);
    }
    else if (0 == strcmp(pName, "minsubeventlen"))
    {
        pParams->minSubeventLen = (uint32_t)BleApp_atoi(pValue);
    }
    else if (0 == strcmp(pName, "maxsubeventlen"))
    {
        pParams->maxSubeventLen = (uint32_t)BleApp_atoi(pValue);
    }
    else if (0 == strcmp(pName, "antcfgindex"))
    {
        uint8_t val = (uint8_t)BleApp_atoi(pValue);

        if (val > 7U)
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->antCfgIndex = val;
        }
    }
    else if (0 == strcmp(pName, "snrcontrolinit"))
    {
        uint8_t val = (uint8_t)BleApp_atoi(pValue);

        if (!ShellIsValidSnrControl(val))
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->snrControlInit = val;
        }
    }
    else if (0 == strcmp(pName, "snrcontrolrefl"))
    {
        uint8_t val = (uint8_t)BleApp_atoi(pValue);

        if (!ShellIsValidSnrControl(val))
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            pParams->snrControlRefl = val;
        }
    }
    else
    {
        status = gBleInvalidParameter_c;
    }

    return status;
}

/*! *********************************************************************************
* \brief        Pre-populates the CS procedure parameters from the current
*               configuration so that unspecified parameters retain their values.
*
* \param[in]    deviceId            Peer device identifier
* \param[out]   pParams             Pointer to CS procedure parameters
********************************************************************************** */
static void ShellSetCsProc_PrefillFromCurrent(deviceId_t deviceId, appCsProcedureParams_t *pParams)
{
    appLocalization_rangeCfg_t currentCfg;

    (void)AppLocalization_ReadConfig(deviceId, &currentCfg);

    pParams->maxProcedureDuration = currentCfg.maxProcedureDuration;
    pParams->minPeriodBetweenProcedures = currentCfg.minPeriodBetweenProcedures;
    pParams->maxPeriodBetweenProcedures = currentCfg.maxPeriodBetweenProcedures;
    pParams->maxNumProcedures = currentCfg.maxNumProcedures;
    pParams->minSubeventLen = currentCfg.minSubeventLen;
    pParams->maxSubeventLen = currentCfg.maxSubeventLen;
    pParams->antCfgIndex = currentCfg.ant_cfg_index;
    pParams->snrControlInit = currentCfg.snr_control_init;
    pParams->snrControlRefl = currentCfg.snr_control_refl;
}

/*! *********************************************************************************
* \brief        Handles successful parameter validation and message posting.
*
* \param[in]    pEventData          Pointer to event data
*
* \return       bleResult_t         Result of message posting
********************************************************************************** */
static bleResult_t ShellSetCsProc_HandleSuccess(appEventData_t *pEventData)
{
    bleResult_t status = gBleSuccess_c;

    status = App_PostCallbackMessage(mpfBleEventHandler, pEventData);

    if (status != gBleSuccess_c)
    {
        (void)MEM_BufferFree(pEventData);
    }
    else
    {
        shell_write("\r\nProcedure parameters set successfully.\r\n");
    }

    return status;
}

/*! *********************************************************************************
* \brief        Set CS Procedure parameters. Allows per-parameter updates by
*               name/value pairs. Unspecified parameters retain their current value.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellSetCsProcedureParams_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    bleResult_t status = gBleSuccess_c;

    if ((argc >= 4) && (((argc - 2) % 2) == 0))
    {
        deviceId_t deviceId = (uint8_t)BleApp_atoi(argv[1]);

        if (deviceId >= (uint8_t)gAppMaxConnections_c)
        {
            status = gBleInvalidParameter_c;
        }

        if (status == gBleSuccess_c)
        {
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t) + sizeof(appCsProcedureParams_t));
                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_Shell_SetCsProcedureParams_Command_c;
                    pEventData->peerDeviceId = deviceId;
                    pEventData->eventData.pData = pEventData + 1;
                    appCsProcedureParams_t *pAppCsProcedureParams = pEventData->eventData.pData;

                    /* Pre-populate with the current configuration so that only the
                       specified parameters are updated. */
                    ShellSetCsProc_PrefillFromCurrent(deviceId, pAppCsProcedureParams);

                    uint8_t argCount = (uint8_t)argc;
                    for (uint8_t i = 2U; i < argCount; i += 2U)
                    {
                        status = ShellSetCsProcedureParams_UpdateParam(argv[i], argv[i + 1U], pAppCsProcedureParams);
                        if (status != gBleSuccess_c)
                        {
                            break;
                        }
                    }

                    if (status == gBleSuccess_c)
                    {
                        status = ShellSetCsProc_HandleSuccess(pEventData);
                    }
                    else
                    {
                        (void)MEM_BufferFree(pEventData);
                    }
                }
                else
                {
                    status = gBleOutOfMemory_c;
                }
            }
        }
    }
    else
    {
        status = gBleInvalidParameter_c;
    }

    if (status == gBleOutOfMemory_c)
    {
        shell_write("\r\nOut of memory\r\n");
    }
    if (status == gBleInvalidParameter_c)
    {
        shell_write("\r\nInvalid parameter\r\n");
        shell_write("\r\nUsage: \
                    \r\nsetcsproc peer_id <paramName> <value> [<paramName> <value> ...] \
                    \r\nparamName: maxprocedureduration, minperiodbetweenprocedures, maxperiodbetweenprocedures, maxnumprocedures, minsubeventlen, maxsubeventlen, antcfgindex, snrcontrolinit, snrcontrolrefl \
                    \r\n");
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Trigger CS distance measurement.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellTriggerDistanceMeasurement_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    bleResult_t status = gBleSuccess_c;

    if ((uint32_t)argc == 2U)
    {
        /* Single device measurement */
        deviceId_t deviceId = (uint8_t)BleApp_AsciiToHex(argv[1], FLib_StrLen(argv[1]));

        if (deviceId < (uint8_t)gAppMaxConnections_c)
        {
            if (mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));

                if (pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_Shell_TriggerCsDistanceMeasurement_Command_c;
                    pEventData->peerDeviceId = deviceId;

                    if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                    {
                        (void)MEM_BufferFree(pEventData);
                        status = gBleOutOfMemory_c;
                    }
                }
                else
                {
                    status = gBleOutOfMemory_c;
                }
            }
        }
        else
        {
            status = gBleInvalidParameter_c;
        }
    }
    else if ((uint32_t)argc == 1U)
    {
        /* All devices measurement */
        if (mpfBleEventHandler != NULL)
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));

            if (pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_TriggerCsDistanceMeasurement_Command_c;
                pEventData->peerDeviceId = gInvalidDeviceId_c;

                if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pEventData);
                    status = gBleOutOfMemory_c;
                }
            }
            else
            {
                status = gBleOutOfMemory_c;
            }
        }
    }
    else
    {
        shell_write("\r\nUsage: \
                    \r\ntdm device_id (trigger measurement with one peer) \
                    \r\ntdm (trigger measurements with all peers) \
                    \r\n");
    }

    if (status == gBleInvalidParameter_c)
    {
        shell_write("\r\nInvalid parameter.\r\n");
    }
    else if (status == gBleOutOfMemory_c)
    {
        shell_write("\r\nOut of memory.\r\n");
    }
    else
    {
        /* MISRA C-2012 Rule 15.7 */
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Set verbosity level during CS procedure.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellSetVerbosityLevel_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if (argc == 2)
    {
        if(mpfBleEventHandler != NULL)
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_SetVerbosityLevel_Command_c;
                if ( sizeof(uint8_t) == BleApp_ParseHexValue(argv[1]) )
                {
                    /* Store verbosity level to be set in eventData.verbosityLevel  */
                    uint8_t verbosityLevel = (uint8_t)*argv[1];
                    if (verbosityLevel >= 3U)
                    {
                        shell_write("\r\nUsage: verbosity level[0-2] \r\n");
                    }
                    else
                    {
                        pEventData->eventData.verbosityLevel = verbosityLevel;
                        if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                        {
                            (void)MEM_BufferFree(pEventData);
                        }
                    }
                }
            }
        }
    }
    else
    {
        shell_write("\r\nUsage: \
                    \r\nverbosity level[0-2] \
                    \r\n");
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Select the Channel Sounding role.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellSetCsRole_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    bleResult_t status = gBleSuccess_c;

    if (argc == 2)
    {
        if (BleApp_CheckActiveConnections() == TRUE)
        {
            shell_write("\r\nCannot change role while in active connection(s).\r\n");
        }
        else
        {
            uint8_t role = (uint8_t)BleApp_AsciiToHex(argv[1], FLib_StrLen(argv[1]));

            if (sizeof(uint8_t) != BleApp_ParseHexValue(argv[1]))
            {
                status = gBleInvalidParameter_c;
            }

            if (status == gBleSuccess_c)
            {
                if (role == 0U)
                {
                    mGlobalRangeSettings.role = gCsRoleInitiator_c;
                    shell_write("\r\nInitiator role set successfully.\r\n");
                }
                else if (role == 1U)
                {
                    mGlobalRangeSettings.role = gCsRoleReflector_c;
                    shell_write("\r\nReflector role set successfully.\r\n");
                }
                else
                {
                    status = gBleInvalidParameter_c;
                }
            }
        }
    }
    else
    {
        shell_write("\r\n Usage: role [0-1]: \
                     \r\n Set Channel Sounding role for the peerId connection.\r\n 1 = Reflector\r\n 0 = Initiator\r\n");
    }

    if (status == gBleInvalidParameter_c)
    {
        shell_write("\r\nInvalid parameter. \
                     \r\nUsage: role [0-1].\r\n");
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Set the number of CS procedures.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellSetNumProcs_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    bleResult_t status = gBleSuccess_c;

    if (argc == 3)
    {
        deviceId_t deviceId = (uint8_t)BleApp_AsciiToHex(argv[1], FLib_StrLen(argv[1]));

        if (sizeof(uint8_t) != BleApp_ParseHexValue(argv[1]) ||
            deviceId >= (uint8_t)gAppMaxConnections_c)
        {
            status = gBleInvalidParameter_c;
        }

        if (status == gBleSuccess_c)
        {
            if (mpfBleEventHandler != NULL)
            {
                uint16_t value = (uint16_t)BleApp_AsciiToHex(argv[2], strlen(argv[2]));

                if (sizeof(uint16_t) != BleApp_ParseHexValue(argv[2]))
                {
                    status = gBleInvalidParameter_c;
                }

                if (status == gBleSuccess_c)
                {
                    appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
                    if (pEventData != NULL)
                    {
                        pEventData->appEvent = mAppEvt_Shell_SetNumProcs_Command_c;
                        pEventData->peerDeviceId = deviceId;
                        pEventData->eventData.numProcedures = value;

                        if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                        {
                           (void)MEM_BufferFree(pEventData);
                        }
                        else
                        {
                            shell_write("\r\nNumber of procedures set successfully.\r\n");
                        }
                    }
                }
            }
            else
            {
                status = gBleOutOfMemory_c;
            }
        }
    }
    else
    {
        shell_write("\r\nUsage: setnumprocs peer_id [0x0000-0xffff].\r\n");
    }

    if (status == gBleOutOfMemory_c)
    {
        shell_write("\r\nOut of memory\r\n");
    }
    if (status == gBleInvalidParameter_c)
    {
        shell_write("\r\nInvalid parameter. \
                     \r\nUsage: setnumprocs peer_id [0x0000-0xffff].\r\n");
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Select the algorithm to run at the end of the CS procedure.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellSelectAlgorithm_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    uint8_t userSelectedAlgo = 0U;
    uint8_t algo = 0U;

    if (argc == 2)
    {
        if (mpfBleEventHandler != NULL)
        {
            appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));
            if(pEventData != NULL)
            {
                pEventData->appEvent = mAppEvt_Shell_SetAlgorithm_Command_c;
                userSelectedAlgo = (uint8_t)*argv[1];
                /* Store algoritms to be set in eventData.algorithmSelection */
                if (userSelectedAlgo > 3U)
                {
                    shell_write("\r\nUsage: setalgo [0-3].\n\rAlgorithm selection value: BIT0 - CDE; BIT1 - RADE\r\n");
                }

                if ((userSelectedAlgo & eMciqAlgoEmbedCDE) != 0U)
                {
#if defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)
                    algo |= eMciqAlgoEmbedCDE;
#else
                    shell_write("\r\nCDE algorithm not enabled at application level! Set gAppUseCDEAlgorithm_d to 1!\r\n");
#endif
                }

                if ((userSelectedAlgo & eMciqAlgoEmbedRADE) != 0U)
                {
#if defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)
                    algo |= eMciqAlgoEmbedRADE;
#else
                    shell_write("\r\nRADE algorithm not enabled at application level! Set gAppUseRADEAlgorithm_d to 1!\r\n");
#endif
                }

                pEventData->eventData.algorithmSelection = algo;
                if (gBleSuccess_c != App_PostCallbackMessage(mpfBleEventHandler, pEventData))
                {
                    (void)MEM_BufferFree(pEventData);
                }
                else
                {
                    shell_write("\r\nAlgorithm set successfully.\r\n");
                }
            }
        }
    }
    else
    {
        shell_write("\r\nUsage: setalgo [0-3].\n\rAlgorithm selection value: BIT0 - CDE; BIT1 - RADE\r\n");
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Parses a string input interpreting its content as a hex number and
*               writes the value at the input address.
*
* \param[in/out]    pInput    Pointer to string to be parsed.
*
* \return       uint8_t   Returns the size of the resulted uint value/array.
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
            value = BleApp_AsciiToHex(pInput + 2, FLib_StrLen(pInput+2));

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

/**!************************************************************************************************
 * \brief  Converts a string into an integer.
 *
 * \param [in]    pStr       pointer to string
 *
 * \retval     int32_t       integer converted from string.
 * ************************************************************************************************/
static int32_t BleApp_atoi
(
    char *pStr
)
{
    int32_t res = 0;
    bool_t bIsNegative = FALSE;

    if (*pStr == '-')
    {
        bIsNegative = TRUE;
        pStr++;
    }

    while ((*pStr != '\0') && (*pStr != ' ') && (*pStr >= '0') && (*pStr <= '9'))
    {
        res = res * 10 + *pStr - '0';
        pStr++;
    }

    if (bIsNegative)
    {
        res = -res;
    }

    return res;
}
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */
