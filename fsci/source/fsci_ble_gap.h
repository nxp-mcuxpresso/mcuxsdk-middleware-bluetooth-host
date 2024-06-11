/*! *********************************************************************************
 * \defgroup FSCI_BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2024 NXP
*
*
* \file
*
* This is a header file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef FSCI_BLE_GAP_H
#define FSCI_BLE_GAP_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble_gap_types.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/

/*! Macro which indicates if FSCI for GAP is enabled or not */
#ifndef gFsciBleGapLayerEnabled_d
    #define gFsciBleGapLayerEnabled_d           0
#endif /* gFsciBleGapLayerEnabled_d */

#ifndef gFsciBleGap2LayerEnabled_d
    #define gFsciBleGap2LayerEnabled_d           0
#endif /* gFsciBleGap2LayerEnabled_d */

/*! FSCI operation group for GAP */
#define gFsciBleGapOpcodeGroup_c                0x48

/*! FSCI operation group for GAP2 */
#define gFsciBleGap2OpcodeGroup_c               0x4C

#if defined(FsciCmdMonitor)
    //#warning "FsciCmdMonitor macro is already defined"
    #undef FsciCmdMonitor
#endif

#if defined(FsciStatusMonitor)
    //#warning "FsciStatusMonitor macro is already defined"
    #undef FsciStatusMonitor
#endif

#if defined(FsciEvtMonitor)
    //#warning "FsciEvtMonitor macro is already defined"
    #undef FsciEvtMonitor
#endif

/*! Macros used for monitoring commands, statuses and events */
#if gFsciIncluded_c && gFsciBleGapLayerEnabled_d

    #if gFsciBleHost_d || gFsciBleTest_d
        #define FsciCmdMonitor(function, ...)       fsciBleGap##function##CmdMonitor(__VA_ARGS__)
    #else
        #define FsciCmdMonitor(function, ...)
    #endif

    #if gFsciBleBBox_d || gFsciBleTest_d
        #define FsciStatusMonitor(function, ...)    fsciBleGapStatusMonitor(__VA_ARGS__)
        #define FsciEvtMonitor(function, ...)       fsciBleGap##function##EvtMonitor(__VA_ARGS__)
    #else
        #define FsciStatusMonitor(function, ...)
        #define FsciEvtMonitor(function, ...)
    #endif

#else

    #define FsciCmdMonitor(function, ...)
    #define FsciStatusMonitor(function, ...)
    #define FsciEvtMonitor(function, ...)

#endif

/*! *********************************************************************************
* \brief  Allocates a FSCI packet for GAP.
*
* \param[in]    opCode      FSCI GAP operation code
* \param[in]    dataSize    Size of the payload
*
* \return The allocated FSCI packet
*
********************************************************************************** */
#define fsciBleGapAllocFsciPacket(opCode, dataSize)         \
        fsciBleAllocFsciPacket(gFsciBleGapOpcodeGroup_c,    \
                               (opCode),                      \
                               (dataSize))

/*! *********************************************************************************
* \brief  Allocates a FSCI packet for GAP2.
*
* \param[in]    opCode      FSCI GAP2 operation code
* \param[in]    dataSize    Size of the payload
*
* \return The allocated FSCI packet
*
********************************************************************************** */
#define fsciBleGap2AllocFsciPacket(opCode, dataSize)         \
        fsciBleAllocFsciPacket(gFsciBleGap2OpcodeGroup_c,    \
                               (opCode),                      \
                               (dataSize))

/*! *********************************************************************************
* \brief  Gap_StopAdvertising command monitoring macro.
*
********************************************************************************** */
#define fsciBleGapStopAdvertisingCmdMonitor()   \
        fsciBleGapNoParamCmdMonitor(gBleGapCmdStopAdvertisingOpCode_c)

/*! *********************************************************************************
* \brief  Gap_StopScanning command monitoring macro.
*
********************************************************************************** */
#define fsciBleGapStopScanningCmdMonitor()  \
        fsciBleGapNoParamCmdMonitor(gBleGapCmdStopScanningOpCode_c)

/*! *********************************************************************************
* \brief  Gap_ReadFilterAcceptListSize command monitoring macro.
*
********************************************************************************** */
#define fsciBleGapReadFilterAcceptListSizeCmdMonitor() \
        fsciBleGapNoParamCmdMonitor(gBleGapCmdReadFilterAcceptListSizeOpCode_c)

/*! *********************************************************************************
* \brief  Gap_ClearFilterAcceptList command monitoring macro.
*
********************************************************************************** */
#define fsciBleGapClearFilterAcceptListCmdMonitor()    \
        fsciBleGapNoParamCmdMonitor(gBleGapCmdClearFilterAcceptListOpCode_c)

/*! *********************************************************************************
* \brief  Gap_RemoveAllBonds command monitoring macro.
*
********************************************************************************** */
#define fsciBleGapRemoveAllBondsCmdMonitor()    \
        fsciBleGapNoParamCmdMonitor(gBleGapCmdRemoveAllBondsOpCode_c)

/*! *********************************************************************************
* \brief  Gap_EncryptLink command monitoring macro.
*
* \param[in]    deviceId    Device ID of the peer.
*
********************************************************************************** */
#define fsciBleGapEncryptLinkCmdMonitor(deviceId)                           \
        fsciBleGapDeviceIdParamCmdMonitor(gBleGapCmdEncryptLinkOpCode_c,    \
                                          (deviceId))

/*! *********************************************************************************
* \brief  Gap_RemoveBond command monitoring macro.
*
* \param[in]    nvmIndex    Index of the device in NVM bonding area.
*
********************************************************************************** */
#define fsciBleGapRemoveBondCmdMonitor(nvmIndex)                        \
        fsciBleGapUint8ParamCmdMonitor(gBleGapCmdRemoveBondOpCode_c,    \
                                       (nvmIndex))

/*! *********************************************************************************
* \brief  Gap_RejectPasskeyRequest command monitoring macro.
*
* \param[in]    deviceId    The GAP peer that requested a passkey entry.
*
********************************************************************************** */
#define fsciBleGapRejectPasskeyRequestCmdMonitor(deviceId)                          \
        fsciBleGapDeviceIdParamCmdMonitor(gBleGapCmdRejectPasskeyRequestOpCode_c,   \
                                          (deviceId))

/*! *********************************************************************************
* \brief  Gap_RejectKeyExchangeRequest command monitoring macro.
*
* \param[in]    deviceId    The GAP peer who requested the Key Exchange procedure.
*
********************************************************************************** */
#define fsciBleGapRejectKeyExchangeRequestCmdMonitor(deviceId)                          \
        fsciBleGapDeviceIdParamCmdMonitor(gBleGapCmdRejectKeyExchangeRequestOpCode_c,   \
                                          (deviceId))

/*! *********************************************************************************
* \brief  Gap_DenyLongTermKey command monitoring macro.
*
* \param[in]    deviceId    The GAP peer who requested encryption.
*
********************************************************************************** */
#define fsciBleGapDenyLongTermKeyCmdMonitor(deviceId)                           \
        fsciBleGapDeviceIdParamCmdMonitor(gBleGapCmdDenyLongTermKeyOpCode_c,    \
                                          (deviceId))

/*! *********************************************************************************
* \brief  Gap_Disconnect command monitoring macro.
*
* \param[in]    deviceId    The connected peer to disconnect from.
*
********************************************************************************** */
#define fsciBleGapDisconnectCmdMonitor(deviceId)                        \
        fsciBleGapDeviceIdParamCmdMonitor(gBleGapCmdDisconnectOpCode_c, \
                                          (deviceId))

/*! *********************************************************************************
* \brief  Gap_CheckNotificationStatus command monitoring macro.
*
* \param[in]    deviceId        The peer GATT Client.
* \param[in]    handle          The handle of the CCCD.
* \param[in]    pOutIsActive    The address to store the status into.
*
********************************************************************************** */
#define fsciBleGapCheckNotificationStatusCmdMonitor(deviceId,                                           \
                                                    handle,                                             \
                                                    pOutIsActive)                                       \
        fsciBleGapCheckNotificationsAndIndicationsCmdMonitor(gBleGapCmdCheckNotificationStatusOpCode_c, \
                                                             (deviceId),                                  \
                                                             (handle),                                    \
                                                             (pOutIsActive))

/*! *********************************************************************************
* \brief  Gap_CheckIndicationStatus command monitoring macro.
*
* \param[in]    deviceId        The peer GATT Client.
* \param[in]    handle          The handle of the CCCD.
* \param[in]    pOutIsActive    The address to store the status into.
*
********************************************************************************** */
#define fsciBleGapCheckIndicationStatusCmdMonitor(deviceId,                                             \
                                                  handle,                                               \
                                                  pOutIsActive)                                         \
        fsciBleGapCheckNotificationsAndIndicationsCmdMonitor(gBleGapCmdCheckIndicationStatusOpCode_c,   \
                                                             (deviceId),                                  \
                                                             (handle),                                    \
                                                             (pOutIsActive))

/*! *********************************************************************************
* \brief  Gap_AddDeviceToFilterAcceptList command monitoring macro.
*
* \param[in]    address         The address of the device to be added to the filter
*                               accept list.
* \param[in]    addressType     The device address type (public or random).
*
********************************************************************************** */
#define fsciBleGapAddDeviceToFilterAcceptListCmdMonitor(addressType,                       \
                                                 address)                           \
        fsciBleGapAddressParamsCmdMonitor(gBleGapCmdAddDeviceToFilterAcceptListOpCode_c,   \
                                          (addressType),                              \
                                          (address))

/*! *********************************************************************************
* \brief  Gap_RemoveDeviceFromFilterAcceptList command monitoring macro.
*
* \param[in]    address         The address of the device in the filter accept list.
* \param[in]    addressType     The device address type (public or random).
*
********************************************************************************** */
#define fsciBleGapRemoveDeviceFromFilterAcceptListCmdMonitor(addressType,                      \
                                                      address)                          \
        fsciBleGapAddressParamsCmdMonitor(gBleGapCmdRemoveDeviceFromFilterAcceptListOpCode_c,  \
                                          (addressType),                                  \
                                          (address))

/*! *********************************************************************************
* \brief  Gap_ReadPublicDeviceAddress command monitoring macro.
*
********************************************************************************** */
#define fsciBleGapReadPublicDeviceAddressCmdMonitor()   \
        fsciBleGapNoParamCmdMonitor(gBleGapCmdReadPublicDeviceAddressOpCode_c)

/*! *********************************************************************************
* \brief  Gap_LeScRegeneratePublicKey command monitoring macro.
*
********************************************************************************** */
#define fsciBleGapLeScRegeneratePublicKeyCmdMonitor()   \
        fsciBleGapNoParamCmdMonitor(gBleGapCmdLeScRegeneratePublicKeyOpCode_c)

/*! *********************************************************************************
* \brief  Gap_LeScGetLocalOobData command monitoring macro.
*
********************************************************************************** */
#define fsciBleGapLeScGetLocalOobDataCmdMonitor()   \
        fsciBleGapNoParamCmdMonitor(gBleGapCmdLeScGetLocalOobDataOpCode_c)

/*! *********************************************************************************
* \brief  Gap_ReadChannelMap command monitoring macro.
*
* \param[in]    deviceId    The connected peer to read the channel map.
*
********************************************************************************** */
#define fsciBleGapReadChannelMapCmdMonitor(deviceId)                \
        fsciBleGapDeviceIdParamCmdMonitor(gBleGapCmdReadChannelMapOpCode_c,   \
                                          (deviceId))

/*! *********************************************************************************
* \brief  Gap_RemoveAdvSet command monitoring macro.
*
* \param[in]    handle                 Set ID to remove or 0xFF to remove all
*
********************************************************************************** */
#define fsciBleGapRemoveAdvSetCmdMonitor(handle)                                    \
        fsciBleGapUint8ParamCmdMonitor(gBleGapCmdRemoveAdvSetOpCode_c,              \
                                       (handle))

