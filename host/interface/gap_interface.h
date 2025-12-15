/*! *********************************************************************************
 * \defgroup GAP Generic Access Profile
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2025 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef GAP_INTERFACE_H
#define GAP_INTERFACE_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ble_general.h"
#include "gap_types.h"
#include "gatt_types.h"
#include "gatt_database.h"
#include "SecLib.h"

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  Macro used to combine two security mode-levels.
*
* \param[in] modeLevelA,modeLevelB The two security mode-levels.
*
* \return  The resulting security mode-level.
*
* \remarks This macro is useful when two different security requirements
* must be satisfied at the same time, such as a device central security requirement
* and a service-specific security requirement.
*
********************************************************************************** */
#define Gap_AddSecurityModesAndLevels(modeLevelA, modeLevelB) \
   ((isSameMode((modeLevelA), (modeLevelB))) ? \
    (addSameSecurityModes((modeLevelA), (modeLevelB))) : \
    (addDifferentSecurityModes((modeLevelA), (modeLevelB))))

/*! *********************************************************************************
* \brief  Macro used to cancel a connection initiated by Gap_Connect(...).
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks This macro can only be used for a connection that has not yet been established, such as
* the "gConnEvtConnected_c" has not been received. For example, call this when a connection
* request has timed out. Application should listen for gCreateConnectionCanceled_c generic event.
********************************************************************************** */
#define Gap_CancelInitiatingConnection()\
    Gap_Disconnect(gCancelOngoingInitiatingConnection_d)

/*! *********************************************************************************
* \brief  Macro used to read the radio transmitter power when advertising.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks The result is contained in the gAdvTxPowerLevelRead_c generic event.
********************************************************************************** */
#define Gap_ReadAdvertisingTxPowerLevel()\
    Gap_ReadRadioPowerLevel(gTxPowerLevelForAdvertising_c, 0U)

/*! *********************************************************************************
* \brief  Macro used to read the RSSI of a radio connection.
*
* \param[in] deviceId Device ID identifying the radio connection.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks The result is contained in the gConnEvtRssiRead_c connection event. The RSSI
* value is a signed byte, and the unit is dBm. If the RSSI cannot be read, the gConnEvtPowerReadFailure_c
* connection event is generated.
********************************************************************************** */
#define Gap_ReadRssi(deviceId)\
    Gap_ReadRadioPowerLevel(gRssi_c, (deviceId))

/*! *********************************************************************************
* \brief  Macro used to read the radio transmitting power level of a radio connection.
*
* \param[in] deviceId Device ID identifying the radio connection.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks The result is contained in the gConnEvtTxPowerLevelRead_c connection event.
* If the TX Power cannot be read, the gConnEvtPowerReadFailure_c
* connection event is generated.
********************************************************************************** */
#define Gap_ReadTxPowerLevelInConnection(deviceId)\
    Gap_ReadRadioPowerLevel(gTxPowerCurrentLevelInConnection_c, (deviceId))

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!*************************************************************************************************
 * \fn           bleResult_t Gap_SetDataRelatedAddressChanges(uint8_t advertisingHandle, uint8_t changeReasons)
 *
 * \brief        Command specifies circumstances when the Controller shall refresh any Resolvable Private Address used by the
 *               advertising set identified by the Advertising_Handle parameter.
 *
 * \param [in]   advertisingHandle       Identify an advertising set.
 * \param [in]   changeReasons           Change reasons. Any combination of gAdvDataChange_c and gScanRspDataChange_c. 0U to disable the feature.
 *
 * \retval       gBleSuccess_c
 * \retval       gBleOutOfMemory_c       Cannot allocate memory for the Host task.
 * \retval       gBleInvalidParameter_c  A parameter has an invalid value or is outside
 *                                       the accepted range
 *
***************************************************************************************************/
bleResult_t Gap_SetDataRelatedAddressChanges
(
    uint8_t advertisingHandle,
    uint8_t changeReasons
);

/*! *********************************************************************************
* \brief  Registers the device security requirements.
* This function includes a central security for all services and, optionally, additional stronger security
* settings for services as required by the profile and/or application.
*
* \param[in] pSecurity A pointer to the application-allocated gapDeviceSecurityRequirements_t structure.
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c   A parameter has an invalid value or is outside
*                                   the accepted range
*
* \remarks pSecurity or any other contained security structure pointers that are NULL are ignored,
* i.e., defaulted to No Security (Security Mode 1 Level 1, No Authorization, Minimum encryption key size).
* This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_RegisterDeviceSecurityRequirements
(
    const gapDeviceSecurityRequirements_t* pSecurity
);

/*! *********************************************************************************
* \brief  Sets up the Advertising Parameters.
*
* \param[in] pAdvertisingParameters   Pointer to gapAdvertisingParameters_t structure.
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_SetAdvertisingParameters
(
    const gapAdvertisingParameters_t*   pAdvertisingParameters
);

/*! *********************************************************************************
* \brief  Sets up the Advertising and Scan Response Data.
*
* \param[in] pAdvertisingData   Pointer to gapAdvertisingData_t structure or NULL.
* \param[in] pScanResponseData  Pointer to gapScanResponseData_t structure or NULL.
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range
* \retval  gGapAdvDataTooLong_c         The advertising data length is too big
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by
*                                       this stack version
*
* \remarks Any of the parameters may be NULL, in which case they are ignored.
* Therefore, this function can be used to set any of the parameters individually or both at once.
* The standard advertising packet payload is 37 bytes. Some of the payload may be occupied by
* the Advertiser Address which takes up 6 bytes and for some advertising PDU types also by
* the Initiator Address which takes another 6 bytes. This leaves 25-31 bytes to the application
* to include advertising structures (Length [1Byte], AD Type [1 Byte], AD Data[Length-1 Bytes])
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_SetAdvertisingData
(
    const gapAdvertisingData_t*         pAdvertisingData,
    const gapScanResponseData_t*        pScanResponseData
);

/*! *********************************************************************************
* \brief  Commands the controller to start advertising.
*
* \param[in] advertisingCallback   Callback used by the application to receive advertising events.
*                                  Can be NULL.
* \param[in] connectionCallback    Callback used by the application to receive connection events.
*                                  Can be NULL.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleInvalidState_c           Advertising is already started or pending to
*                                       be started
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by
*                                       this stack version
*
* \remarks The advertisingCallback confirms or denies whether the advertising has started.
* The connectionCallback is only used if a connection gets established during advertising.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_StartAdvertising
(
    gapAdvertisingCallback_t    advertisingCallback,
    gapConnectionCallback_t     connectionCallback
);

/*! *********************************************************************************
* \brief  Commands the controller to stop advertising.
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidState_c           Advertising is already stopped or pending to
*                                       be stopped
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by
*                                       this stack version
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_StopAdvertising(void);

/*! *********************************************************************************
* \brief  Authorizes a peer for an attribute in the database.
*
* \param[in] deviceId   The peer being authorized.
* \param[in] handle     The attribute handle.
* \param[in] access     The type of access granted (gAccessRead_c or gAccessWrite_c).
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range
* \retval  gBleOutOfMemory_c            The nvm index of the deviceId is bigger than
*                                       the number of allowed bonded devices, or the
*                                       nvm saving operation did not have enough memory
* \retval  gBleInvalidParameter_c       The deviceId parameter is not valid or the
*                                       access parameter is not gAccessRead_c or
*                                       gAccessWrite_c
* \retval  gBleUnexpectedError_c        Saving to NVM failed (module not initialized
*                                       or corrupted NVM)
* \retval  gBleOverflow_c               the number of handles to be read or written
*                                       from the NVM is bigger than
*                                       gcGapMaxAuthorizationHandles_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
*
* \remarks This function executes synchronously.
*
* \remarks GATT Server-only API function.
*
********************************************************************************** */
bleResult_t Gap_Authorize
(
    deviceId_t          deviceId,
    uint16_t            handle,
    gattDbAccessType_t  access
);

/*! *********************************************************************************
* \brief  Save the CCCD value for a specific Client and CCCD handle.
*
* \param[in] deviceId    The peer GATT Client.
* \param[in] handle      The handle of the CCCD as defined in the GATT Database.
* \param[in] cccd        The bit mask representing the CCCD value to be saved.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range
* \retval  gBleUnexpectedError_c        Saving to NVM failed (module not initialized
*                                       or corrupted NVM)
* \retval  gBleOutOfMemory_c            The nvm index of the deviceId is bigger than
*                                       the number of allowed bonded devices, or the
*                                       nvm saving operation did not have enough memory
* \retval  gDevDbCccdLimitReached_c     No more room in NVM
*
* \remarks The GATT Server layer saves the CCCD value automatically when it is written by the Client.
* This API should only be used to save the CCCD in other situations, e.g., when for some reason
* the application decides to disable notifications/indications for a specific Client.
*
* \remarks This function executes synchronously.
*
* \remarks GATT Server-only API function.
*
********************************************************************************** */
bleResult_t Gap_SaveCccd
(
    deviceId_t      deviceId,
    uint16_t        handle,
    gattCccdFlags_t cccd
);

/*! *********************************************************************************
* \brief  Retrieves the notification status for a given Client and a given CCCD handle.
*
* \param[in]  deviceId      The peer GATT Client.
* \param[in]  handle        The handle of the CCCD.
* \param[out] pOutIsActive  The address to store the status into.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range
* \retval  gDevDbCccdNotFound_c         The handle is not CCCD
* \retval  gBleUnavailable_c            The bond data entry for this device is
*                                       corrupted
*
* \remarks This function executes synchronously.
*
* \remarks GATT Server-only API function.
*
********************************************************************************** */
bleResult_t Gap_CheckNotificationStatus
(
    deviceId_t  deviceId,
    uint16_t    handle,
    bool_t*     pOutIsActive
);

/*! *********************************************************************************
* \brief  Retrieves the indication status for a given Client and a given CCCD handle.
*
* \param[in]  deviceId      The peer GATT Client.
* \param[in]  handle        The handle of the CCCD.
* \param[out] pOutIsActive  The address to store the status into.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gDevDbCccdNotFound_c         The handle is not CCCD.
* \retval  gBleUnavailable_c            The bond data entry for this device is
*                                       corrupted.
*
* \remarks This function executes synchronously.
*
* \remarks GATT Server-only API function.
*
********************************************************************************** */
bleResult_t Gap_CheckIndicationStatus
(
    deviceId_t  deviceId,
    uint16_t    handle,
    bool_t*     pOutIsActive
);

