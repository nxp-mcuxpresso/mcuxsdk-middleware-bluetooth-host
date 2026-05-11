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

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "FunctionLib.h"
#include "SecLib.h"
#include "ble_general.h"
#include "tpm_interface.h"
#include "gatt_db_app_interface.h"
#include "gatt_db_handles.h"
#include "gap_interface.h"

/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/
#define gTpmsSigningKeyLength_c         (16U)
#define gTpmsDutyCycleCharValueLength_c (13U)

#define TPMS_DUTY_CYCLE_OFFSET_PARKED           (1U)
#define TPMS_DUTY_CYCLE_OFFSET_EARLY_DRIVING    (3U)
#define TPMS_DUTY_CYCLE_OFFSET_DRIVING          (5U)
#define TPMS_DUTY_CYCLE_OFFSET_EARLY_PARKED     (7U)

#define TPMS_AD_OFFSET_PRESSURE         (2U)
#define TPMS_AD_OFFSET_PRESSURE_ACC     (4U)
#define TPMS_AD_OFFSET_TEMPERATURE      (5U)
#define TPMS_AD_OFFSET_TEMP_ACC         (8U)
#define TPMS_AD_OFFSET_POSITION         (10U)
#define TPMS_AD_OFFSET_SEQNUM           (11U)
#define TPMS_AD_OFFSET_MAC              (15U)

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
/* Global sequence number used in advertising data */
static uint32_t mSeqNum = 0U;

/* Pointer to application TPM service config */
static tpmConfig_t *mpTpmConfig = NULL;
/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
static bool_t isValidAttributeValue(gattServerEvent_t* pServerEvent);
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*!**********************************************************************************
* \brief        Starts Tire Pressure Monitoring Service functionality
*
* \param[in]    pServiceConfig  Pointer to structure that contains server
*                               configuration information.
*
* \return       None
************************************************************************************/
void Tpms_Start(tpmConfig_t *pServiceConfig)
{
    uint8_t mClientId = 0U;
    uint8_t properties = 0U;
    uint16_t valueLength = 0U;

    /* Reset all slots for valid subscribers */
    for (mClientId = 0; mClientId < pServiceConfig->validSubscriberListSize; mClientId++)
    {
        pServiceConfig->aValidSubscriberList[mClientId] = FALSE;
    }

    /* Save reference to config */
    mpTpmConfig = pServiceConfig;

    /* Set TPMS Properties BIT1 - whether HMAC or CMAC is used for signing */
    (void)GattDb_ReadAttribute((uint16_t)value_tpms_properties, 1U, &properties, &valueLength);
#if defined(gTpmsUseHmac_d) && (gTpmsUseHmac_d == 1U)
    properties |= BIT1;
#else
    properties &= ~BIT1;
#endif
    (void)GattDb_WriteAttribute((uint16_t)value_tpms_properties, (uint16_t)sizeof(uint8_t), (const uint8_t*)&properties);
}

/*!**********************************************************************************
* \brief        Subscribes a GATT client to the TPM service
*
* \param[in]    pServiceConfig  Pointer to service configuration structure
* \param[in]    pClient  Client Id in Device DB.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Tpms_Subscribe(tpmConfig_t* pServiceConfig, deviceId_t clientDeviceId)
{
    bleResult_t result = gBleSuccess_c;

    if (clientDeviceId >= pServiceConfig->validSubscriberListSize)
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        pServiceConfig->aValidSubscriberList[clientDeviceId] = TRUE;
    }

    return result;
}

/*!**********************************************************************************
* \brief        Unsubscribes a GATT client from the TPM service
*
* \param[in]    pServiceConfig  Pointer to service configuration structure
* \param[in]    pClient  Client Id in Device DB.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t Tpms_Unsubscribe(tpmConfig_t* pServiceConfig, deviceId_t clientDeviceId)
{
    bleResult_t result = gBleSuccess_c;

    if (clientDeviceId >= pServiceConfig->validSubscriberListSize)
    {
        result = gBleInvalidParameter_c;
    }
    else
    {
        pServiceConfig->aValidSubscriberList[clientDeviceId] = FALSE;
    }

    return result;
}

/*!**********************************************************************************
* \brief        Returns advertising duration, either the default value or as configured
*               in the GATT database by the Primary Monitor.
*
* \return       Advertising duration in units of 10ms.
************************************************************************************/
uint8_t Tpms_GetAdvDuration(void)
{
    uint16_t length;
    uint8_t value[gTpmsDutyCycleCharValueLength_c] = {0U};

    (void)GattDb_ReadAttribute((uint16_t)value_tpms_monitoring_duty_cycle,
                               gTpmsDutyCycleCharValueLength_c, value, &length);

    return value[0];
}

