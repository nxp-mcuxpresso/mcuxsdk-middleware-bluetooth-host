/*! *********************************************************************************
* Copyright 2022-2025 NXP
*
* \file app_localization.c
*
* This is a source file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "EmbeddedTypes.h"
#include "fsl_component_timer_manager.h"
#include "ble_general.h"
#include "hci_types.h"
#include "fsl_port.h"
#include "fsl_component_mem_manager.h"
#include "sensors.h"
#include "fwk_platform_lcl.h"
#include "app_localization.h"
#include "channel_sounding.h"
#include "gatt_client_interface.h"
#include "gap_interface.h"
#include "app_conn.h"

#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
#include "app_localization_algo.h"
#endif

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#include "ranging_interface.h"

#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
#include "ranging_client_interface.h"
#endif /* gRasRREQ_d */
#else
#include "digital_key_interface.h"

#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
#include "btcs_server_interface.h"
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */

#if defined (gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
#include "btcs_client_interface.h"
#endif /* gAppBtcsClient_d */
#endif /* gAppRasDataTransfer_d */

#ifdef LCE_KW47_MCXW72
#include "app_lce_init.h"
#endif /* LCE_KW47_MCXW72 */

#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
#include "pts_test_vectors.h"
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
/* Time between temperature measurement trigger and reading the temperature value */
#ifndef gRefreshTime_c
#define gRefreshTime_c                (25U)     /* milliseconds */
#endif

/* Refresh the temperature value periodically */
#ifndef gTemperaturePollingInterval_c
#define gTemperaturePollingInterval_c (500U)    /* milliseconds */
#endif

#define CS_ANT_BOARD_EVK                   0U   /*!< EVK board, no diversity */
#define CS_ANT_BOARD_ANTDIV_SMA            1U   /*!< X-FR ANTDIV board, EXT antennas (SMA) */
#define CS_ANT_BOARD_ANTDIV_PRINTED        2U   /*!< X-FR ANTDIV board, printed antennas */
#define CS_ANT_BOARD_LOC_SMA               3U   /*!< LOC board, EXT antennas (SMA) */
#define CS_ANT_BOARD_LOC_PRINTED           4U   /*!< LOC board, printed antennas */
#define CS_ANT_BOARD_LOC_PRINTED_DUMMY_SMA 5U  /*!< LOC board, printed antennas, SMA1 connected to 50ohm termination */
#define CS_ANT_BOARD_ANTDIV_4_ANT          6U  /*!< X-FR ANTDIV board, 4 antennas */
                
#define CS_NO_ANT_TYPES                    7U

/* Switching logic for KW47 LOC board */
/*  +---------+-------+-------+-------+
 *  | Antenna | Mode  | VDD   | CTRL  |
 *  |   ANTA  | RF_1  |  1    |   0   |
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
#define LCL_HAL_LOC_UNAVAILABLE   (0xFFU)

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
#define LCL_HAL_ANTDIV_EXT_1_ANT        (0x1U)
#define LCL_HAL_ANTDIV_EXT_2_ANT        (0x0U)
#define LCL_HAL_ANTDIV_ANT_20_ANT       (0x2U)
#define LCL_HAL_ANTDIV_ANT_30_ANT       (0x3U)
#define LCL_HAL_ANTDIV_ALL_OFF          (0x6U)

/*
    CS_VendorConfig parameters length:
        Antenna config: 3U + APP_LOCALIZATION_MAX_NO_ANTENNAS
        PCT rotation: 4U * APP_LOCALIZATION_MAX_NO_ANTENNAS
*/
#define CS_CONFIG_VENDOR_PARAM_LENGTH   (3U + 5U * APP_LOCALIZATION_MAX_NO_ANTENNAS)

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
/*! HCI Vendor Specific config */
typedef PACKED_STRUCT
{
    uint32_t parametersPresence;
    uint8_t parametersLength;
    /* in the HCI message, variable list of parameters follow */
} hciVsCsConfig_t;

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static pfAppCsCallback_t mpfAppCsCallback = NULL;

#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1)
/* Application display callback */
static pfAppDisplayResult_t mpfDisplayResultCallback;
/* Check for RTT Sounding  support to parse mode 1 data */
static bool_t rttSoundingSupported = FALSE;
/* Number of times the algorithm has run for the current procedure sequence */
static uint16_t maAlgoRunCount[gAppMaxConnections_c] = {0U};
#endif /* gAppRunAlgo_d */

/* Local measurement data */
static rasMeasurementData_t mResultData[gAppMaxConnections_c];

static const uint8_t cs_ant_idx_to_GPIO[CS_NO_ANT_TYPES][APP_LOCALIZATION_MAX_NO_ANTENNAS] = {
    {0U, 0U, 0U, 0U}, /* 0 = None */
    {LCL_HAL_ANTDIV_EXT_1_ANT , LCL_HAL_ANTDIV_EXT_2_ANT, LCL_HAL_ANTDIV_ALL_OFF, LCL_HAL_ANTDIV_ALL_OFF}, /* 1 = ANTDIV_SMA */
    {LCL_HAL_ANTDIV_ANT_20_ANT , LCL_HAL_ANTDIV_ANT_30_ANT, LCL_HAL_ANTDIV_ALL_OFF, LCL_HAL_ANTDIV_ALL_OFF},  /* 2 = ANTDIV_PRINTED */
    {LCL_HAL_LOC_UNAVAILABLE, LCL_HAL_LOC_UNAVAILABLE, LCL_HAL_LOC_UNAVAILABLE, LCL_HAL_LOC_UNAVAILABLE}, /* 3 = LOC_SMA */
    {LCL_HAL_LOC_ANT_A_ANT , LCL_HAL_LOC_ANT_B_ANT, LCL_HAL_ANTDIV_ALL_OFF, LCL_HAL_ANTDIV_ALL_OFF}, /* 4 = LOC_PRINTED */
    {LCL_HAL_LOC_UNAVAILABLE, LCL_HAL_LOC_UNAVAILABLE, LCL_HAL_LOC_UNAVAILABLE, LCL_HAL_LOC_UNAVAILABLE}, /* 5 = LOC_PRINTED_DUMMY_SMA */
    {LCL_HAL_ANTDIV_EXT_1_ANT , LCL_HAL_ANTDIV_EXT_2_ANT, LCL_HAL_ANTDIV_ANT_20_ANT , LCL_HAL_ANTDIV_ANT_30_ANT}, /* 6 = ANTDIV_4_ANT */
};

/* Current state for a specific peer */
static appLocalization_State_t maAppLclState[gAppMaxConnections_c];

/* Number of CS procedures executed */
static uint16_t maCsProcCount[gAppMaxConnections_c];

/*
    Cached remote capabilities for each bonded peer
    These capabilities are NOT stored in non-volatile memory
    The purpose of this array is to save capabilities to be transferred to the target during Handover
*/
static csReadRemoteSupportedCapabilitiesCompleteEvent_t* mpCachedRemoteCaps[gMaxBondedDevices_c];

#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
static uint16_t maPsmChannels[gAppMaxConnections_c];
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */

/* Timer to update temperature for the NBU */
static TIMER_MANAGER_HANDLE_DEFINE(mTemperatureTimerId);

/* PCT Rotation calibration parameters */
#if defined (BOARD_LOCALIZATION_REVISION_SUPPORT) && (BOARD_LOCALIZATION_REVISION_SUPPORT == 1U)
/* 0cm compensation for LOC boards */
static uint8_t maPctRotationParams[4U * APP_LOCALIZATION_MAX_NO_ANTENNAS] = {0U};
#else
/* 15cm compensation for EVK boards */
static uint8_t maPctRotationParams[4U * APP_LOCALIZATION_MAX_NO_ANTENNAS] = {67U, 0U, 0U, 0U,
                                                                             67U, 0U, 0U, 0U,
                                                                             67U, 0U, 0U, 0U,
                                                                             67U, 0U, 0U, 0U};
#endif
/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static bleResult_t csMetaEventCallback
(
    csMetaEvent_t* pPacket
);

static bleResult_t csEventCmdCompleteCallback
(
    csCommandCompleteEvent_t* pPacket
);

static bleResult_t csEventCmdStatusCallback
(
    csCommandStatusEvent_t* pPacket
);

static bleResult_t processCsResultsEvent
(
    csSubeventResultEvent_t* pEvent
);

static bleResult_t processCsResultsContinueEvent
(
    csSubeventResultContinueEvent_t* pEvent
);

static void AppLocalizationError
(
    deviceId_t deviceId,
    appLocalizationError_t error
);

static deviceId_t GetDeviceIdInState
(
    appLocalization_State_t state
);

static void AppLocalization_CSMetaEventCallback
(
    void* pCsMetaEvent
);

static void AppLocalization_CSCmdCompleteCallback
(
    void* pCsCmdCompleteEvent
);

static void AppLocalization_CSCmdStatusCallback
(
    void* pCsCmdStatusEvent
);

static bleResult_t processEventResultData
(
    deviceId_t deviceId,
    uint8_t nbSteps,
    uint8_t subEventStatus,
    uint8_t procDoneStatus,
    uint8_t *pEventData
);

static void TemperatureTimerCallback
(
    void *pParam
);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
appLocalization_rangeCfgGlobal_t mGlobalRangeSettings =
{
    .ant_type = 0,
    .t_sw_local = 2,
    .t_pm_tone_ext = 0,
    .tx_pwr = 0,
    .ch_list_auto = TRUE,
    .ch_start = 0,
    .ch_stop = XCVR_CHAN_MAX,
    .ch_isrand = FALSE,
};

appLocalization_rangeCfg_t mDefaultRangeSettings =
{
    .configId = APP_LOCALIZATION_CONFIG_ID,
    .main_mode_type = 2, /* RTP */
    .sub_mode_type = 1, /* RTT */
    .main_mode_min = 4,
    .main_mode_max = 8,
    .main_mode_repeat = 1,
    .mode0_nb = 3,
    .rtt_type = 0, /* coarse */
    .phy = (uint8_t)gLePhy1M_c,
    .cs_sync_phy = (uint8_t)gLePhy1M_c,
    .ant_cfg_index = 0,
    .ant_perm_index = 0,
    .initiator_AA = {0x36, 0xE6, 0x5E, 0x21},
    .reflector_AA = {0x59, 0x8D, 0x11, 0x41},
    .ch_map_repeat = 1,
    .channelSelectionType = 0, /* Algorithm #3b */
    .maxProcedureDuration = 0xFFFF,
    .minPeriodBetweenProcedures = 1,
    .maxPeriodBetweenProcedures = 1,
    .maxNumProcedures = 1,
    .minSubeventLen = 40000,
    .maxSubeventLen = 40000,
    .txPwrDelta = 0, /* 0dBm */
    .t_fcs = 150,
    .t_ip1 = 145,
    .t_ip2 = 145,
    .t_pm = 20,
    .connInterval = 0,
    .csAlgoBuf = NULL
};

appLocalization_rangeCfg_t mRangeSettings[gAppMaxConnections_c];

#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1)
#if defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)
static uint8_t gLocalizationAlgorithm = eMciqAlgoEmbedRADE;
#elif defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)
static uint8_t gLocalizationAlgorithm = eMciqAlgoEmbedCDE;
#else
static uint8_t gLocalizationAlgorithm = 0U;
#endif /* gAppUseRADEAlgorithm_d || gAppUseCDEAlgorithm_d */
#endif /* gAppRunAlgo_d */

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
appLocalization_TimeInfo_t gCsTimeInfo = {0};
#endif

/************************************************************************************
 *************************************************************************************
 *  Public prototypes
 *************************************************************************************
 ************************************************************************************/
extern bleResult_t Ble_HciSend(hciPacketType_t packetType, void* pPacket, uint16_t packetSize);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

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
)
{
    bleResult_t status = gBleSuccess_c;

    if (pfAppCallback == NULL)
    {
        status = gBleInvalidParameter_c;
    }
    else
    {
        const uint8_t CSChMapReal[APP_LOCALIZATION_CH_MAP_LEN] = {0xfc,0xff,0x7f,0xfc,0xff,0xff,0xff,0xff,0xff,0x1f}; /* All valid channels */

        /* Start LCE */
#ifdef LCE_KW47_MCXW72
        (void)APP_InitLce();
#endif

        mpfAppCsCallback = pfAppCallback;

#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1)
        mpfDisplayResultCallback = pfAppDisplayCallback;
#endif

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
        RasClient_OpenRapTimer();
#endif /* gRasRREQ_d */
#elif defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
        BtcsServer_Init();
#endif /* gAppRasDataTransfer_d */

        /* Set default values */
        mGlobalRangeSettings.role = role;
        for (uint8_t index = 0U; index < (uint8_t)gAppMaxConnections_c; index++)
        {
            FLib_MemCpy(&mRangeSettings[index], &mDefaultRangeSettings, sizeof(appLocalization_rangeCfg_t));
            FLib_MemCpy(mRangeSettings[index].ch_map, CSChMapReal, APP_LOCALIZATION_CH_MAP_LEN);
            /* Procedure repeat */
            mRangeSettings[index].maxNumProcedures = gCsProcRepeatMaxNumProcedures_c;
#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
            maPsmChannels[index] = 0U;
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */
        }

        /* Configure pinmux for antenna switching */
        (void)PLATFORM_InitLcl();
        /* Antenna switching enabled also in Debug mode */
        PLATFORM_InitLclGpioDebug(false);

#if defined (BOARD_LOCALIZATION_REVISION_SUPPORT) && (BOARD_LOCALIZATION_REVISION_SUPPORT == 1U)
        /* Set antenna type according to board used */
        /* KW47 and more series */
        mGlobalRangeSettings.ant_type = CS_ANT_BOARD_LOC_PRINTED;

        /* Set antenna configuration to 2 antennas of both initiator and reflector */
        for (uint8_t index = 0U; index < (uint8_t)gAppMaxConnections_c; index++)
        {
            mRangeSettings[index].ant_cfg_index = gAntennaCfgIdx7_c;
        }
#endif
    }

    return status;
}

/*! *********************************************************************************
*\fn           bleResult_t AppLocalization_HostInitHandler(void)
*
*\brief        This function continues the initialization of the Application
*              Localization.common module and should be called after the Host has
*              been initialized.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_HostInitHandler(void)
{
    bleResult_t status = gBleSuccess_c;
    const uint8_t *ant2gpio_p;
    uint32_t paramsPresence = (gCSParamAntennaConfigPresent_c | gCSParamPctPhaseRotationPresent_c);
    uint8_t  paramDataLength = CS_CONFIG_VENDOR_PARAM_LENGTH;
    uint8_t aAppData[CS_CONFIG_VENDOR_PARAM_LENGTH] = {0U};

    /* Initialize channel sounding */
    CS_Init();
    status = CS_RegisterCsMetaEventCallback(csMetaEventCallback);

    if (status == gBleSuccess_c)
    {
        status = CS_RegisterCsEventCallback(csEventCmdCompleteCallback);
    }

    if (status == gBleSuccess_c)
    {
        status = CS_RegisterCmdStatusEventCallback(csEventCmdStatusCallback);
    }

    /* Register application callbacks */
    App_RegisterCsCallbacks(AppLocalization_CSCmdCompleteCallback,
                            AppLocalization_CSCmdStatusCallback,
                            AppLocalization_CSMetaEventCallback);

    /* Reset measurement data */
    for(uint8_t index = 0U; index < (uint8_t)gAppMaxConnections_c; index++)
    {
        FLib_MemSet(&mResultData[index], 0x00, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
        mResultData[index].pData = NULL;

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
        RasClient_Init(mpfAppCsCallback);
#endif /* gRasRREQ_d */
#elif defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
        BtcsClient_Init();
#endif /* gAppRasDataTransfer_d */
    }

    for (uint8_t index = 0U; index < (uint8_t)gMaxBondedDevices_c; index++)
    {
        mpCachedRemoteCaps[index] = NULL;
    }

    if (mGlobalRangeSettings.ant_type >= CS_NO_ANT_TYPES)
    {
        ant2gpio_p = NULL;
        assert(0);
    }
    else
    {
        ant2gpio_p = cs_ant_idx_to_GPIO[mGlobalRangeSettings.ant_type];
    }

    if (ant2gpio_p[0] != LCL_HAL_LOC_UNAVAILABLE)
    {
        /* Fill command data */

        /* Antenna configuration */
        aAppData[0U] = 2U; /* Default antenna switch time */
        aAppData[1U] = 4U; /* Number of antenna paths */
        aAppData[2U] = (mGlobalRangeSettings.ant_type == CS_ANT_BOARD_ANTDIV_4_ANT) ? 4U : 2U; /* Number of antennas (only 2 antennas on reference designs) */
        FLib_MemCpy((void *)(&aAppData[3U]), ant2gpio_p, APP_LOCALIZATION_MAX_NO_ANTENNAS);

        /* PCT rotation calibration */
        FLib_MemCpy((void *)(&aAppData[3U + APP_LOCALIZATION_MAX_NO_ANTENNAS]), maPctRotationParams, (4U * APP_LOCALIZATION_MAX_NO_ANTENNAS));

        status = CS_ConfigVendorCommand(paramsPresence, paramDataLength, aAppData);
    }
    else
    {
        status = gBleInvalidParameter_c;
    }

    if (status == gBleSuccess_c)
    {
        status = CS_ReadLocalSupportedCapabilities();
    }

    /* Start interval timer for temperature updates */
    (void)TM_Open((timer_handle_t)mTemperatureTimerId);
    (void)TM_InstallCallback((timer_handle_t)mTemperatureTimerId, TemperatureTimerCallback, NULL);
    (void)TM_Start((timer_handle_t)mTemperatureTimerId, kTimerModeSingleShot | kTimerModeLowPowerTimer, gTemperaturePollingInterval_c);

    return status;
}

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
)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t nvmIndex = gInvalidNvmIndex_c;
    bool_t isBonded = FALSE;

    result = Gap_CheckIfBonded(deviceId, &isBonded, &nvmIndex);

    if ((mGlobalRangeSettings.role != gCsRoleInitiator_c) ||
        (maAppLclState[deviceId] != gAppLclIdle_c))
    {
        result = gBleInvalidState_c;
    }
    else
    {
        if ((isBonded == TRUE) && (mpCachedRemoteCaps[nvmIndex] == NULL))
        {
            result = CS_ReadRemoteSupportedCapabilities(deviceId);
        }
        else
        {
            if ((result == gBleSuccess_c) && (isBonded == TRUE))
            {
                result = CS_WriteCachedRemoteSupportedCapabilities(mpCachedRemoteCaps[nvmIndex]);
            }
        }
    }

    if (result == gBleSuccess_c)
    {
        if ((isBonded == TRUE) && (mpCachedRemoteCaps[nvmIndex] == NULL))
        {
            maAppLclState[deviceId] = gAppLclWaitingForRRSC_c;
        }
        else
        {
            maAppLclState[deviceId] = gAppLclWaitingForWCCC_c;
        }
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
        gCsTimeInfo.csConfigStartTs = TM_GetTimestamp();
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */
    }

    return result;
}

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
)
{
    bleResult_t result = gBleSuccess_c;

    if (maAppLclState[deviceId] != gAppLclIdle_c)
    {
        result = gBleInvalidState_c;
    }
    else
    {
        result = CS_ProcedureEnable(deviceId, mRangeSettings[deviceId].configId, TRUE);
    }

    if (result == gBleSuccess_c)
    {
        maAppLclState[deviceId] = gAppLclWaitingForPECS_c;
    }

    return result;
}

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
)
{
    bleResult_t result = gBleSuccess_c;

    result = CS_SecurityEnable(deviceId);

    if (result == gBleSuccess_c)
    {
        maAppLclState[deviceId] = gAppLclWaitingForSECS_c;
    }

    return result;
}

/*! *********************************************************************************
*\fn           bleResult_t AppLocalization_SetProcedureParameters(deviceId_t deviceId)
*
*\brief        Set procedure parameters for the given peer device id. Config must exist.
*
*\param  [in]  deviceId     Peer device id.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_SetProcedureParameters(deviceId_t deviceId)
{
    bleResult_t result = gBleSuccess_c;

    gCsSetProcedureParamsCommandParams_t params;

    params.configId = mRangeSettings[deviceId].configId;
    params.maxProcedureDuration = mRangeSettings[deviceId].maxProcedureDuration;
    params.minProcedureInterval = mRangeSettings[deviceId].minPeriodBetweenProcedures;
    params.maxProcedureInterval = mRangeSettings[deviceId].maxPeriodBetweenProcedures;
    params.maxProcCount = mRangeSettings[deviceId].maxNumProcedures;
    params.minSubeventLen = mRangeSettings[deviceId].minSubeventLen;
    params.maxSubeventLen = mRangeSettings[deviceId].maxSubeventLen;
    params.toneAntennaConfigSelection = mRangeSettings[deviceId].ant_cfg_index;
    params.phys = mRangeSettings[deviceId].phy; /* Should have been updated by the app upon connection to be the same as the connection PHY */
    params.txPwrDelta = 0; /* 0dBm */
    params.preferredPeerAntenna = 3U; /* Use any of the 2 antenna */
    params.SNRCtrlInitiator = gSNRControlNotApplied_c;
    params.SNRCtrlReflector = gSNRControlNotApplied_c;
    /* Reset mResultData - if pData is allocated, it remains so for the upcoming procedure */
    FLib_MemSet(&mResultData[deviceId], 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));

    result = CS_SetProcedureParameters(deviceId, &params);

    if (result == gBleSuccess_c)
    {
        /* Waiting for Set Procedure Parameters command complete event. */
        maAppLclState[deviceId] = gAppLclWaitingForSPPCC_c;
    }

    return result;
}

/*! *********************************************************************************
*\fn         void AppLocalization_SetNumberOfProcedures(deviceId_t deviceId, uint16_t value)
*
*\brief      Set the number of CS procedures to be executed during procedure repeat
*
*\param  [in]   deviceId        Peer device identifier.
*\param  [in]   value           Number of procedure repeats.
*
*\return     None
********************************************************************************** */
void AppLocalization_SetNumberOfProcedures
(
    deviceId_t deviceId,
    uint16_t value
)
{
    mRangeSettings[deviceId].maxNumProcedures = value;
}

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
)
{
    mRangeSettings[deviceId].connInterval = connInterval;
}

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
)
{
    bleResult_t result = gBleSuccess_c;

    /* Reset measurement data */
    if (mResultData[deviceId].pData != NULL)
    {
        (void)MEM_BufferFree(mResultData[deviceId].pData);
        mResultData[deviceId].pData = NULL;
    }
    FLib_MemSet(&mResultData[deviceId], 0x00, sizeof(rasMeasurementData_t));

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
    RasClient_ResetRasTransferInfo(deviceId);
#endif /* gRasRREQ_d */
#endif /* gAppRasDataTransfer_d */

    /* Reset state back to idle */
    maAppLclState[deviceId] = gAppLclIdle_c;
    result = CS_ProcedureEnable(deviceId, mRangeSettings[deviceId].configId, FALSE);

    return result;
}

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
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_ReadConfig
(
    deviceId_t deviceId,
    appLocalization_rangeCfg_t *pConfigOut
)
{
    bleResult_t result = gBleSuccess_c;

    if (pConfigOut == NULL)
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        FLib_MemCpy(pConfigOut, &mRangeSettings[deviceId], sizeof(appLocalization_rangeCfg_t));
    }

    return result;
}

/*! *********************************************************************************
*\fn            bleResult_t AppLocalization_WriteConfig(deviceId_t deviceId,
*               appLocalization_rangeCfg_t pConfig)
*
*\brief         Write current localization configuration.
*
*\param  [in]   deviceId        Peer device identifier.
*\param  [out]  pConfig         Pointer to the localization configuration information.
*
*\retval        bleResult_t     Result of the operation.
********************************************************************************** */
bleResult_t AppLocalization_WriteConfig
(
    deviceId_t deviceId,
    appLocalization_rangeCfg_t *pConfig
)
{
    bleResult_t result = gBleSuccess_c;

    if (pConfig == NULL)
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        FLib_MemCpy(&mRangeSettings[deviceId], pConfig, sizeof(appLocalization_rangeCfg_t));
    }

    return result;
}

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
)
{
#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
    if ( maAppLclState[deviceId] == gAppLclWaitingForMeasData_c )
    {
        Ras_SetDataPointer(deviceId, NULL);
    }
#endif

    if ( maAppLclState[deviceId] != gAppLclIdle_c )
    {
        maAppLclState[deviceId] = gAppLclIdle_c;
    }

    /* Reset mResultData */
    FLib_MemSet(&mResultData[deviceId], 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
    if (mResultData[deviceId].pData != NULL)
    {
        (void)MEM_BufferFree(mResultData[deviceId].pData);
        mResultData[deviceId].pData = NULL;
    }

    maCsProcCount[deviceId] = 0U;

    if ((nvmIndex < (uint8_t)gMaxBondedDevices_c) && (disconnected == TRUE) && (mpCachedRemoteCaps[nvmIndex] != NULL))
    {
        (void)MEM_BufferFree(mpCachedRemoteCaps[nvmIndex]);
        mpCachedRemoteCaps[nvmIndex] = NULL;
    }

#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1)
    maAlgoRunCount[deviceId] = 0U;
#endif

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
    RasClient_ResetPeer(deviceId,disconnected);
#endif /* gRasRREQ_d*/
#else
#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
    BtcsServer_ResetPeer(deviceId,disconnected);
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */

#if defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
    BtcsClient_ResetPeer(deviceId,disconnected);
#endif
#endif /* #gAppRasDataTransfer_d */

#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
    if (disconnected == TRUE)
    {
        maPsmChannels[deviceId] = 0U;
    }
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */
}

#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
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
)
{
    gLocalizationAlgorithm = algorithm;
}

/*! *********************************************************************************
*\fn         bleResult_t AppLocalization_GetAlgorithm(void)
*
*\brief      Returns which algorithms are to be run.
*
*\param[in]  none

*
*\return     algorithm      Bitmask (CDE, RADE).
********************************************************************************** */
uint8_t AppLocalization_GetAlgorithm(void)
{
    return gLocalizationAlgorithm;
}
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
)
{
    gCsTimeInfo.connInterval = connInterval;
}
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
)
{
    bleResult_t result = gBleSuccess_c;

    if ((mpCachedRemoteCaps[nvmIdx] == NULL) || (maAppLclState[deviceId] != gAppLclIdle_c))
    {
        result = gBleInvalidState_c;
    }
    else
    {
        result = CS_WriteCachedRemoteSupportedCapabilities(mpCachedRemoteCaps[nvmIdx]);
    }

    if (result == gBleSuccess_c)
    {
        maAppLclState[deviceId] = gAppLclWaitingForWCCC_c;
    }

    return result;
}

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
)
{
    if (mpCachedRemoteCaps[nvmIndex] == NULL)
    {
        mpCachedRemoteCaps[nvmIndex] = MEM_BufferAlloc(sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t));
    }

    if (mpCachedRemoteCaps[nvmIndex] != NULL)
    {
        FLib_MemCpy(mpCachedRemoteCaps[nvmIndex],
                    recvCapabilities,
                    sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t));
    }
}

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
)
{
    return maAppLclState[deviceId];
}

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
)
{
    maAppLclState[deviceId] = recvState;
}

/*! *********************************************************************************
*\fn            void AppLocalization_GetRemoteCachedSupportedCapabilities(uint8_t nvmIndex);
*
*\brief         Get the supported capabilities for the specified peer.
*
*\param[in]     nvmIndex      Index of the device in the NVM
*
*\retval        csReadRemoteSupportedCapabilitiesCompleteEvent_t Pointer to peer capabilities
********************************************************************************** */
csReadRemoteSupportedCapabilitiesCompleteEvent_t* AppLocalization_GetRemoteCachedSupportedCapabilities
(
    uint8_t nvmIndex
)
{
    return mpCachedRemoteCaps[nvmIndex];
}

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
)
{
    return maAlgoRunCount[deviceId];
}

/*! *********************************************************************************
*\fn            void AppLocalization_SetAlgoRunCount(deviceId_t deviceId,
*               uint16_t   recvAlgoRunCount);
*
*\brief         Set the algorithm runs count value received from another anchor.
*
*\param[in]     deviceId      Peer identifier
*\param[in]     uint16_t      Algorithm runs count value
********************************************************************************** */
void AppLocalization_SetAlgoRunCount
(
    deviceId_t deviceId,
    uint16_t   recvAlgoRunCount
)
{
    maAlgoRunCount[deviceId] = recvAlgoRunCount;
}

/*! *********************************************************************************
*\fn            bool_t AppLocalization_GetRttSoundingSupport(void);
*
*\brief         Check local RTT sounding support.
*
*\param[in]     none
*
*\retval        TRUE if RTT sounding is supported, FALSE otherwise
********************************************************************************** */
bool_t AppLocalization_GetRttSoundingSupport(void)
{
    return rttSoundingSupported;
}

#endif

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
)
{
    return maCsProcCount[deviceId];
}

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
/*! *********************************************************************************
*\fn            void AppLocalization_GetProcedureCount(deviceId_t deviceId);
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
)
{
    return mResultData[deviceId].procedureCounter;
}

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
)
{
    FLib_MemSet(&mResultData[deviceId], 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
    if (mResultData[deviceId].pData != NULL)
    {
        FLib_MemSet(mResultData[deviceId].pData, 0U, gRasCsSubeventDataSize_c);
    }
}

/*! *********************************************************************************
*\fn            void AppLocalization_GetProcDoneStatus(deviceId_t deviceId, uint8_t subeventIdx);
*
*\brief         Get the procedure done status for the specified subevent index and peer.
*
*\param[in]     deviceId         Peer identifier
*
*\retval        uint16_t         Procedure done status
********************************************************************************** */
uint8_t AppLocalization_GetProcDoneStatus
(
    deviceId_t deviceId,
    uint8_t subeventIdx
)
{
    return mResultData[deviceId].aSubEventData[subeventIdx].subevtHeader.procedureDoneStatus;
}
#endif /* gRasRREQ_d */
#endif /* gAppRasDataTransfer_d */

/*! *********************************************************************************
*\fn            void AppLocalization_GetSubeventIdx(deviceId_t deviceId);
*
*\brief         Get the current subevent index value for the specified peer.
*
*\param[in]     deviceId      Peer identifier
*
*\retval        uint16_t      Subevent index value
********************************************************************************** */
uint8_t AppLocalization_GetSubeventIdx
(
    deviceId_t deviceId
)
{
    return mResultData[deviceId].subeventIndex;
}

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
)
{
    return mResultData[deviceId].numAntennaPaths;
}

#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
void AppLocalization_SetPsmChannelId
(
    deviceId_t deviceId,
    uint16_t   channelId
)
{
    maPsmChannels[deviceId] = channelId;
}
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */

/*! *********************************************************************************
*\fn            bleResult_t AppLocalization_CreateConfig(deviceId_t deviceId, bool_t createContextRemote)
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
)
{
    bleResult_t result = gBleSuccess_c;

    if (maAppLclState[deviceId] == gAppLclIdle_c)
    {
        gCsCreateConfigCommandParams_t createConfigParams;

        createConfigParams.configId = configId;
        createConfigParams.mainModeType = mRangeSettings[deviceId].main_mode_type;
        createConfigParams.subModeType = mRangeSettings[deviceId].sub_mode_type;
        createConfigParams.mainModeMinSteps = mRangeSettings[deviceId].main_mode_min;
        createConfigParams.mainModeMaxSteps = mRangeSettings[deviceId].main_mode_max;
        createConfigParams.mainModeRepetition = mRangeSettings[deviceId].main_mode_repeat;
        createConfigParams.mode0Steps = mRangeSettings[deviceId].mode0_nb;
        createConfigParams.RTTTypes = (rttTypes_t)mRangeSettings[deviceId].rtt_type;
        createConfigParams.csSyncPhy = mRangeSettings[deviceId].cs_sync_phy;
        FLib_MemCpy(createConfigParams.channelMap, mRangeSettings[deviceId].ch_map, APP_LOCALIZATION_CH_MAP_LEN);
        createConfigParams.channelMapRepetition = mRangeSettings[deviceId].ch_map_repeat;
        createConfigParams.channelSelectionType = mRangeSettings[deviceId].channelSelectionType;
        createConfigParams.ch3cShape = 0U; /* unused */
        createConfigParams.ch3cJump = 3; /* unused */
        createConfigParams.role = mGlobalRangeSettings.role;

        /* If TRUE, create CS configuration in both local and remote Controller */
        createConfigParams.createContext = (uint8_t)createContextRemote;

        result = CS_CreateConfig(deviceId, &createConfigParams);
        if (result != gBleSuccess_c)
        {
            maAppLclState[deviceId] = gAppLclIdle_c;
            AppLocalizationError(deviceId, gAppLclCCConfigError_c);
        }
        else
        {
            if (createContextRemote)
            {
                /* Create CS configuration in both local and remote Controller */
                if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
                {
                    maAppLclState[deviceId] = gAppLclWaitingForCCCS_c;
                }
                else
                {
                    maAppLclState[deviceId] = gAppLclWaitingForCC_c;
                }
            }
            else
            {
                /* Create CS configuration in local Controller only */
                maAppLclState[deviceId] = gAppLclWaitingForLocCfg_c;
            }
        }
    }
    else
    {
            result = gBleInvalidState_c;
    }
    return result;
}

/*! *********************************************************************************
*\fn            bleResult_t AppLocalization_SetDefaultConfig(deviceId_t deviceId,
*                                                            uint8_t configId)
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
)
{
    mRangeSettings[deviceId].configId = configId;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn           bleResult_t csMetaEventCallback(csMetaEvent_t* pPacket)
*
*\brief        Send CS Meta Events from the channel sounding lib to the application.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
static bleResult_t csMetaEventCallback
(
    csMetaEvent_t* pPacket
)
{
    bleResult_t result = gBleSuccess_c;
    csMetaEventData_t *pCsMetaEvent = MEM_BufferAlloc(sizeof(csMetaEventData_t));
    appMsgFromHost_t *pMsgIn = NULL;
    uint32_t msgLen = (uint32_t)&(pMsgIn->msgData) + sizeof(void*);

    if (pCsMetaEvent != NULL)
    {
        /* Copy event data according to event type */
        switch (pPacket->eventType)
        {
            case csReadRemoteSupportedCapabilitiesComplete_c:
            {
                pCsMetaEvent->eventType = gCsMetaEvtReadRemoteSupportedCapabilities_c;
                csReadRemoteSupportedCapabilitiesCompleteEvent_t* pRemoteCapabilities =
                    MEM_BufferAlloc(sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t));

                if (pRemoteCapabilities != NULL)
                {
                    pCsMetaEvent->pEventData = (void*)pRemoteCapabilities;
                    FLib_MemCpy(pCsMetaEvent->pEventData,
                                &pPacket->eventData.csReadRemoteSupportedCapabilitiesComplete,
                                sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t));
                }
                else
                {
                    result = gBleOutOfMemory_c;
                }
            }
            break;

            case csSecurityEnableComplete_c:
            {
                pCsMetaEvent->eventType = gCsMetaEvtSecurityEnableComplete_c;
                csSecurityEnableCompleteEvent_t* pSecurityEnable = MEM_BufferAlloc(sizeof(csSecurityEnableCompleteEvent_t));

                if (pSecurityEnable != NULL)
                {
                    pCsMetaEvent->pEventData = (void*)pSecurityEnable;
                    FLib_MemCpy(pCsMetaEvent->pEventData,
                                &pPacket->eventData.csSecurityEnableComplete,
                                sizeof(csSecurityEnableCompleteEvent_t));
                }
                else
                {
                    result = gBleOutOfMemory_c;
                }
            }
            break;

            case csConfigComplete_c:
            {
                pCsMetaEvent->eventType = gCsMetaEvtConfigComplete_c;
                csConfigCompleteEvent_t* pConfigComplete = MEM_BufferAlloc(sizeof(csConfigCompleteEvent_t));

                if (pConfigComplete != NULL)
                {
                    pCsMetaEvent->pEventData = (void*)pConfigComplete;
                    FLib_MemCpy(pCsMetaEvent->pEventData,
                                &pPacket->eventData.csConfigComplete,
                                sizeof(csConfigCompleteEvent_t));
                }
                else
                {
                    result = gBleOutOfMemory_c;
                }
            }
            break;

            case csProcedureEnableCompleteEvent_c:
            {
                pCsMetaEvent->eventType = gCsMetaEvtProcedureEnableComplete_c;
                csProcedureEnableCompleteEvent_t* pProcEnableComplete = MEM_BufferAlloc(sizeof(csProcedureEnableCompleteEvent_t));

                if (pProcEnableComplete != NULL)
                {
                    pCsMetaEvent->pEventData = (void*)pProcEnableComplete;
                    FLib_MemCpy(pCsMetaEvent->pEventData,
                                &pPacket->eventData.csProcedureEnableCompleteEvent,
                                sizeof(csProcedureEnableCompleteEvent_t));
                }
                else
                {
                    result = gBleOutOfMemory_c;
                }
            }
            break;

            case csSubeventResultEvent_c:
            {
                pCsMetaEvent->eventType = gCsMetaEvtSubeventResult_c;
                csSubeventResultEvent_t* pResultEvent = MEM_BufferAlloc(sizeof(csSubeventResultEvent_t));

                if (pResultEvent != NULL)
                {
                    /* Compute length of received CS data */
                    uint32_t dataSize = 0;
                    uint8_t *stepDataLenPtr = &pPacket->eventData.csSubeventResultEvent.pData[2U * sizeof(uint8_t)]; /* Skip mode, channel */

                    for (uint32_t i = 0; i < pPacket->eventData.csSubeventResultEvent.numStepsReported; i++)
                    {
                        dataSize += ((uint32_t)*(&stepDataLenPtr[dataSize])) + sizeof(uint8_t) * 3U;
                    }

                    pCsMetaEvent->pEventData = (void*)pResultEvent;
                    pResultEvent->pData = MEM_BufferAlloc(dataSize);
                    if (pResultEvent->pData != NULL)
                    {
                        /* Copy header information */
                        FLib_MemCpy(pResultEvent,
                                    &pPacket->eventData.csSubeventResultEvent,
                                    sizeof(csSubeventResultEvent_t) - sizeof(uint8_t*));

                        /* Copy event data */
                        FLib_MemCpy(pResultEvent->pData,
                                    pPacket->eventData.csSubeventResultEvent.pData,
                                    dataSize);
                    }
                    else
                    {
                        result = gBleOutOfMemory_c;
                    }
                }
                else
                {
                    result = gBleOutOfMemory_c;
                }

                /* Free Event Data */
                (void)MEM_BufferFree(pPacket->eventData.csSubeventResultEvent.pData);
            }
            break;

            case csSubeventResultContinueEvent_c:
            {
                pCsMetaEvent->eventType = gCsMetaEvtSubeventResultContinue_c;
                csSubeventResultContinueEvent_t* pResultContinueEvt = MEM_BufferAlloc(sizeof(csSubeventResultContinueEvent_t));

                if (pResultContinueEvt != NULL)
                {
                    /* Compute length of received CS data */
                    uint32_t dataSize = 0;
                    uint8_t *stepDataLenPtr = &pPacket->eventData.csSubeventResultContinueEvent.pData[2U * sizeof(uint8_t)]; /* Skip mode, channel */

                    for (uint32_t i = 0; i < pPacket->eventData.csSubeventResultContinueEvent.numStepsReported; i++)
                    {
                        dataSize += ((uint32_t)*(&stepDataLenPtr[dataSize])) + sizeof(uint8_t) * 3U;
                    }

                    pCsMetaEvent->pEventData = (void*)pResultContinueEvt;
                    pResultContinueEvt->pData = MEM_BufferAlloc(dataSize);
                    if (pResultContinueEvt->pData != NULL)
                    {
                        /* Copy header information */
                        FLib_MemCpy(pResultContinueEvt,
                                    &pPacket->eventData.csSubeventResultContinueEvent,
                                    sizeof(csSubeventResultContinueEvent_t) - sizeof(uint8_t*));

                        /* Copy event data */
                        FLib_MemCpy(pResultContinueEvt->pData,
                                    pPacket->eventData.csSubeventResultContinueEvent.pData,
                                    dataSize);
                    }
                    else
                    {
                        result = gBleOutOfMemory_c;
                    }
                }
                else
                {
                    result = gBleOutOfMemory_c;
                }
                /* Free Event Data */
                (void)MEM_BufferFree(pPacket->eventData.csSubeventResultContinueEvent.pData);
            }
            break;

            case csError_c:
            {
                pCsMetaEvent->eventType = gCsMetaEvtError_c;
                csErrorEvent_t* pError = MEM_BufferAlloc(sizeof(csErrorEvent_t));

                if (pError != NULL)
                {
                    pCsMetaEvent->pEventData = (void*)pError;
                    FLib_MemCpy(pCsMetaEvent->pEventData,
                                &pPacket->eventData.csError,
                                sizeof(csErrorEvent_t));
                }
                else
                {
                    result = gBleOutOfMemory_c;
                }
            }
            break;

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
            case csHciDataLog_c:
            {
                pCsMetaEvent->eventType = gCsMetaEvtHciDataLog_c;
                csHciDataLogEvent_t *pHciDataLog = MEM_BufferAlloc(sizeof(csHciDataLogEvent_t));

                if (pHciDataLog != NULL)
                {
                    pCsMetaEvent->pEventData = (void*)pHciDataLog;
                    pHciDataLog->opCode = pPacket->eventData.csHciDataLogEvent.opCode;
                    pHciDataLog->packetSize = pPacket->eventData.csHciDataLogEvent.packetSize;
                    uint8_t *hciPacket = MEM_BufferAlloc(pPacket->eventData.csHciDataLogEvent.packetSize);

                    if (hciPacket != NULL)
                    {
                        /* Copy HCI data packet into event data */
                        FLib_MemCpy(hciPacket,
                                    pPacket->eventData.csHciDataLogEvent.pPacket,
                                    pPacket->eventData.csHciDataLogEvent.packetSize);
                        pHciDataLog->pPacket = hciPacket;
                    }
                    else
                    {
                        result = gBleOutOfMemory_c;
                    }
                }
                else
                {
                    result = gBleOutOfMemory_c;
                }
            }
            break;
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */

            default:
            {
                ; /* No action required */
            }
            break;
        }

        pMsgIn = MSG_Alloc(msgLen);
        if (pMsgIn != NULL)
        {
            pMsgIn->msgType = gAppCsMetaEventMsg_c;
            pMsgIn->msgData.pCsEventData = pCsMetaEvent;

            /* Put message in the Host Stack to App queue */
            (void)MSG_QueueAddTail(&mHostAppInputQueue, pMsgIn);

            /* Signal application */
            (void)OSA_EventSet(mAppEvent, gAppEvtMsgFromHostStack_c);
        }
        else
        {
            if (pCsMetaEvent->pEventData != NULL)
            {
                (void)MEM_BufferFree(pCsMetaEvent->pEventData);
            }
            (void)MEM_BufferFree(pCsMetaEvent);
        }
    }

    return result;
}

