/***********************************************************************************/
/*!
 *  @brief      
 *  @file       matrix.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef MATRIX_H_
#define MATRIX_H_

#include <common/include/Types.h>
#include <common/include/memory/heap_allocator.h>

#include "Logging.h"
#ifndef __PROGRAM_START
#define __PROGRAM_START 1
#endif
#include "arm_math.h"

#include <algorithm>


#if RANGE_CHECK
#include <stdexcept>
#endif

#if defined(__SAM4E16E__)
#include "sam4.h"
#endif

namespace ranging {

#define MAX_ROW_COL_SIZE					256

#define USE_EXPLICIT_ARM_SPECIALISATIONS	1

/**
 * Defines clearing the data or not at Matrix creation
 */
enum matrix_cleartype {MATRIX_UNINIT_DATA, MATRIX_CLEAR_DATA};

template<class T>
class Matrix {


private:
	// constructors
	Matrix(); /* hide */

public:
	msize_t rows() const { return m_rows; }
	msize_t cols() const { return m_cols; }

public:
	T& operator()(msize_t i, msize_t j) {
#if RANGE_CHECK
		range_check(i, j);
#endif
		return m_elements[i * m_cols + j];
	}
	const T& operator()(msize_t i, msize_t j) const {
#if RANGE_CHECK
		range_check(i, j);
#endif
		return m_elements[i * m_cols + j];
	}
	T& operator()(msize_t n) {
#if RANGE_CHECK
		if (n >= numel() || n < 0)
			throw std::range_error("matrix access out of range");
#endif
		return m_elements[n];
	}
	const T& operator()(msize_t n) const {
#if RANGE_CHECK
		if (n >= numel() || n < 0)
			throw std::range_error("matrix access out of range");
#endif
		return m_elements[n];
	}

	const T& element(msize_t i, msize_t j) const {
#if RANGE_CHECK
		range_check(i, j);
#endif
		return m_elements[i * m_cols + j];
	}
	T& element(msize_t i, msize_t j) {
#if RANGE_CHECK
		range_check(i, j);
#endif
		return m_elements[i * m_cols + j];
	}
public:
	// constructors
	Matrix(msize_t rows, msize_t cols, const T* elements);
	Matrix(msize_t rows, msize_t cols, matrix_cleartype clear = MATRIX_CLEAR_DATA);
	Matrix(const Matrix<T>&);

	// destructor
	virtual ~Matrix();

	// assignment
	Matrix<T>& operator=(const Matrix<T>&);

	// comparison
	bool operator==(const Matrix<T>&) const;
	bool iszero() const;
	bool operator!() const {
		return iszero();
	}

	// support
	msize_t numel() const {
		return (m_rows * m_cols);
	}

	// data
	const T* data() const {
		return m_elements;
	}

	// iterator
	typedef T * iterator;
	typedef T const * const_iterator;

	iterator begin() {
		return &m_elements[0];
	}

	const_iterator begin() const {
		return &m_elements[0];
	}

	iterator end() {
		return &m_elements[m_cols * m_rows];
	}

	const_iterator end() const {
		return &m_elements[m_cols * m_rows];
	}

	// scalar multiplication/division
	Matrix<T>& operator*=(const T& a);
	Matrix<T> operator*(const T& a) const {
		return Matrix<T>(*this).operator*=(a);
	}

	Matrix<T>& operator/=(const T& a);
	Matrix<T> operator/(const T& a) {
		return Matrix<T>(*this).operator/=(a);
	}

	Matrix<T>& operator+=(const T& a);
	Matrix<T> operator+(const T& a) const {
		return Matrix<T>(*this).operator+=(a);
	}

	Matrix<T>& operator-=(const T& a);
	Matrix<T> operator-(const T& a) const {
		return Matrix<T>(*this).operator-=(a);
	}

	// addition/subtraction
	Matrix<T>& operator+=(const Matrix<T>&);
	Matrix<T> operator+(const Matrix<T>& M) const {
		return Matrix<T>(*this).operator+=(M);
	}

