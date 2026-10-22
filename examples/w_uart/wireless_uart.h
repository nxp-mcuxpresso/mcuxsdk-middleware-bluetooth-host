/*! *********************************************************************************
 * \defgroup Wireless UART Application
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2023, 2026 NXP
*
*
* \file
*
* This file is the interface file for the Wireless UART application
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef WIRELESS_UART_H
#define WIRELESS_UART_H

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/

/* Profile Parameters */
#define gScanningTime_c        10   /* 10 s*/

/* Enable/Disable Controller Adv/Scan/Connection Notifications */
#ifndef gUseControllerNotifications_c
#define gUseControllerNotifications_c 0
#endif

/* Receive Notifications in a callback registered in the Controller
   instead of Host GAP Generic callback */
#ifndef gUseControllerNotificationsCallback_c
#define gUseControllerNotificationsCallback_c 0
#endif

/* Enable/Disable the controller CE (Connection Event) RSSI Monitoring demo.
   This is a low-power alternative to the gNotifConnEventOver_c notification: the
   controller buffers RSSI samples and reports a whole batch after a configurable
   number of connection events. It is not role specific and works on both the
   central and the peripheral. Requires a controller that supports CE RSSI
   Monitoring. Disabled by default. */
#ifndef gAppCeRssiMonitoring_c
#define gAppCeRssiMonitoring_c 0
#endif

/* Number of connection events (RSSI samples) the controller buffers before it
   sends a CE RSSI Monitoring report to the host. Used only when
   gAppCeRssiMonitoring_c is enabled. Must not exceed gGapMaxCeRssiSamples_c. */
#ifndef gAppCeRssiMonitoringConnIntervals_c
#define gAppCeRssiMonitoringConnIntervals_c 20
#endif

#if (gAppCeRssiMonitoringConnIntervals_c > gGapMaxCeRssiSamples_c)
#error "gAppCeRssiMonitoringConnIntervals_c must not exceed gGapMaxCeRssiSamples_c"
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

#ifndef gWuart_AutoStart_c
#define gWuart_AutoStart_c 0
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
extern gapConnectionRequestParameters_t gConnReqParams;
extern gapScanningParameters_t          gScanParams;
extern gapAdvertisingData_t             gAppAdvertisingData;
extern gapScanResponseData_t            gAppScanRspData;
extern gapAdvertisingParameters_t       gAdvParams;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif


#endif /* _APP_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
