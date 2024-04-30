/*! *********************************************************************************
 * \defgroup FSCI_BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2019, 2021-2023 NXP
*
*
* \file
*
* This is a header file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef FSCI_BLE_L2CAP_CB_H
#define FSCI_BLE_L2CAP_CB_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble_l2cap_cb_types.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/

/*! Macro which indicates if FSCI for L2CAP CB is enabled or not */
#ifndef gFsciBleL2capCbLayerEnabled_d
    #define gFsciBleL2capCbLayerEnabled_d       0
#endif /* gFsciBleL2capCbLayerEnabled_d */

/*! FSCI operation group for L2CAP CB */
#define gFsciBleL2capCbOpcodeGroup_c            0x42


#if defined(FsciCmdMonitor)
    //#warning "FsciCmdMonitor macro is already defined"
    #undef FsciCmdMonitor
#endif

#if defined(FsciStatusMonitor)
    //#warning "FsciStatusMonitor macro is already defined"
    #undef FsciStatusMonitor
#endif

#if defined(FsciEvtMonitor)
    //#warning "FsciEvtMonitor macro is already defined"
    #undef FsciEvtMonitor
#endif

/*! Macros used for monitoring commands, statuses and events */
#if gFsciIncluded_c && gFsciBleL2capCbLayerEnabled_d

    #if gFsciBleHost_d || gFsciBleTest_d
        #define FsciCmdMonitorCb(function, ...)       fsciBleL2capCb##function##CmdMonitor(__VA_ARGS__)
    #else
        #define FsciCmdMonitorCb(function, ...)
    #endif

    #if gFsciBleBBox_d || gFsciBleTest_d
        #define FsciStatusMonitorCb(function, ...)   fsciBleL2capCbStatusMonitor(__VA_ARGS__)
        #define FsciEvtMonitorCb(function, ...)      fsciBleL2capCb##function##EvtMonitor(__VA_ARGS__)
    #else
        #define FsciStatusMonitorCb(function, ...)
        #define FsciEvtMonitorCb(function, ...)
    #endif

#else
    #define FsciCmdMonitorCb(function, ...)
    #define FsciStatusMonitorCb(function, ...)
    #define FsciEvtMonitorCb(function, ...)
#endif

/*! *********************************************************************************
* \brief  Allocates a FSCI packet for L2CAP CB.
*
* \param[in]    opCode      FSCI L2CAP CB operation code.
* \param[in]    dataSize    Size of the payload.
*
* \return The allocated FSCI packet
*
********************************************************************************** */
#define fsciBleL2capCbAllocFsciPacket(opCode,                   \
                                      dataSize)                 \
        fsciBleAllocFsciPacket(gFsciBleL2capCbOpcodeGroup_c,    \
                               (opCode),                        \
                               (dataSize))

