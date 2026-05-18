/*
 * Copyright 2022 - 2026 NXP
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

/************************************************************************************
 *************************************************************************************
 *  Includes
 *************************************************************************************
 ************************************************************************************/
#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "EmbeddedTypes.h"

#include "fsl_component_timer_manager.h"

#include "isp_debug_helpers.h"
#include "wireless_ranging.h"
#include "wireless_ranging_service.h"
#include "wireless_ranging_measurement.h"

/* BLE Host Stack */
#include "gatt_interface.h"

/************************************************************************************
 *************************************************************************************
 * Private macros
 *************************************************************************************
 ************************************************************************************/
/* Multiplier for application timeout detection (depends on connection interval for more accurate detection) */
#define TIMEOUT_CI_MULTIPLIER (10U)

/* Multiplier for CS procedure application timeout detection (depends on connection interval for more accurate detection) */
/* Since we do not know how many subevents/CI will be used by the controller to run the procedure,
   we need to account for a pretty large number of CI */
#define TIMEOUT_CS_PROC_CI_MULTIPLIER (50U)


/************************************************************************************
 *************************************************************************************
 *  Types
 *************************************************************************************
 ************************************************************************************/
typedef struct wrs_EventResultCompleteEvent_tag {
    csEventResultBuffer_t *eventResult;
    csEventDebugBuffer_t *eventDebug;
} wrs_EventResultCompleteEvent_t;

typedef struct wrs_EventArgs_tag {
    uint32_t arg1;
    uint32_t arg2;
} wrs_EventArgs_t;

/*! List of possible internal WRS events */
typedef enum {
    eEvtProcedureCapabilities,
    eEvtSetDefaultSettings,
    eEvtSetDefaultSettingsComplete,
    eEvtProcedureSecurity,
    eEvtProcedureSecurityComplete,
    eEvtProcedureConfig,
    eEvtProcedureStart,
    eEvtEventResultComplete,
    eEvtOTAResultFrameReceived,
    eEvtOTADebugFrameReceived,
    eEvtTimerOTAFragmentTx,
    eEvtTearDown,
    eEvtOTARxError = 100,
    eEvtOTAUnexpectedFrameReceived,
    eEvtHCICommandError,
    eEvtTimoutOTAResultRx,
    eEvtTimoutOTADebugResultRx,
    eEvtTimoutHCIResult,
} wrs_EventType_t;

/*! FSM States */
typedef enum {
    eStateNotReady,
    eStateIdle,
    eStateWaitEventCapabilities,
    eStateWaitEventSecurity,
    eStateWaitEventConfig,
    eStateWaitEventResult,
    eStateWaitOTAResult,
    eStateTxOTAResult,
    eStateWaitOTADebugResult,
    eStateTxOTADebugResult,
    eStateDone,
} wrs_StateType_t;

/*! Data related to a connection */
typedef struct wrs_RangingConnection_tag {

    /*== Config data */
    wrs_StateType_t state;
    wrs_Role_t wrsRole;
    bool setupComplete;
    wrs_ServiceInfo_t serviceInfo;
    TIMER_MANAGER_HANDLE_DEFINE(timerHandle);
    csEventResultBuffer_t *localCsDataBuffer;
    csEventResultBuffer_t *remoteCsDataBuffer;
    csEventDebugBuffer_t *localCsDebugBuffer;
    csEventDebugBuffer_t *remoteCsDebugBuffer;
    uint32_t connIntervalMs;  /* Will help to adjust timeout values */

    csProcedureEnableCompleteEvent_t csProcedureEnableParams;

    /*== Runtime data */
    uint16_t procIter; /* current procedure repetition counter */
    /* fragmented buffers management */
    uint16_t bufferOffset;    /* Rx offset when client, Tx offset when server */
    uint16_t bufferSize;
    uint8_t nxtFragmentId;
    uint8_t expectedFrameType;
} wrs_RangingConnection_t;

/*! For internal use */
typedef enum wrs_FrameType_tag
{
    eFrameTypeNone = 0,
    eFrameTypeResultData,
    eFrameTypeDebugData,
    eFrameTypeMax
} wrs_FrameType_t;

/*! Ensure that all frames start with frameType on 2 bytes */
typedef PACKED_STRUCT wrs_FrameResult_tag {
        uint8_t frameType;
        uint8_t fragmentId;
        uint16_t size;
        uint16_t remain;
} wrs_FrameResult_t;

/*! Reserve last entry for test mode */
static wrs_RangingConnection_t gWrsConn[gAppMaxConnections_c + 1U];

/*! CS data storage buffers */
static csEventResultBuffer_t gLocalEventResultBuffer;
static csEventResultBuffer_t gRemoteEventResultBuffer;
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
static csEventDebugBuffer_t gLocalEventDebugBuffer;
static csEventDebugBuffer_t gRemoteEventDebugBuffer;
#endif

/************************************************************************************
 *************************************************************************************
 *  Private memory declarations
 *************************************************************************************
 ************************************************************************************/
/*! Buffer to build frame before sending it to the stack */
static uint8_t gFrameBuffer[gAttMaxMtu_c] __ALIGNED(4);

static deviceId_t gCurrentDeviceId;

/************************************************************************************
 *************************************************************************************
 *  Private prototypes
 *************************************************************************************
 ************************************************************************************/
/*! Handles the given event for the specified peer */
static void wrs_EventHandler
(
    deviceId_t deviceId,
    wrs_EventType_t ctrlEvent,
    const void *arg
);
/*! API to post an event to the WRS controller */
static void wrs_EventSend
(
    deviceId_t deviceId,
    wrs_EventType_t ctrlEvent,
    const void *arg
);
/*! API to trigger a timer event */
static void wrs_StartEventTimer
(
    deviceId_t deviceId,
    wrs_EventType_t event,
    uint32_t timeoutMs
);
/*! Reset Fragmentation data */
static void wrs_ResetFragmentationData
(
    deviceId_t deviceId,
    uint8_t expectedFrameType
);
/*! Write fragment to remote CS data buffer */
static void wrs_SetData
(
    deviceId_t deviceId,
    uint8_t *buffer,
    uint16_t offset,
    uint16_t frame_length
);
/*! Read fragment from local CS data */
static void wrs_GetData
(
    deviceId_t deviceId,
    uint8_t *buffer,
    uint32_t offset,
    uint32_t frame_length
);

#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
/*! Write fragment to remote CS data buffer */
static void wrs_SetDebugData
(
    deviceId_t deviceId,
    uint8_t *buffer,
    uint16_t offset,
    uint16_t frame_length
);

/*! Read fragment from local CS data */
static void wrs_GetDebugData
(
    deviceId_t deviceId,
    uint8_t *buffer,
    uint32_t offset,
    uint32_t frame_length
);
#endif

