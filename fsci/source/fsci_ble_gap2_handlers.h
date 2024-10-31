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
    gBleCtrlCmdGenericHciCmdOpCode_c                                            = gBleGapCmdFirstOpCode_c,  /*! Generic wrapper function to send HCI commands */
    gBleCtrlCmdGetDebugInfoCmdOpCode_c                                          = 0x01,                     /*! Command to read the NBU debug information */
    /* BLE 6.0 */
    gBleGapCmdSetExtAdvertisingDecisionDataOpCode_c                             = 0x02,                     /*! Sets up the Extended Advertising Decision Data */
    gBleGapCmdSetDecisionInstructionsOpCode_c                                   = 0x03,                     /*! Sets up the Decision Instructions */

    /* BLE 5.4 */
    gBleGapCmdSetExtAdvertisingParametersV2OpCode_c                             = 0x04,                     /*! Gap_SetExtAdvertisingParametersV2 command operation code */

    gBleUnusedCmdOpCode_c                                                       = 0x05,                     /*! Not Used / Free to use */
    gBleGapCmdEcdhP256ComputeA2BKeyOpCode_c                                     = 0x06,                     /*! ECDH_P256_ComputeA2BKey command operation code */
    gBleGapCmdEcdhP256FreeE2EKeyDataOpCode_c                                    = 0x07,                     /*! ECDH_P256_FreeE2EKeyData command operation code */
    gBleGapCmdExportA2BBlobOpCode_c                                             = 0x08,                     /*! SecLib_ExportA2BBlob command operation code */
    gBleGapCmdImportA2BBlobOpCode_c                                             = 0x09,                     /*! SecLib_ImportA2BBlob command operation code */
    gBleGapCmdEcdhP256GenerateKeysOpCode_c                                      = 0x0A,                     /*! ECDH_P256_GenerateKeys command operation code */

    gBleGapCmdLeChannelOverrideOpCode_c                                         = 0x0B,                     /*! Vendor command to set the adv/scan/init channels */
    gBleGapCmdEncryptAdvertisingDataOpCode_c                                    = 0x0C,                     /*! Gap_EncryptAdvertisingData command operation code */
    gBleGapCmdDecryptAdvertisingDataOpCode_c                                    = 0x0D,                     /*! Gap_DecryptAdvertisingData command operation code */
    gBleGapCmdSetPeriodicAdvertisingSubeventDataOpCode_c                        = 0x0E,                     /*! Gap_SetPeriodicAdvSubeventData command operation code */
    gBleGapCmdSetPeriodicAdvertisingResponseDataOpCode_c                        = 0x0F,                     /*! Gap_SetPeriodicAdvResponseData command operation code */
    gBleGapCmdSetPeriodicSyncSubeventOpCode_c                                   = 0x10,                     /*! Gap_SetPeriodicSyncSubevent command operation code */
    gBleGapCmdConnectV2OpCode_c                                                 = 0x11,                     /*! Gap_ConnectFromPawr command operation code */
    gBleGapCmdSetPeriodicAdvParametersV2OpCode_c                                = 0x12,

    gBleGap2StatusOpCode_c                                                      = 0x80,                     /*! GAP2 status operation code */
    gBleCtrlDebugInfoOpCode_c                                                   = 0x81,                     /*! Debug data from gBleCtrlCmdGetDebugInfoCmdOpCode_c */
    gBleGapEvtGenericEventSetExtAdvertisingDecisionDataSetupComplete_c          = 0x82,                     /*! gapGenericCallback (type = gExtAdvertisingDecisionDataSetupComplete_c) event operation code */
    gBleGapEvtGenericEventSetDecisionInstructionsSetupComplete_c                = 0x83,                     /*! gapGenericCallback (type = gDecisionInstructionsSetupComplete_c) event operation code */
    gBleUnusedOpCode_c                                                          = 0x85,                     /*! Not Used / Free to use */
    gBleGapEvtEcdhP256ComputeA2BKeyOpCode_c                                     = 0x86,                     /*! ECDH_P256_ComputeA2BKey out parameters event operation code */
    gBleGapEvtExportA2BBlobOpCode_c                                             = 0x87,                     /*! SecLib_ExportA2BBlob out parameters event operation code */
    gBleGapEvtImportA2BBlobOpCode_c                                             = 0x88,                     /*! SecLib_ImportA2BBlob out parameters event operation code */
    gBleGapEvtEcdhP256GenerateKeysOpCode_c                                      = 0x89,                     /*! ECDH_P256_GenerateKeys out parameters event operation code */
    gBleGapChannelOverrideComplete_c                                            = 0x8A,                     /*! gapGenericCallback (type = gLeChannelOverrideComplete_c) event operation code */
    gBleGapEvtEncryptAdvertisingDataOpCode_c                                    = 0x8B,                     /*! Gap_EncryptAdvertisingData out parameters event operation code */
    gBleGapEvtDecryptAdvertisingDataOpCode_c                                    = 0x8C,                    /*! Gap_DecryptAdvertisingData out parameters event operation code */
    gBleGapEvtGenericEventPeriodicAdvSetSubeventDataCompleteOpCode_c            = 0x8D,                     /*! gapGenericCallback (type = gPeriodicAdvSetSubeventDataComplete_c) event operation code */
    gBleGapEvtGenericEventPeriodicAdvSetResponseDataCompleteOpCode_c            = 0x8E,                     /*! gapGenericCallback (type = gPeriodicAdvSetResponseDataComplete_c) event operation code */
    gBleGapEvtGenericEventPeriodicSyncSubeventCompleteOpCode_c                  = 0x8F,                     /*! gapGenericCallback (type = gPeriodicSyncSubeventComplete_c) event operation code */
    gBleGapEvtScanningEventPeriodicDeviceScannedV2OpCode_c                      = 0x90,                     /*! gapScanningCallback (type = gPeriodicDeviceScannedV2_c) event operation code */
    gBleGapEvtAdvertisingEventPerAdvSubeventDataRequestOpCode_c                 = 0x91,                     /*! gapAdvertisingCallback (type = gPerAdvSubeventDataRequest_c) event operation code */
    gBleGapEvtAdvertisingEventPerAdvResponseOpCode_c                            = 0x92,
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

