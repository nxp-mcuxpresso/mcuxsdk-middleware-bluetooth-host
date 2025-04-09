/*! *********************************************************************************
* \addtogroup NCP Loc Reader application
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
#include "controller_init.h"
#include "fsl_os_abstraction.h"

#include "fsl_common.h"
#include "ll_types.h"
#include "fsl_adapter_rpmsg.h"
#include "ble_general.h"
#include "controller_api_ll.h"

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

#include "ble_port_fsci.h"
#include "app_conn.h"
#include "app_nvm.h"

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/
#define PACKET_INFO_QUEUE_SIZE    8    /* Number has to be a power of 2 */

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

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief   This function is used to send a message to the Host via HCI
********************************************************************************** */
static void NbuHci_SendPktToHost
(
    unsigned long packetType,
    void *pPacket,
    unsigned short packetSize
);

/*! *********************************************************************************
* \brief   Main application task
********************************************************************************** */
static void start_task(void *argument);

/*! *********************************************************************************
* \brief   This function is used to check the temperature and report a change.
********************************************************************************** */
static void NBU_CheckTemperatureChange(void);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static hci_pkt_info_t hciPacketInfo[PACKET_INFO_QUEUE_SIZE];
volatile static uint8_t pendingPktInfo  = 0;
static uint8_t readPktInfoIdx           = 0;
static uint8_t writePktInfoIdx          = 0;
static uint8_t nbrPacketInfoSkipped     = 0; /* for debug */

const nbuIntf_t nbuInterface = {
    .nbuHciIntf = NbuHci_SendPktToHost,
    .nbuChannelSwitchIntf = NULL,
    .nbuDbgIoSet = NULL,
    .nbuPhySwitchIntf = NULL,
    .nbuEnterCritical = OSA_InterruptDisable,
    .nbuExitCritical = OSA_InterruptEnable
};

/* Set task handle */
static OSA_TASK_HANDLE_DEFINE(s_startTaskHandle);
static OSA_TASK_DEFINE(start_task, gMainThreadPriority_c, 1, gMainThreadStackSize_c, 0);

static bool_t nbu_tasks_init_done = FALSE;
static bool_t isHighZ = FALSE; /*For peak power reduction feature.*/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn        bleResult_t Hcit_PktReceived(hciPacketType_t type, void* packet, uint16_t size)
*\brief     This is the generic callback for Bluetooth events.
*
*\param[in]     type       Packet type
*\param[in]     packet     Pointer to packet
*\param[in]     size       Packet size
*
*\retval    gBleSuccess_c               Operation successfull
********************************************************************************** */
bleResult_t Hcit_PktReceived
(
    hciPacketType_t type,
    void           *packet,
    uint16_t        size
)
{
    PWR_DBG_LOG("Rcv PKT type=%d pkt=%x sz=%d", type, packet, size);
    /* delay processing of HCI commands into idle task as not all NBU tasks are initialized */
    if (nbu_tasks_init_done == TRUE)
    {
        NbuHci_SendPktToController(type, packet, size);
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
           /* ERROR: Out of memory */
           nbrPacketInfoSkipped++;
        }

        if (pendingPktInfo >= PACKET_INFO_QUEUE_SIZE)
        {
           /* ERROR: Message will be lost */
           nbrPacketInfoSkipped++;
        }
        if (nbrPacketInfoSkipped > 0)
        {
            assert(0);
            return gBleOutOfMemory_c;
        }
        hciPacketInfo[writePktInfoIdx].packetType = type;
        hciPacketInfo[writePktInfoIdx].pPacket = pPacketBuffer;
        hciPacketInfo[writePktInfoIdx].packetSize = size;
        writePktInfoIdx = (writePktInfoIdx+1)&(PACKET_INFO_QUEUE_SIZE-1);  /* modulo PACKET_INFO_QUEUE_SIZE */
        pendingPktInfo++;
    }

    return gBleSuccess_c;
}

/*! *********************************************************************************
*\fn           void NBU_Idle(void)
*\brief        Hook from LL Idle Task.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void NBU_Idle(void)
{
    NBU_CheckTemperatureChange();

    /* Enable logging timestamps - required LL to be enabled - move it to somewhere else */
    BOARD_DBGLOGCOUNTERRUNNING();

#if !defined(FPGA_TARGET) || (FPGA_TARGET == 0)
    /* Check if a measure is available and process the result
     * Called under masked interrupts so no more SFA interrupts are received */
    SFC_Process();
