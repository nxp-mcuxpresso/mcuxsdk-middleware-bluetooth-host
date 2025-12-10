/*! *********************************************************************************
* \defgroup Ranging Service
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022-2025 NXP
*
*
* \file
*
* This file is the interface file for the Ranging Service
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef RANGING_INTERFACE_H
#define RANGING_INTERFACE_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#include "channel_sounding.h"
#include "gatt_server_interface.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
/* If the Antenna Path Filter is defined as 0bxxxx1111, then report all 4 antenna paths */
#define gAntennaPathFilterAllowAll_c    15U

/* Default filter value - no filter applied */
#define gNoFilter_c                     0xFFFFFFFFU

/* Antenna Configuratin Index 2:2 */
#define gAntennaCfgIdx7_c               7U

/* The maximum number of antenna pairs used. */
#define ISP_MAX_NO_ANTENNAS             4U

/* Maximum number of subevents to be buffered */
#define gRasMaxNumSubevents_c           10U /* 32 */

/* Maximum number of CS steps to be buffered */
#define gRasMaxNumSteps_c               160U

/* Maximum data size for a CS subevent */
#define gRasCsSubeventDataSize_c        2300U

/* Size of the header for RAS notifications */
#define gRasNotificationHeaderize       sizeof(uint16_t) + sizeof(uint8_t) + \
                                        sizeof(rasSubeventHeader_t)

/* Number of records to store in the Stored Ranging Data Characteristic */
#define gRasNumberOfSupportedRecords_c  1U

/* Minimum value of the RAS segmentation counter */
#define gRasSegmentCounterMinValue_c    0U

/* Maximum value of the RAS segmentation counter */
#define gRasSegmentCounterMaxValue_c    63U

/* Maximum number of segments */
#define gRASMaxNoOfSegments_c           12U

/* Size of Tone_PCT field for mode 2 data */
#define gTone_PCTSize_c                 3U

/* Size of Packet_PCT field for mode 1 and 3 data */
#define gPacket_PCTSize_c               4U

/* Timeout for Data Ready indication after starting CS procedure */
#define gRreqTimeoutDataReadySeconds_c  3U

/* Timeout for Ranging Data ind/not */
#define gRreqTimeoutDataSeconds_c       1U

/* Timeout for ACK from client after sending Complete */
#define gRsspTimeoutCompleteSeconds_c   1U

/* PCT IQ format */
#define gPctFormatIQ_c                  0U

/* PCT phase format */
#define gPctFormatPhase_c               1U

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
/* Lookup table for RAS-CP command size */
extern uint8_t maRasCPCommandSizes[5U];

/* Lookup table for RAS-CP response size */
extern uint8_t maRasCPResponseSizes[3U];

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
/*! RAP timeout categories */
typedef enum
{
    rreqWaitingForDataReady_c   = 0U,
    rreqWaitingForRangingData_c = 1U
} rreqTimeoutReason_tag;

typedef uint8_t rreqTimeoutReason_t;

/*! RAP timer callback parameters */
typedef struct rreqTimeoutData_tag
{
    deviceId_t          deviceId;
    rreqTimeoutReason_t reason;
} rreqTimeoutData_t;

/*! Subevent header information */
typedef PACKED_STRUCT csRasSubeventHeader_tag
{
    uint16_t        startACLConnEvent;
    uint16_t        frequencyCompensation;
    int8_t          referencePowerLevel;
    uint8_t         procedureDoneStatus;
    uint8_t         subeventDoneStatus;
    uint8_t         abortReason;
    uint8_t         numStepsReported;
} csRasSubeventHeader_t;

/*! Subevent indexing for local data */
typedef PACKED_STRUCT measSubEvtData_tag
{
    uint32_t dataIdx;           /* Data index for the subevent. */
    uint16_t dataSize;          /* Data size for the subevent. */
    uint16_t filteredDataSize;  /* Resulting filtered data size for the subevent. */
    uint16_t currentDataSize;   /* Data size already sent/received for the current subevent. */
    csRasSubeventHeader_t subevtHeader;
} measSubEvtData_t;

