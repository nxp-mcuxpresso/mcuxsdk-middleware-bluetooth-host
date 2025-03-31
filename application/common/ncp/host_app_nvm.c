/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file
*
* This is a source file for the common application NVM code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#if (defined gAppUseNvm_d) && (gAppUseNvm_d != 0)
#include "NVM_Interface.h"
#endif /* gAppUseNvm_d */

#include "ble_config.h"
#include "ble_general.h"
#include "fwk_platform_ics.h"
#include "fsl_component_mem_manager.h"
#include "host_ble_init.h"
#include "fwk_hal_macros.h"
#include "FsciInterface.h"
#include "fsci_ble_types.h"
#include "FsciCommunication.h"
#include "host_app_nvm.h"
#include "fwk_platform_ble.h"

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
#if gAppUseNvm_d
/* Host BLE NVM command/event ids */
/* FSCI requests */
#define gAppBleNvmCbCmdEraseOpCode_c    0x00
#define gAppBleNvmCbCmdWriteOpCode_c    0x01
#define gAppBleNvmCbCmdReadOpCode_c     0x02
/* FSCI indications */
#define gAppBleNvmCbCmdEraseIndOpCode_c 0x80
#define gAppBleNvmCbCmdWriteIndOpCode_c 0x81
#define gAppBleNvmCbCmdReadIndOpCode_c  0x82

#define gAppNvmAllSavedCccds_c  0xFE

/* NVM Dataset identifiers */
#define nvmId_BondingHeaderId_c          0x4011
#define nvmId_BondingDataDynamicId_c     0x4012
#define nvmId_BondingDataStaticId_c      0x4013
#define nvmId_BondingDataLegacyId_c      0x4014
#define nvmId_BondingDataDeviceInfoId_c  0x4015
#define nvmId_BondingDataDescriptorId_c  0x4016
#define nvmId_BleLocalKeysId_c           0x4017

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
typedef PACKED_STRUCT appNvmHostEraseCmd_tag
{
    uint8_t     entryIdx;
} appNvmHostEraseCmd_t;

typedef PACKED_STRUCT appNvmHostWriteCmd_tag
{
    uint8_t     entryIdx;
    uint8_t     datasetBitmask;
    uint32_t    descriptorBitmask;
    uint8_t     aNvmData[1];
} appNvmHostWriteCmd_t;

typedef PACKED_STRUCT appNvmHostReadCmd_tag
{
    uint8_t     entryIdx;
    uint8_t     datasetBitmask;
    uint32_t    descriptorBitmask;
} appNvmHostReadCmd_t;

typedef PACKED_STRUCT appNvmHostEraseNotif_tag
{
    bleResult_t result;
    uint8_t     entryIdx;
} appNvmHostEraseNotif_t;

typedef PACKED_STRUCT appNvmHostWriteNotif_tag
{
    bleResult_t result;
    uint8_t     entryIdx;
    uint8_t     datasetBitmask;
    uint32_t    descriptorBitmask;
} appNvmHostWriteNotif_t;

typedef PACKED_STRUCT appNvmHostReadNotif_tag
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
#if gUnmirroredFeatureSet_d == TRUE
static bleBondIdentityHeaderBlob_t*  aBondingHeader[gMaxBondedDevices_c];
static bleBondDataDynamicBlob_t*     aBondingDataDynamic[gMaxBondedDevices_c];
static bleBondDataStaticBlob_t*      aBondingDataStatic[gMaxBondedDevices_c];
static bleBondDataLegacyBlob_t*      aBondingDataLegacy[gMaxBondedDevices_c];
static bleBondDataDeviceInfoBlob_t*  aBondingDataDeviceInfo[gMaxBondedDevices_c];
static bleBondDataDescriptorBlob_t*  aBondingDataDescriptor[gMaxBondedDevices_c *
                                        gcGapMaximumSavedCccds_c];

NVM_RegisterDataSet(aBondingHeader,
                    gMaxBondedDevices_c,
                    (gBleBondIdentityHeaderSize_c),
                    nvmId_BondingHeaderId_c,
                    (uint16_t)gNVM_NotMirroredInRamAutoRestore_c);
NVM_RegisterDataSet(aBondingDataDynamic,
                    gMaxBondedDevices_c,
                    gBleBondDataDynamicSize_c,
                    nvmId_BondingDataDynamicId_c,
                    (uint16_t)gNVM_NotMirroredInRamAutoRestore_c);
NVM_RegisterDataSet(aBondingDataStatic,
                    gMaxBondedDevices_c,
                    gBleBondDataStaticSize_c,
                    nvmId_BondingDataStaticId_c,
                    (uint16_t)gNVM_NotMirroredInRamAutoRestore_c);
NVM_RegisterDataSet(aBondingDataLegacy,
                    gMaxBondedDevices_c,
                    gBleBondDataLegacySize_c,
                    nvmId_BondingDataLegacyId_c,
                    (uint16_t)gNVM_NotMirroredInRamAutoRestore_c);
