/*! *********************************************************************************
 * \defgroup Localization Reader application
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* \file ncp_loc_reader.h
*
* Copyright 2025-2026 NXP
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

#ifndef LOC_READER_H
#define LOC_READER_H

#include "fsl_component_timer_manager.h"
/* Profile / Services */
#include "ranging_interface.h"
#include "app_localization.h"
#include "app_advertiser.h"

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/

/* Profile Parameters */
#define gReducedPowerMinAdvInterval_c   1600 /* 1 s */
#define gReducedPowerMaxAdvInterval_c   4000 /* 2.5 s */

#define gAdvTime_c                      30 /* 30 s*/
#define gGoToSleepAfterDataTime_c       5 /* 5 s*/

#ifndef gAppDeepSleepMode_c
#define gAppDeepSleepMode_c 1
#endif

#define smpEdiv                 0x1F99
#define mcEncryptionKeySize_c   16

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */
extern gapAdvertisingData_t             gAppAdvertisingData;
extern gapScanResponseData_t            gAppScanRspData;
extern gapExtAdvertisingParameters_t    gAdvParams;
extern appExtAdvertisingParams_t        gAppAdvParams;

extern uint16_t gFilterShellVal;
extern bool_t filterTestSend;

extern gapConnectionRequestParameters_t gConnReqParams;
extern gapScanningParameters_t          gScanParams;
/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
void BleApp_Start(void);
void BleApp_Disconnect(void);
void BleApp_FactoryReset(void);
bleResult_t BleApp_TriggerCsDistanceMeasurement(deviceId_t deviceId);
bool_t BleApp_CheckActiveConnections(void);
void BleApp_ListBondingData(void);
#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
void BleApp_RunPtsTest(void *pParam);
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */
#ifdef __cplusplus
}
#endif


#endif /* LOC_READER_H */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
