/*! *********************************************************************************
* \addtogroup FSCI_BLE
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2023 NXP
*
*
* \file
*
* This is a source file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble_l2cap_cb_types.h"


#if gFsciIncluded_c && gFsciBleL2capCbLayerEnabled_d

/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

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
#if defined(gBLE52_d) && (gBLE52_d == 1)
void fsciBleL2capCbGetEnhancedConnReqFromBuffer
(
    l2caEnhancedConnectionRequest_t*    pEnhancedConnectionRequest,
    uint8_t**                           ppBuffer
)
{
    uint8_t iCount = 0;

    /* Read l2caEnhancedConnectionRequest_t structure fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&pEnhancedConnectionRequest->deviceId, ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEnhancedConnectionRequest->lePsm, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEnhancedConnectionRequest->peerMtu, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEnhancedConnectionRequest->peerMps, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEnhancedConnectionRequest->initialCredits, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEnhancedConnectionRequest->noOfChannels, *ppBuffer);

    if (pEnhancedConnectionRequest->noOfChannels <= gL2capEnhancedMaxChannels_c)
    {
        for (iCount = 0; iCount < pEnhancedConnectionRequest->noOfChannels; iCount++)
        {
            fsciBleGetUint16ValueFromBuffer(pEnhancedConnectionRequest->aCids[iCount], *ppBuffer);
        }
    }
}

void fsciBleL2capCbGetBuffFromEnhancedConnReq
(
    l2caEnhancedConnectionRequest_t* pEnhancedConnectionRequest,
    uint8_t** ppBuffer
)
{
    uint8_t iCount = 0;

    /* Write l2caEnhancedConnectionRequest_t structure fields to buffer */
    fsciBleGetBufferFromDeviceId(&pEnhancedConnectionRequest->deviceId, ppBuffer);
    fsciBleGetBufferFromUint16Value(pEnhancedConnectionRequest->lePsm, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pEnhancedConnectionRequest->peerMtu, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pEnhancedConnectionRequest->peerMps, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pEnhancedConnectionRequest->initialCredits, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pEnhancedConnectionRequest->noOfChannels, *ppBuffer);

    for (iCount = 0; iCount < pEnhancedConnectionRequest->noOfChannels; iCount++)
    {
        fsciBleGetBufferFromUint16Value(pEnhancedConnectionRequest->aCids[iCount], *ppBuffer);
    }
}

void fsciBleL2capCbGetEnhancedConnCompleteFromBuffer
(
    l2caEnhancedConnectionComplete_t*    pEnhancedConnectionComplete,
    uint8_t**                            ppBuffer
)
{
    uint8_t iCount = 0;

    /* Read l2caEnhancedConnectionComplete_t structure fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&pEnhancedConnectionComplete->deviceId, ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEnhancedConnectionComplete->peerMtu, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEnhancedConnectionComplete->peerMps, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEnhancedConnectionComplete->initialCredits, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pEnhancedConnectionComplete->result, *ppBuffer, l2caLeCbConnectionRequestResult_t);
    fsciBleGetUint8ValueFromBuffer(pEnhancedConnectionComplete->noOfChannels, *ppBuffer);

    if (pEnhancedConnectionComplete->noOfChannels <= gL2capEnhancedMaxChannels_c)
    {
        for (iCount = 0; iCount < pEnhancedConnectionComplete->noOfChannels; iCount++)
        {
            fsciBleGetUint16ValueFromBuffer(pEnhancedConnectionComplete->aCids[iCount], *ppBuffer);
        }
    }
}

void fsciBleL2capCbGetBuffFromEnhancedConnComplete
(
    l2caEnhancedConnectionComplete_t*    pEnhancedConnectionComplete,
    uint8_t**                            ppBuffer
)
{
    uint8_t iCount = 0;

    /* Write l2caEnhancedConnectionComplete_t structure fields to buffer */
    fsciBleGetBufferFromDeviceId(&pEnhancedConnectionComplete->deviceId, ppBuffer);
    fsciBleGetBufferFromUint16Value(pEnhancedConnectionComplete->peerMtu, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pEnhancedConnectionComplete->peerMps, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pEnhancedConnectionComplete->initialCredits, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pEnhancedConnectionComplete->result, *ppBuffer, l2caLeCbConnectionRequestResult_t);
    fsciBleGetBufferFromUint8Value(pEnhancedConnectionComplete->noOfChannels, *ppBuffer);

    for (iCount = 0; iCount < pEnhancedConnectionComplete->noOfChannels; iCount++)
    {
        fsciBleGetBufferFromUint16Value(pEnhancedConnectionComplete->aCids[iCount], *ppBuffer);
    }
}

