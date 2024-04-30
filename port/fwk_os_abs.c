/*! *********************************************************************************
* Copyright 2020-2023 NXP
*
*
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */


#include "fwk_messaging.h"
#include "fsl_component_messaging.h"
#include "fwk_os_abs.h"
 /*******************************************************************************
 * Definitions
 ******************************************************************************/

 /*******************************************************************************
 * Prototypes
 ******************************************************************************/

 /*******************************************************************************
 * Variables
 ******************************************************************************/

 /*******************************************************************************
 * Code
 ******************************************************************************/
osaStatus_t FWK_OSA_EventCreate(osaEventId_t eventHandle, bool_t autoClear)
{
    return (osaStatus_t)OSA_EventCreate((osa_event_handle_t)eventHandle, (uint8_t)autoClear);
}

osaStatus_t FWK_OSA_TaskCreate(osaTaskId_t taskHandle, const osa_task_def_t *thread_def,osaTaskParam_t task_param)
{
    return (osaStatus_t)OSA_TaskCreate((osa_task_handle_t) taskHandle, thread_def, task_param);
}

