/***********************************************************************************/
/*!
 *  @brief      
 *  @file       PythonWrapper.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#include <stdio.h>

#include "PythonWrapper.h"

#include <common/include/Stream.h>
#include <common/include/Stopwatch.h>

#ifdef COMPONENT_CRNPC
#include <crNonPhaseCoherent/include/reconstructNonPhaseCoherent.h>
#endif

#ifdef COMPONENT_CRPC
#include <crPhaseCoherent/include/reconstructPhaseCoherent.h>
#endif

#include <music/include/MusicRanging.h>


//#define PTRACE(x) 	do { puts(__PRETTY_FUNCTION__); } while (0)
#define PTRACE(x) 	do { /* empty */ } while (0)

using namespace ranging;

void Worker::showSettings() const {
	printf("TOL1=%e;              \n", settings.TOL1);
	printf("TOL2=%e;              \n", settings.TOL2);
	printf("MethodSubspaceSep=%d; \n", settings.MethodSubspaceSep);
	printf("PS_approx=%e;         \n", settings.PS_approx);
	printf("maxiter=%d;           \n", settings.maxiter);
	printf("MinLevel=%f;          \n", settings.MinLevel);
	printf("InitPos =%f           \n", settings.InitPos);
	printf("NFFT=%f;              \n", settings.NFFT);
}

float Worker::calculateCrPhaseCoherent(DataPhMag &m, const float delta_F, const int L) {
	PTRACE();

#ifdef COMPONENT_CRPC
	msize_t rows = m.rows();
	msize_t cols = m.cols();

	MatrixCmpl H_est(rows, cols, MATRIX_UNINIT_DATA);
	{
		MatrixF Ph_A(rows, cols, &m.phaseA[0]); /* get pointer for std::vector data of floats */
		MatrixF mag_A(rows, cols, &m.magnitudeA[0]); /* get pointer for std::vector data of floats */
		MatrixF Ph_B(rows, cols, &m.phaseB[0]); /* get pointer for std::vector data of floats */
		MatrixF mag_B(rows, cols, &m.magnitudeB[0]); /* get pointer for std::vector data of floats */

		msize_t N_bits = 8;
		MatrixF AngleOffset(rows, cols, MATRIX_UNINIT_DATA);
		reconstructPhaseCoherent(Ph_A, mag_A, Ph_B, mag_B, N_bits, H_est, AngleOffset);
	}

	{
		/* copy H_est to output */
		msize_t j = 0;
		for (std::vector<std::complex<float> >::iterator i = m.H_est.data.begin(); i != m.H_est.data.end(); i++) {
			*i = H_est(j);
			j++;
		}
	}

	float dist_est;

	//std::cout << __FUNCTION__ << "  " << H_est << std::endl;

	MusicRanging mr(settings);
	mr.calculate(delta_F, L, H_est, dist_est);

	// printf("MusicRanging::calculate(...) =  %f meter\n", dist_est);

	return dist_est;
#else
	return -1.0f;
#endif
}

float Worker::calculateCrNonPhaseCoherent(DataIQ &m, const float delta_F, const int L) {
	PTRACE();
#ifdef COMPONENT_CRNPC

	msize_t cols = m.cols();
	msize_t rows = m.rows();

	MatrixCmpl H_est(rows, cols, MATRIX_UNINIT_DATA);
	{

		MatrixCmpl IQ_A(rows, cols, &m.IQ_A[0]);
		MatrixCmpl IQ_B(rows, cols, &m.IQ_B[0]);

		MatrixF K_est(1, H_est.rows(), MATRIX_UNINIT_DATA);
		Matrix<int> Uncertainty(1, H_est.rows(), MATRIX_UNINIT_DATA);

		reconstructNonPhaseCoherent(IQ_A, IQ_B, H_est, K_est, Uncertainty);

		{
			/* copy out Kest data */
			msize_t j = 0;
			for (std::vector<float>::iterator i = m.Kest.begin(); i != m.Kest.end(); i++) {
				*i = K_est(j);
			}
		}

		{
			/* copy out Uncertainty data */
			msize_t j = 0;
			for (std::vector<int>::iterator i = m.Uncertainty.begin(); i != m.Uncertainty.end(); i++) {
				*i = Uncertainty(j);
			}
		}

	}

	{
		/* copy H_est to output */
		msize_t j = 0;
		for (std::vector<std::complex<float> >::iterator i = m.H_est.data.begin(); i != m.H_est.data.end(); i++) {
			*i = H_est(j);
			j++;
		}
	}

	float dist_est;

	//std::cout << __FUNCTION__ << "  " << H_est << std::endl;

	MusicRanging mr(settings);
	mr.calculate(delta_F, L, H_est, dist_est);

	//printf("MusicRanging::calculateIQ(...) =  %f meter\n", dist_est);

	return dist_est;
#else
	return -1.0f;
#endif
}

