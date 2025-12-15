/*! *********************************************************************************
 * \defgroup BLE BLE General Definitions
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2014 Freescale Semiconductor, Inc.
* Copyright 2016-2025 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef BLE_GENERAL_H
#define BLE_GENERAL_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "FunctionLib.h"
#include "ble_constants.h"
#include "ble_sig_defines.h"
#include "ble_utils.h"


/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/
/* Maximum number of Advertising sets handled by the Host tables. */
#define gMaxAdvSets_c                               4U

#define gInvalidDeviceId_c                          (0xFFU)
#define gInvalidNvmIndex_c                          (0xFFU)

/*! Boundary values for the Connection Parameters (Standard GAP). */
#define gcConnectionIntervalMin_c                   (0x0006U)
#define gcConnectionIntervalMax_c                   (0x0C80U)
#define gcConnectionPeripheralLatencyMax_c               (0x01F3U)
#define gcConnectionSupervisionTimeoutMin_c         (0x000AU)
#define gcConnectionSupervisionTimeoutMax_c         (0x0C80U)

#define gcConnectionSubrateFactorMin_c              (0x0001U)
#define gcConnectionSubrateFactorMax_c              (0x01F4U)
#define gcConnectionSubrateContinuationNumberMax_c  (0x01F3U)
/*! Default values for the Connection Parameters (Preferred). */

/*! connIntervalmin = Conn_Interval_Min * 1.25 ms */
/*! Value of 0xFFFF indicates no specific minimum. */
#ifndef gcConnectionIntervalMinDefault_c
#define gcConnectionIntervalMinDefault_c            (40U)
#endif

/*! connIntervalmax = Conn_Interval_Max * 1.25 ms */
/*! Value of 0xFFFF indicates no specific maximum. */
#ifndef gcConnectionIntervalMaxDefault_c
#define gcConnectionIntervalMaxDefault_c            (160U)
#endif

#ifndef gcConnectionPeripheralLatencyDefault_c
#define gcConnectionPeripheralLatencyDefault_c           (0U)
#endif

/*! Time = N * 10 ms */
#ifndef gcConnectionSupervisionTimeoutDefault_c
#define gcConnectionSupervisionTimeoutDefault_c     (2000U)
#endif

/*! When unit testing is performed, access from unit test module to static functions/variables
 * within the tested module is not possible and therefore the static storage class identifier
 * shall be removed */
#ifdef gUnitTesting_d
#define STATIC
#else
#define STATIC static
#endif

/*
 *
 * Device addresses may be:
 *
 * -> public
 * -> random -> static
 *           -> private -> resolvable
 *                      -> non-resolvable
 *
 */

/*! Bluetooth Device Address Types */
#define gBleAddrTypePublic_c    (0x00U) /*!< Public Device Address - fixed into the Controller by the manufacturer. */
#define gBleAddrTypeRandom_c    (0x01U) /*!< Random Device Address - set by the Host into the Controller for privacy reasons. */

/*! PRA condition: check the 6th byte - MSB should be 0; 2nd MSB should be 1. */
#define Ble_IsPrivateResolvableDeviceAddress(bleAddress)  \
    ( ((bleAddress[5]) & (BIT7|BIT6)) == BIT6 )

/*! PNRA condition: check the 6th byte - MSB should be 0; 2nd MSB should be 0. */
#define Ble_IsPrivateNonresolvableDeviceAddress(bleAddress)  \
    ( ((bleAddress[5]) & (BIT7|BIT6)) == 0U )

/*! RSA condition: check the 6th byte - MSB should be 1; 2nd MSB should be 1. */
#define Ble_IsRandomStaticDeviceAddress(bleAddress)  \
    ( ((bleAddress[5]) & (BIT7|BIT6)) == (BIT7|BIT6) )

/*! A macro used to compare two device addresses */
#define Ble_DeviceAddressesMatch(bleAddress1, bleAddress2) \
        FLib_MemCmp(bleAddress1, bleAddress2, gcBleDeviceAddressSize_c)

/*! A macro used to copy device addresses */
#define Ble_CopyDeviceAddress(destinationAddress, sourceAddress) \
        FLib_MemCpy(destinationAddress, sourceAddress, gcBleDeviceAddressSize_c);

#define gBleUuidType16_c                    0x01U        /*!< 16-bit standard UUID */
#define gBleUuidType128_c                   0x02U        /*!< 128-bit long/custom UUID */
#define gBleUuidType32_c                    0x03U        /*!< 32-bit UUID - not available as ATT UUID format */

#define gLePhy1MFlag_c                      BIT0         /*!< Host prefers to use LE 1M Tx/Rx Phy, possibly among others */
#define gLePhy2MFlag_c                      BIT1         /*!< Host prefers to use LE 2M Tx/Rx Phy, possibly among others */
#define gLePhyCodedFlag_c                   BIT2         /*!< Host prefers to use LE Coded Tx/Rx Phy, possibly among others */

#define gAdvDataChange_c                    BIT0         /*!< Advertising Data Change reason */
#define gScanRspDataChange_c                BIT1         /*!< Scan Response Data Change reason */

/*! Initiator filter policy values */
#define gUseDeviceAddress_c                 0x00U        /*!< Initiates a connection with a specific device identified by its address. */
#define gUseFilterAcceptList_c                     0x01U        /*!< Initiates connections with all the devices in the Filter Accept List at the same time. */

/*! Initiator filter policy values extended for gBLE60_DecisionBasedAdvertisingFilteringSupport_d == TRUE */
#define gUseDeviceAddressNoDecisionPDUs_c                 0x00U        /*!< Filter Accept List is not used to determine which advertiser to connect to. Decision PDUs shall be ignored. Peer_Address_Type and Peer_Address shall be used. */
#define gUseFilterAcceptListNoDecisionPDUs_c              0x01U        /*!< Filter Accept List is used to determine which advertiser to connect to. Decision PDUs shall be ignored. Peer_Address_Type and Peer_Address shall be ignored. */
#define gUseOnlyDecisionPDUs_c                            0x02U        /*!< Filter Accept List is not used to determine which advertiser to connect to. Only Decision PDUs shall be processed. Peer_Address_Type and Peer_Address shall be ignored. */
#define gUseFilterAcceptListAllPDUs_c                     0x03U        /*!< Filter Accept List is used to determine which advertiser to connect to. All PDUs shall be processed. Peer_Address_Type and Peer_Address shall be ignored. */
#define gUseDecisionPDU_UseFilterAcceptListForOtherPDUs_c 0x04U        /*!< All decision PDUs shall be processed. Filter Accept List is used to determine which other PDUs to process. Peer_Address_Type and Peer_Address shall be ignored. */

/*! Basic Scanning filter policy values. Kept for apps backward compatibility */
#define gScanAll_c                          0x00U             /*!< Basic unfiltered scanning filter policy. */
#define gScanWithFilterAcceptList_c         0x01U             /*!< Basic filtered scanning filter policy (using the Filter Accept List).*/

/*! Scanning filter policy values.*/
#define gBasicUnfilteredScan_c           0x00U             /*!< Basic unfiltered scanning filter policy. */
#define gBasicFilteredScan_c             0x01U             /*!< Basic filtered scanning filter policy (using the Filter Accept List).*/
#define gExtendedUnfilteredScan_c        BIT1              /*!< Extended unfiltered scanning filter policy. */
#define gExtendedFilteredScan_c          (BIT1 | BIT0)     /*!< Extended filtered scanning filter policy(using the Filter Accept List). */

/*! PDU Scanning filter policy values. To be used in bitwise OR operations with scanning filter policy above only with gBLE60_DecisionBasedAdvertisingFilteringSupport_d == TRUE
    e.g. ((uint8_t)(gExtendedUnfilteredScan_c | gScanAllPDUs_c)) */
#define gScanOnlyNonDecisionPDUs_c       0x00U             /*!< Scan Only Non Decision Advertising PDU. */
#define gScanAllPDUs_c                   BIT2              /*!< Scan Decision and Non Decision Advertising PDU. */
#define gScanOnlyDecisionPDUs_c          (BIT2 | BIT3)     /*!< Scan Only Decision Advertising PDU. */

/*! Privacy mode values */
#define gNetworkPrivacy_c                   0x00U       /*!< Use Network Privacy Mode for the peer device (default) */
#define gDevicePrivacy_c                    0x01U       /*!< Use Device Privacy Mode for the peer device */

/*! bearer id 0 is used for the Unenhanced ATT bearer */
#define gUnenhancedBearerId_c               0U

#define gDHKeySize_c                        (32U)       /*!< LE Secure Connections 256 bit DHKey */

#define gSkdSize_c                          (16U)       /*!< LL Session Key Diversfier size */

#define gHCICSChannelMapSize                (10U)      /*!< CS Channel Map size */

#define gCSReflectorTableSize_c             (72U)      /*!< CS reflector table size size */

#define gCSNumPhysMax_c                     (2U)       /*!< CS reflector table size size */

#define gCSNumToneAntennaIds_c              (4U)       /*!< CS number of antenna IDs used in the pattern */

#define gCSTestMaxChannelLength_c           (0x6EU)    /*!< CS number of channels used in the the pattern */

#define gDRBGKeySize_c                      (16U)      /*!< CS debugger key size */

#define gCSSyncRandomSize_c                 (16U)      /*!< CS Sync Random size */

#define gCSMaxSubeventLen_c                 (3U)       /*!< CS maximum duration for each CS sub-event in microseconds */

#define gCsAccessAdressSize_c               (4U)       /*!< CS Access Address used in SYNC packets */

/* Controller values for the PHYs to be used by Handover Time Sync */
#define LL_PHY_1M                       (0U)
#define LL_PHY_2M                       (1U)
#define LL_PHY_S8                       (2U)
#define LL_PHY_S2                       (3U)

#define gVendorHandoverMaxCsLlContextSize_c  (224U)   /*!< Maximum size of the LL Context for Handover (CS context is largest) */

#define gVendorUnitaryTestSize_c             (255U)   /*!< Maximum size of the VENDOR_UNITARY_TEST response */

#define gVendorDebugEventSize_c              (255U)   /*!< Maximum size of the debug payload */
#define gLeExtendedFeaturesSize_c            (1U)     /*!< Current size of the leExtendedFeatures, can be raised up to 240 */

/* Macros for checking extended features */
#define getLeExtendedFeatureByte(bitNumber)     ((bitNumber) / 8U)
#define getLeExtendedFeatureBit(bitNumber)      ((bitNumber) % 8U)

#define isSupportedLeExtendedFeature(features, bitNumber) \
    (((features)[getLeExtendedFeatureByte(bitNumber)] & (1U << getLeExtendedFeatureBit(bitNumber))) != 0U)
