/*! *********************************************************************************
* \addtogroup FSCI_BLE
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022 - 2024 NXP
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
#include "fsci_channel_sounding.h"
#include "channel_sounding.h"
#include "fwk_mem_manager.h"

#if gFsciIncluded_c && gFsciCSLayerEnabled_d
#if defined(gBLE_ChannelSounding_d) && (gBLE_ChannelSounding_d==TRUE)
/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

#if gFsciBleTest_d
#define fsciCSCallApiFunction(apiFunction)          bleResult_t result = (apiFunction); \
                                                    fsciCSStatusMonitor(result)

#define fsciCSCallApiFunctionVoid(apiFunction)      (void)apiFunction

#endif /* gFsciBleTest_d */

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
static bleResult_t csHciHostToControllerInterface
(
    hciPacketType_t packetType,
    void* pPacket,
    uint16_t packetSize
);

/* LE Meta events monitor functions */
static void fsciCSLeMetaErrorStatusMonitor(csErrorEvent_t* errorEvent);
static void fsciCSReadRemoteSupportedCapabilitiesEvtMonitor(csReadRemoteSupportedCapabilitiesCompleteEvent_t* pEvent);
static void fsciCSReadRemoteFAETableEvtMonitor(csReadRemoteFAETableCompleteEvent_t* pEvent);
static void fsciCSSecurityEnableCompleteEvtMonitor(csSecurityEnableCompleteEvent_t* pEvent);
static void fsciCSConfigCompleteEvtMonitor(csConfigCompleteEvent_t* pEvent);
static void fsciCSProcedureEnableCompleteEvtMonitor(csProcedureEnableCompleteEvent_t* pEvent);
static void fsciCSEventResultEvtMonitor(csSubeventResultEvent_t* pEvent);
static void fsciCSEventResultContinueEvtMonitor(csSubeventResultContinueEvent_t* pEvent);

/* Command complete events monitor functions */
static void fsciCSReadLocalSupportedCapabilitiesEvtMonitor(csReadLocalSupportedCapabilities_t* pEvent);
static void fsciCSEventResultDebugEvtMonitor(csEventResultDebugEvent_t* pEvent);
static void fsciCSNoParamsEventMonitor(fsciCSOpCode_t opCode);
static void fsciCSErrorStatusMonitor(csCommandError_t* pErrorEvent);

/* Helper functions */
static void fsciCSStatusMonitor(bleResult_t result);
static void fsciCSNoParamCmdMonitor(fsciCSOpCode_t opCode);
static void fsciCSCmdStatusMonitor(csCommandStatusEvent_t* pPacket);
static void fsciCSCmdCompleteDeviceIdMonitor
(
    deviceId_t deviceId,
    fsciCSOpCode_t opCode
);

/*  Unpack parameters from the received buffer */
static void fsciCSTestFromBuffer
(
    gCsTestCommandParams_t* pTestCmdParams,
    uint8_t**               ppBuffer
);

static void fsciCSWriteCachedRemoteSupportedCapabilitiesFromBuffer
(
    csReadRemoteSupportedCapabilitiesCompleteEvent_t* pCmdParams,
    uint8_t**               ppBuffer
);

static void fsciCSReadRemoteSupportedCapabilitiesFromBuffer
(
    csReadRemoteSupportedCapabilitiesCompleteEvent_t* pEvent,
    uint8_t**                                         ppBuffer
);

static void fsciCSReadRemoteFAETableFromBuffer
(
    csReadRemoteFAETableCompleteEvent_t* pEvent,
    uint8_t**                            ppBuffer
);

static void fsciCSSecurityEnableCompleteFromBuffer
(
    csSecurityEnableCompleteEvent_t* pEvent,
    uint8_t**                        ppBuffer
);

static void fsciCSConfigCompleteFromBuffer
(
    csConfigCompleteEvent_t* pEvent,
    uint8_t**                ppBuffer
);

static void fsciCSProcedureEnableCompleteFromBuffer
(
    csProcedureEnableCompleteEvent_t* pEvent,
    uint8_t**                         ppBuffer
);

static void fsciCSEventResultFromBuffer
(
    csSubeventResultEvent_t* pEvent,
    uint8_t**             ppBuffer
);

static void fsciCSEventResultContinueFromBuffer
(
    csSubeventResultContinueEvent_t* pEvent,
    uint8_t**                     ppBuffer
);

static void fsciCSErrorEvtFromBuffer
(
    csErrorEvent_t* pEvent,
    uint8_t**       ppBuffer
);

static void fsciCSCreateConfigFromBuffer
(
    gCsCreateConfigCommandParams_t*    pCreateConfigParams,
    uint8_t**                          ppBuffer
);

static void fsciCSSetProcedureParametersFromBuffer
(
    gCsSetProcedureParamsCommandParams_t* pProcedurParams,
    uint8_t**                             ppBuffer
);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
#if gFsciBleTest_d
    /* Indicates if FSCI for CS is enabled or not */
    static bool_t bFsciCSEnabled          	= FALSE;

    /* LE Meta application event callback */
    static csMetaEventCallback_t pCsMetaEventCallback = NULL;
#endif /* gFsciBleTest_d */


/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
void fsciRegisterCSMetaEventCallback(csMetaEventCallback_t callback)
{
    pCsMetaEventCallback = callback;
}

void fsciCSHandler(void* pData, void* param, uint32_t fsciBleInterfaceId)
{
    clientPacket_t* pClientPacket   = (clientPacket_t*)pData;
    uint8_t*        pBuffer         = &pClientPacket->structured.payload[0];
#if gFsciBleTest_d

    /* Verify if the command is Mode Select */
    if(gCSModeSelectOpCode_c == pClientPacket->structured.header.opCode)
    {
        fsciBleGetBoolValueFromBuffer(bFsciCSEnabled, pBuffer);
        /* Return status */
        fsciCSStatusMonitor(gBleSuccess_c);
    }
    else
    {
        /* Verify if FSCI is enabled for this layer */
        if(TRUE == bFsciCSEnabled)
        {
#endif /* gFsciBleTest_d */
            /* Select the L2CAP function to be called (using the FSCI opcode) */
            switch(pClientPacket->structured.header.opCode)
            {
#if gFsciBleBBox_d || gFsciBleTest_d
                case gCSCmdInitOpCode_c:
                    {
                        fsciCSCallApiFunctionVoid(CS_Init(csHciHostToControllerInterface));
                    }
                    break;

                case gCSReadLocalSupportedCapabilitiesOpCode_c:
                    {
                        fsciCSCallApiFunction(CS_ReadLocalSupportedCapabilities());
                    }
                    break;

                case gCSReadRemoteSupportedCapabilitiesOpCode_c:
                   {
                       deviceId_t deviceId;

                       /* Get deviceId parameter from the received packet */
                       fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

                       fsciCSCallApiFunction(CS_ReadRemoteSupportedCapabilities(deviceId));
                   }
                   break;

                case gCSSecurityEnableOpCode_c:
                    {
                        deviceId_t deviceId;

                        /* Get deviceId parameter from the received packet */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

                        fsciCSCallApiFunction(CS_SecurityEnable(deviceId));
                    }
                    break;

                case gCSSetDefaultSettingsOpCode_c:
                    {
                        deviceId_t     deviceId;
                        uint8_t        role;
                        uint8_t        syncAntennaSelection;
                        int8_t         maxTxPowerLevel;

                        /* Get deviceId parameter from the received packet */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        /* Get role parameter from the received packet */
                        fsciBleGetUint8ValueFromBuffer(role, pBuffer);
                        /* Get syncAntennaSelection parameter from the received packet */
                        fsciBleGetUint8ValueFromBuffer(syncAntennaSelection, pBuffer);
                        /* Get maxTxPowerLevel parameter from the received packet */
                        fsciBleGetUint8ValueFromBuffer(maxTxPowerLevel, pBuffer);

                        fsciCSCallApiFunction(CS_SetDefaultSettings(deviceId, (csRoleType)role, syncAntennaSelection, maxTxPowerLevel));
                    }
                    break;

                case gCSReadRemoteFAETableOpCode_c:
                    {
                        deviceId_t     deviceId;

                        /* Get deviceId parameter from the received packet */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

                        fsciCSCallApiFunction(CS_ReadRemoteFAETable(deviceId));
                    }
                    break;

                case gCSWriteRemoteFAETableOpCode_c:
                    {
                        deviceId_t     deviceId;
                        uint8_t        aReflectorTable[gCSReflectorTableSize_c];

                        /* Get deviceId parameter from the received packet */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        fsciBleGetArrayFromBuffer(aReflectorTable, pBuffer, gCSReflectorTableSize_c);

                        fsciCSCallApiFunction(CS_WriteRemoteFAETable(deviceId, aReflectorTable));
                    }
                    break;

                case gCSCreateConfigOpCode_c:
                    {
                        deviceId_t     deviceId;
                        gCsCreateConfigCommandParams_t     createConfigParams;

                        /* Get deviceId parameter from the received packet */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        /* Get create config command parameters from the received packet */
                        fsciCSCreateConfigFromBuffer(&createConfigParams, &pBuffer);

                        fsciCSCallApiFunction(CS_CreateConfig(deviceId, &createConfigParams));
                    }
                    break;

                case gCSRemoveConfigOpCode_c:
                    {
                        deviceId_t     deviceId;
                        uint8_t        configId;

                        /* Get deviceId parameter from the received packet */
                        fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                        /* Get configId parameter from the received packet */
                        fsciBleGetUint8ValueFromBuffer(configId, pBuffer);

                        fsciCSCallApiFunction(CS_RemoveConfig(deviceId, configId));
                    }
                    break;

                case gCSSetChannelClassificationOpCode_c:
                    {
                         uint8_t aChannelClassification[gHCICSChannelMapSize];

                         /* Get channel classification information from the received packet */
                         fsciBleGetArrayFromBuffer(aChannelClassification, pBuffer, gHCICSChannelMapSize);

                        fsciCSCallApiFunction(CS_SetChannelClassification(aChannelClassification));
                    }
                    break;

              case gCSProcedureEnableOpCode_c:
                  {
                      deviceId_t     deviceId;
                      bool_t         enable;
                      uint8_t        configId;

                      /* Get deviceId parameter from the received packet */
                      fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                      /* Get procedure enable parameters from the received packet */
                      fsciBleGetBoolValueFromBuffer(enable, pBuffer);
                      fsciBleGetUint8ValueFromBuffer(configId, pBuffer);

                      fsciCSCallApiFunction(CS_ProcedureEnable(deviceId, enable, configId));
                  }
                  break;

              case gCSTestOpCode_c:
                  {
                      gCsTestCommandParams_t testCmdParams;

                      /* Get CS test parameters from the received packet */
                      fsciCSTestFromBuffer(&testCmdParams, &pBuffer);

                      fsciCSCallApiFunction(CS_Test(&testCmdParams));
                  }
                  break;
                  
              case gCSWriteCachedRemoteCapabilitiesOpCode_c:
                  {
                      csReadRemoteSupportedCapabilitiesCompleteEvent_t params;
                      
                      /* Get command parameters from buffer */
                      fsciCSWriteCachedRemoteSupportedCapabilitiesFromBuffer(&params, &pBuffer);

                      fsciCSCallApiFunction(CS_WriteCachedRemoteSupportedCapabilities(&params));

                  }
                  break;

              case gCSTestEndOpCode_c:
                  {
                      fsciCSCallApiFunction(CS_TestEnd());
                  }
                  break;

              case gCSSetProcedureParameters_c:
                  {
                      deviceId_t     deviceId;
                      gCsSetProcedureParamsCommandParams_t procedureParams;

                      /* Get deviceId parameter from the received packet */
                      fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
                      /* Get set procedure parameters from the received packet */
                      fsciCSSetProcedureParametersFromBuffer(&procedureParams, &pBuffer);

                      fsciCSCallApiFunction(CS_SetProcedureParameters(deviceId, &procedureParams));
                  }
                  break;

              case gCSEvtReadRemoteSupportedCapabilitiesOpCode_c:
                  {
                      csMetaEvent_t metaEvent;

                      metaEvent.eventType = csReadRemoteSupportedCapabilitiesComplete_c;
                      fsciCSReadRemoteSupportedCapabilitiesFromBuffer(&metaEvent.eventData.csReadRemoteSupportedCapabilitiesComplete, &pBuffer);
                      /* Trigger application callback */
                      pCsMetaEventCallback(&metaEvent);
                  }
                  break;

              case gCSReadRemoteFAETableCompleteOpCode_c:
                  {
                      csMetaEvent_t metaEvent;

                      metaEvent.eventType = csReadRemoteFAETableComplete_c;
                      fsciCSReadRemoteFAETableFromBuffer(&metaEvent.eventData.csReadRemoteFAETableComplete, &pBuffer);
                      /* Trigger application callback */
                      pCsMetaEventCallback(&metaEvent);
                  }
                  break;

              case gCSSecurityEnableCompleteOpCode_c:
                  {
                      csMetaEvent_t metaEvent;

                      metaEvent.eventType = csSecurityEnableComplete_c;
                      fsciCSSecurityEnableCompleteFromBuffer(&metaEvent.eventData.csSecurityEnableComplete, &pBuffer);
                      /* Trigger application callback */
                      pCsMetaEventCallback(&metaEvent);
                  }
                  break;

              case gCSConfigCompleteOpCode_c:
                  {
                      csMetaEvent_t metaEvent;

                      metaEvent.eventType = csConfigComplete_c;
                      fsciCSConfigCompleteFromBuffer(&metaEvent.eventData.csConfigComplete, &pBuffer);
                      /* Trigger application callback */
                      pCsMetaEventCallback(&metaEvent);
                  }
                  break;

              case gCSProcedureEnableCompleteEventOpCode_c:
                {
                    csMetaEvent_t metaEvent;

                    metaEvent.eventType = csProcedureEnableCompleteEvent_c;
                    fsciCSProcedureEnableCompleteFromBuffer(&metaEvent.eventData.csProcedureEnableCompleteEvent, &pBuffer);
                    pCsMetaEventCallback(&metaEvent);
                }
                break;

              case gcsSubeventResultEventOpCode_c:
                  {
                      csMetaEvent_t metaEvent;
                      uint32_t dataTotalLength = 0U;

                      metaEvent.eventType = csSubeventResultEvent_c;
                      fsciCSEventResultFromBuffer(&metaEvent.eventData.csSubeventResultEvent, &pBuffer);

                      /* Compute total data length */
                      uint8_t *stepDataLenPtr = pBuffer + 2 * sizeof(uint8_t);
                      for (uint8_t step = 0; step < metaEvent.eventData.csSubeventResultEvent.numStepsReported; step++)
                      {
                          /* Account for mode, channel, data_length fields */
                          dataTotalLength += *(stepDataLenPtr + dataTotalLength) + sizeof(uint8_t) * 3;
                      }

                      metaEvent.eventData.csSubeventResultEvent.pData = MEM_BufferAlloc(dataTotalLength);
                      if (metaEvent.eventData.csSubeventResultEvent.pData == NULL)
                      {
                          fsciBleError(gFsciOutOfMessages_c, fsciBleInterfaceId);
                      }
                      else
                      {
                          /* Copy data from buffer */
                          fsciBleGetArrayFromBuffer(metaEvent.eventData.csSubeventResultEvent.pData, pBuffer, dataTotalLength);
                          /* Trigger application callback */
                          pCsMetaEventCallback(&metaEvent);
                          /* Free event data */
                          MEM_BufferFree(metaEvent.eventData.csSubeventResultEvent.pData);
                      }
                  }
                  break;

              case gcsSubeventResultContinueEventOpCode_c:
                  {
                      csMetaEvent_t metaEvent;
                      uint32_t dataTotalLength = 0U;

                      metaEvent.eventType = csSubeventResultContinueEvent_c;
                      fsciCSEventResultContinueFromBuffer(&metaEvent.eventData.csSubeventResultContinueEvent, &pBuffer);

                     /* Compute total data length */
                      uint8_t *stepDataLenPtr = pBuffer + 2 * sizeof(uint8_t);
                      for (uint8_t step = 0; step < metaEvent.eventData.csSubeventResultContinueEvent.numStepsReported; step++)
                      {
                          /* Account for mode, channel, data_length fields */
                          dataTotalLength += *(stepDataLenPtr + dataTotalLength) + sizeof(uint8_t) * 3;
                      }

                      metaEvent.eventData.csSubeventResultContinueEvent.pData = MEM_BufferAlloc(dataTotalLength);
                      if (metaEvent.eventData.csSubeventResultContinueEvent.pData == NULL)
                      {
                          fsciBleError(gFsciOutOfMessages_c, fsciBleInterfaceId);
                      }
                      else
                      {
                          /* Copy data from buffer */
                          fsciBleGetArrayFromBuffer(metaEvent.eventData.csSubeventResultContinueEvent.pData,
                                                    pBuffer,
                                                    dataTotalLength);
                          /* Trigger application callback */
                          pCsMetaEventCallback(&metaEvent);
                          /* Free event data */
                          MEM_BufferFree(metaEvent.eventData.csSubeventResultContinueEvent.pData);
                      }
                  }
                  break;

               case gCSTestEndEvtOpCode_c:
                    {
                        csMetaEvent_t metaEvent;
                        metaEvent.eventType = csTestEnd_c;

                        /* Copy data from buffer */
                        fsciBleGetUint8ValueFromBuffer(metaEvent.eventData.csTestEndEvent.status, pBuffer);
                        /* Trigger application callback */
                        pCsMetaEventCallback(&metaEvent);
                    }
                    break;

                case gCsEventResultDebugOpCode_c:
                    {
                        csMetaEvent_t metaEvent;
                        metaEvent.eventType = csEventResultDebug_c;

                        /* Copy data from buffer */
                        fsciBleGetUint16ValueFromBuffer(metaEvent.eventData.csEventResultDebugEvent.payloadLength, pBuffer);
                        fsciBleGetUint16ValueFromBuffer(metaEvent.eventData.csEventResultDebugEvent.bufferLength, pBuffer);
                        fsciBleGetUint16ValueFromBuffer(metaEvent.eventData.csEventResultDebugEvent.offset, pBuffer);

                        /* Trigger application callback */
                        pCsMetaEventCallback(&metaEvent);
                    }
                    break;

                case gCSErrorOpCode_c:
                    {
                        csMetaEvent_t metaEvent;
                        metaEvent.eventType = csError_c;

                        /* Copy data from buffer */
                        fsciCSErrorEvtFromBuffer(&metaEvent.eventData.csError, &pBuffer);
                        /* Trigger application callback */
                        pCsMetaEventCallback(&metaEvent);
                    }
                    break;

                default:
                    {
                        /* Unknown FSCI opcode */
                        fsciBleError(gFsciUnknownOpcode_c, fsciBleInterfaceId);
                    }
                    break;
            }
#endif
        }
        else
        {
            /* FSCI SAP disabled */
            fsciBleError(gFsciSAPDisabled_c, fsciBleInterfaceId);
        }
    }

    (void)MEM_BufferFree(pData);
}

