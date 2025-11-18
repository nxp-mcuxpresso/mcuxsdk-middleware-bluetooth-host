/***********************************************************************************/
/*!
 *  @brief      
 *  @file       ElapsedTime.hpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef ELAPSEDTIME_HPP_
#define ELAPSEDTIME_HPP_

#include "SystemTime.hpp"

class ElapsedTime {
public:
	ElapsedTime() :
			m_starttime(0), m_stoptime(0) {

		captureStart();
	}

	inline void captureStart() {
		m_starttime = SystemTime::get();
	}

	inline void captureStop() {
		m_stoptime = SystemTime::get();
	}

	/**
	 * Return the elapsed time in useconds
	 * @return
	 */
	inline uint32_t elapsed() {
		captureStop();
		return (m_stoptime - m_starttime);
	}

	/**
	 * update internal for new elapsed time measurement
	 */
	inline void update() {
		m_starttime = m_stoptime;
	}

private:
	uint32_t m_starttime;
	uint32_t m_stoptime;
};

#endif /* ELAPSEDTIME_HPP_ */