/*! Send a frame to the specified peer */
static wrs_ReturnCode_t wrs_FrameSend
(
    deviceId_t deviceId,
    wrs_FrameType_t frameType
);
/*! Handle a received frame */
static wrs_ReturnCode_t wrs_FrameHandle
(
    deviceId_t deviceId,
    uint8_t *pStream,
    uint16_t streamSize
);

/*! Timer callback, deviceId and eventType encoded into timer callback parameter */
static void wrs_TimerHandler(void *param);
/*! Timer callback, deviceId and eventType encoded into timer callback parameter */
static void wrs_TimerHandler(void *param);
/*! API to stop a timer event */
static void wrs_StopEventTimer(deviceId_t deviceId);
/*! Initialize procedure data */
static void wrs_ProcDataInit(deviceId_t deviceId);

/*! Get the local buffer with event result data */
static csEventResultBuffer_t *wrs_getLocalEventResultBuffer(deviceId_t deviceId);
/*! Get the buffer with event result data received from the peer */
static csEventResultBuffer_t *wrs_getRemoteEventResultBuffer(deviceId_t deviceId);

#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
/*! Get the local buffer with debug data */
static csEventDebugBuffer_t *wrs_getLocalEventDebugBuffer(deviceId_t deviceId);
/*! Get the buffer with debug data received from the peer */
static csEventDebugBuffer_t *wrs_getRemoteEventDebugBuffer(deviceId_t deviceId);
#endif

/************************************************************************************
 *************************************************************************************
 *  Public functions
 *************************************************************************************
 ************************************************************************************/

/*! *********************************************************************************
 * \brief        Return TRUE if the service is ready for range requests.
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
bool wrs_ConnIsReady(const deviceId_t deviceId)
{
    return (gWrsConn[deviceId].setupComplete);
}

/*! *********************************************************************************
 * \brief        Initialize data associated with the given peer.
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_ConnDataInit(const deviceId_t deviceId, const uint16_t connInterval)
{
    assert(deviceId < ((uint8_t)gAppMaxConnections_c + 1U));
    gWrsConn[deviceId].wrsRole = eRoleNone;
    gWrsConn[deviceId].state = eStateIdle;
    union {
        uint32_t v32;
        uint16_t v16;
        float vf;
    } tmp = {0U};
    tmp.v16 = connInterval;
    tmp.vf = 1.25f * tmp.vf;
    gWrsConn[deviceId].connIntervalMs = tmp.v32;
    (void)TM_Open((timer_handle_t)gWrsConn[deviceId].timerHandle);
}

/*! *********************************************************************************
 * \brief        Release the data associated with the given peer.
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_ConnDataRelease(deviceId_t deviceId)
{
    DPRINTF_DBG("wrs_ConnDataRelease id=%d\n", deviceId);
    gWrsConn[deviceId].state = eStateNotReady;
    gWrsConn[deviceId].setupComplete = FALSE;
    (void)TM_Close((timer_handle_t)gWrsConn[deviceId].timerHandle);
    FLib_MemSet(&gWrsConn[deviceId], 0, sizeof(wrs_RangingConnection_t));
    /* TODO multi-conn : Release buffers */
}

/*! *********************************************************************************
 * \brief        Initialize CS procedure data associated with the given peer.
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_CSDataInit(deviceId_t deviceId, const csProcedureEnableCompleteEvent_t *csProcedureEnableCompleteEvent)
{
    if (deviceId != (uint8_t)CS_TEST_DEVICEID) {
        assert(csProcedureEnableCompleteEvent != NULL);
        /* Store given configuration */
        gWrsConn[deviceId].csProcedureEnableParams = *csProcedureEnableCompleteEvent;
        gRangeSettings->subeventInterval = csProcedureEnableCompleteEvent->subeventInterval;
    }
}

/*! *********************************************************************************
* \brief        Entry point to be called when application receives a BLE packet
*               targetted to the ranging controller
*
* \param[in] deviceId       Identifier of BLE device (as defined by SDK BLE API)
* \param[in] pStream        Pointer to received packet
* \param[in] streamSize     Size of received packet
********************************************************************************** */
void wrs_msg_received
(
    uint8_t deviceId,
    uint8_t *pStream,
    uint16_t streamSize
)
{
    wrs_ReturnCode_t rc;
    wrs_FrameResult_t frame;
    union {
        uintptr_t vptr;
        void *vp;
    } arg = {0U};

    FLib_MemCpy(&frame, pStream, sizeof(frame));

    DPRINTF_DBG("Rx OTA Msg type=%d streamSz=%d\n", frame.frameType, streamSize);
    if (gWrsConn[deviceId].expectedFrameType == (uint8_t)eFrameTypeNone) {
        arg.vptr = (uintptr_t)frame.frameType;
        wrs_EventHandler(deviceId, eEvtOTAUnexpectedFrameReceived, arg.vp);
    } else {
        rc = wrs_FrameHandle(deviceId, pStream, streamSize);
        if (rc != eReturnSuccess)
        {
            DPRINTF_DBG("Rx frame error %d\n", rc);
            arg.vptr = (uintptr_t)rc;
            wrs_EventHandler(deviceId, eEvtOTARxError, arg.vp);
        }
    }
}

/*! *********************************************************************************
 * \brief        Trigger application event to enable either the test or the CS procedure
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_StartProcedure(deviceId_t deviceId)
{
    wrs_StartEventTimer(deviceId, eEvtProcedureStart, 1U);
}

/*! *********************************************************************************
 * \brief        Trigger application event to create a new CS configuration
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_StartConfig(deviceId_t deviceId)
{
    wrs_StartEventTimer(deviceId, eEvtProcedureConfig, 1U);
}

/*! *********************************************************************************
 * \brief        Trigger application event to read the peer's capabilities.
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_StartSetup(deviceId_t deviceId)
{
    measurement_send_vs_config(gRangeSettings);
    wrs_StartEventTimer(deviceId, eEvtProcedureCapabilities, 1U);
}

/*! *********************************************************************************
 * \brief        Trigger application event to initiate the security procedure.
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_StartSecurity(deviceId_t deviceId)
{
    wrs_EventSend(deviceId, eEvtProcedureSecurity, NULL);
}

/*! *********************************************************************************
 * \brief        Trigger application event to set the default CS settings.
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_SetDefaultSettings(deviceId_t deviceId)
{
    wrs_EventSend(deviceId, eEvtSetDefaultSettings, NULL);
}

/*! *********************************************************************************
 * \brief        Trigger application event to signal the set default settings is complete.
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_SetDefaultSettingsComplete(deviceId_t deviceId)
{
    wrs_EventSend(deviceId, eEvtSetDefaultSettingsComplete, NULL);
}

/*! *********************************************************************************
 * \brief        Trigger application event to switch to idle state and release resources.
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_SecurityComplete(deviceId_t deviceId)
{
    wrs_EventSend(deviceId, eEvtProcedureSecurityComplete, NULL);
}

/*! *********************************************************************************
 * \brief        Trigger application event to signal the receival of a result event.
 *
 * \param[in]    deviceId               Peer identifier
 * \param[in]    eventResultBuffer      Buffer with event result data
 * \param[in]    eventDebugBuffer       Buffer with debug data
 ********************************************************************************** */
