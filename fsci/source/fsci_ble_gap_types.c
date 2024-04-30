/*! *********************************************************************************
* \addtogroup FSCI_BLE
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2024 NXP
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
#include "fsci_ble_gap_types.h"
#include "ble_general.h"
#include "ble_config.h"
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
#include "SecLib.h"
#endif

#include "fsci_ble_gap_handlers.h"
#include "fwk_seclib.h"
#if gFsciIncluded_c && gFsciBleGapLayerEnabled_d

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

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

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

gapSmpKeys_t* fsciBleGapAllocSmpKeysForBuffer(uint8_t* pBuffer)
{
    bool_t          bLtkIncluded        = FALSE;
    uint8_t         ltkSize             = 0;
    bool_t          bIrkIncluded        = FALSE;
    uint8_t         irkSize             = 0;
    bool_t          bCsrkIncluded       = FALSE;
    uint8_t         csrkSize            = 0;
    uint8_t         randSize            = 0;
    bool_t          bAddressIncluded    = FALSE;
    uint8_t         addressSize         = 0;
    gapSmpKeys_t*   pSmpKeys;

    /* Verify if LTK is included or not */
    fsciBleGetBoolValueFromBuffer(bLtkIncluded, pBuffer);

    if(TRUE == bLtkIncluded)
    {
        /* Get LTK size from buffer */
        fsciBleGetUint8ValueFromBuffer(ltkSize, pBuffer);
    }

    /* Go to bIrkIncluded */
    pBuffer += ltkSize;
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
    if(ltkSize > 0U)
    {
        ltkSize = gcSmpMaxBlobSize_c;
    } 
#endif /* (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U)) */    

    /* Verify if IRK is included or not */
    fsciBleGetBoolValueFromBuffer(bIrkIncluded, pBuffer);

    if(TRUE == bIrkIncluded)
    {
        irkSize = gcSmpIrkSize_c;
    }

    /* Go to bIrkIncluded */
    pBuffer += irkSize;

    /* Verify if CSRK is included or not */
    fsciBleGetBoolValueFromBuffer(bCsrkIncluded, pBuffer);

    if(TRUE == bCsrkIncluded)
    {
        csrkSize = gcSmpCsrkSize_c;
    }

    /* Go to bRandIncluded */
    pBuffer += csrkSize;

    /* Random value is included only if LTK is included */
    if(TRUE == bLtkIncluded)
    {
        /* Get random value size from buffer */
        fsciBleGetUint8ValueFromBuffer(randSize, pBuffer);
    }

    /* Go to bAddressIncluded */
    pBuffer += randSize + ((TRUE == bLtkIncluded) ? sizeof(uint16_t) : 0U);

    /* Address is included only if IRK is included */
    if(TRUE == bIrkIncluded)
    {
        /* Verify if address is included or not */
        fsciBleGetBoolValueFromBuffer(bAddressIncluded, pBuffer);

        if(TRUE == bAddressIncluded)
        {
            addressSize =  gcBleDeviceAddressSize_c;
        }
    }

    /* Allocate buffer for SMP keys */
    pSmpKeys = (gapSmpKeys_t*)MEM_BufferAlloc(sizeof(gapSmpKeys_t) + (uint32_t)ltkSize + irkSize + csrkSize + randSize + addressSize);

    if(NULL == pSmpKeys)
    {
        /* No memory */
        return NULL;
    }

    /* Set pointers in gapSmpKeys_t structure */
    pSmpKeys->aLtk      = (uint8_t*)pSmpKeys + sizeof(gapSmpKeys_t);
    pSmpKeys->aIrk      = pSmpKeys->aLtk + ltkSize;
    pSmpKeys->aCsrk     = pSmpKeys->aIrk + irkSize;
    pSmpKeys->aRand     = pSmpKeys->aCsrk + csrkSize;
    pSmpKeys->aAddress  = pSmpKeys->aRand + randSize;

    if(FALSE == bLtkIncluded)
    {
        /* No LTK */
        pSmpKeys->aLtk  = NULL;
        /* No random value */
        pSmpKeys->aRand = NULL;
    }

    if(FALSE == bIrkIncluded)
    {
        /* No IRK */
        pSmpKeys->aIrk  = NULL;
    }

    if(FALSE == bCsrkIncluded)
    {
        /* No CSRK */
        pSmpKeys->aCsrk = NULL;
    }

    if(0U == randSize)
    {
        /* No random value */
        pSmpKeys->aRand = NULL;
    }

    if(FALSE == bAddressIncluded)
    {
        /* No address */
        pSmpKeys->aAddress = NULL;
    }

    /* Return the allocated buffer for SMP keys */
    return pSmpKeys;
}


gapSmpKeys_t *fsciBleGapAllocSmpKeys(void)
{
    gapSmpKeys_t   *pSmpKeys;
    uint32_t        ltkSize;
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
    ltkSize = gcSmpMaxBlobSize_c;
#else
    ltkSize = gcSmpMaxLtkSize_c;
#endif    
    /* Allocate buffer for SMP keys */
    pSmpKeys = (gapSmpKeys_t *)MEM_BufferAlloc(sizeof(gapSmpKeys_t) +
                                               ltkSize +
                                               gcSmpIrkSize_c +
                                               gcSmpCsrkSize_c +
                                               gcSmpMaxRandSize_c +
                                               gcBleDeviceAddressSize_c);

    if (NULL != pSmpKeys)
    {
        /* Set pointers in gapSmpKeys_t structure */
        pSmpKeys->aLtk      = (uint8_t *)pSmpKeys + sizeof(gapSmpKeys_t);
        pSmpKeys->aIrk      = pSmpKeys->aLtk + ltkSize;
        pSmpKeys->aCsrk     = pSmpKeys->aIrk + gcSmpIrkSize_c;
        pSmpKeys->aRand     = pSmpKeys->aCsrk + gcSmpCsrkSize_c;
        pSmpKeys->aAddress  = pSmpKeys->aRand + gcSmpMaxRandSize_c;
    }

    /* Return the allocated buffer for SMP keys or NULL if no memory is available */
    return pSmpKeys;
}


uint32_t fsciBleGapGetSmpKeysBufferSize(const gapSmpKeys_t* pSmpKeys)
{
    /* bLtkIncluded */
    uint32_t bufferSize = sizeof(bool_t);

    if(NULL != pSmpKeys->aLtk)
    {
        /* cLtkSIze and aLtk */
        bufferSize += sizeof(uint8_t) + (uint32_t)pSmpKeys->cLtkSize;
    }

    /* bIrkIncluded */
    bufferSize += sizeof(bool_t);

    if(NULL != pSmpKeys->aIrk)
    {
        /* aIrk */
        bufferSize += gcSmpIrkSize_c;
    }

    /* bCsrkIncluded */
    bufferSize += sizeof(bool_t);

    if(NULL != pSmpKeys->aCsrk)
    {
        /* aCsrk */
        bufferSize += gcSmpCsrkSize_c;
    }

    if(NULL != pSmpKeys->aLtk)
    {
        /* cRandSize, aRand and ediv */
        bufferSize += sizeof(uint8_t) + (uint32_t)pSmpKeys->cRandSize + sizeof(uint16_t);
    }

    if(NULL != pSmpKeys->aIrk)
    {
        /* bAddressIncluded */
        bufferSize += sizeof(bool_t);

        if(NULL != pSmpKeys->aAddress)
        {
            /* addressType and aAddress */
            bufferSize += sizeof(bleAddressType_t) + gcBleDeviceAddressSize_c;
        }
    }

    return bufferSize;
}


void fsciBleGapGetSmpKeysFromBuffer(gapSmpKeys_t* pSmpKeys, uint8_t** ppBuffer)
{
    bool_t  bLtkIncluded;
    bool_t  bIrkIncluded;
    bool_t  bCsrkIncluded;
    bool_t  bAddressIncluded;

    /* Read gapSmpKeys_t fields from buffer */

    /* Verify if LTK is included or not */
    fsciBleGetBoolValueFromBuffer(bLtkIncluded, *ppBuffer);

    if(TRUE == bLtkIncluded)
    {
        /* Get LTK size and LTK value from buffer */
        fsciBleGetUint8ValueFromBuffer(pSmpKeys->cLtkSize, *ppBuffer);
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
        uint8_t tempKey[gcSmpMaxLtkSize_c] = {0U};
        fsciBleGetArrayFromBuffer(tempKey, *ppBuffer, pSmpKeys->cLtkSize);
        if (gSecLibFunctions.pfSecLib_ObfuscateKey != NULL)
        {
            (void)gSecLibFunctions.pfSecLib_ObfuscateKey(tempKey, pSmpKeys->aLtk, 1U);
        }
        pSmpKeys->cLtkSize = gcSmpMaxBlobSize_c;
#else        
        fsciBleGetArrayFromBuffer(pSmpKeys->aLtk, *ppBuffer, pSmpKeys->cLtkSize);
#endif                
    }

    /* Verify if IRK is included or not */
    fsciBleGetBoolValueFromBuffer(bIrkIncluded, *ppBuffer);

    if(TRUE == bIrkIncluded)
    {
        /* Get IRK value from buffer */
        fsciBleGetIrkFromBuffer(pSmpKeys->aIrk, *ppBuffer);
    }

    /* Verify if CSRK is included or not */
    fsciBleGetBoolValueFromBuffer(bCsrkIncluded, *ppBuffer);

    if(TRUE == bCsrkIncluded)
    {
        /* Get CSRK value from buffer */
        fsciBleGetCsrkFromBuffer(pSmpKeys->aCsrk, *ppBuffer);
    }

    if(TRUE == bLtkIncluded)
    {
        /* Rand size, rand value and ediv are present in the buffer only if LTK
        is included */
        fsciBleGetUint8ValueFromBuffer(pSmpKeys->cRandSize, *ppBuffer);
        fsciBleGetArrayFromBuffer(pSmpKeys->aRand, *ppBuffer, pSmpKeys->cRandSize);
        fsciBleGetUint16ValueFromBuffer(pSmpKeys->ediv, *ppBuffer);
    }

    if(TRUE == bIrkIncluded)
    {
        /* Verify if address is included or not */
        fsciBleGetBoolValueFromBuffer(bAddressIncluded, *ppBuffer);

        if(TRUE == bAddressIncluded)
        {
            /* Get address type from buffer */
            fsciBleGetEnumValueFromBuffer(pSmpKeys->addressType, *ppBuffer, bleAddressType_t);

            /* Get address from buffer */
            fsciBleGetAddressFromBuffer(pSmpKeys->aAddress, *ppBuffer);
        }
    }
}


void fsciBleGapGetBufferFromSmpKeys(const gapSmpKeys_t* pSmpKeys, uint8_t** ppBuffer)
{
    bool_t  bLtkIncluded        = (NULL == pSmpKeys->aLtk) ? FALSE : TRUE;
    bool_t  bIrkIncluded        = (NULL == pSmpKeys->aIrk) ? FALSE : TRUE;
    bool_t  bCsrkIncluded       = (NULL == pSmpKeys->aCsrk) ? FALSE : TRUE;
    bool_t  bAddressIncluded    = (NULL == pSmpKeys->aAddress) ? FALSE : TRUE;

    /* Write gapSmpKeys_t fields in buffer */

    /* Write boolean value which indicates if LTK is included or not */
    fsciBleGetBufferFromBoolValue(bLtkIncluded, *ppBuffer);

    if(TRUE == bLtkIncluded)
    {
        /* Write LTK size and LTK value in buffer */
        fsciBleGetBufferFromUint8Value(pSmpKeys->cLtkSize, *ppBuffer);
        fsciBleGetBufferFromArray(pSmpKeys->aLtk, *ppBuffer, pSmpKeys->cLtkSize);
    }

    /* Write boolean value which indicates if IRK is included or not */
    fsciBleGetBufferFromBoolValue(bIrkIncluded, *ppBuffer);

    if(TRUE == bIrkIncluded)
    {
        /* Write IRK value in buffer */
        fsciBleGetBufferFromIrk(pSmpKeys->aIrk, *ppBuffer);
    }

    /* Write boolean value which indicates if CSRK is included or not */
    fsciBleGetBufferFromBoolValue(bCsrkIncluded, *ppBuffer);

    if(TRUE == bCsrkIncluded)
    {
        /* Write CSRK value in buffer */
        fsciBleGetBufferFromCsrk(pSmpKeys->aCsrk, *ppBuffer);
    }

    if(TRUE == bLtkIncluded)
    {
        /* Write rand size, rand value and ediv in buffer only if LTK is included */
        fsciBleGetBufferFromUint8Value(pSmpKeys->cRandSize, *ppBuffer);
        fsciBleGetBufferFromArray(pSmpKeys->aRand, *ppBuffer, pSmpKeys->cRandSize);
        fsciBleGetBufferFromUint16Value(pSmpKeys->ediv, *ppBuffer);
    }

    if(TRUE == bIrkIncluded)
    {
        /* Write boolean value which indicates if address is included or not */
        fsciBleGetBufferFromBoolValue(bAddressIncluded, *ppBuffer);

        if(TRUE == bAddressIncluded)
        {
            /* Write address type in buffer */
            fsciBleGetBufferFromEnumValue(pSmpKeys->addressType, *ppBuffer, bleAddressType_t);

            /* Write address in buffer */
            fsciBleGetBufferFromAddress(pSmpKeys->aAddress, *ppBuffer);
        }
    }
}


