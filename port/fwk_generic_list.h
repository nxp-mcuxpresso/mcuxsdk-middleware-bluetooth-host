/*
 * Copyright 2020-2021, 2023 NXP
 *
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GENERIC_LIST_PORT_H_
#define GENERIC_LIST_PORT_H_
/*==================================================================================================
Include Files
==================================================================================================*/
#include "EmbeddedTypes.h"
#include "fsl_component_messaging.h"

/*==================================================================================================
Public type definitions
==================================================================================================*/
typedef enum
{
  gListOk_c = 0U,
  gListFull_c,
  gListEmpty_c,
  gOrphanElement_c
}listStatus_t;

typedef messaging_t list_t, *listHandle_t;

/*==================================================================================================
Public prototypes
==================================================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

/*! ************************************************************************************************
* \brief  Initializes the list descriptor
*
* \param[in] list    Pointer to list
*
* \return  none
************************************************************************************************* */
void ListInitMsg(listHandle_t list);

/*! ************************************************************************************************
* \brief  Initializes the list descriptor
*
* \param[in] list    Pointer to list
* \param[in] max     Maximum number of elements
*
* \return  none
************************************************************************************************* */
void ListInit(listHandle_t list, uint32_t max);

/*! ************************************************************************************************
* \brief  Links element to the head of the list
*
* \param[in] list   Pointer to list
* \param[in] pMsg   Pointer to the element to be added
*
* \return  listStatus_t
************************************************************************************************* */
listStatus_t ListAddHeadMsg(listHandle_t list, void* pMsg);

/*! ************************************************************************************************
* \brief  Removes element from list
*
* \param[in] pMsg   Pointer to the element to be removed
*
* \return  listStatus_t
************************************************************************************************* */
listStatus_t ListRemoveMsg(void* pMsg);

/*! ************************************************************************************************
* \brief  Returns next element in list after pMsg
*
* \param[in] pMsg   Pointer to element
*
* \return  listStatus_t
************************************************************************************************* */
void* ListGetNextMsg(void* pMsg);

/*! ************************************************************************************************
* \brief  Returns the head element of the list
*
* \param[in] list   Pointer to list
*
* \return  Pointer to head element
************************************************************************************************* */
void* ListGetHeadMsg(listHandle_t list);

/*! ************************************************************************************************
* \brief  Removes the head element of the list
*
* \param[in] list   Pointer to list
*
* \return  Pointer to head element
************************************************************************************************* */
void* ListRemoveHeadMsg(listHandle_t list);

/*! ************************************************************************************************
* \brief  Links element to the tail of the list
*
* \param[in] list   Pointer to list
* \param[in] pMsg   Pointer to the element to be added
*
* \return  listStatus_t
************************************************************************************************* */
listStatus_t ListAddTailMsg(listHandle_t list, void* pMsg);

#if defined(__cplusplus)
}
#endif

#endif /* GENERIC_LIST_PORT_H_ */