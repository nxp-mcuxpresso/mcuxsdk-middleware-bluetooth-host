
#ifndef CHANNELRECONSTRUCT2D_H_
#define CHANNELRECONSTRUCT2D_H_

#include <common/include/Matrix.h>

namespace ranging {

/**
 * function [H_est, K_est, Uncertainty]=ChannelReconstruct_2D(IQ_A,IQ_B)
 *
 */
//original implementation where IQ had saturated path removed beforehand
void ChannelReconstruct_2D(const MatrixCmpl & IQ_A, const MatrixCmpl & IQ_B, MatrixCmpl & H_est, MatrixF & K_est, Matrix<int> & Uncert);
/**
 * function [H_est, K_est, Uncertainty]=ChannelReconstruct_2D(IQ_A,IQ_B,saturated_array)
 *
 */
void ChannelReconstruct_2D(const MatrixCmpl & IQ_A, const MatrixCmpl & IQ_B, MatrixCmpl & H_est, MatrixF & K_est, Matrix<int> & Uncert, const int *saturated_array);
} /* namespace ranging */

#endif /* CHANNELRECONSTRUCT2D_H_ */
