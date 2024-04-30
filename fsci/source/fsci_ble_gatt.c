/*! *********************************************************************************
* \addtogroup FSCI_BLE
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2023 NXP
*
*
* \file
*
* This is a source file for FSCI BLE management.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "fsci_ble_gatt.h"

#if gFsciBleHost_d
    #include "host_ble.h"
#endif /* gFsciBleHost_d */


#if gFsciIncluded_c && gFsciBleGattLayerEnabled_d

/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

#if gFsciBleHost_d
    /* Macro used for saving the out parameters pointers of the GATT functions */
    #define fsciBleGattSaveOutParams(pOutParam) \
            fsciBleGattOutParams.pParam = (uint8_t*)pOutParam

    /* Macro used for restoring the out parameters pointers of the GATT functions */
    #define fsciBleGattRestoreOutParams()   \
            &fsciBleGattOutParams

    /* Macro used for setting the out parameters pointers of the GATT
    functions to NULL */
    #define fsciBleGattCleanOutParams() \
            fsciBleGattOutParams.pParam = NULL
#endif /* gFsciBleHost_d */


#if gFsciBleBBox_d
    #define bFsciBleGattClientAllocatedInfo             TRUE
#endif /* gFsciBleBBox_d */

#define fsciBleGattCallApiFunction(apiFunction)         bleResult_t result = (apiFunction); \
                                                        fsciBleGattStatusMonitor(result)
#define fsciBleGattMonitorOutParams(functionId, ...)    if(gBleSuccess_c == result)                  \
                                                        {                                            \
                                                            FsciEvtMonitor(functionId, __VA_ARGS__); \
                                                        }

#define SizeOfArray(a) (sizeof(a)/sizeof(a[0]))

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/*! Structure that keeps callbacks registered by application or FSCI in GATT. */
typedef struct fsciBleGattCallbacks_tag
{
    gattClientProcedureCallback_t                   clientProcedureCallback;        /* GATT Client procedure callback. */
    gattClientNotificationCallback_t                clientNotificationCallback;     /* GATT Client notification callback. */
    gattClientIndicationCallback_t                  clientIndicationCallback;       /* GATT Client indication callback. */
    gattServerCallback_t                            serverCallback;                 /* GATT Server callback. */
#if defined(gBLE52_d) && (gBLE52_d == TRUE)
    gattClientMultipleValueNotificationCallback_t   clientMultipleValueNtfCallback; /* GATT Client indication callback. */
#if defined(gEATT_d) && (gEATT_d == TRUE)
    gattClientEnhancedProcedureCallback_t       clientEnhancedProcedureCallback;        /* GATT Client Enhanced procedure callback. */
    gattClientEnhancedNotificationCallback_t    clientEnhancedNotificationCallback;     /* GATT Client Enhanced notification callback. */
    gattClientEnhancedIndicationCallback_t      clientEnhancedIndicationCallback;       /* GATT Client Enhanced indication callback. */
    gattServerEnhancedCallback_t                serverEnhancedCallback;                 /* GATT Enhanced Server callback. */
    gattClientEnhancedMultipleValueNotificationCallback_t   clientEnhancedMultipleValueNtfCallback; /* GATT Client Enhanced indication callback. */
#endif
#endif

}fsciBleGattCallbacks_t;

#if gFsciBleHost_d
    /* Structure used for keeping the out parameters pointers of the GATT
     functions */
    typedef struct fsciBleGattOutParams_tag
    {
        uint8_t*    pParam;
    }fsciBleGattOutParams_t;

    /* Structure used for keeping the out parameters pointers of the
    Gatt_GetMtu function */
    typedef struct fsciBleGattGetMtuOutParams_tag
    {
        uint16_t* pMtu;
    }fsciBleGattGetMtuOutParams_t;
#endif /* gFsciBleHost_d */

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
/* Structure used to keep the handle, length, value tuple */
typedef struct
{
    uint16_t  attributeHandle;
    uint16_t  attributeLength;
    uint8_t   attributeValue[1];
} fsciBleGattMultipleHandleValNtfParams_t;
#endif /* gBLE52_d */

/*! This is the type definition for Gatt op code handler pointers. */
typedef void (*pfGattOpCodeHandler_t)
(
    uint8_t     *pBuffer,
    uint32_t    fsciInterfaceId
);

/*! This is the type definition for Gatt Client Procedure handler pointers. */
typedef void (*pfGattClientProcEvtHandler_t)
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

void fsciBleGattClientProcedureCallback(deviceId_t deviceId, gattProcedureType_t procedureType, gattProcedureResult_t procedureResult, bleResult_t error);
void fsciBleGattClientNotificationCallback(deviceId_t deviceId, uint16_t characteristicValueHandle, uint8_t* aValue, uint16_t valueLength);
void fsciBleGattClientIndicationCallback(deviceId_t deviceId, uint16_t characteristicValueHandle, uint8_t* aValue, uint16_t valueLength);
void fsciBleGattServerCallback(deviceId_t deviceId, gattServerEvent_t* pServerEvent);
#if defined(gBLE52_d) && (gBLE52_d == TRUE)
void fsciBleGattClientMultipleValueNotificationCallback(deviceId_t  deviceId, uint8_t* aHandleLenValue, uint32_t totalLength);
#if (defined gEATT_d) && (gEATT_d == TRUE)
void fsciBleGattClientEnhancedProcedureCallback(deviceId_t deviceId, bearerId_t bearerId, gattProcedureType_t procedureType, gattProcedureResult_t procedureResult, bleResult_t error);
void fsciBleGattClientEnhancedNotificationCallback(deviceId_t deviceId, bearerId_t bearerId, uint16_t characteristicValueHandle, uint8_t* aValue, uint16_t valueLength);
void fsciBleGattClientEnhancedIndicationCallback(deviceId_t deviceId, bearerId_t bearerId, uint16_t characteristicValueHandle, uint8_t* aValue, uint16_t valueLength);
void fsciBleGattServerEnhancedCallback(deviceId_t deviceId, bearerId_t bearerId, gattServerEvent_t* pServerEvent);
void fsciBleGattClientEnhancedMultipleValueNotificationCallback(deviceId_t  deviceId, bearerId_t bearerId, uint8_t* aHandleLenValue, uint32_t totalLength);
#endif
#endif

#if gFsciBleBBox_d || gFsciBleTest_d
static void HandleGattCmdInitOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdGetMtuOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientInitOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientResetProceduresOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientRegisterProcedureCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientRegisterNotificationCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientRegisterIndicationCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientExchangeMtuOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientDiscoverAllPrimaryServicesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientDiscoverPrimaryServicesByUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientFindIncludedServicesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientDiscoverAllCharacteristicsOfServiceOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientDiscoverCharacteristicOfServiceByUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientDiscoverAllCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientReadCharacteristicValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientReadUsingCharacteristicUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientReadMultipleCharacteristicValuesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientWriteCharacteristicValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientReadCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientWriteCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
#if defined(gBLE51_d) && (gBLE51_d == TRUE) && \
    defined(gGattCaching_d) && (gGattCaching_d == TRUE)
static void HandleGattCmdClientGetDatabaseHashOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
#endif /* gBLE51_d && gGattCaching_d */
static void HandleGattCmdServerInitOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerRegisterCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerRegisterHandlesForWriteNotificationsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerRegisterHandlesForReadNotificationsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerUnregisterHandlesForWriteNotificationsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerUnregisterHandlesForReadNotificationsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerSendAttributeWrittenStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerSendAttributeReadStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerSendNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerSendIndicationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerSendInstantValueNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerSendInstantValueIndicationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerRegisterUniqueHandlesForNotificationsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
#if defined(gBLE52_d) && (gBLE52_d == TRUE)
static void HandleGattCmdClientRegisterMultipleValueNotificationCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerSendMultipleHandleValueNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientReadMultipleVariableCharacteristicValuesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
#if defined(gEATT_d) && (gEATT_d == TRUE)
static void HandleGattCmdClientRegisterEnhancedProcedureCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientRegisterEnhancedNotificationCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientRegisterEnhancedIndicationCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientRegisterEnhancedMultipleValueNotificationCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedDiscoverAllPrimaryServicesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedDiscoverPrimaryServicesByUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedFindIncludedServicesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedDiscoverAllCharacteristicsOfServiceOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedDiscoverCharacteristicOfServiceByUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedDiscoverAllCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedReadCharacteristicValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedReadUsingCharacteristicUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedReadMultipleCharacteristicValuesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedWriteCharacteristicValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedReadCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedWriteCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerRegisterEnhancedCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerEnhancedSendAttributeWrittenStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerEnhancedSendAttributeReadStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerEnhancedSendNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerEnhancedSendIndicationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerEnhancedSendInstantValueNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerEnhancedSendInstantValueIndicationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdServerEnhancedSendMultipleHandleValueNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattCmdClientEnhancedReadMultipleVariableCharacteristicValuesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
#endif /* gEATT_d */
#endif /* gBLE52_d */
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
static void HandleGattStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtGetMtuOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureExchangeMtuOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureDiscoverAllPrimaryServicesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureDiscoverPrimaryServicesByUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureFindIncludedServicesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureDiscoverAllCharacteristicsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureDiscoverCharacteristicByUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureDiscoverAllCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureReadCharacteristicValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureReadUsingCharacteristicUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureReadMultipleCharacteristicValuesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureWriteCharacteristicValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureReadCharacteristicDescriptorOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientProcedureWriteCharacteristicDescriptorOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtClientIndicationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtServerMtuChangedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtServerHandleValueConfirmationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtServerAttributeWrittenOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtServerCharacteristicCccdWrittenOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtServerAttributeWrittenWithoutResponseOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtServerErrorOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtServerLongCharacteristicWrittenOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
static void HandleGattEvtServerAttributeReadOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId);
#endif /* gFsciBleHost_d */

static void HandleGattProcExchangeMtu
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

static void HandleGattProcDiscoverAllPrimaryServices
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

static void HandleGattProcDiscoverPrimaryServicesByUuid
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

static void HandleGattProcFindIncludedServices
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

static void HandleGattProcDiscoverAllCharacteristics
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

static void HandleGattProcDiscoverCharacteristicByUuid
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

static void HandleGattProcDiscoverAllCharacteristicDescriptors
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

static void HandleGattProcReadCharacteristicValue
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

static void HandleGattProcReadUsingCharacteristicUuid
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

static void HandleGattProcReadMultipleCharacteristicValues
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

static void HandleGattProcWriteCharacteristicValue
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

static void HandleGattProcReadCharacteristicDescriptor
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

static void HandleGattProcWriteCharacteristicDescriptor
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
static void HandleGattProcReadMultipleVarLengthCharValues
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
);
#endif /* gBLE52_d */

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

/* GATT callbacks structure initialized with FSCI empty static functions */
static fsciBleGattCallbacks_t fsciBleGattCallbacks =
{
    fsciBleGattClientProcedureCallback,
    fsciBleGattClientNotificationCallback,
    fsciBleGattClientIndicationCallback,
    fsciBleGattServerCallback,
#if defined(gBLE52_d) && (gBLE52_d == TRUE)
    fsciBleGattClientMultipleValueNotificationCallback,
#if (defined gEATT_d) && (gEATT_d == TRUE)
    fsciBleGattClientEnhancedProcedureCallback,
    fsciBleGattClientEnhancedNotificationCallback,
    fsciBleGattClientEnhancedIndicationCallback,
    fsciBleGattServerEnhancedCallback,
    fsciBleGattClientEnhancedMultipleValueNotificationCallback
#endif
#endif
};

/* Keeps the number of characteristics to be read by GattClient_ReadMultipleCharacteristicValues
function */
static uint8_t fsciBleGattClientNbOfCharacteristics = 0;

#if gFsciBleTest_d
    /* Indicates if FSCI for GATT is enabled or not */
    static bool_t bFsciBleGattEnabled               = FALSE;

    /* Indicates if the command was initiated by FSCI (to be not monitored) */
    static bool_t bFsciBleGattCmdInitiatedByFsci    = FALSE;

    /* Indicates if the client information was allocated by FSCI or just saved */
    static bool_t bFsciBleGattClientAllocatedInfo   = FALSE;
#endif /* gFsciBleTest_d */

#if gFsciBleHost_d
    /* Keeps out parameters pointers for Host - BBox functionality */
    static fsciBleGattOutParams_t fsciBleGattOutParams = {NULL};
#endif /* gFsciBleHost_d */

#if gFsciBleBBox_d || gFsciBleTest_d
    /* Flag that indicates if the last request was a GATT Client request with
    out parameters (this procedures are asynchronous) */
    static bool_t bGattClientRequestWithOutParameters = FALSE;
#endif /* gFsciBleBBox_d || gFsciBleTest_d */
  
