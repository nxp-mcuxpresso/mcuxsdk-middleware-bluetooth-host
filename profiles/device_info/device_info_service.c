/*! *********************************************************************************
* \addtogroup Device Information Service
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2014 Freescale Semiconductor, Inc.
* Copyright 2016-2019, 2022-2023 NXP
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
#include "ble_general.h"
#include "gatt_db_app_interface.h"
#include "gatt_server_interface.h"
#include "gap_interface.h"
#include "device_info_interface.h"

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
static bleResult_t Dis_SetManufacturerName(uint16_t serviceHandle, utf8s_t manufacturerName);
static bleResult_t Dis_SetModelNumber(uint16_t serviceHandle, utf8s_t modelNumber);
static bleResult_t Dis_SetSerialNumber(uint16_t serviceHandle, utf8s_t serialNumber);
static bleResult_t Dis_SetHwRevision(uint16_t serviceHandle, utf8s_t hwRevision);
static bleResult_t Dis_SetFwRevision(uint16_t serviceHandle, utf8s_t fwRevision);
static bleResult_t Dis_SetSwRevision(uint16_t serviceHandle, utf8s_t swRevision);
static bleResult_t Dis_SetSystemId(uint16_t serviceHandle, systemId_t *pSystemId);
static bleResult_t Dis_SetPnpId(uint16_t serviceHandle, pnpId_t *pPnpId);
static bleResult_t Dis_SetRcdl(uint16_t serviceHandle, regCertDataList_t rcdl);
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

bleResult_t Dis_Start(disConfig_t *pServiceConfig)
{
    bleResult_t result = gBleSuccess_c;

    if (pServiceConfig->manufacturerName.stringLength != 0U)
    {
        result = Dis_SetManufacturerName(pServiceConfig->serviceHandle, pServiceConfig->manufacturerName);
    }

    if ((pServiceConfig->modelNumber.stringLength != 0U) && (result == gBleSuccess_c))
    {
        result = Dis_SetModelNumber(pServiceConfig->serviceHandle, pServiceConfig->modelNumber);
    }

    if ((pServiceConfig->serialNumber.stringLength != 0U) && (result == gBleSuccess_c))
    {
        result = Dis_SetSerialNumber(pServiceConfig->serviceHandle, pServiceConfig->serialNumber);
    }

    if ((pServiceConfig->hwRevision.stringLength != 0U) && (result == gBleSuccess_c))
    {
        result = Dis_SetHwRevision(pServiceConfig->serviceHandle, pServiceConfig->hwRevision);
    }

    if ((pServiceConfig->fwRevision.stringLength != 0U) && (result == gBleSuccess_c))
    {
        result = Dis_SetFwRevision(pServiceConfig->serviceHandle, pServiceConfig->fwRevision);
    }

    if ((pServiceConfig->swRevision.stringLength != 0U) && (result == gBleSuccess_c))
    {
        result = Dis_SetSwRevision(pServiceConfig->serviceHandle, pServiceConfig->swRevision);
    }

    if ((pServiceConfig->pSystemId != NULL) && (result == gBleSuccess_c))
    {
        result = Dis_SetSystemId(pServiceConfig->serviceHandle, pServiceConfig->pSystemId);
    }

    if ((pServiceConfig->pPnpId != NULL) && (result == gBleSuccess_c))
    {
        result = Dis_SetPnpId(pServiceConfig->serviceHandle, pServiceConfig->pPnpId);
    }

    if ((pServiceConfig->rcdl.length != 0U) && (result == gBleSuccess_c))
    {
        result = Dis_SetRcdl(pServiceConfig->serviceHandle, pServiceConfig->rcdl);
    }

    return result;
}

bleResult_t Dis_Stop(disConfig_t *pServiceConfig)
{
    return gBleSuccess_c;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
static bleResult_t Dis_SetManufacturerName(uint16_t serviceHandle, utf8s_t manufacturerName)
{
    uint16_t  handle = gGattDbInvalidHandle_d;
    bleResult_t result = gBleSuccess_c;
    bleUuid_t uuid = Uuid16(gBleSig_ManufacturerNameString_d);

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,
             gBleUuidType16_c, &uuid, &handle);

    if (result == gBleSuccess_c)
    {
        /* Update characteristic value */
        result = GattDb_WriteAttribute(handle, manufacturerName.stringLength, (void *)manufacturerName.pUtf8s);
    }

    return result;
}

