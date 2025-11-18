/***********************************************************************************/
/*!
 *  @brief      
 *  @file       Functions.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#include <common/include/Functions.h>

namespace ranging {

/**
 * Return m x m Identity matrix
 * @param m
 * @return
 */
MatrixF eye(const msize_t m) {
	TRACE();

#if RANGE_CHECK
	if (m == 0 || m > MAX_ROW_COL_SIZE)
		throw std::range_error("cannot create matrix of this size");
#endif

	MatrixF e(m, m, MATRIX_UNINIT_DATA);
	e.identity();
	return e;
}

/**
 * Return m x m Identity matrix
 * @param m
 * @return
 */
MatrixCmpl eyeComplex(const msize_t m) {
	TRACE();

#if RANGE_CHECK
	if (m == 0 || m > MAX_ROW_COL_SIZE)
		throw std::range_error("cannot create matrix of this size");
#endif

	MatrixCmpl e(m, m, MATRIX_UNINIT_DATA);
	e.identity();
	return e;
}

/**
 * Calculate the norm of a complex matrix
 * @param m
 * @return
 */
float norm(const MatrixCmpl & M) {
	TRACE();

	float result = 0.0f;

#if 0
	for (msize_t i = 0; i < M.rows(); i++) {
		for (msize_t j = 0; j < M.cols(); j++) {
			ComplexF c = M(i, j);
			result += c.real() * c.real() + c.imag() * c.imag();
		}
	}
#else
	for (msize_t i = 0; i < M.rows(); i++) {
		for (msize_t j = 0; j < M.cols(); j++) {
			result += std::norm(M(i,j));
		}
	}
#endif

	result = sqrtf(result);

	return result;
}

/**
 * Calculate sum over real parts of the diagonal
 * @param M
 * @return
 */
float trace(const MatrixCmpl &M) {
	TRACE();

	float sum = 0.0f;
	msize_t m = std::min(M.rows(), M.cols());

	for (msize_t i = 0; i < m; i++)
		sum += M(i, i).real();

	return sum;
}

/**
 * Create a copy with only real values from the complex input matrix
 * @param m
 * @return
 */
//MatrixF real(const MatrixCmpl & M) {
//	TRACE();
//
//	MatrixF r(M.rows(), M.cols(), MATRIX_UNINIT_DATA);
//
//	for (msize_t i = 0; i < M.rows(); i++) {
//		for (msize_t j = 0; j < M.cols(); j++) {
//			r(i, j) = M(i, j).real();
//		}
//	}
//	return r;
//}

/**
 * Create angle of complex number
 * https://en.wikipedia.org/wiki/Atan2
 * @param f
 * @return
 */
//float angle(const ComplexF & f) {
//	TRACE();
//
//	float y = f.imag();
//	float x = f.real();
//
//	if (x > 0)
//		return atan(y / x);
//	if (x < 0 && y >= 0)
//		return (atan(y / x) + PI);
//	if (x < 0 && y < 0)
//		return (atan(y / x) - PI);
//	if (x == 0 && y > 0)
//		return (PI / 2);
//	if (x == 0 && y < 0)
//		return (-PI / 2);
//
//#if RANGE_CHECK
//	throw std::range_error("angle: undefined");
//#endif
//
//	return 0;
//}

MatrixCmpl array_transpose(const MatrixCmpl &A) {
	TRACE();

	/* create new output matrix of 'transposed' size */
	MatrixCmpl C(A.cols(), A.rows(), MATRIX_UNINIT_DATA);

	for (msize_t i = 0; i < A.rows(); i++) {
		for (msize_t j = 0; j < A.cols(); j++) {
			ComplexF v = A(i, j);
			C(j, i) = v;
		}
	}

	return C;
}

}  // namespace ranging
