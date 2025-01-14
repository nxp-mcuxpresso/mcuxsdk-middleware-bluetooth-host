/*! *********************************************************************************
 * \defgroup Handover
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022-2025 NXP
*
*
* \file gap_handover_interface.h
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef GAP_HANDOVER_INTERFACE_H
#define GAP_HANDOVER_INTERFACE_H
/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ble_general.h"
#include "gap_types.h"
#include "gap_handover_types.h"

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    
/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverGetDataSize(deviceId_t deviceId)
*
* \brief        Returns the size in octets of the Handover Data for the given peer device id.
*
* \param [in]   deviceId        Peer device identifier
* \param [out]  pDataSize       Size in octets of the Handover Data
*
* \return       gBleSuccess_c or error.
***************************************************************************************************/
bleResult_t Gap_HandoverGetDataSize
(
    deviceId_t  deviceId,
    uint32_t    *pDataSize
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverGetData(deviceId_t deviceId, uint8_t *pData)
*
* \brief        Copies the data used by the Handover feature for the given peer device id.
*
* \param [in]   deviceId     Peer device identifier
* \param [in]   pData        Pointer to memory location where the Handover data is to be copied.
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverGetData
(
    deviceId_t  deviceId,
    uint32_t    *pData
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverSetData(uint8_t *pData)
*
* \brief        Set the data used by the Handover feature.
*
* \param [in]   pData                  Pointer to memory location where the Handover data is found.
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverSetData
(
    uint32_t                *pData
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverSetLlPendingData(uint16_t connectionHandle, uint8_t *pTxData)
*
* \brief        Sets the LL data pending on the target device to be transmitted after handover connect
*
* \param [in]   connectionHandle    Handover connection identifier
* \param [in]   pTxData             Pointer to memory location where the pending data is found.
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverSetLlPendingData
(
    uint16_t    connectionHandle,
    uint8_t     *pTxData
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverFreeData(void)
*
* \brief        Free reference to handover data provided through Gap_HandoverSetData().
*
* \return       gBleSuccess_c or gBleInvalidState_c if handover data reference is not set.
***************************************************************************************************/
bleResult_t Gap_HandoverFreeData
(
    void
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverGetTime(void)
*
* \brief        Gets timing information from Link Layer to be used in Handover process.
*
* \param [in]   None
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverGetTime
(
    void
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverSuspendTransmit(deviceId_t deviceId,
*                               bleHandoverSuspendTransmitMode_t mode, uint16_t eventCounter,
*                               uint8_t noOfConnIntervals)
*
* \brief        Suspends TX for the given connection.
*
* \param [in]   deviceId            Peer device identifier.
* \param [in]   mode                Mode.
* \param [in]   eventCounter        Event counter.
* \param [in]   noOfConnIntervals   Number of connection intervals during which Tx is suspended.
*                                   Use 0 for manual resume.
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverSuspendTransmit
(
    deviceId_t                                  deviceId,
    bleHandoverSuspendTransmitMode_t            mode,
    uint16_t                                    eventCounter,
    uint8_t                                     noOfConnIntervals
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverResumeTransmit(deviceId_t deviceId)
*
* \brief        Resumes TX for the given connection.
*
* \param [in]   deviceId        Peer device identifier.
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverResumeTransmit
(
    deviceId_t  deviceId
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverAnchorNotification(deviceId_t deviceId,
*                                bleHandoverAnchorNotificationEnable_t enable, uint8_t noOfReports)
*
* \brief        Enables or disables anchor notifications for the given peer.
*
* \param [in]   deviceId        Peer device identifier.
* \param [in]   enable          Enable or disable notifications.
* \param [in]   noOfReports     Number of reports to receive (0 for manual stop).
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverAnchorNotification
(
    deviceId_t                                  deviceId,
    bleHandoverAnchorNotificationEnable_t       enable,
    uint8_t                                     noOfReports
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverAnchorSearchStart( gapHandoverAnchorSearchStartParams_t *pSearchParams)
*
* \brief        Starts anchor search with the given parameters.
*
* \param [in]   pSearchParams   Pointer to structure containing anchor search parameters.
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverAnchorSearchStart
(
    gapHandoverAnchorSearchStartParams_t *pSearchParams
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverAnchorSearchStop(uint16_t connHandle)
*
* \brief        Stops anchor search for the given conn handle (used to start the anchor search).
*
* \param [in]   connHandle  Connection handle for which to stop the anchor search.
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverAnchorSearchStop
(
    uint16_t connHandle
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverTimeSyncTransmit( gapHandoverTimeSyncTransmitParams_t *pTransmitParams)
*
* \brief        Starts time sync advertising with given parameters.
*
* \param [in]   pTransmitParams   Pointer to structure containing time sync transmit parameters.
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverTimeSyncTransmit
(
    gapHandoverTimeSyncTransmitParams_t *pTransmitParams
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverTimeSyncReceive( gapHandoverTimeSyncReceiveParams_t *pReceiveParams)
*
* \brief        Starts time sync scanning with the given parameters.
*
* \param [in]   pReceiveParams   Pointer to structure containing time sync receive parameters.
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverTimeSyncReceive
(
    gapHandoverTimeSyncReceiveParams_t *pReceiveParams
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverGetConnParams(uint16_t connHandle,
*                                                    gapConnectionCallback_t connectionCallback,
*                                                    uint8_t anchorNotification)
*
* \brief        Connect to a peer device initialized through the Connection Handover procedure.
*
* \param [in]   connHandle          Handover Connection handle
* \param [in]   connectionCallback  Pointer to the connection event handler function
* \param [in]   anchorNotification  Anchor search setting after handover,
*                                   0 - stop anchor notification, 1 - keep anchor notification.
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverConnect
(
    uint16_t                connHandle,
    gapConnectionCallback_t connectionCallback,
    uint8_t                 anchorNotification
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverDisconnect(deviceId_t deviceId)
*
* \brief        Disconnect a peer device for which the connection was handed over to another device.
*
* \param [in]   deviceId    Peer device Id
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverDisconnect
(
    deviceId_t deviceId
);

/*!*************************************************************************************************
* \fn           void Gap_HandoverInit(void)
*
* \brief        Initialize the connection handover feature Must be called before Ble_Initialize().
*
* \param [in]   none.
*
* \retval        bleResult_t    gBleInvalidState_c if Ble_Initialize() was already called,
*                               gBleSuccess_c otherwise.
***************************************************************************************************/
bleResult_t Gap_HandoverInit
(
    void
);

/*!*************************************************************************************************
* \fn           void Gap_HandoverSetSkd(uint8_t nvmIndex, uint8_t *pSkd)
*
* \brief        Set the SKD of the connection that is to be handed over. Used only if
*               gAppSecureMode_d is enabled in the application. Requires Handover Data to be set.
*
* \param [in]   nvmIndex    Bonding Data NVM index.
* \param [in]   pSkd        Pointer to 16 octets SKD.
*
* \retval        bleResult_t    gBleInvalidState_c if Ble_Initialize() was already called,
*                               gBleSuccess_c otherwise.
***************************************************************************************************/
bleResult_t Gap_HandoverSetSkd(
    uint8_t nvmIndex,
    uint8_t *pSkd
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverGetCsLlContext(deviceId_t deviceId)
*
* \brief        Copies the LL CS-related context data for the given peer device id.
*
* \param [in]   deviceId     Peer device identifier
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverGetCsLlContext
(
    deviceId_t  deviceId
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverSetCsLlContext(deviceId_t  deviceId, uint16_t mask,
*                                                      uint8_t contextLength, uint8_t *pContextData)
*
* \brief        Sets the CS-related context data into the LL, for the given peer device id.
*
* \param [in]   deviceId        Peer device identifier
* \param [in]   mask            Context data bitmask (used by the LL).
* \param [in]   contextLength   Length of the context data
* \param [in]   pContextData    Pointer to context data
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverSetCsLlContext
(
    deviceId_t  deviceId,
    uint16_t    mask,
    uint8_t     contextLength,
    uint8_t     *pContextData
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverUpdateConnParams(gapHandoverUpdateConnParams_t *pConnParams)
*
* \brief        Update the channel map and/or phy used in the current anchor monitoring process.
*
* \param [in]   pConnParams         Pointer to connection parameters to be updated
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverUpdateConnParams
(
    gapHandoverUpdateConnParams_t *pConnParams
);

/*!*************************************************************************************************
* \fn           bleResult_t Gap_HandoverApplyConnectionUpdateProcedure(gapHandoverApplyConnectionUpdateProcedure_t *pConnParams)
*
* \brief        This command is used to apply the new connection parameters in case of Connection
*               Update Procedure on the active controller.
*
* \param [in]   pConnParams         Pointer to connection parameters to be applied
*
* \return       gBleSuccess_c or error
***************************************************************************************************/
bleResult_t Gap_HandoverApplyConnectionUpdateProcedure
(
    gapHandoverApplyConnectionUpdateProcedure_t *pConnParams
);
#ifdef __cplusplus
}
#endif

#endif /* GAP_HANDOVER_INTERFACE_H */

/*! *********************************************************************************
* @}
********************************************************************************** */