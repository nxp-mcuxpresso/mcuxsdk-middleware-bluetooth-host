/*! *********************************************************************************
* Copyright 2023 NXP
*
*
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */


#ifndef SECLIB_PORT_H_
#define SECLIB_PORT_H_
/*==================================================================================================
Include Files
==================================================================================================*/
#include "SecLib.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
{
    void (*pfECDH_P256_FreeDhKeyData)(computeDhKeyParam_t *pDhKeyData);
    secResultType_t (*pfSecLib_DeriveBluetoothSKD)(const uint8_t *pInSKD,
                                                   const uint8_t *pLtkBlob,
                                                   bool_t         bOpenKey,
                                                   uint8_t *      pOutSKD);
    secResultType_t (*pfSecLib_ObfuscateKey)(const uint8_t *pKey, uint8_t *pBlob, const uint8_t blobType);
    secResultType_t (*pfSecLib_DeobfuscateKey)(const uint8_t *pBlob, uint8_t *pKey);
    secResultType_t (*pfECDH_P256_ComputeA2BKey)(const ecdhPublicKey_t *pInPeerPublicKey, ecdhDhKey_t *pOutE2EKey);
    secResultType_t (*pfECDH_P256_FreeE2EKeyData)(ecdhDhKey_t *pE2EKeyData);
    secResultType_t (*pfSecLib_ExportA2BBlob)(const void *pKey, const secInputKeyType_t keyType, uint8_t *pOutKey);
    secResultType_t (*pfSecLib_ImportA2BBlob)(const uint8_t *pKey, const secInputKeyType_t keyType, uint8_t *pOutKey);
    secResultType_t (*pfSecLib_GenerateBluetoothF5Keys)(uint8_t *      pMacKey,
                                                        uint8_t *      pLtk,
                                                        const uint8_t *pW,
                                                        const uint8_t *pN1,
                                                        const uint8_t *pN2,
                                                        const uint8_t  a1at,
                                                        const uint8_t *pA1,
                                                        const uint8_t  a2at,
                                                        const uint8_t *pA2);
    secResultType_t (*pfSecLib_VerifyBluetoothAh)(uint8_t *pHash, const uint8_t *pKey, const uint8_t *pR);
    secResultType_t (*pfSecLib_GenerateBluetoothEIRKBlob)(const void * pIRK,
                                                          const bool_t blobInput,
                                                          const bool_t generateDKeyIRK,
                                                          uint8_t *    pOutEIRKblob);
} secLibFunctions_t;

extern secLibFunctions_t gSecLibFunctions;

#if defined(__cplusplus)
}
#endif

#endif /* SECLIB_PORT_H_ */