#if gFsciIncluded_c && gFsciBleGapLayerEnabled_d
/*! Handler functions for fsciBleGapGetGenericEventBufferSize */
#if (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE)
/*! *********************************************************************************
*\fn           uint32_t GetPeriodicAdvSetResponseDataCompleteBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gPeriodicAdvSetResponseDataComplete_c and the
*              gPeriodicSyncSubeventComplete_c events.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
uint32_t GetPeriodicAdvSetResponseDataCompleteBufferSize(gapGenericEvent_t *pGenericEvent);

/*! *********************************************************************************
*\fn           uint32_t GetPeriodicAdvSetSubeventDataCompleteBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gPeriodicAdvSetSubeventDataComplete_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
uint32_t GetPeriodicAdvSetSubeventDataCompleteBufferSize(gapGenericEvent_t *pGenericEvent);

/*! Handler functions for fsciBleGapGetBufferFromGenericEvent */
/*! *********************************************************************************
*\fn           void GetBufferFromPeriodicAdvSetResponseDataCompleteEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the pawrSyncHandle data fields in the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
void GetBufferFromPeriodicAdvSetResponseDataCompleteEvent(gapGenericEvent_t*pGenericEvent, uint8_t **ppBuffer);

/*! *********************************************************************************
*\fn           void GetBufferFromPeriodicAdvSetSubeventDataCompleteEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the pawrAdvHandle data fields in the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
void GetBufferFromPeriodicAdvSetSubeventDataCompleteEvent(gapGenericEvent_t *pGenericEvent, uint8_t **ppBuffer);
#endif /* (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE) */
#endif /* gFsciIncluded_c && gFsciBleGapLayerEnabled_d */

/* GAP2 command handlers */
void fsciBleGap2Handler(void* pData, void* param, uint32_t fsciInterfaceId);
#if gFsciBleTest_d
void fsciBleGap2Enable(bool_t enable);
#endif /* gFsciBleTest_d */
/* GAP2 Event Handlers */
void fsciBleGap2StatusMonitor(bleResult_t result);
void fsciBleCtrlDebugInfoCmdMonitor
(
    uint32_t    debugInfoSize,
    uint8_t     *pDebugInfo
);

#ifdef __cplusplus
}
#endif

#endif /* FSCI_BLE_GAP2_HANDLERS_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
