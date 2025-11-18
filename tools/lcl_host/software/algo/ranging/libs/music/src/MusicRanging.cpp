/***********************************************************************************/
/*!
 *  @brief
 *  @file       MusicRanging.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note
 */
/***********************************************************************************/

#include <stdio.h>
#include <algorithm>
#include <assert.h>
#include <iostream>

#include <common/include/Functions.h>
#include <common/include/Stream.h>
#include <common/include/Stopwatch.h>
#include <common/include/Constants.h>

#include <h2cov/include/h2cov.h>
#include <music/include/MusicRanging.h>
#include <channelReconstruct_2D/include/ChannelReconstruct_2D.h>
#include <crNonPhaseCoherent/include/reconstructNonPhaseCoherent.h>

#ifdef COMPONENT_EVD
#include <evd/include/evd.h>
#endif

#if defined(COMPONENT_EIGEN_SOLVER_SAES) && defined(COMPONENT_EIGEN)
#include <eigenSolverSaes/include/EigenSolverSaes.h>
#endif

#if USE_MEMTRACE
#include <common/include/memtrace.h>
#endif

#define MUSIC_SHOW_INPUTS				0		/* 1 = on, 0 = off */
#define MUSIC_SHOW_OUTPUTS				0		/* 1 = on, 0 = off */
#define MUSIC_SHOW_INTERNAL				0		/* 1 = on, 0 = off */

#define IS_FREQ_SMP_VALID(index, mask) ((mask == NULL) || (((mask[(index)/32] >> ((index)%32)) & 0x1) == 1U))

#if MUSIC_SHOW_INTERNAL
#define PRINT_VARIABLE(m) do { std::cout <<  __FUNCTION__ << "  " << #m << " = " << m << std::endl;  } while (0)
#else
#define PRINT_VARIABLE(m) do { /* empty */ } while (0)
#endif

#if GLOBAL_DEF_EVD_TYPE
int global_evd_type = ALGO_TYPE_EVD;
#endif

