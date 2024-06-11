/*! *********************************************************************************
* \addtogroup FSCI_BLE
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2023-2024 NXP
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
#include "EmbeddedTypes.h"
#include "fwk_platform.h"
#include "fsci_ble_gap.h"
#include "fsci_ble_gap_types.h"
#include "fsci_ble_gap2_handlers.h"
#include "hci_types.h"
#include "fwk_seclib.h"

#if gFsciIncluded_c && gFsciBleGap2LayerEnabled_d

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
#if defined(gA2BSupportEnabled_d) && (gA2BSupportEnabled_d == TRUE)
static const bleResult_t maSecStatusMap[gSecResultPending_c + 1U] = 
{
    gBleSuccess_c,
    gBleOutOfMemory_c,
    gBleUnexpectedError_c,
    gBleInvalidParameter_c,
    gBleSuccess_c
};
#endif /* gA2BSupportEnabled_d */

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
#define fsciBleGap2CallApiFunction(apiFunction)          bleResult_t result = (apiFunction); \
                                                         fsciBleGap2StatusMonitor(result)

#define fsciBleGap2MonitorOutParams(functionId, ...)     if(gBleSuccess_c == result)                  \
                                                         {                                            \
                                                             FsciEvtMonitor(functionId, __VA_ARGS__); \
                                                         }

#define fsciBleGap2CallSecLibApiFunction(apiFunction)   secResultType_t secStatus = (apiFunction); \
                                                        bleResult_t result = (secStatus <= gSecResultPending_c) ? maSecStatusMap[secStatus] : gBleUnexpectedError_c; \
                                                        fsciBleGap2StatusMonitor(result)

#if gFsciBleBBox_d || gFsciBleTest_d
static void HandleCtrlCmdGenericHciCmd
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);

static void HandleCtrlCmdGetDebugInfoCmd
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);

static void HandleCtrlCmdGetDebugInfo2Cmd
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
#if defined(gA2BSupportEnabled_d) && (gA2BSupportEnabled_d == TRUE)
static void HandleGapCmdEcdhP256ComputeA2BKey
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);

static void HandleGapCmdEcdhP256FreeE2EKeyData
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);

static void HandleGapCmdExportA2BBlob
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);

static void HandleGapCmdImportA2BBlob
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);

static void HandleGapCmdEcdhP256GenerateKeys
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);

static void fsciBleGapComputeA2BEvtMonitor
(
    ecdhDhKey_t *pOutKey
);

static void fsciBleGapExportA2BEvtMonitor
(
    uint8_t *pOutKey
);

static void fsciBleGapImportA2BEvtMonitor
(
    uint8_t keyType,
    uint8_t *pOutKey
);

static void fsciBleGapGeneratePubPrvPairEvtMonitor
(
    ecdhPublicKey_t *pOutKey
);
#endif /* gA2BSupportEnabled_d */
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
static void HandleGap2StatusOpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
#endif /* gFsciBleHost_d */


#if defined(gBLE60_DecisionBasedAdvertisingFilteringSupport_d) && (gBLE60_DecisionBasedAdvertisingFilteringSupport_d == TRUE)
void HandleGapCmdSetExtAdvertisingDecisionDataOpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetDecisionInstructionsOpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
#endif /* gBLE60_DecisionBasedAdvertisingFilteringSupport_d */

#if defined(gBLE54_d) && (gBLE54_d == 1U)
void HandleGapCmdSetExtAdvertisingParametersV2OpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
#endif /* defined(gBLE54_d) && (gBLE54_d == 1U) */

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

#if gFsciBleBBox_d || gFsciBleTest_d
const pfGap2OpCodeHandler_t maGap2CmdOpCodeHandlers[]=
{
    HandleCtrlCmdGenericHciCmd,                                                 /* = 0x00, gBleCtrlCmdGenericHciCmdOpCode_c */
    HandleCtrlCmdGetDebugInfoCmd,                                               /* = 0x01 gBleCtrlCmdGetDebugInfoCmdOpCode_c */
#if defined(gBLE60_DecisionBasedAdvertisingFilteringSupport_d) && (gBLE60_DecisionBasedAdvertisingFilteringSupport_d == TRUE)
    HandleGapCmdSetExtAdvertisingDecisionDataOpCode,                            /* = 0x02, gBleGapCmdSetExtAdvertisingDecisionDataOpCode_c */
    HandleGapCmdSetDecisionInstructionsOpCode,                                  /* = 0x03, gBleGapCmdSetDecisionInstructionsOpCode_c */
#else /* gBLE60_DecisionBasedAdvertisingFilteringSupport_d */
    NULL,                                                                       /* reserved: 0x02 */
    NULL,                                                                       /* reserved: 0x03 */
#endif /* gBLE60_DecisionBasedAdvertisingFilteringSupport_d */
#if defined(gBLE54_d) && (gBLE54_d == 1U)
    HandleGapCmdSetExtAdvertisingParametersV2OpCode,                            /* = 0x04, gBleGapCmdSetExtAdvertisingParametersV2OpCode_c */
#else
    NULL,
#endif /* defined(gBLE54_d) && (gBLE54_d == 1U) */
    HandleCtrlCmdGetDebugInfo2Cmd,                                              /* = 0x05 gBleCtrlCmdGetDebugInfo2CmdOpCode_c */
#if defined(gA2BSupportEnabled_d) && (gA2BSupportEnabled_d == TRUE)
    HandleGapCmdEcdhP256ComputeA2BKey,                                          /* = 0x06 gBleGapCmdEcdhP256ComputeA2BKeyOpCode_c */
    HandleGapCmdEcdhP256FreeE2EKeyData,                                         /* = 0x07 gBleGapCmdEcdhP256FreeE2EKeyDataOpCode_c */
    HandleGapCmdExportA2BBlob,                                                  /* = 0x08 gBleGapCmdExportA2BBlobOpCode_c */
    HandleGapCmdImportA2BBlob,                                                  /* = 0x09 gBleGapCmdImportA2BBlobOpCode_c */
    HandleGapCmdEcdhP256GenerateKeys,                                           /* = 0x0A gBleGapCmdEcdhP256GenerateKeysOpCode_c */
#else
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif /* gA2BSupportEnabled_d */
};

