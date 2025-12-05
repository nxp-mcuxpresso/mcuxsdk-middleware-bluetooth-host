/*
 * Copyright 2022 - 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "fsl_os_abstraction.h"
#include "fsl_port.h"
#include "FunctionLib.h"
#include "fwk_mem_manager.h"

#include "wireless_ranging_measurement.h"
#include "wireless_ranging_service.h"
#include "wireless_ranging.h"
#include "hci_types.h"
#include "channel_sounding.h"

#include "isp_interface.h"
#include "isp_debug_helpers.h"

/************************************************************************************
 *************************************************************************************
 * Macros
 *************************************************************************************
 ************************************************************************************/
/* Ignore main mode repetition steps from HCI results */
/* #define SKIP_MAIN_MODES_REPET */

#define GET16(buf) (((uint16_t)(*(buf))) | ( ((uint16_t)(*((buf) + 1U))) << 8U) )

#define CS_TEST_CONNHDL (0xFFFFU)

/* Proprietary debug flags encoded in RTT_PHY field */
#define CS_DBG_FLG_MASK    (0xFCU)
#define CS_DBG_FLG_SHIFT   (2U)

#define gHciVendorCsDebug_c 0x00AEU

#define HciCommand(opCodeGroup, opCodeCommand)\
    (((uint16_t)(opCodeGroup) & (uint16_t)0x3FU)<<(uint16_t)SHIFT10)|(uint16_t)((opCodeCommand) & 0x3FFU)

#define HciLeCommand(opCodeCommand)\
    (((uint16_t)(gHciLeControllerCommands_c) & (uint16_t)0x3FU)<<(uint16_t)SHIFT10)|(uint16_t)((opCodeCommand) & 0x3FFU)

#define Hci_CommandPacket(pHciCmdPacket, parameterTotalLength)\
            Ble_HciSend(\
                gHciCommandPacket_c,\
                (void*)(pHciCmdPacket),\
                gHciCommandPacketHeaderLength_c + (parameterTotalLength))

#define Hci_AclDataPacket(pHciAclPDataPacket, packetLength)\
            Ble_HciSend(\
                    gHciDataPacket_c,\
                    (void*)(pHciAclPDataPacket),\
                    gHciAclDataPacketHeaderLength_c + (packetLength))

#define CS_MAX_NB_ANTENNAS 4U

#define CS_ANT_BOARD_EVK                  0U   /*!< EVK board, no diversity */
#define CS_ANT_BOARD_ANTDIV_SMA           1U   /*!< X-FR ANTDIV board, EXT antennas (SMA) */
#define CS_ANT_BOARD_ANTDIV_PRINTED       2U   /*!< X-FR ANTDIV board, printed antennas */
#define CS_ANT_BOARD_ANTDIV_4_ANT         3U  /*!< X-FR ANTDIV board, 4 antennas */
#define CS_ANT_BOARD_LOC                  4U   /*!< LOC board, (2 antennas) */

/* Switching logic for KW47 LOC board */
/*  +---------+-------+-------+-------+
 *  | Antenna | Mode  | VDD   | CTRL  |
 *  |   ANTA  | RF_2  |  1    |   0   |
 *  |   ANTB  | RF_3  |  1    |   1   |
 *  |     -   | OFF   |  0    |   0   |
 *  +---------+-------+-------+-------+
 *
 * PTD1 (RF_GPO_4) -> RF switch VDD (power)
 * PTD2 (RF_GPO_5) -> RF switch VCTRL
 */
#define LCL_HAL_LOC_ANT_A_ANT     (0x1U)
#define LCL_HAL_LOC_ANT_B_ANT     (0x3U)
#define LCL_HAL_LOC_ALL_OFF       (0x0U)

/* Switching logic for Antenna diversity board + EVK board */
/*  +---------+-------+-------+-------+-------+
 *  | Antenna | Mode  | CTRL3 | CTRL2 | CTRL1 |
 *  |   ANT20 | RF_2  |   0   |   1   |   0   |
 *  |   ANT30 | RF_3  |   0   |   1   |   1   |
 *  |   EXT1  | RF_1  |   0   |   0   |   1   |
 *  |   EXT2  | RF_4  |   0   |   0   |   0   |
 *  |     -   | OFF   |   1   |   1   |   0   |
 *  +---------+-------+-------+-------+-------+
 *
 * PTA18 (RF_GPO_0) -> RF switch CTRL1
 * PTA19 (RF_GPO_1) -> RF switch CTRL2
 * PTA20 (RF_GPO_2) -> RF switch CTRL3
 */
#define LCL_HAL_ANTDIV_EXT_1_ANT      (0x1U)
#define LCL_HAL_ANTDIV_EXT_2_ANT      (0x0U)
#define LCL_HAL_ANTDIV_ANT_20_ANT     (0x2U)
#define LCL_HAL_ANTDIV_ANT_30_ANT     (0x3U)
#define LCL_HAL_ANTDIV_ALL_OFF        (0x6U)

/************************************************************************************
 *************************************************************************************
 * Public memory declarations
 *************************************************************************************
 ************************************************************************************/
/* CS debug buffer */
#define CSBUFF_SIZE 2048

uint32_t m_csbuffer[CSBUFF_SIZE];


/* Store deviceId of current HCI procedure */
static deviceId_t gCurrentDeviceId = gInvalidDeviceId_c;

/* Buffer to hold local and peer measurement data */
csAppData_t gLocalAppDataBuffer;
csAppData_t gRemoteAppDataBuffer;
csAppData_t *localAppDataBuffer = &gLocalAppDataBuffer;
csAppData_t *remoteAppDataBuffer = &gRemoteAppDataBuffer;
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
debug_data_t gCsDebugBuffer;
#endif

/* Buffers to hold event result data and debug data */
csEventResultBuffer_t gCsResultBuffer;
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
csEventDebugBuffer_t gCsDebugRawBuffer;
#endif

/* Measurement parameters */
measurement_parameters_t meas_params;

const uint8_t cs_ant_perm_n_ap[24][4] = {
    {0,1,2,3}, /* A1,A2,A3,A4 */
    {1,0,2,3}, /* A2,A1,A3,A4 */
    {0,2,1,3}, /* A1,A3,A2,A4 */
    {2,0,1,3}, /* A3,A1,A2,A4 */
    {2,1,0,3}, /* A3,A2,A1,A4 */
    {1,2,0,3}, /* A2,A3,A1,A4 */
    {0,1,3,2}, /* A1,A2,A4,A3 */
    {1,0,3,2}, /* A2,A1,A4,A3 */
    {0,3,1,2}, /* A1,A4,A2,A3 */
    {3,0,1,2}, /* A4,A1,A2,A3 */
    {3,1,0,2}, /* A4,A2,A1,A3 */
    {1,3,0,2}, /* A2,A4,A1,A3 */
    {0,3,2,1}, /* A1,A4,A3,A2 */
    {3,0,2,1}, /* A4,A1,A3,A2 */
    {0,2,3,1}, /* A1,A3,A4,A2 */
    {2,0,3,1}, /* A3,A1,A4,A2 */
    {2,3,0,1}, /* A3,A4,A1,A2 */
    {3,2,0,1}, /* A4,A3,A1,A2 */
    {3,1,2,0}, /* A4,A2,A3,A1 */
    {1,3,2,0}, /* A2,A4,A3,A1 */
    {3,2,1,0}, /* A4,A3,A2,A1 */
    {2,3,1,0}, /* A3,A4,A2,A1 */
    {2,1,3,0}, /* A3,A2,A4,A1 */
    {1,2,3,0}, /* A2,A3,A4,A1 */
};

/*===== Antenna diversity conversion LUTs: from antenna index to lant_lut_gpio[3:0] */
/* Size of the conversion table is based on lant_lut_gpio size + one dummy termination */