	Matrix<T>& operator-=(const Matrix<T>&);
	Matrix<T> operator-(const Matrix<T>& M) const {
		return Matrix<T>(*this).operator-=(M);
	}

	// matrix multiplication
	Matrix<T> operator*(const Matrix<T>&) const;
	Matrix<T>& operator*=(const Matrix<T>& M) {
		return *this = *this * M;
	}

	// these member functions are only valid for squares
	Matrix<T> & identity();
	bool isidentity() const;

	// Transpose
	Matrix<T> transpose() const;
	Matrix<T> ctranspose() const; // complex conjugate transpose

	/**
	 * Resize matrix to smaller sub matrix
	 * @param _rows
	 * @param _cols
	 * @return
	 */
	Matrix<T> & resize(const Span &_rows, const Span &_cols);

	/**
	 * Create a sub matrix out of this one
	 * @param _rows
	 * @param _cols
	 * @return
	 */
	Matrix<T> sub(const Span &_rows, const Span &_cols) const;

	/**
	 * Update part of matrix with new data from input matrix
	 * @param _rows
	 * @param _cols
	 * @param u
	 * @return
	 */
	Matrix<T> & update(const Span &_rows, const Span &_cols, const Matrix<T> & u);

	/**
	 * Get one row from the matrix
	 * @param n
	 * @return
	 */
	Matrix<T> getRow(const msize_t n) const;

	/**
	 * Set elements to 0
	 * @param _rows
	 * @param _cols
	 * @return
	 */
	Matrix<T> & zero(const Span &_rows, const Span &_cols);

	/**
	 * Set all elements to 0
	 */
	void zero();


private:
	/* arm FPU representation */
	arm_matrix_instance_f32 arm_matrix_f32_type() const;

#if RANGE_CHECK
	// range check function for matrix access
	void range_check(msize_t i, msize_t j) const;
#endif

	void rows(msize_t r) { m_rows = r; }

public:
	void cols(msize_t c) { m_cols = c; }

private:
	msize_t m_rows;  // number of rows
	msize_t m_cols;  // number of columns
	T * m_elements;  // array of elements

};

template<class T>
Matrix<T>::Matrix(msize_t _rows, msize_t _cols, const T* _data) :
		m_rows(_rows), m_cols(_cols) {
#if RANGE_CHECK
	if (m_rows == 0 || m_cols == 0)
		throw std::range_error("attempt to create a degenerate matrix");
#endif


	/* allocate & clear */
	MatrixAllocator<T> alloc;
	m_elements = alloc.allocate(_rows * _cols);

	// Initialize from array
	if (_data) {
		std::copy_n(_data, _rows * _cols, m_elements);
	} else {
		std::fill_n(m_elements, _rows * _cols, T());
	}
}

template<class T>
Matrix<T>::Matrix(msize_t _rows, msize_t _cols, matrix_cleartype clear) :
		m_rows(_rows), m_cols(_cols) {
#if RANGE_CHECK
	if (m_rows == 0 || m_cols == 0)
		throw std::range_error("attempt to create a degenerate matrix");
#endif


	/* allocate  */
	MatrixAllocator<T> alloc;
	m_elements = alloc.allocate(_rows * _cols);

	// clear when needed
	if (clear == MATRIX_CLEAR_DATA)
		std::fill_n(m_elements, _rows * _cols, T());
}

template<class T>
Matrix<T>::Matrix(const Matrix<T>& cp) :
		m_rows(cp.m_rows), m_cols(cp.m_cols), m_elements(0) {
	DPRINTF("Matrix copy Ctor (%d,%d)\n", m_rows, m_cols);

	MatrixAllocator<T> alloc;

	/* allocate & copy */
	m_elements = alloc.allocate(m_rows * m_cols);
	std::copy_n(cp.m_elements, m_rows * m_cols, m_elements);

}

template<class T>
Matrix<T>::~Matrix() {
	if (m_elements) {
		MatrixAllocator<T> alloc;
		alloc.deallocate(m_elements, m_rows * m_cols);
	}
}