NVM_RegisterDataSet(aBondingDataDeviceInfo,
                    gMaxBondedDevices_c,
                    gBleBondDataDeviceInfoSize_c,
                    nvmId_BondingDataDeviceInfoId_c,
                    (uint16_t)gNVM_NotMirroredInRamAutoRestore_c);
NVM_RegisterDataSet(aBondingDataDescriptor,
                    gMaxBondedDevices_c * gcGapMaximumSavedCccds_c,
                    gBleBondDataDescriptorSize_c,
                    nvmId_BondingDataDescriptorId_c,
                    (uint16_t)gNVM_NotMirroredInRamAutoRestore_c);
#endif /* gUnmirroredFeatureSet_d */

/************************************************************************************
*************************************************************************************
* Private functions declarations
*************************************************************************************
************************************************************************************/
static bleResult_t App_NvmErase(uint8_t mEntryIdx);
static bleResult_t App_NvmWrite
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
static bleResult_t App_NvmRead
(
    uint8_t     mEntryIdx,
    void*       pBondHeader,
    void*       pBondDataDynamic,
    void*       pBondDataStatic,
    void*       pBondDataLegacy,
    void*       pBondDataDeviceInfo,
    void*       pBondDataDescriptor,
    uint32_t    descriptorBitmask,
    uint8_t*    pDataSetBitmask,
    uint32_t*   pDescriptorBitmask
);
static void App_NvmHostErase(void *pNvmReq);
static void App_NvmHostRead(void *pNvmReq);
static void App_NvmHostWrite(void *pNvmReq);
static bleResult_t App_FsciBleNvmDataReq
(
    uint8_t cmdId,
    bleResult_t nvmResult,
    uint8_t entryIdx,
    uint8_t datasetBitmask,
    uint32_t descriptorBitmask,
    uint32_t nvmDataSize,
    uint8_t *pNvmData
);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn        bleResult_t App_HostNvmInit(void)
*\brief     Host NVM initialization function.
*
*\retval    gBleAlreadyInitialized_c    NCP NVM already initialized
*\retval    gBleUnexpectedError_c       Error occured while sending a request to the host
*\retval    gBleSuccess_c               Operation successfull
********************************************************************************** */
bleResult_t App_HostNvmInit(void)
{
    static bool_t initialized = FALSE;
    bleResult_t result = gBleSuccess_c;
    
    if (initialized == TRUE)
    {
        result = gBleAlreadyInitialized_c;
    }
    else
    {
#if !defined(gAppUseNvmOnFsciBridge_d) || (gAppUseNvmOnFsciBridge_d == 0)
        gFsciStatus_t status = gFsciSuccess_c;
        
        /* Initialize FSCI for core 0 communication */
        status = FSCI_RegisterOpGroup(gFsciAppBleNvmCbOpcodeGroup_c, gFsciMonitorMode_c, App_FsciBleNvmCbHandler, NULL, 0);
        
        if (status != gFsciSuccess_c)
        {
            result = gBleUnexpectedError_c;
        }
#endif /* gAppUseNvmOnFsciBridge_d */
    }

    return result;
}