/*!**********************************************************************************
* \brief        Returns standby duration, either the default value or as configured
*               in the GATT database by the Primary Monitor.
*
* \param[in]    state   State of the sensor.
*
* \return       Standby duration in seconds.
************************************************************************************/
uint16_t Tpms_GetStandbyDuration(uint8_t state)
{
    uint16_t length;
    uint8_t value[gTpmsDutyCycleCharValueLength_c] = {0U};
    uint16_t duration = 0U;

    (void)GattDb_ReadAttribute((uint16_t)value_tpms_monitoring_duty_cycle,
                               gTpmsDutyCycleCharValueLength_c,value, &length);

    switch (state)
    {
        case (uint8_t)gParkedState_c:
        {
            duration = Utils_ExtractTwoByteValue(&value[TPMS_DUTY_CYCLE_OFFSET_PARKED]);
        }
        break;

        case (uint8_t)gEarlyDrivingState_c:
        {
            duration = Utils_ExtractTwoByteValue(&value[TPMS_DUTY_CYCLE_OFFSET_EARLY_DRIVING]);
        }
        break;

        case (uint8_t)gDrivingState_c:
        {
            duration = Utils_ExtractTwoByteValue(&value[TPMS_DUTY_CYCLE_OFFSET_DRIVING]);
        }
        break;

        case (uint8_t)gEarlyParkedState_c:
        {
            duration = Utils_ExtractTwoByteValue(&value[TPMS_DUTY_CYCLE_OFFSET_EARLY_PARKED]);
        }
        break;

        default:
        {
            ; /* No change */
        }
        break;
    }

    return duration;
}

