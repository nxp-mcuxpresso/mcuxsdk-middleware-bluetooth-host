/***********************************************************************************/
/*!
 *  @brief
 *  @file       evd.cpp
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
#include <evd/include/evd.h>

#if USE_MEMTRACE
#include <common/include/memtrace.h>
#endif

#include "Logging.h"

#include <stdio.h>
#include <algorithm>

namespace ranging {

#define EVD_SHOW_INPUTS			0		/* 1 = on, 0 = off */
#define EVD_SHOW_OUTPUTS		0		/* 1 = on, 0 = off */
#define EVD_SHOW_DETAILS		0		/* 1 = on, 0 = off */

static const msize_t constMaxNumberOfIterations = 100;

static void maxeig(MatrixCmpl &Cov, const float tole, const msize_t maxIt, float & lambda, MatrixCmpl &x) {
	TRACE();

#if  __ARM_ARCH
	STOPWATCH(__FUNCTION__,STOPWATCH_TRACE_MAXEIG);
#else
	STOPWATCH(__FUNCTION__);
#endif

#if EVD_SHOW_DETAILS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  in Cov  = " << Cov << std::endl;
	std::cout << __FUNCTION__ << "  in tole = " << tole << std::endl;
	std::cout << __FUNCTION__ << "  ---------------" << std::endl;
#endif

	msize_t it = maxIt;

	x.zero();
	x(0) = 1.0f;

	lambda = 1.0f;
	float lambdaold = 0.0f;
	MatrixCmpl Cx(Cov * x);

	while ( (std::abs(lambda - lambdaold) > tole) && it) {
		lambdaold = lambda;

		x = Cx; // split x = Cx / norm(Cx) to prevent additional malloc
		x /= norm(Cx);
		Cx = Cov * x;

		MatrixCmpl lx(x.ctranspose() * Cx);
		lambda = real(lx(0));
		it--;
	}

#if EVD_SHOW_DETAILS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  out x =   " << x << std::endl;
	std::cout << __FUNCTION__ << "  out lambda =   " << lambda << std::endl;
	std::cout << __FUNCTION__ << "  ---------------" << std::endl;
	printf("-- maxeig: num iterations = %d\n", constMaxNumberOfIterations - maxIt);
#endif
}

/* A = A - d(cnt)*V(:,cnt)*V(:,cnt)';
 * Both A  and V(:,cnt)*V(:,cnt)' are hermitians, V(:,cnt) is a vector!
 */
static void dorun_update(MatrixCmpl &A, MatrixCmpl &x, const float lambda) {

#if  __ARM_ARCH
	STOPWATCH(__FUNCTION__,STOPWATCH_TRACE_DORUN_UPDATE);
#else
	STOPWATCH(__FUNCTION__);
#endif

#if RANGE_CHECK
	if (A.rows() != x.rows() || A.cols() != A.rows() || x.cols() != 1)
		throw std::range_error("evd dorun_update error");
#endif
	for (msize_t i = 0; i < A.rows(); i++) {
		for (msize_t j = 0; j < A.cols(); j++) {

			ComplexF xf = conjugate(x(j));
			xf *= x(i);

			ComplexF a = A(i, j);
			a = a - (lambda * xf);
			A(i, j) = a;
		}
	}
}

static void evd_dorun(MatrixCmpl &A, const struct evd_tol_s & tol, const msize_t K, MatrixCmpl &V, MatrixF &d) {
	TRACE();

	float sumeig = trace(A);
	msize_t cnt = 0;
	float Tol2_tmp = tol.sumeig * sumeig;
	float Cap = 0.0f;

	while ((cnt < K) && (Cap < Tol2_tmp)) {
		/*		[d(cnt),V(:,cnt)]=maxeig(self,A,TOL(1));
		 */
		float lambda;
		MatrixCmpl x(A.rows(), 1, MATRIX_UNINIT_DATA);
		maxeig(A, tol.maxeig, constMaxNumberOfIterations, lambda, x);

		d(cnt) = lambda;
		V.update(Span(0, V.rows() - 1), Span(cnt, cnt), x);

		/*
		 A=A - d(cnt)*V(:,cnt)*V(:,cnt)'; %Both A  and V(:,cnt)*V(:,cnt)' are hermitians, V(:,cnt) is a vector!
		 */
		dorun_update(A, x, lambda);

		/*
		 Cap=Cap+d(cnt);
		*/
		Cap += d(cnt);

		DPRINTF("cnt : %d\n", cnt);

		cnt++;
	}

	/* update number of cols() for d (= EVs) */
	d.cols(cnt);
	DPRINTF("EVs resized to : %d columns\n", cnt);

}

void evd(MatrixCmpl &Cov, const struct evd_tol_s & tol, const msize_t K, MatrixCmpl &V, MatrixF &d) {
	TRACE();

#if  __ARM_ARCH
	STOPWATCH(__FUNCTION__,STOPWATCH_TRACE_EVD);
#else
	STOPWATCH(__FUNCTION__);
#endif

#if EVD_SHOW_INPUTS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  in Cov =   " << Cov << std::endl;
	std::cout << __FUNCTION__ << "  in tol.maxeig = " << tol.maxeig << std::endl;
	std::cout << __FUNCTION__ << "  in tol.sumeig = " << tol.sumeig << std::endl;
	std::cout << __FUNCTION__ << "  in K = " << K << std::endl;
	std::cout << __FUNCTION__ << "  in V = " << V << std::endl;
	std::cout << __FUNCTION__ << "  in d = " << d << std::endl;
	std::cout << __FUNCTION__ << "  ---------------" << std::endl;
#endif

	evd_dorun(Cov, tol, K, V, d);

#if USE_MEMTRACE
        MEMTRACE_MARKER("Evd");
#endif

#if EVD_SHOW_OUTPUTS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  out V = " << V << std::endl;
	std::cout << __FUNCTION__ << "  out d = " << d << std::endl;
	std::cout << __FUNCTION__ << "  ---------------" << std::endl;
#endif
}

}  // namespace ranging

// end

