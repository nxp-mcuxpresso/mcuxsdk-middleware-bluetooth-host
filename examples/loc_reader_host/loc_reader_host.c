/*! *********************************************************************************
* \addtogroup Localization Reader application
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file loc_reader_host.c
*
* Copyright 2025-2026 NXP
*
* NXP Proprietary
*
* This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or
* by downloading, installing, activating and/or otherwise using the software, you are
* agreeing that you have read, and that you agree to comply with and are bound by,
* such license terms. If you do not agree to be bound by the applicable license terms,
* then you may not retain, install, activate or otherwise use the software.
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
/* Framework / Drivers */
#include "EmbeddedTypes.h"
#include "fsl_component_button.h"
#include "fsl_component_timer_manager.h"
#include "fsl_component_panic.h"
#include "FunctionLib.h"
#include "fsl_component_mem_manager.h"
#include "fsl_adapter_reset.h"
#include "fsl_debug_console.h"
#include "fsl_format.h"
#include "fsl_shell.h"
#include "FsciCommunication.h"
#include "fsci_ble_interface.h"

#include "app.h"
#include "app_conn.h"
#include "board.h"
#include "fwk_platform_ble.h"
#include "NVM_Interface.h"
#include "RNG_Interface.h"

/* BLE Host Stack */
#include "loc_reader_host.h"
#include "app_localization.h"
#include "app_localization_algo.h"
#include "ranging_client_interface.h"

#include "rpmsg_config.h"
#include "ble_port_fsci.h"
#include "host_app.h"

#ifdef LCE_KW47_MCXW72
#include "app_lce_init.h"
#endif /* LCE_KW47_MCXW72 */
#include "fsl_shell.h"

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/
#if defined (gFsciComponentShellCore0_d) && (gFsciComponentShellCore0_d > 0)
extern SHELL_HANDLE_DEFINE(g_shellHandle);

#define shell_write(a)       (void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, a)
#define SHELL_NEWLINE()      (void)SHELL_WriteSynchronization((shell_handle_t)g_shellHandle, "\r\n", 2U)
#define shell_writeN(a,b)    (void)SHELL_WriteSynchronization((shell_handle_t)g_shellHandle, a, b)
#define shell_writeDec(a)    (void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, (const char*)FORMAT_Dec2Str(a))
#define shell_writeBool(a)   if(a){(void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, "TRUE");}else{(void)SHELL_PrintfSynchronization((shell_handle_t)g_shellHandle, "FALSE");}
#define shell_writeHex       BleApp_PrintHex
#define shell_writeHexLe     BleApp_PrintHexLe
#define shell_cmd_finished() SHELL_PrintPrompt((shell_handle_t)g_shellHandle)
#else
#define shell_write(a)
#define SHELL_NEWLINE()
#define shell_writeN(a,b)
#define shell_writeDec(a)
#define shell_writeBool(a)
#define shell_writeHex
#define shell_writeHexLe
#define shell_cmd_finished()
#endif
/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

#if (defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)) || \
    (defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1))
static float mPreviousDistance = (float)10.0;
#endif

/*! Algorithm result structure */
static localizationAlgoResult_t mAlgoResult;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

/*!*************************************************************************************************
*\brief This is the callback for displaying distance measurement results

***************************************************************************************************/
static void BleApp_PrintMeasurementResults
(
    deviceId_t deviceId,
    localizationAlgoResult_t *pResult
);

/*!*************************************************************************************************
*\brief Checks if remote and local IQ data has been received from Core 1 and the algorithm can be run.
*
***************************************************************************************************/
static void BleApp_CheckRunAlgo(deviceId_t deviceId);

