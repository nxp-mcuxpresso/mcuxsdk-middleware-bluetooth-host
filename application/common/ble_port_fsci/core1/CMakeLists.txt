if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_component_button_core1)
    mcux_add_source(
        SOURCES button/fsl_component_button.c
            button/fsl_component_button.h
    )
    mcux_add_include(
        INCLUDES button
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_component_led_core1)
    mcux_add_source(
        SOURCES led/fsl_component_led.h
    )
    mcux_add_include(
        INCLUDES led
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_sensors_core1)
    mcux_add_source(
        SOURCES sensors/sensors.c
            sensors/sensors.h
    )
    mcux_add_include(
        INCLUDES sensors
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_component_reset_core1)
    mcux_add_source(
        SOURCES reset/fsl_adapter_reset.c
            reset/fsl_adapter_reset.h
    )
    mcux_add_include(
        INCLUDES reset
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_component_lcl_core1)
    mcux_add_source(
        SOURCES lcl/fwk_platform_lcl.c
            lcl/fwk_platform_lcl.h
    )
    mcux_add_include(
        INCLUDES lcl
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_shell_core1)
    mcux_add_source(
        SOURCES shell/fsl_shell.c
            shell/fsl_shell.h
    )
    mcux_add_include(
        INCLUDES shell
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_nvm_core1)
    mcux_add_source(
        SOURCES nvm/NV_Flash.c
            nvm/NVM_Interface.h
            nvm/app_nvm.c
            nvm/app_nvm.h
    )
    mcux_add_include(
        INCLUDES nvm
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ble_port_fsci_platform_core1)
    mcux_add_source(
        SOURCES platform/platform.c
            platform/platform.h
    )
    mcux_add_include(
        INCLUDES platform
    )
endif()
