/*! *********************************************************************************
* \addtogroup FSCI_BLE
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2019, 2021-2023, 2025-2026 NXP
*
*
* \file
*
* This is a source file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble_l2cap_cb.h"


#if gFsciIncluded_c && gFsciBleL2capCbLayerEnabled_d

/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

#define fsciBleL2capCbCallApiFunction(apiFunction)          bleResult_t result = (apiFunction); \
                                                            fsciBleL2capCbStatusMonitor(result)
#define fsciBleL2capCbMonitorOutParams(functionId, ...)     if(gBleSuccess_c == result)                  \
                                                            {                                            \
                                                                FsciEvtMonitor(functionId, __VA_ARGS__); \
                                                            }

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

typedef gFsciStatus_t(*pfFsciBleL2capCbCmdHandler_t)(clientPacket_t* pClientPacket);

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
static void fsciBleL2capCbLeCbDataCallback(deviceId_t deviceId, uint16_t lePsm, uint8_t* pPacket, uint16_t packetLength);
static void fsciBleL2capCbLeCbControlCallback(l2capControlMessage_t *pMessage);
static void fsciBleL2capCbLeCbCtrlEvtMonUpdateOpCodeAndDataSize(l2capControlMessage_t *pMessage, fsciBleL2capCbOpCode_t *pOpCode, uint32_t *pDataSize, bool_t *pEarlyReturn);

#if gFsciBleBBox_d || gFsciBleTest_d
static gFsciStatus_t fsciBleL2capCbCmdRegisterLePsmHandler(clientPacket_t* pClientPacket);
static gFsciStatus_t fsciBleL2capCbCmdDeregisterLePsmHandler(clientPacket_t* pClientPacket);
static gFsciStatus_t fsciBleL2capCbCmdConnectLePsmHandler(clientPacket_t* pClientPacket);
static gFsciStatus_t fsciBleL2capCbCmdDisconnectLeCbChannelHandler(clientPacket_t* pClientPacket);
static gFsciStatus_t fsciBleL2capCbCmdCancelConnectionHandler(clientPacket_t* pClientPacket);
static gFsciStatus_t fsciBleL2capCbCmdSendLeCbDataHandler(clientPacket_t* pClientPacket);
static gFsciStatus_t fsciBleL2capCbCmdSendLeCreditHandler(clientPacket_t* pClientPacket);
#if defined(gBLE52_d) && (gBLE52_d == 1)
static gFsciStatus_t fsciBleL2capCbCmdEnhancedConnectLePsmHandler(clientPacket_t* pClientPacket);
static gFsciStatus_t fsciBleL2capCbCmdEnhancedChannelReconfigureHandler(clientPacket_t* pClientPacket);
static gFsciStatus_t fsciBleL2capCbCmdEnhancedCancelConnectionHandler(clientPacket_t* pClientPacket);
#endif /* defined(gBLE52_d) && (gBLE52_d == 1) */
#endif /* gFsciBleBBox_d || gFsciBleTest_d */
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

#if gFsciBleTest_d
    /* Indicates if FSCI for L2CAP CB is enabled or not */
    static bool_t bFsciBleL2capCbEnabled          	= FALSE;

    /* Indicates if the command was initiated by FSCI (to be not monitored) */
    static bool_t bFsciBleL2capCbCmdInitiatedByFsci = FALSE;
#endif /* gFsciBleTest_d */


/* L2CAP Credit Based data callback initialized with FSCI empty static function */
static l2caLeCbDataCallback_t       l2capCbLeCbDataCallback     = fsciBleL2capCbLeCbDataCallback;

/* L2CAP Credit Based control callback initialized with FSCI empty static function */
static l2caLeCbControlCallback_t    l2capCbLeCbControlCallback  = fsciBleL2capCbLeCbControlCallback;


#if gFsciBleBBox_d || gFsciBleTest_d
/* Array of function pointers used by the fsciBleL2capCbHandler to process the requests in the comments below */
static const pfFsciBleL2capCbCmdHandler_t maFsciBleL2capCbCmdHandler[]=
{
    fsciBleL2capCbCmdRegisterLePsmHandler,                      /* gBleL2capCbCmdRegisterLePsmOpCode_c */
    fsciBleL2capCbCmdDeregisterLePsmHandler,                    /* gBleL2capCbCmdDeregisterLePsmOpCode_c */
    fsciBleL2capCbCmdConnectLePsmHandler,                       /* gBleL2capCbCmdConnectLePsmOpCode_c */
    fsciBleL2capCbCmdDisconnectLeCbChannelHandler,              /* gBleL2capCbCmdDisconnectLeCbChannelOpCode_c */
    fsciBleL2capCbCmdCancelConnectionHandler,                   /* gBleL2capCbCmdCancelConnectionOpCode_c */
    fsciBleL2capCbCmdSendLeCbDataHandler,                       /* gBleL2capCbCmdSendLeCbDataOpCode_c */
    fsciBleL2capCbCmdSendLeCreditHandler,                       /* gBleL2capCbCmdSendLeCreditOpCode_c */
#if defined(gBLE52_d) && (gBLE52_d == 1)
    fsciBleL2capCbCmdEnhancedConnectLePsmHandler,               /* gBleL2capCbCmdEnhancedConnectLePsmOpCode_c */
    fsciBleL2capCbCmdEnhancedChannelReconfigureHandler,         /* gBleL2capCbCmdEnhancedChannelReconfigureOpCode_c */
    fsciBleL2capCbCmdEnhancedCancelConnectionHandler,           /* gBleL2capCbCmdEnhancedCancelConnectionOpCode_c */
#endif /* defined(gBLE52_d) && (gBLE52_d == 1) */
};
#endif /* gFsciBleBBox_d || gFsciBleTest_d */
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