/*! Format of local data */
typedef PACKED_STRUCT rasMeasurementData_tag
{
    deviceId_t          deviceId;
    uint8_t             configId;
    uint16_t            procedureCounter;
    uint8_t             numAntennaPaths;
    uint8_t             totalNumSteps;
    int8_t              selectedTxPower;
    uint32_t            dataIndex;
    uint16_t            totalSentRcvDataIndex;
    uint16_t            dataParsedLen;
    uint8_t             subeventIndex;
    uint8_t             crtNumSteps;
    measSubEvtData_t    aSubEventData[gRasMaxNumSubevents_c];
    uint8_t             *pData;
} rasMeasurementData_t;

/* Segment information for Retrieve Lost Ranging Data Segments command */
typedef struct segmentData_tag
{
    uint8_t     segmentIdx;
    uint16_t    dataIdxStart;
    uint16_t    dataSize;
} segmentData_t;

/*! Ranging Service - Static Configuration */
typedef PACKED_STRUCT rasStaticConfig_tag
{
    uint16_t              serviceHandle;
    uint16_t              controlPointHandle;
    uint16_t              onDemandDataHandle;
    uint16_t              realTimeDataHandle;
    uint16_t              dataReadyHandle;
    uint16_t              dataOverwrittenHandle;
    uint16_t              featuresHandle;
} rasStaticConfig_t;

/*! Ranging Service - Dynamic Configuration */
typedef PACKED_STRUCT rasDynamicConfig_tag
{
    rasMeasurementData_t* pCfg;
    uint8_t*              pRangingDataBody;
} rasDynamicConfig_t;

/*! Ranging Service - RAS Data Ready Indication */
typedef struct rasDataReadyIndication_tag
{
    uint16_t                procedureIndex;
} rasDataReadyIndication_t;

/*! Ranging Service - RAS Control Point Indication Types */
typedef enum {
    rasDataReady_c       = 0x00U,
    rasDataOverwritten_c = 0x01U,
} indicationType_t;

/* BIT values to be set in the segmentation counter for the first and last segment */
typedef enum
{
    gRasNotifFirstSegment_c    = BIT0, /*!< First segment of data */
    gRasNotifLastSegment_c     = BIT1, /*!< Last segment of data */
} rasNotifSegmentType_tag;

/*!< RAS notification payload - ranging data header */
typedef struct rasRangingDataHeader_tag
{
    uint16_t     procCounterConfigId; /* 12 bits for procCounter, 4 for configId */
    uint8_t      selectedTxPower;     /* Units: dbm */
    uint8_t      rfu;                 /* 8 bits Reserved for future use */
} rasRangingDataHeader_t;

/*!< RAS notification payload - subevent header and data */
typedef PACKED_STRUCT rasSubeventDataHeader_tag
{
    uint16_t        startACLConnEvent;
    uint16_t        frequencyCompensation;
    uint8_t         procAndSubeventDoneStatus;  /* 4 bits for procedure done status
                                                   4 bits for subevent done status */
    uint8_t         procAndSubeventAbortReason; /* 4 bits for abort reason if procedure done status is 0x0F, otherwise 0
                                                   4 bits for abort reason if subevent done status is 0x0F, otherwise 0 */
    int8_t          referencePowerLevel;
    uint8_t         numStepsReported;
} rasSubeventDataHeader_t;

/*!< RAS Control Point request operation codes */
typedef uint8_t rasCPCommandsOpCode_t;

typedef enum
{
    getRangingDataOpCode_c          = 0x00U,
    ackRangingDataOpCode_c          = 0x01U,
    retrLostRangingDataOpCode_c     = 0x02U,
    abortOperationOpCode_c          = 0x03U,
    setFilterOpCode_c               = 0x04U,
} rasControlPointOperations_tag;

/*!< RAS Control Operator Values */
typedef uint8_t rasCPOperatorCode_t;

