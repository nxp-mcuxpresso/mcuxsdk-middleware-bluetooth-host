/*! *********************************************************************************
* Copyright 2025-2026 NXP
*
* NXP Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from NXP.
********************************************************************************** */
#ifndef RANGING_CLIENT_INTERFACE_H
#define RANGING_CLIENT_INTERFACE_H
/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
 #if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
#include "EmbeddedTypes.h"
#include "ble_general.h"
#include "ranging_interface.h"
#include "app_localization.h"

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

typedef struct rasTransferStatus_tag
{
    bool_t      expectingSegments;                          /* TRUE if expecting to receive lost segments */
    uint8_t     crtIdxRecvLost;                             /* current index of the received lost segments */
    uint8_t     currentIdxLostSegm;                         /* number of lost segments */
    uint8_t     currentIdxRecvSegm;                         /* number of received segments */
    uint8_t     currentIdxRecvIntermSegm;                   /* number of received intermediary segments */
    uint16_t    lastDataIdx;                                /* size of the received data until losing a segment */
    uint16_t    crtTempDataIdx;                             /* size of the data containted in the intermediary segments */
    uint8_t     lostSegm[gRASMaxNoOfSegments_c];            /* indices of lost segments */
    uint8_t     recvSegm[gRASMaxNoOfSegments_c];            /* indices of received segments */
    uint8_t     recvIntermSegm[gRASMaxNoOfSegments_c];      /* indices of intermediary segments received after losing a segment */
    uint16_t    recvIntermSegmLen[gRASMaxNoOfSegments_c];   /* length of the data containted in the intermediary segments */
    uint8_t*    pNotifTempBuffer;
} rasTransferStatus_t;

typedef struct rasFilter_tag {
    uint32_t filterVal;
    bool_t   filterSet;
} rasFilter_t;

