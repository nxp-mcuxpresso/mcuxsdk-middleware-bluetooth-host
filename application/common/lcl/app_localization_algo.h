/*! *********************************************************************************
* Copyright 2023-2026 NXP
*
* NXP Proprietary
*
* This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or
* by downloading, installing, activating and/or otherwise using the software, you are
* agreeing that you have read, and that you agree to comply with and are bound by,
* such license terms. If you do not agree to be bound by the applicable license terms,
* then you may not retain, install, activate or otherwise use the software.
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
#include "pde_rade.h"
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
 /* Maximum allowable distance result in meters
    Higher values produced by the algorithm will be discarded by the application */
#ifndef gMaxDistanceMeters_c
#define gMaxDistanceMeters_c        (120U)
#endif

/* Number of channels used by CS */
#define gCsChannelsNb_c             (79U)

/* Size of an IQ record */
#define gCsIqSize_c                 (3U)
/* Size of a Tone Quality Indicator */
#define gCsTqiSize_c                (1U)
/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
/* Options bitmask controlling algorithm behavior for a measurement run. */
typedef uint8_t locAlgoOptions_t;
#define gLocAlgoOptionNone_c            (0x00U) /* No special options */
#define gLocAlgoOptionCsCalibration_c   (0x01U) /* CS calibration run: algo sets mn_ref_regen */
#define gLocAlgoOptionCsDataBroadcast_c (0x02U) /* CS Sniffing: broadcast IQ to sniffing anchors */
#define gLocAlgoOptionCsDataReceive_c   (0x04U) /* CS Sniffing: passive run with received MN data */

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
*                                    locAlgoOptions_t options,
*                                    localizationAlgoResult_t *pResult);
*
*\brief      Run given algorithm for given data.
*
*\param[in]  deviceId         DeviceId of the peer (used to retrieve timing info).
*\param[in]  pLocalData       Pointer to local data.
*\param[in]  pPeerData        Pointer to peer data.
*\param[in]  role             CS role of the local device (initiator or reflector).
*\param[in]  options          Options bitmask controlling algorithm behavior.
*\param[out] pResult          Pointer to location for result information.
*
*\retval     none
********************************************************************************** */
void AppLocalizationAlgo_RunMeasurement(deviceId_t deviceId,
                                        rasMeasurementData_t *pLocalData,
                                        rasMeasurementData_t *pPeerData,
                                        uint8_t role,
                                        locAlgoOptions_t options,
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

/*! *********************************************************************************
*\fn         void AppLocalizationAlgo_RegisterMnDataTransferCallback(
*                                    pfMnDataTransferCallback_t pfCallback);
*
*\brief      Register the callback used to transfer multi-node CS data produced by
*            the RADE algorithm (CS Sniffing broadcast) to the sniffing anchor.
*
*\param[in]  pfCallback       Transfer callback, or NULL to disable.
*
*\retval     none
********************************************************************************** */
void AppLocalizationAlgo_RegisterMnDataTransferCallback(pfMnDataTransferCallback_t pfCallback);

/*! *********************************************************************************
*\fn         void AppLocalizationAlgo_SetMnData(uint8_t *pData);
*
*\brief      Set the multi-node CS data blob used by the next passive receive run
*            (CS Sniffing). The buffer must stay valid until the run completes.
*
*\param[in]  pData            Pointer to the received MN data blob, or NULL.
*
*\retval     none
********************************************************************************** */
void AppLocalizationAlgo_SetMnData(uint8_t *pData);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* APP_LOCALIZATION_ALGO_H */