#if gFsciBleTest_d
/* Indicates if FSCI for GAP is enabled or not */
static bool_t bFsciBleGap2Enabled            = FALSE;
#endif /* gFsciBleTest_d */

#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
const pfGap2OpCodeHandler_t maGap2EvtOpCodeHandlers[]=
{
    HandleGap2StatusOpCode,                                                      /* = 0x80, gBleGap2StatusOpCode_c */
}
#endif /* gFsciBleHost_d */

/*! Size of maGap2CmdOpCodeHandlers array */
const uint32_t maGap2CmdOpCodeHandlersArraySize =
    (sizeof(maGap2CmdOpCodeHandlers)/sizeof(maGap2CmdOpCodeHandlers[0]));

#if gFsciBleHost_d
/*! Size of maGap2EvtOpCodeHandlers array */
const uint32_t maGap2EvtOpCodeHandlersArraySize =
    (sizeof(maGap2EvtOpCodeHandlers)/sizeof(maGap2EvtOpCodeHandlers[0]));

/* Keeps out parameters pointers for Host - BBox functionality */
static fsciBleGap2OutParams_t fsciBleGap2OutParams = {NULL, NULL};

#endif /* gFsciBleHost_d */

/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

#if gFsciBleHost_d
    /* Macro used for saving the out parameters pointers of the GAP functions */
    #define fsciBleGap2SaveOutParams(pFirstParam, pSecondParam)      \
            fsciBleGap2OutParams.pParam1 = (uint8_t*)pFirstParam;    \
            fsciBleGap2OutParams.pParam2 = (uint8_t*)pSecondParam

    /* Macro used for restoring the out parameters pointers of the GAP functions */
    #define fsciBleGap2RestoreOutParams()    \
            &fsciBleGap2OutParams

    /* Macro used for setting the out parameters pointers of the GAP
    functions to NULL */
    #define fsciBleGap2CleanOutParams()              \
            fsciBleGap2OutParams.pParam1 = NULL;     \
            fsciBleGap2OutParams.pParam2 = NULL;
#endif /* gFsciBleHost_d */

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
#if gFsciBleTest_d
void fsciBleGap2Enable(bool_t enable)
{
    bFsciBleGap2Enabled = enable;
}
#endif /* gFsciBleTest_d */

void fsciBleGap2Handler(void* pData, void* param, uint32_t fsciInterfaceId)
{
    clientPacket_t* pClientPacket   = (clientPacket_t*)pData;
    uint8_t*        pBuffer         = &pClientPacket->structured.payload[0];
    bool_t          opCodeHandled   = FALSE;

#if gFsciBleTest_d

    /* Verify if FSCI is enabled for this layer */
    if (TRUE == bFsciBleGap2Enabled)
    {
#endif /* gFsciBleTest_d */

#if gFsciBleBBox_d || gFsciBleTest_d
    if (pClientPacket->structured.header.opCode < maGap2CmdOpCodeHandlersArraySize)
    {
        if (maGap2CmdOpCodeHandlers[pClientPacket->structured.header.opCode] != NULL)
        {
            /* Select the GAP function to be called (using the FSCI opcode) */
            maGap2CmdOpCodeHandlers[pClientPacket->structured.header.opCode](pBuffer, fsciInterfaceId);
            opCodeHandled = TRUE;
        }
    }
#endif

#if gFsciBleHost_d
    if ((pClientPacket->structured.header.opCode >= gBleGap2StatusOpCode_c) &&
       (pClientPacket->structured.header.opCode < maGap2EvtOpCodeHandlersArraySize) &&
         (opCodeHandled == FALSE))
    {
        if (maGapEvtOpCodeHandlers[pClientPacket->structured.header.opCode - gBleGap2StatusOpCode_c] != NULL)
        {
            maGapEvtOpCodeHandlers[pClientPacket->structured.header.opCode - gBleGap2StatusOpCode_c](pBuffer, fsciInterfaceId);
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
#endif /* gFsciBleTest_d */

    (void)MEM_BufferFree(pData);
}

#if gFsciBleBBox_d || gFsciBleTest_d
/*! *********************************************************************************
*\private
*\fn           void HandleCtrlCmdGenericHciCmd(uint8_t *pBuffer,
*                                                uint32_t fsciInterfaceId)
*\brief        Handler for the gBleCtrlCmdGenericHciCmdOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleCtrlCmdGenericHciCmd
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
)
{
    uint16_t opcode;
    uint8_t  payloadLength;
    uint8_t* payload;
    bleResult_t status = gBleSuccess_c;

    /* Get command parameters from buffer */
    fsciBleGetUint16ValueFromBuffer(opcode, pBuffer);
    fsciBleGetUint8ValueFromBuffer(payloadLength, pBuffer);
    if (payloadLength > 0U)
    {
        payload = MEM_BufferAlloc(payloadLength);
    }
    if ((payload != NULL) || (payloadLength == 0U))
    {
        if (payloadLength > 0U)
        {
            fsciBleGetArrayFromBuffer(payload, pBuffer, ((uint32_t)payloadLength));
        }

        /* Build HCI packet */
        uint8_t* pHciPacket = MEM_BufferAlloc(payloadLength + gHciCommandPacketHeaderLength_c);
        if (pHciPacket != NULL)
        {
            FLib_MemCpy((void*)pHciPacket, (const void*)&opcode, 2U);
            /* Set HCI parameter length */
            pHciPacket[2] = payloadLength;
            /* Fill command payload */
            if (payloadLength > 0U)
            {
                FLib_MemCpy((void*)(pHciPacket + 3U), payload, payloadLength);
            }

            /* Send HCI command */
            status = Ble_HciSend(gHciCommandPacket_c,(void*)(pHciPacket),gHciCommandPacketHeaderLength_c + payloadLength);
            fsciBleGap2StatusMonitor(status);
        }
        else
        {
            fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
        }
    }
    else
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleCtrlCmdGetDebugInfoCmd(uint8_t *pBuffer,
*                                                uint32_t fsciInterfaceId)
*\brief        Handler for the HandleCtrlCmdGetDebugInfoCmd opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleCtrlCmdGetDebugInfoCmd
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
)
{
    bleResult_t status = gBleSuccess_c;
    uint8_t pAddr[4U];
    uint32_t debugInfoAddress;
    nbuDebugInfo_t debugInfo;

    /* Get SMU debug informarion address */
    FLib_MemCpy(pAddr, pBuffer, 4U);
    debugInfoAddress = Utils_ExtractFourByteValue(pAddr);

    /* Request Radio domain to be active for safe access to shared memory */
    PLATFORM_RemoteActiveReq();

    /* Copy NBU debug data */
    FLib_MemCpy(&debugInfo, (void*)debugInfoAddress, sizeof(nbuDebugInfo_t));
    /* Release Radio Domain */
    PLATFORM_RemoteActiveRel();

    /* Command contains no parameters - Read the debug data and trigger an event with the response */
    fsciBleGap2StatusMonitor(status);

    if (status == gBleSuccess_c)
    {
        fsciBleCtrlDebugInfoCmdMonitor(&debugInfo);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleCtrlCmdGetDebugInfo2Cmd(uint8_t *pBuffer,
*                                                 uint32_t fsciInterfaceId)
*\brief        Handler for the HandleCtrlCmdGetDebugInfo2Cmd opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleCtrlCmdGetDebugInfo2Cmd
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
)
{
    bleResult_t status = gBleSuccess_c;
    uint8_t pAddr[4U];
    uint32_t debugInfoAddress;
    nbuDebugInfo2_t debugInfo;

    /* Get SMU debug informarion address */
    FLib_MemCpy(pAddr, pBuffer, 4U);
    debugInfoAddress = Utils_ExtractFourByteValue(pAddr);

    /* Request Radio domain to be active for safe access to shared memory */
    PLATFORM_RemoteActiveReq();

    /* Copy NBU debug data */
    FLib_MemCpy(&debugInfo, (void*)debugInfoAddress, sizeof(nbuDebugInfo2_t));
    /* Release Radio Domain */
    PLATFORM_RemoteActiveRel();

    /* Command contains no parameters - Read the debug data and trigger an event with the response */
    fsciBleGap2StatusMonitor(status);

    if (status == gBleSuccess_c)
    {
        fsciBleCtrlDebugInfo2CmdMonitor(&debugInfo);
    }
}

#if defined(gA2BSupportEnabled_d) && (gA2BSupportEnabled_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEcdhP256ComputeA2BKey(uint8_t *pBuffer,
*                                                     uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEcdhP256ComputeA2BKeyOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGapCmdEcdhP256ComputeA2BKey
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
)
{
    ecdhPublicKey_t peerPubKey = {0U};
    ecdhDhKey_t outE2EKey = {0U};
    
    if (gSecLibFunctions.pfECDH_P256_ComputeA2BKey != NULL)
    {
        fsciBleGetArrayFromBuffer(&peerPubKey, pBuffer, sizeof(ecdhPublicKey_t));
        
        fsciBleGap2CallSecLibApiFunction(gSecLibFunctions.pfECDH_P256_ComputeA2BKey(&peerPubKey, &outE2EKey));
        fsciBleGap2MonitorOutParams(ComputeA2B, &outE2EKey);
    }
    else
    {
        fsciBleGap2StatusMonitor(gBleUnavailable_c);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEcdhP256FreeE2EKeyData(uint8_t *pBuffer,
*                                                      uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEcdhP256FreeE2EKeyDataOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGapCmdEcdhP256FreeE2EKeyData
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
)
{
    ecdhDhKey_t E2EKey = {0U};
    
    if (gSecLibFunctions.pfECDH_P256_FreeE2EKeyData != NULL)
    {
        fsciBleGetArrayFromBuffer(&E2EKey, pBuffer, sizeof(ecdhDhKey_t));
        fsciBleGap2CallSecLibApiFunction(gSecLibFunctions.pfECDH_P256_FreeE2EKeyData(&E2EKey));
    }
    else
    {
        fsciBleGap2StatusMonitor(gBleUnavailable_c);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdExportA2BBlob(uint8_t *pBuffer,
*                                             uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdExportA2BBlobOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGapCmdExportA2BBlob
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
)
{
    uint8_t aKeyData[gSecLibElkeBlobSize_c] = {0U};
    uint8_t aE2EKeyBlob[gSecLibElkeBlobSize_c] = {0U};
    secInputKeyType_t keyType = gSecPlainText_c;
    
    if (gSecLibFunctions.pfSecLib_ExportA2BBlob != NULL)
    {
        keyType = (secInputKeyType_t)*pBuffer;
        pBuffer = &pBuffer[1];
        
        switch (keyType)
        {
            case gSecPlainText_c:
            {
                fsciBleGetArrayFromBuffer(aKeyData, pBuffer, 16U);
            }
            break;
            case gSecElkeBlob_c:
            case gSecLtkElkeBlob_c:
            {
                fsciBleGetArrayFromBuffer(aKeyData, pBuffer, gSecLibElkeBlobSize_c);
            }
            break;
            default:
            {
                ; /* MISRA compliance */
            }
            break;
        }
        
        fsciBleGap2CallSecLibApiFunction(gSecLibFunctions.pfSecLib_ExportA2BBlob(aKeyData, (secInputKeyType_t)keyType, aE2EKeyBlob));
        fsciBleGap2MonitorOutParams(ExportA2B, aE2EKeyBlob);
    }
    else
    {
        fsciBleGap2StatusMonitor(gBleUnavailable_c);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdImportA2BBlob(uint8_t *pBuffer,
*                                             uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdImportA2BBlobOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGapCmdImportA2BBlob
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
)
{
    uint8_t aE2EKeyBlob[gSecLibElkeBlobSize_c] = {0U};
    uint8_t aKeyData[gSecLibElkeBlobSize_c] = {0U};
    uint8_t keyType = 0U;
    
    if (gSecLibFunctions.pfSecLib_ImportA2BBlob != NULL)
    {
        fsciBleGetUint8ValueFromBuffer(keyType, pBuffer);
        fsciBleGetArrayFromBuffer(aE2EKeyBlob, pBuffer, gSecLibElkeBlobSize_c);
        fsciBleGap2CallSecLibApiFunction(gSecLibFunctions.pfSecLib_ImportA2BBlob(aE2EKeyBlob, (secInputKeyType_t)keyType, aKeyData));
        fsciBleGap2MonitorOutParams(ImportA2B, keyType, aKeyData);
    }
    else
    {
        fsciBleGap2StatusMonitor(gBleUnavailable_c);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEcdhP256GenerateKeys(uint8_t *pBuffer,
*                                                    uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdEcdhP256GenerateKeysOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGapCmdEcdhP256GenerateKeys
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
)
{
    ecdhPublicKey_t pubKey = {0U};
    ecdhPrivateKey_t prvKey = {0U};
    
    fsciBleGap2CallSecLibApiFunction(ECDH_P256_GenerateKeys(&pubKey, &prvKey));
    fsciBleGap2MonitorOutParams(GeneratePubPrvPair, &pubKey);
}

/*! *********************************************************************************
* \brief  fsciBleGapComputeA2BEvtMonitor out parameter monitoring macro.
*
* \param[in]    pOutKey     Pointer to the A2B key.
*
********************************************************************************** */
static void fsciBleGapComputeA2BEvtMonitor
(
    ecdhDhKey_t *pOutKey
)
{
    clientPacketStructured_t *pClientPacket;
    uint8_t *pBuffer;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if (TRUE == bFsciBleGap2Enabled)
    {
#endif /* gFsciBleTest_d */
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleGap2AllocFsciPacket((uint8_t)gBleGapEvtEcdhP256ComputeA2BKeyOpCode_c,
                                                   sizeof(ecdhDhKey_t));
        
        if (NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];
            /* Set event parameters in the buffer */
            fsciBleGetBufferFromArray(pOutKey, pBuffer, sizeof(ecdhDhKey_t));
            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
#if gFsciBleTest_d
    }
#endif /* gFsciBleTest_d */
}

/*! *********************************************************************************
* \brief  fsciSecLibExportA2BEvtMonitor out parameter monitoring macro.
*
* \param[in]    pOutKey     Pointer to the A2B blob.
*
********************************************************************************** */
static void fsciBleGapExportA2BEvtMonitor
(
    uint8_t *pOutKey
)
{
    clientPacketStructured_t *pClientPacket;
    uint8_t *pBuffer;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if (FALSE == bFsciBleGap2Enabled)
    {
#endif /* gFsciBleTest_d */
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleGap2AllocFsciPacket((uint8_t)gBleGapEvtExportA2BBlobOpCode_c,
                                                  gSecLibElkeBlobSize_c);
        
        if (NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];
            /* Set event parameters in the buffer */
            fsciBleGetBufferFromArray(pOutKey, pBuffer, gSecLibElkeBlobSize_c);
            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
#if gFsciBleTest_d
    }
#endif /* gFsciBleTest_d */
}

/*! *********************************************************************************
* \brief  fsciBleGapImportA2BEvtMonitor out parameter monitoring macro.
*
* \param[in]    keyType     Key data format.
* \param[in]    pOutKey     Pointer to the desired key data.
*
********************************************************************************** */
static void fsciBleGapImportA2BEvtMonitor
(
    uint8_t keyType,
    uint8_t *pOutKey
)
{
    clientPacketStructured_t *pClientPacket;
    uint8_t *pBuffer;
    uint8_t keyDataSize = 0U;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if (FALSE == bFsciBleGap2Enabled)
    {
#endif /* gFsciBleTest_d */
        switch (keyType)
        {
            case (uint8_t)gSecPlainText_c:
            {
                keyDataSize = 16U;
            }
            break;
            case (uint8_t)gSecElkeBlob_c:
            case (uint8_t)gSecLtkElkeBlob_c:
            {
                keyDataSize = gSecLibElkeBlobSize_c;
            }
            break;
            default:
            {
                ; /* MISRA compliance */
            }
            break;
        }
        
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleGap2AllocFsciPacket((uint8_t)gBleGapEvtImportA2BBlobOpCode_c,
                                                   (uint32_t)keyDataSize + 1U);
        
        if (NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];
            /* Set event parameters in the buffer */
            fsciBleGetBufferFromUint8Value(keyType, pBuffer);
            fsciBleGetBufferFromArray(pOutKey, pBuffer, keyDataSize);
            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
#if gFsciBleTest_d
    }
#endif /* gFsciBleTest_d */
}

/*! *********************************************************************************
* \brief  fsciBleGapGeneratePubPrvPairEvtMonitor out parameter monitoring macro.
*
* \param[in]    pOutKey     Pointer to the desired key data.
*
********************************************************************************** */
static void fsciBleGapGeneratePubPrvPairEvtMonitor
(
    ecdhPublicKey_t *pOutKey
)
{
    clientPacketStructured_t *pClientPacket;
    uint8_t *pBuffer;

#if gFsciBleTest_d
    /* If GAP is disabled the event must be not monitored */
    if (FALSE == bFsciBleGap2Enabled)
    {
#endif /* gFsciBleTest_d */
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleGap2AllocFsciPacket((uint8_t)gBleGapEvtEcdhP256GenerateKeysOpCode_c,
                                                  sizeof(ecdhPublicKey_t));
        
        if (NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];
            /* Set event parameters in the buffer */
            fsciBleGetBufferFromArray(pOutKey, pBuffer, sizeof(ecdhPublicKey_t));
            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
#if gFsciBleTest_d
    }
#endif /* gFsciBleTest_d */
}

#endif /* gA2BSupportEnabled_d */
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
/*! *********************************************************************************
*\private
*\fn           void HandleGap2StatusOpCode(uint8_t *pBuffer,
*                                         uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGap2StatusOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGap2StatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleResult_t status;

    /* Get status from buffer */
    fsciBleGetEnumValueFromBuffer(status, pBuffer, bleResult_t);

    if(gBleSuccess_c != status)
    {
        /* Clean out parameters pointers kept in FSCI */
        fsciBleGap2CleanOutParams();
    }
}
#endif /* gFsciBleHost_d */

void fsciBleGap2StatusMonitor(bleResult_t result)
{
#if gFsciBleTest_d
    /* If GAP is disabled the status must be not monitored */
    if(FALSE == bFsciBleGap2Enabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Send status over UART */
    fsciBleStatusMonitor(gFsciBleGap2OpcodeGroup_c, (uint8_t)gBleGap2StatusOpCode_c, result);
}

void fsciBleCtrlDebugInfoCmdMonitor
(
    nbuDebugInfo_t* pDebugInfo
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if(FALSE == bFsciBleGap2Enabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGap2AllocFsciPacket((uint8_t)gBleCtrlDebugInfoOpCode_c, (sizeof(nbuDebugInfo_t) + 2U));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(pDebugInfo->debugMode, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->errorCount, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->warCount, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->issueTriggered, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->length, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->sha1Nbu, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->cust.errorBitmask, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->cust.taskSOFmask, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->cust.idleTaskFreeRunningCounter, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.lmStateStatus.ucCurState, pBuffer);
    fsciBleGetBufferFromArray(pDebugInfo->cust.lmStateStatus.StatesCnt, pBuffer, NO_STATES);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.advSchedFreeRunCnt, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.scanSchedFreeRunCnt, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.scanInitSchedFreeRunCnt, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.scanCorrHitFreeRunningCounter, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.scanCorrToFreeRunningCounter, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.txLockFailRunningCounter, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.rxLockFailRunningCounter, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->cust.rtErrorStatus, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->cust.reserved4, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.reserved5, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.scanGenReportFreeRunningCounter, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.scanGenCloseFreeRunningCounter, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.res2ScanHwErrorFreeRunningCounter, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.advTxDoneFreeRunningCounter, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.res3ScanRtErrorFreeRunningCounter, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.res4AdvHwErrorFreeRunningCounter, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->cust.res5AdvRtErrorFreeRunningCounter, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->cust.hwAbortStatus, pBuffer);
    fsciBleGetBufferFromUint8Value(MAX_CUST_ERR_IN_LIST, pBuffer);
    fsciBleGetBufferFromUint8Value(MAX_ERR_IN_LIST - MAX_CUST_ERR_IN_LIST, pBuffer);
    fsciBleGetBufferFromArray(pDebugInfo->cust.errorList, pBuffer, MAX_CUST_ERR_IN_LIST);
    fsciBleGetBufferFromArray(pDebugInfo->cust.reserved1, pBuffer, MAX_ERR_IN_LIST-MAX_CUST_ERR_IN_LIST);
    fsciBleGetBufferFromArray(pDebugInfo->cust.errorInfo, pBuffer, MAX_CUST_ERR_IN_LIST * sizeof(nbuErrDebugInfo_t));
    fsciBleGetBufferFromArray(pDebugInfo->cust.warningList, pBuffer, MAX_CUST_ERR_IN_LIST);
    fsciBleGetBufferFromArray(pDebugInfo->cust.reserved2, pBuffer, MAX_ERR_IN_LIST-MAX_CUST_ERR_IN_LIST);
    fsciBleGetBufferFromArray(pDebugInfo->cust.warningInfo, pBuffer, MAX_CUST_ERR_IN_LIST * sizeof(nbuErrDebugInfo_t));
    fsciBleGetBufferFromUint32Value(pDebugInfo->reserved3, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

#if defined(gBLE60_DecisionBasedAdvertisingFilteringSupport_d) && (gBLE60_DecisionBasedAdvertisingFilteringSupport_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetExtAdvertisingDecisionDataOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetExtAdvertisingDecisionDataOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdSetExtAdvertisingDecisionDataOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bool_t      bDKIncluded;
    uint8_t      handle;
    uint8_t     aDecisionKey[gcSmpIrkSize_c];
    bool_t      bRandomPartIncluded;
    bool_t      bDecisionDataIncluded;
    uint8_t     aRandomPart[3];
    uint8_t     aDecisionData[gcDecisionDataMaxSize_c];
    uint8_t     decisionDataLength;
    gapAdvertisingDecisionData_t gapAdvertisingDecisionData = {0};

    fsciBleGetUint8ValueFromBuffer(handle, pBuffer);
    /* Get from buffer the boolean value which indicates if IRK
    is included in the request */
    fsciBleGetBoolValueFromBuffer(bDKIncluded, pBuffer);

    /* Verify if IRK is included in the request or not */
    if(TRUE == bDKIncluded)
    {
        /* Get decision key from buffer */
        fsciBleGetArrayFromBuffer(aDecisionKey, pBuffer, gcDecisionDataKeySize_c);

        gapAdvertisingDecisionData.pKey = aDecisionKey;

        /* Get from buffer the boolean value which indicates if random
        part is included in the request */
        fsciBleGetBoolValueFromBuffer(bRandomPartIncluded, pBuffer);

        /* Verify if random part is included in the request or not */
        if(TRUE == bRandomPartIncluded)
        {
            /* Get random part (3 bytes) from buffer */
            fsciBleGetArrayFromBuffer(aRandomPart, pBuffer, 3U);
            gapAdvertisingDecisionData.pPrand = aRandomPart;
        }
    }
    fsciBleGetBoolValueFromBuffer(bDecisionDataIncluded, pBuffer);
    if(TRUE == bDecisionDataIncluded)
    {
        uint8_t rTagPresentIndex = 0;
        fsciBleGetUint8ValueFromBuffer(decisionDataLength, pBuffer);
        if(decisionDataLength > gcDecisionDataMaxSize_c)
        {
            decisionDataLength = gcDecisionDataMaxSize_c;
            rTagPresentIndex = decisionDataLength - gcDecisionDataMaxSize_c;
        }
        if(decisionDataLength > 0U)
        {
          fsciBleGetArrayFromBuffer(aDecisionData, pBuffer, decisionDataLength);
          gapAdvertisingDecisionData.dataLength = decisionDataLength;
          gapAdvertisingDecisionData.pDecisionData = aDecisionData;
          pBuffer = &pBuffer[rTagPresentIndex];
          fsciBleGetBoolValueFromBuffer(gapAdvertisingDecisionData.resolvableTagPresent, pBuffer);
        }
    }
    fsciBleGap2CallApiFunction(Gap_SetExtAdvertisingDecisionData(handle, &gapAdvertisingDecisionData));
}
#endif /* gBLE60_DecisionBasedAdvertisingFilteringSupport_d */

#if defined(gBLE60_DecisionBasedAdvertisingFilteringSupport_d) && (gBLE60_DecisionBasedAdvertisingFilteringSupport_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetDecisionInstructionsOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetDecisionInstructionsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdSetDecisionInstructionsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{

    uint8_t     numTests;
    gapDecisionInstructionsData_t *pDecisionInstructions;
    fsciBleGetUint8ValueFromBuffer(numTests, pBuffer);
    pDecisionInstructions = (gapDecisionInstructionsData_t *)MEM_BufferAlloc(((uint32_t) numTests) * sizeof(gapDecisionInstructionsData_t));
    if(pDecisionInstructions == NULL)
    {
        /* No memory => the GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        uint8_t testGroup;
        uint8_t passCriteria;
        uint8_t relevantField;
        for(uint8_t i = 0; i  <numTests; i++)
        {
            fsciBleGetUint8ValueFromBuffer(testGroup, pBuffer);
            fsciBleGetUint8ValueFromBuffer(passCriteria, pBuffer);
            fsciBleGetUint8ValueFromBuffer(relevantField, pBuffer);
            pDecisionInstructions[i].testGroup = (gapDecisionInstructionsTestGroup_t)testGroup;
            pDecisionInstructions[i].passCriteria = (gapDecisionInstructionsTestPassCriteria_t)passCriteria;
            pDecisionInstructions[i].relevantField = (gapDecisionInstructionsRelevantField_t)relevantField;
            switch(pDecisionInstructions[i].relevantField )
            {
            case gDIRF_ResolvableTag_c:
                fsciBleGetArrayFromBuffer(pDecisionInstructions[i].testParameters.resolvableTagKey, pBuffer, gcDecisionDataKeySize_c);
                break;
            case gDIRF_AdvMode_c:
                fsciBleGetUint8ValueFromBuffer(pDecisionInstructions[i].testParameters.advMode, pBuffer);
                break;
            case gDIRF_RSSI_c:
                {
                    uint8_t rssiMin;
                    uint8_t rssiMax;
                    fsciBleGetUint8ValueFromBuffer(rssiMin, pBuffer);
                    fsciBleGetUint8ValueFromBuffer(rssiMax, pBuffer);
                    pDecisionInstructions[i].testParameters.rssi.min = (int8_t)rssiMin;
                    pDecisionInstructions[i].testParameters.rssi.max = (int8_t)rssiMax;
                }
                break;
            case gDIRF_PathLoss_c:
                fsciBleGetUint8ValueFromBuffer(pDecisionInstructions[i].testParameters.pathLoss.min, pBuffer);
                fsciBleGetUint8ValueFromBuffer(pDecisionInstructions[i].testParameters.pathLoss.max, pBuffer);
                break;
            case gDIRF_AdvAddress_c:
                {
                    uint8_t check;
                    fsciBleGetUint8ValueFromBuffer(check, pBuffer);
                    pDecisionInstructions[i].testParameters.advA.check = (gapDecisionInstructionsAdvAChecks_t)check;
                    if(pDecisionInstructions[i].testParameters.advA.check >= gDIAAC_AdvAmatchAddress1_c)
                    {
                        fsciBleGetUint8ValueFromBuffer(pDecisionInstructions[i].testParameters.advA.address1Type, pBuffer);
                        fsciBleGetArrayFromBuffer(pDecisionInstructions[i].testParameters.advA.address1, pBuffer, gcBleDeviceAddressSize_c);
                    }
                    if(pDecisionInstructions[i].testParameters.advA.check == gDIAAC_AdvAmatchAddress1orAddress2_c)
                    {
                        fsciBleGetUint8ValueFromBuffer(pDecisionInstructions[i].testParameters.advA.address2Type, pBuffer);
                        fsciBleGetArrayFromBuffer(pDecisionInstructions[i].testParameters.advA.address2, pBuffer, gcBleDeviceAddressSize_c);
                    }
                }
                break;
            default:
                fsciBleGetArrayFromBuffer(pDecisionInstructions[i].testParameters.arbitraryData.mask, pBuffer, gcDecisionInstructionsArbitraryDataMaskSize_c);
                fsciBleGetArrayFromBuffer(pDecisionInstructions[i].testParameters.arbitraryData.target, pBuffer, gcDecisionInstructionsArbitraryDataTargetSize_c);
                break;
            }
        }
        fsciBleGap2CallApiFunction(Gap_SetDecisionInstructions(numTests, pDecisionInstructions));
        /* Free buffer allocated*/
        (void)MEM_BufferFree(pDecisionInstructions);

    }
}
#endif /* gBLE60_DecisionBasedAdvertisingFilteringSupport_d */

#if defined(gBLE54_d) && (gBLE54_d == 1U)
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetExtAdvertisingParametersV2OpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetExtAdvertisingParametersV2OpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdSetExtAdvertisingParametersV2OpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapExtAdvertisingParametersV2_t advertisingParameters = {0};
    union
    {
        uint8_t fsciPhyOption;
        gapLePhyOptionsFlags_t gapPhyOption;
    }advPhyOptions = {0U};

    /* Get advertising parameters from buffer */
    /* Read gapExtAdvertisingParametersV2_t fields from buffer */
    fsciBleGetUint8ValueFromBuffer( advertisingParameters.SID,                       pBuffer);
    fsciBleGetUint8ValueFromBuffer( advertisingParameters.handle,                    pBuffer);
    fsciBleGetUint32ValueFromBuffer(advertisingParameters.minInterval,               pBuffer);
    fsciBleGetUint32ValueFromBuffer(advertisingParameters.maxInterval,               pBuffer);
    fsciBleGetEnumValueFromBuffer(  advertisingParameters.ownAddressType,            pBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(    advertisingParameters.ownRandomAddr,             pBuffer);
    fsciBleGetEnumValueFromBuffer(  advertisingParameters.peerAddressType,           pBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(    advertisingParameters.peerAddress,               pBuffer);
    fsciBleGetEnumValueFromBuffer(  advertisingParameters.channelMap,                pBuffer, gapAdvertisingChannelMapFlags_t);
    fsciBleGetEnumValueFromBuffer(  advertisingParameters.filterPolicy,              pBuffer, gapAdvertisingFilterPolicy_t);
    fsciBleGetEnumValueFromBuffer(  advertisingParameters.extAdvProperties,          pBuffer, bleAdvRequestProperties_t);
    fsciBleGetUint8ValueFromBufferSigned( advertisingParameters.txPower,             pBuffer);
    fsciBleGetEnumValueFromBuffer(  advertisingParameters.primaryPHY,                pBuffer, gapLePhyMode_t);
    fsciBleGetEnumValueFromBuffer(  advertisingParameters.secondaryPHY,              pBuffer, gapLePhyMode_t);
    fsciBleGetUint8ValueFromBuffer( advertisingParameters.secondaryAdvMaxSkip,       pBuffer);
    fsciBleGetBoolValueFromBuffer(  advertisingParameters.enableScanReqNotification, pBuffer);
    fsciBleGetUint8ValueFromBuffer( advPhyOptions.fsciPhyOption,                     pBuffer);
    advertisingParameters.primaryAdvPhyOptions = advPhyOptions.gapPhyOption;
    fsciBleGetUint8ValueFromBuffer( advPhyOptions.fsciPhyOption,                     pBuffer);
    advertisingParameters.secondaryAdvPhyOptions = advPhyOptions.gapPhyOption;

    fsciBleGap2CallApiFunction(Gap_SetExtAdvertisingParametersV2(&advertisingParameters));
}
#endif

void fsciBleCtrlDebugInfo2CmdMonitor
(
    nbuDebugInfo2_t* pDebugInfo
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

#if gFsciBleTest_d
    /* If GAP is disabled or if the command was initiated by FSCI it must be not monitored */
    if(FALSE == bFsciBleGap2Enabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGap2AllocFsciPacket((uint8_t)gBleCtrlDebugInfo2OpCode_c, (sizeof(nbuDebugInfo2_t)));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(pDebugInfo->debugMode, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->errorCount, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->warCount, pBuffer);
    fsciBleGetBufferFromUint8Value(pDebugInfo->issueTriggered, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->length, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->sha1Nbu, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->errorBitmask, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->taskSOFmask, pBuffer);
    fsciBleGetBufferFromUint32Value(pDebugInfo->idleTaskFreeRunningCounter, pBuffer);
    fsciBleGetBufferFromArray(pDebugInfo->bufferInfo, pBuffer, sizeof(pDebugInfo->bufferInfo));

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

#endif /* gFsciBleGap2LayerEnabled_d */
/*! *********************************************************************************
* @}
********************************************************************************** */