/*! *********************************************************************************
* \brief  Gap_CheckNotificationStatus out parameter monitoring macro.
*
* \param[in]    pOutIsActive    Pointer to the notification status.
*
********************************************************************************** */
#define fsciBleGapCheckNotificationStatusEvtMonitor(pOutIsActive)                   \
        fsciBleGapBoolParamEvtMonitor(gBleGapEvtCheckNotificationStatusOpCode_c,    \
                                      *(pOutIsActive))

/*! *********************************************************************************
* \brief  Gap_CheckIndicationStatus out parameter monitoring macro.
*
* \param[in]    pOutIsActive    Pointer to the indication status.
*
********************************************************************************** */
#define fsciBleGapCheckIndicationStatusEvtMonitor(pOutIsActive)                 \
        fsciBleGapBoolParamEvtMonitor(gBleGapEvtCheckIndicationStatusOpCode_c,  \
                                      *(pOutIsActive))

/*! *********************************************************************************
* \brief  Gap_LoadEncryptionInformation out parameters monitoring macro.
*
* \param[in]    aOutLtk         Array filled with the LTK.
* \param[in]    pOutLtkSize     The LTK size.
*
********************************************************************************** */
#define fsciBleGapLoadEncryptionInformationEvtMonitor(aOutLtk,                              \
                                                      pOutLtkSize)                          \
        fsciBleGapArrayAndSizeParamEvtMonitor(gBleGapEvtLoadEncryptionInformationOpCode_c,  \
                                              (aOutLtk),                                    \
                                              *(pOutLtkSize))

/*! *********************************************************************************
* \brief  Gap_GetBondedDeviceName out parameter monitoring macro.
*
* \param[in]    aOutName    Destination array that keeps the device name.
*
********************************************************************************** */
#define fsciBleGapGetBondedDeviceNameEvtMonitor(aOutName)                               \
        fsciBleGapArrayAndSizeParamEvtMonitor(gBleGapEvtGetBondedDeviceNameOpCode_c,    \
                                              (uint8_t*)(aOutName),                     \
                                              (uint8_t)strlen((char const*)(aOutName)))

/*! *********************************************************************************
* \brief  Gap_GetBondedDevicesCount out parameter monitoring macro.
*
* \param[in]    pOutBondedDevicesCount  Pointer to the number of bonded devices.
*
********************************************************************************** */
#define fsciBleGapGetBondedDevicesCountEvtMonitor(pOutBondedDevicesCount)               \
        fsciBleGapArrayAndSizeParamEvtMonitor(gBleGapEvtGetBondedDevicesCountOpCode_c,  \
                                              NULL,                                     \
                                              *(pOutBondedDevicesCount))

#if defined(gBLE50_d) && (gBLE50_d == 1U)
/*! *********************************************************************************
* \brief  Gap_StopExtAdvertising command monitoring function.
*
* \param[in]    handle                   The ID of the advertising set
*
********************************************************************************** */
#define fsciBleGapStopExtAdvertisingCmdMonitor(handle)                              \
        fsciBleGapUint8ParamCmdMonitor(gBleGapCmdStopExtAdvertisingOpCode_c,        \
                                       (handle))

/*! *********************************************************************************
* \brief  Gap_StartPeriodicAdvertising command monitoring function.
*
* \param[in]    handle                   The ID of the advertising set
*
********************************************************************************** */
#define fsciBleGapStartPeriodicAdvertisingCmdMonitor(handle)                        \
        fsciBleGapUint8ParamCmdMonitor(gBleGapCmdStartPeriodicAdvOpCode_c,          \
                                       (handle))

/*! *********************************************************************************
* \brief  Gap_StopPeriodicAdvertising command monitoring function.
*
* \param[in]    handle                   The ID of the advertising set
*
********************************************************************************** */
#define fsciBleGapStopPeriodicAdvertisingCmdMonitor(handle)                         \
        fsciBleGapUint8ParamCmdMonitor(gBleGapCmdStopPeriodicAdvOpCode_c,           \
                                       (handle))

/*! *********************************************************************************
* \brief  Gap_PeriodicAdvTerminateSync command monitoring function.
*
* \param[in]    SID                   The ID of the advertising set
*
********************************************************************************** */
#define fsciBleGapPeriodicAdvTerminateSyncCmdMonitor(SID)                            \
        fsciBleGapUint8ParamCmdMonitor(gBleGapCmdPeriodicAdvTerminateSyncOpCode_c,   \
                                       (SID))
