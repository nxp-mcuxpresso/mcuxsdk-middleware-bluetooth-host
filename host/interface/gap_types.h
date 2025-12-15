/*! *********************************************************************************
 * \addtogroup GAP
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

#ifndef GAP_TYPES_H
#define GAP_TYPES_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ble_general.h"
#include "SecLib.h"
#include "l2ca_cb_interface.h"

/*************************************************************************************
 *************************************************************************************
 * Public macros
 *************************************************************************************
 ************************************************************************************/
/*! Use this value as a parameter to the Gap_Disconnect(deviceId) function to
 * cancel any ongoing connection initiation, for example if the connection has timed out. */
#define gCancelOngoingInitiatingConnection_d    gInvalidDeviceId_c

/*! Mask to check if a Security Mode-and-Level is Mode 2 */
#define gMode_2_Mask_d  0x20U

/*
 * Useful macros for combining security requirements
 * - When two requirements of the same mode meet, the highest one has priority.
 * - When two requirements of different modes meet, the following table (Core v.4.1) applies:
 *
 * [M_L]   1_0       1_1       1_2
 *
 * 2_1     2_1       1_1       1_2
 *
 * 2_2     2_2       1_2       1_2
 *
 */

/*! Extracts the security level (see gapSecurityLevel_t) from the combined security mode-level (gapSecurityModeAndLevel_t). */
#define getSecurityLevel(modeLevel) ((uint8_t)(modeLevel) & 0x0FU)

/*! Extracts the security mode (see gapSecurityMode_t) from the combined security mode-level (gapSecurityModeAndLevel_t). */
#define getSecurityMode(modeLevel) ((uint8_t)(modeLevel) & 0xF0U)

#define isMode_2(modeLevel) ((modeLevel & gMode_2_Mask_d) != 0x00U)
#define isMode_1(modeLevel) (!isMode_2(modeLevel))
#define isSameMode(modeLevelA, modeLevelB) (isMode_1(modeLevelA) == isMode_1(modeLevelB))
#define addSameSecurityModes(modeLevelA,modeLevelB) \
    (((modeLevelA) > (modeLevelB)) ? (modeLevelA) : (modeLevelB))
#define addMode1AndMode2(mode1,mode2) \
    ((mode1 == (uint8_t)gSecurityMode_1_Level_1_c) ? \
     (mode2) : \
         ((((mode1) == (uint8_t)gSecurityMode_1_Level_2_c) && ((mode2) == (uint8_t)gSecurityMode_2_Level_1_c)) ? \
          ((uint8_t)gSecurityMode_1_Level_2_c) : \
          ((uint8_t)gSecurityMode_1_Level_3_c)))
#define addDifferentSecurityModes(modeLevelA,modeLevelB) \
    (isMode_1(modeLevelA) ? \
     (addMode1AndMode2(modeLevelA, modeLevelB)) : \
     (addMode1AndMode2(modeLevelB, modeLevelA)))

/*! The default (minimum) value for the LTK size */
#define gDefaultEncryptionKeySize_d   7U

/*! The maximum value for the LTK size */
#define gMaxEncryptionKeySize_d       16U

/*! The default value for the Device Security (no requirements) */
#define gGapDefaultDeviceSecurity_d \
{ \
  /* centralSecurityRequirements */     NULL, \
  /* cNumServices */                    0U, \
  /* aServiceSecurityRequirements */    NULL \
}

/*! The default value for a Security Requirement */
#define gGapDefaultSecurityRequirements_d \
{ \
      /* securityModeLevel */               gSecurityMode_1_Level_1_c, \
      /* authorization */                   FALSE, \
      /* minimumEncryptionKeySize */        gDefaultEncryptionKeySize_d \
}

/*
* Advertising Interval Ranges
* Size: 2 bytes
* Unit: 0.625ms.
* Range: 20ms - 10.24s.
* Default: 1.28s
*/

/*! Minimum advertising interval (20 ms) */
#define gGapAdvertisingIntervalRangeMinimum_c       0x0020U
/*! Default advertising interval (1.28 s) */
#define gGapAdvertisingIntervalDefault_c            0x0800U
/*! Maximum advertising interval (10.24 s) */
#define gGapAdvertisingIntervalRangeMaximum_c       0x4000U

/*! Minimum extended advertising interval (20 ms) */
#define gGapExtAdvertisingIntervalRangeMinimum_c    0x000020U
/*! Default extended advertising interval (1.28 s) */
#define gGapExtAdvertisingIntervalDefault_c         0x000800U
/*! Maximum extended advertising interval (10485.76 s) */
#define gGapExtAdvertisingIntervalRangeMaximum_c    0xFFFFFFU

/*! Minimum periodic advertising interval (7.5 ms) */
#define gGapPeriodicAdvIntervalRangeMinimum_c       0x0006U
/*! Default periodic advertising interval (2.56 s) */
#define gGapPeriodicAdvIntervalDefault_c            0x0800U
/*! Maximum periodic advertising interval (81.91875 s) */
#define gGapPeriodicAdvIntervalRangeMaximum_c       0xFFFFU


/*! Default Advertising Channel Map - all 3 channels are enabled */
#define gGapAdvertisingChannelMapDefault_c \
         ( gAdvChanMapFlag37_c \
         | gAdvChanMapFlag38_c \
         | gAdvChanMapFlag39_c )

/*! Default value for Advertising Parameters struct */
#define gGapDefaultAdvertisingParameters_d \
{ \
    /* minInterval */         gGapAdvertisingIntervalDefault_c, \
    /* maxInterval */         gGapAdvertisingIntervalDefault_c, \
    /* advertisingType */     gAdvConnectableUndirected_c, \
    /* addressType */         gBleAddrTypePublic_c, \
    /* peerAddressType */     gBleAddrTypePublic_c, \
    /* peerAddress */         {0U, 0U, 0U, 0U, 0U, 0U}, \
    /* channelMap */          (gapAdvertisingChannelMapFlags_t)gGapAdvertisingChannelMapDefault_c, \
    /* filterPolicy */        gProcessAll_c \
}

/*! Default value for Extended Advertising Parameters struct */
#define gGapDefaultExtAdvertisingParameters_d \
{ \
    /* SID */                       gBleExtAdvDefaultSetId_c, \
    /* handle */                    gBleExtAdvDefaultSetHandle_c, \
    /* minInterval */               gGapExtAdvertisingIntervalDefault_c, \
    /* maxInterval */               gGapExtAdvertisingIntervalDefault_c, \
    /* ownAddressType */            gBleAddrTypePublic_c, \
    /* ownRandomAddr */             {0, 0, 0, 0, 0, 0}, \
    /* peerAddressType */           gBleAddrTypePublic_c, \
    /* peerAddress */               {0, 0, 0, 0, 0, 0}, \
    /* channelMap */                (gapAdvertisingChannelMapFlags_t)gGapAdvertisingChannelMapDefault_c, \
    /* filterPolicy */              gProcessAll_c, \
    /* extAdvProperties */          gAdvReqConnectable_c, \
    /* txPower */                   gBleAdvTxPowerNoPreference_c, \
    /* primaryPHY */                gLePhy1M_c, \
    /* secondaryPHY */              gLePhyCoded_c, \
    /* secondaryAdvMaxSkip */       gBleExtAdvMaxAuxOffsetUsec_c / (gGapExtAdvertisingIntervalDefault_c * 625U) + 1U, \
    /* enableScanReqNotification */ FALSE \
}

/*! Default value for Periodic Advertising Parameters struct */
#define gGapDefaultPeriodicAdvParameters_d \
{ \
    /* handle */                    gBlePeriodicAdvDefaultHandle_c, \
    /* addTxPowerInAdv */           TRUE, \
    /* minInterval */               gGapPeriodicAdvIntervalRangeMinimum_c, \
    /* maxInterval */               gGapPeriodicAdvIntervalDefault_c, \
    /* numSubevents */              0U, \
    /* subeventInterval */          0U, \
    /* responseSlotDelay */         0U, \
    /* responseSlotSpacing */       0U, \
    /* numResponseSlots */          0U \
}

/*
* Scan Interval Ranges
* Size: 2 bytes
* Unit: 0.625ms.
* Range: 2.5ms - 10.24s.
* Default: 10ms
*/

/*! Minimum scan interval (2.5 ms) */
#define gGapScanIntervalMin_d       0x0004U
/*! Default scan interval (10 ms) */
#define gGapScanIntervalDefault_d   0x0010U
/*! Maximum scan interval (10.24 s) */
#define gGapScanIntervalMax_d       0x4000U

/*
* Scan Window Ranges
* Size: 2 bytes
* Unit: 0.625ms.
* Range: 2.5ms - 10.24s.
* Default: 10ms
*/

/*! Minimum scan window (2.5 ms) */
#define gGapScanWindowMin_d       0x0004U
/*! Default scan window (10 ms) */
#define gGapScanWindowDefault_d   0x0010U
/*! Maximum scan window (10.24 s) */
#define gGapScanWindowMax_d       0x4000U

/*
* RSSI Ranges
* Size: 1 byte, signed
* Unit: dBm.
* Range: (-128) - (+20).
* Special value: 127 = RSSI not available
*/

/*! Minimum valid value for RSSI (dB) */
#define gGapRssiMin_d               -128
/*! Maximum valid value for RSSI (dB) */
#define gGapRssiMax_d                +20
/*! A special invalid value for the RSSI indicating that the measurement is not available. */
#define gGapRssiNotAvailable_d       127

/*
*Scan duration
* Range: 0x0001 - 0xFFFF
* Time = N * 10 ms
* Time Range: 10 ms to 655.35 s
*/
/*! Default value for Scanning duration - Scan continuously until explicitly disable */
#define gGapScanContinuously_d       0

/*
* Scan Period - Time interval from when the Controller started its last Scan_Duration
*               until it begins the subsequent Scan_Duration.
* Range: 0x0001 - 0xFFFF
* Time = N * 1.28 sec
* Time Range: 1.28 s to 83,884.8 s
*/
/*! Default value for Scanning period - Periodic scanning disabled */
#define gGapScanPeriodicDisabled_d   0

/*! Default value for Scanning Parameters struct */
#define gGapDefaultScanningParameters_d \
{ \
    /* type */              gScanTypePassive_c, \
    /* interval */          gGapScanIntervalDefault_d, \
    /* window */            gGapScanWindowDefault_d, \
    /* ownAddressType */    gBleAddrTypePublic_c, \
    /* filterPolicy */      gScanAll_c, \
    /* scanning PHY */      gLePhy1MFlag_c\
}

/*
* Connection Interval Ranges
* Size: 2 bytes
* Unit: 1.25ms.
* Range: 7.5ms - 4s.
*/

/*! Minimum connection interval (7.5 ms) */
#define gGapConnIntervalMin_d       0x0006U
/*! Maximum connection interval (4 s) */
#define gGapConnIntervalMax_d       0x0C80U

/*
* Connection Latency Ranges
* Size: 2 bytes.
* Unit: 1 ignored event.
* Range: 0 - 499.
*/

/*! Minimum connection latency value (0 - no connection event may be ignored) */
#define gGapConnLatencyMin_d       0x0000U
/*! Maximum connection latency value (499 connection events may be ignored) */
#define gGapConnLatencyMax_d       0x01F3U

/*
* Supervision Timeout Ranges
* Size: 2 bytes.
* Unit: 10ms.
* Range: 100ms - 32s.
*/

/*! Minimum supervision timeout (100 ms) */
#define gGapConnSuperTimeoutMin_d       0x000AU
/*! Maximum supervision timeout (32 s) */
#define gGapConnSuperTimeoutMax_d       0x0C80U

/*
* Connection Event Length Ranges
* Size: 2 bytes.
* Unit: 0.625ms.
* Range: maximum possible (0 - 40959.375ms)
*/

/*! Minimum value of the connection event length (0 ms) */
#define gGapConnEventLengthMin_d       0x0000U
/*! Maximum value of the connection event length (~41 s) */
#define gGapConnEventLengthMax_d       0xFFFFU

/* Non-standard defaults for connection parameters */

/*! Default connection latency: 0 */
#define gGapDefaultConnectionLatency_d      0x0000U
/*! Default supervision timeout: 10s */
#define gGapDefaultSupervisionTimeout_d     0x03E8U
/*! Default minimum connection interval: 100ms */
#define gGapDefaultMinConnectionInterval_d  0x0050U
/*! Default maximum connection interval: 200ms */
#define gGapDefaultMaxConnectionInterval_d  0x00A0U

/*! The default value for the Connection Request Parameters structure */
#define gGapDefaultConnectionRequestParameters_d \
{ \
    /* scanInterval */          gGapScanIntervalDefault_d, \
    /* scanWindow */            gGapScanWindowDefault_d, \
    /* filterPolicy */          gUseDeviceAddress_c, \
    /* ownAddressType */        gBleAddrTypePublic_c, \
    /* peerAddressType */       gBleAddrTypePublic_c, \
    /* peerAddress */           { 0, 0, 0, 0, 0, 0 }, \
    /* connIntervalMin */       gGapDefaultMinConnectionInterval_d, \
    /* connIntervalMax */       gGapDefaultMaxConnectionInterval_d, \
    /* connLatency */           gGapDefaultConnectionLatency_d, \
    /* supervisionTimeout */    gGapDefaultSupervisionTimeout_d, \
    /* connEventLengthMin */    gGapConnEventLengthMin_d, \
    /* connEventLengthMax */    gGapConnEventLengthMax_d \
    /* initiatingPHYs */        gLePhy1MFlag_c \
}

/*! "Channel Selection Algorithm #2 is used" value in LE Channel Selection Algorithm Event */
#define gGapChSelAlgorithmNo2                   (0x01U)

/*! Sync handle value for which to call the create sync cancel command instead of terminate sync */
#define gBlePeriodicAdvOngoingSyncCancelHandle  (0xFFFFU)

/*! Sync handle used to differentiate extended advertising reports from periodic advertising reports */
#define gGapInvalidSyncHandle                   (0xFFFFU)

