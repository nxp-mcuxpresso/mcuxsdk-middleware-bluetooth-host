/*! *********************************************************************************
 * \addtogroup Main
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022-2025 NXP
*
*
* \file
*
* This is the source file for the main entry point for the application.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "app.h"
#include "fsl_os_abstraction.h"
#include "fwk_platform_ble.h"
#include "host_ble_init.h"
#include "w_uart_application.h"

#include "RNG_Interface.h"
#if defined(gAppUseNvm_d) && (gAppUseNvm_d > 0)
#include "NVM_Interface.h"
#endif /* gAppUseNvm_d */
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d > 0)
#include "PWR_Interface.h"
#endif

/************************************************************************************
 *************************************************************************************
 * Private functions prototypes
 *************************************************************************************
 ************************************************************************************/
#if defined(SDK_OS_FREE_RTOS)
static void start_task(void *argument);
#endif
/************************************************************************************
 *************************************************************************************
 * Private memory declarations
 *************************************************************************************
 ************************************************************************************/
#if defined(SDK_OS_FREE_RTOS)
static OSA_TASK_HANDLE_DEFINE(s_startTaskHandle);
static OSA_TASK_DEFINE(start_task, gMainThreadPriority_c, 1, gMainThreadStackSize_c, 0);
#endif
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
#if defined(SDK_OS_FREE_RTOS)
static void start_task(void *argument)
{
    /* Start Application services (timers, serial manager, low power, led, button, etc..) */
    APP_InitServices();

    /* Initialize shell */
    Shell_Init();

    App_RegisterEventCallback(BleApp_EventCallback);

    BluetoothLEHost_AppInit();
    RFMC->RF2P4GHZ_CFG |= RFMC_RF2P4GHZ_CFG_FORCE_DBG_PWRUP_ACK_MASK;
    CMC0->DBGCTL &= ~CMC_DBGCTL_SOD_MASK;

    /* Start BLE Platform related ressources such as clocks, Link layer and HCI transport to Link Layer */
    (void)APP_InitBle();

    /* RNG software initialization and PRNG initial seeding (from hardware) */
    (void)RNG_Init();

#if (defined gFsciOverRpmsgBridge_c) && (gFsciOverRpmsgBridge_c > 0U)
    BluetoothLEHost_AppInitController();
#endif

    while(TRUE)
    {
        App_HandleMessages();
    }
}
#endif

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

#if !defined(SDK_OS_FREE_RTOS)
/*! *********************************************************************************
*\fn           void App_ProcessIdleTask(void)
*\brief        Handles Connectivity background task, usually executed from Idle task.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void App_ProcessIdleTask(void)
{
#if defined(gAppUseNvm_d) && (gAppUseNvm_d > 0)
    if(NvIdle() == 0)
#endif /* gAppUseNvm_d */
    {
    }
}
#endif

int main(void)
{
    /* Init OSA: should be called before any other OSA API */
    OSA_Init();

    /* Example of baremetal loop if user doesn't want to use OSA API */
#if (FSL_OSA_BM_TIMER_CONFIG != FSL_OSA_BM_TIMER_NONE)
    OSA_TimeInit();
#endif

    BOARD_InitHardware();

#if defined(SDK_OS_FREE_RTOS)

    (void)OSA_TaskCreate((osa_task_handle_t)s_startTaskHandle, OSA_TASK(start_task), NULL);

    /* Start scheduler*/
    OSA_Start();
#else
    /* Start Application services (timers, serial manager, low power, led, button, etc..) */
    APP_InitServices();
    App_RegisterEventCallback(BleApp_EventCallback);

    /* Start Host stack */
    BluetoothLEHost_AppInit();
    RFMC->RF2P4GHZ_CFG |= RFMC_RF2P4GHZ_CFG_FORCE_DBG_PWRUP_ACK_MASK;
    CMC0->DBGCTL &= ~CMC_DBGCTL_SOD_MASK;

    /* Start BLE Platform related ressources such as clocks, Link layer and HCI transport to Link Layer */
    (void)APP_InitBle();

    /* RNG software initialization and PRNG initial seeding (from hardware) */
    (void)RNG_Init();

    /* Initialize shell */
    Shell_Init();

#if (defined gFsciOverRpmsgBridge_c) && (gFsciOverRpmsgBridge_c > 0U)
    BluetoothLEHost_AppInitController();
#endif

    while(TRUE)
    {
        OSA_ProcessTasks();
        App_HandleMessages();

        /* Before executing WFI, need to execute some connectivity background tasks
            (usually done in Idle thread) such as NVM save in Idle, etc.. */
        App_ProcessIdleTask();

        OSA_DisableIRQGlobal();

        /* Check if some connectivity tasks have turned to ready state from interrupts or
              if messages are to be processed in Application process */
        if (( OSA_TaskShouldYield() == FALSE ) && ( App_IsMessagePending() == FALSE ))
        {
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d > 0)
            (void)PWR_EnterLowPower(0U);
#else
            __WFI();
#endif
        }

        OSA_EnableIRQGlobal();
    }
#endif /* SDK_OS_FREE_RTOS */
    /* Won't run here */
    assert(0);
    return 0;
}

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
