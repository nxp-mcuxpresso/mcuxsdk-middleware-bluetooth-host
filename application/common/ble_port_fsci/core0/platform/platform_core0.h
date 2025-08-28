/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file btcs_server_interface.h
*
* This is a header file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef __PLATFORM_CORE0_H__
#define __PLATFORM_CORE0_H__

#if defined (gFsciComponentPlatformCore0_d) && (gFsciComponentPlatformCore0_d > 0)
/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\brief        Initialize Core 0 platform operations handlers
*
*\param  [in]  none
*
*\retval       none
********************************************************************************** */
void Platform_InitCore0Handlers(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* gFsciComponentPlatformCore0_d */
#endif /* __PLATFORM_CORE0_H__ */