/*! *********************************************************************************
* \addtogroup FSCI_BLE
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2019, 2021-2023 NXP
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

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
static void fsciBleL2capCbLeCbDataCallback(deviceId_t deviceId, uint16_t lePsm, uint8_t* pPacket, uint16_t packetLength);
static void fsciBleL2capCbLeCbControlCallback(l2capControlMessage_t *pMessage);

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
    uint8_t*        pBuffer         = &pClientPacket->structured.payload[0];

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
                    {
                        uint16_t lePsm;
                        uint16_t lePsmMtu;

                        /* Get lePsm parameter from the received packet */
                        fsciBleGetUint16ValueFromBuffer(lePsm, pBuffer);
                        fsciBleGetUint16ValueFromBuffer(lePsmMtu, pBuffer);

                        fsciBleL2capCbCallApiFunction(L2ca_RegisterLePsm(lePsm, lePsmMtu));
                    }
                    break;

                case (uint8_t)gBleL2capCbCmdDeregisterLePsmOpCode_c:
                    {
                        uint16_t lePsm;

                        /* Get lePsm parameter from the received packet */
                        fsciBleGetUint16ValueFromBuffer(lePsm, pBuffer);

                        fsciBleL2capCbCallApiFunction(L2ca_DeregisterLePsm(lePsm));
                    }
                    break;

                case (uint8_t)gBleL2capCbCmdConnectLePsmOpCode_c:
                    {
                        uint16_t    lePsm;
                        deviceId_t  deviceId;
                        uint16_t    initialCredits;

                        /* Get lePsm, deviceId and initialCredits parameters from the received packet */
                        fsciBleGetUint16ValueFromBuffer(lePsm, pBuffer);
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        fsciBleGetUint16ValueFromBuffer(initialCredits, pBuffer);

                        fsciBleL2capCbCallApiFunction(L2ca_ConnectLePsm(lePsm, deviceId, initialCredits));
                    }
                    break;

                case (uint8_t)gBleL2capCbCmdDisconnectLeCbChannelOpCode_c:
                    {
                        deviceId_t  deviceId;
                        uint16_t    channelId;

                        /* Get lePsm and deviceId parameters from the received packet */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        fsciBleGetUint16ValueFromBuffer(channelId, pBuffer);

                        fsciBleL2capCbCallApiFunction(L2ca_DisconnectLeCbChannel(deviceId, channelId));
                    }
                    break;

                case (uint8_t)gBleL2capCbCmdCancelConnectionOpCode_c:
                    {
                        uint16_t                            lePsm;
                        deviceId_t                          deviceId;
                        l2caLeCbConnectionRequestResult_t   refuseReason;

                        /* Get lePsm, deviceId and refuseReason parameters from the received packet */
                        fsciBleGetUint16ValueFromBuffer(lePsm, pBuffer);
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        fsciBleGetEnumValueFromBuffer(refuseReason, pBuffer, l2caLeCbConnectionRequestResult_t);

                        fsciBleL2capCbCallApiFunction(L2ca_CancelConnection(lePsm, deviceId, refuseReason));
                    }
                    break;

                case (uint8_t)gBleL2capCbCmdSendLeCbDataOpCode_c:
                    {
                        deviceId_t  deviceId;
                        uint16_t    channelId;
                        uint16_t    packetLength;
                        uint8_t*    pPacket;

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
                            fsciBleError(gFsciOutOfMessages_c, fsciInterface);
                        }
                    }
                    break;

                    case (uint8_t)gBleL2capCbCmdSendLeCreditOpCode_c:
                    {
                        deviceId_t  deviceId;
                        uint16_t    channelId;
                        uint16_t    credits;

                        /* Get lePsm, deviceId and credits parameters from the received packet */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        fsciBleGetUint16ValueFromBuffer(channelId, pBuffer);
                        fsciBleGetUint16ValueFromBuffer(credits, pBuffer);

                        fsciBleL2capCbCallApiFunction(L2ca_SendLeCredit(deviceId, channelId, credits));
                    }
                    break;

