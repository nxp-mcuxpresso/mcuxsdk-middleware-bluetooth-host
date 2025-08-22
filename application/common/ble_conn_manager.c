/*! *********************************************************************************
 * \addtogroup BLE
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2016-2025 NXP
*
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
#include "ble_general.h"
#include "gap_types.h"
#include "gap_interface.h"
#include "ble_conn_manager.h"
#include "board.h"
#include "ble_config.h"
#include "fwk_platform.h"
#include "fwk_seclib.h"
#if (defined(gRepeatedAttempts_d) && (gRepeatedAttempts_d == 1U))
#include "fsl_component_timer_manager.h"
#endif /* gRepeatedAttempts_d */

#include "ble_config.h"
#include "fsl_component_mem_manager.h"
#include "fsl_component_panic.h"

/************************************************************************************
 *************************************************************************************
 * Private macros
 *************************************************************************************
 ************************************************************************************/
#define BleConnManager_GetLocalIrk(pOut)   BleConnManager_GetLocalKey(0U, pOut);
#define BleConnManager_GetLocalCsrk(pOut)  BleConnManager_GetLocalKey(1U, pOut);
/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
bleDeviceAddress_t gaBleDeviceAddress;
bool_t gSettingRandomStaticAddress = FALSE;

#if defined(__CC_ARM)
/* These  definitions are used only by the demo applications which do not have
 * advertisement capabilities in order to fix Keil compilation errors.
 * All other demo applications use the definitions present in app_config.c file.
 */
gapAdvertisingData_t            gAppAdvertisingData __attribute__((weak));
gapScanResponseData_t           gAppScanRspData __attribute__((weak));
gapSmpKeys_t                    gSmpKeys __attribute__((weak));
gapScanningParameters_t         gScanParams __attribute__((weak));
gapAdvertisingParameters_t      gAdvParams __attribute__((weak));
#endif /* __CC_ARM */

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef struct repeatedAttemptsDevice_tag
{
    bleDeviceAddress_t  address;
    uint16_t            baseTimeout;   /* seconds */
    uint16_t            remainingTime; /* seconds */
} repeatedAttemptsDevice_t;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
#if defined(gRandomStaticAddress_d) && (gRandomStaticAddress_d > 0)
STATIC void BleConnManager_MCUInfoToRandomStaticAddress(void);
#endif

