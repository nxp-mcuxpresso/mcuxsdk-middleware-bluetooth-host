/*! *********************************************************************************
* Copyright 2025 NXP
*
*
* \file
*
* This is the source file for the main entry point for a bare-metal application.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "fsl_shell.h"
#include <stdarg.h>
#include "fsl_str.h"
#include "ble_port_fsci_op.h"
#include "ble_port_fsci_types.h"
#include "FsciInterface.h"
#include "FunctionLib.h"
#include "fsl_component_mem_manager.h"
#include "ble_port_fsci.h"

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
#define SHELL_SPRINTF_BUFFER_SIZE       (64U)
#define SHELL_MAX_COMMANDS              (20U)

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
SERIAL_MANAGER_HANDLE_DEFINE(gSerMgrIf);
/* Shell buffer */
static uint8_t mBuffer[SHELL_SPRINTF_BUFFER_SIZE] = {0};
/* Shell commands buffer */
static cmd_function_t maCommands[SHELL_MAX_COMMANDS] = {0};
static uint8_t mBlockTaskShellRegisterIdx = 0U;

/************************************************************************************
*************************************************************************************
* Private functions declarations
*************************************************************************************
************************************************************************************/
static void SHELL_WriteBuffer(char *buffer, int32_t *indicator, char val, int len);
static int SHELL_Sprintf(void *buffer, const char *formatString, va_list ap);
static void RegisterCmdCnf(uint8_t opc, uint8_t len, void *pData);
static void CmdHandler(uint8_t opc, uint8_t len, void *pData);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\brief        This function initializes the shell module.
*
*\param  [in]  shellHandle    Shell handle to be used
*\param  [in]  serialHandle   Serila handle to be used
*\param  [in]  prompt         Prompt to be printed after initialization
*
*\retval       kStatus_SHELL_Success.
*
*\remarks      This function must be called before calling all other Shell functions
********************************************************************************** */
shell_status_t SHELL_Init
(
    shell_handle_t shellHandle,
    serial_handle_t serialHandle,
    char *prompt
)
{
    (void)shellHandle;
    (void)serialHandle;

    mBlockTaskShellRegisterIdx = BLE_PortFsciRegisterBlockingEvent();

    FSCI_transmitPayload(BLE_PORT_FSCI_OG, g_SHELL_Init_c, (uint8_t const*)prompt,
                         strlen(prompt) + 1, gFsciInterface_c);

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
*\brief        This function is used to register the shell command
*
*\param  [in]  shellHandle    Shell handle to be used
*\param  [in]  shellCommand   The command element
*
*\retval       kStatus_SHELL_Success or kStatus_SHELL_Error
********************************************************************************** */
shell_status_t SHELL_RegisterCommand
(
    shell_handle_t shellHandle,
    shell_command_t *shellCommand
)
{
    shell_status_t status = kStatus_SHELL_Error;
    uint8_t *pTemp = NULL;
    uint8_t *pPos = NULL;
    uint32_t cmdLength = 0;
    uint32_t helpLength = 0;
    static uint8_t mCrtCmd = 0;

    /* Save command and helper string */
    cmdLength = FLib_StrLen(shellCommand->pcCommand);
    helpLength = FLib_StrLen(shellCommand->pcHelpString);

    pTemp = MEM_BufferAlloc(1 + cmdLength + 1 + helpLength + 1);
    if (pTemp != NULL)
    {
        maCommands[mCrtCmd] = shellCommand->pFuncCallBack;

        /* Build and message to register the command to the host processor */
        pPos = pTemp;
        pPos[0] = shellCommand->cExpectedNumberOfParameters;
        pPos++;
        FLib_MemCpy(pPos, shellCommand->pcCommand, cmdLength + 1);
        pPos += cmdLength + 1;
        FLib_MemCpy(pPos, shellCommand->pcHelpString, helpLength + 1);

        FSCI_transmitPayload(BLE_PORT_FSCI_OG, g_SHELL_RegisterCommand_c, pTemp,
                             1 + cmdLength + 1 + helpLength + 1,
                             gFsciInterface_c);

        if (mCrtCmd < SHELL_MAX_COMMANDS)
        {
            mCrtCmd++;

            BLE_PortFsciRegisterOpHandler(g_SHELL_RegisterCommand_c,
                                          RegisterCmdCnf);

            /* Block here waiting for the other core to reply */
            BLE_PortFsciBlock(mBlockTaskShellRegisterIdx);

            BLE_PortFsciRegisterOpHandler(g_SHELL_Command_c,
                                          CmdHandler);

            status = kStatus_SHELL_Success;
        }

        MEM_BufferFree(pTemp);
    }

    return status;
}

/*! *********************************************************************************
*\brief        Writes formatted output to the shell output stream with OS synchronization
*
*\param  [in]  shellHandle    Shell handle to be used
*\param  [in]  formatString   Format string
*
*\retval       kStatus_SHELL_Success.
********************************************************************************** */
int SHELL_PrintfSynchronization(shell_handle_t shellHandle, const char *formatString, ...)
{
    va_list ap;
    uint32_t length;

    va_start(ap, formatString);
    length = SHELL_Sprintf(mBuffer, formatString, ap);
    mBuffer[length] = 0;
    FSCI_transmitPayload(BLE_PORT_FSCI_OG, g_SHELL_PrintfSynchronization_c,
                         (uint8_t const*)mBuffer, length + 1, gFsciInterface_c);
    va_end(ap);

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
*\brief        Print shell prompt
*
*\param  [in]  shellHandle    Shell handle to be used
*
*\retval       none
********************************************************************************** */
void SHELL_PrintPrompt(shell_handle_t shellHandle)
{
    FSCI_transmitPayload(BLE_PORT_FSCI_OG, g_SHELL_PrintPrompt_c,
                         NULL, 0, gFsciInterface_c);
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\brief        Writes the given data in mBuffer
*
*\param  [in]  buffer       Buffer in which to write (unused)
*\param  [in]  indicator    Index at which to write
*\param  [in]  val          Value to be written
*\param  [in]  len          Length of the data to be written
*
*\retval       none.
********************************************************************************** */
static void SHELL_WriteBuffer
(
    char *buffer,
    int32_t *indicator,
    char val,
    int len
)
{
    int i = 0;

    for (i = 0; i < len; i++)
    {
        mBuffer[*indicator] = val;
        (*indicator)++;
    }
}

/*! *********************************************************************************
*\brief        Writes the given data in mBuffer
*
*\param  [in]  buffer         Buffer in which to write
*\param  [in]  formatString   Format string
*
*\retval       Length of the written data.
********************************************************************************** */
static int SHELL_Sprintf(void *buffer, const char *formatString, va_list ap)
{
    uint32_t length = 0;

    length = (uint32_t)StrFormatPrintf(formatString, ap, (char *)buffer, SHELL_WriteBuffer);
    return (int32_t)length;
}

/*! *********************************************************************************
*\brief        Handle command registration complete indication from host processor
*
*\param  [in]  opc      FSCI packet opcode.
*\param  [in]  len      FSCI packet len.
*\param  [in]  pParam   FSCI packet payload.
*
*\retval       void.
********************************************************************************** */
static void RegisterCmdCnf
(
    uint8_t opc,
    uint8_t len,
    void *pData
)
{
    /* Unblock core */
    BLE_PortFsciUnblock(mBlockTaskShellRegisterIdx);
}

/*! *********************************************************************************
*\brief        Handle command from host processor: parse ID | argc | args
*
*\param  [in]  opc      FSCI packet opcode.
*\param  [in]  len      FSCI packet len.
*\param  [in]  pData    FSCI packet payload.
*
*\retval       void.
********************************************************************************** */
static void CmdHandler(uint8_t opc, uint8_t len, void *pData)
{
    union {
        fsciPortRegisterCmdData_t *pData;
        void *pRawData;
    } temp = {0};
    char *argv[10] = {0};
    uint32_t i = 0;
    char *pPos = NULL;

    temp.pRawData = pData;
    pPos = (char*)temp.pData->aArgs;
    while(i <  temp.pData->argc)
    {
        argv[i] = pPos;
        pPos += FLib_StrLen(pPos) + 1;
        i++;
    }

    maCommands[temp.pData->id](NULL, temp.pData->argc, argv);
}