void fsciBleSetL2capCbLeCbDataCallback(l2caLeCbDataCallback_t dataCallback)
{
    /* Set callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    l2capCbLeCbDataCallback = (NULL != dataCallback) ?
                              dataCallback :
                              fsciBleL2capCbLeCbDataCallback;
}


void fsciBleSetL2capCbLeCbControlCallback(l2caLeCbControlCallback_t controlCallback)
{
    /* Set callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    l2capCbLeCbControlCallback = (NULL != controlCallback) ?
                                 controlCallback :
                                 fsciBleL2capCbLeCbControlCallback;
}


void fsciBleL2capCbHandler(void* pData, void* param, uint32_t fsciInterface)
{
    clientPacket_t* pClientPacket   = (clientPacket_t*)pData;
#if gFsciBleTest_d || gFsciBleHost_d
    uint8_t*        pBuffer         = &pClientPacket->structured.payload[0];
#endif /* gFsciBleTest_d || gFsciBleHost_d */

#if gFsciBleTest_d
    /* Mark this command as initiated by FSCI */
    bFsciBleL2capCbCmdInitiatedByFsci = TRUE;

    /* Verify if the command is Mode Select */
    if(gBleL2capCbModeSelectOpCode_c == (fsciBleL2capCbOpCode_t)pClientPacket->structured.header.opCode)
    {
        fsciBleGetBoolValueFromBuffer(bFsciBleL2capCbEnabled, pBuffer);
        /* Return status */
        fsciBleL2capCbStatusMonitor(gBleSuccess_c);
    }
    else
    {
        /* Verify if FSCI is enabled for this layer */
        if(TRUE == bFsciBleL2capCbEnabled)
        {
#endif /* gFsciBleTest_d */
            /* Select the L2CAP CB function to be called (using the FSCI opcode) */
            switch(pClientPacket->structured.header.opCode)
            {
#if gFsciBleBBox_d || gFsciBleTest_d
                case (uint8_t)gBleL2capCbCmdRegisterLeCbCallbacksOpCode_c:
                    {
                        fsciBleL2capCbCallApiFunction(L2ca_RegisterLeCbCallbacks(l2capCbLeCbDataCallback, l2capCbLeCbControlCallback));
                    }
                    break;

                case (uint8_t)gBleL2capCbCmdRegisterLePsmOpCode_c:
                case (uint8_t)gBleL2capCbCmdDeregisterLePsmOpCode_c:
                case (uint8_t)gBleL2capCbCmdConnectLePsmOpCode_c:
                case (uint8_t)gBleL2capCbCmdDisconnectLeCbChannelOpCode_c:
                case (uint8_t)gBleL2capCbCmdCancelConnectionOpCode_c:
                case (uint8_t)gBleL2capCbCmdSendLeCbDataOpCode_c:
                case (uint8_t)gBleL2capCbCmdSendLeCreditOpCode_c:
#if defined(gBLE52_d) && (gBLE52_d == 1)
                case (uint8_t)gBleL2capCbCmdEnhancedConnectLePsmOpCode_c:
                case (uint8_t)gBleL2capCbCmdEnhancedChannelReconfigureOpCode_c:
                case (uint8_t)gBleL2capCbCmdEnhancedCancelConnectionOpCode_c:
#endif /* gBLE52_d */
                    {
                        uint8_t cmdIndex = pClientPacket->structured.header.opCode - (uint8_t)gBleL2capCbCmdRegisterLePsmOpCode_c;
                        pfFsciBleL2capCbCmdHandler_t pfFsciBleL2capCbCmdHandler = maFsciBleL2capCbCmdHandler[cmdIndex];
                        gFsciStatus_t fsciStatus = pfFsciBleL2capCbCmdHandler(pClientPacket);
                        if(fsciStatus != gFsciSuccess_c)
                        {
                            fsciBleError(fsciStatus, fsciInterface);
                        }
                    }
                    break;
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
                case gBleL2capCbStatusOpCode_c:
                    {
                        bleResult_t status;

                        fsciBleGetEnumValueFromBuffer(status, pBuffer, bleResult_t);
                    }
                    break;

                case gBleL2capCbEvtLePsmConnectRequestOpCode_c:
                case gBleL2capCbEvtLePsmConnectionCompleteOpCode_c:
                case gBleL2capCbEvtLePsmDisconnectNotificationOpCode_c:
                case gBleL2capCbEvtNoPeerCreditsOpCode_c:
                case gBleL2capCbEvtLocalCreditsNotificationOpCode_c:
#if defined(gBLE52_d) && (gBLE52_d == 1)
                case gBleL2capCbEvtLePsmEnhancedConnectRequestOpCode_c:
                case gBleL2capCbEvtLePsmEnhancedConnectionCompleteOpCode_c:
                case gBleL2capCbEvtEnhancedReconfigureRequestOpCode_c:
                case gBleL2capCbEvtEnhancedReconfigureResponseOpCode_c:
#endif /* gBLE52_d */
                case gBleL2capCbEvtLowPeerCreditsOpCode_c:
                case gBleL2capCbEvtErrorOpCode_c:
                case gBleL2capCbEvtChannelStatusNotificationOpCode_c:
                    {
                        l2capControlMessageType_t   messageType = gL2ca_LePsmConnectRequest_c;
                        uint16_t                    messageSize = 0;
                        bool_t                      bMessageIncluded;

                        switch(pClientPacket->structured.header.opCode)
                        {
#if defined(gBLE52_d) && (gBLE52_d == 1)
                            case gBleL2capCbEvtLePsmEnhancedConnectRequestOpCode_c:
                                {
                                    messageType = gL2ca_LePsmEnhancedConnectRequest_c;
                                    messageSize = sizeof(l2caEnhancedConnectionRequest_t) +
                                                  sizeof(uint16_t) * gL2capEnhancedMaxChannels_c;
                                }
                                break;

                            case gBleL2capCbEvtLePsmEnhancedConnectionCompleteOpCode_c:
                                {
                                    messageType = gL2ca_LePsmEnhancedConnectionComplete_c;
                                    messageSize = sizeof(l2caEnhancedConnectionComplete_t) +
                                                  sizeof(uint16_t) * gL2capEnhancedMaxChannels_c;
                                }
                                break;

                            case gBleL2capCbEvtEnhancedReconfigureRequestOpCode_c:
                                {
                                    messageType = gL2ca_EnhancedReconfigureRequest_c;
                                    messageSize = sizeof(l2caEnhancedReconfigureRequest_t) +
                                                  sizeof(uint16_t) * gL2capEnhancedMaxChannels_c;
                                }
                                break;

                            case gBleL2capCbEvtEnhancedReconfigureResponseOpCode_c:
                                {
                                    messageType = gL2ca_EnhancedReconfigureResponse_c;
                                    messageSize = sizeof(l2caEnhancedReconfigureResponse_t);
                                }
                                break;
#endif /* gBLE52_d */
                            case gBleL2capCbEvtLePsmConnectRequestOpCode_c:
                                {
                                    messageType = gL2ca_LePsmConnectRequest_c;
                                    messageSize = sizeof(l2caLeCbConnectionRequest_t);
                                }
                                break;

                            case gBleL2capCbEvtLePsmConnectionCompleteOpCode_c:
                                {
                                    messageType = gL2ca_LePsmConnectionComplete_c;
                                    messageSize = sizeof(l2caLeCbConnectionComplete_t);
                                }
                                break;

                            case gBleL2capCbEvtLePsmDisconnectNotificationOpCode_c:
                                {
                                    messageType = gL2ca_LePsmDisconnectNotification_c;
                                    messageSize = sizeof(l2caLeCbDisconnection_t);
                                }
                                break;

                            case gBleL2capCbEvtNoPeerCreditsOpCode_c:
                                {
                                    messageType = gL2ca_NoPeerCredits_c;
                                    messageSize = sizeof(l2caLeCbNoPeerCredits_t);
                                }
                                break;

                            case gBleL2capCbEvtLocalCreditsNotificationOpCode_c:
                                {
                                    messageType = gL2ca_LocalCreditsNotification_c;
                                    messageSize = sizeof(l2caLeCbLocalCreditsNotification_t);
                                }
                                break;

                            case gBleL2capCbEvtErrorOpCode_c:
                                {
                                    messageType = gL2ca_Error_c;
                                    messageSize = sizeof(l2caLeCbError_t);
                                }
                                break;

                            case gBleL2capCbEvtChannelStatusNotificationOpCode_c:
                                {
                                    messageType = gL2ca_ChannelStatusNotification_c;
                                    messageSize = sizeof(l2caLeCbChannelStatusNotification_t);
                                }
                                break;

                            case gBleL2capCbEvtLowPeerCreditsOpCode_c:
                                {
                                    messageType = gL2ca_LowPeerCredits_c;
                                    messageSize = sizeof(l2caLeCbLowPeerCredits_t);
                                }
                                break;

                            default:
                                ; /* For MISRA compliance */
                                break;
                        }

                        fsciBleGetBoolValueFromBuffer(bMessageIncluded, pBuffer);

                        if(TRUE == bMessageIncluded)
                        {
                            void* pMessage = MEM_BufferAlloc(messageSize);

                            if(NULL != pMessage)
                            {
                                switch(pClientPacket->structured.header.opCode)
                                {
#if defined(gBLE52_d) && (gBLE52_d == 1)
                                    case gBleL2capCbEvtLePsmEnhancedConnectRequestOpCode_c:
                                        {
                                            fsciBleL2capCbGetEnhancedConnReqFromBuffer((l2caEnhancedConnectionRequest_t*)pMessage, &pBuffer);
                                        }
                                        break;

                                    case gBleL2capCbEvtLePsmEnhancedConnectionCompleteOpCode_c:
                                        {
                                            fsciBleL2capCbGetEnhancedConnCompleteFromBuffer((l2caEnhancedConnectionComplete_t*)pMessage, &pBuffer);
                                        }
                                        break;

                                    case gBleL2capCbEvtEnhancedReconfigureRequestOpCode_c:
                                        {
                                            fsciBleL2capCbGetEnhancedReconfigureReqFromBuffer((l2caEnhancedReconfigureRequest_t*)pMessage, &pBuffer);
                                        }
                                        break;

                                    case gBleL2capCbEvtEnhancedReconfigureResponseOpCode_c:
                                        {
                                            fsciBleL2capCbGetEnhancedReconfigureRspFromBuffer((l2caEnhancedReconfigureResponse_t*)pMessage, &pBuffer);
                                        }
                                        break;
#endif /* gBLE52_d */
                                    case gBleL2capCbEvtLePsmConnectRequestOpCode_c:
                                        {
                                            fsciBleL2capCbGetLeCbConnReqFromBuffer((l2caLeCbConnectionRequest_t*)pMessage, &pBuffer);
                                        }
                                        break;

                                    case gBleL2capCbEvtLePsmConnectionCompleteOpCode_c:
                                        {
                                            fsciBleL2capCbGetLeCbConnectionCompleteFromBuffer((l2caLeCbConnectionComplete_t*)pMessage, &pBuffer);
                                        }
                                        break;

                                    case gBleL2capCbEvtLePsmDisconnectNotificationOpCode_c:
                                        {
                                            fsciBleL2capCbGetLeCbDisconnectionFromBuffer((l2caLeCbDisconnection_t*)pMessage, &pBuffer);
                                        }
                                        break;

                                    case gBleL2capCbEvtNoPeerCreditsOpCode_c:
                                        {
                                            fsciBleL2capCbGetLeCbNoPeerCreditsFromBuffer((l2caLeCbNoPeerCredits_t*)pMessage, &pBuffer);
                                        }
                                        break;

                                    case gBleL2capCbEvtLocalCreditsNotificationOpCode_c:
                                        {
                                            fsciBleL2capCbGetLeCbLocalCreditsNotificationFromBuffer((l2caLeCbLocalCreditsNotification_t*)pMessage, &pBuffer);
                                        }
                                        break;

                                    case gBleL2capCbEvtErrorOpCode_c:
                                        {
                                            fsciBleL2capCbGetLeCbErrorFromBuffer((l2caLeCbError_t*)pMessage, &pBuffer);
                                        }
                                        break;

                                    case gBleL2capCbEvtChannelStatusNotificationOpCode_c:
                                        {
                                            fsciBleL2capCbGetLeCbChannelStatusNotificationFromBuffer((l2caLeCbChannelStatusNotification_t*)pMessage, &pBuffer);
                                        }
                                        break;

                                    case gBleL2capCbEvtLowPeerCreditsOpCode_c:
                                        {
                                            fsciBleL2capCbGetLeCbLowPeerCreditsFromBuffer((l2caLeCbLowPeerCredits_t*)pMessage, &pBuffer);
                                        }
                                        break;

                                    default:
                                        ; /* For MISRA compliance */
                                        break;
                                }

                                l2capCbLeCbControlCallback(messageType, pMessage);

                                (void)MEM_BufferFree(pMessage);
                            }
                            else
                            {
                                /* No buffer available - the L2CAP Credit Based callback can not be
                                called */
                                fsciBleError(gFsciOutOfMessages_c, fsciInterface);
                            }
                        }
                        else
                        {
                            l2capCbLeCbControlCallback(messageType, NULL);
                        }
                    }
                    break;

                case gBleL2capCbEvtLeCbDataOpCode_c:
                    {
                        deviceId_t  deviceId;
                        uint16_t    srcCid;
                        uint8_t*    pPacket;
                        uint16_t    packetLength;

                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        fsciBleGetUint16ValueFromBuffer(srcCid, pBuffer);
                        fsciBleGetUint16ValueFromBuffer(packetLength, pBuffer);

                        if(0 < packetLength)
                        {
                            pPacket = MEM_BufferAlloc(packetLength);

                            if(NULL != pPacket)
                            {
                                fsciBleGetArrayFromBuffer(pPacket, pBuffer, packetLength);

                                l2capCbLeCbDataCallback(deviceId, srcCid, pPacket, packetLength);

                                (void)MEM_BufferFree(pPacket);
                            }
                            else
                            {
                                /* No buffer available - the L2CAP Credit Based callback can not be
                                called */
                                fsciBleError(gFsciOutOfMessages_c, fsciInterface);
                            }
                        }
                        else
                        {
                            l2capCbLeCbDataCallback(deviceId, srcCid, NULL, packetLength);
                        }
                    }
                    break;
#endif /* gFsciBleHost_d */

                default:
                    {
                        /* Unknown FSCI opcode */
                        fsciBleError(gFsciUnknownOpcode_c, fsciInterface);
                    }
                    break;
            }
#if gFsciBleTest_d
        }
        else
        {
            /* FSCI SAP disabled */
            fsciBleError(gFsciSAPDisabled_c, fsciInterface);
        }
    }

    /* Mark the next command as not initiated by FSCI */
    bFsciBleL2capCbCmdInitiatedByFsci = FALSE;
#endif /* gFsciBleTest_d */

    (void)MEM_BufferFree(pData);
}