/* No diversity board */
static const uint8_t cs_ant_idx_to_GPIO_None[CS_MAX_NB_ANTENNAS] =  {0U, 0U, 0U, 0U};
/* Antenna diversity board */
static const uint8_t cs_ant_idx_to_GPIO_ANTDIV_SMA[CS_MAX_NB_ANTENNAS] =  {LCL_HAL_ANTDIV_EXT_1_ANT , LCL_HAL_ANTDIV_EXT_2_ANT, LCL_HAL_ANTDIV_ALL_OFF, LCL_HAL_ANTDIV_ALL_OFF};
static const uint8_t cs_ant_idx_to_GPIO_ANTDIV_ANT[CS_MAX_NB_ANTENNAS] =  {LCL_HAL_ANTDIV_ANT_20_ANT , LCL_HAL_ANTDIV_ANT_30_ANT, LCL_HAL_ANTDIV_ALL_OFF, LCL_HAL_ANTDIV_ALL_OFF};
static const uint8_t cs_ant_idx_to_GPIO_ANTDIV_4ANT[CS_MAX_NB_ANTENNAS] =  {LCL_HAL_ANTDIV_EXT_1_ANT , LCL_HAL_ANTDIV_EXT_2_ANT, LCL_HAL_ANTDIV_ANT_20_ANT , LCL_HAL_ANTDIV_ANT_30_ANT};
/* LOC board (2 antennas) */
static const uint8_t cs_ant_idx_to_GPIO_LOC[CS_MAX_NB_ANTENNAS] =  {LCL_HAL_LOC_ANT_A_ANT , LCL_HAL_LOC_ANT_B_ANT, LCL_HAL_LOC_ALL_OFF, LCL_HAL_LOC_ALL_OFF};

/* PCT Rotation calibration parameters */
#if defined (BOARD_LOCALIZATION_REVISION_SUPPORT) && (BOARD_LOCALIZATION_REVISION_SUPPORT == 1U)
/* 0cm compensation for LOC boards */
static uint8_t maPctRotationParams[4U * CS_MAX_NB_ANTENNAS] = {0U};
#else
/* 15cm compensation for EVK boards */
static uint8_t maPctRotationParams[4U * CS_MAX_NB_ANTENNAS] = {67U, 0U, 0U, 0U,
                                                               67U, 0U, 0U, 0U,
                                                               67U, 0U, 0U, 0U,
                                                               67U, 0U, 0U, 0U};
#endif
/************************************************************************************
 *************************************************************************************
 * Public Types
 *************************************************************************************
 ************************************************************************************/
/*! HCI Vendor Specific config */
typedef PACKED_STRUCT
{
    uint8_t  debugFlags;
    uint8_t  debugFlagsLL;
    uint16_t uiDebugBufferSize; /* in bytes */
    uint32_t ulDebugBufferAddress;
    uint8_t  ucForceCustomConfig;   /* Internal use, keep it set to zero */
} hciVsCsDebug_t;

/*! HCI Vendor Specific config */
typedef PACKED_STRUCT
{
    uint32_t parametersPresence;
    uint8_t parametersLength;
    /* in the HCI message, variable list of parameters follow */
} hciVsCsConfig_t;

/************************************************************************************
 *************************************************************************************
 * Private Prototypes
 *************************************************************************************
 ************************************************************************************/
static void hciCsStoreBytesInIqBuffer
(
    csAppData_t *appData,
    uint8_t *source,
    int nbBytes
);

static void hciCsStoreBytesInMode0Buffer
(
    csAppData_t *appData,
    uint8_t *source,
    int nbBytes
);

static void hciCsStoreBytesInTofBuffer
(
    csAppData_t *appData,
    uint8_t *source,
    int nbBytes
);

static void hciLeProcessEventResultData
(
    uint8_t *pEventData,
    int nbSteps,
    csEventResultBuffer_t *pEventResultBuffer
);

static void measurement_uncompress_response
(
    uint8_t **srcResultBuffer,
    csAppData_t *dstAppBuffer,
    bool skip
);

#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
static void cs_debug_compute_num_iq_per_t_pm
(
    debug_data_t *db_buf_p,
    bool avg_off,
    uint8_t rtt_phy,
    uint8_t t_pm
);

static void measurement_uncompress_debug
(
    csEventDebugBuffer_t *srcDebugBuffer,
    csAppData_t *dstAppBuffer
);
#endif

/* Computes T_SW used by a CS procedure */
static uint8_t BleApp_ComputeTsw(void);
/************************************************************************************
 *************************************************************************************
 * Public Functions
 *************************************************************************************
 ************************************************************************************/
/*! *********************************************************************************
 * \brief        Process subevent result events.
 *
 * \param[in]    pEvent          Event Information
 * \param[in]    pEventData      Event Data
 ********************************************************************************** */
void processCsEventResultData
(
    csSubeventResultEvent_t* pEvent,
    uint8_t* pEventData
)
{
    csEventResultBuffer_t *eventResultBuffer = &gCsResultBuffer;
    uint8_t nbSteps = pEvent->numStepsReported;

    DPRINTF_DBG("CS Result: numStepsReported=%d devId=%d connEvent=%d, status=0x%X/0x%X/0x%X\n", nbSteps,
                 pEvent->deviceId,
                 pEvent->startACLConnEvent,
                 pEvent->subeventDoneStatus,
                 pEvent->procedureDoneStatus,
                 pEvent->abortReason);

    assert(localAppDataBuffer->locked == FALSE);
    if (eventResultBuffer->rawBufferLen == 0)
    {
        /* Sanity check for start of a new buffer */
        assert(localAppDataBuffer->csStepsReceived == 0);
        assert(offsetof(hciLeCsEventResultEvent_t, numStepsReported) == offsetof(csSubeventResultEvent_t, numStepsReported));
    }
#ifdef ENABLE_SANITY_SUBEVT_SEPARATOR
    else
    {
        /* Sanity separator */
        FLib_MemSet(eventResultBuffer->buffer.raw + eventResultBuffer->rawBufferLen, 0xAA, SANITY_SUBEVT_SEPARATOR_SZ);
        eventResultBuffer->rawBufferLen += SANITY_SUBEVT_SEPARATOR_SZ;
    }
#endif

    /* Store pointer to current event */
    eventResultBuffer->currentResult = (hciLeCsEventResultEvent_t *)(void *)(eventResultBuffer->buffer.raw + eventResultBuffer->rawBufferLen);
    /* Copy message Header */
    FLib_MemCpy(eventResultBuffer->currentResult, pEvent, sizeof(hciLeCsEventResultEvent_t));
    eventResultBuffer->rawBufferLen += sizeof(hciLeCsEventResultEvent_t);

    hciLeProcessEventResultData(pEventData, nbSteps, eventResultBuffer);
}

/*! *********************************************************************************
 * \brief        Process subevent result continue events.
 *
 * \param[in]    pEvent          Event Information
 * \param[in]    pEventData      Event Data
 ********************************************************************************** */
void processCsEventResultContinueData
(
    csSubeventResultContinueEvent_t* pEvent,
    uint8_t* pEventData
)
{
    csEventResultBuffer_t *eventResultBuffer = &gCsResultBuffer;
    assert(localAppDataBuffer->locked == FALSE);
    assert(localAppDataBuffer->csStepsReceived != 0);
    assert(eventResultBuffer->rawBufferLen != 0);

    DPRINTF_DBG("CS ResultContinue: numStepsReported=%d devId=0x%04X status=0x%X/0x%X\n",
                pEvent->numStepsReported,
                pEvent->deviceId,
                pEvent->subeventDoneStatus,
                pEvent->procedureDoneStatus);

    /* Accumulate total number of steps in subevent result header */
    /* We loose granularity of individual ResultContinue Events, but it's not needed later on */
    hciLeCsEventResultEvent_t *dataBufferHeader = (hciLeCsEventResultEvent_t *)eventResultBuffer->currentResult;
    assert(dataBufferHeader->deviceId == pEvent->deviceId);
    assert(dataBufferHeader->configId == pEvent->configId);
    dataBufferHeader->numStepsReported += pEvent->numStepsReported;
    dataBufferHeader->subeventDoneStatus = pEvent->subeventDoneStatus;
    dataBufferHeader->procedureDoneStatus = pEvent->procedureDoneStatus;

    hciLeProcessEventResultData(pEventData, pEvent->numStepsReported, eventResultBuffer);
}

/*! *********************************************************************************
 * \brief        Notify peer of procedure end.
 *
 * \param[in]    deviceId             Identifier of peer to be notified.
 * \param[in]    procedureDoneStatus  Indicates end of CS procedure.
 *                                    In case debug is active, application should wait
 *                                    for HCI debug result information before notifying
 *                                    the peer.
 ********************************************************************************** */