/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
/*! BLE result type - the return value of BLE API functions */
typedef enum bleResult_tag
{
    /*
    *
    * Generic result values, base = 0x0000
    *
    */

    gBleStatusBase_c                                            = 0x0000U,                   /*!< General status base. */
    gBleSuccess_c                                               = gBleStatusBase_c | 0x00U,  /*!< Function executed successfully. */

    gBleInvalidParameter_c                                      = gBleStatusBase_c | 0x01U,  /*!< Parameter has an invalid value or is outside the accepted range. */
    gBleOverflow_c                                              = gBleStatusBase_c | 0x02U,  /*!< An internal limit is reached. */
    gBleUnavailable_c                                           = gBleStatusBase_c | 0x03U,  /*!< A requested parameter is not available. */
    gBleFeatureNotSupported_c                                   = gBleStatusBase_c | 0x04U,  /*!< The requested feature is not supported by this stack version. */
    gBleOutOfMemory_c                                           = gBleStatusBase_c | 0x05U,  /*!< An internal memory allocation failed. */
    gBleAlreadyInitialized_c                                    = gBleStatusBase_c | 0x06U,  /*!< Ble_HostInitialize function is incorrectly called a second time. */
    gBleOsError_c                                               = gBleStatusBase_c | 0x07U,  /*!< An error occurred at the OS level. */
    gBleUnexpectedError_c                                       = gBleStatusBase_c | 0x08U,  /*!< A "should never get here"-type error occurred. */
    gBleInvalidState_c                                          = gBleStatusBase_c | 0x09U,  /*!< The requested API cannot be called in the current state. */
    gBleTimerError_c                                            = gBleStatusBase_c | 0x0AU,  /*!< Timer allocation failed. */
    gBleReassemblyInProgress_c                                  = gBleStatusBase_c | 0x0BU,  /*!< HCI Packet reassembly was in progress. The old packet was discarded */
    gBleNVMError_c                                              = gBleStatusBase_c | 0x0CU,  /*!< An error occurred when performing an NVM operation */
    gBleRngError_c                                              = gBleStatusBase_c | 0x0DU,  /*!< error generated by a RNG function call e.g. RNG_GetPseudoRandomNo */
    gBleSecLibError_c                                           = gBleStatusBase_c | 0x0EU,  /*!< error generated by a SecLib function call e.g. SecLib_VerifyBluetoothAh */
    /*
    *
    * HCI result values
    *
    */

    gHciStatusBase_c                                            = 0x0100U,                   /*!< HCI status base. */
    gHciSuccess_c                                               = gBleSuccess_c,             /*!< Alias. */
    /* HCI standard status codes */
    gHciUnknownHciCommand_c                                     = gHciStatusBase_c | 0x01U,  /*!< The HCI Command packet opcode is unknown. */
    gHciUnknownConnectionIdentifier_c                           = gHciStatusBase_c | 0x02U,  /*!< The connection does not exist or it is of a wrong type. */
    gHciHardwareFailure_c                                       = gHciStatusBase_c | 0x03U,  /*!< A failure occurred in the Controller. */
    gHciPageTimeout_c                                           = gHciStatusBase_c | 0x04U,  /*!< A page timed out based on the Page Timeout parameter. */
    gHciAuthenticationFailure_c                                 = gHciStatusBase_c | 0x05U,  /*!< Pairing or authentication failed. Possible causes: incorrect PIN or Link Key. */
    gHciPinOrKeyMissing_c                                       = gHciStatusBase_c | 0x06U,  /*!< Pairing failed because of missing PIN or authentication failed because of missing Key. */
    gHciMemoryCapacityExceeded_c                                = gHciStatusBase_c | 0x07U,  /*!< The controller doesn't have enough memory to store new parameters. */
    gHciConnectionTimeout_c                                     = gHciStatusBase_c | 0x08U,  /*!< The link supervision timeout has expired for a connection or 
                                                                                             *   the synchronization timeout has expired for a broadcast. */
    gHciConnectionLimitExceeded_c                               = gHciStatusBase_c | 0x09U,  /*!< The maximum number of connections has already been reached. */
    gHciSynchronousConnectionLimitToADeviceExceeded_c           = gHciStatusBase_c | 0x0AU,  /*!< The maximum number of synchronous connections has already been reached. */
    gHciAclConnectionAlreadyExists_c                            = gHciStatusBase_c | 0x0BU,  /*!< A connection to this device already exists. */
    gHciCommandDisallowed_c                                     = gHciStatusBase_c | 0x0CU,  /*!< The controller can't process the command at this time. */
    gHciConnectionRejectedDueToLimitedResources_c               = gHciStatusBase_c | 0x0DU,  /*!< The connection was rejected due to limited resources. */
    gHciConnectionRejectedDueToSecurityReasons_c                = gHciStatusBase_c | 0x0EU,  /*!< The connection was rejected because the security requirements were not fulfilled. */
    gHciConnectionRejectedDueToUnacceptableBdAddr_c             = gHciStatusBase_c | 0x0FU,  /*!< The connection was rejected because the device does not accept the given BD_ADDR.
                                                                                             *   A possible cause is that the device only accepts connections from specific BD_ADDRs. */
    gHciConnectionAcceptTimeoutExceeded_c                       = gHciStatusBase_c | 0x10U,  /*!< The connection accept timeout for this connection has been exceeded. */
    gHciUnsupportedFeatureOrParameterValue_c                    = gHciStatusBase_c | 0x11U,  /*!< The feature or parameter in the HCI command is not supported. */
    gHciInvalidHciCommandParameters_c                           = gHciStatusBase_c | 0x12U,  /*!< At least one of the HCI command parameters is invalid. */
    gHciRemoteUserTerminatedConnection_c                        = gHciStatusBase_c | 0x13U,  /*!< The remote device's user terminated the connection or stopped broadcasting. */
    gHciRemoteDeviceTerminatedConnectionLowResources_c          = gHciStatusBase_c | 0x14U,  /*!< The remote device terminated the connection due to low resources. */
    gHciRemoteDeviceTerminatedConnectionPowerOff_c              = gHciStatusBase_c | 0x15U,  /*!< The remote device terminated the connection because it will power off. */
    gHciConnectionTerminatedByLocalHost_c                       = gHciStatusBase_c | 0x16U,  /*!< The local device terminated the connection, synchronization with a broadcaster or stopped broadcasting. */
    gHciRepeatedAttempts_c                                      = gHciStatusBase_c | 0x17U,  /*!< The Controller disallows the authentication or pairing because not enough time has passed since
                                                                                             *   the failed attempt. */
    gHciPairingNotAllowed_c                                     = gHciStatusBase_c | 0x18U,  /*!< The device does not allow pairing. */
    gHciUnknownLpmPdu_c                                         = gHciStatusBase_c | 0x19U,  /*!< The Controller has received an unknown LMP code. */
    gHciUnsupportedRemoteFeature_c                              = gHciStatusBase_c | 0x1AU,  /*!< The remote device doesn't support the feature associated with the issued command, LMP PDU or LL Control PDU. */
    gHciScoOffsetRejected_c                                     = gHciStatusBase_c | 0x1BU,  /*!< The offset requested in the LMP_SCO_LINK_REQ PDU has been rejected. */
    gHciScoIntervalRejected_c                                   = gHciStatusBase_c | 0x1CU,  /*!< The interval requested in the LMP_SCO_LINK_REQ PDU has been rejected. */
    gHciScoAirModeRejected_c                                    = gHciStatusBase_c | 0x1DU,  /*!< The air mode requested in the LMP_SCO_LINK_REQ PDU has been rejected. */
    gHciInvalidLpmParameters_c                                  = gHciStatusBase_c | 0x1EU,  /*!< Some LMP PDU or LL Control PDU parameters were invalid. */
    gHciUnspecifiedError_c                                      = gHciStatusBase_c | 0x1FU,  /*!< No other error code was appropriate. */
    gHciUnsupportedLpmParameterValue_c                          = gHciStatusBase_c | 0x20U,  /*!< An LMP PDU or LL Control PDU contains at least one parameter value not supported by the Controller at this time. */
    gHciRoleChangeNotAllowed_c                                  = gHciStatusBase_c | 0x21U,  /*!< The Controller doesn't allow a role change at this time. */
    gHciLLResponseTimeout_c                                     = gHciStatusBase_c | 0x22U,  /*!< An LMP transaction failed to respond within the LMP response timeout or an LL transaction 
                                                                                             *   failed to respond within the LL response timeout. */
    gHciLmpErrorTransactionCollision_c                          = gHciStatusBase_c | 0x23U,  /*!< An LMP transaction or LL procedure has collided with the same transaction or procedure 
                                                                                             *   which is already in progress. */
    gHciLmpPduNotAllowed_c                                      = gHciStatusBase_c | 0x24U,  /*!< A controller sent an LMP PDU with an opcode that isn't allowed. */
    gHciEncryptionModeNotAcceptable_c                           = gHciStatusBase_c | 0x25U,  /*!< The requested encryption mode is not allowed at this time. */
    gHciLinkKeyCannotBeChanged_c                                = gHciStatusBase_c | 0x26U,  /*!< The link key can't be changed because a fixed unit key is being used. */
    gHciRequestedQosNotSupported_c                              = gHciStatusBase_c | 0x27U,  /*!< The requested QoS is not supported. */
    gHciInstantPassed_c                                         = gHciStatusBase_c | 0x28U,  /*!< An LMP PDU or LL PDU that includes an instant cannot be performed because the instant 
                                                                                             *   when this would have occurred has passed. */
    gHciPairingWithUnitKeyNotSupported_c                        = gHciStatusBase_c | 0x29U,  /*!< It wasn't possible to pair because a unit key was requested but it is not supported. */
    gHciDifferentTransactionCollision_c                         = gHciStatusBase_c | 0x2AU,  /*!< An LMP transaction or LL Procedure was started that collides with another one. */
    gHciReserved_0x2B_c                                         = gHciStatusBase_c | 0x2BU,  
    gHciQosNotAcceptableParameter_c                             = gHciStatusBase_c | 0x2CU,  /*!< The specified QoS parameters couldn't be accepted at this time. */
    gHciQosRejected_c                                           = gHciStatusBase_c | 0x2DU,  /*!< The specified QoS parameters couldn't be accepted and QoS negotiation should be terminated. */
    gHciChannelClassificationNotSupported_c                     = gHciStatusBase_c | 0x2EU,  /*!< The Controller can't perform a channel assessment because it isn't supported. */
    gHciInsufficientSecurity_c                                  = gHciStatusBase_c | 0x2FU,  /*!< The HCI command or LMP PDU sent is only possible on an encrypted link. */
    gHciParameterOutOfMandatoryRange_c                          = gHciStatusBase_c | 0x30U,  /*!< A parameter value requested is outside the mandatory range of parameters for the given 
                                                                                             *   HCI command or LMP PDU. The recipient does not accept that value. */
    gHciReserved_0x31_c                                         = gHciStatusBase_c | 0x31U,
    gHciRoleSwitchPending_c                                     = gHciStatusBase_c | 0x32U,  /*!< A role switch is pending. */
    gHciReserved_0x33_c                                         = gHciStatusBase_c | 0x33U,
    gHciReservedSlotViolation_c                                 = gHciStatusBase_c | 0x34U,  /*!< The Synchronous negotiation was terminated with its state set to Reserved Slot Violation. */
    gHciRoleSwitchFailed_c                                      = gHciStatusBase_c | 0x35U,  /*!< A role switch was attempted but it failed. The original piconet structure is restored. */
    gHciExtendedInquiryResponseTooLarge_c                       = gHciStatusBase_c | 0x36U,  /*!< The extended inquiry response, with the requested requirements for FEC, is too large to 
                                                                                             *   fit in any of the packet types supported by the Controller. */
    gHciSecureSimplePairingNotSupportedByHost_c                 = gHciStatusBase_c | 0x37U,  /*!< The IO capabilities request / response was rejected because the sending Host does not 
                                                                                             *   support Secure Simple Pairing even though the receiving Link Manager does. */
    gHciHostBusyPairing_c                                       = gHciStatusBase_c | 0x38U,  /*!< The Host is busy with another pairing request. Pairing should be retried again later. */
    gHciConnectionRejectedDueToNoSuitableChannelFound_c         = gHciStatusBase_c | 0x39U,  /*!< An appropriate value for the Channel selection operation couldn't be calculated by the Controller. */
    gHciControllerBusy_c                                        = gHciStatusBase_c | 0x3AU,  /*!< The operation was rejected because the Controller was busy. */
    gHciUnacceptableConnectionParameters_c                      = gHciStatusBase_c | 0x3BU,  /*!< The remote device terminated the connection or rejected a request because of at least 
                                                                                             *   one unacceptable connection parameter. */
    gHciDirectedAdvertisingTimeout_c                            = gHciStatusBase_c | 0x3CU,  /*!< Advertising for a fixed duration completed. For directed advertising, advertising completed 
                                                                                             *   without a connection being created. */
    gHciConnectionTerminatedDueToMicFailure_c                   = gHciStatusBase_c | 0x3DU,  /*!< The connection or the synchronization was terminated because the Message Integrity Check failed. */
    gHciConnectionFailedToBeEstablishedOrSyncTimeout_c          = gHciStatusBase_c | 0x3EU,  /*!< The LL initiated a connection / synchronization to periodic advertising but the connection 
                                                                                             *   has failed to be established or the LL failed to synchronize with the periodic advertising within 6 periodic advertising events of the first attempt. */
    gHciMacConnectionFailed_c                                   = gHciStatusBase_c | 0x3FU,
    gHciCoarseClockAdjustmentRejected_c                         = gHciStatusBase_c | 0x40U,  /*!< The Central is unable to make a coarse adjustment to the piconet clock, using the given parameters. 
                                                                                             *   It will attempt to move the clock using clock dragging. */
    gHciType0SubmapNotDefined_c                                 = gHciStatusBase_c | 0x41U,  /*!< The LMP PDU is rejected because the Type 0 submap is not currently defined. */
    gHciUnknownAdvertisingIdentifier_c                          = gHciStatusBase_c | 0x42U,  /*!< A command was sent from the Host that should identify an Advertising or Sync handle, 
                                                                                             *   but that handle doesn't exist. */
    gHciLimitReached_c                                          = gHciStatusBase_c | 0x43U,  /*!< The number of operations requested has been reached. */
    gHciOperationCancelledByHost_c                              = gHciStatusBase_c | 0x44U,  /*!< A request sent from the Host to the Controller which was still pending has been successfully canceled. */
    gHciPacketTooLong_c                                         = gHciStatusBase_c | 0x45U,  /*!< An attempt was made to send or receive a packet that exceeds the maximum allowed packet length. */
    gHciPacketTooLate_c                                         = gHciStatusBase_c | 0x46U,  /*!< Information was provided too late to the Controller. */
    gHciPacketTooEarly_c                                        = gHciStatusBase_c | 0x47U,  /*!< Information was provided too early to the Controller. */

    /* HCI internal status codes */
    gHciAlreadyInit_c                                           = gHciStatusBase_c | 0xA0U,  /*!< HCI has already been initialized. */
    gHciInvalidParameter_c                                      = gHciStatusBase_c | 0xA1U,  /*!< At least one of the HCI command parameters is invalid. */
    gHciCallbackNotInstalled_c                                  = gHciStatusBase_c | 0xA2U,  /*!< Callback was not installed. */
    gHciCallbackAlreadyInstalled_c                              = gHciStatusBase_c | 0xA3U,  /*!< Callback has already been installed. */
    gHciCommandNotSupported_c                                   = gHciStatusBase_c | 0xA4U,  /*!< The HCI command is not supported. */
    gHciEventNotSupported_c                                     = gHciStatusBase_c | 0xA5U,  /*!< The HCI event is not supported. */
    gHciTransportError_c                                        = gHciStatusBase_c | 0xA6U,  /*!< HCI layer initialization failure. HCI data buffering semaphore allocation error. 
                                                                                             *   HCI layer received an unknown packet type. Serial initialization or write failure. */

    /*
    *
    * Controller result values
    *
    */
    gCtrlStatusBase_c                                           = 0x0200U,                    /*!< Controller status base. */
    gCtrlSuccess_c                                              = gBleSuccess_c,              /*!< Alias. */

    /*
    *
    * L2CAP result values
    *
    */
    gL2caStatusBase_c                                           = 0x0300U,                    /*!< L2CAP status base. */
    gL2caSuccess_c                                              = gBleSuccess_c,              /*!< Alias. */
    /* L2CAP standard status codes */

    /* L2CAP internal status codes */
    gL2caAlreadyInit_c                                          = gL2caStatusBase_c | 0xF0U,  /*!< L2CAP has already been initialized. */
    gL2caInsufficientResources_c                                = gL2caStatusBase_c | 0xF1U,  /*!< L2CAP could not allocate resources to perform operations (memory or timers). */
    gL2caCallbackNotInstalled_c                                 = gL2caStatusBase_c | 0xF2U,  /*!< Callback was not installed. */
    gL2caCallbackAlreadyInstalled_c                             = gL2caStatusBase_c | 0xF3U,  /*!< Callback has already been installed. */
    gL2caLePsmInvalid_c                                         = gL2caStatusBase_c | 0xF4U,  /*!< Invalid LE_PSM value. */
    gL2caLePsmAlreadyRegistered_c                               = gL2caStatusBase_c | 0xF5U,  /*!< LE_PSM has already been registered. */
    gL2caLePsmNotRegistered_c                                   = gL2caStatusBase_c | 0xF6U,  /*!< LE_PSM value was not registered. */
    gL2caLePsmInsufficientResources_c                           = gL2caStatusBase_c | 0xF7U,  /*!< No free LE_PSM registration slot was found. */
    gL2caChannelInvalid_c                                       = gL2caStatusBase_c | 0xF8U,  /*!< L2CA channel is invalid. */
    gL2caChannelClosed_c                                        = gL2caStatusBase_c | 0xF9U,  /*!< L2CA channel is closed. */
    gL2caChannelAlreadyConnected_c                              = gL2caStatusBase_c | 0xFAU,  /*!< L2CA channel is already connected. */
    gL2caConnectionParametersRejected_c                         = gL2caStatusBase_c | 0xFBU,  /*!< Connection parameters were rejected. */
    gL2caChannelBusy_c                                          = gL2caStatusBase_c | 0xFCU,  /*!< L2CA channel is busy. */

    gL2caInvalidParameter_c                                     = gL2caStatusBase_c | 0xFEU,  /*!< The command contains at least one invalid parameter. */
    gL2caInternalError_c                                        = gL2caStatusBase_c | 0xFFU,  /*!< There are connected L2CAP channels for the LE_PSM. */

    /*
    *
    * SMP result values
    *
    */
    gSmStatusBase_c                                             = 0x0400U,                  /*!< Security Manager status base. */
    gSmSuccess_c                                                = gBleSuccess_c,            /*!< Alias. */

    gSmNullCBFunction_c                                         = gSmStatusBase_c | 0x01U,
    gSmCommandNotSupported_c                                    = gSmStatusBase_c | 0x02U,  /*!< The Security Manager (SM) does not have the required features or version to support this command */
    gSmUnexpectedCommand_c                                      = gSmStatusBase_c | 0x03U,  /*!< This command is not or cannot be handled in the current context of the SM. */
    gSmInvalidCommandCode_c                                     = gSmStatusBase_c | 0x04U,  /*!< The provided SM command code is invalid. */
    gSmInvalidCommandLength_c                                   = gSmStatusBase_c | 0x05U,  /*!< The provided command length is not valid for the SM command code. */
    gSmInvalidCommandParameter_c                                = gSmStatusBase_c | 0x06U,  /*!< One of the parameters of the SM command is not valid. */
    gSmInvalidDeviceId_c                                        = gSmStatusBase_c | 0x07U,  /*!< The provided Device ID is invalid. */
    gSmInvalidInternalOperation_c                               = gSmStatusBase_c | 0x08U,  /*!< There is a problem with the internal state of the SM. This should not happen during normal operation.
                                                                                            *   A memory corruption or invalid operation may have occurred. */
    gSmInvalidConnectionHandle_c                                = gSmStatusBase_c | 0x09U,  /*!< The target device does not have a valid connection handle. It might be disconnected. */
    gSmInproperKeyDistributionField_c                           = gSmStatusBase_c | 0x0AU,  /*!< The Responder upper layer has set to "1" one or more flags in the Initiator or Responder Key Distribution Fields
                                                                                            *   from the Pairing Request which were set to "0" by the peer device. */
    gSmUnexpectedKeyType_c                                      = gSmStatusBase_c | 0x0BU,  /*!< The Responder upper layer has set a key type field in the Passkey Request Reply
                                                                                            *   command, which is different than the field negotiated with the peer device. */
    gSmUnexpectedPairingTerminationReason_c                     = gSmStatusBase_c | 0x0CU,  /*!< The upper layer tried to cancel the pairing procedure with an unexpected
                                                                                            *   pairing failure reason for the current phase of the pairing procedure. */
    gSmUnexpectedKeyset_c                                       = gSmStatusBase_c | 0x0DU,  /*!< The Responder upper layer is trying to distribute keys which were not requested during the pairing procedure
                                                                                            *   or the peer device has sent a Key Distribution packet which was not expected. */
    gSmSmpTimeoutOccurred_c                                     = gSmStatusBase_c | 0x0EU,  /*!< An SMP timeout has occurred for the peer device.
                                                                                            *   No more operations are accepted until a new physical link is established. */
    gSmUnknownSmpPacketType_c                                   = gSmStatusBase_c | 0x0FU,  /*!< An SMP packet with an unknown (or invalid) type has been received. */
    gSmInvalidSmpPacketLength_c                                 = gSmStatusBase_c | 0x10U,  /*!< An SMP packet with an invalid length for the SMP packet type has been received. */
    gSmInvalidSmpPacketParameter_c                              = gSmStatusBase_c | 0x11U,  /*!< An SMP packet with an invalid parameter has been received. */
    gSmReceivedUnexpectedSmpPacket_c                            = gSmStatusBase_c | 0x12U,  /*!< An unexpected SMP packet was received. */
    gSmReceivedSmpPacketFromUnknownDevice_c                     = gSmStatusBase_c | 0x13U,  /*!< An SMP packet is received but the source Device ID cannot be identified. */
    gSmReceivedUnexpectedHciEvent_c                             = gSmStatusBase_c | 0x14U,  /*!< An HCI event has been received which cannot be handled by the SM or cannot be
                                                                                            *   handled in the current context. */
    gSmReceivedHciEventFromUnknownDevice_c                      = gSmStatusBase_c | 0x15U,  /*!< An HCI event is received but the source Device ID cannot be identified. */
    gSmInvalidHciEventParameter_c                               = gSmStatusBase_c | 0x16U,  /*!< An HCI Event is received with an invalid parameter. */
    gSmLlConnectionEncryptionInProgress_c                       = gSmStatusBase_c | 0x17U,  /*!< A Link Layer Connection encryption was requested by the upper layer or attempted internally by
                                                                                            *   the SM, but it could no be completed because an encryption was already in progress.
                                                                                            *   This situation could lead to an SMP Pairing Failure when the SM cannot encrypt the
                                                                                            *   link with the STK. An unspecified pairing failure reason is used in this instance. */
    gSmLlConnectionEncryptionFailure_c                          = gSmStatusBase_c | 0x18U,  /*!< The Link Layer connection encryption procedure has failed. */
    gSmInsufficientResources_c                                  = gSmStatusBase_c | 0x19U,  /*!< The SM could not allocate resources to perform operations (memory or timers). */
    /* LE SC statuses. */
    gSmOobDataAddressMismatch_c                                 = gSmStatusBase_c | 0x1AU,  /*!< The address of the peer contained in the remote OOB data sent to the stack does not match
                                                                                            *   the address used by the remote device for the connection/pairing procedure. */
    gSmSmpPacketReceivedAfterTimeoutOccurred_c                  = gSmStatusBase_c | 0x1BU,  /*!< A SMP packet has been received from a peer device for which a pairing procedure has timed out.
                                                                                            *   No further operations are permitted until a new connection is established. */
    gSmReceivedTimerEventForUnknownDevice_c                     = gSmStatusBase_c | 0x1CU,  /*!< An Timer event is received but the source Device ID cannot be identified. */
    gSmUnattainableLocalDeviceSecRequirements_c                 = gSmStatusBase_c | 0x1DU,  /*!< The provided pairing parameters cannot lead to a Pairing Procedure which satisfies the minimum
                                                                                            *   security properties for the local device. */
    gSmUnattainableLocalDeviceMinKeySize_c                      = gSmStatusBase_c | 0x1EU,  /*!< The provided pairing parameters cannot lead to a Pairing Procedure which satisfies the minimum
                                                                                            *   encryption key size for the local device. */
    gSmUnattainablePeripheralSecReqRequirements_c               = gSmStatusBase_c | 0x1FU,  /*!< The provided pairing parameters cannot lead to a Pairing Procedure which satisfies the minimum
                                                                                            *   security properties requested by the local device via an SMP Peripheral Security Request. */
    gSmInvalidPeerPublicKey_c                                   = gSmStatusBase_c | 0x20U, /*!< This status covers the case where the peer provides a public key with an identical X coordinate to our own. */

    /* Added for the GAP layer to signal the application about pairing failed reasons. */
    gSmPairingErrorPasskeyEntryFailed_c                         = gSmStatusBase_c | 0x31U,  /*!< The passkey entry failed. */
    gSmPairingErrorConfirmValueFailed_c                         = gSmStatusBase_c | 0x32U,  /*!< The received random confirm value does not match the computed random confirm value. */
    gSmPairingErrorCommandNotSupported_c                        = gSmStatusBase_c | 0x33U,  /*!< The command is not supported. */
    gSmPairingErrorInvalidParameters_c                          = gSmStatusBase_c | 0x34U,  /*!< The command contains at least one invalid parameter. */
    gSmPairingErrorUnknownReason_c                              = gSmStatusBase_c | 0x35U,  /*!< Unknown pairing failure reason. */
    gSmPairingErrorTimeout_c                                    = gSmStatusBase_c | 0x36U,  /*!< The pairing procedure timed out. */
    gSmPairingErrorAuthenticationRequirements_c                 = gSmStatusBase_c | 0x37U,  /*!< Authentication requirements were not met. */
    gSmPairingAlreadyStarted_c                                  = gSmStatusBase_c | 0x38U,  /*!< The pairing process has already started. */
    gSmPairingErrorKeyRejected_c                                = gSmStatusBase_c | 0x39U,  /*!< The key was rejected. */
    gSmPairingErrorPairingNotSupported_c                        = gSmStatusBase_c | 0x3AU,  /*!< Pairing is not supported by the device. */
    gSmPairingErrorEncryptionKeySize_c                          = gSmStatusBase_c | 0x3BU,  /*!< The resultant encryption key size is not long enough for the security requirements of this device. */
    gSmPairingErrorDhKeyCheckFailed_c                           = gSmStatusBase_c | 0x3CU,  /*!< DHKey Check value received doesn't match the one calculated by the local device. */
    gSmPairingErrorNumericComparisonFailed_c                    = gSmStatusBase_c | 0x3DU,  /*!< Indicates that the confirm values in the numeric comparison protocol do not match. */
    gSmPairingErrorOobNotAvailable_c                            = gSmStatusBase_c | 0x3EU,  /*!< Indicates that the OOB data is not available. */
    gSmPairingErrorRepeatedAttempts_c                           = gSmStatusBase_c | 0x3FU,  /*!< Indicates that the pairing or authentication procedure is disallowed because too little time has elapsed since last pairing request or security request. */
    gSmPairingErrorBusy_c                                       = gSmStatusBase_c | 0x40U,  /*!< Indicates that the device is not ready to perform a pairing procedure */ 
    
    /* SM Tool Box statuses. */
    gSmTbResolvableAddressDoesNotMatchIrk_c                     = gSmStatusBase_c | 0x60U,  /*!< The provided Resolvable Private Address and IRK do not match. */
    gSmTbInvalidDataSignature_c                                 = gSmStatusBase_c | 0x61U,  /*!< The provided data signature does not match the computed data signature. */

    gSmKeySessionKeyDerivationFailed_c                          = gSmStatusBase_c | 0x62U,  /*!< The session key derivation failed. */

    /*
    *
    * ATT result values
    *
    */
    gAttStatusBase_c                                            = 0x0500U,                  /*!< ATT status base. */
    gAttSuccess_c                                               = gBleSuccess_c,            /*!< Alias. */

    /* ATT error codes as defined in "att_errors.h" are added to the gAttStatusBase_c. */

    /*
    *
    * GATT result values
    *
    */
    gGattStatusBase_c                                           = 0x0600U,                    /*!< GATT status base. */
    gGattSuccess_c                                              = gBleSuccess_c,              /*!< Alias. */

    gGattAnotherProcedureInProgress_c                           = gGattStatusBase_c | 0x01U,  /*!< Trying to start a GATT procedure while one is already in progress. */
    gGattLongAttributePacketsCorrupted_c                        = gGattStatusBase_c | 0x02U,  /*!< Writing a Long Characteristic failed because Prepare Write Request packets were corrupted. */
    gGattMultipleAttributesOverflow_c                           = gGattStatusBase_c | 0x03U,  /*!< Too many Characteristics are given for a Read Multiple Characteristic procedure. */
    gGattUnexpectedReadMultipleResponseLength_c                 = gGattStatusBase_c | 0x04U,  /*!< Read Multiple Characteristic procedure failed because unexpectedly long data was read. */
    gGattInvalidValueLength_c                                   = gGattStatusBase_c | 0x05U,  /*!< An invalid value length was supplied to a Characteristic Read/Write operation. */
    gGattServerTimeout_c                                        = gGattStatusBase_c | 0x06U,  /*!< No response was received from the Server. */

    gGattIndicationAlreadyInProgress_c                          = gGattStatusBase_c | 0x07U,  /*!< A Server Indication is already waiting for Client Confirmation. */
    gGattClientConfirmationTimeout_c                            = gGattStatusBase_c | 0x08U,  /*!< No Confirmation was received from the Client after a Server Indication. */

    gGattInvalidPduReceived_c                                   = gGattStatusBase_c | 0x09U,  /*!< An invalid PDU length was received. */
    gGattPeerDisconnected_c                                     = gGattStatusBase_c | 0x0AU,  /*!< An ongoing GATT procedure could not be finished due to peer's disconnection. */
    gGattMtuExchangeInProgress_c                                = gGattStatusBase_c | 0x0BU,  /*!< A Server Indication is already waiting for Client Confirmation. */
    gGattOutOfSyncProceduresOngoing_c                           = gGattStatusBase_c | 0x0CU,  /*!< Client can't read the Database Hash until all pending procedures are done. */
    gGattConnectionSecurityRequirementsNotMet_c                 = gGattStatusBase_c | 0x0DU,  /*!< Client cannot initiate communication if the device security requirements are not met,
                                                                                              *   e.g., link is not yet encrypted/authenticated. */

    /*
    *
    * GAP result values
    *
    */
    gGapStatusBase_c                                            = 0x0700U,                    /*!< GAP status base. */
    gGapSuccess_c                                               = gBleSuccess_c,              /*!< Alias. */

    gGapAdvDataTooLong_c                                        = gGapStatusBase_c | 0x01U,   /*!< Trying to set too many bytes in the advertising payload. */
    gGapScanRspDataTooLong_c                                    = gGapStatusBase_c | 0x02U,   /*!< Trying to set too many bytes in the scan response payload. */
    gGapDeviceNotBonded_c                                       = gGapStatusBase_c | 0x03U,   /*!< Trying to execute an API that is only available for bonded devices. */
    gGapAnotherProcedureInProgress_c                            = gGapStatusBase_c | 0x04U,   /*!< Trying to start a GAP procedure while one is already in progress. */

    /*
    *
    * Reserved result values
    *
    */
    gDevDbStatusBase_c                                          = 0x0800U,                       /*!< DeviceDatabase status base. */
    gDevDbSuccess_c                                             = gBleSuccess_c,                 /*!< Alias. */

    gDevDbCccdLimitReached_c                                    = gDevDbStatusBase_c | 0x01U,    /*!< CCCD value cannot be saved because Server's CCCD list is full for the current client. */
    gDevDbCccdNotFound_c                                        = gDevDbStatusBase_c | 0x02U,    /*!< CCCD with the given handle is not found in the Server's list for the current client. */

    /*
    *
    * GATT_DB result values
    *
    */
    gGattDbStatusBase_c                                         = 0x0900U,                       /*!< GATT Database status base. */
    gGattDbSuccess_c                                            = gBleSuccess_c,                 /*!< Alias. */

    gGattDbInvalidHandle_c                                      = gGattDbStatusBase_c | 0x01U,   /*!< An invalid handle was passed as parameter. */
    gGattDbCharacteristicNotFound_c                             = gGattDbStatusBase_c | 0x02U,   /*!< Characteristic was not found. */
    gGattDbCccdNotFound_c                                       = gGattDbStatusBase_c | 0x03U,   /*!< CCCD was not found. */
    gGattDbServiceNotFound_c                                    = gGattDbStatusBase_c | 0x04U,   /*!< Service Declaration was not found. */
    gGattDbDescriptorNotFound_c                                 = gGattDbStatusBase_c | 0x05U,   /*!< Characteristic Descriptor was not found. */
    gGattDbServiceOrCharAlreadyDeclared_c                       = gGattDbStatusBase_c | 0x06U,   /*!< Service or characteristic already declared */

    /*
    *
    * Channel Sounding result values
    *
    */
    gCsStatusBase_c                                             = 0x0A00U,                       /*!< Channel Sounding status base. */
    gCsSuccess_c                                                = gBleSuccess_c,                 /*!< Alias. */

    gCsCallbackAlreadyInstalled_c                               = gCsStatusBase_c | 0x01U,       /*!< The selected callback was already installed. */
    gCsSecurityCheckFail_c                                      = gCsStatusBase_c | 0x02U,       /*!< GAP Mode 4 security check fail. */

} bleResult_t;