/*! *********************************************************************************
*\fn            void App_FsciBleNvmCbHandler(void* pData, void* param, uint32_t fsciInterface)
*\brief         Handle FSCI NCP NVM commands received from Host on FSCI.
*
* \param[in]    pData               Packet (containing FSCI header and FSCI
                                    payload) received over UART.
* \param[in]    param               Pointer given when this function is registered in
                                    FSCI.
* \param[in]    fsciInterfaceId     FSCI interface on which the packet was received.
*
*\retval    void
********************************************************************************** */
void App_FsciBleNvmCbHandler
(
    void*       pData,
    void*       param,
    uint32_t    fsciInterface
)
{
    clientPacket_t* pClientPacket   = (clientPacket_t*)pData;
    uint8_t*        pBuffer         = &pClientPacket->structured.payload[0];
    
    switch(pClientPacket->structured.header.opCode)
    {
        case gAppBleNvmCbCmdEraseOpCode_c:
        {
            uint8_t entryIdx = 0U;
            fsciBleGetUint8ValueFromBuffer(entryIdx, pBuffer);
            /* Avoid buffer allocation by sending the NVM entry index as parameter */
            App_PostCallbackMessage(App_NvmHostErase, (void *)entryIdx);
        }
        break;
        case gAppBleNvmCbCmdWriteOpCode_c:
        {
            appNvmHostWriteCmd_t *pNvmWriteReq = NULL;
            /* NVM data length is payload length minus entryIdx, datasetBitmask and descriptorBitmask */
            uint32_t nvmDataSize = pClientPacket->structured.header.len - (2 * sizeof(uint8_t) + sizeof(uint32_t));
            pNvmWriteReq = MEM_BufferAlloc(sizeof(appNvmHostWriteCmd_t) - sizeof(uint8_t) + nvmDataSize);
            
            if (pNvmWriteReq != NULL)
            {
                fsciBleGetUint8ValueFromBuffer(pNvmWriteReq->entryIdx, pBuffer);
                fsciBleGetUint8ValueFromBuffer(pNvmWriteReq->datasetBitmask, pBuffer);
                fsciBleGetUint32ValueFromBuffer(pNvmWriteReq->descriptorBitmask, pBuffer);
                fsciBleGetArrayFromBuffer(pNvmWriteReq->aNvmData, pBuffer, nvmDataSize);
                App_PostCallbackMessage(App_NvmHostWrite, pNvmWriteReq);
            }
        }
        break;
        case gAppBleNvmCbCmdReadOpCode_c:
        {
            appNvmHostReadCmd_t *pNvmReadReq = MEM_BufferAlloc(sizeof(appNvmHostReadCmd_t));
            
            if (pNvmReadReq != NULL)
            {
                fsciBleGetUint8ValueFromBuffer(pNvmReadReq->entryIdx, pBuffer);
                fsciBleGetUint8ValueFromBuffer(pNvmReadReq->datasetBitmask, pBuffer);
                fsciBleGetUint32ValueFromBuffer(pNvmReadReq->descriptorBitmask, pBuffer);
                App_PostCallbackMessage(App_NvmHostRead, pNvmReadReq);
            }
        }
        break;
        default:
        {
            /* Ignore other events */
        }
        break;
    }

    (void)MEM_BufferFree(pData);
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn            static void App_FsciBleNvmDataReq(uint8_t cmdId,
*                                                 bleResult_t nvmResult,
*                                                 uint8_t entryIdx,
*                                                 uint8_t datasetBitmask,
*                                                 uint32_t descriptorBitmask,
*                                                 uint32_t nvmDataSize,
*                                                 uint8_t *pNvmData)
*\brief         Format FSCI NCP NVM commands to be sent to NCP over FSCI.
*
* \param[in]    cmdId               FSCI command id
* \param[in]    nvmResult           NVM operation result
* \param[in]    entryIdx            NVM Index for the current command.
* \param[in]    datasetBitmask      dataset bitmask for the current command (used
*                                   only for read and write).
* \param[in]    descriptorBitmask   descriptor bitmask for the current command (used
*                                   only for read and write).
* \param[in]    nvmDataSize         NVM data size (used only for write command).
* \param[in]    pNvmData            Pointer to NVM data (used only for write command).
*
*\retval        gBleSuccess_c or gBleOutOfMemory_c
********************************************************************************** */
static bleResult_t App_FsciBleNvmDataReq
(
    uint8_t cmdId,
    bleResult_t nvmResult,
    uint8_t entryIdx,
    uint8_t datasetBitmask,
    uint32_t descriptorBitmask,
    uint32_t nvmDataSize,
    uint8_t *pNvmData
)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t *pClientPacket;
    uint8_t *pBuffer;
    uint32_t fsciDataSize = 0U;
    
    switch(cmdId)
    {
        case gAppBleNvmCbCmdEraseIndOpCode_c:
        {
            /* Payload is result and entryIdx */
            fsciDataSize += sizeof(bleResult_t) + sizeof(uint8_t);
        }
        break;
        case gAppBleNvmCbCmdWriteIndOpCode_c:
        {
            /* Payload is result, entryIdx, datasetBitmask and descriptorBitmask */
            fsciDataSize += sizeof(bleResult_t) + 2 * sizeof(uint8_t) + sizeof(uint32_t);
        }
        break;
        case gAppBleNvmCbCmdReadIndOpCode_c:
        {
            /* Payload is result, datasetBitmask, descriptorBitmask and nvmData */
            fsciDataSize += sizeof(bleResult_t) + sizeof(uint8_t) + sizeof(uint32_t) + nvmDataSize;
        }
        break;
        default:
        {
            /* Ignore */
        }
        break;
    }
    
    pClientPacket = MEM_BufferAlloc(fsciDataSize);
    
    if (pClientPacket != NULL)
    {
        pBuffer = pClientPacket;
        
        switch(cmdId)
        {
            case gAppBleNvmCbCmdEraseIndOpCode_c:
            {
                fsciBleGetBufferFromEnumValue(result, pBuffer, bleResult_t);
                fsciBleGetBufferFromUint8Value(entryIdx, pBuffer);
            }
            break;
            case gAppBleNvmCbCmdWriteIndOpCode_c:
            {
                fsciBleGetBufferFromEnumValue(result, pBuffer, bleResult_t);
                fsciBleGetBufferFromUint8Value(entryIdx, pBuffer);
                fsciBleGetBufferFromUint8Value(datasetBitmask, pBuffer);
                fsciBleGetBufferFromUint32Value(descriptorBitmask, pBuffer);
            }
            break;
            case gAppBleNvmCbCmdReadIndOpCode_c:
            {
                fsciBleGetBufferFromEnumValue(result, pBuffer, bleResult_t);
                fsciBleGetBufferFromUint8Value(datasetBitmask, pBuffer);
                fsciBleGetBufferFromUint32Value(descriptorBitmask, pBuffer);
                fsciBleGetBufferFromArray(pNvmData, pBuffer, nvmDataSize);
            }
            break;
            {
                /* Ignore */
            }
            break;
        }
        APP_FscitransmitPayload(gFsciAppBleNvmCbOpcodeGroup_c, cmdId, (void*)pClientPacket, fsciDataSize);
    }
    else
    {
        result = gBleOutOfMemory_c;
    }
    
    return result;
}

