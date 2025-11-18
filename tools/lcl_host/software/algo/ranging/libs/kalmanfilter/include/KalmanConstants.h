/***********************************************************************************/
/*!
 *  @brief      
 *  @file       KalmanConstants.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef KALMANCONSTANTS_H_
#define KALMANCONSTANTS_H_

#include <common/include/Constants.h>

namespace ranging {
namespace filter {

class KalmanConstants {
public:

	// hardware parameters
	static constexpr float LO_fc_A = 2.4e9f;            // [Hz] passband centre-frequency of Local Oscillator of Initiator
	static constexpr float LO_fc_B = 2.4e9f;            // [Hz] passband centre-frequency of Local Oscillator of Reflector
	static constexpr float Delta_F = CHANNEL_DISTANCE;            // [Hz] channel frequency step
	static constexpr float T_f = 500.0e-6f;             // [sec] Duration of a single tone measurement in two directions.

	static constexpr float alpha = (LO_fc_A + LO_fc_B) / 2.0f * T_f / Delta_F;
};


}  // namespace filter
}  // namespace ranging

#endif /* KALMANCONSTANTS_H_ */