/*! Values of the AD Flags advertising data structure. */
#define gNone_c                                     0x00U    /*!< No information. */
#define gLeLimitedDiscoverableMode_c                BIT0     /*!< This device is in Limited Discoverable mode. */
#define gLeGeneralDiscoverableMode_c                BIT1     /*!< This device is in General Discoverable mode. */
#define gBrEdrNotSupported_c                        BIT2     /*!< This device supports only Bluetooth Low Energy; no support for Classic Bluetooth. */
#define gSimultaneousLeBrEdrCapableController_c     BIT3     /*!< This device's Controller also supports Classic Bluetooth. */
#define gSimultaneousLeBrEdrCapableHost_c           BIT4     /*!< This device's Host also supports Classic Bluetooth. */

/*! Flags indicating the Keys to be exchanged by the SMP during the key exchange phase of pairing. */
#define gNoKeys_c   0x00U     /*!< No key can be distributed. */
#define gLtk_c      BIT0      /*!< Long Term Key. */
#define gIrk_c      BIT1      /*!< Identity Resolving Key. */
#define gCsrk_c     BIT2      /*!< Connection Signature Resolving Key. */

/*! LE Security Mode values for gapSecurityMode_t */
#define gSecurityMode_1_c (0x10U)   /*!< Mode 1 - Encryption required (except for Level 1). */
#define gSecurityMode_2_c (0x20U)   /*!< Mode 2 - Data Signing required. */

/*! LE Security Level values for gapSecurityLevel_t */
#define gSecurityLevel_NoSecurity_c             (0x00U)   /*!< No security (combined only with Mode 1). */
#define gSecurityLevel_NoMitmProtection_c       (0x01U)   /*!< Unauthenticated (no MITM protection). */
#define gSecurityLevel_WithMitmProtection_c     (0x02U)   /*!< Authenticated (MITM protection by PIN or OOB). */
#define gSecurityLevel_LeSecureConnections_c    (0x03U)   /*!< Authenticated with LE Secure Connections. */

/*! Security Mode-and-Level definitions values for gapSecurityModeAndLevel_t */
#define gSecurityMode_1_Level_1_c (gSecurityMode_1_c | gSecurityLevel_NoSecurity_c)             /*!< Mode 1 Level 1 - No Security. */
#define gSecurityMode_1_Level_2_c (gSecurityMode_1_c | gSecurityLevel_NoMitmProtection_c)       /*!< Mode 1 Level 2 - Encryption without authentication. */
#define gSecurityMode_1_Level_3_c (gSecurityMode_1_c | gSecurityLevel_WithMitmProtection_c)     /*!< Mode 1 Level 3 - Encryption with authentication. */
#define gSecurityMode_1_Level_4_c (gSecurityMode_1_c | gSecurityLevel_LeSecureConnections_c)    /*!< Mode 1 Level 4 - Encryption with LE Secure Connections pairing. */
#define gSecurityMode_2_Level_1_c (gSecurityMode_2_c | gSecurityLevel_NoMitmProtection_c)       /*!< Mode 2 Level 1 - Data Signing without authentication. */
#define gSecurityMode_2_Level_2_c (gSecurityMode_2_c | gSecurityLevel_WithMitmProtection_c)     /*!< Mode 2 Level 2 - Data Signing with authentication. */

/*! Reason for rejecting the pairing request used by gapAuthenticationRejectReason_t. These values are equal to the corresponding reasons from SMP. */
#define gOobNotAvailable_c              (0x02U)     /*!< This device does not have the required OOB for authenticated pairing. */
#define gIncompatibleIoCapabilities_c   (0x03U)     /*!< The combination of I/O capabilities does not allow pairing with the desired level of security. */
#define gPairingNotSupported_c          (0x05U)     /*!< This device does not support pairing. */
#define gLowEncryptionKeySize_c         (0x06U)     /*!< The peer's encryption key size is too low for this device's required security level. */
#define gUnspecifiedReason_c            (0x08U)     /*!< The host has rejected the pairing for an unknown reason. */
#define gRepeatedAttempts_c             (0x09U)     /*!< This device is the target of repeated unsuccessful pairing attempts and does not allow further pairing attempts at the moment.  */
#define gLinkEncryptionFailed_c         (0xF0U)     /*!< Link could not be encrypted. This reason may not be used by Gap_RejectPairing! */

/*! I/O Capabilities as defined by the SMP used by gapIoCapabilities_t */
#define gIoDisplayOnly_c                (0x00U)     /*!< May display a PIN, no input. */
#define gIoDisplayYesNo_c               (0x01U)     /*!< May display a PIN and has a binary input (e.g., YES and NO buttons). */
#define gIoKeyboardOnly_c               (0x02U)     /*!< Has keyboard input, no display. */
#define gIoNone_c                       (0x03U)     /*!< No input and no display. */
#define gIoKeyboardDisplay_c            (0x04U)     /*!< Has keyboard input and display. */

/*! AoA/AoD */
#define gGapCteMinLength_c                  (0x02U) /*!< Min CTE length in 8 us units. */
#define gGapCteMaxLength_c                  (0x14U) /*!< Max CTE length in 8 us units. */
#define gGapCteMinCount_c                   (0x01U) /*!< Min number of CTEs to transmit in each periodic advertising interval. */
#define gGapCteMaxCount_c                   (0x10U) /*!< Max number of CTEs to transmit in each periodic advertising interval. */
#define gGapMinSwitchingPatternLength_c     (0x02U) /*!< Min number of antennae in the pattern. */
#define gGapMaxSwitchingPatternLength_c     (0x4BU) /*!< Max number of antennae in the pattern. */
#define gGapEattMaxBearers                  (5U)    /*!< Maximum number of bearer Ids in one GAP request/response. */

/* gapDecisionInstructionsAdvMode_t values */
#define gbmpDIAM_NonConnectableNonScannableUndirected_c   BIT0 /* The check passes for non-connectable non-scannable undirected events. */
#define gbmpDIAM_ConnectableUndirected_c                  BIT1 /* The check passes for connectable undirected events. */
#define gbmpDIAM_ScannableUndirected_c                    BIT2 /* The check passes for scannable undirected events. */

/*************************************************************************************
 *************************************************************************************
 * Public type definitions
 *************************************************************************************
 ************************************************************************************/

/*! GAP Role of a BLE device */
typedef enum {
    gGapCentral_c,      /*!< Central scans and connects to Peripherals. */
    gGapPeripheral_c,   /*!< Peripheral advertises and connects to Centrals. */
    gGapObserver_c,     /*!< Observer only scans and makes no connections. */
    gGapBroadcaster_c   /*!< Broadcaster only advertises and makes no connections. */
} gapRole_t;

/*! I/O Capabilities as defined by the SMP */
typedef uint8_t gapIoCapabilities_t;

/*! Flags indicating the Keys to be exchanged by the SMP during the key exchange phase of pairing. */
typedef uint8_t gapSmpKeyFlags_t;

/*! Structure containing the SMP information exchanged during pairing. */
typedef struct {
    uint8_t     cLtkSize;   /*!< Encryption Key Size. If aLtk is NULL, this is ignored. */
    uint8_t*    aLtk;       /*!< Long Term (Encryption) Key. NULL if LTK is not distributed, else size is given by cLtkSize.*/

    uint8_t*    aIrk;       /*!< Identity Resolving Key. NULL if aIrk is not distributed. */
    uint8_t*    aCsrk;      /*!< Connection Signature Resolving Key. NULL if aCsrk is not distributed. */

    uint8_t     cRandSize;  /*!< Size of RAND; usually equal to gcMaxRandSize_d. If aLtk is NULL, this is ignored. */
    uint8_t*    aRand;      /*!< RAND value used to identify the LTK. If aLtk is NULL, this is ignored. */
    uint16_t    ediv;       /*!< EDIV value used to identify the LTK. If aLtk is NULL, this is ignored. */

    bleAddressType_t    addressType; /*!< Public or Random address. If aAddress is NULL, this is ignored. */
    uint8_t*            aAddress;    /*!< Device Address. NULL if address is not distributed. If aIrk is NULL, this is ignored. */
} gapSmpKeys_t;

/*! LE Security Mode */
typedef uint8_t gapSecurityMode_t;

/*! LE Security Level */
typedef uint8_t gapSecurityLevel_t;

/*! Security Mode-and-Level definitions */
typedef uint8_t gapSecurityModeAndLevel_t;

/*! Security Requirements structure for a Device, a Service or a Characteristic */
typedef struct {
    gapSecurityModeAndLevel_t   securityModeLevel;          /*!< Security mode and level. */
    bool_t                      authorization;              /*!< Authorization required. */
    uint16_t                    minimumEncryptionKeySize;   /*!< Minimum encryption key (LTK) size. Ignored if gSecurityMode_1_Level_4_c is required (set to gMaxEncryptionKeySize_d automatically) */
} gapSecurityRequirements_t;

/*! Service Security Requirements */
typedef struct {
    uint16_t                    serviceHandle;  /*!< Handle of the Service declaration in the GATT Database. */
    gapSecurityRequirements_t   requirements;   /*!< Requirements for all attributes in this service. */
} gapServiceSecurityRequirements_t;

/*! Device Security - Security Requirements + Service Security Requirements */
typedef struct {
    gapSecurityRequirements_t*          pSecurityRequirements;          /*!< Security requirements added to all services. */
    uint8_t                             cNumServices;                   /*!< Number of service-specific requirements; must be less than or equal to gcGapMaxServiceSpecificSecurityRequirements_c. */
    gapServiceSecurityRequirements_t*   aServiceSecurityRequirements;   /*!< Array of service-specific requirements. */
} gapDeviceSecurityRequirements_t;

/*! Keypress Notification Types */
typedef uint8_t gapKeypressNotification_t;
typedef enum gapKeypressNotification_tag
{
    gKnPasskeyEntryStarted_c    = 0x00U,   /*!< Start of the Passkey Entry. */
    gKnPasskeyDigitStarted_c    = 0x01U,   /*!< Digit entered. */
    gKnPasskeyDigitErased_c     = 0x02U,   /*!< Digit erased. */
    gKnPasskeyCleared_c         = 0x03U,   /*!< Passkey cleared. */
    gKnPasskeyEntryCompleted_c  = 0x04U    /*!< Passkey Entry completed. */
} gapKeypressNotification_tag;

/*! List of Attribute Handles for authorization lists. */
typedef struct {
    uint8_t     cNumHandles;                               /*!< Number of handles in this list. */
    uint16_t    aHandles[gcGapMaxAuthorizationHandles_c];  /*!< List of handles. */
} gapHandleList_t;

/*! Connection Security Information structure */
typedef struct {
    bool_t                      authenticated;      /*!< TRUE if pairing was performed with MITM protection. */
    gapHandleList_t             authorizedToRead;   /*!< List of handles the peer has been authorized to read. */
    gapHandleList_t             authorizedToWrite;  /*!< List of handles the peer has been authorized to write. */
} gapConnectionSecurityInformation_t;

/*! Pairing parameters structure for the Gap_Pair and Gap_AcceptPairingRequest APIs */
typedef struct {
    bool_t                      withBonding;            /*!< TRUE if this device is able to and wants to bond after pairing, FALSE otherwise. */
    gapSecurityModeAndLevel_t   securityModeAndLevel;   /*!< The desired security mode-level. */
    uint8_t                     maxEncryptionKeySize;   /*!< Maximum LTK size supported by the device. */
    gapIoCapabilities_t         localIoCapabilities;    /*!< I/O capabilities used to determine the pairing method. */
    bool_t                      oobAvailable;           /*!< TRUE if this device has Out-of-Band data that can be used for authenticated pairing. FALSE otherwise. */
    gapSmpKeyFlags_t            centralKeys;            /*!< Indicates the SMP keys to be distributed by the Central. */
    gapSmpKeyFlags_t            peripheralKeys;         /*!< Indicates the SMP keys to be distributed by the Peripheral. */
    bool_t                      leSecureConnectionSupported;    /*!< Indicates if device supports LE Secure Connections pairing.
                                                                     Conflict if this is FALSE and securityModeAndLevel is gSecurityMode_1_Level_4_c. */
    bool_t                      useKeypressNotifications;       /*!< Indicates if device supports Keypress Notification PDUs during Passkey Entry pairing.
                                                                     Conflict if this is TRUE and localIoCapabilities is set to gIoNone_c. */
} gapPairingParameters_t;

/*! Parameters of a Peripheral Security Request. */
typedef struct {
    bool_t                      bondAfterPairing;       /*!< TRUE if the Peripheral supports bonding. */
    bool_t                      authenticationRequired; /*!< TRUE if the Peripheral requires authentication for MITM protection. */
} gapPeripheralSecurityRequestParameters_t;

/*! Reason for rejecting the pairing request. */
typedef uint8_t gapAuthenticationRejectReason_t;

/*! Scan Mode options; used as parameter for Gap_SetScanMode. */
typedef enum {
    gDefaultScan_c       = 0x00U,   /*!< Reports all scanned devices to the application. */
    gLimitedDiscovery_c  = 0x01U,   /*!< Reports only devices in Limited Discoverable Mode, i.e., containing the Flags AD with the LE Limited Discoverable Flag set. */
    gGeneralDiscovery_c  = 0x02U,   /*!< Reports only devices in General Discoverable Mode, i.e., containing the Flags AD with the LE General Discoverable Flag set. */
    gAutoConnect_c       = 0x03U    /*!< Automatically connects with devices with known addresses and does not report any scanned device to the application. */
} gapScanMode_t;

/*! Advertising Channel Map flags - setting a bit activates advertising on the respective channel. */
typedef enum {
    gAdvChanMapFlag37_c        = BIT0,   /*!< Bit for channel 37. */
    gAdvChanMapFlag38_c        = BIT1,   /*!< Bit for channel 38. */
    gAdvChanMapFlag39_c        = BIT2    /*!< Bit for channel 39. */
} gapAdvertisingChannelMapFlags_t;

/*! Advertising Filter Policy values */
typedef enum {
    gProcessAll_c                    = 0x00U,     /*!< Default value: accept all connect and scan requests. */
    gProcessConnAllScanWL_c          = 0x01U,     /*!< Accept all connect requests, but scan requests only from devices in Filter Accept List. */
    gProcessScanAllConnWL_c          = 0x02U,     /*!< Accept all scan requests, but connect requests only from devices in Filter Accept List. */
    gProcessFilterAcceptListOnly_c   = 0x03U,     /*!< Accept connect and scan requests only from devices in Filter Accept List. */
} gapAdvertisingFilterPolicy_t;

