/*! *********************************************************************************
* Copyright 2022-2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
/* Framework / Drivers */
#include "EmbeddedTypes.h"
#include "RNG_Interface.h"
#include "FunctionLib.h"
#include "SecLib.h"
#include "fsl_component_button.h"
#include "fsl_component_timer_manager.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_messaging.h"
#include "fsl_component_panic.h"
#include "fsl_os_abstraction.h"
#include "fsl_device_registers.h"
#include "fwk_platform.h"
#include "fsl_shell.h"

#include "controller_api.h"
#include "FsciInterface.h"
#include "FsciCommunication.h"

/* BLE Host Stack */
#include "gatt_interface.h"
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gatt_database.h"
#include "gap_interface.h"
#include "gatt_db_app_interface.h"

/* Application */
#include "host_ble_conn_manager.h"
#include "host_ble_init.h"
#include "host_hsdk_interface.h"
#include "app.h"
#include "NVM_Interface.h"
#include "host_app_nvm.h"
#if defined(SDK_OS_FREE_RTOS)
#include "fwk_freertos_utils.h"
#endif /* defined(SDK_OS_FREE_RTOS) */

#if (defined gFsciOverRpmsgBridge_c) && (gFsciOverRpmsgBridge_c > 0U)
#include "fwk_platform_ble.h"
#endif
#if defined(SDK_OS_FREE_RTOS) && (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))
#include "PWR_Interface.h"
#endif /* defined(SDK_OS_FREE_RTOS) && (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)) */
#include "fsci_ble_types.h"
/*************************************************************************************
**************************************************************************************
* Private macros
*************************************************************************************/
/* Application Events */
#define gAppEvtMsgFromHSDK_c            (1U << 0U)
#define gAppEvtAppCallback_c            (1U << 1U)
#define gBleCtrlWritePublicDeviceAddressOpCode_c    0x48U
#define gBleGapCmdReadPublicDeviceAddressOpCode_c   0x25U
/* HSDK OSA event */
OSA_EVENT_HANDLE_DEFINE(mAppEvent);

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************/
/* Application input queues */
static messaging_t mHSDKAppInputQueue;
static messaging_t mAppCbInputQueue;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
#if !defined(gFsciOverRpmsgBridge_c) || (gFsciOverRpmsgBridge_c == 0)
static void BleApp_SerialInit(void);
#endif /* !defined(gFsciOverRpmsgBridge_c) || (gFsciOverRpmsgBridge_c == 0) */

void App_HandleObservedHSDKMessageInput(bleEvtContainer_t* pMsg);

static void HSDKCallback(bleEvtContainer_t *container);

#if (defined gFsciOverRpmsgBridge_c) && (gFsciOverRpmsgBridge_c > 0U)
static void App_BleEventHandler(void *pData);
static void HCI_AppControllerRxCallback(uint8_t packetType, uint8_t *data, uint16_t len);
#endif
#if defined(SDK_OS_FREE_RTOS)
static void BluetoothLEHost_ProcessIdleTask(void);
#endif /* defined(SDK_OS_FREE_RTOS) */
static void App_SetBDAddr(void);
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn           void BluetoothLEHost_AppInit(void)
*\brief        Initializes application specific functionality before the BLE stack init.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void BluetoothLEHost_AppInit(void)
{
#if (defined gFsciOverRpmsgBridge_c) && (gFsciOverRpmsgBridge_c > 0U)
    PLATFORM_SetHciRxCallback(HCI_AppControllerRxCallback);
#endif
#if !defined(gFsciOverRpmsgBridge_c) || (gFsciOverRpmsgBridge_c == 0)
    BleApp_SerialInit();
#endif /* !defined(gFsciOverRpmsgBridge_c) || (gFsciOverRpmsgBridge_c == 0) */
    /* Init HSDK with FSCI Interface Id */
    HsdkInit(0);

    (void)RegistergHSDKCallback(HSDKCallback);
    (void)RegistergAppCallbackHSDK(App_HandleHSDKMessageInput);

    (void)MEM_Init();

    /* Framework init */
#if defined(gRngSeedStorageAddr_d) || defined(gXcvrDacTrimValueSorageAddr_d)
    NV_Init();
#endif /* gRngSeedStorageAddr_d || gXcvrDacTrimValueSorageAddr_d */

    /* Cryptographic hardware initialization */
    SecLib_Init();

#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
    (void)PLATFORM_EnableBleSecureKeyManagement();
#endif
#if defined(gAppUseNvm_d) && (gAppUseNvm_d > 0)
    /* Initialize NV module */
    (void)NvModuleInit();
    (void)App_HostNvmInit();;
#endif /* gAppUseNvm_d */

    /* Create application event */
    (void)OSA_EventCreate(mAppEvent, TRUE);

    /* Prepare application input queue.*/
    MSG_QueueInit(&mHSDKAppInputQueue);

    /* Prepare callback input queue.*/
    MSG_QueueInit(&mAppCbInputQueue);

#if (gAppUseNvm_d && defined(gFsciIncluded_c) && (gFsciIncluded_c))
#if gNvmEnableFSCIMonitoring_c
    NV_SetFSCIMonitoringState(TRUE);
#endif /* gNvmEnableFSCIMonitoring_c */
#if gNvmEnableFSCIRequests_c
    NV_RegisterToFSCI();
#endif /* gNvmEnableFSCIRequests_c */
#endif /* gAppUseNvm_d && gFsciIncluded_c */
}

