/*! *********************************************************************************
 * \addtogroup GATT
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2014 Freescale Semiconductor, Inc.
* Copyright 2016-2019, 2021-2023, 2025 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef GATT_TYPES_H
#define GATT_TYPES_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "att_errors.h"
#include "att_types.h"
#include "att_params.h"
#include "gatt_database.h"


#define gCccdEmpty_c            0x0000U   /*!< Nothing is enabled. */
#define gCccdNotification_c     BIT0      /*!< Enables notifications. */
#define gCccdIndication_c       BIT1      /*!< Enabled indications. */

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
/*! GATT Attribute structure definition */
typedef struct {
    uint16_t        handle;             /*!< Attribute handle. */
    bleUuidType_t   uuidType;           /*!< Type of the UUID. */
    bleUuid_t       uuid;               /*!< The attribute's UUID. */
    uint16_t        valueLength;        /*!< Length of the attribute value array. */
    uint16_t        maxValueLength;     /*!< Maximum length of the attribute value array; if this is set to 0, then the attribute's length is fixed and cannot be changed. */
    uint8_t*        paValue;            /*!< Attribute value array. */
} gattAttribute_t;

/*! GATT Characteristic structure definition */
typedef struct {
    gattCharacteristicPropertiesBitFields_t     properties;         /*!< Characteristic Properties as defined by GATT. */
    gattAttribute_t                             value;              /*!< Characteristic Value attribute. */
    uint8_t                                     cNumDescriptors;    /*!< Size of the Characteristic Descriptors array. */
    gattAttribute_t*                            aDescriptors;       /*!< Characteristic Descriptors array. */
} gattCharacteristic_t;

/*! GATT Service structure definition */
typedef struct gattService_tag {
    uint16_t                startHandle;            /*!< The handle of the Service Declaration attribute. */
    uint16_t                endHandle;              /*!< The last handle belonging to this Service (followed by another Service declaration of the end of the database). */
    bleUuidType_t           uuidType;               /*!< Service UUID type. */
    bleUuid_t               uuid;                   /*!< Service UUID. */
    uint8_t                 cNumCharacteristics;    /*!< Size of the Characteristic array. */
    gattCharacteristic_t*   aCharacteristics;       /*!< Characteristic array. */
    uint8_t                 cNumIncludedServices;   /*!< Size of the Included Services array. */
    struct gattService_tag* aIncludedServices;      /*!< Included Services array. */
} gattService_t;

/*! Flags for the value of the Client Characteristic Configuration Descriptor. */
typedef uint8_t gattCccdFlags_t;

/*! Characteristic Presentation Format Descriptor structure */
typedef struct {
    uint8_t     format;         /*!< Format of the value of this characteristic. */
    int8_t      exponent;       /*!< Exponent field to determine how the value of this characteristic is further formatted. */
    uint16_t    unitUuid16;     /*!< The unit of this characteristic. */
    uint8_t     ns;             /*!< The name space of the description. */
    uint16_t    description;    /*!< The description of this characteristic. */
} gattDbCharPresFormat_t;

/*! GATT Handle Range structure definition */
typedef struct {
    uint16_t    startHandle;    /*!< Start Handle. */
    uint16_t    endHandle;      /*!< End Handle - shall be greater than or equal to Start Handle. */
} gattHandleRange_t;

/*! GATT Caching Client states */
typedef enum
{
    gGattClientChangeUnaware_c          = 0x00U, /*!< Gatt client is "change-unaware" regarding changes in the database definitions on the Server. */
    gGattClientStateChangePending_c     = 0x01U, /*!< Gatt client is in the process of going from "change-unaware" to "change-aware". */
    gGattClientChangeAware_c            = 0x02U, /*!< Gatt client is "change-aware" regarding the database definitions on the Server. */
} gattCachingClientState_c;

/*! Hash Update type */
typedef enum {
    gattClientFirstConnection_c         = 0x00U, /*!< Hash update for the first connection with a peer */
    gattClientReconnectBondedPeer_c     = 0x01U, /*!< Hash update after reconnecting with a bonded peer */
    gattClientActiveConnectionUpdate    = 0x02U, /*!< Hash update when a change occurs during an active connection */
    gattClientNoChange                  = 0x03U, /*!< Idle state - no change */
} gattClientHashUpdateType_t;

