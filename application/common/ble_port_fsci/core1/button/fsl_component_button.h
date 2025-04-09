/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file
*
* This is a source file for the common application NVM code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef __BUTTON_H__
#define __BUTTON_H__

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "fsl_common.h"

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
/*! @brief Definition of button handle size as HAL_GPIO_HANDLE_SIZE + button dedicated size. */
#define BUTTON_HANDLE_SIZE (16U + 24U)

/*! @brief The handle of button */
typedef void *button_handle_t;

/*!
 * @brief Defines the button handle
 *
 * This macro is used to define a 4 byte aligned button handle.
 * Then use "(button_handle_t)name" to get the button handle.
 *
 * The macro should be global and could be optional. You could also define button handle by yourself.
 *
 * This is an example,
 * @code
 * BUTTON_HANDLE_DEFINE(buttonHandle);
 * @endcode
 *
 * @param name The name string of the button handle.
 */
#define BUTTON_HANDLE_DEFINE(name) uint32_t name[((BUTTON_HANDLE_SIZE + sizeof(uint32_t) - 1U) / sizeof(uint32_t))]

/*!
 * @brief Defines the button handle array
 *
 * This macro is used to define a 4 byte aligned button handle array.
 * Then use "(button_handle_t)name[0]" to get the first button handle.
 *
 * The macro should be global and could be optional. You could also define these button handle by yourself.
 *
 * This is an example,
 * @code
 * BUTTON_HANDLE_DEFINE(buttonHandleArray, 1);
 * @endcode
 *
 * @param name The name string of the button handle array.
 * @param count The amount of button handle.
 */
#define BUTTON_HANDLE_ARRAY_DEFINE(name, count) \
    uint32_t name[count][((BUTTON_HANDLE_SIZE + sizeof(uint32_t) - 1U) / sizeof(uint32_t))]

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum _button_status
{
    kStatus_BUTTON_Success    = kStatus_Success,                     /*!< Success */
    kStatus_BUTTON_Error      = MAKE_STATUS(kStatusGroup_BUTTON, 1), /*!< Failed */
    kStatus_BUTTON_LackSource = MAKE_STATUS(kStatusGroup_BUTTON, 2), /*!< Lack of sources */
} button_status_t;

/*! @brief The event type of button */
typedef enum _button_event
{
    kBUTTON_EventOneClick = 0x01U, /*!< One click with short time, the duration of key down and key up is less than
                                      #BUTTON_SHORT_PRESS_THRESHOLD. */
    kBUTTON_EventDoubleClick,      /*!< Double click with short time, the duration of key down and key up is less than
                                      #BUTTON_SHORT_PRESS_THRESHOLD.      And the duration of the two button actions does not
                                      exceed #BUTTON_DOUBLE_CLICK_THRESHOLD. */
    kBUTTON_EventShortPress,       /*!< Press with short time, the duration of key down and key up is no less than
                                      #BUTTON_SHORT_PRESS_THRESHOLD       and less than #BUTTON_LONG_PRESS_THRESHOLD. */
    kBUTTON_EventLongPress,        /*!< Press with long time, the duration of key down and key up is no less than
                                      #BUTTON_LONG_PRESS_THRESHOLD. */
    kBUTTON_EventError,            /*!< Error event if the button actions cannot be identified. */
} button_event_t;

/*! @brief The callback message struct of button */
typedef struct _button_callback_message_struct
{
    button_event_t event;
} button_callback_message_t;

/*! @brief The callback function of button */
typedef button_status_t (*button_callback_t)(void *buttonHandle,
                                             button_callback_message_t *message,
                                             void *callbackParam);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
/*Define button handle*/
extern BUTTON_HANDLE_ARRAY_DEFINE(g_buttonHandle, gAppButtonCnt_c);
#endif /*gAppButtonCnt_c > 0*/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\brief        Installs a button callback and callback parameter.
*
*\param  [in]  buttonHandle     Button handle pointer
*\param  [in]  callback         The callback function.
*\param  [in]  callbackParam    The parameter of the callback function.
*
*\retval    kStatus_BUTTON_Success    Successfully install the callback
********************************************************************************** */
button_status_t BUTTON_InstallCallback
(
    button_handle_t buttonHandle,
    button_callback_t callback,
    void *callbackParam
);

#endif