/*! Unique identifier type for a connected device. */
typedef uint8_t deviceId_t;

/*! ATT bearer identifier. */
typedef uint8_t bearerId_t;

/*! Enhanced ATT bearer status values */
typedef enum bearerStatus_tag
{
    gEattBearerActive_c,
    gEattBearerReconfInProgress_c,
    gEattBearerSuspended_c,
    gEattBearerAllocated_c,
    gEattBearerFree_c,
    gEattBearerAlreadyAllocated_c,
} bearerStatus_t;

/*! Bluetooth Device Address Type - Size: 1 Octet, Range: [gBleAddrTypePublic_c:gBleAddrTypeRandom_c] */
typedef uint8_t bleAddressType_t;

/*! Bluetooth Device Address - array of 6 bytes. */
typedef uint8_t bleDeviceAddress_t[gcBleDeviceAddressSize_c];

/*! Bluetooth Identity Address - array of 6 bytes. */
typedef struct
{
    bleAddressType_t    idAddressType; /*!< Public or Random (static). */
    bleDeviceAddress_t  idAddress;     /*!< 6-byte address. */
} bleIdentityAddress_t;

/*! Bluetooth UUID type - values chosen to correspond with the ATT UUID format */
typedef uint8_t bleUuidType_t;

/*! Union for a Bluetooth UUID; selected according to an accompanying bleUuidType_t */
typedef union {
    uint16_t    uuid16;         /*!< For gBleUuidType16_c. */
    uint32_t    uuid32;         /*!< For gBleUuidType32_c. */
    uint8_t     uuid128[16];    /*!< For gBleUuidType128_c. */
} bleUuid_t;

/*! Advertising Type */
typedef enum {
    gAdvConnectableUndirected_c                 = 0x00U,     /*!< Answers to both connect and scan requests. */
    gAdvDirectedHighDutyCycle_c                 = 0x01U,     /*!< Answers only to connect requests; smaller advertising interval for quicker connection. */
    gAdvScannable_c                             = 0x02U,     /*!< Answers only to scan requests. */
    gAdvNonConnectable_c                        = 0x03U,     /*!< Does not answer to connect nor scan requests. */
    gAdvDirectedLowDutyCycle_c                  = 0x04U      /*!< Answers only to connect requests; larger advertising interval. */
} bleAdvertisingType_t;

/*! Advertising Event properties */
typedef uint16_t bleAdvReportEventProperties_t;
typedef enum
{
    gAdvEventConnectable_c    = BIT0, /*!< Connectable Advertisement */
    gAdvEventScannable_c      = BIT1, /*!< Scannable Advertisement */
    gAdvEventDirected_c       = BIT2, /*!< Directed Advertisement */
    gAdvEventScanResponse_c   = BIT3, /*!< Scan Response */
    gAdvEventLegacy_c         = BIT4, /*!< Legacy Advertisement PDU */
    gAdvEventAnonymous_c      = BIT5, /*!< Anonymous Advertisement */
} bleAdvReportEventProperties_tag;

/*! Advertising Request properties */
typedef uint16_t bleAdvRequestProperties_t;
typedef enum
{
    gAdvReqConnectable_c             = BIT0, /*!< Connectable Advertising */
    gAdvReqScannable_c               = BIT1, /*!< Scannable Advertising */
    gAdvReqDirected_c                = BIT2, /*!< Directed Advertising */
    gAdvReqHighDutyCycle_c           = BIT3, /*!< High Duty Cycle */
    gAdvReqLegacy_c                  = BIT4, /*!< Legacy Advertising PDU */
    gAdvReqAnonymous_c               = BIT5, /*!< Anonymous Advertising */
    gAdvIncludeTxPower_c             = BIT6, /*!< Set this option to include the Tx power in advertising packet. */
    gAdvUseDecisionPDU_c             = BIT7, /*!< Use ADV_DECISION_IND PDU(0b1001). */
    gAdvIncludeAdvAinDecisionPDU_c   = BIT8, /*!< Include AdvA in the extended header of all decision PDUs. */
    gAdvIncludeADIinDecisionPDU_c    = BIT9, /*!< Include ADI in the extended header of all decision PDUs. */
} bleAdvRequestProperties_tag;

typedef struct
{
    uint8_t     enableChannel37:    1;  /*!< Bit for channel 37. */
    uint8_t     enableChannel38:    1;  /*!< Bit for channel 38. */
    uint8_t     enableChannel39:    1;  /*!< Bit for channel 39. */
    uint8_t     reserved:           5;  /*!< Reserved for future use. */
} bleAdvertisingChannelMap_t;