/*! *********************************************************************************
*\private
*\fn           static bleResult_t App_NvmErase(uint8_t mEntryIdx)
*\brief        This function erases the data corresponding to an entry.
*
*\param  [in]  mEntryIdx  Index of the entry that should be deleted.
*
*\retval    gBleInvalidParameter_c  Invalid NVM entry Id
*\retval    gBleNVMError_c          Error occured while performing an NVM operation
*\retval    gBleSuccess_c           Operation successfull
********************************************************************************** */
static bleResult_t App_NvmErase
(
    uint8_t mEntryIdx
)
{
    bleResult_t status = gBleSuccess_c;
    NVM_Status_t nvmStatus = gNVM_OK_c;
    uint32_t mDescIdx = 0U;
    
    if(mEntryIdx >= (uint8_t)gMaxBondedDevices_c)
    {
          status = gBleInvalidParameter_c;
    }
    else
    {
#if gUnmirroredFeatureSet_d == TRUE
        nvmStatus = NvErase((void**)&aBondingHeader[mEntryIdx]);
        
        if (nvmStatus == gNVM_OK_c)
        {
            nvmStatus = NvErase((void**)&aBondingDataDynamic[mEntryIdx]);
        }
        
        if (nvmStatus == gNVM_OK_c)
        {
            nvmStatus = NvErase((void**)&aBondingDataStatic[mEntryIdx]);
        }
        
        if (nvmStatus == gNVM_OK_c)
        {
            nvmStatus = NvErase((void**)&aBondingDataLegacy[mEntryIdx]);
        }
        
        if (nvmStatus == gNVM_OK_c)
        {
            nvmStatus = NvErase((void**)&aBondingDataDeviceInfo[mEntryIdx]);
        }
        
        for(mDescIdx = ((uint32_t)mEntryIdx * gcGapMaximumSavedCccds_c);
            mDescIdx < ((uint32_t)mEntryIdx + 1U) * gcGapMaximumSavedCccds_c; mDescIdx++)
        {
            nvmStatus = NvErase((void**)&aBondingDataDescriptor[mDescIdx]);
            
            if (nvmStatus != gNVM_OK_c)
            {
                break;
            }
        }
        if (nvmStatus != gNVM_OK_c)
        {
            status = gBleNVMError_c;
        }
#endif /* gUnmirroredFeatureSet_d */
    }
    
    return status;
}

