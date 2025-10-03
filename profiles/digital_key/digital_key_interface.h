/*! *********************************************************************************
 * \addtogroup Digital Key Profile
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* \file digital_key_interface.h
*
* Copyright 2020-2022, 2024-2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef DIGITAL_KEY_INTERFACE_H
#define DIGITAL_KEY_INTERFACE_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
#define gDKProtocolVersion_d      0x0100

#define gUWBConfigId_d            0x0001

/* DK message field sizes */
#define gMessageHeaderSize_c      (1U)
#define gPayloadHeaderSize_c      (1U)
#define gLengthFieldSize_c        (2U)

/* Payload lengths */
#define gCommandCompleteSubEventPayloadLength_c (2U)
#define gTimeSyncPayloadLength_c                (23U)
#define gFirstApproachReqRspPayloadLength       (38U)
/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum dkMessageType_tag {
    gDKMessageTypeFrameworkMessage_c               = 0x00,
    gDKMessageTypeSEMessage_c                      = 0x01,
    gDKMessageTypeUWBRangingServiceMessage_c       = 0x02,
    gDKMessageTypeDKEventNotification_c            = 0x03,
    gDKMessageTypeVehicleOEMAppMessage_c           = 0x04,
    gDKMessageTypeSupplementaryServiceMessage_c    = 0x05,
    gDKMessageTypeHeadUnitPairingMessage_c         = 0x06,
    gDKMessageTypeBTCSRangingServiceMessage_c      = 0x07
} dkMessageType_t;

/* Framework Message Payload Types */
typedef enum frameworkMsgId_tag {
    gDkApduRQ_c                      = 0x0B,
    gDkApduRS_c                      = 0x0C,
} frameworkMsgId_tag;

/* UWB Ranging Service Payload Types */
typedef enum rangingMsgId_tag {
    gRangingCapabilityRQ_c           = 0x01,
    gRangingCapabilityRS_c           = 0x02,
    gRangingSessionRQ_c              = 0x03,
    gRangingSessionRS_c              = 0x04,
    gRangingSessionSetup_RQ_c        = 0x05,
    gRangingSessionSetup_RS_c        = 0x06,
    gRangingSuspendRQ_c              = 0x07,
    gRangingSuspendRS_c              = 0x08,
    gRangingRecoveryRQ_c             = 0x09,
    gRangingRecoveryRS_c             = 0x0A,
    gConfigurableRangingRecoveryRQ_c = 0x0B,
    gConfigurableRangingRecoveryRS_c = 0x0C,
} rangingMsgId_t;

/* DK Event Notification Payload Types */
typedef enum dkEvtNotifMsgId_tag {
    gDkEventNotification_c           = 0x01,
} dkEvtNotifMsgId_t;

/* Vehicle OEM App Payload Types */
typedef enum vehicleOEMAppMsgId_tag {
    gPassthrough_c                   = 0x01,
} vehicleOEMAppMsgId_t;

/* Supplementary Service Payload Types */
typedef enum suplServiceMsgId_tag {
    gTimeSync_c                      = 0x01,
    gFirstApproachRQ_c               = 0x02,
    gFirstApproachRS_c               = 0x03,
    gRKEAuthRQ_c                     = 0x04,
    gRKEAuthRS_c                     = 0x05,
    gUWBFinalData_c                  = 0x06,
} suplServiceMsgId_t;

/* Head Unit Pairing Payload Types */
typedef enum headUnitPairingMsgId_tag {
    gHeadUnitPairingPrep_c           = 0x01,
    gHeadUnitPairingRQ_c             = 0x02,
    gHeadUnitPairingRS_c             = 0x03
} headUnitPairingMsgId_t;

typedef struct rangingMsg_tag {
    dkMessageType_t   messageHeader;
    rangingMsgId_t    payloadHeader;
    uint16_t          payloadLength;
    uint8_t*          payloadData;
} rangingMsg_t;

typedef enum dkSubEventCategory_tag {
    gCommandComplete_c             = 0x01,
    gRangingSessionStatusChanged_c = 0x02,
    gDeviceRangingIntent_c         = 0x03,
    gVehicleStatusChange_c         = 0x04,
    gRKERequest_c                  = 0x05,
    gHeadUnitPairing_c             = 0x06
} dkSubEventCategory_t;

