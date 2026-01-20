# HCI Logging and Debug Keys

To facilitate debugging, the Bluetooth LE Host Stack can make use of HCI packet logging and security debug keys. This section describes how to configure and use these features.

## Debug Keys

Debug keys are used during the pairing process to allow developers to decrypt and analyze Bluetooth LE traffic. When debug keys are enabled, the stack uses predefined key material instead of generating random keys, making it possible to monitor and inspect encrypted communications for debugging purposes. To use the Bluetooth LE Core Specification-defined debug keys for pairing, the application must set the `gSecLibUseBleDebugKeys_d` configuration macro to 1 in `app_preinclude.h`. It is sufficient to configure this setting on only one of the two connected devices. Bluetooth LE packet analyzers should be able to correctly decrypt communication.

## HCI Packet Logging (BTSNOOP)

To configure HCI packet logging via the BTSNOOP protocol, the following steps must be performed:

- Enable the BTSNOOP debug framework module in the project configuration file (prj.conf) via west:
  ```
  CONFIG_MCUX_COMPONENT_middleware.wireless.framework.dbg.sbtsnoop=y
  CONFIG_MCUX_COMPONENT_middleware.wireless.framework.dbg.sbtsnoop.port.nxp_ble=y
  ```
- Generate the project.
- Set the `SERIAL_BTSNOOP` configuration macro to 1 in `app_preinclude.h`.
- Refer to [BTSNOOP Module Documentation](https://github.com/nxp-mcuxpresso/mcuxsdk-middleware-connectivity-framework/tree/main/services/DBG/sbtsnoop) for details on how to retrieve logs.