/*! *********************************************************************************
*\fn           void AppLocalization_CSMetaEventCallback(void* pCsMetaEvent)
*
*\brief        Handles CS Meta Events from the channel sounding lib.
*
*\retval       none.
********************************************************************************** */
static void AppLocalization_CSMetaEventCallback
(
    void* pCsMetaEvent
)
{
    bleResult_t result = gBleSuccess_c;
    csMetaEventData_t* pPacket = (csMetaEventData_t*)pCsMetaEvent;
    deviceId_t deviceId = gInvalidDeviceId_c;

    switch (pPacket->eventType)
    {
        case gCsMetaEvtReadRemoteSupportedCapabilities_c:
        {
            csReadRemoteSupportedCapabilitiesCompleteEvent_t* pRemoteCapabilities =
                (csReadRemoteSupportedCapabilitiesCompleteEvent_t*)pPacket->pEventData;
            deviceId = pRemoteCapabilities->deviceId;
            mRangeSettings[deviceId].t_sw_remote = pRemoteCapabilities->TSWtimeSupported;

            if (((mGlobalRangeSettings.role == gCsRoleInitiator_c) &&
                    (maAppLclState[deviceId] == gAppLclWaitingForRRSCC_c)) ||
                (mGlobalRangeSettings.role == gCsRoleReflector_c))
            {
                uint8_t syncAntennaSelection = 0xFF; /* proprietary antenna shuffling */
                int8_t  maxTxPowerLevel = 10; /* 10 dBm */
                uint8_t nvmIndex = gInvalidNvmIndex_c;
                bool_t isBonded = FALSE;

                (void)Gap_CheckIfBonded(deviceId, &isBonded, &nvmIndex);

               /* Save the capabilities of the peer device */
               if ((isBonded == TRUE) && (mpCachedRemoteCaps[nvmIndex] == NULL))
               {
                    mpCachedRemoteCaps[nvmIndex] = MEM_BufferAlloc(sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t));

                    FLib_MemCpy(mpCachedRemoteCaps[nvmIndex],
                                pRemoteCapabilities,
                                sizeof(csReadRemoteSupportedCapabilitiesCompleteEvent_t));
               }

               result = CS_SetDefaultSettings(deviceId,
                                              (csRoleType)(((uint8_t)gEnableCsInitiator_c) | ((uint8_t)gEnableCsReflector_c)),
                                              syncAntennaSelection,
                                              maxTxPowerLevel);



                if (result != gBleSuccess_c)
                {
                    maAppLclState[deviceId] = gAppLclIdle_c;
                    AppLocalizationError(deviceId, gAppLclSDSConfigError_c);
                }
                else
                {
                    if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
                    {
                        maAppLclState[deviceId] = gAppLclWaitingForSDSCC_c;
                    }
                }
            }
            else
            {
                /* Unexpected event. */
                AppLocalizationError(deviceId, gAppLclUnexpectedRRSCC_c);
            }

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
            if (mGlobalRangeSettings.role == gCsRoleReflector_c)
            {
                gCsTimeInfo.csConfigStartTs = TM_GetTimestamp();
            }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */
        }
        break;

        case gCsMetaEvtSecurityEnableComplete_c:
        {
            csSecurityEnableCompleteEvent_t* pSecurityComplete =
                (csSecurityEnableCompleteEvent_t*)pPacket->pEventData;
            deviceId = pSecurityComplete->deviceId;

            if (mpfAppCsCallback != NULL)
            {
                mpfAppCsCallback(deviceId, NULL, gCsSecurityEnabled_c);
            }
        }
        break;

        case gCsMetaEvtConfigComplete_c:
        {
            csConfigCompleteEvent_t* pConfigComplete = (csConfigCompleteEvent_t*)pPacket->pEventData;
            deviceId = pConfigComplete->deviceId;

            if ((maAppLclState[deviceId] == gAppLclWaitingForCC_c) ||
                (maAppLclState[deviceId] == gAppLclWaitingForLocCfg_c) ||
                (mGlobalRangeSettings.role == gCsRoleReflector_c))
            {
                csConfigCompleteEvent_t *pEvent = pConfigComplete;

                /* Save measurement config locally */
                mGlobalRangeSettings.role = pEvent->role;
                mRangeSettings[deviceId].configId = pEvent->configId;
                mRangeSettings[deviceId].main_mode_type = pEvent->mainModeType;
                mRangeSettings[deviceId].sub_mode_type = pEvent->subModeType;
                mRangeSettings[deviceId].main_mode_min = pEvent->mainModeMinSteps;
                mRangeSettings[deviceId].main_mode_max = pEvent->mainModeMaxSteps;
                mRangeSettings[deviceId].main_mode_repeat = pEvent->mainModeRepetition;
                mRangeSettings[deviceId].mode0_nb = pEvent->mode0Steps;
                mRangeSettings[deviceId].rtt_type = pEvent->RTTTypes;
                mRangeSettings[deviceId].cs_sync_phy = pEvent->csSyncPhy;
                FLib_MemCpy(mRangeSettings[deviceId].ch_map, pEvent->channelMap, gHCICSChannelMapSize);
                mRangeSettings[deviceId].ch_map_repeat = pEvent->channelMapRepetition;
                mRangeSettings[deviceId].t_ip1 = pEvent->TIP1time;
                mRangeSettings[deviceId].t_ip2 = pEvent->TIP2time;
                mRangeSettings[deviceId].t_fcs = pEvent->TFCStime;
                mRangeSettings[deviceId].t_pm = pEvent->TPMtime;

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
                gCsTimeInfo.csConfigEndTs = TM_GetTimestamp();
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

                if (mpfAppCsCallback != NULL)
                {
                    if (maAppLclState[deviceId] == gAppLclWaitingForCC_c)
                    {
                        maAppLclState[deviceId] = gAppLclIdle_c;
                        mpfAppCsCallback(deviceId, NULL, gConfigComplete_c);
                    }
                    else
                    {
                        maAppLclState[deviceId] = gAppLclIdle_c;
                        mpfAppCsCallback(deviceId, NULL, gLocalConfigWritten_c);
                    }
                }
            }
            else
            {
                /* Unexpected event. */
                AppLocalizationError(deviceId, gAppLclUnexpectedCC_c);
            }
        }
        break;

        case gCsMetaEvtSubeventResult_c:
        {
            csSubeventResultEvent_t* pSubeventResult = (csSubeventResultEvent_t*)pPacket->pEventData;
            deviceId = pSubeventResult->deviceId;

            /* Clear local data on new procedure start during ongoing RAS transfer */
            if ((maAppLclState[deviceId] == gAppLclWaitingForMeasData_c) || (maAppLclState[deviceId] == gAppRasTransfInProgress_c))
            {
                maAppLclState[deviceId] = gAppLclReceivingMeasData_c;
                /* Clear local data  */
                FLib_MemSet(&mResultData[deviceId], 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
                if (mResultData[deviceId].pData != NULL)
                {
                    FLib_MemSet(mResultData[deviceId].pData, 0U, gRasCsSubeventDataSize_c);
                }

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
                /* clear any leftover peer data */
                RasClient_ResetPeerInfo(deviceId);
#endif /* gRasRREQ_d */

#if defined (gRasRRSP_d) && (gRasRRSP_d == 1U)
                /* If a transfer was in progress send data overwritten indication */
                if ((Ras_CheckRealTimeData(deviceId) == FALSE) && (Ras_CheckTransferInProgress(deviceId) == TRUE))
                {
                    (void)Ras_SendDataOverwritten(deviceId);
                    if (mpfAppCsCallback != NULL)
                    {
                        mpfAppCsCallback(deviceId, NULL, gDataOverwritten_c);
                    }
                }
                /* Clear RAS data pointer to avoid reading of incomplete data. */
                Ras_SetDataPointer(deviceId, NULL);
#endif /* gRasRRSP_d */
#endif /* gAppRasDataTransfer_d */
            }

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
            if (gCsTimeInfo.csDistMeasStart == 0U)
            {
                gCsTimeInfo.csDistMeasDuration += gCsTimeInfo.subeventLen;

                if (gCsTimeInfo.lastAclConnEvtCnt == 0)
                {
                    /* First CS event for this procedure */
                    gCsTimeInfo.noOfRcvSubEvsInConnInt = 1;
                }
                else if (gCsTimeInfo.lastAclConnEvtCnt == pSubeventResult->startACLConnEvent)
                {
                    /* Additional CS event in same connection interval */
                    gCsTimeInfo.noOfRcvSubEvsInConnInt++;
                    gCsTimeInfo.csDistMeasDuration += (uint64_t)gCsTimeInfo.subeventInterval * 625;
                }
                else
                {
                    /* CS event in new connection interval */
                    /* Estimate the time between the end of the last CS event and the begining of
                    the current CS event which is the first CS event in this connection interval */
                    gCsTimeInfo.csDistMeasDuration += (uint64_t)gCsTimeInfo.connInterval * 1250 - ((gCsTimeInfo.noOfRcvSubEvsInConnInt * gCsTimeInfo.subeventLen) + (uint64_t)((gCsTimeInfo.noOfRcvSubEvsInConnInt - 1) * gCsTimeInfo.subeventInterval * 625));
                    gCsTimeInfo.noOfRcvSubEvsInConnInt = 1;

                    /* Take into account the case where multiple ACL connection events may be
                    between consecutive CS events */
                    gCsTimeInfo.csDistMeasDuration += (uint64_t)gCsTimeInfo.connInterval * 1250 * (pSubeventResult->startACLConnEvent - gCsTimeInfo.lastAclConnEvtCnt - 1);
                }

                gCsTimeInfo.lastAclConnEvtCnt = pSubeventResult->startACLConnEvent;
            }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

            if (maAppLclState[deviceId] == gAppLclReceivingMeasData_c)
            {
                /* Save results */
                result = processCsResultsEvent(pSubeventResult);

                if (result == gBleSuccess_c)
                {
                    switch (pSubeventResult->procedureDoneStatus)
                    {
                        case (uint8_t)gCsCompleteResults_c:
                        {
                            /* All results complete for the CS procedure - check if there is data to send */
                            if (mResultData[deviceId].dataIndex > 0U)
                            {
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
                                if (gCsTimeInfo.csDistMeasStart != 0U)
                                {
                                    /* Distance measurement duration for the first procedure. */
                                    gCsTimeInfo.csDistMeasDuration = TM_GetTimestamp() - gCsTimeInfo.csDistMeasStart;
                                    gCsTimeInfo.csDistMeasStart = 0;
                                    gCsTimeInfo.lastAclConnEvtCnt = pSubeventResult->startACLConnEvent;

                                    gCsTimeInfo.lastAclConnEvtCnt = 0;
                                    gCsTimeInfo.noOfRcvSubEvsInConnInt = 0;
                                }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRRSP_d) && (gRasRRSP_d == 1U)
                                Ras_SetDataPointer(pSubeventResult->deviceId, &mResultData[pSubeventResult->deviceId]);
                                /* Send real-time data after the subevent is completed */
                                if (Ras_CheckRealTimeData(deviceId) == FALSE)
                                {
                                    result = Ras_SendDataReady(pSubeventResult->deviceId);
                                }

                                if (result != gBleSuccess_c)
                                {
                                    AppLocalizationError(pSubeventResult->deviceId, gAppLclRasSendIndicationFailed_c);
                                }
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
                                else
                                {
                                    gCsTimeInfo.transferStart = TM_GetTimestamp();
                                }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */
#endif /* gRasRRSP_d */
#elif defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
                                gCsTimeInfo.transferStart = TM_GetTimestamp();
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */
                                /* Start sending L2CAP data */
                                (void)BtcsServer_SendData(pSubeventResult->deviceId,
                                                          maPsmChannels[pSubeventResult->deviceId],
                                                          gRangingProcResStart_c);
#endif /* gAppRasDataTransfer_d */

#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
                                if ((maCsProcCount[deviceId] > 0U) &&
                                    (maAlgoRunCount[deviceId] != maCsProcCount[deviceId]))
                                {
                                    AppLocalizationError(deviceId, gAppLclAlgoNotRun_c);
                                    maAlgoRunCount[deviceId] = maCsProcCount[deviceId];
                                }
#endif /* gAppRunAlgo_d */
                                maCsProcCount[deviceId]++;

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
                                /* Local procedure is over - Wait for RAS transfer */
                                maAppLclState[deviceId] = gAppRasTransfInProgress_c;
#endif /* gRasRREQ_d */
#endif /* gAppRasDataTransfer_d */

#if (defined (gRasRRSP_d) && (gRasRRSP_d == 1U)) || \
    (defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U))
                                if (maCsProcCount[deviceId] == mRangeSettings[deviceId].maxNumProcedures)
                                {
                                    maAppLclState[deviceId] = gAppLclIdle_c;
                                }
                                else
                                {
                                    maAppLclState[deviceId] = gAppLclWaitingForMeasData_c;
                                }
#endif

                                if (mpfAppCsCallback != NULL)
                                {
                                    mpfAppCsCallback(deviceId, NULL, gLocalMeasurementComplete_c);
                                }
                            }
                            else
                            {
                                /* Check if we reached the last procedure */
                                if (maCsProcCount[deviceId] == mRangeSettings[deviceId].maxNumProcedures)
                                {
                                    maAppLclState[deviceId] = gAppLclIdle_c;
                                }
                                else
                                {
                                    maAppLclState[deviceId] = gAppLclWaitingForMeasData_c;
                                }
                            }
                        }
                        break;

                        case (uint8_t)gCsPartialResults_c:
                        {
                            /* Partial results with more to follow for the CS procedure */
                        }
                        break;

                        case (uint8_t)gCsNoResultsProcAborted_c:
                        {
                            /* All subsequent CS procedures aborted */
                            maCsProcCount[deviceId] = 0U;
                            maAppLclState[deviceId] = gAppLclIdle_c;
                            if (mpfAppCsCallback != NULL)
                            {
                                mpfAppCsCallback(deviceId, (void*)&pSubeventResult->abortReason, gErrorProcedureAborted_c);
                            }
                        }
                        break;

                        default:
                        {
                            /* Check if we reached the last procedure */
                            if (maCsProcCount[deviceId] == mRangeSettings[deviceId].maxNumProcedures)
                            {
                                maAppLclState[deviceId] = gAppLclIdle_c;
                            }
                            else
                            {
                                maAppLclState[deviceId] = gAppLclWaitingForMeasData_c;
                            }
                            AppLocalizationError(deviceId, gAppLclProcStatusFailed_c);
                        }
                        break;
                    }
                }
                else
                {
                    /* Error occured! */
                    AppLocalizationError(deviceId, gAppLclErrorProcessingSubevent_c);
                }
            }
            else
            {
                /* Unexpected event. */
                AppLocalizationError(deviceId, gAppLclUnexpectedSRE_c);
            }
            /* Free Data */
            (void)MEM_BufferFree(pSubeventResult->pData);
        }
        break;

        case gCsMetaEvtSubeventResultContinue_c:
        {
            csSubeventResultContinueEvent_t* pSubeventResultContinue =
                (csSubeventResultContinueEvent_t*)pPacket->pEventData;
            deviceId = pSubeventResultContinue->deviceId;

            if ((maAppLclState[deviceId] == gAppLclWaitingForMeasData_c) || (maAppLclState[deviceId] == gAppRasTransfInProgress_c))
            {
                maAppLclState[deviceId] = gAppLclReceivingMeasData_c;

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRRSP_d) && (gRasRRSP_d == 1U)
                /* Clear RAS data pointer to avoid reading of incomplete data. */
                Ras_SetDataPointer(deviceId, NULL);
#endif /* gRasRRSP_d */
#endif /* gAppRasDataTransfer_d */
            }

            if (maAppLclState[deviceId] == gAppLclReceivingMeasData_c)
            {
                /* Save results */
                result = processCsResultsContinueEvent(pSubeventResultContinue);

                if (result == gBleSuccess_c)
                {
                    /* Procedure done - success */
                    switch (pSubeventResultContinue->procedureDoneStatus)
                    {
                        case (uint8_t)gCsCompleteResults_c:
                        {
                            /* All results complete for the CS procedure */
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
                            if (gCsTimeInfo.csDistMeasStart != 0U)
                            {
                                /* Distance measurement duration for the first procedure. */
                                gCsTimeInfo.csDistMeasDuration = TM_GetTimestamp() - gCsTimeInfo.csDistMeasStart;
                                gCsTimeInfo.csDistMeasStart = 0;
                            }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRRSP_d) && (gRasRRSP_d == 1U)
                            Ras_SetDataPointer(deviceId, &mResultData[pSubeventResultContinue->deviceId]);
                            /* Send real-time data after the subevent is completed */
                            if (Ras_CheckRealTimeData(deviceId) == FALSE)
                            {
                                result = Ras_SendDataReady(pSubeventResultContinue->deviceId);
                            }

                            if (result != gBleSuccess_c)
                            {
                                AppLocalizationError(pSubeventResultContinue->deviceId, gAppLclRasSendIndicationFailed_c);
                            }
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
                            else
                            {
                                gCsTimeInfo.transferStart = TM_GetTimestamp();
                            }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */
#endif /* defined (gRasRRSP_d) && (gRasRRSP_d == 1U) */
#elif defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
                                gCsTimeInfo.transferStart = TM_GetTimestamp();
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */
                            /* Start sending L2CAP data */
                            (void)BtcsServer_SendData(pSubeventResultContinue->deviceId,
                                                      maPsmChannels[pSubeventResultContinue->deviceId],
                                                      gRangingProcResStart_c);
#endif /* gAppRasDataTransfer_d */

#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
                            if ((maCsProcCount[deviceId] > 0U) &&
                                (maAlgoRunCount[deviceId] != maCsProcCount[deviceId]))
                            {
                                AppLocalizationError(deviceId, gAppLclAlgoNotRun_c);
                                maAlgoRunCount[deviceId] = maCsProcCount[deviceId];
                            }
#endif /* gAppRunAlgo_d */

                            maCsProcCount[deviceId]++;

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
                            /* Local procedure is over - Wait for RAS transfer */
                            maAppLclState[deviceId] = gAppRasTransfInProgress_c;
#endif /* gRasRREQ_d */
#endif /* gAppRasDataTransfer_d */
#if (defined (gRasRRSP_d) && (gRasRRSP_d == 1U)) || \
    (defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U))
                            if (maCsProcCount[deviceId] == mRangeSettings[deviceId].maxNumProcedures)
                            {
                                maAppLclState[deviceId] = gAppLclIdle_c;
                            }
                            else
                            {
                                maAppLclState[deviceId] = gAppLclWaitingForMeasData_c;
                            }
#endif

                            if (mpfAppCsCallback != NULL)
                            {
                                mpfAppCsCallback(deviceId, NULL, gLocalMeasurementComplete_c);
                            }
                        }
                        break;

                        case (uint8_t)gCsPartialResults_c:
                        {
                            /* Partial results with more to follow for the CS procedure */
                        }
                        break;

                        case (uint8_t)gCsNoResultsProcAborted_c:
                        {
                            /* All subsequent CS procedures aborted */
                            maCsProcCount[deviceId] = 0U;
                            maAppLclState[deviceId] = gAppLclIdle_c;
                            if (mpfAppCsCallback != NULL)
                            {
                                mpfAppCsCallback(deviceId, (void*)&pSubeventResultContinue->abortReason, gErrorProcedureAborted_c);
                            }
                        }
                        break;

                        default:
                        {
                            /* Procedure error! */
                            if (maCsProcCount[deviceId] == mRangeSettings[deviceId].maxNumProcedures)
                            {
                                maAppLclState[deviceId] = gAppLclIdle_c;
                            }
                            else
                            {
                                maAppLclState[deviceId] = gAppLclWaitingForMeasData_c;
                            }
                            AppLocalizationError(deviceId, gAppLclProcStatusFailed_c);
                        }
                        break;
                    }
                }
                else
                {
                    /* Error occured! */
                    AppLocalizationError(deviceId, gAppLclErrorProcessingSubevent_c);
                }
            }
            else
            {
                /* Unexpected event. */
                AppLocalizationError(deviceId, gAppLclUnexpectedSRCE_c);
            }

            /* Free Event Data */
            (void)MEM_BufferFree(pSubeventResultContinue->pData);

        }
        break;

        case gCsMetaEvtProcedureEnableComplete_c:
        {
            csProcedureEnableCompleteEvent_t* pProcEnableComplete = (csProcedureEnableCompleteEvent_t*)pPacket->pEventData;
            deviceId = pProcEnableComplete->deviceId;

            /* Update number of procedures and reset internal counters */
            mRangeSettings[deviceId].maxNumProcedures = pProcEnableComplete->procedureCount;

#if defined (gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
            maAlgoRunCount[deviceId] = 0U;
#endif
            maCsProcCount[deviceId] = 0U;

            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    result = gBleOutOfMemory_c;
                }
            }

            if (result == gBleSuccess_c)
            {
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
                gCsTimeInfo.subeventInterval = pProcEnableComplete->subeventInterval;
                gCsTimeInfo.subeventLen = Utils_ExtractThreeByteValue(pProcEnableComplete->subeventLen);
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

#if defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
                BtcsServer_SetServerCfg(deviceId, mResultData);
#endif /* defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U) */

                /* Wait for measurement data. */
                maAppLclState[deviceId] = gAppLclWaitingForMeasData_c;

                mResultData[deviceId].selectedTxPower = ((int8_t)pProcEnableComplete->selectedTxPower);

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
                gCsTimeInfo.csDistMeasStart = TM_GetTimestamp();
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

                if (mGlobalRangeSettings.role == gCsRoleReflector_c)
                {
                    if (mpfAppCsCallback != NULL)
                    {
                        mpfAppCsCallback(deviceId, NULL, gDistanceMeastStarted_c);
                    }
                }
            }
        }
        break;

        case gCsMetaEvtError_c:
        {
            csErrorEvent_t* pCsMetaEvtError = (csErrorEvent_t*)pPacket->pEventData;
            deviceId = pCsMetaEvtError->deviceId;

            /* Set idle state once for all error cases */
            maAppLclState[deviceId] = gAppLclIdle_c;

            switch (pCsMetaEvtError->csErrorSource)
            {
                case readRemoteSupportedCapabilitiesComplete:
                {
                    /* An error occured during the configuration phase. */
                    AppLocalizationError(deviceId, gAppLclErrorRRSCCC_c);
                }
                break;

                case securityEnableComplete:
                {
                    /* An error occured during the configuration phase. */
                    AppLocalizationError(deviceId, gAppLclErrorSEC_c);
                }
                break;

                case readRemoteFAETableComplete:
                {
                    /* An error occurred during read remote FAE table. */
                    AppLocalizationError(deviceId, gAppLclErrorRRFAETC_c);
                }
                break;

                case configComplete:
                {
                    /* An error occurred during configuration. */
                    AppLocalizationError(deviceId, gAppLclErrorCC_c);
                }
                break;

                case procedureEnableComplete:
                {
                    /* An error occurred during procedure enable. */
                    AppLocalizationError(deviceId, gAppLclErrorPEC_c);
                }
                break;

                case eventResult:
                {
                    /* An error occurred during event result processing. */
                    AppLocalizationError(deviceId, gAppLclErrorERE_c);
                }
                break;

                case eventResultContinue:
                {
                    /* An error occurred during event result continue processing. */
                    AppLocalizationError(deviceId, gAppLclErrorERCE_c);
                }
                break;

                default:
                {
                    ; /* No action required */
                }
                break;
            }
        }
        break;

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0)
        case gCsMetaEvtHciDataLog_c:
        {
            if (mpfAppCsCallback != NULL)
            {
                /* Send HCI data logging event to the application. */
                mpfAppCsCallback(deviceId, (void*)pPacket->pEventData, gCsHciDataLogEvent_c);
            }
        }
        break;
