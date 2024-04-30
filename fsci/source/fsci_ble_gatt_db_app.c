/*! *********************************************************************************
* \addtogroup FSCI_BLE
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2019, 2021-2023 NXP
*
*
* \file
*
* This is a source file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble_gatt_db_app.h"
#include "EmbeddedTypes.h"
#if gFsciBleHost_d
    #include "host_ble.h"
#endif /* gFsciBleHost_d */


#if gFsciIncluded_c && gFsciBleGattDbAppLayerEnabled_d

/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

#if gFsciBleHost_d
    /* Macro used for saving the out parameters pointers of the GATT Database
    (application) functions */
    #define fsciBleGattDbAppSaveOutParams(pFirstParam, pSecondParam)    \
            fsciBleGattDbAppOutParams.pParam1 = (uint8_t*)pFirstParam;  \
            fsciBleGattDbAppOutParams.pParam2 = (uint8_t*)pSecondParam

    /* Macro used for restoring the out parameters pointers of the GATT Database
    (application) functions */
    #define fsciBleGattDbAppRestoreOutParams()    \
            &fsciBleGattDbAppOutParams;

    /* Macro used for cleaning the out parameters pointers of the GATT Database
    (application) functions to NULL */
    #define fsciBleGattDbAppCleanOutParams()            \
            fsciBleGattDbAppOutParams.pParam1 = NULL;   \
            fsciBleGattDbAppOutParams.pParam2 = NULL
#endif /* gFsciBleHost_d */


#define fsciBleGattDbAppCallApiFunction(apiFunction)        bleResult_t result = (apiFunction); \
                                                            fsciBleGattDbAppStatusMonitor(result)
#define fsciBleGattDbAppMonitorOutParams(functionId, ...)   if(gBleSuccess_c == result)                  \
                                                            {                                            \
                                                                FsciEvtMonitor(functionId, __VA_ARGS__); \
                                                            }

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

#if gFsciBleHost_d
    /* Structure used for keeping the out parameters pointers of the GATT Database
    (application) functions */
    typedef struct fsciBleGattDbAppOutParams_tag
    {
        uint8_t*    pParam1;
        uint8_t*    pParam2;
    }fsciBleGattDbAppOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    GattDb_ReadAttribute function */
    typedef struct fsciBleGattDbAppReadAttributeOutParams_tag
    {
        uint8_t*    pValue;
        uint16_t*   pValueLength;
    }fsciBleGattDbAppReadAttributeOutParams_t;

    /* Structure used for keeping the out parameter pointer of the
    GattDb_FindServiceHandle function */
    typedef struct fsciBleGattDbAppFindServiceHandleOutParams_tag
    {
        uint16_t*   pOutServiceHandle;
    }fsciBleGattDbAppFindServiceHandleOutParams_t;

    /* Structure used for keeping the out parameter pointer of the
    GattDb_FindCharValueHandleInService function */
    typedef struct fsciBleGattDbAppFindCharValueHandleInServiceOutParams_tag
    {
        uint16_t*   pCharValueHandle;
    }fsciBleGattDbAppFindCharValueHandleInServiceOutParams_t;

    /* Structure used for keeping the out parameter pointer of the
    GattDb_FindCccdHandleForCharValueHandle function */
    typedef struct fsciBleGattDbAppFindCccdHandleForCharValueHandleOutParams_tag
    {
        uint16_t*   pCccdHandle;
    }fsciBleGattDbAppFindCccdHandleForCharValueHandleOutParams_t;

    /* Structure used for keeping the out parameter pointer of the
    GattDb_FindDescriptorHandleForCharValueHandle function */
    typedef struct fsciBleGattDbAppFindDescriptorHandleForCharValueHandleOutParams_tag
    {
        uint16_t*   pDescriptorHandle;
    }fsciBleGattDbAppFindDescriptorHandleForCharValueHandleOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    GattDbDynamic_AddPrimaryServiceDeclaration function */
    typedef struct fsciBleGattDbAppAddPrimaryServiceDeclarationOutParams_tag
    {
        uint16_t* pOutHandle;
    }fsciBleGattDbAppAddPrimaryServiceDeclarationOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    GattDbDynamic_AddSecondaryServiceDeclaration function */
    typedef struct fsciBleGattDbAppAddSecondaryServiceDeclarationOutParams_tag
    {
        uint16_t* pOutHandle;
    }fsciBleGattDbAppAddSecondaryServiceDeclarationOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    GattDbDynamic_AddIncludeDeclaration function */
    typedef struct fsciBleGattDbAppAddIncludeDeclarationOutParams_tag
    {
        uint16_t* pOutHandle;
    }fsciBleGattDbAppAddIncludeDeclarationOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    GattDbDynamic_AddCharacteristicDeclarationAndValue function */
    typedef struct fsciBleGattDbAppAddCharacteristicDeclarationAndValueOutParams_tag
    {
        uint16_t* pOutHandle;
    }fsciBleGattDbAppAddCharacteristicDeclarationAndValueOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    GattDbDynamic_AddCharDescriptor function */
    typedef struct fsciBleGattDbAppAddCharacteristicDescriptorOutParams_tag
    {
        uint16_t* pOutHandle;
    }fsciBleGattDbAppAddCharacteristicDescriptorOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    GattDbDynamic_AddCccd function */
    typedef struct fsciBleGattDbAppAddCccdOutParams_tag
    {
        uint16_t* pOutHandle;
    }fsciBleGattDbAppAddCccdOutParams_t;

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
    /* Structure used for keeping the out parameters pointers of the
    GattDbDynamic_AddCharAggregateFormat function */
    typedef struct fsciBleGattDbAppAddCharAggrFormatOutParams_tag
    {
        uint16_t* pOutHandle;
    }fsciBleGattDbAppAddCharAggrFormatOutParams_t;
#endif

    /* Structure used for keeping the out parameters pointers of the
    GattDbDynamic_AddCharDeclWithUniqueValue function */
    typedef struct fsciBleGattDbAppAddCharacteristicDeclarationWithUniqueValueOutParams_tag
    {
        uint16_t* pOutHandle;
    }fsciBleGattDbAppAddCharacteristicDeclarationWithUniqueValueOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    GattDbDynamic_AddCharacteristicDescriptorWithUniqueValue function */
    typedef struct fsciBleGattDbAppAddCharDescriptorWithUniqueValueOutParams_tag
    {
        uint16_t* pOutHandle;
    }fsciBleGattDbAppAddCharDescriptorWithUniqueValueOutParams_t;

#endif /* gFsciBleHost_d */

