/***********************************************************************************/
/*!
 *  @brief      
 *  @file       KalmanFilterTest.hpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef KALMANFILTERTEST_HPP_
#define KALMANFILTERTEST_HPP_


#include "kalmanfilter/include/KalmanFilter.hpp"

#include <common/include/Matrix.h>


namespace ranging {
namespace filter {

class KalmanFilterTest {
public:
	KalmanFilterTest();
	virtual ~KalmanFilterTest();

	void test1();
	void test2();

private:
	void compare_ref(MatrixF &x);
};


}  // namespace filter
}  // namespace ranging

#endif /* KALMANFILTERTEST_HPP_ */
