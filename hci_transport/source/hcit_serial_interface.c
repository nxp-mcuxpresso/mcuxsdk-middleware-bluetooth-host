/*! *********************************************************************************
* \addtogroup HCI
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2014 Freescale Semiconductor, Inc.
* Copyright 2016-2020, 2023 NXP
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
#include "EmbeddedTypes.h"
#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
#include "fsl_component_mem_manager.h"
#include "fsl_component_serial_manager.h"
#include "board.h"
#include "app.h"
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
#include "MemManager.h"
#include "app.h"
#endif /*SDK_COMPONENT_INTEGRATION > 0*/

#include "ble_general.h"
#include "hci_transport.h"
#include "fsl_device_registers.h"

#if (defined(gUseHciTransportDownward_d) && (gUseHciTransportDownward_d)) || (defined(gUseHciTransportUpward_d) && gUseHciTransportUpward_d)
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
typedef PACKED_STRUCT hciCommandPacketHeader_tag
{
    uint16_t    opCode;
    uint8_t     parameterTotalLength;
}hciCommandPacketHeader_t;

typedef PACKED_STRUCT hciAclDataPacketHeader_tag
{
    uint16_t    handle      :12;
    uint16_t    pbFlag      :2;
    uint16_t    bcFlag      :2;
    uint16_t    dataTotalLength;
}hciAclDataPacketHeader_t;

typedef PACKED_STRUCT hciEventPacketHeader_tag
{
    hciEventCode_t  eventCode;
    uint8_t     dataTotalLength;
}hciEventPacketHeader_t;

typedef PACKED_STRUCT hcitPacketHdr_tag
{
    hciPacketType_t packetTypeMarker;
    PACKED_UNION
    {
        hciAclDataPacketHeader_t    aclDataPacket;
        hciEventPacketHeader_t      eventPacket;
        hciCommandPacketHeader_t    commandPacket;
    };
}hcitPacketHdr_t;

typedef PACKED_STRUCT hcitPacketStructured_tag
{
    hcitPacketHdr_t header;
    uint8_t         payload[gHcitMaxPayloadLen_c];
} hcitPacketStructured_t;

typedef PACKED_UNION hcitPacket_tag
{
    /* The entire packet as unformatted data. */
    uint8_t raw[sizeof(hcitPacketStructured_t)];
}hcitPacket_t;

typedef struct hcitComm_tag
{
    hcitPacket_t        *pPacket;
    hcitPacketHdr_t     pktHeader;
    uint16_t            bytesReceived;
    uint16_t            expectedLength;
}hcitComm_t;

typedef enum{
    mDetectMarker_c       = 0,
    mDetectHeader_c,
    mPacketInProgress_c
}detectState_t;

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static bool_t   mHcitInit = FALSE;

#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
#if defined(gSerialManagerMaxInterfaces_c) && (gSerialManagerMaxInterfaces_c)
/*hci serial read handle*/
static SERIAL_MANAGER_READ_HANDLE_DEFINE(s_hciReadHandle);
#endif
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
#endif /*SDK_COMPONENT_INTEGRATION > 0*/


#if defined(gSerialManagerMaxInterfaces_c) && (gSerialManagerMaxInterfaces_c)
static hcitComm_t               mHcitData;
static hciTransportInterface_t  mTransportInterface;
static detectState_t            mPacketDetectStep;
static hcitPacket_t             mHcitPacketRaw;
#endif  /* gSerialManagerMaxInterfaces_c */

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
#if defined(gSerialManagerMaxInterfaces_c) && (gSerialManagerMaxInterfaces_c)
#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
static void Hcit_RxCallBack(void *callbackParam,
                           serial_manager_callback_message_t *message,
                           serial_manager_status_t status);
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
static void Hcit_RxCallBack(void *pData);
#endif /*SDK_COMPONENT_INTEGRATION > 0*/
#endif /*defined(gSerialManagerMaxInterfaces_c) && (gSerialManagerMaxInterfaces_c)*/
static void Hcit_FreePacket(void *pPacket);

