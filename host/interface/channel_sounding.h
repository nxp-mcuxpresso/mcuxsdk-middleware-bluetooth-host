/*! *********************************************************************************
 * \defgroup CHANNEL_SOUNDING Channel Sounding
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022 - 2025 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef CHANNEL_SOUNDING_H
#define CHANNEL_SOUNDING_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ble_general.h"
#include "ble_utils.h"
#include "hci_types.h"
#include "hci_interface.h"

#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d==TRUE)

/************************************************************************************
*************************************************************************************
 * Public macros
*************************************************************************************
************************************************************************************/
/* RTT Capability bitmask */
#define gCsCoarseBit_c      BIT0
#define gCsSoundingBit_c    BIT1
#define gCsRandomBit_c      BIT2

/* Testmode override config bitmask */
#define gCSTmOvrdConfigChannelsMask          BIT0
#define gCSTmOvrdConfigSubmodesMask          BIT2
#define gCSTmOvrdConfigTPmExtMask            BIT3
#define gCSTmOvrdConfigAntpermMask           BIT4
#define gCSTmOvrdConfigCsSyncAaMask          BIT5
#define gCSTmOvrdConfigSsMarkPosMask         BIT6
#define gCSTmOvrdConfigSsMarkValMask         BIT7
#define gCSTmOvrdConfigRandPayloadMask       BIT8
#define gCSTmOvrdConfigPhaseTestMask         BIT10

/* LE CS NXP Config vendor command parameters presence bitmask */
#define gCSParamReserved_c                              BIT0
#define gCSParamAntennaConfigPresent_c                  BIT1
#define gCSParamDistanceCalibrationModePresent_c        BIT2
#define gCSParam0DistanceCompensationDataPresent_c      BIT3
#define gCSParamInlinePhaseReturnPresent_c              BIT4
#define gCSParamRttBiasPresent_c                        BIT5
#define gCSParamPctPhaseRotationPresent_c               BIT6
#define gCSParamPaRampingControlPresent_c               BIT7

/* The maximum number of antenna pairs used. */
#define gCsMaxNumberOfAntennas_c             (4U)

/* Accepted values for SNR control field */
#define gSNRControlAdjustment18dB_c          (0U)
#define gSNRControlAdjustment21dB_c          (1U)
#define gSNRControlAdjustment24dB_c          (2U)
#define gSNRControlAdjustment27dB_c          (3U)
#define gSNRControlAdjustment30dB_c          (4U)
#define gSNRControlNotApplied_c              (0xFFU)

/* Role values for the Create Config and Test commands */
#define gCsRoleInitiator_c                   0U
#define gCsRoleReflector_c                   1U

/* CS HCI data logging support */
#ifndef gAppHciDataLogExport_d
#define gAppHciDataLogExport_d               0U
#endif

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
#define gCsSubevtResHdrLength_c              16U
#define gCsSubevtContResHdrLength_c          9U
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
/*
* CS Event Parameters
*/
typedef enum {
    csReadRemoteSupportedCapabilitiesComplete_c,
    csReadRemoteFAETableComplete_c,
    csSecurityEnableComplete_c,
    csConfigComplete_c,
    csSetProcedureParametersComplete_c,
    csProcedureEnableCompleteEvent_c,
    csSubeventResultEvent_c,
    csSubeventResultContinueEvent_c,
    csTestEnd_c,
    csEventResultDebug_c,
    csError_c,
#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
    csHciDataLog_c,
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */
} csMetaEventType_t;

