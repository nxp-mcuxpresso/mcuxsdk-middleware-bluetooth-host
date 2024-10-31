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

static void HandleGapCmdLeChannelOverride
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);

static void HandleGapCmdEncryptAdvertisingData
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);

static void HandleGapCmdDecryptAdvertisingData
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);

static void fsciBleGapEncryptAdvertisingDataEvtMonitor
(
    uint8_t *pOutput,
    uint16_t pOutputSize
);

static void fsciBleGapDecryptAdvertisingDataEvtMonitor
(
    uint8_t *pOutput,
    uint16_t pOutputSize
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
#if (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE)
static void HandleGapEvtScanningEventPeriodicDeviceScannedV2OpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
static void HandleGapEvtAdvertisingEventPerAdvSubeventDataRequestOpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
static void HandleGapEvtAdvertisingEventPerAdvResponseOpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
#endif /* (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE) */
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
#if (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE)
void HandleGapCmdSetPeriodicAdvertisingSubeventDataOpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetPeriodicAdvertisingResponseDataOpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetPeriodicSyncSubeventOpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
void HandleGapCmdConnectV2OpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
void HandleGapCmdSetPeriodicAdvParametersV2OpCode
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
);
#endif /* (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE) */
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
    NULL,                                                                       /* = 0x05 Not Used / Free to use */
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
    HandleGapCmdLeChannelOverride,                                              /* = 0x0B gBleGapCmdLeChannelOverrideOpCode_c */
    HandleGapCmdEncryptAdvertisingData,                                         /* = 0x0C gBleGapCmdEncryptAdvertisingDataOpCode_c */
    HandleGapCmdDecryptAdvertisingData,                                         /* = 0x0D gBleGapCmdDecryptAdvertisingDataOpCode_c */
#if (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE)
    HandleGapCmdSetPeriodicAdvertisingSubeventDataOpCode,                       /* = 0x0E, gBleGapCmdSetPeriodicAdvertisingSubeventDataOpCode_c */
    HandleGapCmdSetPeriodicAdvertisingResponseDataOpCode,                       /* = 0x0F, gBleGapCmdSetPeriodicAdvertisingResponseDataOpCode_c */
    HandleGapCmdSetPeriodicSyncSubeventOpCode,                                  /* = 0x10, gBleGapCmdSetPeriodicSyncSubeventOpCode_c */
    HandleGapCmdConnectV2OpCode,                                                /* = 0x11, gBleGapCmdConnectV2OpCode_c */
    HandleGapCmdSetPeriodicAdvParametersV2OpCode,                               /* = 0x12, gBleGapCmdSetPeriodicAdvParametersV2OpCode_c */
#else /* (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE) */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif /* (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE) */
};

#if gFsciBleTest_d
/* Indicates if FSCI for GAP is enabled or not */
static bool_t bFsciBleGap2Enabled            = FALSE;
#endif /* gFsciBleTest_d */