void wrs_NotifyEventResult
(
    deviceId_t deviceId,
    csEventResultBuffer_t *eventResultBuffer,
    csEventDebugBuffer_t *eventDebugBuffer
)
{
    wrs_EventResultCompleteEvent_t eventResultComplete = {
        eventResultBuffer,
        eventDebugBuffer};
    wrs_EventSend(deviceId, eEvtEventResultComplete, &eventResultComplete);
}

/*! *********************************************************************************
 * \brief        Trigger application event to signal the receival of a HCI
 *               command error event.
 *
 * \param[in]    deviceId           Peer identifier
 * \param[in]    cmdOpCode          Code of the command which caused the error
 * \param[in]    cmdReturnCode      Error code generated
 ********************************************************************************** */
void wrs_NotifyHCICommandError
(
    deviceId_t deviceId,
    uint32_t cmdOpCode,
    uint32_t cmdReturnCode
)
{
    if (deviceId == gInvalidDeviceId_c)
    {
        deviceId = gCurrentDeviceId;
    }
    wrs_EventArgs_t args = {cmdOpCode, cmdReturnCode};
    wrs_EventSend(deviceId, eEvtHCICommandError, &args);
}

/*! *********************************************************************************
 * \brief        Trigger application event to switch to idle state and release resources.
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_ProcTearDown(deviceId_t deviceId)
{
    wrs_EventSend(deviceId, eEvtTearDown, NULL);
}

/*! *********************************************************************************
 * \brief        Set the GATT role of the peer device (server or client).
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_SetRole(deviceId_t deviceId, wrs_Role_t role)
{
    gWrsConn[deviceId].wrsRole = role;
}

/*! *********************************************************************************
 * \brief        Return the GATT role of the peer device (server or client).
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
wrs_Role_t wrs_GetRole(deviceId_t deviceId)
{
    return gWrsConn[deviceId].wrsRole;
}

/*! *********************************************************************************
 * \brief        Save timestamp in service information.
 *
 * \param[in]    serviceInfo      Service information
 * \param[in]    timestampId      Timestamp id
 ********************************************************************************** */
void wrs_RecordTimestamp
(
    wrs_ServiceInfo_t *serviceInfo,
    wrs_Timestamp_t timestampId
)
{
    serviceInfo->timestamps[timestampId] = isp_soc_timestamp_get();
}

/*! *********************************************************************************
 * \brief        Reset procedure data for procedure repeat scenario.
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
void wrs_ResetProcedureData(deviceId_t deviceId)
{
    deviceId_t peerDeviceId;

    if (deviceId == gInvalidDeviceId_c)
    {
        peerDeviceId = CS_TEST_DEVICEID;
    }
    else
    {
        peerDeviceId = deviceId;
    }

    gWrsConn[peerDeviceId].procIter++;

    DPRINTF_DBG("Procedure #%d/%d\n", gWrsConn[peerDeviceId].procIter, gWrsConn[peerDeviceId].csProcedureEnableParams.procedureCount);
    /* Test if procedure repeat is in progress */
    if (gWrsConn[peerDeviceId].procIter < gWrsConn[peerDeviceId].csProcedureEnableParams.procedureCount)
    {
        /* A new procedure repeat is expected, wait for next subeventResult */
        wrs_StopEventTimer(peerDeviceId);
        wrs_ResetFragmentationData(peerDeviceId, (uint8_t)eFrameTypeNone);
        gWrsConn[peerDeviceId].state = eStateWaitEventResult;
        wrs_ProcDataInit(peerDeviceId);
        measurement_buffer_init(meas_params.cfg.debug);
        /* In repeat mode, we cannot do better than timestamping start of the procedure at the ime the current one completes */
        wrs_RecordTimestamp(&gWrsConn[peerDeviceId].serviceInfo, eTimestampStartMeas);
        wrs_StartEventTimer(peerDeviceId,
                            eEvtTimoutHCIResult,
                            gWrsConn[peerDeviceId].csProcedureEnableParams.procedureInterval*gWrsConn[peerDeviceId].connIntervalMs);
    }
    else
    {
        /* procedure repetition is complete */
        gWrsConn[peerDeviceId].procIter = 0;
        /* Switch to eStateIdle, release resources */
        gWrsConn[peerDeviceId].state = eStateIdle;
        /* Switch back to default role in connected mode */
        if (peerDeviceId != (uint8_t)CS_TEST_DEVICEID)
        {
            gWrsConn[peerDeviceId].wrsRole = eRoleServer;
        }
    }
}


/*! *********************************************************************************
 * \brief        Handle Subevent Result Events.
 *
 * \param[in]    deviceId      Peer identifier
 * \param[in]    pEvent        Subevent Result Event data
 ********************************************************************************** */
void wrs_ProcessResultEvents
(
    deviceId_t deviceId,
    csSubeventResultEvent_t* pEvent
)
{
    uint8_t *eventData = pEvent->pData;

    /* Clear any data from previous procedures */
    if ((measurement_check_proc_in_progress() == TRUE) &&
        (gWrsConn[deviceId].state != eStateWaitEventResult))
    {
        DPRINTF("CS measurement [%d] dropped due to an overflow! Starting procedure [%d].. /n",
                gWrsConn[deviceId].procIter,
                pEvent->procedureCounter);

        wrs_ResetProcedureData(deviceId);
    }

    processCsEventResultData(pEvent, eventData);

    /* Only status 0x01 means continue */
    if (pEvent->procedureDoneStatus != 0x01U)
    {
        DEBUG_PIN0_PLS
        notifyEndOfEventResult(deviceId, pEvent->procedureDoneStatus);
    }
}

/************************************************************************************
 *************************************************************************************
 *  Private functions
 *************************************************************************************
 ************************************************************************************/