typedef enum
{
    gBleAdvFilterAllowScanFromAnyAllowConnFromAny_c     = 0x00U,     /*!< Filter Accept List is ignored. */
    gBleAdvFilterAllowScanFromWLAllowConnFromAny_c      = 0x01U,     /*!< Filter Accept List is used only for Scan Requests. */
    gBleAdvFilterAllowScanFromAnyAllowConnFromWL_c      = 0x02U,     /*!< Filter Accept List is used only for Connection Requests. */
    gBleAdvFilterAllowScanFromWLAllowConnFromWL_c       = 0x03U,     /*!< Filter Accept List is used for both Scan and Connection Requests. */
} bleAdvertisingFilterPolicy_t;

typedef enum
{
    gBleLlConnectionCentral_c        = 0x00U, /*!< Link Layer Central Role */
    gBleLlConnectionPeripheral_c         = 0x01U, /*!< Link Layer Peripheral Role */
} bleLlConnectionRole_t;

typedef uint8_t bleCentralClockAccuracy_t;
typedef enum
{
    gBleCentralClkAcc500ppm_c        = 0x00U,
    gBleCentralClkAcc250ppm_c        = 0x01U,
    gBleCentralClkAcc150ppm_c        = 0x02U,
    gBleCentralClkAcc100ppm_c        = 0x03U,
    gBleCentralClkAcc75ppm_c         = 0x04U,
    gBleCentralClkAcc50ppm_c         = 0x05U,
    gBleCentralClkAcc30ppm_c         = 0x06U,
    gBleCentralClkAcc20ppm_c         = 0x07U,
} bleCentralClockAccuracy_tag;

typedef uint8_t bleAdvertiserClockAccuracy_t;
typedef enum
{
    gBleAdvertiserClkAcc500ppm_c     = 0x00U,
    gBleAdvertiserClkAcc250ppm_c     = 0x01U,
    gBleAdvertiserClkAcc150ppm_c     = 0x02U,
    gBleAdvertiserClkAcc100ppm_c     = 0x03U,
    gBleAdvertiserClkAcc75ppm_c      = 0x04U,
    gBleAdvertiserClkAcc50ppm_c      = 0x05U,
    gBleAdvertiserClkAcc30ppm_c      = 0x06U,
    gBleAdvertiserClkAcc20ppm_c      = 0x07U,
} bleAdvertiserClockAccuracy_tag;

typedef enum
{
    gBleAdvRepAdvInd_c              = 0x00U, /* Connectable undirected advertising (ADV_IND) */
    gBleAdvRepAdvDirectInd_c        = 0x01U, /* Connectable directed advertising (ADV_DIRECT_IND) */
    gBleAdvRepAdvScanInd_c          = 0x02U, /* Scannable undirected advertising (ADV_SCAN_IND) */
    gBleAdvRepAdvNonconnInd_c       = 0x03U, /* Non Connectable undirected advertising (ADV_NONCONN_IND) */
    gBleAdvRepScanRsp_c             = 0x04U, /* Scan Response (SCAN_RSP) */
} bleAdvertisingReportEventType_t;

typedef enum
{
    gHciCommandPacket_c          = 0x01U, /*!<  HCI Command */
    gHciDataPacket_c             = 0x02U, /*!<  L2CAP Data Packet */
    gHciSynchronousDataPacket_c  = 0x03U, /*!<  Not used in BLE */
    gHciEventPacket_c            = 0x04U, /*!<  HCI Event */
    gHciIsoDataPacket_c          = 0x05U, /*!<  HCI ISO data packet */
} hciPacketType_t;

/*! Scanning type enumeration. */
typedef enum {
    gScanTypePassive_c   = 0x00U,     /*!< Passive Scanning - advertising packets are immediately reported to the Host. */
    gScanTypeActive_c    = 0x01U      /*!< Active Scanning - the scanner sends scan requests to the advertiser and reports to the Host after the scan response is received. */
} bleScanType_t;

/*! Scanning filter policy enumeration - Size: 1 Octet, Range: [gScanAll_c:gScanWithFilterAcceptList_c] */
typedef uint8_t bleScanningFilterPolicy_t;

/*! Initiator filter policy enumeration - Size: 1 Octet, Range: [gUseDeviceAddress_c:gUseFilterAcceptList_c] */
typedef uint8_t bleInitiatorFilterPolicy_t;

/*! Privacy Mode enumeration - Size: 1 Octet, Range: [gNetworkPrivacy_c:gDevicePrivacy_c] */
typedef uint8_t blePrivacyMode_t;

typedef enum
{
    gReadCurrentTxPowerLevel_c   = 0x00U, /*!< Current TX Power level. */
    gReadMaximumTxPowerLevel_c   = 0x01U, /*!< Maximum recorded TX Power level. */
} bleTransmitPowerLevelType_t;

typedef enum
{
    gTxPowerAdvChannel_c         = 0x00U, /*!< Advertising channel type when setting Tx Power. */
    gTxPowerConnChannel_c        = 0x01U, /*!< Connection channel type when setting Tx Power. */
} bleTransmitPowerChannelType_t;

typedef enum
{
    gOverrideAdvChannels_c         = 0x00U, /*!< Set the advertising channels */
    gOverrideScanChannels_c        = 0x01U, /*!< Set the scanning channels */
    gOverrideInitChannels_c        = 0x02U  /*!< Set the connection initiation channels */
} bleChannelOverrideMode_t;

typedef enum
{
    gBleFreq2402MHz_c       = 0x00U,
    gBleFreq2404MHz_c       = 0x01U,
    gBleFreq2406MHz_c       = 0x02U,
    gBleFreq2408MHz_c       = 0x03U,
    gBleFreq2410MHz_c       = 0x04U,
    gBleFreq2412MHz_c       = 0x05U,
    gBleFreq2414MHz_c       = 0x06U,
    gBleFreq2416MHz_c       = 0x07U,
    gBleFreq2418MHz_c       = 0x08U,
    gBleFreq2420MHz_c       = 0x09U,
    gBleFreq2422MHz_c       = 0x0AU,
    gBleFreq2424MHz_c       = 0x0BU,
    gBleFreq2426MHz_c       = 0x0CU,
    gBleFreq2428MHz_c       = 0x0DU,
    gBleFreq2430MHz_c       = 0x0EU,
    gBleFreq2432MHz_c       = 0x0FU,
    gBleFreq2434MHz_c       = 0x10U,
    gBleFreq2436MHz_c       = 0x11U,
    gBleFreq2438MHz_c       = 0x12U,
    gBleFreq2440MHz_c       = 0x13U,
    gBleFreq2442MHz_c       = 0x14U,
    gBleFreq2444MHz_c       = 0x15U,
    gBleFreq2446MHz_c       = 0x16U,
    gBleFreq2448MHz_c       = 0x17U,
    gBleFreq2450MHz_c       = 0x18U,
    gBleFreq2452MHz_c       = 0x19U,
    gBleFreq2454MHz_c       = 0x1AU,
    gBleFreq2456MHz_c       = 0x1BU,
    gBleFreq2458MHz_c       = 0x1CU,
    gBleFreq2460MHz_c       = 0x1DU,
    gBleFreq2462MHz_c       = 0x1EU,
    gBleFreq2464MHz_c       = 0x1FU,
    gBleFreq2466MHz_c       = 0x20U,
    gBleFreq2468MHz_c       = 0x21U,
    gBleFreq2470MHz_c       = 0x22U,
    gBleFreq2472MHz_c       = 0x23U,
    gBleFreq2474MHz_c       = 0x24U,
    gBleFreq2476MHz_c       = 0x25U,
    gBleFreq2478MHz_c       = 0x26U,
    gBleFreq2480MHz_c       = 0x27U,
} bleChannelFrequency_t;

/*! Bluetooth Channel map - array of 5 bytes. */
typedef uint8_t bleChannelMap_t[gcBleChannelMapSize_c];

typedef enum
{
    gBleTestPacketPayloadPrbs9_c            = 0x00U,
    gBleTestPacketPayloadPattern11110000_c  = 0x01U,
    gBleTestPacketPayloadPattern10101010_c  = 0x02U,
    gBleTestPacketPayloadPrbs15_c           = 0x03U,
    gBleTestPacketPayloadPatternAllBits1_c  = 0x04U,
    gBleTestPacketPayloadPatternAllBits0_c  = 0x05U,
    gBleTestPacketPayloadPattern00001111_c  = 0x06U,
    gBleTestPacketPayloadPattern01010101_c  = 0x07U,
} bleTxTestPacketPayload_t;

typedef enum
{
    bleHwErrCodeNoError_c                   = 0x00U,
} bleHardwareErrorCode_t;

typedef struct
{
    uint8_t     randomValue[gSmpLeScRandomValueSize_c];          /*!< LE SC OOB r (Random value) */
    uint8_t     confirmValue[gSmpLeScRandomConfirmValueSize_c];  /*!< LE SC OOB Cr (Random Confirm value) */
} gapLeScOobData_t;

typedef uint64_t leSupportedFeatures_t;
typedef enum
{
    gLeEncryption_c                            = 1U << (0U * 8U + 0U), /* 0 */
    gLeConnectionParametersRequestProcedure_c  = 1U << (0U * 8U + 1U), /* 1 */
    gLeExtendedRejectIndication_c              = 1U << (0U * 8U + 2U), /* 2 */
    gLePeripheralInitiatedFeaturesExchange_c   = 1U << (0U * 8U + 3U), /* 3 */
    gLePing_c                                  = 1U << (0U * 8U + 4U), /* 4 */
    /* BLE v4.2 */
    gLeDataPacketLengthExtension_c             = 1U << (0U * 8U + 5U), /* 5 */
    gLeLlPrivacy_c                             = 1U << (0U * 8U + 6U), /* 6 */
    gLeExtendedScannerFilterPolicies_c         = 1U << (0U * 8U + 7U), /* 7 */
    /* BLE v5.0 */
    gLe2MbPhy_c                                = 1U << (1U * 8U + 0U), /* 8 */
    gLeStableModulationIdxTx_c                 = 1U << (1U * 8U + 1U), /* 9 */
    gLeStableModulationIdxRx_c                 = 1U << (1U * 8U + 2U), /* 10 */
    gLeCodedPhy_c                              = 1U << (1U * 8U + 3U), /* 11 */
    gLeExtendedAdv_c                           = 1U << (1U * 8U + 4U), /* 12 */
    gLePeriodicAdv_c                           = 1U << (1U * 8U + 5U), /* 13 */
    gLeChannelSelAlg2_c                        = 1U << (1U * 8U + 6U), /* 14 */
    gLePowerClass1_c                           = 1U << (1U * 8U + 7U), /* 15 */
    gLeMinNumOfUsedChanProcedure_c             = 1U << (2U * 8U + 0U), /* 16 */
    /* BLE v5.1 */
    gLePeriodicAdvSyncTransferSender_c         = 1U << (3U * 8U + 0U), /* 24 */
    gLePeriodicAdvSyncTransferReceiver_c       = 1U << (3U * 8U + 1U), /* 25 */
    /* BLE v5.2 */
    gLePowerControlRequest1_c                  = (uint64_t)1 << (4U * 8U + 1U), /* 33 */
    gLePowerControlRequest2_c                  = (uint64_t)1 << (4U * 8U + 2U), /* 34 */
    gLePathLossMonitoring_c                    = (uint64_t)1 << (4U * 8U + 3U), /* 35 */
    /* BLE v5.3 */
    gLeConnectionSubrating_c                   = (uint64_t)1 << (4U * 8U + 5U), /* 37 */
    gLeConnectionSubratingHostSupport_c        = (uint64_t)1 << (4U * 8U + 6U), /* 38 */
    /* BLE v5.4 */
    gAdvertisingCodingSelection_c               = (uint64_t)1 << (5U * 8U), /* 40 */
    gAdvertisingCodingSelectionHostSupport_c    = (uint64_t)1 << (5U * 8U + 1U), /* 41 */
    gLePawrAdvertiser_c                         = (uint64_t)1 << (5U * 8U + 3U), /* 43 */
    gLePawrScanner_c                            = (uint64_t)1 << (5U * 8U + 4U), /* 44 */

    gLeDecisionBasedAdvertisingFiltering_c     = (uint64_t)1 << (5U * 8U + 2U), /* 42 */
} leSupportedFeatures_tag;

/* Extended LE Features beyond the first 64 bits */
typedef enum
{
    /* Features starting from bit 64 (byte 8) */
    gLeMonitoringAdvertisers_c                  = 0U,   /* Bit 0 of byte 8, bit position 64 in Link Layer Feature table */
} leExtendedSupportedFeatures_tag;