#if gFsciBleBBox_d || gFsciBleTest_d
static const pfGattOpCodeHandler_t maGattCmdOpCodeHandlers[]=
{
    NULL,
    HandleGattCmdInitOpCode,                                                            /* = 0x01, gBleGattCmdInitOpCode_c*/
    HandleGattCmdGetMtuOpCode,                                                          /* = 0x02, gBleGattCmdGetMtuOpCode_c */
    HandleGattCmdClientInitOpCode,                                                      /* = 0x03, gBleGattCmdClientInitOpCode_c */
    HandleGattCmdClientResetProceduresOpCode,                                           /* = 0x04, gBleGattCmdClientResetProceduresOpCode_c */
    HandleGattCmdClientRegisterProcedureCallbackOpCode,                                 /* = 0x05, gBleGattCmdClientRegisterProcedureCallbackOpCode_c */
    HandleGattCmdClientRegisterNotificationCallbackOpCode,                              /* = 0x06, gBleGattCmdClientRegisterNotificationCallbackOpCode_c */
    HandleGattCmdClientRegisterIndicationCallbackOpCode,                                /* = 0x07, gBleGattCmdClientRegisterIndicationCallbackOpCode_c */
    HandleGattCmdClientExchangeMtuOpCode,                                               /* = 0x08, gBleGattCmdClientExchangeMtuOpCode_c */
    HandleGattCmdClientDiscoverAllPrimaryServicesOpCode,                                /* = 0x09, gBleGattCmdClientDiscoverAllPrimaryServicesOpCode_c */
    HandleGattCmdClientDiscoverPrimaryServicesByUuidOpCode,                             /* = 0x0A, gBleGattCmdClientDiscoverPrimaryServicesByUuidOpCode_c */
    HandleGattCmdClientFindIncludedServicesOpCode,                                      /* = 0x0B, gBleGattCmdClientFindIncludedServicesOpCode_c */
    HandleGattCmdClientDiscoverAllCharacteristicsOfServiceOpCode,                       /* = 0x0C, gBleGattCmdClientDiscoverAllCharacteristicsOfServiceOpCode_c */
    HandleGattCmdClientDiscoverCharacteristicOfServiceByUuidOpCode,                     /* = 0x0D, gBleGattCmdClientDiscoverCharacteristicOfServiceByUuidOpCode_c */
    HandleGattCmdClientDiscoverAllCharacteristicDescriptorsOpCode,                      /* = 0x0E, gBleGattCmdClientDiscoverAllCharacteristicDescriptorsOpCode_c */
    HandleGattCmdClientReadCharacteristicValueOpCode,                                   /* = 0x0F, gBleGattCmdClientReadCharacteristicValueOpCode_c */
    HandleGattCmdClientReadUsingCharacteristicUuidOpCode,                               /* = 0x10, gBleGattCmdClientReadUsingCharacteristicUuidOpCode_c */
    HandleGattCmdClientReadMultipleCharacteristicValuesOpCode,                          /* = 0x11, gBleGattCmdClientReadMultipleCharacteristicValuesOpCode_c */
    HandleGattCmdClientWriteCharacteristicValueOpCode,                                  /* = 0x12, gBleGattCmdClientWriteCharacteristicValueOpCode_c */
    HandleGattCmdClientReadCharacteristicDescriptorsOpCode,                             /* = 0x13, gBleGattCmdClientReadCharacteristicDescriptorsOpCode_c */
    HandleGattCmdClientWriteCharacteristicDescriptorsOpCode,                            /* = 0x14, gBleGattCmdClientWriteCharacteristicDescriptorsOpCode_c */
    HandleGattCmdServerInitOpCode,                                                      /* = 0x15, gBleGattCmdServerInitOpCode_c */
    HandleGattCmdServerRegisterCallbackOpCode,                                          /* = 0x16, gBleGattCmdServerRegisterCallbackOpCode_c */
    HandleGattCmdServerRegisterHandlesForWriteNotificationsOpCode,                      /* = 0x17, gBleGattCmdServerRegisterHandlesForWriteNotificationsOpCode_c */
    HandleGattCmdServerSendAttributeWrittenStatusOpCode,                                /* = 0x18, gBleGattCmdServerSendAttributeWrittenStatusOpCode_c */
    HandleGattCmdServerSendNotificationOpCode,                                          /* = 0x19, gBleGattCmdServerSendNotificationOpCode_c */
    HandleGattCmdServerSendIndicationOpCode,                                            /* = 0x1A, gBleGattCmdServerSendIndicationOpCode_c */
    HandleGattCmdServerSendInstantValueNotificationOpCode,                              /* = 0x1B, gBleGattCmdServerSendInstantValueNotificationOpCode_c */
    HandleGattCmdServerSendInstantValueIndicationOpCode,                                /* = 0x1C, gBleGattCmdServerSendInstantValueIndicationOpCode_c */
    HandleGattCmdServerRegisterHandlesForReadNotificationsOpCode,                       /* = 0x1D, gBleGattCmdServerRegisterHandlesForReadNotificationsOpCode_c */
    HandleGattCmdServerSendAttributeReadStatusOpCode,                                   /* = 0x1E, gBleGattCmdServerSendAttributeReadStatusOpCode_c */
    HandleGattCmdServerRegisterUniqueHandlesForNotificationsOpCode,                     /* = 0x1F, gBleGattCmdServerRegisterUniqueHandlesForNotificationsOpCode_c */
    HandleGattCmdServerUnregisterHandlesForWriteNotificationsOpCode,                    /* = 0x20, gBleGattCmdServerUnregisterHandlesForWriteNotificationsOpCode_c */
    HandleGattCmdServerUnregisterHandlesForReadNotificationsOpCode,                     /* = 0x21, gBleGattCmdServerUnregisterHandlesForReadNotificationsOpCode_c */
#if defined(gBLE52_d) && (gBLE52_d == TRUE)
    HandleGattCmdServerSendMultipleHandleValueNotificationOpCode,                       /* = 0x22, gBleGattCmdServerSendMultipleHandleValueNotificationOpCode_c */
    HandleGattCmdClientRegisterMultipleValueNotificationCallbackOpCode,                 /* = 0x23, gBleGattCmdClientRegisterMultipleValueNotificationCallbackOpCode_c */
    HandleGattCmdClientReadMultipleVariableCharacteristicValuesOpCode,                  /* = 0x24, gBleGattCmdClientReadMultipleVariableCharacteristicValuesOpCode_c */
#if defined(gEATT_d) && (gEATT_d == TRUE)
    HandleGattCmdClientRegisterEnhancedProcedureCallbackOpCode,                         /* = 0x25, gBleGattCmdClientRegisterEnhancedProcedureCallbackOpCode_c */
    HandleGattCmdClientRegisterEnhancedNotificationCallbackOpCode,                      /* = 0x26, gBleGattCmdClientRegisterEnhancedNotificationCallbackOpCode_c */
    HandleGattCmdClientRegisterEnhancedIndicationCallbackOpCode,                        /* = 0x27, gBleGattCmdClientRegisterEnhancedIndicationCallbackOpCode_c */
    HandleGattCmdClientRegisterEnhancedMultipleValueNotificationCallbackOpCode,         /* = 0x28, gBleGattCmdClientRegisterEnhancedMultipleValueNotificationCallbackOpCode_c */
    HandleGattCmdClientEnhancedDiscoverAllPrimaryServicesOpCode,                        /* = 0x29, gBleGattCmdClientEnhancedDiscoverAllPrimaryServicesOpCode_c */
    HandleGattCmdClientEnhancedDiscoverPrimaryServicesByUuidOpCode,                     /* = 0x2A, gBleGattCmdClientEnhancedDiscoverPrimaryServicesByUuidOpCode_c */
    HandleGattCmdClientEnhancedFindIncludedServicesOpCode,                              /* = 0x2B, gBleGattCmdClientEnhancedFindIncludedServicesOpCode_c */
    HandleGattCmdClientEnhancedDiscoverAllCharacteristicsOfServiceOpCode,               /* = 0x2C, gBleGattCmdClientEnhancedDiscoverAllCharacteristicsOfServiceOpCode_c */
    HandleGattCmdClientEnhancedDiscoverCharacteristicOfServiceByUuidOpCode,             /* = 0x2D, gBleGattCmdClientEnhancedDiscoverCharacteristicOfServiceByUuidOpCode_c */
    HandleGattCmdClientEnhancedDiscoverAllCharacteristicDescriptorsOpCode,              /* = 0x2E, gBleGattCmdClientEnhancedDiscoverAllCharacteristicDescriptorsOpCode_c */
    HandleGattCmdClientEnhancedReadCharacteristicValueOpCode,                           /* = 0x2F, gBleGattCmdClientEnhancedReadCharacteristicValueOpCode_c */
    HandleGattCmdClientEnhancedReadUsingCharacteristicUuidOpCode,                       /* = 0x30, gBleGattCmdClientEnhancedReadUsingCharacteristicUuidOpCode_c */
    HandleGattCmdClientEnhancedReadMultipleCharacteristicValuesOpCode,                  /* = 0x31, gBleGattCmdClientEnhancedReadMultipleCharacteristicValuesOpCode_c */
    HandleGattCmdClientEnhancedWriteCharacteristicValueOpCode,                          /* = 0x32, gBleGattCmdClientEnhancedWriteCharacteristicValueOpCode_c */
    HandleGattCmdClientEnhancedReadCharacteristicDescriptorsOpCode,                     /* = 0x33, gBleGattCmdClientEnhancedReadCharacteristicDescriptorsOpCode_c */
    HandleGattCmdClientEnhancedWriteCharacteristicDescriptorsOpCode,                    /* = 0x34, gBleGattCmdClientEnhancedWriteCharacteristicDescriptorsOpCode_c */
    HandleGattCmdClientEnhancedReadMultipleVariableCharacteristicValuesOpCode,          /* = 0x35, gBleGattCmdClientEnhancedReadMultipleVariableCharacteristicValuesOpCode_c */
    HandleGattCmdServerRegisterEnhancedCallbackOpCode,                                  /* = 0x36, gBleGattCmdServerRegisterEnhancedCallbackOpCode_c */
    HandleGattCmdServerEnhancedSendAttributeWrittenStatusOpCode,                        /* = 0x37, gBleGattCmdServerEnhancedSendAttributeWrittenStatusOpCode_c */
    HandleGattCmdServerEnhancedSendNotificationOpCode,                                  /* = 0x38, gBleGattCmdServerEnhancedSendNotificationOpCode_c */
    HandleGattCmdServerEnhancedSendIndicationOpCode,                                    /* = 0x39, gBleGattCmdServerEnhancedSendIndicationOpCode_c */
    HandleGattCmdServerEnhancedSendInstantValueNotificationOpCode,                      /* = 0x3A, gBleGattCmdServerEnhancedSendInstantValueNotificationOpCode_c */
    HandleGattCmdServerEnhancedSendInstantValueIndicationOpCode,                        /* = 0x3B, gBleGattCmdServerEnhancedSendInstantValueIndicationOpCode_c */
    HandleGattCmdServerEnhancedSendAttributeReadStatusOpCode,                           /* = 0x3C, gBleGattCmdServerEnhancedSendAttributeReadStatusOpCode_c */
    HandleGattCmdServerEnhancedSendMultipleHandleValueNotificationOpCode,               /* = 0x3D, gBleGattCmdServerEnhancedSendMultipleHandleValueNotificationOpCode_c */
#else /* gEATT_d */
    NULL,                                                                               /* reserved: 0x25 */
    NULL,                                                                               /* reserved: 0x26 */
    NULL,                                                                               /* reserved: 0x27 */
    NULL,                                                                               /* reserved: 0x28 */
    NULL,                                                                               /* reserved: 0x29 */
    NULL,                                                                               /* reserved: 0x2A */
    NULL,                                                                               /* reserved: 0x2B */
    NULL,                                                                               /* reserved: 0x2C */
    NULL,                                                                               /* reserved: 0x2D */
    NULL,                                                                               /* reserved: 0x2E */
    NULL,                                                                               /* reserved: 0x2F */
    NULL,                                                                               /* reserved: 0x30 */
    NULL,                                                                               /* reserved: 0x31 */
    NULL,                                                                               /* reserved: 0x32 */
    NULL,                                                                               /* reserved: 0x33 */
    NULL,                                                                               /* reserved: 0x34 */
    NULL,                                                                               /* reserved: 0x35 */
    NULL,                                                                               /* reserved: 0x36 */
    NULL,                                                                               /* reserved: 0x37 */
    NULL,                                                                               /* reserved: 0x38 */
    NULL,                                                                               /* reserved: 0x39 */
    NULL,                                                                               /* reserved: 0x3A */
    NULL,                                                                               /* reserved: 0x3B */
    NULL,                                                                               /* reserved: 0x3C */
    NULL,                                                                               /* reserved: 0x3D */
#endif /* gEATT_d */
#else /* gBLE52_d */
    NULL,                                                                               /* reserved: 0x22 */
    NULL,                                                                               /* reserved: 0x23 */
    NULL,                                                                               /* reserved: 0x24 */
    NULL,                                                                               /* reserved: 0x25 */
    NULL,                                                                               /* reserved: 0x26 */
    NULL,                                                                               /* reserved: 0x27 */
    NULL,                                                                               /* reserved: 0x28 */
    NULL,                                                                               /* reserved: 0x29 */
    NULL,                                                                               /* reserved: 0x2A */
    NULL,                                                                               /* reserved: 0x2B */
    NULL,                                                                               /* reserved: 0x2C */
    NULL,                                                                               /* reserved: 0x2D */
    NULL,                                                                               /* reserved: 0x2E */
    NULL,                                                                               /* reserved: 0x2F */
    NULL,                                                                               /* reserved: 0x30 */
    NULL,                                                                               /* reserved: 0x31 */
    NULL,                                                                               /* reserved: 0x32 */
    NULL,                                                                               /* reserved: 0x33 */
    NULL,                                                                               /* reserved: 0x34 */
    NULL,                                                                               /* reserved: 0x35 */
    NULL,                                                                               /* reserved: 0x36 */
    NULL,                                                                               /* reserved: 0x37 */
    NULL,                                                                               /* reserved: 0x38 */
    NULL,                                                                               /* reserved: 0x39 */
    NULL,                                                                               /* reserved: 0x3A */
    NULL,                                                                               /* reserved: 0x3B */
    NULL,                                                                               /* reserved: 0x3C */
    NULL,                                                                               /* reserved: 0x3D */
#endif /* gBLE52_d */
#if defined(gBLE51_d) && (gBLE51_d == TRUE) && \
    defined(gGattCaching_d) && (gGattCaching_d == TRUE)
    HandleGattCmdClientGetDatabaseHashOpCode                                            /* = 0x3E, gBleGattCmdClientGetDatabaseHashOpCode_c */
#else /* gBLE51_d && gGattCaching_d */
    NULL                                                                                /* reserved: 0x3E */
#endif /* gBLE51_d && gGattCaching_d */
};
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
static const pfGattOpCodeHandler_t maGattEvtOpCodeHandlers[]=
{
    HandleGattStatusOpCode,                                                             /* = 0x80, gBleGattStatusOpCode_c */
    HandleGattEvtGetMtuOpCode,                                                          /* = 0x81, gBleGattEvtGetMtuOpCode_c */
    HandleGattEvtClientProcedureExchangeMtuOpCode,                                      /* = 0x82, gBleGattEvtClientProcedureExchangeMtuOpCode_c */
    HandleGattEvtClientProcedureDiscoverAllPrimaryServicesOpCode,                       /* = 0x83, gBleGattEvtClientProcedureDiscoverAllPrimaryServicesOpCode_c */
    HandleGattEvtClientProcedureDiscoverPrimaryServicesByUuidOpCode,                    /* = 0x84, gBleGattEvtClientProcedureDiscoverPrimaryServicesByUuidOpCode_c */
    HandleGattEvtClientProcedureFindIncludedServicesOpCode,                             /* = 0x85, gBleGattEvtClientProcedureFindIncludedServicesOpCode_c */
    HandleGattEvtClientProcedureDiscoverAllCharacteristicsOpCode,                       /* = 0x86, gBleGattEvtClientProcedureDiscoverAllCharacteristicsOpCode_c */
    HandleGattEvtClientProcedureDiscoverCharacteristicByUuidOpCode,                     /* = 0x87, gBleGattEvtClientProcedureDiscoverCharacteristicByUuidOpCode_c */
    HandleGattEvtClientProcedureDiscoverAllCharacteristicDescriptorsOpCode,             /* = 0x88, gBleGattEvtClientProcedureDiscoverAllCharacteristicDescriptorsOpCode_c */
    HandleGattEvtClientProcedureReadCharacteristicValueOpCode,                          /* = 0x89, gBleGattEvtClientProcedureReadCharacteristicValueOpCode_c */
    HandleGattEvtClientProcedureReadUsingCharacteristicUuidOpCode,                      /* = 0x8A, gBleGattEvtClientProcedureReadUsingCharacteristicUuidOpCode_c */
    HandleGattEvtClientProcedureReadMultipleCharacteristicValuesOpCode,                 /* = 0x8B, gBleGattEvtClientProcedureReadMultipleCharacteristicValuesOpCode_c */
    HandleGattEvtClientProcedureWriteCharacteristicValueOpCode,                         /* = 0x8C, gBleGattEvtClientProcedureWriteCharacteristicValueOpCode_c */
    HandleGattEvtClientProcedureReadCharacteristicDescriptorOpCode,                     /* = 0x8D, gBleGattEvtClientProcedureReadCharacteristicDescriptorOpCode_c */
    HandleGattEvtClientProcedureWriteCharacteristicDescriptorOpCode,                    /* = 0x8E, gBleGattEvtClientProcedureWriteCharacteristicDescriptorOpCode_c */
    HandleGattEvtClientNotificationOpCode,                                              /* = 0x8F, gBleGattEvtClientNotificationOpCode_c */
    HandleGattEvtClientIndicationOpCode,                                                /* = 0x90, gBleGattEvtClientIndicationOpCode_c */
    HandleGattEvtServerMtuChangedOpCode,                                                /* = 0x91, gBleGattEvtServerMtuChangedOpCode_c */
    HandleGattEvtServerHandleValueConfirmationOpCode,                                   /* = 0x92, gBleGattEvtServerHandleValueConfirmationOpCode_c */
    HandleGattEvtServerAttributeWrittenOpCode,                                          /* = 0x93, HandleGattEvtServerAttributeWrittenOpCode */
    HandleGattEvtServerCharacteristicCccdWrittenOpCode,                                 /* = 0x94, gBleGattEvtServerCharacteristicCccdWrittenOpCode_c */
    HandleGattEvtServerAttributeWrittenWithoutResponseOpCode,                           /* = 0x95, gBleGattEvtServerAttributeWrittenWithoutResponseOpCode_c */
    HandleGattEvtServerErrorOpCode,                                                     /* = 0x96, gBleGattEvtServerErrorOpCode_c */
    HandleGattEvtServerLongCharacteristicWrittenOpCode,                                 /* = 0x97, gBleGattEvtServerLongCharacteristicWrittenOpCode_c */
    HandleGattEvtServerAttributeReadOpCode                                              /* = 0x98, gBleGattEvtServerAttributeReadOpCode_c */
};
#endif /* gFsciBleHost_d */

static const pfGattClientProcEvtHandler_t maGattClientProcEvtHandlers[]=
{
    HandleGattProcExchangeMtu,                                                          /* = 0x00, gGattProcExchangeMtu_c */
    HandleGattProcDiscoverAllPrimaryServices,                                           /* = 0x01, gGattProcDiscoverAllPrimaryServices_c */
    HandleGattProcDiscoverPrimaryServicesByUuid,                                        /* = 0x02, gGattProcDiscoverPrimaryServicesByUuid_c */
    HandleGattProcFindIncludedServices,                                                 /* = 0x03, gGattProcFindIncludedServices_c */
    HandleGattProcDiscoverAllCharacteristics,                                           /* = 0x04, gGattProcDiscoverAllCharacteristics_c */
    HandleGattProcDiscoverCharacteristicByUuid,                                         /* = 0x05, gGattProcDiscoverCharacteristicByUuid_c */
    HandleGattProcDiscoverAllCharacteristicDescriptors,                                 /* = 0x06, gGattProcDiscoverAllCharacteristicDescriptors_c */
    HandleGattProcReadCharacteristicValue,                                              /* = 0x07, gGattProcReadCharacteristicValue_c */
    HandleGattProcReadUsingCharacteristicUuid,                                          /* = 0x08, gGattProcReadUsingCharacteristicUuid_c */
    HandleGattProcReadMultipleCharacteristicValues,                                     /* = 0x09, gGattProcReadMultipleCharacteristicValues_c */
    HandleGattProcWriteCharacteristicValue,                                             /* = 0x0A, gGattProcWriteCharacteristicValue_c */
    HandleGattProcReadCharacteristicDescriptor,                                         /* = 0x0B, gGattProcReadCharacteristicDescriptor_c */
    HandleGattProcWriteCharacteristicDescriptor,                                        /* = 0x0C, gGattProcWriteCharacteristicDescriptor_c */
    NULL,                                                                               /* reserved: 0x0D */
    NULL,                                                                               /* reserved: 0x0E */
#if defined(gBLE52_d) && (gBLE52_d == TRUE)
    HandleGattProcReadMultipleVarLengthCharValues                                       /* = 0x0F, gGattProcReadMultipleVarLengthCharValues_c */
#endif /* gBLE52_d */
};

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

void fsciBleSetGattClientProcedureCallback(gattClientProcedureCallback_t clientProcedureCallback)
{
    /* Set GATT Client procedure callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGattCallbacks.clientProcedureCallback  = (NULL != clientProcedureCallback) ?
                                                    clientProcedureCallback :
                                                    fsciBleGattClientProcedureCallback;
}


void fsciBleSetGattClientNotificationCallback(gattClientNotificationCallback_t clientNotificationCallback)
{
    /* Set GATT Client notification callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGattCallbacks.clientNotificationCallback  = (NULL != clientNotificationCallback) ?
                                                       clientNotificationCallback :
                                                       fsciBleGattClientNotificationCallback;
}


void fsciBleSetGattClientIndicationCallback(gattClientIndicationCallback_t clientIndicationCallback)
{
    /* Set GATT Client indication callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGattCallbacks.clientIndicationCallback  = (NULL != clientIndicationCallback) ?
                                                     clientIndicationCallback :
                                                     fsciBleGattClientIndicationCallback;
}

void fsciBleSetGattServerCallback(gattServerCallback_t serverCallback)
{
    /* Set GATT Server callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGattCallbacks.serverCallback  = (NULL != serverCallback) ?
                                                    serverCallback :
                                                    fsciBleGattServerCallback;
}

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
void fsciBleSetGattClientMultipleValueNotificationCallback(gattClientMultipleValueNotificationCallback_t clientMultipleValueNtfCallback)
{
    /* Set GATT Client notification callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGattCallbacks.clientMultipleValueNtfCallback  = (NULL != clientMultipleValueNtfCallback) ?
                                                       clientMultipleValueNtfCallback :
                                                       fsciBleGattClientMultipleValueNotificationCallback;
}

#if defined(gEATT_d) && (gEATT_d == TRUE)
void fsciBleSetGattClientEnhancedProcedureCallback(gattClientEnhancedProcedureCallback_t clientEnhancedProcedureCallback)
{
    /* Set GATT Client Enhanced procedure callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGattCallbacks.clientEnhancedProcedureCallback  = (NULL != clientEnhancedProcedureCallback) ?
                                                    clientEnhancedProcedureCallback :
                                                    fsciBleGattClientEnhancedProcedureCallback;
}


void fsciBleSetGattClientEnhancedNotificationCallback(gattClientEnhancedNotificationCallback_t clientEnhancedNotificationCallback)
{
    /* Set GATT Client Enhanced notification callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGattCallbacks.clientEnhancedNotificationCallback  = (NULL != clientEnhancedNotificationCallback) ?
                                                       clientEnhancedNotificationCallback :
                                                       fsciBleGattClientEnhancedNotificationCallback;
}


void fsciBleSetGattClientEnhancedIndicationCallback(gattClientEnhancedIndicationCallback_t clientEnhancedIndicationCallback)
{
    /* Set GATT Client Enhanced indication callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGattCallbacks.clientEnhancedIndicationCallback  = (NULL != clientEnhancedIndicationCallback) ?
                                                     clientEnhancedIndicationCallback :
                                                     fsciBleGattClientEnhancedIndicationCallback;
}

void fsciBleSetGattClientEnhancedMultipleValueNotificationCallback(gattClientEnhancedMultipleValueNotificationCallback_t clientEnhancedMultipleValueNtfCallback)
{
    /* Set GATT Client Enhanced notification callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGattCallbacks.clientEnhancedMultipleValueNtfCallback  = (NULL != clientEnhancedMultipleValueNtfCallback) ?
                                                       clientEnhancedMultipleValueNtfCallback :
                                                       fsciBleGattClientEnhancedMultipleValueNotificationCallback;
}

void fsciBleSetGattServerEnhancedCallback(gattServerEnhancedCallback_t serverEnhancedCallback)
{
    /* Set GATT Server Enhanced callback to an application desired function, only
    if not NULL. Otherwise set it to the FSCI empty static function */
    fsciBleGattCallbacks.serverEnhancedCallback  = (NULL != serverEnhancedCallback) ?
                                                    serverEnhancedCallback :
                                                    fsciBleGattServerEnhancedCallback;
}
#endif /* gEATT_d */
#endif /* gBLE52_d */

void fsciBleGattHandler(void* pData, void* pParam, uint32_t fsciInterfaceId)
{
    clientPacket_t* pClientPacket   = (clientPacket_t*)pData;
    uint8_t*        pBuffer         = &pClientPacket->structured.payload[0];
    bool_t          opCodeHandled   = FALSE;

#if gFsciBleTest_d
    /* Mark this command as initiated by FSCI */
    bFsciBleGattCmdInitiatedByFsci = TRUE;

    /* Verify if the command is Mode Select */
    if(gBleGattModeSelectOpCode_c == (fsciBleGattOpCode_t)pClientPacket->structured.header.opCode)
    {
        fsciBleGetBoolValueFromBuffer(bFsciBleGattEnabled, pBuffer);
        /* Return status */
        fsciBleGattStatusMonitor(gBleSuccess_c);
    }
    else
    {
        /* Verify if FSCI is enabled for this layer */
        if(TRUE == bFsciBleGattEnabled)
        {
#endif /* gFsciBleTest_d */
#if gFsciBleBBox_d || gFsciBleTest_d
            if ((pClientPacket->structured.header.opCode >= (uint8_t)gBleGattCmdInitOpCode_c) &&
               (pClientPacket->structured.header.opCode < SizeOfArray(maGattCmdOpCodeHandlers)))
            {
                if (maGattCmdOpCodeHandlers[pClientPacket->structured.header.opCode] != NULL)
                {
                    /* Select the GATT, GATT Client or GATT Server function to be called (using the FSCI opcode) */
                    maGattCmdOpCodeHandlers[pClientPacket->structured.header.opCode](pBuffer, fsciInterfaceId);
                    opCodeHandled = TRUE;
                }
            }
#endif /* gFsciBleBBox_d || gFsciBleTest_d */
#if gFsciBleHost_d
            if ((pClientPacket->structured.header.opCode >= gBleGattStatusOpCode_c) &&
               (pClientPacket->structured.header.opCode < SizeOfArray(maGattEvtOpCodeHandlers)) &&
                 (opCodeHandled == FALSE))
            {
                if (maGattEvtOpCodeHandlers[pClientPacket->structured.header.opCode - gBleGattStatusOpCode_c] != NULL)
                {
                    maGattEvtOpCodeHandlers[pClientPacket->structured.header.opCode - gBleGattStatusOpCode_c](pBuffer, fsciInterfaceId);
                    opCodeHandled = TRUE;
                }
            }
#endif /* gFsciBleHost_d */
            if (opCodeHandled == FALSE)
            {
                /* Unknown FSCI opcode */
                fsciBleError(gFsciUnknownOpcode_c, fsciInterfaceId);
            }
#if gFsciBleTest_d
        }
        else
        {
            /* FSCI SAP disabled */
            fsciBleError(gFsciSAPDisabled_c, fsciInterfaceId);
        }
    }

    /* Mark the next command as not initiated by FSCI */
    bFsciBleGattCmdInitiatedByFsci = FALSE;
#endif /* gFsciBleTest_d */

    (void)MEM_BufferFree(pData);
}


#if gFsciBleBBox_d || gFsciBleTest_d