/*! *********************************************************************************
* \brief        Handle a newly received frame from the specified peer.
*
* \param[in] deviceId       Identifier of BLE device (as defined by SDK BLE API)
* \param[in] pStream        Pointer to received packet
* \param[in] streamSize     Size of received packet
********************************************************************************** */
static wrs_ReturnCode_t wrs_FrameHandle
(
    deviceId_t deviceId,
    uint8_t *pStream,
    uint16_t streamSize
)
{
    wrs_EventType_t fsmEvent = eEvtOTARxError;
    void (* setData) (deviceId_t deviceId, uint8_t *buffer, uint16_t offset, uint16_t frame_length) = NULL;
    wrs_ReturnCode_t ret = eReturnSuccess;
    union {
        uint8_t *pStream;
        wrs_FrameResult_t *frame;
    } tmp = {0U};

    tmp.pStream = pStream;

    switch (tmp.frame->frameType)
    {
    case (uint8_t)eFrameTypeResultData:
        fsmEvent = eEvtOTAResultFrameReceived;
        setData = wrs_SetData;
        break;
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
    case (uint8_t)eFrameTypeDebugData:
        fsmEvent = eEvtOTADebugFrameReceived;
        setData = wrs_SetDebugData;
        break;
#endif
    default:
        ret = eReturnUnknownFrameType;
        break;
    }

    if (ret != eReturnSuccess)
    {
        return ret;
    }

    DPRINTF_DBG("  fragId=%d sz=%d remain=%d\n", tmp.frame->fragmentId, tmp.frame->size, tmp.frame->remain);
    if (streamSize != (sizeof(wrs_FrameResult_t) + tmp.frame->size))
    {
        return eReturnInvalidFrameLength;
    }
    if (gWrsConn[deviceId].nxtFragmentId == 0U) {
        /* First frame received */
        gWrsConn[deviceId].bufferSize = tmp.frame->remain;
        if (gWrsConn[deviceId].bufferSize >= (uint16_t)gCsRawBufferSz_c) {
            return eReturnFrameTooBig;
        }
    }
    /* Check if frame arrived 'in order' */
    if (tmp.frame->fragmentId != gWrsConn[deviceId].nxtFragmentId) {
        return eReturnUnorderedFragment;
    }
    gWrsConn[deviceId].nxtFragmentId++;
    /* Check length consistency */
    if ((gWrsConn[deviceId].bufferOffset + tmp.frame->remain) > gWrsConn[deviceId].bufferSize) {
        return eReturnWrongFrameRemain;
    }

    (*setData)(deviceId,
                &pStream[sizeof(wrs_FrameResult_t)],
                gWrsConn[deviceId].bufferOffset,
                tmp.frame->size);
    gWrsConn[deviceId].bufferOffset += tmp.frame->size; /* move to next fragment */
    wrs_EventHandler(deviceId, fsmEvent, NULL);

    return eReturnSuccess;
}

/*! *********************************************************************************
* \brief        Send event via application callback
*               Event must be comsumed within callback (allocated locally on stack)
*
* \param[in] deviceId       Identifier of BLE device (as defined by SDK BLE API)
********************************************************************************** */
static void wrs_SendAppResponse(deviceId_t peerDeviceId)
{
    wrs_app_event_t event;

    /* Trigger application callback */
    event.eventType = eAppEvtMeasureComplete;
    event.deviceId = peerDeviceId;
    wrs_AppMeasureCompleteEvent_t *evt_d = &event.evt_d.measureCompleteEvent;

    evt_d->localEventResultBuffer = wrs_getLocalEventResultBuffer(peerDeviceId);
    evt_d->remoteEventResultBuffer = wrs_getRemoteEventResultBuffer(peerDeviceId);
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
    evt_d->localEventDebugBuffer = wrs_getLocalEventDebugBuffer(peerDeviceId);
    evt_d->remoteEventDebugBuffer = wrs_getRemoteEventDebugBuffer(peerDeviceId);
#else
    evt_d->localEventDebugBuffer = NULL;
    evt_d->remoteEventDebugBuffer = NULL;
#endif

    evt_d->serviceInfo = gWrsConn[peerDeviceId].serviceInfo;
    AppLclCtrlCallback(&event);
}

