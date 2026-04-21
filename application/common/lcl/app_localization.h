/*! *********************************************************************************
* Copyright 2022-2026 NXP
*
* NXP Proprietary
*
* This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or
* by downloading, installing, activating and/or otherwise using the software, you are
* agreeing that you have read, and that you agree to comply with and are bound by,
* such license terms. If you do not agree to be bound by the applicable license terms,
* then you may not retain, install, activate or otherwise use the software.
********************************************************************************** */
#ifndef APP_LOCALIZATION_H
#define APP_LOCALIZATION_H
/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#include "ranging_interface.h"
#endif
#include "ble_general.h"
#if (defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0))
#include "fsl_component_serial_manager.h"
#endif /* gAppHciDataLogExport_d */

#include "channel_sounding.h"
#include "app_localization_config.h"

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
/*! Types of embedded algorithms that can be configured */
#define eMciqAlgoEmbedCDE                   BIT0
#define eMciqAlgoEmbedRADE                  BIT1
#define eMciqAlgoEmbedSlope                 BIT2

/* XCVR characteristics */
#define XCVR_F_RANGE                           (84U)
#define CS_EVTIDX_SHIFT                        (2U)

/* Antenna Configuratin Index 2:2 */
#define gAntennaCfgIdx7_c                      (7U)

/* Channel map length */
#define gCsChannelMapLength_c                  (10U)

/* Config Id used by localization app */
#define gAppLocalizationConfigId_c             (0x01U)

/* 0x7F - RSSI is not available */
#define gRssiNotAvailable_c                    (0x7F)

/* Macro to validate an SNR_Control value (CS Procedure Parameter) */
#define isValidSnrControl(x) \
    ((x) == gSNRControlAdjustment18dB_c || (x) == gSNRControlAdjustment21dB_c || \
     (x) == gSNRControlAdjustment24dB_c || (x) == gSNRControlAdjustment27dB_c || \
     (x) == gSNRControlAdjustment30dB_c || (x) == gSNRControlNotApplied_c)

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
#if !defined(gAppRasDataTransfer_d) || (gAppRasDataTransfer_d == 0)
/*! Subevent header information */
typedef PACKED_STRUCT csRasSubeventHeader_tag
{
    uint16_t        startACLConnEvent;
    uint16_t        frequencyCompensation;
    int8_t          referencePowerLevel;
    uint8_t         procedureDoneStatus;
    uint8_t         subeventDoneStatus;
    uint8_t         abortReason;
    uint8_t         numStepsReported;
} csRasSubeventHeader_t;

/*! Subevent indexing for local data */
typedef PACKED_STRUCT measSubEvtData_tag
{
    uint32_t dataIdx;           /* Data index for the subevent. */
    uint16_t dataSize;          /* Data size for the subevent. */
    uint16_t currentDataSize;   /* Data size already sent/received for the current subevent. */
    csRasSubeventHeader_t subevtHeader;
} measSubEvtData_t;

/*! Format of local data */
typedef PACKED_STRUCT rasMeasurementData_tag
{
    deviceId_t          deviceId;
    uint8_t             configId;
    uint16_t            procedureCounter;
    uint8_t             numAntennaPaths;
    uint8_t             totalNumSteps;
    int8_t              selectedTxPower;
    uint32_t            dataIndex;
    uint16_t            totalSentRcvDataIndex;
    uint16_t            dataParsedLen;
    uint8_t             subeventIndex;
    uint8_t             crtNumSteps;
    measSubEvtData_t    aSubEventData[gMaxNumCsSubevents_c];

    uint8_t             crtStep;
    uint8_t             step;
    uint32_t            position;
    uint8_t             *pRemaining;
    uint32_t            remainingLen;

    uint8_t             *pData; /* Must be the last element in this structure */
} rasMeasurementData_t;
#endif

typedef struct localizationAlgoRun_tag
{
    float    distanceInMeters;
    uint8_t  distanceIntegerPart;
    uint16_t distanceDecimalPart;
    uint8_t  leadingZeroesDecimalPart;
    float    dqiPercentage;
    uint16_t dqiIntegerPart;
    uint16_t dqiDecimalPart;
} localizationAlgoRun_t;

/*! Contains measurement derived values (from isp_parameters and others) */
typedef struct tof_result_tag {
    uint8_t dm_sr;  /*!< Succes rate of ToF packet exchanges [%] */
    int32_t dm_ad;  /*!< Estimated average distance [m], fixed-point s15.16 */
} tof_result_t;

#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1)
typedef struct tofRssiInfo_tag {
    uint8_t rssiLocalNo;
    int8_t aRssiLocal[gMaxNumCsSteps_c];
    uint8_t rssiRemoteNo;
    int8_t aRssiRemote[gMaxNumCsSteps_c];
} tofRssiInfo_t;
#endif /* gAppParseRssiInfo_d */

