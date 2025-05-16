/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file btcs_server_interface.h
*
* This is a header file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef __ADAPTER_RESET_CORE0_H__
#define __ADAPTER_RESET_CORE0_H__

#if defined (gFsciComponentResetCore0_d) && (gFsciComponentResetCore0_d > 0)
/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\brief  Initialize reset command handler
*
*\param  none
*
*\retval    none
********************************************************************************** */
void Reset_InitCore0Handlers(void);

#endif /* __ADAPTER_RESET_CORE0_H__ */
#endif /* gFsciComponentResetCore0_d */