/*! *********************************************************************************
* \addtogroup Localization Reader application
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file shell_loc_reader.c
*
* Copyright 2023-2026 NXP
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
#include "fsl_adapter_reset.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_timer_manager.h"
#include "fsl_format.h"
#include "app.h"

/* BLE Host Stack */
#include "gap_interface.h"

#include "app_conn.h"
#include "loc_reader.h"
#include "shell_loc_reader.h"
#include "ranging_client_interface.h"
#include "ble_conn_manager.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#if defined(gCsDropTestEnabled_d) && (gCsDropTestEnabled_d == 1)
/* Vendor HCI command defines */
#define gHciVendorUnitaryTest_c             0x009EU
#define gForcedMissedSyncSubOpcode_c        0x0BU
#define gForcedMissedSyncPayloadLen_c       18U
#define gForcedMissedSyncTotalLen_c         (gForcedMissedSyncPayloadLen_c + gHciCommandPacketHeaderLength_c)

#define HciCommand(opCodeGroup, opCodeCommand) \
    (((uint16_t)(opCodeGroup) & (uint16_t)0x3FU) << (uint16_t)SHIFT10) | \
    (uint16_t)((opCodeCommand) & 0x3FFU)

#define Hci_CommandPacket(pHciCmdPacket, parameterTotalLength) \
    Ble_HciSend( \
        gHciCommandPacket_c, \
        (void*)(pHciCmdPacket), \
        gHciCommandPacketHeaderLength_c + (parameterTotalLength))
#endif /* gCsDropTestEnabled_d */

