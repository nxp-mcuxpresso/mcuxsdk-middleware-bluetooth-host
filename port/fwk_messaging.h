/*! *********************************************************************************
* Copyright 2020-2021, 2023 NXP
*
*
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */


#ifndef MESSAGING_PORT_H_
#define MESSAGING_PORT_H_
/*==================================================================================================
Include Files
==================================================================================================*/
#include "EmbeddedTypes.h"
#include "fwk_generic_list.h"

/*==================================================================================================
Public macros
==================================================================================================*/
#define MSG_Pending(anchor)             (ListGetHeadMsg(anchor) != NULL)
#define MSG_DeQueue                     ListRemoveHeadMsg
#define MSG_QueueHead                   ListAddHeadMsg
#define MSG_Queue                       ListAddTailMsg
#define MSG_FreeQueue(anchor)           while(MSG_Pending(anchor)) { (void)MSG_Free(MSG_DeQueue(anchor)); }
#define MSG_InitQueue(anchor)           ListInitMsg(anchor)
#define msgQueue_t                      list_t
#define anchor_t                        list_t

/*==================================================================================================
Public prototypes
==================================================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

#ifdef gUnitTesting_d
void* MSG_Alloc(uint32_t length);
void MSG_Free(void *buffer);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* MESSAGING_PORT_H_ */