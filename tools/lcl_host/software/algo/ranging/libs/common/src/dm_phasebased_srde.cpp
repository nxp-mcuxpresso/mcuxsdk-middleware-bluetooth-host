/*
 * Copyright 2019-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <music/include/MusicRanging.h>
#include <common/include/Constants.h>
#include <common/include/Functions.h>
#include <assert.h>
#include "dm_phasebased.h"

dm_status_t dm_srde_distance_estimation(int16_t *pIQin1, int16_t *pIQin2, dm_freq_mask_t *FreqMask, uint16_t n_ap, uint16_t NumberFreqs, uint16_t FreqStep, dm_srde_config * config, dm_srde_estimate_t *DistanceEstimate)
{
    ranging::MusicRanging mr;

    /* Check if inputs variables are consistent */
    if(pIQin1 == NULL || pIQin2 == NULL || FreqMask == NULL || DistanceEstimate == NULL || n_ap == 0 || n_ap > 4)
    {
        return DM_STATUS_INVALID_PARAMETER;
    }      
    
    if(config != NULL)
    {
        ranging::RangingSettings conf_mr;
        conf_mr.MethodSubspaceSep = config->methodSubspaceSeparation;
        conf_mr.maxiter = config->maxiter;
        conf_mr.TOL2 = config->TOL2;
        conf_mr.L = config->L;
        mr.config() = conf_mr;
    }

    if(NumberFreqs < mr.config().L || NumberFreqs < mr.config().MethodSubspaceSep  || mr.config().L == 0)
    {
        return DM_STATUS_INVALID_PARAMETER;
    }
    
    /* Initiliaze output with default values */
    DistanceEstimate->distance_estimate = FLT_MIN;
    DistanceEstimate->dqi = -1.0f;
    
    ranging::MatrixCmpl IQ_A(n_ap /* rows */, NumberFreqs /* cols */, ranging::MATRIX_UNINIT_DATA);
    ranging::MatrixCmpl IQ_B(n_ap /* rows */, NumberFreqs /* cols */, ranging::MATRIX_UNINIT_DATA);

    for (ranging::msize_t i = 0; i < n_ap ; i++) // rows
    {
        for (ranging::msize_t j = 0; j < NumberFreqs; j++) // columns
        {
            if IS_FREQ_SMP_VALID (j, FreqMask)
            {
                IQ_A(i, j) = ranging::ComplexF((float)pIQin1[2*j] / DM_MAX_IQ_CODES, (float)pIQin1[2*j + 1] / DM_MAX_IQ_CODES);
                IQ_B(i, j) = ranging::ComplexF((float)pIQin2[2*j] / DM_MAX_IQ_CODES, (float)pIQin2[2*j + 1] / DM_MAX_IQ_CODES);
#if MUSIC_SHOW_INPUTS
                std::cout << __FUNCTION__ << "  IQ_A(i, j) = " << IQ_A(i, j) << std::endl;
                std::cout << __FUNCTION__ << "  IQ_B(i, j) = " << IQ_B(i, j) << std::endl;
#endif
            }
            else
            {
                if (j == 0)
                {
                    IQ_A(i, j) = ranging::ComplexF(0,0);
                    IQ_B(i, j) = ranging::ComplexF(0,0);
                }
                else
                {
                    IQ_A(i, j) = IQ_A(i, j-1);
                    IQ_B(i, j) = IQ_B(i, j-1);
                }
            }
        }
        pIQin1 += (2*NumberFreqs);
        pIQin2 += (2*NumberFreqs);
    }
      
    mr.calculate_SRDE(IQ_A, IQ_B, (unsigned int*)FreqMask, (float_t)FreqStep * 1000.0f, n_ap, NumberFreqs, DistanceEstimate->distance_estimate , DistanceEstimate->dqi, ALGO_TYPE_EIGEN_SAES);

    /* Check if distance and dqi are valid */
    if(DistanceEstimate->distance_estimate == FLT_MIN || isnan(DistanceEstimate->distance_estimate)){
        return DM_STATUS_INVALID_DISTANCE;
    }
    if(DistanceEstimate->dqi < 0.0f || isnan(DistanceEstimate->dqi)){
        return DM_STATUS_INVALID_DQI;
    }

    return DM_STATUS_OK;
}