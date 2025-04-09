/*! *********************************************************************************
* Copyright 2025 NXP
*
*
* \file
*
* This is the source file for the main entry point for a bare-metal application.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef _BLE_PORT_FSCI_H_
#define _BLE_PORT_FSCI_H_

/************************************************************************************
*************************************************************************************
* Includes
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
extern uint32_t mFsciInterfaceId;

/*************************************************************************************
**************************************************************************************
* Public macros
*************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn           void BLE_PortFsciInit(void)
*\brief        Initializes application specific functionality before the BLE stack init.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void BLE_PortFsciInit();

/*! *********************************************************************************
*\fn           void BLE_PortFsciBlock(uint8_t evtFlag)
*\brief        Block core 1 waiting for the corresponding event to be set after a message
*              is received from core 0
*
*\param  [in]  evtFlag        Event flag to wait for
*
*\retval       void.
********************************************************************************** */
void BLE_PortFsciBlock(uint8_t evtFlag);

/*! *********************************************************************************
*\fn           void .BLE_PortFsciUnblock(uint8_t operation)
*\brief        Unblock core 1 after receiving a message from core 0
*
*\param  [in]  operation    Blocking operation from which to free core 1
*
*\retval       void.
********************************************************************************** */
void BLE_PortFsciUnblock(uint8_t operation);

/*! *********************************************************************************
*\fn           void BLE_PortFsciRegisterBlockingEvent(void)
*\brief        Register a index to be used to unblock Core 0 for a specific blocking operation
*
*\param  [in]  none
*
*\retval       Index to be used when calliing BLE_PortFsciUnblock()
********************************************************************************** */
uint8_t BLE_PortFsciRegisterBlockingEvent (void);

#endif