#if gFsciBleBBox_d || gFsciBleTest_d

void fsciBleL2capCbStatusMonitor(bleResult_t result)
{
#if gFsciBleTest_d
    /* If L2CAP CB is disabled the status must be not monitored */
    if(FALSE == bFsciBleL2capCbEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Send status over UART */
    fsciBleStatusMonitor(gFsciBleL2capCbOpcodeGroup_c, (uint8_t)gBleL2capCbStatusOpCode_c, result);
}


void fsciBleL2capCbNoParamCmdMonitor(fsciBleL2capCbOpCode_t opCode)
{
#if gFsciBleTest_d

    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        return;
    }

    /* Call the generic FSCI BLE monitor for commands or events that have no parameters */
    fsciBleNoParamCmdOrEvtMonitor(gFsciBleL2capCbOpcodeGroup_c, (uint8_t)opCode);
#endif /* gFsciBleTest_d */
}

#endif /* gFsciBleBBox_d || gFsciBleTest_d */


#if gFsciBleHost_d

#if defined(gBLE52_d) && (gBLE52_d == 1)
void fsciBleL2capCbEnhancedConnectLePsmCmdMonitor
(
    uint16_t                lePsm,
    deviceId_t              deviceId,
    uint16_t                initialCredits,
    uint8_t                 noOfChannels,
    uint16_t                *aCids
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    uint8_t                     aCidsLength = 0, iCount = 0;
    bool_t                      bContinueExecution = TRUE;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        bContinueExecution = FALSE;
    }
