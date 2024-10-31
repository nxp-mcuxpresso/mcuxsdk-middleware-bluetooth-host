/*! *********************************************************************************
 * \addtogroup AUTO
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2023-2024 NXP
*
*
* \file app_a2b.c
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "EmbeddedTypes.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_messaging.h"
#include "fwk_seclib.h"
#include "app_a2b.h"
#include "app_conn.h"
#include "ble_conn_manager.h"

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static appA2BEventCb_t mpfAppEventCb = NULL;
static appA2BA2ACommInterfaceCb_t mpfAppA2AInterfaceCb = NULL;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

ecdhPublicKey_t a2aPeerPubKey = {0};
ecdhPublicKey_t a2aPubKey = {0};
ecdhPrivateKey_t a2aPrvKey = {0};
ecdhDhKey_t gE2EkeyData;

#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
bool_t gA2ALocalIrkSet = FALSE;
gapGenericEvent_t *gpBleHostInitComplete = NULL;
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
static bleResult_t A2B_DeriveKey(void);
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 1)
static bleResult_t A2A_InitiatorSyncIrk(void);
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 1) */
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
static bleResult_t A2A_SyncIrk(uint8_t *pE2EIrkBlob);
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */
static void notifyRemoteDevice(uint8_t cmdId, uint16_t len, uint8_t *pData);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn           bleResult_t A2B_Init(appA2BEventCb_t pfAppEventCb,
*                                   appA2BA2ACommInterfaceCb_t pfAppA2AInterfaceCb)
*\brief        Trigger the A2B feature initialization by generating the public key
*               used for the Edgelock to Edgelock key derivation.
*
*\param  [in]  pfAppEventCb             Application callback functions for A2B events.
*\param  [in]  pfAppA2AInterfaceCb      Communication callback for A2B data.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************** */
bleResult_t A2B_Init
(
    appA2BEventCb_t pfAppEventCb,
    appA2BA2ACommInterfaceCb_t pfAppA2AInterfaceCb
)
{
    bleResult_t status = gBleSuccess_c;
    secResultType_t secStatus = gSecSuccess_c;
    
    if ((pfAppEventCb == NULL) ||
        (pfAppA2AInterfaceCb == NULL))
    {
        status = gBleInvalidParameter_c;
    }
    else
    {
        mpfAppEventCb = pfAppEventCb;
        mpfAppA2AInterfaceCb = pfAppA2AInterfaceCb;
        secStatus = ECDH_P256_GenerateKeys(&a2aPubKey, &a2aPrvKey);
        
        if (secStatus != gSecSuccess_c)
        {
            status = gBleUnexpectedError_c;
        }
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 1)
        else
        {
            notifyRemoteDevice(gA2BPublicKeyCommandOpCode_c, gA2BPublicKeyCommandLen_c, a2aPubKey.raw);
        }
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 1) */
    }
    
    return status;
}

