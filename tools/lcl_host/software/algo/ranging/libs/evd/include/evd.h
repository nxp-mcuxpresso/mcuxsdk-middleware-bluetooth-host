/***********************************************************************************/
/*!
 *  @brief      
 *  @file       evd.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef EVD_H_
#define EVD_H_

#include <common/include/Matrix.h>
#include <common/include/Types.h>

namespace ranging {

/**
 *
 * @param Cov
 * @param tol
 * @param K
 * @param V	EigenVecors
 * @param d	EigenValues
 */
void evd(MatrixCmpl &Cov, const struct evd_tol_s & tol, const msize_t K, MatrixCmpl &V, MatrixF &d);

}  // namespace ranging


#endif /* EVD_H_ */
