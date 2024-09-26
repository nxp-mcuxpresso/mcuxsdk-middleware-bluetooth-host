include_guard(GLOBAL)


if (CONFIG_USE_device_K32W1480_startup_wireless)
# Add set(CONFIG_USE_device_K32W1480_startup_wireless true) in config.cmake to use this component

message("device_K32W1480_startup_wireless component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_TOOLCHAIN STREQUAL armgcc OR CONFIG_TOOLCHAIN STREQUAL mcux))
  target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/../../../devices/K32W1480/gcc/startup_K32W1480.S
  )
endif()


endif()


if (CONFIG_USE_wireless_wireless_clock_pin_mux_file)
# Add set(CONFIG_USE_wireless_wireless_clock_pin_mux_file true) in config.cmake to use this component

message("wireless_wireless_clock_pin_mux_file component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../framework/boards/kw45_k32w1/KW45B41Z83/clock_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../framework/boards/kw45_k32w1/KW45B41Z83/pin_mux.c
)


endif()


if (CONFIG_USE_middleware_wireless_ble_xml_fsci)
# Add set(CONFIG_USE_middleware_wireless_ble_xml_fsci true) in config.cmake to use this component

message("middleware_wireless_ble_xml_fsci component is included from ${CMAKE_CURRENT_LIST_FILE}.")


endif()


if (CONFIG_USE_middleware_wireless_ble_xml_hci)
# Add set(CONFIG_USE_middleware_wireless_ble_xml_hci true) in config.cmake to use this component

message("middleware_wireless_ble_xml_hci component is included from ${CMAKE_CURRENT_LIST_FILE}.")


endif()


if (CONFIG_USE_middleware_wireless_ble_host_sdk)
# Add set(CONFIG_USE_middleware_wireless_ble_host_sdk true) in config.cmake to use this component

message("middleware_wireless_ble_host_sdk component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/demo/FsciBootloader.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/demo/GetKinetisDevices.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/physical/PhysicalDevice.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/protocol/Framer.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/sys/EventManager.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/sys/MessageQueue.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/sys/RawFrame.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/sys/hsdkEvent.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/sys/hsdkFile.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/sys/hsdkLock.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/sys/hsdkLogger.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/sys/hsdkSemaphore.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/sys/hsdkThread.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/sys/utils.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/physical/PCAP/PCAPDevice.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/physical/SPI/SPIConfiguration.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/physical/SPI/SPIDevice.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/physical/UART/UARTConfiguration.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/physical/UART/UARTDevice.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/physical/UART/UARTDiscovery.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/protocol/FSCI/FSCIFrame.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk/protocol/FSCI/FSCIFramer.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk-c/demo/HeartRateSensor.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk-c/src/cmd_ble.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk-c/src/evt_ble.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk-c/src/evt_printer_ble.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../tools/wireless/host_sdk/hsdk-c/src/unload_ble.c
)


endif()


if (CONFIG_USE_middleware_wireless_ble_host)
# Add set(CONFIG_USE_middleware_wireless_ble_host true) in config.cmake to use this component

message("middleware_wireless_ble_host component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/./port/fwk_generic_list.c
  ${CMAKE_CURRENT_LIST_DIR}/./port/fwk_messaging.c
  ${CMAKE_CURRENT_LIST_DIR}/./port/fwk_os_abs.c
  ${CMAKE_CURRENT_LIST_DIR}/./port/fwk_timer_manager.c
  ${CMAKE_CURRENT_LIST_DIR}/./application/common/ble_host_tasks.c
  ${CMAKE_CURRENT_LIST_DIR}/./application/common/ble_conn_manager.c
  ${CMAKE_CURRENT_LIST_DIR}/./host/config/ble_globals.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/./host/interface
  ${CMAKE_CURRENT_LIST_DIR}/./application/common
  ${CMAKE_CURRENT_LIST_DIR}/./host/config
  ${CMAKE_CURRENT_LIST_DIR}/./port
)


endif()


if (CONFIG_USE_middleware_wireless_ble_host_AE_component_lib_mcux)
# Add set(CONFIG_USE_middleware_wireless_ble_host_AE_component_lib_mcux true) in config.cmake to use this component

