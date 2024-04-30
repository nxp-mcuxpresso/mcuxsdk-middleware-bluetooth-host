/*
 * Copyright 2020-2023 NXP
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Note for this file:  this file is currently aligned with hcit_generic_adapter_interface.c
    However, it can be simplified to forward straightlty the HCI messages to the Link layer through
    the platform without the need for HCI  header detection because the LL and host stack runs on same core */

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

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define HCI_BUFFER_SIZE  270U   /*260 bytes is not enough. Need at least one more byte + some margin.*/

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
********************************************************************************** */
bleResult_t Hcit_Init(hciTransportInterface_t hcitConfigStruct)
{
    bleResult_t result = gHciSuccess_c;

    if( mHcitInit == FALSE )
    {
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
********************************************************************************** */
bleResult_t Hcit_Deinit(void)
{
    if( mHcitInit == TRUE )
    {
        assert(pHciWriteBuffer != NULL);
        assert(pHciReadBuffer  != NULL);

        /* Reset transport interface */
        mTransportInterface = NULL;

        /* Reset RX callback */
        PLATFORM_SetHciRxCallback(NULL);

        /* Release allocated memory */
        (void)MEM_BufferFree(pHciWriteBuffer);
        (void)MEM_BufferFree(pHciReadBuffer);
        pHciWriteBuffer = NULL;
        pHciReadBuffer  = NULL;

        /* Reset init status */
        mHcitInit = FALSE;
    }

    return gHciSuccess_c;
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

    assert(mHcitInit == TRUE);
    assert(pHciWriteBuffer != NULL);

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
            return gBleSuccess_c;
        }
    }
    else
    {
        pHciWriteBuffer[0U] = (uint8_t)packetType;
        FLib_MemCpy(pHciWriteBuffer + 1U, buf, packetSize);
        totalLen = (uint32_t)packetSize + 1U;
    }

    PLATFORM_SendHciMessage(pHciWriteBuffer, totalLen);

    /* Reset static variables */
    curLen = 0U;
    totalLen = 0U;
    aclDataPkt = false;

    return result;
}

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

    if(mTransportInterface != NULL)
    {
        (void)mTransportInterface((hciPacketType_t)packetType, &pHciReadBuffer[0], (uint16_t)len);
    }
}

/*! *********************************************************************************
* @}
********************************************************************************** */