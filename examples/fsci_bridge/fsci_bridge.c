/*! *********************************************************************************
* \addtogroup FSCI BLE application
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2024-2025 NXP
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
/* Framework / Drivers */
#include "FsciInterface.h"

#include "fsl_component_button.h"
#include "fsl_component_led.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_serial_manager.h"
#include "fsl_component_timer_manager.h"

#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)
#include "PWR_Interface.h"
#endif
#include "FsciCommunication.h"
#include "FsciCommands.h"
#include "app.h"

/*  Application */
#include "fsci_bridge.h"
#include "app_conn.h"
#include "fwk_platform_ble.h"
#include "RNG_Interface.h"
#include "host_ble_init.h"
#include "fsci_ble_types.h"
#if defined(gAppUseNvm_d) && (gAppUseNvm_d > 0)
#include "NVM_Interface.h"
#include "host_app_nvm.h"
#endif /* defined(gAppUseNvm_d) && (gAppUseNvm_d > 0) */

/************************************************************************************
*************************************************************************************
* Private type definitions and macros
*************************************************************************************
************************************************************************************/
#define APP_SERIAL_INTERFACE_ID         0
#define mAppFsciOpcodeGroup_c           0x49    /* gFsciBleGapOpcodeGroup_c + 1 */
/*! FSCI operation group for GAP */
#define gFsciBleGapOpcodeGroup_c                    0x48U
#define gBleCtrlWritePublicDeviceAddressOpCode_c    0x48U
#define gBleGapCmdReadPublicDeviceAddressOpCode_c   0x25U
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static messaging_t mAppCbInputQueue;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
OSA_EVENT_HANDLE_DEFINE(mAppEvent);

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)
static void BleApp_ChangeLowPowerModeConstraints(uint8_t lpMode);
#endif
static void BleApp_HandleReadPublicAddress(void *pParam);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
*************************************************************************************/
#ifndef SDK_OS_FREE_RTOS
/*! *********************************************************************************
 * \fn           void BluetoothLEHost_IsMessagePending(void)
 * \brief        This function checks whether Messages are pending to be processed.
 *
 * \param  [in]  none.
 *
 * \retval       TRUE if pending messages.
********************************************************************************** */
bool BluetoothLEHost_IsMessagePending(void)
{
    bool ret = FALSE;
     /* Check for existing messages in queue */
    if (MSG_QueueGetHead(&mAppCbInputQueue) != NULL) 
    {
        ret = TRUE;
    }
    return ret;
}
#endif /* SDK_OS_FREE_RTOS */

#if !defined(SDK_OS_FREE_RTOS) || (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))
/*! *********************************************************************************
 * \private
 * \fn           void BluetoothLEHost_IsConnectivityTaskToProcess(void)
 * \brief        Returns if there is Connectivity background task to process.
 *
 * \param  [in]  none.
 *
 * \retval       TRUE     If there is a connectivity task to process
 * \retval       FALSE    If there is no connectivity task to process
********************************************************************************** */
bool_t BluetoothLEHost_IsConnectivityTaskToProcess(void)
{
    bool_t isConnectivityTaskToProcess = FALSE;

    do
    {
#if defined(gAppUseNvm_d) && (gAppUseNvm_d > 0)
        if (NvIsPendingOperation())
        {
            isConnectivityTaskToProcess = TRUE;
            break;
        }
#endif /* gAppUseNvm_d */

#if defined (gAppOtaASyncFlashTransactions_c) && (gAppOtaASyncFlashTransactions_c > 0)
        if (OTA_IsTransactionPending())
        {
            isConnectivityTaskToProcess = TRUE;
            break;
        }
#endif
    }while(false);

    return isConnectivityTaskToProcess;
}
#endif /* !defined(SDK_OS_FREE_RTOS) || (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)) */

/*! *********************************************************************************
 * \brief   Stub introduced to get the start_task function
 *          in main.c file compiled after app_conn.c has been
 *          excluded from build.
 * \param[in] none
 *
 * \return void
 *
 ********************************************************************************** */
