/*! *********************************************************************************
 * \addtogroup BLE
 * @{
 ********************************************************************************* */
/*! *********************************************************************************
* Copyright 2016-2021, 2023-2025 NXP
*
*
* \file
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
#include "gap_interface.h"
#include "gatt_client_interface.h"
#include "gatt_server_interface.h"
#include "ble_service_discovery.h"
#include "ble_config.h"

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef struct servDiscInfo_tag
{
    /* Buffer used for Service Discovery */
    gattService_t   *mpServiceDiscoveryBuffer;

    /* Buffer used for Characteristic Discovery */
    gattCharacteristic_t *mpCharDiscoveryBuffer;

    /* Buffer used for Characteristic Descriptor Discovery */
    gattAttribute_t *mpCharDescriptorBuffer;

    /* Counters */
    uint8_t mCurrentServiceInDiscoveryIndex;
    uint8_t mCurrentCharInDiscoveryIndex;
    uint8_t mCurrentDescInDiscoveryIndex;
    uint8_t mcPrimaryServices;
    bool_t  mServDiscInProgress;

} servDiscInfo_t;

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

STATIC servDiscInfo_t maServDiscInfo[gAppMaxConnections_c];
#if defined(gBLE51_d) && (gBLE51_d == 1U) && defined(gGattCaching_d) && (gGattCaching_d == 1U)
/* handle range for service discovery */
extern gattHandleRange_t mClientDatabaseUpdateHandles;
#endif /* gBLE51_d && gGattCaching_d */

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
            result = GattClient_DiscoverAllPrimaryServices(
                        peerDeviceId,
                        maServDiscInfo[peerDeviceId].mpServiceDiscoveryBuffer,
                        gMaxServicesCount_d,
                        &maServDiscInfo[peerDeviceId].mcPrimaryServices);
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
            result = GattClient_DiscoverPrimaryServicesByUuid(
                        peerDeviceId,
                        uuidType,
                        pUuid,
                        maServDiscInfo[peerDeviceId].mpServiceDiscoveryBuffer,
                        1,
                        &maServDiscInfo[peerDeviceId].mcPrimaryServices);
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
    bool_t earlyReturn = FALSE;
    
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
                        pInfo->mCurrentServiceInDiscoveryIndex = 0;
                        pInfo->mCurrentCharInDiscoveryIndex = 0;
                        pInfo->mCurrentDescInDiscoveryIndex = 0;

                        pInfo->mpServiceDiscoveryBuffer->aCharacteristics =
                                 pInfo->mpCharDiscoveryBuffer;

                        /* Start Characteristic Discovery for current service */
                        (void)GattClient_DiscoverAllCharacteristicsOfService(
                                            peerDeviceId,
                                            pInfo->mpServiceDiscoveryBuffer,
                                            gMaxServiceCharCount_d);
                    }
                    else
                    {
#if defined(gBLE51_d) && (gBLE51_d == 1U) && defined(gGattCaching_d) && (gGattCaching_d == 1U) \
    && defined(gGattAutomaticRobustCachingSupport_d) && (gGattAutomaticRobustCachingSupport_d == 1U)
                        /* end of service discovery - read the database hash */
                        (void)GattClient_GetDatabaseHash(peerDeviceId, gInvalidNvmIndex_c);
#else
                        BleServDisc_Finished(peerDeviceId, TRUE);
#endif /* gGattCaching_d && gBLE51_d */
                    }
                }
                break;

                case gGattProcDiscoverAllCharacteristicDescriptors_c:
                /* Fall through */
                case gGattProcDiscoverAllCharacteristics_c:
                {
                    if (procedureType ==
                        gGattProcDiscoverAllCharacteristicDescriptors_c)
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

                    gattService_t *pCurrentService =
                                                 pInfo->mpServiceDiscoveryBuffer +
                                                 pInfo->mCurrentServiceInDiscoveryIndex;

                    if (pInfo->mCurrentCharInDiscoveryIndex < pCurrentService->cNumCharacteristics)
                    {
                        gattCharacteristic_t *pCurrentChar =
                                                 pCurrentService->aCharacteristics +
                                                 pInfo->mCurrentCharInDiscoveryIndex;

                        /* Find next characteristic with descriptors*/
                        while (pInfo->mCurrentCharInDiscoveryIndex < pCurrentService->cNumCharacteristics)
                        {
#if defined(gBLE51_d) && (gBLE51_d == 1U) && defined(gGattCaching_d) && (gGattCaching_d == 1U)
                             /* save the handle for the client supported features characteristic */
                             if (gBleSig_GattClientSupportedFeatures_d == pCurrentChar->value.uuid.uuid16)
                             {
                                 gGattActiveClientSupportedFeaturesHandles[peerDeviceId] = pCurrentChar->value.handle - 1U;
                             }

                             /* save the handle for the service changed characteristic */
                             if (gBleSig_GattServiceChanged_d == pCurrentChar->value.uuid.uuid16)
                             {
                                 mActiveServiceChangedCharHandle[peerDeviceId] = pCurrentChar->value.handle - 1U;
                                 mActiveServiceChangedCCCDHandle[peerDeviceId] = pCurrentChar->value.handle + 1U;
                             }
#endif /* gBLE51_d && gGattCaching_d */

#if defined(gBLE54_d) && (gBLE54_d == 1U) && defined(gGattSecurityLevelChar_d) && (gGattSecurityLevelChar_d == 1U)
                            if (pCurrentChar->value.uuid.uuid16 == gBleSig_GattSecurityLevels_d)
                            {
                                /* Found GATT Security Levels characteristic - inform application */
                                servDiscEvent_t event;
                                event.eventType = gGattSecurityLevelsChar_c;
                                event.eventData.pCharacteristic = pCurrentChar;
                                pfServDiscCallback(peerDeviceId, &event);
                            }
#endif /* gBLE54_d && gGattSecurityLevelChar_d */
                            /* Check if we have handles available between adjacent characteristics */
                            if (pCurrentChar->value.handle + 2U < (pCurrentChar + 1)->value.handle)
                            {
                                if (pInfo->mCurrentDescInDiscoveryIndex < (uint8_t)gMaxCharDescriptorsCount_d)
                                {
                                    pCurrentChar->aDescriptors = pInfo->mpCharDescriptorBuffer +
                                                                 pInfo->mCurrentDescInDiscoveryIndex;
                                    (void)GattClient_DiscoverAllCharacteristicDescriptors(
                                                        peerDeviceId,
                                                        pCurrentChar,
                                                        (pCurrentChar + 1)->value.handle,
                                                        (uint8_t)(gMaxCharDescriptorsCount_d -
                                                        pInfo->mCurrentDescInDiscoveryIndex));
                                    /* Skip remaining processing */
                                    earlyReturn = TRUE;
                                    break;
                                }
                            }

                            pInfo->mCurrentCharInDiscoveryIndex++;
                            if (pInfo->mCurrentCharInDiscoveryIndex < pCurrentService->cNumCharacteristics)
                            {
                                pCurrentChar = pCurrentService->aCharacteristics +
                                               pInfo->mCurrentCharInDiscoveryIndex;
                            }
                        }

                        if (earlyReturn == FALSE)
                        {
                            /* Made it to the last characteristic. Check against service end handle*/
                            if (pCurrentChar->value.handle < pCurrentService->endHandle)
                            {
                                if (pInfo->mCurrentDescInDiscoveryIndex < gMaxCharDescriptorsCount_d)
                                {
                                    pCurrentChar->aDescriptors = pInfo->mpCharDescriptorBuffer +
                                                                 pInfo->mCurrentDescInDiscoveryIndex;
                                    (void)GattClient_DiscoverAllCharacteristicDescriptors(
                                                            peerDeviceId,
                                                            pCurrentChar,
                                                            pCurrentService->endHandle,
                                                            (uint8_t)(gMaxCharDescriptorsCount_d -
                                                            pInfo->mCurrentDescInDiscoveryIndex));
                                    /* Skip remaining processing */
                                    earlyReturn = TRUE;
                                }
                            }
                        }
                    }

                    if (earlyReturn == FALSE)
                    {
                        /* Signal Discovery of Service */
                        BleServDisc_NewService(peerDeviceId, pCurrentService);

                        /* Move on to the next service */
                        pInfo->mCurrentServiceInDiscoveryIndex++;

                        /* Reset characteristic discovery */
                        pInfo->mCurrentCharInDiscoveryIndex = 0;
                        pInfo->mCurrentDescInDiscoveryIndex = 0;
                        FLib_MemSet(pInfo->mpCharDescriptorBuffer,
                                    0,
                                    sizeof(gattAttribute_t) * (uint32_t)gMaxCharDescriptorsCount_d);
                        FLib_MemSet(pInfo->mpCharDiscoveryBuffer,
                                    0,
                                    sizeof(gattCharacteristic_t) * (uint32_t)gMaxServiceCharCount_d);

                        if (pInfo->mCurrentServiceInDiscoveryIndex < pInfo->mcPrimaryServices)
                        {
                            /* Allocate memory for Char Discovery */
                            (pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex)->aCharacteristics =
                                                                   pInfo->mpCharDiscoveryBuffer;

                             /* Start Characteristic Discovery for current service */
                            (void)GattClient_DiscoverAllCharacteristicsOfService(
                                    peerDeviceId,
                                    pInfo->mpServiceDiscoveryBuffer + pInfo->mCurrentServiceInDiscoveryIndex,
                                    gMaxServiceCharCount_d);
                        }
                        else
                        {
#if defined(gBLE51_d) && (gBLE51_d == 1U) && defined(gGattCaching_d) && (gGattCaching_d == 1U) \
    && defined(gGattAutomaticRobustCachingSupport_d) && (gGattAutomaticRobustCachingSupport_d == 1U)
                            /* use a dummy nvm index to know when the function is called at the end of service discovery */
                            (void)GattClient_GetDatabaseHash(peerDeviceId, gInvalidNvmIndex_c);
#else
                            BleServDisc_Finished(peerDeviceId, TRUE);
#endif /* gGattCaching_d && gBLE51_d */
                        }
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

    return;
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
