#ifndef _BLE_PORT_FSCI_H_
#define _BLE_PORT_FSCI_H_

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ble_general.h"

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
extern uint32_t mFsciInterfaceId;

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
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn           void BLE_PortFsciInit(void)
*\brief        Initializes application specific functionality before the BLE stack init.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void BLE_PortFsciInit(void);

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

/*! *********************************************************************************
*\fn           void App_IsMessagePending(void)
*\brief        This function checks whether Messages are pending to be processed.
*
*\param  [in]  none.
*
*\retval       TRUE if pending messages.
********************************************************************************** */
bool_t App_IsMessagePending(void);

#endif /* _BLE_PORT_FSCI_H_*/