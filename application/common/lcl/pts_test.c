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
#include "pts_test.h"

#if defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1)

#include "app_localization.h"
#include "ranging_interface.h"
#include "fsl_component_mem_manager.h"

#include <string.h>

/************************************************************************************
*************************************************************************************
* Private memory
*************************************************************************************
************************************************************************************/
static ptsTestCallbacks_t mPtsCallbacks = {NULL, NULL, NULL};

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
static void PtsTest_SwitchRtData(deviceId_t deviceId, uint16_t cccd, uint8_t *pStep);
static bleResult_t PtsTest_ConfigureRas(deviceId_t deviceId, uint16_t cccd, ptsCharId_t charId);
static void PtsTest_ReadChar(deviceId_t deviceId, ptsCharId_t charId);

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
 * \brief  Wrapper over the registered SwitchRtData callback.
 ********************************************************************************** */
static void PtsTest_SwitchRtData(deviceId_t deviceId, uint16_t cccd, uint8_t *pStep)
{
    if (mPtsCallbacks.pfSwitchRtData != NULL)
    {
        mPtsCallbacks.pfSwitchRtData(deviceId, cccd, pStep);
    }
}

/*! *********************************************************************************
 * \brief  Wrapper over the registered ConfigureRas callback.
 *         Returns gBleSuccess_c if no callback is registered so the calling
 *         state machine is able to progress even without an application handler.
 ********************************************************************************** */
static bleResult_t PtsTest_ConfigureRas(deviceId_t deviceId, uint16_t cccd, ptsCharId_t charId)
{
    bleResult_t result = gBleSuccess_c;

    if (mPtsCallbacks.pfConfigureRas != NULL)
    {
        result = mPtsCallbacks.pfConfigureRas(deviceId, cccd, charId);
    }

    return result;
}

/*! *********************************************************************************
 * \brief  Wrapper over the registered ReadChar callback.
 ********************************************************************************** */
static void PtsTest_ReadChar(deviceId_t deviceId, ptsCharId_t charId)
{
    if (mPtsCallbacks.pfReadChar != NULL)
    {
        mPtsCallbacks.pfReadChar(deviceId, charId);
    }
}

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
 * \brief  Register the application-specific callback table.
 ********************************************************************************** */
void PtsTest_RegisterCallbacks(const ptsTestCallbacks_t *pCallbacks)
{
    if (pCallbacks != NULL)
    {
        mPtsCallbacks = *pCallbacks;
    }
}

/*! *********************************************************************************
 * \brief  Execute one PTS step for the RAP Requester role.
 *         Shared by loc_reader and ncp_loc_reader.
 *
 * \param[in]  pParam  Heap-allocated C-string with the PTS test-case ID.
 *                     Ownership is transferred; the buffer is freed on return.
 ********************************************************************************** */
void PtsTest_RunRapRequester(void *pParam)
{
    const deviceId_t deviceId = 0U;
    char *arg = (char *)pParam;

    if (arg == NULL)
    {
        return;
    }

    if (strcmp(arg, "RAP/REQ/RRD/BV-01-C") == 0)
    {
        static uint8_t testStep = 0U;

        switch (testStep)
        {
            case 0U:
            {
                AppLocalization_RunPtsTest(deviceId, 101U, 0U);
                testStep++;
                break;
            }
            case 1U: /* Fall-through */
            case 3U:
            {
                /* Disable Real-Time data transfer */
                PtsTest_SwitchRtData(deviceId, gCccdEmpty_c, &testStep);
                break;
            }
            case 2U:
            {
                /* Enable Real-Time data transfer */
                PtsTest_SwitchRtData(deviceId, gCccdIndication_c, &testStep);
                break;
            }
            case 4U:
            {
                testStep = 0U;
                break;
            }
            default:
            {
                ; /* No action required */
            }
            break;
        }
    }
    else if (strcmp(arg, "RAP/REQ/RRD/BV-02-C") == 0)
    {
        static uint8_t testStep = 0U;

        switch (testStep)
        {
            case 0U:  /* Fall-through */
            case 4U:  /* Fall-through */
            case 8U:  /* Fall-through */
            case 12U:
            {
                /* Disable Real-Time data transfer */
                PtsTest_SwitchRtData(deviceId, gCccdEmpty_c, &testStep);
                break;
            }
            case 1U:  /* Fall-through */
            case 9U:
            {
                /* Enable Real-Time data transfer notification */
                PtsTest_SwitchRtData(deviceId, gCccdNotification_c, &testStep);
                break;
            }
            /* Round 1 cli commands:
             * filter 0 0x0028 0
             * filter 0 0x0028 1
             * filter 0 0x0031 0
             * filter 0 0x0031 1
             * filter 0 0x004E 0
             * filter 0 0x004E 1
             * filter 0 0x15A3 0
             * filter 0 0x15A3 1
             *
             * Round 2 cli commands:
             * filter 0 0x0020 0
             * filter 0 0x0020 1
             * filter 0 0x0021 0
             * filter 0 0x0021 1
             * filter 0 0x000A 0
             * filter 0 0x000A 1
             * filter 0 0x0423 0
             * filter 0 0x0423 1
             */
            case 5U:  /* Fall-through */
            case 13U:
            {
                /* Enable Real-Time data transfer indication */
                PtsTest_SwitchRtData(deviceId, gCccdIndication_c, &testStep);
                break;
            }
            case 2U:  /* Fall-through */
            case 3U:  /* Fall-through */
            case 6U:  /* Fall-through */
            case 7U:  /* Fall-through */
            case 10U: /* Fall-through */
            case 11U: /* Fall-through */
            case 14U: /* Fall-through */
            case 15U:
            {
                AppLocalization_RunPtsTest(deviceId, 101U, 0U);
                testStep++;
                break;
            }
            case 16U:
            {
                testStep = 0U;
                break;
            }
            default:
            {
                ; /* No action required */
            }
            break;
        }
    }
    else if ((strcmp(arg, "RAP/REQ/RRD/BI-01-C") == 0) ||
             (strcmp(arg, "RAP/REQ/RRD/BI-02-C") == 0))
    {
        AppLocalization_RunPtsTest(deviceId, 1U, 0U);
    }
    else if ((strcmp(arg, "RAP/REQ/ORD/BV-01-C") == 0) ||
             (strcmp(arg, "RAP/REQ/ORD/BV-03-C") == 0))
    {
        AppLocalization_RunPtsTest(deviceId, 101U, 0U);
    }
    else if (strcmp(arg, "RAP/REQ/ORD/BV-02-C") == 0)
    {
        static uint8_t testStep = 0U;
        bleResult_t result = gBleSuccess_c;

        switch (testStep)
        {
            case 0U:  /* Fall-through */
            case 3U:  /* Fall-through */
            case 6U:  /* Fall-through */
            case 9U:
            {
                /* Disable On-Demand data transfer */
                result = PtsTest_ConfigureRas(deviceId, gCccdEmpty_c, gPtsCharOnDemandData_c);

                if (result == gBleSuccess_c)
                {
                    testStep++;
                }
                break;
            }
            /* Round 1 cli commands:
             * filter 0 0x0028 0
             * filter 0 0x0028 1
             * filter 0 0x0031 0
             * filter 0 0x0031 1
             * filter 0 0x004E 0
             * filter 0 0x004E 1
             * filter 0 0x15A3 0
             * filter 0 0x15A3 1
             *
             * Round 2 cli commands:
             * filter 0 0x0020 0
             * filter 0 0x0020 1
             * filter 0 0x0021 0
             * filter 0 0x0021 1
             * filter 0 0x000A 0
             * filter 0 0x000A 1
             * filter 0 0x0423 0
             * filter 0 0x0423 1
             */
            case 1U:  /* Fall-through */
            case 2U:  /* Fall-through */
            case 4U:  /* Fall-through */
            case 5U:  /* Fall-through */
            case 7U:  /* Fall-through */
            case 8U:  /* Fall-through */
            case 10U: /* Fall-through */
            case 11U:
            {
                AppLocalization_RunPtsTest(deviceId, 101U, 0U);
                testStep++;
                break;
            }
            case 12U:
            {
                testStep = 0U;
                break;
            }
            default:
            {
                ; /* No action required */
            }
            break;
        }
    }
    else if (strcmp(arg, "RAP/REQ/ORD/BV-04-C") == 0)
    {
        static uint8_t testStep = 0U;

        switch (testStep)
        {
            case 0U:
            {
                AppLocalization_RunPtsTest(deviceId, 103U, 0U);
                testStep++;
                break;
            }
            case 1U:
            {
                AppLocalization_RunPtsTest(deviceId, 102U, 0U);
                testStep++;
                break;
            }
            case 2U:
            {
                testStep = 0U;
                break;
            }
            default:
            {
                ; /* No action required */
            }
            break;
        }
    }
    else if (strcmp(arg, "RAP/REQ/ORD/BV-05-C") == 0)
    {
        /* Enable Data Ready optional notifications */
        (void)PtsTest_ConfigureRas(deviceId, gCccdNotification_c, gPtsCharDataReady_c);
    }
    else if (strcmp(arg, "RAP/REQ/ORD/BV-06-C") == 0)
    {
        static uint8_t testStep = 0U;
        bleResult_t result = gBleSuccess_c;

        switch (testStep)
        {
            case 0U:
            {
                /* Enable Data Ready optional notifications */
                result = PtsTest_ConfigureRas(deviceId, gCccdNotification_c, gPtsCharDataReady_c);

                if (result == gBleSuccess_c)
                {
                    testStep++;
                }
                break;
            }
            case 1U:
            {
                /* Read Data Ready characteristic */
                PtsTest_ReadChar(deviceId, gPtsCharDataReady_c);
                testStep++;
                break;
            }
            case 2U:
            {
                testStep = 0U;
                break;
            }
            default:
            {
                ; /* No action required */
            }
            break;
        }
    }
    else if (strcmp(arg, "RAP/REQ/ORD/BV-08-C") == 0)
    {
        /* Enable Data Overwritten optional notifications */
        (void)PtsTest_ConfigureRas(deviceId, gCccdNotification_c, gPtsCharDataOverwritten_c);
    }
    else if (strcmp(arg, "RAP/REQ/ORD/BV-09-C") == 0)
    {
        static uint8_t testStep = 0U;
        bleResult_t result = gBleSuccess_c;

        switch (testStep)
        {
            case 0U:
            {
                /* Enable Data Overwritten optional notifications */
                result = PtsTest_ConfigureRas(deviceId, gCccdNotification_c, gPtsCharDataOverwritten_c);

                if (result == gBleSuccess_c)
                {
                    testStep++;
                }
                break;
            }
            case 1U:
            {
                /* Read Data Overwritten characteristic */
                PtsTest_ReadChar(deviceId, gPtsCharDataOverwritten_c);
                testStep++;
                break;
            }
            case 2U:
            {
                testStep = 0U;
                break;
            }
            default:
            {
                ; /* No action required */
            }
            break;
        }
    }
    else if (strcmp(arg, "RAP/REQ/ORD/BI-03-C") == 0)
    {
        AppLocalization_RunPtsTest(deviceId, 101U, 0U);
    }
    else
    {
        ; /* MISRA C:2012 Rule 15.7 */
    }

    (void)MEM_BufferFree(pParam);
}