/*! Advertising Parameters; for defaults see gGapDefaultAdvertisingParameters_d. */
typedef struct {
    uint16_t                            minInterval;            /*!< Minimum desired advertising interval. Default: 1.28 s. */
    uint16_t                            maxInterval;            /*!< Maximum desired advertising interval. Default: 1.28 s. */
    bleAdvertisingType_t                advertisingType;        /*!< Advertising type. Default: connectable undirected. */
    bleAddressType_t                    ownAddressType;         /*!< Indicates whether the advertising address is the public address (BD_ADDR) or the random address (set by Gap_SetRandomAddress). Default: public address.
                                                                     If Controller Privacy is enabled, this parameter is irrelevant as Private Resolvable Addresses are always used. */
    bleAddressType_t                    peerAddressType;        /*!< Address type of the peer; only used in directed advertising and Enhanced Privacy. */
    bleDeviceAddress_t                  peerAddress;            /*!< Address of the peer; same as above. */
    gapAdvertisingChannelMapFlags_t     channelMap;             /*!< Bit mask indicating which of the three advertising channels are used. Default: all three. */
    gapAdvertisingFilterPolicy_t        filterPolicy;           /*!< Indicates whether the connect and scan requests are filtered using the Filter Accept List. Default: does not use Filter Accept List (process all). */
} gapAdvertisingParameters_t;

/*! Extended Advertising Parameters; for defaults see gGapDefaultExtAdvertisingParameters_d. */
typedef struct gapExtAdvertisingParameters_tag {
    uint8_t                             SID;                    /*!< ID of the advertising set chosen by application. Shall be lower than gBleExtAdvMaxSetId_c */
    uint8_t                             handle;                 /*!< ID of the advertising set handled by controller. Shall be lower than gMaxAdvSets_c */
    uint32_t                            minInterval;            /*!< Minimum desired advertising interval. Shall be at least equal or higher than gGapExtAdvertisingIntervalRangeMinimum_c */
    uint32_t                            maxInterval;            /*!< Maximum desired advertising interval. Shall be higher than gGapExtAdvertisingIntervalRangeMinimum_c and higher than minInterval */
    bleAddressType_t                    ownAddressType;         /*!< Indicates whether the advertising address is the public address (BD_ADDR) or the random address (set by Gap_SetRandomAddress). Default: public address.
                                                                     If Controller Privacy is enabled, this parameter is irrelevant as Private Resolvable Addresses are always used. */
    bleDeviceAddress_t                  ownRandomAddr;          /*!< The random address used for advertising on the current handle */
    bleAddressType_t                    peerAddressType;        /*!< Address type of the peer; only used in directed advertising and Enhanced Privacy. */
    bleDeviceAddress_t                  peerAddress;            /*!< Address of the peer; same as above. */
    gapAdvertisingChannelMapFlags_t     channelMap;             /*!< Bit mask indicating which of the three advertising channels are used for primary advertising */
    gapAdvertisingFilterPolicy_t        filterPolicy;           /*!< Indicates whether the connect and scan requests are filtered using the Filter Accept List */

    bleAdvRequestProperties_t           extAdvProperties;       /*!< Type of advertising event */
    int8_t                              txPower;                /*!< The maximum power level at which the adv packets are to be transmitted.
                                                                     The Controller shall choose a power level lower than or equal to the one specified by the Host.
                                                                     Valid range: -127 to 20 */
    gapLePhyMode_t                      primaryPHY;             /*!< The PHY on which the advertising packets are transmitted (1M or Coded PHY). Used for sending ADV_EXT_IND */
    gapLePhyMode_t                      secondaryPHY;           /*!< The PHY used for sending AUX_ADV_IND PDU. Used only for Extended Advertising Events */
    uint8_t                             secondaryAdvMaxSkip;    /*!< Maximum number of advertising events that can be skipped before the AUX_ADV_IND can be sent*/
    bool_t                              enableScanReqNotification; /*!< Indicates whether the Controller shall send notifications upon the receipt of a scan request PDU */
    gapLePhyOptionsFlags_t              primaryAdvPhyOptions;   /*!< Preferred or required coding when transmitting on the primary LE Coded PHY */
    gapLePhyOptionsFlags_t              secondaryAdvPhyOptions; /*!< Preferred or required coding when transmitting on the secondary LE Coded PHY */
} gapExtAdvertisingParameters_t;

/*! Periodic Advertising Parameters [v2]; for defaults see gGapDefaultPeriodicAdvParameters_d. */
typedef struct gapPeriodicAdvParameters_tag {
    uint8_t                             handle;             /*!< ID of the advertising set handled by controller. Shall be lower than gMaxAdvSets_c */
    bool_t                              addTxPowerInAdv;    /*!< Set this option to include the Tx power in advertising packet. */
    uint16_t                            minInterval;        /*!< Minimum advertising interval for periodic advertising. */
    uint16_t                            maxInterval;        /*!< Maximum advertising interval for periodic advertising. Should be different and higher than minInterval. */
    uint8_t                             numSubevents;       /*!< Number of subevents. */
    uint8_t                             subeventInterval;   /*!< Interval between subevents. */
    uint8_t                             responseSlotDelay;  /*!< Time between the advertising packet in a subevent and the first response slot. */
    uint8_t                             responseSlotSpacing;/*!< Time between response slots. */
    uint8_t                             numResponseSlots;   /*!< Number of response slots. */
} gapPeriodicAdvParameters_t;

/*! Periodic Advertisement Sync Transfer parameters */
typedef enum gapPeriodicAdvSyncMode_tag
{
    gapPeriodicSyncNoSyncMode_c = 0x00U,                    /*!< No attempt is made to synchronize to the periodic advertising and no gHciLePeriodicAdvSyncTransferReceived_c event is sent to the Host */
    gapPeriodicSyncNoReports_c  = 0x01U,                    /*!< A gHciLePeriodicAdvSyncTransferReceived_c event is sent to the Host. gHciLePeriodicAdvReportEvent_c events will be disabled. */
    gapPeriodicSyncReportsEnabled_c = 0x02U,                /*!< A gHciLePeriodicAdvSyncTransferReceived_c event is sent to the Host. gHciLePeriodicAdvReportEvent_c events will be enabled with duplicate filtering disabled */
    gapPeriodicSyncReportsEnabledWithDF_c = 0x03U           /*!< A gHciLePeriodicAdvSyncTransferReceived_c event is sent to the Host. gHciLePeriodicAdvReportEvent_c events will be enabled with duplicate filtering enabled */
} gapPeriodicAdvSyncMode_t;

typedef struct gapPeriodicAdvSyncTransfer_tag
{
    deviceId_t deviceId;                                    /*!< Connection identifier */
    uint16_t serviceData;                                   /*!< Value provided by the host */
    uint16_t syncHandle;                                    /*!< Identifier for the periodic advertising train */
} gapPeriodicAdvSyncTransfer_t;

typedef struct gapPeriodicAdvSetInfoTransfer_tag
{
    deviceId_t deviceId;                                    /*!< Connection identifier */
    uint16_t serviceData;                                   /*!< Value provided by the host */
    uint16_t advHandle;                                     /*!< Identifier for the advertising set */
} gapPeriodicAdvSetInfoTransfer_t;

typedef struct gapSetPeriodicAdvSyncTransferParams_tag
{
    deviceId_t deviceId;                                    /*!< Connection identifier */
    gapPeriodicAdvSyncMode_t mode;                          /*!< The action to be taken when periodic advertising synchronization information is received */
    uint16_t skip;                                          /*!< The number of periodic advertising packets that can be skipped after a successful receive */
    uint16_t syncTimeout;                                   /*!< Synchronization timeout for the periodic advertising train */
    bleSyncCteType_t CTEType;                               /*!< 0: Do not sync to packets with an AoA Constant Tone Extension
                                                                 1: Do not sync to packets with an AoD Constant Tone Extension with 1 us slots
                                                                 2: Do not sync to packets with an AoD Constant Tone Extension with 2 us slots
                                                                 4: Do not sync to packets without a Constant Tone Extension */
} gapSetPeriodicAdvSyncTransferParams_t;

/*
*
* Scanning Parameters
*
*/

/*! Scanning parameters; for defaults see gGapDefaultScanningParameters_d. */
typedef struct {
    bleScanType_t               type;               /*!< Scanning type. Default: passive. */
    uint16_t                    interval;           /*!< Scanning interval. Default: 10 ms. */
    uint16_t                    window;             /*!< Scanning window. Default: 10 ms. */
    bleAddressType_t            ownAddressType;     /*!< Indicates whether the address used in scan requests is the public address (BD_ADDR) or the random address (set by Gap_SetRandomAddress). Default: public address.
                                                         If Controller Privacy is enabled, this parameter is irrelevant as Private Resolvable Addresses are always used. */
    bleScanningFilterPolicy_t   filterPolicy;       /*!< Indicates whether the advertising packets are filtered using the Filter Accept List. Default: does not use Filter Accept List (scan all). */
    /* BLE 5.0: Extended Scan only. */
    gapLePhyFlags_t             scanningPHYs;       /*!< Indicates the PHYs on which the advertising packets should be received on the primary advertising channel. */
} gapScanningParameters_t;

typedef enum
{
    gGapDuplicateFilteringDisable_c    = 0x00,     /*!< Duplicate filtering disabled */
    gGapDuplicateFilteringEnable_c     = 0x01,     /*!< Duplicate filtering enabled */
    /* BLE 5.0: Extended Scan Enable only*/
    gGapDuplicateFilteringPeriodicEnable_c = 0x02, /*!< Duplicate filtering enabled, reset for each scan period */
} gapFilterDuplicates_t;

/*! Create Sync Request Filter Policy values */
typedef uint8_t gapCreateSyncReqFilterPolicy_t;
typedef enum {
    gUseCommandParameters_c          = 0x00,     /*!< Use the SID, peerAddressType, and peerAddress parameters to determine which advertiser to listen to. */
    gUsePeriodicAdvList_c            = 0x01,     /*!< Use the Periodic Advertiser List to determine which advertiser to listen to */
} gapCreateSyncReqFilterPolicy_tag;

/*! Create Sync Request Options */
typedef struct gapCreateSyncReqOptions_tag {
  gapCreateSyncReqFilterPolicy_t filterPolicy :1;
  uint8_t reportingEnabled                    :1;
  uint8_t duplicateFilteringEnabled           :1;
} gapCreateSyncReqOptions_t;

/*! Periodic Advertising Sync Request parameters */
typedef struct gapPeriodicAdvSyncReq_tag {
    gapCreateSyncReqOptions_t           options;        /*!< Bit 0: Filter Policy. Bit 1: Reporting enabled/disabled. Bit 2: Duplicate filtering disabled/enabled. */
    uint8_t                             SID;            /*!< The SID advertised by the periodic advertiser in the ADI field. */
    bleAddressType_t                    peerAddressType;/*!< Periodic advertiser's address type (Public or Random )*/
    bleDeviceAddress_t                  peerAddress;    /*!< Periodic advertiser's address */
    uint16_t                            skipCount;      /*!< The number of consecutive periodic advertising packets that the receiver may skip after successfully receiving a periodic advertising packet.*/
    uint16_t                            timeout;        /*!< The maximum permitted time between successful receives. If this time is exceeded, synchronization is lost. */
    bleSyncCteType_t                    cteType;        /*!< Specifies whether to only sync to certain types of CTE */
} gapPeriodicAdvSyncReq_t;


/*
*
* Connection Parameters
*
*/

/*! Connection request parameter structure to be used in the Gap_Connect function; for API-defined defaults, use gGapDefaultConnectionRequestParameters_d. */
typedef struct {
    uint16_t                    scanInterval;           /*!< Scanning interval. Default: 10 ms. */
    uint16_t                    scanWindow;             /*!< Scanning window. Default: 10 ms. */
    bleInitiatorFilterPolicy_t  filterPolicy;           /*!< Indicates whether the connection request is issued for a specific device or for all the devices in the Filter Accept List. Default: specific device.*/
    bleAddressType_t            ownAddressType;         /*!< Indicates whether the address used in connection requests is the public address (BD_ADDR) or the random address (set by Gap_SetRandomAddress). Default: public address. */
    bleAddressType_t            peerAddressType;        /*!< When connecting to a specific device (see filterPolicy), this indicates that device's address type. Default: public address. */
    bleDeviceAddress_t          peerAddress;            /*!< When connecting to a specific device (see filterPolicy), this indicates that device's address. */
    uint16_t                    connIntervalMin;        /*!< The minimum desired connection interval. Default: 100 ms. */
    uint16_t                    connIntervalMax;        /*!< The maximum desired connection interval. Default: 200 ms. */
    uint16_t                    connLatency;            /*!< The desired connection latency (the maximum number of consecutive connection events the Peripheral is allowed to ignore). Default: 0. */
    uint16_t                    supervisionTimeout;     /*!< The maximum time interval between consecutive over-the-air packets; if this timer expires, the connection is dropped. Default: 10 s. */
    uint16_t                    connEventLengthMin;     /*!< The minimum desired connection event length. Default: 0 ms. */
    uint16_t                    connEventLengthMax;     /*!< The maximum desired connection event length. Default: maximum possible, ~41 s. (lets the Controller decide). */
    bool_t                      usePeerIdentityAddress; /*!< If Controller Privacy is enabled and this parameter is TRUE, the address defined in the peerAddressType and peerAddress is an identity address.
                                                             Otherwise, it is a device address. */
    /* BLE 5.0: Extended Connection only*/
    gapLePhyFlags_t             initiatingPHYs;          /*!< Indicates the PHY on which the advertising packets should be received on the primary advertising channel and the PHY for which connection parameters have been specified.*/
} gapConnectionRequestParameters_t;