#endif /* gFsciBleTest_d */

    if (bContinueExecution)
    {
        if (aCids != NULL)
        {
            aCidsLength = sizeof(uint16_t) * noOfChannels;
        }

        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdEnhancedConnectLePsmOpCode_c,
                                                      sizeof(uint16_t) +
                                                      fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                      sizeof(uint16_t) +
                                                      sizeof(uint8_t) +
                                                      aCidsLength);

        if(NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];

            /* Set command parameters in the buffer */
            fsciBleGetBufferFromUint16Value(lePsm, pBuffer);
            fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
            fsciBleGetBufferFromUint16Value(initialCredits, pBuffer);
            fsciBleGetBufferFromUint8Value(noOfChannels, pBuffer);
            if (aCidsLength != 0)
            {
                for (iCount = 0; iCount < noOfChannels; iCount++)
                {
                    fsciBleGetBufferFromUint16Value(aCids[iCount], pBuffer);
                }
            }

            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
    }
}


void fsciBleL2capCbEnhancedChannelReconfigureCmdMonitor
(
    deviceId_t              deviceId,
    uint16_t                newMtu,
    uint16_t                newMps,
    uint8_t                 noOfChannels,
    uint16_t                *aCids
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    uint8_t                     iCount = 0;
    bool_t                      bContinueExecution = TRUE;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        bContinueExecution = FALSE;
    }
#endif /* gFsciBleTest_d */

    if (bContinueExecution)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdEnhancedChannelReconfigureOpCode_c,
                                                      fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                      sizeof(uint16_t) +
                                                      sizeof(uint16_t) +
                                                      sizeof(uint8_t) +
                                                      noOfChannnels * sizeof(uint16_t));

        if(NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];

            /* Set command parameters in the buffer */
            fsciBleGetBufferFromUint16Value(lePsm, pBuffer);
            fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
            fsciBleGetBufferFromUint16Value(initialCredits, pBuffer);
            fsciBleGetBufferFromUint8Value(noOfChannels, pBuffer);
            for (iCount = 0; iCount < noOfChannels; iCount++)
            {
                fsciBleGetBufferFromUint16Value(aCids[iCount], pBuffer);
            }

            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
    }
}

void fsciBleL2capCbEnhancedCancelConnectionCmdMonitor
(
    uint16_t    lePsm,
    deviceId_t  deviceId,
    l2caLeCbConnectionRequestResult_t refuseReason,
    uint8_t     noOfChannels,
    uint16_t    *aCids
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    uint8_t                     iCount = 0;
    bool_t                      bContinueExecution = TRUE;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        bContinueExecution = FALSE;
    }
#endif /* gFsciBleTest_d */

    if (bContinueExecution)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdEnhancedCancelConnectionOpCode_c,
                                                      sizeof(uint16_t) +
                                                      fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                      sizeof(l2caLeCbConnectionRequestResult_t) +
                                                      sizeof(uint8_t) +
                                                      noOfChannnels * sizeof(uint16_t));

        if(NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];

            /* Set command parameters in the buffer */
            fsciBleGetBufferFromUint16Value(lePsm, pBuffer);
            fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
            fsciBleGetBufferFromEnumValue(refuseReason, pBuffer, l2caLeCbConnectionRequestResult_t);
            fsciBleGetBufferFromUint8Value(noOfChannels, pBuffer);
            for (iCount = 0; iCount < noOfChannels; iCount++)
            {
                fsciBleGetBufferFromUint16Value(aCids[iCount], pBuffer);
            }

            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
    }
}
#endif /* gBLE52_d */

void fsciBleL2capCbConnectLePsmCmdMonitor(uint16_t lePsm, deviceId_t deviceId, uint16_t credits)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    bool_t                      bContinueExecution = TRUE;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        bContinueExecution = FALSE;
    }
#endif /* gFsciBleTest_d */

    if (bContinueExecution)
    {
         /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdConnectLePsmOpCode_c,
                                                      sizeof(uint16_t) +
                                                      fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                      sizeof(uint16_t));

        if(NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];

            /* Set command parameters in the buffer */
            fsciBleGetBufferFromUint16Value(lePsm, pBuffer);
            fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
            fsciBleGetBufferFromUint16Value(credits, pBuffer);

            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
    }
}


void fsciBleL2capCbSendLeCreditCmdMonitor(deviceId_t deviceId, uint16_t channelId, uint16_t credits)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    bool_t                      bContinueExecution = TRUE;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        bContinueExecution = FALSE;
    }
