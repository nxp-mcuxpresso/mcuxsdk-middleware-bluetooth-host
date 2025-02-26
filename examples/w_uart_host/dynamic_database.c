/*! *********************************************************************************
* Copyright 2022-2025 NXP
*
*
* \file hsdk_main.c
*
* This is the main source file for the HSDK module
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "fsl_component_mem_manager.h"
#include "fsl_component_timer_manager.h"
#include "fwk_platform.h"

#include "host_hsdk_interface.h"
#include "host_cmd_ble.h"

/* BLE Host Stack */
#include "gap_types.h"
#include "gatt_interface.h"
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gatt_database.h"
#include "gap_interface.h"
#include "gatt_db_app_interface.h"

#include "w_uart_application.h"
#include "dynamic_database.h"
#include "host_ble_service_discovery.h"

/************************************************************************************
*************************************************************************************
* Private macros
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
static uint16_t mCharMonitoredHandles;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static void gattDbAddPrimarySerivceHandler(bleEvtContainer_t* pMsg);
static void gattDbAddCharacteristicHandler(bleEvtContainer_t* pMsg);

static void GATTDBDynamicAddCharacteristicDeclarationAndValue
(
    UuidType_t                              UuidType,
    bleUuid_t                               uuid,
    gattCharacteristicPropertiesBitFields_t characteristicProperties,
    uint16_t                                maxValueLength,
    uint16_t                                initialValueLength,
    uint8_t*                          aInitialValue,
    gattAttributePermissionsBitFields_t     valueAccessPermissions
);

static void GATTDBDynamicAddCharacteristicDescriptor
(
    UuidType_t                              UuidType,
    bleUuid_t                               uuid,
    uint16_t                                valueLength,
    uint8_t*                                aDescriptorValue,
    gattAttributePermissionsBitFields_t     valueAccessPermissions
);

/*!*************************************************************************************************
*\fn    void GATTDBDynamicAddPrimaryServiceDeclaration(uint16_t desiredHandle,UuidType_t uuidType,
*                                                      bleUuid_t uuid);
*
*\brief Sends a request to add a primary service at the specified handle
*
* \param[in]    desiredHandle    Handle at which to add the service
* \param[in]    uuidType         UUID type of the service
* \param[in]    uuid             Desired service UUID
*
*\return       None
***************************************************************************************************/
void GATTDBDynamicAddPrimaryServiceDeclaration
(
    uint16_t desiredHandle,
    UuidType_t uuidType,
    bleUuid_t uuid
)
{
    GATTDBDynamicAddPrimaryServiceDeclarationRequest_t req = { 0 };
    req.DesiredHandle = desiredHandle;
    req.UuidType = uuidType;

    switch (req.UuidType)
    {
        case Uuid16Bits:
          Utils_PackTwoByteValue(uuid.uuid16, req.Uuid.Uuid16Bits);
        break;

        case Uuid128Bits:
          FLib_MemCpy(req.Uuid.Uuid128Bits, uuid.uuid128, 16);
        break;

        case Uuid32Bits:
          Utils_PackFourByteValue(uuid.uuid32, req.Uuid.Uuid32Bits);
        break;

        default:
        {
          ; /* No action required */
        }
        break;
    }

    GATTDBDynamicAddPrimaryServiceDeclarationRequest(&req, gFsciInterface_c);
}

