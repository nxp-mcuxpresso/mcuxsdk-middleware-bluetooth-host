/*! *********************************************************************************
* Copyright 2022-2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "fsl_component_mem_manager.h"
#include "fsl_component_timer_manager.h"
#include "fwk_platform.h"

#include "host_hsdk_interface.h"
#include "host_cmd_ble.h"

/* BLE Host Stack */
#include "gap_types.h"
#include "gatt_interface.h"
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gatt_database.h"
#include "gap_interface.h"
#include "gatt_db_app_interface.h"

#include "host_ble_conn_manager.h"
#include "host_ble_service_discovery.h"
#include "dynamic_database.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

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
static uint8_t mOwnDeviceAddress[gcBleDeviceAddressSize_c];

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
static uint8_t mIdentityInfoCount = 0U;
static uint8_t mFilterAcceptListCount = 0U;
static gapIdentityInformation_t *pOutIdentityAddresses = NULL;
#if (defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 1U))
static bool_t mLastCheckNewBondValue = FALSE;
static bool_t mbPrivacyEnabled = FALSE;
static uint8_t mcDevicesInResolvingList = 0;
static bool_t  mSettingRandomAddressFromApplication = FALSE;
STATIC bool_t  mHaveRandomAddress = FALSE;
#endif /* gAppUsePrivacy_d */
#endif /* gAppUseBonding_d */
#endif /* gAppUsePairing_d */

extern servDiscInfo_t maServDiscInfo[gAppMaxConnections_c];

pfAppCallback_t mpfAppCallback = NULL;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static bool_t App_HandleHSDKMessageInputGAP(bleEvtContainer_t* pMsg);
static bool_t App_HandleHSDKMessageInputGATT(bleEvtContainer_t* pMsg);
static void gapSmpHandler(bleEvtContainer_t* pMsg);

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
static void hsdkObserverGAPSetLocalPasskeyCallback(bleEvtContainer_t *container);
static void hsdkObserverGAPRegisterDeviceSecurityRequirements(bleEvtContainer_t *container);
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
static void hsdkObserverGetBondedDevicesIdentityInformation(bleEvtContainer_t *container);
static void hsdkObserverGAPAddDeviceToFilterAcceptList(bleEvtContainer_t *container);
#endif
#endif