bleResult_t fsciCSLeMetaEventHandler(csMetaEvent_t* pPacket)
{
    switch (pPacket->eventType)
    {
        case csConfigComplete_c:
        {
            fsciCSConfigCompleteEvtMonitor(&pPacket->eventData.csConfigComplete);
        }
        break;

        case csSubeventResultEvent_c:
        {
            fsciCSEventResultEvtMonitor(&pPacket->eventData.csSubeventResultEvent);
            /* Free Event Data */
            (void)MEM_BufferFree(pPacket->eventData.csSubeventResultEvent.pData);
        }
        break;

        case csSubeventResultContinueEvent_c:
        {
            fsciCSEventResultContinueEvtMonitor(&pPacket->eventData.csSubeventResultContinueEvent);
            (void)MEM_BufferFree(pPacket->eventData.csSubeventResultContinueEvent.pData);
        }
        break;

        case csSecurityEnableComplete_c:
        {
            fsciCSSecurityEnableCompleteEvtMonitor(&pPacket->eventData.csSecurityEnableComplete);
        }
        break;

        case csReadRemoteSupportedCapabilitiesComplete_c:
        {
            fsciCSReadRemoteSupportedCapabilitiesEvtMonitor(&pPacket->eventData.csReadRemoteSupportedCapabilitiesComplete);
        }
        break;

        case csReadRemoteFAETableComplete_c:
        {
            fsciCSReadRemoteFAETableEvtMonitor(&pPacket->eventData.csReadRemoteFAETableComplete);
        }
        break;

        case csEventResultDebug_c:
        {
            fsciCSEventResultDebugEvtMonitor(&pPacket->eventData.csEventResultDebugEvent);
        }
        break;

        case csProcedureEnableCompleteEvent_c:
        {
            fsciCSProcedureEnableCompleteEvtMonitor(&pPacket->eventData.csProcedureEnableCompleteEvent);
        }
        break;

        case csError_c:
        {
            fsciCSLeMetaErrorStatusMonitor(&pPacket->eventData.csError);
        }
        break;
        
        case csTestEnd_c:
        {
            fsciCSNoParamsEventMonitor(gCSTestEndEvtOpCode_c);
        }
        break;

        default:
        {
            /* Unkown packet type - silent frop */
        }
        break;
    }

    return gBleSuccess_c;
}