typedef struct csReadRemoteSupportedCapabilitiesCompleteEvent_tag
{
    deviceId_t      deviceId;
    uint8_t         numConfigSupported;
    uint16_t        maxConsecutiveProceduresSupported;
    uint8_t         numAntennasSupported;
    uint8_t         maxAntennaPathsSupported;
    uint8_t         rolesSupported;
    uint8_t         optionalModesSupported;
    uint8_t         RTTCapability;
    uint8_t         RTTAAOnly;
    uint8_t         RTTSoundingN;
    uint8_t         RTTRandomPayload;
    uint16_t        optionalNADMSoundingCapability;
    uint16_t        optionalNADMRandomCapability;
    uint8_t         optionalSyncPhysSupported;
    uint16_t        optionalSubfeaturesSupported;
    uint16_t        optionalTIP1timesSupported;
    uint16_t        optionalTIP2timesSupported;
    uint16_t        optionalTFCStimesSupported;
    uint16_t        optionalTPMtimesSupported;
    uint8_t         TSWtimeSupported;
    uint8_t         optionalTxSNRCapability;
} csReadRemoteSupportedCapabilitiesCompleteEvent_t;

typedef struct csReadRemoteFAETableCompleteEvent_tag
{
    deviceId_t      deviceId;
    uint8_t         aFAETable[gCSReflectorTableSize_c];
} csReadRemoteFAETableCompleteEvent_t;

typedef struct csSecurityEnableCompleteEvent_tag
{
    deviceId_t      deviceId;
} csSecurityEnableCompleteEvent_t;

typedef struct csConfigCompleteEvent_tag
{
    uint8_t         status;
    deviceId_t      deviceId;
    uint8_t         configId;
    uint8_t         action;
    uint8_t         mainModeType;
    uint8_t         subModeType;
    uint8_t         mainModeMinSteps;
    uint8_t         mainModeMaxSteps;
    uint8_t         mainModeRepetition;
    uint8_t         mode0Steps;
    uint8_t         role;
    uint8_t         RTTTypes;
    uint8_t         csSyncPhy;
    uint8_t         channelMap[gHCICSChannelMapSize];
    uint8_t         channelMapRepetition;
    uint8_t         channelSelectionType;
    uint8_t         ch3cShape;
    uint8_t         ch3cJump;
    uint8_t         rfu;
    uint8_t         TIP1time;
    uint8_t         TIP2time;
    uint8_t         TFCStime;
    uint8_t         TPMtime;
} csConfigCompleteEvent_t;

typedef struct csProcedureEnableCompleteEvent_tag
{
    uint8_t         status;
    deviceId_t      deviceId;
    uint8_t         configId;
    uint8_t         state;
    uint8_t         toneAntennaConfigSelection;
    uint8_t         selectedTxPower;
    uint8_t         subeventLen[gCSMaxSubeventLen_c];
    uint8_t         subeventsPerInterval;
    uint16_t        subeventInterval;
    uint16_t        eventInterval;
    uint16_t        procedureInterval;
    uint16_t        procedureCount;
} csProcedureEnableCompleteEvent_t;

typedef struct csSubeventResultEvent_tag
{
    deviceId_t      deviceId;
    uint8_t         configId;
    uint16_t        startACLConnEvent;
    uint16_t        procedureCounter;
    uint16_t        frequencyCompensation;
    int8_t          referencePowerLevel;
    uint8_t         procedureDoneStatus;
    uint8_t         subeventDoneStatus;
    uint8_t         abortReason;
    uint8_t         numAntennaPaths;
    uint8_t         numStepsReported;
    uint8_t*        pData;
} csSubeventResultEvent_t;

typedef struct csSubeventResultContinueEvent_tag
{
    deviceId_t      deviceId;
    uint8_t         configId;
    uint8_t         procedureDoneStatus;
    uint8_t         subeventDoneStatus;
    uint8_t         abortReason;
    uint8_t         numAntennaPaths;
    uint8_t         numStepsReported;
    uint8_t*        pData;
} csSubeventResultContinueEvent_t;

typedef struct csTestEndEvent_tag
{
    bleResult_t     status;
} csTestEndEvent_t;

/*! Debug section for HCI_LE_CS_Procedure_Result event */
typedef struct csEventResultDebugEvent_tag
{
    uint16_t     payloadLength;
    uint16_t     bufferLength;
    uint16_t     offset;
    uint8_t*     pData;
} csEventResultDebugEvent_t;

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
typedef PACKED_STRUCT csHciDataLogEvent_tag
{
    uint8_t         opCode;
    uint8_t         packetSize;
    const uint8_t*  pPacket;
} csHciDataLogEvent_t;
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */

typedef enum
{
    readRemoteSupportedCapabilitiesComplete,
    readRemoteFAETableComplete,
    securityEnableComplete,
    configComplete,
    procedureEnableComplete,
    eventResult,
    eventResultContinue,
} csErrorSource_t;

typedef struct csErrorEvent_tag
{
    csErrorSource_t     csErrorSource;
    bleResult_t         status;
    deviceId_t          deviceId;
} csErrorEvent_t;

typedef struct csMetaEvent_tag
{
    csMetaEventType_t eventType;    /*!< Event type */
    union {
        csReadRemoteSupportedCapabilitiesCompleteEvent_t csReadRemoteSupportedCapabilitiesComplete;
        csReadRemoteFAETableCompleteEvent_t              csReadRemoteFAETableComplete;
        csSecurityEnableCompleteEvent_t                  csSecurityEnableComplete;
        csConfigCompleteEvent_t                          csConfigComplete;
        csProcedureEnableCompleteEvent_t                 csProcedureEnableCompleteEvent;
        csSubeventResultEvent_t                          csSubeventResultEvent;
        csSubeventResultContinueEvent_t                  csSubeventResultContinueEvent;
        csErrorEvent_t                                   csError;
        csTestEndEvent_t                                 csTestEndEvent;
        csEventResultDebugEvent_t                        csEventResultDebugEvent;
#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
        csHciDataLogEvent_t                              csHciDataLogEvent;
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */
    }eventData;
} csMetaEvent_t;

/*
* CS Command Complete Event Parameters
*/
typedef enum
{
    readLocalSupportedCapabilities_c,
    setDefaultSettings_c,
    writeRemoteFAETable_c,
    setChannelClassification_c,
    setProcedureParameters_c,
    testCmd_c,
    writeCachedRemoteCapabilities_c,
    commandError_c,
} csCommandCompleteEventType_t;

typedef enum
{
    csReadLocalSupportedCapabilities,
    csSetDefaultSettings,
    csWriteRemoteFAETable,
    csSetChannelClassification,
    csSetProcedureParameters,
    csTestCmd,
    csWriteCachedRemoteCapabilities,
} commandErrorSource_t;

typedef struct csCommandError_tag
{
    commandErrorSource_t errorSource;
    bleResult_t          status;
    deviceId_t           deviceId;
} csCommandError_t;

typedef struct csReadLocalSupportedCapabilities_tag
{
    uint8_t         numConfigSupported;
    uint16_t        maxConsecutiveProceduresSupported;
    uint8_t         numAntennasSupported;
    uint8_t         maxAntennaPathsSupported;
    uint8_t         rolesSupported;
    uint8_t         optionalModesSupported;
    uint8_t         RTTCapability;
    uint8_t         RTTAAOnly;
    uint8_t         RTTSoundingN;
    uint8_t         RTTRandomPayloadN;
    uint16_t        optionalNADMSoundingCapability;
    uint16_t        optionalNADMRandomCapability;
    uint8_t         optionalSyncPhysSupported;
    uint16_t        optionalSubfeaturesSupported;
    uint16_t        optionalTIP1timesSupported;
    uint16_t        optionalTIP2timesSupported;
    uint16_t        optionalTFCStimesSupported;
    uint16_t        optionalTPMtimesSupported;
    uint8_t         TSWtimeSupported;
    uint8_t         optionalTxSNRCapability;
} csReadLocalSupportedCapabilities_t;

typedef struct csCommandCompleteEvent_tag
{
    csCommandCompleteEventType_t eventType;
    deviceId_t                   deviceId;
    union {
        csReadLocalSupportedCapabilities_t  csReadLocalSupportedCapabilities;
        csCommandError_t                    csCommandError;
    }eventData;
} csCommandCompleteEvent_t;

