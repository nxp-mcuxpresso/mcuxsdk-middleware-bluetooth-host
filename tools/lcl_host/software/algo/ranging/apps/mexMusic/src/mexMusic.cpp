/***********************************************************************************/
/*!
 *  @brief
 *  @file       mexMusic.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note
 */
/***********************************************************************************/

#include "mex.h"

#include <iostream>

#include <common/include/Functions.h>
#include <common/include/RangingSettings.h>

#include <common/include/Stream.h>
#include <common/include/Stopwatch.h>
#include <mexsupport/include/matlabAdapter.h>

#include <music/include/MusicRanging.h>


using namespace ranging;

void checkInputScalar(const mxArray *prhs[], int index) {
	const mxArray *hs = prhs[index];
	if (!mxIsDouble(hs) || mxIsComplex(hs) || mxGetNumberOfElements(hs) != 1) {
		mexErrMsgIdAndTxt("MATLAB:mexMusic:rhs", "Input %d must be a scalar.", index);
	}
}

void showHelp() {
	mexPrintf("HELP:\n");
	mexPrintf("[d,EigenVectors,EigenValues]=mexMusic(delta_F, L, H_est_th, TOL1, TOL2, MethodSubspaceSep, PS_approx, maxiter, MinLevel, InitPos, NFFT [, evdtype])\n");
	mexPrintf("delta_F=1e6              %%Frequency step size\n");
	mexPrintf("L=40                     %%The covariance matrix is square with dimension L by L\n");
	mexPrintf("H_est                    %%Channel estimate\n");
	mexPrintf("TOL1=1e-7                %%Tolerances used by evd\n");
	mexPrintf("TOL2=0.999               %%Tolerances used by evd\n");
	mexPrintf("MethodSubspaceSep=14     %%Maximum number of eigenvectors\n");
	mexPrintf("PS_approx=0.005          %%Stop criteria for the (first) peak-search in Pseudo-Spectrum\n");
	mexPrintf("maxiter=200              %%Maximum number of iterations before FirstPeak_PS stops\n");
	mexPrintf("MinLevel=0.1             %%Criteria to start Nelder-Mead (should be between 0 and 1)\n");
	mexPrintf("InitPos=0                %%Start position used for peak-search\n");
	mexPrintf("NFFT=512                 %%Determines the step-size used by peak-search\n");
	mexPrintf("evdtype=0                %%Optional: choose EVD algorithm. 0=poweriterations, 1=eigen-saes\n");
}

void showSettings(RangingSettings & cfg) {
	mexPrintf("TOL1=%e               \n", cfg.TOL1);
	mexPrintf("TOL2=%e               \n", cfg.TOL2);
	mexPrintf("MethodSubspaceSep=%d  \n", cfg.MethodSubspaceSep);
	mexPrintf("PS_approx=%e          \n", cfg.PS_approx);
	mexPrintf("maxiter=%d            \n", cfg.maxiter);
	mexPrintf("MinLevel=%f           \n", cfg.MinLevel);
	mexPrintf("InitPos =%f           \n", cfg.InitPos);
	mexPrintf("NFFT=%f               \n", cfg.NFFT);
}

/* Make sure these are aligned with defines in MusicRanging.h */
#define ALGO_TYPE_EVD			0	/* IMEC EVD Power Iterations */
#define ALGO_TYPE_EIGEN_SAES	1	/* Self Adjoint Eigen Solver */


/* 	[d,EigenVectors,EigenValues]=mexMusic(delta_F, L, H_est_th, TOL1, TOL2, MethodSubspaceSep, PS_approx, maxiter, MinLevel, InitPos, NFFT [, evdtype]) */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

	//mexPrintf("%s : %s - %s\n", __PRETTY_FUNCTION__, __DATE__, __TIME__);

	MusicRanging mr;

	/* Check for proper number of input arguments. */
	if (nrhs < 11) {
		showHelp();
		mexErrMsgIdAndTxt("MATLAB:mexMusic:rhs", "This function requires 11 input variables");
	}

	/* Check for proper number of output arguments. */
	if (nlhs != 3) {
		showHelp();
		mexErrMsgIdAndTxt("MATLAB:mexMusic:lhs", "Three output variables required.");
	}

	checkInputScalar(prhs, 0);
	checkInputScalar(prhs, 1);

	/* Check for complex input values */
	if (!mxIsComplex(prhs[2])) {
		mexErrMsgIdAndTxt("MATLAB:mexMusic:real", "Input matrix must be complex.");
	}

	/* check other input arguments */
	for (int index = 3; index < 11; index++) {
		checkInputScalar(prhs, index);
	}

	/* extract parameters */
	float deltaF = mxGetScalar(prhs[0]);
	int L = (int) mxGetScalar(prhs[1]);

	/* construct H_est matrix from matlab argument */
	MatrixCmpl H_est(mat2cpp_complex(prhs[2]));

	/* extract configuration settings */
	int index = 3;
	mr.config().TOL1 = mxGetScalar(prhs[index++]);
	mr.config().TOL2 = mxGetScalar(prhs[index++]);
	mr.config().MethodSubspaceSep = (msize_t)mxGetScalar(prhs[index++]);
	mr.config().PS_approx = mxGetScalar(prhs[index++]);
	mr.config().maxiter = (int) mxGetScalar(prhs[index++]);
	mr.config().MinLevel = mxGetScalar(prhs[index++]);
	mr.config().InitPos = mxGetScalar(prhs[index++]);
	mr.config().NFFT = mxGetScalar(prhs[index++]);

	int evdType = ALGO_TYPE_EIGEN_SAES;
	if (nrhs >= 12) {
		evdType = (int) mxGetScalar(prhs[index++]);
	}

	// showSettings(mr.config());

	/* define output parameters */
	float out_dist_est;
	MatrixCmpl eigenvects(L, L, MATRIX_CLEAR_DATA);
	MatrixF EVs(1, L, MATRIX_CLEAR_DATA);

	mr.calculate(deltaF, L, H_est, out_dist_est, eigenvects, EVs, evdType);

	//mexPrintf("Distance: %f\n", out_dist_est);

	/* fill the output pointers and data */
	plhs[0] = mxCreateDoubleScalar(out_dist_est);
	plhs[1] = cpp2mat_complex(eigenvects);
	plhs[2] = cpp2mat_float(EVs);
}