void notifyEndOfEventResult
(
    deviceId_t deviceId,
    uint8_t procedureDoneStatus
)
{
    /* Last steps for current subevent has been received */
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
    /* The app only stores the debug data associated to the first subevent - other debug buffers are ignored */
    if (((localAppDataBuffer->dbgFlags & ISP_DBG_FLG_DBG_INFO) != 0) && (gCsDebugRawBuffer.subeventCnt == 0))
    {
        DPRINTF_DBG("CS Result: wait for HCI Debug Result msg\n");
    }
    else if (procedureDoneStatus == 0x01)
    {
        DPRINTF_DBG("CS Result: wait for more subevents\n");
    }
    else
    {
        DPRINTF_DBG("CS Result: completed\n");
        wrs_NotifyEventResult(deviceId, &gCsResultBuffer, &gCsDebugRawBuffer);
        localAppDataBuffer->locked = TRUE;
    }
#else
    /* No debug event is expected, inform wireless ranging service */
    if (procedureDoneStatus == 0x00)
    {
        DPRINTF_DBG("CS Result: completed\n");
        wrs_NotifyEventResult(deviceId, &gCsResultBuffer, NULL);
        localAppDataBuffer->locked = TRUE;
    }
#endif
}

/*! *********************************************************************************
 * \brief        Process config complete events.
 *
 * \param[in]    pEvent      Event to be processed.
 ********************************************************************************** */
void processCsConfigCompleteEvent
(
    csConfigCompleteEvent_t *pEvent
)
{
    /* Save measurement config locally (as if it was set from CLI)*/
    isp_configuration_range_t *config = &meas_params.cfg;
    config->role = pEvent->role;
    config->main_mode_type = pEvent->mainModeType;
    config->sub_mode_type = pEvent->subModeType;
    config->main_mode_min = pEvent->mainModeMinSteps;
    config->main_mode_max = pEvent->mainModeMaxSteps;
    config->main_mode_repeat = pEvent->mainModeRepetition;
    config->mode0_nb = pEvent->mode0Steps;
    config->rtt_type = pEvent->RTTTypes;
    config->rtt_phy = ((pEvent->csSyncPhy & (~CS_DBG_FLG_MASK)) == 1)? 0:1; /* 0x01 or 0x02 in HCI */
    config->test_mode = 0;
    FLib_MemCpy(config->ch_map, pEvent->channelMap, CS_CH_MAP_LEN);
    config->ch_map_repeat = pEvent->channelMapRepetition;
    config->t_ip1 = pEvent->TIP1time;
    config->t_ip2 = pEvent->TIP2time;
    config->t_fcs = pEvent->TFCStime;
    config->t_pm = pEvent->TPMtime;

    /* Retrieve debug information from RT PHY field */
    config->debug = pEvent->csSyncPhy >> CS_DBG_FLG_SHIFT;

    wrs_StartProcedure(pEvent->deviceId);
}

/*! Process read local supported capabilities complete events */
void processReadLocalSupportedCapabilitiesCompleteEvent
(
    csCommandCompleteEvent_t* pEvent
)
{
    meas_params.cfg.t_sw_local = pEvent->eventData.csReadLocalSupportedCapabilities.TSWtimeSupported;
}

/*! *********************************************************************************
 * \brief        Process read remote supported capabilities events.
 *
 * \param[in]    pEvent      Event to be processed.
 ********************************************************************************** */
void processReadRemoteSupportedCapabilitiesCompleteEvent
(
    csReadRemoteSupportedCapabilitiesCompleteEvent_t* pEvent
)
{
    gRangeSettings->t_sw_remote = pEvent->TSWtimeSupported;
    wrs_StartSecurity(pEvent->deviceId);
}

/*! *********************************************************************************
 * \brief        Process Set Default Settings command complete event.
 *
 * \param[in]    pEvent      Event to be processed.
 ********************************************************************************** */
void processSetDefaultSettingsCompleteEvent
(
    csCommandCompleteEvent_t* pEvent
)
{
    wrs_SetDefaultSettingsComplete(pEvent->deviceId);
}

/*! *********************************************************************************
 * \brief        Process security enable complete events.
 *
 * \param[in]    pEvent      Event to be processed.
 ********************************************************************************** */
void processSecurityEnableCompleteEvent
(
    csSecurityEnableCompleteEvent_t* pEvent
)
{
    wrs_SecurityComplete(pEvent->deviceId);
}

#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
/*! *********************************************************************************
 * \brief        Process debug events.
 *
 * \param[in]    pEvent      Event to be processed.
 ********************************************************************************** */
void processCsEventResultDebugEvent
(
    csEventResultDebugEvent_t *pEvent
)
{
    DPRINTF_DBG("CS ResultDebug: len=%d bufSz=%d offset=%d\n",
                pEvent->payloadLength,
                pEvent->bufferLength,
                pEvent->offset);

    /* The app only stores the debug data associated to the first subevent - other debug buffers are ignored */
    if (gCsDebugRawBuffer.subeventCnt == 0) {
        assert(localAppDataBuffer->locked == FALSE);
        assert(pEvent->bufferLength <= sizeof(gCsDebugRawBuffer.buffer));
        assert((gCsDebugRawBuffer.rawBufferLen + pEvent->payloadLength) < sizeof(gCsDebugRawBuffer.buffer));
        FLib_MemCpy(gCsDebugRawBuffer.buffer.raw + gCsDebugRawBuffer.rawBufferLen, pEvent->pData, pEvent->payloadLength);
        gCsDebugRawBuffer.rawBufferLen += pEvent->payloadLength;
        /* If Last debug event fragment has been received, inform controller */
        if ((pEvent->offset + pEvent->payloadLength) >= pEvent->bufferLength)
        {
            gCsDebugRawBuffer.subeventCnt++;
            gCsDebugBuffer.step_nb = gCsResultBuffer.buffer.firstResult.numStepsReported;
            if (gCsResultBuffer.buffer.firstResult.procedureDoneStatus != 0x01)
            {
                deviceId_t deviceId = gCsResultBuffer.buffer.firstResult.deviceId;
                /* Inform Ranging Service that measurement is complete */
                DPRINTF_DBG("CS Result + Dbg: completed, deviceId %d\n", deviceId);
                wrs_NotifyEventResult(deviceId, &gCsResultBuffer, &gCsDebugRawBuffer);
            }
        }
    }
}
#endif

/*! *********************************************************************************
 * \brief        Set debug IQ buffer, DSB DMA target and send the config via HCI.
 *
 * \param[in]    rparams      Pointer to request parameters.
 ********************************************************************************** */
bool_t measurement_init
(
    isp_configuration_range_t *rparams
)
{
    meas_params.is_valid = false;

    meas_params.cfg = *rparams;

    if(rparams->tx_pwr < XCVR_TX_PWR_MIN || rparams->tx_pwr > XCVR_TX_PWR_MAX)
        return false;

    if (rparams->ch_nb > gHciCsMaxStepsPerSubevent_c)
        return false;

    meas_params.is_valid = true;

    /* Set debug IQ buffer, DSB DMA target */
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
    FLib_MemSet(&gCsDebugBuffer, 0 ,sizeof(gCsDebugBuffer));
    if (rparams->debug & ISP_DBG_IQ_DETAILS)
    {
        gCsDebugBuffer.iq = (uint8_t *)m_csbuffer;
        gCsDebugBuffer.iqBufLength = (uint16_t)CSBUFF_SIZE;
        assert(gCsDebugBuffer.iqBufLength != 0);
        FLib_MemSet((void*)gCsDebugBuffer.iq, 0xFE, gCsDebugBuffer.iqBufLength);
    }
    else
    {
        gCsDebugBuffer.iq = NULL;
        gCsDebugBuffer.iqBufLength = 0;
    }
    gCsDebugBuffer.step_nb = 0;
    gCsDebugBuffer.n_pairs = 1;

    /* Guess number of IQ per step based on debug config - need to be in sync with LL implementation */
    cs_debug_compute_num_iq_per_t_pm(&gCsDebugBuffer,
                                       ((rparams->debug & ISP_DBG_IQ_AVG_OFF) != 0),
                                       meas_params.cfg.rtt_phy, meas_params.cfg.t_pm);
#endif

    /* Send custom config via VS HCI */
    measurement_send_vs_config(rparams);
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
    measurement_send_vs_debug(rparams);
#endif

    return true;
}

/*! *********************************************************************************
 * \brief        Reset procedure buffers for a new measurement.
 *
 * \param[in]    dbgFlags      Debug flags.
 ********************************************************************************** */
void measurement_buffer_init
(
    uint16_t dbgFlags
)
{
    /* Reset procedure buffers for a new measurement */
    FLib_MemSet(&gLocalAppDataBuffer, 0, sizeof(csAppData_t));
    FLib_MemSet(&gRemoteAppDataBuffer, 0, sizeof(csAppData_t));
    FLib_MemSet(&gCsResultBuffer, 0, sizeof(gCsResultBuffer));
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
    FLib_MemSet(&gCsDebugRawBuffer, 0, sizeof(gCsDebugRawBuffer));
    localAppDataBuffer->dbgFlags = dbgFlags;
#else
    localAppDataBuffer->dbgFlags = 0;
#endif
    localAppDataBuffer->locked = FALSE;
}