message("middleware_wireless_ble_host_AE_component_lib_mcux component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL mcux)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_OPT_host_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_AE_component_lib_armgcc)
# Add set(CONFIG_USE_middleware_wireless_ble_host_AE_component_lib_armgcc true) in config.cmake to use this component

message("middleware_wireless_ble_host_AE_component_lib_armgcc component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL armgcc)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_OPT_host_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_AE_central_component_lib_mcux)
# Add set(CONFIG_USE_middleware_wireless_ble_host_AE_central_component_lib_mcux true) in config.cmake to use this component

message("middleware_wireless_ble_host_AE_central_component_lib_mcux component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL mcux)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_OPT_host_central_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_AE_central_component_lib_armgcc)
# Add set(CONFIG_USE_middleware_wireless_ble_host_AE_central_component_lib_armgcc true) in config.cmake to use this component

message("middleware_wireless_ble_host_AE_central_component_lib_armgcc component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL armgcc)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_OPT_host_central_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_AE_peripheral_component_lib_mcux)
# Add set(CONFIG_USE_middleware_wireless_ble_host_AE_peripheral_component_lib_mcux true) in config.cmake to use this component

message("middleware_wireless_ble_host_AE_peripheral_component_lib_mcux component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL mcux)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_OPT_host_peripheral_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_AE_peripheral_component_lib_armgcc)
# Add set(CONFIG_USE_middleware_wireless_ble_host_AE_peripheral_component_lib_armgcc true) in config.cmake to use this component

message("middleware_wireless_ble_host_AE_peripheral_component_lib_armgcc component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL armgcc)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_OPT_host_peripheral_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_component_lib_mcux)
# Add set(CONFIG_USE_middleware_wireless_ble_host_component_lib_mcux true) in config.cmake to use this component

message("middleware_wireless_ble_host_component_lib_mcux component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL mcux)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_host_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_component_lib_armgcc)
# Add set(CONFIG_USE_middleware_wireless_ble_host_component_lib_armgcc true) in config.cmake to use this component

message("middleware_wireless_ble_host_component_lib_armgcc component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL armgcc)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_host_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_central_component_lib_mcux)
# Add set(CONFIG_USE_middleware_wireless_ble_host_central_component_lib_mcux true) in config.cmake to use this component

message("middleware_wireless_ble_host_central_component_lib_mcux component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL mcux)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_host_central_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_central_component_lib_armgcc)
# Add set(CONFIG_USE_middleware_wireless_ble_host_central_component_lib_armgcc true) in config.cmake to use this component

message("middleware_wireless_ble_host_central_component_lib_armgcc component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL armgcc)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_host_central_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_peripheral_component_lib_mcux)
# Add set(CONFIG_USE_middleware_wireless_ble_host_peripheral_component_lib_mcux true) in config.cmake to use this component

message("middleware_wireless_ble_host_peripheral_component_lib_mcux component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL mcux)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_host_peripheral_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_peripheral_component_lib_armgcc)
# Add set(CONFIG_USE_middleware_wireless_ble_host_peripheral_component_lib_armgcc true) in config.cmake to use this component

message("middleware_wireless_ble_host_peripheral_component_lib_armgcc component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL armgcc)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_host_peripheral_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_interface)
# Add set(CONFIG_USE_middleware_wireless_ble_host_interface true) in config.cmake to use this component

message("middleware_wireless_ble_host_interface component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/interface
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/config
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/port
)


endif()


if (CONFIG_USE_middleware_wireless_ble_host_interface_black_box)
# Add set(CONFIG_USE_middleware_wireless_ble_host_interface_black_box true) in config.cmake to use this component

message("middleware_wireless_ble_host_interface_black_box component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/./application/common/ble_host_tasks.c
  ${CMAKE_CURRENT_LIST_DIR}/./host/config/ble_globals.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/./host/interface
  ${CMAKE_CURRENT_LIST_DIR}/./application/common
  ${CMAKE_CURRENT_LIST_DIR}/./host/config
  ${CMAKE_CURRENT_LIST_DIR}/./bluetooth/port
)


endif()


if (CONFIG_USE_middleware_wireless_ble_host_interface_host)
# Add set(CONFIG_USE_middleware_wireless_ble_host_interface_host true) in config.cmake to use this component

message("middleware_wireless_ble_host_interface_host component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/./application/common/ble_conn_manager.c
  ${CMAKE_CURRENT_LIST_DIR}/./host/config/ble_globals.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/./host/interface
  ${CMAKE_CURRENT_LIST_DIR}/./application/common
  ${CMAKE_CURRENT_LIST_DIR}/./host/config
)


endif()


if (CONFIG_USE_middleware_wireless_ble_host_interface_genfsk)
# Add set(CONFIG_USE_middleware_wireless_ble_host_interface_genfsk true) in config.cmake to use this component

message("middleware_wireless_ble_host_interface_genfsk component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/interface
)


endif()


if (CONFIG_USE_middleware_wireless_ble_fsci_interface)
# Add set(CONFIG_USE_middleware_wireless_ble_fsci_interface true) in config.cmake to use this component

message("middleware_wireless_ble_fsci_interface component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/interface
)


endif()


if (CONFIG_USE_middleware_wireless_ble_fsci_private_interface)
# Add set(CONFIG_USE_middleware_wireless_ble_fsci_private_interface true) in config.cmake to use this component

message("middleware_wireless_ble_fsci_private_interface component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/interface
)


endif()


if (CONFIG_USE_middleware_wireless_ble_fsci_source)
# Add set(CONFIG_USE_middleware_wireless_ble_fsci_source true) in config.cmake to use this component

message("middleware_wireless_ble_fsci_source component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_gap.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_gap_types.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_gap_handlers.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_gap2_handlers.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_gatt.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_gatt_db_app.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_gatt_db_app_types.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_gatt_types.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_l2cap_cb.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_l2cap_cb_types.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_types.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source
)


endif()


if (CONFIG_USE_middleware_wireless_ble_fsci_source_handover)
# Add set(CONFIG_USE_middleware_wireless_ble_fsci_source_handover true) in config.cmake to use this component

message("middleware_wireless_ble_fsci_source_handover component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_gap_handover.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_ble_gap_handover_types.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source
)


endif()


if (CONFIG_USE_middleware_wireless_ble_fsci_source_channel_sounding)
# Add set(CONFIG_USE_middleware_wireless_ble_fsci_source_channel_sounding true) in config.cmake to use this component

message("middleware_wireless_ble_fsci_source_channel_sounding component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source/fsci_channel_sounding.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/source
)


endif()


if (CONFIG_USE_middleware_wireless_ble_fsci_host)
# Add set(CONFIG_USE_middleware_wireless_ble_fsci_host true) in config.cmake to use this component

message("middleware_wireless_ble_fsci_host component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/host/host_ble.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/host/host_gap.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/host/host_gatt.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/host/host_gatt_db.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/host/host_l2cap_cb.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/fsci/host
)


endif()


if (CONFIG_USE_middleware_wireless_ble_host_interface_handover)
# Add set(CONFIG_USE_middleware_wireless_ble_host_interface_handover true) in config.cmake to use this component

message("middleware_wireless_ble_host_interface_handover component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/./bluetooth/host/interface
)


endif()


if (CONFIG_USE_middleware_wireless_ble_host_handover_component_lib_mcux)
# Add set(CONFIG_USE_middleware_wireless_ble_host_handover_component_lib_mcux true) in config.cmake to use this component

message("middleware_wireless_ble_host_handover_component_lib_mcux component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL mcux)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_handover_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_ble_host_handover_component_lib_armgcc)
# Add set(CONFIG_USE_middleware_wireless_ble_host_handover_component_lib_armgcc true) in config.cmake to use this component

message("middleware_wireless_ble_host_handover_component_lib_armgcc component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL armgcc)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_handover_cm33_gcc.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_auto)
# Add set(CONFIG_USE_middleware_wireless_auto true) in config.cmake to use this component

message("middleware_wireless_auto component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/auto/app_a2a_interface.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/auto/app_a2b.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/auto/app_handover.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/auto
)


endif()


if (CONFIG_USE_middleware_wireless_ble_hci_transport_serial)
# Add set(CONFIG_USE_middleware_wireless_ble_hci_transport_serial true) in config.cmake to use this component

message("middleware_wireless_ble_hci_transport_serial component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/hci_transport/source/hcit_serial_interface.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/hci_transport/interface
)


endif()


if (CONFIG_USE_middleware_wireless_ble_hci_transport_rpmsg)
# Add set(CONFIG_USE_middleware_wireless_ble_hci_transport_rpmsg true) in config.cmake to use this component

message("middleware_wireless_ble_hci_transport_rpmsg component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/hci_transport/source/hcit_generic_adapter_interface.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/hci_transport/interface
)


endif()


if (CONFIG_USE_middleware_wireless_ble_hci_transport_adapter)
# Add set(CONFIG_USE_middleware_wireless_ble_hci_transport_adapter true) in config.cmake to use this component

message("middleware_wireless_ble_hci_transport_adapter component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/hci_transport/source/hcit_adapter_interface.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/hci_transport/interface
)


endif()


if (CONFIG_USE_middleware_wireless_ble_hci_transport_interface)
# Add set(CONFIG_USE_middleware_wireless_ble_hci_transport_interface true) in config.cmake to use this component

message("middleware_wireless_ble_hci_transport_interface component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/hci_transport/interface
)


endif()


if (CONFIG_USE_middleware_wireless_ble_gatt_db_dynamic)
# Add set(CONFIG_USE_middleware_wireless_ble_gatt_db_dynamic true) in config.cmake to use this component

message("middleware_wireless_ble_gatt_db_dynamic component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/gatt_db/gatt_database.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/gatt_db/gatt_database_dynamic.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/gatt_db
)


endif()


if (CONFIG_USE_middleware_wireless_ble_gatt_service_discovery)
# Add set(CONFIG_USE_middleware_wireless_ble_gatt_service_discovery true) in config.cmake to use this component

message("middleware_wireless_ble_gatt_service_discovery component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/ble_service_discovery.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common
)


endif()


if (CONFIG_USE_middleware_wireless_ble_gatt_db)
# Add set(CONFIG_USE_middleware_wireless_ble_gatt_db true) in config.cmake to use this component

message("middleware_wireless_ble_gatt_db component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/gatt_db/gatt_database.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/gatt_db/macros
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/gatt_db
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common
)


endif()


if (CONFIG_USE_middleware_wireless_ble_profiles_battery_service)
# Add set(CONFIG_USE_middleware_wireless_ble_profiles_battery_service true) in config.cmake to use this component

message("middleware_wireless_ble_profiles_battery_service component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/battery/battery_service.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/battery
)


endif()


if (CONFIG_USE_middleware_wireless_ble_profiles_wireless_uart)
# Add set(CONFIG_USE_middleware_wireless_ble_profiles_wireless_uart true) in config.cmake to use this component

message("middleware_wireless_ble_profiles_wireless_uart component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/wireless_uart/wireless_uart_service.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/wireless_uart
)


endif()


if (CONFIG_USE_middleware_wireless_ble_profiles_ranging)
# Add set(CONFIG_USE_middleware_wireless_ble_profiles_ranging true) in config.cmake to use this component

message("middleware_wireless_ble_profiles_ranging component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/ranging/ranging_service.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/ranging
)


endif()


if (CONFIG_USE_middleware_wireless_ble_controller_dtm)
# Add set(CONFIG_USE_middleware_wireless_ble_controller_dtm true) in config.cmake to use this component

message("middleware_wireless_ble_controller_dtm component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/src/dtm/dtm.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/interface
)


endif()


if (CONFIG_USE_middleware_wireless_controller_api)
# Add set(CONFIG_USE_middleware_wireless_controller_api true) in config.cmake to use this component

message("middleware_wireless_controller_api component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/src/controller_api.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/interface
)


endif()


if (CONFIG_USE_middleware_wireless_ble_controller_interface)
# Add set(CONFIG_USE_middleware_wireless_ble_controller_interface true) in config.cmake to use this component

message("middleware_wireless_ble_controller_interface component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/interface
)


endif()


if (CONFIG_USE_middleware_wireless_ble_controller_thread_interface)
# Add set(CONFIG_USE_middleware_wireless_ble_controller_thread_interface true) in config.cmake to use this component

message("middleware_wireless_ble_controller_thread_interface component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/interface/connected_mcu
)


endif()


if (CONFIG_USE_middleware_wireless_XCVR_KW38_driver)
# Add set(CONFIG_USE_middleware_wireless_XCVR_KW38_driver true) in config.cmake to use this component

message("middleware_wireless_XCVR_KW38_driver component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../framework/XCVR/MKW38Z4/drv
  ${CMAKE_CURRENT_LIST_DIR}/../framework/XCVR/MKW38Z4/drv/nb2p4ghz
  ${CMAKE_CURRENT_LIST_DIR}/../framework/XCVR/MKW38Z4/drv/nb2p4ghz/configs/gen35
)


endif()


if (CONFIG_USE_middleware_wireless_XCVR_GEN40_driver)
# Add set(CONFIG_USE_middleware_wireless_XCVR_GEN40_driver true) in config.cmake to use this component

message("middleware_wireless_XCVR_GEN40_driver component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../framework/XCVR/MKW38Z4/drv
  ${CMAKE_CURRENT_LIST_DIR}/../framework/XCVR/MKW38Z4/drv/nb2p4ghz
  ${CMAKE_CURRENT_LIST_DIR}/../framework/XCVR/MKW38Z4/drv/nb2p4ghz/configs/gen40
)


endif()


if (CONFIG_USE_middleware_wireless_XCVR_GEN45_driver)
# Add set(CONFIG_USE_middleware_wireless_XCVR_GEN45_driver true) in config.cmake to use this component

message("middleware_wireless_XCVR_GEN45_driver component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/mathfp.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/dma_capture.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/rfmc_ctrl.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/dbg_ram_capture.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nxp_xcvr_lcl_ctrl.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nxp_xcvr_ext_ctrl.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/nxp2p4_xcvr.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/nxp_xcvr_trim.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_coding_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_common_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_lcl_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_gfsk_bt_0p5_h_0p5_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_gfsk_bt_0p5_h_0p7_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_gfsk_bt_0p5_h_0p32_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_gfsk_bt_0p5_h_1p0_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_oqpsk_802p15p4_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_mode_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_msk_config.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45
)

if(CONFIG_USE_COMPONENT_CONFIGURATION)
  message("===>Import configuration from ${CMAKE_CURRENT_LIST_FILE}")

  target_compile_definitions(${MCUX_SDK_PROJECT_NAME} PUBLIC
    -DRF_OSC_26MHZ=0
    -DTEST_BUILD_COEX=0
    -DARM_MATH_CM3=1
  )

endif()


endif()


if (CONFIG_USE_middleware_wireless_XCVR_driver)
# Add set(CONFIG_USE_middleware_wireless_XCVR_driver true) in config.cmake to use this component

message("middleware_wireless_XCVR_driver component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/mathfp.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/rfmc_ctrl.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/dbg_ram_capture.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/nxp2p4_xcvr.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/nxp_xcvr_trim.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen40/nxp_xcvr_coding_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen40/nxp_xcvr_common_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen40/nxp_xcvr_gfsk_bt_0p5_h_0p5_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen40/nxp_xcvr_gfsk_bt_0p5_h_0p7_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen40/nxp_xcvr_gfsk_bt_0p5_h_0p32_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen40/nxp_xcvr_gfsk_bt_0p5_h_1p0_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen40/nxp_xcvr_oqpsk_802p15p4_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen40/nxp_xcvr_mode_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen40/nxp_xcvr_msk_config.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen40
)


endif()


if (CONFIG_USE_middleware_wireless_XCVR_GEN45_driver_workaround)
# Add set(CONFIG_USE_middleware_wireless_XCVR_GEN45_driver_workaround true) in config.cmake to use this component

message("middleware_wireless_XCVR_GEN45_driver_workaround component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/mathfp.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/dma_capture.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/rfmc_ctrl.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/dbg_ram_capture.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nxp_xcvr_ext_ctrl.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/nxp2p4_xcvr.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/nxp_xcvr_trim.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_coding_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_lcl_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_gfsk_bt_0p5_h_0p5_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_gfsk_bt_0p5_h_0p7_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_gfsk_bt_0p5_h_0p32_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_gfsk_bt_0p5_h_1p0_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_oqpsk_802p15p4_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_mode_config.c
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45/nxp_xcvr_msk_config.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz
  ${CMAKE_CURRENT_LIST_DIR}/../XCVR/drv/nb2p4ghz/configs/gen45
)

if(CONFIG_USE_COMPONENT_CONFIGURATION)
  message("===>Import configuration from ${CMAKE_CURRENT_LIST_FILE}")

  target_compile_definitions(${MCUX_SDK_PROJECT_NAME} PUBLIC
    -DRF_OSC_26MHZ=0
    -DTEST_BUILD_COEX=0
    -DARM_MATH_CM3=1
  )

endif()


endif()


if (CONFIG_USE_device_MCXW716C_startup_wireless)
# Add set(CONFIG_USE_device_MCXW716C_startup_wireless true) in config.cmake to use this component

message("device_MCXW716C_startup_wireless component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_TOOLCHAIN STREQUAL armgcc OR CONFIG_TOOLCHAIN STREQUAL mcux))
  target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/../../../devices/MCXW716C/gcc/startup_MCXW716C.S
  )
endif()


endif()


if (CONFIG_USE_device_KW45B41Z83_startup_wireless)
# Add set(CONFIG_USE_device_KW45B41Z83_startup_wireless true) in config.cmake to use this component

message("device_KW45B41Z83_startup_wireless component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_TOOLCHAIN STREQUAL armgcc OR CONFIG_TOOLCHAIN STREQUAL mcux))
  target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/../../../devices/KW45B41Z83/gcc/startup_KW45B41Z83.S
  )
endif()


endif()


if (CONFIG_USE_wireless_wireless_seclib_file)
# Add set(CONFIG_USE_wireless_wireless_seclib_file true) in config.cmake to use this component

message("wireless_wireless_seclib_file component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_sec_lib)

else()

message(SEND_ERROR "wireless_wireless_seclib_file dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_host_matter_component_lib_iar)
# Add set(CONFIG_USE_middleware_wireless_ble_host_matter_component_lib_iar true) in config.cmake to use this component

message("middleware_wireless_ble_host_matter_component_lib_iar component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_BOARD STREQUAL k32w148evk OR CONFIG_BOARD STREQUAL frdmmcxw71))

else()

message(SEND_ERROR "middleware_wireless_ble_host_matter_component_lib_iar dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_host_matter_component_lib_mcux)
# Add set(CONFIG_USE_middleware_wireless_ble_host_matter_component_lib_mcux true) in config.cmake to use this component

message("middleware_wireless_ble_host_matter_component_lib_mcux component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_BOARD STREQUAL k32w148evk OR CONFIG_BOARD STREQUAL frdmmcxw71))

if(CONFIG_TOOLCHAIN STREQUAL mcux)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_host_matter_cm33_gcc.a
      -Wl,--end-group
  )
endif()

else()

message(SEND_ERROR "middleware_wireless_ble_host_matter_component_lib_mcux dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_host_matter_component_lib_armgcc)
# Add set(CONFIG_USE_middleware_wireless_ble_host_matter_component_lib_armgcc true) in config.cmake to use this component

message("middleware_wireless_ble_host_matter_component_lib_armgcc component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_BOARD STREQUAL k32w148evk OR CONFIG_BOARD STREQUAL frdmmcxw71))

if(CONFIG_TOOLCHAIN STREQUAL armgcc)
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/host/lib/lib_ble_host_matter_cm33_gcc.a
      -Wl,--end-group
  )
endif()

else()

message(SEND_ERROR "middleware_wireless_ble_host_matter_component_lib_armgcc dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_init_matter)
# Add set(CONFIG_USE_middleware_wireless_ble_init_matter true) in config.cmake to use this component

message("middleware_wireless_ble_init_matter component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_BOARD STREQUAL frdmmcxw71 OR CONFIG_BOARD STREQUAL k32w148evk))

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/matter/ble_init.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/application/common/matter
)

else()

message(SEND_ERROR "middleware_wireless_ble_init_matter dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_profiles_device_info_service)
# Add set(CONFIG_USE_middleware_wireless_ble_profiles_device_info_service true) in config.cmake to use this component

message("middleware_wireless_ble_profiles_device_info_service component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_BOARD STREQUAL kw45b41zevk OR CONFIG_BOARD STREQUAL k32w148evk OR CONFIG_BOARD STREQUAL frdmmcxw71))

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/device_info/device_info_service.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/device_info
)

else()

message(SEND_ERROR "middleware_wireless_ble_profiles_device_info_service dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_profiles_ancs)
# Add set(CONFIG_USE_middleware_wireless_ble_profiles_ancs true) in config.cmake to use this component

message("middleware_wireless_ble_profiles_ancs component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_BOARD STREQUAL kw45b41zevk OR CONFIG_BOARD STREQUAL k32w148evk OR CONFIG_BOARD STREQUAL frdmmcxw71))

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/ancs
)

else()

message(SEND_ERROR "middleware_wireless_ble_profiles_ancs dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_profiles_time)
# Add set(CONFIG_USE_middleware_wireless_ble_profiles_time true) in config.cmake to use this component

message("middleware_wireless_ble_profiles_time component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_BOARD STREQUAL kw45b41zevk OR CONFIG_BOARD STREQUAL k32w148evk OR CONFIG_BOARD STREQUAL frdmmcxw71))

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/time/current_time_service.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/time/next_dst_change_service.c
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/time/reference_time_update_service.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/time
)

else()

message(SEND_ERROR "middleware_wireless_ble_profiles_time dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_profiles_hid)
# Add set(CONFIG_USE_middleware_wireless_ble_profiles_hid true) in config.cmake to use this component

message("middleware_wireless_ble_profiles_hid component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_BOARD STREQUAL kw45b41zevk OR CONFIG_BOARD STREQUAL k32w148evk OR CONFIG_BOARD STREQUAL frdmmcxw71))

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/hid/hid_service.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/hid
)

else()

message(SEND_ERROR "middleware_wireless_ble_profiles_hid dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_profiles_temperature)
# Add set(CONFIG_USE_middleware_wireless_ble_profiles_temperature true) in config.cmake to use this component

message("middleware_wireless_ble_profiles_temperature component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_BOARD STREQUAL kw45b41zevk OR CONFIG_BOARD STREQUAL k32w148evk OR CONFIG_BOARD STREQUAL frdmmcxw71))

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/temperature/temperature_service.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/temperature
)

else()

message(SEND_ERROR "middleware_wireless_ble_profiles_temperature dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_profiles_otap)
# Add set(CONFIG_USE_middleware_wireless_ble_profiles_otap true) in config.cmake to use this component

message("middleware_wireless_ble_profiles_otap component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_BOARD STREQUAL kw45b41zevk OR CONFIG_BOARD STREQUAL k32w148evk OR CONFIG_BOARD STREQUAL frdmmcxw71))

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/otap/otap_service.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/otap
)

else()

message(SEND_ERROR "middleware_wireless_ble_profiles_otap dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_profiles_digital_key)
# Add set(CONFIG_USE_middleware_wireless_ble_profiles_digital_key true) in config.cmake to use this component

message("middleware_wireless_ble_profiles_digital_key component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((CONFIG_BOARD STREQUAL kw45b41zevk OR CONFIG_BOARD STREQUAL kw45b41zloc))

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/digital_key/digital_key_service.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../bluetooth/profiles/digital_key
)

else()

message(SEND_ERROR "middleware_wireless_ble_profiles_digital_key dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_controller_config)
# Add set(CONFIG_USE_middleware_wireless_ble_controller_config true) in config.cmake to use this component

message("middleware_wireless_ble_controller_config component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((NOT (CONFIG_NOT STREQUAL kw45b41zevk OR CONFIG_NOT STREQUAL k32w148evk OR CONFIG_NOT STREQUAL kw45b41zloc OR CONFIG_NOT STREQUAL frdmmcxw71)))

if((CONFIG_TOOLCHAIN STREQUAL armgcc OR CONFIG_TOOLCHAIN STREQUAL mcux))
  target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/config/ble_ll_globals.c
  )
endif()

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/config
)

if((CONFIG_TOOLCHAIN STREQUAL armgcc OR CONFIG_TOOLCHAIN STREQUAL mcux))
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/config/ble_ll_globals.c
      -Wl,--end-group
  )
endif()

else()

message(SEND_ERROR "middleware_wireless_ble_controller_config dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_controller_component)
# Add set(CONFIG_USE_middleware_wireless_ble_controller_component true) in config.cmake to use this component

message("middleware_wireless_ble_controller_component component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((NOT (CONFIG_NOT STREQUAL kw45b41zevk OR CONFIG_NOT STREQUAL k32w148evk OR CONFIG_NOT STREQUAL kw45b41zloc OR CONFIG_NOT STREQUAL frdmmcxw71)))

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/src/ble_controller_task.c
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/config/ble_ll_globals.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/config
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/interface
)

else()

message(SEND_ERROR "middleware_wireless_ble_controller_component dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_ble_controller_MWS)
# Add set(CONFIG_USE_middleware_wireless_ble_controller_MWS true) in config.cmake to use this component

message("middleware_wireless_ble_controller_MWS component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if((NOT (CONFIG_NOT STREQUAL kw45b41zevk OR CONFIG_NOT STREQUAL k32w148evk OR CONFIG_NOT STREQUAL kw45b41zloc OR CONFIG_NOT STREQUAL frdmmcxw71)))

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/src/mws/ble_mws.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../ble_controller/src/mws
)

else()

message(SEND_ERROR "middleware_wireless_ble_controller_MWS dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()

