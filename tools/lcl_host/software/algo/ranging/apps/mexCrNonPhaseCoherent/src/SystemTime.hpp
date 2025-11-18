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

class SystemTime {
public:
	/**
	 * Return the system time in useconds
	 * @return
	 */
	static inline uint32_t get() {

		return 0; /* default no time */
	}

};

#endif /* SYSTEMTIME_HPP_ */
