/*
 * @Copyright (c) 2019, IMEC 
 */
/*
 * Copyright 2020-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Prevent double inclusion */
#ifndef _ISP_CONFIGURATION_H_
#define _ISP_CONFIGURATION_H_

/*!
 * @addtogroup genfsk_isp GENFSK Narrowband Localization
 * @{
 */

/*! @file
 * Configuration data structures definition.
 */
     
/* === Includes ============================================================= */
#include "gap_types.h"
#include "isp_interface.h"

/* === Macros =============================================================== */

/* === Types ================================================================ */

/*! Structure holding configuration of the BLE connections. */
typedef struct isp_configuration_communication_tag
{
    gapRole_t role;                      /*!< Role of the device */
    uint32_t conn_int;                   /*!< Comnnection interval in 1.25ms slots */
    uint8_t address_list_len;            /*!< Number of addresses in Authorized list */
    /* Authorized list of remote devices that can be connected to central device */
    uint8_t address_list[gLclMaxConnections_c][gcBleDeviceAddressSize_c];
} isp_configuration_communication_t;

/*!
 * Structure holding configuration of local device.
 * Most parameters only apply to central node running the algorithms.
 */
typedef struct isp_configuration_local_tag
{
    uint8_t debug;              /*!< Board debug (DTEST ...) */
    uint8_t verbosity;          /*!< Verbosity of output and debug */
    uint8_t mciq_algo_flags;    /*!< flags to enable MCIQ algorithm invocation (MCIQ only) */
    uint16_t cde_threshold;     /*!< For experimentation, keep it set to DM_CDE_THRESHOLD_DEFAULT */
    uint16_t cde_div_threshold; /*!< For experimentation, keep it set to DM_CDE_THRESHOLD_DIVERSITY_DEFAULT */
} isp_configuration_local_t;

/*!
 * Calibration parameters.
 */
typedef struct board_ranging_calibration_tag {
    uint16_t mciq_zdc;  /*!< MCIQ zero distance bias */
    uint16_t tof_zdc;   /*!< ToF zero distance bias */
} board_ranging_calibration_t;

/*!
 * Structure to store board information.
 */
typedef struct board_info_tag {
    uint8_t uid[6];    /*!< placeholder for application's unique_id */
    uint32_t ver;       /*!< placeholder for application's version number */
    board_ranging_calibration_t cal; /*!< board calibration data */
} board_info_t;

/* === Externals ============================================================ */
/*! Local board information */
extern board_info_t local_bid;

#endif /* _ISP_CONFIGURATION_H_ */
/* EOF */
