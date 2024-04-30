/*! *********************************************************************************
 * \defgroup Handover
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022-2024 NXP
*
*
* \file hci_handover_interface.h
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef HCI_HANDOVER_INTERFACE_H
#define HCI_HANDOVER_INTERFACE_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "hci_commands.h"
#include "hci_handover_types.h"
#include "hci_handover_commands.h"

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef struct hciVendorHandoverSuspendTransmitCommand_tag
{
    uint16_t                                     connectionHandle;  /*!< Connection identifier */
    bleHandoverSuspendTransmitMode_t             mode;              /*!< Suspend transmit mode */
    uint16_t                                     eventCounter;      /*!< Transmit suspension event counter, used only for mode gRssiSniffingMode_c */
    uint8_t                                      noOfConnIntervals; /*!< Number of connection intervals (0 for manual resume) */
} hciVendorHandoverSuspendTransmitCommand_t;

typedef struct hciVendorHandoverResumeTransmitCommand_tag
{
    uint16_t                                     connectionHandle;  /*!< Connection identifier */
} hciVendorHandoverResumeTransmitCommand_t;

typedef struct hciVendorHandoverAnchorNotificationCommand_tag
{
    uint16_t                                     connectionHandle;  /*!< Connection identifier */
    bleHandoverAnchorNotificationEnable_t        enable;            /*!< 0/1 for disable/enable the monitor setting */
    uint8_t                                      noOfReports;       /*!< Number of report events to sent; 0: send reports continuously until stop is requested; 1 - 255: stop automatically the monitoring after sending the given number of reports */
} hciVendorHandoverAnchorNotificationCommand_t;

typedef struct hciVendorHandoverAnchorSearchStartCommand_tag
{
    uint8_t     accessAddress[4];                   /*!< Access address */
    uint8_t     crcInit[3];                         /*!< CRC init */
    uint16_t    connInterval;                       /*!< Connection interval (unit 1.25ms) */
    uint16_t    supervisionTimeout;                 /*!< Supervision timeout (unit 10ms) */
    uint16_t    latency;                            /*!< Latency (unit connection interval) */
    uint8_t     channelMap[gcBleChannelMapSize_c];  /*!< Channel map (FF FF FF FF 1F if all channels are used) */
    uint8_t     hopAlgo2;                           /*!< Hop algorithm (0/1 for hop algorithm #1 or #2) */
    uint8_t     hopIncrement;                       /*!< Hop increment */
    uint8_t     unmappedChannelIndex;               /*!< Unmapped channel index (used only for hop algorithm #1) */
    uint8_t     centralSca;                         /*!< Sleep clock accuracy (0 to 7) */
    uint8_t     role;                               /*!< Role (0 for central and 1 for peripheral) */
    uint8_t     centralPhy;                         /*!< TX/RX PHY */
    uint8_t     seqNum;                             /*!< Sequence number; b1 and b0: the latest received SN and NESN; b5 and b4: the latest transmitted SN and NESN; other fields are reserved. */
    uint16_t    eventCounter;                       /*!< Current connection event counter */
    uint32_t    startTime625;                       /*!< Slot of the anchor point timing of the connection event */
    uint16_t    startTimeOffset;                    /*!< Slot offset of the anchor point timing of the connection event */
    uint32_t    lastRxInstant;                      /*!< Last successful access address reception instant (unit 625us) */
    uint16_t    uiEventCounterAdvance;              /*!< Event counter delay to perform the search from the "current connection event counter" above (0 to start asap) */
    uint32_t    timingDiffSlot;                     /*!< Slot difference of the LL timing */
    uint16_t    timingDiffOffset;                   /*!< Slot offset difference of the LL timing */
    uint8_t     timeout;                            /*!< Timeout (unit connection interval, 0 if no timeout) */
    uint8_t     ucNbReports;                        /*!< Number of connection intervals to monitor
                                                            0: monitor continuously until stop is requested
                                                            1 - 255: stop automatically after monitoring the given number of connection intervals
                                                                In suspend or RSSI mode there is one anchor monitor event per connection interval.
                                                                In packet mode this is the number of connection intervals to monitor and there are usually
                                                                2 or more packet events per connection interval. */
    uint8_t     mode;                               /*!< Search mode setting */
} hciVendorHandoverAnchorSearchStartCommand_t;

typedef struct hciVendorHandoverAnchorSearchStopCommand_tag
{
    uint16_t    connectionHandle;   /*!< Connection identifier */
} hciVendorHandoverAnchorSearchStopCommand_t;

/*! OCF 0x00A8 */
typedef struct hciVendorhandoverGetLlContextCommand_tag
{
    uint16_t            connectionHandle;   /*!< Connection identifier */
} hciVendorhandoverGetLlContextCommand_t;

