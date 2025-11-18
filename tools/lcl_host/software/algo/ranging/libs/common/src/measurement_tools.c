/***********************************************************************************/
/*!
 *  @brief      
 *  @file       measurement_tools.c
 *  @author     Bram Hilkens
 *
 *  @note       
 */
/***********************************************************************************/
#include "common/include/measurement_tools.h"

#if __ARM_ARCH == 8 && USE_STOPWATCH
/* redefinition of static function for cycle counting using DWT */
void enableCpuCycleCounter(void)
{
    /* Make sure the DWT trace fucntion is enabled. */
    if (CoreDebug_DEMCR_TRCENA_Msk != (CoreDebug_DEMCR_TRCENA_Msk & CoreDebug->DEMCR))
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }

    /* CYCCNT not supported on this device. */
    assert(DWT_CTRL_NOCYCCNT_Msk != (DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk));

    /* Read CYCCNT directly if CYCCENT has already been enabled, otherwise enable CYCCENT first. */
    if (DWT_CTRL_CYCCNTENA_Msk != (DWT_CTRL_CYCCNTENA_Msk & DWT->CTRL))
    {
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}
/* get current cycle count from appropriate register*/
uint32_t getCpuCycleCount(void)
{
    return DWT->CYCCNT;
}
#endif

#if __ARM_ARCH && MEMORY_RECORD_CSTACK
/* allows to use stack pointers corresponding to the begining and the end of the stack defined during linking by IAR compiler */
extern char CSTACK$$Base[];
extern char CSTACK$$Limit[];
/* char used to mark the stack */
const char marker= 0xAA;
/* limit size of the stack to mark */
const char * cstack_limit = CSTACK$$Limit-0x60;
static unsigned int cstack_max_size = 0;
/* show basic info concerning stack */
void show_cstack_info(void) {
        const unsigned int cstack_size = (unsigned int)(CSTACK$$Limit-CSTACK$$Base);
        printf("cstack base adress : 0x%p\n",CSTACK$$Base);
        printf("cstack end adress : 0x%p\n",CSTACK$$Limit);
        printf("cstack size = 0x%x\n\n",cstack_size);
}

/* mark the stack memory */
void mark_cstack_memory(void) {
        const unsigned int cstack_size_w_offset = (unsigned int)(cstack_limit-CSTACK$$Base);
        memset(CSTACK$$Base,marker,cstack_size_w_offset);
}

/* measure the amount of stack currently used and return the value */
void measure_cstack_size(void) {
        char* real_cstack_limit = CSTACK$$Base;
        for(char* i=CSTACK$$Base; i<(cstack_limit); i+=sizeof(char)){
          if(*i != marker){
            real_cstack_limit = i;
            break;
          }
        }
        const unsigned int real_cstack_size =(unsigned int)(CSTACK$$Limit - real_cstack_limit);
        if(CSTACK$$Base == real_cstack_limit){
          printf("warning all stack used or error in stack measurment");
        }else{
          printf("real cstack size in hexadecimal = 0x%x\n",real_cstack_size);
          printf("real cstack size in decimal = %u\n",real_cstack_size);
          printf("real cstack address : 0x%p\n\n",real_cstack_limit);
        }
        cstack_max_size = MAX(cstack_max_size,real_cstack_size);
}
void display_cstack_max(void){
        printf("Max stack size used : %u\n",cstack_max_size);
}
#endif