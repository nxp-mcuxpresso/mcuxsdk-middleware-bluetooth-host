/*! *********************************************************************************
 * \defgroup BLE_CONFIG BLE Configuration Constants
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

#ifndef BLE_CONSTANTS_H
#define BLE_CONSTANTS_H

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
/*! Size of a BLE Device Address */
#define gcBleDeviceAddressSize_c        (6U)

/*! Size of bond data structures for a bonded device  */
/* gBleBondIdentityHeaderSize_c defined in app_preinclude_common.h */

#define gBleBondDataDynamicSize_c       (8U)

#if ((defined(gAppSecureMode_d) && (gAppSecureMode_d == 1)) || (defined(gHostSecureMode_d) && (gHostSecureMode_d == 1)))
#define gBleBondDataStaticSize_c        (54U)
#else /* (gAppSecureMode_d == TRUE) || (gHostSecureMode_d == TRUE) */
#define gBleBondDataStaticSize_c        (30U)
#endif /* (gAppSecureMode_d == TRUE) || (gHostSecureMode_d == TRUE) */

#define gBleBondDataLegacySize_c        (28U)
#define gBleBondDataDeviceInfoSize_c    (60U)
#define gBleBondDataDescriptorSize_c    (4U)

#define gcSecureModeSavedLocalKeysNo_c  (2U)

/*! Maximum number of CCCDs */
#ifndef gcGapMaximumSavedCccds_c
#define gcGapMaximumSavedCccds_c        (5U)
#endif

/*! Maximum number of attributes that require authorization */
#define gcGapMaxAuthorizationHandles_c  (1U)

/*! Bonding Data Size */
#define gBleBondDataSize_c              (gBleBondIdentityHeaderSize_c + gBleBondDataDynamicSize_c + \
                                         gBleBondDataStaticSize_c + gBleBondDataDeviceInfoSize_c  + \
                                        (gBleBondDataDescriptorSize_c * gcGapMaximumSavedCccds_c))

/*! Size of long UUIDs */
#define gcBleLongUuidSize_c             ((uint16_t)(16U))

/*! Maximum Long Term Key size in bytes */
#define gcSmpMaxLtkSize_c               (16U)

/*! Maximum Long Term Key blob size in bytes */
#define gcSmpMaxBlobSize_c              (40U)

/*! Identity Resolving Key size in bytes */
#define gcSmpIrkSize_c                  (16U)

/*! Maximum Identity Resolving Key blob size in bytes */
#define gcSmpMaxIrkBlobSize_c           (40U)

/*! Connection Signature Resolving Key size in bytes */
#define gcSmpCsrkSize_c                 (16U)

/*! Maximum Rand size in bytes */
#define gcSmpMaxRandSize_c              (8U)

/*! SMP OOB size in bytes */
#define gcSmpOobSize_c  (16U)

/*! SMP LE Secure Connections Pairing Random size in bytes */
#define gSmpLeScRandomValueSize_c           (16U)

/*! SMP LE Secure Connections Pairing Confirm size in bytes */
#define gSmpLeScRandomConfirmValueSize_c    (16U)

/*! Maximum device name size */
#define gcGapMaxDeviceNameSize_c        (19U)

/*! Maximum size of advertising and scan response data */
#define gcGapMaxAdvertisingDataLength_c (31U)

/*! Default value of the ATT_MTU */
#define gAttDefaultMtu_c                (23U)

/*! Maximum possible value of the ATT_MTU for this device. This is used during the MTU Exchange. */
#define gAttMaxMtu_c                    (247U)

/*! Maximum possible value of the ATT_MTU for enhanced ATT bearers on this device. */
#define gEattMaxMtu_c                   (247U)

/*! The maximum length of an attribute value shall be 512 octets. */
#define gAttMaxValueLength_c            (512U)

/*! Minimum MTU size for enhanced ATT channels is 64. */
#define gEattMinMtu_c                   (64U)

/*! Channel the number of the UART hardware module (For example, if UART1 is used, this value should be 1). */
#define gHciTransportUartChannel_c                  (1U)

/*! Number of bytes reserved for storing application-specific information about a device */
#define gcReservedFlashSizeForCustomInformation_c   (40U)

/*! Size of a channel map in a connection */
#define gcBleChannelMapSize_c           (5U)

#define gBleMinTxOctets_c       (27U)
#define gBleMinTxTime_c         (328U)
#define gBleMaxTxOctets_c       (251U)      /*! Data Packet Length Extension, regardless of PHY */
#define gBleMaxTxTime_c         (2120U)     /*! Data Packet Length Extension, Coded PHY not supported */
#define gBleMaxTxTimeCodedPhy_c (17040U)    /*! Data Packet Length Extension, Coded PHY supported */

#define gBleCteMinTxOctets_c    (27U)       /*! Minimum number of octets transmitted in the payload
                                             *  taking into account CTE, regardless of PHY */
