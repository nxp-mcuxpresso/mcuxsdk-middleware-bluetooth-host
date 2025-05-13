/*! *********************************************************************************
* Copyright 2025 NXP
*
* This is the source file for the main entry point for a bare-metal application.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef APP_NVM_H
#define APP_NVM_H

/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "ble_general.h"

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn        bleResult_t App_NcpNvmInit(void)
*\brief     NCP NVM initialization function.
*
*\retval    gBleAlreadyInitialized_c    NCP NVM already initialized
*\retval    gBleOsError_c               Fail to create OSA event
*\retval    gBleUnexpectedError_c       Error occured while sending a request to the host
*\retval    gBleSuccess_c               Operation successfull
********************************************************************************** */
bleResult_t App_NcpNvmInit(void);

/*! *********************************************************************************
*\fn        void App_NvmIdle(void)
*\brief     Send BLE NVM requests to Host to save modified data sets.
*
*\retval    void
********************************************************************************** */
void App_NvmIdle(void);

#endif /* APP_NVM_H */