/*! *********************************************************************************
* \brief        WRS controller state machine
*
* \param[in] deviceId       Identifier of BLE device (as defined by SDK BLE API)
* \param[in] ctrlEvent      Event type
* \param[in] arg            Event data
********************************************************************************** */
static void wrs_EventHandler
(
    deviceId_t deviceId,
    wrs_EventType_t ctrlEvent,
    const void *arg
)
{
    bool unexpectedEvent = FALSE;
    wrs_StateType_t currentState;
    deviceId_t peerDeviceId;

    if (deviceId == gInvalidDeviceId_c)
    {
        peerDeviceId = CS_TEST_DEVICEID;
    }
    else
    {
        peerDeviceId = deviceId;
    }
    gCurrentDeviceId = peerDeviceId;

    currentState = gWrsConn[peerDeviceId].state;
    isp_soc_eventlog((peerDeviceId << 16) | (currentState << 8) | ctrlEvent);
    DPRINTF_DBG("device %d: state %d, event %d\n", peerDeviceId, currentState, ctrlEvent);
    assert(currentState != eStateNotReady);
    switch (currentState)
    {
    case eStateIdle:
        switch(ctrlEvent)
        {
            case eEvtProcedureCapabilities:
                assert(gWrsConn[peerDeviceId].wrsRole != eRoleNone);
                if (measurement_request_capabilities(peerDeviceId) == gBleSuccess_c)
                {
                    wrs_StartEventTimer(peerDeviceId, eEvtTimoutHCIResult, TIMEOUT_CI_MULTIPLIER*gWrsConn[peerDeviceId].connIntervalMs);
                    gWrsConn[peerDeviceId].state = eStateWaitEventCapabilities;
                }
                else
                    unexpectedEvent = TRUE;
                break;
            case eEvtProcedureConfig:
                wrs_ProcDataInit(peerDeviceId);
                measurement_buffer_init(meas_params.cfg.debug);
                measurement_set_default_settings(peerDeviceId, &meas_params.cfg); /* update default settings in case they changed */
                /* Send HCI create config command and go to eStateWaitEventConfig */
                assert(gWrsConn[peerDeviceId].wrsRole != eRoleNone);
                if (gWrsConn[peerDeviceId].wrsRole == eRoleClient)
                {
                    if (measurement_configure(peerDeviceId) == gBleSuccess_c)
                    {
                        wrs_StartEventTimer(peerDeviceId, eEvtTimoutHCIResult, TIMEOUT_CI_MULTIPLIER*gWrsConn[peerDeviceId].connIntervalMs);
                        gWrsConn[peerDeviceId].state = eStateWaitEventConfig;
                    }
                    else
                        unexpectedEvent = TRUE;
                }
                break;
            case eEvtProcedureStart:
                wrs_ProcDataInit(peerDeviceId);
                gWrsConn[peerDeviceId].procIter = 0;
                measurement_buffer_init(meas_params.cfg.debug);
                /* Switch to eStateWaitEventResult, start measurement */
                if (gWrsConn[peerDeviceId].wrsRole == eRoleNone)
                {
                    /* Record timestamp in WRS testmode when start cs procesdure */
                    assert(peerDeviceId == CS_TEST_DEVICEID);
                    wrs_RecordTimestamp(&gWrsConn[peerDeviceId].serviceInfo, eTimestampStartMeas);
                    if (gBleSuccess_c != measurement_run(peerDeviceId))
                    {
                        unexpectedEvent = TRUE;
                    }
                }
                else
                {
                    /* Time between a procedure config and the first SubeventResult HCI Event is 3 + 7 CI + time needed by the CS procedure itself */
                    wrs_StartEventTimer(peerDeviceId, eEvtTimoutHCIResult, (10+TIMEOUT_CS_PROC_CI_MULTIPLIER)*gWrsConn[peerDeviceId].connIntervalMs);
                }
                gWrsConn[peerDeviceId].state = eStateWaitEventResult;
                break;

            case eEvtProcedureSecurity:
                break;
            case eEvtSetDefaultSettings:
                if (gBleSuccess_c == measurement_set_default_settings(peerDeviceId, NULL))
                {
                    wrs_StartEventTimer(peerDeviceId, eEvtTimoutHCIResult, TIMEOUT_CI_MULTIPLIER*gWrsConn[peerDeviceId].connIntervalMs);
                }
                else
                    unexpectedEvent = TRUE;
                break;
            case eEvtSetDefaultSettingsComplete:
                /* Stop the timer for the eEvtTimoutHCIResult started under eEvtSetDefaultSettings case */
                wrs_StopEventTimer(peerDeviceId);
                break;
            case eEvtProcedureSecurityComplete:
                /* received on the peripheral */
                gWrsConn[peerDeviceId].setupComplete = TRUE;
                break;
            default:
                unexpectedEvent = TRUE;
                break;
        }
        break;

    case eStateWaitEventCapabilities:
        switch(ctrlEvent)
        {
            case eEvtProcedureSecurity:
                assert(gWrsConn[peerDeviceId].wrsRole != eRoleNone);
                if (gBleSuccess_c == measurement_start_security(peerDeviceId))
                {
                    wrs_StartEventTimer(peerDeviceId, eEvtTimoutHCIResult, TIMEOUT_CI_MULTIPLIER*gWrsConn[peerDeviceId].connIntervalMs);
                    gWrsConn[peerDeviceId].state = eStateWaitEventSecurity;
                }
                else
                    unexpectedEvent = TRUE;
                break;

            default:
                unexpectedEvent = TRUE;
                break;
        }
        break;

    case eStateWaitEventSecurity:
        switch(ctrlEvent)
        {
            case eEvtProcedureSecurityComplete:
                /* CS setup phase (optional) has completed. Return to eStateIdle to serve ranging requests */
                wrs_StopEventTimer(peerDeviceId);
                gWrsConn[peerDeviceId].state = eStateIdle;
                gWrsConn[peerDeviceId].setupComplete = TRUE;
                break;

            default:
                unexpectedEvent = TRUE;
                break;
        }
        break;

    case eStateWaitEventConfig:
        switch(ctrlEvent)
        {
            case eEvtProcedureStart:
                assert(gWrsConn[peerDeviceId].wrsRole != eRoleNone);
                wrs_RecordTimestamp(&gWrsConn[peerDeviceId].serviceInfo, eTimestampStartMeas);
                /* Switch to eStateWaitEventResult, start measurement */
                if (gWrsConn[peerDeviceId].wrsRole == eRoleClient)
                {
                    if (gBleSuccess_c == measurement_run(peerDeviceId))
                    {
                        DPRINTF_DBG("HCI start\n");
                        /* Time between a procedure config and the first SubeventResult HCI Event is 3 + 7 CI + time needed by the CS procedure itself */
                        wrs_StartEventTimer(peerDeviceId, eEvtTimoutHCIResult, (10+TIMEOUT_CS_PROC_CI_MULTIPLIER)*gWrsConn[peerDeviceId].connIntervalMs);
                        gWrsConn[peerDeviceId].state = eStateWaitEventResult;
                    }
                    else
                    {
                        unexpectedEvent = TRUE;
                    }
                }
                break;
            case eEvtProcedureSecurity:
            case eEvtSetDefaultSettings:
                /* ignored in this state */
                break;
            case eEvtSetDefaultSettingsComplete:
                /* ignored in this state */
                break;
            default:
                unexpectedEvent = TRUE;
                break;
        }
        break;

    case eStateWaitEventResult:
        switch(ctrlEvent)
        {
            case eEvtProcedureStart:
                /* in test mode, we may receive a new 'test r' command while the previous one didn't complete (missed mode0) */
                if (gWrsConn[peerDeviceId].wrsRole == eRoleNone)
                {
                    /* Test mode */
                    assert(peerDeviceId == CS_TEST_DEVICEID);
                    /* End previously running command */
                    (void)measurement_test_end();
                     gWrsConn[peerDeviceId].state = eStateIdle;
                    /* Launch the command */
                    wrs_StartEventTimer(peerDeviceId, eEvtProcedureStart, 1U);
                }
                else
                {
                    unexpectedEvent = TRUE;
                }
                break;

            case eEvtEventResultComplete:
                {
                    wrs_RecordTimestamp(&gWrsConn[peerDeviceId].serviceInfo, eTimestampStartTransfer);
                    wrs_EventResultCompleteEvent_t *eventResultComplete = (wrs_EventResultCompleteEvent_t *)arg;
                    /* Backup result from HCI memory space (TODO: check with stack team) and combine events into App (procedure) memory space */
                    csEventResultBuffer_t *eventResultBuffer = wrs_getLocalEventResultBuffer(peerDeviceId);
                    assert(eventResultBuffer != NULL);
                    eventResultBuffer->rawBufferLen = eventResultComplete->eventResult->rawBufferLen;
                    FLib_MemCpy(eventResultBuffer->buffer.raw, eventResultComplete->eventResult->buffer.raw, eventResultComplete->eventResult->rawBufferLen);
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
                    csEventDebugBuffer_t *eventDebugBuffer = wrs_getLocalEventDebugBuffer(peerDeviceId);
                    assert(eventDebugBuffer != NULL);
                    eventDebugBuffer->rawBufferLen = eventResultComplete->eventDebug->rawBufferLen;
                    FLib_MemCpy(eventDebugBuffer->buffer.raw, eventResultComplete->eventDebug->buffer.raw, eventResultComplete->eventDebug->rawBufferLen);
#endif

                    /* if last event from procedure, switch to eStateWaitEndOTAResultExchange, start OTA transfer */
#if gAppUseServiceDiscovery_d
                    if (gWrsConn[peerDeviceId].wrsRole != eRoleNone)
                    {
                        wrs_ResetFragmentationData(peerDeviceId, eFrameTypeResultData);
                        if (gWrsConn[peerDeviceId].wrsRole == eRoleServer)
                        {
                            /* Record size of data to send */
                            gWrsConn[peerDeviceId].bufferSize = eventResultBuffer->rawBufferLen;
                            gWrsConn[peerDeviceId].state = eStateTxOTAResult;
                            /* Program timer to send first fragment with some delay to allow peer side to complete HCI event processing */
                            wrs_StartEventTimer(peerDeviceId, eEvtTimerOTAFragmentTx, 2U);
                        }
                        else
                        {
                            /* Wait for the BLE stack to call application callback upon pkt Rx */
                            gWrsConn[peerDeviceId].state = eStateWaitOTAResult;
                        }
                    }
                    else
#endif /* gAppUseServiceDiscovery_d */
                    {
                        /* Test mode or peer there is no WR service (IOP) */
                        gWrsConn[peerDeviceId].state = eStateDone;
                        wrs_SendAppResponse(peerDeviceId);
                    }
                }
                break;

            default:
                unexpectedEvent = TRUE;
                break;
        }
        break;

    case eStateWaitOTAResult:
        assert(gWrsConn[peerDeviceId].wrsRole == eRoleClient);
        switch(ctrlEvent)
        {
            case eEvtOTAResultFrameReceived:
            {
                if (gWrsConn[peerDeviceId].bufferOffset < gWrsConn[peerDeviceId].bufferSize)
                {
                    /* Wait for next fragment */
                }
                else
                {
                    /* Last fragment of result: Rx debug data if needed */
                    csEventResultBuffer_t *eventResultBuffer = wrs_getRemoteEventResultBuffer(peerDeviceId);
                    if (meas_params.cfg.debug && eventResultBuffer->buffer.firstResult.subeventDoneStatus == 0x00)
                    {
                        wrs_ResetFragmentationData(peerDeviceId, eFrameTypeDebugData);
                        gWrsConn[peerDeviceId].state = eStateWaitOTADebugResult;
                    }
                    else
                    {
                        /* No debug data to process, switch to eStateDone, notify app */
                        gWrsConn[peerDeviceId].state = eStateDone;
                        gWrsConn[peerDeviceId].serviceInfo.dataExchangeDone = TRUE;
                        wrs_StopEventTimer(peerDeviceId);
                        wrs_SendAppResponse(peerDeviceId);
                    }
                }
            }
            break;

            default:
                unexpectedEvent = TRUE;
                break;
        }
        break;

    case eStateTxOTAResult:
        assert(gWrsConn[peerDeviceId].wrsRole == eRoleServer);
        switch(ctrlEvent)
        {
            case eEvtTimerOTAFragmentTx:
                if (gWrsConn[peerDeviceId].bufferOffset < gWrsConn[peerDeviceId].bufferSize)
                {
                    /* Program next fragment transmission */
                    wrs_StartEventTimer(peerDeviceId, eEvtTimerOTAFragmentTx, 1U);
                    /* Send next fragment */
                    wrs_FrameSend(peerDeviceId, eFrameTypeResultData);
                }
                else
                {
                    /* Last fragment has been sent, check if debug data has to be sent OTA */
                    csEventResultBuffer_t *eventResultBuffer = wrs_getLocalEventResultBuffer(peerDeviceId);
                    if (meas_params.cfg.debug && eventResultBuffer->buffer.firstResult.subeventDoneStatus == 0x00)
                    {
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
                        /* Start sending debug data */
                        wrs_ResetFragmentationData(peerDeviceId, eFrameTypeNone);
                        /* Record size of data to send */
                        csEventDebugBuffer_t *eventDebugBuffer = wrs_getLocalEventDebugBuffer(peerDeviceId);
                        assert(eventDebugBuffer != NULL);
                        gWrsConn[peerDeviceId].bufferSize = eventDebugBuffer->rawBufferLen;
                        gWrsConn[peerDeviceId].state = eStateTxOTADebugResult;
                        /* Program next fragment transmission */
                        wrs_StartEventTimer(peerDeviceId, eEvtTimerOTAFragmentTx, 1U);
#endif
                    }
                    else
                    {
                        /* No debug data to process, switch to eStateDone, notify app */
                        gWrsConn[peerDeviceId].state = eStateDone;
                        gWrsConn[peerDeviceId].serviceInfo.dataExchangeDone = TRUE;
                        wrs_StopEventTimer(peerDeviceId);
                        wrs_SendAppResponse(peerDeviceId);
                    }
                }
                break;

            default:
                unexpectedEvent = TRUE;
                break;
        }
        break;

    case eStateWaitOTADebugResult:
        assert(gWrsConn[peerDeviceId].wrsRole == eRoleClient);
        switch(ctrlEvent)
        {
            case eEvtOTADebugFrameReceived:
                if (gWrsConn[peerDeviceId].bufferOffset < gWrsConn[peerDeviceId].bufferSize)
                {
                    /* Wait for next fragment */
                }
                else
                {
                    /* Last fragment, switch to eStateDone, notify app */
                    gWrsConn[peerDeviceId].state = eStateDone;
                    gWrsConn[peerDeviceId].serviceInfo.dataExchangeDone = TRUE;
                    wrs_StopEventTimer(peerDeviceId);
                    wrs_SendAppResponse(peerDeviceId);
                }
                break;

            default:
                unexpectedEvent = TRUE;
                break;
        }
        break;

    case eStateTxOTADebugResult:
        assert(gWrsConn[peerDeviceId].wrsRole == eRoleServer);
        switch(ctrlEvent)
        {
            case eEvtTimerOTAFragmentTx:
                if (gWrsConn[peerDeviceId].bufferOffset < gWrsConn[peerDeviceId].bufferSize)
                {
                    /* Program next fragment transmission */
                    wrs_StartEventTimer(peerDeviceId, eEvtTimerOTAFragmentTx, 1U);
                    /* Send next fragment */
                    wrs_FrameSend(peerDeviceId, eFrameTypeDebugData);
                }
                else
                {
                    /* If last frame, switch to eStateDone, notify app */
                    gWrsConn[peerDeviceId].state = eStateDone;
                    gWrsConn[peerDeviceId].serviceInfo.dataExchangeDone = TRUE;
                    wrs_StopEventTimer(peerDeviceId);
                    wrs_SendAppResponse(peerDeviceId);
                }
                break;

            default:
                unexpectedEvent = TRUE;
                break;
        }
        break;

    case eStateDone:
        switch(ctrlEvent)
        {
            case eEvtTearDown:
                wrs_ResetProcedureData(peerDeviceId);
                break;

            default:
                unexpectedEvent = TRUE;
                break;
        }
        break;

    default:
        /* Should not get here */
        assert(TRUE);
        break;
    }

    if (unexpectedEvent)
    {
        DPRINTF("error:{code:0x%0X, description: ", ctrlEvent);
        switch(ctrlEvent)
        {
            case eEvtTimoutHCIResult:
                DPRINTF("eEvtTimoutHCIResult");
                break;

            case eEvtTimoutOTAResultRx:
                DPRINTF("eEvtTimoutOTAResultRx");
                break;

            case eEvtHCICommandError:
                DPRINTF("eEvtHCICommandError 0x%0X 0x%0X",
                        ((wrs_EventArgs_t*)arg)->arg1,
                        ((wrs_EventArgs_t*)arg)->arg2);
                break;

            case eEvtOTARxError:
                DPRINTF("eEvtOTARxError 0x%0X", (uint32_t)arg);
                break;

            case eEvtOTAUnexpectedFrameReceived:
                DPRINTF("eEvtOTAUnexpectedFrameReceived st=%d 0x%0X", currentState, (uint32_t)arg);
                break;

            default:
                DPRINTF("UnexpectedTransition st=%d ev=%d", currentState, ctrlEvent);
                break;
        }
        DPRINTF("}\nmarker:[DONE]\n");
        gWrsConn[peerDeviceId].state = eStateIdle;
        wrs_StopEventTimer(peerDeviceId);
    }

    /* (re)-configure Timer */
    switch(gWrsConn[peerDeviceId].state)
    {
        case eStateWaitOTAResult:
            /* Start timeout detection */
            wrs_StartEventTimer(peerDeviceId, eEvtTimoutOTAResultRx, 3000U);
            break;

        case eStateDone:
            wrs_StopEventTimer(peerDeviceId);
            wrs_ResetFragmentationData(peerDeviceId, eFrameTypeNone);
            break;

        case eStateIdle:
            /* Note: a timer may have already been restarted at the time the state transition to Idle.
             * We should leave it running for the next transition, wrs_StopEventTimer must not be called.
             */
            wrs_ResetFragmentationData(peerDeviceId, eFrameTypeNone);
            break;

        default:
        {
            ; /* No action required */
        }
        break;
    }
}

