/*! *********************************************************************************
 * \defgroup Handover
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022-2025 NXP
*
*
* \file gap_handover_types.h
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef GAP_HANDOVER_TYPES_H
#define GAP_HANDOVER_TYPES_H

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/

#define gHandoverAnchorMonitorStatusNesn_c      BIT0    /* NESN */
#define gHandoverAnchorMonitorStatusSn_c        BIT1    /* SN */
#define gHandoverAnchorMonitorStatusCrc_c       BIT2    /* CRC status valid (1) or invalid (0) */
#define gHandoverAnchorMonitorStatusRssi_c      BIT3    /* RSSI status valid (1) or invalid (0) */

#define gHandoverUpdateConnParamsStatusPhy_c    BIT3    /* Phy update procedure */
#define gHandoverUpdateConnParamsStatusChMap_c  BIT4    /* Channel map update procedure */

/*************************************************************************************
 *************************************************************************************
 * Public type definitions
 *************************************************************************************
 ************************************************************************************/
/*! Parameter structure to be used in the Gap_HandoverAnchorSearchStart function. */
typedef struct {
    uint32_t    startTime625;                       /*!< Slot of the anchor point timing of the connection event */
    uint16_t    startTimeOffset;                    /*!< Slot offset of the anchor point timing of the connection event */
    uint32_t    lastRxInstant;                      /*!< Last successful access address reception instant (unit 625us) */
    uint8_t     accessAddress[4];                   /*!< Access address */
    uint8_t     crcInit[3];                         /*!< CRC init */
    uint8_t     channelMap[gcBleChannelMapSize_c];  /*!< Channel map (FF FF FF FF 1F if all channels are used) */
    uint16_t    connInterval;                       /*!< Connection interval (unit 1.25ms) */
    uint16_t    latency;                            /*!< Latency (unit connection interval) */
    uint16_t    supervisionTimeout;                 /*!< Supervision timeout (unit 10ms) */
    uint16_t    eventCounter;                       /*!< Current connection event counter */
    uint8_t     centralSca;                         /*!< Sleep clock accuracy (0 to 7) */
    uint8_t     role;                               /*!< Role (0 for central and 1 for peripheral) */
    uint8_t     centralPhy;                         /*!< TX/RX PHY */
    uint8_t     seqNum;                             /*!< Sequence number; b1 and b0: the latest received SN and NESN; b5 and b4: the latest transmitted SN and NESN; other fields are reserved. */
    uint8_t     hopAlgo2;                           /*!< Hop algorithm (0/1 for hop algorithm #1 or #2) */
    uint8_t     unmappedChannelIndex;               /*!< Unmapped channel index (used only for hop algorithm #1) */
    uint8_t     hopIncrement;                       /*!< Hop increment */
    uint8_t     ucNbReports;                        /*!< Number of connection intervals to monitor
                                                            0: monitor continuously until stop is requested
                                                            1 - 255: stop automatically after monitoring the given number of connection intervals
                                                                In suspend or RSSI mode there is one anchor monitor event per connection interval.
                                                                In packet mode this is the number of connection intervals to monitor and there are usually
                                                                2 or more packet events per connection interval. */
    uint16_t    uiEventCounterAdvance;              /*!< Event counter delay to perform the search from the "current connection event counter" above (0 to start asap) */
    uint8_t     timeout;                            /*!< Timeout (unit connection interval, 0 if no timeout) */
    uint32_t    timingDiffSlot;                     /*!< Slot difference of the LL timing */
    uint16_t    timingDiffOffset;                   /*!< Slot offset difference of the LL timing */
    bleHandoverAnchorSearchMode_t mode;             /*!< Search mode setting */
} gapHandoverAnchorSearchStartParams_t;

/*! Parameter structure to be used in the Gap_HandoverTimeSyncTransmit function. */
typedef struct {
    bleHandoverTimeSyncEnable_t     enable;                                     /*!< 0/1 for disable/enable the transmit */
    uint8_t                         advChannel;                                 /*!< BLE channel (0 to 39) */
    uint8_t                         deviceAddress[gcBleDeviceAddressSize_c];    /*!< Packet identifier, can be the BD address or any value */
    uint8_t                         phys;                                       /*!< PHY (0/1/2/3 for 1M/2M/LR S8/LR S2) */
    uint8_t                         txPowerLevel;                               /*!< TX Power level 0 to 31 */
    uint8_t                         txIntervalSlots625;                         /*!< Packet interval in slots (4 to 255) */
} gapHandoverTimeSyncTransmitParams_t;

/*! Parameter structure to be used in the Gap_HandoverTimeSyncReceive function. */
typedef struct {
    bleHandoverTimeSyncEnable_t         enable;                                     /*!< 0/1 for disable/enable the receive */
    uint8_t                             scanChannel;                                /*!< BLE channel (0 to 39) */
    uint8_t                             deviceAddress[gcBleDeviceAddressSize_c];    /*!< Packet identifier, can be the BD address or any value */
    uint8_t                             phys;                                       /*!< PHY (0/1/2/3 for 1M/2M/LR S8/LR S2) */
    bleHandoverTimeSyncStopWhenFound_t  stopWhenFound;                              /*!< 0/1 for one shot disable/enable */
} gapHandoverTimeSyncReceiveParams_t;

typedef struct {
    uint16_t                connHandle;                             /*!< Handover Connection Handle */
    uint8_t                 status;                                 /*!< Status indicating the event trigger source. Bits 0-2: RFU; bit 3: phy update procedure; bit 4: channel map update procedure */
    uint8_t                 txRxPhy;                                /*!< TX/RX PHY */
    uint8_t                 seqNum;                                 /*!< Sequence number */
    uint8_t                 aChannelMap[gcBleChannelMapSize_c];     /*!< Channel map (FF FF FF FF 1F if all channels are used) */
    uint16_t                eventCounter;                           /*!< Current connection event counter */
} gapHandoverUpdateConnParams_t;

typedef struct {
    uint16_t        connHandle;             /*!< Handover Connection Handle */
    uint8_t         winSize;                /*!< Used to indicate the transmitWindowSize value as: transmitWindowSize = winSize * 1.25 ms */
    uint16_t        winOffset;              /*!< Used to indicate the transmitWindowOffset value as: transmitWindowOffset = winOffset * 1.25 ms */
    uint16_t        interval;               /*!< Used to indicate the connInterval value, as: connInterval = interval * 1.25 ms */
    uint16_t        latency;                /*!< Used to indicate the connPeripheralLatency value as: connPeripheralLatency = latency */
    uint16_t        timeout;                /*!< Used to indicate the connSupervisionTimeout value as: connSupervisionTimeout = timeout * 10 ms */
    uint16_t        instant;                /*!< Connection event counter value indicating when the new connection parameters are applied */
    uint16_t        currentEventCounter;    /*!< Current connection event counter */
} gapHandoverApplyConnectionUpdateProcedure_t;
#endif /* GAP_HANDOVER_TYPES_H */

/*!*************************************************************************************************
* @}
***************************************************************************************************/