#if (defined gFsciOverRpmsgBridge_c) && (gFsciOverRpmsgBridge_c > 0U)
/*! *********************************************************************************
*\fn           void BluetoothLEHost_AppInitController(void)
*\brief        Initializes controller specific functionality
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void BluetoothLEHost_AppInitController(void)
{
    /*Has to be called after RNG_Init(), once seed is generated.*/
    (void)Controller_SetRandomSeed();

    /* configure tx power to use in NBU specfic to BLE */
    (void)Controller_SetTxPowerLevelDbm(mAdvertisingDefaultTxPower_c, gAdvTxChannel_c);
    (void)Controller_SetTxPowerLevelDbm(mConnectionDefaultTxPower_c, gConnTxChannel_c);
    (void)Controller_ConfigureInvalidPduHandling(gLlInvalidPduHandlingType_c);
}
#endif

/*! *********************************************************************************
*\fn           void BluetoothLEHost_HandleMessages(void)
*\brief        This function is responsible for consuming all events coming from the
*              Bluetooth LE stack.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void App_HandleMessages(void)
{
#ifdef SDK_OS_FREE_RTOS
    osa_event_flags_t event = 0U;
    (void)OSA_EventWait((osa_event_handle_t)mAppEvent,
                        osaEventFlagsAll_c,
                        FALSE,
                        gAppTaskWaitTimeout_ms_c ,
                        &event);

#endif /* SDK_OS_FREE_RTOS */

    /* Check for existing messages in queue */
    if (MSG_QueueGetHead(&mHSDKAppInputQueue) != NULL)
    {
        /* Pointer for storing the messages from host. */
        bleEvtContainer_t *pMsgIn = MSG_QueueRemoveHead(&mHSDKAppInputQueue);

        if (pMsgIn != NULL)
        {
            /* Process it */
            App_HandleObservedHSDKMessageInput(pMsgIn);

            /* Messages must always be freed. */
            (void)MSG_Free(pMsgIn);
        }
    }

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
    event = (MSG_QueueGetHead(&mHSDKAppInputQueue) != NULL) ? gAppEvtMsgFromHSDK_c : 0U;
    event |= (MSG_QueueGetHead(&mAppCbInputQueue) != NULL) ? gAppEvtAppCallback_c : 0U;

    if (event != 0U)
    {
    	(void)OSA_EventSet((osa_event_handle_t)mAppEvent, gAppEvtAppCallback_c);
    }
#endif /* SDK_OS_FREE_RTOS */
}

/*! *********************************************************************************
*\fn           void App_IsMessagePending(void)
*\brief        This function checks whether Messages are pending to be processed.
*
*\param  [in]  none.
*
*\retval       TRUE if pending messages.
********************************************************************************** */
bool App_IsMessagePending(void)
{
    bool ret = FALSE;
     /* Check for existing messages in queue */
    if ( (MSG_QueueGetHead(&mHSDKAppInputQueue) != NULL) || (MSG_QueueGetHead(&mAppCbInputQueue) != NULL) )
    {
        ret = TRUE;
    }
    return ret;
}
#if defined(SDK_OS_FREE_RTOS)
/*! *********************************************************************************
*\fn           void vApplicationIdleHook(void)
*\brief        Idle hook function which places the microcontroller into a power
*              saving mode.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void vApplicationIdleHook(void)
{
    /* call some background tasks required by connectivity */
