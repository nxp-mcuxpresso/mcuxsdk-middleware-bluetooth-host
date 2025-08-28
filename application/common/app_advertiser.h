/*! *********************************************************************************
* Copyright 2024-2025 NXP
*
*
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef APP_ADVERTISER_H
#define APP_ADVERTISER_H

#include "app_conn.h"

/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */
typedef struct appAdvertisingParams_tag
{
    gapAdvertisingParameters_t *pGapAdvParams;        /*!< Pointer to the GAP advertising parameters */
    const gapAdvertisingData_t *pGapAdvData;          /*!< Pointer to the GAP advertising data  */
    const gapScanResponseData_t *pScanResponseData;   /*!< Pointer to the scan response data */
} appAdvertisingParams_t;

typedef struct appExtAdvertisingParams_tag
{
    gapExtAdvertisingParameters_t *pGapExtAdvParams;   /*!< Pointer to the GAP extended advertising parameters */
    gapAdvertisingData_t *pGapAdvData;                /*!< Pointer to the GAP advertising data  */
    gapScanResponseData_t *pScanResponseData;         /*!< Pointer to the scan response data */
#if defined(gBLE60_DecisionBasedAdvertisingFilteringSupport_d) && (gBLE60_DecisionBasedAdvertisingFilteringSupport_d == TRUE)
    gapAdvertisingDecisionData_t* pGapDecisionData;
#endif /* gBLE60_DecisionBasedAdvertisingFilteringSupport_d */
    uint8_t                     handle;
    uint16_t                    duration;
    uint8_t                     maxExtAdvEvents;
} appExtAdvertisingParams_t;

/*! *********************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */
extern gapAdvertisingCallback_t pfAdvCallback;
extern appAdvertiserHandler_t pfAdvertiserHandler;

/*
	Default value for this define - application may override it in app_preinclude.h
	Enables APIs to set EAD key material and reencrypt the advertising data for a given set
*/
#ifndef gReencryptAdvDataOnRpaChange_d
#define gReencryptAdvDataOnRpaChange_d 0U
#endif
/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\fn           bleResult_t BluetoothLEHost_StartAdvertising(
*                  appAdvertisingParams_t   *pAdvParams,
*                  gapAdvertisingCallback_t pfAdvertisingCallback,
*                  gapConnectionCallback_t  pfConnectionCallback
*              )
*\brief        Set advertising data, set advertising parameters and start advertising.
*
*\param  [in]  pAdvParams               Pointer to the structure containing the
*                                       advertising parameters.
*\param  [in]  pfAdvertisingCallback    Callback used by the application to receive
*                                       advertising events. Can be NULL.
*\param  [in]  pfConnectionCallback     Callback used by the application to receive
*                                       connection events. Can be NULL.
*
*\return       bleResult_t              Result of the operation.
********************************************************************************** */
bleResult_t BluetoothLEHost_StartAdvertising
(
    appAdvertisingParams_t   *pAdvParams,
    gapAdvertisingCallback_t pfAdvertisingCallback,
    gapConnectionCallback_t  pfConnectionCallback
);

/*! *********************************************************************************
*\fn           bleResult_t BluetoothLEHost_StartExtAdvertising(
*                   appExtAdvertisingParams_t *pExtAdvParams,
*                   gapAdvertisingCallback_t  pfAdvertisingCallback,
*                   gapConnectionCallback_t   pfConnectionCallback
*               )
*\brief        Set advertising data, set advertising parameters and start extended
*              advertising.
*
*\param  [in]  pAdvParams               Pointer to the structure containing the
*                                       advertising.
*\param  [in]  pfAdvertisingCallback    Callback used by the application to receive
*                                       advertising events. Can be NULL.
*\param  [in]  pfConnectionCallback     Callback used by the application to receive
*                                       connection events. Can be NULL.
*
*\return       bleResult_t              Result of the operation.
********************************************************************************** */
bleResult_t BluetoothLEHost_StartExtAdvertising
(
    appExtAdvertisingParams_t *pExtAdvParams,
    gapAdvertisingCallback_t  pfAdvertisingCallback,
    gapConnectionCallback_t   pfConnectionCallback
);

#if defined(gReencryptAdvDataOnRpaChange_d) && (gReencryptAdvDataOnRpaChange_d == 1U)
/*! *************************************************************************************
*\fn           bleResult_t BluetoothLEHost_SetEadKeyMaterial(uint8_t *pKey, uint8_t *pIv)
*\brief        Set the key and initialization vector used to encrypt advertising data.
*
*\param  [in]  pKey     Pointer to 16-byte key.
*\param  [in]  pIV      Pointer to 8-byte initialization vector.
*
*\return       None
************************************************************************************** */
void BluetoothLEHost_SetEadKeyMaterial
(
    uint8_t *pKey,
    uint8_t *pIv
);

/*! *************************************************************************************
*\fn           bleResult_t BluetoothLEHost_ReencryptAdvertisingData(uint8_t advHandle,
*              const gapAdvertisingData_t *pAdvData)
*
*\brief        Re-encrypt the advertising data for the set identified by advHandle using
*              the previously set key and IV and a new randomizer. Should be called by the
*              application when the RPA for the set changes (on gRandomAddressSet_c
*              generic event).
*
*\param  [in]  advHandle    Handle identifying the advertising set
*\param  [in]  pAdvData    Pointer to advertising data to be encrypted
*
*\return       bleResult_t
************************************************************************************** */
bleResult_t BluetoothLEHost_ReencryptAdvertisingData
(
    uint8_t advHandle,
    const gapAdvertisingData_t *pAdvData
);
#endif /* gReencryptAdvDataOnRpaChange_d */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* APP_ADVERTISER_H */
