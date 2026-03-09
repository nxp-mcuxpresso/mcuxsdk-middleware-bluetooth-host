/*
 * Copyright 2021-2023, 2026 NXP
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
#ifndef _ISP_DEBUG_HELPERS_H_
#define _ISP_DEBUG_HELPERS_H_

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wformat"
#endif

/* === Includes ============================================================= */
#include "EmbeddedTypes.h"

/* === Macros =============================================================== */

#ifndef gSocEventlogSize2Log_c
#define gSocEventlogSize2Log_c 5
#endif

#define DPRINTF(...) isp_soc_printf(__VA_ARGS__)
#define DPRINTF_DBG(...) isp_soc_dprintf((uint8_t)eVerbosityDebug, __VA_ARGS__)
#define DPRINTF_INFO(...) isp_soc_dprintf((uint8_t)eVerbosityInfo, __VA_ARGS__)

#if defined(NDEBUG)
#define DEBUG_PIN0_SET
#define DEBUG_PIN0_CLR
#define DEBUG_PIN0_TGL
#define DEBUG_PIN0_PLS
#else
#define DEBUG_PIN0_SET {GPIOC->PSOR = 0x10;} // set PTC4
#define DEBUG_PIN0_CLR {GPIOC->PCOR = 0x10;} // clear PTC4
#define DEBUG_PIN0_TGL {GPIOC->PTOR = 0x10;} // toggle PTC4
#define DEBUG_PIN0_PLS {DEBUG_PIN0_TGL DEBUG_PIN0_TGL} // pulse on PTC4
#endif

/* === Types ================================================================ */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

void isp_soc_debug_set(uint32_t on);
void isp_soc_debug_init(void);

void isp_soc_printf(const char *fmt_s, ...);
void isp_soc_dprintf(uint8_t level, const char *fmt_s, ...);

void isp_soc_eventlog(uint32_t event);
void isp_soc_eventlog_print(void);

uint32_t isp_soc_timestamp_get(void);
uint32_t isp_soc_timestamp_difference_us(uint32_t t0, uint32_t t1);
uint32_t isp_soc_timestamp_difference_ms(uint32_t t0, uint32_t t1);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* _ISP_DEBUG_HELPERS_H_ */