/*!< RAS Retrieve Lost Data CP command operands */
typedef struct rasRetrLostRangingDataParams_tag {
    uint16_t   procCounter;
    uint8_t    startAbsSegment;
    uint8_t    endAbsSegment;
} rasRetrLostRangingDataParams_t;

/*! RAS Control Point Command payload */
typedef PACKED_STRUCT rasControlPoint_tag
{
    rasCPCommandsOpCode_t                 cmdOpCode;
    union {
        uint16_t                          procCounter;
        rasRetrLostRangingDataParams_t    retrLostDataParams;
        uint16_t                          filterValue;
    } cmdParameters;
} rasControlPointReq_t;

/*!< RAS Control Point response operation codes */
typedef uint8_t rasControlPointRspOpCode_t;

typedef enum
{
    completeProcDataRspOpCode_c              = 0x00U,
    completeLostDataSegmentResponseOpCode_c  = 0x01U,
    responseOpCode_c                         = 0x02U,
} rasControlPointRspOpCode_tag;

/*!< RAS Control Point Response Code Value */
typedef uint8_t rasControlPointRspCodeValues_t;

typedef enum
{
    gRasErrorRfu_c                           = 0x00U, /*!< Reserved for future use */
    gRasSuccess_c                            = 0x01U, /*!< Procedure executed successfully */
    gRasOpCodeNotSupportedError_c            = 0x02U, /*!< OpCode that was written to the RAS-CP is not supported by the RRSP */
    gRasInvalidParameterError_c              = 0x03U, /*!< Parameter that was written to the RAS-CP is invalid */
    gRasSuccessPersisted_c                   = 0x04U, /*!< Successful write operation where the values written to RAS-CP are being persisted */
    gRasAbortUnsuccessfulError_c             = 0x05U, /*!< Unable to process the Abort Operation procedure */
    gRasProcedureNotCompletedError_c         = 0x06U, /*!< Server needed to interrupt its data transfer  */
    gRasServerBusyError_c                    = 0x07U, /*!< Server busy - cannot process request */
    gRasNoRecordsFoundError_c                = 0x08U, /*!< Requested ranging counter was not found*/
} rasControlPointRspCodeValues_tag;

/*!< RAS Control Point Response opcodes */
typedef struct rasCtrPointRsp_tag
{
    rasControlPointRspCodeValues_t  rspValue;
} rasCtrPointRsp_t;

/*! RAS Control Point Response payload */
typedef struct rasControlPointRsp_tag
{
    rasControlPointRspOpCode_t         rspOpCode;
    PACKED_UNION
    {
        uint16_t                       procCounter;
        rasRetrLostRangingDataParams_t complLostRsp;
        rasCtrPointRsp_t               rspPayload;
    } cmdParameters;
} rasControlPointRsp_t;

typedef enum
{
    noTransferInProgress_c          = 0x00U, /*!< No transfer in progress */
    sendingRangingData_c            = 0x01U, /*!< Sending notifications/indications */
    waitingAckRangingData_c         = 0x02U, /*!< Transfer completed, awaiting ACK ranging data */
    segmRetransmInProgress_c        = 0x03U, /*!< Segment retransmission in progress */
    waitAckRangingDataRetransm_c    = 0x04U, /*!< segment retransmission complete, awaiting ACK ranging data */
} rasServerTransferStatus_tag;

/*!< RAS Transfer status */
typedef uint8_t rasServerTransferStatus_t;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!**********************************************************************************
* \brief        Starts Ranging Service functionality
*
* \param[in]    pServiceConfig  Pointer to structure that contains server
*                               configuration information.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_Start
(
    rasStaticConfig_t *pServiceConfig
);