typedef enum dkSubEventCommandCompleteType_tag {
    gDeselectSE_c                 = 0x00,
    gBlePairingReady_c            = 0x01,
    gRequireCapabilityExchange_c  = 0x02,
    gRequestStandardTransaction_c = 0x03,
    gRequestOwnerPairing_c        = 0x04,
    gGeneralError_c               = 0x80,
    gDeviceSEBusy_c               = 0x81,
    gDeviceSETransactionLost_c    = 0x82,
    gDeviceBusy_c                 = 0x83,
    gCommandTemporarilyBlocked_c  = 0x84,
    gUnsupportedChannelBitmask_c  = 0x85,
    gOPDeviceNotInsideVehicle_c   = 0x86,
    gOOBMismatch_c                = 0xFC,
    gBLEPairingFailed_c           = 0xFD,
    gFACryptoOperationFailed_c    = 0xFE,
    gWrongParameters_c            = 0xFF
} dkSubEventCommandCompleteType_t;

typedef enum dkSubEventRangingSessionStatusChangedType_tag {
    gRangingSessionUsrkRefresh_c          = 0x00,
    gRangingSessionUsrkNotFound_c         = 0x01,
    gRangingSessionSrFailed_c             = 0x03,
    gRangingSessionTerminated_c           = 0x04,
    gRangingSessionRecoveryFailed_c       = 0x06,
} dkSubEventRangingSessionStatusChangedType_t;

typedef enum dkSubEventDeviceRangingIntentType_tag
{
    gLowApproachConfidence_c    = 0x01,
    gMediumApproachConfidence_c = 0x02,
    gHighApproachConfidence_c   = 0x03
} dkSubEventDeviceRangingIntentType_t;

typedef enum dkSubEventHeadUnitType_tag {
    gHeadUnitPairingSuccess_c   = 0x00,
    gHeadUnitPairingFail_c      = 0x01
} dkSubEventHeadUnitType_t;

#if (defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)) || \
    (defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U))
/* BTCS Ranging Service Payload Types */
typedef enum btcsMsgId_tag {
    gRangingProcResStart_c          = 0x01,
    gRangingProcResCont_c           = 0x02,
    gRangingProcResCfg_c            = 0x03,
} btcsMsgId_t;

/* CSProcedureData Message Fragment Definitions */
typedef enum btcsProcDataMsgFragm_tag {
    gCsProcHeader_c          = 0x01,
    gCsProcContHeader_c      = 0x02,
    gCsSubEvtHeader_c        = 0x03,
    gCsSubEvtContHeader_c    = 0x04,
    gCsSubEvtData_c          = 0x05
} btcsProcDataMsgFragm_t;

/* CSProcHeader Message Fragment */
typedef PACKED_STRUCT gCsProcHeaderData_tag {
    uint8_t    seqNo;
    uint8_t    configId;
    uint8_t    moduleId;
} gCsProcHeaderData_t;

/* CSProcContinueHeader Message Fragment */
typedef PACKED_STRUCT gCsProcContHeaderData_tag {
    uint8_t    seqNo;
} gCsProcContHeaderData_t;

/* CSSubEventHeader Message Fragment - Initiator Data */
typedef PACKED_STRUCT gCsSubEvtHeaderInitData_tag {
    uint16_t    startACLConnEvt;
    uint8_t     procEvtDoneStatus;
    uint8_t     abortReason;
    uint16_t    freqCompensation;
    uint8_t     PBRFormat;
    int8_t      referencePowerLevel;
    uint8_t     totalSubEvtSteps;
    uint8_t     numStepsReported;
} gCsSubEvtHeaderInitData_t;

/* CSSubEventHeader Message Fragment - Reflector Data */
typedef PACKED_STRUCT gCsSubEvtHeaderReflData_tag {
    uint16_t    startACLConnEvt;
    uint8_t     procEvtDoneStatus;
    uint8_t     abortReason;
    uint8_t     PBRFormat;
    int8_t      referencePowerLevel;
    uint8_t     totalSubEvtSteps;
    uint8_t     numStepsReported;
} gCsSubEvtHeaderReflData_t;

/* CSSubEventContinueHeader Message Fragment */
typedef PACKED_STRUCT gCsSubEvtContHeaderData_tag {
    uint16_t    startACLConnEvt;
    uint8_t     numStepsReported;
} gCsSubEvtContHeaderData_t;
#endif /* gAppBtcsClient_d || gAppBtcsServer_d */

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

/*!**********************************************************************************
* \brief        Sends a Digital Key Service message on L2CAP channel.
*
* \param[in]    deviceId          Peer device ID.
* \param[in]    channelId         L2CAP channel ID.
* \param[in]    messageType       Message type.
* \param[in]    msgId             Message ID (payload type).
* \param[in]    length            Payload length.
* \param[in]    pData             Pointer to buffer containing payload.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t DK_SendMessage(deviceId_t deviceId, uint16_t channelId, dkMessageType_t messageType,
                           uint8_t msgId, uint16_t length, uint8_t *pData);
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* DIGITAL_KEY_INTERFACE_H */

/*! **********************************************************************************
 * @}
 ************************************************************************************/