template<class T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& cp) {
#if RANGE_CHECK
	if (cp.m_rows * cp.m_cols > m_rows * m_cols)
		throw std::domain_error("matrix operator= not of same order");
#endif

	DPRINTF("Matrix op= (%d,%d)\n", cp.rows(), cp.cols());

	/* do not allocate unless necessary */
	if (cp.rows() != m_rows || cp.cols() != m_cols)
	{
		MatrixAllocator<T> alloc;
		alloc.deallocate(m_elements, m_rows * m_cols);
		m_elements = alloc.allocate(cp.m_rows * cp.m_cols); // TODO replace with reallocate

		/* set new row & col value */
		m_cols = cp.m_cols;
		m_rows = cp.m_rows;
	}

	/* copy the data elements */
	std::copy_n(cp.m_elements, m_rows * m_cols, m_elements);

	return *this;
}

#if RANGE_CHECK
template<class T>
void Matrix<T>::range_check(msize_t i, msize_t j) const {
	if (m_rows <= i)
		throw std::range_error("matrix access row out of range");
	if (m_cols <= j)
		throw std::range_error("matrix access col out of range");
	if (i < 0 || j < 0)
		throw std::range_error("matrix access out of range");
}
#endif

template<class T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& B) const {
#if RANGE_CHECK
	if (this->m_cols != B.rows())
		throw std::domain_error("matrix operator* Matrices cannot be multiplied (invalid size)");
#endif
	Matrix<T> C(this->m_rows, B.m_cols, MATRIX_UNINIT_DATA);

	for (msize_t i = 0; i < C.m_rows; i++) {
		for (msize_t j = 0; j < C.m_cols; j++) {
			T sum = T(0.0f);
			for (msize_t k = 0; k < this->m_cols; k++) {
				sum += element(i, k) * B(k, j);
			}
			C(i, j) = sum;
		}
	}
	return C;
}

template<class T>
Matrix<T> & Matrix<T>::operator*=(const T & scalar) {
	/* iterate over all elements */
	for (iterator it = begin(); it < end(); it++) {
		*it *= scalar;
	}

	return *this;
}

template<class T>
Matrix<T> & Matrix<T>::operator/=(const T & scalar) {
	/* iterate over all elements */
	for (iterator it = begin(); it < end(); it++) {
		*it /= scalar;
	}

	return *this;
}

template<class T>
Matrix<T> & Matrix<T>::operator+=(const T & scalar) {
	/* iterate over all elements */
	for (iterator it = begin(); it < end(); it++) {
		*it += scalar;
	}

	return *this;
}


template<class T>
Matrix<T> & Matrix<T>::operator-=(const T & scalar) {
	/* iterate over all elements */
	for (iterator it = begin(); it < end(); it++) {
		*it -= scalar;
	}

	return *this;
}

template<class T>
Matrix<T> & Matrix<T>::operator-=(const Matrix<T>& B) {
#if RANGE_CHECK
	if ((this->m_rows != B.m_rows) || (this->m_cols != B.m_cols))
		throw std::range_error("operator-= out of range");
#endif

	/* set the initial value of the iterators */
	iterator it = begin();
	const_iterator itB = B.begin();

	/* iterate over all elements */
	for (; it < end(); it++, itB++) {
		*it -= *itB;
	}

	return *this;
}

template<class T>
Matrix<T> & Matrix<T>::operator+=(const Matrix<T>& B) {
#if RANGE_CHECK
	if ((this->m_rows != B.m_rows) || (this->m_cols != B.m_cols))
		throw std::range_error("operator+= out of range");
#endif

	/* set the initial value of the iterators */
	iterator it = begin();
	const_iterator itB = B.begin();

	/* iterate over all elements */
	for (; it < end(); it++, itB++) {
		*it += *itB;
	}

	return *this;
}

template<class T>
bool Matrix<T>::operator==(const Matrix<T>& B) const {
	const_iterator itB = B.begin();

	/* iterate over all elements and compare */
	for (const_iterator itA = begin(); itA < end(); itA++, itB++) {
		if (*itA != *itB)
			return false;
	}

	return true;
}

