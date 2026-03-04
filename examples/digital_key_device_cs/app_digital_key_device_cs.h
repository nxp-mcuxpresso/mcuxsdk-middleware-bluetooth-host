/*! *********************************************************************************
* \addtogroup Digital Key Device CS Application
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file app_digital_key_device_cs.h
*
* Copyright 2022 - 2026 NXP
*
* NXP Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from NXP.
********************************************************************************** */

#ifndef APP_DIGITAL_KEY_DEVICE_CS_H
#define APP_DIGITAL_KEY_DEVICE_CS_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "gatt_types.h"
#include "digital_key_device_cs.h"

#include "app_localization.h"
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#define mcNumCharacteristics_c               (4U)
#define mcCharVehiclePsmIndex_c              (0U)
#define mcCharVehicleAntennaIdIndex_c        (1U)
#define mcCharTxPowerLevelIndex_c            (2U)
#define mcCharVehiclePsmDkVersionIndex_c     (3U)

#define mcCharVehiclePsmLength_c             (2U)
#define mcCharTxPowerLevelLength_c           (1U)
#define mcCharVehicleAntennaIdLength_c       (2U)
#define mCharReadBufferLength_c              (13U)           /* length of the buffer */
/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum appState_tag{
    mAppIdle_c,
    mAppExchangeMtu_c,
    mAppServiceDisc_c,
    /* certificate exchange */
    mAppCCCPhase2WaitingForRequest_c,
    mAppCCCPhase2WaitingForVerify_c,
    mAppCCCWaitingForPairingReady_c,
    mAppPair_c,
    mAppLocalizationSetup_c,
    mAppLocalization_c,
    mAppRunning_c
}appState_t;

typedef struct appCustomInfo_tag
{
    uint16_t     hDkService;
    uint16_t     hPsmChannelChar;
    uint16_t     hAntennaIdChar;
    uint16_t     hTxPowerChar;
    uint16_t     lePsmValue;
    uint16_t     psmChannelId;
    /* Add persistent information here */
}appCustomInfo_t;

typedef struct appPeerInfo_tag
{
    deviceId_t                  deviceId;
    appCustomInfo_t             customInfo;
    bool_t                      isBonded;
    uint8_t                     nvmIndex;
    bool_t                      btcsSupported;
    appState_t                  appState;
    gapLeScOobData_t            oobData;
    gapLeScOobData_t            peerOobData;
    gapDisconnectionReason_t    disconReason;
}appPeerInfo_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
extern appPeerInfo_t maPeerInformation[gAppMaxConnections_c];
extern gattCharacteristic_t maCharacteristics[mcNumCharacteristics_c]; /* Index 0 - Vehicle PSM */
extern uint8_t mValVehiclePsm[2];
extern uint16_t mValVehicleAntennaId;
extern int8_t mValTxPower;
extern uint8_t mCurrentCharReadingIndex;
extern deviceId_t mCurrentPeerId;
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
void APP_BleEventHandler(void *pData);
void BleApp_StateMachineHandler
(
    deviceId_t peerDeviceId,
    appEvent_t event
);
void BleApp_PrintMeasurementResults(deviceId_t deviceId, localizationAlgoResult_t *pResult);
#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
/* Send BTCS Ranging Procedure Results Config message */
bleResult_t SendBTCSRangingProcResCfg
(
    deviceId_t deviceId,
    bool_t     bEnableProcResTransfer
);
#endif
#ifdef __cplusplus
}
#endif

#endif /* APP_DIGITAL_KEY_DEVICE_H */