/*!*************************************************************************************************
*\fn    bool_t App_HandleHSDKMessageInputGATTDb(bleEvtContainer_t *pMsg)
*
*\brief Handles GATT database specific events
*
* \param[in]   pMsg    Pointer to event container
*
*\return       matchFound
***************************************************************************************************/
bool_t App_HandleHSDKMessageInputGATTDb(bleEvtContainer_t* pMsg)
{
    bool_t matchFound = TRUE;

    switch (pMsg->id)
    {
        case GATTDBDynamicAddPrimaryServiceDeclarationIndication_FSCI_ID:
        {
            gattDbAddPrimarySerivceHandler(pMsg);
        }
        break;

        case GATTDBDynamicAddCharacteristicDeclarationAndValueIndication_FSCI_ID:
        {
            gattDbAddCharacteristicHandler(pMsg);
        }
        break;

        case GATTDBDynamicAddCharacteristicDescriptorIndication_FSCI_ID:
        {
            /* Successfully added the Chararacteristic Presentation Format Descriptor - Add CCCD */
            (void)GATTDBDynamicAddCccdRequest(gFsciInterface_c);
        }
        break;

        case GATTDBDynamicAddCccdIndication_FSCI_ID:
        {
            /* Successfully added the characteristic CCCD - add the Device Information service */
            bleUuid_t uuid;
            uuid.uuid16 = gBleSig_DeviceInformationService_d;
            GATTDBDynamicAddPrimaryServiceDeclaration(mDeviceInfoServiceHandle_c, Uuid16Bits, uuid);
        }
        break;

        case GATTDBConfirm_FSCI_ID:
        case GATTDBDynamicAddSecondaryServiceDeclarationIndication_FSCI_ID:
        case GATTDBDynamicAddIncludeDeclarationIndication_FSCI_ID:
        case GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication_FSCI_ID:
        case GATTDBDynamicAddCharDescriptorWithUniqueValueIndication_FSCI_ID:
        {
            ; /* Untreated GATT database events */
        }
        break;

        default:
        {
            matchFound = FALSE;
        }
        break;
    }

    return matchFound;
}
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*!*************************************************************************************************
*\fn    void GATTDBDynamicAddCharacteristicDescriptor(UuidType_t uuidType, bleUuid_t uuid, uint16_t valueLength,
*                         uint8_t *aDescriptorValue, gattAttributePermissionsBitFields_t valueAccessPermissions)
*
*\brief Sends a request to add characteristic descriptor to the database
*
* \param[in]    uuidType                UUID type of the descriptor
* \param[in]    uuid                    Desired descriptor UUID
* \param[in]    valueLength             Size of the descriptor value
* \param[in]    aDescriptorValue        Pointer to the descriptor value
* \param[in]    valueAccessPermissions  Descriptor access permissions
*
*\return       None
***************************************************************************************************/
static void GATTDBDynamicAddCharacteristicDescriptor
(
    UuidType_t                              UuidType,
    bleUuid_t                               uuid,
    uint16_t                                valueLength,
    uint8_t*                                aDescriptorValue,
    gattAttributePermissionsBitFields_t     valueAccessPermissions
)
{
    GATTDBDynamicAddCharacteristicDescriptorRequest_t req;
    req.UuidType = UuidType;
    switch (req.UuidType)
    {
        case Uuid16Bits:
          Utils_PackTwoByteValue(uuid.uuid16, req.Uuid.Uuid16Bits);
        break;

        case Uuid128Bits:
          FLib_MemCpy(req.Uuid.Uuid128Bits, uuid.uuid128, 16);
        break;

        case Uuid32Bits:
          Utils_PackFourByteValue(uuid.uuid32, req.Uuid.Uuid32Bits);
        break;

        default:
        {
          ; /* No action required */
        }
        break;
    }

    req.DescriptorValueLength = valueLength;
    req.Value = aDescriptorValue;
    req.DescriptorAccessPermissions = (GATTDBDynamicAddCharacteristicDescriptorRequest_DescriptorAccessPermissions_t)valueAccessPermissions;

    GATTDBDynamicAddCharacteristicDescriptorRequest(&req, gFsciInterface_c);
}

/*!*************************************************************************************************
*\fn    void GATTDBDynamicAddCharacteristicDeclarationAndValue(UuidType_t uuidType, bleUuid_t uuid,
*       gattCharacteristicPropertiesBitFields_t characteristicProperties, uint16_t maxValueLength, uint16_t
*       initialValueLength, uint8_t* aInitialValue, gattAttributePermissionsBitFields_t valueAccessPermissions)
*
*\brief Sends a request to add characteristic to the database
*
* \param[in]    uuidType                    UUID type of the characteristic
* \param[in]    uuid                        Desired characteristic UUID
* \param[in]    characteristicProperties    Desired characteristic properties
* \param[in]    maxValueLength              Maximum allowed size for the characteristic value
* \param[in]    initialValueLength          Initial size of the characteristic value
* \param[in]    aInitialValue               Pointer to the characteristic value
* \param[in]    valueAccessPermissions      Characteristic access permissions
*
*\return       None
***************************************************************************************************/
static void GATTDBDynamicAddCharacteristicDeclarationAndValue
(
    UuidType_t                              UuidType,
    bleUuid_t                               uuid,
    gattCharacteristicPropertiesBitFields_t characteristicProperties,
    uint16_t                                maxValueLength,
    uint16_t                                initialValueLength,
    uint8_t*                                aInitialValue,
    gattAttributePermissionsBitFields_t     valueAccessPermissions
)
{
    GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_t req;
    req.UuidType = UuidType;
    switch (req.UuidType)
    {
        case Uuid16Bits:
          Utils_PackTwoByteValue(uuid.uuid16, req.Uuid.Uuid16Bits);
        break;

        case Uuid128Bits:
          FLib_MemCpy(req.Uuid.Uuid128Bits, uuid.uuid128, 16);
        break;

        case Uuid32Bits:
          Utils_PackFourByteValue(uuid.uuid32, req.Uuid.Uuid32Bits);
        break;

        default:
        {
          ; /* No action required */
        }
        break;
    }

    req.CharacteristicProperties = (GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_CharacteristicProperties_t)characteristicProperties;
    req.MaxValueLength = maxValueLength;
    req.InitialValueLength = initialValueLength;
    req.InitialValue = aInitialValue;
    req.ValueAccessPermissions = (GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_t)valueAccessPermissions;

    GATTDBDynamicAddCharacteristicDeclarationAndValueRequest(&req, gFsciInterface_c);
}

