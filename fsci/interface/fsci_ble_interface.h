/*! *********************************************************************************
* \defgroup FSCI_BLE_INTERFACE BLE FSCI
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2018, 2021-2023 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef FSCI_BLE_INTERFACE_H
#define FSCI_BLE_INTERFACE_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "FsciInterface.h"
#include "ble_general.h"
#include "gap_interface.h"
#include "gatt_client_interface.h"
#include "gatt_server_interface.h"

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

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

#ifdef __cplusplus
extern "C" {
#endif

/*! *********************************************************************************
* \brief   This function registers the FSCI handlers for the enabled BLE layers.
*
* \param[in]    fsciInterfaceId        The interface on which data should be printed.
*
********************************************************************************** */
void fsciBleRegister
(
    uint32_t fsciInterfaceId
);


/*! *********************************************************************************
* \brief   This function sets the HCI host to controller interface to an application
*          desired function.
*
* \param[in]    hostToControllerInterface   The function desired by the application to be
*                                           used as host to controller interface, or NULL.
*                                           If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetHciHostToControllerInterface
(
    hciHostToControllerInterface_t hostToControllerInterface
);


/*! *********************************************************************************
* \brief   This function sets the GAP controller callback to an application desired
*          function.
*
* \param[in]    genericCallback         The function desired by the application to be
*                                       used as controller callback, or NULL.
*                                       If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetGapGenericCallback
(
    gapGenericCallback_t genericCallback
);


/*! *********************************************************************************
* \brief   This function sets the GAP advertising callback to an application desired
*          function.
*
* \param[in]    advertisingCallback     The function desired by the application to be
*                                       used as advertising callback, or NULL.
*                                       If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetGapAdvertisingCallback
(
    gapAdvertisingCallback_t advertisingCallback
);


/*! *********************************************************************************
* \brief   This function sets the GAP connection callback to an application desired
*          function.
*
* \param[in]    connectionCallback      The function desired by the application to be
*                                       used as connection callback, or NULL.
*                                       If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetGapConnectionCallback
(
    gapConnectionCallback_t connectionCallback
);


/*! *********************************************************************************
* \brief   This function sets the GAP scanning callback to an application desired
*          function.
*
* \param[in]    scanningCallback        The function desired by the application to be
*                                       used as scanning callback, or NULL.
*                                       If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetGapScanningCallback
(
    gapScanningCallback_t scanningCallback
);


/*! *********************************************************************************
* \brief   This function sets the GATT Client procedure callback to an application desired
*          function.
*
* \param[in]    clientProcedureCallback     The function desired by the application to be
*                                           used as Client procedure callback, or NULL.
*                                           If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetGattClientProcedureCallback
(
    gattClientProcedureCallback_t clientProcedureCallback
);


/*! *********************************************************************************
* \brief   This function sets the GATT Client notification callback to an application
*          desired function.
*
* \param[in]    clientNotificationCallback      The function desired by the application to be
*                                               used as Client notification callback, or NULL.
*                                               If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetGattClientNotificationCallback
(
    gattClientNotificationCallback_t clientNotificationCallback
);


/*! *********************************************************************************
* \brief   This function sets the GATT Client indication callback to an application
*          desired function.
*
* \param[in]    clientIndicationCallback        The function desired by the application to be
*                                               used as Client indication callback, or NULL.
*                                               If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetGattClientIndicationCallback
(
    gattClientIndicationCallback_t clientIndicationCallback
);

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
/*! *********************************************************************************
* \brief   This function sets the GATT Client Multiple Value Notification callback to an application
*          desired function.
*
* \param[in]    clientMultipleValueNtfCallback  The function desired by the application to be
*                                               used as Client Multiple Value Notification callback,
*                                               or NULL.
*                                               If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetGattClientMultipleValueNotificationCallback
(
    gattClientMultipleValueNotificationCallback_t clientMultipleValueNtfCallback
);

#if (defined gEATT_d) && (gEATT_d == TRUE)
/*! *********************************************************************************
* \brief   This function sets the GATT Client Enhanced Procedure callback to an application
*          desired function when EATT is used.
*
* \param[in]    clientEnhancedProcedureCallback  The function desired by the application to be
*                                                used as Client Enhanced Procedure callback,
*                                                or NULL. If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetGattClientEnhancedProcedureCallback
(
    gattClientEnhancedProcedureCallback_t clientEnhancedProcedureCallback
);

void fsciBleSetGattClientEnhancedMultipleValueNotificationCallback
(
    gattClientEnhancedMultipleValueNotificationCallback_t clientEnhancedMultipleValueNtfCallback
);

void fsciBleSetGattClientEnhancedNotificationCallback
(
    gattClientEnhancedNotificationCallback_t clientEnhancedNotificationCallback
);

void fsciBleSetGattClientEnhancedIndicationCallback
(
    gattClientEnhancedIndicationCallback_t clientEnhancedIndicationCallback
);

void fsciBleSetGattServerEnhancedCallback
(
    gattServerEnhancedCallback_t serverEnhancedCallback
);
#endif /* gEATT_d */
#endif /* gBLE52_d */

/*! *********************************************************************************
* \brief   This function sets the GATT Server callback to an application desired function.
*
* \param[in]    serverCallback      The function desired by the application to be
*                                   used as Server callback, or NULL.
*                                   If NULL, the FSCI will use an empty function.
*
********************************************************************************** */
void fsciBleSetGattServerCallback
(
    gattServerCallback_t serverCallback
);


#ifdef __cplusplus
}
#endif

#endif /* FSCI_BLE_INTERFACE_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