namespace ranging {

/* ------------------------------------------------------------------------------
 *
 * ------------------------------------------------------------------------------ */
MusicRanging::MusicRanging() {
	/* empty */
}

MusicRanging::MusicRanging(RangingSettings & r) : m_config(r) {
	/* empty */
}


msize_t MusicRanging::SubSpaceSep(const MatrixF &EVs) {
	TRACE();
	msize_t Ns = std::min(EVs.cols(), m_config.MethodSubspaceSep);

#if MUSIC_SHOW_INTERNAL
	PRINT_VARIABLE(EVs.cols());
	PRINT_VARIABLE(m_config.MethodSubspaceSep);
	PRINT_VARIABLE(Ns);
#endif

	return Ns;
}

MatrixCmpl MusicRanging::fftvec(const msize_t N_b, const float omega) {
	TRACE();

	MatrixCmpl vec(N_b, 1, MATRIX_UNINIT_DATA);

	for (msize_t i = 0; i < vec.rows(); i++) {
		vec(i) = expCmpl(-1.0f * i * omega);
	}

#if MUSIC_SHOW_INTERNAL
	PRINT_VARIABLE(N_b);
	PRINT_VARIABLE(omega);
	PRINT_VARIABLE(vec);
#endif

	return vec;
}

/**
 *
 * @param SigVects
 * @param omega
 * @param L
 * @return output is always in the range [0 .. L]
 */
float MusicRanging::InvPS(const MatrixCmpl &SigVects, const float omega, const msize_t L) {
	TRACE();

	MatrixCmpl FFTMATRIX(fftvec(L, omega));

	MatrixCmpl h(SigVects * FFTMATRIX);

#if MUSIC_SHOW_INTERNAL
	PRINT_VARIABLE(SigVects);
	PRINT_VARIABLE(FFTMATRIX);
	PRINT_VARIABLE(h);
#endif

	float sum = 0.0f;
	for (msize_t i = 0; i < h.rows(); i++) {
		sum += std::norm(h(i));
	}

	float den = (float) L - sum;

#if MUSIC_SHOW_INTERNAL
	PRINT_VARIABLE(sum);
	PRINT_VARIABLE(den);
#endif

	return den;
}

void MusicRanging::FirstPeak_PS(const MatrixCmpl &SigVects, float &omega, float &fval, const float delta_F, const msize_t L) {
	TRACE();

#if  __ARM_ARCH
	STOPWATCH(__FUNCTION__,STOPWATCH_TRACE_FIRSTPEAK_PS);
#else
	STOPWATCH(__FUNCTION__);
#endif

#if MUSIC_SHOW_INTERNAL
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  SigVects = " << SigVects << std::endl;
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

	float CONSTANT = (2.0f * PI * delta_F) / SPEED_OF_LIGHT;
	float tolx = m_config.PS_approx * CONSTANT;

	float v1 = m_config.InitPos * (2.0f * PI * delta_F) / SPEED_OF_LIGHT; /* initial guess, could be zero .. */
	float fv1 = InvPS(SigVects, v1, L);
	float step = 2.0f * PI / m_config.NFFT; /* initial step size in forward (increasing omega) direction */

	int itercount = 1;
	bool isUnderThreshold = false;
    float fv0 = fv1;
	float v2 = 0.0f;
	float fv2 = 0.0f;
	while (itercount < m_config.maxiter) {
		v2 = v1 + step;
		fv2 = InvPS(SigVects, v2, L);
		if (isUnderThreshold == false) {
			/* first we step forward until the InvPS <= MinLevel*L */
			if ((fv2 < (m_config.MinLevel * L)) && (fv2 < fv1)) {
				/* we have reached it */
				isUnderThreshold = true;
			}
		} else {
			if (fv2 < fv1) {
			} else {
				step = -step / 2.0f;
			}
		}
		if (std::abs(step) <= tolx) {
			/* if the step-size (abs(step)) is below tolx, we stop */
			break;
		} else {
			fv1 = fv2;
			v1 = v2;
		}
		itercount++;
	}

	/* select the best result (lowest value) */
	if (itercount >= m_config.maxiter) {
        if((isUnderThreshold == false) && (fv0 < (m_config.MinLevel * L))){
            omega = 0.0;
            fval = fv0;
        }else{
            omega = NAN;
            fval = NAN;
        }
	} else if (fv1 > fv2) {
		omega = fv2;
		fval = fv2;
	} else {
		omega = v1;
		fval = fv1;
	}

#if USE_MEMTRACE
        MEMTRACE_MARKER("FirstPeak_PS");
#endif

#if	MUSIC_SHOW_INTERNAL
	PRINT_VARIABLE(omega);
	PRINT_VARIABLE(fval);
	PRINT_VARIABLE(itercount);
#endif

}


/**
 * Wrapper for music() function (to work without eigenvects and EVs outputs)
 * @param delta_F
 * @param L
 * @param H_est
 * @param dist_est
 */
void MusicRanging::calculate(const float delta_F, const int L, const MatrixCmpl &H_est, float &dist_est, int evdType){
    TRACE();
    float likeliness;
    MatrixCmpl eigenvects(L, std::min(m_config.MethodSubspaceSep, L), MATRIX_CLEAR_DATA);
    MatrixF EVs(1, std::min(m_config.MethodSubspaceSep, L), MATRIX_UNINIT_DATA);
    calculate(delta_F, L, H_est, dist_est, likeliness, eigenvects, EVs, evdType);
}

void MusicRanging::calculate(const float delta_F, const int L, const MatrixCmpl &H_est, float &dist_est, float &likeliness, int evdType) {
	TRACE();
	MatrixCmpl eigenvects(L, std::min(m_config.MethodSubspaceSep, L), MATRIX_CLEAR_DATA);
	MatrixF EVs(1, std::min(m_config.MethodSubspaceSep, L), MATRIX_UNINIT_DATA);
	calculate(delta_F, L, H_est, dist_est, likeliness, eigenvects, EVs, evdType);
}

void MusicRanging::calculate(const float delta_F, const int L, const MatrixCmpl &H_est, float &dist_est, float &likeliness, MatrixCmpl &eigenvects, MatrixF &EVs, int evdType) {
	TRACE();

#if  __ARM_ARCH

	STOPWATCH(__FUNCTION__,STOPWATCH_TRACE_CALCULATE);
#else
	STOPWATCH(__FUNCTION__);
#endif


#if MUSIC_SHOW_INPUTS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  in H_est.rows()  =   " << H_est.rows() << std::endl;
	std::cout << __FUNCTION__ << "  in H_est.cols()  =   " << H_est.cols() << std::endl;
	std::cout << __FUNCTION__ << "  in delta_F  =   " << delta_F << std::endl;
	std::cout << __FUNCTION__ << "  in L        =   " << L << std::endl;
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

	dist_est = 0.0f;
        float CovTrace = 0.0f;

	{
        MatrixCmpl Cov(L, L, MATRIX_CLEAR_DATA);
        for (msize_t n = 0; n < H_est.rows(); n++) {
            /* calculate h2cov for this row and add it inplace */
            MatrixCmpl Hrow(H_est.getRow(n));
            h2cov_inplace(Hrow, Cov);
        }
        for (msize_t n = 0; n < L; n++) {
            CovTrace += Cov(n, n).real();
        }
		struct evd_tol_s tol;
		tol.maxeig = m_config.TOL1;
		tol.sumeig = m_config.TOL2;

		switch (evdType) {

#ifdef COMPONENT_EVD
                    case ALGO_TYPE_EVD:
			evd(Cov, tol, m_config.MethodSubspaceSep, eigenvects, EVs);
			break;
#endif

#if defined(COMPONENT_EIGEN_SOLVER_SAES) && defined(COMPONENT_EIGEN)
                    case ALGO_TYPE_EIGEN_SAES: {
			/* -----------------------------------------------------------------
			 * Run the EVD in Eigen library :: SelfAdjointEigenSolver
			 * ----------------------------------------------------------------- */
			EigenSolverSaes es;
			es.selfAdjointEigenSolver(Cov, tol, m_config.MethodSubspaceSep, eigenvects, EVs);
                }
			break;
#endif

                    default:
			assert(0 && "*** Unsupported Eigen Value Decomposition Type configured\n");
			tol = tol; /* keep compiler happy */
			break;
		}
	}

	PRINT_VARIABLE(Ns);

	float omega = 0.0f;
	float y = 0.0f;

	/* resize eigenvects and EVs based on subspaces */
	msize_t Ns = SubSpaceSep(EVs);
	if (Ns > 0) {
		if (Ns < EVs.cols())
			EVs.resize(Span(0, 0), Span(0, Ns - 1));
		if (Ns < eigenvects.cols())
			eigenvects.resize(Span(0, eigenvects.rows() - 1), Span(0, Ns - 1));
		FirstPeak_PS(eigenvects.transpose(), omega, y, delta_F, L);

		PRINT_VARIABLE(omega);
		PRINT_VARIABLE(y);

		dist_est = omega / (2.0f * PI * delta_F) * SPEED_OF_LIGHT;
		calculate_Likeliness(L, Ns, omega, CovTrace, eigenvects, EVs, likeliness);
	}
	else 
	{
                dist_est = FLT_MIN;
                likeliness = -1.0f;
#if RANGE_CHECK
		throw std::out_of_range("SubSpaceSep of EVs, called Ns, is inferior or equal to 0");
#endif
	}

#if MUSIC_SHOW_OUTPUTS
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
	std::cout << __FUNCTION__ << "  out dist_est  = " << dist_est << std::endl;
    std::cout << __FUNCTION__ << "  likeliness    = " << likeliness << std::endl;
	std::cout << __FUNCTION__ << "  in eigenvects = " << eigenvects << std::endl;
	std::cout << __FUNCTION__ << "  in EVs        = " << EVs << std::endl;
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

#if RANGE_CHECK
	if( EVs.cols() > m_config.MethodSubspaceSep)
		throw std::range_error("EVs dimension error");
#endif

}

void MusicRanging::calculate_Likeliness(const int L, msize_t Ns, float omega, float &CovTrace, MatrixCmpl &eigenvects, MatrixF &EVs, float &likeliness)
{
    TRACE();

    /* the eigenvectors (eigenvects) have dimension Ns x L, where Ns equals the number of eigenvectors associated with
     * the signal-subspace and L the dimension of the coveriance-matrix. the eigen-values (EVs) have dimension Ns x 1.
	 * CovTrace is the trace of the covariance-matrix
     */

#if MUSIC_SHOW_INPUTS
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
    std::cout << __FUNCTION__ << "  in Ns         = " << Ns << std::endl;
    std::cout << __FUNCTION__ << "  in CovTrace   = " << CovTrace << std::endl;
	std::cout << __FUNCTION__ << "  in eigenvects = " << eigenvects << std::endl;
    std::cout << __FUNCTION__ << "  in EVs        = " << EVs << std::endl;
	std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

	/* Reconstruct signal from estimated distance */
    MatrixCmpl fftmatrix(fftvec(L, omega));
	/* Compute dot product btw eigen vectors and FFT matrix */
    MatrixCmpl h(eigenvects.transpose() * fftmatrix);

	MatrixF P(1, Ns, MATRIX_UNINIT_DATA);
    for (msize_t cnt = 0; cnt < Ns; cnt++)
    {
        P(0, cnt) = std::norm(h(cnt)) / L;
    }

#if MUSIC_SHOW_INTERNAL
    std::cout << __FUNCTION__ << "  h  = " << h << std::endl;
    std::cout << __FUNCTION__ << "  P  = " << P << std::endl;
#endif

    MatrixF t(P * EVs.transpose());

    likeliness = t(0, 0) / CovTrace;

#if MUSIC_SHOW_OUTPUTS
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
    std::cout << __FUNCTION__ << "  out likeliness  = " << likeliness << std::endl;
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif
}

msize_t MusicRanging::check_saturated_paths(const MatrixCmpl &IQ_A, const MatrixCmpl &IQ_B, int *saturated_array) 
{
    msize_t nPairs = IQ_A.rows();
    msize_t nTones = IQ_A.cols();

    msize_t nb_unsaturated_paths = 0;

    float sat_tresh = 0.98; /* IQs are considered as saturated when they exceed 98% of available magnitude */
    int sat_limit = (40 * nTones * 2) / 100;
    int nb_saturated;

    for (msize_t p = 0; p < nPairs; p++)
    {
        nb_saturated = 0;
        for (msize_t t = 0; t < nTones; t++)
        {
            if ((std::abs(IQ_A(p, t).real()) >= sat_tresh) || (std::abs(IQ_A(p, t).imag()) >= sat_tresh))
            {
                nb_saturated++;
            }
            if ((std::abs(IQ_B(p, t).real()) >= sat_tresh) || (std::abs(IQ_B(p, t).imag()) >= sat_tresh))
            {
                nb_saturated++;
            }
        }
        if (nb_saturated >= sat_limit)
        {
            saturated_array[p] = 1;
        }
        else 
        {
            saturated_array[p] = 0;
            nb_unsaturated_paths++;
        }
    }
    /* Make sure at least one paths remains */
    if (nb_unsaturated_paths == 0)
    {
        nb_unsaturated_paths = 1;
        saturated_array[0] = 0; /* keep first one */
    }
    return nb_unsaturated_paths;
}

void MusicRanging::calculate_SRDE(const MatrixCmpl & IQ_A,const MatrixCmpl & IQ_B, unsigned int *FreqMask, const float delta_F,const int n_ap, const int nb_freq, float &dist_est, float &likeliness, int evdType)
{
    TRACE();

#if MUSIC_SHOW_INPUTS
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
    std::cout << __FUNCTION__ << "  in nb_freq  =   " << nb_freq << std::endl;
    std::cout << __FUNCTION__ << "  in delta_F  =   " << delta_F << std::endl;
    std::cout << __FUNCTION__ << "  in L        =   " << m_config.L << std::endl;
    std::cout << __FUNCTION__ << "  in IQ_A     =   " << IQ_A << std::endl;
    std::cout << __FUNCTION__ << "  in IQ_B     =   " << IQ_B << std::endl;
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

    msize_t unsaturated_aps = 0;
    
    /* Remove saturated paths */
    int saturated_array[4] = {0,0,0,0};
    if (n_ap == 1)
    {
        unsaturated_aps = n_ap; /* = 1 */
    }
    else
    {
        unsaturated_aps = check_saturated_paths(IQ_A, IQ_B, saturated_array);
    }
    MatrixCmpl H_est(unsaturated_aps, nb_freq, MATRIX_UNINIT_DATA);

    MatrixF K_est(1, H_est.rows(), MATRIX_UNINIT_DATA);
    Matrix<int> Uncertainty(1, H_est.rows(), MATRIX_UNINIT_DATA);

    ChannelReconstruct_2D(IQ_A, IQ_B, H_est, K_est, Uncertainty,saturated_array);

    for (ranging::msize_t i = 0; i < H_est.rows() ; i++) {
        for (ranging::msize_t j = 0; j < H_est.cols(); j++) {
            if (!IS_FREQ_SMP_VALID (j, FreqMask)) {
                H_est(i,j) = ranging::ComplexF(0,0);
            }
        }
    }

    MatrixCmpl eigenvects(m_config.L, std::min(m_config.MethodSubspaceSep, m_config.L), MATRIX_CLEAR_DATA);
    MatrixF EVs(1, std::min(m_config.MethodSubspaceSep, m_config.L), MATRIX_UNINIT_DATA);
    calculate(delta_F, m_config.L, H_est, dist_est, likeliness, eigenvects, EVs, evdType);
}

} /* namespace ranging */
