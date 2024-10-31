/*! *********************************************************************************
 * \defgroup FSCI_BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2023 NXP
*
*
* \file
*
* This is a header file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef FSCI_BLE_GATT_H
#define FSCI_BLE_GATT_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble_gatt_types.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/

/*! Macro which indicates if FSCI for GATT is enabled or not */
#ifndef gFsciBleGattLayerEnabled_d
    #define gFsciBleGattLayerEnabled_d          0
#endif /* gFsciBleGattLayerEnabled_d */

/*! FSCI operation group for GATT */
#define gFsciBleGattOpcodeGroup_c               0x45


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
#if gFsciIncluded_c && gFsciBleGattLayerEnabled_d

    #if gFsciBleHost_d || gFsciBleTest_d
        #define FsciCmdMonitor(function, ...)       fsciBleGatt##function##CmdMonitor(__VA_ARGS__)
    #else
        #define FsciCmdMonitor(function, ...)
    #endif

    #if gFsciBleBBox_d || gFsciBleTest_d
        #define FsciStatusMonitor(function, ...)    fsciBleGattStatusMonitor(__VA_ARGS__)
        #define FsciEvtMonitor(function, ...)       fsciBleGatt##function##EvtMonitor(__VA_ARGS__)
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
* \brief  Allocates a FSCI packet for GATT.
*
* \param[in]    opCode      FSCI GATT operation code
* \param[in]    dataSize    Size of the payload
*
* \return The allocated FSCI packet
*
********************************************************************************** */
#define fsciBleGattAllocFsciPacket(opCode, dataSize)        \
        fsciBleAllocFsciPacket(gFsciBleGattOpcodeGroup_c,   \
                               (opCode),                      \
                               (dataSize))

/*! *********************************************************************************
* \brief  Gatt_Init command monitoring macro.
*
********************************************************************************** */
#define fsciBleGattInitCmdMonitor() \
        fsciBleGattNoParamCmdMonitor(gBleGattCmdInitOpCode_c)

/*! *********************************************************************************
* \brief  GattClient_Init command monitoring macro.
*
********************************************************************************** */
#define fsciBleGattClientInitCmdMonitor() \
        fsciBleGattNoParamCmdMonitor(gBleGattCmdClientInitOpCode_c)

/*! *********************************************************************************
* \brief  GattClient_ResetProcedure command monitoring macro.
*
********************************************************************************** */
#define fsciBleGattClientResetProcedureCmdMonitor() \
        fsciBleGattNoParamCmdMonitor(gBleGattCmdClientResetProceduresOpCode_c)

/*! *********************************************************************************
* \brief  GattClient_ExchangeMtu command monitoring macro.
*
* \param[in]    deviceId    Device ID of the connected peer.
* \param[in]    mtu         Desired MTU size for the connected peer.
*
********************************************************************************** */
#define fsciBleGattClientExchangeMtuCmdMonitor(deviceId, mtu)                \
        fsciBleGattExchangeMtuCmdMonitor(gBleGattCmdClientExchangeMtuOpCode_c,  \
                                 (deviceId),                            \
                                 (mtu))

/*! *********************************************************************************
* \brief  GattClient_FindIncludedServices command monitoring macro.
*
* \param[in]    deviceId            Device ID of the connected peer.
* \param[in]    pIoService          The service within which inclusions should be
*                                       searched.
* \param[in]    maxServiceCount     Maximum number of included services to be filled.
*
********************************************************************************** */
#define fsciBleGattClientFindIncludedServicesCmdMonitor(deviceId,           \
                                                        pIoService,         \
                                                        maxServiceCount)    \
        fsciBleGattClientFindIncludedServicesOrCharacteristicsCmdMonitor(   \
                    gBleGattCmdClientFindIncludedServicesOpCode_c,          \
                    (deviceId),                                               \
                    (pIoService),                                             \
                    (maxServiceCount))

/*! *********************************************************************************
* \brief  GattClient_DiscoverAllCharacteristicsOfService command monitoring macro.
*
* \param[in]    deviceId                  Device ID of the connected peer.
* \param[in]    pIoService                The service within which characteristics
*                                         should be searched.
* \param[in]    maxCharacteristicCount    Maximum number of characteristics to be filled.
*
********************************************************************************** */
#define fsciBleGattClientDiscoverAllCharacteristicsOfServiceCmdMonitor(             \
                    deviceId,                                                       \
                    pIoService,                                                     \
                    maxCharacteristicCount)                                         \
        fsciBleGattClientFindIncludedServicesOrCharacteristicsCmdMonitor(           \
                    gBleGattCmdClientDiscoverAllCharacteristicsOfServiceOpCode_c,   \
                    (deviceId),                                                       \
                    (pIoService),                                                     \
                    (maxCharacteristicCount))

/*! *********************************************************************************
* \brief  GattServer_Init command monitoring macro.
*
********************************************************************************** */
#define fsciBleGattServerInitCmdMonitor() \
        fsciBleGattNoParamCmdMonitor(gBleGattCmdServerInitOpCode_c)

/*! *********************************************************************************
* \brief  GattServer_RegisterHandlesForWriteNotifications command monitoring macro.
*
* \param[in]    handleCount         Number of handles in array.
* \param[in]    aAttributeHandles   Statically allocated array of handles.
*
********************************************************************************** */
#define fsciBleGattServerRegisterHandlesForWriteNotificationsCmdMonitor(        \
                                                            handleCount,        \
                                                            aAttributeHandles)  \
        fsciBleGattServerRegisterHandlesForWriteOrReadNotificationsCmdMonitor(  \
                gBleGattCmdServerRegisterHandlesForWriteNotificationsOpCode_c,  \
                (handleCount),                                                    \
                (aAttributeHandles)                                               \
                )

/*! *********************************************************************************
* \brief  GattServer_RegisterHandlesForReadNotifications command monitoring macro.
*
* \param[in]    handleCount         Number of handles in array.
* \param[in]    aAttributeHandles   Statically allocated array of handles.
*
********************************************************************************** */
#define fsciBleGattServerRegisterHandlesForReadNotificationsCmdMonitor(         \
                                                            handleCount,        \
                                                            aAttributeHandles)  \
        fsciBleGattServerRegisterHandlesForWriteOrReadNotificationsCmdMonitor(  \
                gBleGattCmdServerRegisterHandlesForReadNotificationsOpCode_c,   \
                (handleCount),                                                    \
                (aAttributeHandles)                                               \
                )

/*! *********************************************************************************
* \brief  GattServer_UnregisterHandlesForWriteNotifications command monitoring macro.
*
* \param[in]    handleCount         Number of handles in array.
* \param[in]    aAttributeHandles   Statically allocated array of handles.
*
********************************************************************************** */
#define fsciBleGattServerUnregisterHandlesForWriteNotificationsCmdMonitor(      \
                                                            handleCount,        \
                                                            aAttributeHandles)  \
        fsciBleGattServerRegisterHandlesForWriteOrReadNotificationsCmdMonitor(  \
                gBleGattCmdServerUnregisterHandlesForWriteNotificationsOpCode_c,  \
                (handleCount),                                                    \
                (aAttributeHandles)                                               \
                )