/*!*************************************************************************************************
*\brief Runs the localization algorithm with the data received from Core 1
*
***************************************************************************************************/
static void App_RunAlgo
(
    void* pData
);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief    Initializes application specific functionality before the BLE stack init.
*
********************************************************************************** */
void BluetoothLEHost_AppInit(void)
{
    /* Framework init */
#if defined(gRngSeedStorageAddr_d) || defined(gXcvrDacTrimValueSorageAddr_d)
    NV_Init();
#endif /* gRngSeedStorageAddr_d || gXcvrDacTrimValueSorageAddr_d */

#if defined(gAppUseNvm_d) && (gAppUseNvm_d > 0)
    /* Initialize NV module */
    (void)NvModuleInit();
#endif /* gAppUseNvm_d */

#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
    (void)PLATFORM_EnableBleSecureKeyManagement();
#endif

    /* Initialize the Application specific FSCI module */
    (void)App_HostAppFsciInit();

    /* Start LCE */
#ifdef LCE_KW47_MCXW72
    (void)APP_InitLce();
#endif

#if (gAppUseNvm_d && defined(gFsciIncluded_c) && (gFsciIncluded_c))
#if gNvmEnableFSCIMonitoring_c
    NV_SetFSCIMonitoringState(TRUE);
#endif /* gNvmEnableFSCIMonitoring_c */
#if gNvmEnableFSCIRequests_c
    NV_RegisterToFSCI();
#endif /* gNvmEnableFSCIRequests_c */
#endif /* gAppUseNvm_d && gFsciIncluded_c */
}

/*! *********************************************************************************
*\private
*\fn           void BluetoothLEHost_ProcessIdleTask(void)
*\brief        Handles Connectivity background task, usually executed from Idle task.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void BluetoothLEHost_ProcessIdleTask(void)
{
#if defined(gAppUseNvm_d) && (gAppUseNvm_d > 0)
    if(NvIdle() == 0)
#endif /* gAppUseNvm_d */
    {
    }
}