/*! *********************************************************************************
*\fn           void A2B_ProcessA2ACommand(uint8_t cmdId, uint32_t cmdLen,
*                                           uint8_t *pCmdData)
*\brief        Process A2B commands.
*
*\param  [in]  cmdId        Command identifier.
*\param  [in]  cmdLen       Command length.
*\param  [in]  pCmdData     Command data.
*
*\return       None
********************************************************************************** */
void A2B_ProcessA2ACommand
(
    uint8_t     cmdId,
    uint32_t    cmdLen,
    uint8_t     *pCmdData
)
{
    appA2BError_t error = mAppA2B_UnexpectedError_c;
    
    switch (cmdId)
    {
        
        case gA2BPublicKeyCommandOpCode_c:
        {
            bleResult_t status = gBleSuccess_c;
            FLib_MemCpy(a2aPeerPubKey.raw, pCmdData, 64U);
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
            notifyRemoteDevice(gA2BPublicKeyCommandOpCode_c, gA2BPublicKeyCommandLen_c, a2aPubKey.raw);
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */
            status = A2B_DeriveKey();
            
            if (mpfAppEventCb != NULL)
            {
                if (status == gBleSuccess_c)
                {
                    mpfAppEventCb(mAppA2B_E2EKeyDerivationComplete_c, NULL);
                }
                else
                {
                    error = mAppA2B_E2EKeyDerivationFailiure_c;
                    mpfAppEventCb(mAppA2B_Error_c, &error);
                }
            }
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
            if ((FALSE == gA2ALocalIrkSet) && (status == gBleSuccess_c))
            {
                notifyRemoteDevice(gA2BRequestE2EIrkBlobCommandOpCode_c, 0U, NULL);
            }
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */
        }
        break;
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 1)
        case gA2BRequestE2EIrkBlobCommandOpCode_c:
        {
            bleResult_t status = gBleSuccess_c;
            
            status = A2A_InitiatorSyncIrk();
            
            if (mpfAppEventCb != NULL)
            {
                if (status == gBleSuccess_c)
                {
                    mpfAppEventCb(mAppA2B_E2ELocalIrkSyncComplete_c, NULL);
                }
                else
                {
                    error = mAppA2B_E2ELocalIrkSyncFailiure_c;
                    mpfAppEventCb(mAppA2B_Error_c, &error);
                }
            }
        }
        break;
#endif /* (gA2BInitiator_d) && (gA2BInitiator_d == 1) */
        
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
        case gA2BE2EIrkBlobCommandOpCode_c:
        {
            bleResult_t status = gBleSuccess_c;
            uint8_t aE2EIrkBlob[gcSmpMaxIrkBlobSize_c] = {0};
            
            FLib_MemCpy(aE2EIrkBlob, pCmdData, gcSmpMaxIrkBlobSize_c);
            
            if (FALSE == gA2ALocalIrkSet)
            {
                status = A2A_SyncIrk(aE2EIrkBlob);
                
                if (gBleSuccess_c == status)
                {
                    gA2ALocalIrkSet = TRUE;
                    
                    if (NULL != gpBleHostInitComplete)
                    {
                        BleConnManager_GenericEvent(gpBleHostInitComplete);
                        BleConnManager_GapCommonConfig();
                        MEM_BufferFree(gpBleHostInitComplete);
                        gpBleHostInitComplete = NULL;
                    }
                }
                else
                {
                    error = mAppA2B_E2ELocalIrkSyncFailiure_c;
                }
            }
            
            if (mpfAppEventCb != NULL)
            {
                if (status == gBleSuccess_c)
                {
                    mpfAppEventCb(mAppA2B_E2ELocalIrkSyncComplete_c, NULL);
                }
                else
                {
                    error = mAppA2B_E2ELocalIrkSyncFailiure_c;
                    mpfAppEventCb(mAppA2B_Error_c, &error);
                }
            }
        }
        break;
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */
        default:
        {
            /* */
        }
        break;
    }
}

/*! *********************************************************************************
*\fn           bleResult_t A2B_FreeE2EKey(void)
*\brief        Free the EdgeLock to EdgeLock key
*
*\param  [in]  None.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************** */
bleResult_t A2B_FreeE2EKey(void)
{
    bleResult_t result = gBleSuccess_c;
    secResultType_t secStatus = gSecError_c;

    if (gSecLibFunctions.pfECDH_P256_FreeE2EKeyData != NULL)
    {
        secStatus = gSecLibFunctions.pfECDH_P256_FreeE2EKeyData(&gE2EkeyData);
    }
    if (secStatus != gSecSuccess_c)
    {
        result = gBleUnexpectedError_c;
    }
    
    return result;
}
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn           static bleResult_t A2B_DeriveKey(void)
*\brief        Generates the Edgelock to Edgelock key used to encryt the blobs
*              transfered from one anchor to the other.
*
*\param  [in]  None
*
*\return       bleResult_t    Result of the operation.
********************************************************************************** */
static bleResult_t A2B_DeriveKey(void)
{
    bleResult_t status = gBleSuccess_c;
    secResultType_t secStatus = gSecError_c;
    if (gSecLibFunctions.pfECDH_P256_ComputeA2BKey != NULL)
    {
        secStatus = gSecLibFunctions.pfECDH_P256_ComputeA2BKey(&a2aPeerPubKey, &gE2EkeyData);
    }
    if (secStatus != gSecSuccess_c)
    {
        status = gBleUnexpectedError_c;
    }
    
    return status;
}

