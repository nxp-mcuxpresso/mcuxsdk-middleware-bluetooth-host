/*! *********************************************************************************
 * \defgroup GATT_DB GATT_DB - GATT Database Interface and Definitions
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

#ifndef GATT_DATABASE_H
#define GATT_DATABASE_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ble_general.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
/*! Special value returned by GattDb_GetIndexOfHandle to signal that
    an invalid attribute handle was given as parameter.  */
#define gGattDbInvalidHandleIndex_d     (0xFFFFU)
/*! Special value used to mark an invalid attribute handle.
    Attribute handles are strictly positive. */
#define gGattDbInvalidHandle_d          (0x0000U)

#define    gPermissionNone_c                         0U       /*!< No permissions selected. */

/* Reading Permissions */
#define    gPermissionFlagReadable_c                 BIT0     /*!< Attribute can be read. */
/* if gPermissionFlagReadable_c == 1 */
#define    gPermissionFlagReadWithEncryption_c       BIT1     /*!< Attribute may be read only if link is encrypted. */
#define    gPermissionFlagReadWithAuthentication_c   BIT2     /*!< Attribute may be read only by authenticated peers. */
#define    gPermissionFlagReadWithAuthorization_c    BIT3     /*!< Attribute may be read only by authorized peers. */
/* endif */

/* Writing Permissions */
#define    gPermissionFlagWritable_c                 BIT4     /*!< Attribute can be written. */
/* if gPermissionFlagWritable_c == 1 */
#define    gPermissionFlagWriteWithEncryption_c      BIT5     /*!< Attribute may be written only if link is encrypted. */
#define    gPermissionFlagWriteWithAuthentication_c  BIT6     /*!< Attribute may be written only by authenticated peers. */
#define    gPermissionFlagWriteWithAuthorization_c   BIT7     /*!< Attribute may be written only by authorized peers. */
/* endif */

#define gGattDatabaseHashSize_c                      16U

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*! Attribute structure */
typedef struct {
    uint16_t        handle;                         /*!< The attribute handle - cannot be 0x0000. The attribute handles need not be consecutive, but must be strictly increasing. */
    uint16_t        permissions;                    /*!< Attribute permissions as defined by the ATT. */
    uint32_t        uuid;                           /*!< The UUID should be read according to the gattDbAttribute_t.uuidType member: for 2-byte and 4-byte UUIDs, this contains the value of the UUID;
                                                         for 16-byte UUIDs, this is a pointer to the allocated 16-byte array containing the UUID. */
    uint8_t*        pValue;                         /*!< A pointer to allocated value array. */
    uint16_t        valueLength;                    /*!< The size of the value array. */
    uint16_t        uuidType : 2;                   /*!< Identifies the length of the UUID; values interpreted according to the \ref bleUuidType_t enumeration. */
    uint16_t        maxVariableValueLength : 10;    /*!< The maximum length of the attribute value array; if this is set to 0, then the attribute's length is fixed and cannot be changed. */
} gattDbAttribute_t;

/*! Bit fields for Characteristic properties */
typedef uint8_t gattCharacteristicPropertiesBitFields_t;
typedef enum {
    gGattCharPropNone_c                   = 0U,        /*!< No Properties selected. */
    gGattCharPropBroadcast_c              = BIT0,     /*!< Characteristic can be broadcast. */
    gGattCharPropRead_c                   = BIT1,     /*!< Characteristic can be read. */
    gGattCharPropWriteWithoutRsp_c        = BIT2,     /*!< Characteristic can be written without response. */
    gGattCharPropWrite_c                  = BIT3,     /*!< Characteristic can be written with response. */
    gGattCharPropNotify_c                 = BIT4,     /*!< Characteristic can be notified. */
    gGattCharPropIndicate_c               = BIT5,     /*!< Characteristic can be indicated. */
    gGattCharPropAuthSignedWrites_c       = BIT6,     /*!< Characteristic can be written with signed data. */
    gGattCharPropExtendedProperties_c     = BIT7      /*!< Extended Characteristic properties. */
} gattCharacteristicPropertiesBitFields_tag;

