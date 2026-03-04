/*
 * Copyright 2020, 2025-2026 NXP
 *
 * NXP Confidential Proprietary
 *
 * No part of this document must be reproduced in any form - including copied,
 * transcribed, printed or by any electronic means - without specific written
 * permission from NXP.
 */
#include <stdint.h>
#include "app_version_definitions.h"
#include "app_version.h"

#ifndef APP_VERSION_BUILD
#error "APP_VERSION Macros not defined.."
#endif

const uint8_t c_app_manual = (APP_VERSION_MANUAL & 0x01);
const app_version_t c_app_version = {
    .build = (APP_VERSION_BUILD & 0xFFFF),
    .patch = (APP_VERSION_PATCH & 0xFF),
    .minor = (APP_VERSION_MINOR & 0xF),
    .major = (APP_VERSION_MAJOR & 0xF),
  };

#if defined(gIncludeBuildDate_d) && (gIncludeBuildDate_d == 1U)
const char c_app_build_date [] = {__DATE__" "__TIME__};
#endif
