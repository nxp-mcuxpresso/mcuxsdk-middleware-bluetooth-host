/*! *********************************************************************************
* \file pts_test.c
*
* Copyright 2026 NXP
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
#include "EmbeddedTypes.h"
#include "fsl_os_abstraction.h"
#include "fsl_component_mem_manager.h"
#include "ble_general.h"
#include "app_localization.h"
#include "channel_sounding.h"
#include "pts_test.h"

#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)
#include "pts_test_vectors.h"
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */

#if defined (gAppRasDataTransfer_d) && (gAppRasDataTransfer_d == 1)
#include "ranging_interface.h"
#if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
#include "ranging_client_interface.h"
#endif /* gRasRREQ_d */
#endif /* gAppRasDataTransfer_d */

#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
/* Callbacks registered by app_localization.c */
static pfProcessCsResultsEvent_t mpfProcessCsResultsEvent = NULL;

static pfProcessCsResultsContinueEvent_t mpfProcessCsResultsContinueEvent = NULL;

/* Local aliases keep the test vector code unchanged */
#define processCsResultsEvent(pEvent)         mpfProcessCsResultsEvent(pEvent)
#define processCsResultsContinueEvent(pEvent) mpfProcessCsResultsContinueEvent(pEvent)

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn         PtsTest_RegisterCallbacks(pfProcessCsResultsEvent_t pfProcessResults,
*                                       pfProcessCsResultsContinueEvent_t pfProcessResultsContinue)
*
*\brief      Register the result handlers owned by the application.
*
*\return     None
********************************************************************************** */
void PtsTest_RegisterCallbacks(pfProcessCsResultsEvent_t pfProcessResults,
                               pfProcessCsResultsContinueEvent_t pfProcessResultsContinue)
{
    mpfProcessCsResultsEvent = pfProcessResults;
    mpfProcessCsResultsContinueEvent = pfProcessResultsContinue;
}