#endif

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*! FSCI operation codes for GAP */
typedef enum
{
    gBleGapModeSelectOpCode_c                                                      = 0x00,                       /*! GAP Mode Select operation code */

    gBleGapCmdFirstOpCode_c                                                        = 0x01,
    gBleGapCmdBleHostInitializeOpCode_c                                            = gBleGapCmdFirstOpCode_c,    /*! Ble_HostInitialize command operation code */
    gBleGapCmdRegisterDeviceSecurityRequirementsOpCode_c                           = 0x02,                       /*! Gap_RegisterDeviceSecurityRequirements command operation code */
    gBleGapCmdSetAdvertisingParametersOpCode_c                                     = 0x03,                       /*! Gap_SetAdvertisingParameters command operation code */
    gBleGapCmdSetAdvertisingDataOpCode_c                                           = 0x04,                       /*! Gap_SetAdvertisingData command operation code */
    gBleGapCmdStartAdvertisingOpCode_c                                             = 0x05,                       /*! Gap_StartAdvertising command operation code */
    gBleGapCmdStopAdvertisingOpCode_c                                              = 0x06,                       /*! Gap_StopAdvertising command operation code */
    gBleGapCmdAuthorizeOpCode_c                                                    = 0x07,                       /*! Gap_Authorize command operation code */
    gBleGapCmdSaveCccdOpCode_c                                                     = 0x08,                       /*! Gap_SaveCccd command operation code */
    gBleGapCmdCheckNotificationStatusOpCode_c                                      = 0x09,                       /*! Gap_CheckNotificationStatus command operation code */
    gBleGapCmdCheckIndicationStatusOpCode_c                                        = 0x0A,                       /*! Gap_CheckIndicationStatus command operation code */
    gFsciBleGapReserved1_c                                                         = 0x0B,                       /*! Removed: Gap_GetBondedStaticAddresses command operation code */
    gBleGapCmdPairOpCode_c                                                         = 0x0C,                       /*! Gap_Pair command operation code */
    gBleGapCmdSendPeripheralSecurityRequestOpCode_c                                = 0x0D,                       /*! Gap_SendPeripheralSecurityRequest command operation code */
    gBleGapCmdEncryptLinkOpCode_c                                                  = 0x0E,                       /*! Gap_EncryptLink command operation code */
    gBleGapCmdAcceptPairingRequestOpCode_c                                         = 0x0F,                       /*! Gap_AcceptPairingRequest command operation code */
    gBleGapCmdRejectPairingOpCode_c                                                = 0x10,                       /*! Gap_RejectAuthentication command operation code */
    gBleGapCmdEnterPasskeyOpCode_c                                                 = 0x11,                       /*! Gap_EnterPasskey command operation code */
    gBleGapCmdProvideOobOpCode_c                                                   = 0x12,                       /*! Gap_ProvideOob command operation code */
    gBleGapCmdRejectPasskeyRequestOpCode_c                                         = 0x13,                       /*! Gap_RejectPasskeyRequest command operation code */
    gBleGapCmdSendSmpKeysOpCode_c                                                  = 0x14,                       /*! Gap_SendSmpKeys command operation code */
    gBleGapCmdRejectKeyExchangeRequestOpCode_c                                     = 0x15,                       /*! Gap_RejectKeyExchangeRequest command operation code */
    gBleGapCmdProvideLongTermKeyOpCode_c                                           = 0x16,                       /*! Gap_ProvideLongTermKey command operation code */
    gBleGapCmdDenyLongTermKeyOpCode_c                                              = 0x17,                       /*! Gap_DenyLongTermKey command operation code */
    gBleGapCmdLoadEncryptionInformationOpCode_c                                    = 0x18,                       /*! Gap_LoadEncryptionInformation command operation code */
    gBleGapCmdSetLocalPasskeyOpCode_c                                              = 0x19,                       /*! Gap_SetLocalPasskey command operation code */
    gBleGapCmdStartScanningOpCode_c                                                = 0x1A,                       /*! Gap_StartScanning command operation code */
    gBleGapCmdStopScanningOpCode_c                                                 = 0x1B,                       /*! Gap_StopScanning command operation code */
    gBleGapCmdConnectOpCode_c                                                      = 0x1C,                       /*! Gap_Connect command operation code */
    gBleGapCmdDisconnectOpCode_c                                                   = 0x1D,                       /*! Gap_Disconnect command operation code */
    gBleGapCmdSaveCustomPeerInformationOpCode_c                                    = 0x1E,                       /*! Gap_SaveCustomPeerInformation command operation code */
    gBleGapCmdLoadCustomPeerInformationOpCode_c                                    = 0x1F,                       /*! Gap_LoadCustomPeerInformation command operation code */
    gBleGapCmdCheckIfBondedOpCode_c                                                = 0x20,                       /*! Gap_CheckIfBonded command operation code */
    gBleGapCmdReadFilterAcceptListSizeOpCode_c                                     = 0x21,                       /*! Gap_ReadFilterAcceptListSize command operation code */
    gBleGapCmdClearFilterAcceptListOpCode_c                                        = 0x22,                       /*! Gap_ClearFilterAcceptList command operation code */
    gBleGapCmdAddDeviceToFilterAcceptListOpCode_c                                  = 0x23,                       /*! Gap_AddDeviceToFilterAcceptList command operation code */
    gBleGapCmdRemoveDeviceFromFilterAcceptListOpCode_c                             = 0x24,                       /*! Gap_RemoveDeviceFromFilterAcceptList command operation code */
    gBleGapCmdReadPublicDeviceAddressOpCode_c                                      = 0x25,                       /*! Gap_ReadPublicDeviceAddress command operation code */
    gBleGapCmdCreateRandomDeviceAddressOpCode_c                                    = 0x26,                       /*! Gap_CreateRandomDeviceAddress command operation code */
    gBleGapCmdSaveDeviceNameOpCode_c                                               = 0x27,                       /*! Gap_SaveDeviceName command operation code */
    gBleGapCmdGetBondedDevicesCountOpCode_c                                        = 0x28,                       /*! Gap_GetBondedDevicesCount command operation code */
    gBleGapCmdGetBondedDeviceNameOpCode_c                                          = 0x29,                       /*! Gap_GetBondedDeviceName command operation code */
    gBleGapCmdRemoveBondOpCode_c                                                   = 0x2A,                       /*! Gap_RemoveBond command operation code */
    gBleGapCmdRemoveAllBondsOpCode_c                                               = 0x2B,                       /*! Gap_RemoveAllBonds command operation code */
    gBleGapCmdReadRadioPowerLevelOpCode_c                                          = 0x2C,                       /*! Gap_ReadRadioPowerLevel command operation code */
    gBleGapCmdVerifyPrivateResolvableAddressOpCode_c                               = 0x2D,                       /*! Gap_VerifyPrivateResolvableAddress command operation code */
    gBleGapCmdSetRandomAddressOpCode_c                                             = 0x2E,                       /*! Gap_SetRandomAddress command operation code */
    gBleGapCmdSetScanModeOpCode_c                                                  = 0x2F,                       /*! Gap_SetScanMode command operation code */
    gBleGapCmdSetDefaultPairingParameters_c                                        = 0x30,                       /*! Gap_SetDefaultPairingParameters command operation code */
    gBleGapCmdUpdateConnectionParametersOpCode_c                                   = 0x31,                       /*! Gap_UpdateConnectionParameters command operation code */
    gBleGapCmdEnableUpdateConnectionParametersOpCode_c                             = 0x32,                       /*! Gap_EnableUpdateConnectionParameters command operation code */
    gBleGapCmdUpdateLeDataLengthOpCode_c                                           = 0x33,                       /*! Gap_UpdateLeDataLength command operation code */
    gReserved1_c                                                                   = 0x34,
    gBleGapCmdEnableHostPrivacyOpCode_c                                            = 0x35,                       /*! Gap_EnableHostPrivacy command operation code */
    gBleGapCmdEnableControllerPrivacyOpCode_c                                      = 0x36,                       /*! Gap_EnableControllerPrivacy command operation code */
    gBleGapCmdLeScRegeneratePublicKeyOpCode_c                                      = 0x37,                       /*! Gap_LeScRegeneratePublicKey command operation code */
    gBleGapCmdLeScValidateNumericValueOpCode_c                                     = 0x38,                       /*! Gap_LeScValidateNumericValue command operation code */
    gBleGapCmdLeScGetLocalOobDataOpCode_c                                          = 0x39,                       /*! Gap_LeScGetLocalOobData command operation code */
    gBleGapCmdLeScSetPeerOobDataOpCode_c                                           = 0x3A,                       /*! Gap_LeScSetPeerOobData command operation code */
    gBleGapCmdLeScSendKeypressNotificationPrivacyOpCode_c                          = 0x3B,                       /*! Gap_LeScSendKeypressNotification command operation code */
    gBleGapCmdGetBondedDevicesIdentityInformationOpCode_c                          = 0x3C,                       /*! Gap_GetBondedDevicesIdentityInformation command operation code */
    gBleGapCmdSetTxPowerLevelOpCode_c                                              = 0x3D,                       /*! Gap_SetTxPowerLevel command operation code */
    gBleGapCmdLeReadPhyOpCode_c                                                    = 0x3E,                       /*! Gap_LeReadPhy command operation code */
    gBleGapCmdLeSetPhyOpCode_c                                                     = 0x3F,                       /*! Gap_LeSetPhy command operation code */
    gBleGapCmdControllerNotificationOpCode_c                                       = 0x40,                       /*! Gap_ControllerEnhancedNotification command operation code */
    gBleGapCmdLoadKeysOpCode_c                                                     = 0x41,                       /*! Gap_LoadKeys command operation code */
    gBleGapCmdSaveKeysOpCode_c                                                     = 0x42,                       /*! Gap_SaveKeys command operation code */
    gBleGapCmdSetChannelMapOpCode_c                                                = 0x43,                       /*! Gap_SetChannelMap command operation code */
    gBleGapCmdReadChannelMapOpCode_c                                               = 0x44,                       /*! Gap_ReadChannelMap command operation code */

    gBleGapCmdSetPrivacyModeOpCode_c                                               = 0x45,                       /*! Gap_SetPrivacyMode command operation code */
    gBleCtrlCmdSetScanDupFiltModeOpCode_c                                          = 0x46,                       /*! Controller_SetScanDupFiltMode command operation code */
    gBleGapCmdReadControllerLocalRPAOpCode_c                                       = 0x47,                       /*! Gap_ReadLocalPrivateAddress command operation code */
    gBleCtrlWritePublicDeviceAddressOpCode_c                                       = 0x48,                       /*! Write Public Device Address */
    gBleGapCmdCheckNvmIndexOpCode_c                                                = 0x49,                       /*! Gap_CheckNvmIndex command operation code */
    gBleGapCmdGetDeviceIdFromConnHandleOpCode_c                                    = 0x4A,                       /*! Gap_GetDeviceIdFromConnHandle command operation code */
    gBleGapCmdGetConnectionHandleFromDeviceIdOpCode_c                              = 0x4B,                       /*! Gap_GetConnectionHandleFromDeviceId command operation code */
    gBleGapCmdAdvIndexChangeOpCode_c                                               = 0x4C,                       /*! Gap_BleAdvIndexChange command operation code */
    gBleGapCmdGetHostVersionOpCode_c                                               = 0x4D,                       /*! Gap_GetHostVersion command operation code */
    gBleGapCmdGetReadRemoteVersionInfoOpCode_c                                     = 0x4E,                       /*! Gap_ReadRemoteVersionInformation command operation code */
    gBleGapCmdGetConnParamsOpCode_c                                                = 0x4F,                       /*! Gap_GetConnParamsMonitoring command operation code */

    /* BLE 5.0 */
    gBleGapCmd50FirstOpCode_c                                                      = 0x50,
    gBleGapCmdSetExtAdvertisingParametersOpCode_c                                  = gBleGapCmd50FirstOpCode_c,  /*! Gap_SetExtAdvertisingParameters command operation code */
    gBleGapCmdStartExtAdvertisingOpCode_c                                          = 0x51,                       /*! Gap_StartExtAdvertising command operation code */
    gBleGapCmdRemoveAdvSetOpCode_c                                                 = 0x52,                       /*! Gap_RemoveAdvSet command operation code */
    gBleGapCmdStopExtAdvertisingOpCode_c                                           = 0x53,                       /*! Gap_StopExtAdvertising command operation code */
    gBleGapCmdUpdatePeriodicAdvListOpCode_c                                        = 0x54,                       /*! Gap_UpdatePeriodicAdvList command operation code */
    gBleGapCmdSetPeriodicAdvParametersOpCode_c                                     = 0x55,                       /*! Gap_SetPeriodicAdvParameters command operation code */
    gBleGapCmdStartPeriodicAdvOpCode_c                                             = 0x56,                       /*! Gap_StartPeriodicAdvertising command operation code */
    gBleGapCmdStopPeriodicAdvOpCode_c                                              = 0x57,                       /*! Gap_StopPeriodicAdvertising command operation code */
    gBleGapCmdSetExtAdvertisingDataOpCode_c                                        = 0x58,                       /*! Gap_SetExtAdvertisingData command operation code */
    gBleGapCmdSetPeriodicAdvDataOpCode_c                                           = 0x59,                       /*! Gap_SetPeriodicAdvData command operation code */
    gBleGapCmdPeriodicAdvCreateSyncOpCode_c                                        = 0x5A,                       /*! Gap_PeriodicAdvCreateSync command operation code */
    gBleGapCmdPeriodicAdvTerminateSyncOpCode_c                                     = 0x5B,                       /*! Gap_PeriodicAdvTerminateSync command operation code */
    gBleCtrlCmdConfigureAdvCodingSchemeOpCode_c                                    = 0x5C,                       /*! Controller_ConfigureAdvCodingScheme command operation code */

    /* BLE 5.1 */
    gBleGapCmdSetConnectionlessCteTransmitParametersOpCode_c                       = 0x5D,                       /*! Gap_SetConnectionlessCteTransmitParameters command operation code */
    gBleGapCmdEnableConnectionlessCteTransmitOpCode_c                              = 0x5E,                       /*! Gap_EnableConnectionlessCteTransmit command operation code */
    gBleGapCmdEnableConnectionlessIqSamplingOpCode_c                               = 0x5F,                       /*! Gap_EnableConnectionlessIqSampling command operation code */
    gBleGapCmdSetConnectionCteReceiveParametersOpCode_c                            = 0x60,                       /*! Gap_SetConnectionCteReceiveParameters command operation code */
    gBleGapCmdSetConnectionCteTransmitParametersOpCode_c                           = 0x61,                       /*! Gap_SetConnectionCteTransmitParameters command operation code */
    gBleGapCmdEnableConnectionCteRequestOpCode_c                                   = 0x62,                       /*! Gap_EnableConnectionCteRequest command operation code */
    gBleGapCmdEnableConnectionCteResponseOpCode_c                                  = 0x63,                       /*! Gap_EnableConnectionCteResponse command operation code */
    gBleGapCmdReadAntennaInformationOpCode_c                                       = 0x64,                       /*! Gap_ReadAntennaInformation command operation code */
    gBleGapCmdGenerateDhKeyV2OpCode_c                                              = 0x65,                       /*! Gap_GenerateDhKeyV2 command operation code */
    gBleGapCmdModifySleepClockAccuracy_c                                           = 0x66,                       /*! Gap_ModifySleepClockAccuracy command operation code */

    gBleGapCmdSetPeriodicAdvRecvEnableOpCode_c                                     = 0x67,                       /*! Gap_PeriodicAdvReceiveEnable command operation code */
    gBleGapCmdSetPeriodicAdvRecvDisableOpCode_c                                    = 0x68,                       /*! Gap_PeriodicAdvReceiveDisable command operation code */
    gBleGapCmdPeriodicAdvSyncTransferOpCode_c                                      = 0x69,                       /*! Gap_PeriodicAdvSyncTransfer command operation code */
    gBleGapCmdPeriodicAdvSetInfoTransferOpCode_c                                   = 0x6A,                       /*! Gap_PeriodicAdvSetInfoTransfer command operation code */
    gBleGapCmdSetPeriodicAdvSyncTransferParamsOpCode_c                             = 0x6B,                       /*! Gap_SetPeriodicAdvSyncTransferParams command operation code */
    gBleGapCmdSetDefaultPeriodicAdvSyncTransferParamsOpCode_c                      = 0x6C,                       /*! Gap_SetDefaultPeriodicAdvSyncTransferParams command operation code */

    /* BLE 5.2 */
    gBleGapCmdEnhancedReadTransmitPowerLevelOpCode_c                               = 0x6D,                       /*! Gap_EnhancedReadTransmitPowerLevel command operation code */
    gBleGapCmdReadRemoteTransmitPowerLevelOpCode_c                                 = 0x6E,                       /*! Gap_ReadRemoteTransmitPowerLevel command operation code */
    gBleGapCmdSetPathLossReportingParamsOpCode_c                                   = 0x6F,                       /*! Gap_SetPathLossReportingParameters command operation code */
    gBleGapCmdEnablePathLossReportingOpCode_c                                      = 0x70,                       /*! Gap_EnablePathLossReporting command operation code */
    gBleGapCmdEnableTransmitPowerReportingOpCode_c                                 = 0x71,                       /*! Gap_EnableTransmitPowerReporting command operation code */

    /* EATT */
    gBleGapCmdEattConnectionRequestOpCode_c                                        = 0x72,                       /*! Gap_EattConnectionRequest command operation code */
    gBleGapCmdEattConnectionAcceptOpCode_c                                         = 0x73,                       /*! Gap_EattConnectionRequest command operation code */
    gBleGapCmdEattReconfigureRequestOpCode_c                                       = 0x74,                       /*! Gap_EattReconfigureRequest command operation code */
    gBleGapCmdEattSendCreditsRequestOpCode_c                                       = 0x75,                       /*! Gap_EattSendCreditsRequest command operation code */
    gBleGapCmdEattDisconnectRequestOpCode_c                                        = 0x76,                       /*! Gap_EattDisconnect command operation code */
    gBleCtrlCmdSetConnNotificationModeOpCode_c                                     = 0x77,                       /*! Controller_SetConnNotificationMode command operation code*/

    gBleCmdDisablePrivacyPerAdvSetOpCode_c                                         = 0x78,                       /*! Configure an advertising set to ignore the Privacy setting */
    gBleGapCmdLeSetSchedulerPriority_c                                             = 0x79,                       /*! Set priority for one connection in case of several connections */
    gBleGapCmdLeSetHostFeature_c                                                   = 0x7B,                       /*! Set or clear a bit controlled by the Host in the Link Layer FeatureSet */
    gBleGapCmdPlatformRegisterErrorCallbackOpCode_c                                = 0x7C,                       /*! Register platform error callback */

    gBleGapStatusOpCode_c                                                          = 0x80,                       /*! GAP status operation code */

    gBleGapEvtFirstOpCode_c                                                        = 0x81,
    gBleGapEvtCheckNotificationStatusOpCode_c                                      = gBleGapEvtFirstOpCode_c,    /*! Gap_CheckNotificationStatus command out parameters event operation code */
    gBleGapEvtCheckIndicationStatusOpCode_c                                        = 0x82,                       /*! Gap_CheckIndicationStatus command out parameters event operation code */
    gBleGapEvtLoadKeysOpCode_c                                                     = 0x83,                       /*! Gap_LoadKeys command out parameters event operation code */
    gBleGapEvtLoadEncryptionInformationOpCode_c                                    = 0x84,                       /*! Gap_LoadEncryptionInformation command out parameters event operation code */
    gBleGapEvtLoadCustomPeerInformationOpCode_c                                    = 0x85,                       /*! Gap_LoadCustomPeerInformation command out parameters event operation code */
    gBleGapEvtCheckIfBondedOpCode_c                                                = 0x86,                       /*! Gap_CheckIfBonded command out parameters event operation code */
    gBleGapEvtGetBondedDevicesCountOpCode_c                                        = 0x87,                       /*! Gap_GetBondedDevicesCount command out parameters event operation code */
    gBleGapEvtGetBondedDeviceNameOpCode_c                                          = 0x88,                       /*! Gap_GetBondedDeviceName command out parameters event operation code */

    gBleGapEvtGenericEventInitializationCompleteOpCode_c                           = 0x89,                       /*! gapGenericCallback (type = gInitializationComplete_c) event operation code */
    gBleGapEvtGenericEventInternalErrorOpCode_c                                    = 0x8A,                       /*! gapGenericCallback (type = gInternalError_c) event operation code */
    gBleGapEvtGenericEventAdvertisingSetupFailedOpCode_c                           = 0x8B,                       /*! gapGenericCallback (type = gAdvertisingSetupFailed_c) event operation code */
    gBleGapEvtGenericEventAdvertisingParametersSetupCompleteOpCode_c               = 0x8C,                       /*! gapGenericCallback (type = gAdvertisingParametersSetupComplete_c) event operation code */
    gBleGapEvtGenericEventAdvertisingDataSetupCompleteOpCode_c                     = 0x8D,                       /*! gapGenericCallback (type = gAdvertisingDataSetupComplete_c) event operation code */
    gBleGapEvtGenericEventFilterAcceptListSizeReadOpCode_c                         = 0x8E,                       /*! gapGenericCallback (type = gFilterAcceptListSizeRead_c) event operation code */
    gBleGapEvtGenericEventDeviceAddedToFilterAcceptListOpCode_c                    = 0x8F,                       /*! gapGenericCallback (type = gDeviceAddedToFilterAcceptList_c) event operation code */
    gBleGapEvtGenericEventDeviceRemovedFromFilterAcceptListOpCode_c                = 0x90,                       /*! gapGenericCallback (type = gDeviceRemovedFromFilterAcceptList_c) event operation code */
    gBleGapEvtGenericEventFilterAcceptListClearedOpCode_c                          = 0x91,                       /*! gapGenericCallback (type = gFilterAcceptListCleared_c) event operation code */
    gBleGapEvtGenericEventRandomAddressReadyOpCode_c                               = 0x92,                       /*! gapGenericCallback (type = gRandomAddressReady_c) event operation code */
    gBleGapEvtGenericEventCreateConnectionCanceledOpCode_c                         = 0x93,                       /*! gapGenericCallback (type = gCreateConnectionCanceled_c) event operation code */
    gBleGapEvtGenericEventPublicAddressReadOpCode_c                                = 0x94,                       /*! gapGenericCallback (type = gPublicAddressRead_c) event operation code */
    gBleGapEvtGenericEventAdvTxPowerLevelReadOpCode_c                              = 0x95,                       /*! gapGenericCallback (type = gAdvTxPowerLevelRead_c) event operation code */
    gBleGapEvtGenericEventPrivateResolvableAddressVerifiedOpCode_c                 = 0x96,                       /*! gapGenericCallback (type = gPrivateResolvableAddressVerified_c) event operation code */
    gBleGapEvtGenericEventRandomAddressSetOpCode_c                                 = 0x97,                       /*! gapGenericCallback (type = gRandomAddressSet_c) event operation code */

    gBleGapEvtAdvertisingEventAdvertisingStateChangedOpCode_c                      = 0x98,                       /*! gapAdvertisingCallback (type = gAdvertisingStateChanged_c) event operation code */
    gBleGapEvtAdvertisingEventAdvertisingCommandFailedOpCode_c                     = 0x99,                       /*! gapAdvertisingCallback (type = gAdvertisingCommandFailed_c) event operation code */

    gBleGapEvtScanningEventScanStateChangedOpCode_c                                = 0x9A,                       /*! gapScanningCallback (type = gScanStateChanged_c) event operation code */
    gBleGapEvtScanningEventScanCommandFailedOpCode_c                               = 0x9B,                       /*! gapScanningCallback (type = gScanCommandFailed_c) event operation code */
    gBleGapEvtScanningEventDeviceScannedOpCode_c                                   = 0x9C,                       /*! gapScanningCallback (type = gDeviceScanned_c) event operation code */

    gBleGapEvtConnectionEventConnectedOpCode_c                                     = 0x9D,                       /*! gapConnectionCallback (type = gConnected_c) event operation code */
    gBleGapEvtConnectionEventPairingRequestOpCode_c                                = 0x9E,                       /*! gapConnectionCallback (type = gPairingRequest_c) event operation code */
    gBleGapEvtConnectionEventPeripheralSecurityRequestOpCode_c                     = 0x9F,                       /*! gapConnectionCallback (type = gPeripheralSecurityRequest_c) event operation code */
    gBleGapEvtConnectionEventPairingResponseOpCode_c                               = 0xA0,                       /*! gapConnectionCallback (type = gPairingResponse_c) event operation code */
    gBleGapEvtConnectionEventAuthenticationRejectedOpCode_c                        = 0xA1,                       /*! gapConnectionCallback (type = gAuthenticationRejected_c) event operation code */
    gBleGapEvtConnectionEventPasskeyRequestOpCode_c                                = 0xA2,                       /*! gapConnectionCallback (type = gPasskeyRequest_c) event operation code */
    gBleGapEvtConnectionEventOobRequestOpCode_c                                    = 0xA3,                       /*! gapConnectionCallback (type = gOobRequest_c) event operation code */
    gBleGapEvtConnectionEventPasskeyDisplayOpCode_c                                = 0xA4,                       /*! gapConnectionCallback (type = gPasskeyDisplay_c) event operation code */
    gBleGapEvtConnectionEventKeyExchangeRequestOpCode_c                            = 0xA5,                       /*! gapConnectionCallback (type = gKeyExchangeRequest_c) event operation code */
    gBleGapEvtConnectionEventKeysReceivedOpCode_c                                  = 0xA6,                       /*! gapConnectionCallback (type = gKeysReceived_c) event operation code */
    gBleGapEvtConnectionEventLongTermKeyRequestOpCode_c                            = 0xA7,                       /*! gapConnectionCallback (type = gLongTermKeyRequest_c) event operation code */
    gBleGapEvtConnectionEventEncryptionChangedOpCode_c                             = 0xA8,                       /*! gapConnectionCallback (type = gEncryptionChanged_c) event operation code */
    gBleGapEvtConnectionEventPairingCompleteOpCode_c                               = 0xA9,                       /*! gapConnectionCallback (type = gPairingComplete_c) event operation code */
    gBleGapEvtConnectionEventDisconnectedOpCode_c                                  = 0xAA,                       /*! gapConnectionCallback (type = gDisconnected_c) event operation code */
    gBleGapEvtConnectionEventRssiReadOpCode_c                                      = 0xAB,                       /*! gapConnectionCallback (type = gRssiRead_c) event operation code */
    gBleGapEvtConnectionEventTxPowerLevelReadOpCode_c                              = 0xAC,                       /*! gapConnectionCallback (type = gTxPowerLevelRead_c) event operation code */
    gBleGapEvtConnectionEventPowerReadFailureOpCode_c                              = 0xAD,                       /*! gapConnectionCallback (type = gPowerReadFailureOpCode_c) event operation code */
    gBleGapEvtConnectionEventParameterUpdateRequestOpCode_c                        = 0xAE,                       /*! gapConnectionCallback (type = gConnEvtParameterUpdateRequest_c) event operation code */
    gBleGapEvtConnectionEventParameterUpdateCompleteOpCode_c                       = 0xAF,                       /*! gapConnectionCallback (type = gConnEvtParameterUpdateComplete_c) event operation code */
    gBleGapEvtConnectionEventLeDataLengthChangedOpCode_c                           = 0xB0,                       /*! gapConnectionCallback (type = gConnEvtLeDataLengthChanged_c) event operation code */
    gBleGapEvtConnectionEventLeScOobDataRequestOpCode_c                            = 0xB1,                       /*! gapConnectionCallback (type = gConnEvtLeScOobDataRequest_c) event operation code */
    gBleGapEvtConnectionEventLeScDisplayNumericValueOpCode_c                       = 0xB2,                       /*! gapConnectionCallback (type = gConnEvtLeScDisplayNumericValue_c) event operation code */
    gBleGapEvtConnectionEventLeScKeypressNotificationOpCode_c                      = 0xB3,                       /*! gapConnectionCallback (type = gConnEvtLeScKeypressNotification_c) event operation code */

    gBleGapEvtConnectionEventLeSetDataLengthFailedOpCode_c                         = 0xB4,
    gBleGapEvtGenericEventLeScPublicKeyRegeneratedOpCode_c                         = 0xB5,                       /*! gapGenericCallback (type = gLeScPublicKeyRegenerated_c) event operation code */
    gBleGapEvtGenericEventLeScLocalOobDataOpCode_c                                 = 0xB6,                       /*! gapGenericCallback (type = gLeScLocalOobData_c) event operation code */
    gBleGapEvtGenericEventHostPrivacyStateChangedOpCode_c                          = 0xB7,                       /*! gapGenericCallback (type = gHostPrivacyStateChanged_c) event operation code */
    gBleGapEvtGenericEventControllerPrivacyStateChangedOpCode_c                    = 0xB8,                       /*! gapGenericCallback (type = gControllerPrivacyStateChanged_c) event operation code */
    gBleGapEvtGenericEventTxPowerLevelSetCompleteOpCode_c                          = 0xB9,                       /*! gapGenericCallback (type = gTxPowerLevelSetComplete_c) event operation code */
    gBleGapEvtGenericEventLePhyEventOpCode_c                                       = 0xBA,                       /*! gapGenericCallback (type = gLePhyEvent_c) event operation code */

    gBleGapEvtGetBondedDevicesIdentityInformationOpCode_c                          = 0xBB,                       /*! Gap_GetBondedDevicesIdentityInformation command out parameters event operation code */
    gBleGapEvtGenericEventControllerNotificationOpCode_c                           = 0xBC,                       /*! gapGenericCallback (type = gControllerNotificationEvent_c) event operation code */
    gBleGapEvtGenericEventBondCreatedOpCode_c                                      = 0xBD,                       /*! gapGenericCallback (type = gBondCreatedEvent_c) event operation code */
    gBleGapEvtGenericEventChannelMapSetOpCode_c                                    = 0xBE,                       /*! gapGenericCallback (type = gChannelMapSet_c) event operation code */

    gBleGapEvtConnectionEventChannelMapReadOpCode_c                                = 0xBF,                       /*! gapConnectionCallback (type = gConnEvtChannelMapRead_c) event operation code */
    gBleGapEvtConnectionEventChannelMapReadFailureOpCode_c                         = 0xC0,                       /*! gapConnectionCallback (type = gConnEvtChannelMapReadFailure_c) event operation code */

    /* BLE 5.0: Advertising extensions */
    gBleGapEvtGenericEventExtAdvertisingParametersSetupCompleteOpCode_c            = 0xC1,                       /*! gapGenericCallback (type = gExtAdvertisingParametersSetupComplete_c) event operation code */
    gBleGapEvtGenericEventExtAdvertisingDataSetupCompleteOpCode_c                  = 0xC2,                       /*! gapGenericCallback (type = gExtAdvertisingDataSetupComplete_c) event operation code */
    gBleGapEvtGenericEventPeriodicAdvParamSetupCompleteOpCode_c                    = 0xC3,                       /*! gapGenericCallback (type = gPeriodicAdvParamSetupComplete_c) event operation code */
    gBleGapEvtGenericEventPeriodicAdvDataSetupCompleteOpCode_c                     = 0xC4,                       /*! gapGenericCallback (type = gPeriodicAdvDataSetupComplete_c) event operation code */
    gBleGapEvtGenericEventPeriodicAdvListUpdateCompleteOpCode_c                    = 0xC5,                       /*! gapGenericCallback (type = gPeriodicAdvListUpdateComplete_c) event operation code */

    gBleGapEvtAdvertisingEventExtAdvertisingStateChangedOpCode_c                   = 0xC6,                       /*! gapAdvertisingCallback (type = gExtAdvertisingStateChanged_c) event operation code */
    gBleGapEvtAdvertisingEventAdvertisingSetTerminatedOpCode_c                     = 0xC7,                       /*! gapAdvertisingCallback (type = gAdvertisingSetTerminated_c) event operation code */
    gBleGapEvtAdvertisingEventExtAdvertisingSetRemovedOpCode_c                     = 0xC8,                       /*! gapAdvertisingCallback (type = gExtAdvertisingSetRemoveComplete_c) event operation code */
    gBleGapEvtAdvertisingEventExtScanReqReceivedOpCode_c                           = 0xC9,                       /*! gapAdvertisingCallback (type = gExtScanNotification_c) event operation code */
    gBleGapEvtGenericEventPeriodicAdvertisingStateChangedOpCode_c                  = 0xCA,                       /*! gapGenericCallback  (type = gPeriodicAdvertisingStateChanged_c) event operation code */

    gBleGapEvtScanningEventExtDeviceScannedOpCode_c                                = 0xCB,                       /*! gapScanningCallback (type = gExtDeviceScanned_c) event operation code */
    gBleGapEvtScanningEventPeriodicAdvSyncEstablishedOpCode_c                      = 0xCC,                       /*! gapScanningCallback (type = gPeriodicAdvSyncEstablished_c) event operation code */
    gBleGapEvtScanningEventPeriodicAdvSyncTerminatedOpCode_c                       = 0xCD,                       /*! gapScanningCallback (type = gPeriodicAdvSyncTerminated_c) event operation code */
    gBleGapEvtScanningEventPeriodicAdvSyncLostOpCode_c                             = 0xCE,                       /*! gapScanningCallback (type = gPeriodicAdvSyncLost_c) event operation code */
    gBleGapEvtScanningEventPeriodicDeviceScannedOpCode_c                           = 0xCF,                       /*! gapScanningCallback (type = gPeriodicDeviceScanned_c) event operation code */
    gBleGapEvtGenericEventPeriodicAdvCreateSyncCancelledOpCode_c                   = 0xD0,                       /*! gapGenericCallback (type = gPeriodicAdvCreateSyncCancelled_c) event operation code */
    gBleGapEvtConnectionEventChannelSelectionAlgorithm2OpCode_c                    = 0xD1,                       /*! gapConnectionCallback (type = gConnEvtChanSelectionAlgorithm2_c) event operation code */
    gBleGapEvtGenericEventTxEntryAvailableOpCode_c                                 = 0xD2,                       /*! gapGenericCallback (type = gTxEntryAvailable_c) event operation code */
    
    gBleGapEvtGenericEventControllerLocalRPAReadOpCode_c                           = 0xD3,                       /*! gapGenericCallback (type = gLocalPrivateAddressRead_c) event operation code */

    gBleGapEvtCheckNvmIndexOpCode_c                                                = 0xD4,                       /*! Gap_CheckNvmIndex command out parameters event operation code */
    gBleGapEvtGetDeviceIdFromConnHandleOpCode_c                                    = 0xD5,                       /*! Gap_GetDeviceIdFromConnHandle command operation code */
    gBleGapEvtGetConnectionHandleFromDeviceIdOpCode_c                              = 0xD6,                       /*! Gap_GetConnectionHandleFromDeviceId command operation code */
    gBleGapEvtPairingEventNoLtkOpCode_c                                            = 0xD7,                       /*! gapConnectionCallback (type = gConnEvtPairingNoLtk_c ) event operation code */
    gBleGapEvtPairingAlreadyStartedOpCode_c                                        = 0xD8,                       /*! gapConnectionCallback (type = gConnEvtPairingAlreadyStarted_c  ) event operation code */

    /* BLE 5.1: AoA/AoD */
    gBleGapEvtGenericEventConnectionlessCteTransmitParamsSetupCompleteOpCode_c     = 0xD9,                       /*! gapGenericCallback (type = gConnectionlessCteTransmitParamsSetupComplete_c) event operation code */
    gBleGapEvtGenericEventConnectionlessCteTransmitStateChangedOpCode_c            = 0xDA,                       /*! gapGenericCallback (type = gConnectionlessCteTransmitStateChanged_c) event operation code */
    gBleGapEvtGenericEventConnectionlessIqSamplingStateChangedOpCode_c             = 0xDB,                       /*! gapGenericCallback (type = gConnectionlessIqSamplingStateChanged_c) event operation code */
    gBleGapEvtGenericEventAntennaInformationReadOpCode_c                           = 0xDC,                       /*! gapGenericCallback (type = gAntennaInformationRead_c) event operation code */
    gBleGapEvtScanningEventConnectionlessIqReportReceivedOpCode_c                  = 0xDD,                       /*! gapScanningCallback (type = gConnectionlessIqReportReceived_c) event operation code */
    gBleGapEvtConnectionEventIqReportReceivedOpCode_c                              = 0xDE,                       /*! gapConnectionCallback (type = gConnEvtIqReportReceived_c) event operation code */
    gBleGapEvtConnectionEventCteRequestFailedOpCode_c                              = 0xDF,                       /*! gapConnectionCallback (type = gConnEvtCteRequestFailed_c) event operation code */
    gBleGapEvtConnectionEventCteReceiveParamsSetupCompleteOpCode_c                 = 0xE0,                       /*! gapConnectionCallback (type = gConnEvtCteReceiveParamsSetupComplete_c) event operation code */
    gBleGapEvtConnectionEventCteTransmitParamsSetupCompleteOpCode_c                = 0xE1,                       /*! gapConnectionCallback (type = gConnEvtCteTransmitParamsSetupComplete_c) event operation code */
    gBleGapEvtConnectionEventCteReqStateChangedOpCode_c                            = 0xE2,                       /*! gapConnectionCallback (type = gConnEvtCteReqStateChanged_c) event operation code */
    gBleGapEvtConnectionEventCteRspStateChangedOpCode_c                            = 0xE3,                       /*! gapConnectionCallback (type = gConnEvtCteRspStateChanged_c) event operation code */

    /* Periodic Advertisement Sync Transfer */
    gBleGapEvtGenericEventPeriodicAdvRecvEnableCompleteOpCode_c                    = 0xE4,                       /*! gapGenericCallback (type = gPeriodicAdvRecvEnableComplete_c) event operation code */
    gBleGapEvtGenericEventPeriodicAdvSyncTransferCompleteOpCode_c                  = 0xE5,                       /*! gapGenericCallback (type = gPeriodicAdvSyncTransferComplete_c) event operation code */
    gBleGapEvtGenericEventPeriodicAdvSetInfoTransferCompleteOpCode_c               = 0xE6,                       /*! gapGenericCallback (type = gPeriodicAdvSetInfoTransferComplete_c) event operation code */
    gBleGapEvtGenericEventSetPeriodicAdvSyncTransferParamsCompleteOpCode_c         = 0xE7,                       /*! gapGenericCallback (type = gSetPeriodicAdvSyncTransferParamsComplete_c) event operation code */
    gBleGapEvtGenericEventSetDefaultPeriodicAdvSyncTransferParamsCompleteOpCode_c  = 0xE8,                       /*! gapGenericCallback (type = gSetDefaultPeriodicAdvSyncTransferParamsComplete_c) event operation code */
    gBleGapEvtScanningEventPeriodicAdvSyncTransferReceivedOpCode_c                 = 0xE9,                       /*! gapScanningCallback (type = gPeriodicAdvSyncTransferSucceeded_c) event operation code */

    /* BLE 5.2 */
    gBleGapEvtConnectionEventPathLossThreshold_c                                   = 0xEA,                       /*! gapConnectionCallback (type = gConnEvtPathLossThreshold_c) event operation code */
    gBleGapEvtConnectionEventTransmitPowerReporting_c                              = 0xEB,                       /*! gapConnectionCallback (type = gConnEvtTransmitPowerReporting_c) event operation code */
    gBleGapEvtConnectionEventEnhancedReadTransmitPowerLevel_c                      = 0xEC,                       /*! gapConnectionCallback (type = gConnEvtEnhancedReadTransmitPowerLevel_c) event operation code */
    gBleGapEvtConnectionEventPathLossReportingParamsSetupComplete_c                = 0xED,                       /*! gapConnectionCallback (type = gConnEvtPathLossReportingParamsSetupComplete_c) event operation code */
    gBleGapEvtConnectionEventPathLossReportingStateChanged_c                       = 0xEE,                       /*! gapConnectionCallback (type = gConnEvtPathLossReportingStateChanged_c) event operation code */
    gBleGapEvtConnectionEventTransmitPowerReportingStateChanged_c                  = 0xEF,                       /*! gapConnectionCallback (type = gConnEvtTransmitPowerReportingStateChanged_c) event operation code */

    /* EATT */
    gBleGapEvtConnectionEventEattConnectionRequest_c                               = 0xF0,                       /*! gapConnectionCallback (type = gConnEvtEattConnectionRequest_c) event operation code */
    gBleGapEvtConnectionEventEattConnectionComplete_c                              = 0xF1,                       /*! gapConnectionCallback (type = gConnEvtEattConnectionComplete_c) event operation code */
    gBleGapEvtConnectionEventEattReconfigureResponse_c                             = 0xF2,                       /*! gapConnectionCallback (type = gConnEvtEattChannelReconfigureResponse_c) event operation code */
    gBleGapEvtConnectionEventEattBearerStatusNotification_c                        = 0xF3,                       /*! gapConnectionCallback (type = gConnEvtEattBearerStatusNotification_c) event operation code */
    gBleGapEvtGenericEventLeGenerateDhKeyComplete_c                                = 0xF4,                       /*! gapGenericCallback (type = gConnEvtLeGenerateDhKeyComplete_c) event operation code */

    gBleGapEvtGetHostVersionOpCode_c                                               = 0xF5,                       /*! Gap_GetHostVersion command out parameters event operation code */
    gBleGapEvtGenericEventReadRemoteVersionInfoComplete_c                          = 0xF6,                       /*! gapGenericCallback (type = gRemoteVersionInformationRead_c) event operation code */
    gBleGapEvtGenericEventGetConnParamsComplete_c                                  = 0xF7,                       /*! gapGenericCallback (type = gGetConnParamsComplete_c) event operation code */
    
    gBleGapEvtPlatformError_c                                                      = 0xF8,                       /*! platform error callback event operation code */
}fsciBleGapOpCode_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*! *********************************************************************************
* \brief  Calls the GAP function associated with the operation code received over UART.
*         The GAP function parameters are extracted from the received FSCI payload.
*
* \param[in]    pData               Packet (containing FSCI header and FSCI
*                                   payload) received over UART
* \param[in]    param               Pointer given when this function is registered in
*                                   FSCI
* \param[in]    fsciInterfaceId     FSCI interface on which the packet was received
*
********************************************************************************** */
void fsciBleGapHandler
(
    void* pData,
    void* param,
    uint32_t fsciInterfaceId
);

