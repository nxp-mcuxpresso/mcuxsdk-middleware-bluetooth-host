/*! *********************************************************************************
 * \defgroup Wireless Ranging Application
 * @{
 ********************************************************************************** */
/*
 * Copyright 2020-2026 NXP
 *
 * NXP Confidential Proprietary
 *
 * No part of this document must be reproduced in any form - including copied,
 * transcribed, printed or by any electronic means - without specific written
 * permission from NXP.
 */
#ifndef WIRELESS_RANGING_H
#define WIRELESS_RANGING_H

#include "gap_types.h"
#include "isp_configuration.h"
#include "isp_interface.h"
#include "channel_sounding.h"

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/
/* Profile Parameters */
#define gBleInfoTimeout_c             (500U)    /* milliseconds */
#define gBleInfoRefreshTime_c         (25U)     /* milliseconds */
#define gTemperaturePollingInterval_c (500U)    /* milliseconds */

#define CS_TEST_CONNHDL (0xFFFFU)

/* Rough estimation of worst case CS procedure duration for WR
 (depends on subevent fragmentation, n_ap ..., assumes 4 AP, 80 channels in a single subevent) */
#define WR_CS_PROC_DURATION_MS_MAX 40

/* Rough estimation of worst case CS subevent duration for WR
   assumes 4 AP, 80 channels in a single subevent) */
#define WR_CS_SUBEVT_DURATION_US_MAX 40000U

/* Estimated algo durations (4 AP, 80 channels) for procedure repeat frequency calculation */
#define WR_ALGO_CDE_DURATION_MS  25
#define WR_ALGO_RADE_DURATION_MS 45

/* Estimated print durations for procedure repeat frequency calculation */
#define WR_POSTPROC_VERB_DURATION_MS_MIN 20
#define WR_POSTPROC_VERB_DURATION_MS_MAX 500

/* Estimated minimum value for procedure repeat frequency on FreeRTOS.
   Further investigation TBD to determine why we need to increase the interval. */
#ifdef SDK_OS_FREE_RTOS
#define WR_FREERTOS_PROC_INTERVAL_MIN     10
#endif

/************************************************************************************
*************************************************************************************
* Public types
*************************************************************************************
************************************************************************************/
typedef enum bleConnStates_tag
{
    mConnIdle=1,
    mConnRequest,
    mConnSeek,
    mConnConnecting,
    mConnConnected,
    mConnDisconnect,
    mConnDisconnectRetry
} bleConnStates_t;

typedef enum bleLedStates_tag
{
    mLedOff,
    mLedIdle,
    mLedConnecting,
    mLedConnected,
} bleLedStates_t;

typedef struct bleInfo_tag
{
    bool_t advOn;
    bool_t scnOn;
    bool_t foundDeviceToConnect;
    uint16_t retries;
    bleConnStates_t state;
    bleLedStates_t ledstate;
    gapRole_t role;
    uint8_t max_connections;
    uint8_t address_list[gLclMaxConnections_c][gcBleDeviceAddressSize_c];
    uint16_t connInterval;
} bleInfo_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */
/* From app_config.c */
extern gapScanningParameters_t          gScanParams;
extern gapAdvertisingData_t             gAppAdvertisingData;
extern gapScanResponseData_t            gAppScanRspData;
extern gapAdvertisingParameters_t       gAdvParams;
extern gapPairingParameters_t           gPairingParameters;
extern gapConnectionRequestParameters_t gConnReqParams;

/*!
 * Global storage hosting configuration of local parameters, only affecting local node.
 */
extern isp_configuration_local_t *gLocalSettings;

/*!
 * Global storage hosting configuration of measurements.
 * This configuration will be shared over the air between central and peripheral nodes.
 */
extern isp_configuration_range_t *gRangeSettings;

/*!
 * Global storage hosting configuration of controller.
 * This configuration determines how BLE connections will be established.
 */
extern isp_configuration_communication_t *gCommunicationSettings;

extern uint16_t mAppMtu;

extern bleInfo_t bleInfo;


#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
uint8_t BleInfo_GetConnections
(
    bool_t andActive
);

bleResult_t BleApp_SendMessage
(
    uint8_t deviceId,
    uint8_t *pRecvStream,
    uint16_t streamSize
);

void BleApp_SetLED(bleLedStates_t state);
bool_t BleInfo_IsConnected(void);
deviceId_t BleApp_address_2_device_id(uint8_t *address);

void isp_globals_init(void);
void isp_globals_save(void);

void isp_save_calibration_data
(
    board_ranging_calibration_t *cal_data
);

#ifdef __cplusplus
}
#endif


#endif /* _APP_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
