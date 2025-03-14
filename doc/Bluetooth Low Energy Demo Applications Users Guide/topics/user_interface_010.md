# User interface

After flashing the beacon, the sensor is put in deep sleep \(all LEDs are off\). To flash the board in case the beacon is put in Deep-sleep mode, press the **ADVSW** or **RESET** button. After this step, any attached debugger loses its connection. The default configuration of the application enables low power, which disables LED support. The user can manually change the configuration and enable LED support, otherwise all subsequent LED behavior references are ignored.

By default, the application uses extended advertising. However, it can be configured to use legacy advertising by setting the `gBeaconAE_c` define to `0`. In the legacy configuration, the first press of the advertising switch starts the legacy advertising and the second press stops it.

The table below lists details of the hardware references for the Beacon application.

| Platform                                 | ADVSW | ADVLED   | EXTADVLED |
| ---------------------------------------- | ----- | -------- | --------- |
| KW45B41Z-EVK / K32W148-EVK / MCX-W71-EVK | SW2   | LED2     | LED1      |
| FRDM-MCXW71                              | SW2   | Blue LED | RGB LEDS  |
| KW47-EVK / MCX-W72-EVK                   | SW2   | LED2     | LED1      |
| FRDM-MCXW72                              | SW4   | Blue LED | RGB LEDS  |

**Parent topic:**[Beacon](../topics/beacon.md)