/*! Connection request parameter structure to be used in the Gap_ConnectFromPawr function */
typedef struct {
    uint16_t                    scanInterval;           /*!< Scanning interval. Default: 10 ms. */
    uint16_t                    scanWindow;             /*!< Scanning window. Default: 10 ms. */
    bleInitiatorFilterPolicy_t  filterPolicy;           /*!< Indicates whether the connection request is issued for a specific device or for all the devices in the Filter Accept List. Default: specific device.*/
    bleAddressType_t            ownAddressType;         /*!< Indicates whether the address used in connection requests is the public address (BD_ADDR) or the random address (set by Gap_SetRandomAddress). Default: public address. */
    bleAddressType_t            peerAddressType;        /*!< When connecting to a specific device (see filterPolicy), this indicates that device's address type. Default: public address. */
    bleDeviceAddress_t          peerAddress;            /*!< When connecting to a specific device (see filterPolicy), this indicates that device's address. */
    uint16_t                    connIntervalMin;        /*!< The minimum desired connection interval. Default: 100 ms. */
    uint16_t                    connIntervalMax;        /*!< The maximum desired connection interval. Default: 200 ms. */
    uint16_t                    connLatency;            /*!< The desired connection latency (the maximum number of consecutive connection events the Peripheral is allowed to ignore). Default: 0. */
    uint16_t                    supervisionTimeout;     /*!< The maximum time interval between consecutive over-the-air packets; if this timer expires, the connection is dropped. Default: 10 s. */
    uint16_t                    connEventLengthMin;     /*!< The minimum desired connection event length. Default: 0 ms. */
    uint16_t                    connEventLengthMax;     /*!< The maximum desired connection event length. Default: maximum possible, ~41 s. (lets the Controller decide). */
    bool_t                      usePeerIdentityAddress; /*!< If Controller Privacy is enabled and this parameter is TRUE, the address defined in the peerAddressType and peerAddress is an identity address.
                                                             Otherwise, it is a device address. */
    gapLePhyFlags_t             initiatingPHYs;          /*!< Indicates the PHY on which the advertising packets should be received on the primary advertising channel and the PHY for which connection parameters have been specified.*/
    uint8_t                     advHandle;               /*!< Advertising handle identifying the periodic advertising train. */
    uint8_t                     subevent;                /*!< Subevent where the connection request is to be sent. */
} gapConnectionFromPawrParameters_t;

/*! Connection parameters as received in the gConnEvtConnected_c connection event. */
typedef struct {
    uint16_t                    connInterval;           /*!< Interval between connection events. */
    uint16_t                    connLatency;            /*!< Number of consecutive connection events the Peripheral may ignore. */
    uint16_t                    supervisionTimeout;     /*!< The maximum time interval between consecutive over-the-air packets; if this timer expires, the connection is dropped. */
    bleCentralClockAccuracy_t   centralClockAccuracy;   /*!< Accuracy of central's clock, allowing for frame detection optimizations. */
} gapConnectionParameters_t;

/*! Connection subrate parameters */
typedef struct {
    uint16_t                    subrateMin;           /*!< Minimum subrate factor to be applied to the underlying connection interval. */
    uint16_t                    subrateMax;           /*!< Maximum subrate factor to be applied to the underlying connection interval. */
    uint16_t                    latencyMax;           /*!< Maximum Peripheral latency for the connection in units of subrated connection intervals. */
    uint16_t                    continuationNumber;   /*!< Minimum number of underlying connection events to remain active
                                                           after a packet containing a Link Layer PDU with a non-zero Length field
                                                           is sent or received. */
    uint16_t                    supervisionTimeout;     /*!< The maximum time interval between consecutive over-the-air packets in 10ms units; if this timer expires, the connection is dropped. */
} gapConnectionSubrateParameters_t;
/*
*
* BLE 5.1
*
*/
typedef enum {
    gUseGeneratedKey_c      = 0x00U,    /*!< The private key generated by LE_Read_Local_P-256_Public_Key command, used for DH key generation */
    gUseDebugKey_c          = 0x01U,    /*!< The private debug key, used for DH key generation */
}gapPrivateKeyType_t;

/*! DH Key Generate V2 */
typedef struct {
    ecdhPublicKey_t     remoteP256PublicKey;    /*!< The remote P-256 public key */
    gapPrivateKeyType_t keyType;                /*!< The private key type used for generating the DH key */
}gapGenerateDHKeyV2Params_t;

/*
*
* AoA/AoD Parameters
*
*/

/*! Parameter structure to be used in the Gap_SetConnectionlessCteTransmitParameters function. */
typedef struct {
    uint8_t         handle;                 /*!< Advertising set handle. */
    uint8_t         cteLength;              /*!< Constant Tone Extension length in 8 us. */
    bleCteType_t    cteType;                /*!< Type of CTE. */
    uint8_t         cteCount;               /*!< Number of CTEs to transmit in each periodic advertising interval. */
    uint8_t         switchingPatternLength; /*!< Number of Antenna IDs in pattern. */
    uint8_t         aAntennaIds[1];         /*!< List of Antenna IDs in pattern. */
} gapConnectionlessCteTransmitParams_t;

/*! Parameter structure to be used in the Gap_EnableConnectionlessIqSampling function. */
typedef struct {
    bleIqSamplingEnable_t   iqSamplingEnable;       /*!< Enable or disable IQ sampling. */
    bleSlotDurations_t      slotDurations;          /*!< Switching and sampling slot durations. */
    uint8_t                 maxSampledCtes;         /*!< Maximum number of CTEs to sample and report in each periodic adv interval. */
    uint8_t                 switchingPatternLength; /*!< Number of Antenna IDs in pattern. */
    uint8_t                 aAntennaIds[1];         /*!< List of Antenna IDs in pattern. */
} gapConnectionlessIqSamplingParams_t;

/*! Parameter structure to be used in the Gap_SetConnectionCteTransmitParameters function. */
typedef struct {
    bleCteAllowedTypesMap_t cteTypes;                   /*!< Allowed CTE types. */
    uint8_t                 switchingPatternLength;     /*!< Number of Antenna IDs in pattern. */
    uint8_t                 aAntennaIds[1];             /*!< List of Antenna IDs in pattern. */
} gapConnectionCteTransmitParams_t;

/*! Parameter structure to be used in the Gap_SetConnectionCteReceiveParameters function. */
typedef struct {
    bleIqSamplingEnable_t   iqSamplingEnable;       /*!< Enable or disable IQ sampling. */
    bleSlotDurations_t      slotDurations;          /*!< Switching and sampling slot durations. */
    uint8_t                 switchingPatternLength; /*!< Number of Antenna IDs in pattern. */
    uint8_t                 aAntennaIds[1];         /*!< List of Antenna IDs in pattern. */
} gapConnectionCteReceiveParams_t;

/*! Parameter structure to be used in the Gap_EnableConnectionCteRequest function. */
typedef struct {
    bleCteReqEnable_t   cteReqEnable;        /*!< Enable or disable CTE Req procedure. */
    uint16_t            cteReqInterval;      /*!< Requested interval for initiating CTE Req procedure. */
    uint8_t             requestedCteLength;  /*!< Minimum length of CTEs requested in 8 us units. */
    bleCteType_t        requestedCteType;    /*!< Requested types of CTE. */
} gapConnectionCteReqEnableParams_t;

/*! Parameter structure to be used in the Gap_SetPathLossReportingParameters function. */
typedef struct {
    uint8_t     highThreshold;      /*!< High threshold for the path loss. Units: dB. */
    uint8_t     highHysteresis;     /*!< Hysteresis value for the high threshold. Units: dB.  */
    uint8_t     lowThreshold;       /*!< Low threshold for the path loss. Units: dB.  */
    uint8_t     lowHysteresis;      /*!< Hysteresis value for the low threshold. Units: dB.  */
    uint16_t    minTimeSpent;       /*!< Minimum time in number of connection events to be observed once the
                                         path crosses the threshold before an event is generated.*/
} gapPathLossReportingParams_t;

/*
*
* Advertising and Scan Response Data
*
*/

/*! AD Type values as defined by Bluetooth SIG used when defining gapAdStructure_t structures for advertising or scan response data. */
typedef enum {
    gAdFlags_c                               = 0x01U,    /*!< Defined by the Bluetooth SIG. */
    gAdIncomplete16bitServiceList_c          = 0x02U,    /*!< Defined by the Bluetooth SIG. */
    gAdComplete16bitServiceList_c            = 0x03U,    /*!< Defined by the Bluetooth SIG. */
    gAdIncomplete32bitServiceList_c          = 0x04U,    /*!< Defined by the Bluetooth SIG. */
    gAdComplete32bitServiceList_c            = 0x05U,    /*!< Defined by the Bluetooth SIG. */
    gAdIncomplete128bitServiceList_c         = 0x06U,    /*!< Defined by the Bluetooth SIG. */
    gAdComplete128bitServiceList_c           = 0x07U,    /*!< Defined by the Bluetooth SIG. */
    gAdShortenedLocalName_c                  = 0x08U,    /*!< Defined by the Bluetooth SIG. */
    gAdCompleteLocalName_c                   = 0x09U,    /*!< Defined by the Bluetooth SIG. */
    gAdTxPowerLevel_c                        = 0x0AU,    /*!< Defined by the Bluetooth SIG. */
    gAdClassOfDevice_c                       = 0x0DU,    /*!< Defined by the Bluetooth SIG. */
    gAdSimplePairingHashC192_c               = 0x0EU,    /*!< Defined by the Bluetooth SIG. */
    gAdSimplePairingRandomizerR192_c         = 0x0FU,    /*!< Defined by the Bluetooth SIG. */
    gAdSecurityManagerTkValue_c              = 0x10U,    /*!< Defined by the Bluetooth SIG. */
    gAdSecurityManagerOobFlags_c             = 0x11U,    /*!< Defined by the Bluetooth SIG. */
    gAdPeripheralConnectionIntervalRange_c   = 0x12U,    /*!< Defined by the Bluetooth SIG. */
    gAdServiceSolicitationList16bit_c        = 0x14U,    /*!< Defined by the Bluetooth SIG. */
    gAdServiceSolicitationList32bit_c        = 0x1FU,    /*!< Defined by the Bluetooth SIG. */
    gAdServiceSolicitationList128bit_c       = 0x15U,    /*!< Defined by the Bluetooth SIG. */
    gAdServiceData16bit_c                    = 0x16U,    /*!< Defined by the Bluetooth SIG. */
    gAdServiceData32bit_c                    = 0x20U,    /*!< Defined by the Bluetooth SIG. */
    gAdServiceData128bit_c                   = 0x21U,    /*!< Defined by the Bluetooth SIG. */
    gAdPublicTargetAddress_c                 = 0x17U,    /*!< Defined by the Bluetooth SIG. */
    gAdRandomTargetAddress_c                 = 0x18U,    /*!< Defined by the Bluetooth SIG. */
    gAdAppearance_c                          = 0x19U,    /*!< Defined by the Bluetooth SIG. */
    gAdAdvertisingInterval_c                 = 0x1AU,    /*!< Defined by the Bluetooth SIG. */
    gAdLeDeviceAddress_c                     = 0x1BU,    /*!< Defined by the Bluetooth SIG. */
    gAdLeRole_c                              = 0x1CU,    /*!< Defined by the Bluetooth SIG. */
    gAdSimplePairingHashC256_c               = 0x1DU,    /*!< Defined by the Bluetooth SIG. */
    gAdSimplePairingRandomizerR256_c         = 0x1EU,    /*!< Defined by the Bluetooth SIG. */
    gAd3dInformationData_c                   = 0x3DU,    /*!< Defined by the Bluetooth SIG. */
    gAdUniformResourceIdentifier_c           = 0x24U,    /*!< Defined by the Bluetooth SIG. */
    gAdLeSupportedFeatures_c                 = 0x27U,    /*!< Defined by the Bluetooth SIG. */
    gAdChannelMapUpdateIndication_c          = 0x28U,    /*!< Defined by the Bluetooth SIG. */
    gAdAdvertisingIntervalLong_c             = 0x2FU,    /*!< Defined by the Bluetooth SIG. */
    gAdEncryptedAdvertisingData_c            = 0x31U,    /*!< Defined by the Bluetooth SIG. */
    gAdManufacturerSpecificData_c            = 0xFFU     /*!< Defined by the Bluetooth SIG. */
} gapAdType_t;

/*! Values of the AD Flags advertising data structure. */
typedef uint8_t gapAdTypeFlags_t;

/*! Definition of an AD Structure as contained in Advertising and Scan Response packets. An Advertising or Scan Response packet contains several AD Structures. */
typedef struct
{
    uint8_t         length;     /*!< Total length of the [adType + aData] fields. Equal to 1 + lengthOf(aData). */
    gapAdType_t     adType;     /*!< AD Type of this AD Structure. */
    uint8_t*        aData;      /*!< Data contained in this AD Structure; length of this array is equal to (gapAdStructure_t.length - 1). */
} gapAdStructure_t;

/*! Advertising Data structure : a list of several gapAdStructure_t structures. */
typedef struct {
    uint8_t             cNumAdStructures;   /*!< Number of AD Structures. */
    gapAdStructure_t*   aAdStructures;      /*!< Array of AD Structures. */
} gapAdvertisingData_t;

/*! Extended Advertising Decision Data structure. */
typedef struct {
    uint8_t*      pKey;                /*!< Pointer to the key for the resolvable tag. When pKey != NULL Decision data should contain only arbitrary data or pDecisionData should be NULL. */
    uint8_t*      pPrand;              /*!<  Used only when pKey != NULL. When pPrand != NULL it should point the 3 bytes value of the  prand. Otherwise it will be generated by the host. */
    uint8_t*      pDecisionData;       /*!< Pointer to the decision data. When pDecisionData != NULL decision data may contain the resolvable tag and/or arbitrary data. */
    uint8_t       dataLength;          /*!< The length of the decision data pointed by pDecisionData. When pKey is NULL it should not exceed 8. Otherwise it should not exceed 2*/
    bool_t        resolvableTagPresent;/*!< When pKey is NULL it indicates whether decision data contains the resolvable tag. In this case the 6 <= dataLength <= 8 */
} gapAdvertisingDecisionData_t;

