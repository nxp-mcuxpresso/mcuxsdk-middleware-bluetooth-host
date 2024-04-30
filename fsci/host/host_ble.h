/*! *********************************************************************************
* \addtogroup HOST_BBOX_UTILITY
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2017, 2023 NXP
*
*
* \file
*
* This is a header file for the Host - Blackbox Utility management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _HOST_BLE_H
#define _HOST_BLE_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

bleResult_t Ble_GetCmdStatus(bool_t bHasOutParams);

void Ble_OutParamsReady(void);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif 

#endif /* _HOST_BLE_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
