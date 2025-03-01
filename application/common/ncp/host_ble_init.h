/*! *********************************************************************************
* Copyright 2022-2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef NCP_HOST_H
#define NCP_HOST_H

#include "ble_general.h"
/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef void* appCallbackParam_t;
typedef void (*appCallbackHandler_t)(appCallbackParam_t param);
typedef struct appMsgCallback_tag
{
    appCallbackHandler_t   handler;
    appCallbackParam_t     param;
} appMsgCallback_t;
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

/*! *********************************************************************************
*\fn           void BluetoothLEHost_HandleMessages(void)
*\brief        This function is responsible for consuming all events coming from the
*              Bluetooth LE stack.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void App_HandleMessages(void);

/*! *********************************************************************************
*\fn           void BluetoothLEHost_AppInit(void)
*\brief        Initializes application specific functionality before the BLE stack init.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void BluetoothLEHost_AppInit(void);

#if (defined gFsciOverRpmsgBridge_c) && (gFsciOverRpmsgBridge_c > 0U)
/*! *********************************************************************************
*\fn           void BluetoothLEHost_AppInitController(void)
*\brief        Initializes controller specific functionality.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void BluetoothLEHost_AppInitController(void);
#endif

/*! *********************************************************************************
*\fn           void App_IsMessagePending(void)
*\brief        This function checks whether Messages are pending to be processed.
*
*\param  [in]  none.
*
*\retval       TRUE if pending messages.
********************************************************************************** */
bool App_IsMessagePending(void);

/*! *********************************************************************************
*\fn           void vApplicationIdleHook(void)
*\brief        Idle hook function which places the microcontroller into a power
*              saving mode.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void vApplicationIdleHook(void);

/*! *********************************************************************************
\fn            bleResult_t App_PostCallbackMessage(
*                  appCallbackHandler_t   handler,
*                  appCallbackParam_t     param
               )
*\brief        Store a callback message in the Cb App queue and signal application.
*
*\param  [in]  handler              Callback handler.
*\param  [in]  param                Callback parameter.
*
*\retval       gBleOutOfMemory_c    Message allocation fail.
*\retval       gBleSuccess_c        Successful addition to the Cb App queue.
********************************************************************************** */
bleResult_t App_PostCallbackMessage(appCallbackHandler_t handler, appCallbackParam_t param);

/*! *********************************************************************************
*\fn            void APP_FscitransmitPayload(uint8_t OG,
*                                                   uint8_t OC,
*                                                   const uint8_t *pMsg,
*                                                   uint16_t msgLen)
*\brief         Send FSCI commands to NCP over RPMSG.
*
* \param[in]    OG operation Group
* \param[in]    OC operation Code
* \param[in]    pMsg pointer to payload
* \param[in]    msgLen length of the payload
* \param[in]    fsciInterface the interface on which the packet should be sent
*
*\retval        void
********************************************************************************** */
void APP_FscitransmitPayload(uint8_t OG, uint8_t OC, const uint8_t *pMsg, uint16_t msgLen);

#ifdef __cplusplus
}
#endif


#endif /* NCP_HOST_H */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
