/***********************************************************************************/
/*!
 *  @brief
 *  @file       RangingSettings.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note
 */
/***********************************************************************************/

#ifndef RANGINGSETTINGS_H_
#define RANGINGSETTINGS_H_

#include <common/include/Types.h>

namespace ranging {

class RangingSettings {
public:
	RangingSettings();

public:
	void defaults();

public:
	float PS_approx;
	float InitPos;
	float MinLevel;
	float TOL1; /* also named maxeig */
	float TOL2; /* also named sumeig */
	float NFFT;

	int maxiter;

        msize_t  L;

        msize_t MethodSubspaceSep;
};

} /* namespace ranging */

#endif /* RANGINGSETTINGS_H_ */
