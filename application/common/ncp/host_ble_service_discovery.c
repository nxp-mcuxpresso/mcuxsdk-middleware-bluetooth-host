/*! *********************************************************************************
* Copyright 2016-2021, 2023 - 2024 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_panic.h"
#include "ble_general.h"
#include "gap_types.h"
#include "gatt_types.h"

#include "host_ble_service_discovery.h"
#include "ble_config.h"
#include "host_cmd_ble.h"
#include "host_hsdk_interface.h"

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static void BleServDisc_Reset(deviceId_t peerDeviceId);
STATIC void BleServDisc_NewService(deviceId_t peerDeviceId, gattService_t *pService);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static servDiscCallback_t pfServDiscCallback = NULL;
servDiscInfo_t maServDiscInfo[gAppMaxConnections_c];

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/***********************************************************************************
*\fn           void BleServDisc_RegisterCallback(
*                  servDiscCallback_t pServDiscCallback)
*\brief        Installs an application callback for the Service Discovery module.
*
*\param  [in]  pServDiscCallback         Pointer to service discovery callback.
*
*\retval       void.
********************************************************************************** */
void BleServDisc_RegisterCallback(servDiscCallback_t pServDiscCallback)
{
    pfServDiscCallback = pServDiscCallback;
}

/*! *********************************************************************************
*\fn           bleResult_t BleServDisc_Start(deviceId_t peerDeviceId)
*\brief        Starts the Service Discovery procedure with the peer device.
*
*\param  [in]  peerDeviceId      The GAP peer Id.
*
*\return       bleResult_t       Result of the operation.
********************************************************************************** */
bleResult_t BleServDisc_Start(deviceId_t peerDeviceId)
{
    bleResult_t result = gBleSuccess_c;

    if (!maServDiscInfo[peerDeviceId].mServDiscInProgress)
    {
        /* Allocate memory for Service Discovery */
        maServDiscInfo[peerDeviceId].mpServiceDiscoveryBuffer =
                 MEM_BufferAlloc(sizeof(gattService_t) *
                                 (uint32_t)gMaxServicesCount_d);

        maServDiscInfo[peerDeviceId].mpCharDiscoveryBuffer =
                 MEM_BufferAlloc(sizeof(gattCharacteristic_t) *
                                 (uint32_t)gMaxServiceCharCount_d);

        maServDiscInfo[peerDeviceId].mpCharDescriptorBuffer =
                 MEM_BufferAlloc(sizeof(gattAttribute_t) *
                                 (uint32_t)gMaxCharDescriptorsCount_d);

        if (maServDiscInfo[peerDeviceId].mpServiceDiscoveryBuffer != NULL &&
            maServDiscInfo[peerDeviceId].mpCharDiscoveryBuffer != NULL &&
            maServDiscInfo[peerDeviceId].mpCharDescriptorBuffer != NULL)
        {
            maServDiscInfo[peerDeviceId].mServDiscInProgress = TRUE;

            /* Start Service Discovery*/
            GATTClientDiscoverAllPrimaryServicesRequest_t req;
            req.DeviceId = peerDeviceId;
            req.MaxNbOfServices = gMaxServicesCount_d;
            (void)GATTClientDiscoverAllPrimaryServicesRequest(&req, gFsciInterface_c);
        }
        else
        {
            BleServDisc_Reset(peerDeviceId);
            result = gBleOutOfMemory_c;
        }
    }
    else
    {
        result = gBleInvalidState_c;
    }
    return result;
}

