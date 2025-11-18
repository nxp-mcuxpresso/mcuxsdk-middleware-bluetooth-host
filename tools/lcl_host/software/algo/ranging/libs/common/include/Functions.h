/***********************************************************************************/
/*!
 *  @brief      
 *  @file       Functions.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <common/include/Matrix.h>

#include "Logging.h"

#include <math.h>
#include <algorithm>

#if RANGE_CHECK
#include <stdexcept>
#endif

namespace ranging {

template<typename T>
inline T hankel(const T & m) {
	TRACE();

#if RANGE_CHECK
	if (m.rows() > 1)
		throw std::range_error("cannot hankel matrix from more than 1 row");
#endif

	T h(m.cols(), m.cols());

	for (msize_t i = 0; i < m.cols(); i++) {
		for (msize_t j = 0; j <= i; j++) {
			h(m.cols() - i - 1, j) = m(0, j + m.cols() - i - 1);
		}
	}
	return h;
}

template<typename T>
inline msize_t size(const T & m, const msize_t index) {
	TRACE();
	if (index == 0) {
		return m.rows();
	} else {
		return m.cols();
	}
}

template<typename T>
inline T size(const T & m) {
	TRACE();
	T ret(1, 2);
	ret(0, 0) = m.rows();
	ret(0, 1) = m.cols();
	return ret;
}

/**
 * Calculate the diagonal
 * Return matrix
 * @param m
 * @return
 */
template<typename T>
inline Matrix<T> diag(const Matrix<T> & m, const msize_t offset = 0) {
	TRACE();

	msize_t n = std::min(m.rows(), m.cols()) - offset;

#if RANGE_CHECK
	if (n == 0)
		throw std::range_error("diag() range error");
#endif

	Matrix<T> r(1, n, MATRIX_UNINIT_DATA);

	for (msize_t i = 0; i < n; i++) {
		r(0, i) = m(i, i + offset);
	}
	return r;
}

/**
 * Calculate the diagonal
 * Return matrix
 * @param m
 * @return
 */
template<typename T>
inline Matrix<T> addColumn(const Matrix<T> & M) {
	TRACE();

	Matrix<T> R(M.rows(), M.cols() + 1);

	for (msize_t i = 0; i < M.rows(); i++) {
		for (msize_t j = 0; j < M.cols(); j++) {
			R(i, j + 1) = M(i, j);
		}
	}
	return R;
}

/**
 * Create a squared matrix v * v.transposed()
 * Return matrix
 * @param m
 * @return
 */
inline MatrixF squared(const MatrixF & M) {
	TRACE();

#if RANGE_CHECK
	if (M.cols() != 1)
		throw std::range_error("squared() range error");
#endif

	MatrixF R(M.rows(), M.rows());

	for (msize_t i = 0; i < M.rows(); i++) {
		for (msize_t j = 0; j < M.rows(); j++) {
			R(i, j) = M(i, 0) * M(j, 0);
		}
	}
	return R;
}

inline MatrixCmpl squared(const MatrixCmpl & M) {
	TRACE();

#if RANGE_CHECK
	if (M.cols() != 1)
		throw std::range_error("squared() range error");
#endif

	MatrixCmpl R(M.rows(), M.rows());

	for (msize_t i = 0; i < M.rows(); i++) {
		for (msize_t j = 0; j < M.rows(); j++) {
			ComplexF cf = M(j, 0);
			cf.imag(-cf.imag());
			R(i, j) = M(i, 0) * cf;
		}
	}
	return R;
}


/**
 * Calculate length of matrix
 * Return matrix
 * @param m
 * @return
 */
template<typename T>
inline msize_t length(const Matrix<T> & M) {
	TRACE();
	return M.cols();
}

/**
 * Reorder columns in matrix a based on the indices in the idx array
 * @param a
 * @param idx
 * @return
 */
template<typename T>
inline Matrix<T> reorder(const Matrix<T> &A, const Matrix<msize_t> &idx) {
	TRACE();

#if RANGE_CHECK
	if ((idx.rows() != 1) || (A.cols() != idx.cols()))
		throw std::range_error("reorder index error");
#endif

	/* create new output matrix of same size */
	Matrix<T> V(A.rows(), A.cols(), MATRIX_UNINIT_DATA);

	for (msize_t col = 0; col < idx.cols(); col++) {
		msize_t fromcol = idx(col);
		for (msize_t row = 0; row < V.rows(); row++) {
			V(row, col) = A(row, fromcol);
		}
	}

	return V;
}

/**
 * Array based transpose. Does __NOT__ involve conjugate for complex
 * See matal .' operator
 * @param a
 * @param idx
 * @return
 */

MatrixCmpl array_transpose(const MatrixCmpl &A);

/**
 * complex conjugate
 * https://en.wikipedia.org/wiki/Complex_conjugate
 * @param v
 * @return
 */
inline ComplexF conjugate(const ComplexF & v) {
	TRACE();
	return std::conj(v);
}


/**
 * Calculate the norm of a complex number
 * @param m
 * @return
 */
inline float norm(const ComplexF & v) {
	//float n = sqrtf(v.real() * v.real() + v.imag() * v.imag());
	return std::abs(v);
}

/**
 * Calculate pythag
 * @param m
 * @return
 */
inline float pythag(const float a, const float b) {
	TRACE();
	/* Computes (a^2 + b^2 )^1/2 without destructive under flow or over flow. */
	float absa = std::abs(a);
	float absb = std::abs(b);

	if (absa == 0.0f)
		return absb;
	if (absb == 0.0f)
		return absa;

	float py = 0.0f;

	if (absa > absb) {
		py = absa * sqrtf(1.0f + (absb / absa) * (absb / absa));
	} else {
		py = absb * sqrtf(1.0f + (absa / absb) * (absa / absb));
	}
	return py;
}

/**
 * C = SIGNTRANSF(A1,A2) performs sign transfer: if A2 is negative the result is -abs(A1),
 * if A2 is zero or positive the result is abs(A1).
 * @param a1
 * @param a2
 * @return
 */
inline float signtransf(const float a1, const float a2) {
	float val = std::abs(a1);
	if (a2 < 0.0f)
		val = -val;
	return val;
}



/**
 * Return m x m Identity matrix
 * @param m
 * @return
 */
MatrixF eye(const msize_t m);

/**
 * Return m x m Identity matrix
 * @param m
 * @return
 */
MatrixCmpl eyeComplex(const msize_t m);

/**
 * Calculate the norm of a complex matrix
 * @param m
 * @return
 */
float norm(const MatrixCmpl & M);


/**
 * Calculate sum over real parts of the diagonal
 * @param M
 * @return
 */
float trace(const MatrixCmpl &M);

/**
 * Create a copy with only real values from the complex input matrix
 * @param m
 * @return
 */
MatrixF real(const MatrixCmpl & M);

/**
 * Create angle of complex number
 * https://en.wikipedia.org/wiki/Atan2
 * @param f
 * @return
 */
inline float angle(const ComplexF & f) {
	return std::arg(f);
}

/**
 * https://nl.wikipedia.org/wiki/Formule_van_Euler
 * @param angle
 * @return
 */
inline ComplexF expCmpl(const float angle) {
	 return std::polar(1.0f, angle);
}


}  // namespace ranging

#endif /* FUNCTIONS_H_ */