/*! *********************************************************************************
 * \brief        Create a new measurement configuration.
 *
 * \param[in]    deviceId      Peer identifier.
 ********************************************************************************** */
bleResult_t measurement_configure(deviceId_t deviceId)
{
    gCsCreateConfigCommandParams_t createConfigParams;
    assert(deviceId != CS_TEST_DEVICEID);

    isp_configuration_range_t *config = &meas_params.cfg;
    assert(config->test_mode == 0);
    gCurrentDeviceId = deviceId;

    createConfigParams.configId = WR_HCI_CONFIG_ID;
    createConfigParams.createContext = 1U; /* Create CS configuration in both local and remote Controller */
    createConfigParams.mainModeType = config->main_mode_type;
    createConfigParams.subModeType = config->sub_mode_type;
    createConfigParams.mainModeMinSteps = config->main_mode_min;
    createConfigParams.mainModeMaxSteps = config->main_mode_max;
    createConfigParams.mainModeRepetition = config->main_mode_repeat;
    createConfigParams.mode0Steps = config->mode0_nb;
    createConfigParams.role = config->role;
    createConfigParams.RTTTypes = (rttTypes_t)config->rtt_type;
    createConfigParams.csSyncPhy = (config->rtt_phy == 0) ? 1U:2U;
    FLib_MemCpy(createConfigParams.channelMap, config->ch_map, CS_CH_MAP_LEN);
    createConfigParams.channelMapRepetition = config->ch_map_repeat;
    createConfigParams.channelSelectionType = (hoppingAlgorithmTypes_t)config->ch_sel_algo;
    createConfigParams.ch3cShape = (userShape_t)config->ch_sel_shape;
    createConfigParams.ch3cJump = config->ch_sel_jump;

    return CS_CreateConfig(deviceId, &createConfigParams);
}

/*! *********************************************************************************
 * \brief        Initiate the Read Remote Supported Capabilities procedure
 *
 * \param[in]    peerDeviceId      Peer identifier.
 ********************************************************************************** */
bleResult_t measurement_request_capabilities(deviceId_t peerDeviceId)
{
    bleResult_t result = gBleSuccess_c;

    assert(peerDeviceId != CS_TEST_DEVICEID);
    gCurrentDeviceId = peerDeviceId;
    /* Read peer capabilities on the csReadLocalSupportedCapabilities event */
    result = CS_ReadLocalSupportedCapabilities();

    if (result == gBleSuccess_c)
    {
        result = CS_ReadRemoteSupportedCapabilities(peerDeviceId);
    }

    return result;
}

/*! *********************************************************************************
 * \brief        Initiate the security enable procedure.
 *
 * \param[in]    peerDeviceId      Peer identifier.
 ********************************************************************************** */
bleResult_t measurement_start_security(deviceId_t peerDeviceId)
{
    bleResult_t result = gBleSuccess_c;

    assert(peerDeviceId != CS_TEST_DEVICEID);
    gCurrentDeviceId = peerDeviceId;

    result = CS_SecurityEnable(peerDeviceId);

    return result;
}

/*! *********************************************************************************
 * \brief        Start the LE CS set default settings procedure.
 *
 * \param[in]    peerDeviceId      Peer identifier.
 ********************************************************************************** */
bleResult_t measurement_set_default_settings(deviceId_t peerDeviceId, isp_configuration_range_t *rparams)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t     syncAntennaSelection;
    int8_t      maxTxPowerLevel = 10; /* 10 dBm */

    syncAntennaSelection = (NULL != rparams) ? rparams->ant_CS_SYNC : 0xFF;  /* AntId to be used for RTT. */

    assert(syncAntennaSelection != 0);
    assert(peerDeviceId != CS_TEST_DEVICEID);
    gCurrentDeviceId = peerDeviceId;

    result = CS_SetDefaultSettings(peerDeviceId,
                                   (csRoleType)((uint8_t)gEnableCsInitiator_c | (uint8_t)gEnableCsReflector_c),
                                   syncAntennaSelection,
                                   maxTxPowerLevel);

    return result;
}

/*! *********************************************************************************
 * \brief        Configure pinmux and send config via HCI.
 *
 * \param[in]    rparams      Pointer to request parameters.
 ********************************************************************************** */
void measurement_powerup
(
    isp_configuration_range_t *rparams
)
{
    /* Send custom config via VS HCI */
    measurement_send_vs_config(rparams);
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
    measurement_send_vs_debug(rparams);
#endif
}

/*! *********************************************************************************
 * \brief        If test mode is configure begin the test procedure, otherwise
 *               set the default parameters and enable the CS procedure
 *
 * \param[in]    deviceId      Peer identifier
 ********************************************************************************** */