#if (defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 1U)) && \
    (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
static void App_ManagePrivacyInternal(bleEvtContainer_t* pMsg);
static void App_DisablePrivacy(void);
#endif /* gAppUsePrivacy_d */

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
static void BleConnManager_MCUInfoToSmpKeys(void);
#endif

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*!*************************************************************************************************
*\fn    void App_RegisterEventCallback(pfAppCallback_t callback)
*
*\brief Registers a callback to send events to the application for handling.
*
* \param[in]    callback    Pointer to callback function.
*
*\return       None
***************************************************************************************************/
void App_RegisterEventCallback(pfAppCallback_t callback)
{
    mpfAppCallback = callback;
}

/*!*************************************************************************************************
*\fn    void App_HandleHSDKMessageInput(bleEvtContainer_t* pMsg);
*
*\brief Handles all messages received from the HSDK.
*
* \param[in]    pMsg    Pointer to bleEvtContainer_t.
*
*\return       None
***************************************************************************************************/
void App_HandleHSDKMessageInput(bleEvtContainer_t* pMsg)
{
    bool_t matchFound = FALSE;

    matchFound = App_HandleHSDKMessageInputGAP(pMsg);

    if (matchFound == FALSE)
    {
       matchFound = App_HandleHSDKMessageInputGATTDb(pMsg);
    }

    if (matchFound == FALSE)
    {
       matchFound = App_HandleHSDKMessageInputGATT(pMsg);
    }

    (void)matchFound;
}


#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
/*!*************************************************************************************************
*\fn    static void hsdkObserverGAPSetLocalPasskeyCallback(bleEvtContainer_t *container)
*
*\brief Handles the Set Local Passkey response.
*       Sends a Register Device Security Requirements request.
*
* \param[in]   container    Pointer to event container
*
*\return       None
***************************************************************************************************/
static void hsdkObserverGAPSetLocalPasskeyCallback(bleEvtContainer_t *container)
{
    GAPRegisterDeviceSecurityRequirementsRequest_t req;

    RegisterRemovableObserver(GAPConfirm_FSCI_ID,
                              hsdkObserverGAPRegisterDeviceSecurityRequirements);
    req.SecurityRequirementsIncluded = TRUE;
    req.SecurityRequirements.NbOfServices = deviceSecurityRequirements.cNumServices;
    req.SecurityRequirements.GapServiceSecurityRequirements = NULL;
    req.SecurityRequirements.SecurityRequirements.SecurityModeLevel =
        GAPRegisterDeviceSecurityRequirementsRequest_SecurityRequirements_SecurityRequirements_SecurityModeLevel_gMode1Level3_c;
    req.SecurityRequirements.SecurityRequirements.Authorization = deviceSecurityRequirements.pSecurityRequirements->authorization;
    req.SecurityRequirements.SecurityRequirements.MinimumEncryptionKeySize  = deviceSecurityRequirements.pSecurityRequirements->minimumEncryptionKeySize;
    GAPRegisterDeviceSecurityRequirementsRequest(&req, gFsciInterface_c);
}

/*!*************************************************************************************************
*\fn    static void hsdkObserverGAPRegisterDeviceSecurityRequirements(bleEvtContainer_t *container)
*
*\brief Handles the Register Device Security Requirements response.
*       Sends a Get Bonded Devices Identity Information request.
*
* \param[in]   container    Pointer to event container
*
*\return       None
***************************************************************************************************/
static void hsdkObserverGAPRegisterDeviceSecurityRequirements(bleEvtContainer_t *container)
{
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
    RegisterRemovableObserver(GAPConfirm_FSCI_ID,
                              hsdkObserverGetBondedDevicesIdentityInformation);
    GAPGetBondedDevicesIdentityInformationRequest_t req;
    req.maxDevices = gMaxResolvingListSize_c;
    GAPGetBondedDevicesIdentityInformationRequest(&req, gFsciInterface_c);
#endif
}
#endif

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
/*!*************************************************************************************************
*\fn    static void hsdkObserverGetBondedDevicesIdentityInformation(bleEvtContainer_t *container)
*
*\brief Handles the Get Bonded Devices Identity Information response.
*
* \param[in]   container    Pointer to event container
*
*\return       None
***************************************************************************************************/
static void hsdkObserverGetBondedDevicesIdentityInformation(bleEvtContainer_t *container)
{

}

/*!*************************************************************************************************
*\fn    static void hsdkObserverGAPAddDeviceToFilterAcceptList(bleEvtContainer_t *container)
*
*\brief Handles the Add Device To Filter Accept List response.
*
* \param[in]   container    Pointer to event container
*
*\return       None
***************************************************************************************************/
static void hsdkObserverGAPAddDeviceToFilterAcceptList(bleEvtContainer_t *container)
{
    if (mFilterAcceptListCount < mIdentityInfoCount)
    {
        GAPAddDeviceToFilterAcceptListRequest_t req;
        RegisterRemovableObserver(GAPConfirm_FSCI_ID,
                              hsdkObserverGAPAddDeviceToFilterAcceptList);

        req.AddressType =
          (GAPAddDeviceToFilterAcceptListRequest_AddressType_t)pOutIdentityAddresses[mFilterAcceptListCount].identityAddress.idAddressType;
        FLib_MemCpy(req.Address,
                    pOutIdentityAddresses[mFilterAcceptListCount].identityAddress.idAddress,
                    gcBleDeviceAddressSize_c);
        GAPAddDeviceToFilterAcceptListRequest(&req, gFsciInterface_c);

        mFilterAcceptListCount++;
        GAPAddDeviceToFilterAcceptListRequest(&req, gFsciInterface_c);
    }
}
#endif
#endif

/*!*************************************************************************************************
*\fn    static void App_HandleHSDKMessageInputGAP(bleEvtContainer_t *pMsg)
*
*\brief Handles GAP specific events
*
* \param[in]   container    Pointer to event container
*
*\return       None
***************************************************************************************************/
static bool_t App_HandleHSDKMessageInputGAP(bleEvtContainer_t* pMsg)
{
    bool_t matchFound = TRUE;

    if (mpfAppCallback != NULL)
    {
        mpfAppCallback(pMsg);
    }

    switch (pMsg->id)
    {
        case GAPGenericEventInitializationCompleteIndication_FSCI_ID:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
            BleConnManager_MCUInfoToSmpKeys();
#endif
            GAPReadPublicDeviceAddressRequest(gFsciInterface_c);
        }
        break;

        case GAPGenericEventPublicAddressReadIndication_FSCI_ID:
        {
            /* Save own address */
            FLib_MemCpy(mOwnDeviceAddress,
                        pMsg->Data.GAPGenericEventPublicAddressReadIndication.Address,
                        gcBleDeviceAddressSize_c);

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
#if defined(gPasskeyValue_c)
            GAPSetLocalPasskeyRequest_t req;
            req.Passkey = gPasskeyValue_c;
            RegisterRemovableObserver(GAPConfirm_FSCI_ID,
                              hsdkObserverGAPSetLocalPasskeyCallback);
            GAPSetLocalPasskeyRequest(&req, gFsciInterface_c);
#else
            RegisterRemovableObserver(GAPConfirm_FSCI_ID,
                              hsdkObserverGAPRegisterDeviceSecurityRequirements);
            GAPRegisterDeviceSecurityRequirementsRequest_t req;
            req.SecurityRequirementsIncluded = TRUE;
            req.SecurityRequirements.NbOfServices = deviceSecurityRequirements.cNumService;
            req.SecurityRequirements.GapServiceSecurityRequirements = deviceSecurityRequirements.aServiceSecurityRequirements;
            req.SecurityRequirements.SecurityRequirements.SecurityModeLevel =
                GAPRegisterDeviceSecurityRequirementsRequest_SecurityRequirements_SecurityRequirements_SecurityModeLevel_gMode1Level3_c;
            req.SecurityRequirements.SecurityRequirements.Authorization = deviceSecurityRequirements.pSecurityRequirements.authorization;
            req.SecurityRequirements.SecurityRequirements.MinimumEncryptionKeySize  = deviceSecurityRequirements.pSecurityRequirements.minimumEncryptionKeySize;
            GAPRegisterDeviceSecurityRequirementsRequest(&req, gFsciInterface_c);
#endif /* gPasskeyValue_c */
#endif /* gAppUsePairing_d */
        }
        break;

        case (uint16_t)GAPGenericEventAdvertisingParametersSetupCompleteIndication_FSCI_ID:
        {
            /* Advertising parameters set successfully - set advertising data */
            GAPSetAdvertisingDataRequest_t req = {0U};
            req.AdvertisingDataIncluded = TRUE;
            req.ScanResponseDataIncluded = FALSE;
            req.AdvertisingData.AdStructures = NULL;
            req.ScanResponseData.NbOfAdStructures = 0U;
            req.ScanResponseData.AdStructures = NULL;
            req.AdvertisingData.NbOfAdStructures = gAppAdvertisingData.cNumAdStructures;

            if (gAppAdvertisingData.cNumAdStructures > 0U)
            {
                req.AdvertisingData.AdStructures = MEM_BufferAlloc(gAppAdvertisingData.cNumAdStructures * sizeof(*req.AdvertisingData.AdStructures));

                if (NULL != req.AdvertisingData.AdStructures)
                {
                    for(uint32_t i = 0; i < gAppAdvertisingData.cNumAdStructures; i++)
                    {
                        req.AdvertisingData.AdStructures[i].Length = gAppAdvertisingData.aAdStructures[i].length - 0x01U;
                        req.AdvertisingData.AdStructures[i].Type = (GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_t)gAppAdvertisingData.aAdStructures[i].adType;
                        req.AdvertisingData.AdStructures[i].Data = MEM_BufferAlloc((uint32_t)gAppAdvertisingData.aAdStructures[i].length - 0x01U);

                        if (NULL != req.AdvertisingData.AdStructures[i].Data)
                        {
                            FLib_MemCpy(req.AdvertisingData.AdStructures[i].Data,
                                        gAppAdvertisingData.aAdStructures[i].aData,
                                        (uint32_t)gAppAdvertisingData.aAdStructures[i].length - 0x01U);
                        }
                    }
                }
            }

            (void)GAPSetAdvertisingDataRequest(&req, gFsciInterface_c);

            if (gAppAdvertisingData.cNumAdStructures > 0U)
            {
                for(uint32_t i = 0; i < gAppAdvertisingData.cNumAdStructures; i++)
                {
                    (void)MEM_BufferFree(req.AdvertisingData.AdStructures[i].Data);
                    req.AdvertisingData.AdStructures[i].Data = NULL;
                }
                (void)MEM_BufferFree(req.AdvertisingData.AdStructures);
                req.AdvertisingData.AdStructures = NULL;
            }
        }
        break;

        case GAPGenericEventAdvertisingDataSetupCompleteIndication_FSCI_ID:
        {
            /* Advertising data set successfully - start advertising */
            (void)GAPStartAdvertisingRequest(gFsciInterface_c);
        }
        break;

        case GAPCheckIfBondedIndication_FSCI_ID:
        case GAPLoadCustomPeerInformationIndication_FSCI_ID:
        case GAPConnectionEventEncryptionChangedIndication_FSCI_ID:
        case GAPConnectionEventPeripheralSecurityRequestIndication_FSCI_ID:
        case GAPConnectionEventPasskeyRequestIndication_FSCI_ID:
        case GAPConnectionEventKeyExchangeRequestIndication_FSCI_ID:
        case GAPLeScValidateNumericValueRequest_FSCI_ID:
        case GAPConnectionEventPairingCompleteIndication_FSCI_ID:
        case GAPConnectionEventPairingRequestIndication_FSCI_ID:
        case GAPConnectionEventLeScDisplayNumericValueIndication_FSCI_ID:
        case GAPConnectionEventLongTermKeyRequestIndication_FSCI_ID:
        case GAPPairingEventNoLTKIndication_FSCI_ID:
        case GAPConnectionEventAuthenticationRejectedIndication_FSCI_ID:
        {
            gapSmpHandler(pMsg);
        }
        break;

#if ((gAppUsePrivacy_d == 1U) && (gAppUseBonding_d == 1U))
        case GAPGenericEventRandomAddressReadyIndication_FSCI_ID:
        case GAPGenericEventControllerPrivacyStateChangedIndication_FSCI_ID:
        case GAPGenericEventHostPrivacyStateChangedIndication_FSCI_ID:
        case GAPGetBondedDevicesIdentityInformationIndication_FSCI_ID:
        {
            App_ManagePrivacyInternal(pMsg);
        }
        break;

        case GAPGenericEventDeviceAddedToFilterAcceptListIndication_FSCI_ID:
        {
            App_ManagePrivacyInternal(pMsg);
        }
        break;

        case GAPGenericEventRandomAddressSetIndication_FSCI_ID:
        {
            mHaveRandomAddress = TRUE;
            App_ManagePrivacyInternal(pMsg);
        }
        break;
#endif

        case GAPConfirm_FSCI_ID:
        case GAPConnectionEventChannelSelectionAlgorithm2Indication_FSCI_ID:
        case GAPAdvertisingEventAdvertisingSetTerminatedIndication_FSCI_ID:
        case GAPGenericEventChannelMapSetIndication_FSCI_ID:
        case GAPConnectionEventChannelMapReadIndication_FSCI_ID:
        {
            ; /* Untreated GAP events */
        }
        break;

        default:
        {
            matchFound = FALSE;
        }
        break;
    }

    return matchFound;
}