#endif /* gFsciBleTest_d */

    if (bContinueExecution)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdSendLeCreditOpCode_c,
                                                      fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                      sizeof(uint16_t) +
                                                      sizeof(uint16_t));

        if(NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];

            /* Set command parameters in the buffer */
            fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
            fsciBleGetBufferFromUint16Value(channelId, pBuffer);
            fsciBleGetBufferFromUint16Value(credits, pBuffer);

            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
    }
}


void fsciBleL2capCbRegisterLePsmCmdMonitor(uint16_t lePsm, uint16_t lePsmMtu)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    bool_t                      bContinueExecution = TRUE;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        bContinueExecution = FALSE;
    }
#endif /* gFsciBleTest_d */

    if (bContinueExecution)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdRegisterLePsmOpCode_c,
                                                      sizeof(uint16_t) + sizeof(uint16_t));

        if(NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];

            /* Set command parameters in the buffer */
            fsciBleGetBufferFromUint16Value(lePsm, pBuffer);
            fsciBleGetBufferFromUint16Value(lePsmMtu, pBuffer);

            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
    }
}


void fsciBleL2capCbDeregisterLePsmCmdMonitor(uint16_t lePsm)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    bool_t                      bContinueExecution = TRUE;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        bContinueExecution = FALSE;
    }
#else /* gFsciBleHost_d  */
    l2capCbLeCbDataCallback     = fsciBleL2capCbLeCbDataCallback;
    l2capCbLeCbControlCallback  = fsciBleL2capCbLeCbControlCallback;
#endif /* gFsciBleTest_d */

    if (bContinueExecution)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdDeregisterLePsmOpCode_c,
                                                      sizeof(uint16_t));

        if(NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];

            /* Set command parameters in the buffer */
            fsciBleGetBufferFromUint16Value(lePsm, pBuffer);

            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
    }
}


void fsciBleL2capCbDisconnectLeCbChannelCmdMonitor(deviceId_t deviceId, uint16_t channelId)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    bool_t                      bContinueExecution = TRUE;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        bContinueExecution = FALSE;
    }
#endif /* gFsciBleTest_d */

    if (bContinueExecution)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdDisconnectLeCbChannelOpCode_c,
                                                      fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                      sizeof(uint16_t));

        if(NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];

            /* Set command parameters in the buffer */
            fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
            fsciBleGetBufferFromUint16Value(channelId, pBuffer);

            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
    }
}


void fsciBleL2capCbCancelConnectionCmdMonitor(uint16_t lePsm, deviceId_t deviceId, l2caLeCbConnectionRequestResult_t refuseReason)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    bool_t                      bContinueExecution = TRUE;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        bContinueExecution = FALSE;
    }
#endif /* gFsciBleTest_d */

    if (bContinueExecution)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdCancelConnectionOpCode_c,
                                                      sizeof(uint16_t) +
                                                      fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                      sizeof(l2caLeCbConnectionRequestResult_t));

        if(NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];

            /* Set command parameters in the buffer */
            fsciBleGetBufferFromUint16Value(lePsm, pBuffer);
            fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
            fsciBleGetBufferFromEnumValue(refuseReason, pBuffer, l2caLeCbConnectionRequestResult_t);

            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
    }
}

#endif /* gFsciBleHost_d */

void fsciBleL2capCbSendLeCbDataCmdMonitor(deviceId_t deviceId, uint16_t channelId, const uint8_t* pPacket, uint16_t packetLength)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    bool_t                      bContinueExecution = TRUE;
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        bContinueExecution = FALSE;
    }
    if (bContinueExecution)
#endif /* gFsciBleTest_d */
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleL2capCbAllocFsciPacket((uint8_t)gBleL2capCbCmdSendLeCbDataOpCode_c,
                                                      sizeof(uint16_t) +
                                                      fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                      sizeof(uint16_t) + (uint32_t)packetLength);

        if(NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];

            /* Set command parameters in the buffer */
            fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
            fsciBleGetBufferFromUint16Value(channelId, pBuffer);
            fsciBleGetBufferFromUint16Value(packetLength, pBuffer);
            fsciBleGetBufferFromArray(pPacket, pBuffer, packetLength);

            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
    }
}

#if gFsciBleBBox_d || gFsciBleTest_d

void fsciBleL2capCbLeCbDataEvtMonitor(deviceId_t deviceId, uint16_t srcCid, uint8_t* pPacket, uint16_t packetLength)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled the event must be not monitored */
    bool_t                      bContinueExecution = TRUE;
    if(FALSE == bFsciBleL2capCbEnabled)
    {
        bContinueExecution = FALSE;
    }
    if (bContinueExecution)
#endif /* gFsciBleTest_d */
    {
        union
        {
            uint32_t dataLenTemp;
            uint16_t dataLen;
        }dataLength = {0};

        dataLength.dataLen = packetLength;
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleL2capCbAllocFsciPacket((uint8_t)gBleL2capCbEvtLeCbDataOpCode_c,
                                                      fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                      sizeof(uint16_t) + sizeof(uint16_t) + dataLength.dataLenTemp);

        if(NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];

            /* Set event parameters in the buffer */
            fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
            fsciBleGetBufferFromUint16Value(srcCid, pBuffer);
            fsciBleGetBufferFromUint16Value(packetLength, pBuffer);
            fsciBleGetBufferFromArray(pPacket, pBuffer, packetLength);

            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
    }
}


