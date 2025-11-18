/***********************************************************************************/
/*!
 *  @brief      
 *  @file       SystemTime.hpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef SYSTEMTIME_HPP_
#define SYSTEMTIME_HPP_

#include <stdint.h>

#if __unix__
#include <sys/time.h>
#endif

#ifdef MUSEICV31
#include "ap_timer.h"
#endif

#if WIN32
#include <chrono>
#include <ctime>
#endif 
//	{ /* start timer for time measurements */
//	AP_TIMER_Type * tmr = AP_TIMER0;
//	tmr->RELOAD=0xffffffff;
//	tmr->VALUE=0xffffffff;
//	tmr->CTRL_b.EN = 1;
//	TIMER_IntEnable(tmr); /* need to enable timer interrupt to have it run in QEMU */
//	NVIC_DisableIRQ(TIMER0_IRQn); /* explicit no interrupt ot cpu */
//	}
#if __ARM_ARCH   
#include "fsl_clock.h"
#include "fsl_common_arm.h"
#include "common/include/measurement_tools.h"
#endif

class SystemTime {
public:
	/**
	 * Return the system time in useconds
	 * @return
	 */
#if __ARM_ARCH
        static inline uint32_t get() {
#else
        static inline unsigned int get() {
#endif
#if __unix__
		struct timeval tp;
		gettimeofday(&tp, NULL);
		int us = tp.tv_sec * 1000000 + tp.tv_usec; // get current timestamp
		return us;
#elif MUSEICV31
		AP_TIMER_Type * tmr = AP_TIMER0;
		return 0xffffffff - tmr->VALUE; /* get value from downcounter */
#elif WIN32

		 return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
      
#elif __ARM_ARCH == 8
                /* get the current time in microseconds using the DWT register from ARM CortexM33 */

               return COUNT_TO_USEC(getCpuCycleCount(),CLOCK_GetCoreSysClkFreq());

#else
               return 0; /* default no time */
#endif           
        }

};

#endif /* SYSTEMTIME_HPP_ */