/*! Bit fields for attribute permissions */
typedef uint8_t gattAttributePermissionsBitFields_t;

/*! Attribute access type */
typedef enum {
    gAccessRead_c    = 0x00U,      /*< Attempting to read the attribute. */
    gAccessWrite_c   = 0x01U,     /*< Attempting to write the attribute. */
    gAccessNotify_c  = 0x02U      /*< Attempting to notify the attribute. */
} gattDbAccessType_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
/*! The number of attributes in the GATT Database. */
extern uint16_t gGattDbAttributeCount_c;

/*! Reference to the GATT database */
extern gattDbAttribute_t* gattDatabase;

#if defined(gBLE51_d) && (gBLE51_d == 1U)
#if defined(gGattCaching_d) && (gGattCaching_d == 1U)
extern uint16_t mActiveServiceChangedCharHandle[];
extern uint16_t mActiveServiceChangedCCCDHandle[];
extern uint16_t gGattActiveClientSupportedFeaturesHandles[];
#endif /* gGattCaching_d */
#endif /* gBLE51_d */

extern uint16_t mServerServiceChangedCharHandle;
extern uint16_t mServerServiceChangedCCCDHandle;

extern uint32_t gGattDynamicAttrSize;
extern uint8_t gaGattDynamicAttrBlob[];

extern uint32_t gGattDynamicValSize;
extern uint8_t gaGattDynamicValBlob[];

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*! *********************************************************************************
* \brief     Returns the database index for a given attribute handle.
*
* \param[in] handle  The attribute handle.
*
* \return    The index of the given attribute in the database or gGattDbInvalidHandleIndex_d.
*
********************************************************************************** */
uint16_t GattDb_GetIndexOfHandle(uint16_t handle);

/*! *********************************************************************************
* \brief     Returns the handle of the service to which the given attribute belongs.
*
* \param[in] handle  The attribute handle.
*
* \return    The handle of the service or gGattDbInvalidHandleIndex_d.
*
********************************************************************************** */
uint16_t GattDb_ServiceStartHandle
(
    uint16_t handle
);

/*! *********************************************************************************
* \brief     Finds the start index and attribute count for a given service
*
* \param[in] serviceHandle       The service handle.
* \param[in] pOutStartIndex      The index in the database where the service declaration begins.
* \param[in] pOutAttributeCount  The number of attributes contained by the service.
*
* \retval   gBleSuccess_c
* \retval   gGattDbInvalidHandleIndex_d     Service does not exist in gatt database.
*
********************************************************************************** */
bleResult_t GattDb_FindServiceRange
(
    uint16_t serviceHandle,
    uint16_t* pOutStartIndex,
    uint16_t* pOutAttributeCount
);

/*! *********************************************************************************
* \brief     Returns the value length for a given attribute handle.
*
* \param[in] handle  The attribute handle.
*
* \return    The length of the value of the attribute at the given handle or
*            maxVariableValueLength in case of variable length attributes.
*
********************************************************************************** */
uint16_t GattDb_GetAttributeValueSize(uint16_t handle);

/*! *********************************************************************************
* \brief     Computes the database hash for a static or a dynamic database
*
* \param[in] void.
*
* \retval   gBleSuccess_c
* \retval   gBleInvalidState_c              Database not initialized or empty.
* \retval   gBleOutOfMemory_c               Could not allocate memory for the database
*                                           content used to compute the database hash.
* \retval   gGattDbInvalidHandleIndex_d     Service does not exist in gatt database.
*
********************************************************************************** */
bleResult_t GattDb_ComputeDatabaseHash(void);

#ifdef __cplusplus
}
#endif

#endif /* GATT_DATABASE_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
