/*! *********************************************************************************
* Copyright 2025 - 2026 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef _BLE_PORT_FSCI_H_
#define _BLE_PORT_FSCI_H_

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "ble_port_fsci_op.h"

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
extern uint32_t mFsciInterfaceId;

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\fn           void BLE_PortFsciInit(void)
*\brief        Initializes application specific functionality before the BLE stack init.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void BLE_PortFsciInit(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/************************************************************************************
*************************************************************************************
* Core 0 only
*************************************************************************************
************************************************************************************/
#ifndef NBU_PROJECT_ENABLE

#include "ble_general.h"

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

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\fn           void APP_Init(void)
*\brief        Initializes application specific functionality before the BLE stack init.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void APP_InitQueue(void);

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
*\fn           void App_IsMessagePending(void)
*\brief        This function checks whether Messages are pending to be processed.
*
*\param  [in]  none.
*
*\retval       TRUE if pending messages.
********************************************************************************** */
bool_t App_IsMessagePending(void);

/*! *********************************************************************************
*\brief        Store a callback message in the Cb App queue and signal application.
*
*\param  [in]  handler              Callback handler.
*\param  [in]  param                Callback parameter.
*
*\retval       gBleOutOfMemory_c    Message allocation fail.
*\retval       gBleSuccess_c        Successful addition to the Cb App queue.
********************************************************************************** */
bleResult_t App_PostCallbackMessage
(
    appCallbackHandler_t   handler,
    appCallbackParam_t     param
);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* NBU_PROJECT_ENABLE */

/************************************************************************************
*************************************************************************************
* Core 1 only
*************************************************************************************
************************************************************************************/
#ifdef NBU_PROJECT_ENABLE
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\fn           void BLE_PortFsciBlock(uint8_t evtFlag)
*\brief        Block core 1 waiting for the corresponding event to be set after a message
*              is received from core 0
*
*\param  [in]  evtFlag        Event flag to wait for
*
*\retval       void.
********************************************************************************** */
void BLE_PortFsciBlock(uint8_t evtFlag);

/*! *********************************************************************************
*\fn           void BLE_PortFsciUnblock(uint8_t operation)
*\brief        Unblock core 1 after receiving a message from core 0
*
*\param  [in]  operation    Blocking operation from which to free core 1
*
*\retval       void.
********************************************************************************** */
void BLE_PortFsciUnblock(uint8_t operation);

/*! *********************************************************************************
*\fn           uint8_t BLE_PortFsciRegisterBlockingEvent(void)
*\brief        Register a index to be used to unblock Core 0 for a specific blocking operation
*
*\param  [in]  none
*
*\retval       Index to be used when calling BLE_PortFsciUnblock()
********************************************************************************** */
uint8_t BLE_PortFsciRegisterBlockingEvent(void);

/*! *********************************************************************************
*\fn           void BLE_PortFsciRxCallBack(uint8_t packetType, uint8_t *data, uint16_t len)
*\brief        RPMSG callback function. Transfers the message to the FSCI module.
*
*\param  [in]  packetType   Packet type (ignored)
*\param  [in]  data         Pointer to the FSCI message
*\param  [in]  len          Message length
*
*\retval       void.
********************************************************************************** */
void BLE_PortFsciRxCallBack(uint8_t packetType, uint8_t *data, uint16_t len);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* NBU_PROJECT_ENABLE */
#endif /* _BLE_PORT_FSCI_H_ */
