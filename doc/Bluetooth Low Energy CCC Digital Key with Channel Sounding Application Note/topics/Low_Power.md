# Low-power mode

The CCC Owner Pairing and Passive Entry scenarios (with Channel Sounding support) can also run in Low-power mode. This feature is supported on the following platforms:

-   KW47-EVK
-   KW47-LOC

To enable Low-power mode, set the `gAppLowpowerEnabled_d` macro to `1` in the `app_preinclude.h` file for the **digital_key_car_anchor_cs** and/or the **digital_key_device_cs** projects.

In Low-power mode, the shell interface is not available. Interaction with the demos is done through buttons. The button configuration is the same for all supported platforms.

The below table lists the buttons applicable for the car Anchor.

**Car Anchor buttons**

|**Button**|**Press**      |**Description**         |
|----------|---------------|------------------------|
|   SW2    |(short press)  |Starts Owner Pairing ADV|
|   SW3    |(long press)   |Starts Passive Entry ADV|



The below table lists the buttons applicable for the Device.

**Device buttons**

|**Button**|**Press**      |**Description**|
|----------|---------------|---------------|
|   SW2    |(short press)  |Starts Scanning|