void fsciBleGapGetSecurityRequirementsFromBuffer(gapSecurityRequirements_t* pSecurityRequirements, uint8_t** ppBuffer)
{
    /* Read gapSecurityRequirements_t fields from buffer */
    fsciBleGetEnumValueFromBuffer(pSecurityRequirements->securityModeLevel, *ppBuffer, gapSecurityModeAndLevel_t);
    fsciBleGetBoolValueFromBuffer(pSecurityRequirements->authorization, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pSecurityRequirements->minimumEncryptionKeySize, *ppBuffer);
}


void fsciBleGapGetBufferFromSecurityRequirements(gapSecurityRequirements_t* pSecurityRequirements, uint8_t** ppBuffer)
{
    /* Write gapSecurityRequirements_t fields in buffer */
    fsciBleGetBufferFromEnumValue(pSecurityRequirements->securityModeLevel, *ppBuffer, gapSecurityModeAndLevel_t);
    fsciBleGetBufferFromBoolValue(pSecurityRequirements->authorization, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pSecurityRequirements->minimumEncryptionKeySize, *ppBuffer);
}


void fsciBleGapGetServiceSecurityRequirementsFromBuffer(gapServiceSecurityRequirements_t* pServiceSecurityRequirements, uint8_t** ppBuffer)
{
    /* Read gapServiceSecurityRequirements_t fields from buffer */
    fsciBleGetUint16ValueFromBuffer(pServiceSecurityRequirements->serviceHandle, *ppBuffer);
    fsciBleGapGetSecurityRequirementsFromBuffer(&pServiceSecurityRequirements->requirements, ppBuffer);
}


void fsciBleGapGetBufferFromServiceSecurityRequirements(gapServiceSecurityRequirements_t* pServiceSecurityRequirements, uint8_t** ppBuffer)
{
    /* Write gapServiceSecurityRequirements_t fields in buffer */
    fsciBleGetBufferFromUint16Value(pServiceSecurityRequirements->serviceHandle, *ppBuffer);
    fsciBleGapGetBufferFromSecurityRequirements(&pServiceSecurityRequirements->requirements, ppBuffer);
}


gapDeviceSecurityRequirements_t* fsciBleGapAllocDeviceSecurityRequirementsForBuffer(uint8_t* pBuffer)
{
    union
    {
        uint8_t*                             pSecurityRequirementsTemp;
        gapServiceSecurityRequirements_t*    aServiceSecurityRequirementsTemp;
        gapSecurityRequirements_t*           pDeviceSecurityRequirementsTemp;
    }securityRequirementsVars = {0};

    uint8_t                             nbOfServices = 0U;
    gapDeviceSecurityRequirements_t*    pDeviceSecurityRequirements = NULL;

    /* Go in buffer to the number of services position */
    pBuffer += sizeof(gapSecurityModeAndLevel_t) + sizeof(bool_t) + sizeof(uint16_t);

    /* Get the number of services from buffer */
    fsciBleGetUint8ValueFromBuffer(nbOfServices, pBuffer);

    /* Allocate buffer for the device security requirements */
    pDeviceSecurityRequirements = (gapDeviceSecurityRequirements_t*)MEM_BufferAlloc(sizeof(gapDeviceSecurityRequirements_t) +
                                                                                    sizeof(uint8_t) +
                                                                                    sizeof(gapSecurityRequirements_t) +
                                                                                    (uint32_t)nbOfServices * sizeof(gapServiceSecurityRequirements_t));

    if(NULL == pDeviceSecurityRequirements)
    {
        /* No memory */
        return NULL;
    }

    /* Set pointers in gapDeviceSecurityRequirements_t structure */
    securityRequirementsVars.pSecurityRequirementsTemp = (uint8_t*)pDeviceSecurityRequirements + sizeof(gapDeviceSecurityRequirements_t);
    pDeviceSecurityRequirements->pSecurityRequirements    = securityRequirementsVars.pDeviceSecurityRequirementsTemp;
    securityRequirementsVars.pSecurityRequirementsTemp = (uint8_t*)pDeviceSecurityRequirements->pSecurityRequirements + sizeof(gapSecurityRequirements_t);
    pDeviceSecurityRequirements->aServiceSecurityRequirements   = securityRequirementsVars.aServiceSecurityRequirementsTemp;

    /* Return the allocated buffer for the device security requirements */
    return pDeviceSecurityRequirements;
}


void fsciBleGapGetDeviceSecurityRequirementsFromBuffer(gapDeviceSecurityRequirements_t* pDeviceSecurityRequirements, uint8_t** ppBuffer)
{
    uint32_t iCount = 0U;

    /* Read gapDeviceSecurityRequirements_t fields from buffer */
    fsciBleGapGetSecurityRequirementsFromBuffer(pDeviceSecurityRequirements->pSecurityRequirements, ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pDeviceSecurityRequirements->cNumServices, *ppBuffer);

    /* Read all the service security requirements */
    if ( pDeviceSecurityRequirements->cNumServices <= gGapMaxServiceSpecificSecurityRequirements_c)
    {    
        for(iCount = 0U; iCount < pDeviceSecurityRequirements->cNumServices; iCount++)
        {
            fsciBleGapGetServiceSecurityRequirementsFromBuffer(&pDeviceSecurityRequirements->aServiceSecurityRequirements[iCount], ppBuffer);
        }
    }
}


void fsciBleGapGetBufferFromDeviceSecurityRequirements(const gapDeviceSecurityRequirements_t* pDeviceSecurityRequirements, uint8_t** ppBuffer)
{
    uint32_t iCount = 0U;

    /* Write gapDeviceSecurityRequirements_t fields in buffer */
    fsciBleGapGetBufferFromSecurityRequirements(pDeviceSecurityRequirements->pSecurityRequirements, ppBuffer);
    fsciBleGetBufferFromUint8Value(pDeviceSecurityRequirements->cNumServices, *ppBuffer);

    /* Write all the service security requirements */
    for(iCount = 0U; iCount < pDeviceSecurityRequirements->cNumServices; iCount++)
    {
        fsciBleGapGetBufferFromServiceSecurityRequirements(&pDeviceSecurityRequirements->aServiceSecurityRequirements[iCount], ppBuffer);
    }
}


void fsciBleGapGetHandleListFromBuffer(gapHandleList_t* pHandleList, uint8_t** ppBuffer)
{
    /* Read gapHandleList_t fields from buffer */
    fsciBleGetUint8ValueFromBuffer(pHandleList->cNumHandles, *ppBuffer);
    fsciBleGetArrayFromBuffer(pHandleList->aHandles, *ppBuffer, (uint32_t)pHandleList->cNumHandles * sizeof(uint16_t));
}


void fsciBleGapGetBufferFromHandleList(gapHandleList_t* pHandleList, uint8_t** ppBuffer)
{
    /* Write gapHandleList_t fields in buffer */
    fsciBleGetBufferFromUint8Value(pHandleList->cNumHandles, *ppBuffer);
    fsciBleGetBufferFromArray(pHandleList->aHandles, *ppBuffer, (uint32_t)pHandleList->cNumHandles * sizeof(uint16_t));
}


void fsciBleGapGetConnectionSecurityInformationFromBuffer(gapConnectionSecurityInformation_t* pConnectionSecurityInformation, uint8_t** ppBuffer)
{
    /* Read gapConnectionSecurityInformation_t fields from buffer */
    fsciBleGetBoolValueFromBuffer(pConnectionSecurityInformation->authenticated, *ppBuffer);
    fsciBleGapGetHandleListFromBuffer(&pConnectionSecurityInformation->authorizedToRead, ppBuffer);
    fsciBleGapGetHandleListFromBuffer(&pConnectionSecurityInformation->authorizedToWrite, ppBuffer);
}


void fsciBleGapGetBuffFromConnSecurityInformation(gapConnectionSecurityInformation_t* pConnectionSecurityInformation, uint8_t** ppBuffer)
{
    /* Write gapConnectionSecurityInformation_t fields in buffer */
    fsciBleGetBufferFromBoolValue(pConnectionSecurityInformation->authenticated, *ppBuffer);
    fsciBleGapGetBufferFromHandleList(&pConnectionSecurityInformation->authorizedToRead, ppBuffer);
    fsciBleGapGetBufferFromHandleList(&pConnectionSecurityInformation->authorizedToWrite, ppBuffer);
}


void fsciBleGapGetPairingParametersFromBuffer(gapPairingParameters_t* pPairingParameters, uint8_t** ppBuffer)
{
    /* Read gapPairingParameters_t fields from buffer */
    fsciBleGetBoolValueFromBuffer(pPairingParameters->withBonding, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pPairingParameters->securityModeAndLevel, *ppBuffer, gapSecurityModeAndLevel_t);
    fsciBleGetUint8ValueFromBuffer(pPairingParameters->maxEncryptionKeySize, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pPairingParameters->localIoCapabilities, *ppBuffer, gapIoCapabilities_t);
    fsciBleGetBoolValueFromBuffer(pPairingParameters->oobAvailable, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pPairingParameters->centralKeys, *ppBuffer, gapSmpKeyFlags_t);
    fsciBleGetEnumValueFromBuffer(pPairingParameters->peripheralKeys, *ppBuffer, gapSmpKeyFlags_t);
    fsciBleGetBoolValueFromBuffer(pPairingParameters->leSecureConnectionSupported, *ppBuffer);
    fsciBleGetBoolValueFromBuffer(pPairingParameters->useKeypressNotifications, *ppBuffer);
}


void fsciBleGapGetBufferFromPairingParameters(const gapPairingParameters_t* pPairingParameters, uint8_t** ppBuffer)
{
    /* Write gapPairingParameters_t fields in buffer */
    fsciBleGetBufferFromBoolValue(pPairingParameters->withBonding, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pPairingParameters->securityModeAndLevel, *ppBuffer, gapSecurityModeAndLevel_t);
    fsciBleGetBufferFromUint8Value(pPairingParameters->maxEncryptionKeySize, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pPairingParameters->localIoCapabilities, *ppBuffer, gapIoCapabilities_t);
    fsciBleGetBufferFromBoolValue(pPairingParameters->oobAvailable, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pPairingParameters->centralKeys, *ppBuffer, gapSmpKeyFlags_t);
    fsciBleGetBufferFromEnumValue(pPairingParameters->peripheralKeys, *ppBuffer, gapSmpKeyFlags_t);
    fsciBleGetBufferFromBoolValue(pPairingParameters->leSecureConnectionSupported, *ppBuffer);
    fsciBleGetBufferFromBoolValue(pPairingParameters->useKeypressNotifications, *ppBuffer);
}


void fsciBleGapGetPeripheralSecurityRequestParametersFromBuffer(gapPeripheralSecurityRequestParameters_t* pPeripheralSecurityRequestParameters, uint8_t** ppBuffer)
{
    /* Read gapPeripheralSecurityRequestParameters_t fields from buffer */
    fsciBleGetBoolValueFromBuffer(pPeripheralSecurityRequestParameters->bondAfterPairing, *ppBuffer);
    fsciBleGetBoolValueFromBuffer(pPeripheralSecurityRequestParameters->authenticationRequired, *ppBuffer);
}


void fsciBleGapGetBufferFromPeripheralSecurityRequestParameters(gapPeripheralSecurityRequestParameters_t* pPeripheralSecurityRequestParameters, uint8_t** ppBuffer)
{
    /* Write gapPeripheralSecurityRequestParameters_t fields in buffer */
    fsciBleGetBufferFromBoolValue(pPeripheralSecurityRequestParameters->bondAfterPairing, *ppBuffer);
    fsciBleGetBufferFromBoolValue(pPeripheralSecurityRequestParameters->authenticationRequired, *ppBuffer);
}


