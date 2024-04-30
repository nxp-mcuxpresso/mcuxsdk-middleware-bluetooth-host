/*! *********************************************************************************
 * \addtogroup GATT_DB
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2023 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef GATT_DATABASE_DYNAMIC_H
#define GATT_DATABASE_DYNAMIC_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "EmbeddedTypes.h"
#include "gap_types.h"
#include "gatt_db_dynamic.h"

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/

/*************************************************************************************
**************************************************************************************
* Public types
**************************************************************************************
*************************************************************************************/

/* Service, Characteristics and Descriptors used to add a service */

typedef struct descriptorInfo_tag
{
    uint16_t                                uuid16;
    uint16_t                                handle;
    uint16_t                                valueLength;
    uint8_t*                                pValue;
    gattAttributePermissionsBitFields_t     accessPermissions;
} descriptorInfo_t;

typedef struct characteristicInfo_tag
{
    uint16_t                                uuid16;
    uint16_t                                handle;
    gattCharacteristicPropertiesBitFields_t properties;
    uint16_t                                maxValueLength;
    uint16_t                                valueLength;
    uint8_t*                                pValue;
    gattAttributePermissionsBitFields_t     accessPermissions;
    bool_t                                  bAddCccd;
    uint16_t                                cccdHandle;
    uint8_t                                 nbOfDescriptors;
    descriptorInfo_t*                       pDescriptorInfo;
} characteristicInfo_t;

typedef struct serviceInfo_tag
{
    uint16_t                                uuid16;
    uint16_t                                handle;
    uint8_t                                 nbOfCharacteristics;
    characteristicInfo_t*                   pCharacteristicInfo;
} serviceInfo_t;

/* Output handles returned by the service add functions obtained from Ble Host */

/* GATT service */
typedef struct gattServiceHandles_tag
{
    uint16_t serviceHandle;
    uint16_t charServiceChangedHandle;
    uint16_t charServiceChangedCccdHandle;
} gattServiceHandles_t;

/* GAP service */
typedef struct gapServiceHandles_tag
{
    uint16_t serviceHandle;
    uint16_t charDeviceNameHandle;
    uint16_t charAppearanceHandle;
    uint16_t charPpcpHandle;
} gapServiceHandles_t;

/* IPSS service */
typedef struct ipssServiceHandles_tag
{
    uint16_t serviceHandle;
} ipssServiceHandles_t;

/* Heart rate service */
typedef struct heartRateServiceHandles_tag
{
    uint16_t serviceHandle;
    uint16_t charHrMeasurementHandle;
    uint16_t charHrMeasurementCccdHandle;
    uint16_t charBodySensorLocationHandle;
    uint16_t charHrCtrlPointHandle;
} heartRateServiceHandles_t;

/* Battery service */
typedef struct batteryServiceHandles_tag
{
    uint16_t serviceHandle;
    uint16_t charBatteryLevelHandle;
    uint16_t charBatteryLevelCccdHandle;
} batteryServiceHandles_t;

/* Device info service */
typedef struct deviceInfoServiceHandles_tag
{
    uint16_t serviceHandle;
    uint16_t charManufNameHandle;
    uint16_t charModelNoHandle;
    uint16_t charSerialNoHandle;
    uint16_t charHwRevHandle;
    uint16_t charFwRevHandle;
    uint16_t charSwRevHandle;
    uint16_t charSystemIdHandle;
    uint16_t charRcdlHandle;
} deviceInfoServiceHandles_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*! *********************************************************************************
*\fn            bleResult_t GattDbDynamic_AddServiceInDatabase(
*                   serviceInfo_t* pServiceInfo)
*\brief         Adds service declaration to dynamic gatt db, along with its
*               characteristics and descriptors.
*
*\param  [in]   pServiceInfo   Pointer to the structure containing service
*                              information.
*
*\return        bleResult_t    Result of the operation.
********************************************************************************** */
bleResult_t GattDbDynamic_AddServiceInDatabase(serviceInfo_t* pServiceInfo);

/*! *********************************************************************************
*\fn            bleResult_t GattDbDynamic_AddGattService(
*                   gattServiceHandles_t* pOutServiceHandles)
*\brief         Adds Gatt service to dynamic database.
*
*\param  [in]   none.
*
*\param  [out]  pOutServiceHandles   Pointer to the structure where the Gatt service
*                                    handles will be stored.
*
*\return        bleResult_t          Result of the operation.
********************************************************************************** */
bleResult_t GattDbDynamic_AddGattService(gattServiceHandles_t* pOutServiceHandles);