/*! *********************************************************************************
 * \brief  Execute one PTS step for the RAS Server role.
 *         Used by loc_user_device.
 *
 * \param[in]  pParam  Heap-allocated C-string with the PTS test-case ID.
 *                     Ownership is transferred; the buffer is freed on return.
 ********************************************************************************** */
void PtsTest_RunRasServer(void *pParam)
{
    const deviceId_t deviceId = 0U;
    char *arg = (char *)pParam;

    if (arg == NULL)
    {
        return;
    }

    if ((strcmp(arg, "RAS/SR/RCO/BV-02-C") == 0) ||
        (strcmp(arg, "RAS/SR/RCO/BV-03-C") == 0) ||
        (strcmp(arg, "RAS/SR/RCO/BV-04-C") == 0) ||
        (strcmp(arg, "RAS/SR/RCO/BV-08-C") == 0) ||
        (strcmp(arg, "RAS/SR/RCO/BV-09-C") == 0))
    {
        /* BIT0 - Send a Data Ready for On-Demand */
        /* BIT1 - Send a Data Overwritten          */
        AppLocalization_RunPtsTest(deviceId, 1U, BIT0 | BIT1);
    }
    else if (strcmp(arg, "RAS/SR/RRD/BV-04-C") == 0)
    {
        AppLocalization_RunPtsTest(deviceId, 11U, 0U);
    }
    else if ((strcmp(arg, "RAS/SR/RCO/BV-05-C") == 0)   ||
             (strcmp(arg, "RAS/SR/RCO/BV-06-C") == 0)   ||
             (strcmp(arg, "RAS/SR/RCO/BV-07-C") == 0)   ||
             (strcmp(arg, "RAS/SR/RCP/BV-05-C") == 0)   ||
             (strcmp(arg, "RAP/RES/RSPF/BV-02-C") == 0))
    {
        /* BIT0 - Send a Data Ready for On-Demand */
        AppLocalization_RunPtsTest(deviceId, 1U, BIT0);
    }
    else if (strcmp(arg, "RAS/SR/RCP/BV-02-C") == 0)
    {
        /* BIT0 - Send a Data Ready for On-Demand */
        AppLocalization_RunPtsTest(deviceId, 8U, BIT0);
    }
    else if ((strcmp(arg, "RAP/RES/RSPF/BV-02-C_LT2") == 0) ||
             (strcmp(arg, "RAP/RES/RSPF/BV-03-C_LT2") == 0) ||
             (strcmp(arg, "RAP/RES/RSPF/BV-05-C_LT2") == 0))
    {
        /* BIT0 - Send a Data Ready for On-Demand */
        AppLocalization_RunPtsTest(deviceId + 1U, 2U, BIT0);
    }
    else if ((strcmp(arg, "RAS/SR/RRD/BV-03-C") == 0) ||
             (strcmp(arg, "RAS/SR/RRD/BV-05-C") == 0) ||
             (strcmp(arg, "RAP/RES/RSPF/BV-03-C") == 0))
    {
        AppLocalization_RunPtsTest(deviceId, 1U, 0U);
    }
    else if (strcmp(arg, "RAS/SR/RRD/BV-01-C") == 0)
    {
        AppLocalization_RunPtsTest(deviceId, 8U, 0U);
    }
    else if ((strcmp(arg, "RAS/SR/RCP/BV-01-C") == 0) ||
             (strcmp(arg, "RAS/SR/RCP/BV-03-C") == 0) ||
             (strcmp(arg, "RAS/SR/SPE/BI-05-C") == 0) ||
             (strcmp(arg, "RAS/SR/SPE/BI-06-C") == 0) ||
             (strcmp(arg, "RAS/SR/SPE/BI-07-C") == 0) ||
             (strcmp(arg, "RAS/SR/SPE/BI-10-C") == 0) ||
             (strcmp(arg, "RAP/RES/RSPF/BV-05-C") == 0))
    {
        uint8_t rasPreferenceValue = Ras_GetDataSendPreference(deviceId);
        /* Force the use of indications instead of notifications */
        rasPreferenceValue |= BIT0;
        (void)Ras_SetDataSendPreference(deviceId, rasPreferenceValue);

        /* BIT0 - Send a Data Ready for On-Demand */
        AppLocalization_RunPtsTest(deviceId, 8U, BIT0);
    }
    else if (strcmp(arg, "RAS/SR/RRD/BV-02-C") == 0)
    {
        AppLocalization_RunPtsTest(deviceId, 9U, 0U);
    }
    else if ((strcmp(arg, "RAS/SR/RCP/BV-04-C") == 0) ||
             (strcmp(arg, "RAP/RES/RSPF/BV-04-C") == 0))
    {
        /* BIT0 - Send a Data Ready for On-Demand */
        AppLocalization_RunPtsTest(deviceId, 9U, BIT0);
    }
    else if (strcmp(arg, "RAP/RES/RSPF/BV-04-C_LT2") == 0)
    {
        /* BIT0 - Send a Data Ready for On-Demand */
        AppLocalization_RunPtsTest(deviceId + 1U, 9U, BIT0);
    }
    else if (strcmp(arg, "0") == 0)
    {
        /* BIT0 - Send a Data Ready for On-Demand */
        AppLocalization_RunPtsTest(deviceId, 1U, BIT0);
    }
    else if (strcmp(arg, "1") == 0)  { AppLocalization_RunPtsTest(deviceId, 1U,  0U); }
    else if (strcmp(arg, "2") == 0)  { AppLocalization_RunPtsTest(deviceId, 2U,  0U); }
    else if (strcmp(arg, "3") == 0)  { AppLocalization_RunPtsTest(deviceId, 3U,  0U); }
    else if (strcmp(arg, "4") == 0)  { AppLocalization_RunPtsTest(deviceId, 4U,  0U); }
    else if (strcmp(arg, "5") == 0)  { AppLocalization_RunPtsTest(deviceId, 5U,  0U); }
    else if (strcmp(arg, "6") == 0)  { AppLocalization_RunPtsTest(deviceId, 6U,  0U); }
    else if (strcmp(arg, "7") == 0)  { AppLocalization_RunPtsTest(deviceId, 7U,  0U); }
    else if (strcmp(arg, "8") == 0)  { AppLocalization_RunPtsTest(deviceId, 8U,  0U); }
    else if (strcmp(arg, "9") == 0)  { AppLocalization_RunPtsTest(deviceId, 9U,  0U); }
    else if (strcmp(arg, "10") == 0) { AppLocalization_RunPtsTest(deviceId, 10U, 0U); }
    else if (strcmp(arg, "11") == 0) { AppLocalization_RunPtsTest(deviceId, 11U, 0U); }
    else
    {
        ; /* MISRA C:2012 Rule 15.7 */
    }

    (void)MEM_BufferFree(pParam);
}

#endif /* defined(gRasRapPtsTest_d) && (gRasRapPtsTest_d == 1) */
