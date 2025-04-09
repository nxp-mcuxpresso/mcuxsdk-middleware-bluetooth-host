/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file
*
* This is a source file for the common application NVM code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef __BUTTON_CORE0_H__
#define __BUTTON_CORE_0H__

#if defined (gFsciComponentButtonCore0_d) && (gFsciComponentButtonCore0_d > 0)
/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
*\brief  Initialize all button handlers
*
*\param  none
*
*\retval    none
********************************************************************************** */
void Button_InitCore0Handlers();
#endif /* #if defined (gFsciComponentButtonCore0_d) && (gFsciComponentButtonCore0_d > 0) */
#endif