void fsciBleGattStatusMonitor(bleResult_t result)
{
#if gFsciBleTest_d
    /* If GATT is disabled the status must be not monitored */
    if(FALSE == bFsciBleGattEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    if(TRUE == bGattClientRequestWithOutParameters)
    {
        /* GATT client request without parameters was initiated */
        if(gBleSuccess_c == result)
        {
#if gFsciBleTest_d
            bFsciBleGattClientAllocatedInfo = bFsciBleGattCmdInitiatedByFsci;
#endif /* gFsciBleTest_d */
            /* Keep the information because the procedure was accepted */
            fsciBleGattClientKeepInfo(bFsciBleGattClientAllocatedInfo);
        }
        else
        {
            /* Erase the information because the procedure was rejected */
#if gFsciBleTest_d
            fsciBleGattClientEraseTmpInfo(bFsciBleGattCmdInitiatedByFsci, gInvalidDeviceId_c, gUnenhancedBearerId_c);
#else
            fsciBleGattClientEraseTmpInfo(bFsciBleGattClientAllocatedInfo, gInvalidDeviceId_c, gUnenhancedBearerId_c);
#endif /* gFsciBleTest_d */
        }
    }

    /* Reset the flag that indicates if the last GATT request was a Client request
    with out parameters */
    bGattClientRequestWithOutParameters = FALSE;

    /* Send status over UART */
    fsciBleStatusMonitor(gFsciBleGattOpcodeGroup_c, (uint8_t)gBleGattStatusOpCode_c, result);
}

#endif /* gFsciBleBBox_d || gFsciBleTest_d */


#if gFsciBleHost_d || gFsciBleTest_d

void fsciBleGattNoParamCmdMonitor(fsciBleGattOpCode_t opCode)
{
#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Call the generic FSCI BLE monitor for commands or events that have no parameters */
    fsciBleNoParamCmdOrEvtMonitor((uint8_t)gFsciBleGattOpcodeGroup_c, (uint8_t)opCode);
}


void fsciBleGattMtuCmdMonitor(fsciBleGattOpCode_t opCode, deviceId_t deviceId)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, fsciBleGetDeviceIdBufferSize(&deviceId));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGattExchangeMtuCmdMonitor(fsciBleGattOpCode_t opCode, deviceId_t deviceId, uint16_t mtu)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGattGetMtuCmdMonitor(deviceId_t deviceId, uint16_t* pOutMtu)
{
    fsciBleGattMtuCmdMonitor(gBleGattCmdGetMtuOpCode_c, deviceId);

#if gFsciBleHost_d
    /* Save out parameters pointers */
    fsciBleGattSaveOutParams(pOutMtu);
#endif /* gFsciBleHost_d */
}


void fsciBleGattClientRegisterProcedureCallbackCmdMonitor(gattClientProcedureCallback_t callback)
{
#if gFsciBleHost_d
    fsciBleGattCallbacks.clientProcedureCallback = callback;
#endif /* gFsciBleHost_d */

    fsciBleGattNoParamCmdMonitor(gBleGattCmdClientRegisterProcedureCallbackOpCode_c);
}


void fsciBleGattClientRegisterNotificationCallbackCmdMonitor(gattClientNotificationCallback_t callback)
{
#if gFsciBleHost_d
    fsciBleGattCallbacks.clientNotificationCallback = callback;
#endif /* gFsciBleHost_d */

    fsciBleGattNoParamCmdMonitor(gBleGattCmdClientRegisterNotificationCallbackOpCode_c);
}


void fsciBleGattClientRegisterIndicationCallbackCmdMonitor(gattClientIndicationCallback_t callback)
{
#if gFsciBleHost_d
    fsciBleGattCallbacks.clientIndicationCallback = callback;
#endif /* gFsciBleHost_d */

    fsciBleGattNoParamCmdMonitor(gBleGattCmdClientRegisterIndicationCallbackOpCode_c);
}


void fsciBleGattClientDiscoverAllPrimaryServicesCmdMonitor(deviceId_t deviceId, gattService_t* pOutPrimaryServices, uint8_t maxServiceCount, uint8_t* pOutDiscoveredCount)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)gBleGattCmdClientDiscoverAllPrimaryServicesOpCode_c,
                                               fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(uint8_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint8Value(maxServiceCount, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

    /* Save the information that must be monitored when the
    procedure ends */
    fsciBleGattClientSaveServicesInfo(deviceId, gUnenhancedBearerId_c, pOutPrimaryServices);
    fsciBleGattClientSaveArraySizeInfo(deviceId, gUnenhancedBearerId_c, (uint16_t*)pOutDiscoveredCount);

#if gFsciBleTest_d
    /* Set flag to indicate that the last GATT request was a Client
    request with out parameters */
    bGattClientRequestWithOutParameters = TRUE;
#endif /* gFsciBleTest_d */
}


void fsciBleGattClientDiscoverPrimaryServicesByUuidCmdMonitor(deviceId_t deviceId, bleUuidType_t uuidType, const bleUuid_t* pUuid, gattService_t* aOutPrimaryServices, uint8_t maxServiceCount, uint8_t* pOutDiscoveredCount)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)gBleGattCmdClientDiscoverPrimaryServicesByUuidOpCode_c,
                                               fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(bleUuidType_t) +
                                               fsciBleGetUuidBufferSize(uuidType) + sizeof(uint8_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromEnumValue(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetBufferFromUuid(pUuid, &pBuffer, uuidType);
    fsciBleGetBufferFromUint8Value(maxServiceCount, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

    /* Save the information that must be monitored when the
    procedure ends */
    fsciBleGattClientSaveServicesInfo(deviceId, gUnenhancedBearerId_c, aOutPrimaryServices);
    fsciBleGattClientSaveArraySizeInfo(deviceId, gUnenhancedBearerId_c, (uint16_t*)pOutDiscoveredCount);

#if gFsciBleTest_d
    /* Set flag to indicate that the last GATT request was a Client
    request with out parameters */
    bGattClientRequestWithOutParameters = TRUE;
#endif /* gFsciBleTest_d */
}


void fsciBleGattClientDiscoverCharacteristicOfServiceByUuidCmdMonitor(deviceId_t deviceId, bleUuidType_t uuidType, const bleUuid_t* pUuid, const gattService_t* pIoService, gattCharacteristic_t* aOutCharacteristics, uint8_t maxCharacteristicCount, uint8_t* pOutDiscoveredCount)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)gBleGattCmdClientDiscoverCharacteristicOfServiceByUuidOpCode_c,
                                               fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(bleUuidType_t) +
                                               fsciBleGetUuidBufferSize(uuidType) +
                                               fsciBleGattClientGetServiceBufferSize(pIoService) +
                                               sizeof(uint8_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromEnumValue(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetBufferFromUuid(pUuid, &pBuffer, uuidType);
    fsciBleGattClientGetBufferFromService(pIoService, &pBuffer);
    fsciBleGetBufferFromUint8Value(maxCharacteristicCount, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

    /* Save the information that must be monitored when the
    procedure ends */
    fsciBleGattClientSaveCharacteristicsInfo(deviceId, gUnenhancedBearerId_c, aOutCharacteristics);
    fsciBleGattClientSaveArraySizeInfo(deviceId, gUnenhancedBearerId_c, (uint16_t*)pOutDiscoveredCount);

#if gFsciBleTest_d
    /* Set flag to indicate that the last GATT request was a Client
    request with out parameters */
    bGattClientRequestWithOutParameters = TRUE;
#endif /* gFsciBleTest_d */
}


void fsciBleGattClientDiscoverAllCharacteristicDescriptorsCmdMonitor(deviceId_t deviceId, gattCharacteristic_t* pIoCharacteristic, uint16_t endingHandle, uint8_t maxDescriptorCount)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)gBleGattCmdClientDiscoverAllCharacteristicDescriptorsOpCode_c,
                                               fsciBleGetDeviceIdBufferSize(&deviceId) +
                                               fsciBleGattClientGetCharacteristicBufferSize(pIoCharacteristic) +
                                               sizeof(uint16_t) + sizeof(uint8_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGattClientGetBufferFromCharacteristic(pIoCharacteristic, &pBuffer);
    fsciBleGetBufferFromUint16Value(endingHandle, pBuffer);
    fsciBleGetBufferFromUint8Value(maxDescriptorCount, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

    /* Save the information that must be monitored when the
    procedure ends */
    fsciBleGattClientSaveCharacteristicsInfo(deviceId, gUnenhancedBearerId_c, pIoCharacteristic);

#if gFsciBleTest_d
    /* Set flag to indicate that the last GATT request was a Client
    request with out parameters */
    bGattClientRequestWithOutParameters = TRUE;
#endif /* gFsciBleTest_d */
}


void fsciBleGattClientReadCharValueCmdMonitor(deviceId_t deviceId, gattCharacteristic_t* pIoCharacteristic, uint16_t maxReadBytes)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)gBleGattCmdClientReadCharacteristicValueOpCode_c,
                                               fsciBleGetDeviceIdBufferSize(&deviceId) +
                                               fsciBleGattClientGetCharacteristicBufferSize(pIoCharacteristic) +
                                               sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGattClientGetBufferFromCharacteristic(pIoCharacteristic, &pBuffer);
    fsciBleGetBufferFromUint16Value(maxReadBytes, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

    /* Save the information that must be monitored when the
    procedure ends */
    fsciBleGattClientSaveCharacteristicsInfo(deviceId, gUnenhancedBearerId_c, pIoCharacteristic);

#if gFsciBleTest_d
    /* Set flag to indicate that the last GATT request was a Client
    request with out parameters */
    bGattClientRequestWithOutParameters = TRUE;
#endif /* gFsciBleTest_d */
}


void fsciBleGattClientReadUsingCharacteristicUuidCmdMonitor(deviceId_t deviceId, bleUuidType_t uuidType, const bleUuid_t* pUuid, const gattHandleRange_t* pHandleRange, uint8_t* aOutBuffer, uint16_t maxReadBytes, uint16_t* pOutActualReadBytes)
{
    bool_t                      bHandleRangeIncluded = (NULL == pHandleRange) ? FALSE : TRUE;
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)gBleGattCmdClientReadUsingCharacteristicUuidOpCode_c,
                                               fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(bleUuidType_t) +
                                               fsciBleGetUuidBufferSize(uuidType) + sizeof(bool_t) +
                                               ((TRUE == bHandleRangeIncluded) ?
                                                fsciBleGattGetHandleRangeBufferSize(pHandleRange) : 0U) +
                                               sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromEnumValue(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetBufferFromUuid(pUuid, &pBuffer, uuidType);
    fsciBleGetBufferFromBoolValue(bHandleRangeIncluded, pBuffer);

    if(TRUE == bHandleRangeIncluded)
    {
        fsciBleGattGetBufferFromHandleRange(pHandleRange, &pBuffer);
    }

    fsciBleGetBufferFromUint16Value(maxReadBytes, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

    /* Save the information that must be monitored when the
    procedure ends */
    fsciBleGattClientSaveValueInfo(deviceId, gUnenhancedBearerId_c, aOutBuffer);
    fsciBleGattClientSaveArraySizeInfo(deviceId, gUnenhancedBearerId_c, pOutActualReadBytes);

#if gFsciBleTest_d
    /* Set flag to indicate that the last GATT request was a Client
    request with out parameters */
    bGattClientRequestWithOutParameters = TRUE;
#endif /* gFsciBleTest_d */
}


void fsciBleGattClientReadMultipleCharacteristicValuesCmdMonitor(deviceId_t deviceId, uint8_t cNumCharacteristics, gattCharacteristic_t* aIoCharacteristics)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    uint16_t                    dataSize = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(uint8_t);
    uint32_t                    iCount = 0U;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    for(iCount = 0; iCount < cNumCharacteristics; iCount++)
    {
        dataSize += fsciBleGattClientGetCharacteristicBufferSize(&aIoCharacteristics[iCount]);
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)gBleGattCmdClientReadMultipleCharacteristicValuesOpCode_c, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint8Value(cNumCharacteristics, pBuffer);

    for(iCount = 0; iCount < cNumCharacteristics; iCount++)
    {
        fsciBleGattClientGetBufferFromCharacteristic(&aIoCharacteristics[iCount], &pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

    /* Save the information that must be monitored when the
    procedure ends */
    fsciBleGattClientSaveCharacteristicsInfo(deviceId, gUnenhancedBearerId_c, aIoCharacteristics);
    /* Keep the number of the characteristics (this number can be not
    saved using the general keeping mechanism because it is not
    a pointer, it is a value) */
    fsciBleGattClientNbOfCharacteristics = cNumCharacteristics;

#if gFsciBleTest_d
    /* Set flag to indicate that the last GATT request was a Client
    request with out parameters */
    bGattClientRequestWithOutParameters = TRUE;
#endif /* gFsciBleTest_d */
}


void fsciBleGattClientWriteCharValueCmdMonitor(deviceId_t deviceId, const gattCharacteristic_t* pCharacteristic, uint16_t valueLength, const uint8_t* aValue, bool_t withoutResponse, bool_t signedWrite, bool_t doReliableLongCharWrites, const uint8_t* aCsrk)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)gBleGattCmdClientWriteCharacteristicValueOpCode_c,
                                               fsciBleGetDeviceIdBufferSize(&deviceId) +
                                               fsciBleGattClientGetCharacteristicBufferSize(pCharacteristic) +
                                               sizeof(uint16_t) + valueLength +
                                               sizeof(bool_t) + sizeof(bool_t) +
                                               sizeof(bool_t) + gcSmpCsrkSize_c);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGattClientGetBufferFromCharacteristic(pCharacteristic, &pBuffer);
    fsciBleGetBufferFromUint16Value(valueLength, pBuffer);
    fsciBleGetBufferFromArray(aValue, pBuffer, valueLength);
    fsciBleGetBufferFromBoolValue(withoutResponse, pBuffer);
    fsciBleGetBufferFromBoolValue(signedWrite, pBuffer);
    fsciBleGetBufferFromBoolValue(doReliableLongCharWrites, pBuffer);
    fsciBleGetBufferFromCsrk(aCsrk, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGattClientReadCharacteristicDescriptorCmdMonitor(deviceId_t deviceId, gattAttribute_t* pIoDescriptor, uint16_t maxReadBytes)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
   /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)gBleGattCmdClientReadCharacteristicDescriptorsOpCode_c,
                                               fsciBleGetDeviceIdBufferSize(&deviceId) +
                                               fsciBleGattClientGetAttributeBufferSize(pIoDescriptor) +
                                               sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGattClientGetBufferFromAttribute(pIoDescriptor, &pBuffer);
    fsciBleGetBufferFromUint16Value(maxReadBytes, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

    /* Save the information that must be monitored when the
    procedure ends */
    fsciBleGattClientSaveDescriptorsInfo(deviceId, gUnenhancedBearerId_c, pIoDescriptor);

#if gFsciBleTest_d
    /* Set flag to indicate that the last GATT request was a Client
    request with out parameters */
    bGattClientRequestWithOutParameters = TRUE;
#endif /* gFsciBleTest_d */
}


void fsciBleGattClientWriteCharacteristicDescriptorCmdMonitor(deviceId_t deviceId, const gattAttribute_t* pDescriptor, uint16_t valueLength, const uint8_t* aValue)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)gBleGattCmdClientWriteCharacteristicDescriptorsOpCode_c,
                                               fsciBleGetDeviceIdBufferSize(&deviceId) +
                                               fsciBleGattClientGetAttributeBufferSize(pDescriptor) +
                                               sizeof(uint16_t) + valueLength);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGattClientGetBufferFromAttribute(pDescriptor, &pBuffer);
    fsciBleGetBufferFromUint16Value(valueLength, pBuffer);
    fsciBleGetBufferFromArray(aValue, pBuffer, valueLength);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGattClientFindIncludedServicesOrCharacteristicsCmdMonitor(fsciBleGattOpCode_t opCode, deviceId_t deviceId, gattService_t* pIoService, uint8_t maxCount)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

 #if gFsciBleTest_d
   /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode,
                                               fsciBleGetDeviceIdBufferSize(&deviceId) +
                                               fsciBleGattClientGetServiceBufferSize(pIoService) +
                                               sizeof(uint8_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGattClientGetBufferFromService(pIoService, &pBuffer);
    fsciBleGetBufferFromUint8Value(maxCount, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);

    /* Save the information that must be monitored when the
    procedure ends */
    fsciBleGattClientSaveServicesInfo(deviceId, gUnenhancedBearerId_c, pIoService);

#if gFsciBleTest_d
    /* Set flag to indicate that the last GATT request was a Client
    request with out parameters */
    bGattClientRequestWithOutParameters = TRUE;
#endif /* gFsciBleTest_d */
}


void fsciBleGattServerRegisterCallbackCmdMonitor(gattServerCallback_t callback)
{
#if gFsciBleHost_d
    fsciBleGattCallbacks.serverCallback = callback;
#endif /* gFsciBleHost_d */

    fsciBleGattNoParamCmdMonitor(gBleGattCmdServerRegisterCallbackOpCode_c);
}