void fsciBleL2capCbLeCbControlEvtMonitor(l2capControlMessage_t* pMessage)
{
    uint32_t                    dataSize    = sizeof(bool_t);
    fsciBleL2capCbOpCode_t      opCode;
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    bool_t                      earlyReturn = FALSE;

#if gFsciBleTest_d
    bool_t                      bContinueExecution = TRUE;

    /* If L2CAP CB is disabled the event must be not monitored */
    if(FALSE == bFsciBleL2capCbEnabled)
    {
        bContinueExecution = FALSE;
    }
    if (bContinueExecution)
#endif /* gFsciBleTest_d */
    {
         if (pMessage != NULL)
        {
            /* Get FSCI opCode and update size needed for buffer */
            fsciBleL2capCbLeCbCtrlEvtMonUpdateOpCodeAndDataSize(pMessage, &opCode, &dataSize, &earlyReturn);

            if(!earlyReturn)
            {
                /* Allocate the packet to be sent over UART */
                pClientPacket = fsciBleL2capCbAllocFsciPacket((uint8_t)opCode, dataSize);

                if(NULL != pClientPacket)
                {
                    pBuffer = &pClientPacket->payload[0];

                    /* Set event parameters in the buffer */
                    fsciBleGetBufferFromBoolValue(!earlyReturn, pBuffer);

                    /* pMessage is not NULL and must be monitored */
                    switch(pMessage->messageType)
                    {
#if defined(gBLE52_d) && (gBLE52_d == 1)
                        case gL2ca_LePsmEnhancedConnectRequest_c:
                            {
                                fsciBleL2capCbGetBuffFromEnhancedConnReq(&pMessage->messageData.enhancedConnRequest, &pBuffer);
                            }
                            break;

                        case gL2ca_LePsmEnhancedConnectionComplete_c:
                            {
                                fsciBleL2capCbGetBuffFromEnhancedConnComplete(&pMessage->messageData.enhancedConnComplete, &pBuffer);
                            }
                            break;

                        case gL2ca_EnhancedReconfigureRequest_c:
                            {
                                fsciBleL2capCbGetBuffFromEnhancedReconfigureReq(&pMessage->messageData.reconfigureRequest, &pBuffer);
                            }
                            break;

                        case gL2ca_EnhancedReconfigureResponse_c:
                            {
                                fsciBleL2capCbGetBuffFromEnhancedReconfigureRsp(&pMessage->messageData.reconfigureResponse, &pBuffer);
                            }
                            break;
#endif /* gBLE52_d */
                        case gL2ca_LePsmConnectRequest_c:
                            {
                                fsciBleL2capCbGetBuffFromLeCbConnRequest(&pMessage->messageData.connectionRequest, &pBuffer);
                            }
                            break;

                        case gL2ca_LePsmConnectionComplete_c:
                            {
                                fsciBleL2capCbGetBufferFromLeCbConnectionComplete(&pMessage->messageData.connectionComplete, &pBuffer);
                            }
                            break;

                        case gL2ca_LePsmDisconnectNotification_c:
                            {
                                fsciBleL2capCbGetBuffFromLeCbDisconnection(&pMessage->messageData.disconnection, &pBuffer);
                            }
                            break;

                        case gL2ca_NoPeerCredits_c:
                            {
                                fsciBleL2capCbGetBuffFromLeCbNoPeerCredits(&pMessage->messageData.noPeerCredits, &pBuffer);
                            }
                            break;

                        case gL2ca_LocalCreditsNotification_c:
                            {
                                fsciBleL2capCbGetBuffFromLeCbLocalCreditsNotification(&pMessage->messageData.localCreditsNotification, &pBuffer);
                            }
                            break;

                        case gL2ca_Error_c:
                            {
                                fsciBleL2capCbGetBuffFromLeCbError(&pMessage->messageData.error, &pBuffer);
                            }
                            break;

                        case gL2ca_ChannelStatusNotification_c:
                            {
                                fsciBleL2capCbGetBuffFromLeCbChannelStatusNotification(&pMessage->messageData.channelStatusNotification, &pBuffer);
                            }
                            break;

                        case gL2ca_LowPeerCredits_c:
                            {
                                fsciBleL2capCbGetBuffFromLeCbLowPeerCredits(&pMessage->messageData.lowPeerCredits, &pBuffer);
                            }
                            break;

                        default:
                            ; /* For MISRA compliance */
                            break;
                    }

                    /* Transmit the packet over UART */
                    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
                }
            }
        }
    }
}

#endif /* gFsciBleBBox_d || gFsciBleTest_d */

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

static void fsciBleL2capCbLeCbDataCallback(deviceId_t deviceId, uint16_t lePsm, uint8_t* pPacket, uint16_t packetLength)
{
    fsciBleL2capCbLeCbDataEvtMonitor(deviceId, lePsm, pPacket, packetLength);
}

static void fsciBleL2capCbLeCbControlCallback(l2capControlMessage_t *pMessage)
{
    fsciBleL2capCbLeCbControlEvtMonitor(pMessage);
}
/* Helper function for fsciBleL2capCbLeCbControlEvtMonitor*/
static void fsciBleL2capCbLeCbCtrlEvtMonUpdateOpCodeAndDataSize(l2capControlMessage_t *pMessage, fsciBleL2capCbOpCode_t *pOpCode, uint32_t *pDataSize, bool_t *pEarlyReturn)
{
    switch(pMessage->messageType)
    {
#if defined(gBLE52_d) && (gBLE52_d == 1)
        case gL2ca_LePsmEnhancedConnectRequest_c:
        {
            *pOpCode      = gBleL2capCbEvtLePsmEnhancedConnectRequestOpCode_c;
            *pDataSize   += fsciBleL2capCbGetEnhancedConnectionRequestBufferSize(pMessage->messageData.enhancedConnRequest);
        }
        break;
        
        case gL2ca_LePsmEnhancedConnectionComplete_c:
        {
            *pOpCode      = gBleL2capCbEvtLePsmEnhancedConnectionCompleteOpCode_c;
            *pDataSize   += fsciBleL2capCbGetEnhancedConnectionCompleteBufferSize(pMessage->messageData.enhancedConnComplete);
        }
        break;
        
        case gL2ca_EnhancedReconfigureRequest_c:
        {
            *pOpCode      = gBleL2capCbEvtEnhancedReconfigureRequestOpCode_c;
            *pDataSize   += fsciBleL2capCbGetEnhancedReconfigureRequestBufferSize(pMessage->messageData.reconfigureRequest);
        }
        break;
        
        case gL2ca_EnhancedReconfigureResponse_c:
        {
            *pOpCode      = gBleL2capCbEvtEnhancedReconfigureResponseOpCode_c;
            *pDataSize   += fsciBleL2capCbGetEnhancedReconfigureResponseBufferSize(pMessage->messageData.reconfigureResponse);
        }
        break;
#endif /* gBLE52_d */
        case gL2ca_LePsmConnectRequest_c:
        {
            *pOpCode      = gBleL2capCbEvtLePsmConnectRequestOpCode_c;
            *pDataSize   += fsciBleL2capCbGetLeCbConnectionRequestBufferSize(pMessage->messageData.connectionRequest);
        }
        break;
        
        case gL2ca_LePsmConnectionComplete_c:
        {
            *pOpCode      = gBleL2capCbEvtLePsmConnectionCompleteOpCode_c;
            *pDataSize   += fsciBleL2capCbGetLeCbConnectionCompleteBufferSize(pMessage->messageData.connectionComplete);
        }
        break;
        
        case gL2ca_LePsmDisconnectNotification_c:
        {
            *pOpCode      = gBleL2capCbEvtLePsmDisconnectNotificationOpCode_c;
            *pDataSize   += fsciBleL2capCbGetLeCbDisconnectionBufferSize(pMessage->messageData.disconnection);
        }
        break;
        
        case gL2ca_NoPeerCredits_c:
        {
            *pOpCode      = gBleL2capCbEvtNoPeerCreditsOpCode_c;
            *pDataSize   += fsciBleL2capCbGetLeCbNoPeerCreditsBufferSize(pMessage->messageData.noPeerCredits);
        }
        break;
        
        case gL2ca_LocalCreditsNotification_c:
        {
            *pOpCode      = gBleL2capCbEvtLocalCreditsNotificationOpCode_c;
            *pDataSize   += fsciBleL2capCbGetLeCbLocalCreditsNotificationBufferSize(pMessage->messageData.localCreditsNotification);
        }
        break;
        
        case gL2ca_Error_c:
        {
            *pOpCode      = gBleL2capCbEvtErrorOpCode_c;
            *pDataSize   += fsciBleL2capCbGetLeCbErrorBufferSize((l2caLeCbError_t*)pMessage->messageData);
        }
        break;
        
        case gL2ca_ChannelStatusNotification_c:
        {
            *pOpCode      = gBleL2capCbEvtChannelStatusNotificationOpCode_c;
            *pDataSize   += fsciBleL2capCbGetLeCbChannelStatusNotificationBufferSize(pMessage->messageData.channelStatusNotification);
        }
        break;
        
        case gL2ca_LowPeerCredits_c:
        {
            *pOpCode      = gBleL2capCbEvtLowPeerCreditsOpCode_c;
            *pDataSize   += fsciBleL2capCbGetLeCbLowPeerCreditsBufferSize(pMessage->messageData.lowPeerCredits);
        }
        break;
        
        default:
        {
            /* Unknown message type */
            fsciBleError(gFsciError_c, fsciBleInterfaceId);
            *pEarlyReturn = TRUE;
            break;
        }
    }
}