/*! *********************************************************************************
* \brief  GattServer_UnregisterHandlesForReadNotifications command monitoring macro.
*
* \param[in]    handleCount         Number of handles in array.
* \param[in]    aAttributeHandles   Statically allocated array of handles.
*
********************************************************************************** */
#define fsciBleGattServerUnregisterHandlesForReadNotificationsCmdMonitor(       \
                                                            handleCount,        \
                                                            aAttributeHandles)  \
        fsciBleGattServerRegisterHandlesForWriteOrReadNotificationsCmdMonitor(  \
                gBleGattCmdServerUnregisterHandlesForReadNotificationsOpCode_c,   \
                (handleCount),                                                    \
                (aAttributeHandles)                                               \
                )

/*! *********************************************************************************
* \brief  GattServer_SendAttributeWrittenStatus command monitoring macro.
*
* \param[in]    deviceId            The device ID of the connected peer.
* \param[in]    attributeHandle     The attribute handle that was written.
* \param[in]    status              The status of the write operation.
*
********************************************************************************** */
#define fsciBleGattServerSendAttributeWrittenStatusCmdMonitor(                  \
                                                    deviceId,                   \
                                                    attributeHandle,            \
                                                    status)                     \
        fsciBleGattServerSendAttributeWrittenOrReadStatusCmdMonitor(            \
                gBleGattCmdServerSendAttributeWrittenStatusOpCode_c,            \
                (deviceId),                                                       \
                (attributeHandle),                                                \
                (status))

/*! *********************************************************************************
* \brief  GattServer_SendAttributeReadStatus command monitoring macro.
*
* \param[in]    deviceId            The device ID of the connected peer.
* \param[in]    attributeHandle     The attribute handle that was written.
* \param[in]    status              The status of the write operation.
*
********************************************************************************** */
#define fsciBleGattServerSendAttributeReadStatusCmdMonitor(                     \
                                                    deviceId,                   \
                                                    attributeHandle,            \
                                                    status)                     \
        fsciBleGattServerSendAttributeWrittenOrReadStatusCmdMonitor(            \
                gBleGattCmdServerSendAttributeReadStatusOpCode_c,               \
                (deviceId),                                                       \
                (attributeHandle),                                                \
                (status))

/*! *********************************************************************************
* \brief  GattServer_SendNotification command monitoring macro.
*
* \param[in]    deviceId    The device ID of the connected peer.
* \param[in]    handle      Handle of the Value of the Characteristic to be notified.
*
********************************************************************************** */
#define fsciBleGattServerSendNotificationCmdMonitor(deviceId,       \
                                                    handle)         \
        fsciBleGattServerSendNotificationOrIndicationCmdMonitor(    \
                    gBleGattCmdServerSendNotificationOpCode_c,      \
                    (deviceId),                                       \
                    (handle))

/*! *********************************************************************************
* \brief  GattServer_SendIndication command monitoring macro.
*
* \param[in]    deviceId    The device ID of the connected peer.
* \param[in]    handle      Handle of the Value of the Characteristic to be notified.
*
********************************************************************************** */
#define fsciBleGattServerSendIndicationCmdMonitor(deviceId,         \
                                                  handle)           \
        fsciBleGattServerSendNotificationOrIndicationCmdMonitor(    \
                    gBleGattCmdServerSendIndicationOpCode_c,        \
                    (deviceId),                                       \
                    (handle))


/*! *********************************************************************************
* \brief  GattServer_SendInstantValueNotification command monitoring macro.
*
* \param[in]    opCode          GATT event operation code.
* \param[in]    deviceId        The device ID of the connected peer.
* \param[in]    handle          Handle of the Value of the Characteristic to be notified.
* \param[in]    valueLength     Length of data to be notified.
* \param[in]    pValue          Data to be notified.
*
********************************************************************************** */
#define fsciBleGattServerSendInstantValueNotificationCmdMonitor(deviceId,       \
                                                                handle,         \
                                                                valueLength,    \
                                                                pValue)         \
        fsciBleGattServerSendInstantValueNotificationOrIndicationCmdMonitor(    \
                   gBleGattCmdServerSendInstantValueNotificationOpCode_c,       \
                   (deviceId),                                                    \
                   (handle),                                                      \
                   (valueLength),                                                 \
                   (pValue))

/*! *********************************************************************************
* \brief  GattServer_SendInstantValueIndication command monitoring macro.
*
* \param[in]    opCode          GATT event operation code.
* \param[in]    deviceId        The device ID of the connected peer.
* \param[in]    handle          Handle of the Value of the Characteristic to be indicated.
* \param[in]    valueLength     Length of data to be indicated.
* \param[in]    pValue          Data to be indicated.
*
********************************************************************************** */
#define fsciBleGattServerSendInstantValueIndicationCmdMonitor(deviceId,         \
                                                              handle,           \
                                                              valueLength,      \
                                                              pValue)           \
        fsciBleGattServerSendInstantValueNotificationOrIndicationCmdMonitor(    \
                   gBleGattCmdServerSendInstantValueIndicationOpCode_c,         \
                   (deviceId),                                                    \
                   (handle),                                                      \
                   (valueLength),                                                 \
                   (pValue))

/*! *********************************************************************************
* \brief  gattClientNotificationCallback event monitoring macro.
*
* \param[in]    deviceId                    Device ID of the connected peer.
* \param[in]    characteristicValueHandle   Handle of the value of the characteristic
*                                           that is notified.
* \param[in]    aValue                      Characteristic value.
* \param[in]    valueLength                 Characteristic value length.
*
********************************************************************************** */
#define fsciBleGattClientNotificationEvtMonitor(deviceId,                   \
                                                characteristicValueHandle,  \
                                                aValue,                     \
                                                valueLength)                \
        fsciBleGattClientNotificationOrIndicationEvtMonitor(                \
                                    gBleGattEvtClientNotificationOpCode_c,  \
                                    (deviceId),                               \
                                    gUnenhancedBearerId_c,                    \
                                    (characteristicValueHandle),              \
                                    (aValue),                                 \
                                    (valueLength))

/*! *********************************************************************************
* \brief  gattClientIndicationCallback event monitoring macro.
*
* \param[in]    deviceId                    Device ID of the connected peer.
* \param[in]    characteristicValueHandle   Handle of the value of the characteristic
*                                           that is notified.
* \param[in]    aValue                      Characteristic value.
* \param[in]    valueLength                 Characteristic value length.
*
********************************************************************************** */
#define fsciBleGattClientIndicationEvtMonitor(deviceId,                     \
                                              characteristicValueHandle,    \
                                              aValue,                       \
                                              valueLength)                  \
        fsciBleGattClientNotificationOrIndicationEvtMonitor(                \
                                    gBleGattEvtClientIndicationOpCode_c,    \
                                    (deviceId),                               \
                                    gUnenhancedBearerId_c,                    \
                                    (characteristicValueHandle),              \
                                    (aValue),                                 \
                                    (valueLength))

