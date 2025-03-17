/*! *********************************************************************************
* \addtogroup NCP FSCI application
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2025 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "fsl_common.h"
#include "ll_types.h"
#include "fsl_adapter_rpmsg.h"
#include "fsl_component_serial_manager.h"
#include "ble_general.h"
#include "controller_init.h"
#include "controller_api_ll.h"
#include "app.h"
#include "fsl_component_mem_manager.h"
#include "fsl_os_abstraction.h"
#include "fwk_platform.h"
#include "fwk_platform_ble.h"
#include "fwk_platform_ics.h"
#include "fwk_platform_lowpower.h"
#include "fwk_debug.h"
#include "fwk_rf_sfc.h"
#include "board.h"
#include "nxp2p4_xcvr.h"
#include "fwk_platform_sensors.h"
#include "rpmsg_config.h"
#include "ble_hadm_hal.h"
#include "ble_hadm_types.h"
#include "ble_hadm_api.h"
#include "controller_api_ll.h"
#include "FsciInterface.h"
#include "FsciCommunication.h"
#include "ncp_app_conn.h"
#include "fsci_ble_interface.h"
#include "ncp_fsci_black_box.h"
#include "fsci_ble_gap.h"
#if defined(gNbu_Hadm_d) && (gNbu_Hadm_d==1)
#include "lcl_hadm_measurement.h"
#endif


/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define PACKET_INFO_QUEUE_SIZE  (8U)    /* Number has to be a power of 2 */

#define gHciVendorSpecificDebugCommands_c   0x03FU
#define mHciSetMacAddrCommandLength_c       (8U)
#define gHciSetMacAddrCommand_c             0x0022U
#define HciCommand(opCodeGroup, opCodeCommand)\
    (((uint16_t)(opCodeGroup) & (uint16_t)0x3FU)<<(uint16_t)SHIFT10)|(uint16_t)((opCodeCommand) & 0x3FFU)
#define BT_USER_BD 254
#define gHciCommandPacketHeaderLength_c     (3U)

#if defined(gNbu_Hadm_d) && (gNbu_Hadm_d==1)
#define CS_HANDOFF_ENABLED           0U /* 0: no, 1: event, 2: procedure */
#endif
#if defined(CS_HANDOFF_ENABLED) && (CS_HANDOFF_ENABLED > 0)
/* event config pointer */
#if CS_HANDOFF_ENABLED==1
#define CS_HANDOFF_TYPE               BLE_HADM_EventConfig_t
#elif CS_HANDOFF_ENABLED==2
#define CS_HANDOFF_TYPE               const TBleHadmConnection_t
#else
#error CS_HANDOFF_ENABLED invalid
#endif /* CS_HANDOFF_ENABLED */
#endif /* CS_HANDOFF_ENABLED */
/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef struct hci_pkt_info_tag
{
    void*               pPacket;
    uint16_t            packetSize;
    hciPacketType_t     packetType;
} hci_pkt_info_t;

#if defined(CS_HANDOFF_ENABLED) && (CS_HANDOFF_ENABLED > 0)
/* structure used to copy the event config. As the RPMSG payload size is limited,
   the config copy is segmented */