typedef struct localizationAlgoResult_tag
{
    uint8_t algorithm;
    uint8_t radeError;
    localizationAlgoRun_t resultCDE;
    localizationAlgoRun_t aResultSlope[gMaxNumAntennaPaths_c];
    localizationAlgoRun_t resultRADE;
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
    uint64_t csConfigDuration;
    uint64_t csProcedureDuration;
    uint64_t transferDuration;
    uint64_t algoDuration;
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */
    tof_result_t rttResult;
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1)
    tofRssiInfo_t rssiInfo;
#endif /* gAppParseRssiInfo_d */
} localizationAlgoResult_t;

typedef enum
{
    gAppLclStartMeasurementFail_c,              /*!< CS Procedure Enable failed. */
    gAppLclRasTransferFailed_c,                 /*!< Received an error response from RAS server. */
    gAppLclProcStatusFailed_c,                  /*!< Procedure Status failed received. */
    gAppLclProcedureAborted_c,                  /*!< All subsequent CS procedures aborted. */
    gAppLclInvalidSegmentCounter_c,             /*!< Received segment counter doesn't match the local one. . */
    gAppLclInvalidProcCounter_c,                /*!< Procedure counter received doesn's match the local one. */
    gAppLclSubeventAborted_c,                   /*!< Current CS subevent aborted . */
    gAppLclSubeventStatusFailed_c,              /*!< Subevent Status failed received. */
    gAppLclInvalidProcIndex_c,                  /*!< Received a data ready indication for an out of sync procedure. */
    gAppLclAlgoNotRun_c,                        /*!< Algorithm did not run. */
    gAppLclAlgoNotRunNoDataReady_c,             /*!< Algorithm did not run for the previous procedure - no Data Ready from peer. */
    gAppLclAlgoNotRunNoRealTimeData_c,          /*!< Algorithm did not run for the previous procedure - Real Time data missing. */
    gAppLclAlgoNotRunNoRangingData_c,           /*!< Algorithm did not run for the previous procedure - Ranging Data not complete. */
    gAppLclProcEndSubeventStatusFailed_c,       /*!< Procedure done, subevent status failed was received. */
    gAppLclUnexpectedRRSCC_c,                   /*!< Unexpected csReadRemoteSupportedCapabilitiesComplete_c was received. */
    gAppLclUnexpectedCC_c,                      /*!< Unexpected csConfigComplete_c was received. */
    gAppLclUnexpectedPEC_c,                     /*!< Unexpected csProcedureEnableCompleteEvent_c was received. */
    gAppLclUnexpectedSRE_c,                     /*!< Unexpected csSubeventResultEvent_c was received. */
    gAppLclUnexpectedSDS_c,                     /*!< Unexpected setDefaultSettings_c was received. */
    gAppLclUnexpectedSRCE_c,                    /*!< Unexpected csSubeventResultContinueEvent_c was received. */
    gAppLclErrorRRSCCC_c,                       /*!< Error occured! Source: readRemoteSupportedCapabilitiesComplete. */
    gAppLclErrorSEC_c,                          /*!< Error occured! Source: securityEnableComplete. */
    gAppLclErrorRLSC_c,                         /*!< Error occured! Source: csReadLocalSupportedCapabilities. */
    gAppLclErrorRRFAETC_c,                      /*!< Error occured! Source: readRemoteFAETableComplete. */
    gAppLclErrorCC_c,                           /*!< Error occured! Source: configComplete. */
    gAppLclErrorPEC_c,                          /*!< Error occured! Source: procedureEnableComplete. */
    gAppLclErrorERE_c,                          /*!< Error occured! Source: eventResult. */
    gAppLclErrorERCE_c,                         /*!< Error occured! Source: eventResultContinue. */
    gAppLclUnexpectedSPP_c,                     /*!< Unexpected setProcedureParameters_c command complete was received. */
    gAppLclInvalidDeviceId_c,                   /*!< An event with an invalid device id was received. */
    gAppLclSDSConfigError_c,                    /*!< CS_SetDefaultSettings call resulted in an error. */
    gAppLclCCConfigError_c,                     /*!< CS_CreateConfig call resulted in an error. */
    gAppLclRRSCError_c,                         /*!< Error occured! Source: csReadRemoteSupportedCsCapabilities command status. */
    gAppLclSEError_c,                           /*!< Error occured! Source: csSecurityEnable command status. */
    gAppLclCCError_c,                           /*!< Error occured! Source: csCreateConfig command status. */
    gAppLclRasSendIndicationFailed_c,           /*!< Ras_SendDataReady call failed. */
    gAppLclRasRangingBodyBuildFailed_c,         /*!< Failed to build the ranging data body for On-Demand Data. */
    gAppLclNoSubeventMemoryAvailable_c,         /*!< No more memory available for a local subevent. */
    gAppLclErrorProcessingSubevent_c,           /*!< An error occured in the processing of subevent data. */
    gAppLclUnexpectedWCCC_c,                    /*!< Unexpected writeCachedRemoteCapabilities_c was received. */
    gAppLclOutOfMemory_c,                       /*!< Not enough memory available for the temporary notification buffer */
    gAppLclMaxProceduresReached_c,              /*!< Error occured! Maximum concurrent CS procedures reached */
} appLocalizationError_t;

