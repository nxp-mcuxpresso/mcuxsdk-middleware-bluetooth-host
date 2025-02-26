/* Source file generated from BLE.xml */
/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*==================================================================================================
Include Files
==================================================================================================*/
#include "host_cmd_ble.h"

/*==================================================================================================
Private Prototypes
==================================================================================================*/
#if FSCI_ENABLE
static mem_status_t Load_FSCIMemAllocTestResponse(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_FSCIGetNumberOfFreeBuffersResponse(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_FSCIAckIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_FSCIErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_FSCIAllowDeviceToSleepConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_FSCIWakeUpIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_FSCIGetWakeupReasonResponse(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_FSCIGetNbuVersionResponse(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* FSCI_ENABLE */

#if NVM_ENABLE
static mem_status_t Load_NVMSaveConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_NVMGetDataSetDescConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_NVMGetCountersConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_NVMGetVPSizeResponse(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_NVMSetMonitoringConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_NVMWriteMonitoringIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_NVMPageEraseMonitoringIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_NVMFormatReqConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_NVMRestoreReqConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_NVMRestoreMonitoringIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_NVMVirtualPageMonitoringIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* NVM_ENABLE */

#if HCI_ENABLE
static mem_status_t Load_HCIConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_HCIEventIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_HCIDataIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_HCISynchronousDataIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* HCI_ENABLE */

#if L2CAP_ENABLE
static mem_status_t Load_L2CAPConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPAttDataIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPSmpDataIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPSignalingDataIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* L2CAP_ENABLE */

#if L2CAPCB_ENABLE
static mem_status_t Load_L2CAPCBConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBLePsmConnectionRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBLePsmConnectionCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBLePsmDisconnectNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBNoPeerCreditsIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBLocalCreditsNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBLeCbDataIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBChannelStatusNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBLePsmEnhancedConnectRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBLePsmEnhancedConnectionCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBEnhancedReconfigureRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBEnhancedReconfigureResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_L2CAPCBLowPeerCreditsIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* L2CAPCB_ENABLE */

#if SM_ENABLE
static mem_status_t Load_SMDHKeyGenerateIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMRemoteSecurityRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMRemotePairingRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMRemotePairingResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMPasskeyDisplayRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMPasskeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMPairingKeysetRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMPairingKeysetReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMPairingCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMPairingFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMReceivedPairingFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMLlLtkRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMLlEncryptionStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMLlEncryptResIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMLlRandResIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMTbCreateRandomDeviceAddrResIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMTbCheckResolvalePrivateAddrResIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMTbSignDataResIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMTbVerifyDataSignatureResIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMRemoteKeypressNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMNcDisplayRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMLeScOobDataRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMLocalLeScOobDataIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_SMGenerateNewEcdhPkSkPairResIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* SM_ENABLE */

#if ATT_ENABLE
static mem_status_t Load_ATTConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTGetMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerErrorResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientExchangeMtuRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerExchangeMtuResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientFindInformationRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerFindInformationResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientFindByTypeValueRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerFindByTypeValueResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientReadByTypeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerReadByTypeResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientReadRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerReadResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientReadBlobRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerReadBlobResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientReadMultipleRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerReadMultipleResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientReadByGroupTypeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerReadByGroupTypeResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientWriteCommandIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientSignedWriteCommandIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientPrepareWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerPrepareWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientExecuteWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerExecuteWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerHandleValueIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTUnsupportedOpcodeIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTTimeoutIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTInvalidPduReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerReadMultipleVariableResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTServerIncomingClientReadMultipleVariableRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_ATTClientIncomingServerMultipleHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerErrorResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientFindInformationRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerFindInformationResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientFindByTypeValueRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerFindByTypeValueResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientReadByTypeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerReadByTypeResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientReadRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerReadResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientReadBlobRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerReadBlobResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientReadMultipleRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerReadMultipleResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientReadByGroupTypeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerReadByGroupTypeResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientWriteCommandIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientPrepareWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerPrepareWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientExecuteWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerExecuteWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerHandleValueIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTUnsupportedOpcodeIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTTimeoutIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTInvalidPduReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerReadMultipleVariableResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTServerIncomingClientReadMultipleVariableRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_EATTClientIncomingServerMultipleHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* ATT_ENABLE */

#if GATT_ENABLE
static mem_status_t Load_GATTConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTGetMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureExchangeMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureDiscoverAllPrimaryServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureDiscoverPrimaryServicesByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureFindIncludedServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureDiscoverAllCharacteristicsIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureDiscoverCharacteristicByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureReadCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureReadUsingCharacteristicUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureReadMultipleCharacteristicValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureWriteCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureReadCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureWriteCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientMultipleHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureReadMultipleVariableLenCharValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerMtuChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerAttributeWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerCharacteristicCccdWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerAttributeWrittenWithoutResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerLongCharacteristicWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerAttributeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedDiscoverAllPrimaryServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedDiscoverPrimaryServicesByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedFindIncludedServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedDiscoverAllCharacteristicsIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedDiscoverCharacteristicByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedDiscoverAllCharacteristicDescriptorsIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedReadCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedReadUsingCharacteristicUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedReadMultipleCharacteristicValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedWriteCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedReadCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedWriteCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientProcedureEnhancedReadMultipleVariableLenCharValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientEnhancedMultipleHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientEnhancedNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTClientEnhancedIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerEnhancedHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerEnhancedAttributeWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerEnhancedCharacteristicCccdWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerEnhancedAttributeWrittenWithoutResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerEnhancedErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerEnhancedLongCharacteristicWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTServerEnhancedAttributeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* GATT_ENABLE */

#if GATTDB_APP_ENABLE
static mem_status_t Load_GATTDBConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBReadAttributeIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBFindServiceHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBFindCharValueHandleInServiceIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBFindCccdHandleForCharValueHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBFindDescriptorHandleForCharValueHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBDynamicAddPrimaryServiceDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBDynamicAddSecondaryServiceDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBDynamicAddIncludeDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBDynamicAddCharacteristicDeclarationAndValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBDynamicAddCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBDynamicAddCccdIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBDynamicAddCharDescriptorWithUniqueValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBDynamicAddCharAggregateFormatIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* GATTDB_APP_ENABLE */

#if GATTDB_ATT_ENABLE
static mem_status_t Load_GATTDBAttConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBAttFindInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBAttFindByTypeValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBAttReadByTypeIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBAttReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBAttReadBlobIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBAttReadMultipleIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBAttReadByGroupTypeIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBAttWriteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBAttPrepareWriteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBAttExecuteWriteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBAttExecuteWriteFromQueueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GATTDBAttPrepareNotificationIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* GATTDB_ATT_ENABLE */

#if GAP_ENABLE
static mem_status_t Load_GAPConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPCheckNotificationStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPCheckIndicationStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPLoadKeysIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPLoadEncryptionInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPLoadCustomPeerInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPCheckIfBondedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGetBondedDevicesCountIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGetBondedDeviceNameIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventInitializationCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventInternalErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventAdvertisingSetupFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventAdvertisingParametersSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventAdvertisingDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventFilterAcceptListSizeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventDeviceAddedToFilterAcceptListIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventDeviceRemovedFromFilterAcceptListIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventFilterAcceptListClearedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventRandomAddressReadyIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventCreateConnectionCanceledIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventPublicAddressReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventAdvTxPowerLevelReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventPrivateResolvableAddressVerifiedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventRandomAddressSetIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPAdvertisingEventStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPAdvertisingEventCommandFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPScanningEventStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPScanningEventCommandFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPScanningEventDeviceScannedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventConnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventPairingRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventPeripheralSecurityRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventPairingResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventAuthenticationRejectedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventPasskeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventOobRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventPasskeyDisplayIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventKeyExchangeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventKeysReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventLongTermKeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventEncryptionChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventPairingCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventDisconnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventRssiReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventTxPowerLevelReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventPowerReadFailureIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventParameterUpdateRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventParameterUpdateCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventLeDataLengthChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventLeSetDataLengthFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventLeScOobDataRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventLeScDisplayNumericValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventLeScKeypressNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPLeScPublicKeyRegeneratedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventLeScLocalOobDataIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventHostPrivacyStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventControllerPrivacyStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventTxPowerLevelSetCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventLePhyEventIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGetBondedDevicesIdentityInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPControllerNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPBondCreatedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventChannelMapSetIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventChannelMapReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventChannelMapReadFailureIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventExtAdvertisingParamSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventExtAdvertisingDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventPeriodicAdvParamSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventPeriodicAdvDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventPeriodicAdvListUpdateCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPAdvertisingEventExtAdvertisingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPAdvertisingEventAdvertisingSetTerminatedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPAdvertisingEventExtAdvertisingSetRemoveCompletedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPAdvertisingEventExtScanReqReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventPeriodicAdvertisingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPScanningEventExtDeviceScannedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPScanningEventPeriodicAdvSyncEstablishedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPScanningEventPeriodicAdvSyncTerminatedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPScanningEventPeriodicAdvSyncLostIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPScanningEventPeriodicDeviceScannedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventPeriodicAdvCreateSyncCancelledIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventChannelSelectionAlgorithm2Indication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventTxEntryAvailableIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventControllerLocalRPAReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPCheckNvmIndexIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGetDeviceIdFromConnHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGetConnectionHandleFromDeviceIdIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPPairingEventNoLTKIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPPairingAlreadyStartedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventConnectionlessCteTransmitParamsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventConnectionlessCteTransmitStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventConnectionlessIqSamplingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventAntennaInformationReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPScanningEventConnectionlessIqReportReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventIqReportReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventCteRequestFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventCteReceiveParamsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventCteTransmitParamsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventCteReqStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventCteRspStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventPeriodicAdvRecvEnableCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventPeriodicAdvSyncTransferCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventPeriodicAdvSetInfoTransferCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventSetPeriodicAdvSyncTransferParamsCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventSetDefaultPeriodicAdvSyncTransferParamsCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPScanningEventPeriodicAdvSyncTransferReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventPathLossThresholdIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventTransmitPowerReportingIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventEnhancedReadTransmitPowerLevelIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventPathLossReportingParamsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventPathLossReportingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventTransmitPowerReportingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventEattConnectionRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventEattConnectionCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventEattReconfigureResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPConnectionEventEattBearerStatusNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventLeGenerateDhKeyCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGetHostVersionIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGenericEventReadRemoteVersionInfoCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPGetConnParamsIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAPPlatformErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* GAP_ENABLE */

#if APP_ENABLE
static mem_status_t Load_APPThroughputTestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_APPThroughputTestConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* APP_ENABLE */

#if CS_ENABLE
static mem_status_t Load_CSConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSReadRemoteSupportedCapabilitiesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSReadRemoteFAETableIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSSecurityEnableIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSConfigCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSProcedureEnableCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSSubeventResultIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSSubeventResultContinueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSTestEndEvtIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSEventResultDebugIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSLEMetaErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSReadLocalSupportedCapabilitiesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSSetDefaultSettingsIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSWriteRemoteFAETableIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSSetChannelClassificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSgCSEventTestCmdOpCodeIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSgCSEventSetProcedureParamsCmdOpCodeIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSgCsEventWriteCachedRemoteCapabilitiesOpCodeIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_CSCommandStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* CS_ENABLE */

#if GAPHANDOVER_ENABLE
static mem_status_t Load_GapHandoverConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverGetDataSizeIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverGetCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverSetCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverGetTimeIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverSuspendTransmitCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverResumeTransmitCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverAnchorNotificationStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverAnchorSearchStartedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverAnchorSearchStoppedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverAnchorMonitorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverAnchorMonitorPacketIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverAnchorMonitorPacketContinueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverConnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverDisconnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverTimeSyncTransmitStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverTimeSyncReceiveCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverTimeSyncIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverConnParamUpdateIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverUpdateConnParamsCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GapHandoverLlPendingDataIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* GAPHANDOVER_ENABLE */

#if GAP2_ENABLE
static mem_status_t Load_GAP2Confirm(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAP2CtrlDebugInfoIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAP2GenericEventSetExtAdvertisingDecisionDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAP2GenericEventSetDecisionInstructionsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAP2CtrlDebugInfo2Indication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAP2EcdhP256ComputeA2BKeyIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAP2ExportA2BBlobIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAP2ImportA2BBlobIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAP2EcdhP256GenerateKeysIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static mem_status_t Load_GAP2ChannelOverrideCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* GAP2_ENABLE */

/*==================================================================================================
Private global variables declarations
==================================================================================================*/
static const bleEvtHandler_t evtHandlerTbl[] =
{
#if FSCI_ENABLE
	{FSCIMemAllocTestResponse_FSCI_ID, Load_FSCIMemAllocTestResponse},
	{FSCIGetNumberOfFreeBuffersResponse_FSCI_ID, Load_FSCIGetNumberOfFreeBuffersResponse},
	{FSCIAckIndication_FSCI_ID, Load_FSCIAckIndication},
	{FSCIErrorIndication_FSCI_ID, Load_FSCIErrorIndication},
	{FSCIAllowDeviceToSleepConfirm_FSCI_ID, Load_FSCIAllowDeviceToSleepConfirm},
	{FSCIWakeUpIndication_FSCI_ID, Load_FSCIWakeUpIndication},
	{FSCIGetWakeupReasonResponse_FSCI_ID, Load_FSCIGetWakeupReasonResponse},
	{FSCIGetNbuVersionResponse_FSCI_ID, Load_FSCIGetNbuVersionResponse},
#endif  /* FSCI_ENABLE */

#if NVM_ENABLE
	{NVMSaveConfirm_FSCI_ID, Load_NVMSaveConfirm},
	{NVMGetDataSetDescConfirm_FSCI_ID, Load_NVMGetDataSetDescConfirm},
	{NVMGetCountersConfirm_FSCI_ID, Load_NVMGetCountersConfirm},
	{NVMGetVPSizeResponse_FSCI_ID, Load_NVMGetVPSizeResponse},
	{NVMSetMonitoringConfirm_FSCI_ID, Load_NVMSetMonitoringConfirm},
	{NVMWriteMonitoringIndication_FSCI_ID, Load_NVMWriteMonitoringIndication},
	{NVMPageEraseMonitoringIndication_FSCI_ID, Load_NVMPageEraseMonitoringIndication},
	{NVMFormatReqConfirm_FSCI_ID, Load_NVMFormatReqConfirm},
	{NVMRestoreReqConfirm_FSCI_ID, Load_NVMRestoreReqConfirm},
	{NVMRestoreMonitoringIndication_FSCI_ID, Load_NVMRestoreMonitoringIndication},
	{NVMVirtualPageMonitoringIndication_FSCI_ID, Load_NVMVirtualPageMonitoringIndication},
#endif  /* NVM_ENABLE */

#if HCI_ENABLE
	{HCIConfirm_FSCI_ID, Load_HCIConfirm},
	{HCIEventIndication_FSCI_ID, Load_HCIEventIndication},
	{HCIDataIndication_FSCI_ID, Load_HCIDataIndication},
	{HCISynchronousDataIndication_FSCI_ID, Load_HCISynchronousDataIndication},
#endif  /* HCI_ENABLE */

#if L2CAP_ENABLE
	{L2CAPConfirm_FSCI_ID, Load_L2CAPConfirm},
	{L2CAPAttDataIndication_FSCI_ID, Load_L2CAPAttDataIndication},
	{L2CAPSmpDataIndication_FSCI_ID, Load_L2CAPSmpDataIndication},
	{L2CAPSignalingDataIndication_FSCI_ID, Load_L2CAPSignalingDataIndication},
#endif  /* L2CAP_ENABLE */

#if L2CAPCB_ENABLE
	{L2CAPCBConfirm_FSCI_ID, Load_L2CAPCBConfirm},
	{L2CAPCBLePsmConnectionRequestIndication_FSCI_ID, Load_L2CAPCBLePsmConnectionRequestIndication},
	{L2CAPCBLePsmConnectionCompleteIndication_FSCI_ID, Load_L2CAPCBLePsmConnectionCompleteIndication},
	{L2CAPCBLePsmDisconnectNotificationIndication_FSCI_ID, Load_L2CAPCBLePsmDisconnectNotificationIndication},
	{L2CAPCBNoPeerCreditsIndication_FSCI_ID, Load_L2CAPCBNoPeerCreditsIndication},
	{L2CAPCBLocalCreditsNotificationIndication_FSCI_ID, Load_L2CAPCBLocalCreditsNotificationIndication},
	{L2CAPCBLeCbDataIndication_FSCI_ID, Load_L2CAPCBLeCbDataIndication},
	{L2CAPCBErrorIndication_FSCI_ID, Load_L2CAPCBErrorIndication},
	{L2CAPCBChannelStatusNotificationIndication_FSCI_ID, Load_L2CAPCBChannelStatusNotificationIndication},
	{L2CAPCBLePsmEnhancedConnectRequestIndication_FSCI_ID, Load_L2CAPCBLePsmEnhancedConnectRequestIndication},
	{L2CAPCBLePsmEnhancedConnectionCompleteIndication_FSCI_ID, Load_L2CAPCBLePsmEnhancedConnectionCompleteIndication},
	{L2CAPCBEnhancedReconfigureRequestIndication_FSCI_ID, Load_L2CAPCBEnhancedReconfigureRequestIndication},
	{L2CAPCBEnhancedReconfigureResponseIndication_FSCI_ID, Load_L2CAPCBEnhancedReconfigureResponseIndication},
	{L2CAPCBLowPeerCreditsIndication_FSCI_ID, Load_L2CAPCBLowPeerCreditsIndication},
#endif  /* L2CAPCB_ENABLE */

#if SM_ENABLE
	{SMDHKeyGenerateIndication_FSCI_ID, Load_SMDHKeyGenerateIndication},
	{SMConfirm_FSCI_ID, Load_SMConfirm},
	{SMStatusIndication_FSCI_ID, Load_SMStatusIndication},
	{SMRemoteSecurityRequestIndication_FSCI_ID, Load_SMRemoteSecurityRequestIndication},
	{SMRemotePairingRequestIndication_FSCI_ID, Load_SMRemotePairingRequestIndication},
	{SMRemotePairingResponseIndication_FSCI_ID, Load_SMRemotePairingResponseIndication},
	{SMPasskeyDisplayRequestIndication_FSCI_ID, Load_SMPasskeyDisplayRequestIndication},
	{SMPasskeyRequestIndication_FSCI_ID, Load_SMPasskeyRequestIndication},
	{SMPairingKeysetRequestIndication_FSCI_ID, Load_SMPairingKeysetRequestIndication},
	{SMPairingKeysetReceivedIndication_FSCI_ID, Load_SMPairingKeysetReceivedIndication},
	{SMPairingCompleteIndication_FSCI_ID, Load_SMPairingCompleteIndication},
	{SMPairingFailedIndication_FSCI_ID, Load_SMPairingFailedIndication},
	{SMReceivedPairingFailedIndication_FSCI_ID, Load_SMReceivedPairingFailedIndication},
	{SMLlLtkRequestIndication_FSCI_ID, Load_SMLlLtkRequestIndication},
	{SMLlEncryptionStatusIndication_FSCI_ID, Load_SMLlEncryptionStatusIndication},
	{SMLlEncryptResIndication_FSCI_ID, Load_SMLlEncryptResIndication},
	{SMLlRandResIndication_FSCI_ID, Load_SMLlRandResIndication},
	{SMTbCreateRandomDeviceAddrResIndication_FSCI_ID, Load_SMTbCreateRandomDeviceAddrResIndication},
	{SMTbCheckResolvalePrivateAddrResIndication_FSCI_ID, Load_SMTbCheckResolvalePrivateAddrResIndication},
	{SMTbSignDataResIndication_FSCI_ID, Load_SMTbSignDataResIndication},
	{SMTbVerifyDataSignatureResIndication_FSCI_ID, Load_SMTbVerifyDataSignatureResIndication},
	{SMRemoteKeypressNotificationIndication_FSCI_ID, Load_SMRemoteKeypressNotificationIndication},
	{SMNcDisplayRequestIndication_FSCI_ID, Load_SMNcDisplayRequestIndication},
	{SMLeScOobDataRequestIndication_FSCI_ID, Load_SMLeScOobDataRequestIndication},
	{SMLocalLeScOobDataIndication_FSCI_ID, Load_SMLocalLeScOobDataIndication},
	{SMGenerateNewEcdhPkSkPairResIndication_FSCI_ID, Load_SMGenerateNewEcdhPkSkPairResIndication},
#endif  /* SM_ENABLE */

#if ATT_ENABLE
	{ATTConfirm_FSCI_ID, Load_ATTConfirm},
	{ATTGetMtuIndication_FSCI_ID, Load_ATTGetMtuIndication},
	{ATTClientIncomingServerErrorResponseIndication_FSCI_ID, Load_ATTClientIncomingServerErrorResponseIndication},
	{ATTServerIncomingClientExchangeMtuRequestIndication_FSCI_ID, Load_ATTServerIncomingClientExchangeMtuRequestIndication},
	{ATTClientIncomingServerExchangeMtuResponseIndication_FSCI_ID, Load_ATTClientIncomingServerExchangeMtuResponseIndication},
	{ATTServerIncomingClientFindInformationRequestIndication_FSCI_ID, Load_ATTServerIncomingClientFindInformationRequestIndication},
	{ATTClientIncomingServerFindInformationResponseIndication_FSCI_ID, Load_ATTClientIncomingServerFindInformationResponseIndication},
	{ATTServerIncomingClientFindByTypeValueRequestIndication_FSCI_ID, Load_ATTServerIncomingClientFindByTypeValueRequestIndication},
	{ATTClientIncomingServerFindByTypeValueResponseIndication_FSCI_ID, Load_ATTClientIncomingServerFindByTypeValueResponseIndication},
	{ATTServerIncomingClientReadByTypeRequestIndication_FSCI_ID, Load_ATTServerIncomingClientReadByTypeRequestIndication},
	{ATTClientIncomingServerReadByTypeResponseIndication_FSCI_ID, Load_ATTClientIncomingServerReadByTypeResponseIndication},
	{ATTServerIncomingClientReadRequestIndication_FSCI_ID, Load_ATTServerIncomingClientReadRequestIndication},
	{ATTClientIncomingServerReadResponseIndication_FSCI_ID, Load_ATTClientIncomingServerReadResponseIndication},
	{ATTServerIncomingClientReadBlobRequestIndication_FSCI_ID, Load_ATTServerIncomingClientReadBlobRequestIndication},
	{ATTClientIncomingServerReadBlobResponseIndication_FSCI_ID, Load_ATTClientIncomingServerReadBlobResponseIndication},
	{ATTServerIncomingClientReadMultipleRequestIndication_FSCI_ID, Load_ATTServerIncomingClientReadMultipleRequestIndication},
	{ATTClientIncomingServerReadMultipleResponseIndication_FSCI_ID, Load_ATTClientIncomingServerReadMultipleResponseIndication},
	{ATTServerIncomingClientReadByGroupTypeRequestIndication_FSCI_ID, Load_ATTServerIncomingClientReadByGroupTypeRequestIndication},
	{ATTClientIncomingServerReadByGroupTypeResponseIndication_FSCI_ID, Load_ATTClientIncomingServerReadByGroupTypeResponseIndication},
	{ATTServerIncomingClientWriteRequestIndication_FSCI_ID, Load_ATTServerIncomingClientWriteRequestIndication},
	{ATTClientIncomingServerWriteResponseIndication_FSCI_ID, Load_ATTClientIncomingServerWriteResponseIndication},
	{ATTServerIncomingClientWriteCommandIndication_FSCI_ID, Load_ATTServerIncomingClientWriteCommandIndication},
	{ATTServerIncomingClientSignedWriteCommandIndication_FSCI_ID, Load_ATTServerIncomingClientSignedWriteCommandIndication},
	{ATTServerIncomingClientPrepareWriteRequestIndication_FSCI_ID, Load_ATTServerIncomingClientPrepareWriteRequestIndication},
	{ATTClientIncomingServerPrepareWriteResponseIndication_FSCI_ID, Load_ATTClientIncomingServerPrepareWriteResponseIndication},
	{ATTServerIncomingClientExecuteWriteRequestIndication_FSCI_ID, Load_ATTServerIncomingClientExecuteWriteRequestIndication},
	{ATTClientIncomingServerExecuteWriteResponseIndication_FSCI_ID, Load_ATTClientIncomingServerExecuteWriteResponseIndication},
	{ATTClientIncomingServerHandleValueNotificationIndication_FSCI_ID, Load_ATTClientIncomingServerHandleValueNotificationIndication},
	{ATTClientIncomingServerHandleValueIndicationIndication_FSCI_ID, Load_ATTClientIncomingServerHandleValueIndicationIndication},
	{ATTServerIncomingClientHandleValueConfirmationIndication_FSCI_ID, Load_ATTServerIncomingClientHandleValueConfirmationIndication},
	{ATTUnsupportedOpcodeIndication_FSCI_ID, Load_ATTUnsupportedOpcodeIndication},
	{ATTTimeoutIndication_FSCI_ID, Load_ATTTimeoutIndication},
	{ATTInvalidPduReceivedIndication_FSCI_ID, Load_ATTInvalidPduReceivedIndication},
	{ATTClientIncomingServerReadMultipleVariableResponseIndication_FSCI_ID, Load_ATTClientIncomingServerReadMultipleVariableResponseIndication},
	{ATTServerIncomingClientReadMultipleVariableRequestIndication_FSCI_ID, Load_ATTServerIncomingClientReadMultipleVariableRequestIndication},
	{ATTClientIncomingServerMultipleHandleValueNotificationIndication_FSCI_ID, Load_ATTClientIncomingServerMultipleHandleValueNotificationIndication},
	{EATTClientIncomingServerErrorResponseIndication_FSCI_ID, Load_EATTClientIncomingServerErrorResponseIndication},
	{EATTServerIncomingClientFindInformationRequestIndication_FSCI_ID, Load_EATTServerIncomingClientFindInformationRequestIndication},
	{EATTClientIncomingServerFindInformationResponseIndication_FSCI_ID, Load_EATTClientIncomingServerFindInformationResponseIndication},
	{EATTServerIncomingClientFindByTypeValueRequestIndication_FSCI_ID, Load_EATTServerIncomingClientFindByTypeValueRequestIndication},
	{EATTClientIncomingServerFindByTypeValueResponseIndication_FSCI_ID, Load_EATTClientIncomingServerFindByTypeValueResponseIndication},
	{EATTServerIncomingClientReadByTypeRequestIndication_FSCI_ID, Load_EATTServerIncomingClientReadByTypeRequestIndication},
	{EATTClientIncomingServerReadByTypeResponseIndication_FSCI_ID, Load_EATTClientIncomingServerReadByTypeResponseIndication},
	{EATTServerIncomingClientReadRequestIndication_FSCI_ID, Load_EATTServerIncomingClientReadRequestIndication},
	{EATTClientIncomingServerReadResponseIndication_FSCI_ID, Load_EATTClientIncomingServerReadResponseIndication},
	{EATTServerIncomingClientReadBlobRequestIndication_FSCI_ID, Load_EATTServerIncomingClientReadBlobRequestIndication},
	{EATTClientIncomingServerReadBlobResponseIndication_FSCI_ID, Load_EATTClientIncomingServerReadBlobResponseIndication},
	{EATTServerIncomingClientReadMultipleRequestIndication_FSCI_ID, Load_EATTServerIncomingClientReadMultipleRequestIndication},
	{EATTClientIncomingServerReadMultipleResponseIndication_FSCI_ID, Load_EATTClientIncomingServerReadMultipleResponseIndication},
	{EATTServerIncomingClientReadByGroupTypeRequestIndication_FSCI_ID, Load_EATTServerIncomingClientReadByGroupTypeRequestIndication},
	{EATTClientIncomingServerReadByGroupTypeResponseIndication_FSCI_ID, Load_EATTClientIncomingServerReadByGroupTypeResponseIndication},
	{EATTServerIncomingClientWriteRequestIndication_FSCI_ID, Load_EATTServerIncomingClientWriteRequestIndication},
	{EATTClientIncomingServerWriteResponseIndication_FSCI_ID, Load_EATTClientIncomingServerWriteResponseIndication},
	{EATTServerIncomingClientWriteCommandIndication_FSCI_ID, Load_EATTServerIncomingClientWriteCommandIndication},
	{EATTServerIncomingClientPrepareWriteRequestIndication_FSCI_ID, Load_EATTServerIncomingClientPrepareWriteRequestIndication},
	{EATTClientIncomingServerPrepareWriteResponseIndication_FSCI_ID, Load_EATTClientIncomingServerPrepareWriteResponseIndication},
	{EATTServerIncomingClientExecuteWriteRequestIndication_FSCI_ID, Load_EATTServerIncomingClientExecuteWriteRequestIndication},
	{EATTClientIncomingServerExecuteWriteResponseIndication_FSCI_ID, Load_EATTClientIncomingServerExecuteWriteResponseIndication},
	{EATTClientIncomingServerHandleValueNotificationIndication_FSCI_ID, Load_EATTClientIncomingServerHandleValueNotificationIndication},
	{EATTClientIncomingServerHandleValueIndicationIndication_FSCI_ID, Load_EATTClientIncomingServerHandleValueIndicationIndication},
	{EATTServerIncomingClientHandleValueConfirmationIndication_FSCI_ID, Load_EATTServerIncomingClientHandleValueConfirmationIndication},
	{EATTUnsupportedOpcodeIndication_FSCI_ID, Load_EATTUnsupportedOpcodeIndication},
	{EATTTimeoutIndication_FSCI_ID, Load_EATTTimeoutIndication},
	{EATTInvalidPduReceivedIndication_FSCI_ID, Load_EATTInvalidPduReceivedIndication},
	{EATTClientIncomingServerReadMultipleVariableResponseIndication_FSCI_ID, Load_EATTClientIncomingServerReadMultipleVariableResponseIndication},
	{EATTServerIncomingClientReadMultipleVariableRequestIndication_FSCI_ID, Load_EATTServerIncomingClientReadMultipleVariableRequestIndication},
	{EATTClientIncomingServerMultipleHandleValueNotificationIndication_FSCI_ID, Load_EATTClientIncomingServerMultipleHandleValueNotificationIndication},
#endif  /* ATT_ENABLE */

#if GATT_ENABLE
	{GATTConfirm_FSCI_ID, Load_GATTConfirm},
	{GATTGetMtuIndication_FSCI_ID, Load_GATTGetMtuIndication},
	{GATTClientProcedureExchangeMtuIndication_FSCI_ID, Load_GATTClientProcedureExchangeMtuIndication},
	{GATTClientProcedureDiscoverAllPrimaryServicesIndication_FSCI_ID, Load_GATTClientProcedureDiscoverAllPrimaryServicesIndication},
	{GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_FSCI_ID, Load_GATTClientProcedureDiscoverPrimaryServicesByUuidIndication},
	{GATTClientProcedureFindIncludedServicesIndication_FSCI_ID, Load_GATTClientProcedureFindIncludedServicesIndication},
	{GATTClientProcedureDiscoverAllCharacteristicsIndication_FSCI_ID, Load_GATTClientProcedureDiscoverAllCharacteristicsIndication},
	{GATTClientProcedureDiscoverCharacteristicByUuidIndication_FSCI_ID, Load_GATTClientProcedureDiscoverCharacteristicByUuidIndication},
	{GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_FSCI_ID, Load_GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication},
	{GATTClientProcedureReadCharacteristicValueIndication_FSCI_ID, Load_GATTClientProcedureReadCharacteristicValueIndication},
	{GATTClientProcedureReadUsingCharacteristicUuidIndication_FSCI_ID, Load_GATTClientProcedureReadUsingCharacteristicUuidIndication},
	{GATTClientProcedureReadMultipleCharacteristicValuesIndication_FSCI_ID, Load_GATTClientProcedureReadMultipleCharacteristicValuesIndication},
	{GATTClientProcedureWriteCharacteristicValueIndication_FSCI_ID, Load_GATTClientProcedureWriteCharacteristicValueIndication},
	{GATTClientProcedureReadCharacteristicDescriptorIndication_FSCI_ID, Load_GATTClientProcedureReadCharacteristicDescriptorIndication},
	{GATTClientProcedureWriteCharacteristicDescriptorIndication_FSCI_ID, Load_GATTClientProcedureWriteCharacteristicDescriptorIndication},
	{GATTClientNotificationIndication_FSCI_ID, Load_GATTClientNotificationIndication},
	{GATTClientMultipleHandleValueNotificationIndication_FSCI_ID, Load_GATTClientMultipleHandleValueNotificationIndication},
	{GATTClientProcedureReadMultipleVariableLenCharValuesIndication_FSCI_ID, Load_GATTClientProcedureReadMultipleVariableLenCharValuesIndication},
	{GATTClientIndicationIndication_FSCI_ID, Load_GATTClientIndicationIndication},
	{GATTServerMtuChangedIndication_FSCI_ID, Load_GATTServerMtuChangedIndication},
	{GATTServerHandleValueConfirmationIndication_FSCI_ID, Load_GATTServerHandleValueConfirmationIndication},
	{GATTServerAttributeWrittenIndication_FSCI_ID, Load_GATTServerAttributeWrittenIndication},
	{GATTServerCharacteristicCccdWrittenIndication_FSCI_ID, Load_GATTServerCharacteristicCccdWrittenIndication},
	{GATTServerAttributeWrittenWithoutResponseIndication_FSCI_ID, Load_GATTServerAttributeWrittenWithoutResponseIndication},
	{GATTServerErrorIndication_FSCI_ID, Load_GATTServerErrorIndication},
	{GATTServerLongCharacteristicWrittenIndication_FSCI_ID, Load_GATTServerLongCharacteristicWrittenIndication},
	{GATTServerAttributeReadIndication_FSCI_ID, Load_GATTServerAttributeReadIndication},
	{GATTClientProcedureEnhancedDiscoverAllPrimaryServicesIndication_FSCI_ID, Load_GATTClientProcedureEnhancedDiscoverAllPrimaryServicesIndication},
	{GATTClientProcedureEnhancedDiscoverPrimaryServicesByUuidIndication_FSCI_ID, Load_GATTClientProcedureEnhancedDiscoverPrimaryServicesByUuidIndication},
	{GATTClientProcedureEnhancedFindIncludedServicesIndication_FSCI_ID, Load_GATTClientProcedureEnhancedFindIncludedServicesIndication},
	{GATTClientProcedureEnhancedDiscoverAllCharacteristicsIndication_FSCI_ID, Load_GATTClientProcedureEnhancedDiscoverAllCharacteristicsIndication},
	{GATTClientProcedureEnhancedDiscoverCharacteristicByUuidIndication_FSCI_ID, Load_GATTClientProcedureEnhancedDiscoverCharacteristicByUuidIndication},
	{GATTClientProcedureEnhancedDiscoverAllCharacteristicDescriptorsIndication_FSCI_ID, Load_GATTClientProcedureEnhancedDiscoverAllCharacteristicDescriptorsIndication},
	{GATTClientProcedureEnhancedReadCharacteristicValueIndication_FSCI_ID, Load_GATTClientProcedureEnhancedReadCharacteristicValueIndication},
	{GATTClientProcedureEnhancedReadUsingCharacteristicUuidIndication_FSCI_ID, Load_GATTClientProcedureEnhancedReadUsingCharacteristicUuidIndication},
	{GATTClientProcedureEnhancedReadMultipleCharacteristicValuesIndication_FSCI_ID, Load_GATTClientProcedureEnhancedReadMultipleCharacteristicValuesIndication},
	{GATTClientProcedureEnhancedWriteCharacteristicValueIndication_FSCI_ID, Load_GATTClientProcedureEnhancedWriteCharacteristicValueIndication},
	{GATTClientProcedureEnhancedReadCharacteristicDescriptorIndication_FSCI_ID, Load_GATTClientProcedureEnhancedReadCharacteristicDescriptorIndication},
	{GATTClientProcedureEnhancedWriteCharacteristicDescriptorIndication_FSCI_ID, Load_GATTClientProcedureEnhancedWriteCharacteristicDescriptorIndication},
	{GATTClientProcedureEnhancedReadMultipleVariableLenCharValuesIndication_FSCI_ID, Load_GATTClientProcedureEnhancedReadMultipleVariableLenCharValuesIndication},
	{GATTClientEnhancedMultipleHandleValueNotificationIndication_FSCI_ID, Load_GATTClientEnhancedMultipleHandleValueNotificationIndication},
	{GATTClientEnhancedNotificationIndication_FSCI_ID, Load_GATTClientEnhancedNotificationIndication},
	{GATTClientEnhancedIndicationIndication_FSCI_ID, Load_GATTClientEnhancedIndicationIndication},
	{GATTServerEnhancedHandleValueConfirmationIndication_FSCI_ID, Load_GATTServerEnhancedHandleValueConfirmationIndication},
	{GATTServerEnhancedAttributeWrittenIndication_FSCI_ID, Load_GATTServerEnhancedAttributeWrittenIndication},
	{GATTServerEnhancedCharacteristicCccdWrittenIndication_FSCI_ID, Load_GATTServerEnhancedCharacteristicCccdWrittenIndication},
	{GATTServerEnhancedAttributeWrittenWithoutResponseIndication_FSCI_ID, Load_GATTServerEnhancedAttributeWrittenWithoutResponseIndication},
	{GATTServerEnhancedErrorIndication_FSCI_ID, Load_GATTServerEnhancedErrorIndication},
	{GATTServerEnhancedLongCharacteristicWrittenIndication_FSCI_ID, Load_GATTServerEnhancedLongCharacteristicWrittenIndication},
	{GATTServerEnhancedAttributeReadIndication_FSCI_ID, Load_GATTServerEnhancedAttributeReadIndication},
#endif  /* GATT_ENABLE */

#if GATTDB_APP_ENABLE
	{GATTDBConfirm_FSCI_ID, Load_GATTDBConfirm},
	{GATTDBReadAttributeIndication_FSCI_ID, Load_GATTDBReadAttributeIndication},
	{GATTDBFindServiceHandleIndication_FSCI_ID, Load_GATTDBFindServiceHandleIndication},
	{GATTDBFindCharValueHandleInServiceIndication_FSCI_ID, Load_GATTDBFindCharValueHandleInServiceIndication},
	{GATTDBFindCccdHandleForCharValueHandleIndication_FSCI_ID, Load_GATTDBFindCccdHandleForCharValueHandleIndication},
	{GATTDBFindDescriptorHandleForCharValueHandleIndication_FSCI_ID, Load_GATTDBFindDescriptorHandleForCharValueHandleIndication},
	{GATTDBDynamicAddPrimaryServiceDeclarationIndication_FSCI_ID, Load_GATTDBDynamicAddPrimaryServiceDeclarationIndication},
	{GATTDBDynamicAddSecondaryServiceDeclarationIndication_FSCI_ID, Load_GATTDBDynamicAddSecondaryServiceDeclarationIndication},
	{GATTDBDynamicAddIncludeDeclarationIndication_FSCI_ID, Load_GATTDBDynamicAddIncludeDeclarationIndication},
	{GATTDBDynamicAddCharacteristicDeclarationAndValueIndication_FSCI_ID, Load_GATTDBDynamicAddCharacteristicDeclarationAndValueIndication},
	{GATTDBDynamicAddCharacteristicDescriptorIndication_FSCI_ID, Load_GATTDBDynamicAddCharacteristicDescriptorIndication},
	{GATTDBDynamicAddCccdIndication_FSCI_ID, Load_GATTDBDynamicAddCccdIndication},
	{GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication_FSCI_ID, Load_GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication},
	{GATTDBDynamicAddCharDescriptorWithUniqueValueIndication_FSCI_ID, Load_GATTDBDynamicAddCharDescriptorWithUniqueValueIndication},
	{GATTDBDynamicAddCharAggregateFormatIndication_FSCI_ID, Load_GATTDBDynamicAddCharAggregateFormatIndication},
#endif  /* GATTDB_APP_ENABLE */

#if GATTDB_ATT_ENABLE
	{GATTDBAttConfirm_FSCI_ID, Load_GATTDBAttConfirm},
	{GATTDBAttFindInformationIndication_FSCI_ID, Load_GATTDBAttFindInformationIndication},
	{GATTDBAttFindByTypeValueIndication_FSCI_ID, Load_GATTDBAttFindByTypeValueIndication},
	{GATTDBAttReadByTypeIndication_FSCI_ID, Load_GATTDBAttReadByTypeIndication},
	{GATTDBAttReadIndication_FSCI_ID, Load_GATTDBAttReadIndication},
	{GATTDBAttReadBlobIndication_FSCI_ID, Load_GATTDBAttReadBlobIndication},
	{GATTDBAttReadMultipleIndication_FSCI_ID, Load_GATTDBAttReadMultipleIndication},
	{GATTDBAttReadByGroupTypeIndication_FSCI_ID, Load_GATTDBAttReadByGroupTypeIndication},
	{GATTDBAttWriteIndication_FSCI_ID, Load_GATTDBAttWriteIndication},
	{GATTDBAttPrepareWriteIndication_FSCI_ID, Load_GATTDBAttPrepareWriteIndication},
	{GATTDBAttExecuteWriteIndication_FSCI_ID, Load_GATTDBAttExecuteWriteIndication},
	{GATTDBAttExecuteWriteFromQueueIndication_FSCI_ID, Load_GATTDBAttExecuteWriteFromQueueIndication},
	{GATTDBAttPrepareNotificationIndicationIndication_FSCI_ID, Load_GATTDBAttPrepareNotificationIndicationIndication},
#endif  /* GATTDB_ATT_ENABLE */

#if GAP_ENABLE
	{GAPConfirm_FSCI_ID, Load_GAPConfirm},
	{GAPCheckNotificationStatusIndication_FSCI_ID, Load_GAPCheckNotificationStatusIndication},
	{GAPCheckIndicationStatusIndication_FSCI_ID, Load_GAPCheckIndicationStatusIndication},
	{GAPLoadKeysIndication_FSCI_ID, Load_GAPLoadKeysIndication},
	{GAPLoadEncryptionInformationIndication_FSCI_ID, Load_GAPLoadEncryptionInformationIndication},
	{GAPLoadCustomPeerInformationIndication_FSCI_ID, Load_GAPLoadCustomPeerInformationIndication},
	{GAPCheckIfBondedIndication_FSCI_ID, Load_GAPCheckIfBondedIndication},
	{GAPGetBondedDevicesCountIndication_FSCI_ID, Load_GAPGetBondedDevicesCountIndication},
	{GAPGetBondedDeviceNameIndication_FSCI_ID, Load_GAPGetBondedDeviceNameIndication},
	{GAPGenericEventInitializationCompleteIndication_FSCI_ID, Load_GAPGenericEventInitializationCompleteIndication},
	{GAPGenericEventInternalErrorIndication_FSCI_ID, Load_GAPGenericEventInternalErrorIndication},
	{GAPGenericEventAdvertisingSetupFailedIndication_FSCI_ID, Load_GAPGenericEventAdvertisingSetupFailedIndication},
	{GAPGenericEventAdvertisingParametersSetupCompleteIndication_FSCI_ID, Load_GAPGenericEventAdvertisingParametersSetupCompleteIndication},
	{GAPGenericEventAdvertisingDataSetupCompleteIndication_FSCI_ID, Load_GAPGenericEventAdvertisingDataSetupCompleteIndication},
	{GAPGenericEventFilterAcceptListSizeReadIndication_FSCI_ID, Load_GAPGenericEventFilterAcceptListSizeReadIndication},
	{GAPGenericEventDeviceAddedToFilterAcceptListIndication_FSCI_ID, Load_GAPGenericEventDeviceAddedToFilterAcceptListIndication},
	{GAPGenericEventDeviceRemovedFromFilterAcceptListIndication_FSCI_ID, Load_GAPGenericEventDeviceRemovedFromFilterAcceptListIndication},
	{GAPGenericEventFilterAcceptListClearedIndication_FSCI_ID, Load_GAPGenericEventFilterAcceptListClearedIndication},
	{GAPGenericEventRandomAddressReadyIndication_FSCI_ID, Load_GAPGenericEventRandomAddressReadyIndication},
	{GAPGenericEventCreateConnectionCanceledIndication_FSCI_ID, Load_GAPGenericEventCreateConnectionCanceledIndication},
	{GAPGenericEventPublicAddressReadIndication_FSCI_ID, Load_GAPGenericEventPublicAddressReadIndication},
	{GAPGenericEventAdvTxPowerLevelReadIndication_FSCI_ID, Load_GAPGenericEventAdvTxPowerLevelReadIndication},
	{GAPGenericEventPrivateResolvableAddressVerifiedIndication_FSCI_ID, Load_GAPGenericEventPrivateResolvableAddressVerifiedIndication},
	{GAPGenericEventRandomAddressSetIndication_FSCI_ID, Load_GAPGenericEventRandomAddressSetIndication},
	{GAPAdvertisingEventStateChangedIndication_FSCI_ID, Load_GAPAdvertisingEventStateChangedIndication},
	{GAPAdvertisingEventCommandFailedIndication_FSCI_ID, Load_GAPAdvertisingEventCommandFailedIndication},
	{GAPScanningEventStateChangedIndication_FSCI_ID, Load_GAPScanningEventStateChangedIndication},
	{GAPScanningEventCommandFailedIndication_FSCI_ID, Load_GAPScanningEventCommandFailedIndication},
	{GAPScanningEventDeviceScannedIndication_FSCI_ID, Load_GAPScanningEventDeviceScannedIndication},
	{GAPConnectionEventConnectedIndication_FSCI_ID, Load_GAPConnectionEventConnectedIndication},
	{GAPConnectionEventPairingRequestIndication_FSCI_ID, Load_GAPConnectionEventPairingRequestIndication},
	{GAPConnectionEventPeripheralSecurityRequestIndication_FSCI_ID, Load_GAPConnectionEventPeripheralSecurityRequestIndication},
	{GAPConnectionEventPairingResponseIndication_FSCI_ID, Load_GAPConnectionEventPairingResponseIndication},
	{GAPConnectionEventAuthenticationRejectedIndication_FSCI_ID, Load_GAPConnectionEventAuthenticationRejectedIndication},
	{GAPConnectionEventPasskeyRequestIndication_FSCI_ID, Load_GAPConnectionEventPasskeyRequestIndication},
	{GAPConnectionEventOobRequestIndication_FSCI_ID, Load_GAPConnectionEventOobRequestIndication},
	{GAPConnectionEventPasskeyDisplayIndication_FSCI_ID, Load_GAPConnectionEventPasskeyDisplayIndication},
	{GAPConnectionEventKeyExchangeRequestIndication_FSCI_ID, Load_GAPConnectionEventKeyExchangeRequestIndication},
	{GAPConnectionEventKeysReceivedIndication_FSCI_ID, Load_GAPConnectionEventKeysReceivedIndication},
	{GAPConnectionEventLongTermKeyRequestIndication_FSCI_ID, Load_GAPConnectionEventLongTermKeyRequestIndication},
	{GAPConnectionEventEncryptionChangedIndication_FSCI_ID, Load_GAPConnectionEventEncryptionChangedIndication},
	{GAPConnectionEventPairingCompleteIndication_FSCI_ID, Load_GAPConnectionEventPairingCompleteIndication},
	{GAPConnectionEventDisconnectedIndication_FSCI_ID, Load_GAPConnectionEventDisconnectedIndication},
	{GAPConnectionEventRssiReadIndication_FSCI_ID, Load_GAPConnectionEventRssiReadIndication},
	{GAPConnectionEventTxPowerLevelReadIndication_FSCI_ID, Load_GAPConnectionEventTxPowerLevelReadIndication},
	{GAPConnectionEventPowerReadFailureIndication_FSCI_ID, Load_GAPConnectionEventPowerReadFailureIndication},
	{GAPConnectionEventParameterUpdateRequestIndication_FSCI_ID, Load_GAPConnectionEventParameterUpdateRequestIndication},
	{GAPConnectionEventParameterUpdateCompleteIndication_FSCI_ID, Load_GAPConnectionEventParameterUpdateCompleteIndication},
	{GAPConnectionEventLeDataLengthChangedIndication_FSCI_ID, Load_GAPConnectionEventLeDataLengthChangedIndication},
	{GAPConnectionEventLeSetDataLengthFailedIndication_FSCI_ID, Load_GAPConnectionEventLeSetDataLengthFailedIndication},
	{GAPConnectionEventLeScOobDataRequestIndication_FSCI_ID, Load_GAPConnectionEventLeScOobDataRequestIndication},
	{GAPConnectionEventLeScDisplayNumericValueIndication_FSCI_ID, Load_GAPConnectionEventLeScDisplayNumericValueIndication},
	{GAPConnectionEventLeScKeypressNotificationIndication_FSCI_ID, Load_GAPConnectionEventLeScKeypressNotificationIndication},
	{GAPLeScPublicKeyRegeneratedIndication_FSCI_ID, Load_GAPLeScPublicKeyRegeneratedIndication},
	{GAPGenericEventLeScLocalOobDataIndication_FSCI_ID, Load_GAPGenericEventLeScLocalOobDataIndication},
	{GAPGenericEventHostPrivacyStateChangedIndication_FSCI_ID, Load_GAPGenericEventHostPrivacyStateChangedIndication},
	{GAPGenericEventControllerPrivacyStateChangedIndication_FSCI_ID, Load_GAPGenericEventControllerPrivacyStateChangedIndication},
	{GAPGenericEventTxPowerLevelSetCompleteIndication_FSCI_ID, Load_GAPGenericEventTxPowerLevelSetCompleteIndication},
	{GAPGenericEventLePhyEventIndication_FSCI_ID, Load_GAPGenericEventLePhyEventIndication},
	{GAPGetBondedDevicesIdentityInformationIndication_FSCI_ID, Load_GAPGetBondedDevicesIdentityInformationIndication},
	{GAPControllerNotificationIndication_FSCI_ID, Load_GAPControllerNotificationIndication},
	{GAPBondCreatedIndication_FSCI_ID, Load_GAPBondCreatedIndication},
	{GAPGenericEventChannelMapSetIndication_FSCI_ID, Load_GAPGenericEventChannelMapSetIndication},
	{GAPConnectionEventChannelMapReadIndication_FSCI_ID, Load_GAPConnectionEventChannelMapReadIndication},
	{GAPConnectionEventChannelMapReadFailureIndication_FSCI_ID, Load_GAPConnectionEventChannelMapReadFailureIndication},
	{GAPGenericEventExtAdvertisingParamSetupCompleteIndication_FSCI_ID, Load_GAPGenericEventExtAdvertisingParamSetupCompleteIndication},
	{GAPGenericEventExtAdvertisingDataSetupCompleteIndication_FSCI_ID, Load_GAPGenericEventExtAdvertisingDataSetupCompleteIndication},
	{GAPGenericEventPeriodicAdvParamSetupCompleteIndication_FSCI_ID, Load_GAPGenericEventPeriodicAdvParamSetupCompleteIndication},
	{GAPGenericEventPeriodicAdvDataSetupCompleteIndication_FSCI_ID, Load_GAPGenericEventPeriodicAdvDataSetupCompleteIndication},
	{GAPGenericEventPeriodicAdvListUpdateCompleteIndication_FSCI_ID, Load_GAPGenericEventPeriodicAdvListUpdateCompleteIndication},
	{GAPAdvertisingEventExtAdvertisingStateChangedIndication_FSCI_ID, Load_GAPAdvertisingEventExtAdvertisingStateChangedIndication},
	{GAPAdvertisingEventAdvertisingSetTerminatedIndication_FSCI_ID, Load_GAPAdvertisingEventAdvertisingSetTerminatedIndication},
	{GAPAdvertisingEventExtAdvertisingSetRemoveCompletedIndication_FSCI_ID, Load_GAPAdvertisingEventExtAdvertisingSetRemoveCompletedIndication},
	{GAPAdvertisingEventExtScanReqReceivedIndication_FSCI_ID, Load_GAPAdvertisingEventExtScanReqReceivedIndication},
	{GAPGenericEventPeriodicAdvertisingStateChangedIndication_FSCI_ID, Load_GAPGenericEventPeriodicAdvertisingStateChangedIndication},
	{GAPScanningEventExtDeviceScannedIndication_FSCI_ID, Load_GAPScanningEventExtDeviceScannedIndication},
	{GAPScanningEventPeriodicAdvSyncEstablishedIndication_FSCI_ID, Load_GAPScanningEventPeriodicAdvSyncEstablishedIndication},
	{GAPScanningEventPeriodicAdvSyncTerminatedIndication_FSCI_ID, Load_GAPScanningEventPeriodicAdvSyncTerminatedIndication},
	{GAPScanningEventPeriodicAdvSyncLostIndication_FSCI_ID, Load_GAPScanningEventPeriodicAdvSyncLostIndication},
	{GAPScanningEventPeriodicDeviceScannedIndication_FSCI_ID, Load_GAPScanningEventPeriodicDeviceScannedIndication},
	{GAPGenericEventPeriodicAdvCreateSyncCancelledIndication_FSCI_ID, Load_GAPGenericEventPeriodicAdvCreateSyncCancelledIndication},
	{GAPConnectionEventChannelSelectionAlgorithm2Indication_FSCI_ID, Load_GAPConnectionEventChannelSelectionAlgorithm2Indication},
	{GAPGenericEventTxEntryAvailableIndication_FSCI_ID, Load_GAPGenericEventTxEntryAvailableIndication},
	{GAPGenericEventControllerLocalRPAReadIndication_FSCI_ID, Load_GAPGenericEventControllerLocalRPAReadIndication},
	{GAPCheckNvmIndexIndication_FSCI_ID, Load_GAPCheckNvmIndexIndication},
	{GAPGetDeviceIdFromConnHandleIndication_FSCI_ID, Load_GAPGetDeviceIdFromConnHandleIndication},
	{GAPGetConnectionHandleFromDeviceIdIndication_FSCI_ID, Load_GAPGetConnectionHandleFromDeviceIdIndication},
	{GAPPairingEventNoLTKIndication_FSCI_ID, Load_GAPPairingEventNoLTKIndication},
	{GAPPairingAlreadyStartedIndication_FSCI_ID, Load_GAPPairingAlreadyStartedIndication},
	{GAPGenericEventConnectionlessCteTransmitParamsSetupCompleteIndication_FSCI_ID, Load_GAPGenericEventConnectionlessCteTransmitParamsSetupCompleteIndication},
	{GAPGenericEventConnectionlessCteTransmitStateChangedIndication_FSCI_ID, Load_GAPGenericEventConnectionlessCteTransmitStateChangedIndication},
	{GAPGenericEventConnectionlessIqSamplingStateChangedIndication_FSCI_ID, Load_GAPGenericEventConnectionlessIqSamplingStateChangedIndication},
	{GAPGenericEventAntennaInformationReadIndication_FSCI_ID, Load_GAPGenericEventAntennaInformationReadIndication},
	{GAPScanningEventConnectionlessIqReportReceivedIndication_FSCI_ID, Load_GAPScanningEventConnectionlessIqReportReceivedIndication},
	{GAPConnectionEventIqReportReceivedIndication_FSCI_ID, Load_GAPConnectionEventIqReportReceivedIndication},
	{GAPConnectionEventCteRequestFailedIndication_FSCI_ID, Load_GAPConnectionEventCteRequestFailedIndication},
	{GAPConnectionEventCteReceiveParamsSetupCompleteIndication_FSCI_ID, Load_GAPConnectionEventCteReceiveParamsSetupCompleteIndication},
	{GAPConnectionEventCteTransmitParamsSetupCompleteIndication_FSCI_ID, Load_GAPConnectionEventCteTransmitParamsSetupCompleteIndication},
	{GAPConnectionEventCteReqStateChangedIndication_FSCI_ID, Load_GAPConnectionEventCteReqStateChangedIndication},
	{GAPConnectionEventCteRspStateChangedIndication_FSCI_ID, Load_GAPConnectionEventCteRspStateChangedIndication},
	{GAPGenericEventPeriodicAdvRecvEnableCompleteIndication_FSCI_ID, Load_GAPGenericEventPeriodicAdvRecvEnableCompleteIndication},
	{GAPGenericEventPeriodicAdvSyncTransferCompleteIndication_FSCI_ID, Load_GAPGenericEventPeriodicAdvSyncTransferCompleteIndication},
	{GAPGenericEventPeriodicAdvSetInfoTransferCompleteIndication_FSCI_ID, Load_GAPGenericEventPeriodicAdvSetInfoTransferCompleteIndication},
	{GAPGenericEventSetPeriodicAdvSyncTransferParamsCompleteIndication_FSCI_ID, Load_GAPGenericEventSetPeriodicAdvSyncTransferParamsCompleteIndication},
	{GAPGenericEventSetDefaultPeriodicAdvSyncTransferParamsCompleteIndication_FSCI_ID, Load_GAPGenericEventSetDefaultPeriodicAdvSyncTransferParamsCompleteIndication},
	{GAPScanningEventPeriodicAdvSyncTransferReceivedIndication_FSCI_ID, Load_GAPScanningEventPeriodicAdvSyncTransferReceivedIndication},
	{GAPConnectionEventPathLossThresholdIndication_FSCI_ID, Load_GAPConnectionEventPathLossThresholdIndication},
	{GAPConnectionEventTransmitPowerReportingIndication_FSCI_ID, Load_GAPConnectionEventTransmitPowerReportingIndication},
	{GAPConnectionEventEnhancedReadTransmitPowerLevelIndication_FSCI_ID, Load_GAPConnectionEventEnhancedReadTransmitPowerLevelIndication},
	{GAPConnectionEventPathLossReportingParamsSetupCompleteIndication_FSCI_ID, Load_GAPConnectionEventPathLossReportingParamsSetupCompleteIndication},
	{GAPConnectionEventPathLossReportingStateChangedIndication_FSCI_ID, Load_GAPConnectionEventPathLossReportingStateChangedIndication},
	{GAPConnectionEventTransmitPowerReportingStateChangedIndication_FSCI_ID, Load_GAPConnectionEventTransmitPowerReportingStateChangedIndication},
	{GAPConnectionEventEattConnectionRequestIndication_FSCI_ID, Load_GAPConnectionEventEattConnectionRequestIndication},
	{GAPConnectionEventEattConnectionCompleteIndication_FSCI_ID, Load_GAPConnectionEventEattConnectionCompleteIndication},
	{GAPConnectionEventEattReconfigureResponseIndication_FSCI_ID, Load_GAPConnectionEventEattReconfigureResponseIndication},
	{GAPConnectionEventEattBearerStatusNotificationIndication_FSCI_ID, Load_GAPConnectionEventEattBearerStatusNotificationIndication},
	{GAPGenericEventLeGenerateDhKeyCompleteIndication_FSCI_ID, Load_GAPGenericEventLeGenerateDhKeyCompleteIndication},
	{GAPGetHostVersionIndication_FSCI_ID, Load_GAPGetHostVersionIndication},
	{GAPGenericEventReadRemoteVersionInfoCompleteIndication_FSCI_ID, Load_GAPGenericEventReadRemoteVersionInfoCompleteIndication},
	{GAPGetConnParamsIndication_FSCI_ID, Load_GAPGetConnParamsIndication},
	{GAPPlatformErrorIndication_FSCI_ID, Load_GAPPlatformErrorIndication},
#endif  /* GAP_ENABLE */

#if APP_ENABLE
	{APPThroughputTestIndication_FSCI_ID, Load_APPThroughputTestIndication},
	{APPThroughputTestConfirm_FSCI_ID, Load_APPThroughputTestConfirm},
#endif  /* APP_ENABLE */

#if CS_ENABLE
	{CSConfirm_FSCI_ID, Load_CSConfirm},
	{CSReadRemoteSupportedCapabilitiesIndication_FSCI_ID, Load_CSReadRemoteSupportedCapabilitiesIndication},
	{CSReadRemoteFAETableIndication_FSCI_ID, Load_CSReadRemoteFAETableIndication},
	{CSSecurityEnableIndication_FSCI_ID, Load_CSSecurityEnableIndication},
	{CSConfigCompleteIndication_FSCI_ID, Load_CSConfigCompleteIndication},
	{CSProcedureEnableCompleteIndication_FSCI_ID, Load_CSProcedureEnableCompleteIndication},
	{CSSubeventResultIndication_FSCI_ID, Load_CSSubeventResultIndication},
	{CSSubeventResultContinueIndication_FSCI_ID, Load_CSSubeventResultContinueIndication},
	{CSTestEndEvtIndication_FSCI_ID, Load_CSTestEndEvtIndication},
	{CSEventResultDebugIndication_FSCI_ID, Load_CSEventResultDebugIndication},
	{CSLEMetaErrorIndication_FSCI_ID, Load_CSLEMetaErrorIndication},
	{CSReadLocalSupportedCapabilitiesIndication_FSCI_ID, Load_CSReadLocalSupportedCapabilitiesIndication},
	{CSSetDefaultSettingsIndication_FSCI_ID, Load_CSSetDefaultSettingsIndication},
	{CSWriteRemoteFAETableIndication_FSCI_ID, Load_CSWriteRemoteFAETableIndication},
	{CSSetChannelClassificationIndication_FSCI_ID, Load_CSSetChannelClassificationIndication},
	{CSgCSEventTestCmdOpCodeIndication_FSCI_ID, Load_CSgCSEventTestCmdOpCodeIndication},
	{CSgCSEventSetProcedureParamsCmdOpCodeIndication_FSCI_ID, Load_CSgCSEventSetProcedureParamsCmdOpCodeIndication},
	{CSErrorIndication_FSCI_ID, Load_CSErrorIndication},
	{CSgCsEventWriteCachedRemoteCapabilitiesOpCodeIndication_FSCI_ID, Load_CSgCsEventWriteCachedRemoteCapabilitiesOpCodeIndication},
	{CSCommandStatusIndication_FSCI_ID, Load_CSCommandStatusIndication},
#endif  /* CS_ENABLE */

#if GAPHANDOVER_ENABLE
	{GapHandoverConfirm_FSCI_ID, Load_GapHandoverConfirm},
	{GapHandoverGetDataSizeIndication_FSCI_ID, Load_GapHandoverGetDataSizeIndication},
	{GapHandoverGetCompleteIndication_FSCI_ID, Load_GapHandoverGetCompleteIndication},
	{GapHandoverSetCompleteIndication_FSCI_ID, Load_GapHandoverSetCompleteIndication},
	{GapHandoverGetTimeIndication_FSCI_ID, Load_GapHandoverGetTimeIndication},
	{GapHandoverSuspendTransmitCompleteIndication_FSCI_ID, Load_GapHandoverSuspendTransmitCompleteIndication},
	{GapHandoverResumeTransmitCompleteIndication_FSCI_ID, Load_GapHandoverResumeTransmitCompleteIndication},
	{GapHandoverAnchorNotificationStateChangedIndication_FSCI_ID, Load_GapHandoverAnchorNotificationStateChangedIndication},
	{GapHandoverAnchorSearchStartedIndication_FSCI_ID, Load_GapHandoverAnchorSearchStartedIndication},
	{GapHandoverAnchorSearchStoppedIndication_FSCI_ID, Load_GapHandoverAnchorSearchStoppedIndication},
	{GapHandoverAnchorMonitorIndication_FSCI_ID, Load_GapHandoverAnchorMonitorIndication},
	{GapHandoverAnchorMonitorPacketIndication_FSCI_ID, Load_GapHandoverAnchorMonitorPacketIndication},
	{GapHandoverAnchorMonitorPacketContinueIndication_FSCI_ID, Load_GapHandoverAnchorMonitorPacketContinueIndication},
	{GapHandoverConnectedIndication_FSCI_ID, Load_GapHandoverConnectedIndication},
	{GapHandoverDisconnectedIndication_FSCI_ID, Load_GapHandoverDisconnectedIndication},
	{GapHandoverTimeSyncTransmitStateChangedIndication_FSCI_ID, Load_GapHandoverTimeSyncTransmitStateChangedIndication},
	{GapHandoverTimeSyncReceiveCompleteIndication_FSCI_ID, Load_GapHandoverTimeSyncReceiveCompleteIndication},
	{GapHandoverTimeSyncIndication_FSCI_ID, Load_GapHandoverTimeSyncIndication},
	{GapHandoverConnParamUpdateIndication_FSCI_ID, Load_GapHandoverConnParamUpdateIndication},
	{GapHandoverUpdateConnParamsCompleteIndication_FSCI_ID, Load_GapHandoverUpdateConnParamsCompleteIndication},
	{GapHandoverLlPendingDataIndication_FSCI_ID, Load_GapHandoverLlPendingDataIndication},
#endif  /* GAPHANDOVER_ENABLE */

#if GAP2_ENABLE
	{GAP2Confirm_FSCI_ID, Load_GAP2Confirm},
	{GAP2CtrlDebugInfoIndication_FSCI_ID, Load_GAP2CtrlDebugInfoIndication},
	{GAP2GenericEventSetExtAdvertisingDecisionDataSetupCompleteIndication_FSCI_ID, Load_GAP2GenericEventSetExtAdvertisingDecisionDataSetupCompleteIndication},
	{GAP2GenericEventSetDecisionInstructionsSetupCompleteIndication_FSCI_ID, Load_GAP2GenericEventSetDecisionInstructionsSetupCompleteIndication},
	{GAP2CtrlDebugInfo2Indication_FSCI_ID, Load_GAP2CtrlDebugInfo2Indication},
	{GAP2EcdhP256ComputeA2BKeyIndication_FSCI_ID, Load_GAP2EcdhP256ComputeA2BKeyIndication},
	{GAP2ExportA2BBlobIndication_FSCI_ID, Load_GAP2ExportA2BBlobIndication},
	{GAP2ImportA2BBlobIndication_FSCI_ID, Load_GAP2ImportA2BBlobIndication},
	{GAP2EcdhP256GenerateKeysIndication_FSCI_ID, Load_GAP2EcdhP256GenerateKeysIndication},
	{GAP2ChannelOverrideCompleteIndication_FSCI_ID, Load_GAP2ChannelOverrideCompleteIndication},
#endif  /* GAP2_ENABLE */
};

/*==================================================================================================
Public Functions
==================================================================================================*/
#if FSCI_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_FSCIMemAllocTestResponse(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI-MemAllocTest.Response description
***************************************************************************************************/
static mem_status_t Load_FSCIMemAllocTestResponse(bleEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIMemAllocTestResponse_t *evt = &(container->Data.FSCIMemAllocTestResponse);

	/* Store (OG, OC) in ID */
	container->id = FSCIMemAllocTestResponse_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIMemAllocTestResponse_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_FSCIGetNumberOfFreeBuffersResponse(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI-GetNumberOfFreeBuffers.Response description
***************************************************************************************************/
static mem_status_t Load_FSCIGetNumberOfFreeBuffersResponse(bleEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIGetNumberOfFreeBuffersResponse_t *evt = &(container->Data.FSCIGetNumberOfFreeBuffersResponse);

	/* Store (OG, OC) in ID */
	container->id = FSCIGetNumberOfFreeBuffersResponse_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIGetNumberOfFreeBuffersResponse_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_FSCIAckIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI packet sent as acknowledgment to a received FSCI packet.
***************************************************************************************************/
static mem_status_t Load_FSCIAckIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIAckIndication_t *evt = &(container->Data.FSCIAckIndication);

	/* Store (OG, OC) in ID */
	container->id = FSCIAckIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIAckIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_FSCIErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI is reporting an error condition.
***************************************************************************************************/
static mem_status_t Load_FSCIErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIErrorIndication_t *evt = &(container->Data.FSCIErrorIndication);

	/* Store (OG, OC) in ID */
	container->id = FSCIErrorIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIErrorIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_FSCIAllowDeviceToSleepConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI-AllowDeviceToSleep.Confirm description
***************************************************************************************************/
static mem_status_t Load_FSCIAllowDeviceToSleepConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIAllowDeviceToSleepConfirm_t *evt = &(container->Data.FSCIAllowDeviceToSleepConfirm);

	/* Store (OG, OC) in ID */
	container->id = FSCIAllowDeviceToSleepConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIAllowDeviceToSleepConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_FSCIWakeUpIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI-WakeUp.Indication description
***************************************************************************************************/
static mem_status_t Load_FSCIWakeUpIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIWakeUpIndication_t *evt = &(container->Data.FSCIWakeUpIndication);

	/* Store (OG, OC) in ID */
	container->id = FSCIWakeUpIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIWakeUpIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_FSCIGetWakeupReasonResponse(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI-GetWakeupReason.Response description
***************************************************************************************************/
static mem_status_t Load_FSCIGetWakeupReasonResponse(bleEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIGetWakeupReasonResponse_t *evt = &(container->Data.FSCIGetWakeupReasonResponse);

	/* Store (OG, OC) in ID */
	container->id = FSCIGetWakeupReasonResponse_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIGetWakeupReasonResponse_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_FSCIGetNbuVersionResponse(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI-GetNbuVersion.Response description
***************************************************************************************************/
static mem_status_t Load_FSCIGetNbuVersionResponse(bleEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIGetNbuVersionResponse_t *evt = &(container->Data.FSCIGetNbuVersionResponse);

	/* Store (OG, OC) in ID */
	container->id = FSCIGetNbuVersionResponse_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIGetNbuVersionResponse_t));

	return kStatus_MemSuccess;
}

#endif  /* FSCI_ENABLE */

#if NVM_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_NVMSaveConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for Updating NVM
***************************************************************************************************/
static mem_status_t Load_NVMSaveConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	NVMSaveConfirm_t *evt = &(container->Data.NVMSaveConfirm);

	/* Store (OG, OC) in ID */
	container->id = NVMSaveConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(NVMSaveConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_NVMGetDataSetDescConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Get the ID and size of each defined NV data set.
***************************************************************************************************/
static mem_status_t Load_NVMGetDataSetDescConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	NVMGetDataSetDescConfirm_t *evt = &(container->Data.NVMGetDataSetDescConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = NVMGetDataSetDescConfirm_FSCI_ID;

	evt->Status = pPayload[idx]; idx++;
	evt->Count = pPayload[idx]; idx++;

	if (evt->Count > 0)
	{
		evt->CountSizeandID = MEM_BufferAlloc(evt->Count * 7);

		if (!evt->CountSizeandID)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->CountSizeandID = NULL;
	}

	FLib_MemCpy(evt->CountSizeandID, pPayload + idx, evt->Count * 7); idx += evt->Count * 7;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_NVMGetCountersConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Get the counters of each NV page erased and dataset write
***************************************************************************************************/
static mem_status_t Load_NVMGetCountersConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	NVMGetCountersConfirm_t *evt = &(container->Data.NVMGetCountersConfirm);

	/* Store (OG, OC) in ID */
	container->id = NVMGetCountersConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(NVMGetCountersConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_NVMGetVPSizeResponse(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Get the NVM Virtual Page Size
***************************************************************************************************/
static mem_status_t Load_NVMGetVPSizeResponse(bleEvtContainer_t *container, uint8_t *pPayload)
{
	NVMGetVPSizeResponse_t *evt = &(container->Data.NVMGetVPSizeResponse);

	/* Store (OG, OC) in ID */
	container->id = NVMGetVPSizeResponse_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(NVMGetVPSizeResponse_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_NVMSetMonitoringConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Get the counters of each NV page erased and dataset write
***************************************************************************************************/
static mem_status_t Load_NVMSetMonitoringConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	NVMSetMonitoringConfirm_t *evt = &(container->Data.NVMSetMonitoringConfirm);

	/* Store (OG, OC) in ID */
	container->id = NVMSetMonitoringConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(NVMSetMonitoringConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_NVMWriteMonitoringIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Receive curent Id of nvm dataset write
***************************************************************************************************/
static mem_status_t Load_NVMWriteMonitoringIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	NVMWriteMonitoringIndication_t *evt = &(container->Data.NVMWriteMonitoringIndication);

	/* Store (OG, OC) in ID */
	container->id = NVMWriteMonitoringIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(NVMWriteMonitoringIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_NVMPageEraseMonitoringIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Receive curent address of nvm erased page
***************************************************************************************************/
static mem_status_t Load_NVMPageEraseMonitoringIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	NVMPageEraseMonitoringIndication_t *evt = &(container->Data.NVMPageEraseMonitoringIndication);

	/* Store (OG, OC) in ID */
	container->id = NVMPageEraseMonitoringIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(NVMPageEraseMonitoringIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_NVMFormatReqConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirm of the NV format request comand
***************************************************************************************************/
static mem_status_t Load_NVMFormatReqConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	NVMFormatReqConfirm_t *evt = &(container->Data.NVMFormatReqConfirm);

	/* Store (OG, OC) in ID */
	container->id = NVMFormatReqConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(NVMFormatReqConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_NVMRestoreReqConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirm of the NV format request comand
***************************************************************************************************/
static mem_status_t Load_NVMRestoreReqConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	NVMRestoreReqConfirm_t *evt = &(container->Data.NVMRestoreReqConfirm);

	/* Store (OG, OC) in ID */
	container->id = NVMRestoreReqConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(NVMRestoreReqConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_NVMRestoreMonitoringIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Indication that a nvm dataset was restored
***************************************************************************************************/
static mem_status_t Load_NVMRestoreMonitoringIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	NVMRestoreMonitoringIndication_t *evt = &(container->Data.NVMRestoreMonitoringIndication);

	/* Store (OG, OC) in ID */
	container->id = NVMRestoreMonitoringIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(NVMRestoreMonitoringIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_NVMVirtualPageMonitoringIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Indication that the commute to the next virtual page
***************************************************************************************************/
static mem_status_t Load_NVMVirtualPageMonitoringIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	NVMVirtualPageMonitoringIndication_t *evt = &(container->Data.NVMVirtualPageMonitoringIndication);

	/* Store (OG, OC) in ID */
	container->id = NVMVirtualPageMonitoringIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(NVMVirtualPageMonitoringIndication_t));

	return kStatus_MemSuccess;
}

#endif  /* NVM_ENABLE */

#if HCI_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_HCIConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the HCI request
***************************************************************************************************/
static mem_status_t Load_HCIConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	HCIConfirm_t *evt = &(container->Data.HCIConfirm);

	/* Store (OG, OC) in ID */
	container->id = HCIConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(HCIConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_HCIEventIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	HCI received event
***************************************************************************************************/
static mem_status_t Load_HCIEventIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	HCIEventIndication_t *evt = &(container->Data.HCIEventIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = HCIEventIndication_FSCI_ID;

	FLib_MemCpy(&(evt->EventLength), pPayload + idx, sizeof(evt->EventLength)); idx += sizeof(evt->EventLength);

	if (evt->EventLength > 0)
	{
		evt->Event = MEM_BufferAlloc(evt->EventLength);

		if (!evt->Event)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Event = NULL;
	}

	FLib_MemCpy(evt->Event, pPayload + idx, evt->EventLength); idx += evt->EventLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_HCIDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	HCI received data
***************************************************************************************************/
static mem_status_t Load_HCIDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	HCIDataIndication_t *evt = &(container->Data.HCIDataIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = HCIDataIndication_FSCI_ID;

	FLib_MemCpy(&(evt->DataLength), pPayload + idx, sizeof(evt->DataLength)); idx += sizeof(evt->DataLength);

	if (evt->DataLength > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->DataLength);

		if (!evt->Data)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->DataLength); idx += evt->DataLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_HCISynchronousDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	HCI received synchronous data
***************************************************************************************************/
static mem_status_t Load_HCISynchronousDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	HCISynchronousDataIndication_t *evt = &(container->Data.HCISynchronousDataIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = HCISynchronousDataIndication_FSCI_ID;

	FLib_MemCpy(&(evt->SynchronousDataLength), pPayload + idx, sizeof(evt->SynchronousDataLength)); idx += sizeof(evt->SynchronousDataLength);

	if (evt->SynchronousDataLength > 0)
	{
		evt->SynchronousData = MEM_BufferAlloc(evt->SynchronousDataLength);

		if (!evt->SynchronousData)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->SynchronousData = NULL;
	}

	FLib_MemCpy(evt->SynchronousData, pPayload + idx, evt->SynchronousDataLength); idx += evt->SynchronousDataLength;

	return kStatus_MemSuccess;
}

#endif  /* HCI_ENABLE */

#if L2CAP_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the L2CAP request
***************************************************************************************************/
static mem_status_t Load_L2CAPConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPConfirm_t *evt = &(container->Data.L2CAPConfirm);

	/* Store (OG, OC) in ID */
	container->id = L2CAPConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(L2CAPConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPAttDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Data packet received through ATT Channel
***************************************************************************************************/
static mem_status_t Load_L2CAPAttDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPAttDataIndication_t *evt = &(container->Data.L2CAPAttDataIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPAttDataIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->PacketLength), pPayload + idx, sizeof(evt->PacketLength)); idx += sizeof(evt->PacketLength);

	if (evt->PacketLength > 0)
	{
		evt->Packet = MEM_BufferAlloc(evt->PacketLength);

		if (!evt->Packet)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Packet = NULL;
	}

	FLib_MemCpy(evt->Packet, pPayload + idx, evt->PacketLength); idx += evt->PacketLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPSmpDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Data packet received through SM Channel
***************************************************************************************************/
static mem_status_t Load_L2CAPSmpDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPSmpDataIndication_t *evt = &(container->Data.L2CAPSmpDataIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPSmpDataIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->PacketLength), pPayload + idx, sizeof(evt->PacketLength)); idx += sizeof(evt->PacketLength);

	if (evt->PacketLength > 0)
	{
		evt->Packet = MEM_BufferAlloc(evt->PacketLength);

		if (!evt->Packet)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Packet = NULL;
	}

	FLib_MemCpy(evt->Packet, pPayload + idx, evt->PacketLength); idx += evt->PacketLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPSignalingDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Data packet received through Signaling Channel
***************************************************************************************************/
static mem_status_t Load_L2CAPSignalingDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPSignalingDataIndication_t *evt = &(container->Data.L2CAPSignalingDataIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPSignalingDataIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->PacketLength), pPayload + idx, sizeof(evt->PacketLength)); idx += sizeof(evt->PacketLength);

	if (evt->PacketLength > 0)
	{
		evt->Packet = MEM_BufferAlloc(evt->PacketLength);

		if (!evt->Packet)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Packet = NULL;
	}

	FLib_MemCpy(evt->Packet, pPayload + idx, evt->PacketLength); idx += evt->PacketLength;

	return kStatus_MemSuccess;
}

#endif  /* L2CAP_ENABLE */

#if L2CAPCB_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the L2CAP request
***************************************************************************************************/
static mem_status_t Load_L2CAPCBConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBConfirm_t *evt = &(container->Data.L2CAPCBConfirm);

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(L2CAPCBConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBLePsmConnectionRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Connection request event
***************************************************************************************************/
static mem_status_t Load_L2CAPCBLePsmConnectionRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBLePsmConnectionRequestIndication_t *evt = &(container->Data.L2CAPCBLePsmConnectionRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBLePsmConnectionRequestIndication_FSCI_ID;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->LeCbConnectionRequest.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->LeCbConnectionRequest.LePsm), pPayload + idx, sizeof(evt->LeCbConnectionRequest.LePsm)); idx += sizeof(evt->LeCbConnectionRequest.LePsm);
		FLib_MemCpy(&(evt->LeCbConnectionRequest.PeerMtu), pPayload + idx, sizeof(evt->LeCbConnectionRequest.PeerMtu)); idx += sizeof(evt->LeCbConnectionRequest.PeerMtu);
		FLib_MemCpy(&(evt->LeCbConnectionRequest.PeerMps), pPayload + idx, sizeof(evt->LeCbConnectionRequest.PeerMps)); idx += sizeof(evt->LeCbConnectionRequest.PeerMps);
		FLib_MemCpy(&(evt->LeCbConnectionRequest.InitialCredits), pPayload + idx, sizeof(evt->LeCbConnectionRequest.InitialCredits)); idx += sizeof(evt->LeCbConnectionRequest.InitialCredits);
	}


	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBLePsmConnectionCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Connection complete event
***************************************************************************************************/
static mem_status_t Load_L2CAPCBLePsmConnectionCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBLePsmConnectionCompleteIndication_t *evt = &(container->Data.L2CAPCBLePsmConnectionCompleteIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBLePsmConnectionCompleteIndication_FSCI_ID;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->LeCbConnectionComplete.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->LeCbConnectionComplete.ChannelId), pPayload + idx, sizeof(evt->LeCbConnectionComplete.ChannelId)); idx += sizeof(evt->LeCbConnectionComplete.ChannelId);
		FLib_MemCpy(&(evt->LeCbConnectionComplete.PeerMtu), pPayload + idx, sizeof(evt->LeCbConnectionComplete.PeerMtu)); idx += sizeof(evt->LeCbConnectionComplete.PeerMtu);
		FLib_MemCpy(&(evt->LeCbConnectionComplete.PeerMps), pPayload + idx, sizeof(evt->LeCbConnectionComplete.PeerMps)); idx += sizeof(evt->LeCbConnectionComplete.PeerMps);
		FLib_MemCpy(&(evt->LeCbConnectionComplete.InitialCredits), pPayload + idx, sizeof(evt->LeCbConnectionComplete.InitialCredits)); idx += sizeof(evt->LeCbConnectionComplete.InitialCredits);
		FLib_MemCpy(&(evt->LeCbConnectionComplete.Result), pPayload + idx, 2); idx += 2;
	}


	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBLePsmDisconnectNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Disconnection notification event
***************************************************************************************************/
static mem_status_t Load_L2CAPCBLePsmDisconnectNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBLePsmDisconnectNotificationIndication_t *evt = &(container->Data.L2CAPCBLePsmDisconnectNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBLePsmDisconnectNotificationIndication_FSCI_ID;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->LeCbDisconnection.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->LeCbDisconnection.ChannelId), pPayload + idx, sizeof(evt->LeCbDisconnection.ChannelId)); idx += sizeof(evt->LeCbDisconnection.ChannelId);
	}


	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBNoPeerCreditsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - No peer credits event
***************************************************************************************************/
static mem_status_t Load_L2CAPCBNoPeerCreditsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBNoPeerCreditsIndication_t *evt = &(container->Data.L2CAPCBNoPeerCreditsIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBNoPeerCreditsIndication_FSCI_ID;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->LeCbNoPeerCredits.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->LeCbNoPeerCredits.ChannelId), pPayload + idx, sizeof(evt->LeCbNoPeerCredits.ChannelId)); idx += sizeof(evt->LeCbNoPeerCredits.ChannelId);
	}


	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBLocalCreditsNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Local credits notification event
***************************************************************************************************/
static mem_status_t Load_L2CAPCBLocalCreditsNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBLocalCreditsNotificationIndication_t *evt = &(container->Data.L2CAPCBLocalCreditsNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBLocalCreditsNotificationIndication_FSCI_ID;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->LeCbLocalCreditsNotification.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->LeCbLocalCreditsNotification.ChannelId), pPayload + idx, sizeof(evt->LeCbLocalCreditsNotification.ChannelId)); idx += sizeof(evt->LeCbLocalCreditsNotification.ChannelId);
		FLib_MemCpy(&(evt->LeCbLocalCreditsNotification.LocalCredits), pPayload + idx, sizeof(evt->LeCbLocalCreditsNotification.LocalCredits)); idx += sizeof(evt->LeCbLocalCreditsNotification.LocalCredits);
	}


	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBLeCbDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Data packet received through Credit Based Channel
***************************************************************************************************/
static mem_status_t Load_L2CAPCBLeCbDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBLeCbDataIndication_t *evt = &(container->Data.L2CAPCBLeCbDataIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBLeCbDataIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->SrcCid), pPayload + idx, sizeof(evt->SrcCid)); idx += sizeof(evt->SrcCid);
	FLib_MemCpy(&(evt->PacketLength), pPayload + idx, sizeof(evt->PacketLength)); idx += sizeof(evt->PacketLength);

	if (evt->PacketLength > 0)
	{
		evt->Packet = MEM_BufferAlloc(evt->PacketLength);

		if (!evt->Packet)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Packet = NULL;
	}

	FLib_MemCpy(evt->Packet, pPayload + idx, evt->PacketLength); idx += evt->PacketLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Internal error event
***************************************************************************************************/
static mem_status_t Load_L2CAPCBErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBErrorIndication_t *evt = &(container->Data.L2CAPCBErrorIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBErrorIndication_FSCI_ID;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->LeCbError.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->LeCbError.Error), pPayload + idx, 2); idx += 2;
		evt->LeCbError.l2caErrorSource_t = (L2CAPCBErrorIndication_LeCbError_l2caErrorSource_t_t)pPayload[idx]; idx++;
	}


	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBChannelStatusNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Channel status notification
***************************************************************************************************/
static mem_status_t Load_L2CAPCBChannelStatusNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBChannelStatusNotificationIndication_t *evt = &(container->Data.L2CAPCBChannelStatusNotificationIndication);

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBChannelStatusNotificationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(L2CAPCBChannelStatusNotificationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBLePsmEnhancedConnectRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Enhanced Connection Request event
***************************************************************************************************/
static mem_status_t Load_L2CAPCBLePsmEnhancedConnectRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBLePsmEnhancedConnectRequestIndication_t *evt = &(container->Data.L2CAPCBLePsmEnhancedConnectRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBLePsmEnhancedConnectRequestIndication_FSCI_ID;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->EnhancedConnRequest.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->EnhancedConnRequest.LePsm), pPayload + idx, sizeof(evt->EnhancedConnRequest.LePsm)); idx += sizeof(evt->EnhancedConnRequest.LePsm);
		FLib_MemCpy(&(evt->EnhancedConnRequest.PeerMtu), pPayload + idx, sizeof(evt->EnhancedConnRequest.PeerMtu)); idx += sizeof(evt->EnhancedConnRequest.PeerMtu);
		FLib_MemCpy(&(evt->EnhancedConnRequest.PeerMps), pPayload + idx, sizeof(evt->EnhancedConnRequest.PeerMps)); idx += sizeof(evt->EnhancedConnRequest.PeerMps);
		FLib_MemCpy(&(evt->EnhancedConnRequest.InitialCredits), pPayload + idx, sizeof(evt->EnhancedConnRequest.InitialCredits)); idx += sizeof(evt->EnhancedConnRequest.InitialCredits);
		evt->EnhancedConnRequest.NoOfChannels = pPayload[idx]; idx++;

		if (evt->EnhancedConnRequest.NoOfChannels > 0)
		{
			evt->EnhancedConnRequest.aCids = MEM_BufferAlloc(evt->EnhancedConnRequest.NoOfChannels * 2);

			if (!evt->EnhancedConnRequest.aCids)
			{
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->EnhancedConnRequest.aCids = NULL;
		}

		FLib_MemCpy(evt->EnhancedConnRequest.aCids, pPayload + idx, evt->EnhancedConnRequest.NoOfChannels * 2); idx += evt->EnhancedConnRequest.NoOfChannels * 2;
	}


	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBLePsmEnhancedConnectionCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Enhanced Connection Complete event
***************************************************************************************************/
static mem_status_t Load_L2CAPCBLePsmEnhancedConnectionCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBLePsmEnhancedConnectionCompleteIndication_t *evt = &(container->Data.L2CAPCBLePsmEnhancedConnectionCompleteIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBLePsmEnhancedConnectionCompleteIndication_FSCI_ID;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->EnhancedConnComplete.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->EnhancedConnComplete.PeerMtu), pPayload + idx, sizeof(evt->EnhancedConnComplete.PeerMtu)); idx += sizeof(evt->EnhancedConnComplete.PeerMtu);
		FLib_MemCpy(&(evt->EnhancedConnComplete.PeerMps), pPayload + idx, sizeof(evt->EnhancedConnComplete.PeerMps)); idx += sizeof(evt->EnhancedConnComplete.PeerMps);
		FLib_MemCpy(&(evt->EnhancedConnComplete.InitialCredits), pPayload + idx, sizeof(evt->EnhancedConnComplete.InitialCredits)); idx += sizeof(evt->EnhancedConnComplete.InitialCredits);
		FLib_MemCpy(&(evt->EnhancedConnComplete.Result), pPayload + idx, 2); idx += 2;
		evt->EnhancedConnComplete.NoOfChannels = pPayload[idx]; idx++;

		if (evt->EnhancedConnComplete.NoOfChannels > 0)
		{
			evt->EnhancedConnComplete.aCids = MEM_BufferAlloc(evt->EnhancedConnComplete.NoOfChannels * 2);

			if (!evt->EnhancedConnComplete.aCids)
			{
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->EnhancedConnComplete.aCids = NULL;
		}

		FLib_MemCpy(evt->EnhancedConnComplete.aCids, pPayload + idx, evt->EnhancedConnComplete.NoOfChannels * 2); idx += evt->EnhancedConnComplete.NoOfChannels * 2;
	}


	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBEnhancedReconfigureRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Enhanced Reconfigure Request event
***************************************************************************************************/
static mem_status_t Load_L2CAPCBEnhancedReconfigureRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBEnhancedReconfigureRequestIndication_t *evt = &(container->Data.L2CAPCBEnhancedReconfigureRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBEnhancedReconfigureRequestIndication_FSCI_ID;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->EnhancedReconfigureRequest.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->EnhancedReconfigureRequest.NewMtu), pPayload + idx, sizeof(evt->EnhancedReconfigureRequest.NewMtu)); idx += sizeof(evt->EnhancedReconfigureRequest.NewMtu);
		FLib_MemCpy(&(evt->EnhancedReconfigureRequest.NewMps), pPayload + idx, sizeof(evt->EnhancedReconfigureRequest.NewMps)); idx += sizeof(evt->EnhancedReconfigureRequest.NewMps);
		FLib_MemCpy(&(evt->EnhancedReconfigureRequest.Result), pPayload + idx, 2); idx += 2;
		evt->EnhancedReconfigureRequest.NoOfChannels = pPayload[idx]; idx++;

		if (evt->EnhancedReconfigureRequest.NoOfChannels > 0)
		{
			evt->EnhancedReconfigureRequest.aCids = MEM_BufferAlloc(evt->EnhancedReconfigureRequest.NoOfChannels * 2);

			if (!evt->EnhancedReconfigureRequest.aCids)
			{
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->EnhancedReconfigureRequest.aCids = NULL;
		}

		FLib_MemCpy(evt->EnhancedReconfigureRequest.aCids, pPayload + idx, evt->EnhancedReconfigureRequest.NoOfChannels * 2); idx += evt->EnhancedReconfigureRequest.NoOfChannels * 2;
	}


	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBEnhancedReconfigureResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Enhanced Reconfigure Response event
***************************************************************************************************/
static mem_status_t Load_L2CAPCBEnhancedReconfigureResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBEnhancedReconfigureResponseIndication_t *evt = &(container->Data.L2CAPCBEnhancedReconfigureResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBEnhancedReconfigureResponseIndication_FSCI_ID;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->EnhancedReconfigureResponse.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->EnhancedReconfigureResponse.Result), pPayload + idx, 2); idx += 2;
	}


	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_L2CAPCBLowPeerCreditsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Low peer credits event
***************************************************************************************************/
static mem_status_t Load_L2CAPCBLowPeerCreditsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPCBLowPeerCreditsIndication_t *evt = &(container->Data.L2CAPCBLowPeerCreditsIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = L2CAPCBLowPeerCreditsIndication_FSCI_ID;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->LeCbLowPeerCredits.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->LeCbLowPeerCredits.ChannelId), pPayload + idx, sizeof(evt->LeCbLowPeerCredits.ChannelId)); idx += sizeof(evt->LeCbLowPeerCredits.ChannelId);
	}


	return kStatus_MemSuccess;
}