/*! *********************************************************************************
* \brief        API to post an event to the WRS controller
*
* \param[in] deviceId       Identifier of BLE device (as defined by SDK BLE API)
* \param[in] ctrlEvent      Event type
* \param[in] arg            Event data
********************************************************************************** */
static void wrs_EventSend
(
    deviceId_t deviceId,
    wrs_EventType_t ctrlEvent,
    const void *arg
)
{
    /* For now, synchronous call to the state machine, no need for a separate task */
    wrs_EventHandler(deviceId, ctrlEvent, arg);
}

/*! *********************************************************************************
* \brief        Timer callback, deviceId and eventType encoded into timer callback parameter
*
* \param[in]    param       Timer event data
********************************************************************************** */
static void wrs_TimerHandler(void *param)
{
    uint32_t eventType;
    union {
        uint32_t v32;
        void *vp;
    } arg = {0U};

    arg.vp = param;
    eventType = arg.v32 & 0xFFU;

    wrs_EventSend((deviceId_t)(arg.v32 >> 8U),
                  (wrs_EventType_t)eventType,
                  NULL);
}

/*! *********************************************************************************
* \brief        API to trigger a timer event
*
* \param[in]    deviceId       Peer identifier
* \param[in]    event          Event type
* \param[in]    timeoutMs      Time to wait until triggering the next event
********************************************************************************** */
static void wrs_StartEventTimer
(
    deviceId_t deviceId,
    wrs_EventType_t event,
    uint32_t timeoutMs
)
{
    union {
        uint32_t v32;
        void *vp;
    } arg = {0U};

    arg.v32 = ((uint32_t)deviceId << 8U) | (uint8_t)event;

    DPRINTF_DBG("start tmr dev=%d %dms\n", deviceId, timeoutMs);
    (void)TM_InstallCallback(gWrsConn[deviceId].timerHandle, wrs_TimerHandler, arg.vp);
    (void)TM_Start(gWrsConn[deviceId].timerHandle, kTimerModeSingleShot | kTimerModeLowPowerTimer, timeoutMs);
}

