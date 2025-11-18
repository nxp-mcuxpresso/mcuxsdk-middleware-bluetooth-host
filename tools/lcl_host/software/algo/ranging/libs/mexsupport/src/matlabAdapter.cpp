/***********************************************************************************/
/*!
 *  @brief
 *  @file       matlabAdapter.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note
 */
/***********************************************************************************/

#include <iostream>
#include <common/include/Functions.h>
#include <mexsupport/include/matlabAdapter.h>


mxArray* cpp2mat_complex(const MatrixCmpl & M) {

	msize_t rows = M.rows();
	msize_t cols = M.cols();

	mxArray * X = mxCreateDoubleMatrix(rows, cols, mxCOMPLEX);

	/* pointers to real and imag part */
	double * xreal = mxGetPr(X);
	double * ximag = mxGetPi(X);

	for (msize_t j = 0; j < cols; j++) {
		for (msize_t i = 0; i < rows; i++) {
			*xreal++ = static_cast<double>(M(i, j).real());
			*ximag++ = static_cast<double>(M(i, j).imag());
		}
	}
	return (X);
}

MatrixCmpl mat2cpp_complex(const mxArray *marray) {

	//mexPrintf("marray rows=%d\n", mxGetM(marray));
	//mexPrintf("marray cols=%d\n", mxGetN(marray));

	msize_t rows = mxGetM(marray);
	msize_t cols = mxGetN(marray);

	MatrixCmpl M(rows, cols);

	/* pointers to real and imag part */
	double * xreal = mxGetPr(marray);
	double * ximag = mxGetPi(marray);

	if (ximag == NULL)
		mexErrMsgIdAndTxt("MATLAB:mat2cpp_complex:complex", "Matrix is not complex.");

	/* matlab organizes data  as a long array built from cols of data */
	for (msize_t j = 0; j < cols; j++) {
		for (msize_t i = 0; i < rows; i++) {
			float real = static_cast<float>(*xreal++);
			float imag = static_cast<float>(*ximag++);
			ComplexF val(real, imag);
			M(i, j) = val;
		}
	}

	return M;
}

mxArray* cpp2mat_float(const MatrixF & M) {

	msize_t rows = M.rows();
	msize_t cols = M.cols();

	mxArray * X = mxCreateDoubleMatrix(rows, cols, mxREAL);

	/* pointer to data space */
	double * xptr = mxGetPr(X);

	for (msize_t j = 0; j < cols; j++) {
		for (msize_t i = 0; i < rows; i++) {
			*xptr++ = static_cast<double>(M(i, j));
		}
	}
	return X;
}

MatrixF mat2cpp_float(const mxArray *marray) {

	msize_t rows = mxGetM(marray);
	msize_t cols = mxGetN(marray);

	MatrixF M(rows, cols);

	/* pointer to data space */
	double * xptr = mxGetPr(marray);

	/* matlab organizes data  as a long array built from cols of data */
	for (msize_t j = 0; j < cols; j++) {
		for (msize_t i = 0; i < rows; i++) {
			float val = static_cast<float>(*xptr++);
			M(i, j) = val;
		}
	}

	return M;
}

mxArray* cpp2mat_integer(const Matrix<int> & M) {

	msize_t rows = M.rows();
	msize_t cols = M.cols();

	mxArray * X = mxCreateDoubleMatrix(rows, cols, mxREAL);

	/* pointer to data space */
	double * xptr = mxGetPr(X);

	for (msize_t j = 0; j < cols; j++) {
		for (msize_t i = 0; i < rows; i++) {
			*xptr++ = static_cast<double>(M(i, j));
		}
	}
	return X;
}


// end