typedef PACKED_STRUCT
{
    uint8_t  packetType;          /* 0x04: ACL data */
    uint16_t handle;              /* 0xFFFx: invalid handle for config identification, x = segment number */
    uint16_t dataLength;          /* payload length in this message */
    uint64_t startTime;           /* event start time for TSTMR0 */
    uint16_t configSize;          /* event config size */
    uint16_t configOffset;        /* starting offset of the config being copied */
    uint16_t copiedSize;          /* copied config size */
    uint8_t  config[RL_BUFFER_PAYLOAD_SIZE-32]; /* partial config copied */
} EventConfig_t;
#endif /* CS_HANDOFF_ENABLED */

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
static void NbuHci_SendPktToHost(unsigned long packetType, void *pPacket, unsigned short packetSize);
static bool_t nbu_tasks_init_done = FALSE;
static bool_t isHighZ = FALSE; /*For peak power reduction feature.*/
/*osa start_task*/
static void start_task(void *argument);
static void Hcit_RxCallBack(uint8_t packetType, uint8_t *data, uint16_t len);
#if defined(gNbu_Hadm_d) && (gNbu_Hadm_d==1)
static void NBU_CheckTemperatureChange(void);
#endif /* defined(gNbu_Hadm_d) && (gNbu_Hadm_d==1) */
static void NBU_Init(void);
static void Ble_SetBDAddr(bleDeviceAddress_t bdAddr);
static bleResult_t BleApp_ReadPublicDeviceAddress(void);
static void BleApp_HandleWritePublicDeviceAddress(void *pParam);
static void AppFSCI_Send( uint8_t *pPacket, uint16_t packetLen, bool_t freePacket);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static hci_pkt_info_t maHciPacketInfo[PACKET_INFO_QUEUE_SIZE];
volatile static uint8_t mPendingPktInfo  = 0U;
static uint8_t mReadPktInfoIdx           = 0U;
static uint8_t mWritePktInfoIdx          = 0U;
static uint8_t mNbrPacketInfoSkipped     = 0U; /* for debug */
static OSA_TASK_HANDLE_DEFINE(s_startTaskHandle);
static OSA_TASK_DEFINE(start_task, gMainThreadPriority_c, 1, gMainThreadStackSize_c, 0);
static bool_t mAppInitInProgress = FALSE;
static const nbuIntf_t nbuInterface = {
    .nbuHciIntf = NbuHci_SendPktToHost,
    .nbuChannelSwitchIntf = NULL,
    .nbuDbgIoSet = NULL,
    .nbuPhySwitchIntf = NULL,
    .nbuEnterCritical = OSA_InterruptDisable,
    .nbuExitCritical = OSA_InterruptEnable
};
/* Set fsci handler */
static serial_handle_t g_fsciHandleList[gFsciIncluded_c];

#if defined(CS_HANDOFF_ENABLED) && (CS_HANDOFF_ENABLED > 0)
static CS_HANDOFF_TYPE *p_hadm_config = NULL;
/* event start time in LL timing */
static uint32_t ulStartTimeHSlot;
static uint16_t ulStartTimeOffsetUs;
/* event start time in TSTMR0 value */
static uint64_t ullStartTimeTsTmr;
static EventConfig_t sHciConfig;
#endif /* CS_HANDOFF_ENABLED */

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
/* Definition missing from marvell NBU libs
   ThreadX requires this two variables to be set to know the location o*/
uint32_t *      _tx_initialize_low_level_ptr;
uint32_t *      tx_application_define_ptr;

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  This is the initialization function for each application. This function
*         should contain all the initialization code required by the Bluetooth demo.
********************************************************************************** */
void BluetoothLEHost_AppInit(void)
{
    union 
    {
        void *pVoid;
        pfFSCI_Send_t pfFSCI_Send;
    } fsciHandle;
    fsciHandle.pfFSCI_Send = &AppFSCI_Send;
    g_fsciHandleList[0] = fsciHandle.pVoid;

    /* Init FSCI */
    FSCI_commInit(g_fsciHandleList);

    /* Register BLE handlers in FSCI */
    fsciBleRegister(0);
    
    PLATFORM_SetHciRxCallback(Hcit_RxCallBack);

    /* Register generic callback */
    BluetoothLEHost_SetGenericCallback(BleApp_GenericCallback);

    /* Initialize Bluetooth Host Stack */
    BluetoothLEHost_Init(NULL);
    
    /* Bluetooth LE Host initialization postponed until the public device
    address is set in the Controller */
    mAppInitInProgress = TRUE;
    (void)BleApp_ReadPublicDeviceAddress();
}

/*! *********************************************************************************
* \brief  This is the generic callback for Bluetooth events.
********************************************************************************** */
void BleApp_GenericCallback(gapGenericEvent_t* pGenericEvent)
{
    fsciBleGapGenericEvtMonitor(pGenericEvent);
}