/*! *********************************************************************************
*\fn            bleResult_t GattDbDynamic_AddGapService(
*                   gapServiceHandles_t* pOutServiceHandles)
*\brief         Adds Gap service to dynamic database.
*
*\param  [in]   none.
*
*\param  [out]  pOutServiceHandles   Pointer to the structure where the Gap service
*                                    handles will be stored.
*
*\return        bleResult_t          Result of the operation.
********************************************************************************** */
bleResult_t GattDbDynamic_AddGapService(gapServiceHandles_t* pOutServiceHandles);

/*! *********************************************************************************
*\fn            bleResult_t GattDbDynamic_AddIpssService(
*                   ipssServiceHandles_t* pOutServiceHandles)
*\brief         Adds Ips service to dynamic database.
*
*\param  [in]   none.
*
*\param  [out]  pOutServiceHandles   Pointer to the structure where the Ips service
*                                    handles will be stored.
*
*\return        bleResult_t          Result of the operation.
********************************************************************************** */
bleResult_t GattDbDynamic_AddIpssService(ipssServiceHandles_t* pOutServiceHandles);

/*! *********************************************************************************
*\fn            bleResult_t GattDbDynamic_AddHeartRateService(
*                   heartRateServiceHandles_t* pOutServiceHandles)
*\brief         Adds Heart Rate service to dynamic database.
*
*\param  [in]   none.
*
*\param  [out]  pOutServiceHandles   Pointer to the structure where the Hrs service
*                                    handles will be stored.
*
*\return        bleResult_t          Result of the operation.
********************************************************************************** */
bleResult_t GattDbDynamic_AddHeartRateService
(
    heartRateServiceHandles_t* pOutServiceHandles
);

/*! *********************************************************************************
*\fn            bleResult_t GattDbDynamic_AddBatteryService(
*                   batteryServiceHandles_t* pOutServiceHandles)
*\brief         Adds Battery service to dynamic database.
*
*\param  [in]   none.
*
*\param  [out]  pOutServiceHandles   Pointer to the structure where the Battery
*                                    service handles will be stored.
*
*\return        bleResult_t          Result of the operation.
********************************************************************************** */
bleResult_t GattDbDynamic_AddBatteryService
(
    batteryServiceHandles_t* pOutServiceHandles
);

/*! *********************************************************************************
*\fn            bleResult_t GattDbDynamic_AddDeviceInformationService(
*                   deviceInfoServiceHandles_t* pOutServiceHandles)
*\brief         Adds Device Information service to dynamic database.
*
*\param  [in]   none.
*
*\param  [out]  pOutServiceHandles   Pointer to the structure where the Device
*                                    Information service handles will be stored.
*
*\return        bleResult_t          Result of the operation.
********************************************************************************** */
bleResult_t GattDbDynamic_AddDeviceInformationService
(
    deviceInfoServiceHandles_t* pOutServiceHandles
);

/*! *********************************************************************************
*\fn            serviceInfo_t* GattDbDynamic_GetIpssService(void)
*\brief         Populates a service info structure with Ips service information.
*
*\param  [in]   none.
*
*\return        serviceInfo_t*       Pointer to the Ips service structure.
********************************************************************************** */
serviceInfo_t* GattDbDynamic_GetIpssService(void);

/*! *********************************************************************************
*\fn            serviceInfo_t* GattDbDynamic_GetHeartRateService(void)
*\brief         Populates a service info structure with Heart Rate service information.
*
*\param  [in]   none.
*
*\return        serviceInfo_t*       Pointer to the Hrs service structure.
********************************************************************************** */
serviceInfo_t* GattDbDynamic_GetHeartRateService(void);

/*! *********************************************************************************
*\fn            serviceInfo_t* GattDbDynamic_GetBatteryService(void)
*\brief         Populates a service info structure with Battery service information.
*
*\param  [in]   none.
*
*\return        serviceInfo_t*       Pointer to the Battery service structure.
********************************************************************************** */
serviceInfo_t* GattDbDynamic_GetBatteryService(void);

/*! *********************************************************************************
*\fn            serviceInfo_t* GattDbDynamic_GetDeviceInformationService(void)
*\brief         Populates a service info structure with Device Information service
*               information.
*
*\param  [in]   none.
*
*\return        serviceInfo_t*       Pointer to the Device Information service
*                                    structure.
********************************************************************************** */
serviceInfo_t* GattDbDynamic_GetDeviceInformationService(void);

#ifdef __cplusplus
}
#endif

#endif /* GATT_DATABASE_DYNAMIC_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