#if defined(gBLE52_d) && (gBLE52_d == 1)
                    case (uint8_t)gBleL2capCbCmdEnhancedConnectLePsmOpCode_c:
                    {
                        uint16_t    lePsm;
                        deviceId_t  deviceId;
                        uint16_t    mtu;
                        uint16_t    initialCredits;
                        uint8_t     noOfChannels;
                        uint16_t    aCids[gL2capEnhancedMaxChannels_c] = {0};
                        uint8_t     iCount = 0;

                        /* Get lePsm, deviceId and initialCredits parameters from the received packet */
                        fsciBleGetUint16ValueFromBuffer(lePsm, pBuffer);
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        fsciBleGetUint16ValueFromBuffer(mtu, pBuffer);
                        fsciBleGetUint16ValueFromBuffer(initialCredits, pBuffer);
                        /* Get number of channels (and list of channels if given) */
                        fsciBleGetUint8ValueFromBuffer(noOfChannels, pBuffer);
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
                    }
                    break;

                    case (uint8_t)gBleL2capCbCmdEnhancedChannelReconfigureOpCode_c:
                    {
                        deviceId_t  deviceId;
                        uint16_t    newMtu;
                        uint16_t    newMps;
                        uint8_t     noOfChannels;
                        uint16_t    aCids[gL2capEnhancedMaxChannels_c] = {0};
                        uint8_t     iCount = 0;

                        /* Get deviceId, new MTU and new MPS parameters from the received packet */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        fsciBleGetUint16ValueFromBuffer(newMtu, pBuffer);
                        fsciBleGetUint16ValueFromBuffer(newMps, pBuffer);
                        /* Get number of channels and list of channels to reconfigure */
                        fsciBleGetUint8ValueFromBuffer(noOfChannels, pBuffer);
                        for (iCount = 0; iCount < noOfChannels; iCount++)
                        {
                            fsciBleGetUint16ValueFromBuffer(aCids[iCount], pBuffer);
                        }
                        fsciBleL2capCbCallApiFunction(L2ca_EnhancedChannelReconfigure(deviceId, newMtu, newMps, noOfChannels, aCids));
                    }
                    break;

                    case (uint8_t)gBleL2capCbCmdEnhancedCancelConnectionOpCode_c:
                    {
                        uint16_t    lePsm;
                        deviceId_t  deviceId;
                        l2caLeCbConnectionRequestResult_t refuseReason;
                        uint8_t     noOfChannels;
                        uint16_t    aCids[gL2capEnhancedMaxChannels_c] = {0};
                        uint8_t     iCount = 0;

                        /* Get lePsm, deviceId and refuseReason parameters from the received packet */
                        fsciBleGetUint16ValueFromBuffer(lePsm, pBuffer);
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        fsciBleGetEnumValueFromBuffer(refuseReason, pBuffer, l2caLeCbConnectionRequestResult_t);

                        /* Get number and list of channels for which to cancel the pending connection */
                        fsciBleGetUint8ValueFromBuffer(noOfChannels, pBuffer);
                        for (iCount = 0; iCount < noOfChannels; iCount++)
                        {
                            fsciBleGetUint16ValueFromBuffer(aCids[iCount], pBuffer);
                        }
                        fsciBleL2capCbCallApiFunction(L2ca_EnhancedCancelConnection(lePsm, deviceId, refuseReason, noOfChannels, aCids));
                    }
                    break;

#endif /* gBLE52_d */
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

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

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

    if(NULL == pClientPacket)
    {
        return;
    }

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

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdEnhancedChannelReconfigureOpCode_c,
                                                  fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                  sizeof(uint16_t) +
                                                  sizeof(uint16_t) +
                                                  sizeof(uint8_t) +
                                                  noOfChannnels * sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

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

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdEnhancedCancelConnectionOpCode_c,
                                                  sizeof(uint16_t) +
                                                  fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                  sizeof(l2caLeCbConnectionRequestResult_t) +
                                                  sizeof(uint8_t) +
                                                  noOfChannnels * sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

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
#endif /* gBLE52_d */

