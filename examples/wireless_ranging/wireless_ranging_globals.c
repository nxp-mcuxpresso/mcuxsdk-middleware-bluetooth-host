/*
 * Copyright 2020-2023, 2026 NXP
 *
 * NXP Proprietary
 *
 * This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or
 * by downloading, installing, activating and/or otherwise using the software, you are
 * agreeing that you have read, and that you agree to comply with and are bound by,
 * such license terms. If you do not agree to be bound by the applicable license terms,
 * then you may not retain, install, activate or otherwise use the software.
 */

/*
 * NXP interface with ISP code.
 * Contains functions to wrap SDK APIs to ISP needs.
 */
#include "wireless_ranging.h"
#include "fsl_port.h"
#include "fsl_component_serial_manager.h"
#include "fsl_component_mem_manager.h"

#if gNvStorageIncluded_d
#include "NVM_Interface.h"
#endif

#include "stdbool.h"
#include "isp_interface.h"

static struct {
    isp_configuration_local_t mLocalSettings;
    isp_configuration_range_t mRangeSettings;
    isp_configuration_communication_t mCommunicationSettings;
} mIspSettings;

#if gNvStorageIncluded_d
#define mIspSettings_ID_c 0x0010
#define mRangingCalibrationData_ID_c 0x0011
NVM_RegisterDataSet(&mIspSettings, 1, (uint16_t)sizeof(mIspSettings), mIspSettings_ID_c, (uint16_t)gNVM_MirroredInRam_c);
NVM_RegisterDataSet(&local_bid.cal, 1, (uint16_t)sizeof(board_ranging_calibration_t), mRangingCalibrationData_ID_c, (uint16_t)gNVM_MirroredInRam_c);
#endif

isp_configuration_local_t *gLocalSettings = &mIspSettings.mLocalSettings;
isp_configuration_range_t *gRangeSettings = &mIspSettings.mRangeSettings;
isp_configuration_communication_t *gCommunicationSettings = &mIspSettings.mCommunicationSettings;

/*
 * Load data set from NVM if applicable.
 * Initialize as expected by isp code (0xFF) in case page does not exist
 */
static void isp_global_init(void *pData, size_t size)
{
    bool inited = FALSE;
#if gNvStorageIncluded_d
    NVM_Status_t status;
    /* Read data from flash */
    status = NvRestoreDataSet(pData, TRUE);
    if (gNVM_OK_c == status) {
        inited = TRUE;
    }
#endif
    if (!inited) {
        /* NVRAM not enabled, do as if the NVM records where not initialized */
        FLib_MemSet(pData, 0xFF, size);
    }

     /* Restore calibration data from flash */
    board_ranging_calibration_t *cal_data = &local_bid.cal;
    inited = FALSE;
#if gNvStorageIncluded_d
    status = NvRestoreDataSet(cal_data, TRUE);
    if (gNVM_OK_c == status)
    {
        inited = TRUE;
    }
#endif
    if (!inited) /* use default values */
    {
        cal_data->mciq_zdc = ZERO_DIST_CAL_MCIQ;
        cal_data->tof_zdc = ZERO_DIST_CAL_TOF;
    }
}

/* Initialize data sets as expected by isp code (0xFF if not in NVM) */
void isp_globals_init(void)
{
    isp_global_init(&mIspSettings, sizeof(mIspSettings));
}

/* Save all isp data sets to NVM */
void isp_globals_save(void)
{
#if gNvStorageIncluded_d
    (void)NvSaveOnIdle(&mIspSettings, TRUE);
#endif
}

void isp_save_calibration_data(board_ranging_calibration_t *cal_data)
{
#if gNvStorageIncluded_d
    (void)NvSaveOnIdle(cal_data, TRUE);
#endif
}

/* Hack to fix the case where a GCC library is linked with IAR-compiled application */
void __attribute__((weak)) __assertion_failed(char* s) {}