typedef enum
{
    gAppLclLocalHost_c = 0x1U,                  /*!< Procedure aborted error: Reason - local Host or remote request. */
    gAppLclRequiredChannelNumber_c = 0x2U,      /*!< Procedure aborted error: Reason - filtered channel map has less than 15 channels. */
    gAppLclChannelMapInstant_c = 0x4U,          /*!< Procedure aborted error: Reason - channel map update instant has passed. */
    gAppLclNoCsSync_c = 0x20U,                  /*!< Subevent aborted error: No CS_SYNC (mode0) received. */
    gAppLclScheduleConflict_c = 0x30U,          /*!< Subevent aborted error: Scheduling conflicts or limited resources. */
    gAppLclTimePassed_c = 0xC0U,                /*!< Subevent aborted error: Time passed. */
    gAppLclInvalidArguments_c = 0xD0U,          /*!< Subevent aborted error: Invalid arguments. */
    gAppLclAborted_c = 0xE0U,                   /*!< Subevent aborted error: Aborted. */
    gAppLclUnspecifiedReasons_c = 0x0FU,        /*!< Unspecified reasons for subevent aborted error and procedure aborted error. */
} appLocalizationAbort_t;

/*! Structure holding configuration of a ranging measurement. */
typedef struct appLocalization_rangeCfgGlobal_tag
{
    uint8_t role;                               /*!< CS role */
    uint8_t ant_type;                           /*!< Antenna diversity board type */
    uint8_t t_sw_local;                         /*!< CS T_SW local */
} appLocalization_rangeCfgGlobal_t;

typedef struct appLocalization_rangeCfg_tag
{
    uint8_t configId;                           /*!< Configuration Id */
    uint8_t main_mode_type;                     /*!< Main mode of CS steps */
    uint8_t sub_mode_type;                      /*!< Sub mode of CS steps */
    uint8_t main_mode_min;                      /*!< Minimum number of Main mode steps */
    uint8_t main_mode_max;                      /*!< Maximum number of Main mode steps */
    uint8_t main_mode_repeat;                   /*!< number of Main mode steps to be inserted after a mode0 */
    uint8_t mode0_nb;                           /*!< Number of mode0 steps to be inserted at each event */
    uint8_t rtt_type;                           /*!< RTT type */
    uint8_t phy;                                /*!< PHY to be used by the CS procedure */
    uint8_t cs_sync_phy;                        /*!< PHY to be used for CS_SYNC exchanges */
    uint8_t ant_cfg_index;                      /*!< Antenna configuration index 0-7 */
    uint8_t ch_map[gCsChannelMapLength_c];      /*!< Bitmask for channels 0-78 */
    uint8_t ch_map_repeat;                      /*!< channel map repetition */
    uint8_t channelSelectionType;               /*!< Channel selection type */
    uint16_t maxProcedureDuration;              /*!< Maximum duration for each CS procedure */
    uint16_t minPeriodBetweenProcedures;        /*!< Minimum number of connection events between consecutive CS procedures */
    uint16_t maxPeriodBetweenProcedures;        /*!< Maximum number of connection events between consecutive CS procedures */
    uint16_t maxNumProcedures;                  /*!< Maximum number of CS procedures to be scheduled */
    uint32_t minSubeventLen;                    /*!< Minimum suggested duration for each CS subevent in microseconds */
    uint32_t maxSubeventLen;                    /*!< Maximum suggested duration for each CS subevent in microseconds */
    int8_t txPwrDelta;                          /*!< Transmit power delta, in signed dB */
    uint8_t t_fcs;                              /*!< CS T_FCS - not configurable by the application */
    uint8_t t_ip1;                              /*!< CS T_IP1 - not configurable by the application */
    uint8_t t_ip2;                              /*!< CS T_IP2 - not configurable by the application */
    uint8_t t_pm;                               /*!< CS T_PM  - not configurable by the application */
    uint8_t t_sw_remote;                        /*!< CS T_SW remote */
    uint8_t snr_control_init;                   /*!< CS SNR_Control_Initiator */
    uint8_t snr_control_refl;                   /*!< CS SNR_Control_Reflector */
    uint16_t subfeaturesSupported;              /*!< CS Subfeatures Supported as reported by Read Remote Capabilities */
    bool_t inlinePctEnabled;                    /*!< TRUE if IPT enabled in current CS config */
    /* Used by algorithm */
    uint16_t connInterval;                      /*!< Connection interval of the Bluetooth LE link (units of 1.25ms) - required by algorithm */
    void *csAlgoBuf;                            /*!< Buffer used by RADE */
} appLocalization_rangeCfg_t;

