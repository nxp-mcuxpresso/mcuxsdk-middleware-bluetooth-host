/*
 * Copyright 2022 - 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _WIRELESS_RANGING_MEASUREMENT_H
#define _WIRELESS_RANGING_MEASUREMENT_H

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
#include "isp_measurement.h"

/* BLE Host Stack */
#include "channel_sounding.h"

/************************************************************************************
 *************************************************************************************
 *  Public macros
 *************************************************************************************
 ************************************************************************************/
#define hciLeCsEventResultEventSize_c            (11U)
#define hciLeCsEventResultContinueEventSize_c    (8U)

/************************************************************************************
 *************************************************************************************
 *  Public Types
 *************************************************************************************
 ************************************************************************************/
/*! CS Role of a BLE device */
typedef enum {
    gCsInitiator_c = 0,   /*!< Initiator emits first during an CS step. */
    gCsReflector_c = 1,   /*!< Reflector emits during the second part of an CS step. */
} hciLeCsRole_t;

/*! HCI_LE_CS_Event_Result event */
typedef PACKED_STRUCT hciLeCsEventResultEvent_tag
{
    deviceId_t  deviceId;
    uint8_t     configId;                /*!< CS configuration id */
    uint16_t    startACLConnEvent;       /*!< Starting ACL con event count for this result */
    uint16_t    procedureCounter;        /*!< Procedure count since completion of the Security Start procedure */
    int16_t     frequencyCompensation;   /*!< Frequency compensation value in units of 0.01 ppm (15-bit signed integer) */    
    int8_t      referencePowerLevel;     /*!< Reference Power Level for CS proc (signed dBm) */
    uint8_t     procedureDoneStatus;     /*!< 0=not complete, 1=completed success, 0xFE/0xFF=Error */
    uint8_t     subeventDoneStatus;      /*!< 0x00 = complete success, 0x01 = not complete, 0xFE/0xFF=Error */
    uint8_t     abortReason;             /*!< Abort reason for the CS procedure: b0-3 procedure done status = 0xF; b4-7 subevent done status = 0x0F */
    uint8_t     numAntennaPaths;         /*!< Number of antenna paths for RTP steps */
    uint8_t     numStepsReported;        /*!< Number of steps reported in this event [0x01-0xA0] */
    uint8_t     data[0];                 /*!< Encoded data, can't be structured */
} hciLeCsEventResultEvent_t;

/*! Buffer to queue result event data */
typedef struct csEventResultBuffer_tag {
    uint16_t rawBufferLen;
    hciLeCsEventResultEvent_t *currentResult;
    union {
        hciLeCsEventResultEvent_t firstResult;
        uint8_t raw[gCsRawBufferSz_c];
    } buffer;
} csEventResultBuffer_t;

/*! Buffer to queue debug event data */
typedef struct csEventDebugBuffer_tag {
    uint16_t subeventCnt;
    uint16_t rawBufferLen;
    union {
        event_internal_data_t debug;
        uint8_t raw[gCsDebugRawBufferSz_c];
    } buffer;
} csEventDebugBuffer_t;

typedef struct csAppData_tag {
    bool locked; /* locked for read access */
    uint32_t csStepsReceived;
    uint16_t dbgFlags;
    tof_data_t tof_data;
    mciq_data_t mciq_data;
    cs_mode0_data_t mode0_data;
    cs_data_t csData;
    event_internal_data_t internal_data;

    /* runtime */
    uint16_t mode0BufferOffset;
    uint16_t tofBufferOffset;
    uint16_t mciqBufferOffset;
    uint16_t dbgBufferOffset;

    /* Buffers */
    uint8_t dbgBuffer[1024];
    uint8_t mode0Buffer[gCsMode0Sz_c * CS_STEP_NB_MODE0_MAX * CS_SUBEVT_NB_MAX];
    uint8_t tofBuffer[gCsTofTsSz_c * CS_STEP_NB_MAX_PER_PROC];
    uint8_t mciqBuffer[gCsMciqSz_c * ISP_MAX_NO_ANTENNAS * CS_STEP_NB_MAX_PER_PROC];
} csAppData_t;

