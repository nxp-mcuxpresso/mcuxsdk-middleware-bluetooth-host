/*! *********************************************************************************
* Copyright 2024-2026 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef __SENSORS_CORE1_H__
#define __SENSORS_CORE1_H__

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "ble_port_fsci_op.h"
#include "FsciInterface.h"

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\brief        Dummy placeholder for SENSORS_GetBatteryLevel()
*
*\param[in]    none
*
*\retval       void.
********************************************************************************** */
uint8_t SENSORS_GetBatteryLevel(void);

/*! *********************************************************************************
*\brief        Sends message to core0 to trigger temperature measurement.
*
*\param[in]    none
*
*\retval       void.
********************************************************************************** */
#define SENSORS_TriggerTemperatureMeasurement()    FSCI_transmitPayload(BLE_PORT_FSCI_OG, \
                                                       (uint8_t)g_SensorsTriggerTemperatureMeasurement_c, \
                                                       NULL, 0U, gFsciInterface_c)


/*! *********************************************************************************
*\brief        Sends message to core0 to refresh temperature value in RAM.
*
*\param[in]    none
*
*\retval       void.
********************************************************************************** */
int32_t SENSORS_RefreshTemperatureValue(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* __SENSORS_CORE1_H__ */