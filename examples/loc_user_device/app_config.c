/*! *********************************************************************************
* \addtogroup App Config
* @{
********************************************************************************** */
/*! *********************************************************************************
* \file app_config.c
*
* Copyright 2023 - 2026 NXP
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
#include "loc_user_device.h"
#include "app_conn.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define mDefaultTxPower         gBleAdvTxPowerNoPreference_c

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

gapScanningParameters_t gScanParams =
{
    /* type */              gScanTypePassive_c,
    /* interval */          gGapScanIntervalDefault_d,
    /* window */            gGapScanWindowDefault_d,
#if defined(gRandomStaticAddress_d) && (gRandomStaticAddress_d > 0)
    /* ownAddressType */    gBleAddrTypeRandom_c,
#else
    /* ownAddressType */    gBleAddrTypePublic_c,
#endif
    /* filterPolicy */      (uint8_t)gScanAll_c,
    /* scanning PHY */      (uint8_t)gLePhy1MFlag_c
};

/* Default Connection Request Parameters */
gapConnectionRequestParameters_t gConnReqParams =
{
    .scanInterval = gcScanInterval_c,
    .scanWindow = gcScanWindow_c,
    .filterPolicy = (uint8_t)gUseDeviceAddress_c,
#if defined(gRandomStaticAddress_d) && (gRandomStaticAddress_d > 0)
    .ownAddressType = gBleAddrTypeRandom_c,
#else
    .ownAddressType = gBleAddrTypePublic_c,
#endif
    .connIntervalMin = gcConnectionInterval_c,
    .connIntervalMax = gcConnectionInterval_c,
    .connLatency = 0,
    .supervisionTimeout = 0x03E8,
    .connEventLengthMin = 0,
    .connEventLengthMax = 0xFFFF,
    .initiatingPHYs = (uint8_t)gLePhy1MFlag_c
};

/* SMP Data */
gapPairingParameters_t gPairingParameters = {
    .withBonding = (bool_t)gAppUseBonding_d,
    .securityModeAndLevel = gSecurityMode_1_Level_4_c,
    .maxEncryptionKeySize = mcEncryptionKeySize_c,
    .localIoCapabilities = gIoKeyboardDisplay_c,
    .oobAvailable = FALSE,
    .centralKeys = (gapSmpKeyFlags_t) (gIrk_c | gLtk_c),
    .peripheralKeys = (gapSmpKeyFlags_t) (gIrk_c | gLtk_c),
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

/* CSRK */
static uint8_t smpCsrk[gcSmpCsrkSize_c] =
    {0x90, 0xD5, 0x06, 0x95, 0x92, 0xED, 0x91, 0xD7,
     0xA8, 0x9E, 0x2C, 0xDC, 0x4A, 0x93, 0x5B, 0xF9};

gapSmpKeys_t gSmpKeys = {
    .cLtkSize = mcEncryptionKeySize_c,
    .aLtk = (void *)smpLtk,
    .aIrk = (void *)smpIrk,
    .aCsrk = (void *)smpCsrk,
    .aRand = (void *)smpRand,
    .cRandSize = gcSmpMaxRandSize_c,
    .ediv = smpEdiv,
};

/* Device Security Requirements */
static gapSecurityRequirements_t        deviceSecurity = gGapDefaultSecurityRequirements_d;
static gapServiceSecurityRequirements_t serviceSecurity[1] = {
  {
    .requirements = {
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
        .securityModeLevel = gSecurityMode_1_Level_1_c,
#else
        .securityModeLevel = gSecurityMode_1_Level_2_c,
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */
        .authorization = FALSE,
        .minimumEncryptionKeySize = gDefaultEncryptionKeySize_d
    },
    .serviceHandle = (uint16_t)service_ranging
  }
};

gapDeviceSecurityRequirements_t deviceSecurityRequirements = {
    .pSecurityRequirements          = (void*)&deviceSecurity,
    .cNumServices                   = 3,
    .aServiceSecurityRequirements   = serviceSecurity
};

/* Default Advertising Parameters */
gapExtAdvertisingParameters_t gAdvParams =
{
    /* SID */                       0xB, \
    /* handle */                    gAdvSetHandle_c, \
    /* minInterval */               gcAdvertisingInterval_c, \
    /* maxInterval */               gcAdvertisingInterval_c,
#if defined(gRandomStaticAddress_d) && (gRandomStaticAddress_d > 0)
    /* addressType */               gBleAddrTypeRandom_c,
#else
    /* addressType */               gBleAddrTypePublic_c,
#endif
    /* ownAddress */                {0, 0, 0, 0, 0, 0}, \
    /* peerAddrType */              gBleAddrTypePublic_c,\
    /* peerAddress */               {0, 0, 0, 0, 0, 0}, \
    /* channelMap */                (gapAdvertisingChannelMapFlags_t) (gAdvChanMapFlag37_c), \
    /* filterPolicy */              gProcessAll_c, \
    /* extAdvProperties */          (bleAdvRequestProperties_t)(gAdvReqConnectable_c | gAdvReqScannable_c | gAdvReqLegacy_c),\
    /* TxPower */                   mDefaultTxPower, \
    /* primaryPHY */                (gapLePhyMode_t)gLePhy1M_c, \
    /* secondaryPHY */              (gapLePhyMode_t)gLePhy1M_c, \
    /* secondaryAdvMaxSkip */       0, \
    /* enableScanReqNotification*/  FALSE, \
    /* primaryAdvPhyOptions */      gLeCodingNoPreference_c, \
    /* secondaryAdvPhyOptions */    gLeCodingNoPreference_c \
};

static uint8_t adData0[2] = { UuidArray(gBleSig_RangingService_d) };

/* Advertising Data */
static gapAdStructure_t advScanStruct[] = {
  {
    .length = NumberOfElements(adData0) + 1,
    .adType = gAdIncomplete16bitServiceList_c,
    .aData = (uint8_t *)adData0
  },
  {
    .length = 7U,
    .adType = gAdShortenedLocalName_c,
    .aData = (uint8_t*)"NXP_CS"
  },
#if (defined(gAppUseTAK_d) && gAppUseTAK_d)
  {
    .length = sizeof(gAppTAKAdvID_c),
    .adType = gAdManufacturerSpecificData_c,
    .aData = (uint8_t*)gAppTAKAdvID_c
  }
#endif /* (defined(gAppUseTAK_d) && gAppUseTAK_d) */
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

/*Default Application Advertising Parameters */
appExtAdvertisingParams_t gAppAdvParams =
{
    &gAdvParams,
    &gAppAdvertisingData,
    &gAppScanRspData,
    gAdvSetHandle_c,
    gBleExtAdvNoDuration_c,
    gBleExtAdvNoMaxEvents_c
};