float Worker::calculateDistance(MusicData &m, int evdType) {
	PTRACE();

	float dist_est_out;
    float likeliness;

	MatrixCmpl H_est(m.hest.rows(), m.hest.cols(), MATRIX_UNINIT_DATA);

	{
		/* copy MusicData :: hest data to H_est */
		msize_t j = 0;
		for (auto i = m.hest.data.begin(); i != m.hest.data.end(); i++, j++) {
			H_est(j) = *i;
		}
	}

	MatrixCmpl eigenvects(m.EigenVects.rows(), m.EigenVects.cols(), MATRIX_CLEAR_DATA);
	MatrixF EVs(1, m.cols(), MATRIX_CLEAR_DATA);

	//std::cout << __FUNCTION__ << "  " << H_est << std::endl;

	MusicRanging mr(settings);
	mr.calculate(m.delta_F, m.L, H_est, dist_est_out, likeliness, eigenvects, EVs, evdType);

	// printf("MusicRanging::calculate(...) =  %f meter\n", dist_est_out);

	{
		/* resize, beacuse size of matrix changes in music() */
		m.EigenVects.resize(eigenvects.rows(), eigenvects.cols());

		/* copy eigenvects to MusicData :: EigenVects */
		msize_t j = 0;
		for (auto i = m.EigenVects.data.begin(); i != m.EigenVects.data.end(); i++, j++) {
			*i = eigenvects(j);
		}
	}

	{
		/* copy EVs to MusicData :: EigenValues */
		msize_t j = 0;
		m.EigenValues.resize(EVs.cols());
		for (auto i = m.EigenValues.begin(); i != m.EigenValues.end(); i++, j++) {
			*i = EVs(j);
		}
	}

	return dist_est_out;
}

void Worker::crPhaseCoherent(DataPhMag &m) const {
	PTRACE();

#ifdef COMPONENT_CRPC
	msize_t rows = m.rows();
	msize_t cols = m.cols();

	MatrixCmpl H_est(rows, cols, MATRIX_UNINIT_DATA);
	{
		MatrixF Ph_A(rows, cols, &m.phaseA[0]); /* get pointer for std::vector data of floats */
		MatrixF mag_A(rows, cols, &m.magnitudeA[0]); /* get pointer for std::vector data of floats */
		MatrixF Ph_B(rows, cols, &m.phaseB[0]); /* get pointer for std::vector data of floats */
		MatrixF mag_B(rows, cols, &m.magnitudeB[0]); /* get pointer for std::vector data of floats */

		msize_t N_bits = 8;
		MatrixF AngleOffset(rows, cols, MATRIX_UNINIT_DATA);
		reconstructPhaseCoherent(Ph_A, mag_A, Ph_B, mag_B, N_bits, H_est, AngleOffset);
	}

	{
		/* copy H_est to output */
		msize_t j = 0;
		for (std::vector<std::complex<float> >::iterator i = m.H_est.data.begin(); i != m.H_est.data.end(); i++) {
			*i = H_est(j);
			j++;
		}
	}
#endif
}

void Worker::crNonPhaseCoherent(DataIQ &m) const {
	PTRACE();

#ifdef COMPONENT_CRNPC
	msize_t cols = m.cols();
	msize_t rows = m.rows();

	MatrixCmpl H_est(rows, cols, MATRIX_UNINIT_DATA);
	{
		MatrixCmpl IQ_A(rows, cols, &m.IQ_A[0]);
		MatrixCmpl IQ_B(rows, cols, &m.IQ_B[0]);

		MatrixF K_est(1, H_est.rows(), MATRIX_UNINIT_DATA);
		Matrix<int> Uncertainty(1, H_est.rows(), MATRIX_UNINIT_DATA);

		reconstructNonPhaseCoherent(IQ_A, IQ_B, H_est, K_est, Uncertainty);

		{
			/* copy out Kest data */
			msize_t j = 0;
			for (std::vector<float>::iterator i = m.Kest.begin(); i != m.Kest.end(); i++) {
				*i = K_est(j);
			}
		}

		{
			/* copy out Uncertainty data */
			msize_t j = 0;
			for (std::vector<int>::iterator i = m.Uncertainty.begin(); i != m.Uncertainty.end(); i++) {
				*i = Uncertainty(j);
			}
		}
	}

	{
		/* copy H_est to output */
		msize_t j = 0;
		for (std::vector<std::complex<float> >::iterator i = m.H_est.data.begin(); i != m.H_est.data.end(); i++) {
			*i = H_est(j);
			j++;
		}
	}
#endif
}

// end