bleResult_t measurement_run(deviceId_t deviceId)
{
    bleResult_t result;

    if (meas_params.cfg.test_mode)
    {
        /* Test Mode */
        gCsTestCommandParams_t *pCommand = MEM_BufferAlloc(sizeof(gCsTestCommandParams_t) + 255U);

        if (pCommand != NULL)
        {
            uint8_t *pOvrdData = &pCommand->overrideParametersData[0];

            pCommand->mainModeType = meas_params.cfg.main_mode_type;
            pCommand->subModeType = meas_params.cfg.sub_mode_type;
            pCommand->mainModeRepetition = meas_params.cfg.main_mode_repeat;
            pCommand->mode0Steps = meas_params.cfg.mode0_nb;              /* Nb mode0 to be included at Mode0Interval */
            pCommand->role = meas_params.cfg.role;                        /* 0=initiator, 1=reflector */
            pCommand->RTTTypes = (rttTypes_t)meas_params.cfg.rtt_type;     /* 0=coarse, 1=frac, 2=frac+soundSeq */
            pCommand->CSSyncPhy = (meas_params.cfg.rtt_phy == 0) ? 1U:2U; /* 1=1Mbps, 2=2Mbps */
            pCommand->CSSYNCAntennaSelection = meas_params.cfg.ant_CS_SYNC;  /* AntId to be used for RTT. */
            /* If subevent lenght has been specified explicitly */
            if (meas_params.cfg.subevent_len != WR_CS_SUBEVT_DURATION_US_MAX)
            {
                pCommand->subeventLen[0] = meas_params.cfg.subevent_len & 0x000000FF;
                pCommand->subeventLen[1] = (meas_params.cfg.subevent_len & 0x0000FF00) >> 8U;
                pCommand->subeventLen[2] = (meas_params.cfg.subevent_len & 0x00FF0000) >> 16U;
                pCommand->subeventInterval = MIN(5000U, (meas_params.cfg.subevent_len + meas_params.cfg.subevent_len / 2U) / 625U);
            }
            else
            {
                /* as per spec, subvent_interval=0 means not fragmentation */
                pCommand->subeventLen[0] = 0xFFU;
                pCommand->subeventLen[1] = 0xFFU;
                pCommand->subeventLen[2] = 0xFFU;
                pCommand->subeventInterval = 0U;
            }
            pCommand->maxNumSubevents = 0U;
            pCommand->transmitPowerLevel = meas_params.cfg.tx_pwr;  /* Tx power (dBm) - adjust to nearest */
            pCommand->TIP1time = meas_params.cfg.t_ip1;     /* T_IP1 in us */
            pCommand->TIP2time = meas_params.cfg.t_ip2;     /* T_IP2 in us */
            pCommand->TFCStime = meas_params.cfg.t_fcs;     /* T_FCS in us */
            pCommand->TPMtime = meas_params.cfg.t_pm;      /* T_PM in us */
            pCommand->TSWtime = meas_params.cfg.ant_cfg_index == 0 ? 0 : meas_params.cfg.t_sw_local;      /* T_SW in us */
            pCommand->channelMapRepetition = meas_params.cfg.ch_map_repeat;/* channel map repetition */
            pCommand->toneAntennaConfig = meas_params.cfg.ant_cfg_index;/* Antenna configuration index */
            pCommand->DRBG_Nonce = 0; /* not used */
            pCommand->SNRCtrlInitiator = gSNRControlNotApplied_c;
            pCommand->SNRCtrlReflector = gSNRControlNotApplied_c;

            /* build overrides */
            pCommand->overrideConfig = gCSTmOvrdConfigSubmodesMask | gCSTmOvrdConfigTPmExtMask | gCSTmOvrdConfigAntpermMask;
            if (meas_params.cfg.ch_isrand == FALSE)
            {
                pCommand->overrideConfig |= gCSTmOvrdConfigChannelsMask;
                *pOvrdData++ = meas_params.cfg.ch_nb;
                FLib_MemCpy(pOvrdData, meas_params.cfg.ch_list, meas_params.cfg.ch_nb);
                pOvrdData += meas_params.cfg.ch_nb;
            }
            else
            {
                FLib_MemCpy(pOvrdData, meas_params.cfg.ch_map, CS_CH_MAP_LEN);
                pOvrdData += CS_CH_MAP_LEN;
                *pOvrdData++ = meas_params.cfg.ch_sel_algo;
                *pOvrdData++ = meas_params.cfg.ch_sel_shape;
                *pOvrdData++ = meas_params.cfg.ch_sel_jump;
            }
            *pOvrdData++ = meas_params.cfg.main_mode_min;
            *pOvrdData++ = meas_params.cfg.t_pm_tone_ext;
            *pOvrdData++ = meas_params.cfg.ant_perm_index;

            /* If initiator AA is specified as zero, we do not use CSSyncAA override and DRBG randomization will be used */
            if (!FLib_MemCmpToVal(meas_params.cfg.initiator_AA, 0U, 4U))
            {
                pCommand->overrideConfig |= gCSTmOvrdConfigCsSyncAaMask;
                FLib_MemCpy(pOvrdData, meas_params.cfg.initiator_AA, 4U);
                pOvrdData += 4U;
                FLib_MemCpy(pOvrdData, meas_params.cfg.reflector_AA, 4U);
                pOvrdData += 4U;
            }

            pCommand->overrideParametersLength = pOvrdData - &pCommand->overrideParametersData[0];

            gCurrentDeviceId = deviceId;
            result = CS_Test(pCommand);
            MEM_BufferFree(pCommand);
        }
        else
        {
            result = gBleOutOfMemory_c;
        }
    }
    else
    {
        gCsSetProcedureParamsCommandParams_t params;

        params.configId = WR_HCI_CONFIG_ID;
        params.maxProcedureDuration = 0xFFFF;
        if (gRangeSettings->proc_interval != 0) {
            /* Use value provided by application */
            params.minProcedureInterval = gRangeSettings->proc_interval;
        } else {
            uint32_t proc_interval;
            /* Programme procedure interval as short as possible, based on verbosity/embedded algo*/
            proc_interval = WR_CS_PROC_DURATION_MS_MAX;
            if ((gLocalSettings->verbosity & (uint8_t)eVerbosityMeasurementData) == 0) {
                proc_interval += WR_POSTPROC_VERB_DURATION_MS_MIN;
            } else {
                proc_interval += WR_POSTPROC_VERB_DURATION_MS_MAX;
            }
            if ((gLocalSettings->mciq_algo_flags & eMciqAlgoEmbedCDE) != 0) {
                proc_interval += WR_ALGO_CDE_DURATION_MS;
            }
            if ((gLocalSettings->mciq_algo_flags & eMciqAlgoEmbedRADE) != 0) {
                proc_interval += WR_ALGO_RADE_DURATION_MS;
            }

            /* Convert ms to connection intervals */
            proc_interval = 1 + proc_interval*1000/(bleInfo.connInterval*1250);

#ifdef SDK_OS_FREE_RTOS
            if (proc_interval < WR_FREERTOS_PROC_INTERVAL_MIN)
            {
                proc_interval = WR_FREERTOS_PROC_INTERVAL_MIN;
            }
#endif
            params.minProcedureInterval = (uint16_t) proc_interval;
        }
        params.maxProcedureInterval = params.minProcedureInterval;
        params.maxProcCount = meas_params.cfg.max_proc_count;
        params.minSubeventLen = meas_params.cfg.subevent_len & 0xFFFFFFU;
        params.maxSubeventLen = params.minSubeventLen;
        params.toneAntennaConfigSelection = meas_params.cfg.ant_cfg_index;
        params.phys = 1U; /* Should reflect ACL PHY */
        params.txPwrDelta = 0; /* power delta in dBm */
        params.preferredPeerAntenna = 3U; /* Use first ordered antenna element */
        params.SNRCtrlInitiator = gSNRControlNotApplied_c;
        params.SNRCtrlReflector = gSNRControlNotApplied_c;
        gCurrentDeviceId = deviceId;
        result = CS_SetProcedureParameters(deviceId, &params);

        if (result == gBleSuccess_c) {
            result = CS_ProcedureEnable(gCurrentDeviceId, WR_HCI_CONFIG_ID, TRUE);
        }
    }
    return result;
}

/*! *********************************************************************************
 * \brief        If test mode is configure, end the test procedure
 *
 ********************************************************************************** */
bleResult_t measurement_test_end()
{
    bleResult_t result = gBleInvalidState_c;

    if (meas_params.cfg.test_mode)
    {
        result = CS_TestEnd();
    }
    return result;
}

/*! *********************************************************************************
 * \brief        Convert raw measurement data
 *
 * \param[in]    response               Response data
 * \param[in]    localResultBuffer      Buffer with local event result data
 * \param[in]    remoteResultBuffer     Buffer with remote event result data
 * \param[in]    localDebugBuffer       Buffer with local debug data
 * \param[in]    remoteDebugBuffer      Buffer with remote debug data
 ********************************************************************************** */