/*! *********************************************************************************
* \brief  L2ca_RegisterLeCbCallbacks command monitoring macro.
*
* \param[in]    pCallback           Callback function for data plane messages
* \param[in]    pCtrlCallback       Callback function for control plane messages
*
********************************************************************************** */
#define fsciBleL2capCbRegisterLeCbCallbacksCmdMonitor(pCallback,    \
                                                    pCtrlCallback)  \
        fsciBleL2capCbNoParamCmdMonitor(gBleL2capCbCmdRegisterLeCbCallbacksOpCode_c)

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*! FSCI operation codes for L2CAP CB */
typedef enum
{
    gBleL2capCbModeSelectOpCode_c                          = 0x00,                         /*! L2CAP CB Mode Select operation code */
    gBleL2capCbCmdFirstOpCode_c                            = 0x01, 
    gBleL2capCbCmdRegisterLeCbCallbacksOpCode_c            = gBleL2capCbCmdFirstOpCode_c,  /*! L2ca_RegisterLeCbCallbacks command operation code */ 
    gBleL2capCbCmdRegisterLePsmOpCode_c                    = 0x02,                         /*! L2ca_RegisterLePsm command operation code */
    gBleL2capCbCmdDeregisterLePsmOpCode_c                  = 0x03,                         /*! L2ca_DeregisterLePsm command operation code */
    gBleL2capCbCmdConnectLePsmOpCode_c                     = 0x04,                         /*! L2ca_ConnectLePsm command operation code */
    gBleL2capCbCmdDisconnectLeCbChannelOpCode_c            = 0x05,                         /*! L2ca_DisconnectLeCbChannel command operation code */
    gBleL2capCbCmdCancelConnectionOpCode_c                 = 0x06,                         /*! L2ca_CancelConnection command operation code */
    gBleL2capCbCmdSendLeCbDataOpCode_c                     = 0x07,                         /*! L2ca_SendLeCbData command operation code */
    gBleL2capCbCmdSendLeCreditOpCode_c                     = 0x08,                         /*! L2ca_SendLeCredit command operation code */
#if defined(gBLE52_d) && (gBLE52_d == 1)
    gBleL2capCbCmdEnhancedConnectLePsmOpCode_c             = 0x09,                         /*! L2ca_EnhancedConnectLePsm command operation code */
    gBleL2capCbCmdEnhancedChannelReconfigureOpCode_c       = 0x0A,                         /*! L2ca_EnhancedChannelReconfigure command operation code */
    gBleL2capCbCmdEnhancedCancelConnectionOpCode_c         = 0x0B,                         /*! L2ca_EnhancedCancelConnection command operation code */
#endif /* gBLE52_d */
    
    gBleL2capCbStatusOpCode_c                              = 0x80,                         /*! L2CAP CB status operation code */
    
    /* Events */
    gBleL2capCbEvtFirstOpCode_c                            = 0x81, 
    gBleL2capCbEvtLePsmConnectRequestOpCode_c              = gBleL2capCbEvtFirstOpCode_c,  /*! l2caLeCbControlCallback event (messageType == gL2ca_LePsmConnectRequest_c) operation code */
    gBleL2capCbEvtLePsmConnectionCompleteOpCode_c          = 0x82,                         /*! l2caLeCbControlCallback event (messageType == gL2ca_LePsmConnectionComplete_c) operation code */
    gBleL2capCbEvtLePsmDisconnectNotificationOpCode_c      = 0x83,                         /*! l2caLeCbControlCallback event (messageType == gL2ca_LePsmDisconnectNotification_c) operation code */
    gBleL2capCbEvtNoPeerCreditsOpCode_c                    = 0x84,                         /*! l2caLeCbControlCallback event (messageType == gL2ca_NoPeerCredits_c) operation code */
    gBleL2capCbEvtLocalCreditsNotificationOpCode_c         = 0x85,                         /*! l2caLeCbControlCallback event (messageType == gL2ca_LocalCreditsNotification_c) operation code */
    gBleL2capCbEvtLeCbDataOpCode_c                         = 0x86,                         /*! l2caLeCbDataCallback event operation code */
    gBleL2capCbEvtErrorOpCode_c                            = 0x87,                         /*! l2caLeCbControlCallback event (messageType == gL2ca_Error_c) operation code */
    gBleL2capCbEvtChannelStatusNotificationOpCode_c        = 0x88,                         /*! l2caLeCbControlCallback event (messageType == gL2ca_ChannelStatusNotification_c) operation code */
#if defined(gBLE52_d) && (gBLE52_d == 1)
    gBleL2capCbEvtLePsmEnhancedConnectRequestOpCode_c      = 0x89,                         /*! l2caLeCbControlCallback event (messageType == gL2ca_LePsmEnhancedConnectRequest_c) operation code */
    gBleL2capCbEvtLePsmEnhancedConnectionCompleteOpCode_c  = 0x8A,                         /*! l2caLeCbControlCallback event (messageType == gL2ca_LePsmEnhancedConnectionComplete_c) operation code */
    gBleL2capCbEvtEnhancedReconfigureRequestOpCode_c       = 0x8B,                         /*! l2caLeCbControlCallback event (messageType == gL2ca_EnhancedReconfigureRequest_c) operation code */
    gBleL2capCbEvtEnhancedReconfigureResponseOpCode_c      = 0x8C,                         /*! l2caLeCbControlCallback event (messageType == gL2ca_EnhancedReconfigureResponse_c) operation code */
#endif /* gBLE52_d */
    gBleL2capCbEvtLowPeerCreditsOpCode_c                   = 0x8D,                         /*! l2caLeCbControlCallback event (messageType == gL2ca_LowPeerCredits_c) operation code */
}fsciBleL2capCbOpCode_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*! *********************************************************************************
* \brief  Calls the L2CAP CB function associated with the operation code received over UART.
*         The L2CAP CB function parameters are extracted from the received FSCI payload.
*
* \param[in]    pData               Packet (containing FSCI header and FSCI
                                    payload) received over UART.
