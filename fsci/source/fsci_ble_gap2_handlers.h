/*! *********************************************************************************
 * \defgroup FSCI_BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2023-2026 NXP
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
    /*!                                                                         = 0x04,                         Not Used / Free to use */

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
    gBleGapCmdLeChasConfigOpCode_c                                              = 0x12,                     /*! Gap_LeChasConfig command operation code */
    gBleCtrlCmdGetTimestampExOpCode_c                                           = 0x13,                     /*! Controller_GetTimestampEx command operation code */
    gBleGapCmdSetDataRelatedAddressChanges_c                                    = 0x14,                     /*! Gap_SetDataRelatedAddress command operation code */
    gBleGapCmdSetBondedDeviceNameOpCode_c                                       = 0x15,                     /*! Gap_SetBondedDeviceName command operation code */
    gBleCtrlCmdPlatformGetDeltaTimeStampOpCode_c                                = 0x16,                     /*! PLATFORM_GetDeltaTimeStamp command operation code */
    gBleGapCmdLoadCustomBondedDeviceInformationOpCode_c                         = 0x17,                     /*! Gap_LoadCustomBondedDeviceInformation command operation code */
    gBleGapCmdLePeriodicAdvUpdateSyncOpCode_c                                   = 0x18,                     /*! Gap_LePeriodicAdvUpdateSync command operation code */
    gBleGapCmdAddDeviceToMonAdvListOpCode_c                                     = 0x19,                     /*! Gap_AddDeviceToMonAdvList command operation code */
    gBleGapCmdRemoveDeviceFromMonAdvListOpCode_c                                = 0x1A,                     /*! Gap_RemoveDeviceFromMonAdvList command operation code */
    gBleGapCmdClearMonAdvListOpCode_c                                           = 0x1B,                     /*! Gap_ClearMonAdvList command operation code */
    gBleGapCmdEnableMonAdvOpCode_c                                              = 0x1C,                     /*! Gap_EnableMonAdv command operation code */
    gBleGapCmdReadMonAdvListSizeOpCode_c                                        = 0x1D,                     /*! Gap_ReadMonAdvListSize command operation code */
    gBleGapCmdLeSetLocalPeripheralLatencyEnableOpCode_c                         = 0x1E,                     /*! Gap_LeSetLocalPeripheralLatencyEnable command operation code */
    gBleGapCmdLeGap_EncryptLinkTakOpCode_c                                      = 0x1F,                     /*! Gap_EncryptLinkTak command operation code */
    gBleGapCmdLeGap_ProvideLongTermKeyTakOpCode_c                               = 0x20,                     /*! Gap_ProvideLongTermKeyTak command operation code */
    gBleGapCmdLeModulationTestOpCode_c                                          = 0x21,                     /*! Gap_LeModulationTest command operation code */

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
    gBleGapEvtDecryptAdvertisingDataOpCode_c                                    = 0x8C,                     /*! Gap_DecryptAdvertisingData out parameters event operation code */
    gBleGapEvtGenericEventPeriodicAdvSetSubeventDataCompleteOpCode_c            = 0x8D,                     /*! gapGenericCallback (type = gPeriodicAdvSetSubeventDataComplete_c) event operation code */
    gBleGapEvtGenericEventPeriodicAdvSetResponseDataCompleteOpCode_c            = 0x8E,                     /*! gapGenericCallback (type = gPeriodicAdvSetResponseDataComplete_c) event operation code */
    gBleGapEvtGenericEventPeriodicSyncSubeventCompleteOpCode_c                  = 0x8F,                     /*! gapGenericCallback (type = gPeriodicSyncSubeventComplete_c) event operation code */
    gBleGapEvtScanningEventPeriodicDeviceScannedV2OpCode_c                      = 0x90,                     /*! gapScanningCallback (type = gPeriodicDeviceScannedV2_c) event operation code */
    gBleGapEvtAdvertisingEventPerAdvSubeventDataRequestOpCode_c                 = 0x91,                     /*! gapAdvertisingCallback (type = gPerAdvSubeventDataRequest_c) event operation code */
    gBleGapEvtAdvertisingEventPerAdvResponseOpCode_c                            = 0x92,                     /*! gapAdvertisingCallback (type = gPerAdvResponse_c) event operation code */
    gBleCtrlEvtGetTimestampExOpCode_c                                           = 0x93,                     /*! Output data from gBleCtrlCmdGetTimestampExOpCode_c */
    gBleGapEvtGenericEventVendorUnitaryTestCompleteOpCode_c                     = 0x94,                     /*! gapGenericCallback (type = gVendorUnitaryTestComplete_c) event operation code */
    gBleGapEvtGenericEventSetDataRelatedAddressChangesComplete_c                = 0x95,                     /*! gapGenericCallback (type = gLeSetDataRelatedAddressCommandComplete_c) event operation code */
    gBleCtrlEvtPlatformGetDeltaTimeStampOpCode_c                                = 0x96,                     /*! Output data from gBleCtrlCmdPlatformGetDeltaTimeStampOpCode_c */
    gBleGapEvtLoadCustomBondedDeviceInformationOpCode_c                         = 0x97,                     /*! Gap_LoadCustomBondedDeviceInformation out parameters event operation code */
    gBleGapPeriodicAdvUpdateSyncComplete_c                                      = 0x98,                     /*! gapGenericCallback (type = gLePeriodicAdvUpdateSyncComplete_c) event operation code */
    gBleGapEvtGenericEventDeviceAddedToMonAdvListOpCode_c                       = 0x99,                     /*! gapGenericCallback (type = gDeviceAddedToMonAdvList_c) event operation code */
    gBleGapEvtGenericEventDeviceRemovedFromMonAdvListOpCode_c                   = 0x9A,                     /*! gapGenericCallback (type = gDeviceRemovedFromMonAdvList_c) event operation code */
    gBleGapEvtGenericEventMonAdvListClearedOpCode_c                             = 0x9B,                     /*! gapGenericCallback (type = gMonAdvListCleared_c) event operation code */
    gBleGapEvtGenericEventMonAdvEnabledOpCode_c                                 = 0x9C,                     /*! gapGenericCallback (type = gMonAdvEnabled_c) event operation code */
    gBleGapEvtGenericEventMonAdvListSizeReadOpCode_c                            = 0x9D,                     /*! gapGenericCallback (type = gMonAdvListSizeRead_c) event operation code */
    gBleGapEvtScanningEventMonAdvReportOpCode_c                                 = 0x9E,                     /*! gapScanningCallback (type = gMonAdvReportEventReceived_c) event operation code */
    gBleGapEvtGenericEventVendorDebugOpCode_c                                   = 0x9F,                     /*! gapGenericCallback (type = gVendorDebugEvent_c) event operation code */
    gBleGapEvtGenericEventSetDefaultConnectionSubrateParametersSetupComplete_c  = 0xA0,                     /*! gapGenericCallback (type = gLeSetDefaultConnectionSubrateParametersSetupComplete_c) event operation code */
    gBleGapEvtGenericEventLeChasConfigComplete_c                                = 0xA1,                     /*! gapGenericCallback (type = gLeChasConfigComplete_c) event operation code */
    gBleGapEvtGenericEventLeModulationTestComplete_c                            = 0xA2,                     /*! gapGenericCallback (type = gLeModulationTestComplete_c) event operation code */
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

