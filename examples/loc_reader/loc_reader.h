/*! *********************************************************************************
 * \defgroup Localization Reader application
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* \file loc_reader.h
*
* Copyright 2023-2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
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

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1)
#define gCsHciDataHdrLength_c   3U
#define gHciPacketIndicator_c   0x04
#define gHciEventCode_c         0x3E
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1) */

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