#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 1)
/*! *********************************************************************************
*\fn           static bleResult_t A2A_InitiatorSyncIrk(void)
*\brief        Send the local IRK to the other anchor as an ecrypted blob..
*
*\param  [in]  None
*
*\return       bleResult_t    Result of the operation.
********************************************************************************** */
static bleResult_t A2A_InitiatorSyncIrk(void)
{
    bleResult_t status = gBleSuccess_c;
    secResultType_t secStatus = gSecError_c;
    bleLocalKeysBlob_t localIrk = {0};
    uint8_t aE2EIrkBlob[gcSmpMaxIrkBlobSize_c] = {0};
    
    status = App_NvmReadLocalIRK(&localIrk);
    
    if ((gBleSuccess_c == status) && (localIrk.keyGenerated == TRUE))
    {
        /* Obtain E2E blob for local IRK. */
        if (gSecLibFunctions.pfSecLib_ExportA2BBlob != NULL)
        {
            secStatus = gSecLibFunctions.pfSecLib_ExportA2BBlob(localIrk.pKey, gSecElkeBlob_c, aE2EIrkBlob);
        }
        if (gSecSuccess_c == secStatus)
        {
            /* Send IRK blob to the other Anchor. */
            notifyRemoteDevice(gA2BE2EIrkBlobCommandOpCode_c, gA2BE2EIrkBlobCommandLen_c, aE2EIrkBlob);
        }
        else
        {
            status = gBleUnexpectedError_c;
        }
    }
    else
    {
        status = gBleUnexpectedError_c;
    }
    
    return status;
}
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 1) */

#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
/*! *********************************************************************************
*\fn           static bleResult_t A2A_SyncIrk(uint8_t *pE2EIrkBlob)
*\brief        Process the IRK blob from the Initiator anchor and set the local IRK.
*
*\param  [in]  pE2EIrkBlob  EdgeLock to EdgeLock blob for the IRK.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************** */
static bleResult_t A2A_SyncIrk(uint8_t *pE2EIrkBlob)
{
    bleResult_t status = gBleUnexpectedError_c;
    secResultType_t secStatus = gSecError_c;
    bleLocalKeysBlob_t localIrk = {0};
    
    /* Obtain ELKE blob for local IRK. */
    if (gSecLibFunctions.pfSecLib_ImportA2BBlob != NULL)
    {
        secStatus = gSecLibFunctions.pfSecLib_ImportA2BBlob(pE2EIrkBlob, gSecElkeBlob_c, localIrk.pKey);
    }
    if (gSecSuccess_c == secStatus)
    {
        /* Overwrite local ELKE IRK blob.in NVM */
        localIrk.keyGenerated = TRUE;
        
        status = App_NvmWriteLocalIRK(&localIrk);
    }
    
    return status;
}
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */

/*! ********************************************************************************************************************
*\fn           static void notifyRemoteDevice(uint8_t cmdId, uint8_t len, uint8_t *pData)
*\brief        Send command to the remote device involved in the connection handover process.
*
*\param  [in]  cmdId    Handover command identifier.
*\param  [in]  len      Data payload length.
*\param  [in]  pData    Data
*
*\return       None
********************************************************************************************************************* */
static void notifyRemoteDevice(uint8_t cmdId, uint16_t len, uint8_t *pData)
{
    if (mpfAppA2AInterfaceCb != NULL)
    {
        mpfAppA2AInterfaceCb(gA2BCommandsOpGroup_c, cmdId, len, pData);
    }
}

#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */