/*! *********************************************************************************
 * \addtogroup GATT_DB
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2014 Freescale Semiconductor, Inc.
* Copyright 2016-2019, 2021-2023 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Default local configuration
*************************************************************************************
************************************************************************************/
#ifndef gGattDbDynamic_d
#define gGattDbDynamic_d    0
#endif /* gGattDbDynamic_d */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "gatt_database.h"
#include "gatt_db_app_interface.h"
#include "gatt_types.h"
#include "gap_types.h"
#include "board.h"
#include "ble_utils.h"

#include "fsl_component_mem_manager.h"
#include "SecLib.h"

#if gGattDbDynamic_d
#include "gatt_db_dynamic.h"
#else /* gGattDbDynamic_d */
/*! Macros and X-Macros */
#include "gatt_db_macros.h"
#include "gatt_db_x_macros.h"
#include "gatt_db_handles.h"
#endif /* gGattDbDynamic_d */

/************************************************************************************
*************************************************************************************
* X-Macro expansions - enums, structs and memory allocations
*************************************************************************************
************************************************************************************/
#if !gGattDbDynamic_d

/*! Allocate custom 128-bit UUIDs, if any */
#include "gatt_uuid_def_x.h"

/*! Allocate the arrays for Attribute Values */
#include "gatt_alloc_x.h"

/*! Declare the Attribute database */
static gattDbAttribute_t static_gattDatabase[] = {
#include "gatt_decl_x.h"
};

gattDbAttribute_t* gattDatabase = static_gattDatabase;

/*! Declare structure to compute the database size */
typedef struct sizeCounterStruct_tag {
#include "gatt_size_x.h"
} sizeCounterStruct_t;

/*! Compute the database size at compile time */
#define localGattDbAttributeCount_d  ((sizeof(sizeCounterStruct_t))/4U)
uint16_t gGattDbAttributeCount_c;

#else /* gGattDbDynamic_d */
gattDbAttribute_t*  gattDatabase;
uint16_t            gGattDbAttributeCount_c;
#endif /* gGattDbDynamic_d */

#if defined(gBLE51_d) && (gBLE51_d == 1U)
/* Database hash value - the client needs a hash value for each possible peer
*  The server needs only one value, as the database is the same for all client
*/

extern uint32_t mServerDatabaseHashIndex;
extern uint32_t mServerClientSupportedFeatureIndex;
#endif /* gBLE51_d */
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn            bleResult_t GattDb_Init(void)
*\brief         Function performing runtime initialization of the GATT database.
*
*\param  [in]   none.
*
*\retval   gBleSuccess_c
*\retval   gBleOutOfMemory_c            Could not allocate memory for the GATT
*                                       dynamic database.
*\retval   gBleAlreadyInitialized_c     GATT database already initialized.
*
*\remarks       This function should be called only once at device startup.
********************************************************************************** */
bleResult_t GattDb_Init(void)
{
#if !gGattDbDynamic_d
    static bool_t mAlreadyInit = FALSE;
    if (mAlreadyInit)
    {
        return gBleAlreadyInitialized_c;
    }
    mAlreadyInit = TRUE;

    /*! Assign the database size to the global */
    gGattDbAttributeCount_c = localGattDbAttributeCount_d;

    /*! Attribute-specific initialization by X-Macro expansion */
#include "gatt_init_x.h"

    return gBleSuccess_c;
#else /* gGattDbDynamic_d */
    return GattDbDynamic_Init();
#endif /* gGattDbDynamic_d */
}

/*! *********************************************************************************
*\fn            bleResult_t GattDb_Deinit(void)
*\brief         Function performing runtime deinitialization of the GATT database.
*
*\param  [in]   none.
*
*\retval        gBleSuccess_c
*\retval        gBleInvalidState_c      GATT dynamic database not initialized.
*
*\remarks       This function has effect for dynamic data base only .
********************************************************************************** */
bleResult_t GattDb_Deinit(void)
{
    bleResult_t result = gBleSuccess_c;
    
#if gGattDbDynamic_d
    result = GattDbDynamic_ReleaseDatabase();
#endif /* gGattDbDynamic_d */
    return result;
}
/*! *********************************************************************************
*\fn            uint16_t GattDb_GetIndexOfHandle(uint16_t handle)
*\brief         Database searching function, return the index for a given attribute
*               handle.
*
*\param  [in]   handle      The attribute handle.
*
*\return        uint16_t    The index of the given attribute in the database or
*                           gGattDbInvalidHandleIndex_d.
********************************************************************************** */
uint16_t GattDb_GetIndexOfHandle(uint16_t handle)
{
    uint16_t init = (handle >= gGattDbAttributeCount_c) ?
                    (gGattDbAttributeCount_c - 1U) : handle;
    for (uint16_t j = init; j != 0xFFFFU && gattDatabase[j].handle >= handle; j--)
    {
        if (gattDatabase[j].handle == handle)
        {
            return j;
        }
    }
    return gGattDbInvalidHandleIndex_d;
}

