/***********************************************************************************/
/*!
 *  @brief      
 *  @file       main.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <fenv.h>
#include <unity.h>

#include "appversion.h"

#include <AdaptiveKalmanFilterTest.hpp>
#include <KalmanFilterTest.hpp>


#define ENABLE_FLOATING_POINT_EXCEPTIONS		0

using namespace ranging;

void filtertest_kalman1() {
	filter::KalmanFilterTest filterTest;
	filterTest.test1();
}

void filtertest_kalman2() {
	filter::KalmanFilterTest filterTest;
	filterTest.test2();
}

void filtertest_adaptive_kalman1() {
	filter::AdaptiveKalmanFilterTest filterTest;
	filterTest.test1();
}

void filtertest_adaptive_kalman2() {
	filter::AdaptiveKalmanFilterTest filterTest;
	filterTest.test2();
}

int filtertest_runner() {
	UNITY_BEGIN();

	RUN_TEST(filtertest_kalman1);
	RUN_TEST(filtertest_kalman2);
	RUN_TEST(filtertest_adaptive_kalman1);
	RUN_TEST(filtertest_adaptive_kalman2);

	return UNITY_END();
}

int main(void) {

	int ret = 0;

	puts("Kalman Filter Test.");

	printf("GIT Version: %d   hash: %s   date: %s\n", APPVERSION, APPHASH, APPDATE);

#if ENABLE_FLOATING_POINT_EXCEPTIONS
#if defined(_MSC_VER)
	_controlfp( _EM_INVALID, _MCW_EM );
#else
	feenableexcept(fegetexcept() | FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW);
#endif
#endif

	/* configure format of float printing */
	// std::cout << std::fixed << std::setw(8) << std::setprecision(12);
	filtertest_runner();

	puts("Finished.");

	return ret;
}

// end
