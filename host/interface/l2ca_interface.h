/*! *********************************************************************************
* \defgroup L2CA L2CAP
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2014 Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2023-2024 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef L2CA_INTERFACE_H
#define L2CA_INTERFACE_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ble_general.h"
#include "l2ca_types.h"
#include "l2ca_cb_interface.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef struct l2caConfigStruct_tag
{
    /* The list of the supported LE features for the Controller */
    leSupportedFeatures_t    leFeatures;

    /*
     * Maximum length (in octets) of the data portion of each HCI ACL Data Packet
     * that the Controller is able to accept.
     */
    uint32_t    hciLeBufferSize;

    /*
     * The maximum size of payload data in octets that the L2CAP layer entity is
     * capable of accepting. The MPS corresponds to the maximum PDU payload size.
     */
    uint16_t    maxPduPayloadSize;
} l2caConfigStruct_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Interface callback type definitions
*************************************************************************************
************************************************************************************/
typedef l2caGenericCallback_t   l2caAttChannelCallback_t;
typedef l2caGenericCallback_t   l2caSmpChannelCallback_t;
typedef l2caEattCallback_t      l2caEattChannelCallback_t;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*
* L2CAP Interface Primitives
*/

/*! *********************************************************************************
* \brief         Initializes L2CAP.
*
* \param[in]     none.
*
* \retval        gL2caSuccess_c      Successful L2CAP initialization.
* \retval        gL2caAlreadyInit_c  L2CAP already initialized.
********************************************************************************** */
bleResult_t L2ca_Init( void );

/*! *********************************************************************************
* \brief        Sends a data packet through ATT Channel.
*
* \param[in]    deviceId            The DeviceID for which the command is intended.
* \param[in]    pPacket             Data buffer to be transmitted.
* \param[in]    packetLength        Length of the data buffer.
*
* \retval       gL2caSuccess_c          Message was successfully enqueued.
* \retval       gBleInvalidParameter_c  Invalid device id.
* \retval       gBleOutOfMemory_c       Message allocation fail.
* \retval       gBleOverflow_c          The number of packets in the TX queue exceeds
*                                       gMaxL2caQueueSize.
* \retval       gL2caInsufficientResources_c    No more room in the TX queue.
*
********************************************************************************** */
bleResult_t L2ca_SendAttData
(
    deviceId_t        deviceId,
    const uint8_t*    pPacket,
    uint16_t          packetLength
);

/*! *********************************************************************************
* \brief        Sends a data packet through SM Channel.
*
* \param[in]    deviceId            The DeviceID for which the command is intended.
* \param[in]    pPacket             Data buffer to be transmitted.
* \param[in]    packetLength        Length of the data buffer.
*
* \retval       gL2caSuccess_c          Message was successfully enqueued.
* \retval       gBleInvalidParameter_c  Invalid device id.
* \retval       gBleOutOfMemory_c       Message allocation fail.
* \retval       gBleOverflow_c          The number of packets in the TX queue exceeds
*                                       gMaxL2caQueueSize.
* \retval       gL2caInsufficientResources_c    No more room in the TX queue.
********************************************************************************** */
bleResult_t L2ca_SendSmpData
(
    deviceId_t        deviceId,
    const uint8_t*    pPacket,
    uint16_t          packetLength
);

/*! *********************************************************************************
* \brief       Registers an ATT callback with the L2CAP.
*
* \param[in]   pCallback                          ATT callback function.
*
* \retval      gL2caSuccess_c                     Successful callback installation.
* \retval      gL2caCallbackAlreadyInstalled_c    Callback already installed.
********************************************************************************** */
bleResult_t L2ca_RegisterAttCallback
(
    l2caAttChannelCallback_t    pCallback
);

/*! *********************************************************************************
* \brief          Registers an SMP callback with the L2CAP.
*
* \param[in]      pCallback                          SMP callback function.
*
* \retval         gL2caSuccess_c                     Successful callback installation.
* \retval         gL2caCallbackAlreadyInstalled_c    Callback already installed.
********************************************************************************** */
bleResult_t L2ca_RegisterSmpCallback
(
    l2caSmpChannelCallback_t    pCallback
);

/*! *********************************************************************************
* \brief          Registers EATT callbacks with the L2CAP.
*
* \param[in]      pDataCallback                      Callback function for EATT
*                                                    credit-based data messages.
* \param[in]      pControlCallback                   Callback function for EATT
*                                                    credit-based control messages.
*
* \retval         gL2caSuccess_c                     Successful callback installation.
* \retval         gL2caCallbackAlreadyInstalled_c    Callback already installed.
********************************************************************************** */
bleResult_t L2ca_RegisterEattCallbacks
(
    l2caEattChannelCallback_t       pDataCallback,
    l2caLeCbControlCallback_t       pControlCallback
);

/*! *********************************************************************************
* \brief         Clears the connection information field of the given device ID from
*                the L2CAP local data structure.
*
* \param[in]     deviceId       Device Id for which the command is intended.
*
* \retval        void.
********************************************************************************** */
void L2ca_NotifyConnection(deviceId_t deviceId);

/*! *********************************************************************************
* \brief         Disconnects channel associated with the given device id.
*
* \param[in]     deviceId       Device Id for which the command is intended.
*
* \retval        void.
********************************************************************************** */
void L2ca_NotifyDisconnection(deviceId_t deviceId);

#if defined (gHostDeinitSupport_d) && (gHostDeinitSupport_d == TRUE)
/*! *********************************************************************************
*\fn             void L2ca_Deinit(void)
*\brief          Frees all L2CA allocated resources
*
*\param  [in]    none.
*
*\retval         void.
********************************************************************************** */
void L2ca_Deinit(void);
#endif /* defined (gHostDeinitSupport_d) && (gHostDeinitSupport_d == TRUE) */
#ifdef __cplusplus
}
#endif

#endif /* L2CA_INTERFACE_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