#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
STATIC void BleConnManager_ManageLocalKeys(void);
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
STATIC void BleConnManager_GetLocalKey(uint8_t id, uint8_t* pOut);
#endif /* gAppUsePairing_d */
#else
STATIC void BleConnManager_MCUInfoToSmpKeys(void);
#endif
STATIC void BleConnManager_DataLengthUpdateProcedure(deviceId_t peerDeviceId);
#if (defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 1U)) && \
    (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
STATIC bleResult_t BleConnManager_ManagePrivacyInternal(bool_t bCheckNewBond);
#endif /* gAppUsePrivacy_d */

#if (defined(gRepeatedAttempts_d) && (gRepeatedAttempts_d == 1U))
STATIC bool_t   RepeatedAttempts_CheckRequest(bleDeviceAddress_t address);
STATIC void     RepeatedAttempts_LogAttempt
                (
                    gapPairingCompleteEvent_t *pEvent,
                    bleDeviceAddress_t        address
                );
STATIC void     RepeatedAttempts_UpdateTimePassed(uint16_t seconds);
STATIC uint16_t RepeatedAttempts_GetMinTimeToWait();
STATIC void     RepeatedAttempts_TimerCb(void *param);
#endif /* gRepeatedAttempts_d */

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
STATIC void BleConnManager_GapPeripheralEventEvtKeyExchangeRequest
                (
                    deviceId_t peerDeviceId,
                    gapConnectionEvent_t* pConnectionEvent
                );
STATIC void BleConnManager_GapPeripheralEventEvtPairingComplete
                (
                    deviceId_t            peerDeviceId,
                    gapConnectionEvent_t* pConnectionEvent
                );
STATIC void BleConnManager_GapCentralEventEvtKeyExchangeRequest
                (
                    deviceId_t            peerDeviceId,
                    gapConnectionEvent_t* pConnectionEvent
                );
#endif
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
STATIC leSupportedFeatures_t             mSupportedFeatures;

#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
 /* The number of bonded devices. It is incremented each time a new bond is created,
    on 'gBondCreatedEvent_c' generic event. When a bond or all bonds are removed,
    this variable must be updated accordingly by the application, since there
    is no corresponding 'bond removed' event triggered by the LE Host Stack */
uint8_t gcBondedDevices = 0;
#if gAppUsePrivacy_d
STATIC uint8_t mcDevicesInResolvingList = 0; /*
                                              * Number of devices present in host
                                              * resolving list
                                              */
STATIC bool_t  mbPrivacyEnabled = FALSE;
STATIC bool_t  mHaveRandomAddress = FALSE;
STATIC bool_t  mSettingRPAFromApplication = FALSE;
#endif /* gAppUsePrivacy_d */
#endif /* gAppUseBonding_d */

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
static bleDeviceAddress_t   maPeerDeviceAddress;
#if gAppUseBonding_d
static bleAddressType_t     mPeerDeviceAddressType;
#endif /* gAppUseBonding_d */
STATIC uint8_t              mSuccessfulPairings;
STATIC uint8_t              mFailedPairings;
#endif /* gAppUsePairing_d */

#if (defined(gRepeatedAttempts_d) && (gRepeatedAttempts_d == 1U))
repeatedAttemptsDevice_t    maPairingPeers[gRepeatedAttemptsNoOfDevices_c] = {0};
STATIC bleDeviceAddress_t   maPeerDeviceOriginalAddress;
static TIMER_MANAGER_HANDLE_DEFINE(mRepeatedAttemptsTimerId);
STATIC uint16_t             mMinTimeToWait = 0;
#endif /* gRepeatedAttempts_d */


/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn           void BleConnManager_GenericEvent(gapGenericEvent_t* pGenericEvent)
*\brief        Performs common operations on the BLE stack on the generic callback.
*
*\param  [in]  pGenericEvent    GAP Generic event from the Host Stack.
*
*\retval       void.
********************************************************************************** */
void BleConnManager_GenericEvent(gapGenericEvent_t* pGenericEvent)
{
    switch (pGenericEvent->eventType)
    {
        case gInitializationComplete_c:
        {
            /* Save feature set */
            mSupportedFeatures =
                 pGenericEvent->eventData.initCompleteData.supportedFeatures;
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
            BleConnManager_ManageLocalKeys();
#else
            BleConnManager_MCUInfoToSmpKeys();
#endif
#if defined(gRandomStaticAddress_d) && (gRandomStaticAddress_d > 0)
            BleConnManager_MCUInfoToRandomStaticAddress();
#endif
#if (defined(gRepeatedAttempts_d) && (gRepeatedAttempts_d == 1U))
            (void)TM_Open(mRepeatedAttemptsTimerId);
#endif /* gRepeatedAttempts_d */

        }
        break;

#if (defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 1U)) && \
    (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))

        case gRandomAddressSet_c:
        {
#if defined(gRandomStaticAddress_d) && (gRandomStaticAddress_d > 0)
            /* The Random Static Address does not count as RPA/NRPA -
               mHaveRandomAddress is set to TRUE on the else branch */
            if (gSettingRandomStaticAddress == TRUE)
            {
                gSettingRandomStaticAddress = FALSE;
            }
            else
            {
#endif
                mHaveRandomAddress = TRUE;
#if defined(gRandomStaticAddress_d) && (gRandomStaticAddress_d > 0)
            }
#endif
        }
        break;

        case gRandomAddressReady_c:
        {
            if ((FALSE == mHaveRandomAddress) &&
                (TRUE == mSettingRPAFromApplication))
            {
                mSettingRPAFromApplication = FALSE;
                (void)Gap_SetRandomAddress(
                        pGenericEvent->eventData.addrReady.aAddress);
            }
        }
        break;

#endif /* gAppUseBonding_d && gAppUsePrivacy_d */

        case gPublicAddressRead_c:
        {
            /* Use address read from the controller */
            FLib_MemCpy(gaBleDeviceAddress,
                        pGenericEvent->eventData.aAddress,
                        sizeof(bleDeviceAddress_t));
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
            gSmpKeys.addressType = gBleAddrTypePublic_c;
            gSmpKeys.aAddress = gaBleDeviceAddress;
#endif /* gAppUsePairing_d */
        }
        break;

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
        case gLeScPublicKeyRegenerated_c:
        {
            /* Key pair regenerated -> reset pairing counters */
            mFailedPairings = mSuccessfulPairings = 0;
        }
        break;
#endif /* gAppUsePairing_d */

#if defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U)
        case gBondCreatedEvent_c:
        {
            /*
             * Stack created a bond after pairing or at app
             * request, update global counter.
             */
            gcBondedDevices++;
        }
        break;

        /*
         * There is no event for bond removal. This action
         * is driven by the application and is synchronous,
         * so it is application's responsibility to update
         * gcBondedDevices if necessary.
         */
#endif /* gAppUseBonding_d */

#if (defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 1U)) && \
    (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
#if defined(gBleEnableControllerPrivacy_d) && \
    (gBleEnableControllerPrivacy_d > 0)
        case gControllerPrivacyStateChanged_c:
        {
            /*
             * If controller privacy has been disabled,
             * set a new random address when it is
             * enabled again - IRK may have changed
             */
            if (pGenericEvent->eventData.newControllerPrivacyState ==
                FALSE)
            {
                mHaveRandomAddress = FALSE;
                mbPrivacyEnabled = FALSE;
            }
            else
            {
                mbPrivacyEnabled = TRUE;
            }
        }
        break;
#endif /* gBleEnableControllerPrivacy_d */
        case gHostPrivacyStateChanged_c:
        {
            if (pGenericEvent->eventData.newHostPrivacyState == TRUE)
            {
                mbPrivacyEnabled = TRUE;
            }
            else
            {
                mbPrivacyEnabled = FALSE;
            }
        }
        break;
#endif /* gAppUseBonding_d && gAppUsePrivacy_d */

        case gInternalError_c:
        {
            /*
             * This is a generic error which is triggered when a
             * dedicated error does not exist for the specific
             * problem. Treated below are some of the most
             * important error cases which could trigger an
             * internal error. Modify at need according to use
             * case. A list of all error codes and sources can
             * be found in ble_general.h - see bleResult_t and
             * gapInternalErrorSource_t enums. The error codes
             * for which panic is not triggered are not critical,
             * but are included here for awareness.
             */
            switch (pGenericEvent->eventData.internalError.errorCode)
            {
                /*
                 * An error occurred at the OS level.
                 * Host or Controller task creation
                 * or initialization failed.
                 */
                case gBleOsError_c: /*Fall Through*/
                /*
                 * A "should never get here"-type error.
                 * BLE Radio Init failed or an error occured
                 * when starting the next advertising event
                 * for the next SID while in
                 * Limited Discoverable Mode.
                 */
                case gBleUnexpectedError_c: /*Fall Through*/

                /*
                 * HCI layer initialization failure. HCI
                 * data buffering semaphore allocation error.
                 * HCI layer received an unknown packet type.
                 * Serial initialization or write failure.
                 */
                case gHciTransportError_c: /*Fall Through*/

                /*
                 * There is a problem with the internal
                 * state of the SM. This should not happen
                 *  during normal operations.
                 */
                case gSmInvalidInternalOperation_c: /*Fall Through*/

                /*
                 * Timer allocation failed.
                 * Consider increasing the number of timers
                 * in app_preinclude.h.
                 */
                case gBleTimerError_c:
                {
                    panic(0,0,0,0);
                }
                break;

                /*
                 * The Host has temporarily run out of buffers.
                 * This issue should be handled by the Host
                 * by freeing resources after data processing.
                 * If this error code persists on being received,
                 * consider modifying the buffer configuration
                 * in app_preinclude.h.
                 */
                case gBleOutOfMemory_c: /*Fall Through*/

                /* An internal limit is reached:
                 * L2CAP TX queue is full.
                 * This issue should be handled by the Host
                 * by freeing resources after data processing.
                 * Exceeded the allowed number of connections.
                 * Consider updating gAppMaxConnections_c in
                 * app_preinclude.h according to hardware and
                 * use case.
                 */
                case gBleOverflow_c:
                {
                    ; /*
                       * For awareness only.
                       * No action required
                       */
                }
                break;

                case gHciInvalidHciCommandParameters_c:
                {
                    if (pGenericEvent->eventData.internalError.errorSource == gSetChannelMap_c)
                    {
                        ; /*
                           * Gap_SetChannelMap command
                           * called with invalid parameters; check if the
                           * Channel Map complies with the BLE specification
                           * (it should include at least one valid channel
                           * and not use any advertising channels)
                           */
                    }
                }
                break;

                default:
                {
                    ; /* No action required */
                }
                break;
            }
        }
        break;

        default:
        {
            ; /* No action required */
        }
        break;
    }
}

/*! *********************************************************************************
*\fn           void BleConnManager_GapPeripheralEvent(
*                  deviceId_t            peerDeviceId,
*                  gapConnectionEvent_t* pConnectionEvent)
*\brief        Performs GAP Peripheral operations on the BLE stack on the
*              connection callback.
*
*\param  [in]  peerDeviceId        The GAP peer Id.
*
*\param  [in]  pConnectionEvent    GAP Connection event from the Host Stack.
*
*\retval       void.
********************************************************************************** */
void BleConnManager_GapPeripheralEvent
(
    deviceId_t            peerDeviceId,
    gapConnectionEvent_t* pConnectionEvent
)
{
    switch (pConnectionEvent->eventType)
    {
        case gConnEvtConnected_c:
        {
#ifndef gCentralInitiatedPairing_d
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
#if (defined(gRepeatedAttempts_d) && (gRepeatedAttempts_d == 1U))
            FLib_MemCpy(maPeerDeviceOriginalAddress,
                        pConnectionEvent->eventData.connectedEvent.peerAddress,
                        sizeof(bleDeviceAddress_t));
#endif /* gRepeatedAttempts_d */
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
            bool_t isBonded = FALSE;
            uint8_t nvmIndex = gInvalidNvmIndex_c;

            /* Copy peer device address information */
            mPeerDeviceAddressType =
                      pConnectionEvent->eventData.connectedEvent.peerAddressType;
            FLib_MemCpy(maPeerDeviceAddress,
                        pConnectionEvent->eventData.connectedEvent.peerAddress,
                        sizeof(bleDeviceAddress_t));

            /*
             * Perform pairing if peer is not bonded or resolution procedure
             * for its address failed
             */
            if ((gBleSuccess_c == Gap_CheckIfBonded(peerDeviceId, &isBonded, &nvmIndex) &&
                FALSE == isBonded) ||
                (Ble_IsPrivateResolvableDeviceAddress(maPeerDeviceAddress) &&
                FALSE == pConnectionEvent->eventData.connectedEvent.peerRpaResolved))
#endif /* gAppUseBonding_d */
            {
                (void)Gap_SendPeripheralSecurityRequest(peerDeviceId, &gPairingParameters);
            }
#endif /* gAppUsePairing_d */
#endif /* gCentralInitiatedPairing_d */

#if gConnUpdateAlwaysAccept_d
            (void)Gap_EnableUpdateConnectionParameters(peerDeviceId, TRUE);
#endif /* gConnUpdateAlwaysAccept_d */

            /* Initiate Data Length Update Procedure */
            BleConnManager_DataLengthUpdateProcedure(peerDeviceId);

#if gConnInitiatePhyUpdateRequest_c
            if ((mSupportedFeatures & ((leSupportedFeatures_t)gLe2MbPhy_c |
                (leSupportedFeatures_t)gLeCodedPhy_c)) != 0U)
            {
                (void)Gap_LeSetPhy(FALSE,
                                   peerDeviceId,
                                   0,
                                   gConnPhyUpdateReqTxPhySettings_c,
                                   gConnPhyUpdateReqRxPhySettings_c,
                                   (uint16_t)gConnPhyUpdateReqPhyOptions_c);
            }
#endif /* gConnInitiatePhyUpdateRequest_c */
        }
        break;

        case gConnEvtPairingRequest_c:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
            gPairingParameters.centralKeys =
                  pConnectionEvent->eventData.pairingEvent.centralKeys;
#if (defined(gRepeatedAttempts_d) && (gRepeatedAttempts_d == 1U))
            if (RepeatedAttempts_CheckRequest(maPeerDeviceOriginalAddress) == TRUE)
            {
                (void)Gap_AcceptPairingRequest(peerDeviceId,
                                               &gPairingParameters);
            }
            else
            {
                (void)Gap_RejectPairing(peerDeviceId,
                                        gRepeatedAttempts_c);
            }
#else  /* gRepeatedAttempts_d */
            (void)Gap_AcceptPairingRequest(peerDeviceId,
                                           &gPairingParameters);
#endif /* gRepeatedAttempts_d */
#else  /* gAppUsePairing_d */
            (void)Gap_RejectPairing(peerDeviceId,
                                    gPairingNotSupported_c);
#endif /* gAppUsePairing_d */
        }
        break;

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))

        case gConnEvtPasskeyRequest_c:
        {
            /* Depending on the IO capabilities, the peripheral may be required to input a passkey.
               The passkey should be read here, e.g., from UART/keyboard and passed to the stack by
               calling Gap_EnterPasskey */
               ;
        }
        break;

        case gConnEvtKeyExchangeRequest_c:
        {
            BleConnManager_GapPeripheralEventEvtKeyExchangeRequest(peerDeviceId, pConnectionEvent);
        }
        break;

        case gConnEvtKeysReceived_c:
        {
            /* Copy peer device address information when IRK is used */
            if (pConnectionEvent->eventData.keysReceivedEvent.pKeys->aIrk != NULL)
            {
#if gAppUseBonding_d
                mPeerDeviceAddressType =
                       pConnectionEvent->eventData.keysReceivedEvent.pKeys->addressType;
#endif /* gAppUseBonding_d */
                FLib_MemCpy(maPeerDeviceAddress,
                            pConnectionEvent->eventData.keysReceivedEvent.pKeys->aAddress,
                            sizeof(bleDeviceAddress_t));
            }
        }
        break;

        case gConnEvtPairingComplete_c:
        {
            BleConnManager_GapPeripheralEventEvtPairingComplete(peerDeviceId, pConnectionEvent);
        }
        break;

        case    gConnEvtLongTermKeyRequest_c:
        {
            if ((pConnectionEvent->eventData.longTermKeyRequestEvent.ediv == gSmpKeys.ediv) &&
                (pConnectionEvent->eventData.longTermKeyRequestEvent.randSize == gSmpKeys.cRandSize) &&
                (TRUE == FLib_MemCmp(pConnectionEvent->eventData.longTermKeyRequestEvent.aRand,
                                     gSmpKeys.aRand,
                                     gSmpKeys.cRandSize)))
            {
                /* EDIV and RAND both matched */
                (void)Gap_ProvideLongTermKey(peerDeviceId,
                                             gSmpKeys.aLtk,
                                             gSmpKeys.cLtkSize);
            }
            else
            /* EDIV or RAND size did not match */
            {
                (void)Gap_DenyLongTermKey(peerDeviceId);
            }
        }
        break;

        case gConnEvtLeScDisplayNumericValue_c:
        {
            (void) pConnectionEvent->eventData.numericValueForDisplay;
            /*
             * Display on a screen for user confirmation then
             * validate/invalidate based on value.
             */
            (void)Gap_LeScValidateNumericValue(peerDeviceId, TRUE);
        }
        break;

        case gConnEvtPairingNoLtk_c:
        {
#ifndef gCentralInitiatedPairing_d
            /* Peripheral does not have the LTK for peer Central.
             * Re-initiate pairing.
             */
            (void)Gap_SendPeripheralSecurityRequest(peerDeviceId,
                                               &gPairingParameters);
#endif /* gCentralInitiatedPairing_d */
        }
        break;

