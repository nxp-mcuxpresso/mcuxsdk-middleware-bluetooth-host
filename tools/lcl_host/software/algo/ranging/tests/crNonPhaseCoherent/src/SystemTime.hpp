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

//	{ /* start timer for time measurements */
//	AP_TIMER_Type * tmr = AP_TIMER0;
//	tmr->RELOAD=0xffffffff;
//	tmr->VALUE=0xffffffff;
//	tmr->CTRL_b.EN = 1;
//	TIMER_IntEnable(tmr); /* need to enable timer interrupt to have it run in QEMU */
//	NVIC_DisableIRQ(TIMER0_IRQn); /* explicit no interrupt ot cpu */
//	}



class SystemTime {
public:
	/**
	 * Return the system time in useconds
	 * @return
	 */
	static inline uint32_t get() {

#if __unix__
		struct timeval tp;
		gettimeofday(&tp, NULL);
		int us = tp.tv_sec * 1000000 + tp.tv_usec; // get current timestamp
		return us;
#endif

#ifdef MUSEICV31
		AP_TIMER_Type * tmr = AP_TIMER0;
		return 0xffffffff - tmr->VALUE; /* get value from downcounter */
#endif

		return 0; /* default no time */
	}

};

#endif /* SYSTEMTIME_HPP_ */
