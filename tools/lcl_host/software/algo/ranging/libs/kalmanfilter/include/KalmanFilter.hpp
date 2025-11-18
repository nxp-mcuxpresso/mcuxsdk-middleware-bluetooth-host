/***********************************************************************************/
/*!
 *  @brief      
 *  @file       KalmanFilter.hpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef KALMANFILTER_HPP_
#define KALMANFILTER_HPP_

#include "kalmanfilter/include/IFilter.hpp"
#include <common/include/Matrix.h>

namespace ranging {
namespace filter {

class KalmanFilter : public IFilter {
public:

	KalmanFilter();
	virtual ~KalmanFilter();

	/* implement interface */
	void update(const Input &in, Output &out);

	void reset();

private:
	void init(float d);
	void runCore(const Input &in, Output &out);

private:
	float var_a;
	float var_d;

	float xhat[2];
	float P[2][2];
	bool m_initialized;
};

}  // namespace filter
}  // namespace ranging

#endif /* KALMANFILTER_HPP_ */
