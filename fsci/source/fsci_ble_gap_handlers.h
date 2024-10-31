/*! *********************************************************************************
 * \defgroup FSCI_BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022-2023 NXP
*
*
* \file
*
* This is a header file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef FSCI_BLE_GAP_HANDLERS_H
#define FSCI_BLE_GAP_HANDLERS_H

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

/*! This is the type definition for fsciBleGapGetConnectionEventFromBuffer
 *  handler pointers. */
typedef void (*pfGapGetConnEventFromBufferHandler_t)
(
    gapConnectionEvent_t *pConnectionEvent,
    uint8_t              **ppBuffer
);

/*! This is the type definition for fsciBleGapGetBufferFromConnectionEvent
 *  handler pointers. */
typedef pfGapGetConnEventFromBufferHandler_t pfGapGetBufferFromConnEventHandler_t;

/*! This is the type definition for fsciBleGapGetConnectionEventBufferSize
 *  handler pointers. */
typedef uint32_t (*pfGapGetConnEventBufferSizeHandler_t)
(
    gapConnectionEvent_t *pConnectionEvent
);

/*! This is the type definition for fsciBleGapGetGenericEventFromBuffer
 *  handler pointers. */
typedef void (*pfGapGetGenericEventFromBufferHandler_t)
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

/*! This is the type definition for fsciBleGapGetBufferFromGenericEvent
 *  handler pointers. */
typedef pfGapGetGenericEventFromBufferHandler_t pfGapGetBufferFromGenericEventHandler_t;

/*! This is the type definition for fsciBleGapGetGenericEventBufferSize
 *  handler pointers. */
typedef uint32_t (*pfGapGetGenericEventBufferSizeHandler_t)
(
    gapGenericEvent_t *pGenericEvent
);

/*! This is the type definition for Gap opcode handler pointers. */
typedef void (*pfGapOpCodeHandler_t)
(
    uint8_t     *pBuffer,
    uint32_t    fsciInterfaceId
);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
**********************************************************************************/

/*! Array of handlers for fsciBleGapGetConnectionEventFromBuffer */
extern const pfGapGetConnEventFromBufferHandler_t maGapGetConnEventFromBufferHandlers[];

/*! Size of maGapGetConnEventFromBufferHandlers array */
extern const uint32_t mGapGetConnEventFromBufferHandlersArraySize;

/*! Array of handlers for fsciBleGapGetBufferFromConnectionEvent */
extern const pfGapGetBufferFromConnEventHandler_t maGapGetBufferFromConnEventHandlers[];

/*! Size of maGapGetBufferFromConnEventHandlers array */
extern const uint32_t mGapGetBufferFromConnEventHandlersArraySize;

/*! Array of handlers for fsciBleGapGetConnectionEventBufferSize */
extern const pfGapGetConnEventBufferSizeHandler_t maGapGetConnEventBufferSizeHandlers[];

/*! Size of maGapGetConnEventBufferSizeHandlers array */
extern const uint32_t mGapGetConnEventBufferSizeHandlersArraySize;

/*! Array of handlers for fsciBleGapGetGenericEventFromBuffer */
extern const pfGapGetGenericEventFromBufferHandler_t maGapGetGenericEventFromBufferHandlers[];

/*! Size of maGapGetGenericEventFromBufferHandlers array */
extern const uint32_t mGapGetGenericEventFromBufferHandlersArraySize;

/*! Array of handlers for fsciBleGapGetGenericEventFromBuffer */
extern const pfGapGetBufferFromGenericEventHandler_t maGapGetBufferFromGenericEventHandlers[];

/*! Size of maGapGetBufferFromGenericEventHandlers array */
extern const uint32_t mGapGetBufferFromGenericEventHandlersArraySize;

/*! Array of handlers for fsciBleGapGetGenericEventBufferSize */
extern const pfGapGetGenericEventBufferSizeHandler_t maGapGetGenericEventBufferSizeHandlers[];

/*! Size of maGapGetGenericEventBufferSizeHandlers array */
extern const uint32_t mGapGetGenericEventBufferSizeHandlersArraySize;

/*! Array of handlers for GAP command opcodes */
extern const pfGapOpCodeHandler_t maGapCmdOpCodeHandlers[];

/*! Size of maGapCmdOpCodeHandlers array */
extern const uint32_t maGapCmdOpCodeHandlersArraySize;

/*! Located in ble_globals.c */
extern bool_t gDisablePrivacyPerAdvSet[];

#if gFsciBleHost_d
/*! Array of handlers for GAP event opcodes */
extern const pfGapOpCodeHandler_t maGapEvtOpCodeHandlers[];

/*! Size of maGapEvtOpCodeHandlers array */
extern const uint32_t maGapEvtOpCodeHandlersArraySize;
#endif /* gFsciBleHost_d */
/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  
#if gFsciBleBBox_d || gFsciBleTest_d
void HandleGapCmdBleHostInitializeOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);  
  
void HandleCtrlWritePublicDeviceAddressOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

void HandleGapCmdConnectOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

void HandleGapCmdSendPeripheralSecurityRequestOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

void HandleGapCmdSetDefaultPairingParameters
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

void HandleGapCmdSetRandomAddressOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

void HandleGapCmdSetScanModeOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

void HandleGapCmdStartAdvertisingOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

void HandleGapCmdStartScanningOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);

#if defined(gBLE50_d) && (gBLE50_d == 1U)
void HandleGapCmdStartExtAdvertisingOpCode
(
    uint8_t *pBuffer, 
    uint32_t fsciInterfaceId
);
#endif /* gBLE50_d */

#endif /* gFsciBleBBox_d || gFsciBleTest_d */
  
#ifdef __cplusplus
}
#endif

#endif /* FSCI_BLE_GAP_HANDLERS_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
