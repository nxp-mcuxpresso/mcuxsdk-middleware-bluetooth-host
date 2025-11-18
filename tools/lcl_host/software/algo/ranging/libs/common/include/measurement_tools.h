/***********************************************************************************/
/*!
 *  @brief      
 *  @file       measurement_tools.h
 *  @author     Bram Hilkens
 *
 *  @note       
 */
/***********************************************************************************/
#ifndef _MEASUREMENT_TOOLS_H_
#define _MEASUREMENT_TOOLS_H_


#include "fsl_device_registers.h"
#include "board.h"
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#if MEMORY_RECORD_CSTACK
/* show basic info concerning stack */
void show_cstack_info(void);
/* mark the stack memory */
void mark_cstack_memory(void);
/* measure the amount of stack currently used and return the value */
void measure_cstack_size(void);
/* display max stack usage */
void display_cstack_max(void);
#endif


#if __ARM_ARCH && USE_STOPWATCH
/* enable CPU cycle counting using DWT register */
void enableCpuCycleCounter(void);
/* get current cycle count from  DWT register*/
uint32_t getCpuCycleCount(void);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif