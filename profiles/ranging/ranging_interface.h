/*! *********************************************************************************
* \defgroup Ranging Service
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022-2024 NXP
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
#define gNoFilter_c                     0xFFFFU

/* Antenna Configuratin Index 2:2 */
#define gAntennaCfgIdx7_c               7U

/* The maximum number of antenna pairs used. */
#define ISP_MAX_NO_ANTENNAS             4U

/* Maximum number of subevents to be buffered */
#define gRasMaxNumSubevents_c           10U /* 32 */

/* Maximum number of CS steps to be buffered */
#define gRasMaxNumSteps_c               160U

/* Raw buffer size: able to hold hciLeHadmEventResultEvent_t message header
 * + ToF and IQ data for the max number of steps */
#define gHadmDataSizeMax_c              (4 /* HCI data header */ + \
                                        (6 + 1 + 4 * (1 + ISP_MAX_NO_ANTENNAS)))

/* Maximum data size for a CS subevent */
#define gRasCsSubeventDataSize_c        2300U

/* Size of the header for RAS notifications */
#define gRasNotificationHeaderize       sizeof(uint16_t) + sizeof(uint8_t) + \
                                        sizeof(rasSubeventHeader_t)

/* Number of records to store in the Stored Ranging Data Characteristic */
#define gRasNumberOfSupportedRecords_c  1U

#define gRasSegmentCounterMinValue_c    0U

#define gRasSegmentCounterMaxValue_c    63U

#define gRASMaxNoOfSegments_c           12U

/* Size of Tone_PCT field for mode 2 data */
#define gTone_PCTSize_c                 3U
                                          
/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*! Subevent indexing for local data */
typedef PACKED_STRUCT measSubEvtData_tag
{
    uint32_t dataIdx;           /* Data index for the subevent. */
    uint16_t dataSize;          /* Data size for the subevent. */
    uint8_t  numSteps;          /* Number of steps in the subevent */
    uint16_t currentDataSize;   /* Data size already sent/received for the current subevent. */
}measSubEvtData_t;

/*! Format of local data */
typedef PACKED_STRUCT rasMeasurementData_tag
{
    deviceId_t          deviceId;
    uint8_t             configId;
    uint16_t            startACLConnEvent;
    uint16_t            procedureCounter;
    uint16_t            frequencyCompensation;
    int8_t              referencePowerLevel;
    int8_t              selectedTxPower;
    uint8_t             procedureDoneStatus;
    uint8_t             subeventDoneStatus;
    uint8_t             abortReason;
    uint8_t             numAntennaPaths;
    uint8_t             numStepsReported;
    uint32_t            dataIndex;
    uint16_t            totalSentRcvDataIndex;
    uint16_t            dataParsedLen;
    uint8_t             subeventIndex;
    uint8_t             numberOfSubevents;
    uint8_t             crtNumSteps;
    measSubEvtData_t    aSubEventData[gRasMaxNumSubevents_c];
    uint8_t             *pData;
} rasMeasurementData_t;

typedef struct segmentData_tag
{
    uint8_t     segmentIdx;
    uint16_t    dataIdxStart;
    uint16_t    dataSize;
} segmentData_t;

/*! Ranging Service - Configuration */
typedef PACKED_STRUCT rasConfig_tag
{
    uint16_t              serviceHandle;
    uint16_t              controlPointHandle;
    uint16_t              storedDataHandle;
    uint16_t              realTimeDataHandle;
    uint16_t              dataReadyHandle;
    uint16_t              dataOverwrittenHandle;
    uint16_t              featuresHandle;
    rasMeasurementData_t* pData;
    uint8_t*              pRangingDataBody; 
} rasConfig_t;

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

typedef enum
{
    gRasNotifFirstSegment_c    = BIT0, /*!< First segment of data */
    gRasNotifLastSegment_c     = BIT1, /*!< Last segment of data */
} rasNotifSegmentType_tag;

/*!< RAS notification payload - procedure header */
typedef struct rasProcedureHeader_tag
{
    uint16_t     procCounterConfigId; /* 12 bits for procCounter, 4 for configId */
    uint8_t      selectedTxPower; /* Units: dbm */
    uint8_t      antennaPathsMaskPctFormat; /* 6 bits for antennaPathsMask, 2 for pctFormat */
} rasProcedureHeader_t;

