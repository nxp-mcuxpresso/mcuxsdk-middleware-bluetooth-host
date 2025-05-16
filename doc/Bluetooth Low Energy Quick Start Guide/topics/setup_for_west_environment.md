# Getting Started with west Build Environment

These steps are intended for **GitHub users** working with Bluetooth Low Energy (BLE) demo applications from the GitHub MCUXpresso SDK repository.

Follow the steps in the order presented in [Getting Started with MCUXpresso SDK Repository](https://mcuxpresso.nxp.com/mcuxsdk/latest/html/gsd/installation.html) to install the west environment.

**Command Line to Generate/Build Projects**

**Example: `w_uart` on `kw47evk`**

To build the `w_uart` example for the `kw47evk` board using the IAR toolchain and generate an IDE project, use the following command:

```
west build -b kw47evk examples/wireless_examples/bluetooth/w_uart/bm/ \ --toolchain=iar -t guiproject -Dcore_id=cm33_core0 --pristine
```

| Argument         | Description                                                  |
|------------------|--------------------------------------------------------------|
| `-b kw47evk`     | Specifies the board                                           |
| `--toolchain=iar`| Specifies the toolchain (default is `armgcc`)                |
| `-t guiproject`  | Creates an IDE project                                       |
| `-Dcore_id=cm33_core0` | Core ID, if available                                 |
| `--pristine`     | Cleans the build folder before building                      |

Use the following steps in order to build and flash the BLE software demo applications on all toolchains presented in:

- [Building and flashing the BLE software demo applications using IAR Embedded Workbench](https://mcuxpresso.nxp.com/mcuxsdk/latest/html/middleware/wireless/bluetooth/doc/Bluetooth%20Low%20Energy%20Quick%20Start%20Guide/topics/building_the_binaries.html#building-and-flashing-the-ble-software-demo-applications-using-iar-embedded-workbench)
- [Building and flashing the BLE Software Demo applications using MCUXpresso IDE](https://mcuxpresso.nxp.com/mcuxsdk/latest/html/middleware/wireless/bluetooth/doc/Bluetooth%20Low%20Energy%20Quick%20Start%20Guide/topics/building_the_binaries.html#building-and-flashing-the-ble-software-demo-applications-using-mcuxpresso-ide)
- [Building and flashing the BLE software demo applications using Visual Studio Code](https://mcuxpresso.nxp.com/mcuxsdk/latest/html/middleware/wireless/bluetooth/doc/Bluetooth%20Low%20Energy%20Quick%20Start%20Guide/topics/building_the_binaries.html#building-and-flashing-the-ble-software-demo-applications-using-visual-studio-code)
