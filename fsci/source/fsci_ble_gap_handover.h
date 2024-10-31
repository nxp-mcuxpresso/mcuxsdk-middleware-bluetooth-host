/*! *********************************************************************************
 * \defgroup FSCI_BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2022-2024 NXP
*
*
* \file
*
* This is a header file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef FSCI_BLE_GAP_HANDOVER_H
#define FSCI_BLE_GAP_HANDOVER_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "ble_general.h"
#include "fsci_ble.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/

/*! Macro which indicates if FSCI for Gap Handover is enabled or not */
#ifndef gFsciBleGapHandoverLayerEnabled_d
    #define gFsciBleGapHandoverLayerEnabled_d           0
#endif /* gFsciBleGapHandoverLayerEnabled_d */

/*! FSCI operation group for Gap Handover */
#define gFsciBleGapHandoverOpcodeGroup_c                0x4B


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
#if gFsciIncluded_c && gFsciBleGapHandoverLayerEnabled_d

    #if gFsciBleHost_d || gFsciBleTest_d
        #define FsciCmdMonitor(function, ...)       fsciBleGapHandover##function##CmdMonitor(__VA_ARGS__)
    #else
        #define FsciCmdMonitor(function, ...)
    #endif

    #if gFsciBleBBox_d || gFsciBleTest_d
        #define FsciStatusMonitor(function, ...)    fsciBleGapHandoverStatusMonitor(__VA_ARGS__)
        #define FsciEvtMonitor(function, ...)       fsciBleGapHandover##function##EvtMonitor(__VA_ARGS__)
    #else
        #define FsciStatusMonitor(function, ...)
        #define FsciEvtMonitor(function, ...)
    #endif

#else

    #define FsciCmdMonitor(function, ...)
    #define FsciStatusMonitor(function, ...)
    #define FsciEvtMonitor(function, ...)

#endif

