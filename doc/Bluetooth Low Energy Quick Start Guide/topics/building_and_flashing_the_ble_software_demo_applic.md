# Building and flashing BLE demo applications using IAR Embedded Workbench

This guide shows how to build and flash Bluetooth Low Energy demo applications using IAR Embedded Workbench and the west build environment.

## Prerequisites

- **west build environment** - Follow [Getting Started with MCUXpresso SDK Repository](https://mcuxpresso.nxp.com/mcuxsdk/latest/html/gsd/installation.html)
- **IAR Embedded Workbench for Arm** (see release notes for version)
- **GitHub MCUXpresso SDK repository** cloned locally (west init -m https://github.com/nxp-mcuxpresso/mcuxsdk-manifests.git)

## Step 1: Generate IAR Project

Use west build to generate the IAR project:

```bash
west build -b <board> <application_path> --toolchain=iar -t guiproject -Dcore_id=<core_id> --pristine
```

**Example for Wireless UART on KW47-EVK:**

```bash
west build -b kw47evk examples/wireless_examples/bluetooth/w_uart/freertos/ --toolchain=iar -t guiproject -Dcore_id=cm33_core0 --pristine
```

**Parameters:**

| Parameter | Description |
|-----------|-------------|
| `-b <board>` | Target board (kw47evk, kw47loc, frdmkw43, frdmmcxw72, etc.) |
| `--toolchain=iar` | Use IAR toolchain |
| `-t guiproject` | Generate IAR workspace |
| `-Dcore_id=cm33_core0` | CPU core (for KW47, MCXW72) |
| `--pristine` | Clean build |

**Supported boards:** kw47evk, kw47loc, frdmmcxw71, frdmmcxw72, mcxw72evk, frdmkw43.

**Output:** IAR workspace at `build/iar/<project_name>.eww`

**Note:** The west build command builds in debug mode by default. To build in release mode, add `--config=release` to the command.

## Step 2: Open IAR Workspace

1. Navigate to `build/iar/`
2. Open the `.eww` file in IAR Embedded Workbench

![IAR workspace location](../images/Rev1.1_figure4.png)

![IAR workspace](../images/Rev1.1_figure6.png)

## Step 3: Build Project

Build the project using **Project > Make** or **F7**.

![Build application](../images/MCXW72_I/Figure8.png)

## Step 4: Configure Debugger

Go to **Project > Options (Alt+F7) > Debugger > Setup > Driver**

Select debugger:
- **CMSIS-DAP** for kw45b41zevk, mcxw71evk, frdmmcxw71
- **J-Link** for kw47evk, kw47loc, mcxw72evk, frdmmcxw72, frdmkw43

![Debugger settings](../images/MCXW72_I/Figure9.png)

## Step 5: Flash and Debug

Click **Download and Debug** or press **Ctrl+D**.

![Download and debug](../images/image6.png)

## Step 6: Run Application

Press **Go (F5)** to start the application.

![Running code](../images/MCXW72_I/Figure11.png)

## Notes

- To rebuild after configuration changes, run west build with `--pristine`
- Build artifacts are in the `build/` directory
- Change board with `-b` parameter and application path as needed

**Parent topic:** [Building the binaries](../topics/building_the_binaries.md)