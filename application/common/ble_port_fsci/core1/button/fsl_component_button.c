/*! *********************************************************************************
* Copyright 2025-2026 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "fsl_component_button.h"
#include "ble_port_fsci_op.h"
#include "FsciInterface.h"

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
typedef struct _button_state
{
    button_callback_t callback;
    void *callbackParam;
} button_state_t;

typedef struct _fsciPortButtonData_t
{
    uint8_t id;
    button_event_t event;
} fsciPortButtonData_t;

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
/*Define button handle*/
BUTTON_HANDLE_ARRAY_DEFINE(g_buttonHandle, gAppButtonCnt_c);
#endif /*gAppButtonCnt_c > 0*/

/************************************************************************************
*************************************************************************************
* Private functions declarations
*************************************************************************************
************************************************************************************/
/* Button Handler for events coming from the remote core */
static void ButtonsHandler
(
    uint8_t opc,
    uint8_t len,
    void *pData
);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\brief        Installs a button callback and callback parameter.
*
*\param  [in]  buttonHandle     Button handle pointer
*\param  [in]  callback         The callback function.
*\param  [in]  callbackParam    The parameter of the callback function.
*
*\retval    kStatus_BUTTON_Success    Successfully install the callback
********************************************************************************** */
button_status_t BUTTON_InstallCallback
(
    button_handle_t buttonHandle,
    button_callback_t callback,
    void *callbackParam
)
{
    button_state_t *buttonState;
    uint8_t buttonId;

    assert(buttonHandle);

    buttonState = (button_state_t *)buttonHandle;
    buttonState->callback      = callback;
    buttonState->callbackParam = callbackParam;

    const uint8_t *pHandleBytes = (const uint8_t *)buttonHandle;
    const uint8_t *pBaseBytes   = (const uint8_t *)(const void *)g_buttonHandle;
    ptrdiff_t      handleOffset = pHandleBytes - pBaseBytes;

    buttonId = 0U;
    if (handleOffset > 0)
    {
        buttonId = (uint8_t)(((size_t)handleOffset / BUTTON_HANDLE_SIZE) & 0xFFU);
    }

    FSCI_transmitPayload(BLE_PORT_FSCI_OG, (uint8_t)g_BUTTON_InstallCallback_c,
                         (void*)&buttonId, (uint16_t)sizeof(buttonId), gFsciInterface_c);

    BLE_PortFsciRegisterOpHandler((uint8_t)g_BUTTON_InstallCallback_c, ButtonsHandler);

    return kStatus_BUTTON_Success;
}

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn           static void ButtonsHandler(uint8_t opc, uint8_t len, void *pData)
*\brief        Button Handler for events coming from the remote core
*
*\param  [in]  opc      Opcode of the FSCI command to be treated
*\param  [in]  len      Command payload length
*\param  [in]  pData    Pointer to command payload
*
*\retval    none
********************************************************************************** */
static void ButtonsHandler
(
    uint8_t opc,
    uint8_t len,
    void *pData
)
{
   (void)len;
   (void)opc;

    fsciPortButtonData_t *pButtonData = (fsciPortButtonData_t *)(void *)pData;

    button_handle_t buttonHandle;
    button_state_t *buttonState;
    button_callback_message_t msg;

    buttonHandle = &g_buttonHandle[pButtonData->id];
    buttonState = (button_state_t *)buttonHandle;
    msg.event = pButtonData->event;

    (void)buttonState->callback(buttonHandle, &msg, buttonState->callbackParam);
}