/*! This is the type definition for GattDbApp op code handler pointers. */
typedef void (*pfGattDbAppOpCodeHandler_t)
(
    uint8_t     *pBuffer,
    uint32_t    fsciInterfaceId
);
/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
#if gFsciBleBBox_d || gFsciBleTest_d
static void HandleGattDbAppCmdWriteAttributeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdReadAttributeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdFindServiceHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdFindCharValueHandleInServiceOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdFindDescriptorHandleForCharValueHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdFindCccdHandleForCharValueHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdInitDatabaseOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdReleaseDatabaseOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdAddPrimaryServiceDeclarationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdAddSecondaryServiceDeclarationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdAddIncludeDeclarationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdAddCharacteristicDeclarationAndValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdAddCharacteristicDescriptorOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdAddCccdOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
#if defined(gBLE52_d) && (gBLE52_d == TRUE)
static void HandleGattDbAppCmdAddCharAggregateFormatOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
#endif
static void HandleGattDbAppCmdAddCharacteristicDeclarationWithUniqueValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdRemoveServiceOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdRemoveCharacteristicOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdAddCharDescriptorWithUniqueValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppCmdEndDatabaseUpdateOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
#endif /* gFsciBleBBox_d || gFsciBleTest_d */
#if gFsciBleHost_d
static void HandleGattDbAppStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppEvtReadAttributeValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppEvtFindServiceHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppEvtFindCharValueHandleInServiceOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppEvtFindCccdHandleForCharValueHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppEvtFindDescriptorHandleForCharValueHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppEvtAddPrimaryServiceDeclarationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppEvtAddSecondaryServiceDeclarationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppEvtAddIncludeDeclarationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppEvtAddCharacteristicDeclarationAndValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppEvtAddCharacteristicDescriptorOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppEvtAddCccdOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
#if defined(gBLE52_d) && (gBLE52_d == TRUE)
static void HandleGattDbAppEvtAddCharAggregateFormatOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
#endif
static void HandleGattDbAppEvtAddCharacteristicDeclarationWithUniqueValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattDbAppEvtAddCharDescriptorWithUniqueValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
#endif /* gFsciBleHost_d */

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

#if gFsciBleTest_d
    /* Indicates if FSCI for GATT Database (application) is enabled or not */
    static bool_t bFsciBleGattDbAppEnabled              = FALSE;

     /* Indicates if the command was initiated by FSCI (to be not monitored) */
    static bool_t bFsciBleGattDbAppCmdInitiatedByFsci   = FALSE;
#endif /* gFsciBleTest_d */

#if gFsciBleHost_d
    /* Keeps out parameters pointers for Host - BBox functionality */
    static fsciBleGattDbAppOutParams_t fsciBleGattDbAppOutParams = {NULL, NULL};
#endif /* gFsciBleHost_d */

#if gFsciBleBBox_d || gFsciBleTest_d
static const pfGattDbAppOpCodeHandler_t maGattDbAppCmdOpCodeHandlers[]=
{
NULL,                                                                         /* reserved: 0x00 */
NULL,                                                                         /* reserved: 0x01 */
HandleGattDbAppCmdWriteAttributeOpCode,                                       /* = 0x02, gBleGattDbAppCmdWriteAttributeOpCode_c */
HandleGattDbAppCmdReadAttributeOpCode,                                        /* = 0x03, gBleGattDbAppCmdReadAttributeOpCode_c */
HandleGattDbAppCmdFindServiceHandleOpCode,                                    /* = 0x04, gBleGattDbAppCmdFindServiceHandleOpCode_c */
HandleGattDbAppCmdFindCharValueHandleInServiceOpCode,                         /* = 0x05, gBleGattDbAppCmdFindCharValueHandleInServiceOpCode_c */
HandleGattDbAppCmdFindCccdHandleForCharValueHandleOpCode,                     /* = 0x06, gBleGattDbAppCmdFindCccdHandleForCharValueHandleOpCode_c */
HandleGattDbAppCmdFindDescriptorHandleForCharValueHandleOpCode,               /* = 0x07, gBleGattDbAppCmdFindDescriptorHandleForCharValueHandleOpCode_c */
HandleGattDbAppCmdInitDatabaseOpCode,                                         /* = 0x08, gBleGattDbAppCmdInitDatabaseOpCode_c */
HandleGattDbAppCmdReleaseDatabaseOpCode,                                      /* = 0x09, gBleGattDbAppCmdReleaseDatabaseOpCode_c */
HandleGattDbAppCmdAddPrimaryServiceDeclarationOpCode,                         /* = 0x0A, gBleGattDbAppCmdAddPrimaryServiceDeclarationOpCode_c */
HandleGattDbAppCmdAddSecondaryServiceDeclarationOpCode,                       /* = 0x0B, gBleGattDbAppCmdAddSecondaryServiceDeclarationOpCode_c */
HandleGattDbAppCmdAddIncludeDeclarationOpCode,                                /* = 0x0C, gBleGattDbAppCmdAddIncludeDeclarationOpCode_c */
HandleGattDbAppCmdAddCharacteristicDeclarationAndValueOpCode,                 /* = 0x0D, gBleGattDbAppCmdAddCharacteristicDeclarationAndValueOpCode_c */
HandleGattDbAppCmdAddCharacteristicDescriptorOpCode,                          /* = 0x0E, gBleGattDbAppCmdAddCharacteristicDescriptorOpCode_c */
HandleGattDbAppCmdAddCccdOpCode,                                              /* = 0x0F, gBleGattDbAppCmdAddCccdOpCode_c */
HandleGattDbAppCmdAddCharacteristicDeclarationWithUniqueValueOpCode,          /* = 0x10, gBleGattDbAppCmdAddCharacteristicDeclarationWithUniqueValueOpCode_c */
HandleGattDbAppCmdRemoveServiceOpCode,                                        /* = 0x11, gBleGattDbAppCmdRemoveServiceOpCode_c */
HandleGattDbAppCmdRemoveCharacteristicOpCode,                                 /* = 0x12, gBleGattDbAppCmdRemoveCharacteristicOpCode_c */
HandleGattDbAppCmdAddCharDescriptorWithUniqueValueOpCode,                     /* = 0x13, gBleGattDbAppCmdAddCharDescriptorWithUniqueValueOpCode_c */
HandleGattDbAppCmdEndDatabaseUpdateOpCode,                                    /* = 0x14, gBleGattDbAppCmdEndDatabaseUpdateOpCode_c */
#if defined(gBLE52_d) && (gBLE52_d == TRUE)
HandleGattDbAppCmdAddCharAggregateFormatOpCode                                /* = 0x15, gBleGattDbAppCmdAddCharAggregateFormatOpCode_c */
#else
NULL                                                                          /* reserved: 0x015 */
#endif
};
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
static const pfGattDbAppOpCodeHandler_t maGattDbAppEvtOpCodeHandlers[]=
{
HandleGattDbAppStatusOpCode,                                                  /* = 0x80, gBleGattDbAppStatusOpCode_c */
HandleGattDbAppEvtReadAttributeValueOpCode,                                   /* = 0x81, gBleGattDbAppEvtReadAttributeValueOpCode_c */
HandleGattDbAppEvtFindServiceHandleOpCode,                                    /* = 0x82, gBleGattDbAppEvtFindServiceHandleOpCode_c */
HandleGattDbAppEvtFindCharValueHandleInServiceOpCode,                         /* = 0x83, gBleGattDbAppEvtFindCharValueHandleInServiceOpCode_c */
HandleGattDbAppEvtFindCccdHandleForCharValueHandleOpCode,                     /* = 0x84, gBleGattDbAppEvtFindCccdHandleForCharValueHandleOpCode_c */
HandleGattDbAppEvtFindDescriptorHandleForCharValueHandleOpCode,               /* = 0x85, gBleGattDbAppEvtFindDescriptorHandleForCharValueHandleOpCode_c */
HandleGattDbAppEvtAddPrimaryServiceDeclarationOpCode,                         /* = 0x86, gBleGattDbAppEvtAddPrimaryServiceDeclarationOpCode_c */
HandleGattDbAppEvtAddSecondaryServiceDeclarationOpCode,                       /* = 0x87, gBleGattDbAppEvtAddSecondaryServiceDeclarationOpCode_c */
HandleGattDbAppEvtAddIncludeDeclarationOpCode,                                /* = 0x88, gBleGattDbAppEvtAddIncludeDeclarationOpCode_c */
HandleGattDbAppEvtAddCharacteristicDeclarationAndValueOpCode,                 /* = 0x89, gBleGattDbAppEvtAddCharacteristicDeclarationAndValueOpCode_c */
HandleGattDbAppEvtAddCharacteristicDescriptorOpCode,                          /* = 0x8A, gBleGattDbAppEvtAddCharacteristicDescriptorOpCode_c */
HandleGattDbAppEvtAddCccdOpCode,                                              /* = 0x8B, gBleGattDbAppEvtAddCccdOpCode_c */
HandleGattDbAppEvtAddCharacteristicDeclarationWithUniqueValueOpCode,          /* = 0X8C, gBleGattDbAppEvtAddCharacteristicDeclarationWithUniqueValueOpCode_c */
HandleGattDbAppEvtAddCharDescriptorWithUniqueValueOpCode,                     /* = 0x8D, gBleGattDbAppEvtAddCharDescriptorWithUniqueValueOpCode_c */
#if defined(gBLE52_d) && (gBLE52_d == TRUE)
HandleGattDbAppEvtAddCharAggregateFormatOpCode                                /* = 0x8E, gBleGattDbAppEvtAddCharAggregateFormatOpCode_c */
#else
NULL                                                                          /* reserved: 0x08E */
#endif
};
#endif /* gFsciBleHost_d */
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