#endif /* defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 0) */

        default:
        {
            ; /* No action required */
        }
        break;
    }

    /* Send event to the application. */
    if (mpfAppCsCallback != NULL)
    {
        mpfAppCsCallback(deviceId, (void*)pPacket, gCsMetaEvent_c);
    }

    /* Free packet data and packet */
    (void)MEM_BufferFree(pPacket->pEventData);
    (void)MEM_BufferFree(pPacket);
}

/*! *********************************************************************************
*\fn           bleResult_t csEventCmdCompleteCallback(csCommandCompleteEvent_t* pPacket)
*
*\brief        Sends Command Complete Events from the channel sounding lib to the app.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
static bleResult_t csEventCmdCompleteCallback
(
    csCommandCompleteEvent_t* pPacket
)
{
    bleResult_t result = gBleSuccess_c;
    appMsgFromHost_t *pMsgIn = NULL;
    uint32_t msgLen = (uint32_t)&(pMsgIn->msgData) + sizeof(void*);

    csCommandCompleteEvent_t *pCsCmdCompleteEvent = MEM_BufferAlloc(sizeof(csCommandCompleteEvent_t));
    if (pCsCmdCompleteEvent != NULL)
    {
        FLib_MemCpy(pCsCmdCompleteEvent, pPacket, sizeof(csCommandCompleteEvent_t));

        pMsgIn = MSG_Alloc(msgLen);
        if (pMsgIn != NULL)
        {
            pMsgIn->msgType = gAppCsCmdCompleteEventMsg_c;
            pMsgIn->msgData.pCsEventData = pCsCmdCompleteEvent;

            /* Put message in the Host Stack to App queue */
            (void)MSG_QueueAddTail(&mHostAppInputQueue, pMsgIn);

            /* Signal application */
            (void)OSA_EventSet(mAppEvent, gAppEvtMsgFromHostStack_c);
        }
        else
        {
            (void)MEM_BufferFree(pCsCmdCompleteEvent);
        }
    }
    else
    {
        result = gBleOutOfMemory_c;
    }

    return result;
}

/*! *********************************************************************************
*\fn           void AppLocalization_CSCmdCompleteCallback(void* pCsCmdCompleteEvent)
*
*\brief        Handles CS Cmd Complete Events from the channel sounding lib.
*
*\retval       none.
********************************************************************************** */
static void AppLocalization_CSCmdCompleteCallback
(
    void* pCsCmdCompleteEvent
)
{
    bleResult_t result = gBleSuccess_c;

    csCommandCompleteEvent_t *pPacket = (csCommandCompleteEvent_t*)pCsCmdCompleteEvent;

    switch (pPacket->eventType)
    {
        case readLocalSupportedCapabilities_c:
        {
            mGlobalRangeSettings.t_sw_local = pPacket->eventData.csReadLocalSupportedCapabilities.TSWtimeSupported;

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
            rttSoundingSupported = (pPacket->eventData.csReadLocalSupportedCapabilities.RTTSoundingN != 0U) ? TRUE:FALSE;
#endif /* gRasRREQ_d */
#endif /* gAppRasDataTransfer_d */
        }
        break;

        case setProcedureParameters_c:
        {
            deviceId_t deviceId = pPacket->deviceId;

            if (maAppLclState[deviceId] != gAppLclWaitingForSPPCC_c)
            {
                /* Unexpected event. */
                AppLocalizationError(deviceId, gAppLclUnexpectedSPP_c);
            }
            else
            {
                maAppLclState[deviceId] = gAppLclIdle_c;

                if (mpfAppCsCallback != NULL)
                {
                    mpfAppCsCallback(deviceId, NULL, gSetProcParamsComplete_c);
                }
            }
        }
        break;

        case setDefaultSettings_c:
        {
            deviceId_t deviceId = pPacket->deviceId;

            if (mGlobalRangeSettings.role == gCsRoleInitiator_c)
            {
                appLocalization_State_t prevState = maAppLclState[deviceId];
                maAppLclState[deviceId] = gAppLclIdle_c;

                if (prevState == gAppLclWaitingForSDSCC_c)
                {
                    (void)AppLocalization_CreateConfig(deviceId, mRangeSettings[deviceId].configId, TRUE);
                }
                else if (prevState == gAppLclWaitingForSDSCCWC_c)
                {
                    (void)AppLocalization_CreateConfig(deviceId, mRangeSettings[deviceId].configId, FALSE);
                }
                else
                {
                    /* Unexpected event. */
                    AppLocalizationError(deviceId, gAppLclUnexpectedSDS_c);
                }
            }
        }
        break;

        case writeCachedRemoteCapabilities_c:
        {
            deviceId_t deviceId = pPacket->deviceId;

            if (maAppLclState[deviceId] == gAppLclWaitingForWCCC_c)
            {
               uint8_t syncAntennaSelection = 0xFF; /* proprietary antenna shuffling */
               int8_t  maxTxPowerLevel = 10; /* 10 dBm */

               result = CS_SetDefaultSettings(deviceId,
                                              (csRoleType)(((uint8_t)gEnableCsInitiator_c) | ((uint8_t)gEnableCsReflector_c)),
                                              syncAntennaSelection,
                                              maxTxPowerLevel);

                if (result != gBleSuccess_c)
                {
                    maAppLclState[deviceId] = gAppLclIdle_c;
                    AppLocalizationError(deviceId, gAppLclSDSConfigError_c);
                }
                else
                {
                    maAppLclState[deviceId] = gAppLclWaitingForSDSCCWC_c;
                }
            }
            else
            {
                /* Unexpected event. */
                AppLocalizationError(deviceId, gAppLclUnexpectedWCCC_c);
            }

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
            if (mGlobalRangeSettings.role == gCsRoleReflector_c)
            {
                gCsTimeInfo.csConfigStartTs = TM_GetTimestamp();
            }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */
        }
        break;

        case commandError_c:
        {
            switch (pPacket->eventData.csCommandError.errorSource)
            {
                case csReadLocalSupportedCapabilities:
                {
                    /* Error occured during initialization. */
                    AppLocalizationError(gInvalidDeviceId_c, gAppLclErrorRLSC_c);
                }
                break;

                default:
                {
                    ; /* No action required */
                }
                break;
            }
        }
        break;

        default:
        {
            ; /* Do nothing. */
        }
        break;
    }

    if (mpfAppCsCallback != NULL)
    {
        mpfAppCsCallback(pPacket->deviceId, (void*)pPacket, gCsCcEvent_c);
    }

    (void)MEM_BufferFree(pPacket);
}

/*! *********************************************************************************
*\fn           bleResult_t csEventCmdStatusCallback(csCommandStatusEvent_t* pPacket)
*
*\brief        Sends Command Status Events from the channel sounding lib to the app.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
static bleResult_t csEventCmdStatusCallback
(
    csCommandStatusEvent_t* pPacket
)
{
    bleResult_t result = gBleSuccess_c;
    appMsgFromHost_t *pMsgIn = NULL;
    uint32_t msgLen = (uint32_t)&(pMsgIn->msgData) + sizeof(void*);

    csCommandStatusEvent_t *pCsCmdStatusEvent = MEM_BufferAlloc(sizeof(csCommandStatusEvent_t));
    if (pCsCmdStatusEvent != NULL)
    {
        FLib_MemCpy(pCsCmdStatusEvent, pPacket, sizeof(csCommandStatusEvent_t));

        pMsgIn = MSG_Alloc(msgLen);
        if (pMsgIn != NULL)
        {
            pMsgIn->msgType = gAppCsCmdStatusEventMsg_c;
            pMsgIn->msgData.pCsEventData = pCsCmdStatusEvent;

            /* Put message in the Host Stack to App queue */
            (void)MSG_QueueAddTail(&mHostAppInputQueue, pMsgIn);

            /* Signal application */
            (void)OSA_EventSet(mAppEvent, gAppEvtMsgFromHostStack_c);
        }
        else
        {
            (void)MEM_BufferFree(pCsCmdStatusEvent);
        }
    }
    else
    {
        result = gBleOutOfMemory_c;
    }

    return result;
}

