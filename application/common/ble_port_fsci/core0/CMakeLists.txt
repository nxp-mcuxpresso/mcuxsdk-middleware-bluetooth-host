if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_component_button_core0)
    mcux_add_source(
        SOURCES button/fsl_component_button_core0.c
            button/fsl_component_button_core0.h
    )
    mcux_add_macro(gFsciComponentButtonCore0_d=1)
    mcux_add_include(
        INCLUDES button
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_component_led_core0)
    mcux_add_source(
        SOURCES led/fsl_component_led_core0.c
            led/fsl_component_led_core0.h
    )
    mcux_add_macro(gFsciComponentLedCore0_d=1)
    mcux_add_include(
        INCLUDES led
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_component_reset_core0)
    mcux_add_source(
        SOURCES reset/fsl_adapter_reset_core0.c
            reset/fsl_adapter_reset_core0.h
    )
    mcux_add_macro(gFsciComponentResetCore0_d=1)
    mcux_add_include(
        INCLUDES reset
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_shell_core0)
    mcux_add_source(
        SOURCES shell/fsl_shell_core0.c
            shell/fsl_shell_core0.h
    )
    mcux_add_macro(gFsciComponentShellCore0_d=1)
    mcux_add_include(
        INCLUDES shell
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_component_nvm_core0)
    mcux_add_source(
        SOURCES nvm/app_nvm_core0.c
            nvm/app_nvm_core0.h
    )
    mcux_add_macro(gFsciComponentNvmCore0_d=1)
    mcux_add_include(
        INCLUDES nvm
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_component_platform_core0)
    mcux_add_source(
        SOURCES platform/platform_core0.c
            platform/platform_core0.h
    )
    mcux_add_macro(gFsciComponentPlatformCore0_d=1)
    mcux_add_include(
        INCLUDES platform
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_component_sensors_core0)
    mcux_add_source(
        SOURCES sensors/sensors_core0.c
            sensors/sensors_core0.h
    )
    mcux_add_macro(gFsciComponentSensorsCore0_d=1)
    mcux_add_include(
        INCLUDES sensors
    )
endif()

