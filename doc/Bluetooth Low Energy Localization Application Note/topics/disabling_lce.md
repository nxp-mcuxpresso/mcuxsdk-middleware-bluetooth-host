# Disabling LCE for non-LCE Phantoms

**IAR Embedded Workbench for Arm**

1. Go to Options > Linker > Config > Configuration file symbol definitions and change the value of gReserveCeRam_d from 1 to 0.
2. Go to Options > Linker > Library and replace lib_lcl_algo_cm33_lce_iar.a with the non-LCE library lib_lcl_algo_cm33_iar.a.
3. Go to Options > C/C++ Compiler > Preprocessor > Defined symbols and remove LCE_KW47_MCXW72 from the list.
4. Running the RADE algorithm without LCE support increases stack size requirements. Ensure the stack size for the task on which the algorithm runs is at least 3000 octets.

**MCUXpresso IDE**

1. Go to Properties > C/C++ Build > MCU settings > Memory details and delete the following sections: ce_image, ce_ctrl, ce_data1, ce_data2, ce_data_reserved, ce_data_unused. Edit the size of the SRAM section to 0x39c00 to reuse the RAM resulted from the deletion of the CE sections.
2. Go to Properties > C/C++ Build > Settings > MCU C Compiler > Preprocessor and remove the LCE_KW47_MCXW72 symbol from the Defined symbols list.
3. Go to Properties > C/C++ Build > Settings > MCU Linker > Libraries and replace _lcl_algo_cm33_lce_gcc with _lcl_algo_cm33_gcc in the Libraries list.
4. Copy lib_lcl_algo_cm33_gcc.a from middleware\wireless\bluetooth\localization\lib in the downloaded package to  <mcux_workspace>\<project_name>\bluetooth\localization\lib.
5. Go to linkscripts\symbols.ldt and remove the "LCE MEMORY" section at the end of the file.
6. Go to linkscripts\section_tail.ldt and remove the entire content of the file.
7. Running the RADE algorithm without LCE support increases stack size requirements. Ensure the stack size for the task on which the algorithm runs is at least 3000 octets.

**ARMGCC**

1. Open <project_name>\armgcc\flags.cmake and remove "-DLCE_KW47_MCXW72" from the CMAKE_C_FLAGS_DEBUG and/or CMAKE_C_FLAGS_RELEASE section. In the same file, change the value of gReserveCeRam_d from 1 to 0 in the CMAKE_EXE_LINKER_FLAGS_DEBUG and/or CMAKE_EXE_FLAGS_RELEASE section.
2. Open <project_name>\armgcc\config.cmake and change set(CONFIG_USE_middleware_wireless_genfsk_wr_lcl_algo_cm33_lce_lib true) to set(CONFIG_USE_middleware_wireless_genfsk_wr_lcl_algo_cm33_lib true) in order to use the non-LCE algorithm library.
3. Open <project_name>\linkscripts\symbols.ldt and remove the "LCE MEMORY" section at the end of the file.
4. Open <project_name>\linkscipts\section_tail.ldt and remove the entire content of the file.
5. Running the RADE algorithm without LCE support increases stack size requirements. Ensure the stack size for the task on which the algorithm runs is at least 3000 octets.

