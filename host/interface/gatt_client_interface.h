/*! *********************************************************************************
 * \defgroup GATT_CLIENT GATT - Client APIs
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2018, 2020-2023, 2025 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef GATT_CLIENT_INTERFACE_H
#define GATT_CLIENT_INTERFACE_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "gatt_interface.h"

/************************************************************************************
*************************************************************************************
* Macros
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  Executes the basic Characteristic Write operation (with server confirmation).
*
* \param[in] deviceId       Device ID of the connected GATT Server.
* \param[in] pChar          Pointer to the Characteristic being written.
* \param[in] valueLength    Size in bytes of the value to be written.
* \param[in] aValue         Array of bytes to be written.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOverflow_c              TX queue for device is full.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
#define GattClient_SimpleCharacteristicWrite(deviceId, pChar, valueLength, aValue) \
    GattClient_WriteCharacteristicValue\
        (deviceId, pChar, valueLength, aValue, FALSE, FALSE, FALSE, NULL)

/*! *********************************************************************************
* \brief  Executes the Characteristic Write Without Response operation.
*
* \param[in] deviceId       Device ID of the connected GATT Server.
* \param[in] pChar          Pointer to the Characteristic being written.
* \param[in] valueLength    Size in bytes of the value to be written.
* \param[in] aValue         Array of bytes to be written.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOverflow_c              TX queue for device is full.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
#define GattClient_CharacteristicWriteWithoutResponse(deviceId, pChar, valueLength, aValue) \
    GattClient_WriteCharacteristicValue\
        (deviceId, pChar, valueLength, aValue, TRUE, FALSE, FALSE, NULL)

/*! *********************************************************************************
* \brief  Executes the Characteristic Signed Write Without Response operation.
*
* \param[in] deviceId       Device ID of the connected GATT Server.
* \param[in] pChar          Pointer to the Characteristic being written.
* \param[in] valueLength    Size in bytes of the value to be written.
* \param[in] aValue         Array of bytes to be written.
* \param[in] aCsrk          CSRK to be used for data signing.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOverflow_c              TX queue for device is full.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
#define GattClient_CharacteristicSignedWrite(deviceId, pChar, valueLength, aValue, aCsrk) \
    GattClient_WriteCharacteristicValue\
        (deviceId, pChar, valueLength, aValue, TRUE, TRUE, FALSE, aCsrk)

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/


/*! GATT Client Procedure Result type */
typedef enum {
    gGattProcSuccess_c  = 0x00U,     /*!< The procedure was completed successfully. */
    gGattProcError_c    = 0x01U      /*!< The procedure was terminated due to an error. */
} gattProcedureResult_t;

/*! GATT Client Procedure Callback type */
typedef void (*gattClientProcedureCallback_t)
(
    deviceId_t              deviceId,           /*!< Device ID identifying the active connection. */
    gattProcedureType_t     procedureType,      /*!< The type of the procedure that was completed. */
    gattProcedureResult_t   procedureResult,    /*!< The result of the completed procedure. */
    bleResult_t             error               /*!< If procedureResult is not gGattProcSuccess_c, this contains the error that terminated the procedure. */
);

/*! GATT Client Notification Callback prototype */
typedef void (*gattClientNotificationCallback_t)
(
    deviceId_t  deviceId,                   /*!< Device ID identifying the active connection. */
    uint16_t    characteristicValueHandle,  /*!< Handle of the Characteristic Value attribute to be notified. */
    uint8_t*    aValue,                     /*!< The Characteristic value array. */
    uint16_t    valueLength                 /*!< Value array size. */
);

/*! GATT Client Indication Callback prototype */
typedef gattClientNotificationCallback_t gattClientIndicationCallback_t;

/*! GATT Client Multiple Value Notification Callback prototype */
typedef void (*gattClientMultipleValueNotificationCallback_t)
(
    deviceId_t  deviceId,                   /*!< Device ID identifying the active connection. */
    uint8_t*    aHandleLenValue,            /*!< The array of handle, value length, value tuples. */
    uint32_t    totalLength                 /*!< Value array size. */
);

/*! GATT Client Enhanced Procedure Callback type */
typedef void (*gattClientEnhancedProcedureCallback_t)
(
    deviceId_t              deviceId,           /*!< Device ID identifying the active connection. */
    bearerId_t              bearerId,           /*!< Bearer ID identifying the Enhanced ATT bearer used. */
    gattProcedureType_t     procedureType,      /*!< The type of the procedure that was completed. */
    gattProcedureResult_t   procedureResult,    /*!< The result of the completed procedure. */
    bleResult_t             error               /*!< If procedureResult is not gGattProcSuccess_c, this contains the error that terminated the procedure. */
);