void fsciBleGattDbAppHandler(void* pData, void* pParam, uint32_t fsciInterfaceId)
{
    clientPacket_t* pClientPacket   = (clientPacket_t*)pData;
    uint8_t*        pBuffer         = &pClientPacket->structured.payload[0];
    bool_t          opCodeHandled   = FALSE;

#if gFsciBleTest_d
    /* Mark this command as initiated by FSCI */
    bFsciBleGattDbAppCmdInitiatedByFsci = TRUE;

    /* Verify if the command is Mode Select */
    if(gBleGattDbAppModeSelectOpCode_c == (fsciBleGattDbAppOpCode_t)pClientPacket->structured.header.opCode)
    {
        fsciBleGetBoolValueFromBuffer(bFsciBleGattDbAppEnabled, pBuffer);
        /* Return status */
        fsciBleGattDbAppStatusMonitor(gBleSuccess_c);
    }
    else
    {
        /* Verify if FSCI is enabled for this layer */
        if(TRUE == bFsciBleGattDbAppEnabled)
        {
#endif /* gFsciBleTest_d*/

#if gFsciBleBBox_d || gFsciBleTest_d
            if ((pClientPacket->structured.header.opCode >= (uint8_t)gBleGattDbAppCmdWriteAttributeOpCode_c) &&
               (pClientPacket->structured.header.opCode < NumberOfElements(maGattDbAppCmdOpCodeHandlers)))
            {
                if (maGattDbAppCmdOpCodeHandlers[pClientPacket->structured.header.opCode] != NULL)
                {
                    /* Select the GATT, GATT Client or GATT Server function to be called (using the FSCI opcode) */
                    maGattDbAppCmdOpCodeHandlers[pClientPacket->structured.header.opCode](pBuffer, fsciInterfaceId);
                    opCodeHandled = TRUE;
                }
            }
#endif /* gFsciBleBBox_d || gFsciBleTest_d */
#if gFsciBleHost_d
            if ((pClientPacket->structured.header.opCode >= gBleGattDbAppStatusOpCode_c) &&
                (pClientPacket->structured.header.opCode < NumberOfElements(maGattDbAppEvtOpCodeHandlers)) &&
                (opCodeHandled == FALSE))
            {
                if (maGattDbAppEvtOpCodeHandlers[pClientPacket->structured.header.opCode - gBleGattStatusOpCode_c] != NULL)
                {
                    maGattDbAppEvtOpCodeHandlers[pClientPacket->structured.header.opCode - gBleGattStatusOpCode_c](pBuffer, fsciInterfaceId);
                    opCodeHandled = TRUE;
                }
            }
#endif /* gFsciBleHost_d */
            if (opCodeHandled == FALSE)
            {
                /* Unknown FSCI opcode */
                fsciBleError(gFsciUnknownOpcode_c, fsciInterfaceId);
            }
#if gFsciBleTest_d
        }
        else
        {
            /* FSCI SAP disabled */
            fsciBleError(gFsciSAPDisabled_c, fsciInterfaceId);
        }
    }

    /* Mark the next command as not initiated by FSCI */
    bFsciBleGattDbAppCmdInitiatedByFsci = FALSE;
#endif /* gFsciBleTest_d */

    (void)MEM_BufferFree(pData);
}


#if gFsciBleBBox_d || gFsciBleTest_d

void fsciBleGattDbAppStatusMonitor(bleResult_t result)
{
#if gFsciBleTest_d
    /* If GATT Database (application) is disabled the status must be not monitored */
    if(bFsciBleGattDbAppEnabled == FALSE)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    fsciBleStatusMonitor(gFsciBleGattDbAppOpcodeGroup_c, (uint8_t)gBleGattDbAppStatusOpCode_c, result);
}

#endif /* gFsciBleBBox_d || gFsciBleTest_d */


#if gFsciBleHost_d || gFsciBleTest_d

void fsciBleGattDbAppNoParamCmdMonitor(fsciBleGattDbAppOpCode_t opCode)
{
#if gFsciBleTest_d
    /* If GATT Database (application) is disabled or if the command was initiated by FSCI it
    must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Call the generic FSCI BLE monitor for commands or events that have no parameters */
    fsciBleNoParamCmdOrEvtMonitor(gFsciBleGattDbAppOpcodeGroup_c, (uint8_t)opCode);
}


void fsciBleGattDbAppUint16ParamCmdMonitor(fsciBleGattDbAppOpCode_t opCode, uint16_t value)
{
#if gFsciBleTest_d
    /* If GATT Database (application) is disabled or if the command was initiated by FSCI it
    must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Call the generic FSCI BLE monitor for commands or events that have an uint16_t parameters */
    fsciBleGattDbAppUint16ParamEvtMonitor(opCode, &value);
}


void fsciBleGattDbAppUuidTypeAndUuidParamCmdMonitor(fsciBleGattDbAppOpCode_t opCode, uint16_t desiredHandle, bleUuidType_t uuidType, const bleUuid_t* pUuid, uint16_t* pOutHandle)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If Application GATT Database is disabled or if the command was initiated by
    FSCI it must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)opCode,
                                                    sizeof(uint16_t) +
                                                    sizeof(bleUuidType_t) +
                                                    fsciBleGetUuidBufferSize(uuidType));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(desiredHandle, pBuffer);
    fsciBleGetBufferFromEnumValue(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetBufferFromUuid(pUuid, &pBuffer, uuidType);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGattDbAppSaveOutParams(pOutHandle, NULL);
#endif /* gFsciBleHost_d */
}


