/*! *********************************************************************************
 * \addtogroup AUTO
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2023-2025 NXP
*
*
* \file app_a2b.h
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef APP_A2B_H
#define APP_A2B_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "ble_general.h"


/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum appA2BEvent_tag {
    mAppA2B_E2EKeyDerivationComplete_c,
    mAppA2B_E2ELocalIrkSyncComplete_c,
    mAppA2B_Error_c,
} appA2BEvent_t;

typedef enum appA2BError_tag {
    mAppA2B_E2EKeyDerivationFailiure_c,
    mAppA2B_E2ELocalIrkSyncFailiure_c,
    mAppA2B_UnexpectedError_c,
} appA2BError_t;

typedef void (*appA2BEventCb_t)(appA2BEvent_t eventType, void *pData);
typedef void (*appA2BA2ACommInterfaceCb_t)(uint8_t opGroup, uint8_t cmdId, uint16_t len, uint8_t *pData);

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/
#define gA2BCommandsOpGroup_c                   0xAB

/* A2B commands identifier */
#define gA2BPublicKeyCommandOpCode_c            0x00    /* Send public key for Edgelock to Edgelock key derivation */
#define gA2BRequestE2EIrkBlobCommandOpCode_c    0x01    /* Request E2E(Edgelock to Edgelock) IRK blob for IRK synchronization between devices. */
#define gA2BE2EIrkBlobCommandOpCode_c           0x02    /* E2E IRK blob for IRK synchronization between devices. */

/* A2B commands length */
#define gA2BPublicKeyCommandLen_c               64U
#define gA2BE2EIrkBlobCommandLen_c              gcSmpMaxIrkBlobSize_c

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
#if defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U)
#if defined(gA2BInitiator_d) && (gA2BInitiator_d == 0)
extern gapGenericEvent_t *gpBleHostInitComplete;
extern bool_t gA2ALocalIrkSet;
#endif /* defined(gA2BInitiator_d) && (gA2BInitiator_d == 0) */
#endif /* defined(gA2BEnabled_d) && (gA2BEnabled_d > 0U) */
/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

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
bleResult_t A2B_Init(appA2BEventCb_t pfAppEventCb, appA2BA2ACommInterfaceCb_t pfAppA2AInterfaceCb
);

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
void A2B_ProcessA2ACommand(uint8_t cmdId, uint32_t cmdLen, uint8_t *pCmdData);

/*! *********************************************************************************
*\fn           bleResult_t A2B_FreeE2EKey(void)
*\brief        Free the EdgeLock to EdgeLock key
*
*\param  [in]  None.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************** */
bleResult_t A2B_FreeE2EKey(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* APP_A2B_H */