/*! *********************************************************************************
*\fn           void AppLocalization_CSCmdStatusCallback(void* pCsCmdStatusEvent)
*
*\brief        Handles CS Cmd Status Events from the channel sounding lib.
*
*\retval       none.
********************************************************************************** */
static void AppLocalization_CSCmdStatusCallback
(
    void* pCsCmdStatusEvent
)
{
    deviceId_t deviceId = gInvalidDeviceId_c;

    csCommandStatusEvent_t *pPacket = (csCommandStatusEvent_t*)pCsCmdStatusEvent;
    switch (pPacket->eventType)
    {
        case csReadRemoteSupportedCapabilities:
        {
            /* Find the peer device for which this event is expected. The command
            status doesn't contain a connection identifier. */
            deviceId = GetDeviceIdInState(gAppLclWaitingForRRSC_c);

            if (deviceId == gInvalidDeviceId_c)
            {
                /* Unexpected event. */
                AppLocalizationError(gInvalidDeviceId_c, gAppLclInvalidDeviceId_c);
            }
            else if (pPacket->status != gHciSuccess_c)
            {
                /* An error occured during the configuration phase. */
                maAppLclState[deviceId] = gAppLclIdle_c;
                AppLocalizationError(deviceId, gAppLclRRSCError_c);
            }
            else
            {
                /* Wait for Read Remote Supported Capabilities Meta Event. */
                maAppLclState[deviceId] = gAppLclWaitingForRRSCC_c;
            }
        }
        break;

        case csSecurityEnable:
        {
            /* Find the peer device for which this event is expected. The command
            status doesn't contain a connection identifier. */
            deviceId = GetDeviceIdInState(gAppLclWaitingForSECS_c);

            if (deviceId == gInvalidDeviceId_c)
            {
                /* Unexpected event. */
                AppLocalizationError(gInvalidDeviceId_c, gAppLclInvalidDeviceId_c);
            }
            else if (pPacket->status != gHciSuccess_c)
            {
                /* An error occured during the configuration phase. */
                maAppLclState[deviceId] = gAppLclIdle_c;
                AppLocalizationError(deviceId, gAppLclSEError_c);
            }
            else
            {
                /* Wait for Security Enable Complete event. */
                maAppLclState[deviceId] = gAppLclWaitingForSEC_c;
            }
        }
        break;

        case csCreateConfig:
        {
            /* Find the peer device for which this event is expected. The command
            status doesn't contain a connection identifier. */
            deviceId = GetDeviceIdInState(gAppLclWaitingForCCCS_c);

            if (deviceId == gInvalidDeviceId_c)
            {
                deviceId = GetDeviceIdInState(gAppLclWaitingForLocCfg_c);

                if (deviceId == gInvalidDeviceId_c)
                {
                    /* Unexpected event. */
                    AppLocalizationError(gInvalidDeviceId_c, gAppLclInvalidDeviceId_c);
                }
                else if (pPacket->status != gHciSuccess_c)
                {
                    /* An error occured during the configuration phase. */
                    maAppLclState[deviceId] = gAppLclIdle_c;
                    AppLocalizationError(deviceId, gAppLclCCError_c);
                }
                else
                {
                    /* No action needed */
                }
            }
            else
            {
                if (pPacket->status != gHciSuccess_c)
                {
                    /* An error occured during the configuration phase. */
                    maAppLclState[deviceId] = gAppLclIdle_c;
                    AppLocalizationError(deviceId, gAppLclCCError_c);
                }
                else
                {
                    maAppLclState[deviceId] = gAppLclWaitingForCC_c;
                }
            }
        }
        break;

        case csProcedureEnable:
        {
            /* Find the peer device for which this event is expected. The command
            status doesn't contain a connection identifier. */
            deviceId = GetDeviceIdInState(gAppLclWaitingForPECS_c);

            if (deviceId == gInvalidDeviceId_c)
            {
                /* Unexpected event. */
                AppLocalizationError(gInvalidDeviceId_c, gAppLclInvalidDeviceId_c);
            }
            else if (pPacket->status != gHciSuccess_c)
            {
                /* An error occured during procedure enable. */
                maAppLclState[deviceId] = gAppLclIdle_c;
                AppLocalizationError(deviceId, gAppLclStartMeasurementFail_c);
            }
            else
            {
                /* Wait for Procedure Enable Complete event. */
                maAppLclState[deviceId] = gAppLclWaitingForPEC_c;
            }
        }
        break;

        default:
        {
             ; /* Do nothing. */
        }
        break;
    }

    if (mpfAppCsCallback != NULL)
    {
        mpfAppCsCallback(deviceId, (void*)pPacket, gCsStatusEvent_c);
    }

    (void)MEM_BufferFree(pPacket);
}

/*! *********************************************************************************
*\fn           static bleResult_t processCsResultsEvent(csSubeventResultEvent_t* pEvent)
*
*\brief        Handles Channel Sounding Results Meta event.
*
*\param  [in]  pEvent       Pointer to Channel Souding Results event.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
static bleResult_t processCsResultsEvent
(
    csSubeventResultEvent_t* pEvent
)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t eventIdx = 0;
    deviceId_t deviceId = pEvent->deviceId;
    uint8_t subeventIndex = mResultData[deviceId].subeventIndex;
    csRasSubeventHeader_t *pSubevtHeader = &mResultData[deviceId].aSubEventData[subeventIndex].subevtHeader;

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined(gRasRREQ_d) && (gRasRREQ_d == 1U)
    rreqTimeoutData_t mRreqTimeoutData;
#endif /* gRasRREQ_d*/
#endif /* gAppRasDataTransfer_d*/

    if (pEvent->subeventDoneStatus == (uint8_t)gCsNoResultsProcAborted_c)
    {
        maCsProcCount[deviceId]++;
#if defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
        maAlgoRunCount[deviceId]++;
#endif

        if (mpfAppCsCallback != NULL)
        {
            mpfAppCsCallback(deviceId, (void*)&pEvent->abortReason, gErrorSubeventAborted_c);
        }
    }

    /* Set results data parameters */
    if (mResultData[deviceId].dataIndex == 0U)
    {
        mResultData[deviceId].deviceId = pEvent->deviceId;
        mResultData[deviceId].configId = pEvent->configId;
        mResultData[deviceId].procedureCounter = pEvent->procedureCounter;
        mResultData[deviceId].numAntennaPaths = pEvent->numAntennaPaths;

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined(gRasRREQ_d) && (gRasRREQ_d == 1U)
        /* First results event - Expecting Data Ready from peer */
        mRreqTimeoutData.deviceId = deviceId;
        mRreqTimeoutData.reason = (uint8_t)rreqWaitingForDataReady_c;
        /* Start Data Ready (On demad)/Real-time data timer */
        RasClient_SartRapTimer(deviceId, mRreqTimeoutData);
#endif /* gRasRREQ_d */
#endif /* gAppRasDataTransfer_d */
    }

    /* Update subevent information */
    pSubevtHeader->startACLConnEvent = pEvent->startACLConnEvent;
    pSubevtHeader->frequencyCompensation = pEvent->frequencyCompensation;
    pSubevtHeader->referencePowerLevel = pEvent->referencePowerLevel;
    pSubevtHeader->procedureDoneStatus = pEvent->procedureDoneStatus;
    pSubevtHeader->subeventDoneStatus = pEvent->subeventDoneStatus;
    pSubevtHeader->abortReason = pEvent->abortReason;
    pSubevtHeader->numStepsReported = pEvent->numStepsReported;

    mResultData[deviceId].totalNumSteps += pEvent->numStepsReported;

    /* Use free bits of packet_AA_quality of first mode0 step to store eventIdx (we do not keep track of all subevent headers) */
    eventIdx = pEvent->startACLConnEvent - pSubevtHeader->startACLConnEvent;
    *(pEvent->pData + 3U * sizeof(uint8_t)) |= (uint8_t)(eventIdx << CS_EVTIDX_SHIFT);

    result = processEventResultData(deviceId, pEvent->numStepsReported,
                                    pEvent->subeventDoneStatus, pEvent->procedureDoneStatus, pEvent->pData);

    return result;
}

/*! *********************************************************************************
*\fn           static bleResult_t processCsResultsContinueEvent(csSubeventResultContinueEvent_t* pEvent)
*
*\brief        Handles Channel Sounding Results Continue Meta event.
*
*\param  [in]  pEvent       Pointer to Channel Souding Results Continue event.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
static bleResult_t processCsResultsContinueEvent
(
    csSubeventResultContinueEvent_t* pEvent
)
{
    bleResult_t result = gBleSuccess_c;
    deviceId_t deviceId = pEvent->deviceId;
    uint8_t subeventIndex = mResultData[deviceId].subeventIndex;
    csRasSubeventHeader_t *pSubevtHeader = &mResultData[deviceId].aSubEventData[subeventIndex].subevtHeader;

    if (pEvent->subeventDoneStatus == (uint8_t)gCsNoResultsProcAborted_c)
    {
        maCsProcCount[deviceId]++;
#if defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
        maAlgoRunCount[deviceId]++;
#endif
        if (mpfAppCsCallback != NULL)
        {
            mpfAppCsCallback(deviceId, (void*)&pEvent->abortReason, gErrorSubeventAborted_c);
        }
    }

    /* Update results data parameters */
    mResultData[deviceId].totalNumSteps += pEvent->numStepsReported;
    pSubevtHeader->subeventDoneStatus = pEvent->subeventDoneStatus;
    pSubevtHeader->procedureDoneStatus = pEvent->procedureDoneStatus;
    pSubevtHeader->numStepsReported += pEvent->numStepsReported;

    result = processEventResultData(deviceId, pEvent->numStepsReported,
                                    pEvent->subeventDoneStatus, pEvent->procedureDoneStatus, pEvent->pData);

    return result;
}

/*! *********************************************************************************
*\fn           static bleResult_t processEventResultData(deviceId_t deviceId, uint8_t nbSteps,
*              uint8_t subEventStatus, uint8_t procDoneStatus, uint8_t *pEventData)
*
*\brief        Process Channel Sounding measurement result data.
*
*\param  [in]  deviceId      Device ID.
*\param  [in]  nbSteps         Number of steps included in measurement data.
*\param  [in]  subEventStatus   SubEvent status.
*\param  [in]  procDoneStatus   Procedure done status
*\param  [in]  pEventData       Pointer to Channel Souding measurement data.
*
*\retval       bleResult_t      Result of the operation.
********************************************************************************** */
static bleResult_t processEventResultData
(
    deviceId_t deviceId,
    uint8_t nbSteps,
    uint8_t subEventStatus,
    uint8_t procDoneStatus,
    uint8_t *pEventData
)
{
    bleResult_t result = gBleSuccess_c;
    uint32_t dataSize = 0;
    uint8_t *stepDataLenPtr = pEventData + 2U * sizeof(uint8_t); /* Skip mode, channel */
    uint8_t subeventIndex = mResultData[deviceId].subeventIndex;

    /* Compute results data size */
    for (uint32_t i = 0; i < nbSteps; i++)
    {
        dataSize += ((uint32_t)*(stepDataLenPtr + dataSize)) + sizeof(uint8_t) * 3U; /* also account for mode, channel, data_length fields */
    }

    if (((mResultData[deviceId].dataIndex + dataSize) >= gMeasurementBufferSize_c) ||
        (subeventIndex >= APP_LOCALIZATION_MAX_SUBEVENTS))
    {
        result = gBleOutOfMemory_c;
        AppLocalizationError(deviceId, gAppLclNoSubeventMemoryAvailable_c);
    }
    else
    {
        /* Increment subevent data size */
        mResultData[deviceId].aSubEventData[subeventIndex].dataSize += (uint16_t)dataSize;
        /* Add results to buffer */
        FLib_MemCpy(&mResultData[deviceId].pData[mResultData[deviceId].dataIndex], pEventData, dataSize);
        mResultData[deviceId].dataIndex += dataSize;

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRRSP_d) && (gRasRRSP_d == 1U)
        /* Parse new subevent data */
        Ras_SetDataPointer(deviceId, &mResultData[deviceId]);

        /* This is continuation data for the current subevent */
        if ((subeventIndex == 0U) &&
            (mResultData[deviceId].aSubEventData[subeventIndex].dataSize == (uint16_t)dataSize))
        {
            if (subEventStatus == (uint8_t)gCsCompleteResults_c ||
                subEventStatus == (uint8_t)gCsNoResultsProcAborted_c)
            {
                /* The first chunk includes a complete subevent, pack its header as well */
                result = Ras_BuildRangingDataBody(deviceId, TRUE, TRUE);
            }
            else
            {
                /* The first chunk does not include a complete subevent, pack only the procedure header */
                result = Ras_BuildRangingDataBody(deviceId, TRUE, FALSE);
            }
        }
        else if (subEventStatus == (uint8_t)gCsCompleteResults_c ||
                 subEventStatus == (uint8_t)gCsNoResultsProcAborted_c)
        {
            /* The subevent is complete, can pack its header */
            result = Ras_BuildRangingDataBody(deviceId, FALSE, TRUE);
        }
        else
        {
            /* Intermediate chunk, no header to be packed */
            result = Ras_BuildRangingDataBody(deviceId, FALSE, FALSE);
        }
#endif /* gRasRRSP_d */
#elif defined(gAppBtcsServer_d) && (gAppBtcsServer_d == 1U)
        if ((subeventIndex == 0U) &&
            (mResultData[deviceId].aSubEventData[subeventIndex].dataSize == (uint16_t)dataSize))
        {
            result = BtcsServer_BuildRangingData(deviceId, nbSteps, gCsProcHeader_c);
        }
        else if ((subeventIndex > 0U) &&
            (mResultData[deviceId].aSubEventData[subeventIndex].dataSize == (uint16_t)dataSize))
        {
            result = BtcsServer_BuildRangingData(deviceId, nbSteps, gCsSubEvtHeader_c);
        }
        else
        {
            result = BtcsServer_BuildRangingData(deviceId, nbSteps, gCsSubEvtContHeader_c);
        }
#endif /* gAppRasDataTransfer_d */

        if (((subEventStatus == (uint8_t)gCsCompleteResults_c) ||
             (subEventStatus == (uint8_t)gCsNoResultsProcAborted_c)) &&
            (mResultData[deviceId].subeventIndex < APP_LOCALIZATION_MAX_SUBEVENTS)
           )
        {
            /* Subevent Done - move to next subevent */
            if (procDoneStatus == (uint8_t)gCsPartialResults_c)
            {
                mResultData[deviceId].subeventIndex++;
                subeventIndex = mResultData[deviceId].subeventIndex;
                /* Set data index for next sub event. There is no need to set the data index for the first sub event as it is 0*/
                mResultData[deviceId].aSubEventData[subeventIndex].dataIdx = mResultData[deviceId].dataIndex;
            }
            else
            {
#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#if defined (gRasRRSP_d) && (gRasRRSP_d == 1U)
                /* Send real-time data after the subevent is completed */
                if ((Ras_CheckRealTimeData(deviceId)) && (result == gBleSuccess_c))
                {
                    Ras_ClearDataForTransfer(deviceId);

                    if ((Ras_GetDataSendPreference(deviceId) & BIT3) == 0U)
                    {
                        /* Peer configured transfer through notifications */
                        result = Ras_SendRangingDataNotifs(deviceId, mRasServiceConfig.realTimeDataHandle);
                    }
                    else
                    {
                        /* Peer configured transfer through indications */
                        result = Ras_SendRangingDataIndication(deviceId, mRasServiceConfig.realTimeDataHandle);
                    }
                }
#endif /* gRasRRSP_d */
#endif /* gAppRasDataTransfer_d */
            }
         }
    }

    return result;
}

/*! *********************************************************************************
*\fn         static void AppLocalizationError(appLocalizationError_t error)
*
*\brief      Send error event to the application.
*
*\param[in]  handle         Handle value
*
*\retval     none
********************************************************************************** */
static void AppLocalizationError
(
    deviceId_t deviceId,
    appLocalizationError_t error
)
{
    if (mpfAppCsCallback != NULL)
    {
        mpfAppCsCallback(deviceId, (void*)&error, gErrorEvent_c);
    }
}

/*! *********************************************************************************
*\fn           static deviceId_t GetDeviceIdInState(appLocalization_State_t state)
*
*\brief        Returns peer device id which is found in a specified localization state.
*
*\param  [in]  state    State to look for.
*
*\retval       deviceId_t   Device id if found, gInvalidDeviceId_c otherwise.
********************************************************************************** */
static deviceId_t GetDeviceIdInState
(
    appLocalization_State_t state
)
{
    deviceId_t deviceId = gInvalidDeviceId_c;

    for (uint8_t i = 0; i < (uint8_t)gAppMaxConnections_c; i++)
    {
        if (maAppLclState[i] == state)
        {
            deviceId = i;
            break;
        }
    }

    return deviceId;
}

/*! *********************************************************************************
*\fn           static  void TemperatureTimerCallback(void *pParam)
*
*\brief        Timer callback. Refreshes the temperature value and informs the NBU.
*
*\param  [in]  pParam       Parameter for the callback
*
*\retval       none
********************************************************************************** */
static void TemperatureTimerCallback
(
    void *pParam
)
{
    (void)pParam;

    static bool_t bWaitingForRefresh = FALSE;

    if (!bWaitingForRefresh)
    {
        /* Trigger measurement */
        SENSORS_TriggerTemperatureMeasurement();
        bWaitingForRefresh = TRUE;

        /* Reschedule for gRefreshTime_c to read the result */
        (void)TM_Start((timer_handle_t)mTemperatureTimerId,
                       kTimerModeSingleShot | kTimerModeLowPowerTimer, gRefreshTime_c);
    }
    else
    {
        /* Read measurement result */
        (void)SENSORS_RefreshTemperatureValue();
        bWaitingForRefresh = FALSE;

        /* Reschedule for next measurement cycle */
        (void)TM_Start((timer_handle_t)mTemperatureTimerId,
                       kTimerModeSingleShot | kTimerModeLowPowerTimer, gTemperaturePollingInterval_c);
    }
}