#endif /* gAppUsePairing_d */

        default:
        {
            ; /* No action required */
        }
        break;
    }
}

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
/*! *********************************************************************************
*\private
*\fn           void BleConnManager_GapPeripheralEventEvtKeyExchangeRequest(
*                  deviceId_t            peerDeviceId,
*                  gapConnectionEvent_t* pConnectionEvent)
*\brief        Handles gConnEvtKeyExchangeRequest_c event from BleConnManager_GapPeripheralEvent.
*              This function is created so BleConnManager_GapPeripheralEvent can
*              respect the under 20 CCM metric. 
*
*\param  [in]  peerDeviceId        The GAP peer Id.
*
*\param  [in]  pConnectionEvent    GAP Connection event from the Host Stack.
*
*\retval       void.
********************************************************************************** */
STATIC void BleConnManager_GapPeripheralEventEvtKeyExchangeRequest
(
    deviceId_t            peerDeviceId,
    gapConnectionEvent_t* pConnectionEvent
)
{
    gapSmpKeys_t sentSmpKeys = gSmpKeys;
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))            
    uint8_t aIrk[gcSmpIrkSize_c];
    uint8_t aCsrk[gcSmpCsrkSize_c];
#endif
    if ((pConnectionEvent->eventData.keyExchangeRequestEvent.requestedKeys &
        (uint8_t)gLtk_c) == 0U)
    {
        sentSmpKeys.aLtk = NULL;
        /*
            * When the LTK is NULL EDIV and Rand are not sent
            * and will be ignored.
            */
    }

    if ((pConnectionEvent->eventData.keyExchangeRequestEvent.requestedKeys &
        (uint8_t)gIrk_c) == 0U)
    {
        sentSmpKeys.aIrk = NULL;
        /*
            * When the IRK is NULL the Address and Address Type
            * are not sent and will be ignored.
            */
    }
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
    else
    {
        sentSmpKeys.aIrk = aIrk;
        BleConnManager_GetLocalIrk(sentSmpKeys.aIrk);
    }
