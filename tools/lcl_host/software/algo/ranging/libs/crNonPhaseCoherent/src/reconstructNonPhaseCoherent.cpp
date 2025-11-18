/***********************************************************************************/
/*!
 *  @brief      
 *  @file       reconstructNonPhaseCoherent.cpp
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
#include <common/include/VectorTypes.h>
#include <common/include/Constants.h>
#include <crNonPhaseCoherent/include/reconstructNonPhaseCoherent.h>

#if USE_MEMTRACE
#include <common/include/memtrace.h>
#endif

#include <assert.h>

#define RECNPC_SHOW_INPUTS				0		/* 1 = on, 0 = off */
#define RECNPC_SHOW_OUTPUTS				0		/* 1 = on, 0 = off */

namespace ranging {

void sign_correct(MatrixCmpl &H_est, msize_t rowcount) {
	TRACE();

	msize_t scols = H_est.cols() - 1;

	bool_vector I(scols);
	assert((size_t )scols <= I.size());

	for (msize_t col = 0; col < scols; col++) {
		I[col] = std::abs(H_est(rowcount, col + 1) - H_est(rowcount, col)) > std::abs(H_est(rowcount, col + 1) + H_est(rowcount, col));
	}

	for (msize_t col = 0; col < scols; col++) {
		if (I[col] == true) {
			for (msize_t hcol = col + 1; hcol < H_est.cols(); hcol++) {
				H_est(rowcount, hcol) *= -1.0f; /* invert sign */
			}
		}
	}
}

static float calculate_Phase_est_coarse(const MatrixCmpl & H_est_sq) {

	TRACE();

	float Phase_est_coarse = 0.0f;

	MatrixF angles(1, H_est_sq.cols(), MATRIX_UNINIT_DATA);
	for (msize_t col = 0; col < H_est_sq.cols(); col++) {
		float _angle = angle(H_est_sq(col));
		angles(col) = _angle;
	}

	MatrixF diffAngles(1, H_est_sq.cols() - 1, MATRIX_UNINIT_DATA);
	for (msize_t col = 0; col < diffAngles.cols(); col++) {
		diffAngles(col) = angles(col + 1) - angles(col);
	}

	for (msize_t col = 0; col < diffAngles.cols(); col++) {
		float v = diffAngles(col);
		if (v > PI_F)
			v -= (2.0f * PI_F);
		if (v < -PI_F)
			v += (2.0f * PI_F);
		diffAngles(col) = v;
		Phase_est_coarse += v;
	}

	Phase_est_coarse /= (static_cast<float>(diffAngles.cols()));

	return Phase_est_coarse;
}

static float calculate_Kest(const MatrixCmpl & H_demod) {
	TRACE();

	ComplexF mean_H_demod(0, 0);
	float mean_abs_H_demod_2 = 0.0f;

	for (msize_t i = 0; i < H_demod.cols(); i++) {
		mean_H_demod += H_demod(i);
		float t = std::abs(H_demod(i));
		mean_abs_H_demod_2 += (t * t);
	}

	mean_H_demod /= static_cast<float>(H_demod.cols());
	float abs_mean_H_demod = std::abs(mean_H_demod);

	mean_abs_H_demod_2 /= static_cast<float>(H_demod.cols());

	float abs_mean_H_demod_2 = abs_mean_H_demod * abs_mean_H_demod;
	float Kest = abs_mean_H_demod_2 / (mean_abs_H_demod_2 - abs_mean_H_demod_2) / 2.0f;

	return Kest;
}

void reconstructNonPhaseCoherent(const MatrixCmpl & IQ_A, const MatrixCmpl & IQ_B, MatrixCmpl & H_est, MatrixF & K_est, Matrix<int> & Uncert) {
	TRACE();

#if  __ARM_ARCH
	STOPWATCH(__FUNCTION__,STOPWATCH_TRACE_RECONSTRUCTNONPHASECOHERENT);
#else
	STOPWATCH(__FUNCTION__);
#endif

#if RECNPC_SHOW_INPUTS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  in IQ_A   =   " << IQ_A;
	std::cout << __FUNCTION__ << "  in IQ_B   =   " << IQ_B;
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

#if RANGE_CHECK
	if ((IQ_A.rows() != IQ_B.rows()) || (IQ_A.cols() != IQ_B.cols()))
		throw std::range_error("reconstructNonPhaseCoherent input matrices do not match");
	if ((H_est.rows() != IQ_A.rows()) || (H_est.cols() != IQ_A.cols()))
		throw std::range_error("reconstructNonPhaseCoherent input matrices do not match");
	if ((IQ_A.rows() != K_est.cols()) || (K_est.rows() != 1))
		throw std::range_error("reconstructNonPhaseCoherent input Kest wrong size");
	if ((IQ_A.rows() != Uncert.cols()) || (Uncert.rows() != 1))
		throw std::range_error("reconstructNonPhaseCoherent input Uncert wrong size");
#endif

	for (msize_t rowcount = 0; rowcount < H_est.rows(); rowcount++) {

#if RECNPC_SHOW_INPUTS
		printf("*** reconstructNonPhaseCoherent: process row: %d\n", rowcount);
#endif

		MatrixCmpl H_est_sq(1, H_est.cols(), MATRIX_UNINIT_DATA);

		for (msize_t col = 0; col < H_est.cols(); col++) {
			ComplexF prod = IQ_A(rowcount, col) * IQ_B(rowcount, col);
			H_est_sq(col) = prod;
			H_est(rowcount, col) = std::sqrt(prod);
		}

		sign_correct(H_est, rowcount);

		float Phase_est_coarse = calculate_Phase_est_coarse(H_est_sq);

		MatrixCmpl H_demod(1, H_est.cols(), MATRIX_UNINIT_DATA);
		for (msize_t col = 0; col < H_demod.cols(); col++) {
			ComplexF dm = expCmpl(-(Phase_est_coarse / 2) * static_cast<float>(col));
			ComplexF prod = H_est(rowcount, col) * dm;
			H_demod(col) = prod;
		}

		float kest = calculate_Kest(H_demod);
		K_est(rowcount) = kest;

		int uncertainty = 0;

		{
			MatrixCmpl H_lowside(1, H_demod.cols() - 2, MATRIX_CLEAR_DATA);
			MatrixCmpl H_highside(1, H_demod.cols() - 2, MATRIX_CLEAR_DATA);
			MatrixCmpl Err_low1(1, H_demod.cols() - 2, MATRIX_CLEAR_DATA);
			MatrixCmpl Err_low2(1, H_demod.cols() - 2, MATRIX_CLEAR_DATA);
			MatrixCmpl Err_high1(1, H_demod.cols() - 2, MATRIX_CLEAR_DATA);
			MatrixCmpl Err_high2(1, H_demod.cols() - 2, MATRIX_CLEAR_DATA);

			bool_vector Xa(H_demod.cols() - 2);
			assert((size_t )(H_demod.cols() - 2) <= Xa.size());
			bool_vector Xb(H_demod.cols() - 2);
			assert((size_t )(H_demod.cols() - 2) <= Xb.size());

			// init first two elements of array Xa and Xb
			Xa[0] = false;
			Xa[1] = false;
			Xb[0] = false;
			Xb[1] = false;

			for (msize_t cnt = 2; cnt < (H_demod.cols() - 2); cnt++) {
				H_lowside(cnt) = H_demod(cnt - 1) + H_demod(cnt - 1) - H_demod(cnt - 2);
				H_highside(cnt) = H_demod(cnt + 1) + H_demod(cnt + 1) - H_demod(cnt + 2);
				Err_low1(cnt) = H_demod(cnt) - H_lowside(cnt);
				Err_low2(cnt) = -H_demod(cnt) - H_lowside(cnt);
				Err_high1(cnt) = H_demod(cnt) - H_highside(cnt);
				Err_high2(cnt) = -H_demod(cnt) - H_highside(cnt);

				Xa[cnt] = abs(Err_low1(cnt)) > abs(Err_low2(cnt));    //  %X=abs(Err)-Parameter1*mean(abs(Err(3:end)));
				Xb[cnt] = abs(Err_high1(cnt)) > abs(Err_high2(cnt)); //  %X=abs(Err)-Parameter1*mean(abs(Err(3:end)));
				if ((Xa[cnt] == true) && (Xa[cnt - 1] == false)) {
					if ((Xb[cnt - 1] == true) || (Xb[cnt - 2] == true)) {
						for (msize_t hcol = cnt; hcol < H_est.cols(); hcol++) {
							H_est(rowcount, hcol) = -H_est(rowcount, hcol);
						}

						uncertainty = uncertainty + 1;
					}
				}
			}
		}

		Uncert(rowcount) = uncertainty;
	}
#if USE_MEMTRACE
        MEMTRACE_MARKER("reconstructNonPhaseCoherent");
#endif

#if RECNPC_SHOW_OUTPUTS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  out H_est   =   " << H_est;
	std::cout << __FUNCTION__ << "  out K_est   =   " << K_est;
	std::cout << __FUNCTION__ << "  out Uncertainty  =   " << Uncert;
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

}

void reconstructNonPhaseCoherent(const MatrixCmpl & IQ_A, const MatrixCmpl & IQ_B, MatrixCmpl & H_est, MatrixF & K_est, Matrix<int> & Uncert, const int *saturated_array ) {
	TRACE();

#if  __ARM_ARCH
	STOPWATCH(__FUNCTION__,STOPWATCH_TRACE_RECONSTRUCTNONPHASECOHERENT);
#else
	STOPWATCH(__FUNCTION__);
#endif

#if RECNPC_SHOW_INPUTS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  in IQ_A   =   " << IQ_A;
	std::cout << __FUNCTION__ << "  in IQ_B   =   " << IQ_B;
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

#if RANGE_CHECK
	if ((IQ_A.rows() != IQ_B.rows()) || (IQ_A.cols() != IQ_B.cols()))
		throw std::range_error("reconstructNonPhaseCoherent input matrices do not match");
	if ((H_est.cols() != IQ_A.cols()))
		throw std::range_error("reconstructNonPhaseCoherent input matrices do not match");
	if ((H_est.rows() != K_est.cols()) || (K_est.rows() != 1))
		throw std::range_error("reconstructNonPhaseCoherent input Kest wrong size");
	if ((H_est.rows() != Uncert.cols()) || (Uncert.rows() != 1))
		throw std::range_error("reconstructNonPhaseCoherent input Uncert wrong size");
#endif
        msize_t rowcount = 0;
	for (msize_t rowcount_IQ = 0; rowcount_IQ < IQ_A.rows(); rowcount_IQ++) {
            if(saturated_array[rowcount_IQ] == 0){
#if RECNPC_SHOW_INPUTS
                printf("*** reconstructNonPhaseCoherent: process row: %d\n", rowcount);
#endif

                MatrixCmpl H_est_sq(1, H_est.cols(), MATRIX_UNINIT_DATA);

                for (msize_t col = 0; col < H_est.cols(); col++) {
                        ComplexF prod = IQ_A(rowcount_IQ, col) * IQ_B(rowcount_IQ, col);
                        H_est_sq(col) = prod;
                        H_est(rowcount, col) = std::sqrt(prod);
                }

                sign_correct(H_est, rowcount);

                float Phase_est_coarse = calculate_Phase_est_coarse(H_est_sq);

                MatrixCmpl H_demod(1, H_est.cols(), MATRIX_UNINIT_DATA);
                for (msize_t col = 0; col < H_demod.cols(); col++) {
                        ComplexF dm = expCmpl(-(Phase_est_coarse / 2) * static_cast<float>(col));
                        ComplexF prod = H_est(rowcount, col) * dm;
                        H_demod(col) = prod;
                }

                float kest = calculate_Kest(H_demod);
                K_est(rowcount) = kest;

                int uncertainty = 0;

                {
                        MatrixCmpl H_lowside(1, H_demod.cols() - 2, MATRIX_CLEAR_DATA);
                        MatrixCmpl H_highside(1, H_demod.cols() - 2, MATRIX_CLEAR_DATA);
                        MatrixCmpl Err_low1(1, H_demod.cols() - 2, MATRIX_CLEAR_DATA);
                        MatrixCmpl Err_low2(1, H_demod.cols() - 2, MATRIX_CLEAR_DATA);
                        MatrixCmpl Err_high1(1, H_demod.cols() - 2, MATRIX_CLEAR_DATA);
                        MatrixCmpl Err_high2(1, H_demod.cols() - 2, MATRIX_CLEAR_DATA);

                        bool_vector Xa(H_demod.cols() - 2);
                        assert((size_t )(H_demod.cols() - 2) <= Xa.size());
                        bool_vector Xb(H_demod.cols() - 2);
                        assert((size_t )(H_demod.cols() - 2) <= Xb.size());

                        // init first two elements of array Xa and Xb
                        Xa[0] = false;
                        Xa[1] = false;
                        Xb[0] = false;
                        Xb[1] = false;

                        for (msize_t cnt = 2; cnt < (H_demod.cols() - 2); cnt++) {
                                H_lowside(cnt) = H_demod(cnt - 1) + H_demod(cnt - 1) - H_demod(cnt - 2);
                                H_highside(cnt) = H_demod(cnt + 1) + H_demod(cnt + 1) - H_demod(cnt + 2);
                                Err_low1(cnt) = H_demod(cnt) - H_lowside(cnt);
                                Err_low2(cnt) = -H_demod(cnt) - H_lowside(cnt);
                                Err_high1(cnt) = H_demod(cnt) - H_highside(cnt);
                                Err_high2(cnt) = -H_demod(cnt) - H_highside(cnt);

                                Xa[cnt] = abs(Err_low1(cnt)) > abs(Err_low2(cnt));    //  %X=abs(Err)-Parameter1*mean(abs(Err(3:end)));
                                Xb[cnt] = abs(Err_high1(cnt)) > abs(Err_high2(cnt)); //  %X=abs(Err)-Parameter1*mean(abs(Err(3:end)));
                                if ((Xa[cnt] == true) && (Xa[cnt - 1] == false)) {
                                        if ((Xb[cnt - 1] == true) || (Xb[cnt - 2] == true)) {
                                                for (msize_t hcol = cnt; hcol < H_est.cols(); hcol++) {
                                                        H_est(rowcount, hcol) = -H_est(rowcount, hcol);
                                                }

                                                uncertainty = uncertainty + 1;
                                        }
                                }
                        }
                }

                Uncert(rowcount) = uncertainty;
                rowcount++;
            }
	}
#if USE_MEMTRACE
        MEMTRACE_MARKER("reconstructNonPhaseCoherent");
#endif

#if RECNPC_SHOW_OUTPUTS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  out H_est   =   " << H_est;
	std::cout << __FUNCTION__ << "  out K_est   =   " << K_est;
	std::cout << __FUNCTION__ << "  out Uncertainty  =   " << Uncert;
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

}


} /* namespace ranging */