/*! *********************************************************************************
*\private
*\fn           static bleResult_t App_NvmWrite(uint8_t  mEntryIdx,
*                                void*    pBondHeader,
*                                void*    pBondDataDynamic,
*                                void*    pBondDataStatic,
*                                void*    pBondDataLegacy,
*                                void*    pBondDataDeviceInfo,
*                                void*    pBondDataDescriptor,
*                                uint32_t descriptorBitmask)
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
*\param  [in]  descriptorBitmask      Bitmask of CCCDs to write.
*
*\retval    gBleInvalidParameter_c  Invalid NVM entry Id
*\retval    gBleNVMError_c          Error occured while performing an NVM operation
*\retval    gBleSuccess_c           Operation successfull
*
********************************************************************************** */
static bleResult_t App_NvmWrite
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
    NVM_Status_t nvmStatus = gNVM_OK_c;
    if(mEntryIdx >= (uint8_t)gMaxBondedDevices_c)
    {
          status = gBleInvalidParameter_c;
    }
    else
    {
        uint8_t  idx   = 0;
        
#if gUnmirroredFeatureSet_d == TRUE
        uint32_t mSize = 0;
        void**   ppNvmData = NULL;
        void*    pRamData = NULL;

        for(idx = 0U; idx < 6U; idx++)
        {
            ppNvmData = NULL;
            switch(*(uint8_t*)&idx)
            {
                case 0:
                {
                    if(pBondHeader != NULL)
                    {
                        ppNvmData = (void**)&aBondingHeader[mEntryIdx];
                        pRamData  = pBondHeader;
                        mSize     = gBleBondIdentityHeaderSize_c;
                    }
                }
                break;
                case 1:
                {
                    if(pBondDataDynamic != NULL)
                    {
                        ppNvmData = (void**)&aBondingDataDynamic[mEntryIdx];
                        pRamData  = pBondDataDynamic;
                        mSize     = gBleBondDataDynamicSize_c;
                    }
                }
                break;
                case 2:
                {
                    if(pBondDataStatic != NULL)
                    {
                        ppNvmData = (void**)&aBondingDataStatic[mEntryIdx];
                        pRamData  = pBondDataStatic;
                        mSize     = gBleBondDataStaticSize_c;
                    }
                }
                break;
                case 3:
                {
                    if(pBondDataLegacy != NULL)
                    {
                        ppNvmData = (void**)&aBondingDataLegacy[mEntryIdx];
                        pRamData  = pBondDataLegacy;
                        mSize     = gBleBondDataLegacySize_c;
                    }
                }
                break;
                case 4:
                {
                    if(pBondDataDeviceInfo != NULL)
                    {
                        ppNvmData = (void**)&aBondingDataDeviceInfo[mEntryIdx];
                        pRamData  = pBondDataDeviceInfo;
                        mSize     = gBleBondDataDeviceInfoSize_c;
                    }
                }
                break;
                case 5:
                {
                    if(pBondDataDescriptor != NULL)
                    {
                        uint32_t tempDescBitmask = descriptorBitmask;
                        uint8_t descIdx = 0U;
                        pRamData  = pBondDataDescriptor;
                        mSize     = gBleBondDataDescriptorSize_c;
                    
                        while(tempDescBitmask != 0U)
                        {
                            descIdx = HAL_CTZ(tempDescBitmask);
                            
                            ppNvmData = (void**)&aBondingDataDescriptor[mEntryIdx *
                                    gcGapMaximumSavedCccds_c + descIdx];
                            
                            if(ppNvmData != NULL)
                            {
                                if(gNVM_OK_c == NvMoveToRam(ppNvmData))
                                {
                                    FLib_MemCpy(*ppNvmData, pRamData, mSize);
                                    nvmStatus = NvSaveOnIdle(ppNvmData, FALSE);
                                }
                                else
                                {
                                    *ppNvmData = pRamData;
                                    nvmStatus = NvSyncSave(ppNvmData, FALSE);
                                }
                            }
                            
                            pRamData = (void *)((uint8_t *)pRamData + mSize);
                            
                            if (nvmStatus != gNVM_OK_c)
                            {
                                break;
                            }

                            tempDescBitmask &= tempDescBitmask - 1;
                        }
                        
                        ppNvmData = NULL;
                    }
                }
                break;
                default:
                ; /* No action required */
                break;
            }
            
            if(ppNvmData != NULL)
            {
                if(gNVM_OK_c == NvMoveToRam(ppNvmData))
                {
                    FLib_MemCpy(*ppNvmData, pRamData, mSize);
                    nvmStatus = NvSaveOnIdle(ppNvmData, FALSE);
                }
                else
                {
                    *ppNvmData = pRamData;
                    nvmStatus = NvSyncSave(ppNvmData, FALSE);
                }
            }
            
            if (nvmStatus != gNVM_OK_c)
            {
                /* An error occured, return error status. */
                status = gBleNVMError_c;
                break;
            }
        }
#endif /* gUnmirroredFeatureSet_d */
    }
    
    return status;
}

