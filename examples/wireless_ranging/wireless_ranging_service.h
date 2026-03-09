/*
 * Copyright 2020-2023, 2025-2026 NXP
 *
 * NXP Proprietary
 *
 * This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or
 * by downloading, installing, activating and/or otherwise using the software, you are
 * agreeing that you have read, and that you agree to comply with and are bound by,
 * such license terms. If you do not agree to be bound by the applicable license terms,
 * then you may not retain, install, activate or otherwise use the software.
 */

/* Prevent double inclusion */
#ifndef _WIRELESS_RANGING_SERVICE_H
#define _WIRELESS_RANGING_SERVICE_H

/************************************************************************************
 *************************************************************************************
 *  Includes
 *************************************************************************************
 ************************************************************************************/
#include "EmbeddedTypes.h"
#include "wireless_ranging_measurement.h"

/************************************************************************************
 *************************************************************************************
 *  Macros
 *************************************************************************************
 ************************************************************************************/
#define CS_TEST_DEVICEID (gAppMaxConnections_c)

/************************************************************************************
 *************************************************************************************
 *  Types
 *************************************************************************************
 ************************************************************************************/
/*! Return status values for data frame transfer */
typedef enum wrsReturnCode_tag {
    eReturnSuccess = 0,
    eReturnUnknownProtocolType,
    eReturnUnexpectedFrameType,
    eReturnInvalidFrameLength,
    eReturnFrameTooBig,
    eReturnWrongFrameRemain,
    eReturnUnorderedFragment,
    eReturnUnknownFrameType,
    eReturnWrongFrameLength,
    eReturnEmptyFrame,
    eReturnError
} wrs_ReturnCode_t;

/*! Possible GATT roles */
typedef enum wrs_Role_tag {
    eRoleNone,
    eRoleServer,
    eRoleClient,
} wrs_Role_t;

/*! Timestamp types */
typedef enum {
    eTimestampStartMeas,
    eTimestampStartTransfer,
    eTimestampStartPostProcessing,
    eTimestampStop,
    eTimestampMax
} wrs_Timestamp_t;

/*! Information regarding ranging service execution */
typedef struct wrs_ServiceInfo_tag {
    bool dataExchangeDone;
    uint32_t timestamps[(uint32_t)eTimestampMax];
} wrs_ServiceInfo_t;

/*! Measurement Complete Event */
typedef struct wrs_AppMeasureCompleteEvent_tag {
    wrs_ServiceInfo_t serviceInfo;
    csEventResultBuffer_t *localEventResultBuffer;
    csEventResultBuffer_t *remoteEventResultBuffer;
    csEventDebugBuffer_t *localEventDebugBuffer;
    csEventDebugBuffer_t *remoteEventDebugBuffer;
} wrs_AppMeasureCompleteEvent_t;

/*! List of possible ranging events */
typedef enum {
    eAppEvtEventComplete,
    eAppEvtMeasureComplete,
} wrs_AppEventType_t;

/*! Controller event structure: type + data. */
typedef struct {
    /*!< Event type. */
    wrs_AppEventType_t eventType;
    deviceId_t deviceId;
    /*!< Event data : selected according to event type. */
    union {
        /*!< For event type eCtrlEvtMeasureComplete: measurement result. */
        wrs_AppMeasureCompleteEvent_t  measureCompleteEvent;
    } evt_d;
} wrs_app_event_t;

/************************************************************************************
 *************************************************************************************
 *  Public function prototypes
 *************************************************************************************
 ************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*! Callback to be implemented by application in order to receive lcl events */
void AppLclCtrlCallback(wrs_app_event_t *pEvent);

/*! Return TRUE if the service is ready for range requests. */
bool wrs_ConnIsReady(const deviceId_t deviceId);
/*! Initialize data associated with the given peer */
void wrs_ConnDataInit(const deviceId_t deviceId, const uint16_t connInterval);
/*! Clear data associated with the given peer */
void wrs_ConnDataRelease(deviceId_t deviceId);
/*! Initialize CS procedure data */
void wrs_CSDataInit(deviceId_t deviceId, const csProcedureEnableCompleteEvent_t *csProcedureEnableCompleteEvent);
/*! Trigger application event to enable either the test or the CS procedure */
void wrs_StartProcedure(deviceId_t deviceId);
/*! Trigger application event to create a new CS configuration */
void wrs_StartConfig(deviceId_t deviceId);
/*! Trigger application event to read the peer's capabilities */
void wrs_StartSetup(deviceId_t deviceId);
/*! Trigger application event to initiate the security procedure */
void wrs_StartSecurity(deviceId_t deviceId);
/*! Trigger application event to set the default CS settings. */
void wrs_SetDefaultSettings(deviceId_t deviceId);
/*! Trigger application event to signal the set default settings is complete */
void wrs_SetDefaultSettingsComplete(deviceId_t deviceId);
/*! Trigger application event to signal the security procedure is complete */
void wrs_SecurityComplete(deviceId_t deviceId);
/*! Trigger application event to switch to idle state and release resources */
void wrs_ProcTearDown(deviceId_t deviceId);
/*! Return the GATT role of the peer device */
wrs_Role_t wrs_GetRole(deviceId_t deviceId);
/*! Reset procedure data for procedure repeat scenario. */
void wrs_ResetProcedureData(deviceId_t deviceId);

/*! Entry point to be called when the application receives a BLE packet
    targetted to the ranging controller.*/
void wrs_msg_received
(
    uint8_t deviceId,
    uint8_t *pStream,
    uint16_t streamSize
);
/*! Trigger application event to signal the receival of a subevent result event */
void wrs_NotifyEventResult
(
    deviceId_t deviceId,
    csEventResultBuffer_t *eventResultBuffer,
    csEventDebugBuffer_t *eventDebugBuffer
);
/*! Trigger application event to signal the receival of a debug event */
void wrs_NotifyDebugResult
(
    deviceId_t deviceId,
    csEventResultBuffer_t *eventResultBuffer,
    csEventDebugBuffer_t *eventDebugBuffer
);
/*! Trigger application event to signal the receival of a HCI command error event */
void wrs_NotifyHCICommandError
(
    deviceId_t deviceId,
    uint32_t cmdOpCode,
    uint32_t cmdReturnCode
);
/*! Set the role of the local device as either GATT client or server */
void wrs_SetRole
(
    deviceId_t deviceId,
    wrs_Role_t role
);
/*! Save timestamp in service information */
void wrs_RecordTimestamp
(
    wrs_ServiceInfo_t *serviceInfo,
    wrs_Timestamp_t timestampId
);

/*! Handle Subevent Result Events */
void wrs_ProcessResultEvents
(
    deviceId_t deviceId,
    csSubeventResultEvent_t* pEvent
);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* _WIRELESS_RANGING_SERVICE_H */

