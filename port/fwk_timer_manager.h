/*! *********************************************************************************
* Copyright 2020-2023 NXP
*
*
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */


#ifndef TIMER_MANAGER_PORT_H_
#define TIMER_MANAGER_PORT_H_
/*==================================================================================================
Include Files
==================================================================================================*/
#include "EmbeddedTypes.h"

/*==================================================================================================
Public macros
==================================================================================================*/
/* Numbers of Timers from Application if using ConnFWK TimerManager
    Not used on KW45, K32W1 programs so set it to zero for compatibility */
#ifndef gTmrApplicationTimers_c
#define gTmrApplicationTimers_c          0U
#endif

/*
 * \brief   Total number of timers
 * VALID RANGE: sum of application and stack timers
 */
#ifndef gTmrTotalTimers_c
#define gTmrTotalTimers_c   ( gTmrApplicationTimers_c + gTmrStackTimers_c )
#endif

#define gTmrInvalidTimerID_c    (0xFFU)
/*
 * \brief   Timer types coded values
 */
#define gTmrSingleShotTimer_c   0x01U
#define gTmrIntervalTimer_c     0x02U
#define gTmrSetMinuteTimer_c    0x04U
#define gTmrSetSecondTimer_c    0x08U
#define gTmrLowPowerTimer_c     0x10U

/*
 * \brief   Times specified in milliseconds (max 0xFFFFFFFF)
 */
typedef uint32_t    tmrTimeInMilliseconds_t;

#define TmrMilliseconds( n )    ( (tmrTimeInMilliseconds_t) (n) )
/*
 * \brief   Converts the macro argument (i.e. seconds) into milliseconds
 */
#define TmrSeconds( n )         ( (tmrTimeInMilliseconds_t) (TmrMilliseconds(n) * 1000UL) )
/*
 * \brief   Converts the macro argument from microseconds to seconds
 */
#define TmrMicrosecondsToSeconds( n )   ( ((n) + 500000U) / 1000000U )
/*
 * \brief   Starts a low power timer and sets the callback function to be called when timer expires
 */
#define TMR_StartLowPowerTimer(timerId, timerType, time, callback, param) \
        TMR_StartTimer((timerId), ((timerType) | gTmrLowPowerTimer_c), (time), (callback), (param))

/*
 * \brief   Starts a single shot timer and sets the callback function to be called when timer expires
 */
#define TMR_StartSingleShotTimer(timerId, time, callback, param) \
        TMR_StartTimer((timerId), gTmrSingleShotTimer_c, (time), (callback), (param))

/*
 * \brief   Starts an interval timer and sets the callback function to be called when timer expires
 */
#define TMR_StartIntervalTimer(timerId, time, callback, param) \
        TMR_StartTimer((timerId), kTimerModeIntervalTimer, (time), (callback), (param))

/*==================================================================================================
Public type definitions
==================================================================================================*/
typedef uint8_t     tmrTimerID_t;
typedef uint8_t     tmrTimerType_t;
typedef uint8_t     tmrStatus_t;
typedef void ( *pfTmrCallBack_t ) ( void * param );

typedef enum {
    gTmrSuccess_c,
    gTmrInvalidId_c,
    gTmrOutOfRange_c,
    gTmrError_c
}tmrErrCode_t;

/*
 * \brief   Minute timer definition
 */
#define gTmrMinuteTimer_c       ( gTmrSetMinuteTimer_c )

/*
 * \brief   Second timer definition
 */
#define gTmrSecondTimer_c       ( gTmrSetSecondTimer_c )

/*
 * \brief   LP minute/second/millisecond timer definitions
 */
#define gTmrLowPowerMinuteTimer_c           ( gTmrMinuteTimer_c     | gTmrLowPowerTimer_c )
#define gTmrLowPowerSecondTimer_c           ( gTmrSecondTimer_c     | gTmrLowPowerTimer_c )
#define gTmrLowPowerSingleShotMillisTimer_c ( gTmrSingleShotTimer_c | gTmrLowPowerTimer_c )
#define gTmrLowPowerIntervalMillisTimer_c   ( gTmrIntervalTimer_c   | gTmrLowPowerTimer_c )
#define gTmrAllTypes_c                      ( gTmrSingleShotTimer_c | gTmrIntervalTimer_c  | \
                                              gTmrSetMinuteTimer_c  | gTmrSetSecondTimer_c | \
                                              gTmrLowPowerTimer_c)
/*==================================================================================================
Public prototypes
==================================================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

/*! ************************************************************************************************
* \brief  Allocates the timer handle from a dedicated memory pool
*
* \param[in] none
*
* \return  tmrTimerID_t or gTmrInvalidTimerID_c
************************************************************************************************* */
tmrTimerID_t TMR_AllocateTimer(void);

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
);

/*! ************************************************************************************************
* \brief  Stops and frees timer
*
* \param[in] timerID    Timer ID returned by TMR_AllocateTimer
*
* \return  tmrErrCode_t
************************************************************************************************* */
tmrErrCode_t TMR_FreeTimer(tmrTimerID_t timerID);

/*! ************************************************************************************************
* \brief  Checks if a timer ID is active
*
* \param[in] timerID    Timer ID returned by TMR_AllocateTimer
*
* \return  TRUE or FALSE
************************************************************************************************* */
bool_t TMR_IsTimerActive(tmrTimerID_t timerId);

/*! ************************************************************************************************
* \brief  Stops timer but keeps it allocated
*
* \param[in] timerID    Timer ID returned by TMR_AllocateTimer
*
* \return  tmrErrCode_t
************************************************************************************************* */
tmrErrCode_t TMR_StopTimer(tmrTimerID_t timerId);

/*! ************************************************************************************************
* \brief  Checks if a timer ID is ready
*
* \param[in] timerID    Timer ID returned by TMR_AllocateTimer
*
* \return  TRUE or FALSE
************************************************************************************************* */
bool_t TMR_IsTimerReady(tmrTimerID_t timerID);

/*! ************************************************************************************************
* \brief  Returns a 64-bit absolute timestamp
*
* \param[in] none
*
* \return  uint64_t
************************************************************************************************* */
uint64_t TMR_GetTimestamp (void);

/*! ************************************************************************************************
* \brief     Returns the remaining time until timeout
*
* \param[in] timerID    Timer ID returned by TMR_AllocateTimer
*
* \return    uint32_t   Remaining time in microseconds until first timer timeouts.
 ************************************************************************************************* */
uint32_t TMR_GetRemainingTime(tmrTimerID_t timerID);

#if defined(__cplusplus)
}
#endif

#endif /* TIMER_MANAGER_PORT_H_ */