void measurement_populate_response
(
    isp_meas_response_t *response,
    csEventResultBuffer_t *localResultBuffer,
    csEventResultBuffer_t *remoteResultBuffer,
    csEventDebugBuffer_t *localDebugBuffer,
    csEventDebugBuffer_t *remoteDebugBuffer
)
{
    csAppData_t *csDataBuffer0, *csDataBuffer1;
    uint32_t subeventIdx = 0;
    bool skipSubevent;
    uint32_t abortReasonProcedure = 0;
    uint32_t procedureCounter;
    uint8_t *localResultPtr = (uint8_t *)&localResultBuffer->buffer.raw;
    uint8_t *remoteResultPtr = (uint8_t *)&remoteResultBuffer->buffer.raw;
    hciLeCsEventResultEvent_t *localEvent_p;
    hciLeCsEventResultEvent_t *remoteEvent_p;
    /* Remote data may not be combined depending on the configuration (server side or test mode) */
    bool combineRemote = (remoteResultBuffer->rawBufferLen != 0);

    localAppDataBuffer->csData.step_nb = 0;
    remoteAppDataBuffer->csData.step_nb = 0;
    localAppDataBuffer->csData.subevt_nb = 0;
    remoteAppDataBuffer->csData.subevt_nb = 0;
    localAppDataBuffer->csData.startAclCnt = localResultBuffer->buffer.firstResult.startACLConnEvent;
    remoteAppDataBuffer->csData.startAclCnt = remoteResultBuffer->buffer.firstResult.startACLConnEvent;
    localAppDataBuffer->mciq_data.n_ap = localResultBuffer->buffer.firstResult.numAntennaPaths;
    remoteAppDataBuffer->mciq_data.n_ap = localResultBuffer->buffer.firstResult.numAntennaPaths;
    procedureCounter = localResultBuffer->buffer.firstResult.procedureCounter;
    (void)procedureCounter;
    do {
        DPRINTF_DBG("== Subevent %d ==\n", subeventIdx);

        localEvent_p = (hciLeCsEventResultEvent_t *)(void *)localResultPtr;
        remoteEvent_p = (hciLeCsEventResultEvent_t *)(void *)remoteResultPtr;
        /* Skip the current subevent if one of the devices did not receive it. Do not skip local data if remote data is ignored. */
        skipSubevent = combineRemote ? ((localEvent_p->numStepsReported == 0) || (remoteEvent_p->numStepsReported == 0)) : false;
        DPRINTF_DBG("= Local : abort %d, nbsteps %d\n", localEvent_p->abortReason, localEvent_p->numStepsReported);
        assert(localEvent_p->procedureCounter == procedureCounter);
        measurement_uncompress_response(&localResultPtr, localAppDataBuffer, skipSubevent);
        /* Accumulates status for the whole procedure (last failure will be reported) */
        if (localEvent_p->abortReason != 0)
        {
            abortReasonProcedure = localEvent_p->abortReason;
        }
        if (combineRemote)
        {
            DPRINTF_DBG("= Remote: abort %d, nbsteps %d\n", remoteEvent_p->abortReason, remoteEvent_p->numStepsReported);
            assert(remoteEvent_p->procedureCounter == procedureCounter);
            measurement_uncompress_response(&remoteResultPtr, remoteAppDataBuffer, skipSubevent);
            assert(localAppDataBuffer->csData.step_nb == remoteAppDataBuffer->csData.step_nb);
            if (remoteEvent_p->abortReason != 0)
            {
                abortReasonProcedure = remoteEvent_p->abortReason;
            }
            assert(localEvent_p->procedureDoneStatus == remoteEvent_p->procedureDoneStatus);
        }
        subeventIdx++;
    } while (localEvent_p->procedureDoneStatus == 1);

#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
    measurement_uncompress_debug(localDebugBuffer, localAppDataBuffer);
    if (combineRemote)
    {
        measurement_uncompress_debug(remoteDebugBuffer, remoteAppDataBuffer);
    }
#endif

    /* Populate response struct */
    FLib_MemSet(response, 0, sizeof(isp_meas_response_t));
    response->mparams = &meas_params;
    response->cs_data = &localAppDataBuffer->csData;
    /* Populate timing information to be used by algorithm */
    response->mparams->cfg.t_sw  = BleApp_ComputeTsw();
    response->mparams->cfg.connInterval = bleInfo.connInterval;
    /* Report an error if we didn't get any step after combining both device results */
    response->cs_data->status = (localAppDataBuffer->csData.step_nb == 0) ? abortReasonProcedure:0x00;

    /* Reorder data so that index 0 represents initiator, index 1 represents reflector whatever the device role is */
    if (meas_params.cfg.role == gCsInitiator_c)
    {
        csDataBuffer0 = localAppDataBuffer;
        csDataBuffer1 = remoteAppDataBuffer;
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
        response->debug_data[0] = &gCsDebugBuffer;
#else
        response->debug_data[0] = NULL;
#endif
        response->debug_data[1] = NULL;
        FLib_MemCpy(response->cs_data->subevtRefPowerLevelInit, localAppDataBuffer->csData.subevtRefPowerLevelInit, gCsSubeventMax_c);
        FLib_MemCpy(response->cs_data->subevtRefPowerLevelRefl, remoteAppDataBuffer->csData.subevtRefPowerLevelInit, gCsSubeventMax_c);
    }
    else
    {
        csDataBuffer1 = localAppDataBuffer;
        csDataBuffer0 = remoteAppDataBuffer;
        response->debug_data[0] = NULL;
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
        response->debug_data[1] = &gCsDebugBuffer;
#else
         response->debug_data[1] = NULL;
#endif
        FLib_MemCpy(response->cs_data->subevtRefPowerLevelRefl, localAppDataBuffer->csData.subevtRefPowerLevelInit, gCsSubeventMax_c);
        FLib_MemCpy(response->cs_data->subevtRefPowerLevelInit, remoteAppDataBuffer->csData.subevtRefPowerLevelInit, gCsSubeventMax_c);
    }

    if (localAppDataBuffer->csData.mode0_nb != 0)
    {
        /* Mode0 */
        response->mode0_data[0].mode0_buffer = csDataBuffer0->mode0Buffer;
        response->mode0_data[1].mode0_buffer = csDataBuffer1->mode0Buffer;
    }
    if (localAppDataBuffer->mciq_data.nbSteps != 0)
    {
        /* MCIQ */
        response->mciq_data[0] = csDataBuffer0->mciq_data;
        response->mciq_data[0].iq = (uint8_t *)(csDataBuffer0->mciqBuffer);
        response->mciq_data[1] = csDataBuffer1->mciq_data;
        response->mciq_data[1].iq = (uint8_t *)(csDataBuffer1->mciqBuffer);
    }
    if (localAppDataBuffer->tof_data.nbSteps != 0)
    {
        /* ToF */
        response->tof_data[0] = csDataBuffer0->tof_data;
        response->tof_data[0].ts = csDataBuffer0->tofBuffer;
        response->tof_data[1] = csDataBuffer1->tof_data;
        response->tof_data[1].ts = csDataBuffer1->tofBuffer;
    }
    response->internal_data[0] = &csDataBuffer0->internal_data;
    response->internal_data[1] = &csDataBuffer1->internal_data;
}

#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
/*! *********************************************************************************
 * \brief        Send vendor specific debug config via HCI
 *
 * \param[in]    rparams               Pointer to request parameters
 ********************************************************************************** */
bool_t measurement_send_vs_debug
(
    isp_configuration_range_t *rparams
)
{
    uint8_t pHciPacket[sizeof(hciVsCsDebug_t) + gHciCommandPacketHeaderLength_c];
    uint16_t opcode = HciCommand(gHciVendorSpecificDebugCommands_c, gHciVendorCsDebug_c);
    uint32_t DebugBufferAddress = (uint32_t)m_csbuffer;

    /* Set HCI opcode */
    FLib_MemCpy((void*)pHciPacket, (const void*)&opcode, 2U);
    /* Set HCI parameter length */
    pHciPacket[2] = (uint8_t)sizeof(hciVsCsDebug_t);
    /* Fill msg */
    pHciPacket[3] = rparams->debug;
    pHciPacket[4] = 0x3U; /* Allow invalid CS channels and don't check link encryption */
    pHciPacket[5] = (uint8_t)CSBUFF_SIZE;
    pHciPacket[6] = (uint8_t)((uint32_t)CSBUFF_SIZE >> 8U);
    pHciPacket[7] = (uint8_t)DebugBufferAddress;
    pHciPacket[8] = (uint8_t)(DebugBufferAddress >> 8U);
    pHciPacket[9] = (uint8_t)(DebugBufferAddress >> 16U);
    pHciPacket[10] = (uint8_t)(DebugBufferAddress >> 24U);

    pHciPacket[11] = 0; /* ucForceCustomConfig = 0 */

    /* Send HCI command */
    return Hci_CommandPacket( pHciPacket, (uint16_t)pHciPacket[2]);
}
#endif

#define CS_VS_CONFIG_PARAM1_SIZE (23U)
/*! *********************************************************************************
 * \brief        Send vendor specific config via HCI
 *
 * \param[in]    rparams               Pointer to request parameters
 ********************************************************************************** */
bool_t measurement_send_vs_config
(
    isp_configuration_range_t *rparams
)
{
    const uint8_t *ant2gpio_p;
    uint32_t paramsPresence = (gCSParamAntennaConfigPresent_c | gCSParamPctPhaseRotationPresent_c);
    uint8_t  paramDataLength = CS_VS_CONFIG_PARAM1_SIZE;
    uint8_t aAppData[CS_VS_CONFIG_PARAM1_SIZE] = {0U};
    uint8_t numAntennas = 2U; /* Number of antennas (only 2 antennas on reference designs) */

    switch (rparams->ant_type)
    {
    case CS_ANT_BOARD_EVK:
      ant2gpio_p = cs_ant_idx_to_GPIO_None;
      break;
    case CS_ANT_BOARD_ANTDIV_SMA:
      ant2gpio_p = cs_ant_idx_to_GPIO_ANTDIV_SMA;
      break;
    case CS_ANT_BOARD_ANTDIV_PRINTED:
      ant2gpio_p = cs_ant_idx_to_GPIO_ANTDIV_ANT;
      break;
    case CS_ANT_BOARD_ANTDIV_4_ANT:
      ant2gpio_p = cs_ant_idx_to_GPIO_ANTDIV_4ANT;
      numAntennas = 4U;
      break;
    case CS_ANT_BOARD_LOC:
      ant2gpio_p = cs_ant_idx_to_GPIO_LOC;
      break;
    default:
      ant2gpio_p = NULL;
      break;
    }

    if (ant2gpio_p != NULL)
    {
        /* Fill message data */
        aAppData[0U] = 2U; /* Default antenna switch time */
        aAppData[1U] = 4U; /* Number of antenna paths */
        aAppData[2U] = numAntennas; /* Number of antennas */
        FLib_MemCpy((void *)(&aAppData[3U]), ant2gpio_p, CS_MAX_NB_ANTENNAS);
        /* PCT rotation calibration */
        FLib_MemCpy((void *)(&aAppData[3U + CS_MAX_NB_ANTENNAS]), maPctRotationParams, (4U * CS_MAX_NB_ANTENNAS));
        return (CS_ConfigVendorCommand(paramsPresence, paramDataLength, aAppData) == gBleSuccess_c);
    }
    else
    {
        return FALSE;
    }
}

/*! *********************************************************************************
 * \brief        Check if a procedure is in progress
 ********************************************************************************** */
