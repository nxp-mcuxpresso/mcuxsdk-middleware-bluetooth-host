/***********************************************************************************/
/*!
 *  @brief      
 *  @file       test_utils.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef TEST_UTILS_H_
#define TEST_UTILS_H_

#include <common/include/Matrix.h>
#include <common/include/Constants.h>

using namespace ranging;

/**
 * Test two matrices for equality within a certain epsilon
 * @param M
 * @param N
 * @param epsilon
 * @return true means equal
 */
bool test_cmp(const Matrix<float>& M, const Matrix<float>& N, float epsilon);

/**
 * Test two matrices for equality within a certain epsilon
 * @param M
 * @param N
 * @param epsilon
 * @return true means equal
 */
bool test_cmp(const Matrix<std::complex<float> >& M, const Matrix<std::complex<float> >& N, float epsilon);

#endif /* TEST_UTILS_H_ */