/*!< RAS notification payload - subevent header and data */
typedef PACKED_STRUCT rasSubeventDataHeader_tag
{
    uint16_t        startACLConnEvent;
    uint16_t        frequencyCompensation;
    uint8_t         procAndSubeventDoneStatus; /* 4 bits for procedure done status
                                                  4 bits for subevent done status */
    uint8_t         procAndSubeventAbortReason; /* 4 bits for abort reason when procedure done status is 0x0F, otherwise 0
                                                   4 bits for abort reason when subevent done status is 0x0F, otherwise 0 */
    int8_t          referencePowerLevel;
    uint8_t         numStepsReported;
} rasSubeventDataHeader_t;

/*!< RAS Control Point request operation codes */
typedef uint8_t rasCPCommandsOpCode_t;

typedef enum
{
    getProcDataOpCode_c            = 0x00U,
    ackProcDataOpCode_c            = 0x01U,
    retrieveLostProcDataOpCode_c   = 0x02U,
    abortOperationOpCode_c         = 0x03U,
    filterOpCode_c                 = 0x04U,
    PCTFilter_c                    = 0x05U,
} rasControlPointOperations_tag;

/*!< RAS Control Operator Values */
typedef uint8_t rasCPOperatorCode_t;

typedef enum
{
    rasCPOperatorNull_c                 = 0x00U,
    rasCPOperatorIncludeExclude_c       = 0x01U,
    rasCPOperatorOneRecord_c            = 0x02U,
    rasCPOperatorGreaterThanEqualTo_c   = 0x03U,
} rasControlPointOperators_tag;

/*!< RAS Retrieve Lost Data CP command operands */
typedef struct rasRetrieveLostDataCmdParameters_tag {
    uint16_t   procCounter;
    uint8_t    startAbsSegment;
    uint8_t    endAbsSegment;
} rasRetrieveLostDataCmdParameters_t;