#if gFsciBleBBox_d || gFsciBleTest_d
/*! Array of handlers for GAP2 command opcodes */
extern const pfGap2OpCodeHandler_t maGap2CmdOpCodeHandlers[];

/*! Size of maGap2CmdOpCodeHandlers array */
extern const uint32_t maGap2CmdOpCodeHandlersArraySize;
#endif /* gFsciBleBBox_d || gFsciBleTest_d */
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

/*! *********************************************************************************
*\fn           uint32_t GetVendorUnitaryTestCompleteBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              vendorUnitaryTestEvent_t event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
uint32_t GetVendorUnitaryTestCompleteBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

/*! *********************************************************************************
*\fn           void GetBufferFromVendorUnitaryTestCompleteEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the vendorUnitaryTestEvent_t data fields in the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
void GetBufferFromVendorUnitaryTestCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

/*! *********************************************************************************
*\fn           void GetBufferFromVendorDebugEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the vendorSpecificDbgEvent_t data fields in the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
void GetBufferFromVendorDebugEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

/*! *********************************************************************************
*\fn           uint32_t GetVendorDebugEventBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gVendorDebugEvent_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
uint32_t GetVendorDebugEventBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

#if (defined gBLE60_MonitoredAdvertisers_d) && (gBLE60_MonitoredAdvertisers_d == TRUE)
/*! *********************************************************************************
*\fn           uint32_t GetMonAdvListSizeReadBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gMonAdvListSizeRead_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
uint32_t GetMonAdvListSizeReadBufferSize
(
    gapGenericEvent_t   *pGenericEvent
);

/*! *********************************************************************************
*\fn           void GetBufferFromMonAdvListSizeReadEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the gMonAdvListSizeRead_c data fields in the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
void GetBufferFromMonAdvListSizeReadEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
);

/*! *********************************************************************************
*\fn           void fsciBleGapGetBufferFromMonAdvReportEventReceived(
*                                           gapMonAdvReportReport_t *pScanningEvent,
*                                           uint8_t                 **ppBuffer)
*
*\brief        Writes the gMonAdvReportEventReceived_c scanning event data fields 
*              in the provided buffer.
*
*\param  [in]  pScanningEvent      Pointer to the scanning event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
void fsciBleGapGetBufferFromMonAdvReportEventReceived
(
    gapMonAdvReportReport_t *pScannedDevice,
    uint8_t                 **ppBuffer
);
#endif /* (defined gBLE60_MonitoredAdvertisers_d) && (gBLE60_MonitoredAdvertisers_d == TRUE) */
#endif /* gFsciIncluded_c && gFsciBleGapLayerEnabled_d */

