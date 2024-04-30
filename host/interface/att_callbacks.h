/*! *********************************************************************************
 * \addtogroup ATT
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2023 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef ATT_CALLBACKS_H
#define ATT_CALLBACKS_H

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef union
{
    void *                                        pGenericParam;
    attErrorResponseParams_t*                     pErrorResponseParams;
    attExchangeMtuRequestParams_t*                pExchangeMtuRequestParams;
    attExchangeMtuResponseParams_t*               pExchangeMtuResponseParams;
    attFindInformationRequestParams_t*            pFindInformationRequestParams;
    attFindInformationResponseParams_t*           pFindInformationResponseParams;
    attFindByTypeValueRequestParams_t*            pFindByTypeValueRequestParams;
    attFindByTypeValueResponseParams_t*           pFindByTypeValueResponseParams;
    attReadByTypeRequestParams_t*                 pReadByTypeRequestParams;
    attReadByTypeResponseParams_t*                pReadByTypeResponseParams;
    attReadRequestParams_t*                       pReadRequestParams;
    attReadResponseParams_t*                      pReadResponseParams;
    attReadBlobRequestParams_t*                   pReadBlobRequestParams;
    attReadBlobResponseParams_t*                  pReadBlobResponseParams;
    attReadMultipleRequestParams_t*               pReadMultipleRequestParams;
    attReadMultipleResponseParams_t*              pReadMultipleResponseParams;
    attReadByGroupTypeRequestParams_t*            pReadByGroupTypeRequestParams;
    attReadByGroupTypeResponseParams_t*           pReadByGroupTypeResponseParams;
    attWriteRequestAndCommandParams_t*            pWriteRequestAndCommandParams;
    attSignedWriteCommandParams_t*                pSignedWriteCommandParams;
    attPrepareWriteRequestResponseParams_t*       pPrepareWriteRequestResponseParams;
    attExecuteWriteRequestParams_t*               pExecuteWriteRequestParams;
    attHandleValueNotificationIndicationParams_t* pHandleValueNotificationIndicationParams;
    attMultipleHandleValueNotificationParams_t*   pMultipleValueNotificationParams;
    attReadMultipleVariableResponseParams_t*      pMultipleVariableLenResponseParams;
    attInvalidPduIndicationParams_t*              pInvalidPduIndicationParams;
}attCbParams_t;

typedef void (*pfAttToUpperLayerCallback_t)(deviceId_t deviceId, bearerId_t bearerId, attOpcode_t opcode, void* pParam);

/************************************************************************************
 *************************************************************************************
 * Public prototypes
 *************************************************************************************
 ************************************************************************************/

/*! *********************************************************************************
 * \brief Register ATT's upper layer callback function
 *
 * \return gBleSuccess_c
 *
 ********************************************************************************** */
bleResult_t Att_RegisterCallback
(
    pfAttToUpperLayerCallback_t        callback
);

/*!*************************************************************************************************
*\brief        This function is a wrapper for the mpfAttUpperLayerCallback call.

*\param [in]   deviceId       Identifier of the connected peer
*\param [in]   bearerId       Identifier of the bearer
*\param [in]   opcode         Operation opcode
*\param [in]   pParam         Pointer to the operation parameters
*
*\return       void
***************************************************************************************************/
void attToUpperLayerCallback(deviceId_t deviceId, bearerId_t bearerId, attOpcode_t opcode, void* pParam);

#endif /* ATT_CALLBACKS_H */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