/*! GATT Client Procedure type */
typedef enum {
    gGattProcExchangeMtu_c                          = 0x00U, /*!< MTU Exchange */
    gGattProcDiscoverAllPrimaryServices_c           = 0x01U, /*!< Primary Service Discovery */
    gGattProcDiscoverPrimaryServicesByUuid_c        = 0x02U, /*!< Discovery of Services by UUID */
    gGattProcFindIncludedServices_c                 = 0x03U, /*!< Discovery of Included Services within a Service range */
    gGattProcDiscoverAllCharacteristics_c           = 0x04U, /*!< Characteristic Discovery within Service range */
    gGattProcDiscoverCharacteristicByUuid_c         = 0x05U, /*!< Characteristic Discovery by UUID */
    gGattProcDiscoverAllCharacteristicDescriptors_c = 0x06U, /*!< Characteristic Descriptor Discovery */
    gGattProcReadCharacteristicValue_c              = 0x07U, /*!< Characteristic Reading using Value handle */
    gGattProcReadUsingCharacteristicUuid_c          = 0x08U, /*!< Characteristic Reading by UUID */
    gGattProcReadMultipleCharacteristicValues_c     = 0x09U, /*!< Reading multiple Characteristics at once */
    gGattProcWriteCharacteristicValue_c             = 0x0AU, /*!< Characteristic Writing */
    gGattProcReadCharacteristicDescriptor_c         = 0x0BU, /*!< Reading Characteristic Descriptors */
    gGattProcWriteCharacteristicDescriptor_c        = 0x0CU, /*!< Writing Characteristic Descriptors */
    gGattProcUpdateDatabaseCopy_c                   = 0x0DU, /*!< Inform the application to update its database copy */
    gGattProcSignalServiceDiscoveryComplete_c       = 0x0EU, /*!< Inform the application that service discovery has finished */
    gGattProcReadMultipleVarLengthCharValues_c      = 0x0FU, /*!< Read Multiple Variable Length Characteristic Values */
} gattProcedureType_t;

/*! GATT Procedure phase */
typedef enum {
    gattProcPhaseInitiated,     /*!< GATT procedure has been initiated. */
    gattProcPhaseRunning        /*!< GATT procedure is running. */
} gattProcedurePhase_t;

/*! GATT Procedure Status structure definition */
typedef struct {
    bool_t                  isOngoing;              /*!< Indicates whether the GATT procedure type is ongoing. */
    gattProcedureType_t     ongoingProcedureType;   /*!< Procedure type. */
    gattProcedurePhase_t    ongoingProcedurePhase;  /*!< Procedure phase. */
} procStatus_t;

/*! GATT Procedure Data structure definition.
 * Structure members are ordered to minimize the padding size */
typedef struct {
    uint16_t index;                                             /*!< Number of entries currently stored. */
    uint16_t max;                                               /*!< Maximum number of entries to store. */
    bleUuid_t charUuid;                                         /*!< UUID of the characteristic to be discovered. */
    bleUuidType_t charUuidType;                                 /*!< UUID type of the characteristic to be discovered. */
    bool_t reliableLongWrite;                                   /*!< Indicates whether to do reliable long writes. */
    bool_t bAllocatedArray;                                     /*!< Indicates whether the array is allocated. */

    union {
        uint8_t* pCount8b;                                      /*!< Pointer to a uint8_t memory location where to store the final number of added entries. */
        uint16_t* pCount16b;                                    /*!< Pointer to a uint16_t memory location where to store the final number of added entries. */
    } pOutActualCount;                                          /*!< Pointer to the memory locatio where to store the final number of added entries. */

    union {
        gattService_t* aServices;                               /*!< Array of services. */
        gattCharacteristic_t* aChars;                           /*!< Array of characteristics. */
        gattAttribute_t* aDescriptors;                          /*!< Array of descriptors. */
        uint8_t* aBytes;                                        /*!< Array of bytes. */
        uint16_t* aHandles;                                     /*!< Array of handles. */
    } array;                                                    /*!< Array where data is stored. */

    union {
        attReadByGroupTypeRequestParams_t rbgtParams;           /*!< Read By Group Type Request Parameters. */
        attFindByTypeValueRequestParams_t fbtvParams;           /*!< Find By Type Value Request Parameters. */
        attReadByTypeRequestParams_t rbtParams;                 /*!< Read By Type Request Parameters. */
        attFindInformationRequestParams_t fiParams;             /*!< Find Information Request Parameters. */
        attReadRequestParams_t rParams;                         /*!< Read Request Parameters. */
        attReadBlobRequestParams_t rbParams;                    /*!< Read Blob Request Parameters. */
        attReadMultipleRequestParams_t rmParams;                /*!< Read Multiple Request Parameters. */
        attVarWriteRequestAndCommandParams_t wParams;           /*!< Write Request and Write Command Parameters for variable value length. */
        attSignedWriteCommandParams_t swParams;                 /*!< Signed Write Command parameters. */
        attPrepareWriteRequestResponseParams_t pwParams;        /*!< Prepare Write Request and Prepare Write Response Parameters. */
        attExecuteWriteRequestParams_t ewParams;                /*!< Execute Write Request Parameters */
    } reqParams;                                                /*!< Parameters of the GATT procedure */
} procDataStruct_t;

#endif /* GATT_TYPES_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
