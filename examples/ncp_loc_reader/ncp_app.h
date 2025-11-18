/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file btcs_server_interface.h
*
* This is a header file for the connection common application code.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef NCP_APP_H
#define NCP_APP_H

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

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum
{
    gConnected_c            = 0x00U,    /*!< A new peer has connected */
    gDisonnected_c          = 0x01U,    /*!< A peer has disconnected */
    gPairComplete_c         = 0x02U,    /*!< Pairing completed */
    gLinkEncrypted_c        = 0x03U,    /*!< Link encryption complete */
    gMtuExchanged_c         = 0x04U,    /*!< MTU exchange complete */
    gCsConfigCompl_c        = 0x05U,    /*!< Channel Sounding configuration complete */
} ncpConnMsgType_t;

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

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn        bleResult_t App_NcpAppFsciInit(void)
*
*\brief     Ncp application specific messages initialization function.
*
*\retval    gBleAlreadyInitialized_c    Module already initialized
*\retval    gBleUnexpectedError_c       Error occured while sending a request to the host
*\retval    gBleSuccess_c               Operation successfull
********************************************************************************** */
bleResult_t App_NcpAppFsciInit(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* NCP_APP_H */