/*! Generic Event Type */
typedef enum {
    gInitializationComplete_c                               = 0x00U, /*!< Initial setup started by Ble_HostInitialize is complete. */
    gInternalError_c                                        = 0x01U, /*!< An internal error occurred. */
    gAdvertisingSetupFailed_c                               = 0x02U, /*!< Error during advertising setup. */
    gAdvertisingParametersSetupComplete_c                   = 0x03U, /*!< Advertising parameters have been successfully set. Response to Gap_SetAdvertisingParameters. */
    gAdvertisingDataSetupComplete_c                         = 0x04U, /*!< Advertising and/or scan response data has been successfully set. Response to Gap_SetAdvertisingData. */
    gFilterAcceptListSizeRead_c                             = 0x05U, /*!< Contains the Filter Accept List size. Response to Gap_ReadFilterAcceptListSize. */
    gDeviceAddedToFilterAcceptList_c                        = 0x06U, /*!< Device has been added to Filter Accept List. Response to Gap_AddDeviceToFilterAcceptList. */
    gDeviceRemovedFromFilterAcceptList_c                    = 0x07U, /*!< Device has been removed from the Filter Accept List. Response to Gap_RemoveDeviceFromFilterAcceptList. */
    gFilterAcceptListCleared_c                              = 0x08U, /*!< Filter Accept List has been cleared. Response to Gap_ClearFilterAcceptList. */
    gRandomAddressReady_c                                   = 0x09U, /*!< A random device address has been created. Response to Gap_CreateRandomDeviceAddress. */
    gCreateConnectionCanceled_c                             = 0x0AU, /*!< Connection initiation was successfully cancelled. Response to Gap_CancelInitiatingConnection. */
    gPublicAddressRead_c                                    = 0x0BU, /*!< Contains the public device address. Response to Gap_ReadPublicDeviceAddress. */
    gAdvTxPowerLevelRead_c                                  = 0x0CU, /*!< Contains the TX power on the advertising channel. Response to Gap_ReadAdvertisingTxPowerLevel. */
    gPrivateResolvableAddressVerified_c                     = 0x0DU, /*!< Contains the result of PRA verification. Response to Gap_VerifyPrivateResolvableAddress. */
    gRandomAddressSet_c                                     = 0x0EU, /*!< Random address has been set into the Controller. Response to Gap_SetRandomAddress. */
    gLeScPublicKeyRegenerated_c                             = 0x0FU, /*!< The private/public key pair used for LE Secure Connections pairing has been regenerated. */
    gLeScLocalOobData_c                                     = 0x10U, /*!< Local OOB data used for LE Secure Connections pairing. */
    gHostPrivacyStateChanged_c                              = 0x11U, /*!< Host Privacy was enabled or disabled. */
    gControllerPrivacyStateChanged_c                        = 0x12U, /*!< Controller Privacy was enabled or disabled. */
    gControllerTestEvent_c                                  = 0x13U, /*!< Controller Test was started or stopped. */
    gTxPowerLevelSetComplete_c                              = 0x14U, /*!< Controller Tx Power Level set complete or invalid. */
    gLePhyEvent_c                                           = 0x15U, /*!< Phy Mode of a connection has been updated by the Controller. */
    gControllerNotificationEvent_c                          = 0x16U, /*!< Controller Enhanced Notification received. */
    gGetConnParamsComplete_c                                = 0x17U, /*!< Get Connection Parameters command complete */
    gBondCreatedEvent_c                                     = 0x18U, /*!< Bond Created Event signalling the stack created a bond after pairing or at app request. */
    gChannelMapSet_c                                        = 0x19U, /*!< Channel map set complete in the Controller. */
    gExtAdvertisingParametersSetupComplete_c                = 0x1AU, /*!< Extended advertising parameters have been successfully set. */
    gExtAdvertisingDataSetupComplete_c                      = 0x1BU, /*!< Extended advertising data has been successfully set. */
    gExtAdvertisingSetRemoveComplete_c                      = 0x1CU, /*!< An advertising set has been removed from the Controller. */
    gPeriodicAdvParamSetupComplete_c                        = 0x1DU, /*!< Periodic advertising parameters have been successfully set. */
    gPeriodicAdvDataSetupComplete_c                         = 0x1EU, /*!< Periodic advertising data have been successfully set. */
    gPeriodicAdvertisingStateChanged_c                      = 0x1FU, /*!< Event received when periodic advertising has been successfully enabled or disabled. */
    gPeriodicAdvListUpdateComplete_c                        = 0x20U, /*!< Periodic advertiser list has been successfully updated. */
    gPeriodicAdvCreateSyncCancelled_c                       = 0x21U, /*!< Periodic advertising create sync command was successfully cancelled */
    gTxEntryAvailable_c                                     = 0x22U, /*!< This event is generated when a TX entry becomes available after they were all in use. */
    gControllerLocalRPARead_c                               = 0x23U, /*!< Contains the resolvable private device address. Response to Gap_ReadControllerLocalRPA. */
    gConnectionlessCteTransmitParamsSetupComplete_c         = 0x24U, /*!< Connectionless CTE transmit parameters have been successfully set. */
    gConnectionlessCteTransmitStateChanged_c                = 0x25U, /*!< Connectionless CTE for an advertising set was enabled or disabled. */
    gConnectionlessIqSamplingStateChanged_c                 = 0x26U, /*!< Connectionless CTE IQ sampling for an advertising train was enabled or disabled. */
    gAntennaInformationRead_c                               = 0x27U, /*!< Antenna information was read from the controller. */
    gModifiedSleepClockAccuracy_c                           = 0x28U, /*!< The Sleep Clock accuracy was changed */
    gPeriodicAdvRecvEnableComplete_c                        = 0x29U, /*!< Enable or disable reports for the periodic advertising train command is complete */
    gPeriodicAdvSyncTransferComplete_c                      = 0x2AU, /*!< The command used to instruct the Controller to send synchronization information about the periodic advertising train identified by the Sync_Handle parameter to a connected device is complete */
    gPeriodicAdvSetInfoTransferComplete_c                   = 0x2BU, /*!< The command used to instruct the Controller to send synchronization information about the periodic advertising in an advertising set to a connected device is complete */
    gSetPeriodicAdvSyncTransferParamsComplete_c             = 0x2CU, /*!< The command specifying how the Controller will process periodic advertising synchronization information is complete */
    gSetDefaultPeriodicAdvSyncTransferParamsComplete_c      = 0x2DU, /*!< The command which set the default parameters for  periodic advertising synchronization information is complete */
    gPeriodicAdvSyncTransferSucceeded_c                     = 0x2EU, /*!< Event received when Controller has succeeded the Synchronization to the periodic advertising train */
    gPeriodicAdvSyncTransferFailed_c                        = 0x2FU, /*!< Event received when Controller has failed the Synchronization to the periodic advertising train */
    gConnEvtLeGenerateDhKeyComplete_c                       = 0x30U, /*!< DHKey generation is complete. Key can be found in gapConnectionEvent_t.eventData.leGenerateDhKeyCompleteEvent */
    gHandoverGetComplete_c                                  = 0x31U, /*!< Handover data get complete. */
    gHandoverSetComplete_c                                  = 0x32U, /*!< Handover data set complete. */
    gHandoverGetCsLlContextComplete_c                       = 0x33U, /*!< Handover CS LL context data get complete. */
    gHandoverSetCsLlContextComplete_c                       = 0x34U, /*!< Handover CS LL context data set complete. */
    gHandoverGetTime_c                                      = 0x35U, /*!< Handover Get Time command complete */
    gHandoverSuspendTransmitComplete_c                      = 0x36U, /*!< Handover Suspend Transmit command complete */
    gHandoverResumeTransmitComplete_c                       = 0x37U, /*!< Handover Resume Transmit command complete */
    gHandoverAnchorNotificationStateChanged_c               = 0x38U, /*!< Handover Anchor Notification command complete */
    gHandoverAnchorSearchStarted_c                          = 0x39U, /*!< Handover Anchor Search Start command complete */
    gHandoverAnchorSearchStopped_c                          = 0x3AU, /*!< Handover Anchor Search Stop command complete */
    gHandoverTimeSyncTransmitStateChanged_c                 = 0x3BU, /*!< Handover Time Sync Transmit command complete */
    gHandoverTimeSyncReceiveComplete_c                      = 0x3CU, /*!< Handover Time Sync Receive command complete */
    gHandoverAnchorMonitorEvent_c                           = 0x3DU, /*!< Event received from Controller - Handover Anchor Monitor */
    gHandoverTimeSyncEvent_c                                = 0x3EU, /*!< Event received from Controller - Handover Time Sync */
    gHandoverConnParamUpdateEvent_c                         = 0x3FU, /*!< Event received from Controller - Handover Conn Params Update */
    gRemoteVersionInformationRead_c                         = 0x40U, /*!< Version Information of a peer was read */
    gLlSkdReportEvent_c                                     = 0x41U, /*!< Session Key Diversifier report */
    gLeSetSchedulerPriorityComplete_c                       = 0x42U, /*!< LE Set Scheduler Priority command complete */
    gDeInitializationComplete_c                             = 0x43U, /*!< Event received when Ble_HostDeInitialize is complete. */
    gHandoverAnchorMonitorPacketEvent_c                     = 0x44U, /*!< Event received from Controller - Handover Anchor Monitor Packet. pPdu must be freed by the application */
    gHandoverAnchorMonitorPacketContinueEvent_c             = 0x45U, /*!< Event received from Controller - Handover Anchor Monitor Packet Continue. pPdu must be freed by the application */
    gHandoverFreeComplete_c                                 = 0x46U, /*!< Handover data free complete. */
    gHandoverUpdateConnParamsComplete_c                     = 0x47U, /*!< Handover Update Connection Parameters command complete */
    gExtAdvertisingDecisionDataSetupComplete_c              = 0x48U, /*!< Extended advertising decision data has been successfully set. */
    gDecisionInstructionsSetupComplete_c                    = 0x49U, /*!< decision instructions used when listening for decision advertisements PDUs has been successfully set. */
    gHandoverLlPendingData_c                                = 0x4AU, /*!< ACL Data pending to be transmited by the LL */
    gLeChannelOverrideComplete_c                            = 0x4BU, /*!< LE Channel Override command complete */
    gPeriodicAdvSetSubeventDataComplete_c                   = 0x4CU, /*!< Periodic advertising subevent data has been successfully set. */
    gPeriodicAdvSetResponseDataComplete_c                   = 0x4DU, /*!< Periodic advertising response data has been successfully set. */
    gPeriodicSyncSubeventComplete_c                         = 0x4EU, /*!< Set Sync Subevent command successfully completed. */
    gHandoverConnectionUpdateProcedureEvent_c               = 0x4FU, /*!< This event is used to report the new connection parameters indicated during the Connection Update procedure */
    gHandoverApplyConnectionUpdateProcedureComplete_c       = 0x50U, /*!< This event is used to report the new connection parameters indicated during the Connection Update procedure */
    gVendorUnitaryTestComplete_c                            = 0x51U, /*!< Vendor Unitary Test command complete */
    gLeSetDataRelatedAddressChangesComplete_c               = 0x52U, /*!< Set Data Related Address Changes command complete */
    gLePeriodicAdvUpdateSyncComplete_c                      = 0x53U, /*!< Le Periodic Adv Update Sync command complete */
    gDeviceAddedToMonAdvList_c                              = 0x54U, /*!< LE Add Device To Monitored Advertisers List command complete */
    gDeviceRemovedFromMonAdvList_c                          = 0x55U, /*!< Remove Device From Monitored Advertisers List command complete */
    gMonAdvListCleared_c                                    = 0x56U, /*!< Clear Monitored Advertisers List command complete */
    gMonAdvEnabled_c                                        = 0x57U, /*!< Enable Monitoring Advertisers command complete */
    gMonAdvListSizeRead_c                                   = 0x58U, /*!< Read Monitored Advertisers List Size command complete */
    gDebugNbuFatalError_c                                   = 0x59U, /*!< Vendor specific debugging event signaling NBU fatal error */
    gLeSetDefaultConnectionSubrateParametersSetupComplete_c = 0x5AU, /*!< Default Connection Subrate Parameters.have been successfully set */
} gapGenericEventType_t;

/*! Internal Error Source - the command that triggered the error */
typedef enum {
    gHciCommandStatus_c = 0x00U,
    gCheckPrivateResolvableAddress_c = 0x01U,
    gVerifySignature_c = 0x02U,
    gAddNewConnection_c = 0x03U,
    gResetController_c = 0x04U,
    gSetEventMask_c = 0x05U,
    gReadLeBufferSize_c = 0x06U,
    gSetLeEventMask_c = 0x07U,
    gReadDeviceAddress_c = 0x08U,
    gReadLocalSupportedFeatures_c = 0x09U,
    gReadFilterAcceptListSize_c = 0x0AU,
    gClearFilterAcceptList_c = 0x0BU,
    gAddDeviceToFilterAcceptList_c = 0x0CU,
    gRemoveDeviceFromFilterAcceptList_c = 0x0DU,
    gCancelCreateConnection_c = 0x0EU,
    gReadRadioPower_c = 0x0FU,
    gSetRandomAddress_c = 0x10U,
    gCreateRandomAddress_c = 0x11U,
    gEncryptLink_c = 0x12U,
    gProvideLongTermKey_c = 0x13U,
    gDenyLongTermKey_c = 0x14U,
    gConnect_c = 0x15U,
    gDisconnect_c = 0x16U,
    gTerminatePairing_c = 0x17U,
    gSendPeripheralSecurityRequest_c = 0x18U,
    gEnterPasskey_c = 0x19U,
    gProvideOob_c = 0x1AU,
    gSendSmpKeys_c = 0x1BU,
    gWriteSuggestedDefaultDataLength_c = 0x1CU,
    gReadSuggestedDefaultDataLength_c = 0x1DU,
    gUpdateLeDataLength_c = 0x1EU,
    gEnableHostPrivacy_c = 0x1FU,
    gEnableControllerPrivacy_c = 0x20U,
    gLeScSendKeypressNotification_c = 0x21U,
    gLeScSetPeerOobData_c = 0x22U,
    gLeScGetLocalOobData_c = 0x23U,
    gLeScValidateNumericValue_c = 0x24U,
    gLeScRegeneratePublicKey_c = 0x25U,
    gLeSetResolvablePrivateAddressTimeout_c = 0x26U,
    gDefaultPairingProcedure_c = 0x27U,
    gLeControllerTest_c = 0x28U,
    gLeReadPhy_c = 0x29U,
    gLeSetPhy_c = 0x2AU,
    gSaveKeys_c = 0x2BU,
    gSetChannelMap_c = 0x2CU,
    gReadLocalSupportedCommands_c = 0x2DU,
    gEnableLdmTimer_c = 0x2EU,
    gRemoveAdvertisingSet_c = 0x2FU,
    gLePeriodicAdvSyncEstb_c = 0x30U,
    gLePeriodicAdvSyncLost_c = 0x31U,
    gLeRemoveDeviceFromPeriodicAdvList_c = 0x32U,
    gLeClearPeriodicAdvList_c = 0x33U,
    gLeAddDeviceToPeriodicAdvList_c = 0x34U,
    gLeReadNumOfSupportedAdvSets_c = 0x35U,
    gLeReadPeriodicAdvListSize_c = 0x36U,
    gLeReadMaxAdvDataLen_c = 0x37U,
    gPeriodicAdvCreateSync = 0x38U,
    gPeriodicAdvCancelSync = 0x39U,
    gPeriodicAdvTerminateSync = 0x3AU,
    gL2capRxPacket_c = 0x3BU,
    gExtAdvReportProcess_c = 0x3CU,
    gReadControllerLocalRPA_c = 0x3DU,
    gHciEventReceiveHandler_c = 0x3EU,
    gSetConnectionlessCteTransmitParams_c = 0x3FU,
    gSetConnectionlessCteTransmitEnable_c = 0x40U,
    gSetConnectionlessIqSamplingEnable_c = 0x41U,
    gReadAntennaInformation_c = 0x42U,
    gSetConnectionCteReceiveParams_c = 0x43U,
    gSetConnectionCteTransmitParams_c = 0x44U,
    gConnectionCteReqEnable_c = 0x45U,
    gConnectionCteRspEnable_c = 0x46U,
    gGenerateDHKeyV2_c = 0x47U,
    gModifySleepClockAccuracy_c = 0x48U,
    gPeriodicAdvRcvEnable_c = 0x49U,
    gPeriodicAdvSyncTransfer_c = 0x4AU,
    gPeriodicAdvSetInfoTransfer_c = 0x4BU,
    gSetPeriodicAdvSyncTransferParams_c = 0x4CU,
    gSetDefaultPeriodicAdvSyncTransferParams_c = 0x4DU,
    gEnhancedReadTransmitPowerLevel_c = 0x4EU,
    gReadRemoteTransmitPowerLevel_c = 0x4FU,
    gSetPathLossReportingParams_c = 0x50U,
    gSetPathLossReportingEnable_c = 0x51U,
    gSetTransmitPowerReportingEnable_c = 0x52U,
    gEattConnectionRequest_c = 0x53U,
    gEattConnectionAccept_c = 0x54U,
    gEattReconfigureRequest_c = 0x55U,
    gEattSendCreditsRequest_c = 0x56U,
    gEattDisconnectRequest_c = 0x57U,
    gEattL2caCancelConnection_c = 0x58U,
    gEattL2caSendLeFlowControlCredit_c = 0x59U,
    gEattL2caDisconnectLePsm_c = 0x5AU,
    gEattL2caHandleSendLeCbData_c = 0x5BU,
    gEattL2caHandleRecvLeCbData_c = 0x5CU,
    gEattL2caEnhancedReconfigureReq_c = 0x5DU,
    gEattL2caEnhancedCancelConnection_c = 0x5EU,
    gHciRecvFragmentOfPacket_c = 0x5FU,
    gHciDataDiscardedAlloc_c = 0x60U,               /*!< A memory allocation failure occurred in the HCI layer. Data was discarded. */
    gHciDataDiscardedInvalidStateParam_c = 0x61U,   /*!< A packet with an invalid parameter or length was received by the HCI layer. Data was discarded. */
    gGetConnParams_c = 0x62U,                       /*!< An error occurred during the Get Connection Params procedure */
    gHandover_c = 0x63U,                            /*!< An error occurred during the connection handover process */
    gHandoverGetLlContext_c = 0x64U,                /*!< An error occurred during the Handover Get LL Context procedure */
    gHandoverConnect_c = 0x65U,                     /*!< An error occurred during the Handover Connect procedure */
    gHandoverDisconnect_c = 0x66U,                  /*!< An error occurred during the Handover Disconnect procedure */
    gHandoverSetSkd_c = 0x67U,                      /*!< An error occurred during the Handover Set SKD procedure */
    gHandoverSuspendTransmitLlProcInProgress_c = 0x68U, /*!< Could not suspend transmit at LL level due to an LL procedure being in progress */
    gHandoverSuspendTransmitHostTxInProgress_c = 0x69U, /*!< Could not suspend transmit at Host level due to ongoing data transfers */
    gHandoverGetCsLlContext_c = 0x6AU,              /*!< An error occurred during the Handover Get CS LL Context procedure */
    gHandoverSetCsLlContext_c = 0x6BU,              /*!< An error occurred during the Handover Set CS LL Context procedure */
    gHandoverUpdateConnParams_c = 0x6CU,            /*!< An error occurred during the Handover Update Connection Parameters procedure */
    gReadRemoteVersionInfo_c = 0x6DU,
    gLeSetSchedulerPriority_c = 0x6EU,
    gLeSetHostFeature_c = 0x6FU,
    gSetExtAdvDecisionData_c = 0x70U,
    gSetDecisionInstructions_c = 0x71U,
    gSetExpmSupportedFeatures_c = 0x72U,
    gHandoverSuspendTransmitHciTx_c = 0x73U,        /*!< Could not suspend transmit due to HCI tx error */
    gHandoverResumeTransmitHciTx_c = 0x74U,         /*!< Could not resume transmit due to HCI tx error */
    gHandoverAnchorNotifHciTx_c = 0x75U,            /*!< Could not enable/disable Anchor Notification due to HCI tx error */
    gHandoverAnchorSearchStartHciTx_c = 0x76U,      /*!< Could not start Anchor Search due to HCI tx error */
    gHandoverAnchorSearchStopHciTx_c = 0x77U,       /*!< Could not stop Anchor Search due to HCI tx error */
    gHandoverTimeSyncTxHciTx_c = 0x78U,             /*!< Could not start/stop Anchor Time Sync transmit due to HCI tx error */
    gHandoverTimeSyncRxHciTx_c = 0x79U,             /*!< Could not start/stop Anchor Time Receive transmit due to HCI tx error */
    gHandoverSuspendTransmitLl_c = 0x7AU,           /*!< Handover Suspend Transmit Link Layer error */
    gHandoverGetTimeLl_c = 0x7BU,                   /*!< Handover Get Time Link Layer error */
    gHandoverResumeTransmitLl_c = 0x7CU,            /*!< Handover Resume Transmit Link Layer error */
    gHandoverAnchorNotifLl_c = 0x7DU,               /*!< Handover Anchor Notification Link Layer error */
    gHandoverAnchorSearchStartLl_c = 0x7EU,         /*!< Handover Anchor Search Start Link Layer error */
    gHandoverAnchorSearchStopLl_c = 0x7FU,          /*!< Handover Anchor Search Stop Link Layer error */
    gHandoverTimeSyncTxLl_c = 0x80U,                /*!< Handover Time Sync Transmit Link Layer error */
    gHandoverTimeSyncRxLl_c = 0x81U,                /*!< Handover Time Sync Receive Link Layer error */
    gHandoverSetLlPendingData_c = 0x82U,            /*!< Handover Set LL Pending Data error */
    gLeChannelOverride_c = 0x83U,                   /*!< LE Channel Override command unsuccessful */
    gLeSetPeriodicAdvParamsV2_c = 0x84U,
    gLeSetPeriodicAdvSubeventData_c = 0x85U,
    gLeSetPeriodicAdvResponseData_c = 0x86U,
    gLeSetPeriodicSyncSubevent_c = 0x87U,
    gLePeriodicAdvResponseReport_c = 0x88U,
    gHandoverApplyConnectionUpdateProcedure_c = 0x89U,  /*!< An error occurred during the Handover Apply Connection Update procedure */
    gVendorUnitaryTest_c = 0x8AU,                   /*!< An error occurred during the Vendor Unitary Test procedure */
    gSetDataRelatedAddressChanges_c = 0x8BU,        /*!< An error occurred during the Set Data Related Address procedure */
    gLePeriodicAdvUpdateSync_c = 0x8CU,             /*!< An error occurred during the Le Periodic Adv Update Sync procedure */
    gReadAllLocalSupportedFeatures_c = 0x8DU,       /*!< An error occurred during the LE Read All Local Supported Features command */
    gAddDeviceToMonAdvList_c = 0x8EU,               /*!< An error occurred during the Add Device To Monitored Advertisers List */
    gRemoveDeviceFromMonAdvList_c = 0x8FU,          /*!< An error occurred during the Remove Device From Monitored Advertisers List */
    gClearMonAdvList_c = 0x90U,                     /*!< An error occurred during the Clear Monitored Advertisers List */
    gEnableMonAdv_c = 0x91U,                        /*!< An error occurred during the Enable Monitoring Advertisers */
    gReadMonAdvListSize_c = 0x92U,                  /*!< An error occurred during the Read Monitored Advertisers List Size */
    gSetDefaultConnectionSubrateParameters_c = 0x93U,/*!< An error occurred while setting the Default Connection Subrate Parameters*/
    gConnectionSubrateRequest_c = 0x94U,             /*!< An error occurred during the Connection Subrate procedure */
} gapInternalErrorSource_t;