/*! GATT Client Enhanced Notification Callback prototype */
typedef void (*gattClientEnhancedNotificationCallback_t)
(
    deviceId_t  deviceId,                   /*!< Device ID identifying the active connection. */
    bearerId_t  bearerId,                   /*!< Bearer ID identifying the Enhanced ATT bearer used. */
    uint16_t    characteristicValueHandle,  /*!< Handle of the Characteristic Value attribute to be notified. */
    uint8_t*    aValue,                     /*!< The Characteristic value array. */
    uint16_t    valueLength                 /*!< Value array size. */
);

/*! GATT Client Enhanced Indication Callback prototype */
typedef gattClientEnhancedNotificationCallback_t gattClientEnhancedIndicationCallback_t;

/*! GATT Client Enhanced Multiple Value Notification Callback prototype */
typedef void (*gattClientEnhancedMultipleValueNotificationCallback_t)
(
    deviceId_t  deviceId,                   /*!< Device ID identifying the active connection. */
    bearerId_t  bearerId,                   /*!< Bearer ID Identifying the Enhanced ATT bearer used. */
    uint8_t*    aHandleLenValue,            /*!< The array of handle, value length, value tuples. */
    uint32_t    totalLength                 /*!< Value array size. */
);

extern procDataStruct_t* pProcedureData[];

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*
 *
 * Local Client Configuration
 *
 */

/*! *********************************************************************************
* \brief  Initializes the GATT Client functionality.
*
* \remarks This should be called once at device startup, if necessary.
*
* \remarks This function executes synchronously.
*
* \retval       gBleSuccess_c
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_Init(void);

/*! *********************************************************************************
* \brief  Resets any ongoing GATT Client Procedure.
*
* \remarks This function should be called if an ongoing Client procedure
* needs to be stopped.
*
* \retval       gBleSuccess_c
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_ResetProcedure(void);

/*! *********************************************************************************
* \brief  Installs the application callback for the GATT Client module Procedures.
*
* \param[in] callback   Application defined callback to be triggered by this module.
*
* \remarks This function executes synchronously.
*
* \retval       gBleSuccess_c
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_RegisterProcedureCallback
(
    gattClientProcedureCallback_t callback
);

/*! *********************************************************************************
* \brief  Installs the application callback for Server Notifications.
*
* \param[in] callback   Application defined callback to be triggered by this module.
*
* \remarks This function executes synchronously.
*
* \retval       gBleSuccess_c
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_RegisterNotificationCallback
(
    gattClientNotificationCallback_t callback
);

/*! *********************************************************************************
* \brief  Installs the application callback for Server Indications.
*
* \param[in] callback   Application defined callback to be triggered by this module.
*
* \remarks This function executes synchronously.
*
* \retval       gBleSuccess_c
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_RegisterIndicationCallback
(
    gattClientIndicationCallback_t callback
);

/*! *********************************************************************************
* \brief  Installs the application callback for Server Multiple Value Notification.
*
* \param[in] callback   Application defined callback to be triggered by this module.
*
* \remarks This function executes synchronously.
*
* \retval       gBleSuccess_c
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_RegisterMultipleValueNotificationCallback
(
    gattClientMultipleValueNotificationCallback_t callback
);
/*
 *
 * GATT Client Procedures
 *
 */