void fsciBleL2capCbConnectLePsmCmdMonitor(uint16_t lePsm, deviceId_t deviceId, uint16_t credits)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdConnectLePsmOpCode_c,
                                                  sizeof(uint16_t) +
                                                  fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                  sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(lePsm, pBuffer);
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(credits, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleL2capCbSendLeCreditCmdMonitor(deviceId_t deviceId, uint16_t channelId, uint16_t credits)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdSendLeCreditOpCode_c,
                                                  fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                  sizeof(uint16_t) +
                                                  sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(channelId, pBuffer);
    fsciBleGetBufferFromUint16Value(credits, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleL2capCbRegisterLePsmCmdMonitor(uint16_t lePsm, uint16_t lePsmMtu)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdRegisterLePsmOpCode_c,
                                                  sizeof(uint16_t) + sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(lePsm, pBuffer);
    fsciBleGetBufferFromUint16Value(lePsmMtu, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleL2capCbDeregisterLePsmCmdMonitor(uint16_t lePsm)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        return;
    }
#else /* gFsciBleHost_d  */
    l2capCbLeCbDataCallback     = fsciBleL2capCbLeCbDataCallback;
    l2capCbLeCbControlCallback  = fsciBleL2capCbLeCbControlCallback;
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdDeregisterLePsmOpCode_c,
                                                  sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(lePsm, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleL2capCbDisconnectLeCbChannelCmdMonitor(deviceId_t deviceId, uint16_t channelId)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdDisconnectLeCbChannelOpCode_c,
                                                  fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                  sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(channelId, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleL2capCbCancelConnectionCmdMonitor(uint16_t lePsm, deviceId_t deviceId, l2caLeCbConnectionRequestResult_t refuseReason)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleL2capCbAllocFsciPacket(gBleL2capCbCmdCancelConnectionOpCode_c,
                                                  sizeof(uint16_t) +
                                                  fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                  sizeof(l2caLeCbConnectionRequestResult_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(lePsm, pBuffer);
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromEnumValue(refuseReason, pBuffer, l2caLeCbConnectionRequestResult_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

#endif /* gFsciBleHost_d */

void fsciBleL2capCbSendLeCbDataCmdMonitor(deviceId_t deviceId, uint16_t channelId, const uint8_t* pPacket, uint16_t packetLength)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleL2capCbEnabled) ||
       (TRUE == bFsciBleL2capCbCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleL2capCbAllocFsciPacket((uint8_t)gBleL2capCbCmdSendLeCbDataOpCode_c,
                                                  sizeof(uint16_t) +
                                                  fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                  sizeof(uint16_t) + (uint32_t)packetLength);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(channelId, pBuffer);
    fsciBleGetBufferFromUint16Value(packetLength, pBuffer);
    fsciBleGetBufferFromArray(pPacket, pBuffer, packetLength);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

#if gFsciBleBBox_d || gFsciBleTest_d

void fsciBleL2capCbLeCbDataEvtMonitor(deviceId_t deviceId, uint16_t srcCid, uint8_t* pPacket, uint16_t packetLength)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled the event must be not monitored */
    if(FALSE == bFsciBleL2capCbEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

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

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(srcCid, pBuffer);
    fsciBleGetBufferFromUint16Value(packetLength, pBuffer);
    fsciBleGetBufferFromArray(pPacket, pBuffer, packetLength);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleL2capCbLeCbControlEvtMonitor(l2capControlMessage_t* pMessage)
{
    uint32_t                    dataSize    = sizeof(bool_t);
    fsciBleL2capCbOpCode_t      opCode;
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    bool_t                      earlyReturn = FALSE;

#if gFsciBleTest_d
    /* If L2CAP CB is disabled the event must be not monitored */
    if(FALSE == bFsciBleL2capCbEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    if (pMessage != NULL)
    {
        /* Get FSCI opCode and update size needed for buffer */
        switch(pMessage->messageType)
        {
#if defined(gBLE52_d) && (gBLE52_d == 1)
            case gL2ca_LePsmEnhancedConnectRequest_c:
                {
                    opCode      = gBleL2capCbEvtLePsmEnhancedConnectRequestOpCode_c;
                    dataSize   += fsciBleL2capCbGetEnhancedConnectionRequestBufferSize(pMessage->messageData.enhancedConnRequest);
                }
                break;

            case gL2ca_LePsmEnhancedConnectionComplete_c:
                {
                    opCode      = gBleL2capCbEvtLePsmEnhancedConnectionCompleteOpCode_c;
                    dataSize   += fsciBleL2capCbGetEnhancedConnectionCompleteBufferSize(pMessage->messageData.enhancedConnComplete);
                }
                break;

            case gL2ca_EnhancedReconfigureRequest_c:
                {
                    opCode      = gBleL2capCbEvtEnhancedReconfigureRequestOpCode_c;
                    dataSize   += fsciBleL2capCbGetEnhancedReconfigureRequestBufferSize(pMessage->messageData.reconfigureRequest);
                }
                break;

            case gL2ca_EnhancedReconfigureResponse_c:
                {
                    opCode      = gBleL2capCbEvtEnhancedReconfigureResponseOpCode_c;
                    dataSize   += fsciBleL2capCbGetEnhancedReconfigureResponseBufferSize(pMessage->messageData.reconfigureResponse);
                }
                break;
#endif /* gBLE52_d */
            case gL2ca_LePsmConnectRequest_c:
                {
                    opCode      = gBleL2capCbEvtLePsmConnectRequestOpCode_c;
                    dataSize   += fsciBleL2capCbGetLeCbConnectionRequestBufferSize(pMessage->messageData.connectionRequest);
                }
                break;

            case gL2ca_LePsmConnectionComplete_c:
                {
                    opCode      = gBleL2capCbEvtLePsmConnectionCompleteOpCode_c;
                    dataSize   += fsciBleL2capCbGetLeCbConnectionCompleteBufferSize(pMessage->messageData.connectionComplete);
                }
                break;

            case gL2ca_LePsmDisconnectNotification_c:
                {
                    opCode      = gBleL2capCbEvtLePsmDisconnectNotificationOpCode_c;
                    dataSize   += fsciBleL2capCbGetLeCbDisconnectionBufferSize(pMessage->messageData.disconnection);
                }
                break;

            case gL2ca_NoPeerCredits_c:
                {
                    opCode      = gBleL2capCbEvtNoPeerCreditsOpCode_c;
                    dataSize   += fsciBleL2capCbGetLeCbNoPeerCreditsBufferSize(pMessage->messageData.noPeerCredits);
                }
                break;

            case gL2ca_LocalCreditsNotification_c:
                {
                    opCode      = gBleL2capCbEvtLocalCreditsNotificationOpCode_c;
                    dataSize   += fsciBleL2capCbGetLeCbLocalCreditsNotificationBufferSize(pMessage->messageData.localCreditsNotification);
                }
                break;

            case gL2ca_Error_c:
                {
                    opCode      = gBleL2capCbEvtErrorOpCode_c;
                    dataSize   += fsciBleL2capCbGetLeCbErrorBufferSize((l2caLeCbError_t*)pMessage->messageData);
                }
                break;

            case gL2ca_ChannelStatusNotification_c:
                {
                    opCode      = gBleL2capCbEvtChannelStatusNotificationOpCode_c;
                    dataSize   += fsciBleL2capCbGetLeCbChannelStatusNotificationBufferSize(pMessage->messageData.channelStatusNotification);
                }
                break;

            case gL2ca_LowPeerCredits_c:
                {
                    opCode      = gBleL2capCbEvtLowPeerCreditsOpCode_c;
                    dataSize   += fsciBleL2capCbGetLeCbLowPeerCreditsBufferSize(pMessage->messageData.lowPeerCredits);
                }
                break;

            default:
                {
                    /* Unknown message type */
                    fsciBleError(gFsciError_c, fsciBleInterfaceId);
                    earlyReturn = TRUE;
                    break;
                }
        }

        if(!earlyReturn)
        {
            /* Allocate the packet to be sent over UART */
            pClientPacket = fsciBleL2capCbAllocFsciPacket((uint8_t)opCode, dataSize);

            if(NULL == pClientPacket)
            {
                return;
            }

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

#endif /* gFsciIncluded_c && gFsciBleL2capCbLayerEnabled_d */

/*! *********************************************************************************
* @}
********************************************************************************** */