/*!*************************************************************************************************
*\fn    static void gattDbAddPrimarySerivceHandler(bleEvtContainer_t *pMsg)
*
*\brief Handles the response to Add Primary Serivce command
*
* \param[in]   pMsg    Pointer to event container
*
*\return       None
***************************************************************************************************/
static void gattDbAddPrimarySerivceHandler(bleEvtContainer_t* pMsg)
{
    uint16_t handle = pMsg->Data.GATTDBDynamicAddPrimaryServiceDeclarationIndication.ServiceHandle;
    bleUuid_t uuid;
    gattCharacteristicPropertiesBitFields_t characteristicProperties;
    uint16_t maxValueLength;
    uint16_t initialValueLength;
    gattAttributePermissionsBitFields_t valueAccessPermissions;

    switch (handle)
    {
        case mcGenericAccessProfileHandle_c:
        {
            uint8_t aInitialValue[17] = "NXP_WIRELESS_UART";

            /* Generic Access Profile added - add the Device Name characteristic */
            uuid.uuid16 = gBleSig_GapDeviceName_d;
            characteristicProperties = gGattCharPropRead_c;
            maxValueLength = 0U;
            initialValueLength = 17U;
            valueAccessPermissions = gPermissionFlagReadable_c;

            GATTDBDynamicAddCharacteristicDeclarationAndValue(Uuid16Bits,
                                                              uuid,
                                                              characteristicProperties,
                                                              maxValueLength,
                                                              initialValueLength,
                                                              aInitialValue,
                                                              valueAccessPermissions);
        }
        break;

        case mWUartServiceHandle_c:
        {
            uint8_t aInitialValue = 0U;

            /* Wireless UART profile added - add the UART Stream characteristic */
            FLib_MemCpy(uuid.uuid128, uuid_uart_stream, 16U);
            characteristicProperties = gGattCharPropWriteWithoutRsp_c;
            maxValueLength = gAttMaxWriteDataSize_d(gAttMaxMtu_c);
            initialValueLength = 1U;
            valueAccessPermissions = gPermissionFlagWritable_c;
            GATTDBDynamicAddCharacteristicDeclarationAndValue(Uuid128Bits,
                                                              uuid,
                                                              characteristicProperties,
                                                              maxValueLength,
                                                              initialValueLength,
                                                              &aInitialValue,
                                                              valueAccessPermissions);
        }
        break;

        case mBatteryServiceHandle_c:
        {
            uint8_t aInitialValue = 0x5AU;

            /* Battery Service Added - add the Battery Level Characteristic */
            uuid.uuid16 = gBleSig_BatteryLevel_d;
            characteristicProperties = (gGattCharPropNotify_c | gGattCharPropRead_c);
            maxValueLength = 0U;
            initialValueLength = 1U;
            valueAccessPermissions = gPermissionFlagReadable_c;
            GATTDBDynamicAddCharacteristicDeclarationAndValue(Uuid16Bits,
                                                              uuid,
                                                              characteristicProperties,
                                                              maxValueLength,
                                                              initialValueLength,
                                                              &aInitialValue,
                                                              valueAccessPermissions);
        }
        break;

        case mDeviceInfoServiceHandle_c:
        {
            uint8_t aInitialValue[3] = "NXP";

            /* Device Information Service added - add Manufacturer Name characteristic */
            uuid.uuid16 = gBleSig_ManufacturerNameString_d;
            characteristicProperties = gGattCharPropRead_c;
            maxValueLength = 0U;
            initialValueLength = 3U;
            valueAccessPermissions = gPermissionFlagReadable_c;
            GATTDBDynamicAddCharacteristicDeclarationAndValue(Uuid16Bits,
                                                              uuid,
                                                              characteristicProperties,
                                                              maxValueLength,
                                                              initialValueLength,
                                                              aInitialValue,
                                                              valueAccessPermissions);
        }
        break;

        default:
        break;
    }
}