/*! *********************************************************************************
* \brief        API to stop a timer event
*
* \param[in]    deviceId       Peer identifier
********************************************************************************** */
static void wrs_StopEventTimer(deviceId_t deviceId)
{
    (void)TM_Stop(gWrsConn[deviceId].timerHandle);
    DPRINTF_DBG("Stop TMR\n");
}

/*! *********************************************************************************
* \brief        Reset fragmentation data
*
* \param[in]    deviceId            Peer identifier
* \param[in]    expectedFrameType   Type of frame which should be received next
********************************************************************************** */
static void wrs_ResetFragmentationData
(
    deviceId_t deviceId,
    uint8_t expectedFrameType
)
{
    gWrsConn[deviceId].expectedFrameType = expectedFrameType;
    gWrsConn[deviceId].bufferOffset = 0;
    gWrsConn[deviceId].bufferSize = 0;
    gWrsConn[deviceId].nxtFragmentId = 0;
}

/*! *********************************************************************************
* \brief        Reset procedure data
*
* \param[in]    deviceId            Peer identifier
********************************************************************************** */
static void wrs_ProcDataInit(deviceId_t deviceId)
{
    assert(deviceId <= gAppMaxConnections_c);
    gWrsConn[deviceId].serviceInfo.dataExchangeDone = FALSE;
    gWrsConn[deviceId].localCsDataBuffer = wrs_getLocalEventResultBuffer(deviceId);
    gWrsConn[deviceId].localCsDataBuffer->rawBufferLen = 0;
    gWrsConn[deviceId].remoteCsDataBuffer = wrs_getRemoteEventResultBuffer(deviceId);
    gWrsConn[deviceId].remoteCsDataBuffer->rawBufferLen = 0;
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
    gWrsConn[deviceId].localCsDebugBuffer = wrs_getLocalEventDebugBuffer(deviceId);
    gWrsConn[deviceId].localCsDebugBuffer->subeventCnt = 0;
    gWrsConn[deviceId].localCsDebugBuffer->rawBufferLen = 0;
    gWrsConn[deviceId].remoteCsDebugBuffer = wrs_getRemoteEventDebugBuffer(deviceId);
    gWrsConn[deviceId].remoteCsDebugBuffer->subeventCnt = 0;
    gWrsConn[deviceId].remoteCsDebugBuffer->rawBufferLen = 0;
#endif
}

/*! *********************************************************************************
* \brief        Write fragment to remote CS data buffer
*
* \param[in]    deviceId          Peer identifier
* \param[in]    buffer            Source buffer
* \param[in]    offset            Offset at which to write
* \param[in]    frame_length      Length of data to be written
********************************************************************************** */
static void wrs_SetData
(
    deviceId_t deviceId,
    uint8_t *buffer,
    uint16_t offset,
    uint16_t frame_length
)
{
    csEventResultBuffer_t *remoteCsDataBuffer = gWrsConn[deviceId].remoteCsDataBuffer;

    assert(remoteCsDataBuffer != NULL);
    assert((remoteCsDataBuffer->rawBufferLen + frame_length) < (uint16_t)gCsRawBufferSz_c);
    FLib_MemCpy(remoteCsDataBuffer->buffer.raw + offset, buffer, frame_length);
    remoteCsDataBuffer->rawBufferLen += frame_length;
}

