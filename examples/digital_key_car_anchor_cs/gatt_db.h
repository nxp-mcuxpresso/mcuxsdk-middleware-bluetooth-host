PRIMARY_SERVICE(service_gatt, gBleSig_GenericAttributeProfile_d)
        CHARACTERISTIC(char_service_changed, gBleSig_GattServiceChanged_d, (gGattCharPropIndicate_c) )
            VALUE(value_service_changed, gBleSig_GattServiceChanged_d, (gPermissionNone_c), 4, 0x00, 0x00, 0x00, 0x00)
            CCCD(cccd_service_changed)

PRIMARY_SERVICE(service_gap, gBleSig_GenericAccessProfile_d)
    CHARACTERISTIC(char_device_name, gBleSig_GapDeviceName_d, (gGattCharPropRead_c) )
            VALUE(value_device_name, gBleSig_GapDeviceName_d, (gPermissionFlagReadable_c), 11, "NXP_BLE_CAR")
    CHARACTERISTIC(char_security_levels, gBleSig_GattSecurityLevels_d, (gGattCharPropRead_c) )
            VALUE(value_security_levels, gBleSig_GattSecurityLevels_d, (gPermissionFlagReadable_c), 2, 0x01, 0x01)

PRIMARY_SERVICE(service_dk, gBleSig_CCC_DK_UUID_d)
    CHARACTERISTIC_UUID128(char_vehicle_psm, uuid_char_vehicle_psm, (gGattCharPropRead_c) )
            VALUE_UUID128(value_vehicle_psm, uuid_char_vehicle_psm, (gPermissionFlagReadable_c), 2, MSB2(gDK_DefaultVehiclePsm_c), LSB2(gDK_DefaultVehiclePsm_c))
CHARACTERISTIC_UUID128(char_vehicle_psm_vdbt_version, uuid_char_vehicle_psm_vdbt_version, (gGattCharPropRead_c) )
            VALUE_UUID128(value_vehicle_psm_vdbt_version, uuid_char_vehicle_psm_vdbt_version, (gPermissionFlagReadable_c | gPermissionFlagReadWithEncryption_c | gPermissionFlagReadWithAuthentication_c), 7, MSB2(gDK_DefaultVehiclePsm_c), LSB2(gDK_DefaultVehiclePsm_c), 1, 0x03, 0x00, 1, 0xFF)

/* Placed at the end of file due to macro interpretation of line numbers. */
/*! *********************************************************************************
* \file gatt_db.h
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