void fsciBleGattServerRegisterHandlesForWriteOrReadNotificationsCmdMonitor(fsciBleGattOpCode_t opCode, uint8_t handleCount, const uint16_t* aAttributeHandles)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode,
                                               sizeof(uint8_t) + handleCount * sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromUint8Value(handleCount, pBuffer);
    fsciBleGetBufferFromArray(aAttributeHandles, pBuffer, handleCount * sizeof(uint16_t));

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGattServerSendAttributeWrittenOrReadStatusCmdMonitor(fsciBleGattOpCode_t opCode, deviceId_t deviceId, uint16_t attributeHandle, uint8_t status)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

 #if gFsciBleTest_d
   /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode,
                                               fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(uint16_t) + sizeof(uint8_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(attributeHandle, pBuffer);
    fsciBleGetBufferFromUint8Value(status, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGattServerSendNotificationOrIndicationCmdMonitor(fsciBleGattOpCode_t opCode, deviceId_t deviceId, uint16_t handle)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                       sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(handle, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


void fsciBleGattServerSendInstantValueNotificationOrIndicationCmdMonitor(fsciBleGattOpCode_t opCode, deviceId_t deviceId, uint16_t handle, uint16_t valueLength, const uint8_t* pValue)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                       sizeof(uint16_t) + sizeof(uint16_t) + valueLength);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    fsciBleGetBufferFromUint16Value(handle, pBuffer);
    fsciBleGetBufferFromUint16Value(valueLength, pBuffer);
    fsciBleGetBufferFromArray(pValue, pBuffer, valueLength);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

void fsciBleGattServerRegisterUniqueHandlesForNotificationsCmdMonitor
(
    bool_t bWrite,
    bool_t bRead
)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled or if the command was initiated by FSCI it must be not monitored */
    if((FALSE == bFsciBleGattEnabled) ||
       (TRUE == bFsciBleGattCmdInitiatedByFsci))
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)gBleGattCmdServerRegisterUniqueHandlesForNotificationsOpCode_c,
                                               2U * sizeof(bool_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set command parameters in the buffer */
    fsciBleGetBufferFromBoolValue(bWrite, pBuffer);
    fsciBleGetBufferFromBoolValue(bRead, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

#endif /* gFsciBleHost_d || gFsciBleTest_d */


void fsciBleGattGetMtuEvtMonitor(uint16_t* pOutMtu)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;

#if gFsciBleTest_d
    /* If GATT is disabled the event must be not monitored */
    if(FALSE == bFsciBleGattEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)gBleGattEvtGetMtuOpCode_c, sizeof(uint16_t));

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromUint16Value(*pOutMtu, pBuffer);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}


#if gFsciBleBBox_d || gFsciBleTest_d

void fsciBleGattClientProcedureEvtMonitor(deviceId_t deviceId, bearerId_t bearerId, gattProcedureType_t procedureType, gattProcedureResult_t procedureResult, bleResult_t error)
{
    bool_t earlyReturn = TRUE;

#if gFsciBleTest_d
    /* If GATT is disabled the event must be not monitored */
    if(FALSE != bFsciBleGattEnabled)
    {
#endif /* gFsciBleTest_d */
        if ((uint8_t)procedureType < SizeOfArray(maGattClientProcEvtHandlers))
        {
            if (maGattClientProcEvtHandlers[procedureType] != NULL)
            {
                maGattClientProcEvtHandlers[procedureType](deviceId, bearerId, procedureResult, error);
                earlyReturn = FALSE;
            }
        }
        
        if (earlyReturn == TRUE)
        {
            /* Procedure not recognized */
            fsciBleError(gFsciError_c, fsciBleInterfaceId);
            fsciBleGattClientEraseInfo(bFsciBleGattClientAllocatedInfo, deviceId, bearerId);
        }

#if gFsciBleTest_d
    }
#endif /* gFsciBleTest_d */

    if(!earlyReturn)
    {
        /* Erase the information kept (allocated or just saved) */
        fsciBleGattClientEraseInfo(bFsciBleGattClientAllocatedInfo, deviceId, bearerId);
    }
}


void fsciBleGattClientNotificationOrIndicationEvtMonitor(fsciBleGattOpCode_t opCode, deviceId_t deviceId, bearerId_t bearerId, uint16_t characteristicValueHandle, uint8_t* aValue, uint16_t valueLength)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    uint32_t                    dataSize = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(uint16_t) +
                                                        sizeof(uint16_t) + (uint32_t)valueLength;

#if gFsciBleTest_d
    /* If GATT is disabled the event must be not monitored */
    if(FALSE == bFsciBleGattEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }
    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

    if (bearerId != gUnenhancedBearerId_c)
    {
    fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromUint16Value(characteristicValueHandle, pBuffer);
    fsciBleGetBufferFromUint16Value(valueLength, pBuffer);
    fsciBleGetBufferFromArray(aValue, pBuffer, valueLength);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
void fsciBleGattClientMultipleValueNotificationEvtMonitor(deviceId_t  deviceId, bearerId_t bearerId, uint8_t* aHandleLenValue, uint32_t totalLength)
{
    clientPacketStructured_t    *pClientPacket = NULL;
    uint8_t                     *pBuffer = NULL;
    uint8_t                     handleCount = 0U;
    uint32_t                    crtLen = 0;
    uint8_t                     *pCrtPos = NULL;
    fsciBleGattOpCode_t         opCode = gBleGattEvtClientMultipleValueNotificationOpCode_c;
    uint16_t                    dataSize = (uint16_t)(fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(uint16_t) + totalLength);
#if gFsciBleTest_d
    /* If GATT is disabled the event must be not monitored */
    if(FALSE == bFsciBleGattEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    if (bearerId != gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientEnhancedMultipleValueNotificationOpCode_c;
        dataSize += ((uint16_t)sizeof(&bearerId));
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    /* Compute number of handles */
    pCrtPos = aHandleLenValue;
    while (crtLen < totalLength)
    {
        uint32_t handleLen = 0U;
        uint16_t temp = 0U;
        pCrtPos += sizeof(uint16_t); /* pass over handle Id */
        crtLen += sizeof(uint16_t); /* add handle Id length */
        fsciBleGetUint16ValueFromBuffer(temp, pCrtPos);
        handleLen = (uint32_t)temp;
        crtLen += sizeof(uint16_t); /* add value length field's length */
        crtLen += handleLen; /* add value */
        pCrtPos += handleLen;
        handleCount++;
    }
    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromUint16Value(handleCount, pBuffer);
    fsciBleGetBufferFromArray(aHandleLenValue, pBuffer, totalLength);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}
#endif /* gBLE52_d */

void fsciBleGattServerEvtMonitor(deviceId_t deviceId, bearerId_t bearerId, gattServerEvent_t* pServerEvent)
{
    clientPacketStructured_t*   pClientPacket = NULL;
    uint8_t*                    pBuffer = NULL;
    fsciBleGattOpCode_t         opCode = gBleGattEvtClientNotificationOpCode_c;
    bool_t earlyReturn = FALSE;

#if gFsciBleTest_d
    /* If GATT is disabled the event must be not monitored */
    if(FALSE == bFsciBleGattEnabled)
    {
        return;
    }
#endif /* gFsciBleTest_d */

    /* Get the FSCI opcode */
    switch(pServerEvent->eventType)
    {
        case gEvtMtuChanged_c:
            {
                opCode = gBleGattEvtServerMtuChangedOpCode_c;
            }
            break;

        case gEvtHandleValueConfirmation_c:
            {
                if (bearerId == gUnenhancedBearerId_c)
                {
                    opCode = gBleGattEvtServerHandleValueConfirmationOpCode_c;
                }
                else
                {
                    opCode = gBleGattEvtServerEnhancedHandleValueConfirmationOpCode_c;
                }
            }
          break;

        case gEvtAttributeWritten_c:
            {
                if (bearerId == gUnenhancedBearerId_c)
                {
                    opCode = gBleGattEvtServerAttributeWrittenOpCode_c;
                }
                else
                {
                    opCode = gBleGattEvtServerEnhancedAttributeWrittenOpCode_c;
                }
            }
            break;

        case gEvtCharacteristicCccdWritten_c:
            {
                if (bearerId == gUnenhancedBearerId_c)
                {
                    opCode = gBleGattEvtServerCharacteristicCccdWrittenOpCode_c;
                }
                else
                {
                    opCode = gBleGattEvtServerEnhancedCharacteristicCccdWrittenOpCode_c;
                }
            }
            break;

        case gEvtAttributeWrittenWithoutResponse_c:
            {
                if (bearerId == gUnenhancedBearerId_c)
                {
                    opCode = gBleGattEvtServerAttributeWrittenWithoutResponseOpCode_c;
                }
                else
                {
                    opCode = gBleGattEvtServerEnhancedAttributeWrittenWithoutResponseOpCode_c;
                }
            }
            break;

        case gEvtError_c:
            {
                if (bearerId == gUnenhancedBearerId_c)
                {
                    opCode = gBleGattEvtServerErrorOpCode_c;
                }
                else
                {
                    opCode = gBleGattEvtServerEnhancedErrorOpCode_c;
                }
            }
            break;

        case gEvtLongCharacteristicWritten_c:
            {
                if (bearerId == gUnenhancedBearerId_c)
                {
                    opCode = gBleGattEvtServerLongCharacteristicWrittenOpCode_c;
                }
                else
                {
                    opCode = gBleGattEvtServerEnhancedLongCharacteristicWrittenOpCode_c;
                }
            }
            break;

        case gEvtAttributeRead_c:
            {
                if (bearerId == gUnenhancedBearerId_c)
                {
                    opCode = gBleGattEvtServerAttributeReadOpCode_c;
                }
                else
                {
                    opCode = gBleGattEvtServerEnhancedAttributeReadOpCode_c;
                }
            }
            break;

        default:
            {
                fsciBleError(gFsciError_c, fsciBleInterfaceId);
                earlyReturn = TRUE;
                break;
            }
    }

    if (!earlyReturn)
    {
        /* Allocate the packet to be sent over UART */
        pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, fsciBleGetDeviceIdBufferSize(&deviceId) +
                                                           sizeof(&bearerId) +
                                                           fsciBleGattServerGetServerEventBufferSize(pServerEvent));

        if(NULL == pClientPacket)
        {
            return;
        }

        pBuffer = &pClientPacket->payload[0];

        /* Set event parameters in the buffer */
        fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

        if (bearerId != gUnenhancedBearerId_c)
        {
            fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
        }

        fsciBleGattServGetBuffrFromServEvent(pServerEvent, &pBuffer);

        /* Transmit the packet over UART */
        fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
    }
}

#endif /* gFsciBleBBox_d || gFsciBleTest_d */

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

void fsciBleGattClientProcedureCallback(deviceId_t deviceId, gattProcedureType_t procedureType, gattProcedureResult_t procedureResult, bleResult_t error)
{
    fsciBleGattClientProcedureEvtMonitor(deviceId, gUnenhancedBearerId_c, procedureType, procedureResult, error);
}

void fsciBleGattClientNotificationCallback(deviceId_t deviceId, uint16_t characteristicValueHandle, uint8_t* aValue, uint16_t valueLength)
{
    fsciBleGattClientNotificationEvtMonitor(deviceId, characteristicValueHandle, aValue, valueLength);
}

void fsciBleGattClientIndicationCallback(deviceId_t deviceId, uint16_t characteristicValueHandle, uint8_t* aValue, uint16_t valueLength)
{
    fsciBleGattClientIndicationEvtMonitor(deviceId, characteristicValueHandle, aValue, valueLength);
}

void fsciBleGattServerCallback(deviceId_t deviceId, gattServerEvent_t* pServerEvent)
{
    fsciBleGattServerEvtMonitor(deviceId, gUnenhancedBearerId_c, pServerEvent);
}

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
void fsciBleGattClientMultipleValueNotificationCallback(deviceId_t  deviceId, uint8_t* aHandleLenValue, uint32_t totalLength)
{
    fsciBleGattClientMultipleValueNotificationEvtMonitor(deviceId, gUnenhancedBearerId_c, aHandleLenValue, totalLength);
}

#if (defined gEATT_d) && (gEATT_d == TRUE)
void fsciBleGattClientEnhancedProcedureCallback(deviceId_t deviceId, bearerId_t bearerId, gattProcedureType_t procedureType, gattProcedureResult_t procedureResult, bleResult_t error)
{
    fsciBleGattClientProcedureEvtMonitor(deviceId, bearerId, procedureType, procedureResult, error);
}

void fsciBleGattClientEnhancedNotificationCallback(deviceId_t deviceId, bearerId_t bearerId, uint16_t characteristicValueHandle, uint8_t* aValue, uint16_t valueLength)
{
    fsciBleGattClientEnhancedNotificationEvtMonitor(deviceId, bearerId, characteristicValueHandle, aValue, valueLength);
}

void fsciBleGattClientEnhancedIndicationCallback(deviceId_t deviceId, bearerId_t bearerId, uint16_t characteristicValueHandle, uint8_t* aValue, uint16_t valueLength)
{
    fsciBleGattClientEnhancedIndicationEvtMonitor(deviceId, bearerId, characteristicValueHandle, aValue, valueLength);
}

void fsciBleGattServerEnhancedCallback(deviceId_t deviceId, bearerId_t bearerId, gattServerEvent_t* pServerEvent)
{
    fsciBleGattServerEvtMonitor(deviceId, bearerId, pServerEvent);
}

void fsciBleGattClientEnhancedMultipleValueNotificationCallback(deviceId_t  deviceId, bearerId_t bearerId, uint8_t* aHandleLenValue, uint32_t totalLength)
{
    fsciBleGattClientMultipleValueNotificationEvtMonitor(deviceId, bearerId, aHandleLenValue, totalLength);
}
#endif /* gEATT_d */
#endif /* gBLE52_d */

#if gFsciBleBBox_d || gFsciBleTest_d
/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdInitOpCode(uint8_t *pBuffer,
*                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdInitOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdInitOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(Gatt_Init());
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdGetMtuOpCode(uint8_t *pBuffer,
*                                             uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdGetMtuOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdGetMtuOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint16_t    mtu = 0U;

    /* Get deviceId from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    fsciBleGattCallApiFunction(Gatt_GetMtu(deviceId, &mtu));
    fsciBleGattMonitorOutParams(GetMtu, &mtu);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientInitOpCode(uint8_t *pBuffer,
*                                                 uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientInitOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientInitOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattClient_Init());
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientResetProceduresOpCode(uint8_t *pBuffer,
*                                                            uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientResetProceduresOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientResetProceduresOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattClient_ResetProcedure());
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientRegisterProcedureCallbackOpCode(
*                                                            uint8_t *pBuffer,
*                                                            uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientRegisterProcedureCallbackOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientRegisterProcedureCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattClient_RegisterProcedureCallback(fsciBleGattCallbacks.clientProcedureCallback));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientRegisterNotificationCallbackOpCode(
*                                                            uint8_t *pBuffer,
*                                                            uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientRegisterNotificationCallbackOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientRegisterNotificationCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattClient_RegisterNotificationCallback(fsciBleGattCallbacks.clientNotificationCallback));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientRegisterIndicationCallbackOpCode(
*                                                            uint8_t *pBuffer,
*                                                            uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientRegisterIndicationCallbackOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientRegisterIndicationCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattClient_RegisterIndicationCallback(fsciBleGattCallbacks.clientIndicationCallback));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientExchangeMtuOpCode(uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientExchangeMtuOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientExchangeMtuOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    deviceId_t  deviceId = gInvalidDeviceId_c;
    uint16_t    mtu = 0U;

    /* Get deviceId parameter from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(mtu, pBuffer);

    fsciBleGattCallApiFunction(GattClient_ExchangeMtu(deviceId, mtu));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientDiscoverAllPrimaryServicesOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientDiscoverAllPrimaryServicesOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientDiscoverAllPrimaryServicesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t         maxServiceCount = 0U;
    gattService_t*  pOutServices = NULL;
    uint8_t*        pOutServiceCount = NULL;
    deviceId_t      deviceId = gInvalidDeviceId_c;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(maxServiceCount, pBuffer);

    /* Allocate buffer for the services array - consider that maxServiceCount
     is bigger than 0 */
    pOutServices        = fsciBleGattClientAllocOutOrIoServices(maxServiceCount, deviceId, gUnenhancedBearerId_c);
    /* Allocate buffer for the services array count */
    pOutServiceCount    = (uint8_t*)fsciBleGattClientAllocOutOrIoArraySize(deviceId, gUnenhancedBearerId_c);

    if((NULL != pOutServices) &&
       (NULL != pOutServiceCount))
    {
        /* Set flag to indicate that the last GATT request was a Client
        request with out parameters */
        bGattClientRequestWithOutParameters = TRUE;

        fsciBleGattCallApiFunction(GattClient_DiscoverAllPrimaryServices(deviceId, pOutServices,
                                                                         maxServiceCount,
                                                                         pOutServiceCount));
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientDiscoverPrimaryServicesByUuidOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientDiscoverPrimaryServicesByUuidOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientDiscoverPrimaryServicesByUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleUuidType_t   uuidType = {0};
    bleUuid_t       uuid = {0};
    uint8_t         maxServiceCount = 0U;
    gattService_t*  pOutServices = NULL;
    uint8_t*        pOutServiceCount = NULL;
    deviceId_t      deviceId = gInvalidDeviceId_c;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&uuid, &pBuffer, uuidType);
    fsciBleGetUint8ValueFromBuffer(maxServiceCount, pBuffer);

    /* Allocate buffer for the services array - consider that maxServiceCount
    is bigger than 0 */
    pOutServices        = fsciBleGattClientAllocOutOrIoServices(maxServiceCount, deviceId, gUnenhancedBearerId_c);
    /* Allocate buffer for the services array count */
    pOutServiceCount    = (uint8_t*)fsciBleGattClientAllocOutOrIoArraySize(deviceId, gUnenhancedBearerId_c);

    if((NULL != pOutServices) ||
       (NULL != pOutServiceCount))
    {
        /* Set flag to indicate that the last GATT request was a Client
        request with out parameters */
        bGattClientRequestWithOutParameters = TRUE;

        fsciBleGattCallApiFunction(GattClient_DiscoverPrimaryServicesByUuid(deviceId, uuidType, &uuid,
                                                                            pOutServices,
                                                                            maxServiceCount,
                                                                            pOutServiceCount));
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientFindIncludedServicesOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientFindIncludedServicesOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientFindIncludedServicesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattService_t*  pIoService = NULL;
    deviceId_t      deviceId = gInvalidDeviceId_c;

    /* Get deviceId from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    /* Allocate buffer for the service */
    pIoService = fsciBleGattClientAllocOutOrIoServices(1, deviceId, gUnenhancedBearerId_c);

    if(NULL != pIoService)
    {
        uint8_t         maxIncludedServiceCount = 0U;
        gattService_t*  pOutIncludedServices = NULL;

        /* Get more parameters from the received packet */
        fsciBleGattClientGetServiceFromBuffer(pIoService, &pBuffer);
        fsciBleGetUint8ValueFromBuffer(maxIncludedServiceCount, pBuffer);

        /* Allocate buffer for the included services - consider that maxIncludedServiceCount
        is bigger than 0 */
        pOutIncludedServices = fsciBleGattClientAllocOutOrIoIncludedServices(maxIncludedServiceCount, deviceId, gUnenhancedBearerId_c);

        if(NULL != pOutIncludedServices)
        {
            pIoService->aIncludedServices = pOutIncludedServices;

            /* Set flag to indicate that the last GATT request was a Client
            request with out parameters */
            bGattClientRequestWithOutParameters = TRUE;

            fsciBleGattCallApiFunction(GattClient_FindIncludedServices(deviceId, pIoService,
                                                                       maxIncludedServiceCount));
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientDiscoverAllCharacteristicsOfServiceOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientDiscoverAllCharacteristicsOfServiceOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientDiscoverAllCharacteristicsOfServiceOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattService_t*  pIoService = NULL;
    deviceId_t      deviceId = gInvalidDeviceId_c;

    /* Get deviceId from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    /* Allocate buffer for the service */
    pIoService = fsciBleGattClientAllocOutOrIoServices(1, deviceId, gUnenhancedBearerId_c);

    if(NULL != pIoService)
    {
        uint8_t                 maxCharacteristicCount = 0U;
        gattCharacteristic_t*   pOutCharacteristics = NULL;

        /* Get more parameters from the received packet */
        fsciBleGattClientGetServiceFromBuffer(pIoService, &pBuffer);
        fsciBleGetUint8ValueFromBuffer(maxCharacteristicCount, pBuffer);

        /* Allocate buffer for the characteristics - consider that maxCharacteristicCount
        is bigger than 0 */
        pOutCharacteristics = fsciBleGattClientAllocOutOrIoCharacteristics(maxCharacteristicCount, deviceId, gUnenhancedBearerId_c);

        if(NULL != pOutCharacteristics)
        {
            pIoService->aCharacteristics = pOutCharacteristics;

            /* Set flag to indicate that the last GATT request was a Client
            request with out parameters */
            bGattClientRequestWithOutParameters = TRUE;

            fsciBleGattCallApiFunction(GattClient_DiscoverAllCharacteristicsOfService(deviceId, pIoService,
                                                                                      maxCharacteristicCount));
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientDiscoverCharacteristicOfServiceByUuidOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientDiscoverCharacteristicOfServiceByUuidOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientDiscoverCharacteristicOfServiceByUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleUuidType_t           uuidType = {0};
    bleUuid_t               uuid = {0};
    gattService_t           service = {0};
    uint8_t                 maxCharacteristicCount = 0U;
    gattCharacteristic_t*   pOutCharacteristics = NULL;
    uint8_t*                pOutCharacteristicCount = NULL;
    deviceId_t              deviceId = gInvalidDeviceId_c;


    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&uuid, &pBuffer, uuidType);
    fsciBleGattClientGetServiceFromBuffer(&service, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(maxCharacteristicCount, pBuffer);

    /* Allocate buffer for the characteristics - consider that maxCharacteristicCount
    is bigger than 0 */
    pOutCharacteristics     = fsciBleGattClientAllocOutOrIoCharacteristics(maxCharacteristicCount, deviceId, gUnenhancedBearerId_c);
    /* Allocate buffer for the characteristics array count */
    pOutCharacteristicCount = (uint8_t*)fsciBleGattClientAllocOutOrIoArraySize(deviceId, gUnenhancedBearerId_c);

    if((NULL != pOutCharacteristics) &&
       (NULL != pOutCharacteristicCount))
    {
        /* Set flag to indicate that the last GATT request was a Client
        request with out parameters */
        bGattClientRequestWithOutParameters = TRUE;

        fsciBleGattCallApiFunction(GattClient_DiscoverCharacteristicOfServiceByUuid(deviceId, uuidType, &uuid,
                                                                                    &service, pOutCharacteristics,
                                                                                    maxCharacteristicCount,
                                                                                    pOutCharacteristicCount));
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientDiscoverAllCharacteristicDescriptorsOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientDiscoverAllCharacteristicDescriptorsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientDiscoverAllCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattCharacteristic_t*   pIoCharacteristic = NULL;
    deviceId_t              deviceId = gInvalidDeviceId_c;

    /* Get deviceId from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    /* Allocate buffer for the characteristic */
    pIoCharacteristic = fsciBleGattClientAllocOutOrIoCharacteristics(1, deviceId, gUnenhancedBearerId_c);

    if(NULL != pIoCharacteristic)
    {
        uint16_t            endingHandle = 0U;
        uint8_t             maxDescriptorCount = 0U;
        gattAttribute_t*    pOutDescriptors = NULL;

        /* Get more parameters from the received packet */
        fsciBleGattClientGetCharFromBuffer(pIoCharacteristic, &pBuffer);
        fsciBleGetUint16ValueFromBuffer(endingHandle, pBuffer);
        fsciBleGetUint8ValueFromBuffer(maxDescriptorCount, pBuffer);

        /* Allocate buffer for the descriptors - consider that maxDescriptorCount
        is bigger than 0 */
        pOutDescriptors = fsciBleGattClientAllocOutOrIoAttributes(maxDescriptorCount, deviceId, gUnenhancedBearerId_c);

        if(NULL != pOutDescriptors)
        {
            pIoCharacteristic->aDescriptors = pOutDescriptors;

            /* Set flag to indicate that the last GATT request was a Client
            request with out parameters */
            bGattClientRequestWithOutParameters = TRUE;

            fsciBleGattCallApiFunction(GattClient_DiscoverAllCharacteristicDescriptors(deviceId, pIoCharacteristic,
                                                                                       endingHandle,
                                                                                       maxDescriptorCount));
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientReadCharacteristicValueOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientReadCharacteristicValueOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientReadCharacteristicValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattCharacteristic_t*   pIoCharacteristic = NULL;
    deviceId_t              deviceId = gInvalidDeviceId_c;

    /* Get deviceId from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    /* Allocate buffer for the characteristic */
    pIoCharacteristic = fsciBleGattClientAllocOutOrIoCharacteristics(1, deviceId, gUnenhancedBearerId_c);

    if(NULL != pIoCharacteristic)
    {
        uint16_t    maxValueLength = 0U;
        uint8_t*    pOutValue = NULL;

        /* Get more parameters from the received packet */
        fsciBleGattClientGetCharFromBuffer(pIoCharacteristic, &pBuffer);
        fsciBleGetUint16ValueFromBuffer(maxValueLength, pBuffer);

        /* Allocate buffer for the descriptors - consider that maxValueLength
        is bigger than 0 */
        pOutValue = fsciBleGattClientAllocOutOrIoValue(maxValueLength, deviceId, gUnenhancedBearerId_c);

        if(NULL != pOutValue)
        {
            pIoCharacteristic->value.paValue = pOutValue;

            /* Set flag to indicate that the last GATT request was a Client
            request with out parameters */
            bGattClientRequestWithOutParameters = TRUE;

            fsciBleGattCallApiFunction(GattClient_ReadCharacteristicValue(deviceId,
                                                                          pIoCharacteristic,
                                                                          maxValueLength));
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientReadUsingCharacteristicUuidOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientReadUsingCharacteristicUuidOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientReadUsingCharacteristicUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleUuidType_t       uuidType = {0};
    bleUuid_t           uuid = {0};
    bool_t              bHandleRangeIncluded = FALSE;
    gattHandleRange_t   handleRange;
    gattHandleRange_t*  pHandleRange = NULL;
    uint16_t            maxValueLength = 0U;
    uint8_t*            pOutValue = NULL;
    uint16_t*           pOutValueLength = NULL;
    deviceId_t          deviceId = gInvalidDeviceId_c;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&uuid, &pBuffer, uuidType);
    fsciBleGetBoolValueFromBuffer(bHandleRangeIncluded, pBuffer);

    if(TRUE == bHandleRangeIncluded)
    {
        fsciBleGattGetHandleRangeFromBuffer(&handleRange, &pBuffer);
        pHandleRange = &handleRange;
    }

    fsciBleGetUint16ValueFromBuffer(maxValueLength, pBuffer);

    /* Allocate buffer for the value - consider that maxValueLength
    is bigger than 0 */
    pOutValue       = fsciBleGattClientAllocOutOrIoValue(maxValueLength, deviceId, gUnenhancedBearerId_c);
    /* Allocate buffer for the value length */
    pOutValueLength = fsciBleGattClientAllocOutOrIoArraySize(deviceId, gUnenhancedBearerId_c);

    if((NULL != pOutValue) &&
       (NULL != pOutValueLength))
    {
        /* Set flag to indicate that the last GATT request was a Client
        request with out parameters */
        bGattClientRequestWithOutParameters = TRUE;

        fsciBleGattCallApiFunction(GattClient_ReadUsingCharacteristicUuid(deviceId, uuidType, &uuid,
                                                                          pHandleRange, pOutValue,
                                                                          maxValueLength, pOutValueLength));
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientReadMultipleCharacteristicValuesOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientReadMultipleCharacteristicValuesOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientReadMultipleCharacteristicValuesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattCharacteristic_t*   pIoCharacteristics = NULL;
    deviceId_t              deviceId = gInvalidDeviceId_c;
    bool_t                  earlyReturn = FALSE;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(fsciBleGattClientNbOfCharacteristics, pBuffer);

    /* Allocate buffer for the characteristics - considering that fsciBleGattClientNbOfCharacteristics
    is bigger than 0 */
    pIoCharacteristics = fsciBleGattClientAllocOutOrIoCharacteristics(fsciBleGattClientNbOfCharacteristics, deviceId, gUnenhancedBearerId_c);

    if(NULL != pIoCharacteristics)
    {
        uint16_t    maxValueLength = 0U;
        uint8_t*    pOutValue = NULL;
        uint8_t     iCount = 0U;

        /* Get all characteristics from the received packet */
        for(iCount = 0; iCount < fsciBleGattClientNbOfCharacteristics; iCount++)
        {
            fsciBleGattClientGetCharFromBuffer(&pIoCharacteristics[iCount], &pBuffer);

            if (pIoCharacteristics[iCount].value.maxValueLength <= gAttMaxValueLength_c)
            {
                /* Compute the total length of all the characteristics values */
                maxValueLength += pIoCharacteristics[iCount].value.maxValueLength;
            }
            else
            {
                fsciBleError(gFsciError_c, fsciInterfaceId);
                fsciBleGattClientEraseTmpInfo(TRUE, deviceId, gUnenhancedBearerId_c);
                earlyReturn = TRUE;
                break;
            }
        }

        if (!earlyReturn)
        {
            /* Allocate a single buffer for all the characteristics values - considering
            that maxValueLength is bigger than 0 */
            pOutValue = fsciBleGattClientAllocOutOrIoValue(maxValueLength, deviceId, gUnenhancedBearerId_c);

            if(NULL != pOutValue)
            {
                /* Set paValue field for all the characteristics to the corresponding position in the
                value buffer */
                for(iCount = 0; iCount < fsciBleGattClientNbOfCharacteristics; iCount++)
                {
                    pIoCharacteristics[iCount].value.paValue = pOutValue;

                    pOutValue += pIoCharacteristics[iCount].value.maxValueLength;
                }

                /* Set flag to indicate that the last GATT request was a Client
                request with out parameters */
                bGattClientRequestWithOutParameters = TRUE;

                fsciBleGattCallApiFunction(GattClient_ReadMultipleCharacteristicValues(deviceId,
                                                                                       fsciBleGattClientNbOfCharacteristics,
                                                                                       pIoCharacteristics));
            }
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientWriteCharacteristicValueOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientWriteCharacteristicValueOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientWriteCharacteristicValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattCharacteristic_t    characteristic = {0};
    uint16_t                valueLength = 0U;
    uint8_t*                pValue = NULL;
    deviceId_t              deviceId = gInvalidDeviceId_c;

    characteristic.aDescriptors = NULL;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGattClientGetCharFromBuffer(&characteristic, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(valueLength, pBuffer);

    if (valueLength <= gAttMaxValueLength_c)
    {
        /* Allocate buffer for the value */
        pValue = MEM_BufferAlloc(valueLength);
    }

    if((0U == valueLength) ||
       (NULL != pValue))
    {
        bool_t      bWithoutResponse = FALSE;
        bool_t      bSignedWrite = FALSE;
        bool_t      bDoReliableLongCharWrites = FALSE;
        uint8_t     csrk[gcSmpCsrkSize_c] = {0};

        /* Get more parameters from the received packet */
        fsciBleGetArrayFromBuffer(pValue, pBuffer, valueLength);
        fsciBleGetBoolValueFromBuffer(bWithoutResponse, pBuffer);
        fsciBleGetBoolValueFromBuffer(bSignedWrite, pBuffer);
        fsciBleGetBoolValueFromBuffer(bDoReliableLongCharWrites, pBuffer);
        fsciBleGetCsrkFromBuffer(csrk, pBuffer);

        fsciBleGattCallApiFunction(GattClient_WriteCharacteristicValue(deviceId, &characteristic,
                                                                       valueLength, pValue,
                                                                       bWithoutResponse, bSignedWrite,
                                                                       bDoReliableLongCharWrites,
                                                                       csrk));

        (void)MEM_BufferFree(pValue);
    }
    else
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientReadCharacteristicDescriptorsOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientReadCharacteristicDescriptorsOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientReadCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattAttribute_t*    pIoDescriptor = NULL;
    deviceId_t          deviceId = gInvalidDeviceId_c;

    /* Get deviceId from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    /* Allocate buffer for the descriptor */
    pIoDescriptor = fsciBleGattClientAllocOutOrIoAttributes(1, deviceId, gUnenhancedBearerId_c);

    if(NULL != pIoDescriptor)
    {
        uint16_t    maxValueLength = 0U;
        uint8_t*    pOutValue = NULL;

        /* Get more parameters from the received packet */
        fsciBleGattClientGetAttributeFromBuffer(pIoDescriptor, &pBuffer);
        fsciBleGetUint16ValueFromBuffer(maxValueLength, pBuffer);

        /* Allocate buffer for the value - considering that maxValueLength
        is bigger than 0 */
        pOutValue = fsciBleGattClientAllocOutOrIoValue(maxValueLength, deviceId, gUnenhancedBearerId_c);

        if(NULL != pOutValue)
        {
            pIoDescriptor->paValue = pOutValue;

            /* Set flag to indicate that the last GATT request was a Client
            request with out parameters */
            bGattClientRequestWithOutParameters = TRUE;

            fsciBleGattCallApiFunction(GattClient_ReadCharacteristicDescriptor(deviceId,
                                                                               pIoDescriptor,
                                                                               maxValueLength));
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientWriteCharacteristicDescriptorsOpCode(
*                                                        uint8_t *pBuffer,
*                                                        uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientWriteCharacteristicDescriptorsOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientWriteCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattAttribute_t descriptor;
    uint16_t        valueLength = 0U;
    uint8_t*        pValue = NULL;
    deviceId_t      deviceId = gInvalidDeviceId_c;

    descriptor.paValue = NULL;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGattClientGetAttributeFromBuffer(&descriptor, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(valueLength, pBuffer);

    if (valueLength <= gAttMaxValueLength_c)
    {
        /* Allocate buffer for the value */
        pValue = MEM_BufferAlloc(valueLength);
    }

    if((0U == valueLength) ||
       (NULL != pValue))
    {
        /* Get value from the received packet */
        fsciBleGetArrayFromBuffer(pValue, pBuffer, valueLength);

        fsciBleGattCallApiFunction(GattClient_WriteCharacteristicDescriptor(deviceId, &descriptor,
                                                                            valueLength, pValue));

        (void)MEM_BufferFree(pValue);
    }
    else
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}

#if defined(gBLE51_d) && (gBLE51_d == TRUE)
#if defined(gGattCaching_d) && (gGattCaching_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientGetDatabaseHashOpCode(uint8_t *pBuffer,
*                                                            uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdClientGetDatabaseHashOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientGetDatabaseHashOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t     bondIdx = 0U;
    uint8_t*    pOutValue = NULL;
    uint16_t*   pOutValueLength = NULL;
    deviceId_t  deviceId = gInvalidDeviceId_c;

    /* Get deviceId and bondIdx parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bondIdx, pBuffer);

    /* Allocate buffer for the response */
    pOutValue       = fsciBleGattClientAllocOutOrIoValue(gGattDatabaseHashSize_c, deviceId, gUnenhancedBearerId_c);
    /* Allocate buffer for the value length */
    pOutValueLength = fsciBleGattClientAllocOutOrIoArraySize(deviceId, gUnenhancedBearerId_c);

    if((NULL != pOutValue) &&
       (NULL != pOutValueLength))
    {
        /* Set flag to indicate that the last GATT request was a Client
        request with out parameters */
        *pOutValueLength = gGattDatabaseHashSize_c;
        bGattClientRequestWithOutParameters = TRUE;
        fsciBleGattCallApiFunction(GattClient_GetDatabaseHash(deviceId, bondIdx));
    }
}
#endif /* gGattCaching_d */
#endif /* gBLE51_d */

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerInitOpCode(uint8_t *pBuffer,
*                                                 uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdServerInitOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerInitOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattServer_Init());
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerRegisterCallbackOpCode(uint8_t *pBuffer,
*                                                             uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdServerRegisterCallbackOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerRegisterCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattServer_RegisterCallback(fsciBleGattCallbacks.serverCallback));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerRegisterHandlesForWriteNotificationsOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdServerRegisterHandlesForWriteNotificationsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerRegisterHandlesForWriteNotificationsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t     handleCount = 0U;
    uint16_t*   pAttributeHandles = NULL;

    /* Read handleCount parameter from buffer */
    fsciBleGetUint8ValueFromBuffer(handleCount, pBuffer);

    /* Allocate buffer for pAttributeHandles */
    pAttributeHandles = (uint16_t*)MEM_BufferAlloc((uint32_t)handleCount * sizeof(uint16_t));

    if(NULL == pAttributeHandles)
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get pAttributeHandles parameter from the packet */
        fsciBleGetArrayFromBuffer(pAttributeHandles, pBuffer, (uint32_t)handleCount * sizeof(uint16_t));

        fsciBleGattCallApiFunction(GattServer_RegisterHandlesForWriteNotifications(handleCount, pAttributeHandles));

        /* Free the buffer used for pAttributeHandles */
        (void)MEM_BufferFree(pAttributeHandles);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerRegisterHandlesForReadNotificationsOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdServerRegisterHandlesForReadNotificationsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerRegisterHandlesForReadNotificationsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t     handleCount = 0U;
    uint16_t*   pAttributeHandles = NULL;

    /* Read handleCount parameter from buffer */
    fsciBleGetUint8ValueFromBuffer(handleCount, pBuffer);

    /* Allocate buffer for pAttributeHandles */
    pAttributeHandles = (uint16_t*)MEM_BufferAlloc((uint32_t)handleCount * sizeof(uint16_t));

    if(NULL == pAttributeHandles)
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get pAttributeHandles parameter from the packet */
        fsciBleGetArrayFromBuffer(pAttributeHandles, pBuffer, (uint32_t)handleCount * sizeof(uint16_t));

        fsciBleGattCallApiFunction(GattServer_RegisterHandlesForReadNotifications(handleCount, pAttributeHandles));

        /* Free the buffer used for pAttributeHandles */
        (void)MEM_BufferFree(pAttributeHandles);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerUnregisterHandlesForWriteNotificationsOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdServerUnregisterHandlesForWriteNotificationsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerUnregisterHandlesForWriteNotificationsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t     handleCount = 0U;
    uint16_t*   pAttributeHandles = NULL;

    /* Read handleCount parameter from buffer */
    fsciBleGetUint8ValueFromBuffer(handleCount, pBuffer);

    /* Allocate buffer for pAttributeHandles */
    pAttributeHandles = (uint16_t*)MEM_BufferAlloc((uint32_t)handleCount * sizeof(uint16_t));

    if(NULL == pAttributeHandles)
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get pAttributeHandles parameter from the packet */
        fsciBleGetArrayFromBuffer(pAttributeHandles, pBuffer, (uint32_t)handleCount * sizeof(uint16_t));

        fsciBleGattCallApiFunction(GattServer_UnregisterHandlesForWriteNotifications(handleCount, pAttributeHandles));

        /* Free the buffer used for pAttributeHandles */
        (void)MEM_BufferFree(pAttributeHandles);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerUnregisterHandlesForReadNotificationsOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdServerUnregisterHandlesForReadNotificationsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerUnregisterHandlesForReadNotificationsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t     handleCount = 0U;
    uint16_t*   pAttributeHandles = NULL;

    /* Read handleCount parameter from buffer */
    fsciBleGetUint8ValueFromBuffer(handleCount, pBuffer);

    /* Allocate buffer for pAttributeHandles */
    pAttributeHandles = (uint16_t*)MEM_BufferAlloc((uint32_t)handleCount * sizeof(uint16_t));

    if(NULL == pAttributeHandles)
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get pAttributeHandles parameter from the packet */
        fsciBleGetArrayFromBuffer(pAttributeHandles, pBuffer, (uint32_t)handleCount * sizeof(uint16_t));

        fsciBleGattCallApiFunction(GattServer_UnregisterHandlesForReadNotifications(handleCount, pAttributeHandles));

        /* Free the buffer used for pAttributeHandles */
        (void)MEM_BufferFree(pAttributeHandles);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerSendAttributeWrittenStatusOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdServerSendAttributeWrittenStatusOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerSendAttributeWrittenStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    attributeHandle = gGattDbInvalidHandle_d;
    uint8_t     status = (uint8_t)gBleSuccess_c;
    deviceId_t  deviceId = gInvalidDeviceId_c;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(attributeHandle, pBuffer);
    fsciBleGetUint8ValueFromBuffer(status, pBuffer);

    fsciBleGattCallApiFunction(GattServer_SendAttributeWrittenStatus(deviceId, attributeHandle, status));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerSendAttributeReadStatusOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdServerSendAttributeReadStatusOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerSendAttributeReadStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    attributeHandle = gGattDbInvalidHandle_d;
    uint8_t     status = (uint8_t)gBleSuccess_c;
    deviceId_t  deviceId = gInvalidDeviceId_c;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(attributeHandle, pBuffer);
    fsciBleGetUint8ValueFromBuffer(status, pBuffer);

    fsciBleGattCallApiFunction(GattServer_SendAttributeReadStatus(deviceId, attributeHandle, status));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerSendNotificationOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdServerSendNotificationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerSendNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    handle = gGattDbInvalidHandle_d;
    deviceId_t  deviceId = gInvalidDeviceId_c;

    /* Get deviceId and handle parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);

    fsciBleGattCallApiFunction(GattServer_SendNotification(deviceId, handle));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerSendIndicationOpCode(uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdServerSendIndicationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerSendIndicationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    handle = gGattDbInvalidHandle_d;
    deviceId_t  deviceId = gInvalidDeviceId_c;

    /* Get deviceId and handle parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);

    fsciBleGattCallApiFunction(GattServer_SendIndication(deviceId, handle));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerSendInstantValueNotificationOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdServerSendInstantValueNotificationOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerSendInstantValueNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    handle = gGattDbInvalidHandle_d;
    uint16_t    valueLength = 0U;
    uint8_t*    pValue = NULL;
    deviceId_t  deviceId = gInvalidDeviceId_c;

    /* Get deviceId, handle and valueLength parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);
    fsciBleGetUint16ValueFromBuffer(valueLength, pBuffer);

    if (valueLength <= gAttMaxValueLength_c)
    {
        /* Allocate buffer for pValue */
        pValue = MEM_BufferAlloc(valueLength);
    }

    if((0U < valueLength) &&
       (NULL == pValue))
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get pValue parameter from the packet */
        fsciBleGetArrayFromBuffer(pValue, pBuffer, valueLength);

        fsciBleGattCallApiFunction(GattServer_SendInstantValueNotification(deviceId, handle, valueLength, pValue));

        /* Free the buffer used for pValue */
        (void)MEM_BufferFree(pValue);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerSendInstantValueIndicationOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdServerSendInstantValueIndicationOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerSendInstantValueIndicationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    handle = 0U;
    uint16_t    valueLength = 0U;
    uint8_t*    pValue = NULL;
    deviceId_t  deviceId = gInvalidDeviceId_c;

    /* Get deviceId, handle and valueLength parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);
    fsciBleGetUint16ValueFromBuffer(valueLength, pBuffer);

    if (valueLength <= gAttMaxValueLength_c)
    {
      /* Allocate buffer for pValue */
      pValue = MEM_BufferAlloc(valueLength);
    }

    if((0U < valueLength) &&
       (NULL == pValue))
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get pValue parameter from the packet */
        fsciBleGetArrayFromBuffer(pValue, pBuffer, valueLength);

        fsciBleGattCallApiFunction(GattServer_SendInstantValueIndication(deviceId, handle, valueLength, pValue));

        /* Free the buffer used for pValue */
        (void)MEM_BufferFree(pValue);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerRegisterUniqueHandlesForNotificationsOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdServerRegisterUniqueHandlesForNotificationsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerRegisterUniqueHandlesForNotificationsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bool_t bWrite = FALSE;
    bool_t bRead = FALSE;

    fsciBleGetBoolValueFromBuffer(bWrite, pBuffer);
    fsciBleGetBoolValueFromBuffer(bRead, pBuffer);

    fsciBleGattCallApiFunction(GattServer_RegisterUniqueHandlesForNotifications(bWrite, bRead));
}

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientRegisterMultipleValueNotificationCallbackOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientRegisterMultipleValueNotificationCallbackOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientRegisterMultipleValueNotificationCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattClient_RegisterMultipleValueNotificationCallback(fsciBleGattCallbacks.clientMultipleValueNtfCallback));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerSendMultipleHandleValueNotificationOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdServerSendMultipleHandleValueNotificationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerSendMultipleHandleValueNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t     handleCount = gGattDbInvalidHandle_d;
    uint8_t     *pValue = NULL;
    uint32_t    totalLength = 0U;
    uint8_t     *pCrtPos = NULL;
    deviceId_t  deviceId = gInvalidDeviceId_c;

    /* Get deviceId, handle and valueLength parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(handleCount, pBuffer);

    /* Get the variable size for the needed buffer */
    pCrtPos = pBuffer;
    for(uint32_t iCount = 0; iCount < handleCount; iCount++)
    {
        uint16_t attLength = 0U;
        pBuffer += sizeof(uint16_t);
        fsciBleGetUint16ValueFromBuffer(attLength, pBuffer);
        pBuffer += attLength;
        totalLength += ((uint32_t)attLength + 2U*sizeof(uint16_t));
    }

    if (totalLength <= gAttMaxValueLength_c)
    {
        /* Allocate buffer for pValue */
        pValue = MEM_BufferAlloc(totalLength);
    }

    if((0U < totalLength) && (NULL == pValue))
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get pValue parameter from the packet */
        fsciBleGetArrayFromBuffer(pValue, pCrtPos, totalLength);

        fsciBleGattCallApiFunction(GattServer_SendMultipleHandleValueNotification(deviceId, totalLength, pValue));

        /* Free the buffer used for pValue */
        (void)MEM_BufferFree(pValue);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientReadMultipleVariableCharacteristicValuesOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientReadMultipleVariableCharacteristicValuesOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientReadMultipleVariableCharacteristicValuesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattCharacteristic_t*   pIoCharacteristics = NULL;
    uint16_t                maxValueLength = 0;
    uint8_t*                pOutValue = NULL;
    deviceId_t              deviceId = gInvalidDeviceId_c;
    bool_t                  earlyReturn = FALSE;

    /* Get deviceId and number of characteristics parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(fsciBleGattClientNbOfCharacteristics, pBuffer);

    /* Allocate buffer for the characteristics - considering that fsciBleGattClientNbOfCharacteristics
    is bigger than 0 */
    pIoCharacteristics = fsciBleGattClientAllocOutOrIoCharacteristics(fsciBleGattClientNbOfCharacteristics, deviceId, gUnenhancedBearerId_c);

    if(NULL != pIoCharacteristics)
    {
        /* Get all characteristics from the received packet */
        for(uint32_t iCount = 0; iCount < fsciBleGattClientNbOfCharacteristics; iCount++)
        {
            fsciBleGattClientGetCharFromBuffer(&pIoCharacteristics[iCount], &pBuffer);

            if (pIoCharacteristics[iCount].value.maxValueLength <= gAttMaxValueLength_c)
            {
                /* Compute the total length of all the characteristics values */
                maxValueLength += pIoCharacteristics[iCount].value.maxValueLength;
            }
            else
            {
                fsciBleError(gFsciError_c, fsciInterfaceId);
                fsciBleGattClientEraseTmpInfo(TRUE, deviceId, gUnenhancedBearerId_c);
                earlyReturn = TRUE;
                break;
            }
        }

        if (!earlyReturn)
        {
            /* Allocate a single buffer for all the characteristics values - considering
            that maxValueLength is bigger than 0 */
            pOutValue = fsciBleGattClientAllocOutOrIoValue(maxValueLength, deviceId, gUnenhancedBearerId_c);

            if(NULL != pOutValue)
            {
                /* Set paValue field for all the characteristics to the corresponding position in the
                value buffer */
                for(uint32_t iCount = 0; iCount < fsciBleGattClientNbOfCharacteristics; iCount++)
                {
                    pIoCharacteristics[iCount].value.paValue = pOutValue;

                    pOutValue += pIoCharacteristics[iCount].value.maxValueLength;
                }

                /* Set flag to indicate that the last GATT request was a Client
                request with out parameters */
                bGattClientRequestWithOutParameters = TRUE;
                fsciBleGattCallApiFunction(GattClient_ReadMultipleVariableCharacteristicValues(deviceId, fsciBleGattClientNbOfCharacteristics, pIoCharacteristics));
            }
            else
            {
                fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
            }
        }
    }
    else
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}

#if defined(gEATT_d) && (gEATT_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientRegisterEnhancedProcedureCallbackOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientRegisterEnhancedProcedureCallbackOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientRegisterEnhancedProcedureCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattClient_RegisterEnhancedProcedureCallback(fsciBleGattCallbacks.clientEnhancedProcedureCallback));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientRegisterEnhancedNotificationCallbackOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientRegisterEnhancedNotificationCallbackOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientRegisterEnhancedNotificationCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattClient_RegisterEnhancedNotificationCallback(fsciBleGattCallbacks.clientEnhancedNotificationCallback));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientRegisterEnhancedIndicationCallbackOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientRegisterEnhancedIndicationCallbackOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientRegisterEnhancedIndicationCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattClient_RegisterEnhancedIndicationCallback(fsciBleGattCallbacks.clientEnhancedIndicationCallback));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientRegisterEnhancedMultipleValueNotificationCallbackOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientRegisterEnhancedMultipleValueNotificationCallbackOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientRegisterEnhancedMultipleValueNotificationCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattClient_RegisterEnhancedMultipleValueNotificationCallback(fsciBleGattCallbacks.clientEnhancedMultipleValueNtfCallback));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedDiscoverAllPrimaryServicesOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedDiscoverAllPrimaryServicesOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedDiscoverAllPrimaryServicesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t         maxServiceCount = 0U;
    gattService_t*  pOutServices = NULL;
    uint8_t*        pOutServiceCount = NULL;
    deviceId_t      deviceId = gInvalidDeviceId_c;
    bearerId_t      bearerId = gUnenhancedBearerId_c;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetUint8ValueFromBuffer(maxServiceCount, pBuffer);

    /* Allocate buffer for the services array - consider that maxServiceCount
     is bigger than 0 */
    pOutServices        = fsciBleGattClientAllocOutOrIoServices(maxServiceCount, deviceId, bearerId);
    /* Allocate buffer for the services array count */
    pOutServiceCount    = (uint8_t*)fsciBleGattClientAllocOutOrIoArraySize(deviceId, bearerId);

    if((NULL != pOutServices) &&
       (NULL != pOutServiceCount))
    {
        /* Set flag to indicate that the last GATT request was a Client
        request with out parameters */
        bGattClientRequestWithOutParameters = TRUE;

        fsciBleGattCallApiFunction(GattClient_EnhancedDiscoverAllPrimaryServices(deviceId, bearerId,
                                                                         pOutServices,
                                                                         maxServiceCount,
                                                                         pOutServiceCount));
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedDiscoverPrimaryServicesByUuidOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedDiscoverPrimaryServicesByUuidOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedDiscoverPrimaryServicesByUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleUuidType_t   uuidType = {0};
    bleUuid_t       uuid = {0};
    uint8_t         maxServiceCount = 0U;
    gattService_t*  pOutServices = NULL;
    uint8_t*        pOutServiceCount = NULL;
    deviceId_t      deviceId = gInvalidDeviceId_c;
    bearerId_t      bearerId = gUnenhancedBearerId_c;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetEnumValueFromBuffer(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&uuid, &pBuffer, uuidType);
    fsciBleGetUint8ValueFromBuffer(maxServiceCount, pBuffer);

    /* Allocate buffer for the services array - consider that maxServiceCount
    is bigger than 0 */
    pOutServices        = fsciBleGattClientAllocOutOrIoServices(maxServiceCount, deviceId, bearerId);
    /* Allocate buffer for the services array count */
    pOutServiceCount    = (uint8_t*)fsciBleGattClientAllocOutOrIoArraySize(deviceId, bearerId);

    if((NULL != pOutServices) ||
       (NULL != pOutServiceCount))
    {
        /* Set flag to indicate that the last GATT request was a Client
        request with out parameters */
        bGattClientRequestWithOutParameters = TRUE;

        fsciBleGattCallApiFunction(GattClient_EnhancedDiscoverPrimaryServicesByUuid(deviceId, bearerId, uuidType, &uuid,
                                                                            pOutServices,
                                                                            maxServiceCount,
                                                                            pOutServiceCount));
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedFindIncludedServicesOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedFindIncludedServicesOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedFindIncludedServicesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattService_t*  pIoService = NULL;
    deviceId_t      deviceId = gInvalidDeviceId_c;
    bearerId_t      bearerId = gUnenhancedBearerId_c;

    /* Get deviceId from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);

    /* Allocate buffer for the service */
    pIoService = fsciBleGattClientAllocOutOrIoServices(1, deviceId, bearerId);

    if(NULL != pIoService)
    {
        uint8_t         maxIncludedServiceCount = 0U;
        gattService_t*  pOutIncludedServices = NULL;

        /* Get more parameters from the received packet */
        fsciBleGattClientGetServiceFromBuffer(pIoService, &pBuffer);
        fsciBleGetUint8ValueFromBuffer(maxIncludedServiceCount, pBuffer);

        /* Allocate buffer for the included services - consider that maxIncludedServiceCount
        is bigger than 0 */
        pOutIncludedServices = fsciBleGattClientAllocOutOrIoIncludedServices(maxIncludedServiceCount, deviceId, bearerId);

        if(NULL != pOutIncludedServices)
        {
            pIoService->aIncludedServices = pOutIncludedServices;

            /* Set flag to indicate that the last GATT request was a Client
            request with out parameters */
            bGattClientRequestWithOutParameters = TRUE;

            fsciBleGattCallApiFunction(GattClient_EnhancedFindIncludedServices(deviceId, bearerId, pIoService,
                                                                       maxIncludedServiceCount));
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedDiscoverAllCharacteristicsOfServiceOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedDiscoverAllCharacteristicsOfServiceOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedDiscoverAllCharacteristicsOfServiceOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattService_t*  pIoService = NULL;
    deviceId_t      deviceId = gInvalidDeviceId_c;
    bearerId_t      bearerId = gUnenhancedBearerId_c;

    /* Get deviceId from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);

    /* Allocate buffer for the service */
    pIoService = fsciBleGattClientAllocOutOrIoServices(1, deviceId, bearerId);

    if(NULL != pIoService)
    {
        uint8_t                 maxCharacteristicCount = 0U;
        gattCharacteristic_t*   pOutCharacteristics = NULL;

        /* Get more parameters from the received packet */
        fsciBleGattClientGetServiceFromBuffer(pIoService, &pBuffer);
        fsciBleGetUint8ValueFromBuffer(maxCharacteristicCount, pBuffer);

        /* Allocate buffer for the characteristics - consider that maxCharacteristicCount
        is bigger than 0 */
        pOutCharacteristics = fsciBleGattClientAllocOutOrIoCharacteristics(maxCharacteristicCount, deviceId, bearerId);

        if(NULL != pOutCharacteristics)
        {
            pIoService->aCharacteristics = pOutCharacteristics;

            /* Set flag to indicate that the last GATT request was a Client
            request with out parameters */
            bGattClientRequestWithOutParameters = TRUE;

            fsciBleGattCallApiFunction(GattClient_EnhancedDiscoverAllCharacteristicsOfService(deviceId, bearerId, pIoService,
                                                                                      maxCharacteristicCount));
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedDiscoverCharacteristicOfServiceByUuidOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedDiscoverCharacteristicOfServiceByUuidOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedDiscoverCharacteristicOfServiceByUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleUuidType_t           uuidType = {0};
    bleUuid_t               uuid = {0};
    gattService_t           service = {0};
    uint8_t                 maxCharacteristicCount = 0U;
    gattCharacteristic_t*   pOutCharacteristics = NULL;
    uint8_t*                pOutCharacteristicCount = NULL;
    deviceId_t              deviceId = gInvalidDeviceId_c;
    bearerId_t              bearerId = gUnenhancedBearerId_c;


    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetEnumValueFromBuffer(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&uuid, &pBuffer, uuidType);
    fsciBleGattClientGetServiceFromBuffer(&service, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(maxCharacteristicCount, pBuffer);

    /* Allocate buffer for the characteristics - consider that maxCharacteristicCount
    is bigger than 0 */
    pOutCharacteristics     = fsciBleGattClientAllocOutOrIoCharacteristics(maxCharacteristicCount, deviceId, bearerId);
    /* Allocate buffer for the characteristics array count */
    pOutCharacteristicCount = (uint8_t*)fsciBleGattClientAllocOutOrIoArraySize(deviceId, bearerId);

    if((NULL != pOutCharacteristics) &&
       (NULL != pOutCharacteristicCount))
    {
        /* Set flag to indicate that the last GATT request was a Client
        request with out parameters */
        bGattClientRequestWithOutParameters = TRUE;

        fsciBleGattCallApiFunction(GattClient_EnhancedDiscoverCharacteristicOfServiceByUuid(deviceId, bearerId, uuidType, &uuid,
                                                                                    &service, pOutCharacteristics,
                                                                                    maxCharacteristicCount,
                                                                                    pOutCharacteristicCount));
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedDiscoverAllCharacteristicDescriptorsOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedDiscoverAllCharacteristicDescriptorsOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedDiscoverAllCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattCharacteristic_t*   pIoCharacteristic = NULL;
    deviceId_t              deviceId = gInvalidDeviceId_c;
    bearerId_t              bearerId = gUnenhancedBearerId_c;

    /* Get deviceId from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);

    /* Allocate buffer for the characteristic */
    pIoCharacteristic = fsciBleGattClientAllocOutOrIoCharacteristics(1, deviceId, bearerId);

    if(NULL != pIoCharacteristic)
    {
        uint16_t            endingHandle = 0U;
        uint8_t             maxDescriptorCount = 0U;
        gattAttribute_t*    pOutDescriptors = NULL;

        /* Get more parameters from the received packet */
        fsciBleGattClientGetCharFromBuffer(pIoCharacteristic, &pBuffer);
        fsciBleGetUint16ValueFromBuffer(endingHandle, pBuffer);
        fsciBleGetUint8ValueFromBuffer(maxDescriptorCount, pBuffer);

        /* Allocate buffer for the descriptors - consider that maxDescriptorCount
        is bigger than 0 */
        pOutDescriptors = fsciBleGattClientAllocOutOrIoAttributes(maxDescriptorCount, deviceId, bearerId);

        if(NULL != pOutDescriptors)
        {
            pIoCharacteristic->aDescriptors = pOutDescriptors;

            /* Set flag to indicate that the last GATT request was a Client
            request with out parameters */
            bGattClientRequestWithOutParameters = TRUE;

            fsciBleGattCallApiFunction(GattClient_EnhancedDiscoverAllCharacteristicDescriptors(deviceId, bearerId, pIoCharacteristic,
                                                                                       endingHandle,
                                                                                       maxDescriptorCount));
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedReadCharacteristicValueOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedReadCharacteristicValueOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedReadCharacteristicValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattCharacteristic_t*   pIoCharacteristic = NULL;
    deviceId_t              deviceId = gInvalidDeviceId_c;
    bearerId_t              bearerId = gUnenhancedBearerId_c;

    /* Get deviceId from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);

    /* Allocate buffer for the characteristic */
    pIoCharacteristic = fsciBleGattClientAllocOutOrIoCharacteristics(1, deviceId, bearerId);

    if(NULL != pIoCharacteristic)
    {
        uint16_t    maxValueLength = 0U;
        uint8_t*    pOutValue = NULL;

        /* Get more parameters from the received packet */
        fsciBleGattClientGetCharFromBuffer(pIoCharacteristic, &pBuffer);
        fsciBleGetUint16ValueFromBuffer(maxValueLength, pBuffer);

        /* Allocate buffer for the descriptors - consider that maxValueLength
        is bigger than 0 */
        pOutValue = fsciBleGattClientAllocOutOrIoValue(maxValueLength, deviceId, bearerId);

        if(NULL != pOutValue)
        {
            pIoCharacteristic->value.paValue = pOutValue;

            /* Set flag to indicate that the last GATT request was a Client
            request with out parameters */
            bGattClientRequestWithOutParameters = TRUE;

            fsciBleGattCallApiFunction(GattClient_EnhancedReadCharacteristicValue(deviceId, bearerId,
                                                                          pIoCharacteristic,
                                                                          maxValueLength));
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedReadUsingCharacteristicUuidOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedReadUsingCharacteristicUuidOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedReadUsingCharacteristicUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleUuidType_t       uuidType = {0};
    bleUuid_t           uuid = {0};
    bool_t              bHandleRangeIncluded = FALSE;
    gattHandleRange_t   handleRange = {0};
    gattHandleRange_t*  pHandleRange = NULL;
    uint16_t            maxValueLength = 0U;
    uint8_t*            pOutValue = NULL;
    uint16_t*           pOutValueLength = NULL;
    deviceId_t          deviceId = gInvalidDeviceId_c;
    bearerId_t          bearerId = gUnenhancedBearerId_c;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetEnumValueFromBuffer(uuidType, pBuffer, bleUuidType_t);
    fsciBleGetUuidFromBuffer(&uuid, &pBuffer, uuidType);
    fsciBleGetBoolValueFromBuffer(bHandleRangeIncluded, pBuffer);

    if(TRUE == bHandleRangeIncluded)
    {
        fsciBleGattGetHandleRangeFromBuffer(&handleRange, &pBuffer);
        pHandleRange = &handleRange;
    }

    fsciBleGetUint16ValueFromBuffer(maxValueLength, pBuffer);

    /* Allocate buffer for the value - consider that maxValueLength
    is bigger than 0 */
    pOutValue       = fsciBleGattClientAllocOutOrIoValue(maxValueLength, deviceId, bearerId);
    /* Allocate buffer for the value length */
    pOutValueLength = fsciBleGattClientAllocOutOrIoArraySize(deviceId, bearerId);

    if((NULL != pOutValue) &&
       (NULL != pOutValueLength))
    {
        /* Set flag to indicate that the last GATT request was a Client
        request with out parameters */
        bGattClientRequestWithOutParameters = TRUE;

        fsciBleGattCallApiFunction(GattClient_EnhancedReadUsingCharacteristicUuid(deviceId, bearerId, uuidType, &uuid,
                                                                          pHandleRange, pOutValue,
                                                                          maxValueLength, pOutValueLength));
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedReadMultipleCharacteristicValuesOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedReadMultipleCharacteristicValuesOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedReadMultipleCharacteristicValuesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattCharacteristic_t*   pIoCharacteristics = NULL;
    deviceId_t              deviceId = gInvalidDeviceId_c;
    bearerId_t              bearerId = gUnenhancedBearerId_c;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetUint8ValueFromBuffer(fsciBleGattClientNbOfCharacteristics, pBuffer);

    /* Allocate buffer for the characteristics - considering that fsciBleGattClientNbOfCharacteristics
    is bigger than 0 */
    pIoCharacteristics = fsciBleGattClientAllocOutOrIoCharacteristics(fsciBleGattClientNbOfCharacteristics, deviceId, bearerId);

    if(NULL != pIoCharacteristics)
    {
        uint16_t    maxValueLength = 0U;
        uint8_t*    pOutValue = NULL;
        uint8_t     iCount = 0U;
        bool_t earlyReturn = FALSE;

        /* Get all characteristics from the received packet */
        for(iCount = 0; iCount < fsciBleGattClientNbOfCharacteristics; iCount++)
        {
            fsciBleGattClientGetCharFromBuffer(&pIoCharacteristics[iCount], &pBuffer);

            if (pIoCharacteristics[iCount].value.maxValueLength <= gAttMaxValueLength_c)
            {
                /* Compute the total length of all the characteristics values */
                maxValueLength += pIoCharacteristics[iCount].value.maxValueLength;
            }
            else
            {
                fsciBleError(gFsciError_c, fsciInterfaceId);
                fsciBleGattClientEraseTmpInfo(TRUE, deviceId, bearerId);
                earlyReturn = TRUE;
                break;
            }
        }

        if (!earlyReturn)
        {
            /* Allocate a single buffer for all the characteristics values - considering
            that maxValueLength is bigger than 0 */
            pOutValue = fsciBleGattClientAllocOutOrIoValue(maxValueLength, deviceId, bearerId);

            if(NULL != pOutValue)
            {
                /* Set paValue field for all the characteristics to the corresponding position in the
                value buffer */
                for(iCount = 0; iCount < fsciBleGattClientNbOfCharacteristics; iCount++)
                {
                    pIoCharacteristics[iCount].value.paValue = pOutValue;

                    pOutValue += pIoCharacteristics[iCount].value.maxValueLength;
                }

                /* Set flag to indicate that the last GATT request was a Client
                request with out parameters */
                bGattClientRequestWithOutParameters = TRUE;

                fsciBleGattCallApiFunction(GattClient_EnhancedReadMultipleCharacteristicValues(deviceId, bearerId,
                                                                                       fsciBleGattClientNbOfCharacteristics,
                                                                                       pIoCharacteristics));
            }
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedWriteCharacteristicValueOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedWriteCharacteristicValueOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedWriteCharacteristicValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattCharacteristic_t    characteristic = {0};
    uint16_t                valueLength = 0U;
    uint8_t*                pValue = NULL;
    deviceId_t              deviceId = gInvalidDeviceId_c;
    bearerId_t              bearerId = gUnenhancedBearerId_c;

    characteristic.aDescriptors = NULL;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGattClientGetCharFromBuffer(&characteristic, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(valueLength, pBuffer);

    if (valueLength <= gAttMaxValueLength_c)
    {
        /* Allocate buffer for the value */
        pValue = MEM_BufferAlloc(valueLength);
    }

    if((0U == valueLength) ||
       (NULL != pValue))
    {
        bool_t      bWithoutResponse = FALSE;
        bool_t      bDoReliableLongCharWrites = FALSE;
        uint8_t     csrk[gcSmpCsrkSize_c] = {0};

        /* Get more parameters from the received packet */
        fsciBleGetArrayFromBuffer(pValue, pBuffer, valueLength);
        fsciBleGetBoolValueFromBuffer(bWithoutResponse, pBuffer);
        fsciBleGetBoolValueFromBuffer(bDoReliableLongCharWrites, pBuffer);
        fsciBleGetCsrkFromBuffer(csrk, pBuffer);

        fsciBleGattCallApiFunction(GattClient_EnhancedWriteCharacteristicValue(deviceId, bearerId, &characteristic,
                                                                       valueLength, pValue,
                                                                       bWithoutResponse,
                                                                       bDoReliableLongCharWrites,
                                                                       csrk));

        (void)MEM_BufferFree(pValue);
    }
    else
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedReadCharacteristicDescriptorsOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedReadCharacteristicDescriptorsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedReadCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattAttribute_t*    pIoDescriptor = NULL;
    deviceId_t          deviceId = gInvalidDeviceId_c;
    bearerId_t          bearerId = gUnenhancedBearerId_c;

    /* Get deviceId from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);

    /* Allocate buffer for the descriptor */
    pIoDescriptor = fsciBleGattClientAllocOutOrIoAttributes(1, deviceId, bearerId);

    if(NULL != pIoDescriptor)
    {
        uint16_t    maxValueLength = 0U;
        uint8_t*    pOutValue = NULL;

        /* Get more parameters from the received packet */
        fsciBleGattClientGetAttributeFromBuffer(pIoDescriptor, &pBuffer);
        fsciBleGetUint16ValueFromBuffer(maxValueLength, pBuffer);

        /* Allocate buffer for the value - considering that maxValueLength
        is bigger than 0 */
        pOutValue = fsciBleGattClientAllocOutOrIoValue(maxValueLength, deviceId, bearerId);

        if(NULL != pOutValue)
        {
            pIoDescriptor->paValue = pOutValue;

            /* Set flag to indicate that the last GATT request was a Client
            request with out parameters */
            bGattClientRequestWithOutParameters = TRUE;

            fsciBleGattCallApiFunction(GattClient_EnhancedReadCharacteristicDescriptor(deviceId, bearerId,
                                                                               pIoDescriptor,
                                                                               maxValueLength));
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedWriteCharacteristicDescriptorsOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedWriteCharacteristicDescriptorsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedWriteCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattAttribute_t descriptor = {0};
    uint16_t        valueLength = 0U;
    uint8_t*        pValue = NULL;
    deviceId_t      deviceId = gInvalidDeviceId_c;
    bearerId_t      bearerId = gUnenhancedBearerId_c;

    descriptor.paValue = NULL;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGattClientGetAttributeFromBuffer(&descriptor, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(valueLength, pBuffer);

    if (valueLength <= gAttMaxValueLength_c)
    {
        /* Allocate buffer for the value */
        pValue = MEM_BufferAlloc(valueLength);
    }

    if((0U == valueLength) ||
       (NULL != pValue))
    {
        /* Get value from the received packet */
        fsciBleGetArrayFromBuffer(pValue, pBuffer, valueLength);

        fsciBleGattCallApiFunction(GattClient_EnhancedWriteCharacteristicDescriptor(deviceId, bearerId, &descriptor,
                                                                            valueLength, pValue));

        (void)MEM_BufferFree(pValue);
    }
    else
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerRegisterEnhancedCallbackOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdServerRegisterEnhancedCallbackOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerRegisterEnhancedCallbackOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattCallApiFunction(GattServer_RegisterCallback(fsciBleGattCallbacks.serverCallback));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerEnhancedSendAttributeWrittenStatusOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdServerEnhancedSendAttributeWrittenStatusOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerEnhancedSendAttributeWrittenStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    attributeHandle = gGattDbInvalidHandle_d;
    uint8_t     status = 0U;
    deviceId_t  deviceId = gInvalidDeviceId_c;
    bearerId_t  bearerId = gUnenhancedBearerId_c;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetUint16ValueFromBuffer(attributeHandle, pBuffer);
    fsciBleGetUint8ValueFromBuffer(status, pBuffer);

    fsciBleGattCallApiFunction(GattServer_EnhancedSendAttributeWrittenStatus(deviceId, bearerId, attributeHandle, status));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerEnhancedSendAttributeReadStatusOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdServerEnhancedSendAttributeReadStatusOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerEnhancedSendAttributeReadStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    attributeHandle = gGattDbInvalidHandle_d;
    uint8_t     status = 0U;
    deviceId_t  deviceId = gInvalidDeviceId_c;
    bearerId_t  bearerId = gUnenhancedBearerId_c;

    /* Get function parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetUint16ValueFromBuffer(attributeHandle, pBuffer);
    fsciBleGetUint8ValueFromBuffer(status, pBuffer);

    fsciBleGattCallApiFunction(GattServer_EnhancedSendAttributeReadStatus(deviceId, bearerId, attributeHandle, status));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerEnhancedSendNotificationOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdServerEnhancedSendNotificationOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerEnhancedSendNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    handle = gGattDbInvalidHandle_d;
    deviceId_t  deviceId = gInvalidDeviceId_c;
    bearerId_t  bearerId = gUnenhancedBearerId_c;

    /* Get deviceId and handle parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);

    fsciBleGattCallApiFunction(GattServer_EnhancedSendNotification(deviceId, bearerId, handle));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerEnhancedSendIndicationOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the gBleGattCmdServerEnhancedSendIndicationOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerEnhancedSendIndicationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    handle = gGattDbInvalidHandle_d;
    deviceId_t  deviceId = gInvalidDeviceId_c;
    bearerId_t  bearerId = gUnenhancedBearerId_c;

    /* Get deviceId and handle parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);

    fsciBleGattCallApiFunction(GattServer_EnhancedSendIndication(deviceId, bearerId, handle));
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerEnhancedSendInstantValueNotificationOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdServerEnhancedSendInstantValueNotificationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerEnhancedSendInstantValueNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    handle = gGattDbInvalidHandle_d;
    uint16_t    valueLength = 0U;
    uint8_t*    pValue = NULL;
    deviceId_t  deviceId = gInvalidDeviceId_c;
    bearerId_t  bearerId = gUnenhancedBearerId_c;

    /* Get deviceId, handle and valueLength parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);
    fsciBleGetUint16ValueFromBuffer(valueLength, pBuffer);

    if (valueLength <= gAttMaxValueLength_c)
    {
        /* Allocate buffer for pValue */
        pValue = MEM_BufferAlloc(valueLength);
    }

    if((0U < valueLength) &&
       (NULL == pValue))
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get pValue parameter from the packet */
        fsciBleGetArrayFromBuffer(pValue, pBuffer, valueLength);

        fsciBleGattCallApiFunction(GattServer_EnhancedSendInstantValueNotification(deviceId, bearerId, handle, valueLength, pValue));

        /* Free the buffer used for pValue */
        (void)MEM_BufferFree(pValue);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerEnhancedSendInstantValueIndicationOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdServerEnhancedSendInstantValueIndicationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerEnhancedSendInstantValueIndicationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    handle = gGattDbInvalidHandle_d;
    uint16_t    valueLength = 0U;
    uint8_t*    pValue = NULL;
    deviceId_t  deviceId = gInvalidDeviceId_c;
    bearerId_t  bearerId = gUnenhancedBearerId_c;

    /* Get deviceId, handle and valueLength parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetUint16ValueFromBuffer(handle, pBuffer);
    fsciBleGetUint16ValueFromBuffer(valueLength, pBuffer);

    if (valueLength <= gAttMaxValueLength_c)
    {
        /* Allocate buffer for pValue */
        pValue = MEM_BufferAlloc(valueLength);
    }

    if((0U < valueLength) &&
       (NULL == pValue))
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get pValue parameter from the packet */
        fsciBleGetArrayFromBuffer(pValue, pBuffer, valueLength);

        fsciBleGattCallApiFunction(GattServer_EnhancedSendInstantValueIndication(deviceId, bearerId, handle, valueLength, pValue));

        /* Free the buffer used for pValue */
        (void)MEM_BufferFree(pValue);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdServerEnhancedSendMultipleHandleValueNotificationOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdServerEnhancedSendMultipleHandleValueNotificationOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdServerEnhancedSendMultipleHandleValueNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint8_t     *pValue = NULL;
    uint32_t    totalLength = 0U;
    deviceId_t  deviceId = gInvalidDeviceId_c;
    bearerId_t  bearerId = gUnenhancedBearerId_c;

    /* Get deviceId, handle and totalValue parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetUint32ValueFromBuffer(totalLength, pBuffer);

    if (totalLength <= gAttMaxValueLength_c)
    {
        /* Allocate buffer for pValue */
        pValue = MEM_BufferAlloc(totalLength);
    }

    if((0U < totalLength) && (NULL == pValue))
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get pValue parameter from the packet */
        fsciBleGetArrayFromBuffer(pValue, pBuffer, totalLength);

        fsciBleGattCallApiFunction(GattServer_EnhancedSendMultipleHandleValueNotification(deviceId, bearerId, totalLength, pValue));

        /* Free the buffer used for pValue */
        (void)MEM_BufferFree(pValue);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattCmdClientEnhancedReadMultipleVariableCharacteristicValuesOpCode(
*                                                           uint8_t *pBuffer,
*                                                           uint32_t fsciInterfaceId)
*\brief        Handler for the
*              gBleGattCmdClientEnhancedReadMultipleVariableCharacteristicValuesOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattCmdClientEnhancedReadMultipleVariableCharacteristicValuesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattCharacteristic_t*   pIoCharacteristics = NULL;
    uint16_t                maxValueLength = 0;
    uint8_t*                pOutValue = NULL;
    deviceId_t              deviceId = gInvalidDeviceId_c;
    bearerId_t              bearerId = gUnenhancedBearerId_c;
    bool_t                  earlyReturn = FALSE;

    /* Get deviceId and number of characteristics parameters from the received packet */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint8ValueFromBuffer(bearerId, pBuffer);
    fsciBleGetUint8ValueFromBuffer(fsciBleGattClientNbOfCharacteristics, pBuffer);

    /* Allocate buffer for the characteristics - considering that fsciBleGattClientNbOfCharacteristics
    is bigger than 0 */
    pIoCharacteristics = fsciBleGattClientAllocOutOrIoCharacteristics(fsciBleGattClientNbOfCharacteristics, deviceId, bearerId);

    if(NULL != pIoCharacteristics)
    {
        /* Get all characteristics from the received packet */
        for(uint32_t iCount = 0; iCount < fsciBleGattClientNbOfCharacteristics; iCount++)
        {
            fsciBleGattClientGetCharFromBuffer(&pIoCharacteristics[iCount], &pBuffer);

            if (pIoCharacteristics[iCount].value.maxValueLength <= gAttMaxValueLength_c)
            {
                /* Compute the total length of all the characteristics values */
                maxValueLength += pIoCharacteristics[iCount].value.maxValueLength;
            }
            else
            {
                fsciBleError(gFsciError_c, fsciInterfaceId);
                fsciBleGattClientEraseTmpInfo(TRUE, deviceId, bearerId);
                earlyReturn = TRUE;
                break;
            }
        }

        if(!earlyReturn)
        {
            /* Allocate a single buffer for all the characteristics values - considering
            that maxValueLength is bigger than 0 */
            pOutValue = fsciBleGattClientAllocOutOrIoValue(maxValueLength, deviceId, bearerId);

            if(NULL != pOutValue)
            {
                /* Set paValue field for all the characteristics to the corresponding position in the
                value buffer */
                for(uint32_t iCount = 0; iCount < fsciBleGattClientNbOfCharacteristics; iCount++)
                {
                    pIoCharacteristics[iCount].value.paValue = pOutValue;

                    pOutValue += pIoCharacteristics[iCount].value.maxValueLength;
                }

                /* Set flag to indicate that the last GATT request was a Client
                request with out parameters */
                bGattClientRequestWithOutParameters = TRUE;
                fsciBleGattCallApiFunction(GattClient_EnhancedReadMultipleVariableCharacteristicValues(deviceId, bearerId, fsciBleGattClientNbOfCharacteristics, pIoCharacteristics));
            }
            else
            {
                fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
            }
        }
    }
    else
    {
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
}
#endif /* gEATT_d */
#endif /* gBLE52_d */
#endif /* gFsciBleBBox_d || gFsciBleTest_d */

#if gFsciBleHost_d
/*! *********************************************************************************
*\private
*\fn           void HandleGattStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattStatusOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattStatusOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    bleResult_t status = gBleSuccess_c;
    deviceId_t  deviceId = gInvalidDeviceId_c;

    /* Get status from buffer */
    fsciBleGetEnumValueFromBuffer(status, pBuffer, bleResult_t);

    if(gBleSuccess_c != status)
    {
        /* Clean out parameters pointers kept in FSCI (only for synchronous function) */
        fsciBleGattCleanOutParams();

        /* Erase the information kept (for asynchronous functions) */
        fsciBleGattClientEraseInfo(FALSE, deviceId, bearerId);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtGetMtuOpCode(uint8_t *pBuffer,
*                                             uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtGetMtuOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtGetMtuOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    fsciBleGattGetMtuOutParams_t* pOutParams = (fsciBleGattGetMtuOutParams_t*)fsciBleGattRestoreOutParams();

    if(NULL != pOutParams->pMtu)
    {
        /* Get out parameter of the Gatt_GetMtu function from buffer */
        fsciBleGetUint16ValueFromBuffer(*pOutParams->pMtu, pBuffer);

        /* Clean out parameters pointers kept in FSCI */
        fsciBleGattCleanOutParams();

        /* Signal out parameters ready */
        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureExchangeMtuOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtClientProcedureExchangeMtuOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureExchangeMtuOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcExchangeMtu_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureDiscoverAllPrimaryServicesOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the
*              gBleGattEvtClientProcedureDiscoverAllPrimaryServicesOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureDiscoverAllPrimaryServicesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcDiscoverAllPrimaryServices_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Get services array size parameter from buffer */
        fsciBleGetUint8ValueFromBuffer(*(uint8_t*)pArraySize, pBuffer);
        /* Get services array parameter from buffer */
        for(iCount = 0; iCount < *(uint8_t*)pArraySize; iCount++)
        {
            fsciBleGattClientGetServiceFromBuffer(&pServices[iCount], &pBuffer);
        }
    }

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureDiscoverPrimaryServicesByUuidOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the
*              gBleGattEvtClientProcedureDiscoverPrimaryServicesByUuidOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureDiscoverPrimaryServicesByUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcDiscoverPrimaryServicesByUuid_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Get services array size parameter from buffer */
        fsciBleGetUint8ValueFromBuffer(*(uint8_t*)pArraySize, pBuffer);
        /* Get services array parameter from buffer */
        for(iCount = 0; iCount < *(uint8_t*)pArraySize; iCount++)
        {
            fsciBleGattClientGetServiceFromBuffer(&pServices[iCount], &pBuffer);
        }
    }

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureFindIncludedServicesOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtClientProcedureFindIncludedServicesOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureFindIncludedServicesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcFindIncludedServices_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Get service parameter from buffer */
        fsciBleGattClientGetServiceFromBuffer(pServices, &pBuffer);
    }

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureDiscoverAllCharacteristicsOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the
*              gBleGattEvtClientProcedureDiscoverAllCharacteristicsOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureDiscoverAllCharacteristicsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcDiscoverAllCharacteristics_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Get service parameter from buffer */
        fsciBleGattClientGetServiceFromBuffer(pServices, &pBuffer);
    }

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureDiscoverCharacteristicByUuidOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the
*              gBleGattEvtClientProcedureDiscoverCharacteristicByUuidOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureDiscoverCharacteristicByUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcDiscoverCharacteristicByUuid_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Get characteristics array size parameter from buffer */
        fsciBleGetUint8ValueFromBuffer(*(uint8_t*)pArraySize, pBuffer);
        /* Get characteristics array parameter from buffer */
        for(iCount = 0; iCount < *(uint8_t*)pArraySize; iCount++)
        {
            fsciBleGattClientGetCharFromBuffer(&pCharacteristics[iCount], &pBuffer);
        }
    }

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureDiscoverAllCharacteristicDescriptorsOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the
*              gBleGattEvtClientProcedureDiscoverAllCharacteristicDescriptorsOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureDiscoverAllCharacteristicDescriptorsOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcDiscoverAllCharacteristicDescriptors_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Get characteristic parameter from buffer */
        fsciBleGattClientGetCharFromBuffer(pCharacteristics, &pBuffer);
    }

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureReadCharacteristicValueOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the
*              gBleGattEvtClientProcedureReadCharacteristicValueOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureReadCharacteristicValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcReadCharacteristicValue_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Get characteristic parameter from buffer */
        fsciBleGattClientGetCharFromBuffer(pCharacteristics, &pBuffer);
    }

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureReadUsingCharacteristicUuidOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the
*              gBleGattEvtClientProcedureReadUsingCharacteristicUuidOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureReadUsingCharacteristicUuidOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcReadUsingCharacteristicUuid_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Get value size parameter from buffer */
        fsciBleGetUint16ValueFromBuffer(*pArraySize, pBuffer);
        /* Get value parameter from buffer */
        fsciBleGetArrayFromBuffer(pValue, pBuffer, *pArraySize);
    }

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureReadMultipleCharacteristicValuesOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the
*              gBleGattEvtClientProcedureReadMultipleCharacteristicValuesOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureReadMultipleCharacteristicValuesOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcReadMultipleCharacteristicValues_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;
    uint8_t                     nbOfCharacteristics;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Get characteristics array size parameter from buffer */
        fsciBleGetUint8ValueFromBuffer(nbOfCharacteristics, pBuffer);
        /* Get characteristics array parameter from buffer */
        for(iCount = 0; iCount < nbOfCharacteristics; iCount++)
        {
            fsciBleGattClientGetCharFromBuffer(&pCharacteristics[iCount], &pBuffer);
        }
    }

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureWriteCharacteristicValueOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the
*              gBleGattEvtClientProcedureWriteCharacteristicValueOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureWriteCharacteristicValueOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcWriteCharacteristicValue_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;
    uint8_t                     nbOfCharacteristics;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureReadCharacteristicDescriptorOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the
*              gBleGattEvtClientProcedureReadCharacteristicDescriptorOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureReadCharacteristicDescriptorOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcReadCharacteristicDescriptor_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;
    uint8_t                     nbOfCharacteristics;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Get descriptor parameter from buffer */
        fsciBleGattClientGetAttributeFromBuffer(pDescriptors, &pBuffer);
    }

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientProcedureWriteCharacteristicDescriptorOpCode(
*                                               uint8_t *pBuffer,
*                                               uint32_t fsciInterfaceId)
*
*\brief        Handler for the
*              gBleGattEvtClientProcedureWriteCharacteristicDescriptorOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientProcedureWriteCharacteristicDescriptorOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattProcedureType_t         procedureType = gGattProcWriteCharacteristicDescriptor_c;
    gattProcedureResult_t       procedureResult = gGattProcSuccess_c;
    bleResult_t                 error = gBleSuccess_c;
    /* Procedure completed - get the kept information pointers */
    gattService_t*              pServices           = NULL;
    gattCharacteristic_t*       pCharacteristics    = NULL;
    gattAttribute_t*            pDescriptors        = NULL;
    uint8_t*                    pValue              = NULL;
    uint16_t*                   pArraySize          = NULL;
    uint32_t                    iCount              = 0U;
    deviceId_t                  deviceId            = gInvalidDeviceId_c;
    uint8_t                     nbOfCharacteristics;

    /* Get client procedure callback parameters (from buffer) */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetEnumValueFromBuffer(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetEnumValueFromBuffer(error, pBuffer, bleResult_t);

    pServices           = fsciBleGattClientGetServicesInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pCharacteristics    = fsciBleGattClientGetCharInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pDescriptors        = fsciBleGattClientGetDescriptorsInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pValue              = fsciBleGattClientGetValueInfo(deviceId, gUnenhancedBearerId_c, FALSE);
    pArraySize          = fsciBleGattClientGetArraySizeInfo(deviceId, gUnenhancedBearerId_c, FALSE);

    /* Call client procedure callback */
    fsciBleGattCallbacks.clientProcedureCallback(deviceId, procedureType, procedureResult, error);

    /* Erase the information kept (allocated or just saved) */
    fsciBleGattClientEraseInfo(FALSE, deviceId, gUnenhancedBearerId_c);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientNotificationOpCode(uint8_t *pBuffer,
*                                                         uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtClientNotificationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientNotificationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    characteristicValueHandle = gGattDbInvalidHandle_d;
    uint8_t*    pValue = NULL;
    uint16_t    valueLength = 0U;
    deviceId_t  deviceId = gInvalidDeviceId_c;

    /* Get event parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(characteristicValueHandle, pBuffer);
    fsciBleGetUint16ValueFromBuffer(valueLength, pBuffer);

    pValue = MEM_BufferAlloc(valueLength);

    if(NULL == pValue)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        fsciBleGetArrayFromBuffer(pValue, pBuffer, valueLength);

        /* Call client notification callback */
        fsciBleGattCallbacks.clientNotificationCallback(deviceId, characteristicValueHandle, pValue, valueLength);

        (void)MEM_BufferFree(pValue);

        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtClientIndicationOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtClientIndicationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtClientIndicationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    uint16_t    characteristicValueHandle = gGattDbInvalidHandle_d;
    uint8_t*    pValue = NULL;
    uint16_t    valueLength = 0U;
    deviceId_t  deviceId = gInvalidDeviceId_c;

    /* Get event parameters from buffer */
    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
    fsciBleGetUint16ValueFromBuffer(characteristicValueHandle, pBuffer);
    fsciBleGetUint16ValueFromBuffer(valueLength, pBuffer);

    pValue = MEM_BufferAlloc(valueLength);

    if(NULL == pValue)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        fsciBleGetArrayFromBuffer(pValue, pBuffer, valueLength);

        /* Call client indication callback */
        fsciBleGattCallbacks.clientIndicationCallback(deviceId, characteristicValueHandle, pValue, valueLength);

        (void)MEM_BufferFree(pValue);

        Ble_OutParamsReady();
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtServerMtuChangedOpCode(uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtServerMtuChangedOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtServerMtuChangedOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattServerEvent_t*      pServerEvent = NULL;
    gattServerEventType_t   eventType = gEvtMtuChanged_c;
    deviceId_t              deviceId = gInvalidDeviceId_c;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    pServerEvent = fsciBleGattServerAllocServerEventForBuffer(eventType, pBuffer);

    if(NULL == pServerEvent)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get event parameters from buffer */
        fsciBleGattServGetServEventFromBuffer(pServerEvent, &pBuffer);

        /* Call server callback */
        fsciBleGattCallbacks.serverCallback(deviceId, pServerEvent);

        /* Free allocated server event */
        (void)MEM_BufferFree(pServerEvent);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtServerHandleValueConfirmationOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtServerHandleValueConfirmationOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtServerHandleValueConfirmationOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattServerEvent_t*      pServerEvent = NULL;
    gattServerEventType_t   eventType = gEvtHandleValueConfirmation_c;
    deviceId_t              deviceId = gInvalidDeviceId_c;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    pServerEvent = fsciBleGattServerAllocServerEventForBuffer(eventType, pBuffer);

    if(NULL == pServerEvent)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get event parameters from buffer */
        fsciBleGattServGetServEventFromBuffer(pServerEvent, &pBuffer);

        /* Call server callback */
        fsciBleGattCallbacks.serverCallback(deviceId, pServerEvent);

        /* Free allocated server event */
        (void)MEM_BufferFree(pServerEvent);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtServerAttributeWrittenOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtServerAttributeWrittenOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtServerAttributeWrittenOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattServerEvent_t*      pServerEvent = NULL;
    gattServerEventType_t   eventType = gEvtAttributeWritten_c;
    deviceId_t              deviceId = gInvalidDeviceId_c;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    pServerEvent = fsciBleGattServerAllocServerEventForBuffer(eventType, pBuffer);

    if(NULL == pServerEvent)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get event parameters from buffer */
        fsciBleGattServGetServEventFromBuffer(pServerEvent, &pBuffer);

        /* Call server callback */
        fsciBleGattCallbacks.serverCallback(deviceId, pServerEvent);

        /* Free allocated server event */
        (void)MEM_BufferFree(pServerEvent);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtServerCharacteristicCccdWrittenOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtServerCharacteristicCccdWrittenOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtServerCharacteristicCccdWrittenOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattServerEvent_t*      pServerEvent = NULL;
    gattServerEventType_t   eventType = gEvtCharacteristicCccdWritten_c;
    deviceId_t              deviceId = gInvalidDeviceId_c;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    pServerEvent = fsciBleGattServerAllocServerEventForBuffer(eventType, pBuffer);

    if(NULL == pServerEvent)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get event parameters from buffer */
        fsciBleGattServGetServEventFromBuffer(pServerEvent, &pBuffer);

        /* Call server callback */
        fsciBleGattCallbacks.serverCallback(deviceId, pServerEvent);

        /* Free allocated server event */
        (void)MEM_BufferFree(pServerEvent);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtServerAttributeWrittenWithoutResponseOpCode(
*                                                       uint8_t *pBuffer,
*                                                       uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtServerAttributeWrittenWithoutResponseOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtServerAttributeWrittenWithoutResponseOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattServerEvent_t*      pServerEvent = NULL;
    gattServerEventType_t   eventType = gEvtAttributeWrittenWithoutResponse_c;
    deviceId_t              deviceId = gInvalidDeviceId_c;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    pServerEvent = fsciBleGattServerAllocServerEventForBuffer(eventType, pBuffer);

    if(NULL == pServerEvent)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get event parameters from buffer */
        fsciBleGattServGetServEventFromBuffer(pServerEvent, &pBuffer);

        /* Call server callback */
        fsciBleGattCallbacks.serverCallback(deviceId, pServerEvent);

        /* Free allocated server event */
        (void)MEM_BufferFree(pServerEvent);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtServerErrorOpCode(uint8_t *pBuffer,
*                                                  uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtServerErrorOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtServerErrorOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattServerEvent_t*      pServerEvent = NULL;
    gattServerEventType_t   eventType = gEvtError_c;
    deviceId_t              deviceId = gInvalidDeviceId_c;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    pServerEvent = fsciBleGattServerAllocServerEventForBuffer(eventType, pBuffer);

    if(NULL == pServerEvent)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get event parameters from buffer */
        fsciBleGattServGetServEventFromBuffer(pServerEvent, &pBuffer);

        /* Call server callback */
        fsciBleGattCallbacks.serverCallback(deviceId, pServerEvent);

        /* Free allocated server event */
        (void)MEM_BufferFree(pServerEvent);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtServerLongCharacteristicWrittenOpCode(
*                                                  uint8_t *pBuffer,
*                                                  uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtServerLongCharacteristicWrittenOpCode_c
*              opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtServerLongCharacteristicWrittenOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattServerEvent_t*      pServerEvent = NULL;
    gattServerEventType_t   eventType = gEvtLongCharacteristicWritten_c;
    deviceId_t              deviceId = gInvalidDeviceId_c;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    pServerEvent = fsciBleGattServerAllocServerEventForBuffer(eventType, pBuffer);

    if(NULL == pServerEvent)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get event parameters from buffer */
        fsciBleGattServGetServEventFromBuffer(pServerEvent, &pBuffer);

        /* Call server callback */
        fsciBleGattCallbacks.serverCallback(deviceId, pServerEvent);

        /* Free allocated server event */
        (void)MEM_BufferFree(pServerEvent);
    }
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattEvtServerAttributeReadOpCode(uint8_t *pBuffer,
*                                                          uint32_t fsciInterfaceId)
*
*\brief        Handler for the gBleGattEvtServerAttributeReadOpCode_c opCode.
*
*\param  [in]  pBuffer              Pointer to the command parameters.
*\param  [in]  fsciInterfaceId      FSCI interface identifier.
*
*\retval       void.
********************************************************************************** */
static void HandleGattEvtServerAttributeReadOpCode(uint8_t *pBuffer, uint32_t fsciInterfaceId)
{
    gattServerEvent_t*      pServerEvent = NULL;
    gattServerEventType_t   eventType = gEvtAttributeRead_c;
    deviceId_t              deviceId = gInvalidDeviceId_c;

    fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

    pServerEvent = fsciBleGattServerAllocServerEventForBuffer(eventType, pBuffer);

    if(NULL == pServerEvent)
    {
        /* No memory */
        fsciBleError(gFsciOutOfMessages_c, fsciInterfaceId);
    }
    else
    {
        /* Get event parameters from buffer */
        fsciBleGattServGetServEventFromBuffer(pServerEvent, &pBuffer);

        /* Call server callback */
        fsciBleGattCallbacks.serverCallback(deviceId, pServerEvent);

        /* Free allocated server event */
        (void)MEM_BufferFree(pServerEvent);
    }
}
#endif /* gFsciBleHost_d */

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcExchangeMtu(deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcExchangeMtu_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcExchangeMtu
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode          = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize        = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                  sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket   = NULL;
    uint8_t*                    pBuffer         = NULL;

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcDiscoverAllPrimaryServices(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcDiscoverAllPrimaryServices_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcDiscoverAllPrimaryServices
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode          = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize        = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                  sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket   = NULL;
    uint8_t*                    pBuffer         = NULL;
    uint16_t*                   pArraySize      = fsciBleGattClientGetArraySizeInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    gattService_t*              pServices       = fsciBleGattClientGetServicesInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureDiscoverAllPrimaryServicesOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedDiscoverAllPrimaryServicesOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }
    
    /* When the procedure successfully completes, the information kept when
    the procedure was started, must be also monitored */
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Consider also the size of the kept services */
        dataSize += sizeof(uint8_t);
        for(uint32_t iCount = 0; iCount < (uint8_t)*pArraySize; iCount++)
        {
            dataSize += fsciBleGattClientGetServiceBufferSize(&pServices[iCount]);
        }
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Set services array size parameter in the buffer */
        fsciBleGetBufferFromUint8Value((uint8_t)*pArraySize, pBuffer);
        /* Set services array parameter in the buffer */
        for(uint32_t iCount = 0; iCount < (uint8_t)*pArraySize; iCount++)
        {
            fsciBleGattClientGetBufferFromService(&pServices[iCount], &pBuffer);
        }
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcDiscoverPrimaryServicesByUuid(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcDiscoverPrimaryServicesByUuid_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcDiscoverPrimaryServicesByUuid
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode          = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize        = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                  sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket   = NULL;
    uint8_t*                    pBuffer         = NULL;
    uint16_t*                   pArraySize      = fsciBleGattClientGetArraySizeInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    gattService_t*              pServices       = fsciBleGattClientGetServicesInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureDiscoverPrimaryServicesByUuidOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedDiscoverPrimaryServicesByUuidOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }
    
    /* When the procedure successfully completes, the information kept when
    the procedure was started, must be also monitored */
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Consider also the size of the kept services */
        dataSize += sizeof(uint8_t);
        for(uint32_t iCount = 0; iCount < (uint8_t)*pArraySize; iCount++)
        {
            dataSize += fsciBleGattClientGetServiceBufferSize(&pServices[iCount]);
        }
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Set services array size parameter in the buffer */
        fsciBleGetBufferFromUint8Value((uint8_t)*pArraySize, pBuffer);
        /* Set services array parameter in the buffer */
        for(uint32_t iCount = 0; iCount < (uint8_t)*pArraySize; iCount++)
        {
            fsciBleGattClientGetBufferFromService(&pServices[iCount], &pBuffer);
        }
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcFindIncludedServices(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcFindIncludedServices_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcFindIncludedServices
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode          = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize        = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                  sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket   = NULL;
    uint8_t*                    pBuffer         = NULL;
    gattService_t*              pServices       = fsciBleGattClientGetServicesInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureFindIncludedServicesOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedFindIncludedServicesOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }
    
    /* When the procedure successfully completes, the information kept when
    the procedure was started, must be also monitored */
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Consider also the size of the kept service */
        dataSize += fsciBleGattClientGetServiceBufferSize(pServices);
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Set service parameter in the buffer */
        fsciBleGattClientGetBufferFromService(pServices, &pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcDiscoverAllCharacteristics(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcDiscoverAllCharacteristics_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcDiscoverAllCharacteristics
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode          = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize        = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                  sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket   = NULL;
    uint8_t*                    pBuffer         = NULL;
    gattService_t*              pServices       = fsciBleGattClientGetServicesInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureDiscoverAllCharacteristicsOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedDiscoverAllCharacteristicsOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }
    
    /* When the procedure successfully completes, the information kept when
    the procedure was started, must be also monitored */
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Consider also the size of the kept service */
        dataSize += fsciBleGattClientGetServiceBufferSize(pServices);
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Set service parameter in the buffer */
        fsciBleGattClientGetBufferFromService(pServices, &pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcDiscoverCharacteristicByUuid(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcDiscoverCharacteristicByUuid_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcDiscoverCharacteristicByUuid
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode                  = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize                = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                          sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket           = NULL;
    uint8_t*                    pBuffer                 = NULL;
    uint16_t*                   pArraySize              = fsciBleGattClientGetArraySizeInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    gattCharacteristic_t*       pCharacteristics        = fsciBleGattClientGetCharInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureDiscoverCharacteristicByUuidOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedDiscoverCharacteristicByUuidOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }
    
    /* When the procedure successfully completes, the information kept when
    the procedure was started, must be also monitored */
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Consider also the size of the kept characteristics */
        dataSize += sizeof(uint8_t);
        for(uint32_t iCount = 0; iCount < (uint8_t)*pArraySize; iCount++)
        {
            dataSize += fsciBleGattClientGetCharacteristicBufferSize(&pCharacteristics[iCount]);
        }
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Set characteristics array size parameter in the buffer */
        fsciBleGetBufferFromUint8Value((uint8_t)*pArraySize, pBuffer);
        /* Set characteristics array parameter in the buffer */
        for(uint32_t iCount = 0; iCount < (uint8_t)*pArraySize; iCount++)
        {
            fsciBleGattClientGetBufferFromCharacteristic(&pCharacteristics[iCount], &pBuffer);
        }
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcDiscoverAllCharacteristicDescriptors(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcDiscoverAllCharacteristicDescriptors_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcDiscoverAllCharacteristicDescriptors
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode                  = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize                = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                          sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket           = NULL;
    uint8_t*                    pBuffer                 = NULL;
    gattCharacteristic_t*       pCharacteristics        = fsciBleGattClientGetCharInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureDiscoverAllCharacteristicDescriptorsOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedDiscoverAllCharacteristicDescriptorsOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }
    
    /* When the procedure successfully completes, the information kept when
    the procedure was started, must be also monitored */
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Consider also the size of the kept characteristic */
        dataSize += fsciBleGattClientGetCharacteristicBufferSize(pCharacteristics);
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Set characteristic parameter in the buffer */
        fsciBleGattClientGetBufferFromCharacteristic(pCharacteristics, &pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcReadCharacteristicValue(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcReadCharacteristicValue_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcReadCharacteristicValue
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode                  = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize                = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                          sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket           = NULL;
    uint8_t*                    pBuffer                 = NULL;
    gattCharacteristic_t*       pCharacteristics        = fsciBleGattClientGetCharInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureReadCharacteristicValueOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedReadCharacteristicValueOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }
    
    /* When the procedure successfully completes, the information kept when
    the procedure was started, must be also monitored */
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Consider also the size of the kept characteristic */
        dataSize += fsciBleGattClientGetCharacteristicBufferSize(pCharacteristics);
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Set characteristic parameter in the buffer */
        fsciBleGattClientGetBufferFromCharacteristic(pCharacteristics, &pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcReadUsingCharacteristicUuid(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcReadUsingCharacteristicUuid_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcReadUsingCharacteristicUuid
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode                  = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize                = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                          sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket           = NULL;
    uint8_t*                    pBuffer                 = NULL;
    uint16_t*                   pArraySize              = fsciBleGattClientGetArraySizeInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    uint8_t*                    pValue                  = fsciBleGattClientGetValueInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureReadUsingCharacteristicUuidOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedReadUsingCharacteristicUuidOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }
    
    /* When the procedure successfully completes, the information kept when
    the procedure was started, must be also monitored */
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Consider also the size of the kept value */
        dataSize += sizeof(uint16_t) + *pArraySize;
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Set value size parameter in the buffer */
        fsciBleGetBufferFromUint16Value(*pArraySize, pBuffer);
        /* Set value parameter in the buffer */
        fsciBleGetBufferFromArray(pValue, pBuffer, *pArraySize);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcReadMultipleCharacteristicValues(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcReadMultipleCharacteristicValues_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcReadMultipleCharacteristicValues
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode                  = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize                = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                          sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket           = NULL;
    uint8_t*                    pBuffer                 = NULL;
    gattCharacteristic_t*       pCharacteristics        = fsciBleGattClientGetCharInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureReadMultipleCharacteristicValuesOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedReadMultipleCharacteristicValuesOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }
    
    /* When the procedure successfully completes, the information kept when
    the procedure was started, must be also monitored */
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Consider also the size of the kept characteristics */
        dataSize += sizeof(uint8_t);
        for(uint32_t iCount = 0; iCount < fsciBleGattClientNbOfCharacteristics; iCount++)
        {
            dataSize += fsciBleGattClientGetCharacteristicBufferSize(&pCharacteristics[iCount]);
        }
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Set characteristics array size parameter in the buffer */
        fsciBleGetBufferFromUint8Value(fsciBleGattClientNbOfCharacteristics, pBuffer);
        /* Set characteristics array parameter in the buffer */
        for(uint32_t iCount = 0; iCount < fsciBleGattClientNbOfCharacteristics; iCount++)
        {
            fsciBleGattClientGetBufferFromCharacteristic(&pCharacteristics[iCount], &pBuffer);
        }
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcWriteCharacteristicValue(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcWriteCharacteristicValue_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcWriteCharacteristicValue
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode                  = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize                = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                          sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket           = NULL;
    uint8_t*                    pBuffer                 = NULL;
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureWriteCharacteristicValueOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedWriteCharacteristicValueOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcReadCharacteristicDescriptor(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcReadCharacteristicDescriptor_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcReadCharacteristicDescriptor
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode                  = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize                = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                          sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket           = NULL;
    uint8_t*                    pBuffer                 = NULL;
    gattAttribute_t*            pDescriptors            = fsciBleGattClientGetDescriptorsInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureReadCharacteristicDescriptorOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedReadCharacteristicDescriptorOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }
    
    /* When the procedure successfully completes, the information kept when
    the procedure was started, must be also monitored */
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Consider also the size of the kept descriptor */
        dataSize += fsciBleGattClientGetAttributeBufferSize(pDescriptors);
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Set descriptor parameter in the buffer */
        fsciBleGattClientGetBufferFromAttribute(pDescriptors, &pBuffer);
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

/*! *********************************************************************************
*\private
*\fn           void HandleGattProcWriteCharacteristicDescriptor(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcWriteCharacteristicDescriptor_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcWriteCharacteristicDescriptor
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode                  = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize                = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                          sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket           = NULL;
    uint8_t*                    pBuffer                 = NULL;
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureWriteCharacteristicDescriptorOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedWriteCharacteristicDescriptorOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

#if defined(gBLE52_d) && (gBLE52_d == TRUE)
/*! *********************************************************************************
*\private
*\fn           void HandleGattProcReadMultipleVarLengthCharValues(
*                                             deviceId_t            deviceId,
*                                             bearerId_t            bearerId,
*                                             gattProcedureResult_t procedureResult,
*                                             bleResult_t           error
*
*\brief        Handler for the gGattProcReadMultipleVarLengthCharValues_c opCode.
*
*\param  [in]  deviceId              Peer identifier.
*\param  [in]  bearerId              Identifier of the bearer.
*\param  [in]  procedureResult       Result of the procedure.
*\param  [in]  error                 If procedureResult is not gGattProcSuccess_c,
*                                    this contains the error that terminated the
*                                    procedure.
*
*\retval       void.
********************************************************************************** */
static void HandleGattProcReadMultipleVarLengthCharValues
(
    deviceId_t                  deviceId,
    bearerId_t                  bearerId,
    gattProcedureResult_t       procedureResult,
    bleResult_t                 error
)
{
    fsciBleGattOpCode_t         opCode                  = gBleGattEvtClientProcedureExchangeMtuOpCode_c;
    uint32_t                    dataSize                = fsciBleGetDeviceIdBufferSize(&deviceId) + sizeof(gattProcedureResult_t) +
                                                          sizeof(bleResult_t);
    clientPacketStructured_t*   pClientPacket           = NULL;
    uint8_t*                    pBuffer                 = NULL;
    gattCharacteristic_t*       pCharacteristics        = fsciBleGattClientGetCharInfo(deviceId, bearerId, bFsciBleGattClientAllocatedInfo);
    
    if (bearerId == gUnenhancedBearerId_c)
    {
        opCode = gBleGattEvtClientProcedureReadMultipleVariableLenCharValuesOpCode_c;
    }
    else
    {
        opCode = gBleGattEvtClientProcedureEnhancedReadMultipleVariableLenCharValuesOpCode_c;
    }
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        dataSize += sizeof(&bearerId);
    }
    
    /* When the procedure successfully completes, the information kept when
    the procedure was started, must be also monitored */
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Consider also the size of the kept characteristics */
        dataSize += sizeof(uint8_t);
        for(uint32_t iCount = 0; iCount < fsciBleGattClientNbOfCharacteristics; iCount++)
        {
            dataSize += fsciBleGattClientGetCharacteristicBufferSize(&pCharacteristics[iCount]);
        }
    }

    /* Allocate the packet to be sent over UART */
    pClientPacket = fsciBleGattAllocFsciPacket((uint8_t)opCode, dataSize);

    if(NULL == pClientPacket)
    {
        return;
    }

    pBuffer = &pClientPacket->payload[0];

    /* Set event parameters in the buffer */
    fsciBleGetBufferFromDeviceId(&deviceId, &pBuffer);
    
    if (bearerId != gUnenhancedBearerId_c)
    {
        fsciBleGetBufferFromUint8Value(bearerId, pBuffer);
    }

    fsciBleGetBufferFromEnumValue(procedureResult, pBuffer, gattProcedureResult_t);
    fsciBleGetBufferFromEnumValue(error, pBuffer, bleResult_t);
    
    if(gGattProcSuccess_c == procedureResult)
    {
        /* Set characteristics array size parameter in the buffer */
        fsciBleGetBufferFromUint8Value(fsciBleGattClientNbOfCharacteristics, pBuffer);
        /* Set characteristics array parameter in the buffer */
        for(uint32_t iCount = 0; iCount < fsciBleGattClientNbOfCharacteristics; iCount++)
        {
            fsciBleGattClientGetBufferFromCharacteristic(&pCharacteristics[iCount], &pBuffer);
        }
    }

    /* Transmit the packet over UART */
    fsciBleTransmitFormatedPacket(pClientPacket, fsciBleInterfaceId);
}

#endif /* gBLE52_d*/

#endif /* gFsciIncluded_c && gFsciBleGattLayerEnabled_d */

/*! *********************************************************************************
* @}
********************************************************************************** */
