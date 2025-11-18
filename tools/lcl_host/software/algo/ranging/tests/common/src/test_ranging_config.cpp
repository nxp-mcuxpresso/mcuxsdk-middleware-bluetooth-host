/***********************************************************************************/
/*!
 *  @brief      
 *  @file       test_ranging_config.cpp
 *  @author     Nicolas Moro (nicolas.moro@imec.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#include <common/include/RangingSettings.h>

#include <testsupport/include/test_utils.h>
#include "unity.h"

/* ------------------------------------------------------------------------------ */


static void test_set_defaults() {

  float   default_PS_approx = 0.005f;
  float   default_InitPos = 0.0f;
  float   default_MinLevel = 0.100f;
  float   default_TOL1 = 1.0e-7f;
  float   default_TOL2 = 0.999f;
  float   default_NFFT = 512.0f;
  int     default_maxiter = 100;
  msize_t default_MethodSubspaceSep = 14;

  RangingSettings r;


  TEST_ASSERT_EQUAL( r.PS_approx,         default_PS_approx );
  TEST_ASSERT_EQUAL( r.InitPos,           default_InitPos );
  TEST_ASSERT_EQUAL( r.MinLevel,          default_MinLevel );
  TEST_ASSERT_EQUAL( r.TOL1,              default_TOL1 );
  TEST_ASSERT_EQUAL( r.TOL2,              default_TOL2 );
  TEST_ASSERT_EQUAL( r.NFFT,              default_NFFT );
  TEST_ASSERT_EQUAL( r.maxiter,           default_maxiter );
  TEST_ASSERT_EQUAL( r.MethodSubspaceSep, default_MethodSubspaceSep );


  /* set values to 0 */
  r.PS_approx        = 0;
  r.InitPos          = 0;
  r.MinLevel         = 0;
  r.TOL1             = 0;
  r.TOL2             = 0;
  r.NFFT             = 0;
  r.maxiter          = 0;
  r.MethodSubspaceSep= 0;

  r.defaults();

  TEST_ASSERT_EQUAL( r.PS_approx,         default_PS_approx );
  TEST_ASSERT_EQUAL( r.InitPos,           default_InitPos );
  TEST_ASSERT_EQUAL( r.MinLevel,          default_MinLevel );
  TEST_ASSERT_EQUAL( r.TOL1,              default_TOL1 );
  TEST_ASSERT_EQUAL( r.TOL2,              default_TOL2 );
  TEST_ASSERT_EQUAL( r.NFFT,              default_NFFT );
  TEST_ASSERT_EQUAL( r.maxiter,           default_maxiter );
  TEST_ASSERT_EQUAL( r.MethodSubspaceSep, default_MethodSubspaceSep );
}

/**
 * Run the tests
 * @return number of test failures
 */
int test_runner_ranging_config() {

	UNITY_BEGIN();

	RUN_TEST(test_set_defaults);

	return UNITY_END();
}


//End of File

