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

#ifndef FSCI_BLE_GATT_TYPES_H
#define FSCI_BLE_GATT_TYPES_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble.h"
#include "fsl_component_panic.h"
#include "gatt_types.h"
#include "gatt_interface.h"
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/

#define fsciBleGattClientGetAttributeBufferSize(pAttribute) \
        (sizeof(uint16_t) + sizeof(bleUuidType_t) +         \
        fsciBleGetUuidBufferSize((pAttribute)->uuidType) +  \
        sizeof(uint16_t) + sizeof(uint16_t) + (pAttribute)->valueLength)

#define fsciBleGattClientEraseTmpInfo(bAllocated, deviceId, bearerId)   \
        fsciBleGattClientErasePermanentOrTmpInfo((bAllocated), TRUE, deviceId, bearerId)

#define fsciBleGattClientEraseInfo(bAllocated, deviceId, bearerId)  \
        fsciBleGattClientErasePermanentOrTmpInfo((bAllocated), FALSE, deviceId, bearerId)

#define fsciBleGattServerGetCharacteristicBufferSize(pCharacteristic)   \
        fsciBleGattClientGetCharacteristicBufferSize(pCharacteristic)

#define fsciBleGattServerGetCharacteristicFromBuffer(pCharacteristic, ppBuffer) \
        fsciBleGattClientGetCharFromBuffer((pCharacteristic), (ppBuffer))

#define fsciBleGattServerGetBufferFromCharacteristic(pCharacteristic, ppBuffer) \
        fsciBleGattClientGetBufferFromCharacteristic((pCharacteristic), (ppBuffer))

#define fsciBleGattServerGetServerMtuChangedEventBufferSize(pServerMtuChangedEvent) \
        sizeof(uint16_t)

#define fsciBleGattServerGetServerMtuChangedEventFromBuffer(pServerMtuChangedEvent, ppBuffer)   \
        fsciBleGetUint16ValueFromBuffer((pServerMtuChangedEvent)->newMtu, *(ppBuffer))

#define fsciBleGattServerGetBufferFromServerMtuChangedEvent(pServerMtuChangedEvent, ppBuffer)   \
        fsciBleGetBufferFromUint16Value((pServerMtuChangedEvent)->newMtu, *(ppBuffer))

#define fsciBleGattServerGetServerAttributeWrittenEventBufferSize(pServerAttributeWrittenEvent) \
        (sizeof(uint16_t) + sizeof(uint16_t) + (uint32_t)(pServerAttributeWrittenEvent)->cValueLength)

#define fsciBleGattServerGetServerLongCharacteristicWrittenEventBufferSize(pServerLongCharacteristicWrittenEvent) \
        (sizeof(uint16_t) + sizeof(uint16_t) + (uint32_t)(pServerLongCharacteristicWrittenEvent)->cValueLength)

#define fsciBleGattServerGetServerLongCharacteristicWrittenEventBufferSize(pServerLongCharacteristicWrittenEvent) \
        (sizeof(uint16_t) + sizeof(uint16_t) + (uint32_t)(pServerLongCharacteristicWrittenEvent)->cValueLength)

#define fsciBleGattServerGetServerCccdWrittenEventBufferSize(pServerCccdWrittenEvent)   \
        (sizeof(uint16_t) + sizeof(gattCccdFlags_t))

#define fsciBleGattServerGetServerAttributeReadEventBufferSize(pServerAttributeReadEvent)   \
        sizeof(uint16_t)

#define fsciBleGattServerGetServerAttributeReadEventFromBuffer(pServerAttributeReadEvent, ppBuffer)   \
        fsciBleGetUint16ValueFromBuffer((pServerAttributeReadEvent)->handle, *(ppBuffer))

#define fsciBleGattServerGetBufferFromServerAttributeReadEvent(pServerAttributeReadEvent, ppBuffer)   \
        fsciBleGetBufferFromUint16Value((pServerAttributeReadEvent)->handle, *(ppBuffer))

