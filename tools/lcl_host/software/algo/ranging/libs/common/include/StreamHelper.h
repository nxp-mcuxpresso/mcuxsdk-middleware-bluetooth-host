/***********************************************************************************/
/*!
 *  @brief      
 *  @file       StreamHelper.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef STREAMHELPER_H_
#define STREAMHELPER_H_

#if defined(__unix__) // || defined(_WIN32)

#include <common/include/Stream.h>

using namespace ranging;

/**
 * Helper class to print Channel Response in CSV format
 *
 * example: std::cout << StreamChannelResponseCsv(H_est) << std::endl;
 *
 */
class StreamChannelResponseCsv {
public:
	StreamChannelResponseCsv(const MatrixCmpl &H_est) : _M(H_est) { 	}
	friend std::ostream& operator<<(std::ostream &o, const StreamChannelResponseCsv &obj) {
		o << "CHANNEL RESPONSE in csv format" << std::endl;
		for (msize_t i = 0; i < obj._M.rows(); i++) {
			if (i != 0)
				o << std::endl;
			for (msize_t j = 0; j < obj._M.cols(); j++) {
				if (j != 0)
					o << ", ";
				ComplexF f(obj._M(i, j));
				float p = 0;
				if (f != ComplexF(0,0)) p = 20 * std::log10(std::norm(f));
				o << p;
			}
		}
		o << std::endl;
		return o;
	}
private:
	const MatrixCmpl &_M;
};

/**
 * Helper class to print IQ data in VNA CSV format
 *
 * example: std::cout << StreamComplexPairCsv(IQa, IQb) << std::endl;
 *
 */
class StreamComplexPairCsv {
public:
	StreamComplexPairCsv(const MatrixCmpl &inA, const MatrixCmpl &inB) :
			_A(inA), _B(inB) {
		/* empty */
	}
	friend std::ostream& operator<<(std::ostream &o, const StreamComplexPairCsv &obj) {
		//o << in._A.cols() << std::endl;
		for (msize_t i = 0; i < obj._A.rows(); i++) {
			for (msize_t j = 0; j < obj._A.cols(); j++) {
				o << obj._A(i, j).real() << ", " << obj._A(i, j).imag() << ", ";
				o << obj._B(i, j).real() << ", " << obj._B(i, j).imag() << std::endl;
			}
		}
		return o;
	}
private:
	const MatrixCmpl &_A;
	const MatrixCmpl &_B;
};


/**
 * Helper class to print Channel Response in C code format
 *
 * example:
 *    std::cout << StreamChannelResponseCode(H_est, "H_est") << std::endl;
 *
 *
 */
class StreamChannelResponseCode {
public:
	StreamChannelResponseCode(const MatrixCmpl &H_est, std::string name = "") :
			_M(H_est), _s(name) {
	}
	friend std::ostream& operator<<(std::ostream &o, const StreamChannelResponseCode &obj) {

		std::ostringstream ss;
		ss << std::setprecision(32);

		std::string name(obj._s);
		if (name.empty()) name = "variable";

		std::string tstring = type_name<ComplexF>();
		if (tstring == "float") tstring = "";

		ss << type_name<ComplexF>() << " _" << name << "[] = {" << std::endl;
		for (msize_t i = 0; i < obj._M.rows(); i++) {
			ss << "\t";
			for (msize_t j = 0; j < obj._M.cols(); j++) {
				ss << tstring << obj._M(i, j) << ", ";
			}
			ss << std::endl;
		}
		ss << "};" << std::endl;

		ss << "Matrix<" << type_name<ComplexF>() << "> " << name << "(" << obj._M.rows() << ", " << obj._M.cols() << ", _" << name << ");" << std::endl;

		o << ss.str();
		return o;
	}
private:
	const MatrixCmpl &_M;
	const std::string &_s;
};

#endif /* unix */

#endif /* STREAMHELPER_H_ */
