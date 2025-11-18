/*
 * Copyright 2019-2020, 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* This file is deprecated in favor of dm_phasebased.h.
 * Maintained only for backwards compatibility.
 */

#ifndef _PDE_POSTPROCESSING_H_
#define _PDE_POSTPROCESSING_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include "dm_phasebased.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! Indicates the maximum number of distance estimates to be calculated by the postprocessing software */
#define LCL_PDE_POSTPROC_MAX_ESTIMATES        (10U)

/*! Describes which filters should be applied to measurement */
#define LCL_PDE_POSTPROC_FILTER_HISTORICAL    (1UL << 0U)  /*!< Historical filter: compare last measurement to previously archived measurements */
#define LCL_PDE_POSTPROC_FILTER_ALL           (0xFFFFFFFFU)

/*! threshold configuration */
#define LCL_PDE_POSTPROC_THRESHOLD_UNDEF      (0U)
#define LCL_PDE_POSTPROC_THRESHOLD_DEFAULT    DM_CDE_THRESHOLD_DEFAULT

#define PDE_PostprocessingDistanceEstimation(pIQin1, pIQin2, PhaseVector, FreqMask, NumberFreqs, FreqStep, ZeroMComp, filterFlags, threshold, ChosenEstimate) \
        dm_cde_distance_estimation(pIQin1, pIQin2, PhaseVector, FreqMask, NumberFreqs, FreqStep, ZeroMComp, threshold, ChosenEstimate)

#define PDE_PostprocessingInit()

typedef uint32_t pde_postproc_freq_mask_t;
typedef dm_cde_estimate_t pde_estimate_t;

#endif /* _PDE_POSTPROCESSING_H_ */

/*! @} */

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