/*!*************************************************************************************************
*\fn    static void gapSmpHandler(bleEvtContainer_t *pMsg)
*
*\brief Handles Security Manager specific events
*
* \param[in]   pMsg    Pointer to event container
*
*\return       None
***************************************************************************************************/
static void gapSmpHandler(bleEvtContainer_t* pMsg)
{
    switch (pMsg->id)
    {
        case GAPConnectionEventPairingCompleteIndication_FSCI_ID:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
            if (pMsg->Data.GAPConnectionEventPairingCompleteIndication.PairingStatus
                == GAPConnectionEventPairingCompleteIndication_PairingStatus_PairingSuccessful)
            {
                if (pMsg->Data.GAPConnectionEventPairingCompleteIndication.PairingData.PairingSuccessful_WithBonding)
                {
#if (defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 1U))
                    mLastCheckNewBondValue = TRUE;
#endif /* (defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 1U)) */
                }
            }
#endif /* (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U)) */
        }
        break;

        case GAPConnectionEventPeripheralSecurityRequestIndication_FSCI_ID:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
            /* Initiate pairing procedure */
            GAPPairRequest_t req;
            req.DeviceId = pMsg->Data.GAPConnectionEventPeripheralSecurityRequestIndication.DeviceId;
            req.PairingParameters.WithBonding = gPairingParameters.withBonding;
            req.PairingParameters.SecurityModeAndLevel = GAPPairRequest_PairingParameters_SecurityModeAndLevel_gMode1Level3_c;
            req.PairingParameters.MaxEncryptionKeySize = mcEncryptionKeySize_c;
            req.PairingParameters.LocalIoCapabilities = GAPPairRequest_PairingParameters_LocalIoCapabilities_gIoKeyboardDisplay_c;
            req.PairingParameters.OobAvailable = gPairingParameters.oobAvailable;
            req.PairingParameters.CentralKeys = gPairingParameters.centralKeys;
            req.PairingParameters.PeripheralKeys = gPairingParameters.peripheralKeys;
            req.PairingParameters.LeSecureConnectionSupported = gPairingParameters.leSecureConnectionSupported;
            req.PairingParameters.UseKeypressNotifications = gPairingParameters.useKeypressNotifications;

            (void)GAPPairRequest(&req, gFsciInterface_c);
#else
            /* Pairing not supported */
            GAPRejectPairingRequest_t req;
            req.DeviceId = pMsg->Data.GAPConnectionEventPeripheralSecurityRequestIndication.DeviceId;
            req.Reason = GAPRejectPairingRequest_Reason_gPairingNotSupported_c;

            (void)GAPRejectPairingRequest(&req, gFsciInterface_c);
#endif
        }
        break;

        case GAPConnectionEventPasskeyRequestIndication_FSCI_ID:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
