/***********************************************************************************/
/*!
 *  @brief      
 *  @file       main.cpp
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
#include <string.h>
#include <fenv.h>
#include <signal.h>

#include "appversion.h"


#ifdef __linux__
#define ENABLE_FLOATING_POINT_EXCEPTIONS		1
#else
#define ENABLE_FLOATING_POINT_EXCEPTIONS		0
#endif

extern int test_runner_impl();

#if MEMORY_RECORD
#include "allocator.hpp"
#endif

#if defined(STATIC_MEMORY_BUILD)
extern "C" void memory_global_init(void);
#endif

#if ENABLE_FLOATING_POINT_EXCEPTIONS
static void fpe_decode(int ferror) {
	printf("*** FPE: Floating Point Error: 0x%x [", ferror);
	if (ferror & (FE_DIVBYZERO))
		printf(" FE_DIVBYZERO");
	if (ferror & (FE_INEXACT))
		printf(" FE_INEXACT");
	if (ferror & (FE_INVALID))
		printf(" FE_INVALID");
	if (ferror & (FE_OVERFLOW))
		printf(" FE_OVERFLOW");
	if (ferror & (FE_UNDERFLOW))
		printf(" FE_UNDERFLOW");
	if ((ferror & (FE_ALL_EXCEPT)) == 0)
		printf(" none");
	printf("]\n");
}

static void fpe_handler(int sig, siginfo_t *siginfo, void *context) {
	int ferror = siginfo->si_code;
	fpe_decode(ferror);
	exit(-1);
}
#endif

int main(void) {

	int ret = 0;

	puts("Test.");

	printf("GIT Version: %d   hash: %s   date: %s\n", APPVERSION, APPHASH, APPDATE);

	// optional for windows to enable FP exceptions (for now it breaks execution)
	//#if defined(_MSC_VER)
	//	_controlfp( _EM_INVALID, _MCW_EM );
	//#endif

#if ENABLE_FLOATING_POINT_EXCEPTIONS
	int excepts = fegetexcept() | FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW;
	feenableexcept(excepts);

	struct sigaction act;
	memset(&act, 0, sizeof(act));

	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &fpe_handler;

	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGFPE, &act, NULL);
#endif

#if defined(STATIC_MEMORY_BUILD)
	memory_global_init();
#endif

	/* configure format of float printing */
	// std::cout << std::fixed << std::setw(8) << std::setprecision(12);

	test_runner_impl();
        /* display the maximum amount of heap memory used (refer to main.h) */
	puts("Finished.");

	return ret;
}

// end
