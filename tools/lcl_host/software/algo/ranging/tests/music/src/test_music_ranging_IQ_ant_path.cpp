#include "IQ_data_py.hpp"

#include <channelReconstruct_2D/include/ChannelReconstruct_2D.h>
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

static int nb_test_H_est = 0;
static int nb_test_IQ = 0;

/* call SRDE algorithm routine with CR2D */
static void test_call_IQ(void){
        TRACE();
        
        float likeliness;
        float dist_est;
        MusicRanging mr;
        const ranging::MatrixCmpl _IQ_A(kA_IQ, kB_IQ, _IQ_array[nb_test_IQ]);
        const ranging::MatrixCmpl _IQ_B(kA_IQ, kB_IQ, _IQ_array[nb_test_IQ+1]);
        unsigned int FreqMask[9] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

        mr.config().L = 40;
        mr.config().maxiter = 200;
        mr.config().MethodSubspaceSep = 8;
        mr.calculate_SRDE(_IQ_A,_IQ_B, FreqMask, CHANNEL_DISTANCE,kA_IQ, kB_IQ, dist_est, likeliness, ALGO_TYPE_EIGEN_SAES);

//        ranging::MatrixCmpl H_est(kA_IQ, kB_IQ, MATRIX_UNINIT_DATA);
//        ranging::MatrixCmpl H_est_true(kA_IQ, kB_IQ, _H_est_array_div[nb_test_H_est]);
//        MatrixF K_est(1, H_est.rows(), MATRIX_UNINIT_DATA);
//        Matrix<int> Uncertainty(1, H_est.rows(), MATRIX_UNINIT_DATA);
//        ChannelReconstruct_2D(_IQ_A, _IQ_B, H_est, K_est, Uncertainty);
//
//        if(H_est == H_est_true){
//            printf("Same H_est\n");
//        }else{
//            printf("not the same H_est\n");
//        }
        TEST_ASSERT_FLOAT_WITHIN(DISTANCE_TOLERANCE, _IQ_array_true_distance[nb_test_H_est], dist_est);
        
        nb_test_IQ += 2;
}
/* call the SRDE routine on all the data contained in H_est_data.cpp */
void test_IQ_ant_path(void) {
        for ( int i = 0; i <= kC_IQ-1 ; i++ ){
           nb_test_H_est = i;
           printf("test_IQ_ant_path_%d_%d_num_%d\n",kA_IQ,kB_IQ,i+1);
#if MEMORY_RECORD_CSTACK
           mark_cstack_memory();
#endif
           RUN_TEST(test_call_IQ);
#if MEMORY_RECORD_CSTACK
           measure_cstack_size();
#endif
        }
        nb_test_H_est = 0;
}