/*! *********************************************************************************
* \brief  Retrieves a list of the identity information of bonded devices, if any.
*
* \param[out] aOutIdentityAddresses  Array of identities to be filled.
* \param[in]  maxDevices             Maximum number of identities to be obtained.
* \param[out] pOutActualCount        The actual number of identities written.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleInvalidParameter_c       A parameter has an invalid value, is
*                                       outside the accepted range, or the NVM entry
*                                       is invalid
*
* \remarks This API may be useful when creating a filter accept list or a resolving list.
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_GetBondedDevicesIdentityInformation
(
    gapIdentityInformation_t*   aOutIdentityAddresses,
    uint8_t                     maxDevices,
    uint8_t*                    pOutActualCount
);

/*! *********************************************************************************
* \brief  Initiates pairing with a peer device.
*
* \param[in] deviceId            The peer to pair with.
* \param[in] pPairingParameters  Pairing parameters as required by the SMP.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task
*
* \remarks GAP Central-only API function.
*
********************************************************************************** */
bleResult_t Gap_Pair
(
    deviceId_t                        deviceId,
    const gapPairingParameters_t*     pPairingParameters
);

/*! *********************************************************************************
* \brief  Informs the peer Central about the local security requirements.
*
* \param[in] deviceId               The GAP peer to pair with.
* \param[in] pPairingParameters     Pairing parameters as required by the SMP.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task
*
* \remarks The procedure has the same parameters as the pairing request, but, because it is initiated
* by the Peripheral, it has no pairing effect. It only informs the Central about the requirements.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_SendPeripheralSecurityRequest
(
    deviceId_t                      deviceId,
    const gapPairingParameters_t    *pPairingParameters
);

/*! *********************************************************************************
* \brief  Encrypts the link with a bonded peer.
*
* \param[in] deviceId   Device ID of the peer.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gGapDeviceNotBonded_c        Trying to execute an API that is only available
*                                       for bonded devices.
* \remarks GAP Central-only API function.
*
********************************************************************************** */
bleResult_t Gap_EncryptLink
(
    deviceId_t                  deviceId
);

/*! *********************************************************************************
* \brief  Accepts the pairing request from a peer.
*
* \param[in] deviceId            The peer requesting authentication.
* \param[in] pPairingParameters  Pairing parameters as required by the SMP.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \remarks This should be called in response to a gPairingRequest_c event.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_AcceptPairingRequest
(
    deviceId_t                        deviceId,
    const gapPairingParameters_t*     pPairingParameters
);

/*! *********************************************************************************
* \brief  Rejects the peer's authentication request.
*
* \param[in] deviceId The GAP peer who requested authentication.
*
* \param[in] reason Reason why the current device rejects the authentication.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
********************************************************************************** */
bleResult_t Gap_RejectPairing
(
    deviceId_t                      deviceId,
    gapAuthenticationRejectReason_t reason
);

/*! *********************************************************************************
* \brief  Enters the passkey requested by the peer during the pairing process.
*
* \param[in] deviceId      The GAP peer that requested a passkey entry.
* \param[in] passkey       The peer's secret passkey.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
********************************************************************************** */
bleResult_t Gap_EnterPasskey
(
    deviceId_t  deviceId,
    uint32_t    passkey
);

/*! *********************************************************************************
* \brief  Provides the Out-Of-Band data for the SMP Pairing process.
*
* \param[in] deviceId   The pairing device.
* \param[in] aOob       Pointer to OOB data (array of gcSmpOobSize_c size).
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
********************************************************************************** */
bleResult_t Gap_ProvideOob
(
    deviceId_t      deviceId,
    const uint8_t*  aOob
);

/*! *********************************************************************************
* \brief  Rejects the passkey request from a peer.
*
* \param[in] deviceId The GAP peer that requested a passkey entry.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.

* \remarks GAP Central-only API function.
*
********************************************************************************** */
bleResult_t Gap_RejectPasskeyRequest
(
    deviceId_t deviceId
);

/*! *********************************************************************************
* \brief  Sends the SMP keys during the SMP Key Exchange procedure.
*
* \param[in] deviceId   The GAP peer who initiated the procedure.
* \param[in] pKeys      The SMP keys of the local device.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
********************************************************************************** */
bleResult_t Gap_SendSmpKeys
(
    deviceId_t      deviceId,
    const gapSmpKeys_t*   pKeys
);

/*! *********************************************************************************
* \brief  Rejects the Key Exchange procedure with a paired peer.
*
* \param[in] deviceId  The GAP peer who requested the Key Exchange procedure.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
********************************************************************************** */
bleResult_t Gap_RejectKeyExchangeRequest
(
    deviceId_t deviceId
);

/*! *********************************************************************************
* \brief  Regenerates the private/public key pair used for LE Secure Connections pairing.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks The application should listen for the gLeScPublicKeyRegenerated_c generic event.
*
********************************************************************************** */
bleResult_t Gap_LeScRegeneratePublicKey(void);

/*! *********************************************************************************
* \brief     Validates the numeric value during the Numeric Comparison LE Secure Connections pairing.
*
* \param[in] deviceId  Device ID of the peer.
*
* \param[in] valid     TRUE if user has indicated that numeric values are matched, FALSE otherwise.
*
* \retval    gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
********************************************************************************** */
bleResult_t Gap_LeScValidateNumericValue
(
    deviceId_t  deviceId,
    bool_t      valid
);

/*! *********************************************************************************
* \brief  Retrieves local OOB data used for LE Secure Connections pairing.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks The application should listen for the gLeScLocalOobData_c generic event.
*
********************************************************************************** */
bleResult_t Gap_LeScGetLocalOobData(void);

/*! *********************************************************************************
* \brief     Sets peer OOB data used for LE Secure Connections pairing.
*
* \param[in] deviceId       Device ID of the peer.
*
* \param[in] pPeerOobData   OOB data received from the peer.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \remarks This function should be called in response to the gConnEvtLeScOobDataRequest_c event.
*
********************************************************************************** */
bleResult_t Gap_LeScSetPeerOobData
(
    deviceId_t                    deviceId,
    const gapLeScOobData_t*       pPeerOobData
);

/*! *********************************************************************************
* \brief     Sends a Keypress Notification to the peer.
*
* \param[in] deviceId               Device ID of the peer.
* \param[in] keypressNotification   Value of the Keypress Notification.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks   This function shall only be called during the passkey entry process and
             only if both peers support Keypress Notifications.
*
********************************************************************************** */
bleResult_t Gap_LeScSendKeypressNotification
(
    deviceId_t                  deviceId,
    gapKeypressNotification_t   keypressNotification
);

