/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file
*
* This is a source file for the common application NVM code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#if defined (gFsciComponentButtonCore0_d) && (gFsciComponentButtonCore0_d > 0)
/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "fsl_component_button.h"
#include "fsl_component_button_core0.h"
#include "ble_port_fsci_op.h"
#include "FsciInterface.h"
#include "FsciCommunication.h"
#include "app.h"

/*! *********************************************************************************
*************************************************************************************
* Public types
*************************************************************************************
********************************************************************************** */
typedef struct _fsciPortButtonData_t
{
    uint8_t id;
    button_event_t event;
} fsciPortButtonData_t;

/*! *********************************************************************************
*************************************************************************************
* Private function prototypes
*************************************************************************************
********************************************************************************** */
/* Button event handler */
static button_status_t KeysHandler(
    void *buttonHandle,
    button_callback_message_t *message,
    void *callbackParam
);

/* Handle BUTTON_InstallCallback call from the remote core */
static void BUTTON_InstallCallbackRemoteHandler
(
    uint8_t opc,
    uint8_t len,
    void *pData
);

/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */
/*! *********************************************************************************
*\brief  Initialize all button handlers
*
*\param  none
*
*\retval    none
********************************************************************************** */
void Button_InitCore0Handlers(void)
{
    BLE_PortFsciRegisterOpHandler((uint8_t)g_BUTTON_InstallCallback_c,
                                  BUTTON_InstallCallbackRemoteHandler);
}

/*! *********************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
********************************************************************************** */
/*! *********************************************************************************
*\brief  Button event handler
*
*\param  [in]  buttonHandle     Button handle pointer
*\param  [in]  callback         The callback function.
*\param  [in]  callbackParam    The parameter of the callback function.
*
*\retval    kStatus_BUTTON_Success
********************************************************************************** */
static button_status_t KeysHandler(
    void *buttonHandle,
    button_callback_message_t *message,
    void *callbackParam
)
{
    /* Convert button handle to array index */
    fsciPortButtonData_t buttonData;
    buttonData.id = (uint8_t)(((uint32_t)buttonHandle -
                               (uint32_t)g_buttonHandle) / BUTTON_HANDLE_SIZE);
    buttonData.event = message->event;

    /* Send Button event to the other core */
    FSCI_transmitPayload(BLE_PORT_FSCI_OG, (uint8_t)g_BUTTON_InstallCallback_c,
                         (void*)&buttonData, (uint16_t)sizeof(buttonData),
                         gFsciInterface_c);

    return kStatus_BUTTON_Success;
}

/*! *********************************************************************************
*\brief        Handle BUTTON_InstallCallback call from the remopte core
*
*\param  [in]  opc      Opcode of the FSCI command to be treated
*\param  [in]  len      Command payload length
*\param  [in]  pData    Pointer to command payload
*
*\retval    none
********************************************************************************** */
static void BUTTON_InstallCallbackRemoteHandler(uint8_t opc, uint8_t len, void *pData)
{
    uint8_t buttonId = *((uint8_t*)pData);
    (void)BUTTON_InstallCallback((button_handle_t)g_buttonHandle[buttonId],
                                 KeysHandler, NULL);
}
#endif