/*!*************************************************************************************************
*\fn    static void gattDbAddCharacteristicHandler(bleEvtContainer_t *pMsg)
*
*\brief Handles the response to the Add Characteristic command
*
* \param[in]   pMsg    Pointer to event container
*
*\return       None
***************************************************************************************************/
static void gattDbAddCharacteristicHandler(bleEvtContainer_t* pMsg)
{
    uint16_t handle = pMsg->Data.GATTDBDynamicAddCharacteristicDeclarationAndValueIndication.CharacteristicHandle;
    bleUuid_t uuid;

    /* Successfully added the Device Name characteristic. */
    switch (handle)
    {
        case mcGenericAccessProfileHandle_c + 1U:
        case mcGenericAccessProfileHandle_c + 2U:
        {
            /* Add the Appearance characteristic */
            uint16_t maxValueLength = 0U;
            uint16_t initialValueLength = 2U;
            uint8_t aInitialValue[2] = {0U};
            gattAttributePermissionsBitFields_t valueAccessPermissions = gPermissionFlagReadable_c;
            gattCharacteristicPropertiesBitFields_t characteristicProperties = gGattCharPropRead_c;
            uuid.uuid16 = gBleSig_GapAppearance_d;

            GATTDBDynamicAddCharacteristicDeclarationAndValue(Uuid16Bits,
                                                              uuid,
                                                              characteristicProperties,
                                                              maxValueLength,
                                                              initialValueLength,
                                                              aInitialValue,
                                                              valueAccessPermissions);
        }
        break;

        case mcGenericAccessProfileHandle_c + 3U:
        case mcGenericAccessProfileHandle_c + 4U:
        {
            /* Successfully added the Appearance characteristic - Add the Wireless UART service */
            FLib_MemCpy(uuid.uuid128, uuid_service_wireless_uart, 16U);
            GATTDBDynamicAddPrimaryServiceDeclaration(mWUartServiceHandle_c, Uuid128Bits, uuid);
        }
        break;

        case mWUartServiceHandle_c + 1U:
        case mWUartServiceHandle_c + 2U:
        {
            /* Successfully added the UART Stream characteristic - Add the Battery service */
            mCharMonitoredHandles = handle + 1U;
            uuid.uuid16 = gBleSig_BatteryService_d;
            GATTDBDynamicAddPrimaryServiceDeclaration(mBatteryServiceHandle_c, Uuid16Bits, uuid);
        }
        break;

        case mBatteryServiceHandle_c + 1U:
        case mBatteryServiceHandle_c + 2U:
        {
            /* Successfully added the battery service characteristic
               Add the Chararacteristic Presentation Format Descriptor */
            uint16_t valueLength = 7U;
            uint8_t aDescriptorValue[7] = {0x04, 0x00, 0xAD, 0x27, 0x01, 0x01, 0x00};
            gattAttributePermissionsBitFields_t valueAccessPermissions = gPermissionFlagReadable_c;
            uuid.uuid16 = gBleSig_CharPresFormatDescriptor_d;

            GATTDBDynamicAddCharacteristicDescriptor(Uuid16Bits,
                                                     uuid,
                                                     valueLength,
                                                     aDescriptorValue,
                                                     valueAccessPermissions);
        }
        break;

        case mDeviceInfoServiceHandle_c + 1U:
        case mDeviceInfoServiceHandle_c + 2U:
        {
            /* Add the Model Number characteristic */
            gattCharacteristicPropertiesBitFields_t characteristicProperties = gGattCharPropRead_c;
            uint16_t maxValueLength = 0U;
            uint16_t initialValueLength = 18U;
            uint8_t aInitialValue[18] = "Wireless UART Demo";
            gattAttributePermissionsBitFields_t valueAccessPermissions = gPermissionFlagReadable_c;
            uuid.uuid16 = gBleSig_ModelNumberString_d;

            GATTDBDynamicAddCharacteristicDeclarationAndValue(Uuid16Bits,
                                                              uuid,
                                                              characteristicProperties,
                                                              maxValueLength,
                                                              initialValueLength,
                                                              aInitialValue,
                                                              valueAccessPermissions);
        }
        break;

        case mDeviceInfoServiceHandle_c + 3U:
        case mDeviceInfoServiceHandle_c + 4U:
        {
            /* Add the Serial Number characteristic */
            gattCharacteristicPropertiesBitFields_t characteristicProperties = gGattCharPropRead_c;
            uint16_t maxValueLength = 0U;
            uint16_t initialValueLength = 7U;
            uint8_t aInitialValue[7] = "BLESN01";
            gattAttributePermissionsBitFields_t valueAccessPermissions = gPermissionFlagReadable_c;
            uuid.uuid16 = gBleSig_SerialNumberString_d;

            GATTDBDynamicAddCharacteristicDeclarationAndValue(Uuid16Bits,
                                                              uuid,
                                                              characteristicProperties,
                                                              maxValueLength,
                                                              initialValueLength,
                                                              aInitialValue,
                                                              valueAccessPermissions);
        }
        break;

        case mDeviceInfoServiceHandle_c + 5U:
        case mDeviceInfoServiceHandle_c + 6U:
        {
            /* Add the Hardware revision characteristic */
            gattCharacteristicPropertiesBitFields_t characteristicProperties = gGattCharPropRead_c;
            uint16_t maxValueLength = 0U;
            uint16_t initialValueLength = 7U;
            uint8_t aInitialValue[7] = "MCXW345";
            gattAttributePermissionsBitFields_t valueAccessPermissions = gPermissionFlagReadable_c;
            uuid.uuid16 = gBleSig_HardwareRevisionString_d;

            GATTDBDynamicAddCharacteristicDeclarationAndValue(Uuid16Bits,
                                                              uuid,
                                                              characteristicProperties,
                                                              maxValueLength,
                                                              initialValueLength,
                                                              aInitialValue,
                                                              valueAccessPermissions);
        }
        break;

        case mDeviceInfoServiceHandle_c + 7U:
        case mDeviceInfoServiceHandle_c + 8U:
        {
            /* Add the Firmware Revision characteristic */
            gattCharacteristicPropertiesBitFields_t characteristicProperties = gGattCharPropRead_c;
            uint16_t maxValueLength = 0U;
            uint16_t initialValueLength = 5U;
            uint8_t aInitialValue[7] = "1.1.1";
            gattAttributePermissionsBitFields_t valueAccessPermissions = gPermissionFlagReadable_c;
            uuid.uuid16 = gBleSig_FirmwareRevisionString_d;

            GATTDBDynamicAddCharacteristicDeclarationAndValue(Uuid16Bits,
                                                              uuid,
                                                              characteristicProperties,
                                                              maxValueLength,
                                                              initialValueLength,
                                                              aInitialValue,
                                                              valueAccessPermissions);
        }
        break;

        case mDeviceInfoServiceHandle_c + 9U:
        case mDeviceInfoServiceHandle_c + 10U:
        {
            /* Add the Softeare Revision characteristic */
            gattCharacteristicPropertiesBitFields_t characteristicProperties = gGattCharPropRead_c;
            uint16_t maxValueLength = 0U;
            uint16_t initialValueLength = 5U;
            uint8_t aInitialValue[7] = "1.1.4";
            gattAttributePermissionsBitFields_t valueAccessPermissions = gPermissionFlagReadable_c;
            uuid.uuid16 = gBleSig_SoftwareRevisionString_d;

            GATTDBDynamicAddCharacteristicDeclarationAndValue(Uuid16Bits,
                                                              uuid,
                                                              characteristicProperties,
                                                              maxValueLength,
                                                              initialValueLength,
                                                              aInitialValue,
                                                              valueAccessPermissions);
        }
        break;

        case mDeviceInfoServiceHandle_c + 11U:
        case mDeviceInfoServiceHandle_c + 12U:
        {
            /* Register wireless UART hanle for write notifications */
            GATTServerRegisterHandlesForWriteNotificationsRequest_t req;
            req.HandleCount = 1U;
            req.AttributeHandles = &mCharMonitoredHandles;
            GATTServerRegisterHandlesForWriteNotificationsRequest(&req, gFsciInterface_c);
        }
        break;

        default:
        break;

    }
}
/*! *********************************************************************************
* @}
********************************************************************************** */
