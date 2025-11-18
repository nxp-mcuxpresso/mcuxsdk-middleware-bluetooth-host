/***********************************************************************************/
/*!
 *  @brief      
 *  @file       test_execute.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>

/* define the test runners */
extern int test_runner_music();

//#define EXIT_ON_ERROR() if (ret) ::exit(-ret);
#define EXIT_ON_ERROR() /* empty */

/* execute every test runner, exit(-) when there is an FAIL detected */
int test_execute(void) {

	int ret = 0;

	puts("Test execute.");

	ret += test_runner_music();
	EXIT_ON_ERROR();

	return ret;
}

// end
