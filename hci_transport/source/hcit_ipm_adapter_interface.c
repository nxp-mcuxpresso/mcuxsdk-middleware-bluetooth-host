/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "fwk_platform_ble.h"
#include "fwk_debug.h"
#include "ble_general.h"
#include "hci_transport.h"

/************************************************************************************
*************************************************************************************
* Private macros
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
* \retval         gBleOsError_c         Initialization of OS object was unsucessfull.
* \retval         gBleOutOfMemory_c     Memory allocation failure.
********************************************************************************** */
bleResult_t Hcit_Init(hciTransportInterface_t hcitConfigStruct)
{
    bleResult_t result = gHciSuccess_c;

    if( mHcitInit == FALSE )
    {
        /* Initialize HCI Transport interface */
        mTransportInterface = hcitConfigStruct;

        /* Register RX callback to PLATFORM layer */
        PLATFORM_SetHciRxCallback(Hcit_RxCallBack);

        /* Flag initialization on module */
        mHcitInit = TRUE;
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
            /* Reset transport interface */
            mTransportInterface = NULL;

            /* Reset RX callback */
            PLATFORM_SetHciRxCallback(NULL);

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
    bleResult_t result = gBleSuccess_c;

    if (PLATFORM_SendHciMessageAlt(packetType, pPacket, packetSize) != 0)
    {
        result = gHciTransportError_c;
    }

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
    if(mTransportInterface != NULL)
    {
        (void)mTransportInterface((hciPacketType_t)packetType, data, len);
    }
}

/*! *********************************************************************************
* @}
********************************************************************************** */