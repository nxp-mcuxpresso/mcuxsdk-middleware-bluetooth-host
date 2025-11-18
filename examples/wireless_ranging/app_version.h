/*

Copyright 2020, 2025 NXP

SPDX-License-Identifier: BSD-3-Clause

*/

/* Prevent double inclusion */
#ifndef APP_VERSION_H
#define APP_VERSION_H

/* === Includes ============================================================= */

/* === Macros =============================================================== */
/* Save the build date and display it in the terminal */
#define gIncludeBuildDate_d         (1U)

/* === Types ================================================================ */
typedef struct app_version_tag
{
  uint32_t build  : 16;
  uint32_t patch  : 8;
  uint32_t minor  : 4;
  uint32_t major  : 4;
} app_version_t;


/* === Externals ============================================================ */
extern const uint8_t c_app_manual;
extern const app_version_t c_app_version;
extern const char c_app_git_hash [];
extern const char c_app_git_date [];
#if defined(gIncludeBuildDate_d) && (gIncludeBuildDate_d == 1U)
extern const char c_app_build_date [];
#endif

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_VERSION_H */
/* EOF */