/*! *********************************************************************************
* \brief  gattClientEnhancedNotificationCallback event monitoring macro.
*
* \param[in]    deviceId                    Device ID of the connected peer.
* \param[in]    bearerId                    Bearer ID of the of the ATT bearer used.
* \param[in]    characteristicValueHandle   Handle of the value of the characteristic
*                                           that is notified.
* \param[in]    aValue                      Characteristic value.
* \param[in]    valueLength                 Characteristic value length.
*
********************************************************************************** */
#define fsciBleGattClientEnhancedNotificationEvtMonitor(deviceId,           \
                                                bearerId,                   \
                                                characteristicValueHandle,  \
                                                aValue,                     \
                                                valueLength)                \
        fsciBleGattClientNotificationOrIndicationEvtMonitor(                            \
                                    gBleGattEvtClientEnhancedNotificationOpCode_c,      \
                                    (deviceId),                                         \
                                    (bearerId),                                         \
                                    (characteristicValueHandle),                        \
                                    (aValue),                                           \
                                    (valueLength))

/*! *********************************************************************************
* \brief  gattClientEnhancedIndicationCallback event monitoring macro.
*
* \param[in]    deviceId                    Device ID of the connected peer.
* \param[in]    bearerId                    Bearer ID of the of the ATT bearer used.
* \param[in]    characteristicValueHandle   Handle of the value of the characteristic
*                                           that is notified.
* \param[in]    aValue                      Characteristic value.
* \param[in]    valueLength                 Characteristic value length.
*
********************************************************************************** */
#define fsciBleGattClientEnhancedIndicationEvtMonitor(deviceId,             \
                                              bearerId,                     \
                                              characteristicValueHandle,    \
                                              aValue,                       \
                                              valueLength)                  \
        fsciBleGattClientNotificationOrIndicationEvtMonitor(                            \
                                    gBleGattEvtClientEnhancedIndicationOpCode_c,        \
                                    (deviceId),                                         \
                                    (bearerId),                                         \
                                    (characteristicValueHandle),                        \
                                    (aValue),                                           \
                                    (valueLength))

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*! FSCI operation codes for GATT */
typedef enum
{
    gBleGattModeSelectOpCode_c                                                         = 0x00,                         /*! GAP Mode Select operation code */

    gBleGattCmdFirstOpCode_c                                                           = 0x01,
    gBleGattCmdInitOpCode_c                                                            = gBleGattCmdFirstOpCode_c,     /*! Gatt_Init command operation code */
    gBleGattCmdGetMtuOpCode_c                                                          = 0x02,                          /*! Gatt_GetMtu command operation code */

    gBleGattCmdClientInitOpCode_c                                                      = 0x03,                         /*! GattClient_Init command operation code */
    gBleGattCmdClientResetProceduresOpCode_c                                           = 0x04,                         /*! GattClient_ResetProcedures command operation code */
    gBleGattCmdClientRegisterProcedureCallbackOpCode_c                                 = 0x05,                         /*! GattClient_RegisterProcedureCallback command operation code */
    gBleGattCmdClientRegisterNotificationCallbackOpCode_c                              = 0x06,                         /*! GattClient_RegisterNotificationCallback command operation code */
    gBleGattCmdClientRegisterIndicationCallbackOpCode_c                                = 0x07,                         /*! GattClient_RegisterIndicationCallback command operation code */
    gBleGattCmdClientExchangeMtuOpCode_c                                               = 0x08,                         /*! GattClient_ExchangeMtu command operation code */
    gBleGattCmdClientDiscoverAllPrimaryServicesOpCode_c                                = 0x09,                         /*! GattClient_DiscoverAllPrimaryServices command operation code */
    gBleGattCmdClientDiscoverPrimaryServicesByUuidOpCode_c                             = 0x0A,                         /*! GattClient_DiscoverPrimaryServicesByUuid command operation code */
    gBleGattCmdClientFindIncludedServicesOpCode_c                                      = 0x0B,                         /*! GattClient_FindIncludedServices command operation code */
    gBleGattCmdClientDiscoverAllCharacteristicsOfServiceOpCode_c                       = 0x0C,                         /*! GattClient_DiscoverAllCharacteristicsOfService command operation code */
    gBleGattCmdClientDiscoverCharacteristicOfServiceByUuidOpCode_c                     = 0x0D,                         /*! GattClient_DiscoverCharacteristicOfServiceByUuid command operation code */
    gBleGattCmdClientDiscoverAllCharacteristicDescriptorsOpCode_c                      = 0x0E,                         /*! GattClient_DiscoverAllCharacteristicDescriptors command operation code */
    gBleGattCmdClientReadCharacteristicValueOpCode_c                                   = 0x0F,                         /*! GattClient_ReadCharacteristicValue command operation code */
    gBleGattCmdClientReadUsingCharacteristicUuidOpCode_c                               = 0x10,                         /*! GattClient_ReadUsingCharacteristicUuid command operation code */
    gBleGattCmdClientReadMultipleCharacteristicValuesOpCode_c                          = 0x11,                         /*! GattClient_ReadMultipleCharacteristicValues command operation code */
    gBleGattCmdClientWriteCharacteristicValueOpCode_c                                  = 0x12,                         /*! GattClient_WriteCharacteristicValue command operation code */
    gBleGattCmdClientReadCharacteristicDescriptorsOpCode_c                             = 0x13,                         /*! GattClient_ReadCharacteristicDescriptors command operation code */
    gBleGattCmdClientWriteCharacteristicDescriptorsOpCode_c                            = 0x14,                         /*! GattClient_WriteCharacteristicDescriptors command operation code */

    gBleGattCmdServerInitOpCode_c                                                      = 0x15,                         /*! GattServer_Init command operation code */
    gBleGattCmdServerRegisterCallbackOpCode_c                                          = 0x16,                         /*! GattServer_RegisterCallback command operation code */
    gBleGattCmdServerRegisterHandlesForWriteNotificationsOpCode_c                      = 0x17,                         /*! GattServer_RegisterHandlesForWriteNotifications command operation code */
    gBleGattCmdServerSendAttributeWrittenStatusOpCode_c                                = 0x18,                         /*! GattServer_SendAttributeWrittenStatus command operation code */
    gBleGattCmdServerSendNotificationOpCode_c                                          = 0x19,                         /*! GattServer_SendNotification command operation code */
    gBleGattCmdServerSendIndicationOpCode_c                                            = 0x1A,                         /*! GattServer_SendIndication command operation code */
    gBleGattCmdServerSendInstantValueNotificationOpCode_c                              = 0x1B,                         /*! GattServer_SendInstantValueNotification command operation code */
    gBleGattCmdServerSendInstantValueIndicationOpCode_c                                = 0x1C,                         /*! GattServer_SendInstantValueIndication command operation code */
    gBleGattCmdServerRegisterHandlesForReadNotificationsOpCode_c                       = 0x1D,                         /*! GattServer_RegisterHandlesForReadNotifications command operation code */
    gBleGattCmdServerSendAttributeReadStatusOpCode_c                                   = 0x1E,                         /*! GattServer_SendAttributeReadStatus command operation code */
    gBleGattCmdServerRegisterUniqueHandlesForNotificationsOpCode_c                     = 0x1F,                         /*! GattServer_RegisterUniqueHandlesForNotifications command operation code */
    gBleGattCmdServerUnregisterHandlesForWriteNotificationsOpCode_c                    = 0x20,                         /*! GattServer_UnregisterHandlesForWriteNotifications command operation code */
    gBleGattCmdServerUnregisterHandlesForReadNotificationsOpCode_c                     = 0x21,                         /*! GattServer_UnregisterHandlesForReadNotifications command operation code */
    gBleGattCmdServerSendMultipleHandleValueNotificationOpCode_c                       = 0x22,                         /*! GattServer_SendMultipleHandleValueNotification command operation code */
    gBleGattCmdClientRegisterMultipleValueNotificationCallbackOpCode_c                 = 0x23,                         /*! GattClient_RegisterMultipleValueNotificationCallback command operation code */
    gBleGattCmdClientReadMultipleVariableCharacteristicValuesOpCode_c                  = 0x24,                         /*! GattClient_ReadMultipleVariableCharacteristicValues*/

    gBleGattCmdClientRegisterEnhancedProcedureCallbackOpCode_c                         = 0x25,                         /*! GattClient_RegisterEnhancedProcedureCallback command operation code */
    gBleGattCmdClientRegisterEnhancedNotificationCallbackOpCode_c                      = 0x26,                         /*! GattClient_RegisterEnhancedNotificationCallback command operation code */
    gBleGattCmdClientRegisterEnhancedIndicationCallbackOpCode_c                        = 0x27,                         /*! GattClient_RegisterEnhancedIndicationCallback command operation code */
    gBleGattCmdClientRegisterEnhancedMultipleValueNotificationCallbackOpCode_c         = 0x28,                         /*! GattClient_RegisterMultipleValueNotificationCallback command operation code */
    gBleGattCmdClientEnhancedDiscoverAllPrimaryServicesOpCode_c                        = 0x29,                         /*! GattClient_EnhancedDiscoverAllPrimaryServices command operation code */
    gBleGattCmdClientEnhancedDiscoverPrimaryServicesByUuidOpCode_c                     = 0x2A,                         /*! GattClient_EnhancedDiscoverPrimaryServicesByUuid command operation code */
    gBleGattCmdClientEnhancedFindIncludedServicesOpCode_c                              = 0x2B,                         /*! GattClient_EnhancedFindIncludedServices command operation code */
    gBleGattCmdClientEnhancedDiscoverAllCharacteristicsOfServiceOpCode_c               = 0x2C,                         /*! GattClient_EnhancedDiscoverAllCharacteristicsOfService command operation code */
    gBleGattCmdClientEnhancedDiscoverCharacteristicOfServiceByUuidOpCode_c             = 0x2D,                         /*! GattClient_EnhancedDiscoverCharacteristicOfServiceByUuid command operation code */
    gBleGattCmdClientEnhancedDiscoverAllCharacteristicDescriptorsOpCode_c              = 0x2E,                         /*! GattClient_EnhancedDiscoverAllCharacteristicDescriptors command operation code */
    gBleGattCmdClientEnhancedReadCharacteristicValueOpCode_c                           = 0x2F,                         /*! GattClient_EnhancedReadCharacteristicValue command operation code */
    gBleGattCmdClientEnhancedReadUsingCharacteristicUuidOpCode_c                       = 0x30,                         /*! GattClient_EnhancedReadUsingCharacteristicUuid command operation code */
    gBleGattCmdClientEnhancedReadMultipleCharacteristicValuesOpCode_c                  = 0x31,                         /*! GattClient_EnhancedReadMultipleCharacteristicValues command operation code */
    gBleGattCmdClientEnhancedWriteCharacteristicValueOpCode_c                          = 0x32,                         /*! GattClient_EnhancedWriteCharacteristicValue command operation code */
    gBleGattCmdClientEnhancedReadCharacteristicDescriptorsOpCode_c                     = 0x33,                         /*! GattClient_EnhancedReadCharacteristicDescriptors command operation code */
    gBleGattCmdClientEnhancedWriteCharacteristicDescriptorsOpCode_c                    = 0x34,                         /*! GattClient_EnhancedWriteCharacteristicDescriptors command operation code */
    gBleGattCmdClientEnhancedReadMultipleVariableCharacteristicValuesOpCode_c          = 0x35,                         /*! GattClient_EnhancedReadMultipleVariableCharacteristicValues*/

    gBleGattCmdServerRegisterEnhancedCallbackOpCode_c                                  = 0x36,                         /*! GattServer_RegisterEnhancedCallback command operation code */
    gBleGattCmdServerEnhancedSendAttributeWrittenStatusOpCode_c                        = 0x37,                         /*! GattServer_EnhancedSendAttributeWrittenStatus command operation code */
    gBleGattCmdServerEnhancedSendNotificationOpCode_c                                  = 0x38,                         /*! GattServer_EnhancedSendNotification command operation code */
    gBleGattCmdServerEnhancedSendIndicationOpCode_c                                    = 0x39,                         /*! GattServer_EnhancedSendIndication command operation code */
    gBleGattCmdServerEnhancedSendInstantValueNotificationOpCode_c                      = 0x3A,                         /*! GattServer_EnhancedSendInstantValueNotification command operation code */
    gBleGattCmdServerEnhancedSendInstantValueIndicationOpCode_c                        = 0x3B,                         /*! GattServer_EnhancedSendInstantValueIndication command operation code */
    gBleGattCmdServerEnhancedSendAttributeReadStatusOpCode_c                           = 0x3C,                         /*! GattServer_EnhancedSendAttributeReadStatus command operation code */
    gBleGattCmdServerEnhancedSendMultipleHandleValueNotificationOpCode_c               = 0x3D,                         /*! GattServer_EnhancedSendMultipleHandleValueNotification command operation code */

    gBleGattCmdClientGetDatabaseHashOpCode_c                                           = 0x3E,                         /*! GattClient_GetDatabaseHash command operation code */


    gBleGattStatusOpCode_c                                                             = 0x80,                         /*! GAP status operation code */

    gBleGattEvtFirstOpCode_c                                                           = 0x81,
    gBleGattEvtGetMtuOpCode_c                                                          = gBleGattEvtFirstOpCode_c,     /*! Gatt_GetMtu command out parameter event operation code */

    gBleGattEvtClientProcedureExchangeMtuOpCode_c                                      = 0x82,                         /*! gattClientProcedureCallback (procedureType == gGattProcExchangeMtu_c) event operation code */
    gBleGattEvtClientProcedureDiscoverAllPrimaryServicesOpCode_c                       = 0x83,                         /*! gattClientProcedureCallback (procedureType == gGattProcDiscoverAllPrimaryServices_c) event operation code */
    gBleGattEvtClientProcedureDiscoverPrimaryServicesByUuidOpCode_c                    = 0x84,                         /*! gattClientProcedureCallback (procedureType == gGattProcDiscoverPrimaryServicesByUuid_c) event operation code */
    gBleGattEvtClientProcedureFindIncludedServicesOpCode_c                             = 0x85,                         /*! gattClientProcedureCallback (procedureType == gGattProcFindIncludedServices_c) event operation code */
    gBleGattEvtClientProcedureDiscoverAllCharacteristicsOpCode_c                       = 0x86,                         /*! gattClientProcedureCallback (procedureType == gGattProcDiscoverAllCharacteristics_c) event operation code */
    gBleGattEvtClientProcedureDiscoverCharacteristicByUuidOpCode_c                     = 0x87,                         /*! gattClientProcedureCallback (procedureType == gGattProcDiscoverCharacteristicByUuid_c) event operation code */
    gBleGattEvtClientProcedureDiscoverAllCharacteristicDescriptorsOpCode_c             = 0x88,                         /*! gattClientProcedureCallback (procedureType == gGattProcDiscoverAllCharacteristicDescriptors_c) event operation code */
    gBleGattEvtClientProcedureReadCharacteristicValueOpCode_c                          = 0x89,                         /*! gattClientProcedureCallback (procedureType == gGattProcReadCharacteristicValue_c) event operation code */
    gBleGattEvtClientProcedureReadUsingCharacteristicUuidOpCode_c                      = 0x8A,                         /*! gattClientProcedureCallback (procedureType == gGattProcReadUsingCharacteristicUuid_c) event operation code */
    gBleGattEvtClientProcedureReadMultipleCharacteristicValuesOpCode_c                 = 0x8B,                         /*! gattClientProcedureCallback (procedureType == gGattProcReadMultipleCharacteristicValues_c) event operation code */
    gBleGattEvtClientProcedureWriteCharacteristicValueOpCode_c                         = 0x8C,                         /*! gattClientProcedureCallback (procedureType == gGattProcWriteCharacteristicValue_c) event operation code */
    gBleGattEvtClientProcedureReadCharacteristicDescriptorOpCode_c                     = 0x8D,                         /*! gattClientProcedureCallback (procedureType == gGattProcReadCharacteristicDescriptor_c) event operation code */
    gBleGattEvtClientProcedureWriteCharacteristicDescriptorOpCode_c                    = 0x8E,                         /*! gattClientProcedureCallback (procedureType == gGattProcWriteCharacteristicDescriptor_c) event operation code */
    gBleGattEvtClientNotificationOpCode_c                                              = 0x8F,                         /*! gattClientNotificationCallback event operation code */
    gBleGattEvtClientIndicationOpCode_c                                                = 0x90,                         /*! gattClientIndicationCallback event operation code */

    gBleGattEvtServerMtuChangedOpCode_c                                                = 0x91,                         /*! gattServerCallback (eventType == gEvtMtuChanged_c) event operation code */
    gBleGattEvtServerHandleValueConfirmationOpCode_c                                   = 0x92,                         /*! gattServerCallback (eventType == gEvtHandleValueConfirmation_c) event operation code */
    gBleGattEvtServerAttributeWrittenOpCode_c                                          = 0x93,                         /*! gattServerCallback (eventType == gEvtAttributeWritten_c) event operation code */
    gBleGattEvtServerCharacteristicCccdWrittenOpCode_c                                 = 0x94,                         /*! gattServerCallback (eventType == gEvtCharacteristicCccdWritten_c) event operation code */
    gBleGattEvtServerAttributeWrittenWithoutResponseOpCode_c                           = 0x95,                         /*! gattServerCallback (eventType == gEvtAttributeWrittenWithoutResponse_c) event operation code */
    gBleGattEvtServerErrorOpCode_c                                                     = 0x96,                         /*! gattServerCallback (eventType == gEvtErrorOpCode_c) event operation code */
    gBleGattEvtServerLongCharacteristicWrittenOpCode_c                                 = 0x97,                         /*! gattServerCallback (eventType == gEvtLongCharacteristicWritten_c) event operation code */
    gBleGattEvtServerAttributeReadOpCode_c                                              = 0x98,                         /*! gattServerCallback (eventType == gEvtAttributeRead_c) event operation code */
    gBleGattEvtClientMultipleValueNotificationOpCode_c                                 = 0x99,                         /*! gattClientMultipleValueNotificationCallback event operation code */
    gBleGattEvtClientProcedureReadMultipleVariableLenCharValuesOpCode_c                = 0x9A,                         /*! gattClientProcedureCallback (procedureType == gGattProcReadMultipleVarLengthCharValues_c) event operation code */

    gBleGattEvtClientProcedureEnhancedDiscoverAllPrimaryServicesOpCode_c               = 0x9B,                         /*! gattClientProcedureCallback (procedureType == gGattProcDiscoverAllPrimaryServices_c) event operation code */
    gBleGattEvtClientProcedureEnhancedDiscoverPrimaryServicesByUuidOpCode_c            = 0x9C,                         /*! gattClientProcedureCallback (procedureType == gGattProcDiscoverPrimaryServicesByUuid_c) event operation code */
    gBleGattEvtClientProcedureEnhancedFindIncludedServicesOpCode_c                     = 0x9D,                         /*! gattClientProcedureCallback (procedureType == gGattProcFindIncludedServices_c) event operation code */
    gBleGattEvtClientProcedureEnhancedDiscoverAllCharacteristicsOpCode_c               = 0x9E,                         /*! gattClientProcedureCallback (procedureType == gGattProcDiscoverAllCharacteristics_c) event operation code */
    gBleGattEvtClientProcedureEnhancedDiscoverCharacteristicByUuidOpCode_c             = 0x9F,                         /*! gattClientProcedureCallback (procedureType == gGattProcDiscoverCharacteristicByUuid_c) event operation code */
    gBleGattEvtClientProcedureEnhancedDiscoverAllCharacteristicDescriptorsOpCode_c     = 0xA0,                         /*! gattClientProcedureCallback (procedureType == gGattProcDiscoverAllCharacteristicDescriptors_c) event operation code */
    gBleGattEvtClientProcedureEnhancedReadCharacteristicValueOpCode_c                  = 0xA1,                         /*! gattClientProcedureCallback (procedureType == gGattProcReadCharacteristicValue_c) event operation code */
    gBleGattEvtClientProcedureEnhancedReadUsingCharacteristicUuidOpCode_c              = 0xA2,                         /*! gattClientProcedureCallback (procedureType == gGattProcReadUsingCharacteristicUuid_c) event operation code */
    gBleGattEvtClientProcedureEnhancedReadMultipleCharacteristicValuesOpCode_c         = 0xA3,                         /*! gattClientProcedureCallback (procedureType == gGattProcReadMultipleCharacteristicValues_c) event operation code */
    gBleGattEvtClientProcedureEnhancedWriteCharacteristicValueOpCode_c                 = 0xA4,                         /*! gattClientProcedureCallback (procedureType == gGattProcWriteCharacteristicValue_c) event operation code */
    gBleGattEvtClientProcedureEnhancedReadCharacteristicDescriptorOpCode_c             = 0xA5,                         /*! gattClientProcedureCallback (procedureType == gGattProcReadCharacteristicDescriptor_c) event operation code */
    gBleGattEvtClientProcedureEnhancedWriteCharacteristicDescriptorOpCode_c            = 0xA6,                         /*! gattClientProcedureCallback (procedureType == gGattProcWriteCharacteristicDescriptor_c) event operation code */
    gBleGattEvtClientProcedureEnhancedReadMultipleVariableLenCharValuesOpCode_c        = 0xA7,                         /*! gattClientProcedureCallback (procedureType == gGattProcReadMultipleVarLengthCharValues_c) event operation code */
    gBleGattEvtClientEnhancedMultipleValueNotificationOpCode_c                         = 0xA8,                         /*! gattClientMultipleValueNotificationCallback event operation code */
    gBleGattEvtClientEnhancedNotificationOpCode_c                                      = 0xA9,                         /*! gattClientNotificationCallback event operation code */
    gBleGattEvtClientEnhancedIndicationOpCode_c                                        = 0xAA,                         /*! gattClientIndicationCallback event operation code */

    gBleGattEvtServerEnhancedHandleValueConfirmationOpCode_c                           = 0xAB,                         /*! gattServerCallback (eventType == gEvtHandleValueConfirmation_c) event operation code */
    gBleGattEvtServerEnhancedAttributeWrittenOpCode_c                                  = 0xAC,                         /*! gattServerCallback (eventType == gEvtAttributeWritten_c) event operation code */
    gBleGattEvtServerEnhancedCharacteristicCccdWrittenOpCode_c                         = 0xAD,                         /*! gattServerCallback (eventType == gEvtCharacteristicCccdWritten_c) event operation code */
    gBleGattEvtServerEnhancedAttributeWrittenWithoutResponseOpCode_c                   = 0xAE,                         /*! gattServerCallback (eventType == gEvtAttributeWrittenWithoutResponse_c) event operation code */
    gBleGattEvtServerEnhancedErrorOpCode_c                                             = 0xAF,                         /*! gattServerCallback (eventType == gEvtErrorOpCode_c) event operation code */
    gBleGattEvtServerEnhancedLongCharacteristicWrittenOpCode_c                         = 0xB0,                         /*! gattServerCallback (eventType == gEvtLongCharacteristicWritten_c) event operation code */
    gBleGattEvtServerEnhancedAttributeReadOpCode_c                                     = 0xB1,                         /*! gattServerCallback (eventType == gEvtAttributeRead_c) event operation code */
}fsciBleGattOpCode_t;

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
* \brief  Calls the GATT function associated with the operation code received over UART.
*         The GATT function parameters are extracted from the received FSCI payload.
*
* \param[in]    pData               Packet (containing FSCI header and FSCI
                                    payload) received over UART
