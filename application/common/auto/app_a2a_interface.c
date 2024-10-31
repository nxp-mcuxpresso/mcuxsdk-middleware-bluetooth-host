/*! *********************************************************************************
 * \addtogroup AUTO
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2023-2024 NXP
*
*
* \file app_a2a_interface.c
*
* This is a source file for the Anchor to Anchor serial interface.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#if defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1)
#include "EmbeddedTypes.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_messaging.h"
#include "app_a2a_interface.h"
#include "app_conn.h"

/*************************************************************************************
**************************************************************************************
* Private macros
**************************************************************************************
*************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static uint16_t mReceivedBytes = 0;
static clientPacketHdr_t gPacketHeader;
static clientPacketStructured_t *gpRxPacket; /* Used for receiving */
/* Anchor2Anchor serial manager read handle buffer */
static SERIAL_MANAGER_READ_HANDLE_DEFINE(gA2ASerialReadHandle);
/* Anchor2Anchor serial manager write handle buffer */
static SERIAL_MANAGER_WRITE_HANDLE_DEFINE(gA2ASerialWriteHandle);
static appA2ADataIndicationCb_t mfpAppDataIndCb = NULL;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn           static packetStatus_t A2A_CheckPacket(clientPacketStructured_t *pData,
*                                                    uint16_t bytes)
*\brief        Check received data for valid packet.
*
*\param  [in]  pData    Pointer to received data.
*\param  [in]  bytes    Number of bytes received.
*
*\retval       packetStatus_t   Status of current packet.
********************************************************************************** */
static packetStatus_t A2A_CheckPacket
(
    clientPacketStructured_t *pData,
    uint16_t bytes
);

/*! *********************************************************************************
*\fn           static uint8_t A2A_ComputeChecksum(void *pBuffer,
*                                                 uint16_t size)
*\brief        Compute checksum of a received packet.
*
*\param  [in]  pBuffer      Pointer to packet data.
*\param  [in]  size         Received packet length.
*
*\retval       uint8_t      checksum.
********************************************************************************** */
static uint8_t A2A_ComputeChecksum
(
    void *pBuffer,
    uint16_t size
);

/*! *********************************************************************************
*\fn           static void A2A_RxCallback(void *callbackParam,
*                                         serial_manager_callback_message_t *message,
*                                         serial_manager_status_t status)
*\brief        Serial interface RX callback.
*
*\param  [in]  callbackParam    Callback parameter.
*\param  [in]  message          Callback message structure.
*\param  [in]  status           Serial manager status.
*
*\retval       void.
********************************************************************************** */
static void A2A_RxCallback
(
    void *callbackParam,
    serial_manager_callback_message_t *message,
    serial_manager_status_t status
);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn           bleResult_t A2A_Init(serial_handle_t pSerialHandle,
*                                   appA2ADataIndicationCb_t pfDataIndCb)
*\brief        Performs initialization of the Anchor to Anchor serial communication.
*
*\param  [in]  pSerialHandle    Serial interface handle to be used for communication.
*\param  [in]  pfDataIndCb      Application callback for received packets.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************** */
bleResult_t A2A_Init
(
    serial_handle_t pSerialHandle,
    appA2ADataIndicationCb_t pfDataIndCb
)
{
    bleResult_t status = gBleUnexpectedError_c;
    
    if ((NULL == pSerialHandle) || (NULL == pfDataIndCb))
    {
        status = gBleInvalidParameter_c;
    }
    else
    {
        serial_manager_status_t serialMngStatus = kStatus_SerialManager_Success;
        /* Open read handle */
        serialMngStatus = SerialManager_OpenReadHandle(pSerialHandle, (serial_read_handle_t)gA2ASerialReadHandle);
        
        if (kStatus_SerialManager_Success == serialMngStatus)
        {
            serialMngStatus = SerialManager_InstallRxCallback((serial_read_handle_t)gA2ASerialReadHandle,
                                                              A2A_RxCallback,
                                                              NULL);
        }
        if (kStatus_SerialManager_Success == serialMngStatus)
        {
            /* Open write handle */
            serialMngStatus = SerialManager_OpenWriteHandle(pSerialHandle,
                                                            (serial_write_handle_t)gA2ASerialWriteHandle);
        }
        if (kStatus_SerialManager_Success == serialMngStatus)
        {
            mfpAppDataIndCb = pfDataIndCb;
            status = gBleSuccess_c;
        }
    }
    
    return status;
}

