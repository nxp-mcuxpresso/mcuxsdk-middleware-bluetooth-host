/***********************************************************************************/
/*!
 *  @brief      
 *  @file       RangingSettings.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#include <common/include/RangingSettings.h>

namespace ranging {

RangingSettings::RangingSettings() {

	/* load default settings */
	defaults();
}

void RangingSettings::defaults(void) {
	PS_approx = 0.005f;
	InitPos = 0.0f;
	MinLevel = 0.100f;

	TOL1 = 1.0e-7f;
	TOL2 = 0.999f;
	NFFT = 512.0f;

	maxiter = 100;

        L = 40;

	MethodSubspaceSep = 14;
}

} /* namespace ranging */

// end