/*! *********************************************************************************
* \brief  Provides the Long Term Key (LTK) to the controller for encryption setup.
*
* \param[in] deviceId   The GAP peer who requested encryption.
* \param[in] aLtk       The Long Term Key.
* \param[in] ltkSize    The Long Term Key size.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
* \remarks The application should provide the same LTK used during bonding with the respective peer.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_ProvideLongTermKey
(
    deviceId_t        deviceId,
    const uint8_t*    aLtk,
    uint8_t           ltkSize
);

/*! *********************************************************************************
* \brief  Rejects an LTK request originating from the controller.
*
* \param[in] deviceId The GAP peer who requested encryption.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_DenyLongTermKey
(
    deviceId_t deviceId
);

/*! *********************************************************************************
* \brief  Loads the encryption key for a bonded device.
*
* \param[in]  deviceId          Device ID of the peer.
* \param[out] aOutLtk           Array of size gcMaxLtkSize_d to be filled with the LTK.
* \param[out] pOutLtkSize       The LTK size.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleUnavailable_c            The bond data entry for this device is
*                                       corrupted.
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_LoadEncryptionInformation
(
    deviceId_t   deviceId,
    uint8_t*     aOutLtk,
    uint8_t*     pOutLtkSize
);

/*! *********************************************************************************
* \brief  Sets the SMP passkey for this device.
*
* \param[in] passkey    The SMP passkey.
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c           A parameter has an invalid value or is
*                                           outside the accepted range.
* \retval  gSmInvalidDeviceId_c             The device ID is not valid or it has been
*                                           disconnected in the meantime.
* \retval  gSmInvalidCommandCode_c          An invalid internal state machine code was
*                                           requested.
* \retval  gSmPairingAlreadyStarted_c       Pairing process was already started.
* \retval  gSmInvalidInternalOperation_c    A memory corruption or invalid operation
*                                           may have occurred.
* \retval  gSmSmpTimeoutOccurred_c          An SMP timeout has occurred for the peer
*                                           device.
* \retval  gSmInvalidCommandParameter_c     One of the parameters of the SM command is
*                                           not valid.
* \retval  gSmCommandNotSupported_c         The Security Manager does not have the
*                                           required features or version to support
*                                           this command
* \retval  gSmUnexpectedCommand_c           This command is not or cannot be handled in
*                                           the current context of the SM.
* \retval  gSmSmpPacketReceivedAfterTimeoutOccurred_c   A SMP packet has been received
*                                                       from a peer device for which a
*                                                       pairing procedure has timed out.
* \retval  gSmUnexpectedPairingTerminationReason_c      The upper layer tried to cancel
*                                                       the pairing procedure with an
*                                                       unexpected pairing failure
*                                                       reason for the current phase of
*                                                       the pairing procedure.
* \remarks This is the PIN that the peer's user must enter during pairing.
*
* \remarks This function executes synchronously.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_SetLocalPasskey
(
    uint32_t passkey
);

/*! *********************************************************************************
* \brief  Sets internal scan filters and actions.
*
* \param[in] scanMode           The scan mode to be activated. Default is gDefaultScan_c.
* \param[in] pAutoConnectParams Pointer to the gapAutoConnectParams_t structures if scanMode
*                               is set to gAutoConnect_c. The memory used must be persistent
*                               and should not change during the next scan periods or until
*                               another Gap_SetScanMode is called.
* \param[in] connCallback       Auto-Connect callback. Must be set if scanMode is set to gAutoConnect_c.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \remarks This function can be called before Gap_StartScanning. If this function is
* never called, then the default value of gDefaultScan_c is considered and all scanned
* devices are reported to the application without any additional filtering or action.
*
* \remarks This function executes synchronously.
*
* \remarks GAP Central-only API function.
*
********************************************************************************** */
bleResult_t Gap_SetScanMode
(
    gapScanMode_t           scanMode,
    gapAutoConnectParams_t* pAutoConnectParams,
    gapConnectionCallback_t connCallback
);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_SetDecisionInstructions(uint8_t numTest,
*       const gapDecisionInstructionsData_t *pDecisionInstructions)
*
* \brief  Sets up the Decision Instructions.
*
* \param[in] numTest             The number of tests in the decision instructions.
* \param[in] pDecisionInstructions   Pointer to an array of gapDecisionInstructionsData_t structures.
*                                    containing decision instruction for each test.
*
* \retval  gBleFeatureNotSupported_c in case gLeExtendedAdv_c or gLeDecisionBasedAdvertisingFiltering_c are not supported.
* \retval  gBleInvalidParameter_c in case at least 1 parameter is incorrect.
* \retval  gBleOutOfMemory_c in case the memory allocation for the app to host message fails.
* \retval  gBleSuccess_c otherwise.
*
* \remarks GAP Central-only API function.
*
* \remarks Application should wait for gDecisionInstructionsSetupComplete_c
*          generic event or for gInternalError_c generic event with errorSource = gSetDecisionInstructions_c.
*
********************************************************************************** */
bleResult_t Gap_SetDecisionInstructions
(
    uint8_t numTest,
    const gapDecisionInstructionsData_t *pDecisionInstructions
);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_InitMonitoringAdvertisers(void)
*
*\brief Initialize function table for Monitored Advertisers functionality.
*
*\retval  gBleSuccess_c
*\retval  gBleFeatureNotSupported_c    Feature is not supported.
***************************************************************************************************/
bleResult_t Gap_InitMonitoringAdvertisers(void);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_AddDeviceToMonAdvList(bleAddressType_t addressType, 
*                                                           bleDeviceAddress_t address, 
*                                                           int8_t rssiLowThreshold, 
*                                                           int8_t rssiHighThreshold, 
*                                                           uint8_t timeout)
*
*\brief This function adds a device to the Monitored Advertisers List.
*
*\param [in]    addressType         Address type of the device.
*\param [in]    address             Address of the device.
*\param [in]    rssiLowThreshold    RSSI low threshold value.
*\param [in]    rssiHighThreshold   RSSI high threshold value.
*\param [in]    timeout             Timeout value.
*
*\retval  gBleSuccess_c
*\retval  gBleInvalidParameter_c       One or more parameters are invalid.
*\retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
***************************************************************************************************/
bleResult_t Gap_AddDeviceToMonAdvList
(
    bleAddressType_t    addressType,
    bleDeviceAddress_t  address,
    int8_t              rssiLowThreshold,
    int8_t              rssiHighThreshold,
    uint8_t             timeout
);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_RemoveDeviceFromMonAdvList(bleAddressType_t addressType, 
*                                                                bleDeviceAddress_t address)
*
*\brief This function removes a device from the Monitored Advertisers List.
*
*\param [in]    addressType         Address type of the device.
*\param [in]    address             Address of the device.
*
*\retval  gBleSuccess_c
*\retval  gBleInvalidParameter_c       One or more parameters are invalid.
*\retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
***************************************************************************************************/
bleResult_t Gap_RemoveDeviceFromMonAdvList
(
    bleAddressType_t    addressType,
    bleDeviceAddress_t  address
);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_ClearMonAdvList(void)
*
*\brief This function clears the Monitored Advertisers List.
*
*\retval  gBleSuccess_c
*\retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
***************************************************************************************************/
bleResult_t Gap_ClearMonAdvList(void);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_EnableMonAdv(bool_t enable)
*
*\brief This function enables or disables monitoring of advertisers.
*
*\param [in]    enable              Enable or disable monitoring.
*
*\retval  gBleSuccess_c
*\retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
***************************************************************************************************/
bleResult_t Gap_EnableMonAdv(bool_t enable);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_ReadMonAdvListSize(void)
*
*\brief This function reads the size of the Monitored Advertisers List.
*
*\retval  gBleSuccess_c
*\retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
***************************************************************************************************/
bleResult_t Gap_ReadMonAdvListSize(void);

/*! *********************************************************************************
* \brief  Optionally sets the scanning parameters and begins scanning.
*
* \param[in] pScanningParameters The scanning parameters; may be NULL.
* \param[in] scanningCallback The scanning callback.
* \param[in] enableFilterDuplicates Enable or disable duplicate advertising
*            report filtering
* \param[in] duration  Scan duration expressed in units of 10 ms.
*                      Set 0 for continuous scan until explicitly disabled.
*                      Used only for BLE5.0, otherwise ignored.
* \param[in] period    Time interval expressed in units of 1.28 seconds
*                      from when the Controller started its last Scan_Duration
*                      until it begins the subsequent Scan_Duration.
*                      Set 0 to disable periodic scanning.
*                      Used only for BLE5.0, otherwise ignored.
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidState_c           The requested API cannot be called in the
*                                       current state, scanning was already started.
* \remarks Use this API to both set the scanning parameters and start scanning.
* If pScanningParameters is NULL, scanning is started with the existing settings.
*
* \remarks GAP Central-only API function.
*
********************************************************************************** */
bleResult_t Gap_StartScanning
(
    const gapScanningParameters_t*    pScanningParameters,
    gapScanningCallback_t       scanningCallback,
    gapFilterDuplicates_t       enableFilterDuplicates,
    uint16_t                    duration,
    uint16_t                    period

);

/*! *********************************************************************************
* \brief  Commands the controller to stop scanning.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidState_c           The requested API cannot be called in the
*                                       current state, scanning is in the process of
*                                       stopping.
* \remarks GAP Central-only API function.
*
********************************************************************************** */
bleResult_t Gap_StopScanning(void);

/*! *********************************************************************************
* \brief  Connects to a scanned device.
*
* \param[in] pParameters  Create Connection command parameters.
* \param[in] connCallback Callback used to receive connection events.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \remarks GAP Central-only API function.
*
********************************************************************************** */
bleResult_t Gap_Connect
(
    const gapConnectionRequestParameters_t*   pParameters,
    gapConnectionCallback_t                   connCallback
);

/*! *********************************************************************************
* \brief  Connects to a device which is doing periodic advertising with responses.
*
* \param[in] pParameters  Create Connection command parameters.
* \param[in] connCallback Callback used to receive connection events.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \remarks GAP Central-only API function.
*
********************************************************************************** */
bleResult_t Gap_ConnectFromPawr
(
    const gapConnectionFromPawrParameters_t*   pParameters,
    gapConnectionCallback_t                    connCallback
);

/*! *********************************************************************************
* \brief  Initiates disconnection from a connected peer device.
*
* \param[in] deviceId The connected peer to disconnect from.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_Disconnect
(
    deviceId_t deviceId
);

/*! *********************************************************************************
* \brief  Saves custom peer information in raw data format.
*
* \param[in] deviceId          Device ID of the GAP peer.
* \param[in] pInfo             Pointer to the beginning of the data.
* \param[in] offset            Offset from the beginning of the reserved memory area.
* \param[in] infoSize          Data size (maximum equal to gcReservedFlashSizeForCustomInformation_d).
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            The nvm index of the deviceId is bigger than
*                                       the number of allowed bonded devices, or the
*                                       nvm saving operation did not have enough memory.
* \remarks This function can be called by the application to save custom information about the
* peer device, e.g., Service Discovery data (to avoid doing it again on reconnection).
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_SaveCustomPeerInformation
(
    deviceId_t        deviceId,
    const uint8_t*    pInfo,
    uint16_t          offset,
    uint16_t          infoSize
);

/*! *********************************************************************************
* \brief  Loads the custom peer information in raw data format.
*
* \param[in] deviceId           Device ID of the GAP peer.
* \param[out] pOutInfo          Pointer to the beginning of the allocated memory.
* \param[in] offset             Offset from the beginning of the reserved memory area.
* \param[in] infoSize           Data size (maximum equal to gcReservedFlashSizeForCustomInformation_d).
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleUnavailable_c            The bond data entry for this device is
*                                       corrupted.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            The nvm index of the deviceId is bigger than
*                                       the number of allowed bonded devices, or the
*                                       nvm saving operation did not have enough memory.
* \remarks This function can be called by the application to load custom information about the
* peer device, e.g., Service Discovery data (to avoid doing it again on reconnection).
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_LoadCustomPeerInformation
(
    deviceId_t  deviceId,
    uint8_t*    pOutInfo,
    uint16_t    offset,
    uint16_t    infoSize
);

/*! *********************************************************************************
* \brief  Loads the custom bonded device information in raw data format.
*         Unlike Gap_LoadCustomPeerInformation, it does not require an active connection.
*
* \param[in] nvmIndex           Index of the device in NVM bonding area.
* \param[out] pOutInfo          Pointer to the beginning of the allocated memory.
* \param[in] offset             Offset from the beginning of the reserved memory area.
* \param[in] infoSize           Data size (maximum equal to gcReservedFlashSizeForCustomInformation_d).
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleUnavailable_c            The bond data entry for this device is
*                                       corrupted.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            The nvm index of the deviceId is bigger than
*                                       the number of allowed bonded devices, or the
*                                       nvm saving operation did not have enough memory.
* \remarks This function can be called by the application to load custom information about the
* bonded device.
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_LoadCustomBondedDeviceInformation
(
    uint8_t     nvmIndex,
    uint8_t*    pOutInfo,
    uint16_t    offset,
    uint16_t    infoSize
);

/*! *********************************************************************************
* \brief  Returns whether or not a connected peer device is bonded and the NVM index.
*
* \param[in] deviceId           Device ID of the GAP peer.
* \param[out] pOutIsBonded      Boolean to be filled with the bonded flag.
* \param[out] pOutNvmIndex      If bonded, to be filled optionally with the NVM index.
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_CheckIfBonded
(
    deviceId_t  deviceId,
    bool_t*     pOutIsBonded,
    uint8_t*    pOutNvmIndex
);

/*! *********************************************************************************
*\fn    bleResult_t Gap_CheckIfConnected(bleAddressType_t addressType,
*                                        const bleDeviceAddress_t aAddress,
*                                        bool_t addrResolved,
*                                        bool_t* pOutIsConnected)
*
*\brief Checks if a connection exists between the local device and another device
*       that has the provided address information.
*       This function executes synchronously.
*
*\param [in]   addressType      Peer address type
*\param [in]   aAddress         Peer address
*\param [in]   addrResolved     Set to TRUE if the address contained in the
*                               addressType and aAddress fields is the identity
*                               address of a resolved RPA.
*\param [out]  pOutIsConnected  TRUE if the device is found
*
* \retval  bleResult_t
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
********************************************************************************** */
bleResult_t Gap_CheckIfConnected
(
    bleAddressType_t addressType,
    const bleDeviceAddress_t aAddress,
    bool_t addrResolved,
    bool_t* pOutIsConnected
);