/*! Decision Instruction Relevant Field Type. */
typedef enum
{
    gDIRF_ResolvableTag_c                 = 0x00U,
    gDIRF_AdvMode_c                       = 0x06U,
    gDIRF_RSSI_c                          = 0x07U,
    gDIRF_PathLoss_c                      = 0x08U,
    gDIRF_AdvAddress_c                    = 0x09U,
    gDIRF_ArbitraryDataOfExactly_1Byte_c  = 0x11U,
    gDIRF_ArbitraryDataOfExactly_2Bytes_c = 0x12U,
    gDIRF_ArbitraryDataOfExactly_3Bytes_c = 0x13U,
    gDIRF_ArbitraryDataOfExactly_4Bytes_c = 0x14U,
    gDIRF_ArbitraryDataOfExactly_5Bytes_c = 0x15U,
    gDIRF_ArbitraryDataOfExactly_6Bytes_c = 0x16U,
    gDIRF_ArbitraryDataOfExactly_7Bytes_c = 0x17U,
    gDIRF_ArbitraryDataOfExactly_8Bytes_c = 0x18U,
    gDIRF_ArbitraryDataOfAtLeast_1Byte_c =  0x21U,/* 1 <= arbitrary data length <= 8*/
    gDIRF_ArbitraryDataOfAtLeast_2Bytes_c = 0x22U,/* 2 <= arbitrary data length <= 8*/
    gDIRF_ArbitraryDataOfAtLeast_3Bytes_c = 0x23U,/* 3 <= arbitrary data length <= 8*/
    gDIRF_ArbitraryDataOfAtLeast_4Bytes_c = 0x24U,/* 4 <= arbitrary data length <= 8*/
    gDIRF_ArbitraryDataOfAtLeast_5Bytes_c = 0x25U,/* 5 <= arbitrary data length <= 8*/
    gDIRF_ArbitraryDataOfAtLeast_6Bytes_c = 0x26U,/* 6 <= arbitrary data length <= 8*/
    gDIRF_ArbitraryDataOfAtLeast_7Bytes_c = 0x27U,/* 7 <= arbitrary data length <= 8*/
    gDIRF_ArbitraryDataOfAtLeast_8Bytes_c = 0x28U,/* 8 <= arbitrary data length <= 8 the same as gDIRF_ArbitraryDataOfExactly_8Bytes_c*/
    gDIRF_ArbitraryDataOfAtMost_1Byte_c   = 0x31U,/* 1 <= arbitrary data length <= 1 the same as gDIRF_ArbitraryDataOfExactly_1Bytes_c*/
    gDIRF_ArbitraryDataOfAtMost_2Bytes_c  = 0x32U,/* 1 <= arbitrary data length <= 2 */
    gDIRF_ArbitraryDataOfAtMost_3Bytes_c  = 0x33U,/* 1 <= arbitrary data length <= 3 */
    gDIRF_ArbitraryDataOfAtMost_4Bytes_c  = 0x34U,/* 1 <= arbitrary data length <= 4 */
    gDIRF_ArbitraryDataOfAtMost_5Bytes_c  = 0x35U,/* 1 <= arbitrary data length <= 5 */
    gDIRF_ArbitraryDataOfAtMost_6Bytes_c  = 0x36U,/* 1 <= arbitrary data length <= 6 */
    gDIRF_ArbitraryDataOfAtMost_7Bytes_c  = 0x37U,/* 1 <= arbitrary data length <= 7 */
    gDIRF_ArbitraryDataOfAtMost_8Bytes_c  = 0x38U,/* 1 <= arbitrary data length <= 8 the same as gDIRF_ArbitraryDataOfAtLeast_1Byte_c*/
}gapDecisionInstructionsRelevantField_t;

typedef enum
{
    gDITPC_Never_c                                = 0x00U, /* test always fails*/
    gDITPC_CheckPasses_c                          = 0x01U, /* The decision data contains the relevant field and the check passes*/
    gDITPC_CheckFails_c                           = 0x02U, /* The decision data contains the relevant field and the check fails*/
    gDITPC_RelevantFieldPresent_c                 = 0x03U, /* The decision data contains the relevant field, irrespective of whether the check passes */
    gDITPC_RelevantFieldNotPresent_c              = 0x04U, /* The decision data does not contain the relevant field */
    gDITPC_CheckPassesOrRelevantFieldNotPresent_c = 0x05U, /* Either the decision data contains the relevant field and the check passes, OR the decision data does not contain the relevant field */
    gDITPC_CheckFailsOrRelevantFieldNotPresent_c  = 0x06U, /* Either the decision data contains the relevant field and the check fails, OR the decision data does not contain the relevant field */
    gDITPC_Always_c                               = 0x07U, /* test always passes*/
}gapDecisionInstructionsTestPassCriteria_t;
typedef enum
{
    gDITG_SameTestGroup_c     = 0x00U, /* The test is in the same group as the previous test */
    gDITG_NewTestGroup_c      = 0x01U, /* The test is in a different group from all lower-numbered tests. The first test must be this type */
}gapDecisionInstructionsTestGroup_t;

/* Checks to be made when the relevant field is advertiser address*/
typedef enum
{
    gDIAAC_AdvAinFilterAcceptList_c       = 0x00U, /* The check passes if the AdvA field is for a device in the Filter Accept List. The Addresses are ignored. */
    gDIAAC_AdvAmatchAddress1_c            = 0x01U, /* The check passes if the AdvA field specifies the same device as Address 1. Address 2 is ignored. */
    gDIAAC_AdvAmatchAddress1orAddress2_c  = 0x02U, /* The check passes if the AdvA field specifies the same device as Address 1 or as Address 2. */
}gapDecisionInstructionsAdvAChecks_t;

/* Checks to be made when the relevant field is advertising mode. */
/* gbmpDIAM_NonConnectableNonScannableUndirected_c, gbmpDIAM_ConnectableUndirected_c, gbmpDIAM_ScannableUndirected_c */
/* Any bitwise OR operation of them is valid.*/
/*e.g.(gbmpDIAM_NonConnectableNonScannableUndirected_c | gbmpDIAM_ScannableUndirected_c) means the check passes for non-connectable undirected events.*/
typedef uint8_t gapDecisionInstructionsAdvMode_t;

typedef struct gapDecisionInstructionsData_tag
{
    gapDecisionInstructionsTestGroup_t         testGroup;
    gapDecisionInstructionsTestPassCriteria_t  passCriteria;
    gapDecisionInstructionsRelevantField_t     relevantField;
    union
    {
        uint8_t resolvableTagKey[gcDecisionDataKeySize_c]; /* test parameter for resolvable tag relevant field*/
        struct{
            uint8_t mask[gcDecisionInstructionsArbitraryDataMaskSize_c];
            uint8_t target[gcDecisionInstructionsArbitraryDataTargetSize_c];
        }arbitraryData; /* test parameter for arbitrary data relevant field*/
        struct{
            int8_t min;
            int8_t max;
        }rssi;/* test parameter for RSSI relevant field*/
        struct{
            uint8_t min;
            uint8_t max;
        }pathLoss;/* test parameter for path loss relevant field*/
        struct{
            gapDecisionInstructionsAdvAChecks_t check;
            bleAddressType_t                    address1Type;
            bleDeviceAddress_t                  address1;
            bleAddressType_t                    address2Type;
            bleDeviceAddress_t                  address2;
        }advA;/* test parameter for advertiser address relevant field*/
        gapDecisionInstructionsAdvMode_t advMode;/* test parameter for advertising mode relevant field*/
    }testParameters;
}gapDecisionInstructionsData_t;

/*! Scan Response Data structure : a list of several gapAdStructure_t structures. */
typedef gapAdvertisingData_t gapScanResponseData_t;

/*! Definition of a Subevent Data structure as contained in Periodic Advertising subevents. */
typedef struct
{
    uint8_t                     subevent;               /*!< Subevent index for this data. */
    uint8_t                     responseSlotStart;      /*!< The first response slots to be used in this subevent. */
    uint8_t                     responseSlotCount;      /*!< The number of response slots to be used. */
    gapAdvertisingData_t*       pAdvertisingData;       /*!< Advertising data. */
} gapSubeventDataStructure_t;

/*! Advertising Data structure : a list of several gapAdStructure_t structures. */
typedef struct {
    uint8_t                       cNumSubevents;             /*!< Number of subevents. */
    gapSubeventDataStructure_t*   aSubeventDataStructures;   /*!< Data for each subevent. */
} gapPeriodicAdvertisingSubeventData_t;

/*! Periodic Advertising Response data */
typedef struct
{
    uint16_t                    requestEvent;           /*!< The value of paEventCounter (see [Vol 6] Part B, Section 4.4.2.1) for
                                                             the periodic advertising packet that the Host is responding to */
    uint8_t                     requestSubevent;        /*!< The subevent for the periodic advertising packet that the Host is
                                                             responding to. */
    uint8_t                     responseSubevent;       /*!< Used to identify the subevent of the PAwR train. */
    uint8_t                     responseSlot;           /*!< Used to identify the response slot of the PAwR train. */
    gapAdvertisingData_t*       pResponseData;          /*!< Response data. */
} gapPeriodicAdvertisingResponseData_t;

/*! Periodic Sync Subevent Parameters */
typedef struct
{
    uint16_t perAdvProperties; /*!< Bit 6: Include TxPower in the advertising PDU */
    uint8_t  numSubevents;     /*!< Number of subevents. */
    uint8_t  aSubevents[1];    /*!< List of subevents */
} gapPeriodicSyncSubeventParameters_t;

/*! Enumeration used by the Gap_ReadRadioPowerLevel function. */
typedef enum {
    gTxPowerCurrentLevelInConnection_c = gReadCurrentTxPowerLevel_c,        /*!< Reading the instantaneous TX power level in a connection. */
    gTxPowerMaximumLevelInConnection_c = gReadMaximumTxPowerLevel_c,        /*!< Reading the maximum TX power level achieved during a connection. */
    gTxPowerLevelForAdvertising_c,                                          /*!< Reading the TX power on the advertising channels. */
    gRssi_c                                                                 /*!< Reading the Received Signal Strength Indication in a connection. */
} gapRadioPowerLevelReadType_t;

/*! Enumeration for Controller Test commands. */
typedef enum
{
    gControllerTestCmdStartRx_c,    /*!< Start Receiver Test. */
    gControllerTestCmdStartTx_c,    /*!< Start Transmitter Test. */
    gControllerTestCmdEnd_c,        /*!< End Test. */
} gapControllerTestCmd_t;

/*! Enumeration for Controller Transmitter Test payload types. */
typedef uint8_t gapControllerTestTxType_t;
typedef enum
{
    gControllerTestTxPrbs9_c,   /*!< PRBS9 sequence '11111111100000111101'... */
    gControllerTestTxF0_c,      /*!< Repeated '11110000' */
    gControllerTestTxAA_c,      /*!< Repeated '10101010' */
    gControllerTestTxPrbs15_c,  /*!< PRBS15 sequence */
    gControllerTestTxFF_c,      /*!< Repeated '11111111' */
    gControllerTestTx00_c,      /*!< Repeated '00000000' */
    gControllerTestTx0F_c,      /*!< Repeated '00001111' */
    gControllerTestTx55_c,      /*!< Repeated '01010101' */
} gapControllerTestTxType_tag;

/*! Enumeration for Sleep Clock Accuracy command. */
typedef enum
{
    gSwitchToMoreAccurateClock_c    = 0x00U,
    gSwitchToLessAccurateClock_c    = 0x01U,
}gapSleepClockAccuracy_t;

/*
*
* GAP Events
*
*/

/* Advertising events */

/*! Advertising event type enumeration, as contained in the gapAdvertisingEvent_t. */
typedef enum {
    gAdvertisingStateChanged_c    = 0x00U,      /*!< Event received when advertising has been successfully enabled or disabled. */
    gAdvertisingCommandFailed_c   = 0x01U,      /*!< Event received when advertising could not be enabled or disabled. Reason contained in gapAdvertisingEvent_t.eventData.failReason. */
    /* BLE 5.0: Advertising extensions */
    gExtAdvertisingStateChanged_c = 0x02U,      /*!< Event received when extended advertising has been successfully enabled or disabled. */
    gAdvertisingSetTerminated_c   = 0x03U,      /*!< Event received when advertising in a given advertising set has stopped. */
    gExtScanNotification_c        = 0x04U,      /*!< Event indicates that a SCAN_REQ PDU or an AUX_SCAN_REQ PDU has been received by the extended advertiser. */
    gPerAdvSubeventDataRequest_c  = 0x05U,      /*!< Event received when doing PAWR. */
    gPerAdvResponse_c             = 0x06U,      /*!< Periodic Advertising Response. */
} gapAdvertisingEventType_t;

typedef struct {
    uint8_t                         handle;                 /*!< Advertising Handle */
    bleAddressType_t                scannerAddrType;        /*!< Scanner device's address type. */
    bleDeviceAddress_t              aScannerAddr;           /*!< Scanner device's address. */
    bool_t                          scannerAddrResolved;    /*!< Whether the address corresponds to Resolved Private Address. */
}gapExtScanNotification_t;

typedef struct {
    bleResult_t                     status;                   /*!< Status of advertising set termination */
    uint8_t                         handle;                   /*!< Advertising Handle */
    deviceId_t                      deviceId;                 /*!< Valid only if the advertising ended with a connection */
    uint8_t                         numCompletedExtAdvEvents; /*!< Number of advertising events sent by Controller */
}gapAdvertisingSetTerminated_t;

typedef struct {
    uint8_t  handle;            /*!< Used to identify a periodic advertising train. */
    uint8_t  subeventStart;     /*!< The first subevent that data is requested for. */
    uint8_t  subeventDataCount; /*!<The number of subevents that data is requested for. */
}gapPerAdvSubeventDataRequest_t;

typedef struct {
    uint8_t advHandle;
    uint8_t subevent;
    int8_t  txPower;
    int8_t  rssi;
    uint8_t cteType;
    uint8_t responseSlot;
    uint8_t dataLength;
    uint8_t aData[255];
} gapPerAdvResponse_t;