#endif
    if ((pConnectionEvent->eventData.keyExchangeRequestEvent.requestedKeys &
        (uint8_t)gCsrk_c) == 0U)
    {
        sentSmpKeys.aCsrk = NULL;
    }
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
    else
    {
        sentSmpKeys.aCsrk = aCsrk;
        BleConnManager_GetLocalCsrk(sentSmpKeys.aCsrk);
    }
#endif
    (void)Gap_SendSmpKeys(peerDeviceId, &sentSmpKeys);
}

/*! *********************************************************************************
*\private
*\fn           void BleConnManager_GapPeripheralEventEvtPairingComplete(
*                  deviceId_t            peerDeviceId,
*                  gapConnectionEvent_t* pConnectionEvent)
*\brief        Handles gConnEvtPairingComplete_c event from BleConnManager_GapPeripheralEvent.
*              This function is created so BleConnManager_GapPeripheralEvent can
*              respect the under 20 CCM metric. 
*
*\param  [in]  peerDeviceId        The GAP peer Id.
*
*\param  [in]  pConnectionEvent    GAP Connection event from the Host Stack.
*
*\retval       void.
********************************************************************************** */
STATIC void BleConnManager_GapPeripheralEventEvtPairingComplete
(
    deviceId_t            peerDeviceId,
    gapConnectionEvent_t* pConnectionEvent
)
{
#if (defined(gRepeatedAttempts_d) && (gRepeatedAttempts_d == 1U))
    RepeatedAttempts_LogAttempt(&pConnectionEvent->eventData.pairingCompleteEvent,
                                maPeerDeviceOriginalAddress);
#endif /* gRepeatedAttempts_d */
#if gAppUseBonding_d
    if (pConnectionEvent->eventData.pairingCompleteEvent.pairingSuccessful &&
        pConnectionEvent->eventData.pairingCompleteEvent.pairingCompleteData.withBonding)
    {
        /* If a bond is created, write device address in controller's Filter Accept List */
        (void)Gap_AddDeviceToFilterAcceptList(mPeerDeviceAddressType, maPeerDeviceAddress);
#if gAppUsePrivacy_d
        (void)BleConnManager_ManagePrivacyInternal(TRUE);
#endif /* gAppUsePrivacy_d */
    }
#endif /* gAppUseBonding_d */

    if (gPairingParameters.leSecureConnectionSupported)
    {
        pConnectionEvent->eventData.pairingCompleteEvent.pairingSuccessful ?
            mSuccessfulPairings++ : mFailedPairings++;

        /* Apply recommendations to change key pair after a number of attempts */
        if (((mFailedPairings * gConnPairFailToSucessCount_c) + mSuccessfulPairings) >
            gConnPairChangeKeyThreshold_d)
        {
            (void)Gap_LeScRegeneratePublicKey();
        }
    }
}
#endif /* gAppUsePairing_d */