typedef enum
{
    csReadRemoteSupportedCapabilities,
    csSecurityEnable,
    csReadRemoteFAETable,
    csCreateConfig,
    csRemoveConfig,
    csProcedureEnable,
    csTestEnd,
} csCommandStatusType;

typedef struct csCommandStatusEvent_tag
{
    csCommandStatusType     eventType;
    hciErrorCode_t          status;
} csCommandStatusEvent_t;

/*
* CS Event Callbacks
*/
/*! CS Le Meta callback prototype. */
typedef bleResult_t (*csMetaEventCallback_t)
(
    csMetaEvent_t* pPacket             /*!< Pointer to event packet. */
);

/*! CS Command Complete callback prototype. */
typedef bleResult_t (*csEventCmdCompleteCallback_t)
(
    csCommandCompleteEvent_t* pPacket  /*!< Pointer to event packet. */
);

/*! CS Command Status callback prototype. */
typedef bleResult_t (*csCmdStatusEventCallback_t)
(
    csCommandStatusEvent_t* pPacket    /*!< Pointer to event packet. */
);

/*
* CS API data types
*/
typedef enum
{
    gEnableCsInitiator_c  = BIT0,
    gEnableCsReflector_c  = BIT1,
} csRoleType;

typedef enum
{
    gRTTCoarse_c,
    gRTTFractional32bitSoundingSequence_c,
    gRTTFractional96bitSoundingSequence_c,
    gRTTFractional32bitRandomSequence_c,
    gRTTFractional64bitRandomSequence_c,
    gRTTFractional96bitRandomSequence_c,
    gRTTFractional128bitRandomSequence_c,
} rttTypes_t;

typedef enum
{
    gChannelSelectionAlgorithm3_c,
    gUserSelectedAlgorithm_c,
} hoppingAlgorithmTypes_t;

typedef enum
{
    gHatShape_c,
    gInterleavedShape_c,
} userShape_t;

typedef struct gCsCreateConfigCommandParams_tag
{
    uint8_t                     configId;
    uint8_t                     createContext;
    uint8_t                     mainModeType;
    uint8_t                     subModeType;
    uint8_t                     mainModeMinSteps;
    uint8_t                     mainModeMaxSteps;
    uint8_t                     mainModeRepetition;
    uint8_t                     mode0Steps;
    uint8_t                     role;
    rttTypes_t                  RTTTypes;
    uint8_t                     csSyncPhy;
    uint8_t                     channelMap[gHCICSChannelMapSize];
    uint8_t                     channelMapRepetition;
    uint8_t                     channelSelectionType;
    uint8_t                     ch3cShape;
    uint8_t                     ch3cJump;
    uint8_t                     rfu;
} gCsCreateConfigCommandParams_t;

typedef struct gCsSetProcedureParamsCommandParams_tag
{
    uint8_t         configId;
    uint16_t        maxProcedureDuration;
    uint16_t        minProcedureInterval;
    uint16_t        maxProcedureInterval;
    uint16_t        maxProcCount;
    uint32_t        minSubeventLen;
    uint32_t        maxSubeventLen;
    uint8_t         toneAntennaConfigSelection;
    uint8_t         phys;
    int8_t          txPwrDelta;
    uint8_t         preferredPeerAntenna;
    uint8_t         SNRCtrlInitiator;
    uint8_t         SNRCtrlReflector;
} gCsSetProcedureParamsCommandParams_t;

