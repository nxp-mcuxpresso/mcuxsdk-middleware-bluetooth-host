/*! *********************************************************************************
* Copyright 2022-2023, 2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "FsciInterface.h"
#include "FunctionLib.h"
#include "fsl_component_panic.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_messaging.h"
#include "host_hsdk_interface.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef enum hsdkObserverType_tag{
    hsdkObserverTypeInvalid_c = 0,                /* Observer no longer in use. */
    hsdkObserverTypermvAfterFirstMatch_c = 1,     /* The observer is removed after the
                                                     first match found */
}hsdkObserverType_t;

typedef struct observerInfo_tag
{
    hsdkObserverType_t observerType;
    bleFsciIds_t fsciIds;
    eventCallback_t callback;
}observerInfo_t;

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static eventCallback_t   mpfHSDKMessageCallback = NULL;
static eventCallback_t   mpfAppHSDKMessageCallback = NULL;

/* An array that holds the available observers */
static observerInfo_t maObserverList[gObserverListSize_c] = { (hsdkObserverType_t)0 };

/* Variable that holds the available number of in-use observers */
static uint8_t mObserverListIndex = 0;

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief        Registers callback in aplication for hsdk events.
*
* \param[in]    pCallback           Aplication callback function for hsdk messages
*
* \return       Result of the operation
*
********************************************************************************** */
gHSDKStatus_t RegistergAppCallbackHSDK
(
    eventCallback_t    pCallback
)
{
    mpfAppHSDKMessageCallback = pCallback;
    return gHSDKSuccess_c;
}


/*! *********************************************************************************
* \brief        Registers callback for hsdk events.
*
* \param[in]    pCallback           Callback function for hsdk messages
*
* \return       Result of the operation
*
********************************************************************************** */
gHSDKStatus_t RegistergHSDKCallback
(
    eventCallback_t    pCallback
)
{
    mpfHSDKMessageCallback = pCallback;
    return gHSDKSuccess_c;
}

/*****************************************************************************
* \brief        Registers an observer that will be removed after the first match.
*
* \param[in]    fsciIds    Event identifier that is awaited.
* \param[in]    pCallback  Callback where to direct the event.
*
* Return value: True if the observer has been registered with success
*               False otherwise.
*****************************************************************************/
bool_t RegisterRemovableObserver(bleFsciIds_t fsciIds, eventCallback_t pCallback)
{
    for (uint8_t i = 0; i < gObserverListSize_c; i++)
    {
        if(maObserverList[i].observerType == hsdkObserverTypeInvalid_c)
        {
            maObserverList[i].observerType = hsdkObserverTypermvAfterFirstMatch_c;
            maObserverList[i].fsciIds = fsciIds;
            maObserverList[i].callback = pCallback;
            mObserverListIndex++;
            return TRUE;
        }
    }
    return FALSE;
}

/*****************************************************************************
* \brief        Mark observer as invalid.
*
* \param[in]    fsciIds    Event identifier that is awaited.
* \param[in]    pCallback  Callback where to direct the event.
*
* Return value: True if the observer has been marked as invalid with success
*               False otherwise.
*****************************************************************************/
bool_t RemoveObserver(bleFsciIds_t fsciIds, eventCallback_t pCallback)
{
    for (uint8_t i = 0; i < gObserverListSize_c; i++)
    {
        if(maObserverList[i].observerType != hsdkObserverTypeInvalid_c)
        {
            if((maObserverList[i].fsciIds == fsciIds) &&
               (maObserverList[i].callback == pCallback))
            {
                maObserverList[i].observerType = hsdkObserverTypeInvalid_c;
                mObserverListIndex--;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*! *********************************************************************************
* \brief   This function registers the FSCI handlers for the enabled HSDK.
*
* \param[in]    fsciInterfaceId        The interface on which data should be
                                       received and sent.
*
********************************************************************************** */
void HsdkInit(uint32_t fsciInterfaceId)
{
#if defined(gFsciIncluded_c) && (gFsciIncluded_c > 0)
    /* Register L2CAP CB command handler */
    if(FSCI_RegisterOpGroup(gFsciBleL2capCbOpcodeGroup_c,
                              gFsciMonitorMode_c,
                              fsciCbHandler,
                              NULL,
                              fsciInterfaceId) != gFsciSuccess_c)
    {
        panic(0, 0, 0, 0);
    }

    /* Register GATT command handler */
    if(FSCI_RegisterOpGroup(gFsciBleGattOpcodeGroup_c,
                              gFsciMonitorMode_c,
                              fsciCbHandler,
                              NULL,
                              fsciInterfaceId) != gFsciSuccess_c)
    {
        panic(0, 0, 0, 0);
    }

    /* Register GATT Database (application) command handler */
    if(FSCI_RegisterOpGroup(gFsciBleGattDbAppOpcodeGroup_c,
                              gFsciMonitorMode_c,
                              fsciCbHandler,
                              NULL,
                              fsciInterfaceId) != gFsciSuccess_c)
    {
        panic(0, 0, 0, 0);
    }

    /* Register GAP command handler */
    if(FSCI_RegisterOpGroup(gFsciBleGapOpcodeGroup_c,
                              gFsciMonitorMode_c,
                              fsciCbHandler,
                              NULL,
                              fsciInterfaceId) != gFsciSuccess_c)
    {
        panic(0, 0, 0, 0);
    }
#endif
}


void fsciCbHandler(void* pData, void* param, uint32_t fsciInterface)
{
    if (mpfHSDKMessageCallback != NULL)
    {
       bleEvtContainer_t *container = (bleEvtContainer_t*)MSG_Alloc(sizeof(bleEvtContainer_t));
       if (container != NULL)
       {
           KHC_BLE_RX_MsgHandler(pData, container, (uint8_t)fsciInterface);
           mpfHSDKMessageCallback(container);
       }
    }

    (void)MEM_BufferFree(pData);
}

void HSDK_transmitPayload
(
    void *arg,            /* Optional argument passed to the function */
    uint8_t og,           /* FSCI operation group */
    uint8_t oc,           /* FSCI operation code */
    void *msg,            /* Pointer to payload */
    uint16_t msgLen,      /* Payload length */
    uint8_t fsciInterface /* FSCI interface ID */
 )
{
#if defined(gFsciIncluded_c) && (gFsciIncluded_c > 0)
    FSCI_transmitPayload(og, oc, msg, msgLen, fsciInterface);
#endif
}

void App_HandleObservedHSDKMessageInput(bleEvtContainer_t* pMsg)
{
    bool_t removedObserverFlag = FALSE;
    bool_t matchFoundFlag = FALSE;
    eventCallback_t savedCallback = NULL;

    /* Verify if any observers are in use. */
    if (mObserverListIndex > 0U)
    {
        for (uint8_t i = 0; i < gObserverListSize_c; i++)
        {
            if (maObserverList[i].observerType != hsdkObserverTypeInvalid_c)
            {
                if (maObserverList[i].fsciIds == (bleFsciIds_t)pMsg->id)
                {
                    matchFoundFlag = TRUE;
                    if (maObserverList[i].observerType == hsdkObserverTypermvAfterFirstMatch_c)
                    {   /* Observer that will be removed after the first match */
                        savedCallback = maObserverList[i].callback;
                        /* Mark observer as removed */
                        maObserverList[i].observerType = hsdkObserverTypeInvalid_c;
                        removedObserverFlag = TRUE;
                    }
                    else
                    {
                        maObserverList[i].callback(pMsg);
                    }
                    break;
                }
            }
        }

        if (removedObserverFlag == TRUE)
        {
            /* Decreases the number of observers in use */
            mObserverListIndex--;
        }

        if (matchFoundFlag == TRUE)
        {
            if (savedCallback != NULL)
            {
                savedCallback(pMsg);
            }

            /* Match found, no need to call the application HSDK message handler. */
        }
    }

    if (!matchFoundFlag)
    {
        /* No observer found call application HSDK message handler. */
        mpfAppHSDKMessageCallback(pMsg);
    }
}

/*! *********************************************************************************
* @}
********************************************************************************** */
