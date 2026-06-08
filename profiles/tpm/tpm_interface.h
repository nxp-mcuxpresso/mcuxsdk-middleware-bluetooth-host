/*! *********************************************************************************
 * \addtogroup TPMS Service
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2025-2026 NXP
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef TPM_INTERFACE_H
#define TPM_INTERFACE_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "gatt_server_interface.h"

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
/*! Length in bytes of the TPMS Signing Key */
#define gTpmsSigningKeyLength_c         (16U)

/*! Number of advertising sequence-number updates between successive saves of the
    sequence number to non-volatile storage. The sequence number is not saved on
    every update (that would be too frequent); instead it is saved once every
    gTpmsSeqNumSaveInterval_c updates. On restore the application advances the
    value by this amount to account for updates that may not have been persisted,
    so that the same sequence number is never reused with a given signing key. */
#define gTpmsSeqNumSaveInterval_c       (100U)


typedef enum
{
    gParkedState_c                            = 0x00U,
    gEarlyDrivingState_c                      = 0x01U,
    gEarlyDrivingPrimaryMonitorPairingState_c = 0x02U,
    gDrivingState_c                           = 0x03U,
    gEarlyParkedState_c                       = 0x04U,
    gServiceState_c                           = 0x05U
} tpmsSensorState_tag;
/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
/*! Structure holding sensor-read data */
typedef struct
{
    uint16_t   tirePressure;
    uint8_t    tirePressureAccuracy;
    uint8_t    tireTemperature;
    uint8_t    tireTemperatureAccuracy;
} tpmsSensorReadData_t;

/*! Callback invoked by the service when the Signing Key changes, so the
    application can persist the new key (e.g. into NVM).

    \param[in] pKey       Pointer to the new Signing Key bytes.
    \param[in] keyLength  Length of the key in bytes. */
typedef void (*tpmsSigningKeyChangedCb_t)(const uint8_t *pKey, uint16_t keyLength);

/*! Callback invoked periodically by the service so the application can persist
    the advertising sequence number (e.g. into NVM). It is called once every
    gTpmsSeqNumSaveInterval_c sequence-number updates rather than on every update.

    \param[in] seqNum  The current sequence-number value to persist. */
typedef void (*tpmsSeqNumSaveCb_t)(uint32_t seqNum);

/*! TPM Service - Configuration */
typedef struct tpmConfig_tag
{
    uint16_t                    serviceHandle;
    bool_t*                     aValidSubscriberList;
    uint8_t                     validSubscriberListSize;
    tpmsSigningKeyChangedCb_t   signingKeyChangedCb;
    tpmsSeqNumSaveCb_t          seqNumSaveCb;
} tpmConfig_t;

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

/*!**********************************************************************************
* \brief        Starts Tire Pressure Monitoring Service functionality
*
* \param[in]    pServiceConfig  Pointer to structure that contains server
*                               configuration information.
*
* \return       None.
************************************************************************************/
void Tpms_Start(tpmConfig_t *pServiceConfig);

/*!**********************************************************************************
* \brief        Subscribes a GATT client to the TPM service
*
* \param[in]    pServiceConfig  Pointer to service configuration structure
* \param[in]    pClient  Client Id in Device DB.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Tpms_Subscribe(tpmConfig_t* pServiceConfig, deviceId_t clientDeviceId);

/*!**********************************************************************************
* \brief        Unsubscribes a GATT client from the TPM service
*
* \param[in]    pServiceConfig  Pointer to service configuration structure
* \param[in]    pClient  Client Id in Device DB.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Tpms_Unsubscribe(tpmConfig_t* pServiceConfig, deviceId_t clientDeviceId);

/*!**********************************************************************************
* \brief        Handles an ATT write from the peer. Checks if the peer is the Primary
*               Monitor and acts accordingly.
*
* \return       None.
************************************************************************************/
void Tpms_HandleAttributeWritten(deviceId_t deviceId, gattServerEvent_t* pServerEvent);

/*!**********************************************************************************
* \brief        Handles an ATT read from the peer. Checks if the peer is the Primary
*               Monitor and acts accordingly.
*
* \return       None.
************************************************************************************/
void Tpms_HandleAttributeRead(deviceId_t deviceId, gattServerEvent_t* pServerEvent);

/*!**********************************************************************************
* \brief        Returns advertising duration, either the default value or as configured
*               in the GATT database by the Primary Monitor.
*
* \return       Advertising duration in units of 10ms.
************************************************************************************/
uint8_t Tpms_GetAdvDuration(void);

/*!**********************************************************************************
* \brief        Returns standby duration, either the default value or as configured
*               in the GATT database by the Primary Monitor.
*
* \param[in]    state   State of the sensor.
*
* \return       Standby duration in seconds.
************************************************************************************/
uint16_t Tpms_GetStandbyDuration(uint8_t state);

/*!**********************************************************************************
* \brief        Updates TMPS advertising data, including Sequence Number and MAC.
*
* \param[in/out]    pData            Pointer to array holding advertising data.
* \param[in]        pSensorReadData  Pointer to struct holding data read from sensors.
*
* \return       None
************************************************************************************/
void Tpms_UpdateAdvData(uint8_t *pData, tpmsSensorReadData_t *pSensorReadData);

/*!**********************************************************************************
* \brief        Sends Tire Pressure notifications to clients.
*
* \param[in]    pServiceConfig   Pointer to structure that contains server
*                                configuration information.
* \param[in]    pSensorReadData  Pointer to struct holding data read from sensors.
*
* \return       None.
************************************************************************************/
void Tpms_NotifyTirePressure(tpmConfig_t *pServiceConfig, tpmsSensorReadData_t *pSensorReadData);

/*!**********************************************************************************
* \brief        Update Signing Key characteristic.
*
* \param[in]    pServiceConfig  Pointer to structure that contains server
*                               configuration information.
*
* \return       None.
************************************************************************************/
void Tpms_UpdateSigningKey(tpmConfig_t *pServiceConfig);

/*!**********************************************************************************
* \brief        Sets the Signing Key characteristic to a known value. Used by the
*               application to restore a previously persisted key (e.g. from NVM).
*               No indication is sent and the signingKeyChangedCb is not invoked.
*
* \param[in]    pServiceConfig  Pointer to structure that contains server
*                               configuration information.
* \param[in]    pKey            Pointer to the key bytes to set.
* \param[in]    keyLength       Length of the key in bytes (gTpmsSigningKeyLength_c).
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Tpms_SetSigningKey(tpmConfig_t *pServiceConfig, const uint8_t *pKey, uint16_t keyLength);

/*!**********************************************************************************
* \brief        Sets the advertising sequence number to a known value. Used by the
*               application to restore a previously persisted sequence number
*               (e.g. from NVM) so that values are not reused with the same key.
*               The seqNumSaveCb is not invoked.
*
* \param[in]    seqNum  The sequence-number value to set.
*
* \return       None.
************************************************************************************/
void Tpms_SetSeqNum(uint32_t seqNum);
#ifdef __cplusplus
}
#endif

#endif /* TPM_INTERFACE_H */

/*! **********************************************************************************
 * @}
 ************************************************************************************/
