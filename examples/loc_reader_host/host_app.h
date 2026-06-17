/*! *********************************************************************************
* Copyright 2025-2026 NXP
*
* NXP Proprietary
*
* This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or
* by downloading, installing, activating and/or otherwise using the software, you are
* agreeing that you have read, and that you agree to comply with and are bound by,
* such license terms. If you do not agree to be bound by the applicable license terms,
* then you may not retain, install, activate or otherwise use the software.
********************************************************************************** */

#ifndef HOST_APP_H
#define HOST_APP_H

/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "EmbeddedTypes.h"
#include "ble_general.h"
#include "app_localization.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
/* NCP BLE App FSCI group */
#define gFsciNcpAppOpcodeGroup_c               0x4FU

/* NCP BLE App FSCI requests ids */
#define gAppBleCSDataOpCode_c                  0x00U
#define gAppSendRangeSettingsOpCode_c          0x01U
#define gAppSetAlgoTypeIDOpCode_c              0x02U
#define gAppSendDisconnectNotifOpCode_c        0x03U
#define gAppSendProcRssiOpCode_c               0x04U

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum
{
    gIQLocalTrStart_c       = 0x00,    /*!< Transmit the first chunk of local localization data */
    gIQLocalTrCont_c        = 0x01,    /*!< Transmit a chunk of local localization data */
    gIQLocalTrEnd_c         = 0x02,    /*!< Transmit the last chunk of local localization data */
    gIQRemoteTrStart_c      = 0x03,    /*!< Transmit the first chunk of remote localization data */
    gIQRemoteTrCont_c       = 0x04,    /*!< Transmit a chunk of remote localization data */
    gIQRemoteTrEnd_c        = 0x05,    /*!< Transmit the last chunk of remote localization data */
} ncpIqTransferMsgType_t;

typedef struct appCsRes_tag
{
    rasMeasurementData_t *pData;
    bool_t isLocal;
} appCsRes_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\fn        bleResult_t App_HostAppFsciInit(void)
*
*\brief     Host Shell initialization function.
*
*\retval    gBleAlreadyInitialized_c    Module already initialized
*\retval    gBleUnexpectedError_c       Error occured while sending a request to the host
*\retval    gBleSuccess_c               Operation successfull
********************************************************************************** */
bleResult_t App_HostAppFsciInit(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /*  HOST_APP_H*/