/*! *********************************************************************************
*\private
*\fn        static bleResult_t App_NvmRead(uint8_t  mEntryIdx,
*                             void*         pBondHeader,
*                             void*         pBondDataDynamic,
*                             void*         pBondDataStatic,
*                             void*         pBondDataLegacy,
*                             void*         pBondDataDeviceInfo,
*                             void*         pBondDataDescriptor,
*                             uint32_t*     descriptorBitmask
*                             uint8_t*      pDataSetBitmask,
*                             uint32_t*     pDescriptorBitmask)
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
*\param[in]  descriptorBitmask      Descriptor set bitmask specifying the  CCCDs to be written.
*\param[out] pDataSetBitmask        Pointer to data set bitmask specifying the data sets
*                                   that have been successfully read.
*\param[out] pDescriptorBitmask     Pointer to data set bitmask specifying the CCCDs
*                                   that have been successfully read.
*
*\retval    gBleInvalidParameter_c  Invalid NVM entry Id
*\retval    gBleSuccess_c           NVM data sets read as specified in pDataSetBitmask
********************************************************************************** */
static bleResult_t App_NvmRead
(
    uint8_t     mEntryIdx,
    void*       pBondHeader,
    void*       pBondDataDynamic,
    void*       pBondDataStatic,
    void*       pBondDataLegacy,
    void*       pBondDataDeviceInfo,
    void*       pBondDataDescriptor,
    uint32_t    descriptorBitmask,
    uint8_t*    pDataSetBitmask,
    uint32_t*   pDescriptorBitmask
)
{
    bleResult_t status = gBleSuccess_c;
    *pDataSetBitmask = 0U;
    *pDescriptorBitmask = 0U;

    if(mEntryIdx >= (uint8_t)gMaxBondedDevices_c)
    {
          status = gBleInvalidParameter_c;
    }
    else
    {
        uint8_t  idx = 0;
#if gUnmirroredFeatureSet_d == TRUE
        uint32_t mSize = 0;
        void**   ppNvmData = NULL;
        void*    pRamData = NULL;

        for(idx = 0U; idx < 6U; idx++)
        {
            ppNvmData = NULL;
            switch(*(uint8_t*)&idx)
            {
                case 0:
                {
                    if(pBondHeader != NULL)
                    {
                        ppNvmData = (void**)&aBondingHeader[mEntryIdx];
                        pRamData  = pBondHeader;
                        mSize     = gBleBondIdentityHeaderSize_c;

                        if((NULL != ppNvmData) && (NULL != *ppNvmData))
                        {
                            *pDataSetBitmask |= nvmId_BondingHeaderBit_c;
                        }
                    }
                }
                break;
                case 1:
                {
                    if(pBondDataDynamic != NULL)
                    {
                        ppNvmData = (void**)&aBondingDataDynamic[mEntryIdx];
                        pRamData  = pBondDataDynamic;
                        mSize     = gBleBondDataDynamicSize_c;

                        if((NULL != ppNvmData) && (NULL != *ppNvmData))
                        {
                            *pDataSetBitmask |= nvmId_BondingDataDynamicBit_c;
                        }
                    }
                }
                break;
                case 2:
                {
                    if(pBondDataStatic != NULL)
                    {
                        ppNvmData = (void**)&aBondingDataStatic[mEntryIdx];
                        pRamData  = pBondDataStatic;
                        mSize     = gBleBondDataStaticSize_c;

                        if((NULL != ppNvmData) && (NULL != *ppNvmData))
                        {
                            *pDataSetBitmask |= nvmId_BondingDataStaticBit_c;
                        }
                    }
                }
                break;
                case 3:
                {
                    if(pBondDataLegacy != NULL)
                    {
                        ppNvmData = (void**)&aBondingDataLegacy[mEntryIdx];
                        pRamData  = pBondDataLegacy;
                        mSize     = gBleBondDataLegacySize_c;

                        if((NULL != ppNvmData) && (NULL != *ppNvmData))
                        {
                            *pDataSetBitmask |= nvmId_BondingDataLegacyBit_c;
                        }
                    }
                }
                break;
                case 4:
                {
                    if(pBondDataDeviceInfo != NULL)
                    {
                        ppNvmData = (void**)&aBondingDataDeviceInfo[mEntryIdx];
                        pRamData  = pBondDataDeviceInfo;
                        mSize     = gBleBondDataDeviceInfoSize_c;

                        if((NULL != ppNvmData) && (NULL != *ppNvmData))
                        {
                            *pDataSetBitmask |= nvmId_BondingDataDeviceInfoBit_c;
                        }
                    }
                }
                break;
                case 5:
                {
                    if(pBondDataDescriptor != NULL)
                    {
                        uint32_t tempDescBitmask = descriptorBitmask;
                        uint8_t descIdx = 0U;
                        pRamData  = pBondDataDescriptor;
                        mSize     = gBleBondDataDescriptorSize_c;

                        while(tempDescBitmask != 0U)
                        {
                            descIdx = HAL_CTZ(tempDescBitmask);
                            
                            ppNvmData = (void**)&aBondingDataDescriptor[mEntryIdx *
                                gcGapMaximumSavedCccds_c + descIdx];
                                
                            if((NULL != ppNvmData) && (NULL != *ppNvmData))
                            {
                                FLib_MemCpy(pRamData, *ppNvmData, mSize);
                                pRamData = (void *)((uint8_t *)pRamData + mSize);
                                *pDataSetBitmask |= nvmId_BondingDataDescriptorBit_c;
                                *pDescriptorBitmask |= (1 << descIdx);
                            }

                            tempDescBitmask &= tempDescBitmask - 1;
                        }
                    }
                }
                break;
                default:
                {
                    ; /* No action required */
                }
                break;
            }
            
            /* if ppNvmData is not NULL the same holds for pRamData */
            if((NULL != ppNvmData) && (NULL != *ppNvmData))
            {
                FLib_MemCpy(pRamData, *ppNvmData, mSize);
            }
        }
#endif /* gUnmirroredFeatureSet_d == TRUE */
    }
    
    return status;
}

/*! *********************************************************************************
*\private
*\fn            static void App_NvmHostErase(void *data)
*\brief         Handle NCP NVM Erase command.
*
*\param[in]     data        The id for the corresponding data entry to be erased
*
*\retval        void.
********************************************************************************** */
static void App_NvmHostErase(void *data)
{
    bleResult_t result = gBleSuccess_c;
    uint32_t entryIdx = (uint32_t)data;

    result = App_NvmErase((uint8_t)entryIdx);

    (void)App_FsciBleNvmDataReq(gAppBleNvmCbCmdEraseIndOpCode_c, result, entryIdx, 0U, 0U, 0U, NULL);
}