#if defined(gPasskeyValue_c)
            GAPEnterPasskeyRequest_t req;
            req.DeviceId = pMsg->Data.GAPConnectionEventPasskeyRequestIndication.DeviceId;
            req.Passkey = gPasskeyValue_c;

            GAPEnterPasskeyRequest(&req, gFsciInterface_c);
#endif
#endif
        }
        break;

        case GAPConnectionEventLeScDisplayNumericValueIndication_FSCI_ID:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
            GAPLeScValidateNumericValueRequest_t req;
            req.DeviceId = pMsg->Data.GAPConnectionEventLeScDisplayNumericValueIndication.DeviceId;
            req.Valid = TRUE;
            GAPLeScValidateNumericValueRequest(&req, gFsciInterface_c);
#endif
        }
        break;

        case GAPConnectionEventKeyExchangeRequestIndication_FSCI_ID:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
            GAPSendSmpKeysRequest_t req;
            req.DeviceId = pMsg->Data.GAPConnectionEventKeyExchangeRequestIndication.DeviceId;

            if ((pMsg->Data.GAPConnectionEventKeyExchangeRequestIndication.RequestedKeys &
                (uint8_t)gLtk_c) == 0U)
            {
                req.Keys.LtkIncluded = FALSE;
                /*
                 * When the LTK is NULL EDIV and Rand are not sent and will be
                 * ignored.
                 */
            }
            else
            {
                req.Keys.LtkIncluded = TRUE;
                req.Keys.LtkInfo.LtkSize = pMsg->Data.GAPConnectionEventKeyExchangeRequestIndication.RequestedLtkSize;
                req.Keys.LtkInfo.Ltk = gSmpKeys.aLtk;
            }

            if ((pMsg->Data.GAPConnectionEventKeyExchangeRequestIndication.RequestedKeys &
                (uint8_t)gIrk_c) == 0U)
            {
                req.Keys.IrkIncluded = FALSE;
                /*
                 * When the IRK is NULL the Address and Address Type are not sent
                 * and will be ignored.
                 */
            }
            else
            {
                req.Keys.IrkIncluded = TRUE;
                FLib_MemCpy(req.Keys.Irk, gSmpKeys.aIrk, gcSmpIrkSize_c);
            }

            if ((pMsg->Data.GAPConnectionEventKeyExchangeRequestIndication.RequestedKeys &
                (uint8_t)gCsrk_c) == 0U)
            {
                req.Keys.CsrkIncluded = FALSE;
            }
            else
            {
                req.Keys.CsrkIncluded = TRUE;
                FLib_MemCpy(req.Keys.Csrk, gSmpKeys.aCsrk, gcSmpCsrkSize_c);
            }

            req.Keys.RandEdivInfo.RandSize = gSmpKeys.cRandSize;
            req.Keys.RandEdivInfo.Rand = gSmpKeys.aRand;
            req.Keys.RandEdivInfo.Ediv = gSmpKeys.ediv;
            req.Keys.AddressIncluded = TRUE;
            req.Keys.AddressInfo.DeviceAddressType = GAPSendSmpKeysRequest_Keys_AddressInfo_DeviceAddressType_gPublic_c;
            FLib_MemCpy(req.Keys.AddressInfo.DeviceAddress,
                        mOwnDeviceAddress,
                        gcBleDeviceAddressSize_c);
            GAPSendSmpKeysRequest(&req, gFsciInterface_c);
