/*! *********************************************************************************
* \addtogroup Battery Service
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2025 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "gap_interface.h"
#include "host_battery_interface.h"
#include "host_hsdk_interface.h"

/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

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

/* Last pointer to ServiceConfig used in response handlers */
static basConfig_t *mpLastServiceConfig = NULL;

/* Last handle to be used when sending notifications to multiple devices */
static uint16_t mLastSendNotificationsHandle = gGattDbInvalidHandle_d;

/* Module variable to keep count of the devices notified from Bas_SendNotifications */
static uint8_t mClientId = 0U;

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
 * \brief        Callback function to handle the Events for the Request.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void gattDbFindCharValueHandleInServiceHandler
(
    bleEvtContainer_t* pMsg
);

/*! *********************************************************************************
 * \brief        Callback function to handle the Events for the Request.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void Bas_SendNotifications
(
    bleEvtContainer_t* pMsg
);

/*! *********************************************************************************
 * \brief        Callback function to handle the Events for the Request.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void gattDbFindCccdHandleForCharValueHandleHandler
(
    bleEvtContainer_t* pMsg
);

/*! *********************************************************************************
 * \brief        Callback function to handle the Events for the Request.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void gapCheckNotificationStatusHandler
(
    bleEvtContainer_t* pMsg
);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

bleResult_t Bas_Start(basConfig_t *pServiceConfig)
{
    uint8_t clientId = 0U;

    /* reset all slots for valid subscribers */
    for (clientId = 0; clientId < pServiceConfig->validSubscriberListSize; clientId++)
    {
        pServiceConfig->aValidSubscriberList[clientId] = FALSE;
    }

    /* Record initial battery level measurement */
    Bas_RecordBatteryMeasurement((void*)pServiceConfig);

    return gBleSuccess_c;
}

bleResult_t Bas_Stop(basConfig_t *pServiceConfig)
{
    uint8_t clientId = 0U;

    /* reset all slots for valid subscribers */
    for (clientId = 0; clientId < pServiceConfig->validSubscriberListSize; clientId++)
    {
        pServiceConfig->aValidSubscriberList[clientId] = FALSE;
    }

    return gBleSuccess_c;
}

bleResult_t Bas_Subscribe(basConfig_t *pServiceConfig, deviceId_t clientDeviceId)
{
    bleResult_t result = gBleSuccess_c;

    if (clientDeviceId >= pServiceConfig->validSubscriberListSize)
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        pServiceConfig->aValidSubscriberList[clientDeviceId] = TRUE;
    }

    return result;
}

bleResult_t Bas_Unsubscribe(basConfig_t *pServiceConfig, deviceId_t clientDeviceId)
{
    bleResult_t result = gBleSuccess_c;

    if (clientDeviceId >= pServiceConfig->validSubscriberListSize)
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        pServiceConfig->aValidSubscriberList[clientDeviceId] = FALSE;
    }

    return result;
}

void Bas_RecordBatteryMeasurement(void *pParam)
{
    basConfig_t *pServiceConfig = (basConfig_t *)pParam;
    bleUuid_t uuid = {.uuid16 = gBleSig_BatteryLevel_d};
    GATTDBFindCharValueHandleInServiceRequest_t req = {};

    /* Save reference from last pServiceConfig to be used in Observer Callback */
    mpLastServiceConfig = pServiceConfig;

    /* Populate request */
    req.ServiceHandle = pServiceConfig->serviceHandle;
    req.UuidType = Uuid16Bits;
    Utils_PackTwoByteValue(uuid.uuid16, req.Uuid.Uuid16Bits);

    /* Register callback to be used when the expected indication is received */
    RegisterRemovableObserver(GATTDBConfirm_FSCI_ID,
                              gattDbFindCharValueHandleInServiceHandler);

    /* Register callback to be used when the expected indication is received */
    RegisterRemovableObserver(GATTDBFindCharValueHandleInServiceIndication_FSCI_ID,
                              gattDbFindCharValueHandleInServiceHandler);

    /* Get handle of characteristic */
    GATTDBFindCharValueHandleInServiceRequest(&req, gFsciInterface_c);
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
 * \brief        Callback function to handle the Events for the Request.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void gattDbFindCharValueHandleInServiceHandler
(
    bleEvtContainer_t* pMsg
)
{
    switch ((bleFsciIds_t)pMsg->id)
    {
        case GATTDBConfirm_FSCI_ID:
        {
            if (pMsg->Data.GATTDBConfirm.Status != GATTDBConfirm_Status_gBleSuccess_c)
            {
                /* If confirm has status different than success
                 * the targeted indication will not be received */
                RemoveObserver(GATTDBFindCharValueHandleInServiceIndication_FSCI_ID,
                               gattDbFindCharValueHandleInServiceHandler);
            }
            break;
        }

        case GATTDBFindCharValueHandleInServiceIndication_FSCI_ID:
        {
            GATTDBWriteAttributeRequest_t req = {};
            uint16_t handle = pMsg->Data.GATTDBFindCharValueHandleInServiceIndication.CharValueHandle;

            /* Store the module handle to be used in callbacks */
            mLastSendNotificationsHandle = handle;

            if (mpLastServiceConfig != NULL)
            {
                /* Populate request */
                req.Handle = handle;
                req.ValueLength = sizeof(mpLastServiceConfig->batteryLevel);
                req.Value = &mpLastServiceConfig->batteryLevel;

                /* Register callback to be used when the expected indication is received */
                RegisterRemovableObserver(GATTDBConfirm_FSCI_ID,
                                          Bas_SendNotifications);

                /* Update characteristic value and send notification */
                GATTDBWriteAttributeRequest(&req, gFsciInterface_c);
            }
            break;
        }

        default:
        {
            ; /* No action required */
            break;
        }
    }
}

/*! *********************************************************************************
 * \brief        Callback function to handle the Events for the Request.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void Bas_SendNotifications
(
    bleEvtContainer_t* pMsg
)
{
    GATTDBFindCccdHandleForCharValueHandleRequest_t req = {};
    
    if ((bleFsciIds_t)pMsg->id == GATTDBConfirm_FSCI_ID &&
        pMsg->Data.GATTDBConfirm.Status == GATTDBConfirm_Status_gBleSuccess_c)
    {
        /* Populate request */
        req.CharValueHandle = mLastSendNotificationsHandle;

        /* Register callback to be used when the expected indication is received */
        RegisterRemovableObserver(GATTDBConfirm_FSCI_ID,
                                  gattDbFindCccdHandleForCharValueHandleHandler);

        /* Register callback to be used when the expected indication is received */
        RegisterRemovableObserver(GATTDBFindCccdHandleForCharValueHandleIndication_FSCI_ID,
                                  gattDbFindCccdHandleForCharValueHandleHandler);
        
        /* Reset the counter for sending the notifications to peers */
        mClientId = 0U;

        /* Get handle of CCCD */
        GATTDBFindCccdHandleForCharValueHandleRequest(&req, gFsciInterface_c);
    }
}