* \param[in]    param               Pointer given when this function is registered in
                                    FSCI.
* \param[in]    fsciInterfaceId     FSCI interface on which the packet was received.
*
********************************************************************************** */
void fsciBleL2capCbHandler
(
    void*       pData,
    void*       param,
    uint32_t    fsciInterface
);

/*! *********************************************************************************
* \brief  Creates the FSCI packet which contains the status of the last executed L2CAP
*         CB command and sends it over UART.
*
* \param[in]    result      Status of the last executed L2CAP CB command.
*
********************************************************************************** */
void fsciBleL2capCbStatusMonitor
(
    bleResult_t result
);

/*! *********************************************************************************
* \brief  Creates a L2CAP CB FSCI packet without payload and sends it over UART.
*
* \param[in]    opCode      L2CAP CB command operation code.
*
********************************************************************************** */
void fsciBleL2capCbNoParamCmdMonitor
(
    fsciBleL2capCbOpCode_t opCode
);

#if gFsciBleHost_d
/*! *********************************************************************************
* \brief  L2ca_RegisterLePsm command monitoring function.
*
* \param[in]    lePsm               Bluetooth SIG or Vendor LE_PSM.
* \param[in]    lePsmMtu            MTU value.
*
********************************************************************************** */
void fsciBleL2capCbRegisterLePsmCmdMonitor
(
    uint16_t                    lePsm,
    uint16_t                    lePsmMtu
);

/*! *********************************************************************************
* \brief  L2ca_DeregisterLePsm command monitoring function.
*
* \param[in]    lePsm               Bluetooth SIG or Vendor LE_PSM.
*
********************************************************************************** */
void fsciBleL2capCbDeregisterLePsmCmdMonitor
(
    uint16_t lePsm
);

/*! *********************************************************************************
* \brief  L2ca_ConnectLePsm commands monitoring function.
*
* \param[in]    lePsm               Bluetooth SIG or Vendor LE_PSM.
* \param[in]    deviceId            The DeviceID for which the command is intended.
* \param[in]    initialCredits      Initial credits.
*
********************************************************************************** */
void fsciBleL2capCbConnectLePsmCmdMonitor
(
    uint16_t                lePsm,
    deviceId_t              deviceId,
    uint16_t                initialCredits
);

/*! *********************************************************************************
* \brief  L2ca_DisconnectLeCbChannel command monitoring function.
*
* \param[in]    deviceId            The DeviceID for which the command is intended.
* \param[in]    channelId           The L2CAP CB Channel Id assigned on the initiator
*
********************************************************************************** */
void fsciBleL2capCbDisconnectLeCbChannelCmdMonitor
(
    deviceId_t  deviceId,
    uint16_t    channelId
);

/*! *********************************************************************************
* \brief  L2ca_CancelConnection command monitoring function.
*
* \param[in]    lePsm               Bluetooth SIG or Vendor LE_PSM
* \param[in]    deviceId            The DeviceID for which the command is intended.
* \param[in]    refuseReason        Connection refusal reason
*
********************************************************************************** */
void fsciBleL2capCbCancelConnectionCmdMonitor
(
    uint16_t                            lePsm,
    deviceId_t                          deviceId,
    l2caLeCbConnectionRequestResult_t   refuseReason
);

#endif /* gFsciBleHost_d */