/*! Advertising event structure: type + data. */
typedef struct {
    gapAdvertisingEventType_t eventType;    /*!< Event type. */
    union {
        bleResult_t                     failReason;         /*!< Event data for gAdvertisingCommandFailed_c event type: reason of failure to enable or disable advertising. */
        gapExtScanNotification_t        scanNotification;   /*!< Event data for gExtScanNotification_c event type: Scan Request Received Event. */
        gapAdvertisingSetTerminated_t   advSetTerminated;   /*!< Event received when advertising in a given advertising set has stopped. */
        gapPerAdvSubeventDataRequest_t  subeventDataRequest; /*!< Event received when doing PAWR. */
        gapPerAdvResponse_t             perAdvResponse;      /*!< Periodic Advertising Response. */
        uint8_t                         advHandle;          /*!< Event data for gExtAdvertisingStateChanged_c event type. */
    } eventData;                                            /*!< Event data, to be interpreted according to gapAdvertisingEvent_t.eventType. */
} gapAdvertisingEvent_t;

/* Scanning events */

/*! Scanning event type enumeration, as contained in the gapScanningEvent_t. */
typedef enum {
    gScanStateChanged_c               = 0x00U,   /*!< Event received when scanning had been successfully enabled or disabled, or a Scan duration time-out has occurred.*/
    gScanCommandFailed_c              = 0x01U,   /*!< Event received when scanning could not be enabled or disabled. Reason contained in gapScanningEvent_t.eventData.failReason. */
    gDeviceScanned_c                  = 0x02U,   /*!< Event received when an advertising device has been scanned. Device data contained in gapScanningEvent_t.eventData.scannedDevice. */
    /* BLE 5.0: Advertising extensions */
    gExtDeviceScanned_c               = 0x03U,   /*!< Event received when an advertising device has been scanned. Device data contained in gapScanningEvent_t.eventData.extScannedDevice. */
    gPeriodicDeviceScanned_c          = 0x04U,   /*!< Event received when an Periodic advertising device has been scanned. Device data contained in gapScanningEvent_t.eventData.periodicScannedDevice. */
    gPeriodicDeviceScannedV2_c        = 0x05U,   /*!< Event received when an Periodic advertising device has been scanned. Device data contained in gapScanningEvent_t.eventData.periodicScannedDeviceV2. */
    gPeriodicAdvSyncEstablished_c     = 0x06U,   /*!< Event received when a sync with a periodic advertiser was established. */
    gPeriodicAdvSyncLost_c            = 0x07U,   /*!< Event received when a sync with a periodic advertiser have been lost. */
    gPeriodicAdvSyncTerminated_c      = 0x08U,   /*!< Event received when a sync with a periodic advertiser have been terminated. */
    /* BLE 5.1 */
    gConnectionlessIqReportReceived_c = 0x09U,   /*!< Event received when the Controller has reported IQ information from the CTE of a received advertising packet */
    /* BLE 6.0 */
    gMonAdvReportEventReceived_c      = 0x0AU,   /*!< Event received when Monitored Advertisers is enabled */
} gapScanningEventType_t;

/*! Scanned device information structure, obtained from LE Advertising Reports. */
typedef struct {
    bleAddressType_t                addressType;    /*!< Device's advertising address type. */
    bleDeviceAddress_t              aAddress;       /*!< Device's advertising address. */
    int8_t                          rssi;           /*!< RSSI on the advertising channel; may be compared to the TX power contained in the AD Structure of type gAdTxPowerLevel_c to estimate distance from the advertiser.  */
    uint8_t                         dataLength;     /*!< Length of the advertising or scan response data. */
    uint8_t*                        data;           /*!< Advertising or scan response data. */
    bleAdvertisingReportEventType_t advEventType;   /*!< Advertising report type, indicating what type of event generated this data (advertising, scan response). */
    bool_t                          directRpaUsed;  /*!< TRUE if directed advertising with Resolvable Private Address as Direct Address was detected while Enhanced Privacy is enabled.  */
    bleDeviceAddress_t              directRpa;      /*!< Resolvable Private Address set as Direct Address for directed advertising. Valid only when directRpaUsed is TRUE. */
    bool_t                          advertisingAddressResolved;  /*!< If this is TRUE, the address contained in the addressType and aAddress fields is the identity address of a resolved RPA from the Advertising Address field.
                                                                      Otherwise, the address from the respective fields is the public or random device address contained in the Advertising Address field. */
} gapScannedDevice_t;

typedef struct
{
    bleAddressType_t               addressType;         /*!< Device's advertising address type. */
    bleDeviceAddress_t             aAddress;            /*!< Device's advertising address. */
    uint8_t                        SID;                 /*!< Advertising set id */
    bool_t                         advertisingAddressResolved;  /*!< If this is TRUE, the address contained in the addressType and aAddress fields is the identity address of a resolved RPA from the Advertising Address field.
                                                                     Otherwise, the address from the respective fields is the public or random device address contained in the Advertising Address field. */
    bleAdvReportEventProperties_t  advEventProperties;  /*!< Advertising report properties, indicating what type of event generated this data (advertising, scan response). */
    int8_t                         rssi;                /*!< RSSI on the advertising channel; may be compared to the TX power contained in the AD Structure of type gAdTxPowerLevel_c to estimate distance from the advertiser.  */
    int8_t                         txPower;             /*!< The Tx power level of the advertiser */
    uint8_t                        primaryPHY;          /*!< Advertiser PHY for primary channel*/
    uint8_t                        secondaryPHY;        /*!< Advertiser PHY for secondary channel*/
    uint16_t                       periodicAdvInterval; /*!< Interval of the periodic advertising. Zero if not periodic advertising. */
    bool_t                         directRpaUsed;       /*!< TRUE if directed advertising with Resolvable Private Address as Direct Address was detected while Enhanced Privacy is enabled.  */
    bleAddressType_t               directRpaType;       /*!< Address type for directed advertising. Valid only when directRpaUsed is TRUE. */
    bleDeviceAddress_t             directRpa;           /*!< Resolvable Private Address set as Direct Address for directed advertising. Valid only when directRpaUsed is TRUE. */
    uint16_t                       dataLength;          /*!< Length of the advertising or scan response data. */
    uint8_t*                       pData;               /*!< Advertising or scan response data. */
} gapExtScannedDevice_t;

typedef struct
{
    uint16_t                       syncHandle;          /*!< Sync Handle */
    int8_t                         txPower;             /*!< The Tx power level of the advertiser */
    int8_t                         rssi;                /*!< RSSI on the advertising channel; may be compared to the TX power contained in the AD Structure of type gAdTxPowerLevel_c to estimate distance from the advertiser.  */
    bleCteType_t                   cteType;             /*!< Type of Constant Tone Extension in the periodic advertising packets. */
    uint16_t                       dataLength;          /*!< Length of the advertising or scan response data. */
    uint8_t*                       pData;               /*!< Advertising or scan response data. */
} gapPeriodicScannedDevice_t;

typedef struct
{
    uint16_t                       syncHandle;          /*!< Sync Handle */
    int8_t                         txPower;             /*!< The Tx power level of the advertiser */
    int8_t                         rssi;                /*!< RSSI on the advertising channel; may be compared to the TX power contained in the AD Structure of type gAdTxPowerLevel_c to estimate distance from the advertiser.  */
    bleCteType_t                   cteType;             /*!< Type of Constant Tone Extension in the periodic advertising packets. */
    uint16_t                       periodicEventCounter; /*!<The value of paEventCounter (see [Vol 6] Part B, Section 4.4.2.1) for the reported periodic advertising packet. */
    uint8_t                        subevent;             /*!< Subevent number. */
    uint16_t                       dataLength;          /*!< Length of the advertising or scan response data. */
    uint8_t*                       pData;               /*!< Advertising or scan response data. */
} gapPeriodicScannedDeviceV2_t;

typedef struct
{
    bleResult_t                    status;              /*!< Status of the Sync Established Event */
    uint16_t                       syncHandle;          /*!< Sync Handle */
    uint8_t                        SID;                 /*!< Value of the Advertising SID subfield in the ADI field of the PDU */
    bleAddressType_t               peerAddressType;     /*!< Advertiser Address Type */
    bleDeviceAddress_t             peerAddress;         /*!< Advertiser Address */
    gapLePhyMode_t                 PHY;                 /*!< Advertiser PHY */
    uint16_t                       periodicAdvInterval; /*!< Periodic advertising interval */
    bleAdvertiserClockAccuracy_t   advertiserClockAccuracy; /*!< Advertiser Clock Accuracy */
    uint8_t                        numSubevents;        /*!< Only relevant for v2 of the event, otherwise value is 0 */
    uint8_t                        subeventInterval;    /*!< Only relevant for v2 of the event, otherwise value is 0 */
    uint8_t                        responseSlotDelay;   /*!< Only relevant for v2 of the event, otherwise value is 0 */
    uint8_t                        responseSlotSpacing; /*!< Only relevant for v2 of the event, otherwise value is 0 */
} gapSyncEstbEventData_t;

typedef struct
{
    uint16_t                       syncHandle;          /*!< Sync Handle */
} gapSyncLostEventData_t;

typedef struct
{
    uint16_t                        syncHandle;             /*!< Sync Handle. */
    uint8_t                         channelIndex;           /*!< Index of the channel on which the packet was received. */
    int16_t                         rssi;                   /*!< RSSI of the packet. */
    uint8_t                         rssiAntennaId;          /*!< Antenna ID. */
    bleCteType_t                    cteType;                /*!< Type of CTE on the received packet (AoA, AoD 1 us, AoD 2 us). */
    bleSlotDurations_t              slotDurations;          /*!< Durations of switching/sampling slots. */
    bleIqReportPacketStatus_t       packetStatus;           /*!< Packet status, information about CRC. */
    uint16_t                        periodicEventCounter;   /*!< The value of paEventCounter for the reported AUX_SYNC_IND PDU. */
    uint8_t                         sampleCount;            /*!< Total number of sample pairs. */
    int8_t                          *aI_samples;            /*!< List of sampleCount I_samples. */
    int8_t                          *aQ_samples;            /*!< List of sampleCount Q_samples. */
} gapConnectionlessIqReport_t;

typedef enum
{
    gBleMonAdvConditionRssiLowThreshold_c          = 0x00U, /*!< Monitored Advertisers RSSI value below the RSSI low threshold. */
    gBleMonAdvConditionRssiHighThreshold_c         = 0x01U, /*!< Monitored Advertisers RSSI value greater than or equal to the RSSI high threshold. */
} bleMonAdvCondition_t;

typedef struct
{
    bleAddressType_t                peerAddressType;        /*!< Advertiser Address Type */
    bleDeviceAddress_t              peerAddress;            /*!< Advertiser Address */
    bleMonAdvCondition_t            condition;              /*!< Condition parameter */
} gapMonAdvReportReport_t;

/*! Scanning event structure: type + data. */
typedef struct {
    gapScanningEventType_t eventType;   /*!< Event type. */
    union {
        bleResult_t                 failReason;       /*!< Event data for gScanCommandFailed_c or gPeriodicAdvSyncEstablished_c event type: reason of failure to enable/disable scanning or to establish sync. */
        gapScannedDevice_t          scannedDevice;    /*!< Event data for gDeviceScanned_c event type: scanned device information. */
        gapExtScannedDevice_t       extScannedDevice; /*!< Event data for gExtDeviceScanned_c event type: extended scanned device information. */
        gapPeriodicScannedDevice_t  periodicScannedDevice; /*!< Event data for gPeriodicDeviceScanned_c event type. */
        gapPeriodicScannedDeviceV2_t  periodicScannedDeviceV2; /*!< Event data for gPeriodicDeviceScannedV2_c event type. */
        gapSyncEstbEventData_t      syncEstb;         /*!< Event data for gPeriodicAdvSyncEstablished_c event type: Sync handle information for the application. */
        gapSyncLostEventData_t      syncLost;         /*!< Event data for gPeriodicAdvSyncLost_c event type: Sync handle information for the application. */
        gapConnectionlessIqReport_t iqReport;         /*!< Event data for gConnectionlessIqReportReceived_c event type: IQ information for the application. */
        gapMonAdvReportReport_t     monAdvReport;     /*!< Event data for gMonAdvReportEventReceived_c event type. */
    } eventData;                                     /*!< Event data, to be interpreted according to gapScanningEvent_t.eventType. */
} gapScanningEvent_t;

/* Connection events */