void BluetoothLEHost_HandleMessages(void)
{
#ifdef SDK_OS_FREE_RTOS
    osa_event_flags_t event = 0U;

    (void)OSA_EventWait((osa_event_handle_t)mAppEvent,
                        osaEventFlagsAll_c,
                        (uint8_t)FALSE,
                        gAppTaskWaitTimeout_ms_c ,
                        &event);
#endif /* SDK_OS_FREE_RTOS */

    /* Check for existing messages in queue */
    if (MSG_QueueGetHead(&mAppCbInputQueue) != NULL)
    {
        /* Pointer for storing the callback messages. */
        appMsgCallback_t *pMsgIn = MSG_QueueGetHead(&mAppCbInputQueue);

        if (pMsgIn != NULL)
        {
            /* Execute callback handler */
            if (pMsgIn->handler != NULL)
            {
                pMsgIn->handler(pMsgIn->param);
            }

            /* Messages must always be freed. */
            (void)MSG_Free(pMsgIn);
        }
    }

#ifdef SDK_OS_FREE_RTOS
    /* Signal the main_thread again if there are more messages pending */
    event = (MSG_QueueGetHead(&mAppCbInputQueue) != NULL) ? gAppEvtAppCallback_c : 0U;

    if (event != 0U)
    {
        (void)OSA_EventSet((osa_event_handle_t)mAppEvent, gAppEvtAppCallback_c);
    }
#endif /* SDK_OS_FREE_RTOS */
}

/*! *********************************************************************************
 * \private
 * \fn           void BluetoothLEHost_ProcessIdleTask(void)
 * \brief        Handles Connectivity background task, usually executed from Idle task.
 *
 * \param  [in]  none.
 *
 * \retval       void.
 ********************************************************************************** */
void BluetoothLEHost_ProcessIdleTask(void)
{
#if defined(gAppUseNvm_d) && (gAppUseNvm_d > 0)
    if (NvIdle() == 0)
#endif /* gAppUseNvm_d */
    {
#if defined (gAppOtaASyncFlashTransactions_c) && (gAppOtaASyncFlashTransactions_c > 0)
        if (OTA_TransactionResume() == 0)
#endif
        {
            if (RNG_IsReseedNeeded())
            {
                (void)RNG_SetSeed();
            }
        }
    }
}

/*! *********************************************************************************
 * \brief   This function is registered to be called from PLATFORM_HciRpmsgRxCallback
 *          instead of Hcit_RxCallBack in hcit_generic_adapter_interface.c.
 * \param[in] packetType command/data/event
 * \param[in] data       pointer to the packet
 * \param[in] len        length of the packet
 *
 * \return void
 *
 ********************************************************************************** */
static void ReceiveFromNBUCallback(uint8_t packetType, uint8_t *data, uint16_t len)
{
    clientPacket_t* pSerialPacket = NULL;

    /* Increase by 1 the lenght to insert the packet type */
    pSerialPacket = MEM_BufferAlloc((uint32_t)len + 1U);
    if (pSerialPacket != NULL)
    {
        pSerialPacket->raw[0] = packetType;
        FLib_MemCpy(&pSerialPacket->raw[1], (uint8_t*)data, len);
        if ((pSerialPacket->headerAndStatus.header.opGroup == gFsciBleGapOpcodeGroup_c) &&
            (pSerialPacket->headerAndStatus.header.opCode == gBleGapCmdReadPublicDeviceAddressOpCode_c))
        {
            (void)MEM_BufferFree(pSerialPacket);
            (void)App_PostCallbackMessage(BleApp_HandleReadPublicAddress, NULL);
        }
#if defined(gAppUseNvmOnFsciBridge_d) && (gAppUseNvmOnFsciBridge_d == 1)
        else if (pSerialPacket->headerAndStatus.header.opGroup == gFsciAppBleNvmCbOpcodeGroup_c)
        {
            App_FsciBleNvmCbHandler(pSerialPacket, NULL, 0U);
        }
#endif
        else
        {
            FSCI_transmitFormatedPacket(pSerialPacket, APP_SERIAL_INTERFACE_ID);
        }
    }
}

