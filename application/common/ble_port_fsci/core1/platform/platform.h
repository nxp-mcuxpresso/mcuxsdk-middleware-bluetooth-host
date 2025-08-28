/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file btcs_server_interface.h
*
* This is a header file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

/************************************************************************************
*************************************************************************************
* Includes
*************************************************************************************
************************************************************************************/
#include "fsl_common.h"
#include "ble_general.h"

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\brief        Request Core 0 application to send the Public Device Address.
*
*\param  [in]  none
*
*\retval       bleResult_t  gBleOutOfMemory_c in case of memory allocation failure
*                           gBleSuccess_c otherwise.
********************************************************************************** */
bleResult_t BleApp_ReadPublicDeviceAddress(void);

/*! *********************************************************************************
*\fn        bleResult_t Hcit_PktReceived(hciPacketType_t type, void* packet, uint16_t size)
*\brief     This is the generic callback for Bluetooth events.
*
*\param[in]     type       Packet type
*\param[in]     packet     Pointer to packet
*\param[in]     size       Packet size
*
*\retval    gBleSuccess_c               Operation successfull
********************************************************************************** */
bleResult_t Hcit_PktReceived
(
    hciPacketType_t type,
    void* packet,
    uint16_t size
);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* __PLATFORM_H__ */