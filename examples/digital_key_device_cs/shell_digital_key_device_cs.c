/*! *********************************************************************************
* \addtogroup Digital Key Device CS Shell
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file shell_digital_key_device_cs.c
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
* \brief        Set CS Create Config default parameters.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellSetCsConfigParams_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    bleResult_t status = gBleSuccess_c;

    if (argc == 13)
    {
        deviceId_t deviceId = (uint8_t)BleApp_atoi(argv[1]);

        if (deviceId > gAppMaxConnections_c)
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

                    pAppCsConfigParams->mainModeType = (uint8_t)BleApp_atoi(argv[2]);

                    if ((pAppCsConfigParams->mainModeType == 0U) || (pAppCsConfigParams->mainModeType > 3U))
                    {
                        status = gBleInvalidParameter_c;
                    }

                    if (status == gBleSuccess_c)
                    {
                        pAppCsConfigParams->subModeType = (uint8_t)BleApp_atoi(argv[3]);

                        if ((pAppCsConfigParams->subModeType == 0U) || (pAppCsConfigParams->subModeType > 3U))
                        {
                            status = gBleInvalidParameter_c;
                        }
                    }

                    if (status == gBleSuccess_c)
                    {
                        pAppCsConfigParams->mainModeMinSteps = (uint8_t)BleApp_atoi(argv[4]);
                        pAppCsConfigParams->mainModeMaxSteps = (uint8_t)BleApp_atoi(argv[5]);
                        pAppCsConfigParams->mainModeRepetition = (uint8_t)BleApp_atoi(argv[6]);

                        if (pAppCsConfigParams->mainModeRepetition > 3U)
                        {
                            status = gBleInvalidParameter_c;
                        }
                    }

                    if (status == gBleSuccess_c)
                    {
                        pAppCsConfigParams->mode0Steps = (uint8_t)BleApp_atoi(argv[7]);

                        if ((pAppCsConfigParams->mode0Steps == 0U) || (pAppCsConfigParams->mode0Steps > 3U))
                        {
                            status = gBleInvalidParameter_c;
                        }
                    }

                    if (status == gBleSuccess_c)
                    {
                        pAppCsConfigParams->role = (uint8_t)BleApp_atoi(argv[8]);

                        if (pAppCsConfigParams->role > 1U)
                        {
                            status = gBleInvalidParameter_c;
                        }
                    }

                    if (status == gBleSuccess_c)
                    {
                        pAppCsConfigParams->RTTType = (uint8_t)BleApp_atoi(argv[9]);

                        if (pAppCsConfigParams->RTTType > 6U)
                        {
                            status = gBleInvalidParameter_c;
                        }
                    }

                    if (status == gBleSuccess_c)
                    {
                        if (gHCICSChannelMapSize ==  BleApp_ParseHexValue(argv[10]))
                        {
                            FLib_MemCpy(pAppCsConfigParams->channelMap, argv[10], APP_LOCALIZATION_CH_MAP_LEN);
                        }
                        else
                        {
                            status = gBleInvalidParameter_c;
                        }
                    }

                    if (status == gBleSuccess_c)
                    {
                        pAppCsConfigParams->channelMapRepetition = (uint8_t)BleApp_atoi(argv[11]);

                        if (pAppCsConfigParams->channelMapRepetition == 0U)
                        {
                            status = gBleInvalidParameter_c;
                        }
                    }

                    if (status == gBleSuccess_c)
                    {
                        pAppCsConfigParams->channelSelectionType = (uint8_t)BleApp_atoi(argv[12]);

                        if (pAppCsConfigParams->channelSelectionType > 1U)
                        {
                            status = gBleInvalidParameter_c;
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
        shell_write("\r\nUsage: \
                    \r\nsetcsconfig peer_id, main_mode_type, sub_mode_type, main_mode_min_steps, main_mode_max_steps, main_mode_repetition, mode_0_steps, role, rtt_types, chann_map, chan_map_repetition, chan_sel_type \
                    \r\n");
    }

    if (status == gBleOutOfMemory_c)
    {
        shell_write("\r\nOut of memory\r\n");
    }
    if (status == gBleInvalidParameter_c)
    {
        shell_write("\r\nInvalid parameter\r\n");
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Set CS Procedure default parameters.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellSetCsProcedureParams_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    bleResult_t status = gBleSuccess_c;

    if (argc == 9)
    {
        deviceId_t deviceId = (uint8_t)BleApp_atoi(argv[1]);

        if (deviceId > gAppMaxConnections_c)
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

                    pAppCsProcedureParams->maxProcedureDuration = (uint16_t)BleApp_atoi(argv[2]);

                    if (pAppCsProcedureParams->maxProcedureDuration == 0U)
                    {
                        status = gBleInvalidParameter_c;
                    }

                    if (status == gBleSuccess_c)
                    {
                        pAppCsProcedureParams->minPeriodBetweenProcedures = (uint16_t)BleApp_atoi(argv[3]);
                        pAppCsProcedureParams->maxPeriodBetweenProcedures = (uint16_t)BleApp_atoi(argv[4]);
                        pAppCsProcedureParams->maxNumProcedures = (uint16_t)BleApp_atoi(argv[5]);
                        pAppCsProcedureParams->minSubeventLen = (uint32_t)BleApp_atoi(argv[6]);
                        pAppCsProcedureParams->maxSubeventLen = (uint32_t)BleApp_atoi(argv[7]);
                        pAppCsProcedureParams->antCfgIndex = (uint8_t)BleApp_atoi(argv[8]);

                        if (pAppCsProcedureParams->antCfgIndex > 7U)
                        {
                            status = gBleInvalidParameter_c;
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
                        shell_write("\r\nProcedure parameters set successfully.\r\n");
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
        shell_write("\r\nUsage: \
                    \r\nsetcsproc peer_id, max_proc_duration, min_period_between_proc, max_period_between_proc, max_num_proc, min_subevent_len, max_subevent_len, ant_config_idx \
                    \r\n");
    }

    if (status == gBleOutOfMemory_c)
    {
        shell_write("\r\nOut of memory\r\n");
    }
    if (status == gBleInvalidParameter_c)
    {
        shell_write("\r\nInvalid parameter\r\n");
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
    if ((uint32_t)argc == 2U)
    {
        deviceId_t deviceId = (uint8_t)BleApp_AsciiToHex(argv[1], FLib_StrLen(argv[1]));

        if (deviceId < (uint8_t)gAppMaxConnections_c)
        {
            if(mpfBleEventHandler != NULL)
            {
                appEventData_t *pEventData = MEM_BufferAlloc(sizeof(appEventData_t));

                if(pEventData != NULL)
                {
                    pEventData->appEvent = mAppEvt_Shell_TriggerCsDistanceMeasurement_Command_c;
                    pEventData->peerDeviceId = deviceId;

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
                        \r\ntdm peer_id \
                        \r\n");
        }
    }
    else
    {
        shell_write("\r\nUsage: \
                    \r\ntdm peer_id \
                    \r\n");
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

                if (sizeof(uint16_t) != BleApp_ParseHexValue(argv[2]) ||
                    value == 0U)
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
        shell_write("\r\nUsage: setnumprocs peer_id [0x0001-0xffff].\r\n");
    }

    if (status == gBleOutOfMemory_c)
    {
        shell_write("\r\nOut of memory\r\n");
    }
    if (status == gBleInvalidParameter_c)
    {
        shell_write("\r\nInvalid parameter. \
                     \r\nUsage: setnumprocs peer_id [0x0001-0xffff].\r\n");
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
