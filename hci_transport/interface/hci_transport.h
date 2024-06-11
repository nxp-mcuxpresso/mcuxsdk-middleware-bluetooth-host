/*! *********************************************************************************
* \defgroup HCI
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2014 Freescale Semiconductor, Inc.
* Copyright 2017, 2019-2023 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef HCI_TRANSPORT_H
#define HCI_TRANSPORT_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ble_general.h"
#include "hci_types.h"

#include "fsl_component_serial_manager.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
#ifndef gHcitMaxPayloadLen_c
#define gHcitMaxPayloadLen_c            (gHcLeAclDataPacketLengthDefault_c + gHciAclDataPacketHeaderLength_c)
#endif /* gHcitMaxPayloadLen_c */

/* Enables Upward HCI Transport.
 * The controller sends HCI packets to be transported through the serial interface */
#ifndef gUseHciTransportUpward_d
#define gUseHciTransportUpward_d        0
#endif /* gUseHciTransportUpward_d */

/* Enables Downward HCI Transport.
 * The Host sends HCI packets to be transported through the serial interface */
#ifndef gUseHciTransportDownward_d
#define gUseHciTransportDownward_d      0
#endif /* gUseHciTransportDownward_d */

#if (gUseHciTransportDownward_d) && (gUseHciTransportUpward_d)
#error "Select maximum one HCI transport method!"
#endif /* (gUseHciTransportDownward_d) && (gUseHciTransportUpward_d) */

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

typedef bleResult_t (* hciTransportInterface_t)
(
    hciPacketType_t packetType,     /*!< HCI Packet Type. */
    void*           pPacket,        /*!< Pointer to packet payload. */
    uint16_t        packetSize      /*!< Packet payload size. */
);

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

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
#ifdef __cplusplus
    extern "C" {
#endif

#if (defined(gUseHciTransportDownward_d) && (gUseHciTransportDownward_d)) || \
    (defined(gUseHciTransportUpward_d) && gUseHciTransportUpward_d)
/*! *********************************************************************************
* \fn             bleResult_t Hcit_Init(hciTransportInterface_t hcitConfigStruct)
* \brief          Reconfigures the HCI Transport module.
*
* \param  [in]    hcitConfigStruct      HCI Transport interface function.
*
* \retval         gHciSuccess_c         Initialization was successful.
* \retval         gHciAlreadyInit_c     The module has already been initialized.
* \retval         gHciTransportError_c  Initialization was unsuccessful.
* \retval         gBleOsError_c         Initialization of OS object was unsucessfull.
* \retval         gBleOutOfMemory_c     Memory allocation failure.
********************************************************************************** */
bleResult_t Hcit_Init(hciTransportInterface_t hcitConfigStruct);

/*! *********************************************************************************
* \fn             bleResult_t Hcit_Deinit(void)
* \brief          Terminates HCI Transport module and release allocated memory
*
* \retval         gHciSuccess_c         Termination was successful.
* \retval         gBleOsError_c         Termination of OS object was unsucessfull.
* \retval         gBleOutOfMemory_c     Memory disallocation failure.
********************************************************************************** */
bleResult_t Hcit_Deinit(void);

/*! *********************************************************************************
* \fn             bleResult_t Hcit_Reconfigure(
*                                   hciTransportInterface_t hcitConfigStruct)
* \brief          Initializes the HCI Transport module.
*
* \param  [in]    hcitConfigStruct      HCI Transport interface function.
*
* \retval         gHciSuccess_c         Reconfiguration was successful.
* \retval         gHciTransportError_c  Reconfiguratoion was unsuccessful.
********************************************************************************** */
bleResult_t Hcit_Reconfigure(hciTransportInterface_t hcitConfigStruct);

/*! *********************************************************************************
* \fn             bleResult_t Hcit_SendPacket(
*                           hciPacketType_t packetType,
*                           void*           pPacket,
*                           uint16_t        packetSize)
* \brief          Sends a packet to controller.
*
* \param  [in]    packetType             HCI packet type.
* \param  [in]    pPacket                Pointer to the packet payload.
* \param  [in]    packetSize             Packet payload size.
*
* \retval         gBleSuccess_c          Packet is successfully sent.
* \retval         gBleOutOfMemory_c      Memory allocation for the packet fails.
* \retval         gHciTransportError_c   Packet is unsuccessfully sent.
********************************************************************************** */
bleResult_t Hcit_SendPacket
(
    hciPacketType_t packetType,
    void*           pPacket,
    uint16_t        packetSize
);

/*! *********************************************************************************
* \fn             bleResult_t Hcit_RecvPacket(void* pPacket, uint16_t packetSize)
* \brief          Calls the HCI module to handle a received packet.
*
* \param  [in]    pPacket        Pointer to the received packet.
* \param  [in]    packetSize     Packet size.
*
* \return         bleResult_t    Result of the operation.
********************************************************************************** */
bleResult_t Hcit_RecvPacket(void* pPacket, uint16_t packetSize);

#endif /*gUseHciTransportDownward_d || gUseHciTransportUpward_d*/

#ifdef __cplusplus
    }
#endif

#endif /* HCI_TRANSPORT_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