/*! *********************************************************************************
* \brief     Returns the value length for a given attribute handle.
*
* \param[in] handle  The attribute handle.
*
* \return    The length of the value of the attribute at the given handle or
*            maxVariableValueLength in case of variable length attributes.
*
********************************************************************************** */
uint16_t GattDb_GetAttributeValueSize(uint16_t handle)
{
    uint16_t index = GattDb_GetIndexOfHandle(handle);
    uint16_t len = 0U;
    
    if (index != gGattDbInvalidHandleIndex_d)
    {
        if (0U != gattDatabase[index].maxVariableValueLength)
        {
            /* Variable-size value */
            len = gattDatabase[index].maxVariableValueLength;
        }
        else
        {
            /* Fixed-size value */
            len = gattDatabase[index].valueLength;
        }
    }
        
    return len;
}

/*! *********************************************************************************
* \brief    Finds the range of a service for a given attribute handle.
*
* \param[in]  handle                The attribute handle.
* \param[out] pOutStartIndex        Index at which the service starts
* \param[out] pOutAttributeCount    Number of attributes in the service
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
)
{
    bleResult_t result = gGattDbInvalidHandle_c;

    for (uint32_t j = 0U; j < gGattDbAttributeCount_c; j++)
    {
        if (gattDatabase[j].handle == serviceHandle)
        {
            if ((gattDatabase[j].uuidType == (uint16_t)gBleUuidType16_c) &&
                BleSig_IsServiceDeclarationUuid16(gattDatabase[j].uuid))
            {
                *pOutStartIndex = (uint16_t)j;
                uint32_t k;
                for (k = j + 1U; k < gGattDbAttributeCount_c; k++)
                {
                    if ((gattDatabase[k].uuidType == (uint16_t)gBleUuidType16_c) &&
                        BleSig_IsServiceDeclarationUuid16(gattDatabase[k].uuid))
                    {
                        break;
                    }
                }
                *pOutAttributeCount = (uint16_t)(k - j);
                result = gBleSuccess_c;
                break;
            }
        }
    }
    return result;
}

/*! *********************************************************************************
* \brief    Finds the handle of the service to which the given attribute belongs to
*
* \param[in]  handle                The attribute handle.
*
* \return       The handle of the service or gGattDbInvalidHandleIndex_d.
*
********************************************************************************** */
uint16_t GattDb_ServiceStartHandle(uint16_t handle)
{
    uint16_t result = gGattDbInvalidHandleIndex_d;
    uint16_t index = GattDb_GetIndexOfHandle(handle);

    if (index != gGattDbInvalidHandleIndex_d)
    {
        for (int32_t j = (int32_t)index; j >= 0; j--)
        {
            if (gattDatabase[j].uuidType == (uint16_t)gBleUuidType16_c)
            {
                if (BleSig_IsServiceDeclarationUuid16(gattDatabase[j].uuid))
                {
                    result = gattDatabase[j].handle;
                    break;
                }
            }
        }
    }

    return result;
}

