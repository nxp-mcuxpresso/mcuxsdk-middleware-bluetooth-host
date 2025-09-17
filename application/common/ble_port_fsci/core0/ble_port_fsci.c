/*! *********************************************************************************
* Copyright 2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
/* Framework / Components */
#include "fsl_component_mem_manager.h"
#include "fsl_component_serial_manager.h"
#include "FunctionLib.h"
#include "FsciInterface.h"
#include "FsciCommunication.h"

/* Application */
#include "app.h"
#include "fwk_platform_ble.h"
#include "platform_core0.h"
#include "app_nvm_core0.h"

#include "ble_port_fsci_op.h"
#include "fsl_component_led_core0.h"
#include "fsl_component_button_core0.h"
#include "fsl_shell_core0.h"
#include "fsl_adapter_reset_core0.h"
#include "sensors_core0.h"

#include "fsci_ble_interface.h"

/*************************************************************************************
**************************************************************************************
* Private macros
*************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************/
uint32_t mFsciInterfaceId = 0;
pfFsciPortOpHandler_t mOpHandlers[255] = {0};

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
static void BLE_PortFsciSend(uint8_t *pPacket, uint16_t packetLen, bool_t freePacket);
static void BLE_PortFsciRxCallBack(uint8_t packetType, uint8_t *data, uint16_t len);
static void BLE_PortFsciRxHandler(void* pData, void* param, uint32_t fsciInterface);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn           void BLE_PortFsciInit(void)
*\brief        Initializes application specific functionality before the BLE stack init.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void BLE_PortFsciInit(void)
{
    union
    {
        void *pVoid;
        pfFSCI_Send_t pfFSCI_Send;
    } fsciHandle;
    fsciHandle.pfFSCI_Send = &BLE_PortFsciSend;
    g_fsciHandleList[0] = fsciHandle.pVoid;

    /* Init FSCI */
    FSCI_commInit(g_fsciHandleList);

    RFMC->RF2P4GHZ_CFG |= RFMC_RF2P4GHZ_CFG_FORCE_DBG_PWRUP_ACK_MASK;
    CMC0->DBGCTL &= ~CMC_DBGCTL_SOD_MASK;

    /* Initialize rpmsg */
    PLATFORM_InitTimerManager();
    PLATFORM_InitBle();
    PLATFORM_SetHciRxCallback(BLE_PortFsciRxCallBack);

#if defined(gAppDisableControllerLowPower_d) && (gAppDisableControllerLowPower_d > 0)
    {
        /* Disallow Controller low power entry
         * Depending on the platform, this can concern multiple controllers
         * Controller low power is always enabled by default, so this should be
         * called mainly for debug purpose
         */
        PLATFORM_DisableControllerLowPower();
    }
#endif
#if defined(gAppHighNBUClockFrequency_d) && (gAppHighNBUClockFrequency_d > 0)
    /* some Link layer use cases require radio core (NBU) to run at higher frequency :
     * Increase LDO core voltage to 1.1v - make sure the DCDC output voltage is at least 1.35mV
     * Request Radio core to switch to higher frequency
     * @warning : make sure LDO core is not decreased to lower voltage after this step (when going to low power for instance)
     **/
    PLATFORM_SetLdoCoreNormalDriveVoltage();
    PLATFORM_SetNbuConstraintFrequency(PLATFORM_NBU_MIN_FREQ_64MHZ);
#endif

    /* Register BLE handlers in FSCI */
    FSCI_RegisterOpGroup(BLE_PORT_FSCI_OG,
        gFsciMonitorMode_c,
        BLE_PortFsciRxHandler,
        NULL,
        mFsciInterfaceId);

    /* Initialize core1 components */
#if defined (gFsciComponentLedCore0_d) && (gFsciComponentLedCore0_d > 0)
    LED_InitCore0Handlers();
#endif
#if defined (gFsciComponentButtonCore0_d) && (gFsciComponentButtonCore0_d > 0)
    Button_InitCore0Handlers();
#endif
#if defined (gFsciComponentShellCore0_d) && (gFsciComponentShellCore0_d > 0)
    Shell_InitCore0Handlers();
#endif
#if defined (gFsciComponentResetCore0_d) && (gFsciComponentResetCore0_d > 0)
    Reset_InitCore0Handlers();
#endif
#if defined (gFsciComponentPlatformCore0_d) && (gFsciComponentPlatformCore0_d > 0)
    Platform_InitCore0Handlers();
#endif
#if defined(gAppUseNvm_d) && (gAppUseNvm_d > 0)
    (void)AppNvm_InitCore0Handlers();
#endif /* gAppUseNvm_d */
#if defined (gFsciComponentSensorsCore0_d) && (gFsciComponentSensorsCore0_d > 0)
    SENSORS_InitCore0Handlers();
#endif
}

void BLE_PortFsciRegisterOpHandler(uint8_t op, pfFsciPortOpHandler_t pfHandler)
{
    mOpHandlers[op] = pfHandler;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\brief        Helper function to send a message to Core 0 via FSCI
*
*\param  [in]  pPacket      Pointer to the message to be sent
*\param  [in]  packetLen    Message length
*\param  [in] freePacket    TRUE if the message should be freed after sending, FALSE otherwise
*
*\retval       void.
********************************************************************************** */
static void BLE_PortFsciSend(uint8_t *pPacket, uint16_t packetLen, bool_t freePacket)
{
    (void)PLATFORM_SendHciMessage(pPacket, (uint32_t)packetLen);
    if (freePacket)
    {
        (void)MEM_BufferFree(pPacket);
    }
}

/*! *********************************************************************************
*\brief        RPMSG callback function. Transfers the message to the FSIC module.
*
*\param  [in]  packetType   Packet type (ignored)
*\param  [in]  data         Pointer to the FSCI message
*\param  [in]  len          Message length
*
*\retval       void.
********************************************************************************** */
static void BLE_PortFsciRxCallBack(uint8_t packetType, uint8_t *data, uint16_t len)
{
    uint8_t *pPacketBuffer = MEM_BufferAlloc((uint32_t)len + 1U);

    if (pPacketBuffer != NULL)
    {
        pPacketBuffer[0] = packetType;
        FLib_MemCpy(&pPacketBuffer[1], data, len);

        FSCI_receivePacket(pPacketBuffer);
    }
}

/*! *********************************************************************************
*\brief        FSCI callback function. Calls the registered handler for the given opcode.
*
*\param  [in]  pData          Pointer to the FSCI message
*\param  [in]  param          Pointer to the handler function parameter
*\param  [in]  fsciInterface  FSCI interface on which the message was received
*
*\retval       void.
********************************************************************************** */
static void BLE_PortFsciRxHandler(void* pData, void* param, uint32_t fsciInterface)
{
    clientPacket_t* pClientPacket   = (clientPacket_t*)pData;

    if (mOpHandlers[pClientPacket->structured.header.opCode] != NULL)
    {
        mOpHandlers[pClientPacket->structured.header.opCode](
            pClientPacket->structured.header.opCode,
            pClientPacket->structured.header.len,
            pClientPacket->structured.payload);
    }

    (void)MEM_BufferFree(pData);
}

/*! *********************************************************************************
* @}
********************************************************************************** */