typedef struct gCsTestCommandParams_tag
{
    uint8_t                     mainModeType;
    uint8_t                     subModeType;
    uint8_t                     mainModeRepetition;
    uint8_t                     mode0Steps;
    uint8_t                     role;
    rttTypes_t                  RTTTypes;
    uint8_t                     CSSyncPhy;
    uint8_t                     CSSYNCAntennaSelection;
    uint8_t                     subeventLen[gCSMaxSubeventLen_c];
    uint16_t                    subeventInterval;
    uint8_t                     maxNumSubevents;
    int8_t                      transmitPowerLevel;
    uint8_t                     TIP1time;
    uint8_t                     TIP2time;
    uint8_t                     TFCStime;
    uint8_t                     TPMtime;
    uint8_t                     TSWtime;
    uint8_t                     toneAntennaConfig;
    uint8_t                     rfu;
    uint8_t                     SNRCtrlInitiator;
    uint8_t                     SNRCtrlReflector;
    uint16_t                    DRBG_Nonce;
    uint8_t                     channelMapRepetition;
    uint16_t                    overrideConfig;
    uint8_t                     overrideParametersLength;
    uint8_t                     overrideParametersData[1]; /* Placeholder */
} gCsTestCommandParams_t;

typedef enum {
    csLvl1CsToneOrCsRtt_c,
    csLvl2RttAndCsTone150usAcc_c,
    csLvl3RttAndCsTone10usAcc_c,
    csLvl4RttAndCsTone10usAccEdlc_c,
} csSecLevel_t;

typedef bool_t (*csEventCallback_t) (uint8_t *pMsg);
typedef bleResult_t (*csEventHandlerLeMetaEvent) (hciLeMetaEvent_t* pHciEvent);
typedef bleResult_t (*csEventHandlerCmdCompleteEvent) (hciCommandCompleteEvent_t *pHciCmdCompleteEvent);
typedef bleResult_t (*csEventHandlerCmdStatusEvent) (hciCommandStatusEvent_t* pHciEvent);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
#if defined(gUnitTesting_d)
extern uint8_t gAppHciDataLogEnabled;
#else
extern const uint8_t gAppHciDataLogEnabled;
#endif /* gUnitTesting_d */

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
* \brief  Register the application to CS callback
*
* \remarks This function must be called exclusively by the CS library
*
********************************************************************************** */
void Host_RegisterCsEventCallback(csEventCallback_t eventCallback);

/*! *********************************************************************************
* \brief  Register the HCI to CS HCI LE Meta event callback
*
* \remarks This function must be called exclusively by the CS library
*
********************************************************************************** */
void Host_RegisterLeCsMetaEventCallback(csEventHandlerLeMetaEvent eventCallback);

/*! *********************************************************************************
* \brief  Register the HCI to CS command complete event callback
*
* \remarks This function must be called exclusively by the CS library
*
********************************************************************************** */
void Host_RegisterCmdCompleteEventCallback(csEventHandlerCmdCompleteEvent eventCallback);

/*! *********************************************************************************
* \brief  Register the HCI to CS command status event callback
*
* \remarks This function must be called exclusively by the CS library
*
********************************************************************************** */
void Host_RegisterCmdStatusEventCallback(csEventHandlerCmdStatusEvent eventCallback);

/*! *********************************************************************************
* \brief  Initialize CS
*
* \return  gHciSuccess_c
*
********************************************************************************** */
void CS_Init(void);

/*! *********************************************************************************
* \brief  Register the CS meta event application callback
*
* \param[in]    leMetaCallback Pointer to the Le Meta Event callback
*

* \retval       gBleSuccess_c
* \retval       gCsCallbackAlreadyInstalled_c    Callback was already installed
*
********************************************************************************** */
bleResult_t CS_RegisterCsMetaEventCallback(csMetaEventCallback_t leMetaCallback);

/*! *********************************************************************************
* \brief  Register the application callback for command complete events
*
* \param[in]    csEventCallback Pointer to the command complete callback
*
* \retval       gBleSuccess_c
* \retval       gCsCallbackAlreadyInstalled_c    Callback was already installed
*
********************************************************************************** */
bleResult_t CS_RegisterCsEventCallback(csEventCmdCompleteCallback_t csEventCallback);

/*! *********************************************************************************
* \brief  Register the application callback for command status events
*
* \param[in]    csEventCallback Pointer to the command status callback
*
* \retval       gBleSuccess_c
* \retval       gCsCallbackAlreadyInstalled_c    Callback was already installed
*
********************************************************************************** */
bleResult_t CS_RegisterCmdStatusEventCallback(csCmdStatusEventCallback_t csEventCallback);