/*! *********************************************************************************
* \brief  This function is used to send HCI packets to the controller.
********************************************************************************** */
bleResult_t Hcit_PktReceived(hciPacketType_t type, void* packet, uint16_t size)
{
    PWR_DBG_LOG("Rcv PKT type=%d pkt=%x sz=%d", type, packet, size);
    /* delay processing of HCI commands into idle task as not all NBU tasks are initialized */
    if (nbu_tasks_init_done == TRUE)
    {
        NbuHci_SendPktToController((unsigned long)type, packet, size);
    }
    else
    {
        /* We are in interrupt context, we can't directly send the packet to the LL
         or we will have issue with ThreadX
         So, we store the packet and wait for Idle to send it */
        uint8_t *pPacketBuffer = MEM_BufferAlloc((uint32_t)size);
        if (pPacketBuffer != NULL)
        {
            FLib_MemCpy(pPacketBuffer, (uint8_t*)packet, size);
        }
        else
        {
            /* Out of memory */
            mNbrPacketInfoSkipped++;
        }

        if (mPendingPktInfo >= PACKET_INFO_QUEUE_SIZE)
        {
            /* ERROR: Message will be lost */
            mNbrPacketInfoSkipped++;
        }
        if (mNbrPacketInfoSkipped > 0U)
        {
            assert(0);
            return gBleOutOfMemory_c;
        }
        maHciPacketInfo[mWritePktInfoIdx].packetType = type;
        maHciPacketInfo[mWritePktInfoIdx].pPacket = pPacketBuffer;
        maHciPacketInfo[mWritePktInfoIdx].packetSize = size;
        mWritePktInfoIdx = (mWritePktInfoIdx + 1U) & (PACKET_INFO_QUEUE_SIZE - 1U);
        mPendingPktInfo++;
    }
    
    return gBleSuccess_c;
}

/*! *********************************************************************************
* \brief   This function is used to set the BD address in the Controller.
********************************************************************************** */
static void Ble_SetBDAddr(bleDeviceAddress_t bdAddr)
{
    /* Set BD address by HCI message */
    uint8_t aHciPacket[mHciSetMacAddrCommandLength_c + gHciCommandPacketHeaderLength_c];
    uint16_t opcode = HciCommand(gHciVendorSpecificDebugCommands_c, gHciSetMacAddrCommand_c);

        /* Set HCI opcode */
    FLib_MemCpy((void*)aHciPacket, (const void*)&opcode, 2U);
    /* Set HCI parameter length */
    aHciPacket[2] = (uint8_t)mHciSetMacAddrCommandLength_c;
    /* Set command parameter ID */
    aHciPacket[3] = (uint8_t)BT_USER_BD;
    /* Set command parameter length */
    aHciPacket[4] = (uint8_t)6U;

    FLib_MemCpy((void*)&aHciPacket[gHciCommandPacketHeaderLength_c + 2U], (const void*)bdAddr, gcBleDeviceAddressSize_c);

    /* Send HCI command */
    (void)Hcit_PktReceived(gHciCommandPacket_c, aHciPacket, gHciCommandPacketHeaderLength_c + mHciSetMacAddrCommandLength_c);
}

/*! *********************************************************************************
* \brief    API allows to read the CI coding indicator just after it is available
*           (after header decoding done).
*               DATA_RATE = 3 => S2 encoding
*               DATA_RATE = 2 => S8 encoding
*           There is another mean to read the CI
*           (though ptBLE_RD->RX_PKT_STATUS.RX_BLE_PKT_STATUS.uRxCodedIndic) but
*           this CI indication is only valid when packet is entirely received.
********************************************************************************** */
unsigned char NbuGetCodedIndicator(void)
{
    return (((XCVR_2P4GHZ_PHY->STAT0 & GEN4PHY_STAT0_DATA_RATE_MASK) >> GEN4PHY_STAT0_DATA_RATE_SHIFT) == 3U ? 1U : 0U);
}