/************************************************************************************
 *************************************************************************************
 *  Public memory declarations
 *************************************************************************************
 ************************************************************************************/
/* Measurement parameters */
extern measurement_parameters_t meas_params;

/************************************************************************************
 *************************************************************************************
 *  Public prototypes
 *************************************************************************************
 ************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! Create a new measurement configuration */
bleResult_t measurement_configure(deviceId_t deviceId);
/*! If test mode is configure begin the test procedure, otherwise
    set the default parameters and enable the CS procedure */
bleResult_t measurement_run(deviceId_t deviceId);
/*!  If test mode is configure, end the test procedure */
bleResult_t measurement_test_end();
/*! Initiate the Read Remote Supported Capabilities procedure */
bleResult_t measurement_request_capabilities(deviceId_t peerDeviceId);
/*! Start the LE CS set default settings procedure */
bleResult_t measurement_set_default_settings(deviceId_t peerDeviceId, isp_configuration_range_t *rparams);
/*! Start the security enable procedure */
bleResult_t measurement_start_security(deviceId_t peerDeviceId);
/*! Reset procedure buffers for a new measurement */
void measurement_buffer_init(uint16_t dbgFlags);
/*! Configure pinmux for antenna switching */
void isp_rf_gpo_init(void);
/*! Check if a procedure is in progress */
bool_t measurement_check_proc_in_progress(void);

/*! Set debug IQ buffer, DSB DMA target and send the config via HCI */
bool_t measurement_init
(
    isp_configuration_range_t *rparams
);

/*! Send custom debug config via vendor specific HCI */
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
bool_t measurement_send_vs_debug
(
    isp_configuration_range_t *rparams
);
#endif

/*! Send custom config via vendor specific HCI */
bool_t measurement_send_vs_config
(
    isp_configuration_range_t *rparams
);
/*! Configure pinmux and send custom config via HCI */
void measurement_powerup
(
    isp_configuration_range_t *rparams
);

/*! Convert raw measurement data */
void measurement_populate_response
(
    isp_meas_response_t *response,
    csEventResultBuffer_t *localResultBuffer,
    csEventResultBuffer_t *remoteResultBuffer,
    csEventDebugBuffer_t *localDebugBuffer,
    csEventDebugBuffer_t *remoteDebugBuffer
);
/*! Notify peer of procedure end */
void notifyEndOfEventResult
(
    deviceId_t deviceId,
    uint8_t procedureDoneStatus
);
/*! Process event result events */
void processCsEventResultData
(
    csSubeventResultEvent_t* pEvent,
    uint8_t* pEventData
);
/*! Process event result continue events */
void processCsEventResultContinueData
(
    csSubeventResultContinueEvent_t* pEvent,
    uint8_t *pEventData
);
/*! Process config compelte events */
void processCsConfigCompleteEvent
(
    csConfigCompleteEvent_t *pEvent
);
/*! Process read local supported capabilities complete events */
void processReadLocalSupportedCapabilitiesCompleteEvent
(
    csCommandCompleteEvent_t* pEvent
);
/*! Process read remote supported capabilities complete events */
void processReadRemoteSupportedCapabilitiesCompleteEvent
(
    csReadRemoteSupportedCapabilitiesCompleteEvent_t* pEvent
);
/*! Process set default settings complete events */
void processSetDefaultSettingsCompleteEvent
(
    csCommandCompleteEvent_t* pEvent
);
/*! Process security enable complete event */
void processSecurityEnableCompleteEvent
(
    csSecurityEnableCompleteEvent_t *pEvent
);
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
/*! Process debug event */
void processCsEventResultDebugEvent
(
    csEventResultDebugEvent_t *pEvent
);
#endif

/* From hci_commands.c/.h (temporary) - needed to send vendor specific HCI commands */
bleResult_t Ble_HciSend
    (
        hciPacketType_t packetType,
        void* pPacket,
        uint16_t packetSize
    );

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _WIRELESS_RANGING_MEASUREMENT_H */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