/*! *********************************************************************************
*\fn           void BleConnManager_GapCentralEvent(
*                  deviceId_t            peerDeviceId,
*                  gapConnectionEvent_t* pConnectionEvent)
*\brief        Performs GAP Central operations on the BLE stack on the
*              connection callback.
*
*\param  [in]  peerDeviceId        The GAP peer Id.
*
*\param  [in]  pConnectionEvent    GAP Connection event from the Host Stack.
*
*\retval       void.
********************************************************************************** */
void BleConnManager_GapCentralEvent
(
    deviceId_t            peerDeviceId,
    gapConnectionEvent_t* pConnectionEvent
)
{
    switch (pConnectionEvent->eventType)
    {
        case gConnEvtConnected_c:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
#if (defined(gRepeatedAttempts_d) && (gRepeatedAttempts_d == 1U))
            FLib_MemCpy(maPeerDeviceOriginalAddress,
                        pConnectionEvent->eventData.connectedEvent.peerAddress,
                        sizeof(bleDeviceAddress_t));
#endif /* gRepeatedAttempts_d */
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
            /* Copy peer device address information */
            mPeerDeviceAddressType =
                    pConnectionEvent->eventData.connectedEvent.peerAddressType;
            FLib_MemCpy(maPeerDeviceAddress,
                        pConnectionEvent->eventData.connectedEvent.peerAddress,
                        sizeof(bleDeviceAddress_t));
#endif /* gAppUseBonding_d */
#endif /* gAppUsePairing_d */
#if gConnUpdateAlwaysAccept_d
            (void)Gap_EnableUpdateConnectionParameters(peerDeviceId, TRUE);
#endif /* gConnUpdateAlwaysAccept_d */
            /* Initiate Data Length Update Procedure */
            BleConnManager_DataLengthUpdateProcedure(peerDeviceId);
#if gConnInitiatePhyUpdateRequest_c
            if ((mSupportedFeatures & ((leSupportedFeatures_t)gLe2MbPhy_c |
                (leSupportedFeatures_t)gLeCodedPhy_c)) != 0U)
            {
                (void)Gap_LeSetPhy(FALSE,
                                   peerDeviceId,
                                   0,
                                   gConnPhyUpdateReqTxPhySettings_c,
                                   gConnPhyUpdateReqRxPhySettings_c,
                                   (uint16_t)gConnPhyUpdateReqPhyOptions_c);
            }
#endif /* gConnInitiatePhyUpdateRequest_c */
        }
        break;

        case gConnEvtParameterUpdateRequest_c:
        {
#if !gConnUpdateAlwaysAccept_d
            gapConnParamsUpdateReq_t *pUpdateReq =
                         &pConnectionEvent->eventData.connectionUpdateRequest;
            bool_t result;

            /* Check values match the configured intervals */
            result = (pUpdateReq->intervalMin > gConnUpdateIntervalMin_d) &&
                     (pUpdateReq->intervalMax < gConnUpdateIntervalMax_d) &&
                     (pUpdateReq->peripheralLatency > gConnUpdateLatencyMin_d) &&
                     (pUpdateReq->peripheralLatency < gConnUpdateLatencyMax_d) &&
                     (pUpdateReq->timeoutMultiplier > gConnUpdateSuperTimeoutMin_d) &&
                     (pUpdateReq->timeoutMultiplier < gConnUpdateSuperTimeoutMax_d);

            (void)Gap_EnableUpdateConnectionParameters(peerDeviceId, result);
#else  /* gConnUpdateAlwaysAccept_d */
            (void)Gap_EnableUpdateConnectionParameters(peerDeviceId, TRUE);
#endif /* gConnUpdateAlwaysAccept_d */
        }
        break;

        case gConnEvtPeripheralSecurityRequest_c:
        {
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
            bool_t isBonded = FALSE;
            uint8_t nvmIndex = gInvalidNvmIndex_c;

            if ((gBleSuccess_c == Gap_CheckIfBonded(peerDeviceId, &isBonded, &nvmIndex)) &&
                (isBonded))
            {
                (void)Gap_EncryptLink(peerDeviceId);
            }
            else
            {
#if (defined(gRepeatedAttempts_d) && (gRepeatedAttempts_d == 1U))
                if (RepeatedAttempts_CheckRequest(maPeerDeviceOriginalAddress) == TRUE)
                {
                    (void)Gap_Pair(peerDeviceId, &gPairingParameters);
                }
                else
                {
                    (void)Gap_RejectPairing(peerDeviceId,
                                            gRepeatedAttempts_c);
                }
#else  /* gRepeatedAttempts_d */
                (void)Gap_Pair(peerDeviceId, &gPairingParameters);
#endif /* gRepeatedAttempts_d */
            }
#else  /* gAppUsePairing_d */
            (void)Gap_RejectPairing(peerDeviceId,
                                    gPairingNotSupported_c);
#endif /* gAppUsePairing_d */
        }
        break;

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
#if defined(gPasskeyValue_c)
        case gConnEvtPasskeyRequest_c:
            (void)Gap_EnterPasskey(peerDeviceId, gPasskeyValue_c);
            break;
#endif /* gPasskeyValue_c */

        case gConnEvtPasskeyDisplay_c:
        {
            ; /* Display on a screen or simply ignore */
        }
        break;

        case gConnEvtLeScDisplayNumericValue_c:
        {
            (void)pConnectionEvent->eventData.numericValueForDisplay;
            /*
             * Display on a screen for user confirmation then
             * validate/invalidate based on value.
             */
            (void)Gap_LeScValidateNumericValue(peerDeviceId, TRUE);
        }
        break;

        case gConnEvtKeyExchangeRequest_c:
        {
            BleConnManager_GapCentralEventEvtKeyExchangeRequest(peerDeviceId, pConnectionEvent);
        }
        break;

        case gConnEvtKeysReceived_c:
        {
            /* Copy peer device address information when IRK is used */
            if (pConnectionEvent->eventData.keysReceivedEvent.pKeys->aIrk != NULL)
            {
#if gAppUseBonding_d
                mPeerDeviceAddressType =
                         pConnectionEvent->eventData.keysReceivedEvent.pKeys->addressType;
#endif /* gAppUseBonding_d */
                FLib_MemCpy(maPeerDeviceAddress,
                            pConnectionEvent->eventData.keysReceivedEvent.pKeys->aAddress,
                            sizeof(bleDeviceAddress_t));
            }
        }
        break;

        case gConnEvtEncryptionChanged_c:
        {
            ; /* Display on a screen or simply ignore */
        }
        break;

        case gConnEvtPairingComplete_c:
        {
#if (defined(gRepeatedAttempts_d) && (gRepeatedAttempts_d == 1U))
            RepeatedAttempts_LogAttempt(&pConnectionEvent->eventData.pairingCompleteEvent,
                                        maPeerDeviceOriginalAddress);
#endif /* gRepeatedAttempts_d */
#if gAppUseBonding_d
            if (pConnectionEvent->eventData.pairingCompleteEvent.pairingSuccessful &&
                pConnectionEvent->eventData.pairingCompleteEvent.pairingCompleteData.withBonding)
            {
                /*
                * If a bond is created, write device address in
                 * controller's Filter Accept List
                 */
                (void)Gap_AddDeviceToFilterAcceptList(mPeerDeviceAddressType,
                                               maPeerDeviceAddress);
#if gAppUsePrivacy_d
                (void)BleConnManager_ManagePrivacyInternal(TRUE);
#endif /* gAppUsePrivacy_d */
            }

#endif /* gAppUseBonding_d */

            if (gPairingParameters.leSecureConnectionSupported)
            {
                pConnectionEvent->eventData.pairingCompleteEvent.pairingSuccessful ?
                    mSuccessfulPairings++ : mFailedPairings++;

                /* Apply recommendations to change key pair after a number of attempts */
                if ((mSuccessfulPairings + (gConnPairFailToSucessCount_c * mFailedPairings)) >
                    gConnPairChangeKeyThreshold_d)
                {
                    (void)Gap_LeScRegeneratePublicKey();
                }
            }
        }
        break;
#endif /* gAppUsePairing_d */

        default:
        {
            ; /* No action required */
        }
        break;
    }
}

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
/*! *********************************************************************************
*\private
*\fn           void BleConnManager_GapCentralEventEvtKeyExchangeRequest(
*                  deviceId_t            peerDeviceId,
*                  gapConnectionEvent_t* pConnectionEvent)
*\brief        Handles gConnEvtKeyExchangeRequest_c event from BleConnManager_GapCentralEvent.
*              This function is created so BleConnManager_GapCentralEvent can
*              respect the under 20 CCM metric. 
*
*\param  [in]  peerDeviceId        The GAP peer Id.
*
*\param  [in]  pConnectionEvent    GAP Connection event from the Host Stack.
*
*\retval       void.
********************************************************************************** */
STATIC void BleConnManager_GapCentralEventEvtKeyExchangeRequest
(
    deviceId_t            peerDeviceId,
    gapConnectionEvent_t* pConnectionEvent
)
{
    gapSmpKeys_t sentSmpKeys = gSmpKeys;
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))            
    uint8_t aIrk[gcSmpIrkSize_c];
    uint8_t aCsrk[gcSmpCsrkSize_c];
#endif
    if ((pConnectionEvent->eventData.keyExchangeRequestEvent.requestedKeys &
        (uint8_t)gLtk_c) == 0U)
    {
        sentSmpKeys.aLtk = NULL;
        /*
            * When the LTK is NULL EDIV and Rand are not sent and will be
            * ignored.
            */
    }

    if ((pConnectionEvent->eventData.keyExchangeRequestEvent.requestedKeys &
        (uint8_t)gIrk_c) == 0U)
    {
        sentSmpKeys.aIrk = NULL;
        /*
            * When the IRK is NULL the Address and Address Type are not sent
            * and will be ignored.
            */
    }
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
    else
    {
        sentSmpKeys.aIrk = aIrk;
        BleConnManager_GetLocalIrk(sentSmpKeys.aIrk);
    }
#endif
    if ((pConnectionEvent->eventData.keyExchangeRequestEvent.requestedKeys &
        (uint8_t)gCsrk_c) == 0U)
    {
        sentSmpKeys.aCsrk = NULL;
    }
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
    else
    {
        sentSmpKeys.aCsrk = aCsrk;
        BleConnManager_GetLocalCsrk(sentSmpKeys.aCsrk);
    }
#endif
    (void)Gap_SendSmpKeys(peerDeviceId, &sentSmpKeys);
}
#endif /* gAppUsePairing_d */