/*! *********************************************************************************
* \brief        Handles Shell_Factory Reset Command event.
********************************************************************************** */
void BleApp_FactoryReset(void)
{
    /* Erase NVM Datasets */
#if defined(gAppUseNvm_d) && (gAppUseNvm_d  == 1)
    NVM_Status_t status = NvFormat();
    if (status != gNVM_OK_c)
    {
         /* NvFormat exited with an error status */
         panic(0, (uint32_t)BleApp_FactoryReset, 0, 0);
    }
#endif
    /* Reset MCU */
    HAL_ResetMCU();
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
static void unpackDataHeader
(
    rasMeasurementData_t* pDstData,
    uint8_t* pSrcData
)
{
    uint8_t* pSrcAux = pSrcData;

    if (pDstData != NULL)
    {
        pDstData->deviceId = *pSrcAux++;
        pDstData->configId = *pSrcAux++;
        pDstData->procedureCounter = Utils_ExtractTwoByteValue(pSrcAux);
        pSrcAux = &pSrcAux[2U];
        pDstData->numAntennaPaths = *pSrcAux++;
        pDstData->totalNumSteps = *pSrcAux++;
        pDstData->selectedTxPower = (int8_t)(*pSrcAux++);
        pDstData->dataIndex = Utils_ExtractFourByteValue(pSrcAux);
        pSrcAux = &pSrcAux[4U];
        pDstData->totalSentRcvDataIndex = Utils_ExtractTwoByteValue(pSrcAux);
        pSrcAux = &pSrcAux[2U];
        pDstData->dataParsedLen = Utils_ExtractTwoByteValue(pSrcAux);
        pSrcAux = &pSrcAux[2U];
        pDstData->subeventIndex = *pSrcAux++;
        pDstData->crtNumSteps = *pSrcAux++;

        for (uint8_t idx = 0U; idx < gRasMaxNumSubevents_c; idx++)
        {
            pDstData->aSubEventData[idx].dataIdx = Utils_ExtractFourByteValue(pSrcAux);
            pSrcAux = &pSrcAux[4U];
            pDstData->aSubEventData[idx].dataSize = Utils_ExtractTwoByteValue(pSrcAux);
            pSrcAux = &pSrcAux[2U];
            pDstData->aSubEventData[idx].filteredDataSize = Utils_ExtractTwoByteValue(pSrcAux);
            pSrcAux = &pSrcAux[2U];
            pDstData->aSubEventData[idx].currentDataSize = Utils_ExtractTwoByteValue(pSrcAux);
            pSrcAux = &pSrcAux[2U];
            pDstData->aSubEventData[idx].subevtHeader.startACLConnEvent = Utils_ExtractTwoByteValue(pSrcAux);
            pSrcAux = &pSrcAux[2U];
            pDstData->aSubEventData[idx].subevtHeader.frequencyCompensation = Utils_ExtractTwoByteValue(pSrcAux);
            pSrcAux = &pSrcAux[2U];
            pDstData->aSubEventData[idx].subevtHeader.referencePowerLevel = (int8_t)(*pSrcAux++);
            pDstData->aSubEventData[idx].subevtHeader.procedureDoneStatus = *pSrcAux++;
            pDstData->aSubEventData[idx].subevtHeader.subeventDoneStatus = *pSrcAux++;
            pDstData->aSubEventData[idx].subevtHeader.abortReason = *pSrcAux++;
            pDstData->aSubEventData[idx].subevtHeader.numStepsReported = *pSrcAux++;
        }
    }
}

/*!*************************************************************************************************
*\fn    void BleApp_HandleNcpMsg(void* pMsg);
*\brief Handler function for messages received from Core 1
*
*\param[in]   pMsg    Pointer to message
*
*\return      none
***************************************************************************************************/
void BleApp_HandleNcpMsg
(
   ncpIqTransferMsgType_t eventType,
    uint8_t* pMsg
)
{
    uint8_t* pEventData = pMsg;
    deviceId_t deviceId = pEventData[0];
    pEventData++;
    uint16_t dataHeaderSize = (sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
    uint32_t evenDataSize = (uint32_t)gFsciMaxPayloadLen_c - sizeof(ncpIqTransferMsgType_t) - sizeof(deviceId_t) - 1U;
    union {
      uint32_t u32;
      uint16_t u16;
    } dataLen = {};

    rasMeasurementData_t* pLocalData = AppLocalization_GetLocalData(deviceId);
    rasMeasurementData_t* pPeerData = RasClient_GetPeerRangingData(deviceId);

    switch (eventType)
    {
        case gIQLocalTrStart_c:
        {
            unpackDataHeader(pLocalData, pEventData);
            /* Save local data */
            if (pLocalData->pData == NULL)
            {
                pLocalData->pData = MEM_BufferAlloc(sizeof(csAppData_t));
            }

            if (pLocalData->pData != NULL)
            {
                pLocalData->totalSentRcvDataIndex = 0U;
                FLib_MemCpy(pLocalData->pData,
                            pEventData + dataHeaderSize,
                            (evenDataSize - dataHeaderSize));
                dataLen.u32 = evenDataSize - dataHeaderSize;
                pLocalData->totalSentRcvDataIndex += dataLen.u16;
            }
        }
        break;

        case gIQLocalTrCont_c:
        {
            /* Save local data */
            if (pLocalData->pData != NULL)
            {
                FLib_MemCpy(pLocalData->pData + pLocalData->totalSentRcvDataIndex,
                            pEventData,
                            evenDataSize);
                dataLen.u32 = evenDataSize;
                pLocalData->totalSentRcvDataIndex += dataLen.u16;
            }
        }
        break;

        case gIQLocalTrEnd_c:
        {
            /* Save local data */
            if (pLocalData->pData != NULL)
            {
                uint32_t remainingBytes = sizeof(csAppData_t) - pLocalData->totalSentRcvDataIndex;

                FLib_MemCpy(pLocalData->pData + pLocalData->totalSentRcvDataIndex,
                            pEventData,
                            remainingBytes);
                dataLen.u32 = pLocalData->dataIndex;
                pLocalData->totalSentRcvDataIndex = pLocalData->dataIndex;

            }
        }
        break;

        case gIQRemoteTrStart_c:
        {
            RasClient_Init(NULL, deviceId);
            unpackDataHeader(pPeerData, pEventData);
            /* Save remote data */
            if (pPeerData->pData == NULL)
            {
                pPeerData->pData = MEM_BufferAlloc(sizeof(csAppData_t));
            }

            if (pPeerData->pData != NULL)
            {
                pPeerData->totalSentRcvDataIndex = 0U;
                FLib_MemCpy(pPeerData->pData,
                            pEventData+dataHeaderSize,
                            (evenDataSize - dataHeaderSize));
                dataLen.u32 = evenDataSize - dataHeaderSize;
                pPeerData->totalSentRcvDataIndex += dataLen.u16;
            }
        }
        break;

        case gIQRemoteTrCont_c:
        {
            /* Save remote data */
            if (pPeerData->pData != NULL)
            {
                FLib_MemCpy(pPeerData->pData + pPeerData->totalSentRcvDataIndex,
                            pEventData,
                            evenDataSize);
                dataLen.u32 = evenDataSize;
                pPeerData->totalSentRcvDataIndex += dataLen.u16;
            }
        }
        break;

        case gIQRemoteTrEnd_c:
        {
            /* Save remote data */
            if (pPeerData->pData != NULL)
            {
                uint32_t remainingBytes = sizeof(csAppData_t) - pPeerData->totalSentRcvDataIndex;

                FLib_MemCpy(pPeerData->pData + pPeerData->totalSentRcvDataIndex,
                            pEventData,
                            remainingBytes);
                dataLen.u32 = pPeerData->dataIndex;
                pPeerData->totalSentRcvDataIndex = pPeerData->dataIndex;

                /* Chck if all data was received and run algorithm if yes */
                BleApp_CheckRunAlgo(deviceId);
            }
        }
        break;

        default:
        {
            ; /* No action required */
        }
        break;
    }
}

/*!*************************************************************************************************
*\fn    void App_RunAlgo(void* pData);
*\brief Handler function to run the algorithm with the data received from Core 1
*
*\param[in]   pData    Pointer to message - unused
*
*\return      none
***************************************************************************************************/
static void App_RunAlgo
(
    void* pData
)
{
    deviceId_t deviceId = (deviceId_t)(uint32_t)pData;

    rasMeasurementData_t* pLocalData = AppLocalization_GetLocalData(deviceId);
    rasMeasurementData_t* pPeerData = RasClient_GetPeerRangingData(deviceId);

    FLib_MemSet(&mAlgoResult, 0U, sizeof(localizationAlgoResult_t));
    mAlgoResult.algorithm = AppLocalization_GetAlgorithm();

    /* We have both remote and local data - run algo */
    RasClient_SetPeerRangingData(deviceId, pPeerData);
    AppLocalizationAlgo_RunMeasurement(deviceId, pLocalData, pPeerData, mGlobalRangeSettings.role,  &mAlgoResult);
    BleApp_PrintMeasurementResults(deviceId, &mAlgoResult);

    /* Free local data */
    if (pLocalData->pData != NULL)
    {
        (void)MEM_BufferFree(pLocalData->pData);
        pLocalData->pData = NULL;
    }
    FLib_MemSet(pLocalData, 0U, sizeof(rasMeasurementData_t));

    /* Free peer data */
    if (pPeerData->pData != NULL)
    {
        (void)MEM_BufferFree(pPeerData->pData);
        pPeerData->pData = NULL;
    }
    FLib_MemSet(pPeerData, 0U, sizeof(rasMeasurementData_t));
}

/*!*************************************************************************************************
*\fn    void BleApp_CheckRunAlgo(void);
*\brief Checks if remote and local IQ data has been received from Core 1 and the algorithm can be run.
*
*\param[in]   none
*
*\return      none
***************************************************************************************************/
static void BleApp_CheckRunAlgo(deviceId_t deviceId)
{
    rasMeasurementData_t* pLocalData = AppLocalization_GetLocalData(deviceId);
    rasMeasurementData_t* pPeerData = RasClient_GetPeerRangingData(deviceId);

    if ((pLocalData->pData != NULL) && (pPeerData->pData != NULL))
    {
        (void)App_PostCallbackMessage(App_RunAlgo, (void*)(uint32_t)deviceId);
    }
}

/*!*************************************************************************************************
*\fn    void BleApp_PrintMeasurementResults(deviceId_t deviceId, localizationAlgoResult_t *pResult)
*\brief This is the callback for displaying distance measurement results
*
*\param[in]   deviceId    Peer identifier
*\param[in]   pResult     Pointer to algorithm results to be printed
*
*\return      none
***************************************************************************************************/
static void BleApp_PrintMeasurementResults
(
    deviceId_t deviceId,
    localizationAlgoResult_t *pResult
)
{
    uint16_t qInt =0U;
    uint16_t qFrac = 0U;

#if ((defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)) || \
    (defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)))
    bool_t bUIUpdated = FALSE;
#endif /* ((defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)) || \
            (defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1))) */