bool_t measurement_check_proc_in_progress(void)
{
    return localAppDataBuffer->locked;
}

/************************************************************************************
 *************************************************************************************
 * Private Functions
 *************************************************************************************
 ************************************************************************************/

/*! *********************************************************************************
 * \brief        Uncompress combined subevents HCI/OTA data (HCI-like format) for a given device
 *
 * \param[in]    srcResultBuffer    Pointer to uncompressed data
 * \param[in]    dstAppBuffer       Pointer to destination buffer for uncompressed data
 * \param[in]    skipSubevent       srcResultBuffer will be consumed, but not stored in dstAppBuffer
 ********************************************************************************** */
static void measurement_uncompress_response
(
    uint8_t **srcResultBuffer,
    csAppData_t *dstAppBuffer,
    bool skipSubevent
)
{
    int numSteps;
    int step;
#ifdef SKIP_MAIN_MODES_REPET
    uint32_t parsedMainModeNb = CS_MAIN_MODE_REPEAT_MAX;
#endif
    uint8_t *eventData, *stepData;
    hciLeCsEventResultEvent_t *event_p;

    event_p = (hciLeCsEventResultEvent_t *)*srcResultBuffer;
    numSteps = event_p->numStepsReported;
    eventData = event_p->data;
    step = dstAppBuffer->csData.step_nb;

    for (int i = 0; i < numSteps; i ++)
    {
        bool_t tofPresent = FALSE;
        bool_t mciqPresent = FALSE;
        uint8_t mode = *eventData++;
        uint8_t channel = *eventData++;
        uint8_t stepDataLength = *eventData++;
        assert(stepDataLength <= gCsDataSizeMax_c);
        stepData = eventData;

        DPRINTF_DBG("step %d, mode %d, chan %d\n", i, mode, channel);
        if (skipSubevent)
        {
            /* Just consume source buffer to the end of the subevent */
            stepData += stepDataLength;
            eventData = stepData;
            continue;
        }

        dstAppBuffer->csData.channelMap[step] = channel;
        dstAppBuffer->csData.modeMap[step] = mode;
        switch(mode)
        {
            case 0:
            {
#ifdef SKIP_MAIN_MODES_REPET
                /* Reset main mode repetition counter when reaching and of mode0 phase*/
                if (dstAppBuffer->csData.mode0_nb >= meas_params.cfg.mode0_nb)
                {
                  parsedMainModeNb = 0;
                }
#endif
                /* Pkt record */
                stepData++; /* Skip Packet_AA_Quality */
                uint16_t cfo = 0;
                hciCsStoreBytesInMode0Buffer(dstAppBuffer, stepData, 1); /* RSSI */
                stepData ++;
                stepData ++; /* Skip Packet_Antenna */
                if (stepDataLength > 3)
                {
                    /* This is initiator data */
                    FLib_MemCpy(&cfo, stepData, 2); /* Measured_Freq_Offset on 16 bits */
                    stepData += 2;
                    /* mode0 IQs are only available on initiator */
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
                    gCsDebugBuffer.samples_nb[step] = gCsDebugBuffer.samplesPerStepMode0;
#endif
                }
                else
                {
                    cfo = 0xFFFF;
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
                    gCsDebugBuffer.samples_nb[step] = 0;
#endif
                }
                DPRINTF_DBG("CFO:%d\n", (int16_t)cfo);
                hciCsStoreBytesInMode0Buffer(dstAppBuffer, (uint8_t *)&cfo, 2);
                dstAppBuffer->csData.mode0_nb ++;
            }
            break;

            case 1:
            {
                /* ToF record */
                tofPresent = TRUE;
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
                gCsDebugBuffer.samples_nb[step] = 0;
#endif
            }
            break;

            case 2:
            {
#ifdef SKIP_MAIN_MODES_REPET
                if (parsedMainModeNb < meas_params.cfg.main_mode_repeat) {
                    DPRINTF_DBG("skip rep channel=%d, prev=%d\n", channel, dstAppBuffer->csData.channelMap[step-1]);
                    stepData += stepDataLength;
                    eventData = stepData;
                    parsedMainModeNb++;
                    continue;
                }
#endif
                /* Tone record */
                mciqPresent = TRUE;
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
                gCsDebugBuffer.samples_nb[step] = gCsDebugBuffer.samplesPerStep;
#endif
            }
            break;

            case 3:
            {
                /* ToF+Tone record */
                tofPresent = TRUE;
                mciqPresent = TRUE;
#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
                gCsDebugBuffer.samples_nb[step] = gCsDebugBuffer.samplesPerStep;
#endif
            }
            break;

            default:
            {
                /* mode not yet implemented, skip data */
                assert(false);
                stepData += stepDataLength;
            }
            break;
        }

        if (tofPresent)
        {
            uint32_t ts_diff = 0;
            int16_t ts_diff_hci = 0;
            /* ToF record */
            uint8_t quality = *stepData++; /* Packet_AA_Quality, store for later */
            hciCsStoreBytesInTofBuffer(dstAppBuffer, stepData, CS_NADM_SIZE); /* Packet_NADM */
            stepData ++;
            hciCsStoreBytesInTofBuffer(dstAppBuffer, stepData, CS_RSSI_SIZE); /* Packet_RSSI */
            stepData ++;
            memcpy(&ts_diff_hci, stepData, 2); /* Time Diff signed Q16, 2 bytes */
            DPRINTF_DBG("TS:%02X%02X(%d) Q:%d\n", stepData[1], stepData[0], ts_diff_hci, quality);
            stepData += 2;
            stepData ++; /* Packet_Antenna, ignored */

            /* Combine TS_DIFF & quality on 24 bits and store in local buffer */
            ts_diff = ts_diff_hci/2; /* HCI reports half ns, application expects ns in Tof Buffer */
            ts_diff &= 0x00FFFFU;
            ts_diff |= (quality&0x0FU)<<20;
            hciCsStoreBytesInTofBuffer(dstAppBuffer, (uint8_t *)&ts_diff, CS_TS_SIZE);
            dstAppBuffer->tof_data.nbSteps ++;
        }

        if (mciqPresent)
        {
            /* Tone record */
            uint8_t antPermIndex = *stepData++; /* Antenna_Permutation_Index */
            assert(antPermIndex < 24);
            const uint8_t *antIndex_p = &cs_ant_perm_n_ap[antPermIndex][0];
            uint32_t iq_dec[ISP_MAX_NO_ANTENNAS];
            uint8_t tqi[ISP_MAX_NO_ANTENNAS];

#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
            gCsDebugBuffer.samples_nb[step] *= (1U + event_p->numAntennaPaths);
#endif
            /* Num_Antenna_Path + 1 are reported by the firmware, but discard last one */
            /* Re-order per antenna path index */
            for (int i = 0; i < event_p->numAntennaPaths; i++)
            {
                int antIdx = antIndex_p[i];
                uint32_t iq = stepData[2]<<16 | stepData[1]<<8 | stepData[0];

                /* Swap I and Q as application expects I as MSB and Q as LSB (opposite from Tone_PCT[k]) */
                iq_dec[antIdx] = ((iq & 0xfffU) << 12) | ((iq >> 12) & 0xfffU);
                stepData += 3;
                tqi[antIdx] = *stepData;
                stepData += 1;
            }

            for (int i = 0; i < event_p->numAntennaPaths; i++)
            {
                hciCsStoreBytesInIqBuffer(dstAppBuffer, (uint8_t *)&iq_dec[i], 3);
                hciCsStoreBytesInIqBuffer(dstAppBuffer, &tqi[i], 1);
                DPRINTF_DBG("%d/%d/%d I:%d Q:%d TQI:%d\n", step, antPermIndex, antIndex_p[i], ((iq_dec[i] >> 12) & 0xfffU), (iq_dec[i] & 0xfffU), tqi[i]);
            }
            /* Skip last IQ data (n_ap+1) */
            stepData += 4;
            dstAppBuffer->mciq_data.nbSteps ++;
        }
        assert(stepData - eventData == stepDataLength);
        eventData = stepData;
        step ++;
    }
    dstAppBuffer->csData.step_nb = step;
    dstAppBuffer->csData.subevtStopIdx[dstAppBuffer->csData.subevt_nb] = step;
    dstAppBuffer->csData.subevtConnEvent[dstAppBuffer->csData.subevt_nb] = event_p->startACLConnEvent - dstAppBuffer->csData.startAclCnt;
    dstAppBuffer->csData.subevtRefPowerLevelInit[dstAppBuffer->csData.subevt_nb] = event_p->referencePowerLevel;
    dstAppBuffer->csData.subevt_nb++;

#ifdef ENABLE_SANITY_SUBEVT_SEPARATOR
    /* Check/skip sanity zone (except for last subevent) */
    if (event_p->procedureDoneStatus == 0x1)
    {
        assert(eventData[0] == 0xAA);
        assert(eventData[1] == 0xAA);
        eventData += SANITY_SUBEVT_SEPARATOR_SZ;
    }
#endif

    /* Advance result buffer pointer to the end of the processed section */
    *srcResultBuffer = eventData;
}

