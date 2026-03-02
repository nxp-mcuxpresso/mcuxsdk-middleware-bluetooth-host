
/*! *********************************************************************************
* Copyright 2022-2026 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef W_UART_APPLICATION_H
#define W_UART_APPLICATION_H

#include "gap_types.h"
#include "gatt_types.h"
#include "host_cmd_ble.h"
#include "host_ble_service_discovery.h"

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/
/* Database Service Handle values */
#define mcGenericAccessProfileHandle_c         (1U)
#define mWUartServiceHandle_c                  (7U)
#define mBatteryServiceHandle_c                (11U)
#define mDeviceInfoServiceHandle_c             (17U)

#define smpEdiv                                (0x1F99)
#define mcEncryptionKeySize_c                  (16u)

/* Profile Parameters */
#define gScanningTime_c                        (10U)   /* 10 s*/

/* Enable/Disable Controller Adv/Scan/Connection Notifications */
#ifndef gUseControllerNotifications_c
#define gUseControllerNotifications_c 0
#endif

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c == 1))
  /* switch press timer timeout */
  #ifndef gSwitchPressTimeout_c
  #define gSwitchPressTimeout_c    (1000UL)
  #endif
  /* switch press threshold (number of key presses to toggle the GAP role) */
  #ifndef gSwitchPressThreshold_c
  #define gSwitchPressThreshold_c    (2)
  #endif
#endif

#ifndef gWuart_CentralRole_c
#define gWuart_CentralRole_c 1
#endif

#ifndef gWuart_PeripheralRole_c
#define gWuart_PeripheralRole_c 1
#endif

#ifndef gWuart_AutoStartGapRole_c
#define gWuart_AutoStartGapRole_c gGapCentral_c
#endif

/* !< Specify if the database is already known at startup, to avoid Service Discovery
 * in order to populate it we need to set the handles required by this GATT Client
 */
#ifndef gDbOobPopulated_c
#define gDbOobPopulated_c               0
#endif

#if (defined(gDbOobPopulated_c) && gDbOobPopulated_c == 1)
#define gDbOobServiceHandle_c           0x000EU
#define gDbOobUartStreamHandle_c        0x0010U
#endif

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */
extern gapAdvertisingData_t             gAppAdvertisingData;
extern gapAdvertisingParameters_t       gAdvParams;
extern gapScanningParameters_t          gScanParams;
extern gapConnectionRequestParameters_t gConnReqParams;

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
extern gapPairingParameters_t           gPairingParameters;
extern gapSmpKeys_t                     gSmpKeys;
extern gapDeviceSecurityRequirements_t  deviceSecurityRequirements;
#endif

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
/*! Wireless UART Service - Configuration */
typedef struct wusConfig_tag
{
    uint16_t    serviceHandle;
} wusConfig_t;

/*! Wireless UART Client - Configuration */
typedef struct wucConfig_tag
{
    uint16_t    hService;
    uint16_t    hUartStream;
} wucConfig_t;

typedef enum appEvent_tag
{
    mAppEvt_PeerConnected_c,
    mAppEvt_PairingComplete_c,
    mAppEvt_EncryptionComplete_c,
    mAppEvt_ServiceDiscoveryComplete_c,
    mAppEvt_ServiceDiscoveryNotFound_c,
    mAppEvt_ServiceDiscoveryFailed_c,
    mAppEvt_GattProcComplete_c,
    mAppEvt_GattProcError_c
} appEvent_t;

typedef enum appState_tag
{
    mAppIdle_c,
    mAppExchangeMtu_c,
    mAppServiceDisc_c,
    mAppServiceDiscRetry_c,
    mAppRunning_c
} appState_t;

typedef struct appPeerInfo_tag
{
    deviceId_t  deviceId;
    bool_t      isBonded;
    wucConfig_t clientInfo;
    appState_t  appState;
    gapRole_t   gapRole;
    bleAddressType_t  addressType;
    bleDeviceAddress_t address;
} appPeerInfo_t;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void Shell_Init(void);

void BleApp_SendUartStream
(
    uint8_t *pRecvStream,
    uint32_t streamSize
);

void BleApp_EventCallback
(
    bleEvtContainer_t *pMsg
);

void BleApp_ServiceDiscoveryCallback
(
    deviceId_t peerDeviceId,
    servDiscEvent_t *pEvent
);

void BleApp_StateMachineHandler
(
    deviceId_t peerDeviceId,
    appEvent_t event
);

#ifdef __cplusplus
}
#endif

#endif /* W_UART_APPLICATION_H */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