#define gBleCteMinTxTime_c      (328U)      /*! Minimum number of microseconds taken to transmit a packet
                                             *  taking into account CTE, regardless of PHY */
#define gBleCteMaxTxOctets_c    (251U)      /*! Maximum number of octets transmitted in the payload
                                             *  taking into account CTE, regardless of PHY */
#define gBleCteMaxTxTime_c      (2128U)     /*! Maximum number of microseconds taken to transmit a packet
                                             *  taking into account CTE, Coded PHY not supported */

/*! Maximum value of the advertising SID. */
#define gBleExtAdvMaxSetId_c            (0x0FU)

/*! Maximum value of the periodic advertising handle. */
#define gBlePeriodicAdvMaxSyncHandle_c  (0x0EFFU)

/*! SID of the legacy advertising set. */
#define gBleExtAdvLegacySetId_c         (0x00U)

/*! Handle of the legacy advertising set. */
#define gBleExtAdvLegacySetHandle_c     gBleExtAdvLegacySetId_c

/*! Default SID for extended advertising. */
#define gBleExtAdvDefaultSetId_c        (0x00U)

/*! Default handle for extended advertising. */
#define gBleExtAdvDefaultSetHandle_c    gBleExtAdvDefaultSetId_c

/*! Host has no preference for Tx Power */
#define gBleAdvTxPowerNoPreference_c    (127)

/*! No advertising duration. Advertising to continue until the Host disables it. */
#define gBleExtAdvNoDuration_c          (0x0000U)

/*! Default advertising duration in high duty directed advertising 1.28s = 1280ms/10ms(unit) = 128 */
#define gBleHighDutyDirectedAdvDuration (0x80U)

/*! No maximum number of advertising events. */
#define gBleExtAdvNoMaxEvents_c         (0x00U)

/*! Periodic advertising default handle. */
#define gBlePeriodicAdvDefaultHandle_c  (gBleExtAdvDefaultSetHandle_c)

/*! Minimum value for the sync_timeout parameter */
#define gBlePeriodicAdvSyncTimeoutMin_c (0x000A)

/*! Maximum value for the sync_timeout parameter */
#define gBlePeriodicAdvSyncTimeoutMax_c (0x4000)

/*! Maximum value for the skip parameter */
#define gBlePeriodicAdvSkipMax_c        (0x01F3)

/*! Maximum length of an AD structure */
#define gBleMaxADStructureLength_c      (254U)

/*! Maximum length of Extended Advertising Data */
#define gBleMaxExtAdvDataLength_c       (1650U)

/*! Maximum value in us of AUX Offset(13 bits) in AuxPtr in 300us units, i.e. ((1 << 13) - 1) * 300 */
#define gBleExtAdvMaxAuxOffsetUsec_c    (0x257ED4U)

/*! Length of P-256 key in octets */
#define gBleP256KeyLength_c             (64U)

/*! decision data key size in bytes */
#define gcDecisionDataKeySize_c            (16U)

/*! decision instructions parameter size in bytes */
#define gcDecisionInstructionsParamSize_c            (16U)

/*! decision instructions arbitrary data mask size in bytes */
#define gcDecisionInstructionsArbitraryDataMaskSize_c            (8U)

/*! decision instructions arbitrary data target size in bytes */
#define gcDecisionInstructionsArbitraryDataTargetSize_c            (8U)

/*! decision data random part size in bytes */
#define gcDecisionDataPrandSize_c          (3U)

/*! decision data maximum size in bytes */
#define gcDecisionDataMaxSize_c            (8U)

/*! decision data maximum size in bytes */
#define gcDecisionDataResolvableTagSize_c  (6U)

/*! The maximum number of tests in the Gap_SetDecisionInstructions
((max param length) - sizeof(numTests))/((sizeof(TestFlag) + sizeof(TestField) + sizeof(TestParameters)) = (255-1)/(1 + 1 + 16) */
#define gcMaxNumTestsInDecisionInstruction_c  (14U)

/*! Size of the Encrypted Advertising Data randomizer in bytes */
#define gcEadRandomizerSize_c    (5U)

/*! Size of the Encrypted Advertising Data MIC in bytes */
#define gcEadMicSize_c           (4U)

/*! Size of the Encrypted Advertising Data Initialization Vector in bytes */
#define gcEadIvSize_c            (8U)

/*! Size of the Encrypted Advertising Data Key in bytes */
#define gcEadKeySize_c           (16U)

/*! Value of the Additional Authentication Data field for the CCM algorithm used by EAD */
#define gcEadAadValue_c          (0xEA)

/*! Minimum RSSI threshold value for Monitored Advertisers */
#define gBleMonAdvRssiThresholdMin_c    (-127)

/*! Maximum RSSI threshold value for Monitored Advertisers */
#define gBleMonAdvRssiThresholdMax_c    (20)
#endif /* BLE_CONSTANTS_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