/*! *********************************************************************************
*\private
*\fn            static void App_NvmHostRead(void *pData)
*\brief         Handle NCP NVM Erase command.
*
*\param[in]     pData       NCP NVM Read command payload
*
*\retval        void.
********************************************************************************** */
static void App_NvmHostRead(void *pData)
{
    bleResult_t result = gBleSuccess_c;
    appNvmHostReadCmd_t *pNvmReq = pData;
    uint8_t *pNvmData = NULL;
    uint16_t nvmDataSize = 0U;
    uint8_t entryIdx = pNvmReq->entryIdx;
    uint8_t requestedDatasetBitmask = pNvmReq->datasetBitmask;
    uint32_t descriptorBitmask = pNvmReq->descriptorBitmask;

    /* Buffers for requested data sets */
    uint8_t aBondHeader[gBleBondIdentityHeaderSize_c] = {0U};
    uint8_t aBondDataDynamic[gBleBondDataDynamicSize_c] = {0U};
    uint8_t aBondDataStatic[gBleBondDataStaticSize_c] = {0U};
    uint8_t aBondDataLegacy[gBleBondDataLegacySize_c] = {0U};
    uint8_t aBondDataDeviceInfo[gBleBondDataDeviceInfoSize_c] = {0U};
    uint8_t aBondDataDescriptor[gBleBondDataDescriptorSize_c * gcGapMaximumSavedCccds_c] = {0U};
    uint8_t readDatasetBitmask = 0U;
    uint32_t readDescriptorBitmask = 0U;
    uint8_t noOfReadDescriptors = 0U;

    uint8_t *pBondHeader = NULL;
    uint8_t *pBondDataDynamic = NULL;
    uint8_t *pBondDataStatic = NULL;
    uint8_t *pBondDataLegacy = NULL;
    uint8_t *pBondDataDeviceInfo = NULL;
    uint8_t *pBondDataDescriptor = NULL;

    /* Identify requested data sets */
    if (requestedDatasetBitmask & nvmId_BondingHeaderBit_c)
    {
        pBondHeader = aBondHeader;
    }
    if (requestedDatasetBitmask & nvmId_BondingDataDynamicBit_c)
    {
        pBondDataDynamic = aBondDataDynamic;
    }
    if (requestedDatasetBitmask & nvmId_BondingDataStaticBit_c)
    {
        pBondDataStatic = aBondDataStatic;
    }
    if (requestedDatasetBitmask & nvmId_BondingDataLegacyBit_c)
    {
        pBondDataLegacy = aBondDataLegacy;
    }
    if (requestedDatasetBitmask & nvmId_BondingDataDeviceInfoBit_c)
    {
        pBondDataDeviceInfo = aBondDataDeviceInfo;
    }
    if (requestedDatasetBitmask & nvmId_BondingDataDescriptorBit_c)
    {
        pBondDataDescriptor = aBondDataDescriptor;
    }

    /* Read requested data sets */
    result = App_NvmRead(entryIdx, pBondHeader, pBondDataDynamic, pBondDataStatic, pBondDataLegacy, pBondDataDeviceInfo, pBondDataDescriptor, descriptorBitmask, &readDatasetBitmask, &readDescriptorBitmask);

    /* Calculate the size of the NVM data */
    if (readDatasetBitmask & nvmId_BondingHeaderBit_c)
    {
        nvmDataSize += (gBleBondIdentityHeaderSize_c);
    }
    if (readDatasetBitmask & nvmId_BondingDataDynamicBit_c)
    {
        nvmDataSize += gBleBondDataDynamicSize_c;
    }
    if (readDatasetBitmask & nvmId_BondingDataStaticBit_c)
    {
        nvmDataSize += gBleBondDataStaticSize_c;
    }
    if (readDatasetBitmask & nvmId_BondingDataLegacyBit_c)
    {
        nvmDataSize += gBleBondDataLegacySize_c;
    }
    if (readDatasetBitmask & nvmId_BondingDataDeviceInfoBit_c)
    {
        nvmDataSize += gBleBondDataDeviceInfoSize_c;
    }
    if (readDatasetBitmask & nvmId_BondingDataDescriptorBit_c)
    {
        /* Count the number of bits set in readDescriptorBitmask */
        uint32_t tempDescBitmask = readDescriptorBitmask;
        
        for (noOfReadDescriptors = 0U; tempDescBitmask > 0U; noOfReadDescriptors++)
        {
            tempDescBitmask &= tempDescBitmask - 1;
        }
        
        /* Compute required size for the number of CCCDs read */
        nvmDataSize += noOfReadDescriptors * gBleBondDataDescriptorSize_c;
    }

    /* Allocate memory and build response */
    pNvmData = MEM_BufferAlloc(nvmDataSize);
    
    if (pNvmData != NULL)
    {
        uint8_t *pData = pNvmData;
        
        if (result == gBleSuccess_c)
        {
            if (readDatasetBitmask & nvmId_BondingHeaderBit_c)
            {
                FLib_MemCpy(pData, aBondHeader, (gBleBondIdentityHeaderSize_c));
                pData += (gBleBondIdentityHeaderSize_c);
            }
            if (readDatasetBitmask & nvmId_BondingDataDynamicBit_c)
            {
                FLib_MemCpy(pData, aBondDataDynamic, gBleBondDataDynamicSize_c);
                pData += gBleBondDataDynamicSize_c;
            }
            if (readDatasetBitmask & nvmId_BondingDataStaticBit_c)
            {
                FLib_MemCpy(pData, aBondDataStatic, gBleBondDataStaticSize_c);
                pData += gBleBondDataStaticSize_c;
            }
            if (readDatasetBitmask & nvmId_BondingDataLegacyBit_c)
            {
                FLib_MemCpy(pData, aBondDataLegacy, gBleBondDataLegacySize_c);
                pData += gBleBondDataLegacySize_c;
            }
            if (readDatasetBitmask & nvmId_BondingDataDeviceInfoBit_c)
            {
                FLib_MemCpy(pData, aBondDataDeviceInfo, gBleBondDataDeviceInfoSize_c);
                pData += gBleBondDataDeviceInfoSize_c;
            }
            if (readDatasetBitmask & nvmId_BondingDataDescriptorBit_c)
            {
                FLib_MemCpy(pData, aBondDataDescriptor, gBleBondDataDescriptorSize_c * noOfReadDescriptors);
                pData += gBleBondDataDescriptorSize_c * noOfReadDescriptors;
            }
        }

        /* Send NVM operation read response to NCP */
        (void)App_FsciBleNvmDataReq(gAppBleNvmCbCmdReadIndOpCode_c, result, 0U, readDatasetBitmask, readDescriptorBitmask, nvmDataSize, pNvmData);
        (void)MEM_BufferFree(pNvmData);
    }
}

