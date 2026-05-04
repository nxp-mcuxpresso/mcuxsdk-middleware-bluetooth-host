/*! *********************************************************************************
 * \defgroup App Digital Key Car CS Anchor
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* \file app_digital_key_car_anchor_cs.h
*
* Copyright 2022 - 2026 NXP
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

#ifndef APP_DIGITAL_KEY_CAR_ANCHOR_CS_H
#define APP_DIGITAL_KEY_CAR_ANCHOR_CS_H

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "gap_types.h"
#include "ble_general.h"

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/
#define L2CAP_SAMPLE_MESSAGE                                     "H E L L O"

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d == 1)
/* Id of the device selected for handover */
extern deviceId_t gHandoverDeviceId;
#endif

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
#if defined(gAppUseShellInApplication_d) && (gAppUseShellInApplication_d == 1)
void App_HandleShellCmds(void *pData);
#endif
void APP_BleEventHandler(void *pData);
#if (defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U))
void BleApp_A2BEventHandler(appA2BEvent_t eventType, void *pData);
#endif
void APP_UserInterfaceEventHandler(void *pData);

#ifdef __cplusplus
}
#endif

#endif /* APP_DIGITAL_KEY_CAR_ANCHOR_CS_H */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
