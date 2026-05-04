/*! *********************************************************************************
* Copyright 2026 NXP
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
#ifndef APP_LOCALIZATION_DEBUG_H
#define APP_LOCALIZATION_DEBUG_H
/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
/* Debug logging configuration - disabled by default */
#ifndef gAppCsDebugLogging_d
#define gAppCsDebugLogging_d                (0U)
#endif

/* Debug logging verbosity levels */
#define gCsLogLevelNone_c                   (0x00U)
#define gCsLogLevelError_c                  (0x01U)
#define gCsLogLevelWarning_c                (0x02U)
#define gCsLogLevelInfo_c                   (0x04U)
#define gCsLogLevelDebug_c                  (0x08U)
#define gCsLogLevelVerbose_c                (0x10U)

/* Default log level mask when logging is enabled */
#ifndef gAppCsDebugLogLevel_d
#define gAppCsDebugLogLevel_d               (gCsLogLevelError_c | gCsLogLevelWarning_c | gCsLogLevelInfo_c)
#endif

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

/*
 * Debug Logging Macros
 * Disabled by default. Enable by setting gAppCsDebugLogging_d to 1.
 */
#if defined(gAppCsDebugLogging_d) && (gAppCsDebugLogging_d == 1U)

extern uint32_t g_shellHandle[];
#include "fsl_shell.h"

/* Wrapper macro that calls shell print directly */
#define CS_DebugPrint(fmt, ...)             (void)SHELL_PrintfSynchronization(g_shellHandle, fmt, ##__VA_ARGS__)

/* Compile-time conditional logging macros */
#if ((gAppCsDebugLogLevel_d & gCsLogLevelError_c) != 0U)
#define CS_LOG_ERROR(fmt, ...)              CS_DebugPrint("CS_ERR: " fmt "\r\n", ##__VA_ARGS__)
#else
#define CS_LOG_ERROR(fmt, ...)              ((void)0)
#endif

#if ((gAppCsDebugLogLevel_d & gCsLogLevelWarning_c) != 0U)
#define CS_LOG_WARNING(fmt, ...)            CS_DebugPrint("CS_WRN: " fmt "\r\n", ##__VA_ARGS__)
#else
#define CS_LOG_WARNING(fmt, ...)            ((void)0)
#endif

#if ((gAppCsDebugLogLevel_d & gCsLogLevelInfo_c) != 0U)
#define CS_LOG_INFO(fmt, ...)               CS_DebugPrint("CS_INF: " fmt "\r\n", ##__VA_ARGS__)
#else
#define CS_LOG_INFO(fmt, ...)               ((void)0)
#endif

#if ((gAppCsDebugLogLevel_d & gCsLogLevelDebug_c) != 0U)
#define CS_LOG_DEBUG(fmt, ...)              CS_DebugPrint("CS_DBG: " fmt "\r\n", ##__VA_ARGS__)
#else
#define CS_LOG_DEBUG(fmt, ...)              ((void)0)
#endif

#if ((gAppCsDebugLogLevel_d & gCsLogLevelVerbose_c) != 0U)
#define CS_LOG_VERBOSE(fmt, ...)            CS_DebugPrint("CS_VRB: " fmt "\r\n", ##__VA_ARGS__)
#else
#define CS_LOG_VERBOSE(fmt, ...)            ((void)0)
#endif

/* Category-specific logging macros */
#define CS_LOG_SUBEVENT(fmt, ...)           CS_LOG_INFO("[SUBEVT] " fmt, ##__VA_ARGS__)
#define CS_LOG_RAS(fmt, ...)                CS_LOG_INFO("[RAS] " fmt, ##__VA_ARGS__)
#define CS_LOG_BTCS(fmt, ...)               CS_LOG_INFO("[BTCS] " fmt, ##__VA_ARGS__)

#else /* gAppCsDebugLogging_d disabled */

#define CS_LOG_ERROR(fmt, ...)              ((void)0)
#define CS_LOG_WARNING(fmt, ...)            ((void)0)
#define CS_LOG_INFO(fmt, ...)               ((void)0)
#define CS_LOG_DEBUG(fmt, ...)              ((void)0)
#define CS_LOG_VERBOSE(fmt, ...)            ((void)0)

#define CS_LOG_SUBEVENT(fmt, ...)           ((void)0)
#define CS_LOG_RAS(fmt, ...)                ((void)0)
#define CS_LOG_BTCS(fmt, ...)               ((void)0)

#endif /* gAppCsDebugLogging_d */

#endif /* APP_LOCALIZATION_DEBUG_H */