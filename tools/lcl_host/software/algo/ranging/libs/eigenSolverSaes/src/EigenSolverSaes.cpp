/***********************************************************************************/
/*!
 *  @brief      
 *  @file       EigenSolverSaes.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#include <common/include/Functions.h>
#include <common/include/Stopwatch.h>
#include <common/include/Stream.h>
#include <common/include/Types.h>
#include <eigenSolverSaes/include/EigenSolverSaes.h>

#if USE_MEMTRACE
#include <common/include/memtrace.h>
#endif

#include "Logging.h"

/* Suppress Eigen warnings when using IAR compiler (Those warnings are suppressed by Eigen with other compiler) */
#if defined(__ICCARM__)
#pragma diag_suppress=Pe111, Pe609, Pe550, Pa089
#endif
/* ThirdParty include */
#include <Eigen/Dense>
#if defined(__ICCARM__)
#pragma diag_default=Pe111, Pe609, Pe550, Pa089
#endif

namespace ranging {

#define EIGEN_SHOW_INPUTS			0		/* 1 = on, 0 = off */
#define EIGEN_SHOW_OUTPUTS			0		/* 1 = on, 0 = off */
#define EIGEN_SHOW_DETAILS			0		/* 1 = on, 0 = off */
#define EIGEN_UNOPTIMIZED			0		/* 1 = on, 0 = off */

EigenSolverSaes::EigenSolverSaes() {
	// TODO Auto-generated constructor stub

}

EigenSolverSaes::~EigenSolverSaes() {
	// TODO Auto-generated destructor stub
}

void EigenSolverSaes::selfAdjointEigenSolver(MatrixCmpl &Cov, const struct evd_tol_s &tol, const msize_t K, MatrixCmpl &V, MatrixF &d) {
	TRACE();

#if  __ARM_ARCH
	STOPWATCH(__FUNCTION__,STOPWATCH_TRACE_EVD_SAES);
#else
	STOPWATCH(__FUNCTION__);
#endif

#if RANGE_CHECK
	if (Cov.rows() != Cov.cols() || V.cols() < K || V.rows() != Cov.rows() || d.cols() < K || d.rows() != 1)
		throw std::range_error("eigen-saes input error");
#endif
	Eigen::Map<Eigen::MatrixXcf, Eigen::RowMajor> eCov((ComplexF*) (Cov.data()), Cov.rows(), Cov.cols());

	/* -----------------------------------------------------------------
	 * Run the EVD in Eigen library :: SelfAdjointEigenSolverSaes
	 * ----------------------------------------------------------------- */
	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXcf> saes;

	saes.compute(eCov);

	/* The eigenvalue are returned in reverse order and ComplexFloat matrix */
	const Eigen::VectorXf &ed = saes.eigenvalues();
#if RANGE_CHECK
	if (ed.cols() != 1 || ed.rows() < K)
		throw std::range_error("eigen-saes eigenvalues error");
#endif

	float sumeig = trace(Cov);
	msize_t cnt = 0;
	float Tol2_tmp = tol.sumeig * sumeig;
	float Cap = 0.0f;


	for(; (cnt < K) && (Cap < Tol2_tmp); cnt++)
		Cap += ed(ed.rows() - cnt - 1, 0);

	for(int i = 0; i < std::min(d.cols(), (int)ed.rows()); i++)
		d(0, i) = ed(ed.rows() - i - 1, 0);

	d.cols(cnt);
	DPRINTF("EVs resized to : %d columns\n", cnt);

	/* The eigenvectors are returned transposed and in reverse row order */
	const Eigen::MatrixXcf &eV = saes.eigenvectors();
#if RANGE_CHECK
	if (eV.rows() != eV.cols() || V.rows() != Cov.rows())
		throw std::range_error("eigen-saes eigenvectors error");
#endif
	for(int i = 0; i < eV.cols(); i++)
		for(int j = 0; j < std::min(d.cols(), std::min(V.cols(), (int)eV.rows())); j++)
			V(i, j) = std::conj(eV(i, eV.cols() - j - 1));

#if USE_MEMTRACE
                        MEMTRACE_MARKER("selfAdjointEigenSolver");
#endif
}

}  // namespace ranging

// end

