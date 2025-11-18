/***********************************************************************************/
/*!
 *  @brief      
 *  @file       IFilter.hpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef IFILTER_HPP_
#define IFILTER_HPP_

#include <common/include/Matrix.h>

namespace ranging {
namespace filter {

class IFilter {
public:

	struct Input {
		float dist;
		float Ts;
	};

	struct Output {
		float dist;
		float speed;
		bool outlier;
	};

	IFilter() = default;
	virtual ~IFilter() = default;

	/* Interface */
	virtual void update(const Input &in, Output &out) = 0;

	virtual void reset() = 0;

};

}  // namespace filter
}  // namespace ranging

#endif /* IFILTER_HPP_ */