void fsciBleGapGetAdvertisingParametersFromBuffer(gapAdvertisingParameters_t* pAdvertisingParameters, uint8_t** ppBuffer)
{
    /* Read gapAdvertisingParameters_t fields from buffer */
    fsciBleGetUint16ValueFromBuffer(pAdvertisingParameters->minInterval, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pAdvertisingParameters->maxInterval, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pAdvertisingParameters->advertisingType, *ppBuffer, bleAdvertisingType_t);
    fsciBleGetEnumValueFromBuffer(pAdvertisingParameters->ownAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetEnumValueFromBuffer(pAdvertisingParameters->peerAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(pAdvertisingParameters->peerAddress, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pAdvertisingParameters->channelMap, *ppBuffer, gapAdvertisingChannelMapFlags_t);
    fsciBleGetEnumValueFromBuffer(pAdvertisingParameters->filterPolicy, *ppBuffer, gapAdvertisingFilterPolicy_t);
}


#if defined(gBLE50_d) && (gBLE50_d == 1U)
void fsciBleGapGetExtAdvertisingParametersFromBuffer(gapExtAdvertisingParameters_t* pAdvertisingParameters, uint8_t** ppBuffer)
{
    /* Read gapExtAdvertisingParameters_t fields from buffer */
    fsciBleGetUint8ValueFromBuffer( pAdvertisingParameters->SID,                       *ppBuffer);
    fsciBleGetUint8ValueFromBuffer( pAdvertisingParameters->handle,                    *ppBuffer);
    fsciBleGetUint32ValueFromBuffer(pAdvertisingParameters->minInterval,               *ppBuffer);
    fsciBleGetUint32ValueFromBuffer(pAdvertisingParameters->maxInterval,               *ppBuffer);
    fsciBleGetEnumValueFromBuffer(  pAdvertisingParameters->ownAddressType,            *ppBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(    pAdvertisingParameters->ownRandomAddr,             *ppBuffer);
    fsciBleGetEnumValueFromBuffer(  pAdvertisingParameters->peerAddressType,           *ppBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(    pAdvertisingParameters->peerAddress,               *ppBuffer);
    fsciBleGetEnumValueFromBuffer(  pAdvertisingParameters->channelMap,                *ppBuffer, gapAdvertisingChannelMapFlags_t);
    fsciBleGetEnumValueFromBuffer(  pAdvertisingParameters->filterPolicy,              *ppBuffer, gapAdvertisingFilterPolicy_t);
    fsciBleGetEnumValueFromBuffer(  pAdvertisingParameters->extAdvProperties,          *ppBuffer, bleAdvRequestProperties_t);
    fsciBleGetUint8ValueFromBufferSigned( pAdvertisingParameters->txPower,             *ppBuffer);
    fsciBleGetEnumValueFromBuffer(  pAdvertisingParameters->primaryPHY,                *ppBuffer, gapLePhyMode_t);
    fsciBleGetEnumValueFromBuffer(  pAdvertisingParameters->secondaryPHY,              *ppBuffer, gapLePhyMode_t);
    fsciBleGetUint8ValueFromBuffer( pAdvertisingParameters->secondaryAdvMaxSkip,       *ppBuffer);
    fsciBleGetBoolValueFromBuffer(  pAdvertisingParameters->enableScanReqNotification, *ppBuffer);
}

void fsciBleGapGetPeriodicAdvSyncReqFromBuffer(gapPeriodicAdvSyncReq_t* pReq, uint8_t** ppBuffer)
{
    fsciBleGetEnumValueFromBuffer(pReq->options, *ppBuffer, gapCreateSyncReqOptions_t);
    fsciBleGetUint8ValueFromBuffer(pReq->SID, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pReq->peerAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(pReq->peerAddress, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pReq->skipCount, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pReq->timeout, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pReq->cteType, *ppBuffer, bleSyncCteType_t);
}

void fsciBleGapGetBufferFromPerAdvSyncReq(gapPeriodicAdvSyncReq_t* pReq, uint8_t** ppBuffer)
{
    fsciBleGetBufferFromEnumValue(pReq->options, *ppBuffer, gapCreateSyncReqOptions_t);
    fsciBleGetBufferFromUint8Value(pReq->SID, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pReq->peerAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(pReq->peerAddress, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pReq->skipCount, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pReq->timeout, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pReq->cteType, *ppBuffer, bleCteType_t);
}
#endif

#if defined(gBLE51_d) && (gBLE51_d == 1U)

void fsciBleGapGetConnectionCteReqParametersFromBuffer(gapConnectionCteReqEnableParams_t* pReqEnableParams, uint8_t** ppBuffer)
{
    fsciBleGetEnumValueFromBuffer(pReqEnableParams->cteReqEnable, *ppBuffer, bleCteReqEnable_t);
    fsciBleGetUint16ValueFromBuffer(pReqEnableParams->cteReqInterval, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pReqEnableParams->requestedCteLength, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pReqEnableParams->requestedCteType, *ppBuffer, bleCteType_t);
}

void fsciBleGapGetPeriodicAdvSncTransferParametersFromBuffer(gapPeriodicAdvSyncTransfer_t* pParams, uint8_t** ppBuffer)
{
    fsciBleGetDeviceIdFromBuffer(&pParams->deviceId, ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pParams->serviceData, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pParams->syncHandle, *ppBuffer);
}

void fsciBleGapGetPeriodicAdvSetInfoTransferFromBuffer(gapPeriodicAdvSetInfoTransfer_t* pParams, uint8_t** ppBuffer)
{
    fsciBleGetDeviceIdFromBuffer(&pParams->deviceId, ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pParams->serviceData, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pParams->advHandle, *ppBuffer);
}

void fsciBleGapSetPeriodicAdvSyncTransferParamsFromBuffer(gapSetPeriodicAdvSyncTransferParams_t* pParams, uint8_t** ppBuffer)
{
    fsciBleGetDeviceIdFromBuffer(&pParams->deviceId, ppBuffer);
    fsciBleGetBufferFromEnumValue(pParams->mode, *ppBuffer, gapPeriodicAdvSyncMode_t);
    fsciBleGetUint16ValueFromBuffer(pParams->skip, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pParams->syncTimeout, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pParams->CTEType, *ppBuffer, bleSyncCteType_t);
}

#endif /* gBLE51_d */

#if defined(gBLE52_d) && (gBLE52_d == 1U)
void fsciBleGapGetPathLossReportingParametersFromBuffer(gapPathLossReportingParams_t* pParams, uint8_t** ppBuffer)
{
    fsciBleGetUint8ValueFromBuffer(pParams->highThreshold, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->highHysteresis, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->lowThreshold, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pParams->lowHysteresis, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pParams->minTimeSpent, *ppBuffer);
}
#endif /* gBLE52_d */

void fsciBleGapGetBuffFromAdvParameters(const gapAdvertisingParameters_t* pAdvertisingParameters, uint8_t** ppBuffer)
{
    /* Write gapAdvertisingParameters_t fields in buffer */
    fsciBleGetBufferFromUint16Value(pAdvertisingParameters->minInterval, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pAdvertisingParameters->maxInterval, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pAdvertisingParameters->advertisingType, *ppBuffer, bleAdvertisingType_t);
    fsciBleGetBufferFromEnumValue(pAdvertisingParameters->ownAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromEnumValue(pAdvertisingParameters->peerAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(pAdvertisingParameters->peerAddress, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pAdvertisingParameters->channelMap, *ppBuffer, gapAdvertisingChannelMapFlags_t);
    fsciBleGetBufferFromEnumValue(pAdvertisingParameters->filterPolicy, *ppBuffer, gapAdvertisingFilterPolicy_t);
}


#if defined(gBLE50_d) && (gBLE50_d == 1U)
void fsciBleGapGetBufferFromExtAdvertisingParameters(gapExtAdvertisingParameters_t* pAdvertisingParameters, uint8_t** ppBuffer)
{
    /* Write gapExtAdvertisingParameters_t fields in buffer */
    fsciBleGetBufferFromUint8Value( pAdvertisingParameters->SID,                       *ppBuffer);
    fsciBleGetBufferFromUint8Value( pAdvertisingParameters->handle,                    *ppBuffer);
    fsciBleGetBufferFromUint32Value(pAdvertisingParameters->minInterval,               *ppBuffer);
    fsciBleGetBufferFromUint32Value(pAdvertisingParameters->maxInterval,               *ppBuffer);
    fsciBleGetBufferFromEnumValue(  pAdvertisingParameters->ownAddressType,            *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(    pAdvertisingParameters->ownRandomAddr,             *ppBuffer);
    fsciBleGetBufferFromEnumValue(  pAdvertisingParameters->peerAddressType,           *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(    pAdvertisingParameters->peerAddress,               *ppBuffer);
    fsciBleGetBufferFromEnumValue(  pAdvertisingParameters->channelMap,                *ppBuffer, gapAdvertisingChannelMapFlags_t);
    fsciBleGetBufferFromEnumValue(  pAdvertisingParameters->filterPolicy,              *ppBuffer, gapAdvertisingFilterPolicy_t);
    fsciBleGetBufferFromEnumValue(  pAdvertisingParameters->extAdvProperties,          *ppBuffer, bleAdvRequestProperties_t);
    fsciBleGetBufferFromUint8ValueSigned( pAdvertisingParameters->txPower,             *ppBuffer);
    fsciBleGetBufferFromEnumValue(  pAdvertisingParameters->primaryPHY,                *ppBuffer, gapLePhyMode_t);
    fsciBleGetBufferFromEnumValue(  pAdvertisingParameters->secondaryPHY,              *ppBuffer, gapLePhyMode_t);
    fsciBleGetBufferFromUint8Value( pAdvertisingParameters->secondaryAdvMaxSkip,       *ppBuffer);
    fsciBleGetBufferFromBoolValue(  pAdvertisingParameters->enableScanReqNotification, *ppBuffer);
}
#endif


void fsciBleGapGetScanningParametersFromBuffer(gapScanningParameters_t* pScanningParameters, uint8_t** ppBuffer)
{
    /* Read gapScanningParameters_t fields from buffer */
    fsciBleGetEnumValueFromBuffer(pScanningParameters->type, *ppBuffer, bleScanType_t);
    fsciBleGetUint16ValueFromBuffer(pScanningParameters->interval, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pScanningParameters->window, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pScanningParameters->ownAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetEnumValueFromBuffer(pScanningParameters->filterPolicy, *ppBuffer, bleScanningFilterPolicy_t);
}


void fsciBleGapGetBuffFromScanParameters(const gapScanningParameters_t* pScanningParameters, uint8_t** ppBuffer)
{
    /* Write gapScanningParameters_t fields in buffer */
    fsciBleGetBufferFromEnumValue(pScanningParameters->type, *ppBuffer, bleScanType_t);
    fsciBleGetBufferFromUint16Value(pScanningParameters->interval, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pScanningParameters->window, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pScanningParameters->ownAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromEnumValue(pScanningParameters->filterPolicy, *ppBuffer, bleScanningFilterPolicy_t);
}


void fsciBleGapGetConnectionRequestParametersFromBuffer(gapConnectionRequestParameters_t* pConnectionRequestParameters, uint8_t** ppBuffer)
{
    /* Read gapConnectionRequestParameters_t fields from buffer */
    fsciBleGetUint16ValueFromBuffer(pConnectionRequestParameters->scanInterval, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnectionRequestParameters->scanWindow, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pConnectionRequestParameters->filterPolicy, *ppBuffer, bleInitiatorFilterPolicy_t);
    fsciBleGetEnumValueFromBuffer(pConnectionRequestParameters->ownAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetEnumValueFromBuffer(pConnectionRequestParameters->peerAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(pConnectionRequestParameters->peerAddress, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnectionRequestParameters->connIntervalMin, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnectionRequestParameters->connIntervalMax, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnectionRequestParameters->connLatency, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnectionRequestParameters->supervisionTimeout, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnectionRequestParameters->connEventLengthMin, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnectionRequestParameters->connEventLengthMax, *ppBuffer);
    fsciBleGetBoolValueFromBuffer(pConnectionRequestParameters->usePeerIdentityAddress, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pConnectionRequestParameters->initiatingPHYs, *ppBuffer, gapLePhyFlags_t);
}


void fsciBleGapGetBuffFromConnReqParameters(const gapConnectionRequestParameters_t* pConnectionRequestParameters, uint8_t** ppBuffer)
{
    /* Write gapConnectionRequestParameters_t fields in buffer */
    fsciBleGetBufferFromUint16Value(pConnectionRequestParameters->scanInterval, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnectionRequestParameters->scanWindow, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pConnectionRequestParameters->filterPolicy, *ppBuffer, bleInitiatorFilterPolicy_t);
    fsciBleGetBufferFromEnumValue(pConnectionRequestParameters->ownAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromEnumValue(pConnectionRequestParameters->peerAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(pConnectionRequestParameters->peerAddress, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnectionRequestParameters->connIntervalMin, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnectionRequestParameters->connIntervalMax, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnectionRequestParameters->connLatency, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnectionRequestParameters->supervisionTimeout, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnectionRequestParameters->connEventLengthMin, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnectionRequestParameters->connEventLengthMax, *ppBuffer);
    fsciBleGetBufferFromBoolValue(pConnectionRequestParameters->usePeerIdentityAddress, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pConnectionRequestParameters->initiatingPHYs, *ppBuffer, gapLePhyFlags_t);
}


void fsciBleGapGetConnectionParametersFromBuffer(gapConnectionParameters_t* pConnectionParameters, uint8_t** ppBuffer)
{
    /* Read gapConnectionParameters_t fields from buffer */
    fsciBleGetUint16ValueFromBuffer(pConnectionParameters->connInterval, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnectionParameters->connLatency, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnectionParameters->supervisionTimeout, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pConnectionParameters->centralClockAccuracy, *ppBuffer, bleCentralClockAccuracy_t);
}


void fsciBleGapGetBuffFromConnParameters(gapConnectionParameters_t* pConnectionParameters, uint8_t** ppBuffer)
{
    /* Write gapConnectionParameters_t fields in buffer */
    fsciBleGetBufferFromUint16Value(pConnectionParameters->connInterval, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnectionParameters->connLatency, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnectionParameters->supervisionTimeout, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pConnectionParameters->centralClockAccuracy, *ppBuffer, bleCentralClockAccuracy_t);
}


gapAdStructure_t* fsciBleGapAllocAdStructureForBuffer(uint8_t* pBuffer)
{
    uint8_t             length;
    gapAdStructure_t*   pAdStructure;

    /* Get the data length from buffer */
    fsciBleGetUint8ValueFromBuffer(length, pBuffer);

    /* Allocate buffer for the gapAdStructure_t structure */
    pAdStructure = (gapAdStructure_t*)MEM_BufferAlloc(sizeof(gapAdStructure_t) + (uint32_t)length);

    if(NULL != pAdStructure)
    {
        /* Set data pointer in gapAdStructure_t structure */
        pAdStructure->aData = (uint8_t*)pAdStructure + sizeof(gapAdStructure_t);
    }

    /* Return the buffer allocated for gapAdStructure_t structure, or NULL */
    return pAdStructure;
}


void fsciBleGapGetAdStructureFromBuffer(gapAdStructure_t* pAdStructure, uint8_t** ppBuffer)
{
    /* Read gapAdStructure_t fields from buffer */
    fsciBleGetUint8ValueFromBuffer(pAdStructure->length, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pAdStructure->adType, *ppBuffer, gapAdType_t);
    fsciBleGetArrayFromBuffer(pAdStructure->aData, *ppBuffer, pAdStructure->length);

    union
    {
        uint32_t lenTemp;
        uint8_t advLen;
    }advLength = {0};

    /* The length value must contain also the type (uint8_t) */
    advLength.lenTemp = sizeof(uint8_t);
    pAdStructure->length += advLength.advLen;
}


void fsciBleGapGetBufferFromAdStructure(gapAdStructure_t* pAdStructure, uint8_t** ppBuffer)
{
    /* Write gapAdStructure_t fields in buffer */
    union
    {
        uint32_t lenTemp;
        uint8_t advLen;
    }advLength = {0};

    advLength.advLen = pAdStructure->length;

    /* In TestTool, the length is only the data length */
    fsciBleGetBufferFromUint8Value((uint8_t)(advLength.lenTemp - sizeof(uint8_t)), *ppBuffer);
    fsciBleGetBufferFromEnumValue(pAdStructure->adType, *ppBuffer, gapAdType_t);
    fsciBleGetBufferFromArray(pAdStructure->aData, *ppBuffer, ((uint32_t)pAdStructure->length - sizeof(uint8_t)));
}


gapAdvertisingData_t* fsciBleGapAllocAdvertisingDataForBuffer(uint8_t* pBuffer)
{
    union
    {
        uint8_t*                    pAdvertisingDataTemp;
        gapAdStructure_t*           aAdStructuresTemp;
    }advertisingVars = {0};

    uint32_t                advertisingDataSize = sizeof(gapAdvertisingData_t);
    uint8_t                 nbOfAdStructures = 0U;
    uint8_t*                aDataSizeArray = NULL;
    gapAdvertisingData_t*   pAdvertisingData = NULL;
    uint8_t*                pData = NULL;
    uint32_t                iCount = 0U;

    /* Get from buffer the number of AdStructures */
    fsciBleGetUint8ValueFromBuffer(nbOfAdStructures, pBuffer);

    if(nbOfAdStructures > 0U)
    {
        /* Allocate buffer to keep each AdStructure length */
        aDataSizeArray = MEM_BufferAlloc(nbOfAdStructures);

        if(NULL == aDataSizeArray)
        {
            /* No memory */
            return NULL;
        }

        /* Read from buffer each AdStructure length */
        for(iCount = 0U; iCount < nbOfAdStructures; iCount++)
        {
            /* Get from buffer the AdStructure data length */
            fsciBleGetUint8ValueFromBuffer(aDataSizeArray[iCount], pBuffer);

            /* Go in buffer to the next AdStructure */
            pBuffer += sizeof(uint8_t) + aDataSizeArray[iCount];

            /* Add this AdStructure size to the total length needed for the allocated buffer */
            advertisingDataSize += (sizeof(gapAdStructure_t) + (uint16_t)aDataSizeArray[iCount]);
        }
    }

    /* Allocate buffer for the advertising data */
    pAdvertisingData = (gapAdvertisingData_t*)MEM_BufferAlloc(advertisingDataSize);

    if(NULL == pAdvertisingData)
    {
        /* No memory */
        /* Free the buffer used to keep the AdStructures lengths */
        (void)MEM_BufferFree(aDataSizeArray);
        return NULL;
    }

    /* Set pointers in gapAdvertisingData_t structure */
    advertisingVars.pAdvertisingDataTemp = (uint8_t*)pAdvertisingData + sizeof(gapAdvertisingData_t);
    pAdvertisingData->aAdStructures = advertisingVars.aAdStructuresTemp;
    pData                           = (uint8_t*)pAdvertisingData->aAdStructures + nbOfAdStructures * sizeof(gapAdStructure_t);

    if(nbOfAdStructures > 0U)
    {
        /* Set data pointer in each AdStructure */
        for(iCount = 0U; iCount < nbOfAdStructures; iCount++)
        {
            pAdvertisingData->aAdStructures[iCount].aData = pData;

            pData += aDataSizeArray[iCount];
        }

        /* Free the buffer used to keep the AdStructures lengths */
        (void)MEM_BufferFree(aDataSizeArray);
    }

    /* Return the buffer allocated for gapAdvertisingData_t structure */
    return pAdvertisingData;
}


uint32_t fsciBleGapGetAdvertisingDataBufferSize(const gapAdvertisingData_t* pAdvertisingData)
{
    /* Get the constant size for the needed buffer */
    uint32_t    bufferSize = sizeof(uint8_t);
    uint32_t    iCount = 0U;

    /* Get the variable size for the needed buffer */
    for(iCount = 0U; iCount < pAdvertisingData->cNumAdStructures; iCount++)
    {
        bufferSize += (uint16_t)fsciBleGapGetAdStructureBufferSize(&pAdvertisingData->aAdStructures[iCount]);
    }

    /* Return the size needed for the buffer */
    return bufferSize;
}


void fsciBleGapGetAdvertisingDataFromBuffer(gapAdvertisingData_t* pAdvertisingData, uint8_t** ppBuffer)
{
    uint32_t iCount = 0U;

    /* Read gapAdvertisingData_t fields from buffer */
    fsciBleGetUint8ValueFromBuffer(pAdvertisingData->cNumAdStructures, *ppBuffer);

    for(iCount = 0U; iCount < pAdvertisingData->cNumAdStructures; iCount++)
    {
        fsciBleGapGetAdStructureFromBuffer(&pAdvertisingData->aAdStructures[iCount], ppBuffer);
    }
}


void fsciBleGapGetBufferFromAdvertisingData(const gapAdvertisingData_t* pAdvertisingData, uint8_t** ppBuffer)
{
    uint32_t iCount = 0U;

    /* Write gapAdvertisingData_t fields in buffer */
    fsciBleGetBufferFromUint8Value(pAdvertisingData->cNumAdStructures, *ppBuffer);

    for(iCount = 0U; iCount < pAdvertisingData->cNumAdStructures; iCount++)
    {
        fsciBleGapGetBufferFromAdStructure(&pAdvertisingData->aAdStructures[iCount], ppBuffer);
    }
}


void fsciBleGapGetScannedDeviceFromBuffer(gapScannedDevice_t* pScannedDevice, uint8_t** ppBuffer)
{
    /* Read gapScannedDevice_t fields from buffer */
    fsciBleGetEnumValueFromBuffer(pScannedDevice->addressType, *ppBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(pScannedDevice->aAddress, *ppBuffer);
    fsciBleGetUint8ValueFromBufferSigned(pScannedDevice->rssi, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pScannedDevice->dataLength, *ppBuffer);
    fsciBleGetArrayFromBuffer(pScannedDevice->data, *ppBuffer, pScannedDevice->dataLength);
    fsciBleGetEnumValueFromBuffer(pScannedDevice->advEventType, *ppBuffer, bleAdvertisingReportEventType_t);
    fsciBleGetBoolValueFromBuffer(pScannedDevice->directRpaUsed, *ppBuffer);
    if( pScannedDevice->directRpaUsed )
    {
        fsciBleGetAddressFromBuffer(pScannedDevice->directRpa, *ppBuffer);
    }
    fsciBleGetBoolValueFromBuffer(pScannedDevice->advertisingAddressResolved, *ppBuffer);
}


void fsciBleGapGetBufferFromScannedDevice(gapScannedDevice_t* pScannedDevice, uint8_t** ppBuffer)
{
    /* Write gapScannedDevice_t fields in buffer */
    fsciBleGetBufferFromEnumValue(pScannedDevice->addressType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(pScannedDevice->aAddress, *ppBuffer);
    fsciBleGetBufferFromUint8ValueSigned(pScannedDevice->rssi, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pScannedDevice->dataLength, *ppBuffer);
    fsciBleGetBufferFromArray(pScannedDevice->data, *ppBuffer, pScannedDevice->dataLength);
    fsciBleGetBufferFromEnumValue(pScannedDevice->advEventType, *ppBuffer, bleAdvertisingReportEventType_t);
    fsciBleGetBufferFromBoolValue(pScannedDevice->directRpaUsed, *ppBuffer);
    if( pScannedDevice->directRpaUsed )
    {
        fsciBleGetBufferFromAddress(pScannedDevice->directRpa, *ppBuffer);
    }
    fsciBleGetBufferFromBoolValue(pScannedDevice->advertisingAddressResolved, *ppBuffer);
}

#if defined(gBLE50_d) && (gBLE50_d == 1U)

void fsciBleGapGetExtScannedDeviceFromBuffer(gapExtScannedDevice_t* pScannedDevice, uint8_t** ppBuffer)
{
    /* Read gapExtScannedDevice_t fields from buffer */
    fsciBleGetEnumValueFromBuffer(pScannedDevice->addressType, *ppBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(pScannedDevice->aAddress, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pScannedDevice->SID, *ppBuffer);
    fsciBleGetBoolValueFromBuffer(pScannedDevice->advertisingAddressResolved, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pScannedDevice->advEventProperties, *ppBuffer, bleAdvReportEventProperties_t);
    fsciBleGetUint8ValueFromBufferSigned(pScannedDevice->rssi, *ppBuffer);
    fsciBleGetUint8ValueFromBufferSigned(pScannedDevice->txPower, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pScannedDevice->primaryPHY, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pScannedDevice->secondaryPHY, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pScannedDevice->periodicAdvInterval, *ppBuffer);
    fsciBleGetBoolValueFromBuffer(pScannedDevice->directRpaUsed, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pScannedDevice->directRpaType, *ppBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(pScannedDevice->directRpa, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pScannedDevice->dataLength, *ppBuffer);
    fsciBleGetArrayFromBuffer(pScannedDevice->pData, *ppBuffer, pScannedDevice->dataLength);
}

void fsciBleGapGetBufferFromExtScannedDevice(gapExtScannedDevice_t* pScannedDevice, uint8_t** ppBuffer)
{
    /* Write gapExtScannedDevice_t fields in buffer */
    fsciBleGetBufferFromEnumValue(pScannedDevice->addressType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(pScannedDevice->aAddress, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pScannedDevice->SID, *ppBuffer);
    fsciBleGetBufferFromBoolValue(pScannedDevice->advertisingAddressResolved, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pScannedDevice->advEventProperties, *ppBuffer, bleAdvReportEventProperties_t);
    fsciBleGetBufferFromUint8ValueSigned(pScannedDevice->rssi, *ppBuffer);
    fsciBleGetBufferFromUint8ValueSigned(pScannedDevice->txPower, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pScannedDevice->primaryPHY, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pScannedDevice->secondaryPHY, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pScannedDevice->periodicAdvInterval, *ppBuffer);
    fsciBleGetBufferFromBoolValue(pScannedDevice->directRpaUsed, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pScannedDevice->directRpaType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(pScannedDevice->directRpa, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pScannedDevice->dataLength, *ppBuffer);
    fsciBleGetBufferFromArray(pScannedDevice->pData, *ppBuffer, pScannedDevice->dataLength);
}

void fsciBleGapGetPeriodicScannedDeviceFromBuffer(gapPeriodicScannedDevice_t* pScannedDevice, uint8_t** ppBuffer)
{
    /* Read gapExtScannedDevice_t fields from buffer */
    fsciBleGetUint16ValueFromBuffer(pScannedDevice->syncHandle, *ppBuffer);
    fsciBleGetUint8ValueFromBufferSigned(pScannedDevice->rssi, *ppBuffer);
    fsciBleGetUint8ValueFromBufferSigned(pScannedDevice->txPower, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pScannedDevice->dataLength, *ppBuffer);
    fsciBleGetArrayFromBuffer(pScannedDevice->pData, *ppBuffer, pScannedDevice->dataLength);
}

void fsciBleGapGetBufferFromPerScannedDevice(gapPeriodicScannedDevice_t* pScannedDevice, uint8_t** ppBuffer)
{
    /* Write gapExtScannedDevice_t fields in buffer */
    fsciBleGetBufferFromUint16Value(pScannedDevice->syncHandle, *ppBuffer);
    fsciBleGetBufferFromUint8ValueSigned(pScannedDevice->rssi, *ppBuffer);
    fsciBleGetBufferFromUint8ValueSigned(pScannedDevice->txPower, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pScannedDevice->dataLength, *ppBuffer);
    fsciBleGetBufferFromArray(pScannedDevice->pData, *ppBuffer, pScannedDevice->dataLength);
}
#endif  /* gBLE50_d */


#if defined(gBLE51_d) && (gBLE51_d == 1U)
void fsciBleGapGetBufferFromConnectionlessIqReportReceived(gapConnectionlessIqReport_t* pIqReport, uint8_t** ppBuffer)
{
    /* Write gapConnectionlessIqReport_t fields in buffer */
    fsciBleGetBufferFromUint16Value(pIqReport->syncHandle, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pIqReport->channelIndex, *ppBuffer);
    fsciBleGetBufferFromUint16Value((uint16_t)pIqReport->rssi, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pIqReport->rssiAntennaId, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pIqReport->cteType, *ppBuffer, bleCteType_t);
    fsciBleGetBufferFromEnumValue(pIqReport->slotDurations, *ppBuffer, bleSlotDurations_t);
    fsciBleGetBufferFromEnumValue(pIqReport->packetStatus, *ppBuffer, bleIqReportPacketStatus_t);
    fsciBleGetBufferFromUint16Value(pIqReport->periodicEventCounter, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pIqReport->sampleCount, *ppBuffer);
    fsciBleGetBufferFromArray(pIqReport->aI_samples, *ppBuffer, pIqReport->sampleCount);
    fsciBleGetBufferFromArray(pIqReport->aQ_samples, *ppBuffer, pIqReport->sampleCount);
}

void fsciBleGapGetBufferFromConnIqReportReceived(gapConnIqReport_t* pIqReport, uint8_t** ppBuffer)
{
    /* Write gapConnIqReport_t fields in buffer */
    fsciBleGetBufferFromEnumValue(pIqReport->rxPhy, *ppBuffer, gapLePhyMode_t);
    fsciBleGetBufferFromUint8Value(pIqReport->dataChannelIndex, *ppBuffer);
    fsciBleGetBufferFromUint16Value((uint16_t)pIqReport->rssi, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pIqReport->rssiAntennaId, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pIqReport->cteType, *ppBuffer, bleCteType_t);
    fsciBleGetBufferFromEnumValue(pIqReport->slotDurations, *ppBuffer, bleSlotDurations_t);
    fsciBleGetBufferFromEnumValue(pIqReport->packetStatus, *ppBuffer, bleIqReportPacketStatus_t);
    fsciBleGetBufferFromUint16Value(pIqReport->connEventCounter, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pIqReport->sampleCount, *ppBuffer);
    fsciBleGetBufferFromArray(pIqReport->aI_samples, *ppBuffer, pIqReport->sampleCount);
    fsciBleGetBufferFromArray(pIqReport->aQ_samples, *ppBuffer, pIqReport->sampleCount);
}

void fsciBleGapGetConnectionlessIqReportReceivedFromBuffer(gapConnectionlessIqReport_t* pIqReport, uint8_t** ppBuffer)
{
    uint16_t temp = (uint16_t)pIqReport->rssi;
    
     /* Read gapConnectionlessIqReport_t fields from buffer */
    fsciBleGetUint16ValueFromBuffer(pIqReport->syncHandle, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pIqReport->channelIndex, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(temp, *ppBuffer);
    pIqReport->rssi = (int16_t)temp;
    fsciBleGetUint8ValueFromBuffer(pIqReport->rssiAntennaId, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pIqReport->cteType, *ppBuffer, bleCteType_t);
    fsciBleGetEnumValueFromBuffer(pIqReport->slotDurations, *ppBuffer, bleSlotDurations_t);
    fsciBleGetEnumValueFromBuffer(pIqReport->packetStatus, *ppBuffer, bleIqReportPacketStatus_t);
    fsciBleGetUint16ValueFromBuffer(pIqReport->periodicEventCounter, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pIqReport->sampleCount, *ppBuffer);
    fsciBleGetArrayFromBuffer(pIqReport->aI_samples, *ppBuffer, pIqReport->sampleCount);
    fsciBleGetArrayFromBuffer(pIqReport->aQ_samples, *ppBuffer, pIqReport->sampleCount);
}

void fsciBleGapGetConnIqReportReceivedFromBuffer(gapConnIqReport_t* pIqReport, uint8_t** ppBuffer)
{
    uint16_t temp = (uint16_t)pIqReport->rssi;
    
     /* Read gapConnIqReport_t fields from buffer */
    fsciBleGetBufferFromEnumValue(pIqReport->rxPhy, *ppBuffer, gapLePhyMode_t);
    fsciBleGetUint8ValueFromBuffer(pIqReport->dataChannelIndex, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(temp, *ppBuffer);
    pIqReport->rssi = (int16_t)temp;
    fsciBleGetUint8ValueFromBuffer(pIqReport->rssiAntennaId, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pIqReport->cteType, *ppBuffer, bleCteType_t);
    fsciBleGetEnumValueFromBuffer(pIqReport->slotDurations, *ppBuffer, bleSlotDurations_t);
    fsciBleGetEnumValueFromBuffer(pIqReport->packetStatus, *ppBuffer, bleIqReportPacketStatus_t);
    fsciBleGetUint16ValueFromBuffer(pIqReport->connEventCounter, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pIqReport->sampleCount, *ppBuffer);
    fsciBleGetArrayFromBuffer(pIqReport->aI_samples, *ppBuffer, pIqReport->sampleCount);
    fsciBleGetArrayFromBuffer(pIqReport->aQ_samples, *ppBuffer, pIqReport->sampleCount);
}

void fsciBleGapGetBufferFromPeriodicAdvSyncTransferReceived(gapSyncTransferReceivedEventData_t* pSyncTransferReceived, uint8_t** ppBuffer)
{
    /* Write gapSyncTransferReceivedEventData_t fields in buffer */
    fsciBleGetBufferFromUint16Value((uint16_t)pSyncTransferReceived->status, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pSyncTransferReceived->deviceId, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pSyncTransferReceived->serviceData, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pSyncTransferReceived->syncHandle, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pSyncTransferReceived->advSID, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pSyncTransferReceived->advAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(pSyncTransferReceived->advAddress, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pSyncTransferReceived->advPhy, *ppBuffer, gapLePhyMode_t);
    fsciBleGetBufferFromUint16Value(pSyncTransferReceived->periodicAdvInt, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pSyncTransferReceived->advClockAccuracy, *ppBuffer);
}
#endif  /* gBLE51_d */

#if defined(gBLE52_d) && (gBLE52_d == 1U)
void fsciBleGapGetPathLossThresholdFromBuffer(gapPathLossThresholdEvent_t *pPathLossThreshold, uint8_t **ppBuffer)
{
    /* Read gapPathLossThresholdEvent_t fields from buffer */
    fsciBleGetUint8ValueFromBuffer(pPathLossThreshold->currentPathLoss, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pPathLossThreshold->zoneEntered, *ppBuffer, blePathLossThresholdZoneEntered_t);
}

void fsciBleGapGetTransmitPowerReportingFromBuffer(gapTransmitPowerReporting_t *pTransmitPowerReporting, uint8_t **ppBuffer)
{
    /* Read gapTransmitPowerReporting_t fields from buffer */
    fsciBleGetEnumValueFromBuffer(pTransmitPowerReporting->reason, *ppBuffer, bleTxPowerReportingReason_t);
    fsciBleGetEnumValueFromBuffer(pTransmitPowerReporting->phy, *ppBuffer, blePowerControlPhyType_t);
    fsciBleGetUint8ValueFromBufferSigned(pTransmitPowerReporting->txPowerLevel, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pTransmitPowerReporting->flags, *ppBuffer, bleTxPowerLevelFlags_t);
    fsciBleGetUint8ValueFromBufferSigned(pTransmitPowerReporting->delta, *ppBuffer);
}

void fsciBleGapGetTransmitPowerInfoFromBuffer(gapTransmitPowerInfo_t *pTransmitPowerInfo, uint8_t **ppBuffer)
{
    /* Read gapTransmitPowerInfo_t fields from buffer */
    fsciBleGetEnumValueFromBuffer(pTransmitPowerInfo->phy, *ppBuffer, blePowerControlPhyType_t);
    fsciBleGetUint8ValueFromBufferSigned(pTransmitPowerInfo->currTxPowerLevel, *ppBuffer);
    fsciBleGetUint8ValueFromBufferSigned(pTransmitPowerInfo->maxTxPowerLevel, *ppBuffer);
}

void fsciBleGapGetBufferFromPathLossThreshold(gapPathLossThresholdEvent_t *pPathLossThreshold, uint8_t **ppBuffer)
{
    /* Write gapPathLossThresholdEvent_t fields into buffer */
    fsciBleGetBufferFromUint8Value(pPathLossThreshold->currentPathLoss, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pPathLossThreshold->zoneEntered, *ppBuffer, blePathLossThresholdZoneEntered_t);
}

void fsciBleGapGetBufferFromTransmitPowerReporting(gapTransmitPowerReporting_t *pTransmitPowerReporting, uint8_t **ppBuffer)
{
    /* Write gapTransmitPowerReporting_t fields into buffer */
    fsciBleGetBufferFromEnumValue(pTransmitPowerReporting->reason, *ppBuffer, bleTxPowerReportingReason_t);
    fsciBleGetBufferFromEnumValue(pTransmitPowerReporting->phy, *ppBuffer, blePowerControlPhyType_t);
    fsciBleGetBufferFromUint8ValueSigned(pTransmitPowerReporting->txPowerLevel, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pTransmitPowerReporting->flags, *ppBuffer, bleTxPowerLevelFlags_t);
    fsciBleGetBufferFromUint8ValueSigned(pTransmitPowerReporting->delta, *ppBuffer);
}

void fsciBleGapGetBufferFromTransmitPowerInfo(gapTransmitPowerInfo_t *pTransmitPowerInfo, uint8_t **ppBuffer)
{
    /* Write gapTransmitPowerInfo_t fields into buffer */
    fsciBleGetBufferFromEnumValue(pTransmitPowerInfo->phy, *ppBuffer, blePowerControlPhyType_t);
    fsciBleGetBufferFromUint8ValueSigned(pTransmitPowerInfo->currTxPowerLevel, *ppBuffer);
    fsciBleGetBufferFromUint8ValueSigned(pTransmitPowerInfo->maxTxPowerLevel, *ppBuffer);
}

#if defined(gEATT_d) && (gEATT_d == TRUE)
void fsciBleGapGetBufferFromEattConnectionRequest(gapEattConnectionRequest_t *pEattConnectionRequest, uint8_t **ppBuffer)
{
    /* Write gapEattConnectionRequest_t fields into buffer */
    fsciBleGetBufferFromUint16Value(pEattConnectionRequest->mtu, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pEattConnectionRequest->cBearers, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pEattConnectionRequest->initialCredits, *ppBuffer);
}

void fsciBleGapGetBufferFromEattConnectionComplete(gapEattConnectionComplete_t *pEattConnectionComplete, uint8_t **ppBuffer)
{
    /* Write gapEattConnectionComplete_t fields into buffer */
    fsciBleGetBufferFromUint8Value((uint8_t)pEattConnectionComplete->status, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pEattConnectionComplete->mtu, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pEattConnectionComplete->cBearers, *ppBuffer);

    for (uint32_t i = 0U; i < pEattConnectionComplete->cBearers; i++)
    {
        fsciBleGetBufferFromUint8Value(pEattConnectionComplete->aBearerIds[i], *ppBuffer);
    }
}

void fsciBleGapGetBufferFromEattReconfigureResponse(gapEattReconfigureResponse_t *pEattReconfigureResponse, uint8_t **ppBuffer)
{
    /* Write gapEattReconfigureResponse_t fields into buffer */
    fsciBleGetBufferFromUint8Value((uint8_t)pEattReconfigureResponse->status, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pEattReconfigureResponse->localMtu, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pEattReconfigureResponse->cBearers, *ppBuffer);

    for (uint32_t i = 0U; i < pEattReconfigureResponse->cBearers; i++)
    {
        fsciBleGetBufferFromUint8Value(pEattReconfigureResponse->aBearerIds[i], *ppBuffer);
    }
}

void fsciBleGapGetBufferFromEattBearerStatusNotification(gapEattBearerStatusNotification_t *pEattBearerStatusNotification, uint8_t **ppBuffer)
{
    /* Write gapEattBearerStatusNotification_t fields into buffer */
    fsciBleGetBufferFromUint8Value(pEattBearerStatusNotification->bearerId, *ppBuffer);
    fsciBleGetBufferFromUint8Value((uint8_t)pEattBearerStatusNotification->status, *ppBuffer);
}
#endif
#endif

void fsciBleGapGetConnectedEventFromBuffer(gapConnectedEvent_t* pConnectedEvent, uint8_t** ppBuffer)
{
    /* Read gapConnectedEvent_t fields from buffer */
    fsciBleGapGetConnectionParametersFromBuffer(&pConnectedEvent->connParameters, ppBuffer);
    fsciBleGetEnumValueFromBuffer(pConnectedEvent->peerAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(pConnectedEvent->peerAddress, *ppBuffer);
    fsciBleGetBoolValueFromBuffer(pConnectedEvent->peerRpaResolved, *ppBuffer);
    if( pConnectedEvent->peerRpaResolved )
    {
        fsciBleGetAddressFromBuffer(pConnectedEvent->peerRpa, *ppBuffer);
    }
    fsciBleGetBoolValueFromBuffer(pConnectedEvent->localRpaUsed, *ppBuffer);
    if( pConnectedEvent->localRpaUsed )
    {
        fsciBleGetAddressFromBuffer(pConnectedEvent->localRpa, *ppBuffer);
    }
    fsciBleGetEnumValueFromBuffer(pConnectedEvent->connectionRole, *ppBuffer, bleLlConnectionRole_t);
}


void fsciBleGapGetBufferFromConnectedEvent(gapConnectedEvent_t* pConnectedEvent, uint8_t** ppBuffer)
{
    /* Write gapConnectedEvent_t fields in buffer */
    fsciBleGapGetBuffFromConnParameters(&pConnectedEvent->connParameters, ppBuffer);
    fsciBleGetBufferFromEnumValue(pConnectedEvent->peerAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(pConnectedEvent->peerAddress, *ppBuffer);
    fsciBleGetBufferFromBoolValue(pConnectedEvent->peerRpaResolved, *ppBuffer);
    if( pConnectedEvent->peerRpaResolved )
    {
        fsciBleGetBufferFromAddress(pConnectedEvent->peerRpa, *ppBuffer);
    }
    fsciBleGetBufferFromBoolValue(pConnectedEvent->localRpaUsed, *ppBuffer);
    if( pConnectedEvent->localRpaUsed )
    {
        fsciBleGetBufferFromAddress(pConnectedEvent->localRpa, *ppBuffer);
    }
    fsciBleGetBufferFromEnumValue(pConnectedEvent->connectionRole, *ppBuffer, bleLlConnectionRole_t);
}


void fsciBleGapGetKeyExchangeRequestEventFromBuffer(gapKeyExchangeRequestEvent_t* pKeyExchangeRequestEvent, uint8_t** ppBuffer)
{
    /* Read gapKeyExchangeRequestEvent_t fields from buffer */
    fsciBleGetEnumValueFromBuffer(pKeyExchangeRequestEvent->requestedKeys, *ppBuffer, gapSmpKeyFlags_t);
    fsciBleGetUint8ValueFromBuffer(pKeyExchangeRequestEvent->requestedLtkSize, *ppBuffer);
}


void fsciBleGapGetBufferFromKeyExchangeRequestEvent(gapKeyExchangeRequestEvent_t* pKeyExchangeRequestEvent, uint8_t** ppBuffer)
{
    /* Write gapKeyExchangeRequestEvent_t fields in buffer */
    fsciBleGetBufferFromEnumValue(pKeyExchangeRequestEvent->requestedKeys, *ppBuffer, gapSmpKeyFlags_t);
    fsciBleGetBufferFromUint8Value(pKeyExchangeRequestEvent->requestedLtkSize, *ppBuffer);
}


uint32_t fsciBleGapGetPairingCompleteEventBufferSize(gapPairingCompleteEvent_t* pPairingCompleteEvent)
{
    /* Return the size needed for the buffer */
    return (sizeof(bool_t) + ((TRUE == pPairingCompleteEvent->pairingSuccessful) ? sizeof(bool_t) : sizeof(bleResult_t)));
}


void fsciBleGapGetPairComplEventFromBuffer(gapPairingCompleteEvent_t* pPairingCompleteEvent, uint8_t** ppBuffer)
{
    /* Read gapPairingCompleteEvent_t fields from buffer */
    fsciBleGetBoolValueFromBuffer(pPairingCompleteEvent->pairingSuccessful, *ppBuffer);

    if(TRUE == pPairingCompleteEvent->pairingSuccessful)
    {
        fsciBleGetBoolValueFromBuffer(pPairingCompleteEvent->pairingCompleteData.withBonding, *ppBuffer);
    }
    else
    {
        fsciBleGetEnumValueFromBuffer(pPairingCompleteEvent->pairingCompleteData.failReason, *ppBuffer, bleResult_t);
    }
}


void fsciBleGapGetBufferFromPairingCompleteEvent(gapPairingCompleteEvent_t* pPairingCompleteEvent, uint8_t** ppBuffer)
{
    /* Write gapPairingCompleteEvent_t fields in buffer */
    fsciBleGetBufferFromBoolValue(pPairingCompleteEvent->pairingSuccessful, *ppBuffer);

    if(TRUE == pPairingCompleteEvent->pairingSuccessful)
    {
        fsciBleGetBufferFromBoolValue(pPairingCompleteEvent->pairingCompleteData.withBonding, *ppBuffer);
    }
    else
    {
        fsciBleGetBufferFromEnumValue(pPairingCompleteEvent->pairingCompleteData.failReason, *ppBuffer, bleResult_t);
    }
}


void fsciBleGapGetLongTermKeyRequestEventFromBuffer(gapLongTermKeyRequestEvent_t* pLongTermKeyRequestEvent, uint8_t** ppBuffer)
{
    /* Read gapLongTermKeyRequestEvent_t fields from buffer */
    fsciBleGetUint16ValueFromBuffer(pLongTermKeyRequestEvent->ediv, *ppBuffer);
    fsciBleGetUint8ValueFromBuffer(pLongTermKeyRequestEvent->randSize, *ppBuffer);
    fsciBleGetArrayFromBuffer(pLongTermKeyRequestEvent->aRand, *ppBuffer, pLongTermKeyRequestEvent->randSize);
}


void fsciBleGapGetBufferFromLongTermKeyRequestEvent(gapLongTermKeyRequestEvent_t* pLongTermKeyRequestEvent, uint8_t** ppBuffer)
{
    /* Write gapLongTermKeyRequestEvent_t fields in buffer */
    fsciBleGetBufferFromUint16Value(pLongTermKeyRequestEvent->ediv, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pLongTermKeyRequestEvent->randSize, *ppBuffer);
    fsciBleGetBufferFromArray(pLongTermKeyRequestEvent->aRand, *ppBuffer, pLongTermKeyRequestEvent->randSize);
}


void fsciBleGapGetInternalErrorFromBuffer(gapInternalError_t* pInternalError, uint8_t** ppBuffer)
{
    /* Read gapInternalError_t fields from buffer */
    fsciBleGetEnumValueFromBuffer(pInternalError->errorCode, *ppBuffer, bleResult_t);
    fsciBleGetEnumValueFromBuffer(pInternalError->errorSource, *ppBuffer, gapInternalErrorSource_t);
    fsciBleGetUint16ValueFromBuffer(pInternalError->hciCommandOpcode, *ppBuffer);
}


void fsciBleGapGetBufferFromInternalError(gapInternalError_t* pInternalError, uint8_t** ppBuffer)
{
    /* Write gapInternalError_t fields in buffer */
    fsciBleGetBufferFromEnumValue(pInternalError->errorCode, *ppBuffer, bleResult_t);
    fsciBleGetBufferFromEnumValue(pInternalError->errorSource, *ppBuffer, gapInternalErrorSource_t);
    fsciBleGetBufferFromUint16Value(pInternalError->hciCommandOpcode, *ppBuffer);
}


void fsciBleGapGetConnParamUpdateReqFromBuffer(gapConnParamsUpdateReq_t* pConnParameterUpdateRequest, uint8_t** ppBuffer)
{
    /* Read gapConnParamsUpdateReq_t structure fields from buffer */
    fsciBleGetUint16ValueFromBuffer(pConnParameterUpdateRequest->intervalMin, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnParameterUpdateRequest->intervalMax, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnParameterUpdateRequest->peripheralLatency, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnParameterUpdateRequest->timeoutMultiplier, *ppBuffer);
}


void fsciBleGapGetBufferFromConnParameterUpdateRequest(gapConnParamsUpdateReq_t* pConnParameterUpdateRequest, uint8_t** ppBuffer)
{
    /* Write gapConnParamsUpdateReq_t structure fields in buffer */
    fsciBleGetBufferFromUint16Value(pConnParameterUpdateRequest->intervalMin, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnParameterUpdateRequest->intervalMax, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnParameterUpdateRequest->peripheralLatency, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnParameterUpdateRequest->timeoutMultiplier, *ppBuffer);
}


void fsciBleGapGetConnParameterUpdateCompleteFromBuffer(gapConnParamsUpdateComplete_t* pConnectionParameterUpdateComplete, uint8_t** ppBuffer)
{
    /* Read gapConnParamsUpdateComplete_t structure fields from buffer */
    fsciBleGetEnumValueFromBuffer(pConnectionParameterUpdateComplete->status, *ppBuffer, bleResult_t);
    fsciBleGetUint16ValueFromBuffer(pConnectionParameterUpdateComplete->connInterval, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnectionParameterUpdateComplete->connLatency, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnectionParameterUpdateComplete->supervisionTimeout, *ppBuffer);
}


void fsciBleGapGetBuffFromConnParameterUpdateComplete(gapConnParamsUpdateComplete_t* pConnParameterUpdateComplete, uint8_t** ppBuffer)
{
    /* Write gapConnParamsUpdateComplete_t structure fields in buffer */
    fsciBleGetBufferFromEnumValue(pConnParameterUpdateComplete->status, *ppBuffer, bleResult_t);
    fsciBleGetBufferFromUint16Value(pConnParameterUpdateComplete->connInterval, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnParameterUpdateComplete->connLatency, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnParameterUpdateComplete->supervisionTimeout, *ppBuffer);
}

void fsciBleGapGetConnLeDataLengthChangedFromBuffer(gapConnLeDataLengthChanged_t* pConnLeDataLengthChanged, uint8_t** ppBuffer)
{
    /* Read gapConnParamsUpdateReq_t structure fields from buffer */
    fsciBleGetUint16ValueFromBuffer(pConnLeDataLengthChanged->maxTxOctets, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnLeDataLengthChanged->maxTxTime, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnLeDataLengthChanged->maxRxOctets, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pConnLeDataLengthChanged->maxRxTime, *ppBuffer);
}

void fsciBleGapGetBufferFromConnLeSetDataLengthChangedFailure(bleResult_t reason, uint8_t** ppBuffer)
{
    /* Write bleResult_t structure fields in buffer */
    fsciBleGetBufferFromUint16Value((uint16_t)reason, *ppBuffer);
}

void fsciBleGapGetBufferFromConnLeDataLengthChanged(gapConnLeDataLengthChanged_t* pConnLeDataLengthChanged, uint8_t** ppBuffer)
{
    /* Write gapConnLeDataLengthChanged_t structure fields in buffer */
    fsciBleGetBufferFromUint16Value(pConnLeDataLengthChanged->maxTxOctets, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnLeDataLengthChanged->maxTxTime, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnLeDataLengthChanged->maxRxOctets, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pConnLeDataLengthChanged->maxRxTime, *ppBuffer);
}


void fsciBleGapGetIdentityInformationFromBuffer(gapIdentityInformation_t* pIdentityInformation, uint8_t** ppBuffer)
{
    /* Read gapIdentityInformation_t structure fields from buffer */
    fsciBleGetEnumValueFromBuffer(pIdentityInformation->identityAddress.idAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetAddressFromBuffer(pIdentityInformation->identityAddress.idAddress, *ppBuffer);
    fsciBleGetIrkFromBuffer(pIdentityInformation->irk, *ppBuffer);
    fsciBleGetEnumValueFromBuffer(pIdentityInformation->privacyMode, *ppBuffer, blePrivacyMode_t);
}


void fsciBleGapGetBufferFromIdentityInformation(const gapIdentityInformation_t* pIdentityInformation, uint8_t** ppBuffer)
{
    /* Write gapIdentityInformation_t structure fields in buffer */
    fsciBleGetBufferFromEnumValue(pIdentityInformation->identityAddress.idAddressType, *ppBuffer, bleAddressType_t);
    fsciBleGetBufferFromAddress(pIdentityInformation->identityAddress.idAddress, *ppBuffer);
    fsciBleGetBufferFromIrk((const void*)pIdentityInformation->irk, *ppBuffer);
    fsciBleGetBufferFromEnumValue(pIdentityInformation->privacyMode, *ppBuffer, blePrivacyMode_t);
}

void fsciBleGapGetBufferFromHostVersion(gapHostVersion_t *pHostVersion, uint8_t** ppBuffer)
{
    /* Write gapHostVersion_t structure fields in buffer */
    fsciBleGetBufferFromUint8Value(pHostVersion->bleHostVerMajor, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHostVersion->bleHostVerMinor, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pHostVersion->bleHostVerPatch, *ppBuffer);
}

void fsciBleGapGetBufferFromGetConnParamsCompleteEvent(getConnParams_t* pGetConnParams, uint8_t** ppBuffer)
{
    fsciBleGetBufferFromUint16Value(pGetConnParams->connectionHandle, *ppBuffer);
    fsciBleGetBufferFromUint32Value(pGetConnParams->ulTxAccCode, *ppBuffer);
    fsciBleGetBufferFromArray(pGetConnParams->aCrcInitVal, *ppBuffer, (uint32_t)(3U * sizeof(uint8_t)));
    fsciBleGetBufferFromUint16Value(pGetConnParams->uiConnInterval, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pGetConnParams->uiSuperTO, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pGetConnParams->uiConnLatency, *ppBuffer);
    fsciBleGetBufferFromArray(pGetConnParams->aChMapBm, *ppBuffer, (uint32_t)(5U * sizeof(uint8_t)));
    fsciBleGetBufferFromUint8Value(pGetConnParams->ucChannelSelection, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGetConnParams->ucHop, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGetConnParams->ucUnMapChIdx, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGetConnParams->ucCentralSCA, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGetConnParams->ucRole, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGetConnParams->aucRemoteMasRxPHY, *ppBuffer);
    fsciBleGetBufferFromUint8Value(pGetConnParams->seqNum, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pGetConnParams->uiConnEvent, *ppBuffer);
    fsciBleGetBufferFromUint32Value(pGetConnParams->ulAnchorClk, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pGetConnParams->uiAnchorDelay, *ppBuffer);
    fsciBleGetBufferFromUint32Value(pGetConnParams->ulRxInstant, *ppBuffer);
}

uint32_t fsciBleGapGetGenericEventBufferSize(gapGenericEvent_t* pGenericEvent)
{
    /* Get the constant size for the needed buffer */
    uint32_t bufferSize = 0;

    /* Call the appropriate handler to get the variable size for the needed buffer */
    if (((uint32_t)pGenericEvent->eventType < mGapGetGenericEventBufferSizeHandlersArraySize) &&
        (maGapGetGenericEventBufferSizeHandlers[pGenericEvent->eventType] != NULL))
    {
        bufferSize += maGapGetGenericEventBufferSizeHandlers[pGenericEvent->eventType](pGenericEvent);
    }

    /* Return the size needed for the buffer */
    return bufferSize;
}


void fsciBleGapGetGenericEventFromBuffer(gapGenericEvent_t* pGenericEvent, uint8_t** ppBuffer)
{
    /* Call the appropriate handler to read gapGenericEvent_t fields
     * from buffer (without eventType) */
    if (((uint32_t)pGenericEvent->eventType < mGapGetGenericEventFromBufferHandlersArraySize) &&
        (maGapGetGenericEventFromBufferHandlers[pGenericEvent->eventType] != NULL))
    {
        maGapGetGenericEventFromBufferHandlers[pGenericEvent->eventType](pGenericEvent, ppBuffer);
    }
}


void fsciBleGapGetBufferFromGenericEvent(gapGenericEvent_t* pGenericEvent, uint8_t** ppBuffer)
{
    /* Call the appropriate handler to write gapGenericEvent_t fields
     * in buffer (without eventType) */
    if (((uint32_t)pGenericEvent->eventType < mGapGetBufferFromGenericEventHandlersArraySize) &&
        (maGapGetBufferFromGenericEventHandlers[pGenericEvent->eventType] != NULL))
    {
        maGapGetBufferFromGenericEventHandlers[pGenericEvent->eventType](pGenericEvent, ppBuffer);
    }
}


uint32_t fsciBleGapGetAdvertisingEventBufferSize(gapAdvertisingEvent_t* pAdvertisingEvent)
{
    /* Get the constant size for the needed buffer */
    uint32_t bufferSize = 0;

    /* Get the variable size for the needed buffer */
    switch(pAdvertisingEvent->eventType)
    {
        case gAdvertisingCommandFailed_c:
            {
                bufferSize += sizeof(bleResult_t);
            }
            break;

#if defined(gBLE50_d) && (gBLE50_d == 1U)
        case gExtScanNotification_c:
            {
                bufferSize += sizeof(uint8_t);          /* handle */
                bufferSize += sizeof(bleAddressType_t); /* scannerAddrType */
                bufferSize += gcBleDeviceAddressSize_c; /* aScannerAddr */
                bufferSize += sizeof(bool_t);           /* scannerAddrResolved */
            }
            break;
        case gAdvertisingSetTerminated_c:
            {
                bufferSize += sizeof(bleResult_t);      /* status */
                bufferSize += sizeof(uint8_t);          /* handle */
                bufferSize += sizeof(deviceId_t);       /* deviceId */
                bufferSize += sizeof(uint8_t);          /* numCompletedExtAdvEvents */
            }
            break;
#endif

        default:
            ; /* For MISRA compliance */
            break;
    }

    /* Return the size needed for the buffer */
    return bufferSize;
}


void fsciBleGapGetAdvertisingEventFromBuffer(gapAdvertisingEvent_t* pAdvertisingEvent, uint8_t** ppBuffer)
{
    /* Read gapAdvertisingEvent_t fields from buffer (without eventType) */
    switch(pAdvertisingEvent->eventType)
    {
        case gAdvertisingCommandFailed_c:
            {
                fsciBleGetEnumValueFromBuffer(pAdvertisingEvent->eventData.failReason, *ppBuffer, bleResult_t);
            }
            break;
#if defined(gBLE50_d) && (gBLE50_d == 1U)
        case gAdvertisingSetTerminated_c:
            {
                fsciBleGetEnumValueFromBuffer(pAdvertisingEvent->eventData.advSetTerminated.status, *ppBuffer, bleResult_t);
                fsciBleGetUint8ValueFromBuffer(pAdvertisingEvent->eventData.advSetTerminated.handle, *ppBuffer);
                fsciBleGetUint8ValueFromBuffer(pAdvertisingEvent->eventData.advSetTerminated.deviceId, *ppBuffer);
                fsciBleGetUint8ValueFromBuffer(pAdvertisingEvent->eventData.advSetTerminated.numCompletedExtAdvEvents, *ppBuffer);
            }
            break;
#endif
        default:
            ; /* For MISRA compliance */
            break;

    }
}


void fsciBleGapGetBuffFromAdvEvent(gapAdvertisingEvent_t* pAdvertisingEvent, uint8_t** ppBuffer)
{
    /* Write gapAdvertisingEvent_t fields in buffer (without eventType) */
    switch(pAdvertisingEvent->eventType)
    {
        case gAdvertisingCommandFailed_c:
            {
                fsciBleGetBufferFromEnumValue(pAdvertisingEvent->eventData.failReason, *ppBuffer, bleResult_t);
            }
            break;

#if defined(gBLE50_d) && (gBLE50_d == 1U)
        case gExtScanNotification_c:
            {
                fsciBleGetBufferFromUint8Value(pAdvertisingEvent->eventData.scanNotification.handle, *ppBuffer);
                fsciBleGetBufferFromEnumValue( pAdvertisingEvent->eventData.scanNotification.scannerAddrType, *ppBuffer, bleAddressType_t);
                fsciBleGetBufferFromAddress(   pAdvertisingEvent->eventData.scanNotification.aScannerAddr, *ppBuffer);
                fsciBleGetBufferFromBoolValue( pAdvertisingEvent->eventData.scanNotification.scannerAddrResolved, *ppBuffer);
            }
            break;
        case gAdvertisingSetTerminated_c:
            {
                fsciBleGetBufferFromEnumValue(pAdvertisingEvent->eventData.advSetTerminated.status, *ppBuffer, bleResult_t);
                fsciBleGetBufferFromUint8Value(pAdvertisingEvent->eventData.advSetTerminated.handle, *ppBuffer);
                fsciBleGetBufferFromUint8Value(pAdvertisingEvent->eventData.advSetTerminated.deviceId, *ppBuffer);
                fsciBleGetBufferFromUint8Value(pAdvertisingEvent->eventData.advSetTerminated.numCompletedExtAdvEvents, *ppBuffer);
            }
            break;
#endif

        default:
            ; /* For MISRA compliance */
            break;

    }
}


gapScanningEvent_t* fsciBleGapAllocScanningEventForBuffer(gapScanningEventType_t eventType, uint8_t* pBuffer)
{
    uint16_t            variableLength = 0U;
    gapScanningEvent_t* pScanningEvent = NULL;

    if(gDeviceScanned_c == eventType)
    {
        /* Go to dataLength field in gapScannedDevice_t structure */
        pBuffer += sizeof(bleAddressType_t) + gcBleDeviceAddressSize_c + sizeof(int8_t);

        /* Get dataLength field from buffer */
        fsciBleGetUint8ValueFromBuffer(variableLength, pBuffer);
    }
#if defined(gBLE50_d) && (gBLE50_d == 1U)
    else if (gExtDeviceScanned_c == eventType)
    {
        /* Go to dataLength field in gapExtScannedDevice_t structure */
        pBuffer += sizeof(bleAddressType_t) +
                   sizeof(bleDeviceAddress_t) +
                   sizeof(uint8_t) +
                   sizeof(bool_t) +
                   sizeof(bleAdvReportEventProperties_t) +
                   sizeof(int8_t) +
                   sizeof(int8_t) +
                   sizeof(uint8_t) +
                   sizeof(uint8_t) +
                   sizeof(uint16_t) +
                   sizeof(bool_t) +
                   sizeof(bleAddressType_t) +
                   sizeof(bleDeviceAddress_t);

        /* Get dataLength field from buffer */
        fsciBleGetUint16ValueFromBuffer(variableLength, pBuffer);
    }
    else if (gPeriodicDeviceScanned_c == eventType)
    {
        /* Go to dataLength field in gapPeriodicScannedDevice_t structure */
        pBuffer += sizeof(uint16_t) +
                   sizeof(int8_t) +
                   sizeof(int8_t) +
                   sizeof(bleCteType_t);

        /* Get dataLength field from buffer */
        fsciBleGetUint16ValueFromBuffer(variableLength, pBuffer);
    }
#endif
    else
    {
         ; /* For MISRA compliance */
    }

    /* Allocate memory for the scanning event */
    pScanningEvent = (gapScanningEvent_t*)MEM_BufferAlloc(sizeof(gapScanningEvent_t) + (uint32_t)variableLength);

    if(NULL != pScanningEvent)
    {
        /* Set event type in scanning event */
        pScanningEvent->eventType = eventType;

        if (gDeviceScanned_c == eventType)
        {
            /* Set pointer for the variable length data */
             pScanningEvent->eventData.scannedDevice.data = (uint8_t*)pScanningEvent + sizeof(gapScanningEvent_t);
        }
#if defined(gBLE50_d) && (gBLE50_d == 1U)
        else if (gExtDeviceScanned_c == eventType)
        {
            /* Set pointer for the variable length data */
            pScanningEvent->eventData.extScannedDevice.pData = (uint8_t*)pScanningEvent + sizeof(gapScanningEvent_t);
        }
        else if (gPeriodicDeviceScanned_c == eventType)
        {
            /* Set pointer for the variable length data */
            pScanningEvent->eventData.periodicScannedDevice.pData = (uint8_t*)pScanningEvent + sizeof(gapScanningEvent_t);
        }
#endif
        else
        {
             ; /* For MISRA compliance */
        }
    }

    /* Return memory allocated for the scanning event */
    return pScanningEvent;
}


uint32_t fsciBleGapGetScanningEventBufferSize(gapScanningEvent_t* pScanningEvent)
{
    /* Get the constant size for the needed buffer */
    uint32_t bufferSize = 0U;

    /* Get the variable size for the needed buffer */
    switch(pScanningEvent->eventType)
    {
        case gScanCommandFailed_c:
            {
                bufferSize += sizeof(bleResult_t);
            }
            break;

        case gDeviceScanned_c:
            {
                bufferSize += fsciBleGapGetScannedDeviceBufferSize(&pScanningEvent->eventData.scannedDevice);
                if( FALSE == pScanningEvent->eventData.scannedDevice.directRpaUsed )
                {
                    bufferSize -= gcBleDeviceAddressSize_c;
                }
            }
            break;
#if defined(gBLE50_d) && (gBLE50_d == 1U)
        case gExtDeviceScanned_c:
            {
                bufferSize += fsciBleGapGetExtScannedDeviceBufferSize(&pScanningEvent->eventData.extScannedDevice);
            }
            break;

        case gPeriodicDeviceScanned_c:
            {
                bufferSize += fsciBleGapGetPeriodicScannedDeviceBufferSize(&pScanningEvent->eventData.periodicScannedDevice);
            }
            break;

        case gPeriodicAdvSyncEstablished_c:
            {
                bufferSize += sizeof(bleResult_t);  /* Status */
                bufferSize += sizeof(uint16_t);     /* Sync Handle */
            }
            break;

        case gPeriodicAdvSyncLost_c:
            {
                bufferSize += sizeof(uint16_t);     /* Sync Handle */
            }
            break;

#endif
#if defined(gBLE51_d) && (gBLE51_d == 1U)
        case gConnectionlessIqReportReceived_c:
            {
                bufferSize += fsciBleGapGetConnectionlessIqReportReceivedBufferSize(&pScanningEvent->eventData.iqReport);
            }
            break;
#endif
        default:
            ; /* For MISRA compliance */
            break;
    }

    /* Return the size needed for the buffer */
    return bufferSize;
}


void fsciBleGapGetScanningEventFromBuffer(gapScanningEvent_t* pScanningEvent, uint8_t** ppBuffer)
{
    /* Read gapScanningEvent_t fields from buffer (without eventType) */
    switch(pScanningEvent->eventType)
    {
        case gScanCommandFailed_c:
            {
                fsciBleGetEnumValueFromBuffer(pScanningEvent->eventData.failReason, *ppBuffer, bleResult_t);
            }
            break;

        case gDeviceScanned_c:
            {
                fsciBleGapGetScannedDeviceFromBuffer(&pScanningEvent->eventData.scannedDevice, ppBuffer);
            }
            break;
#if defined(gBLE50_d) && (gBLE50_d == 1U)
        case gExtDeviceScanned_c:
            {
                fsciBleGapGetExtScannedDeviceFromBuffer(&pScanningEvent->eventData.extScannedDevice, ppBuffer);
            }
            break;

        case gPeriodicDeviceScanned_c:
            {
                fsciBleGapGetPeriodicScannedDeviceFromBuffer(&pScanningEvent->eventData.periodicScannedDevice, ppBuffer);
            }
            break;

        case gPeriodicAdvSyncLost_c:
            {
                fsciBleGetUint16ValueFromBuffer(pScanningEvent->eventData.syncLost.syncHandle, *ppBuffer);
            }
            break;
#endif
#if defined(gBLE51_d) && (gBLE51_d == 1U)
        case gConnectionlessIqReportReceived_c:
            {
                fsciBleGapGetConnectionlessIqReportReceivedFromBuffer(&pScanningEvent->eventData.iqReport, ppBuffer);
            }
            break;
#endif
        default:
            ; /* For MISRA compliance */
            break;
    }
}


void fsciBleGapGetBufferFromScanningEvent(gapScanningEvent_t* pScanningEvent, uint8_t** ppBuffer)
{
    /* Write gapScanningEvent_t fields in buffer (without eventType) */
    switch(pScanningEvent->eventType)
    {
        case gScanCommandFailed_c:
            {
                fsciBleGetBufferFromEnumValue(pScanningEvent->eventData.failReason, *ppBuffer, bleResult_t);
            }
            break;

        case gDeviceScanned_c:
            {
                fsciBleGapGetBufferFromScannedDevice(&pScanningEvent->eventData.scannedDevice, ppBuffer);
            }
            break;

#if defined(gBLE50_d) && (gBLE50_d == 1U)
        case gExtDeviceScanned_c:
            {
                fsciBleGapGetBufferFromExtScannedDevice(&pScanningEvent->eventData.extScannedDevice, ppBuffer);
            }
            break;

        case gPeriodicDeviceScanned_c:
            {
                fsciBleGapGetBufferFromPerScannedDevice(&pScanningEvent->eventData.periodicScannedDevice, ppBuffer);
            }
            break;

        case gPeriodicAdvSyncEstablished_c:
            {
                fsciBleGetBufferFromEnumValue(pScanningEvent->eventData.syncEstb.status, *ppBuffer, bleResult_t);
                fsciBleGetBufferFromUint16Value(pScanningEvent->eventData.syncEstb.syncHandle, *ppBuffer);
            }
            break;

        case gPeriodicAdvSyncLost_c:
            {
                fsciBleGetBufferFromUint16Value(pScanningEvent->eventData.syncLost.syncHandle, *ppBuffer);
            }
            break;
#endif
#if defined(gBLE51_d) && (gBLE51_d == 1U)
        case gConnectionlessIqReportReceived_c:
            {
                fsciBleGapGetBufferFromConnectionlessIqReportReceived(&pScanningEvent->eventData.iqReport, ppBuffer);
            }
            break;
#endif
        default:
            ; /* For MISRA compliance */
            break;
    }
}


gapConnectionEvent_t* fsciBleGapAllocConnectionEventForBuffer(gapConnectionEventType_t eventType, uint8_t* pBuffer)
{
    /* Allocate memory for the connection event */
    gapConnectionEvent_t* pConnectionEvent = (gapConnectionEvent_t*)MEM_BufferAlloc(sizeof(gapConnectionEvent_t));

    if(NULL != pConnectionEvent)
    {
        /* Set event type in buffer */
        pConnectionEvent->eventType = eventType;

        if(gConnEvtKeysReceived_c == eventType)
        {
            /* Allocate memory for the received keys */
            pConnectionEvent->eventData.keysReceivedEvent.pKeys = fsciBleGapAllocSmpKeysForBuffer(pBuffer);

            if(NULL == pConnectionEvent->eventData.keysReceivedEvent.pKeys)
            {
                /* No memory */
                fsciBleError(gFsciOutOfMessages_c, fsciBleInterfaceId);

                /* Free memory allocated for the connection event */
                (void)MEM_BufferFree(pConnectionEvent);

                pConnectionEvent = NULL;
            }
        }
    }

    /* Return the allocated connection event */
    return pConnectionEvent;
}


uint32_t fsciBleGapGetConnectionEventBufferSize(gapConnectionEvent_t* pConnectionEvent)
{
    /* Get the constant size for the needed buffer */
    uint32_t bufferSize = 0;

    /* Call the appropriate handler to get the variable size for the needed buffer */
    if (((uint32_t)pConnectionEvent->eventType < mGapGetConnEventBufferSizeHandlersArraySize) &&
        (maGapGetConnEventBufferSizeHandlers[pConnectionEvent->eventType] != NULL))
    {
        bufferSize += maGapGetConnEventBufferSizeHandlers[pConnectionEvent->eventType](pConnectionEvent);
    }

    /* Return the size needed for the buffer */
    return bufferSize;
}


void fsciBleGapGetConnectionEventFromBuffer(gapConnectionEvent_t* pConnectionEvent, uint8_t** ppBuffer)
{
    /* Call the appropriate handler to read gapConnectionEvent_t
     * fields from buffer (without eventType) */
    if (((uint32_t)pConnectionEvent->eventType < mGapGetConnEventFromBufferHandlersArraySize) &&
        (maGapGetConnEventFromBufferHandlers[pConnectionEvent->eventType] != NULL))
    {
        maGapGetConnEventFromBufferHandlers[pConnectionEvent->eventType](pConnectionEvent, ppBuffer);
    }
}


void fsciBleGapGetBufferFromConnectionEvent(gapConnectionEvent_t* pConnectionEvent, uint8_t** ppBuffer)
{
    /* Call the appropriate handler to write gapConnectionEvent_t fields
     * in buffer (without eventType) */
    if(((uint32_t)pConnectionEvent->eventType < mGapGetBufferFromConnEventHandlersArraySize) &&
        (maGapGetBufferFromConnEventHandlers[pConnectionEvent->eventType] != NULL))
    {
        maGapGetBufferFromConnEventHandlers[pConnectionEvent->eventType](pConnectionEvent, ppBuffer);
    }
}


void fsciBleGapFreeConnectionEvent(gapConnectionEvent_t* pConnectionEvent)
{
    if(gConnEvtKeysReceived_c == pConnectionEvent->eventType)
    {
        /* Free memory allocated for the received keys */
        (void)MEM_BufferFree(pConnectionEvent->eventData.keysReceivedEvent.pKeys);
    }

    /* Free memory allocated for the connection event */
    (void)MEM_BufferFree(pConnectionEvent);
}


gapAutoConnectParams_t* fsciBleGapAllocAutoConnectParamsForBuffer(uint8_t* pBuffer)
{
    union
    {
        uint8_t*                                pConnectionRequestParamsTemp;
        gapConnectionRequestParameters_t*       aAutoConnectDataTemp;
    }connectionVars = {0};

    uint8_t                 cNumAddresses;
    gapAutoConnectParams_t*	pAutoConnectParams;

    /* Get cNumAddresses from buffer */
    fsciBleGetUint8ValueFromBuffer(cNumAddresses, pBuffer);

    /* Allocate memory for pAutoConnectParams */
    pAutoConnectParams = (gapAutoConnectParams_t*)MEM_BufferAlloc(sizeof(gapAutoConnectParams_t) + (uint32_t)cNumAddresses * sizeof(gapConnectionRequestParameters_t));

    if(NULL != pAutoConnectParams)
    {
        /* Set aAutoConnectData pointer in gapAutoConnectParams_t structure */
        connectionVars.pConnectionRequestParamsTemp = (uint8_t*)pAutoConnectParams + sizeof(gapAutoConnectParams_t);
        pAutoConnectParams->aAutoConnectData = connectionVars.aAutoConnectDataTemp;
    }

    /* Return the buffer allocated for gapAutoConnectParams_t structure, or NULL */
    return pAutoConnectParams;
}


void fsciBleGapGetAutoConnectParamsFromBuffer(gapAutoConnectParams_t* pAutoConnectParams, uint8_t** ppBuffer)
{
    uint32_t iCount;

    /* Read gapAutoConnectParams_t fields from buffer */
    fsciBleGetUint8ValueFromBuffer(pAutoConnectParams->cNumAddresses, *ppBuffer);
    fsciBleGetBoolValueFromBuffer(pAutoConnectParams->writeInFilterAcceptList, *ppBuffer);

    for(iCount = 0; iCount < pAutoConnectParams->cNumAddresses; iCount ++)
    {
        fsciBleGapGetConnectionRequestParametersFromBuffer(&pAutoConnectParams->aAutoConnectData[iCount], ppBuffer);
    }
}


void fsciBleGapGetBufferFromAutoConnectParams(gapAutoConnectParams_t* pAutoConnectParams, uint8_t** ppBuffer)
{
    uint32_t iCount;

    /* Write gapAutoConnectParams_t fields in buffer */
    fsciBleGetBufferFromUint8Value(pAutoConnectParams->cNumAddresses, *ppBuffer);
    fsciBleGetBufferFromBoolValue(pAutoConnectParams->writeInFilterAcceptList, *ppBuffer);

    for(iCount = 0; iCount < pAutoConnectParams->cNumAddresses; iCount ++)
    {
        fsciBleGapGetBuffFromConnReqParameters(pAutoConnectParams->aAutoConnectData, ppBuffer);
    }
}


#if defined(gBLE50_d) && (gBLE50_d == 1U)
void fsciBleGapGetBufferFromPerAdvParameters(gapPeriodicAdvParameters_t* pAdvertisingParameters, uint8_t** ppBuffer)
{
    /* Write gapPeriodicAdvParameters_t fields in buffer */
    fsciBleGetBufferFromUint8Value( pAdvertisingParameters->handle,          *ppBuffer);
    fsciBleGetBufferFromBoolValue(  pAdvertisingParameters->addTxPowerInAdv, *ppBuffer);
    fsciBleGetBufferFromUint16Value(pAdvertisingParameters->minInterval,     *ppBuffer);
    fsciBleGetBufferFromUint16Value(pAdvertisingParameters->maxInterval,     *ppBuffer);
}

void fsciBleGapGetPeriodicAdvParametersFromBuffer(gapPeriodicAdvParameters_t* pAdvertisingParameters, uint8_t** ppBuffer)
{
    /* Read gapPeriodicAdvParameters_t fields from buffer */
    fsciBleGetUint8ValueFromBuffer( pAdvertisingParameters->handle,          *ppBuffer);
    fsciBleGetBoolValueFromBuffer(  pAdvertisingParameters->addTxPowerInAdv, *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pAdvertisingParameters->minInterval,     *ppBuffer);
    fsciBleGetUint16ValueFromBuffer(pAdvertisingParameters->maxInterval,     *ppBuffer);
}
#endif

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

#endif /* gFsciIncluded_c && gFsciBleGapLayerEnabled_d */

/*! *********************************************************************************
* @}
********************************************************************************** */