/*! Internal Error Event Data */
typedef struct {
    bleResult_t                 errorCode;          /*!< Host Stack error code. */
    gapInternalErrorSource_t    errorSource;        /*!< The command that generated the error; useful when it is not obvious from the error code. */
    uint16_t                    hciCommandOpcode;   /*!< Only for errorSource = gHciCommandStatus_c; the HCI Command that received an error status. */
} gapInternalError_t;

/*! Controller Test Event Type */
typedef enum
{
    gControllerReceiverTestStarted_c,
    gControllerTransmitterTestStarted_c,
    gControllerTestEnded_c,
} gapControllerTestEventType_t;

/*! Controller Test Event */
typedef struct {
    gapControllerTestEventType_t    testEventType;
    uint16_t                        receivedPackets; /* Valid only if testEventType == gControllerTestEnded_c and the test was a Receiver Test. */
} gapControllerTestEvent_t;

/*! Le All Phys Preferences flags. */
typedef enum {
    gLeTxPhyNoPreference_c        = BIT0,   /*!< Host has no preference for Tx Phy */
    gLeRxPhyNoPreference_c        = BIT1,   /*!< Host has no preference for Rx Phy */
} gapLeAllPhyFlags_t;

/*! Le Tx/Rx Phys Preferences flags. */
typedef uint8_t gapLePhyFlags_t;

/*! Le Phys Options Preferences flags. */
typedef enum {
    gLeCodingNoPreference_c = 0x00U,         /*!< Host has no preference on the LE Coded Phy */
    gLeCodingS2_c           = 0x01U,         /*!< Host prefers to use S=2 on the LE Coded Phy */
    gLeCodingS8_c           = 0x02U,         /*!< Host prefers to use S=8 on the LE Coded Phy */
    gLeCodingS2Req_c        = 0x03U,         /*!< Host requires to use S=2 on the LE Coded Phy */
    gLeCodingS8Req_c        = 0x04U,         /*!< Host requires to use S=8 on the LE Coded Phy */
} gapLePhyOptionsFlags_t;

/*! Le Tx/Rx Phys. */
typedef uint8_t gapLePhyMode_t;
typedef enum  {
    gLePhy1M_c            = 0x01U,         /*!< Tx/Rx Phy on the connection is LE 1M */
    gLePhy2M_c            = 0x02U,         /*!< Tx/Rx Phy on the connection is LE 2M */
    gLePhyCoded_c         = 0x03U,         /*!< Tx/Rx Phy on the connection is LE Coded */
} gapLePhyMode_tag;

/*! Phy Event Type */
typedef enum
{
    gPhySetDefaultComplete_c  = 0x00U,               /*!< Gap_LeSetPhy default mode was successful */
    gPhyRead_c                = 0x01U,               /*!< Gap_LeReadPhy return values */
    gPhyUpdateComplete_c      = 0x02U,               /*!< Gap_LeSetPhy return values for a connection or an update occurred */
} gapPhyEventType_t;

/*! Phy Event */
typedef struct {
    gapPhyEventType_t    phyEventType;
    deviceId_t           deviceId;      /* Valid only if phyEventType == gPhyRead_c or gPhyUpdateComplete_c. */
    uint8_t              txPhy;         /* Valid only if phyEventType == gPhyRead_c or gPhyUpdateComplete_c. */
    uint8_t              rxPhy;         /* Valid only if phyEventType == gPhyRead_c or gPhyUpdateComplete_c. */
} gapPhyEvent_t;

/*! Controller Enhanced Notification Event Type */
typedef uint16_t bleNotificationEventType_t;
typedef enum
{
    gNotifEventNone_c                   =  0x00,         /*!< No enhanced notification event enabled */
    gNotifConnEventOver_c               =  BIT0,         /*!< Connection event over */
    gNotifConnRxPdu_c                   =  BIT1,         /*!< Connection Rx PDU */
    gNotifAdvEventOver_c                =  BIT2,         /*!< Advertising event over */
    gNotifAdvTx_c                       =  BIT3,         /*!< Advertising ADV transmitted */
    gNotifAdvScanReqRx_c                =  BIT4,         /*!< Advertising SCAN REQ Rx */
    gNotifAdvConnReqRx_c                =  BIT5,         /*!< Advertising CONN REQ Rx */
    gNotifScanEventOver_c               =  BIT6,         /*!< Scanning event over */
    gNotifScanAdvPktRx_c                =  BIT7,         /*!< Scanning ADV PKT Rx */
    gNotifScanRspRx_c                   =  BIT8,         /*!< Scanning SCAN RSP Rx */
    gNotifScanReqTx_c                   =  BIT9,         /*!< Scanning SCAN REQ Tx */
    gNotifConnCreated_c                 =  BIT10,        /*!< Connection created */
    gNotifChannelMatrix_c               =  BIT11,        /*!< Enable channel status monitoring (KW37 only) */
    gNotifPhyReq_c                      =  BIT12,        /*!< Phy Req Pdu ack received (KW37 only) */
    gNotifConnChannelMapUpdate_c        =  BIT13,        /*!< Channel  map update */
    gNotifConnInd_c                     =  BIT14,        /*!< Connect indication */
    gNotifPhyUpdateInd_c                =  BIT15,        /*!< Phy update indication */
} bleNotificationEventType_tag;

/*! Controller Enhanced Notification Event */
typedef struct {
    bleNotificationEventType_t eventType;       /*!< Enhanced notification event type */
    deviceId_t                 deviceId;        /*!< Device id of the peer, valid for connection events */
    int8_t                     rssi;            /*!< RSSI, valid for Rx event types */
    uint8_t                    channel;         /*!< Channel, valid for conn event over or Rx/Tx events */
    uint16_t                   ce_counter;      /*!< Connection event counter, valid for conn event over or Conn Rx event */
    bleResult_t                status;          /*!< Status of the request to select which events to be enabled/disabled */
    uint32_t                   timestamp;       /*!< Timestamp in microseconds, valid for Conn Rx event and Conn Created event */
    uint8_t                    adv_handle;      /*!< Advertising Handle, valid for advertising events, if multiple ADV sets supported */
    bleDeviceAddress_t         scanned_addr;    /*!< Scanned address, valid for gNotifScanAdvPktRx_c event, all-zeroes otherwise */
} bleNotificationEvent_t;

/*! gInitializationComplete_c event data */
typedef struct {
    leSupportedFeatures_t supportedFeatures;
    uint8_t               leExtendedFeatures[gLeExtendedFeaturesSize_c];
    uint16_t maxAdvDataSize;
    uint8_t  numOfSupportedAdvSets;
    uint8_t  periodicAdvListSize;
}gapInitComplete_t;

/*! Bond Created Event */
typedef struct bleBondCreatedEvent_tag {
    uint8_t             nvmIndex;    /*!< NVM index for the new created bond */
    bleAddressType_t    addressType; /*!< Public or Random (static) address of the bond */
    bleDeviceAddress_t  address;     /*!< Address of the bond */
} bleBondCreatedEvent_t;

/*! Address Ready Event */
typedef struct gapAddrReadyEvent_t_tag {
    bleDeviceAddress_t  aAddress;   /*!< Generated device address */
    uint8_t             advHandle;  /*!< Advertising set handle if the generated device address will be used on an extended set.
                                         Reserved value 0xFF for other purposes: legacy advertising or scanning and initiating address. */
} gapAddrReadyEvent_t;

typedef struct
{
    uint8_t     switchingSupportedAodTransmission:      1; /*!< Bit to mark that 1 us switching is supported for AoD transmission. */
    uint8_t     samplingSupportedAodReception:          1; /*!< Bit to mark that 1 us sampling is supported for AoD reception. */
    uint8_t     switchingSamplingSupportedAoaReception: 1; /*!< Bit to mark that 1 us switching and sampling is supported
                                                                for AoA reception. */
    uint8_t     reserved:                               5; /*!< Reserved for future use. */
} bleSupportedSwitchingSamplingRates_t;

/*! Antenna Information Read Event */
typedef struct bleAntennaInformation_tag {
    bleSupportedSwitchingSamplingRates_t    supportedSwitchingSamplingRates;
    uint8_t                                 numAntennae;
    uint8_t                                 maxSwitchingPatternLength;
    uint8_t                                 maxCteLength;
} bleAntennaInformation_t;

typedef struct periodicAdvSyncTransferEvent_tag {
    deviceId_t deviceId;
    bleResult_t status;
} periodicAdvSyncTransferEvent_t;

typedef struct periodicAdvSetInfoTransferEvent_tag {
    deviceId_t deviceId;
    bleResult_t status;
} periodicAdvSetInfoTransferEvent_t;

typedef struct periodicAdvSetSyncTransferParamsEvent_tag {
    deviceId_t deviceId;
    bleResult_t status;
} periodicAdvSetSyncTransferParamsEvent_t;

typedef struct gapSyncTransferReceivedEventData_tag
{
    bleResult_t                     status;
    deviceId_t                      deviceId;           /*!< Id of the connected device from which we received periodic advertising sync transfer */
    uint16_t                        serviceData;        /*!< A value provided by the peer device */
    uint16_t                        syncHandle;         /*!< Sync Handle identifying the periodic advertising train */
    uint8_t                         advSID;             /*!< Value of the Advertising SID used to advertise the periodic advertising */
    bleAddressType_t                advAddressType;     /*!< Device's advertising address type. */
    bleDeviceAddress_t              advAddress;         /*!< Device's advertising address. */
    gapLePhyMode_t                  advPhy;             /*!< PHY used for advertising */
    uint16_t                        periodicAdvInt;     /*!< Periodic advertising interval */
    bleCentralClockAccuracy_t       advClockAccuracy;   /*!< Advertiser clock accuracy */
    uint8_t                         numSubevents;        /*!< Only relevant for v2 of the event, otherwise value is 0 */
    uint8_t                         subeventInterval;    /*!< Only relevant for v2 of the event, otherwise value is 0 */
    uint8_t                         responseSlotDelay;   /*!< Only relevant for v2 of the event, otherwise value is 0 */
    uint8_t                         responseSlotSpacing; /*!< Only relevant for v2 of the event, otherwise value is 0 */
} gapSyncTransferReceivedEventData_t;

typedef struct getConnParams_tag
{
    uint16_t        connectionHandle;   /*!< Connection identifier */
    uint32_t        ulTxAccCode;        /*!< Access address */
    uint8_t         aCrcInitVal[3U];    /*!< CRC init */
    uint16_t        uiConnInterval;     /*!< Connection interval (unit 1.25ms) */
    uint16_t        uiSuperTO;          /*!< Supervision timeout (unit 10ms) */
    uint16_t        uiConnLatency;      /*!< Latency (unit connection interval) */
    uint8_t         aChMapBm[5U];       /*!< Channel map (FF FF FF FF 1F if all channels are used) */
    uint8_t         ucChannelSelection; /*!< Hop algorithm (0/1 for hop algorithm #1 or #2) */
    uint8_t         ucHop;              /*!< Hop increment */
    uint8_t         ucUnMapChIdx;       /*!< Unmapped channel index (used only for hop algorithm #1) */
    uint8_t         ucCentralSCA;       /*!< Sleep clock accuracy (0 to 7) */
    uint8_t         ucRole;             /*!< Role (0 for central and 1 for peripheral) */
    uint8_t         aucRemoteMasRxPHY;  /*!< TX/RX PHY */
    uint8_t         seqNum;             /*!< Sequence number; b1 and b0: the latest received SN and NESN; b5 and b4: the latest transmitted SN and NESN; other fields are reserved. */
    uint16_t        uiConnEvent;        /*!< Current connection event counter */
    uint32_t        ulAnchorClk;        /*!< Slot of the anchor point timing of the connection event */
    uint16_t        uiAnchorDelay;      /*!< Slot offset of the anchor point timing of the connection event */
    uint32_t        ulRxInstant;        /*!< Last successful access address reception instant (unit 625us) */
} getConnParams_t;

typedef struct handoverGetTime_tag
{
    bleResult_t                     status;     /*!< Command status */
    uint32_t                        slot;       /*!< LL timing slot counter (unit 625us) */
    uint16_t                        us_offset;  /*!< LL timing slot offset (0 to 624, unit us) */
} handoverGetTime_t;

typedef struct handoverAnchorSearchStart_tag
{
    bleResult_t                     status;             /*!< Command status */
    uint16_t                        connectionHandle;   /*!< Connection identifier */
} handoverAnchorSearchStart_t;

typedef struct handoverAnchorSearchStop_tag
{
    bleResult_t                     status;             /*!< Command status */
    uint16_t                        connectionHandle;   /*!< Connection identifier */
} handoverAnchorSearchStop_t;

typedef struct handoverConnect_tag
{
    bleResult_t status;             /*!< Command status */
    uint16_t    connectionHandle;   /*!< Connection identifier */
} handoverConnect_t;

