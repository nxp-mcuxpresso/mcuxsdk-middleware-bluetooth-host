PRIMARY_SERVICE(service_gatt, gBleSig_GenericAttributeProfile_d)
        CHARACTERISTIC(char_service_changed, gBleSig_GattServiceChanged_d, (gGattCharPropIndicate_c) )
            VALUE(value_service_changed, gBleSig_GattServiceChanged_d, (gPermissionNone_c), 4, 0x00, 0x00, 0x00, 0x00)
            CCCD(cccd_service_changed)

PRIMARY_SERVICE(service_gap, gBleSig_GenericAccessProfile_d)
    CHARACTERISTIC(char_device_name, gBleSig_GapDeviceName_d, (gGattCharPropRead_c) )
            VALUE(value_device_name, gBleSig_GapDeviceName_d, (gPermissionFlagReadable_c), 12, "NXP_BLE_TEMP")
    CHARACTERISTIC(char_security_levels, gBleSig_GattSecurityLevels_d, (gGattCharPropRead_c) )
            VALUE(value_security_levels, gBleSig_GattSecurityLevels_d, (gPermissionFlagReadable_c), 2, 0x02, 0x01)

PRIMARY_SERVICE(service_ranging, gBleSig_RangingService_d)
    CHARACTERISTIC(char_ras_ctrl_point, gBleSig_RasControlPoint_d, (gGattCharPropWriteWithoutRsp_c | gGattCharPropIndicate_c) )
        VALUE_VARLEN(value_ras_ctrl_point, gBleSig_RasControlPoint_d, (gPermissionFlagWritable_c | gPermissionFlagWriteWithEncryption_c), 0x05, 0x01, 0x00)
        CCCD(cccd_ras_ctrl_point)
    CHARACTERISTIC(char_ras_stored_data, gBleSig_RasOnDemandProcData_d, (gGattCharPropNotify_c | gGattCharPropIndicate_c) )
        VALUE_VARLEN(value_ras_stored_data, gBleSig_RasOnDemandProcData_d, (gPermissionNone_c), gAttMaxNotifIndDataSize_d(gAttMaxMtu_c), 0x01, 0x00)
        CCCD(cccd_ras_stored_data)
    CHARACTERISTIC(char_ras_real_time_data, gBleSig_RasRealTimeProcData_d, (gGattCharPropNotify_c | gGattCharPropIndicate_c) )
        VALUE_VARLEN(value_ras_real_time_data, gBleSig_RasRealTimeProcData_d, (gPermissionNone_c), gAttMaxNotifIndDataSize_d(gAttMaxMtu_c), 0x01, 0x00)
        CCCD(cccd_ras_real_time_data)
    CHARACTERISTIC(char_ras_ranging_data_ready, gBleSig_RasProcDataReady_d, (gGattCharPropRead_c | gGattCharPropNotify_c | gGattCharPropIndicate_c) )
        VALUE(value_ras_ranging_data_ready, gBleSig_RasProcDataReady_d, (gPermissionFlagReadable_c | gPermissionFlagReadWithEncryption_c), 0x02, 0x00, 0x00)
        CCCD(cccd_ras_data_ready)
    CHARACTERISTIC(char_ras_ranging_data_overwritten, gBleSig_RasprocDataOverwritten_d, (gGattCharPropRead_c | gGattCharPropNotify_c | gGattCharPropIndicate_c) )
        VALUE(value_ras_ranging_data_overwritten, gBleSig_RasprocDataOverwritten_d, (gPermissionFlagReadable_c | gPermissionFlagReadWithEncryption_c), 0x02, 0x00, 0x00)
        CCCD(cccd_ras_data_overwritten)
    CHARACTERISTIC(char_ras_feature, gBleSig_RasFeature_d, (gGattCharPropRead_c) )
        VALUE(value_ras_feature, gBleSig_RasFeature_d, (gPermissionFlagReadable_c | gPermissionFlagReadWithEncryption_c), 4, 0x0F, 0x00, 0x00, 0x00)

/* Placed at the end of file due to macro interpretation of line numbers. */
/*! *********************************************************************************
* \file gatt_db.h
*
* Copyright 2023-2026 NXP
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