/*! *********************************************************************************
* \brief  Returns whether or not the given NVM index is free.
*
* \param[in]  nvmIndex          NVM index.
* \param[out] pOutIsFree        TRUE if free, FALSE if occupied.
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_CheckNvmIndex
(
    uint8_t     nvmIndex,
    bool_t*     pOutIsFree
);

/*! *********************************************************************************
* \brief  Retrieves the size of the Filter Accept List.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks Response is received in the gFilterAcceptListSizeRead_c generic event.
*
********************************************************************************** */
bleResult_t Gap_ReadFilterAcceptListSize(void);

/*! *********************************************************************************
* \brief  Removes all addresses from the Filter Accept List, if any.
*
* \retval  gBleSuccess_c
*
* \remarks Confirmation is received in the gFilterAcceptListCleared_c generic event.
*
********************************************************************************** */
bleResult_t Gap_ClearFilterAcceptList(void);

/*! *********************************************************************************
* \brief  Adds a device address to the Filter Accept List.
*
* \param[in] address The address of the filter accept listed device.
* \param[in] addressType The device address type (public or random).
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_AddDeviceToFilterAcceptList
(
    bleAddressType_t          addressType,
    const bleDeviceAddress_t  address
);

/*! *********************************************************************************
* \brief  Removes a device address from the Filter Accept List.
*
* \param[in] address The address of the filter accept listed device.
* \param[in] addressType The device address type (public or random).
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_RemoveDeviceFromFilterAcceptList
(
    bleAddressType_t          addressType,
    const bleDeviceAddress_t  address
);

/*! *********************************************************************************
* \brief  Reads the device's public address from the controller.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks The application should listen for the gPublicAddressRead_c generic event.
*
********************************************************************************** */
bleResult_t Gap_ReadPublicDeviceAddress(void);

/*! *********************************************************************************
* \brief  Requests the controller to create a random address.
*
* \param[in] aIrk           The Identity Resolving Key to be used for a private resolvable address
*                           or NULL for a private non-resolvable address (fully random).
* \param[in] aRandomPart    If aIrk is not NULL, this is a 3-byte array containing the Random Part
*                           of a Private Resolvable Address, in LSB to MSB order; the most significant
*                           two bits of the most significant byte (aRandomPart[3] & 0xC0) are ignored.
*                           This may be NULL, in which case the Random Part is randomly generated internally.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks The application should listen for the gRandomAddressReady_c generic event.
*          Note that this does not set the random address in the Controller. To set the random address,
*          call Gap_SetRandomAddress() with the generated address contained in the event data.
*
********************************************************************************** */
bleResult_t Gap_CreateRandomDeviceAddress
(
    const uint8_t* aIrk,
    const uint8_t* aRandomPart
);

/*! *********************************************************************************
* \brief  Store the name of a bonded device.
*
* \param[in]  deviceId      Device ID for the active peer which name is saved.
* \param[in]  pName         Array of characters holding the name.
* \param[in]  cNameSize     Number of characters to be saved.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            The nvm index of the deviceId is bigger than
*                                       the number of allowed bonded devices, or the
*                                       nvm saving operation did not have enough memory.
*
* \remarks This function copies cNameSize characters from the pName array and
* adds the NULL character to terminate the string.
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_SaveDeviceName
(
    deviceId_t        deviceId,
    const uchar_t*    pName,
    uint8_t           cNameSize
);

/*! *********************************************************************************
* \brief  Retrieves the number of bonded devices.
*
* \param[out] pOutBondedDevicesCount   Pointer to integer to be written.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_GetBondedDevicesCount
(
    uint8_t*    pOutBondedDevicesCount
);

/*! *********************************************************************************
* \brief  Retrieves the name of a bonded device.
*
* \param[in]  nvmIndex      Index of the device in NVM bonding area.
* \param[out] pOutName      Destination array to copy the name into.
* \param[in]  maxNameSize   Maximum number of characters to be copied,
*                           including the terminating NULL character.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleUnavailable_c            The bond data entry for this device is
*                                       corrupted.
*
* \remarks nvmIndex is an integer ranging from 0 to N-1, where N is the number of
* bonded devices and can be obtained by calling Gap_GetBondedDevicesCount(&N).
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_GetBondedDeviceName
(
    uint8_t     nvmIndex,
    uchar_t*    pOutName,
    uint8_t     maxNameSize
);

/*! *********************************************************************************
* \brief  Store the name of a bonded device.
*
* \param[in]  nvmIndex      Index of the device in NVM bonding area.
* \param[in]  pName         Array of characters holding the name.
* \param[in]  cNameSize     Number of characters to be saved.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            The nvm index of the deviceId is bigger than
*                                       the number of allowed bonded devices, or the
*                                       nvm saving operation did not have enough memory.
*
* \remarks This function copies cNameSize characters from the pName array and
* adds the NULL character to terminate the string.
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_SetBondedDeviceName
(
    uint8_t           nvmIndex,
    const uchar_t*    pName,
    uint8_t           cNameSize
);
/*! *********************************************************************************
* \brief  Removes the bond with a device.
*
* \param[in] nvmIndex           Index of the device in the NVM bonding area.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleUnavailable_c            The bond data entry for this device is
*                                       corrupted.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleInvalidState_c           The bond is not active.
*
* \remarks This API requires that there are no active connections at call time.
*          The Gap_CheckIfBonded() API can be called to obtain the nvmIndex
*          of a bonded peer.
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_RemoveBond
(
    uint8_t nvmIndex
);

/*! *********************************************************************************
* \brief  Removes all bonds with other devices.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version
* \retval  gBleInvalidState_c           At least one bond is still active.
**
* \remarks This API requires that there are no active connections at call time.
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_RemoveAllBonds(void);

/*! *********************************************************************************
*\brief Reads the power level of the controller's radio.
*       The response is contained in the gConnEvtTxPowerLevelRead_c connection event when
*       reading connection TX power level, the gAdvTxPowerLevelRead_c generic event when reading
*       the advertising TX power level, or the gConnEvtRssiRead_c connection event when reading the RSSI.
*
*\param [in]    txReadType     Advertising or connection Tx power
*\param [in]    deviceId       Peer identifier (for connections only, otherwise ignored)
*
*\retval  gBleSuccess_c
*\retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
********************************************************************************** */
bleResult_t Gap_ReadRadioPowerLevel
(
    gapRadioPowerLevelReadType_t    txReadType,
    deviceId_t                      deviceId
);

/*! *********************************************************************************
* \brief  Sets the Tx power level on the controller's radio.
*
* \param[in]  powerLevel      Power level as specified in the controller interface.
* \param[in]  channelType     The advertising or connection channel type.
*
* \retval  gBleSuccess_c
*
* \remarks The application should listen for the gTxPowerLevelSetComplete_c generic event.
* \remarks This function executes synchronously.
* \remarks For QN908x platform this command is not supported by the controller.
* \remarks Instead, use RF_SetTxPowerLevel API to set the desired TX power level.
*
********************************************************************************** */
bleResult_t Gap_SetTxPowerLevel
(
    uint8_t                         powerLevel,
    bleTransmitPowerChannelType_t   channelType
);

/*! *********************************************************************************
* \brief  Verifies a Private Resolvable Address with a bonded device's IRK.
*
* \param[in]  nvmIndex      Index of the device in NVM bonding area whose IRK must be checked.
* \param[in]  aAddress      The Private Resolvable Address to be verified.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks nvmIndex is an integer ranging from 0 to N-1, where N is the number of
* bonded devices and can be obtained by calling Gap_GetBondedDevicesCount(&N); the application
* should listen to the gPrivateResolvableAddressVerified_c event.
*
********************************************************************************** */
bleResult_t Gap_VerifyPrivateResolvableAddress
(
    uint8_t                   nvmIndex,
    const bleDeviceAddress_t  aAddress
);

/*! *********************************************************************************
* \brief  Sets a random address into the Controller.
*
* \param[in] aAddress  The Private Resolvable, Private Non-Resolvable, or Static Random Address.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks The application should listen for the gRandomAddressSet_c generic event.
*
********************************************************************************** */
bleResult_t Gap_SetRandomAddress
(
    const bleDeviceAddress_t aAddress
);

/*! *********************************************************************************
* \brief  Reads the device's Local Private Address for a specific peer device from the controller.
*
*\param [in]    pIdAddress pointer to the peer identity address the local private address
*               should be retrieved for.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidState_c           The Controller privacy is not enabled.
*
* \remarks The application should listen for the gControllerLocalRPARead_c generic event.
*
    ********************************************************************************** */
bleResult_t Gap_ReadControllerLocalRPA(const bleIdentityAddress_t *pIdAddress);

/*! *********************************************************************************
* \brief  Sets the default pairing parameters to be used by automatic pairing procedures.
*
* \param[in] pPairingParameters  Pairing parameters as required by the SMP or NULL.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
* \remarks When these parameters are set, the Security Manager automatically responds
* to a Pairing Request or a Peripheral Security Request using these parameters. If NULL is provided,
* it returns to the default state where all security requests are sent to the application.
*
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_SetDefaultPairingParameters
(
    const gapPairingParameters_t* pPairingParameters
);

/*! *********************************************************************************
* \brief        Request a set of new connection parameters
*
* \param[in]    deviceId            The DeviceID for which the command is intended
* \param[in]    intervalMin         The minimum value for the connection event interval
* \param[in]    intervalMax         The maximum value for the connection event interval
* \param[in]    peripheralLatency        The peripheral latency parameter
* \param[in]    timeoutMultiplier   The connection timeout parameter
* \param[in]    minCeLength         The minimum value for the connection event length
* \param[in]    maxCeLength         The maximum value for the connection event length
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
* \pre          A connection must be in place
*
********************************************************************************** */
bleResult_t Gap_UpdateConnectionParameters
(
    deviceId_t  deviceId,
    uint16_t    intervalMin,
    uint16_t    intervalMax,
    uint16_t    peripheralLatency,
    uint16_t    timeoutMultiplier,
    uint16_t    minCeLength,
    uint16_t    maxCeLength
);

