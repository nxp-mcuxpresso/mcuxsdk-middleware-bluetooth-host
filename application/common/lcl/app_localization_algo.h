/*! *********************************************************************************
* Copyright 2023-2026 NXP
*
* \file app_localization_algo.h
*
* This is a source file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef APP_LOCALIZATION_ALGO_H
#define APP_LOCALIZATION_ALGO_H
/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "app_localization.h"
#if defined(gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1U)
#include "ranging_interface.h"
#endif /* defined(gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1U) */
#include "app_localization_utils.h"
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
 /* Maximum allowable distance result in meters
    Higher values produced by the algorithm will be discarded by the application */
#ifndef gMaxDistanceMeters_c
#define gMaxDistanceMeters_c  (120U)
#endif

#define gCsChannelsNb_c       (79U)

#define gCsSubeventMax_c      (16U)

/* Mode0 data size - RSSI 8 bits + Pkt Quality 4bits + CFO 20 bits */
#define gCsMode0Sz_c          (4U)
/* ToF(Pkt) data size - RSSI 8 bits + Pkt Quality 4bits + TS_DIFF 20 bits */
#define gCsTofTsSz_c          (4U)
/* Tone IQ data size - PCT 3 bytes, Tone_Quality_Indicator 1 byte */
#define gCsMciqSz_c           (4U)

#define CS_TS_SIZE            (3U)
#define CS_RSSI_SIZE          (1U)
#define CS_NADM_SIZE          (1U)

/* Size of an IQ record */
#define IQ_SIZE                 (3U)
/* Size of a Tone Quality Indicator */
#define TQI_SIZE                (1U)
/* Size of channel encoding */
#define CHANNEL_SIZE            (1U)

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
 *************************************************************************************
 *  Public prototypes
 *************************************************************************************
 ************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\fn         void AppLocalizationAlgo_RunMeasurement(deviceId_t deviceId,
*                                    rasMeasurementData_t *pLocalData,
*                                    rasMeasurementData_t *pPeerData,
*                                    uint8_t role,
*                                    localizationAlgoResult_t *pResult);
*
*\brief      Run given algorithm for given data.
*
*\param[in]  deviceId         DeviceId of the peer (used to retrieve timing info).
*\param[in]  pLocalData       Pointer to local data.
*\param[in]  pPeerData        Pointer to peer data.
*\param[in]  role             CS role of the local device (initiator or reflector).
*\param[out] pResult          Pointer to location for result information.
*
*\retval     none
********************************************************************************** */
void AppLocalizationAlgo_RunMeasurement(deviceId_t deviceId,
                                        rasMeasurementData_t *pLocalData,
                                        rasMeasurementData_t *pPeerData,
                                        uint8_t role,
                                        localizationAlgoResult_t *pResult);

/*! *********************************************************************************
*\fn         void AppLocalizationAlgo_ResetPeer(deviceId_t deviceId);
*
*\brief      Algo-related cleanup on peer disconnection.
*
*\param[in]  deviceId         DeviceId of the peer.
*
*\retval     none
********************************************************************************** */
void AppLocalizationAlgo_ResetPeer(deviceId_t deviceId);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* APP_LOCALIZATION_ALGO_H */