/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file
*
* This is a source file for the common application NVM code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef __LED_CORE0_H__
#define __LED_CORE0_H__

#if defined (gFsciComponentLedCore0_d) && (gFsciComponentLedCore0_d > 0)
 /*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\brief  Initialize all LED handlers
*
*\param  none
*
*\retval    none
********************************************************************************** */
void LED_InitCore0Handlers(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* gFsciComponentLedCore0_d */
#endif /* __LED_CORE0_H__ */