typedef enum
{
    gCsMetaEvent_c,                             /* CS Meta event */
    gCsCcEvent_c,                               /* CS Command Complete event */
    gCsStatusEvent_c,                           /* CS Command Status event */
    gCsSecurityEnabled_c,                       /* CS Security Enabled event */
    gConfigComplete_c,                          /* Localization Config complete */
    gSetProcParamsComplete_c,                   /* Localization Set Procedure Parameters complete */
    gDistanceMeastStarted_c,                    /* Distance measurement started on Reflector. */
    gLocalMeasurementComplete_c,                /* Local distance measurement complete. */
    gDataOverwritten_c,                         /* Received Data Overwritten indication */
    gLocalConfigWritten_c,                      /* Config Complete - configuration written to local controller only */
    gErrorEvent_c,                              /* Error Event */
    gErrorSubeventAborted_c,                    /* Current CS subevent aborted - includes Abort Reason */
    gErrorProcedureAborted_c,                   /* All subsequent CS procedures aborted - includes Abort Reason */
    gErrRasOpCodeNotSupported_c,                /* OpCode that was written to the RAS-CP is not supported by the RRSP */
    gErrRasInvalidParameter_c,                  /* Parameter that was written to the RAS-CP is invalid */
    gErrRasParameterNotSupported_c,             /* Parameter that was written to the RAS-CP is not supported by the RRSP */
    gErrRasAbortUnsuccessful_c,                 /* Unable to process the Abort Operation procedure */
    gErrRasProcedureNotCompleted_c,             /* Server needed to interrupt its data transfer  */
    gErrRasServerBusy_c,                        /* Server busy - cannot process request */
    gErrRasNoRecordsFound_c,                    /* Requested ranging counter was not found*/
#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
    gCsHciDataLogEvent_c,                       /* CS HCI data logging event */
    gCsRemoteDataLogEvent_c,                    /* CS remote RAS data logging event */
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */
} appCsEventType_t;

typedef enum
{
    gCsCompleteResults_c = 0x00U,               /* Subevent done status: All results complete for the CS procedure. */
    gCsPartialResults_c = 0x01U,                /* Subevent done status: Partial results with more to follow for the CS procedure. */
    gCsNoResultsProcAborted_c = 0x0FU,          /* Subevent done status: All subsequent CS procedures aborted. */
} appCsResult_t;

/* APP -  pointer to function for CS events */
typedef void (*pfAppCsCallback_t)(deviceId_t deviceId, void* pData, appCsEventType_t eventType);

/* APP - pointer to function to display measurement results */
typedef void (*pfAppDisplayResult_t)(deviceId_t deviceId, localizationAlgoResult_t *pResult);

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
typedef struct appLocalization_TimeInfo_tag
{
    uint64_t        csConfigStartTs;
    uint64_t        csConfigEndTs;
    uint64_t        csDistMeasStart;
    uint64_t        csDistMeasDuration;
    uint64_t        transferStart;
    uint64_t        transferEnd;
    uint64_t        algoStart;
    uint64_t        algoEnd;
    uint32_t        subeventLen;
    uint16_t        subeventInterval;
    uint16_t        lastAclConnEvtCnt;
    uint16_t        connInterval;
    uint8_t         noOfRcvSubEvsInConnInt;
} appLocalization_TimeInfo_t;
#endif

typedef enum
{
    gAppLclIdle_c,
    gAppLclWaitingForRRSC_c,                    /* Wait for Read Remote Supported Capabilities Command Status */
    gAppLclWaitingForRRSCC_c,                   /* Wait for Read Remote Supported Capabilities Meta Event */
    gAppLclWaitingForSDSCC_c,                   /* Wait for Set Default Settings Command Complete */
    gAppLclWaitingForSDSCCWC_c,                 /* Wait for Set Default Settings Command Complete after writing cached capabilities */
    gAppLclWaitingForSECS_c,                    /* Wait for Security Enable Command Status */
    gAppLclWaitingForSEC_c,                     /* Wait for Security Enable Complete Meta Event */
    gAppLclWaitingForCCCS_c,                    /* Wait for Create Config Command Status */
    gAppLclWaitingForCC_c,                      /* Wait for Create Config Complete Meta Event */
    gAppLclWaitingForSPPCC_c,                   /* Wait for Set Procedure Parameters Command Complete */
    gAppLclWaitingForPECS_c,                    /* Wait for Procedure Enable Command Status */
    gAppLclWaitingForPEC_c,                     /* Wait for Procedure Enable Complete Meta Event */
    gAppLclWaitingForMeasData_c,                /* Wait for the first Subevent Result Event */
    gAppLclReceivingMeasDataDropLeftovers_c,    /* Wait for the end of the current procedure - drop leftover RAS ranging data from previous procedure */
    gAppLclReceivingMeasData_c,                 /* Wait for the end of the current procedure */
    gAppLclWaitingForWCCC_c,                    /* Wait for Write Cached Remote Capabilities command complete */
    gAppLclWaitingForLocCfg_c,                  /* Wait for Create Config Complete Meta Event after writing the local config */
    gAppRasTransfInProgress_c,                  /* Wait for RAS transfer to be over */
} appLocalization_State_t;

