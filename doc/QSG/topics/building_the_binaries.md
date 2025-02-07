# Building the binaries

This section describes the necessary steps for obtaining the binary files for usage with the boards.

## Prerequisites {#section_tfx_vfc_pcc .section}

To build any of the demo applications, you need the following toolchain:

-   IAR Embedded Workbench for Arm \(details in release note\)
-   MCUXpresso IDE \(details in release note\)
-   Visual Studio Code with "*MCUXpresso for Visual Studio Code*" extension \(details in release note\)
-   Teraterm \(version 4.105 or higher\)

The Connectivity Software Package does not include support for any other toolchains. The packages must be built with the debug configuration to enable debugging information. This package includes various sample applications that can be used as a starting point.

## Conventions for building the *wireless\_UART* application. {#002 .section}

The following sections present the steps required for building the *wireless\_UART* application. All applications can be found using the following placeholders for text:

-   `<connectivity_path>`: represents the root path for the SDK.

-   `<board>`: represents the target board for the demo app, “`kw47evk`” in this case.

-   `<RTOS>`: represents the scheduler or RTOS used by the app; it can be either “`bm`” or “`freertos`”.

-   `<demo_app>`: represents the demo application name.

-   `<IDE>`: represents the integrated development environment used to build projects; “iar” in this case.

-   `<core_id>`: represents the target CPU on which the application will run, "`cm33_core0`" in this case \(applicable only on KW47-EVK, KW47-LOC, MCX-W72-EVK and FRDM-MCXW72 boards\).
-   The general folder structure of the demo applications is the following:

    `<connectivity_path>\boars\<board>\wireless_examples\bluetooth\<demo_app>\<core_id>\<RTOS>\<IDE>`


**Selected application**: w\_uart

**Board**: One of the following boards:

-   kw45b41zevk
-   kw45b41zloc
-   k32w148evk
-   frdmmcxw71
-   kw47evk
-   kw47loc
-   mcxw72evk
-   frdmmcxw72

**RTOS**: FreeRTOS

**Resulting location**:

`<connectivity_path>\boards\<kw45b41zevk / kw45b41zloc / k32w148evk / frdmmcxw71 / kw47evk / kw47loc / mcxw72evk / frdmmcxw72>\wireless_examples\bluetooth\w_uart\freertos\<IDE>`


```{include} ../topics/building_and_flashing_the_ble_software_demo_applic.md
:heading-offset: 1
```

```{include} ../topics/building_and_flashing_the_ble_software_demo_applic_01.md
:heading-offset: 1
```

```{include} ../topics/building_the_ble_software_VS_code.md
:heading-offset: 1
```

