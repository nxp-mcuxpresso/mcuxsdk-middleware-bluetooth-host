/*! *********************************************************************************
* Copyright 2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef HOST_APP_NVM_H
#define HOST_APP_NVM_H

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "EmbeddedTypes.h"

/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */

/*! Enable/disable use NV flash procedures for operations triggered by the host stack
    Do not modify directly. Redefine it in the app_preinclude.h file*/
#ifndef gAppUseNvm_d
#define gAppUseNvm_d                    (FALSE)
#endif /* gAppUseNvm_d */

/* FSCI group */
#define gFsciAppBleNvmCbOpcodeGroup_c   0x4D
/*! *********************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
*\fn        bleResult_t App_HostNvmInit(void)
*\brief     Host NVM initialization function.
*
*\retval    gBleAlreadyInitialized_c    NCP NVM already initialized
*\retval    gBleUnexpectedError_c       Error occured while sending a request to the host
*\retval    gBleSuccess_c               Operation successfull
********************************************************************************** */
bleResult_t App_HostNvmInit(void);

/*! *********************************************************************************
*\fn            void App_FsciBleNvmCbHandler(void* pData, void* param, uint32_t fsciInterface)
*\brief         Handle FSCI NCP NVM commands received from Host on FSCI.
*
* \param[in]    pData               Packet (containing FSCI header and FSCI
                                    payload) received over UART.
* \param[in]    param               Pointer given when this function is registered in
                                    FSCI.
* \param[in]    fsciInterfaceId     FSCI interface on which the packet was received.
*
*\retval    void
********************************************************************************** */
void App_FsciBleNvmCbHandler
(
    void*       pData,
    void*       param,
    uint32_t    fsciInterface
);
#endif /* HOST_APP_NVM_H */
