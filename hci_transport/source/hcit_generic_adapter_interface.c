/*
 * Copyright 2020-2023 NXP
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "fsl_component_mem_manager.h"
#include "fwk_platform_ble.h"
#include "fwk_debug.h"
#include "ble_general.h"
#include "hci_transport.h"
#include "fsl_os_abstraction.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define HCI_BUFFER_SIZE  270U   /*260 bytes is not enough. Need at least one more byte + some margin.*/

#if USE_RTOS
#define gHcitUseMutex_c 1
#else
#define gHcitUseMutex_c 0
#endif /*USE_RTOS*/

#if gHcitUseMutex_c
#define HCI_MUTEX_LOCK()   (void)OSA_MutexLock((osa_mutex_handle_t)mHCIMutexId, osaWaitForever_c)
#define HCI_MUTEX_UNLOCK() (void)OSA_MutexUnlock((osa_mutex_handle_t)mHCIMutexId)
#else
#define HCI_MUTEX_LOCK()
#define HCI_MUTEX_UNLOCK()
#endif /* gHcitUseMutex_c */

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
static uint8_t*                 pHciWriteBuffer     = NULL;
static uint8_t*                 pHciReadBuffer      = NULL;

static bool_t                   mHcitInit           = FALSE;
static hciTransportInterface_t  mTransportInterface = NULL;
#if defined(gAppEnableHybridGenfsk_d) && (gAppEnableHybridGenfsk_d == 1)
static hciToGenfskInterface_t   mGenfskInterface    = NULL;
#endif /* defined(gAppEnableHybridGenfsk_d) && (gAppEnableHybridGenfsk_d == 1) */

#if gHcitUseMutex_c
static OSA_MUTEX_HANDLE_DEFINE(mHCIMutexId);
#endif /* gHcitUseMutex_c */

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
static void Hcit_RxCallBack(uint8_t packetType,uint8_t *data, uint16_t len);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \fn             bleResult_t Hcit_Init(hciTransportInterface_t hcitConfigStruct)
* \brief          Reconfigures the HCI Transport module.
*
* \param  [in]    hcitConfigStruct      HCI Transport interface function.
*
* \retval         gHciSuccess_c         Initialization was successful.
* \retval         gHciAlreadyInit_c     The module has already been initialized.
* \retval         gHciTransportError_c  Initialization was unsuccessful.
* \retval         gBleOsError_c         Initialization of OS object was unsucessfull.
* \retval         gBleOutOfMemory_c     Memory allocation failure.
********************************************************************************** */
bleResult_t Hcit_Init(hciTransportInterface_t hcitConfigStruct)
{
    bleResult_t result = gHciSuccess_c;

    if( mHcitInit == FALSE )
    {
        do
        {
#if gHcitUseMutex_c
            if (KOSA_StatusSuccess != OSA_MutexCreate((osa_mutex_handle_t)mHCIMutexId))
            {
                result = gBleOsError_c;
                break;
            }
#endif
            HCI_MUTEX_LOCK();

            /* Allocate dynamically so we can release the memory at deinit */
            pHciWriteBuffer = MEM_BufferAlloc(HCI_BUFFER_SIZE);

            if(pHciWriteBuffer == NULL)
            {
                result = gBleOutOfMemory_c;
            }
            else
            {
                pHciReadBuffer  = MEM_BufferAlloc(HCI_BUFFER_SIZE);

                if(pHciReadBuffer == NULL)
                {
                    result = gBleOutOfMemory_c;
                    (void)MEM_BufferFree(pHciWriteBuffer);
                    pHciWriteBuffer = NULL;
                }
            }
            HCI_MUTEX_UNLOCK();

        } while(false);


        if(result == gHciSuccess_c)
        {
            /* Initialize HCI Transport interface */
            mTransportInterface = hcitConfigStruct;

            /* Register RX callback to PLATFORM layer */
            PLATFORM_SetHciRxCallback(Hcit_RxCallBack);

            /* Flag initialization on module */
            mHcitInit = TRUE;
        }
    }
    else
    {
        /* Module has already been initialized */
        result = gHciAlreadyInit_c;
    }

    return result;
}