/*! *********************************************************************************
* \brief        Read fragment from local CS data
*
* \param[in]    deviceId          Peer identifier
* \param[in]    buffer            Destination buffer
* \param[in]    offset            Offset from which to read
* \param[in]    frame_length      Length of data to be read
********************************************************************************** */
static void wrs_GetData
(
    deviceId_t deviceId,
    uint8_t *buffer,
    uint32_t offset,
    uint32_t frame_length
)
{
    csEventResultBuffer_t *localCsDataBuffer = gWrsConn[deviceId].localCsDataBuffer;

    assert(localCsDataBuffer != NULL);
    assert(frame_length <= localCsDataBuffer->rawBufferLen - offset);
    FLib_MemCpy(buffer, localCsDataBuffer->buffer.raw + offset, frame_length);
}

#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
/*! *********************************************************************************
* \brief        Write fragment to remote CS data buffer
*
* \param[in]    deviceId          Peer identifier
* \param[in]    buffer            Source buffer
* \param[in]    offset            Offset from which to write
* \param[in]    frame_length      Length of data to be written
********************************************************************************** */
static void wrs_SetDebugData
(
    deviceId_t deviceId,
    uint8_t *buffer,
    uint16_t offset,
    uint16_t frame_length
)
{
    csEventDebugBuffer_t *remoteCsDebugBuffer = gWrsConn[deviceId].remoteCsDebugBuffer;

    assert(remoteCsDebugBuffer != NULL);
    assert((remoteCsDebugBuffer->rawBufferLen + frame_length) < (uint16_t)gCsDebugRawBufferSz_c);
    FLib_MemCpy(remoteCsDebugBuffer->buffer.raw + offset, buffer, frame_length);
    remoteCsDebugBuffer->rawBufferLen += frame_length;
}

/*! *********************************************************************************
* \brief        Read fragment from local CS data
*
* \param[in]    deviceId          Peer identifier
* \param[in]    buffer            Destination buffer
* \param[in]    offset            Offset from which to read
* \param[in]    frame_length      Length of data to be read
********************************************************************************** */
static void wrs_GetDebugData
(
    deviceId_t deviceId,
    uint8_t *buffer,
    uint32_t offset,
    uint32_t frame_length
)
{
    csEventDebugBuffer_t *localCsDebugBuffer = gWrsConn[deviceId].localCsDebugBuffer;

    assert(localCsDebugBuffer != NULL);
    assert(frame_length <= localCsDebugBuffer->rawBufferLen - offset);
    FLib_MemCpy(buffer, localCsDebugBuffer->buffer.raw + offset, frame_length);
}
#endif

/*! *********************************************************************************
* \brief        Return local event result data buffer
*
* \param[in]    deviceId          Peer identifier
********************************************************************************** */
static csEventResultBuffer_t *wrs_getLocalEventResultBuffer(deviceId_t deviceId)
{
    return &gLocalEventResultBuffer;
}

/*! *********************************************************************************
* \brief        Return remote event result data buffer
*
* \param[in]    deviceId          Peer identifier
********************************************************************************** */
static csEventResultBuffer_t *wrs_getRemoteEventResultBuffer(deviceId_t deviceId)
{
    return &gRemoteEventResultBuffer;
}

#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
/*! *********************************************************************************
* \brief        Return local debug data buffer
*
* \param[in]    deviceId          Peer identifier
********************************************************************************** */
static csEventDebugBuffer_t *wrs_getLocalEventDebugBuffer(deviceId_t deviceId)
{
    return &gLocalEventDebugBuffer;
}

/*! *********************************************************************************
* \brief        Return remote debug data buffer
*
* \param[in]    deviceId          Peer identifier
********************************************************************************** */
static csEventDebugBuffer_t *wrs_getRemoteEventDebugBuffer(deviceId_t deviceId)
{
    return &gRemoteEventDebugBuffer;
}
#endif

/*! *********************************************************************************
* \brief        Send a frame to the specified peer
*
* \param[in]    deviceId          Peer identifier
* \param[in]    frameType         Type of frame to be sent
********************************************************************************** */
static wrs_ReturnCode_t wrs_FrameSend
(
    deviceId_t deviceId,
    wrs_FrameType_t frameType
)
{
    uint32_t bufferSize = gWrsConn[deviceId].bufferSize;
    void (* getData)(deviceId_t deviceId, uint8_t *buffer, uint32_t offset, uint32_t frame_length) = NULL;
    wrs_ReturnCode_t ret = eReturnSuccess;
    union {
        uint8_t *frameBuffer;
        wrs_FrameResult_t *frame;
    } tmp = {0U};

    tmp.frameBuffer = gFrameBuffer;

    switch (frameType)
    {
        case eFrameTypeResultData:
            getData = wrs_GetData;
            break;
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
        case eFrameTypeDebugData:
            getData = wrs_GetDebugData;
            break;
#endif
        default:
            ret = eReturnUnknownFrameType;
            break;
    }

    if (ret != eReturnSuccess)
    {
        return ret;
    }

    tmp.frame->frameType = (uint8_t)frameType;
    assert(bufferSize > gWrsConn[deviceId].bufferOffset);
    tmp.frame->remain = (uint16_t)bufferSize - gWrsConn[deviceId].bufferOffset;
    tmp.frame->size = gAttMaxWriteDataSize_d(mAppMtu) - (uint16_t)sizeof(wrs_FrameResult_t);
    tmp.frame->fragmentId = gWrsConn[deviceId].nxtFragmentId;

    if (tmp.frame->remain < tmp.frame->size)
    {
        tmp.frame->size = tmp.frame->remain;
    }

    if (tmp.frame->size == 0U)
    {
        return eReturnEmptyFrame;
    }
    (*getData)(deviceId,
                &tmp.frameBuffer[sizeof(wrs_FrameResult_t)],
                gWrsConn[deviceId].bufferOffset,
                tmp.frame->size);

    DPRINTF_DBG("Tx OTA Msg type=%d sz=%d remain=%d\n", tmp.frame->frameType, tmp.frame->size, tmp.frame->remain);
    /* Advance offset only if the stack was able to send the packet, will be retried on next iteration */

    if (BleApp_SendMessage(deviceId, tmp.frameBuffer, tmp.frame->size + (uint16_t)sizeof(wrs_FrameResult_t)) == gBleSuccess_c)
    {
        DPRINTF_DBG("Msg sent \n");
        gWrsConn[deviceId].bufferOffset += tmp.frame->size; /* move to next fragment */
        gWrsConn[deviceId].nxtFragmentId++;
        return eReturnSuccess;
    }
    else
    {
        return eReturnError;
    }
}
