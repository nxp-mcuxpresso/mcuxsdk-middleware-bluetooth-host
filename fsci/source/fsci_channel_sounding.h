/*! *********************************************************************************
 * \defgroup FSCI_BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022 - 2023 NXP
*
*
* \file
*
* This is a header file for FSCI channel sounding management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef FSCI_CS_H
#define FSCI_CS_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "fsci_ble.h"
#include "channel_sounding.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
/*! Macro which indicates if FSCI for Channel Sounding is enabled or not */
#ifndef gFsciCSLayerEnabled_d
    #define gFsciCSLayerEnabled_d         0
#endif /* gFsciCSLayerEnabled_d */

/*! FSCI operation group for Channel Sounding */
#define gFsciCSOpcodeGroup_c              0x4A

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

#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d==TRUE)
/*! Macros used for monitoring commands, statuses and events */
#if gFsciIncluded_c && gFsciCSLayerEnabled_d

    #if gFsciBleHost_d || gFsciBleTest_d
        #define FsciCmdMonitor(function, ...)       fsciCS##function##CmdMonitor(__VA_ARGS__)
    #else
        #define FsciCmdMonitor(function, ...)
    #endif

    #if gFsciBleBBox_d || gFsciBleTest_d
        #define FsciStatusMonitor(function, ...)    fsciCSStatusMonitor(__VA_ARGS__)
        #define FsciEvtMonitor(function, ...)       fsciCS##function##EvtMonitor(__VA_ARGS__)
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
* \brief  Allocates a FSCI packet for CS.
*
* \param[in]    opCode      FSCI CS operation code.
* \param[in]    dataSize    Size of the payload.
*
* \return The allocated FSCI packet
*
********************************************************************************** */
#define fsciCSAllocFsciPacket(opCode,                 \
                              dataSize)               \
        fsciBleAllocFsciPacket(gFsciCSOpcodeGroup_c,  \
                               (opCode),              \
                               (dataSize))