#if defined(gAppRunAlgo_d) && (gAppRunAlgo_d == 1U)
/*! *********************************************************************************
*\fn            void AppLocalization_RunAlgorithm(deviceId_t deviceId);
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
)
{
#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
   /* Parse header of the ranging data body */
    uint16_t peerProcCount = 0U;
    RasClient_ParseDataHeader(deviceId);
    rasMeasurementData_t* mpPeerResultData = RasClient_GetPeerRangingData(deviceId);
    peerProcCount = RasClient_GetPeerProcCount(deviceId);

    /* Clear transfer data */
    RasClient_ResetRasTransferInfo(deviceId);

    /* Only compare the lower 12 bits of the counter, RAS truncates the original 16-bit value */
    if ((mResultData[deviceId].procedureCounter & 0x0FFFU) == peerProcCount)
    {
        if (RasClient_GetRealTimeMode(deviceId) == FALSE)
        {
            (void)RasClient_SendRasCommand(deviceId, ackRangingDataOpCode_c,
                                           0U, 0U,
                                           mResultData[deviceId].procedureCounter,
                                           gAntennaPathFilterAllowAll_c);
        }
#elif defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
    rasMeasurementData_t* mpPeerResultData = BtcsClient_GetPeerRangingData(deviceId);
#endif
        localizationAlgoResult_t algoResult;
        FLib_MemSet(&algoResult, 0U, sizeof(localizationAlgoResult_t));
#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
        gCsTimeInfo.transferEnd = TM_GetTimestamp();
#endif
        algoResult.algorithm = gLocalizationAlgorithm;

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
        gCsTimeInfo.algoStart = TM_GetTimestamp();
#endif
        /* All data is received - feed into algorithm */
        maAlgoRunCount[deviceId]++;

        if ((gLocalizationAlgorithm > 0U) && (mResultData[deviceId].dataIndex > 0U))
        {
            AppLocalizationAlgo_RunMeasurement(deviceId,
                                               &mResultData[deviceId],
                                               mpPeerResultData,
                                               mGlobalRangeSettings.role,
                                               &algoResult);
        }

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
        gCsTimeInfo.algoEnd = TM_GetTimestamp();
        algoResult.csConfigDuration = gCsTimeInfo.csConfigEndTs - gCsTimeInfo.csConfigStartTs;
        algoResult.csProcedureDuration = gCsTimeInfo.csDistMeasDuration;
        algoResult.transferDuration = gCsTimeInfo.transferEnd - gCsTimeInfo.transferStart;
        algoResult.algoDuration = gCsTimeInfo.algoEnd - gCsTimeInfo.algoStart;
        gCsTimeInfo.csDistMeasDuration = 0;
        gCsTimeInfo.lastAclConnEvtCnt = 0;
        gCsTimeInfo.noOfRcvSubEvsInConnInt = 0;
        gCsTimeInfo.csConfigEndTs = 0UL;
        gCsTimeInfo.csConfigStartTs = 0UL;
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

        /* Update state */
        if (maCsProcCount[deviceId] == mRangeSettings[deviceId].maxNumProcedures)
        {
            maAppLclState[deviceId] = gAppLclIdle_c;
        }
        else
        {
            maAppLclState[deviceId] = gAppLclWaitingForMeasData_c;
        }

#if defined(gAppHciDataLogExport_d) && (gAppHciDataLogExport_d > 1)
        /* Print remote data */
        if (mpfAppCsCallback != NULL)
        {
            /* Send data logging event to the application. */
            mpfAppCsCallback(deviceId, (void*)mpPeerResultData->pData, gCsRemoteDataLogEvent_c);
        }
#endif

        /* Reset mResultData and mPeerResultData */
        FLib_MemSet(&mResultData[deviceId], 0U, sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
        FLib_MemSet(mResultData[deviceId].pData, 0U, gRasCsSubeventDataSize_c);

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
        RasClient_ResetPeerProcData(deviceId);
#elif defined(gAppBtcsClient_d) && (gAppBtcsClient_d == 1U)
        BtcsClient_ResetPeer(deviceId, FALSE);
#endif /* gAppRasDataTransfer_d */

        if (gLocalizationAlgorithm > 0U)
        {
            if (mpfDisplayResultCallback != NULL)
            {
                mpfDisplayResultCallback(deviceId,  &algoResult);
            }
        }
#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
    }
#endif
}
#endif /* gAppRunAlgo_d */