/*! *********************************************************************************
* \brief    This function is used to configure LDOs for peak power reduction purpose.
*           It is called by LL if peak power reduction feature is enabled in LL.
********************************************************************************** */
void NbuPwrPeakReductionActivityStart(void)
{
    XCVR_forceLdoAntEnable();
    isHighZ = FALSE;
}

/*! *********************************************************************************
* \brief    This function is used to configure LDOs for peak power reduction purpose.
*           It is called by LL if peak power reduction feature is enabled in LL.
********************************************************************************** */
void NbuPwrPeakReductionActivityStop(void)
{
    XCVR_setLdoAntHiz();
    isHighZ = TRUE;
}

/*! *********************************************************************************
* \brief    This function is used to configure LDOs for peak power reduction purpose.
*           It is called by LL if peak power reduction feature is enabled in LL.
********************************************************************************** */
void NbuPwrPeakReductionDisable(void)
{
    if (isHighZ)
    {
        XCVR_releaseLdoAntAll();
        isHighZ = FALSE;
    }
}

/*! *********************************************************************************
* \brief   Hook from LL Idle Task.
********************************************************************************** */
void NBU_Idle(void)
{
#if defined(CS_HANDOFF_ENABLED) && (CS_HANDOFF_ENABLED!=0)
    if( p_hadm_config != NULL )
    {
        NBU_HADM_CopyConfig();
        p_hadm_config = NULL;
    }
#endif
#if defined(gNbu_Hadm_d) && (gNbu_Hadm_d==1)
    NBU_CheckTemperatureChange();
#endif /* defined(gNbu_Hadm_d) && (gNbu_Hadm_d==1) */
    /* Enable logging timestamps - required LL to be enabled - move it to somewhere else */
    BOARD_DBGLOGCOUNTERRUNNING();
#if !defined(FPGA_TARGET) || (FPGA_TARGET == 0)
    /* Check if a measure is available and process the result
     * Called under masked interrupts so no more SFA interrupts are received */
    SFC_Process();
#endif /* FPGA_TARGET */
    OSA_DisableIRQGlobal();

    if(mPendingPktInfo > 0U)
    {
        PWR_DBG_LOG("mPendingPktInfo=%x", mPendingPktInfo);
        mPendingPktInfo--;
        OSA_EnableIRQGlobal();
        BOARD_DBGLPIOSET(1U, 0U);
        /* we are not under exception context, we can send the packet now */
        NbuHci_SendPktToController((unsigned long)maHciPacketInfo[mReadPktInfoIdx].packetType, maHciPacketInfo[mReadPktInfoIdx].pPacket, maHciPacketInfo[mReadPktInfoIdx].packetSize);
        (void)MEM_BufferFree(maHciPacketInfo[mReadPktInfoIdx].pPacket);
        mReadPktInfoIdx = (mReadPktInfoIdx + 1U) & (PACKET_INFO_QUEUE_SIZE - 1U);
        BOARD_DBGLPIOSET(1U, 1U);
    }
    else
    {
        BOARD_DBGLPIOSET(0U, 0U);
#if !defined(gNbuJtagCapability) || (gNbuJtagCapability==0)
        /* Try to go to low power (Deep Sleep), if that's not possible, it will
         go to WFI only. */
         /* To keep full debug capability, set gNbuJtagCapability to 1 to avoid
         Deep Sleep or WFI. */
        PLATFORM_EnterLowPower();
#endif
        BOARD_DBGLPIOSET(0U, 1U);
        OSA_EnableIRQGlobal();
    }
    nbu_tasks_init_done = TRUE;
#if (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U))
    BluetoothLEHost_ProcessIdleTask();
#endif /* (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U)) */
}

/*! *********************************************************************************
* \brief    This function is called from ThreadX's tx_application_define function
*           If needed, we can create ThreadX objects (tasks, queues...) from there
*           This is also used to configure the Systicks (weren't before) 
********************************************************************************** */
void tx_application_define_hook(void)
{
    (void)OSA_TaskCreate((osa_task_handle_t)s_startTaskHandle, OSA_TASK(start_task), NULL);
    NBU_Init();
}

