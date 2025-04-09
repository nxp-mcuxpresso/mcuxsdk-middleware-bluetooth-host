/**********************************************************************************
 * Copyright 2025 NXP
 *
 *
 * Non-volatile storage module interface declarations for the CORTEX-M4 processor
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ********************************************************************************** */

#ifndef _NVM_INTERFACE_H
#define _NVM_INTERFACE_H


#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Enumerated data type definition for NV storage module error codes.
 */
typedef enum NVM_Status_tag
{
    gNVM_OK_c,                       /*!< operation successful */
    gNVM_Error_c,                    /*!< in case of error */
    gNVM_InvalidPageID_c,            /*!< page ID is not valid*/
    gNVM_PageIsNotBlank_c,           /*!< page is not blank */
    gNVM_SectorEraseFail_c,          /*!< page cannot be erased */
    gNVM_NullPointer_c,              /*!< provided pointer is null */
    gNVM_PointerOutOfRange_c,        /*!< pointer is out of range */
    gNVM_AddressOutOfRange_c,        /*!< address is out of range */
    gNVM_InvalidSectorsCount_c,      /*!< sector count configured in the project linker file is invalid*/
    gNVM_InvalidTableEntry_c,        /*!< table entry is not valid */
    gNVM_PageIsEmpty_c,              /*!< page is empty */
    gNVM_MetaNotFound_c,             /*!< meta info not found */
    gNVM_RecordWriteError_c,         /*!< record couldn't be written */
    gNVM_MetaInfoWriteError_c,       /*!< meta info couldn't be written */
    gNVM_ModuleNotInitialized_c,     /*!< NVM  module is not initialized */
    gNVM_CriticalSectionActive_c,    /*!< NVM  module is in critical sequence */
    gNVM_ModuleAlreadyInitialized_c, /*!< NVM  module already initialized*/
    gNVM_PageCopyPending_c,          /*!< page copy is pending */
    gNVM_RestoreFailure_c,           /*!< recovery failure */
    gNVM_FormatFailure_c,            /*!< format operation fails */
    gNVM_RegisterFailure_c,          /*!< invalid id or unmirrored data set */
    gNVM_AlreadyRegistered_c,        /*!< id already registered in another entry*/
    gNVM_SaveRequestRejected_c,      /*!< request couldn't be queued */
    gNVM_NvTableExceedFlexRAMSize_c, /*!< the table exceed the size of FlexRAM window */
    gNVM_NvWrongFlashDataIFRMap_c,   /*!< invalid data flash IFR map */
    gNVM_CannotCreateMutex_c,        /*!< no mutex available */
    gNVM_NoMemory_c,                 /*!< no free memory block */
    gNVM_IsMirroredDataSet_c,        /*!< data set is mirrored in RAM */
    gNVM_DefragBufferTooSmall_c,     /*!< buffer too small */
    gNVM_ReservedFlashTooSmall_c,    /*!< Flash buffer is too small */
    gNVM_FragmentedEntry_c,          /*!< entry fragmented */
    gNVM_AlignmentError_c,           /*!< alignment error */
    gNVM_InvalidTableEntriesCount_c, /*!< invalid table entries count */
    gNVM_SaveRequestRecursive_c,     /*!< save request flag to run again */
    gNVM_AtomicSaveRecursive_c,      /*!< atomic save request flag to run again*/
    gNVM_EccFault_c,                 /*!< ECC Fault detect on read */
    gNVM_EccFaultWritingRecord_c,    /*!< ECC Fault detected on record read back after write */
    gNVM_EccFaultWritingMeta_c,      /*!< ECC Fault detected on meta info read back after write */
    gNVM_NbStatusCodes_c             /*< Not an erro code : number of existing */

} NVM_Status_t;
/*! *********************************************************************************
 * \brief Format the NV storage system.\n
 *        The function erases both virtual pages and then writes the page counter to active page.
 *
 * \return gNVM_OK_c: if the operation completes successfully \n
 *         gNVM_FormatFailure_c:  if the format operation fails \n
 *         gNVM_ModuleNotInitialized_c: if the NVM  module is not initialised \n
 *         gNVM_CriticalSectionActive_c: if the system has entered in a critical sequence
 ********************************************************************************* */
extern NVM_Status_t NvFormat(void);
#ifdef __cplusplus
}
#endif

#endif /* _NVM_INTERFACE_H */