void fsciBleGattDbAppWriteAttributeCmdMonitor(uint16_t handle, uint16_t valueLength, const uint8_t* aValue)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If GATT Database (application) is disabled or if the command was initiated by FSCI it
    must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)gBleGattDbAppCmdWriteAttributeOpCode_c, sizeof(uint16_t) + sizeof(uint16_t) + valueLength);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(handle, pBuffer);
    fsciBleGetBufferFromUint16Value(valueLength, pBuffer);
    fsciBleGetBufferFromArray(aValue, pBuffer, valueLength);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGattDbAppReadAttributeCmdMonitor(uint16_t handle, uint16_t maxBytes, uint8_t* aOutValue, uint16_t*pOutValueLength)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If GATT Database (application) is disabled or if the command was initiated by FSCI it
    must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)gBleGattDbAppCmdReadAttributeOpCode_c, sizeof(uint16_t) + sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(handle, pBuffer);
    fsciBleGetBufferFromUint16Value(maxBytes, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGattDbAppSaveOutParams(aOutValue, pOutValueLength);
#endif /* gFsciBleHost_d */
}


void fsciBleGattDbAppFindCccdHandleForCharValueHandleCmdMonitor(uint16_t charValueHandle, uint16_t* pOutCccdHandle)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If GATT Database (application) is disabled or if the command was initiated by FSCI it
    must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)gBleGattDbAppCmdFindCccdHandleForCharValueHandleOpCode_c, sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(charValueHandle, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGattDbAppSaveOutParams(pOutCccdHandle, NULL);
#endif /* gFsciBleHost_d */
}


void fsciBleGattDbAppFindServiceCharValueOrDescriptorHandleCmdMonitor(fsciBleGattDbAppOpCode_t opCode, uint16_t handle, bleUuidType_t uuidType, const bleUuid_t* pUuid, uint16_t* pOutHandle)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If GATT Database (application) is disabled or if the command was initiated by FSCI it
    must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)opCode,
                                                 sizeof(uint16_t) + sizeof(bleUuidType_t) +
                                                 fsciBleGetUuidBufferSize(uuidType));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(handle, pBuffer);
    fsciBleGetBufferFromEnumValue(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetBufferFromUuid(pUuid, &pBuffer, uuidType);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGattDbAppSaveOutParams(pOutHandle, NULL);
#endif /* gFsciBleHost_d */
}


void fsciBleGattDbAppAddIncludeDeclarationCmdMonitor(uint16_t includedServiceHandle, uint16_t endGroupHandle, bleUuidType_t serviceUuidType, const bleUuid_t* pServiceUuid, uint16_t* pOutHandle)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If Application GATT Database is disabled or if the command was initiated by
    FSCI it must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)gBleGattDbAppCmdAddIncludeDeclarationOpCode_c,
                                                    sizeof(uint16_t) + sizeof(uint16_t) +
                                                    sizeof(bleUuidType_t) +
                                                    fsciBleGetUuidBufferSize(serviceUuidType));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(includedServiceHandle, pBuffer);
    fsciBleGetBufferFromUint16Value(endGroupHandle, pBuffer);
    fsciBleGetBufferFromEnumValue(serviceUuidType, pBuffer, bleUuidType_t);
    fsciBleGetBufferFromUuid(pServiceUuid, &pBuffer, serviceUuidType);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGattDbAppSaveOutParams(pOutHandle, NULL);
#endif /* gFsciBleHost_d */
}


void fsciBleGattDbAppAddCharacteristicDeclarationAndValueCmdMonitor(bleUuidType_t characteristicUuidType, const bleUuid_t* pCharacteristicUuid, gattCharacteristicPropertiesBitFields_t characteristicProperties, uint16_t maxValueLength, uint16_t initialValueLength, const uint8_t* aInitialValue, gattAttributePermissionsBitFields_t valueAccessPermissions, uint16_t* pOutHandle)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If Application GATT Database is disabled or if the command was initiated by
    FSCI it must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)gBleGattDbAppCmdAddCharacteristicDeclarationAndValueOpCode_c,
                                                    sizeof(bleUuidType_t) +
                                                    fsciBleGetUuidBufferSize(characteristicUuidType) +
                                                    sizeof(gattCharacteristicPropertiesBitFields_t) +
                                                    sizeof(uint16_t) + sizeof(uint16_t) + initialValueLength +
                                                    sizeof(gattAttributePermissionsBitFields_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromEnumValue(characteristicUuidType, pBuffer, bleUuidType_t);
    fsciBleGetBufferFromUuid(pCharacteristicUuid, &pBuffer, characteristicUuidType);
    fsciBleGetBufferFromEnumValue(characteristicProperties, pBuffer, gattCharacteristicPropertiesBitFields_t);
    fsciBleGetBufferFromUint16Value(maxValueLength, pBuffer);
    fsciBleGetBufferFromUint16Value(initialValueLength, pBuffer);
    fsciBleGetBufferFromArray(aInitialValue, pBuffer, initialValueLength);
    fsciBleGetBufferFromEnumValue(valueAccessPermissions, pBuffer, gattAttributePermissionsBitFields_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGattDbAppSaveOutParams(pOutHandle, NULL);
#endif /* gFsciBleHost_d */
}


void fsciBleGattDbAppAddCharDeclWithUniqueValCmdMonitor(bleUuidType_t characteristicUuidType, const bleUuid_t* pCharacteristicUuid, gattCharacteristicPropertiesBitFields_t characteristicProperties, gattAttributePermissionsBitFields_t valueAccessPermissions, uint16_t* pOutHandle)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If Application GATT Database is disabled or if the command was initiated by
    FSCI it must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)gBleGattDbAppCmdAddCharacteristicDeclarationWithUniqueValueOpCode_c,
                                                    sizeof(bleUuidType_t) +
                                                    fsciBleGetUuidBufferSize(characteristicUuidType) +
                                                    sizeof(gattCharacteristicPropertiesBitFields_t) +
                                                    sizeof(gattAttributePermissionsBitFields_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromEnumValue(characteristicUuidType, pBuffer, bleUuidType_t);
    fsciBleGetBufferFromUuid(pCharacteristicUuid, &pBuffer, characteristicUuidType);
    fsciBleGetBufferFromEnumValue(characteristicProperties, pBuffer, gattCharacteristicPropertiesBitFields_t);
    fsciBleGetBufferFromEnumValue(valueAccessPermissions, pBuffer, gattAttributePermissionsBitFields_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGattDbAppSaveOutParams(pOutHandle, NULL);
#endif /* gFsciBleHost_d */
}


void fsciBleGattDbAppAddCharDescrCmdMonitor( bleUuidType_t descriptorUuidType, const bleUuid_t* pDescriptorUuid, uint16_t descriptorValueLength, const uint8_t* aInitialValue, gattAttributePermissionsBitFields_t descriptorAccessPermissions, uint16_t* pOutHandle)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If Application GATT Database is disabled or if the command was initiated by
    FSCI it must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)gBleGattDbAppCmdAddCharacteristicDescriptorOpCode_c,
                                                    sizeof(bleUuidType_t) +
                                                    fsciBleGetUuidBufferSize(descriptorUuidType) +
                                                    sizeof(uint16_t) + descriptorValueLength +
                                                    sizeof(gattAttributePermissionsBitFields_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromEnumValue(descriptorUuidType, pBuffer, bleUuidType_t);
    fsciBleGetBufferFromUuid(pDescriptorUuid, &pBuffer, descriptorUuidType);
    fsciBleGetBufferFromUint16Value(descriptorValueLength, pBuffer);
    fsciBleGetBufferFromArray(aInitialValue, pBuffer, descriptorValueLength);
    fsciBleGetBufferFromEnumValue(descriptorAccessPermissions, pBuffer, gattAttributePermissionsBitFields_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGattDbAppSaveOutParams(pOutHandle, NULL);
#endif /* gFsciBleHost_d */
}


void fsciBleGattDbAppAddCccdCmdMonitor(uint16_t* pOutHandle)
{
    fsciBleGattDbAppNoParamCmdMonitor(gBleGattDbAppCmdAddCccdOpCode_c);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGattDbAppSaveOutParams(pOutHandle, NULL);
#endif /* gFsciBleHost_d */
}

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
void fsciBleGattDbAppAddCharAggrFormatCmdMonitor(uint16_t descriptorValueLength, const uint8_t* aInitialValue, uint16_t* pOutHandle)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If Application GATT Database is disabled or if the command was initiated by
    FSCI it must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)gBleGattDbAppCmdAddCharAggregateFormatOpCode_c,
                                                    sizeof(uint16_t) + descriptorValueLength);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint16Value(descriptorValueLength, pBuffer);
    fsciBleGetBufferFromArray(aInitialValue, pBuffer, descriptorValueLength);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGattDbAppSaveOutParams(pOutHandle, NULL);