#if gFsciBleBBox_d || gFsciBleTest_d
/*! *********************************************************************************
* \brief  Creates the FSCI packet which contains the status of the last executed GAP
*         command and sends it over UART.
*
* \param[in]    result      Status of the last executed GAP command.
*
********************************************************************************** */
void fsciBleGapStatusMonitor
(
    bleResult_t result
);
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d || gFsciBleTest_d
/*! *********************************************************************************
* \brief  Creates a GAP FSCI packet without payload and sends it over UART.
*
* \param[in]    opCode      GAP command operation code.
*
********************************************************************************** */
void fsciBleGapNoParamCmdMonitor
(
    fsciBleGapOpCode_t opCode
);

/*! *********************************************************************************
* \brief  Creates a GAP FSCI packet with one byte of payload and sends it over UART.
*
* \param[in]    opCode      GAP command operation code.
* \param[in]    param       One byte parameter.
*
********************************************************************************** */
void fsciBleGapUint8ParamCmdMonitor
(
    fsciBleGapOpCode_t  opCode,
    uint8_t             param
);

/*! *********************************************************************************
* \brief  Creates a GAP FSCI packet with deviceId as payload and sends it over UART.
*
* \param[in]    opCode      GAP command operation code.
* \param[in]    deviceId    Device identifier of the peer.
*
********************************************************************************** */
void fsciBleGapDeviceIdParamCmdMonitor
(
    fsciBleGapOpCode_t  opCode,
    deviceId_t          deviceId
);