/* GAP2 command handlers */
void fsciBleGap2Handler(void* pData, void* param, uint32_t fsciInterfaceId);
#if gFsciBleTest_d
void fsciBleGap2Enable(bool_t enable);
#endif /* gFsciBleTest_d */
/* GAP2 Event Handlers */
void fsciBleGap2StatusMonitor(bleResult_t result);

/*! *********************************************************************************
*\fn           void fsciBleCtrlDebugInfoCmdMonitor(
*                                           uint32_t    debugInfoSize,
*                                           uint8_t     *pDebugInfo)
*
*\brief        Constructs the FSCI packet by serializing the input parameters
*              executes FSCI transmit.
*
*\param[in]    debugInfoSize    Size of the debug info stored in pDebugInfo
*\param[in]    *pDebugInfo      Debug info from controller. The structure varies.
*
*\retval       void.
********************************************************************************** */
void fsciBleCtrlDebugInfoCmdMonitor
(
    uint32_t    debugInfoSize,
    uint8_t     *pDebugInfo
);

/*! *********************************************************************************
*\fn           void fsciBleCtrlGetTimestampExCmdMonitor(
*                                           uint32_t    ll_timing_slot,
*                                           uint16_t    ll_timing_us,
*                                           uint64_t    tstmr)
*
*\brief        Constructs the FSCI packet by serializing the input parameters
*              executes FSCI transmit.
*
*\param[in]    ll_timing_slot   Link layer timing slot
*\param[in]    ll_timing_us     Link layer timing micro second offset inside the slot
*\param[in]    tstmr            TSTMR value in us when capturing the link layer timing
*
*\retval       void.
********************************************************************************** */
void fsciBleCtrlGetTimestampExCmdMonitor
(
    uint32_t    ll_timing_slot,
    uint16_t    ll_timing_us,
    uint64_t    tstmr
);

#ifdef __cplusplus
}
#endif

#endif /* FSCI_BLE_GAP2_HANDLERS_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