/*! *********************************************************************************
 * \brief   This function replaces the one in FsciMain(excluded from build) to
 *          re-direct the FSCI packets received to RPMSG.
 * \param[in] pPacket a pointer to the message payload
 * \param[in] fsciInterface the interface on which the data was received
 *
 * \return the status of the operation
 *
 ********************************************************************************** */
gFsciStatus_t FSCI_ProcessRxPkt(clientPacket_t *pPacket, uint32_t fsciInterface)
{
    gFsciStatus_t status = gFsciSuccess_c;

    if ((pPacket->structured.header.opGroup == (uint8_t)gFSCI_ReqOpcodeGroup_c) &&
        (pPacket->structured.header.opCode == (uint8_t)mFsciMsgResetCPUReq_c))
    {
        (void)FSCI_MsgResetCPUReqFunc(pPacket, fsciInterface);
    }
    else
    {
        if (PLATFORM_SendHciMessage(pPacket->raw,
            pPacket->structured.header.len + sizeof(clientPacketHdr_t) + 1U) != 0)
        {
            status = gFsciError_c;
        }
    }
    (void)MEM_BufferFree(pPacket);

    return status;
}

/*! *********************************************************************************
 *\fn           void BluetoothLEHost_AppInit(void)
 *\brief        This is the initialization function for each application.
 *              This function should contain all the initialization code required
 *              by the bluetooth demo.
 *
 *\param  [in]  none.
 *
 *\retval       void.
 ********************************************************************************** */
void BluetoothLEHost_AppInit(void)
{
    /* Init FSCI */
    (void)MEM_Init();
    FSCI_commInit( g_fsciHandleList );
    PLATFORM_SetHciRxCallback(&ReceiveFromNBUCallback);
    (void)RNG_Init();
#if defined(gAppUseNvm_d) && (gAppUseNvm_d > 0)
    /* Initialize NV module */
    (void)NvModuleInit();
    (void)App_HostNvmInit();
#endif /* gAppUseNvm_d */
    /* Prepare callback input queue.*/
    MSG_QueueInit(&mAppCbInputQueue);
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)
    BleApp_ChangeLowPowerModeConstraints(gAppLowPowerModeConstraints_c);
#endif
    /* Create application event */
    (void)OSA_EventCreate(mAppEvent, (uint8_t)TRUE);
}

/*! *********************************************************************************
\fn            bleResult_t App_PostCallbackMessage(
*                  appCallbackHandler_t   handler,
*                  appCallbackParam_t     param
               )
*\brief        Store a callback message in the Cb App queue and signal application.
*
*\param  [in]  handler              Callback handler.
*\param  [in]  param                Callback parameter.
*
*\retval       gBleOutOfMemory_c    Message allocation fail.
*\retval       gBleSuccess_c        Successful addition to the Cb App queue.
********************************************************************************** */
bleResult_t App_PostCallbackMessage
(
    appCallbackHandler_t   handler,
    appCallbackParam_t     param
)
{
    appMsgCallback_t *pMsgIn = NULL;

    /* Allocate a buffer with enough space to store the packet */
    pMsgIn = MSG_Alloc(sizeof (appMsgCallback_t));

    if (pMsgIn == NULL)
    {
        return gBleOutOfMemory_c;
    }

    pMsgIn->handler = handler;
    pMsgIn->param = param;

    /* Put message in the Cb App queue */
    (void)MSG_QueueAddTail(&mAppCbInputQueue, pMsgIn);

    /* Signal application */
    (void)OSA_EventSet(mAppEvent, gAppEvtAppCallback_c);

    return gBleSuccess_c;
}

/*! *********************************************************************************
 *\fn           void App_SetBDAddr(void)
 *\brief        This is used to set the Bluetooth LE Device Address on the NCP.
 *
 *\param  [in]  none.
 *
 *\retval       void.
 ********************************************************************************** */
