/*! *********************************************************************************
* Copyright 2025-2026 NXP
*
* \file app_localization_data_export.h
*
* This is a source file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef __APP_LOCALIZATION_DATA_EXPORT_H__
#define __APP_LOCALIZATION_DATA_EXPORT_H__

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "stdint.h"
#include "stdio.h"
#include "app_localization_algo.h"
#include "app_localization.h"

#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
#include "fsl_shell.h"
#endif

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#if !defined(gAppUseShellInApplication_d) || (defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 0))
    #define shell_data_export_writeN(a,b) 
#else
    extern SHELL_HANDLE_DEFINE(g_shellHandle);

    #define shell_data_export_writeN(a,b)    (void)SHELL_WriteSynchronization((shell_handle_t)g_shellHandle, a, b)
#endif

#ifndef gAppLocDataExport_d
#define gAppLocDataExport_d             0 /* 0: No localization data exporting; 1: Export CS config data + Ranging results; 2: Export CS config + PBR data + ToF data + ranging results */
#endif /* gAppLocDataExport_d */
/************************************************************************************
 *************************************************************************************
 *  Public prototypes
 *************************************************************************************
 ************************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
void app_print_cs_data(isp_meas_response_t *meas_response, engine_response_t *engine_response, appLocalization_rangeCfg_t *ranging_cfg);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* __APP_LOCALIZATION_DATA_EXPORT_H__ */