/* Compile-time validation */
#if (gHciCommandPacketHeaderLength_c < 3U)
#error "HCI command packet header length must be at least 3 bytes"
#endif
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
/* Shell */
static shell_status_t ShellReset_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellDisconnect_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellFactoryReset_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellStartBle_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSetCsConfigParams_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSetCsProcedureParams_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellTriggerDistanceMeasurement_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSelectAlgorithm_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSetCsRole_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSetNumProcs_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellFilter_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellListBd_Command (shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellSetMaxConcurrentProcs_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static shell_status_t ShellToggleLoop_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
static shell_status_t ShellRunTest_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */

static uint32_t BleApp_AsciiToHex(char *pString, uint32_t strLen);
static int32_t BleApp_atoi(char *pStr);
static uint8_t BleApp_ParseHexValue(char* pInput);
static void ShellResetTimeoutTimerCallback(void* pParam);

#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
static shell_status_t ShellTak_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */

#if defined(gCsDropTestEnabled_d) && (gCsDropTestEnabled_d == 1)
static shell_status_t ShellCsDrop_Command(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static bleResult_t CS_VendorUnitaryTestForceMissedSync(uint32_t ceDropNum, uint32_t ceDropDenom,
                                                        uint32_t csDropNum, uint32_t csDropDenom);
#endif /* defined(gCsDropTestEnabled_d) && (gCsDropTestEnabled_d == 1) */

#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

/************************************************************************************
*************************************************************************************
* External declarations
*************************************************************************************
************************************************************************************/
#if defined(gCsDropTestEnabled_d) && (gCsDropTestEnabled_d == 1)
extern bleResult_t Ble_HciSend
(
    hciPacketType_t packetType,
    void*           pPacket,
    uint16_t        packetSize
);
#endif /* defined(gCsDropTestEnabled_d) && (gCsDropTestEnabled_d == 1) */
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
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

static shell_command_t mSbCmd =
{
    .pcCommand = "sb",
    .pcHelpString = "\r\n\"sb\": Start BLE.\r\n",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellStartBle_Command,
};

static shell_command_t mDcntCmd =
{
    .pcCommand = "dcnt",
    .pcHelpString = "\r\n\"dcnt\": Disconnect all peers.\r\n",
    .cExpectedNumberOfParameters = 0,
    .pFuncCallBack = ShellDisconnect_Command,
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
    .pcHelpString = "\r\n\"tdm\": Trigger Channel Sounding distance measurements.\r\n",
};

static shell_command_t mSelectalgoCmd =
{
    .pcCommand = "setalgo",
    .pcHelpString = "\r\n\"setalgo\": Select the algorithm to run at the end of the CS procedure [0-3].\r\n"
                    "BIT0 - CDE; BIT1 - RADE\r\n",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellSelectAlgorithm_Command,
};

#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
static shell_command_t mRunTestCmd =
{
    .pcCommand = "ptstest",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellRunTest_Command,
    .pcHelpString = "\r\n\"ptstest\": Run ptstest test_id.\r\n",
};
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */

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

static shell_command_t mFilterCmd =
{
    .pcCommand = "filter",
    .pcHelpString = "\r\n\"filter\": Send Filter command before enabling not.ind.\r\n",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellFilter_Command,
};

static shell_command_t mListBdCmd =
{
    .pcCommand = "listbd",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellListBd_Command,
    .pcHelpString = "\r\n\"listbd\": List bonded devices information.\r\n",
};

static shell_command_t mSetMaxConcurrentProcsCmd =
{
    .pcCommand = "setmcp",
    .pcHelpString = "\r\n\"setmcp\": Set maximum number of supported concurrent CS procedures [1-6].\r\n",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellSetMaxConcurrentProcs_Command,
};

static shell_command_t mToggleLoopCmd =
{
    .pcCommand = "loop",
    .pcHelpString = "\r\n\"loop\": Toggle auto-loop of CS procedure (restart on end).\r\n",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellToggleLoop_Command,
};

#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
static shell_command_t mTakCmd =
{
    .pcCommand = "tak",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellTak_Command,
    .pcHelpString = "\r\n\"tak\": Set a Transient Application Key for a device.\r\n",
};
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */

#if defined(gCsDropTestEnabled_d) && (gCsDropTestEnabled_d == 1)
static shell_command_t mCsDropCmd =
{
    .pcCommand = "csdrop",
    .pcHelpString = "\r\n\"csdrop\": Set CS drop ratios for testing (based on mode0 drops).\r\n"
                    "  Usage: csdrop <ce_num> <ce_denom> <cs_num> <cs_denom>\r\n"
                    "  Note: Drop ratio is based on mode0 steps, NOT subevents directly.\r\n"
                    "        To drop N subevents out of M, use: cs_num = N * mode0_nb\r\n"
                    "                                           cs_denom = M * mode0_nb\r\n"
                    "  Example with mode0_nb=3:\r\n"
                    "    csdrop 0 1 3 6   -> drop 1/2 subevents (3 mode0 / 6 mode0)\r\n"
                    "    csdrop 0 1 3 9   -> drop 1/3 subevents (3 mode0 / 9 mode0)\r\n"
                    "    csdrop 0 1 3 12  -> drop 1/4 subevents (3 mode0 / 12 mode0)\r\n",
    .cExpectedNumberOfParameters = SHELL_IGNORE_PARAMETER_COUNT,
    .pFuncCallBack = ShellCsDrop_Command,
};
#endif /* defined(gCsDropTestEnabled_d) && (gCsDropTestEnabled_d == 1) */

static TIMER_MANAGER_HANDLE_DEFINE(mResetTmrId);

#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
SHELL_HANDLE_DEFINE(g_shellHandle);
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */

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
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSbCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mDcntCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSetCsConfigParamsCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSetCsProcParamsCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mTriggerCsDistMeasCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSelectalgoCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSetCsRoleCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSetNumProcsCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mFilterCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mListBdCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mSetMaxConcurrentProcsCmd);
    assert(kStatus_SHELL_Success == status);
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mToggleLoopCmd);
    assert(kStatus_SHELL_Success == status);
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mTakCmd);
    assert(kStatus_SHELL_Success == status);
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */
#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mRunTestCmd);
    assert(kStatus_SHELL_Success == status);
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */
#if defined(gCsDropTestEnabled_d) && (gCsDropTestEnabled_d == 1)
    /* Register csdrop command - TEST ONLY */
    status = SHELL_RegisterCommand((shell_handle_t)g_shellHandle, &mCsDropCmd);
    assert(kStatus_SHELL_Success == status);
#endif /* defined(gCsDropTestEnabled_d) && (gCsDropTestEnabled_d == 1) */
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
    (void)pParam;
    HAL_ResetMCU();
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
    BleApp_FactoryReset();
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
static shell_status_t ShellStartBle_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    BleApp_Start();
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
    BleApp_Disconnect();
    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Parse and validate the first group of CS Create Config parameters.
*
* \param[in]    argv           Pointer to arguments
* \param[out]   pCfg           Pointer to the configuration structure to update
*
* \return       bleResult_t    gBleSuccess_c on success, error code otherwise
********************************************************************************** */
static bleResult_t ShellSetCsConfigParams_ParseGroup1(char * argv[], appLocalization_rangeCfg_t *pCfg)
{
    bleResult_t status = gBleSuccess_c;

    pCfg->main_mode_type = (uint8_t)BleApp_atoi(argv[2]);

    if ((pCfg->main_mode_type == 0U) || (pCfg->main_mode_type > 3U))
    {
        status = gBleInvalidParameter_c;
    }

    if (status == gBleSuccess_c)
    {
        pCfg->sub_mode_type = (uint8_t)BleApp_atoi(argv[3]);

        if ((pCfg->sub_mode_type == 0U) ||
            ((pCfg->sub_mode_type > 3U) && (pCfg->sub_mode_type != 255U)))
        {
            status = gBleInvalidParameter_c;
        }
    }

    if (status == gBleSuccess_c)
    {
        pCfg->main_mode_min = (uint8_t)BleApp_atoi(argv[4]);
        pCfg->main_mode_max = (uint8_t)BleApp_atoi(argv[5]);
        pCfg->main_mode_repeat = (uint8_t)BleApp_atoi(argv[6]);

        if (pCfg->main_mode_repeat > 3U)
        {
            status = gBleInvalidParameter_c;
        }
    }

    if (status == gBleSuccess_c)
    {
        pCfg->mode0_nb = (uint8_t)BleApp_atoi(argv[7]);

        if ((pCfg->mode0_nb == 0U) || (pCfg->mode0_nb > 3U))
        {
            status = gBleInvalidParameter_c;
        }
    }

    return status;
}

/*! *********************************************************************************
* \brief        Parse and validate the second group of CS Create Config parameters.
*
* \param[in]    argv           Pointer to arguments
* \param[out]   pCfg           Pointer to the configuration structure to update
*
* \return       bleResult_t    gBleSuccess_c on success, error code otherwise
********************************************************************************** */
static bleResult_t ShellSetCsConfigParams_ParseGroup2(char * argv[], appLocalization_rangeCfg_t *pCfg)
{
    bleResult_t status = gBleSuccess_c;
    uint8_t role = (uint8_t)BleApp_atoi(argv[8]);

    if (role > 1U)
    {
        status = gBleInvalidParameter_c;
    }
    else
    {
        mGlobalRangeSettings.role = role;
    }

    if (status == gBleSuccess_c)
    {
        pCfg->rtt_type = (uint8_t)BleApp_atoi(argv[9]);

        if (pCfg->rtt_type > 6U)
        {
            status = gBleInvalidParameter_c;
        }
    }

    if (status == gBleSuccess_c)
    {
        if (gCsChannelMapLength_c ==  BleApp_ParseHexValue(argv[10]))
        {
            FLib_MemCpy(pCfg->ch_map, argv[10], gCsChannelMapLength_c);
        }
        else
        {
            status = gBleInvalidParameter_c;
        }
    }

    if (status == gBleSuccess_c)
    {
        pCfg->ch_map_repeat = (uint8_t)BleApp_atoi(argv[11]);

        if (pCfg->ch_map_repeat == 0U)
        {
            status = gBleInvalidParameter_c;
        }
    }

    if (status == gBleSuccess_c)
    {
        pCfg->channelSelectionType = (uint8_t)BleApp_atoi(argv[12]);

        if (pCfg->channelSelectionType > 1U)
        {
            status = gBleInvalidParameter_c;
        }
    }

    if (status == gBleSuccess_c)
    {
        pCfg->cs_sync_phy = (uint8_t)BleApp_atoi(argv[13]);

        if ((pCfg->cs_sync_phy < 1U) || (pCfg->cs_sync_phy > 3U))
        {
            status = gBleInvalidParameter_c;
        }
    }

    return status;
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

    if (argc == 14)
    {
        deviceId_t deviceId = (uint8_t)BleApp_atoi(argv[1]);

        if (deviceId >= (uint8_t)gAppMaxConnections_c)
        {
            status = gBleInvalidParameter_c;
        }

        if (status == gBleSuccess_c)
        {
            appLocalization_rangeCfg_t csConfigParams;

            /* Read current configuration. */
            (void)AppLocalization_ReadConfig(deviceId, &csConfigParams);

            /* Update and validate configuration with the new values. */
            status = ShellSetCsConfigParams_ParseGroup1(argv, &csConfigParams);

            if (status == gBleSuccess_c)
            {
                status = ShellSetCsConfigParams_ParseGroup2(argv, &csConfigParams);
            }

            if (status == gBleSuccess_c)
            {
                status = AppLocalization_WriteConfig(deviceId, &csConfigParams);
            }

            if (status == gBleSuccess_c)
            {
                shell_write("\r\nConfig parameters set successfully.\r\n");
            }
        }
    }
    else
    {
        shell_write("\r\nUsage: \
                    \r\nsetcsconfig peer_id, main_mode_type, sub_mode_type, main_mode_min_steps, main_mode_max_steps, main_mode_repetition, mode_0_steps, role, rtt_types, chann_map, chan_map_repetition, chan_sel_type cs_sync_phy\
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
* \brief        Parse and validate the CS Procedure parameters.
*
* \param[in]    argv           Pointer to arguments
* \param[out]   pCfg           Pointer to the configuration structure to update
*
* \return       bleResult_t    gBleSuccess_c on success, error code otherwise
********************************************************************************** */
static bleResult_t ShellSetCsProcedureParams_Parse(char * argv[], appLocalization_rangeCfg_t *pCfg)
{
    bleResult_t status = gBleSuccess_c;

    pCfg->maxProcedureDuration = (uint16_t)BleApp_atoi(argv[2]);

    if (pCfg->maxProcedureDuration == 0U)
    {
        status = gBleInvalidParameter_c;
    }

    if (status == gBleSuccess_c)
    {
        pCfg->minPeriodBetweenProcedures = (uint16_t)BleApp_atoi(argv[3]);
        pCfg->maxPeriodBetweenProcedures = (uint16_t)BleApp_atoi(argv[4]);
        pCfg->maxNumProcedures = (uint16_t)BleApp_atoi(argv[5]);
        pCfg->minSubeventLen = (uint32_t)BleApp_atoi(argv[6]);
        pCfg->maxSubeventLen = (uint32_t)BleApp_atoi(argv[7]);
        pCfg->ant_cfg_index = (uint8_t)BleApp_atoi(argv[8]);
        pCfg->snr_control_init = (uint8_t)BleApp_atoi(argv[9]);
        pCfg->snr_control_refl = (uint8_t)BleApp_atoi(argv[10]);

        if (pCfg->ant_cfg_index > 7U)
        {
            status = gBleInvalidParameter_c;
        }

        if(!isValidSnrControl(pCfg->snr_control_init) || !isValidSnrControl(pCfg->snr_control_refl))
        {
            status = gBleInvalidParameter_c;
        }
    }

    return status;
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

    if (argc == 11)
    {
        deviceId_t deviceId = (uint8_t)BleApp_atoi(argv[1]);

        if (deviceId >= (uint8_t)gAppMaxConnections_c)
        {
            status = gBleInvalidParameter_c;
        }

        if (status == gBleSuccess_c)
        {
            appLocalization_rangeCfg_t csConfigParams;

            /* Read current configuration. */
            (void)AppLocalization_ReadConfig(deviceId, &csConfigParams);

            /* Update and validate configuration with the new values. */
            status = ShellSetCsProcedureParams_Parse(argv, &csConfigParams);

            if (status == gBleSuccess_c)
            {
                status = AppLocalization_WriteConfig(deviceId, &csConfigParams);
            }

            if (status == gBleSuccess_c)
            {
                shell_write("\r\nProcedure parameters set successfully.\r\n");
            }
        }
    }
    else
    {
        shell_write("\r\nUsage: \
                    \r\nsetcsproc peer_id, max_proc_duration, min_period_between_proc, max_period_between_proc, max_num_proc, min_subevent_len, max_subevent_len, ant_config_idx snr_control_init snr_control_refl \
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
    bleResult_t status = gBleSuccess_c;

    if ((uint32_t)argc == 2U)
    {
        deviceId_t deviceId = (uint8_t)BleApp_AsciiToHex(argv[1], FLib_StrLen(argv[1]));

        if (deviceId < (uint8_t)gAppMaxConnections_c)
        {
            status = BleApp_TriggerCsDistanceMeasurement(deviceId);
        }
        else
        {
            status = gBleInvalidParameter_c;
        }
    }
    else if ((uint32_t)argc == 1U)
    {
        status = BleApp_TriggerCsDistanceMeasurement(gInvalidDeviceId_c);
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
        if ( sizeof(uint8_t) == BleApp_ParseHexValue(argv[1]) )
        {
            userSelectedAlgo = (uint8_t)*argv[1];
            /* Store verbosity level to be set in eventData.algorithmSelection */
            if (userSelectedAlgo > 3U)
            {
                shell_write("\r\nUsage: setalgo [0-3].\n\rAlgorithm selection value: BIT0 - CDE; BIT1 - RADE\r\n");
            }
            else
            {
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
                AppLocalization_SetAlgorithm(algo);

                shell_write("\r\nAlgorithm set successfully.\r\n");
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
        shell_write("\r\n Usage: role [0-1]: Set Channel Sounding role.\r\n 1 = Reflector\r\n 0 = Initiator\r\n");
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

            uint16_t value = (uint16_t)BleApp_AsciiToHex(argv[2], strlen(argv[2]));

            if (sizeof(uint16_t) != BleApp_ParseHexValue(argv[2]))
            {
                status = gBleInvalidParameter_c;
            }

            if (status == gBleSuccess_c)
            {
                AppLocalization_SetNumberOfProcedures(deviceId, value);

                shell_write("\r\nNumber of procedures set successfully.\r\n");
            }
        }
    }
    else
    {
        shell_write("\r\nUsage: setnumprocs peer_id [0x0000-0xffff].\r\n");
    }

    if (status == gBleInvalidParameter_c)
    {
        shell_write("\r\nInvalid parameter. \
                     \r\nUsage: setnumprocs peer_id [0x0000-0xffff].\r\n");
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Send Filter command before enabling notifications/indications.
*
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellFilter_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    bleResult_t status = gBleSuccess_c;
    bool_t filterSetDone = FALSE;

    if (argc == 4)
    {
        deviceId_t deviceId = (uint8_t)BleApp_AsciiToHex(argv[1], FLib_StrLen(argv[1]));

        if (sizeof(uint8_t) != BleApp_ParseHexValue(argv[1]) ||
            deviceId >= (uint8_t)gAppMaxConnections_c)
        {
            status = gBleInvalidParameter_c;
        }

        if (status == gBleSuccess_c)
        {
            if ( sizeof(uint16_t) == BleApp_ParseHexValue(argv[2]) )
            {
                gFilterShellVal = Utils_ExtractTwoByteValue(argv[2]);
                filterTestSend = (bool)BleApp_AsciiToHex(argv[3], FLib_StrLen(argv[3]));
                if ((uint8_t)filterTestSend > 1U)
                {
                    status = gBleInvalidParameter_c;
                }
                else
                {
                    (void)RasClient_RasSetFilter(deviceId, gFilterShellVal, filterTestSend, &filterSetDone);
                }
            }
        }
    }
    else
    {
        status = gBleInvalidParameter_c;
    }

    if ( status == gBleInvalidParameter_c)
    {
        shell_write("\r\n Usage: filter [deviceId] [hex_filter] [send]: Set RAS Filter.\r\n");
    }

    return kStatus_SHELL_Success;
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

static shell_status_t ShellListBd_Command (shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    BleApp_ListBondingData();

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Set the maximum number of concurrent CS procedures.
*
* \param[in]    shellHandle    Shell handle
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellSetMaxConcurrentProcs_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    bleResult_t status = gBleSuccess_c;

    if (argc == 2)
    {
        uint8_t maxProcs = (uint8_t)BleApp_atoi(argv[1]);

        if ((maxProcs == 0U) || (maxProcs > 6U))
        {
            status = gBleInvalidParameter_c;
        }
        else
        {
            AppLocalization_SetMaxNumConcurrentProcs(maxProcs);
            shell_write("\r\nMaximum concurrent procedures set successfully.\r\n");
        }
    }
    else
    {
        shell_write("\r\nUsage: setmcp [1-6]\r\n");
    }

    if (status == gBleInvalidParameter_c)
    {
        shell_write("\r\nInvalid parameter. \
                     \r\nUsage: setmcp [1-6]\r\n");
    }

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
* \brief        Toggle auto-loop of CS procedure.
*
* \param[in]    shellHandle    Shell handle
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellToggleLoop_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    static bool_t mLastValue = FALSE;

    AppLocalization_EnableProcedureRestart(!mLastValue);
    shell_write("CS procedure auto-restart: ");
    if (!mLastValue)
    {
        shell_write("ENABLED\r\n");
    }
    else
    {
        shell_write("DISABLED\r\n");
    }
    mLastValue = !mLastValue;

    return kStatus_SHELL_Success;
}

#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
/*! *********************************************************************************
 * \brief        Handles "tak" shell command.
 *
 * \param[in]    argc           Number of arguments
 * \param[in]    argv           Array of argument's values
 *
 * \return       shell_status_t Command status
 ********************************************************************************** */
static shell_status_t ShellTak_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    deviceId_t deviceId = gInvalidDeviceId_c;
    takEntry_t *pTakEntry = NULL;
    uint32_t takLength = 0;

    /* Both arguments must be provided */
    if (argc == 3U)
    {
        deviceId = (uint8_t)BleApp_atoi(argv[1]);
        takLength = BleApp_ParseHexValue(argv[2]);

        /* First check if this deviceID exist in the list */
        pTakEntry = BleConnManager_GetTak(deviceId, FALSE);
        if (pTakEntry != NULL)
        {
            FLib_MemCpy(pTakEntry->aTak, argv[2], takLength);
        }
        else
        {
            /* If this deviceID was not previously registered, try to add a new entry */
            pTakEntry = BleConnManager_GetTak(deviceId, TRUE);
            if (pTakEntry != NULL)
            {
                pTakEntry->device = deviceId;
                FLib_MemCpy(pTakEntry->aTak, argv[2], takLength);
            }
            else
            {
                shell_write("No more room for a new entry, update BLE_SHELL_MAX_TAK_ENTRIES\n\r");
            }
        }
    }
    else
    {
        shell_write("Usage: gap tak <deviceID> <Transient Application Key>\n\r");
    }

    return kStatus_SHELL_Success;
}
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */

#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
static shell_status_t ShellRunTest_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    if (argc != 2)
    {
        shell_write("\r\nUsage: ptstest test_id\r\n");
    }
    else
    {
        uint32_t argSize = FLib_StrLen(argv[1]);
        void *pArgvCopy = MEM_BufferAlloc(argSize + 1U);

        if (pArgvCopy != NULL)
        {
            FLib_MemCpy(pArgvCopy, (const void *)argv[1], argSize + 1U);

            (void)App_PostCallbackMessage(BleApp_RunPtsTest, pArgvCopy);
        }
    }

    return kStatus_SHELL_Success;
}
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */

#if defined(gCsDropTestEnabled_d) && (gCsDropTestEnabled_d == 1)
/*!*************************************************************************************************
*\fn    static bleResult_t CS_VendorUnitaryTestForceMissedSync(...)
*
*\brief Send vendor HCI command to force CS subevent drops (0xFC9E / 0x0B).
*
*\param [in]    ceDropNum         CE drop ratio numerator
*\param [in]    ceDropDenom       CE drop ratio denominator (must be > 0)
*\param [in]    csDropNum         CS drop ratio numerator
*\param [in]    csDropDenom       CS drop ratio denominator (must be > 0)
*
*\retval        gBleSuccess_c               Command sent successfully
*\retval        gBleInvalidParameter_c      Invalid parameters
*\retval        gHciTransportError_c        HCI send failed
***************************************************************************************************/
static bleResult_t CS_VendorUnitaryTestForceMissedSync
(
    uint32_t ceDropNum,
    uint32_t ceDropDenom,
    uint32_t csDropNum,
    uint32_t csDropDenom
)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t pHciPacket[gForcedMissedSyncTotalLen_c];
    assert(gForcedMissedSyncTotalLen_c == 21U);
    uint16_t opcode = HciCommand(gHciVendorSpecificDebugCommands_c, gHciVendorUnitaryTest_c);

    if ((ceDropDenom == 0U) || (csDropDenom == 0U) ||
        (ceDropNum > ceDropDenom) || (csDropNum > csDropDenom))
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        /* Build HCI packet */
        /* Bytes 0-1: Opcode (0xFC9E little-endian) */
        FLib_MemCpy((void*)pHciPacket, (const void*)&opcode, 2U);

        /* Byte 2: Length = 18 (0x12) */
        pHciPacket[2] = (uint8_t)gForcedMissedSyncPayloadLen_c;

        /* Byte 3: Subopcode (0x0B) */
        pHciPacket[3] = gForcedMissedSyncSubOpcode_c;
        
        /* Byte 4: Reserved */
        pHciPacket[4] = 0x00U;

        /* Bytes 5-8: CE Drop Num */
        pHciPacket[5]  = (uint8_t)(ceDropNum & 0xFFU);
        pHciPacket[6]  = (uint8_t)((ceDropNum >> 8U) & 0xFFU);
        pHciPacket[7]  = (uint8_t)((ceDropNum >> 16U) & 0xFFU);
        pHciPacket[8]  = (uint8_t)((ceDropNum >> 24U) & 0xFFU);

        /* Bytes 9-12: CE Drop Denom */
        pHciPacket[9]  = (uint8_t)(ceDropDenom & 0xFFU);
        pHciPacket[10] = (uint8_t)((ceDropDenom >> 8U) & 0xFFU);
        pHciPacket[11] = (uint8_t)((ceDropDenom >> 16U) & 0xFFU);
        pHciPacket[12] = (uint8_t)((ceDropDenom >> 24U) & 0xFFU);

        /* Bytes 13-16: CS Drop Num */
        pHciPacket[13] = (uint8_t)(csDropNum & 0xFFU);
        pHciPacket[14] = (uint8_t)((csDropNum >> 8U) & 0xFFU);
        pHciPacket[15] = (uint8_t)((csDropNum >> 16U) & 0xFFU);
        pHciPacket[16] = (uint8_t)((csDropNum >> 24U) & 0xFFU);

        /* Bytes 17-20: CS Drop Denom */
        pHciPacket[17] = (uint8_t)(csDropDenom & 0xFFU);
        pHciPacket[18] = (uint8_t)((csDropDenom >> 8U) & 0xFFU);
        pHciPacket[19] = (uint8_t)((csDropDenom >> 16U) & 0xFFU);
        pHciPacket[20] = (uint8_t)((csDropDenom >> 24U) & 0xFFU);

        /* Send HCI command */
        result = Hci_CommandPacket(pHciPacket, (uint16_t)pHciPacket[2]);

    }

    return result;
}

/*! *********************************************************************************
* \brief        Set CS drop ratios for testing packet loss.
*
* \param[in]    shellHandle    Shell handle
* \param[in]    argc           Number of arguments
* \param[in]    argv           Pointer to arguments
*
* \return       shell_status_t  Returns the command processing status
********************************************************************************** */
static shell_status_t ShellCsDrop_Command(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    bleResult_t result = gBleSuccess_c;
    uint32_t ceDropNum = 0U;
    uint32_t ceDropDenom = 1U;
    uint32_t csDropNum = 0U;
    uint32_t csDropDenom = 1U;

    if (argc == 5)
    {
        ceDropNum   = (uint32_t)BleApp_atoi(argv[1]);
        ceDropDenom = (uint32_t)BleApp_atoi(argv[2]);
        csDropNum   = (uint32_t)BleApp_atoi(argv[3]);
        csDropDenom = (uint32_t)BleApp_atoi(argv[4]);

        result = CS_VendorUnitaryTestForceMissedSync(ceDropNum, ceDropDenom, csDropNum, csDropDenom);

        if (result == gBleSuccess_c)
        {
            shell_write("\r\ncsdrop: CE=");
            shell_writeDec(ceDropNum);
            shell_write("/");
            shell_writeDec(ceDropDenom);
            shell_write(" CS=");
            shell_writeDec(csDropNum);
            shell_write("/");
            shell_writeDec(csDropDenom);
            shell_write(" [OK]\r\n");
        }
        else
        {
            shell_write("\r\ncsdrop: error 0x");
            shell_writeHex((uint8_t*)&result, 1);
            shell_write("\r\n");
        }
    }
    else if (argc == 1)
    {
        /* No arguments - reset to no drop */
        result = CS_VendorUnitaryTestForceMissedSync(0U, 1U, 0U, 1U);

        if (result == gBleSuccess_c)
        {
            shell_write("\r\ncsdrop: disabled [OK]\r\n");
        }
        else
        {
            shell_write("\r\ncsdrop: error 0x");
            shell_writeHex((uint8_t*)&result, 1);
            shell_write("\r\n");
        }
    }
    else
    {
        shell_write("\r\n");
        shell_write("Usage: csdrop <ce_num> <ce_denom> <cs_num> <cs_denom>\r\n");
        shell_write("       csdrop (no args = disable)\r\n");
        shell_write("\r\n");
        shell_write("NOTE: Drop ratio is based on MODE0 steps, not subevents!\r\n");
        shell_write("      Formula: cs_num = subevents_to_drop * mode0_nb\r\n");
        shell_write("               cs_denom = total_subevents * mode0_nb\r\n");
        shell_write("\r\n");
        shell_write("Examples (assuming mode0_nb = 3):\r\n");
        shell_write("  csdrop 0 1 3 6   -> drop 1/2 subevents (50%%)\r\n");
    }

    return kStatus_SHELL_Success;
}
#endif /* defined(gCsDropTestEnabled_d) && (gCsDropTestEnabled_d == 1) */
#endif /* defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1) */