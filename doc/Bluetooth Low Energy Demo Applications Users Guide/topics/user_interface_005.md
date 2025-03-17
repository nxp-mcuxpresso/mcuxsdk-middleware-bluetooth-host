# User interface

After flashing the board, both nodes enter Deep-sleep mode. To flash the board again, press **WAKESW**. The application default configuration enables low power that disables LED support. The user disables low power and enables LED support setting the gAppLowpowerEnabled_d define to 0. To wake up the node, press the **WAKESW** button. Both applications provide guidance over the UART.

Open a serial port terminal using the following settings:

**baud rate 115200, data bits 8, parity none, stop bits 1**.

See Table 1 and Table 2 below for hardware references.

**Hardware references**:
Table 1.  Extended Advertising Peripherals
| Platform                                 | WAKESW | OPTSW | ADVLED   | CONNLED |
| ---------------------------------------- | ------ | ----- | -------- | ------- |
| KW45B41Z-EVK / K32W148-EVK / MCX-W71-EVK | SW3    | SW2   | LED2     | LED1    |
| FRDM-MCXW71                              | SW4    | SW2   | Blue LED | RGB LED |
| KW47-EVK / MCX-W72-EVK                   | SW3    | SW2   | LED2     | LED1    |
| FRDM-MCXW72                              | SW2    | SW4   | Blue LED | RGB LED |

Table 2.  Extended Advertising Central
| Platform                                 | WAKESW | SCANLED  | CONNLED |
| ---------------------------------------- | ------ | -------- | ------- |
| KW45B41Z-EVK / K32W148-EVK / MCX-W71-EVK | SW2    | LED2     | LED1    |
| FRDM-MCXW71                              | SW2    | Blue LED | RGB LED |
| KW47-EVK / MCX-W72-EVK                   | SW3    | LED2     | LED1    |
| FRDM-MCXW72                              | SW4    | Blue LED | RGB LED |

**Parent topic:**[Low-power extended advertising Peripheral and Central](../topics/low-power_extended_advertising_peripheral_and_exte.md)