* \param[in]    param               Pointer given when this function is registered in
                                    FSCI
* \param[in]    fsciInterfaceId     FSCI interface on which the packet was received
*
********************************************************************************** */
void fsciBleGattHandler
(
    void*       pData,
    void*       pParam,
    uint32_t    fsciInterfaceId
);

/*! *********************************************************************************
* \brief  Creates the FSCI packet which contains the status of the last executed GATT
*         command and sends it over UART.
*
* \param[in]    result      Status of the last executed GATT command.
*
********************************************************************************** */
void fsciBleGattStatusMonitor
(
    bleResult_t result
);

#if gFsciBleHost_d || gFsciBleTest_d
/*! *********************************************************************************
* \brief  Creates a GATT FSCI packet without payload and sends it over UART.
*
* \param[in]    opCode      GATT command operation code.
*
********************************************************************************** */
void fsciBleGattNoParamCmdMonitor
(
    fsciBleGattOpCode_t opCode
);

/*! *********************************************************************************
* \brief  Gatt_GetMtu commands monitoring helper function. Creates a GATT FSCI packet
*         with the device ID as payload and sends it over UART.
*
* \param[in]    opCode          GATT command operation code.
* \param[in]    deviceId        The device ID of the connected peer.
*
********************************************************************************** */
void fsciBleGattMtuCmdMonitor
(
    fsciBleGattOpCode_t opCode,
    deviceId_t          deviceId
);