/*! *********************************************************************************
* \brief    main function used for initialization 
********************************************************************************** */
int main(void)
{
    /* Configure FRO192M clock */
#if !defined(FPGA_TARGET) || (FPGA_TARGET == 0)
    PLATFORM_InitFro192M();
#endif

#if !defined(FPGA_TARGET) || (FPGA_TARGET == 0)
    /* By default the NBU runs to 32MHz, set the constraint in the init to
     * prevent the app core to set a slower speed for the NBU on its side */
    PLATFORM_SetFrequencyConstraintFromController(2U);
#endif

#define TICK_RATE_HZ 1000U
    SysTick->LOAD |= (BOARD_GetSystemCoreClockFreq() / TICK_RATE_HZ) - 1U;
    /* Not enabling the Systicks now, will be done in _tx_thread_schedule */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;

#if !defined(RF_OSC_26MHZ) || (RF_OSC_26MHZ == 0)
    /* bt_eclk should be >=16MHz, so for XO=26MHz select XO while for XO=32MHz XO/2 can be selected
     * This allows to save some power during active mode */
    RADIO_CTRL->RF_CLK_CTRL &= ~RADIO_CTRL_RF_CLK_CTRL_BT_ECLK_DIV_MASK;
    RADIO_CTRL->RF_CLK_CTRL |= RADIO_CTRL_RF_CLK_CTRL_BT_ECLK_DIV(0x1U);
#endif

    /* Init OSA: should be called before any other OSA API*/
    OSA_Init();
    (void)Controller_RadioInit();
    /* Debug init */
    BOARD_DBGINITRFACTIVE();
    BOARD_DBGINITDTEST();
    BOARD_DBGCONFIGINITNBU(true);
    /* Init NbuDbg IOs, will configure pinmux and GPIOD
        - need to be done after PLATFORM_RemoteActiveReq() */
    BOARD_DBGINITDBGIO();
    (void)Controller_SetNbuVersion(nbu_version.repo_digest);
    /* Start LL scheduler */
    (void)Controller_Init(&nbuInterface);  /* never returns */
    /* Won't run here*/
    assert(0);
    return 0;
}

/*! *********************************************************************************
* \brief    Called from Controller reset vector in name. It is not be used in our code
********************************************************************************** */
int __main(void)
{
    assert(0);
    return 0;
}

#if defined(CS_HANDOFF_ENABLED) && (CS_HANDOFF_ENABLED > 0)
/* Callback from link-layer */
#if CS_HANDOFF_ENABLED==1
BLE_HADM_STATUS_t BLE_HADM_SubeventContinue(BLE_HADM_SubeventConfig_t *pConfig, uint32 startTimeHSlot, uint32 startTimeOffsetUs)
#elif CS_HANDOFF_ENABLED==2
BLE_HADM_STATUS_t BLE_HADM_ProcedureContinue(const TBleHadmConnection_t *pConfig, uint32 startTimeHSlot, uint32 startTimeOffsetUs)
#endif
{
    /* save LL CS event start time */
    ulStartTimeHSlot = startTimeHSlot;
    ulStartTimeOffsetUs = (uint16_t)startTimeOffsetUs;

    /* get event config pointer */
    p_hadm_config = pConfig;

    /* The event config is updated after the return of the call, so it is not available yet.
       Return HADM_HAL_ABORTED to request CS proceudre abort in the LL. */
    return HADM_HAL_ABORTED;
}
#endif /* CS_HANDOFF_ENABLED */

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  This function is used to send HCI packets to the host.
********************************************************************************** */
static void NbuHci_SendPktToHost(unsigned long packetType, void *pPacket, unsigned short packetSize)
{
    (void)Ble_HciRecvFromIsr((hciPacketType_t)packetType, pPacket, (uint16_t)packetSize);
}


