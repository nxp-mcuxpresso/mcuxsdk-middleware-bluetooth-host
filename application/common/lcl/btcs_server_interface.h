/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file btcs_server_interface.h
*
* This is a header file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef BTCS_SERVER_INTERFACE_H
#define BTCS_SERVER_INTERFACE_H
/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
#include "EmbeddedTypes.h"
#include "ble_general.h"
#include "digital_key_interface.h"
#include "app_localization.h"

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

/*!**********************************************************************************
* \brief        Initialize internal BTCS structures
*
* \param[in]    none
*
* \return       none.
************************************************************************************/
void BtcsServer_Init(void);

/*!**********************************************************************************
* \brief        Set the received configuration information for the given peer
*
* \param[in]    deviceId    Peer device ID.
* \param[in]    pCfg        Pointer to the local result data
*
* \return       none.
************************************************************************************/
void BtcsServer_SetServerCfg
(
    deviceId_t            deviceId,
    rasMeasurementData_t* pCfg
);

/*!**********************************************************************************
* \brief        Build the ranging data information for the given peer
*
* \param[in]    deviceId    Peer device ID.
* \param[in]    numSteps    Number of steps included in the subevent
* \param[in]    fragmType   CSProcedureData Message Fragment type
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t BtcsServer_BuildRangingData
(
    deviceId_t             deviceId,
    uint8_t                numSteps,
    btcsProcDataMsgFragm_t fragmType
);

/*!**********************************************************************************
* \brief        Send the ranging data information to the given peer
*
* \param[in]    deviceId    Peer device ID.
* \param[in]    channelId   L2CAP channel Id to be used
* \param[in]    btcsMsgId   BTCS Ranging Service Payload Type
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t BtcsServer_SendData
(
    deviceId_t   deviceId,
    uint16_t     channelId,
    btcsMsgId_t  btcsMsgId
);

/*!**********************************************************************************
* \brief        Check if a BTCS data transfer is in progress with the given peer
*
* \param[in]    deviceId    Peer device ID.
*
* \return       TRUE if transfer is in progress, FALSE otherwise
************************************************************************************/
bool_t BtcsServer_CheckTransferInProgress
(
    deviceId_t deviceId
);

/*!**********************************************************************************
* \brief        Clear internal BTCS Server data for the given peer
*
* \param[in]    deviceId        Peer device ID.
* \param[in]    disconnected    TRUE if the peer has disconnected, FALSE otherwise
*
* \return       none
************************************************************************************/
void BtcsServer_ResetPeer
(
    deviceId_t deviceId,
    bool_t     disconnected
);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */
#endif /* BTCS_SERVER_INTERFACE_H */