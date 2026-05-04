/*! *********************************************************************************
 * \defgroup Localization User Device application
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* \file loc_user_device.h
*
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

#ifndef LOC_USER_DEVICE_H
#define LOC_USER_DEVICE_H

#include "fsl_format.h"
#include "fsl_shell.h"
#include "app_conn.h"
#include "app_advertiser.h"
/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/
#define smpEdiv                 0x1F99
#define mcEncryptionKeySize_c   16

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */
extern gapConnectionRequestParameters_t gConnReqParams;
extern gapScanningParameters_t          gScanParams;

extern gapAdvertisingData_t             gAppAdvertisingData;
extern gapScanResponseData_t            gAppScanRspData;
extern gapExtAdvertisingParameters_t    gAdvParams;
extern appExtAdvertisingParams_t        gAppAdvParams;

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

#endif /* LOC_USER_DEVICE_H */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
