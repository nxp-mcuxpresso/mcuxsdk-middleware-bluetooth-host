/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file
*
* This is a source file for the common application NVM code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef __LED_CORE1_H__
#define __LED_CORE1_H__

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ble_port_fsci_op.h"
#include "FsciInterface.h"

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
#if (defined(gAppLedCnt_c) && (gAppLedCnt_c > 0))
/* Start flashing all LEDs.*/
#define LedStartFlashingAllLeds(void)         FSCI_transmitPayload(BLE_PORT_FSCI_OG,\
                                                  (uint8_t)g_LedStartFlashingAllLeds_c, NULL, 0, gFsciInterface_c);
/* Stop flashing all LEDs */
#define LedStopFlashingAllLeds(void)          FSCI_transmitPayload(BLE_PORT_FSCI_OG,\
                                                  (uint8_t)g_LedStopFlashingAllLeds_c, NULL, 0, gFsciInterface_c);
/* Trun on LED1 */
#define Led1On(void)                          FSCI_transmitPayload(BLE_PORT_FSCI_OG,\
                                                  (uint8_t)g_Led1On_c, NULL, 0, gFsciInterface_c);
/* Trun on LED2 */
#define Led2On(void)                          FSCI_transmitPayload(BLE_PORT_FSCI_OG,\
                                                  (uint8_t)g_Led2On_c, NULL, 0, gFsciInterface_c);
/* Start flashing LED1 */
#define  Led1Flashing(void)                   FSCI_transmitPayload(BLE_PORT_FSCI_OG,\
                                                  (uint8_t)g_Led1Flashing_c, NULL, 0, gFsciInterface_c);
/* Start flashing LED2 */
#define Led2Flashing(void)                    FSCI_transmitPayload(BLE_PORT_FSCI_OG,\
                                                  (uint8_t)g_Led2Flashing_c, NULL, 0, gFsciInterface_c);
/* Trun off LED1 */
#define Led1Off(void)                         FSCI_transmitPayload(BLE_PORT_FSCI_OG,\
                                                  (uint8_t)g_Led1Off_c, NULL, 0, gFsciInterface_c);
#else
/* Start flashing all LEDs.*/
#define LedStartFlashingAllLeds(void)
/* Stop flashing all LEDs */
#define LedStopFlashingAllLeds(void)
/* Trun on LED1 */
#define Led1On(void)
/* Trun on LED2 */
#define Led2On(void)
/* Start flashing LED1 */
#define  Led1Flashing(void)
/* Start flashing LED2 */
#define Led2Flashing(void)
/* Trun off LED1 */
#define Led1Off(void)
#endif /* gAppLedCnt_c > 0 */

#endif /* __LED_CORE1_H__ */