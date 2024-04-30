/*! *********************************************************************************
* \addtogroup HOST_BBOX_UTILITY
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright(c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2019, 2022-2023 NXP
*
*
* \file
*
* This file is the source file for the Host - Blackbox GAP management
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble_gap.h"
#include "host_ble.h"

#if gFsciIncluded_c
#include "FsciCommunication.h"
#endif

/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

bleResult_t Ble_HostInitialize
(
    gapGenericCallback_t            genericCallback,
    hciHostToControllerInterface_t  hostToControllerInterface
)
{
    FsciCmdMonitor(HostInitialize, genericCallback, hostToControllerInterface);

    return gBleSuccess_c;
}


bleResult_t Gap_RegisterDeviceSecurityRequirements
(
    gapDeviceSecurityRequirements_t* pSecurity
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(RegisterDeviceSecurityRequirements, pSecurity);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_SetAdvertisingParameters
(
    gapAdvertisingParameters_t* pAdvertisingParameters
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SetAdvertisingParameters, pAdvertisingParameters);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


#if gBLE50_d
bleResult_t Gap_SetExtAdvertisingParameters
(
    gapExtAdvertisingParameters_t* pAdvertisingParameters
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SetExtAdvertisingParameters, pAdvertisingParameters);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}
#endif


bleResult_t Gap_SetAdvertisingData
(
    gapAdvertisingData_t*         pAdvertisingData,
    gapScanResponseData_t*        pScanResponseData
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SetAdvertisingData, pAdvertisingData, pScanResponseData);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_StartAdvertising
(
    gapAdvertisingCallback_t    advertisingCallback,
    gapConnectionCallback_t     connectionCallback
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(StartAdvertising, advertisingCallback, connectionCallback);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}

#if gBLE50_d
bleResult_t Gap_StartExtAdvertising
(
    gapAdvertisingCallback_t    advertisingCallback,
    gapConnectionCallback_t     connectionCallback,
    uint8_t                     handle,
    uint16_t                    duration,
    uint8_t                     maxExtAdvEvents
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(StartExtAdvertising, advertisingCallback, connectionCallback, handle, duration, maxExtAdvEvents);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}
#endif

bleResult_t Gap_StopAdvertising(void)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(StopAdvertising);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}

#if gBLE50_d
bleResult_t Gap_StopExtAdvertising
(
    uint8_t handle
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(StopExtAdvertising, handle);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}
#endif

bleResult_t Gap_Authorize
(
    deviceId_t          deviceId,
    uint16_t            handle,
    gattDbAccessType_t  access
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(Authorize, deviceId, handle, access);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_SaveCccd
(
    deviceId_t      deviceId,
    uint16_t        handle,
    gattCccdFlags_t cccd
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SaveCccd, deviceId, handle, cccd);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_CheckNotificationStatus
(
    deviceId_t  deviceId,
    uint16_t    handle,
    bool_t*     pOutIsActive
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(CheckNotificationStatus, deviceId, handle, pOutIsActive);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_CheckIndicationStatus
(
    deviceId_t  deviceId,
    uint16_t    handle,
    bool_t*     pOutIsActive
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(CheckIndicationStatus, deviceId, handle, pOutIsActive);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_GetBondedStaticAddresses
(
    bleDeviceAddress_t* aOutDeviceAddresses,
    uint8_t             maxDevices,
    uint8_t*            pOutActualCount
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(GetBondedStaticAddresses, aOutDeviceAddresses, maxDevices, pOutActualCount);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_GetBondedDevicesIdentityInformation
(
    gapIdentityInformation_t* aOutIdentityAddresses,
    uint8_t                   maxDevices,
    uint8_t*                  pOutActualCount
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(GetBondedDevicesIdentityInformation, aOutIdentityAddresses, maxDevices, pOutActualCount);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_Pair
(
    deviceId_t                  deviceId,
    gapPairingParameters_t*     pPairingParameters
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(Pair, deviceId, pPairingParameters);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_SendPeripheralSecurityRequest
(
    deviceId_t                  deviceId,
    gapPairingParameters_t*     pPairingParameters
)
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SendPeripheralSecurityRequest, deviceId, pPairingParameters);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_EncryptLink
(
        deviceId_t              deviceId
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(EncryptLink, deviceId);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_AcceptPairingRequest
(
    deviceId_t                  deviceId,
    gapPairingParameters_t*     pPairingParameters
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(AcceptPairingRequest, deviceId, pPairingParameters);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_RejectPairing
(
    deviceId_t                          deviceId,
    gapAuthenticationRejectReason_t     reason
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(RejectPairing, deviceId, reason);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_EnterPasskey
(
    deviceId_t  deviceId,
    uint32_t    passkey
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(EnterPasskey, deviceId, passkey);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_ProvideOob
(
    deviceId_t      deviceId,
    uint8_t*        aOob
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(ProvideOob, deviceId, aOob);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_RejectPasskeyRequest
(
    deviceId_t deviceId
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(RejectPasskeyRequest, deviceId);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_SendSmpKeys
(
    deviceId_t      deviceId,
    gapSmpKeys_t*   pKeys
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SendSmpKeys, deviceId, pKeys);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_RejectKeyExchangeRequest
(
    deviceId_t deviceId
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(RejectKeyExchangeRequest, deviceId);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_ProvideLongTermKey
(
    deviceId_t  deviceId,
    uint8_t*    aLtk,
    uint8_t     ltkSize
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(ProvideLongTermKey, deviceId, aLtk, ltkSize);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_DenyLongTermKey
(
    deviceId_t deviceId
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(DenyLongTermKey, deviceId);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_LoadEncryptionInformation
(
    deviceId_t                  deviceId,
    uint8_t*                    aOutLtk,
    uint8_t*                    pOutLtkSize
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(LoadEncryptionInformation, deviceId, aOutLtk, pOutLtkSize);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_SetLocalPasskey
(
    uint32_t passkey
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SetLocalPasskey, passkey);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_StartScanning
(
    gapScanningParameters_t*    pScanningParameters,
    gapScanningCallback_t       scanningCallback,
    gapFilterDuplicates_t       enableFilterDuplicates,
    uint16_t                    duration,
    uint16_t                    period

)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(StartScanning, pScanningParameters, scanningCallback, enableFilterDuplicates, duration, period);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_StopScanning(void)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(StopScanning);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_Connect
(
    gapConnectionRequestParameters_t*   pParameters,
    gapConnectionCallback_t             connCallback
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(Connect, pParameters, connCallback);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_Disconnect
(
    deviceId_t deviceId
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(Disconnect, deviceId);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_SaveCustomPeerInformation
(
    uint8_t  deviceId,
    uint8_t* aInfo,
    uint16_t offset,
    uint16_t infoSize
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SaveCustomPeerInformation, deviceId, aInfo, offset, infoSize);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_LoadCustomPeerInformation
(
    uint8_t  deviceId,
    uint8_t* aOutInfo,
    uint16_t offset,
    uint16_t infoSize
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(LoadCustomPeerInformation, deviceId, aOutInfo, offset, infoSize);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_CheckIfBonded
(
    deviceId_t deviceId,
    bool_t* pOutIsBonded
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(CheckIfBonded, deviceId, pOutIsBonded);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_ReadFilterAcceptListSize(void)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(ReadFilterAcceptListSize);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_ClearFilterAcceptList(void)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(ClearFilterAcceptList);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_AddDeviceToFilterAcceptList
(
    bleAddressType_t    addressType,
    bleDeviceAddress_t  address
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(AddDeviceToFilterAcceptList, addressType, address);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_RemoveDeviceFromFilterAcceptList
(
    bleAddressType_t    addressType,
    bleDeviceAddress_t  address
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(RemoveDeviceFromFilterAcceptList, addressType, address);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_ReadPublicDeviceAddress(void)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(ReadPublicDeviceAddress);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_CreateRandomDeviceAddress
(
    uint8_t* aIrk,
    uint8_t* aRandomPart
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(CreateRandomDeviceAddress, aIrk, aRandomPart);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_SaveDeviceName
(
    deviceId_t  deviceId,
    uchar_t*    aName,
    uint8_t     cNameSize
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SaveDeviceName, deviceId, aName, cNameSize);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_GetBondedDevicesCount
(
    uint8_t* pOutBondedDevicesCount
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(GetBondedDevicesCount, pOutBondedDevicesCount);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_GetBondedDeviceName
(
    uint8_t     nvmIndex,
    uchar_t*    aOutName,
    uint8_t     maxNameSize
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(GetBondedDeviceName, nvmIndex, aOutName, maxNameSize);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_VerifyPrivateResolvableAddress
(
    uint8_t             nvmIndex,
    bleDeviceAddress_t  aAddress
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(VerifyPrivateResolvableAddress, nvmIndex, aAddress);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_SetRandomAddress
(
    bleDeviceAddress_t aAddress
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SetRandomAddress, aAddress);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_RemoveBond
(
    uint8_t nvmIndex
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(RemoveBond, nvmIndex);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_RemoveAllBonds(void)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(RemoveAllBonds);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_ReadRadioPowerLevel
(
    gapRadioPowerLevelReadType_t    txPowerReadType,
    deviceId_t                      deviceId
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(ReadRadioPowerLevel, txPowerReadType, deviceId);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_SetDefaultPairingParameters
(
    gapPairingParameters_t* pPairingParameters
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SetDefaultPairingParameters, pPairingParameters);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_UpdateConnectionParameters
(
    deviceId_t  deviceId,
    uint16_t    intervalMin,
    uint16_t    intervalMax,
    uint16_t    peripheralLatency,
    uint16_t    timeoutMultiplier,
    uint16_t    minCeLength,
    uint16_t    maxCeLength
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(UpdateConnectionParameters, deviceId, intervalMin, intervalMax, peripheralLatency, timeoutMultiplier, minCeLength, maxCeLength);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_EnableUpdateConnectionParameters
(
    deviceId_t  deviceId,
    bool_t      enable
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(EnableUpdateConnectionParameters, deviceId, enable);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_UpdateLeDataLength
(
    deviceId_t  deviceId,
    uint16_t    txOctets,
    uint16_t    txTime
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(UpdateLeDataLength, deviceId, txOctets, txTime);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_LeScRegeneratePublicKey
(
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(LeScRegeneratePublicKey);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_LeScValidateNumericValue
(
    deviceId_t  deviceId,
    bool_t      valid
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(LeScValidateNumericValue, deviceId, valid);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_LeScGetLocalOobData
(
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(LeScGetLocalOobData);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_LeScSetPeerOobData
(
    deviceId_t              deviceId,
    gapLeScOobData_t*       pPeerOobData
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(LeScSetPeerOobData, deviceId, pPeerOobData);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_LeScSendKeypressNotification
(
    deviceId_t                  deviceId,
    gapKeypressNotification_t   keypressNotification
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(LeScSendKeypressNotification, deviceId, keypressNotification);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_SetScanMode
(
    gapScanMode_t           scanMode,
    gapAutoConnectParams_t* pAutoConnectParams
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SetScanMode, scanMode, pAutoConnectParams);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_ControllerReset
(
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(ControllerReset);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_EnableHostPrivacy
(
    bool_t      enable,
    uint8_t*    aIrk
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(EnableHostPrivacy, enable, aIrk);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t Gap_EnableControllerPrivacy
(
    bool_t                      enable,
    uint8_t*                    aOwnIrk,
    uint8_t                     peerIdCount,
    gapIdentityInformation_t*   aPeerIdentities
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(EnableControllerPrivacy, enable, aOwnIrk, peerIdCount, aPeerIdentities);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}

#if gBLE50_d
bleResult_t Gap_UpdatePeriodicAdvList
(
    gapPeriodicAdvListOperation_t operation,
    bleAddressType_t addrType,
    uint8_t* pAddr,
    uint8_t SID
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(UpdatePeriodicAdvList, operation, addrType, pAddr, SID);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}

bleResult_t Gap_RemoveAdvSet
(
    uint8_t handle
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(RemoveAdvSet, handle);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}

bleResult_t Gap_SetPeriodicAdvParameters
(
    gapPeriodicAdvParameters_t *pAdvertisingParameters
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(SetPeriodicAdvParameters, pAdvertisingParameters);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}

bleResult_t Gap_StartPeriodicAdvertising
(
    uint8_t handle
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(StartPeriodicAdvertising, handle);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}

bleResult_t Gap_StopPeriodicAdvertising
(
    uint8_t handle
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(StopPeriodicAdvertising, handle);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}

bleResult_t Gap_PeriodicAdvCreateSync
(
    gapPeriodicAdvSyncReq_t * pReq,
    gapScanningCallback_t     scanningCallback
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGapOpcodeGroup_c, gBleGapStatusOpCode_c);
    FsciCmdMonitor(PeriodicAdvCreateSync, pReq, scanningCallback);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}

#endif /* gBLE50_d */

/*************************************************************************************
 *************************************************************************************
 * Private functions
 *************************************************************************************
 ************************************************************************************/

/*! *********************************************************************************
* @}
********************************************************************************** */