template<class T>
bool Matrix<T>::iszero() const {
	/* iterate over all elements */
	for (const_iterator it = begin(); it < end(); it++) {
		if (*it != T(0.0f))
			return false;
	}
	return true;
}

template<class T>
void Matrix<T>::zero() {
	/* iterate over all elements */
	for (iterator it = begin(); it < end(); it++) {
		*it = T(0.0f);
	}
}

template<class T>
bool Matrix<T>::isidentity() const {
	if (m_cols != m_rows)
		return false;

	for (msize_t i = 0; i < m_rows; i++) {
		for (msize_t j = 0; j < m_cols; j++) {
			if (i == j) {
				if (element(i, j) != T(1.0f))
					return false;
			} else {
				if (element(i, j) != T(0.0f))
					return false;
			}
		}
	}
	return true;
}

/**
 * Make this matrix an Identity matrix (all 0, but 1 on diagonal)
 * @return
 */
template<class T>
Matrix<T> & Matrix<T>::identity() {
#if RANGE_CHECK
	if (m_rows != m_cols)
		throw std::domain_error("matrix not square");
#endif

	for (msize_t i = 0; i < m_rows; i++) {
		for (msize_t j = 0; j < m_cols; j++) {
			if (i == j) {
				element(i, j) = T(1.0f);
			} else {
				element(i, j) = T(0.0f);
			}
		}
	}
	return *this;
}

/* Explicit float specialization */
template<>
inline arm_matrix_instance_f32 Matrix<float>::arm_matrix_f32_type() const {
	arm_matrix_instance_f32 arm_repr;
	arm_repr.numCols = m_cols;
	arm_repr.numRows = m_rows;
	arm_repr.pData = const_cast<float32_t *>(data());
	return arm_repr;
}

/* Explicit float specialization */
template<>
inline arm_matrix_instance_f32 Matrix<ComplexF>::arm_matrix_f32_type() const {
	arm_matrix_instance_f32 arm_repr;
	arm_repr.numCols = m_cols;
	arm_repr.numRows = m_rows;

	ComplexF * cp = const_cast<ComplexF*>(data());
	arm_repr.pData = reinterpret_cast<float*>(cp);

	return arm_repr;
}

#if USE_EXPLICIT_ARM_SPECIALISATIONS
/* Explicit float specialization */
template<>
inline Matrix<float>& Matrix<float>::operator+=(const Matrix<float>& B) {
	const arm_matrix_instance_f32 mA = this->arm_matrix_f32_type();
	const arm_matrix_instance_f32 mB = B.arm_matrix_f32_type();
	arm_matrix_instance_f32 dest = this->arm_matrix_f32_type();

#if RANGE_CHECK
	arm_status ret =
#endif
	arm_mat_add_f32(&mA, &mB, &dest);

#if RANGE_CHECK
	if (ret != ARM_MATH_SUCCESS)
		throw std::domain_error("arm matrix operation");
#endif

	return *this;
}

/* Explicit float specialization */
template<>
inline Matrix<float>& Matrix<float>::operator-=(const Matrix<float>& B) {
	const arm_matrix_instance_f32 mA = this->arm_matrix_f32_type();
	const arm_matrix_instance_f32 mB = B.arm_matrix_f32_type();
	arm_matrix_instance_f32 dest = this->arm_matrix_f32_type();

#if RANGE_CHECK
	arm_status ret =
#endif
	arm_mat_sub_f32(&mA, &mB, &dest);

#if RANGE_CHECK
	if (ret != ARM_MATH_SUCCESS)
		throw std::domain_error("arm matrix operation");
#endif

	return *this;
}

/* Explicit float specialization */
template<>
inline Matrix<float> Matrix<float>::operator*(const Matrix<float>& B) const {
	Matrix<float> C(this->m_rows, B.m_cols, MATRIX_UNINIT_DATA);

	const arm_matrix_instance_f32 mA = this->arm_matrix_f32_type();
	const arm_matrix_instance_f32 mB = B.arm_matrix_f32_type();
	arm_matrix_instance_f32 dest = C.arm_matrix_f32_type();

#if RANGE_CHECK
	arm_status ret =
#endif
	arm_mat_mult_f32(&mA, &mB, &dest);

#if RANGE_CHECK
	if (ret != ARM_MATH_SUCCESS)
		throw std::domain_error("arm matrix operation");
#endif

	return C;
}