/*! *********************************************************************************
* \brief  Initializes the MTU Exchange procedure.
*
* \param[in] deviceId       Device ID of the connected peer.
* \param[in] mtu            Desired MTU size.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_ExchangeMtu
(
    deviceId_t      deviceId,
    uint16_t        mtu
);

/*! *********************************************************************************
* \brief  Initializes the Primary Service Discovery procedure.
*
* \param[in]  deviceId              Device ID of the connected peer.
* \param[out] aOutPrimaryServices   Statically allocated array of gattService_t. The GATT
*                                   module fills each Service's handle range and UUID.
* \param[in]  maxServiceCount       Maximum number of services to be filled.
* \param[out] pOutDiscoveredCount   The actual number of services discovered.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_DiscoverAllPrimaryServices
(
    deviceId_t      deviceId,
    gattService_t*  aOutPrimaryServices,
    uint8_t         maxServiceCount,
    uint8_t*        pOutDiscoveredCount
);

/*! *********************************************************************************
* \brief  Initializes the Primary Service Discovery By UUID procedure.
*
* \param[in]  deviceId              Device ID of the connected peer.
* \param[in]  uuidType              Service UUID type.
* \param[in]  pUuid                 Service UUID.
* \param[out] aOutPrimaryServices   Statically allocated array of gattService_t. The GATT
*                                   module fills each Service's handle range.
* \param[in]  maxServiceCount       Maximum number of services to be filled.
* \param[out] pOutDiscoveredCount   The actual number of services discovered.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_DiscoverPrimaryServicesByUuid
(
    deviceId_t         deviceId,
    bleUuidType_t      uuidType,
    const bleUuid_t*   pUuid,
    gattService_t*     aOutPrimaryServices,
    uint8_t            maxServiceCount,
    uint8_t*           pOutDiscoveredCount
);

/*! *********************************************************************************
* \brief  Initializes the Find Included Services procedure.
*
* \param[in]  deviceId              Device ID of the connected peer.
* \param[inout]  pIoService         The service within which inclusions should be searched.
*                                   The GATT module uses the Service's handle range and fills
*                                   the included Services' handle ranges, UUID types and
*                                   the UUIDs if they are 16-bit UUIDs.
* \param[in]  maxServiceCount       Maximum number of included services to be filled.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_FindIncludedServices
(
    deviceId_t      deviceId,
    gattService_t*  pIoService,
    uint8_t         maxServiceCount
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Discovery procedure for a given Service.
*
* \param[in]    deviceId                  Device ID of the connected peer.
* \param[inout] pIoService                The service within which characteristics should be searched.
*                                         The GATT module uses the Characteristic's range.
* \param[in]    maxCharacteristicCount    Maximum number of characteristics to be filled.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_DiscoverAllCharacteristicsOfService
(
    deviceId_t      deviceId,
    gattService_t*  pIoService,
    uint8_t         maxCharacteristicCount
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Discovery procedure for a given Service, with a given UUID.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  uuidType                  Characteristic UUID type.
* \param[in]  pUuid                     Characteristic UUID.
* \param[in]  pService                  The service within which characteristics should be searched.
* \param[out] aOutCharacteristics       The allocated array of Characteristics to be filled.
* \param[in]  maxCharacteristicCount    Maximum number of characteristics to be filled.
* \param[out] pOutDiscoveredCount       The actual number of characteristics discovered.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_DiscoverCharacteristicOfServiceByUuid
(
    deviceId_t              deviceId,
    bleUuidType_t           uuidType,
    const bleUuid_t*        pUuid,
    const gattService_t*    pService,
    gattCharacteristic_t*   aOutCharacteristics,
    uint8_t                 maxCharacteristicCount,
    uint8_t*                pOutDiscoveredCount
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Descriptor Discovery procedure for a given Characteristic.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[inout]  pIoCharacteristic      The characteristic within which descriptors should be searched.
*                                       The GATT module uses the Characteristic's handle and fills each
*                                       descriptor's handle and UUID.
* \param[in]  endingHandle              The last handle of the Characteristic.
* \param[in]  maxDescriptorCount        Maximum number of descriptors to be filled.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback. The endingHandle parameter should be known by the
* application if Characteristic Discovery was performed, i.e., if the next Characteristic
* declaration handle is known, then subtract 1 to obtain the endingHandle for the current Characteristic.
* If the last handle of the Characteristic is still unknown, set the endingHandle parameter to 0xFFFF.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_DiscoverAllCharacteristicDescriptors
(
    deviceId_t              deviceId,
    gattCharacteristic_t*   pIoCharacteristic,
    uint16_t                endingHandle,
    uint8_t                 maxDescriptorCount
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Read procedure for a given Characteristic.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[inout]  pIoCharacteristic      The characteristic whose value must be read.
*                                       The GATT module uses the value handle and
*                                       fills the value and length.
* \param[in]  maxReadBytes              Maximum number of bytes to be read.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_ReadCharacteristicValue
(
    deviceId_t              deviceId,
    gattCharacteristic_t*   pIoCharacteristic,
    uint16_t                maxReadBytes
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Read By UUID procedure.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  uuidType                  Characteristic UUID type.
* \param[in]  pUuid                     Characteristic UUID.
* \param[in]  pHandleRange              Handle range for the search or NULL. If
*                                       this is NULL, the search range is 0x0001-0xffff.
* \param[out] aOutBuffer                The allocated buffer to read into.
* \param[in]  maxReadBytes              Maximum number of bytes to be read.
* \param[out] pOutActualReadBytes       The actual number of bytes read.
*
* \remarks This procedure returns the Characteristics found within the specified range
* with the specified UUID. aOutBuffer will contain the Handle-Value pair length (1 byte),
* then Handle-Value pairs for all Characteristic Values found with the specified UUID.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_ReadUsingCharacteristicUuid
(
    deviceId_t                deviceId,
    bleUuidType_t             uuidType,
    const bleUuid_t*          pUuid,
    const gattHandleRange_t*  pHandleRange,
    uint8_t*                  aOutBuffer,
    uint16_t                  maxReadBytes,
    uint16_t*                 pOutActualReadBytes
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Read Multiple procedure.
*
* \param[in]    deviceId                  Device ID of the connected peer.
* \param[inout] aIoCharacteristics        Array of the characteristics whose values are to be read.
*                                         The GATT module uses each Characteristic's value handle and maxValueLength
*                                         fills each value and length.
* \param[in]    cNumCharacteristics       Number of characteristics in the array.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_ReadMultipleCharacteristicValues
(
    deviceId_t              deviceId,
    uint8_t                 cNumCharacteristics,
    gattCharacteristic_t*   aIoCharacteristics
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Write procedure for a given Characteristic.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  pCharacteristic           The characteristic whose value must be written.
*                                       The GATT module uses the value handle.
* \param[in]  valueLength               Number of bytes to be written.
* \param[in]  aValue                    Array of bytes to be written.
* \param[in]  withoutResponse           Indicates if a Write Command is used.
* \param[in]  signedWrite               Indicates if a Signed Write is performed.
* \param[in]  doReliableLongCharWrites  Indicates Reliable Long Writes.
* \param[in]  aCsrk                     The CSRK (gcCsrkSize_d bytes) if signedWrite is TRUE, ignored otherwise.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleOverflow_c              TX queue for device is full.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_WriteCharacteristicValue
(
    deviceId_t                    deviceId,
    const gattCharacteristic_t*   pCharacteristic,
    uint16_t                      valueLength,
    const uint8_t*                aValue,
    bool_t                        withoutResponse,
    bool_t                        signedWrite,
    bool_t                        doReliableLongCharWrites,
    const uint8_t*                aCsrk
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Descriptor Read procedure for a given Characteristic Descriptor.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[inout]  pIoDescriptor          The characteristic descriptor whose value must be read.
*                                       The GATT module uses the attribute's handle and fills
*                                       the attribute's value and length.
* \param[in]  maxReadBytes              Maximum number of bytes to be read.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_ReadCharacteristicDescriptor
(
    deviceId_t          deviceId,
    gattAttribute_t*    pIoDescriptor,
    uint16_t            maxReadBytes
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Descriptor Write procedure for a given Characteristic Descriptor.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  pDescriptor               The characteristic descriptor whose value must be written.
*                                       The GATT module uses the attribute's handle.
* \param[in]  valueLength               Number of bytes to be written.
* \param[in]  aValue                    Array of bytes to be written.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_WriteCharacteristicDescriptor
(
    deviceId_t                deviceId,
    const gattAttribute_t*    pDescriptor,
    uint16_t                  valueLength,
    const uint8_t*            aValue
);

/*! *********************************************************************************
* \brief  Initializes the Read Multiple Variable Length Characteristic Values procedure.
*
* \param[in]    deviceId                  Device ID of the connected peer.
* \param[in]    cNumCharacteristics       Number of characteristics in the array.
* \param[in]    pIoCharacteristics        Pointer to the array of the characteristics whose values are to be read.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_ReadMultipleVariableCharacteristicValues
(
    deviceId_t              deviceId,
    uint8_t                 cNumCharacteristics,
    gattCharacteristic_t*   pIoCharacteristics
);

#if defined(gBLE51_d) && (gBLE51_d == 1U)
#if defined(gGattCaching_d) && (gGattCaching_d == 1U)
/*! *********************************************************************************
* \brief  Update the gatt database hash value for the peer server.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  bondIdx                   Index of the bond in NVM or gcGapMaximumBondedDevices_d if peer is not bonded;
*                                       gInvalidNvmIndex_c if this is the first connection with the peer
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_GetDatabaseHash(deviceId_t deviceId, uint8_t bondIdx);
#endif /* gGattCaching_d */
#endif /* gBLE51_d */

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
#if (defined gEATT_d) && (gEATT_d == TRUE)
/*
 *
 * GATT Client Enhanced Procedures
 *
 */