/*!*************************************************************************************************
*\fn    static bool_t CSBuildHciCsLeMetaEvent(hciLeMetaEvent_t *pHciLeMetaEvent, const uint8_t *pPayload, hciErrorCode_t hciStatus)
*
*\brief HCI LE meta event callback
*
*\param [in]    pHciLeMetaEvent Pointer to the HCI le meta event
*\param [in]    pPayload        Pointer to the Payload data
*\param [in]    hciStatus       Unused
*
*\retval        bool_t         TRUE if opCode was handled, FALSE otherwise
*
* \remarks      Called from Ble_HciRecvLeMetaEvent
*
***************************************************************************************************/
bool_t CSBuildHciCsLeMetaEvent
(
    hciLeMetaEvent_t    *pHciLeMetaEvent,
    const uint8_t       *pPayload,
    hciErrorCode_t      hciStatus
);

/*
* CS API prototypes
*/
/*! *********************************************************************************
* \brief  LE CS Read Local Supported Capabilities Command
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c    Could not allocate message for Host task.
*
********************************************************************************** */
bleResult_t CS_ReadLocalSupportedCapabilities(void);

/*! *********************************************************************************
* \brief  LE CS Read Remote Supported Capabilities Command
*
* \param[in]    deviceId    Peer identifier
*
*\retval        gBleSuccess_c
*\retval        gBleInvalidParameter_c  Pointer to cached capabilities is invalid
*\retval        gBleOutOfMemory_c       Could not allocate message for Host task.
*
********************************************************************************** */
bleResult_t CS_ReadRemoteSupportedCapabilities
(
    deviceId_t deviceId
);

/*! *********************************************************************************
* \brief  LE CS Write Cached Remote Supported Capabilities command
*
* \param[in]    pParam               Cached copy of the supported capabilities of a
*                                    peer device.
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c    Could not allocate message for Host task.
*
********************************************************************************** */
bleResult_t CS_WriteCachedRemoteSupportedCapabilities
(
    csReadRemoteSupportedCapabilitiesCompleteEvent_t *pParam
);

/*! *********************************************************************************
* \brief  LE CS Security Enable Command
*
* \param[in]    deviceId    Peer identifier
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c    Could not allocate message for Host task.
*
********************************************************************************** */
bleResult_t CS_SecurityEnable
(
    deviceId_t deviceId
);

/*! *********************************************************************************
* \brief  LE CS Set Default Settings command
*
* \param[in]    deviceId               Peer identifier
* \param[in]    role                   Role of the local device
* \param[in]    syncAntennaSelection   Antenna identifier to be used for CS SYNC packets
* \param[in]    maxTxPowerLevel        Maximum Tx power level to be used for all CS transmissions
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c    Could not allocate message for Host task.
*
********************************************************************************** */
bleResult_t CS_SetDefaultSettings
(
    deviceId_t     deviceId,
    csRoleType     role,
    uint8_t        syncAntennaSelection,
    int8_t         maxTxPowerLevel
);

/*! *********************************************************************************
* \brief  LE CS Read Remote FAE Table command
*
* \param[in]    deviceId            Peer identifier
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c    Could not allocate message for Host task.
*
********************************************************************************** */
bleResult_t CS_ReadRemoteFAETable
(
    deviceId_t     deviceId
);

/*! *********************************************************************************
* \brief  LE CS Write Remote FAE Table command
*
* \param[in]    deviceId            Peer identifier
* \param[in]    pReflectorTable     Pointer to the location where the table value to be
*                                   written on the peer is stored.
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c         Could not allocate message for Host task.
*\retval        gBleInvalidParameter_c    An invalid parameter was provided.
*
********************************************************************************** */
bleResult_t CS_WriteRemoteFAETable
(
    deviceId_t     deviceId,
    uint8_t        *pReflectorTable
);