#endif /* gFsciBleHost_d */
}
#endif

void fsciBleGattDbAppAddCharDescriptorWithUniqueValueCmdMonitor( bleUuidType_t descriptorUuidType,
                                                                 bleUuid_t* pDescriptorUuid,
                                                                 gattAttributePermissionsBitFields_t descriptorAccessPermissions,
                                                                 uint16_t* pOutHandle)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If Application GATT Database is disabled or if the command was initiated by
    FSCI it must be not monitored */
    if((FALSE == bFsciBleGattDbAppEnabled) ||
       (TRUE == bFsciBleGattDbAppCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)gBleGattDbAppCmdAddCharDescriptorWithUniqueValueOpCode_c,
                                                    sizeof(bleUuidType_t) +
                                                    fsciBleGetUuidBufferSize(descriptorUuidType) +
                                                    sizeof(gattAttributePermissionsBitFields_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromEnumValue(descriptorUuidType, pBuffer, bleUuidType_t);
    fsciBleGetBufferFromUuid(pDescriptorUuid, &pBuffer, descriptorUuidType);
    fsciBleGetBufferFromEnumValue(descriptorAccessPermissions, pBuffer, gattAttributePermissionsBitFields_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGattDbAppSaveOutParams(pOutHandle, NULL);
#endif /* gFsciBleHost_d */
}

#endif /* gFsciBleHost_d || gFsciBleTest_d */


#if gFsciBleBBox_d || gFsciBleTest_d


void fsciBleGattDbAppReadAttributeEvtMonitor(uint8_t* aOutValue, uint16_t* pOutValueLength)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If GATT Database (application) is disabled the event must be not monitored */
    if(bFsciBleGattDbAppEnabled == FALSE)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)gBleGattDbAppEvtReadAttributeValueOpCode_c,
                                                 sizeof(uint16_t) + *pOutValueLength);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromUint16Value(*pOutValueLength, pBuffer);
    fsciBleGetBufferFromArray(aOutValue, pBuffer, *pOutValueLength);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGattDbAppUint16ParamEvtMonitor(fsciBleGattDbAppOpCode_t opCode, const uint16_t* pValue)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If GATT Database (application) is disabled the event must be not monitored */
    if(bFsciBleGattDbAppEnabled == FALSE)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattDbAppAllocFsciPacket((uint8_t)opCode, sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromUint16Value(*pValue, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

#endif /* gFsciBleBBox_d || gFsciBleTest_d */

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

#if gFsciBleBBox_d || gFsciBleTest_d
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdWriteAttributeOpCode(uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdWriteAttributeOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdWriteAttributeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    handle = gGattDbInvalidHandle_d;
    uint16_t    valueLength = 0U;
    uint8_t*    pValue = NULL;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);
    fsciBleGetUint16ValueFromBuffer(valueLength, pBuffer);

    if (valueLength <= gAttMaxValueLength_c)
    {
        /* Allocate buffer for the attribute value */
        pValue = MEM_BufferAlloc(valueLength);
    }

    if((0U < valueLength) &&
       (NULL == pValue))
    {
        /* No memory => The GATT Database (application) function can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get attribute value from buffer */
        fsciBleGetArrayFromBuffer(pValue, pBuffer, valueLength);

        fsciBleGattDbAppCallApiFunction(GattDb_WriteAttribute(handle, valueLength, pValue));

        /* Free the buffer allocated for the attribute value */
        (void)MEM_BufferFree(pValue);
    }
}
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdReadAttributeOpCode(uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdReadAttributeOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdReadAttributeOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    handle = gGattDbInvalidHandle_d;
    uint16_t    maxBytes = 0U;
    uint8_t*    pValue = NULL;
    uint16_t    valueLength = 0U;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);
    fsciBleGetUint16ValueFromBuffer(maxBytes, pBuffer);

    /* Allocate buffer for the attribute value (consider that
    maxBytes is bigger than 0) */
    pValue = MEM_BufferAlloc(maxBytes);

    if(NULL == pValue)
    {
        /* No memory => The GATT Database (application) function can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        fsciBleGattDbAppCallApiFunction(GattDb_ReadAttribute(handle, maxBytes, pValue, &valueLength));
        fsciBleGattDbAppMonitorOutParams(ReadAttribute, pValue, &valueLength);

        /* Free the buffer allocated for the attribute value */
        (void)MEM_BufferFree(pValue);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdFindServiceHandleOpCode(uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdFindServiceHandleOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdFindServiceHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t        givenHandle = gGattDbInvalidHandle_d;
    bleUuidType_t   uuidType = {0};
    bleUuid_t       uuid = {0};
    uint16_t        requestedHandle = gGattDbInvalidHandle_d;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(givenHandle, pBuffer);
    fsciBleGetEnumValueFromBuffer(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&uuid, &pBuffer, uuidType);
    fsciBleGattDbAppCallApiFunction(GattDb_FindServiceHandle(givenHandle, uuidType, &uuid, &requestedHandle));
    fsciBleGattDbAppMonitorOutParams(FindServiceHandle, &requestedHandle);
}
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdFindCharValueHandleInServiceOpCode(
*                                                         uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdFindCharValueHandleInServiceOpCode_c 
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdFindCharValueHandleInServiceOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t        givenHandle = gGattDbInvalidHandle_d;
    bleUuidType_t   uuidType = {0};
    bleUuid_t       uuid = {0};
    uint16_t        requestedHandle = gGattDbInvalidHandle_d;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(givenHandle, pBuffer);
    fsciBleGetEnumValueFromBuffer(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&uuid, &pBuffer, uuidType);
    fsciBleGattDbAppCallApiFunction(GattDb_FindCharValueHandleInService(givenHandle, uuidType, &uuid, &requestedHandle));
    fsciBleGattDbAppMonitorOutParams(FindCharValueHandleInService, &requestedHandle);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdFindDescriptorHandleForCharValueHandleOpCode(
*                                                         uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the 
*              gBleGattDbAppCmdFindCccdHandleForCharValueHandleOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdFindDescriptorHandleForCharValueHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t        givenHandle = gGattDbInvalidHandle_d;
    bleUuidType_t   uuidType = {0};
    bleUuid_t       uuid = {0};
    uint16_t        requestedHandle = gGattDbInvalidHandle_d;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(givenHandle, pBuffer);
    fsciBleGetEnumValueFromBuffer(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&uuid, &pBuffer, uuidType);
    fsciBleGattDbAppCallApiFunction(GattDb_FindDescriptorHandleForCharValueHandle(givenHandle, uuidType, &uuid, &requestedHandle));
    fsciBleGattDbAppMonitorOutParams(FindDescriptorHandleForCharValueHandle, &requestedHandle);

}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdFindCccdHandleForCharValueHandleOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the 
*              gBleGattDbAppCmdFindDescriptorHandleForCharValueHandleOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdFindCccdHandleForCharValueHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t charValueHandle = gGattDbInvalidHandle_d;
    uint16_t cccdHandle = gGattDbInvalidHandle_d;
    /* Get command parameter from buffer */
    fsciBleGetUint16ValueFromBuffer(charValueHandle, pBuffer);
    fsciBleGattDbAppCallApiFunction(GattDb_FindCccdHandleForCharValueHandle(charValueHandle, &cccdHandle));
    fsciBleGattDbAppMonitorOutParams(FindCccdHandleForCharValueHandle, &cccdHandle);
}
                    
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdInitDatabaseOpCode(uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdInitDatabaseOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdInitDatabaseOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppCallApiFunction(GattDbDynamic_Init());
}
        
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdReleaseDatabaseOpCode(uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdReleaseDatabaseOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */            
static void HandleGattDbAppCmdReleaseDatabaseOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppCallApiFunction(GattDbDynamic_ReleaseDatabase());
}
                    
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdAddPrimaryServiceDeclarationOpCode(
*                                                          uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdAddPrimaryServiceDeclarationOpCode_c 
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdAddPrimaryServiceDeclarationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t        desiredHandle = gGattDbInvalidHandle_d;
    bleUuidType_t   serviceUuidType = {0};
    bleUuid_t       serviceUuid = {0};
    uint16_t        outHandle = gGattDbInvalidHandle_d;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(desiredHandle, pBuffer);
    fsciBleGetEnumValueFromBuffer(serviceUuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&serviceUuid, &pBuffer, serviceUuidType);

    fsciBleGattDbAppCallApiFunction(GattDbDynamic_AddPrimaryServiceDeclaration(desiredHandle, serviceUuidType, &serviceUuid, &outHandle));
    fsciBleGattDbAppMonitorOutParams(AddPrimaryServiceDeclaration, &outHandle);
}
                    
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdAddSecondaryServiceDeclarationOpCode(
*                                                          uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdAddSecondaryServiceDeclarationOpCode_c 
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdAddSecondaryServiceDeclarationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t        desiredHandle = gGattDbInvalidHandle_d;
    bleUuidType_t   serviceUuidType = {0};
    bleUuid_t       serviceUuid = {0};
    uint16_t        outHandle = gGattDbInvalidHandle_d;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(desiredHandle, pBuffer);
    fsciBleGetEnumValueFromBuffer(serviceUuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&serviceUuid, &pBuffer, serviceUuidType);

    fsciBleGattDbAppCallApiFunction(GattDbDynamic_AddSecondaryServiceDeclaration(desiredHandle, serviceUuidType, &serviceUuid, &outHandle));
    fsciBleGattDbAppMonitorOutParams(AddSecondaryServiceDeclaration, &outHandle);
}
                    
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdAddIncludeDeclarationOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdAddIncludeDeclarationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdAddIncludeDeclarationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t        includedServiceHandle = gGattDbInvalidHandle_d;
    uint16_t        endGroupHandle = gGattDbInvalidHandle_d;
    bleUuidType_t   serviceUuidType = {0};
    bleUuid_t       serviceUuid = {0};
    uint16_t        outHandle = gGattDbInvalidHandle_d;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(includedServiceHandle, pBuffer);
    fsciBleGetUint16ValueFromBuffer(endGroupHandle, pBuffer);
    fsciBleGetEnumValueFromBuffer(serviceUuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&serviceUuid, &pBuffer, serviceUuidType);

    fsciBleGattDbAppCallApiFunction(GattDbDynamic_AddIncludeDeclaration(includedServiceHandle, endGroupHandle,
                                                                        serviceUuidType, &serviceUuid, &outHandle));
    fsciBleGattDbAppMonitorOutParams(AddIncludeDeclaration, &outHandle);

}
                    
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdAddCharacteristicDeclarationAndValueOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the 
*              gBleGattDbAppCmdAddCharacteristicDeclarationAndValueOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdAddCharacteristicDeclarationAndValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleUuidType_t                               characteristicUuidType = {0};
    bleUuid_t                                   characteristicUuid = {0};
    gattCharacteristicPropertiesBitFields_t     characteristicProperties;
    uint16_t                                    maxValueLength = 0U;
    uint16_t                                    initialValueLength = 0U;
    uint8_t*                                    pInitialValue = NULL;
    gattAttributePermissionsBitFields_t         valueAccessPermissions;
    uint16_t                                    outHandle = gGattDbInvalidHandle_d;

    /* Get command parameters from buffer */
    fsciBleGetEnumValueFromBuffer(characteristicUuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&characteristicUuid, &pBuffer, characteristicUuidType);
    fsciBleGetEnumValueFromBuffer(characteristicProperties, pBuffer, gattCharacteristicPropertiesBitFields_t);
    fsciBleGetUint16ValueFromBuffer(maxValueLength, pBuffer);
    fsciBleGetUint16ValueFromBuffer(initialValueLength, pBuffer);

    if (initialValueLength <= gAttMaxValueLength_c)
    {
        /* Allocate memory buffer */
        pInitialValue = MEM_BufferAlloc(initialValueLength);
    }

    if(NULL != pInitialValue)
    {
        /* Get pInitialValue and valueAccessPermissions from buffer */
        fsciBleGetArrayFromBuffer(pInitialValue, pBuffer, initialValueLength);
        fsciBleGetEnumValueFromBuffer(valueAccessPermissions, pBuffer, gattAttributePermissionsBitFields_t);

        fsciBleGattDbAppCallApiFunction(GattDbDynamic_AddCharacteristicDeclarationAndValue(characteristicUuidType,
                                                                                           &characteristicUuid,
                                                                                           characteristicProperties,
                                                                                           maxValueLength,
                                                                                           initialValueLength,
                                                                                           pInitialValue,
                                                                                           valueAccessPermissions,
                                                                                           &outHandle));
        fsciBleGattDbAppMonitorOutParams(AddCharacteristicDeclarationAndValue, &outHandle);

        (void)MEM_BufferFree(pInitialValue);
    }
    else
    {
        /* No memory - the Application GATT Database command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}
                    
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdAddCharacteristicDescriptorOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdAddCharacteristicDescriptorOpCode_c 
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdAddCharacteristicDescriptorOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleUuidType_t                               descriptorUuidType = {0};
    bleUuid_t                                   descriptorUuid = {0};
    uint16_t                                    descriptorValueLength = 0U;
    uint8_t*                                    pInitialValue = NULL;
    gattAttributePermissionsBitFields_t         descriptorAccessPermissions;
    uint16_t                                    outHandle = gGattDbInvalidHandle_d;

    /* Get command parameters from buffer */
    fsciBleGetEnumValueFromBuffer(descriptorUuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&descriptorUuid, &pBuffer, descriptorUuidType);
    fsciBleGetUint16ValueFromBuffer(descriptorValueLength, pBuffer);

    if (descriptorValueLength <= gAttMaxValueLength_c)
    {
        /* Allocate memory buffer */
        pInitialValue = MEM_BufferAlloc(descriptorValueLength);
    }

    if(NULL != pInitialValue)
    {
        /* Get pInitialValue and descriptorAccessPermissions from buffer */
        fsciBleGetArrayFromBuffer(pInitialValue, pBuffer, descriptorValueLength);
        fsciBleGetEnumValueFromBuffer(descriptorAccessPermissions, pBuffer, gattAttributePermissionsBitFields_t);

        fsciBleGattDbAppCallApiFunction(GattDbDynamic_AddCharDescriptor(descriptorUuidType,
                                                                                  &descriptorUuid,
                                                                                  descriptorValueLength,
                                                                                  pInitialValue,
                                                                                  descriptorAccessPermissions,
                                                                                  &outHandle));
        fsciBleGattDbAppMonitorOutParams(AddCharacteristicDescriptor, &outHandle);

        (void)MEM_BufferFree(pInitialValue);
    }
    else
    {
        /* No memory - the Application GATT Database command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}
                    
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdAddCccdOpCode(uint8_t *pBuffer,
*                                                   uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdAddCccdOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdAddCccdOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t outHandle = gGattDbInvalidHandle_d;

    fsciBleGattDbAppCallApiFunction(GattDbDynamic_AddCccd(&outHandle));
    fsciBleGattDbAppMonitorOutParams(AddCccd, &outHandle);
}
                    

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdAddCharAggregateFormatOpCode(uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdAddCharAggregateFormatOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdAddCharAggregateFormatOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t outHandle = gGattDbInvalidHandle_d;
    uint16_t descriptorValueLength = 0U;
    uint8_t* pInitialValue = NULL;

    fsciBleGetUint16ValueFromBuffer(descriptorValueLength, pBuffer);

    if (descriptorValueLength <= gAttMaxValueLength_c)
    {
        /* Allocate memory buffer */
        pInitialValue = MEM_BufferAlloc(descriptorValueLength);
    }

    if(NULL != pInitialValue)
    {
        /* Get pInitialValue and descriptorAccessPermissions from buffer */
        fsciBleGetArrayFromBuffer(pInitialValue, pBuffer, descriptorValueLength);

        fsciBleGattDbAppCallApiFunction(GattDbDynamic_AddCharAggregateFormat(descriptorValueLength,
                                                                             pInitialValue,
                                                                             &outHandle));
        fsciBleGattDbAppMonitorOutParams(AddCharAggregateFormat, &outHandle);

        (void)MEM_BufferFree(pInitialValue);
    }
    else
    {
        /* No memory - the Application GATT Database command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}             
#endif

/*! *********************************************************************************
*\private
*\fn           void 
*                HandleGattDbAppCmdAddCharacteristicDeclarationWithUniqueValueOpCode(
*                                                          uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the 
*              gBleGattDbAppCmdAddCharacteristicDeclarationWithUniqueValueOpCode_c 
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdAddCharacteristicDeclarationWithUniqueValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleUuidType_t                               characteristicUuidType = {0};
    bleUuid_t                                   characteristicUuid = {0};
    gattCharacteristicPropertiesBitFields_t     characteristicProperties;
    gattAttributePermissionsBitFields_t         valueAccessPermissions;
    uint16_t                                    outHandle = gGattDbInvalidHandle_d;

    /* Get command parameters from buffer */
    fsciBleGetEnumValueFromBuffer(characteristicUuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&characteristicUuid, &pBuffer, characteristicUuidType);
    fsciBleGetEnumValueFromBuffer(characteristicProperties, pBuffer, gattCharacteristicPropertiesBitFields_t);
    fsciBleGetEnumValueFromBuffer(valueAccessPermissions, pBuffer, gattAttributePermissionsBitFields_t);

    fsciBleGattDbAppCallApiFunction(GattDbDynamic_AddCharDeclWithUniqueValue(characteristicUuidType,
                                                                                              &characteristicUuid,
                                                                                              characteristicProperties,
                                                                                              valueAccessPermissions,
                                                                                              &outHandle));
    fsciBleGattDbAppMonitorOutParams(AddCharacteristicDeclarationWithUniqueValue, &outHandle);
}
                    
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdRemoveServiceOpCode(uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdRemoveServiceOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdRemoveServiceOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t serviceHandle = gGattDbInvalidHandle_d;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(serviceHandle, pBuffer);

    fsciBleGattDbAppCallApiFunction(GattDbDynamic_RemoveService(serviceHandle));
}
                    
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdRemoveCharacteristicOpCode(uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdRemoveCharacteristicOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdRemoveCharacteristicOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t characteristicHandle = gGattDbInvalidHandle_d;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(characteristicHandle, pBuffer);

    fsciBleGattDbAppCallApiFunction(GattDbDynamic_RemoveCharacteristic(characteristicHandle));
}
                    
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdAddCharDescriptorWithUniqueValueOpCode(
*                                                         uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the 
*              gBleGattDbAppCmdAddCharDescriptorWithUniqueValueOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdAddCharDescriptorWithUniqueValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleUuidType_t                               descriptorUuidType = {0};
    bleUuid_t                                   descriptorUuid = {0};
    gattAttributePermissionsBitFields_t         descriptorAccessPermissions;
    uint16_t                                    outHandle = gGattDbInvalidHandle_d;

    /* Get command parameters from buffer */
    fsciBleGetEnumValueFromBuffer(descriptorUuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&descriptorUuid, &pBuffer, descriptorUuidType);
    fsciBleGetEnumValueFromBuffer(descriptorAccessPermissions, pBuffer, gattAttributePermissionsBitFields_t);

    fsciBleGattDbAppCallApiFunction(GattDbDynamic_AddCharDescriptorWithUniqueValue(descriptorUuidType,
                                                         &descriptorUuid,
                                                         descriptorAccessPermissions,
                                                         &outHandle));
    fsciBleGattDbAppMonitorOutParams(AddCharDescriptorWithUniqueValue, &outHandle);
}
                    
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppCmdEndDatabaseUpdateOpCode(uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppCmdEndDatabaseUpdateOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppCmdEndDatabaseUpdateOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppCallApiFunction(GattDbDynamic_EndDatabaseUpdate());
}
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppStatusOpCode(uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppStatusOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleResult_t status = gBleSuccess_c;

    fsciBleGetEnumValueFromBuffer(status, pBuffer, bleResult_t);

    if(gBleSuccess_c != status)
    {
        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtReadAttributeValueOpCode(uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppEvtReadAttributeValueOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtReadAttributeValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppReadAttributeOutParams_t* pOutParams = (fsciBleGattDbAppReadAttributeOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    /* Get out parameters of the GattDb_ReadAttribute function from buffer */
    if(NULL != pOutParams->pValueLength)
    {
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pValueLength, pBuffer);
    }

    if(NULL != pOutParams->pValue)
    {
        fsciBleGetArrayFromBuffer(pOutParams->pValue, pBuffer, *pOutParams->pValueLength);
    }

    if((NULL != pOutParams->pValueLength) ||
       (NULL != pOutParams->pValue))
    {
        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();

        /* Signal out parameters ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtFindServiceHandleOpCode(uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppEvtFindServiceHandleOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtFindServiceHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppFindServiceHandleOutParams_t* pOutParams = (fsciBleGattDbAppFindServiceHandleOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if(NULL != pOutParams->pOutServiceHandle)
    {
        /* Get out parameters of the GattDb_FindServiceHandle function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pOutServiceHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();

        /* Signal out parameter ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtFindCharValueHandleInServiceOpCode(
*                                                         uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppEvtFindCharValueHandleInServiceOpCode_c 
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtFindCharValueHandleInServiceOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppFindCharValueHandleInServiceOutParams_t* pOutParams = (fsciBleGattDbAppFindCharValueHandleInServiceOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if(NULL != pOutParams->pCharValueHandle)
    {
        /* Get out parameters of the GattDb_FindCharValueHandleInService function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pCharValueHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();

        /* Signal out parameter ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtFindCccdHandleForCharValueHandleOpCode(
*                                                          uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the 
*              gBleGattDbAppEvtFindCccdHandleForCharValueHandleOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtFindCccdHandleForCharValueHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppFindCccdHandleForCharValueHandleOutParams_t* pOutParams = (fsciBleGattDbAppFindCccdHandleForCharValueHandleOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if(NULL != pOutParams->pCccdHandle)
    {
        /* Get out parameters of the GattDb_FindCccdHandleForCharValueHandle function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pCccdHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();

        /* Signal out parameter ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtFindDescriptorHandleForCharValueHandleOpCode(
*                                                         uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the 
*              gBleGattDbAppEvtFindDescriptorHandleForCharValueHandleOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtFindDescriptorHandleForCharValueHandleOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppFindDescriptorHandleForCharValueHandleOutParams_t* pOutParams = (fsciBleGattDbAppFindDescriptorHandleForCharValueHandleOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if(NULL != pOutParams->pDescriptorHandle)
    {
        /* Get out parameters of the GattDb_FindDescriptorHandleForCharValueHandle function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pDescriptorHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();

        /* Signal out parameter ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtAddPrimaryServiceDeclarationOpCode(
*                                                          uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppEvtAddPrimaryServiceDeclarationOpCode_c 
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtAddPrimaryServiceDeclarationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppAddPrimaryServiceDeclarationOutParams_t* pOutParams = (fsciBleGattDbAppAddPrimaryServiceDeclarationOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if(NULL != pOutParams->pOutHandle)
    {
        /* Get out parameters of the GattDbDynamic_AddPrimaryServiceDeclaration function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pOutHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();
    }

    /* Signal out parameter ready */
    Ble_OutParamsReady();
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtAddSecondaryServiceDeclarationOpCode(
*                                                          uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppEvtAddSecondaryServiceDeclarationOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtAddSecondaryServiceDeclarationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppAddSecondaryServiceDeclarationOutParams_t* pOutParams = (fsciBleGattDbAppAddSecondaryServiceDeclarationOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if(NULL != pOutParams->pOutHandle)
    {
        /* Get out parameters of the GattDbDynamic_AddSecondaryServiceDeclaration function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pOutHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();
    }

    /* Signal out parameter ready */
    Ble_OutParamsReady();
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtAddIncludeDeclarationOpCode(uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppEvtAddIncludeDeclarationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtAddIncludeDeclarationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppAddIncludeDeclarationOutParams_t* pOutParams = (fsciBleGattDbAppAddIncludeDeclarationOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if(NULL != pOutParams->pOutHandle)
    {
        /* Get out parameters of the GattDbDynamic_AddIncludeDeclaration function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pOutHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();
    }

    /* Signal out parameter ready */
    Ble_OutParamsReady();
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtAddCharacteristicDeclarationAndValueOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the 
*              gBleGattDbAppEvtAddCharacteristicDeclarationAndValueOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtAddCharacteristicDeclarationAndValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppAddCharacteristicDeclarationAndValueOutParams_t* pOutParams = (fsciBleGattDbAppAddCharacteristicDeclarationAndValueOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if(NULL != pOutParams->pOutHandle)
    {
        /* Get out parameters of the GattDbDynamic_AddCharacteristicDeclarationAndValue function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pOutHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();
    }

    /* Signal out parameter ready */
    Ble_OutParamsReady();
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtAddCharacteristicDescriptorOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppEvtAddCharacteristicDescriptorOpCode_c 
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtAddCharacteristicDescriptorOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppAddCharacteristicDescriptorOutParams_t* pOutParams = (fsciBleGattDbAppAddCharacteristicDescriptorOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if(NULL != pOutParams->pOutHandle)
    {
        /* Get out parameters of the GattDbDynamic_AddCharDescriptor function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pOutHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();
    }

    /* Signal out parameter ready */
    Ble_OutParamsReady();
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtAddCccdOpCode(uint8_t *pBuffer,
*                                                   uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppEvtAddCccdOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtAddCccdOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppAddCccdOutParams_t* pOutParams = (fsciBleGattDbAppAddCccdOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if(NULL != pOutParams->pOutHandle)
    {
        /* Get out parameters of the GattDbDynamic_AddCccd function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pOutHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();
    }

    /* Signal out parameter ready */
    Ble_OutParamsReady();
}
                    
#if defined(gBLE52_d) && (gBLE52_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtAddCharAggregateFormatOpCode(uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattDbAppEvtAddCharAggregateFormatOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtAddCharAggregateFormatOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppAddCharAggrFormatOutParams_t* pOutParams = (fsciBleGattDbAppAddCharAggrFormatOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if (NULL != pOutParams->pOutHandle)
    {
        /* Get out parameters of the GattDbDynamic_AddCharAggregateFormat function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pOutHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();
    }

    /* Signal out parameter ready */
    Ble_OutParamsReady();
}              
#endif

/*! *********************************************************************************
*\private
*\fn           void 
*                HandleGattDbAppEvtAddCharacteristicDeclarationWithUniqueValueOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the 
*              gBleGattDbAppEvtAddCharacteristicDeclarationWithUniqueValueOpCode_c 
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattDbAppEvtAddCharacteristicDeclarationWithUniqueValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppAddCharacteristicDeclarationWithUniqueValueOutParams_t* pOutParams = (fsciBleGattDbAppAddCharacteristicDeclarationWithUniqueValueOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if(NULL != pOutParams->pOutHandle)
    {
        /* Get out parameters of the GattDbDynamic_AddCharDeclWithUniqueValue function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pOutHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();
    }

    /* Signal out parameter ready */
    Ble_OutParamsReady();
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattDbAppEvtAddCharDescriptorWithUniqueValueOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the 
*              gBleGattDbAppEvtAddCharDescriptorWithUniqueValueOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */    
static void HandleGattDbAppEvtAddCharDescriptorWithUniqueValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattDbAppAddCharDescriptorValueOutParams_t* pOutParams = (fsciBleGattDbAppAddCharDescriptorWithUniqueValueOutParams_t*)fsciBleGattDbAppRestoreOutParams();

    if(NULL != pOutParams->pOutHandle)
    {
        /* Get out parameters of the GattDbDynamic_AddCharacteristicDeclarationWithUniqueValue function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pOutHandle, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattDbAppCleanOutParams();
    }

    /* Signal out parameter ready */
    Ble_OutParamsReady();
}
#endif /* gFsciBleHost_d */

#endif /* gFsciIncluded_c && gFsciBleGattDbAppLayerEnabled_d */


/*! *********************************************************************************
* @}
********************************************************************************** */