#if ((gAppUseNvm_d) || \
    (defined gAppOtaASyncFlashTransactions_c && (gAppOtaASyncFlashTransactions_c > 0)))

    /* Use a specific tick compensation mechanism implemented in the Connectivity Framework for FreeRTOS idle hook
     * This function, in pair with FWK_PostIdleHookTickCompensation, will measure the time taken during this idle
     * hook and estimate the ticks missed by the kernel by comparing the TickCount and the time elapsed using SOC
     * timers. This is useful when performing operations that block the system for more than 1 tick. */
    FWK_PreIdleHookTickCompensation();

    OSA_DisableIRQGlobal();

    if (PLATFORM_CheckNextBleConnectivityActivity() == true)
    {
#endif
        BluetoothLEHost_ProcessIdleTask();
#if ((gAppUseNvm_d) || \
    (defined gAppOtaASyncFlashTransactions_c && (gAppOtaASyncFlashTransactions_c > 0)))
    }

    OSA_EnableIRQGlobal();

    FWK_PostIdleHookTickCompensation();
#endif
}
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)

/*! *********************************************************************************
*\private
*\fn           void BluetoothLEHost_IsConnectivityTaskToProcess(void)
*\brief        Returns if there is Connectivity background task to process.
*
*\param  [in]  none.
*
*\retval       TRUE     If there is a connectivity task to process
*\retval       FALSE    If there is no connectivity task to process
********************************************************************************** */
bool_t BluetoothLEHost_IsConnectivityTaskToProcess(void)
{
    bool_t isConnectivityTaskToProcess = FALSE;
    do
    {
#if defined(gAppUseNvm_d) && (gAppUseNvm_d > 0)
        if(NvIsPendingOperation())
        {
            isConnectivityTaskToProcess = TRUE;
            break;
        }
#endif /* gAppUseNvm_d */

#if defined (gAppOtaASyncFlashTransactions_c) && (gAppOtaASyncFlashTransactions_c > 0)
        if(OTA_IsTransactionPending())
        {
            isConnectivityTaskToProcess = TRUE;
            break;
        }
#endif
    }while(false);

    return isConnectivityTaskToProcess;
}

#define TICK_TO_US(tick) (uint64_t)((uint64_t)tick * (uint64_t)portTICK_PERIOD_MS * (uint64_t)1000)
#define US_TO_TICK(us)   (TickType_t)((uint64_t)us / ((uint64_t)portTICK_PERIOD_MS * (uint64_t)1000U))


/*! *********************************************************************************
*\private
*\fn           void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
*\brief        This function will try to put the MCU into a deep sleep mode for at
*              most the maximum OS idle time specified. Else the MCU will enter a
*              sleep mode until the first IRQ.
*
*\param  [in]  xExpectedIdleTime    The idle time in OS ticks.
*
*\retval       none.
*
*\remarks      This feature is available only for FreeRTOS.
********************************************************************************** */
void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    if (BluetoothLEHost_IsConnectivityTaskToProcess() == FALSE)
    {
        bool abortIdle = false;
        uint64_t actualIdleTimeUs = 0U, expectedIdleTimeUs = 0U;

        /* The OSA_InterruptDisable() API will prevent us to wakeup so we use
         * OSA_DisableIRQGlobal() */
        OSA_DisableIRQGlobal();

        /* Disable and prepare systicks for low power */
        abortIdle = PWR_SysticksPreProcess((uint32_t)xExpectedIdleTime, &expectedIdleTimeUs);

        if (abortIdle == false)
        {
                /* Enter low power with a maximal timeout */
                actualIdleTimeUs = PWR_EnterLowPower(expectedIdleTimeUs);

                /* Re enable systicks and compensate systick timebase */
                PWR_SysticksPostProcess(expectedIdleTimeUs, actualIdleTimeUs);
        }

        /* Exit from critical section */
        OSA_EnableIRQGlobal();
    }
}
#endif /* defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0) */
#endif /* defined(SDK_OS_FREE_RTOS) */
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
        (void)MEM_BufferFree(buffer_ptr);
    }
}
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
#if !defined(gFsciOverRpmsgBridge_c) || (gFsciOverRpmsgBridge_c == 0)
/*!*************************************************************************************************
*\fn    static void BleApp_SerialInit(void)
*\brief Function used to setup the serial interface
*
*\param[in]   none
*
*\return      none
***************************************************************************************************/
static void BleApp_SerialInit(void)
{
#if defined(gFsciIncluded_c) && (gFsciIncluded_c > 0)
    g_fsciHandleList[0] = gSerMgrIf2;

    /* Init FSCI */
    FSCI_commInit( g_fsciHandleList );
#endif
}
#endif /* !defined(gFsciOverRpmsgBridge_c) || (gFsciOverRpmsgBridge_c == 0) */
/*!*************************************************************************************************
*\fn    static void HSDKCallback(bleEvtContainer_t *container)
*\brief HSDK message handler function
*
*\param[in]   container    Pointer to event data
*
*\return      none
***************************************************************************************************/
static void HSDKCallback(bleEvtContainer_t *container)
{
    /* Put message in the HSDK Stack to App queue */
    (void)MSG_QueueAddTail(&mHSDKAppInputQueue, container);

    /* Signal application */
    (void)OSA_EventSet(mAppEvent, gAppEvtMsgFromHSDK_c);
}

