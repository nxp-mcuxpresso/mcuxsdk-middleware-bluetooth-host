# User interface

After flashing the board, both nodes enter Low-power mode. In case the sensor is put in deep sleep, press WAKESW or RESET. To flash the board in case the sensor is put in deep sleep, press either **WAKESW** or **RESET** button. By default, the application is configured to be in low power mode, which disables LED support.

The user can manually change this configuration and enable LED support, else all subsequent LED behavior references are ignored and all LEDs are off. The devices disconnect and enter Deep-sleep only if low power is enabled. When the node is awake and communicating, **CONNLED** is on. To wake up the node, press the **WAKESW** button.

See [Table 1](#GUID-C51728A3-DBD3-482A-B25C-686AE9147C40) below for hardware references.

|**Platform**|**WAKESW**|**CONNLED**|
|------------|----------|-----------|
|KW45B41Z-EVK / K32W148-EVK|SW2|LED2|
|FRDM-MCXW71|SW2|Blue LED|
|KW47-EVK/MCXW72-EVK|SW2|LED2|
|FRDM-MCXW72|SW4|Blue LED|

**Parent topic:**[Low-power temperature sensor and collector](../topics/low-power_temperature_sensor_and_collector.md)