/*! *********************************************************************************
* \brief  Gatt_GetMtu command monitoring function.
*
* \param[in]    deviceId    The device ID of the connected peer.
* \param[in]    pOutMtu     Pointer to integer to be written.
*
********************************************************************************** */
void fsciBleGattGetMtuCmdMonitor
(
    deviceId_t  deviceId,
    uint16_t*   pOutMtu
);

/*! *********************************************************************************
* \brief  GattClient_ExchangeMtu commands monitoring function.
*
* \param[in]    opCode          GATT command operation code.
* \param[in]    deviceId        The device ID of the connected peer.
* \param[in]    mtu             Desired MTU size for the connected peer.
*
********************************************************************************** */
void fsciBleGattExchangeMtuCmdMonitor
(
    fsciBleGattOpCode_t opCode,
    deviceId_t          deviceId,
    uint16_t            mtu
);

/*! *********************************************************************************
*
{brief  GattClient_RegisterProcedureCallback command monitoring function.
*
*
{param[in]    callback    Application defined callback to be triggered by GATT module.
*
********************************************************************************** */
void fsciBleGattClientRegisterProcedureCallbackCmdMonitor
(
    gattClientProcedureCallback_t callback
);

/*! *********************************************************************************
*
{brief  GattClient_RegisterNotificationCallback command monitoring function.
*
*
{param[in]    callback    Application defined callback to be triggered by GATT module.
*
********************************************************************************** */
void fsciBleGattClientRegisterNotificationCallbackCmdMonitor
(
    gattClientNotificationCallback_t callback
);