/*! *********************************************************************************
* \brief  Gap_CheckNotificationStatus and Gap_CheckIndicationsStatus commands
*         monitoring function.
*
* \param[in]    opCode          GAP command operation code.
* \param[in]    deviceId        The peer GATT Client.
* \param[in]    handle          The handle of the CCCD.
*
********************************************************************************** */
void fsciBleGapCheckNotificationsAndIndicationsCmdMonitor
(
    fsciBleGapOpCode_t  opCode,
    deviceId_t          deviceId,
    uint16_t            handle,
    bool_t*             pOutIsActive
);

/*! *********************************************************************************
* \brief  Gap_AddDeviceToFilterAcceptList and Gap_RemoveDeviceFromFilterAcceptList commands
*         monitoring function.
*
* \param[in]    opCode          GAP command operation code.
* \param[in]    address         The address of the device.
* \param[in]    addressType     The device address type (public or random).
*
********************************************************************************** */
void fsciBleGapAddressParamsCmdMonitor
(
    fsciBleGapOpCode_t  opCode,
    bleAddressType_t    addressType,
    const bleDeviceAddress_t  address
);

/*! *********************************************************************************
* \brief  Ble_HostInitialize command monitoring function.
*
* \param[in]    genericCallback             Callback used to propagate GAP controller
                                            events to the application.
* \param[in]    hostToControllerInterface   LE Controller uplink interface function pointer
*
********************************************************************************** */
void fsciBleGapHostInitializeCmdMonitor
(
    gapGenericCallback_t            genericCallback,
    hciHostToControllerInterface_t  hostToControllerInterface
);