/*!**********************************************************************************
* \brief        Updates TMPS advertising data, including Sequence Number and MAC.
*
* \param[in/out]    pData            Pointer to array holding advertising data.
* \param[in]        pSensorReadData  Pointer to struct holding data read from sensors.
*
* \return       None
************************************************************************************/
void Tpms_UpdateAdvData(uint8_t *pData, tpmsSensorReadData_t *pSensorReadData)
{
    if ((pData != NULL) && (pSensorReadData != NULL))
    {
        uint8_t aKey[gTpmsSigningKeyLength_c] = {0U}, hmac_output[32] = {0U};
        uint8_t aKeyReversed[gTpmsSigningKeyLength_c] = {0U};
        uint16_t length = 0U;
        uint8_t position = 0U;

        /* Update sensor data in the GATT database and AD data */
        (void)GattDb_WriteAttribute((uint16_t)value_tire_pressure, (uint16_t)sizeof(pSensorReadData->tirePressure), (const uint8_t*)&pSensorReadData->tirePressure);
        FLib_MemCpy(&pData[TPMS_AD_OFFSET_PRESSURE], (uint8_t*)&pSensorReadData->tirePressure, 2U);
        FLib_MemCpy(&pData[TPMS_AD_OFFSET_PRESSURE_ACC], (uint8_t*)&pSensorReadData->tirePressureAccuracy, 1U);
        (void)GattDb_WriteAttribute((uint16_t)value_tire_temperature, sizeof(pSensorReadData->tireTemperature), (const uint8_t*)&pSensorReadData->tireTemperature);
        FLib_MemCpy(&pData[TPMS_AD_OFFSET_TEMPERATURE], (uint8_t*)&pSensorReadData->tireTemperature, 1U);
        FLib_MemCpy(&pData[TPMS_AD_OFFSET_TEMP_ACC], (uint8_t*)&pSensorReadData->tireTemperatureAccuracy, 1U);

        /* Update position */
        (void)GattDb_ReadAttribute((uint16_t)value_tpms_position, 1U, &position, &length);
        FLib_MemCpy(&pData[TPMS_AD_OFFSET_POSITION], (uint8_t*)&position, 1U);

        /* Increment sequence number */
        /* Sequence number shall not roll over while the TPMS sensor uses the same signing key
           To avoid the rollover, the TPMS Sensor shall change the value of the signing key
           (i.e., the value of the TPMS Signing Key characteristic is modified (see Section 3.7 in [1]) and reset the SEQ to 0. */
        if (mSeqNum < UINT32_MAX)
        {
            mSeqNum++;
        }
        else
        {
            /* Function also sets mSeqNum to 0 */
            Tpms_UpdateSigningKey(mpTpmConfig);
        }
        FLib_MemCpy(&pData[TPMS_AD_OFFSET_SEQNUM], (uint8_t*)&mSeqNum, 4U);
     
        /* Compute and add the MAC */
        (void)GattDb_ReadAttribute((uint16_t)value_tpms_signing_key, gTpmsSigningKeyLength_c, aKey, &length);
        FLib_MemCpyReverseOrder(aKeyReversed, aKey, gTpmsSigningKeyLength_c);
    #if defined(gTpmsUseHmac_d) && (gTpmsUseHmac_d == 1U)
        HMAC_SHA256(aKeyReversed, gTpmsSigningKeyLength_c, pData, TPMS_AD_OFFSET_MAC, (uint8_t*)&hmac_output);
    #else
        AES_128_CMAC(pData, TPMS_AD_OFFSET_MAC, aKeyReversed, (uint8_t*)&hmac_output);
    #endif
        FLib_MemCpyReverseOrder(&pData[TPMS_AD_OFFSET_MAC], &hmac_output, 4U);
    }
}

/*!**********************************************************************************
* \brief        Handles an ATT write from the peer. Checks if the peer is the Primary
*               Monitor and acts accordingly.
*
* \return       None.
************************************************************************************/
void Tpms_HandleAttributeWritten(deviceId_t deviceId, gattServerEvent_t* pServerEvent)
{
    uint8_t nvmIndex = 0U;
    bool_t isBonded = FALSE;
    uint8_t errorCode;

    (void)Gap_CheckIfBonded(deviceId, &isBonded, &nvmIndex);

    /* Only the Primary Monitor is allowed to write into the GATT database */
    if (isBonded)
    {
        if (isValidAttributeValue(pServerEvent) == TRUE)
        {
            (void)GattDb_WriteAttribute(pServerEvent->eventData.attributeWrittenEvent.handle,
                                  pServerEvent->eventData.attributeWrittenEvent.cValueLength,
                                  pServerEvent->eventData.attributeWrittenEvent.aValue);
            errorCode = (uint8_t)gAttErrCodeNoError_c;

            /* Seq may be reset to zero when the signing key changes */
            if (pServerEvent->eventData.attributeWrittenEvent.handle == (uint16_t)value_tpms_signing_key)
            {
                mSeqNum = 0U;
            }
        }
        else
        {
            if (pServerEvent->eventData.attributeWrittenEvent.handle == (uint16_t)value_tpms_monitoring_duty_cycle)
            {
                errorCode = (uint8_t)gAttErrCodeOutOfRange_c;
            }
            else
            {
                errorCode = (uint8_t)gAttErrCodeWriteRequestRejected_c;
            }
        }
    }
    else
    {
        errorCode = (uint8_t)gAttErrCodeWriteRequestRejected_c;
    }

   (void)GattServer_SendAttributeWrittenStatus(deviceId,
                                          pServerEvent->eventData.attributeWrittenEvent.handle,
                                          errorCode);
}

