# Running the Bluetooth Low Energy Localization Scenario Using the Button Interface

To run the Bluetooth Low Energy Localization scenario using the button interface perform the following steps:

-   Press **SW2** on both boards to connect the devices. After the devices are connected, the link is encrypted and the Channel Sounding configuration is created, distance measurement is triggered. When two consecutive distance measurements return a value lower or equal to 1, the LED1 on the Localization Reader starts blinking. When the measured distance is higher than 1, LED1 turns off.
-   Press **SW3** on the *loc\_user\_device* board to trigger distance measurement. LED1 blinks or turns off, based on the results of the distance measurement.

**Parent topic:**[Running the Bluetooth Low Energy Localization Scenario](../topics/running_bluetooth_low_energy_localization_scenario.md)