/*! *********************************************************************************
* \brief  Gap_RegisterDeviceSecurityRequirements command monitoring function.
*
* \param[in]    pSecurity       Pointer to the application-allocated
*                               gapDeviceSecurityRequirements_t structure.
*
********************************************************************************** */
void fsciBleGapRegisterDeviceSecurityRequirementsCmdMonitor
(
    const gapDeviceSecurityRequirements_t* pSecurity
);

/*! *********************************************************************************
* \brief  Gap_SetAdvertisingParameters command monitoring function.
*
* \param[in]    pAdvertisingParameters      Pointer to gapAdvertisingParameters_t
*                                           structure.
*
********************************************************************************** */
void fsciBleGapSetAdvertisingParametersCmdMonitor
(
    const gapAdvertisingParameters_t* pAdvertisingParameters
);

/*! *********************************************************************************
* \brief  Gap_SetExtAdvertisingParameters command monitoring function.
*
* \param[in]    pAdvertisingParameters      Pointer to gapExtAdvertisingParameters_t
*                                           structure.
*
********************************************************************************** */
void fsciBleGapSetExtAdvertisingParametersCmdMonitor
(
    gapExtAdvertisingParameters_t* pAdvertisingParameters
);

/*! *********************************************************************************
* \brief  Gap_SetExtAdvertisingData command monitoring function.
*
* \param[in]    handle                      The advertising set identifier
* \param[in]    pStaticAdvertisingData      Pointer to gapAdvertisingData_t structure
*                                           or NULL.
* \param[in]    pStaticScanResponseData     Pointer to gapScanResponseData_t structure
*                                           or NULL.
*
********************************************************************************** */
void fsciBleGapSetExtAdvertisingDataCmdMonitor
(
    uint8_t handle,
    gapAdvertisingData_t*   pAdvertisingData,
    gapScanResponseData_t*  pScanResponseData
);

/*! *********************************************************************************
* \brief  Gap_SetAdvertisingData command monitoring function.
*
* \param[in]    pStaticAdvertisingData      Pointer to gapAdvertisingData_t structure
*                                           or NULL.
* \param[in]    pStaticScanResponseData     Pointer to gapScanResponseData_t structure
*                                           or NULL.
*
********************************************************************************** */
void fsciBleGapSetAdvertisingDataCmdMonitor
(
    gapAdvertisingData_t*   pAdvertisingData,
    gapScanResponseData_t*  pScanResponseData
);

/*! *********************************************************************************
* \brief  Gap_StartAdvertising command monitoring function.
*
* \param[in]    advertisingCallback     Callback used by the application to receive
                                        advertising events.
* \param[in]    connectionCallback      Callback used by the application to receive
                                        connection events.
*
********************************************************************************** */
void fsciBleGapStartAdvertisingCmdMonitor
(
    gapAdvertisingCallback_t    advertisingCallback,
    gapConnectionCallback_t     connectionCallback
);

#if defined(gBLE50_d) && (gBLE50_d == 1U)
/*! *********************************************************************************
* \brief  Gap_StartExtAdvertising command monitoring function.
*
* \param[in]    advertisingCallback     Callback used by the application to receive
                                        advertising events.
* \param[in]    connectionCallback      Callback used by the application to receive
                                        connection events.
* \param[in]    handle                  The ID of the advertising set
* \param[in]    duration                The duration of the advertising
* \param[in]    maxExtAdvEvents         The maximum number of advertising events
*
********************************************************************************** */
void fsciBleGapStartExtAdvertisingCmdMonitor
(
    gapAdvertisingCallback_t advertisingCallback,
    gapConnectionCallback_t connectionCallback,
    uint8_t handle,
    uint16_t duration,
    uint8_t maxExtAdvEvents
);

/*! *********************************************************************************
* \brief  Gap_UpdatePeriodicAdvList command monitoring function.
*
* \param[in]    operation     operation to be executed on the list
* \param[in]    addrType      the address type of the device
* \param[in]    pAddr         address of the device
* \param[in]    SID           id of the advertising set used to identify the periodic advertising
*
********************************************************************************** */
void fsciBleGapUpdatePeriodicAdvListCmdMonitor
(
    gapPeriodicAdvListOperation_t operation,
    bleAddressType_t addrType,
    uint8_t * pAddr,
    uint8_t SID
);

/*! *********************************************************************************
* \brief  Gap_SetPeriodicAdvertisingData command monitoring function.
*
* \param[in]    handle                      Identifier of the Periodic Advertising set
* \param[in]    pStaticAdvertisingData      Pointer to gapAdvertisingData_t structure
*
********************************************************************************** */
void fsciBleGapSetPeriodicAdvertisingDataCmdMonitor
(
    uint8_t handle,
    gapAdvertisingData_t*   pAdvertisingData
);

/*! *********************************************************************************
* \brief  Gap_PeriodicAdvCreateSync command monitoring function.
*
* \param[in]    pReq                    Pointer to gapPeriodicAdvSyncReq_t structure
*                                       or NULL
*
********************************************************************************** */
void fsciBleGapPeriodicAdvCreateSyncCmdMonitor
(
    gapPeriodicAdvSyncReq_t * pReq
);
#endif
/*! *********************************************************************************
* \brief  Gap_Authorize command monitoring function.
*
* \param[in]    deviceId    The peer being authorized.
* \param[in]    handle      The attribute handle.
* \param[in]    access      The type of access granted (gAccessRead_c or gAccessWrite_c).
*
********************************************************************************** */
void fsciBleGapAuthorizeCmdMonitor
(
    deviceId_t          deviceId,
    uint16_t            handle,
    gattDbAccessType_t  access
);

/*! *********************************************************************************
* \brief  Gap_SaveCccd command monitoring function.
*
* \param[in]    deviceId    The peer GATT Client.
* \param[in]    handle      The handle of the CCCD as defined in the GATT Database.
* \param[in]    cccd        The bit mask representing the CCCD value to be saved.
*
********************************************************************************** */
void fsciBleGapSaveCccdCmdMonitor
(
    deviceId_t      deviceId,
    uint16_t        handle,
    gattCccdFlags_t cccd
);

/*! *********************************************************************************
* \brief  Gap_GetBondedDevicesIdentityInformation command monitoring function.
*
* \param[in]    aOutIdentityAddresses     Array of addresses to be filled.
* \param[in]    maxDevices                Maximum number of addresses to be obtained.
* \param[in]    pOutActualCount           The actual number of addresses written.
*
********************************************************************************** */
void fsciBleGapGetBondedDevicesIdentityInformationCmdMonitor
(
    const gapIdentityInformation_t*   aOutIdentityAddresses,
    uint8_t                           maxDevices,
    const uint8_t*                    pOutActualCount
);

/*! *********************************************************************************
* \brief  Gap_Pair command monitoring function.
*
* \param[in]    deviceId            The peer to pair with.
* \param[in]    pPairingParameters  Pairing parameters as required by the SMP.
*
********************************************************************************** */
void fsciBleGapPairCmdMonitor
(
    deviceId_t                    deviceId,
    const gapPairingParameters_t  *pPairingParameters
);

/*! *********************************************************************************
* \brief  Gap_SendPeripheralSecurityRequest command monitoring function.
*
* \param[in]    deviceId            The GAP peer to pair with.
* \param[in]    pPairingParameters  Pairing parameters as required by the SMP.
*
********************************************************************************** */
void fsciBleGapSendPeripheralSecurityRequestCmdMonitor
(
    deviceId_t                    deviceId,
    const gapPairingParameters_t  *pPairingParameters
);

/*! *********************************************************************************
* \brief  Gap_AcceptPairingRequest command monitoring function.
*
* \param[in]    deviceId            The peer requesting authentication.
* \param[in]    pPairingParameters  Pairing parameters as required by the SMP.
*
********************************************************************************** */
void fsciBleGapAcceptPairingRequestCmdMonitor
(
    deviceId_t                    deviceId,
    const gapPairingParameters_t  *pPairingParameters
);

/*! *********************************************************************************
* \brief  Gap_RejectAuthentication command monitoring function.
*
* \param[in]    deviceId    The GAP peer who requested authentication.
* \param[in]    reason      The reason of rejection.
*
********************************************************************************** */
void fsciBleGapRejectPairingCmdMonitor
(
    deviceId_t                          deviceId,
    gapAuthenticationRejectReason_t     reason
);

/*! *********************************************************************************
* \brief  Gap_EnterPasskey command monitoring function.
*
* \param[in]    deviceId    The GAP peer that requested a passkey entry.
* \param[in]    passkey     The peer's secret passkey.
*
********************************************************************************** */
void fsciBleGapEnterPasskeyCmdMonitor
(
    deviceId_t  deviceId,
    uint32_t    passkey
);

/*! *********************************************************************************
* \brief  Gap_ProvideOob command monitoring function.
*
* \param[in]    deviceId    The pairing device.
* \param[in]    aOob        Pointer to OOB data (array of gcSmpOobSize_d size).
*
********************************************************************************** */
void fsciBleGapProvideOobCmdMonitor
(
    deviceId_t  deviceId,
    const uint8_t*    aOob
);