/*!**********************************************************************************
* \brief        Handles an ATT read from the peer. Checks if the peer is the Primary
*               Monitor and acts accordingly.
*
* \return       None.
************************************************************************************/
void Tpms_HandleAttributeRead(deviceId_t deviceId, gattServerEvent_t* pServerEvent)
{
    uint8_t errorCode = (uint8_t)gAttErrCodeNoError_c;

    if (pServerEvent->eventData.attributeReadEvent.handle == (uint16_t)value_tpms_signing_key)
    {
        uint8_t nvmIndex = 0U;
        bool_t isBonded = FALSE;

        (void)Gap_CheckIfBonded(deviceId, &isBonded, &nvmIndex);

        /* Only the Primary Monitor is allowed to read the TPMS Signing Key */
        if (!isBonded)
        {
            errorCode = (uint8_t)gAttErrCodeInsufficientAuthorization_c;
        }
    }

   (void)GattServer_SendAttributeReadStatus(deviceId,
                                            pServerEvent->eventData.attributeReadEvent.handle,
                                            errorCode);
}

/*!**********************************************************************************
* \brief        Sends Tire Pressure notifications to clients.
*
* \param[in]    pServiceConfig   Pointer to structure that contains server
*                                configuration information.
* \param[in]    pSensorReadData  Pointer to struct holding data read from sensors.
*
* \return       None.
************************************************************************************/
void Tpms_NotifyTirePressure(tpmConfig_t *pServiceConfig, tpmsSensorReadData_t *pSensorReadData)
{
    if ((pServiceConfig != NULL) && (pSensorReadData != NULL))
    {
        uint8_t mClientId = 0U;
        bool_t    isNotifActive = FALSE;

        /* Overwrite value from GATT DB */
        (void)GattDb_WriteAttribute((uint16_t)value_tire_pressure, 2U, (uint8_t*)&pSensorReadData->tirePressure);
        (void)GattDb_WriteAttribute((uint16_t)value_tire_temperature, 2U, (uint8_t*)&pSensorReadData->tireTemperature);

        /* Send to all subscribed peers */
        for (mClientId = 0; mClientId < pServiceConfig->validSubscriberListSize; mClientId++)
        {
            if (pServiceConfig->aValidSubscriberList[mClientId] == TRUE)
            {
                if (gBleSuccess_c == Gap_CheckNotificationStatus
                    (mClientId, (uint16_t)cccd_tire_pressure, &isNotifActive) &&
                    TRUE == isNotifActive)
                {
                    (void)GattServer_SendNotification(mClientId, (uint16_t)value_tire_pressure);
                }
                
                if (gBleSuccess_c == Gap_CheckNotificationStatus
                    (mClientId, (uint16_t)cccd_tire_temperature, &isNotifActive) &&
                    TRUE == isNotifActive)
                {
                    (void)GattServer_SendNotification(mClientId, (uint16_t)value_tire_temperature);
                }
            }   
        }
    }
}