/*! *********************************************************************************
* \fn             bleResult_t Hcit_Deinit(void)
* \brief          Terminates HCI Transport module and release allocated memory
*
* \retval         gHciSuccess_c         Termination was successful.
* \retval         gBleOsError_c         Termination of OS object was unsucessfull.
* \retval         gBleOutOfMemory_c     Memory disallocation failure.
********************************************************************************** */
bleResult_t Hcit_Deinit(void)
{
    bleResult_t result = gHciSuccess_c;
    do
    {
        if( mHcitInit == TRUE )
        {
            assert(pHciWriteBuffer != NULL);
            assert(pHciReadBuffer  != NULL);

            /* Reset transport interface */
            mTransportInterface = NULL;

            /* Reset RX callback */
            PLATFORM_SetHciRxCallback(NULL);

            HCI_MUTEX_LOCK();
            /* Release allocated memory */
            if ((MEM_BufferFree(pHciWriteBuffer)) != kStatus_MemSuccess)
            {
                result = gBleOutOfMemory_c;
                HCI_MUTEX_UNLOCK();
                break;
            }
            if ((MEM_BufferFree(pHciReadBuffer)) != kStatus_MemSuccess)
            {
                result = gBleOutOfMemory_c;
                HCI_MUTEX_UNLOCK();
                break;
            }
            pHciWriteBuffer = NULL;
            pHciReadBuffer  = NULL;
            HCI_MUTEX_UNLOCK();

#if gHcitUseMutex_c
            if (KOSA_StatusSuccess != OSA_MutexDestroy((osa_mutex_handle_t)mHCIMutexId))
            {
                result = gBleOsError_c;
                break;
            }
#endif
            /* Reset init status */
            mHcitInit = FALSE;
        }
    } while(false);

    return result;
}

/*! *********************************************************************************
* \fn             bleResult_t Hcit_Reconfigure(
*                                   hciTransportInterface_t hcitConfigStruct)
* \brief          Initializes the HCI Transport module.
*
* \param  [in]    hcitConfigStruct      HCI Transport interface function.
*
* \retval         gHciSuccess_c         Reconfiguration was successful.
* \retval         gHciTransportError_c  Reconfiguratoion was unsuccessful.
********************************************************************************** */
bleResult_t Hcit_Reconfigure(hciTransportInterface_t hcitConfigStruct)
{
    bleResult_t result = gHciSuccess_c;

    /* Terminate HCI transport module if already initialized */
    (void)Hcit_Deinit();

    /* Initialized HCI transport module with the new interface */
    result = Hcit_Init(hcitConfigStruct);

    return result;
}

/*! *********************************************************************************
* \fn             bleResult_t Hcit_SendPacket(
*                           hciPacketType_t packetType,
*                           void*           pPacket,
*                           uint16_t        packetSize)
* \brief          Sends a packet to controller.
*
* \param  [in]    packetType             HCI packet type.
* \param  [in]    pPacket                Pointer to the packet payload.
* \param  [in]    packetSize             Packet payload size.
*
* \retval         gBleSuccess_c          Packet is successfully sent.
* \retval         gBleOutOfMemory_c      Memory allocation for the packet fails.
* \retval         gHciTransportError_c   Packet is unsuccessfully sent.
********************************************************************************** */
bleResult_t Hcit_SendPacket
(
    hciPacketType_t packetType,
    void*           pPacket,
    uint16_t        packetSize
)
{
    static uint32_t totalLen   = 0U;
    static uint32_t curLen     = 0U;
    static bool aclDataPkt     = false;
    uint8_t* buf               = (uint8_t*)pPacket;
    bleResult_t result         = gBleSuccess_c;

    HCI_MUTEX_LOCK();
    assert(mHcitInit == TRUE);
    assert(pHciWriteBuffer != NULL);
    do
    {
        if(packetType == gHciDataPacket_c)
        {
            aclDataPkt = true;
            totalLen = (((((uint32_t)buf[3U]) << 8U) | ((uint32_t)buf[2U])) + 5U);
        }
        else
        {
            totalLen = (uint32_t)packetSize + 1U;
        }

        if(aclDataPkt == true)
        {
            if(curLen == 0U)
            {
                curLen = 1U;
                pHciWriteBuffer[0] = (uint8_t)packetType;
            }

            FLib_MemCpy(pHciWriteBuffer + curLen, buf, packetSize);
            curLen += packetSize;

            if(curLen != totalLen)
            {
                /* wait to get the full data packet before sending it */
                result = gBleSuccess_c;
                break;
            }
        }
        else
        {
            pHciWriteBuffer[0U] = (uint8_t)packetType;
            FLib_MemCpy(pHciWriteBuffer + 1U, buf, packetSize);
            totalLen = (uint32_t)packetSize + 1U;
        }

        if (PLATFORM_SendHciMessage(pHciWriteBuffer, totalLen) != 0)
        {
            result = gHciTransportError_c;
        }

        /* Reset static variables */
        curLen = 0U;
        totalLen = 0U;
        aclDataPkt = false;

    } while(false);

    HCI_MUTEX_UNLOCK();

    return result;
}