/*! *********************************************************************************
* \brief  Gap_SendSmpKeys command monitoring function.
*
* \param[in]    deviceId    The GAP peer who initiated the procedure.
* \param[in]    pKeys       The SMP keys of the local device.
*
********************************************************************************** */
void fsciBleGapSendSmpKeysCmdMonitor
(
    deviceId_t      deviceId,
    const gapSmpKeys_t*   pKeys
);

/*! *********************************************************************************
* \brief  Gap_ProvideLongTermKey command monitoring function.
*
* \param[in]    deviceId    The GAP peer who requested encryption.
* \param[in]    aLtk        The Long Term Key.
* \param[in]    ltkSize     The Long Term Key size.
*
********************************************************************************** */
void fsciBleGapProvideLongTermKeyCmdMonitor
(
    deviceId_t  deviceId,
    const uint8_t*    aLtk,
    uint8_t     ltkSize
);

/*! *********************************************************************************
* \brief  Gap_LoadEncryptionInformation command monitoring function.
*
* \param[in]    deviceId        Device ID of the peer.
* \param[in]    aOutLtk         Array of size gcMaxLtkSize_d to be filled with the LTK.
* \param[in]    pOutLtkSize     The LTK size.
*
********************************************************************************** */
void fsciBleGapLoadEncryptionInformationCmdMonitor
(
    deviceId_t  deviceId,
    const uint8_t*    aOutLtk,
    const uint8_t*    pOutLtkSize
);

/*! *********************************************************************************
* \brief  Gap_SetLocalPasskey command monitoring function.
*
* \param[in]    passkey     The SMP passkey.
*
********************************************************************************** */
void fsciBleGapSetLocalPasskeyCmdMonitor
(
    uint32_t passkey
);

/*! *********************************************************************************
* \brief  Gap_StartScanning command monitoring function.
*
* \param[in]    pScanningParameters     The scanning parameters, may be NULL.
* \param[in]    scanningCallback        The scanning callback.
* \param[in]    enableFilterDuplicates  Enable or disable duplicate advertising reports
*                                       filtering
*
********************************************************************************** */
void fsciBleGapStartScanningCmdMonitor
(
    const gapScanningParameters_t*    pScanningParameters,
    gapScanningCallback_t       scanningCallback,
    gapFilterDuplicates_t       enableFilterDuplicates,
    uint16_t                    duration,
    uint16_t                    period
);

/*! *********************************************************************************
* \brief  Gap_Connect command monitoring function.
*
* \param[in]    pParameters     Create Connection command parameters.
* \param[in]    connCallback    Callback used to receive connection events.
*
********************************************************************************** */
void fsciBleGapConnectCmdMonitor
(
    const gapConnectionRequestParameters_t*   pParameters,
    gapConnectionCallback_t             connCallback
);

/*! *********************************************************************************
* \brief  Gap_SaveCustomPeerInformation command monitoring function.
*
* \param[in]    deviceId    Device ID of the GAP peer.
* \param[in]    aInfo       Pointer to the beginning of the data.
* \param[in]    offset      Offset from the beginning of the reserved memory area.
* \param[in]    infoSize    Data size (maximum equal to gcReservedFlashSizeForCustomInformation_d).
*
********************************************************************************** */
void fsciBleGapSaveCustomPeerInformationCmdMonitor
(
    deviceId_t  deviceId,
    const uint8_t*    aInfo,
    uint16_t    offset,
    uint16_t    infoSize
);

/*! *********************************************************************************
* \brief  Gap_LoadCustomPeerInformation command monitoring function.
*
* \param[in]    deviceId    Device ID of the GAP peer.
* \param[in]    aOutInfo    Pointer to the beginning of the allocated memory.
* \param[in]    offset      Offset from the beginning of the reserved memory area.
* \param[in]    infoSize    Data size (maximum equal to gcReservedFlashSizeForCustomInformation_d).
*
********************************************************************************** */
void fsciBleGapLoadCustomPeerInformationCmdMonitor
(
    deviceId_t  deviceId,
    const uint8_t*    aOutInfo,
    uint16_t    offset,
    uint16_t    infoSize
);

/*! *********************************************************************************
* \brief  Gap_CheckIfBonded command monitoring function.
*
* \param[in]    deviceId        Device ID of the GAP peer.
* \param[in]    pOutIsBonded    Boolean to be filled with the bonded flag.
*
********************************************************************************** */
void fsciBleGapCheckIfBondedCmdMonitor
(
    deviceId_t  deviceId,
    bool_t*     pOutIsBonded
);

/*! *********************************************************************************
* \brief  Gap_CheckNvmIndex command monitoring function.
*
* \param[in]    nvmIndex      NVM index to be checked.
* \param[in]    pOutIsFree    Boolean to be filled with index status (TRUE if free).
*
********************************************************************************** */
void fsciBleGapCheckNvmIndexCmdMonitor
(
    uint8_t  nvmIndex,
    bool_t*  pOutIsFree
);

/*! *********************************************************************************
* \brief  Gap_GetConnectionHandleFromDeviceId command monitoring macro.
*
* \param[in]    deviceId    The peer identifier.
*
********************************************************************************** */
void fsciBleGapGetConnectionHandleFromDeviceIdCmdMonitor
(
    deviceId_t deviceId
);

/*! *********************************************************************************
* \brief  Gap_GetDeviceIdFromConnHandle command monitoring macro.
*
* \param[in]    connHandle  Corresponding connection handle.
*
********************************************************************************** */
void fsciBleGapGetDeviceIdFromConnHandleCmdMonitor
(
    uint16_t    connHandle
);

/*! *********************************************************************************
* \brief  Gap_CreateRandomDeviceAddress command monitoring function.
*
* \param[in]    aIrk            The Identity Resolving Key to be used for a private
*                               resolvable address or NULL for a private non-resolvable
*                               address (fully random).
* \param[in]    aRandomPart     If aIrk is not NULL, this is a 3-byte array containing
*                               the Random Part of a Private Resolvable Address, in LSB
*                               to MSB order; the most significant two bits of the most
*                               significant byte (aRandomPart[3] & 0xC0) are ignored.
*                               This may be NULL, in which case the Random Part is
*                               randomly generated internally.
*
********************************************************************************** */
void fsciBleGapCreateRandomDeviceAddressCmdMonitor
(
    const uint8_t*    aIrk,
    const uint8_t*    aRandomPart
);

/*! *********************************************************************************
* \brief  Gap_SaveDeviceName command monitoring function.
*
* \param[in]    deviceId    Device ID for the active peer which name is saved.
* \param[in]    aName       Array of characters holding the name.
* \param[in]    cNameSize   Number of characters to be saved.
*
********************************************************************************** */
void fsciBleGapSaveDeviceNameCmdMonitor
(
    deviceId_t     deviceId,
    const uchar_t* aName,
    uint8_t        cNameSize
);

/*! *********************************************************************************
* \brief  Gap_GetBondedDevicesCount command monitoring function.
*
* \param[in]    pOutBondedDevicesCount  Pointer to integer to be written.
*
********************************************************************************** */
void fsciBleGapGetBondedDevicesCountCmdMonitor
(
    const uint8_t* pOutBondedDevicesCount
);

/*! *********************************************************************************
* \brief  Gap_GetBondedDeviceName command monitoring function.
*
* \param[in]    nvmIndex        Index of the device in NVM bonding area.
* \param[in]    aOutName        Destination array to copy the name into.
* \param[in]    maxNameSize     Maximum number of characters to be copied,
*
********************************************************************************** */
void fsciBleGapGetBondedDeviceNameCmdMonitor
(
    uint8_t     nvmIndex,
    uchar_t*    aOutName,
    uint8_t     maxNameSize
);

/*! *********************************************************************************
* \brief  Gap_ReadRadioPowerLevel command monitoring function.
*
* \param[in]    txReadType      Radio power level read type.
* \param[in]    deviceId        Device identifier.
*
********************************************************************************** */
void fsciBleGapReadRadioPowerLevelCmdMonitor
(
    gapRadioPowerLevelReadType_t    txReadType,
    deviceId_t                      deviceId
);

/*! *********************************************************************************
* \brief  Gap_UpdateLeDataLength command monitoring function.
*
* \param[in]    txReadType      Radio power level read type.
* \param[in]    deviceId        Device identifier.
*
********************************************************************************** */
void fsciBleGapUpdateLeDataLengthCmdMonitor
(
    deviceId_t  deviceId,
    uint16_t    txOctets,
    uint16_t    txTime
);

/*! *********************************************************************************
* \brief  Gap_EnableHostPrivacy command monitoring function.
*
* \param[in]    enable      TRUE to enable, FALSE to disable.
* \param[in]    aIrk        Local IRK to be used for Resolvable Private Address generation
*                           or NULL for Non-Resolvable Private Address generation. Ignored if enable is FALSE.
*
********************************************************************************** */
void fsciBleGapEnableHostPrivacyCmdMonitor
(
    bool_t         enable,
    const uint8_t* aIrk
);

/*! *********************************************************************************
* \brief  Gap_EnableHostPrivacy command monitoring function.
*
* \param[in] enable             TRUE to enable, FALSE to disable.
* \param[in] aOwnIrk            Local IRK. Ignored if enable is FALSE, otherwise shall not be NULL.
* \param[in] peerIdCount        Size of aPeerIdentities array. Ignored if enable is FALSE.
* \param[in] aPeerIdentities    Array of peer identity addresses and IRKs. Ignored if enable is FALSE,
*                               otherwise shall not be NULL.
*
********************************************************************************** */
void fsciBleGapEnableControllerPrivacyCmdMonitor
(
    bool_t                      enable,
    const uint8_t*              aOwnIrk,
    uint8_t                     peerIdCount,
    const gapIdentityInformation_t*   aPeerIdentities
);

/*! *********************************************************************************
* \brief  Gap_SetPrivacyMode command monitoring function.
*
* \param[in] nvmIndex           Index of the device in the NVM bonding area.
* \param[in] privacyMode        Controller privacy mode: Network or Device
*
********************************************************************************** */
void fsciBleGapSetPrivacyModeCmdMonitor
(
    uint8_t nvmIndex,
    blePrivacyMode_t privacyMode
);

/*! *********************************************************************************
* \brief  Gap_LeScValidateNumericValue command monitoring function.
*
* \param[in] deviceId           The DeviceID for which the command is intended.
* \param[in] valid              TRUE if user has indicated that numeric values are matched, FALSE otherwise.
*
********************************************************************************** */
void fsciBleGapLeScValidateNumericValueCmdMonitor
(
    deviceId_t  deviceId,
    bool_t      valid
);

/*! *********************************************************************************
* \brief  Gap_LeScSetPeerOobData command monitoring function.
*
* \param[in] deviceId           Device ID of the peer.
* \param[in] pPeerOobData       OOB data received from the peer.
*
********************************************************************************** */
void fsciBleGapLeScSetPeerOobDataCmdMonitor
(
    deviceId_t          deviceId,
    const gapLeScOobData_t*   pPeerOobData
);

/*! *********************************************************************************
* \brief  Gap_LeScSendKeypressNotification command monitoring function.
*
* \param[in] deviceId               Device ID of the peer.
* \param[in] keypressNotification   Value of the Keypress Notification.
*
********************************************************************************** */
void fsciBleGapLeScSendKeypressNotificationCmdMonitor
(
    deviceId_t                deviceId,
    gapKeypressNotification_t keypressNotification
);

/*! *********************************************************************************
* \brief  Gap_SetTxPowerLevel command monitoring function.
*
* \param[in] powerLevel     Power level to set.
* \param[in] channelType    Type of channel for which the power level is set,
*                           advertising channel or connection channel.
*
********************************************************************************** */
void fsciBleGapSetTxPowerLevelCmdMonitor
(
    uint8_t                         powerLevel,
    bleTransmitPowerChannelType_t   channelType
);

/*! *********************************************************************************
* \brief  Gap_LeReadPhy command monitoring function.
*
* \param deviceId  Device ID of the peer.
*
********************************************************************************** */
void fsciBleGapLeReadPhyCmdMonitor
(
    deviceId_t deviceId
);

