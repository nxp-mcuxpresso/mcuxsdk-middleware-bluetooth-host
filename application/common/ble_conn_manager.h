/*! *********************************************************************************
 * \addtogroup BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2016-2023, 2025 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef BLE_CONN_MANAGER_H
#define BLE_CONN_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
*************************************************************************************
* Includes
*************************************************************************************
************************************************************************************/
#include "gap_types.h"
#include "SecLib.h"
/************************************************************************************
*************************************************************************************
* Public Macros
*************************************************************************************
************************************************************************************/

/*! Enable / Disable Connection Update procedure on Peripheral */
#ifndef gConnUpdateAlwaysAccept_d
#define gConnUpdateAlwaysAccept_d               1
#endif /* gConnUpdateAlwaysAccept_d */

/*! Minimum connection interval for accepting an update */
#ifndef gConnUpdateIntervalMin_d
#define gConnUpdateIntervalMin_d                gGapConnIntervalMin_d
#endif /* gConnUpdateIntervalMin_d */

/*! Maximum connection interval for accepting an update */
#ifndef gConnUpdateIntervalMax_d
#define gConnUpdateIntervalMax_d                gGapConnIntervalMax_d
#endif /* gConnUpdateIntervalMax_d */

/*! Minimum latency for accepting an update */
#ifndef gConnUpdateLatencyMin_d
#define gConnUpdateLatencyMin_d                 gGapConnLatencyMin_d
#endif /* gConnUpdateLatencyMin_d */

/*! Maximum latency for accepting an update */
#ifndef gConnUpdateLatencyMax_d
#define gConnUpdateLatencyMax_d                 gGapConnLatencyMax_d
#endif /* gConnUpdateLatencyMax_d */

/*! Minimum supervision timeout for accepting an update */
#ifndef gConnUpdateSuperTimeoutMin_d
#define gConnUpdateSuperTimeoutMin_d            gGapConnSuperTimeoutMin_d
#endif /* gConnUpdateSuperTimeoutMin_d */

/*! Maximum supervision timeout for accepting an update */
#ifndef gConnUpdateSuperTimeoutMax_d
#define gConnUpdateSuperTimeoutMax_d            gGapConnSuperTimeoutMax_d
#endif /* gConnUpdateSuperTimeoutMax_d */

/*! Change ECDH public-private key pair after this number of
pairing attempts */
#ifndef gConnPairChangeKeyThreshold_d
#define gConnPairChangeKeyThreshold_d    (8U)
#endif /* gConnPairChangeKeyThreshold_d */

/*! One failed pairing counts as this number of successful pairings */
#ifndef gConnPairFailToSucessCount_c
#define gConnPairFailToSucessCount_c            (3U)
#endif /* gConnPairFailToSucessCount_c */

/*! Controller privacy, default enabled */
#ifndef gBleEnableControllerPrivacy_d
#define gBleEnableControllerPrivacy_d           (1U)
#endif /* gBleEnableControllerPrivacy_d */

/*! specifies host preferred values for the transmitter PHY to be used
for all subsequent connections over the LE transport */

#ifndef gConnDefaultTxPhySettings_c
#define gConnDefaultTxPhySettings_c             (gLePhy1MFlag_c | gLePhy2MFlag_c)
#endif /* gConnDefaultTxPhySettings_c */

/*! specifies host preferred values for the receiver PHY to be used for all
subsequent connections over the LE transport */
#ifndef gConnDefaultRxPhySettings_c
#define gConnDefaultRxPhySettings_c             (gLePhy1MFlag_c | gLePhy2MFlag_c)
#endif /* gConnDefaultRxPhySettings_c */

/*! specifies whether the phy update procedure is going to be initiated in
connection or not */
#ifndef gConnInitiatePhyUpdateRequest_c
#define gConnInitiatePhyUpdateRequest_c         (0U)
#endif /* gConnInitiatePhyUpdateRequest_c */