typedef enum csAppMetaEventType_tag {
  gCsMetaEvtReadRemoteSupportedCapabilities_c,
  gCsMetaEvtSecurityEnableComplete_c,
  gCsMetaEvtConfigComplete_c,
  gCsMetaEvtProcedureEnableComplete_c,
  gCsMetaEvtSubeventResult_c,
  gCsMetaEvtSubeventResultContinue_c,
  gCsMetaEvtError_c,
#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
  gCsMetaEvtHciDataLog_c,
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */
} csAppMetaEventType_t;

typedef struct csMetaEventData_tag
{
    csAppMetaEventType_t   eventType;           /*!< Event type. */
    void*                  pEventData;          /*!< Event data, allocated according to event type. */
} csMetaEventData_t;

typedef enum
{
    gCsStepMode0_c      = 0x00U,    /* Step mode 0 */
    gCsStepMode1_c      = 0x01U,    /* Step mode 1 */
    gCsStepMode2_c      = 0x02U,    /* Step mode 2 */
    gCsStepMode3_c      = 0x03U,    /* Step mode 3 */
} csStepMode_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
extern appLocalization_rangeCfgGlobal_t mGlobalRangeSettings;
extern appLocalization_rangeCfg_t mRangeSettings[gAppMaxConnections_c];
extern appLocalization_rangeCfg_t mDefaultRangeSettings;

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
extern appLocalization_TimeInfo_t gCsTimeInfo;
#endif

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRRSP_d) && (gRasRRSP_d == 1U)
extern rasStaticConfig_t mRasServiceConfig;
#endif /* gRasRRSP_d */
#endif /* gAppRasDataTransfer_d */

/************************************************************************************
 *************************************************************************************
 *  Public prototypes
 *************************************************************************************
 ************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\brief         Get pointer to local measurement data for the specified peer.
*
*\param[in]     deviceId      Peer identifier
*
*\retval        Pointer to local measurement data
********************************************************************************** */
rasMeasurementData_t* AppLocalization_GetLocalData
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn           bleResult_t AppLocalization_Init(uint8_t role,
*                                               pfAppCsCallback_t pfAppCallback,
*                                               pfAppDisplayResult_t pfAppDisplayCallback)
*
*\brief        This is the application localization module initialization function.
*
*\param  [in]  role                    Channel Sounding role.
*\param  [in]  pfAppCallback           Application callback for localization events.
*\param  [in]  pfAppDisplayCallback    Application callback for displaying results.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_Init
(
    uint8_t role,
    pfAppCsCallback_t pfAppCallback,
    pfAppDisplayResult_t pfAppDisplayCallback
);

/*! *********************************************************************************
*\fn           bleResult_t AppLocalization_HostInitHandler(void)
*
*\brief        This function continues the initialization of the Application
*              Localization.common module and should be called after the Host has
*              been initialized.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_HostInitHandler(void);

/*! *********************************************************************************
*\fn           bleResult_t AppLocalization_Config(deviceId_t deviceId)
*
*\brief        Trigger localization configuration phase.
*
*\param  [in]  deviceId     Peer device id.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_Config
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn           bleResult_t AppLocalization_StartMeasurement(deviceId_t deviceId)
*
*\brief        Start distance measurement for the given peer device id.
*
*\param  [in]  deviceId     Peer device id.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_StartMeasurement
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn           bleResult_t AppLocalization_SecurityEnable(deviceId_t deviceId)
*
*\brief        Enables Channel Sounding security for the given peer device id.
*              This is only called by the central device in the connection.
*
*\param  [in]  deviceId     Peer device id.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_SecurityEnable
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn           bleResult_t AppLocalization_SetProcedureParameters(deviceId_t deviceId)
*
*\brief        Set procedure parameters for the given peer device id. Config must exist.
*
*\param  [in]  deviceId     Peer device id.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_SetProcedureParameters
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            void AppLocalization_SetNumberOfProcedures(deviceId_t deviceId, uint16_t value)
*
*\brief         Set the number of CS procedures to be executed during procedure repeat
*
*\param  [in]   deviceId        Peer device identifier.
*\param  [in]   value           Number of procedure repeats.
*
*\return        None
********************************************************************************** */
void AppLocalization_SetNumberOfProcedures
(
    deviceId_t deviceId,
    uint16_t value
);

/*! *********************************************************************************
*\fn            void AppLocalization_SetConnectionInterval(deviceId_t deviceId, uint16_t connInterval)
*
*\brief         Set the connection interval of the Bluetooth LE link. Needed by algorithm.
*
*\param  [in]   deviceId        Peer device identifier.
*\param  [in]   connInterval    Connection interval (in units of 1.25ms).
*
*\return        None
********************************************************************************** */
void AppLocalization_SetConnectionInterval
(
    deviceId_t deviceId,
    uint16_t connInterval
);