#endif
        }
        break;

        case GAPConnectionEventAuthenticationRejectedIndication_FSCI_ID:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
             /* Peer lost the bond - Initiate pairing procedure */
            GAPPairRequest_t req;
            req.DeviceId = pMsg->Data.GAPConnectionEventAuthenticationRejectedIndication.DeviceId;
            req.PairingParameters.WithBonding = gPairingParameters.withBonding;
            req.PairingParameters.SecurityModeAndLevel = GAPPairRequest_PairingParameters_SecurityModeAndLevel_gMode1Level3_c;
            req.PairingParameters.MaxEncryptionKeySize = mcEncryptionKeySize_c;
            req.PairingParameters.LocalIoCapabilities = GAPPairRequest_PairingParameters_LocalIoCapabilities_gIoKeyboardDisplay_c;
            req.PairingParameters.OobAvailable = gPairingParameters.oobAvailable;
            req.PairingParameters.CentralKeys = gPairingParameters.centralKeys;
            req.PairingParameters.PeripheralKeys = gPairingParameters.peripheralKeys;
            req.PairingParameters.LeSecureConnectionSupported = gPairingParameters.leSecureConnectionSupported;
            req.PairingParameters.UseKeypressNotifications = gPairingParameters.useKeypressNotifications;

            (void)GAPPairRequest(&req, gFsciInterface_c);
#endif
        }
        break;

        case GAPConnectionEventPairingRequestIndication_FSCI_ID:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
            GAPAcceptPairingRequestRequest_t req;
            req.DeviceId = pMsg->Data.GAPConnectionEventPairingRequestIndication.DeviceId;
            req.PairingParameters.WithBonding = gPairingParameters.withBonding;
            req.PairingParameters.SecurityModeAndLevel = GAPAcceptPairingRequestRequest_PairingParameters_SecurityModeAndLevel_gMode1Level3_c;
            req.PairingParameters.MaxEncryptionKeySize = gPairingParameters.maxEncryptionKeySize;
            req.PairingParameters.LocalIoCapabilities =
              GAPAcceptPairingRequestRequest_PairingParameters_LocalIoCapabilities_gIoDisplayOnly_c;
            req.PairingParameters.OobAvailable = gPairingParameters.oobAvailable;
            req.PairingParameters.CentralKeys = gPairingParameters.centralKeys;
            req.PairingParameters.PeripheralKeys = gPairingParameters.peripheralKeys;
            req.PairingParameters.LeSecureConnectionSupported = FALSE;
            req.PairingParameters.UseKeypressNotifications = FALSE;
            GAPAcceptPairingRequestRequest(&req, gFsciInterface_c);
#else
            GAPRejectPairingRequest_t req;
            req.DeviceId = pMsg->Data.GAPConnectionEventPairingRequestIndication.DeviceId;
            req.Reason = GAPRejectPairingRequest_Reason_gPairingNotSupported_c;
            GAPRejectPairingRequest(&req, gFsciInterface_c);
#endif
        }
        break;

        case GAPConnectionEventLongTermKeyRequestIndication_FSCI_ID:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
            if ((pMsg->Data.GAPConnectionEventLongTermKeyRequestIndication.Ediv == gSmpKeys.ediv) &&
                (pMsg->Data.GAPConnectionEventLongTermKeyRequestIndication.RandSize == gSmpKeys.cRandSize) &&
                (TRUE == FLib_MemCmp(pMsg->Data.GAPConnectionEventLongTermKeyRequestIndication.Rand,
                                     gSmpKeys.aRand,
                                     gSmpKeys.cRandSize)))
            {
                /* EDIV and RAND both matched */
                 GAPProvideLongTermKeyRequest_t req;
                 req.DeviceId = pMsg->Data.GAPConnectionEventLongTermKeyRequestIndication.DeviceId;
                 req.LtkSize =  gSmpKeys.cLtkSize;
                 req.Ltk = gSmpKeys.aLtk;
                 GAPProvideLongTermKeyRequest(&req, gFsciInterface_c);
            }
            else
            /* EDIV or RAND size did not match */
            {
                GAPDenyLongTermKeyRequest_t req;
                req.DeviceId = pMsg->Data.GAPConnectionEventLongTermKeyRequestIndication.DeviceId;
                GAPDenyLongTermKeyRequest(&req, gFsciInterface_c);
            }
#endif
        }
        break;

        default:
        break;
    }
}

/*!*************************************************************************************************
*\fn    static void App_HandleHSDKMessageInputGATT(bleEvtContainer_t *pMsg)
*
*\brief Handles GATT specific events
*
* \param[in]   pMsg    Pointer to event container
*
*\return       None
***************************************************************************************************/
static bool_t App_HandleHSDKMessageInputGATT(bleEvtContainer_t* pMsg)
{
    bool_t matchFound = TRUE;

    switch (pMsg->id)
    {
        case GATTClientProcedureDiscoverAllPrimaryServicesIndication_FSCI_ID:
        {
            deviceId_t deviceId = pMsg->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication.DeviceId;
            maServDiscInfo[deviceId].mcPrimaryServices = pMsg->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication.NbOfDiscoveredServices;
            maServDiscInfo[deviceId].mpServiceDiscoveryBuffer =
                (gattService_t*)pMsg->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication.DiscoveredServices;

            BleServDisc_SignalGattClientEvent(deviceId,
                                              gGattProcDiscoverAllPrimaryServices_c,
                                              (gattProcedureResult_t)pMsg->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication.ProcedureResult,
                                              (bleResult_t)pMsg->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication.Error);
        }
        break;

        case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_FSCI_ID:
        {
            deviceId_t deviceId = pMsg->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.DeviceId;
            maServDiscInfo[deviceId].mcPrimaryServices = pMsg->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.NbOfDiscoveredServices;
            maServDiscInfo[deviceId].mpServiceDiscoveryBuffer->startHandle =
                pMsg->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.DiscoveredServices->StartHandle;
            maServDiscInfo[deviceId].mpServiceDiscoveryBuffer->endHandle =
                pMsg->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.DiscoveredServices->EndHandle;

            if (pMsg->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.DiscoveredServices->UuidType == Uuid16Bits)
            {
                maServDiscInfo[deviceId].mpServiceDiscoveryBuffer->uuidType = gBleUuidType16_c;
                FLib_MemCpy(&maServDiscInfo[deviceId].mpServiceDiscoveryBuffer->uuid.uuid16,
                            pMsg->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.DiscoveredServices->Uuid.Uuid16Bits,
                            sizeof(uint16_t));
            }
            else
            {
                maServDiscInfo[deviceId].mpServiceDiscoveryBuffer->uuidType = gBleUuidType128_c;
                FLib_MemCpy(maServDiscInfo[deviceId].mpServiceDiscoveryBuffer->uuid.uuid128,
                            pMsg->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.DiscoveredServices->Uuid.Uuid16Bits,
                            16);
            }
            maServDiscInfo[deviceId].mpServiceDiscoveryBuffer->cNumCharacteristics =
                pMsg->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.DiscoveredServices->NbOfCharacteristics;
            maServDiscInfo[deviceId].mpServiceDiscoveryBuffer->cNumIncludedServices =
                pMsg->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.DiscoveredServices->NbOfIncludedServices;

            BleServDisc_SignalGattClientEvent(pMsg->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.DeviceId,
                                              gGattProcDiscoverPrimaryServicesByUuid_c,
                                              (gattProcedureResult_t)pMsg->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.ProcedureResult,
                                              (bleResult_t)pMsg->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.Error);
        }
        break;

        case GATTClientProcedureFindIncludedServicesIndication_FSCI_ID:
        {
            BleServDisc_SignalGattClientEvent(pMsg->Data.GATTClientProcedureFindIncludedServicesIndication.DeviceId,
                                              gGattProcFindIncludedServices_c,
                                              (gattProcedureResult_t)pMsg->Data.GATTClientProcedureFindIncludedServicesIndication.ProcedureResult,
                                              (bleResult_t)pMsg->Data.GATTClientProcedureFindIncludedServicesIndication.Error);
        }
        break;

        case GATTClientProcedureDiscoverAllCharacteristicsIndication_FSCI_ID:
        {
            maServDiscInfo[pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication.DeviceId].mpServiceDiscoveryBuffer->cNumCharacteristics =
                pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication.Service.NbOfCharacteristics;
            maServDiscInfo[pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication.DeviceId].mpServiceDiscoveryBuffer->aCharacteristics =
                (gattCharacteristic_t*)pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication.Service.Characteristics;
            BleServDisc_SignalGattClientEvent(pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication.DeviceId,
                                              gGattProcDiscoverAllCharacteristics_c,
                                              (gattProcedureResult_t)pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication.ProcedureResult,
                                              (bleResult_t)pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication.Error);
        }
        break;

        case GATTClientProcedureDiscoverCharacteristicByUuidIndication_FSCI_ID:
        {
            BleServDisc_SignalGattClientEvent(pMsg->Data.GATTClientProcedureDiscoverCharacteristicByUuidIndication.DeviceId,
                                              gGattProcDiscoverCharacteristicByUuid_c,
                                              (gattProcedureResult_t)pMsg->Data.GATTClientProcedureDiscoverCharacteristicByUuidIndication.ProcedureResult,
                                              (bleResult_t)pMsg->Data.GATTClientProcedureDiscoverCharacteristicByUuidIndication.Error);
        }
        break;

        case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_FSCI_ID:
        {
            BleServDisc_SignalGattClientEvent(pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication.DeviceId,
                                              gGattProcDiscoverAllCharacteristicDescriptors_c,
                                              (gattProcedureResult_t)pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication.ProcedureResult,
                                              (bleResult_t)pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication.Error);
        }
        break;

        case GATTClientProcedureReadCharacteristicValueIndication_FSCI_ID:
        {
            BleServDisc_SignalGattClientEvent(pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication.DeviceId,
                                              gGattProcDiscoverAllCharacteristicDescriptors_c,
                                              (gattProcedureResult_t)pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication.ProcedureResult,
                                              (bleResult_t)pMsg->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication.Error);
        }
        break;

        case GATTConfirm_FSCI_ID:
        case GATTClientProcedureWriteCharacteristicValueIndication_FSCI_ID:
        case GATTClientProcedureReadCharacteristicDescriptorIndication_FSCI_ID:
        case GATTClientProcedureWriteCharacteristicDescriptorIndication_FSCI_ID:
        {
            ; /* Untreated GATT events */
        }
        break;

        default:
        {
            matchFound = FALSE;
        }
        break;
    }
    return matchFound;
}



#if (defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 1U)) && \
    (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
/*!*************************************************************************************************
*\fn    static void App_ManagePrivacyInternal(bleEvtContainer_t *pMsg)
*
*\brief Handles privacy related events
*
* \param[in]   pMsg    Pointer to event container
*
*\return       None
***************************************************************************************************/
static void App_ManagePrivacyInternal(bleEvtContainer_t* pMsg)
{
    switch (pMsg->id)
    {
        case GAPGenericEventDeviceAddedToFilterAcceptListIndication_FSCI_ID:
        {
            if (mLastCheckNewBondValue == TRUE)
            {
                /* New Bonded Device */
                GAPGetBondedDevicesIdentityInformationRequest_t req;
                req.maxDevices = gMaxResolvingListSize_c;
                GAPGetBondedDevicesIdentityInformationRequest(&req, gFsciInterface_c);
            }
            else
            {
                 /* New bonded devices found */
                if (mFilterAcceptListCount >= mIdentityInfoCount)
                {
                    if ((mIdentityInfoCount == mcDevicesInResolvingList + 1U)
                        || (mLastCheckNewBondValue == FALSE))
                    {
                        App_DisablePrivacy();

                        mcDevicesInResolvingList = mIdentityInfoCount;

                        if( mcDevicesInResolvingList > (uint8_t)gMaxResolvingListSize_c )
                        {
                            mcDevicesInResolvingList = gMaxResolvingListSize_c;
                        }

                        if (FALSE == mHaveRandomAddress)
                        {
                            /* Create Random Address */
                            mSettingRandomAddressFromApplication = TRUE;
                            GAPCreateRandomDeviceAddressRequest_t req;
                            req.IrkIncluded = TRUE;
                            FLib_MemCpy(req.Irk, gSmpKeys.aIrk, gcSmpIrkSize_c);
                            req.RandomPartIncluded = FALSE;
                            FLib_MemSet(req.RandomPart, 0U, 3U);
                            GAPCreateRandomDeviceAddressRequest(&req, gFsciInterface_c);
                        }
                    }
                }
            }
        }
        break;

        case GAPGetBondedDevicesIdentityInformationIndication_FSCI_ID:
        {
            mIdentityInfoCount = pMsg->Data.GAPGetBondedDevicesIdentityInformationIndication.NbOfDeviceIdentityAddresses;

            if (mIdentityInfoCount > 0U)
            {
                if (pOutIdentityAddresses == NULL)
                {
                    pOutIdentityAddresses = MEM_BufferAlloc(sizeof(gapIdentityInformation_t) * mIdentityInfoCount);
                }

                if (pOutIdentityAddresses != NULL)
                {
                     /* Copy identity information */
                    for (uint8_t idx = 0U; idx < mIdentityInfoCount; idx++)
                    {
                        pOutIdentityAddresses[idx].identityAddress.idAddressType =
                            (bleAddressType_t)pMsg->Data.GAPGetBondedDevicesIdentityInformationIndication.IdentityAddresses[idx].IdentityAddressType;
                        FLib_MemCpy(pOutIdentityAddresses[idx].identityAddress.idAddress,
                                    pMsg->Data.GAPGetBondedDevicesIdentityInformationIndication.IdentityAddresses[idx].IdentityAddress,
                                    gcBleDeviceAddressSize_c);
                        FLib_MemCpy(pOutIdentityAddresses[idx].irk,
                                    pMsg->Data.GAPGetBondedDevicesIdentityInformationIndication.IdentityAddresses[idx].Irk,
                                    gcSmpIrkSize_c);
                        pOutIdentityAddresses[idx].privacyMode =
                            (blePrivacyMode_t)pMsg->Data.GAPGetBondedDevicesIdentityInformationIndication.IdentityAddresses[idx].PrivacyMode;
                    }

                    /* Add first device to filter accept list */
                    if (mLastCheckNewBondValue == FALSE)
                    {
                        if (mIdentityInfoCount > 1U)
                        {
                            mFilterAcceptListCount = 1U;
                            RegisterRemovableObserver(GAPConfirm_FSCI_ID,
                                                      hsdkObserverGAPAddDeviceToFilterAcceptList);
                        }

                        GAPAddDeviceToFilterAcceptListRequest_t req;
                        req.AddressType =
                          (GAPAddDeviceToFilterAcceptListRequest_AddressType_t)pOutIdentityAddresses[0].identityAddress.idAddressType;
                        FLib_MemCpy(req.Address,
                                    pOutIdentityAddresses[0].identityAddress.idAddress,
                                    gcBleDeviceAddressSize_c);
                        GAPAddDeviceToFilterAcceptListRequest(&req, gFsciInterface_c);
                    }
                }
            }
            else
            {
                /* No identity information available - enable Host Privacy */
                GAPEnableHostPrivacyRequest_t req;
                req.Enable = TRUE;
                FLib_MemCpy(req.Irk, gSmpKeys.aIrk, gcSmpIrkSize_c);
                GAPEnableHostPrivacyRequest(&req, gFsciInterface_c);
            }
        }
        break;

        case GAPGenericEventRandomAddressReadyIndication_FSCI_ID:
        {
            /* Set the newly generated address */
            if ((FALSE == mHaveRandomAddress) &&
                (TRUE == mSettingRandomAddressFromApplication))
            {
                GAPSetRandomAddressRequest_t req;
                FLib_MemCpy(req.Address,
                            pMsg->Data.GAPGenericEventRandomAddressReadyIndication.Address,
                            gcBleDeviceAddressSize_c);
                GAPSetRandomAddressRequest(&req, gFsciInterface_c);
            }
        }
        break;

        case GAPGenericEventRandomAddressSetIndication_FSCI_ID:
        {
            /* Enable Privacy */
#if defined(gBleEnableControllerPrivacy_d) && (gBleEnableControllerPrivacy_d > 0)
            if (pOutIdentityAddresses != NULL)
            {
                GAPEnableControllerPrivacyRequest_t req;
                req.Enable = TRUE;
                FLib_MemCpy(req.OwnIrk, gSmpKeys.aIrk, gcSmpIrkSize_c);
                req.PeerIdCount = mcDevicesInResolvingList;
                req.PeerIdentities = MEM_BufferAlloc(
                            (uint32_t)gMaxResolvingListSize_c *
                            sizeof(gapIdentityInformation_t));
                if (req.PeerIdentities != NULL)
                {
                    for (uint8_t idx = 0; idx < mcDevicesInResolvingList; idx++)
                    {
                        req.PeerIdentities[idx].IdentityAddressType =
                          (GAPEnableControllerPrivacyRequest_PeerIdentities_IdentityAddressType_t)pOutIdentityAddresses[idx].identityAddress.idAddressType;
                        FLib_MemCpy(req.PeerIdentities[idx].IdentityAddress,
                                    pOutIdentityAddresses[idx].identityAddress.idAddress,
                                    gcBleDeviceAddressSize_c);
                         FLib_MemCpy(req.PeerIdentities[idx].Irk,
                                    pOutIdentityAddresses[idx].irk,
                                    gcBleDeviceAddressSize_c);
                         req.PeerIdentities[idx].PrivacyMode =
                             (GAPEnableControllerPrivacyRequest_PeerIdentities_PrivacyMode_t)pOutIdentityAddresses[idx].privacyMode;
                    }

                    GAPEnableControllerPrivacyRequest(&req, gFsciInterface_c);
                }
            }
#else  /* gBleEnableControllerPrivacy_d */
            if (TRUE == mSettingRandomAddressFromApplication)
            {
                mSettingRandomAddressFromApplication = FALSE;
                GAPEnableHostPrivacyRequest_t req;
                req.Enable = TRUE;
                FLib_MemCpy(req.Irk, gSmpKeys.aIrk, gcSmpIrkSize_c);
                GAPEnableHostPrivacyRequest(&req, gFsciInterface_c);
            }
#endif /* gBleEnableControllerPrivacy_d */
        }
        break;

        case GAPGenericEventControllerPrivacyStateChangedIndication_FSCI_ID:
        {
            if (pMsg->Data.GAPGenericEventControllerPrivacyStateChangedIndication.NewControllerPrivacyState == TRUE)
            {
                mbPrivacyEnabled = TRUE;
            }
            else
            {
                mbPrivacyEnabled = FALSE;
            }
        }
        break;

        case GAPGenericEventHostPrivacyStateChangedIndication_FSCI_ID:
        {
            if (pMsg->Data.GAPGenericEventHostPrivacyStateChangedIndication.NewHostPrivacyState == TRUE)
            {
                mbPrivacyEnabled = TRUE;
            }
            else
            {
                mbPrivacyEnabled = FALSE;
            }
        }
        break;

        default:
            break;

    }
}

/*!*************************************************************************************************
*\fn    static void App_DisablePrivacy(void)
*
*\brief Disables host or controller privacy
*
* \param[in]   pMsg    Pointer to event container
*
*\return       None
***************************************************************************************************/
static void App_DisablePrivacy(void)
{
    if( mbPrivacyEnabled )
    {
#if defined(gBleEnableControllerPrivacy_d) && \
    (gBleEnableControllerPrivacy_d > 0)
        if( mcDevicesInResolvingList == 0U )
        {
            GAPEnableHostPrivacyRequest_t req;
            req.Enable = FALSE;
            FLib_MemSet(req.Irk, 0U, gcSmpIrkSize_c);
            GAPEnableHostPrivacyRequest(&req, gFsciInterface_c);
        }
        else
        {
            GAPEnableControllerPrivacyRequest_t req;
            req.Enable = FALSE;
            FLib_MemSet(req.OwnIrk, 0U, gcSmpIrkSize_c);
            req.PeerIdCount = 0;
            req.PeerIdentities = NULL;
            GAPEnableControllerPrivacyRequest(&req, gFsciInterface_c);
        }
#else  /* gBleEnableControllerPrivacy_d */
        GAPEnableHostPrivacyRequest_t req;
        req.Enable = FALSE;
        FLib_MemSet(req.Irk, 0U, gcSmpIrkSize_c);
        GAPEnableHostPrivacyRequest(&req, gFsciInterface_c);
#endif /* gBleEnableControllerPrivacy_d */

        mbPrivacyEnabled = FALSE;
    }
}
#endif /* (gAppUsePrivacy_d) && (gAppUseBonding_d) */

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
/*! *********************************************************************************
*\fn         void BleConnManager_MCUInfoToSmpKeys(void)
*\brief      Generates LTK, IRK, CSRK, ediv and rand.
*
*\param[in]  none.
*
*\retval     void.
********************************************************************************** */
static void BleConnManager_MCUInfoToSmpKeys(void)
{
    uint8_t uid[16] = {0};
    uint8_t len = 0;
    uint8_t sha256Output[SHA256_HASH_SIZE] = {0};

    PLATFORM_GetMCUUid (uid, &len);

    if(len > 0U)
    {
        /* generate LTK. LTK size always smaller than SHA1 hash size */
        uid[len - 1U]++;
        SHA256_Hash (uid, len, sha256Output);
        FLib_MemCpy (gSmpKeys.aLtk, sha256Output, gSmpKeys.cLtkSize);

#if (!defined(gUseCustomIRK_d) || (gUseCustomIRK_d == 0U))
        /* generate IRK */
        uid[len - 1U]++;
        SHA256_Hash (uid, len, sha256Output);
        FLib_MemCpy (gSmpKeys.aIrk, sha256Output, gcSmpIrkSize_c);
#endif /* gUseCustomIRK_d */

        /* generate CSRK */
        uid[len - 1U]++;
        SHA256_Hash (uid, len, sha256Output);
        FLib_MemCpy (gSmpKeys.aCsrk, sha256Output, gcSmpCsrkSize_c);

        /* generate ediv and rand */
        uid[len - 1U]++;
        SHA256_Hash (uid, len, sha256Output);
        gSmpKeys.ediv = (uint16_t)sha256Output[0];
        FLib_MemCpy (&(gSmpKeys.ediv),
                     &(sha256Output[0]),
                     sizeof(gSmpKeys.ediv));
        FLib_MemCpy (gSmpKeys.aRand,
                     &(sha256Output[sizeof(gSmpKeys.ediv)]),
                     gSmpKeys.cRandSize);
    }
}
#endif
/*! *********************************************************************************
* @}
********************************************************************************** */
