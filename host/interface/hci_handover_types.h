/*!*************************************************************************************************
* \addtogroup Handover
* @{
***************************************************************************************************/
/*! *********************************************************************************
* Copyright 2022-2024 NXP
*
*
* \file hci_handover_types.h
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef HCI_HANDOVER_TYPES_H
#define HCI_HANDOVER_TYPES_H

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
#define gHciVendorHandoverGetTimeCommand_c                       0x009FU
#define gHciVendorHandoverSuspendTransmitCommand_c               0x00A0U
#define gHciVendorHandoverResumeTransmitCommand_c                0x00A1U
#define gHciVendorHandoverAnchorNotificationCommand_c            0x00A2U
#define gHciVendorHandoverDisconnect_c                           0x00A4U
#define gHciVendorHandoverAnchorSearchStartCommand_c             0x00A5U
#define gHciVendorHandoverAnchorSearchStopCommand_c              0x00A6U
#define gHciVendorHandoverConnect_c                              0x00A7U
#define gHciVendorHandoverGetLlContext_c                         0x00A8U
#define gHciVendorHandoverSetLlContext_c                         0x00A9U
#define gHciVendorHandoverTimeSyncTransmitCommand_c              0x00AAU
#define gHciVendorHandoverTimeSyncReceiveCommand_c               0x00ABU
#define gHciVendorHandoverGetCsLlContext_c                       0x00ACU
#define gHciVendorHandoverUpdateConnParams_c                     0x00ADU
/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum
{
    gHciVendorHandoverAnchorMonitorEvent_c                  = 0xA1,
    gHciVendorHandoverTimeSyncEvent_c                       = 0xA2,
    gHciVendorHandoverConnParamsUpdateEvent_c               = 0xA3,
    gHciVendorHandoverAnchorMonitorPacketEvent_c            = 0xA4,
    gHciVendorHandoverAnchorMonitorPacketContinueEvent_c    = 0xA5,
} hciVendorHandoverEventCode_tag;

#endif /* HCI_HANDOVER_TYPES_H */

/*!**********************************************************************************
* @}
************************************************************************************/