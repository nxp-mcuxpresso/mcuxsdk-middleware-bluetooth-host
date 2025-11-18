/*! *********************************************************************************
 * \defgroup Localization Reader application
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* \file loc_reader_host.h
*
* Copyright 2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef LOC_READER_H
#define LOC_READER_H

#include "fsl_component_timer_manager.h"
/* Profile / Services */
#include "app_localization.h"
#include "host_app.h"

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/

/* Profile Parameters */
#define gReducedPowerMinAdvInterval_c   1600 /* 1 s */
#define gReducedPowerMaxAdvInterval_c   4000 /* 2.5 s */

#define gAdvTime_c                      30 /* 30 s*/
#define gGoToSleepAfterDataTime_c       5 /* 5 s*/

#ifndef gAppDeepSleepMode_c
#define gAppDeepSleepMode_c 1
#endif

#define smpEdiv                 0x1F99
#define mcEncryptionKeySize_c   16

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!*************************************************************************************************
*\fn    void BleApp_FactoryReset(void)
*\brief Handles Shell_Factory Reset Command.
*
*\param[in]   none
*
*\return      none
***************************************************************************************************/
void BleApp_FactoryReset(void);

/*!*************************************************************************************************
*\fn    void App_SetCsParams(deviceId_t deviceId, appLocalization_rangeCfg_t csConfigParams)
*\brief Send command to Core 1 to set given CS parameters.
*
*\param[in]   deviceId          Peer Identifier
*\param[in]   csConfigParams    New desired CS parameters
*
*\return      none
***************************************************************************************************/
void App_SetCsParams
(
    deviceId_t deviceId,
    appLocalization_rangeCfg_t csConfigParams
);

/*!*************************************************************************************************
*\fn    void BleApp_SetNewCsRole(void)
*\brief Sends the Role command to Core 1 and prints an informative the new role.
*
*\param[in]   none
*
*\return      none
***************************************************************************************************/
void BleApp_SetNewCsRole(void);

/*!*************************************************************************************************
*\fn    void BleApp_HandleNcpMsg(void* pMsg);
*\brief Handler function for messages received from Core 1
*
*\param[in]   pMsg    Pointer to message
*
*\return      none
***************************************************************************************************/
void BleApp_HandleNcpMsg
(
   ncpIqTransferMsgType_t eventType,
    uint8_t* pMsg
);

#ifdef __cplusplus
}
#endif


#endif /* LOC_READER_H */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
