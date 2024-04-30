/*! *********************************************************************************
* \addtogroup HOST_BBOX_UTILITY
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright(c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017, 2019, 2021, 2023 NXP
*
*
* \file
*
* This file is the source file for the Host - Blackbox GATT Database management
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble_gatt_db_app.h"
#include "host_ble.h"

#if gFsciIncluded_c
#include "FsciCommunication.h"
#endif

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

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

bleResult_t GattDb_WriteAttribute
(
    uint16_t    handle,
    uint16_t    valueLength,
    uint8_t*    aValue
)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);  
    FsciCmdMonitor(WriteAttribute, handle, valueLength, aValue);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;
}


bleResult_t GattDb_ReadAttribute
(
    uint16_t    handle,
    uint16_t    maxBytes,
    uint8_t*    aOutValue,
    uint16_t*   pOutValueLength
)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(ReadAttribute, handle, maxBytes, aOutValue, pOutValueLength);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;
}


bleResult_t GattDb_FindServiceHandle
(
    uint16_t        startHandle,
    bleUuidType_t   uuidType,
    bleUuid_t*      pUuid,
    uint16_t*       pOutServiceHandle
)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(FindServiceHandle, startHandle, uuidType, pUuid, pOutServiceHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;
}


bleResult_t GattDb_FindCharValueHandleInService
(
    uint16_t        serviceHandle,
    bleUuidType_t   uuidType,
    bleUuid_t*      pUuid,
    uint16_t*       pOutCharValueHandle
)
{
    bleResult_t result = gBleSuccess_c;
    
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(FindCharValueHandleInService, serviceHandle, uuidType, pUuid, pOutCharValueHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId); 
    
    return result;
}


bleResult_t GattDb_FindCccdHandleForCharValueHandle
(
    uint16_t        charValueHandle,
    uint16_t*       pOutCccdHandle
)
{
    bleResult_t result = gBleSuccess_c;
    
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);    
    FsciCmdMonitor(FindCccdHandleForCharValueHandle, charValueHandle, pOutCccdHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId); 
    
    return result;
}


bleResult_t GattDb_FindDescriptorHandleForCharValueHandle
(
    uint16_t        charValueHandle,
    bleUuidType_t   uuidType,
    bleUuid_t*      pUuid,
    uint16_t*       pOutDescriptorHandle
)
{
    bleResult_t result = gBleSuccess_c;
    
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(FindDescriptorHandleForCharValueHandle, charValueHandle, uuidType, pUuid, pOutDescriptorHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId); 
    
    return result;
}


bleResult_t GattDbDynamic_Init(void)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);  
    FsciCmdMonitor(InitDatabase);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;
}


bleResult_t GattDbDynamic_ReleaseDatabase(void)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);  
    FsciCmdMonitor(ReleaseDatabase);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;
}


bleResult_t GattDbDynamic_AddPrimaryServiceDeclaration
(
    uint16_t        desiredHandle,
    bleUuidType_t   serviceUuidType,
    bleUuid_t*      pServiceUuid,
    uint16_t*       pOutHandle
)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(AddPrimaryServiceDeclaration, desiredHandle, serviceUuidType, pServiceUuid, pOutHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);
    
    return result;
}


bleResult_t GattDbDynamic_AddSecondaryServiceDeclaration
(
    uint16_t        desiredHandle,
    bleUuidType_t   serviceUuidType,
    bleUuid_t*      pServiceUuid,
    uint16_t*       pOutHandle
)
{
    bleResult_t result = gBleSuccess_c;
    
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(AddSecondaryServiceDeclaration, desiredHandle, serviceUuidType, pServiceUuid, pOutHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);
    
    return result;
}


bleResult_t GattDbDynamic_AddIncludeDeclaration
(
    uint16_t        includedServiceHandle,
    uint16_t        endGroupHandle,
    bleUuidType_t   serviceUuidType,
    bleUuid_t*      pServiceUuid,
    uint16_t*       pOutHandle
)
{
    bleResult_t result = gBleSuccess_c;
    
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(AddIncludeDeclaration, includedServiceHandle, endGroupHandle, serviceUuidType, pServiceUuid, pOutHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);
    
    return result;
}


bleResult_t GattDbDynamic_AddCharacteristicDeclarationAndValue
(
    bleUuidType_t                               characteristicUuidType,
    bleUuid_t*                                  pCharacteristicUuid,
    gattCharacteristicPropertiesBitFields_t     characteristicProperties,
    uint16_t                                    maxValueLength,
    uint16_t                                    initialValueLength,
    uint8_t*                                    aInitialValue,
    gattAttributePermissionsBitFields_t         valueAccessPermissions,
    uint16_t*                                   pOutHandle
)
{
    bleResult_t result = gBleSuccess_c;
    
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(AddCharacteristicDeclarationAndValue, characteristicUuidType, pCharacteristicUuid, 
                        characteristicProperties, maxValueLength, initialValueLength, 
                        aInitialValue, valueAccessPermissions, pOutHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);

    return result;
}


bleResult_t GattDbDynamic_AddCharacteristicDescriptor
(
    bleUuidType_t                               descriptorUuidType,
    bleUuid_t*                                  pDescriptorUuid,
    uint16_t                                    descriptorValueLength,
    uint8_t*                                    aInitialValue,
    gattAttributePermissionsBitFields_t         descriptorAccessPermissions,
    uint16_t*                                   pOutHandle
)
{
    bleResult_t result = gBleSuccess_c;
    
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(AddCharacteristicDescriptor, descriptorUuidType, pDescriptorUuid, descriptorValueLength, 
                         aInitialValue, descriptorAccessPermissions, pOutHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;
}
  
  
bleResult_t GattDbDynamic_AddCccd(uint16_t* pOutHandle)
{
    bleResult_t result = gBleSuccess_c;
    
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(AddCccd, pOutHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);
    
    return result;
}

bleResult_t GattDbDynamic_AddCharAggregateFormat(uint16_t descriptorValueLength, uint8_t* pInitialValue, uint16_t* pOutHandle)
{
    bleResult_t result = gBleSuccess_c;
    
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(AddCharAggregateFormat, descriptorValueLength, pInitialValue, pOutHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);
    
    return result;
}

bleResult_t GattDbDynamic_AddCharacteristicDeclarationWithUniqueValue
(
    bleUuidType_t                               characteristicUuidType,
    bleUuid_t*                                  pCharacteristicUuid,
    gattCharacteristicPropertiesBitFields_t     characteristicProperties,
    gattAttributePermissionsBitFields_t         valueAccessPermissions,
    uint16_t*                                   pOutHandle
)
{
    bleResult_t result = gBleSuccess_c;
    
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(AddCharacteristicDeclarationWithUniqueValue, characteristicUuidType, pCharacteristicUuid, 
                        characteristicProperties, valueAccessPermissions, pOutHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    

    return result;
}


bleResult_t GattDbDynamic_RemoveService
(
    uint16_t serviceHandle
)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);  
    FsciCmdMonitor(RemoveService, serviceHandle);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;        
}


bleResult_t GattDbDynamic_RemoveCharacteristic
(
    uint16_t characteristicHandle
)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);  
    FsciCmdMonitor(RemoveCharacteristic, characteristicHandle);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;   
}

bleResult_t GattDbDynamic_AddCharDescriptorWithUniqueValue
(
    bleUuidType_t                               descriptorUuidType,
    bleUuid_t*                                  pDescriptorUuid,
    gattAttributePermissionsBitFields_t         descriptorAccessPermissions,
    uint16_t*                                   pOutHandle
)
{
    bleResult_t result = gBleSuccess_c;

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleGattDbAppOpcodeGroup_c, gBleGattDbAppStatusOpCode_c);
    FsciCmdMonitor(AddCharDescriptorWithUniqueValue, descriptorUuidType, pDescriptorUuid, descriptorAccessPermissions, pOutHandle);
    result = Ble_GetCmdStatus(TRUE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);
    
    return result;
}

/*************************************************************************************
 *************************************************************************************
 * Private functions
 *************************************************************************************
 ************************************************************************************/

/*! *********************************************************************************
* @}
********************************************************************************** */