/*! *********************************************************************************
* \brief        Update the connection parameters
*
* \param[in]    deviceId            The DeviceID for which the command is intended
* \param[in]    enable              Allow/disallow the parameters update
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       The device id is not valid.
*
* \pre          A connection must be in place
*
* \remarks      The LE central Host may accept the requested parameters or reject the request
*
********************************************************************************** */
bleResult_t Gap_EnableUpdateConnectionParameters
(
    deviceId_t  deviceId,
    bool_t      enable
);

/*! *********************************************************************************
* \brief        Update the Tx Data Length
*
* \param[in]    deviceId            The DeviceID for which the command is intended
* \param[in]    txOctets            Maximum transmission number of payload octets
* \param[in]    txTime              Maximum transmission time
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       The device id is not valid.
*
* \pre          A connection must be in place
*
* \remarks      The response is contained in the gConnEvtLeDataLengthChanged_c connection event.
*
********************************************************************************** */
bleResult_t Gap_UpdateLeDataLength
(
    deviceId_t  deviceId,
    uint16_t    txOctets,
    uint16_t    txTime
);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_SetDefaultConnectionSubrateParameters( const gapConnectionSubrateParameters_t* pConnSubrateParam)
*
*\brief The function sets the initial values of the acceptable parameters
*       for subrating requests, for all future connections where the Controller is Central.
*       User should wait for a gLeSetDefaultConnectionSubrateParametersSetupComplete_c generic event or for a
*       gInternalError_c generic event with source gSetDefaultConnectionSubrateParameters_c .
*
*\param [in]    pConnSubrateParam   Pointer to the new set of connection subrate parameters
*
*\retval        bleResult_t         gBleSuccess_c or error.
***************************************************************************************************/
bleResult_t Gap_SetDefaultConnectionSubrateParameters
(
    const gapConnectionSubrateParameters_t* pConnSubrateParam
);
/*!*************************************************************************************************
*\fn    bleResult_t Gap_ConnectionSubrateRequest(deviceId_t deviceId,
*       const gapConnectionSubrateParameters_t* pConnSubrateParam)
*
*\brief Requests a set of new connection subrate parameters. A connection must be in place.
*       When initiated on a Central this procedure triggers the Link Layer Connection Subrate Update procedure.
*       When initiated on a Peripheral this procedure triggers the Link Layer Connection Subrate Request procedure.
*       User should wait for a gConnEvtLeSubrateChange_c connection event or for a
*       gInternalError_c generic event with source gConnectionSubrateRequest_c.
*
*\param [in]    deviceId            The DeviceID for which the command is intended
*\param [in]    pConnSubrateParam   Pointer to the new set of connection subrate parameters
*
*\retval        bleResult_t         gBleSuccess_c or error.
***************************************************************************************************/
bleResult_t Gap_ConnectionSubrateRequest
(
    deviceId_t  deviceId,
    const gapConnectionSubrateParameters_t* pConnSubrateParam
);

/*! *********************************************************************************
* \brief     Enables or disables Host Privacy (automatic regeneration of a Private Address).
*
* \param[in] enable TRUE to enable, FALSE to disable.
* \param[in] aIrk   Local IRK to be used for Resolvable Private Address generation
*                   or NULL for Non-Resolvable Private Address generation. Ignored if enable is FALSE.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks   The application should listen for the gHostPrivacyStateChanged_c generic event.
*
********************************************************************************** */
bleResult_t Gap_EnableHostPrivacy
(
    bool_t            enable,
    const uint8_t*    aIrk
);

/*! *********************************************************************************
* \brief     Enables or disables Controller Privacy (Enhanced Privacy feature).
*
* \param[in] enable             TRUE to enable, FALSE to disable.
* \param[in] aOwnIrk            Local IRK. Ignored if enable is FALSE, otherwise shall not be NULL.
* \param[in] peerIdCount        Size of aPeerIdentities array. Shall not be zero or greater than
*                               gcGapControllerResolvingListSize_c. Ignored if enable is FALSE.
* \param[in] aPeerIdentities    Array of peer identity addresses and IRKs. Ignored if enable is FALSE,
*                               otherwise shall not be NULL.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleFeatureNotSupported_c    Cannot use Device Privacy mode if controller
*                                       does not support LE Set Privacy Mode Command.
*
* \remarks   The application should listen for the gControllerPrivacyStateChanged_c generic event.
*
********************************************************************************** */
bleResult_t Gap_EnableControllerPrivacy
(
    bool_t                            enable,
    const uint8_t*                    aOwnIrk,
    uint8_t                           peerIdCount,
    const gapIdentityInformation_t*   aPeerIdentities
);

/*! *********************************************************************************
* \brief  Sets the privacy mode to an existing bond.
*
* \param[in] nvmIndex       Index of the device in the NVM bonding area.
* \param[in] privacyMode    Controller privacy mode: Network or Device
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    Cannot use Device Privacy mode if controller
*                                       does not support LE Set Privacy Mode Command.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleUnavailable_c            The bond data entry for this device is
*                                       corrupted.
* \retval  gBleOutOfMemory_c            There is no more room to allocate memory for
*                                       a bonding entry.
* \remarks The change has no effect (other than the change in NVM) unless controller
*          privacy is enabled for the bonded identities.
*
********************************************************************************** */
bleResult_t Gap_SetPrivacyMode
(
    uint8_t             nvmIndex,
    blePrivacyMode_t    privacyMode
);

/*! *********************************************************************************
* \brief  Commands a Controller Test procedure.
*
* \param testCmd            Command type - "start TX test", "start RX test" or "end test".
* \param radioChannel       Radio channel index. Valid range: 0-39.
*                           Frequency will be F[MHz] = 2402 + 2 * index. Effective range: 2402-2480 MHz.
*                           Ignored if command is "end test".
* \param txDataLength       Size of packet payload for TX tests.
*                           Ignored if command is "start RX test" or "end test".
* \param txPayloadType      Type of packet payload for TX tests.
*                           Ignored if command is "start RX test" or "end test".
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
* \remarks The application should listen for the gControllerTestEvent_c generic event.
* \remarks This API function is available only in the full-featured host library.
*
********************************************************************************** */
bleResult_t Gap_ControllerTest
(
    gapControllerTestCmd_t      testCmd,
    uint8_t                     radioChannel,
    uint8_t                     txDataLength,
    gapControllerTestTxType_t   txPayloadType
);

/*! *********************************************************************************
* \brief  Requests the Controller to change its sleep clock accuracy for testing purposes.
*
* \param  action          Specifies whether the sleep clock should be changed to
*                         one that is more accurate or one that is less accurate.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
* \remarks The application should listen for the generic event.
*
********************************************************************************** */
bleResult_t Gap_ModifySleepClockAccuracy
(
    gapSleepClockAccuracy_t action
);

/*! *********************************************************************************
* \brief     Read the Tx and Rx Phy on the connection with a device
*
* \param[in] deviceId  Device ID of the peer.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks   The application should listen for the gLePhyEvent_c generic event.
*            This API is available only in the Bluetooth 5.0 Host Stack.
*
********************************************************************************** */
bleResult_t Gap_LeReadPhy
(
    deviceId_t  deviceId
);

/*! *********************************************************************************
* \brief     Set the Tx and Rx Phy preferences on the connection with a device or all subsequent connections
*
* \param[in] defaultMode   Use the provided values for all subsequent connections
* \param[in] deviceId      Device ID of the peer
*                          Ignored if defaultMode is TRUE.
* \param[in] allPhys       Host preferences on Tx and Rx Phy, as defined by gapLeAllPhyFlags_t
* \param[in] txPhys        Host preferences on Tx Phy, as defined by gapLePhyFlags_t, ignored for gLeTxPhyNoPreference_c
* \param[in] rxPhys        Host preferences on Rx Phy, as defined by gapLePhyFlags_t, ignored for gLeRxPhyNoPreference_c
* \param[in] phyOptions    Host preferences on Coded Phy, as defined by gapLePhyOptionsFlags_t
*                          Ignored if defaultMode is TRUE.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by
*                                       this stack version
*
* \remarks   The application should listen for the gLePhyEvent_c generic event.
*            This API is available only in the Bluetooth 5.0 Host Stack.
*
********************************************************************************** */
bleResult_t Gap_LeSetPhy
(
    bool_t      defaultMode,
    deviceId_t  deviceId,
    uint8_t     allPhys,
    uint8_t     txPhys,
    uint8_t     rxPhys,
    uint16_t    phyOptions
);

/*! *********************************************************************************
* \brief  Configures enhanced notifications on advertising, scanning and connection events
*         on the controller.
*
* \param[in]  eventType       Event type selection as specified by bleNotificationEventType_t.
* \param[in]  deviceId        Device ID of the peer, used only for connection events.
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            The nvm index of the deviceId is bigger than
*                                       the number of allowed bonded devices, or the
*                                       nvm saving operation did not have enough memory.
*
* \remarks The application should listen for the gControllerNotificationEvent_c generic event.
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_ControllerEnhancedNotification
(
    uint16_t    eventType,
    deviceId_t  deviceId
);

/*!*************************************************************************************************
* \brief This function configures the advertising index type.
*
* \param [in]    advIndexType           Advertising index type
* \param [in]    aUserDefinedChannels   User defined channels array
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
***************************************************************************************************/
bleResult_t Gap_BleAdvIndexChange
(
    bleAdvIndexType_t     advIndexType,
    uint8_t               aUserDefinedChannels[3]
);

/*! *********************************************************************************
* \brief  Retrieves the keys from an existing bond with a device.
*
* \param[in]  nvmIndex           Index of the device in the NVM bonding area.
* \param[out] pOutKeys           Pointer to fill the keys distributed during pairing.
* \param[out] pOutKeyFlags       Pointer to indicate which keys were distributed during pairing.
* \param[out] pOutLeSc           Pointer to mark if LE Secure Connections was used during pairing.
* \param[out] pOutAuth           Pointer to mark if the device was authenticated for MITM during pairing.
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
* \remarks This API requires that the aAddress in the pOutKeys shall not be NULL.
* \remarks The application will check pOutKeyFlags to see which information is valid in pOutKeys.
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_LoadKeys
(
    uint8_t           nvmIndex,
    gapSmpKeys_t*     pOutKeys,
    gapSmpKeyFlags_t* pOutKeyFlags,
    bool_t*           pOutLeSc,
    bool_t*           pOutAuth
);

/*! *********************************************************************************
* \brief  Saves the keys to a new or existing bond based on OOB information.
*
* \param[in] nvmIndex        Index of the device in the NVM bonding area.
* \param[in] pKeys           Pointer to the keys distributed during pairing.
* \param[in] leSc            Indicates if LE Secure Connections was used during pairing.
* \param[in] auth            Indicates if the device was authenticated for MITM during pairing.
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \remarks This API requires that the aAddress in the pKeys shall not be NULL.
* \remarks If any of the keys are passed as NULL, they will not be saved.
* \remarks The application listen for gBondCreatedEvent_c to confirm the bond was created.
*
********************************************************************************** */
bleResult_t Gap_SaveKeys
(
    uint8_t             nvmIndex,
    const gapSmpKeys_t  *pKeys,
    bool_t              leSc,
    bool_t              auth
);