/*!**********************************************************************************
* \brief        Update Signing Key characteristic.
*
* \param[in]    pServiceConfig  Pointer to structure that contains server
*                               configuration information.
*
* \return       None.
************************************************************************************/
void Tpms_UpdateSigningKey(tpmConfig_t *pServiceConfig)
{
    uint16_t length;
    uint8_t aCurrentKey[gTpmsSigningKeyLength_c] = {0U};
    uint8_t aNewKey[gTpmsSigningKeyLength_c] = {TPMS_SIGNING_KEY};
    
    uint8_t mClientId = 0U;
    bool_t  isIndActive = FALSE;

    (void)GattDb_ReadAttribute((uint16_t)value_tpms_signing_key, gTpmsSigningKeyLength_c, (uint8_t *)aCurrentKey, &length);

    if(FLib_MemCmp(aCurrentKey, aNewKey, gTpmsSigningKeyLength_c))
    {
        FLib_MemCpyReverseOrder(aNewKey, aCurrentKey, gTpmsSigningKeyLength_c);
    }
    
    (void)GattDb_WriteAttribute((uint16_t)value_tpms_signing_key, gTpmsSigningKeyLength_c, aNewKey);
    
    /* Send to all subscribed peers */
    for (mClientId = 0; mClientId < pServiceConfig->validSubscriberListSize; mClientId++)
    {
        if (pServiceConfig->aValidSubscriberList[mClientId] == TRUE)
        {
            if (gBleSuccess_c == Gap_CheckIndicationStatus
                (mClientId, (uint16_t)cccd_tpms_signing_key, &isIndActive) &&
                TRUE == isIndActive)
            {
                (void)GattServer_SendIndication(mClientId, (uint16_t)value_tpms_signing_key);
            }
        }
    }

    mSeqNum = 0U;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*!**********************************************************************************
* \brief        Checks if the value written by the TPMS Monitor is valid.
*
* \return       TRUE if valid, FALSE otherwise.
************************************************************************************/
static bool_t isValidAttributeValue(gattServerEvent_t* pServerEvent)
{
    bool_t bValid = TRUE;

    switch (pServerEvent->eventData.attributeWrittenEvent.handle)
    {
        case (uint16_t)value_tpms_monitoring_duty_cycle:
        {
            uint8_t advStateDuration =
                pServerEvent->eventData.attributeWrittenEvent.aValue[0];
            uint16_t advStandbyDurationParkedState =
                Utils_ExtractTwoByteValue(&pServerEvent->eventData.attributeWrittenEvent.aValue[1]);
            uint16_t advStandbyDurationEarlyDrivingState =
                Utils_ExtractTwoByteValue(&pServerEvent->eventData.attributeWrittenEvent.aValue[3]);
            uint16_t advStandbyDurationDrivingState =
                Utils_ExtractTwoByteValue(&pServerEvent->eventData.attributeWrittenEvent.aValue[5]);
            uint16_t advStandbyDurationEarlyParkedState =
                Utils_ExtractTwoByteValue(&pServerEvent->eventData.attributeWrittenEvent.aValue[7]);
            uint8_t earlyDrivingStateDuration =
                pServerEvent->eventData.attributeWrittenEvent.aValue[9];
            uint8_t earlyDrivingPrimaryMonitorPairingStateDuration =
                pServerEvent->eventData.attributeWrittenEvent.aValue[10];
            uint8_t earlyParkedStateDuration =
                pServerEvent->eventData.attributeWrittenEvent.aValue[11];
            uint8_t serviceStateDuration =
                pServerEvent->eventData.attributeWrittenEvent.aValue[12];
            if ((advStateDuration < 2U) || (advStateDuration > 200U) ||          /* valid range: 2-200 (10ms increments) */
                (advStandbyDurationParkedState > 3600U)  ||                      /* valid range: 0-3600s */
                (advStandbyDurationEarlyDrivingState > 3600U)  ||                /* valid range: 0-3600s */
                (advStandbyDurationDrivingState > 3600U)  ||                     /* valid range: 0-3600s */
                (advStandbyDurationEarlyParkedState > 3600U)  ||                 /* valid range: 0-3600s */
                (earlyDrivingStateDuration > 10U) ||                             /* valid range: 0-10m */
                (earlyDrivingPrimaryMonitorPairingStateDuration > 10U) ||        /* valid range: 0-10m */
                (earlyParkedStateDuration > 10U) ||                              /* valid range: 0-10m */
                (serviceStateDuration < 1U)                                      /* valid range: 1-255s */
                )
            {
                bValid = FALSE;
            }
        }
        break;

        case (uint16_t)value_tpms_position:
        {
            uint8_t position =
                pServerEvent->eventData.attributeWrittenEvent.aValue[0];

            if ((position < 1U) || (position > 30U)) /* valid range: 1-30 */
            {
                bValid = FALSE;
            }
        }
        break;

        default:
        {
            ; /* MISRA */
        }
        break;
    }

    return bValid;
}
/*! *********************************************************************************
 * @}
 ********************************************************************************** */