/************************************************************************************
*************************************************************************************
* Public function prototypes
*************************************************************************************
************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\fn          void RasClient_Init(pfAppCsCallback_t pAppCallback)
*
*\brief       Initialize internal RAS client structures.
*
*\param[in]   pAppCallback   Application callback
*
*\retval      none
********************************************************************************** */
void RasClient_Init
(
    pfAppCsCallback_t pAppCallback,
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn          bleResult_t RasClient_StorePeerMeasurementData(deviceId_t deviceId,
*             uint8_t*    pValue, uint16_t    valueLength)
*
*\brief       Store measurement data received from the given peer.
*
*\param[in]   deviceId        Peer device id.
*\param[in]   pValue          Pointer to value.
*\param[in]   valueLength     Value length.
*
*\retval     gBleSuccess_c        Successful
*\retval     gBleOutOfMemory_c    Could not allocate memory for peer data
*\retval     gBleUnavailable_c    Algorithm not run, app must call it directly
********************************************************************************** */
bleResult_t RasClient_StorePeerMeasurementData
(
    deviceId_t deviceId,
    uint8_t*    pValue,
    uint16_t    valueLength
);

/*! *********************************************************************************
*\fn         bleResult_t RasClient_ProcessRasDataReadyIndications(deviceId_t deviceId,
*            uint8_t* pValue, uint16_t valueLength)
*
*\brief      Process indications received for the RAS Ranging Data Ready .
*
*\param[in]  deviceId         Peer device id.
*\param[in]  pValue           Pointer to value.
*\param[in]  valueLength      Value length.
*
*\retval     bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t RasClient_ProcessRasDataReadyIndications
(
    deviceId_t  deviceId,
    uint8_t*    pValue,
    uint16_t    valueLength
);

/*! *********************************************************************************
*\fn         bleResult_t RasClient_ProcessRasDataOverwrittenIndications(deviceId_t deviceId,
*            uint8_t* pValue, uint16_t valueLength)
*
*\brief      Process indications received for the RAS Ranging Data Overwritten .
*
*\param[in]  deviceId         Peer device id.
*\param[in]  pValue           Pointer to value.
*\param[in]  valueLength      Value length.
*
*\retval     bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t RasClient_ProcessRasDataOverwrittenIndications
(
    deviceId_t  deviceId,
    uint8_t*    pValue,
    uint16_t    valueLength
);

/*! *********************************************************************************
*\fn         bleResult_t RasClient_ProcessRasCPRsp(deviceId_t deviceId,
*            uint8_t* pValue, uint16_t valueLength)
*
*\brief      Process indications received for the RAS control point characteristic .
*
*\param[in]  deviceId         Peer device id.
*\param[in]  pValue           Pointer to value.
*\param[in]  valueLength      Value length.
*
*\retval     bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t RasClient_ProcessRasCPRsp
(
    deviceId_t deviceId,
    uint8_t*   pValue,
    uint16_t   valueLength
);

/*! *********************************************************************************
*\fn         bleResult_t RasClient_SendRasCommand(deviceId_t peerDeviceId,
*            rasControlPointOperations_tag rasCmdOpcode, uint8_t startSegm, uint8_t endSegm,
*            uint16_t procCounter, uint16_t filter)
*
*\brief      Handle a RAS Command Response received from the peer.
*
*\param[in]  peerDeviceId       Peer device id.
*\param[in]  rasCmdOpcode       OpCode of the command to be sent
*\param[in]  startSegm          Start segment parameter of Retrieve Lost Ranging Data Segments command
*\param[in]  endSegm            Start segment parameter of Retrieve Lost Ranging Data Segments command
*\param[in]  procCounter        Identifier of the procedure to which the command is reffering
*\param[in]  filter             Filter value to be used for the Filter command
*
*\retval     bleResult_t        Result of the operation.
********************************************************************************** */
bleResult_t RasClient_SendRasCommand
(
    deviceId_t                    peerDeviceId,
    rasControlPointOperations_tag rasCmdOpcode,
    uint8_t                       startSegm,
    uint8_t                       endSegm,
    uint16_t                      procCounter,
    uint16_t                      filter
);

/*! *********************************************************************************
*\fn         void RasClient_SetRasControlPointHandle(deviceId_t deviceId, uint16_t handle);
*
*\brief      Register the handle value for the RAS Control Point characteristic.
*
*\param[in]  deviceId       Peer Identifier
*\param[in]  handle         Handle value
*
*\retval     none
********************************************************************************** */
void RasClient_SetRasControlPointHandle
(
    deviceId_t deviceId,
    uint16_t   handle
);

/*! *********************************************************************************
*\fn         void RasClient_SetRasSupportedFeatures (deviceId_t deviceId,
*                                                    uint32_t features);
*
*\brief      Register the handle value for the RAS Control Point characteristic.
*
*\param[in]  deviceId         Peer device id.
*\param[in]  features         Optional RAS features supported by peer
*
*\retval     none
********************************************************************************** */
void RasClient_SetRasSupportedFeatures
(
    deviceId_t deviceId,
    uint32_t   features
);

/*! *********************************************************************************
*\fn            void RasClient_ParseReceivedSubeventHeader(deviceId_t deviceId,
*               uint8_t* pRangingData);
*
*\brief         Parse the subevent header information in the ranging data.
*
*\param[in]     deviceId      Peer identifier
*\param[in]     pRangingData  Pointer to ranging data
*
*\retval        none
********************************************************************************** */
void RasClient_ParseReceivedSubeventHeader
(
    deviceId_t deviceId,
    uint8_t*   pRangingData
);

/*! *********************************************************************************
*\fn            uint16_t RasClient_GetModeFilter(deviceId_t deviceId, uint8_t mode);
*
*\brief         Return the filter set for the giver peer id and step mode.
*
*\param[in]     deviceId      Peer identifier
*\param[in]     mode          Step Mode (0 - 3)
*
*\retval        uint16_t      Filter value
********************************************************************************** */
uint16_t RasClient_GetModeFilter
(
    deviceId_t deviceId,
    uint8_t    mode
);

/*! *********************************************************************************
*\fn            bleResult_t RasClient_RasSetFilter(deviceId_t deviceId,
*                           uint16_t  filterValue, bool_t sendCommand, bool_t* pOutFilterSetDone);
*
*\brief         Send a Filter command to the peer with the provided filter value.
*
*\param[in]     deviceId             Peer identifier
*\param[in]     filterValue          Filter value to be set
*\param[in]     sendCommand          TRUE if command should be sent to peer, FALSE otherwise.
*\param[out]    pOutFilterSetDone    TRUE if all filters have been set, FALSE otherwise.
*
*\retval        gBleSuccess_c or error
********************************************************************************** */
bleResult_t RasClient_RasSetFilter
(
    deviceId_t deviceId,
    uint16_t   filterValue,
    bool_t     sendCommand,
    bool_t*    pOutFilterSetDone
);

/*! *********************************************************************************
*\fn            void RasClient_SetRealTimePreference(deviceId_t deviceId);
*
*\brief         Enable if the client application wishes to use real time transfer
*
*\param[in]     deviceId           Peer identifier
*\param[in]     realTimePreference TRUE if real-time transfer is enabled, FALSE otherwise
*
*\retval        none
********************************************************************************** */
void RasClient_SetRealTimePreference
(
    deviceId_t deviceId,
    bool_t     realTimePreference
);

/*! *********************************************************************************
*\fn            bool_t RasClient_GetRealTimePreference(deviceId_t deviceId);
*
*\brief         Get the current setting for real time data transfer.
*
*\param[in]     deviceId         Peer identifier
*
*\retval        bool_t  TRUE if real-time data transfer is enabled, FALSE otherwise
********************************************************************************** */
bool_t RasClient_GetRealTimePreference
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn         void RasClient_SetRasRealTimeHandle(deviceId_t deviceId, uint16_t handle);
*
*\brief      Register the handle value for the RAS Real-Time Ranging Data characteristic.
*
*\param[in]  deviceId       Peer Identifier
*\param[in]  handle         Handle value
*
*\retval     none
********************************************************************************** */
void RasClient_SetRasRealTimeHandle
(
    deviceId_t deviceId,
    uint16_t   handle
);

/*! *********************************************************************************
*\fn         void RasClient_GetRealTimeMode(deviceId_t deviceId);
*
*\brief      Returns TRUE if Real Time RAS transfer is enabled, FALSE otherwise.
*
*\param[in]  deviceId       Peer Identifier
*
*\retval     TRUE if Real Time RAS transfer is enabled, FALSE otherwise.
********************************************************************************** */
bool_t RasClient_GetRealTimeMode
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn         void RasClient_ResetRasTransferInfo(deviceId_t deviceId);
*
*\brief      Resets internal RAS trasnfer information for the given peer
*
*\param[in]  deviceId       Peer Identifier
*
*\retval     none
********************************************************************************** */
void RasClient_ResetRasTransferInfo
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn         void RasClient_ResetPeerInfo(deviceId_t deviceId);
*
*\brief      Resets internal RAS data for the given peer
*
*\param[in]  deviceId       Peer Identifier
*
*\retval     none
********************************************************************************** */
void RasClient_ResetPeerInfo
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            void RasClient_ResetPeer(deviceId_t deviceId, bool_t disconnected)
*
*\brief         Resets all RAS client data for the given peer device.
*
*\param  [in]   deviceId        Peer device identifier.
*\param  [in]   disconnected    TRUE if the peer disconnected, FALSE otherwise.
*
*\retval        none.
********************************************************************************** */
void RasClient_ResetPeer
(
    deviceId_t deviceId,
    bool_t     disconnected
);

/*! *********************************************************************************
*\fn            void RasClient_OpenRapTimer(void)
*
*\brief         Open the RAP for the given peer device.
*
*\param  [in]   none
*
*\retval        none.
********************************************************************************** */
void RasClient_OpenRapTimer(void);

/*! *********************************************************************************
*\fn            void RasClient_OpenRapTimer(deviceId_t deviceId,
*                                           rreqTimeoutData_t rreqTimeoutData)
*
*\brief         Start the RAP for the given peer device.
*
*\param  [in]   deviceId           Peer device identifier.
*\param  [in]   rreqTimeoutData    Timer callback information
*
*\retval        none.
********************************************************************************** */
void RasClient_SartRapTimer
(
    deviceId_t        deviceId,
    rreqTimeoutData_t rreqTimeoutData
);

/*! *********************************************************************************
*\fn            void RasClient_ParseDataHeader(uint8_t **ppRangingData,
*               uint32_t *pRangingLength, deviceId_t deviceId);
*
*\brief         Parser for header data.
*
*\param[in/out] ppRangingData   Double pointer to the received data
*\param[in/out] pRangingLength  Pointer to the received data size
*\param[in]     deviceId        Peer identifier
*
*\retval        None
********************************************************************************** */
void RasClient_ParseDataHeader
(
    uint8_t **ppRangingData,
    uint32_t *pRangingLength,
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            uint16_t RasClient_GetPeerProcCount(deviceId_t deviceId);
*
*\brief         Return the procedure counter received from the peer
*
*\param[in]     deviceId        Peer identifier
*
*\retval        uint16_t        Peer procedure counter
********************************************************************************** */
uint16_t RasClient_GetPeerProcCount
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            rasMeasurementData_t* RasClient_GetPeerRangingData(deviceId_t deviceId);
*
*\brief         Get a pointer to the peer ranging data
*
*\param[in]     deviceId        Peer identifier
*
*\retval        Pointer to peer ranging data
********************************************************************************** */
rasMeasurementData_t* RasClient_GetPeerRangingData
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            void RasClient_SetPeerRangingData(deviceId_t deviceId);
*
*\brief         Set a pointer to the peer ranging data
*
*\param[in]     deviceId     Peer identifier
*\param[in]     pData        Pointer to the new ranging data
*
*\retval        Pointer to peer ranging data
********************************************************************************** */
void RasClient_SetPeerRangingData
(
    deviceId_t deviceId,
    rasMeasurementData_t* pData
);

/*! *********************************************************************************
*\fn            uint16_t RasClient_GetPeerRangingDataSize(deviceId_t deviceId);
*
*\brief         Get the size of the peer ranging data
*
*\param[in]     deviceId        Peer identifier
*
*\retval        Peer ranging data size
********************************************************************************** */
uint16_t RasClient_GetPeerRangingDataSize
(
    deviceId_t deviceId
);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* gRasRREQ_d */
#endif /* gAppRasDataTransfer_d */

#endif /* RANGING_CLIENT_INTERFACE_H */