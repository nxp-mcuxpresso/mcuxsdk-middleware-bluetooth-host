/*! *********************************************************************************
* Copyright 2020-2021, 2023 NXP
*
*
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */


#ifndef OSA_ABS_PORT_H_
#define OSA_ABS_PORT_H_
/*==================================================================================================
Include Files
==================================================================================================*/
#include "EmbeddedTypes.h"
#include "fsl_os_abstraction.h"

/*==================================================================================================
Public macros
==================================================================================================*/
#define osaTaskParam_t  osa_task_param_t

/*! @brief Constant to pass as timeout value in order to wait indefinitely. */
#define osaWaitForever_c   ((uint32_t)(-1))

/*==================================================================================================
Public type definitions
==================================================================================================*/
/*! @brief Type for the event handler, returned by the OSA_EventCreate function. */
typedef void* osaEventId_t;

/*! @brief Type for a task handler, returned by the OSA_TaskCreate function. */
typedef void* osaTaskId_t;

/*! @brief Type for an event flags group, bit 32 is reserved. */
typedef uint32_t osaEventFlags_t;

/*! @brief Defines the return status of OSA's functions */
typedef enum
{
    osaStatus_Success = 0U, /*!< Success */
    osaStatus_Error   = 1U, /*!< Failed */
    osaStatus_Timeout = 2U, /*!< Timeout occurs while waiting */
    osaStatus_Idle    = 3U  /*!< Used for bare metal only, the wait object is not ready
                                and timeout still not occur */
}osaStatus_t;
/*==================================================================================================
Public prototypes
==================================================================================================*/
osaStatus_t FWK_OSA_EventCreate(osaEventId_t eventHandle, bool_t autoClear);
osaStatus_t FWK_OSA_TaskCreate(osaTaskId_t taskHandle, const osa_task_def_t *thread_def,osaTaskParam_t task_param);
/*==================================================================================================
Public global variables declarations
==================================================================================================*/

/*==================================================================================================
Public functions
==================================================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

#ifdef gUnitTesting_d
osa_status_t OSA_EventWait(osaEventId_t eventId, osaEventFlags_t flagsToWait, bool_t waitAll, uint32_t millisec, osaEventFlags_t *pSetFlags);
osa_status_t OSA_EventSet(osaEventId_t eventId, osaEventFlags_t flagsToSet);
void OSA_InterruptEnable(void);
void OSA_InterruptDisable(void);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* OSA_ABS_PORT_H_ */