/*! RAS Control Point Command payload */
typedef PACKED_STRUCT rasControlPoint_tag
{
    rasCPCommandsOpCode_t             cmdOpCode;
    union {
        uint16_t                              procCounter;
        rasRetrieveLostDataCmdParameters_t    retrieveLostDataParams;
        uint16_t                              filterValue;
        uint8_t                               pctFormat;
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
typedef uint16_t rasControlPointRspCodeValues_t;

typedef enum
{
    gRasErrorRfu_c                           = 0x00U, /*!< Reserved for future use */  
    gRasSuccess_c                            = 0x01U, /*!< Procedure executed successfully */
    gRasOpCodeNotSupportedError_c            = 0x02U, /*!< OpCode that was written to the RAS-CP is not supported by the RRSP */
    gRasInvalidParameterError_c              = 0x03U, /*!< Parameter that was written to the RAS-CP is invalid */
    gRasParameterNotSupportedError_c         = 0x04U, /*!< Parameter that was written to the RAS-CP is not supported by the RRSP */
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
    rasControlPointRspOpCode_t      rspOpCode;
    PACKED_UNION
    {
        uint16_t           procCounter;
        rasCtrPointRsp_t   rspPayload;
    } cmdParameters;
} rasControlPointRsp_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
extern const uint8_t hadm_ant_perm_n_ap[24][4];

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
bleResult_t Ras_Start(rasConfig_t *pServiceConfig);

/*!**********************************************************************************
* \brief        Stops Ranging Service functionality
*
* \param[in]    pServiceConfig  Pointer to structure that contains server
*                               configuration information.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_Stop(rasConfig_t *pServiceConfig);

/*!**********************************************************************************
* \brief        Subscribes a GATT client to the Ranging service
*
* \param[in]    clientDeviceId   Client Id in Device DB.
* \param[in]    realTime         TRUE if peer has enabled real-time data transfer, FALSE for on-demand.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_Subscribe
(
    deviceId_t clientDeviceId,
    bool_t realTime
);

/*!**********************************************************************************
* \brief        Unsubscribes a GATT client from the Ranging service
*
* \param[in]    clientdeviceId  Identifier of the peer to be unsubscribed
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_Unsubscribe(deviceId_t clientdeviceId);

/*!**********************************************************************************
* \brief        Send a Data Ready Indication to the peer after all procedure
*               data has been received from the local controller.
*
* \param[in]    pServiceConfig              Pointer to structure that contains server
*                                           configuration information.
* \param[in]    clientdeviceId              Identifier of the peer
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_SendDataReadyIndication
(
    rasConfig_t *pServiceConfig,
    deviceId_t   clientdeviceId
);

/*!**********************************************************************************
* \brief        Send a Data Overwritten Indication to the peer if new procedure data.
*               was received before the peer send the Delete Data command.
*
* \param[in]    pServiceConfig              Pointer to structure that contains server
*                                           configuration information.
* \param[in]    clientdeviceId              Identifier of the peer
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Ras_SendDataOverwrittenIndication
(
    rasConfig_t *pServiceConfig,
    deviceId_t   clientdeviceId
);

/*!**********************************************************************************
* \brief        Handles write operations on the Control point.
*
* \param[in]    deviceId        Identifier of the peer
* \param[in]    pServiceConfig  Pointer to structure that contains server
*                               configuration information.
* \param[in]    eventType       GATT event type - could be
*                               gEvtAttributeWrittenWithoutResponse_c or
*                               gEvtAttributeWritten_c.
* \param[in]    pEvent          Pointer to the GATT Server event.
*
* \retval       none.
************************************************************************************/
bleResult_t Ras_ControlPointHandler
(
    deviceId_t                          deviceId,
    rasConfig_t*                        pServiceConfig,
    gattServerEventType_t               eventType,
    gattServerAttributeWrittenEvent_t*  pEvent
);

/*!**********************************************************************************
* \brief        Set the ATT MTU value to be used by RAS for Ranging Data notifications
*
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
* \brief        Set the pointer to the local measurement data
*
* \param[in]    none
*
* \return       none
************************************************************************************/
void Ras_SetDataPointer(rasMeasurementData_t* pData);

/*!**********************************************************************************
* \brief        Check if data transfer is currently in progress
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
* \param[in]    deviceId        Peer identifier
* \param[in]    notifInds       On-Demand Data behavior: BIT0 = 0 notifications BIT0 = 1 indications
*                               Control Point behavior: BIT1 = 0 notifications BIT1 = 1 indications
*                               Data Ready behavior: BIT2 = 0 notifications BIT2 = 1 indications
*                               Data Overwritten behavior: BIT3 = 0 notifications BIT3 = 1 indications
*                               Real-Time Data behavior: BIT4 = 0 notifications BIT4 = 1 indications
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_SetDataSendPreference(deviceId_t deviceId, uint8_t notifInds);

/*!**********************************************************************************
* \brief        Get peer preference for transmitting the ranging data through
*               notifications or indications.
*
* \param[in]    deviceId        Peer identifier
*
* \return       On-Demand Data behavior: BIT0 = 0 notifications BIT0 = 1 indications
*               Control Point behavior: BIT1 = 0 notifications BIT1 = 1 indications
*               Data Ready behavior: BIT2 = 0 notifications BIT2 = 1 indications
*               Data Overwritten behavior: BIT3 = 0 notifications BIT3 = 1 indications
*               Real-Time Data behavior: BIT4 = 0 notifications BIT4 = 1 indications
*
************************************************************************************/
uint8_t Ras_GetDataSendPreference(deviceId_t deviceId);

/*!**********************************************************************************
* \brief        Sends indications with the stored procedure data
*
* \param[in]    deviceId            Identifier of the peer
* \param[in]    hValueRasNotif      Handle of the Subevent Ranging Data Characteristic
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_SendStoredRangingDataIndication
(
    deviceId_t  deviceId,
    uint16_t    hValueRasNotif
);

/*!**********************************************************************************
* \brief        Sends indications with the stored procedure data
*
* \param[in]    deviceId          Identifier of the peer
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_HandleGetRecordSegmentsIndications
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
bool_t Ras_CheckSegmentTransmInProgress(deviceId_t deviceId);

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
* \brief        Sends notifications with the stored procedure data
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
* \param[in]    firstSubevt      TRUE if the ranging data body should include the procedure
*                                header, FALSE otherwise
*
* \return       gBleSuccess_c or error
************************************************************************************/
bleResult_t Ras_BuildRangingDataBody
(
  deviceId_t deviceId,
  bool_t     firstSubevt
);

#ifdef __cplusplus
}
#endif

#endif /* RANGING_INTERFACE_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