/*! *********************************************************************************
* \brief  L2ca_SendLeCbData command monitoring function.
*
* \param[in]    deviceId            The DeviceID for which the command is intended.
* \param[in]    channelId           The L2CAP CB Channel Id assigned on the initiator.
* \param[in]    pPacket             Data buffer to be transmitted.
* \param[in]    packetLength        Length of the data buffer.
*
********************************************************************************** */
void fsciBleL2capCbSendLeCbDataCmdMonitor
(
    deviceId_t     deviceId,
    uint16_t       channelId,
    const uint8_t* pPacket,
    uint16_t       packetLength
);

#if gFsciBleHost_d

/*! *********************************************************************************
* \brief  L2ca_SendLeCredit commands monitoring function.
*
* \param[in]    deviceId            The DeviceID for which the command is intended.
* \param[in]    channelId           The L2CAP CB Channel Id assigned on the initiator.
* \param[in]    credits             Initial credits or number of credits to be given.
*
********************************************************************************** */
void fsciBleL2capCbSendLeCreditCmdMonitor
(
    deviceId_t              deviceId,
    uint16_t                channelId,
    uint16_t                credits
);

#if defined(gBLE52_d) && (gBLE52_d == 1)
/*! *********************************************************************************
* \brief  L2ca_EnhancedConnectLePsm commands monitoring function.
*
* \param[in]    lePsm               Bluetooth SIG or Vendor LE_PSM
* \param[in]    deviceId            The DeviceID for which the command is intended
* \param[in]    initialCredits      Initial credits
* \param[in]    noOfChannels        Number of channels to request for opening. Max 5.
* \param[in]    aCids               List of cids if an Enhanced Connection Request has
*                                   been previously received. NULL if not.
*
********************************************************************************** */
void fsciBleL2capCbEnhancedConnectLePsmCmdMonitor
(
    uint16_t                lePsm,
    deviceId_t              deviceId,
    uint16_t                initialCredits,
    uint8_t                 noOfChannels,
    uint16_t                *aCids
);

/*! *********************************************************************************
* \brief  L2ca_EnhancedChannelReconfigure commands monitoring function.
*
* \param[in]    deviceId            The DeviceID for which the command is intended
* \param[in]    newMtu              New MTU value to be configured.
* \param[in]    newMps              New MPS value to be configured.
* \param[in]    noOfChannels        Number of channels to reconfigure. Max 5.
* \param[in]    *aCids              The list of CIDs (endpoints on local device).
*
********************************************************************************** */
void fsciBleL2capCbEnhancedChannelReconfigureCmdMonitor
(
    deviceId_t              deviceId,
    uint16_t                newMtu,
    uint16_t                newMps,
    uint8_t                 noOfChannels,
    uint16_t                *aCids
);
#endif /* gBLE52_d */
#endif /* gFsciBleHost_d */

/*! *********************************************************************************
* \brief  fsciBleL2capLeCbDataEvtMonitor event monitoring macro.
*
* \param[in]    deviceId            The DeviceID from which data is received.
* \param[in]    srcCid              Source Channel ID
* \param[in]    pPacket             Received data buffer.
* \param[in]    packetLength        Length of the data buffer.
*
********************************************************************************** */
void fsciBleL2capCbLeCbDataEvtMonitor
(
    deviceId_t              deviceId,
    uint16_t                srcCid,
    uint8_t*                pPacket,
    uint16_t                packetLength
);

/*! *********************************************************************************
* \brief  l2caLeCbControlCallback events monitoring function.
*
* \param[in]    pMessage        Message data (can be NULL).
*
********************************************************************************** */
void fsciBleL2capCbLeCbControlEvtMonitor
(
    l2capControlMessage_t* pMessage
);

/*! *********************************************************************************
* \brief   This function sets the callback function for data plane messages.
*
* \param[in]    dataCallback        The function desired by the application to be
*                                   used as data plane messages callback, or NULL.
*                                   If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetL2capCbLeCbDataCallback
(
    l2caLeCbDataCallback_t dataCallback
);

/*! *********************************************************************************
* \brief   This function sets the callback function for control plane messages.
*
* \param[in]    controlCallback     The function desired by the application to be
*                                   used as control plane messages callback, or NULL.
*                                   If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetL2capCbLeCbControlCallback
(
    l2caLeCbControlCallback_t controlCallback
);

#ifdef __cplusplus
}
#endif

#endif /* FSCI_BLE_L2ca_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
