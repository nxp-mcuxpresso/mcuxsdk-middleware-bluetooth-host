/*! *********************************************************************************
 * \addtogroup BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2020-2021, 2023 NXP
*
*
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */


#include "fwk_generic_list.h"
#include "fsl_component_generic_list.h"
#include "fsl_component_messaging.h"

 /*******************************************************************************
 * Definitions
 ******************************************************************************/

 /*******************************************************************************
 * Prototypes
 ******************************************************************************/

 /*******************************************************************************
 * Variables
 ******************************************************************************/

 /*******************************************************************************
 * Code
 ******************************************************************************/

/*! ************************************************************************************************
* \brief  Initializes the list descriptor
*
* \param[in] list    Pointer to list
*
* \return  none
************************************************************************************************* */
void ListInitMsg(listHandle_t list)
{
    MSG_QueueInit((list_handle_t)list);
}

/*! ************************************************************************************************
* \brief  Initializes the list descriptor
*
* \param[in] list    Pointer to list
* \param[in] max     Maximum number of elements
*
* \return  none
************************************************************************************************* */
void ListInit(listHandle_t list, uint32_t max)
{
    LIST_Init((list_handle_t)list, max);
}

/*! ************************************************************************************************
* \brief  Links element to the head of the list
*
* \param[in] list   Pointer to list
* \param[in] pMsg   Pointer to the element to be added
*
* \return  listStatus_t
************************************************************************************************* */
listStatus_t ListAddHeadMsg(listHandle_t list, void* pMsg)
{
    return (listStatus_t)MSG_QueueAddHead((list_handle_t)list, pMsg);
}

/*! ************************************************************************************************
* \brief  Removes element from list
*
* \param[in] pMsg   Pointer to the element to be removed
*
* \return  listStatus_t
************************************************************************************************* */
listStatus_t ListRemoveMsg(void* pMsg)
{
    return (listStatus_t)MSG_QueueRemove(pMsg);
}

/*! ************************************************************************************************
* \brief  Returns next element in list after pMsg
*
* \param[in] pMsg   Pointer to element
*
* \return  listStatus_t
************************************************************************************************* */
void* ListGetNextMsg(void* pMsg)
{
    return MSG_QueueGetNext(pMsg);
}

/*! ************************************************************************************************
* \brief  Returns the head element of the list
*
* \param[in] list   Pointer to list
*
* \return  Pointer to head element
************************************************************************************************* */
void* ListGetHeadMsg(listHandle_t list)
{
    return MSG_QueueGetHead((list_handle_t)list);
}

/*! ************************************************************************************************
* \brief  Removes the head element of the list
*
* \param[in] list   Pointer to list
*
* \return  Pointer to head element
************************************************************************************************* */
void* ListRemoveHeadMsg(listHandle_t list)
{
    return MSG_QueueRemoveHead((list_handle_t)list);
}

/*! ************************************************************************************************
* \brief  Links element to the tail of the list
*
* \param[in] list   Pointer to list
* \param[in] pMsg   Pointer to the element to be added
*
* \return  listStatus_t
************************************************************************************************* */
listStatus_t ListAddTailMsg(listHandle_t list, void* pMsg)
{
    return (listStatus_t)MSG_QueueAddTail((list_handle_t)list, pMsg);
}
