/*! *********************************************************************************
 * \addtogroup Main
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2021-2025 NXP
*
*
* \file
*
* This is the source file for the main entry point for a FreeRTOS application.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "app.h"
#include "app_conn.h"
#include "fsl_os_abstraction.h"

#include "RNG_Interface.h"

#ifdef LCE_KW47_MCXW72
#include "app_lce_init.h"
#endif
#if !defined(SDK_OS_FREE_RTOS) && (defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0))
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

    /* Start Host stack */
    BluetoothLEHost_AppInit();

    /* Start BLE Platform related ressources such as clocks, Link layer and HCI transport to Link Layer */
    (void)APP_InitBle();

    /* RNG software initialization and PRNG initial seeding (from hardware) */
    (void)RNG_Init();

    /* Start LCE */
#ifdef LCE_KW47_MCXW72
    (void)APP_InitLce();
#endif

    while(TRUE)
    {
        BluetoothLEHost_HandleMessages();
    }
}
#endif
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
int main(void)
{
    /* Init OSA: should be called before any other OSA API */
    OSA_Init();

    BOARD_InitHardware();
#if defined(SDK_OS_FREE_RTOS)
    (void)OSA_TaskCreate((osa_task_handle_t)s_startTaskHandle, OSA_TASK(start_task), NULL);

    /* Start scheduler*/
    OSA_Start();
#else
    /* Start Application services (timers, serial manager, low power, led, button, etc..) */
    APP_InitServices();
    /* Example of baremetal loop if user doesn't want to use OSA API */
#if (FSL_OSA_BM_TIMER_CONFIG != FSL_OSA_BM_TIMER_NONE)
    OSA_TimeInit();
#endif

    /* Start Host stack */
    BluetoothLEHost_AppInit();
    
    /* Start BLE Platform related ressources such as clocks, Link layer and HCI transport to Link Layer */
    (void)APP_InitBle();

    /* RNG software initialization and PRNG initial seeding (from hardware) */
    (void)RNG_Init();

    /* Start LCE */
#ifdef LCE_KW47_MCXW72
    (void)APP_InitLce();
#endif

    while(TRUE)
    {
        OSA_ProcessTasks();
        BluetoothLEHost_HandleMessages();

        /* Before executing WFI, need to execute some connectivity background tasks
            (usually done in Idle thread) such as NVM save in Idle, etc.. */
        BluetoothLEHost_ProcessIdleTask();

        OSA_DisableIRQGlobal();

        /* Check if some connectivity tasks have turned to ready state from interrupts or
              if messages are to be processed in Application process */
        if (( OSA_TaskShouldYield() == FALSE ) && ( BluetoothLEHost_IsMessagePending() == FALSE ) && (BluetoothLEHost_IsConnectivityTaskToProcess() == FALSE))
        {
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d > 0)
            (void)PWR_EnterLowPower(0U);
#else
            __WFI();
#endif
        }

        OSA_EnableIRQGlobal();
    }
#endif
    /*won't run here*/
    assert(0);
    return 0;
}

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
