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

#if USE_MEMTRACE
#include <common/include/memtrace.h>
#if __ICCARM__
#include "iar_dlmalloc.h"
#endif
#endif

#if __ARM_ARCH
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "common/include/measurement_tools.h"

#if USE_STOPWATCH
#include <common/include/Stopwatch.h>
#endif
#endif

#ifdef __linux__
#define ENABLE_FLOATING_POINT_EXCEPTIONS		1
#else
#define ENABLE_FLOATING_POINT_EXCEPTIONS		0
#endif

extern int test_execute();

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
#if __ARM_ARCH
        /*Board init */
        BOARD_InitPins();
        BOARD_BootClockRUN();
        //BOARD_InitDebugConsole();
#endif
	int ret = 0;
        /*           */
	puts("Test.");

	printf("GIT Version: %d   hash: %s   date: %s\n", APPVERSION, APPHASH, APPDATE);

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

#if __ARM_ARCH && USE_STOPWATCH
	/* enable cycle counting */
        enableCpuCycleCounter();
#endif
	/* run the standard test cases */
	test_execute();

#if MEMORY_RECORD_CSTACK
        display_cstack_max();
#endif

#if USE_MEMTRACE
        MEMTRACE_MARKER_PEAK_SUMMARY(" summary ");
#if __ICCARM__
        __iar_dlmalloc_stats();
#endif
#endif

#if USE_STOPWATCH
        stopwatch_display_stat();
        stopwatch_display_max();
#endif
	puts("Finished.");
	return ret;
}

// end