/*! Connection event type enumeration, as contained in the gapConnectionEvent_t. */
typedef enum {
    gConnEvtConnected_c                     = 0x00U, /*!< A connection has been established. Data in gapConnectionEvent_t.eventData.connectedEvent. */
    gConnEvtPairingRequest_c                = 0x01U, /*!< A pairing request has been received from the peer Central. Data in gapConnectionEvent_t.eventData.pairingEvent. */
    gConnEvtPeripheralSecurityRequest_c     = 0x02U, /*!< A Peripheral Security Request has been received from the peer Peripheral. Data in gapConnectionEvent_t.eventData.peripheralSecurityRequestEvent. */
    gConnEvtPairingResponse_c               = 0x03U, /*!< A pairing response has been received from the peer Peripheral. Data in gapConnectionEvent_t.eventData.pairingEvent. */
    gConnEvtAuthenticationRejected_c        = 0x04U, /*!< A link encryption or pairing request has been rejected by the peer device. Data in gapConnectionEvent_t.eventData.authenticationRejectedEvent. */
    gConnEvtPasskeyRequest_c                = 0x05U, /*!< Peer has requested a passkey (maximum 6 digit PIN) for the pairing procedure. Device should respond with Gap_EnterPasskey. */
    gConnEvtOobRequest_c                    = 0x06U, /*!< Out-of-Band data must be provided for the pairing procedure. Central or Peripheral should respond with Gap_ProvideOob. */
    gConnEvtPasskeyDisplay_c                = 0x07U, /*!< The pairing procedure requires this Peripheral to display the passkey for the Central's user. */
    gConnEvtKeyExchangeRequest_c            = 0x08U, /*!< The pairing procedure requires the SMP keys to be distributed to the peer. Data in gapConnectionEvent_t.eventData.keyExchangeRequestEvent. */
    gConnEvtKeysReceived_c                  = 0x09U, /*!< SMP keys distributed by the peer during pairing have been received. Data in gapConnectionEvent_t.eventData.keysReceivedEvent. */
    gConnEvtLongTermKeyRequest_c            = 0x0AU, /*!< The bonded peer Central has requested link encryption and the LTK must be provided. Peripheral should respond with Gap_ProvideLongTermKey. Data in gapConnectionEvent_t.eventData.longTermKeyRequestEvent. */
    gConnEvtEncryptionChanged_c             = 0x0BU, /*!< Link's encryption state has changed, e.g., during pairing or after a reconnection with a bonded peer. Data in gapConnectionEvent_t.eventData.encryptionChangedEvent. */
    gConnEvtPairingComplete_c               = 0x0CU, /*!< Pairing procedure is complete, either successfully or with failure. Data in gapConnectionEvent_t.eventData.pairingCompleteEvent. */
    gConnEvtDisconnected_c                  = 0x0DU, /*!< A connection has been terminated. Data in gapConnectionEvent_t.eventData.disconnectedEvent. */
    gConnEvtRssiRead_c                      = 0x0EU, /*!< RSSI for an active connection has been read. Data in gapConnectionEvent_t.eventData.rssi_dBm. */
    gConnEvtTxPowerLevelRead_c              = 0x0FU, /*!< TX power level for an active connection has been read. Data in gapConnectionEvent_t.eventData.txPowerLevel_dBm. */
    gConnEvtPowerReadFailure_c              = 0x10U, /*!< Power reading could not be performed. Data in gapConnectionEvent_t.eventData.failReason. */
    gConnEvtParameterUpdateRequest_c        = 0x11U, /*!< A connection parameter update request has been received. Data in gapConnectionEvent_t.eventData.connectionUpdateRequest. */
    gConnEvtParameterUpdateComplete_c       = 0x12U, /*!< The connection has new parameters. Data in gapConnectionEvent_t.eventData.connectionUpdateComplete. */
    gConnEvtLeDataLengthChanged_c           = 0x13U, /*!< The new TX/RX Data Length parameters. Data in gapConnectionEvent_t.eventData.rssi_dBm.leDataLengthChanged. */
    gConnEvtLeScOobDataRequest_c            = 0x14U, /*!< Event sent to request LE SC OOB Data (r, Cr and Addr) received from a peer. */
    gConnEvtLeScDisplayNumericValue_c       = 0x15U, /*!< Event sent to display and confirm a Numeric Comparison Value when using the LE SC Numeric Comparison pairing method. */
    gConnEvtLeScKeypressNotification_c      = 0x16U, /*!< Remote Keypress Notification received during Passkey Entry Pairing Method. */
    gConnEvtChannelMapRead_c                = 0x17U, /*!< Channel Map was read for a connection. Data is contained in gapConnectionEvent_t.eventData.channelMap */
    gConnEvtChannelMapReadFailure_c         = 0x18U, /*!< Channel Map reading could not be performed. Data in gapConnectionEvent_t.eventData.failReason. */
    /* BLE 5.0: Advertising extensions */
    gConnEvtChanSelectionAlgorithm2_c       = 0x19U, /*!< LE Channel Selection Algorithm #2 is used on the data channel connection. */

    gConnEvtPairingNoLtk_c                  = 0x1AU, /*!< No LTK was found for the Central peer. Pairing shall be performed again. */
    gConnEvtPairingAlreadyStarted_c         = 0x1BU, /*!< Pairing process was already started */

    /* BLE 5.1 */
    gConnEvtIqReportReceived_c                  = 0x1CU, /*!< Controller has reported IQ information received from a connected peer. Data in gapConnectionEvent_t.eventData.connIqReport. */
    gConnEvtCteRequestFailed_c                  = 0x1DU, /*!< CTE Request to a connected peer has failed. Data in gapConnectionEvent_t.eventData.cteRequestFailedEvent. */
    gConnEvtCteReceiveParamsSetupComplete_c     = 0x1EU, /*!< Connection CTE receive parameters have been successfully set. */
    gConnEvtCteTransmitParamsSetupComplete_c    = 0x1FU, /*!< Connection CTE transmit parameters have been successfully set. */
    gConnEvtCteReqStateChanged_c                = 0x20U, /*!< Controller started or stopped initiating the CTE Request procedure. */
    gConnEvtCteRspStateChanged_c                = 0x21U, /*!< Controller enabled or disabled sending CTE Responses for a connection. */
    /* BLE 5.2 */
    gConnEvtPathLossThreshold_c                     = 0x22U, /*!< Received a Path Loss Threshold event. Data in gapConnectionEvent_t.eventData.pathLossThreshold. */
    gConnEvtTransmitPowerReporting_c                = 0x23U, /*!< Received a Transmit Power report. Data in gapConnectionEvent_t.eventData.transmitPowerReporting. */
    gConnEvtEnhancedReadTransmitPowerLevel_c        = 0x24U, /*!< Local information has been read from Controller. Data in gapConnectionEvent_t.eventData.transmitPowerInfo. */
    gConnEvtPathLossReportingParamsSetupComplete_c  = 0x25U, /*!< Path Loss Reporting parameters have been successfully set. */
    gConnEvtPathLossReportingStateChanged_c         = 0x26U, /*!< Path Loss Reporting has been enabled or disabled. */
    gConnEvtTransmitPowerReportingStateChanged_c    = 0x27U, /*!< Transmit Power Reporting has been enabled or disabled for local and/or remote Controllers. */
    /* BLE 5.2 EATT */
    gConnEvtEattConnectionRequest_c                 = 0x28U, /*!< Received Enhanced Connection Request for EATT channels. */
    gConnEvtEattConnectionComplete_c                = 0x29U, /*!< Received Enhanced Connection Response for EATT channels. */
    gConnEvtEattChannelReconfigureResponse_c        = 0x2AU, /*!< Received Enhanced Channel Reconfigure Response for EATT channels. */
    gConnEvtEattBearerStatusNotification_c          = 0x2BU, /*!< Enhanced Bearer status updated */
    /* Handover */
    gConnEvtHandoverConnected_c                     = 0x2CU, /*!< A connection has been established through the Handover feature. Data in gapConnectionEvent_t.eventData.handoverConnectedEvent. */
    gHandoverDisconnected_c                         = 0x2DU, /*!< A connection has been terminated as a result of connection handover. */

    gConnEvtLeSetDataLengthFailure_c                = 0x2EU, /*!< The Set Data Length command has failed. */
    gConnEvtSmError_c                               = 0x2FU, /*!< Security Manager error occured. */
    gConnEvtLeSubrateChange_c                       = 0x30U, /*!< The connection has new parameters. Data in gapConnectionEvent_t.eventData.gapSubrateChangeEvent. */
} gapConnectionEventType_t;

/*! Event data structure for the gConnEvtConnected_c event. */
typedef struct {
    gapConnectionParameters_t   connParameters;         /*!< Connection parameters established by the Controller. */
    bleAddressType_t            peerAddressType;        /*!< Connected device's address type. */
    bleDeviceAddress_t          peerAddress;            /*!< Connected device's address. */
    bool_t                      peerRpaResolved;        /*!< If this is TRUE, the address defined by peerAddressType and peerAddress is the identity address of the peer, and the peer used an RPA
                                                             that was resolved by the Controller and is contained in the peerRpa field. Otherwise, it is a device address. This parameter is irrelevant
                                                             if Controller Privacy is not enabled. */
    bleDeviceAddress_t          peerRpa;                /*!< Peer Resolvable Private Address if Controller Privacy is active and peerRpaResolved is TRUE. */
    bool_t                      localRpaUsed;           /*!< If this is TRUE, the Controller has used an RPA contained in the localRpa field. This parameter is irrelevant
                                                             if Controller Privacy is not enabled. */
    bleDeviceAddress_t          localRpa;               /*!< Local Resolvable Private Address if Controller Privacy is active and localRpaUsed is TRUE. */
    bleLlConnectionRole_t       connectionRole;         /*!< Connection Role - central or peripheral. */
    uint8_t                     advHandle;              /*!< Only relevant if the connection was from PAWR. */
    uint16_t                    syncHandle;             /*!< Only relevant if the connection was from PAWR. */
} gapConnectedEvent_t;

/*! Event data structure for the gConnEvtKeyExchangeRequest_c event. */
typedef struct {
    gapSmpKeyFlags_t    requestedKeys;      /*!< Mask identifying the keys being requested. */
    uint8_t             requestedLtkSize;   /*!< Requested size of the encryption key. */
} gapKeyExchangeRequestEvent_t;

/*! Event data structure for the gConnEvtKeysReceived_c event. */
typedef struct {
    gapSmpKeys_t*        pKeys;   /*!< The SMP keys distributed by the peer. */
} gapKeysReceivedEvent_t;

/*! Event data structure for the gConnEvtAuthenticationRejected_c event. */
typedef struct {
    gapAuthenticationRejectReason_t rejectReason;   /*!< Peripheral's reason for rejecting the authentication. */
} gapAuthenticationRejectedEvent_t;

/*! Event data structure for the gConnEvtPairingComplete_c event. */
typedef struct {
    bool_t    pairingSuccessful;    /*!< TRUE if pairing succeeded, FALSE otherwise. */
    union {
        bool_t      withBonding;        /*!< If pairingSuccessful is TRUE, this indicates whether the devices bonded. */
        bleResult_t failReason;         /*!< If pairingSuccessful is FALSE, this contains the reason of failure. */
    } pairingCompleteData;          /*!< Information of completion, selected upon the value of gapPairingCompleteEvent_t.pairingSuccessful. */
} gapPairingCompleteEvent_t;

/*! Event data structure for the gConnEvtLongTermKeyRequest_c event. */
typedef struct {
    uint16_t ediv;                          /*!< The Encryption Diversifier, as defined by the SMP. */
    uint8_t  aRand[gcSmpMaxRandSize_c];    /*!< The Random number, as defined by the SMP.  */
    uint8_t  randSize;                      /*!< Usually equal to gcMaxRandSize_d. */
} gapLongTermKeyRequestEvent_t;

/*! Event data structure for the gConnEvtEncryptionChanged_c event. */
typedef struct {
    bool_t    newEncryptionState;   /*!< TRUE if link has been encrypted, FALSE if encryption was paused or removed. */
} gapEncryptionChangedEvent_t;

/*! Disconnection reason alias - reasons are contained in HCI error codes. */
typedef bleResult_t gapDisconnectionReason_t;

/*! Event data structure for the gConnEvtDisconnected_c event. */
typedef struct {
    gapDisconnectionReason_t    reason;     /*!< Reason for disconnection. */
} gapDisconnectedEvent_t;

/*! Event data structure for the gConnEvtParameterUpdateRequest_c event. */
typedef struct {
    uint16_t    intervalMin;        /*!< Minimum interval between connection events. */
    uint16_t    intervalMax;        /*!< Maximum interval between connection events. */
    uint16_t    peripheralLatency;  /*!< Number of consecutive connection events the Peripheral may ignore. */
    uint16_t    timeoutMultiplier;  /*!< The maximum time interval between consecutive over-the-air packets; if this timer expires, the connection is dropped. */
} gapConnParamsUpdateReq_t;

/*! Event data structure for the gConnEvtParameterUpdateComplete_c event. */
typedef struct {
    bleResult_t status;
    uint16_t    connInterval;       /*!< Interval between connection events. */
    uint16_t    connLatency;        /*!< Number of consecutive connection events the Peripheral may ignore. */
    uint16_t    supervisionTimeout; /*!< The maximum time interval between consecutive over-the-air packets; if this timer expires, the connection is dropped. */
} gapConnParamsUpdateComplete_t;

/*! Event data structure for the gConnEvtLeDataLengthChanged_c event. */
typedef struct {
    uint16_t    maxTxOctets;       /*!< The maximum number of payload octets in a Link Layer Data Channel PDU to transmit on this connection. */
    uint16_t    maxTxTime;         /*!< The maximum time that the local Controller will take to send a Link Layer Data Channel PDU on this connection. */
    uint16_t    maxRxOctets;       /*!< The maximum number of payload octets in a Link Layer Data Channel PDU to receive on this connection. */
    uint16_t    maxRxTime;         /*!< The maximum time that the local Controller will take to receive a Link Layer Data Channel PDU on this connection. */
} gapConnLeDataLengthChanged_t;

/*! Event data structure for the gConnEvtIqReportReceived_c event. */
typedef struct {
    gapLePhyMode_t                  rxPhy;              /*!< Identifies receiver PHY for this connection. */
    uint8_t                         dataChannelIndex;   /*!< Index of the data channel on which the Data Physical Channel PDU was received. */
    int16_t                         rssi;               /*!< RSSI of the packet. */
    uint8_t                         rssiAntennaId;      /*!< Antenna ID. */
    bleCteType_t                    cteType;            /*!< Type of CTE on the received packet (AoA, AoD 1 us, AoD 2 us). */
    bleSlotDurations_t              slotDurations;      /*!< Durations of switching/sampling slots. */
    bleIqReportPacketStatus_t       packetStatus;       /*!< Packet status, information about CRC. */
    uint16_t                        connEventCounter;   /*!< The value of the connEventCounter for the reported PDU. */
    uint8_t                         sampleCount;        /*!< Total number of sample pairs. */
    int8_t                          *aI_samples;        /*!< List of sampleCount I_samples */
    int8_t                          *aQ_samples;        /*!< List of sampleCount Q_samples */
} gapConnIqReport_t;

/*! Event data structure for the gConnEvtCteRequestFailed_c event. */
typedef struct {
    bleResult_t     status;     /*!< Status (0x00 - received response but without CTE, 0x01-0xFF - peer rejected request */
} gapConnCteRequestFailed_t;

/*! Event data structure for the gConnEvtPathLossThreshold_c event. */
typedef struct {
    uint8_t                             currentPathLoss;    /*!< Current path loss. Units: dB. */
    blePathLossThresholdZoneEntered_t   zoneEntered;        /*!< Low, middle or high. */
} gapPathLossThresholdEvent_t;