#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
const pfGap2OpCodeHandler_t maGap2EvtOpCodeHandlers[]=
{
    HandleGap2StatusOpCode,                                                     /* = 0x80, gBleGap2StatusOpCode_c */
    NULL,                                                                       /* reserved: 0x81 */
    NULL,                                                                       /* reserved: 0x82 */
    NULL,                                                                       /* reserved: 0x83 */
    NULL,                                                                       /* reserved: 0x84 */
    NULL,                                                                       /* reserved: 0x85 */
    NULL,                                                                       /* reserved: 0x86 */
    NULL,                                                                       /* reserved: 0x87 */
    NULL,                                                                       /* reserved: 0x88 */
    NULL,                                                                       /* reserved: 0x8A */
    NULL,                                                                       /* reserved: 0x8B */
    NULL,                                                                       /* reserved: 0x8C */
#if (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE)
    HandleGapEvtScanningEventPeriodicDeviceScannedV2OpCode,                     /* = 0x8D, gBleGapEvtScanningEventPeriodicDeviceScannedV2OpCode_c */
    HandleGapEvtAdvertisingEventPerAdvSubeventDataRequestOpCode,                /* = 0x8E, gBleGapEvtAdvertisingEventPerAdvSubeventDataRequestOpCode_c */
    HandleGapEvtAdvertisingEventPerAdvResponseOpCode,                           /* = 0x8F, gBleGapEvtAdvertisingEventPerAdvResponseOpCode_c */
#else /* (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE) */
    NULL,                                                                       /* reserved: 0x8D */
    NULL,                                                                       /* reserved: 0x8E */
    NULL,                                                                       /* reserved: 0x8F */
#endif /* (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE) */
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

#if gFsciIncluded_c && gFsciBleGapLayerEnabled_d
/*! Handler functions for fsciBleGapGetGenericEventBufferSize */
#if (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE)
/*! *********************************************************************************
*\fn           uint32_t GetPeriodicAdvSetResponseDataCompleteBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gPeriodicAdvSetResponseDataComplete_c and the
*              gPeriodicSyncSubeventComplete_c events.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
uint32_t GetPeriodicAdvSetResponseDataCompleteBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint16_t);
}

/*! *********************************************************************************
*\fn           uint32_t GetPeriodicAdvSetSubeventDataCompleteBufferSize(
*                                           gapGenericEvent_t    *pGenericEvent)
*
*\brief        Returns the required FSCI buffer size for the
*              gPeriodicAdvSetSubeventDataComplete_c event.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*
*\return       uint32_t            Buffer size.
********************************************************************************** */
uint32_t GetPeriodicAdvSetSubeventDataCompleteBufferSize
(
    gapGenericEvent_t   *pGenericEvent
)
{
    return sizeof(uint8_t);
}

/*! Handler functions for fsciBleGapGetBufferFromGenericEvent */
/*! *********************************************************************************
*\fn           void GetBufferFromPeriodicAdvSetResponseDataCompleteEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the pawrSyncHandle data fields in the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
void GetBufferFromPeriodicAdvSetResponseDataCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint16Value((uint8_t)pGenericEvent->eventData.pawrSyncHandle, *ppBuffer);
}

/*! *********************************************************************************
*\fn           void GetBufferFromPeriodicAdvSetSubeventDataCompleteEvent(
*                                           gapGenericEvent_t    *pGenericEvent,
*                                           uint8_t              **ppBuffer)
*
*\brief        Writes the pawrAdvHandle data fields in the provided
*              buffer.
*
*\param  [in]  pGenericEvent       Pointer to the generic event.
*\param  [in]  ppBuffer            Pointer to the buffer where the data fields
*                                  should be written.
*
*\retval       void.
********************************************************************************** */
void GetBufferFromPeriodicAdvSetSubeventDataCompleteEvent
(
    gapGenericEvent_t   *pGenericEvent,
    uint8_t             **ppBuffer
)
{
    fsciBleGetBufferFromUint8Value((uint8_t)pGenericEvent->eventData.pawrAdvHandle, *ppBuffer);
}
#endif /* (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE) */
#endif /* gFsciIncluded_c && gFsciBleGapLayerEnabled_d */

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
    uint8_t* payload = NULL;
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
    uint32_t debugInfoAddress = 0U;
    uint32_t debugInfoSize = 0U;
    uint8_t *pDebugInfo = NULL;

    /* Get SMU debug information size */
    fsciBleGetUint32ValueFromBuffer(debugInfoSize, pBuffer);
    /* Get SMU debug information address */
    fsciBleGetUint32ValueFromBuffer(debugInfoAddress, pBuffer);

    pDebugInfo = MEM_BufferAlloc(debugInfoSize);

    if (NULL != pDebugInfo)
    {
        /* Request Radio domain to be active for safe access to shared memory */
        PLATFORM_RemoteActiveReq();

        /* Copy NBU debug data */
        FLib_MemCpy((void*)pDebugInfo, (void*)debugInfoAddress, debugInfoSize);
        /* Release Radio Domain */
        PLATFORM_RemoteActiveRel();

        /* Command contains no parameters - Read the debug data and trigger an event with the response */
        fsciBleGap2StatusMonitor(status);
    }
    else
    {
        status = gBleOutOfMemory_c;
    }

    if (status == gBleSuccess_c)
    {
        fsciBleCtrlDebugInfoCmdMonitor(debugInfoSize, pDebugInfo);

        (void)MEM_BufferFree(pDebugInfo);
    }
    else
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdLeChannelOverride(uint8_t *pBuffer,
*                                                 uint32_t fsciInterfaceId)
*\brief        Handler for gBleGapCmdLeChannelOverrideOpCode_c.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGapCmdLeChannelOverride
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
)
{
    bleChannelOverrideMode_t mode;
    uint8_t channelListLength;
    uint8_t aChannelList[6U];

    /* Get command parameters from buffer */
    fsciBleGetEnumValueFromBuffer(mode, pBuffer, bleChannelOverrideMode_t);
    fsciBleGetUint8ValueFromBuffer(channelListLength, pBuffer);
    fsciBleGetArrayFromBuffer(aChannelList, pBuffer, channelListLength);

    fsciBleGap2CallApiFunction(Gap_LeChannelOverride(mode, channelListLength, aChannelList));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdEncryptAdvertisingData(uint8_t *pBuffer,
*                                                 uint32_t fsciInterfaceId)
*\brief        Handler for gBleGapCmdEncryptAdvertisingDataOpCode_c.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGapCmdEncryptAdvertisingData
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
)
{
    gapAdvertisingData_t* pAdvertisingData    = NULL;
    uint16_t              advDataSize         = 0U;
    uint8_t*              pOutput             = NULL;
    uint8_t               key[gcEadKeySize_c] = {0U};
    uint8_t               iv[gcEadIvSize_c]   = {0U};

    pAdvertisingData = fsciBleGapAllocAdvertisingDataForBuffer(pBuffer);

    if (pAdvertisingData != NULL)
    {
        fsciBleGapGetAdvertisingDataFromBuffer(pAdvertisingData, &pBuffer);
        advDataSize = fsciBleGapGetAdvertisingDataBufferSize(pAdvertisingData) - 1U;
        pOutput = MEM_BufferAlloc(advDataSize + gcEadRandomizerSize_c + gcEadMicSize_c);

        if (pOutput != NULL)
        {
            fsciBleGetArrayFromBuffer(key, pBuffer, gcEadKeySize_c);
            fsciBleGetArrayFromBuffer(iv, pBuffer, gcEadIvSize_c);
            fsciBleGap2CallApiFunction(Gap_EncryptAdvertisingData(pAdvertisingData, key, iv, pOutput));
            fsciBleGap2MonitorOutParams(EncryptAdvertisingData, pOutput, (advDataSize + gcEadRandomizerSize_c + gcEadMicSize_c));

            (void)MEM_BufferFree(pOutput);
        }
        else
        {
            fsciBleError(gFsciOutOfMessages_c, fsciBleInterfaceId);
        }
        (void)MEM_BufferFree(pAdvertisingData);
    }
    else
    {
        fsciBleError(gFsciOutOfMessages_c, fsciBleInterfaceId);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdDecryptAdvertisingData(uint8_t *pBuffer,
*                                                 uint32_t fsciInterfaceId)
*\brief        Handler for gBleGapCmdDecryptAdvertisingDataOpCode_c.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGapCmdDecryptAdvertisingData
(
    uint8_t *pBuffer,
    uint32_t fsciInterfaceId
)
{
    uint8_t*              pInput              = NULL;
    uint8_t*              pOutput             = NULL;
    uint16_t              inputSize           = 0U;
    uint8_t               key[gcEadKeySize_c] = {0U};
    uint8_t               iv[gcEadIvSize_c]   = {0U};

    fsciBleGetUint16ValueFromBuffer(inputSize, pBuffer);

    if (inputSize > (gcEadMicSize_c + gcEadRandomizerSize_c))
    {
        pInput = MEM_BufferAlloc(inputSize);
        if (pInput != NULL)
        {
            fsciBleGetArrayFromBuffer(pInput, pBuffer, inputSize);

            pOutput = MEM_BufferAlloc(inputSize - gcEadMicSize_c - gcEadRandomizerSize_c);

            if (pOutput != NULL)
            {
                fsciBleGetArrayFromBuffer(key, pBuffer, gcEadKeySize_c);
                fsciBleGetArrayFromBuffer(iv, pBuffer, gcEadIvSize_c);
                fsciBleGap2CallApiFunction(Gap_DecryptAdvertisingData(pInput, inputSize, key, iv, pOutput));
                fsciBleGap2MonitorOutParams(DecryptAdvertisingData, pOutput, (inputSize - gcEadMicSize_c - gcEadRandomizerSize_c));

                (void)MEM_BufferFree(pOutput);
            }
            else
            {
                fsciBleError(gFsciOutOfMessages_c, fsciBleInterfaceId);
            }
            (void)MEM_BufferFree(pInput);
        }
        else
        {
            fsciBleError(gFsciOutOfMessages_c, fsciBleInterfaceId);
        }
    }
    else
    {
        fsciBleError(gFsciError_c, fsciBleInterfaceId);
    }
}

/*! *********************************************************************************
* \brief  fsciBleGapEncryptAdvertisingDataMonitor out parameter monitoring macro.
*
* \param[in]    pOutput     Encrypted output.
* \param[in]    pOutputSize Encrypted output size.
*
********************************************************************************** */
static void fsciBleGapEncryptAdvertisingDataEvtMonitor
(
    uint8_t *pOutput,
    uint16_t outputSize
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
        pClientPacket = fsciBleGap2AllocFsciPacket((uint8_t)gBleGapEvtEncryptAdvertisingDataOpCode_c,
                                                   outputSize + sizeof(uint16_t));

        if (NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];
            /* Set event parameters in the buffer */
            fsciBleGetBufferFromUint16Value(outputSize, pBuffer);
            fsciBleGetBufferFromArray(pOutput, pBuffer, outputSize);
            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
#if gFsciBleTest_d
    }
#endif /* gFsciBleTest_d */
}

/*! *********************************************************************************
* \brief  fsciBleGapDecryptAdvertisingDataMonitor out parameter monitoring macro.
*
* \param[in]    pOutput     Decrypted output
* \param[in]    pOutputSize Decrypted output size.
*
********************************************************************************** */
static void fsciBleGapDecryptAdvertisingDataEvtMonitor
(
    uint8_t *pOutput,
    uint16_t outputSize
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
        pClientPacket = fsciBleGap2AllocFsciPacket((uint8_t)gBleGapEvtDecryptAdvertisingDataOpCode_c,
                                                   outputSize + sizeof(uint16_t));

        if (NULL != pClientPacket)
        {
            pBuffer = &pClientPacket->payload[0];
            /* Set event parameters in the buffer */
            fsciBleGetBufferFromUint16Value(outputSize, pBuffer);
            fsciBleGetBufferFromArray(pOutput, pBuffer, outputSize);
            /* Transmit the packet over UART */
            fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
        }
#if gFsciBleTest_d
    }
#endif /* gFsciBleTest_d */
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

#if (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtScanningEventPeriodicDeviceScannedV2OpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtScanningEventPeriodicDeviceScannedV2OpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtScanningEventPeriodicDeviceScannedV2OpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapScanningEvent_t*     pScanningEvent = NULL;
    gapScanningEventType_t  eventType = gScanStateChanged_c;
    pScanningEvent = fsciBleGapAllocScanningEventForBuffer(eventType, pBuffer);

    if(NULL == pScanningEvent)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        fsciBleGapGetScanningEventFromBuffer(pScanningEvent, &pBuffer);

        fsciBleGapCallbacks.scanningCallback(pScanningEvent);

        fsciBleGapFreeScanningEvent(pScanningEvent);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtAdvertisingEventPerAdvSubeventDataRequestOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtAdvertisingEventPerAdvSubeventDataRequestOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtAdvertisingEventPerAdvSubeventDataRequestOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapAdvertisingEvent_t advertisingEvent = {0};
    advertisingEvent.eventType = gPerAdvSubeventDataRequest_c;

    fsciBleGapGetAdvertisingEventFromBuffer(&advertisingEvent, &pBuffer);
    fsciBleGapCallbacks.advertisingCallback(&advertisingEvent);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapEvtAdvertisingEventPerAdvResponseOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapEvtAdvertisingEventPerAdvResponseOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */   
void HandleGapEvtAdvertisingEventPerAdvResponseOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapAdvertisingEvent_t advertisingEvent = {0};
    advertisingEvent.eventType = gPerAdvResponse_c;

    fsciBleGapGetAdvertisingEventFromBuffer(&advertisingEvent, &pBuffer);
    fsciBleGapCallbacks.advertisingCallback(&advertisingEvent);
}
#endif /* (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE) */
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
    uint32_t    debugInfoSize,
    uint8_t     *pDebugInfo
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
    pClientPacket = fsciBleGap2AllocFsciPacket((uint8_t)gBleCtrlDebugInfoOpCode_c, sizeof(debugInfoSize) + debugInfoSize); /* sizeof debugInfoSize + 
                                                                                                                            * actual debugInfo */

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint32Value(debugInfoSize, pBuffer);
    fsciBleGetBufferFromArray(pDebugInfo, pBuffer, debugInfoSize);

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
#if (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetPeriodicAdvertisingSubeventDataOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetPeriodicAdvertisingSubeventDataOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdSetPeriodicAdvertisingSubeventDataOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t advHandle = 0U;
    gapPeriodicAdvertisingSubeventData_t advertisingSubeventData = {0};
    gapSubeventDataStructure_t *pSubeventDataStructure = NULL;
    gapAdvertisingData_t *pAdvertisingData = NULL;
    uint8_t idx = 0U;
    uint8_t j = 0U;
    bleResult_t status = gBleSuccess_c;

    /* Get periodic advertising subevent data from buffer */
    /* Read gapPeriodicAdvertisingSubeventData_t fields from buffer */
    fsciBleGetUint8ValueFromBuffer(advHandle,                                   pBuffer);
    fsciBleGetUint8ValueFromBuffer(advertisingSubeventData.cNumSubevents,       pBuffer);
    
    /* Allocate buffer for each gapSubeventDataStructure_t structure */
    advertisingSubeventData.aSubeventDataStructures = 
        (gapSubeventDataStructure_t*)MEM_BufferAlloc(sizeof(gapSubeventDataStructure_t) *
                                                     advertisingSubeventData.cNumSubevents);
    
    if(NULL == advertisingSubeventData.aSubeventDataStructures)
    {
        /* No memory */
        status = gBleOverflow_c;
    }
    else
    {
        for(idx = 0; idx < advertisingSubeventData.cNumSubevents; idx++)
        {
            /* if one MEM_BufferAlloc fails, the advertisingSubeventData.aSubeventDataStructures is freed */
            if(gBleSuccess_c == status)
            {
                pSubeventDataStructure = &advertisingSubeventData.aSubeventDataStructures[idx];

                fsciBleGetUint8ValueFromBuffer(pSubeventDataStructure->subevent,            pBuffer);
                fsciBleGetUint8ValueFromBuffer(pSubeventDataStructure->responseSlotStart,   pBuffer);
                fsciBleGetUint8ValueFromBuffer(pSubeventDataStructure->responseSlotCount,   pBuffer);

                /* Allocate buffer for the advertising data */
                pAdvertisingData = fsciBleGapAllocAdvertisingDataForBuffer(pBuffer);

                if(NULL == pAdvertisingData)
                {
                    /* No memory */
                    status = gBleOverflow_c;

                    /* failed to alloc pAdvertisingData at step idx, free all previous allocated */
                    for (j = 0U; j < idx ; j++)
                    {
                        (void)fsciBleGapFreeAdvertisingData(advertisingSubeventData.aSubeventDataStructures[j].pAdvertisingData);
                    }
                    
                    /* failed to alloc pAdvertisingData, but advertisingSubeventData.aSubeventDataStructures is allocated */
                    (void)MEM_BufferFree(advertisingSubeventData.aSubeventDataStructures);
                }
                else
                {
                    /* Get the advertising data from buffer */
                    fsciBleGapGetAdvertisingDataFromBuffer(pAdvertisingData, &pBuffer);
                    
                    pSubeventDataStructure->pAdvertisingData = pAdvertisingData;
                }
            }
        }
    }

    if(gBleSuccess_c == status)
    {
        fsciBleGap2CallApiFunction(Gap_SetPeriodicAdvSubeventData(advHandle, &advertisingSubeventData));

        /* free all MEM_BufferAlloc allocated memory */
        for(idx = 0; idx < advertisingSubeventData.cNumSubevents; idx++)
        {
            (void)fsciBleGapFreeAdvertisingData(advertisingSubeventData.aSubeventDataStructures[idx].pAdvertisingData);
        }
        (void)MEM_BufferFree(advertisingSubeventData.aSubeventDataStructures);
    }
    else
    {
        /* The GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetPeriodicAdvertisingResponseDataOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetPeriodicAdvertisingResponseDataOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdSetPeriodicAdvertisingResponseDataOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t handle = 0U;
    gapPeriodicAdvertisingResponseData_t advertisingResponseData = {0};
    gapAdvertisingData_t *pAdvertisingData = NULL;
    bleResult_t status = gBleSuccess_c;

    /* Get periodic advertising response data from buffer */
    /* Read gapPeriodicAdvertisingResponseData_t fields from buffer */
    fsciBleGetUint16ValueFromBuffer(handle,                                     pBuffer);
    fsciBleGetUint16ValueFromBuffer(advertisingResponseData.requestEvent,       pBuffer);
    fsciBleGetUint8ValueFromBuffer(advertisingResponseData.requestSubevent,     pBuffer);
    fsciBleGetUint8ValueFromBuffer(advertisingResponseData.responseSubevent,    pBuffer);
    fsciBleGetUint8ValueFromBuffer(advertisingResponseData.responseSlot,        pBuffer);

    /* Allocate buffer for the advertising data */
    pAdvertisingData = fsciBleGapAllocAdvertisingDataForBuffer(pBuffer);

    if(NULL == pAdvertisingData)
    {
        /* No memory */
        status = gBleOverflow_c;
    }
    else
    {
        /* Get the advertising data from buffer */
        fsciBleGapGetAdvertisingDataFromBuffer(pAdvertisingData, &pBuffer);
        
        advertisingResponseData.pResponseData = pAdvertisingData;
    }

    if(gBleSuccess_c == status)
    {
        fsciBleGap2CallApiFunction(Gap_SetPeriodicAdvResponseData(handle, &advertisingResponseData));

        /* Free the buffer allocated for advertising response data */
        (void)fsciBleGapFreeAdvertisingData(pAdvertisingData);
    }
    else
    {
        /* The GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetPeriodicSyncSubeventOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetPeriodicSyncSubeventOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdSetPeriodicSyncSubeventOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t handle = 0U;
    uint16_t perAdvProperties = 0U;
    uint8_t  numSubevents = 0U;
    gapPeriodicSyncSubeventParameters_t *pPerSyncSubeventParams = NULL;
    bleResult_t status = gBleSuccess_c;
    
    /* Read gapPeriodicAdvertisingResponseData_t fields from buffer */
    fsciBleGetUint16ValueFromBuffer(handle,             pBuffer);
    fsciBleGetUint16ValueFromBuffer(perAdvProperties,   pBuffer);
    fsciBleGetUint8ValueFromBuffer(numSubevents,        pBuffer);

    pPerSyncSubeventParams = (gapPeriodicSyncSubeventParameters_t*)MEM_BufferAlloc(sizeof(gapPeriodicSyncSubeventParameters_t) +    /* gapPeriodicSyncSubeventParameters_t size */
                                                                                   (sizeof(uint8_t) * numSubevents) -               /* buffer for the uint8_t aSubevents */
                                                                                   sizeof(uint8_t));                                /* substract the uint8_t aSubevents[1]; */

    if(NULL == pPerSyncSubeventParams)
    {
        /* No memory */
        status = gBleOverflow_c;
    }
    else
    {
        pPerSyncSubeventParams->perAdvProperties = perAdvProperties;
        pPerSyncSubeventParams->numSubevents = numSubevents;

        fsciBleGetArrayFromBuffer(pPerSyncSubeventParams->aSubevents , pBuffer, numSubevents);
    }

    
    if(gBleSuccess_c == status)
    {
        fsciBleGap2CallApiFunction(Gap_SetPeriodicSyncSubevent(handle, pPerSyncSubeventParams));

        /* free all MEM_BufferAlloc allocated memory */
        (void)MEM_BufferFree(pPerSyncSubeventParams);
    }
    else
    {
        /* The GAP command can not be executed */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdConnectV2OpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdConnectV2OpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdConnectV2OpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapConnectionFromPawrParameters_t connectionRequestParameters = {0};

    /* Get connection request parameters from buffer */
    fsciBleGapGetConnectionRequestParametersV2FromBuffer(&connectionRequestParameters, &pBuffer);

    fsciBleGap2CallApiFunction(Gap_ConnectFromPawr(&connectionRequestParameters, fsciBleGapCallbacks.connectionCallback));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGapCmdSetPeriodicAdvParametersV2OpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGapCmdSetPeriodicAdvParametersV2OpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
void HandleGapCmdSetPeriodicAdvParametersV2OpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gapPeriodicAdvParametersV2_t advertisingParameters = {0};

    /* Get advertising parameters from buffer */
    fsciBleGapGetPeriodicAdvParametersV2FromBuffer(&advertisingParameters, &pBuffer);

    fsciBleGap2CallApiFunction(Gap_SetPeriodicAdvParametersV2(&advertisingParameters));
}
#endif /* (defined gBLE54_PawrSupport_d) && (gBLE54_PawrSupport_d == TRUE) */
#endif /* defined(gBLE54_d) && (gBLE54_d == 1U) */

#endif /* gFsciBleGap2LayerEnabled_d */
/*! *********************************************************************************
* @}
********************************************************************************** */
