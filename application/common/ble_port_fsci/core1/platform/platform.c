/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file btcs_server_interface.h
*
* This is a header file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Includes
*************************************************************************************
************************************************************************************/
#include "fsl_component_mem_manager.h"
#include "ble_port_fsci_op.h"
#include "FsciInterface.h"
#include "ble_general.h"
#include "fsci_ble_types.h"
#include "platform.h"
#include "FsciCommunication.h"
#include "ble_port_fsci.h"

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static uint8_t mBlockTaskReadAddrIdx = 0U;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
/* Handler for the FSCI Write Public Device address command */
static void BleApp_HandleWritePublicDeviceAddress(uint8_t opc, uint8_t len, void *pParam);
/* This function is used to set the BD address in the Controller. */
static void Ble_SetBDAddr(bleDeviceAddress_t bdAddr);

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/
#define gHciVendorSpecificDebugCommands_c    (0x03FU)
#define mHciSetMacAddrCommandLength_c        (0x8U)
#define gHciSetMacAddrCommand_c              (0x0022U)

#define BT_USER_BD                          (254)
#define gHciCommandPacketHeaderLength_c     (3U)

#define HciCommand(opCodeGroup, opCodeCommand)\
    (((uint16_t)(opCodeGroup) & (uint16_t)0x3FU)<<(uint16_t)SHIFT10)|(uint16_t)((opCodeCommand) & 0x3FFU)

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\private
*\fn           static bleResult_t BleApp_ReadPublicDeviceAddress(void)
*\brief        Request Core 0 application to send the Public Device Address.
*
*\param  [in]  none
*
*\retval       bleResult_t  gBleOutOfMemory_c in case of memory allocation failure
*                           gBleSuccess_c otherwise.
********************************************************************************** */
bleResult_t BleApp_ReadPublicDeviceAddress(void)
{
    bleResult_t result = gBleSuccess_c;
    uint32_t fsciDataSize = 0U;

    mBlockTaskReadAddrIdx = BLE_PortFsciRegisterBlockingEvent();

    FSCI_transmitPayload(BLE_PORT_FSCI_OG, g_AppReadPublicDeviceAddress_c,
                         NULL, fsciDataSize, gFsciInterface_c);

    BLE_PortFsciRegisterOpHandler(g_AppWritePublicDeviceAddress_c, BleApp_HandleWritePublicDeviceAddress);

    BLE_PortFsciBlock(mBlockTaskReadAddrIdx);

    return result;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\brief        Used to process the FSCI Write Public Device address command on the
*              application task.
*
*\param  [in]  opc      FSCI packet opcode.
*\param  [in]  len      FSCI packet len.
*\param  [in]  pParam   FSCI packet payload.
*
*\retval       void.
********************************************************************************** */
static void BleApp_HandleWritePublicDeviceAddress
(
    uint8_t opc,
    uint8_t len,
    void *pParam
)
{
    (void)len;

    uint8_t*        pBuffer         = (uint8_t*)pParam;
    bleDeviceAddress_t deviceAddress = {0U};

    /* Get Device Address from buffer */
    fsciBleGetArrayFromBuffer(deviceAddress, pBuffer, ((uint32_t)gcBleDeviceAddressSize_c));
    /* Set address in the Controller */
    Ble_SetBDAddr(deviceAddress);

    /* Resume application initialization */
    BLE_PortFsciUnblock(mBlockTaskReadAddrIdx);
}

/*! *********************************************************************************
*\brief        This function is used to set the BD address in the Controller.
*
*\param  [in]  The address to be set
*
*\retval       none
********************************************************************************** */
static void Ble_SetBDAddr(bleDeviceAddress_t bdAddr)
{
    /* Set BD address by HCI message */
    uint8_t aHciPacket[mHciSetMacAddrCommandLength_c + gHciCommandPacketHeaderLength_c];
    uint16_t opcode = HciCommand(gHciVendorSpecificDebugCommands_c, gHciSetMacAddrCommand_c);

    /* Set HCI opcode */
    FLib_MemCpy((void*)aHciPacket, (const void*)&opcode, 2U);
    /* Set HCI parameter length */
    aHciPacket[2] = (uint8_t)mHciSetMacAddrCommandLength_c;
    /* Set command parameter ID */
    aHciPacket[3] = (uint8_t)BT_USER_BD;
    /* Set command parameter length */
    aHciPacket[4] = (uint8_t)6U;

    FLib_MemCpy((void*)&aHciPacket[gHciCommandPacketHeaderLength_c + 2U], (const void*)bdAddr, gcBleDeviceAddressSize_c);

    /* Send HCI command */
    (void)Hcit_PktReceived(gHciCommandPacket_c, aHciPacket, gHciCommandPacketHeaderLength_c + mHciSetMacAddrCommandLength_c);
}