/*! *********************************************************************************
*\fn           bleResult_t AppLocalization_StopMeasurement(deviceId_t deviceId)
*
*\brief        Stop distance measurement for the given peer device id.
*
*\param  [in]  deviceId     Peer device id.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_StopMeasurement
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            bleResult_t AppLocalization_ReadConfig(deviceId_t deviceId,
*               appLocalization_rangeCfg_t pConfigOut)
*
*\brief         Read current localization configuration.
*
*\param  [in]   deviceId        Peer device identifier.
*\param  [out]  pConfigOut      Pointer to where the localization configuration information
*                               is to be copied.
*
*\retval        bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_ReadConfig
(
    deviceId_t deviceId,
    appLocalization_rangeCfg_t *pConfigOut
);

/*! *********************************************************************************
*\fn            bleResult_t AppLocalization_WriteConfig(deviceId_t deviceId,
*               appLocalization_rangeCfg_t pConfig)
*
*\brief         Write current localization configuration.
*
*\param  [in]   deviceId        Peer device identifier.
*\param  [out]  pConfig         Pointer to the localization configuration information.
*
*\retval        bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_WriteConfig
(
    deviceId_t deviceId,
    appLocalization_rangeCfg_t *pConfig
);

/*! *********************************************************************************
*\fn            void AppLocalization_ResetPeer(deviceId_t deviceId,
*               bool_t disconnected, uint8_t nvmIndex)
*
*\brief         Resets localization data for the given peer device.
*
*\param  [in]   deviceId        Peer device identifier.
*\param  [in]   disconnected    TRUE if the peer disconnected, FALSE otherwise.
*\param  [in]   nvmIndex        Used to clear mpCachedRemoteCaps.
*
*\retval        none.
********************************************************************************** */
void AppLocalization_ResetPeer
(
    deviceId_t deviceId,
    bool_t disconnected,
    uint8_t nvmIndex
);

#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1)
/*! *********************************************************************************
*\fn         bleResult_t AppLocalization_SetAlgorithm(uint8_t algorithm)
*
*\brief      Configures which algorithms are to be run.
*
*\param[in]  algorithm      Bitmask (CDE, RADE).
*
*\return     None
********************************************************************************** */
void AppLocalization_SetAlgorithm
(
    uint8_t algorithm
);

/*! *********************************************************************************
*\fn         bleResult_t AppLocalization_GetAlgorithm(void)
*
*\brief      Returns which algorithms are to be run.
*
*\param[in]  none

*
*\return     algorithm      Bitmask (CDE, RADE).
********************************************************************************** */
uint8_t AppLocalization_GetAlgorithm(void);
#endif /* gAppRunAlgo_d */

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
/*! *********************************************************************************
*\fn            void AppLocalization_TimeInfoSetConnInterval(uint16_t connInterval);
*
*\brief         Set connection interval to be used for CS procedure duration estimation.
*
*\param[in]     connInterval    Connection interval value.
*
*\retval        none.
********************************************************************************** */
void AppLocalization_TimeInfoSetConnInterval
(
    uint16_t connInterval
);
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

/*! *********************************************************************************
*\fn            void AppLocalization_WriteCachedRemoteCapabilities(deviceId_t deviceId,
*               uint8_t nvmIdx);
*
*\brief         Write the cached copy of the CS capabilities that are supported by the remote Controller .
*
*\param[in]     deviceId       Peer identifier
*\param[in]     nvmIdx         Index of the bonded device in the NVM
*
*\retval        bleResult_t      Result of the operation
********************************************************************************** */
bleResult_t AppLocalization_WriteCachedRemoteCapabilities
(
    deviceId_t deviceId,
    uint8_t nvmIdx
);

/*! *********************************************************************************
*\fn            void AppLocalization_SetRemoteCachedSupportedCapabilities(uint8_t nvmIndex,
*               csReadRemoteSupportedCapabilitiesCompleteEvent_t* recvCapabilities);
*
*\brief         Set the supported capabilities received from another anchor.
*
*\param[in]     nvmIndex      Index of the device in the NVM
*\param[in]     csReadRemoteSupportedCapabilitiesCompleteEvent_t Pointer to peer capabilities
*
*\retval        none
********************************************************************************** */
void AppLocalization_SetRemoteCachedSupportedCapabilities
(
    uint8_t nvmIndex,
    csReadRemoteSupportedCapabilitiesCompleteEvent_t* recvCapabilities
);