/*! *********************************************************************************
* \brief  LE CS Create Config command
*
* \param[in]    deviceId               Peer identifier
* \param[in]    pCreateConfigParams    Pointer to the LE CS Create Config parameters
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c         Could not allocate message for Host task.
*\retval        gCsSecurityCheckFail_c    Parameters are not in accordance with GAP Mode 4 security
*
********************************************************************************** */
bleResult_t CS_CreateConfig
(
    deviceId_t                         deviceId,
    gCsCreateConfigCommandParams_t*    pCreateConfigParams
);

/*! *********************************************************************************
* \brief  LE CS Remove Config command
*
* \param[in]    deviceId            Peer identifier
* \param[in]    configId            Configuration identifier
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c         Could not allocate message for Host task.
*
********************************************************************************** */
bleResult_t CS_RemoveConfig
(
    deviceId_t      deviceId,
    uint8_t         configId
);

/*! *********************************************************************************
* \brief  LE CS Set Channel Classification command
*
* \param[in]    pChannelClassification      Pointer to the channel classification information
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c         Could not allocate message for Host task.
*\retval        gBleInvalidParameter_c    An invalid parameter was provided.
*
********************************************************************************** */
bleResult_t CS_SetChannelClassification
(
    uint8_t*         pChannelClassification
);

/*! *********************************************************************************
* \brief  LE CS Set Procedure Parameters command
*
* \param[in]    deviceId                 Peer identifier
* \param[in]    pProcedureParams         Pointer to the command parameters
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c         Could not allocate message for Host task.
*
********************************************************************************** */
bleResult_t CS_SetProcedureParameters
(
    deviceId_t                                deviceId,
    gCsSetProcedureParamsCommandParams_t*     pProcedureParams
);

/*! *********************************************************************************
* \brief  LE CS Procedure Enable command
*
* \param[in]    deviceId           Peer identifier
* \param[in]    enable             Enable or disable procedure
* \param[in]    configId           Procedure identifier
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c         Could not allocate message for Host task.
*
********************************************************************************** */
bleResult_t CS_ProcedureEnable
(
    deviceId_t      deviceId,
    uint8_t         configId,
    bool_t          enable
);

/*! *********************************************************************************
* \brief  LE CS Test command
*
* \param[in]    pTestParams    Pointer to the LE CS Test command parameters
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c         Could not allocate message for Host task.
*
********************************************************************************** */
bleResult_t CS_Test
(
    gCsTestCommandParams_t*   pTestParams
);

/*! *********************************************************************************
* \brief  LE CS Test End command
*
*\retval        gBleSuccess_c
*\retval        gBleOutOfMemory_c         Could not allocate message for Host task.
*
********************************************************************************** */
bleResult_t CS_TestEnd(void);

/*!*************************************************************************************************
*\fn    static bleResult_t CS_SetSecLevelForMode4(csSecLevel_t secLevel)
*
*\brief Set GAP security level for security mode 4.
*
*\param [in]    secLevel        Desired security level.
*
*\retval        none
***************************************************************************************************/
void CS_SetSecLevelForMode4
(
    csSecLevel_t secLevel
);

/*!*************************************************************************************************
*\fn    bleResult_t CS_ConfigVendorCommand(uint32_t paramsPresence, uint8_t  paramDataLength,
*                                          uint8_t* pData)
*
*\brief Configure non-standard behavior for Channel Sounding feature.Each command may contain
*       several parameters data, identified by the bits set in Params_Presence
*
*\param [in]    paramsPresence        Parameters data to be set
*\param [in]    paramDataLength       Length in byte of combined Param Data
*\param [in]    pData                 Parameter-specific information reported as Param_specific_info
*
*\retval        gBleSuccess_c or appropriate error
***************************************************************************************************/
bleResult_t CS_ConfigVendorCommand
(
    uint32_t paramsPresence,
    uint8_t  paramDataLength,
    uint8_t* pData
);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* gBLE_ChannelSounding_d */
#endif /* CHANNEL_SOUNDING_H */

/*! *********************************************************************************
* @}
********************************************************************************** */