#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
/*! *********************************************************************************
*\fn         AppLocalization_RunPtsTest(deviceId_t deviceId, uint8_t index, uint8_t actionMask)
*
*\brief      Test vector parsing to process the data.
*
*\return     None
********************************************************************************** */
void AppLocalization_RunPtsTest(deviceId_t deviceId, uint8_t index, uint8_t actionMask)
{
    bool_t bSendDataReady       = (actionMask & BIT0) != 0U;
    bool_t bDataOverwrittenTest = (actionMask & BIT1) != 0U;

    switch(index)
    {
        case 1U: /* Test vector 1 - single_subevent_mode1_only */
        {
            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    break;
                }
            }

            mResultData[deviceId].selectedTxPower = 0x7F;
            mResultData[deviceId].subeventIndex = 0U;
            csSubeventResultEvent_t subeventResult = {
                .deviceId = deviceId,
                .configId = 1U,
                .startACLConnEvent = 0x003A,
                .procedureCounter = 0U,
                .frequencyCompensation = 0x00C0,
                .referencePowerLevel = (int8_t)0xF7,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x01,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 19,
                .pData = tv1,
            };
            (void)processCsResultsEvent(&subeventResult);

            csSubeventResultContinueEvent_t subeventResultContinue = {
                .deviceId = deviceId,
                .configId = 1U,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x01,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 19,
                .pData = &tv1[tv1Se0DataIndex],
            };
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            subeventResultContinue.procedureDoneStatus = 0x00;
            subeventResultContinue.subeventDoneStatus = 0x00;
            subeventResultContinue.abortReason = 0x00;
            subeventResultContinue.numAntennaPaths = 1U;
            subeventResultContinue.numStepsReported = 19;
            subeventResultContinue.pData = &tv1[tv1Se0DataIndex +
                                                tv1Se0c0DataIndex];
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            break;
        }

        case 2U: /* Test vector 2 - single_subevent_mode2_mode1 (actually two subevents) */
        {
            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    break;
                }
            }

            mResultData[deviceId].selectedTxPower = 0x7F;
            mResultData[deviceId].subeventIndex = 0U;
            csSubeventResultEvent_t subeventResult = {
                .deviceId = deviceId,
                .configId = 1U,
                .startACLConnEvent = 0x003A,
                .procedureCounter = 0U,
                .frequencyCompensation = 0x00C0,
                .referencePowerLevel = (int8_t)0xFF,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x01,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 21,
                .pData = tv2,
            };
            (void)processCsResultsEvent(&subeventResult);

            csSubeventResultContinueEvent_t subeventResultContinue = {
                .deviceId = deviceId,
                .configId = 1U,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x01,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 20,
                .pData = &tv2[tv2Se0DataIndex],
            };
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            subeventResultContinue.procedureDoneStatus = 0x01;
            subeventResultContinue.subeventDoneStatus = 0x01;
            subeventResultContinue.abortReason = 0x00;
            subeventResultContinue.numAntennaPaths = 1U;
            subeventResultContinue.numStepsReported = 20;
            subeventResultContinue.pData = &tv2[tv2Se0DataIndex +
                                                tv2Se0c0DataIndex];
            (void)processCsResultsContinueEvent(&subeventResultContinue);


            subeventResultContinue.procedureDoneStatus = 0x01;
            subeventResultContinue.subeventDoneStatus = 0x00;
            subeventResultContinue.abortReason = 0x00;
            subeventResultContinue.numAntennaPaths = 1U;
            subeventResultContinue.numStepsReported = 20;
            subeventResultContinue.pData = &tv2[tv2Se0DataIndex +
                                                tv2Se0c0DataIndex +
                                                tv2Se0c1DataIndex];
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            mResultData[deviceId].subeventIndex = 1U;
            subeventResult.startACLConnEvent = 0x00B5;
            subeventResult.frequencyCompensation = 0xC000;
            subeventResult.referencePowerLevel = (int8_t)0xFE;
            subeventResult.procedureDoneStatus = 0x00;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 17;
            subeventResult.pData = &tv2[tv2Se0DataIndex +
                                        tv2Se0c0DataIndex +
                                        tv2Se0c1DataIndex +
                                        tv2Se0c2DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            break;
        }

        case 3U: /* Test vector 3 - single_subevent_mode2_only */
        {
            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    break;
                }
            }

            mResultData[deviceId].selectedTxPower = 0x7F;
            mResultData[deviceId].subeventIndex = 0U;
            csSubeventResultEvent_t subeventResult = {
                .deviceId = deviceId,
                .configId = 1U,
                .startACLConnEvent = 0x0039,
                .procedureCounter = 0U,
                .frequencyCompensation = 0xC000,
                .referencePowerLevel = (int8_t)0xF9,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x01,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 20,
                .pData = tv3,
            };
            (void)processCsResultsEvent(&subeventResult);

            csSubeventResultContinueEvent_t subeventResultContinue = {
                .deviceId = deviceId,
                .configId = 1U,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x01,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 19,
                .pData = &tv3[tv3Se0DataIndex],
            };
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            subeventResultContinue.procedureDoneStatus = 0x01;
            subeventResultContinue.subeventDoneStatus = 0x01;
            subeventResultContinue.abortReason = 0x00;
            subeventResultContinue.numAntennaPaths = 1U;
            subeventResultContinue.numStepsReported = 19;
            subeventResultContinue.pData = &tv3[tv3Se0DataIndex +
                                                tv3Se0c0DataIndex];
            (void)processCsResultsContinueEvent(&subeventResultContinue);


            subeventResultContinue.procedureDoneStatus = 0x00;
            subeventResultContinue.subeventDoneStatus = 0x00;
            subeventResultContinue.abortReason = 0x00;
            subeventResultContinue.numAntennaPaths = 1U;
            subeventResultContinue.numStepsReported = 16;
            subeventResultContinue.pData = &tv3[tv3Se0DataIndex +
                                                tv3Se0c0DataIndex +
                                                tv3Se0c1DataIndex];
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            break;
        }

        case 4U: /* Test vector 4 - single_subevent_mode3_only (actually two subevents) */
        {
            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    break;
                }
            }

            mResultData[deviceId].selectedTxPower = 0x7F;
            mResultData[deviceId].subeventIndex = 0U;
            csSubeventResultEvent_t subeventResult = {
                .deviceId = deviceId,
                .configId = 1U,
                .startACLConnEvent = 0x0039,
                .procedureCounter = 0U,
                .frequencyCompensation = 0xC000,
                .referencePowerLevel = (int8_t)0xF8,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x01,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 14,
                .pData = tv4,
            };
            (void)processCsResultsEvent(&subeventResult);

            csSubeventResultContinueEvent_t subeventResultContinue = {
                .deviceId = deviceId,
                .configId = 1U,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x01,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 13,
                .pData = &tv4[tv4Se0DataIndex],
            };
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            subeventResultContinue.procedureDoneStatus = 0x01;
            subeventResultContinue.subeventDoneStatus = 0x01;
            subeventResultContinue.abortReason = 0x00;
            subeventResultContinue.numAntennaPaths = 1U;
            subeventResultContinue.numStepsReported = 13;
            subeventResultContinue.pData = &tv4[tv4Se0DataIndex +
                                                tv4Se0c0DataIndex];
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            subeventResultContinue.procedureDoneStatus = 0x01;
            subeventResultContinue.subeventDoneStatus = 0x01;
            subeventResultContinue.abortReason = 0x00;
            subeventResultContinue.numAntennaPaths = 1U;
            subeventResultContinue.numStepsReported = 20;
            subeventResultContinue.pData = &tv4[tv4Se0DataIndex +
                                                tv4Se0c0DataIndex +
                                                tv4Se0c1DataIndex];
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            subeventResultContinue.procedureDoneStatus = 0x01;
            subeventResultContinue.subeventDoneStatus = 0x00;
            subeventResultContinue.abortReason = 0x00;
            subeventResultContinue.numAntennaPaths = 1U;
            subeventResultContinue.numStepsReported = 11;
            subeventResultContinue.pData = &tv4[tv4Se0DataIndex +
                                                tv4Se0c0DataIndex +
                                                tv4Se0c1DataIndex +
                                                tv4Se0c2DataIndex];
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            mResultData[deviceId].subeventIndex = 1U;
            subeventResult.startACLConnEvent = 0x0039;
            subeventResult.frequencyCompensation = 0xC000;
            subeventResult.referencePowerLevel = (int8_t)0xF8;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x01;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 14;
            subeventResult.pData = &tv4[tv4Se0DataIndex +
                                        tv4Se0c0DataIndex +
                                        tv4Se0c1DataIndex +
                                        tv4Se0c2DataIndex +
                                        tv4Se0c3DataIndex];
            (void)processCsResultsEvent(&subeventResult);


            subeventResultContinue.procedureDoneStatus = 0x00;
            subeventResultContinue.subeventDoneStatus = 0x00;
            subeventResultContinue.abortReason = 0x00;
            subeventResultContinue.numAntennaPaths = 1U;
            subeventResultContinue.numStepsReported = 1;
            subeventResultContinue.pData = &tv4[tv4Se0DataIndex +
                                                tv4Se0c0DataIndex +
                                                tv4Se0c1DataIndex +
                                                tv4Se0c2DataIndex +
                                                tv4Se0c3DataIndex +
                                                tv4Se1DataIndex];
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            break;
        }

        case 5U: /* Test vector 5 - multiple_subevents_mode1_only (7 subevents) */
        {
            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    break;
                }
            }

            mResultData[deviceId].selectedTxPower = 0x7F;
            mResultData[deviceId].subeventIndex = 0U;
            csSubeventResultEvent_t subeventResult = {
                .deviceId = deviceId,
                .configId = 1U,
                .startACLConnEvent = 0x00B0,
                .procedureCounter = 0U,
                .frequencyCompensation = 0x0000,
                .referencePowerLevel = (int8_t)0xF6,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x00,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 16,
                .pData = tv5,
            };
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 1U;
            subeventResult.startACLConnEvent = 0x00B1;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF8;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 16;
            subeventResult.pData = &tv5[tv5Se0DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 2U;
            subeventResult.startACLConnEvent = 0x00B2;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xFA;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 16;
            subeventResult.pData = &tv5[tv5Se0DataIndex +
                                        tv5Se1DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 3U;
            subeventResult.startACLConnEvent = 0x00B3;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF6;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 16;
            subeventResult.pData = &tv5[tv5Se0DataIndex +
                                        tv5Se1DataIndex +
                                        tv5Se2DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 4U;
            subeventResult.startACLConnEvent = 0x00B4;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF7;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 16;
            subeventResult.pData = &tv5[tv5Se0DataIndex +
                                        tv5Se1DataIndex +
                                        tv5Se2DataIndex +
                                        tv5Se3DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 5U;
            subeventResult.startACLConnEvent = 0x00B5;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF8;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 16;
            subeventResult.pData = &tv5[tv5Se0DataIndex +
                                        tv5Se1DataIndex +
                                        tv5Se2DataIndex +
                                        tv5Se3DataIndex +
                                        tv5Se4DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 6U;
            subeventResult.startACLConnEvent = 0x00B6;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF8;
            subeventResult.procedureDoneStatus = 0x00;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 8;
            subeventResult.pData = &tv5[tv5Se0DataIndex +
                                        tv5Se1DataIndex +
                                        tv5Se2DataIndex +
                                        tv5Se3DataIndex +
                                        tv5Se4DataIndex +
                                        tv5Se5DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            break;
        }

        case 6U: /* Test vector 6 - multiple_subevents_mode2_mode1 (7 subevents) */
        {
            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    break;
                }
            }

            mResultData[deviceId].selectedTxPower = 0x7F;
            mResultData[deviceId].subeventIndex = 0U;
            csSubeventResultEvent_t subeventResult = {
                .deviceId = deviceId,
                .configId = 1U,
                .startACLConnEvent = 0x008F,
                .procedureCounter = 0U,
                .frequencyCompensation = 0x0000,
                .referencePowerLevel = (int8_t)0xF7,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x00,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 18,
                .pData = tv6,
            };
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 1U;
            subeventResult.startACLConnEvent = 0x0090;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF9;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 18;
            subeventResult.pData = &tv6[tv6Se0DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 2U;
            subeventResult.startACLConnEvent = 0x0091;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF9;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 18;
            subeventResult.pData = &tv6[tv6Se0DataIndex +
                                        tv6Se1DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 3U;
            subeventResult.startACLConnEvent = 0x0092;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF9;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 18;
            subeventResult.pData = &tv6[tv6Se0DataIndex +
                                        tv6Se1DataIndex +
                                        tv6Se2DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 4U;
            subeventResult.startACLConnEvent = 0x0093;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF9;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 18;
            subeventResult.pData = &tv6[tv6Se0DataIndex +
                                        tv6Se1DataIndex +
                                        tv6Se2DataIndex +
                                        tv6Se3DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 5U;
            subeventResult.startACLConnEvent = 0x0094;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF8;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 18;
            subeventResult.pData = &tv6[tv6Se0DataIndex +
                                        tv6Se1DataIndex +
                                        tv6Se2DataIndex +
                                        tv6Se3DataIndex +
                                        tv6Se4DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 6U;
            subeventResult.startACLConnEvent = 0x0095;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF9;
            subeventResult.procedureDoneStatus = 0x00;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 15;
            subeventResult.pData = &tv6[tv6Se0DataIndex +
                                        tv6Se1DataIndex +
                                        tv6Se2DataIndex +
                                        tv6Se3DataIndex +
                                        tv6Se4DataIndex +
                                        tv6Se5DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            break;
        }

        case 7U: /* Test vector 7 - multiple_subevents_mode2_only (6 subevents) */
        {
            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    break;
                }
            }

            mResultData[deviceId].selectedTxPower = 0x7F;
            mResultData[deviceId].subeventIndex = 0U;
            csSubeventResultEvent_t subeventResult = {
                .deviceId = deviceId,
                .configId = 1U,
                .startACLConnEvent = 0x0091,
                .procedureCounter = 0U,
                .frequencyCompensation = 0x0000,
                .referencePowerLevel = (int8_t)0xF7,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x00,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 0x12,
                .pData = tv7,
            };
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 1U;
            subeventResult.startACLConnEvent = 0x0092;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xFB;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 0x12;
            subeventResult.pData = &tv7[tv7Se0DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 2U;
            subeventResult.startACLConnEvent = 0x0093;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF6;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 0x12;
            subeventResult.pData = &tv7[tv7Se0DataIndex +
                                        tv7Se1DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 3U;
            subeventResult.startACLConnEvent = 0x0094;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF8;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 0x12;
            subeventResult.pData = &tv7[tv7Se0DataIndex +
                                        tv7Se1DataIndex +
                                        tv7Se2DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 4U;
            subeventResult.startACLConnEvent = 0x0095;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF9;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 0x12;
            subeventResult.pData = &tv7[tv7Se0DataIndex +
                                        tv7Se1DataIndex +
                                        tv7Se2DataIndex +
                                        tv7Se3DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 5U;
            subeventResult.startACLConnEvent = 0x0096;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF6;
            subeventResult.procedureDoneStatus = 0x00;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 0x09;
            subeventResult.pData = &tv7[tv7Se0DataIndex +
                                        tv7Se1DataIndex +
                                        tv7Se2DataIndex +
                                        tv7Se3DataIndex +
                                        tv7Se4DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            break;
        }

        case 11U: /* Fall-through */
                  /* Test vector 11 - long procedure 1 aborted, start another procedure */
        {
            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    break;
                }
            }

            mResultData[deviceId].selectedTxPower = 0x7F;
            mResultData[deviceId].subeventIndex = 0U;
            csSubeventResultEvent_t subeventResult = {
                .deviceId = deviceId,
                .configId = 1U,
                .startACLConnEvent = 0x0091,
                .procedureCounter = 0U,
                .frequencyCompensation = 0x0000,
                .referencePowerLevel = (int8_t)0xF7,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x00,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 0x12,
                .pData = tv7,
            };
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 1U;
            subeventResult.startACLConnEvent = 0x0092;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xFB;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 0x12;
            subeventResult.pData = &tv7[tv7Se0DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 2U;
            subeventResult.startACLConnEvent = 0x0093;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF6;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 0x12;
            subeventResult.pData = &tv7[tv7Se0DataIndex +
                                        tv7Se1DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 3U;
            subeventResult.startACLConnEvent = 0x0094;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF8;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 0x12;
            subeventResult.pData = &tv7[tv7Se0DataIndex +
                                        tv7Se1DataIndex +
                                        tv7Se2DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 4U;
            subeventResult.startACLConnEvent = 0x0095;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF9;
            subeventResult.procedureDoneStatus = 0x0F;
            subeventResult.subeventDoneStatus = 0x0F;
            subeventResult.abortReason = 0x0F;
            subeventResult.numStepsReported = 0x12;
            subeventResult.pData = &tv7[tv7Se0DataIndex +
                                        tv7Se1DataIndex +
                                        tv7Se2DataIndex +
                                        tv7Se3DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            /* This subevent is aborted and the next is sent */
        }

        case 8U: /* Test vector 8 - multiple_subevents_mode3_only (8 subevents) */
        {
            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    break;
                }
            }

            mResultData[deviceId].selectedTxPower = 0x7F;
            mResultData[deviceId].subeventIndex = 0U;
            csSubeventResultEvent_t subeventResult = {
                .deviceId = deviceId,
                .configId = 1U,
                .startACLConnEvent = 0x00B1,
                .procedureCounter = 0U,
                .frequencyCompensation = 0x0000,
                .referencePowerLevel = (int8_t)0xF9,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x00,
                .abortReason = 0x00,
                .numAntennaPaths = 1U,
                .numStepsReported = 14,
                .pData = tv8,
            };
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 1U;
            subeventResult.startACLConnEvent = 0x00B2;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF8;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 14;
            subeventResult.pData = &tv8[tv8Se0DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 2U;
            subeventResult.startACLConnEvent = 0x00B3;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xFA;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 14;
            subeventResult.pData = &tv8[tv8Se0DataIndex +
                                        tv8Se1DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 3U;
            subeventResult.startACLConnEvent = 0x00B4;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF6;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 14;
            subeventResult.pData = &tv8[tv8Se0DataIndex +
                                        tv8Se1DataIndex +
                                        tv8Se2DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 4U;
            subeventResult.startACLConnEvent = 0x00B5;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF6;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 14;
            subeventResult.pData = &tv8[tv8Se0DataIndex +
                                        tv8Se1DataIndex +
                                        tv8Se2DataIndex +
                                        tv8Se3DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 5U;
            subeventResult.startACLConnEvent = 0x00B6;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF6;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 14;
            subeventResult.pData = &tv8[tv8Se0DataIndex +
                                        tv8Se1DataIndex +
                                        tv8Se2DataIndex +
                                        tv8Se3DataIndex +
                                        tv8Se4DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 6U;
            subeventResult.startACLConnEvent = 0x00B7;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF9;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 14;
            subeventResult.pData = &tv8[tv8Se0DataIndex +
                                        tv8Se1DataIndex +
                                        tv8Se2DataIndex +
                                        tv8Se3DataIndex +
                                        tv8Se4DataIndex +
                                        tv8Se5DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 7U;
            subeventResult.startACLConnEvent = 0x00B8;
            subeventResult.frequencyCompensation = 0x0000;
            subeventResult.referencePowerLevel = (int8_t)0xF9;
            subeventResult.procedureDoneStatus = 0x00;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 11;
            subeventResult.pData = &tv8[tv8Se0DataIndex +
                                        tv8Se1DataIndex +
                                        tv8Se2DataIndex +
                                        tv8Se3DataIndex +
                                        tv8Se4DataIndex +
                                        tv8Se5DataIndex +
                                        tv8Se6DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            break;
        }

        case 9U: /* Test vector 9  - filter_input_mode0123_Ini_sounding_nAP4 (4 subevents) */
        {
            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    break;
                }
            }

            mResultData[deviceId].selectedTxPower = 0x7F;
            mResultData[deviceId].subeventIndex = 0U;
            csSubeventResultEvent_t subeventResult = {
                .deviceId = deviceId,
                .configId = 0U,
                .startACLConnEvent = 0x0084,
                .procedureCounter = 0U,
                .frequencyCompensation = 0xFF20,
                .referencePowerLevel = (int8_t)0x7F,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x01,
                .abortReason = 0x00,
                .numAntennaPaths = 4U,
                .numStepsReported = 15,
                .pData = tv9,
            };
            (void)processCsResultsEvent(&subeventResult);

            csSubeventResultContinueEvent_t subeventResultContinue = {
                .deviceId = deviceId,
                .configId = 0U,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x00,
                .abortReason = 0x00,
                .numAntennaPaths = 4U,
                .numStepsReported = 10,
                .pData = &tv9[tv9Se0DataIndex],
            };
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            mResultData[deviceId].subeventIndex = 1U;
            subeventResult.startACLConnEvent = 0x0137;
            subeventResult.frequencyCompensation = 0xFF27;
            subeventResult.referencePowerLevel = (int8_t)0xFF;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x01;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 11;
            subeventResult.pData = &tv9[tv9Se0DataIndex +
                                        tv9Se0c0DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            subeventResultContinue.procedureDoneStatus = 0x01;
            subeventResultContinue.subeventDoneStatus = 0x00;
            subeventResultContinue.abortReason = 0x00;
            subeventResultContinue.numAntennaPaths = 4U;
            subeventResultContinue.numStepsReported = 4;
            subeventResultContinue.pData = &tv9[tv9Se0DataIndex +
                                                tv9Se0c0DataIndex +
                                                tv9Se1DataIndex];
            (void)processCsResultsContinueEvent(&subeventResultContinue);

            mResultData[deviceId].subeventIndex = 2U;
            subeventResult.startACLConnEvent = 0x0137;
            subeventResult.frequencyCompensation = 0xFF1E;
            subeventResult.referencePowerLevel = (int8_t)0xFF;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 6;
            subeventResult.pData = &tv9[tv9Se0DataIndex +
                                        tv9Se0c0DataIndex +
                                        tv9Se1DataIndex +
                                        tv9Se1c0DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 3U;
            subeventResult.startACLConnEvent = 0x0137;
            subeventResult.frequencyCompensation = 0xFF1E;
            subeventResult.referencePowerLevel = (int8_t)0xFF;
            subeventResult.procedureDoneStatus = 0x00;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 4;
            subeventResult.pData = &tv9[tv9Se0DataIndex +
                                        tv9Se0c0DataIndex +
                                        tv9Se1DataIndex +
                                        tv9Se1c0DataIndex +
                                        tv9Se2DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            break;
        }

        case 10U: /* Test vector 10 (case 9)  - filter_input_mode0123_Ini_sounding_nAP4 (4 subevents) */
                  /* (Merged Continue Events) */
        {
            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    break;
                }
            }

            mResultData[deviceId].selectedTxPower = 0x7F;
            mResultData[deviceId].subeventIndex = 0U;
            csSubeventResultEvent_t subeventResult = {
                .deviceId = deviceId,
                .configId = 0U,
                .startACLConnEvent = 0x0084,
                .procedureCounter = 0U,
                .frequencyCompensation = 0xFF20,
                .referencePowerLevel = (int8_t)0x7F,
                .procedureDoneStatus = 0x01,
                .subeventDoneStatus = 0x00,
                .abortReason = 0x00,
                .numAntennaPaths = 4U,
                .numStepsReported = 15 + 10,
                .pData = tv9,
            };
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 1U;
            subeventResult.startACLConnEvent = 0x0137;
            subeventResult.frequencyCompensation = 0xFF27;
            subeventResult.referencePowerLevel = (int8_t)0xFF;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 11 + 4;
            subeventResult.pData = &tv9[tv9Se0DataIndex +
                                        tv9Se0c0DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 2U;
            subeventResult.startACLConnEvent = 0x0137;
            subeventResult.frequencyCompensation = 0xFF1E;
            subeventResult.referencePowerLevel = (int8_t)0xFF;
            subeventResult.procedureDoneStatus = 0x01;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 6;
            subeventResult.pData = &tv9[tv9Se0DataIndex +
                                        tv9Se0c0DataIndex +
                                        tv9Se1DataIndex +
                                        tv9Se1c0DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            mResultData[deviceId].subeventIndex = 3U;
            subeventResult.startACLConnEvent = 0x0137;
            subeventResult.frequencyCompensation = 0xFF1E;
            subeventResult.referencePowerLevel = (int8_t)0xFF;
            subeventResult.procedureDoneStatus = 0x00;
            subeventResult.subeventDoneStatus = 0x00;
            subeventResult.abortReason = 0x00;
            subeventResult.numStepsReported = 4;
            subeventResult.pData = &tv9[tv9Se0DataIndex +
                                        tv9Se0c0DataIndex +
                                        tv9Se1DataIndex +
                                        tv9Se1c0DataIndex +
                                        tv9Se2DataIndex];
            (void)processCsResultsEvent(&subeventResult);

            break;
        }

        /* reader tests */
        case 103U: /* Fall-through */
        {
            /* PTS sends data starting with procedure count 0x100 for this test */
            mResultData[deviceId].procedureCounter = 0x100;
        }

        case 101U:
        {
            if (mResultData[deviceId].pData == NULL)
            {
                mResultData[deviceId].pData = MEM_BufferAlloc(gRasCsSubeventDataSize_c);
                if (mResultData[deviceId].pData == NULL)
                {
                    break;
                }
            }
            break;
        }

#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
        case 102U:
        {
            (void)RasClient_SendRasCommand(deviceId, abortOperationOpCode_c,
                                           0U, 0U, 0U, gAntennaPathFilterAllowAll_c);

             break;
        }
#endif

        default:
        {
            break;
        }
    }

    if (bSendDataReady)
    {
        (void)Ras_SendDataReady(deviceId);
    }
    
    if (bDataOverwrittenTest == TRUE)
    {
        OSA_TimeDelay(2000);
        (void)Ras_SendDataOverwritten(deviceId);
    }
}
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */
