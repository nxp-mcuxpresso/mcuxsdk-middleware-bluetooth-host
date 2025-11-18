/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file btcs_server_interface.h
*
* This is a header file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef BTCS_CLIENT_INTERFACE_H
#define BTCS_CLIENT_INTERFACE_H
/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "EmbeddedTypes.h"
#include "ble_general.h"
#include "digital_key_interface.h"
#include "app_localization.h"

#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
/* PBR format is 12 bit I/Q */
#define gPBRFormatIQ_c    0U

/* Module ID not supplied (data is from vehicle) */
#define gModuleId_c       0xFEU

/* Automatic Gain Control configuration not provided */
#define gAGC_c            0U

/* Mode 0 data: Packet Quality, RSSI, Packet Antenna */
#define gMode0DataSize_c  3U

/* Mode 1 data : Packet Quality, NADM, RSSI, Time Diff, Packet Antenna */
#define gMode1DataSize_c  6U

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\fn            void BtcsClient_Init(void)
*
*\brief         Initialize the internal BTCS client structures
*
*\param[in]     none
*
*\retval        none
********************************************************************************** */
void BtcsClient_Init(void);

/*! *********************************************************************************
*\fn            void BtcsClient_HandleRangingServiceMsg(deviceId_t deviceId,
*                                                       uint8_t* pMsgData)
*
*\brief         Handles BTCS messages reveived from a peer.
*
*\param[in]     deviceId         Peer identifier
*\param[in]     pMsgData         Pointer to the BTCS message
*
*\retval        gBleSuccess_c or error
********************************************************************************** */
bleResult_t BtcsClient_HandleRangingServiceMsg
(
    deviceId_t deviceId,
    uint8_t*   pMsgData
);

/*! *********************************************************************************
*\fn            void BtcsClient_GetPeerRangingData(deviceId_t deviceId)
*
*\brief         Returns a pointer to the BTCS data of the client.
*
*\param[in]     deviceId         Peer identifier
*
*\retval        Pointer to the BTCS data of the client
********************************************************************************** */
rasMeasurementData_t* BtcsClient_GetPeerRangingData
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            uint16_t BtcsClient_GetPeerRangingDataSize(deviceId_t deviceId);
*
*\brief         Get the size of the peer ranging data
*
*\param[in]     deviceId        Peer identifier
*
*\retval        Peer ranging data size
********************************************************************************** */
uint16_t BtcsClient_GetPeerRangingDataSize
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            void BtcsClient_ResetPeer(deviceId_t deviceId, bool_t disconnected)
*
*\brief         Clears the internal BTCS client data
*
*\param[in]     deviceId         Peer identifier
*\param[in]     disconnected     TRUE if peer has disconnected, FALSE otherwise
*
*\retval        none
********************************************************************************** */
void BtcsClient_ResetPeer
(
    deviceId_t deviceId,
    bool_t     disconnected
);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* gAppBtcsClient_d */
#endif /* BTCS_CLIENT_INTERFACE_H */