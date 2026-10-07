/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016 - 2023, 2025-2026 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ncp_ble_init.h"
#include "ble_host_task_config.h"

#if defined(gBoard_ExtPaSupport_d) && (gBoard_ExtPaSupport_d > 0)
#include "board_extPA.h"
#endif
#include "fwk_platform_ble.h"

#if !(defined(gNcpCore1Execution_d) && (gNcpCore1Execution_d > 0))
/* NBU/cm33_core1 apps have no access to HWParameter or DevMgr; only pull in
 * the DevMgr API when it is actually built (host core). */
#include "fwk_devmgr.h"
#endif /* gNcpCore1Execution_d */

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
#ifndef cMCU_SleepDuringBleEvents
    #define cMCU_SleepDuringBleEvents                      0
#endif /* cMCU_SleepDuringBleEvents */

#if defined(gUseHciTransportDownward_d) && gUseHciTransportDownward_d
#if defined (gBleSetMacAddrFromVendorCommand_d) && (gBleSetMacAddrFromVendorCommand_d == 1)
#define mHciSetMacAddrCommandLength_c       (8U)
#define gHciSetMacAddrCommand_c             0x0022U
#define HciCommand(opCodeGroup, opCodeCommand)\
    (((uint16_t)(opCodeGroup) & (uint16_t)0x3FU)<<(uint16_t)SHIFT10)|(uint16_t)((opCodeCommand) & 0x3FFU)
#define BT_USER_BD 254
#endif /* gBleSetMacAddrFromVendorCommand_d */
#endif

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
#if !defined(gUseHciTransportUpward_d) || (!gUseHciTransportUpward_d)
bool_t gHostInitResetController = gHostInitResetController_c;
#endif /* gUseHciTransportUpward_d */

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn           bleResult_t Ble_Initialize(gapGenericCallback_t gapGenericCallback)
*\brief        Performs full initialization of the BLE stack.
*
*\param  [in]  genericCallback    Callback used by the Host Stack to propagate GAP
*                                 generic events to the application.
*
*\return       bleResult_t        Result of the operation.
********************************************************************************** */
bleResult_t Ble_Initialize
(
    gapGenericCallback_t gapGenericCallback
)
{
#if defined(gUseHciTransportDownward_d) && gUseHciTransportDownward_d
    /* HCI Transport Init */
    if (gHciSuccess_c != Hcit_Init(Ble_HciRecvFromIsr))
    {
        return gHciTransportError_c;
    }

    /*
     * Set BD Address in Controller. Must be done after HCI init
     * and before Host init.
     */
    Ble_SetBDAddr();

    /* Check for available memory storage */
    if (!Ble_CheckMemoryStorage())
    {
        return gBleOutOfMemory_c;
    }

    /* BLE Host Tasks Init */
    if (KOSA_StatusSuccess != Ble_HostTaskInit())
    {
        return gBleOsError_c;
    }

    /* BLE Host Stack Init */
    return Ble_HostInitialize(gapGenericCallback, Hcit_SendPacket);

#elif defined(gUseHciTransportUpward_d) && gUseHciTransportUpward_d

    if (KOSA_StatusSuccess != Controller_TaskInit())
    {
        return gBleOsError_c;
    }

    /* BLE Controller Init */
    if (KOSA_StatusSuccess != Controller_Init(Hcit_SendPacket))
    {
        return gBleOsError_c;
    }

    return Controller_SendPacketToController(Hci_SendPacketToController);

#else /* gUseHciTransportUpward_d */
    /* Check for available memory storage */
    if (!Ble_CheckMemoryStorage())
    {
        return gBleOutOfMemory_c;
    }

    /* BLE Host Tasks Init */
    if (KOSA_StatusSuccess != Ble_HostTaskInit())
    {
        return gBleOsError_c;
    }

    /* BLE Host Stack Init */
    return Ble_HostInitialize(gapGenericCallback, Hcit_PktReceived);

#endif /* gUseHciTransportUpward_d */
}

/*! *********************************************************************************
*\fn           bleResult_t Ble_DeInitialize(void)
*\brief        Frees the resources not allocated by the BLE stack.
*
*\param  [in]  none
*
*\return       bleResult_t        Result of the operation.
********************************************************************************** */
bleResult_t Ble_DeInitialize(void)
{
#if defined(gUseHciTransportDownward_d) && gUseHciTransportDownward_d
    /* HCI Transport Init */
    bleResult_t result = Hcit_Deinit();
    if(result == gHciSuccess_c)
    {
         /* BLE Host Tasks DeInit */
        if (KOSA_StatusSuccess != Ble_HostTaskDeInit())
        {
            result = gBleOsError_c;
        }
    }
    return result;
#elif defined(gUseHciTransportUpward_d) && gUseHciTransportUpward_d
return gBleSuccess_c;
#else /* gUseHciTransportUpward_d */
return gBleSuccess_c;
#endif /* gUseHciTransportUpward_d */
}
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

#if defined(gUseHciTransportDownward_d) && gUseHciTransportDownward_d
void Ble_SetBDAddr(void)
{
#if !(defined(gNcpCore1Execution_d) && (gNcpCore1Execution_d > 0))
    uint8_t bleDeviceAddress[gcBleDeviceAddressSize_c] = {0};

    (void)DEVMGR_GetBdAddress(bleDeviceAddress, NULL);

#if !defined(gBleSetMacAddrFromVendorCommand_d) || (defined(gBleSetMacAddrFromVendorCommand_d) && (gBleSetMacAddrFromVendorCommand_d == 0))
    /* Set BD address by API call */
    (void)Controller_SetDeviceAddress(bleDeviceAddress);
#else
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

    FLib_MemCpy((void*)&aHciPacket[gHciCommandPacketHeaderLength_c + 2U], (const void*)bleDeviceAddress, gcBleDeviceAddressSize_c);

    /* Send HCI command */
    (void)Hcit_SendPacket(gHciCommandPacket_c, aHciPacket, gHciCommandPacketHeaderLength_c + mHciSetMacAddrCommandLength_c);
#endif /* gBleSetMacAddrFromVendorCommand_d */
#endif /* gNcpCore1Execution_d */
}
#endif

/*! *********************************************************************************
* @}
********************************************************************************** */
