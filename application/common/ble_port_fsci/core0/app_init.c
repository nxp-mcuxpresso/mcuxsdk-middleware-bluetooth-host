/*! *********************************************************************************
* Copyright 2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
/* Framework / Drivers */
#include "EmbeddedTypes.h"

#include "fsl_component_mem_manager.h"
#include "fsl_component_messaging.h"

#include "fsl_os_abstraction.h"
#include "fwk_platform.h"

/* Application */
#include "ble_general.h"
#include "ble_port_fsci.h"

#if (defined gFsciOverRpmsgBridge_c) && (gFsciOverRpmsgBridge_c > 0U)
#include "fwk_platform_ble.h"
#endif

/*************************************************************************************
**************************************************************************************
* Private macros
*************************************************************************************/
/* Application Events */
#define gAppEvtAppCallback_c            (1U << 0U)

/* HSDK OSA event */
OSA_EVENT_HANDLE_DEFINE(mAppEvent);

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************/
/* Application input queues */
static messaging_t mAppCbInputQueue;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn           void APP_Init(void)
*\brief        Initializes application specific functionality before the BLE stack init.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void APP_InitQueue(void)
{
    /* Create application event */
    (void)OSA_EventCreate(mAppEvent, TRUE);

    /* Prepare callback input queue.*/
    MSG_QueueInit(&mAppCbInputQueue);
}

/*! *********************************************************************************
*\fn           void BluetoothLEHost_HandleMessages(void)
*\brief        This function is responsible for consuming all events coming from the
*              Bluetooth LE stack.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void App_HandleMessages(void)
{
#ifdef SDK_OS_FREE_RTOS
    osa_event_flags_t event = 0U;
    (void)OSA_EventWait((osa_event_handle_t)mAppEvent,
                        osaEventFlagsAll_c,
                        FALSE,
                        gAppTaskWaitTimeout_ms_c ,
                        &event);

#endif /* SDK_OS_FREE_RTOS */

    /* Check for existing messages in queue */
    if (MSG_QueueGetHead(&mAppCbInputQueue) != NULL)
    {
        /* Pointer for storing the callback messages. */
        appMsgCallback_t *pMsgIn = MSG_QueueGetHead(&mAppCbInputQueue);

        if (pMsgIn != NULL)
        {
            /* Execute callback handler */
            if (pMsgIn->handler != NULL)
            {
                pMsgIn->handler(pMsgIn->param);
            }

            /* Messages must always be freed. */
            (void)MSG_Free(pMsgIn);
        }
    }

#ifdef SDK_OS_FREE_RTOS
    /* Signal the main_thread again if there are more messages pending */
    event = (MSG_QueueGetHead(&mAppCbInputQueue) != NULL) ? gAppEvtAppCallback_c : 0U;

    if (event != 0U)
    {
        (void)OSA_EventSet((osa_event_handle_t)mAppEvent, gAppEvtAppCallback_c);
    }
#endif /* SDK_OS_FREE_RTOS */
}

/*! *********************************************************************************
*\fn           void App_IsMessagePending(void)
*\brief        This function checks whether Messages are pending to be processed.
*
*\param  [in]  none.
*
*\retval       TRUE if pending messages.
********************************************************************************** */
bool_t App_IsMessagePending(void)
{
    bool ret = FALSE;
     /* Check for existing messages in queue */
    if (MSG_QueueGetHead(&mAppCbInputQueue) != NULL)
    {
        ret = TRUE;
    }
    return ret;
}

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
bleResult_t App_PostCallbackMessage
(
    appCallbackHandler_t   handler,
    appCallbackParam_t     param
)
{
    appMsgCallback_t *pMsgIn = NULL;

    /* Allocate a buffer with enough space to store the packet */
    pMsgIn = MSG_Alloc(sizeof (appMsgCallback_t));

    if (pMsgIn == NULL)
    {
        return gBleOutOfMemory_c;
    }

    pMsgIn->handler = handler;
    pMsgIn->param = param;

    /* Put message in the Cb App queue */
    (void)MSG_QueueAddTail(&mAppCbInputQueue, pMsgIn);

    /* Signal application */
    (void)OSA_EventSet(mAppEvent, gAppEvtAppCallback_c);

    return gBleSuccess_c;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* @}
********************************************************************************** */
