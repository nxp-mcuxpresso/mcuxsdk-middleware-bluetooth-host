/***********************************************************************************/
/*!
 *  @brief      
 *  @file       reconstructNonPhaseCoherent.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef RECONSTRUCT_NPC_H_
#define RECONSTRUCT_NPC_H_

#include <common/include/Matrix.h>

namespace ranging {

/**
 * function [H_est, K_est, Uncertainty]=reconstructNonPhaseCoherent(IQ_A,IQ_B)
 *
 */
void reconstructNonPhaseCoherent(const MatrixCmpl & IQ_A, const MatrixCmpl & IQ_B, MatrixCmpl & H_est, MatrixF & K_est, Matrix<int> & Uncert);

void reconstructNonPhaseCoherent(const MatrixCmpl & IQ_A, const MatrixCmpl & IQ_B, MatrixCmpl & H_est, MatrixF & K_est, Matrix<int> & Uncert, const int *saturated_array );

} /* namespace ranging */

#endif /* RECONSTRUCT_NPC_H_ */
