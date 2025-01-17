/*! *********************************************************************************
 * \defgroup FSCI_BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2019, 2023 NXP
*
*
* \file
*
* This is a header file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef FSCI_BLE_GATT_DB_APP_TYPES_H
#define FSCI_BLE_GATT_DB_APP_TYPES_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble.h"
#include "gatt_types.h"
#include "gatt_db_app_interface.h"
#include "gatt_db_dynamic.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
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

#ifdef __cplusplus
extern "C" {
#endif
  
void fsciBleGattDbAppGetCharPresFormatFromBuffer
(
    gattDbCharPresFormat_t* pCharPresFormat, 
    uint8_t**               ppBuffer
);

void fsciBleGattDbAppGetBufferFromCharPresFormat
(
    gattDbCharPresFormat_t* pCharPresFormat, 
    uint8_t**               ppBuffer
);

#ifdef __cplusplus
}
#endif 

#endif /* FSCI_BLE_GATT_DB_APP_TYPES_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