/*! *********************************************************************************
* \brief  Set the channel map in the Controller and trigger an LL channel map update.
*         Specifies the channel map for data, secondary advertising and periodic physical channels.
*
* \param[in]  channelMap      Array with the channels using 0 for bad channels and 1 for unknown.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
*
* \remarks The application should listen for the gChannelMapSet_c generic event.
* \remarks This function executes synchronously.
*
* \remarks API supported on GAP Central devices or Broadcaster with Extended Advertising support.
*
********************************************************************************** */
bleResult_t Gap_SetChannelMap
(
    const bleChannelMap_t channelMap
);

/*! *********************************************************************************
* \brief  Reads the channel map of a connection.
*
* \param[in]  deviceId        Device ID of the peer.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
* \remarks The application should listen for the gConnEvtChannelMapRead_c connection event.
* \remarks This function executes synchronously.
*
********************************************************************************** */
bleResult_t Gap_ReadChannelMap
(
    deviceId_t  deviceId
);

/*! *********************************************************************************
* \brief  Sets up the Extended Advertising Parameters.
*
* \param[in] pAdvertisingParameters   Pointer to gapExtAdvertisingParameters_t structure.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gGapAnotherProcedureInProgress_c Another set extended advertisment parameters
*                                           command is in progress.
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_SetExtAdvertisingParameters
(
    gapExtAdvertisingParameters_t*   pAdvertisingParameters
);

/*! *********************************************************************************
* \brief  Sets up the Extended Advertising and Extended Scan Response Data.
*
* \param[in] handle             The ID of the advertising set
* \param[in] pAdvertisingData   Pointer to gapAdvertisingData_t structure or NULL.
* \param[in] pScanResponseData  Pointer to gapScanResponseData_t structure or NULL.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            The nvm index of the deviceId is bigger than
*                                       the number of allowed bonded devices, or the
*                                       nvm saving operation did not have enough memory.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleInvalidState_c           The advertising set was not configured
*                                       previously using Gap_SetExtAdvertisingParameters.
* \retval  gGapAdvDataTooLong_c         The advertising  data is too long.
*
* \remarks Any of the parameters may be NULL, in which case they are ignored.
* Therefore, this function can be used to set any of the parameters individually or both at once.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_SetExtAdvertisingData
(
    uint8_t handle,
    gapAdvertisingData_t*  pAdvertisingData,
    gapScanResponseData_t* pScanResponseData
);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_SetExtAdvertisingDecisionData(uint8_t handle,
*       const gapAdvertisingDecisionData_t *pAdvertisingDecisionData)
*
* \brief  Sets up the Extended Advertising Decision Data.
*
* \param[in] handle             The ID of the advertising set
* \param[in] pAdvertisingDecisionData   Pointer to gapAdvertisingDecisionData_t structure.
*
* \retval  gBleFeatureNotSupported_c in case gLeExtendedAdv_c or gLeDecisionBasedAdvertisingFiltering_c are not supported.
* \retval  gBleInvalidParameter_c in case at least 1 parameter is incorrect.
* \retval  gBleOutOfMemory_c in case the memory allocation for the app to host message fails.
* \retval  gBleSuccess_c otherwise.
*
* \remarks pDecisionData or pKey in the gapAdvertisingDecisionData_t structure may be NULL but not both.
*
* \remarks GAP Peripheral-only API function.
*
* \remarks Application should wait for gExtAdvertisingDecisionDataSetupComplete_c or
*          generic event or for gInternalError_c generic event with errorSource = gSetExtAdvDecisionData_c.
*
********************************************************************************** */
bleResult_t Gap_SetExtAdvertisingDecisionData
(
    uint8_t handle,
    const gapAdvertisingDecisionData_t *pAdvertisingDecisionData
);

/*! *********************************************************************************
* \brief  Commands the controller to start the extended advertising.
*
* \param[in] advertisingCallback   Callback used by the application to receive advertising events.
*                                  Can be NULL.
* \param[in] connectionCallback    Callback used by the application to receive connection events.
*                                  Can be NULL.
*
* \param[in] handle                The ID of the advertising set
* \param[in] duration              The duration of the advertising
* \param[in] maxExtAdvEvents       The maximum number of advertising events
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidState_c           The advertising is already started or pending
*                                       to be started.
*
* \remarks The advertisingCallback confirms or denies whether the advertising has started.
* The connectionCallback is only used if a connection gets established during advertising.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_StartExtAdvertising
(
    gapAdvertisingCallback_t    advertisingCallback,
    gapConnectionCallback_t     connectionCallback,
    uint8_t                     handle,
    uint16_t                    duration,
    uint8_t                     maxExtAdvEvents
);

/*! *********************************************************************************
* \brief  Commands the controller to stop extended advertising for set ID.
*
* \param[in] handle                   The ID of the advertising set
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleInvalidState_c           The advertising set is not started.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_StopExtAdvertising
(
    uint8_t handle
);

/*! *********************************************************************************
* \brief  Commands the controller to remove the specified advertising set
*         and all its data.
*
* \param[in] handle   The ID of the advertising set
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_RemoveAdvSet
(
    uint8_t handle
);

/*! *********************************************************************************
* \brief  Sets up the Periodic Advertising Parameters.
*
* \param[in] pAdvertisingParameters   Pointer to gapPeriodicAdvParameters_t structure.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_SetPeriodicAdvParameters
(
    gapPeriodicAdvParameters_t*   pAdvertisingParameters
);

/*! *********************************************************************************
* \brief  Sets up the Periodic Advertising Subevent Data.
*
* \param[in] advHandle                             Advertising handle.
* \param[in] gapPeriodicAdvertisingSubeventData_t  Pointer to gapPeriodicAdvertisingSubeventData_t structure.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_SetPeriodicAdvSubeventData
(
    uint8_t                                       advHandle,
    const gapPeriodicAdvertisingSubeventData_t*   pData
);

/*! *********************************************************************************
* \brief  Sets up the Periodic Advertising Response Data.
*
* \param[in] syncHandle                            Sync handle.
* \param[in] gapPeriodicAdvertisingResponseData_t  Pointer to gapPeriodicAdvertisingResponseData_t structure.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
*
********************************************************************************** */
bleResult_t Gap_SetPeriodicAdvResponseData
(
    uint16_t                                      syncHandle,
    const gapPeriodicAdvertisingResponseData_t*   pData
);

/*! *********************************************************************************
* \brief  Instructs the controller to sync with a subset of the subevents within a PAWR train identified by syncHandle.
*
* \param[in] syncHandle                           Sync handle.
* \param[in] gapPeriodicSyncSubeventParameters_t  Pointer to gapPeriodicSyncSubeventParameters_t structure.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
*
********************************************************************************** */
bleResult_t Gap_SetPeriodicSyncSubevent
(
    uint16_t                                     syncHandle,
    const gapPeriodicSyncSubeventParameters_t*   pParams
);

/*! *********************************************************************************
* \brief  Sets up the Periodic Advertising Data.
*
* \param[in] handle             The ID of the periodic advertising set
* \param[in] pAdvertisingData   Pointer to gapAdvertisingData_t structure.
* \param [in] bUpdateDid        If TRUE, use operation mode gHciExtDataUnchanged_c.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleInvalidState_c           The advertising set was not previously
*                                       configured using Gap_SetPeriodicAdvParameters.
* \retval  gGapAdvDataTooLong_c         The advertising data is too long.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_SetPeriodicAdvertisingData
(
    uint8_t handle,
    gapAdvertisingData_t*  pAdvertisingData,
    bool_t bUpdateDID
);

/*! *********************************************************************************
* \brief  Commands the controller to start periodic advertising for set ID.
*
* \param[in] handle                   The ID of the periodic advertising set
* \param [in] bIncludeADI     If TRUE, include ADI field in AUX_SYNC_IND PDUs
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleInvalidState_c           The advertising set was not previously
*                                       configured using Gap_SetPeriodicAdvParameters.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_StartPeriodicAdvertising
(
    uint8_t handle,
    bool_t  bIncludeADI
);

/*! *********************************************************************************
* \brief  Commands the controller to stop periodic advertising for set ID.
*
* \param[in] handle                   The ID of the periodic advertising set
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleInvalidState_c           The advertising set was not previously
*                                       configured using Gap_SetPeriodicAdvParameters.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks GAP Peripheral-only API function.
*
********************************************************************************** */
bleResult_t Gap_StopPeriodicAdvertising
(
    uint8_t handle
);

/*! *********************************************************************************
* \brief  Manage the periodic advertising list.
*
* \param[in] operation  The list operation: add/remove a device, or clear all.
* \param[in] addrType   The address type of the periodic advertiser.
* \param[in] pAddr      Pointer to the advertiser's address.
* \param[in] SID        The ID of the advertising set.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gGapAnotherProcedureInProgress_c Periodic advertising create sync is in
*                                           progress.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks GAP Central-only API function.
*
********************************************************************************** */
bleResult_t Gap_UpdatePeriodicAdvList
(
    gapPeriodicAdvListOperation_t operation,
    bleAddressType_t addrType,
    uint8_t * pAddr,
    uint8_t SID
);

/*! *********************************************************************************
* \brief  Start tracking periodic advertisements.
* Scanning is required to be ON for this request to be processed,
* so the scanning callback will receive the periodic advertising events.
* \param[in] pReq              Pointer to the Sync Request parameters.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gGapAnotherProcedureInProgress_c Periodic advertising create sync is in
*                                           progress.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \remarks GAP Central-only API function.
*
********************************************************************************** */
bleResult_t Gap_PeriodicAdvCreateSync
(
    gapPeriodicAdvSyncReq_t * pReq
);

