/*
 * Copyright 2020-2023, 2026 NXP
 *
 * NXP Proprietary
 *
 * This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or
 * by downloading, installing, activating and/or otherwise using the software, you are
 * agreeing that you have read, and that you agree to comply with and are bound by,
 * such license terms. If you do not agree to be bound by the applicable license terms,
 * then you may not retain, install, activate or otherwise use the software.
 */

/* Prevent double inclusion */
#ifndef _ISP_CLI_H_
#define _ISP_CLI_H_

/* === Includes ============================================================= */
#include <stdint.h>
#include <stdarg.h>

#include "fsl_component_button.h"
#include "fsl_component_serial_manager.h"

#include "isp_configuration.h"
#include "isp_ranging_engine.h"
#include "wireless_ranging_service.h"

/* === Macros =============================================================== */
#define CLI_PRINTF(...) cli_printf(__VA_ARGS__)
#define CLI_PRINTF_DBG(...) cli_dprintf((uint8_t)eVerbosityDebug, __VA_ARGS__)
#define CLI_PRINTF_INFO(...) cli_dprintf((uint8_t)eVerbosityInfo, __VA_ARGS__)

/* === Types ================================================================ */


/* === Externals ============================================================ */
/*isp cli serial manager write handle*/
extern SERIAL_MANAGER_WRITE_HANDLE_DEFINE(g_ispCliWriteHandle);

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
extern button_status_t BleApp_HandleKeys0(void *buttonHandle, button_callback_message_t *message,void *callbackParam);
extern button_status_t BleApp_HandleKeys1(void *buttonHandle, button_callback_message_t *message,void *callbackParam);
#endif /*gAppButtonCnt_c > 0*/

void isp_cli_powerup(void);
void cli_printf(const char *fmt_s, ...);
void cli_dprintf(uint8_t level, const char *fmt_s, ...);
void cli_print_measurement(isp_meas_response_t *meas_response, engine_response_t *engine_response, wrs_ServiceInfo_t *serviceInfo);
uint32_t CRC32( uint32_t WrsDataCRC32, const unsigned char *buf, size_t  size);
#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* _ISP_CLI_H_ */

/* EOF */