/*! *********************************************************************************
*\fn           bleResult_t BleConnManager_EnablePrivacy(void)
*\brief        Checks bonding devices and enables controller/host privacy.
*
*\param  [in]  none.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************** */
bleResult_t BleConnManager_EnablePrivacy(void)
{
#if (defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 1U)) && \
    (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
    bleResult_t result = gBleSuccess_c;

    if( FALSE == mbPrivacyEnabled )
    {
        result = BleConnManager_ManagePrivacyInternal(FALSE);
    }

    return result;
#else  /* gAppUsePrivacy_d && gAppUseBonding_d */
    return gBleFeatureNotSupported_c;
#endif /* gAppUsePrivacy_d && gAppUseBonding_d */
}

/*! *********************************************************************************
*\fn           bleResult_t BleConnManager_DisablePrivacy(void)
*\brief        Checks enabled privacy and disables controller/host privacy.
*
*\param  [in]  none.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************** */
bleResult_t BleConnManager_DisablePrivacy(void)
{
#if (defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 1U)) && \
    (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
    bleResult_t result = gBleSuccess_c;

    if( mbPrivacyEnabled )
    {
#if defined(gBleEnableControllerPrivacy_d) && \
    (gBleEnableControllerPrivacy_d > 0)
        if( mcDevicesInResolvingList == 0U )
        {
            result = Gap_EnableHostPrivacy(FALSE, NULL);
        }
        else
        {
            result = Gap_EnableControllerPrivacy(FALSE,
                                                 NULL,
                                                 0,
                                                 NULL);
        }
#else  /* gBleEnableControllerPrivacy_d */
        result = Gap_EnableHostPrivacy(FALSE, NULL);
#endif /* gBleEnableControllerPrivacy_d */
    }

    return result;
#else  /* gAppUsePrivacy_d && gAppUseBonding_d */
    return gBleFeatureNotSupported_c;
#endif /* gAppUsePrivacy_d && gAppUseBonding_d */
}

/*! *********************************************************************************
*\fn           void BleConnManager_GapCommonConfig(void)
*\brief        Performs common GAP configuration.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
void BleConnManager_GapCommonConfig(void)
{
#if defined(gRandomStaticAddress_d) && (gRandomStaticAddress_d > 0)
    /* gaBleDeviceAddress already created in BleConnManager_MCUInfoToRandomStaticAddress - set it */
    gSettingRandomStaticAddress = TRUE;
    (void)Gap_SetRandomAddress(gaBleDeviceAddress);
#else
    /* Read public address from controller - gaBleDeviceAddress will be populated on event */
    (void)Gap_ReadPublicDeviceAddress();
#endif /* gRandomStaticAddress_d */

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
    /* Register security requirements if pairing is used */
    (void)Gap_RegisterDeviceSecurityRequirements(&deviceSecurityRequirements);

#ifdef gPasskeyValue_c
    /*
     * Set local passkey. If not defined, passkey will be generated random in SMP.
     * Do not set local passkey if device IO capabilities are gIoKeyboardOnly_c.
     * It will need to be input during the pairing process.
     */
    if (gPairingParameters.localIoCapabilities != gIoKeyboardOnly_c)
    {
        (void)Gap_SetLocalPasskey(gPasskeyValue_c);
    }
#endif /* gPasskeyValue_c */

#endif /* gAppUsePairing_d */

    /* Populate Filter Accept List if bonding is supported */
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
    gapIdentityInformation_t aIdentity[gMaxBondedDevices_c] = {0U};
    bleResult_t result = Gap_GetBondedDevicesIdentityInformation(aIdentity,
                                                                 gMaxBondedDevices_c,
                                                                 &gcBondedDevices);

    if (gBleSuccess_c == result && gcBondedDevices > 0U)
    {
        for (uint8_t i = 0; i < gcBondedDevices; i++)
        {
            (void)Gap_AddDeviceToFilterAcceptList(aIdentity[i].identityAddress.idAddressType,
                                          aIdentity[i].identityAddress.idAddress);
        }
    }
#endif /* gAppUseBonding_d */

#if (defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 1U))
#if (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))
    (void)BleConnManager_ManagePrivacyInternal(FALSE);
#else /* gAppUseBonding_d */
    /* Use Non-Resolvable Private Addresses */
    (void)Gap_EnableHostPrivacy(TRUE, NULL);
#endif /* gAppUseBonding_d */
#endif /* gAppUsePrivacy_d */
    if ((mSupportedFeatures & ((leSupportedFeatures_t)gLe2MbPhy_c |
        (leSupportedFeatures_t)gLeCodedPhy_c)) != 0U)
    {
        (void)Gap_LeSetPhy(TRUE,
                           0,
                           0,
                           gConnDefaultTxPhySettings_c,
                           gConnDefaultRxPhySettings_c,
                           0);
    }
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
#if defined(gRandomStaticAddress_d) && (gRandomStaticAddress_d > 0)
/*! *********************************************************************************
*\private
*\fn           void BleConnManager_MCUInfoToRandomStaticAddress(void)
*\brief        Generates a Random Static Address from the board UID.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
STATIC void BleConnManager_MCUInfoToRandomStaticAddress(void)
{
    uint8_t uid[16] = {0};
    uint8_t len = 0;
    uint8_t sha256Output[SHA256_HASH_SIZE] = {0};

    /* Take gcBleDeviceAddressSize_c octets from the end of sha256Output
       to avoid collision with other keys generated from this hash */
    PLATFORM_GetMCUUid (uid, &len);
    SHA256_Hash (uid, len, sha256Output);
    FLib_MemCpy(gaBleDeviceAddress, &(sha256Output[SHA256_HASH_SIZE - gcBleDeviceAddressSize_c]), gcBleDeviceAddressSize_c);

    /* Most significant two bits of a Random Static Address must be 11 */
    gaBleDeviceAddress[gcBleDeviceAddressSize_c - 1U] |= (BIT7 | BIT6);

    /* Set the Random Static address into gSmpKeys */
    gSmpKeys.aAddress = gaBleDeviceAddress;
    gSmpKeys.addressType = gBleAddrTypeRandom_c;
}
#endif

#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
#include "app_conn.h"
/*! *********************************************************************************
*\private
*\fn           void BleConnManager_ManageLocalKeys(void)
*\brief        Generates local IRK and CSRK and stores them in NVM.
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
STATIC void BleConnManager_ManageLocalKeys(void)
{
    bleLocalKeysBlob_t localIrk = {0};
    bleLocalKeysBlob_t localCSRK = {0};
    bool_t lkFailure = TRUE;
    (void)App_NvmReadLocalIRK(&localIrk);
    (void)App_NvmReadLocalCSRK(&localCSRK);
    do
    {
        if(localIrk.keyGenerated == FALSE)
        {
            if(gSecSuccess_c != SecLib_GenerateSymmetricKey(gcSmpIrkSize_c, TRUE, localIrk.raw ))
            {
                break;
            }
            localIrk.keyGenerated = TRUE;
            if(gBleSuccess_c != App_NvmWriteLocalIRK(&localIrk))
            {
                break;
            }
        }
        if (gSecLibFunctions.pfSecLib_GenerateBluetoothEIRKBlob != NULL)
        {
            if(gSecSuccess_c != gSecLibFunctions.pfSecLib_GenerateBluetoothEIRKBlob(localIrk.raw , TRUE, TRUE, gSmpKeys.aIrk))
            {
                break;
            }
        }
        if(localCSRK.keyGenerated == FALSE)
        {
            if(gSecSuccess_c != SecLib_GenerateSymmetricKey(gcSmpCsrkSize_c, TRUE, localCSRK.raw ))
            {
                break;
            }
            localCSRK.keyGenerated = TRUE;
            if(gBleSuccess_c != App_NvmWriteLocalCSRK(&localCSRK))
            {
                break;
            }
        }
        lkFailure = FALSE;
#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
        gPairingParameters.securityModeAndLevel = gSecurityMode_1_Level_4_c;
        gPairingParameters.leSecureConnectionSupported = TRUE;
#endif /* gAppUsePairing_d */
    }
    while(FALSE);
    if(lkFailure)
    {
        panic(0,0,0,0);  
    }           
}