/*! *********************************************************************************
* \brief  Installs the application callback for the GATT Client module Procedures on
*         Enhanced ATT bearers.
*
* \param[in] callback   Application defined callback to be triggered by this module.
*
* \remarks This function executes synchronously.
*
* \retval       gBleSuccess_c
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_RegisterEnhancedProcedureCallback
(
    gattClientEnhancedProcedureCallback_t callback
);

/*! *********************************************************************************
* \brief  Installs the application callback for Server Notifications.
*
* \param[in] callback   Application defined callback to be triggered by this module.
*
* \remarks This function executes synchronously.
*
* \retval       gBleSuccess_c
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_RegisterEnhancedNotificationCallback
(
    gattClientEnhancedNotificationCallback_t callback
);

/*! *********************************************************************************
* \brief  Installs the application callback for Server Indications.
*
* \param[in] callback   Application defined callback to be triggered by this module.
*
* \remarks This function executes synchronously.
*
* \retval       gBleSuccess_c
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_RegisterEnhancedIndicationCallback
(
    gattClientEnhancedIndicationCallback_t callback
);

/*! *********************************************************************************
* \brief  Installs the application callback for Server Multiple Value Notification.
*
* \param[in] callback   Application defined callback to be triggered by this module.
*
* \remarks This function executes synchronously.
*
* \retval       gBleSuccess_c
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_RegisterEnhancedMultipleValueNotificationCallback
(
    gattClientEnhancedMultipleValueNotificationCallback_t callback
);

/*! *********************************************************************************
* \brief  Initializes the Primary Service Discovery procedure.
*
* \param[in]  deviceId              Device ID of the connected peer.
* \param[in]  bearerId              Enhanced ATT bearer ID of the connected peer.
* \param[out] aOutPrimaryServices   Statically allocated array of gattService_t. The GATT
*                                   module fills each Service's handle range and UUID.
* \param[in]  maxServiceCount       Maximum number of services to be filled.
* \param[out] pOutDiscoveredCount   The actual number of services discovered.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedDiscoverAllPrimaryServices
(
    deviceId_t      deviceId,
    bearerId_t      bearerId,
    gattService_t*  aOutPrimaryServices,
    uint8_t         maxServiceCount,
    uint8_t*        pOutDiscoveredCount
);

/*! *********************************************************************************
* \brief  Initializes the Primary Service Discovery By UUID procedure.
*
* \param[in]  deviceId              Device ID of the connected peer.
* \param[in]  bearerId              Enhanced ATT bearer ID of the connected peer.
* \param[in]  uuidType              Service UUID type.
* \param[in]  pUuid                 Service UUID.
* \param[out] aOutPrimaryServices   Statically allocated array of gattService_t. The GATT
*                                   module fills each Service's handle range.
* \param[in]  maxServiceCount       Maximum number of services to be filled.
* \param[out] pOutDiscoveredCount   The actual number of services discovered.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedDiscoverPrimaryServicesByUuid
(
    deviceId_t         deviceId,
    bearerId_t         bearerId,
    bleUuidType_t      uuidType,
    const bleUuid_t*   pUuid,
    gattService_t*     aOutPrimaryServices,
    uint8_t            maxServiceCount,
    uint8_t*           pOutDiscoveredCount
);

/*! *********************************************************************************
* \brief  Initializes the Find Included Services procedure.
*
* \param[in]  deviceId              Device ID of the connected peer.
* \param[in]  bearerId              Enhanced ATT bearer ID of the connected peer.
* \param[inout]  pIoService         The service within which inclusions should be searched.
*                                   The GATT module uses the Service's handle range and fills
*                                   the included Services' handle ranges, UUID types and
*                                   the UUIDs if they are 16-bit UUIDs.
* \param[in]  maxServiceCount       Maximum number of included services to be filled.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedFindIncludedServices
(
    deviceId_t      deviceId,
    bearerId_t      bearerId,
    gattService_t*  pIoService,
    uint8_t         maxServiceCount
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Discovery procedure for a given Service.
*
* \param[in]    deviceId                  Device ID of the connected peer.
* \param[in]    bearerId                  Enhanced ATT bearer ID of the connected peer.
* \param[inout] pIoService                The service within which characteristics should be searched.
*                                         The GATT module uses the Characteristic's range.
* \param[in]    maxCharacteristicCount    Maximum number of characteristics to be filled.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedDiscoverAllCharacteristicsOfService
(
    deviceId_t      deviceId,
    bearerId_t      bearerId,
    gattService_t*  pIoService,
    uint8_t         maxCharacteristicCount
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Discovery procedure for a given Service, with a given UUID.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  bearerId                  Enhanced ATT bearer ID of the connected peer.
* \param[in]  uuidType                  Characteristic UUID type.
* \param[in]  pUuid                     Characteristic UUID.
* \param[in]  pService                  The service within which characteristics should be searched.
* \param[out] aOutCharacteristics       The allocated array of Characteristics to be filled.
* \param[in]  maxCharacteristicCount    Maximum number of characteristics to be filled.
* \param[out] pOutDiscoveredCount       The actual number of characteristics discovered.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedDiscoverCharacteristicOfServiceByUuid
(
    deviceId_t              deviceId,
    bearerId_t              bearerId,
    bleUuidType_t           uuidType,
    const bleUuid_t*        pUuid,
    const gattService_t*    pService,
    gattCharacteristic_t*   aOutCharacteristics,
    uint8_t                 maxCharacteristicCount,
    uint8_t*                pOutDiscoveredCount
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Descriptor Discovery procedure for a given Characteristic.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  bearerId                  Enhanced ATT bearer ID of the connected peer.
* \param[inout]  pIoCharacteristic      The characteristic within which descriptors should be searched.
*                                       The GATT module uses the Characteristic's handle and fills each
*                                       descriptor's handle and UUID.
* \param[in]  endingHandle              The last handle of the Characteristic.
* \param[in]  maxDescriptorCount        Maximum number of descriptors to be filled.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback. The endingHandle parameter should be known by the
* application if Characteristic Discovery was performed, i.e., if the next Characteristic
* declaration handle is known, then subtract 1 to obtain the endingHandle for the current Characteristic.
* If the last handle of the Characteristic is still unknown, set the endingHandle parameter to 0xFFFF.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedDiscoverAllCharacteristicDescriptors
(
    deviceId_t              deviceId,
    bearerId_t              bearerId,
    gattCharacteristic_t*   pIoCharacteristic,
    uint16_t                endingHandle,
    uint8_t                 maxDescriptorCount
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Read procedure for a given Characteristic.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  bearerId                  Enhanced ATT bearer ID of the connected peer.
* \param[inout]  pIoCharacteristic      The characteristic whose value must be read.
*                                       The GATT module uses the value handle and
*                                       fills the value and length.
* \param[in]  maxReadBytes              Maximum number of bytes to be read.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedReadCharacteristicValue
(
    deviceId_t              deviceId,
    bearerId_t              bearerId,
    gattCharacteristic_t*   pIoCharacteristic,
    uint16_t                maxReadBytes
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Read By UUID procedure.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  bearerId                  Enhanced ATT bearer ID of the connected peer.
* \param[in]  uuidType                  Characteristic UUID type.
* \param[in]  pUuid                     Characteristic UUID.
* \param[in]  pHandleRange              Handle range for the search or NULL. If
*                                       this is NULL, the search range is 0x0001-0xffff.
* \param[out] aOutBuffer                The allocated buffer to read into.
* \param[in]  maxReadBytes              Maximum number of bytes to be read.
* \param[out] pOutActualReadBytes       The actual number of bytes read.
*
* \remarks This procedure returns the Characteristics found within the specified range
* with the specified UUID. aOutBuffer will contain the Handle-Value pair length (1 byte),
* then Handle-Value pairs for all Characteristic Values found with the specified UUID.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedReadUsingCharacteristicUuid
(
    deviceId_t                deviceId,
    bearerId_t                bearerId,
    bleUuidType_t             uuidType,
    const bleUuid_t*          pUuid,
    const gattHandleRange_t*  pHandleRange,
    uint8_t*                  aOutBuffer,
    uint16_t                  maxReadBytes,
    uint16_t*                 pOutActualReadBytes
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Read Multiple procedure.
*
* \param[in]    deviceId                  Device ID of the connected peer.
* \param[in]    bearerId                  Enhanced ATT bearer ID of the connected peer.
* \param[inout] aIoCharacteristics        Array of the characteristics whose values are to be read.
*                                         The GATT module uses each Characteristic's value handle and maxValueLength
*                                         fills each value and length.
* \param[in]    cNumCharacteristics       Number of characteristics in the array.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedReadMultipleCharacteristicValues
(
    deviceId_t              deviceId,
    bearerId_t              bearerId,
    uint8_t                 cNumCharacteristics,
    gattCharacteristic_t*   aIoCharacteristics
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Write procedure for a given Characteristic.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  bearerId                  Enhanced ATT bearer ID of the connected peer.
* \param[in]  pCharacteristic           The characteristic whose value must be written.
*                                       The GATT module uses the value handle.
* \param[in]  valueLength               Number of bytes to be written.
* \param[in]  aValue                    Array of bytes to be written.
* \param[in]  withoutResponse           Indicates if a Write Command is used.
* \param[in]  signedWrite               Indicates if a Signed Write is performed.
* \param[in]  doReliableLongCharWrites  Indicates Reliable Long Writes.
* \param[in]  aCsrk                     The CSRK (gcCsrkSize_d bytes) if signedWrite is TRUE, ignored otherwise.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOverflow_c              TX queue for device is full.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedWriteCharacteristicValue
(
    deviceId_t                    deviceId,
    bearerId_t                    bearerId,
    const gattCharacteristic_t*   pCharacteristic,
    uint16_t                      valueLength,
    const uint8_t*                aValue,
    bool_t                        withoutResponse,
    bool_t                        doReliableLongCharWrites,
    const uint8_t*                aCsrk
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Descriptor Read procedure for a given Characteristic Descriptor.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  bearerId                  Enhanced ATT bearer ID of the connected peer.
* \param[inout]  pIoDescriptor          The characteristic descriptor whose value must be read.
*                                       The GATT module uses the attribute's handle and fills
*                                       the attribute's value and length.
* \param[in]  maxReadBytes              Maximum number of bytes to be read.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedReadCharacteristicDescriptor
(
    deviceId_t          deviceId,
    bearerId_t          bearerId,
    gattAttribute_t*    pIoDescriptor,
    uint16_t            maxReadBytes
);

/*! *********************************************************************************
* \brief  Initializes the Characteristic Descriptor Write procedure for a given Characteristic Descriptor.
*
* \param[in]  deviceId                  Device ID of the connected peer.
* \param[in]  bearerId                  Enhanced ATT bearer ID of the connected peer.
* \param[in]  pDescriptor               The characteristic descriptor whose value must be written.
*                                       The GATT module uses the attribute's handle.
* \param[in]  valueLength               Number of bytes to be written.
* \param[in]  aValue                    Array of bytes to be written.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedWriteCharacteristicDescriptor
(
    deviceId_t                deviceId,
    bearerId_t                bearerId,
    const gattAttribute_t*    pDescriptor,
    uint16_t                  valueLength,
    const uint8_t*            aValue
);

/*! *********************************************************************************
* \brief  Initializes the Read Multiple Variable Length Characteristic Values procedure.
*
* \param[in]    deviceId                  Device ID of the connected peer.
* \param[in]    bearerId                  Enhanced ATT bearer ID of the connected peer.
* \param[in]    cNumCharacteristics       Number of characteristics in the array.
* \param[in]    pIoCharacteristics        Pointer to the array of the characteristics whose values are to be read.
*
* \remarks If gBleSuccess_c is returned, the completion of this procedure is signaled
* by the Client Enhanced Procedure callback.
*
* \retval       gBleSuccess_c
* \retval       gBleInvalidParameter_c      An invalid parameter was provided.
* \retval       gBleOutOfMemory_c           Could not allocate message for Host task.
* \retval       gBleFeatureNotSupported_c   Host library was compiled without GATT client support.
*
********************************************************************************** */
bleResult_t GattClient_EnhancedReadMultipleVariableCharacteristicValues
(
    deviceId_t              deviceId,
    bearerId_t              bearerId,
    uint8_t                 cNumCharacteristics,
    gattCharacteristic_t*   pIoCharacteristics
);

#endif /* gEATT_d */
#endif /* gBLE52_d */
#ifdef __cplusplus
}
#endif

#endif /* GATT_CLIENT_INTERFACE_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
