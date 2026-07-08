/*! *********************************************************************************
 * \file pts_test.h
 *
 * Copyright 2026 NXP
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

#ifndef PTS_TEST_H
#define PTS_TEST_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "ble_general.h"
#include "gatt_types.h"

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*!
 * \brief RAS characteristic selector used with the application callbacks.
 *        Lets the common code request an operation on a specific RAS
 *        characteristic without knowing how the application stores its handles.
 */
typedef enum
{
    gPtsCharOnDemandData_c    = 0U, /*!< On-Demand Ranging Data characteristic */
    gPtsCharDataReady_c       = 1U, /*!< Ranging Data Ready characteristic     */
    gPtsCharDataOverwritten_c = 2U  /*!< Ranging Data Overwritten characteristic */
} ptsCharId_t;

/*!
 * \brief Callback: switch the real-time data CCCD state (RAP Requester role).
 *
 * The application must configure the peer's real-time data CCCD to the
 * requested value and, on success, increment the step counter pointed to by
 * pStep so the calling state machine can advance.
 *
 * \param[in]     deviceId  Connection handle.
 * \param[in]     cccd      New CCCD value (gCccdEmpty_c / gCccdNotification_c /
 *                          gCccdIndication_c).
 * \param[in,out] pStep     Step counter; incremented by the callee on success.
 */
typedef void (*PtsTest_SwitchRtDataCb_t)(deviceId_t deviceId,
                                         uint16_t   cccd,
                                         uint8_t   *pStep);

/*!
 * \brief Callback: configure a CCCD on a RAS characteristic (RAP Requester role).
 *
 * The application is responsible for resolving charId to the correct attribute
 * handle, allocating any working buffer it needs, and writing the CCCD on the
 * remote RAS server.
 *
 * \param[in]  deviceId  Connection handle.
 * \param[in]  cccd      New CCCD value.
 * \param[in]  charId    Target RAS characteristic (see ptsCharId_t).
 * \return     bleResult_t Result of the configuration attempt.
 */
typedef bleResult_t (*PtsTest_ConfigureRasCb_t)(deviceId_t  deviceId,
                                                uint16_t    cccd,
                                                ptsCharId_t charId);

/*!
 * \brief Callback: read a RAS characteristic value (RAP Requester role).
 *
 * The application is responsible for resolving charId to the correct attribute
 * handle and issuing the GATT read.
 *
 * \param[in]  deviceId  Connection handle.
 * \param[in]  charId    Target RAS characteristic (see ptsCharId_t).
 */
typedef void (*PtsTest_ReadCharCb_t)(deviceId_t  deviceId,
                                     ptsCharId_t charId);

/*!
 * \brief Aggregated callback table injected by the application via
 *        PtsTest_RegisterCallbacks(). Unused callbacks must be set to NULL.
 *        The RAS Server role (loc_user_device) needs no callbacks and may pass
 *        an all-NULL table.
 */
typedef struct
{
    PtsTest_SwitchRtDataCb_t pfSwitchRtData;  /*!< NULL for RAS Server role */
    PtsTest_ConfigureRasCb_t pfConfigureRas;  /*!< NULL for RAS Server role */
    PtsTest_ReadCharCb_t     pfReadChar;      /*!< NULL for RAS Server role */
} ptsTestCallbacks_t;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Register the application-specific callback table.
 *        Must be called before PtsTest_RunRapRequester(). The RAS Server role
 *        may register an all-NULL table or skip registration.
 *
 * \param[in]  pCallbacks  Pointer to a statically allocated callback table.
 */
void PtsTest_RegisterCallbacks(const ptsTestCallbacks_t *pCallbacks);

/*!
 * \brief Execute one PTS step for the RAP Requester role (loc_reader,
 *        ncp_loc_reader). Ownership of pParam is transferred; the buffer is
 *        freed before returning.
 *
 * \param[in]  pParam  Heap-allocated C-string holding the PTS test-case ID.
 */
void PtsTest_RunRapRequester(void *pParam);

/*!
 * \brief Execute one PTS step for the RAS Server role (loc_user_device).
 *        Ownership of pParam is transferred; the buffer is freed before
 *        returning.
 *
 * \param[in]  pParam  Heap-allocated C-string holding the PTS test-case ID.
 */
void PtsTest_RunRasServer(void *pParam);

#ifdef __cplusplus
}
#endif

#endif /* PTS_TEST_H */