typedef struct handoverGetData_tag
{
    bleResult_t     status; /*!< Command status */
    uint32_t        *pData; /*!< Connection handover data */
} handoverGetData_t;

typedef struct handoverSetData_tag
{
    bleResult_t     status; /*!< Command status */
    uint32_t        *pData; /*!< Connection handover data */
} handoverSetData_t;

typedef struct handoverGetCsLlContext_tag
{
    bleResult_t  status;                                            /*!< Command status */
    uint16_t     responseMask;                                      /*!< LL context bitmap indicating the Channel Sounding context */
    uint8_t      llContextLength;                                   /*!< Context data length */
    uint8_t      llContext[gVendorHandoverMaxCsLlContextSize_c];    /*!< LL Context */
} handoverGetCsLlContext_t;

typedef struct handoverAnchorMonitorEvent_tag
{
    uint16_t connectionHandle;      /*!< Connection identifier */
    uint16_t connEvent;             /*!< Current connection event counter */
    int8_t   rssiRemote;            /*!< RSSI of the packet from the remote device (+127 if not available) */
    uint8_t  lqiRemote;             /*!< LQI (Link Quality Indicator) of the packet from the remote device */
    uint8_t  statusRemote;          /*!< Status of the packet from the remote device; b0: NESN; b1: SN; b2: CRC status valid (1) or invalid (0); b3: RSSI status valid (1) or invalid (0)*/
    int8_t   rssiActive;            /*!< RSSI of the packet from the active device (+127 if not available) */
    uint8_t  lqiActive;             /*!< LQI (Link Quality Indicator) of the packet from the active device */
    uint8_t  statusActive;          /*!< Status of the packet from the active device; b0: NESN; b1: SN; b2: CRC status valid (1) or invalid (0); b3: RSSI status valid (1) or invalid (0)*/
    uint32_t anchorClock625Us;      /*!< Slot of the anchor point timing of the connection event */
    uint16_t anchorDelay;           /*!< Slot offset of the anchor point timing of the connection event */
    uint8_t  chIdx;                 /*!< BLE channel index */
    uint8_t  ucNbReports;           /*!< Number of remaining reports including the current one; 0: continuous reporting; 1-255: remaining number of reports to receive including the current one */
} handoverAnchorMonitorEvent_t;

typedef struct handoverAnchorMonitorPacketEvent_tag
{

    uint8_t  packetCounter;         /*!< Packet counter, incremented for each event */
    uint16_t connectionHandle;      /*!< Connection identifier */
    uint8_t  statusPacket;          /*!< Status of the packet */
    uint8_t  phy;                   /*!< PHY (0/1/2/3 for 1M/2M/LR S8/LR S2) */
    uint8_t  chIdx;                 /*!< BLE channel index */
    int8_t   rssiPacket;            /*!< RSSI of the packet (+127 if not available) */
    uint8_t  lqiPacket;             /*!< LQI (Link Quality Indicator) of the packet */
    uint16_t connEvent;             /*!< Current connection event counter */
    uint32_t anchorClock625Us;      /*!< Slot value of packet start time (in 625us unit) */
    uint16_t anchorDelay;           /*!< Slot offset value of packet start time (in 1us unit) */
    uint8_t  ucNbConnIntervals;     /*!< Number of remaining connection intervals to monitor including the current one; 0: continuous reporting; 1-255: remaining number of reports to receive including the current one */
    uint8_t  pduSize;               /*!< PDU length */
    uint8_t  *pPdu;                 /*!< Full or first part of the data PDU including: PDU header, payload if present, MIC if present, CRC */
} handoverAnchorMonitorPacketEvent_t;

typedef struct handoverAnchorMonitorPacketContinueEvent_tag
{
    uint8_t  packetCounter;     /*!< Packet counter, incremented for each event */
    uint16_t connectionHandle;  /*!< Connection identifier */
    uint8_t  pduSize;           /*!< PDU length */
    uint8_t  *pPdu;             /*!< Full or first part of the data PDU including: PDU header, payload if present, MIC if present, CRC */
} handoverAnchorMonitorPacketContinueEvent_t;

typedef struct handoverConnectionUpdateProcedureEvent_tag
{
    uint16_t        connectionHandle;       /*!< Connection identifier */
    uint8_t         winSize;                /*!< Used to indicate the transmitWindowSize value as: transmitWindowSize = winSize * 1.25 ms */
    uint16_t        winOffset;              /*!< Used to indicate the transmitWindowOffset value as: transmitWindowOffset = winOffset * 1.25 ms */
    uint16_t        interval;               /*!< Used to indicate the connInterval value, as: connInterval = interval * 1.25 ms */
    uint16_t        latency;                /*!< Used to indicate the connPeripheralLatency value as: connPeripheralLatency = latency */
    uint16_t        timeout;                /*!< Used to indicate the connSupervisionTimeout value as: connSupervisionTimeout = timeout * 10 ms */
    uint16_t        instant;                /*!< Connection event counter value indicating when the new connection parameters are applied */
    uint16_t        currentEventCounter;    /*!< Current connection event counter */
} handoverConnectionUpdateProcedureEvent_t;

typedef struct handoverTimeSyncEvent_tag
{
    uint32_t txClkSlot;     /*!< Transmitter packet start time in slot (625 us) */
    uint16_t txUs;          /*!< Transmitter packet start time offset inside the slot (0 to 624 us) */
    uint32_t rxClkSlot;     /*!< Receiver packet start time in slot (625 us) */
    uint16_t rxUs;          /*!< Receiver packet start time offset inside the slot (0 to 624 us) */
    uint8_t  rssi;          /*!< RSSI */
} handoverTimeSyncEvent_t;

typedef struct handoverConnParamUpdateEvent_tag
{
    uint8_t         status;             /*!< Status indicating the event trigger source: bit 0: remote SN^NESN toggled; bit 1: local SN^NESN toggled; bit 2: RFU; bit 3: phy update procedure; bit 4: channel map update procedure */
    uint16_t        connectionHandle;   /*!< Connection identifier */
    uint32_t        ulTxAccCode;        /*!< Access address */
    uint8_t         aCrcInitVal[3U];    /*!< CRC init */
    uint16_t        uiConnInterval;     /*!< Connection interval (unit 1.25ms) */
    uint16_t        uiSuperTO;          /*!< Supervision timeout (unit 10ms) */
    uint16_t        uiConnLatency;      /*!< Latency (unit connection interval) */
    uint8_t         aChMapBm[5U];       /*!< Channel map (FF FF FF FF 1F if all channels are used) */
    uint8_t         ucChannelSelection; /*!< Hop algorithm (0/1 for hop algorithm #1 or #2) */
    uint8_t         ucHop;              /*!< Hop increment */
    uint8_t         ucUnMapChIdx;       /*!< Unmapped channel index (used only for hop algorithm #1) */
    uint8_t         ucCentralSCA;       /*!< Sleep clock accuracy (0 to 7) */
    uint8_t         ucRole;             /*!< Role (0 for central and 1 for peripheral) */
    uint8_t         aucRemoteMasRxPHY;  /*!< TX/RX PHY */
    uint8_t         seqNum;             /*!< Sequence number; b1 and b0: the latest received SN and NESN; b5 and b4: the latest transmitted SN and NESN; other fields are reserved. */
    uint16_t        uiConnEvent;        /*!< Current connection event counter */
    uint32_t        ulAnchorClk;        /*!< Slot of the anchor point timing of the connection event */
    uint16_t        uiAnchorDelay;      /*!< Slot offset of the anchor point timing of the connection event */
    uint32_t        ulRxInstant;        /*!< Last successful access address reception instant (unit 625us) */
} handoverConnParamUpdateEvent_t;

typedef struct handoverSuspendTransmitCompleteEvent_tag
{
    uint16_t    connectionHandle;  /*!< Connection identifier */
    uint8_t     noOfPendingAclPackets; /*!< Number of pending ACL packets (including the packet being transmitted partially or fully but not fully acked)*/
    uint16_t    sizeOfPendingAclPackets; /*!< Pending ACL packet data size including all the data in the oldest packet:
                                                        - data transmitted but not acked in the oldest ACL packet
                                                        - data transmitted and acked in the oldest ACL packet */
    uint16_t    sizeOfDataTxInOldestPacket; /*!< Size of data transmitted and acked in the oldest ACL packet */
    uint8_t     sizeOfDataNAckInOldestPacket; /*!< Size of data transmitted but not acked in the oldest ACL packet */
} handoverSuspendTransmitCompleteEvent_t;

typedef struct handoverResumeTransmitCompleteEvent_tag
{
    uint16_t connectionHandle;  /*!< Connection identifier */
} handoverResumeTransmitCompleteEvent_t;

typedef struct handoverUpdateConnParams_tag
{
    bleResult_t status;             /*!< Command status */
    uint16_t    connectionHandle;   /*!< Connection identifier */
} handoverUpdateConnParams_t;

typedef struct handoverApplyConnectionUpdateProcedure_tag
{
    uint16_t    connectionHandle;   /*!< Connection identifier */
} handoverApplyConnectionUpdateProcedure_t;

typedef struct handoverAnchorNotificationStateChanged_tag
{
    uint16_t connectionHandle;  /*!< Connection identifier */
} handoverAnchorNotificationStateChanged_t;

typedef struct handoverLlPendingDataIndication_tag
{
    uint16_t    dataSize;   /*!< Pending HCI ACL data packet size */
    uint8_t     *pData;     /*!< Pointer to message containing the HCI ACL data packet. Should be free by the application using MSG_Free() */
} handoverLlPendingDataIndication_t;

typedef struct {
    uint8_t              aDHKey[gDHKeySize_c];
} gapLEGenerateDhKeyCompleteEvent_t;

typedef struct gapRemoteVersionInfoRead_tag
{
    bleResult_t status;
    deviceId_t  deviceId;
    uint8_t     version;
    uint16_t    manufacturerName;
    uint16_t    subversion;
} gapRemoteVersionInfoRead_t;

typedef struct gapLlSkdReport_tag
{
    deviceId_t deviceId;        /*!< Peer device identifier */
    uint8_t aSKD[gSkdSize_c];   /*!< LL SKD */
} gapLlSkdReport_t;

typedef struct vendorUnitaryTestEvent_tag
{
    bleResult_t status;
    uint8_t     paramLength;
    uint8_t     aParam[gVendorUnitaryTestSize_c];
} vendorUnitaryTestEvent_t;

typedef struct vendorSpecificDbgEvent_tag
{
    uint8_t     dataSize;
    uint8_t     pData[gVendorDebugEventSize_c];
} vendorSpecificDbgEvent_t;

/*! Generic Event Structure = type + data */
typedef struct {
    gapGenericEventType_t  eventType;       /*!< Event type. */
    union {
        gapInternalError_t          internalError;              /*!< Data for the gInternalError_c event. The error that has occurred and the command that triggered it. */
        uint8_t                     filterAcceptListSize;              /*!< Data for the gFilterAcceptListSizeRead_c event. The size of the Filter Accept List. */
        bleDeviceAddress_t          aAddress;                   /*!< Data for the gPublicAddressRead_c event. Contains the requested device address. */
        gapAddrReadyEvent_t         addrReady;                  /*!< Data for the gRandomAddressReady_c event. Contains the generated device address and advertising handle if applicable (0xFF otherwise). */
        uint8_t                     advHandle;                  /*!< Data for the gRandomAddressSet_c event. Contains the handle of the configured advertising set or 0xFF for legacy advertising. */
        bleResult_t                 setupFailError;             /*!< Data for the gAdvertisingSetupFailed_c event. The error that occurred during the advertising setup. */
        int8_t                      advTxPowerLevel_dBm;        /*!< Data for the gExtAdvertisingParametersSetupComplete_c and gAdvTxPowerLevelRead_c events. Value in dBm. */
        bool_t                      verified;                   /*!< Data for the gPrivateResolvableAddressVerified_c event. TRUE if the PRA was resolved with the given IRK. */
        gapLeScOobData_t            localOobData;               /*!< Data for the gLeScLocalOobData_c event. Contains local OOB data for LESC Pairing. */
        bool_t                      newHostPrivacyState;        /*!< Data for the gHostPrivacyStateChanged_c event. TRUE if enabled, FALSE if disabled. */
        bool_t                      newControllerPrivacyState;  /*!< Data for the gControllerPrivacyStateChanged_c event. TRUE if enabled, FALSE if disabled. */
        gapControllerTestEvent_t    testEvent;                  /*!< Data for the gControllerTestEvent_c event. Contains test event type and received packets. */
        bleResult_t                 txPowerLevelSetStatus;      /*!< Data for the gTxPowerLevelSetComplete_c event. Status of the set request. */
        gapPhyEvent_t               phyEvent;                   /*!< Data for the gLePhyEvent_c event. Contains Tx and Rx Phy for a connection. */
        deviceId_t                  deviceId;                   /*!< Data for the gTxEntryAvailable_c event. */
        gapInitComplete_t           initCompleteData;           /*!< Data for the gInitializationComplete_c event. Contains the supported features, number of advertising sets
                                                                     and the size of the periodic advertiser list */
        bleNotificationEvent_t      notifEvent;                 /*!< Data for the gControllerNotificationEvent_c event. Contains status and adv/scan/conn event data. */
        bleBondCreatedEvent_t       bondCreatedEvent;           /*!< Data for the gBondCreatedEvent_c event. Contains the NVM index and the address of the bond. */
        bleDeviceAddress_t          aControllerLocalRPA;        /*!< Data for the gControllerLocalRPARead_c event. Contains the requested device address. */
        getConnParams_t             getConnParams;              /*!< Data for the gGetConnParamsComplete_c event */

        /* BLE 5.1: AoA/AoD */
        uint16_t                    syncHandle;                 /*!< Data for the gConnectionlessIqSamplingStateChanged_c event. Contains the sync handle for the advertising train. */
        bleAntennaInformation_t     antennaInformation;         /*!< Data for the gAntennaInformationRead_c. Contains antenna information. */
        /* Periodic Advertisement Sync Transfer */
        bleResult_t                             perAdvSyncTransferEnable;                   /*!< Data for the gPeriodicAdvRecvEnableComplete_c event */
        periodicAdvSyncTransferEvent_t          perAdvSyncTransfer;                         /*!< Data for the gPeriodicAdvSyncTransferComplete_c event */
        periodicAdvSetInfoTransferEvent_t       perAdvSetInfoTransfer;                      /*!< Data for the gPeriodicAdvSetInfoTransferComplete_c event */
        periodicAdvSetSyncTransferParamsEvent_t perAdvSetSyncTransferParams;                /*!< Data for the gSetPeriodicAdvSyncTransferParamsComplete_c event */
        bleResult_t                             perAdvSetDefaultPerAdvSyncTransferParams;   /*!< Data for the gSetDefaultPeriodicAdvSyncTransferParamsComplete_c event */
        gapSyncTransferReceivedEventData_t      perAdvSyncTransferReceived;                 /*!< Data for the gPeriodicAdvSyncTransferReceived_c event */
        gapLEGenerateDhKeyCompleteEvent_t       leGenerateDhKeyCompleteEvent;               /*!< Data for gConnEvtLeGenerateDhKeyComplete_c: new key value */
        /* PAWR */
        uint16_t                                pawrSyncHandle;                             /*!< Data for gPeriodicAdvSetResponseDataComplete_c and gPeriodicSyncSubeventComplete_c */
        uint8_t                                 pawrAdvHandle;                              /*!< Data for gPeriodicAdvSetSubeventDataComplete_c */
        /* Handover */
        handoverGetData_t                         handoverGetData;                          /*!< Data for the gHandoverGetComplete_c event */
        handoverSetData_t                         handoverSetData;                          /*!< Data for the gHandoverSetComplete_c event */
        handoverGetCsLlContext_t                  handoverGetCsLlContext;                   /*!< Data for the gHandoverGetCsLlContextComplete_c event */
        handoverGetTime_t                         handoverGetTime;                          /*!< Data for the gHandoverGetTime_c event */
        handoverConnect_t                         handoverConnect;                          /*!< Data for the gHandoverConnect_c event */
        handoverAnchorSearchStart_t               handoverAnchorSearchStart;                /*!< Data for the gHandoverAnchorSearchStarted_c event */
        handoverAnchorSearchStop_t                handoverAnchorSearchStop;                 /*!< Data for the gHandoverAnchorSearchStopped_c event */
        handoverAnchorMonitorEvent_t              handoverAnchorMonitor;                    /*!< Data for the gHandoverAnchorMonitorEvent_c event */
        handoverTimeSyncEvent_t                   handoverTimeSync;                         /*!< Data for the gHandoverTimeSyncEvent_c event */
        handoverSuspendTransmitCompleteEvent_t    handoverSuspendTransmitComplete;          /*!< Data for the gHandoverSuspendTransmitComplete_c event */
        handoverResumeTransmitCompleteEvent_t     handoverResumeTransmitComplete;           /*!< Data for the gHandoverResumeTransmitComplete_c event */
        handoverAnchorNotificationStateChanged_t  handoverAnchorNotificationStateChanged;   /*!< Data for the gHandoverAnchorNotificationStateChanged_c event */
        handoverConnParamUpdateEvent_t            handoverConnParamUpdate;                  /*!< Data for the gHandoverConnParamUpdateEvent_c event */
        gapRemoteVersionInfoRead_t                gapRemoteVersionInfoRead;                 /*!< Data for the hciReadRemoteVersionInformationCompleteEvent_t event */
        gapLlSkdReport_t                          gapSkdReport;                             /*!< Data for the gLlSkdReportEvent_c event */
        handoverAnchorMonitorPacketEvent_t          handoverAnchorMonitorPacket;            /*!< Data for the gHandoverAnchorMonitorPacketEvent_c event. pPdu must be freed by the application */
        handoverAnchorMonitorPacketContinueEvent_t  handoverAnchorMonitorPacketContinue;    /*!< Data for the gHandoverAnchorMonitorPacketContinueEvent_c event. pPdu must be freed by the application */
        handoverUpdateConnParams_t                  handoverUpdateConnParams;               /*!< Data for the gHandoverUpdateConnParamsComplete_c event */
        handoverLlPendingDataIndication_t           handoverLlPendingDataIndication;        /*!< Data for the gHandoverLlPendingData_c event */
        handoverConnectionUpdateProcedureEvent_t    handoverConnectionUpdateProcedure;      /*!< Data for the gHandoverConnectionUpdateProcedureEvent_c event */
        handoverApplyConnectionUpdateProcedure_t    handoverApplyConnectionUpdateProcedure; /*!< Data for the gHandoverApplyConnectionUpdateProcedureComplete_c event */
        vendorUnitaryTestEvent_t                  unitaryTestData;                          /*!< Data for the gVendorUnitaryTestComplete_c event */
        uint8_t                                   monAdvListSize;                           /*!< Data for the gMonAdvListSizeRead_c event. The size of the Monitored Advertisers List. */
        vendorSpecificDbgEvent_t                  vendorDbgFatalError;                      /*!< Data for the gDebugNbuFatalError_c event. */
    } eventData;                            /*!< Event data, selected according to event type. */
} gapGenericEvent_t;

