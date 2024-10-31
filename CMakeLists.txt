
if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host)
    mcux_add_source(
        SOURCES host/interface/att_callbacks.h
                host/interface/att_errors.h
                host/interface/att_params.h
                host/interface/att_types.h
                host/interface/ble_constants.h
                host/interface/ble_general.h
                host/interface/ble_host_tasks.h
                host/interface/ble_sig_defines.h
                host/interface/ble_utils.h
                host/interface/gap_interface.h
                host/interface/gap_types.h
                host/interface/gatt_client_interface.h
                host/interface/gatt_database.h
                host/interface/gatt_db_app_interface.h
                host/interface/gatt_db_att_interface.h
                host/interface/gatt_db_dynamic.h
                host/interface/gatt_interface.h
                host/interface/gatt_server_interface.h
                host/interface/gatt_types.h
                host/interface/hci_interface.h
                host/interface/hci_types.h
                host/interface/l2ca_cb_interface.h
                host/interface/l2ca_interface.h
                host/interface/l2ca_types.h
                host/interface/vendor_debug_commands.h
                port/fwk_generic_list.h
                port/fwk_generic_list.c
                port/fwk_mem_manager.h
                port/fwk_messaging.h
                port/fwk_messaging.c
                port/fwk_os_abs.h
                port/fwk_os_abs.c
                port/fwk_panic.h
                port/fwk_timer_manager.h
                port/fwk_timer_manager.c
                port/fwk_seclib.h
                host/config/ble_config.h
                host/config/ble_globals.c
    )
    mcux_add_include(
        INCLUDES host/interface
                 host/config
                 port
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_common)
    mcux_add_source(
        SOURCES application/common/ble_host_tasks.c
                application/common/ble_host_task_config.h
                application/common/ble_conn_manager.c
                application/common/ble_conn_manager.h
    )
    mcux_add_include(
        INCLUDES application/common
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_ncp_common)
    mcux_add_source(
        SOURCES application/common/ble_host_tasks.c
                application/common/ble_host_task_config.h
                application/common/ncp/ncp_app_conn.c
                application/common/ncp/ncp_app_conn.h
                application/common/ncp/ncp_ble_conn_manager.c
                application/common/ncp/ncp_ble_init.c
                application/common/ncp/ncp_ble_init.h
    )
    mcux_add_include(
        INCLUDES application/common
                 application/common/ncp
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_OPT_component_lib)
    mcux_add_library(
        LIBS host/lib/lib_ble_OPT_host_cm33_iar.a
        DSP DSP
        FPU SP_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_OPT_host_cm33_nofp_nodsp_iar.a
        DSP NO_DSP
        FPU NO_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_OPT_host_cm33_gcc.a
        TOOLCHAINS mcux armgcc
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_OPT_central_component_lib)
    mcux_add_library(
        LIBS host/lib/lib_ble_OPT_host_central_cm33_iar.a
        DSP DSP
        FPU SP_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_OPT_host_central_cm33_nofp_nodsp_iar.a
        DSP NO_DSP
        FPU NO_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_OPT_host_central_cm33_gcc.a
        TOOLCHAINS mcux armgcc
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_OPT_peripheral_component_lib)
    mcux_add_library(
        LIBS host/lib/lib_ble_OPT_host_peripheral_cm33_iar.a
        DSP DSP
        FPU SP_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_OPT_host_peripheral_cm33_nofp_nodsp_iar.a
        DSP NO_DSP
        FPU NO_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_OPT_host_peripheral_cm33_gcc.a
        TOOLCHAINS mcux armgcc
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_component_lib)
    mcux_add_library(
        LIBS host/lib/lib_ble_host_cm33_iar.a
        DSP DSP
        FPU SP_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_host_cm33_nofp_nodsp_iar.a
        DSP NO_DSP
        FPU NO_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_host_cm33_gcc.a
        TOOLCHAINS mcux armgcc
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_central_component_lib)
    mcux_add_library(
        LIBS host/lib/lib_ble_host_central_cm33_iar.a
        DSP DSP
        FPU SP_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_host_central_cm33_nofp_nodsp_iar.a
        DSP NO_DSP
        FPU NO_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_host_central_cm33_gcc.a
        TOOLCHAINS mcux armgcc
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_peripheral_component_lib)
    mcux_add_library(
        LIBS host/lib/lib_ble_host_peripheral_cm33_iar.a
        DSP DSP
        FPU SP_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_host_peripheral_cm33_nofp_nodsp_iar.a
        DSP NO_DSP
        FPU NO_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_host_peripheral_cm33_gcc.a
        TOOLCHAINS mcux armgcc
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_matter_component_lib)
    mcux_add_library(
        LIBS host/lib/lib_ble_host_matter_cm33_iar.a
        DSP DSP
        FPU SP_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_host_matter_cm33_nofp_nodsp_iar.a
        DSP NO_DSP
        FPU NO_FPU
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_host_matter_cm33_gcc.a
        TOOLCHAINS mcux armgcc
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_exp_OPT_component_lib)
    mcux_add_library(
        LIBS host/lib_exp/lib_ble_OPT_host_cm33_iar.a
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib_exp/lib_ble_OPT_host_cm33_gcc.a
        TOOLCHAINS mcux armgcc
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_exp_OPT_central_component_lib)
    mcux_add_library(
        LIBS host/lib_exp/lib_ble_OPT_host_central_cm33_iar.a
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib_exp/lib_ble_OPT_host_central_cm33_gcc.a
        TOOLCHAINS mcux armgcc
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_exp_OPT_peripheral_component_lib)
    mcux_add_library(
        LIBS host/lib_exp/lib_ble_OPT_host_peripheral_cm33_iar.a
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib_exp/lib_ble_OPT_host_peripheral_cm33_gcc.a
        TOOLCHAINS mcux armgcc
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_interface)
    mcux_add_source(
        SOURCES host/interface/att_callbacks.h
                host/interface/att_errors.h
                host/interface/att_params.h
                host/interface/att_types.h
                host/interface/ble_constants.h
                host/interface/ble_general.h
                host/interface/ble_host_tasks.h
                host/interface/ble_sig_defines.h
                host/interface/ble_utils.h
                host/interface/gap_interface.h
                host/interface/gap_types.h
                host/interface/gatt_client_interface.h
                host/interface/gatt_database.h
                host/interface/gatt_db_app_interface.h
                host/interface/gatt_db_dynamic.h
                host/interface/gatt_interface.h
                host/interface/gatt_server_interface.h
                host/interface/gatt_types.h
                host/interface/hci_interface.h
                host/interface/hci_types.h
                host/interface/l2ca_cb_interface.h
                host/interface/l2ca_interface.h
                host/interface/l2ca_types.h
                host/interface/vendor_debug_commands.h
                host/config/ble_config.h
                application/common/ble_host_task_config.h
                port/fwk_generic_list.h
                port/fwk_mem_manager.h
                port/fwk_messaging.h
                port/fwk_os_abs.h
                port/fwk_panic.h
                port/fwk_timer_manager.h
                port/fwk_seclib.h
    )
    mcux_add_include(
        INCLUDES host/interface
                 host/config
                 application/common
                 port
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_xml)
    mcux_add_source(
        SOURCES fsci/xml/package/BLE_1.10.3.xml
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_sdk)
    mcux_add_source(
        SOURCES tools/host_sdk/CHANGELOG
            tools/host_sdk/HOWTO-FSCIACK
            tools/host_sdk/HOWTO-SPI
            tools/host_sdk/hsdk/ConnectivityLibrary.sln
            tools/host_sdk/hsdk/ConnectivityLibrary.vcxproj
            tools/host_sdk/hsdk/ConnectivityLibrary.vcxproj.filters
            tools/host_sdk/hsdk/demo/bin/dk07/libframer.so
            tools/host_sdk/hsdk/demo/bin/dk07/libfsci.so
            tools/host_sdk/hsdk/demo/bin/dk07/libphysical.so
            tools/host_sdk/hsdk/demo/bin/dk07/libspi.so
            tools/host_sdk/hsdk/demo/bin/dk07/libsys.so
            tools/host_sdk/hsdk/demo/bin/dk07/libuart.so
            tools/host_sdk/hsdk/demo/bin/dk07/setup.dk07.sh
            tools/host_sdk/hsdk/demo/bin/dk07/Thread_Shell
            tools/host_sdk/hsdk/demo/FsciBootloader.c
            tools/host_sdk/hsdk/demo/GetKinetisDevices.c
            tools/host_sdk/hsdk/demo/Makefile
            tools/host_sdk/hsdk/doc/Protocol.md
            tools/host_sdk/hsdk/doc/Serial.md
            tools/host_sdk/hsdk/doc/Sys.md
            tools/host_sdk/hsdk/HSDK.rc
            tools/host_sdk/hsdk/include/physical/PCAP/PCAPDevice.h
            tools/host_sdk/hsdk/include/physical/PhysicalDevice.h
            tools/host_sdk/hsdk/include/physical/SPI/SPIConfiguration.h
            tools/host_sdk/hsdk/include/physical/SPI/SPIDevice.h
            tools/host_sdk/hsdk/include/physical/UART/UARTConfiguration.h
            tools/host_sdk/hsdk/include/physical/UART/UARTDevice.h
            tools/host_sdk/hsdk/include/physical/UART/UARTDiscovery.h
            tools/host_sdk/hsdk/include/protocol/Framer.h
            tools/host_sdk/hsdk/include/protocol/FSCI/FSCIFrame.h
            tools/host_sdk/hsdk/include/protocol/FSCI/FSCIFramer.h
            tools/host_sdk/hsdk/include/sys/EventManager.h
            tools/host_sdk/hsdk/include/sys/hsdkError.h
            tools/host_sdk/hsdk/include/sys/hsdkLogger.h
            tools/host_sdk/hsdk/include/sys/hsdkOSCommon.h
            tools/host_sdk/hsdk/include/sys/MessageQueue.h
            tools/host_sdk/hsdk/include/sys/RawFrame.h
            tools/host_sdk/hsdk/include/sys/utils.h
            tools/host_sdk/hsdk/Makefile
            tools/host_sdk/hsdk/physical/PCAP/PCAPDevice.c
            tools/host_sdk/hsdk/physical/PhysicalDevice.c
            tools/host_sdk/hsdk/physical/SPI/SPIConfiguration.c
            tools/host_sdk/hsdk/physical/SPI/SPIDevice.c
            tools/host_sdk/hsdk/physical/UART/UARTConfiguration.c
            tools/host_sdk/hsdk/physical/UART/UARTDevice.c
            tools/host_sdk/hsdk/physical/UART/UARTDiscovery.c
            tools/host_sdk/hsdk/protocol/Framer.c
            tools/host_sdk/hsdk/protocol/FSCI/FSCIFrame.c
            tools/host_sdk/hsdk/protocol/FSCI/FSCIFramer.c
            tools/host_sdk/hsdk/README.md
            tools/host_sdk/hsdk/res/77-mm-usb-device-blacklist.rules
            tools/host_sdk/hsdk/res/hsdk.conf
            tools/host_sdk/hsdk/resource.h
            tools/host_sdk/hsdk/sys/EventManager.c
            tools/host_sdk/hsdk/sys/hsdkEvent.c
            tools/host_sdk/hsdk/sys/hsdkFile.c
            tools/host_sdk/hsdk/sys/hsdkLock.c
            tools/host_sdk/hsdk/sys/hsdkLogger.c
            tools/host_sdk/hsdk/sys/hsdkSemaphore.c
            tools/host_sdk/hsdk/sys/hsdkThread.c
            tools/host_sdk/hsdk/sys/MessageQueue.c
            tools/host_sdk/hsdk/sys/RawFrame.c
            tools/host_sdk/hsdk/sys/utils.c
            tools/host_sdk/hsdk-c/demo/HeartRateSensor.c
            tools/host_sdk/hsdk-c/demo/Makefile
            tools/host_sdk/hsdk-c/inc/ble_sig_defines.h
            tools/host_sdk/hsdk-c/inc/cmd_ble.h
            tools/host_sdk/hsdk-c/inc/os_abstraction.h
            tools/host_sdk/hsdk-c/README.md
            tools/host_sdk/hsdk-c/src/cmd_ble.c
            tools/host_sdk/hsdk-c/src/evt_ble.c
            tools/host_sdk/hsdk-c/src/evt_printer_ble.c
            tools/host_sdk/hsdk-c/src/unload_ble.c
            tools/host_sdk/hsdk-python/lib/README.md
            tools/host_sdk/hsdk-python/lib/x64/HSDK.dll
            tools/host_sdk/hsdk-python/lib/x86/HSDK.dll
            tools/host_sdk/hsdk-python/README.md
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/ble/ble_sig_defines.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/ble/enums.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/ble/events.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/ble/frames.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/ble/gatt_database_dynamic.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/ble/heart_rate_interface.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/ble/operations.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/ble/spec.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/ble/sync_requests.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/ble/__init__.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/comm.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/firmware/enums.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/firmware/events.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/firmware/frames.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/firmware/operations.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/firmware/spec.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/firmware/sync_requests.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/firmware/__init__.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/fsci_data_packet.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/fsci_frame_description.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/fsci_operation.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/fsci_parameter.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/commands/__init__.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/CFsciLibrary.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/config.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/CUartLibrary.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/device/device_manager.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/device/physical_device.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/device/__init__.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/framing/fsci_command.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/framing/fsci_framer.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/framing/__init__.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/library_loader.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/ota_server.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/singleton.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/sniffer.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/utils.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/hsdk/__init__.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/test/hrs.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/test/__init__.py
            tools/host_sdk/hsdk-python/src/com/nxp/wireless_connectivity/__init__.py
            tools/host_sdk/hsdk-python/src/com/nxp/__init__.py
            tools/host_sdk/hsdk-python/src/com/__init__.py
            tools/host_sdk/hsdk-python/src/__init__.py
            tools/host_sdk/middleware_wireless_ble_host_sdk.cmake
            tools/host_sdk/README.txt
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_interface_black_box)
    mcux_add_source(
        SOURCES host/interface/att_callbacks.h
                host/interface/att_errors.h
                host/interface/att_params.h
                host/interface/att_types.h
                host/interface/ble_constants.h
                host/interface/ble_general.h
                host/interface/ble_host_tasks.h
                host/interface/ble_sig_defines.h
                host/interface/ble_utils.h
                host/interface/gap_interface.h
                host/interface/gap_types.h
                host/interface/gatt_client_interface.h
                host/interface/gatt_database.h
                host/interface/gatt_db_app_interface.h
                host/interface/gatt_db_dynamic.h
                host/interface/gatt_interface.h
                host/interface/gatt_server_interface.h
                host/interface/gatt_types.h
                host/interface/hci_interface.h
                host/interface/hci_types.h
                host/interface/l2ca_cb_interface.h
                host/interface/l2ca_interface.h
                host/interface/l2ca_types.h
                host/interface/vendor_debug_commands.h
                application/common/ble_host_tasks.c
                application/common/ble_host_task_config.h
                host/config/ble_config.h
                host/config/ble_globals.c
                port/fwk_generic_list.h
                port/fwk_mem_manager.h
                port/fwk_messaging.h
                port/fwk_os_abs.h
                port/fwk_panic.h
                port/fwk_timer_manager.h
                port/fwk_seclib.h
    )
    mcux_add_include(
        INCLUDES host/interface
                 application/common
                 host/config
                 port
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_interface_host)
    mcux_add_source(
        SOURCES host/interface/att_errors.h
                host/interface/att_types.h
                host/interface/ble_constants.h
                host/interface/ble_general.h
                host/interface/ble_sig_defines.h
                host/interface/ble_utils.h
                host/interface/gap_interface.h
                host/interface/gap_types.h
                host/interface/gatt_client_interface.h
                host/interface/gatt_database.h
                host/interface/gatt_db_app_interface.h
                host/interface/gatt_interface.h
                host/interface/gatt_server_interface.h
                host/interface/gatt_types.h
                host/interface/gatt_db_dynamic.h
                host/interface/l2ca_cb_interface.h
                host/interface/l2ca_interface.h
                host/interface/l2ca_types.h
                host/interface/vendor_debug_commands.h
                host/config/ble_config.h
                host/config/ble_globals.c
    )
    mcux_add_include(
        INCLUDES host/interface
                 application/common
                 host/config
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_interface_genfsk)
    mcux_add_source(
        SOURCES host/interface/ble_constants.h
                host/interface/ble_general.h
                host/interface/hci_types.h
                host/interface/ble_sig_defines.h
                host/interface/ble_utils.h
                host/interface/vendor_debug_commands.h
    )
    mcux_add_include(
        INCLUDES host/interface
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_fsci_interface)
    mcux_add_source(
        SOURCES fsci/interface/fsci_ble_interface.h
                fsci/interface/fsci_ble_private_interface.h
    )
    mcux_add_include(
        INCLUDES fsci/interface
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_fsci_private_interface)
    mcux_add_source(
        SOURCES fsci/interface/fsci_ble_private_interface.h
    )
    mcux_add_include(
        INCLUDES fsci/interface
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_fsci_source)
    mcux_add_source(
        SOURCES fsci/source/fsci_ble.c
                fsci/source/fsci_ble.h
                fsci/source/fsci_ble_gap.c
                fsci/source/fsci_ble_gap.h
                fsci/source/fsci_ble_gap_types.c
                fsci/source/fsci_ble_gap_types.h
                fsci/source/fsci_ble_gap_handlers.c
                fsci/source/fsci_ble_gap_handlers.h
                fsci/source/fsci_ble_gap2_handlers.c
                fsci/source/fsci_ble_gap2_handlers.h
                fsci/source/fsci_ble_gatt.c
                fsci/source/fsci_ble_gatt.h
                fsci/source/fsci_ble_gatt_db_app.c
                fsci/source/fsci_ble_gatt_db_app.h
                fsci/source/fsci_ble_gatt_db_app_types.c
                fsci/source/fsci_ble_gatt_db_app_types.h
                fsci/source/fsci_ble_gatt_types.c
                fsci/source/fsci_ble_gatt_types.h
                fsci/source/fsci_ble_l2cap_cb.c
                fsci/source/fsci_ble_l2cap_cb.h
                fsci/source/fsci_ble_l2cap_cb_types.c
                fsci/source/fsci_ble_l2cap_cb_types.h
                fsci/source/fsci_ble_types.c
                fsci/source/fsci_ble_types.h
    )
    mcux_add_include(
        INCLUDES fsci/source
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_ncp_fsci_source)
    mcux_add_source(
        SOURCES application/common/ncp/ncp_fsci_ble_gap_handlers.c
                application/common/ncp/ncp_fsci_ble_gap.c
                fsci/source/fsci_ble.c
                fsci/source/fsci_ble.h
                fsci/source/fsci_ble_gap.h
                fsci/source/fsci_ble_gap_types.c
                fsci/source/fsci_ble_gap_types.h
                fsci/source/fsci_ble_gap_handlers.h
                fsci/source/fsci_ble_gap2_handlers.c
                fsci/source/fsci_ble_gap2_handlers.h
                fsci/source/fsci_ble_gatt.c
                fsci/source/fsci_ble_gatt.h
                fsci/source/fsci_ble_gatt_db_app.c
                fsci/source/fsci_ble_gatt_db_app.h
                fsci/source/fsci_ble_gatt_db_app_types.c
                fsci/source/fsci_ble_gatt_db_app_types.h
                fsci/source/fsci_ble_gatt_types.c
                fsci/source/fsci_ble_gatt_types.h
                fsci/source/fsci_ble_l2cap_cb.c
                fsci/source/fsci_ble_l2cap_cb.h
                fsci/source/fsci_ble_l2cap_cb_types.c
                fsci/source/fsci_ble_l2cap_cb_types.h
                fsci/source/fsci_ble_types.c
                fsci/source/fsci_ble_types.h
    )
    mcux_add_include(
        INCLUDES fsci/source
                 application/common/ncp
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_fsci_source_handover)
    mcux_add_source(
        SOURCES fsci/source/fsci_ble_gap_handover.c
                fsci/source/fsci_ble_gap_handover.h
                fsci/source/fsci_ble_gap_handover_types.c
                fsci/source/fsci_ble_gap_handover_types.h
    )
    mcux_add_include(
        INCLUDES fsci/source
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_fsci_host)
    mcux_add_source(
        SOURCES fsci/host/host_ble.c
                fsci/host/host_ble.h
                fsci/host/host_gap.c
                fsci/host/host_gatt.c
                fsci/host/host_gatt_db.c
                fsci/host/host_l2cap_cb.c
    )
    mcux_add_include(
        INCLUDES fsci/host
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_interface_handover)
    mcux_add_source(
        SOURCES host/interface/gap_handover_interface.h
                host/interface/gap_handover_types.h
                host/interface/hci_handover_interface.h
                host/interface/hci_handover_types.h
    )
    mcux_add_include(
        INCLUDES host/interface
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_host_handover_component_lib)
    mcux_add_library(
        LIBS host/lib/lib_ble_handover_cm33_iar.a
        TOOLCHAINS iar
    )
    mcux_add_library(
        LIBS host/lib/lib_ble_handover_cm33_gcc.a
        TOOLCHAINS mcux armgcc
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.auto)
    mcux_add_source(
        SOURCES application/common/auto/app_a2a_interface.c
                application/common/auto/app_a2a_interface.h
                application/common/auto/app_a2b.c
                application/common/auto/app_a2b.h
                application/common/auto/app_handover.c
                application/common/auto/app_handover.h
    )
    mcux_add_include(
        INCLUDES application/common/auto
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_init_matter)
    mcux_add_source(
        SOURCES application/common/ble_init.c
                application/common/ble_init.h
    )
    mcux_add_include(
        INCLUDES application/common/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_hci_transport_serial)
    mcux_add_source(
        SOURCES hci_transport/interface/hci_transport.h
                hci_transport/source/hcit_serial_interface.c
    )
    mcux_add_include(
        INCLUDES hci_transport/interface
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_hci_transport_rpmsg)
    mcux_add_source(
        SOURCES hci_transport/interface/hci_transport.h
                hci_transport/source/hcit_generic_adapter_interface.c
    )
    mcux_add_include(
        INCLUDES hci_transport/interface
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_hci_transport_adapter)
    mcux_add_source(
        SOURCES hci_transport/interface/hci_transport.h
                hci_transport/source/hcit_adapter_interface.c
    )
    mcux_add_include(
        INCLUDES hci_transport/interface
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_hci_transport_interface)
    mcux_add_source(
        SOURCES hci_transport/interface/hci_transport.h
    )
    mcux_add_include(
        INCLUDES hci_transport/interface
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_gatt_db_dynamic)
    mcux_add_source(
        SOURCES application/common/gatt_db/gatt_database.c
                application/common/gatt_db/gatt_database_dynamic.c
                application/common/gatt_db/gatt_database_dynamic.h
    )
    mcux_add_include(
        INCLUDES application/common/gatt_db
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_gatt_service_discovery)
    mcux_add_source(
        SOURCES application/common/ble_service_discovery.c
                application/common/ble_service_discovery.h
    )
    mcux_add_include(
        INCLUDES application/common
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_gatt_db)
    mcux_add_source(
        SOURCES application/common/gatt_db/macros/gatt_alloc_x.h
                application/common/gatt_db/macros/gatt_db_macros.h
                application/common/gatt_db/macros/gatt_db_x_macros.h
                application/common/gatt_db/macros/gatt_decl_x.h
                application/common/gatt_db/macros/gatt_enum_x.h
                application/common/gatt_db/macros/gatt_init_x.h
                application/common/gatt_db/macros/gatt_size_x.h
                application/common/gatt_db/macros/gatt_uuid_decl_x.h
                application/common/gatt_db/macros/gatt_uuid_def_x.h
                application/common/gatt_db/gatt_database.c
                application/common/gatt_db/gatt_db_handles.h
                application/common/ieee11073.h
    )
    mcux_add_include(
        INCLUDES application/common/gatt_db/macros
                 application/common/gatt_db
                 application/common
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_profiles_battery_service)
    mcux_add_source(
        SOURCES profiles/battery/battery_service.c
                profiles/battery/battery_interface.h
    )
    mcux_add_include(
        INCLUDES profiles/battery
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_profiles_device_info_service)
    mcux_add_source(
        SOURCES profiles/device_info/device_info_service.c
                profiles/device_info/device_info_interface.h
    )
    mcux_add_include(
        INCLUDES profiles/device_info
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_profiles_ancs)
    mcux_add_source(
        SOURCES profiles/ancs/ancs_interface.h
    )
    mcux_add_include(
        INCLUDES profiles/ancs
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_profiles_time)
    mcux_add_source(
        SOURCES profiles/time/current_time_interface.h
                profiles/time/current_time_service.c
                profiles/time/next_dst_change_interface.h
                profiles/time/next_dst_change_service.c
                profiles/time/reference_time_update_interface.h
                profiles/time/reference_time_update_service.c
    )
    mcux_add_include(
        INCLUDES profiles/time
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_profiles_hid)
    mcux_add_source(
        SOURCES profiles/hid/hid_service.c
                profiles/hid/hid_interface.h
    )
    mcux_add_include(
        INCLUDES profiles/hid
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_profiles_temperature)
    mcux_add_source(
        SOURCES profiles/temperature/temperature_service.c
                profiles/temperature/temperature_interface.h
    )
    mcux_add_include(
        INCLUDES profiles/temperature
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_profiles_wireless_uart)
    mcux_add_source(
        SOURCES profiles/wireless_uart/wireless_uart_service.c
                profiles/wireless_uart/wireless_uart_interface.h
    )
    mcux_add_include(
        INCLUDES profiles/wireless_uart
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_profiles_otap)
    mcux_add_source(
        SOURCES profiles/otap/otap_service.c
                profiles/otap/otap_interface.h
    )
    mcux_add_include(
        INCLUDES profiles/otap
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_profiles_digital_key)
    mcux_add_source(
        SOURCES profiles/digital_key/digital_key_service.c
                profiles/digital_key/digital_key_interface.h
    )
    mcux_add_include(
        INCLUDES profiles/digital_key
    )
endif()