/*! *********************************************************************************
*\fn           bleResult_t BleServDisc_FindService(deviceId_t    peerDeviceId,
*                                                  bleUuidType_t uuidType,
*                                                  bleUuid_t     *pUuid)
*\brief        Starts the Service Discovery procedure for a specified service UUID,
*              with the peer device.
*
*\param  [in]  peerDeviceId       The GAP peer Id.
*\param  [in]  uuidType           Service UUID type.
*\param  [in]  pUuid              Service UUID.
*
*\return       bleResult_t        Result of the operation.
********************************************************************************** */
bleResult_t BleServDisc_FindService
(
    deviceId_t    peerDeviceId,
    bleUuidType_t uuidType,
    bleUuid_t     *pUuid
)
{
    bleResult_t result = gBleSuccess_c;

    if (!maServDiscInfo[peerDeviceId].mServDiscInProgress)
    {
        /* Allocate memory for Service Discovery */
        maServDiscInfo[peerDeviceId].mpServiceDiscoveryBuffer =
                  MEM_BufferAlloc(sizeof(gattService_t));

        maServDiscInfo[peerDeviceId].mpCharDiscoveryBuffer =
                  MEM_BufferAlloc(sizeof(gattCharacteristic_t) *
                                  (uint32_t)gMaxServiceCharCount_d);

        maServDiscInfo[peerDeviceId].mpCharDescriptorBuffer =
                  MEM_BufferAlloc(sizeof(gattAttribute_t) *
                                  (uint32_t)gMaxCharDescriptorsCount_d);

        if (maServDiscInfo[peerDeviceId].mpServiceDiscoveryBuffer != NULL &&
            maServDiscInfo[peerDeviceId].mpCharDiscoveryBuffer != NULL &&
            maServDiscInfo[peerDeviceId].mpCharDescriptorBuffer != NULL)
        {
            maServDiscInfo[peerDeviceId].mServDiscInProgress = TRUE;

            /* Start Service Discovery*/
            GATTClientDiscoverPrimaryServicesByUuidRequest_t req;
            req.DeviceId = peerDeviceId;
            req.UuidType = (UuidType_t)uuidType;
            if (uuidType == Uuid16Bits)
            {
                Utils_PackTwoByteValue(pUuid->uuid16, req.Uuid.Uuid16Bits);
            }
            else
            {
                FLib_MemCpy(req.Uuid.Uuid128Bits, pUuid->uuid128, 16);
            }
                      
            req.MaxNbOfServices = 1;
            (void)GATTClientDiscoverPrimaryServicesByUuidRequest(&req, gFsciInterface_c);
        }
        else
        {
            BleServDisc_Reset(peerDeviceId);
            result = gBleOutOfMemory_c;
        }
    }
    else
    {
        result = gBleInvalidState_c;
    }
    return result;
}

/*! *********************************************************************************
*\fn           void BleServDisc_Stop(deviceId_t peerDeviceId)
*\brief        Stops the Service Discovery procedure with the peer device.
*
*\param  [in]  peerDeviceId      The GAP peer Id.
*
*\retval       void.
********************************************************************************** */
void BleServDisc_Stop(deviceId_t peerDeviceId)
{
    if (maServDiscInfo[peerDeviceId].mServDiscInProgress)
    {
        maServDiscInfo[peerDeviceId].mServDiscInProgress = FALSE;
        BleServDisc_Reset(peerDeviceId);
    }
}

