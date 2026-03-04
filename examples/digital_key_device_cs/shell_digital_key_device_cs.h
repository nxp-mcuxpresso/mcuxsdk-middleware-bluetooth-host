/*! *********************************************************************************
 * \defgroup Digital Key Device CS Shell
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* \file shell_digital_key_device_cs.h
*
* Copyright 2022 - 2026 NXP
*
* NXP Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from NXP.
********************************************************************************** */

#ifndef SHELL_DIGITAL_KEY_CS_H
#define SHELL_DIGITAL_KEY_CS_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "app_localization.h"

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/
#if !defined(gAppUseShellInApplication_d) || (defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 0))
    #define shell_write(function, ...)
    #define shell_writeDec(function, ...)
    #define shell_writeHex(function, ...)
    #define shell_writeHexLe(function, ...)
    #define SHELL_NEWLINE(function, ...)
    #define shell_writeN(function, ...)
    #define shell_cmd_finished(function, ...)
    #define shell_init(function, ...)
    #define shell_register_function(function, ...)
    #define shell_refresh(function, ...)
    #define kStatus_SHELL_Success   0
#else
    extern SHELL_HANDLE_DEFINE(g_shellHandle);

    #define shell_write(a)       (void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, a)
    #define SHELL_NEWLINE()      (void)SHELL_WriteSynchronization((shell_handle_t)g_shellHandle, "\r\n", 2U)
    #define shell_writeN(a,b)    (void)SHELL_WriteSynchronization((shell_handle_t)g_shellHandle, a, b)
    #define shell_writeDec(a)    (void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, (const char*)FORMAT_Dec2Str(a))
    #define shell_writeBool(a)   if(a){(void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, "TRUE");}else{(void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, "FALSE");}
    #define shell_writeHex       BleApp_PrintHex
    #define shell_writeHexLe     BleApp_PrintHexLe
    #define shell_cmd_finished() SHELL_PrintPrompt((shell_handle_t)g_shellHandle)
#endif

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef struct appBondingData_tag
{
    uint8_t nvmIndex;
    uint8_t addrType;
    uint8_t deviceAddr[6];
    uint8_t aLtk[16];
    uint8_t aIrk[16];
}appBondingData_t;

typedef struct appCsConfigParams_tag
{
    uint8_t     mainModeType;
    uint8_t     subModeType;
    uint8_t     mainModeMinSteps;
    uint8_t     mainModeMaxSteps;
    uint8_t     mainModeRepetition;
    uint8_t     mode0Steps;
    uint8_t     role; /*!< 0 - Initiator, 1 - Reflector. */
    uint8_t     RTTType;
    uint8_t     channelMap[APP_LOCALIZATION_CH_MAP_LEN];
    uint8_t     channelMapRepetition;
    uint8_t     channelSelectionType;
    uint8_t     csSyncPhy;
} appCsConfigParams_t;

typedef struct appCsProcedureParams_tag
{
    uint16_t    maxProcedureDuration;
    uint16_t    minPeriodBetweenProcedures;
    uint16_t    maxPeriodBetweenProcedures;
    uint16_t    maxNumProcedures;
    uint32_t    minSubeventLen;
    uint32_t    maxSubeventLen;
    uint8_t     antCfgIndex;
    uint8_t     snrControlInit;
    uint8_t     snrControlRefl;
} appCsProcedureParams_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void AppShellInit(char* prompt);

#ifdef __cplusplus
}
#endif


#endif /* SHELL_DIGITAL_KEY_CS_H */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */