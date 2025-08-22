/*! *********************************************************************************
* Copyright 2021, 2024-2025 NXP
*
* \file
*
* This is a source file for the common application advertising code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "app_conn.h"
#include "app_advertiser.h"
#include "fsl_component_panic.h"
#include "fwk_messaging.h"
#include "fwk_mem_manager.h"

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static void App_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent);
static void App_AdvertiserHandler(gapGenericEventType_t  eventType);

#if defined(gReencryptAdvDataOnRpaChange_d) && (gReencryptAdvDataOnRpaChange_d == 1U)
static uint16_t getAdvDataLen(const gapAdvertisingData_t * pAdvData);
#endif
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static appAdvertisingParams_t *mpAdvParams = NULL;
static appExtAdvertisingParams_t *mpExtAdvParams = NULL;

#if defined(gReencryptAdvDataOnRpaChange_d) && (gReencryptAdvDataOnRpaChange_d == 1U)
static uint8_t mKey[gcEadKeySize_c] = {0x0U};
static uint8_t mIv[gcEadIvSize_c] = {0x0U};
static gapAdStructure_t mEncryptedAdvScanStruct;

static gapAdvertisingData_t mEncryptedAdvertisingData =
{
     1U,
    (void *)&mEncryptedAdvScanStruct
};
#endif
/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn          bleResult_t BluetoothLEHost_StartAdvertising(
*                 appAdvertisingParams_t   *pAdvParams,
                  gapAdvertisingCallback_t pfAdvertisingCallback,
                  gapConnectionCallback_t  pfConnectionCallback
              )
*\brief       Set advertising data, set advertising parameters and start advertising.
*
*\param  [in] pAdvParams             Pointer to the structure containing the
*                                    advertising parameters.
*\param  [in] pfAdvertisingCallback  Callback used by the application to receive
*                                    advertising events.
*                                    Can be NULL.
*\param  [in] pfConnectionCallback   Callback used by the application to receive
*                                    connection events.
*                                    Can be NULL.
*
*\return      bleResult_t            Result of the operation.
********************************************************************************** */
bleResult_t BluetoothLEHost_StartAdvertising(
    appAdvertisingParams_t *pAdvParams,
    gapAdvertisingCallback_t pfAdvertisingCallback,
    gapConnectionCallback_t pfConnectionCallback
)
{
    pfAdvCallback = pfAdvertisingCallback;
    pfConnCallback = pfConnectionCallback;

    pfAdvertiserHandler = App_AdvertiserHandler;
    mpAdvParams = pAdvParams;
    return Gap_SetAdvertisingParameters(pAdvParams->pGapAdvParams);
}

/*! *********************************************************************************
*\fn          bleResult_t BluetoothLEHost_StartExtAdvertising(
*                 appExtAdvertisingParams_t *pExtAdvParams,
                  gapAdvertisingCallback_t  pfAdvertisingCallback,
                  gapConnectionCallback_t   pfConnectionCallback
              )
*\brief       Set advertising data, set advertising parameters and start extended
*             advertising.
*
*\param  [in] pAdvParams             Pointer to the structure containing the
*                                    advertising parameters.
*\param  [in] pfAdvertisingCallback  Callback used by the application to receive
*                                    advertising events.
*                                    Can be NULL.
*\param  [in] pfConnectionCallback   Callback used by the application to receive
*                                    connection events.
*                                    Can be NULL.
*
*\return      bleResult_t            Result of the operation.
********************************************************************************** */
bleResult_t BluetoothLEHost_StartExtAdvertising(
    appExtAdvertisingParams_t *pExtAdvParams,
    gapAdvertisingCallback_t pfAdvertisingCallback,
    gapConnectionCallback_t pfConnectionCallback
)
{
    pfAdvCallback = pfAdvertisingCallback;
    pfConnCallback = pfConnectionCallback;

    pfAdvertiserHandler = App_AdvertiserHandler;
    mpExtAdvParams = pExtAdvParams;
    return Gap_SetExtAdvertisingParameters(pExtAdvParams->pGapExtAdvParams);
}

#if defined(gReencryptAdvDataOnRpaChange_d) && (gReencryptAdvDataOnRpaChange_d == 1U)
/*! *************************************************************************************
*\fn           bleResult_t BluetoothLEHost_SetEadKeyMaterial(uint8_t *pKey, uint8_t *pIv)
*\brief        Set the key and initialization vector used to encrypt advertising data.
*
*\param  [in]  pKey     Pointer to 16-byte key.
*\param  [in]  pIV      Pointer to 8-byte initialization vector.
*
*\return       None
************************************************************************************** */
void BluetoothLEHost_SetEadKeyMaterial
(
    uint8_t *pKey,
    uint8_t *pIv
)
{
    if (pKey != NULL)
    {
        FLib_MemCpy(mKey, pKey, gcEadKeySize_c);
    }
    if (pIv != NULL)
    {
        FLib_MemCpy(mIv, pIv, gcEadIvSize_c);
    }
}

/*! *************************************************************************************
*\fn           bleResult_t BluetoothLEHost_ReencryptAdvertisingData(uint8_t advHandle,
*              const gapAdvertisingData_t *pAdvData)
*
*\brief        Re-encrypt the advertising data for the set identified by advHandle using
*              the previously set key and IV and a new randomizer. Should be called by the
*              application when the RPA for the set changes (on gRandomAddressSet_c
*              generic event).
*
*\param  [in]  advHandle    Handle identifying the advertising set
*\param  [in]  pAdvData    Pointer to advertising data to be encrypted
*
*\return       bleResult_t
************************************************************************************** */
bleResult_t BluetoothLEHost_ReencryptAdvertisingData
(
    uint8_t advHandle,
    const gapAdvertisingData_t *pAdvData
)
{
    bleResult_t result = gBleSuccess_c;

    if ((advHandle >= gMaxAdvSets_c) || (pAdvData == NULL))
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        uint16_t advLength = getAdvDataLen(pAdvData) + gcEadRandomizerSize_c +  gcEadMicSize_c;
        uint8_t *pOutput = MEM_BufferAlloc(advLength);

        if (pOutput != NULL)
        {
            result = Gap_EncryptAdvertisingData(pAdvData, mKey, mIv, pOutput);
            if (result == gBleSuccess_c)
            {
                mEncryptedAdvScanStruct.length = (uint8_t)advLength;
                mEncryptedAdvScanStruct.adType = gAdEncryptedAdvertisingData_c;
                mEncryptedAdvScanStruct.aData = pOutput;
                result = Gap_SetExtAdvertisingData(advHandle, &mEncryptedAdvertisingData, NULL);
            }
            (void)MEM_BufferFree(pOutput);
        }
        else
        {
            result = gBleOutOfMemory_c;
        }
    }

    return result;
}
#endif /* gReencryptAdvDataOnRpaChange_d */

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\private
*\fn          void App_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent)
*\brief       Sends the GAP Advertising Event triggered by the Host Stack to the
*             application.
*
*\param  [in] pAdvertisingEvent      Pointer to the advertising event.
*
*\retval      void.
********************************************************************************** */
static void App_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent)
{
    appMsgFromHost_t *pMsgIn = NULL;

    pMsgIn = MSG_Alloc(GetRelAddr(appMsgFromHost_t,msgData) + sizeof(gapAdvertisingEvent_t));

    if (pMsgIn != NULL)
    {
        pMsgIn->msgType = (uint32_t)gAppGapAdvertisementMsg_c;
        pMsgIn->msgData.advMsg.eventType = pAdvertisingEvent->eventType;
        pMsgIn->msgData.advMsg.eventData = pAdvertisingEvent->eventData;

        /* Put message in the Host Stack to App queue */
        if (MSG_QueueAddTail(&mHostAppInputQueue, pMsgIn) == kMSG_Success)
        {
            /* Signal application */
            (void)OSA_EventSet(mAppEvent, gAppEvtMsgFromHostStack_c);
        }
        else
        {
            /* Free the message if queueing failed */
            (void)MSG_Free(pMsgIn);
        }
    }

    return;
}

