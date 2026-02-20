if(CONFIG_MCUX_PRJSEG_module.wireless.ble.app_common)
    mcux_add_include(
        INCLUDES
        .
        ../../boards/${board}
    )

    mcux_add_source(
        SOURCES
        app_conn.c
        app_conn.h
        ble_init.c
        ble_init.h
        app_advertiser.c
        app_advertiser.h
        app_connection.c
        app_scanner.c
        app_scanner.h
        app_nvm.c
    )

    mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/wireless/${bluetooth_root}/boards/${board}/app_preinclude_common.h
    )

    mcux_add_include(
        BASE_PATH ${SdkRootDirPath}
        INCLUDES middleware/wireless/${bluetooth_root}/boards/${board}
    )

    mcux_add_configuration(
        CC "-DgUseHciTransportDownward_d=1"
    )

    mcux_add_source(
        SOURCES
        freertos/main.c
        freertos/FreeRTOSConfig.h
        app_low_power.c
        COMPONENTS middleware.freertos-kernel
    )

    mcux_add_include(
        INCLUDES
        ./freertos
        COMPONENTS middleware.freertos-kernel

    )

    mcux_add_source(
        SOURCES
            bm/main.c
        COMPONENTS component.osa_bm
    )

    mcux_add_include(
        INCLUDES
            ./bm
        COMPONENTS component.osa_bm
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.wireless.ids_test)
    mcux_add_source(
        SOURCES ids_test/ids_test.c
                ids_test/ids_test.h
    )
    mcux_add_include(
        INCLUDES ids_test
    )
endif()
