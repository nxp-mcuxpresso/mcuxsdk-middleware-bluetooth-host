/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file
*
* This is a source file for the common application NVM code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#if defined (gFsciComponentPlatformCore0_d) && (gFsciComponentPlatformCore0_d > 0)
/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "fsl_component_mem_manager.h"
#include "ble_port_fsci_op.h"
#include "FsciInterface.h"
#include "fsci_ble_types.h"
#include "fwk_platform_ble.h"
#include "fwk_platform_lcl.h"
#include "app.h"
#include "ble_general.h"

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static void BleApp_HandleReadPublicAddress(uint8_t opc, uint8_t len, void *pData);
static void BleApp_HandlePlatformInitLcl(uint8_t opc, uint8_t len, void *pData);
static void BleApp_HandlePlatformInitLclGpioDebug(uint8_t opc, uint8_t len, void *pData);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\brief        Initialize Core 0 platform operations handlers
*
*\param  [in]  none
*
*\retval       none
********************************************************************************** */
void Platform_InitCore0Handlers()
{
    BLE_PortFsciRegisterOpHandler(g_AppReadPublicDeviceAddress_c, BleApp_HandleReadPublicAddress);
    BLE_PortFsciRegisterOpHandler(g_PlatformInitLclOpCode_c, BleApp_HandlePlatformInitLcl);
    BLE_PortFsciRegisterOpHandler(g_PlatformInitLclGpioDebugOpCode_c, BleApp_HandlePlatformInitLclGpioDebug);
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\brief        Used to process the FSCI Read Public Device address command
*
*\param  [in]  opc      FSCI packet opcode.
*\param  [in]  len      FSCI packet len.
*\param  [in]  pParam   FSCI packet payload.
*
*\retval       void.
********************************************************************************** */
static void BleApp_HandleReadPublicAddress
(
    uint8_t opc,
    uint8_t len,
    void *pData
)
{
    (void)pData;

    /* FSCI payload size is 7 (1 octet reset field and 6 octets for the address) */
    uint8_t *pClientPacket;
    uint8_t *pBuffer;
    uint8_t aBdAddr[gcBleDeviceAddressSize_c] = {0U};

    PLATFORM_GetBDAddr(aBdAddr);

    /* Build FSCI Request */
    pClientPacket = MEM_BufferAlloc(gcBleDeviceAddressSize_c);

    if (pClientPacket != NULL)
    {
        pBuffer = pClientPacket;

        /* Add address */
        fsciBleGetBufferFromArray(aBdAddr, pBuffer, gcBleDeviceAddressSize_c);

        /* Transmit packet */
        FSCI_transmitPayload(BLE_PORT_FSCI_OG, g_AppWritePublicDeviceAddress_c,
                             pClientPacket, gcBleDeviceAddressSize_c, gFsciInterface_c);

        (void)MEM_BufferFree(pClientPacket);
    }
}

/*! *********************************************************************************
*\brief        Used to process the Platform Init Lcl command
*
*\param  [in]  opc      FSCI packet opcode.
*\param  [in]  len      FSCI packet len.
*\param  [in]  pParam   FSCI packet payload.
*
*\retval       void.
********************************************************************************** */
static void BleApp_HandlePlatformInitLcl
(
    uint8_t opc,
    uint8_t len,
    void *pData
)
{
    /* Configure pinmux for antenna switching */
    (void)PLATFORM_InitLcl();
}

/*! *********************************************************************************
*\brief        Used to process the Platform Init Lcl Gpio Debug command
*
*\param  [in]  opc      FSCI packet opcode.
*\param  [in]  len      FSCI packet len.
*\param  [in]  pParam   FSCI packet payload.
*
*\retval       void.
********************************************************************************** */
static void BleApp_HandlePlatformInitLclGpioDebug
(
    uint8_t opc,
    uint8_t len,
    void *pData
)
{
    /* Antenna switching enabled also in Debug mode */
    PLATFORM_InitLclGpioDebug(false);
}
#endif /* gFsciComponentPlatformCore0_d */