bleResult_t fsciCSCmdCompleteEventHandler(csCommandCompleteEvent_t* pPacket)
{
    switch (pPacket->eventType)
    {
        case readLocalSupportedCapabilities_c:
        {
            fsciCSReadLocalSupportedCapabilitiesEvtMonitor(&pPacket->eventData.csReadLocalSupportedCapabilities);
        }
        break;
      
        case setDefaultSettings_c:
        {
            fsciCSCmdCompleteDeviceIdMonitor(pPacket->deviceId, gCSEventSetDefaultSettingsOpCode_c);
        }
        break;
        
        case writeRemoteFAETable_c:
        {
            fsciCSCmdCompleteDeviceIdMonitor(pPacket->deviceId, gCSEventWriteRemotelFAETableOpCode_c);
        }
        break;
    
        case setChannelClassification_c:
        {
            fsciCSNoParamCmdMonitor(gCSEventSetChannelClassificationOpCode_c);
        }
        break;
        
        case setProcedureParameters_c:
        {
            fsciCSCmdCompleteDeviceIdMonitor(pPacket->deviceId, gCSEvenSetProcedureParamsCmdOpCode_c);
        }
        break;
        
        case writeCachedRemoteCapabilities_c:
        {
            fsciCSCmdCompleteDeviceIdMonitor(pPacket->deviceId, gCsEventWriteCachedRemoteCapabilitiesOpCode_c);
        }
        break;
    
        case testCmd_c:
        {
            fsciCSNoParamCmdMonitor(gCSEventTestCmdOpCode_c);
        }
        break;
        
        case commandError_c:
        {
            fsciCSErrorStatusMonitor(&pPacket->eventData.csCommandError);
        }
        break;

        default:
        break;
    }

    return gBleSuccess_c;
}

bleResult_t fsciCSCmdStatusEventHandler(csCommandStatusEvent_t* pPacket)
{
    fsciCSCmdStatusMonitor(pPacket);
        
    return gBleSuccess_c;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
static bleResult_t csHciHostToControllerInterface
(
    hciPacketType_t packetType,
    void* pPacket,
    uint16_t packetSize
)
{
    return gBleSuccess_c;
}

/*! *********************************************************************************
* \brief  Creates the FSCI packet which contains the status of the last executed CS
*         command and sends it over UART.
*
* \param[in]    result      Status of the last executed CS command.
*
********************************************************************************** */
static void fsciCSStatusMonitor(bleResult_t result)
{
    /* If CS is disabled the status must be not monitored */
    if(FALSE == bFsciCSEnabled)
    {
        return;
    }
    /* Send status over UART */
    fsciBleStatusMonitor(gFsciCSOpcodeGroup_c, gCSStatusOpCode_c, result);
}

/*! *********************************************************************************
* \brief  Creates a CS FSCI packet without payload and sends it over UART.
*
* \param[in]    opCode      CS command operation code.
*
********************************************************************************** */
static void fsciCSNoParamCmdMonitor(fsciCSOpCode_t opCode)
{
    /* If CS is disabled or if the command was initiated by FSCI it must be not monitored */
    if (FALSE == bFsciCSEnabled)
    {
        return;
    }

    /* Call the generic FSCI BLE monitor for commands or events that have no parameters */
    fsciBleNoParamCmdOrEvtMonitor(gFsciCSOpcodeGroup_c, opCode);
}

/*! *********************************************************************************
* \brief  Monitor function for CS command status events
*
* \param[in]    pPacket       Pointer to command status event
*
********************************************************************************** */
static void fsciCSCmdStatusMonitor(csCommandStatusEvent_t* pPacket)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket(gCSEventCommandStatusOpCode_c, sizeof(csCommandStatusEvent_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromEnumValue(pPacket->eventType, pBuffer, csCommandStatusType);
    fsciBleGetBufferFromEnumValue(pPacket->status, pBuffer, hciErrorCode_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  csErrorEvent_t event monitoring function.
*
* \param[in]    pEvent     Pointer to the event information
*
********************************************************************************** */
static void fsciCSLeMetaErrorStatusMonitor(csErrorEvent_t* errorEvent)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket(gCSErrorOpCode_c, sizeof(csErrorEvent_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromEnumValue(errorEvent->csErrorSource, pBuffer, csErrorSource_t);
    fsciBleGetBufferFromEnumValue(errorEvent->status, pBuffer, bleResult_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  csSecurityEnableCompleteEvent_t event monitoring function.
*
* \param[in]    pEvent     Pointer to the event information
*
********************************************************************************** */
static void fsciCSSecurityEnableCompleteEvtMonitor
(
    csSecurityEnableCompleteEvent_t* pEvent
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket((uint8_t)gCSSecurityEnableCompleteOpCode_c, sizeof(deviceId_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&pEvent->deviceId, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  csConfigCompleteEvent_t event monitoring function.
*
* \param[in]    pEvent     Pointer to the event information
*
********************************************************************************** */
static void fsciCSConfigCompleteEvtMonitor
(
    csConfigCompleteEvent_t* pEvent
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    uint32_t dataLength = sizeof(deviceId_t) + 21U * sizeof(uint8_t) + gHCICSChannelMapSize;

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket((uint8_t)gCSConfigCompleteOpCode_c, dataLength);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&pEvent->deviceId, &pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->configId, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->status, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->action, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->mainModeType, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->subModeType, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->mainModeMinSteps, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->mainModeMaxSteps, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->mainModeRepetition, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->mode0Steps, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->role, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->RTTTypes, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->RTTPhy, pBuffer);
    fsciBleGetBufferFromArray(pEvent->channelMap, pBuffer, gHCICSChannelMapSize);
    fsciBleGetBufferFromUint8Value(pEvent->channelMapRepetition, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->channelSelectionType, pBuffer);
    fsciBleGetBufferFromEnumValue(pEvent->ch3cShape, pBuffer, hoppingAlgorithmTypes_t);
    fsciBleGetBufferFromUint8Value(pEvent->ch3cJump, pBuffer);
    fsciBleGetBufferFromBoolValue(pEvent->companionSignalEnable, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->TIP1time, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->TIP2time, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->TFCStime, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->TPMtime, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  csReadRemoteFAETableCompleteEvent_t event monitoring function.
*
* \param[in]    pEvent     Pointer to the event information
*
********************************************************************************** */
static void fsciCSReadRemoteFAETableEvtMonitor
(
    csReadRemoteFAETableCompleteEvent_t* pEvent
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    uint32_t dataLength = sizeof(deviceId_t) + gCSReflectorTableSize_c * sizeof(uint8_t);

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket((uint8_t)gCSReadRemoteFAETableCompleteOpCode_c, dataLength);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&pEvent->deviceId, &pBuffer);
    fsciBleGetBufferFromArray(pEvent->aFAETable, pBuffer, gCSReflectorTableSize_c);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  csProcedureEnableCompleteEvent_t event monitoring function.
*
* \param[in]    pEvent     Pointer to the event information
*
********************************************************************************** */
static void fsciCSProcedureEnableCompleteEvtMonitor
(
    csProcedureEnableCompleteEvent_t* pEvent
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    uint32_t dataLength = sizeof(deviceId_t) + (6U + gCSMaxSubeventLen_c) * sizeof(uint8_t) + 4U * sizeof(uint16_t);

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket((uint8_t)gCSProcedureEnableCompleteEventOpCode_c, dataLength);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    fsciBleGetBufferFromDeviceId(&pEvent->deviceId, &pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->configId, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->status, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->state, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->toneAntennaConfigSelection, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->selectedTxPower, pBuffer);
    fsciBleGetBufferFromArray(pEvent->subeventLen, pBuffer, gCSMaxSubeventLen_c);
    fsciBleGetBufferFromUint8Value(pEvent->subeventsPerInterval, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->subeventInterval, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->eventInterval, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->procedureInterval, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->procedureCount, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  csSubeventResultEvent_t event monitoring function.
*
* \param[in]    pEvent     Pointer to the event information
*
********************************************************************************** */
static void fsciCSEventResultEvtMonitor
(
    csSubeventResultEvent_t* pEvent
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    uint16_t                    dataLength = 0U;

    uint32_t dataLengthTotal = sizeof(deviceId_t) + 7U * sizeof(uint8_t) + 4U * sizeof(uint16_t);

    uint8_t *stepDataLenPtr = pEvent->pData + 2 * sizeof(uint8_t); /* Skip mode, channel */
    for (uint8_t step = 0; step < pEvent->numStepsReported; step ++)
    {
        /* also account for mode, channel, data_length fields */
        dataLength += *(stepDataLenPtr + dataLength) + sizeof(uint8_t) * 3U;
    }

    dataLengthTotal += dataLength * sizeof(uint8_t);

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket((uint8_t)gcsSubeventResultEventOpCode_c, dataLengthTotal);

    if (NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    fsciBleGetBufferFromDeviceId(&pEvent->deviceId, &pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->configId, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->startACLConnEvent, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->procedureCounter, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->frequencyCompensation, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->referencePowerLevel, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->procedureDoneStatus, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->subeventDoneStatus, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->abortReason, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->numAntennaPaths, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->numStepsReported, pBuffer);
    fsciBleGetBufferFromUint16Value(dataLength, pBuffer);
    fsciBleGetBufferFromArray(pEvent->pData, pBuffer, dataLength);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  csSubeventResultContinueEvent_t event monitoring function.
*
* \param[in]    pEvent     Pointer to the event information
*
********************************************************************************** */
static void fsciCSEventResultContinueEvtMonitor
(
    csSubeventResultContinueEvent_t* pEvent
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    uint16_t                    dataLength = 0U;

    uint32_t dataLengthTotal = sizeof(deviceId_t) + 8U * sizeof(uint8_t);

    uint8_t *stepDataLenPtr = pEvent->pData + 2 * sizeof(uint8_t); /* Skip mode, channel */
    for (uint8_t step = 0; step < pEvent->numStepsReported; step ++)
    {
        /* also account for mode, channel, data_length fields */
        dataLength += *(stepDataLenPtr + dataLength) + sizeof(uint8_t) * 3;
    }

    dataLengthTotal += dataLength * sizeof(uint8_t);

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket((uint8_t)gcsSubeventResultContinueEventOpCode_c, dataLengthTotal);

    if (NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    fsciBleGetBufferFromDeviceId(&pEvent->deviceId, &pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->configId, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->procedureDoneStatus, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->subeventDoneStatus, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->abortReason, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->numAntennaPaths, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->numStepsReported, pBuffer);
    fsciBleGetBufferFromUint16Value(dataLength, pBuffer);
    fsciBleGetBufferFromArray(pEvent->pData, pBuffer, dataLength);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  Monitor function for events with only deviceId as information
*
* \param[in]    deviceId     Peer device Id
* \param[in]    opCode       Event opcode
*
********************************************************************************** */
static void fsciCSCmdCompleteDeviceIdMonitor(deviceId_t deviceId, fsciCSOpCode_t opCode)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket(opCode, sizeof(deviceId_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  csReadLocalSupportedCapabilities_t event monitoring function.
*
* \param[in]    pEvent     Pointer to the event information
*
********************************************************************************** */
static void fsciCSReadLocalSupportedCapabilitiesEvtMonitor
(
    csReadLocalSupportedCapabilities_t* pEvent
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    uint32_t dataLength = 11U * sizeof(uint8_t) + 9U * sizeof(uint16_t);

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket((uint8_t)gCSEventReadLocalSupportedCapabilitiesOpCode_c, dataLength);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromUint8Value(pEvent->numConfigSupported, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->maxConsecutiveProceduresSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->numAntennasSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->maxAntennaPathsSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->rolesSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->optionalModesSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->RTTCapability, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->RTTAAOnly, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->RTTSoundingN, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->RTTRandomPayloadN, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalNADMSoundingCapability, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalNADMRandomCapability, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->optionalSyncPhysSupported, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalSubfeaturesSupported, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalTIP1timesSupported, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalTIP2timesSupported, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalTFCStimesSupported, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalTPMtimesSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->TSWtimesSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->optionalTxSNRCapability, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  csReadRemoteSupportedCapabilitiesCompleteEvent_t event monitoring function.
*
* \param[in]    pEvent     Pointer to the event information
*
********************************************************************************** */
static void fsciCSReadRemoteSupportedCapabilitiesEvtMonitor
(
    csReadRemoteSupportedCapabilitiesCompleteEvent_t* pEvent
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;
    uint32_t dataLength = sizeof(deviceId_t) + 12U * sizeof(uint8_t) + 8U * sizeof(uint16_t);

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket((uint8_t)gCSEvtReadRemoteSupportedCapabilitiesOpCode_c, dataLength);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&pEvent->deviceId, &pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->numConfigSupported, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->maxConsecutiveProceduresSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->numAntennasSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->maxAntennaPathsSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->rolesSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->optionalModesSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->RTTCapability, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->RTTAAOnly, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->RTTSoundingN, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->RTTRandomPayload, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalNADMSoundingCapability, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalNADMRandomCapability, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->optionalSyncPhysSupported, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalSubfeaturesSupported, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalTIP1timesSupported, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalTIP2timesSupported, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalTFCStimesSupported, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->optionalTPMtimesSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->TSWtimesSupported, pBuffer);
    fsciBleGetBufferFromUint8Value(pEvent->optionalTxSNRCapability, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  csEventResultDebugEvent_t event monitoring function.
*
* \param[in]    pEvent     Pointer to the event information
*
********************************************************************************** */
static void fsciCSEventResultDebugEvtMonitor
(
    csEventResultDebugEvent_t* pEvent
)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket((uint8_t)gCsEventResultDebugOpCode_c, 3U * sizeof(uint16_t)
                                          + pEvent->payloadLength);

    if (NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromUint16Value(pEvent->payloadLength, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->bufferLength, pBuffer);
    fsciBleGetBufferFromUint16Value(pEvent->offset, pBuffer);
    fsciBleGetBufferFromArray(pEvent->pData, pBuffer, pEvent->payloadLength);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  Monitor function for error events
*
* \param[in]    errorEvent      Command complete error event
*
********************************************************************************** */
static void fsciCSErrorStatusMonitor(csCommandError_t* pErrorEvent)
{
    clientPacketStructured_t*   pClientPacket;
    uint8_t*                    pBuffer;

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciCSAllocFsciPacket(gCSEventErrorOpCode_c, sizeof(csCommandError_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&pErrorEvent->deviceId, &pBuffer);
    fsciBleGetBufferFromEnumValue(pErrorEvent->errorSource, pBuffer, commandErrorSource_t);
    fsciBleGetBufferFromEnumValue(pErrorEvent->status, pBuffer, bleResult_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
* \brief  Monitor function for events with no parameters
*
* \param[in]    opCode       Event opcode
*
********************************************************************************** */
static void fsciCSNoParamsEventMonitor(fsciCSOpCode_t opCode)
{
    /* Call the generic FSCI BLE monitor for commands or events that have no parameters */
    fsciBleNoParamCmdOrEvtMonitor(gFsciCSOpcodeGroup_c, opCode);
}

/*! *********************************************************************************
* \brief  Unpack CS Create Config command parameters from the received buffer
*
* \param[in]    pCreateConfigParams     CS create config command parameters.
* \param[in]    ppBuffer                UART data buffer
*
********************************************************************************** */
static void fsciCSCreateConfigFromBuffer
(
    gCsCreateConfigCommandParams_t*    pCreateConfigParams,
    uint8_t**                          ppBuffer
)
{
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->configId, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->createContext, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->mainModeType, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->subModeType, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->mainModeMinSteps, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->mainModeMaxSteps, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->mainModeRepetition, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->mode0Steps, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->role, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pCreateConfigParams->RTTTypes, *ppBuffer, rttTypes_t);
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->cSyncPhy, *ppBuffer);
    fsciBleGetArrayFromBuffer(pCreateConfigParams->channelMap, *ppBuffer, gHCICSChannelMapSize);
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->channelMapRepetition, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->channelSelectionType, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pCreateConfigParams->ch3cShape, *ppBuffer, hoppingAlgorithmTypes_t);
    fsciBleGetUint8ValueFromBuffer(pCreateConfigParams->ch3cJump, *ppBuffer);
    fsciBleGetBoolValueFromBuffer(pCreateConfigParams->companionSignalEnable, *ppBuffer);
}

/*! *********************************************************************************
* \brief  Unpack CS Set Procedure Parameter command parameters from the received buffer
*
* \param[in]    pProcedurParams     CS set procedure parameters command parameters.
* \param[in]    ppBuffer            UART data buffer
*
********************************************************************************** */
static void fsciCSSetProcedureParametersFromBuffer
(
    gCsSetProcedureParamsCommandParams_t* pProcedurParams,
    uint8_t**                             ppBuffer
)
{
    fsciBleGetUint8ValueFromBuffer(pProcedurParams->configId, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pProcedurParams->maxProcedureDuration, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pProcedurParams->minProcedureInterval, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pProcedurParams->maxProcedureInterval, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pProcedurParams->maxProcCount, *ppBuffer);
    fsciBleGetUint32ValueFromBuffer(pProcedurParams->minSubeventLen, *ppBuffer);
    fsciBleGetUint32ValueFromBuffer(pProcedurParams->maxSubeventLen, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pProcedurParams->toneAntennaConfigSelection, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pProcedurParams->phys, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pProcedurParams->txPwrDelta, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pProcedurParams->preferredPeerAntenna, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pProcedurParams->SNRCtrlInitiator, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pProcedurParams->SNRCtrlReflector, *ppBuffer);
}

/*! *********************************************************************************
* \brief  Unpack CS Test command parameters from the received buffer
*
* \param[in]    pTestCmdParams       CS test command parameters.
* \param[in]    ppBuffer             UART data buffer
*
********************************************************************************** */
static void fsciCSTestFromBuffer
(
    gCsTestCommandParams_t* pTestCmdParams,
    uint8_t**               ppBuffer
)
{
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->mainModeType, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->subModeType, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->mainModeRepetition, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->mode0Steps, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->role, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pTestCmdParams->RTTTypes, *ppBuffer, rttTypes_t);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->CSSyncPhy, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->CSSYNCAntennaSelection, *ppBuffer);
    fsciBleGetArrayFromBuffer(pTestCmdParams->subeventLen, *ppBuffer, gCSMaxSubeventLen_c);
    fsciBleGetUint16ValueFromBuffer(pTestCmdParams->subeventInterval, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->maxNumSubevents, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->transmitPowerLevel, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->TIP1time, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->TIP2time, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->TFCStime, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->TPMtime, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->TSWtime, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->toneAntennaConfig, *ppBuffer);
    fsciBleGetBoolValueFromBuffer(pTestCmdParams->companionSignalEnable, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->SNRCtrlInitiator, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->SNRCtrlReflector, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pTestCmdParams->DRBG_Nonce, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->channelMapRepetition, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pTestCmdParams->overrideConfig, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pTestCmdParams->overrideParametersLength, *ppBuffer);
    fsciBleGetArrayFromBuffer(pTestCmdParams->overrideParametersData,
                              *ppBuffer,
                              pTestCmdParams->overrideParametersLength);
}

/*! *********************************************************************************
* \brief  Unpack CS Write Cached Remote Supported Capabilities command parameters
*         from the received buffer
*
* \param[in]    pCmdParams       CS write cached command parameters.
* \param[in]    ppBuffer         UART data buffer
*
********************************************************************************** */
static void fsciCSWriteCachedRemoteSupportedCapabilitiesFromBuffer
(
    csReadRemoteSupportedCapabilitiesCompleteEvent_t* pCmdParams,
    uint8_t**                                         ppBuffer
)
{
    fsciBleGetDeviceIdFromBuffer(&pCmdParams->deviceId, ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCmdParams->numConfigSupported, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pCmdParams->maxConsecutiveProceduresSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCmdParams->numAntennasSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCmdParams->maxAntennaPathsSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCmdParams->rolesSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCmdParams->optionalModesSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCmdParams->RTTCapability, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCmdParams->RTTAAOnly, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCmdParams->RTTSoundingN, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCmdParams->RTTRandomPayload, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pCmdParams->optionalNADMRandomCapability, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pCmdParams->optionalNADMSoundingCapability, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCmdParams->optionalSyncPhysSupported, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pCmdParams->optionalSubfeaturesSupported, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pCmdParams->optionalTIP1timesSupported, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pCmdParams->optionalTIP2timesSupported, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pCmdParams->optionalTFCStimesSupported, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pCmdParams->optionalTPMtimesSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCmdParams->TSWtimesSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pCmdParams->optionalTxSNRCapability, *ppBuffer);
}

/*! *********************************************************************************
* \brief  Unpack CS Read Remote Supported Capabilities event from the received buffer
*
* \param[in]    pEvent       CS event parameters.
* \param[in]    ppBuffer     UART data buffer
*
********************************************************************************** */
static void fsciCSReadRemoteSupportedCapabilitiesFromBuffer
(
    csReadRemoteSupportedCapabilitiesCompleteEvent_t* pEvent,
    uint8_t**                                         ppBuffer
)
{
    fsciBleGetDeviceIdFromBuffer(&pEvent->deviceId, ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->numConfigSupported, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->maxConsecutiveProceduresSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->numAntennasSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->maxAntennaPathsSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->rolesSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->optionalModesSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->RTTCapability, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->RTTAAOnly, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->RTTSoundingN, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->RTTRandomPayload, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->optionalNADMSoundingCapability, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->optionalNADMRandomCapability, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->optionalSyncPhysSupported, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->optionalSubfeaturesSupported, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->optionalTIP1timesSupported, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->optionalTIP2timesSupported, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->optionalTFCStimesSupported, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->optionalTPMtimesSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->TSWtimesSupported, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->optionalTxSNRCapability, *ppBuffer);
}

/*! *********************************************************************************
* \brief  Unpack CS Read Remote FAE Table event from the received buffer
*
* \param[in]    pEvent       CS event parameters.
* \param[in]    ppBuffer     UART data buffer
*
********************************************************************************** */
static void fsciCSReadRemoteFAETableFromBuffer
(
    csReadRemoteFAETableCompleteEvent_t* pEvent,
    uint8_t**                            ppBuffer
)
{
    fsciBleGetDeviceIdFromBuffer(&pEvent->deviceId, ppBuffer);
    fsciBleGetArrayFromBuffer(pEvent->aFAETable, *ppBuffer, gCSReflectorTableSize_c);
}

/*! *********************************************************************************
* \brief  Unpack CS Security Enable event from the received buffer
*
* \param[in]    pEvent       CS event parameters.
* \param[in]    ppBuffer     UART data buffer
*
********************************************************************************** */
static void fsciCSSecurityEnableCompleteFromBuffer
(
    csSecurityEnableCompleteEvent_t* pEvent,
    uint8_t**                        ppBuffer
)
{
    fsciBleGetDeviceIdFromBuffer(&pEvent->deviceId, ppBuffer);
}

/*! *********************************************************************************
* \brief  Unpack CS Config Complete event from the received buffer
*
* \param[in]    pEvent       CS event parameters.
* \param[in]    ppBuffer     UART data buffer
*
********************************************************************************** */
static void fsciCSConfigCompleteFromBuffer
(
    csConfigCompleteEvent_t* pEvent,
    uint8_t**                ppBuffer
)
{
    fsciBleGetDeviceIdFromBuffer(&pEvent->deviceId, ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->configId, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->status, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->action, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->mainModeType, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->subModeType, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->mainModeMinSteps, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->mainModeMaxSteps, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->mainModeRepetition, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->mode0Steps, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->role, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->RTTTypes, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->RTTPhy, *ppBuffer);
    fsciBleGetArrayFromBuffer(pEvent->channelMap, *ppBuffer, gHCICSChannelMapSize);
    fsciBleGetUint8ValueFromBuffer(pEvent->channelMapRepetition, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->channelSelectionType, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pEvent->ch3cShape, *ppBuffer, hoppingAlgorithmTypes_t);
    fsciBleGetUint8ValueFromBuffer(pEvent->ch3cJump, *ppBuffer);
    fsciBleGetBoolValueFromBuffer(pEvent->companionSignalEnable, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->TIP1time, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->TIP2time, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->TFCStime, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->TPMtime, *ppBuffer);
}

/*! *********************************************************************************
* \brief  Unpack CS Procedure Enable event from the received buffer
*
* \param[in]    pEvent       CS event parameters.
* \param[in]    ppBuffer     UART data buffer
*
********************************************************************************** */
static void fsciCSProcedureEnableCompleteFromBuffer
(
    csProcedureEnableCompleteEvent_t* pEvent,
    uint8_t**                         ppBuffer
)
{
    fsciBleGetDeviceIdFromBuffer(&pEvent->deviceId, ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->configId, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->status, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->state, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->toneAntennaConfigSelection, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->selectedTxPower, *ppBuffer);
    fsciBleGetArrayFromBuffer(pEvent->subeventLen, *ppBuffer, gCSMaxSubeventLen_c);
    fsciBleGetUint8ValueFromBuffer(pEvent->subeventsPerInterval, *ppBuffer);
    fsciBleGetArrayFromBuffer(pEvent->subeventInterval, *ppBuffer, gCSMaxSubeventLen_c);
    fsciBleGetUint16ValueFromBuffer(pEvent->eventInterval, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->procedureInterval, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->procedureCount, *ppBuffer);
}

/*! *********************************************************************************
* \brief  Unpack CS Event Result event from the received buffer
*
* \param[in]    pEvent       CS event parameters.
* \param[in]    ppBuffer     UART data buffer
*
********************************************************************************** */
static void fsciCSEventResultFromBuffer
(
    csSubeventResultEvent_t* pEvent,
    uint8_t**             ppBuffer
)
{
    fsciBleGetDeviceIdFromBuffer(&pEvent->deviceId, ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->configId, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->startACLConnEvent, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->procedureCounter, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pEvent->frequencyCompensation, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->referencePowerLevel, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->procedureDoneStatus, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->subeventDoneStatus, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->numAntennaPaths, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->numStepsReported, *ppBuffer);
}

/*! *********************************************************************************
* \brief  Unpack CS Event Continue Result event from the received buffer
*
* \param[in]    pEvent       CS event parameters.
* \param[in]    ppBuffer     UART data buffer
*
********************************************************************************** */
static void fsciCSEventResultContinueFromBuffer
(
    csSubeventResultContinueEvent_t* pEvent,
    uint8_t**             ppBuffer
)
{
    fsciBleGetDeviceIdFromBuffer(&pEvent->deviceId, ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->configId, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->procedureDoneStatus, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->subeventDoneStatus, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->numAntennaPaths, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pEvent->numStepsReported, *ppBuffer);
}

/*! *********************************************************************************
* \brief  Unpack CS Error event from the received buffer
*
* \param[in]    pEvent       CS event parameters.
* \param[in]    ppBuffer     UART data buffer
*
********************************************************************************** */
static void fsciCSErrorEvtFromBuffer
(
    csErrorEvent_t* pEvent,
    uint8_t**       ppBuffer
)
{
    fsciBleGetEnumValueFromBuffer(pEvent->csErrorSource, *ppBuffer, csErrorSource_t);
    fsciBleGetEnumValueFromBuffer(pEvent->status, *ppBuffer, bleResult_t);
}

#endif /* gBLE_ChannelSounding_d */
#endif /* gFsciIncluded_c && gFsciBleL2capLayerEnabled_d */

/*! *********************************************************************************
* @}
********************************************************************************** */
