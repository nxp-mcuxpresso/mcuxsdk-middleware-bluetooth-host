/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file
*
* This is a source file for the common application NVM code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#if defined (gFsciComponentLedCore0_d) && (gFsciComponentLedCore0_d > 0)
/*! *********************************************************************************
*************************************************************************************
* Includes
*************************************************************************************
********************************************************************************** */
#include "ble_port_fsci_op.h"
#include "fsl_component_led_core0.h"
#include "FsciInterface.h"
#include "app.h"

/*! *********************************************************************************
*************************************************************************************
* Private function prototypes
*************************************************************************************
********************************************************************************** */
static void LedStartFlashingAllLedsHandler(uint8_t opc, uint8_t len, void *pData);
static void LedStopFlashingAllLedsHandler(uint8_t opc, uint8_t len, void *pData);
static void Led1FlashingHandler(uint8_t opc, uint8_t len, void *pData);

/*! *********************************************************************************
*\brief  Initialize all LED handlers
*
*\param  none
*
*\retval    none
********************************************************************************** */
void LED_InitCore0Handlers(void)
{
    BLE_PortFsciRegisterOpHandler((uint8_t)g_LedStartFlashingAllLeds_c, LedStartFlashingAllLedsHandler);
    BLE_PortFsciRegisterOpHandler((uint8_t)g_LedStopFlashingAllLeds_c, LedStopFlashingAllLedsHandler);
    BLE_PortFsciRegisterOpHandler((uint8_t)g_Led1Flashing_c, Led1FlashingHandler);
}

/*! *********************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
*\brief     Start flashing all LEDs command handler.
*
*\retval    none
********************************************************************************** */
static void LedStartFlashingAllLedsHandler(uint8_t opc, uint8_t len, void *pData)
{
    LedStartFlashingAllLeds();
}

/*! *********************************************************************************
*\brief     Stop flashing all LEDs command handler.
*
*\retval    none
********************************************************************************** */
static void LedStopFlashingAllLedsHandler(uint8_t opc, uint8_t len, void *pData)
{
    LedStopFlashingAllLeds();
}

/*! *********************************************************************************
*\brief     LED1 flashing command handler.
*
*\retval    none
********************************************************************************** */
static void Led1FlashingHandler(uint8_t opc, uint8_t len, void *pData)
{
    Led1Flashing();
}
#endif