/* Explicit float specialization */
template<>
inline Matrix<ComplexF> Matrix<ComplexF>::operator*(const Matrix<ComplexF>& B) const {
#if RANGE_CHECK
	if ((this->m_rows > MAX_ROW_COL_SIZE) || (this->m_cols > MAX_ROW_COL_SIZE) || (B.m_rows > MAX_ROW_COL_SIZE) || (B.m_cols > MAX_ROW_COL_SIZE))
		throw std::range_error("complex matrix multiplication out of range");
#endif

	Matrix<ComplexF> Dest(this->m_rows, B.m_cols, MATRIX_UNINIT_DATA);

	const arm_matrix_instance_f32 mA = this->arm_matrix_f32_type();
	const arm_matrix_instance_f32 mB = B.arm_matrix_f32_type();
	arm_matrix_instance_f32 dest = Dest.arm_matrix_f32_type();

#if RANGE_CHECK
	arm_status ret =
#endif
	arm_mat_cmplx_mult_f32(&mA, &mB, &dest);

#if RANGE_CHECK
	if (ret != ARM_MATH_SUCCESS)
		throw std::domain_error("arm matrix operation");
#endif

	return Dest;
}
#endif

/* Explicit float specialization */
template<>
inline Matrix<float> Matrix<float>::transpose() const {
	Matrix<float> result(m_cols, m_rows, MATRIX_UNINIT_DATA); /* switched rows & cols */

	const arm_matrix_instance_f32 mA = this->arm_matrix_f32_type();
	arm_matrix_instance_f32 dest = result.arm_matrix_f32_type();

#if RANGE_CHECK
	arm_status ret =
#endif
	arm_mat_trans_f32(&mA, &dest);

#if RANGE_CHECK
	if (ret != ARM_MATH_SUCCESS)
		throw std::domain_error("arm matrix operation");
#endif

	return result;
}

/* Explicit complex float specialization */
template<>
inline Matrix<ComplexF> Matrix<ComplexF>::transpose() const {
	/* copy matrix, uninitialized data */
	Matrix<ComplexF> C(m_cols, m_rows, MATRIX_UNINIT_DATA);

	for (msize_t i = 0; i < m_rows; i++) {
		for (msize_t j = 0; j < m_cols; j++) {
			ComplexF v = element(i, j);
			C(j, i) = v;
		}
	}

	return C;
}

/* Explicit complex float specialization */
template<>
inline Matrix<ComplexF> Matrix<ComplexF>::ctranspose() const {
	/* copy matrix, uninitialized data */
	Matrix<ComplexF> C(m_cols, m_rows, MATRIX_UNINIT_DATA);

	for (msize_t i = 0; i < m_rows; i++) {
		for (msize_t j = 0; j < m_cols; j++) {
			ComplexF v = element(i, j);
			v.imag(-v.imag()); /* change imag sign */
			C(j, i) = v;
		}
	}

	return C;
}

//not shown:
//	minor(msize_t i, msize_t j) const,
//	minor_det(msize_t i, msize_t j) const,
//	det() const -- mb
//
//	inverse() const,
//	getrow(msize_t i) const,
//	getcol(msize_t j) const,
//	delcol(msize_t j) const,
//	delrow(msize_t i) const,
//	setcol(msize_t j, const matrix<T>& C)
//	setrow(msize_t i, const matrix<T>& R), identity() const,
//	pow(int exp) const,
//	pow(const matrix<T>& M, int exp)