#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
static void Hcit_SerialFreePacket(void *pPacket,
                              serial_manager_callback_message_t *message,
                              serial_manager_status_t status);
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
#endif /*SDK_COMPONENT_INTEGRATION > 0*/
/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
#if gUseHciTransportDownward_d
#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
OSA_SEMAPHORE_HANDLE_DEFINE(gHciDataBufferingSem);
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
osaSemaphoreId_t gHciDataBufferingSem;
#endif /*SDK_COMPONENT_INTEGRATION > 0*/
#endif /* gUseHciTransportDownward_d */

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief WEAK function required to link the ARM MDK projects
*
* \param[in] packetType - ignored
* \param[in] pHciPacket - ignored
* \param[in] packetSize - ignored
*
* \param[out]
*
* \return gBleSuccess_c
*
* \pre
*
* \remarks
*
********************************************************************************** */
#if defined CPU_QN9080C
#if defined( __CC_ARM )
__weak bleResult_t Ble_HciRecv
(
    hciPacketType_t packetType,
    void *pHciPacket,
    uint16_t packetSize
)
{
    (void)packetType;
    (void)pHciPacket;
    (void)packetSize;

    return gBleSuccess_c;
}
#endif /* __CC_ARM */
#endif /* CPU_QN9080C */

/*! *********************************************************************************
* \brief
*
* \param[in]
*
* \param[out]
*
* \return
*
* \pre
*
* \remarks
*
********************************************************************************** */
bleResult_t Hcit_Init(hciTransportInterface_t hcitConfigStruct)
{
    bleResult_t result = gHciSuccess_c;

    if( mHcitInit == FALSE )
    {
#if gUseHciTransportDownward_d
#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
        osa_status_t status = OSA_SemaphoreCreate(gHciDataBufferingSem, 0);
        if (status != KOSA_StatusSuccess )
        {
            return gHciTransportError_c;
        }
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
        gHciDataBufferingSem = OSA_SemaphoreCreate(0);

        if (gHciDataBufferingSem == NULL)
        {
            return gHciTransportError_c;
        }
#endif /*SDK_COMPONENT_INTEGRATION > 0*/
#endif /* gUseHciTransportDownward_d */

#if defined(gSerialManagerMaxInterfaces_c) && (gSerialManagerMaxInterfaces_c)
        /* Initialize HCI Transport interface */
        mTransportInterface = hcitConfigStruct;

#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
        if (kStatus_SerialManager_Success != SerialManager_OpenReadHandle((serial_handle_t)g_HciSerialHandle, (serial_read_handle_t)s_hciReadHandle))
        {
            return gHciTransportError_c;
        }
        (void)SerialManager_InstallRxCallback((serial_read_handle_t)s_hciReadHandle, Hcit_RxCallBack, NULL);

#else  /*SDK_COMPONENT_INTEGRATION > 0*/
        /* Install Controller Events Callback handler */
        (void)Serial_SetRxCallBack(mHcitSerMgrIf, Hcit_RxCallBack, NULL);
#endif /*SDK_COMPONENT_INTEGRATION > 0*/
#endif  /* gSerialManagerMaxInterfaces_c */

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
* \brief
*
* \param[in]
*
* \param[out]
*
* \return
*
* \pre
*
* \remarks
*
********************************************************************************** */
bleResult_t Hcit_SendPacket
    (
        hciPacketType_t packetType,
        void*           pPacket,
        uint16_t        packetSize
    )
{
    uint8_t*        pSerialPacket = NULL;
    bleResult_t     result = gBleSuccess_c;
#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
    uint8_t* pPacketBuffer;
    uint8_t* hciWriteHandle;
    serial_manager_status_t  status = kStatus_SerialManager_Success;
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
    serialStatus_t  status = gSerial_Success_c;
#endif /*SDK_COMPONENT_INTEGRATION > 0*/

#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
    pPacketBuffer = MEM_BufferAlloc(SERIAL_MANAGER_WRITE_HANDLE_SIZE + 1UL + (uint32_t)packetSize);
    if (pPacketBuffer == NULL)
    {
        result = gBleOutOfMemory_c;
    }
    hciWriteHandle = pPacketBuffer;
    pSerialPacket = pPacketBuffer + SERIAL_MANAGER_WRITE_HANDLE_SIZE;
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
    pSerialPacket = MEM_BufferAlloc(1UL + (uint32_t)packetSize);
#endif /*SDK_COMPONENT_INTEGRATION > 0*/
    if ((result == gBleSuccess_c) && ( NULL != pSerialPacket ))
    {
        pSerialPacket[0] = (uint8_t)packetType;
        FLib_MemCpy(pSerialPacket+1, (uint8_t*)pPacket, packetSize);
#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
#if defined(gSerialManagerMaxInterfaces_c) && (gSerialManagerMaxInterfaces_c)
        if (kStatus_SerialManager_Success != SerialManager_OpenWriteHandle((serial_handle_t)g_HciSerialHandle, (serial_write_handle_t)hciWriteHandle))
        {
            Hcit_FreePacket(pPacketBuffer);
            return gHciTransportError_c;
        }
#endif
        (void)SerialManager_InstallTxCallback((serial_write_handle_t)hciWriteHandle, Hcit_SerialFreePacket, pPacketBuffer);
        status = SerialManager_WriteNonBlocking((serial_write_handle_t)hciWriteHandle, pSerialPacket, 1U + packetSize);

        if(kStatus_SerialManager_Success != status)
        {
            Hcit_FreePacket(pPacketBuffer);
            result = gHciTransportError_c;
        }
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
        status = Serial_AsyncWrite(mHcitSerMgrIf, pSerialPacket, 1U + packetSize, Hcit_FreePacket, pSerialPacket);
        (void)status;

        if( gSerial_Success_c != status )
        {
            Hcit_FreePacket(pSerialPacket);
            result = gHciTransportError_c;
        }
#endif /*SDK_COMPONENT_INTEGRATION > 0*/
    }
    else
    {
        result = gBleOutOfMemory_c;
    }

    return result;
}

/*! *********************************************************************************
* \brief
*
* \param[in]
*
* \param[out]
*
* \return
*
* \pre
*
* \remarks
*
********************************************************************************** */
bleResult_t Hcit_RecvPacket
    (
        void*           pPacket,
        uint16_t        packetSize
    )
{
    bleResult_t result = gHciSuccess_c;

    uint8_t* aData = (uint8_t*) pPacket;
    uint8_t type = aData[0];

    if (type != 0x01U && type != 0x02U && type != 0x04U)
    {
        result = /* Something more meaningful? */ gHciTransportError_c;
    }
    else
    {
        hciPacketType_t packetType = (hciPacketType_t) type;
        result = Ble_HciRecv
        (
            packetType,
            aData + 1,
            packetSize - 1U
        );

        Hcit_FreePacket(pPacket);  ///TODO: Verify here in FSCI
    }

    return result;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
#if defined(gSerialManagerMaxInterfaces_c) && (gSerialManagerMaxInterfaces_c)
static inline void Hcit_SendMessage(void)
{
    /* Send the message to HCI */
    (void)mTransportInterface(mHcitData.pktHeader.packetTypeMarker,
                              mHcitData.pPacket,
                              mHcitData.bytesReceived);

    mHcitData.pPacket = NULL;
    mPacketDetectStep = mDetectMarker_c;
}

#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
static void Hcit_RxCallBack(void *callbackParam,
                           serial_manager_callback_message_t *message,
                          serial_manager_status_t status)
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
static void Hcit_RxCallBack(void *pData)
#endif /*SDK_COMPONENT_INTEGRATION > 0*/
{
    uint8_t         recvChar;

#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
    uint32_t        count = 0U;
    if(kStatus_SerialManager_Success != SerialManager_TryRead( (serial_read_handle_t)s_hciReadHandle, &recvChar, 1, &count))
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
    uint16_t        count = 0U;
    serialStatus_t temp = Serial_GetByteFromRxBuffer( mHcitSerMgrIf, &recvChar, &count);
    if(temp != gSerial_Success_c)
#endif /*SDK_COMPONENT_INTEGRATION > 0*/
    {
        return;
    }

    while( count != 0U )
    {
        switch( mPacketDetectStep )
        {
            case mDetectMarker_c:
                if( (recvChar == (uint8_t)gHciDataPacket_c) ||
                    (recvChar == (uint8_t)gHciEventPacket_c) ||
                    (recvChar == (uint8_t)gHciCommandPacket_c) )
                {
                    union
                    {
                        hcitPacketHdr_t *pPacketHdr;
                        hcitPacket_t    *pPacket;
                    } packetTemp; /* MISRA rule 11.3 */

                    packetTemp.pPacketHdr = &mHcitData.pktHeader;
                    mHcitData.pPacket = packetTemp.pPacket;

                    mHcitData.pktHeader.packetTypeMarker = (hciPacketType_t)recvChar;
                    mHcitData.bytesReceived = 1;

                    mPacketDetectStep = mDetectHeader_c;
                }
                break;

            case mDetectHeader_c:
                mHcitData.pPacket->raw[mHcitData.bytesReceived++] = recvChar;

                switch( mHcitData.pktHeader.packetTypeMarker )
                {
                    case gHciDataPacket_c:
                        /* ACL Data Packet */
                        if( mHcitData.bytesReceived == (gHciAclDataPacketHeaderLength_c + 1U) )
                        {
                            /* Validate ACL Data packet length */
                            if( mHcitData.pktHeader.aclDataPacket.dataTotalLength > gHcLeAclDataPacketLengthDefault_c )
                            {
                                mHcitData.pPacket = NULL;
                                mPacketDetectStep = mDetectMarker_c;
                                break;
                            }
                            mHcitData.expectedLength = gHciAclDataPacketHeaderLength_c +
                                                       mHcitData.pktHeader.aclDataPacket.dataTotalLength;

                            mPacketDetectStep = mPacketInProgress_c;
                        }
                        break;

                    case gHciEventPacket_c:
                        /* HCI Event Packet */
                        if( mHcitData.bytesReceived == (gHciEventPacketHeaderLength_c + 1U) )
                        {
                            /* Validate HCI Event packet length
                            if( mHcitData.pktHeader.eventPacket.dataTotalLength > gHcEventPacketLengthDefault_c )
                            {
                                mHcitData.pPacket = NULL;
                                mPacketDetectStep = mDetectMarker_c;
                                break;
                            } */
                            mHcitData.expectedLength = gHciEventPacketHeaderLength_c +
                                                       (uint16_t)mHcitData.pktHeader.eventPacket.dataTotalLength;
                            mPacketDetectStep = mPacketInProgress_c;
                        }
                        break;

                    case gHciCommandPacket_c:
                        /* HCI Command Packet */
                        if( mHcitData.bytesReceived == (gHciCommandPacketHeaderLength_c + 1U) )
                        {

                            mHcitData.expectedLength = gHciCommandPacketHeaderLength_c +
                                                       (uint16_t)mHcitData.pktHeader.commandPacket.parameterTotalLength;
                            mPacketDetectStep = mPacketInProgress_c;
                        }
                        break;
                    case gHciSynchronousDataPacket_c:
                    default:
                        ; /* Not Supported */
                        break;
                }

                if( mPacketDetectStep == mPacketInProgress_c )
                {
                    mHcitData.pPacket = &mHcitPacketRaw;
                    FLib_MemCpy(mHcitData.pPacket, (uint8_t*)&mHcitData.pktHeader + 1, sizeof(hcitPacketHdr_t) - 1U);
                    mHcitData.bytesReceived -= 1U;

                    if( mHcitData.bytesReceived == mHcitData.expectedLength )
                    {
                        Hcit_SendMessage();
                    }
                }
                break;

            case mPacketInProgress_c:
                mHcitData.pPacket->raw[mHcitData.bytesReceived++] = recvChar;

                if( mHcitData.bytesReceived == mHcitData.expectedLength )
                {
                    Hcit_SendMessage();
                }
                break;

            default:
                ; /* No action required */
                break;
        }

#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
        if(kStatus_SerialManager_Success != SerialManager_TryRead( (serial_read_handle_t)s_hciReadHandle, &recvChar, 1, &count))
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
        if(gSerial_Success_c != Serial_GetByteFromRxBuffer( mHcitSerMgrIf, &recvChar, &count))
#endif /*SDK_COMPONENT_INTEGRATION > 0*/
        {
            return;
        }
    }
}
#endif  /* gSerialManagerMaxInterfaces_c */

static void Hcit_FreePacket
(
    void *pPacket
)
{
    (void)MEM_BufferFree(pPacket);
}

#if (defined(SDK_COMPONENT_INTEGRATION) && (SDK_COMPONENT_INTEGRATION > 0))
static void Hcit_SerialFreePacket(void *pPacket,
                              serial_manager_callback_message_t *message,
                              serial_manager_status_t status)
{
    SerialManager_CloseWriteHandle((serial_write_handle_t)pPacket);
    (void)MEM_BufferFree(pPacket);
}
#else  /*SDK_COMPONENT_INTEGRATION > 0*/
#endif /*SDK_COMPONENT_INTEGRATION > 0*/
#endif /*gUseHciTransportDownward_d || gUseHciTransportUpward_d*/
/*! *********************************************************************************
* @}
********************************************************************************** */