#if defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1)
    if ((pResult->algorithm & eMciqAlgoEmbedRADE) != 0U)
    {
        shell_write("\r\n[");
        shell_writeDec((uint8_t)deviceId);

        if (pResult->radeError != 0U)
        {
            shell_write("] RADE Error: ");
            shell_writeDec(pResult->radeError);
            shell_write("!\r\n");
        }
        else if (pResult->resultRADE.dqiIntegerPart == 0U)
        {
            shell_write("] Low Quality data for RADE! Quality indicator is 0! \n\r");
        }
        else if (pResult->resultRADE.distanceIntegerPart > gMaxDistanceMeters_c)
        {
            shell_write("] Outlier RADE distance result - discarded. Check gMaxDistanceMeters_c value. \n\r");
        }
        else
        {
            shell_write("] Distance (RADE): ");
            /* Display the integer part of the distance in meters. */
            shell_writeDec(pResult->resultRADE.distanceIntegerPart);
            shell_write(".");

            /* Leading zeroes for decimal part */
            for (uint8_t i = 0U; i < pResult->resultRADE.leadingZeroesDecimalPart; i++)
            {
                shell_write("0");
            }

            /* Display the decimal part of the distance in meters. */
            shell_writeDec(pResult->resultRADE.distanceDecimalPart);
            shell_write(" m   ");

            shell_write("Quality: ");
            shell_writeDec(pResult->resultRADE.dqiIntegerPart);
            shell_write(".");
            shell_writeDec(pResult->resultRADE.dqiDecimalPart);
            shell_write("%%\r\n");
            /* Flash LEDs if distance is less than 1m. */
            if ((mPreviousDistance <= (float)1) && (pResult->resultRADE.distanceInMeters <= (float)1))
            {
                LedStartFlashingAllLeds();
                bUIUpdated = TRUE;
            }
            else if ((mPreviousDistance > (float)1) && (pResult->resultRADE.distanceInMeters > (float)1))
            {
                LedStopFlashingAllLeds();
                Led1On();
                bUIUpdated = TRUE;
            }
            else
            {
                /* MISRA */
            }
            mPreviousDistance = pResult->resultRADE.distanceInMeters;
        }
    }
