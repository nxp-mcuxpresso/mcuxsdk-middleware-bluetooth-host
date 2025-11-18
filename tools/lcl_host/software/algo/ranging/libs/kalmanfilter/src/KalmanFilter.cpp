/***********************************************************************************/
/*!
 *  @brief      
 *  @file       kalman.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#include "kalmanfilter/include/KalmanFilter.hpp"
#include "kalmanfilter/include/KalmanConstants.h"


namespace ranging {
namespace filter {

#define DEBUG_SHOW_DETAILS		0		/* 1 = on, 0 = off */

KalmanFilter::KalmanFilter() :
		var_a(1.0f), var_d(1.0f), m_initialized(false) {
	TRACE();

	/* empty */
}

KalmanFilter::~KalmanFilter() {
	TRACE();
	/* empty */
}

void KalmanFilter::reset() {
	TRACE();
	m_initialized = false;
}

void KalmanFilter::init(float d) {
	TRACE();

	var_a = 1.0f;
	var_d = 1.0f;

	//%% KF initialization
	P[0][0] = 1.0f;
	P[0][1] = 0.0f;
	P[1][0] = 0.0f;
	P[1][1] = 1.0f;

	/* set initial value */
	xhat[1] = 0.0f;
	xhat[0] = d;

	m_initialized = true;
}

void KalmanFilter::update(const Input &in, Output &out) {
	TRACE();

	/* take special care when this is the first data iteration */
	if (m_initialized == false) {
		init(in.dist);
		/* set initial output data */
		out.dist = xhat[0];
		out.speed = xhat[1];
		return;
	}

	runCore(in, out);
}

void KalmanFilter::runCore(const Input &in, Output &out) {
	TRACE();

	float d_ss = in.dist;
	float Ts = in.Ts;

	//  % prediction
	//  xhat_plus[1] = xhat[1]+Ts*xhat(2);
	//  xhat_plus(2) = xhat(2);
	//  Pplus(1,1) = P(1,1)+Ts*P(1,2)+Ts*P(2,1)+(Ts^2)*P(2,2)+Ts^4*var_a/4;
	//  Pplus(1,2) = P(1,2)+Ts*P(2,2)+Ts^3*var_a/2;
	//  Pplus(2,1) = P(2,1)+Ts*P(2,2)+Ts^3*var_a/2;
	//  Pplus(2,2) = P(2,2)+Ts^2*var_a;
	float xhat_plus[2];
	xhat_plus[0] = xhat[0] + Ts * xhat[1];
	xhat_plus[1] = xhat[1];

	float Pplus[2][2];
	Pplus[0][0] = P[0][0] + Ts * P[0][1] + Ts * P[1][0] + Ts * Ts * P[1][1] + Ts * Ts * Ts * Ts * var_a / 4.0f;
	Pplus[0][1] = P[0][1] + Ts * P[1][1] + Ts * Ts * Ts * var_a / 2.0f;
	Pplus[1][0] = P[1][0] + Ts * P[1][1] + Ts * Ts * Ts * var_a / 2.0f;
	Pplus[1][1] = P[1][1] + Ts * Ts * var_a;

#if DEBUG_SHOW_DETAILS
std::cout << "xhat_plus= " << xhat_plus << std::endl;
std::cout << "Pplus= " << Pplus << std::endl;
#endif

	// % Update
	// if isnan(d_ss)              % @Martijn, sometimes a measurement is completed but no range can be estimated, i.e. the output of ranging algorithm would be NaN
	if (std::isnan(d_ss)) {
		// xhat = xhat_plus;
		// P = Pplus;
		xhat[0] = xhat_plus[0];
		xhat[1] = xhat_plus[1];

		//P = Pplus;
		P[0][0] = Pplus[0][0];
		P[0][1] = Pplus[0][1];
		P[1][0] = Pplus[1][0];
		P[1][1] = Pplus[1][1];

	} else {
		// Y = xhat_plus[1]+KalmanConstants::alpha*xhat_plus(2);
		// Z = d_ss-Y;
		// R = var_d;              % constant in KF but not in AKF
		// S = R+Pplus(1,1)+KalmanConstants::alpha*Pplus(1,2)+KalmanConstants::alpha*Pplus(2,1)+KalmanConstants::alpha^2*Pplus(2,2);

		float Y = xhat_plus[0] + KalmanConstants::alpha * xhat_plus[1];
		float Z = d_ss - Y;
		float R = var_d;
		// constant in KF but not in AKF
		float S = R + Pplus[0][0] + KalmanConstants::alpha * Pplus[0][1] + KalmanConstants::alpha * Pplus[1][0] + KalmanConstants::alpha * KalmanConstants::alpha * Pplus[1][1];

		// K(1) = (Pplus(1,1)+KalmanConstants::alpha*Pplus(1,2))/S;
		// K(2) = (Pplus(2,1)+KalmanConstants::alpha*Pplus(2,2))/S;
		float K[2];
		K[0] = (Pplus[0][0] + KalmanConstants::alpha * Pplus[0][1]) / S;
		K[1] = (Pplus[1][0] + KalmanConstants::alpha * Pplus[1][1]) / S;

#if DEBUG_SHOW_DETAILS
	std::cout << "Y= " << Y << std::endl;
	std::cout << "Z= " << Z << std::endl;
	std::cout << "R= " << R << std::endl;
	std::cout << "S= " << S << std::endl;
	std::cout << "K[0]= " << K[0] << std::endl;
	std::cout << "K[1]= " << K[1] << std::endl;
#endif

		// xhat[1] = xhat_plus[1]+K(1)*Z;
		// xhat(2) = xhat_plus(2)+K(2)*Z;
		xhat[0] = xhat_plus[0] + K[0] * Z;
		xhat[1] = xhat_plus[1] + K[1] * Z;

		// P(1,1) = Pplus(1,1)-K(1)*Pplus(1,1)-KalmanConstants::alpha*K(1)*Pplus(2,1);
		// P(1,2) = Pplus(1,2)-K(1)*Pplus(1,2)-KalmanConstants::alpha*K(1)*Pplus(2,2);
		// P(2,1) = Pplus(2,1)-K(2)*Pplus(1,1)-KalmanConstants::alpha*K(2)*Pplus(2,1);
		// P(2,2) = Pplus(2,2)-K(2)*Pplus(1,2)-KalmanConstants::alpha*K(2)*Pplus(2,2);
		// P(1,2) = (P(1,2)+P(2,1))/2;
		// P(2,1) = (P(1,2)+P(2,1))/2;
		P[0][0] = Pplus[0][0] - K[0] * Pplus[0][0] - KalmanConstants::alpha * K[0] * Pplus[1][0];
		P[0][1] = Pplus[0][1] - K[0] * Pplus[0][1] - KalmanConstants::alpha * K[0] * Pplus[1][1];
		P[1][0] = Pplus[1][0] - K[1] * Pplus[0][0] - KalmanConstants::alpha * K[1] * Pplus[1][0];
		P[1][1] = Pplus[1][1] - K[1] * Pplus[0][1] - KalmanConstants::alpha * K[1] * Pplus[1][1];
		P[0][1] = (P[0][1] + P[1][0]) / 2.0f;
		P[1][0] = (P[0][1] + P[1][0]) / 2.0f;

	}

	/* set output data */
	// x_out(1,cnt) = xhat[1];
	// x_out(2,cnt) = xhat(2);
	out.dist = xhat[0];
	out.speed = xhat[1];
}

}  // namespace filter
}  // namespace ranging

// end