/*! *********************************************************************************
* \brief  Stop tracking periodic advertisements.
*
* \param[in] syncHandle        Used to identify the periodic advertiser
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gGapAnotherProcedureInProgress_c Periodic advertising create sync is in
*                                           progress.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \remarks GAP Central-only API function.
*
********************************************************************************** */
bleResult_t Gap_PeriodicAdvTerminateSync
(
    uint16_t syncHandle
);

/*! *********************************************************************************
* \brief  Enable Periodic Advertisement Sync Transfer Controller feature.
*
* \param[in] syncHandle        Used to identify the periodic advertiser
* \param[in] enableDuplicateFiltering   Used to enable or disable duplicate filtering
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_PeriodicAdvReceiveEnable(uint16_t syncHandle, bool_t enableDuplicateFiltering);

/*! *********************************************************************************
* \brief  Disable Periodic Advertisement Sync Transfer Controller feature.
*
* \param[in] syncHandle        Used to identify the periodic advertiser
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_PeriodicAdvReceiveDisable(uint16_t syncHandle);

/*! *********************************************************************************
* \brief  Instruct the Controller to send synchronization information about the
*         periodic advertising train identified by the sync handle to a connected device.
*
* \param[in] pParam             Pointer to the command arguments
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_PeriodicAdvSyncTransfer(gapPeriodicAdvSyncTransfer_t *pParam);

/*! *********************************************************************************
* \brief  Instruct the Controller to send synchronization information about the
*         periodic advertising to a connected device.
*
* \param[in] pParam             Pointer to the command arguments
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_PeriodicAdvSetInfoTransfer(gapPeriodicAdvSetInfoTransfer_t *pParam);

/*! *********************************************************************************
* \brief  Specify how the Controller will process periodic advertising
*         synchronization information received from the device identified by the
*         device id.
*
* \param[in] pParam             Pointer to the command arguments
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_SetPeriodicAdvSyncTransferParams(gapSetPeriodicAdvSyncTransferParams_t *pParam);

/*! *********************************************************************************
* \brief  Specify the initial value for the mode, skip, timeout, and Constant Tone
*         Extension type (set by the Gap_SetPeriodicAdvSyncTransferParams command) to
*         be used for all subsequent connections over the LE transport.
*
* \param[in] pParam             Pointer to the command arguments
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_SetDefaultPeriodicAdvSyncTransferParams(gapSetPeriodicAdvSyncTransferParams_t *pParam);

/*! *********************************************************************************
* \brief Resume the pairing process. At this point the ECDH key must be computed.
*        This function should be called only for secured LE connections. In any other
*        cases the user should make his own code for handling the case when the ECDH
*        computation is completed.
*
* \param[in] pData Pointer to the data used to resume the host state machine. The
*                  data is allocated by the stack when it requests an ECDH
*                  multiplication. It is also freed by the stack at the end of the
*                  multiplication.
*
* \retval  gBleSuccess_c
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
********************************************************************************** */
bleResult_t Gap_ResumeLeScStateMachine
(
    computeDhKeyParam_t *pData
);

/*! *********************************************************************************
* \brief  Set Connectionless CTE Transmit Parameters for an advertising set.
*
* \param[in] pTransmitParams    Pointer to struct containing parameters.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_SetConnectionlessCteTransmitParameters
(
    gapConnectionlessCteTransmitParams_t *pTransmitParams
);

/*! *********************************************************************************
* \brief Enable or disable Connectionless CTE Transmit for an advertising set.
*
* \param[in] handle    Advertising set handle.
* \param[in] enable    Enable or disable CTE Transmit.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_EnableConnectionlessCteTransmit
(
    uint8_t                 handle,
    bleCteTransmitEnable_t  enable
);

/*! *********************************************************************************
* \brief  Enable or disable Connectionless IQ sampling for an advertising train.
*
* \param[in] syncHandle         Used to identify advertising train.
* \param[in] pIqSamplingParams  Pointer to struct containing parameters.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_EnableConnectionlessIqSampling
(
    uint16_t                             syncHandle,
    gapConnectionlessIqSamplingParams_t  *pIqSamplingParams
);

/*! *********************************************************************************
* \brief  Set CTE Receive Parameters for a certain connection.
*
* \param[in] deviceId        Peer device ID.
* \param[in] pReceiveParams  Pointer to struct containing parameters.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_SetConnectionCteReceiveParameters
(
    deviceId_t                      deviceId,
    gapConnectionCteReceiveParams_t *pReceiveParams
);

/*! *********************************************************************************
* \brief  Set CTE Transmit Parameters for a certain connection.
*
* \param[in] deviceId        Peer device ID.
* \param[in] pTransmitParams  Pointer to struct containing parameters.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_SetConnectionCteTransmitParameters
(
    deviceId_t                        deviceId,
    gapConnectionCteTransmitParams_t *pTransmitParams
);

/*! *********************************************************************************
* \brief  Enable or disable CTE Request procedure for a certain connection.
*
* \param[in] deviceId              Peer device ID.
* \param[in] pCteReqEnableParams  Pointer to struct containing parameters.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_EnableConnectionCteRequest
(
    deviceId_t                        deviceId,
    gapConnectionCteReqEnableParams_t *pCteReqEnableParams
);

/*! *********************************************************************************
* \brief  Enable or disable sending CTE Responses for a certain connection.
*
* \param[in] deviceId   Peer device ID.
* \param[in] enable     Enable or disable sending CTE Responses.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_EnableConnectionCteResponse
(
    deviceId_t        deviceId,
    bleCteRspEnable_t enable
);

/*! *********************************************************************************
* \brief Read Antenna Information.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks Antenna information contained in gAntennaInformationRead_c generic event.
********************************************************************************** */
bleResult_t Gap_ReadAntennaInformation(void);

/*! *********************************************************************************
* \brief  Read local current and maximum tx power levels for a certain connection
*         and PHY.
*
* \param[in] deviceId   Peer device ID.
* \param[in] phy        PHY.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_EnhancedReadTransmitPowerLevel
(
    deviceId_t                  deviceId,
    blePowerControlPhyType_t    phy
);

/*! *********************************************************************************
* \brief  Read remote tx power for a certain connection and PHY.
*
* \param[in] deviceId   Peer device ID.
* \param[in] phy        PHY.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_ReadRemoteTransmitPowerLevel
(
    deviceId_t                  deviceId,
    blePowerControlPhyType_t    phy
);

/*! *********************************************************************************
* \brief Set path loss threshold reporting parameters for a certain connection.
*
* \param[in] deviceId                     Peer device ID.
* \param[in] pPathLossReportingParams     Pointer to struct containing parameters.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_SetPathLossReportingParameters
(
    deviceId_t                      deviceId,
    gapPathLossReportingParams_t    *pPathLossReportingParams
);

/*! *********************************************************************************
* \brief  Enable or disable path loss threshold reporting for a certain connection.
*
* \param[in] deviceId   Peer device ID.
* \param[in] enable     Enable or disable path loss threshold reporting.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_EnablePathLossReporting
(
    deviceId_t                   deviceId,
    blePathLossReportingEnable_t enable
);

/*! *********************************************************************************
* \brief  Enable or disable tx power reporting for a certain connection.
*
* \param[in] deviceId         Peer device ID.
* \param[in] localEnable      Enable or disable local tx power reports.
* \param[in] removeEnable     Enable or disable remote tx power reports.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_EnableTransmitPowerReporting
(
    deviceId_t                  deviceId,
    bleTxPowerReportingEnable_t localEnable,
    bleTxPowerReportingEnable_t remoteEnable
);

/*! *********************************************************************************
* \brief Initiates generation of a Diffie-Hellman key in the Controller for use
*           over the LE transport. Version 2 of LE Generate DHKey command.
*
* \param[in] pRemoteP256PublicKey   Pointer to the remote P-256 public key used as input for DH key generation.
* \param[in] keyType                The private key used of DH key generation.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
* \remarks Generated key is contained in gHciLeGenerateDhKeyCompleteEvent_c event.
********************************************************************************** */
bleResult_t Gap_GenerateDhKeyV2
(
    ecdhPublicKey_t*    pRemoteP256PublicKey,
    gapPrivateKeyType_t keyType
);

/*! *********************************************************************************
* \brief     Open up to 5 Enhanced ATT bearers.
*
* \param[in] deviceId              Peer device id
* \param[in] mtu                   MTU
* \param[in] cBearers              Number of bearers
* \param[in] initialCredits        Initial credits
* \param[in] autoCreditsMgmt       EATT will automatically send credits
*                                  in chunks of initialCredits if TRUE.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_EattConnectionRequest
(
    deviceId_t  deviceId,
    uint16_t    mtu,
    uint8_t     cBearers,
    uint16_t    initialCredits,
    bool_t      autoCreditsMgmt
);

/*! *********************************************************************************
* \brief     Accept or reject a received EATT Connection Request
*
* \param[in] deviceId             Peer device id
* \param[in] accept               TRUE - accept received EattConnectionRequest
*                                 FALSE - reject incoming EattConnectionRequest
* \param[in] localMtu             Local MTU
* \param[in] initialCredits,      Initial credits
* \param[in] autoCreditsMgmt      EATT will automatically send credits
*                                 in chunks of initialCredits if TRUE.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_EattConnectionAccept
(
    deviceId_t  deviceId,
    bool_t      accept,
    uint16_t    localMtu,
    uint16_t    initialCredits,
    bool_t      autoCreditsMgmt
);

/*! *********************************************************************************
* \brief     Reconfigure the MTU of up to 5 Enhanced ATT bearers.
*
* \param[in] deviceId      Peer device id
* \param[in] mtu           New MTU value to be configured.
* \param[in] mps           New MPS value to be configured. Set to 0 to use
*                          current maximum mps value of the channels being
*                          reconfigured
* \param[in] cBearers      Number of bearers
* \param[in] *pBearers     Initial credits
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_EattReconfigureRequest
(
    deviceId_t  deviceId,
    uint16_t    mtu,
    uint16_t    mps,
    uint8_t     cBearers,
    bearerId_t  *pBearers
);

/*! *********************************************************************************
* \brief     Send L2cap credits for Enhanced ATT bearers.
*
* \param[in] deviceId      Peer device id
* \param[in] bearerId      Enhanced ATT bearer id.
* \param[in] credits       Number of credits.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_EattSendCredits
(
    deviceId_t  deviceId,
    bearerId_t  bearerId,
    uint16_t    credits
);

/*! *********************************************************************************
* \brief     Disconnect the specified bearer.
*
* \param[in] deviceId      Peer device id
* \param[in] bearerId      Enhanced ATT bearer id to be disconnected.
*
* \retval  gBleSuccess_c
* \retval  gBleFeatureNotSupported_c    The requested feature is not supported by this
*                                       stack version.
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
********************************************************************************** */
bleResult_t Gap_EattDisconnect
(
    deviceId_t  deviceId,
    bearerId_t  bearerId
);