/*! *********************************************************************************
*\private
*\fn            static void App_NvmHostWrite(void *pData)
*\brief         Handle NCP NVM Write command.
*
*\param[in]     pData       NCP NVM Write command payload
*
*\retval        void.
********************************************************************************** */
static void App_NvmHostWrite(void *pData)
{
    bleResult_t result = gBleSuccess_c;
    appNvmHostWriteCmd_t *pNvmReq = pData;
    uint8_t entryIdx = pNvmReq->entryIdx;
    uint8_t datasetBitmask = pNvmReq->datasetBitmask;
    uint32_t descriptorBitmask = pNvmReq->descriptorBitmask;
    uint8_t *pNvmData = pNvmReq->aNvmData;
    uint8_t *pBondHeader = NULL;
    uint8_t *pBondDataDynamic = NULL;
    uint8_t *pBondDataStatic = NULL;
    uint8_t *pBondDataLegacy = NULL;
    uint8_t *pBondDataDeviceInfo = NULL;
    uint8_t *pBondDataDescriptor = NULL;
    
    if (datasetBitmask & nvmId_BondingHeaderBit_c)
    {
        pBondHeader = pNvmData;
        pNvmData += (gBleBondIdentityHeaderSize_c);
    }
    if (datasetBitmask & nvmId_BondingDataDynamicBit_c)
    {
        pBondDataDynamic = pNvmData;
        pNvmData += gBleBondDataDynamicSize_c;
    }
    if (datasetBitmask & nvmId_BondingDataStaticBit_c)
    {
        pBondDataStatic = pNvmData;
        pNvmData += gBleBondDataStaticSize_c;
    }
    if (datasetBitmask & nvmId_BondingDataLegacyBit_c)
    {
        pBondDataLegacy = pNvmData;
        pNvmData += gBleBondDataLegacySize_c;
    }
    if (datasetBitmask & nvmId_BondingDataDeviceInfoBit_c)
    {
        pBondDataDeviceInfo = pNvmData;
        pNvmData += gBleBondDataDeviceInfoSize_c;
    }
    if (datasetBitmask & nvmId_BondingDataDescriptorBit_c)
    {
        pBondDataDescriptor = pNvmData;
    }

    result = App_NvmWrite(entryIdx, pBondHeader, pBondDataDynamic, pBondDataStatic, pBondDataLegacy, pBondDataDeviceInfo, pBondDataDescriptor, descriptorBitmask);

    (void)App_FsciBleNvmDataReq(gAppBleNvmCbCmdWriteIndOpCode_c, result, entryIdx, datasetBitmask, descriptorBitmask, 0U, NULL);
}

#endif /* gAppUseNvm_d */
