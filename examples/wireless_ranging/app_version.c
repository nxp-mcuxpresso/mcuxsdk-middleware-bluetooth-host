/*
 * Copyright 2020, 2025-2026 NXP
 *
 * NXP Proprietary
 *
 * This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or
 * by downloading, installing, activating and/or otherwise using the software, you are
 * agreeing that you have read, and that you agree to comply with and are bound by,
 * such license terms. If you do not agree to be bound by the applicable license terms,
 * then you may not retain, install, activate or otherwise use the software.
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