/*! *********************************************************************************
* \brief  Gap_LeSetPhy command monitoring function.
*
* \param defaultMode   Use the provided values for all subsequent connections
* \param deviceId      Device ID of the peer
*                      Ignored if defaultMode is TRUE.
* \param allPhys       Host preferences on Tx and Rx Phy, as defined by gapLeAllPhyFlags_t
* \param txPhys        Host preferences on Tx Phy, as defined by gapLePhyFlags_t
* \param rxPhys        Host preferences on Rx Phy, as defined by gapLePhyFlags_t
* \param phyOptions    Host preferences on Coded Phy, as defined by gapLePhyOptionsFlags_t
*                      Ignored if defaultMode is TRUE.
*
********************************************************************************** */
void fsciBleGapLeSetPhyCmdMonitor
(
    bool_t defaultMode,
    deviceId_t deviceId,
    uint8_t allPhys,
    uint8_t txPhys,
    uint8_t rxPhys,
    uint16_t phyOptions
);

/*! *********************************************************************************
* \brief  Gap_ControllerEnhancedNotification command monitoring function.
*
* \param eventType     Event Type selection
* \param deviceId      Device ID of the peer
*
********************************************************************************** */
void fsciBleGapControllerEnhancedNotificationCmdMonitor
(
    uint16_t   eventType,
    deviceId_t deviceId
);

/*! *********************************************************************************
* \brief  Gap_LoadKeys command monitoring function.
*
* \param[in]     nvmIndex    Index to save the keys.
* \param[out]    pOutKeys    The bond keys.
* \param[out]    pOutKeyFlags Key flags.
* \param[out]    leSc        LE SC connections was used.
* \param[out]    auth        Peer was authenticated.
*
********************************************************************************** */
void fsciBleGapLoadKeysCmdMonitor
(
    uint8_t           nvmIndex,
    gapSmpKeys_t*     pOutKeys,
    gapSmpKeyFlags_t* pOutKeyFlags,
    bool_t*           pOutLeSc,
    bool_t*           pOutAuth
);

/*! *********************************************************************************
* \brief  Gap_SaveKeys command monitoring function.
*
* \param[in]    nvmIndex    Index to save the keys.
* \param[in]    pKeys       The bond keys.
* \param[in]    leSc        LE SC connections was used.
* \param[in]    auth        Peer was authenticated.
*
********************************************************************************** */
void fsciBleGapSaveKeysCmdMonitor
(
    uint8_t        nvmIndex,
    gapSmpKeys_t*  pKeys,
    bool_t         leSc,
    bool_t         auth
);

/*! *********************************************************************************
* \brief  Gap_SetChannelMap command monitoring function.
*
* \param channelMap    Channel map to be set in Controller
*
********************************************************************************** */
void fsciBleGapSetChannelMapCmdMonitor
(
    bleChannelMap_t channelMap
);

/*! *********************************************************************************
* \brief  Gap_VerifyPrivateResolvableAddress command monitoring function.
*
* \param[in]    nvmIndex    Index of the device in NVM bonding area whose IRK must
*                           be checked.
* \param[in]    aAddress    The Private Resolvable Address to be verified.
*
********************************************************************************** */
void fsciBleGapVerifyPrivateResolvableAddressCmdMonitor
(
    uint8_t             nvmIndex,
    const bleDeviceAddress_t  aAddress
);

/*! *********************************************************************************
* \brief  Gap_SetRandomAddress command monitoring function.
*
* \param[in]    aAddress    The Private Resolvable, Private Non-Resolvable or Static
*                           Random Address.
*
********************************************************************************** */
void fsciBleGapSetRandomAddressCmdMonitor
(
    const bleDeviceAddress_t aAddress
);

/*! *********************************************************************************
* \brief  Gap_SetScanMode command monitoring function.
*
* \param[in]    discoveryMode       The scan mode to be activated.
* \param[in]    pAutoConnectParams  Parameters for the Auto Connect Scan Mode.
*
********************************************************************************** */
void fsciBleGapSetScanModeCmdMonitor
(
    gapScanMode_t           scanMode,
    gapAutoConnectParams_t* pAutoConnectParams
);


/*! *********************************************************************************
* \brief  Gap_SetDefaultPairingParameters command monitoring function.
*
* \param[in]    pPairingParameters  Pairing parameters as required by the SMP or NULL.
*
********************************************************************************** */
void fsciBleGapSetDefaultPairingParametersCmdMonitor
(
    const gapPairingParameters_t* pPairingParameters
);

/*! *********************************************************************************
* \brief  Gap_UpdateConnectionParameters command monitoring function.
*
* \param[in]    deviceId            The DeviceID for which the command is intended.
* \param[in]    intervalMin         The minimum value for the connection event interval.
* \param[in]    intervalMax         The maximum value for the connection event interval.
* \param[in]    peripheralLatency        The peripheral latency parameter.
* \param[in]    timeoutMultiplier   The connection timeout parameter.
* \param[in]    minCeLength         The minimum connection event length.
* \param[in]    maxCeLength         The maximum connection event length.
*
********************************************************************************** */
void fsciBleGapUpdateConnectionParametersCmdMonitor
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
* \brief  Gap_EnableUpdateConnectionParameters command monitoring function.
*
* \param[in]    deviceId            The DeviceID for which the command is intended.
* \param[in]    enable              Allow/disallow the parameters update.
*
********************************************************************************** */
void fsciBleGapEnableUpdateConnectionParametersCmdMonitor
(
    deviceId_t  deviceId,
    bool_t      enable
);

/*! *********************************************************************************
* \brief  Gap_SetPeriodicAdvParameters command monitoring function.
*
* \param[in]    pAdvertisingParameters      Pointer to gapPeriodicAdvParameters_t
*                                           structure.
*
********************************************************************************** */
void fsciBleGapSetPeriodicAdvParametersCmdMonitor
(
    gapPeriodicAdvParameters_t* pAdvertisingParameters
);

#if defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U) && (defined(CPU_KW45B41Z83AFTA) || defined(CPU_K32W1480VFTA))
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdPlatformRegisterErrorCallback(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdPlatformRegisterErrorCallbackOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdPlatformRegisterErrorCallback
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
#endif /* defined(gFsciBleTest_d) && (gFsciBleTest_d == 1U) && (defined(CPU_KW45B41Z83AFTA) || defined(CPU_K32W1480VFTA)) */

#endif /* gFsciBleHost_d || gFsciBleTest_d */
/*! *********************************************************************************
* \brief  Creates a GAP FSCI packet with a boolean payload and sends it over UART.
*
* \param[in]    opCode      GAP event operation code.
* \param[in]    bParam      The boolean parameter.
*
********************************************************************************** */
void fsciBleGapBoolParamEvtMonitor
(
    fsciBleGapOpCode_t  opCode,
    bool_t              bParam
);

/*! *********************************************************************************
* \brief  Creates a GAP FSCI packet which contains an array as payload and sends
*         it over UART.
*
* \param[in]    opCode      GAP event operation code.
* \param[in]    aArray      The array.
* \param[in]    arraySize   The array size.
*
********************************************************************************** */
void fsciBleGapArrayAndSizeParamEvtMonitor
(
    fsciBleGapOpCode_t  opCode,
    const uint8_t*      aArray,
    uint8_t             arraySize
);

/*! *********************************************************************************
* \brief  Gap_GetBondedDevicesIdentityInformation command out parameters monitoring function.
*
* \param[in]    aOutIdentityAddresses   Array of addresses.
* \param[in]    pOutActualCount         The number of addresses.
*
********************************************************************************** */
void fsciBleGapGetBondedDevIdentityInfoEvtMonitor
(
    const gapIdentityInformation_t*   aOutIdentityAddresses,
    const uint8_t*                    pOutActualCount
);

/*! *********************************************************************************
* \brief  Gap_LoadCustomPeerInformation command out parameters monitoring function.
*
* \param[in]    aOutInfo    Pointer to the beginning of the allocated memory.
* \param[in]    infoSize    Data size (maximum equal to gcReservedFlashSizeForCustomInformation_d).
*
********************************************************************************** */
void fsciBleGapLoadCustomPeerInfoEvtMonitor
(
    const uint8_t*    aOutInfo,
    uint16_t    infoSize
);

/*! *********************************************************************************
* \brief  Gap_CheckIfBonded out parameter monitoring macro.
*
* \param[in]    pOutIsActive    Pointer to the bonded flag.
* \param[in]    pOutNvmIndex    Pointer to the NVM index.
*
********************************************************************************** */
void fsciBleGapCheckIfBondedEvtMonitor
(
    bool_t*     pOutIsBonded,
    uint8_t*    pOutNvmIndex
);

/*! *********************************************************************************
* \brief  Gap_GetDeviceIdFromConnHandle out parameter monitoring macro.
*
* \param[in]    pDeviceId    Pointer to the device id value.
*
********************************************************************************** */
void fsciBleGapGetDeviceIdFromConnHandleEvtMonitor
(
    deviceId_t    deviceId
);

/*! *********************************************************************************
* \brief  GetConnectionHandleFromDeviceId out parameter monitoring macro.
*
* \param[in]    pConnHandle    Pointer to the connection handle value.
*
********************************************************************************** */
void fsciBleGapGetConnectionHandleFromDeviceIdEvtMonitor
(
    uint16_t    connHandle
);

/*! *********************************************************************************
* \brief  Gap_CheckNvmIndex out parameter monitoring macro.
*
* \param[in]    pOutIsFree    Pointer to the status flag.
*
********************************************************************************** */
void fsciBleGapCheckNvmIndexEvtMonitor
(
    bool_t*     pOutIsFree
);

/*! *********************************************************************************
* \brief  Gap_LoadKeys out parameter monitoring macro.
*
* \param[in]    pOutIsActive    Pointer to the keys.
* \param[in]    pOutKeyFlags    Pointer to the key flags.
* \param[in]    pOutLeSc        Pointer to the LE SC used.
* \param[in]    pOutAuth        Pointer to the authenticated used.
*
********************************************************************************** */
void fsciBleGapLoadKeysEvtMonitor
(
    gapSmpKeys_t*     pOutKeys,
    gapSmpKeyFlags_t* pOutKeyFlags,
    bool_t*           pOutLeSc,
    bool_t*           pOutAuth
);

/*! *********************************************************************************
* \brief  gapGenericCallback event monitoring function.
*
* \param[in]    pGenericEvent    Controller event information.
*
********************************************************************************** */
void fsciBleGapGenericEvtMonitor
(
    gapGenericEvent_t* pGenericEvent
);

/*! *********************************************************************************
* \brief  gapAdvertisingCallback event monitoring function.
*
* \param[in]    pAdvertisingEvent    Advertising event information.
*
********************************************************************************** */
void fsciBleGapAdvertisingEvtMonitor
(
    gapAdvertisingEvent_t* pAdvertisingEvent
);

/*! *********************************************************************************
* \brief  gapScanningCallback event monitoring function.
*
* \param[in]    pScanningEvent    Scanning event information.
*
********************************************************************************** */
void fsciBleGapScanningEvtMonitor
(
    gapScanningEvent_t* pScanningEvent
);

/*! *********************************************************************************
* \brief  gapConnectionCallback event monitoring function.
*
* \param[in]    deviceId            Device identifier.
* \param[in]    pConnectionEvent    Connection event information.
*
********************************************************************************** */
void fsciBleGapConnectionEvtMonitor
(
    deviceId_t              deviceId,
    gapConnectionEvent_t*   pConnectionEvent
);

/*! *********************************************************************************
* \brief  GetHostVersion out parameter monitoring macro.
*
* \param[in]    pOutHostVersion          Pointer to the Host Version structure.
*
********************************************************************************** */
void fsciBleGapGetHostVersionEvtMonitor
(
    gapHostVersion_t *pOutHostVersion
);

#ifdef __cplusplus
}
#endif

#endif /* FSCI_BLE_ATT_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