/*! *********************************************************************************
* \brief     Finds the handle for the service to which the given attribute handle belongs.
*
* \param[in] handle                The attribute handle.
*
* \retval   gBleSuccess_c
* \retval   gBleInvalidState_c              Database not initialized or empty.
* \retval   gBleOutOfMemory_c               Could not allocate memory for the database
*                                           content used to compute the database hash.
* \retval   gGattDbInvalidHandleIndex_d     Service does not exist in gatt database.
*
********************************************************************************** */
bleResult_t GattDb_ComputeDatabaseHash(void)
{
    bleResult_t result = gBleInvalidState_c;
#if defined(gBLE51_d) && (gBLE51_d == 1U)
#if defined(gGattCaching_d) && (gGattCaching_d == 1U) && defined(gGattDbComputeHash_d) && (gGattDbComputeHash_d == 1U)
    uint8_t *pDatabaseContent;
    uint32_t databaseLength = 0U;
    uint8_t aKey[gGattDatabaseHashSize_c] = {0U};

    /* the database is initialized or is not empty */
    if ((gattDatabase != NULL) && (gGattDbAttributeCount_c > 0U))
    {
        for (uint32_t index = 0U; index < gGattDbAttributeCount_c; index ++)
        {
            if (gattDatabase[index].uuidType == (uint16_t)gBleUuidType16_c)
            {
                switch(gattDatabase[index].uuid)
                {
                    case gBleSig_CharUserDescription_d:
                    case gBleSig_CCCD_d:
                    case gBleSig_SCCD_d:
                    case gBleSig_CharPresFormatDescriptor_d:
                    case gBleSig_CharAggregateFormat_d:
                        {
                              /* attribute handle */
                              databaseLength += sizeof(uint16_t);

                              /* attribute uuid */
                              databaseLength += sizeof(uint16_t);

                        }
                        break;

                    case gBleSig_PrimaryService_d:
                    case gBleSig_SecondaryService_d:
                    case gBleSig_Include_d:
                    case gBleSig_Characteristic_d:
                    case gBleSig_CharExtendedProperties_d:
                        {
                              /* attribute handle */
                              databaseLength += sizeof(uint16_t);

                              /* attribute uuid */
                              databaseLength += sizeof(uint16_t);

                              /* attribute value */
                              databaseLength += gattDatabase[index].valueLength;
                        }
                        break;

                    case gBleSig_GattClientSupportedFeatures_d:
                        {
                            mServerClientSupportedFeatureIndex = index;
                        }
                        break;

                    default:
                        {
                            ; /* For MISRA rule 16.4 */
                        }
                        break;
                }
            }
        }

        pDatabaseContent = MEM_BufferAlloc(databaseLength);
        if (pDatabaseContent == NULL)
        {
            result = gBleOutOfMemory_c;
        }
        else
        {
            databaseLength = 0U;
            for (uint32_t index = 0U; index < gGattDbAttributeCount_c; index ++)
            {
                if (gattDatabase[index].uuidType == (uint16_t)gBleUuidType16_c)
                {
                    /* contents are kept in little endian order in the database */
                    switch(gattDatabase[index].uuid)
                    {
                        case gBleSig_PrimaryService_d:
                        case gBleSig_SecondaryService_d:
                        case gBleSig_Include_d:
                        case gBleSig_Characteristic_d:
                        case gBleSig_CharExtendedProperties_d:
                            {
                                /* attribute handle */
                                Utils_PackTwoByteValue(gattDatabase[index].handle, &pDatabaseContent[databaseLength]);
                                databaseLength += sizeof(uint16_t);

                                /* attribute uuid */
                                Utils_PackTwoByteValue(gattDatabase[index].uuid, &pDatabaseContent[databaseLength]);
                                databaseLength += sizeof(uint16_t);

                                /* attribute value */
                                FLib_MemCpy(&pDatabaseContent[databaseLength], gattDatabase[index].pValue, gattDatabase[index].valueLength);
                                databaseLength += gattDatabase[index].valueLength;
                            }
                            break;

                        case gBleSig_CharUserDescription_d:
                        case gBleSig_CCCD_d:
                        case gBleSig_SCCD_d:
                        case gBleSig_CharPresFormatDescriptor_d:
                        case gBleSig_CharAggregateFormat_d:
                            {
                                /* attribute handle */
                                Utils_PackTwoByteValue(gattDatabase[index].handle, &pDatabaseContent[databaseLength]);
                                databaseLength += sizeof(uint16_t);

                                /* attribute uuid */
                                Utils_PackTwoByteValue(gattDatabase[index].uuid, &pDatabaseContent[databaseLength]);
                                databaseLength += sizeof(uint16_t);
                            }
                            break;

                        case gBleSig_GattDatabaseHash_d:
                            {
                                mServerDatabaseHashIndex = index;
                            }
                            break;

                        default:
                            {
                                ; /* For MISRA rule 16.4 */
                            }
                            break;
                    }
                }
            }

            /* update the the characteristic value */
            if (mServerDatabaseHashIndex != gGattDbInvalidHandleIndex_d)
            {
                /* do AES_CMAC do obtain the database hash and update the value in the database */
                AES_128_CMAC((uint8_t*)pDatabaseContent, databaseLength, aKey, gattDatabase[mServerDatabaseHashIndex].pValue);
                Utils_RevertByteArray(gattDatabase[mServerDatabaseHashIndex].pValue, gGattDatabaseHashSize_c);
            }

            (void)MEM_BufferFree(pDatabaseContent);
            result = gBleSuccess_c;
        }
    }
#endif /* gGattCaching_d && gGattDbComputeHash_d */
#endif /* gBLE51_d */
    return result;
}


/*! *********************************************************************************
* @}
********************************************************************************** */