/*! specifies preferred Tx Phy Settings for the phy update procedure*/
#ifndef gConnPhyUpdateReqTxPhySettings_c
#define gConnPhyUpdateReqTxPhySettings_c        (gLePhy1MFlag_c | gLePhy2MFlag_c)
#endif /* gConnPhyUpdateReqTxPhySettings_c */

/*! specifies preferred Rx Phy Settings for the phy update procedure*/
#ifndef gConnPhyUpdateReqRxPhySettings_c
#define gConnPhyUpdateReqRxPhySettings_c        (gLePhy1MFlag_c | gLePhy2MFlag_c)
#endif /* gConnPhyUpdateReqRxPhySettings_c */

/*! specifies preferred Phy Options coding when transmitting on the
LE Coded PHY*/
#ifndef gConnPhyUpdateReqPhyOptions_c
#define gConnPhyUpdateReqPhyOptions_c           (gLeCodingNoPreference_c)
#endif /* gConnPhyUpdateReqPhyOptions_c */

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */
extern gapSmpKeys_t                     gSmpKeys;
extern bleDeviceAddress_t               gaBleDeviceAddress;
extern bool_t                           gSettingRandomStaticAddress;

#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
extern uint8_t gcBondedDevices;
#endif /* gAppUseBonding_d */

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
extern gapPairingParameters_t           gPairingParameters;
extern gapDeviceSecurityRequirements_t  deviceSecurityRequirements;
#endif /* gAppUsePairing_d */

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn           void BleConnManager_GenericEvent(gapGenericEvent_t* pGenericEvent)
*\brief        Performs common operations on the BLE stack on the generic callback.
*
*\param  [in]  pGenericEvent    GAP Generic event from the Host Stack.
*
*\retval       void.
********************************************************************************** */
void BleConnManager_GenericEvent(gapGenericEvent_t* pGenericEvent);

/*! *********************************************************************************
*\fn           void BleConnManager_GapCommonConfig(void)
*\brief        Performs common GAP configuration.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void BleConnManager_GapCommonConfig(void);

/*! *********************************************************************************
*\fn           void BleConnManager_GapPeripheralEvent(
*                  deviceId_t            peerDeviceId,
*                  gapConnectionEvent_t* pConnectionEvent)
*\brief        Performs GAP Peripheral operations on the BLE stack on the
*              connection callback.
*
*\param  [in]  peerDeviceId        The GAP peer Id.
*
*\param  [in]  pConnectionEvent    GAP Connection event from the Host Stack.
*
*\retval       void.
********************************************************************************** */
void BleConnManager_GapPeripheralEvent
(
    deviceId_t            peerDeviceId,
    gapConnectionEvent_t* pConnectionEvent
);

/*! *********************************************************************************
*\fn           void BleConnManager_GapCentralEvent(
*                  deviceId_t            peerDeviceId,
*                  gapConnectionEvent_t* pConnectionEvent)
*\brief        Performs GAP Central operations on the BLE stack on the
*              connection callback.
*
*\param  [in]  peerDeviceId        The GAP peer Id.
*
*\param  [in]  pConnectionEvent    GAP Connection event from the Host Stack.
*
*\retval       void.
********************************************************************************** */
void BleConnManager_GapCentralEvent
(
    deviceId_t            peerDeviceId,
    gapConnectionEvent_t* pConnectionEvent
);

/*! *********************************************************************************
*\fn           bleResult_t BleConnManager_EnablePrivacy(void)
*\brief        Checks bonding devices and enables controller/host privacy.
*
*\param  [in]  none.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************** */
bleResult_t BleConnManager_EnablePrivacy(void);

/*! *********************************************************************************
*\fn           bleResult_t BleConnManager_DisablePrivacy(void)
*\brief        Checks enabled privacy and disables controller/host privacy.
*
*\param  [in]  none.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************** */
bleResult_t BleConnManager_DisablePrivacy(void);

#ifdef __cplusplus
}
#endif

#endif /* BLE_CONN_MANAGER_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