/*!*************************************************************************************************
*\brief        Returns the deviceId associated with the received connection handle.
*
*\param [in]   connHandle              Connection identifier
*\param [out]  pDeviceId               Corresponding device id
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
***************************************************************************************************/
bleResult_t Gap_GetDeviceIdFromConnHandle
(
    uint16_t    connHandle,
    deviceId_t* pDeviceId
);

/*!*************************************************************************************************
*\brief        Returns the deviceId associated with the received connection handle.
*
*\param [in]   deviceId         Connection identifier
*\param [out]  pConnHandle       Corresponding connection handle
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
*
***************************************************************************************************/
bleResult_t Gap_GetConnectionHandleFromDeviceId
(
    deviceId_t deviceId,
    uint16_t*  pConnHandle
);

/*!*************************************************************************************************
 * \fn           bleResult_t Gap_GetHostVersion(gapHostVersion_t *pOutHostVersion)
 *
 * \brief        Retrieves Host Version information.
 *
 * \param [out]   pOutHostVersion   Pointer to the memory location where the Host Version
                                    information should be stored.
 *
 * \retval  gBleSuccess_c
 * \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
 *                                       outside the accepted range.
 *
***************************************************************************************************/
bleResult_t Gap_GetHostVersion
(
    gapHostVersion_t *pOutHostVersion
);

/*!*************************************************************************************************
 * \fn           bleResult_t Gap_ReadRemoteVersionInformation(deviceId_t deviceId)
 *
 * \brief        Reads the version information of a peer device.
 *
 * \param [in]   deviceId       Peer device identifier.
 *
 * \retval       gBleSuccess_c
 * \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
 *
***************************************************************************************************/
bleResult_t Gap_ReadRemoteVersionInformation
(
    deviceId_t  deviceId
);

/*!*************************************************************************************************
* \def          Gap_GetConnParams(deviceId)
*
* \brief        Get the Connection parameters for the given deviceId.
*
* \param [in]   deviceId   Peer device Id.
*
* \retval  gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
***************************************************************************************************/
#define Gap_GetConnParams(deviceId) \
    Gap_GetConnParamsMonitoring(deviceId, 0U)

/*!*************************************************************************************************
* \fn           bleResult_t Gap_GetConnParamsMonitoring(deviceId_t deviceId, uint8_t mode)
*
* \brief        Get the Connection parameters for the given deviceId in the given mode.
*
* \param [in]   deviceId   Peer device Id.
* \param [in]   mode       Connection parameters event report mode:
*                               bit 0: update the local sequence number for central / peripheral differentiation
*                                       and send the LE_Handover_Connection_Parameters_Update_Event 
*                                       after remote sequence number change.
*                               bit 1: Send LE_Handover_Connection_Parameters_Update_Event when the connection 
*                                       parameters are updated following an LL procedure
*
* \retval       gBleSuccess_c
* \retval  gBleInvalidParameter_c       A parameter has an invalid value or is
*                                       outside the accepted range.
* \retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*
***************************************************************************************************/
bleResult_t Gap_GetConnParamsMonitoring
(
    deviceId_t deviceId,
    uint8_t    mode
);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_LeChannelOverride(bleChannelOverrideMode_t mode, uint8_t channelListLength, uint8_t *pChannelList)
*
*\brief This function sets the channels to be used for advertising, scanning or initiation.
*
*\param [in]    mode               Override the advertising, scanning or initiation channels.
*\param [in]    channelListLength  Length of channel list, must be between 1 and 6.
*\param [in]    pChannelList       List of channels (see remarks for more details).
*
*\retval  gBleSuccess_c
*\retval  gBleInvalidParameter_c       One or more parameters are invalid.
*\retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
*\remarks In the channel list, the MSB of each byte indicates if the channel is BLE channel index or generic channel
* index:
* - BLE channel index: 0 to 39
* - Generic channel index: 0x80+0 to 0x80+127
*
* If the mode is 0x00 (advertising), the specified channels are used by the following
* advertising commands. The channels used by the ongoing advertising will not change until
* the advertising is disabled, even if a new channel override command with mode 0x00 is
* received. This allows different advertising channels for different advertising sets.
* The mode 0x00 (advertising) can be issued when an advertising is ongoing.
* The setting will be used by the next advertising enable command.
*
* If the mode is 0x01 or 0x02 (scan or initiation), the specified channels are used by the
* following scan and initiation. The command with mode 0x01 or 0x02 should be issued when
* the scan or initiation is not ongoing.
*
* When a generic channel is used, the whitening is initialized by the lower 6 bits
* of the generic channel index. When a BLE channel is used, the whitening is initialized
* by the BLE channel index. So the BLE channel 37/38/39 are not strictly the same as the
* generic channels 42/66/120: they operate on the same frequency but their channel whitening
* is different.
***************************************************************************************************/
bleResult_t Gap_LeChannelOverride
(
    bleChannelOverrideMode_t mode,
    uint8_t channelListLength,
    uint8_t *pChannelList
);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_LeSetSchedulerPriority(uint16_t priorityHandle)
*
*\brief This function sets the priority for one connection in case of several connections by calling
*       the corresponding HCI command.
*
*\param [in]    priorityHandle         Parameter that sets the priority for connections
*
*\retval  gBleSuccess_c
*\retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
***************************************************************************************************/
bleResult_t Gap_LeSetSchedulerPriority
(
    uint16_t  priorityHandle
);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_LePeriodicAdvUpdateSync(uint16_t syncHandle, uint16_t skip, uint16_t syncTimeout)
*
*\brief This function updates the synchronization parameters for periodic advertising.
*
*\param [in]    syncHandle         Handle identifying the periodic advertising sync.
*\param [in]    skip               Number of periodic advertising packets to skip.
*\param [in]    syncTimeout        Synchronization timeout value.
*
*\retval  gBleSuccess_c
*\retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
***************************************************************************************************/
bleResult_t Gap_LePeriodicAdvUpdateSync(
    uint16_t syncHandle,
    uint16_t skip,
    uint16_t syncTimeout
);

/*!*************************************************************************************************
*\fn    void Gap_LeSetHostFeature(uint8_t bitNumber, bool_t enable)
*
*\brief This function is used by the Host to set or clear a bit controlled by the Host in the
*       Link Layer FeatureSet.
*
*\param [in]    bitNumber         Bit position in the FeatureSet
*\param [in]    enable            Host feature is enabled/disabled
*
*\retval  gBleSuccess_c
*\retval  gBleOutOfMemory_c            Cannot allocate memory for the Host task.
***************************************************************************************************/
bleResult_t Gap_LeSetHostFeature
(
    uint8_t bitNumber,
    bool_t  enable
);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_EncryptAdvertisingData(const gapAdvertisingData_t *pAdvertisingData, const uint8_t *pKey,
*                                              const uint8_t *pIV, uint8_t *pOutput)
*
*\brief This function is used to encrypt a series of AD structures into an encrypted AD structure.
*
*\param [in]    pAdvertisingData   Pointer to AD(s) to be encrypted.
*\param [in]    pKey               Pointer to 16-byte key to be used for encryption (must be big endian).
*\param [in]    pIV                Pointer to 8-byte initialization vector (must be little endian).
*\param [out]   pOutput            Pointer for location of output encrypted data. Must be large enough
*                                  to accommodate the size of the total data to be encrypted
*                                  plus the size of the Randomizer and MIC fields.
*
*\retval  gBleSuccess_c
*\retval  gBleInvalidParameter_c   At least one of the provided parameters is invalid.
*\retval  gBleOutOfMemory_c        Could not allocate memory for the buffers used in CCM.
*\retval  gBleRngError_c           Could not generate the randomizer.
*\retval  gBleSecLibError_c        CCM call failed.
***************************************************************************************************/
bleResult_t Gap_EncryptAdvertisingData
(
    const gapAdvertisingData_t *pAdvertisingData,
    const uint8_t              *pKey,
    const uint8_t              *pIV,
    uint8_t                    *pOutput
);

/*!*************************************************************************************************
*\fn    bleResult_t Gap_DecryptAdvertisingData(uint8_t *pData, uint16_t dataLength, const uint8_t *pKey,
*                                              const uint8_t *pIV, uint8_t *pOutput)
*
*\brief This function is used to decrypt advertising data.
*
*\param [in]    pData       Pointer to encrypted data (AD payload).
*\param [in]    dataLength  Length of the encrypted data (includes randomizer and MIC fields).
*\param [in]    pKey        Pointer to 16-byte key to be used for decryption (must be big endian).
*\param [in]    pIV         Pointer to 8-byte initialization vector (must be little endian).
*\param [out]   pOutput     Pointer to the location of decrypted data.
*
*\retval  gBleSuccess_c
*\retval  gBleInvalidParameter_c   At least one of the provided parameters is invalid.
*\retval  gBleOutOfMemory_c        Could not allocate memory for the buffers used in CCM.
*\retval  gBleSecLibError_c        CCM call failed.
***************************************************************************************************/
bleResult_t Gap_DecryptAdvertisingData
(
    uint8_t        *pData,
    uint16_t       dataLength,
    const uint8_t  *pKey,
    const uint8_t  *pIV,
    uint8_t        *pOutput
);

/*!*************************************************************************************************
*\fn           bleResult_t Gap_SetConnectionCallback(gapConnectionCallback_t connectionCallback)
*
*\brief        Set the GAP connection callback.
*
*\param [in]   pfConnectionCallback     The function called when one of the events defined in
*                                       gapConnectionEventType_t occurs.
*
* \return  none
***************************************************************************************************/
void Gap_SetConnectionCallback
(
    gapConnectionCallback_t pfConnectionCallback
);

/*!*************************************************************************************************
*\fn           bleResult_t Gap_SetScanningCallback(gapScanningCallback_t pfScanningCallback)
*
*\brief        Set the GAP scanning callback.
*
*\param [in]   pfScanningCallback     The function called when one of the events defined in
*                                     gapScanningEventType_t occurs.
*
* \return  none
***************************************************************************************************/
void Gap_SetScanningCallback
(
    gapScanningCallback_t pfScanningCallback
);
#ifdef __cplusplus
}
#endif

#endif /* GAP_INTERFACE_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
