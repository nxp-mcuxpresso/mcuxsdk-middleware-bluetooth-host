# Integration

Perform the following steps to integrate the RADE algorithm on the distance-measuring application:

1. Add the following components to the application project via Kconfig:
   - `CONFIG_MCUX_COMPONENT_middleware.wireless.rade2_algo_cm33_lce_lib=y`
   - `CONFIG_MCUX_PRJSEG_module.board.wireless.app_lce=y`
   - `CONFIG_MCUX_PRJSEG_module.board.wireless.linker_script_ble=n`
   - `CONFIG_MCUX_PRJSEG_module.board.wireless.linker_script_ble_loc=y`
2. `APP_InitLce()` will be automatically called by `AppLocalization_Init()` during application initialization.
3. Heap areas required by the algorithm are defined and registered in `app_lce_init.c`.