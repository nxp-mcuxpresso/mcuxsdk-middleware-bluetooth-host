/*! *********************************************************************************
 * \defgroup RAS/RAP PTS test interface
 * @{
 ********************************************************************************** */
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

#include "EmbeddedTypes.h"
#include "ble_general.h"

#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
#include "channel_sounding.h"

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
/* Callback invoked to process a subevent result */
typedef bleResult_t (*pfProcessCsResultsEvent_t)(csSubeventResultEvent_t* pEvent);
/* Callback invoked to process a subevent result continue */
typedef bleResult_t (*pfProcessCsResultsContinueEvent_t)(csSubeventResultContinueEvent_t* pEvent);

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*! *********************************************************************************
* \brief      Register the result handlers owned by the application.
********************************************************************************** */
void PtsTest_RegisterCallbacks(pfProcessCsResultsEvent_t pfProcessResults,
                               pfProcessCsResultsContinueEvent_t pfProcessResultsContinue);

/*! *********************************************************************************
* \brief      Test vector parsing to process the data.
********************************************************************************** */
void AppLocalization_RunPtsTest(deviceId_t deviceId, uint8_t index, uint8_t actionMask);

#ifdef __cplusplus
}
#endif

#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */

#endif /* PTS_TEST_H */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