/*! *********************************************************************************
*\fn             void BleServDisc_SignalGattClientEvent(
*                    deviceId_t              peerDeviceId,
*                    gattProcedureType_t     procedureType,
*                    gattProcedureResult_t   procedureResult,
*                    bleResult_t             error)
*\brief          Signals the module a GATT client callback from host stack.
*                Must be called by the application, which is responsible for.
*
*\param  [in]    peerDeviceId        GATT Server device ID.
*\param  [in]    procedureType       Procedure type.
*\param  [in]    procedureResult     Procedure result.
*\param  [in]    error               Callback result.
*
*\retval         void.
********************************************************************************** */
void BleServDisc_SignalGattClientEvent
(
    deviceId_t              peerDeviceId,
    gattProcedureType_t     procedureType,
    gattProcedureResult_t   procedureResult,
    bleResult_t             error
)
{
    servDiscInfo_t  *pInfo = &maServDiscInfo[peerDeviceId];

    if (pInfo->mServDiscInProgress)
    {
        if (procedureResult == gGattProcError_c)
        {
            BleServDisc_Finished(peerDeviceId, FALSE);
        }
        else
        {
            switch (procedureType)
            {
                case gGattProcDiscoverPrimaryServicesByUuid_c:
                /* Fall through */
                case gGattProcDiscoverAllPrimaryServices_c:
                {
                    /* We found at least one service */
                    if (pInfo->mcPrimaryServices != 0U)
                    {
                        /* Start characteristic discovery with first service*/
                        pInfo->mCurrentServiceInDiscoveryIndex = 0U;
                        pInfo->mCurrentCharInDiscoveryIndex = 0U;
                        pInfo->mCurrentDescInDiscoveryIndex = 0U;

                        pInfo->mpServiceDiscoveryBuffer->aCharacteristics =
                                 pInfo->mpCharDiscoveryBuffer;
                        
                        GATTClientDiscoverAllCharacteristicsOfServiceRequest_t req;
                        req.DeviceId = peerDeviceId;
                        req.MaxNbOfCharacteristics = gMaxServiceCharCount_d;
                        req.Service.StartHandle = pInfo->mpServiceDiscoveryBuffer->startHandle;
                        req.Service.EndHandle = pInfo->mpServiceDiscoveryBuffer->endHandle;
                        req.Service.UuidType  = (UuidType_t)pInfo->mpServiceDiscoveryBuffer->uuidType;
                        if (pInfo->mpServiceDiscoveryBuffer->uuidType == Uuid16Bits)
                        {
                            Utils_PackTwoByteValue(pInfo->mpServiceDiscoveryBuffer->uuid.uuid16, req.Service.Uuid.Uuid16Bits);
                        }
                        else
                        {
                            FLib_MemCpy(req.Service.Uuid.Uuid128Bits, pInfo->mpServiceDiscoveryBuffer->uuid.uuid128, 16U);
                        }
                        req.Service.NbOfCharacteristics = pInfo->mpServiceDiscoveryBuffer->cNumCharacteristics;
                        req.Service.Characteristics = (void*)pInfo->mpServiceDiscoveryBuffer->aCharacteristics,                          
                        req.Service.NbOfIncludedServices = pInfo->mpServiceDiscoveryBuffer->cNumIncludedServices;
                        req.Service.IncludedServices = (void*)pInfo->mpServiceDiscoveryBuffer->aIncludedServices;
                        GATTClientDiscoverAllCharacteristicsOfServiceRequest(&req, gFsciInterface_c);
                    }
                    else
                    { 
                        BleServDisc_Finished(peerDeviceId, TRUE);  
                    }
                }
                break;

                case gGattProcDiscoverAllCharacteristicDescriptors_c:
                /* Fall through */
                case gGattProcDiscoverAllCharacteristics_c:
                {
                    if (procedureType == gGattProcDiscoverAllCharacteristicDescriptors_c)
                    {
                        gattService_t        *pCurrentService =
                                                 pInfo->mpServiceDiscoveryBuffer +
                                                 pInfo->mCurrentServiceInDiscoveryIndex;

                        gattCharacteristic_t *pCurrentChar    =
                                                 pCurrentService->aCharacteristics +
                                                 pInfo->mCurrentCharInDiscoveryIndex;

                        pInfo->mCurrentDescInDiscoveryIndex +=
                                                 pCurrentChar->cNumDescriptors;

                        /* Move on to the next characteristic */
                        pInfo->mCurrentCharInDiscoveryIndex++;
                    }

                    gattService_t *pCurrentService = pInfo->mpServiceDiscoveryBuffer +
                                                     pInfo->mCurrentServiceInDiscoveryIndex;

                    if (pInfo->mCurrentCharInDiscoveryIndex < pCurrentService->cNumCharacteristics)
                    {
                        gattCharacteristic_t *pCurrentChar =
                                                 pCurrentService->aCharacteristics +
                                                 pInfo->mCurrentCharInDiscoveryIndex;

                        /* Find next characteristic with descriptors*/
                        while (pInfo->mCurrentCharInDiscoveryIndex < pCurrentService->cNumCharacteristics - 1U)
                        {
                            /* Check if we have handles available between adjacent characteristics */
                            if (pCurrentChar->value.handle + 2U < (pCurrentChar + 1)->value.handle)
                            {
                                if (pInfo->mCurrentDescInDiscoveryIndex < (uint8_t)gMaxCharDescriptorsCount_d)
                                {
                                    pCurrentChar->aDescriptors = pInfo->mpCharDescriptorBuffer +
                                                                 pInfo->mCurrentDescInDiscoveryIndex;
                                    
                                    GATTClientDiscoverAllCharacteristicDescriptorsRequest_t req;
                                    req.DeviceId = peerDeviceId;
                                    req.Characteristic.Value.Handle = pCurrentChar->value.handle;
                                    req.Characteristic.Value.UuidType = (UuidType_t)pCurrentChar->value.uuidType;
                                    if (req.Characteristic.Value.UuidType == Uuid16Bits)
                                    {
                                        FLib_MemCpy(&req.Characteristic.Value.Uuid.Uuid16Bits,
                                                    &pCurrentChar->value.uuid.uuid16,
                                                    sizeof(uint16_t));
                                    }
                                    else
                                    {
                                        FLib_MemCpy(&req.Characteristic.Value.Uuid.Uuid128Bits,
                                                    &pCurrentChar->value.uuid.uuid128,
                                                    16U);
                                    }
                                    req.Characteristic.Value.ValueLength = pCurrentChar->value.valueLength;
                                    req.Characteristic.Value.MaxValueLength = pCurrentChar->value.maxValueLength;
                                    req.EndingHandle = (pCurrentChar + 1U)->value.handle;
                                    req.MaxNbOfDescriptors = (uint8_t)(gMaxCharDescriptorsCount_d - pInfo->mCurrentDescInDiscoveryIndex);
                                    GATTClientDiscoverAllCharacteristicDescriptorsRequest(&req, gFsciInterface_c);
                                    return;
                                }
                            }

                            pInfo->mCurrentCharInDiscoveryIndex++;
                            pCurrentChar = pCurrentService->aCharacteristics +
                                           pInfo->mCurrentCharInDiscoveryIndex;
                        }

                        /* Made it to the last characteristic. Check against service end handle*/
                        if (pCurrentChar->value.handle < pCurrentService->endHandle)
                        {
                            if (pInfo->mCurrentDescInDiscoveryIndex < gMaxCharDescriptorsCount_d)
                            {
                                pCurrentChar->aDescriptors = pInfo->mpCharDescriptorBuffer +
                                                             pInfo->mCurrentDescInDiscoveryIndex;
                                
                                GATTClientDiscoverAllCharacteristicDescriptorsRequest_t req;
                                req.DeviceId = peerDeviceId;
                                req.Characteristic.Properties = (Properties_t)pCurrentChar->properties;
                                req.Characteristic.NbOfDescriptors = pCurrentChar->cNumDescriptors;
                                req.Characteristic.Value.Handle = pCurrentChar->value.handle;
                                req.Characteristic.Value.UuidType = (UuidType_t)pCurrentChar->value.uuidType;
                                if (req.Characteristic.Value.UuidType == Uuid16Bits)
                                {
                                    FLib_MemCpy(&req.Characteristic.Value.Uuid.Uuid16Bits,
                                                &pCurrentChar->value.uuid.uuid16,
                                                sizeof(uint16_t));
                                }
                                else
                                {
                                    FLib_MemCpy(&req.Characteristic.Value.Uuid.Uuid128Bits,
                                                &pCurrentChar->value.uuid.uuid128,
                                                16U);
                                }
                                req.Characteristic.Value.ValueLength = pCurrentChar->value.valueLength;
                                req.Characteristic.Value.MaxValueLength = pCurrentChar->value.maxValueLength;
                                
                                req.EndingHandle = pCurrentService->endHandle;
                                req.MaxNbOfDescriptors = (uint8_t)(gMaxCharDescriptorsCount_d - pInfo->mCurrentDescInDiscoveryIndex);
                                GATTClientDiscoverAllCharacteristicDescriptorsRequest(&req, gFsciInterface_c);
                                return;
                            }

                        }
                    }

                    /* Signal Discovery of Service */
                    BleServDisc_NewService(peerDeviceId, pCurrentService);

                    /* Move on to the next service */
                    pInfo->mCurrentServiceInDiscoveryIndex++;

                    /* Reset characteristic discovery */
                    pInfo->mCurrentCharInDiscoveryIndex = 0U;
                    pInfo->mCurrentDescInDiscoveryIndex = 0U;
                    FLib_MemSet(pInfo->mpCharDescriptorBuffer,
                                0U,
                                sizeof(gattAttribute_t) * (uint32_t)gMaxCharDescriptorsCount_d);
                    FLib_MemSet(pInfo->mpCharDiscoveryBuffer,
                                0U,
                                sizeof(gattCharacteristic_t) * (uint32_t)gMaxServiceCharCount_d);

                    if (pInfo->mCurrentServiceInDiscoveryIndex < pInfo->mcPrimaryServices)
                    {
                        /* Allocate memory for Char Discovery */
                        (pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex)->aCharacteristics =
                                                               pInfo->mpCharDiscoveryBuffer;

                         /* Start Characteristic Discovery for current service */
                         GATTClientDiscoverAllCharacteristicsOfServiceRequest_t req;
                         req.DeviceId = peerDeviceId;
                         req.MaxNbOfCharacteristics = gMaxServiceCharCount_d;
                         req.Service.StartHandle = (pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex)->startHandle;
                         req.Service.EndHandle = (pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex)->endHandle;
                         req.Service.UuidType  = (UuidType_t)(pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex)->uuidType;
                         if ((pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex)->uuidType == Uuid16Bits)
                         {
                             Utils_PackTwoByteValue((pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex)->uuid.uuid16,
                                                    req.Service.Uuid.Uuid16Bits);
                         }
                         else
                         {
                             FLib_MemCpy(req.Service.Uuid.Uuid128Bits,
                                         (pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex)->uuid.uuid128,
                                         16U);
                         }
                         req.Service.NbOfCharacteristics = (pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex)->cNumCharacteristics;
                         req.Service.Characteristics = (void*)(pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex)->aCharacteristics,                          
                         req.Service.NbOfIncludedServices = (pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex)->cNumIncludedServices;
                         req.Service.IncludedServices = (void*)(pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex)->aIncludedServices;
                         GATTClientDiscoverAllCharacteristicsOfServiceRequest(&req, gFsciInterface_c);
                    }
                    else
                    {
                        BleServDisc_Finished(peerDeviceId, TRUE);
                    }
                }
                break;

                default:
                {
                    ; /* No action required */
                }
                break;
            }
        }
    }
}

