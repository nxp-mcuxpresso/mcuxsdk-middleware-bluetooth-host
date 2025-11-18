/***********************************************************************************/
/*!
 *  @brief      
 *  @file       h2cov.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#include <h2cov/include/h2cov.h>

#include <common/include/Functions.h>
#include <common/include/Stopwatch.h>
#include <common/include/Stream.h>
#include "Logging.h"

#if USE_MEMTRACE
#include <common/include/memtrace.h>
#endif

#define H2COV_SHOW_INPUTS			0		/* 1 = on, 0 = off */
#define H2COV_SHOW_OUTPUTS			0		/* 1 = on, 0 = off */

/*
    NofMult=0;
    N=size(H_est,2);
    COV=zeros(L,L,class(H_est));
    for diagOffset=0:L-1
        for cnt=1:L-diagOffset
            if cnt==1
                for cnt2=1:N-(L)+1
                    COV(cnt,cnt+diagOffset)=COV(cnt,cnt+diagOffset)+conj(H_est(cnt2))*H_est(cnt2+diagOffset);
                    NofMult=NofMult+1;
                end
            else
                COV(cnt,cnt+diagOffset)=COV(cnt-1,cnt-1+diagOffset)-conj(H_est(cnt-1))*H_est(cnt-1+diagOffset)+conj(H_est(cnt+N-(L)))*H_est(cnt+N-(L)+diagOffset);
                NofMult=NofMult+2;
            end
            if diagOffset~=0
                COV(cnt+diagOffset,cnt)=conj(COV(cnt,cnt+diagOffset));
            end
        end
    end
*/

namespace ranging {

MatrixCmpl h2cov(const MatrixCmpl & H_est, const msize_t L) {
	TRACE();

#if  __ARM_ARCH
	STOPWATCH(__FUNCTION__,STOPWATCH_TRACE_H2COV);
#else
	STOPWATCH(__FUNCTION__);
#endif

#if RANGE_CHECK
	if (H_est.cols() < L)
		throw std::range_error("h2cov: L too large for H_est size");
#endif

#if H2COV_SHOW_INPUTS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  in H_est =   " << H_est;
	std::cout << __FUNCTION__ << "  in L=   " << L << std::endl;
	std::cout << __FUNCTION__ << "  ---------------" << std::endl;
#endif

	msize_t N = H_est.cols();

	MatrixCmpl COV(L, L, MATRIX_CLEAR_DATA);

	for (msize_t diagOffset = 0; diagOffset < L; diagOffset++) {

		for (msize_t cnt = 1; cnt <= (L - diagOffset); cnt++) {

			if (cnt == 1) {
				for (msize_t cnt2 = 1; cnt2 <= (N - L + 1); cnt2++) {
					COV(cnt - 1, cnt + diagOffset - 1) = COV(cnt - 1, cnt + diagOffset - 1)
							+ conj(H_est(cnt2 - 1)) * H_est(cnt2 + diagOffset - 1);
				}
			} else {
				COV(cnt - 1, cnt + diagOffset - 1) = COV(cnt - 2, cnt + diagOffset - 2)
						- conj(H_est(cnt - 2)) * H_est(cnt + diagOffset - 2)
						+ conj(H_est(cnt + N - L - 1)) * H_est(cnt + N - L + diagOffset - 1);
			}

			if (diagOffset != 0)
				COV(cnt + diagOffset - 1, cnt - 1) = conj(COV(cnt - 1, cnt + diagOffset - 1));

		} /* end for cnt */

	} /* end for diagOffset */

#if USE_MEMTRACE
        MEMTRACE_MARKER("H2cov");
#endif

#if H2COV_SHOW_OUTPUTS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  out COV=   " << COV;
	std::cout << __FUNCTION__ << "  ---------------" << std::endl;
#endif


	return COV;
}

void h2cov_inplace(const MatrixCmpl & H_est, MatrixCmpl & COV) {
	TRACE();

#if  __ARM_ARCH
	STOPWATCH(__FUNCTION__,STOPWATCH_TRACE_H2COV_INPLACE);
#else
	STOPWATCH(__FUNCTION__);
#endif

    msize_t L = COV.rows();

#if RANGE_CHECK
	if (H_est.cols() < L)
		throw std::range_error("h2cov: COV matrix size (L) too large for H_est size");
	if (H_est.rows() != 1)
		throw std::range_error("h2cov: H_est has more than 1 row");
	if (COV.cols() != COV.rows())
		throw std::range_error("h2cov: COV is not a square matrix");
#endif

#if H2COV_SHOW_INPUTS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  in H_est =   " << H_est;
	std::cout << __FUNCTION__ << "  in L=   " << L << std::endl;
	std::cout << __FUNCTION__ << "  ---------------" << std::endl;
#endif

	msize_t N = H_est.cols();

	for (msize_t diagOffset = 0; diagOffset < L; diagOffset++) {
		ComplexF cur(0.0), prev(0.0);
		for (msize_t cnt = 1; cnt <= (L - diagOffset); cnt++) {
			if (cnt == 1) {
				for (msize_t cnt2 = 1; cnt2 <= (N - L + 1); cnt2++) {
					cur = cur
							+ conj(H_est(cnt2 - 1)) * H_est(cnt2 + diagOffset - 1);
				}
			} else {
				cur = prev
						- conj(H_est(cnt - 2)) * H_est(cnt + diagOffset - 2)
						+ conj(H_est(cnt + N - L - 1)) * H_est(cnt + N - L + diagOffset - 1);
			}

			COV(cnt - 1, cnt + diagOffset - 1) += cur;
			if (diagOffset != 0)
				COV(cnt + diagOffset - 1, cnt - 1) += conj(cur);
			prev = cur;

		} /* end for cnt */

	} /* end for diagOffset */

#if USE_MEMTRACE
        MEMTRACE_MARKER("H2cov_inplace");
#endif

#if H2COV_SHOW_OUTPUTS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  out COV=   " << COV;
	std::cout << __FUNCTION__ << "  ---------------" << std::endl;
#endif

}


} /* namespace ranging */
