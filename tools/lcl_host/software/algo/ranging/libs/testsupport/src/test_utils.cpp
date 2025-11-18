/***********************************************************************************/
/*!
 *  @brief      
 *  @file       test_utils.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#include <testsupport/include/test_utils.h>

#define TEST_UTILS_CMP_DETAIL			0		/* 1 = on, 0 = off */


extern "C" void setUp(void) {
	// std::cout << __PRETTY_FUNCTION__ << std::endl;
	//puts("# start");
}

extern "C" void tearDown(void) {
	// std::cout << __PRETTY_FUNCTION__ << std::endl;
	//puts("# end");
}


bool test_cmp(const Matrix<float>& M, const Matrix<float>& N, float epsilon) {

	float error = 0.0;

	if (M.rows() != N.rows()) return false;
	if (M.cols() != N.cols()) return false;

	/* first check */
	if (M == N)
		return true;

	for (msize_t i = 0; i < M.rows(); i++) {
		for (msize_t j = 0; j < M.cols(); j++) {
			float diff = std::abs(M(i, j) - N(i, j));
			error = std::max(error, diff);
#if TEST_UTILS_CMP_DETAIL
			printf("act %16.16e ref %16.16e error %16.16e\n", M(i, j), N(i, j), error);
#endif
		}
	}

	if (error >= epsilon) {
		printf("test_cmp Matrix<float>: error %16.16e", error);
		printf(" >= %16.16e (epsilon) ==> %s\n", epsilon, (error < epsilon) ? "TRUE" : "FALSE");
	}

	return (error < epsilon);
}

bool test_cmp(const Matrix<std::complex<float> >& M, const Matrix<std::complex<float> >& N, float epsilon) {

	float error = 0.0f;

	if (M.rows() != N.rows()) return false;
	if (M.cols() != N.cols()) return false;

	/* first check */
	if (M == N)
		return true;

	for (msize_t i = 0; i < M.rows(); i++) {
		for (msize_t j = 0; j < M.cols(); j++) {
			float diff1 = std::abs(M(i, j).real() - N(i, j).real());
			error = std::max(error, diff1);
#if TEST_UTILS_CMP_DETAIL
			printf("real %16.16e real %16.16e diff %16.16e  error %16.16e\n", M(i, j).real(), N(i, j).real(), diff1, error);
#endif
			float diff2 = std::abs(M(i, j).imag() - N(i, j).imag());
			error = std::max(error, diff2);
#if TEST_UTILS_CMP_DETAIL
			printf("imag %16.16e imag %16.16e  diff %16.16e error %16.16e\n", M(i, j).imag(), N(i, j).imag(), diff2, error);
			printf("element: (%d,%d) --> diff_r %16.16e  diff_i %16.16e  error %16.16e\n", i, j, diff1, diff2, error);
#endif
		}
	}

#if TEST_UTILS_CMP_DETAIL
	printf("test_cmp Matrix<std::complex<float>>: error %16.16e",error);
	printf(" < %16.16e is %s\n", epsilon ,(error < epsilon) ? "TRUE" : "FALSE");
#endif

	return (error < epsilon);
}


//End of File