#if (defined(gAppUsePairing_d) && (gAppUsePairing_d == 1U))
/*! *********************************************************************************
*\private
*\fn           void BleConnManager_GetLocalKey(void)
*\brief        Reads local key blob from NVM and outputs the key in plain text.
*
*\param  [in]  id   - local key Entry Index 0/1 IRK/CSRK.
*
*\param  [out] pOut - the address of the buffer where the plain text key will de written.
*
*\retval       void.
********************************************************************************** */
STATIC void BleConnManager_GetLocalKey(uint8_t id, uint8_t* pOut)
{
    bleLocalKeysBlob_t localKey = {0};
    (void)App_NvmReadLocalKeys(id , &localKey);
    if (gSecLibFunctions.pfSecLib_DeobfuscateKey != NULL)
    {
        if(gSecSuccess_c !=  gSecLibFunctions.pfSecLib_DeobfuscateKey(localKey.pKey, pOut))
        {
            panic(0,0,0,0);
        }
    }
}
#endif /* gAppUsePairing_d */

#else /* (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U)) */
/*! *********************************************************************************
*\private
*\fn           void BleConnManager_MCUInfoToSmpKeys(void)
*\brief        Generates LTK, IRK, CSRK, ediv and rand. Optionally: identity address
*              (Random Static).
*
*\param  [in]  none.
*
*\retval       void.
********************************************************************************** */
STATIC void BleConnManager_MCUInfoToSmpKeys(void)
{
    uint8_t uid[16] = {0};
    uint8_t len = 0;
    uint8_t sha256Output[SHA256_HASH_SIZE] = {0};

    PLATFORM_GetMCUUid (uid, &len);

    if((len > 0U) && (len <= 16U))
    {
        /* generate LTK. LTK size always smaller than SHA1 hash size */
        uid[len - 1U]++;
        SHA256_Hash (uid, len, sha256Output);
        FLib_MemCpy (gSmpKeys.aLtk, sha256Output, gSmpKeys.cLtkSize);

#if !defined(gUseCustomIRK_d)
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
#endif /* (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U)) */
/*! *********************************************************************************
*\private
*\fn           void BleConnManager_DataLengthUpdateProcedure(deviceId_t peerDeviceId)
*\brief        Initiates Data Length Update Procedure.
*
*\param  [in]  peerDeviceId    The GAP peer Id.
*
*\retval       void.
********************************************************************************** */
STATIC void BleConnManager_DataLengthUpdateProcedure(deviceId_t peerDeviceId)
{
    if ((mSupportedFeatures &
        (leSupportedFeatures_t)gLeDataPacketLengthExtension_c) != 0U)
    {
        /* Check for Coded PHY support */
        bool_t codedPhySupported = (mSupportedFeatures & (leSupportedFeatures_t)gLeCodedPhy_c) != 0U;

        (void)Gap_UpdateLeDataLength(peerDeviceId, gBleMaxTxOctets_c,
                                     codedPhySupported ?
                                     gBleMaxTxTimeCodedPhy_c : gBleMaxTxTime_c);
    }
}

#if (defined(gAppUsePrivacy_d) && (gAppUsePrivacy_d == 1U)) && \
    (defined(gAppUseBonding_d) && (gAppUseBonding_d == 1U))

/*! *********************************************************************************
*\private
*\fn           bleResult_t BleConnManager_ManagePrivacyInternal(bool_t bCheckNewBond)
*\brief        Manage privacy for host or controller based on the available bonding
*              information.
*
*\param  [in]  bCheckNewBond     Verify that the latest bonded device is new and
*                                requires a privacy list update.
*
*\return       bleResult_t       Result of the operation.
********************************************************************************** */
STATIC bleResult_t BleConnManager_ManagePrivacyInternal(bool_t bCheckNewBond)
{
    gapIdentityInformation_t *pOutIdentityAddresses = NULL;
    uint8_t                  identitiesCount = 0;
    bleResult_t              result = gBleSuccess_c;

    pOutIdentityAddresses =
          (gapIdentityInformation_t *)MEM_BufferAlloc(
                            (uint32_t)gMaxResolvingListSize_c *
                            sizeof(gapIdentityInformation_t));

    if( NULL != pOutIdentityAddresses )
    {
        uint8_t* pLocalIrk; 
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))    
        bleLocalKeysBlob_t localIrk = {0};
        (void)App_NvmReadLocalIRK(&localIrk);
        pLocalIrk = localIrk.pKey;
#else
        pLocalIrk = gSmpKeys.aIrk;
#endif        
        result = Gap_GetBondedDevicesIdentityInformation(pOutIdentityAddresses,
                                                         gMaxResolvingListSize_c,
                                                         &identitiesCount);

        if((gBleSuccess_c == result) && (identitiesCount > 0U ))
        {
            if ((identitiesCount == mcDevicesInResolvingList + 1U ) ||
                (bCheckNewBond == FALSE))
            {
                (void)BleConnManager_DisablePrivacy();

                mcDevicesInResolvingList = identitiesCount;

                if( mcDevicesInResolvingList > (uint8_t)gMaxResolvingListSize_c )
                {
                    mcDevicesInResolvingList = gMaxResolvingListSize_c;
                }

                /* If Controller Privacy is enabled, the Host requires an RPA/NRPA
                   in order for a central device to be able to maintain privacy
                   when connecting with a new unbonded peripheral - if no RPA/NRPA
                   has been set - create and set it now */
                if (FALSE == mHaveRandomAddress)
                {
                    mSettingRPAFromApplication = TRUE;
                    (void)Gap_CreateRandomDeviceAddress(pLocalIrk, NULL);
                }
#if defined(gBleEnableControllerPrivacy_d) && (gBleEnableControllerPrivacy_d > 0)
                result = Gap_EnableControllerPrivacy(TRUE,
                                                     gSmpKeys.aIrk,
                                                     mcDevicesInResolvingList,
                                                     pOutIdentityAddresses);
#else  /* gBleEnableControllerPrivacy_d */
                result = Gap_EnableHostPrivacy(TRUE, pLocalIrk);
#endif /* gBleEnableControllerPrivacy_d */
            }
            else
            {
                /* This is not a new bond, leave privacy as is */
            }
        }
        else
        {
            if(mbPrivacyEnabled == FALSE)
            {
                /* No bonds with valid IRK were found, reset the number of the devices from the Resolving List */
                mcDevicesInResolvingList = 0U;
                result = Gap_EnableHostPrivacy(TRUE, pLocalIrk);
            }
        }
        (void)MEM_BufferFree(pOutIdentityAddresses);
    }
    else
    {
        result = gBleOutOfMemory_c;
    }

    return result;
}
#endif /* (gAppUsePrivacy_d) && (gAppUseBonding_d) */

#if (defined(gRepeatedAttempts_d) && (gRepeatedAttempts_d == 1U))

/*! *********************************************************************************
*\private
*\fn           bool_t RepeatedAttempts_CheckRequest(bleDeviceAddress_t address)
*\brief        Check if a pairing/peripheral security request should be accepted.
*
*\param  [in]  address    The address of the peer.
*
*\retval       TRUE       If ok.
*\retval       FALSE      If this is a repeated attempt and timeout has not expired.
********************************************************************************** */
STATIC bool_t RepeatedAttempts_CheckRequest(bleDeviceAddress_t address)
{
    bool_t okToProceed = TRUE;
    uint8_t iCount;

    /*
     * Check if this device has been involved in a failed pairing and is
     * still in the waiting period
     */
    for (iCount = 0; iCount < (uint8_t)gRepeatedAttemptsNoOfDevices_c; iCount++)
    {
        if (FLib_MemCmp(maPairingPeers[iCount].address,
                        address,
                        gcBleDeviceAddressSize_c))
        {
            if (maPairingPeers[iCount].remainingTime != 0U)
            {
                okToProceed = FALSE;
                break;
            }
        }
    }

    return okToProceed;
}

