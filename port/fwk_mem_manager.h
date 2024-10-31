/*! *********************************************************************************
* Copyright 2020-2021, 2023 NXP
*
*
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */


#ifndef MEM_MANAGER_PORT_H_
#define MEM_MANAGER_PORT_H_
/*==================================================================================================
Include Files
==================================================================================================*/
#include "EmbeddedTypes.h"
#include "fsl_component_mem_manager.h"

/*==================================================================================================
Public macros
==================================================================================================*/
#define MEM_BufferAlloc(numBytes) MEM_BufferAllocWithId(numBytes, 0)

/*==================================================================================================
Public prototypes
==================================================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

#ifdef gUnitTesting_d
/*!
 * @brief Allocate a block from the memory pools. The function uses the
 *        numBytes argument to look up a pool with adequate block sizes.
 *
 * @param numBytes           The number of bytes will be allocated.
 * @param poolId             The ID of the pool where to search for a free buffer.
 * @retval Memory buffer address when allocate success, NULL when allocate fail.
 */
extern void *MEM_BufferAllocWithId(uint32_t numBytes, uint8_t poolId);

/*!
 * @brief Memory buffer free .
 *
 * @param buffer                     The memory buffer address will be free.
 * @retval kStatus_MemSuccess        Memory free succeed.
 * @retval kStatus_MemFreeError      Memory free error occurred.
 */
extern mem_status_t MEM_BufferFree(void *buffer);

/*!
 * @brief Returns the size of a given buffer.
 *
 * @param buffer  The memory buffer address will be get size.
 * @retval The size of a given buffer.
 */
extern uint16_t MEM_BufferGetSize(void *buffer);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* MEM_MANAGER_PORT_H_ */