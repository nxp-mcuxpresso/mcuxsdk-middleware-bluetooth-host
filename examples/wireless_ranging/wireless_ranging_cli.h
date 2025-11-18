/*
 * Copyright 2020-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
