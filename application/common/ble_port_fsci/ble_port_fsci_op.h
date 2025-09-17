/*! *********************************************************************************
* Copyright 2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _BLE_PORT_FSCI_OP_
#define _BLE_PORT_FSCI_OP_

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "EmbeddedTypes.h"

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
#define gFsciInterface_c        0
#define BLE_PORT_FSCI_OG        0x50

/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */
typedef enum
{
    /* LED */
    g_LedStartFlashingAllLeds_c = 0x00,         /* Start flashing all LEDs command */
    g_LedStopFlashingAllLeds_c,                 /* Stop flashing all LEDs command */
    g_Led1Flashing_c,                           /* Start flashing LED1 command */
    g_Led1On_c,                                 /* Turn on LED1 command */
    g_Led2Flashing_c,                           /* Start flashing LED2 command */
    g_Led2On_c,                                 /* Turn on LED2 command */
    g_Led1Off_c,                                /* Turn off LED1 command */
    /* Button */
    g_BUTTON_InstallCallback_c,                 /* Install button callback command */
    /* Platform */
    g_PlatformInitLclOpCode_c,                  /* PLATFORM_InitLcl() command */
    g_PlatformInitLclGpioDebugOpCode_c,         /* PLATFORM_InitLclGpioDebug() command */
    g_AppReadPublicDeviceAddress_c,             /* Read public device address command */
    g_AppWritePublicDeviceAddress_c,            /* Write public device address command */
    /* Shell */
    g_SHELL_Init_c,                             /* SHELL_Init() command */
    g_SHELL_PrintfSynchronization_c,            /* SHELL_PrintfSynchronization() command */
    g_SHELL_PrintPrompt_c,                      /* SHELL_PrintPrompt() command */
    g_SHELL_RegisterCommand_c,                  /* SHELL_RegisterCommand() register new command */
    g_SHELL_Command_c,                          /* Register callback for the new shell command  */
    /* Adapter Reset */
    g_HAL_ResetMCU_c,                           /* HAL_ResetMCU() command */
    /* NVM */
    g_AppBleNvmCbCmdErase_c,                    /* Erase data on the host processor command */
    g_AppBleNvmCbCmdWrite_c,                    /* Write data to NVM on the host processor command */
    g_AppBleNvmCbCmdRead_c,                     /* Read NVM data from the host processor*/
    g_AppBleNvmCbCmdEraseInd_c,                 /* Indication from host processor that the erase operation was performed */
    g_AppBleNvmCbCmdWriteInd_c,                 /* Indication from host processor that the write operation was performed */
    g_AppBleNvmCbCmdReadInd_c,                  /* Indication from host processor that the read operation was performed */
    /* NvFormat */
    g_AppBleNvFormatCommand_c,                  /* NvFormat() command */
    /* Sensors */
    g_SensorsTriggerTemperatureMeasurement_c,   /* SENSORS_TriggerTemperatureMeasurement command */
    g_SensorsRefreshTemperatureValue_c,         /* SENSORS_RefreshTemperatureValue command */
} blePortFsciOpCodes_t;

/* FSCI callback handler prototype */
typedef void (*pfFsciPortOpHandler_t)(uint8_t opc, uint8_t len, void *pData);

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\fn            void (uint8_t op, pfFsciPortOpHandler_t pfHandler)
*\brief         Ftunction to register a callback for the given opcode
*
*\param[in]     op          The opcode to which the callback corresponds
*\param[in]     pfHandler   Callback to be invoked when the corresponding command is received
*
*\retval        void.
********************************************************************************** */
void BLE_PortFsciRegisterOpHandler
(
    uint8_t op,
    pfFsciPortOpHandler_t pfHandler
);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif