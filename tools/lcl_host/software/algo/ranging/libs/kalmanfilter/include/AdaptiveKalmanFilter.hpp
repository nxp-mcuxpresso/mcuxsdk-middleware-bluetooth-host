/***********************************************************************************/
/*!
 *  @brief      
 *  @file       AdaptiveKalmanFilter.hpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef ADAPTIVEKALMANFILTER_HPP_
#define ADAPTIVEKALMANFILTER_HPP_

#include "kalmanfilter/include/IFilter.hpp"
#include <common/include/Matrix.h>

namespace ranging {
namespace filter {

class AdaptiveKalmanFilter: public IFilter {
public:

	AdaptiveKalmanFilter();
	virtual ~AdaptiveKalmanFilter();

	/* implement interface */
	void update(const Input &in, Output &out);

	void reset();

private:
	void init(float distance);
	void runCore(const Input &in, Output &out);

private:
	enum { VELOCITY_HISTORY_NUM = 5 };
	const int n_a = VELOCITY_HISTORY_NUM;

private:
	float var_a;
	float var_a_ref;
	float var_d;

	float xhat[2];
	float P[2][2];
	bool m_initialized;

	int cnt;

	float a_store[VELOCITY_HISTORY_NUM];

	float m_previousSpeed;
};

}  // namespace filter
}  // namespace ranging

#endif /* ADAPTIVEKALMANFILTER_HPP_ */