/*! *********************************************************************************
 * \brief        Callback function to handle the Events for the Request.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void gattDbFindCccdHandleForCharValueHandleHandler
(
    bleEvtContainer_t* pMsg
)
{
    static GAPCheckNotificationStatusRequest_t req = {};

    switch ((bleFsciIds_t)pMsg->id)
    {
        case GATTDBConfirm_FSCI_ID:
        {
            if (pMsg->Data.GATTDBConfirm.Status != GATTDBConfirm_Status_gBleSuccess_c)
            {
                /* If GAP Confirm has status different than success
                 * the targeted indication will not be received */
                RemoveObserver(GATTDBFindCccdHandleForCharValueHandleIndication_FSCI_ID,
                               gattDbFindCccdHandleForCharValueHandleHandler);
            }
            break;
        }

        case GATTDBFindCccdHandleForCharValueHandleIndication_FSCI_ID:
        {
            /* Populate request */
            req.Handle = pMsg->Data.GATTDBFindCccdHandleForCharValueHandleIndication.CccdHandle;
        }
        /* Fall through */
        case GATTConfirm_FSCI_ID:
        {
            while ((mClientId < mpLastServiceConfig->validSubscriberListSize) &&
                   !(mpLastServiceConfig->aValidSubscriberList[mClientId]))
            {
                /* Increment counter in case the current subscriber is not valid */
                mClientId++;
            }

            if ((mClientId < mpLastServiceConfig->validSubscriberListSize) &&
                (mpLastServiceConfig->aValidSubscriberList[mClientId]))
            {
                /* Populate request */
                req.DeviceId = mClientId;

                /* Register callback to be used when the expected indication is received */
                RegisterRemovableObserver(GAPConfirm_FSCI_ID,
                                          gapCheckNotificationStatusHandler);

                RegisterRemovableObserver(GAPCheckNotificationStatusIndication_FSCI_ID,
                                          gapCheckNotificationStatusHandler);

                /* Check that for a (deviceId, handle) pair, notifications are configured */
                GAPCheckNotificationStatusRequest(&req, gFsciInterface_c);
            }

            break;
        }

        default:
        {
            ; /* No action required */
            break;
        }
    }
}

/*! *********************************************************************************
 * \brief        Callback function to handle the Events for the Request.
 *
 * \param[in]    pContainer              Pointer to bleEvtContainer_t.
 ********************************************************************************** */
static void gapCheckNotificationStatusHandler
(
    bleEvtContainer_t* pMsg
)
{
    switch ((bleFsciIds_t)pMsg->id)
    {
        case GAPConfirm_FSCI_ID:
        {
            if (pMsg->Data.GAPConfirm.Status != GAPConfirm_Status_gBleSuccess_c)
            {
                /* If confirm has status different than success
                 * the targeted indication will not be received */
                RemoveObserver(GAPCheckNotificationStatusIndication_FSCI_ID,
                               gapCheckNotificationStatusHandler);
            }
            break;
        }

        case GAPCheckNotificationStatusIndication_FSCI_ID:
        {
            bool_t isNotifActive = pMsg->Data.GAPCheckNotificationStatusIndication.IsActive;
            GATTServerSendNotificationRequest_t req = {};

            /* Populate request */
            req.DeviceId = mClientId;
            req.Handle = mLastSendNotificationsHandle;

            if (mClientId < (uint8_t)UINT8_MAX)
            {
                mClientId++;
            }

            if (TRUE == isNotifActive)
            {
                /* Register callback to be used when the expected indication is received */
                (void)RegisterRemovableObserver(GATTConfirm_FSCI_ID,
                                          gattDbFindCccdHandleForCharValueHandleHandler);

                /* Send notification with the battery level */
                GATTServerSendNotificationRequest(&req, gFsciInterface_c);
            }
            break;
        }

        default:
        {
            ; /* No action required */
            break;
        }
    }
}
/*! *********************************************************************************
* @}
********************************************************************************** */