/*! *********************************************************************************
*
{brief  GattClient_RegisterIndicationCallback command monitoring function.
*
*
{param[in]    callback    Application defined callback to be triggered by GATT module.
*
********************************************************************************** */
void fsciBleGattClientRegisterIndicationCallbackCmdMonitor
(
    gattClientIndicationCallback_t callback
);

/*! *********************************************************************************
* \brief  GattClient_DiscoverAllPrimaryServices command monitoring function.
*
* \param[in]    deviceId                Device ID of the connected peer.
* \param[in]    aOutPrimaryServices     Statically allocated array of gattService_t.
* \param[in]    maxServiceCount         Maximum number of services to be filled.
* \param[in]    pOutDiscoveredCount     The actual number of services discovered.
*
********************************************************************************** */
void fsciBleGattClientDiscoverAllPrimaryServicesCmdMonitor
(
    deviceId_t      deviceId,
    gattService_t*  pOutPrimaryServices,
    uint8_t         maxServiceCount,
    uint8_t*        pOutDiscoveredCount
);

/*! *********************************************************************************
* \brief  GattClient_DiscoverPrimaryServicesByUuid command monitoring function.
*
* \param[in]    deviceId              Device ID of the connected peer.
* \param[in]    uuidType              Service UUID type.
* \param[in]    pUuid                 Service UUID.
* \param[in]    aOutPrimaryServices   Statically allocated array of gattService_t.
* \param[in]    maxServiceCount       Maximum number of services to be filled.
* \param[in]    pOutDiscoveredCount   The actual number of services discovered.
*
********************************************************************************** */
void fsciBleGattClientDiscoverPrimaryServicesByUuidCmdMonitor
(
    deviceId_t       deviceId,
    bleUuidType_t    uuidType,
    const bleUuid_t* pUuid,
    gattService_t*   aOutPrimaryServices,
    uint8_t          maxServiceCount,
    uint8_t*         pOutDiscoveredCount
);

/*! *********************************************************************************
* \brief  GattClient_DiscoverCharacteristicOfServiceByUuid command monitoring function.
*
* \param[in]    deviceId                  Device ID of the connected peer.
* \param[in]    uuidType                  Characteristic UUID type.
* \param[in]    pUuid                     Characteristic UUID.
* \param[in]    pService                  The service within which characteristics should be searched.
* \param[in]    aOutCharacteristics       The allocated array of Characteristics to be filled.
* \param[in]    maxCharacteristicCount    Maximum number of characteristics to be filled.
* \param[in]    pOutDiscoveredCount       The actual number of characteristics discovered.
*
********************************************************************************** */
void fsciBleGattClientDiscoverCharacteristicOfServiceByUuidCmdMonitor
(
    deviceId_t              deviceId,
    bleUuidType_t           uuidType,
    const bleUuid_t*        pUuid,
    const gattService_t*    pIoService,
    gattCharacteristic_t*   aOutCharacteristics,
    uint8_t                 maxCharacteristicCount,
    uint8_t*                pOutDiscoveredCount
);

/*! *********************************************************************************
* \brief  GattClient_DiscoverAllCharacteristicDescriptors command monitoring function.
*
* \param[in]    deviceId            Device ID of the connected peer.
* \param[in]    pIoCharacteristic   The characteristic within which descriptors should be searched.
* \param[in]    endingHandle        The last handle of the Characteristic.
* \param[in]    maxDescriptorCount  Maximum number of descriptors to be filled.
*
********************************************************************************** */
void fsciBleGattClientDiscoverAllCharacteristicDescriptorsCmdMonitor
(
    deviceId_t              deviceId,
    gattCharacteristic_t*   pIoCharacteristic,
    uint16_t                endingHandle,
    uint8_t                 maxDescriptorCount
);

/*! *********************************************************************************
* \brief  GattClient_ReadCharacteristicValue command monitoring function.
*
* \param[in]    deviceId            Device ID of the connected peer.
* \param[in]    pIoCharacteristic   The characteristic whose value must be read.
* \param[in]    maxReadBytes        Maximum number of bytes to be read.
*
********************************************************************************** */
void fsciBleGattClientReadCharValueCmdMonitor
(
    deviceId_t              deviceId,
    gattCharacteristic_t*   pIoCharacteristic,
    uint16_t                maxReadBytes
);

/*! *********************************************************************************
* \brief  GattClient_ReadUsingCharacteristicUuid command monitoring function.
*
* \param[in]    deviceId                Device ID of the connected peer.
* \param[in]    uuidType                Characteristic UUID type.
* \param[in]    pUuid                   Characteristic UUID.
* \param[in]    aOutBuffer              The allocated buffer to read into.
* \param[in]    maxReadBytes            Maximum number of bytes to be read.
* \param[in]    pOutActualReadBytes     The actual number of bytes read.
*
********************************************************************************** */
void fsciBleGattClientReadUsingCharacteristicUuidCmdMonitor
(
    deviceId_t          deviceId,
    bleUuidType_t       uuidType,
    const bleUuid_t*          pUuid,
    const gattHandleRange_t*  pHandleRange,
    uint8_t*            aOutBuffer,
    uint16_t            maxReadBytes,
    uint16_t*           pOutActualReadBytes
);

/*! *********************************************************************************
* \brief  GattClient_ReadMultipleCharacteristicValues command monitoring function.
*
* \param[in]    deviceId                Device ID of the connected peer.
* \param[in]    aIoCharacteristics      Array of the characteristics whose values
*                                       are to be read.
* \param[in]    cNumCharacteristics     Number of characteristics in the array.
*
********************************************************************************** */
void fsciBleGattClientReadMultipleCharacteristicValuesCmdMonitor
(
    deviceId_t              deviceId,
    uint8_t                 cNumCharacteristics,
    gattCharacteristic_t*   aIoCharacteristics
);

/*! *********************************************************************************
* \brief  GattClient_WriteCharacteristicValue command monitoring function.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  pCharacteristic           The characteristic whose value must be written.
* \param[in]  valueLength               Number of bytes to be written.
* \param[in]  aValue                    Array of bytes to be written.
* \param[in]  withoutResponse           Indicates if a Write Command will be used.
* \param[in]  signedWrite               Indicates if a Signed Write will be performed.
* \param[in]  doReliableLongCharWrites  Indicates Reliable Long Writes.
* \param[in]  aCsrk                     The CSRK (gcCsrkSize_d bytes).
*
********************************************************************************** */
void fsciBleGattClientWriteCharValueCmdMonitor
(
    deviceId_t                  deviceId,
    const gattCharacteristic_t* pCharacteristic,
    uint16_t                    valueLength,
    const uint8_t*              aValue,
    bool_t                      withoutResponse,
    bool_t                      signedWrite,
    bool_t                      doReliableLongCharWrites,
    const uint8_t*              aCsrk
);

/*! *********************************************************************************
* \brief  GattClient_ReadCharacteristicDescriptor command monitoring function.
*
* \param[in]  deviceId          Device ID of the connected peer.
* \param[in]  pIoDescriptor     The characteristic descriptor whose value must be read.
* \param[in]  maxReadBytes      Maximum number of bytes to be read.
*
********************************************************************************** */
void fsciBleGattClientReadCharacteristicDescriptorCmdMonitor
(
    deviceId_t             deviceId,
    gattAttribute_t* pIoDescriptor,
    uint16_t               maxReadBytes
);

/*! *********************************************************************************
* \brief  GattClient_ReadCharacteristicDescriptor command monitoring function.
*
* \param[in]  deviceId      Device ID of the connected peer.
* \param[in]  pDescriptor   The characteristic descriptor whose value must be written.
* \param[in]  valueLength   Number of bytes to be written.
* \param[in]  aValue        Array of bytes to be written.
*
********************************************************************************** */
void fsciBleGattClientWriteCharacteristicDescriptorCmdMonitor
(
    deviceId_t             deviceId,
    const gattAttribute_t* pDescriptor,
    uint16_t               valueLength,
    const uint8_t*         aValue
);

