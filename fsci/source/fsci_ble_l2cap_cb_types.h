/*! *********************************************************************************
 * \defgroup FSCI_BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2023 NXP
*
*
* \file
*
* This is a header file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef FSCI_BLE_L2CAP_CB_TYPES_H
#define FSCI_BLE_L2CAP_CB_TYPES_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble.h"
#include "l2ca_cb_interface.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
#define fsciBleL2capCbGetEnhancedConnectionRequestBufferSize(pEnhancedConnectionRequest)    \
        (fsciBleGetDeviceIdBufferSize(&(pEnhancedConnectionRequest)->deviceId) +        \
        sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) +  sizeof(uint16_t) + \
        sizeof(uint8_t) + gL2capEnhancedMaxChannels_c * sizeof(uint16_t))

#define fsciBleL2capCbGetEnhancedConnectionCompleteBufferSize(pEnhancedConnectionComplete)    \
        (fsciBleGetDeviceIdBufferSize(&(pEnhancedConnectionComplete)->deviceId) +        \
        sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) + \
        sizeof(l2caLeCbConnectionRequestResult_t) + sizeof(uint8_t) + \
        gL2capEnhancedMaxChannels_c * sizeof(uint16_t))

#define fsciBleL2capCbGetEnhancedReconfigureRequestBufferSize(pEnhancedReconfigureRequest)    \
        (fsciBleGetDeviceIdBufferSize(&(pEnhancedReconfigureRequest)->deviceId) +        \
        sizeof(uint16_t) + sizeof(uint16_t) + sizeof(l2capReconfigureResponse_t) + sizeof(uint8_t) + \
        gL2capEnhancedMaxChannels_c * sizeof(uint16_t))

#define fsciBleL2capCbGetEnhancedReconfigureResponseBufferSize(pEnhancedReconfigureResponse)    \
        (fsciBleGetDeviceIdBufferSize(&(pEnhancedReconfigureResponse)->deviceId) +        \
        sizeof(l2capReconfigureResponse_t))

#define fsciBleL2capCbGetLeCbConnectionRequestBufferSize(pLeCbConnectionRequest)    \
        (fsciBleGetDeviceIdBufferSize(&(pLeCbConnectionRequest)->deviceId) +        \
        sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) +  sizeof(uint16_t))

#define fsciBleL2capCbGetLeCbConnectionCompleteBufferSize(pLeCbConnectionComplete)  \
        (fsciBleGetDeviceIdBufferSize(&(pLeCbConnectionComplete)->deviceId) +       \
        sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) + \
        sizeof(l2caLeCbConnectionRequestResult_t))

#define fsciBleL2capCbGetLeCbDisconnectionBufferSize(pLeCbDisconnection)    \
        (fsciBleGetDeviceIdBufferSize(&(pLeCbDisconnection)->deviceId) +    \
        sizeof(uint16_t))

#define fsciBleL2capCbGetLeCbNoPeerCreditsBufferSize(pLeCbNoPeerCredits)    \
        (fsciBleGetDeviceIdBufferSize(&(pLeCbNoPeerCredits)->deviceId) +    \
        sizeof(uint16_t))

#define fsciBleL2capCbGetLeCbLocalCreditsNotificationBufferSize(pLeCbLocalCreditsNotification)  \
        (fsciBleGetDeviceIdBufferSize(&(pLeCbLocalCreditsNotification)->deviceId) +             \
        sizeof(uint16_t) + sizeof(uint16_t))

#define fsciBleL2capCbGetLeCbErrorBufferSize(pLeCbError)         \
        (fsciBleGetDeviceIdBufferSize(&(pLeCbError)->deviceId) + \
        sizeof(bleResult_t) + sizeof(l2caErrorSource_t))

#define fsciBleL2capCbGetLeCbChannelStatusNotificationBufferSize(pLeCbChannelStatusNotification) \
        (fsciBleGetDeviceIdBufferSize(&(pLeCbChannelStatusNotification)->deviceId) + \
        sizeof(uint16_t) + sizeof(l2caChannelStatus_t))

#define fsciBleL2capCbGetLeCbLowPeerCreditsBufferSize(pLeCbLowPeerCredits)    \
        (fsciBleGetDeviceIdBufferSize(&(pLeCbLowPeerCredits)->deviceId) +    \
        sizeof(uint16_t))

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

#ifdef __cplusplus
extern "C" {
#endif

#if defined(gBLE52_d) && (gBLE52_d == 1)
void fsciBleL2capCbGetEnhancedConnReqFromBuffer
(
    l2caEnhancedConnectionRequest_t*    pEnhancedConnectionRequest,
    uint8_t**                           ppBuffer
);

void fsciBleL2capCbGetBuffFromEnhancedConnReq
(
    l2caEnhancedConnectionRequest_t*    pEnhancedConnectionRequest,
    uint8_t**                           ppBuffer
);


void fsciBleL2capCbGetEnhancedConnCompleteFromBuffer
(
    l2caEnhancedConnectionComplete_t*    pEnhancedConnectionComplete,
    uint8_t**                            ppBuffer
);

void fsciBleL2capCbGetBuffFromEnhancedConnComplete
(
    l2caEnhancedConnectionComplete_t*    pEnhancedConnectionComplete,
    uint8_t**                            ppBuffer
);

void fsciBleL2capCbGetEnhancedReconfigureReqFromBuffer
(
    l2caEnhancedReconfigureRequest_t*    pEnhancedReconfigureRequest,
    uint8_t**                            ppBuffer
);

void fsciBleL2capCbGetBuffFromEnhancedReconfigureReq
(
    l2caEnhancedReconfigureRequest_t*    pEnhancedReconfigureRequest,
    uint8_t**                            ppBuffer
);

void fsciBleL2capCbGetEnhancedReconfigureRspFromBuffer
(
    l2caEnhancedReconfigureResponse_t*    pEnhancedReconfigureResponse,
    uint8_t**                             ppBuffer
);

void fsciBleL2capCbGetBuffFromEnhancedReconfigureRsp
(
    l2caEnhancedReconfigureResponse_t*    pEnhancedReconfigureResponse,
    uint8_t**                             ppBuffer
);
#endif /* gBLE52_d */

