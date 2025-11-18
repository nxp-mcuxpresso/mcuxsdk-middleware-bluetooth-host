/***********************************************************************************/
/*!
 *  @brief      
 *  @file       Stream.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef STREAM_H_
#define STREAM_H_

#ifdef _WIN32
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
#endif

#if defined(__unix__) || defined(_WIN32)

#include <common/include/Matrix.h>
#include <common/include/Functions.h>

using namespace ranging;

#include <iostream>
#include <iomanip>
#include <stdlib.h>

#define SHOW_ROW_NUMBERS                   1      /* 1 = on, 0 = off */

#if defined(__unix__)
#include <cxxabi.h>
#endif

/**
 * Get string of type name
 * example in a template: type_name<T>()
 * @return
 */
template<typename T>
std::string type_name() {
	std::string tname = typeid(T).name();
#if defined(__unix__)
	int status;
	char *demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
	if (status == 0) {
		tname = demangled_name;
		::free(demangled_name);
	}
#endif
	return tname;
}


template<class T>
inline std::ostream& operator<<(std::ostream& o, const Matrix<T>& M) {
	o << M.rows() << "x" << M.cols() << " " << type_name<T>() << std::endl;
	for (msize_t i = 0; i < M.rows(); i++) {
		o << "\t";
#if SHOW_ROW_NUMBERS
		o << "row(" << i << ") ";
#endif
		o << "[ ";
		for (msize_t j = 0; j < M.cols(); j++)
			o << M(i, j) << " ";
		o << "]" << std::endl;
	}
	return o;
}

/* Specialization for MatrixCmpl and Matlab Compatible output */
inline std::ostream& operator<<(std::ostream& o, const MatrixCmpl& M) {
	o << M.rows() << "x" << M.cols() << " " << "std::complex<float>" << std::endl;
	o << "[ ";
	for (msize_t i = 0; i < M.rows(); i++) {
		if (i !=0) o << ";" << std::endl;
		o << "  ";
		for (msize_t j = 0; j < M.cols(); j++) {
			if (j !=0) o << ", ";
			o << "complex"<< M(i, j);
		}
	}
	o << " ];" << std::endl;
	return o;
}

template<>
inline std::ostream& operator<<(std::ostream& o, const Matrix<std::pair<float,int> >& M) {
	o << M.rows() << "x" << M.cols() << " std::pair<float,int> " <<  std::endl;
	for (msize_t i = 0; i < M.rows(); i++) {
		o << "\t[ ";
		for (msize_t j = 0; j < M.cols(); j++)
			o << "(" << M(i, j).first << "," << M(i, j).second << ") ";
		o << "]";
	}
	return o;
}

inline std::ostream& operator<<(std::ostream& o, const Span& R) {
	o << " range[" << R.from() << "," << R.to() << "]" << std::endl;
	return o;
}


/**
 * example: 	std::cout <<  generateInitializer( std::complex<float>(V), "V") ;
 *
 * @param M
 * @param name
 * @return
 */
template<typename T>
inline std::string generateInitializer(const Matrix<T>& M, std::string name = "") {
	std::ostringstream ss;
	ss << std::setprecision(32);

	if (name.empty()) name = "ref";

	ss << "static const float TEST_EPSILON = 1.0e-4;" << std::endl;

	std::string tstring = type_name<T>();
	if (tstring == "float") tstring = "";

	ss << type_name<T>() << " _" << name << "ref[] = { ";
	for (msize_t i = 0; i < M.rows(); i++) {
		for (msize_t j = 0; j < M.cols(); j++) {
			ss << tstring << M(i, j) << ", ";
		}
	}
	ss << "};" << std::endl;
	ss << "Matrix<" << type_name<T>() << "> " << name << "ref(" << M.rows() << ", " << M.cols() << ", _" << name << "ref);" << std::endl;

	ss << "TEST_ASSERT_TRUE(test_cmp(" << name << ", " << name << "ref, TEST_EPSILON));" << std::endl;

	return ss.str();
}

#endif /* unix */

#endif /* STREAM_H_ */