/*! *********************************************************************************
*\fn         AppLocalization_RunPtsTest(deviceId_t deviceId, uint8_t index, uint8_t actionMask)
*
*\brief      Test vector parsing to process the data.
*
*\return     None
********************************************************************************** */
void AppLocalization_RunPtsTest(deviceId_t deviceId, uint8_t index, uint8_t actionMask)
{
    bool_t bSendDataReady       = (actionMask & BIT0) != 0U;
    bool_t bDataOverwrittenTest = (actionMask & BIT1) != 0U;
    /* Measurement data base obtained through the exposed getter */
    rasMeasurementData_t* pResultData = AppLocalization_GetLocalData(0U);

    if (pResultData != NULL)
    {
        switch(index)
        {
            case 1U: /* Test vector 1 - single_subevent_mode1_only */
            {
                if (AppLocalization_AllocLocalData(deviceId) == NULL)
                {
                    break;
                }

                pResultData[deviceId].selectedTxPower = 0x7F;
                pResultData[deviceId].subeventIndex = 0U;
                csSubeventResultEvent_t subeventResult = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .startACLConnEvent = 0x003A,
                    .procedureCounter = 0U,
                    .frequencyCompensation = 0x00C0,
                    .referencePowerLevel = (int8_t)0xF7,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x01,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 19,
                    .pData = tv1,
                };
                (void)processCsResultsEvent(&subeventResult);

                csSubeventResultContinueEvent_t subeventResultContinue = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x01,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 19,
                    .pData = &tv1[tv1Se0DataIndex],
                };
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                subeventResultContinue.procedureDoneStatus = 0x00;
                subeventResultContinue.subeventDoneStatus = 0x00;
                subeventResultContinue.abortReason = 0x00;
                subeventResultContinue.numAntennaPaths = 1U;
                subeventResultContinue.numStepsReported = 19;
                subeventResultContinue.pData = &tv1[tv1Se0DataIndex +
                                                    tv1Se0c0DataIndex];
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                break;
            }

            case 2U: /* Test vector 2 - single_subevent_mode2_mode1 (actually two subevents) */
            {
                if (AppLocalization_AllocLocalData(deviceId) == NULL)
                {
                    break;
                }

                pResultData[deviceId].selectedTxPower = 0x7F;
                pResultData[deviceId].subeventIndex = 0U;
                csSubeventResultEvent_t subeventResult = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .startACLConnEvent = 0x003A,
                    .procedureCounter = 0U,
                    .frequencyCompensation = 0x00C0,
                    .referencePowerLevel = (int8_t)0xFF,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x01,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 21,
                    .pData = tv2,
                };
                (void)processCsResultsEvent(&subeventResult);

                csSubeventResultContinueEvent_t subeventResultContinue = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x01,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 20,
                    .pData = &tv2[tv2Se0DataIndex],
                };
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                subeventResultContinue.procedureDoneStatus = 0x01;
                subeventResultContinue.subeventDoneStatus = 0x01;
                subeventResultContinue.abortReason = 0x00;
                subeventResultContinue.numAntennaPaths = 1U;
                subeventResultContinue.numStepsReported = 20;
                subeventResultContinue.pData = &tv2[tv2Se0DataIndex +
                                                    tv2Se0c0DataIndex];
                (void)processCsResultsContinueEvent(&subeventResultContinue);


                subeventResultContinue.procedureDoneStatus = 0x01;
                subeventResultContinue.subeventDoneStatus = 0x00;
                subeventResultContinue.abortReason = 0x00;
                subeventResultContinue.numAntennaPaths = 1U;
                subeventResultContinue.numStepsReported = 20;
                subeventResultContinue.pData = &tv2[tv2Se0DataIndex +
                                                    tv2Se0c0DataIndex +
                                                    tv2Se0c1DataIndex];
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                pResultData[deviceId].subeventIndex = 1U;
                subeventResult.startACLConnEvent = 0x00B5;
                subeventResult.frequencyCompensation = 0xC000;
                subeventResult.referencePowerLevel = (int8_t)0xFE;
                subeventResult.procedureDoneStatus = 0x00;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 17;
                subeventResult.pData = &tv2[tv2Se0DataIndex +
                                            tv2Se0c0DataIndex +
                                            tv2Se0c1DataIndex +
                                            tv2Se0c2DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                break;
            }

            case 3U: /* Test vector 3 - single_subevent_mode2_only */
            {
                if (AppLocalization_AllocLocalData(deviceId) == NULL)
                {
                    break;
                }

                pResultData[deviceId].selectedTxPower = 0x7F;
                pResultData[deviceId].subeventIndex = 0U;
                csSubeventResultEvent_t subeventResult = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .startACLConnEvent = 0x0039,
                    .procedureCounter = 0U,
                    .frequencyCompensation = 0xC000,
                    .referencePowerLevel = (int8_t)0xF9,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x01,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 20,
                    .pData = tv3,
                };
                (void)processCsResultsEvent(&subeventResult);

                csSubeventResultContinueEvent_t subeventResultContinue = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x01,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 19,
                    .pData = &tv3[tv3Se0DataIndex],
                };
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                subeventResultContinue.procedureDoneStatus = 0x01;
                subeventResultContinue.subeventDoneStatus = 0x01;
                subeventResultContinue.abortReason = 0x00;
                subeventResultContinue.numAntennaPaths = 1U;
                subeventResultContinue.numStepsReported = 19;
                subeventResultContinue.pData = &tv3[tv3Se0DataIndex +
                                                    tv3Se0c0DataIndex];
                (void)processCsResultsContinueEvent(&subeventResultContinue);


                subeventResultContinue.procedureDoneStatus = 0x00;
                subeventResultContinue.subeventDoneStatus = 0x00;
                subeventResultContinue.abortReason = 0x00;
                subeventResultContinue.numAntennaPaths = 1U;
                subeventResultContinue.numStepsReported = 16;
                subeventResultContinue.pData = &tv3[tv3Se0DataIndex +
                                                    tv3Se0c0DataIndex +
                                                    tv3Se0c1DataIndex];
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                break;
            }

            case 4U: /* Test vector 4 - single_subevent_mode3_only (actually two subevents) */
            {
                if (AppLocalization_AllocLocalData(deviceId) == NULL)
                {
                    break;
                }

                pResultData[deviceId].selectedTxPower = 0x7F;
                pResultData[deviceId].subeventIndex = 0U;
                csSubeventResultEvent_t subeventResult = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .startACLConnEvent = 0x0039,
                    .procedureCounter = 0U,
                    .frequencyCompensation = 0xC000,
                    .referencePowerLevel = (int8_t)0xF8,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x01,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 14,
                    .pData = tv4,
                };
                (void)processCsResultsEvent(&subeventResult);

                csSubeventResultContinueEvent_t subeventResultContinue = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x01,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 13,
                    .pData = &tv4[tv4Se0DataIndex],
                };
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                subeventResultContinue.procedureDoneStatus = 0x01;
                subeventResultContinue.subeventDoneStatus = 0x01;
                subeventResultContinue.abortReason = 0x00;
                subeventResultContinue.numAntennaPaths = 1U;
                subeventResultContinue.numStepsReported = 13;
                subeventResultContinue.pData = &tv4[tv4Se0DataIndex +
                                                    tv4Se0c0DataIndex];
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                subeventResultContinue.procedureDoneStatus = 0x01;
                subeventResultContinue.subeventDoneStatus = 0x01;
                subeventResultContinue.abortReason = 0x00;
                subeventResultContinue.numAntennaPaths = 1U;
                subeventResultContinue.numStepsReported = 20;
                subeventResultContinue.pData = &tv4[tv4Se0DataIndex +
                                                    tv4Se0c0DataIndex +
                                                    tv4Se0c1DataIndex];
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                subeventResultContinue.procedureDoneStatus = 0x01;
                subeventResultContinue.subeventDoneStatus = 0x00;
                subeventResultContinue.abortReason = 0x00;
                subeventResultContinue.numAntennaPaths = 1U;
                subeventResultContinue.numStepsReported = 11;
                subeventResultContinue.pData = &tv4[tv4Se0DataIndex +
                                                    tv4Se0c0DataIndex +
                                                    tv4Se0c1DataIndex +
                                                    tv4Se0c2DataIndex];
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                pResultData[deviceId].subeventIndex = 1U;
                subeventResult.startACLConnEvent = 0x0039;
                subeventResult.frequencyCompensation = 0xC000;
                subeventResult.referencePowerLevel = (int8_t)0xF8;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x01;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 14;
                subeventResult.pData = &tv4[tv4Se0DataIndex +
                                            tv4Se0c0DataIndex +
                                            tv4Se0c1DataIndex +
                                            tv4Se0c2DataIndex +
                                            tv4Se0c3DataIndex];
                (void)processCsResultsEvent(&subeventResult);


                subeventResultContinue.procedureDoneStatus = 0x00;
                subeventResultContinue.subeventDoneStatus = 0x00;
                subeventResultContinue.abortReason = 0x00;
                subeventResultContinue.numAntennaPaths = 1U;
                subeventResultContinue.numStepsReported = 1;
                subeventResultContinue.pData = &tv4[tv4Se0DataIndex +
                                                    tv4Se0c0DataIndex +
                                                    tv4Se0c1DataIndex +
                                                    tv4Se0c2DataIndex +
                                                    tv4Se0c3DataIndex +
                                                    tv4Se1DataIndex];
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                break;
            }

            case 5U: /* Test vector 5 - multiple_subevents_mode1_only (7 subevents) */
            {
                if (AppLocalization_AllocLocalData(deviceId) == NULL)
                {
                    break;
                }

                pResultData[deviceId].selectedTxPower = 0x7F;
                pResultData[deviceId].subeventIndex = 0U;
                csSubeventResultEvent_t subeventResult = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .startACLConnEvent = 0x00B0,
                    .procedureCounter = 0U,
                    .frequencyCompensation = 0x0000,
                    .referencePowerLevel = (int8_t)0xF6,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x00,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 16,
                    .pData = tv5,
                };
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 1U;
                subeventResult.startACLConnEvent = 0x00B1;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF8;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 16;
                subeventResult.pData = &tv5[tv5Se0DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 2U;
                subeventResult.startACLConnEvent = 0x00B2;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xFA;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 16;
                subeventResult.pData = &tv5[tv5Se0DataIndex +
                                            tv5Se1DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 3U;
                subeventResult.startACLConnEvent = 0x00B3;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF6;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 16;
                subeventResult.pData = &tv5[tv5Se0DataIndex +
                                            tv5Se1DataIndex +
                                            tv5Se2DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 4U;
                subeventResult.startACLConnEvent = 0x00B4;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF7;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 16;
                subeventResult.pData = &tv5[tv5Se0DataIndex +
                                            tv5Se1DataIndex +
                                            tv5Se2DataIndex +
                                            tv5Se3DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 5U;
                subeventResult.startACLConnEvent = 0x00B5;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF8;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 16;
                subeventResult.pData = &tv5[tv5Se0DataIndex +
                                            tv5Se1DataIndex +
                                            tv5Se2DataIndex +
                                            tv5Se3DataIndex +
                                            tv5Se4DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 6U;
                subeventResult.startACLConnEvent = 0x00B6;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF8;
                subeventResult.procedureDoneStatus = 0x00;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 8;
                subeventResult.pData = &tv5[tv5Se0DataIndex +
                                            tv5Se1DataIndex +
                                            tv5Se2DataIndex +
                                            tv5Se3DataIndex +
                                            tv5Se4DataIndex +
                                            tv5Se5DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                break;
            }

            case 6U: /* Test vector 6 - multiple_subevents_mode2_mode1 (7 subevents) */
            {
                if (AppLocalization_AllocLocalData(deviceId) == NULL)
                {
                    break;
                }

                pResultData[deviceId].selectedTxPower = 0x7F;
                pResultData[deviceId].subeventIndex = 0U;
                csSubeventResultEvent_t subeventResult = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .startACLConnEvent = 0x008F,
                    .procedureCounter = 0U,
                    .frequencyCompensation = 0x0000,
                    .referencePowerLevel = (int8_t)0xF7,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x00,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 18,
                    .pData = tv6,
                };
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 1U;
                subeventResult.startACLConnEvent = 0x0090;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF9;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 18;
                subeventResult.pData = &tv6[tv6Se0DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 2U;
                subeventResult.startACLConnEvent = 0x0091;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF9;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 18;
                subeventResult.pData = &tv6[tv6Se0DataIndex +
                                            tv6Se1DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 3U;
                subeventResult.startACLConnEvent = 0x0092;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF9;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 18;
                subeventResult.pData = &tv6[tv6Se0DataIndex +
                                            tv6Se1DataIndex +
                                            tv6Se2DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 4U;
                subeventResult.startACLConnEvent = 0x0093;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF9;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 18;
                subeventResult.pData = &tv6[tv6Se0DataIndex +
                                            tv6Se1DataIndex +
                                            tv6Se2DataIndex +
                                            tv6Se3DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 5U;
                subeventResult.startACLConnEvent = 0x0094;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF8;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 18;
                subeventResult.pData = &tv6[tv6Se0DataIndex +
                                            tv6Se1DataIndex +
                                            tv6Se2DataIndex +
                                            tv6Se3DataIndex +
                                            tv6Se4DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 6U;
                subeventResult.startACLConnEvent = 0x0095;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF9;
                subeventResult.procedureDoneStatus = 0x00;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 15;
                subeventResult.pData = &tv6[tv6Se0DataIndex +
                                            tv6Se1DataIndex +
                                            tv6Se2DataIndex +
                                            tv6Se3DataIndex +
                                            tv6Se4DataIndex +
                                            tv6Se5DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                break;
            }

            case 7U: /* Test vector 7 - multiple_subevents_mode2_only (6 subevents) */
            {
                if (AppLocalization_AllocLocalData(deviceId) == NULL)
                {
                    break;
                }

                pResultData[deviceId].selectedTxPower = 0x7F;
                pResultData[deviceId].subeventIndex = 0U;
                csSubeventResultEvent_t subeventResult = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .startACLConnEvent = 0x0091,
                    .procedureCounter = 0U,
                    .frequencyCompensation = 0x0000,
                    .referencePowerLevel = (int8_t)0xF7,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x00,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 0x12,
                    .pData = tv7,
                };
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 1U;
                subeventResult.startACLConnEvent = 0x0092;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xFB;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 0x12;
                subeventResult.pData = &tv7[tv7Se0DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 2U;
                subeventResult.startACLConnEvent = 0x0093;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF6;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 0x12;
                subeventResult.pData = &tv7[tv7Se0DataIndex +
                                            tv7Se1DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 3U;
                subeventResult.startACLConnEvent = 0x0094;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF8;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 0x12;
                subeventResult.pData = &tv7[tv7Se0DataIndex +
                                            tv7Se1DataIndex +
                                            tv7Se2DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 4U;
                subeventResult.startACLConnEvent = 0x0095;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF9;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 0x12;
                subeventResult.pData = &tv7[tv7Se0DataIndex +
                                            tv7Se1DataIndex +
                                            tv7Se2DataIndex +
                                            tv7Se3DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 5U;
                subeventResult.startACLConnEvent = 0x0096;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF6;
                subeventResult.procedureDoneStatus = 0x00;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 0x09;
                subeventResult.pData = &tv7[tv7Se0DataIndex +
                                            tv7Se1DataIndex +
                                            tv7Se2DataIndex +
                                            tv7Se3DataIndex +
                                            tv7Se4DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                break;
            }

            case 11U: /* Fall-through */
                      /* Test vector 11 - long procedure 1 aborted, start another procedure */
            {
                if (AppLocalization_AllocLocalData(deviceId) == NULL)
                {
                    break;
                }

                pResultData[deviceId].selectedTxPower = 0x7F;
                pResultData[deviceId].subeventIndex = 0U;
                csSubeventResultEvent_t subeventResult = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .startACLConnEvent = 0x0091,
                    .procedureCounter = 0U,
                    .frequencyCompensation = 0x0000,
                    .referencePowerLevel = (int8_t)0xF7,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x00,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 0x12,
                    .pData = tv7,
                };
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 1U;
                subeventResult.startACLConnEvent = 0x0092;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xFB;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 0x12;
                subeventResult.pData = &tv7[tv7Se0DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 2U;
                subeventResult.startACLConnEvent = 0x0093;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF6;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 0x12;
                subeventResult.pData = &tv7[tv7Se0DataIndex +
                                            tv7Se1DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 3U;
                subeventResult.startACLConnEvent = 0x0094;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF8;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 0x12;
                subeventResult.pData = &tv7[tv7Se0DataIndex +
                                            tv7Se1DataIndex +
                                            tv7Se2DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 4U;
                subeventResult.startACLConnEvent = 0x0095;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF9;
                subeventResult.procedureDoneStatus = 0x0F;
                subeventResult.subeventDoneStatus = 0x0F;
                subeventResult.abortReason = 0x0F;
                subeventResult.numStepsReported = 0x12;
                subeventResult.pData = &tv7[tv7Se0DataIndex +
                                            tv7Se1DataIndex +
                                            tv7Se2DataIndex +
                                            tv7Se3DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                /* This subevent is aborted and the next is sent */
            }

            case 8U: /* Test vector 8 - multiple_subevents_mode3_only (8 subevents) */
            {
                if (AppLocalization_AllocLocalData(deviceId) == NULL)
                {
                    break;
                }

                pResultData[deviceId].selectedTxPower = 0x7F;
                pResultData[deviceId].subeventIndex = 0U;
                csSubeventResultEvent_t subeventResult = {
                    .deviceId = deviceId,
                    .configId = 1U,
                    .startACLConnEvent = 0x00B1,
                    .procedureCounter = 0U,
                    .frequencyCompensation = 0x0000,
                    .referencePowerLevel = (int8_t)0xF9,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x00,
                    .abortReason = 0x00,
                    .numAntennaPaths = 1U,
                    .numStepsReported = 14,
                    .pData = tv8,
                };
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 1U;
                subeventResult.startACLConnEvent = 0x00B2;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF8;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 14;
                subeventResult.pData = &tv8[tv8Se0DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 2U;
                subeventResult.startACLConnEvent = 0x00B3;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xFA;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 14;
                subeventResult.pData = &tv8[tv8Se0DataIndex +
                                            tv8Se1DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 3U;
                subeventResult.startACLConnEvent = 0x00B4;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF6;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 14;
                subeventResult.pData = &tv8[tv8Se0DataIndex +
                                            tv8Se1DataIndex +
                                            tv8Se2DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 4U;
                subeventResult.startACLConnEvent = 0x00B5;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF6;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 14;
                subeventResult.pData = &tv8[tv8Se0DataIndex +
                                            tv8Se1DataIndex +
                                            tv8Se2DataIndex +
                                            tv8Se3DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 5U;
                subeventResult.startACLConnEvent = 0x00B6;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF6;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 14;
                subeventResult.pData = &tv8[tv8Se0DataIndex +
                                            tv8Se1DataIndex +
                                            tv8Se2DataIndex +
                                            tv8Se3DataIndex +
                                            tv8Se4DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 6U;
                subeventResult.startACLConnEvent = 0x00B7;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF9;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 14;
                subeventResult.pData = &tv8[tv8Se0DataIndex +
                                            tv8Se1DataIndex +
                                            tv8Se2DataIndex +
                                            tv8Se3DataIndex +
                                            tv8Se4DataIndex +
                                            tv8Se5DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 7U;
                subeventResult.startACLConnEvent = 0x00B8;
                subeventResult.frequencyCompensation = 0x0000;
                subeventResult.referencePowerLevel = (int8_t)0xF9;
                subeventResult.procedureDoneStatus = 0x00;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 11;
                subeventResult.pData = &tv8[tv8Se0DataIndex +
                                            tv8Se1DataIndex +
                                            tv8Se2DataIndex +
                                            tv8Se3DataIndex +
                                            tv8Se4DataIndex +
                                            tv8Se5DataIndex +
                                            tv8Se6DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                break;
            }

            case 9U: /* Test vector 9  - filter_input_mode0123_Ini_sounding_nAP4 (4 subevents) */
            {
                if (AppLocalization_AllocLocalData(deviceId) == NULL)
                {
                    break;
                }

                pResultData[deviceId].selectedTxPower = 0x7F;
                pResultData[deviceId].subeventIndex = 0U;
                csSubeventResultEvent_t subeventResult = {
                    .deviceId = deviceId,
                    .configId = 0U,
                    .startACLConnEvent = 0x0084,
                    .procedureCounter = 0U,
                    .frequencyCompensation = 0xFF20,
                    .referencePowerLevel = (int8_t)0x7F,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x01,
                    .abortReason = 0x00,
                    .numAntennaPaths = 4U,
                    .numStepsReported = 15,
                    .pData = tv9,
                };
                (void)processCsResultsEvent(&subeventResult);

                csSubeventResultContinueEvent_t subeventResultContinue = {
                    .deviceId = deviceId,
                    .configId = 0U,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x00,
                    .abortReason = 0x00,
                    .numAntennaPaths = 4U,
                    .numStepsReported = 10,
                    .pData = &tv9[tv9Se0DataIndex],
                };
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                pResultData[deviceId].subeventIndex = 1U;
                subeventResult.startACLConnEvent = 0x0137;
                subeventResult.frequencyCompensation = 0xFF27;
                subeventResult.referencePowerLevel = (int8_t)0xFF;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x01;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 11;
                subeventResult.pData = &tv9[tv9Se0DataIndex +
                                            tv9Se0c0DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                subeventResultContinue.procedureDoneStatus = 0x01;
                subeventResultContinue.subeventDoneStatus = 0x00;
                subeventResultContinue.abortReason = 0x00;
                subeventResultContinue.numAntennaPaths = 4U;
                subeventResultContinue.numStepsReported = 4;
                subeventResultContinue.pData = &tv9[tv9Se0DataIndex +
                                                    tv9Se0c0DataIndex +
                                                    tv9Se1DataIndex];
                (void)processCsResultsContinueEvent(&subeventResultContinue);

                pResultData[deviceId].subeventIndex = 2U;
                subeventResult.startACLConnEvent = 0x0137;
                subeventResult.frequencyCompensation = 0xFF1E;
                subeventResult.referencePowerLevel = (int8_t)0xFF;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 6;
                subeventResult.pData = &tv9[tv9Se0DataIndex +
                                            tv9Se0c0DataIndex +
                                            tv9Se1DataIndex +
                                            tv9Se1c0DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 3U;
                subeventResult.startACLConnEvent = 0x0137;
                subeventResult.frequencyCompensation = 0xFF1E;
                subeventResult.referencePowerLevel = (int8_t)0xFF;
                subeventResult.procedureDoneStatus = 0x00;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 4;
                subeventResult.pData = &tv9[tv9Se0DataIndex +
                                            tv9Se0c0DataIndex +
                                            tv9Se1DataIndex +
                                            tv9Se1c0DataIndex +
                                            tv9Se2DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                break;
            }

            case 10U: /* Test vector 10 (case 9)  - filter_input_mode0123_Ini_sounding_nAP4 (4 subevents) */
                      /* (Merged Continue Events) */
            {
                if (AppLocalization_AllocLocalData(deviceId) == NULL)
                {
                    break;
                }

                pResultData[deviceId].selectedTxPower = 0x7F;
                pResultData[deviceId].subeventIndex = 0U;
                csSubeventResultEvent_t subeventResult = {
                    .deviceId = deviceId,
                    .configId = 0U,
                    .startACLConnEvent = 0x0084,
                    .procedureCounter = 0U,
                    .frequencyCompensation = 0xFF20,
                    .referencePowerLevel = (int8_t)0x7F,
                    .procedureDoneStatus = 0x01,
                    .subeventDoneStatus = 0x00,
                    .abortReason = 0x00,
                    .numAntennaPaths = 4U,
                    .numStepsReported = 15 + 10,
                    .pData = tv9,
                };
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 1U;
                subeventResult.startACLConnEvent = 0x0137;
                subeventResult.frequencyCompensation = 0xFF27;
                subeventResult.referencePowerLevel = (int8_t)0xFF;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 11 + 4;
                subeventResult.pData = &tv9[tv9Se0DataIndex +
                                            tv9Se0c0DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 2U;
                subeventResult.startACLConnEvent = 0x0137;
                subeventResult.frequencyCompensation = 0xFF1E;
                subeventResult.referencePowerLevel = (int8_t)0xFF;
                subeventResult.procedureDoneStatus = 0x01;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 6;
                subeventResult.pData = &tv9[tv9Se0DataIndex +
                                            tv9Se0c0DataIndex +
                                            tv9Se1DataIndex +
                                            tv9Se1c0DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                pResultData[deviceId].subeventIndex = 3U;
                subeventResult.startACLConnEvent = 0x0137;
                subeventResult.frequencyCompensation = 0xFF1E;
                subeventResult.referencePowerLevel = (int8_t)0xFF;
                subeventResult.procedureDoneStatus = 0x00;
                subeventResult.subeventDoneStatus = 0x00;
                subeventResult.abortReason = 0x00;
                subeventResult.numStepsReported = 4;
                subeventResult.pData = &tv9[tv9Se0DataIndex +
                                            tv9Se0c0DataIndex +
                                            tv9Se1DataIndex +
                                            tv9Se1c0DataIndex +
                                            tv9Se2DataIndex];
                (void)processCsResultsEvent(&subeventResult);

                break;
            }

            /* reader tests */
            case 103U: /* Fall-through */
            {
                /* PTS sends data starting with procedure count 0x100 for this test */
                pResultData[deviceId].procedureCounter = 0x100;
            }

            case 101U:
            {
                if (AppLocalization_AllocLocalData(deviceId) == NULL)
                {
                    break;
                }
                break;
            }

    #if defined (gRasRREQ_d) && (gRasRREQ_d == 1U)
            case 102U:
            {
                (void)RasClient_SendRasCommand(deviceId, abortOperationOpCode_c,
                                               0U, 0U, 0U, gAntennaPathFilterAllowAll_c);

                 break;
            }
    #endif

            default:
            {
                /* MISRA C-2012 Rule 16.4: Ensure default case is present */
            }
        }
    }

    if (bSendDataReady)
    {
        (void)Ras_SendDataReady(deviceId);
    }
    
    if (bDataOverwrittenTest == TRUE)
    {
        OSA_TimeDelay(2000);
        (void)Ras_SendDataOverwritten(deviceId);
    }
}
#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */
