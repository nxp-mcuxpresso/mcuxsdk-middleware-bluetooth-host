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


/* === Includes ============================================================ */
#include <stdio.h>
#include <stdarg.h>

#include "isp_debug_helpers.h"
#include "isp_interface.h"
#include "wireless_ranging.h"
#include "fsl_os_abstraction.h"
#include "clock_config.h"
#include "fwk_platform.h"

/* === Types =============================================================== */
typedef struct soc_eventlog_tag {
    uint32_t timestamps[1<<gSocEventlogSize2Log_c];
    uint32_t events[1<<gSocEventlogSize2Log_c];
    uint8_t idx;
    uint8_t idx_mask;
} soc_eventlog_t;

/* === Macros ============================================================== */
#define DTEST_RSM0                   		(0x04)
#define DTEST_RSM1                   		(0x05)
#define DTEST_RSM2                   		(0x06)
#define DTEST_XCVR_DMA                   	(0x07)
#define DTEST_RX_IQ                   		(0x17)
#define DTEST_NBU_LL                   		(0x43)

#define MUXMASK     (~PORT_PCR_MUX_MASK)
#define MUXVAL      (PORT_PCR_MUX(8))

/* DTEST signal selectors for use with dtest_init() */
#define DTEST0          (1UL<<0U)
#define DTEST1          (1UL<<1U)
#define DTEST2          (1UL<<2U)
#define DTEST3          (1UL<<3U)
#define DTEST4          (1UL<<4U)
#define DTEST5          (1UL<<5U)
#define DTEST6          (1UL<<6U)
#define DTEST7          (1UL<<7U)
#define DTEST8          (1UL<<8U)
#define DTEST9          (1UL<<9U)
#define DTEST10         (1UL<<10U)
#define DTEST11         (1UL<<11U)
#define DTEST12         (1UL<<12U)
#define DTEST13         (1UL<<13U)
#define DTEST_ALL       0x1FFFU

/* DTEST1 conflicts with PTC1 (SW2) on localization board */
#if ((defined(BOARD_LOCALIZATION_REVISION_SUPPORT)) && BOARD_LOCALIZATION_REVISION_SUPPORT)
#undef DTEST1
#define DTEST1 0UL
#endif

/*
 * DTEST2 & DTEST3 conflict with UART1
 * DTEST6 conflicts with PTC6 (SW3)
 * DTEST7 (aa_matched_to_ll in DTEST_RSM2) conflicts with PTB0 = monochrome LED (LED2)
 * PTA19, PTA20, PTA21 (RGB LED) conflicts with RF_GPO (antenna switching)
 */
#if (defined(DEFAULT_APP_UART) && (DEFAULT_APP_UART == 0))
#define DTEST_LIST_DBG_OFF (DTEST0 | DTEST1 | DTEST2 | DTEST3 | DTEST4 | DTEST5 | /*DTEST7 |*/ DTEST8 | DTEST9 | DTEST10 | DTEST11 | DTEST12 | DTEST13)
#define DTEST_LIST_DBG_ON (DTEST0 | DTEST1 | DTEST2 | DTEST3 | DTEST4 | DTEST5 | DTEST7 | DTEST8 | DTEST9 | DTEST10 | DTEST11 | DTEST12 | DTEST13)
#else
#define DTEST_LIST_DBG_OFF (DTEST0 | DTEST1 | /*DTEST2 | DTEST3 |*/ DTEST4 | DTEST5 | /*DTEST7 |*/ DTEST8 | DTEST9 | DTEST10 | DTEST11 | DTEST12 | DTEST13)
#define DTEST_LIST_DBG_ON (DTEST0 | DTEST1 | /*DTEST2 | DTEST3 |*/ DTEST4 | DTEST5 | DTEST7 | DTEST8 | DTEST9 | DTEST10 | DTEST11 | DTEST12 | DTEST13)
#endif

#define DTEST_OVRD_TSM_COMBO_RX_EN  0x5U

/* === Globals ============================================================= */
static soc_eventlog_t soc_eventlog;

/* === Externals =========================================================== */

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */
#if !(defined(CPU_KW45B41Z82AFPA_NBU) || defined(CPU_KW45B41Z82AFTA_NBU) || defined(CPU_KW45B41Z83AFPA_NBU) || defined(CPU_KW45B41Z83AFTA_NBU))
void isp_dtest_init(uint32_t option)
{
    uint32_t temp;
    if ((option & (DTEST0 | DTEST1 | DTEST2 | DTEST3 | DTEST4 | DTEST5 | DTEST6 | DTEST13)) != 0U)
    {
        CLOCK_EnableClock(kCLOCK_PortC);
    }
    if ((option & (DTEST7 | DTEST8 | DTEST9 | DTEST3 | DTEST10 | DTEST11 | DTEST12)) != 0U)
    {
        CLOCK_EnableClock(kCLOCK_PortB);
    }
    if ((option & DTEST0) != 0U) /* PTC0 alt 8 */
    {
        temp = PORTC->PCR[0];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTC->PCR[0] = temp;
    }
    if ((option & DTEST1) != 0U) /* PTC1 alt 8 */
    {
        temp = PORTC->PCR[1];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTC->PCR[1] = temp;
    }
    if ((option & DTEST2) != 0U) /* PTC2 alt 8 */
    {
        temp = PORTC->PCR[2];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTC->PCR[2] = temp;
    }
    if ((option & DTEST3) != 0U) /* PTC3 alt 8 */
    {
        temp = PORTC->PCR[3];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTC->PCR[3] = temp;
    }
    if ((option & DTEST4) != 0U) /* PTC4 alt 8 */
    {
        temp = PORTC->PCR[4];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTC->PCR[4] = temp;
    }
    if ((option & DTEST5) != 0U) /* PTC5 alt 8 */
    {
        temp = PORTC->PCR[5];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTC->PCR[5] = temp;
    }
    if ((option & DTEST6) != 0U) /* PTC6 alt 8 */
    {
        temp = PORTC->PCR[6];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTC->PCR[6] = temp;
    }
    if ((option & DTEST7) != 0U) /* PTB0 alt 8*/
    {
        temp = PORTB->PCR[0];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTB->PCR[0] = temp;
    }
    if ((option & DTEST8) != 0U) /* PTB1 alt 8 */
    {
        temp = PORTB->PCR[1];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTB->PCR[1] = temp;
    }
    if ((option & DTEST9) != 0U) /* PTB2 alt 8 */
    {
        temp = PORTB->PCR[2];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTB->PCR[2] = temp;
    }
    if ((option & DTEST10) != 0U) /* PTB3 alt 8 */
    {
        temp = PORTB->PCR[3];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTB->PCR[3] = temp;
    }
    if ((option & DTEST11) != 0U) /* PTB4 alt 8 */
    {
        temp = PORTB->PCR[4];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTB->PCR[4] = temp;
    }
    if ((option & DTEST12) != 0U) /* PTCB5 alt 8  */
    {
        temp = PORTB->PCR[5];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTB->PCR[5] = temp;
    }
    if ((option & DTEST13) != 0U) /* PTC7 alt 8 */
    {
        temp = PORTC->PCR[7];
        temp &= MUXMASK;
        temp |= MUXVAL;
        PORTC->PCR[7] = temp;
    }
}
#endif

void isp_soc_debug_set(uint32_t on)
{
    if (on == 0) {
        isp_dtest_init(DTEST_LIST_DBG_OFF);
    } else {
        isp_dtest_init(DTEST_LIST_DBG_ON);
    }
}

void isp_soc_debug_init(void)
{
    soc_eventlog.idx_mask = (1<<gSocEventlogSize2Log_c) - 1;
    soc_eventlog.idx = 0;
    
    uint32_t temp, dtest_page = DTEST_RSM2; //DTEST_RX_IQ;
    /* Set the DTEST page selection and DTEST enable */
    temp = RADIO_CTRL->DTEST_CTRL;
    temp &= ~(RADIO_CTRL_DTEST_CTRL_DTEST_PAGE_MASK | RADIO_CTRL_DTEST_CTRL_DTEST_EN_MASK);
    temp |= RADIO_CTRL_DTEST_CTRL_DTEST_PAGE(dtest_page) | RADIO_CTRL_DTEST_CTRL_DTEST_EN(1);
    RADIO_CTRL->DTEST_CTRL = temp;

    isp_soc_debug_set(0);

    /* Override pin 11 with tsm_combined_rx_en */ 
    RADIO_CTRL->DTEST_PIN_CTRL2 &= ~RADIO_CTRL_DTEST_PIN_CTRL2_DTEST_PIN11_OVRD_SEL_MASK;
    RADIO_CTRL->DTEST_PIN_CTRL2 |= RADIO_CTRL_DTEST_PIN_CTRL2_DTEST_PIN11_OVRD_SEL(DTEST_OVRD_TSM_COMBO_RX_EN);
}

/**
 * Get time stamp timer value.
 * Use time stamp timer, because polling GENFSK_EVENT_TMR occupies XCVR bus
*/
uint32_t isp_soc_timestamp_get(void)
{
    uint64_t timeStamp;
    OSA_InterruptDisable();
    timeStamp = PLATFORM_GetTimeStamp();
    OSA_InterruptEnable();
    return (uint32_t)timeStamp;
}

/**
 * Calculate difference between two time stamp values.
 */
uint32_t isp_soc_timestamp_difference_us(uint32_t t0, uint32_t t1) {
    return (t1 - t0);
}

uint32_t isp_soc_timestamp_difference_ms(uint32_t t0, uint32_t t1) {
    return (isp_soc_timestamp_difference_us(t0, t1) >> 10U);
}

void isp_soc_printf(const char *fmt_s, ...)
{
    va_list args;

    va_start (args, fmt_s);
    vprintf (fmt_s, args);
    va_end (args);
}

void isp_soc_dprintf(uint8_t level, const char *fmt_s, ...)
{
    va_list args;

    if((gLocalSettings->verbosity & level) == 0)
        return;
    if(level & eVerbosityDebug)
        printf("DBG: ");
    va_start (args, fmt_s);
    vprintf (fmt_s, args);
    va_end (args);
}

void isp_soc_eventlog(uint32_t event)
{
    OSA_InterruptDisable();
    soc_eventlog.timestamps[soc_eventlog.idx] = isp_soc_timestamp_get();
    soc_eventlog.events[soc_eventlog.idx] = event;
    soc_eventlog.idx++;
    soc_eventlog.idx &= soc_eventlog.idx_mask;
    OSA_InterruptEnable();
}

void isp_soc_eventlog_print(void)
{
    uint8_t x, i, j;
    for(x = 0; x <= soc_eventlog.idx_mask; x++)
    {
        i = soc_eventlog.idx - x - 1;
        i &= soc_eventlog.idx_mask;
        j = i - 1;
        j &= soc_eventlog.idx_mask;
        DPRINTF("0x%08x +%7luus\n", (unsigned int)soc_eventlog.events[i], isp_soc_timestamp_difference_us(soc_eventlog.timestamps[j], soc_eventlog.timestamps[i]));
    }
}