/*! *********************************************************************************
*\fn            appLocalization_State_t AppLocalization_GetLocState(deviceId_t deviceId);
*
*\brief         Get the current local state in relation to the specified peer.
*
*\param[in]     deviceId                   Peer identifier
*
*\retval        appLocalization_State_t    Local state
********************************************************************************** */
appLocalization_State_t AppLocalization_GetLocState
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            void AppLocalization_SetLocState(deviceId_t deviceId,
*               appLocalization_State_t recvState);
*
*\brief         Set the local state value received from another anchor.
*
*\param[in]     deviceId      Peer identifier
*\param[in]     recvState     State value
*
*\retval        none
********************************************************************************** */
void AppLocalization_SetLocState
(
    deviceId_t deviceId,
    appLocalization_State_t recvState
);

/*! *********************************************************************************
*\fn            void AppLocalization_GetRemoteCachedSupportedCapabilities(uint8_t nvmIndex);
*
*\brief         Get the supported capabilities for the specified peer.
*
*\param[in]     nvmIndex      Index of the device in the NVM
*
*\retval        csReadRemoteSupportedCapabilitiesCompleteEvent_t* Pointer to peer capabilities
********************************************************************************** */
csReadRemoteSupportedCapabilitiesCompleteEvent_t* AppLocalization_GetRemoteCachedSupportedCapabilities
(
    uint8_t nvmIndex
);

#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
/*! *********************************************************************************
*\fn            void AppLocalization_GetAlgoRunCount(deviceId_t deviceId);
*
*\brief         Get the current algorithm runs count value for the specified peer.
*
*\param[in]     deviceId      Peer identifier
*
*\retval        uint16_t      Algorithm runs count value
********************************************************************************** */
uint16_t AppLocalization_GetAlgoRunCount
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            void AppLocalization_SetAlgoRunCount(deviceId_t deviceId,
*               uint16_t recvAlgoRunCount);
*
*\brief         Set the algorithm runs count value received from another anchor.
*
*\param[in]     deviceId                Peer identifier
*\param[in]     recvAlgoRunCount        Algorithm runs count value
*
*\retval        none
********************************************************************************** */
void AppLocalization_SetAlgoRunCount
(
    deviceId_t deviceId,
    uint16_t   recvAlgoRunCount
);
#endif

/*! *********************************************************************************
*\fn            bool_t AppLocalization_GetRttSoundingSupport(void);
*
*\brief         Check local RTT sounding support.
*
*\param[in]     none
*
*\retval        TRUE if RTT sounding is supported, FALSE otherwise
********************************************************************************** */
bool_t AppLocalization_GetRttSoundingSupport(void);

/*! *********************************************************************************
*\fn            uint8_t AppLocalization_GetSubeventIdx(deviceId_t deviceId);
*
*\brief         Get the current subevent index value for the specified peer.
*
*\param[in]     deviceId      Peer identifier
*
*\retval        uint8_t       Subevent index
********************************************************************************** */
uint8_t AppLocalization_GetSubeventIdx
(
    deviceId_t deviceId
);

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
/*! *********************************************************************************
*\fn            uint16_t AppLocalization_GetProcedureCount(deviceId_t deviceId);
*
*\brief         Get the global procedure counter value for the specified peer.
*
*\param[in]     deviceId      Peer identifier
*
*\retval        uint16_t      Procedure count value
********************************************************************************** */
uint16_t AppLocalization_GetGlobalProcedureCount
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            uint8_t AppLocalization_GetProcDoneStatus(deviceId_t deviceId,
*                                                         uint8_t subeventIdx);
*
*\brief         Get the procedure done status for the specified subevent index and peer.
*
*\param[in]     deviceId      Peer identifier
*\param[in]     subeventIdx   Index of the subevent for which to return the procedure
*                             done status
*
*\retval        uint8_t       Procedure done status
********************************************************************************** */
uint8_t AppLocalization_GetProcDoneStatus
(
    deviceId_t deviceId,
    uint8_t    subeventIdx
);

#endif /* gRasRREQ_d */
#else
#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
/*! *********************************************************************************
*\fn            void AppLocalization_SetPsmChannelId(deviceId_t deviceId, uint16_t channelId);
*
*\brief         Sets which L2CAP PSM to be used for the BTCS protocol
*
*\param[in]     deviceId         Peer identifier
*
*\retval        none
********************************************************************************** */
void AppLocalization_SetPsmChannelId
(
    deviceId_t deviceId,
    uint16_t   channelId
);
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */
#endif /* gAppRasDataTransfer_d */