/*!**********************************************************************************
* \brief        Stops Ranging Service functionality
*
* \param[in]    pServiceConfig  Pointer to the server configuration information.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_Stop
(
    rasStaticConfig_t *pServiceConfig
);

/*!**********************************************************************************
* \brief        Subscribes a GATT client to the Ranging service
*
* \param[in]    clientDeviceId   Peer identifier
* \param[in]    realTime         TRUE if peer has enabled real-time data transfer,
*                                FALSE for on-demand.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_Subscribe
(
    deviceId_t clientDeviceId,
    bool_t     realTime
);

/*!**********************************************************************************
* \brief        Unsubscribes a GATT client from the Ranging service
*
* \param[in]    clientDeviceId  Identifier of the peer to be unsubscribed
* \param[in]    disconnect      TRUE if peer has disconnected, FALSE otherwise
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_Unsubscribe
(
    deviceId_t clientDeviceId,
    bool_t     disconnect
);

/*!**********************************************************************************
* \brief        Send a Data Ready indication or notification to the peer after all
*               procedure data has been received from the local controller.
*
* \param[in]    clientDeviceId              Identifier of the peer
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_SendDataReady
(
    deviceId_t   clientDeviceId
);

/*!**********************************************************************************
* \brief        Send a Data Overwritten indication or notification to the peer if new.
*               procedure data was received before the peer send the Delete Data command.
*
* \param[in]    clientDeviceId              Identifier of the peer
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_SendDataOverwritten
(
    deviceId_t  clientDeviceId
);

/*!**********************************************************************************
* \brief        Handles write operations on the Control point.
*
* \param[in]    deviceId      Identifier of the peer
* \param[in]    pEvent        Pointer to the GATT Server event
*
* \retval       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_ControlPointHandler
(
    deviceId_t                          deviceId,
    gattServerAttributeWrittenEvent_t*  pEvent
);

/*!**********************************************************************************
* \brief        Set the ATT MTU value to be used by RAS for Ranging Data notifications
*
* \param[in]    deviceId    Identifier of the peer
* \param[in]    attMtu      ATT MTU value
*
* \return       none
************************************************************************************/
void Ras_SetMtuValue
(
    deviceId_t deviceId,
    uint16_t   attMtu
);

/*!**********************************************************************************
* \brief        Set the RAS data pointer to the local measurement data
*
* \param[in]    deviceId    Identifier of the peer
* \param[in]    pData       Pointer to the local measurement data
*
* \return       none
************************************************************************************/
void Ras_SetDataPointer
(
    deviceId_t deviceId,
    rasMeasurementData_t* pData
);

/*!**********************************************************************************
* \brief        Check if data transfer is currently in progress for a given peer
*
* \param[in]    deviceId        Peer identifier
*
* \return       TRUE if transfer is ongoing, FALSE otherwise
************************************************************************************/
bool_t Ras_CheckTransferInProgress(deviceId_t deviceId);

/*!**********************************************************************************
* \brief        Set peer preference for transmitting the ranging data through
*               notifications or indications.
*
* \param[in]    deviceId     Peer identifier
* \param[in]    notifInds    On-Demand Data: BIT0 = 0 notifications BIT0 = 1 indications
*                            Data Ready: BIT1 = 0 notifications BIT2 = 1 indications
*                            Data Overwritten: BIT2 = 0 notifications BIT3 = 1 indications
*                            Real-Time Data: BIT3 = 0 notifications BIT4 = 1 indications
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_SetDataSendPreference
(
    deviceId_t deviceId,
    uint8_t notifInds
);

/*!**********************************************************************************
* \brief        Get peer preference for transmitting the ranging data through
*               notifications or indications.
*
* \param[in]    deviceId        Peer identifier
*
* \return       On-Demand Data behavior: BIT0 = 0 notifications BIT0 = 1 indications
*               Data Ready behavior: BIT1 = 0 notifications BIT1 = 1 indications
*               Data Overwritten behavior: BIT2 = 0 notifications BIT2 = 1 indications
*               Real-Time Data behavior: BIT3 = 0 notifications BIT3 = 1 indications
*
************************************************************************************/
uint8_t Ras_GetDataSendPreference
(
    deviceId_t deviceId
);

