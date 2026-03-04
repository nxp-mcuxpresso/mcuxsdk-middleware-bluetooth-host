/*! *********************************************************************************
 * \defgroup Localization User Device application
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* \file loc_user_device.h
*
* Copyright 2023-2026 NXP
*
* NXP Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from NXP.
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

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1)
#define gCsHciDataHdrLength_c   3U
#define gHciPacketIndicator_c   0x04
#define gHciEventCode_c         0x3E
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d == 1) */

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
