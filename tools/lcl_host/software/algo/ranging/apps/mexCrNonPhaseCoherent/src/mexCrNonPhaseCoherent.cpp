/***********************************************************************************/
/*!
 *  @brief
 *  @file       mexCrNonPhaseCoherent.cpp
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
#include <crNonPhaseCoherent/include/reconstructNonPhaseCoherent.h>

#include <mexsupport/include/matlabAdapter.h>


using namespace ranging;

void checkInputScalar(const mxArray *prhs[], int index) {
	const mxArray *hs = prhs[index];
	if (!mxIsDouble(hs) || mxIsComplex(hs) || mxGetNumberOfElements(hs) != 1) {
		mexErrMsgIdAndTxt("MATLAB:mexCrNonPhaseCoherent:rhs", "Input %d must be a scalar.", index);
	}
}

void showHelp() {
	mexPrintf("%s : %s - %s\n", __PRETTY_FUNCTION__, __DATE__, __TIME__);
	mexPrintf("\n");
	mexPrintf("HELP:\n");
	mexPrintf("[H_est, K_est, Uncertainty]=mexCrNonPhaseCoherent(IQ_A,IQ_B);\n");
	mexPrintf("\n");
}

/* d = func(1e6, 40, H_est)  */

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

	//mexPrintf("%s : %s - %s\n", __PRETTY_FUNCTION__, __DATE__, __TIME__);

	/* Check for proper number of input arguments. */
	if (nrhs != 2) {
		showHelp();
		mexErrMsgIdAndTxt("MATLAB:mexCrNonPhaseCoherent:rhs", "This function requires 2 input variables");
	}

	/* Check for proper number of output arguments. */
	if (nlhs != 3) {
		showHelp();
		mexErrMsgIdAndTxt("MATLAB:mexCrNonPhaseCoherent:lhs", "This function requires 3 output variables");
	}

	/* Check for complex input values */
	if (!mxIsComplex(prhs[0])) {
		mexErrMsgIdAndTxt("MATLAB:mexCrNonPhaseCoherent:real", "Input matrix IQ_A must be complex.");
	}

	if (!mxIsComplex(prhs[1])) {
		mexErrMsgIdAndTxt("MATLAB:mexCrNonPhaseCoherent:real", "Input matrix IQ_B must be complex.");
	}

	/* construct H_est matrix from matlab argument */
	MatrixCmpl IQ_A(mat2cpp_complex(prhs[0]));
	MatrixCmpl IQ_B(mat2cpp_complex(prhs[1]));


	MatrixCmpl H_est(IQ_A.rows(), IQ_A.cols(), MATRIX_UNINIT_DATA);
	MatrixF K_est(1, H_est.rows(), MATRIX_UNINIT_DATA);
	Matrix<int> Uncertainty(1, H_est.rows(), MATRIX_UNINIT_DATA);

	reconstructNonPhaseCoherent(IQ_A, IQ_B, H_est, K_est, Uncertainty);

	plhs[0] = cpp2mat_complex(H_est);
	plhs[1] = cpp2mat_float(K_est);
	plhs[2] = cpp2mat_integer(Uncertainty);
}
