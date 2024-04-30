/*! *********************************************************************************
 * \addtogroup BLE
 * @{
 ********************************************************************************* */
/*! *********************************************************************************
* Copyright 2014 - 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2023 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef BLE_SERVICE_DISCOVERY_H
#define BLE_SERVICE_DISCOVERY_H

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
*************************************************************************************
* Includes
*************************************************************************************
************************************************************************************/
#include "ble_general.h"
#include "gatt_types.h"
#include "gatt_client_interface.h"

/************************************************************************************
*************************************************************************************
* Public Macros
*************************************************************************************
************************************************************************************/
/*! Maximum Number of Services that can be stored during Service Discovery */
#ifndef gMaxServicesCount_d
#define gMaxServicesCount_d             6U
#endif /* gMaxServicesCount_d */

/*! Maximum Number of Characteristics per Service that can be stored during
Service Discovery */
#ifndef gMaxServiceCharCount_d
#define gMaxServiceCharCount_d          10U
#endif /* gMaxServiceCharCount_d */

/*! Maximum Number of Descriptors per Service that can be stored during
Service Discovery */
#ifndef gMaxCharDescriptorsCount_d
#define gMaxCharDescriptorsCount_d      4U
#endif /* gMaxCharDescriptorsCount_d */

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum servDiscEventType_tag{
    gServiceDiscovered_c,               /*!< A service has been discovered. */
    gDiscoveryFinished_c,               /*!< Service Discovery is finished. */
    gGattSecurityLevelsChar_c,          /*!< Discovered the GATT Security Levels Characteristic. */
} servDiscEventType_t;

/*! Service Discovery Event */
typedef struct servDiscEvent_tag {
    servDiscEventType_t  eventType;     /*!< Event type. */
    union {
        gattService_t*        pService;
        gattCharacteristic_t* pCharacteristic;
        bool_t                success;
    } eventData;                        /*!< Event data, selected accordingto event type. */
} servDiscEvent_t;

typedef void (*servDiscCallback_t)
(
    deviceId_t          deviceId,       /*!< Device ID identifying the active connection. */
    servDiscEvent_t*    pEvent          /*!< Service Discovery Event. */
);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */

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
void BleServDisc_RegisterCallback(servDiscCallback_t pServDiscCallback);

/*! *********************************************************************************
*\fn           bleResult_t BleServDisc_Start(deviceId_t peerDeviceId)
*\brief        Starts the Service Discovery procedure with the peer device.
*
*\param  [in]  peerDeviceId      The GAP peer Id.
*
*\return       bleResult_t       Result of the operation.
********************************************************************************** */
bleResult_t BleServDisc_Start(deviceId_t peerDeviceId);

/*! *********************************************************************************
*\fn           void BleServDisc_Stop(deviceId_t peerDeviceId)
*\brief        Stops the Service Discovery procedure with the peer device.
*
*\param  [in]  peerDeviceId      The GAP peer Id.
*
*\retval       void.
********************************************************************************** */
void BleServDisc_Stop(deviceId_t peerDeviceId);

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
);

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
);

/*! *********************************************************************************
* \brief        Signals the module that service discovery has ended
*               Should be called by the application procedure callback for the
*               gGattProcSignalServiceDiscoveryComplete_c event if robust caching is supported
*
* \param[in]    peerDeviceId        GATT Server device ID.
* \param[in]    result              Procedure result - TRUE for success and FALSE otherwise.
*
* \return  gBleSuccess_c or error.
*
********************************************************************************** */
void BleServDisc_Finished(deviceId_t peerDeviceId, bool_t result);

#ifdef __cplusplus
}
#endif

#endif /* BLE_SERVICE_DISCOVERY_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
