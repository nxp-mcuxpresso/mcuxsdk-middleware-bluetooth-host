/***********************************************************************************/
/*!
 *  @brief      
 *  @file       NoFilter.hpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef NOFILTER_HPP_
#define NOFILTER_HPP_

#include "kalmanfilter/include/IFilter.hpp"

namespace ranging {
namespace filter {

class NoFilter: public IFilter {
public:

	NoFilter() = default;
	virtual ~NoFilter() = default;

	/* no filter action, pass the data on as it is received */
	void update(const Input &in, Output &out) {
		out.dist = in.dist;
		out.speed = 0.0f;
	}

	void reset() {
		/* empty */
	}

};

}  // namespace filter
}  // namespace ranging

#endif /* NOFILTER_HPP_ */