static bleResult_t Dis_SetModelNumber(uint16_t serviceHandle, utf8s_t modelNumber)
{
    uint16_t  handle = gGattDbInvalidHandle_d;
    bleResult_t result = gBleSuccess_c;
    bleUuid_t uuid = Uuid16(gBleSig_ModelNumberString_d);

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,
             gBleUuidType16_c, &uuid, &handle);

    if (result == gBleSuccess_c)
    {
        /* Update characteristic value */
        result = GattDb_WriteAttribute(handle, modelNumber.stringLength, (void *)modelNumber.pUtf8s);
    }

    return result;
}


static bleResult_t Dis_SetSerialNumber(uint16_t serviceHandle, utf8s_t serialNumber)
{
    uint16_t  handle = gGattDbInvalidHandle_d;
    bleResult_t result = gBleSuccess_c;
    bleUuid_t uuid = Uuid16(gBleSig_SerialNumberString_d);

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,
             gBleUuidType16_c, &uuid, &handle);

    if (result == gBleSuccess_c)
    {
        /* Update characteristic value */
        result = GattDb_WriteAttribute(handle, serialNumber.stringLength, (void *)serialNumber.pUtf8s);
    }

    return result;
}

static bleResult_t Dis_SetHwRevision(uint16_t serviceHandle, utf8s_t hwRevision)
{
    uint16_t  handle = gGattDbInvalidHandle_d;
    bleResult_t result = gBleSuccess_c;
    bleUuid_t uuid = Uuid16(gBleSig_HardwareRevisionString_d);

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,
             gBleUuidType16_c, &uuid, &handle);

    if (result == gBleSuccess_c)
    {
        /* Update characteristic value */
        result = GattDb_WriteAttribute(handle, hwRevision.stringLength, (void *)hwRevision.pUtf8s);
    }

    return result;
}


static bleResult_t Dis_SetFwRevision(uint16_t serviceHandle, utf8s_t fwRevision)
{
    uint16_t  handle = gGattDbInvalidHandle_d;
    bleResult_t result = gBleSuccess_c;
    bleUuid_t uuid = Uuid16(gBleSig_FirmwareRevisionString_d);

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,
             gBleUuidType16_c, &uuid, &handle);

    if (result == gBleSuccess_c)
    {
        /* Update characteristic value */
        result = GattDb_WriteAttribute(handle, fwRevision.stringLength, (void *)fwRevision.pUtf8s);
    }

    return result;
}


static bleResult_t Dis_SetSwRevision(uint16_t serviceHandle, utf8s_t swRevision)
{
    uint16_t  handle = gGattDbInvalidHandle_d;
    bleResult_t result = gBleSuccess_c;
    bleUuid_t uuid = Uuid16(gBleSig_SoftwareRevisionString_d);

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,
             gBleUuidType16_c, &uuid, &handle);

    if (result == gBleSuccess_c)
    {
        /* Update characteristic value */
        result = GattDb_WriteAttribute(handle, swRevision.stringLength, (void *)swRevision.pUtf8s);
    }

    return result;
}

static bleResult_t Dis_SetSystemId(uint16_t serviceHandle, systemId_t *pSystemId)
{
    uint16_t  handle = gGattDbInvalidHandle_d;
    bleResult_t result = gBleSuccess_c;
    bleUuid_t uuid = Uuid16(gBleSig_SystemId_d);

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,
             gBleUuidType16_c, &uuid, &handle);

    if (result == gBleSuccess_c)
    {
        /* Update characteristic value */
        result = GattDb_WriteAttribute(handle, (uint16_t)sizeof(systemId_t), (void *)pSystemId);
    }

    return result;
}

static bleResult_t Dis_SetRcdl(uint16_t serviceHandle, regCertDataList_t rcdl)
{
    uint16_t  handle = gGattDbInvalidHandle_d;
    bleResult_t result = gBleSuccess_c;
    bleUuid_t uuid = Uuid16(gBleSig_IeeeRcdl_d);

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,
             gBleUuidType16_c, &uuid, &handle);

    if (result == gBleSuccess_c)
    {
        /* Update characteristic value */
        result = GattDb_WriteAttribute(handle, rcdl.length, rcdl.pList);
    }

    return result;
}

static bleResult_t Dis_SetPnpId(uint16_t serviceHandle, pnpId_t *pPnpId)
{
    uint16_t  handle = gGattDbInvalidHandle_d;
    bleResult_t result = gBleSuccess_c;
    bleUuid_t uuid = Uuid16(gBleSig_PnpId_d);

    /* Get handle of characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,
             gBleUuidType16_c, &uuid, &handle);

    if (result == gBleSuccess_c)
    {
        /* Update characteristic value */
        result = GattDb_WriteAttribute(handle, (uint16_t)sizeof(pnpId_t), (void *)pPnpId);
    }

    return result;
}


/*! *********************************************************************************
* @}
********************************************************************************** */
