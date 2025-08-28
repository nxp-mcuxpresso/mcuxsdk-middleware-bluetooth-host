/*! *********************************************************************************
* Copyright 2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef __SHELL_CORE0_H__
#define __SHELL_CORE0_H__

#if defined (gFsciComponentShellCore0_d) && (gFsciComponentShellCore0_d > 0)
/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\brief  Initialize all shell handlers
*
*\param  none
*
*\retval    none
********************************************************************************** */
void Shell_InitCore0Handlers(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* gFsciComponentShellCore0_d */
#endif /* __SHELL_CORE0_H__ */