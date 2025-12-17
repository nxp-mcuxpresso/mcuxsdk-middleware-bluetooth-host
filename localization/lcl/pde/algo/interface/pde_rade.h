/*
 * Copyright 2022 - 2023, 2025 NXP
 *
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PDE_RADE_H__
#define __PDE_RADE_H__

/*!
 * @addtogroup rade_group RADE Algorithm API
 * @{
 */

/*! \file pde_rade
 * \brief CS Ranging Algorithm API Header.
 */

#include "stdint.h"

typedef float float_rade_t;

/*! \enum rade_result_type_t 
 * \brief RADE Algorithm Return Typedef 
 */
typedef enum _rade_result_type
{
    kRadeSuccess                = 0x00U,  /*!< RADE execution success */
    kRadeFail                   = 0x01U,  /*!< High execution failure */
    kRadeMemAllocFail           = 0x02U,  /*!< RADE memory allocation failure */
    kRadeMemAllocSuccess        = 0x03U,  /*!< RADE memory allocation success */
    kRadeModuleFail             = 0x04U,  /*!< RADE module execution failure */
    kRadeModuleSuccess          = 0x05U   /*!< RADE module execution success */
} rade_result_type_t;

/*! \enum rade_mode_t 
 * \brief RADE Algorithm Operation Mode Typedef 
 */
typedef enum _rade_mode
{
    kRadeNormal                 = 0x00U,  /*!< Normal ranging mode */
    kRadeCalibration            = 0x01U   /*!< Calibration mode */
} rade_mode_t;

/*! \enum rade_para_t 
 * \brief RADE Algorithm Operation Parameters Typedef 
 */
typedef struct _rade_para
{
    rade_mode_t radeMode;       /*!< RADE algorithm operation mode */
    float_rade_t distBias;      /*!< Distance bias */
    uint8_t ceHeap_id;          /*!< LCE heap ID */
} rade_para_t;

/*! \struct rade_cs_para_t 
 * \brief RADE Algorithm CS configuration parameters Typedef 
 */
typedef struct _rade_cs_para
{
    uint8_t main_mode_repeat;   /*!< Number of repeated main mode channels */
    uint16_t step_nb;           /*!< Number of CS steps */
    uint16_t startAclCnt;       /*!< ACL event index of CS start */
    uint16_t connInterval;      /*!< Connection interval of BLE link */
    uint8_t mode0_nb;           /*!< Mode-0 steps */
    uint8_t subevt_nb;          /*!< Number of subevents in a CS procedure */
    uint8_t rtt_type;           /*!< RTT type */
    uint8_t rtt_phy;            /*!< RTT PHY */
    uint8_t t_fcs;              /*!< T_FCS */
    uint8_t t_ip1;              /*!< T_IP1 */
    uint8_t t_ip2;              /*!< T_IP2 */
    uint8_t t_pm;               /*!< T_PM */
    uint8_t t_sw;               /*!< T_SW */
    uint8_t *channelMap;        /*!< Channel map array */
    uint8_t *modeMap;           /*!< Mode map array */
    uint8_t *subevtStopIdx;     /*!< StepId of the beginning of the next subevent */ 
    uint8_t *subevtConnEvent;   /*!< Delta regarding ACL counter of first subevent */ 
    uint8_t main_mode_type;     /*!< Main mode type */
    uint8_t sub_mode_type;      /*!< Sub-mode type */
    int8_t *refPowerLevel_init; /*!< referencePowerLevel at initiator */
    int8_t *refPowerLevel_refl; /*!< referencePowerLevel at reflector */
    uint8_t *subevtDoneStatus_init;  /*!< Subevent Done status - initiator */
    uint8_t *subevtDoneStatus_refl;  /*!< Subevent Done status - reflector */
} rade_cs_para_t;

/*! \struct rade_result_t
 * \brief RADE Algorithm Result Typedef
 */
typedef struct _rade_result
{
    float_rade_t *rng_est;     /*!< Distance estimation raw result */
    float_rade_t *rng_trk;     /*!< Distance estimation tracking result */
    float_rade_t *rng_est_qi;  /*!< Distance estimation quality indicator */
    float_rade_t *reserved;    /*!< Reserved port */
} rade_result_t;

/*! \struct rade_data_t
 * \brief RADE Algorithm Input Data Typedef
 */
typedef struct _rade_data
{
    int16_t *pct_i;            /*!< CS mode-2 and mode-3 PCT IQ samples from initiator */
    int16_t *pct_r;            /*!< CS mode-2 and mode-3 PCT IQ samples from reflector */
    uint32_t *tqi_mask;        /*!< TQI mask */
    uint32_t *chan_mask;       /*!< Channel map mask */
    uint8_t n_ap;              /*!< Number of antenna paths */
} rade_data_t;

#ifndef null_RNG
/*! \def null_RNG 
 * \brief Invalid RNG state identifier (value: -1.2345f). 
 */
#define null_RNG (-1.2345f)
#endif

/*! *********************************************************************************
*\fn           rade_result_type_t pde_rade(
*                  rade_data_t *radeData,
*                  void **csAlgoBuf,
*                  rade_cs_para_t *radeCsPara,
*                  rade_result_t *radeResult,
*                  rade_para_t radePara
*              )
*
*\brief        CS Phase-Based Ranging (PBR) using the RADE algorithm with specified
*              parameters and operational mode, storing ranging results and algorithm buffer.
*
*\param  [in]  radeData       Pointer to the RADE algorithm input data structure.
*\param  [out] csAlgoBuf      Double pointer to a context buffer of RADE algorithm for each CS link. Memory is allocated internally 
*                             by the function. Caller must free it using `rade_deinit()` when the CS link ends.
*\param  [in]  radeCsPara     Pointer to CS parameters. Must use effective CS parameters and formats defined in Bluetooth Core Specification 
*                             v6.0, Vol 4, Part E, Host Controller Interface Functional Specification.
*\param  [out] radeResult     Pointer to the structure where ranging results will be stored.
*\param  [in]  radePara       Operational parameters of the RADE algorithm.
*
*\return       rade_result_type_t  Return code indicating success or specific error conditions.
*
*\remarks      The function allocates memory for `*csAlgoBuf` and the caller is responsible for freeing it when the CS link ends.
********************************************************************************** */
rade_result_type_t pde_rade(rade_data_t *radeData, void **csAlgoBuf, rade_cs_para_t *radeCsPara, rade_result_t *radeResult, rade_para_t *radePara);

/*! *********************************************************************************
*\fn           void rade_deinit(
*                  void **rngAlgoBuf
*              )
*
*\brief        Deinitialize and release resources associated with the RADE algorithm, 
*              including dynamically allocated buffers.
*
*\param  [in,out] rngAlgoBuf  Double pointer to the RADE algorithm context buffer to be deinitialized.
*                             The function frees the buffer memory and sets the pointer to NULL.
*
*\remarks      This function must be called when the CS link ends (e.g., in disconnection event callback) to clean up 
*              resources allocated during RADE algorithm execution. 
*              After deinitialization, rngAlgoBuf becomes invalid and should not be reused. 
*              Failing to call this function may result in memory leaks.
********************************************************************************** */
void rade_deinit(void **rngAlgoBuf);

/*! @} */

#endif /* __PDE_RADE_H__ */

/* EOF */