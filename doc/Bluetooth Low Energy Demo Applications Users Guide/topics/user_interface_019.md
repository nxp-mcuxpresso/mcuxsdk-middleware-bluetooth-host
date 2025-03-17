# User interface

After flashing the board, both nodes enter Low-power mode. In case the sensor is put in deep sleep, press **WAKESW** or **RESET**. To flash the board in case the sensor is put in deep sleep, press either **WAKESW** or **RESET** button. By default, the application is configured to be in low power mode, which disables LED support.

The user can manually change this configuration and enable LED support, else all subsequent LED behavior references are ignored and all LEDs are off. The devices disconnect and enter Deep-sleep only if low power is enabled. When the node is awake and communicating, **CONNLED** is on. To wake up the node, press the **WAKESW** button.

The table below describes the hardware references.

| Platform                                 | WAKESW | CONNLED  |
| ---------------------------------------- | ------ | -------- |
| KW45B41Z-EVK / K32W148-EVK / MCX-W71-EVK | SW2    | LED2     |
| FRDM-MCXW71                              | SW2    | Blue LED |
| KW47-EVK / MCX-W72-EVK                   | SW2    | LED2     |
| FRDM-MCXW72                              | SW4    | Blue LED |

**Parent topic:**[Low-power temperature sensor and collector](../topics/low-power_temperature_sensor_and_collector.md)

