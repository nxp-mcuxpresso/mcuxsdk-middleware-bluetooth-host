/*! *********************************************************************************
* Copyright 2014 Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2023 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef GATT_INIT_X_H
#define GATT_INIT_X_H

#define PRIMARY_SERVICE(...)
#define PRIMARY_SERVICE_UUID32(...)
#define PRIMARY_SERVICE_UUID128                     XINIT_PRIMARY_SERVICE_UUID128
#define SECONDARY_SERVICE(...)
#define SECONDARY_SERVICE_UUID32(...)
#define SECONDARY_SERVICE_UUID128                   XINIT_SECONDARY_SERVICE_UUID128
#define INCLUDE                                     XINIT_INCLUDE
#define INCLUDE_CUSTOM                              XINIT_INCLUDE_CUSTOM
#define CHARACTERISTIC(...)
#define CHARACTERISTIC_UUID32(...)
#define CHARACTERISTIC_UUID128                      XINIT_CHARACTERISTIC_UUID128
#define VALUE(...)
#define VALUE_UUID32(...)
#define VALUE_UUID128(...)
#define VALUE_VARLEN(...)
#define VALUE_UUID32_VARLEN(...)
#define VALUE_UUID128_VARLEN(...)
#define CCCD(...)
#define DESCRIPTOR(...)
#define DESCRIPTOR_UUID32(...)
#define DESCRIPTOR_UUID128(...)
#define CHARACTERISTIC_AGGREGATE(...)

#include "gatt_db.h"

#undef PRIMARY_SERVICE
#undef PRIMARY_SERVICE_UUID32
#undef PRIMARY_SERVICE_UUID128
#undef SECONDARY_SERVICE
#undef SECONDARY_SERVICE_UUID32
#undef SECONDARY_SERVICE_UUID128
#undef INCLUDE
#undef INCLUDE_CUSTOM
#undef CHARACTERISTIC
#undef CHARACTERISTIC_UUID32
#undef CHARACTERISTIC_UUID128
#undef VALUE
#undef VALUE_UUID32
#undef VALUE_UUID128
#undef VALUE_VARLEN
#undef VALUE_UUID32_VARLEN
#undef VALUE_UUID128_VARLEN
#undef CCCD
#undef DESCRIPTOR
#undef DESCRIPTOR_UUID32
#undef DESCRIPTOR_UUID128
#undef CHARACTERISTIC_AGGREGATE

#endif /* GATT_INIT_X_H */