/*! *********************************************************************************
* \brief    Stops the Service Discovery procedure with the peer device.
*
* \param[in] peerDeviceId      The GAP peer Id
*
* \param[in] result            The result (true or false)
*
* \return none
*
********************************************************************************** */
void BleServDisc_Finished(deviceId_t peerDeviceId, bool_t result)
{
    servDiscEvent_t event;

    BleServDisc_Stop(peerDeviceId);
    event.eventType = gDiscoveryFinished_c;
    event.eventData.success = result;
    pfServDiscCallback(peerDeviceId, &event);
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\private
*\fn           void BleServDisc_Reset(deviceId_t peerDeviceId)
*\brief        Frees the memory that has been allocated for Service Discovery.
*
*\param  [in]  peerDeviceId        GATT Server device ID.

* \retval      void.
********************************************************************************** */
static void BleServDisc_Reset(deviceId_t peerDeviceId)
{
    if (maServDiscInfo[peerDeviceId].mpServiceDiscoveryBuffer != NULL)
    {
        (void)MEM_BufferFree(maServDiscInfo[peerDeviceId].mpServiceDiscoveryBuffer);
        maServDiscInfo[peerDeviceId].mpServiceDiscoveryBuffer = NULL;
    }

    if (maServDiscInfo[peerDeviceId].mpCharDiscoveryBuffer != NULL)
    {
        (void)MEM_BufferFree(maServDiscInfo[peerDeviceId].mpCharDiscoveryBuffer);
        maServDiscInfo[peerDeviceId].mpCharDiscoveryBuffer = NULL;
    }

    if (maServDiscInfo[peerDeviceId].mpCharDescriptorBuffer != NULL)
    {
        (void)MEM_BufferFree(maServDiscInfo[peerDeviceId].mpCharDescriptorBuffer);
        maServDiscInfo[peerDeviceId].mpCharDescriptorBuffer = NULL;
    }
}

/*! *********************************************************************************
*\private
*\fn           void BleServDisc_NewService(deviceId_t    peerDeviceId,
*                                          gattService_t *pService)
*\brief        Signals the discovery of a new service.
*
*\param  [in]  peerDeviceId      The GAP peer Id.
*\param  [in]  pService          The service that was discovered.
*
*\retval       void.
********************************************************************************** */
STATIC void BleServDisc_NewService(deviceId_t peerDeviceId, gattService_t *pService)
{
    servDiscEvent_t event;

    event.eventType = gServiceDiscovered_c;
    event.eventData.pService = pService;
    pfServDiscCallback(peerDeviceId, &event);
}

/*! *********************************************************************************
* @}
********************************************************************************** */