/*! *********************************************************************************
 *\fn           void App_SetBDAddr(void)
 *\brief        This is used to set the Bluetooth LE Device Address on the NCP.
 *
 *\param  [in]  none.
 *
 *\retval       void.
 ********************************************************************************** */
static void App_SetBDAddr(void)
{
    /* FSCI payload size is 7 (1 octet reset field and 6 octets for the address) */
    uint32_t fsciDataSize = sizeof(uint8_t) + gcBleDeviceAddressSize_c;
    uint8_t *pClientPacket;
    uint8_t *pBuffer;
    uint8_t aBdAddr[gcBleDeviceAddressSize_c] = {0U};

    /* Get the public address from Framework. If the address is not available one
    will be generated */
    PLATFORM_GetBDAddr(aBdAddr);

    /* Send the address to Core 1 */
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
        (void)MEM_BufferFree(pClientPacket);
    }
}

#if (defined gFsciOverRpmsgBridge_c) && (gFsciOverRpmsgBridge_c > 0U)
/*!*************************************************************************************************
 \fn     uint8_t App_BleEventHandler(void *pData)
 \brief  This function is used to handle FSCI messages received over rpmsg

 \param  [in]   pData - pointer to data;
 ***************************************************************************************************/
static void App_BleEventHandler(void *pData)
{
    clientPacket_t *pSerialPacket = (clientPacket_t*)pData;
    
    if ((pSerialPacket->headerAndStatus.header.opGroup == gFsciBleGapOpcodeGroup_c) &&
        (pSerialPacket->headerAndStatus.header.opCode == gBleGapCmdReadPublicDeviceAddressOpCode_c))
    {
        /* Handle request for public address from NCP here since fsci/hsdk doesn't support this feature */
        (void)MEM_BufferFree(pSerialPacket);
        App_SetBDAddr();
    }
    else
    {
        FSCI_ProcessRxPkt(pSerialPacket, 0U);
    }
}

/*!*************************************************************************************************
*\fn    void HCI_AppControllerRxCallback( uint8_t packetType, uint8_t *data, uint16_t len)
*\brief RPMSG to FSCI bridge function: takes a message received over RPMSG and passes it to the
*       FSCI task for handling
*
*\param[in]   packetType    Packet type
*\param[in]   data          Pointer to the data (FSCI packet)
*\param[in]   len           FSCI packet size
*
*\return      none
***************************************************************************************************/
static void HCI_AppControllerRxCallback
(
    uint8_t packetType,
    uint8_t *data,
    uint16_t len
)
{
    clientPacket_t* pSerialPacket = NULL;

    /* Increase by 1 the length to insert the packet type */
    pSerialPacket = MEM_BufferAlloc((uint32_t)len + 1);
    if (pSerialPacket != NULL)
    {
        pSerialPacket->raw[0] = packetType; /* packetType is actually the first byte of the FSCI payload */
        FLib_MemCpy(&pSerialPacket->raw[1], (uint8_t*)data, len);
        App_PostCallbackMessage(App_BleEventHandler, pSerialPacket);

    }
}
#endif
#if defined(SDK_OS_FREE_RTOS)
/*! *********************************************************************************
*\private
*\fn           static void BluetoothLEHost_ProcessIdleTask(void)
*\brief        Handles Connectivity background task, usually executed from Idle task.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
static void BluetoothLEHost_ProcessIdleTask(void)
{
#if defined(gAppUseNvm_d) && (gAppUseNvm_d > 0)
    if(NvIdle() == 0)
#endif /* gAppUseNvm_d */
    {
    }
}
#endif /* defined(SDK_OS_FREE_RTOS) */
/*! *********************************************************************************
* @}
********************************************************************************** */