/*! *********************************************************************************
*\private
*\fn          void App_AdvertiserHandler(gapGenericEventType_t  eventType)
*\brief       This function handles the advertising events state machine.
*
*\param  [in] eventType              Received advertising event type.
*
*\retval      void.
********************************************************************************** */
static void App_AdvertiserHandler(gapGenericEventType_t  eventType)
{
    switch (eventType)
    {
        case gAdvertisingParametersSetupComplete_c:
        {
            (void)Gap_SetAdvertisingData(mpAdvParams->pGapAdvData,
                                         mpAdvParams->pScanResponseData);
        }
        break;

        case gAdvertisingDataSetupComplete_c:
        {
            (void)Gap_StartAdvertising(App_AdvertisingCallback,
                                       App_ConnectionCallback);
        }
        break;

        case gAdvertisingSetupFailed_c:
        {
            panic(0,0,0,0);
        }
        break;

        case gExtAdvertisingParametersSetupComplete_c:
        {
            (void)Gap_SetExtAdvertisingData(mpExtAdvParams->handle,
                                            mpExtAdvParams->pGapAdvData,
                                            mpExtAdvParams->pScanResponseData);
        }
        break;

        case gExtAdvertisingDataSetupComplete_c:
        {
#if defined(gBLE60_DecisionBasedAdvertisingFilteringSupport_d) && (gBLE60_DecisionBasedAdvertisingFilteringSupport_d == TRUE)
            if((mpExtAdvParams->pGapDecisionData == NULL) || ((mpExtAdvParams->pGapExtAdvParams->extAdvProperties & (bleAdvRequestProperties_t)gAdvUseDecisionPDU_c) == (bleAdvRequestProperties_t)0x00U))
            {
#endif /* gBLE60_DecisionBasedAdvertisingFilteringSupport_d */
                (void)Gap_StartExtAdvertising(App_AdvertisingCallback,
                                              App_ConnectionCallback,
                                              mpExtAdvParams->handle,
                                              mpExtAdvParams->duration,
                                              mpExtAdvParams->maxExtAdvEvents);
#if defined(gBLE60_DecisionBasedAdvertisingFilteringSupport_d) && (gBLE60_DecisionBasedAdvertisingFilteringSupport_d == TRUE)
            }
            else
            {
                (void)Gap_SetExtAdvertisingDecisionData(mpExtAdvParams->handle, mpExtAdvParams->pGapDecisionData);
            }
#endif /* gBLE60_DecisionBasedAdvertisingFilteringSupport_d */
        }
        break;
#if defined(gBLE60_DecisionBasedAdvertisingFilteringSupport_d) && (gBLE60_DecisionBasedAdvertisingFilteringSupport_d == TRUE)
        case gExtAdvertisingDecisionDataSetupComplete_c:
        {
            (void)Gap_StartExtAdvertising(App_AdvertisingCallback,
                                          App_ConnectionCallback,
                                          mpExtAdvParams->handle,
                                          mpExtAdvParams->duration,
                                          mpExtAdvParams->maxExtAdvEvents);
        }
        break;
#endif /* gBLE60_DecisionBasedAdvertisingFilteringSupport_d */
        case gExtAdvertisingSetRemoveComplete_c:
        {
            /* TBD */
        }
        break;

        default:
        {
            ; /* MISRA rule 16.4 */
        }
        break;
    }
}

#if defined(gReencryptAdvDataOnRpaChange_d) && (gReencryptAdvDataOnRpaChange_d == 1U)
/*! *********************************************************************************
*\private
*\fn          uint16_t getAdvDataLen(gapAdvertisingData_t * pAdvData)
*\brief       Helper function that calculates the length of given gapAdvertisingData_t.
*
*\param  [in] pAdvData   Pointer to structure containing advertising data.
*
*\retval      void.
********************************************************************************** */
static uint16_t getAdvDataLen(const gapAdvertisingData_t * pAdvData)
{
    uint16_t len = 0U;
    uint32_t i;

    if (pAdvData != NULL)
    {
        for (i = 0U; i < pAdvData->cNumAdStructures; i++)
        {
            len += 1U + (uint16_t)pAdvData->aAdStructures[i].length;
        }
    }

    return len;
}
#endif /* gReencryptAdvDataOnRpaChange_d */