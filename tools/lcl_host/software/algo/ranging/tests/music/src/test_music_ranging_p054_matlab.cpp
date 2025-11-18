#include "H_est_data.hpp"

#include <common/include/Stream.h>
#include <common/include/Functions.h>
#include <music/include/MusicRanging.h>
#include <common/include/Stopwatch.h>
#if USE_MEMTRACE
#include <common/include/memtrace.h>
#endif
#if __ARM_ARCH
#include "fsl_common.h"
#endif
#include <testsupport/include/test_utils.h>
#include "unity.h"

static const float DISTANCE_TOLERANCE = 0.02f;  /* Tolerance of xx centimeters */

static int nb_test = 0;

/* call SRDE algorithm routine for data from H_est_data.cpp */
void test_call(void){
        TRACE();

        float dist_est;
        MusicRanging mr;
        Matrix<std::complex<float>> H_est(kA, 80, _H_est_array[nb_test]);

#if GLOBAL_DEF_EVD_TYPE
        mr.calculate(CHANNEL_DISTANCE, 40, H_est, dist_est,global_evd_type);
#else
        mr.calculate(CHANNEL_DISTANCE, 40, H_est, dist_est);
#endif
        TEST_ASSERT_FLOAT_WITHIN(DISTANCE_TOLERANCE, _H_est_true_distance[nb_test], dist_est);
}

/* call the SRDE routine on all the data contained in H_est_data.cpp */
void test_P054_1MHz_80(void) {
        for ( int i = 0; i <= kC-1 ; i++ ){
           nb_test = i;
           printf("test_P054_1MHz_channel_%d_80_num_%d\n",kA,i+1);
#if MEMORY_RECORD_CSTACK
           mark_cstack_memory();
#endif
           RUN_TEST(test_call);
#if MEMORY_RECORD_CSTACK
           measure_cstack_size();
#endif
        }
        nb_test = 0;
}

