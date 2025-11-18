/***********************************************************************************/
/*!
 *  @brief      
 *  @file       AdaptiveKalmanFilterTest.hpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef ADAPTIVEKALMANFILTERTEST_HPP_
#define ADAPTIVEKALMANFILTERTEST_HPP_


#include "kalmanfilter/include/AdaptiveKalmanFilter.hpp"

#include <common/include/Matrix.h>


namespace ranging {
namespace filter {

class AdaptiveKalmanFilterTest {
public:
	AdaptiveKalmanFilterTest();
	virtual ~AdaptiveKalmanFilterTest();

	void test1();
	void test2();

private:
	void compare_ref_test1(MatrixF &x);
};


}  // namespace filter
}  // namespace ranging

#endif /* ADAPTIVEKALMANFILTERTEST_HPP_ */
