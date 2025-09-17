/*! *********************************************************************************
* Copyright 2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef __SENSORS_CORE0_H__
#define __SENSORS_CORE0_H__

#if defined (gFsciComponentSensorsCore0_d) && (gFsciComponentSensorsCore0_d > 0)
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
void SENSORS_InitCore0Handlers(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* gFsciComponentSensorsCore0_d */
#endif /* __SENSORS_CORE0_H__ */