#if defined(gAppEnableHybridGenfsk_d) && (gAppEnableHybridGenfsk_d == 1)
/*! *********************************************************************************
* \fn             bleResult_t Hcit_RegisterGfskEventCallback(hciToGenfskInterface_t pfGfskEventCallback)
* \brief          Reconfigures the Genfsk Transport module.
*
* \param  [in]    hciToGenfskInterface_t                Genfsk Transport interface function.
*
* \retval         gHciSuccess_c                         Initialization was successful.
* \retval         gHciCallbackAlreadyInstalled_c        Callback has already been installed.
********************************************************************************** */
bleResult_t Hcit_RegisterGfskEventCallback(hciToGenfskInterface_t pfGfskEventCallback)
{
    bleResult_t result = gHciSuccess_c;

    if( mGenfskInterface == NULL )
    {
        /* Callback locus for this event is empty */
        mGenfskInterface = pfGfskEventCallback;
    }
    else
    {
        /* Callback for this event was already registered */
        result = gHciCallbackAlreadyInstalled_c;
    }

    return result;
}
#endif /* defined(gAppEnableHybridGenfsk_d) && (gAppEnableHybridGenfsk_d == 1) */

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \private
* \fn             void Hcit_RxCallBack(uint8_t packetType, uint8_t *data, uint16_t len)
* \brief          HCI Transport Rx Callback.
*
* \param  [in]    packetType                    packetType
* \param  [in]    data                          Pointer to the received packet.
* \param  [in]    len                           Received data length.
*
********************************************************************************** */
static void Hcit_RxCallBack(uint8_t packetType, uint8_t *data, uint16_t len)
{
    assert(mHcitInit == TRUE);
    assert(pHciReadBuffer != NULL);

    FLib_MemCpy(pHciReadBuffer, data, len);

#if defined(gAppEnableHybridGenfsk_d) && (gAppEnableHybridGenfsk_d == 1)
    bool_t found = FALSE;

    if(mGenfskInterface != NULL)
    {
        found = mGenfskInterface((hciPacketType_t)packetType, &pHciReadBuffer[0], (uint16_t)len);
    }
    if(FALSE == found)
    {
#endif /* defined(gAppEnableHybridGenfsk_d) && (gAppEnableHybridGenfsk_d == 1) */
        if(mTransportInterface != NULL)
        {
            (void)mTransportInterface((hciPacketType_t)packetType, &pHciReadBuffer[0], (uint16_t)len);
        }
#if defined(gAppEnableHybridGenfsk_d) && (gAppEnableHybridGenfsk_d == 1)
    }
#endif /* defined(gAppEnableHybridGenfsk_d) && (gAppEnableHybridGenfsk_d == 1) */
}

/*! *********************************************************************************
* @}
********************************************************************************** */