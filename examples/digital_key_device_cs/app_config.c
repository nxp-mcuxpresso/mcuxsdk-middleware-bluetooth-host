/*! *********************************************************************************
* \addtogroup App Config
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file app_config.c
*
* Copyright 2022 - 2026 NXP
*
* NXP Proprietary
*
* This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or
* by downloading, installing, activating and/or otherwise using the software, you are
* agreeing that you have read, and that you agree to comply with and are bound by,
* such license terms. If you do not agree to be bound by the applicable license terms,
* then you may not retain, install, activate or otherwise use the software.
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "gap_interface.h"
#include "ble_constants.h"
#include "gatt_db_handles.h"
#include "ble_conn_manager.h"
#include "digital_key_device_cs.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define smpEdiv                 0x1F99
#define mcEncryptionKeySize_c   16

#if (defined(gAppAdvNonCCC_d) && (gAppAdvNonCCC_d == 1U))
#define mDefaultTxPower         gBleAdvTxPowerNoPreference_c
#endif /* (defined(gAppAdvNonCCC_d) && (gAppAdvNonCCC_d == 1U)) */

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

gapScanningParameters_t gScanParams =
{
    /* type */              gScanTypePassive_c,
    /* interval */          gcScanIntervalCCC_c,
    /* window */            gcScanWindowCCC_c,
#if defined(gRandomStaticAddress_d) && (gRandomStaticAddress_d > 0)
    /* ownAddressType */    gBleAddrTypeRandom_c,
#else
    /* ownAddressType */    gBleAddrTypePublic_c,
#endif
    /* filterPolicy */      (uint8_t)gScanAll_c,
    /* scanning PHY */      (uint8_t)( gLePhy1MFlag_c | gLePhyCodedFlag_c )
};

/* Default Connection Request Parameters */
gapConnectionRequestParameters_t gConnReqParams =
{
    .scanInterval = gcScanIntervalCCC_c,
    .scanWindow = gcScanWindowCCC_c,
    .filterPolicy = (uint8_t)gUseDeviceAddress_c,
#if defined(gRandomStaticAddress_d) && (gRandomStaticAddress_d > 0)
    .ownAddressType = gBleAddrTypeRandom_c,
#else
    .ownAddressType = gBleAddrTypePublic_c,
#endif
    .connIntervalMin = gcConnectionIntervalCCC_c,
    .connIntervalMax = gcConnectionIntervalCCC_c,
    .connLatency = 0,
    .supervisionTimeout = 0x03E8,
    .connEventLengthMin = 0,
    .connEventLengthMax = 0xFFFF,
    .initiatingPHYs = (uint8_t)( gLePhy1MFlag_c | gLePhyCodedFlag_c )
};

#if (defined(gAppAdvNonCCC_d) && (gAppAdvNonCCC_d == 1U))
/* Advertising parameters used when the device acts as GAP Peripheral (non-CCC) */
gapExtAdvertisingParameters_t gLegacyAdvParams =
{
    /* SID */                       0xB,
    /* handle */                    gLegacyAdvSetHandle_c,
    /* minInterval */               gcAdvertisingIntervalCCC_1M_c /* 42 ms */,
    /* maxInterval */               gcAdvertisingIntervalCCC_1M_c /* 42 ms */,
    /* ownAddrType */               gBleAddrTypePublic_c,
    /* ownAddress */                {0, 0, 0, 0, 0, 0},
    /* peerAddrType */              gBleAddrTypePublic_c,
    /* peerAddress */               {0, 0, 0, 0, 0, 0},
    /* channelMap */                (gapAdvertisingChannelMapFlags_t) (gAdvChanMapFlag37_c | gAdvChanMapFlag38_c | gAdvChanMapFlag39_c),
    /* filterPolicy */              gProcessAll_c,
    /* extAdvProperties */          (bleAdvRequestProperties_t)(gAdvReqConnectable_c | gAdvReqScannable_c | gAdvReqLegacy_c),
    /* TxPower */                   mDefaultTxPower,
    /* primaryPHY */                (gapLePhyMode_t)gLePhy1M_c,
    /* secondaryPHY */              (gapLePhyMode_t)gLePhy1M_c,
    /* secondaryAdvMaxSkip */       0,
    /* enableScanReqNotification*/  FALSE,
    /* primaryAdvPhyOptions */      gLeCodingNoPreference_c,
    /* secondaryAdvPhyOptions */    gLeCodingNoPreference_c
};

/* Application Advertising Parameters */
appExtAdvertisingParams_t gAppAdvParams =
{
    &gLegacyAdvParams,
    &gAppAdvertisingData,
    &gAppScanRspData,
    gLegacyAdvSetHandle_c,
    gBleExtAdvNoDuration_c,
    gBleExtAdvNoMaxEvents_c
};

/* Scanning and Advertising Data */
/* Non-CCC uses a private UUID so regular CCC scanners do not match this device. */
static uint8_t adData0[2] = {UuidArray(gNonCccDkUUID_d)};

static gapAdStructure_t advScanStruct[1] = {
  {
    .length = 3U,
    .adType = gAdComplete16bitServiceList_c,
    .aData = (uint8_t*)adData0
  }
};

gapAdvertisingData_t gAppAdvertisingData =
{
    NumberOfElements(advScanStruct),
    (void *)advScanStruct
};

gapScanResponseData_t gAppScanRspData =
{
    0,
    NULL
};
#endif /* (defined(gAppAdvNonCCC_d) && (gAppAdvNonCCC_d == 1U)) */

/* SMP Data */
gapPairingParameters_t gPairingParameters = {
    .withBonding = (bool_t)gAppUseBonding_d,
    .securityModeAndLevel = gSecurityMode_1_Level_4_c,
    .maxEncryptionKeySize = mcEncryptionKeySize_c,
    .localIoCapabilities = gIoNone_c,
    .oobAvailable = TRUE,
    .centralKeys = (gapSmpKeyFlags_t) (gIrk_c),
    .peripheralKeys = (gapSmpKeyFlags_t) (gIrk_c),
    .leSecureConnectionSupported = TRUE,
    .useKeypressNotifications = FALSE,
};

/* LTK */
static uint8_t smpLtk[gcSmpMaxLtkSize_c] =
    {0xD6, 0x93, 0xE8, 0xA4, 0x23, 0x55, 0x48, 0x99,
     0x1D, 0x77, 0x61, 0xE6, 0x63, 0x2B, 0x10, 0x8E};

/* RAND*/
static uint8_t smpRand[gcSmpMaxRandSize_c] =
    {0x26, 0x1E, 0xF6, 0x09, 0x97, 0x2E, 0xAD, 0x7E};

/* IRK */
static uint8_t smpIrk[gcSmpIrkSize_c] =
    {0x0A, 0x2D, 0xF4, 0x65, 0xE3, 0xBD, 0x7B, 0x49,
     0x1E, 0xB4, 0xC0, 0x95, 0x95, 0x13, 0x46, 0x73};

gapSmpKeys_t gSmpKeys = {
    .cLtkSize = mcEncryptionKeySize_c,
    .aLtk = (void *)smpLtk,
    .aIrk = (void *)smpIrk,
    .aRand = (void *)smpRand,
    .cRandSize = gcSmpMaxRandSize_c,
    .ediv = smpEdiv,
};

/* Device Security Requirements */
static gapSecurityRequirements_t        deviceSecurity = gGapDefaultSecurityRequirements_d;

gapDeviceSecurityRequirements_t deviceSecurityRequirements = {
    .pSecurityRequirements          = (void*)&deviceSecurity,
    .cNumServices                   = 0,
    .aServiceSecurityRequirements   = NULL
};