/*! *********************************************************************************
*\fn            void AppLocalization_AllocLocalData(deviceId_t deviceId);
*
*\brief         Allocate the local CS data space for the specified peer.
*
*\param[in]     deviceId         Peer identifier
*
*\retval        pointer to the allocated area
********************************************************************************** */
void* AppLocalization_AllocLocalData
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            void AppLocalization_ClearLocalData(deviceId_t deviceId);
*
*\brief         Clear the local CS data for the specified peer.
*
*\param[in]     deviceId         Peer identifier
*
*\retval        none
********************************************************************************** */
void AppLocalization_ClearLocalData
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            void AppLocalization_FreeLocalData(deviceId_t deviceId);
*
*\brief         Free the memory of the local CS data for the specified peer.
*
*\param[in]     deviceId         Peer identifier
*
*\retval        none
********************************************************************************** */
void AppLocalization_FreeLocalData
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            uint8_t AppLocalization_GetNumAntennaPaths(deviceId_t deviceId);
*
*\brief         Get the number of antenna paths for the specified subevent index and peer.
*
*\param[in]     deviceId      Peer identifier
*
*\retval        uint8_t       Procedure done status
********************************************************************************** */
uint8_t AppLocalization_GetNumAntennaPaths
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            void AppLocalization_GetProcedureCount(deviceId_t deviceId);
*
*\brief         Get the current procedure count value for the specified peer.
*
*\param[in]     deviceId      Peer identifier
*
*\retval        uint16_t      Procedure count value
********************************************************************************** */
uint16_t AppLocalization_GetProcedureCount
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            void AppLocalization_ComputeMaxProcedureDuration(uint32_t procInterval,
*                               uint16_t connInterval, uint16_t *pOutMaxProcDuration);
*
*\brief         Helper function. Compute the CS Max Procedure Duration based on Proc
*               Interval and the connection interval.
*
*\param[in]     procInterval         CS Procedure Interval in number of connection intervals.
*\param[in]     connInterval         Bluetooth LE connection interval in units of 1.25ms.
*\param[out]    pOutMaxProcDuration  Pointer to computed Max Proc Duration value.
*
*\retval        None
********************************************************************************** */
void AppLocalization_ComputeMaxProcedureDuration
(
    uint32_t procInterval,
    uint16_t connInterval,
    uint16_t *pOutMaxProcDuration
);

/*! *********************************************************************************
*\fn            void AppLocalization_ComputeSubeventLength(uint32_t connInterval,
*                               uint32_t *pOutMinSubeventLen, uint32_t *pOutMaxSubeventLen);
*
*\brief         Helper function. Compute the CS Min/Max Subevent Length based on system
*               and controller constraints based on a custom rule.
*
*\param[in]     connInterval         Bluetooth LE connection interval in units of 1.25ms.
*\param[out]    pOutMinSubeventLen   Pointer to computed Min Subevent Len value.
*\param[out]    pOutMaxSubeventLen   Pointer to computed Max Subevent Len value.
*
*\retval        None
********************************************************************************** */
void AppLocalization_ComputeSubeventLength
(
    uint32_t connInterval,
    uint32_t *pOutMinSubeventLen,
    uint32_t *pOutMaxSubeventLen
);

/*! *********************************************************************************
*\fn            bool_t AppLocalization_RunAlgorithm(deviceId_t deviceId);
*
*\brief         Runs the distance measurement algorithm for a given peer.
*
*\param[in]     deviceId         Peer identifier
*
*\retval        none
********************************************************************************** */
void AppLocalization_RunAlgorithm
(
    deviceId_t deviceId
);

/*! *********************************************************************************
*\fn            bleResult_t AppLocalization_CreateConfig(deviceId_t deviceId,
*                                       uint8_t configId, bool_t createContextRemote)
*
*\brief         Create a new localization configuration.
*
*\param  [in]   deviceId               Peer device identifier.
*\param  [in]   configId               Configuration Id.
*\param  [out]  createContextRemote    TRUE if configuration must be created on the peer device
*
*\retval        bleResult_t     Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_CreateConfig
(
    deviceId_t deviceId,
    uint8_t    configId,
    bool_t     createContextRemote
);

/*! *********************************************************************************
*\fn            void AppLocalization_SetDefaultConfig(deviceId_t deviceId, uint8_t configId)
*
*\brief         Set the default CS configuration to be used with the given peer.
*               This value will later be used to start the CS procedure.
*
*\param  [in]   deviceId               Peer device identifier.
*\param  [in]   configId               Configuration Id.
*
*\retval        none
********************************************************************************** */
void AppLocalization_SetDefaultConfig
(
    deviceId_t deviceId,
    uint8_t    configId
);

#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
/*! *********************************************************************************
*\fn            void AppLocalization_SetTakEnable(deviceId_t deviceId)
*
*\brief         Set the the TAK support for a connection.
*
*\param  [in]   deviceId               Peer device identifier.
*
*\retval        none
********************************************************************************** */
void AppLocalization_SetTakEnable(deviceId_t deviceId);
#endif /* #if (defined(gAppUseTAK_d) && gAppUseTAK_d) */

#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
/*! *********************************************************************************
*\fn         AppLocalization_RunPtsTest(deviceId_t deviceId, uint8_t index, uint8_t actionMask)
*
*\brief      Test vector parsing to process the data.
*
*\return     None
********************************************************************************** */
void AppLocalization_RunPtsTest(deviceId_t deviceId, uint8_t index, uint8_t actionMask);
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* APP_LOCALIZATION_H */