#endif /* gAppUseRADEAlgorithm_d */

#if defined(gAppUseCDEAlgorithm_d) && (gAppUseCDEAlgorithm_d == 1)
    if ((pResult->algorithm & eMciqAlgoEmbedCDE) != 0U)
    {
        shell_write("\r\n[");
        shell_writeDec((uint8_t)deviceId);
        shell_write("] Distance (CDE): ");
        /* Display the integer part of the distance in meters. */
        shell_writeDec(pResult->resultCDE.distanceIntegerPart);
        shell_write(".");

        /* Leading zeroes for decimal part */
        for (uint8_t i = 0U; i < pResult->resultCDE.leadingZeroesDecimalPart; i++)
        {
            shell_write("0");
        }

        /* Display the decimal part of the distance in meters. */
        shell_writeDec(pResult->resultCDE.distanceDecimalPart);
        shell_write(" m   ");

        shell_write("Quality: ");
        shell_writeDec(pResult->resultCDE.dqiIntegerPart);
        shell_write(".");
        shell_writeDec(pResult->resultCDE.dqiDecimalPart);
        shell_write("%%\r\n");

        /* Flash LEDs if distance is less than 1m. */
        if (bUIUpdated == FALSE)
        {
            if ((mPreviousDistance <= (float)1) && (pResult->resultCDE.distanceInMeters <= (float)1))
            {
                LedStartFlashingAllLeds();
            }
            else if ((mPreviousDistance > (float)1) && (pResult->resultCDE.distanceInMeters > (float)1))
            {
                LedStopFlashingAllLeds();
                Led1On();
            }
            else
            {
                /* MISRA */
            }
            mPreviousDistance = pResult->resultCDE.distanceInMeters;
        }
    }
#endif /* gAppUseCDEAlgorithm_d */

#if defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1)
    shell_write("Time information:");
    shell_write("\r\n");
    if (pResult->csConfigDuration != 0)
    {
        shell_write("CS Config: ");
        shell_writeDec(pResult->csConfigDuration/1000);
        shell_write("ms\r\n");
    }
    if (pResult->csProcedureDuration != 0)
    {
        shell_write("CS Procedure: ");
        shell_writeDec(pResult->csProcedureDuration/1000);
        shell_write("ms\r\n");
    }
    if (pResult->transferDuration != 0)
    {
        shell_write("RAS transfer: ");
        shell_writeDec(pResult->transferDuration/1000);
        shell_write("ms\r\n");
    }
    if (pResult->algoDuration != 0)
    {
        shell_write("Localization algorithm: ");
        shell_writeDec(pResult->algoDuration/1000);
        shell_write("ms\r\n");
    }
