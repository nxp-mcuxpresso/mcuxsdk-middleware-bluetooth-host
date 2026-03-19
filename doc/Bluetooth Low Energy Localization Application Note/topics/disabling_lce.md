# Enabling RADE v1 Usage (non-LCE)

Follow the steps below to disable LCE acceleration support and use the RADE v1 algorithm instead of the default RADE v2.

1. Open <SDK>/examples/_boards/<board>/wireless_examples/bluetooth/<example_name>/prj.conf:
 - replace CONFIG_MCUX_COMPONENT_middleware.wireless.genfsk_wr_lcl_algo_cm33_lce_lib=y with CONFIG_MCUX_COMPONENT_middleware.wireless.genfsk_wr_lcl_algo_cm33_lib=y.
 - replace CONFIG_MCUX_PRJSEG_module.board.wireless.linker_script_ble_loc=y with CONFIG_MCUX_PRJSEG_module.board.wireless.linker_script_ble=y
 - disable CONFIG_MCUX_PRJSEG_module.board.wireless.app_lce

2. Running the RADE algorithm without LCE support increases stack size requirements. Ensure the stack size for the task on which the algorithm runs is at least 3000 octets.

Note: Make sure the above components are listed only once in the prj.conf file. If duplicates exist, remove them before building the project.