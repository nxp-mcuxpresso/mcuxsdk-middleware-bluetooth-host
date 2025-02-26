/*! *********************************************************************************
* Copyright 2022-2023 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef HSDK_INTERFACE_H
#define HSDK_INTERFACE_H

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "EmbeddedTypes.h"
#include "fsl_component_serial_manager.h"
#include "host_cmd_ble.h"

/*! *********************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
********************************************************************************** */
/* FSCI interface to be used to comunicate with BB */
#ifndef gFsciInterface_c
#define gFsciInterface_c        0
#endif
                
/*! FSCI operation group for L2CAP CB */
#define gFsciBleL2capCbOpcodeGroup_c            0x42

/*! FSCI operation group for GATT */
#define gFsciBleGattOpcodeGroup_c               0x45

/*! FSCI operation group for GATT Database (application) */
#define gFsciBleGattDbAppOpcodeGroup_c          0x46

/*! FSCI operation group for GAP */
#define gFsciBleGapOpcodeGroup_c                0x48

/*! FSCI operation group for GAP */
#define gObserverListSize_c                     0x0AU

/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */
/* HSDK status codes */
typedef enum{
    gHSDKSuccess_c                 = 0x00,
    gHSDKError_c                   = 0xFF    /* General catchall error. */
} gHSDKStatus_t;

/*! *********************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Interface callback type definitions
*************************************************************************************
************************************************************************************/
typedef void(*eventCallback_t)(bleEvtContainer_t *container);

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */
#ifdef __cplusplus
extern "C" {
#endif

/*! *********************************************************************************
* \brief   This function registers the FSCI handlers for the enabled HSDK.
*
* \param[in]    fsciInterfaceId        The interface on which data should be 
                                        received and sent.
*
********************************************************************************** */
void HsdkInit
(
    uint32_t fsciInterfaceId
);

/*! *********************************************************************************
* \brief  Calls the CB function associated with the FSCI packet received over UART.
*
* \param[in]    pData               Packet (containing FSCI header and FSCI 
                                    payload) received over UART.  
* \param[in]    param               Pointer given when this function is registered in
                                    FSCI.
* \param[in]    fsciInterfaceId     FSCI interface on which the packet was received.
*
********************************************************************************** */
void fsciCbHandler
(
    void*       pData, 
    void*       param, 
    uint32_t    fsciInterface
);


/*! *********************************************************************************
* \brief  Send the FSCI packet over UART.
*
* \param[in]    arg               Optional argument passed to the function
* \param[in]    og                FSCI operation group
* \param[in]    oc                FSCI operation code
* \param[in]    msg               Pointer to payload
* \param[in]    msgLen            Payload length
* \param[in]    fsciInterface     FSCI interface ID
*
********************************************************************************** */
void HSDK_transmitPayload
(
    void *arg,
    uint8_t og,
    uint8_t oc,
    void *msg,
    uint16_t msgLen,
    uint8_t fsciInterface
);

/*! *********************************************************************************
* \brief        Registers callback in aplication for hsdk events.
*
* \param[in]    pCallback           Aplication callback function for hsdk messages
*
* \return       Result of the operation
*
********************************************************************************** */
gHSDKStatus_t RegistergAppCallbackHSDK
(
    eventCallback_t    pCallback
);

/*! *********************************************************************************
* \brief        Registers callback for hsdk events.
*
* \param[in]    pCallback           Callback function for hsdk messages
*
* \return       Result of the operation
*
********************************************************************************** */
gHSDKStatus_t RegistergHSDKCallback
(
    eventCallback_t    pCallback
);

/*****************************************************************************
* Handles all messages received from the HSDK.
*
* \param[in]    pMsg    Pointer to bleEvtContainer_t.
*
* Return value: None
*****************************************************************************/
void App_HandleObservedHSDKMessageInput(bleEvtContainer_t* pMsg);

/*****************************************************************************
* \brief        Registers an observer that will be removed after the first match.
*
* \param[in]    fsciIds    Event identifier that is awaited.
* \param[in]    pCallback  Callback where to direct teh event.
*
* Return value: True if the observer has been registered with success
*               False otherwise. 
*****************************************************************************/
bool_t RegisterRemovableObserver(bleFsciIds_t fsciIds, eventCallback_t pCallback);

/*****************************************************************************
* \brief        Registers an observer that will be removed after the first match.
*
* \param[in]    fsciIds    Event identifier that is awaited.
* \param[in]    pCallback  Callback where to direct teh event.
*
* Return value: True if the observer has been registered with success
*               False otherwise. 
*****************************************************************************/
bool_t RemoveObserver(bleFsciIds_t fsciIds, eventCallback_t pCallback);

#ifdef __cplusplus
}
#endif

#endif /* HSDK_INTERFACE_H */