/*! Generic Callback prototype. */
typedef void (*gapGenericCallback_t)
(
    gapGenericEvent_t* pGenericEvent  /*!< Generic event. */
);

/*! Host-to-Controller API prototype. */
typedef bleResult_t (*hciHostToControllerInterface_t)
(
    hciPacketType_t packetType,     /*!< HCI Packet Type. */
    void* pPacket,                  /*!< Pointer to packet payload. */
    uint16_t packetSize             /*!< Packet payload size. */
);

typedef struct
{
    uint32_t raw[(gBleBondIdentityHeaderSize_c + 3U) / sizeof(uint32_t)];
} bleBondIdentityHeaderBlob_t;

typedef struct
{
    uint32_t raw[(gBleBondDataDynamicSize_c + 3U) / sizeof(uint32_t)];
} bleBondDataDynamicBlob_t;

typedef struct
{
    uint32_t raw[(gBleBondDataStaticSize_c + 3U) / sizeof(uint32_t)];
} bleBondDataStaticBlob_t;

typedef struct
{
    uint32_t raw[(gBleBondDataLegacySize_c + 3U) / sizeof(uint32_t)];
} bleBondDataLegacyBlob_t;

typedef struct
{
    uint32_t raw[(gBleBondDataDeviceInfoSize_c + 3U) / sizeof(uint32_t)];
} bleBondDataDeviceInfoBlob_t;

typedef struct
{
    uint32_t raw[(gBleBondDataDescriptorSize_c + 3U) / sizeof(uint32_t)];
}bleBondDataDescriptorBlob_t;

typedef struct
{
    union
    {
    uint32_t raw[(gcSmpMaxIrkBlobSize_c + 3U) / sizeof(uint32_t)];
    uint8_t pKey[gcSmpMaxIrkBlobSize_c];
    };
    bool_t keyGenerated;
} bleLocalKeysBlob_t;

typedef struct
{
    bleBondIdentityHeaderBlob_t     bondHeader;
    bleBondDataDynamicBlob_t        bondDataBlobDynamic;
    bleBondDataStaticBlob_t         bondDataBlobStatic;
    bleBondDataLegacyBlob_t         bondDataBlobLegacy;
    bleBondDataDescriptorBlob_t     bondDataDescriptors[gcGapMaximumSavedCccds_c];
    bleBondDataDeviceInfoBlob_t     bondDataBlobDeviceInfo;
}bleBondDataBlob_t;

typedef struct
{
    bleBondIdentityHeaderBlob_t     bondHeader;
    bleBondDataDynamicBlob_t        bondDataBlobDynamic;
    bleBondDataStaticBlob_t         bondDataBlobStatic;
    bleBondDataLegacyBlob_t         bondDataBlobLegacy;
    bleBondDataDeviceInfoBlob_t     bondDataBlobDeviceInfo;
    bleBondDataDescriptorBlob_t     bondDataDescriptors[1];
}bleBondDataRam_t;

typedef struct
{
    uint8_t     allowAoA:       1;  /*!< Bit to allow AoA CTE Response. */
    uint8_t     allowAoD1us:    1;  /*!< Bit to allow AoD CTE Response with 1 microsecond slots. */
    uint8_t     allowAoD2us:    1;  /*!< Bit to allow AoD CTE Response with 2 microsecond slots. */
    uint8_t     reserved:       5;  /*!< Reserved for future use. */
} bleCteAllowedTypesMap_t;

typedef enum
{
    gCteTransmitDisable_c    = 0x00,
    gCteTransmitEnable_c    = 0x01,
} bleCteTransmitEnable_t;

typedef enum
{
    gIqSamplingDisable_c   = 0x00,
    gIqSamplingEnable_c    = 0x01,
} bleIqSamplingEnable_t;

typedef enum
{
    gCteReqDisable_c   = 0x00,
    gCteReqEnable_c    = 0x01,
} bleCteReqEnable_t;

typedef enum
{
    gCteRspDisable_c   = 0x00,
    gCteRspEnable_c    = 0x01,
} bleCteRspEnable_t;

typedef uint8_t bleCteType_t;

typedef enum
{
    gCteTypeAoA_c     = 0x01, /*!< AoA Constant Tone Extension */
    gCteTypeAoD1us_c  = 0x02, /*!< AoD Constant Tone Extension with 1 microsecond slots */
    gCteTypeAoD2us_c  = 0x03, /*!< AoD Constant Tone Extension with 2 microsecond slots */
    gCteTypeNoCte_c   = 0xFF, /*!< No Constant Tone Extension */
} bleCteType_tag;

typedef struct bleSyncCteType_tag
{
    uint8_t     doNotSyncWithAoA:       1;  /*!< Bit for CTE type AoA. */
    uint8_t     doNotSyncWithAoD1us:    1;  /*!< Bit for CTE type AoD 1us. */
    uint8_t     doNotSyncWithAoD2us:    1;  /*!< Bit for CTE type AoD 2us. */
    uint8_t     doNotSyncWithType3:     1;  /*!< Bit for CTE type 3 (reserved). */
    uint8_t     doNotSyncWithoutCte:    1;  /*!< Bit for no CTE type. */
    uint8_t     reserved:               3;  /*!< Reserved for future use. */
} bleSyncCteType_t;

typedef uint8_t bleSlotDurations_t;

typedef enum
{
    gSlotDurations1us_c = 0x01, /*!< Switching and sampling slot durations are 1 microsecond each */
    gSlotDurations2us_c = 0x02  /*!< Switching and sampling slot durations are 2 microseconds each */
} bleSlotDurations_tag;

typedef uint8_t bleIqReportPacketStatus_t;

typedef enum
{
    gIqReportPacketStatusCorrectCrc_c             = 0x00, /*!< CRC was correct */
    gIqReportPacketStatusCrcIncorrectUsedLength_c = 0x01, /*!< CRC was incorrect and the Length and CTETime fields of the packet
                                                                     were used to determine sampling points */
    gIqReportPacketStatusCrcIncorrectUsedOther_c  = 0x02, /*!< CRC was incorrect but the Controller has determined the position and
                                                                     length of the Constant Tone Extension in some other way */
    gIqReportPacketStatusInsufficientResources_c  = 0xFF  /*!< Insufficient resources to sample (Channel_Index, CTE_Type, and Slot_Durations invalid). */
} bleIqReportPacketStatus_tag;

typedef uint8_t blePathLossThresholdZoneEntered_t;

typedef enum
{
    gPathLossThresholdLowZone_c         = 0x00,     /*!< Entered low zone. */
    gPathLossThresholdMiddleZone_c      = 0x01,     /*!< Entered middle zone. */
    gPathLossThresholdHighZone_c        = 0x02      /*!< Entered high zone. */
} blePathLossThresholdZoneEntered_tag;

typedef uint8_t bleTxPowerReportingReason_t;

typedef enum
{
    gLocalTxPowerChanged_c                      = 0x00,     /*!< Local transmit power changed. */
    gRemoteTxPowerChanged_c                     = 0x01,     /*!< Remote transmit power changed. */
    gReadRemoteTxPowerLevelCommandCompleted_c   = 0x02      /*!< HCI_LE_Read_Remote_Transmit_Power_Level command completed. */
} bleTxPowerReportingReason_tag;

typedef uint8_t blePowerControlPhyType_t;

typedef enum
{
    gPowerControlLePhy1M_c          = 0x01,     /*!< LE 1M Phy. */
    gPowerControlLePhy2M_c          = 0x02,     /*!< LE 2M Phy. */
    gPowerControlLePhyCodedS8_c     = 0x03,     /*!< LE Coded Phy with S=8 data coding. */
    gPowerControlLePhyCodedS2_c     = 0x04      /*!< LE Coded Phy with S=2 data coding. */
} blePowerControlPhyType_tag;

typedef struct
{
    uint8_t     minimum:    1;  /*!< Tx Power Level is at minimum level. */
    uint8_t     maximum:    1;  /*!< Tx Power Level is at maximum level. */
    uint8_t     reserved:   6;  /*!< Reserved for future use. */
} bleTxPowerLevelFlags_t;

typedef enum
{
    gPathLossReportingDisable_c   = 0x00,
    gPathLossReportingEnable_c    = 0x01,
} blePathLossReportingEnable_t;

typedef enum
{
    gTxPowerReportingDisable_c   = 0x00,
    gTxPowerReportingEnable_c    = 0x01,
} bleTxPowerReportingEnable_t;

typedef enum
{
    gDoNotUseEventCounter_c    = 0x00, /* Suspend TX as soon as possible */
    gUseEventCounter_c         = 0x01, /* Suspend TX at the suspension event counter */
    gSafeStopLlTx_c            = 0x02, /* Suspend TX only if no ongoing LL procedures */
    gSafeStopHostLlTx_c        = 0x03, /* Suspend TX only if no ongoing LL procedures
                                          and no ongoing Host data transfers */
} bleHandoverSuspendTransmitMode_t;

typedef enum
{
    gSuspendTxMode_c    = 0x00, /* Connection monitoring after source device suspended the TX */
    gRssiSniffingMode_c = 0x01, /* RSSI monitoring based on the SN for central/peripheral differentiation when the source device is in normal operation */
    gPacketMode_c       = 0x02, /* Report all packets together with information when the source device is in normal operation */
} bleHandoverAnchorSearchMode_t;

typedef enum
{
    gSendEventAfterRemoteSNChange_c = BIT0, /* Update the local SN for central/peripheral
                                             * differentiation and send the LE_Vendor_Conn_Param_Update event
                                             * after remote SN change. */
    gSendEventAfterLLProcUpdate_c   = BIT1, /* Send LE_Vendor_Conn_Param_Update event when the conn params
                                             * are updated following an LL procedure. */
} bleGetConnParamsMode_t;

typedef enum
{
    gAnchorNotificationDisable_c   = 0x00,
    gAnchorNotificationEnable_c    = 0x01,
} bleHandoverAnchorNotificationEnable_t;

typedef enum
{
    gTimeSyncDisable_c   = 0x00,
    gTimeSyncEnable_c    = 0x01,
} bleHandoverTimeSyncEnable_t;

typedef enum
{
    gTimeSyncDoNotStopWhenFound_c = 0x00,
    gTimeSyncStopWhenFound_c      = 0x01,
} bleHandoverTimeSyncStopWhenFound_t;

typedef enum
{
   gAdvIndexAscend_c  = 0x00U,
   gAdvIndexDescend_c = 0x01U,
   gAdvIndexUser_c    = 0x02U,
   gAdvIndexRandom_c  = 0x03U,
} bleAdvIndexType_t;
/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
extern const uint8_t  gBleMaxActiveConnections;
extern const uint16_t gcConnectionEventMinDefault_c;
extern const uint16_t gcConnectionEventMaxDefault_c;

extern const uint8_t  gBleEattMaxConnectionChannels;
extern const uint16_t gBleEattPsmMtu;

extern const uint8_t gMaxAdvReportQueueSize;

/*! Enables/Disables Hci Command Flow Control if the host lib supports this feature */
#if defined(gUnitTesting_d)
extern bool_t gUseHciCommandFlowControl;
#else /* gUnitTesting_d */
extern const bool_t gUseHciCommandFlowControl;
#endif /* gUnitTesting_d */



/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*! *********************************************************************************
* \brief  Performs initialization of the BLE Host stack.
*
* \param[in] genericCallback  Callback used to propagate GAP generic events to the application.
* \param[in] hostToControllerInterface  LE Controller uplink interface function pointer
*
* \return  gBleSuccess_c or error.
*
* \remarks Application must wait for the gInitializationComplete_c generic event.
*
********************************************************************************** */
bleResult_t Ble_HostInitialize
(
    gapGenericCallback_t            genericCallback,
    hciHostToControllerInterface_t  hostToControllerInterface
);

/*!*************************************************************************************************
*\brief performs De-Initialize the Bluetooth LE Host
*
*\param      none
*
*\retval     gBleSuccess_c or error.
*
* \remarks Application must wait for the gDeInitializationComplete_c generic event.
*
***************************************************************************************************/
bleResult_t Ble_HostDeInitialize(void);
/*! *********************************************************************************
* \brief  This is the BLE Host downlink interface function.
*
* \param[in] packetType     The type of the packet sent by the LE Controller
* \param[in] pHciPacket     Pointer to the packet sent by the LE Controller
* \param[in] packetSize     Number of bytes sent by the LE Controller
*
* \return gBleSuccess_c or gBleOutOfMemory_c
*
* \remarks This function must be registered as a callback by the LE Controller and called
* to send HCI packets (events and LE-U data) to the BLE Host.
*
********************************************************************************** */
bleResult_t Ble_HciRecv
(
    hciPacketType_t     packetType,
    void*               pHciPacket,
    uint16_t            packetSize
);

/*! *********************************************************************************
* \brief  This is the BLE Host downlink interface function to be used in ISR context.
*
* \param[in] packetType     The type of the packet sent by the LE Controller
* \param[in] pHciPacket     Pointer to the packet sent by the LE Controller
* \param[in] packetSize     Number of bytes sent by the LE Controller
*
* \return gBleSuccess_c or gBleOutOfMemory_c
*
* \remarks This function must be registered as a callback by the HCI Transport layer and called
* to send HCI packets from the controller (events and LE-U data) to the BLE Host.
*
********************************************************************************** */
bleResult_t Ble_HciRecvFromIsr
(
    hciPacketType_t     packetType,
    void*               pHciPacket,
    uint16_t            packetSize
);

/*! *********************************************************************************
* \brief  This function performs runtime checks to determine whether memory
* storage is properly configured for the application settings.
*
* \return True if memory is configured correctly False otherwise.
*
*
********************************************************************************** */
bool_t Ble_CheckMemoryStorage(void);
#ifdef __cplusplus
}
#endif

#endif /* _BLE_GENERAL_H_ */

/*! *********************************************************************************
* @}
********************************************************************************** */