#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
/*! *********************************************************************************
 * \brief        Uncompress Debug OTA data for a given device
 *
 * \param[in]    srcDebugBuffer     Pointer to uncompressed data
 * \param[in]    dstAppBuffer       Pointer to destination buffer for uncompressed data
 ********************************************************************************** */
static void measurement_uncompress_debug
(
    csEventDebugBuffer_t *srcDebugBuffer,
    csAppData_t *dstAppBuffer
)
{
    event_internal_data_t *debug_p = &srcDebugBuffer->buffer.debug;

    DPRINTF_DBG("DBUG BUFFER len=%d\n", srcDebugBuffer->rawBufferLen);
    FLib_MemCpy(dstAppBuffer->dbgBuffer, srcDebugBuffer->buffer.raw, srcDebugBuffer->rawBufferLen);

    /* Get length of rtt_internal and mciq_internal sections */
    dstAppBuffer->internal_data.rtt_internal_nb = debug_p->rtt_internal_nb;
    dstAppBuffer->internal_data.mciq_internal_nb = debug_p->mciq_internal_nb;
    /* Update pointers to debug buffer */
    dstAppBuffer->internal_data.event_internal = (event_internal_t *)(dstAppBuffer->dbgBuffer + 2*sizeof(uint16_t));
    dstAppBuffer->internal_data.rtt_internal = (rtt_internal_t *)(((uint8_t*)dstAppBuffer->internal_data.event_internal) + sizeof(event_internal_t));
    dstAppBuffer->internal_data.mciq_internal = (mciq_internal_t *)(((uint8_t*)dstAppBuffer->internal_data.rtt_internal) + dstAppBuffer->internal_data.rtt_internal_nb*sizeof(rtt_internal_t));
}
#endif

/*! *********************************************************************************
 * \brief        Process subevent result event data
 *
 * \param[in]    pEventData               Pointer to event data
 * \param[in]    nbSteps                  Number of procedure steps
 * \param[in]    pEventResultBuffer       Buffer to copy event data to
 ********************************************************************************** */
static void hciLeProcessEventResultData
(
    uint8_t *pEventData,
    int nbSteps,
    csEventResultBuffer_t *pEventResultBuffer
)
{
    int totalDataLength;

    /* We have to compute total data length for the HCI event as it's not provided by the HCI message nor by the Stack callback */
    totalDataLength = 0;
    uint8_t *stepDataLenPtr = pEventData + 2 * sizeof(uint8_t); /* Skip mode, channel */
    for (int step = 0; step < nbSteps; step ++) {
        assert(*stepDataLenPtr <= gCsDataSizeMax_c);
        totalDataLength += *(stepDataLenPtr + totalDataLength) + sizeof(uint8_t) * 3; /* also account for mode, channel, data_length fields */
    }

    /* Concatenate CsEventResultEvent data at rawBufferLen */
    FLib_MemCpy(pEventResultBuffer->buffer.raw + pEventResultBuffer->rawBufferLen, pEventData, totalDataLength);
    pEventResultBuffer->rawBufferLen += totalDataLength;
    localAppDataBuffer->csStepsReceived += nbSteps;
    assert(localAppDataBuffer->csStepsReceived <= CS_STEP_NB_MAX_PER_PROC);
    assert(pEventResultBuffer->rawBufferLen < sizeof(pEventResultBuffer->buffer.raw));
}

/*! *********************************************************************************
 * \brief        Store IQ information in application buffer.
 *
 * \param[in]    appData           Pointer to application data
 * \param[in]    source            Pointer to source of IQ data
 * \param[in]    nbBytes           Number of bytes to be copied
 ********************************************************************************** */
static void hciCsStoreBytesInIqBuffer
(
    csAppData_t *appData,
    uint8_t *source,
    int nbBytes
)
{
    FLib_MemCpy(appData->mciqBuffer + appData->mciqBufferOffset, source, nbBytes);
    appData->mciqBufferOffset += nbBytes;
}

/*! *********************************************************************************
 * \brief        Store data generated in mode0 in application buffer.
 *
 * \param[in]    appData           Pointer to application data
 * \param[in]    source            Pointer to source of mode 0 data
 * \param[in]    nbBytes           Number of bytes to be copied
 ********************************************************************************** */
static void hciCsStoreBytesInMode0Buffer
(
    csAppData_t *appData,
    uint8_t *source,
    int nbBytes
)
{
    assert(appData->mode0BufferOffset + nbBytes <= sizeof(appData->mode0Buffer));
    FLib_MemCpy(appData->mode0Buffer + appData->mode0BufferOffset, source, nbBytes);
    appData->mode0BufferOffset += nbBytes;
}

/*! *********************************************************************************
 * \brief        Store data to send in application buffer.
 *
 * \param[in]    appData           Pointer to application data
 * \param[in]    source            Pointer to source of data
 * \param[in]    nbBytes           Number of bytes to be copied
 ********************************************************************************** */
static void hciCsStoreBytesInTofBuffer
(
    csAppData_t *appData,
    uint8_t *source,
    int nbBytes
)
{
    FLib_MemCpy(appData->tofBuffer + appData->tofBufferOffset, source, nbBytes);
    appData->tofBufferOffset += nbBytes;
}

#if defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
/*! *********************************************************************************
 * \brief        Computes the number of IQ per step, based on what is implemented in CS firmware
 *
 * \param[in]    db_buf_p       Pointer to debug data
 * \param[in]    avg_off        Averaging is enabled/disabled
 * \param[in]    rtt_phy        RTT Phy value
 * \param[in]    t_pm           T_PM time value
 ********************************************************************************** */
static void cs_debug_compute_num_iq_per_t_pm
(
    debug_data_t *db_buf_p,
    bool avg_off,
    uint8_t rtt_phy,
    uint8_t t_pm
)
{
    if (!avg_off)
    {
        /* IQ averaging is enabled, so we get 4 IQ samples / step */
        db_buf_p->samplesPerStepMode0 = gCsIQNbPerStep_Avg;
        db_buf_p->samplesPerStep = gCsIQNbPerStep_Avg;
    }
    else
    {
        /* IQ averaging is turned off, so we get raw IQs */
        if (t_pm == 40)
        {
            db_buf_p->samplesPerStep = 32*4;
        }
        else if (t_pm == 20)
        {
            db_buf_p->samplesPerStep = 16*4;
        }
        else /* (t_pm == 10) */
        {
            db_buf_p->samplesPerStep = 8*4;
        }
        if (rtt_phy == 1) db_buf_p->samplesPerStep *= 2;
        db_buf_p->samplesPerStepMode0 = db_buf_p->samplesPerStep;
    }
}
#endif

/*! *********************************************************************************
 * \brief        Function which computes the T_SW used by a CS procedure.
 *
 ********************************************************************************** */
static uint8_t BleApp_ComputeTsw(void)
{
    uint8_t t_sw = 0U;

    switch (meas_params.cfg.ant_cfg_index)
    {
        /* For N:1 it's the initator's T_SW */
        case 1U:
        case 2U:
        case 3U:
        {
            t_sw = (meas_params.cfg.role == gCsRoleInitiator_c) ? meas_params.cfg.t_sw_local : meas_params.cfg.t_sw_remote;
        }
        break;

        /* For 1:N it's the reflector's T_SW */
        case 4U:
        case 5U:
        case 6U:
        {
            t_sw = (meas_params.cfg.role == gCsRoleReflector_c) ? meas_params.cfg.t_sw_local : meas_params.cfg.t_sw_remote;
        }
        break;

        /* For 2:2 it's the larger T_SW value from the initiator and the reflector */
        case 7U:
        {
            t_sw = (meas_params.cfg.t_sw_local >= meas_params.cfg.t_sw_remote) ? meas_params.cfg.t_sw_local : meas_params.cfg.t_sw_remote;
        }
        break;

        default:
        {
            ; /* leave t_sw with the initialization value of 0 */
        }
        break;
    }

    return t_sw;
}