void fsciBleL2capCbGetEnhancedReconfigureReqFromBuffer
(
    l2caEnhancedReconfigureRequest_t*    pEnhancedReconfigureRequest,
    uint8_t**                            ppBuffer
)
{
    uint8_t iCount = 0;

    /* Read l2caEnhancedReconfigureRequest_t structure fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&pEnhancedReconfigureRequest->deviceId, ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEnhancedReconfigureRequest->newMtu, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEnhancedReconfigureRequest->newMps, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pEnhancedReconfigureRequest->result, *ppBuffer, l2capReconfigureResponse_t);
    fsciBleGetUint8ValueFromBuffer(pEnhancedReconfigureRequest->noOfChannels, *ppBuffer);

    if (pEnhancedReconfigureRequest->noOfChannels <= gL2capEnhancedMaxChannels_c)
    {
        for (iCount = 0; iCount < pEnhancedReconfigureRequest->noOfChannels; iCount++)
        {
            fsciBleGetUint16ValueFromBuffer(pEnhancedReconfigureRequest->aCids[iCount], *ppBuffer);
        }
    }
}

void fsciBleL2capCbGetBuffFromEnhancedReconfigureReq
(
    l2caEnhancedReconfigureRequest_t*    pEnhancedReconfigureRequest,
    uint8_t**                            ppBuffer
)
{
    uint8_t iCount = 0;

    /* Write l2caEnhancedReconfigureRequest_t structure fields to buffer */
    fsciBleGetBufferFromDeviceId(&pEnhancedReconfigureRequest->deviceId, ppBuffer);
    fsciBleGetBufferFromUint16Value(pEnhancedReconfigureRequest->newMtu, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pEnhancedReconfigureRequest->newMps, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pEnhancedReconfigureRequest->result, *ppBuffer, l2capReconfigureResponse_t);
    fsciBleGetBufferFromUint8Value(pEnhancedReconfigureRequest->noOfChannels, *ppBuffer);

    for (iCount = 0; iCount < pEnhancedReconfigureRequest->noOfChannels; iCount++)
    {
        fsciBleGetBufferFromUint16Value(pEnhancedReconfigureRequest->aCids[iCount], *ppBuffer);
    }
}

void fsciBleL2capCbGetEnhancedReconfigureRspFromBuffer
(
    l2caEnhancedReconfigureResponse_t*    pEnhancedReconfigureResponse,
    uint8_t**                             ppBuffer
)
{
    /* Read l2caEnhancedReconfigureResponse_t structure fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&pEnhancedReconfigureResponse->deviceId, ppBuffer);
    fsciBleGetEnumValueFromBuffer(pEnhancedReconfigureResponse->result, *ppBuffer, l2capReconfigureResponse_t);
}

void fsciBleL2capCbGetBuffFromEnhancedReconfigureRsp
(
    l2caEnhancedReconfigureResponse_t*    pEnhancedReconfigureResponse,
    uint8_t**                             ppBuffer
)
{
    /* Write l2caEnhancedReconfigureResponse_t structure fields to buffer */
    fsciBleGetBufferFromDeviceId(&pEnhancedReconfigureResponse->deviceId, ppBuffer);
    fsciBleGetBufferFromEnumValue(pEnhancedReconfigureResponse->result, *ppBuffer, l2capReconfigureResponse_t);
}
#endif /* gBLE52_d */

void fsciBleL2capCbGetLeCbConnReqFromBuffer(l2caLeCbConnectionRequest_t* pLeCbConnectionRequest, uint8_t** ppBuffer)
{
    /* Read l2caLeCbConnectionRequest_t structure fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&pLeCbConnectionRequest->deviceId, ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbConnectionRequest->lePsm, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbConnectionRequest->peerMtu, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbConnectionRequest->peerMps, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbConnectionRequest->initialCredits, *ppBuffer);
}


void fsciBleL2capCbGetBuffFromLeCbConnRequest(l2caLeCbConnectionRequest_t* pLeCbConnectionRequest, uint8_t** ppBuffer)
{
    /* Write l2caLeCbConnectionRequest_t structure fields in buffer */
    fsciBleGetBufferFromDeviceId(&pLeCbConnectionRequest->deviceId, ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbConnectionRequest->lePsm, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbConnectionRequest->peerMtu, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbConnectionRequest->peerMps, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbConnectionRequest->initialCredits, *ppBuffer);
}


void fsciBleL2capCbGetLeCbConnectionCompleteFromBuffer(l2caLeCbConnectionComplete_t* pLeCbConnectionComplete, uint8_t** ppBuffer)
{
    /* Read l2caLeCbConnectionComplete_t structure fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&pLeCbConnectionComplete->deviceId, ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbConnectionComplete->cId, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbConnectionComplete->peerMtu, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbConnectionComplete->peerMps, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbConnectionComplete->initialCredits, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pLeCbConnectionComplete->result, *ppBuffer, l2caLeCbConnectionRequestResult_t);
}


void fsciBleL2capCbGetBufferFromLeCbConnectionComplete(l2caLeCbConnectionComplete_t* pLeCbConnectionComplete, uint8_t** ppBuffer)
{
    /* Write l2caLeCbConnectionComplete_t structure fields in buffer */
    fsciBleGetBufferFromDeviceId(&pLeCbConnectionComplete->deviceId, ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbConnectionComplete->cId, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbConnectionComplete->peerMtu, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbConnectionComplete->peerMps, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbConnectionComplete->initialCredits, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pLeCbConnectionComplete->result, *ppBuffer, l2caLeCbConnectionRequestResult_t);
}


void fsciBleL2capCbGetLeCbDisconnectionFromBuffer(l2caLeCbDisconnection_t* pLeCbDisconnection, uint8_t** ppBuffer)
{
    /* Read l2caLeCbDisconnection_t structure fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&pLeCbDisconnection->deviceId, ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbDisconnection->cId, *ppBuffer);
}


void fsciBleL2capCbGetBuffFromLeCbDisconnection(l2caLeCbDisconnection_t* pLeCbDisconnection, uint8_t** ppBuffer)
{
    /* Write l2caLeCbDisconnection_t structure fields in buffer */
    fsciBleGetBufferFromDeviceId(&pLeCbDisconnection->deviceId, ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbDisconnection->cId, *ppBuffer);
}