/*! *********************************************************************************
* \brief  Allocates a FSCI packet for Gap Handover.
*
* \param[in]    opCode      FSCI Gap Handover operation code
* \param[in]    dataSize    Size of the payload
*
* \return The allocated FSCI packet
*
********************************************************************************** */
#define fsciBleGapHandoverAllocFsciPacket(opCode, dataSize)            \
        fsciBleAllocFsciPacket(gFsciBleGapHandoverOpcodeGroup_c,       \
                               (opCode),                               \
                               (dataSize))

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*! FSCI operation codes for Handover */
typedef enum
{
    gBleGapHandoverModeSelectOpCode_c                           = 0x00,                               /*! Gap Handover Mode Select operation code */

    gBleGapHandoverCmdFirstOpCode_c                             = 0x01,
    gBleGapHandoverCmdGetDataSizeOpCode_c                       = gBleGapHandoverCmdFirstOpCode_c,    /*! Gap_HandoverGetDataSize command operation code */
    gBleGapHandoverCmdGetDataOpCode_c                           = 0x02,                               /*! Gap_HandoverGetData command operation code */
    gBleGapHandoverCmdSetDataOpCode_c                           = 0x03,                               /*! Gap_HandoverSetData command operation code */
    gBleGapHandoverCmdGetTimeOpCode_c                           = 0x04,                               /*! Gap_HandoverGetTime command operation code */
    gBleGapHandoverCmdSuspendTransmitOpCode_c                   = 0x05,                               /*! Gap_HandoverSuspendTransmit command operation code */
    gBleGapHandoverCmdResumeTransmitOpCode_c                    = 0x06,                               /*! Gap_HandoverResumeTransmit command operation code */
    gBleGapHandoverCmdAnchorNotificationOpCode_c                = 0x07,                               /*! Gap_HandoverAnchorNotification command operation code */
    gBleGapHandoverCmdAnchorSearchStartOpCode_c                 = 0x08,                               /*! Gap_HandoverAnchorSearchStart command operation code */
    gBleGapHandoverCmdConnectOpCode_c                           = 0x09,                               /*! Gap_HandoverConnect command operation code */
    gBleGapHandoverCmdDisconnectOpCode_c                        = 0x0A,                               /*! Gap_HandoverDisconnect command operation code */
    gBleGapHandoverCmdInitOpCode_c                              = 0x0B,                               /*! Gap_HandoverInit command operation code */
    gBleGapHandoverCmdTimeSyncTransmitOpCode_c                  = 0x0C,                               /*! Gap_HandoverTimeSyncTransmit command operation code */
    gBleGapHandoverCmdTimeSyncReceiveOpCode_c                   = 0x0D,                               /*! Gap_HandoverTimeSyncReceive command operation code */
    gBleGapHandoverCmdAnchorSearchStopOpCode_c                  = 0x0E,                               /*! Gap_HandoverAnchorSearchStop command operation code */
    gBleGapHandoverCmdGetCsLlContextOpCode_c                    = 0x0F,                               /*! Gap_HandoverGetCsLlContext command operation code */
    gBleGapHandoverCmdSetCsLlContextOpCode_c                    = 0x10,                               /*! Gap_HandoverSetCsLlContext command operation code */
    gBleGapHandoverUpdateConnParamsOpCode_c                     = 0x11,                               /*! Gap_HandoverUpdateConnParams command operation code */
    gBleGapHandoverSetLlPendingDataOpCode_c                     = 0x12,                               /*! Gap_HandoverSetLlPendingData command operation code */
    
    gBleGapHandoverStatusOpCode_c                               = 0x80,                               /*! GAP status operation code */

    gBleGapHandoverEvtFirstOpCode_c                             = 0x81,
    gBleGapHandoverEvtGetDataSizeOpCode_c                       = gBleGapHandoverEvtFirstOpCode_c,    /*! Gap_HandoverGetDataSize command out parameters event operation code */
    gBleGapHandoverEvtGetCompleteOpCode_c                       = 0x82,                               /*! gapGenericCallback (type = gHandoverGetComplete_c) event operation code */
    gBleGapHandoverEvtSetCompleteOpCode_c                       = 0x83,                               /*! gapGenericCallback (type = gHandoverSetComplete_c) event operation code */
    gBleGapHandoverEvtGetTimeOpCode_c                           = 0x84,                               /*! gapGenericCallback (type = gHandoverGetTime_c) event operation code */
    gBleGapHandoverEvtSuspendTransmitCompleteOpCode_c           = 0x85,                               /*! gapGenericCallback (type = gHandoverSuspendTransmitComplete_c) event operation code */
    gBleGapHandoverEvtResumeTransmitCompleteOpCode_c            = 0x86,                               /*! gapGenericCallback (type = gHandoverResumeTransmitComplete_c) event operation code */
    gBleGapHandoverEvtAnchorNotificationStateChangedOpCode_c    = 0x87,                               /*! gapGenericCallback (type = gHandoverAnchorNotificationStateChanged_c) event operation code */
    gBleGapHandoverEvtAnchorSearchStartedOpCode_c               = 0x88,                               /*! gapGenericCallback (type = gHandoverAnchorSearchStarted_c) event operation code */
    gBleGapHandoverEvtAnchorSearchStoppedOpCode_c               = 0x89,                               /*! gapGenericCallback (type = gHandoverAnchorSearchStopped_c) event operation code */
    gBleGapHandoverEvtAnchorMonitorOpCode_c                     = 0x8B,                               /*! gapGenericCallback (type = gHandoverAnchorMonitorEvent_c) event operation code */
    gBleGapHandoverEvtConnectedOpCode_c                         = 0x8C,                               /*! gapConnectionCallback (type = gConnEvtHandoverConnected_c) event operation code */
    gBleGapHandoverEvtDisconnectedOpCode_c                      = 0x8D,                               /*! gapConnectionCallback (type = gHandoverDisconnected_c) event operation code */
    gBleGapHandoverTimeSyncTransmitStateChangedOpCode_c         = 0x8E,                               /*! gapGenericCallback (type = gHandoverTimeSyncTransmitStateChanged_c) event operation code */
    gBleGapHandoverTimeSyncReceiveCompleteOpCode_c              = 0x8F,                               /*! gapGenericCallback (type = gHandoverTimeSyncReceiveComplete_c) event operation code */
    gBleGapHandoverEvtTimeSyncEventOpCode_c                     = 0x90,                               /*! gapGenericCallback (type = gHandoverTimeSyncEvent_c) event operation code */
    gBleGapHandoverEvtGetCsLlContextCompleteOpCode_c            = 0x91,                               /*! gapGenericCallback (type = gHandoverGetCsLlContextComplete_c) event operation code */
    gBleGapHandoverEvtSetCsLlContextCompleteOpCode_c            = 0x92,                               /*! gapGenericCallback (type = gHandoverSetCsLlContextComplete_c) event operation code */
    gBleGapHandoverEvtConnParamUpdateOpCode_c                   = 0x93,                               /*! gapGenericCallback (type = gHandoverConnParamUpdateEvent_c) event operation code */
    gBleGapHandoverEvtAnchorMonitorPacketOpCode_c               = 0x94,                               /*! gapGenericCallback (type = gHandoverAnchorMonitorPacketEvent_c) event operation code */
    gBleGapHandoverEvtAnchorMonitorPacketContinueOpCode_c       = 0x95,                               /*! gapGenericCallback (type = gHandoverAnchorMonitorPacketContinueEvent_c) event operation code */
    gBleGapHandoverEvtUpdateConnParamsCompleteOpCode_c          = 0x96,                               /*! gapGenericCallback (type = gHandoverUpdateConnParamsComplete_c) event operation code */
    gBleGapHandoverEvtLlPendingDataOpCode_c                     = 0x97,                               /*! gapGenericCallback (type = gHandoverLlPendingData_c) event operation code */

}fsciBleGapHandoverOpCode_t;

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
* \brief  Calls the Gap Handover function associated with the operation code received
*         over UART. The Gap Handover function parameters are extracted from the
*         received FSCI payload.
*
* \param[in]    pData               Packet (containing FSCI header and FSCI
*                                   payload) received over UART.
* \param[in]    param               Pointer given when this function is
*                                   registered in FSCI.
* \param[in]    fsciInterfaceId     FSCI interface on which the packet was
*                                   received.
*
********************************************************************************** */
void fsciBleGapHandoverHandler
(
    void        *pData,
    void        *param,
    uint32_t    fsciInterfaceId
);

#if gFsciBleBBox_d || gFsciBleTest_d
/*! *********************************************************************************
* \brief  Creates the FSCI packet which contains the status of the last executed
*         Gap Handover command and sends it over UART.
*
* \param[in]    result      Status of the last executed Gap Handover command.
*
********************************************************************************** */
void fsciBleGapHandoverStatusMonitor
(
    bleResult_t result
);

/*! *********************************************************************************
* \brief  Gap_HandoverGetDataSize command out parameters monitoring function.
*
* \param[in]    pDataSize    Pointer to the Handover Data size.
*
********************************************************************************** */
void fsciBleGapHandoverGetDataSizeEvtMonitor
(
    const uint32_t *pDataSize
);

/*! *********************************************************************************
* \brief  Handover connection event monitoring function.
*
* \param[in]    deviceId           Connection peer ID.
* \param[in]    pConnectionEvent   Pointer to the connection event data.
*
********************************************************************************** */
void fsciBleGapHandoverConnectionEvtMonitor
(
    deviceId_t                  deviceId,
    gapConnectionEvent_t        *pConnectionEvent
);
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#ifdef __cplusplus
}
#endif

#endif /* FSCI_BLE_GAP_HANDOVER_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