/*! *********************************************************************************
*\fn           void A2A_SendCommand(uint8_t opGroup, uint8_t opCode, uint8_t *pPayload, uint16_t len);
*\brief        Send packet over the Anchor to Anchor interface.
*
*\param  [in]  opGroup      Group identifier.
*\param  [in]  opCode       Command identifier.
*\param  [in]  pPayload     Pointer to command data.
*\param  [in]  len          Command length.
*
*\retval       void.
********************************************************************************** */
void A2A_SendCommand
(
    uint8_t opGroup,
    uint8_t opCode,
    uint8_t *pPayload,
    uint16_t len
)
{
    clientPacket_t *pPacket = NULL;
    
    pPacket = MEM_BufferAlloc(sizeof(clientPacketStructured_t) + len);
    
    if (pPacket != NULL)
    {
        uint32_t  size;
        uint8_t   checksum;
        pPacket->structured.header.opCode = opCode;
        pPacket->structured.header.opGroup = opGroup;
        pPacket->structured.header.startMarker = gA2ACommandStartMarker_c;
        pPacket->structured.header.len = len;
        FLib_MemCpy(pPacket->structured.payload, pPayload, len);

        /* Compute Checksum */
        size = sizeof(clientPacketStructured_t) + pPacket->structured.header.len/* CRC included */;
        checksum = A2A_ComputeChecksum( &pPacket->raw[1], (uint16_t)size - 2U);
        pPacket->structured.payload[pPacket->structured.header.len] = checksum;
        (void)SerialManager_WriteBlocking(gA2ASerialWriteHandle, pPacket->raw, size);
        (void)MEM_BufferFree(pPacket);
    }
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn           static uint8_t A2A_ComputeChecksum(uint8_t *pBuffer, uint16_t size)
*\brief        Compute checksum of a received packet.
*
*\param  [in]  pBuffer      Pointer to packet data.
*\param  [in]  size         Received packet length.
*
*\retval       uint8_t      checksum.
********************************************************************************** */
static uint8_t A2A_ComputeChecksum
(
    void *pBuffer,
    uint16_t size
)
{
    uint16_t index;
    uint8_t  checksum = 0;

    for ( index = 0; index < size; index++ )
    {
        checksum ^= ((uint8_t *)pBuffer)[index];
    }

    return checksum;
}

/*! *********************************************************************************
*\fn           static packetStatus_t A2A_CheckPacket(clientPacket_t *pData,
*                                                    uint16_t bytes)
*\brief        Check received data for valid packet.
*
*\param  [in]  pData    Pointer to received data.
*\param  [in]  bytes    Number of bytes received.
*
*\retval       packetStatus_t   Status of current packet.
********************************************************************************** */
static packetStatus_t A2A_CheckPacket
(
    clientPacketStructured_t *pData,
    uint16_t bytes
)
{
    uint8_t checksum = 0;
    uint16_t len;
    packetStatus_t status = FRAMING_ERROR;

    if ( (uint32_t)bytes < sizeof(clientPacketHdr_t) )
    {
        status = PACKET_TOO_SHORT;
    }
    else
    {
        len = pData->header.len;
        
        if ( (uint32_t)bytes < (uint32_t)len + sizeof(clientPacketHdr_t) + sizeof(checksum) )
        {
            status = PACKET_TOO_SHORT;
        }
        /* If the length looks right, make sure that the checksum is correct. */
        else if( (uint32_t)bytes == (uint32_t)len + sizeof(clientPacketHdr_t) + sizeof(checksum) )
        {
            checksum = A2A_ComputeChecksum(&pData->header.opGroup, len + (uint16_t)sizeof(clientPacketHdr_t) - 1U);
            if( checksum == pData->payload[len] )
            {
                status = PACKET_IS_VALID;
            }
        }
        else
        {
            status = FRAMING_ERROR;
        }
    }

    return status;
}

/*! *********************************************************************************
*\fn           static void A2A_RxCallback(void *callbackParam,
*                                         serial_manager_callback_message_t *message,
*                                         serial_manager_status_t status)
*\brief        Serial interface RX callback.
*
*\param  [in]  callbackParam    Callback parameter.
*\param  [in]  message          Callback message structure.
*\param  [in]  status           Serial manager status.
*
*\retval       void.
********************************************************************************** */
static void A2A_RxCallback
(
    void *callbackParam,
    serial_manager_callback_message_t *message,
    serial_manager_status_t status
)
{
    packetStatus_t pktStatus;
    uint8_t c; uint32_t readBytes;

    if(kStatus_SerialManager_Success == SerialManager_TryRead((serial_read_handle_t)gA2ASerialReadHandle,
                                                              &c, 1, &readBytes))
    {
        while (readBytes != 0U)
        {
            /* Wait for start marker */
            switch(mReceivedBytes)
            {
                case 0U:
                {
                    if( c == (uint8_t)gA2ACommandStartMarker_c )
                    {
                        gPacketHeader.startMarker = c;
                        mReceivedBytes++;
                    }
                }
                break;
                
                case 1U:
                {
                    gPacketHeader.opGroup = c;
                    mReceivedBytes++;
                }
                break;
                
                case 2U:
                {
                    gPacketHeader.opCode = c;
                    mReceivedBytes++;
                }
                break;
                
                case 3U:
                {
                    gPacketHeader.len = (uint16_t)c;
                    mReceivedBytes++;
                }
                break;
                
                case 4U:
                {
                    gPacketHeader.len = gPacketHeader.len | (uint16_t)c << 8;
                    mReceivedBytes++;
                    
                    /* Allocate memory for serial packet containing header, data and CRC(1 octet) */
                    gpRxPacket = MEM_BufferAlloc(sizeof(clientPacketHdr_t) + gPacketHeader.len + sizeof(uint8_t));
                    
                    if (gpRxPacket != NULL)
                    {
                        FLib_MemCpy(gpRxPacket, &gPacketHeader, sizeof(gPacketHeader));
                    }
                    else
                    {
                        mReceivedBytes = 0U;
                    }
                }
                break;
                
                default:
                {
                    gpRxPacket->payload[mReceivedBytes - sizeof(clientPacketHdr_t)] = c;
                    mReceivedBytes++;
                    
                    pktStatus = A2A_CheckPacket(gpRxPacket, mReceivedBytes);
                    
                    if(pktStatus == PACKET_IS_VALID)
                    {
                        if (gBleSuccess_c != App_PostCallbackMessage(mfpAppDataIndCb, gpRxPacket))
                        {
                            MEM_BufferFree(gpRxPacket);
                        }
                        mReceivedBytes = 0;
                        break;
                    }
                    if(pktStatus == FRAMING_ERROR)
                    {
                        MEM_BufferFree(gpRxPacket);
                        mReceivedBytes = 0;
                        break;
                    }
                }
                break;
            }
            
            if (kStatus_SerialManager_Success != SerialManager_TryRead((serial_read_handle_t)gA2ASerialReadHandle,
                                                                       &c, 1, &readBytes))
            {
                break;
            }
        }
    }
}
#endif /* defined(gA2ASerialInterface_d) && (gA2ASerialInterface_d == 1) */