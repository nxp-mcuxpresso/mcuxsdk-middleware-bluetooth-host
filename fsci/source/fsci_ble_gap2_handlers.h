/*! *********************************************************************************
 * \defgroup FSCI_BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2023-2024 NXP
*
*
* \file
*
* This is a header file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef FSCI_BLE_GAP2_HANDLERS_H
#define FSCI_BLE_GAP2_HANDLERS_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "gap_types.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*! This is the type definition for Gap opcode handler pointers. */
typedef void (*pfGap2OpCodeHandler_t)
(
    uint8_t     *pBuffer,
    uint32_t    fsciInterfaceId
);

/*! FSCI operation codes for GAP */
typedef enum
{
    gBleGap2CmdFirstOpCode_c                                                    = 0x00,
    gBleCtrlCmdGenericHciCmdOpCode_c                                            = gBleGapCmdFirstOpCode_c, /*! Generic wrapper function to send HCI commands */
    gBleCtrlCmdGetDebugInfoCmdOpCode_c                                          = 0x01,                    /*! Command to read the NBU debug information */
    /* BLE 6.0 */
    gBleGapCmdSetExtAdvertisingDecisionDataOpCode_c                             = 0x02,                       /*! Sets up the Extended Advertising Decision Data */
    gBleGapCmdSetDecisionInstructionsOpCode_c                                   = 0x03,                       /*! Sets up the Decision Instructions */
    /* BLE 5.4 */
    gBleGapCmdSetExtAdvertisingParametersV2OpCode_c                             = 0x04,                     /*! Gap_SetExtAdvertisingParametersV2 command operation code */
    gBleCtrlCmdGetDebugInfo2CmdOpCode_c                                         = 0x05,                    /*! Command to read the NBU debug information 2 */
    gBleGap2StatusOpCode_c                                                      = 0x80,                    /*! GAP2 status operation code */
    gBleCtrlDebugInfoOpCode_c                                                   = 0x81,                    /*! Debug data from gBleCtrlCmdGetDebugInfoCmdOpCode_c */
    gBleGapEvtGenericEventSetExtAdvertisingDecisionDataSetupComplete_c          = 0x82,                       /*! gapGenericCallback (type = gExtAdvertisingDecisionDataSetupComplete_c) event operation code */
    gBleGapEvtGenericEventSetDecisionInstructionsSetupComplete_c                = 0x83,                       /*! gapGenericCallback (type = gDecisionInstructionsSetupComplete_c) event operation code */
    gBleCtrlDebugInfo2OpCode_c                                                  = 0x85,                    /*! Debug data 2 from gBleCtrlCmdGetDebugInfo2CmdOpCode_c */
} fsciBleGap2OpCode_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
**********************************************************************************/

#if gFsciBleHost_d
/*! Array of handlers for GAP event opcodes */
extern const pfGap2OpCodeHandler_t maGap2EvtOpCodeHandlers[];

/*! Size of maGapEvtOpCodeHandlers array */
extern const uint32_t maGap2EvtOpCodeHandlersArraySize;
#endif /* gFsciBleHost_d */

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern bleResult_t Ble_HciSend(hciPacketType_t packetType, void* pPacket, uint16_t packetSize);

/* GAP2 command handlers */
void fsciBleGap2Handler(void* pData, void* param, uint32_t fsciInterfaceId);
void fsciBleGap2Enable(bool_t enable);

/* GAP2 Event Handlers */
void fsciBleGap2StatusMonitor(bleResult_t result);
void fsciBleCtrlDebugInfoCmdMonitor
(
    nbuDebugInfo_t* pDebugInfo
);
void fsciBleCtrlDebugInfo2CmdMonitor
(
    nbuDebugInfo2_t* pDebugInfo
);

#ifdef __cplusplus
}
#endif

#endif /* FSCI_BLE_GAP2_HANDLERS_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