template<class T>
Matrix<T> & Matrix<T>::resize(const Span &_rows, const Span &_cols) {
#if RANGE_CHECK
	if ((_rows.to() > m_rows) || (_cols.to() > m_cols))
		throw std::range_error("resize() range error");
#endif

	msize_t new_rows = _rows.to() - _rows.from() + 1;
	msize_t new_cols = _cols.to() - _cols.from() + 1;

	Matrix<T> N(new_rows, new_cols, MATRIX_UNINIT_DATA);

	for (msize_t i = 0; i < new_rows; i++) {
		for (msize_t j = 0; j < new_cols; j++) {
			N(i, j) = element(i + _rows.from(), j + _cols.from());
		}
	}

	/* delete the current data store */
	if (m_elements) {
		MatrixAllocator<T> alloc;
		alloc.deallocate(m_elements, m_rows * m_cols);
	}

	/* link the N data to this */
	m_elements = N.m_elements;

	m_rows = N.m_rows;
	m_cols = N.m_cols;

	/* set N data store to NULL */
	N.m_elements = (T *)0;

	return *this;
}

template<class T>
Matrix<T> Matrix<T>::sub(const Span &_rows, const Span &_cols) const {
#if RANGE_CHECK
	if ((_rows.to() > m_rows) ||( _cols.to() > m_cols))
		throw std::range_error("sub() range error");
#endif

	msize_t new_rows = _rows.to() - _rows.from() + 1;
	msize_t new_cols = _cols.to() - _cols.from() + 1;

	Matrix<T> N(new_rows, new_cols, MATRIX_UNINIT_DATA);

	for (msize_t i = 0; i < new_rows; i++) {
		for (msize_t j = 0; j < new_cols; j++) {
			N(i, j) = element(i + _rows.from(), j + _cols.from());
		}
	}

	return N;
}

template<class T>
Matrix<T> Matrix<T>::getRow(const msize_t n) const {
#if RANGE_CHECK
	if (n >= m_rows)
		throw std::range_error("getRow() out of range");
#endif

	msize_t new_rows = 1;
	msize_t new_cols = m_cols;

	Matrix<T> M(new_rows, new_cols, MATRIX_UNINIT_DATA);

	for (msize_t i = 0; i < new_cols; i++) {
		M(0, i) = element(n, i);
	}
	return M;
}

template<class T>
Matrix<T> & Matrix<T>::update(const Span &_rows, const Span &_cols, const Matrix<T> & u) {
#if RANGE_CHECK
	if (u.m_rows != (_rows.to() - _rows.from() + 1))
		throw std::range_error("update() range error");
	if (u.m_cols != (_cols.to() - _cols.from() + 1))
		throw std::range_error("update() range error");
#endif

	for (msize_t i = 0; i < u.m_rows; i++) {
		for (msize_t j = 0; j < u.m_cols; j++) {
			element(i + _rows.from(), j + _cols.from()) = u(i, j);
		}
	}
	return *this;
}

template<class T>
Matrix<T> & Matrix<T>::zero(const Span &_rows, const Span &_cols) {
	msize_t u_rows = (_rows.to() - _rows.from() + 1);
	msize_t u_cols = (_cols.to() - _cols.from() + 1);

#if RANGE_CHECK
	if (u_rows <= 0 || u_rows > m_rows)
		throw std::range_error("zero() range error");
	if (u_cols <= 0 || u_cols > m_cols)
		throw std::range_error("zero() range error");
#endif

	for (msize_t i = 0; i < u_rows; i++) {
		for (msize_t j = 0; j < u_cols; j++) {
			element(i + _rows.from(), j + _cols.from()) = 0.0f;
		}
	}
	return *this;
}

/* --------- Derived class template  --------------- */
template<class T>
class DerivedMatrix: public Matrix<T> {

public:
	// constructors

	DerivedMatrix(msize_t _rows, msize_t _cols, matrix_cleartype clear = MATRIX_CLEAR_DATA) :
			Matrix<T>(_rows, _cols, clear) {

	}

	// destructor
	virtual ~DerivedMatrix() {
	}

};


/* --------- TYPE definitions --------------- */

typedef Matrix<float> MatrixF;
typedef Matrix< std::complex<float> > MatrixCmpl;

}  // namespace ranging

#endif /* MATRIX_H_ */
