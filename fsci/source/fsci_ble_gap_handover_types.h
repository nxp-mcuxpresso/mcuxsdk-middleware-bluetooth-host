/*! *********************************************************************************
 * \defgroup FSCI_BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2022-2023 NXP
*
*
* \file
*
* This is a header file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef FSCI_BLE_GAP_HANDOVER_TYPES_H
#define FSCI_BLE_GAP_HANDOVER_TYPES_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include <string.h>
#include "fsci_ble.h"
#include "gap_handover_types.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
#define gHandoverMaxPayloadLen_c 512U

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
**********************************************************************************/

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*! *********************************************************************************
* \brief  Retrieves Anchor Search Start parameters from a buffer.
*
* \param[out]      pParams  Pointer to the parameters structure.
* \param[in,out]   ppBuffer Double pointer to the buffer.
*
********************************************************************************** */
void fsciBleGapHandoverGetAnchorSearchStartParametersFromBuffer
(
    gapHandoverAnchorSearchStartParams_t        *pParams,
    uint8_t                                     **ppBuffer
);

/*! *********************************************************************************
* \brief  Retrieves Time Sync Transmit parameters from a buffer.
*
* \param[out]      pParams  Pointer to the parameters structure.
* \param[in,out]   ppBuffer Double pointer to the buffer.
*
********************************************************************************** */
void fsciBleGapHandoverGetTimeSyncTransmitParametersFromBuffer
(
    gapHandoverTimeSyncTransmitParams_t        *pParams,
    uint8_t                                    **ppBuffer
);

/*! *********************************************************************************
* \brief  Retrieves Time Sync Receive parameters from a buffer.
*
* \param[out]      pParams  Pointer to the parameters structure.
* \param[in,out]   ppBuffer Double pointer to the buffer.
*
********************************************************************************** */
void fsciBleGapHandoverGetTimeSyncReceiveParametersFromBuffer
(
    gapHandoverTimeSyncReceiveParams_t        *pParams,
    uint8_t                                   **ppBuffer
);

/*! *********************************************************************************
* \brief  Retrieves size of a given generic event.
*
* \param[in]    pGenericEvent Pointer to the generic event data.
*
********************************************************************************** */
uint32_t fsciBleGapHandoverGetGenericEventBufferSize
(
    gapGenericEvent_t*  pGenericEvent
);

/*! *********************************************************************************
* \brief  Places generic event data into a buffer.
*
* \param[in]    pGenericEvent  Pointer to the generic event data.
* \param[out]   ppBuffer       Double pointer to the buffer.
*
********************************************************************************** */
void fsciBleGapHandoverGetBufferFromGenericEvent
(
    gapGenericEvent_t*  pGenericEvent,
    uint8_t**           ppBuffer
);

/*! *********************************************************************************
* \brief  Generic event monitor for Handover.
*
* \param[in]    pGenericEvent Pointer to the generic event data.
*
********************************************************************************** */
void fsciBleGapHandoverGenericEvtMonitor
(
    gapGenericEvent_t* pGenericEvent
    
);

/*! *********************************************************************************
* \brief  Retrieves size of a given connection event.
*
* \param[in]    pConnectionEvent Pointer to the connection event data.
*
********************************************************************************** */
uint32_t fsciBleGapHandoverGetConnectionEventBufferSize
(
    gapConnectionEvent_t* pConnectionEvent
);

/*! *********************************************************************************
* \brief  Places connection event data into a buffer.
*
* \param[in]    pConnectionEvent  Pointer to the connection event data.
* \param[out]   ppBuffer          Double pointer to the buffer.
*
********************************************************************************** */
void fsciBleGapHandoverGetBufferFromConnectionEvent
(
    gapConnectionEvent_t*   pConnectionEvent,
    uint8_t**               ppBuffer
);

#ifdef __cplusplus
}
#endif

#endif /* FSCI_BLE_GAP_TYPES_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
