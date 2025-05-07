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
#include "fsci_ble_interface.h"
#include "RNG_Interface.h"
#include "ble_port_fsci.h"
#include "fwk_platform_ble.h"
#include "ble_port_fsci_op.h"
#include "app_conn.h"
#include "platform.h"
#include "app_nvm.h"

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
* Public memory declarations
*************************************************************************************/
serial_handle_t g_fsciHandleList[gFsciIncluded_c];
uint32_t mFsciInterfaceId = 0;

OSA_EVENT_HANDLE_DEFINE(mNcpHostEvent);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************/
static uint8_t mpBlockOpIdx = (1U << 0U); /* First OSA event index */
static pfFsciPortOpHandler_t mOpHandlers[255] = {0};

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
/* Helper function to send a message to Core 0 via FSCI */
static void AppFSCI_Send(uint8_t *pPacket, uint16_t packetLen, bool_t freePacket);
/* Helper function to handle FSCI messages */
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
    fsciHandle.pfFSCI_Send = &AppFSCI_Send;
    g_fsciHandleList[0] = fsciHandle.pVoid;

    /* Init FSCI */
    FSCI_commInit(g_fsciHandleList);

    /* Register BLE handlers in FSCI */
    FSCI_RegisterOpGroup(BLE_PORT_FSCI_OG,
        gFsciMonitorMode_c,
        BLE_PortFsciRxHandler,
        NULL,
        mFsciInterfaceId);

    RNG_Init();

    /* Initialize events */
    (void)OSA_EventCreate(mNcpHostEvent, (uint8_t)TRUE);

    /* Read and set device address */
    BleApp_ReadPublicDeviceAddress();

#if (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U))
    /* Init NVM */
    (void)App_NcpNvmInit();
#endif /* (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U)) */

    /* Initialize core1 application */
    BluetoothLEHost_AppInit();
}

/*! *********************************************************************************
*\fn           void BLE_PortFsciRegisterOpHandler(uint8_t op, pfFsciPortOpHandler_t pfHandler)
*\brief        Register a FSCI handler for the specified opcode
*
*\param  [in]  op           opcode corresponding to the given handler.
*\param  [in]  pfHandler    Pointer to the command handler
*
*\retval       void.
********************************************************************************** */
void BLE_PortFsciRegisterOpHandler(uint8_t op, pfFsciPortOpHandler_t pfHandler)
{
    mOpHandlers[op] = pfHandler;
}

/*! *********************************************************************************
*\fn           void BLE_PortFsciRegisterBlockingEvent(void)
*\brief        Register a index to be used to unblock Core 0 for a specific blocking operation
*
*\param  [in]  none
*
*\retval       Index to be used when calliing BLE_PortFsciUnblock()
********************************************************************************** */
uint8_t BLE_PortFsciRegisterBlockingEvent (void)
{
    uint8_t currntIdx = mpBlockOpIdx;

    /* Increase index */
    mpBlockOpIdx = (mpBlockOpIdx << 1U);

    return currntIdx;
}

/*! *********************************************************************************
*\fn           void BLE_PortFsciBlock(uint8_t evtFlag)
*\brief        Block core 1 waiting for the corresponding event to be set after a message
*              is received from core 0
*
*\param  [in]  evtFlag        Event flag to wait for
*
*\retval       void.
********************************************************************************** */
void BLE_PortFsciBlock(uint8_t evtFlag)
{
    osa_event_flags_t event = 0U;

    (void)OSA_EventWait((osa_event_handle_t)mNcpHostEvent,
                        evtFlag,
                        (uint8_t)FALSE,
                        osaWaitForever_c,
                        &event);

}

/*! *********************************************************************************
*\fn           void BLE_PortFsciUnblock(uint8_t operation)
*\brief        Unblock core 1 after receiving a message from core 0
*
*\param  [in]  operation    Blocking operation from which to free core 1
*
*\retval       void.
********************************************************************************** */
void BLE_PortFsciUnblock(uint8_t operation)
{
    (void)OSA_EventSet(mNcpHostEvent, operation);
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn           void AppFSCI_Send(uint8_t *pPacket, uint16_t packetLen, bool_t freePacket)
*\brief        Helper function to send a message to Core 0 via FSCI
*
*\param  [in]  pPacket      Pointer to the message to be sent
*\param  [in]  packetLen    Message length
*\param  [in] freePacket    TRUE if the message should be freed after sending, FALSE otherwise
*
*\retval       void.
********************************************************************************** */
static void AppFSCI_Send(uint8_t *pPacket, uint16_t packetLen, bool_t freePacket)
{
    (void)PLATFORM_SendHciMessage(pPacket, (uint32_t)packetLen);

    if (freePacket)
    {
        (void)MEM_BufferFree(pPacket);
    }
}

/*! *********************************************************************************
*\fn           void AppFSCI_RxCallBack(uint8_t packetType, uint8_t *data, uint16_t len)
*\brief        RPMSG callback function. Transfers the message to the FSCI module.
*
*\param  [in]  packetType   Packet type (ignored)
*\param  [in]  data         Pointer to the FSCI message
*\param  [in]  len          Message length
*
*\retval       void.
********************************************************************************** */
void AppFSCI_RxCallBack(uint8_t packetType, uint8_t *data, uint16_t len)
{
    uint8_t *pPacketBuffer = MEM_BufferAlloc((uint32_t)len + 1U);

    if (pPacketBuffer != NULL)
    {
        pPacketBuffer[0] = packetType;
        FLib_MemCpy(&pPacketBuffer[1], data, len);

        /* Pass message on to FSCI task */
        FSCI_receivePacket(pPacketBuffer);
    }
}

/*! *********************************************************************************
*\fn           void BLE_PortFsciRxHandler(void* pData, void* param, uint32_t fsciInterface)
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

    /* Call registered handler function */
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
