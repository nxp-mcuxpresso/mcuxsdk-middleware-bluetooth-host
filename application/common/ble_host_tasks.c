/*! *********************************************************************************
 * \addtogroup BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2023 NXP
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
#include "fsl_component_messaging.h"
#include "fsl_component_panic.h"
#include "ble_host_tasks.h"
#include "ble_host_task_config.h"
#include "fsl_component_mem_manager.h"

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
OSA_EVENT_HANDLE_DEFINE(gHost_TaskEvent);
messaging_t   gApp2Host_TaskQueue;
messaging_t   gHci2Host_TaskQueue;


/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static void Host_Task(osa_task_param_t argument);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static OSA_TASK_HANDLE_DEFINE(mHost_TaskId);
static OSA_TASK_DEFINE(Host_Task, gHost_TaskPriority_c, 1, gHost_TaskStackSize_c, (uint8_t)FALSE);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn           osa_status_t Ble_HostTaskInit(void)
*\brief        Initializes the two tasks of the BLE Host Stack.
*
*\param  [in]  none.
*
*\return       osa_status_t    Initialization return status.
********************************************************************************** */
osa_status_t Ble_HostTaskInit(void)
{
    osa_status_t status;

    /* Initialization of task related */
    if (KOSA_StatusSuccess != OSA_EventCreate((osa_event_handle_t)gHost_TaskEvent,
                                              1U))
    {
        return KOSA_StatusError;
    }

    /* Initialization of task message queue */
    MSG_QueueInit(&gApp2Host_TaskQueue);
    MSG_QueueInit(&gHci2Host_TaskQueue);

    /* Task creation */
    status = OSA_TaskCreate((osa_task_handle_t)mHost_TaskId,
                            OSA_TASK(Host_Task),
                            NULL);
    (void)status;
    assert(KOSA_StatusSuccess == status);

    return KOSA_StatusSuccess;
}

/*! *********************************************************************************
*\fn           osa_status_t Ble_HostTaskDeInit(void)
*\brief        Destroys the BLE Host Stack tasks and its event.
*
*\param  [in]  none.
*
*\return       osa_status_t    Initialization return status.
********************************************************************************** */
osa_status_t Ble_HostTaskDeInit(void)
{
    osa_status_t status;
    /* Destroy the host task */
    status = OSA_TaskDestroy((osa_task_handle_t)mHost_TaskId);
    if(status == KOSA_StatusSuccess)
    {
        /* Destroy the host task event*/
        status = OSA_EventDestroy((osa_event_handle_t)gHost_TaskEvent);
    }
    return status;
}


/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\private
*\fn           void Host_Task(osa_task_param_t argument)
*\brief        Definition of the host task.
*
*\param  [in]  argument       Not used.
*
*\retval       void.
********************************************************************************** */
static void Host_Task(osa_task_param_t argument)
{
    Host_TaskHandler((void *) NULL);
}

/*! *********************************************************************************
* @}
********************************************************************************** */