#define fsciBleGattServerGetServerProcedureErrorBufferSize(pServerProcedureError)   \
        (sizeof(gattServerProcedureType_t) + sizeof(bleResult_t))

#define fsciBleGattServerFreeServerEvent(pServerEvent)  \
        MEM_BufferFree(pServerEvent)

#define fsciBleGattGetHandleRangeBufferSize(pHandleRange)    \
        (sizeof(uint16_t) + sizeof(uint16_t))

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

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

uint16_t* fsciBleGattClientAllocOutOrIoArraySize
(
    deviceId_t      deviceId,
    bearerId_t      bearerId
);

uint8_t* fsciBleGattClientAllocOutOrIoValue
(
    uint16_t        maxValueLength,
    deviceId_t      deviceId,
    bearerId_t      bearerId
);

gattAttribute_t* fsciBleGattClientAllocOutOrIoAttributes
(
    uint8_t         maxNbOfAtributes,
    deviceId_t      deviceId,
    bearerId_t      bearerId
);

void fsciBleGattClientGetAttributeFromBuffer
(
    gattAttribute_t*    pAttribute,
    uint8_t**           ppBuffer
);

void fsciBleGattClientGetBufferFromAttribute
(
    const gattAttribute_t*    pAttribute,
    uint8_t**           ppBuffer
);

gattCharacteristic_t* fsciBleGattClientAllocOutOrIoCharacteristics
(
    uint8_t    maxNbOfCharacteristics,
    deviceId_t deviceId,
    bearerId_t bearerId
);

uint32_t fsciBleGattClientGetCharacteristicBufferSize
(
    const gattCharacteristic_t* pCharacteristic
);

void fsciBleGattClientGetCharFromBuffer
(
    gattCharacteristic_t*   pCharacteristic,
    uint8_t**               ppBuffer
);

void fsciBleGattClientGetBufferFromCharacteristic
(
    const gattCharacteristic_t*   pCharacteristic,
    uint8_t**               ppBuffer
);

gattService_t* fsciBleGattClientAllocOutOrIoIncludedServices
(
    uint8_t    maxNbOfIncludedServices,
    deviceId_t deviceId,
    bearerId_t bearerId
);

gattService_t* fsciBleGattClientAllocOutOrIoServices
(
    uint8_t    maxNbOfServices,
    deviceId_t deviceId,
    bearerId_t bearerId
);

uint32_t fsciBleGattClientGetServiceBufferSize
(
    const gattService_t* pService
);

void fsciBleGattClientGetServiceFromBuffer
(
    gattService_t*  pService,
    uint8_t**       ppBuffer
);

void fsciBleGattClientGetBufferFromService
(
    const gattService_t*  pService,
    uint8_t**       ppBuffer
);

void fsciBleGattClientSaveServicesInfo
(
    deviceId_t     deviceId,
    bearerId_t     bearerId,
    gattService_t* pServices
);

void fsciBleGattClientSaveIncludedServicesInfo
(
    deviceId_t     deviceId,
    bearerId_t     bearerId,
    gattService_t* pIncludedServices
);

void fsciBleGattClientSaveCharacteristicsInfo
(
    deviceId_t            deviceId,
    bearerId_t            bearerId,
    gattCharacteristic_t* pCharacteristics
);

void fsciBleGattClientSaveDescriptorsInfo
(
    deviceId_t       deviceId,
    bearerId_t       bearerId,
    gattAttribute_t* pDescriptors
);

void fsciBleGattClientSaveValueInfo
(
    deviceId_t deviceId,
    bearerId_t bearerId,
    uint8_t*   pValue
);

void fsciBleGattClientSaveArraySizeInfo
(
    deviceId_t deviceId,
    bearerId_t bearerId,
    uint16_t*  pArraySize
);

gattService_t* fsciBleGattClientGetServicesInfo
(
    deviceId_t deviceId,
    bearerId_t bearerId,
    bool_t     bAllocated
);