/*! *********************************************************************************
* \brief  GattClient_FindIncludedServices and GattClient_DiscoverAllCharacteristicsOfService
*         commands monitoring function.
*
* \param[in]    opCode      GATT event operation code.
* \param[in]    deviceId    Device ID of the connected peer.
* \param[in]    pIoService  The service within which inclusions or characteristics
*                           should be searched.
* \param[in]    maxCount    Maximum number of included services or characteristics.
*
********************************************************************************** */
void fsciBleGattClientFindIncludedServicesOrCharacteristicsCmdMonitor
(
    fsciBleGattOpCode_t opCode,
    deviceId_t          deviceId,
    gattService_t*      pIoService,
    uint8_t             maxCount
);

/*! *********************************************************************************
* \brief  GattServer_RegisterCallback command monitoring function.
*
* \param[in]    callback    Application-defined callback to be triggered by GATT module.
*
********************************************************************************** */
void fsciBleGattServerRegisterCallbackCmdMonitor
(
    gattServerCallback_t callback
);

/*! *********************************************************************************
* \brief  GattServer_RegisterHandlesForWriteNotifications and
*         GattServer_RegisterHandlesForReadNotifications commands monitoring function.
*
* \param[in]    opCode              GATT event operation code.
* \param[in]    handleCount         Number of handles in array.
* \param[in]    aAttributeHandles   Statically allocated array of handles.
*
********************************************************************************** */
void fsciBleGattServerRegisterHandlesForWriteOrReadNotificationsCmdMonitor
(
    fsciBleGattOpCode_t opCode,
    uint8_t             handleCount,
    const uint16_t*     aAttributeHandles
);

/*! *********************************************************************************
* \brief  GattServer_SendAttributeWrittenStatus and GattServer_SendAttributeReadStatus
*         commands monitoring function.
*
* \param[in]    opCode              GATT event operation code.
* \param[in]    deviceId            The device ID of the connected peer.
* \param[in]    attributeHandle     The attribute handle that was written.
* \param[in]    status              The status of the write operation.
*
********************************************************************************** */
void fsciBleGattServerSendAttributeWrittenOrReadStatusCmdMonitor
(
    fsciBleGattOpCode_t opCode,
    deviceId_t          deviceId,
    uint16_t            attributeHandle,
    uint8_t             status
);

/*! *********************************************************************************
* \brief  GattServer_SendNotification and GattServer_SendIndication commands
*         monitoring function.
*
* \param[in]    opCode      GATT event operation code.
* \param[in]    deviceId    The device ID of the connected peer.
* \param[in]    handle      Handle of the Value of the Characteristic to be notified
*                           or indicated.
*
********************************************************************************** */
void fsciBleGattServerSendNotificationOrIndicationCmdMonitor
(
    fsciBleGattOpCode_t opCode,
    deviceId_t          deviceId,
    uint16_t            handle
);

/*! *********************************************************************************
* \brief  GattServer_SendInstantValueNotification and GattServer_SendInstantValueIndication
*         commands monitoring function.
*
* \param[in]    opCode          GATT event operation code.
* \param[in]    deviceId        The device ID of the connected peer.
* \param[in]    handle          Handle of the Value of the Characteristic to be notified
*                               or indicated.
* \param[in]    valueLength     Length of data to be notified or indicated.
* \param[in]    pValue          Data to be notified or indicated.
*
********************************************************************************** */
void fsciBleGattServerSendInstantValueNotificationOrIndicationCmdMonitor
(
    fsciBleGattOpCode_t opCode,
    deviceId_t          deviceId,
    uint16_t            handle,
    uint16_t            valueLength,
    const uint8_t*      pValue
);

/*! *********************************************************************************
* \brief  GattServer_RegisterUniqueHandlesForNotifications command monitoring function.
*
* \param[in]    bWrite        Enable/Disable write notifications for unique value handles.
* \param[in]    bRead         Enable/Disable read notifications for unique value handles.
*
********************************************************************************** */
void fsciBleGattServerRegisterUniqueHandlesForNotificationsCmdMonitor
(
    bool_t bWrite,
    bool_t bRead
);

#endif /* gFsciBleHost_d || gFsciBleTest_d */
/*! *********************************************************************************
* \brief  Gatt_GetMtu out parameter monitoring function.
*
* \param[in]    pOutMtu     Pointer to integer to the MTU value.
*
********************************************************************************** */
void fsciBleGattGetMtuEvtMonitor
(
    uint16_t* pOutMtu
);

/*! *********************************************************************************
* \brief  gattClientProcedureCallback event monitoring function.
*
* \param[in]    deviceId            The device ID of the connected peer.
* \param[in]    procedureType       The procedure type.
* \param[in]    procedureResult     The procedure result.
* \param[in]    error               Error occurred.
*
********************************************************************************** */
void fsciBleGattClientProcedureEvtMonitor
(
    deviceId_t              deviceId,
    bearerId_t              bearerId,
    gattProcedureType_t     procedureType,
    gattProcedureResult_t   procedureResult,
    bleResult_t             error
);

/*! *********************************************************************************
* \brief  gattClientNotificationCallback and gattClientIndicationCallback events
*         monitoring function.
*
* \param[in]    opCode                      GATT event operation code.
* \param[in]    deviceId                    Device ID of the connected peer.
* \param[in]    characteristicValueHandle   Handle of the value of the characteristic
*                                           that is notified.
* \param[in]    aValue                      Characteristic value.
* \param[in]    valueLength                 Characteristic value length.
*
********************************************************************************** */
void fsciBleGattClientNotificationOrIndicationEvtMonitor
(
    fsciBleGattOpCode_t opCode,
    deviceId_t          deviceId,
    bearerId_t          bearerId,
    uint16_t            characteristicValueHandle,
    uint8_t*            aValue,
    uint16_t            valueLength
);

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
/*! *********************************************************************************
* \brief  GattClientMultipleValueNotificationCallback event monitoring function.
*
* \param[in]    opCode                      GATT event operation code.
* \param[in]    deviceId                    Device ID of the connected peer.
* \param[in]    aHandleLenValue             Array of handle, value length and value tuples.
* \param[in]    totalLength                 Total length of the array.
*
********************************************************************************** */
void fsciBleGattClientMultipleValueNotificationEvtMonitor(deviceId_t  deviceId, bearerId_t bearerId, uint8_t* aHandleLenValue, uint32_t totalLength);
#endif /* gBLE52_d */

/*! *********************************************************************************
* \brief  gattServerCallback event monitoring function.
*
* \param[in]    deviceId                    Device ID of the connected peer.
* \param[in]    bearerId                    Bearer ID of the ATT bearer used.
* \param[in]    pServerEvent                Server event information.
*
********************************************************************************** */
void fsciBleGattServerEvtMonitor
(
    deviceId_t          deviceId,
    bearerId_t          bearerId,
    gattServerEvent_t*  pServerEvent
);

#ifdef __cplusplus
}
#endif

#endif /* FSCI_BLE_ATT_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
