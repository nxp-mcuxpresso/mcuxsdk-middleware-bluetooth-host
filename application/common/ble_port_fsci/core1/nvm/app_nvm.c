/*! *********************************************************************************
* Copyright 2024-2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "fsl_component_mem_manager.h"
#include "fsl_os_abstraction.h"
#include "fsl_component_messaging.h"
#include "fwk_hal_macros.h"
#include "FsciInterface.h"
#include "fsci_ble_types.h"
#include "fsci_ble.h"
#include "ble_config.h"
#include "ble_general.h"
#include "ble_port_fsci_op.h"
#include "app_nvm.h"
#include "ble_port_fsci.h"
#include "app_conn.h"

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
/* NVM NCP Dataset identifiers */
#define nvmId_BondingHeaderBit_c         BIT0
#define nvmId_BondingDataDynamicBit_c    BIT1
#define nvmId_BondingDataStaticBit_c     BIT2
#define nvmId_BondingDataLegacyBit_c     BIT3
#define nvmId_BondingDataDeviceInfoBit_c BIT4
#define nvmId_BondingDataDescriptorBit_c BIT5

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef struct
{
    bleBondIdentityHeaderBlob_t     bondHeader;
    bleBondDataDynamicBlob_t        bondDataBlobDynamic;
    bleBondDataStaticBlob_t         bondDataBlobStatic;
    bleBondDataLegacyBlob_t         bondDataBlobLegacy;
    bleBondDataDescriptorBlob_t     bondDataDescriptors[gcGapMaximumSavedCccds_c];
    bleBondDataDeviceInfoBlob_t     bondDataBlobDeviceInfo;
    uint8_t                         bondDataSetValidBitmask;
    uint8_t                         bondDataSetValidHostBitmask;
    uint32_t                        bondDescriptorSetValidBitmask;
    uint32_t                        bondDescriptorSetValidHostBitmask;
}bleNcpBondDataBlob_t;

typedef struct appNvmHostReadNotif_tag
{
    bleResult_t result;
    uint8_t     dataSetBitmask;
    uint32_t    descriptorBitmask;
    uint8_t     aNvmData[1];
} appNvmHostReadNotif_t;

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
#if (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U))
static bleNcpBondDataBlob_t          maBondDataBlobs[gMaxBondedDevices_c] = {{{{0}}}};
#define gNcpBondDataEntrySize_c ((gBleBondIdentityHeaderSize_c + 3U) / sizeof(uint32_t) +\
                                (gBleBondDataDynamicSize_c + 3U) / sizeof(uint32_t) +\
                                (gBleBondDataStaticSize_c + 3U) / sizeof(uint32_t) +\
                                (gBleBondDataLegacySize_c + 3U) / sizeof(uint32_t) +\
                                (gBleBondDataDeviceInfoSize_c + 3U) / sizeof(uint32_t) +\
                                (gBleBondDataDescriptorSize_c + 3U) / sizeof(uint32_t) * gcGapMaximumSavedCccds_c)

static messaging_t mAppNvmInputQueue;
static bool_t gAppNvmOperationPending;
#else
static bleBondDataBlob_t          maBondDataBlobs[gMaxBondedDevices_c] = {{{{0}}}};
#endif /* (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U)) */

static uint8_t mBlockTaskNvmReadIdx = 0U;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
#if (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U))
/* This function erases the data corresponding to an entry on the host */
static bleResult_t App_HostNvmErase
(
    uint8_t mEntryIdx
);

/* This function writes the data to the corresponding NVM entry on the host */
static bleResult_t App_HostNvmWrite
(
    uint8_t  mEntryIdx,
    void*    pBondHeader,
    void*    pBondDataDynamic,
    void*    pBondDataStatic,
    void*    pBondDataLegacy,
    void*    pBondDataDeviceInfo,
    void*    pBondDataDescriptor,
    uint32_t descriptorBitmask
);

/* This function handles a NVM read indication from the host */
static void nvmCmdReadHandler
(
    uint8_t opc,
    uint8_t len,
    void *pData
);

/* This function handles a NVM write indication from the host */
static void nvmCmdWriteHandler
(
    uint8_t opc,
    uint8_t len,
    void *pData
);

/* This function handles a NVM erase indication from the host */
static void nvmCmdEraseHandler
(
    uint8_t opc,
    uint8_t len,
    void *pData
);

/* Blocking function used to read data from NVM on the host processor.*/
static bleResult_t App_HostNvmRead
(
    uint8_t  mEntryIdx,
    void*    pBondHeader,
    void*    pBondDataDynamic,
    void*    pBondDataStatic,
    void*    pBondDataLegacy,
    void*    pBondDataDeviceInfo,
    void*    pBondDataDescriptor,
    uint32_t descriptorBitmask,
    uint8_t* pDataSetBitmask,
    uint32_t *pBondDescriptorBitmask
);

#endif /* (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U)) */
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn           bleResult_t App_NvmErase(uint8_t mEntryIdx)
*\brief        This function erases the data corresponding to an entry.
*
*\param  [in]  mEntryIdx  Index of the entry that should be deleted.
*
*\retval    gBleInvalidParameter_c  Invalid NVM entry Id
*\retval    gBleUnexpectedError_c   Error occured while sending the request to the host
*\retval    gBleSuccess_c           Operation successfull
********************************************************************************** */
bleResult_t App_NvmErase
(
    uint8_t mEntryIdx
)
{
    bleResult_t status = gBleSuccess_c;

    if (mEntryIdx >= (uint8_t)gMaxBondedDevices_c)
    {
          status = gBleInvalidParameter_c;
    }
    else
    {
#if (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U))
        /* Clear data sets for this entry */
        FLib_MemSet(&maBondDataBlobs[mEntryIdx], 0, gNcpBondDataEntrySize_c);
        /* Mark the entry as empty */
        maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask = 0U;
        maBondDataBlobs[mEntryIdx].bondDescriptorSetValidBitmask = 0U;
        gAppNvmOperationPending = TRUE;
#else
        FLib_MemSet(&maBondDataBlobs[mEntryIdx], 0, sizeof(bleBondDataBlob_t));
#endif /* (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U)) */
    }

    return status;
}

