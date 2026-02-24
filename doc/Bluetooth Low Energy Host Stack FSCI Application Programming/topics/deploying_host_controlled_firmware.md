# Deploying Host Controlled Firmware


IAR Embedded Workbench for Arm® (EWARM) and ARMGCC are the development toolchains used to deploy the Bluetooth LE host stack software applications.

Detailed information about how to build, deploy and debug an IAR or ARMGCC-based project are presented in the [Bluetooth Low Energy Demo Applications User Guide](../../Bluetooth%20Low%20Energy%20Demo%20Applications%20Users%20Guide/Bluetooth%20Low%20Energy%20Demo%20Applications%20Users%20Guide.md).


**Bluetooth LE application configuration**

To exercise the Host API, the Bluetooth LE _black box_ firmware is required to be flashed on a compatible platform. The Bluetooth LE _black box_ is represented by the `ble_fscibb` application firmware that can be interfaced and configured with FSCI commands over the serial interface.

The user can compile the _black box_ image of the `ble_fscibb` software application using IAR Embedded Workbench for Arm (EWARM) or ARMGCC. For information on how to build the application see additional documentation provided in the package.


**Parent topic:** [Bluetooth Low Energy Host Stack FSCI Application Programming](../Bluetooth%20Low%20Energy%20Host%20Stack%20FSCI%20Application%20Programming.md)
