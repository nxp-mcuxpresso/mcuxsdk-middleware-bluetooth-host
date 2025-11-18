/***********************************************************************************/
/*!
 *  @brief      
 *  @file       h2cov.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef H2COV_H_
#define H2COV_H_

#include <common/include/Matrix.h>

namespace ranging {

/**
 * Transform a complex array of measurements into a covariance matrix
 * matlab:        Hankel=H_2_Hankel(H_est,L-1);
 *                COV_ref=Hankel'*Hankel;
 *
 * @param H_est array of iq estimations
 * @param L new length
 * @return COV matrix of size LxL
 */
MatrixCmpl h2cov(const MatrixCmpl & H_est, const msize_t L);

/**
 * Transform a complex array of measurements into a covariance matrix
 * matlab:        Hankel=H_2_Hankel(H_est,L-1);
 *                COV_ref=Hankel'*Hankel;
 *
 * @param H_est array of iq estimations
 * @param COV matrix of size LxL
 */
void h2cov_inplace(const MatrixCmpl & H_est, MatrixCmpl & COV);

} /* namespace ranging */

#endif /* H2COV_H_ */