#if defined(CS_HANDOFF_ENABLED) && (CS_HANDOFF_ENABLED > 0)
/*! *********************************************************************************
* \brief        Configures HADM after initialization
********************************************************************************** */
static void  NBU_HADM_Init(void)
{
}

/*! *********************************************************************************
* \brief    This function is used to send the config to the app core.
*           Sample code to send the config to the app core:
*   CS Event config size: 1228
*       02 00 F0 DE 01 95 64 8A 00 00 00 00 00 CC 04 00 00 D0 01   00 00 00 00 03 00 00 01 5B 13
*       02 01 F0 DE 01 95 64 8A 00 00 00 00 00 CC 04 D0 01 D0 01   00 00 00 00 00 00 00 00 00 00
*       02 02 F0 3A 01 95 64 8A 00 00 00 00 00 CC 04 A0 03 2C 01   00 00 00 00 00 00 00 00 00 00
*   CS Procedure: 204 + 88
*       02 00 F0 DA 00 68 C4 7A 00 00 00 00 00 CC 00 00 00 CC 00   80 00 02 00 0F 00 40 00 01 0A
*       02 00 F8 66 00 68 C4 7A 00 00 00 00 00 58 00 00 00 58 00   00 C0 14 00 E5 B6 03 00 D7 5D
********************************************************************************** */
static void  NBU_HADM_CopyConfig(void)
{
    #if CS_HANDOFF_ENABLED==1
    /* debug only: check if the config is valid, no update here */
    BLE_HADM_STATUS_t status = BLE_HADM_EventCheckConfig(p_hadm_config);
    assert(status == HADM_HAL_SUCCESS);
    #endif

    /* non optimized sample code to convert LL timing to TSTMR counter value in us */
    uint32_t clock;
    uint16_t qus;
    LL_API_GetBleTiming(&clock, &qus);
    uint64_t tstmr = *(uint64_t *)TSTMR0;

    uint64_t current = ((uint64_t)clock*625*2 + (uint64_t)qus) / 4;
    uint64_t start   = (uint64_t)ulStartTimeHSlot * 625 * 2 / 4 + (uint64_t)ulStartTimeOffsetUs;

    /* event start time in ullStartTimeTsTmr */
    uint64_t distance = start - current;
    ullStartTimeTsTmr = tstmr + distance;

    /* Event config is ready. Send it through RPMSG channel as ACL data messages */
    uint32_t remaining = sizeof(CS_HANDOFF_TYPE); /* ~1228 bytes for event, ~204 bytes for procedure */
    uint32_t no = 0U;
    while(remaining > 0U)
    {
        /* fill the copy message */
        sHciConfig.packetType = 0x02;
        sHciConfig.handle = 0xf000 + no;
        uint32_t sz = remaining > sizeof(sHciConfig.config) ? sizeof(sHciConfig.config):remaining;
        sHciConfig.dataLength = 8U + 2U + 2U + 2U + sz;
        sHciConfig.startTime = ullStartTimeTsTmr;
        sHciConfig.configSize = sizeof(CS_HANDOFF_TYPE);
        sHciConfig.configOffset = sizeof(CS_HANDOFF_TYPE) - remaining;
        sHciConfig.copiedSize = sz;
        memcpy(&sHciConfig.config, (uint8_t*)p_hadm_config + sHciConfig.configOffset, sz);
        remaining -= sz;
        no++;
        /* do the copy */
        OSA_InterruptDisable();
        PLATFORM_SendHciMessage((uint8_t*)&sHciConfig, 1+2+2+sHciConfig.dataLength);
        OSA_InterruptEnable();
    }

#if CS_HANDOFF_ENABLED==2
    /* copy TBleHadmConfiguration_t */
    remaining = sizeof(TBleHadmConfiguration_t); /* ~88 bytes */
    no = 0U;
    while(remaining > 0)
    {
        /* fill the copy message */
        sHciConfig.packetType = 0x02;
        sHciConfig.handle = 0xf800 + no;
        uint32_t sz = remaining > sizeof(sHciConfig.config) ? sizeof(sHciConfig.config):remaining;
        sHciConfig.dataLength = 8U + 2U + 2U + 2U + sz;
        sHciConfig.startTime = ullStartTimeTsTmr;
        sHciConfig.configSize = sizeof(TBleHadmConfiguration_t);
        sHciConfig.configOffset = sizeof(TBleHadmConfiguration_t) - remaining;
        sHciConfig.copiedSize = sz;
        memcpy(&sHciConfig.config, (uint8_t*)p_hadm_config->pActiveConfig + sHciConfig.configOffset, sz);
        remaining -= sz;
        no++;
        /* do the copy */
        OSA_InterruptDisable();
        PLATFORM_SendHciMessage((uint8_t*)&sHciConfig, 1+2+2+sHciConfig.dataLength);
        OSA_InterruptEnable();
    }
#endif
}
#endif /* #if CS_HANDOFF_ENABLED==1 || CS_HANDOFF_ENABLED==2 */

