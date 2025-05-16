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
#if defined (gFsciComponentShellCore0_d) && (gFsciComponentShellCore0_d > 0)
/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "fsl_shell.h"
#include "fsl_shell_core0.h"
#include "ble_port_fsci_op.h"
#include "ble_port_fsci_types.h"
#include "FsciInterface.h"
#include "fsl_component_mem_manager.h"
#include "FunctionLib.h"
#include "app.h"

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
#define SHELL_MAX_COMMANDS              (20U)

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
SHELL_HANDLE_DEFINE(g_shellHandle);
static uint8_t *mpStrings[SHELL_MAX_COMMANDS] = {0};
static shell_command_t aCmds[SHELL_MAX_COMMANDS] = {0};

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static uint8_t mCrtCmd = 0; /*!< Last index of the mpStrings used entry */

/************************************************************************************
*************************************************************************************
* Private functions declarations
*************************************************************************************
************************************************************************************/
static shell_status_t SHELL_LocalHandler(shell_handle_t shellHandle, int32_t argc, char * argv[]);
static void SHELL_RemoteHandler(uint8_t opc, uint8_t len, void *pData);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\brief  Initialize all shell handlers
*
*\param  none
*
*\retval    none
********************************************************************************** */
void Shell_InitCore0Handlers(void)
{
    BLE_PortFsciRegisterOpHandler((uint8_t)g_SHELL_Init_c,
                                  SHELL_RemoteHandler);
    BLE_PortFsciRegisterOpHandler((uint8_t)g_SHELL_RegisterCommand_c,
                                  SHELL_RemoteHandler);
    BLE_PortFsciRegisterOpHandler((uint8_t)g_SHELL_PrintfSynchronization_c,
                                  SHELL_RemoteHandler);
    BLE_PortFsciRegisterOpHandler((uint8_t)g_SHELL_PrintPrompt_c,
                                  SHELL_RemoteHandler);
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\brief  Local shell command handler send the command to radio core for processing
*
*\param  [in]  shellHandle    Shell handle used
*\param  [in]  argc           Argument count
*\param  [in]  argv           Argument list
*
*\retval       kStatus_SHELL_Success
********************************************************************************** */
static shell_status_t SHELL_LocalHandler(shell_handle_t shellHandle, int32_t argc, char * argv[])
{
    fsciPortRegisterCmdData_t *pOutput;
    uint32_t argvLength = 0;
    uint32_t idx = 0;

    /* Compute the total length of the arguments (including NULL-termination
      characters) */
    for(idx=0;idx<argc;idx++)
    {
        argvLength += FLib_StrLen(argv[idx]) + 1U;
    }

    pOutput = MEM_BufferAlloc(sizeof(fsciPortRegisterCmdData_t) + argvLength);
    if (pOutput != NULL)
    {
        uint8_t *pPos = NULL;
        pOutput->argc = (uint8_t)argc;

        /* Copy all arguments */
        pPos = pOutput->aArgs;
        for(idx=0; idx<argc; idx++)
        {
            uint32_t argLength = FLib_StrLen(argv[idx]);
            FLib_MemCpy(pPos, argv[idx], argLength + 1U);
            pPos += argLength + 1U;
        }

        /* Search for this command index in mpStrings */
        for(idx=0; idx<mCrtCmd; idx++)
        {
            if (FLib_MemCmp(argv[0], mpStrings[idx], FLib_StrLen(argv[0])) == TRUE)
            {
                pOutput->id = (uint8_t)idx;
                break;
            }
        }
    }

    /* Send command to core 1 */
    FSCI_transmitPayload(BLE_PORT_FSCI_OG, (uint8_t)g_SHELL_Command_c,
                       (void*)pOutput, (uint16_t)(sizeof(fsciPortRegisterCmdData_t) +
                       argvLength), gFsciInterface_c);
    (void)MEM_BufferFree(pOutput);

    return kStatus_SHELL_Success;
}

/*! *********************************************************************************
*\brief        Handle SHELL_RemoteHandler call from the remote core
*
*\param  [in]  opc      Opcode of the FSCI command to be treated
*\param  [in]  len      Command payload length
*\param  [in]  pData    Pointer to command payload
*
*\retval       kStatus_SHELL_Success
********************************************************************************** */
static void SHELL_RemoteHandler(uint8_t opc, uint8_t len, void *pData)
{
    switch (opc)
    {
        case (uint8_t)g_SHELL_Init_c:
        {
            uint32_t promptLength = FLib_StrLen(pData);
            void *pPrompt = MEM_BufferAlloc(promptLength);
            if (pPrompt != NULL)
            {
                FLib_MemCpy(pPrompt, pData, promptLength + 1U);
                (void)SHELL_Init((shell_handle_t)g_shellHandle,
                                 (serial_handle_t)gSerMgrIf, pPrompt);
            }
        }
        break;

        /* cmd ID | cmd null terminated | help string null terminated */
        case (uint8_t)g_SHELL_RegisterCommand_c:
        {
            uint8_t *pPos = (uint8_t*)pData;
            uint8_t *pCmd = NULL;
            uint8_t *pHelp = NULL;
            uint32_t cmdLength = 0;
            uint32_t helpLength = 0;
            shell_command_t *pShellCmd = &aCmds[mCrtCmd];

            pShellCmd->cExpectedNumberOfParameters = *pPos;
            pPos++;
            pCmd = pPos;
            cmdLength = FLib_StrLen((char const*)pCmd);
            pPos += cmdLength + 1U;
            pHelp = pPos;
            helpLength = FLib_StrLen((char const*)pPos);
            pShellCmd->pFuncCallBack = SHELL_LocalHandler;

            mpStrings[mCrtCmd] = MEM_BufferAlloc(cmdLength + 1U + helpLength + 1U);
            if (mpStrings[mCrtCmd] != NULL)
            {
                pShellCmd->pcCommand = (char const*)mpStrings[mCrtCmd];
                FLib_MemCpy(mpStrings[mCrtCmd], pCmd, cmdLength + 1U);
                pShellCmd->pcHelpString = (char *)(pShellCmd->pcCommand + cmdLength + 1U);
                FLib_MemCpy(pShellCmd->pcHelpString, pHelp, helpLength + 1U);
                (void)SHELL_RegisterCommand((shell_handle_t)g_shellHandle,
                                            (void*)pShellCmd);

                FSCI_transmitPayload(BLE_PORT_FSCI_OG, (uint8_t)g_SHELL_RegisterCommand_c,
                  NULL, 0, gFsciInterface_c);
                mCrtCmd++;
            }
        }
        break;

        case (uint8_t)g_SHELL_PrintfSynchronization_c:
            (void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, "%s",
                                        pData);
        break;

        case (uint8_t)g_SHELL_PrintPrompt_c:
            SHELL_PrintPrompt((shell_handle_t)g_shellHandle);
        break;

        default:
        {
            ;
        }
        break;
    }
}
#endif /* gFsciComponentShellCore0_d */