gattService_t* fsciBleGattClientGetIncludedServicesInfo
(
    deviceId_t deviceId,
    bearerId_t bearerId,
    bool_t     bAllocated
);

gattCharacteristic_t* fsciBleGattClientGetCharInfo
(
    deviceId_t deviceId,
    bearerId_t bearerId,
    bool_t     bAllocated
);

gattAttribute_t* fsciBleGattClientGetDescriptorsInfo
(
    deviceId_t deviceId,
    bearerId_t bearerId,
    bool_t     bAllocated
);

uint8_t* fsciBleGattClientGetValueInfo
(
    deviceId_t deviceId,
    bearerId_t bearerId,
    bool_t     bAllocated
);

uint16_t* fsciBleGattClientGetArraySizeInfo
(
    deviceId_t deviceId,
    bearerId_t bearerId,
    bool_t     bAllocated
);

void fsciBleGattClientKeepInfo
(
    bool_t     bAllocatedInfo
);

void fsciBleGattClientErasePermanentOrTmpInfo
(
    bool_t     bAllocatedInfo,
    bool_t     bTmp,
    deviceId_t deviceId, 
    bearerId_t bearerId
);

void fsciBleGattServerGetServerAttributeWrittenEventFromBuffer
(
    gattServerAttributeWrittenEvent_t*  pServerAttributeWrittenEvent,
    uint8_t**                           ppBuffer
);

void fsciBleGattServerGetBufferFromServerAttributeWrittenEvent
(
    gattServerAttributeWrittenEvent_t*  pServerAttributeWrittenEvent,
    uint8_t**                           ppBuffer
);

void fsciBleGattServerGetServerLongCharacteristicWrittenEventFromBuffer
(
    gattServerLongCharacteristicWrittenEvent_t*	pServerLongCharacteristicWrittenEvent,
    uint8_t**                                   ppBuffer
);

void fsciBleGattServGetBuffFromServLongCharacteristicWrittenEvent
(
    gattServerLongCharacteristicWrittenEvent_t*	pServerLongCharacteristicWrittenEvent,
    uint8_t**                                   ppBuffer
);

void fsciBleGattServerGetServerCccdWrittenEventFromBuffer
(
    gattServerCccdWrittenEvent_t*   pServerCccdWrittenEvent,
    uint8_t**                       ppBuffer
);

void fsciBleGattServGetBuffFromServCccdWrittenEvent
(
    gattServerCccdWrittenEvent_t*   pServerCccdWrittenEvent,
    uint8_t**                       ppBuffer
);

void fsciBleGattServerGetServerProcedureErrorFromBuffer
(
    gattServerProcedureError_t* pServerProcedureError,
    uint8_t**                   ppBuffer
);

void fsciBleGattServGetBuffFromServProcedureError
(
    gattServerProcedureError_t* pServerProcedureError,
    uint8_t**                   ppBuffer
);

gattServerEvent_t* fsciBleGattServerAllocServerEventForBuffer
(
    gattServerEventType_t   eventType,
    uint8_t*                pBuffer
);

uint32_t fsciBleGattServerGetServerEventBufferSize
(
    gattServerEvent_t* pServerEvent
);

void fsciBleGattServGetServEventFromBuffer
(
    gattServerEvent_t*  pServerEvent,
    uint8_t**           ppBuffer
);

void fsciBleGattServGetBuffrFromServEvent
(
    gattServerEvent_t*  pServerEvent,
    uint8_t**           ppBuffer
);

void fsciBleGattGetHandleRangeFromBuffer
(
    gattHandleRange_t*  pHandleRange,
    uint8_t**           ppBuffer
);

void fsciBleGattGetBufferFromHandleRange
(
    const gattHandleRange_t*  pHandleRange,
    uint8_t**           ppBuffer
);

#ifdef __cplusplus
}
#endif

#endif /* FSCI_BLE_GATT_TYPES_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
