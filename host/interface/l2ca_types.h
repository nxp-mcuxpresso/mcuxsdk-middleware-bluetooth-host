/*! *********************************************************************************
* \addtogroup L2CA
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2014 Freescale Semiconductor, Inc.
* Copyright 2016-2019, 2021-2023, 2025 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef L2CA_TYPES_H
#define L2CA_TYPES_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ble_general.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
/*
    CID name space on LE-U logical link

    CID             Description
    -------------   ---------------------
    0X0000          Null identifier (Not Allowed)
    0x0001-0x0003   Reserved
    0x0004          Attribute Protocol
    0x0005          Low Energy L2CAP Signalling channel
    0x0006          Security Manager Protocol
    0x0007-0x001F   Reserved
    0x0020-0x003E   Assigned Numbers
    0x003F          Reserved
    0x0040-0x007F   Dynamically allocated
    0x0080-0xFFFF   Reserved
*/
#define gL2capCidNull_c                         (0x0000U)
#define gL2capCidAtt_c                          (0x0004U)
#define gL2capCidSignaling_c                    (0x0005U)
#define gL2capCidSmp_c                          (0x0006U)

#define gL2capCidSigAssignedFirst_c             (0x0020U)
#define gL2capCidSigAssignedLast_c              (0x003EU)

#define gL2capCidLePsmDynamicFirst_c            (0x0040U)
#define gL2capCidLePsmDynamicLast_c             (0x007FU)

#define gL2capCidNotApplicable_c                (0xFFFFU)

/*
    LE Protocol/Service Multiplexer

    Range           Description
    -------------   ---------------------
    0x0001-0x007F   Fixed, SIG assigned. LE_PSM is fixed for all implementations
    0x0080-0x00FF   Dynamic LE_PSM may be fixed for a given implementation or
                    may be assigned at the time the service is registered in GATT
    0x0100-0xFFFF   Reserved Not applicable
*/
#define gL2caLePsmSigAssignedFirst_c            (0x0001U)
#define gL2caLePsmSigAssignedEatt_c             (0x0027U)
#define gL2caLePsmSigAssignedLast_c             (0x007FU)

#define gL2caLePsmDynamicFirst_c                (0x0080U)
#define gL2caLePsmDynamicLast_c                 (0x00FFU)

#define gL2capDefaultMtu_c                      (23U)
#define gL2capDefaultMps_c                      (23U)
#define gL2capMaximumMps_c                      (65533U)
#define gEnhancedL2capMinimumMps_c              (64U)

#define gL2capHeaderLength_c                    (4U)

#define gExpandAsEnum_m(a,b,c) b = c,
#define gExpandAsTable_m(a,b,c) c,

/* Maximum number of channels that can be included in one Enhanced Connect/Reconfigure Request */
#define gL2capEnhancedMaxChannels_c             (5U)
/*
*   Add here all Bluetooth SIG Assigned Numbers for L2CAP LE_PSM
*/
#define gLePsmSigAssignedNumbersTable_m(entry) \
    entry((0), gL2ca_Le_Psm_Ipsp_c,   (0x0023U)) \


/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum
{
    gL2ca_LePsmConnectRequest_c                 = 0x00U,
    gL2ca_LePsmConnectionComplete_c             = 0x01U,
    gL2ca_LePsmDisconnectNotification_c         = 0x02U,
    gL2ca_NoPeerCredits_c                       = 0x03U,
    gL2ca_LowPeerCredits_c                      = 0x04U,
    gL2ca_LocalCreditsNotification_c            = 0x05U,
    gL2ca_Error_c                               = 0x06U,
    gL2ca_ChannelStatusNotification_c           = 0x07U,
    gL2ca_LePsmEnhancedConnectRequest_c         = 0x08U,
    gL2ca_LePsmEnhancedConnectionComplete_c     = 0x09U,
    gL2ca_EnhancedReconfigureRequest_c          = 0x0AU,
    gL2ca_EnhancedReconfigureResponse_c         = 0x0BU,
    gL2ca_HandoverConnectionComplete_c          = 0x0CU,
} l2capControlMessageType_t;

/************************************************************************************
*************************************************************************************
* Interface callback type definitions
*************************************************************************************
************************************************************************************/
typedef void(*l2caGenericCallback_t)
                (deviceId_t deviceId,
                 uint8_t*   pPacket,
                 uint16_t   packetLength);

typedef void(*l2caEattCallback_t)
                (deviceId_t deviceId,
                 uint16_t   channelId,
                 uint8_t*   pPacket,
                 uint16_t   packetLength);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif

#endif /* L2CA_TYPES_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