#if defined(gNbu_Hadm_d) && (gNbu_Hadm_d==1)
/*! *********************************************************************************
* \brief   This function is used to check the temperature and report a change.
********************************************************************************** */
static void NBU_CheckTemperatureChange(void)
{
    static int32_t nbu_last_temperature = PLATFORM_SENSOR_UNKNOWN_TEMPERATURE/10;
    int32_t new_temperature;

    /* Simply read cached data into memory, not a sensor polling */
    PLATFORM_GetTemperatureValue(&new_temperature);
    /* Temperature is provided in tenth of Celsisus degrees.
     * We rely on sensor inertia to avoid very frequent updates.
     */
    if (new_temperature/10 != nbu_last_temperature)
    {
        assert(new_temperature < 5000);
        nbu_last_temperature = new_temperature/10;

        /* Inform interested parties */
        lcl_hadm_handle_temperature_change(new_temperature/10);
    }
}
#endif /* gNbu_Hadm_d */
/*! *********************************************************************************
* \brief   Application task.
********************************************************************************** */
static void start_task(void *argument)
{
    BluetoothLEHost_AppInit();
    
    while(TRUE)
    {
        BluetoothLEHost_HandleMessages();
    }
}

/*! *********************************************************************************
* \private
* \fn             void Hcit_RxCallBack(uint8_t packetType, uint8_t *data, uint16_t len)
* \brief          HCI Transport Rx Callback.
*
* \param  [in]    packetType                    packetType
* \param  [in]    data                          Pointer to the received packet.
* \param  [in]    len                           Received data length.
*
********************************************************************************** */
static void Hcit_RxCallBack(uint8_t packetType, uint8_t *data, uint16_t len)
{
    uint8_t *pPacketBuffer = MEM_BufferAlloc((uint32_t)len + 1U);
    clientPacket_t *pFsciPacket = NULL;

    if (pPacketBuffer != NULL)
    {
        pPacketBuffer[0] = packetType;
        FLib_MemCpy(&pPacketBuffer[1], data, len);
        pFsciPacket = (clientPacket_t *)(void *)pPacketBuffer;
        
        if ((pFsciPacket->headerAndStatus.header.opGroup == gFsciBleGapOpcodeGroup_c) &&
            (pFsciPacket->headerAndStatus.header.opCode == (uint8_t)gBleCtrlWritePublicDeviceAddressOpCode_c))
        {
            /* Write public address request. Send to application for processing */
            (void)App_PostCallbackMessage(BleApp_HandleWritePublicDeviceAddress, pPacketBuffer);
        }
        else
        {
            FSCI_receivePacket(pPacketBuffer);
        }
    }
}

