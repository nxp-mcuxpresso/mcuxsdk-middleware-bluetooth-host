/*! *********************************************************************************
* \addtogroup FSCI BLE application
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2023, 2025-2026 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
/* Framework / Drivers */
#include "fsl_component_led.h"
#include "fsl_component_button.h"
#include "fsl_component_serial_manager.h"

#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0) 
#include "PWR_Interface.h"
#endif

#include "app.h"
#include "FsciInterface.h"
#include "FsciCommunication.h"
#include "fwk_platform_ble.h"

/*  Application */
#include "fsci_ble_interface.h"
#include "fsci_ble_gap.h"
#include "ble_fsci_black_box.h"
#include "app_conn.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0) 
#ifndef gAppLowPowerModeConstraints_c
#define gAppLowPowerModeConstraints_c   PWR_DeepSleep         
#endif
#endif

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)
static void BluetoothLEHost_Initialized(void);
#endif

#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0) 
static void BleApp_ChangeLowPowerModeConstraints(uint8_t lpMode);
#endif

#if defined(gIntrusionDetectionSystem_d) && (gIntrusionDetectionSystem_d == 1U)
static void BleApp_IdsCallback(idsEventData_t *pEventData);
#endif

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief  This is the initialization function for each application. This function
*         should contain all the initialization code required by the Bluetooth demo.
********************************************************************************** */
void BluetoothLEHost_AppInit(void)
{
    /* Init FSCI */
    FSCI_commInit( g_fsciHandleList );

    /* Register BLE handlers in FSCI */
    fsciBleRegister(0);

    /* Register generic callback */
    BluetoothLEHost_SetGenericCallback(BleApp_GenericCallback);

#if defined(gIntrusionDetectionSystem_d) && (gIntrusionDetectionSystem_d == 1U)
    BluetoothLEHost_SetIdsCallback(BleApp_IdsCallback, gGapIdsAllFlags_c);
#endif

#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)
    /* Initialize Bluetooth Host Stack */
    BluetoothLEHost_Init(BluetoothLEHost_Initialized);
#else
    BluetoothLEHost_Init(NULL);
#endif
}

/*! *********************************************************************************
* \brief  This is the generic callback for Bluetooth events.
********************************************************************************** */
void BleApp_GenericCallback(gapGenericEvent_t* pGenericEvent)
{
    fsciBleGapGenericEvtMonitor(pGenericEvent);
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
 * \brief        Configures BLE Stack after initialization
 *
 ********************************************************************************** */
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0) 
static void BluetoothLEHost_Initialized(void)
{

    /* Add post host initialization application configuration here. */
    BleApp_ChangeLowPowerModeConstraints(gAppLowPowerModeConstraints_c);
}
#endif

#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0) 
/*! *********************************************************************************
 * \brief        Changes low power constraints.
 *
 ********************************************************************************** */
static void BleApp_ChangeLowPowerModeConstraints(uint8_t lpMode)
{
#if defined(gAppLowpowerEnabled_d) && (gAppLowpowerEnabled_d>0)
    static uint8_t currentLPMode = (uint8_t)PWR_DeepSleep;
    if(currentLPMode != lpMode)
    {
        (void)PWR_ReleaseLowPowerModeConstraint((PWR_LowpowerMode_t)currentLPMode);
        (void)PWR_SetLowPowerModeConstraint((PWR_LowpowerMode_t)lpMode);
        currentLPMode = lpMode;
    }
#else    
    (void)lpMode;
#endif
}
#endif

#if defined(gIntrusionDetectionSystem_d) && (gIntrusionDetectionSystem_d == 1U)
/*! *********************************************************************************
* \brief        Application callback for handling intrusion events reported by
*               the Host stack.
*
********************************************************************************** */
static void BleApp_IdsCallback(idsEventData_t *pEventData)
{
    fsciBleIdsEvtMonitor(pEventData);
}
#endif /* gIntrusionDetectionSystem_d */
/*! *********************************************************************************
* @}
********************************************************************************** */