void fsciBleL2capCbGetLeCbConnReqFromBuffer
(
    l2caLeCbConnectionRequest_t*    pLeCbConnectionRequest,
    uint8_t**                       ppBuffer
);

void fsciBleL2capCbGetBuffFromLeCbConnRequest
(
    l2caLeCbConnectionRequest_t*    pLeCbConnectionRequest,
    uint8_t**                       ppBuffer
);

void fsciBleL2capCbGetLeCbConnectionCompleteFromBuffer
(
    l2caLeCbConnectionComplete_t*   pLeCbConnectionComplete,
    uint8_t**                       ppBuffer
);

void fsciBleL2capCbGetBufferFromLeCbConnectionComplete
(
    l2caLeCbConnectionComplete_t*   pLeCbConnectionComplete,
    uint8_t**                       ppBuffer
);

void fsciBleL2capCbGetLeCbDisconnectionFromBuffer
(
    l2caLeCbDisconnection_t*    pLeCbDisconnection,
    uint8_t**                   ppBuffer
);

void fsciBleL2capCbGetBuffFromLeCbDisconnection
(
    l2caLeCbDisconnection_t*    pLeCbDisconnection,
    uint8_t**                   ppBuffer
);

void fsciBleL2capCbGetLeCbNoPeerCreditsFromBuffer
(
    l2caLeCbNoPeerCredits_t*    pLeCbNoPeerCredits,
    uint8_t**                   ppBuffer
);

void fsciBleL2capCbGetBuffFromLeCbNoPeerCredits
(
    l2caLeCbNoPeerCredits_t*    pLeCbNoPeerCredits,
    uint8_t**                   ppBuffer
);

void fsciBleL2capCbGetLeCbLowPeerCreditsFromBuffer
(
    l2caLeCbLowPeerCredits_t*   pLeCbLowPeerCredits,
    uint8_t**                   ppBuffer
);

void fsciBleL2capCbGetBuffFromLeCbLowPeerCredits
(
    l2caLeCbLowPeerCredits_t*   pLeCbLowPeerCredits,
    uint8_t**                   ppBuffer
);

void fsciBleL2capCbGetLeCbLocalCreditsNotificationFromBuffer
(
    l2caLeCbLocalCreditsNotification_t* pLeCbLocalCreditsNotification,
    uint8_t**                           ppBuffer
);

void fsciBleL2capCbGetBuffFromLeCbLocalCreditsNotification
(
    l2caLeCbLocalCreditsNotification_t* pLeCbLocalCreditsNotification,
    uint8_t**                           ppBuffer
);

void fsciBleL2capCbGetLeCbErrorFromBuffer
(
    l2caLeCbError_t* pLeCbError,
    uint8_t**        ppBuffer
);

void fsciBleL2capCbGetBuffFromLeCbError
(
    l2caLeCbError_t* pLeCbError,
    uint8_t**        ppBuffer
);

void fsciBleL2capCbGetLeCbChannelStatusNotificationFromBuffer
(
    l2caLeCbChannelStatusNotification_t* pLeCbChannelStatusNotification,
    uint8_t** ppBuffer
);

void fsciBleL2capCbGetBuffFromLeCbChannelStatusNotification
(
    l2caLeCbChannelStatusNotification_t* pLeCbChannelStatusNotification,
    uint8_t** ppBuffer
);

#ifdef __cplusplus
}
#endif

#endif /* FSCI_BLE_L2CAP_CB_TYPES_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