#if gFsciBleBBox_d || gFsciBleTest_d
/* Funstion called by the fsciBleL2capCbHandler to process the gBleL2capCbCmdRegisterLePsmOpCode_c */
static gFsciStatus_t fsciBleL2capCbCmdRegisterLePsmHandler(clientPacket_t* pClientPacket)
{
    uint8_t *pBuffer = &pClientPacket->structured.payload[0];
    uint16_t lePsm;
    uint16_t lePsmMtu;
    gFsciStatus_t fsciStatus = gFsciSuccess_c;

    /* Get lePsm parameter from the received packet */
    fsciBleGetUint16ValueFromBuffer(lePsm, pBuffer);
    fsciBleGetUint16ValueFromBuffer(lePsmMtu, pBuffer);

    fsciBleL2capCbCallApiFunction(L2ca_RegisterLePsm(lePsm, lePsmMtu));
    return fsciStatus;
}
/* Funstion called by the fsciBleL2capCbHandler to process the gBleL2capCbCmdDeregisterLePsmOpCode_c */
static gFsciStatus_t fsciBleL2capCbCmdDeregisterLePsmHandler(clientPacket_t* pClientPacket)
{
    uint8_t *pBuffer = &pClientPacket->structured.payload[0];
    uint16_t lePsm;
    gFsciStatus_t fsciStatus = gFsciSuccess_c;

    /* Get lePsm parameter from the received packet */
    fsciBleGetUint16ValueFromBuffer(lePsm, pBuffer);

    fsciBleL2capCbCallApiFunction(L2ca_DeregisterLePsm(lePsm));
    return fsciStatus;
}
/* Funstion called by the fsciBleL2capCbHandler to process the gBleL2capCbCmdConnectLePsmOpCode_c */
static gFsciStatus_t fsciBleL2capCbCmdConnectLePsmHandler(clientPacket_t* pClientPacket)
{
    uint8_t *pBuffer = &pClientPacket->structured.payload[0];
    uint16_t    lePsm;
    deviceId_t  deviceId;
    uint16_t    initialCredits;
    gFsciStatus_t fsciStatus = gFsciSuccess_c;

    /* Get lePsm, deviceId and initialCredits parameters from the received packet */
    fsciBleGetUint16ValueFromBuffer(lePsm, pBuffer);
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(initialCredits, pBuffer);

    fsciBleL2capCbCallApiFunction(L2ca_ConnectLePsm(lePsm, deviceId, initialCredits));
    return fsciStatus;
}
/* Funstion called by the fsciBleL2capCbHandler to process the gBleL2capCbCmdDisconnectLeCbChannelOpCode_c */
static gFsciStatus_t fsciBleL2capCbCmdDisconnectLeCbChannelHandler(clientPacket_t* pClientPacket)
{
    uint8_t *pBuffer = &pClientPacket->structured.payload[0];
    deviceId_t  deviceId;
    uint16_t    channelId;
    gFsciStatus_t fsciStatus = gFsciSuccess_c;

    /* Get lePsm and deviceId parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(channelId, pBuffer);

    fsciBleL2capCbCallApiFunction(L2ca_DisconnectLeCbChannel(deviceId, channelId));
    return fsciStatus;
}
/* Funstion called by the fsciBleL2capCbHandler to process the gBleL2capCbCmdCancelConnectionOpCode_c */
static gFsciStatus_t fsciBleL2capCbCmdCancelConnectionHandler(clientPacket_t* pClientPacket)
{
    uint8_t *pBuffer = &pClientPacket->structured.payload[0];
    uint16_t                            lePsm;
    deviceId_t                          deviceId;
    l2caLeCbConnectionRequestResult_t   refuseReason;
    gFsciStatus_t fsciStatus = gFsciSuccess_c;

    /* Get lePsm, deviceId and refuseReason parameters from the received packet */
    fsciBleGetUint16ValueFromBuffer(lePsm, pBuffer);
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(refuseReason, pBuffer, l2caLeCbConnectionRequestResult_t);

    fsciBleL2capCbCallApiFunction(L2ca_CancelConnection(lePsm, deviceId, refuseReason));
    return fsciStatus;
}
/* Funstion called by the fsciBleL2capCbHandler to process the gBleL2capCbCmdSendLeCbDataOpCode_c */
static gFsciStatus_t fsciBleL2capCbCmdSendLeCbDataHandler(clientPacket_t* pClientPacket)
{
    uint8_t *pBuffer = &pClientPacket->structured.payload[0];
    deviceId_t  deviceId;
    uint16_t    channelId;
    uint16_t    packetLength;
    uint8_t*    pPacket;
    gFsciStatus_t fsciStatus = gFsciSuccess_c;

    /* Get lePsm, deviceId and packetLength parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(channelId, pBuffer);
    fsciBleGetUint16ValueFromBuffer(packetLength, pBuffer);
    
    /* Allocate buffer for pPacket - consider that packetLength is bigger than 0 */
    pPacket = MEM_BufferAlloc(packetLength);
    
    if(NULL != pPacket)
    {
        /* Get pPacket parameter from the received packet */
        fsciBleGetArrayFromBuffer(pPacket, pBuffer, ((uint32_t)packetLength));
        
        fsciBleL2capCbCallApiFunction(L2ca_SendLeCbData(deviceId, channelId, pPacket, packetLength));
        
        /* Free the buffer allocated for pPacket */
        (void)MEM_BufferFree(pPacket);
    }
    else
    {
        /* No buffer available - the L2CAP CB command can not be
        executed */
        fsciStatus = gFsciOutOfMessages_c;
    }
    return fsciStatus;
}
/* Funstion called by the fsciBleL2capCbHandler to process the gBleL2capCbCmdSendLeCreditOpCode_c */
static gFsciStatus_t fsciBleL2capCbCmdSendLeCreditHandler(clientPacket_t* pClientPacket)
{
    uint8_t *pBuffer = &pClientPacket->structured.payload[0];
    deviceId_t  deviceId;
    uint16_t    channelId;
    uint16_t    credits;
    gFsciStatus_t fsciStatus = gFsciSuccess_c;

    /* Get lePsm, deviceId and credits parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(channelId, pBuffer);
    fsciBleGetUint16ValueFromBuffer(credits, pBuffer);

    fsciBleL2capCbCallApiFunction(L2ca_SendLeCredit(deviceId, channelId, credits));
    return fsciStatus;
}

#if defined(gBLE52_d) && (gBLE52_d == 1)
/* Funstion called by the fsciBleL2capCbHandler to process the gBleL2capCbCmdEnhancedConnectLePsmOpCode_c */
static gFsciStatus_t fsciBleL2capCbCmdEnhancedConnectLePsmHandler(clientPacket_t* pClientPacket)
{
    uint8_t *pBuffer = &pClientPacket->structured.payload[0];
    uint16_t    lePsm;
    deviceId_t  deviceId;
    uint16_t    mtu;
    uint16_t    initialCredits;
    uint8_t     noOfChannels;
    uint16_t    aCids[gL2capEnhancedMaxChannels_c] = {0};
    uint8_t     iCount = 0;
    gFsciStatus_t fsciStatus = gFsciSuccess_c;

    /* Get lePsm, deviceId and initialCredits parameters from the received packet */
    fsciBleGetUint16ValueFromBuffer(lePsm, pBuffer);
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(mtu, pBuffer);
    fsciBleGetUint16ValueFromBuffer(initialCredits, pBuffer);
    /* Get number of channels (and list of channels if given) */
    fsciBleGetUint8ValueFromBuffer(noOfChannels, pBuffer);
    if(noOfChannels > (uint8_t)gL2capEnhancedMaxChannels_c)
    {
        noOfChannels = (uint8_t)gL2capEnhancedMaxChannels_c;
    }
    if(pClientPacket->structured.header.len >= (sizeof(uint16_t) + sizeof(deviceId_t)
                                                + sizeof(uint16_t) + sizeof(uint16_t)
                                                    + sizeof(uint8_t) + noOfChannels * sizeof(uint16_t)))
    {
        for (iCount = 0; iCount < noOfChannels; iCount++)
        {
            fsciBleGetUint16ValueFromBuffer(aCids[iCount], pBuffer);
        }
    }
    fsciBleL2capCbCallApiFunction(L2ca_EnhancedConnectLePsm(lePsm, deviceId, mtu, initialCredits, noOfChannels, aCids));
    return fsciStatus;
}
/* Funstion called by the fsciBleL2capCbHandler to process the gBleL2capCbCmdEnhancedChannelReconfigureOpCode_c */
static gFsciStatus_t fsciBleL2capCbCmdEnhancedChannelReconfigureHandler(clientPacket_t* pClientPacket)
{
    uint8_t *pBuffer = &pClientPacket->structured.payload[0];
    deviceId_t  deviceId;
    uint16_t    newMtu;
    uint16_t    newMps;
    uint8_t     noOfChannels;
    uint16_t    aCids[gL2capEnhancedMaxChannels_c] = {0};
    uint8_t     iCount = 0;
    gFsciStatus_t fsciStatus = gFsciSuccess_c;

    /* Get deviceId, new MTU and new MPS parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(newMtu, pBuffer);
    fsciBleGetUint16ValueFromBuffer(newMps, pBuffer);
    /* Get number of channels and list of channels to reconfigure */
    fsciBleGetUint8ValueFromBuffer(noOfChannels, pBuffer);
    if (noOfChannels <= gL2capEnhancedMaxChannels_c)
    {
        for (iCount = 0; iCount < noOfChannels; iCount++)
        {
            fsciBleGetUint16ValueFromBuffer(aCids[iCount], pBuffer);
        }
        fsciBleL2capCbCallApiFunction(L2ca_EnhancedChannelReconfigure(deviceId, newMtu, newMps, noOfChannels, aCids));
    }
    else
    {
        /* Invalid number of channels, trigger error handling */
        fsciBleL2capCbStatusMonitor(gBleInvalidParameter_c);
    }
    return fsciStatus;
}
/* Funstion called by the fsciBleL2capCbHandler to process the gBleL2capCbCmdEnhancedCancelConnectionOpCode_c */
static gFsciStatus_t fsciBleL2capCbCmdEnhancedCancelConnectionHandler(clientPacket_t* pClientPacket)
{
    uint8_t *pBuffer = &pClientPacket->structured.payload[0];
    uint16_t    lePsm;
    deviceId_t  deviceId;
    l2caLeCbConnectionRequestResult_t refuseReason;
    uint8_t     noOfChannels;
    uint16_t    aCids[gL2capEnhancedMaxChannels_c] = {0};
    uint8_t     iCount = 0;
    gFsciStatus_t fsciStatus = gFsciSuccess_c;

    /* Get lePsm, deviceId and refuseReason parameters from the received packet */
    fsciBleGetUint16ValueFromBuffer(lePsm, pBuffer);
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(refuseReason, pBuffer, l2caLeCbConnectionRequestResult_t);

    /* Get number and list of channels for which to cancel the pending connection */
    fsciBleGetUint8ValueFromBuffer(noOfChannels, pBuffer);

    if((noOfChannels > 0U) && (noOfChannels <= gL2capEnhancedMaxChannels_c))
    {
        for (iCount = 0; iCount < noOfChannels; iCount++)
        {
            fsciBleGetUint16ValueFromBuffer(aCids[iCount], pBuffer);
        }
        fsciBleL2capCbCallApiFunction(L2ca_EnhancedCancelConnection(lePsm, deviceId, refuseReason, noOfChannels, aCids));
    }
    else
    {
        /* Invalid number of channels, trigger error handling */
        fsciBleL2capCbStatusMonitor(gBleInvalidParameter_c);
    }
    return fsciStatus;
}
#endif /* defined(gBLE52_d) && (gBLE52_d == 1) */
#endif /* gFsciBleBBox_d || gFsciBleTest_d */
#endif /* gFsciIncluded_c && gFsciBleL2capCbLayerEnabled_d */

/*! *********************************************************************************
* @}
********************************************************************************** */