/*!**********************************************************************************
* \brief        Sends indications with the ranging data
*
* \param[in]    deviceId            Identifier of the peer
* \param[in]    hValueRasNotif      Handle of the Subevent Ranging Data Characteristic
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_SendRangingDataIndication
(
    deviceId_t  deviceId,
    uint16_t    hValueRasNotif
);

/*!**********************************************************************************
* \brief        Sends indications with the ranging data
*
* \param[in]    deviceId          Identifier of the peer
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_HandleGetRangingDataSegmInd
(
    deviceId_t deviceId
);

/*!**********************************************************************************
* \brief        Check if segment retransmission is currently in progress
*
* \param[in]    deviceId        Peer identifier
*
* \return       TRUE if transfer is ongoing, FALSE otherwise
************************************************************************************/
bool_t Ras_CheckSegmentTransmInProgress
(
    deviceId_t deviceId
);

/*!**********************************************************************************
* \brief        Checks if peer wants to receive real time data
*
* \param[in]    deviceId            Identifier of the peer
*
* \return       TRUE if peer has configured real time data, false otherwise
************************************************************************************/
bool_t Ras_CheckRealTimeData
(
    deviceId_t  deviceId
);

/*!**********************************************************************************
* \brief        Sends notifications with the ranging data body
*
* \param[in]    deviceId            Identifier of the peer
* \param[in]    hValueRasNotif      Handle of the Subevent Ranging Data Characteristic
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_SendRangingDataNotifs
(
    deviceId_t  deviceId,
    uint16_t    hValueRasNotif
);

/*!**********************************************************************************
* \brief        Build the ranging data body from local HCI data
*
* \param[in]    deviceId         Identifier of the peer
* \param[in]    packProcHdr      TRUE if the ranging data body should include the procedure
*                                header, FALSE otherwise
* \param[in]    packSubevtHdr    TRUE if the ranging data body should include the subevent
*                                header, FALSE otherwise
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_BuildRangingDataBody
(
  deviceId_t deviceId,
  bool_t     packProcHdr,
  bool_t     packSubevtHdr
);

/*!**********************************************************************************
* \brief        Clear data before starting a new transfer
*
* \param[in]    deviceId         Identifier of the peer
*
* \return       gBleSuccess_c or error
************************************************************************************/
void Ras_ClearDataForTransfer
(
    deviceId_t deviceId
);

/*!**********************************************************************************
* \brief        Check if the peer has subscribed to either real-time or on-demand
*               notifications/indications.
*
* \param[in]    deviceId         Identifier of the peer
*
* \return       TRUE if the peer has subscribed, FALSE otherwise
************************************************************************************/
bool_t Ras_CheckIfSubscribed
(
    deviceId_t deviceId
);

/*!**********************************************************************************
* \brief        Returns the flag indicating whether notifications are currently 
*               paused for the specified device.
*
* \param[in]    deviceId   The ID of the device.
*
* \return       FALSE if notifications are not paused, TRUE otherwise.
************************************************************************************/
bool_t Ras_GetNotificationsPause
(
    deviceId_t deviceId
);

/*!**********************************************************************************
* \brief         Sets the flag indicating whether notifications are paused for the
*                specified device due to HCI data slot occupation.
*
* \param[in]    deviceId   The ID of the device.
* \param[in]    value      FALSE to allow sending, TRUE to pause.
************************************************************************************/
void Ras_SetNotificationsPause
(
    deviceId_t  deviceId,
    bool_t      value
);

/*!**********************************************************************************
* \brief        Handles GATT Value Confirmation from the Client.
*
* \param[in]    deviceId      Identifier of the peer
*
* \retval       none
************************************************************************************/
void Ras_GattValueConfirmationHandler
(
    deviceId_t                          deviceId
);

/*!**********************************************************************************
* \brief        Clears the indication pending flag for a peer.
*
* \param[in]    deviceId      Identifier of the peer
*
* \retval       none
************************************************************************************/
void Ras_ClearIndicationPendingFlag
(
    deviceId_t deviceId
);

#ifdef __cplusplus
}
#endif

#endif /* #if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1) */
#endif /* RANGING_INTERFACE_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
