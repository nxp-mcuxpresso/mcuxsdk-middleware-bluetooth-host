/***********************************************************************************/
/*!
 *  @brief
 *  @file       mexCrPhaseCoherent.cpp
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
#include <crPhaseCoherent/include/reconstructPhaseCoherent.h>

#include <mexsupport/include/matlabAdapter.h>


using namespace ranging;

void checkInputScalar(const mxArray *prhs[], int index) {
	const mxArray *hs = prhs[index];
	if (!mxIsDouble(hs) || mxIsComplex(hs) || mxGetNumberOfElements(hs) != 1) {
		mexErrMsgIdAndTxt("MATLAB:mexCrPhaseCoherent:rhs", "Input %d must be a scalar.", index);
	}
}

void showHelp() {
	mexPrintf("%s : %s - %s\n", __PRETTY_FUNCTION__, __DATE__, __TIME__);
	mexPrintf("\n");
	mexPrintf("HELP:\n");
	mexPrintf("[H_est,AngleOffset]=mexCrPhaseCoherent(Ph_A, Mag_A, Ph_B, Mag_B, N_bits);\n");
	mexPrintf("\tPh_A, Mag_A, Ph_B, Mag_B = Matrix of Doubles  (example size: 1x80);\n");
	mexPrintf("\tN_Bits = Integer value\n");
	mexPrintf("\tH_est = Matrix of Complex Doubles\n");
	mexPrintf("\tAngleOffset = Matrix of Doubles\n");
	mexPrintf("\n");
}


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

	//mexPrintf("%s : %s - %s\n", __PRETTY_FUNCTION__, __DATE__, __TIME__);

	/* Check for proper number of input arguments. */
	if (nrhs != 5) {
		showHelp();
		mexErrMsgIdAndTxt("MATLAB:mexCrPhaseCoherent:rhs", "This function requires 5 input variables");
	}

	/* Check for proper number of output arguments. */
	if (nlhs != 2) {
		showHelp();
		mexErrMsgIdAndTxt("MATLAB:mexCrPhaseCoherent:lhs", "Two outputs required.");
	}

	/* Check for single precision float input values */
	if (!mxIsDouble(prhs[0])) {
		mexErrMsgIdAndTxt("MATLAB:mexCrPhaseCoherent:double", "Input matrix Ph_A must be double.");
	}

	if (!mxIsDouble(prhs[1])) {
		mexErrMsgIdAndTxt("MATLAB:mexCrPhaseCoherent:double", "Input matrix Mag_A must be double.");
	}

	if (!mxIsDouble(prhs[2])) {
		mexErrMsgIdAndTxt("MATLAB:mexCrPhaseCoherent:double", "Input matrix Ph_B must be double.");
	}

	if (!mxIsDouble(prhs[3])) {
		mexErrMsgIdAndTxt("MATLAB:mexCrPhaseCoherent:double", "Input matrix Mag_B must be double.");
	}

	checkInputScalar(prhs, 4);

	/* construct Ph and Mag matrix from matlab arguments */
	MatrixF Ph_A(mat2cpp_float(prhs[0]));
	MatrixF Mag_A(mat2cpp_float(prhs[1]));

	MatrixF Ph_B(mat2cpp_float(prhs[2]));
	MatrixF Mag_B(mat2cpp_float(prhs[3]));

	msize_t N_bits = (msize_t) mxGetScalar(prhs[4]);


	MatrixCmpl H_est(Ph_A.rows(), Ph_A.cols(), MATRIX_UNINIT_DATA);
	MatrixF AngleOffset(Ph_A.rows(), Ph_A.cols(), MATRIX_UNINIT_DATA);

	reconstructPhaseCoherent(Ph_A, Mag_A, Ph_B, Mag_B, N_bits, H_est, AngleOffset);

	plhs[0] = cpp2mat_complex(H_est);
	plhs[1] = cpp2mat_float(AngleOffset);

}
