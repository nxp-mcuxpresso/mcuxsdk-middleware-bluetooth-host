/*! *********************************************************************************
* Copyright 2020-2023 NXP
*
*
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */


/*==================================================================================================
Include Files
==================================================================================================*/
#include "fwk_timer_manager.h"
#include "fwk_mem_manager.h"
#include "fsl_component_timer_manager.h"
#include "fwk_os_abs.h"
#include "ble_general.h"
/*==================================================================================================
Private macros
==================================================================================================*/
#define TMR_IsTimerAllocated(timerId)           maTmrStatusTable[timerId]
#define TMR_SetTimerStatus(timerId, newStatus)  maTmrStatusTable[timerId] = newStatus

#define mTmrStatusFree_c        (0x00U)
#define mTmrStatusActive_c      (0x02U)
#define mTmrStatusInactive_c    (0x04U)
/*==================================================================================================
Private type definitions
==================================================================================================*/

/*==================================================================================================
Private prototypes
==================================================================================================*/
typedef struct tmrTimerTableEntry_tag
{
    TIMER_MANAGER_HANDLE_DEFINE(timerHandle);
}tmrTimerTableEntry_t;
/*==================================================================================================
Private global variables declarations
==================================================================================================*/
/* Keeps the timer handle and its status */
static tmrTimerTableEntry_t maTmrTimerTable[gTmrTotalTimers_c] = {0};
STATIC tmrStatus_t          maTmrStatusTable[gTmrTotalTimers_c] = {0};
/*==================================================================================================
Public functions
==================================================================================================*/
/*! ************************************************************************************************
* \brief  Allocates the timer handle
*
* \param[in] none
*
* \return  tmrTimerID_t or gTmrInvalidTimerID_c
************************************************************************************************* */
tmrTimerID_t TMR_AllocateTimer(void)
{
    tmrTimerID_t timerId = gTmrInvalidTimerID_c;
    timer_status_t status;

    for (uint8_t i = 0U; i < NumberOfElements(maTmrTimerTable); ++i)
    {
        OSA_InterruptDisable();
        if (TMR_IsTimerAllocated(i) == 0U)
        {
            TMR_SetTimerStatus((tmrTimerID_t)i, mTmrStatusInactive_c);
            OSA_InterruptEnable();
            timerId = (tmrTimerID_t)i;
            break;
        }
        OSA_InterruptEnable();
   }

    if (timerId != gTmrInvalidTimerID_c)
    {
        status = TM_Open(maTmrTimerTable[timerId].timerHandle);
        if (kStatus_TimerSuccess != status)
        {
            TMR_SetTimerStatus(timerId, mTmrStatusFree_c);
            timerId = gTmrInvalidTimerID_c;
        }
    }

    return timerId;
}

/*! ************************************************************************************************
* \brief  Sets the callback function to be called when timer expires and starts the timer
*
* \param[in] timerId        Timer ID returned by TMR_AllocateTimer
* \param[in] timerType      See timer type coded values
* \param[in] time           Expiration timeout in milliseconds
* \param[in] callback       Function callback to be called when timer expires
* \param[in] param          Pointer to params given to function callback
*
* \return  tmrErrCode_t
************************************************************************************************* */
tmrErrCode_t TMR_StartTimer
(
    tmrTimerID_t timerId,
    tmrTimerType_t timerType,
    uint32_t time,
    pfTmrCallBack_t callback,
    void *param
)
{   tmrErrCode_t status = gTmrError_c;

    if (timerId < gTmrTotalTimers_c)
    {
      (void)TM_InstallCallback(maTmrTimerTable[timerId].timerHandle,callback, param);

      if (kStatus_TimerSuccess == TM_Start(maTmrTimerTable[timerId].timerHandle, timerType, time))
      {
          TMR_SetTimerStatus(timerId, mTmrStatusActive_c);
          status = gTmrSuccess_c;
      }
    }
    else
    {
        status = gTmrOutOfRange_c;
    }

    return status;
}

/*! ************************************************************************************************
* \brief  Stops and frees timer
*
* \param[in] timerID    Timer ID returned by TMR_AllocateTimer
*
* \return  tmrErrCode_t
************************************************************************************************* */
tmrErrCode_t TMR_FreeTimer(tmrTimerID_t timerID)
{
    tmrErrCode_t status = gTmrOutOfRange_c;

    if (timerID < gTmrTotalTimers_c)
    {
        (void)TM_Close(maTmrTimerTable[timerID].timerHandle);
        TMR_SetTimerStatus(timerID, mTmrStatusFree_c);
        status = gTmrSuccess_c;
    }

    return status;
}

/*! ************************************************************************************************
* \brief  Checks if a timer ID is active
*
* \param[in] timerID    Timer ID returned by TMR_AllocateTimer
*
* \return  TRUE or FALSE
************************************************************************************************* */
bool_t TMR_IsTimerActive(tmrTimerID_t timerId)
{
    return (bool_t)TM_IsTimerActive(maTmrTimerTable[timerId].timerHandle);
}

/*! ************************************************************************************************
* \brief  Stops timer but keeps it allocated
*
* \param[in] timerID    Timer ID returned by TMR_AllocateTimer
*
* \return  tmrErrCode_t
************************************************************************************************* */
tmrErrCode_t TMR_StopTimer(tmrTimerID_t timerId)
{
    tmrErrCode_t result = gTmrInvalidId_c;
    if( (timerId >= gTmrTotalTimers_c) || (TMR_IsTimerAllocated(timerId) == 0U) )
    {
        result = gTmrInvalidId_c;
    }
    else
    {
        result = (tmrErrCode_t)TM_Stop(maTmrTimerTable[timerId].timerHandle);
    }
    return result;
}

/*! ************************************************************************************************
* \brief  Checks if a timer ID is ready
*
* \param[in] timerID    Timer ID returned by TMR_AllocateTimer
*
* \return  TRUE or FALSE
************************************************************************************************* */
bool_t TMR_IsTimerReady(tmrTimerID_t timerID)
{
    return (bool_t)TM_IsTimerReady(maTmrTimerTable[timerID].timerHandle);
}

/*! ************************************************************************************************
* \brief  Returns a 64-bit absolute timestamp
*
* \param[in] none
*
* \return  uint64_t
************************************************************************************************* */
uint64_t TMR_GetTimestamp (void)
{
    return TM_GetTimestamp();
}

/*! ************************************************************************************************
* \brief     Returns the remaining time until timeout
*
* \param[in] timerID    Timer ID returned by TMR_AllocateTimer
*
* \return    uint32_t   Remaining time in microseconds until first timer timeouts.
 ************************************************************************************************* */
uint32_t TMR_GetRemainingTime(tmrTimerID_t timerID)
{
    return TM_GetRemainingTime(maTmrTimerTable[timerID].timerHandle);
}

 /*==================================================================================================
Private functions
==================================================================================================*/

/*==================================================================================================
Private debug functions
==================================================================================================*/