/*! OCF 0x00A9 */
typedef struct hciVendorhandoverSetLlContextCommand_tag
{
    uint16_t            connectionHandle;                               /*!< Connection identifier */
    uint16_t            maskCmd;                                        /*!< LL context bitmap indicating the connection handover context */
    uint8_t             llContextLength;                                /*!< Context data length */
    uint8_t             llContext[gVendorHandoverMaxCsLlContextSize_c]; /*!< LL Context */
} hciVendorhandoverSetLlContextCommand_t;

/*! OCF 0x00AA */
typedef struct hciVendorHandoverTimeSyncTransmitCommand_tag
{
    bleHandoverTimeSyncEnable_t enable;                                     /*!< 0/1 for disable/enable the transmit */
    uint8_t                     advChannel;                                 /*!< BLE channel (0 to 39) */
    uint8_t                     accessAddress[4];                           /*!< BLE spec compliant access address to use */
    uint8_t                     deviceAddress[gcBleDeviceAddressSize_c];    /*!< Packet identifier, can be the BD address or any value */
    uint8_t                     phys;                                       /*!< PHY (0/1/2/3 for 1M/2M/LR S8/LR S2) */
    uint8_t                     txPowerLevel;                               /*!< TX Power level 0 to 31 */
    uint8_t                     txIntervalSlots625;                         /*!< Packet interval in slots (4 to 255) */
} hciVendorHandoverTimeSyncTransmitCommand_t;

/*! OCF 0x00AB */
typedef struct hciVendorHandoverTimeSyncReceiveCommand_tag
{
    bleHandoverTimeSyncEnable_t        enable;                                  /*!< 0/1 for disable/enable the receive */
    uint8_t                            scanChannel;                             /*!< BLE channel (0 to 39) */
    uint8_t                            accessAddress[4];                        /*!< BLE spec compliant access address to use */
    uint8_t                            deviceAddress[gcBleDeviceAddressSize_c]; /*!< Packet identifier, can be the BD address or any value */
    uint8_t                            phys;                                    /*!< PHY (0/1/2/3 for 1M/2M/LR S8/LR S2) */
    bleHandoverTimeSyncStopWhenFound_t stopWhenFound;                           /*!< 0/1 for one shot disable/enable */
} hciVendorHandoverTimeSyncReceiveCommand_t;

/*! OCF 0x00AC */
typedef struct hciVendorhandoverGetCsLlContextCommand_tag
{
    uint16_t            connectionHandle;   /*!< Connection identifier */
} hciVendorhandoverGetCsLlContextCommand_t;

/*! OCF 0x00AD */
typedef struct hciVendorHandoverUpdateConnParamsCommand_tag
{
    uint16_t                connHandle;                             /*!< Handover Connection Handle */
    uint8_t                 status;                                 /*!< Status indicating the event trigger source. Bits 0-2: RFU; bit 3: phy update procedure; bit 4: channel map update procedure */
    uint8_t                 txRxPhy;                                /*!< TX/RX PHY */
    uint8_t                 seqNum;                                 /*!< Sequence number */
    uint8_t                 aChannelMap[gcBleChannelMapSize_c];     /*!< Channel map (FF FF FF FF 1F if all channels are used) */
    uint16_t                eventCounter;                           /*!< Current connection event counter */
} hciVendorHandoverUpdateConnParamsCommand_t;
/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*! *********************************************************************************
* \brief        The function sends the HCI Vendor Handover Get Time
*               (OGF : 0x3f; OCF : 0x009F) command to the Controller.
*
* \param[in]    None
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverGetTime(void);