/*! Event data structure for the gConnEvtTransmitPowerReporting_c event. */
typedef struct {
    bleTxPowerReportingReason_t         reason;         /*!< Reason for generating this event
                                                             - local tx power changed
                                                             - remote tx power changed
                                                             - a HCI_LE_Read_Remote_Transmit_Power_Level command completed */
    blePowerControlPhyType_t            phy;            /*!< The PHY for which the event is generated. */
    int8_t                              txPowerLevel;   /*!< New transmit power level on this connection and PHY. */
    bleTxPowerLevelFlags_t              flags;          /*!< Tx power level flags. */
    int8_t                              delta;          /*!< Change in tx power level. Units: dB. */
} gapTransmitPowerReporting_t;

/*! Event data structure for the gConnEvtLeSubrateChange_c event. */
typedef struct {
    bleResult_t status;
    uint16_t                            subrateFactor;     /*!< subrate factor applied to the underlying connection interval. */
    uint16_t                            peripheralLatency; /*!< Peripheral latency for the connection in units of subrated connection intervals. */
    uint16_t                            continuationNumber; /*!< Minimum number of underlying connection events to remain active
                                                                 after a packet containing a Link Layer PDU with a non-zero Length field
                                                                 is sent or received. */
    uint16_t                            supervisionTimeout; /*!< The maximum time interval between consecutive over-the-air packets in 10ms units; if this timer expires, the connection is dropped. */
} gapSubrateChangeEvent_t;

/*! Event data structure for the gConnEvtEnhancedReadTransmitPowerLevel_c event. */
typedef struct {
    blePowerControlPhyType_t    phy;                /*!< The PHY for which the event is generated. */
    int8_t                      currTxPowerLevel;   /*!< Current tx power level on this connection and PHY. */
    int8_t                      maxTxPowerLevel;    /*!< Maximum tx power level on this connection and PHY. */
} gapTransmitPowerInfo_t;

/*! Event data structure for the gConnEvtEattConnectionRequest_c event. */
typedef struct {
    uint16_t            mtu;
    uint8_t             cBearers;
    uint16_t            initialCredits;
} gapEattConnectionRequest_t;

/*! Event data structure for the gConnEvtEattConnectionComplete_c event. */
typedef struct {
    l2caLeCbConnectionRequestResult_t           status;
    uint16_t                                    mtu;
    uint8_t                                     cBearers;
    bearerId_t                                  aBearerIds[gGapEattMaxBearers];
} gapEattConnectionComplete_t;

/*! Event data structure for the gConnEvtEattChannelReconfigureResponse_c event. */
typedef struct {
    l2capReconfigureResponse_t          status;
    uint16_t                            localMtu;
    uint8_t                             cBearers;
    bearerId_t                          aBearerIds[gGapEattMaxBearers];
} gapEattReconfigureResponse_t;

/*! Enumeration for Bearer Status Notification command status. */
typedef enum
{
    gEnhancedBearerActive_c                             = 0x00U,
    gEnhancedBearerSuspendedNoLocalCredits_c            = 0x01U,
    gEnhancedBearerNoPeerCredits_c                      = 0x02U,
    gEnhancedBearerDisconnected_c                       = 0x03U,
    gEnhancedBearerStatusEnd_c,
}gapEattBearerStatus_t;

/*! Event data structure for the bearer status update event. */
typedef struct {
    bearerId_t                  bearerId;
    gapEattBearerStatus_t       status;
} gapEattBearerStatusNotification_t;

/*! Event data structure for the gConnEvtHandoverConnected_c event. */
typedef struct {
    bleAddressType_t            peerAddressType;        /*!< Connected device's address type. */
    bleDeviceAddress_t          peerAddress;            /*!< Connected device's address. */
    bleLlConnectionRole_t       connectionRole;         /*!< Connection Role - master or slave. */
} gapHandoverConnectedEvent_t;

/*! Event data structure for the gHandoverDisconnected_c event. */
typedef struct {
    bleResult_t     status;     /*!< Status for the handover disconnect command */
} gapHandoverDisconnectedEvent_t;

/*! Connection event structure: type + data. */
typedef struct {
    gapConnectionEventType_t eventType;  /*!< Event type */
    union {
        gapConnectedEvent_t                 connectedEvent;                 /*!< Data for gConnEvtConnected_c: information about the connection parameters. */
        gapPairingParameters_t              pairingEvent;                   /*!< Data for gConnEvtPairingRequest_c, gConnEvtPairingResponse_c: pairing parameters. */
        gapAuthenticationRejectedEvent_t    authenticationRejectedEvent;    /*!< Data for gConnEvtAuthenticationRejected_c: reason for rejection. */
        gapPeripheralSecurityRequestParameters_t    peripheralSecurityRequestEvent; /*!< Data for gConnEvtPeripheralSecurityRequest_c: Peripheral's security requirements. */
        gapKeyExchangeRequestEvent_t        keyExchangeRequestEvent;        /*!< Data for gConnEvtKeyExchangeRequest_c: mask indicating the keys that were requested by the peer. */
        gapKeysReceivedEvent_t              keysReceivedEvent;              /*!< Data for gConnEvtKeysReceived_c: the keys received from the peer. */
        gapPairingCompleteEvent_t           pairingCompleteEvent;           /*!< Data for gConnEvtPairingComplete_c: fail reason or (if successful) bonding state. */
        gapLongTermKeyRequestEvent_t        longTermKeyRequestEvent;        /*!< Data for gConnEvtLongTermKeyRequest_c: encryption diversifier and random number. */
        gapEncryptionChangedEvent_t         encryptionChangedEvent;         /*!< Data for gConnEvtEncryptionChanged_c: new encryption state. */
        gapDisconnectedEvent_t              disconnectedEvent;              /*!< Data for gConnEvtDisconnected_c: reason for disconnection. */
        int8_t                              rssi_dBm;                       /*!< Data for gConnEvtRssiRead_c: value of the RSSI in dBm. */
        int8_t                              txPowerLevel_dBm;               /*!< Data for gConnEvtTxPowerLevelRead_c: value of the TX power. */
        bleResult_t                         failReason;                     /*!< Data for gConnEvtPowerReadFailure_c: reason for power reading failure. */
        uint32_t                            passkeyForDisplay;
        gapConnParamsUpdateReq_t            connectionUpdateRequest;        /*!< Data for gConnEvtParameterUpdateRequest_c: connection parameters update. */
        gapConnParamsUpdateComplete_t       connectionUpdateComplete;       /*!< Data for gConnEvtParameterUpdateComplete_c: connection parameters update. */
        gapConnLeDataLengthChanged_t        leDataLengthChanged;            /*!< Data for gConnEvtLeDataLengthChanged_c: new data length parameters */
        gapKeypressNotification_t           incomingKeypressNotification;
        uint32_t                            numericValueForDisplay;
        bleChannelMap_t                     channelMap;                      /*!< Data for gConnEvtChannelMapRead_c: channel map read from the Controller */
        gapConnIqReport_t                   connIqReport;                    /*!< Data for gConnEvtIqReportReceived_c: IQ information received from a peer */
        gapConnCteRequestFailed_t           cteRequestFailedEvent;           /*!< Data for gConnEvtCteRequestFailed_c: CTE Request to peer has failed */
        bleResult_t                         perAdvSyncTransferStatus;
        gapPathLossThresholdEvent_t         pathLossThreshold;               /*!< Data for gConnEvtPathLossThreshold_c: current path loss and zone entered. */
        gapTransmitPowerReporting_t         transmitPowerReporting;          /*!< Data for gConnEvtTransmitPowerReporting_c: changes in power levels. */
        gapTransmitPowerInfo_t              transmitPowerInfo;               /*!< Data for gConnEvtEnhancedReadTransmitPowerLevel_c: local power level information. */
        gapEattConnectionRequest_t          eattConnectionRequest;           /*!< Data for gConnEvtEattConnectionRequest_c: notification of L2Cap Enhanced Connect request for EATT. */
        gapEattConnectionComplete_t         eattConnectionComplete;          /*!< Data for gConnEvtEattConnectionComplete_c: result of Gap_EattConnectionRequest(). */
        gapEattReconfigureResponse_t        eattReconfigureResponse;         /*!< Data for gConnEvtEattChannelReconfigureResponse_c: result of Gap_EattReconfigureRequest(). */
        gapEattBearerStatusNotification_t   eattBearerStatusNotification;    /*!< Data for gConnEvtEattBearerStatusNotification_c: bearer status update. */
        gapHandoverConnectedEvent_t         handoverConnectedEvent;          /*!< Data for gConnEvtHandoverConnected_c: information about the connection parameters. */
        gapHandoverDisconnectedEvent_t      handoverDisconnectedEvent;       /*!< Data for gHandoverDisconnected_c: status of the Gap_HandoverDisconnect. */
        bleResult_t                         smError;                         /*!< Data for gConnEvtSmError_c: SM error status. */
        gapSubrateChangeEvent_t             gapSubrateChangeEvent;           /*!< Data for gConnEvtLeSubrateChange_c: connection parameters update. */
    } eventData;                        /*!< Event data, to be interpreted according to gapConnectionEvent_t.eventType. */
} gapConnectionEvent_t;

/*! Central Address Resolution characteristic state */
typedef enum
{
    CAR_Unknown,     /*!< The Central Address Resolution characteristic was not read */
    CAR_Unavailable, /*!< The device tried to read the Central Address Resolution characteristic, but it's unavailable */
    CAR_Unsupported, /*!< The device has read the Central Address Resolution characteristic, and the it's value is FALSE */
    CAR_Supported,   /*!< The device has read the Central Address Resolution characteristic, and the it's value is TRUE */
}gapCarSupport_t;

/*! Identity Information structure definition */
typedef struct
{
    bleIdentityAddress_t    identityAddress;        /*!< Identity Address - Public or Random Static */
    uint8_t                 irk[gcSmpIrkSize_c];    /*!< Identity Resolving Key - must not be all-zero if Network Privacy is used */
    blePrivacyMode_t        privacyMode;            /*!< Privacy mode to be used for the entry on the resolving list */
} gapIdentityInformation_t;

/*
*
* GAP Callbacks
*
*/

/*! Advertising Callback prototype. */
typedef void (*gapAdvertisingCallback_t)
(
    gapAdvertisingEvent_t* pAdvertisingEvent    /*!< Advertising event. */
);

/*! Scanning Callback prototype. */
typedef void (*gapScanningCallback_t)
(
    gapScanningEvent_t* pScanningEvent          /*!< Scanning event. */
);

/*! Connection Callback prototype. */
typedef void (*gapConnectionCallback_t)
(
    deviceId_t              deviceId,           /*!< Device ID identifying the connection. */
    gapConnectionEvent_t*   pConnectionEvent    /*!< Connection event. */
);

/*! Parameters for the Auto Connect Scan Mode. */
typedef struct {
    uint8_t                             cNumAddresses;              /*!< Number of device addresses to automatically connect to. */
    bool_t                              writeInFilterAcceptList;    /*!< If set to TRUE, the device addresses are written in the Filter Accept List before scanning is enabled. */
    gapConnectionRequestParameters_t*   aAutoConnectData;           /*!< The array of connection request parameters, of size equal to cNumAddresses. */
} gapAutoConnectParams_t;

/*
*
* GAP Service
*
*/

typedef enum {
    gAddDevice_c        = 0x00,
    gRemoveDevice_c     = 0x01,
    gRemoveAllDevices_c = 0x02,
}gapPeriodicAdvListOperation_t;

/*! Appearance characteristic enumeration, also used in advertising. */
typedef enum gapAppearance_tag {
    gUnknown_c                              = 0,
    gGenericPhone_c                         = 64,
    gGenericComputer_c                      = 128,
    gGenericWatch_c                         = 192,
    gSportsWatch_c                          = 193,
    gGenericClock_c                         = 256,
    gGenericDisplay_c                       = 320,
    gGenericRemoteControl_c                 = 384,
    gGenericEyeglasses_c                    = 448,
    gGenericTag_c                           = 512,
    gGenericKeyring_c                       = 576,
    gGenericMediaPlayer_c                   = 640,
    gGenericBarcodeScanner_c                = 704,
    gGenericThermometer_c                   = 768,
    gThermometerEar_c                       = 769,
    gGenericHeartrateSensor_c               = 832,
    gHeartRateSensorHeartRateBelt_c         = 833,
    gGenericBloodPressure_c                 = 896,
    gBloodPressureArm_c                     = 897,
    gBloodPressureWrist_c                   = 898,
    gHumanInterfaceDevice_c                 = 960,
    gKeyboard_c                             = 961,
    gMouse_c                                = 962,
    gJoystick_c                             = 963,
    gGamepad_c                              = 964,
    gDigitizerTablet_c                      = 965,
    gCardReader_c                           = 966,
    gDigitalPen_c                           = 967,
    gBarcodeScanner_c                       = 968,
    gGenericGlucoseMeter_c                  = 1024,
    gGenericRunningWalkingSensor_c          = 1088,
    gRunningWalkingSensorInShoe_c           = 1089,
    gRunningWalkingSensorOnShoe_c           = 1090,
    gRunningWalkingSensorOnHip_c            = 1091,
    gGenericCycling_c                       = 1152,
    gCyclingComputer_c                      = 1153,
    gCyclingSpeedSensor_c                   = 1154,
    gCyclingCadenceSensor_c                 = 1155,
    gCyclingPowerSensor_c                   = 1156,
    gCyclingSpeedandCadenceSensor_c         = 1157,
    gGenericPulseOximeter_c                 = 3136,
    gFingertip_c                            = 3137,
    gWristWorn_c                            = 3138,
    gGenericWeightScale_c                   = 3200,
    gGenericOutdoorSportsActivity_c         = 5184,
    gLocationDisplayDevice_c                = 5185,
    gLocationandNavigationDisplayDevice_c   = 5186,
    gLocationPod_c                          = 5187,
    gLocationAndNavigationPod_c             = 5188
}gapAppearance_t;

/* Host Version */
typedef struct gapHostVersion_tag
{
    uint8_t bleHostVerMajor;
    uint8_t bleHostVerMinor;
    uint8_t bleHostVerPatch;
} gapHostVersion_t;

#endif /* GAP_TYPES_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