void App_SetBDAddr(void)
{
    /* FSCI payload size is 7 (1 octet reset field and 6 octets for the address) */
    uint32_t fsciDataSize = sizeof(uint8_t) + gcBleDeviceAddressSize_c;
    uint8_t *pClientPacket;
    uint8_t *pBuffer;
    uint8_t aBdAddr[gcBleDeviceAddressSize_c] = {0U};

    PLATFORM_GetBDAddr(aBdAddr);

    /* Build FSCI Request */
    pClientPacket = MEM_BufferAlloc(fsciDataSize);
    
    if (pClientPacket != NULL)
    {
        pBuffer = pClientPacket;
        /* Add reset field (ignored) */
        fsciBleGetBufferFromBoolValue(FALSE, pBuffer);
        /* Add address */
        fsciBleGetBufferFromArray(aBdAddr, pBuffer, gcBleDeviceAddressSize_c);
        /* Send request. Group GAP,command WritePublicDeviceAddress */
        APP_FscitransmitPayload(gFsciBleGapOpcodeGroup_c, gBleCtrlWritePublicDeviceAddressOpCode_c, (void *)pClientPacket, fsciDataSize);
    }
}

/*! *********************************************************************************
*\fn            void APP_FscitransmitPayload(uint8_t OG,
*                                                   uint8_t OC,
*                                                   const uint8_t *pMsg,
*                                                   uint16_t msgLen)
*\brief         Send FSCI commands to NCP over RPMSG.
*
* \param[in]    OG operation Group
* \param[in]    OC operation Code
* \param[in]    pMsg pointer to payload
* \param[in]    msgLen length of the payload
* \param[in]    fsciInterface the interface on which the packet should be sent
*
*\retval        void
********************************************************************************** */
void APP_FscitransmitPayload(uint8_t OG, uint8_t OC, const uint8_t *pMsg, uint16_t msgLen)
{
    uint8_t          *buffer_ptr = NULL;
    uint16_t          buffer_size, index;
    uint8_t           checksum;
    clientPacketHdr_t header;
    
    /* Compute size */
    buffer_size = sizeof(clientPacketHdr_t) + msgLen + gFsci_TailBytes_c;
    
    /* Allocate buffer */
    buffer_ptr = MEM_BufferAlloc(buffer_size);
    if (NULL != buffer_ptr)
    {
        /* Message header */
        header.startMarker = 0x02U;
        header.opGroup     = OG;
        header.opCode      = OC;
        header.len         = msgLen;
        
        /* Compute CRC for TX packet, on opcode group, opcode, payload length, and payload fields */
        checksum = FSCI_computeChecksum((uint8_t *)&header + 1, sizeof(header) - 1u);
        checksum ^= FSCI_computeChecksum(pMsg, msgLen);
        
        index = 0;
        FLib_MemCpy(&buffer_ptr[index], &header, sizeof(header));
        index += sizeof(header);
        FLib_MemCpy(&buffer_ptr[index], pMsg, msgLen);
        index += msgLen;
        /* Store the Checksum */
        buffer_ptr[index++] = checksum;

        /* send message to Serial Manager */
        (void)PLATFORM_SendHciMessage(buffer_ptr, index);
    }
}
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
 * \brief        Configures BLE Stack after initialization
 *
 ********************************************************************************** */

#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)
/*! *********************************************************************************
 * \brief        Changes low power constraints.
 *
 ********************************************************************************** */
static void BleApp_ChangeLowPowerModeConstraints(uint8_t lpMode)
{
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)
    static uint8_t currentLPMode = (uint8_t)PWR_DeepSleep;
    if (currentLPMode != lpMode)
    {
        (void)PWR_ReleaseLowPowerModeConstraint((PWR_LowpowerMode_t)currentLPMode);
        (void)PWR_SetLowPowerModeConstraint((PWR_LowpowerMode_t)lpMode);
        currentLPMode = lpMode;
    }
#else
    (void)lpMode;
#endif
}
#endif

/*! *********************************************************************************
*\fn            static void BleApp_HandleReadPublicAddress(void *pParam)
*\brief         Handles Read Public Address from Core 1 application.
*
* \param[in]    pParam  Not used.
*
*\retval        void
********************************************************************************** */
static void BleApp_HandleReadPublicAddress(void *pParam)
{
    (void)pParam;
    App_SetBDAddr();
}

/*! *********************************************************************************
* @}
********************************************************************************** */