void fsciBleL2capCbGetLeCbNoPeerCreditsFromBuffer(l2caLeCbNoPeerCredits_t* pLeCbNoPeerCredits, uint8_t** ppBuffer)
{
    /* Read l2caLeCbNoPeerCredits_t structure fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&pLeCbNoPeerCredits->deviceId, ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbNoPeerCredits->cId, *ppBuffer);
}


void fsciBleL2capCbGetBuffFromLeCbNoPeerCredits(l2caLeCbNoPeerCredits_t* pLeCbNoPeerCredits, uint8_t** ppBuffer)
{
    fsciBleGetBufferFromDeviceId(&pLeCbNoPeerCredits->deviceId, ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbNoPeerCredits->cId, *ppBuffer);
}


void fsciBleL2capCbGetLeCbLowPeerCreditsFromBuffer(l2caLeCbLowPeerCredits_t* pLeCbLowPeerCredits, uint8_t** ppBuffer)
{
    /* Read l2caLeCbLowPeerCredits_t structure fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&pLeCbLowPeerCredits->deviceId, ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbLowPeerCredits->cId, *ppBuffer);
}


void fsciBleL2capCbGetBuffFromLeCbLowPeerCredits(l2caLeCbLowPeerCredits_t* pLeCbLowPeerCredits, uint8_t** ppBuffer)
{
    fsciBleGetBufferFromDeviceId(&pLeCbLowPeerCredits->deviceId, ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbLowPeerCredits->cId, *ppBuffer);
}


void fsciBleL2capCbGetLeCbLocalCreditsNotificationFromBuffer(l2caLeCbLocalCreditsNotification_t* pLeCbLocalCreditsNotification, uint8_t** ppBuffer)
{
    /* Read l2caLeCbLocalCreditsNotification_t structure fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&pLeCbLocalCreditsNotification->deviceId, ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbLocalCreditsNotification->cId, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbLocalCreditsNotification->localCredits, *ppBuffer);
}


void fsciBleL2capCbGetBuffFromLeCbLocalCreditsNotification(l2caLeCbLocalCreditsNotification_t* pLeCbLocalCreditsNotification, uint8_t** ppBuffer)
{
    /* Write l2caLeCbLocalCreditsNotification_t structure fields in buffer */
    fsciBleGetBufferFromDeviceId(&pLeCbLocalCreditsNotification->deviceId, ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbLocalCreditsNotification->cId, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbLocalCreditsNotification->localCredits, *ppBuffer);
}

void fsciBleL2capCbGetLeCbErrorFromBuffer(l2caLeCbError_t* pLeCbError, uint8_t** ppBuffer)
{
    /* Read l2caLeCbError_t structure fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&pLeCbError->deviceId, ppBuffer);
    fsciBleGetEnumValueFromBuffer(pLeCbError->result, *ppBuffer, bleResult_t);
    fsciBleGetEnumValueFromBuffer(pLeCbError->errorSource, *ppBuffer, l2caErrorSource_t);
}


void fsciBleL2capCbGetBuffFromLeCbError(l2caLeCbError_t* pLeCbError, uint8_t** ppBuffer)
{
    /* Write l2caLeCbError_t structure fields in buffer */
    fsciBleGetBufferFromDeviceId(&pLeCbError->deviceId, ppBuffer);
    fsciBleGetBufferFromEnumValue(pLeCbError->result, *ppBuffer, bleResult_t);
    fsciBleGetBufferFromEnumValue(pLeCbError->errorSource, *ppBuffer, l2caErrorSource_t);
}

void fsciBleL2capCbGetLeCbChannelStatusNotificationFromBuffer(l2caLeCbChannelStatusNotification_t* pLeCbChannelStatusNotification, uint8_t** ppBuffer)
{
    /* Read l2caLeCbChannelStatusNotification_t structure fields from buffer */
    fsciBleGetDeviceIdFromBuffer(&pLeCbChannelStatusNotification->deviceId, ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pLeCbChannelStatusNotification->cId, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pLeCbChannelStatusNotification->status, *ppBuffer, l2caChannelStatus_t);
}


void fsciBleL2capCbGetBuffFromLeCbChannelStatusNotification(l2caLeCbChannelStatusNotification_t* pLeCbChannelStatusNotification, uint8_t** ppBuffer)
{
    /* Write l2caLeCbChannelStatusNotification_t structure fields in buffer */
    fsciBleGetBufferFromDeviceId(&pLeCbChannelStatusNotification->deviceId, ppBuffer);
    fsciBleGetBufferFromUint16Value(pLeCbChannelStatusNotification->cId, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pLeCbChannelStatusNotification->status, *ppBuffer, l2caChannelStatus_t);
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

#endif /* gFsciIncluded_c && gFsciBleL2capCbLayerEnabled_d */

/*! *********************************************************************************
* @}
********************************************************************************** */
