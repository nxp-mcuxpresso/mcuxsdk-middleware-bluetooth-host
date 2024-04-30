/*! *********************************************************************************
* \addtogroup HOST_BBOX_UTILITY
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright(c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017, 2023 NXP
*
*
* \file
*
* This file is the source file for the Host - Blackbox L2CAP Credit Based management
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble_l2cap_cb.h"
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

bleResult_t L2ca_RegisterLeCbCallbacks(l2caLeCbDataCallback_t pCallback, l2caLeCbControlCallback_t pCtrlCallback)
{
    bleResult_t result = gBleSuccess_c;
    
    fsciBleSetL2capCbLeCbDataCallback(pCallback);
    fsciBleSetL2capCbLeCbControlCallback(pCtrlCallback);

    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleL2capCbOpcodeGroup_c, gBleL2capCbStatusOpCode_c);    
    FsciCmdMonitor(RegisterLeCbCallbacks, pCallback, pCtrlCallback);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;
}


bleResult_t L2ca_RegisterLePsm(uint16_t lePsm, uint16_t lePsmMtu)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleL2capCbOpcodeGroup_c, gBleL2capCbStatusOpCode_c);    
    FsciCmdMonitor(RegisterLePsm, lePsm, lePsmMtu);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;    
}


bleResult_t L2ca_DeregisterLePsm(uint16_t lePsm)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleL2capCbOpcodeGroup_c, gBleL2capCbStatusOpCode_c);  
    FsciCmdMonitor(DeregisterLePsm, lePsm);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;    
}


bleResult_t L2ca_ConnectLePsm(uint16_t lePsm, deviceId_t deviceId, uint16_t initialCredits)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleL2capCbOpcodeGroup_c, gBleL2capCbStatusOpCode_c);  
    FsciCmdMonitor(ConnectLePsm, lePsm, deviceId, initialCredits);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;    
}


bleResult_t L2ca_DisconnectLeCbChannel(deviceId_t deviceId, uint16_t channelId)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleL2capCbOpcodeGroup_c, gBleL2capCbStatusOpCode_c);  
    FsciCmdMonitor(DisconnectLeCbChannel, deviceId, channelId);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;   
}


bleResult_t L2ca_CancelConnection(uint16_t lePsm, deviceId_t deviceId, l2caLeCbConnectionRequestResult_t refuseReason)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleL2capCbOpcodeGroup_c, gBleL2capCbStatusOpCode_c);  
    FsciCmdMonitor(CancelConnection, lePsm, deviceId, refuseReason);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;    
}

 
bleResult_t L2ca_SendLeCbData(deviceId_t deviceId, uint16_t channelId, uint8_t*pPacket, uint16_t packetLength)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleL2capCbOpcodeGroup_c, gBleL2capCbStatusOpCode_c);  
    FsciCmdMonitor(SendLeCbData, deviceId, channelId, pPacket, packetLength);
    result = Ble_GetCmdStatus(FALSE);
    FSCI_HostSyncUnlock(fsciBleInterfaceId);    
    
    return result;    
}


bleResult_t L2ca_SendLeCredit(deviceId_t deviceId, uint16_t channelId, uint16_t credits)
{
    bleResult_t result = gBleSuccess_c;
  
    FSCI_HostSyncLock(fsciBleInterfaceId, gFsciBleL2capCbOpcodeGroup_c, gBleL2capCbStatusOpCode_c);  
    FsciCmdMonitor(SendLeCredit, deviceId, channelId, credits);
    result = Ble_GetCmdStatus(FALSE);
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
