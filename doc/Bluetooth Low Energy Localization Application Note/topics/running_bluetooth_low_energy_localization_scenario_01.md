# Running a Bluetooth Low Energy Localization Scenario Using the Shell Interface

The following Bluetooth Low Energy localization examples are provisioned by default with a shell command-line interface accessible via a Terminal application such as Tera Term:

-   Localization Reader
-   Localization User Device

After connecting with the Terminal application, pressing the **RST** switch on the board displays the application name.

To display the list of available commands and their description, enter the '`help`' command as shown in [Figure 1](../images/Figure6.png).

![](../images/Figure6.png "Localization Reader application available commands")

To run the application, perform the following steps:

-   Enter the `sb` command on both terminals that are connected to the Localization Reader and Localization User Device applications. The devices connect, perform pairing if not previously bonded, and trigger distance measurement.

![](../images/Figure7.png "Starting the Localization Reader application ")

[Figure 2](../images/Figure7.png) illustrates the start of the Localization Reader application.

![](../images/Figure8.png "Starting the Localization User Device application ")



After the devices are connected, the link is encrypted, the Channel Sounding configuration is created, and distance measurement is triggered. This is shown in [Figure 3](../images/Figure8.png). If two consecutive distance measurements return a value lower or equal to 1 meter, the LED1 on the Localization Reader starts blinking. If the measured distance is higher than 1, LED1 turns off.

-  To trigger a new distance measurement with peer device id `0`, enter "`tdm 0`" command. This command can be executed on either the Localization User Device or the Localization Reader. [Figure 4](../images/Figure5_Localization_User_Device_TriggerDistMeasurement.png) shows an example of triggering the distance measurement from the Localization User Device.

![](../images/Figure5_Localization_User_Device_TriggerDistMeasurement.png "Localization User Device triggering distance measurement")

![](../images/Fig6_Localization_Reader_trigger_distance_measurement.png "Localization Reader trigger distance measurement data")

[Figure 5](../images/Fig6_Localization_Reader_trigger_distance_measurement.png) shows the available distance measurement data.

**Parent topic:**[Running the Bluetooth Low Energy Localization Scenario](../topics/running_bluetooth_low_energy_localization_scenario.md)