/*! *********************************************************************************
* \brief        The function sends the HCI Vendor Handover Suspend Transmit
*               (OGF : 0x3f; OCF : 0x00A0) command to the Controller.
*
* \param[in]    pParam  Pointer to structure containing the Suspend Transmit parameters.
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverSuspendTransmit(hciVendorHandoverSuspendTransmitCommand_t *pParam);

/*! *********************************************************************************
* \brief        The function sends the HCI Vendor Handover Resume Transmit
*               (OGF : 0x3f; OCF : 0x00A1) command to the Controller.
*
* \param[in]    pParam  Pointer to structure containing the Resume Transmit parameters.
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverResumeTransmit(hciVendorHandoverResumeTransmitCommand_t *pParam);

/*! *********************************************************************************
* \brief        The function sends the HCI Vendor Handover Anchor Notification
*               (OGF : 0x3f; OCF : 0x00A2) command to the Controller.
*
* \param[in]    pParam  Pointer to structure containing the Anchor Notification parameters.
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverAnchorNotification(hciVendorHandoverAnchorNotificationCommand_t *pParam);

/*! *********************************************************************************
* \brief        The function sends the HCI Vendor Handover Anchor Search Start
*               (OGF : 0x3f; OCF : 0x00A5) command to the Controller.
*
* \param[in]    pParam  Pointer to structure containing the Anchor Search Start parameters.
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverAnchorSearchStart(hciVendorHandoverAnchorSearchStartCommand_t *pParam);

/*! *********************************************************************************
* \brief        The function sends the HCI Vendor Handover Anchor Search Stop
*               (OGF : 0x3f; OCF : 0x00A6) command to the Controller.
*
* \param[in]    pParam  Pointer to structure containing the Anchor Search Stop parameters.
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverAnchorSearchStop(hciVendorHandoverAnchorSearchStopCommand_t *pParam);

/*! *********************************************************************************
* \fn           bleResult_t Hci_VendorHandoverDisconnect(uint16_t connectionHandle)
*
* \brief        The function sends the HCI Vendor Handover Disconnect
*               (OGF : 0x3f; OCF : 0x00A4) command to the Controller.
*
* \param[in]    connectionHandle    Connection handle of the peer device.
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverDisconnect(uint16_t connectionHandle);

/*! *********************************************************************************
* \fn           bleResult_t Hci_VendorHandoverConnect(uint16_t connectionHandle, uint8_t anchorNotification)
*
* \brief        The function sends the HCI Vendor Handover Connect
*               (OGF : 0x3f; OCF : 0x00A7) command to the Controller.
*
* \param[in]    connectionHandle    Connection handle of the peer device.
* \param[in]    anchorNotification  Anchor search setting after the handover,
*               0 - stop anchor notification, 1 - keep anchor notification.
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverConnect(uint16_t connectionHandle, uint8_t anchorNotification);

/*! *********************************************************************************
* \fn           bleResult_t Hci_VendorHandoverGetLlContext(hciVendorhandoverGetLlContextCommand_t *pParam)
*
* \brief        The function sends the HCI Vendor Handover Get LL Context
*               (OGF : 0x3f; OCF : 0x00A8) command to the Controller.
*
* \param[in]    pParam  pointer to a structure containing the connection handle.
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverGetLlContext(hciVendorhandoverGetLlContextCommand_t *pParam);

/*! *********************************************************************************
* \fn           bleResult_t Hci_VendorHandoverGetCsLlContext(hciVendorhandoverGetCsLlContextCommand_t *pParam)
*
* \brief        The function sends the HCI Vendor Handover Get CS LL Context
*               (OGF : 0x3f; OCF : 0x00AC) command to the Controller.
*
* \param[in]    pParam  pointer to a structure containing the connection handle.
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverGetCsLlContext(hciVendorhandoverGetCsLlContextCommand_t *pParam);

/*! *********************************************************************************
* \fn           bleResult_t Hci_VendorHandoverSetLlContext(hciVendorhandoverSetLlContextCommand_t *pParam)
*
* \brief        The function sends the HCI Vendor Handover Set LL Context
*               (OGF : 0x3f; OCF : 0x00A9) command to the Controller.
*
* \param[in]    pParam  pointer to a structure containing the connection handle,
*                       maskCmd and LL context.
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverSetLlContext(hciVendorhandoverSetLlContextCommand_t *pParam);

/*! *********************************************************************************
* \fn           bleResult_t Hci_VendorHandoverTimeSyncTransmit(hciVendorHandoverTimeSyncTransmitCommand_t *pParam)
*
* \brief        The function sends the HCI Vendor Handover Time Sync Transmit
*               (OGF : 0x3f; OCF : 0x00AA) command to the Controller.
*
* \param[in]    pParam  pointer to a structure containing the time sync transmit parameters
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverTimeSyncTransmit(hciVendorHandoverTimeSyncTransmitCommand_t *pParam);

/*! *********************************************************************************
* \fn           bleResult_t Hci_VendorHandoverTimeSyncReceive(hciVendorHandoverTimeSyncReceiveCommand_t *pParam)
*
* \brief        The function sends the HCI Vendor Handover Time Sync Receive
*               (OGF : 0x3f; OCF : 0x00AB) command to the Controller.
*
* \param[in]    pParam  pointer to a structure containing the time sync receive parameters
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverTimeSyncReceive(hciVendorHandoverTimeSyncReceiveCommand_t *pParam);

/*! *********************************************************************************
* \fn           bleResult_t Hci_VendorHandoverUpdateConnParams(hciVendorHandoverUpdateConnParamsCommand_t *pParam)
*
* \brief        The function sends the HCI Vendor Handover Update Connection Parameters
*               (OGF : 0x3f; OCF : 0x00AD) command to the Controller.
*
* \param[in]    pParam  pointer to a structure containing the connection parameters to be updated
* \param[out]   None
*
* \return       Status
*
* \remarks      HCI Vendor Specific Command
*
********************************************************************************** */
bleResult_t Hci_VendorHandoverUpdateConnParams(hciVendorHandoverUpdateConnParamsCommand_t *pParam);

#ifdef __cplusplus
}
#endif

#endif /* HCI_HANDOVER_INTERFACE_H */

/*! *********************************************************************************
* @}
********************************************************************************** */