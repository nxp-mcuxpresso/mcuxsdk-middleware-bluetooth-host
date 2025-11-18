/***********************************************************************************/
/*!
 *  @brief      
 *  @file       RTypes.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef RTYPES_H_
#define RTYPES_H_

#include <complex>

#if RANGE_CHECK
#include <stdexcept>
#endif

namespace ranging {

/* type definitions */
typedef int msize_t;
using ComplexF = std::complex<float>;


struct evd_tol_s {
	float maxeig; /* also TOL1 */
	float sumeig; /* also TOL2 */
};

/* -------------------------------------------------------- */

/**
 * Define a range for use in matrix operations
 */
class Span {
private:
	Span(); /* hide */

public:
	Span(msize_t _from, msize_t _to) : m_from(_from), m_to(_to) {

#if RANGE_CHECK
		if (m_from > m_to)
			throw std::range_error("from > to");
#endif

	}

	Span(const Span& cp) : m_from(cp.m_from), m_to(cp.m_to) {

#if RANGE_CHECK
		if (m_from > m_to)
			throw std::range_error("from > to");
#endif
	}

	msize_t from() const {
		return m_from;
	}

	msize_t to() const {
		return m_to;
	}

private:
	msize_t m_from;
	msize_t m_to;
};

}  // namespace ranging

#endif /* RTYPES_H_ */