/*! *********************************************************************************
*\fn           bleResult_t App_NvmWrite(uint8_t  mEntryIdx,
*                                void*    pBondHeader,
*                                void*    pBondDataDynamic,
*                                void*    pBondDataStatic,
*                                void*    pBondDataLegacy,
*                                void*    pBondDataDeviceInfo,
*                                void*    pBondDataDescriptor,
*                                uint8_t  mDescriptorIndex)
*\brief        Write data to NVM.
*
*\param  [in]  mEntryIdx              NVM entry index.
*\param  [in]  pBondHeader            Pointer to the bonding header. Can be NULL.
*\param  [in]  pBondDataDynamic       Pointer to the dynamic bonding data structure.
*                                     Can be NULL.
*\param  [in]   pBondDataStatic       Pointer to the static bonding data structure.
*                                     Can be NULL.
*\param  [in]   pBondDataLegacy       Pointer to the legacy bonding data structure.
*                                     Can be NULL.
*\param  [in]  pBondDataDeviceInfo    Pointer to the bonding data device info
*                                     structure. Can be NULL.
*\param  [in]  pBondDataDescriptor    Pointer to the bonding data descriptor.
*                                     Can be NULL.
*\param  [in]  mDescriptorIndex       Bonding data descriptor index.
*
*\retval    gBleInvalidParameter_c  Invalid NVM entry Id
*\retval    gBleUnexpectedError_c   Error occured while sending the request to the host
*\retval    gBleSuccess_c           Operation successfull
********************************************************************************** */
bleResult_t App_NvmWrite
(
    uint8_t  mEntryIdx,
    void*    pBondHeader,
    void*    pBondDataDynamic,
    void*    pBondDataStatic,
    void*    pBondDataLegacy,
    void*    pBondDataDeviceInfo,
    void*    pBondDataDescriptor,
    uint8_t  mDescriptorIndex
)
{
    bleResult_t status = gBleSuccess_c;
    union {
      uint8_t u8;
      uint32_t u32;
    } descrIdx = {};

    if (mEntryIdx >= (uint8_t)gMaxBondedDevices_c)
    {
          status = gBleInvalidParameter_c;
    }
    else
    {
#if (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U))
        if (pBondHeader != NULL)
        {
            FLib_MemCpy(&maBondDataBlobs[mEntryIdx].bondHeader, pBondHeader, gBleBondIdentityHeaderSize_c );
            maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask |= nvmId_BondingHeaderBit_c;
            maBondDataBlobs[mEntryIdx].bondDataSetValidHostBitmask &= (uint8_t)(~nvmId_BondingHeaderBit_c);
        }

        if (pBondDataDynamic != NULL)
        {
            FLib_MemCpy((uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobDynamic,
                        pBondDataDynamic,
                        gBleBondDataDynamicSize_c);
            maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask |= nvmId_BondingDataDynamicBit_c;
            maBondDataBlobs[mEntryIdx].bondDataSetValidHostBitmask &= (uint8_t)(~nvmId_BondingDataDynamicBit_c);
        }

        if (pBondDataStatic != NULL)
        {
            FLib_MemCpy((uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobStatic,
                        pBondDataStatic,
                        gBleBondDataStaticSize_c);
            maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask |= nvmId_BondingDataStaticBit_c;
            maBondDataBlobs[mEntryIdx].bondDataSetValidHostBitmask &= (uint8_t)(~nvmId_BondingDataStaticBit_c);
        }

        if (pBondDataLegacy != NULL)
        {
            FLib_MemCpy((uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobLegacy,
                        pBondDataLegacy,
                        gBleBondDataLegacySize_c);
            maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask |= nvmId_BondingDataLegacyBit_c;
            maBondDataBlobs[mEntryIdx].bondDataSetValidHostBitmask &= (uint8_t)(~nvmId_BondingDataLegacyBit_c);
        }

        if (pBondDataDeviceInfo != NULL)
        {
            FLib_MemCpy((uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobDeviceInfo,
                        pBondDataDeviceInfo,
                        gBleBondDataDeviceInfoSize_c);
            maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask |= nvmId_BondingDataDeviceInfoBit_c;
            maBondDataBlobs[mEntryIdx].bondDataSetValidHostBitmask &=(uint8_t)( ~nvmId_BondingDataDeviceInfoBit_c);
        }

        if (pBondDataDescriptor != NULL && mDescriptorIndex < gcGapMaximumSavedCccds_c)
        {
            FLib_MemCpy((uint8_t*)&(maBondDataBlobs[mEntryIdx].bondDataDescriptors[mDescriptorIndex]),
                        pBondDataDescriptor,
                        gBleBondDataDescriptorSize_c);
            maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask |= nvmId_BondingDataDescriptorBit_c;
            maBondDataBlobs[mEntryIdx].bondDataSetValidHostBitmask &= (uint8_t)(~nvmId_BondingDataDescriptorBit_c);
            descrIdx.u8 = 1U << mDescriptorIndex;
            maBondDataBlobs[mEntryIdx].bondDescriptorSetValidBitmask |= descrIdx.u32;
            descrIdx.u8 = 1U << mDescriptorIndex;
            maBondDataBlobs[mEntryIdx].bondDescriptorSetValidHostBitmask &= ~descrIdx.u32;
        }

        gAppNvmOperationPending = TRUE;
#else /* (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U)) */
        if (pBondHeader != NULL)
        {
            FLib_MemCpy(&maBondDataBlobs[mEntryIdx].bondHeader, pBondHeader, gBleBondIdentityHeaderSize_c);
        }

        if(pBondDataDynamic != NULL)
        {
            FLib_MemCpy((uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobDynamic,
                        pBondDataDynamic,
                        gBleBondDataDynamicSize_c
                            );
        }

        if (pBondDataStatic != NULL)
        {
            FLib_MemCpy((uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobStatic,
                        pBondDataStatic,
                        gBleBondDataStaticSize_c
                            );
        }

        if (pBondDataLegacy != NULL)
        {
            FLib_MemCpy((uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobLegacy,
                        pBondDataLegacy,
                        gBleBondDataLegacySize_c
                            );
        }

        if (pBondDataDeviceInfo != NULL)
        {
            FLib_MemCpy((uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobDeviceInfo,
                        pBondDataDeviceInfo,
                        gBleBondDataDeviceInfoSize_c
                            );
        }

        if (pBondDataDescriptor != NULL && mDescriptorIndex != gcGapMaximumSavedCccds_c)
        {
            FLib_MemCpy((uint8_t*)&(maBondDataBlobs[mEntryIdx].bondDataDescriptors[mDescriptorIndex]),
                        pBondDataDescriptor,
                        gBleBondDataDescriptorSize_c
                            );
        }
#endif /* (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U)) */
    }

    return status;
}

/*! *********************************************************************************
*\fn        bleResult_t App_NvmRead(uint8_t  mEntryIdx,
*                             void*    pBondHeader,
*                             void*    pBondDataDynamic,
*                             void*    pBondDataStatic,
*                             void*    pBondDataLegacy,
*                             void*    pBondDataDeviceInfo,
*                             void*    pBondDataDescriptor,
*                             uint8_t  mDescriptorIndex)
*\brief      Read data from NVM.
*
*\param[in]  mEntryIdx              NVM entry index.
*\param[in]  pBondHeader            Pointer to the place where the the bonding header
*                                   will be read.. Can be NULL.
*\param[in]  pBondDataDynamic       Pointer to the place where the the dynamic bonding
*                                   data structure will be read. Can be NULL.
*\param[in]  pBondDataStatic        Pointer to the place where the static bonding data
*                                   structure will be read. Can be NULL.
*\param[in]  pBondDataLegacy        Pointer to the place where the legacy bonding data
*                                   structure will be read. Can be NULL.
*\param[in]  pBondDataDeviceInfo    Pointer to the place where the bonding data device
*                                   info structure will be read. Can be NULL.
*\param[in]  pBondDataDescriptor    Pointer to the place where the bonding data descriptor
*                                    will be read. Can be NULL.
*\param[in]  mDescriptorIndex       Bonding data descriptor index.
*
*\retval    gBleInvalidParameter_c  Invalid NVM entry Id
*\retval    gBleUnexpectedError_c   Error occured while sending the request to the host
*\retval    gBleSuccess_c           Operation successfull
********************************************************************************** */
bleResult_t App_NvmRead
(
    uint8_t  mEntryIdx,
    void*    pBondHeader,
    void*    pBondDataDynamic,
    void*    pBondDataStatic,
    void*    pBondDataLegacy,
    void*    pBondDataDeviceInfo,
    void*    pBondDataDescriptor,
    uint8_t  mDescriptorIndex
)
{
    bleResult_t status = gBleSuccess_c;
    union {
      uint8_t u8;
      uint32_t u32;
    } descrIdx = {};

    if (mEntryIdx >= (uint8_t)gMaxBondedDevices_c)
    {
          status = gBleInvalidParameter_c;
    }
    else
    {
#if (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U))
        if (pBondHeader != NULL)
        {
            if ((maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask & nvmId_BondingHeaderBit_c) != 0U)
            {
                FLib_MemCpy(pBondHeader, &maBondDataBlobs[mEntryIdx].bondHeader, gBleBondIdentityHeaderSize_c);
            }
            else
            {
                status = gBleUnavailable_c;
            }
        }

        if (pBondDataDynamic != NULL)
        {
            if ((maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask & nvmId_BondingDataDynamicBit_c) != 0U)
            {
                FLib_MemCpy(pBondDataDynamic,
                            (uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobDynamic,
                            gBleBondDataDynamicSize_c);
            }
            else
            {
                status = gBleUnavailable_c;
            }
        }

        if (pBondDataStatic != NULL)
        {
            if ((maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask & nvmId_BondingDataStaticBit_c) != 0U)
            {
                FLib_MemCpy(pBondDataStatic,
                            (uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobStatic,
                            gBleBondDataStaticSize_c);
            }
            else
            {
                status = gBleUnavailable_c;
            }
        }

        if (pBondDataLegacy != NULL)
        {
            if ((maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask & nvmId_BondingDataLegacyBit_c) != 0U)
            {
                FLib_MemCpy(pBondDataLegacy,
                            (uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobLegacy,
                            gBleBondDataLegacySize_c);
            }
            else
            {
                status = gBleUnavailable_c;
            }
        }

        if (pBondDataDeviceInfo != NULL)
        {
            if ((maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask & nvmId_BondingDataDeviceInfoBit_c) != 0U)
            {
                FLib_MemCpy(pBondDataDeviceInfo,
                            (uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobDeviceInfo,
                            gBleBondDataDeviceInfoSize_c);
            }
            else
            {
                status = gBleUnavailable_c;
            }
        }

        if (pBondDataDescriptor != NULL && mDescriptorIndex < gcGapMaximumSavedCccds_c)
        {
            descrIdx.u8 = 1U << mDescriptorIndex;
            if (((maBondDataBlobs[mEntryIdx].bondDataSetValidBitmask & nvmId_BondingDataDescriptorBit_c) != 0U) &&
                ((maBondDataBlobs[mEntryIdx].bondDescriptorSetValidBitmask & descrIdx.u32) != 0U))
            {
                FLib_MemCpy(pBondDataDescriptor,
                            (uint8_t*)&(maBondDataBlobs[mEntryIdx].bondDataDescriptors[mDescriptorIndex]),
                            gBleBondDataDescriptorSize_c);
            }
            else
            {
                status = gBleUnavailable_c;
            }
        }
#else /* (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U)) */
        if (pBondHeader != NULL)
        {
            FLib_MemCpy(pBondHeader, &maBondDataBlobs[mEntryIdx].bondHeader, gBleBondIdentityHeaderSize_c);
        }

        if(pBondDataDynamic != NULL)
        {
            FLib_MemCpy(pBondDataDynamic,
                        (uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobDynamic,
                        gBleBondDataDynamicSize_c);
        }

        if (pBondDataStatic != NULL)
        {
            FLib_MemCpy(pBondDataStatic,
                        (uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobStatic,
                        gBleBondDataStaticSize_c);
        }

        if (pBondDataLegacy != NULL)
        {
            FLib_MemCpy(pBondDataLegacy,
                        (uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobLegacy,
                        gBleBondDataLegacySize_c);
        }

        if (pBondDataDeviceInfo != NULL)
        {
            FLib_MemCpy(pBondDataDeviceInfo,
                        (uint8_t*)&maBondDataBlobs[mEntryIdx].bondDataBlobDeviceInfo,
                        gBleBondDataDeviceInfoSize_c);
        }

        if (pBondDataDescriptor != NULL && mDescriptorIndex < gcGapMaximumSavedCccds_c)
        {
            FLib_MemCpy(pBondDataDescriptor,
                        (uint8_t*)&(maBondDataBlobs[mEntryIdx].bondDataDescriptors[mDescriptorIndex]),
                        gBleBondDataDescriptorSize_c);
        }
#endif /* (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U)) */
    }

    return status;
}

#if (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U))
/*! *********************************************************************************
*\fn        bleResult_t App_HostNvmRead(uint8_t  mEntryIdx,
*                             void*     pBondHeader,
*                             void*     pBondDataDynamic,
*                             void*     pBondDataStatic,
*                             void*     pBondDataLegacy,
*                             void*     pBondDataDeviceInfo,
*                             void*     pBondDataDescriptor,
*                             uint32_t  descriptorBitmask,
*                             uint8_t*  pDataSetBitmask,
*                             uint32_t* pBondDescriptorBitmask)
*\brief      Blocking function used to read data from NVM on the host processor.
*
*\param[in]  mEntryIdx              NVM entry index.
*\param[in]  pBondHeader            Pointer to the place where the the bonding header
*                                   will be read.. Can be NULL.
*\param[in]  pBondDataDynamic       Pointer to the place where the the dynamic bonding
*                                   data structure will be read. Can be NULL.
*\param[in]  pBondDataStatic        Pointer to the place where the static bonding data
*                                   structure will be read. Can be NULL.
*\param[in]  pBondDataLegacy        Pointer to the place where the legacy bonding data
*                                   structure will be read. Can be NULL.
*\param[in]  pBondDataDeviceInfo    Pointer to the place where the bonding data device
*                                   info structure will be read. Can be NULL.
*\param[in]  pBondDataDescriptor    Pointer to the place where the bonding data descriptor
*                                    will be read. Can be NULL.
*\param[in]  descriptorBitmask      Descriptor set bitmask specifying the CCCDs to be read.
*\param[in]  pDataSetBitmask        Pointer to data set bitmask specifying the data sets
*                                   that have been successfully read.
*\param[in]  pBondDescriptorBitmask Pointer to bond descriptor set bitmask specifying the
*                                   CCCDs that have been successfully read.
*
*\retval    gBleInvalidParameter_c  Invalid NVM entry Id
*\retval    gBleUnexpectedError_c   Error occured while sending the request to the host
*\retval    gBleSuccess_c           Operation successfull
********************************************************************************** */
static bleResult_t App_HostNvmRead
(
    uint8_t  mEntryIdx,
    void*    pBondHeader,
    void*    pBondDataDynamic,
    void*    pBondDataStatic,
    void*    pBondDataLegacy,
    void*    pBondDataDeviceInfo,
    void*    pBondDataDescriptor,
    uint32_t descriptorBitmask,
    uint8_t* pDataSetBitmask,
    uint32_t *pBondDescriptorBitmask
)
{
    bleResult_t status = gBleSuccess_c;
    uint8_t datasetBitmask = 0U;
    uint32_t fsciDataSize = 0U;
    uint8_t *pBuffer = NULL;
    uint8_t *pNvmData = NULL;
    appNvmHostReadNotif_t *pAppBleNvmReadData = NULL;

    if (mEntryIdx >= (uint8_t)gMaxBondedDevices_c)
    {
        status = gBleInvalidParameter_c;
    }
    else
    {
        if (pBondHeader != NULL)
        {
            datasetBitmask |= nvmId_BondingHeaderBit_c;
        }

        if (pBondDataDynamic != NULL)
        {
            datasetBitmask |= nvmId_BondingDataDynamicBit_c;
        }

        if (pBondDataStatic != NULL)
        {
            datasetBitmask |= nvmId_BondingDataStaticBit_c;
        }

        if (pBondDataLegacy != NULL)
        {
            datasetBitmask |= nvmId_BondingDataLegacyBit_c;
        }

        if (pBondDataDeviceInfo != NULL)
        {
            datasetBitmask |= nvmId_BondingDataDeviceInfoBit_c;
        }

        if (pBondDataDescriptor != NULL && descriptorBitmask != 0U)
        {
            datasetBitmask |= nvmId_BondingDataDescriptorBit_c;
        }

        fsciDataSize += 2U * sizeof(uint8_t) + sizeof(uint32_t);

        pBuffer = MEM_BufferAlloc(fsciDataSize);

        if (pBuffer != NULL)
        {
            uint8_t* pBufAux = pBuffer;

            fsciBleGetBufferFromUint8Value(mEntryIdx, pBufAux);
            fsciBleGetBufferFromUint8Value(datasetBitmask, pBufAux);
            fsciBleGetBufferFromUint32Value(descriptorBitmask, pBufAux);

            FSCI_transmitPayload(BLE_PORT_FSCI_OG, (uint8_t)g_AppBleNvmCbCmdRead_c,
                                 pBuffer, (uint16_t)fsciDataSize, gFsciInterface_c);

            BLE_PortFsciRegisterOpHandler((uint8_t)g_AppBleNvmCbCmdReadInd_c, nvmCmdReadHandler);

            /* Block and wait for core 0 response */
            BLE_PortFsciBlock(mBlockTaskNvmReadIdx);

            pAppBleNvmReadData = MSG_QueueRemoveHead(&mAppNvmInputQueue);

            if (pAppBleNvmReadData == NULL)
            {
                status = gBleOutOfMemory_c;
            }
            else
            {
                status = pAppBleNvmReadData->result;
                pNvmData = pAppBleNvmReadData->aNvmData;
                *pDataSetBitmask = pAppBleNvmReadData->dataSetBitmask;
                *pBondDescriptorBitmask = pAppBleNvmReadData->descriptorBitmask;
            }

            if (status == gBleSuccess_c)
            {
                if ((*pDataSetBitmask & nvmId_BondingHeaderBit_c) != 0U)
                {
                    FLib_MemCpy(pBondHeader, pNvmData, (gBleBondIdentityHeaderSize_c));
                    pNvmData += (gBleBondIdentityHeaderSize_c);
                }
                if ((*pDataSetBitmask & nvmId_BondingDataDynamicBit_c) != 0U)
                {
                    FLib_MemCpy(pBondDataDynamic, pNvmData, gBleBondDataDynamicSize_c);
                    pNvmData += gBleBondDataDynamicSize_c;
                }
                if ((*pDataSetBitmask & nvmId_BondingDataStaticBit_c) != 0U)
                {
                    FLib_MemCpy(pBondDataStatic, pNvmData, gBleBondDataStaticSize_c);
                    pNvmData += gBleBondDataStaticSize_c;
                }
                if ((*pDataSetBitmask & nvmId_BondingDataLegacyBit_c) != 0U)
                {
                    FLib_MemCpy(pBondDataLegacy, pNvmData, gBleBondDataLegacySize_c);
                    pNvmData += gBleBondDataLegacySize_c;
                }
                if ((*pDataSetBitmask & nvmId_BondingDataDeviceInfoBit_c) != 0U)
                {
                    FLib_MemCpy(pBondDataDeviceInfo, pNvmData, gBleBondDataDeviceInfoSize_c);
                    pNvmData += gBleBondDataDeviceInfoSize_c;
                }
                if ((*pDataSetBitmask & nvmId_BondingDataDescriptorBit_c) != 0U)
                {
                    uint32_t tempDescBitmask = *pBondDescriptorBitmask;
                    uint8_t descIdx = 0U;

                    while(tempDescBitmask != 0U)
                    {
                        descIdx = HAL_CTZ(tempDescBitmask);
                        FLib_MemCpy(&((bleBondDataDescriptorBlob_t *)pBondDataDescriptor)[descIdx], pNvmData, gBleBondDataDescriptorSize_c);
                        tempDescBitmask &= tempDescBitmask - 1U;
                        pNvmData += gBleBondDataDescriptorSize_c;
                    }
                }
            }

            (void)MSG_Free(pAppBleNvmReadData);
            (void)MEM_BufferFree(pBuffer);
        }
        else
        {
            status = gBleOutOfMemory_c;
        }
    }

    return status;
}

/*! *********************************************************************************
*\fn        bleResult_t App_NcpNvmInit(void)
*\brief     NCP NVM initialization function.
*
*\retval    gBleAlreadyInitialized_c    NCP NVM already initialized
*\retval    gBleOsError_c               Fail to create OSA event
*\retval    gBleUnexpectedError_c       Error occured while sending a request to the host
*\retval    gBleSuccess_c               Operation successfull
********************************************************************************** */
bleResult_t App_NcpNvmInit(void)
{
    bleResult_t result = gBleSuccess_c;

    MSG_QueueInit(&mAppNvmInputQueue);

    mBlockTaskNvmReadIdx = BLE_PortFsciRegisterBlockingEvent();

    /* Initialize RAM NVM entries */
    for (uint8_t i = 0U; i < (uint8_t)gMaxBondedDevices_c; i++)
    {
        result = App_HostNvmRead(i, &maBondDataBlobs[i].bondHeader,
                                 &maBondDataBlobs[i].bondDataBlobDynamic,
                                 &maBondDataBlobs[i].bondDataBlobStatic,
                                 &maBondDataBlobs[i].bondDataBlobLegacy,
                                 &maBondDataBlobs[i].bondDataBlobDeviceInfo,
                                 &maBondDataBlobs[i].bondDataDescriptors,
                                 (1U << gcGapMaximumSavedCccds_c) - 1U,
                                 &maBondDataBlobs[i].bondDataSetValidHostBitmask,
                                 &maBondDataBlobs[i].bondDescriptorSetValidHostBitmask);

        if (result == gBleSuccess_c)
        {
            /* Mark valid data sets in RAM */
            maBondDataBlobs[i].bondDataSetValidBitmask = maBondDataBlobs[i].bondDataSetValidHostBitmask;
            maBondDataBlobs[i].bondDescriptorSetValidBitmask = maBondDataBlobs[i].bondDescriptorSetValidHostBitmask;
        }
    }

    return result;
}

/*! *********************************************************************************
*\fn        void App_NvmIdle(void)
*\brief     Send BLE NVM requests to Host to save modified data sets.
*
*\retval    void
********************************************************************************** */
void App_NvmIdle(void)
{
    void* pBondHeader = NULL;
    void* pBondDataDynamic = NULL;
    void* pBondDataStatic = NULL;
    void* pBondDataLegacy = NULL;
    void* pBondDataDeviceInfo = NULL;
    void* pBondDataDescriptor = NULL;

    if (gAppNvmOperationPending)
    {
        for (uint8_t i = 0U; i < (uint8_t)gMaxBondedDevices_c; i++)
        {
            if (maBondDataBlobs[i].bondDataSetValidBitmask != maBondDataBlobs[i].bondDataSetValidHostBitmask)
            {
                if (maBondDataBlobs[i].bondDataSetValidBitmask == 0U)
                {
                    /* Entry was erased */
                    (void)App_HostNvmErase(i);
                }
                else
                {
                    if (((maBondDataBlobs[i].bondDataSetValidBitmask ^ maBondDataBlobs[i].bondDataSetValidHostBitmask)
                          & nvmId_BondingHeaderBit_c) != 0U)
                    {
                        pBondHeader = &maBondDataBlobs[i].bondHeader;
                    }
                    if (((maBondDataBlobs[i].bondDataSetValidBitmask ^ maBondDataBlobs[i].bondDataSetValidHostBitmask)
                          & nvmId_BondingDataDynamicBit_c) != 0U)
                    {
                        pBondDataDynamic = &maBondDataBlobs[i].bondDataBlobDynamic;
                    }
                    if (((maBondDataBlobs[i].bondDataSetValidBitmask ^ maBondDataBlobs[i].bondDataSetValidHostBitmask)
                          & nvmId_BondingDataStaticBit_c) != 0U)
                    {
                        pBondDataStatic = &maBondDataBlobs[i].bondDataBlobStatic;
                    }
                    if (((maBondDataBlobs[i].bondDataSetValidBitmask ^ maBondDataBlobs[i].bondDataSetValidHostBitmask)
                          & nvmId_BondingDataLegacyBit_c) != 0U)
                    {
                        pBondDataLegacy = &maBondDataBlobs[i].bondDataBlobLegacy;
                    }
                    if (((maBondDataBlobs[i].bondDataSetValidBitmask ^ maBondDataBlobs[i].bondDataSetValidHostBitmask)
                          & nvmId_BondingDataDeviceInfoBit_c) != 0U)
                    {
                        pBondDataDeviceInfo = &maBondDataBlobs[i].bondDataBlobDeviceInfo;
                    }
                    if (((maBondDataBlobs[i].bondDataSetValidBitmask ^ maBondDataBlobs[i].bondDataSetValidHostBitmask)
                          & nvmId_BondingDataDescriptorBit_c) != 0U)
                    {
                        pBondDataDescriptor = &maBondDataBlobs[i].bondDataDescriptors;
                    }

                    /* Write data on the host */
                    (void)App_HostNvmWrite(i, pBondHeader, pBondDataDynamic, pBondDataStatic,
                                           pBondDataLegacy, pBondDataDeviceInfo, pBondDataDescriptor,
                                           maBondDataBlobs[i].bondDescriptorSetValidBitmask ^ maBondDataBlobs[i].bondDescriptorSetValidHostBitmask);
                }
            }
        }

        gAppNvmOperationPending = FALSE;
    }
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\brief        Handle NVM Erase indication from host processor
*
*\param  [in]  opc      FSCI packet opcode.
*\param  [in]  len      FSCI packet len.
*\param  [in]  pParam   FSCI packet payload.
*
*\retval       void.
********************************************************************************** */
static void nvmCmdEraseHandler
(
    uint8_t opc,
    uint8_t len,
    void *pData
)
{
    uint8_t entryIdx = 0U;
    uint8_t* pBuffer = (uint8_t*)pData;
    bleResult_t result = gBleSuccess_c;

    fsciBleGetEnumValueFromBuffer(result, pBuffer, bleResult_t);
    fsciBleGetUint8ValueFromBuffer(entryIdx, pBuffer);

    if (result == gBleSuccess_c)
    {
        fsciBleGetUint8ValueFromBuffer(entryIdx, pBuffer);
        maBondDataBlobs[entryIdx].bondDataSetValidHostBitmask = 0U;
    }
}

/*! *********************************************************************************
*\brief        Handle NVM Write indication from host processor
*
*\param  [in]  opc      FSCI packet opcode.
*\param  [in]  len      FSCI packet len.
*\param  [in]  pParam   FSCI packet payload.
*
*\retval       void.
********************************************************************************** */
static void nvmCmdWriteHandler
(
    uint8_t opc,
    uint8_t len,
    void *pData
)
{
    uint8_t* pBuffer = (uint8_t*)pData;
    bleResult_t result = gBleSuccess_c;

    fsciBleGetEnumValueFromBuffer(result, pBuffer, bleResult_t);

    if (result == gBleSuccess_c)
    {
        uint8_t entryIdx = 0U;
        uint8_t datasetBitmask = 0U;
        uint32_t descriptorBitmask = 0U;

        fsciBleGetUint8ValueFromBuffer(entryIdx, pBuffer);
        fsciBleGetUint8ValueFromBuffer(datasetBitmask, pBuffer);
        fsciBleGetUint32ValueFromBuffer(descriptorBitmask, pBuffer);

        maBondDataBlobs[entryIdx].bondDataSetValidHostBitmask |= datasetBitmask;
        maBondDataBlobs[entryIdx].bondDescriptorSetValidHostBitmask |= descriptorBitmask;
    }
}

/*! *********************************************************************************
*\brief        Handle NVM Read indication from host processor
*
*\param  [in]  opc      FSCI packet opcode.
*\param  [in]  len      FSCI packet len.
*\param  [in]  pParam   FSCI packet payload.
*
*\retval       void.
********************************************************************************** */
static void nvmCmdReadHandler(uint8_t opc, uint8_t len, void *pData)
{
    appNvmHostReadNotif_t *pAppBleNvmReadData = MSG_Alloc(len);
    uint8_t *pBuffer = (uint8_t*)pData;

    if (pAppBleNvmReadData != NULL)
    {
        /* NVM Data length is packet length minus result, dataSetBitmask and dataSetBitmask */
        uint32_t nvmDataLen = (uint32_t)len - (sizeof(bleResult_t) + sizeof(uint8_t) + sizeof(uint32_t));

        fsciBleGetEnumValueFromBuffer(pAppBleNvmReadData->result, pBuffer, bleResult_t);
        fsciBleGetUint8ValueFromBuffer(pAppBleNvmReadData->dataSetBitmask, pBuffer);
        fsciBleGetUint32ValueFromBuffer(pAppBleNvmReadData->descriptorBitmask, pBuffer);
        fsciBleGetArrayFromBuffer(pAppBleNvmReadData->aNvmData, pBuffer, nvmDataLen);
        (void)MSG_QueueAddTail(&mAppNvmInputQueue, pAppBleNvmReadData);
    }

    BLE_PortFsciUnblock(mBlockTaskNvmReadIdx);
}

/*! *********************************************************************************
*\private
*\fn           static bleResult_t App_HostNvmErase(uint8_t mEntryIdx)
*\brief        This function erases the data corresponding to an entry on the host
*              processor.
*
*\param  [in]  mEntryIdx  Index of the entry that should be deleted.
*
*\retval    gBleInvalidParameter_c      Invalid entry identifier
*\retval    gBleUnexpectedError_c       Error occured while sending a request to the host
*\retval    gBleSuccess_c               Operation successfull
********************************************************************************** */
static bleResult_t App_HostNvmErase
(
    uint8_t mEntryIdx
)
{
    bleResult_t status = gBleSuccess_c;

    if(mEntryIdx >= (uint8_t)gMaxBondedDevices_c)
    {
          status = gBleInvalidParameter_c;
    }
    else
    {
        FSCI_transmitPayload(BLE_PORT_FSCI_OG, (uint8_t)g_AppBleNvmCbCmdErase_c,
                             &mEntryIdx, (uint16_t)sizeof(uint8_t), gFsciInterface_c);

        BLE_PortFsciRegisterOpHandler((uint8_t)g_AppBleNvmCbCmdEraseInd_c, nvmCmdEraseHandler);
    }

    return status;
}

/*! *********************************************************************************
*\fn           static bleResult_t App_HostNvmWrite(uint8_t  mEntryIdx,
*                                void*    pBondHeader,
*                                void*    pBondDataDynamic,
*                                void*    pBondDataStatic,
*                                void*    pBondDataLegacy,
*                                void*    pBondDataDeviceInfo,
*                                void*    pBondDataDescriptor,
*                                uint8_t  mDescriptorIndex)
*\brief        Write data to NVM on the host processor.
*
*\param  [in]  mEntryIdx              NVM entry index.
*\param  [in]  pBondHeader            Pointer to the bonding header. Can be NULL.
*\param  [in]  pBondDataDynamic       Pointer to the dynamic bonding data structure.
*                                     Can be NULL.
*\param  [in]   pBondDataStatic       Pointer to the static bonding data structure.
*                                     Can be NULL.
*\param  [in]   pBondDataLegacy       Pointer to the legacy bonding data structure.
*                                     Can be NULL.
*\param  [in]  pBondDataDeviceInfo    Pointer to the bonding data device info
*                                     structure. Can be NULL.
*\param  [in]  pBondDataDescriptor    Pointer to the bonding data descriptor array containing
*                                     the entries to be written as specified by descriptorBitmask.
*                                     Can be NULL
*\param  [in]  descriptorBitmask      Descriptor set bitmask specifying the
*                                     CCCDs to be written.
*
*\retval    gBleInvalidParameter_c  Invalid NVM entry Id
*\retval    gBleUnexpectedError_c   Error occured while sending the request to the host
*\retval    gBleOutOfMemory_c       Memory allocation fail
*\retval    gBleSuccess_c           Operation successfull
*
********************************************************************************** */
static bleResult_t App_HostNvmWrite
(
    uint8_t  mEntryIdx,
    void*    pBondHeader,
    void*    pBondDataDynamic,
    void*    pBondDataStatic,
    void*    pBondDataLegacy,
    void*    pBondDataDeviceInfo,
    void*    pBondDataDescriptor,
    uint32_t descriptorBitmask
)
{
    bleResult_t status = gBleSuccess_c;
    uint8_t datasetBitmask = 0U;
    uint16_t nvmDataSize = 0U;
    void *pNvmData = NULL;
    uint8_t noOfWriteDescriptors = 0U;
    union {
        uint8_t u8;
        uint16_t u16;
    } nvmData = {};

    if (mEntryIdx >= (uint8_t)gMaxBondedDevices_c)
    {
          status = gBleInvalidParameter_c;
    }
    else
    {
        /* Mark requested entries and compute request size */
        if (pBondHeader != NULL)
        {
            datasetBitmask |= nvmId_BondingHeaderBit_c;
            nvmDataSize += (gBleBondIdentityHeaderSize_c);
        }

        if (pBondDataDynamic != NULL)
        {
            datasetBitmask |= nvmId_BondingDataDynamicBit_c;
            nvmDataSize += gBleBondDataDynamicSize_c;
        }

        if (pBondDataStatic != NULL)
        {
            datasetBitmask |= nvmId_BondingDataStaticBit_c;
            nvmDataSize += gBleBondDataStaticSize_c;
        }

        if (pBondDataLegacy != NULL)
        {
            datasetBitmask |= nvmId_BondingDataLegacyBit_c;
            nvmDataSize += gBleBondDataLegacySize_c;
        }

        if (pBondDataDeviceInfo != NULL)
        {
            datasetBitmask |= nvmId_BondingDataDeviceInfoBit_c;
            nvmDataSize += gBleBondDataDeviceInfoSize_c;
        }

        if (pBondDataDescriptor != NULL && descriptorBitmask != 0U)
        {
            /* Count the number of bits set in descriptorBitmask */
            uint32_t tempDescBitmask = descriptorBitmask;

            for (noOfWriteDescriptors = 0U; tempDescBitmask > 0U; noOfWriteDescriptors++)
            {
                tempDescBitmask &= tempDescBitmask - 1U;
            }

            /* Compute required size for the number of CCCDs read */
            nvmData.u8 = noOfWriteDescriptors * gBleBondDataDescriptorSize_c;
            nvmDataSize += nvmData.u16;
            datasetBitmask |= nvmId_BondingDataDescriptorBit_c;
        }

        /* Allocate memory and fill in the request */
        pNvmData = MEM_BufferAlloc(nvmDataSize);

        if (pNvmData != NULL)
        {
            uint8_t *pData = pNvmData;
            uint32_t fsciDataSize = 2U * sizeof(uint8_t) + sizeof(uint32_t) + (uint32_t)nvmDataSize;
            uint8_t *pBuffer = MEM_BufferAlloc(fsciDataSize);

            if ((datasetBitmask & nvmId_BondingHeaderBit_c) != 0U)
            {
                FLib_MemCpy(pData, pBondHeader, (gBleBondIdentityHeaderSize_c));
                pData += (gBleBondIdentityHeaderSize_c);
            }
            if ((datasetBitmask & nvmId_BondingDataDynamicBit_c) != 0U)
            {
                FLib_MemCpy(pData, pBondDataDynamic, gBleBondDataDynamicSize_c);
                pData += gBleBondDataDynamicSize_c;
            }
            if ((datasetBitmask & nvmId_BondingDataStaticBit_c) != 0U)
            {
                FLib_MemCpy(pData, pBondDataStatic, gBleBondDataStaticSize_c);
                pData += gBleBondDataStaticSize_c;
            }
            if ((datasetBitmask & nvmId_BondingDataLegacyBit_c) != 0U)
            {
                FLib_MemCpy(pData, pBondDataLegacy, gBleBondDataLegacySize_c);
                pData += gBleBondDataLegacySize_c;
            }
            if ((datasetBitmask & nvmId_BondingDataDeviceInfoBit_c) != 0U)
            {
                FLib_MemCpy(pData, pBondDataDeviceInfo, gBleBondDataDeviceInfoSize_c);
                pData += gBleBondDataDeviceInfoSize_c;
            }
            if ((datasetBitmask & nvmId_BondingDataDescriptorBit_c) != 0U)
            {
                uint32_t tempDescBitmask = descriptorBitmask;
                uint8_t descIdx = 0U;

                while (tempDescBitmask != 0U)
                {
                    descIdx = HAL_CTZ(tempDescBitmask);
                    FLib_MemCpy(pData, &((bleBondDataDescriptorBlob_t *)pBondDataDescriptor)[descIdx], gBleBondDataDescriptorSize_c);
                    tempDescBitmask &= tempDescBitmask - 1U;
                    pData += gBleBondDataDescriptorSize_c;
                }
            }

            /* Send request to Host */
            if (pBuffer != NULL)
            {
                uint8_t *pBufAux = pBuffer;

                fsciBleGetBufferFromUint8Value(mEntryIdx, pBufAux);
                fsciBleGetBufferFromUint8Value(datasetBitmask, pBufAux);
                fsciBleGetBufferFromUint32Value(descriptorBitmask, pBufAux);
                fsciBleGetBufferFromArray(pNvmData, pBufAux, nvmDataSize);

                FSCI_transmitPayload(BLE_PORT_FSCI_OG, (uint8_t)g_AppBleNvmCbCmdWrite_c,
                                     pBuffer, (uint16_t)fsciDataSize, gFsciInterface_c);

                BLE_PortFsciRegisterOpHandler((uint8_t)g_AppBleNvmCbCmdWriteInd_c, nvmCmdWriteHandler);
                (void)MEM_BufferFree(pBuffer);
            }
            else
            {
                status = gBleOutOfMemory_c;
            }
        }
        else
        {
            status = gBleOutOfMemory_c;
        }
    }

    return status;
}
#endif /* (defined(gAppUseNvmNcp_d) && (gAppUseNvmNcp_d > 0U)) */