#endif  /* L2CAPCB_ENABLE */

#if SM_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_SMDHKeyGenerateIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	SM DHKey Generated
***************************************************************************************************/
static mem_status_t Load_SMDHKeyGenerateIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMDHKeyGenerateIndication_t *evt = &(container->Data.SMDHKeyGenerateIndication);

	/* Store (OG, OC) in ID */
	container->id = SMDHKeyGenerateIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMDHKeyGenerateIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the SM request
***************************************************************************************************/
static mem_status_t Load_SMConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMConfirm_t *evt = &(container->Data.SMConfirm);

	/* Store (OG, OC) in ID */
	container->id = SMConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Received internal status of the SM (which was not generated by an upper layer action)
***************************************************************************************************/
static mem_status_t Load_SMStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMStatusIndication_t *evt = &(container->Data.SMStatusIndication);

	/* Store (OG, OC) in ID */
	container->id = SMStatusIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMStatusIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMRemoteSecurityRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	SMP Security Request has been received from an Peripheral device
***************************************************************************************************/
static mem_status_t Load_SMRemoteSecurityRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMRemoteSecurityRequestIndication_t *evt = &(container->Data.SMRemoteSecurityRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = SMRemoteSecurityRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMRemoteSecurityRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMRemotePairingRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Pairing Request has been received from a Central device
***************************************************************************************************/
static mem_status_t Load_SMRemotePairingRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMRemotePairingRequestIndication_t *evt = &(container->Data.SMRemotePairingRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = SMRemotePairingRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMRemotePairingRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMRemotePairingResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Pairing Response has been received from a Peripheral device
***************************************************************************************************/
static mem_status_t Load_SMRemotePairingResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMRemotePairingResponseIndication_t *evt = &(container->Data.SMRemotePairingResponseIndication);

	/* Store (OG, OC) in ID */
	container->id = SMRemotePairingResponseIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMRemotePairingResponseIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMPasskeyDisplayRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Display a Passkey from 0 to 999.999 which must be entered on the peer device
***************************************************************************************************/
static mem_status_t Load_SMPasskeyDisplayRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMPasskeyDisplayRequestIndication_t *evt = &(container->Data.SMPasskeyDisplayRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = SMPasskeyDisplayRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMPasskeyDisplayRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMPasskeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Request a Passkey (either decimal 0 to 999.999 or 16 bytes OOB)
***************************************************************************************************/
static mem_status_t Load_SMPasskeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMPasskeyRequestIndication_t *evt = &(container->Data.SMPasskeyRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = SMPasskeyRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMPasskeyRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMPairingKeysetRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Sent to the host by the SM to request a pairing keyset
***************************************************************************************************/
static mem_status_t Load_SMPairingKeysetRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMPairingKeysetRequestIndication_t *evt = &(container->Data.SMPairingKeysetRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = SMPairingKeysetRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMPairingKeysetRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMPairingKeysetReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Sent to the host by the SM, containing a pairing keyset received from a peer device during the Key Distribution phase
***************************************************************************************************/
static mem_status_t Load_SMPairingKeysetReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMPairingKeysetReceivedIndication_t *evt = &(container->Data.SMPairingKeysetReceivedIndication);

	/* Store (OG, OC) in ID */
	container->id = SMPairingKeysetReceivedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMPairingKeysetReceivedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMPairingCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Signals the successful completion of a SMP Pairing procedure with a peer device
***************************************************************************************************/
static mem_status_t Load_SMPairingCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMPairingCompleteIndication_t *evt = &(container->Data.SMPairingCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = SMPairingCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMPairingCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMPairingFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Signals a local pairing procedure failure and the reason
***************************************************************************************************/
static mem_status_t Load_SMPairingFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMPairingFailedIndication_t *evt = &(container->Data.SMPairingFailedIndication);

	/* Store (OG, OC) in ID */
	container->id = SMPairingFailedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMPairingFailedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMReceivedPairingFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Signals a remote pairing procedure failure  and the reason
***************************************************************************************************/
static mem_status_t Load_SMReceivedPairingFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMReceivedPairingFailedIndication_t *evt = &(container->Data.SMReceivedPairingFailedIndication);

	/* Store (OG, OC) in ID */
	container->id = SMReceivedPairingFailedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMReceivedPairingFailedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMLlLtkRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Link layer requires the LTK from the upper layer to encrypt a link
***************************************************************************************************/
static mem_status_t Load_SMLlLtkRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMLlLtkRequestIndication_t *evt = &(container->Data.SMLlLtkRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = SMLlLtkRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMLlLtkRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMLlEncryptionStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of a Link Layer encryption procedure
***************************************************************************************************/
static mem_status_t Load_SMLlEncryptionStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMLlEncryptionStatusIndication_t *evt = &(container->Data.SMLlEncryptionStatusIndication);

	/* Store (OG, OC) in ID */
	container->id = SMLlEncryptionStatusIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMLlEncryptionStatusIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMLlEncryptResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	128 bit data block was encrypted (ES-128) with the provided key
***************************************************************************************************/
static mem_status_t Load_SMLlEncryptResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMLlEncryptResIndication_t *evt = &(container->Data.SMLlEncryptResIndication);

	/* Store (OG, OC) in ID */
	container->id = SMLlEncryptResIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMLlEncryptResIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMLlRandResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	64 bit random number
***************************************************************************************************/
static mem_status_t Load_SMLlRandResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMLlRandResIndication_t *evt = &(container->Data.SMLlRandResIndication);

	/* Store (OG, OC) in ID */
	container->id = SMLlRandResIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMLlRandResIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMTbCreateRandomDeviceAddrResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	SM Tool Box: A Random Device Address of the requested type
***************************************************************************************************/
static mem_status_t Load_SMTbCreateRandomDeviceAddrResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMTbCreateRandomDeviceAddrResIndication_t *evt = &(container->Data.SMTbCreateRandomDeviceAddrResIndication);

	/* Store (OG, OC) in ID */
	container->id = SMTbCreateRandomDeviceAddrResIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMTbCreateRandomDeviceAddrResIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMTbCheckResolvalePrivateAddrResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	SM Tool Box: Result of a verification of a private address against a given IRK
***************************************************************************************************/
static mem_status_t Load_SMTbCheckResolvalePrivateAddrResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMTbCheckResolvalePrivateAddrResIndication_t *evt = &(container->Data.SMTbCheckResolvalePrivateAddrResIndication);

	/* Store (OG, OC) in ID */
	container->id = SMTbCheckResolvalePrivateAddrResIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMTbCheckResolvalePrivateAddrResIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMTbSignDataResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	SM Tool Box: Returns the signature for a data block
***************************************************************************************************/
static mem_status_t Load_SMTbSignDataResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMTbSignDataResIndication_t *evt = &(container->Data.SMTbSignDataResIndication);

	/* Store (OG, OC) in ID */
	container->id = SMTbSignDataResIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMTbSignDataResIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMTbVerifyDataSignatureResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	SM Tool Box: Result of the verification of the signature of a data block
***************************************************************************************************/
static mem_status_t Load_SMTbVerifyDataSignatureResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMTbVerifyDataSignatureResIndication_t *evt = &(container->Data.SMTbVerifyDataSignatureResIndication);

	/* Store (OG, OC) in ID */
	container->id = SMTbVerifyDataSignatureResIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMTbVerifyDataSignatureResIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMRemoteKeypressNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Remote SMP Keypress Notification recieved during Passkey Entry Pairing Method
***************************************************************************************************/
static mem_status_t Load_SMRemoteKeypressNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMRemoteKeypressNotificationIndication_t *evt = &(container->Data.SMRemoteKeypressNotificationIndication);

	/* Store (OG, OC) in ID */
	container->id = SMRemoteKeypressNotificationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMRemoteKeypressNotificationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMNcDisplayRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event sent to display and confirm a Numeric Comparison Value when using the LE SC Numeric Comparison pairing method
***************************************************************************************************/
static mem_status_t Load_SMNcDisplayRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMNcDisplayRequestIndication_t *evt = &(container->Data.SMNcDisplayRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = SMNcDisplayRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMNcDisplayRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMLeScOobDataRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event sent to the host by the SM to request LE SC OOB Data (r, Cr and Addr) received from a peer
***************************************************************************************************/
static mem_status_t Load_SMLeScOobDataRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMLeScOobDataRequestIndication_t *evt = &(container->Data.SMLeScOobDataRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = SMLeScOobDataRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMLeScOobDataRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMLocalLeScOobDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event sent to the host by the SM to provide local LE SC OOB Data (r, Cr and Addr) to be sent to a peer
***************************************************************************************************/
static mem_status_t Load_SMLocalLeScOobDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMLocalLeScOobDataIndication_t *evt = &(container->Data.SMLocalLeScOobDataIndication);

	/* Store (OG, OC) in ID */
	container->id = SMLocalLeScOobDataIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMLocalLeScOobDataIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_SMGenerateNewEcdhPkSkPairResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Result of the generation of a new ECDH Public Key - Secret Key pair
***************************************************************************************************/
static mem_status_t Load_SMGenerateNewEcdhPkSkPairResIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	SMGenerateNewEcdhPkSkPairResIndication_t *evt = &(container->Data.SMGenerateNewEcdhPkSkPairResIndication);

	/* Store (OG, OC) in ID */
	container->id = SMGenerateNewEcdhPkSkPairResIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(SMGenerateNewEcdhPkSkPairResIndication_t));

	return kStatus_MemSuccess;
}

#endif  /* SM_ENABLE */

#if ATT_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the ATT request
***************************************************************************************************/
static mem_status_t Load_ATTConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTConfirm_t *evt = &(container->Data.ATTConfirm);

	/* Store (OG, OC) in ID */
	container->id = ATTConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTGetMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT Get MTU Indication
***************************************************************************************************/
static mem_status_t Load_ATTGetMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTGetMtuIndication_t *evt = &(container->Data.ATTGetMtuIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTGetMtuIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTGetMtuIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerErrorResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received error response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerErrorResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerErrorResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerErrorResponseIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerErrorResponseIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTClientIncomingServerErrorResponseIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientExchangeMtuRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received exchange MTU request (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientExchangeMtuRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientExchangeMtuRequestIndication_t *evt = &(container->Data.ATTServerIncomingClientExchangeMtuRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientExchangeMtuRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTServerIncomingClientExchangeMtuRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerExchangeMtuResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received exchange MTU response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerExchangeMtuResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerExchangeMtuResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerExchangeMtuResponseIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerExchangeMtuResponseIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTClientIncomingServerExchangeMtuResponseIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientFindInformationRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received find information request (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientFindInformationRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientFindInformationRequestIndication_t *evt = &(container->Data.ATTServerIncomingClientFindInformationRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientFindInformationRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTServerIncomingClientFindInformationRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerFindInformationResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received find information response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerFindInformationResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerFindInformationResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerFindInformationResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerFindInformationResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->Params.Format = (ATTClientIncomingServerFindInformationResponseIndication_Params_Format_t)pPayload[idx]; idx++;

	switch (evt->Params.Format)
	{
		case ATTClientIncomingServerFindInformationResponseIndication_Params_Format_Uuid16BitFormat:
			evt->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount = pPayload[idx]; idx++;

			if (evt->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount > 0)
			{
				evt->Params.InformationData.Uuid16BitFormat.HandleUuid16 = MEM_BufferAlloc(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount * sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[0]));

				if (!evt->Params.InformationData.Uuid16BitFormat.HandleUuid16)
				{
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Params.InformationData.Uuid16BitFormat.HandleUuid16 = NULL;
			}


			for (uint32_t i = 0; i < evt->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount; i++)
			{
				FLib_MemCpy(&(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle), pPayload + idx, sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle)); idx += sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle);
				FLib_MemCpy(&(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16), pPayload + idx, sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16)); idx += sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16);
			}
			break;

		case ATTClientIncomingServerFindInformationResponseIndication_Params_Format_Uuid128BitFormat:
			evt->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount = pPayload[idx]; idx++;

			if (evt->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount > 0)
			{
				evt->Params.InformationData.Uuid128BitFormat.HandleUuid128 = MEM_BufferAlloc(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount * sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[0]));

				if (!evt->Params.InformationData.Uuid128BitFormat.HandleUuid128)
				{
					MEM_BufferFree(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Params.InformationData.Uuid128BitFormat.HandleUuid128 = NULL;
			}


			for (uint32_t i = 0; i < evt->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount; i++)
			{
				FLib_MemCpy(&(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle), pPayload + idx, sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle)); idx += sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle);
				FLib_MemCpy(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128, pPayload + idx, sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128)); idx += sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128);
			}
			break;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientFindByTypeValueRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received find by type value request (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientFindByTypeValueRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientFindByTypeValueRequestIndication_t *evt = &(container->Data.ATTServerIncomingClientFindByTypeValueRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientFindByTypeValueRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.StartingHandle), pPayload + idx, sizeof(evt->Params.StartingHandle)); idx += sizeof(evt->Params.StartingHandle);
	FLib_MemCpy(&(evt->Params.EndingHandle), pPayload + idx, sizeof(evt->Params.EndingHandle)); idx += sizeof(evt->Params.EndingHandle);
	FLib_MemCpy(&(evt->Params.AttributeType), pPayload + idx, sizeof(evt->Params.AttributeType)); idx += sizeof(evt->Params.AttributeType);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerFindByTypeValueResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received find by type value response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerFindByTypeValueResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerFindByTypeValueResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerFindByTypeValueResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerFindByTypeValueResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.GroupCount), pPayload + idx, sizeof(evt->Params.GroupCount)); idx += sizeof(evt->Params.GroupCount);

	if (evt->Params.GroupCount > 0)
	{
		evt->Params.HandleGroup = MEM_BufferAlloc(evt->Params.GroupCount * sizeof(evt->Params.HandleGroup[0]));

		if (!evt->Params.HandleGroup)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.HandleGroup = NULL;
	}


	for (uint32_t i = 0; i < evt->Params.GroupCount; i++)
	{
		FLib_MemCpy(&(evt->Params.HandleGroup[i].StartingHandle), pPayload + idx, sizeof(evt->Params.HandleGroup[i].StartingHandle)); idx += sizeof(evt->Params.HandleGroup[i].StartingHandle);
		FLib_MemCpy(&(evt->Params.HandleGroup[i].EndingHandle), pPayload + idx, sizeof(evt->Params.HandleGroup[i].EndingHandle)); idx += sizeof(evt->Params.HandleGroup[i].EndingHandle);
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientReadByTypeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received read by type request (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientReadByTypeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientReadByTypeRequestIndication_t *evt = &(container->Data.ATTServerIncomingClientReadByTypeRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientReadByTypeRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.StartingHandle), pPayload + idx, sizeof(evt->Params.StartingHandle)); idx += sizeof(evt->Params.StartingHandle);
	FLib_MemCpy(&(evt->Params.EndingHandle), pPayload + idx, sizeof(evt->Params.EndingHandle)); idx += sizeof(evt->Params.EndingHandle);
	evt->Params.Format = (ATTServerIncomingClientReadByTypeRequestIndication_Params_Format_t)pPayload[idx]; idx++;

	switch (evt->Params.Format)
	{
		case ATTServerIncomingClientReadByTypeRequestIndication_Params_Format_Uuid16BitFormat:
			FLib_MemCpy(&(evt->Params.AttributeType.Uuid16BitFormat), pPayload + idx, sizeof(evt->Params.AttributeType.Uuid16BitFormat)); idx += sizeof(evt->Params.AttributeType.Uuid16BitFormat);
			break;

		case ATTServerIncomingClientReadByTypeRequestIndication_Params_Format_Uuid128BitFormat:
			FLib_MemCpy(evt->Params.AttributeType.Uuid128BitFormat, pPayload + idx, sizeof(evt->Params.AttributeType.Uuid128BitFormat)); idx += sizeof(evt->Params.AttributeType.Uuid128BitFormat);
			break;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerReadByTypeResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received read by type response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerReadByTypeResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerReadByTypeResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerReadByTypeResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerReadByTypeResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->Params.Length = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeDataListLength), pPayload + idx, sizeof(evt->Params.AttributeDataListLength)); idx += sizeof(evt->Params.AttributeDataListLength);

	if (evt->Params.AttributeDataListLength > 0)
	{
		evt->Params.AttributeDataList = MEM_BufferAlloc(evt->Params.AttributeDataListLength);

		if (!evt->Params.AttributeDataList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeDataList = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeDataList, pPayload + idx, evt->Params.AttributeDataListLength); idx += evt->Params.AttributeDataListLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientReadRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received read request (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientReadRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientReadRequestIndication_t *evt = &(container->Data.ATTServerIncomingClientReadRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientReadRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTServerIncomingClientReadRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerReadResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received read response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerReadResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerReadResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerReadResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerReadResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientReadBlobRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received read blob request (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientReadBlobRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientReadBlobRequestIndication_t *evt = &(container->Data.ATTServerIncomingClientReadBlobRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientReadBlobRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTServerIncomingClientReadBlobRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerReadBlobResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received read blob response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerReadBlobResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerReadBlobResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerReadBlobResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerReadBlobResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientReadMultipleRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received read multiple request (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientReadMultipleRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientReadMultipleRequestIndication_t *evt = &(container->Data.ATTServerIncomingClientReadMultipleRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientReadMultipleRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.HandleCount), pPayload + idx, sizeof(evt->Params.HandleCount)); idx += sizeof(evt->Params.HandleCount);

	if (evt->Params.HandleCount > 0)
	{
		evt->Params.ListOfHandles = MEM_BufferAlloc(evt->Params.HandleCount * 2);

		if (!evt->Params.ListOfHandles)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.ListOfHandles = NULL;
	}

	FLib_MemCpy(evt->Params.ListOfHandles, pPayload + idx, evt->Params.HandleCount * 2); idx += evt->Params.HandleCount * 2;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerReadMultipleResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received read multiple response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerReadMultipleResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerReadMultipleResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerReadMultipleResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerReadMultipleResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.ListLength), pPayload + idx, sizeof(evt->Params.ListLength)); idx += sizeof(evt->Params.ListLength);

	if (evt->Params.ListLength > 0)
	{
		evt->Params.ListOfValues = MEM_BufferAlloc(evt->Params.ListLength);

		if (!evt->Params.ListOfValues)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.ListOfValues = NULL;
	}

	FLib_MemCpy(evt->Params.ListOfValues, pPayload + idx, evt->Params.ListLength); idx += evt->Params.ListLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientReadByGroupTypeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received read by group type request (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientReadByGroupTypeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientReadByGroupTypeRequestIndication_t *evt = &(container->Data.ATTServerIncomingClientReadByGroupTypeRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientReadByGroupTypeRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.StartingHandle), pPayload + idx, sizeof(evt->Params.StartingHandle)); idx += sizeof(evt->Params.StartingHandle);
	FLib_MemCpy(&(evt->Params.EndingHandle), pPayload + idx, sizeof(evt->Params.EndingHandle)); idx += sizeof(evt->Params.EndingHandle);
	evt->Params.Format = (ATTServerIncomingClientReadByGroupTypeRequestIndication_Params_Format_t)pPayload[idx]; idx++;

	switch (evt->Params.Format)
	{
		case ATTServerIncomingClientReadByGroupTypeRequestIndication_Params_Format_Uuid16BitFormat:
			FLib_MemCpy(&(evt->Params.AttributeType.Uuid16BitFormat), pPayload + idx, sizeof(evt->Params.AttributeType.Uuid16BitFormat)); idx += sizeof(evt->Params.AttributeType.Uuid16BitFormat);
			break;

		case ATTServerIncomingClientReadByGroupTypeRequestIndication_Params_Format_Uuid128BitFormat:
			FLib_MemCpy(evt->Params.AttributeType.Uuid128BitFormat, pPayload + idx, sizeof(evt->Params.AttributeType.Uuid128BitFormat)); idx += sizeof(evt->Params.AttributeType.Uuid128BitFormat);
			break;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerReadByGroupTypeResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received read by group type response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerReadByGroupTypeResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerReadByGroupTypeResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerReadByGroupTypeResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerReadByGroupTypeResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->Params.Length = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeDataListLength), pPayload + idx, sizeof(evt->Params.AttributeDataListLength)); idx += sizeof(evt->Params.AttributeDataListLength);

	if (evt->Params.AttributeDataListLength > 0)
	{
		evt->Params.AttributeDataList = MEM_BufferAlloc(evt->Params.AttributeDataListLength);

		if (!evt->Params.AttributeDataList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeDataList = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeDataList, pPayload + idx, evt->Params.AttributeDataListLength); idx += evt->Params.AttributeDataListLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received write request (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientWriteRequestIndication_t *evt = &(container->Data.ATTServerIncomingClientWriteRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientWriteRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received write response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerWriteResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerWriteResponseIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerWriteResponseIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTClientIncomingServerWriteResponseIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientWriteCommandIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received write command (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientWriteCommandIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientWriteCommandIndication_t *evt = &(container->Data.ATTServerIncomingClientWriteCommandIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientWriteCommandIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientSignedWriteCommandIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received signed write command (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientSignedWriteCommandIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientSignedWriteCommandIndication_t *evt = &(container->Data.ATTServerIncomingClientSignedWriteCommandIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientSignedWriteCommandIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;
	FLib_MemCpy(evt->Params.AuthenticationSignature, pPayload + idx, 12); idx += 12;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientPrepareWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received prepare write request (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientPrepareWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientPrepareWriteRequestIndication_t *evt = &(container->Data.ATTServerIncomingClientPrepareWriteRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientPrepareWriteRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.ValueOffset), pPayload + idx, sizeof(evt->Params.ValueOffset)); idx += sizeof(evt->Params.ValueOffset);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerPrepareWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received prepare write response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerPrepareWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerPrepareWriteResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerPrepareWriteResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerPrepareWriteResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.ValueOffset), pPayload + idx, sizeof(evt->Params.ValueOffset)); idx += sizeof(evt->Params.ValueOffset);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientExecuteWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received execute write request (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientExecuteWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientExecuteWriteRequestIndication_t *evt = &(container->Data.ATTServerIncomingClientExecuteWriteRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientExecuteWriteRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTServerIncomingClientExecuteWriteRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerExecuteWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received execute write response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerExecuteWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerExecuteWriteResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerExecuteWriteResponseIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerExecuteWriteResponseIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTClientIncomingServerExecuteWriteResponseIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received handle value notification (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerHandleValueNotificationIndication_t *evt = &(container->Data.ATTClientIncomingServerHandleValueNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerHandleValueNotificationIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerHandleValueIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received handle value indication (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerHandleValueIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerHandleValueIndicationIndication_t *evt = &(container->Data.ATTClientIncomingServerHandleValueIndicationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerHandleValueIndicationIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received handle value confirmation (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientHandleValueConfirmationIndication_t *evt = &(container->Data.ATTServerIncomingClientHandleValueConfirmationIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientHandleValueConfirmationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTServerIncomingClientHandleValueConfirmationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTUnsupportedOpcodeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT unsupported opcode
***************************************************************************************************/
static mem_status_t Load_ATTUnsupportedOpcodeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTUnsupportedOpcodeIndication_t *evt = &(container->Data.ATTUnsupportedOpcodeIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTUnsupportedOpcodeIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTUnsupportedOpcodeIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTTimeoutIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT timeout event
***************************************************************************************************/
static mem_status_t Load_ATTTimeoutIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTTimeoutIndication_t *evt = &(container->Data.ATTTimeoutIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTTimeoutIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTTimeoutIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTInvalidPduReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT invalid PDU received event
***************************************************************************************************/
static mem_status_t Load_ATTInvalidPduReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTInvalidPduReceivedIndication_t *evt = &(container->Data.ATTInvalidPduReceivedIndication);

	/* Store (OG, OC) in ID */
	container->id = ATTInvalidPduReceivedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(ATTInvalidPduReceivedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerReadMultipleVariableResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received read multiple variable response (on client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerReadMultipleVariableResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerReadMultipleVariableResponseIndication_t *evt = &(container->Data.ATTClientIncomingServerReadMultipleVariableResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerReadMultipleVariableResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.ListLength), pPayload + idx, sizeof(evt->Params.ListLength)); idx += sizeof(evt->Params.ListLength);

	if (evt->Params.ListLength > 0)
	{
		evt->Params.lengthValueTupleList = MEM_BufferAlloc(evt->Params.ListLength);

		if (!evt->Params.lengthValueTupleList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.lengthValueTupleList = NULL;
	}

	FLib_MemCpy(evt->Params.lengthValueTupleList, pPayload + idx, evt->Params.ListLength); idx += evt->Params.ListLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTServerIncomingClientReadMultipleVariableRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT received read multiple variable request (on server)
***************************************************************************************************/
static mem_status_t Load_ATTServerIncomingClientReadMultipleVariableRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTServerIncomingClientReadMultipleVariableRequestIndication_t *evt = &(container->Data.ATTServerIncomingClientReadMultipleVariableRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTServerIncomingClientReadMultipleVariableRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.HandleCount), pPayload + idx, sizeof(evt->Params.HandleCount)); idx += sizeof(evt->Params.HandleCount);

	if (evt->Params.HandleCount > 0)
	{
		evt->Params.ListOfHandles = MEM_BufferAlloc(evt->Params.HandleCount * 2);

		if (!evt->Params.ListOfHandles)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.ListOfHandles = NULL;
	}

	FLib_MemCpy(evt->Params.ListOfHandles, pPayload + idx, evt->Params.HandleCount * 2); idx += evt->Params.HandleCount * 2;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_ATTClientIncomingServerMultipleHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ATT multiple handle notification received (on Client)
***************************************************************************************************/
static mem_status_t Load_ATTClientIncomingServerMultipleHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	ATTClientIncomingServerMultipleHandleValueNotificationIndication_t *evt = &(container->Data.ATTClientIncomingServerMultipleHandleValueNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = ATTClientIncomingServerMultipleHandleValueNotificationIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->TotalLength), pPayload + idx, sizeof(evt->TotalLength)); idx += sizeof(evt->TotalLength);

	if (evt->TotalLength > 0)
	{
		evt->HandleLengthValueList = MEM_BufferAlloc(evt->TotalLength);

		if (!evt->HandleLengthValueList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->HandleLengthValueList = NULL;
	}

	FLib_MemCpy(evt->HandleLengthValueList, pPayload + idx, evt->TotalLength); idx += evt->TotalLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerErrorResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received error response (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerErrorResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerErrorResponseIndication_t *evt = &(container->Data.EATTClientIncomingServerErrorResponseIndication);

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerErrorResponseIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(EATTClientIncomingServerErrorResponseIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientFindInformationRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received find information request (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientFindInformationRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientFindInformationRequestIndication_t *evt = &(container->Data.EATTServerIncomingClientFindInformationRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientFindInformationRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(EATTServerIncomingClientFindInformationRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerFindInformationResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received find information response (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerFindInformationResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerFindInformationResponseIndication_t *evt = &(container->Data.EATTClientIncomingServerFindInformationResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerFindInformationResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->Params.Format = (EATTClientIncomingServerFindInformationResponseIndication_Params_Format_t)pPayload[idx]; idx++;

	switch (evt->Params.Format)
	{
		case EATTClientIncomingServerFindInformationResponseIndication_Params_Format_Uuid16BitFormat:
			evt->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount = pPayload[idx]; idx++;

			if (evt->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount > 0)
			{
				evt->Params.InformationData.Uuid16BitFormat.HandleUuid16 = MEM_BufferAlloc(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount * sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[0]));

				if (!evt->Params.InformationData.Uuid16BitFormat.HandleUuid16)
				{
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Params.InformationData.Uuid16BitFormat.HandleUuid16 = NULL;
			}


			for (uint32_t i = 0; i < evt->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount; i++)
			{
				FLib_MemCpy(&(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle), pPayload + idx, sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle)); idx += sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle);
				FLib_MemCpy(&(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16), pPayload + idx, sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16)); idx += sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16);
			}
			break;

		case EATTClientIncomingServerFindInformationResponseIndication_Params_Format_Uuid128BitFormat:
			evt->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount = pPayload[idx]; idx++;

			if (evt->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount > 0)
			{
				evt->Params.InformationData.Uuid128BitFormat.HandleUuid128 = MEM_BufferAlloc(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount * sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[0]));

				if (!evt->Params.InformationData.Uuid128BitFormat.HandleUuid128)
				{
					MEM_BufferFree(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Params.InformationData.Uuid128BitFormat.HandleUuid128 = NULL;
			}


			for (uint32_t i = 0; i < evt->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount; i++)
			{
				FLib_MemCpy(&(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle), pPayload + idx, sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle)); idx += sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle);
				FLib_MemCpy(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128, pPayload + idx, sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128)); idx += sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128);
			}
			break;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientFindByTypeValueRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received find by type value request (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientFindByTypeValueRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientFindByTypeValueRequestIndication_t *evt = &(container->Data.EATTServerIncomingClientFindByTypeValueRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientFindByTypeValueRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.StartingHandle), pPayload + idx, sizeof(evt->Params.StartingHandle)); idx += sizeof(evt->Params.StartingHandle);
	FLib_MemCpy(&(evt->Params.EndingHandle), pPayload + idx, sizeof(evt->Params.EndingHandle)); idx += sizeof(evt->Params.EndingHandle);
	FLib_MemCpy(&(evt->Params.AttributeType), pPayload + idx, sizeof(evt->Params.AttributeType)); idx += sizeof(evt->Params.AttributeType);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerFindByTypeValueResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received find by type value response (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerFindByTypeValueResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerFindByTypeValueResponseIndication_t *evt = &(container->Data.EATTClientIncomingServerFindByTypeValueResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerFindByTypeValueResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.GroupCount), pPayload + idx, sizeof(evt->Params.GroupCount)); idx += sizeof(evt->Params.GroupCount);

	if (evt->Params.GroupCount > 0)
	{
		evt->Params.HandleGroup = MEM_BufferAlloc(evt->Params.GroupCount * sizeof(evt->Params.HandleGroup[0]));

		if (!evt->Params.HandleGroup)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.HandleGroup = NULL;
	}


	for (uint32_t i = 0; i < evt->Params.GroupCount; i++)
	{
		FLib_MemCpy(&(evt->Params.HandleGroup[i].StartingHandle), pPayload + idx, sizeof(evt->Params.HandleGroup[i].StartingHandle)); idx += sizeof(evt->Params.HandleGroup[i].StartingHandle);
		FLib_MemCpy(&(evt->Params.HandleGroup[i].EndingHandle), pPayload + idx, sizeof(evt->Params.HandleGroup[i].EndingHandle)); idx += sizeof(evt->Params.HandleGroup[i].EndingHandle);
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientReadByTypeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received read by type request (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientReadByTypeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientReadByTypeRequestIndication_t *evt = &(container->Data.EATTServerIncomingClientReadByTypeRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientReadByTypeRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.StartingHandle), pPayload + idx, sizeof(evt->Params.StartingHandle)); idx += sizeof(evt->Params.StartingHandle);
	FLib_MemCpy(&(evt->Params.EndingHandle), pPayload + idx, sizeof(evt->Params.EndingHandle)); idx += sizeof(evt->Params.EndingHandle);
	evt->Params.Format = (EATTServerIncomingClientReadByTypeRequestIndication_Params_Format_t)pPayload[idx]; idx++;

	switch (evt->Params.Format)
	{
		case EATTServerIncomingClientReadByTypeRequestIndication_Params_Format_Uuid16BitFormat:
			FLib_MemCpy(&(evt->Params.AttributeType.Uuid16BitFormat), pPayload + idx, sizeof(evt->Params.AttributeType.Uuid16BitFormat)); idx += sizeof(evt->Params.AttributeType.Uuid16BitFormat);
			break;

		case EATTServerIncomingClientReadByTypeRequestIndication_Params_Format_Uuid128BitFormat:
			FLib_MemCpy(evt->Params.AttributeType.Uuid128BitFormat, pPayload + idx, sizeof(evt->Params.AttributeType.Uuid128BitFormat)); idx += sizeof(evt->Params.AttributeType.Uuid128BitFormat);
			break;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerReadByTypeResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received read by type response (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerReadByTypeResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerReadByTypeResponseIndication_t *evt = &(container->Data.EATTClientIncomingServerReadByTypeResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerReadByTypeResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->Params.Length = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeDataListLength), pPayload + idx, sizeof(evt->Params.AttributeDataListLength)); idx += sizeof(evt->Params.AttributeDataListLength);

	if (evt->Params.AttributeDataListLength > 0)
	{
		evt->Params.AttributeDataList = MEM_BufferAlloc(evt->Params.AttributeDataListLength);

		if (!evt->Params.AttributeDataList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeDataList = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeDataList, pPayload + idx, evt->Params.AttributeDataListLength); idx += evt->Params.AttributeDataListLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientReadRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received read request (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientReadRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientReadRequestIndication_t *evt = &(container->Data.EATTServerIncomingClientReadRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientReadRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(EATTServerIncomingClientReadRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerReadResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received read response (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerReadResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerReadResponseIndication_t *evt = &(container->Data.EATTClientIncomingServerReadResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerReadResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientReadBlobRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received read blob request (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientReadBlobRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientReadBlobRequestIndication_t *evt = &(container->Data.EATTServerIncomingClientReadBlobRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientReadBlobRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(EATTServerIncomingClientReadBlobRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerReadBlobResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received read blob response (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerReadBlobResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerReadBlobResponseIndication_t *evt = &(container->Data.EATTClientIncomingServerReadBlobResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerReadBlobResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientReadMultipleRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received read multiple request (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientReadMultipleRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientReadMultipleRequestIndication_t *evt = &(container->Data.EATTServerIncomingClientReadMultipleRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientReadMultipleRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.HandleCount), pPayload + idx, sizeof(evt->Params.HandleCount)); idx += sizeof(evt->Params.HandleCount);

	if (evt->Params.HandleCount > 0)
	{
		evt->Params.ListOfHandles = MEM_BufferAlloc(evt->Params.HandleCount * 2);

		if (!evt->Params.ListOfHandles)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.ListOfHandles = NULL;
	}

	FLib_MemCpy(evt->Params.ListOfHandles, pPayload + idx, evt->Params.HandleCount * 2); idx += evt->Params.HandleCount * 2;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerReadMultipleResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received read multiple response (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerReadMultipleResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerReadMultipleResponseIndication_t *evt = &(container->Data.EATTClientIncomingServerReadMultipleResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerReadMultipleResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.ListLength), pPayload + idx, sizeof(evt->Params.ListLength)); idx += sizeof(evt->Params.ListLength);

	if (evt->Params.ListLength > 0)
	{
		evt->Params.ListOfValues = MEM_BufferAlloc(evt->Params.ListLength);

		if (!evt->Params.ListOfValues)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.ListOfValues = NULL;
	}

	FLib_MemCpy(evt->Params.ListOfValues, pPayload + idx, evt->Params.ListLength); idx += evt->Params.ListLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientReadByGroupTypeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received read by group type request (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientReadByGroupTypeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientReadByGroupTypeRequestIndication_t *evt = &(container->Data.EATTServerIncomingClientReadByGroupTypeRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientReadByGroupTypeRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.StartingHandle), pPayload + idx, sizeof(evt->Params.StartingHandle)); idx += sizeof(evt->Params.StartingHandle);
	FLib_MemCpy(&(evt->Params.EndingHandle), pPayload + idx, sizeof(evt->Params.EndingHandle)); idx += sizeof(evt->Params.EndingHandle);
	evt->Params.Format = (EATTServerIncomingClientReadByGroupTypeRequestIndication_Params_Format_t)pPayload[idx]; idx++;

	switch (evt->Params.Format)
	{
		case EATTServerIncomingClientReadByGroupTypeRequestIndication_Params_Format_Uuid16BitFormat:
			FLib_MemCpy(&(evt->Params.AttributeType.Uuid16BitFormat), pPayload + idx, sizeof(evt->Params.AttributeType.Uuid16BitFormat)); idx += sizeof(evt->Params.AttributeType.Uuid16BitFormat);
			break;

		case EATTServerIncomingClientReadByGroupTypeRequestIndication_Params_Format_Uuid128BitFormat:
			FLib_MemCpy(evt->Params.AttributeType.Uuid128BitFormat, pPayload + idx, sizeof(evt->Params.AttributeType.Uuid128BitFormat)); idx += sizeof(evt->Params.AttributeType.Uuid128BitFormat);
			break;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerReadByGroupTypeResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received read by group type response (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerReadByGroupTypeResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerReadByGroupTypeResponseIndication_t *evt = &(container->Data.EATTClientIncomingServerReadByGroupTypeResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerReadByGroupTypeResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->Params.Length = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeDataListLength), pPayload + idx, sizeof(evt->Params.AttributeDataListLength)); idx += sizeof(evt->Params.AttributeDataListLength);

	if (evt->Params.AttributeDataListLength > 0)
	{
		evt->Params.AttributeDataList = MEM_BufferAlloc(evt->Params.AttributeDataListLength);

		if (!evt->Params.AttributeDataList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeDataList = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeDataList, pPayload + idx, evt->Params.AttributeDataListLength); idx += evt->Params.AttributeDataListLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received write request (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientWriteRequestIndication_t *evt = &(container->Data.EATTServerIncomingClientWriteRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientWriteRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received write response (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerWriteResponseIndication_t *evt = &(container->Data.EATTClientIncomingServerWriteResponseIndication);

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerWriteResponseIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(EATTClientIncomingServerWriteResponseIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientWriteCommandIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received write command (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientWriteCommandIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientWriteCommandIndication_t *evt = &(container->Data.EATTServerIncomingClientWriteCommandIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientWriteCommandIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientPrepareWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received prepare write request (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientPrepareWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientPrepareWriteRequestIndication_t *evt = &(container->Data.EATTServerIncomingClientPrepareWriteRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientPrepareWriteRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.ValueOffset), pPayload + idx, sizeof(evt->Params.ValueOffset)); idx += sizeof(evt->Params.ValueOffset);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerPrepareWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received prepare write response (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerPrepareWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerPrepareWriteResponseIndication_t *evt = &(container->Data.EATTClientIncomingServerPrepareWriteResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerPrepareWriteResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.ValueOffset), pPayload + idx, sizeof(evt->Params.ValueOffset)); idx += sizeof(evt->Params.ValueOffset);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientExecuteWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received execute write request (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientExecuteWriteRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientExecuteWriteRequestIndication_t *evt = &(container->Data.EATTServerIncomingClientExecuteWriteRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientExecuteWriteRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(EATTServerIncomingClientExecuteWriteRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerExecuteWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received execute write response (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerExecuteWriteResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerExecuteWriteResponseIndication_t *evt = &(container->Data.EATTClientIncomingServerExecuteWriteResponseIndication);

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerExecuteWriteResponseIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(EATTClientIncomingServerExecuteWriteResponseIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received handle value notification (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerHandleValueNotificationIndication_t *evt = &(container->Data.EATTClientIncomingServerHandleValueNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerHandleValueNotificationIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerHandleValueIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received handle value indication (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerHandleValueIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerHandleValueIndicationIndication_t *evt = &(container->Data.EATTClientIncomingServerHandleValueIndicationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerHandleValueIndicationIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT received handle value confirmation (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientHandleValueConfirmationIndication_t *evt = &(container->Data.EATTServerIncomingClientHandleValueConfirmationIndication);

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientHandleValueConfirmationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(EATTServerIncomingClientHandleValueConfirmationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTUnsupportedOpcodeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT unsupported opcode
***************************************************************************************************/
static mem_status_t Load_EATTUnsupportedOpcodeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTUnsupportedOpcodeIndication_t *evt = &(container->Data.EATTUnsupportedOpcodeIndication);

	/* Store (OG, OC) in ID */
	container->id = EATTUnsupportedOpcodeIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(EATTUnsupportedOpcodeIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTTimeoutIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT timeout event
***************************************************************************************************/
static mem_status_t Load_EATTTimeoutIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTTimeoutIndication_t *evt = &(container->Data.EATTTimeoutIndication);

	/* Store (OG, OC) in ID */
	container->id = EATTTimeoutIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(EATTTimeoutIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTInvalidPduReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT invalid PDU received event
***************************************************************************************************/
static mem_status_t Load_EATTInvalidPduReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTInvalidPduReceivedIndication_t *evt = &(container->Data.EATTInvalidPduReceivedIndication);

	/* Store (OG, OC) in ID */
	container->id = EATTInvalidPduReceivedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(EATTInvalidPduReceivedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerReadMultipleVariableResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT read multiple variable response received (on client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerReadMultipleVariableResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerReadMultipleVariableResponseIndication_t *evt = &(container->Data.EATTClientIncomingServerReadMultipleVariableResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerReadMultipleVariableResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.ListLength), pPayload + idx, sizeof(evt->Params.ListLength)); idx += sizeof(evt->Params.ListLength);

	if (evt->Params.ListLength > 0)
	{
		evt->Params.lengthValueTupleList = MEM_BufferAlloc(evt->Params.ListLength);

		if (!evt->Params.lengthValueTupleList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.lengthValueTupleList = NULL;
	}

	FLib_MemCpy(evt->Params.lengthValueTupleList, pPayload + idx, evt->Params.ListLength); idx += evt->Params.ListLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTServerIncomingClientReadMultipleVariableRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT read multiple variable request received (on server)
***************************************************************************************************/
static mem_status_t Load_EATTServerIncomingClientReadMultipleVariableRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTServerIncomingClientReadMultipleVariableRequestIndication_t *evt = &(container->Data.EATTServerIncomingClientReadMultipleVariableRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTServerIncomingClientReadMultipleVariableRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.HandleCount), pPayload + idx, sizeof(evt->Params.HandleCount)); idx += sizeof(evt->Params.HandleCount);

	if (evt->Params.HandleCount > 0)
	{
		evt->Params.ListOfHandles = MEM_BufferAlloc(evt->Params.HandleCount * 2);

		if (!evt->Params.ListOfHandles)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.ListOfHandles = NULL;
	}

	FLib_MemCpy(evt->Params.ListOfHandles, pPayload + idx, evt->Params.HandleCount * 2); idx += evt->Params.HandleCount * 2;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_EATTClientIncomingServerMultipleHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	EATT multiple handle notification received (on Client)
***************************************************************************************************/
static mem_status_t Load_EATTClientIncomingServerMultipleHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	EATTClientIncomingServerMultipleHandleValueNotificationIndication_t *evt = &(container->Data.EATTClientIncomingServerMultipleHandleValueNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = EATTClientIncomingServerMultipleHandleValueNotificationIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->TotalLength), pPayload + idx, sizeof(evt->TotalLength)); idx += sizeof(evt->TotalLength);

	if (evt->TotalLength > 0)
	{
		evt->HandleLengthValueList = MEM_BufferAlloc(evt->TotalLength);

		if (!evt->HandleLengthValueList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->HandleLengthValueList = NULL;
	}

	FLib_MemCpy(evt->HandleLengthValueList, pPayload + idx, evt->TotalLength); idx += evt->TotalLength;

	return kStatus_MemSuccess;
}

#endif  /* ATT_ENABLE */

#if GATT_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the GATT request
***************************************************************************************************/
static mem_status_t Load_GATTConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTConfirm_t *evt = &(container->Data.GATTConfirm);

	/* Store (OG, OC) in ID */
	container->id = GATTConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTGetMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies the MTU used with a given connected device
***************************************************************************************************/
static mem_status_t Load_GATTGetMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTGetMtuIndication_t *evt = &(container->Data.GATTGetMtuIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTGetMtuIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTGetMtuIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureExchangeMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the MTU exchange procedure ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureExchangeMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureExchangeMtuIndication_t *evt = &(container->Data.GATTClientProcedureExchangeMtuIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureExchangeMtuIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTClientProcedureExchangeMtuIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureDiscoverAllPrimaryServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Primary Service Discovery procedure ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureDiscoverAllPrimaryServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureDiscoverAllPrimaryServicesIndication_t *evt = &(container->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureDiscoverAllPrimaryServicesIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureDiscoverAllPrimaryServicesIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfDiscoveredServices = pPayload[idx]; idx++;

	if (evt->NbOfDiscoveredServices > 0)
	{
		evt->DiscoveredServices = MEM_BufferAlloc(evt->NbOfDiscoveredServices * sizeof(evt->DiscoveredServices[0]));

		if (!evt->DiscoveredServices)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->DiscoveredServices = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
	{
		FLib_MemCpy(&(evt->DiscoveredServices[i].StartHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].StartHandle)); idx += sizeof(evt->DiscoveredServices[i].StartHandle);
		FLib_MemCpy(&(evt->DiscoveredServices[i].EndHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].EndHandle)); idx += sizeof(evt->DiscoveredServices[i].EndHandle);
		evt->DiscoveredServices[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->DiscoveredServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		evt->DiscoveredServices[i].NbOfCharacteristics = pPayload[idx]; idx++;

		if (evt->DiscoveredServices[i].NbOfCharacteristics > 0)
		{
			evt->DiscoveredServices[i].Characteristics = MEM_BufferAlloc(evt->DiscoveredServices[i].NbOfCharacteristics * sizeof(evt->DiscoveredServices[i].Characteristics[0]));

			if (!evt->DiscoveredServices[i].Characteristics)
			{
				MEM_BufferFree(evt->DiscoveredServices);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->DiscoveredServices[i].Characteristics = NULL;
		}


		for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
		{
			evt->DiscoveredServices[i].Characteristics[j].Properties = (Properties_t)pPayload[idx]; idx++;
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.Handle), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.Handle)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.Handle);
			evt->DiscoveredServices[i].Characteristics[j].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->DiscoveredServices[i].Characteristics[j].Value.UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength);
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength);

			if (evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength > 0)
			{
				evt->DiscoveredServices[i].Characteristics[j].Value.Value = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength);

				if (!evt->DiscoveredServices[i].Characteristics[j].Value.Value)
				{
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
					}
					MEM_BufferFree(evt->DiscoveredServices);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->DiscoveredServices[i].Characteristics[j].Value.Value = NULL;
			}

			FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Value, pPayload + idx, evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength); idx += evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength;
			evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors = pPayload[idx]; idx++;

			if (evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors > 0)
			{
				evt->DiscoveredServices[i].Characteristics[j].Descriptors = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors * sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[0]));

				if (!evt->DiscoveredServices[i].Characteristics[j].Descriptors)
				{
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
						}
					}
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
					}
					MEM_BufferFree(evt->DiscoveredServices);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->DiscoveredServices[i].Characteristics[j].Descriptors = NULL;
			}


			for (uint32_t k = 0; k < evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors; k++)
			{
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle);
				evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].UuidType = (UuidType_t)pPayload[idx]; idx++;

				switch (evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].UuidType)
				{
					case Uuid16Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
						break;

					case Uuid128Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
						break;

					case Uuid32Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
						break;
				}
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength);
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength);

				if (evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength > 0)
				{
					evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength);

					if (!evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value)
					{
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
							{
								MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors);
							}
						}
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
							{
								MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
							}
						}
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
						}
						MEM_BufferFree(evt->DiscoveredServices);
						return kStatus_MemAllocError;
					}

				}
				else
				{
					evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value = NULL;
				}

				FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value, pPayload + idx, evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength); idx += evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength;
			}
		}
		evt->DiscoveredServices[i].NbOfIncludedServices = pPayload[idx]; idx++;

		if (evt->DiscoveredServices[i].NbOfIncludedServices > 0)
		{
			evt->DiscoveredServices[i].IncludedServices = MEM_BufferAlloc(evt->DiscoveredServices[i].NbOfIncludedServices * sizeof(evt->DiscoveredServices[i].IncludedServices[0]));

			if (!evt->DiscoveredServices[i].IncludedServices)
			{
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						for (uint32_t k = 0; k < evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors; k++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value);
						}
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors);
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
				}
				MEM_BufferFree(evt->DiscoveredServices);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->DiscoveredServices[i].IncludedServices = NULL;
		}


		for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfIncludedServices; j++)
		{
			FLib_MemCpy(&(evt->DiscoveredServices[i].IncludedServices[j].StartHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].IncludedServices[j].StartHandle)); idx += sizeof(evt->DiscoveredServices[i].IncludedServices[j].StartHandle);
			FLib_MemCpy(&(evt->DiscoveredServices[i].IncludedServices[j].EndHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].IncludedServices[j].EndHandle)); idx += sizeof(evt->DiscoveredServices[i].IncludedServices[j].EndHandle);
			evt->DiscoveredServices[i].IncludedServices[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->DiscoveredServices[i].IncludedServices[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			evt->DiscoveredServices[i].IncludedServices[j].NbOfCharacteristics = pPayload[idx]; idx++;
			evt->DiscoveredServices[i].IncludedServices[j].NbOfIncludedServices = pPayload[idx]; idx++;
		}
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureDiscoverPrimaryServicesByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Primary Service Discovery By UUID procedure ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureDiscoverPrimaryServicesByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_t *evt = &(container->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfDiscoveredServices = pPayload[idx]; idx++;

	if (evt->NbOfDiscoveredServices > 0)
	{
		evt->DiscoveredServices = MEM_BufferAlloc(evt->NbOfDiscoveredServices * sizeof(evt->DiscoveredServices[0]));

		if (!evt->DiscoveredServices)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->DiscoveredServices = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
	{
		FLib_MemCpy(&(evt->DiscoveredServices[i].StartHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].StartHandle)); idx += sizeof(evt->DiscoveredServices[i].StartHandle);
		FLib_MemCpy(&(evt->DiscoveredServices[i].EndHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].EndHandle)); idx += sizeof(evt->DiscoveredServices[i].EndHandle);
		evt->DiscoveredServices[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->DiscoveredServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		evt->DiscoveredServices[i].NbOfCharacteristics = pPayload[idx]; idx++;

		if (evt->DiscoveredServices[i].NbOfCharacteristics > 0)
		{
			evt->DiscoveredServices[i].Characteristics = MEM_BufferAlloc(evt->DiscoveredServices[i].NbOfCharacteristics * sizeof(evt->DiscoveredServices[i].Characteristics[0]));

			if (!evt->DiscoveredServices[i].Characteristics)
			{
				MEM_BufferFree(evt->DiscoveredServices);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->DiscoveredServices[i].Characteristics = NULL;
		}


		for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
		{
			evt->DiscoveredServices[i].Characteristics[j].Properties = (Properties_t)pPayload[idx]; idx++;
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.Handle), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.Handle)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.Handle);
			evt->DiscoveredServices[i].Characteristics[j].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->DiscoveredServices[i].Characteristics[j].Value.UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength);
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength);

			if (evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength > 0)
			{
				evt->DiscoveredServices[i].Characteristics[j].Value.Value = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength);

				if (!evt->DiscoveredServices[i].Characteristics[j].Value.Value)
				{
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
					}
					MEM_BufferFree(evt->DiscoveredServices);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->DiscoveredServices[i].Characteristics[j].Value.Value = NULL;
			}

			FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Value, pPayload + idx, evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength); idx += evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength;
			evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors = pPayload[idx]; idx++;

			if (evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors > 0)
			{
				evt->DiscoveredServices[i].Characteristics[j].Descriptors = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors * sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[0]));

				if (!evt->DiscoveredServices[i].Characteristics[j].Descriptors)
				{
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
						}
					}
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
					}
					MEM_BufferFree(evt->DiscoveredServices);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->DiscoveredServices[i].Characteristics[j].Descriptors = NULL;
			}


			for (uint32_t k = 0; k < evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors; k++)
			{
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle);
				evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].UuidType = (UuidType_t)pPayload[idx]; idx++;

				switch (evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].UuidType)
				{
					case Uuid16Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
						break;

					case Uuid128Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
						break;

					case Uuid32Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
						break;
				}
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength);
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength);

				if (evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength > 0)
				{
					evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength);

					if (!evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value)
					{
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
							{
								MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors);
							}
						}
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
							{
								MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
							}
						}
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
						}
						MEM_BufferFree(evt->DiscoveredServices);
						return kStatus_MemAllocError;
					}

				}
				else
				{
					evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value = NULL;
				}

				FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value, pPayload + idx, evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength); idx += evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength;
			}
		}
		evt->DiscoveredServices[i].NbOfIncludedServices = pPayload[idx]; idx++;

		if (evt->DiscoveredServices[i].NbOfIncludedServices > 0)
		{
			evt->DiscoveredServices[i].IncludedServices = MEM_BufferAlloc(evt->DiscoveredServices[i].NbOfIncludedServices * sizeof(evt->DiscoveredServices[i].IncludedServices[0]));

			if (!evt->DiscoveredServices[i].IncludedServices)
			{
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						for (uint32_t k = 0; k < evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors; k++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value);
						}
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors);
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
				}
				MEM_BufferFree(evt->DiscoveredServices);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->DiscoveredServices[i].IncludedServices = NULL;
		}


		for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfIncludedServices; j++)
		{
			FLib_MemCpy(&(evt->DiscoveredServices[i].IncludedServices[j].StartHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].IncludedServices[j].StartHandle)); idx += sizeof(evt->DiscoveredServices[i].IncludedServices[j].StartHandle);
			FLib_MemCpy(&(evt->DiscoveredServices[i].IncludedServices[j].EndHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].IncludedServices[j].EndHandle)); idx += sizeof(evt->DiscoveredServices[i].IncludedServices[j].EndHandle);
			evt->DiscoveredServices[i].IncludedServices[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->DiscoveredServices[i].IncludedServices[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			evt->DiscoveredServices[i].IncludedServices[j].NbOfCharacteristics = pPayload[idx]; idx++;
			evt->DiscoveredServices[i].IncludedServices[j].NbOfIncludedServices = pPayload[idx]; idx++;
		}
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureFindIncludedServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Find Included Services procedure ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureFindIncludedServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureFindIncludedServicesIndication_t *evt = &(container->Data.GATTClientProcedureFindIncludedServicesIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureFindIncludedServicesIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureFindIncludedServicesIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->Service.StartHandle), pPayload + idx, sizeof(evt->Service.StartHandle)); idx += sizeof(evt->Service.StartHandle);
	FLib_MemCpy(&(evt->Service.EndHandle), pPayload + idx, sizeof(evt->Service.EndHandle)); idx += sizeof(evt->Service.EndHandle);
	evt->Service.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Service.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	evt->Service.NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->Service.NbOfCharacteristics > 0)
	{
		evt->Service.Characteristics = MEM_BufferAlloc(evt->Service.NbOfCharacteristics * sizeof(evt->Service.Characteristics[0]));

		if (!evt->Service.Characteristics)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Service.Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
	{
		evt->Service.Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.Handle)); idx += sizeof(evt->Service.Characteristics[i].Value.Handle);
		evt->Service.Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Service.Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Service.Characteristics[i].Value.MaxValueLength);

		if (evt->Service.Characteristics[i].Value.ValueLength > 0)
		{
			evt->Service.Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Service.Characteristics[i].Value.ValueLength);

			if (!evt->Service.Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Service.Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Service.Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Service.Characteristics[i].Value.Value, pPayload + idx, evt->Service.Characteristics[i].Value.ValueLength); idx += evt->Service.Characteristics[i].Value.ValueLength;
		evt->Service.Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Service.Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Service.Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Service.Characteristics[i].NbOfDescriptors * sizeof(evt->Service.Characteristics[i].Descriptors[0]));

			if (!evt->Service.Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Service.Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Service.Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].Handle);
			evt->Service.Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Service.Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Service.Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Service.Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Service.Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Service.Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Service.Characteristics);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Service.Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Service.Characteristics[i].Descriptors[j].ValueLength); idx += evt->Service.Characteristics[i].Descriptors[j].ValueLength;
		}
	}
	evt->Service.NbOfIncludedServices = pPayload[idx]; idx++;

	if (evt->Service.NbOfIncludedServices > 0)
	{
		evt->Service.IncludedServices = MEM_BufferAlloc(evt->Service.NbOfIncludedServices * sizeof(evt->Service.IncludedServices[0]));

		if (!evt->Service.IncludedServices)
		{
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				for (uint32_t j = 0; j < evt->Service.Characteristics[i].NbOfDescriptors; j++)
				{
					MEM_BufferFree(evt->Service.Characteristics[i].Descriptors[j].Value);
				}
			}
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				MEM_BufferFree(evt->Service.Characteristics[i].Descriptors);
			}
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
			}
			MEM_BufferFree(evt->Service.Characteristics);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Service.IncludedServices = NULL;
	}


	for (uint32_t i = 0; i < evt->Service.NbOfIncludedServices; i++)
	{
		FLib_MemCpy(&(evt->Service.IncludedServices[i].StartHandle), pPayload + idx, sizeof(evt->Service.IncludedServices[i].StartHandle)); idx += sizeof(evt->Service.IncludedServices[i].StartHandle);
		FLib_MemCpy(&(evt->Service.IncludedServices[i].EndHandle), pPayload + idx, sizeof(evt->Service.IncludedServices[i].EndHandle)); idx += sizeof(evt->Service.IncludedServices[i].EndHandle);
		evt->Service.IncludedServices[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		evt->Service.IncludedServices[i].NbOfCharacteristics = pPayload[idx]; idx++;
		evt->Service.IncludedServices[i].NbOfIncludedServices = pPayload[idx]; idx++;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureDiscoverAllCharacteristicsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Discovery procedure for a given service ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureDiscoverAllCharacteristicsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureDiscoverAllCharacteristicsIndication_t *evt = &(container->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureDiscoverAllCharacteristicsIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureDiscoverAllCharacteristicsIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->Service.StartHandle), pPayload + idx, sizeof(evt->Service.StartHandle)); idx += sizeof(evt->Service.StartHandle);
	FLib_MemCpy(&(evt->Service.EndHandle), pPayload + idx, sizeof(evt->Service.EndHandle)); idx += sizeof(evt->Service.EndHandle);
	evt->Service.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Service.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	evt->Service.NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->Service.NbOfCharacteristics > 0)
	{
		evt->Service.Characteristics = MEM_BufferAlloc(evt->Service.NbOfCharacteristics * sizeof(evt->Service.Characteristics[0]));

		if (!evt->Service.Characteristics)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Service.Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
	{
		evt->Service.Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.Handle)); idx += sizeof(evt->Service.Characteristics[i].Value.Handle);
		evt->Service.Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Service.Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Service.Characteristics[i].Value.MaxValueLength);

		if (evt->Service.Characteristics[i].Value.ValueLength > 0)
		{
			evt->Service.Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Service.Characteristics[i].Value.ValueLength);

			if (!evt->Service.Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Service.Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Service.Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Service.Characteristics[i].Value.Value, pPayload + idx, evt->Service.Characteristics[i].Value.ValueLength); idx += evt->Service.Characteristics[i].Value.ValueLength;
		evt->Service.Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Service.Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Service.Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Service.Characteristics[i].NbOfDescriptors * sizeof(evt->Service.Characteristics[i].Descriptors[0]));

			if (!evt->Service.Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Service.Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Service.Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].Handle);
			evt->Service.Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Service.Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Service.Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Service.Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Service.Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Service.Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Service.Characteristics);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Service.Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Service.Characteristics[i].Descriptors[j].ValueLength); idx += evt->Service.Characteristics[i].Descriptors[j].ValueLength;
		}
	}
	evt->Service.NbOfIncludedServices = pPayload[idx]; idx++;

	if (evt->Service.NbOfIncludedServices > 0)
	{
		evt->Service.IncludedServices = MEM_BufferAlloc(evt->Service.NbOfIncludedServices * sizeof(evt->Service.IncludedServices[0]));

		if (!evt->Service.IncludedServices)
		{
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				for (uint32_t j = 0; j < evt->Service.Characteristics[i].NbOfDescriptors; j++)
				{
					MEM_BufferFree(evt->Service.Characteristics[i].Descriptors[j].Value);
				}
			}
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				MEM_BufferFree(evt->Service.Characteristics[i].Descriptors);
			}
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
			}
			MEM_BufferFree(evt->Service.Characteristics);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Service.IncludedServices = NULL;
	}


	for (uint32_t i = 0; i < evt->Service.NbOfIncludedServices; i++)
	{
		FLib_MemCpy(&(evt->Service.IncludedServices[i].StartHandle), pPayload + idx, sizeof(evt->Service.IncludedServices[i].StartHandle)); idx += sizeof(evt->Service.IncludedServices[i].StartHandle);
		FLib_MemCpy(&(evt->Service.IncludedServices[i].EndHandle), pPayload + idx, sizeof(evt->Service.IncludedServices[i].EndHandle)); idx += sizeof(evt->Service.IncludedServices[i].EndHandle);
		evt->Service.IncludedServices[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		evt->Service.IncludedServices[i].NbOfCharacteristics = pPayload[idx]; idx++;
		evt->Service.IncludedServices[i].NbOfIncludedServices = pPayload[idx]; idx++;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureDiscoverCharacteristicByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Discovery procedure for a given service (with a given UUID) ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureDiscoverCharacteristicByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureDiscoverCharacteristicByUuidIndication_t *evt = &(container->Data.GATTClientProcedureDiscoverCharacteristicByUuidIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureDiscoverCharacteristicByUuidIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureDiscoverCharacteristicByUuidIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->NbOfCharacteristics > 0)
	{
		evt->Characteristics = MEM_BufferAlloc(evt->NbOfCharacteristics * sizeof(evt->Characteristics[0]));

		if (!evt->Characteristics)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
	{
		evt->Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Characteristics[i].Value.Handle)); idx += sizeof(evt->Characteristics[i].Value.Handle);
		evt->Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Characteristics[i].Value.MaxValueLength);

		if (evt->Characteristics[i].Value.ValueLength > 0)
		{
			evt->Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Characteristics[i].Value.ValueLength);

			if (!evt->Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Characteristics[i].Value.Value, pPayload + idx, evt->Characteristics[i].Value.ValueLength); idx += evt->Characteristics[i].Value.ValueLength;
		evt->Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Characteristics[i].NbOfDescriptors * sizeof(evt->Characteristics[i].Descriptors[0]));

			if (!evt->Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Characteristics[i].Descriptors[j].Handle);
			evt->Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Characteristics);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Characteristics[i].Descriptors[j].ValueLength); idx += evt->Characteristics[i].Descriptors[j].ValueLength;
		}
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Descriptor Discovery procedure for a given characteristic ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_t *evt = &(container->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->Characteristic.Properties = (Properties_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Characteristic.Value.Handle), pPayload + idx, sizeof(evt->Characteristic.Value.Handle)); idx += sizeof(evt->Characteristic.Value.Handle);
	evt->Characteristic.Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	FLib_MemCpy(&(evt->Characteristic.Value.ValueLength), pPayload + idx, sizeof(evt->Characteristic.Value.ValueLength)); idx += sizeof(evt->Characteristic.Value.ValueLength);
	FLib_MemCpy(&(evt->Characteristic.Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristic.Value.MaxValueLength)); idx += sizeof(evt->Characteristic.Value.MaxValueLength);

	if (evt->Characteristic.Value.ValueLength > 0)
	{
		evt->Characteristic.Value.Value = MEM_BufferAlloc(evt->Characteristic.Value.ValueLength);

		if (!evt->Characteristic.Value.Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristic.Value.Value = NULL;
	}

	FLib_MemCpy(evt->Characteristic.Value.Value, pPayload + idx, evt->Characteristic.Value.ValueLength); idx += evt->Characteristic.Value.ValueLength;
	evt->Characteristic.NbOfDescriptors = pPayload[idx]; idx++;

	if (evt->Characteristic.NbOfDescriptors > 0)
	{
		evt->Characteristic.Descriptors = MEM_BufferAlloc(evt->Characteristic.NbOfDescriptors * sizeof(evt->Characteristic.Descriptors[0]));

		if (!evt->Characteristic.Descriptors)
		{
			MEM_BufferFree(evt->Characteristic.Value.Value);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristic.Descriptors = NULL;
	}


	for (uint32_t i = 0; i < evt->Characteristic.NbOfDescriptors; i++)
	{
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].Handle), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].Handle)); idx += sizeof(evt->Characteristic.Descriptors[i].Handle);
		evt->Characteristic.Descriptors[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].ValueLength), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].ValueLength)); idx += sizeof(evt->Characteristic.Descriptors[i].ValueLength);
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].MaxValueLength), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].MaxValueLength)); idx += sizeof(evt->Characteristic.Descriptors[i].MaxValueLength);

		if (evt->Characteristic.Descriptors[i].ValueLength > 0)
		{
			evt->Characteristic.Descriptors[i].Value = MEM_BufferAlloc(evt->Characteristic.Descriptors[i].ValueLength);

			if (!evt->Characteristic.Descriptors[i].Value)
			{
				MEM_BufferFree(evt->Characteristic.Descriptors);
				MEM_BufferFree(evt->Characteristic.Value.Value);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristic.Descriptors[i].Value = NULL;
		}

		FLib_MemCpy(evt->Characteristic.Descriptors[i].Value, pPayload + idx, evt->Characteristic.Descriptors[i].ValueLength); idx += evt->Characteristic.Descriptors[i].ValueLength;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureReadCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Read procedure for a given characteristic ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureReadCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureReadCharacteristicValueIndication_t *evt = &(container->Data.GATTClientProcedureReadCharacteristicValueIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureReadCharacteristicValueIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureReadCharacteristicValueIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->Characteristic.Properties = (Properties_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Characteristic.Value.Handle), pPayload + idx, sizeof(evt->Characteristic.Value.Handle)); idx += sizeof(evt->Characteristic.Value.Handle);
	evt->Characteristic.Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	FLib_MemCpy(&(evt->Characteristic.Value.ValueLength), pPayload + idx, sizeof(evt->Characteristic.Value.ValueLength)); idx += sizeof(evt->Characteristic.Value.ValueLength);
	FLib_MemCpy(&(evt->Characteristic.Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristic.Value.MaxValueLength)); idx += sizeof(evt->Characteristic.Value.MaxValueLength);

	if (evt->Characteristic.Value.ValueLength > 0)
	{
		evt->Characteristic.Value.Value = MEM_BufferAlloc(evt->Characteristic.Value.ValueLength);

		if (!evt->Characteristic.Value.Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristic.Value.Value = NULL;
	}

	FLib_MemCpy(evt->Characteristic.Value.Value, pPayload + idx, evt->Characteristic.Value.ValueLength); idx += evt->Characteristic.Value.ValueLength;
	evt->Characteristic.NbOfDescriptors = pPayload[idx]; idx++;

	if (evt->Characteristic.NbOfDescriptors > 0)
	{
		evt->Characteristic.Descriptors = MEM_BufferAlloc(evt->Characteristic.NbOfDescriptors * sizeof(evt->Characteristic.Descriptors[0]));

		if (!evt->Characteristic.Descriptors)
		{
			MEM_BufferFree(evt->Characteristic.Value.Value);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristic.Descriptors = NULL;
	}


	for (uint32_t i = 0; i < evt->Characteristic.NbOfDescriptors; i++)
	{
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].Handle), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].Handle)); idx += sizeof(evt->Characteristic.Descriptors[i].Handle);
		evt->Characteristic.Descriptors[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].ValueLength), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].ValueLength)); idx += sizeof(evt->Characteristic.Descriptors[i].ValueLength);
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].MaxValueLength), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].MaxValueLength)); idx += sizeof(evt->Characteristic.Descriptors[i].MaxValueLength);

		if (evt->Characteristic.Descriptors[i].ValueLength > 0)
		{
			evt->Characteristic.Descriptors[i].Value = MEM_BufferAlloc(evt->Characteristic.Descriptors[i].ValueLength);

			if (!evt->Characteristic.Descriptors[i].Value)
			{
				MEM_BufferFree(evt->Characteristic.Descriptors);
				MEM_BufferFree(evt->Characteristic.Value.Value);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristic.Descriptors[i].Value = NULL;
		}

		FLib_MemCpy(evt->Characteristic.Descriptors[i].Value, pPayload + idx, evt->Characteristic.Descriptors[i].ValueLength); idx += evt->Characteristic.Descriptors[i].ValueLength;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureReadUsingCharacteristicUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Read By UUID procedure ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureReadUsingCharacteristicUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureReadUsingCharacteristicUuidIndication_t *evt = &(container->Data.GATTClientProcedureReadUsingCharacteristicUuidIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureReadUsingCharacteristicUuidIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureReadUsingCharacteristicUuidIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->NbOfReadBytes), pPayload + idx, sizeof(evt->NbOfReadBytes)); idx += sizeof(evt->NbOfReadBytes);

	if (evt->NbOfReadBytes > 0)
	{
		evt->ReadBytes = MEM_BufferAlloc(evt->NbOfReadBytes);

		if (!evt->ReadBytes)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->ReadBytes = NULL;
	}

	FLib_MemCpy(evt->ReadBytes, pPayload + idx, evt->NbOfReadBytes); idx += evt->NbOfReadBytes;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureReadMultipleCharacteristicValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Read Multiple procedure ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureReadMultipleCharacteristicValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureReadMultipleCharacteristicValuesIndication_t *evt = &(container->Data.GATTClientProcedureReadMultipleCharacteristicValuesIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureReadMultipleCharacteristicValuesIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureReadMultipleCharacteristicValuesIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->NbOfCharacteristics > 0)
	{
		evt->Characteristics = MEM_BufferAlloc(evt->NbOfCharacteristics * sizeof(evt->Characteristics[0]));

		if (!evt->Characteristics)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
	{
		evt->Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Characteristics[i].Value.Handle)); idx += sizeof(evt->Characteristics[i].Value.Handle);
		evt->Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Characteristics[i].Value.MaxValueLength);

		if (evt->Characteristics[i].Value.ValueLength > 0)
		{
			evt->Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Characteristics[i].Value.ValueLength);

			if (!evt->Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Characteristics[i].Value.Value, pPayload + idx, evt->Characteristics[i].Value.ValueLength); idx += evt->Characteristics[i].Value.ValueLength;
		evt->Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Characteristics[i].NbOfDescriptors * sizeof(evt->Characteristics[i].Descriptors[0]));

			if (!evt->Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Characteristics[i].Descriptors[j].Handle);
			evt->Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Characteristics);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Characteristics[i].Descriptors[j].ValueLength); idx += evt->Characteristics[i].Descriptors[j].ValueLength;
		}
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureWriteCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Write procedure for a given characteristic ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureWriteCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureWriteCharacteristicValueIndication_t *evt = &(container->Data.GATTClientProcedureWriteCharacteristicValueIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureWriteCharacteristicValueIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTClientProcedureWriteCharacteristicValueIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureReadCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Descriptor Read procedure for a given characteristic's descriptor ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureReadCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureReadCharacteristicDescriptorIndication_t *evt = &(container->Data.GATTClientProcedureReadCharacteristicDescriptorIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureReadCharacteristicDescriptorIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureReadCharacteristicDescriptorIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->Descriptor.Handle), pPayload + idx, sizeof(evt->Descriptor.Handle)); idx += sizeof(evt->Descriptor.Handle);
	evt->Descriptor.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Descriptor.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Descriptor.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Descriptor.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Descriptor.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	FLib_MemCpy(&(evt->Descriptor.ValueLength), pPayload + idx, sizeof(evt->Descriptor.ValueLength)); idx += sizeof(evt->Descriptor.ValueLength);
	FLib_MemCpy(&(evt->Descriptor.MaxValueLength), pPayload + idx, sizeof(evt->Descriptor.MaxValueLength)); idx += sizeof(evt->Descriptor.MaxValueLength);

	if (evt->Descriptor.ValueLength > 0)
	{
		evt->Descriptor.Value = MEM_BufferAlloc(evt->Descriptor.ValueLength);

		if (!evt->Descriptor.Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Descriptor.Value = NULL;
	}

	FLib_MemCpy(evt->Descriptor.Value, pPayload + idx, evt->Descriptor.ValueLength); idx += evt->Descriptor.ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureWriteCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Descriptor Write procedure for a given characteristic's descriptor ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureWriteCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureWriteCharacteristicDescriptorIndication_t *evt = &(container->Data.GATTClientProcedureWriteCharacteristicDescriptorIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureWriteCharacteristicDescriptorIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTClientProcedureWriteCharacteristicDescriptorIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Client notification
***************************************************************************************************/
static mem_status_t Load_GATTClientNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientNotificationIndication_t *evt = &(container->Data.GATTClientNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientNotificationIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->CharacteristicValueHandle), pPayload + idx, sizeof(evt->CharacteristicValueHandle)); idx += sizeof(evt->CharacteristicValueHandle);
	FLib_MemCpy(&(evt->ValueLength), pPayload + idx, sizeof(evt->ValueLength)); idx += sizeof(evt->ValueLength);

	if (evt->ValueLength > 0)
	{
		evt->Value = MEM_BufferAlloc(evt->ValueLength);

		if (!evt->Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Value = NULL;
	}

	FLib_MemCpy(evt->Value, pPayload + idx, evt->ValueLength); idx += evt->ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientMultipleHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Client notification
***************************************************************************************************/
static mem_status_t Load_GATTClientMultipleHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientMultipleHandleValueNotificationIndication_t *evt = &(container->Data.GATTClientMultipleHandleValueNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientMultipleHandleValueNotificationIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->HandleCount), pPayload + idx, sizeof(evt->HandleCount)); idx += sizeof(evt->HandleCount);

	if (evt->HandleCount > 0)
	{
		evt->HandleLengthValueList = MEM_BufferAlloc(evt->HandleCount * sizeof(evt->HandleLengthValueList[0]));

		if (!evt->HandleLengthValueList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->HandleLengthValueList = NULL;
	}


	for (uint32_t i = 0; i < evt->HandleCount; i++)
	{
		FLib_MemCpy(&(evt->HandleLengthValueList[i].Handle), pPayload + idx, sizeof(evt->HandleLengthValueList[i].Handle)); idx += sizeof(evt->HandleLengthValueList[i].Handle);
		FLib_MemCpy(&(evt->HandleLengthValueList[i].ValueLength), pPayload + idx, sizeof(evt->HandleLengthValueList[i].ValueLength)); idx += sizeof(evt->HandleLengthValueList[i].ValueLength);

		if (evt->HandleLengthValueList[i].ValueLength > 0)
		{
			evt->HandleLengthValueList[i].Value = MEM_BufferAlloc(evt->HandleLengthValueList[i].ValueLength);

			if (!evt->HandleLengthValueList[i].Value)
			{
				MEM_BufferFree(evt->HandleLengthValueList);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->HandleLengthValueList[i].Value = NULL;
		}

		FLib_MemCpy(evt->HandleLengthValueList[i].Value, pPayload + idx, evt->HandleLengthValueList[i].ValueLength); idx += evt->HandleLengthValueList[i].ValueLength;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureReadMultipleVariableLenCharValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Client Read Multiple Variable Length Characteristic Values
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureReadMultipleVariableLenCharValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureReadMultipleVariableLenCharValuesIndication_t *evt = &(container->Data.GATTClientProcedureReadMultipleVariableLenCharValuesIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureReadMultipleVariableLenCharValuesIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureReadMultipleVariableLenCharValuesIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->NbOfCharacteristics > 0)
	{
		evt->Characteristics = MEM_BufferAlloc(evt->NbOfCharacteristics * sizeof(evt->Characteristics[0]));

		if (!evt->Characteristics)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
	{
		evt->Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Characteristics[i].Value.Handle)); idx += sizeof(evt->Characteristics[i].Value.Handle);
		evt->Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Characteristics[i].Value.MaxValueLength);

		if (evt->Characteristics[i].Value.ValueLength > 0)
		{
			evt->Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Characteristics[i].Value.ValueLength);

			if (!evt->Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Characteristics[i].Value.Value, pPayload + idx, evt->Characteristics[i].Value.ValueLength); idx += evt->Characteristics[i].Value.ValueLength;
		evt->Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Characteristics[i].NbOfDescriptors * sizeof(evt->Characteristics[i].Descriptors[0]));

			if (!evt->Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Characteristics[i].Descriptors[j].Handle);
			evt->Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Characteristics);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Characteristics[i].Descriptors[j].ValueLength); idx += evt->Characteristics[i].Descriptors[j].ValueLength;
		}
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Client indication
***************************************************************************************************/
static mem_status_t Load_GATTClientIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientIndicationIndication_t *evt = &(container->Data.GATTClientIndicationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientIndicationIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->CharacteristicValueHandle), pPayload + idx, sizeof(evt->CharacteristicValueHandle)); idx += sizeof(evt->CharacteristicValueHandle);
	FLib_MemCpy(&(evt->ValueLength), pPayload + idx, sizeof(evt->ValueLength)); idx += sizeof(evt->ValueLength);

	if (evt->ValueLength > 0)
	{
		evt->Value = MEM_BufferAlloc(evt->ValueLength);

		if (!evt->Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Value = NULL;
	}

	FLib_MemCpy(evt->Value, pPayload + idx, evt->ValueLength); idx += evt->ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerMtuChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server MTU changed indication
***************************************************************************************************/
static mem_status_t Load_GATTServerMtuChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerMtuChangedIndication_t *evt = &(container->Data.GATTServerMtuChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTServerMtuChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerMtuChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server handle value confirmation
***************************************************************************************************/
static mem_status_t Load_GATTServerHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerHandleValueConfirmationIndication_t *evt = &(container->Data.GATTServerHandleValueConfirmationIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTServerHandleValueConfirmationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerHandleValueConfirmationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerAttributeWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server attribute written
***************************************************************************************************/
static mem_status_t Load_GATTServerAttributeWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerAttributeWrittenIndication_t *evt = &(container->Data.GATTServerAttributeWrittenIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTServerAttributeWrittenIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->AttributeWrittenEvent.Handle), pPayload + idx, sizeof(evt->AttributeWrittenEvent.Handle)); idx += sizeof(evt->AttributeWrittenEvent.Handle);
	FLib_MemCpy(&(evt->AttributeWrittenEvent.ValueLength), pPayload + idx, sizeof(evt->AttributeWrittenEvent.ValueLength)); idx += sizeof(evt->AttributeWrittenEvent.ValueLength);

	if (evt->AttributeWrittenEvent.ValueLength > 0)
	{
		evt->AttributeWrittenEvent.Value = MEM_BufferAlloc(evt->AttributeWrittenEvent.ValueLength);

		if (!evt->AttributeWrittenEvent.Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->AttributeWrittenEvent.Value = NULL;
	}

	FLib_MemCpy(evt->AttributeWrittenEvent.Value, pPayload + idx, evt->AttributeWrittenEvent.ValueLength); idx += evt->AttributeWrittenEvent.ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerCharacteristicCccdWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server characteristic cccd written
***************************************************************************************************/
static mem_status_t Load_GATTServerCharacteristicCccdWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerCharacteristicCccdWrittenIndication_t *evt = &(container->Data.GATTServerCharacteristicCccdWrittenIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTServerCharacteristicCccdWrittenIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerCharacteristicCccdWrittenIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerAttributeWrittenWithoutResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server attribute written without response
***************************************************************************************************/
static mem_status_t Load_GATTServerAttributeWrittenWithoutResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerAttributeWrittenWithoutResponseIndication_t *evt = &(container->Data.GATTServerAttributeWrittenWithoutResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTServerAttributeWrittenWithoutResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->AttributeWrittenEvent.Handle), pPayload + idx, sizeof(evt->AttributeWrittenEvent.Handle)); idx += sizeof(evt->AttributeWrittenEvent.Handle);
	FLib_MemCpy(&(evt->AttributeWrittenEvent.ValueLength), pPayload + idx, sizeof(evt->AttributeWrittenEvent.ValueLength)); idx += sizeof(evt->AttributeWrittenEvent.ValueLength);

	if (evt->AttributeWrittenEvent.ValueLength > 0)
	{
		evt->AttributeWrittenEvent.Value = MEM_BufferAlloc(evt->AttributeWrittenEvent.ValueLength);

		if (!evt->AttributeWrittenEvent.Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->AttributeWrittenEvent.Value = NULL;
	}

	FLib_MemCpy(evt->AttributeWrittenEvent.Value, pPayload + idx, evt->AttributeWrittenEvent.ValueLength); idx += evt->AttributeWrittenEvent.ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server error
***************************************************************************************************/
static mem_status_t Load_GATTServerErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerErrorIndication_t *evt = &(container->Data.GATTServerErrorIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTServerErrorIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerErrorIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerLongCharacteristicWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server long characteristic written
***************************************************************************************************/
static mem_status_t Load_GATTServerLongCharacteristicWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerLongCharacteristicWrittenIndication_t *evt = &(container->Data.GATTServerLongCharacteristicWrittenIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTServerLongCharacteristicWrittenIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->LongCharacteristicWrittenEvent.Handle), pPayload + idx, sizeof(evt->LongCharacteristicWrittenEvent.Handle)); idx += sizeof(evt->LongCharacteristicWrittenEvent.Handle);
	FLib_MemCpy(&(evt->LongCharacteristicWrittenEvent.ValueLength), pPayload + idx, sizeof(evt->LongCharacteristicWrittenEvent.ValueLength)); idx += sizeof(evt->LongCharacteristicWrittenEvent.ValueLength);

	if (evt->LongCharacteristicWrittenEvent.ValueLength > 0)
	{
		evt->LongCharacteristicWrittenEvent.Value = MEM_BufferAlloc(evt->LongCharacteristicWrittenEvent.ValueLength);

		if (!evt->LongCharacteristicWrittenEvent.Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->LongCharacteristicWrittenEvent.Value = NULL;
	}

	FLib_MemCpy(evt->LongCharacteristicWrittenEvent.Value, pPayload + idx, evt->LongCharacteristicWrittenEvent.ValueLength); idx += evt->LongCharacteristicWrittenEvent.ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerAttributeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server attribute read
***************************************************************************************************/
static mem_status_t Load_GATTServerAttributeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerAttributeReadIndication_t *evt = &(container->Data.GATTServerAttributeReadIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTServerAttributeReadIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerAttributeReadIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedDiscoverAllPrimaryServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Primary Service Discovery procedure ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedDiscoverAllPrimaryServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedDiscoverAllPrimaryServicesIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedDiscoverAllPrimaryServicesIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedDiscoverAllPrimaryServicesIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureEnhancedDiscoverAllPrimaryServicesIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfDiscoveredServices = pPayload[idx]; idx++;

	if (evt->NbOfDiscoveredServices > 0)
	{
		evt->DiscoveredServices = MEM_BufferAlloc(evt->NbOfDiscoveredServices * sizeof(evt->DiscoveredServices[0]));

		if (!evt->DiscoveredServices)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->DiscoveredServices = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
	{
		FLib_MemCpy(&(evt->DiscoveredServices[i].StartHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].StartHandle)); idx += sizeof(evt->DiscoveredServices[i].StartHandle);
		FLib_MemCpy(&(evt->DiscoveredServices[i].EndHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].EndHandle)); idx += sizeof(evt->DiscoveredServices[i].EndHandle);
		evt->DiscoveredServices[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->DiscoveredServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		evt->DiscoveredServices[i].NbOfCharacteristics = pPayload[idx]; idx++;

		if (evt->DiscoveredServices[i].NbOfCharacteristics > 0)
		{
			evt->DiscoveredServices[i].Characteristics = MEM_BufferAlloc(evt->DiscoveredServices[i].NbOfCharacteristics * sizeof(evt->DiscoveredServices[i].Characteristics[0]));

			if (!evt->DiscoveredServices[i].Characteristics)
			{
				MEM_BufferFree(evt->DiscoveredServices);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->DiscoveredServices[i].Characteristics = NULL;
		}


		for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
		{
			evt->DiscoveredServices[i].Characteristics[j].Properties = (Properties_t)pPayload[idx]; idx++;
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.Handle), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.Handle)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.Handle);
			evt->DiscoveredServices[i].Characteristics[j].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->DiscoveredServices[i].Characteristics[j].Value.UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength);
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength);

			if (evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength > 0)
			{
				evt->DiscoveredServices[i].Characteristics[j].Value.Value = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength);

				if (!evt->DiscoveredServices[i].Characteristics[j].Value.Value)
				{
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
					}
					MEM_BufferFree(evt->DiscoveredServices);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->DiscoveredServices[i].Characteristics[j].Value.Value = NULL;
			}

			FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Value, pPayload + idx, evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength); idx += evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength;
			evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors = pPayload[idx]; idx++;

			if (evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors > 0)
			{
				evt->DiscoveredServices[i].Characteristics[j].Descriptors = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors * sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[0]));

				if (!evt->DiscoveredServices[i].Characteristics[j].Descriptors)
				{
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
						}
					}
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
					}
					MEM_BufferFree(evt->DiscoveredServices);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->DiscoveredServices[i].Characteristics[j].Descriptors = NULL;
			}


			for (uint32_t k = 0; k < evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors; k++)
			{
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle);
				evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].UuidType = (UuidType_t)pPayload[idx]; idx++;

				switch (evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].UuidType)
				{
					case Uuid16Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
						break;

					case Uuid128Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
						break;

					case Uuid32Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
						break;
				}
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength);
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength);

				if (evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength > 0)
				{
					evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength);

					if (!evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value)
					{
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
							{
								MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors);
							}
						}
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
							{
								MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
							}
						}
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
						}
						MEM_BufferFree(evt->DiscoveredServices);
						return kStatus_MemAllocError;
					}

				}
				else
				{
					evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value = NULL;
				}

				FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value, pPayload + idx, evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength); idx += evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength;
			}
		}
		evt->DiscoveredServices[i].NbOfIncludedServices = pPayload[idx]; idx++;

		if (evt->DiscoveredServices[i].NbOfIncludedServices > 0)
		{
			evt->DiscoveredServices[i].IncludedServices = MEM_BufferAlloc(evt->DiscoveredServices[i].NbOfIncludedServices * sizeof(evt->DiscoveredServices[i].IncludedServices[0]));

			if (!evt->DiscoveredServices[i].IncludedServices)
			{
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						for (uint32_t k = 0; k < evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors; k++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value);
						}
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors);
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
				}
				MEM_BufferFree(evt->DiscoveredServices);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->DiscoveredServices[i].IncludedServices = NULL;
		}


		for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfIncludedServices; j++)
		{
			FLib_MemCpy(&(evt->DiscoveredServices[i].IncludedServices[j].StartHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].IncludedServices[j].StartHandle)); idx += sizeof(evt->DiscoveredServices[i].IncludedServices[j].StartHandle);
			FLib_MemCpy(&(evt->DiscoveredServices[i].IncludedServices[j].EndHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].IncludedServices[j].EndHandle)); idx += sizeof(evt->DiscoveredServices[i].IncludedServices[j].EndHandle);
			evt->DiscoveredServices[i].IncludedServices[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->DiscoveredServices[i].IncludedServices[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			evt->DiscoveredServices[i].IncludedServices[j].NbOfCharacteristics = pPayload[idx]; idx++;
			evt->DiscoveredServices[i].IncludedServices[j].NbOfIncludedServices = pPayload[idx]; idx++;
		}
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedDiscoverPrimaryServicesByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Primary Service Discovery By UUID procedure ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedDiscoverPrimaryServicesByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedDiscoverPrimaryServicesByUuidIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedDiscoverPrimaryServicesByUuidIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedDiscoverPrimaryServicesByUuidIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureEnhancedDiscoverPrimaryServicesByUuidIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfDiscoveredServices = pPayload[idx]; idx++;

	if (evt->NbOfDiscoveredServices > 0)
	{
		evt->DiscoveredServices = MEM_BufferAlloc(evt->NbOfDiscoveredServices * sizeof(evt->DiscoveredServices[0]));

		if (!evt->DiscoveredServices)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->DiscoveredServices = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
	{
		FLib_MemCpy(&(evt->DiscoveredServices[i].StartHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].StartHandle)); idx += sizeof(evt->DiscoveredServices[i].StartHandle);
		FLib_MemCpy(&(evt->DiscoveredServices[i].EndHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].EndHandle)); idx += sizeof(evt->DiscoveredServices[i].EndHandle);
		evt->DiscoveredServices[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->DiscoveredServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		evt->DiscoveredServices[i].NbOfCharacteristics = pPayload[idx]; idx++;

		if (evt->DiscoveredServices[i].NbOfCharacteristics > 0)
		{
			evt->DiscoveredServices[i].Characteristics = MEM_BufferAlloc(evt->DiscoveredServices[i].NbOfCharacteristics * sizeof(evt->DiscoveredServices[i].Characteristics[0]));

			if (!evt->DiscoveredServices[i].Characteristics)
			{
				MEM_BufferFree(evt->DiscoveredServices);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->DiscoveredServices[i].Characteristics = NULL;
		}


		for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
		{
			evt->DiscoveredServices[i].Characteristics[j].Properties = (Properties_t)pPayload[idx]; idx++;
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.Handle), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.Handle)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.Handle);
			evt->DiscoveredServices[i].Characteristics[j].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->DiscoveredServices[i].Characteristics[j].Value.UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength);
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength);

			if (evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength > 0)
			{
				evt->DiscoveredServices[i].Characteristics[j].Value.Value = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength);

				if (!evt->DiscoveredServices[i].Characteristics[j].Value.Value)
				{
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
					}
					MEM_BufferFree(evt->DiscoveredServices);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->DiscoveredServices[i].Characteristics[j].Value.Value = NULL;
			}

			FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Value, pPayload + idx, evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength); idx += evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength;
			evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors = pPayload[idx]; idx++;

			if (evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors > 0)
			{
				evt->DiscoveredServices[i].Characteristics[j].Descriptors = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors * sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[0]));

				if (!evt->DiscoveredServices[i].Characteristics[j].Descriptors)
				{
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
						}
					}
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
					}
					MEM_BufferFree(evt->DiscoveredServices);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->DiscoveredServices[i].Characteristics[j].Descriptors = NULL;
			}


			for (uint32_t k = 0; k < evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors; k++)
			{
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle);
				evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].UuidType = (UuidType_t)pPayload[idx]; idx++;

				switch (evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].UuidType)
				{
					case Uuid16Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
						break;

					case Uuid128Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
						break;

					case Uuid32Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
						break;
				}
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength);
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength);

				if (evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength > 0)
				{
					evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength);

					if (!evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value)
					{
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
							{
								MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors);
							}
						}
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
							{
								MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
							}
						}
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
						}
						MEM_BufferFree(evt->DiscoveredServices);
						return kStatus_MemAllocError;
					}

				}
				else
				{
					evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value = NULL;
				}

				FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value, pPayload + idx, evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength); idx += evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength;
			}
		}
		evt->DiscoveredServices[i].NbOfIncludedServices = pPayload[idx]; idx++;

		if (evt->DiscoveredServices[i].NbOfIncludedServices > 0)
		{
			evt->DiscoveredServices[i].IncludedServices = MEM_BufferAlloc(evt->DiscoveredServices[i].NbOfIncludedServices * sizeof(evt->DiscoveredServices[i].IncludedServices[0]));

			if (!evt->DiscoveredServices[i].IncludedServices)
			{
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						for (uint32_t k = 0; k < evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors; k++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value);
						}
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors);
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
				}
				MEM_BufferFree(evt->DiscoveredServices);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->DiscoveredServices[i].IncludedServices = NULL;
		}


		for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfIncludedServices; j++)
		{
			FLib_MemCpy(&(evt->DiscoveredServices[i].IncludedServices[j].StartHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].IncludedServices[j].StartHandle)); idx += sizeof(evt->DiscoveredServices[i].IncludedServices[j].StartHandle);
			FLib_MemCpy(&(evt->DiscoveredServices[i].IncludedServices[j].EndHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].IncludedServices[j].EndHandle)); idx += sizeof(evt->DiscoveredServices[i].IncludedServices[j].EndHandle);
			evt->DiscoveredServices[i].IncludedServices[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->DiscoveredServices[i].IncludedServices[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			evt->DiscoveredServices[i].IncludedServices[j].NbOfCharacteristics = pPayload[idx]; idx++;
			evt->DiscoveredServices[i].IncludedServices[j].NbOfIncludedServices = pPayload[idx]; idx++;
		}
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedFindIncludedServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Find Included Services procedure ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedFindIncludedServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedFindIncludedServicesIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedFindIncludedServicesIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedFindIncludedServicesIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureEnhancedFindIncludedServicesIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->Service.StartHandle), pPayload + idx, sizeof(evt->Service.StartHandle)); idx += sizeof(evt->Service.StartHandle);
	FLib_MemCpy(&(evt->Service.EndHandle), pPayload + idx, sizeof(evt->Service.EndHandle)); idx += sizeof(evt->Service.EndHandle);
	evt->Service.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Service.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	evt->Service.NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->Service.NbOfCharacteristics > 0)
	{
		evt->Service.Characteristics = MEM_BufferAlloc(evt->Service.NbOfCharacteristics * sizeof(evt->Service.Characteristics[0]));

		if (!evt->Service.Characteristics)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Service.Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
	{
		evt->Service.Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.Handle)); idx += sizeof(evt->Service.Characteristics[i].Value.Handle);
		evt->Service.Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Service.Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Service.Characteristics[i].Value.MaxValueLength);

		if (evt->Service.Characteristics[i].Value.ValueLength > 0)
		{
			evt->Service.Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Service.Characteristics[i].Value.ValueLength);

			if (!evt->Service.Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Service.Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Service.Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Service.Characteristics[i].Value.Value, pPayload + idx, evt->Service.Characteristics[i].Value.ValueLength); idx += evt->Service.Characteristics[i].Value.ValueLength;
		evt->Service.Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Service.Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Service.Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Service.Characteristics[i].NbOfDescriptors * sizeof(evt->Service.Characteristics[i].Descriptors[0]));

			if (!evt->Service.Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Service.Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Service.Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].Handle);
			evt->Service.Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Service.Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Service.Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Service.Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Service.Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Service.Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Service.Characteristics);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Service.Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Service.Characteristics[i].Descriptors[j].ValueLength); idx += evt->Service.Characteristics[i].Descriptors[j].ValueLength;
		}
	}
	evt->Service.NbOfIncludedServices = pPayload[idx]; idx++;

	if (evt->Service.NbOfIncludedServices > 0)
	{
		evt->Service.IncludedServices = MEM_BufferAlloc(evt->Service.NbOfIncludedServices * sizeof(evt->Service.IncludedServices[0]));

		if (!evt->Service.IncludedServices)
		{
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				for (uint32_t j = 0; j < evt->Service.Characteristics[i].NbOfDescriptors; j++)
				{
					MEM_BufferFree(evt->Service.Characteristics[i].Descriptors[j].Value);
				}
			}
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				MEM_BufferFree(evt->Service.Characteristics[i].Descriptors);
			}
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
			}
			MEM_BufferFree(evt->Service.Characteristics);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Service.IncludedServices = NULL;
	}


	for (uint32_t i = 0; i < evt->Service.NbOfIncludedServices; i++)
	{
		FLib_MemCpy(&(evt->Service.IncludedServices[i].StartHandle), pPayload + idx, sizeof(evt->Service.IncludedServices[i].StartHandle)); idx += sizeof(evt->Service.IncludedServices[i].StartHandle);
		FLib_MemCpy(&(evt->Service.IncludedServices[i].EndHandle), pPayload + idx, sizeof(evt->Service.IncludedServices[i].EndHandle)); idx += sizeof(evt->Service.IncludedServices[i].EndHandle);
		evt->Service.IncludedServices[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		evt->Service.IncludedServices[i].NbOfCharacteristics = pPayload[idx]; idx++;
		evt->Service.IncludedServices[i].NbOfIncludedServices = pPayload[idx]; idx++;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedDiscoverAllCharacteristicsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Discovery procedure for a given service ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedDiscoverAllCharacteristicsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedDiscoverAllCharacteristicsIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedDiscoverAllCharacteristicsIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedDiscoverAllCharacteristicsIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureEnhancedDiscoverAllCharacteristicsIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->Service.StartHandle), pPayload + idx, sizeof(evt->Service.StartHandle)); idx += sizeof(evt->Service.StartHandle);
	FLib_MemCpy(&(evt->Service.EndHandle), pPayload + idx, sizeof(evt->Service.EndHandle)); idx += sizeof(evt->Service.EndHandle);
	evt->Service.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Service.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	evt->Service.NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->Service.NbOfCharacteristics > 0)
	{
		evt->Service.Characteristics = MEM_BufferAlloc(evt->Service.NbOfCharacteristics * sizeof(evt->Service.Characteristics[0]));

		if (!evt->Service.Characteristics)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Service.Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
	{
		evt->Service.Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.Handle)); idx += sizeof(evt->Service.Characteristics[i].Value.Handle);
		evt->Service.Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Service.Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Service.Characteristics[i].Value.MaxValueLength);

		if (evt->Service.Characteristics[i].Value.ValueLength > 0)
		{
			evt->Service.Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Service.Characteristics[i].Value.ValueLength);

			if (!evt->Service.Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Service.Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Service.Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Service.Characteristics[i].Value.Value, pPayload + idx, evt->Service.Characteristics[i].Value.ValueLength); idx += evt->Service.Characteristics[i].Value.ValueLength;
		evt->Service.Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Service.Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Service.Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Service.Characteristics[i].NbOfDescriptors * sizeof(evt->Service.Characteristics[i].Descriptors[0]));

			if (!evt->Service.Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Service.Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Service.Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].Handle);
			evt->Service.Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Service.Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Service.Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Service.Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Service.Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Service.Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Service.Characteristics);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Service.Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Service.Characteristics[i].Descriptors[j].ValueLength); idx += evt->Service.Characteristics[i].Descriptors[j].ValueLength;
		}
	}
	evt->Service.NbOfIncludedServices = pPayload[idx]; idx++;

	if (evt->Service.NbOfIncludedServices > 0)
	{
		evt->Service.IncludedServices = MEM_BufferAlloc(evt->Service.NbOfIncludedServices * sizeof(evt->Service.IncludedServices[0]));

		if (!evt->Service.IncludedServices)
		{
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				for (uint32_t j = 0; j < evt->Service.Characteristics[i].NbOfDescriptors; j++)
				{
					MEM_BufferFree(evt->Service.Characteristics[i].Descriptors[j].Value);
				}
			}
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				MEM_BufferFree(evt->Service.Characteristics[i].Descriptors);
			}
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
			}
			MEM_BufferFree(evt->Service.Characteristics);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Service.IncludedServices = NULL;
	}


	for (uint32_t i = 0; i < evt->Service.NbOfIncludedServices; i++)
	{
		FLib_MemCpy(&(evt->Service.IncludedServices[i].StartHandle), pPayload + idx, sizeof(evt->Service.IncludedServices[i].StartHandle)); idx += sizeof(evt->Service.IncludedServices[i].StartHandle);
		FLib_MemCpy(&(evt->Service.IncludedServices[i].EndHandle), pPayload + idx, sizeof(evt->Service.IncludedServices[i].EndHandle)); idx += sizeof(evt->Service.IncludedServices[i].EndHandle);
		evt->Service.IncludedServices[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		evt->Service.IncludedServices[i].NbOfCharacteristics = pPayload[idx]; idx++;
		evt->Service.IncludedServices[i].NbOfIncludedServices = pPayload[idx]; idx++;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedDiscoverCharacteristicByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Discovery procedure for a given service (with a given UUID) ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedDiscoverCharacteristicByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedDiscoverCharacteristicByUuidIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedDiscoverCharacteristicByUuidIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedDiscoverCharacteristicByUuidIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureEnhancedDiscoverCharacteristicByUuidIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->NbOfCharacteristics > 0)
	{
		evt->Characteristics = MEM_BufferAlloc(evt->NbOfCharacteristics * sizeof(evt->Characteristics[0]));

		if (!evt->Characteristics)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
	{
		evt->Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Characteristics[i].Value.Handle)); idx += sizeof(evt->Characteristics[i].Value.Handle);
		evt->Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Characteristics[i].Value.MaxValueLength);

		if (evt->Characteristics[i].Value.ValueLength > 0)
		{
			evt->Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Characteristics[i].Value.ValueLength);

			if (!evt->Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Characteristics[i].Value.Value, pPayload + idx, evt->Characteristics[i].Value.ValueLength); idx += evt->Characteristics[i].Value.ValueLength;
		evt->Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Characteristics[i].NbOfDescriptors * sizeof(evt->Characteristics[i].Descriptors[0]));

			if (!evt->Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Characteristics[i].Descriptors[j].Handle);
			evt->Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Characteristics);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Characteristics[i].Descriptors[j].ValueLength); idx += evt->Characteristics[i].Descriptors[j].ValueLength;
		}
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedDiscoverAllCharacteristicDescriptorsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Descriptor Discovery procedure for a given characteristic ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedDiscoverAllCharacteristicDescriptorsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedDiscoverAllCharacteristicDescriptorsIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedDiscoverAllCharacteristicDescriptorsIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedDiscoverAllCharacteristicDescriptorsIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureEnhancedDiscoverAllCharacteristicDescriptorsIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->Characteristic.Properties = (Properties_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Characteristic.Value.Handle), pPayload + idx, sizeof(evt->Characteristic.Value.Handle)); idx += sizeof(evt->Characteristic.Value.Handle);
	evt->Characteristic.Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	FLib_MemCpy(&(evt->Characteristic.Value.ValueLength), pPayload + idx, sizeof(evt->Characteristic.Value.ValueLength)); idx += sizeof(evt->Characteristic.Value.ValueLength);
	FLib_MemCpy(&(evt->Characteristic.Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristic.Value.MaxValueLength)); idx += sizeof(evt->Characteristic.Value.MaxValueLength);

	if (evt->Characteristic.Value.ValueLength > 0)
	{
		evt->Characteristic.Value.Value = MEM_BufferAlloc(evt->Characteristic.Value.ValueLength);

		if (!evt->Characteristic.Value.Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristic.Value.Value = NULL;
	}

	FLib_MemCpy(evt->Characteristic.Value.Value, pPayload + idx, evt->Characteristic.Value.ValueLength); idx += evt->Characteristic.Value.ValueLength;
	evt->Characteristic.NbOfDescriptors = pPayload[idx]; idx++;

	if (evt->Characteristic.NbOfDescriptors > 0)
	{
		evt->Characteristic.Descriptors = MEM_BufferAlloc(evt->Characteristic.NbOfDescriptors * sizeof(evt->Characteristic.Descriptors[0]));

		if (!evt->Characteristic.Descriptors)
		{
			MEM_BufferFree(evt->Characteristic.Value.Value);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristic.Descriptors = NULL;
	}


	for (uint32_t i = 0; i < evt->Characteristic.NbOfDescriptors; i++)
	{
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].Handle), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].Handle)); idx += sizeof(evt->Characteristic.Descriptors[i].Handle);
		evt->Characteristic.Descriptors[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].ValueLength), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].ValueLength)); idx += sizeof(evt->Characteristic.Descriptors[i].ValueLength);
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].MaxValueLength), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].MaxValueLength)); idx += sizeof(evt->Characteristic.Descriptors[i].MaxValueLength);

		if (evt->Characteristic.Descriptors[i].ValueLength > 0)
		{
			evt->Characteristic.Descriptors[i].Value = MEM_BufferAlloc(evt->Characteristic.Descriptors[i].ValueLength);

			if (!evt->Characteristic.Descriptors[i].Value)
			{
				MEM_BufferFree(evt->Characteristic.Descriptors);
				MEM_BufferFree(evt->Characteristic.Value.Value);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristic.Descriptors[i].Value = NULL;
		}

		FLib_MemCpy(evt->Characteristic.Descriptors[i].Value, pPayload + idx, evt->Characteristic.Descriptors[i].ValueLength); idx += evt->Characteristic.Descriptors[i].ValueLength;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedReadCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Read procedure for a given characteristic ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedReadCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedReadCharacteristicValueIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedReadCharacteristicValueIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedReadCharacteristicValueIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureEnhancedReadCharacteristicValueIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->Characteristic.Properties = (Properties_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Characteristic.Value.Handle), pPayload + idx, sizeof(evt->Characteristic.Value.Handle)); idx += sizeof(evt->Characteristic.Value.Handle);
	evt->Characteristic.Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	FLib_MemCpy(&(evt->Characteristic.Value.ValueLength), pPayload + idx, sizeof(evt->Characteristic.Value.ValueLength)); idx += sizeof(evt->Characteristic.Value.ValueLength);
	FLib_MemCpy(&(evt->Characteristic.Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristic.Value.MaxValueLength)); idx += sizeof(evt->Characteristic.Value.MaxValueLength);

	if (evt->Characteristic.Value.ValueLength > 0)
	{
		evt->Characteristic.Value.Value = MEM_BufferAlloc(evt->Characteristic.Value.ValueLength);

		if (!evt->Characteristic.Value.Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristic.Value.Value = NULL;
	}

	FLib_MemCpy(evt->Characteristic.Value.Value, pPayload + idx, evt->Characteristic.Value.ValueLength); idx += evt->Characteristic.Value.ValueLength;
	evt->Characteristic.NbOfDescriptors = pPayload[idx]; idx++;

	if (evt->Characteristic.NbOfDescriptors > 0)
	{
		evt->Characteristic.Descriptors = MEM_BufferAlloc(evt->Characteristic.NbOfDescriptors * sizeof(evt->Characteristic.Descriptors[0]));

		if (!evt->Characteristic.Descriptors)
		{
			MEM_BufferFree(evt->Characteristic.Value.Value);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristic.Descriptors = NULL;
	}


	for (uint32_t i = 0; i < evt->Characteristic.NbOfDescriptors; i++)
	{
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].Handle), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].Handle)); idx += sizeof(evt->Characteristic.Descriptors[i].Handle);
		evt->Characteristic.Descriptors[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].ValueLength), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].ValueLength)); idx += sizeof(evt->Characteristic.Descriptors[i].ValueLength);
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].MaxValueLength), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].MaxValueLength)); idx += sizeof(evt->Characteristic.Descriptors[i].MaxValueLength);

		if (evt->Characteristic.Descriptors[i].ValueLength > 0)
		{
			evt->Characteristic.Descriptors[i].Value = MEM_BufferAlloc(evt->Characteristic.Descriptors[i].ValueLength);

			if (!evt->Characteristic.Descriptors[i].Value)
			{
				MEM_BufferFree(evt->Characteristic.Descriptors);
				MEM_BufferFree(evt->Characteristic.Value.Value);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristic.Descriptors[i].Value = NULL;
		}

		FLib_MemCpy(evt->Characteristic.Descriptors[i].Value, pPayload + idx, evt->Characteristic.Descriptors[i].ValueLength); idx += evt->Characteristic.Descriptors[i].ValueLength;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedReadUsingCharacteristicUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Read By UUID procedure ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedReadUsingCharacteristicUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedReadUsingCharacteristicUuidIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedReadUsingCharacteristicUuidIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedReadUsingCharacteristicUuidIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureEnhancedReadUsingCharacteristicUuidIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->NbOfReadBytes), pPayload + idx, sizeof(evt->NbOfReadBytes)); idx += sizeof(evt->NbOfReadBytes);

	if (evt->NbOfReadBytes > 0)
	{
		evt->ReadBytes = MEM_BufferAlloc(evt->NbOfReadBytes);

		if (!evt->ReadBytes)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->ReadBytes = NULL;
	}

	FLib_MemCpy(evt->ReadBytes, pPayload + idx, evt->NbOfReadBytes); idx += evt->NbOfReadBytes;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedReadMultipleCharacteristicValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Read Multiple procedure ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedReadMultipleCharacteristicValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedReadMultipleCharacteristicValuesIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedReadMultipleCharacteristicValuesIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedReadMultipleCharacteristicValuesIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureEnhancedReadMultipleCharacteristicValuesIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->NbOfCharacteristics > 0)
	{
		evt->Characteristics = MEM_BufferAlloc(evt->NbOfCharacteristics * sizeof(evt->Characteristics[0]));

		if (!evt->Characteristics)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
	{
		evt->Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Characteristics[i].Value.Handle)); idx += sizeof(evt->Characteristics[i].Value.Handle);
		evt->Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Characteristics[i].Value.MaxValueLength);

		if (evt->Characteristics[i].Value.ValueLength > 0)
		{
			evt->Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Characteristics[i].Value.ValueLength);

			if (!evt->Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Characteristics[i].Value.Value, pPayload + idx, evt->Characteristics[i].Value.ValueLength); idx += evt->Characteristics[i].Value.ValueLength;
		evt->Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Characteristics[i].NbOfDescriptors * sizeof(evt->Characteristics[i].Descriptors[0]));

			if (!evt->Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Characteristics[i].Descriptors[j].Handle);
			evt->Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Characteristics);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Characteristics[i].Descriptors[j].ValueLength); idx += evt->Characteristics[i].Descriptors[j].ValueLength;
		}
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedWriteCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Write procedure for a given characteristic ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedWriteCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedWriteCharacteristicValueIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedWriteCharacteristicValueIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedWriteCharacteristicValueIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTClientProcedureEnhancedWriteCharacteristicValueIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedReadCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Descriptor Read procedure for a given characteristic's descriptor ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedReadCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedReadCharacteristicDescriptorIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedReadCharacteristicDescriptorIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedReadCharacteristicDescriptorIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureEnhancedReadCharacteristicDescriptorIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->Descriptor.Handle), pPayload + idx, sizeof(evt->Descriptor.Handle)); idx += sizeof(evt->Descriptor.Handle);
	evt->Descriptor.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Descriptor.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Descriptor.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Descriptor.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Descriptor.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	FLib_MemCpy(&(evt->Descriptor.ValueLength), pPayload + idx, sizeof(evt->Descriptor.ValueLength)); idx += sizeof(evt->Descriptor.ValueLength);
	FLib_MemCpy(&(evt->Descriptor.MaxValueLength), pPayload + idx, sizeof(evt->Descriptor.MaxValueLength)); idx += sizeof(evt->Descriptor.MaxValueLength);

	if (evt->Descriptor.ValueLength > 0)
	{
		evt->Descriptor.Value = MEM_BufferAlloc(evt->Descriptor.ValueLength);

		if (!evt->Descriptor.Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Descriptor.Value = NULL;
	}

	FLib_MemCpy(evt->Descriptor.Value, pPayload + idx, evt->Descriptor.ValueLength); idx += evt->Descriptor.ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedWriteCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Descriptor Write procedure for a given characteristic's descriptor ended
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedWriteCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedWriteCharacteristicDescriptorIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedWriteCharacteristicDescriptorIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedWriteCharacteristicDescriptorIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTClientProcedureEnhancedWriteCharacteristicDescriptorIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientProcedureEnhancedReadMultipleVariableLenCharValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Client Enhanced Read Multiple Variable Length Characteristic Values
***************************************************************************************************/
static mem_status_t Load_GATTClientProcedureEnhancedReadMultipleVariableLenCharValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureEnhancedReadMultipleVariableLenCharValuesIndication_t *evt = &(container->Data.GATTClientProcedureEnhancedReadMultipleVariableLenCharValuesIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientProcedureEnhancedReadMultipleVariableLenCharValuesIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureEnhancedReadMultipleVariableLenCharValuesIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->NbOfCharacteristics > 0)
	{
		evt->Characteristics = MEM_BufferAlloc(evt->NbOfCharacteristics * sizeof(evt->Characteristics[0]));

		if (!evt->Characteristics)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
	{
		evt->Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Characteristics[i].Value.Handle)); idx += sizeof(evt->Characteristics[i].Value.Handle);
		evt->Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Characteristics[i].Value.MaxValueLength);

		if (evt->Characteristics[i].Value.ValueLength > 0)
		{
			evt->Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Characteristics[i].Value.ValueLength);

			if (!evt->Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Characteristics[i].Value.Value, pPayload + idx, evt->Characteristics[i].Value.ValueLength); idx += evt->Characteristics[i].Value.ValueLength;
		evt->Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Characteristics[i].NbOfDescriptors * sizeof(evt->Characteristics[i].Descriptors[0]));

			if (!evt->Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Characteristics);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Characteristics[i].Descriptors[j].Handle);
			evt->Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Characteristics);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Characteristics[i].Descriptors[j].ValueLength); idx += evt->Characteristics[i].Descriptors[j].ValueLength;
		}
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientEnhancedMultipleHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Client Enhanced Read Multiple Variable Length Characteristic Values
***************************************************************************************************/
static mem_status_t Load_GATTClientEnhancedMultipleHandleValueNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientEnhancedMultipleHandleValueNotificationIndication_t *evt = &(container->Data.GATTClientEnhancedMultipleHandleValueNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientEnhancedMultipleHandleValueNotificationIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->HandleCount), pPayload + idx, sizeof(evt->HandleCount)); idx += sizeof(evt->HandleCount);

	if (evt->HandleCount > 0)
	{
		evt->HandleLengthValueList = MEM_BufferAlloc(evt->HandleCount * sizeof(evt->HandleLengthValueList[0]));

		if (!evt->HandleLengthValueList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->HandleLengthValueList = NULL;
	}


	for (uint32_t i = 0; i < evt->HandleCount; i++)
	{
		FLib_MemCpy(&(evt->HandleLengthValueList[i].Handle), pPayload + idx, sizeof(evt->HandleLengthValueList[i].Handle)); idx += sizeof(evt->HandleLengthValueList[i].Handle);
		FLib_MemCpy(&(evt->HandleLengthValueList[i].ValueLength), pPayload + idx, sizeof(evt->HandleLengthValueList[i].ValueLength)); idx += sizeof(evt->HandleLengthValueList[i].ValueLength);

		if (evt->HandleLengthValueList[i].ValueLength > 0)
		{
			evt->HandleLengthValueList[i].Value = MEM_BufferAlloc(evt->HandleLengthValueList[i].ValueLength);

			if (!evt->HandleLengthValueList[i].Value)
			{
				MEM_BufferFree(evt->HandleLengthValueList);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->HandleLengthValueList[i].Value = NULL;
		}

		FLib_MemCpy(evt->HandleLengthValueList[i].Value, pPayload + idx, evt->HandleLengthValueList[i].ValueLength); idx += evt->HandleLengthValueList[i].ValueLength;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientEnhancedNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Client notification
***************************************************************************************************/
static mem_status_t Load_GATTClientEnhancedNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientEnhancedNotificationIndication_t *evt = &(container->Data.GATTClientEnhancedNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientEnhancedNotificationIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->CharacteristicValueHandle), pPayload + idx, sizeof(evt->CharacteristicValueHandle)); idx += sizeof(evt->CharacteristicValueHandle);
	FLib_MemCpy(&(evt->ValueLength), pPayload + idx, sizeof(evt->ValueLength)); idx += sizeof(evt->ValueLength);

	if (evt->ValueLength > 0)
	{
		evt->Value = MEM_BufferAlloc(evt->ValueLength);

		if (!evt->Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Value = NULL;
	}

	FLib_MemCpy(evt->Value, pPayload + idx, evt->ValueLength); idx += evt->ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTClientEnhancedIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Client indication
***************************************************************************************************/
static mem_status_t Load_GATTClientEnhancedIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientEnhancedIndicationIndication_t *evt = &(container->Data.GATTClientEnhancedIndicationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTClientEnhancedIndicationIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->CharacteristicValueHandle), pPayload + idx, sizeof(evt->CharacteristicValueHandle)); idx += sizeof(evt->CharacteristicValueHandle);
	FLib_MemCpy(&(evt->ValueLength), pPayload + idx, sizeof(evt->ValueLength)); idx += sizeof(evt->ValueLength);

	if (evt->ValueLength > 0)
	{
		evt->Value = MEM_BufferAlloc(evt->ValueLength);

		if (!evt->Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Value = NULL;
	}

	FLib_MemCpy(evt->Value, pPayload + idx, evt->ValueLength); idx += evt->ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerEnhancedHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server handle value confirmation
***************************************************************************************************/
static mem_status_t Load_GATTServerEnhancedHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerEnhancedHandleValueConfirmationIndication_t *evt = &(container->Data.GATTServerEnhancedHandleValueConfirmationIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTServerEnhancedHandleValueConfirmationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerEnhancedHandleValueConfirmationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerEnhancedAttributeWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server attribute written
***************************************************************************************************/
static mem_status_t Load_GATTServerEnhancedAttributeWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerEnhancedAttributeWrittenIndication_t *evt = &(container->Data.GATTServerEnhancedAttributeWrittenIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTServerEnhancedAttributeWrittenIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->AttributeWrittenEvent.Handle), pPayload + idx, sizeof(evt->AttributeWrittenEvent.Handle)); idx += sizeof(evt->AttributeWrittenEvent.Handle);
	FLib_MemCpy(&(evt->AttributeWrittenEvent.ValueLength), pPayload + idx, sizeof(evt->AttributeWrittenEvent.ValueLength)); idx += sizeof(evt->AttributeWrittenEvent.ValueLength);

	if (evt->AttributeWrittenEvent.ValueLength > 0)
	{
		evt->AttributeWrittenEvent.Value = MEM_BufferAlloc(evt->AttributeWrittenEvent.ValueLength);

		if (!evt->AttributeWrittenEvent.Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->AttributeWrittenEvent.Value = NULL;
	}

	FLib_MemCpy(evt->AttributeWrittenEvent.Value, pPayload + idx, evt->AttributeWrittenEvent.ValueLength); idx += evt->AttributeWrittenEvent.ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerEnhancedCharacteristicCccdWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server characteristic cccd written
***************************************************************************************************/
static mem_status_t Load_GATTServerEnhancedCharacteristicCccdWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerEnhancedCharacteristicCccdWrittenIndication_t *evt = &(container->Data.GATTServerEnhancedCharacteristicCccdWrittenIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTServerEnhancedCharacteristicCccdWrittenIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerEnhancedCharacteristicCccdWrittenIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerEnhancedAttributeWrittenWithoutResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server attribute written without response
***************************************************************************************************/
static mem_status_t Load_GATTServerEnhancedAttributeWrittenWithoutResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerEnhancedAttributeWrittenWithoutResponseIndication_t *evt = &(container->Data.GATTServerEnhancedAttributeWrittenWithoutResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTServerEnhancedAttributeWrittenWithoutResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->AttributeWrittenEvent.Handle), pPayload + idx, sizeof(evt->AttributeWrittenEvent.Handle)); idx += sizeof(evt->AttributeWrittenEvent.Handle);
	FLib_MemCpy(&(evt->AttributeWrittenEvent.ValueLength), pPayload + idx, sizeof(evt->AttributeWrittenEvent.ValueLength)); idx += sizeof(evt->AttributeWrittenEvent.ValueLength);

	if (evt->AttributeWrittenEvent.ValueLength > 0)
	{
		evt->AttributeWrittenEvent.Value = MEM_BufferAlloc(evt->AttributeWrittenEvent.ValueLength);

		if (!evt->AttributeWrittenEvent.Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->AttributeWrittenEvent.Value = NULL;
	}

	FLib_MemCpy(evt->AttributeWrittenEvent.Value, pPayload + idx, evt->AttributeWrittenEvent.ValueLength); idx += evt->AttributeWrittenEvent.ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerEnhancedErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server error
***************************************************************************************************/
static mem_status_t Load_GATTServerEnhancedErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerEnhancedErrorIndication_t *evt = &(container->Data.GATTServerEnhancedErrorIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTServerEnhancedErrorIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerEnhancedErrorIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerEnhancedLongCharacteristicWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server long characteristic written
***************************************************************************************************/
static mem_status_t Load_GATTServerEnhancedLongCharacteristicWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerEnhancedLongCharacteristicWrittenIndication_t *evt = &(container->Data.GATTServerEnhancedLongCharacteristicWrittenIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTServerEnhancedLongCharacteristicWrittenIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->BearerId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->LongCharacteristicWrittenEvent.Handle), pPayload + idx, sizeof(evt->LongCharacteristicWrittenEvent.Handle)); idx += sizeof(evt->LongCharacteristicWrittenEvent.Handle);
	FLib_MemCpy(&(evt->LongCharacteristicWrittenEvent.ValueLength), pPayload + idx, sizeof(evt->LongCharacteristicWrittenEvent.ValueLength)); idx += sizeof(evt->LongCharacteristicWrittenEvent.ValueLength);

	if (evt->LongCharacteristicWrittenEvent.ValueLength > 0)
	{
		evt->LongCharacteristicWrittenEvent.Value = MEM_BufferAlloc(evt->LongCharacteristicWrittenEvent.ValueLength);

		if (!evt->LongCharacteristicWrittenEvent.Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->LongCharacteristicWrittenEvent.Value = NULL;
	}

	FLib_MemCpy(evt->LongCharacteristicWrittenEvent.Value, pPayload + idx, evt->LongCharacteristicWrittenEvent.ValueLength); idx += evt->LongCharacteristicWrittenEvent.ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTServerEnhancedAttributeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server attribute read
***************************************************************************************************/
static mem_status_t Load_GATTServerEnhancedAttributeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerEnhancedAttributeReadIndication_t *evt = &(container->Data.GATTServerEnhancedAttributeReadIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTServerEnhancedAttributeReadIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerEnhancedAttributeReadIndication_t));

	return kStatus_MemSuccess;
}

#endif  /* GATT_ENABLE */

#if GATTDB_APP_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the GATT Database (application) request
***************************************************************************************************/
static mem_status_t Load_GATTDBConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBConfirm_t *evt = &(container->Data.GATTDBConfirm);

	/* Store (OG, OC) in ID */
	container->id = GATTDBConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBReadAttributeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Reads an attribute value (from application level)
***************************************************************************************************/
static mem_status_t Load_GATTDBReadAttributeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBReadAttributeIndication_t *evt = &(container->Data.GATTDBReadAttributeIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTDBReadAttributeIndication_FSCI_ID;

	FLib_MemCpy(&(evt->ValueLength), pPayload + idx, sizeof(evt->ValueLength)); idx += sizeof(evt->ValueLength);

	if (evt->ValueLength > 0)
	{
		evt->Value = MEM_BufferAlloc(evt->ValueLength);

		if (!evt->Value)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Value = NULL;
	}

	FLib_MemCpy(evt->Value, pPayload + idx, evt->ValueLength); idx += evt->ValueLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBFindServiceHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Finds the handle of a Service Declaration with a given UUID inside the database
***************************************************************************************************/
static mem_status_t Load_GATTDBFindServiceHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBFindServiceHandleIndication_t *evt = &(container->Data.GATTDBFindServiceHandleIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBFindServiceHandleIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBFindServiceHandleIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBFindCharValueHandleInServiceIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Finds the handle of a characteristic value (with a given UUID) inside a service
***************************************************************************************************/
static mem_status_t Load_GATTDBFindCharValueHandleInServiceIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBFindCharValueHandleInServiceIndication_t *evt = &(container->Data.GATTDBFindCharValueHandleInServiceIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBFindCharValueHandleInServiceIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBFindCharValueHandleInServiceIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBFindCccdHandleForCharValueHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Finds the handle of a characteristic's CCCD (giving the characteristic's value handle)
***************************************************************************************************/
static mem_status_t Load_GATTDBFindCccdHandleForCharValueHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBFindCccdHandleForCharValueHandleIndication_t *evt = &(container->Data.GATTDBFindCccdHandleForCharValueHandleIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBFindCccdHandleForCharValueHandleIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBFindCccdHandleForCharValueHandleIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBFindDescriptorHandleForCharValueHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Finds the handle of a characteristic descriptor (giving the characteristic's value handle and descriptor's UUID)
***************************************************************************************************/
static mem_status_t Load_GATTDBFindDescriptorHandleForCharValueHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBFindDescriptorHandleForCharValueHandleIndication_t *evt = &(container->Data.GATTDBFindDescriptorHandleForCharValueHandleIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBFindDescriptorHandleForCharValueHandleIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBFindDescriptorHandleForCharValueHandleIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBDynamicAddPrimaryServiceDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a Primary Service declaration into the database
***************************************************************************************************/
static mem_status_t Load_GATTDBDynamicAddPrimaryServiceDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddPrimaryServiceDeclarationIndication_t *evt = &(container->Data.GATTDBDynamicAddPrimaryServiceDeclarationIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBDynamicAddPrimaryServiceDeclarationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddPrimaryServiceDeclarationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBDynamicAddSecondaryServiceDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a Secondary Service declaration into the database
***************************************************************************************************/
static mem_status_t Load_GATTDBDynamicAddSecondaryServiceDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddSecondaryServiceDeclarationIndication_t *evt = &(container->Data.GATTDBDynamicAddSecondaryServiceDeclarationIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBDynamicAddSecondaryServiceDeclarationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddSecondaryServiceDeclarationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBDynamicAddIncludeDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds an Include declaration into the database
***************************************************************************************************/
static mem_status_t Load_GATTDBDynamicAddIncludeDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddIncludeDeclarationIndication_t *evt = &(container->Data.GATTDBDynamicAddIncludeDeclarationIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBDynamicAddIncludeDeclarationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddIncludeDeclarationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBDynamicAddCharacteristicDeclarationAndValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a Characteristic declaration and its Value into the database
***************************************************************************************************/
static mem_status_t Load_GATTDBDynamicAddCharacteristicDeclarationAndValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddCharacteristicDeclarationAndValueIndication_t *evt = &(container->Data.GATTDBDynamicAddCharacteristicDeclarationAndValueIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBDynamicAddCharacteristicDeclarationAndValueIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddCharacteristicDeclarationAndValueIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBDynamicAddCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a Characteristic descriptor into the database
***************************************************************************************************/
static mem_status_t Load_GATTDBDynamicAddCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddCharacteristicDescriptorIndication_t *evt = &(container->Data.GATTDBDynamicAddCharacteristicDescriptorIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBDynamicAddCharacteristicDescriptorIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddCharacteristicDescriptorIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBDynamicAddCccdIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a CCCD in the database
***************************************************************************************************/
static mem_status_t Load_GATTDBDynamicAddCccdIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddCccdIndication_t *evt = &(container->Data.GATTDBDynamicAddCccdIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBDynamicAddCccdIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddCccdIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a Characteristic declaration with a Value contained in an universal value buffer
***************************************************************************************************/
static mem_status_t Load_GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication_t *evt = &(container->Data.GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBDynamicAddCharDescriptorWithUniqueValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a Characteristic descriptor with a Value contained in an universal value buffer
***************************************************************************************************/
static mem_status_t Load_GATTDBDynamicAddCharDescriptorWithUniqueValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddCharDescriptorWithUniqueValueIndication_t *evt = &(container->Data.GATTDBDynamicAddCharDescriptorWithUniqueValueIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBDynamicAddCharDescriptorWithUniqueValueIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddCharDescriptorWithUniqueValueIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBDynamicAddCharAggregateFormatIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a Characteristic Aggregate Format Descriptor in the database
***************************************************************************************************/
static mem_status_t Load_GATTDBDynamicAddCharAggregateFormatIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddCharAggregateFormatIndication_t *evt = &(container->Data.GATTDBDynamicAddCharAggregateFormatIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBDynamicAddCharAggregateFormatIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddCharAggregateFormatIndication_t));

	return kStatus_MemSuccess;
}

#endif  /* GATTDB_APP_ENABLE */

#if GATTDB_ATT_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the GATT Database (ATT) request
***************************************************************************************************/
static mem_status_t Load_GATTDBAttConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttConfirm_t *evt = &(container->Data.GATTDBAttConfirm);

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBAttConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttFindInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Find information
***************************************************************************************************/
static mem_status_t Load_GATTDBAttFindInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttFindInformationIndication_t *evt = &(container->Data.GATTDBAttFindInformationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttFindInformationIndication_FSCI_ID;

	evt->Params.Format = (GATTDBAttFindInformationIndication_Params_Format_t)pPayload[idx]; idx++;

	switch (evt->Params.Format)
	{
		case GATTDBAttFindInformationIndication_Params_Format_Uuid16BitFormat:
			evt->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount = pPayload[idx]; idx++;

			if (evt->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount > 0)
			{
				evt->Params.InformationData.Uuid16BitFormat.HandleUuid16 = MEM_BufferAlloc(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount * sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[0]));

				if (!evt->Params.InformationData.Uuid16BitFormat.HandleUuid16)
				{
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Params.InformationData.Uuid16BitFormat.HandleUuid16 = NULL;
			}


			for (uint32_t i = 0; i < evt->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount; i++)
			{
				FLib_MemCpy(&(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle), pPayload + idx, sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle)); idx += sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle);
				FLib_MemCpy(&(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16), pPayload + idx, sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16)); idx += sizeof(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16);
			}
			break;

		case GATTDBAttFindInformationIndication_Params_Format_Uuid128BitFormat:
			evt->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount = pPayload[idx]; idx++;

			if (evt->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount > 0)
			{
				evt->Params.InformationData.Uuid128BitFormat.HandleUuid128 = MEM_BufferAlloc(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount * sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[0]));

				if (!evt->Params.InformationData.Uuid128BitFormat.HandleUuid128)
				{
					MEM_BufferFree(evt->Params.InformationData.Uuid16BitFormat.HandleUuid16);
					return kStatus_MemAllocError;
				}

			}
			else
			{
				evt->Params.InformationData.Uuid128BitFormat.HandleUuid128 = NULL;
			}


			for (uint32_t i = 0; i < evt->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount; i++)
			{
				FLib_MemCpy(&(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle), pPayload + idx, sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle)); idx += sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle);
				FLib_MemCpy(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128, pPayload + idx, sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128)); idx += sizeof(evt->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128);
			}
			break;
	}
	FLib_MemCpy(&(evt->ErrorAttributeHandle), pPayload + idx, sizeof(evt->ErrorAttributeHandle)); idx += sizeof(evt->ErrorAttributeHandle);

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttFindByTypeValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Find by type value
***************************************************************************************************/
static mem_status_t Load_GATTDBAttFindByTypeValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttFindByTypeValueIndication_t *evt = &(container->Data.GATTDBAttFindByTypeValueIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttFindByTypeValueIndication_FSCI_ID;

	FLib_MemCpy(&(evt->Params.GroupCount), pPayload + idx, sizeof(evt->Params.GroupCount)); idx += sizeof(evt->Params.GroupCount);

	if (evt->Params.GroupCount > 0)
	{
		evt->Params.HandleGroup = MEM_BufferAlloc(evt->Params.GroupCount * sizeof(evt->Params.HandleGroup[0]));

		if (!evt->Params.HandleGroup)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.HandleGroup = NULL;
	}


	for (uint32_t i = 0; i < evt->Params.GroupCount; i++)
	{
		FLib_MemCpy(&(evt->Params.HandleGroup[i].StartingHandle), pPayload + idx, sizeof(evt->Params.HandleGroup[i].StartingHandle)); idx += sizeof(evt->Params.HandleGroup[i].StartingHandle);
		FLib_MemCpy(&(evt->Params.HandleGroup[i].EndingHandle), pPayload + idx, sizeof(evt->Params.HandleGroup[i].EndingHandle)); idx += sizeof(evt->Params.HandleGroup[i].EndingHandle);
	}
	FLib_MemCpy(&(evt->ErrorAttributeHandle), pPayload + idx, sizeof(evt->ErrorAttributeHandle)); idx += sizeof(evt->ErrorAttributeHandle);

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttReadByTypeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Read by type
***************************************************************************************************/
static mem_status_t Load_GATTDBAttReadByTypeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttReadByTypeIndication_t *evt = &(container->Data.GATTDBAttReadByTypeIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttReadByTypeIndication_FSCI_ID;

	evt->Params.Length = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeDataListLength), pPayload + idx, sizeof(evt->Params.AttributeDataListLength)); idx += sizeof(evt->Params.AttributeDataListLength);

	if (evt->Params.AttributeDataListLength > 0)
	{
		evt->Params.AttributeDataList = MEM_BufferAlloc(evt->Params.AttributeDataListLength);

		if (!evt->Params.AttributeDataList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeDataList = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeDataList, pPayload + idx, evt->Params.AttributeDataListLength); idx += evt->Params.AttributeDataListLength;
	FLib_MemCpy(&(evt->ErrorAttributeHandle), pPayload + idx, sizeof(evt->ErrorAttributeHandle)); idx += sizeof(evt->ErrorAttributeHandle);

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Read
***************************************************************************************************/
static mem_status_t Load_GATTDBAttReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttReadIndication_t *evt = &(container->Data.GATTDBAttReadIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttReadIndication_FSCI_ID;

	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;
	FLib_MemCpy(&(evt->ErrorAttributeHandle), pPayload + idx, sizeof(evt->ErrorAttributeHandle)); idx += sizeof(evt->ErrorAttributeHandle);

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttReadBlobIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Read blob
***************************************************************************************************/
static mem_status_t Load_GATTDBAttReadBlobIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttReadBlobIndication_t *evt = &(container->Data.GATTDBAttReadBlobIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttReadBlobIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBAttReadBlobIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttReadMultipleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Read multiple
***************************************************************************************************/
static mem_status_t Load_GATTDBAttReadMultipleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttReadMultipleIndication_t *evt = &(container->Data.GATTDBAttReadMultipleIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttReadMultipleIndication_FSCI_ID;

	FLib_MemCpy(&(evt->Params.ListLength), pPayload + idx, sizeof(evt->Params.ListLength)); idx += sizeof(evt->Params.ListLength);

	if (evt->Params.ListLength > 0)
	{
		evt->Params.ListOfValues = MEM_BufferAlloc(evt->Params.ListLength);

		if (!evt->Params.ListOfValues)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.ListOfValues = NULL;
	}

	FLib_MemCpy(evt->Params.ListOfValues, pPayload + idx, evt->Params.ListLength); idx += evt->Params.ListLength;
	FLib_MemCpy(&(evt->ErrorAttributeHandle), pPayload + idx, sizeof(evt->ErrorAttributeHandle)); idx += sizeof(evt->ErrorAttributeHandle);

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttReadByGroupTypeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Read by group type
***************************************************************************************************/
static mem_status_t Load_GATTDBAttReadByGroupTypeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttReadByGroupTypeIndication_t *evt = &(container->Data.GATTDBAttReadByGroupTypeIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttReadByGroupTypeIndication_FSCI_ID;

	evt->Params.Length = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Params.AttributeDataListLength), pPayload + idx, sizeof(evt->Params.AttributeDataListLength)); idx += sizeof(evt->Params.AttributeDataListLength);

	if (evt->Params.AttributeDataListLength > 0)
	{
		evt->Params.AttributeDataList = MEM_BufferAlloc(evt->Params.AttributeDataListLength);

		if (!evt->Params.AttributeDataList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeDataList = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeDataList, pPayload + idx, evt->Params.AttributeDataListLength); idx += evt->Params.AttributeDataListLength;
	FLib_MemCpy(&(evt->ErrorAttributeHandle), pPayload + idx, sizeof(evt->ErrorAttributeHandle)); idx += sizeof(evt->ErrorAttributeHandle);

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttWriteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Write
***************************************************************************************************/
static mem_status_t Load_GATTDBAttWriteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttWriteIndication_t *evt = &(container->Data.GATTDBAttWriteIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttWriteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBAttWriteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttPrepareWriteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Prepare write
***************************************************************************************************/
static mem_status_t Load_GATTDBAttPrepareWriteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttPrepareWriteIndication_t *evt = &(container->Data.GATTDBAttPrepareWriteIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttPrepareWriteIndication_FSCI_ID;

	FLib_MemCpy(&(evt->Params.AttributeHandle), pPayload + idx, sizeof(evt->Params.AttributeHandle)); idx += sizeof(evt->Params.AttributeHandle);
	FLib_MemCpy(&(evt->Params.ValueOffset), pPayload + idx, sizeof(evt->Params.ValueOffset)); idx += sizeof(evt->Params.ValueOffset);
	FLib_MemCpy(&(evt->Params.AttributeLength), pPayload + idx, sizeof(evt->Params.AttributeLength)); idx += sizeof(evt->Params.AttributeLength);

	if (evt->Params.AttributeLength > 0)
	{
		evt->Params.AttributeValue = MEM_BufferAlloc(evt->Params.AttributeLength);

		if (!evt->Params.AttributeValue)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Params.AttributeValue = NULL;
	}

	FLib_MemCpy(evt->Params.AttributeValue, pPayload + idx, evt->Params.AttributeLength); idx += evt->Params.AttributeLength;
	FLib_MemCpy(&(evt->ErrorAttributeHandle), pPayload + idx, sizeof(evt->ErrorAttributeHandle)); idx += sizeof(evt->ErrorAttributeHandle);

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttExecuteWriteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Execute write
***************************************************************************************************/
static mem_status_t Load_GATTDBAttExecuteWriteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttExecuteWriteIndication_t *evt = &(container->Data.GATTDBAttExecuteWriteIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttExecuteWriteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBAttExecuteWriteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttExecuteWriteFromQueueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Executes an operation from a Prepare Write queue
***************************************************************************************************/
static mem_status_t Load_GATTDBAttExecuteWriteFromQueueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttExecuteWriteFromQueueIndication_t *evt = &(container->Data.GATTDBAttExecuteWriteFromQueueIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttExecuteWriteFromQueueIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBAttExecuteWriteFromQueueIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GATTDBAttPrepareNotificationIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Prepare notification/indication
***************************************************************************************************/
static mem_status_t Load_GATTDBAttPrepareNotificationIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBAttPrepareNotificationIndicationIndication_t *evt = &(container->Data.GATTDBAttPrepareNotificationIndicationIndication);

	/* Store (OG, OC) in ID */
	container->id = GATTDBAttPrepareNotificationIndicationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBAttPrepareNotificationIndicationIndication_t));

	return kStatus_MemSuccess;
}

#endif  /* GATTDB_ATT_ENABLE */

#if GAP_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the GAP request
***************************************************************************************************/
static mem_status_t Load_GAPConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConfirm_t *evt = &(container->Data.GAPConfirm);

	/* Store (OG, OC) in ID */
	container->id = GAPConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPCheckNotificationStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the notification status for a given Client and a given CCCD handle
***************************************************************************************************/
static mem_status_t Load_GAPCheckNotificationStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPCheckNotificationStatusIndication_t *evt = &(container->Data.GAPCheckNotificationStatusIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPCheckNotificationStatusIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPCheckNotificationStatusIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPCheckIndicationStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the indication status for a given Client and a given CCCD handle
***************************************************************************************************/
static mem_status_t Load_GAPCheckIndicationStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPCheckIndicationStatusIndication_t *evt = &(container->Data.GAPCheckIndicationStatusIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPCheckIndicationStatusIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPCheckIndicationStatusIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPLoadKeysIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the keys of a bonded device
***************************************************************************************************/
static mem_status_t Load_GAPLoadKeysIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPLoadKeysIndication_t *evt = &(container->Data.GAPLoadKeysIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPLoadKeysIndication_FSCI_ID;

	evt->Keys.LtkIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->Keys.LtkIncluded)
	{
		evt->Keys.LtkInfo.LtkSize = pPayload[idx]; idx++;

		if (evt->Keys.LtkInfo.LtkSize > 0)
		{
			evt->Keys.LtkInfo.Ltk = MEM_BufferAlloc(evt->Keys.LtkInfo.LtkSize);

			if (!evt->Keys.LtkInfo.Ltk)
			{
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Keys.LtkInfo.Ltk = NULL;
		}

		FLib_MemCpy(evt->Keys.LtkInfo.Ltk, pPayload + idx, evt->Keys.LtkInfo.LtkSize); idx += evt->Keys.LtkInfo.LtkSize;
	}

	evt->Keys.IrkIncluded = (bool_t)pPayload[idx]; idx++;


	evt->Keys.CsrkIncluded = (bool_t)pPayload[idx]; idx++;



	if (evt->Keys.LtkIncluded)
	{
		evt->Keys.RandEdivInfo.RandSize = pPayload[idx]; idx++;

		if (evt->Keys.RandEdivInfo.RandSize > 0)
		{
			evt->Keys.RandEdivInfo.Rand = MEM_BufferAlloc(evt->Keys.RandEdivInfo.RandSize);

			if (!evt->Keys.RandEdivInfo.Rand)
			{
				MEM_BufferFree(evt->Keys.LtkInfo.Ltk);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Keys.RandEdivInfo.Rand = NULL;
		}

		FLib_MemCpy(evt->Keys.RandEdivInfo.Rand, pPayload + idx, evt->Keys.RandEdivInfo.RandSize); idx += evt->Keys.RandEdivInfo.RandSize;
		FLib_MemCpy(&(evt->Keys.RandEdivInfo.Ediv), pPayload + idx, sizeof(evt->Keys.RandEdivInfo.Ediv)); idx += sizeof(evt->Keys.RandEdivInfo.Ediv);
	}


	if (evt->Keys.IrkIncluded)
	{
		evt->Keys.AddressIncluded = (bool_t)pPayload[idx]; idx++;
	}



	if (evt->Keys.AddressIncluded)
	{
		evt->Keys.AddressInfo.DeviceAddressType = (GAPLoadKeysIndication_Keys_AddressInfo_DeviceAddressType_t)pPayload[idx]; idx++;
		FLib_MemCpy(evt->Keys.AddressInfo.DeviceAddress, pPayload + idx, 6); idx += 6;
	}

	evt->KeyFlags = pPayload[idx]; idx++;
	evt->LeSc = (bool_t)pPayload[idx]; idx++;
	evt->Authenticated = (bool_t)pPayload[idx]; idx++;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPLoadEncryptionInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the encryption key for a bonded device
***************************************************************************************************/
static mem_status_t Load_GAPLoadEncryptionInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPLoadEncryptionInformationIndication_t *evt = &(container->Data.GAPLoadEncryptionInformationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPLoadEncryptionInformationIndication_FSCI_ID;

	evt->LtkSize = pPayload[idx]; idx++;

	if (evt->LtkSize > 0)
	{
		evt->Ltk = MEM_BufferAlloc(evt->LtkSize);

		if (!evt->Ltk)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Ltk = NULL;
	}

	FLib_MemCpy(evt->Ltk, pPayload + idx, evt->LtkSize); idx += evt->LtkSize;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPLoadCustomPeerInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the custom peer information in raw data format
***************************************************************************************************/
static mem_status_t Load_GAPLoadCustomPeerInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPLoadCustomPeerInformationIndication_t *evt = &(container->Data.GAPLoadCustomPeerInformationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPLoadCustomPeerInformationIndication_FSCI_ID;

	FLib_MemCpy(&(evt->InfoSize), pPayload + idx, sizeof(evt->InfoSize)); idx += sizeof(evt->InfoSize);

	if (evt->InfoSize > 0)
	{
		evt->Info = MEM_BufferAlloc(evt->InfoSize);

		if (!evt->Info)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Info = NULL;
	}

	FLib_MemCpy(evt->Info, pPayload + idx, evt->InfoSize); idx += evt->InfoSize;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPCheckIfBondedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns whether a connected peer device is bonded or not
***************************************************************************************************/
static mem_status_t Load_GAPCheckIfBondedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPCheckIfBondedIndication_t *evt = &(container->Data.GAPCheckIfBondedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPCheckIfBondedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPCheckIfBondedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGetBondedDevicesCountIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the number of bonded devices
***************************************************************************************************/
static mem_status_t Load_GAPGetBondedDevicesCountIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGetBondedDevicesCountIndication_t *evt = &(container->Data.GAPGetBondedDevicesCountIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGetBondedDevicesCountIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGetBondedDevicesCountIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGetBondedDeviceNameIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the name of a bonded device
***************************************************************************************************/
static mem_status_t Load_GAPGetBondedDeviceNameIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGetBondedDeviceNameIndication_t *evt = &(container->Data.GAPGetBondedDeviceNameIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPGetBondedDeviceNameIndication_FSCI_ID;

	evt->NameSize = pPayload[idx]; idx++;

	if (evt->NameSize > 0)
	{
		evt->Name = MEM_BufferAlloc(evt->NameSize);

		if (!evt->Name)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Name = NULL;
	}

	FLib_MemCpy(evt->Name, pPayload + idx, evt->NameSize); idx += evt->NameSize;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventInitializationCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - initialization complete
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventInitializationCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventInitializationCompleteIndication_t *evt = &(container->Data.GAPGenericEventInitializationCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventInitializationCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventInitializationCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventInternalErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - controller error
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventInternalErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventInternalErrorIndication_t *evt = &(container->Data.GAPGenericEventInternalErrorIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventInternalErrorIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventInternalErrorIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventAdvertisingSetupFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - advertising setup failed
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventAdvertisingSetupFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventAdvertisingSetupFailedIndication_t *evt = &(container->Data.GAPGenericEventAdvertisingSetupFailedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventAdvertisingSetupFailedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventAdvertisingSetupFailedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventAdvertisingParametersSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - advertising parameters setup completed
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventAdvertisingParametersSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventAdvertisingParametersSetupCompleteIndication_t *evt = &(container->Data.GAPGenericEventAdvertisingParametersSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventAdvertisingParametersSetupCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventAdvertisingParametersSetupCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventAdvertisingDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - advertising data setup completed
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventAdvertisingDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventAdvertisingDataSetupCompleteIndication_t *evt = &(container->Data.GAPGenericEventAdvertisingDataSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventAdvertisingDataSetupCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventAdvertisingDataSetupCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventFilterAcceptListSizeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - Filter Accept List size
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventFilterAcceptListSizeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventFilterAcceptListSizeReadIndication_t *evt = &(container->Data.GAPGenericEventFilterAcceptListSizeReadIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventFilterAcceptListSizeReadIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventFilterAcceptListSizeReadIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventDeviceAddedToFilterAcceptListIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - device added to Filter Accept List
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventDeviceAddedToFilterAcceptListIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventDeviceAddedToFilterAcceptListIndication_t *evt = &(container->Data.GAPGenericEventDeviceAddedToFilterAcceptListIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventDeviceAddedToFilterAcceptListIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventDeviceAddedToFilterAcceptListIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventDeviceRemovedFromFilterAcceptListIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - device removed from Filter Accept List
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventDeviceRemovedFromFilterAcceptListIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventDeviceRemovedFromFilterAcceptListIndication_t *evt = &(container->Data.GAPGenericEventDeviceRemovedFromFilterAcceptListIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventDeviceRemovedFromFilterAcceptListIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventDeviceRemovedFromFilterAcceptListIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventFilterAcceptListClearedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - Filter Accept List cleared
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventFilterAcceptListClearedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventFilterAcceptListClearedIndication_t *evt = &(container->Data.GAPGenericEventFilterAcceptListClearedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventFilterAcceptListClearedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventFilterAcceptListClearedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventRandomAddressReadyIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - random address ready
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventRandomAddressReadyIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventRandomAddressReadyIndication_t *evt = &(container->Data.GAPGenericEventRandomAddressReadyIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventRandomAddressReadyIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventRandomAddressReadyIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventCreateConnectionCanceledIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - create connection procedure canceled
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventCreateConnectionCanceledIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventCreateConnectionCanceledIndication_t *evt = &(container->Data.GAPGenericEventCreateConnectionCanceledIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventCreateConnectionCanceledIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventCreateConnectionCanceledIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventPublicAddressReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - public address read
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventPublicAddressReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventPublicAddressReadIndication_t *evt = &(container->Data.GAPGenericEventPublicAddressReadIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventPublicAddressReadIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventPublicAddressReadIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventAdvTxPowerLevelReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - advertising transmission power level
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventAdvTxPowerLevelReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventAdvTxPowerLevelReadIndication_t *evt = &(container->Data.GAPGenericEventAdvTxPowerLevelReadIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventAdvTxPowerLevelReadIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventAdvTxPowerLevelReadIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventPrivateResolvableAddressVerifiedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - Private Resolvable Address verified
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventPrivateResolvableAddressVerifiedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventPrivateResolvableAddressVerifiedIndication_t *evt = &(container->Data.GAPGenericEventPrivateResolvableAddressVerifiedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventPrivateResolvableAddressVerifiedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventPrivateResolvableAddressVerifiedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventRandomAddressSetIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - A random address was set
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventRandomAddressSetIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventRandomAddressSetIndication_t *evt = &(container->Data.GAPGenericEventRandomAddressSetIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventRandomAddressSetIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventRandomAddressSetIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPAdvertisingEventStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event received when advertising has been successfully enabled or disabled
***************************************************************************************************/
static mem_status_t Load_GAPAdvertisingEventStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPAdvertisingEventStateChangedIndication_t *evt = &(container->Data.GAPAdvertisingEventStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPAdvertisingEventStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPAdvertisingEventStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPAdvertisingEventCommandFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event received when advertising could not be enabled or disabled
***************************************************************************************************/
static mem_status_t Load_GAPAdvertisingEventCommandFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPAdvertisingEventCommandFailedIndication_t *evt = &(container->Data.GAPAdvertisingEventCommandFailedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPAdvertisingEventCommandFailedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPAdvertisingEventCommandFailedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPScanningEventStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event received when scanning had been successfully enabled or disabled
***************************************************************************************************/
static mem_status_t Load_GAPScanningEventStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventStateChangedIndication_t *evt = &(container->Data.GAPScanningEventStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPScanningEventStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPScanningEventStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPScanningEventCommandFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event received when scanning could not be enabled or disabled
***************************************************************************************************/
static mem_status_t Load_GAPScanningEventCommandFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventCommandFailedIndication_t *evt = &(container->Data.GAPScanningEventCommandFailedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPScanningEventCommandFailedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPScanningEventCommandFailedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPScanningEventDeviceScannedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event received when an advertising device has been scanned
***************************************************************************************************/
static mem_status_t Load_GAPScanningEventDeviceScannedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventDeviceScannedIndication_t *evt = &(container->Data.GAPScanningEventDeviceScannedIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPScanningEventDeviceScannedIndication_FSCI_ID;

	evt->AddressType = (GAPScanningEventDeviceScannedIndication_AddressType_t)pPayload[idx]; idx++;
	FLib_MemCpy(evt->Address, pPayload + idx, 6); idx += 6;
	evt->Rssi = pPayload[idx]; idx++;
	evt->DataLength = pPayload[idx]; idx++;

	if (evt->DataLength > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->DataLength);

		if (!evt->Data)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->DataLength); idx += evt->DataLength;
	evt->AdvEventType = (GAPScanningEventDeviceScannedIndication_AdvEventType_t)pPayload[idx]; idx++;
	evt->DirectRpaUsed = (bool_t)pPayload[idx]; idx++;


	evt->advertisingAddressResolved = (bool_t)pPayload[idx]; idx++;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventConnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A connection has been established
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventConnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventConnectedIndication_t *evt = &(container->Data.GAPConnectionEventConnectedIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventConnectedIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->ConnectionParameters.ConnInterval), pPayload + idx, sizeof(evt->ConnectionParameters.ConnInterval)); idx += sizeof(evt->ConnectionParameters.ConnInterval);
	FLib_MemCpy(&(evt->ConnectionParameters.ConnLatency), pPayload + idx, sizeof(evt->ConnectionParameters.ConnLatency)); idx += sizeof(evt->ConnectionParameters.ConnLatency);
	FLib_MemCpy(&(evt->ConnectionParameters.SupervisionTimeout), pPayload + idx, sizeof(evt->ConnectionParameters.SupervisionTimeout)); idx += sizeof(evt->ConnectionParameters.SupervisionTimeout);
	evt->ConnectionParameters.CentralClockAccuracy = (GAPConnectionEventConnectedIndication_ConnectionParameters_CentralClockAccuracy_t)pPayload[idx]; idx++;
	evt->PeerAddressType = (GAPConnectionEventConnectedIndication_PeerAddressType_t)pPayload[idx]; idx++;
	FLib_MemCpy(evt->PeerAddress, pPayload + idx, 6); idx += 6;
	evt->peerRpaResolved = (bool_t)pPayload[idx]; idx++;


	evt->localRpaUsed = (bool_t)pPayload[idx]; idx++;


	evt->connectionRole = (GAPConnectionEventConnectedIndication_connectionRole_t)pPayload[idx]; idx++;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventPairingRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A pairing request has been received from the peer Central
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventPairingRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPairingRequestIndication_t *evt = &(container->Data.GAPConnectionEventPairingRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventPairingRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPairingRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventPeripheralSecurityRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A Peripheral Security Request has been received from the peer Peripheral
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventPeripheralSecurityRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPeripheralSecurityRequestIndication_t *evt = &(container->Data.GAPConnectionEventPeripheralSecurityRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventPeripheralSecurityRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPeripheralSecurityRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventPairingResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A pairing response has been received from the peer Peripheral
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventPairingResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPairingResponseIndication_t *evt = &(container->Data.GAPConnectionEventPairingResponseIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventPairingResponseIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPairingResponseIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventAuthenticationRejectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A link encryption or pairing request has been rejected by the peer Peripheral
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventAuthenticationRejectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventAuthenticationRejectedIndication_t *evt = &(container->Data.GAPConnectionEventAuthenticationRejectedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventAuthenticationRejectedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventAuthenticationRejectedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventPasskeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Peer Peripheral has requested a passkey (maximum 6 digit PIN) for the pairing procedure
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventPasskeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPasskeyRequestIndication_t *evt = &(container->Data.GAPConnectionEventPasskeyRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventPasskeyRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPasskeyRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventOobRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Out-of-Band data must be provided for the pairing procedure
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventOobRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventOobRequestIndication_t *evt = &(container->Data.GAPConnectionEventOobRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventOobRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventOobRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventPasskeyDisplayIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The pairing procedure requires this Peripheral to display the passkey for the Central's user
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventPasskeyDisplayIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPasskeyDisplayIndication_t *evt = &(container->Data.GAPConnectionEventPasskeyDisplayIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventPasskeyDisplayIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPasskeyDisplayIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventKeyExchangeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The pairing procedure requires the SMP keys to be distributed to the peer
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventKeyExchangeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventKeyExchangeRequestIndication_t *evt = &(container->Data.GAPConnectionEventKeyExchangeRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventKeyExchangeRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventKeyExchangeRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventKeysReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	SMP keys distributed by the peer during pairing have been received
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventKeysReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventKeysReceivedIndication_t *evt = &(container->Data.GAPConnectionEventKeysReceivedIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventKeysReceivedIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->Keys.LtkIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->Keys.LtkIncluded)
	{
		evt->Keys.LtkInfo.LtkSize = pPayload[idx]; idx++;

		if (evt->Keys.LtkInfo.LtkSize > 0)
		{
			evt->Keys.LtkInfo.Ltk = MEM_BufferAlloc(evt->Keys.LtkInfo.LtkSize);

			if (!evt->Keys.LtkInfo.Ltk)
			{
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Keys.LtkInfo.Ltk = NULL;
		}

		FLib_MemCpy(evt->Keys.LtkInfo.Ltk, pPayload + idx, evt->Keys.LtkInfo.LtkSize); idx += evt->Keys.LtkInfo.LtkSize;
	}

	evt->Keys.IrkIncluded = (bool_t)pPayload[idx]; idx++;


	evt->Keys.CsrkIncluded = (bool_t)pPayload[idx]; idx++;



	if (evt->Keys.LtkIncluded)
	{
		evt->Keys.RandEdivInfo.RandSize = pPayload[idx]; idx++;

		if (evt->Keys.RandEdivInfo.RandSize > 0)
		{
			evt->Keys.RandEdivInfo.Rand = MEM_BufferAlloc(evt->Keys.RandEdivInfo.RandSize);

			if (!evt->Keys.RandEdivInfo.Rand)
			{
				MEM_BufferFree(evt->Keys.LtkInfo.Ltk);
				return kStatus_MemAllocError;
			}

		}
		else
		{
			evt->Keys.RandEdivInfo.Rand = NULL;
		}

		FLib_MemCpy(evt->Keys.RandEdivInfo.Rand, pPayload + idx, evt->Keys.RandEdivInfo.RandSize); idx += evt->Keys.RandEdivInfo.RandSize;
		FLib_MemCpy(&(evt->Keys.RandEdivInfo.Ediv), pPayload + idx, sizeof(evt->Keys.RandEdivInfo.Ediv)); idx += sizeof(evt->Keys.RandEdivInfo.Ediv);
	}


	if (evt->Keys.IrkIncluded)
	{
		evt->Keys.AddressIncluded = (bool_t)pPayload[idx]; idx++;
	}



	if (evt->Keys.AddressIncluded)
	{
		evt->Keys.AddressInfo.DeviceAddressType = (GAPConnectionEventKeysReceivedIndication_Keys_AddressInfo_DeviceAddressType_t)pPayload[idx]; idx++;
		FLib_MemCpy(evt->Keys.AddressInfo.DeviceAddress, pPayload + idx, 6); idx += 6;
	}


	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventLongTermKeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The bonded peer Central has requested link encryption and the LTK must be provided
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventLongTermKeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventLongTermKeyRequestIndication_t *evt = &(container->Data.GAPConnectionEventLongTermKeyRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventLongTermKeyRequestIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Ediv), pPayload + idx, sizeof(evt->Ediv)); idx += sizeof(evt->Ediv);
	evt->RandSize = pPayload[idx]; idx++;

	if (evt->RandSize > 0)
	{
		evt->Rand = MEM_BufferAlloc(evt->RandSize);

		if (!evt->Rand)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Rand = NULL;
	}

	FLib_MemCpy(evt->Rand, pPayload + idx, evt->RandSize); idx += evt->RandSize;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventEncryptionChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Link's encryption state has changed, e.g. during pairing or after a reconnection with a bonded peer
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventEncryptionChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventEncryptionChangedIndication_t *evt = &(container->Data.GAPConnectionEventEncryptionChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventEncryptionChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventEncryptionChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventPairingCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Pairing procedure is complete, either successfully or with failure
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventPairingCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPairingCompleteIndication_t *evt = &(container->Data.GAPConnectionEventPairingCompleteIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventPairingCompleteIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->PairingStatus = (GAPConnectionEventPairingCompleteIndication_PairingStatus_t)pPayload[idx]; idx++;

	switch (evt->PairingStatus)
	{
		case GAPConnectionEventPairingCompleteIndication_PairingStatus_PairingSuccessful:
			evt->PairingData.PairingSuccessful_WithBonding = (bool_t)pPayload[idx]; idx++;
			break;

		case GAPConnectionEventPairingCompleteIndication_PairingStatus_PairingFailed:
			FLib_MemCpy(&(evt->PairingData.PairingFailed_FailReason), pPayload + idx, 2); idx += 2;
			break;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventDisconnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A connection has been terminated
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventDisconnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventDisconnectedIndication_t *evt = &(container->Data.GAPConnectionEventDisconnectedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventDisconnectedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventDisconnectedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventRssiReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	RSSI for an active connection has been read
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventRssiReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventRssiReadIndication_t *evt = &(container->Data.GAPConnectionEventRssiReadIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventRssiReadIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventRssiReadIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventTxPowerLevelReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	TX power level for an active connection has been read
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventTxPowerLevelReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventTxPowerLevelReadIndication_t *evt = &(container->Data.GAPConnectionEventTxPowerLevelReadIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventTxPowerLevelReadIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventTxPowerLevelReadIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventPowerReadFailureIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Power reading could not be performed
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventPowerReadFailureIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPowerReadFailureIndication_t *evt = &(container->Data.GAPConnectionEventPowerReadFailureIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventPowerReadFailureIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPowerReadFailureIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventParameterUpdateRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A connection parameter update request has been received
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventParameterUpdateRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventParameterUpdateRequestIndication_t *evt = &(container->Data.GAPConnectionEventParameterUpdateRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventParameterUpdateRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventParameterUpdateRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventParameterUpdateCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The connection has new parameters
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventParameterUpdateCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventParameterUpdateCompleteIndication_t *evt = &(container->Data.GAPConnectionEventParameterUpdateCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventParameterUpdateCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventParameterUpdateCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventLeDataLengthChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The new TX/RX Data Length paramaters
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventLeDataLengthChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventLeDataLengthChangedIndication_t *evt = &(container->Data.GAPConnectionEventLeDataLengthChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventLeDataLengthChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventLeDataLengthChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventLeSetDataLengthFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Failure indication for Set Data Length command
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventLeSetDataLengthFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventLeSetDataLengthFailedIndication_t *evt = &(container->Data.GAPConnectionEventLeSetDataLengthFailedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventLeSetDataLengthFailedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventLeSetDataLengthFailedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventLeScOobDataRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event sent to request LE SC OOB Data (r, Cr and Addr) received from a peer
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventLeScOobDataRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventLeScOobDataRequestIndication_t *evt = &(container->Data.GAPConnectionEventLeScOobDataRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventLeScOobDataRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventLeScOobDataRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventLeScDisplayNumericValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event sent to display and confirm a Numeric Comparison Value when using the LE SC Numeric Comparison pairing method
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventLeScDisplayNumericValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventLeScDisplayNumericValueIndication_t *evt = &(container->Data.GAPConnectionEventLeScDisplayNumericValueIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventLeScDisplayNumericValueIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventLeScDisplayNumericValueIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventLeScKeypressNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Remote Keypress Notification recieved during Passkey Entry Pairing Method
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventLeScKeypressNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventLeScKeypressNotificationIndication_t *evt = &(container->Data.GAPConnectionEventLeScKeypressNotificationIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventLeScKeypressNotificationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventLeScKeypressNotificationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPLeScPublicKeyRegeneratedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The private/public key pair used for LE Secure Connections pairing has been regenerated
***************************************************************************************************/
static mem_status_t Load_GAPLeScPublicKeyRegeneratedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPLeScPublicKeyRegeneratedIndication_t *evt = &(container->Data.GAPLeScPublicKeyRegeneratedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPLeScPublicKeyRegeneratedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPLeScPublicKeyRegeneratedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventLeScLocalOobDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Local OOB data used for LE Secure Connections pairing
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventLeScLocalOobDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventLeScLocalOobDataIndication_t *evt = &(container->Data.GAPGenericEventLeScLocalOobDataIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventLeScLocalOobDataIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventLeScLocalOobDataIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventHostPrivacyStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The Host Privacy was enabled or disabled
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventHostPrivacyStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventHostPrivacyStateChangedIndication_t *evt = &(container->Data.GAPGenericEventHostPrivacyStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventHostPrivacyStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventHostPrivacyStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventControllerPrivacyStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The Controller Privacy was enabled or disabled
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventControllerPrivacyStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventControllerPrivacyStateChangedIndication_t *evt = &(container->Data.GAPGenericEventControllerPrivacyStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventControllerPrivacyStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventControllerPrivacyStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventTxPowerLevelSetCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Tx Power Level set completed.
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventTxPowerLevelSetCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventTxPowerLevelSetCompleteIndication_t *evt = &(container->Data.GAPGenericEventTxPowerLevelSetCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventTxPowerLevelSetCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventTxPowerLevelSetCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventLePhyEventIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Phy Mode of a connection has been updated by the Controller.
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventLePhyEventIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventLePhyEventIndication_t *evt = &(container->Data.GAPGenericEventLePhyEventIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventLePhyEventIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventLePhyEventIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGetBondedDevicesIdentityInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns a list of the identity information of bonded devices
***************************************************************************************************/
static mem_status_t Load_GAPGetBondedDevicesIdentityInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGetBondedDevicesIdentityInformationIndication_t *evt = &(container->Data.GAPGetBondedDevicesIdentityInformationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPGetBondedDevicesIdentityInformationIndication_FSCI_ID;

	evt->NbOfDeviceIdentityAddresses = pPayload[idx]; idx++;

	if (evt->NbOfDeviceIdentityAddresses > 0)
	{
		evt->IdentityAddresses = MEM_BufferAlloc(evt->NbOfDeviceIdentityAddresses * sizeof(evt->IdentityAddresses[0]));

		if (!evt->IdentityAddresses)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->IdentityAddresses = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfDeviceIdentityAddresses; i++)
	{
		evt->IdentityAddresses[i].IdentityAddressType = (GAPGetBondedDevicesIdentityInformationIndication_IdentityAddresses_IdentityAddressType_t)pPayload[idx]; idx++;
		FLib_MemCpy(evt->IdentityAddresses[i].IdentityAddress, pPayload + idx, 6); idx += 6;
		FLib_MemCpy(evt->IdentityAddresses[i].Irk, pPayload + idx, 16); idx += 16;
		evt->IdentityAddresses[i].PrivacyMode = (GAPGetBondedDevicesIdentityInformationIndication_IdentityAddresses_PrivacyMode_t)pPayload[idx]; idx++;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPControllerNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	ADV/SCAN/CONN notification event from the controller
***************************************************************************************************/
static mem_status_t Load_GAPControllerNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPControllerNotificationIndication_t *evt = &(container->Data.GAPControllerNotificationIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPControllerNotificationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPControllerNotificationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPBondCreatedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A bond has been created by the stack or the application
***************************************************************************************************/
static mem_status_t Load_GAPBondCreatedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPBondCreatedIndication_t *evt = &(container->Data.GAPBondCreatedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPBondCreatedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPBondCreatedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventChannelMapSetIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A channel map set operation has completed
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventChannelMapSetIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventChannelMapSetIndication_t *evt = &(container->Data.GAPGenericEventChannelMapSetIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventChannelMapSetIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventChannelMapSetIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventChannelMapReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A channel map read operation has completed
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventChannelMapReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventChannelMapReadIndication_t *evt = &(container->Data.GAPConnectionEventChannelMapReadIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventChannelMapReadIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventChannelMapReadIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventChannelMapReadFailureIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A channel map set operation has failed
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventChannelMapReadFailureIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventChannelMapReadFailureIndication_t *evt = &(container->Data.GAPConnectionEventChannelMapReadFailureIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventChannelMapReadFailureIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventChannelMapReadFailureIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventExtAdvertisingParamSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Extended advertising parameters setup completed
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventExtAdvertisingParamSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventExtAdvertisingParamSetupCompleteIndication_t *evt = &(container->Data.GAPGenericEventExtAdvertisingParamSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventExtAdvertisingParamSetupCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventExtAdvertisingParamSetupCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventExtAdvertisingDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Extended advertising data setup completed
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventExtAdvertisingDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventExtAdvertisingDataSetupCompleteIndication_t *evt = &(container->Data.GAPGenericEventExtAdvertisingDataSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventExtAdvertisingDataSetupCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventExtAdvertisingDataSetupCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventPeriodicAdvParamSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Periodic advertising parameters setup completed
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventPeriodicAdvParamSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventPeriodicAdvParamSetupCompleteIndication_t *evt = &(container->Data.GAPGenericEventPeriodicAdvParamSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventPeriodicAdvParamSetupCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventPeriodicAdvParamSetupCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventPeriodicAdvDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Periodic advertising data setup completed
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventPeriodicAdvDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventPeriodicAdvDataSetupCompleteIndication_t *evt = &(container->Data.GAPGenericEventPeriodicAdvDataSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventPeriodicAdvDataSetupCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventPeriodicAdvDataSetupCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventPeriodicAdvListUpdateCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	An update to the periodic advertiser list was successfully made
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventPeriodicAdvListUpdateCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventPeriodicAdvListUpdateCompleteIndication_t *evt = &(container->Data.GAPGenericEventPeriodicAdvListUpdateCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventPeriodicAdvListUpdateCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventPeriodicAdvListUpdateCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPAdvertisingEventExtAdvertisingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - extended advertising state changed indication
***************************************************************************************************/
static mem_status_t Load_GAPAdvertisingEventExtAdvertisingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPAdvertisingEventExtAdvertisingStateChangedIndication_t *evt = &(container->Data.GAPAdvertisingEventExtAdvertisingStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPAdvertisingEventExtAdvertisingStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPAdvertisingEventExtAdvertisingStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPAdvertisingEventAdvertisingSetTerminatedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - advertising was terminated in a given advertising set
***************************************************************************************************/
static mem_status_t Load_GAPAdvertisingEventAdvertisingSetTerminatedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPAdvertisingEventAdvertisingSetTerminatedIndication_t *evt = &(container->Data.GAPAdvertisingEventAdvertisingSetTerminatedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPAdvertisingEventAdvertisingSetTerminatedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPAdvertisingEventAdvertisingSetTerminatedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPAdvertisingEventExtAdvertisingSetRemoveCompletedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - advertising set(s) removed successfully from the controller
***************************************************************************************************/
static mem_status_t Load_GAPAdvertisingEventExtAdvertisingSetRemoveCompletedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPAdvertisingEventExtAdvertisingSetRemoveCompletedIndication_t *evt = &(container->Data.GAPAdvertisingEventExtAdvertisingSetRemoveCompletedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPAdvertisingEventExtAdvertisingSetRemoveCompletedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPAdvertisingEventExtAdvertisingSetRemoveCompletedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPAdvertisingEventExtScanReqReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - a SCAN_REQ PDU or an AUX_SCAN_REQ PDU has been received
***************************************************************************************************/
static mem_status_t Load_GAPAdvertisingEventExtScanReqReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPAdvertisingEventExtScanReqReceivedIndication_t *evt = &(container->Data.GAPAdvertisingEventExtScanReqReceivedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPAdvertisingEventExtScanReqReceivedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPAdvertisingEventExtScanReqReceivedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventPeriodicAdvertisingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - Periodic advertising state changed indication
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventPeriodicAdvertisingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventPeriodicAdvertisingStateChangedIndication_t *evt = &(container->Data.GAPGenericEventPeriodicAdvertisingStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventPeriodicAdvertisingStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventPeriodicAdvertisingStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPScanningEventExtDeviceScannedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event received when an extended advertising device has been scanned
***************************************************************************************************/
static mem_status_t Load_GAPScanningEventExtDeviceScannedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventExtDeviceScannedIndication_t *evt = &(container->Data.GAPScanningEventExtDeviceScannedIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPScanningEventExtDeviceScannedIndication_FSCI_ID;

	evt->AddressType = (GAPScanningEventExtDeviceScannedIndication_AddressType_t)pPayload[idx]; idx++;
	FLib_MemCpy(evt->Address, pPayload + idx, 6); idx += 6;
	evt->SID = pPayload[idx]; idx++;
	evt->AdvertisingAddressResolved = (bool_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->AdvEventProperties), pPayload + idx, sizeof(evt->AdvEventProperties)); idx += sizeof(evt->AdvEventProperties);
	evt->Rssi = pPayload[idx]; idx++;
	evt->TxPower = pPayload[idx]; idx++;
	evt->PrimaryPHY = pPayload[idx]; idx++;
	evt->SecondaryPHY = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->PeriodicAdvInterval), pPayload + idx, sizeof(evt->PeriodicAdvInterval)); idx += sizeof(evt->PeriodicAdvInterval);
	evt->DirectRpaUsed = (bool_t)pPayload[idx]; idx++;
	evt->DirectRpaType = (GAPScanningEventExtDeviceScannedIndication_DirectRpaType_t)pPayload[idx]; idx++;
	FLib_MemCpy(evt->DirectRpa, pPayload + idx, 6); idx += 6;
	FLib_MemCpy(&(evt->DataLength), pPayload + idx, sizeof(evt->DataLength)); idx += sizeof(evt->DataLength);

	if (evt->DataLength > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->DataLength);

		if (!evt->Data)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->DataLength); idx += evt->DataLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPScanningEventPeriodicAdvSyncEstablishedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - periodic advertising sync established indication
***************************************************************************************************/
static mem_status_t Load_GAPScanningEventPeriodicAdvSyncEstablishedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventPeriodicAdvSyncEstablishedIndication_t *evt = &(container->Data.GAPScanningEventPeriodicAdvSyncEstablishedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPScanningEventPeriodicAdvSyncEstablishedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPScanningEventPeriodicAdvSyncEstablishedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPScanningEventPeriodicAdvSyncTerminatedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - the controller stopped receiving periodic advertisements
***************************************************************************************************/
static mem_status_t Load_GAPScanningEventPeriodicAdvSyncTerminatedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventPeriodicAdvSyncTerminatedIndication_t *evt = &(container->Data.GAPScanningEventPeriodicAdvSyncTerminatedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPScanningEventPeriodicAdvSyncTerminatedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPScanningEventPeriodicAdvSyncTerminatedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPScanningEventPeriodicAdvSyncLostIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - the controller has not received a periodic advertising packet identified by the handle within the timeout period
***************************************************************************************************/
static mem_status_t Load_GAPScanningEventPeriodicAdvSyncLostIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventPeriodicAdvSyncLostIndication_t *evt = &(container->Data.GAPScanningEventPeriodicAdvSyncLostIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPScanningEventPeriodicAdvSyncLostIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPScanningEventPeriodicAdvSyncLostIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPScanningEventPeriodicDeviceScannedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event received when periodic advertising has been received
***************************************************************************************************/
static mem_status_t Load_GAPScanningEventPeriodicDeviceScannedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventPeriodicDeviceScannedIndication_t *evt = &(container->Data.GAPScanningEventPeriodicDeviceScannedIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPScanningEventPeriodicDeviceScannedIndication_FSCI_ID;

	FLib_MemCpy(&(evt->SyncHandle), pPayload + idx, sizeof(evt->SyncHandle)); idx += sizeof(evt->SyncHandle);
	evt->Rssi = pPayload[idx]; idx++;
	evt->TxPower = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->DataLength), pPayload + idx, sizeof(evt->DataLength)); idx += sizeof(evt->DataLength);

	if (evt->DataLength > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->DataLength);

		if (!evt->Data)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->DataLength); idx += evt->DataLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventPeriodicAdvCreateSyncCancelledIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - periodic advertising create sync procedure canceled
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventPeriodicAdvCreateSyncCancelledIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventPeriodicAdvCreateSyncCancelledIndication_t *evt = &(container->Data.GAPGenericEventPeriodicAdvCreateSyncCancelledIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventPeriodicAdvCreateSyncCancelledIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventPeriodicAdvCreateSyncCancelledIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventChannelSelectionAlgorithm2Indication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	LE Channel Selection Algorithm #2 is used on the data channel connection
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventChannelSelectionAlgorithm2Indication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventChannelSelectionAlgorithm2Indication_t *evt = &(container->Data.GAPConnectionEventChannelSelectionAlgorithm2Indication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventChannelSelectionAlgorithm2Indication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventChannelSelectionAlgorithm2Indication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventTxEntryAvailableIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	At least one Tx entry is available in the L2CAP queue
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventTxEntryAvailableIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventTxEntryAvailableIndication_t *evt = &(container->Data.GAPGenericEventTxEntryAvailableIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventTxEntryAvailableIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventTxEntryAvailableIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventControllerLocalRPAReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - local resolvable private address read
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventControllerLocalRPAReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventControllerLocalRPAReadIndication_t *evt = &(container->Data.GAPGenericEventControllerLocalRPAReadIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventControllerLocalRPAReadIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventControllerLocalRPAReadIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPCheckNvmIndexIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns whether an NVM index is free or not
***************************************************************************************************/
static mem_status_t Load_GAPCheckNvmIndexIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPCheckNvmIndexIndication_t *evt = &(container->Data.GAPCheckNvmIndexIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPCheckNvmIndexIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPCheckNvmIndexIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGetDeviceIdFromConnHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the device id corresponding to a connection handle
***************************************************************************************************/
static mem_status_t Load_GAPGetDeviceIdFromConnHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGetDeviceIdFromConnHandleIndication_t *evt = &(container->Data.GAPGetDeviceIdFromConnHandleIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGetDeviceIdFromConnHandleIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGetDeviceIdFromConnHandleIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGetConnectionHandleFromDeviceIdIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the connection handle corresponding to a device id
***************************************************************************************************/
static mem_status_t Load_GAPGetConnectionHandleFromDeviceIdIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGetConnectionHandleFromDeviceIdIndication_t *evt = &(container->Data.GAPGetConnectionHandleFromDeviceIdIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGetConnectionHandleFromDeviceIdIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGetConnectionHandleFromDeviceIdIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPPairingEventNoLTKIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	No LTK was found for the Master peer
***************************************************************************************************/
static mem_status_t Load_GAPPairingEventNoLTKIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPPairingEventNoLTKIndication_t *evt = &(container->Data.GAPPairingEventNoLTKIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPPairingEventNoLTKIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPPairingEventNoLTKIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPPairingAlreadyStartedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Pairing was already started on this device ID
***************************************************************************************************/
static mem_status_t Load_GAPPairingAlreadyStartedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPPairingAlreadyStartedIndication_t *evt = &(container->Data.GAPPairingAlreadyStartedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPPairingAlreadyStartedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPPairingAlreadyStartedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventConnectionlessCteTransmitParamsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Connectionless CTE transmit parameters have been successfully set
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventConnectionlessCteTransmitParamsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventConnectionlessCteTransmitParamsSetupCompleteIndication_t *evt = &(container->Data.GAPGenericEventConnectionlessCteTransmitParamsSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventConnectionlessCteTransmitParamsSetupCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventConnectionlessCteTransmitParamsSetupCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventConnectionlessCteTransmitStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Connectionless CTE for an advertising set was enabled or disabled
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventConnectionlessCteTransmitStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventConnectionlessCteTransmitStateChangedIndication_t *evt = &(container->Data.GAPGenericEventConnectionlessCteTransmitStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventConnectionlessCteTransmitStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventConnectionlessCteTransmitStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventConnectionlessIqSamplingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Connectionless CTE IQ sampling for an advertising train was enabled or disabled
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventConnectionlessIqSamplingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventConnectionlessIqSamplingStateChangedIndication_t *evt = &(container->Data.GAPGenericEventConnectionlessIqSamplingStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventConnectionlessIqSamplingStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventConnectionlessIqSamplingStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventAntennaInformationReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Antenna information was read from the controller
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventAntennaInformationReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventAntennaInformationReadIndication_t *evt = &(container->Data.GAPGenericEventAntennaInformationReadIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventAntennaInformationReadIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventAntennaInformationReadIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPScanningEventConnectionlessIqReportReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	IQ information from the CTE of a received advertising packet was reported
***************************************************************************************************/
static mem_status_t Load_GAPScanningEventConnectionlessIqReportReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventConnectionlessIqReportReceivedIndication_t *evt = &(container->Data.GAPScanningEventConnectionlessIqReportReceivedIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPScanningEventConnectionlessIqReportReceivedIndication_FSCI_ID;

	FLib_MemCpy(&(evt->SyncHandle), pPayload + idx, sizeof(evt->SyncHandle)); idx += sizeof(evt->SyncHandle);
	evt->ChannelIndex = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Rssi), pPayload + idx, sizeof(evt->Rssi)); idx += sizeof(evt->Rssi);
	evt->RssiAntennaId = pPayload[idx]; idx++;
	evt->CteType = (GAPScanningEventConnectionlessIqReportReceivedIndication_CteType_t)pPayload[idx]; idx++;
	evt->SlotDurations = (GAPScanningEventConnectionlessIqReportReceivedIndication_SlotDurations_t)pPayload[idx]; idx++;
	evt->PacketStatus = (GAPScanningEventConnectionlessIqReportReceivedIndication_PacketStatus_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->PeriodicEventCounter), pPayload + idx, sizeof(evt->PeriodicEventCounter)); idx += sizeof(evt->PeriodicEventCounter);
	evt->sampleCount = pPayload[idx]; idx++;

	if (evt->sampleCount > 0)
	{
		evt->I_samples = MEM_BufferAlloc(evt->sampleCount);

		if (!evt->I_samples)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->I_samples = NULL;
	}

	FLib_MemCpy(evt->I_samples, pPayload + idx, evt->sampleCount); idx += evt->sampleCount;

	if (evt->sampleCount > 0)
	{
		evt->Q_samples = MEM_BufferAlloc(evt->sampleCount);

		if (!evt->Q_samples)
		{
			MEM_BufferFree(evt->I_samples);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Q_samples = NULL;
	}

	FLib_MemCpy(evt->Q_samples, pPayload + idx, evt->sampleCount); idx += evt->sampleCount;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventIqReportReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	IQ information received from a connected peer was reported
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventIqReportReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventIqReportReceivedIndication_t *evt = &(container->Data.GAPConnectionEventIqReportReceivedIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventIqReportReceivedIndication_FSCI_ID;

	evt->RxPhy = (GAPConnectionEventIqReportReceivedIndication_RxPhy_t)pPayload[idx]; idx++;
	evt->DataChannelIndex = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Rssi), pPayload + idx, sizeof(evt->Rssi)); idx += sizeof(evt->Rssi);
	evt->RssiAntennaId = pPayload[idx]; idx++;
	evt->CteType = (GAPConnectionEventIqReportReceivedIndication_CteType_t)pPayload[idx]; idx++;
	evt->SlotDurations = (GAPConnectionEventIqReportReceivedIndication_SlotDurations_t)pPayload[idx]; idx++;
	evt->PacketStatus = (GAPConnectionEventIqReportReceivedIndication_PacketStatus_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->ConnEventCounter), pPayload + idx, sizeof(evt->ConnEventCounter)); idx += sizeof(evt->ConnEventCounter);
	evt->sampleCount = pPayload[idx]; idx++;

	if (evt->sampleCount > 0)
	{
		evt->I_samples = MEM_BufferAlloc(evt->sampleCount);

		if (!evt->I_samples)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->I_samples = NULL;
	}

	FLib_MemCpy(evt->I_samples, pPayload + idx, evt->sampleCount); idx += evt->sampleCount;

	if (evt->sampleCount > 0)
	{
		evt->Q_samples = MEM_BufferAlloc(evt->sampleCount);

		if (!evt->Q_samples)
		{
			MEM_BufferFree(evt->I_samples);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Q_samples = NULL;
	}

	FLib_MemCpy(evt->Q_samples, pPayload + idx, evt->sampleCount); idx += evt->sampleCount;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventCteRequestFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CTE Request to a connected peer has failed
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventCteRequestFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventCteRequestFailedIndication_t *evt = &(container->Data.GAPConnectionEventCteRequestFailedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventCteRequestFailedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventCteRequestFailedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventCteReceiveParamsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Connection CTE receive parameters have been successfully set
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventCteReceiveParamsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventCteReceiveParamsSetupCompleteIndication_t *evt = &(container->Data.GAPConnectionEventCteReceiveParamsSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventCteReceiveParamsSetupCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventCteReceiveParamsSetupCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventCteTransmitParamsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Connection CTE transmit parameters have been successfully set
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventCteTransmitParamsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventCteTransmitParamsSetupCompleteIndication_t *evt = &(container->Data.GAPConnectionEventCteTransmitParamsSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventCteTransmitParamsSetupCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventCteTransmitParamsSetupCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventCteReqStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CTE Request procedure was enabled or disabled
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventCteReqStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventCteReqStateChangedIndication_t *evt = &(container->Data.GAPConnectionEventCteReqStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventCteReqStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventCteReqStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventCteRspStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CTE Request procedure was enabled or disabled
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventCteRspStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventCteRspStateChangedIndication_t *evt = &(container->Data.GAPConnectionEventCteRspStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventCteRspStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventCteRspStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventPeriodicAdvRecvEnableCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Enable or disable reports for the periodic advertising train command is complete
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventPeriodicAdvRecvEnableCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventPeriodicAdvRecvEnableCompleteIndication_t *evt = &(container->Data.GAPGenericEventPeriodicAdvRecvEnableCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventPeriodicAdvRecvEnableCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventPeriodicAdvRecvEnableCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventPeriodicAdvSyncTransferCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The command used to instruct the Controller to send synchronization information about the advertising
                train to a connected device is complete
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventPeriodicAdvSyncTransferCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventPeriodicAdvSyncTransferCompleteIndication_t *evt = &(container->Data.GAPGenericEventPeriodicAdvSyncTransferCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventPeriodicAdvSyncTransferCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventPeriodicAdvSyncTransferCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventPeriodicAdvSetInfoTransferCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The command used to instruct the Controller to send synchronization information about the periodic advertising in an
                advertising set to a connected device is complete
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventPeriodicAdvSetInfoTransferCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventPeriodicAdvSetInfoTransferCompleteIndication_t *evt = &(container->Data.GAPGenericEventPeriodicAdvSetInfoTransferCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventPeriodicAdvSetInfoTransferCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventPeriodicAdvSetInfoTransferCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventSetPeriodicAdvSyncTransferParamsCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The command specifying how the Controller will process periodic advertising synchronization information is complete
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventSetPeriodicAdvSyncTransferParamsCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventSetPeriodicAdvSyncTransferParamsCompleteIndication_t *evt = &(container->Data.GAPGenericEventSetPeriodicAdvSyncTransferParamsCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventSetPeriodicAdvSyncTransferParamsCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventSetPeriodicAdvSyncTransferParamsCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventSetDefaultPeriodicAdvSyncTransferParamsCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The command which sets the default parameters for  periodic advertising synchronization information is complete
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventSetDefaultPeriodicAdvSyncTransferParamsCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventSetDefaultPeriodicAdvSyncTransferParamsCompleteIndication_t *evt = &(container->Data.GAPGenericEventSetDefaultPeriodicAdvSyncTransferParamsCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventSetDefaultPeriodicAdvSyncTransferParamsCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventSetDefaultPeriodicAdvSyncTransferParamsCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPScanningEventPeriodicAdvSyncTransferReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The synchronization to the periodic advertising was completed
***************************************************************************************************/
static mem_status_t Load_GAPScanningEventPeriodicAdvSyncTransferReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventPeriodicAdvSyncTransferReceivedIndication_t *evt = &(container->Data.GAPScanningEventPeriodicAdvSyncTransferReceivedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPScanningEventPeriodicAdvSyncTransferReceivedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPScanningEventPeriodicAdvSyncTransferReceivedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventPathLossThresholdIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Path Loss Threshold event received
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventPathLossThresholdIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPathLossThresholdIndication_t *evt = &(container->Data.GAPConnectionEventPathLossThresholdIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventPathLossThresholdIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPathLossThresholdIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventTransmitPowerReportingIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A Transmit Power Report was received
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventTransmitPowerReportingIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventTransmitPowerReportingIndication_t *evt = &(container->Data.GAPConnectionEventTransmitPowerReportingIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventTransmitPowerReportingIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventTransmitPowerReportingIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventEnhancedReadTransmitPowerLevelIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Local information has been read
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventEnhancedReadTransmitPowerLevelIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventEnhancedReadTransmitPowerLevelIndication_t *evt = &(container->Data.GAPConnectionEventEnhancedReadTransmitPowerLevelIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventEnhancedReadTransmitPowerLevelIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventEnhancedReadTransmitPowerLevelIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventPathLossReportingParamsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Path Loss Reporting parameters have been successfully set
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventPathLossReportingParamsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPathLossReportingParamsSetupCompleteIndication_t *evt = &(container->Data.GAPConnectionEventPathLossReportingParamsSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventPathLossReportingParamsSetupCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPathLossReportingParamsSetupCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventPathLossReportingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Path Loss Reporting has been enabled or disabled
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventPathLossReportingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPathLossReportingStateChangedIndication_t *evt = &(container->Data.GAPConnectionEventPathLossReportingStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventPathLossReportingStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPathLossReportingStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventTransmitPowerReportingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Transmit Power Reporting has been enabled or disabled for local and/or remote Controllers
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventTransmitPowerReportingStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventTransmitPowerReportingStateChangedIndication_t *evt = &(container->Data.GAPConnectionEventTransmitPowerReportingStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventTransmitPowerReportingStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventTransmitPowerReportingStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventEattConnectionRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GAP EATT Enhanced Connection Request event
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventEattConnectionRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventEattConnectionRequestIndication_t *evt = &(container->Data.GAPConnectionEventEattConnectionRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventEattConnectionRequestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventEattConnectionRequestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventEattConnectionCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GAP EATT Enhanced Connection Complete event
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventEattConnectionCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventEattConnectionCompleteIndication_t *evt = &(container->Data.GAPConnectionEventEattConnectionCompleteIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventEattConnectionCompleteIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->Status = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->MTU), pPayload + idx, sizeof(evt->MTU)); idx += sizeof(evt->MTU);
	evt->NoOfBearers = pPayload[idx]; idx++;

	if (evt->NoOfBearers > 0)
	{
		evt->BearerIDs = MEM_BufferAlloc(evt->NoOfBearers);

		if (!evt->BearerIDs)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->BearerIDs = NULL;
	}

	FLib_MemCpy(evt->BearerIDs, pPayload + idx, evt->NoOfBearers); idx += evt->NoOfBearers;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventEattReconfigureResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GAP EATT Reconfigure Response event
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventEattReconfigureResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventEattReconfigureResponseIndication_t *evt = &(container->Data.GAPConnectionEventEattReconfigureResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventEattReconfigureResponseIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->Status = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->LocalMtu), pPayload + idx, sizeof(evt->LocalMtu)); idx += sizeof(evt->LocalMtu);
	evt->NoOfBearers = pPayload[idx]; idx++;

	if (evt->NoOfBearers > 0)
	{
		evt->BearerIDs = MEM_BufferAlloc(evt->NoOfBearers);

		if (!evt->BearerIDs)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->BearerIDs = NULL;
	}

	FLib_MemCpy(evt->BearerIDs, pPayload + idx, evt->NoOfBearers); idx += evt->NoOfBearers;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPConnectionEventEattBearerStatusNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GAP EATT Reconfigure Response event
***************************************************************************************************/
static mem_status_t Load_GAPConnectionEventEattBearerStatusNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventEattBearerStatusNotificationIndication_t *evt = &(container->Data.GAPConnectionEventEattBearerStatusNotificationIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPConnectionEventEattBearerStatusNotificationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventEattBearerStatusNotificationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventLeGenerateDhKeyCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GAP LE DHKey generation complete event
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventLeGenerateDhKeyCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventLeGenerateDhKeyCompleteIndication_t *evt = &(container->Data.GAPGenericEventLeGenerateDhKeyCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventLeGenerateDhKeyCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventLeGenerateDhKeyCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGetHostVersionIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns Host Version information
***************************************************************************************************/
static mem_status_t Load_GAPGetHostVersionIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGetHostVersionIndication_t *evt = &(container->Data.GAPGetHostVersionIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGetHostVersionIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGetHostVersionIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGenericEventReadRemoteVersionInfoCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Read Remote Version Information Complete Event
***************************************************************************************************/
static mem_status_t Load_GAPGenericEventReadRemoteVersionInfoCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventReadRemoteVersionInfoCompleteIndication_t *evt = &(container->Data.GAPGenericEventReadRemoteVersionInfoCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGenericEventReadRemoteVersionInfoCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventReadRemoteVersionInfoCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPGetConnParamsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Get Connection Parameters Complete Event
***************************************************************************************************/
static mem_status_t Load_GAPGetConnParamsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGetConnParamsIndication_t *evt = &(container->Data.GAPGetConnParamsIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPGetConnParamsIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGetConnParamsIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAPPlatformErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Platform error callback event
***************************************************************************************************/
static mem_status_t Load_GAPPlatformErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPPlatformErrorIndication_t *evt = &(container->Data.GAPPlatformErrorIndication);

	/* Store (OG, OC) in ID */
	container->id = GAPPlatformErrorIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAPPlatformErrorIndication_t));

	return kStatus_MemSuccess;
}

#endif  /* GAP_ENABLE */

#if APP_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_APPThroughputTestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Start a throughput test
***************************************************************************************************/
static mem_status_t Load_APPThroughputTestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	APPThroughputTestIndication_t *evt = &(container->Data.APPThroughputTestIndication);

	/* Store (OG, OC) in ID */
	container->id = APPThroughputTestIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(APPThroughputTestIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_APPThroughputTestConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Start a throughput test
***************************************************************************************************/
static mem_status_t Load_APPThroughputTestConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	APPThroughputTestConfirm_t *evt = &(container->Data.APPThroughputTestConfirm);

	/* Store (OG, OC) in ID */
	container->id = APPThroughputTestConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(APPThroughputTestConfirm_t));

	return kStatus_MemSuccess;
}

#endif  /* APP_ENABLE */

#if CS_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_CSConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the CS request
***************************************************************************************************/
static mem_status_t Load_CSConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSConfirm_t *evt = &(container->Data.CSConfirm);

	/* Store (OG, OC) in ID */
	container->id = CSConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSReadRemoteSupportedCapabilitiesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Read Remote Supported Capabilities Event Data
***************************************************************************************************/
static mem_status_t Load_CSReadRemoteSupportedCapabilitiesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSReadRemoteSupportedCapabilitiesIndication_t *evt = &(container->Data.CSReadRemoteSupportedCapabilitiesIndication);

	/* Store (OG, OC) in ID */
	container->id = CSReadRemoteSupportedCapabilitiesIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSReadRemoteSupportedCapabilitiesIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSReadRemoteFAETableIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Read Remote FAE Table Event Data
***************************************************************************************************/
static mem_status_t Load_CSReadRemoteFAETableIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSReadRemoteFAETableIndication_t *evt = &(container->Data.CSReadRemoteFAETableIndication);

	/* Store (OG, OC) in ID */
	container->id = CSReadRemoteFAETableIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSReadRemoteFAETableIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSSecurityEnableIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Security Enable Event Data
***************************************************************************************************/
static mem_status_t Load_CSSecurityEnableIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSSecurityEnableIndication_t *evt = &(container->Data.CSSecurityEnableIndication);

	/* Store (OG, OC) in ID */
	container->id = CSSecurityEnableIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSSecurityEnableIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSConfigCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Config Complete Event Data
***************************************************************************************************/
static mem_status_t Load_CSConfigCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSConfigCompleteIndication_t *evt = &(container->Data.CSConfigCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = CSConfigCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSConfigCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSProcedureEnableCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Procedure Enable Complete Event Data
***************************************************************************************************/
static mem_status_t Load_CSProcedureEnableCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSProcedureEnableCompleteIndication_t *evt = &(container->Data.CSProcedureEnableCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = CSProcedureEnableCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSProcedureEnableCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSSubeventResultIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Subevent Result Event Data
***************************************************************************************************/
static mem_status_t Load_CSSubeventResultIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSSubeventResultIndication_t *evt = &(container->Data.CSSubeventResultIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = CSSubeventResultIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->CSEventResult.ConfigId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->CSEventResult.StartACLConnEvent), pPayload + idx, sizeof(evt->CSEventResult.StartACLConnEvent)); idx += sizeof(evt->CSEventResult.StartACLConnEvent);
	FLib_MemCpy(&(evt->CSEventResult.ProcedureCounter), pPayload + idx, sizeof(evt->CSEventResult.ProcedureCounter)); idx += sizeof(evt->CSEventResult.ProcedureCounter);
	FLib_MemCpy(&(evt->CSEventResult.FrequencyCompensation), pPayload + idx, sizeof(evt->CSEventResult.FrequencyCompensation)); idx += sizeof(evt->CSEventResult.FrequencyCompensation);
	evt->CSEventResult.ReferencePowerLevel = pPayload[idx]; idx++;
	evt->CSEventResult.ProcedureDoneStatus = pPayload[idx]; idx++;
	evt->CSEventResult.SubeventDoneStatus = pPayload[idx]; idx++;
	evt->CSEventResult.AbortReason = pPayload[idx]; idx++;
	evt->CSEventResult.NumAntennaPaths = pPayload[idx]; idx++;
	evt->CSEventResult.NumStepsReported = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->CSEventResult.DataLength), pPayload + idx, sizeof(evt->CSEventResult.DataLength)); idx += sizeof(evt->CSEventResult.DataLength);

	if (evt->CSEventResult.DataLength > 0)
	{
		evt->CSEventResult.StepData = MEM_BufferAlloc(evt->CSEventResult.DataLength);

		if (!evt->CSEventResult.StepData)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->CSEventResult.StepData = NULL;
	}

	FLib_MemCpy(evt->CSEventResult.StepData, pPayload + idx, evt->CSEventResult.DataLength); idx += evt->CSEventResult.DataLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSSubeventResultContinueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Subevent Result Continue Event Data
***************************************************************************************************/
static mem_status_t Load_CSSubeventResultContinueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSSubeventResultContinueIndication_t *evt = &(container->Data.CSSubeventResultContinueIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = CSSubeventResultContinueIndication_FSCI_ID;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->CSEventContinueResult.ConfigId = pPayload[idx]; idx++;
	evt->CSEventContinueResult.ProcedureDoneStatus = pPayload[idx]; idx++;
	evt->CSEventContinueResult.SubeventDoneStatus = pPayload[idx]; idx++;
	evt->CSEventContinueResult.AbortReason = pPayload[idx]; idx++;
	evt->CSEventContinueResult.NumAntennaPaths = pPayload[idx]; idx++;
	evt->CSEventContinueResult.NumStepsReported = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->CSEventContinueResult.DataLength), pPayload + idx, sizeof(evt->CSEventContinueResult.DataLength)); idx += sizeof(evt->CSEventContinueResult.DataLength);

	if (evt->CSEventContinueResult.DataLength > 0)
	{
		evt->CSEventContinueResult.StepData = MEM_BufferAlloc(evt->CSEventContinueResult.DataLength);

		if (!evt->CSEventContinueResult.StepData)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->CSEventContinueResult.StepData = NULL;
	}

	FLib_MemCpy(evt->CSEventContinueResult.StepData, pPayload + idx, evt->CSEventContinueResult.DataLength); idx += evt->CSEventContinueResult.DataLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSTestEndEvtIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Test End Event Data
***************************************************************************************************/
static mem_status_t Load_CSTestEndEvtIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSTestEndEvtIndication_t *evt = &(container->Data.CSTestEndEvtIndication);

	/* Store (OG, OC) in ID */
	container->id = CSTestEndEvtIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSTestEndEvtIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSEventResultDebugIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Event Result Debug Event Data
***************************************************************************************************/
static mem_status_t Load_CSEventResultDebugIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSEventResultDebugIndication_t *evt = &(container->Data.CSEventResultDebugIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = CSEventResultDebugIndication_FSCI_ID;

	FLib_MemCpy(&(evt->DataLength), pPayload + idx, sizeof(evt->DataLength)); idx += sizeof(evt->DataLength);
	FLib_MemCpy(&(evt->BufferLength), pPayload + idx, sizeof(evt->BufferLength)); idx += sizeof(evt->BufferLength);
	FLib_MemCpy(&(evt->Offset), pPayload + idx, sizeof(evt->Offset)); idx += sizeof(evt->Offset);

	if (evt->DataLength > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->DataLength);

		if (!evt->Data)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->DataLength); idx += evt->DataLength;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSLEMetaErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Error in treating LE Meta events
***************************************************************************************************/
static mem_status_t Load_CSLEMetaErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSLEMetaErrorIndication_t *evt = &(container->Data.CSLEMetaErrorIndication);

	/* Store (OG, OC) in ID */
	container->id = CSLEMetaErrorIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSLEMetaErrorIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSReadLocalSupportedCapabilitiesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Read Local Supported Capabilities Event Data
***************************************************************************************************/
static mem_status_t Load_CSReadLocalSupportedCapabilitiesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSReadLocalSupportedCapabilitiesIndication_t *evt = &(container->Data.CSReadLocalSupportedCapabilitiesIndication);

	/* Store (OG, OC) in ID */
	container->id = CSReadLocalSupportedCapabilitiesIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSReadLocalSupportedCapabilitiesIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSSetDefaultSettingsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Set Default Settings Event Data
***************************************************************************************************/
static mem_status_t Load_CSSetDefaultSettingsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSSetDefaultSettingsIndication_t *evt = &(container->Data.CSSetDefaultSettingsIndication);

	/* Store (OG, OC) in ID */
	container->id = CSSetDefaultSettingsIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSSetDefaultSettingsIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSWriteRemoteFAETableIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Write Remote FAE Table Event Data
***************************************************************************************************/
static mem_status_t Load_CSWriteRemoteFAETableIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSWriteRemoteFAETableIndication_t *evt = &(container->Data.CSWriteRemoteFAETableIndication);

	/* Store (OG, OC) in ID */
	container->id = CSWriteRemoteFAETableIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSWriteRemoteFAETableIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSSetChannelClassificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Set Channel Classification Event Data
***************************************************************************************************/
static mem_status_t Load_CSSetChannelClassificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSSetChannelClassificationIndication_t *evt = &(container->Data.CSSetChannelClassificationIndication);

	/* Store (OG, OC) in ID */
	container->id = CSSetChannelClassificationIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSSetChannelClassificationIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSgCSEventTestCmdOpCodeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Test Command Complete Event Data
***************************************************************************************************/
static mem_status_t Load_CSgCSEventTestCmdOpCodeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSgCSEventTestCmdOpCodeIndication_t *evt = &(container->Data.CSgCSEventTestCmdOpCodeIndication);

	/* Store (OG, OC) in ID */
	container->id = CSgCSEventTestCmdOpCodeIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSgCSEventTestCmdOpCodeIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSgCSEventSetProcedureParamsCmdOpCodeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Set Procedure Parameters Command Complete Event Data
***************************************************************************************************/
static mem_status_t Load_CSgCSEventSetProcedureParamsCmdOpCodeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSgCSEventSetProcedureParamsCmdOpCodeIndication_t *evt = &(container->Data.CSgCSEventSetProcedureParamsCmdOpCodeIndication);

	/* Store (OG, OC) in ID */
	container->id = CSgCSEventSetProcedureParamsCmdOpCodeIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSgCSEventSetProcedureParamsCmdOpCodeIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Error Event Data
***************************************************************************************************/
static mem_status_t Load_CSErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSErrorIndication_t *evt = &(container->Data.CSErrorIndication);

	/* Store (OG, OC) in ID */
	container->id = CSErrorIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSErrorIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSgCsEventWriteCachedRemoteCapabilitiesOpCodeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Write Cached Remote Capabilities Command Complete Event Data
***************************************************************************************************/
static mem_status_t Load_CSgCsEventWriteCachedRemoteCapabilitiesOpCodeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSgCsEventWriteCachedRemoteCapabilitiesOpCodeIndication_t *evt = &(container->Data.CSgCsEventWriteCachedRemoteCapabilitiesOpCodeIndication);

	/* Store (OG, OC) in ID */
	container->id = CSgCsEventWriteCachedRemoteCapabilitiesOpCodeIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSgCsEventWriteCachedRemoteCapabilitiesOpCodeIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_CSCommandStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	CS Error Event Data
***************************************************************************************************/
static mem_status_t Load_CSCommandStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	CSCommandStatusIndication_t *evt = &(container->Data.CSCommandStatusIndication);

	/* Store (OG, OC) in ID */
	container->id = CSCommandStatusIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(CSCommandStatusIndication_t));

	return kStatus_MemSuccess;
}

#endif  /* CS_ENABLE */

#if GAPHANDOVER_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the GAP request
***************************************************************************************************/
static mem_status_t Load_GapHandoverConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverConfirm_t *evt = &(container->Data.GapHandoverConfirm);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverConfirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverConfirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverGetDataSizeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the Handover Data size for a given peer 
***************************************************************************************************/
static mem_status_t Load_GapHandoverGetDataSizeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverGetDataSizeIndication_t *evt = &(container->Data.GapHandoverGetDataSizeIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverGetDataSizeIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverGetDataSizeIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverGetCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Handover data was copied for a given peer 
***************************************************************************************************/
static mem_status_t Load_GapHandoverGetCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverGetCompleteIndication_t *evt = &(container->Data.GapHandoverGetCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverGetCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverGetCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverSetCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Handover data was set 
***************************************************************************************************/
static mem_status_t Load_GapHandoverSetCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverSetCompleteIndication_t *evt = &(container->Data.GapHandoverSetCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverSetCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverSetCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverGetTimeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Timing information was received from the Link Layer 
***************************************************************************************************/
static mem_status_t Load_GapHandoverGetTimeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverGetTimeIndication_t *evt = &(container->Data.GapHandoverGetTimeIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverGetTimeIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverGetTimeIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverSuspendTransmitCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Tx for a connection was suspended 
***************************************************************************************************/
static mem_status_t Load_GapHandoverSuspendTransmitCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverSuspendTransmitCompleteIndication_t *evt = &(container->Data.GapHandoverSuspendTransmitCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverSuspendTransmitCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverSuspendTransmitCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverResumeTransmitCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Tx for a connection was resumed 
***************************************************************************************************/
static mem_status_t Load_GapHandoverResumeTransmitCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverResumeTransmitCompleteIndication_t *evt = &(container->Data.GapHandoverResumeTransmitCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverResumeTransmitCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverResumeTransmitCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverAnchorNotificationStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Anchor notifications were enabled or disabled for a peer 
***************************************************************************************************/
static mem_status_t Load_GapHandoverAnchorNotificationStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverAnchorNotificationStateChangedIndication_t *evt = &(container->Data.GapHandoverAnchorNotificationStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverAnchorNotificationStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverAnchorNotificationStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverAnchorSearchStartedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Anchor search was started for a peer
***************************************************************************************************/
static mem_status_t Load_GapHandoverAnchorSearchStartedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverAnchorSearchStartedIndication_t *evt = &(container->Data.GapHandoverAnchorSearchStartedIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverAnchorSearchStartedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverAnchorSearchStartedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverAnchorSearchStoppedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Anchor search was stopped for a peer
***************************************************************************************************/
static mem_status_t Load_GapHandoverAnchorSearchStoppedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverAnchorSearchStoppedIndication_t *evt = &(container->Data.GapHandoverAnchorSearchStoppedIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverAnchorSearchStoppedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverAnchorSearchStoppedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverAnchorMonitorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Handover Anchor Monitor was received from Controller
***************************************************************************************************/
static mem_status_t Load_GapHandoverAnchorMonitorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverAnchorMonitorIndication_t *evt = &(container->Data.GapHandoverAnchorMonitorIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverAnchorMonitorIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverAnchorMonitorIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverAnchorMonitorPacketIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Handover Anchor Monitor Packet was received from Controller
***************************************************************************************************/
static mem_status_t Load_GapHandoverAnchorMonitorPacketIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverAnchorMonitorPacketIndication_t *evt = &(container->Data.GapHandoverAnchorMonitorPacketIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GapHandoverAnchorMonitorPacketIndication_FSCI_ID;

	evt->PacketCounter = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->ConnectionHandle), pPayload + idx, sizeof(evt->ConnectionHandle)); idx += sizeof(evt->ConnectionHandle);
	evt->StatusPacket = pPayload[idx]; idx++;
	evt->PHY = pPayload[idx]; idx++;
	evt->ChIdx = pPayload[idx]; idx++;
	evt->RssiPacket = pPayload[idx]; idx++;
	evt->LqiPacket = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->ConnEvent), pPayload + idx, sizeof(evt->ConnEvent)); idx += sizeof(evt->ConnEvent);
	FLib_MemCpy(&(evt->AnchorClock625Us), pPayload + idx, sizeof(evt->AnchorClock625Us)); idx += sizeof(evt->AnchorClock625Us);
	FLib_MemCpy(&(evt->AnchorDelay), pPayload + idx, sizeof(evt->AnchorDelay)); idx += sizeof(evt->AnchorDelay);
	evt->NbConnIntervals = pPayload[idx]; idx++;
	evt->PduSize = pPayload[idx]; idx++;

	if (evt->PduSize > 0)
	{
		evt->Pdu = MEM_BufferAlloc(evt->PduSize);

		if (!evt->Pdu)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Pdu = NULL;
	}

	FLib_MemCpy(evt->Pdu, pPayload + idx, evt->PduSize); idx += evt->PduSize;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverAnchorMonitorPacketContinueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Handover Anchor Monitor Packet Continue was received from Controller
***************************************************************************************************/
static mem_status_t Load_GapHandoverAnchorMonitorPacketContinueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverAnchorMonitorPacketContinueIndication_t *evt = &(container->Data.GapHandoverAnchorMonitorPacketContinueIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GapHandoverAnchorMonitorPacketContinueIndication_FSCI_ID;

	evt->PacketCounter = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->ConnectionHandle), pPayload + idx, sizeof(evt->ConnectionHandle)); idx += sizeof(evt->ConnectionHandle);
	evt->PduSize = pPayload[idx]; idx++;

	if (evt->PduSize > 0)
	{
		evt->Pdu = MEM_BufferAlloc(evt->PduSize);

		if (!evt->Pdu)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Pdu = NULL;
	}

	FLib_MemCpy(evt->Pdu, pPayload + idx, evt->PduSize); idx += evt->PduSize;

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverConnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Connection Event - Handover connection has been established
***************************************************************************************************/
static mem_status_t Load_GapHandoverConnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverConnectedIndication_t *evt = &(container->Data.GapHandoverConnectedIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverConnectedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverConnectedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverDisconnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Connection Event - Handover connection has been terminated
***************************************************************************************************/
static mem_status_t Load_GapHandoverDisconnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverDisconnectedIndication_t *evt = &(container->Data.GapHandoverDisconnectedIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverDisconnectedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverDisconnectedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverTimeSyncTransmitStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Time Sync Transmit started or stopped
***************************************************************************************************/
static mem_status_t Load_GapHandoverTimeSyncTransmitStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverTimeSyncTransmitStateChangedIndication_t *evt = &(container->Data.GapHandoverTimeSyncTransmitStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverTimeSyncTransmitStateChangedIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverTimeSyncTransmitStateChangedIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverTimeSyncReceiveCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Time Sync Receive command was successful
***************************************************************************************************/
static mem_status_t Load_GapHandoverTimeSyncReceiveCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverTimeSyncReceiveCompleteIndication_t *evt = &(container->Data.GapHandoverTimeSyncReceiveCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverTimeSyncReceiveCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverTimeSyncReceiveCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverTimeSyncIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Handover Time Sync Event received from Controller
***************************************************************************************************/
static mem_status_t Load_GapHandoverTimeSyncIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverTimeSyncIndication_t *evt = &(container->Data.GapHandoverTimeSyncIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverTimeSyncIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverTimeSyncIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverConnParamUpdateIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Connection Parameters Update Event received from Controller
***************************************************************************************************/
static mem_status_t Load_GapHandoverConnParamUpdateIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverConnParamUpdateIndication_t *evt = &(container->Data.GapHandoverConnParamUpdateIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverConnParamUpdateIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverConnParamUpdateIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverUpdateConnParamsCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Update Connection Parameters command complete
***************************************************************************************************/
static mem_status_t Load_GapHandoverUpdateConnParamsCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverUpdateConnParamsCompleteIndication_t *evt = &(container->Data.GapHandoverUpdateConnParamsCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GapHandoverUpdateConnParamsCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GapHandoverUpdateConnParamsCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GapHandoverLlPendingDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Update Connection Parameters command complete
***************************************************************************************************/
static mem_status_t Load_GapHandoverLlPendingDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GapHandoverLlPendingDataIndication_t *evt = &(container->Data.GapHandoverLlPendingDataIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GapHandoverLlPendingDataIndication_FSCI_ID;

	FLib_MemCpy(&(evt->DataSize), pPayload + idx, sizeof(evt->DataSize)); idx += sizeof(evt->DataSize);

	if (evt->DataSize > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->DataSize * 2);

		if (!evt->Data)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->DataSize * 2); idx += evt->DataSize * 2;

	return kStatus_MemSuccess;
}

#endif  /* GAPHANDOVER_ENABLE */

#if GAP2_ENABLE
/*!*************************************************************************************************
\fn		static mem_status_t Load_GAP2Confirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the GAP request
***************************************************************************************************/
static mem_status_t Load_GAP2Confirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAP2Confirm_t *evt = &(container->Data.GAP2Confirm);

	/* Store (OG, OC) in ID */
	container->id = GAP2Confirm_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAP2Confirm_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAP2CtrlDebugInfoIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Debug Information from the LL
***************************************************************************************************/
static mem_status_t Load_GAP2CtrlDebugInfoIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAP2CtrlDebugInfoIndication_t *evt = &(container->Data.GAP2CtrlDebugInfoIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAP2CtrlDebugInfoIndication_FSCI_ID;

	evt->DebugMode = pPayload[idx]; idx++;
	evt->ErrorCount = pPayload[idx]; idx++;
	evt->WarCount = pPayload[idx]; idx++;
	evt->IssueTriggered = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Length), pPayload + idx, sizeof(evt->Length)); idx += sizeof(evt->Length);
	FLib_MemCpy(&(evt->Sha1Nbu), pPayload + idx, sizeof(evt->Sha1Nbu)); idx += sizeof(evt->Sha1Nbu);
	FLib_MemCpy(&(evt->ErrorBitmask), pPayload + idx, sizeof(evt->ErrorBitmask)); idx += sizeof(evt->ErrorBitmask);
	FLib_MemCpy(&(evt->TaskSOFmask), pPayload + idx, sizeof(evt->TaskSOFmask)); idx += sizeof(evt->TaskSOFmask);
	FLib_MemCpy(&(evt->IdleTaskFreeRunningCounter), pPayload + idx, sizeof(evt->IdleTaskFreeRunningCounter)); idx += sizeof(evt->IdleTaskFreeRunningCounter);
	evt->CurrentState = pPayload[idx]; idx++;
	FLib_MemCpy(evt->StateCount, pPayload + idx, 4); idx += 4;
	evt->AdvSchedFreeRunCnt = pPayload[idx]; idx++;
	evt->ScanSchedFreeRunCnt = pPayload[idx]; idx++;
	evt->ScanInitSchedFreeRunCnt = pPayload[idx]; idx++;
	evt->ScanCorrHitFreeRunningCounter = pPayload[idx]; idx++;
	evt->ScanCorrToFreeRunningCounter = pPayload[idx]; idx++;
	evt->TxLockFailRunningCounter = pPayload[idx]; idx++;
	evt->RxLockFailRunningCounter = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->RtErrorStatus), pPayload + idx, sizeof(evt->RtErrorStatus)); idx += sizeof(evt->RtErrorStatus);
	FLib_MemCpy(&(evt->Reserved4), pPayload + idx, sizeof(evt->Reserved4)); idx += sizeof(evt->Reserved4);
	evt->Reserved5 = pPayload[idx]; idx++;
	evt->ScanGenReportFreeRunningCounter = pPayload[idx]; idx++;
	evt->ScanGenCloseFreeRunningCounter = pPayload[idx]; idx++;
	evt->Res2ScanHwErrorFreeRunningCounter = pPayload[idx]; idx++;
	evt->AdvTxDoneFreeRunningCounter = pPayload[idx]; idx++;
	evt->Res3ScanRtErrorFreeRunningCounter = pPayload[idx]; idx++;
	evt->Res4AdvHwErrorFreeRunningCounter = pPayload[idx]; idx++;
	evt->Res5AdvRtErrorFreeRunningCounter = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->HwAbortStatus), pPayload + idx, sizeof(evt->HwAbortStatus)); idx += sizeof(evt->HwAbortStatus);
	evt->MaxCustErrInList = pPayload[idx]; idx++;
	evt->MaxErrInListDiff = pPayload[idx]; idx++;

	if (evt->MaxCustErrInList > 0)
	{
		evt->ErrorList = MEM_BufferAlloc(evt->MaxCustErrInList);

		if (!evt->ErrorList)
		{
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->ErrorList = NULL;
	}

	FLib_MemCpy(evt->ErrorList, pPayload + idx, evt->MaxCustErrInList); idx += evt->MaxCustErrInList;

	if (evt->MaxErrInListDiff > 0)
	{
		evt->Reserved1 = MEM_BufferAlloc(evt->MaxErrInListDiff);

		if (!evt->Reserved1)
		{
			MEM_BufferFree(evt->ErrorList);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Reserved1 = NULL;
	}

	FLib_MemCpy(evt->Reserved1, pPayload + idx, evt->MaxErrInListDiff); idx += evt->MaxErrInListDiff;

	if (evt->MaxCustErrInList > 0)
	{
		evt->ErrorDebugInfo = MEM_BufferAlloc(evt->MaxCustErrInList * sizeof(evt->ErrorDebugInfo[0]));

		if (!evt->ErrorDebugInfo)
		{
			MEM_BufferFree(evt->Reserved1);
			MEM_BufferFree(evt->ErrorList);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->ErrorDebugInfo = NULL;
	}


	for (uint32_t i = 0; i < evt->MaxCustErrInList; i++)
	{
		FLib_MemCpy(&(evt->ErrorDebugInfo[i].PCErr), pPayload + idx, sizeof(evt->ErrorDebugInfo[i].PCErr)); idx += sizeof(evt->ErrorDebugInfo[i].PCErr);
		FLib_MemCpy(&(evt->ErrorDebugInfo[i].LRErr), pPayload + idx, sizeof(evt->ErrorDebugInfo[i].LRErr)); idx += sizeof(evt->ErrorDebugInfo[i].LRErr);
		FLib_MemCpy(&(evt->ErrorDebugInfo[i].MoreInfoErr), pPayload + idx, sizeof(evt->ErrorDebugInfo[i].MoreInfoErr)); idx += sizeof(evt->ErrorDebugInfo[i].MoreInfoErr);
	}

	if (evt->MaxCustErrInList > 0)
	{
		evt->WarningList = MEM_BufferAlloc(evt->MaxCustErrInList);

		if (!evt->WarningList)
		{
			MEM_BufferFree(evt->ErrorDebugInfo);
			MEM_BufferFree(evt->Reserved1);
			MEM_BufferFree(evt->ErrorList);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->WarningList = NULL;
	}

	FLib_MemCpy(evt->WarningList, pPayload + idx, evt->MaxCustErrInList); idx += evt->MaxCustErrInList;

	if (evt->MaxErrInListDiff > 0)
	{
		evt->Reserved2 = MEM_BufferAlloc(evt->MaxErrInListDiff);

		if (!evt->Reserved2)
		{
			MEM_BufferFree(evt->WarningList);
			MEM_BufferFree(evt->ErrorDebugInfo);
			MEM_BufferFree(evt->Reserved1);
			MEM_BufferFree(evt->ErrorList);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->Reserved2 = NULL;
	}

	FLib_MemCpy(evt->Reserved2, pPayload + idx, evt->MaxErrInListDiff); idx += evt->MaxErrInListDiff;

	if (evt->MaxCustErrInList > 0)
	{
		evt->WarningDebugInfo = MEM_BufferAlloc(evt->MaxCustErrInList * sizeof(evt->WarningDebugInfo[0]));

		if (!evt->WarningDebugInfo)
		{
			MEM_BufferFree(evt->Reserved2);
			MEM_BufferFree(evt->WarningList);
			MEM_BufferFree(evt->ErrorDebugInfo);
			MEM_BufferFree(evt->Reserved1);
			MEM_BufferFree(evt->ErrorList);
			return kStatus_MemAllocError;
		}

	}
	else
	{
		evt->WarningDebugInfo = NULL;
	}


	for (uint32_t i = 0; i < evt->MaxCustErrInList; i++)
	{
		FLib_MemCpy(&(evt->WarningDebugInfo[i].PCWar), pPayload + idx, sizeof(evt->WarningDebugInfo[i].PCWar)); idx += sizeof(evt->WarningDebugInfo[i].PCWar);
		FLib_MemCpy(&(evt->WarningDebugInfo[i].LRWar), pPayload + idx, sizeof(evt->WarningDebugInfo[i].LRWar)); idx += sizeof(evt->WarningDebugInfo[i].LRWar);
		FLib_MemCpy(&(evt->WarningDebugInfo[i].MoreInfoWar), pPayload + idx, sizeof(evt->WarningDebugInfo[i].MoreInfoWar)); idx += sizeof(evt->WarningDebugInfo[i].MoreInfoWar);
	}
	FLib_MemCpy(&(evt->Reserved3), pPayload + idx, sizeof(evt->Reserved3)); idx += sizeof(evt->Reserved3);

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAP2GenericEventSetExtAdvertisingDecisionDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Gap_SetExtAdvertisingDecisionData Complete Event
***************************************************************************************************/
static mem_status_t Load_GAP2GenericEventSetExtAdvertisingDecisionDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAP2GenericEventSetExtAdvertisingDecisionDataSetupCompleteIndication_t *evt = &(container->Data.GAP2GenericEventSetExtAdvertisingDecisionDataSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAP2GenericEventSetExtAdvertisingDecisionDataSetupCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAP2GenericEventSetExtAdvertisingDecisionDataSetupCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAP2GenericEventSetDecisionInstructionsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Gap_SetDecisionInstructions Complete Event
***************************************************************************************************/
static mem_status_t Load_GAP2GenericEventSetDecisionInstructionsSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAP2GenericEventSetDecisionInstructionsSetupCompleteIndication_t *evt = &(container->Data.GAP2GenericEventSetDecisionInstructionsSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAP2GenericEventSetDecisionInstructionsSetupCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAP2GenericEventSetDecisionInstructionsSetupCompleteIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAP2CtrlDebugInfo2Indication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Generic Event - Secondary Debug Information from the LL
***************************************************************************************************/
static mem_status_t Load_GAP2CtrlDebugInfo2Indication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAP2CtrlDebugInfo2Indication_t *evt = &(container->Data.GAP2CtrlDebugInfo2Indication);

	/* Store (OG, OC) in ID */
	container->id = GAP2CtrlDebugInfo2Indication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAP2CtrlDebugInfo2Indication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAP2EcdhP256ComputeA2BKeyIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A2B key data
***************************************************************************************************/
static mem_status_t Load_GAP2EcdhP256ComputeA2BKeyIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAP2EcdhP256ComputeA2BKeyIndication_t *evt = &(container->Data.GAP2EcdhP256ComputeA2BKeyIndication);

	/* Store (OG, OC) in ID */
	container->id = GAP2EcdhP256ComputeA2BKeyIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAP2EcdhP256ComputeA2BKeyIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAP2ExportA2BBlobIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Obtain A2B encrypted key data
***************************************************************************************************/
static mem_status_t Load_GAP2ExportA2BBlobIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAP2ExportA2BBlobIndication_t *evt = &(container->Data.GAP2ExportA2BBlobIndication);

	/* Store (OG, OC) in ID */
	container->id = GAP2ExportA2BBlobIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAP2ExportA2BBlobIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAP2ImportA2BBlobIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Obtain A2B encrypted key data
***************************************************************************************************/
static mem_status_t Load_GAP2ImportA2BBlobIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAP2ImportA2BBlobIndication_t *evt = &(container->Data.GAP2ImportA2BBlobIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = GAP2ImportA2BBlobIndication_FSCI_ID;

	evt->KeyType = (GAP2ImportA2BBlobIndication_KeyType_t)pPayload[idx]; idx++;

	switch (evt->KeyType)
	{
		case GAP2ImportA2BBlobIndication_KeyType_gSecPlainText_c:
			FLib_MemCpy(evt->Key.gSecPlainText_c, pPayload + idx, 16); idx += 16;
			break;

		case GAP2ImportA2BBlobIndication_KeyType_gSecElkeBlob_c:
			FLib_MemCpy(evt->Key.gSecElkeBlob_c, pPayload + idx, 40); idx += 40;
			break;

		case GAP2ImportA2BBlobIndication_KeyType_gSecLtkElkeBlob_c:
			FLib_MemCpy(evt->Key.gSecLtkElkeBlob_c, pPayload + idx, 40); idx += 40;
			break;
	}

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAP2EcdhP256GenerateKeysIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Obtain A2B encrypted key data
***************************************************************************************************/
static mem_status_t Load_GAP2EcdhP256GenerateKeysIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAP2EcdhP256GenerateKeysIndication_t *evt = &(container->Data.GAP2EcdhP256GenerateKeysIndication);

	/* Store (OG, OC) in ID */
	container->id = GAP2EcdhP256GenerateKeysIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAP2EcdhP256GenerateKeysIndication_t));

	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		static mem_status_t Load_GAP2ChannelOverrideCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Gap_LeChannelOverride Complete Event
***************************************************************************************************/
static mem_status_t Load_GAP2ChannelOverrideCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAP2ChannelOverrideCompleteIndication_t *evt = &(container->Data.GAP2ChannelOverrideCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = GAP2ChannelOverrideCompleteIndication_FSCI_ID;

	FLib_MemCpy(evt, pPayload, sizeof(GAP2ChannelOverrideCompleteIndication_t));

	return kStatus_MemSuccess;
}

#endif  /* GAP2_ENABLE */


void KHC_BLE_RX_MsgHandler(void *pData, void *param, uint8_t fsciInterface)
{
	if (!pData || !param)
	{
		return;
	}

	clientPacket_t *frame = (clientPacket_t *)pData;
	bleEvtContainer_t *container = (bleEvtContainer_t *)param;
	uint8_t og = frame->structured.header.opGroup;
	uint8_t oc = frame->structured.header.opCode;
	uint8_t *pPayload = frame->structured.payload;
	uint16_t id = (og << 8) + oc, i;

	for (i = 0; i < sizeof(evtHandlerTbl) / sizeof(evtHandlerTbl[0]); i++)
	{
		if (evtHandlerTbl[i].id == id)
		{
			evtHandlerTbl[i].handlerFunc(container, pPayload);
			break;
		}
	}
}