/*! *********************************************************************************
* \brief  CS_Init command monitoring macro.
*
********************************************************************************** */
#define fsciCSInitCmdMonitor()  \
        fsciCSNoParamCmdMonitor(gCSCmdInitOpCode_c)

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
/*! FSCI operation codes for Channel Sounding */
typedef enum
{
    /* Commands */
    gCSModeSelectOpCode_c                      = 0x00,                    /*! CS Mode Select operation code */
    gCSCmdFirstOpCode_c                        = 0x01,
    gCSCmdInitOpCode_c                         = gCSCmdFirstOpCode_c,     /*! CS_Init command operation code */
    gCSReadLocalSupportedCapabilitiesOpCode_c  = 0x02,                    /*! CS_ReadLocalSupportedCapabilities command operation code */
    gCSReadRemoteSupportedCapabilitiesOpCode_c = 0x03,                    /*! CS_ReadRemoteSupportedCapabilities command operation code */
    gCSSecurityEnableOpCode_c                  = 0x04,                    /*! CS_SecurityEnable command operation code */
    gCSSetDefaultSettingsOpCode_c              = 0x05,                    /*! CS_SetDefaultSettings command operation code */
    gCSReadRemoteFAETableOpCode_c              = 0x06,                    /*! CS_ReadRemoteFAETable command operation code */
    gCSWriteRemoteFAETableOpCode_c             = 0x07,                    /*! CS_WriteRemoteFAETable command operation code */
    gCSCreateConfigOpCode_c                    = 0x08,                    /*! CS_CreateConfig command operation code */
    gCSRemoveConfigOpCode_c                    = 0x09,                    /*! CS_RemoveConfig command operation code */
    gCSSetChannelClassificationOpCode_c        = 0x0A,                    /*! CS_SetChannelClassification command operation code */
    gCSSetProcedureParameters_c                = 0x0B,                    /*! CS_SetProcedureParameters command operation code */
    gCSProcedureEnableOpCode_c                 = 0x0C,                    /*! CS_ProcedureEnable command operation code */
    gCSTestOpCode_c                            = 0x0D,                    /*! CS_Test command operation code */
    gCSTestEndOpCode_c                         = 0x0E,                    /*! CS_TestEnd command operation code */
    gCSWriteCachedRemoteCapabilitiesOpCode_c   = 0x0F,                    /*! CS_WriteCachedRemoteSupportedCapabilities command operation code */


    /* Events */
    gCSStatusOpCode_c                                    = 0x80,                 /*! CS status operation code */
    gCSEvtFirstOpCode_c                                  = 0x81,
    gCSEvtReadRemoteSupportedCapabilitiesOpCode_c        = gCSEvtFirstOpCode_c,  /*! csReadRemoteSupportedCapabilitiesComplete event operation code */
    gCSReadRemoteFAETableCompleteOpCode_c                = 0x82,                 /*! csReadRemoteFAETableComplete event operation code */
    gCSSecurityEnableCompleteOpCode_c                    = 0x83,                 /*! csSecurityEnableComplete event operation code */
    gCSConfigCompleteOpCode_c                            = 0x84,                 /*! csConfigComplete event operation code */
    gCSProcedureEnableCompleteEventOpCode_c              = 0x85,                 /*! csProcedureEnableCompleteEvent event operation code */
    gcsSubeventResultEventOpCode_c                       = 0x86,                 /*! csSubeventResultEvent event operation code */
    gcsSubeventResultContinueEventOpCode_c               = 0x87,                 /*! csSubeventResultContinueEvent event operation code */
    gCSTestEndEvtOpCode_c                                = 0x88,                 /*! csTestEnd event operation code */
    gCsEventResultDebugOpCode_c                          = 0x89,                 /*! csEventResultDebugEvent event operation code */
    gCSErrorOpCode_c                                     = 0x8A,                 /*! csError event operation code */

    /* Command Complete Events */
    gCSEventReadLocalSupportedCapabilitiesOpCode_c       = 0x90,                 /*! readLocalSupportedCapabilities command complete event operation code */
    gCSEventSetDefaultSettingsOpCode_c                   = 0x91,                 /*! setDefaultSettings_c command complete event operation code */
    gCSEventWriteRemotelFAETableOpCode_c                 = 0x92,                 /*! writeRemoteFAETable command complete event operation code */
    gCSEventSetChannelClassificationOpCode_c             = 0x93,                 /*! setChannelClassification command complete event operation code */ 
    gCSEventTestCmdOpCode_c                              = 0x94,                 /*! test command complete event operation code */
    gCSEvenSetProcedureParamsCmdOpCode_c                 = 0x95,                 /*! setProcedureParameters_c command complete event operation code */
    gCSEventErrorOpCode_c                                = 0x96,                 /*! error event operation code */
    gCsEventWriteCachedRemoteCapabilitiesOpCode_c        = 0x97,                 /*! writeCachedRemoteCapabilities_c command complete event operation code */
    
    /* Command Status Event */
    gCSEventCommandStatusOpCode_c                        = 0xA0,                 /*! csCommandStatusEvent_t event operation code */
}fsciCSOpCode_t;

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
/*! *********************************************************************************
* \brief  Registers the LE Meta application event callback.
*
* \param[in]    callback      The LE Meta event callback.
*
********************************************************************************** */
void fsciRegisterCSMetaEventCallback
(
    csMetaEventCallback_t callback
);

/*! *********************************************************************************
* \brief  Calls the CS function associated with the operation code received over UART.
*         The CS function parameters are extracted from the received FSCI payload.
*
* \param[in]    pData               Packet (containing FSCI header and FSCI.
                                    payload) received over UART.
* \param[in]    param               Pointer given when this function is registered in
                                    FSCI.
* \param[in]    fsciInterfaceId     FSCI interface on which the packet was received.
*
********************************************************************************** */
void fsciCSHandler
(
    void* pData,
    void* param,
    uint32_t fsciBleInterfaceId
);

/*! *********************************************************************************
* \brief  Event Monitor Handler for CS LE Meta events.
*
* \param[in]    pPacket      Pointer to event data
*
********************************************************************************** */
bleResult_t fsciCSLeMetaEventHandler(csMetaEvent_t* pPacket);

/*! *********************************************************************************
* \brief  Event Monitor Handler for CS command complete events.
*
* \param[in]    pPacket      Pointer to event data
*
********************************************************************************** */
bleResult_t fsciCSCmdCompleteEventHandler(csCommandCompleteEvent_t* pPacket);

/*! *********************************************************************************
* \brief  Event Monitor Handler for CS command status events.
*
* \param[in]    errorEvent       Pointer to error event
*
********************************************************************************** */
bleResult_t fsciCSCmdStatusEventHandler(csCommandStatusEvent_t* pPacket);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* gBLE_ChannelSounding_d */
#endif /* FSCI_CS_H */

/*! *********************************************************************************
* @}
********************************************************************************** */