/*! *********************************************************************************
*\private
*\fn           void RepeatedAttempts_LogAttempt(gapPairingCompleteEvent_t *pEvent,
*                                               bleDeviceAddress_t        address)
*\brief        Track a pairing attempt.
*
*\param  [in]  pEvent     Pointer to the pairing complete event.
*\param  [in]  address    Address of the peer.
*
*\retval       void.
********************************************************************************** */
STATIC void RepeatedAttempts_LogAttempt
(
    gapPairingCompleteEvent_t *pEvent,
    bleDeviceAddress_t        address
)
{
    bool_t found = FALSE;
    uint8_t iCount;
    uint8_t emptySlotIndex = 0xFF;
    uint8_t indexToReplace = 0;
    uint16_t maxTimeout = 0;
    
    /* Stop timer if it is running */
    if (TM_IsTimerActive((timer_handle_t)mRepeatedAttemptsTimerId))
    {
      (void)TM_Stop((timer_handle_t)mRepeatedAttemptsTimerId);
      RepeatedAttempts_UpdateTimePassed(mMinTimeToWait - TM_GetRemainingTime((timer_handle_t)mRepeatedAttemptsTimerId)/1000U);
    }

    /* Check if this peer was in the repeated attempts array */
    for (iCount = 0; iCount < (uint8_t)gRepeatedAttemptsNoOfDevices_c; iCount++)
    {
        if (FLib_MemCmp(maPairingPeers[iCount].address,
                        address,
                        gcBleDeviceAddressSize_c))
        {
            found = TRUE;
            if (pEvent->pairingSuccessful == TRUE)
            {
                /* On successful pairing remove device from array */
                FLib_MemSet(&maPairingPeers[iCount],
                            0x00U,
                            sizeof(repeatedAttemptsDevice_t));
            }
            else
            {
                /* On failed pairing, double waiting period */
                maPairingPeers[iCount].baseTimeout = FLib_GetMin(
                                 2U * maPairingPeers[iCount].baseTimeout,
                                 gRepeatedAttemptsTimeoutMax_c);
                maPairingPeers[iCount].remainingTime =
                                 maPairingPeers[iCount].baseTimeout;
            }
            break;
        }

        /* Memorize first empty slot if this peer is a new device which must be added */
        if ((maPairingPeers[iCount].baseTimeout == 0U) &&
            (emptySlotIndex == 0xFF))
        {
            emptySlotIndex = iCount;
        }

        /*
         * Memorize the longest waiting device in the array if this peer
         * is a new device which must be added and the array is full
         */
        if (maPairingPeers[iCount].remainingTime > maxTimeout)
        {
            maxTimeout = maPairingPeers[iCount].remainingTime;
            indexToReplace = iCount;
        }
    }

    /* If this is a new peer add it to the array - replace oldest entry if full */
    if ((found == FALSE) && (pEvent->pairingSuccessful == FALSE))
    {
        uint8_t index;

        if (emptySlotIndex != 0xFF)
        {
            index = emptySlotIndex;
        }
        else
        {
            index = indexToReplace;
        }

        FLib_MemCpy(maPairingPeers[index].address, address, gcBleDeviceAddressSize_c);
        maPairingPeers[index].baseTimeout   = gRepeatedAttemptsTimeoutMin_c;
        maPairingPeers[index].remainingTime = gRepeatedAttemptsTimeoutMin_c;
    }

    /* Restart timer */
    mMinTimeToWait = RepeatedAttempts_GetMinTimeToWait();

    if (mMinTimeToWait < (gRepeatedAttemptsTimeoutMax_c + 1U))
    {
        (void)TM_InstallCallback((timer_handle_t)mRepeatedAttemptsTimerId, RepeatedAttempts_TimerCb, NULL);
        (void)TM_Start((timer_handle_t)mRepeatedAttemptsTimerId, (uint8_t)kTimerModeSingleShot | (uint8_t)kTimerModeLowPowerTimer, TmSecondsToMilliseconds(mMinTimeToWait));
    }
}

/*! *********************************************************************************
*\private
*\fn           void RepeatedAttempts_UpdateTimePassed(uint16_t seconds)
*\brief        Update array waiting times after stopping timer.
*
*\param  [in]  seconds    Seconds to subtract from waiting times.
*
*\retval       void.
********************************************************************************** */
STATIC void RepeatedAttempts_UpdateTimePassed(uint16_t seconds)
{
    uint8_t iCount;

    for (iCount = 0; iCount < (uint8_t)gRepeatedAttemptsNoOfDevices_c; iCount++)
    {
        if (maPairingPeers[iCount].remainingTime > seconds)
        {
            maPairingPeers[iCount].remainingTime -= seconds;
        }
        else
        {
            maPairingPeers[iCount].remainingTime = 0;
        }
    }
}

/*! *********************************************************************************
*\private
*\fn           uint16_t RepeatedAttempts_GetMinTimeToWait(void)
*\brief        Return minimum waiting time from array.
*
*\param  [in]  none.
*
*\return       uint16_t    Minimum remaining wait time in the array.
********************************************************************************** */
STATIC uint16_t RepeatedAttempts_GetMinTimeToWait(void)
{
    uint8_t iCount;
    uint16_t minRemainingTime = gRepeatedAttemptsTimeoutMax_c + 1U;

    for (iCount = 0; iCount < (uint8_t)gRepeatedAttemptsNoOfDevices_c; iCount++)
    {
        if ((maPairingPeers[iCount].remainingTime != 0) &&
            (maPairingPeers[iCount].remainingTime < minRemainingTime))
        {
            minRemainingTime = maPairingPeers[iCount].remainingTime;
        }
    }

    return minRemainingTime;
}

/*! *********************************************************************************
*\private
*\fn           void RepeatedAttempts_TimerCb(void *param)
*\brief        Handles advertising timer callback.
*
*\param  [in]  pParam        Callback parameters.
*
*\retval       void.
********************************************************************************** */
STATIC void RepeatedAttempts_TimerCb(void *param)
{
    RepeatedAttempts_UpdateTimePassed(mMinTimeToWait);

    mMinTimeToWait = RepeatedAttempts_GetMinTimeToWait();

    if (mMinTimeToWait < (gRepeatedAttemptsTimeoutMax_c + 1U))
    {
        (void)TM_InstallCallback((timer_handle_t)mRepeatedAttemptsTimerId, RepeatedAttempts_TimerCb, NULL);
        (void)TM_Start((timer_handle_t)mRepeatedAttemptsTimerId, (uint8_t)kTimerModeSingleShot | (uint8_t)kTimerModeLowPowerTimer, TmSecondsToMilliseconds(mMinTimeToWait));
    }
    else
    {
        (void)TM_Stop((timer_handle_t)mRepeatedAttemptsTimerId);
    }
}
#endif /* gRepeatedAttempts_d */

/*! *********************************************************************************
* @}
********************************************************************************** */
