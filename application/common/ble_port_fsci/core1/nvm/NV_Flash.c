/*! *********************************************************************************
 * Copyright 2025 NXP
 * All rights reserved.
 *
 * \file
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ********************************************************************************** */

#include "NVM_Interface.h"
#include "FsciInterface.h"
#include "ble_port_fsci_op.h"

/******************************************************************************
 * Name: NvFormat
 * Description: Format the NV storage system. The function erases both virtual
 *              pages and then writes the page counter/ram table to active page.
 * Parameter(s): -
 * Return: gNVM_OK_c - if the operation completes successfully
 *         gNVM_FormatFailure_c - if the format operation fails
 *         gNVM_ModuleNotInitialized_c - if the NVM  module is not initialized
 *         gNVM_CriticalSectionActive_c - if the system has entered in a
 *                                        critical section
 *****************************************************************************/
NVM_Status_t NvFormat(void)
{
    FSCI_transmitPayload(BLE_PORT_FSCI_OG, (uint8_t)g_AppBleNvFormatCommand_c,
                         NULL, 0, gFsciInterface_c);
    return gNVM_OK_c;
}