/*! *********************************************************************************
* \brief    Application configuration function called after the NBU has been initialized.
********************************************************************************** */
static void NBU_Init(void)
{
    /* Init MemManager for buffer allocation in serial manager */
    (void)MEM_Init();
    /* Low level init for the BLE controller */
    PLATFORM_InitBle();
    /* Init Framework Intercore Service */
    (void)PLATFORM_FwkSrvInit();
#if !defined(FPGA_TARGET) || (FPGA_TARGET == 0)
    /* SFC module requires FwkSrv service to be initialized */
    SFC_Init();
#endif /* FPGA_TARGET */
#if defined(CS_HANDOFF_ENABLED) && (CS_HANDOFF_ENABLED!=0)
    NBU_HADM_Init();
#endif /* CS_HANDOFF_ENABLED */
#if !defined(gNbuDisableLowpower_d) || (gNbuDisableLowpower_d==0)
    /* Initialize required ressources before requesting low power entry
     * If gNbuDisableLowpower_d is set to 1, this function won't be called so
     * PLATFORM_EnterLowPower will only go to WFI
     * CAUTION: do not move before Controller_RadioInit */
    PLATFORM_LowPowerInit();
#endif /* gNbuDisableLowpower_d */
}

/*! *********************************************************************************
* \brief    Request Core 0 application to send the Public Device Address.
********************************************************************************** */

/*! *********************************************************************************
*\private
*\fn           static bleResult_t BleApp_ReadPublicDeviceAddress(void)
*\brief        Request Core 0 application to send the Public Device Address.
*
*\param  [in]  none
*
*\retval       bleResult_t  gBleOutOfMemory_c in case of memory allocation failure
*                           gBleSuccess_c otherwise.
********************************************************************************** */
static bleResult_t BleApp_ReadPublicDeviceAddress(void)
{
    bleResult_t result = gBleSuccess_c;
    clientPacketStructured_t *pClientPacket;
    uint32_t fsciDataSize = 0U;
    
    pClientPacket = fsciBleAllocFsciPacket(gFsciBleGapOpcodeGroup_c, (uint8_t)gBleGapCmdReadPublicDeviceAddressOpCode_c, fsciDataSize);
    
    if (pClientPacket != NULL)
    {
        fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
    }
    else
    {
        result = gBleOutOfMemory_c;
    }
    
    return result;
}

/*! *********************************************************************************
*\private
*\fn           static void BleApp_HandleWritePublicDeviceAddress(void *pParam)
*\brief        Used to process the FSCI Write Public Device address command on the
*              application task.
*
*\param  [in]  pParam   FSCI packet.
*
*\retval       void.
********************************************************************************** */
static void BleApp_HandleWritePublicDeviceAddress(void *pParam)
{
    clientPacket_t* pClientPacket   = (clientPacket_t*)pParam;
    uint8_t*        pBuffer         = &pClientPacket->structured.payload[0];
    bleDeviceAddress_t deviceAddress = {0U};
    bool_t  reset = FALSE;

    /* Reset field is ignored on Core 1 */
    fsciBleGetBoolValueFromBuffer(reset, pBuffer);
    (void)reset;

    /* Get Device Address from buffer */
    fsciBleGetArrayFromBuffer(deviceAddress, pBuffer, ((uint32_t)gcBleDeviceAddressSize_c));
    /* Set address in the Controller */
    Ble_SetBDAddr(deviceAddress);
    (void)MEM_BufferFree(pParam);
    
    if (mAppInitInProgress == TRUE)
    {
        /* Resume application initialization */
        mAppInitInProgress = FALSE;
        BluetoothLEHost_ResumeInit();
    }
}

/*! *********************************************************************************
*\private
*\fn           static void AppFSCI_Send(uint8_t *pPacket, uint16_t packetLen, bool_t freePacket)
*\brief        This function sends a fsci packet through rpmsg.
*
*\param  [in]  pPacket      FSCI packet.
*\param  [in]  packetLen    Packet length.
*\param  [in]  freePacket   Set to TRUE when pPacket should be freed, FALSE otherwise.
*
*\retval       void.
********************************************************************************** */
static void AppFSCI_Send(uint8_t *pPacket, uint16_t packetLen, bool_t freePacket)
{
    (void)PLATFORM_SendHciMessage(pPacket, (uint32_t)packetLen);
    if(freePacket)
    {
        (void)MEM_BufferFree(pPacket);
    }
}