#endif /* defined(gAppCsTimeInfo_d) && (gAppCsTimeInfo_d == 1) */

    /* Print RTT information */
    shell_write("\r\n[");
    shell_writeDec((uint8_t)deviceId);
    shell_write("] RTT Distance: ");

    /* Set negative distance to zero */
    if (pResult->rttResult.dm_ad <= 0)
    {
        qInt = 0U;
        qFrac = 0U;
    }
    else
    {
        /* Round to nearest integer */
        pResult->rttResult.dm_ad += 3277;
        qInt = (uint16_t)((uint32_t)pResult->rttResult.dm_ad >> 16U);
        qFrac = (uint16_t)((((uint32_t)pResult->rttResult.dm_ad & 0x0000FFFFU)*10U) >> 16U);
    }

    /* Print Results */
    shell_writeDec(qInt);
    shell_write(".");
    shell_writeDec(qFrac);
    shell_write(" m Success Rate: ");
    shell_writeDec(pResult->rttResult.dm_sr);
    shell_write("\r\n");

/* Print average RSSI values */
#if defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1)
    shell_write("RSSI information:");
    shell_write("\r\n");
    if (pResult->rssiInfo.rssiLocalNo != 0U)
    {
        int8_t rssiLocalAverage = 0;
        shell_write("Local Average: ");
        for (uint8_t idx = 0U; idx < pResult->rssiInfo.rssiLocalNo; idx++)
        {
            rssiLocalAverage += (pResult->rssiInfo.aRssiLocal[idx]/pResult->rssiInfo.rssiLocalNo);
        }
        if (((uint8_t)rssiLocalAverage >> 7U) != 0U)
        {
            shell_write("-");
            rssiLocalAverage = ~((uint8_t)rssiLocalAverage - 1U);
        }
        shell_writeDec(rssiLocalAverage);
        shell_write("    ");
    }

    if (pResult->rssiInfo.rssiRemoteNo != 0U)
    {
        int8_t rssiRemoteAverage = 0;
        shell_write("Remote Average: ");
        for (uint8_t idx = 0U; idx < pResult->rssiInfo.rssiRemoteNo; idx++)
        {
            rssiRemoteAverage += (pResult->rssiInfo.aRssiRemote[idx]/pResult->rssiInfo.rssiRemoteNo);
        }
        if (((uint8_t)rssiRemoteAverage >> 7U) != 0U)
        {
            shell_write("-");
            rssiRemoteAverage = ~((uint8_t)rssiRemoteAverage - 1U);
        }
        shell_writeDec(rssiRemoteAverage);
        shell_write("\r\n");
    }
#endif /* defined(gAppParseRssiInfo_d) && (gAppParseRssiInfo_d == 1) */
}

#if defined(gAppDemoPrint_d) && (gAppDemoPrint_d == 1)
/*! *********************************************************************************
* \brief  This is the callback for displaying IQ data samples
********************************************************************************** */
static void BleApp_PrintIqResults(int16_t *iq1, int16_t *iq2, uint8_t n_ap)
{
    shell_write("IQ:");

    for(uint8_t i = 0; i < n_ap; i++)
    {
        for(uint8_t j = 0; j < gHadmChannelsNb_c; j++)
        {
            shell_writeHexLe((uint8_t*)&iq1[i * gHadmChannelsNb_c * 2 + 2 * j + 1], 2);
            shell_writeHexLe((uint8_t*)&iq1[i * gHadmChannelsNb_c * 2 + 2 * j], 2);
        }
        for(uint8_t j = 0; j < gHadmChannelsNb_c; j ++)
        {
            shell_writeHexLe((uint8_t*)&iq2[i * gHadmChannelsNb_c * 2 + 2 * j + 1], 2);
            shell_writeHexLe((uint8_t*)&iq2[i * gHadmChannelsNb_c * 2 + 2 * j], 2);
        }
    }
    shell_write("\r\n");
}
#endif /* defined(gAppDemoPrint_d) && (gAppDemoPrint_d == 1) */