#endif /* FPGA_TARGET */

    OSA_DisableIRQGlobal();

    if (pendingPktInfo > 0)
    {
        PWR_DBG_LOG("pendingPktInfo=%x", pendingPktInfo);
        pendingPktInfo--;
        OSA_EnableIRQGlobal();

        BOARD_DBGLPIOSET(1u, 0u);

        /* we are not under exception context, we can send the packet now */
        NbuHci_SendPktToController(hciPacketInfo[readPktInfoIdx].packetType, hciPacketInfo[readPktInfoIdx].pPacket, hciPacketInfo[readPktInfoIdx].packetSize);
        MEM_BufferFree(hciPacketInfo[readPktInfoIdx].pPacket);

        readPktInfoIdx = (readPktInfoIdx+1)&(PACKET_INFO_QUEUE_SIZE-1);  /* modulo PACKET_INFO_QUEUE_SIZE */

        BOARD_DBGLPIOSET(1u, 1u);
    }
    else
    {
        BOARD_DBGLPIOSET(0u, 0u);

#if !defined(gNbuJtagCapability)    || (gNbuJtagCapability==0)
        /* Try to go to low power (Deep Sleep), if that's not possible, it will
         * go to WFI only.
         * To keep full debug capability, set gNbuJtagCapability to 1 to avoid
         * Deep Sleep or WFI. */
        PLATFORM_EnterLowPower();
#endif

        BOARD_DBGLPIOSET(0u, 1u);

        OSA_EnableIRQGlobal();
    }
    nbu_tasks_init_done = TRUE;
#if (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U))
    App_NvmIdle();
#endif /* (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U)) */
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
uint8_t NbuGetCodedIndicator()
{
  return (((XCVR_2P4GHZ_PHY->STAT0 & GEN4PHY_STAT0_DATA_RATE_MASK) >> GEN4PHY_STAT0_DATA_RATE_SHIFT) == 3 ? 1 : 0);
}

/*! *********************************************************************************
* \brief    This function is used to configure LDOs for peak power reduction purpose.
*           It is called by LL if peak power reduction feature is enabled in LL.
********************************************************************************** */
void NbuPwrPeakReductionActivityStart()
{
    XCVR_forceLdoAntEnable();
    isHighZ = FALSE;
}

/*! *********************************************************************************
* \brief    This function is used to configure LDOs for peak power reduction purpose.
*           It is called by LL if peak power reduction feature is enabled in LL.
********************************************************************************** */
void NbuPwrPeakReductionActivityStop()
{
    XCVR_setLdoAntHiz();
    isHighZ = TRUE;
}

/*! *********************************************************************************
* \brief    This function is used to configure LDOs for peak power reduction purpose.
*           It is called by LL if peak power reduction feature is enabled in LL.
********************************************************************************** */
void NbuPwrPeakReductionDisable()
{
    if (isHighZ)
    {
        XCVR_releaseLdoAntAll();
        isHighZ = FALSE;
    }
}

/*! *********************************************************************************
* \brief    Application configuration function called after the NBU has been initialized.
********************************************************************************** */
void NBU_Init()
{
    /* Init MemManager for buffer allocation in serial manager */
    MEM_Init();
    /* Low level init for the BLE controller */
    PLATFORM_InitBle();
    /* Init Framework Intercore Service */
    PLATFORM_FwkSrvInit();

#if !defined(FPGA_TARGET) || (FPGA_TARGET == 0)
    /* SFC module requires FwkSrv service to be initialized */
    SFC_Init();
#endif /* FPGA_TARGET */

#if defined(CS_HANDOFF_ENABLED) && (CS_HANDOFF_ENABLED!=0)
    NBU_HADM_Init();
#endif

#if !defined(gNbuDisableLowpower_d) || (gNbuDisableLowpower_d==0)
        /* Initialize required ressources before requesting low power entry
         * If gNbuDisableLowpower_d is set to 1, this function won't be called so
         * PLATFORM_EnterLowPower will only go to WFI
         * CAUTION: do not move before Controller_RadioInit */
        PLATFORM_LowPowerInit();
#endif
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
    PLATFORM_SetFrequencyConstraintFromController(2);
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
    Controller_RadioInit();
    /* Debug init */
    BOARD_DBGINITRFACTIVE();
    BOARD_DBGINITDTEST();
    BOARD_DBGCONFIGINITNBU(true);
    /* Init NbuDbg IOs, will configure pinmux and GPIOD
        - need to be done after PLATFORM_RemoteActiveReq() */
    BOARD_DBGINITDBGIO();
    Controller_SetNbuVersion(nbu_version.repo_digest);
    /* Start LL scheduler */
    Controller_Init(&nbuInterface);  /* never returns */
    /* Won't run here*/
    assert(0);
    return 0;
}

/* Definition missing from marvell NBU libs
   ThreadX requires this two variables to be set to know the location o*/
uint32_t *      _tx_initialize_low_level_ptr;
uint32_t *      tx_application_define_ptr;

/* called from Marvell reset vector in name. It is not be used in our code */
int __main(void)
{
    assert(0);
    return 0;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  This function is used to send HCI packets to the host.
********************************************************************************** */
static void NbuHci_SendPktToHost
(
    unsigned long packetType,
    void *pPacket,
    unsigned short packetSize
)
{
    Ble_HciRecvFromIsr((hciPacketType_t)packetType, pPacket, (uint16_t)packetSize);
}

/*! *********************************************************************************
* \brief   This function is used to check the temperature and report a change.
********************************************************************************** */
static void NBU_CheckTemperatureChange(void)
{
#if defined(gNbu_Hadm_d) && (gNbu_Hadm_d==1)
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
#endif /* gNbu_Hadm_d */
}

/*! *********************************************************************************
* \brief   Application task.
********************************************************************************** */
static void start_task(void *argument)
{
    /* Initialize application specific functionality */
    BLE_PortFsciInit();

    while (TRUE)
    {
        BluetoothLEHost_HandleMessages();
    }
}

