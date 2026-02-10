# Enabling RADE v1 Usage (non-LCE)

Follow the steps below to disable LCE acceleration support and use the RADE v1 algorithm instead of the default RADE v2.

**IAR Embedded Workbench for Arm**

1. Go to Options > Linker > Config > Configuration file symbol definitions and change the value of gReserveCeRam_d from 1 to 0.
2. Go to Options > Linker > Library and replace lib_lcl_algo_cm33_lce_iar.a with the non-LCE library lib_lcl_algo_cm33_iar.a.
3. Go to Options > C/C++ Compiler > Preprocessor > Defined symbols and remove LCE_KW47_MCXW72 from the list.
4. Running the RADE algorithm without LCE support increases stack size requirements. Ensure the stack size for the task on which the algorithm runs is at least 3000 octets.

**ARMGCC**

1. Open <project_name>\armgcc\flags.cmake and remove "-DLCE_KW47_MCXW72" from the CMAKE_C_FLAGS_DEBUG and/or CMAKE_C_FLAGS_RELEASE section. In the same file, change the value of gReserveCeRam_d from 1 to 0 in the CMAKE_EXE_LINKER_FLAGS_DEBUG and/or CMAKE_EXE_FLAGS_RELEASE section.
2. Open <project_name>\armgcc\config.cmake and change set(CONFIG_USE_middleware_wireless_genfsk_wr_lcl_algo_cm33_lce_lib true) to set(CONFIG_USE_middleware_wireless_genfsk_wr_lcl_algo_cm33_lib true) in order to use the non-LCE algorithm library.
3. Open <project_name>\linkscripts\symbols.ldt and remove the "LCE MEMORY" section at the end of the file.
4. Open <project_name>\linkscipts\section_tail.ldt and remove the entire content of the file.
5. Running the RADE algorithm without LCE support increases stack size requirements. Ensure the stack size for the task on which the algorithm runs is at least 3000 octets.

