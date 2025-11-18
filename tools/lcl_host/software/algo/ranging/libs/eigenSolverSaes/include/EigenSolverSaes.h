/***********************************************************************************/
/*!
 *  @brief      
 *  @file       EigenSolverSaes.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef EIGEN_SRC_EIGENSOLVER_SAES_H_
#define EIGEN_SRC_EIGENSOLVER_SAES_H_

#include <common/include/Matrix.h>
#include <common/include/Types.h>

namespace ranging {

class EigenSolverSaes {
public:
	EigenSolverSaes();
	virtual ~EigenSolverSaes();

	EigenSolverSaes(const EigenSolverSaes &other) = delete;
	EigenSolverSaes(EigenSolverSaes &&other) = delete;
	EigenSolverSaes& operator=(const EigenSolverSaes &other) = delete;
	EigenSolverSaes& operator=(EigenSolverSaes &&other) = delete;

	/**
	 *
	 * @param Cov
	 * @param tol
	 * @param K
	 * @param V	EigenVecors
	 * @param d	EigenValues
	 */
	void selfAdjointEigenSolver(MatrixCmpl &Cov, const struct evd_tol_s & tol, const msize_t K, MatrixCmpl &V, MatrixF &d);
};


}  // namespace ranging


#endif /* EIGEN_SRC_EIGENSOLVER_SAES_H_ */
